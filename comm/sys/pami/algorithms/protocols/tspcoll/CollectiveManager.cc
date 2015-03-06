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
 * \file algorithms/protocols/tspcoll/CollectiveManager.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/Collective.h"

/* ************************************************************************ */
/*            Initialize the collective manager singleton                   */
/* ************************************************************************ */
#ifdef XLPGAS_PAMI_CAU
template <class T_NI>
void xlpgas::CollectiveManager<T_NI>::InitializeLapi (lapi_handle_t handle, xlpgas::CollectiveManager<T_NI>* cmgr)
{
  __global._id_to_collmgr_table[handle] = cmgr;
}
#endif
/* ************************************************************************ */
/*       Collective life cycle manager: constructor                         */
/* ************************************************************************ */
template <class T_NI>
xlpgas::CollectiveManager<T_NI>::CollectiveManager(int ctxt): _ctxt(ctxt)
{
  for (int i=0; i<MAXKIND; i++)
    {
      _kindlist [i] = (CollArray_t *) __global.heap_mm->malloc (sizeof(CollArray_t));
      assert (_kindlist[i] != NULL);
      new (_kindlist[i]) CollArray_t ();
    }
}

/* ************************************************************************ */
/*              find an instance                                            */
/* ************************************************************************ */
template <class T_NI>
xlpgas::Collective<T_NI> *
xlpgas::CollectiveManager<T_NI>::find (CollectiveKind kind, int tag)
{
  assert (0 <= kind && kind < MAXKIND);
  return (*_kindlist[kind])[tag];
}
