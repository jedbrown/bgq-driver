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
 * \file algorithms/protocols/tspcoll/SHMReduceBcast.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/SHMReduceBcast.h"
#include "algorithms/protocols/tspcoll/Team.h"

#define XLPGAS_SHM_ROOT 0

extern "C" char* xlpgas_shm_buf;
extern "C" char* xlpgas_shm_buf_bcast;
extern "C" char* xlpgas_shm_buf_lg_bcast;

/* ************************************************************************* */
/*                      start a reduce operation                         */
/* ************************************************************************* */
template<class T_NI>
xlpgas::SHMReduce<T_NI>::
SHMReduce (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset, void* device_info, T_NI* ni) :
  CollExchange<T_NI> (ctxt, comm, kind, tag,offset,ni), fl(comm->size(),this->ordinal(),XLPGAS_SHM_ROOT,((device_info_type*)device_info)->shm_buffers()._reduce_buf)
{
}

template <class T_NI>
void xlpgas::SHMReduce<T_NI>::setContext (pami_context_t ctxt) {
  this->_pami_ctxt=ctxt;
  fl.setContext(ctxt);
}

template<class T_NI>
void xlpgas::SHMReduce<T_NI>::reset (int rootindex,
			       const void         * sbuf, 
			       void               * dbuf,
			       xlpgas_ops_t       op,
			       xlpgas_dtypes_t    dt,
			       unsigned           nelems,
			       user_func_t*       uf)
{
  this->cau_op = xlpgas::cau_op_dtype(op,dt);  
  this->_rbuf[0]   = dbuf;
  this->_sbuf[0]   = const_cast<void*>(sbuf);
  this->_sbufln[0] = nelems;
  this->_done      = false;
  fl.set_undone();
}


template<class T_NI>
pami_result_t repost_function (pami_context_t context, void *cookie) {
  xlpgas::SHMReduce<T_NI>* coll = (xlpgas::SHMReduce<T_NI>*)cookie;
  coll->kick_internal();
  if(coll->isdone()) {
    return PAMI_SUCCESS;
  }
  else return PAMI_EAGAIN;
}

template<class T_NI>
void xlpgas::SHMReduce<T_NI>::kick_internal (void) {
  fl.reduce ( (int64_t*)this->_sbuf[0], (int64_t*)this->_rbuf[0] , cau_op, this->_sbufln[0]);
}

template<class T_NI>
void xlpgas::SHMReduce<T_NI>::kick (void) {
  kick_internal();
  if(!fl.isdone()) {
    //repost
    PAMI::Device::Generic::GenericThread *work = new ((void*)(&_work_pami)) PAMI::Device::Generic::GenericThread(repost_function<T_NI>, (void*)this);
    this->_dev[0].postThread(work);
  }
}

template<class T_NI>
inline bool xlpgas::SHMReduce<T_NI>::isdone() const
{
  return fl.isdone();
}

/* ************************************************************************* */
/*                      start a bcast operation                         */
/* ************************************************************************* */
template<class T_NI>
xlpgas::SHMBcast<T_NI>::
SHMBcast (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset, void* device_info, T_NI* ni) :
  CollExchange<T_NI> (ctxt, comm, kind, tag, offset, ni), fl(comm->size(),this->ordinal(),XLPGAS_SHM_ROOT,((device_info_type*)device_info)->shm_buffers()._bcast_buf)
{
}

template <class T_NI>
void xlpgas::SHMBcast<T_NI>::setContext (pami_context_t ctxt) {
  this->_pami_ctxt=ctxt;
  fl.setContext(ctxt);
}

template<class T_NI>
void xlpgas::SHMBcast<T_NI>::reset (int rootindex,
			      const void         * sbuf, 
			      void               * dbuf,
			      unsigned           nbytes)
{
  this->_rbuf[0] = dbuf;
  this->_sbuf[0] = const_cast<void*>(sbuf);
  _nbytes = nbytes;
  this->_done = false;
  fl.set_undone();
}

