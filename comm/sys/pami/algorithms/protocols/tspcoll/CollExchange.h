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
 * \file algorithms/protocols/tspcoll/CollExchange.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_CollExchange_h__
#define __algorithms_protocols_tspcoll_CollExchange_h__

#include "algorithms/protocols/tspcoll/Collective.h"
#include "common/NativeInterface.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>

#include "util/trace.h"

/*#define DEBUG_COLLEXCHANGE 0*/
#undef TRACE
#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#ifdef DEBUG_COLLEXCHANGE
 #define TRACE(x)  fprintf x;
 #define DO_TRACE_ENTEREXIT 1
 #define DO_TRACE_DEBUG     1
#else
 #define TRACE(x)
 #define DO_TRACE_ENTEREXIT 0
 #define DO_TRACE_DEBUG     0
#endif



/* *********************************************************************** */
/*  Base class for implementing a non-blocking collective using s.   */
/* *********************************************************************** */
namespace xlpgas
{
  template <class T_NI>
  class CollExchange: public Collective<T_NI>
  {
  protected:
    static const int MAX_PHASES=64;
    typedef void (* cb_Coll_t) (CollExchange *, unsigned);
    typedef PAMI::PipeWorkQueue * (* cb_CollRcv_t) (CollExchange *, unsigned, unsigned, size_t);

  public:
    /* ------------------------------ */
    /*  public API                    */
    /* ------------------------------ */
    virtual void  kick             ();
    virtual bool  isdone           () const;
    static  void  amsend_reg       (xlpgas_AMHeaderReg_t amsend_regnum);

  protected:

    CollExchange                   (int ctxt,
				    Team *,
				    CollectiveKind,
				    int tag,
				    int off,
                                    T_NI* ni,
				    xlpgas_LCompHandler_t cb_complete=NULL,
				    void * arg = NULL);
    virtual void          reset            (void);


  private:

    /* ------------------------------ */
    /*  local functions               */
    /* ------------------------------ */

    void          send                     (int phase);
    static void   cb_recvcomplete          (void*, void *, pami_result_t);
    static void   cb_senddone              (void*, void *, pami_result_t);
  public:
    static inline void cb_incoming(pami_context_t         context,
                                   void                 * cookie,
                                   const void           * header_addr,
                                   size_t                 header_size,
                                   const void           * pipe_addr,
                                   size_t                 data_size,
                                   pami_endpoint_t        origin,
                                   pami_pwq_recv_t      * recv);

  protected:

    /* ------------------------------ */
    /* static: set by constructor     */
    /* ------------------------------ */

    int          _numphases;

    /* ------------------------------ */
    /* set by start()                 */
    /* ------------------------------ */

    xlpgas_endpoint_t   _dest     [MAX_PHASES];    /* list of destination nodes     */
    void              * _sbuf     [MAX_PHASES];    /* list of source addresses      */
    void              * _rbuf     [MAX_PHASES];    /* list of destination addresses */
    size_t              _sbufln   [MAX_PHASES];    /* list of send buffer lenghts. This is byte length only. */
    size_t              _rbufln   [MAX_PHASES];    /* list of recv buffer lenghts. This is byte length only. Used on recv to check for msg complete*/
    size_t              _spwqln   [MAX_PHASES];    /* list of send pwq buffer lenths. This include bytes+strides if Applicable */
    size_t              _rpwqln   [MAX_PHASES];    /* list of recv pwq buffer lenths. This include bytes+strides if Applicable */
    cb_CollRcv_t        _cb_rcvhdr[MAX_PHASES];    /* what to do before reception; adjust reception buffers    */
    cb_Coll_t           _postrcv  [MAX_PHASES];    /* what to do after reception    */
    PAMI::PipeWorkQueue _sndpwq   [MAX_PHASES];    /* pwq used by sendPWQ to support non-contigous dt */
    PAMI::PipeWorkQueue _rcvpwq   [MAX_PHASES];    /* pwq used by recvPWQ (sendPWQ dispatch) to support non-contigous dt */

    /* --------------------------------- */
    /* STATE: changes during execution   */
    /* --------------------------------- */

