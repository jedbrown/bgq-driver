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

//! \file  rdma_object.c 
//! \brief Manage RDMA exchanges using RDMA objects.

#include "mudm_macro.h"
#include "rdma_object.h"
#include "common_inlines.h"
#include "mudm_wu.h"



uint64_t free_rdma_obj_region(struct mudm_memory_region * memregion, struct my_context * mcontext, void * args){
 uint64_t free_status = 0;
 struct rdma_obj_controls * roc = (struct rdma_obj_controls *)args;
 ENTER;
 if (roc-> counter_list != NULL){
    MDUMPHEXSTR("active rdma_object on counter_list",roc-> counter_list,sizeof(struct mudm_rdma_object) );
  }

 free_status = free_mem(memregion, mcontext,args);
 EXIT;
 return free_status;
}


#define RPRINT PRINT
#define NUM_REFID 8
//32*8 =256 bytes
uint64_t rdma_poll_rdma_counter(struct my_context * mcontext,struct rdma_obj_controls * roc){
  uint64_t counters_pending=0;
  int mudm_status_error=0;
  int refid_list_length = 0;
  uint64_t refid_list[NUM_REFID];
  uint32_t error_list[NUM_REFID];
  uint32_t ret_val[NUM_REFID];
  struct mudm_rdma_object * mro = NULL;
  struct mudm_rdma_object * last_in_mro2repost_list = NULL;
  struct mudm_rdma_object * mro2repost_list = NULL;
  struct mudm_rdma_object * last_in_mro2free_list = NULL;
  struct mudm_rdma_object * mro2free_list = NULL;

  SPIN_LOCK(roc->rdma_list_lock);
   mro = roc->counter_list;  
   roc->counter_list = NULL;  // detached list
  SPIN_UNLOCK(roc->rdma_list_lock);

  while(mro != NULL){
    
    // check for hung RDMA here or special routine for hangs of all kinds
    if( (mro->mue->mu_counter==0 ) || (mro->mue->error!=0) ) {
      my_ppc_msync(); /* need to msync memory for memory written into DDR by the MU */
      /* using deterministic system directput, no more directputs targeting counter after error update by a directput */  
      RDMA_COUNTER_HIT(&mcontext->mudm_hi_wrap_flight_recorder,mro);
      //If remote_rdma counter, SEND COMPLETION RDMA
      if (mro->remote_rdma_obj_pa){
         uint64_t desc_count;
         struct mudm_connection * ccontext = mro-> local_connect;
         uint64_t source_physaddr = mcontext->mudm_context_phys_addr + offsetof(struct my_context,rdma_GOOD_source);
         if ( unlikely(mro->mue->error!=0)) source_physaddr =  mcontext->mudm_context_phys_addr + offsetof(struct my_context,rdma_EINVAL_source);
         CHECK_CONN_MAGIC(ccontext);
         desc_count = direct_put(ccontext ,
                                           source_physaddr, 
                                           mro->remote_rdma_obj_pa + offsetof(struct mu_element,error ),
                                           sizeof(mro->mue->error),
                                           mro->remote_rdma_obj_pa );
         RPRINT("rdma_poll_rdma_counter sent read-done to remote  mro=%p  counter=%llu poll_count=%llx rdma_object_pa=%p mro->local_request_id=%p\n",mro,(LLUS)mro->mue->mu_counter,(LLUS)mro->poll_count,(void*) mro->rdma_object_pa,(void*)mro->local_request_id);

      }
      if (mro->local_request_id){
        refid_list[refid_list_length]=mro->local_request_id;
        mro->local_request_id=0;
        error_list[refid_list_length]=mro->mue->error;
        mro->mue->error=0;
        refid_list_length++;
      }
         

      if (mro2free_list){
        last_in_mro2free_list->next = mro;
        mro->prev = last_in_mro2free_list;
        last_in_mro2free_list = mro;             
      }
      else {
        mro2free_list = mro;
        last_in_mro2free_list = mro;
        mro2free_list->prev = NULL;                     
      }
      mro=mro->next;
      last_in_mro2free_list->next = NULL;

      //if the list saturates, return rdma tracking resources 
      //and then send back refid list
      if ( unlikely(NUM_REFID==refid_list_length) ){
        RPRINT("FLUSHING:  refid_list_length=%d \n", refid_list_length);
        SPIN_LOCK(roc->rdma_list_lock);
            RPRINT("mro2free_list=%p last_in_mro2free_list=%p roc-> rdma_free_list=%p \n", mro2free_list,last_in_mro2free_list,roc-> rdma_free_list);
            last_in_mro2free_list->next = roc-> rdma_free_list;
            roc-> rdma_free_list = mro2free_list;
        SPIN_UNLOCK(roc->rdma_list_lock);
        mro2free_list = NULL;

        mudm_status_error = (mcontext->status)( (void *) refid_list,error_list,mcontext->callback_context,ret_val,refid_list_length);
        refid_list_length=0;
        mudm_status_error=0;
       }//end of flushing the list


    }
    else {//not (0 counter or an error)
      counters_pending++;
      if (mro2repost_list){
        last_in_mro2repost_list->next = mro;
        mro->prev = last_in_mro2repost_list;
        last_in_mro2repost_list = mro;
      }
      else{
        mro2repost_list = mro;
        last_in_mro2repost_list = mro;
        mro->prev=NULL;
      }
      mro=mro->next;
      last_in_mro2repost_list->next=NULL; 
    }
  }//endwhile
  if (mro2free_list){
    SPIN_LOCK(roc->rdma_list_lock);
      RPRINT("mro2free_list=%p last_in_mro2free_list=%p roc-> rdma_free_list=%p \n", mro2free_list,last_in_mro2free_list,roc-> rdma_free_list);
      last_in_mro2free_list->next = roc-> rdma_free_list;
      roc-> rdma_free_list = mro2free_list;
    SPIN_UNLOCK(roc->rdma_list_lock);
  }
  if (mro2repost_list){
    SPIN_LOCK(roc->rdma_list_lock);
      last_in_mro2repost_list->next = roc->counter_list;
      roc->counter_list = mro2repost_list;
    SPIN_UNLOCK(roc->rdma_list_lock);
  }
  if (refid_list_length){
    mudm_status_error = (mcontext->status)( (void *) refid_list,error_list,mcontext->callback_context,ret_val,refid_list_length);
  }
  return counters_pending;
};


 void release_rdma_obj_to_poll(struct mudm_rdma_object * mro, int list_type){

  struct mudm_rdma_object ** mro_list = NULL;
  ENTER;
  mro_list = &mro->rdma_control->counter_list;  
  my_ppc_msync();//ensure nonzero counter value is seen by MU
  SPIN_LOCK(mro->rdma_control->rdma_list_lock);
  mro->prev = NULL;
  mro->next = *mro_list;
  if (*mro_list != NULL) (*mro_list)->prev=mro;
  *mro_list = mro;
  SPIN_UNLOCK(mro->rdma_control->rdma_list_lock);
  wakeup();
PRINT("COUNTER counter_list=%p \n",mro->rdma_control->counter_list);
EXIT;
};

 void free_rdma_obj(struct mudm_rdma_object * mro){
ENTER;  

  SPIN_LOCK(mro->rdma_control->rdma_list_lock);
    mro->next = mro->rdma_control-> rdma_free_list;
    mro->rdma_control-> rdma_free_list = mro;
    mro->rdma_control->rdma_freed_count++;
  SPIN_UNLOCK(mro->rdma_control->rdma_list_lock);
EXIT;
};

