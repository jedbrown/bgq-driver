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
 * \file algorithms/protocols/tspcoll/Alltoallv.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_Alltoallv_h__
#define __algorithms_protocols_tspcoll_Alltoallv_h__

#include "algorithms/protocols/tspcoll/Alltoall.h"

#undef TRACE
#ifdef DEBUG_COLL
#define TRACE(x)  fprintf x;
#else
#define TRACE(x)
#endif

namespace xlpgas
{
  template <class T_NI, class CntType>
  class Alltoallv : public Alltoall<T_NI>
  {
  public:
    void * operator new (size_t, void * addr) { return addr; }

    Alltoallv (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset, T_NI* ni) :
      Alltoall<T_NI> (ctxt, comm, kind, tag, offset,ni)
      {
      }

    virtual void reset (const void   *sbuf,
			void *        dbuf,
			TypeCode     *stype,
			const CntType *scnts,
			const CntType *sdispls,
			TypeCode     *rtype,
			const CntType *rcnts,
			const CntType *rdispls);

    virtual void kick    ();
    void kick_internal   ();

    static  void  amsend_reg       (xlpgas_AMHeaderReg_t amsend_regnum) {
      xlpgas_tsp_amsend_reg (amsend_regnum, Alltoallv::cb_incoming_v);
    }

    static inline void cb_incoming_v(pami_context_t          context,
                                     void                  * cookie,
                                     const void            * header_addr,
                                     size_t                  header_size,
                                     const void            * pipe_addr,
                                     size_t                  data_size,
                                     pami_endpoint_t         origin,
                                     pami_pwq_recv_t       * recv);

  private:
    const CntType    * _scnts;      /* send counts    */
    const CntType    * _sdispls;    /* send displacements */
    const CntType    * _rcnts;      /* recv counts    */
    const CntType    * _rdispls;    /* recv displacements */
    int                _in_place;
  }; /* Alltoallv */
} /* Xlpgas */

#include "algorithms/protocols/tspcoll/Alltoallv.cc"

#endif /* __xlpgas_Alltoallv_h__ */
