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
 * \file algorithms/protocols/tspcoll/Allreduce.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_Allreduce_h__
#define __algorithms_protocols_tspcoll_Allreduce_h__

#include "algorithms/protocols/tspcoll/CollExchange.h"

namespace xlpgas
{

  namespace Allreduce
    {
      /* ******************************************************************* */
      /*               allreduce execution functions                         */
      /* ******************************************************************* */

      typedef coremath cb_Allreduce_t;
      inline cb_Allreduce_t  getcallback (pami_data_function, TypeCode*);
      inline size_t          datawidthof (xlpgas_dtypes_t);

      /* ******************************************************************* */
      /* ******************************************************************* */
      template <class T_NI>
      class Long: public CollExchange<T_NI>
	{
	public:
	  void * operator new (size_t, void * addr) { return addr; }
	  Long (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset, T_NI*);
          ~Long()
            {
              if(_tmpbuf)
                __global.heap_mm->free(_tmpbuf);
            }
	  void reset (const void * s, void * d,
	              pami_data_function op, TypeCode * sdt, size_t nelems,
	              TypeCode * rdt, user_func_t* uf);

	protected:
	  static void cb_allreduce (CollExchange<T_NI> *, unsigned phase);

	protected:
	  size_t        _nelems;
	  int           _logMaxBF;
	  void        * _dbuf;
      TypeCode    * _sdt;
      TypeCode    * _rdt;
	  coremath      _cb_allreduce;
	  char          _dummy;
	  void        * _tmpbuf;
      void        * _tmpredbuf;
	  size_t        _tmpbuflen;
	  user_func_t*  _uf;
      int           _contig;
	}; /* Long Allreduce */

    template <class T_NI>
    class Short: public CollExchange<T_NI>
    {
    public:
      static const int MAXBUF = 512;
      void * operator new (size_t, void * addr) { return addr; }
      Short (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset,T_NI*);
      void reset (const void        * s,
		  void              * d,
                  pami_data_function  op,
                  TypeCode          * sdt,
                  size_t              nelems,
                  TypeCode          * rdt,
                  user_func_t*        uf);

    protected:
      static PAMI::PipeWorkQueue * cb_switchbuf (CollExchange<T_NI> *, unsigned phase, unsigned counter, size_t data_size);
      static void cb_allreduce (CollExchange<T_NI> *, unsigned phase);

    protected:
      int           _nelems, _logMaxBF;
      void        * _dbuf;
      coremath      _cb_allreduce;
      char          _dummy;
      user_func_t*  _uf;

    protected:
      typedef char PhaseBufType[MAXBUF] __attribute__((__aligned__(16)));
      PhaseBufType  _phasebuf[CollExchange<T_NI>::MAX_PHASES][2];
      int           _bufctr  [CollExchange<T_NI>::MAX_PHASES]; /* 0 or 1 */
      TypeCode     *_rtype;
    }; /* Short Allreduce */

  }//end Allreduce

} /* Xlpgas */

#include "algorithms/protocols/tspcoll/LongAllreduce.cc"
#include "algorithms/protocols/tspcoll/ShortAllreduce.cc"
#include "algorithms/protocols/tspcoll/Allreduce.cc"

#endif /* __xlpgas_Allreduce_h__ */
