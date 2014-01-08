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


#ifndef	_MUDM_PKT_H_ /* Prevent multiple inclusion */
#define	_MUDM_PKT_H_
#include "common.h"
#include <spi/include/mu/InjFifo.h>
#include <mudm/include/mudm_utils.h>

#include "mudm_macro.h"
#include "mudm_lock.h"
#include "mudm_trace.h"
#include "mudm_wu.h"


struct my_context; 


struct pkt_descriptor { 
  struct pkt_descriptor *next;
  struct pkt_descriptor *prev;
  uint64_t desc_cnt;     /* descriptor count when sent to MU              */
  uint64_t req_id;       /* request ID for status callback                */    
  uint64_t error_val;    /* error_val                                     */
  struct pkt_controls * pkt_control;  /* point back to the controls       */
  char *  pkt_msg; /* point to the packet                         */
  uint64_t message_pa;      /* physical address of the packet              */  
  uint64_t * flightinfo4delay;                                           
  uint64_t timestamp;   
  struct mudm_connection * ccontext;   
}  __attribute__ ((aligned (32)));


struct pkt_controls   {
  uint64_t num_pkts;               /* number of packets=descriptors     */
  Lock_Atomic_t pkt_list_lock;      /* packet polling spinlock          */
  uint64_t packet_size;            /* size of packets under control     */
  struct mudm_memory_region pkt_desc_region;
  struct pkt_descriptor * pkt_desc_list; 
  struct pkt_descriptor * pkt_free_list;
  struct pkt_descriptor * pkt_reqid_list;
  struct mudm_memory_region pkt_list_region;
  char  * packetlist;  
  MUSPI_InjFifo_t * IdToInjFifo; /* using Injection Fifo Pointer */  
  
} ;

void check4notRight(struct pkt_controls * pkt_ctls,BG_FlightRecorderRegistry_t* logregistry);

__INLINE__ void update_IdToInjFifo(struct pkt_controls * pc,  MUSPI_InjFifo_t * using_IdToInjFifo){
     pc->IdToInjFifo = using_IdToInjFifo;
};
int alloc_pkt_list_object( struct my_context * mcontext, uint32_t max_connections , uint64_t packetsize, struct mudm_memory_region * memregion);

__INLINE__ void set_pkt_req_id(struct pkt_descriptor * pktd, uint64_t value){
  pktd->req_id = value;
}
__INLINE__ void set_pkt_error_val(struct pkt_descriptor * pktd, uint64_t value){
  pktd->error_val = value;
}

__INLINE__ void set_pkt_count(struct pkt_descriptor * pktd, uint64_t desc_count){
  pktd->desc_cnt = desc_count;
}

void release_pkt_to_poll(struct pkt_descriptor * pktd, uint64_t desc_count);

void release_pkt_to_free(struct pkt_descriptor * pktd, uint64_t desc_count);

struct pkt_descriptor * alloc_pkt_message(struct pkt_controls * pkt_ctls);


uint64_t poll_pkt_message(struct my_context * mcontext,struct pkt_controls * pkt_ctls);

__INLINE__ void release_pkt_to_free(struct pkt_descriptor * pktd, uint64_t desc_count){
ENTER;  
  pktd->desc_cnt = 0;
  SPIN_LOCK(pktd->pkt_control->pkt_list_lock);
    pktd->next = pktd->pkt_control-> pkt_free_list;
    pktd->pkt_control-> pkt_free_list = pktd;
  SPIN_UNLOCK(pktd->pkt_control->pkt_list_lock);  
EXIT;
}
int32_t flush_pkt_reqid(struct my_context * mcontext,struct pkt_controls * pkt_ctls);

__INLINE__
void release_pkt_to_poll_ccontext(struct pkt_descriptor * pktd, uint64_t desc_count,struct mudm_connection * ccontext){
  MUDM_MU_PKT_2POLL(ccontext->flight_recorder,pktd,ccontext,desc_count,pktd->req_id);
  pktd->ccontext = ccontext;
  if (pktd->req_id){
    release_pkt_to_poll(pktd, desc_count);
    wakeup();
  }
  else{
    release_pkt_to_poll(pktd, desc_count);
  }
};

#endif
