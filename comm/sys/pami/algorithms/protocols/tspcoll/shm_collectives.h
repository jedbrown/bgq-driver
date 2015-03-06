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
 * \file algorithms/protocols/tspcoll/shm_collectives.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_shm_collectives_h__
#define __algorithms_protocols_tspcoll_shm_collectives_h__
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sched.h>


#include "algorithms/protocols/tspcoll/local/SingleAllreduce.h"
#include "algorithms/protocols/tspcoll/local/FixedLeader.h"

namespace xlpgas{

  template <class T_NI>
  struct Wait
  {
    static void wait2() {
      PAMI_assert(0);
      //printf("replace with dev->advance\n");
      //xlpgas::CollectiveManager<T_NI>::instance(0)->device()->advance();
    }
    static void wait1() { sched_yield(); }
  };

  struct IntPlus
  {
    int operator()(const int a, const int b) const { return a+b; }
  };


  void shm_init(int);

}//end namespace xlpgas
#endif
