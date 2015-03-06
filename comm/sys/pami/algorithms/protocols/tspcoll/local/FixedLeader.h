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
/* *********************************************************************** */
/* *********************************************************************** */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
//#include <builtins.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef __algorithms_protocols_tspcoll_local_FixedLeader_h__
#define __algorithms_protocols_tspcoll_local_FixedLeader_h__

#include "algorithms/protocols/tspcoll/cau_collectives.h" //for ops on one element

#undef TRACE
//#define TRACE_FL
#ifdef TRACE_FL
#define TRACE(x) fprintf x
#else
#define TRACE(x)
#endif

/* *********************************************************************** */
/*           Fixed-leader broadcast and reduction                          */
/* *********************************************************************** */
/* Wait is a class with a function called wait() that progresses the network */
/* T is a type with a size of <= 8 bytes */
/* Op is a class with an operator() that performs the reduction */

#define SHM_BUF_SIZE 524288
#define BWC_REDUCE   500
#define BWC_LG_BCAST 100000

namespace xlpgas
{
  namespace local
  {
    template <class Wait> 
    class FixedLeader
    {
    public:
      FixedLeader (int N, int myindex, int leader, void * shmem
,  int nc=2);
      FixedLeader(int N, int myindex, int leader, int nchildren) : _N(N), _me(myindex), _nchildren(nchildren),_leader(leader) {}
      void bcast (xlpgas_local_addr_t buffer, size_t len);
      void reduce (int64_t* val, int64_t* dest, const cau_reduce_op_t&, size_t nelems);
      void reset(int leader);
      int root() const {return this->_leader;}
      bool isdone           () const;
      void set_undone(){_done=false;}
      void setComplete (xlpgas_LCompHandler_t cb,
			void *arg,
			int ctxt) { this->_cb_complete = cb; this->_arg = arg; this->_ctxt=ctxt;}
      void setContext (pami_context_t ctxt) {_pami_ctxt=ctxt;}

    public:
      typedef _State State;

    protected:
      State*     _state;
      int        _N, _me, _nchildren;
      int        _children[128], _parent, _leader;
      int        BusyWaitCycles;
      bool _done;
      //handle cb
      xlpgas_LCompHandler_t     _cb_complete;
      void*                     _arg;
      int                       _ctxt;
      pami_context_t            _pami_ctxt;

    protected:
      void wait (int dest, int counter) const;
      bool wait1 (int dest, int counter) const;
    public:
      bool haveParent   () const { return (_parent>=0); }
    }; /* FixedLeader */


    /* Fixed leader; one buffer; large messages pipelined*/
    template <class Wait>
    class FixedLeaderLB : public FixedLeader<Wait> {
      typedef FixedLeader<Wait> base;
      char* _large_buffer;
    public:
      FixedLeaderLB (int N, int myindex, int leader, pgas_shm_buffers& bufs, int nc=128);
      void bcast (xlpgas_local_addr_t buffer, size_t len);
    };
    
    /******************************************************/
    /* Fixed leader; two buffers; large messages pipelined*/
    template <class Wait>
    class FixedLeader2LB : public FixedLeader<Wait> {
      typedef FixedLeader<Wait> base;
      char* _large_buffer0;
      char* _large_buffer1;
    public:
      FixedLeader2LB (int N, int myindex, int leader, pgas_shm_buffers& bufs, int nc=128);
      void bcast (xlpgas_local_addr_t buffer, size_t len);
    };
    
  } /* local */
} /* xlpgas */


/* *********************************************************************** */
/*                   allreduce constructor                                 */
/* *********************************************************************** */

template <class Wait> 
inline xlpgas::local::FixedLeader<Wait>::
FixedLeader (int N, int me, int leader, void * sh_mem, int nchildren):
_N(N), _me(me), _nchildren(nchildren)
{
  assert (nchildren <= 32);
  this->BusyWaitCycles = BWC_REDUCE;
  int k = (me-leader+N) %N;  // my distance from leader, modulo N

  for (int c=0; c<this->_nchildren; c++)
    if (this->_nchildren*k+c+1<N) this->_children[c] = (this->_nchildren*k+c+1+leader)%N;
    else                    this->_children[c] = -1;
    
  /* parent: (k+1)/this->_nchildren from leader */
  if (k>0) _parent = ((k-1)/this->_nchildren+leader)%N; else _parent = -1;

  this->_state = (State *) sh_mem;
  _done = true;
  
  //memory is set to zero when allocated; reset to zero is a bug
  // when sub teams are created; they will wipe out whatever was here;
  //subteams are guarded by another if and they won't call hybrid collectives
  //memset ((void *)(this->_state + me), 0, sizeof(State));
#if 0
  fprintf (stderr, "%d: PID=%d k=%d parent=%d  shmem=%p\n", me, getpid(), k, _parent,sh_mem);
#endif
}

