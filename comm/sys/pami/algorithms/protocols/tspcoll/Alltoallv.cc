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
 * \file algorithms/protocols/tspcoll/Alltoallv.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/Alltoallv.h"
#include "algorithms/protocols/tspcoll/Team.h"

/* **************************************************************** */
/*      start a new alltoallv. Old alltoallv has to be complete       */
/* **************************************************************** */
template<class T_NI, class CntType>
void xlpgas::Alltoallv<T_NI,CntType>::reset (const void * s, void * d,
			       TypeCode     *stype,
			       const CntType *scnts,
			       const CntType *sdispls,
			       TypeCode     *rtype,
			       const CntType *rcnts,
			       const CntType *rdispls)
{
  MUTEX_LOCK(&this->_mutex);
  //alltoall common
  this->_odd = (!this->_odd);
  this->_sndcount[this->_odd] = 0;
  this->_sndstartedcount[this->_odd] = 0;
  this->_rcvcount[this->_odd] = 0;
  this->_rbuf           = (char *)d;
  this->_sbuf           = (const char *)s;
  this->_stype          = stype;
  this->_rtype          = rtype;
  this->_current        = this->ordinal();
  //altoallv specific
  this->_scnts          = scnts;
  this->_sdispls        = sdispls;
  this->_rcnts          = rcnts;
  this->_rdispls        = rdispls;
  this->_in_place       = 0;
  if(this->_sndpwq == NULL)
  {
    unsigned slot_size = (this->_comm->size()<MAX_PENDING)?this->_comm->size():MAX_PENDING;
    unsigned i = 0;
    this->_sndpwq   = (pwq_list_t *)__global.heap_mm->malloc (sizeof(pwq_list_t)* slot_size);
    this->_sndpwqFL = NULL;
    for(i=0;i<slot_size;i++) {
      (this->_sndpwq+i)->next = this->_sndpwqFL;
      this->_sndpwqFL = (this->_sndpwq + i);
    }
    this->_a2ascookie   = (alltoall_cookie_t *)__global.heap_mm->malloc (sizeof(alltoall_cookie_t)* slot_size);
    this->_a2ascookieFL = NULL;
    for(i=0;i<slot_size;i++) {
      (this->_a2ascookie+i)->next = this->_a2ascookieFL;
      this->_a2ascookieFL = (this->_a2ascookie + i);
    }
    this->_rcvpwqList = (pwq_list_t **)__global.heap_mm->malloc (sizeof(pwq_list_t*)* slot_size);
    memset(this->_rcvpwqList, 0, sizeof(pwq_list_t*)*slot_size);
    this->_a2arcookieList = (alltoall_cookie_t **)__global.heap_mm->malloc (sizeof(alltoall_cookie_t*)* slot_size);
    memset(this->_a2arcookieList, 0, sizeof(alltoall_cookie_t*)*slot_size);
    pwq_list_t *rcvpwqs = (pwq_list_t *)__global.heap_mm->malloc (sizeof(pwq_list_t)* slot_size);
    this->_rcvpwqFL = NULL;
    for(i=0;i<slot_size;i++) {
      (rcvpwqs+i)->next = this->_rcvpwqFL;
      this->_rcvpwqFL = (rcvpwqs + i);
    }
    alltoall_cookie_t *a2arcookies   = (alltoall_cookie_t *)__global.heap_mm->malloc (sizeof(alltoall_cookie_t)* slot_size);
    this->_a2arcookieFL = NULL;
    for(i=0;i<slot_size;i++) {
      (a2arcookies+i)->next = this->_a2arcookieFL;
      this->_a2arcookieFL = (a2arcookies + i);
    }
    this->_rcvpwqList[0]     = rcvpwqs;
    this->_a2arcookieList[0] = a2arcookies;
  }

  if(s == PAMI_IN_PLACE)
  {
    this->_in_place = 1;
    size_t sumcnts = 0;
    unsigned csize;
    for(csize = 0; csize < this->_comm->size(); csize++)
      sumcnts += rcnts[csize];
    this->_sbuf    = (char *)__global.heap_mm->malloc (sumcnts*rtype->GetExtent());
    PAMI_assert_alwaysf(this->_sbuf != NULL, "Error allocating memory in %s %d",__FILE__,__LINE__);
    this->_scnts   = this->_rcnts;
    this->_sdispls = this->_rdispls;
    this->_stype   = this->_rtype;
    memcpy((void*)this->_sbuf, this->_rbuf, sumcnts*rtype->GetExtent());
  }
  MUTEX_UNLOCK(&this->_mutex);
}