int32_t flush_rdma_counter_reqid(struct my_context * mcontext,struct rdma_obj_controls * roc){
  uint64_t ref_req_id = 0;
  uint32_t error_val = -ECANCELED;
  struct mudm_rdma_object * mro = NULL;
  struct mudm_rdma_object * last_mro = NULL;
  struct mudm_rdma_object * first_mro = NULL;
  uint32_t error_return;

  if (NULL==roc->counter_list)return 0;

  SPIN_LOCK(roc->rdma_list_lock);
   mro = roc->counter_list;  
   roc->counter_list = NULL;  // detached list
  SPIN_UNLOCK(roc->rdma_list_lock);

  if ( unlikely(NULL==mro) ) return 0; 

  first_mro = mro;
  while(mro != NULL){
    last_mro = mro; //will eventually be the last in the list
    ref_req_id = mro->local_request_id;
    mro->mue->mu_counter =0;
    mro->local_request_id = 0;
    if (ref_req_id){
        (mcontext->status)(  (void*)&ref_req_id, &error_val,mcontext->callback_context,&error_return,1);
    }
    mro=mro->next;
  }//endwhile
  // put processed entries back onto the free list
  
  SPIN_LOCK(roc->rdma_list_lock);
    last_mro->next = roc-> rdma_free_list;
    roc-> rdma_free_list = first_mro;
  SPIN_UNLOCK(roc->rdma_list_lock);
  return 0;
};

