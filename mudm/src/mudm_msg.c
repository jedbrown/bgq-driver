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


//! \file  mudm_cnmf.c
//! \brief Compute node message facility for implementation of communications for IO purposes between compute nodes


#include <spi/include/kernel/MU.h>
#include <spi/include/kernel/memory.h>

#include <mudm/include/mudm.h>
#include "mudm_macro.h"
#include <mudm/include/mudm_inlines.h>
#include <mudm/include/mudm_utils.h>



#include "common.h"
#include "mudm_pkt.h"
#include "rdma_object.h"
#include "common_inlines.h"
//need mudm_connection for SetUpDestination
#include "mudm_connection.h" 


#ifdef __FWEXT__
#include <firmware/include/fwext/fwext.h>
#include <firmware/include/Firmware.h>
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <firmware/include/fwext/fwext_lib.h>

#include <firmware/include/fwext/fwext_nd.h>
#include <firmware/include/fwext/fwext_mu.h>

#endif
#include <hwi/include/bqc/nd_500_dcr.h>
#include <firmware/include/personality.h>
#include <spi/include/mu/Addressing_inlines.h>


int    mudm_send_msg(void* mudm_context , 
                     void* requestID, 
                     uint32_t destination_torus_address,  
                     uint16_t  type,  
                     uint8_t  options,
                     void * payload_paddr, /* physical address of payload */
                     uint16_t payload_length)
{    
    uint64_t           desc_count = -1; 
    int IO_link_choice = 0;  
    MUHWI_Destination_t  destination;         /* remote destination    */
    struct my_context * mcontext = (struct my_context *)mudm_context;
    uint8_t io_port = MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_IO_PORT;
    int direction; /* IONODE2CN, CN2ION, CN2CN, ION2ION */
    
        /* Injection Memory FIFO Descriptor on local stack */
    // Nneed to templatize ...... with base copy in mudm_connect block.......
    __attribute((aligned(64))) MUHWI_Descriptor_t mu_iMemoryFifoDescriptor;

    SoftwareBytes_t  SoftwareBytes;
    memcpy(&SoftwareBytes.ionet_pkt.torus_source_node, &mcontext->myTorusAddress.Destination,sizeof(uint32_t));
    SoftwareBytes.ionet_pkt.torus_dest_node = destination_torus_address;
    SoftwareBytes.ionet_pkt.payload_length = payload_length;
    SoftwareBytes.ionet_pkt.type=type;
    SoftwareBytes.ionet_pkt.options = options;
    SoftwareBytes.ionet_pkt.port_info = io_port;
    SoftwareBytes.ionet_pkt.reserved = -1;

    switch(options){
      case MUDM_IOLINK_PORT6:
      case MUDM_IOLINK_PORT7:
        IO_link_choice = 1;  //use other inject and receive queue if not port 10 
        direction=IONODE2CN;
        io_port=options;
      break;  
      case MUDM_IOLINK_PORT10:  
        direction=IONODE2CN;
        io_port=options;
      break;
      //IOLINK_ALL_PORTS defaults to the default port initially; need to send copies on the other ports per options
      case MUDM_IOLINK_ALL_PORTS:
      case MUDM_USE_DEFAULT_IO_LINK:
        direction=IONODE2CN;
        io_port = mcontext->my_using_IO_port;
        break;
      case MUDM_CNB2IONODE:
        direction=CN2IONODE;
        io_port = mcontext->my_using_IO_port;
        break;
      case MUDM_NO_IOLINK:
      default: 
        direction=NODE2NODE;
    }

    if (payload_length > TORUS_MAX_PAYLOAD_SIZE) return -EINVAL; /*exceeded memfifo packet size */

    //! \todo  Need to sort out IONDDE2IONODE or CN2CN
    if (CN2IONODE==direction) {/* need bridging CN which has IO link */
       destination= mcontext->mybridgingCNnode; 
    }
    else/* Use actual Destination value of CN */
    {
      memcpy(&destination.Destination,&destination_torus_address,sizeof(uint32_t));
    }

    InitPt2PtMemoryFIFODescriptor( &mu_iMemoryFifoDescriptor,
                                   &SoftwareBytes,
                                  (uint64_t)payload_paddr,
                                  (uint64_t)payload_length, 
                                  destination,
                                  direction, 
                                  io_port,
                                  mcontext->sys_rec_fifo_id[IO_link_choice] );
    
    desc_count = InjFifoInject (&mcontext->injfifo_ctls[IO_link_choice], &mu_iMemoryFifoDescriptor,&mcontext->mudm_hi_wrap_flight_recorder);
    // ensure completion so that physical memory can be released
    while ( MUSPI_CheckDescComplete(mcontext->injfifo_ctls[IO_link_choice].injfifo, desc_count) == 0);
    //! \todo: TODO: req_id handling 
    return 0;
}
EXPORT_SYMBOL(mudm_send_msg);