template<class T_NI>
pami_result_t repost_bcast_function (pami_context_t context, void *cookie) {
  xlpgas::SHMBcast<T_NI>* coll = (xlpgas::SHMBcast<T_NI>*)cookie;
  coll->kick_internal();
  if(coll->isdone()) {
    return PAMI_SUCCESS;
  }
  else return PAMI_EAGAIN;
}

template<class T_NI>
void xlpgas::SHMBcast<T_NI>::kick_internal (void) {
  if(!fl.haveParent()){
    memcpy(this->_rbuf[0], this->_sbuf[0], _nbytes);
  }
  fl.bcast ((xlpgas_local_addr_t)this->_rbuf[0], _nbytes);
}

template<class T_NI>
void xlpgas::SHMBcast<T_NI>::kick (void) {
  kick_internal();
  if(!fl.isdone()) {
    //repost
    PAMI::Device::Generic::GenericThread *work = new ((void*)(&_work_pami)) PAMI::Device::Generic::GenericThread(repost_bcast_function<T_NI>, (void*)this);
    this->_dev[0].postThread(work);
  }
}

template<class T_NI>
inline bool xlpgas::SHMBcast<T_NI>::isdone() const
{
  return fl.isdone();
}

/* ************************************************************************* */
/*                      Large shared bcast operation                         */
/* ************************************************************************* */
template<class T_NI>
xlpgas::SHMLargeBcast<T_NI>::
SHMLargeBcast (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset, void* device_info, T_NI* ni) :
  Collective<T_NI> (ctxt, comm, kind, tag, NULL,NULL,ni), 
  fl(comm->size(),
     this->ordinal(),
     XLPGAS_SHM_ROOT,
     ((device_info_type*)device_info)->shm_buffers(), 
     comm->size())//fl
{
  CNT        = 0;
}

template <class T_NI>
void xlpgas::SHMLargeBcast<T_NI>::setContext (pami_context_t ctxt) {
  this->_pami_ctxt=ctxt;
  fl.setContext(ctxt);
}

template<class T_NI>
void xlpgas::SHMLargeBcast<T_NI>::reset (int rootindex,
                                  const void         * sbuf, 
                                  void               * dbuf,
                                  unsigned           nbytes)
{
  _sbuf = sbuf;
  _dbuf = dbuf;
  _nbytes = nbytes;//total number of bytes; it will be chopped out in _nbytes/buf_size chunks
  if(rootindex != fl.root()) fl.reset(rootindex);
}

template<class T_NI>
pami_result_t repost_lgbcast_function (pami_context_t context, void *cookie) {
  xlpgas::SHMLargeBcast<T_NI>* coll = (xlpgas::SHMLargeBcast<T_NI>*)cookie;
  coll->kick_internal();
  if(coll->isdone()) {
    coll->CNT--;
    return PAMI_SUCCESS;
  }
  else return PAMI_EAGAIN;
}

template<class T_NI>
void xlpgas::SHMLargeBcast<T_NI>::kick_internal (void) {
  fl.set_undone();//if we are here it means we are not done;
                  //it can't be in the reset because reset happens to be called in the callback of the current collective sometimes(pipelining)
  fl.bcast((xlpgas_local_addr_t)_dbuf, _nbytes);
}

template<class T_NI>
inline bool xlpgas::SHMLargeBcast<T_NI>::isdone() const
{
  return fl.isdone();
}
  
template<class T_NI>
void xlpgas::SHMLargeBcast<T_NI>::kick () {
  if( ! fl.haveParent() && _dbuf != _sbuf) {
    memcpy(_dbuf, _sbuf, _nbytes);
  }
  kick_internal();
  if(!this->isdone() && CNT==0) {
    //repost
    CNT++;
    PAMI::Device::Generic::GenericThread *work = new ((void*)(&_work_pami)) PAMI::Device::Generic::GenericThread(repost_lgbcast_function<T_NI>, (void*)this);
    work->setStatus(PAMI::Device::Ready);
    this->_dev[0].postThread(work);
  }
}
