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
 * \file algorithms/protocols/tspcoll/Permute.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/Permute.h"
#include "algorithms/protocols/tspcoll/Team.h"


/* **************************************************************** */
/*      start a new permute.                                        */
/* **************************************************************** */


/* TO be fixed when copy in place; race condition */
template <class T_NI>
void xlpgas::Permute<T_NI>::reset (int dest, const void * s, void * d,
		    TypeCode           * stype,
		    size_t               stypecount,
		    TypeCode           * rtype,
		    size_t               rtypecount)
{
  _dest           = dest;
  _rcvcount       = 0;
  _rbuf           = (char *)d;
  _sbuf           = (const char *)s;
  _len            = stype->GetDataSize() * stypecount;
  _spwqlen        = stype->GetExtent()   * stypecount;
  _rpwqlen        = rtype->GetExtent()   * rtypecount;
  _sndpwq.configure((char *)this->_sbuf, this->_spwqlen, this->_spwqlen, NULL, stype);
  _rcvpwq.configure((char *)this->_rbuf, this->_rpwqlen, 0, rtype);
}

/* **************************************************************** */
/*                   do all the sends in an alltoall                */
/* **************************************************************** */
template <class T_NI>
void xlpgas::Permute<T_NI>::kick    () {
  if (this->_dest == this->ordinal())
    {
      memcpy (this->_rbuf ,
	      this->_sbuf ,
	      this->_len);

      this->_rcvcount++;
      /* UNLOCK */

      if (this->_cb_complete)
	this->_cb_complete (this->_pami_ctxt,this->_arg, PAMI_SUCCESS);
    }
  else {
    xlpgas_endpoint_t dst = this->_comm->index2Endpoint (this->_dest);
//    ((AMHeader&)(_header->hdr)).dest_ctxt = dst.ctxt;
    pami_send_t p_send;
    pami_send_event_t   events;
    p_send.send.header.iov_base  = this->_header;
    p_send.send.header.iov_len   = sizeof(*this->_header);
    p_send.send.data.iov_base    = (char*)this->_sbuf;
    p_send.send.data.iov_len     = this->_len;
    p_send.send.dispatch         = -1;
    memset(&p_send.send.hints, 0, sizeof(p_send.send.hints));
    p_send.send.dest             = dst;
    events.cookie         = this;
    events.local_fn       = this->cb_senddone;
    events.remote_fn      = NULL;
    this->_p2p_iface->sendPWQ(this->_pami_ctxt, dst, 0, sizeof(*this->_header), this->_header,this->_len, &_sndpwq, &events);
    //this->_p2p_iface->send(&p_send);
    }
}

/* **************************************************************** */
/*                     check whether we are done                    */
/* **************************************************************** */
template <class T_NI>
bool xlpgas::Permute<T_NI>::isdone () const
{
  return (_rcvcount >= 1);
}

/* **************************************************************** */
/*               send completion in permute                         */
/* **************************************************************** */
template <class T_NI>
void xlpgas::Permute<T_NI>::cb_senddone (void * ctxt, void * arg, pami_result_t res)
{
  Permute * self = (Permute*)arg;
  /* LOCK */
  TRACE((stderr, "%d: Permute: <%d,%d> SENDDONE sndcount=%d\n",
         XLPGAS_MYNODE, hdr->tag, hdr->kind,
         self->_sndcount[self->_odd]));

  /* UNLOCK */
  if (self->_cb_complete)
    self->_cb_complete (self->_pami_ctxt, self->_arg, res);
}

/* **************************************************************** */
/*               reception header handler                           */
/* **************************************************************** */

template<class T_NI>
inline void xlpgas::Permute<T_NI>::cb_incoming(pami_context_t    context,
                                               void            * cookie,
                                               const void      * hdr,
                                               size_t            header_size,
                                               const void      * pipe_addr,
                                               size_t            data_size,
                                               pami_endpoint_t   origin,
                                               pami_pwq_recv_t * recv)
{
  struct AMHeader * header = (struct AMHeader *) hdr;

  int ctxt = header->dest_ctxt;
  CollectiveManager<T_NI> *mc = (CollectiveManager<T_NI>*) cookie;
  void * base0 =  mc->find (header->kind, header->tag);

  if (base0 == NULL)
    xlpgas_fatalerror (-1, "%d: Permute/v: <%d,%d> is undefined",
                         XLPGAS_MYNODE, header->tag, header->kind);
  Permute * s = (Permute * ) ((char *)base0 + header->offset);
  TRACE((stderr, "%d: Permute: <%d,%d> INCOMING base=%p ptr=%p len=%d\n",
         XLPGAS_MYNODE, header->tag, header->kind, base0, s, s->_len));

  PAMI::PipeWorkQueue * z = &s->_rcvpwq;
  recv->rcvpwq                     = z;
  recv->cb_done.function           = Permute<T_NI>::cb_recvcomplete;
  recv->cb_done.clientdata         = s;
  recv->totalRcvln                 = s->_len;
  return;
}

/* **************************************************************** */
/*           active message reception complete                      */
/* **************************************************************** */
template <class T_NI>
void xlpgas::Permute<T_NI>::cb_recvcomplete (void * unused, void * arg, pami_result_t res)
{
  Permute * self = (Permute *) arg;
  self->_rcvcount++;

  TRACE((stderr, "%d: Permute: <%d,%d> RECVDONE\n",
         XLPGAS_MYNODE, self->_header.tag, self->_header.id));

  if (self->_rcvcount >= 1)
    if (self->_cb_complete)
      self->_cb_complete (self->_pami_ctxt, self->_arg, res);
}
