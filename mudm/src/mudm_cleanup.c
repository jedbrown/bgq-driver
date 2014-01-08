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

//! \file  mudm_cleanup.c
//! \brief Handle termination of MUDM context

#include "mudm_cleanup.h"
#include "common.h"
#include "common_inlines.h"
#include "mudm_macro.h"
#include "mudm_pkt.h"
#include "rdma_object.h"
#include "mudm_wu.h"
#include "mudm_connection.h"
#include <spi/include/mu/Reset.h> 
#include <spi/include/mu/Addressing_inlines.h>


struct cleanup_entry clean_table[MAX_CLEANUP_ENTRIES]; //base for table 
int num_cleanup_entries=0;

uint64_t free_mem(struct mudm_memory_region * memregion,struct my_context * mcontext, void * args){
  int status_free = (mcontext->free)(memregion,mcontext->callback_context); 
  return status_free;
}

int process_cleanup_table(struct my_context * mcontext){
  int i;
  for (i=num_cleanup_entries - 1 ; i>=0; i--){
    PRINT("i=%d \n",i);
    PRINT("cleanup_fn=%p memregion=%p mcontext=%p argslist=%p \n",clean_table[i].cleanup_fn,clean_table[i].memregion,mcontext,clean_table[i].argslist);
    if (clean_table[i].cleanup_fn != NULL){
       clean_table[i].status  = clean_table[i].cleanup_fn(clean_table[i].memregion,
                                                                           mcontext,
                                                                           clean_table[i].argslist);
    }
    else {
      clean_table[i].status  = free_mem(clean_table[i].memregion, mcontext, clean_table[i].argslist);
    }
    //if (clean_table[i].status) return clean_table[i].status;                     
  }

  return 0;
}



int alloc_object(size_t length_mem,struct mudm_memory_region * mregion,struct my_context * mcontext, void * args, cleanup_fn_t fn){
   int alloc_status; /* status from allocate callback */   
  
  if (num_cleanup_entries >= MAX_CLEANUP_ENTRIES){
     MPRINT("EXCEED MAX_CLEANUP_ENTRIES: num_cleanup_entries=%lld \n",(LLUS)num_cleanup_entries);
     return -ENOMEM;
  }

  alloc_status = (mcontext->allocate)(mregion,length_mem,mcontext->callback_context);
  if (alloc_status){
    //! \todo cleanup by running cleanup function and freeing storage
    return -ENOMEM;
  }  
  zero_memregion(mregion);
  
  clean_table[num_cleanup_entries].memregion = mregion;
  clean_table[num_cleanup_entries].status = 0;
  clean_table[num_cleanup_entries].argslist = args;
  clean_table[num_cleanup_entries].cleanup_fn = fn;
  num_cleanup_entries++;
  //PRINT("num_cleanup_entries=%lld \n",(LLUS)num_cleanup_entries);
  return 0;

};

int32_t disable_active_connection_list(struct my_context * mcontext,struct mudm_connection ** conn_activelist){
  int32_t num = 0;
  struct mudm_connection * ccontext;
  num++;
  MSPIN_LOCK(mcontext->conn_list_lock);//yeah, this lock is hold a long time
  ccontext = *conn_activelist;
    // change the state of all on the active list and void the local reference to the active list
    while (NULL != ccontext){
     ccontext->state=CONN_DISCONNECTING;
     ccontext->conn_activelist=NULL;
     ccontext = ccontext->nextconn;
    } 
  MSPIN_UNLOCK(mcontext->conn_list_lock);
  return 0;
};

int32_t disable_pending_connection_list(struct my_context * mcontext,struct mudm_connection ** conn_pendlist,struct mudm_connection ** conn_abortlist){
  int32_t num = 0;
  struct mudm_connection * ccontext;
  struct mudm_connection * last_ccontext=NULL;
  MSPIN_LOCK(mcontext->conn_list_lock);
    ccontext = *conn_pendlist;
    // change the state of all on the pend list
    while (NULL != ccontext){
     num++;
     ccontext->state=CONN_ABORTED;
     last_ccontext = ccontext;
     ccontext = ccontext->nextconn;
    } 
    if (last_ccontext){
      last_ccontext->nextconn = *conn_abortlist;
      *conn_abortlist = *conn_pendlist;
      *conn_pendlist=NULL;
    }
  MSPIN_UNLOCK(mcontext->conn_list_lock);
  return num;
};


#define RMFIFOEN16 MHERE; MPRINT("DCRReadPv(MU_DCR(RMFIFO_EN) + 16)=%llx \n",(LLUS)DCRReadPriv(MU_DCR(RMFIFO_EN) + 16) )


