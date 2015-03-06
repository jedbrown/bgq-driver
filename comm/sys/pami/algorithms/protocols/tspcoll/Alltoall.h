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
 * \file algorithms/protocols/tspcoll/Alltoall.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_Alltoall_h__
#define __algorithms_protocols_tspcoll_Alltoall_h__

#include "algorithms/protocols/tspcoll/Team.h"
#include "algorithms/protocols/tspcoll/CollExchange.h"

#undef TRACE
#ifdef DEBUG_COLL
#define TRACE(x)  fprintf x;
#else
#define TRACE(x)
#endif


#ifndef  PAMI_PLATFORM_A2A_PACING_WINDOW
  #define MAX_PENDING 1024
#else
  #define MAX_PENDING PAMI_PLATFORM_A2A_PACING_WINDOW
#endif

typedef struct
{
  void *me;
  void *pwq;
  void *next;
}alltoall_cookie_t;

typedef struct
{
  PAMI::PipeWorkQueue  pwq;
  void                *next;
} pwq_list_t;

namespace xlpgas
{
  template <class T_NI>
  class Alltoall : public Collective<T_NI>
  {
  public:

    void * operator new (size_t, void * addr) { return addr; }

    Alltoall (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset, T_NI* ni) :
      Collective<T_NI> (ctxt, comm, kind, tag, NULL, NULL, ni),
      _offset(offset)
      {
        _sndcount[0] = comm->size(); _sndcount[1] = comm->size();
        _rcvcount[0] = comm->size(); _rcvcount[1] = comm->size();
        _odd = 1;

        _header.hdr.handler    = XLPGAS_TSP_AMSEND_COLLA2A;
        _header.hdr.headerlen = sizeof (struct AMHeader);
        _header.kind          = this->_kind;
        _header.tag           = this->_tag;
        _header.offset        = _offset;
        _header.senderID      = this->ordinal();
        _sndpwq               = NULL;
      }

    ~Alltoall()
      {
          if(_sndpwq != NULL)
          {
            __global.heap_mm->free(_sndpwq);
            __global.heap_mm->free(_a2ascookie);
            unsigned i = 0;
            while(_rcvpwqList[i])
            {
              __global.heap_mm->free(_rcvpwqList[i]);
              i++;
            }
            __global.heap_mm->free(_rcvpwqList);
            i = 0;
            while(_a2arcookieList[i])
            {
              __global.heap_mm->free(_a2arcookieList[i]);
              i++;
            }
            __global.heap_mm->free(_a2arcookieList);
          }
      }

    static  void  amsend_reg       (xlpgas_AMHeaderReg_t amsend_regnum) {
      xlpgas_tsp_amsend_reg (amsend_regnum, Alltoall::cb_incoming);
    }

    virtual void reset (const void * sbuf,
			void               * dbuf,
			TypeCode           * stype,
			size_t               stypecount,
			TypeCode           * rtype,
			size_t               rtypecount);

    static void cb_senddone (void * ctxt, void * arg, pami_result_t result);

    virtual void kick    ();
    void kick_internal   ();
    void print_info(char*) const ;
    virtual bool isdone  (void) const;
    bool buffer_full  (void) const;
    bool all_sent  (void) const;

    static inline void cb_incoming(pami_context_t          context,
                                   void                  * cookie,
                                   const void            * header_addr,
                                   size_t                  header_size,
                                   const void            * pipe_addr,
                                   size_t                  data_size,
                                   pami_endpoint_t         origin,
                                   pami_pwq_recv_t       * recv);

    static void cb_recvcomplete (void * unused, void * arg, pami_result_t result);

  protected:
    const char          * _sbuf;         /* send buffer    */
    char                * _rbuf;         /* receive buffer */
    size_t                _len;          /* msg length     */
    size_t                _spwqlen;      /* msg send pwq length     */
    size_t                _rpwqlen;      /* msg recv pwq length     */
    TypeCode            * _stype;        /* Single datatype of the send buffer */
    TypeCode            * _rtype;        /* Single datatype of the recv buffer */
    pwq_list_t          * _sndpwq;       /* A list of send pwqs. We get "free" pwqs from this list and return them when done */
    pwq_list_t          * _sndpwqFL;     /* A pointer to next free sndpwq */
    alltoall_cookie_t   * _a2ascookie;   /* A place holder for object and send pwq to put back to free list */
    alltoall_cookie_t   * _a2ascookieFL; /* A pointer to next free a2ascookie */
    pwq_list_t         ** _rcvpwqList;   /* A list of lists of rcvpwqs*/
    pwq_list_t          * _rcvpwqFL;     /* A pointer to next free rcvpwq */
    alltoall_cookie_t  ** _a2arcookieList;/* A list of lists of a2arcookie */
    alltoall_cookie_t   * _a2arcookieFL; /* A pointer to next free a2arcookie */

    pami_work_t           _work_pami;     /* work to be reposted if not finished */
    int                   _sndcount[2], _sndstartedcount[2], _rcvcount[2], _odd, _offset;
    size_t                _current;
    int                   _in_place;

    struct AMHeader
    {
      xlpgas_AMHeader_t    hdr;
      CollectiveKind      kind;
      int                 tag;
      int                 offset;
      int                 counter;
      int                 phase;
      int                 dest_ctxt;
      int senderID;
    }  _header;

  }; /* Alltoall */
} /* Xlpgas */

#include "algorithms/protocols/tspcoll/Alltoall.cc"

#endif /* __xlpgas_Alltoall_h__ */
