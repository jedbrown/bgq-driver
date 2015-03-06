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
 * \file algorithms/protocols/tspcoll/QuadnomBcast.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_QuadnomBcast_h__
#define __algorithms_protocols_tspcoll_QuadnomBcast_h__

#if 0
#include "algorithms/protocols/tspcoll/CollExchangeX.h"

namespace xlpgas
{
  template <class T_NI>
  class QuadnomBcast: public CollExchangeX<T_NI, 3>
  {
  public:
    void * operator new (size_t, void * addr) { return addr; }
    QuadnomBcast (int ctxt, 
		  Team * comm, 
		  CollectiveKind kind, int tag, int offset);
    virtual void reset (int root,
			const void * sbuf,
			void * dbuf,
			unsigned nbytes);
    
  protected:
    int _logN, _qN;
    char          _dummy;
  }; /* QuadnomBcast */
} /* xlpgas */

#include "algorithms/protocols/tspcoll/QuadnomBcast.cc"
#endif

#endif /* __xlpgas_QuadnomBcast_h__ */