int mudm_resetIOlink(void* mudm_context , uint32_t BlockID){
  MUSPI_RESET_t mureset;
  int i=0;
  int32_t rc = 0;
  uint64_t entry_num;
  struct my_context * mcontext = (struct my_context *)mudm_context; 
  uint64_t num_uninjected_descriptors = 0;
  uint64_t cycles_per_usec = 1600;
  uint64_t time_diff = 0;
  uint64_t timestamp_begin = GetTimeBase2(); 
  uint64_t timestamp_end = timestamp_begin;
  
  if (mcontext->magic != MUDM_MAGIC){
    
    MPRINT("-EFAULT:  Bad Magic for mudm_context=%p TERMINATING FUNCTION %s \n",mudm_context, __FUNCTION__);
    return -EFAULT;
  }

  
  if (mcontext->StuckState){
     MENTER;
  }
  //quit polling for links with the BlockID
  for (i=0;i<NUM_IO_LINKS;i++){  /*BEAM*//*loop doesn't iterate*/
     //MPRINT("ififo_subgroup->groupStatus->isEnabled=%llx   \n",(LLUS)mcontext->MU_subgroup_mem[i]->ififo_subgroup.groupStatus->isEnabled); 
    
    if(  mcontext->remote_BlockID[i]==BlockID  )
    { entry_num =
      MUDM_IO_LINKRESET(&mcontext->mudm_no_wrap_flight_recorder,i,BlockID,mcontext->remote_BlockID[i],mcontext->link4reset[i]);
      // set state of connection and reject requests that lead to injection
      mcontext->injfifo_ctls[i].state = 0;  //INACTIVE injection FIFO 
      num_uninjected_descriptors = log_injfifo_info (&mcontext->injfifo_ctls[i],&mcontext->mudm_hi_wrap_flight_recorder,MUDMRAS_STUCK_INJ_AT_LINKRESET);
          
      if (num_uninjected_descriptors){
        // if a stuck packet was not previously caught, note the state of the injection FIFO had uninjected descriptors
        if (0==mcontext->StuckState) dump_flightlog_leadup(&mcontext->mudm_hi_wrap_flight_recorder, entry_num,1); 
      }
    }
    else {
      MUDM_IO_LINKSKIPPED(&mcontext->mudm_no_wrap_flight_recorder,i,BlockID,mcontext->remote_BlockID[i],mcontext->link4reset[i]);
   }

  }

  
  for (i=0;i<NUM_IO_LINKS;i++){  /*BEAM*//*loop doesn't iterate*/
   
   if( mcontext->remote_BlockID[i]==BlockID )//reset if no report on link
   {   
      //put the link active list of connections into disconnect state
      rc = disable_active_connection_list(mcontext, &mcontext->conn_activelist[i]);
      //put the link pending list of connections into ABORT state
      rc = disable_pending_connection_list(mcontext, &mcontext->conn_pendlist[i],&mcontext->conn_abortlist[i]);

      //first flush back of request IDs to user
      rc=flush_pkt_reqid(mcontext,mcontext->packetcontrols[i]);
      if (rc) MPRINT("rc = %d flush_pkt_reqid(mcontext,mcontext->packetcontrols[i]) line=%d \n",rc,__LINE__);
      rc=flush_pkt_reqid(mcontext,mcontext->smallpa_obj_ctls[i]);
      if (rc) MPRINT("rc = %d flush_pkt_reqid(mcontext,mcontext->smallpa_obj_ctls[i]); line=%d \n",rc,__LINE__);
      rc = flush_rdma_counter_reqid(mcontext,mcontext->rdma_obj_ctls[i]);
      if (rc) MPRINT("rc = %d flush_rdma_counter_reqid(mcontext,mcontext->rdma_obj_ctls[i]) line=%d \n",rc,__LINE__);
      MUDM_IO_LINKFLUSHED(&mcontext->mudm_no_wrap_flight_recorder,i,BlockID,mcontext->remote_BlockID[i],mcontext->link4reset[i]);
      //link in reset:
      
      rc = MUSPI_InitResetHold(&mureset, mcontext->MU_subgroup_mem[i]->subgroupID, mcontext->link4reset[i]);
      //MH;
      do{
        // <--- Using this API for between job cleanup requires a GI barrier here.  But that should not be necessary
        //      for ionode link cleanup (since there is only 1 participant)  Possibly required if we 
        //      ever do IO torus mixed with compute partitions.  Don't worry about that for now.
        
        rc = MUSPI_Reset(&mureset);

        //MPRINT("ififo_subgroup->groupStatus->isEnabled=%llx  mcontext->MU_subgroup_mem[i]->subgroupID=%d mcontext->link4reset[i]=%llu ",(LLUS)mcontext->MU_subgroup_mem[i]->ififo_subgroup.groupStatus->isEnabled,mcontext->MU_subgroup_mem[i]->subgroupID,(LLUS)mcontext->link4reset[i]); MHERE;
        //MPRINT("rc = MUSPI_Reset(&mureset) rc=%d \n",rc);
      } while(rc == EAGAIN);
      if (rc) MPRINT("postwhileloop rc = MUSPI_Reset(&mureset) rc=%d \n",rc);
      

      // second flush back of request IDs to user
      rc=flush_pkt_reqid(mcontext,mcontext->packetcontrols[i]);
      if (rc) MPRINT("rc = %d flush_pkt_reqid(mcontext,mcontext->packetcontrols[i]) line=%d \n",rc,__LINE__);
      rc=flush_pkt_reqid(mcontext,mcontext->smallpa_obj_ctls[i]);
      if (rc) MPRINT("rc = %d flush_pkt_reqid(mcontext,mcontext->smallpa_obj_ctls[i]); line=%d \n",rc,__LINE__);
      rc = flush_rdma_counter_reqid(mcontext,mcontext->rdma_obj_ctls[i]);
      if (rc) MPRINT("rc = %d flush_rdma_counter_reqid(mcontext,mcontext->rdma_obj_ctls[i]) line=%d \n",rc,__LINE__);
      MUDM_IO_LINKFLUSHED(&mcontext->mudm_no_wrap_flight_recorder,i,BlockID,mcontext->remote_BlockID[i],mcontext->link4reset[i]);

      //when done with the reset, release the IO link from reset:

      MUSPI_InitResetRelease(&mureset, mcontext->MU_subgroup_mem[i]->subgroupID, mcontext->link4reset[i]);
      do{
        // <--- Using this API for between job cleanup requires a GI barrier here.  But that should not be necessary
        //      for ionode link cleanup (since there is only 1 participant)  Possibly required if we
        //      ever do IO torus mixed with compute partitions.  Don't worry about that for now.
        rc = MUSPI_Reset(&mureset);
        //MH;
        //RMFIFOEN16;
      } while(rc == EAGAIN);
   
     // MPRINT("ififo_subgroup->groupStatus->isEnabled=%llx   \n",(LLUS)mcontext->MU_subgroup_mem[i]->ififo_subgroup.groupStatus->isEnabled); MHERE;
      // Deactivations done by MUSPI_Reset()
      if (rc) MPRINT("2nd postwhileloop rc = MUSPI_Reset(&mureset) rc=%d \n",rc);

      // third and final flush back of request IDs to user
      rc=flush_pkt_reqid(mcontext,mcontext->packetcontrols[i]);
      if (rc) MPRINT("rc = %d flush_pkt_reqid(mcontext,mcontext->packetcontrols[i]) line=%d \n",rc,__LINE__);
      rc=flush_pkt_reqid(mcontext,mcontext->smallpa_obj_ctls[i]);
      if (rc) MPRINT("rc = %d flush_pkt_reqid(mcontext,mcontext->smallpa_obj_ctls[i]); line=%d \n",rc,__LINE__);
      rc = flush_rdma_counter_reqid(mcontext,mcontext->rdma_obj_ctls[i]);
      if (rc) MPRINT("rc = %d flush_rdma_counter_reqid(mcontext,mcontext->rdma_obj_ctls[i]) line=%d \n",rc,__LINE__);
      //MH;

      rc = flush_iolink_connection_list(mcontext, &mcontext->conn_activelist[i]);
      MUDM_IO_LINKFLUSHED(&mcontext->mudm_no_wrap_flight_recorder,i,BlockID,mcontext->remote_BlockID[i],mcontext->link4reset[i]);
#if 0
      check4notRight( mcontext -> packetcontrols[i],&mcontext->mudm_no_wrap_flight_recorder);
      check4notRight( mcontext-> smallpa_obj_ctls[i],&mcontext->mudm_no_wrap_flight_recorder);
#endif 
   }
   //MH;
  }//endfor i
 
  for (i=0;i<NUM_IO_LINKS;i++){     /*BEAM*//*loop doesn't iterate*/
   //if( (mcontext->remote_BlockID[i]==BlockID) || mcontext->remote_BlockID[i]==0)//reset if no report on link
   if( mcontext->remote_BlockID[i]==BlockID )//reset if no report on link
   {   
      //re-activate base address table, receive and injection FIFOs

      activate_base_address_table(&mcontext->MU_subgroup_mem[i]->bat,BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP,mcontext->MU_subgroup_mem[i]->batids,mcontext->MU_subgroup_mem[i]->subgroupID,mcontext->MU_subgroup_mem[i]->batvalues);
      // Need to reset the base address for broadcast counter to physical address of main control block mcontext

        /* init the io inj fifo lock */
      mcontext->injfifo_ctls[i].inj_fifo_lock.atom = 0;
      activate_injfifos( mcontext->MU_subgroup_mem[i]);
      
      activate_recfifos(mcontext->MU_subgroup_mem[i]);
      
      // Enable state for connecting over link
      mcontext->injfifo_ctls[i].state = 1;  //ACTIVE injection FIFO 
      MUDM_IO_LINKRSETC(&mcontext->mudm_no_wrap_flight_recorder,i,BlockID,mcontext->remote_BlockID[i],mcontext->link4reset[i]);
      mcontext->remote_BlockID[i]=0;
   }

  }//endfor i on activation
  if (mcontext->StuckState){
     MEXIT;
  }
  mcontext->StuckState = 0;  //Clear note of StuckState which dumped extra info on free
  timestamp_end = GetTimeBase2(); 
  time_diff = (timestamp_end - timestamp_begin)/cycles_per_usec;
  MUDM_DM_USEC_DIFF(&mcontext->mudm_hi_wrap_flight_recorder,time_diff, __LINE__, timestamp_begin, timestamp_end);

  return 0;
};
EXPORT_SYMBOL(mudm_resetIOlink);