template <class Wait> 
void inline xlpgas::local::FixedLeader<Wait>::reset (int leader)
{
  _leader = leader;
 int k = (this->_me - _leader + _N) % _N;  // my distance from leader, modulo N
  for (int c=0; c<this->_nchildren; c++)
    if (this->_nchildren*k+c+1<_N) this->_children[c] = (this->_nchildren*k+c+1+leader) % _N;
    else                    this->_children[c] = -1;
    
  /* parent: (k+1)/this->_nchildren from leader */
  if (k>0) _parent = ((k-1)/this->_nchildren + _leader) % _N; else _parent = -1;
}

/* *********************************************************************** */
/*                     reduce (blocking)                                   */
/* *********************************************************************** */
template <class Wait>
void xlpgas::local::FixedLeader<Wait>::reduce (int64_t* val, 
					       int64_t* dest, 
					       const cau_reduce_op_t& op,
					       size_t nelems)
{
  /* ------------------------------------------------------------- */
  /* wait for children to have data                                */
  /* perform operations using children's values                    */
  /* ------------------------------------------------------------- */
  bool all = true;
  for (int c=0; c<this->_nchildren; c++){
    if (this->_children[c]>=0) {
      bool wait_i = wait1 (this->_children[c], this->_state[this->_me].counter+1);
      all = all && wait_i;
    }
  }
  if(!all) return;
  for (int c=0; c<this->_nchildren; c++) {
    if (this->_children[c]>=0){
      reduce_op(val, (int64_t *) this->_state[this->_children[c]].buffer, op, nelems);
    }
  }

  /* ------------------------------------------------------------- */
  /* wait for parent to have read my *previous* posted value.      */
  /* and then post my new result                                   */
  /* ------------------------------------------------------------- */
  if(_parent != -1)
    all = wait1 (_parent, this->_state[this->_me].counter);
  if(!all) return;

  //* (volatile int64_t *) this->_state[this->_me].buffer = *val;
  memcpy(this->_state[this->_me].buffer, val, nelems*sizeof(int64_t));
  /* ------------------------------------------------------- */
  /* put out notice that I have data for this iteration      */
  /* ------------------------------------------------------- */

  __lwsync(); /* write barrier */
  volatile int32_t * p = (volatile int32_t *) &this->_state[this->_me].counter;
  (*p) = (*p)+1;
  __lwsync(); /* write barrier */

  memcpy(dest, val, nelems*sizeof(int64_t));
  _done = true;
  if (this->_cb_complete)
    _cb_complete ((void*)_pami_ctxt, _arg, PAMI_SUCCESS);
}


template <class Wait>
bool xlpgas::local::FixedLeader<Wait>::isdone () const {
  return _done;
}

/* *********************************************************************** */
/*                     broadcast (blocking)                                */
/* *********************************************************************** */
template <class Wait>
  void xlpgas::local::FixedLeader<Wait>::
  bcast (xlpgas_local_addr_t buf, size_t len)
{


  /* ------------------------------------------------------------------ */
  /* wait until both children say they have data for previous iteration */
  /* ------------------------------------------------------------------ */
  bool all = true;
  for (int c=0; c<this->_nchildren; c++) {
    if (this->_children[c]>=0) {
      bool wait_i = wait1 (this->_children[c], this->_state[this->_me].counter);
      all = all && wait_i;
    }
  }

  if(!all) return;
  /* -------------------------------------------------- */
  /* wait for my parent to have data for this iteration */
  /* -------------------------------------------------- */
  if(_parent != -1)
    all = wait1 (_parent, this->_state[this->_me].counter+1);

  if(!all) return;

  /* ------------------------------------------------------- */
  /* copy data from my parent (or from input if I am leader) */
  /* ------------------------------------------------------- */
  __lwsync(); /* read barrier */
  memcpy ((void *)(&this->_state[this->_me].buffer),
	  haveParent()?(void *)this->_state[_parent].buffer:buf, len);
  __lwsync(); /* write barrier */

  /* ------------------------------------------------------- */
  /* put out notice that I have data for this iteration      */
  /* ------------------------------------------------------- */
  volatile int32_t * p = (volatile int32_t *) &this->_state[this->_me].counter;
  (*p) = (*p) + 1;
  __lwsync(); /* write barrier */

  /* ------------------------------------------------------- */
  /*   copy data to output buffer                            */
  /* ------------------------------------------------------- */
  if (haveParent()) {
    memcpy (buf, (void *)(&this->_state[this->_me].buffer), len);
  }
  _done = true;
  if (this->_cb_complete)
    _cb_complete ((void*)_pami_ctxt, _arg, PAMI_SUCCESS);

}

