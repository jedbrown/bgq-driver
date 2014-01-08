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

//! \file  mudm_poll_bcast.c 
//! \brief Implementation of poll-side processing for broadcast implementations
//! \remarks primarily intended for OFED or CN verbs implementers over torus

#include <spi/include/kernel/MU.h>
#include <spi/include/kernel/memory.h>

#include <mudm/include/mudm.h>
#include <mudm/include/mudm_bcast.h>
#include <mudm/include/mudm_inlines.h>
#include "mudm_macro.h"
#include <mudm/include/mudm_inlines.h>
#include <mudm/include/mudm_utils.h>



#include "common.h"
#include "mudm_pkt.h"
#include "rdma_object.h"

#include "common_inlines.h"
#include "mudm_connection.h"

#include <hwi/include/bqc/nd_500_dcr.h>
#include <firmware/include/personality.h>
#include <spi/include/mu/Addressing_inlines.h>



uint64_t InjFifoBcastDirectPut (MUSPI_InjFifo_t   * injfifo, 
					    MUHWI_Descriptor_t * desc, 
                                            uint64_t            bytes,
                                            uint64_t  source_physaddr, 
                                            uint64_t  dest_physaddr,
                                            uint64_t  Rec_Counter_Offset,
                                            Lock_Atomic_t   * inj_fifo_lock,
                                            uint8_t  class_route,
                                            BG_FlightRecorderRegistry_t * logregistry) 
{   
    MUHWI_Descriptor_t * descPtr;
    void * descPtrTemp;
    uint64_t desc_count;
    ENTER;
    lock(inj_fifo_lock);
    //MPRINT("bytes=%llu source_physaddr=%p dest_physaddr=%p Rec_Counter_Offset=%p \n",(LLUS)bytes, (void *)source_physaddr,(void *)dest_physaddr,(void *)Rec_Counter_Offset);
    do {
       desc_count = MUSPI_InjFifoNextDesc (injfifo, (void **)&descPtrTemp);
    }while (desc_count == -1);
        descPtr = (MUHWI_Descriptor_t  * )descPtrTemp;
    memcpy (descPtr, desc, BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    set_dma_directput(descPtr, bytes,source_physaddr, dest_physaddr);
    set_counter_directput(descPtr, Rec_Counter_Offset);
    descPtr->PacketHeader.NetworkHeader.collective.Byte2.Byte2 = class_route << 4; //set the (system) class route
    //$$$
    DUMPHEXSTR("directput inj descriptor",descPtr,BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    LOG_DESCR_HEADER(logregistry,descPtr);
    //! \todo TODO change name here to INJECT_BCASTDPUT
    INJ_RDMA_BCAST_DPUT(logregistry,&descPtr->PacketHeader);
    my_mbar();//no need for mbar once special TLB bit U1=1
    MUSPI_InjFifoAdvanceDesc (injfifo);
    unlock(inj_fifo_lock);
    EXIT;
    return desc_count;
}


int clear_rdma_bcast_receive_atomic(struct my_context * mcontext, MUHWI_PacketHeader_t * hdr, uint32_t bytes) 
{
  struct mudm_bcast *mb = &mcontext->system_bcast;
  struct bcast_rdma_object * bro = &mb->RDMA_bcast_control;
  struct rdma_bcast_set_counter * rbsc = (struct rdma_bcast_set_counter *)(((char *)hdr)+32);
  void * ref_req_id = 0;
  uint32_t error_val = 0;
  uint32_t error_return;
  ENTER;
  ref_req_id = rbsc->mrb.requestID;
  rbsc->mrb.requestID = NULL;
  bro->state=BCAST_INACTIVE;
  HERE;
  if (ref_req_id){
     HERE;
    MPRINT("ref_req_id=%p \n", ref_req_id);
    (mcontext->status)(  (void*)&ref_req_id, &error_val,mcontext->callback_context,&error_return,1);
  }

  //MDUMPHEXSTR("scratch_area",mcontext->scratch_area,64);
  //MDUMPHEXSTR("scratch_area2",mcontext->scratch_area2,64);
  EXIT;
  return 0;
}

int clear_rdma_bcast_receive_reduce_all(struct my_context * mcontext, MUHWI_PacketHeader_t * hdr, uint32_t bytes) 
{
    uint64_t* value = (uint64_t *)(((char *)hdr)+32);
    memcpy(mcontext->bcast_reduceall_data, value, mcontext->bcast_reduceall_datasize);
    struct mudm_bcast *mb = &mcontext->system_bcast;
    struct bcast_rdma_object * bro = &mb->RDMA_bcast_control;
    struct rdma_bcast_set_counter * rbsc = &bro->bcast_counter_info;
    
    ENTER;
    rbsc->mrb.requestID = NULL;
    bro->state=BCAST_INACTIVE;
    HERE;
    mcontext->bcast_reduceall_complete = 1;
    EXIT;
    return 0;
}

int clear_rdma_bcast_receive_reduce(struct my_context * mcontext, MUHWI_PacketHeader_t * hdr, uint32_t bytes) 
{
  struct mudm_bcast *mb = &mcontext->system_bcast;
  struct bcast_rdma_object * bro = &mb->RDMA_bcast_control;
  struct rdma_bcast_set_counter * rbsc = &bro->bcast_counter_info;
  void * ref_req_id = 0;
  uint32_t error_val = 0;
  uint32_t error_return;

  ENTER;
  ref_req_id = rbsc->mrb.requestID;
  rbsc->mrb.requestID = NULL;
  bro->state=BCAST_INACTIVE;
  HERE;
  if (ref_req_id){
     MHERE;
     (mcontext->status)(  (void*)&ref_req_id, &error_val,mcontext->callback_context,&error_return,1);
  }

  EXIT;
  return 0;
}

// alternative to ready_rdma_bcast_receive_atomic
int ready_rdma_bcast_receive_reduce(struct my_context * mcontext, MUHWI_PacketHeader_t * hdr, uint32_t bytes) 
{
  struct mudm_bcast *mb = &mcontext->system_bcast;
  struct bcast_rdma_object * bro = &mb->RDMA_bcast_control;
  struct rdma_bcast_set_counter * rbsc = (struct rdma_bcast_set_counter *)(((char *)hdr)+32);
  
  ENTER;
  // establish the counter value for the receiving of the RDMA bcast
  bro->mu_counter_bcast_addr->mu_counter = rbsc->mrb.payload_length;// counter target for directput descriptors--polled until 0 on all nodes
  PRINT("mcontext->system_bcast.RDMA_bcast_control.mu_counter_bcast_addr->mu_counter = %llx \n",(LLUS)mcontext->system_bcast.RDMA_bcast_control.mu_counter_bcast_addr->mu_counter);
  bro->origin_bcast_node = rbsc->origination_node;
  bro->class_route4bcast = rbsc->mrb.class_route;
  bro->local_status_address = rbsc->mrb.status_mem;
  bro->local_status_value   = rbsc->mrb.status_val;

  //! \todo  TODO Determine how to handle locking and errors--for now, assume all is good


  if (rbsc->mrb.payload_length){
   //Send reduce to originator which will trigger doing the directput broadcast....
    mcontext->system_bcast.RDMA_bcast_control.num_compute_nodes_in_class_route = 1;
    mudm_bcast_reduce((void* )mcontext, MUDM_REDUCE_BCAST_ORIGIN,  
                          MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_ADD /* op_code */, 
                          (void *)(mcontext->system_bcast.RDMA_bcast_control.bcast_rdma_object_pa 
                              + offsetof(struct bcast_rdma_object,num_compute_nodes_in_class_route)), 
                          sizeof(mcontext->system_bcast.RDMA_bcast_control.num_compute_nodes_in_class_route) /*payload_length*/,
                          mcontext->system_bcast.RDMA_bcast_control.origin_bcast_node,
                          mcontext->system_bcast.RDMA_bcast_control.class_route4bcast ,
                          MUHWI_COLLECTIVE_TYPE_REDUCE, 
                          NULL );
  }
  bro->state=BCAST_RECEIVER;
  EXIT;
  return 0;
}


int mudm_recv_bcast (struct my_context * mcontext, MUHWI_PacketHeader_t * hdr, uint32_t bytes) 
{
 
  SoftwareBytes_t *sw_hdr = (SoftwareBytes_t *)(&hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Put_Offset_LSB);
  char * message = (char *)sw_hdr;
  MUHWI_Descriptor_t * desc =  &mcontext->system_bcast.RDMA_bcast_control.mu_iDirectPutDescriptor;
  ENTER;
  RCV_BCAST_DESCR(&mcontext->mudm_hi_wrap_flight_recorder ,hdr);

  if (0==hdr->NetworkHeader.pt2pt.Byte2.Interrupt)  //oh no, not set--need to fix
  {
   MDUMPHEXSTR("No interrupt bit in this mudm_recv_packet ",hdr, 32);
   MHERE;
  }

  switch(sw_hdr->ionet_hdr.type)
  {
     MPRINT("sw_hdr->ionet_hdr.type=%llx \n",(LLUS)sw_hdr->ionet_hdr.type);
     case MUDM_RDY_RDMA_BCAST:
         //MDUMPHEXSTR("MUDM_RDY_RDMA_BCAST",((char *)hdr)+32,128);
         ready_rdma_bcast_receive_reduce(mcontext, hdr, bytes); 
     break;
     case MUDM_CLR_RDMA_BCAST:
         clear_rdma_bcast_receive_atomic(mcontext, hdr, bytes);  
     break;
      case MUDM_REDUCE_ALL:
          clear_rdma_bcast_receive_reduce_all(mcontext, hdr, bytes);
          break;
     case MUDM_REDUCE_BCAST_ORIGIN:
       //start broadcasts
        HERE;
       if (mcontext->system_bcast.RDMA_bcast_control.bcast_counter_info.mrb.payload_length)
                     InjFifoBcastDirectPut (mcontext->system_bcast.IdToInjFifo, 
					    desc, 
                                            mcontext->system_bcast.RDMA_bcast_control.bcast_counter_info.mrb.payload_length,
                                            (uint64_t)mcontext->system_bcast.RDMA_bcast_control.bcast_counter_info.mrb.source_payload_paddr, 
                                            (uint64_t) mcontext->system_bcast.RDMA_bcast_control.bcast_counter_info.mrb.dest_payload_paddr,
                                            mcontext->system_bcast.RDMA_bcast_control.bcast_counter_info.mu_counter_offset,
                                            mcontext->system_bcast.inj_fifo_lock,
                                            mcontext->system_bcast.RDMA_bcast_control.bcast_counter_info.mrb.class_route,
                                            &mcontext->mudm_hi_wrap_flight_recorder);
     break;
     case MUDM_REDUCE_BCAST_FINISH:
        HERE;
        clear_rdma_bcast_receive_reduce(mcontext, hdr, bytes); 
     break;

     default: /*call default */
        MHERE;
        (mcontext-> recv)( message,mcontext->callback_context);
  }
  EXIT;
  return 0;
}

int rdma_bcast_poll_reduce(struct my_context * mcontext){
    struct mudm_bcast *mb = &mcontext->system_bcast;
    struct bcast_rdma_object * bro = &mb->RDMA_bcast_control;
    if (mcontext->system_bcast.RDMA_bcast_control.mu_counter_bcast_addr->mu_counter){
       //MPRINT("mu_counter=%llu \n ", (LLUS)mcontext->system_bcast.RDMA_bcast_control.mu_counter_bcast_addr->mu_counter);
       return -EINPROGRESS;
    }
    if (mcontext->system_bcast.RDMA_bcast_control.state!=BCAST_RECEIVER)return -EINPROGRESS;
    // if status to post to user, do so 
    MUDM_LOG_RDMA_C0_BCAST(&mcontext->mudm_hi_wrap_flight_recorder,bro->origin_bcast_node.Destination.Destination,bro->class_route4bcast,bro->local_status_address,bro->local_status_value); 
    if (mcontext->system_bcast.RDMA_bcast_control.local_status_address){
      uint64_t * stat_addr = (uint64_t *)mcontext->system_bcast.RDMA_bcast_control.local_status_address;
      *stat_addr = mcontext->system_bcast.RDMA_bcast_control.local_status_value;
    }
    ENTER;
      //Send reduce all to finish broadcast operation
    mcontext->system_bcast.RDMA_bcast_control.num_compute_nodes_in_class_route = 1;
    mudm_bcast_reduce((void* )mcontext, MUDM_REDUCE_BCAST_FINISH,  
                          MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_ADD /* op_code */, 
                          (void *)(mcontext->system_bcast.RDMA_bcast_control.bcast_rdma_object_pa 
                              + offsetof(struct bcast_rdma_object,num_compute_nodes_in_class_route)), 
                          sizeof(mcontext->system_bcast.RDMA_bcast_control.num_compute_nodes_in_class_route) /*payload_length*/,
                          mcontext->system_bcast.RDMA_bcast_control.origin_bcast_node,
                          mcontext->system_bcast.RDMA_bcast_control.class_route4bcast ,
                          MUHWI_COLLECTIVE_TYPE_ALLREDUCE, 
                          NULL );
    mcontext->system_bcast.RDMA_bcast_control.state = BCAST_WAIT4CLEAR;
    EXIT;
    return 0;
}

