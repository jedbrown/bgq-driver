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
 * \file algorithms/protocols/tspcoll/AMReduce.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/AMExchange.h" // this includes AMBcast.h

void xlpgas::AMReduce::reset (int root, const void * s, unsigned l)
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
}


void xlpgas::AMReduce::root(void){
  void *temp_buf;
  size_t res_size;
  //get the function pointer from the registry
  (*(xlpgas_tsp_amcoll_lookup(_header->user_cb).amreduce))(_ctxt, _team_id, _root, _rlen, _header->dt, _header->op, _header, _header->hdr.headerlen, &temp_buf, NULL, NULL, &_user_cb_received, &_cookie);
  memcpy (_rbuf, temp_buf, _rlen);
}

void* xlpgas::AMReduce::parent_incomming(void){
  void * temp_buf;
  size_t res_size;
  (*(xlpgas_tsp_amcoll_lookup(_header->user_cb).amreduce))(_ctxt, _team_id, _root, _rlen, _header->dt, _header->op, _header, _header->hdr.headerlen, &temp_buf, NULL, NULL, &_user_cb_received, &_cookie);
  return temp_buf;
}

void xlpgas::AMReduce::allocate_space_result(void){
  if(_children != 0) {
    _temp = __global.heap_mm->malloc(_children * _rlen);
    assert(_temp != NULL);
  }
  else {
    _temp = NULL;
  }
 }

void xlpgas::AMReduce::merge_data(void){
  //merge or reduce performed here
  //printf("Merge Called L=%d ; Data stored in %x\n", _rlen, _rbuf);
  //for(int i=0;i<4;++i){
  //  printf("L%d MERGE val=[%d %d]\n", XLPGAS_MYNODE, ((int*)_rbuf)[i], ((int*)_temp)[i]);
  //}
  xlpgas::Allreduce::cb_Allreduce_t  cb = xlpgas::Allreduce::getcallback (_header->op, _header->dt);
  user_func_t uf;
  for(int i=0;i<_children;++i){
    //for all children; merge their data in rbuf
    cb(_rbuf, (char*)_temp + i*_rlen, _rlen / xlpgas::Allreduce::datawidthof(_header->dt), uf);
  }
}
