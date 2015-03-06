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
 * \file algorithms/protocols/tspcoll/Gather.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/Gather.h"
#include "algorithms/protocols/tspcoll/Team.h"


/* **************************************************************** */
/*      start a new alltoall. Old alltoall has to be complete       */
/* **************************************************************** */
template<class T_NI>
void xlpgas::Gather<T_NI>::reset (int root, const void * s, void * d,
            TypeCode           * stype,
            size_t               stypecount,
            TypeCode           * rtype,
            size_t               rtypecount)
{
  _root           = root;
  _rcvcount       = 0;
  _rbuf           = (char *)d;
  _sbuf           = (const char *)s;
  _len            = rtype->GetDataSize() * rtypecount;
  _spwqln         = stype->GetExtent() * stypecount;
  _rpwqln         = rtype->GetExtent() * rtypecount;
  _stype          = stype;
  _rtype          = rtype;
  _sndpwq.configure((char *)this->_sbuf, this->_spwqln, this->_spwqln, NULL, stype);
}

/* **************************************************************** */
/*                   do all the sends in an alltoall                */
/* **************************************************************** */
template<class T_NI>
void xlpgas::Gather<T_NI>::kick    () {
  //printf("ROOT2=%d <> %d", _root, this->ordinal());
  if (_root == this->ordinal())
    {
      memcpy (_rbuf + _root * _len,
	      _sbuf ,
	      _len);
      PAMI_Type_transform_data((char *)_sbuf, _stype, 0, _rbuf + _root * _rpwqln, _rtype,
                               0, _len, PAMI_DATA_COPY, NULL);

      _rcvcount++;
      /* UNLOCK */

      if (this->_cb_complete)
	this->_cb_complete (this->_pami_ctxt,this->_arg, PAMI_SUCCESS);
    }
  else {
    xlpgas_endpoint_t dst = this->_comm->index2Endpoint (this->_root);
    unsigned connection_Id = _header->tag;
//    ((AMHeader&)(_header->hdr)).dest_ctxt = dst.ctxt;
    pami_send_event_t   events;
    events.cookie         = this;
    events.local_fn       = this->cb_senddone;
    events.remote_fn      = NULL;
    this->_p2p_iface->sendPWQ(this->_pami_ctxt, dst,connection_Id,sizeof(*this->_header),this->_header, this->_len, &_sndpwq, &events);
    //this->_p2p_iface->send(&p_send);
    }
}

/* **************************************************************** */
/*                     check whether we are done                    */
/* **************************************************************** */
template<class T_NI>
bool xlpgas::Gather<T_NI>::isdone () const
{
  return (this->_rcvcount >= (int)this->_comm->size());
}

/* **************************************************************** */
/*               send completion in scatter                         */
/* **************************************************************** */
template<class T_NI>
void xlpgas::Gather<T_NI>::cb_senddone (void * ctxt, void * arg, pami_result_t result)
{
  Gather<T_NI> * self = (Gather<T_NI>*)arg;
  /* LOCK */
  TRACE((stderr, "%d: Gather<T_NI>: <%d,%d> SENDDONE sndcount=%d\n",
         XLPGAS_MYNODE, hdr->tag, hdr->kind,
         self->_sndcount[self->_odd]));

  self->_rcvcount = self->_comm->size();
  /* UNLOCK */
  if (self->_cb_complete)
    self->_cb_complete (self->_pami_ctxt, self->_arg, result);
}

/* **************************************************************** */
/*               reception header handler                           */
/* **************************************************************** */
template<class T_NI>
inline void xlpgas::Gather<T_NI>::cb_incoming(pami_context_t          context,
                                              void                  * cookie,
                                              const void            * hdr,
                                              size_t                  header_size,
                                              const void            * pipe_addr,
                                              size_t                  data_size,
                                              pami_endpoint_t         origin,
                                              pami_pwq_recv_t       * recv)
{
  struct AMHeader * header = (struct AMHeader *) hdr;
  //unused:  int ctxt = header->dest_ctxt;
  CollectiveManager<T_NI> *mc = (CollectiveManager<T_NI>*) cookie;
  void * base0 =  mc->find (header->kind, header->tag);


  if (base0 == NULL)
    xlpgas_fatalerror (-1, "%d: Gather<T_NI>/v: <%d,%d> is undefined",
                         XLPGAS_MYNODE, header->tag, header->kind);
  Gather<T_NI> * s = (Gather<T_NI> * ) ((char *)base0 + header->offset);
  TRACE((stderr, "%d: Gather<T_NI>: <%d,%d> INCOMING base=%p ptr=%p len=%d\n",
         XLPGAS_MYNODE, header->tag, header->kind, base0, s, s->_len));

  char * rbuf = s->_rbuf + header->senderID * s->_rpwqln;

  s->_rcvpwq.configure(rbuf, s->_rpwqln, 0, s->_rtype);

  PAMI::PipeWorkQueue * z = &s->_rcvpwq;

  recv->rcvpwq                     = z;
  recv->cb_done.function           = Gather<T_NI>::cb_recvcomplete;
  recv->cb_done.clientdata         = s;
  recv->totalRcvln                 = s->_len;

  return;
}

/* **************************************************************** */
/*           active message reception complete                      */
/* **************************************************************** */
template<class T_NI>
void xlpgas::Gather<T_NI>::cb_recvcomplete (void * unused, void * arg, pami_result_t res)
{
  Gather<T_NI> * self = (Gather<T_NI> *) arg;
  self->_rcvcount++;

  TRACE((stderr, "%d: Gather<T_NI>: <%d,%d> RECVDONE\n",
         XLPGAS_MYNODE, self->_header.tag, self->_header.id));

  if (self->_rcvcount >= (int)self->_comm->size())
    if (self->_cb_complete)
      self->_cb_complete (self->_pami_ctxt, self->_arg, res);
}
