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
 * \file algorithms/protocols/tspcoll/CollExchangeX.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_CollExchangeX_h__
#define __algorithms_protocols_tspcoll_CollExchangeX_h__

#include "algorithms/protocols/tspcoll/Collective.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#define DEBUG_COLLEXCHANGEX 0
#undef TRACE

#ifdef DEBUG_COLLEXCHANGEX
#define TRACE(x)  fprintf x;
#else
#define TRACE(x)
#endif

/* *********************************************************************** */
/*  Base class for implementing a non-blocking collective using s.   */
/* *********************************************************************** */
namespace xlpgas
{
  template<class T_NI, int X> class CollExchangeX: public Collective<T_NI>
  {
  protected:
    static const int MAX_PHASES=64;
    typedef void (* cb_Coll_t) (CollExchangeX *, unsigned);
    typedef xlpgas_local_addr_t (* cb_CollRcv_t) (CollExchangeX *, unsigned, unsigned);
    
  public:  
    /* ------------------------------ */
    /*  public API                    */
    /* ------------------------------ */
    virtual void  kick             (void);
    virtual bool  isdone           () const;
    static  void  amsend_reg       (xlpgas_AMHeaderReg_t amsend_regnum);

  protected:

    CollExchangeX                   (int ctxt,
				    Team *,
				    CollectiveKind, 
				    int tag, 
				    int off, 
				    xlpgas_LCompHandler_t cb_complete=NULL,
				    void * arg = NULL);
    virtual void          reset            (void);


  private:
    
    /* ------------------------------ */
    /*  local functions               */
    /* ------------------------------ */
    
    void          send                     (int phase);
    static xlpgas_local_addr_t cb_incoming    (const xlpgas_AMHeader_t * hdr,
					 xlpgas_LCompHandler_t *,
					 void ** arg);
    static void   cb_recvcomplete          (void*, void *);
    static void   cb_senddone              (void*, void *);
    
  protected:
    
    /* ------------------------------ */
    /* static: set by constructor     */
    /* ------------------------------ */

    int          _numphases;

    /* ------------------------------ */
    /* set by start()                 */
    /* ------------------------------ */
    
    xlpgas_endpoint_t _dest[MAX_PHASES][X]; /* list of destination nodes     */
    void       * _sbuf     [MAX_PHASES];    /* list of source addresses      */
    void       * _rbuf     [MAX_PHASES];    /* list of destination addresses */
    size_t       _sbufln   [MAX_PHASES];    /* list of buffer lenghts        */

    /* --------------------------------- */
    /* STATE: changes during execution   */
    /* --------------------------------- */
    
  protected:
    int          _phase;                    /* phase in current execution    */
    int          _counter;                  /* how many times been reset     */
    int          _sendstarted; 
    int          _sendcomplete;             /* #sends complete               */
    int          _recvcomplete[MAX_PHASES]; /* #recv complete in each phase  */
    
  private:
    
    /* ------------------------------ */
    /*   active message headers       */
    /* ------------------------------ */
    
    struct Header
    {
      xlpgas_AMHeader_t   hdr;
      CollectiveKind      kind;
      int                 tag;
      int                 offset;
      int                 counter;
      int                 phase;
      int                 dest_ctxt;
    }
    _header [MAX_PHASES][X] __attribute__((__aligned__(16)));
    
    /* --------------------------------- */
    /* send & receive completion helper  */
    /* --------------------------------- */
    
    struct CompleteHelper
    {
      int                phase;
      int                counter;
      CollExchangeX     * base;
    }
    _cmplt [MAX_PHASES];

    void internalerror (Header *, int);

    static xlpgas_AMHeaderReg_t _regnum;
  };
}
  
/* *********************************************************************** */
/*                  register collexchange                                  */
/* *********************************************************************** */

template<int X> inline void 
xlpgas::CollExchangeX<X>::amsend_reg  (xlpgas_AMHeaderReg_t amsend_regnum)
{
  _regnum = amsend_regnum;
  xlpgas_tsp_amsend_reg (amsend_regnum, cb_incoming);
}