uint64_t compute_rdma_section_size( uint32_t max_connections, uint64_t * num_mu_elements,int num_io_links){
    uint64_t compare_area_size = 2048; //minimum size for compare area
    uint32_t objects_per_connection = 4;
    if (num_io_links > 1) objects_per_connection = 2;//assume roughly evenly divided across 2 links
    *num_mu_elements = objects_per_connection * max_connections;
    if (*num_mu_elements < 128) *num_mu_elements = 128;
    while (compare_area_size < ((*num_mu_elements) * sizeof(struct mu_element)) ) compare_area_size = compare_area_size << 1;
    //MPRINT("compare_area_size=%llx *num_mu_elements=%llx \n", (LLUS)compare_area_size, (LLUS)*num_mu_elements);
    if (compare_area_size == *num_mu_elements * sizeof(struct mu_element))
     *num_mu_elements = (*num_mu_elements)-1;//reserve last bytes for control bytes
    return compare_area_size;
}

 void rdma_obj_init( struct rdma_obj_controls  * roc){
 int i;
 struct mudm_rdma_object * ro = roc->rdma_obj_list;
 struct mu_element * base = roc-> base_va;
 roc->rdma_list_lock.atom=0;

 roc->rdma_free_list = ro;
 roc->counter_list  = NULL;

 for (i=0;i<roc->num_obj;i++){
   ro = roc->rdma_obj_list + i;
   ro->next = ro + 1;
   ro->prev = NULL;
   ro->rdma_control = roc;
   ro->rdma_object_pa = roc->base_pa + i * sizeof(struct mu_element);
   ro->mue =  base + i;
   if ( (0xFFFFFFFFFF & (uint64_t)ro->mue)^(uint64_t)ro->rdma_object_pa){
     MHERE;
     MPRINT("ro->mue=%p ro->rdma_object_pa=%llx \n",ro->mue,(LLUS)ro->rdma_object_pa);
   }
 }
 ro = roc->rdma_obj_list + (roc->num_obj-1);//last one in the list
 ro->next = NULL;

 return;
}
int rdma_init(struct my_context * mcontext, uint32_t max_connections,int num_io_links){
   int i = 0;
   int alloc_status = 0;
   uint64_t num_mu_elements = 128;  //this may be changed when setting rss (side effect)
   uint64_t rss = compute_rdma_section_size(max_connections, &num_mu_elements,num_io_links);
   uint64_t lowbit_mask=0;
   uint64_t lowbits_value = 0;
   uint64_t compare_block_offset=0;
   
   //MHERE;
   //MPRINT("rss=%llu hex(%llx)\n",(LLUS)rss,(LLUS)rss);
   //MPRINT("max_connections=%llu num_mu_elements=%llu num_io_links=%llu \n",(LLUS)max_connections, (LLUS)num_mu_elements,(LLUS)num_io_links);
   //MPRINT("rdma section size=%llu \n",(LLUS)rss);
   for (i=0;i<num_io_links;i++){

     //! \todo TODO  Replace free-mem with routine for canceling of transaction IDs, etc.
     //! \note need to on an address that is a multiple of rss, so allocation includes 2*rss for adjusting to the boundary
     alloc_status = alloc_object(rss * 2 + sizeof(struct rdma_obj_controls) + num_mu_elements * sizeof(struct mudm_rdma_object),
        &mcontext->rdma_mregion[i],mcontext,NULL, free_mem);

     mcontext->memblock[i].pa_start = (uint64_t)mcontext->rdma_mregion[i].base_paddr;
     lowbit_mask = rss-1; //assumed that rss is a power of 2
     mcontext->memblock[i].mask = lowbit_mask^(uint64_t)(-1) ;
     lowbits_value = (mcontext->memblock[i].pa_start & lowbit_mask);
     if (lowbits_value){
       //MHERE;
       //MPRINT("compare block pa=%llx mask=%llx \n", (LLUS)mcontext->memblock[i].pa_start, (LLUS)mcontext->memblock[i].mask);
       mcontext->memblock[i].pa_start += rss;
       mcontext->memblock[i].pa_start &= mcontext->memblock[i].mask;
       compare_block_offset = mcontext->memblock[i].pa_start - (uint64_t)mcontext->rdma_mregion[i].base_paddr;
     }
     //MHERE;
     //MPRINT("compare block pa=%llx mask=%llx \n", (LLUS)mcontext->memblock[i].pa_start, (LLUS)mcontext->memblock[i].mask);
     //MPRINT("alloc pa=%llx  compare_block_offset = %llx \n", (LLUS)mcontext->rdma_mregion[i].base_paddr   ,(LLUS)compare_block_offset);
         
     mcontext->compare_area[i]=mcontext->rdma_mregion[i].base_vaddr + compare_block_offset;
     //MPRINT("mcontext->compare_area[i]=%llx \n", (LLUS)mcontext->compare_area[i]);

     mcontext->rdma_obj_ctls[i] = (struct rdma_obj_controls *)((void *)mcontext->compare_area[i] + rss); //after the compare area
     mcontext->rdma_obj_ctls[i]->rdma_obj_list = (struct mudm_rdma_object *)(mcontext->rdma_obj_ctls[i] + 1);//after object ctls
         
     mcontext->rdma_obj_ctls[i]->base_va=(struct mu_element *)mcontext->compare_area[i];
     //MPRINT("mcontext->rdma_obj_ctls[i]->base_va=%p \n",mcontext->rdma_obj_ctls[i]->base_va);

     num_mu_elements--;
     mcontext->rdma_obj_ctls[i]->rdma_bcast_controls = mcontext->rdma_obj_ctls[i]->base_va + num_mu_elements;
     mcontext->rdma_obj_ctls[i]->rdma_bcast_controls_offset =  num_mu_elements * sizeof(struct mu_element);
     mcontext->rdma_obj_ctls[i]->num_obj=num_mu_elements;

     mcontext->rdma_obj_ctls[i]->base_pa=mcontext->memblock[i].pa_start; //physical address corresponding to pinned base_va
     mcontext->rdma_obj_ctls[i]->compare_area_size = rss;
          
     rdma_obj_init( mcontext->rdma_obj_ctls[i]);     
   }
   return 0;
}
