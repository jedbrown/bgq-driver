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

#include "mudm_pkt.h"
#include "common_inlines.h"
#include "mudm_ras.h"

void release_pkt_to_poll(struct pkt_descriptor * pktd, uint64_t desc_count){
ENTER;
  pktd->desc_cnt = desc_count;
  pktd->timestamp = GetTimeBase2(); 
  //PRINT("reg_id = %p \n", (void *)pktd->req_id);
  SPIN_LOCK(pktd->pkt_control->pkt_list_lock);
  pktd->prev = NULL;
  pktd->next = pktd->pkt_control-> pkt_reqid_list;
  if (pktd->pkt_control-> pkt_reqid_list != NULL) pktd->pkt_control-> pkt_reqid_list->prev=pktd;
  pktd->pkt_control->  pkt_reqid_list = pktd; 
  SPIN_UNLOCK(pktd->pkt_control->pkt_list_lock); 
EXIT;
};


struct pkt_descriptor * alloc_pkt_message(struct pkt_controls * pkt_ctls){

  struct pkt_descriptor * pktd = NULL;
  ENTER;
  SPIN_LOCK(pkt_ctls->pkt_list_lock);
     pktd = pkt_ctls-> pkt_free_list;
     if (pktd != NULL) pkt_ctls-> pkt_free_list = pktd->next;
  SPIN_UNLOCK(pkt_ctls->pkt_list_lock);
  // force poll? attempt to take this descriptor if packet got processed and no poll active?  
  if (pktd==NULL){
   PRINT("pktd NULL return in %s %d \n",__FILE__,__LINE__);
   return NULL;   
  }
  pktd->req_id=0;
  pktd->error_val=0;
  pktd->flightinfo4delay = NULL;
  pktd->timestamp = 0;
  pktd->desc_cnt= -1; /* largest possible value */
  pktd->ccontext=NULL;
EXIT;
  return pktd;
};




int alloc_pkt_list_object( struct my_context * mcontext, uint32_t max_connections , uint64_t packetsize, struct mudm_memory_region * memregion){
   int i; 
   struct pkt_descriptor * pktd = NULL;
   int alloc_status; /* status from allocate callback */
   struct pkt_controls  * pc;
   alloc_status = alloc_object(sizeof(struct pkt_controls),memregion, mcontext,NULL, free_mem);
   if (alloc_status) return alloc_status;

   pc = (struct pkt_controls * )memregion->base_vaddr;
   pc->num_pkts =  max_connections;
   pc->pkt_list_lock.atom =0;
   pc->packet_size = packetsize;
   
   //! \todo Need assert that packetsize is multiple of 32
   alloc_status = alloc_object(pc->num_pkts * packetsize,&pc->pkt_list_region,mcontext,NULL, free_mem);
   if (alloc_status) return alloc_status;
   pc->packetlist = (char *)pc-> pkt_list_region.base_vaddr;

   alloc_status = alloc_object(pc->num_pkts * sizeof(struct pkt_descriptor ),&pc->pkt_desc_region,mcontext,NULL, free_mem);
   if (alloc_status) return alloc_status;
   pc->pkt_desc_list = (struct pkt_descriptor * )pc->pkt_desc_region.base_vaddr;

   pc->pkt_free_list = pc->pkt_desc_list;

   pc->pkt_reqid_list = NULL;

   //free list setup, single linked list
   for (i=0;i<pc->num_pkts;i++){
     pktd = pc->pkt_desc_list + i;
     pktd->next = pktd + 1;
     pktd->prev = NULL;
     pktd->pkt_msg = pc->packetlist + i*packetsize; /* char * pointer set up */
     pktd->message_pa = (uint64_t)pc-> pkt_list_region.base_paddr + i * packetsize;;
     pktd->pkt_control = pc;
   }
   pktd = pc->pkt_desc_list + (pc->num_pkts-1);
   pktd->next = NULL;
   PRINT("pc->num_pkts=%llu \n ", (LLUS)pc->num_pkts);

 return 0;
}


