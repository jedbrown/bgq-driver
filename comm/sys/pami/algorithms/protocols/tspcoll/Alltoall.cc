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
 * \file algorithms/protocols/tspcoll/Alltoall.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/Alltoall.h"
#include "algorithms/protocols/tspcoll/Team.h"

/* **************************************************************** */
/*      start a new alltoall. Old alltoall has to be complete       */
/* **************************************************************** */
template<class T_NI>
void xlpgas::Alltoall<T_NI>::reset (const void        * s,
                                          void        * d,
                                          TypeCode    * stype,
                                          size_t        stypecount,
                                          TypeCode    * rtype,
                                          size_t        rtypecount)
{
  MUTEX_LOCK(&this->_mutex);
  _odd            = (!_odd);
  _sndcount[_odd] = 0;
  _sndstartedcount[_odd] = 0;
  _rcvcount[_odd] = 0;
  _rbuf           = (char *)d;
  _sbuf           = (const char *)s;
  _len            = rtype->GetDataSize() * rtypecount;
  _spwqlen        = stype->GetExtent() * stypecount;
  _rpwqlen        = rtype->GetExtent() * rtypecount;
  _stype          = stype;
  _rtype          = rtype;
  _current        = this->ordinal();
  _in_place       = 0;
  if(_sndpwq == NULL)
  {
    unsigned slot_size = (this->_comm->size()<MAX_PENDING)?this->_comm->size():MAX_PENDING;
    unsigned i = 0;
    _sndpwq   = (pwq_list_t *)__global.heap_mm->malloc (sizeof(pwq_list_t)* slot_size);
    _sndpwqFL = NULL;
    for(i=0;i<slot_size;i++) {
      (_sndpwq+i)->next = _sndpwqFL;
      _sndpwqFL = (_sndpwq + i);
    }
    _a2ascookie   = (alltoall_cookie_t *)__global.heap_mm->malloc (sizeof(alltoall_cookie_t)* slot_size);
    _a2ascookieFL = NULL;
    for(i=0;i<slot_size;i++) {
      (_a2ascookie+i)->next = _a2ascookieFL;
      _a2ascookieFL = (_a2ascookie + i);
    }
    _rcvpwqList = (pwq_list_t **)__global.heap_mm->malloc (sizeof(pwq_list_t*)* slot_size);
    memset(_rcvpwqList, 0, sizeof(pwq_list_t*)*slot_size);
    _a2arcookieList = (alltoall_cookie_t **)__global.heap_mm->malloc (sizeof(alltoall_cookie_t*)* slot_size);
    memset(_a2arcookieList, 0, sizeof(alltoall_cookie_t*)*slot_size);
    pwq_list_t *rcvpwqs = (pwq_list_t *)__global.heap_mm->malloc (sizeof(pwq_list_t)* slot_size);
    _rcvpwqFL = NULL;
    for(i=0;i<slot_size;i++) {
      (rcvpwqs+i)->next = _rcvpwqFL;
      _rcvpwqFL = (rcvpwqs + i);
    }
    alltoall_cookie_t *a2arcookies   = (alltoall_cookie_t *)__global.heap_mm->malloc (sizeof(alltoall_cookie_t)* slot_size);
    _a2arcookieFL = NULL;
    for(i=0;i<slot_size;i++) {
      (a2arcookies+i)->next = _a2arcookieFL;
      _a2arcookieFL = (a2arcookies + i);
    }
    _rcvpwqList[0]    = rcvpwqs;
    _a2arcookieList[0] = a2arcookies;
  }

  if(s == PAMI_IN_PLACE)
  {
    _in_place      = 1;
    size_t datalen = _rpwqlen * this->_comm->size();
    _sbuf    = (char *)__global.heap_mm->malloc (datalen);
    PAMI_assert_alwaysf(_sbuf != NULL, "Error allocating memory in %s %d",__FILE__,__LINE__);
    _spwqlen = _rpwqlen;
    _stype   = _rtype;
    memcpy((void *)_sbuf, _rbuf, datalen); 
  }
  MUTEX_UNLOCK(&this->_mutex);
}

