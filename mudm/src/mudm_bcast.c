/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (c) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/*                                                                  */
/* US Government Users Restricted Rights - Use, duplication or      */
/* disclosure restricted by GSA ADP Schedule Contract with IBM      */
/* Corporation.                                                     */
/*                                                                  */
/* This software is available to you under either the GNU General   */
/* Public License (GPL) version 2 or the Eclipse Public License     */
/* (EPL) at your discretion.                                        */
/*                                                                  */

//! \file  mudm_bcast.c
//! \brief Implementation of special programming interfaces for broadcasts
//! \remarks primarily intended for CNK broadcast methods for supporting CNK job controls


#include <mudm/include/mudm_bcast.h>
#include "common.h"
#include "common_inlines.h"
#include "mudm_pkt.h"
#include "rdma_object.h"
int mudm_bcast_memfifo(void* mudm_context,
                          void* requestID, 
                          uint16_t type, 
                          void * payload_paddr,
                          uint16_t payload_length,
                          uint8_t  class_route)
{
  uint64_t           desc_count = -1; 
  struct my_context * mcontext = (struct my_context *)mudm_context;
  ENTER;
  PRINT("payload_paddr=%p payload_length=%llu class_route=%d \n",(void*)payload_paddr,(LLUS)payload_length, class_route);
  desc_count = InjFifoInjectMemFifoBcast (mcontext->injfifo_ctls,
                                         &mcontext->system_bcast.mu_iMemoryFifoDescriptor,
                                         type,
                                         (uint64_t) payload_paddr,
                                         (uint64_t) payload_length, 
                                         class_route,
                                         &mcontext->system_bcast.send_count,
                                         &mcontext->mudm_hi_wrap_flight_recorder);

  while ( MUSPI_CheckDescComplete(mcontext->system_bcast.IdToInjFifo, desc_count) == 0)
  {
  }
  EXIT;
  return 0;
}

int mudm_bcast_reduce(void* mudm_context,
                          uint16_t type,  
                          uint8_t op_code, 
                          void * payload_paddr,
                          uint16_t payload_length,
                          MUHWI_Destination_t torus_destination,
                          uint8_t  class_route,
                          uint8_t  reduce_choice,
                          void* result_vaddr
    )
{  
  uint64_t bytesLength         = payload_length;
  unsigned int numNonZeroBits = popcnt64 (bytesLength);
  MUHWI_Destination_t dest;
  uint64_t           desc_count = -1; 
  struct my_context * mcontext = (struct my_context *)mudm_context;
  dest = torus_destination;
  ENTER;
  if (numNonZeroBits>1){
      MPRINT("payload length must be 4*2**n for 4 to 512 \n");
      return -EINVAL;
  }
  if (payload_length < 4){
    MPRINT("payload length must be 4*2**n for 4 to 512 \n");
    return -EINVAL;
  }

  if (MUHWI_COLLECTIVE_TYPE_ALLREDUCE == reduce_choice);
  else if (MUHWI_COLLECTIVE_TYPE_REDUCE == reduce_choice);
  else {
    MPRINT("reduce choice must be  MUHWI_COLLECTIVE_TYPE_ALLREDUCE or MUHWI_COLLECTIVE_TYPE_REDUCE \n");
    return -EINVAL;
  }
  
  if(result_vaddr)
  {
      mcontext->bcast_reduceall_data     = result_vaddr;
      mcontext->bcast_reduceall_datasize = payload_length;
      mcontext->bcast_reduceall_complete = 0;
      ppc_msync();
  }
  
  desc_count = InjFifoInjectReduce (mcontext->system_bcast.IdToInjFifo,
                                         &mcontext->system_bcast.mu_iMemoryFifoDescriptor,
                                         type,
                                         (uint64_t) payload_paddr,
                                         (uint64_t) payload_length, 
                                         op_code,
                                         dest,
                                         mcontext->system_bcast.inj_fifo_lock,
                                         reduce_choice,
                                         class_route,
                                         &mcontext->system_bcast.send_count,
                                         &mcontext->mudm_hi_wrap_flight_recorder); 
  while ( MUSPI_CheckDescComplete(mcontext->system_bcast.IdToInjFifo, desc_count) == 0)
  {
  }
  
  if(result_vaddr)
  {
      while(mcontext->bcast_reduceall_complete == 0)
      {
      }
      ppc_msync();
  }
  
  EXIT;
  return 0;
};
/*
struct mudm_rdma_bcast {  
   void* requestID; // call status callback request ID    
   void * dest_payload_paddr;
   void * source_payload_paddr;
   uint64_t payload_length;
   uint32_t num_in_class_route;
   uint8_t class_route;
   uint8_t reserved;
};  
*/