/* **************************************************************** */
/*                   do all the sends in an alltoall                */
/* **************************************************************** */
template<class T_NI, class CntType>
void xlpgas::Alltoallv<T_NI,CntType>::kick_internal    () {
  MUTEX_LOCK(&this->_mutex);
  size_t datawidth  = this->_rtype->GetDataSize();
  size_t dataextent = this->_rtype->GetExtent();

  int j = this->_sndstartedcount[this->_odd];
  for (; j < (int)this->_comm->size(); j++) {
    //if the buffer is full then we give the system some time to
    //complete pending sends
    if (this->buffer_full()) {
      MUTEX_UNLOCK(&this->_mutex);
      break;
    }

    void * sbuf = (void *)(this->_sbuf + this->_sdispls[this->_current] * this->_stype->GetExtent());
    void * rbuf = this->_rbuf + this->_rdispls[this->_current] * dataextent;
    if (this->_current == this->ordinal())
    {
      PAMI_Type_transform_data(sbuf, this->_stype, 0, rbuf, this->_rtype,
                               0, this->_rcnts[this->_current]*datawidth, PAMI_DATA_COPY, NULL);


	this->_sndcount[this->_odd]++;
	this->_sndstartedcount[this->_odd]++;
	this->_rcvcount[this->_odd]++;

	/* UNLOCK */
	MUTEX_UNLOCK(&this->_mutex);
	if (this->_sndcount[this->_odd] >= (int)this->_comm->size() &&
	    this->_rcvcount[this->_odd] >= (int)this->_comm->size())
        {
          if(_in_place)__global.heap_mm->free((void*)this->_sbuf);
	  if (this->_cb_complete)
	    this->_cb_complete (this->_pami_ctxt,this->_arg, PAMI_SUCCESS);
        }
    }
    else {
      pwq_list_t *sndpwq = this->_sndpwqFL;
      PAMI_assert_alwaysf(sndpwq != NULL, "Ran out of free sendpwqs in %s %d",__FILE__,__LINE__);
      this->_sndpwqFL = (pwq_list_t*)sndpwq->next;

      alltoall_cookie_t *a2ascookie = this->_a2ascookieFL;
      PAMI_assert_alwaysf(a2ascookie != NULL, "Ran out of alltoall send cookies in %s %d",__FILE__,__LINE__);
      this->_a2ascookieFL = (alltoall_cookie_t*)a2ascookie->next;

      MUTEX_UNLOCK(&this->_mutex);
      a2ascookie->me  = this;
      a2ascookie->pwq = sndpwq;
      this->_sndstartedcount[this->_odd]++;
      unsigned connection_Id = this->_header.tag;
//      _headers[i].dest_ctxt = _comm->endpoint(i).ctxt;
      pami_send_event_t   events;
      events.cookie         = a2ascookie;
      events.local_fn       = this->cb_senddone;
      events.remote_fn      = NULL;
      sndpwq->pwq.configure((char*) sbuf, this->_scnts[this->_current] * dataextent, this->_scnts[this->_current] * dataextent, NULL, this->_stype);
      this->_p2p_iface->sendPWQ(this->_pami_ctxt, this->_comm->index2Endpoint (this->_current), connection_Id, sizeof(this->_header),&this->_header,this->_scnts[this->_current] * this->_stype->GetDataSize(), &sndpwq->pwq, &events);
    }

    // increment current wrapping arround
    this->_current += 1;
    if(this->_current == this->_comm->size())
      this->_current = 0;

  }
}

/*
  Alltoall pushes a certain number of messages and waits
 */
template<class T_NI, class CntType>
pami_result_t repost_all2allv_function (pami_context_t context, void *cookie) {
  xlpgas::Alltoallv<T_NI,CntType>* coll = (xlpgas::Alltoallv<T_NI,CntType>*)cookie;
  coll->kick_internal();
  if( coll->all_sent() ) {
    return PAMI_SUCCESS;
  }
  else {
    return PAMI_EAGAIN;
  }
}

template<class T_NI, class CntType>
void xlpgas::Alltoallv<T_NI,CntType>::kick    () {
  this->kick_internal();
  if( ! this->all_sent() ) {
    //repost if not all messages sent due to buffer full
    PAMI::Device::Generic::GenericThread *work = new ((void*)(&(this->_work_pami))) PAMI::Device::Generic::GenericThread(repost_all2allv_function<T_NI, CntType>, (void*)this);
    this->_dev[0].postThread(work);
  }
}


/* **************************************************************** */
/*               reception header handler                           */
/* **************************************************************** */
template<class T_NI, class CntType>
inline void xlpgas::Alltoallv<T_NI,CntType>::cb_incoming_v(pami_context_t          context,
                                                   void                  * cookie,
                                                   const void            * hdr,
                                                   size_t                  header_size,
                                                   const void            * pipe_addr,
                                                   size_t                  data_size,
                                                   pami_endpoint_t         origin,
                                                   pami_pwq_recv_t       * recv)
{
  struct Alltoall<T_NI>::AMHeader * header = (struct Alltoall<T_NI>::AMHeader *) hdr;
  //unused:   int ctxt = header->dest_ctxt;
  CollectiveManager<T_NI> *mc = (CollectiveManager<T_NI>*) cookie;
  void * base0 =  mc->find (header->kind, header->tag);

  if (base0 == NULL)
    xlpgas_fatalerror (-1, "%d: Alltoallv<T_NI>/v: <%d,%d> is undefined",
                         XLPGAS_MYNODE, header->tag, header->kind);
  Alltoallv<T_NI,CntType> * s = (Alltoallv<T_NI,CntType> * ) ((char *)base0 + header->offset);
  TRACE((stderr, "%d: ALLTOALL: <%d,%d> INCOMING base=%p ptr=%p len=%d\n",
         XLPGAS_MYNODE, header->tag, header->kind, base0, s, s->_len));


  char * rbuf =  s->_rbuf + s->_rdispls[header->senderID] * s->_rtype->GetExtent();

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

  rcvpwq->pwq.configure(rbuf, s->_rcnts[header->senderID] * s->_rtype->GetExtent(), 0, s->_rtype);

  PAMI::PipeWorkQueue * z = &rcvpwq->pwq;
  a2arcookie->me          = s;
  a2arcookie->pwq         = rcvpwq;

  recv->rcvpwq                     = z;
  recv->cb_done.function           = Alltoallv<T_NI,CntType>::cb_recvcomplete;
  recv->cb_done.clientdata         = a2arcookie;
  recv->totalRcvln                 = s->_rcnts[header->senderID] * s->_rtype->GetDataSize();
  return;
}
