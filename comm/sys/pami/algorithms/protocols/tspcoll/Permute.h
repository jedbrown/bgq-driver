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
 * \file algorithms/protocols/tspcoll/Permute.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_Permute_h__
#define __algorithms_protocols_tspcoll_Permute_h__

#include "algorithms/protocols/tspcoll/Team.h"
#include "algorithms/protocols/tspcoll/CollExchange.h"

#undef TRACE
#ifdef DEBUG_COLL
#define TRACE(x)  fprintf x;
#else
#define TRACE(x)
#endif

namespace xlpgas
{
  template <class T_NI>
  class Permute : public Collective<T_NI>
  {
  public:
    void * operator new (size_t, void * addr) { return addr; }

    Permute (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset,T_NI* ni) :
      Collective<T_NI> (ctxt, comm, kind, tag, NULL, NULL, ni)
      {
        _rcvcount = 1;

        _header = (struct AMHeader *)__global.heap_mm->malloc (sizeof(struct AMHeader) );
        assert (_header != NULL);

        _header->hdr.handler   = XLPGAS_TSP_AMSEND_PERM;
        _header->hdr.headerlen = sizeof (struct AMHeader);
        _header->kind          = kind;
        _header->tag           = tag;
        _header->offset        = offset;
        _header->senderID      = this->ordinal();
        _header->dest_ctxt     = -1;
      }

    ~Permute()
      {
        __global.heap_mm->free(_header);
      }

    static  void  amsend_reg       (xlpgas_AMHeaderReg_t amsend_regnum) {
      xlpgas_tsp_amsend_reg (amsend_regnum, Permute::cb_incoming);
    }

    virtual void reset (int dest,
		    const void         * sbuf,
		    void               * dbuf,
		    TypeCode           * stype,
		    size_t               stypecount,
		    TypeCode           * rtype,
		    size_t               rtypecount);

    static void cb_senddone (void * ctxt, void * arg, pami_result_t result);

    virtual void kick    ();
    virtual bool isdone  (void) const;


    static inline void cb_incoming(pami_context_t     context,
                                   void             * cookie,
                                   const void       * header_addr,
                                   size_t             header_size,
                                   const void       * pipe_addr,
                                   size_t             data_size,
                                   pami_endpoint_t    origin,
                                   pami_pwq_recv_t  * recv);

    static void cb_recvcomplete (void * unused, void * arg, pami_result_t result);

  private:
    const char    * _sbuf;         /* send buffer    */
    char          * _rbuf;         /* receive buffer */
    size_t          _len;          /* msg length     */
    size_t          _spwqlen;
    size_t          _rpwqlen;
    size_t          _dest;
    PAMI::PipeWorkQueue  _sndpwq;
    PAMI::PipeWorkQueue  _rcvpwq;
    int             _rcvcount;

    struct AMHeader
    {
      xlpgas_AMHeader_t    hdr;
      CollectiveKind      kind;
      int                 tag;
      int                 offset;
      int                 counter;
      int                 phase;
      int                 senderID;
      int                 dest_ctxt;
    } * _header;

  }; /* Permute */
} /* Xlpgas */

#include "algorithms/protocols/tspcoll/Permute.cc"

#endif /* __xlpgas_Permute_h__ */