/* *********************************************************************** */
/*                  CollExchangeX constructor                               */
/* *********************************************************************** */

template<int X> inline xlpgas::CollExchangeX<X>::
CollExchangeX (int ctxt,
	       Team * comm, 
	       CollectiveKind kind, 
	       int tag, 
	       int offset,
	       xlpgas_LCompHandler_t cb_complete,
	       void *arg):
Collective (ctxt, comm, kind, tag, cb_complete, arg)
{
  _counter         = 0;
  _numphases       = -100 * kind;
  _phase           = _numphases+1;
  for (int i=0; i<MAX_PHASES; i++)
    {
      _sbuf[i]                 = NULL;
      _rbuf[i]                 = NULL;
      _sbufln[i]               = 0;
      _cmplt[i].phase          = i;
      _cmplt[i].base           = this;
      _recvcomplete[i]         = 0;
      for (int k=0; k<X; k++)
	{
	  _header[i][k].hdr.handler   = _regnum;
	  _header[i][k].hdr.headerlen = sizeof (struct Header);
	  _header[i][k].kind          = kind;
	  _header[i][k].tag           = tag;
	  _header[i][k].offset        = offset;
	  _header[i][k].phase         = i;
	  _header[i][k].counter       = 0;
	  _header[i][k].dest_ctxt     = -1;
	  _dest[i][k].node            = -1;
	  _dest[i][k].ctxt            = -1;
	}
    }

  _sendstarted = _sendcomplete = 0;
  MUTEX_INIT(&_mutex);
}

/* *********************************************************************** */
/*    reinitialize the state machine for another collective execution      */
/* *********************************************************************** */
template<int X> inline void xlpgas::CollExchangeX<X>::reset()
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

template<int X> inline void xlpgas::CollExchangeX<X>::kick()
{
  for (; _phase < _numphases; _phase++)
    {
      /* ---------------------------------------------------- */
      /* deal with sending what we have to send in this phase */
      /* ---------------------------------------------------- */
      if (_sendstarted <= _phase)
	{
	  _sendstarted++;
	  for (int k=0; k<X; k++) if (_dest[_phase][k].node<0) _sendcomplete++;
	  send (_phase); //send unlocks inside
	}
      
      /* ---------------------------------------------------- */
      /*  we cannot do anything else until send is complete   */
      /* ---------------------------------------------------- */
      if (_sendcomplete <= _phase*X) 
	{
	  MUTEX_UNLOCK(&_mutex);
	  return;
	}
      
      /* ------------------------------------------------------- */
      /* are we waiting to receive any data?                     */
      /* ------------------------------------------------------- */
      if (_rbuf[_phase] == NULL)
	{
	  _recvcomplete[_phase]++;          /* no receive, no callback */
	  assert (_recvcomplete[_phase] <= _counter);
	}
      
      /* ------------------------------------------------------- */
      /*  we are waiting for data to arrive.                     */
      /* ------------------------------------------------------- */
      if (_recvcomplete[_phase] < _counter) 
	{
	  MUTEX_UNLOCK(&_mutex);
	  return;
	}
    }

  if (_cb_complete) 
    if (_phase == _numphases) { _phase++; _cb_complete ((void*)&_ctxt, _arg); }

  MUTEX_UNLOCK(&_mutex);
}

/* *********************************************************************** */
/*    advance the progress engine                                          */
/* *********************************************************************** */

template<int X> inline bool xlpgas::CollExchangeX<X>::isdone() const
{
  
  bool res =  (_phase >= _numphases && _sendcomplete >= _numphases);
  return res;
}

/* *********************************************************************** */
/*                     send an active message                              */
/* *********************************************************************** */