  protected:
    int          _phase;                    /* phase in current execution    */
    int          _counter;                  /* how many times been reset     */
    int          _sendstarted;
    int          _sendcomplete;             /* #sends complete               */
    int          _recvcomplete[MAX_PHASES]; /* #recv complete in each phase  */

    /* ------------------------------ */
    /*   active message headers       */
    /* ------------------------------ */

    struct AMHeader
    {
      xlpgas_AMHeader_t   hdr;
      CollectiveKind      kind;
      int                 tag;
      int                 offset;
      int                 counter;
      int                 phase;
      int                 dest_ctxt;
    }
    _header [MAX_PHASES] __attribute__((__aligned__(16)));

    /* --------------------------------- */
    /* send & receive completion helper  */
    /* --------------------------------- */

    struct CompleteHelper
    {
      int                phase;
      int                counter;
      CollExchange     * base;
    }
    _cmplt [MAX_PHASES];

    void internalerror (AMHeader *, int);

  };
}

// clang is stricter than gcc, and performs syntax checks for all code, even not instantianated.
// It seems PAMI never instantianate templates that depends on xlpgas_tsp_amsend_reg
// These templates seems to be dead code for PAMI
typedef void FAKEPARAM(pami_context_t, void *, const void *, size_t, const void *, size_t, pami_endpoint_t, pami_pwq_recv_t *);
inline void xlpgas_tsp_amsend_reg (xlpgas_AMHeaderReg_t, FAKEPARAM)
{
  assert(0);
}

/* *********************************************************************** */
/*                  register collexchange                                  */
/* *********************************************************************** */
template <class T_NI>
inline void xlpgas::CollExchange<T_NI>::amsend_reg  (xlpgas_AMHeaderReg_t amsend_regnum)
{
  xlpgas_tsp_amsend_reg (amsend_regnum, cb_incoming);
}

/* *********************************************************************** */
/*                  CollExchange constructor                               */
/* *********************************************************************** */
template <class T_NI>
inline xlpgas::CollExchange<T_NI>::
CollExchange (int ctxt,
	      Team * comm,
	      CollectiveKind kind,
	      int tag,
	      int offset,
              T_NI* ni,
	      xlpgas_LCompHandler_t cb_complete,
	      void *arg):
  Collective<T_NI> (ctxt, comm, kind, tag, cb_complete, arg, ni)
{
  _counter         = 0;
  _numphases       = -100 * kind;
  _phase           = _numphases+1;
  for (int i=0; i<MAX_PHASES; i++)
    {
      _sbuf[i]                 = NULL;
      _rbuf[i]                 = NULL;
      _sbufln[i]               = 0;
      _rbufln[i]               = 0;
      _spwqln[i]               = 0;
      _rpwqln[i]               = 0;
      _postrcv[i]              = NULL;
      _cb_rcvhdr[i]            = NULL;
      _cmplt[i].phase          = i;
      _cmplt[i].base           = this;
      _recvcomplete[i]         = 0;
//      _header[i].hdr.handler   = _regnum;
//      _header[i].hdr.headerlen = sizeof (struct AMHeader);
      _header[i].kind          = kind;
      _header[i].tag           = tag;
      _header[i].offset        = offset;
      _header[i].phase         = i;
      _header[i].counter       = 0;
      _header[i].dest_ctxt     = -1;
    }

  _sendstarted = _sendcomplete = 0;
  MUTEX_INIT(&_mutex);
}

/* *********************************************************************** */
/*    reinitialize the state machine for another collective execution      */
/* *********************************************************************** */
template <class T_NI>
inline void xlpgas::CollExchange<T_NI>::reset()
{
  //printf("L%d reset lock\n",XLPGAS_MYNODE);
  MUTEX_LOCK(&_mutex);
  _sendstarted = _sendcomplete = 0;
  _counter++;
  _phase = 0;
}