int32_t flush_pkt_reqid(struct my_context * mcontext,struct pkt_controls * pkt_ctls){
  struct pkt_descriptor * pktd = NULL;
  struct pkt_descriptor * last_pktd = NULL;
  struct pkt_descriptor * first_pktd = NULL;
  uint64_t ref_req_id = 0;
  uint32_t error_val = -ECANCELED;
  uint32_t error_return;

  if (NULL == pkt_ctls-> pkt_reqid_list) return 0;

  SPIN_LOCK(pkt_ctls->pkt_list_lock);
    pktd = pkt_ctls-> pkt_reqid_list;
    pkt_ctls-> pkt_reqid_list = NULL; //detach the list
  SPIN_UNLOCK(pkt_ctls->pkt_list_lock);
  if ( unlikely(NULL==pktd) ) return 0; 
  first_pktd = pktd;

  while (pktd != NULL){
       last_pktd = pktd;  //will eventually be the last in the list
       ref_req_id=pktd->req_id;  
       pktd->req_id=0;     
       //$$$
       PRINT(">>ECANCELED packet RequestID= %p \n",(void*)ref_req_id);     
       //if (ref_req_id) (mcontext->status)( (void*)ref_req_id, error_val, mcontext->callback_context);
       if (ref_req_id) (mcontext->status)(  (void*)&ref_req_id, &error_val,mcontext->callback_context,&error_return,1);
       pktd = pktd->next;
  }
  //move the list to the free list now
  SPIN_LOCK(pkt_ctls->pkt_list_lock);
    last_pktd->next = pkt_ctls->pkt_free_list;
    pkt_ctls->pkt_free_list = first_pktd;
  SPIN_UNLOCK(pkt_ctls->pkt_list_lock);
  return 0;
};

