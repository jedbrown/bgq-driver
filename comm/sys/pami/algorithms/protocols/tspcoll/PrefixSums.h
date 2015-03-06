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
 * \file algorithms/protocols/tspcoll/PrefixSums.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_PrefixSums_h__
#define __algorithms_protocols_tspcoll_PrefixSums_h__

#include "algorithms/protocols/tspcoll/CollExchange.h"
#include "algorithms/protocols/tspcoll/Allreduce.h"

namespace xlpgas
{
  template <class T_NI>
  class PrefixSums: public CollExchange<T_NI>
  {
	public:
      void * operator new (size_t, void * addr) { return addr; }
      PrefixSums (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset, T_NI*);
          ~PrefixSums()
            {
              if(_tmpbuf)
                __global.heap_mm->free(_tmpbuf);
            }

      void reset (const void         * sbuf,
              void               * dbuf,
              pami_data_function   op,
              TypeCode           * sdt,
              TypeCode           * rdt,
              size_t               nelems);
      static void cb_prefixsums (CollExchange<T_NI> *coll, unsigned phase);

      void setExclusive(int exclusive) { _exclusive = exclusive; }

	protected:
	  int           _logMaxBF;
	  size_t        _nelems;
	  void        * _dbuf;
	  char          _dummy;
	  void        * _tmpbuf;
      void        * _tmpredbuf;
	  size_t        _tmpbuflen;
	  coremath      _cb_prefixsums;
	  user_func_t * _uf;
	  TypeCode    * _sdt;
	  TypeCode    * _rdt;
      int           _contig;
	  int           _exclusive; // 0 = Inclusive scan, 1 = Exclusive scan

  }; /* PrefixSums */
} /* Xlpgas */

#include "algorithms/protocols/tspcoll/PrefixSums.cc"

#endif /* __xlpgas_PrefixSums_h__ */