/* *********************************************************************** */
/*                   kick the state machine (make progress)                */
/* *********************************************************************** */
template <class T_NI>
inline void xlpgas::CollExchange<T_NI>::kick()
{

  for (; _phase < _numphases; _phase++)
    {
      /* ---------------------------------------------------- */
      /* deal with sending what we have to send in this phase */
      /* ---------------------------------------------------- */
      if (_sendstarted <= _phase)
      {
        _sendstarted++;
        if (_sbuf[_phase]) {
          int phase = _phase;
          send (phase); //send unlocks inside
          return;
        }
        else _sendcomplete++;
      }

      /* ---------------------------------------------------- */
      /*  we cannot do anything else until send is complete   */
      /* ---------------------------------------------------- */
      if (_sendcomplete <= _phase) {
        MUTEX_UNLOCK(&_mutex);
        return;
      }
      /* ------------------------------------------------------- */
      /* are we waiting to receive any data?                     */
      /* if no, advance immediately                              */
      /* ------------------------------------------------------- */
      if (_rbuf[_phase] == NULL)
      {
        _recvcomplete[_phase]++;          /* no receive, no callback */
        assert (_recvcomplete[_phase] <= _counter);
	  //continue;
      }

      /* ------------------------------------------------------- */
      /*  we are waiting for data to arrive.                     */
      /* ------------------------------------------------------- */
      if (_recvcomplete[_phase] < _counter) {
        MUTEX_UNLOCK(&_mutex);
        return;
      }

      if (this->_postrcv[_phase]) {
	//printf ("L%d --------> ADD \n",XLPGAS_MYNODE);
        this->_postrcv[_phase](this, _phase);
      }
    }
  TRACE((stderr, "%d:%p: FINI tag=%d ctr=%d phase=%d/%d sendcmplt=%d\n",
	 XLPGAS_MYNODE, this->_pami_ctxt, _header[_phase].tag, _counter,
	 _phase, _numphases, _sendcomplete));
  if (this->_cb_complete)
    if (_phase == _numphases) { _phase++; this->_cb_complete (this->_pami_ctxt, this->_arg, PAMI_SUCCESS); }

    MUTEX_UNLOCK(&_mutex);
}

/* *********************************************************************** */
/*    advance the progress engine                                          */
/* *********************************************************************** */
template <class T_NI>
inline bool xlpgas::CollExchange<T_NI>::isdone() const
{

  bool res =  (_phase >= _numphases && _sendcomplete >= _numphases);
  return res;
}

/* *********************************************************************** */
/*                     send an active message                              */
/* *********************************************************************** */
template <class T_NI>
inline void xlpgas::CollExchange<T_NI>::send (int phase)
{
  TRACE((stderr,"L%d:%p SEND tag=%d ctr=%d phase=%d tgt=%d nbytes=%zu  sbuf=%p sbufln=%zu numphases=%d  PHASE inside _cplt=%d and inside _headers=%d\n",
          this->rank(), this->_pami_ctxt, _header[_phase].tag, _counter, phase,
	 _dest[phase], _sbufln[phase], _sbuf[phase],
         _sbufln[phase],_numphases,_cmplt[phase].phase, _header[phase].phase ));
  _header[phase].counter       = _counter;

  //We need connection Id to be unique. We are using geometryId here (tag)
  unsigned connection_Id = _header[_phase].tag;


  MUTEX_UNLOCK(&_mutex);

  assert (_sbuf[phase] != NULL);

  pami_send_event_t   events;
  events.cookie           = &_cmplt[phase];
  events.local_fn         = CollExchange::cb_senddone;
  events.remote_fn        = NULL;
  this->_p2p_iface->sendPWQ(this->_pami_ctxt, _dest[phase], connection_Id, sizeof(_header[phase]),&_header[phase],_sbufln[phase], &_sndpwq[phase], &events);
  //this->_p2p_iface->send(&s);

}

/* *********************************************************************** */
/*                             send complete                               */
/* *********************************************************************** */
template <class T_NI>
inline void xlpgas::CollExchange<T_NI>::cb_senddone (void* ctxt, void * arg, pami_result_t result)
{

  CollExchange * base  = ((CompleteHelper *) arg)->base;
  MUTEX_LOCK(&base->_mutex);
  /* BEGIN ATOMIC */
  TRACE((stderr,"L%d SEND_DONE ctr=%d phase=%d nphases=%d \n",
	 XLPGAS_MYNODE, base->_counter,
	 base->_phase, base->_numphases));

  base->_sendcomplete++;
  base->kick();
}

