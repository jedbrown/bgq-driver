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
 * \file algorithms/protocols/tspcoll/local/SingleAllreduce.h
 * \brief ???
 */
/* *********************************************************************** */
/*    SMP short Allreduce implementation                                   */
/* *********************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef __algorithms_protocols_tspcoll_local_SingleAllreduce_h__
#define __algorithms_protocols_tspcoll_local_SingleAllreduce_h__

#ifdef XLPGAS_PAMI_CAU

/* *********************************************************************** */
/*             Class definition for shared memory allreduce                */
/* *********************************************************************** */
/* Wait is a class with a function called wait() that progresses the network */
/* T is a type with a size of <= 8 bytes */
/* Op is a class with an operator() that performs the reduction */

namespace xlpgas
{
  namespace local
  {
    template <class Wait, class T, class Op> class SingleAllreduce
    {
    public:
      SingleAllreduce (int nparticipants, int myindex, void * shmem);
      T execute (T val);
      
    public:
      struct State
      {
	volatile union { int64_t d; T t; } value[12][2];        /* 192 bytes */
	volatile int32_t recv_counters[12]; /* rcv counters */  /*  48 bytes */
	int32_t instance_counter;        /* barrier counter  */ /*   4 bytes */
	int32_t filling[3];                                     /*  12 bytes */
      };
      
    private:
      State    * _state;
      int        _me, _logN, _N, _Nhalf;
      static const int BusyWaitCycles=10000;
      
    private:
      void write_notify (int phase, int instanceno, int dest, T val);
      T    wait_read    (int phase, int instanceno) const;
    }; /* SingleAllreduce */
  } /* local */
} /* xlpgas */


/* *********************************************************************** */
/*                   allreduce constructor                                 */
/* *********************************************************************** */

template <class Wait, class T, class Op> 
  inline xlpgas::local::SingleAllreduce<Wait, T, Op>::
  SingleAllreduce (int N, int me, void * shmem): 
  _N(N), _me(me)
{
  for (_logN=0; (1<<_logN)<=_N; _logN++) ; if (_logN>0) _logN--;
  _Nhalf = 1<<(_logN); if (_Nhalf>_N) _Nhalf = _N/2;
  assert (_logN <= 10);

  _state = (State *) shmem;
  memset (_state + me, 0, sizeof(State));

  assert (sizeof(T)<=sizeof(int64_t));

#if 0
  printf ("%d: ALLREDUCE pid=%d N=%d logN=%d Nhalf=%d state=%p\n",
	  _me, getpid(), _N, _logN, _Nhalf, _state);
#endif
}

/* *********************************************************************** */
/*                   execute an allreduce                                  */
/* *********************************************************************** */

template <class Wait, class T, class Op>
  inline T xlpgas::local::SingleAllreduce<Wait, T, Op>::
  execute (T val)
{
  /* ----------------------------------------- */
  /* Increment counter in my state machine     */
  /* ----------------------------------------- */

  unsigned cntr = (++_state[_me].instance_counter);

  /* ------------- */
  /* first phase   */
  /* ------------- */

  if (_me>=_Nhalf) write_notify (0, cntr, _me-_Nhalf, val);
  else if (_me<_N-_Nhalf) { T tmp = wait_read (0, cntr); val = Op()(val,tmp); }

  /* ------------- */
  /* middle phases */
  /* ------------- */

  if (_me<_Nhalf)
    for (int l=0; l<_logN; l++)
      {
	int neighbor = _me^(1<<l);
	write_notify (l+1, cntr, neighbor, val);
	T tmp = wait_read (l+1, cntr); val = Op()(val, tmp);
      }

  /* ------------- */
  /* last phase    */
  /* ------------- */

  if (_me>=_Nhalf) val=wait_read (_logN, cntr);
  else if (_me<_N-_Nhalf) write_notify (_logN, cntr, _me+_Nhalf, val);
  return val;
}

template <class Wait, class T, class Op> inline T 
  xlpgas::local::SingleAllreduce<Wait,T,Op>::
  wait_read (int phase, int cntr) const
{
  volatile int * p1 = & _state[_me].recv_counters[phase];
#pragma nounroll
  for (int z = 0; z<BusyWaitCycles && *p1<cntr ; z++) ;
#pragma nounroll
  for (int z = 0; z<BusyWaitCycles && *p1<cntr ; z++) Wait::wait1();
#pragma nounroll
  for (; *p1<cntr ; ) Wait::wait2();
  __lwsync();
  return _state[_me].value[phase][cntr&1].t;
}

template <class Wait, class T, class Op> inline void 
  xlpgas::local::SingleAllreduce<Wait,T,Op>::
  write_notify (int phase, int cntr, int dest,  T value)
{
  int c = cntr & 1;
  _state[dest].value[phase][c].t = value;
  __lwsync();
  _state[dest].recv_counters[phase] = cntr;
}

#endif // XLPGAS_PAMI_CAU

#endif
