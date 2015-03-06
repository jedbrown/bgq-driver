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
 * \file algorithms/protocols/tspcoll/Barrier.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_Barrier_h__
#define __algorithms_protocols_tspcoll_Barrier_h__

#include "algorithms/protocols/tspcoll/CollExchange.h"
#include "algorithms/protocols/tspcoll/Team.h"

#undef TRACE
//#define DEBUG_TSPCOLL_BARRIER 0
#ifdef DEBUG_TSPCOLL_BARRIER
#define TRACE(x)  fprintf x;
#else
#define TRACE(x)
#endif

/* *********************************************************************** */
/*                 Pairwise exchange barrier                               */
/* *********************************************************************** */

namespace xlpgas
{
  template <class T_NI>
  class Barrier: public CollExchange<T_NI>
  {
  public:
    void * operator new (size_t, void * addr)    { return addr; }
    Barrier (int                 ctxt,
	     Team              * comm,
	     CollectiveKind      kind,
	     int                 tag,
	     int                 offset,
             T_NI              * ni);
    void reset () { CollExchange<T_NI>::reset(); }
  private:
    char        _dummy;
  };
}

/* *********************************************************************** */
/*                 constructor                                             */
/* *********************************************************************** */
template <class T_NI>
inline xlpgas::Barrier<T_NI>::Barrier (int               ctxt,
                                       Team            * comm,
                                       CollectiveKind    kind,
                                       int               tag,
                                       int               offset,
                                       T_NI             *ni) :
  CollExchange<T_NI> (ctxt, comm, kind, tag, offset, ni)
{
  TRACE((stderr, "%d: Barrier constructor: rank=%d of %d\n",
	 XLPGAS_MYNODE, this->_comm->rank(), this->_comm->size()));

  this->_numphases = -1; for (int n=2*this->_comm->size()-1; n>0; n>>=1) this->_numphases++;
  this->_sendcomplete = this->_numphases;
  this->_phase        = this->_numphases;

  /* --------------------------------------------------- */
  /* initialize destinations, offsets and buffer lengths */
  /* --------------------------------------------------- */

    for (int i=0; i<this->_numphases; i++)
    {
      this->_dest[i]      = comm->index2Endpoint((this->ordinal()+(1<<i))%comm->size());
      TRACE((stderr, "%d: Barrier constructor: dest[%d]=%d \n",XLPGAS_MYNODE,i ,this->_dest[i]));

      this->_sbuf[i]      = &this->_dummy;
      this->_sbufln[i]    = this->_rbufln[i] = 1;
      this->_sndpwq[i].configure((char *)this->_sbuf[i], this->_sbufln[i], this->_sbufln[i]);
      this->_rbuf[i]      = &this->_dummy;
      this->_rcvpwq[i].configure((char *)this->_rbuf[i], this->_rbufln[i], 0);
    }
}

#endif
