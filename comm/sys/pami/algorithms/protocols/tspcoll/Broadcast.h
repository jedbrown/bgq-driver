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
 * \file algorithms/protocols/tspcoll/Broadcast.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_Broadcast_h__
#define __algorithms_protocols_tspcoll_Broadcast_h__

#include "algorithms/protocols/tspcoll/CollExchange.h"

namespace xlpgas
{
  template <class T_NI>
  class Broadcast: public CollExchange<T_NI>
  {
	public:
	  void * operator new (size_t, void * addr) { return addr; }
	  Broadcast (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset,T_NI*);
	  virtual void reset (int root,
		      const void * sbuf,
		      void * dbuf,
		      TypeCode * type,
		      size_t typecount);

       protected:
	  static void cb_bcast (CollExchange<T_NI> *, unsigned phase);

	protected:
	  int           _logMaxBF;
	  unsigned      _nbytes;
	  void        * _dbuf;
	  void       (* _cb_allreduce) (void *, const void *, unsigned);
	  char          _dummy;
	  void        * _tmpbuf;
	  size_t        _tmpbuflen;
  }; /* Broadcast */
} /* Xlpgas */

#include "algorithms/protocols/tspcoll/Broadcast.cc"

#endif /* __xlpgas_Broadcast_h__ */
