/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file algorithms/protocols/tspcoll/ShmCauAllReduce.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/Allreduce.h"
#include "algorithms/protocols/tspcoll/ShmCauAllReduce.h"
#include "algorithms/protocols/tspcoll/Team.h"

#include "algorithms/protocols/tspcoll/cau_collectives.h"

template <class T_NI>
void next_phase (void* ctxt, void * arg, pami_result_t){
  xlpgas::Collective<T_NI>* b = (xlpgas::Collective<T_NI>*)arg;
  b->kick();
}

template <class T_NI, class T_Device>
void xlpgas::ShmCauAllReduce<T_NI,T_Device>::kick (){
  shm_reduce->kick();
}

template <class T_NI, class T_Device>
bool xlpgas::ShmCauAllReduce<T_NI,T_Device>::isdone () const {
  if(shm_bcast==NULL) return true;
  else return shm_bcast->isdone();
}

template <class T_NI, class T_Device>
void xlpgas::ShmCauAllReduce<T_NI,T_Device>::setContext (pami_context_t ctxt) {
  this->_pami_ctxt=ctxt;
  shm_reduce->setContext(ctxt);
  shm_bcast->setContext(ctxt);
  if(cau_reduce!=NULL) cau_reduce->setContext(ctxt);
  if(cau_bcast!=NULL)  cau_bcast->setContext(ctxt);
}

template <class T_NI, class T_Device>
void xlpgas::ShmCauAllReduce<T_NI,T_Device>::setComplete (xlpgas_LCompHandler_t cb,
							  void *arg) {
  shm_bcast->setComplete(cb,arg);
}

template <class T_NI, class T_Device>
void xlpgas::ShmCauAllReduce<T_NI,T_Device>::reset (const void         * sbuf,
                                                    void               * rbuf,
                                                    pami_data_function   op,
                                                    TypeCode           * sdt,
                                                    unsigned           nelems,
                                                    TypeCode           * rdt,
                                                    user_func_t        * uf
				     ) {
  //with CAU we can reduce up to to 64 bytes of data; 
  //the data will be processes in group of 64 bit so a max of only 8 elements
  assert(nelems <= 8);
  TypeCode           * lsdt = sdt;
  if(sbuf == PAMI_IN_PLACE)
  {
    sbuf = rbuf;
    lsdt = rdt;
  }
  uintptr_t i_op, i_dt;
  PAMI::Type::TypeFunc::GetEnums((void*)lsdt,
                                 ( void (*)(void*, void*, size_t, void*) )op,
                                 i_dt,i_op);

  bool finish_early=false;//if shared memory only the collective ends without bcast;
  if(local_team->size() == team->size()) finish_early = true;
  
  //maybe add here a macro to memset and memcpy
  memcpy(&s[0], sbuf, nelems * lsdt->GetDataSize() );
  memset(&tmp[0],     0, nelems * lsdt->GetDataSize());
  memset(&tmp_cau[0], 0, nelems * lsdt->GetDataSize());

  int leader = 0; //ordinal zero in the current group
  
  //allocate shm bcast
  assert (shm_bcast != NULL);
  shm_bcast->reset (leader, &tmp_cau[0], rbuf, nelems * lsdt->GetDataSize() );
  //allocate shm reduce
  assert (shm_reduce != NULL);
  if(!finish_early) {
    shm_reduce->reset (leader, &s[0], &tmp[0], (pami_op)i_op, (pami_dt)i_dt, nelems, uf);
  }
  else {
    shm_reduce->reset (leader, &s[0], &tmp_cau[0], (pami_op)i_op, (pami_dt)i_dt, nelems, uf);
  }

  if(!finish_early){
    if(this->_is_leader){
      //allocate caureduce
      assert (cau_reduce != NULL);
      cau_reduce->reset (leader, &tmp[0], &tmp_cau[0], (pami_op)i_op, (pami_dt)i_dt, nelems, uf);
      shm_reduce->setComplete(&next_phase<T_NI>,cau_reduce);

      //allocate cau bcast
      assert (cau_bcast != NULL);
      cau_bcast->reset (leader, &tmp_cau[0], &tmp_cau[0], nelems * lsdt->GetDataSize() );
      cau_reduce->setComplete(&next_phase<T_NI>,cau_bcast);

      cau_bcast->setComplete(&next_phase<T_NI>,shm_bcast);
    }
    else {
      shm_reduce->setComplete(&next_phase<T_NI>,shm_bcast);
    }
  } else {
    shm_reduce->setComplete(&next_phase<T_NI>,shm_bcast);
  }
}
