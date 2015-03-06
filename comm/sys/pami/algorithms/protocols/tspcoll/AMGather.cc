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
 * \file algorithms/protocols/tspcoll/AMGather.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/AMExchange.h"

void xlpgas::AMGather::reset (int root, const void * s, unsigned l)
{
  _header->root   = root;
  _root           = root;
  _children_rcv  = 0;
  _rcvcount       = 0;
  _sndcount       = 0;
  _sbuf           = 0;
  _rbuf = const_cast<void*>(s);
  _slen            = l;
  _rlen            = l;
  _ret_values      = true;
  _to_be_received = _header->rlen;
}


void xlpgas::AMGather::root(void){
  void *temp_buf;
  size_t res_size;
  //get the function pointer from the registry
  (*(xlpgas_tsp_amcoll_lookup(_header->user_cb).amgather))(_ctxt, _team_id, _root, _rlen,  _header, _header->hdr.headerlen, &temp_buf, NULL, NULL, &_user_cb_received);
  memcpy (_rbuf, temp_buf, _rlen);
  _off_rbuf+=_rlen;
}

void* xlpgas::AMGather::parent_incomming(void){
  void * temp_buf;
  size_t res_size;
  (*(xlpgas_tsp_amcoll_lookup(_header->user_cb).amgather))(_ctxt, _team_id, _root, _rlen, _header, _header->hdr.headerlen, &temp_buf, NULL, NULL, &_user_cb_received);
  _off_rbuf += _rlen;
  return temp_buf;
}

void xlpgas::AMGather::allocate_space_result(void){
  if(_parent == -1)
    _temp = _rbuf;
  else
    _temp = __global.heap_mm->malloc(_header->rlen);
  assert(_temp != NULL);
}

void xlpgas::AMGather::adjust_header(size_t distance){
  //divide by two plust adjust if non power of two
  _header->rlen = _to_be_received - distance * _rlen;
  _to_be_received -= _header->rlen;
}

void xlpgas::AMGather::merge_data(void){
  memcpy (_temp, _rbuf, _rlen);
  //_off_rbuf+=_rlen;
  //prepare for return to parent; we ship the concatenated buffer
  _rlen = _off_rbuf;//we ship up a buffer whose size increases
  _rbuf = _temp;
}