/* *********************************************************************** */
/*              waiting for announcement                                   */
/* *********************************************************************** */

template <class Wait> inline bool
  xlpgas::local::FixedLeader<Wait>::
  wait1 (int dest, int cntr) const
{
  if (dest<0) return true;
  volatile int * p1 = & this->_state[dest].counter;
#pragma nounroll
  for (int z = 0; z<BusyWaitCycles && *p1<cntr ; z++) ;

  if(*p1 < cntr)
    return false;
  else
    return true;

  //#pragma nounroll
  // yield cause some performance loss in mpp all2all benchmark
  //  for (int z = 0; z<BusyWaitCycles && *p1<cntr ; z++) Wait::wait1();
#pragma nounroll
  for (; *p1<cntr ; ) Wait::wait2();

  return true;
}


template <class Wait> inline void
  xlpgas::local::FixedLeader<Wait>::
  wait (int dest, int cntr) const
{
  if (dest<0) return;
  volatile int * p1 = & this->_state[dest].counter;
#pragma nounroll
  for (int z = 0; z<BusyWaitCycles && *p1<cntr ; z++) ;
  //#pragma nounroll
  // yield cause some performance loss in mpp all2all benchmark
  //  for (int z = 0; z<BusyWaitCycles && *p1<cntr ; z++) Wait::wait1();
#pragma nounroll
  for (; *p1<cntr ; ) Wait::wait2();
}


template <class Wait> 
inline xlpgas::local::FixedLeaderLB<Wait>::
 FixedLeaderLB (int N, int me, int leader, pgas_shm_buffers& bufs, int nchildren) : xlpgas::local::FixedLeader<Wait>(N,me,leader,nchildren){
  assert (nchildren <= 128);
  this->BusyWaitCycles = BWC_LG_BCAST;
  int k = (me-leader+N) % N;  // my distance from leader, modulo N
  for (int c=0; c<this->_nchildren; c++)
    if (this->_nchildren*k+c+1<N) this->_children[c] = (this->_nchildren*k+c+1+leader)%N;
    else                    this->_children[c] = -1;
    
  /* parent: (k+1)/this->_nchildren from leader */
  if (k>0) this->_parent = ((k-1)/this->_nchildren+leader)%N; else this->_parent = -1;
  typedef typename xlpgas::local::FixedLeaderLB<Wait>::base::State State;
  this->_state = (State *) bufs._large_bcast_buf;
  _large_buffer = (char*)(bufs._large_bcast_data_buf);
}

/* *********************************************************************** */
/*                     large message broadcast (blocking)                  */
/* *********************************************************************** */
template <class Wait>
void xlpgas::local::FixedLeaderLB<Wait>::
  bcast (xlpgas_local_addr_t buf, size_t len)
{
  /* ------------------------------------------------------------------ */
  /* wait until both children say they have data for previous iteration */
  /* ------------------------------------------------------------------ */
  bool all = true;
  for (int c=0; c<this->_nchildren; c++) {
    if (this->_children[c]>=0) {
      bool wait_i = base::wait1 (this->_children[c], this->_state[this->_me].counter);
      all = all && wait_i;
    }
  }

  if(!all) return;
  /* -------------------------------------------------- */
  /* wait for my parent to have data for this iteration */
  /* -------------------------------------------------- */
  if(this->_parent != -1)
    all = base::wait1 (this->_parent, this->_state[this->_me].counter+1);

  if(!all) return;
  /* ------------------------------------------------------- */
  /* copy data from my parent (or from input if I am leader) */
  /* ------------------------------------------------------- */
  __lwsync(); /* read barrier */
  if(! this->haveParent()) memcpy ((void *)(_large_buffer), (void *)buf, len);
  __lwsync(); /* write barrier */


  /* ------------------------------------------------------- */
  /*   children copy data to output buffer                            */
  /* ------------------------------------------------------- */
  if (this->haveParent()) {
    memcpy (buf, (void *)(_large_buffer), len);
  }

  /* ------------------------------------------------------- */
  /* put out notice that I have data for this iteration      */
  /* ------------------------------------------------------- */
  volatile int32_t * p = (volatile int32_t *) &this->_state[this->_me].counter;
  (*p) = (*p) + 1;
  __lwsync(); /* write barrier */

  
  this->_done = true;
  if (this->_cb_complete) 
    this->_cb_complete ((void*)&(this->_ctxt),this-> _arg, PAMI_SUCCESS);
}