/* **************************************************************** */
/*                   do all the sends in an alltoall                */
/* **************************************************************** */
template<class T_NI>
void xlpgas::Alltoall<T_NI>::kick_internal    () {
  MUTEX_LOCK(&this->_mutex);
  // send a message to all members of the geometry; this can be
  // potentially expensive in terms of run time and resources
  // allocated
  int j = _sndstartedcount[_odd];
  int csize=(int)this->_comm->size();
  for (; j < csize; j++) {
    if (buffer_full()) {
      MUTEX_UNLOCK(&this->_mutex);
      break;
    }
    if (_current == this->ordinal())
    {
       char * sbuf = (char*)_sbuf + _current * _spwqlen;
       char * rbuf = (char*)_rbuf + _current * _rpwqlen;
       PAMI_Type_transform_data((void*)sbuf, _stype, 0,
                                             rbuf, _rtype, 0, _len,
                                             PAMI_DATA_COPY, NULL);

	  _sndcount[_odd]++;
      _sndstartedcount[_odd]++;
	  _rcvcount[_odd]++;
	  /* UNLOCK */
	  MUTEX_UNLOCK(&this->_mutex);
      if (this->isdone())
      {
        if(_in_place)__global.heap_mm->free((void*)this->_sbuf);
        if (this->_cb_complete)
            this->_cb_complete (this->_pami_ctxt,this->_arg,PAMI_SUCCESS);
      }
    }
    else {
      pwq_list_t *sndpwq = _sndpwqFL;
      PAMI_assert_alwaysf(sndpwq != NULL, "Ran out of free sendpwqs in %s %d",__FILE__,__LINE__);
      _sndpwqFL = (pwq_list_t*)sndpwq->next;

      alltoall_cookie_t *a2ascookie = _a2ascookieFL;
      PAMI_assert_alwaysf(a2ascookie != NULL, "Ran out of alltoall send cookies in %s %d",__FILE__,__LINE__);
      _a2ascookieFL = (alltoall_cookie_t*)a2ascookie->next;

      MUTEX_UNLOCK(&this->_mutex);
      a2ascookie->me  = this;
      a2ascookie->pwq = sndpwq;
      _sndstartedcount[_odd]++;
      unsigned connection_Id = _header.tag;
      pami_send_event_t   events;
      events.cookie          = a2ascookie;
      events.local_fn        = this->cb_senddone;
      events.remote_fn       = NULL;
      sndpwq->pwq.configure((char *)_sbuf + _current * _spwqlen, this->_spwqlen, this->_spwqlen, NULL, _stype);
      this->_p2p_iface->sendPWQ(this->_pami_ctxt, this->_comm->index2Endpoint (_current), connection_Id, sizeof(_header),&_header,this->_len, &sndpwq->pwq, &events);
    }

    // increment current wrapping arround
    _current += 1;
    if((int)_current == csize)
      _current = 0;
  }
}

/*
  Alltoall pushes a certain number of messages and waits
 */
template<class T_NI>
pami_result_t repost_all2all_function (pami_context_t context, void *cookie) {
  xlpgas::Alltoall<T_NI>* coll = (xlpgas::Alltoall<T_NI>*)cookie;
  coll->kick_internal();
  if( coll->all_sent() ) {
    return PAMI_SUCCESS;
  }
  else {
    return PAMI_EAGAIN;
  }
}

template<class T_NI>
void xlpgas::Alltoall<T_NI>::kick    () {
  this->kick_internal();
  if( ! this->all_sent() ) {
    //repost if not all messages sent due to buffer full
    PAMI::Device::Generic::GenericThread *work = new ((void*)(&_work_pami)) PAMI::Device::Generic::GenericThread(repost_all2all_function<T_NI>, (void*)this);
    this->_dev[0].postThread(work);
  }
}

/* **************************************************************** */
/*                     check whether we are done                    */
/* **************************************************************** */
template<class T_NI>
bool xlpgas::Alltoall<T_NI>::isdone () const
{
  return (this->_sndcount[this->_odd] == this->_sndstartedcount[this->_odd] &&
	  this->_sndcount[this->_odd] >= (int)this->_comm->size() &&
          this->_rcvcount[this->_odd] >= (int)this->_comm->size());
}

template<class T_NI>
bool xlpgas::Alltoall<T_NI>::buffer_full () const
{
  return ( (size_t)(this->_sndstartedcount[this->_odd] - this->_sndcount[this->_odd]) >= MAX_PENDING);
}

template<class T_NI>
bool xlpgas::Alltoall<T_NI>::all_sent () const
{
  return ( this->_sndstartedcount[this->_odd] >= (int)this->_comm->size());
}
/* **************************************************************** */
/*               send completion in alltoall                        */
/* **************************************************************** */
template<class T_NI>
void xlpgas::Alltoall<T_NI>::cb_senddone (void * ctxt, void * arg, pami_result_t res)
{
  alltoall_cookie_t * a2ascookie = (alltoall_cookie_t*)arg;
  
  Alltoall<T_NI>    * self       = (Alltoall<T_NI>*)a2ascookie->me;
  pwq_list_t        * sndpwq     = (pwq_list_t*)a2ascookie->pwq;
  /* LOCK */
  MUTEX_LOCK(&self->_mutex);
  TRACE((stderr, "%d: ALLTOALL: <%d,%d> SENDDONE sndcount=%d\n",
         XLPGAS_MYNODE, hdr->tag, hdr->kind,
         self->_sndcount[self->_odd]));
 
  sndpwq->next = self->_sndpwqFL;
  self->_sndpwqFL = sndpwq;

  a2ascookie->next = self->_a2ascookieFL;
  self->_a2ascookieFL = a2ascookie;

  self->_sndcount[self->_odd]++;

  /* UNLOCK */
  MUTEX_UNLOCK(&self->_mutex);
  if (self->isdone())
  {
    if(self->_in_place)__global.heap_mm->free((void*)self->_sbuf);
    if (self->_cb_complete)
      self->_cb_complete (self->_pami_ctxt, self->_arg, res);
  }
}