int quiet_IOlinks(void* mudm_context){
  int i=0;
  int32_t rc = 0;
  struct my_context * mcontext = (struct my_context *)mudm_context; 

  for (i=0;i<NUM_IO_LINKS;i++){     /*loop doesn't iterate*/
    mcontext->injfifo_ctls[i].state = 0;  //INACTIVE injection FIFO 
    my_ppc_msync();//just to be overly cautious
    wakeup();
  }

  for (i=0;i<NUM_IO_LINKS;i++){     /*loop doesn't iterate*/

    //put the link active list of connections into disconnect state
    rc = disable_active_connection_list(mcontext, &mcontext->conn_activelist[i]);
    //put the link pending list of connections into ABORT state
    rc = disable_pending_connection_list(mcontext, &mcontext->conn_pendlist[i],&mcontext->conn_abortlist[i]);

    //first flush back of request IDs to user--no checking of return codes
    flush_pkt_reqid(mcontext,mcontext->packetcontrols[i]);
    flush_pkt_reqid(mcontext,mcontext->smallpa_obj_ctls[i]);
    flush_rdma_counter_reqid(mcontext,mcontext->rdma_obj_ctls[i]);
    flush_iolink_connection_list(mcontext, &mcontext->conn_activelist[i]);
  }
  return 0;
};