/* *********************************************************************** */
/*                     large message broadcast with two buffers (blocking)                  */
/* *********************************************************************** */
template <class Wait> 
inline xlpgas::local::FixedLeader2LB<Wait>::
 FixedLeader2LB (int N, int me, int leader, pgas_shm_buffers& bufs, int nchildren) : xlpgas::local::FixedLeader<Wait>(N,me,leader,nchildren){
  assert (nchildren <= 128);
  this->BusyWaitCycles = BWC_LG_BCAST;
  int k = (me-leader+N) % N;  // my distance from leader, modulo N
  for (int c=0; c<this->_nchildren; c++)
    if (this->_nchildren*k+c+1<N) this->_children[c] = (this->_nchildren*k+c+1+leader)%N;
    else                    this->_children[c] = -1;
    
  /* parent: (k+1)/this->_nchildren from leader */
  if (k>0) this->_parent = ((k-1)/this->_nchildren+leader)%N; else this->_parent = -1;
  typedef typename xlpgas::local::FixedLeader2LB<Wait>::base::State State;
  this->_state = (State *) bufs._large_bcast_buf;
  _large_buffer0 = (char*)(bufs._large_bcast_data_buf);
  _large_buffer1 = (char*)(bufs._large_bcast_data_buf) + bufs._bcast_buf_sz / 2;
}

template <class Wait>
void xlpgas::local::FixedLeader2LB<Wait>::
  bcast (xlpgas_local_addr_t buf, size_t len)
{
  /* ------------------------------------------------------------------ */
  /* wait until both children say they have data for previous iteration */
  /* ------------------------------------------------------------------ */
  //for (int c=0; c<this->_nchildren; c++) 
  //  base::wait (this->_children[c], this->_state[this->_me].counter-1);
  bool all = true;
  for (int c=0; c<this->_nchildren; c++) {
    if (this->_children[c]>=0) {
      bool wait_i = base::wait1 (this->_children[c], this->_state[this->_me].counter-1);
      all = all && wait_i;
    }
  }

  if(!all) return;

  /* -------------------------------------------------- */
  /* wait for my parent to have data for this iteration */
  /* -------------------------------------------------- */
  //base::wait (this->_parent, this->_state[this->_me].counter+1);
  if(this->_parent != -1)
    all = base::wait1 (this->_parent, this->_state[this->_me].counter+1);

  if(!all) return;

  /* ------------------------------------------------------- */
  /* copy data from my parent (or from input if I am leader) */
  /* ------------------------------------------------------- */
  __lwsync(); /* read barrier */
  if(! this->haveParent()) {
    if((this->_state[this->_me].counter & 1) == 0 )
      memcpy ((void *)(_large_buffer0), (void *)buf, len);
    else
      memcpy ((void *)(_large_buffer1), (void *)buf, len);
  }
  __lwsync(); /* write barrier */

  /* ------------------------------------------------------- */
  /*   children copy data to output buffer                            */
  /* ------------------------------------------------------- */
  __isync();
  if (this->haveParent()) {
    if((this->_state[this->_me].counter & 1) == 0 )
      memcpy (buf, (void *)(_large_buffer0), len);
    else
      memcpy (buf, (void *)(_large_buffer1), len);
  }

  /* ------------------------------------------------------- */
  /* put out notice that I have data for this iteration      */
  /* ------------------------------------------------------- */
  volatile int32_t * p = (volatile int32_t *) &this->_state[this->_me].counter;
  (*p) = (*p) + 1;
  __lwsync(); /* write barrier */

  this->_done = true;
  if (this->_cb_complete) 
    this->_cb_complete ((void*)&(this->_ctxt),this-> _arg, PAMI_SUCCESS);
}

#endif