int mudm_bcast_RDMA_write(void* mudm_context, struct mudm_rdma_bcast * mrb)
{  
  struct my_context * mcontext = (struct my_context *)mudm_context;
  struct mudm_bcast *mb = &mcontext->system_bcast;
  struct bcast_rdma_object * bro = &mb->RDMA_bcast_control;
  uint64_t payload_physAddr = mcontext->system_bcast.RDMA_bcast_control.bcast_rdma_object_pa + offsetof(struct bcast_rdma_object,bcast_counter_info);
  
  ENTER;
  PRINT("payload_physAddr =%p &bro->bcast_counter_info.mrb=%p \n",(void *)payload_physAddr, &bro->bcast_counter_info.mrb);
  PRINT("mrb->requestID=%p \n",mrb->requestID);
  lock(mb->inj_fifo_lock);
  if (bro->state){
    unlock(mb->inj_fifo_lock);
    return -EBUSY;
  }  
  bro->state=BCAST_SETUP;
  unlock(mb->inj_fifo_lock);
  MUDM_LOG_RDMA_WRITE_BCAST(&mcontext->mudm_hi_wrap_flight_recorder,mrb->requestID,mrb->dest_payload_paddr,mrb->source_payload_paddr,mrb->payload_length);
  memcpy( &bro->bcast_counter_info.mrb, mrb, sizeof(struct mudm_rdma_bcast));

  PRINT("bro->mu_counter_bcast_addr=%p \n",bro->mu_counter_bcast_addr);
  bro->mu_counter_bcast_addr->mu_counter = mrb->payload_length;// counter target for directput descriptors--polled until 0 on all nodes
  HERE;
  bro->bcast_counter_info.origination_node = mcontext->myTorusAddress;
  HERE;
  mudm_bcast_memfifo( (void* )mcontext,
                       NULL /*requestID*/,
                       MUDM_RDY_RDMA_BCAST,
                      (void *)(payload_physAddr), 
                       sizeof(struct rdma_bcast_set_counter), //payload_length
                       mrb->class_route);
  
  return -EINPROGRESS;
  MEXIT;
}


void count_CN_in_block(struct my_context * mcontext){

   MUHWI_Destination_t dest;
   dest.Destination.Destination = 0;
   mcontext->num_compute_nodes = 1;
   mudm_bcast_reduce((void* )mcontext, MUDM_REDUCE_ALL_INT,  
                          MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_ADD /* op_code */, 
                          (void *)(mcontext->mudm_context_phys_addr + offsetof(struct my_context,num_compute_nodes)), sizeof(mcontext->num_compute_nodes) /*payload_length*/,
                          dest,
                          15 /*class_route*/,
                          MUHWI_COLLECTIVE_TYPE_ALLREDUCE, NULL );

}


void largest_node_corner_in_block(struct my_context * mcontext){
   MUHWI_Destination_t dest;
   dest.Destination.Destination = 0;
   mcontext->largest_node_corner = mcontext->myTorusAddress.Destination.Destination;
   //MPRINT("mcontext->largest_node_corner=%llx \n",(LLUS)mcontext->largest_node_corner);
   mudm_bcast_reduce((void* )mcontext, MUDM_REDUCE_ALL_LNC,  
                          MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_MAX /* op_code */, 
                          (void *)(mcontext->mudm_context_phys_addr + offsetof(struct my_context,largest_node_corner)), sizeof(mcontext->largest_node_corner) /*payload_length*/,
                          dest,
                          15 /*class_route*/,
                          MUHWI_COLLECTIVE_TYPE_ALLREDUCE, NULL );

}