int mudm_rdma_read_on_torus(       void* mudm_context ,
                                   MUHWI_Destination_t torus_destination, //remote torus location
                                   uint64_t bytes_xfer, 
                                   void * counter_paddr,                                 
                                   void * local_paddr, 
                                   void * remote_paddr){
  uint64_t desc_count = (uint64_t)-1;
  struct my_context * mcontext=(struct my_context *)mudm_context;

  MUHWI_Descriptor_t *muspi_iDirectPutDescriptor=NULL;
  struct mudm_rdma_object * rdma_obj_item=NULL;
  struct pkt_descriptor * pktd = NULL;  /* for directput handling */

  if (mcontext->magic != MUDM_MAGIC){
    
    MPRINT("-EFAULT:  Bad Magic for mudm_context=%p TERMINATING FUNCTION %s \n",mudm_context, __FUNCTION__);
    
    return -EFAULT;
  }

  pktd = alloc_pkt_message(mcontext->smallpa_obj_ctls[0]);
  if (pktd == NULL){
     MPRINT("pktd==NULL %s line %d \n",__FUNCTION__,__LINE__);
     EXIT;
     return -EBUSY;
  }

  muspi_iDirectPutDescriptor = (MUHWI_Descriptor_t * )pktd->pkt_msg;
  
  //synchronous operation handled by MUDM if counter physical address is NULL
  if (counter_paddr==NULL){
     rdma_obj_item = allocate_rdma_object(mcontext->rdma_obj_ctls[0]);
     if (NULL==rdma_obj_item){
      release_pkt_to_free(pktd,0);
      MPRINT("rdma_obj_item is NULL \n");
      EXIT;
      return -EBUSY;
     }
     
     rdma_obj_item->mue-> mu_counter = bytes_xfer;
     rdma_obj_item->mue-> error = 0;
     initPt2PtDirectPutDescriptor( muspi_iDirectPutDescriptor,
                                   (uint64_t) remote_paddr,
                                   bytes_xfer,
                                   mcontext->myTorusAddress,NODE2NODE,
                                   0,
                                   rdma_obj_item->rdma_object_pa,
                                   (uint64_t)local_paddr);

     
     //MDUMPHEXSTR("packaging directput descriptor",muspi_iDirectPutDescriptor,BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES); 
     
     desc_count = InjFifoInjectMemFifo_withDest( &mcontext->injfifo_ctls[0], 
                                             &mcontext->mu_iMemoryFifoDescriptor,
                                             torus_destination,
                                             MUDM_DPUT,
                                             pktd->message_pa, 
                                             (uint64_t)(sizeof(MUHWI_Descriptor_t)), //sending directput to remote
                                             &mcontext->rget_memfifo_sent,
                                             &mcontext->mudm_hi_wrap_flight_recorder);
     //MPRINT("rdma_obj_item->mu_counter=%llu\n",(LLUS)rdma_obj_item->mu_counter);
     if (mcontext->poll_active==0){ 
       do{
       }while  (rdma_obj_item->mue->mu_counter != 0);     
       release_pkt_to_free(pktd,0);
       free_rdma_obj(rdma_obj_item);
     }
     else{
      release_pkt_to_free(pktd,0);
      release_rdma_obj_to_poll(rdma_obj_item, COUNTER);
      return -EFAULT; //problem when blocking within poller
     }
  }
  //checking counter is the user responsibility
  else{
    initPt2PtDirectPutDescriptor( muspi_iDirectPutDescriptor,
                                   (uint64_t)remote_paddr,
                                   bytes_xfer,
                                   mcontext->myTorusAddress,NODE2NODE,
                                   0,
                                   (uint64_t)counter_paddr,
                                   (uint64_t)local_paddr);
    if (torus_destination.Destination.Destination != mcontext->myTorusAddress.Destination.Destination){ 
      desc_count = InjFifoInjectMemFifo_withDest( &mcontext->injfifo_ctls[0], 
                                             &mcontext->mu_iMemoryFifoDescriptor,
                                             torus_destination,
                                             MUDM_DPUT,
                                             pktd->message_pa, 
                                             (uint64_t)(sizeof(MUHWI_Descriptor_t)), //sending directput to remote
                                             &mcontext->rget_memfifo_sent,
                                             &mcontext->mudm_hi_wrap_flight_recorder);
      while( MUSPI_CheckDescComplete(mcontext->injfifo_ctls[0].injfifo, desc_count) == 0)
      {
      }
      release_pkt_to_free(pktd,0);

    }
    else {//local memcopy--handle it now
      desc_count = InjFifoInject_rget_emulation (&mcontext->injfifo_ctls[0],&mcontext->mu_iMemoryFifoDescriptor, &mcontext->mudm_hi_wrap_flight_recorder);
      release_pkt_to_free(pktd,0);
    }
  }  


  
return 0;
};

