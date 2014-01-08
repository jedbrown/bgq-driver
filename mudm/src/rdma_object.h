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

#ifndef	_RDMA_OBJECT_H_ /* Prevent multiple inclusion */
#define	_RDMA_OBJECT_H_
#include "common.h"
#include "mudm_macro.h"
#include "mudm_lock.h"

  
struct mudm_rdma_object {
  uint64_t remote_rdma_obj_pa;/* physical address of remote mudm_rdma_object    */
  struct mu_element * mue;
  uint64_t rdma_object_pa;    /* physical address of mu_counter */ 
  uint64_t local_request_id;  /* call status callback      */  
  struct mudm_connection * local_connect;      /* local connection block      */
  struct rdma_obj_controls * rdma_control;  /* point back to the controls       */         
  struct mudm_rdma_object * next;
  struct mudm_rdma_object * prev;
 }__attribute__ ((aligned (64)));

struct rdma_obj_controls  {
  uint64_t num_obj;                   /* number of mudm_rdma_object      */
  volatile uint64_t rdma_freed_count;
  volatile uint64_t rdma_allocated_count;
  uint64_t base_pa;  //physical address for base_va
  struct mu_element * base_va;
  uint64_t compare_area_size;
  struct mudm_rdma_object * rdma_obj_list; 
  struct mudm_memory_region rdma_obj_region; 
  struct mudm_rdma_object * rdma_free_list; 
  struct mudm_rdma_object * counter_list; 
  struct mu_element * rdma_bcast_controls;
  uint64_t rdma_bcast_controls_offset;
  Lock_Atomic_t rdma_list_lock;      /* packet polling spinlock          */ 
  uint64_t dummy_hold;  //pad out
} __attribute__((__packed__)) ;


int rdma_init(struct my_context * mcontext, uint32_t max_connections,int num_io_links);


#define COUNTER 0
 


__INLINE__ struct mudm_rdma_object * allocate_rdma_object(struct rdma_obj_controls * ro_ctls){
  struct mudm_rdma_object * mro = NULL;
  ENTER;
  SPIN_LOCK(ro_ctls->rdma_list_lock);
  mro = ro_ctls->rdma_free_list;
  if (mro != NULL) ro_ctls->rdma_free_list = mro->next;
  ro_ctls->rdma_allocated_count++;
  SPIN_UNLOCK(ro_ctls->rdma_list_lock);
  if (mro==NULL){
    PRINT("mro NULL return in %s %d \n",__FILE__,__LINE__);
    EXIT;
    return NULL;
  }
  mro->local_request_id=0;
  mro->remote_rdma_obj_pa=0;
  EXIT;
  return mro;
};


uint64_t rdma_poll_rdma_counter(struct my_context * mcontext,struct rdma_obj_controls * roc);
int rdma_poll_rgets_list(struct my_context * mcontext,struct rdma_obj_controls * roc);

void release_rdma_obj_to_poll(struct mudm_rdma_object * mro, int list_type);
void free_rdma_obj(struct mudm_rdma_object * mro);

int32_t flush_rdma_counter_reqid(struct my_context * mcontext,struct rdma_obj_controls * roc);

#endif //_RDMA_OBJECT_H_