template<int X> inline void xlpgas::CollExchangeX<X>::send (int phase)
{
  MUTEX_UNLOCK(&_mutex);
  assert (_sbuf[phase] != NULL);
  assert (_sbufln[phase] >= 0);
  for (int k=0; k<X; k++)
    {
      if (_dest[phase][k].node < 0) continue;
      _header[phase][k].counter = _counter;
      ((Header&)(_header[phase][k].hdr)).dest_ctxt = _dest[phase][k].ctxt;
      xlpgas_tsp_amsend (_ctxt,
			 _dest[phase][k],
			 & _header[phase][k].hdr,
			 (xlpgas_local_addr_t) _sbuf[phase],
			 _sbufln[phase],
			 CollExchangeX::cb_senddone,
			 &_cmplt[phase]);
    }
  MUTEX_LOCK(&_mutex);
}

/* *********************************************************************** */
/*                             send complete                               */
/* *********************************************************************** */

template<int X> 
inline void xlpgas::CollExchangeX<X>::cb_senddone (void* ctxt, void * arg)
{

  CollExchangeX * base  = ((CompleteHelper *) arg)->base;
  MUTEX_LOCK(&base->_mutex);
  base->_sendcomplete++;
  base->kick();
}

/* *********************************************************************** */
/*                   incoming active message                               */
/* *********************************************************************** */

template<int X> inline xlpgas_local_addr_t xlpgas::CollExchangeX<X>::
cb_incoming (const struct xlpgas_AMHeader_t * hdr,
	     xlpgas_LCompHandler_t * comp_h,
	     void ** arg)
{
  struct Header * header = (struct Header *) hdr;
  int ctxt = header->dest_ctxt; 
  void * b0 =CollectiveManager::instance(ctxt)->find(header->kind,header->tag);

  if (b0 == NULL){
    xlpgas_fatalerror (-1, "%d: incoming: cannot find coll=<%d,%d>",
		      XLPGAS_MYNODE, header->kind, header->tag);
  }

  CollExchangeX * b = (CollExchangeX * ) ((char *)b0 + header->offset);

  MUTEX_LOCK(&b->_mutex);
  assert (b->_header[0][0].tag == header->tag);
  assert (b->_numphases > 0);
  
  b->_cmplt[header->phase].counter = header->counter;
  *comp_h = &CollExchangeX::cb_recvcomplete;
  *arg    = &b->_cmplt[header->phase];

  /* should this be atomic */
  xlpgas_local_addr_t z = (xlpgas_local_addr_t) b->_rbuf[header->phase];
  if (z == NULL) b->internalerror (header, __LINE__);

  MUTEX_UNLOCK(&b->_mutex);
  return z;
}

/* *********************************************************************** */
/*                  active message reception complete                      */
/* *********************************************************************** */

template<int X> inline void 
xlpgas::CollExchangeX<X>::cb_recvcomplete (void* ctxt, void * arg)
{
  CollExchangeX * base  = ((CompleteHelper *) arg)->base;
  unsigned  phase = ((CompleteHelper *) arg)->phase;
  MUTEX_LOCK(&base->_mutex);
  base->_recvcomplete[phase]++;
  if(base->_recvcomplete[phase] <= base->_counter) base->kick();
  else MUTEX_UNLOCK(&base->_mutex);

}

/* *********************************************************************** */
/*      something bad happened. We print the state as best as we can.      */
/* *********************************************************************** */

template<int X> inline void 
xlpgas::CollExchangeX<X>::internalerror (Header * header, int lineno)
{
  if (header)
    fprintf (stdout, "%d: CollExchangeX internal: line=%d "
	     "tag=%d id=%d phase=%d/%d ctr=%d "
	     "header: tag=%d id=%d phase=%d ctr=%d\n",
	     XLPGAS_MYNODE, lineno,
             _kind, _tag, 
	     _phase, _numphases, _counter,
	     header->kind, header->tag, header->phase,
	     header->counter);
  else
    fprintf (stdout, "%d: CollExchangeX internal: line=%d "
	     "tag=%d id=%d phase=%d/%d ctr=%d\n",
	     XLPGAS_MYNODE, lineno,
	     _kind, _tag,
	     _phase, _numphases, _counter);
  abort();
}

#undef TRACE
#endif