/* **************************************************************** */
/*               reception header handler                           */
/* **************************************************************** */
template<class T_NI>
inline void xlpgas::Alltoall<T_NI>::cb_incoming(pami_context_t          context,
                                                void                  * cookie,
                                                const void            * hdr,
                                                size_t                  header_size,
                                                const void            * pipe_addr,
                                                size_t                  data_size,
                                                pami_endpoint_t         origin,
                                                pami_pwq_recv_t       * recv)
{
  struct AMHeader * header = (struct AMHeader *) hdr;
  //unused:   int ctxt = header->dest_ctxt;
  CollectiveManager<T_NI> *mc = (CollectiveManager<T_NI>*) cookie;
  void * base0 =  mc->find (header->kind, header->tag);

  if (base0 == NULL)
    xlpgas_fatalerror (-1, "%d: Alltoall<T_NI>/v: <%d,%d> is undefined",
                         XLPGAS_MYNODE, header->tag, header->kind);
  Alltoall<T_NI> * s = (Alltoall<T_NI> * ) ((char *)base0 + header->offset);
  TRACE((stderr, "%d: ALLTOALL: <%d,%d> INCOMING base=%p ptr=%p len=%d\n",
         XLPGAS_MYNODE, header->tag, header->kind, base0, s, s->_len));
  MUTEX_LOCK(&s->_mutex);
  pwq_list_t *rcvpwq = s->_rcvpwqFL;
  if(rcvpwq != NULL)
  {
    s->_rcvpwqFL = (pwq_list_t*)rcvpwq->next;
  }
  else
  {
    pwq_list_t * rcvpwqs = (pwq_list_t*)__global.heap_mm->malloc (sizeof(pwq_list_t)* MAX_PENDING);
    unsigned i = 0;
    for(i=0;i<MAX_PENDING;i++) {
      (rcvpwqs+i)->next = s->_rcvpwqFL;
      s->_rcvpwqFL = (rcvpwqs + i);
    }
    i = 0;
    while(s->_rcvpwqList[i])i++;
    s->_rcvpwqList[i] = rcvpwqs;
    rcvpwq = s->_rcvpwqFL;
    s->_rcvpwqFL = (pwq_list_t*)rcvpwq->next;
  }

  alltoall_cookie_t *a2arcookie = s->_a2arcookieFL;
  if(a2arcookie != NULL)
  {
    s->_a2arcookieFL = (alltoall_cookie_t*)a2arcookie->next;
  }
  else
  {
    alltoall_cookie_t * a2arcookies = (alltoall_cookie_t*)__global.heap_mm->malloc (sizeof(alltoall_cookie_t)* MAX_PENDING);
    unsigned i = 0;
    for(i=0;i<MAX_PENDING;i++) {
      (a2arcookies+i)->next = s->_a2arcookieFL;
      s->_a2arcookieFL = (a2arcookies + i);
    }
    i = 0;
    while(s->_a2arcookieList[i])i++;
    s->_a2arcookieList[i] = a2arcookies;
    a2arcookie = s->_a2arcookieFL;
    s->_a2arcookieFL = (alltoall_cookie_t*)a2arcookie->next;
  }
  MUTEX_UNLOCK(&s->_mutex);
  rcvpwq->pwq.configure((char *)s->_rbuf + header->senderID * s->_rpwqlen, s->_rpwqlen, 0, s->_rtype);

  PAMI::PipeWorkQueue * z = &rcvpwq->pwq;
  a2arcookie->me          = s;
  a2arcookie->pwq         = rcvpwq;

  recv->rcvpwq                     = z;
  recv->cb_done.function           = Alltoall<T_NI>::cb_recvcomplete;
  recv->cb_done.clientdata         = a2arcookie;
  recv->totalRcvln                 = s->_len;

  return;
}

/* **************************************************************** */
/*           active message reception complete                      */
/* **************************************************************** */
template<class T_NI>
void xlpgas::Alltoall<T_NI>::cb_recvcomplete (void * unused, void * arg, pami_result_t res)
{
  alltoall_cookie_t * a2arcookie = (alltoall_cookie_t*)arg;
  
  Alltoall<T_NI>    * self       = (Alltoall<T_NI>*)a2arcookie->me;
  pwq_list_t        * rcvpwq     = (pwq_list_t*)a2arcookie->pwq;

  MUTEX_LOCK(&self->_mutex);
  rcvpwq->next = self->_rcvpwqFL;
  self->_rcvpwqFL = rcvpwq;

  a2arcookie->next = self->_a2arcookieFL;
  self->_a2arcookieFL = a2arcookie;

  self->_rcvcount[self->_odd]++;
  MUTEX_UNLOCK(&self->_mutex);
  TRACE((stderr, "%d: ALLTOALL: <%d,%d> RECVDONE\n",
         XLPGAS_MYNODE, self->_header.tag, self->_header.id));

  if (self->isdone())
  {
    if(self->_in_place)__global.heap_mm->free((void *)self->_sbuf);
    if (self->_cb_complete)
      self->_cb_complete (self->_pami_ctxt, self->_arg, res);
  }
}