/* *********************************************************************** */
/*                   incoming active message                               */
/* *********************************************************************** */
template<class T_NI>
inline void xlpgas::CollExchange<T_NI>::cb_incoming(pami_context_t          context,
                                                    void                  * cookie,
                                                    const void            * hdr,
                                                    size_t                  header_size,
                                                    const void            * pipe_addr,
                                                    size_t                  data_size,
                                                    pami_endpoint_t         origin,
                                                    pami_pwq_recv_t       * recv)
{
  TRACE_FN_ENTER();

  struct AMHeader * header = (struct AMHeader *) hdr;
  // unused:  int ctxt = header->dest_ctxt;
  CollectiveManager<T_NI> *mc = (CollectiveManager<T_NI>*) cookie;
  void * base0 = mc->find (header->kind,header->tag);

  if (base0 == NULL){
    xlpgas_fatalerror (-1, "%d: incoming: cannot find coll=<%d,%d>",
		      XLPGAS_MYNODE, header->kind, header->tag);
  }

  CollExchange * b = (CollExchange * ) ((char *)base0 + header->offset);


  MUTEX_LOCK(&b->_mutex);

  TRACE((stderr, "L%d I%d :%d INC kind=%d tag=%d ctr=%d phase=%d nphases=%d "
         "msgctr=%d msgphase=%d\n",
         b->ordinal(), b->rank(), b->_ctxt,header->kind, header->tag, b->_counter,
         b->_phase, b->_numphases,
        header->counter, header->phase));

  assert (b->_header[0].tag == header->tag);
  assert (b->_numphases > 0);

  PAMI::PipeWorkQueue * z = &(b->_rcvpwq[header->phase]);
  //adjust if user cb function present
  if (b->_cb_rcvhdr[header->phase]){
    z = (PAMI::PipeWorkQueue *)b->_cb_rcvhdr[header->phase](b, header->phase, header->counter, data_size);
  }
  if (z == NULL) {
    b->internalerror (header, __LINE__);
  }

  recv->rcvpwq                     = z;
  b->_cmplt[header->phase].counter = header->counter;
  recv->cb_done.function           = CollExchange::cb_recvcomplete;
  recv->cb_done.clientdata         = &b->_cmplt[header->phase];
  recv->totalRcvln                 = b->_rbufln[header->phase];

  MUTEX_UNLOCK(&b->_mutex);
  TRACE_FN_EXIT();
  return;
}

/* *********************************************************************** */
/*                  active message reception complete                      */
/* *********************************************************************** */
template <class T_NI>
inline void
xlpgas::CollExchange<T_NI>::cb_recvcomplete (void* ctxt, void * arg, pami_result_t result)
{
  CollExchange * base  = ((CompleteHelper *) arg)->base;
  unsigned  phase = ((CompleteHelper *) arg)->phase;

  /* The execution of post-receive if done inside kick depending if
     the we are in the right phase of the computation */

  TRACE((stderr, "L%d I%d ctxt:%d CPLT phase=%d [counter=%d recvcplt=%d] \n",
          base->ordinal(), base->rank(), base->_ctxt, phase, base->_counter, base->_recvcomplete[phase]));

  /* BEGIN ATOMIC */
  MUTEX_LOCK(&base->_mutex);
  base->_recvcomplete[phase]++;
  if(base->_recvcomplete[phase] <= base->_counter) base->kick();
  else {MUTEX_UNLOCK(&base->_mutex);}

}

/* *********************************************************************** */
/*      something bad happened. We print the state as best as we can.      */
/* *********************************************************************** */
template <class T_NI>
inline void
xlpgas::CollExchange<T_NI>::internalerror (AMHeader * header, int lineno)
{
  if (header)
    fprintf (stdout, "%d: CollExchange internal: line=%d "
	     "tag=%d id=%d phase=%d/%d ctr=%d "
	     "header: tag=%d id=%d phase=%d ctr=%d\n",
	     this->rank(), lineno,
             this->_kind, this->_tag,
	     _phase, _numphases, _counter,
	     header->kind, header->tag, header->phase,
	     header->counter);
  else
    fprintf (stdout, "%d: CollExchange internal: line=%d "
	     "tag=%d id=%d phase=%d/%d ctr=%d\n",
	     this->rank(), lineno,
	     this->_kind, this->_tag,
	     _phase, _numphases, _counter);
  abort();
}

#undef TRACE
#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG
#endif
