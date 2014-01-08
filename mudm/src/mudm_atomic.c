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

//! \file  mudm_atomic.c
//! \brief Implementation of special programming interfaces for broadcasts
//! \remarks primarily intended for CNK broadcast methods for supporting CNK job controls

#include "common.h"
#include "common_inlines.h"
 

int mudm_remoteget_load_atomic(void* mudm_context,  struct  remoteget_atomic_controls * atc){

  struct my_context * mcontext = (struct my_context *)mudm_context;
  uint64_t payload_phys_addr = atc->remote_paddr; //manipulate a local copy to turn into atomic
  ENTER;
  payload_phys_addr |= MUHWI_ATOMIC_ADDRESS_INDICATOR | (atc-> atomic_op & 7); //atomic indicator on and atomic op is last 3 bits
  atc->local_counter = 8; // counter value for atomics is 8
  DUMPHEXSTR("scratch area before initPt2PtDirectPutDescriptor", atc->scratch_area,sizeof(struct  remoteget_atomic_controls) );
  initPt2PtDirectPutDescriptor( (MUHWI_Descriptor_t *)atc->scratch_area,  //build remote directput descriptor in the scratch 
                                               payload_phys_addr,         //payload_phys_addr (remote side is the payload)
                                               8,                         // length of atomic transfer is 8B
                                               mcontext->myTorusAddress,  //destination from remote is to here torus address
                                               CN2CN,                     // direction 
                                               0,                         //Ignore  io_port
                                               atc->paddr_here + offsetof(struct remoteget_atomic_controls,local_counter),    //counter target for remoteget directput 
                                               atc->paddr_here + offsetof(struct remoteget_atomic_controls,returned_val)      //uint64_t Rec_Payload_Offset
                                               );
  DUMPHEXSTR("scratch area after initPt2PtDirectPutDescriptor", atc->scratch_area,sizeof(struct  remoteget_atomic_controls) );
  PRINT("atc->scratch_area=%p atc->paddr_here=%p \n",atc->scratch_area,(void *)atc->paddr_here);
  if (atc->torus_destination.Destination.Destination != mcontext->myTorusAddress.Destination.Destination){
    mudm_send_msg(mudm_context, NULL, atc->torus_destination.Destination.Destination, MUDM_DPUT, MUDM_NO_IOLINK, (void *)(atc->paddr_here), 64);
  }
  else{
    InjFifoInject_rget_emulation (&mcontext->injfifo_ctls[0],(MUHWI_Descriptor_t *)atc->scratch_area, &mcontext->mudm_hi_wrap_flight_recorder);
  }
  EXIT;
  return 0;
}


int mudm_directput_store_atomic(void* mudm_context,  struct  directput_atomic_controls * atc){
  uint64_t           desc_count = -1; 
  struct my_context * mcontext = (struct my_context *)mudm_context;
  uint64_t source_physaddr = atc-> paddr_here + offsetof(struct  directput_atomic_controls,source_val);
  uint64_t dest_physaddr = atc->remote_paddr;
  ENTER;
  //source_physaddr |= MUHWI_ATOMIC_ADDRESS_INDICATOR | (atc-> atomic_op & 7); //atomic indicator on and atomic op is last 3 bits
  dest_physaddr |= MUHWI_ATOMIC_ADDRESS_INDICATOR | (atc-> atomic_op & 7); //atomic indicator on and atomic op is last 3 bits
  initPt2PtDirectPutDescriptor( (MUHWI_Descriptor_t *)atc->scratch_area,  //build remote directput descriptor in the scratch 
                                               source_physaddr,           //payload_phys_addr plus atomic store operation
                                               8,                         // length of atomic transfer is 8B
                                               atc->torus_destination,    //destination torus coordinates
                                               CN2CN,                     // direction 
                                               0,                         //Ignore  io_port
                                               atc->remote_counter_physAddr,       //counter target for directput 
                                               dest_physaddr              //uint64_t Rec_Payload_Offset
                                               );
  desc_count = InjFifoInject (&mcontext->injfifo_ctls[0],(void *)atc->scratch_area, &mcontext->mudm_hi_wrap_flight_recorder);
  EXIT;
  return 0;
}

void run_atomic_tests(struct my_context * mcontext){
  // Must be CNK!!! not valid for IO node
  int i;
  ENTER;
  // this test has this node as source and destination for stand-alone testing
  for (i=0;i<8;i++)   mcontext->rgatc.scratch_area[i]=0;
  mcontext->rgatc.returned_val = (uint64_t)-1;
  mcontext->rgatc.test1_reserved = 1;
  mcontext->rgatc.test2_reserved =  (uint64_t)-1;
  mcontext->rgatc.paddr_here = mcontext-> mudm_context_phys_addr + offsetof( struct my_context, rgatc);
  mcontext->rgatc.remote_paddr = mcontext-> mudm_context_phys_addr + offsetof( struct my_context, rgatc) + offsetof(struct remoteget_atomic_controls, test1_reserved);
  mcontext->rgatc.atomic_op = MUHWI_ATOMIC_OPCODE_LOAD_INCREMENT;  
  mcontext->rgatc.torus_destination = mcontext->myTorusAddress;  //destination from remote is to here torus address
  mcontext->rgatc.local_counter=8;
  mudm_remoteget_load_atomic( (void*) mcontext,  &mcontext->rgatc);

  EXIT;
};