int mudm_terminate(void* mudm_context)
{


  struct my_context * mcontext = (struct my_context *)mudm_context;

if (mcontext->magic != MUDM_MAGIC){
    
    MPRINT("-EFAULT:  Bad Magic for mudm_context=%p TERMINATING FUNCTION %s \n",mudm_context, __FUNCTION__);
    
    return -EFAULT;
}
mcontext->state = MUDM_TERMINATING;//=0
MUDM_TERM_START(&mcontext->mudm_hi_wrap_flight_recorder,__LINE__,MUDM_VERSION,NUM_IO_LINKS,mcontext);
quiet_IOlinks(mcontext);

/* need to free allocated storage only if Linux */
//#ifdef __LINUX__ 
  process_cleanup_table(mcontext);
//#endif 

PRINT("\n ==== MUDM TERMINATED====\n");
mcontext->magic = 0; /* clear for termination */
EXIT;
return 0;
}
EXPORT_SYMBOL(mudm_terminate);


int add_object(struct mudm_memory_region * mregion,struct my_context * mcontext, void * args, cleanup_fn_t fn){
  
  if (num_cleanup_entries >= MAX_CLEANUP_ENTRIES){
     MPRINT("EXCEED MAX_CLEANUP_ENTRIES: num_cleanup_entries=%lld \n",(LLUS)num_cleanup_entries);
     return -ENOMEM;
  }

  clean_table[num_cleanup_entries].memregion = mregion;
  clean_table[num_cleanup_entries].status = 0;
  clean_table[num_cleanup_entries].argslist = args;
  clean_table[num_cleanup_entries].cleanup_fn = fn;
  num_cleanup_entries++;
  //PRINT("num_cleanup_entries=%lld \n",(LLUS)num_cleanup_entries);
  return 0;

};