int mudm_rdma_write_on_torus(      void* mudm_context ,
                                   MUHWI_Destination_t torus_destination, //remote torus location
                                   uint64_t bytes_xfer, 
                                   void * counter_paddr,                                 
                                   void * local_paddr, 
                                   void * remote_paddr){
  uint64_t desc_count = (uint64_t)-1;
  struct my_context * mcontext=(struct my_context *)mudm_context;
  uint64_t phys_counter_addr = (uint64_t)counter_paddr;
  MUHWI_Descriptor_t muspi_iDirectPut;
  MUHWI_Descriptor_t *muspi_iDirectPutDescriptor=&muspi_iDirectPut;
  if (mcontext->magic != MUDM_MAGIC){    
    MPRINT("-EFAULT:  Bad Magic for mudm_context=%p TERMINATING FUNCTION %s \n",mudm_context, __FUNCTION__); 
    return -EFAULT;
  }

  //If counter_paddr is NULL, use a remote dummy physical counter
  //Set counter to use MUDM base objectaddress + offset to dummy counter  
  //Note:  cheating since physical address of mudm_context is the same on every CNK
  if (counter_paddr==NULL){
     phys_counter_addr = mcontext->mudm_context_phys_addr + offsetof(struct my_context,dummy_counter);
  }
  initPt2PtDirectPutDescriptor( muspi_iDirectPutDescriptor,
                                   (uint64_t)local_paddr,
                                   bytes_xfer,
                                   torus_destination,
                                   NODE2NODE,
                                   0,
                                   (uint64_t)phys_counter_addr,
                                   (uint64_t)remote_paddr);
  
  desc_count= InjFifoInjectDirectPut(&mcontext->injfifo_ctls[0], 
					    muspi_iDirectPutDescriptor,
                                            bytes_xfer,
                                             (uint64_t)local_paddr, //source physical address 
                                            (uint64_t)remote_paddr,//destination physical address
                                            phys_counter_addr,
                                            &mcontext->mudm_hi_wrap_flight_recorder);
 
  return 0;
};
