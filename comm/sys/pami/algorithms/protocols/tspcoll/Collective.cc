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
 * \file algorithms/protocols/tspcoll/Collective.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/Collective.h"
#include "algorithms/protocols/tspcoll/Team.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>


/* ************************************************************************ */
/*                 Collective constructor                                   */
/* ************************************************************************ */
template<class T_NI>
xlpgas::Collective<T_NI>::Collective (int                     ctxt,
                                      Team                  * comm,
                                      CollectiveKind          kind,
                                      int                     tag,
                                      xlpgas_LCompHandler_t    cb_complete,
                                      void                  * arg,
                                      T_NI                  * ni):
  _ctxt        (ctxt),
  _comm        (comm),
  _kind        (kind),
  _tag         (tag),
  _cb_complete (cb_complete),
  _arg         (arg),
  _p2p_iface   (ni)
{
  _my_rank  = _p2p_iface->endpoint();
  _my_index = _comm->endpoint2Index(_my_rank);
  _dev = NULL;
  _is_leader = false;
}

/* ************************************************************************ */
/*                     Collective factory                                   */
/* ************************************************************************ */


/* ************************************************************************ */
/*                    initialize the factory                                */
/* ************************************************************************ */
template<class T_NI>
void xlpgas::Collective<T_NI>::Initialize ()
{
#if 0
  xlpgas::CollExchange::amsend_reg ();
  xlpgas::Scatter::amsend_reg ();
  xlpgas::Alltoall::amsend_reg ();
  xlpgas::Alltoallv::amsend_reg ();
  // xlpgas::Gather::amsend_reg ();
#endif
}