#define NUM_REFID 8
uint64_t poll_pkt_message(struct my_context * mcontext,struct pkt_controls * pkt_ctls){
  uint64_t unfreed_req_id = 0;
  int mudm_status_error=0;
  int refid_list_length = 0;
  uint64_t refid_list[NUM_REFID];
  uint32_t error_list[NUM_REFID];
  uint32_t ret_val[NUM_REFID];  
  uint64_t ref_desc_count = -1;
  uint64_t timestamp; 


  struct pkt_descriptor * pktd = NULL;
  struct pkt_descriptor * last_in_pktd2repost_list = NULL;
  struct pkt_descriptor * last_in_pktd2free_list = NULL;
  struct pkt_descriptor * pktd2free_list = NULL;
  struct pkt_descriptor * pktd2repost_list = NULL;

  //if (NULL == pkt_ctls-> pkt_reqid_list) return 0;
  //checked before calling into here

  SPIN_LOCK(pkt_ctls->pkt_list_lock);
    pktd = pkt_ctls-> pkt_reqid_list;
    pkt_ctls-> pkt_reqid_list = NULL; //detach the list
  SPIN_UNLOCK(pkt_ctls->pkt_list_lock);

  if ( unlikely(NULL==pktd) ) return 0; 

  // comment the next line out to cause job hang
  ref_desc_count = MUSPI_getHwDescCount (pkt_ctls ->IdToInjFifo);
  timestamp=GetTimeBase2(); 
  while (pktd != NULL){
    if (ref_desc_count >= pktd->desc_cnt){
      MUDM_MU_PKT_PCOMP( &mcontext->mudm_hi_wrap_flight_recorder,pktd,pktd->ccontext,pktd->desc_cnt,pktd->req_id);
      if (pktd->req_id){
        refid_list[refid_list_length]=pktd->req_id;
        pktd->req_id=0;
        error_list[refid_list_length]=pktd->error_val;
        pktd->error_val=0;
        refid_list_length++;
      }        
      if (pktd2free_list){
        last_in_pktd2free_list->next = pktd;
        pktd->prev = last_in_pktd2free_list;
        last_in_pktd2free_list = pktd;             
      }
      else {
        pktd2free_list = pktd;
        last_in_pktd2free_list = pktd;
        pktd2free_list->prev = NULL;                     
      }
      
      pktd=pktd->next;
      last_in_pktd2free_list->next = NULL;
  
      //if the list saturates, return resources 
      //and then send back refid list
      if ( unlikely(NUM_REFID==refid_list_length) ){
        PRINT("FLUSHING:  refid_list_length=%d \n", refid_list_length);
        SPIN_LOCK(pkt_ctls->pkt_list_lock);
            last_in_pktd2free_list->next = pkt_ctls->pkt_free_list;
            pkt_ctls->pkt_free_list = pktd2free_list;
        SPIN_UNLOCK(pkt_ctls->pkt_list_lock);
        pktd2free_list = NULL;
       
        mudm_status_error = (mcontext->status)( (void *) refid_list,error_list,mcontext->callback_context,ret_val,refid_list_length);
        mudm_status_error=0;
        refid_list_length=0;
       }//end of flushing the list
     }
     else {// NOT (ref_desc_count >= pktd->desc_cnt)
      if (pktd->req_id){
        unfreed_req_id ++;
      } 
//96000000000ull is about 1 minute on a 1.6hz machine
#define TOO_MANY_CYCLES 192000000000ull
      if ( likely(pktd->timestamp) ){
         if ( unlikely( ( (timestamp - pktd->timestamp)> (TOO_MANY_CYCLES) ) && (timestamp > pktd->timestamp) ) ){
            
              if (mcontext->StuckState==0){
                uint64_t cycles_per_sec = microsec2cycles(mcontext->personality,1000000); 
                uint64_t seconds_diff = (timestamp - pktd->timestamp)/cycles_per_sec;
                uint64_t num_uninjected_descriptors = log_injfifo_info (pktd->ccontext->injfifo_ctls,&mcontext->mudm_hi_wrap_flight_recorder,MUDMRAS_STUCK_INJ_RESETCOMPUTES);
                uint64_t entry_num = 
                 MUDM_STUCK_PKT(&mcontext->mudm_hi_wrap_flight_recorder,pktd->timestamp,timestamp,pktd->ccontext,pktd->req_id);
                MPRINT("num_uninjected_descriptors=%llu \n",(LLUS)num_uninjected_descriptors);
                MPRINT("STUCK pktd=%p timestamp=%llx pktd->timestamp=%llx  pktd->desc_cnt=%llu ref_desc_count=%llu req_id=%llx\n",pktd,(LLUS)timestamp, (LLUS)pktd->timestamp,(LLUS)pktd->desc_cnt,(LLUS)ref_desc_count,(LLUS)pktd->req_id ); 
                MPRINT("STUCKTIME=%llu (seconds) \n", (LLUS)seconds_diff);
                
                dump_flightlog_leadup(&mcontext->mudm_hi_wrap_flight_recorder, entry_num,10);
                dump_ccontext_info(pktd->ccontext);
                
                mcontext->StuckState = 1;  //extra dumping at free time
                          
                //! \todo TODO fix RAS info for stuck packet 
               MUDM_RASBEGIN(5); MUDM_RASPUSH((uint64_t)pktd)  MUDM_RASPUSH(timestamp) MUDM_RASPUSH(pktd->timestamp) MUDM_RASPUSH(pktd->desc_cnt) MUDM_RASPUSH(ref_desc_count) MUDM_RASFINAL(MUDMRAS_STUCK_PKT);  
             }//end of dumping info for stuck packet
                        
         pktd->timestamp = 0; 
       }
      }
      if (pktd2repost_list){
        last_in_pktd2repost_list->next = pktd;
        pktd->prev = last_in_pktd2repost_list;
        last_in_pktd2repost_list = pktd;
      }
      else{
        pktd2repost_list = pktd;
        last_in_pktd2repost_list = pktd;
        pktd->prev=NULL;
      }
      pktd=pktd->next;
      last_in_pktd2repost_list->next=NULL; 
    }//endofelse
   }//endwhile

   if (pktd2free_list){
        PRINT("FLUSHING:  refid_list_length=%d \n", refid_list_length);
        SPIN_LOCK(pkt_ctls->pkt_list_lock);
            last_in_pktd2free_list->next = pkt_ctls->pkt_free_list;
            pkt_ctls->pkt_free_list = pktd2free_list;
        SPIN_UNLOCK(pkt_ctls->pkt_list_lock);
   }
   if (pktd2repost_list){
    SPIN_LOCK(pkt_ctls->pkt_list_lock);
      last_in_pktd2repost_list->next = pkt_ctls-> pkt_reqid_list;
      pkt_ctls-> pkt_reqid_list = pktd2repost_list;
    SPIN_UNLOCK(pkt_ctls->pkt_list_lock);
   }
   if (refid_list_length){
     mudm_status_error = (mcontext->status)( (void *) refid_list,error_list,mcontext->callback_context,ret_val,refid_list_length);
   }
   if ( unlikely(mudm_status_error !=0) ){
     int i=0;
     for (i=0;i<refid_list_length;i++){
       if (ret_val[i])MPRINT("mudm_status_error i=%d refid_list[i]=%p ret_val[i]=%llx \n",i,(void *)refid_list[i],(LLUS)ret_val[i]);
     }
   }
   return unfreed_req_id;
}

void check4notRight(struct pkt_controls * pkt_ctls,BG_FlightRecorderRegistry_t* logregistry){

  uint64_t expected = pkt_ctls->num_pkts;

  uint64_t free_num = 0;
  uint64_t reqid_num = 0;
  //uint64_t backwards_reqid_num = 0;
  struct pkt_descriptor * pktd = NULL;
  //LOCK
  SPIN_LOCK(pkt_ctls->pkt_list_lock);
  pktd = pkt_ctls->pkt_free_list;
  while (pktd){
    free_num++;
    pktd = pktd->next;
    if (free_num > expected){
      break;
    }
  }
  pktd = pkt_ctls->pkt_reqid_list;
  while (pktd){
    reqid_num++;
    pktd = pktd->next;
    if (reqid_num > expected){ 
      DB_PKT_CHECK(logregistry,pkt_ctls,expected,free_num,reqid_num);
      break;
    }
  }
  PKT_CHECK(logregistry,pkt_ctls,expected,free_num,reqid_num);
  
  //UNLOCK
  SPIN_UNLOCK(pkt_ctls->pkt_list_lock);

}

