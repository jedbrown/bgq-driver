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
 * \file algorithms/protocols/tspcoll/cau_collectives.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/cau_collectives.h"
#include "algorithms/protocols/tspcoll/SHMReduceBcast.h"

///////////////////////////////////////////////////////
template <class T_NI>
int xlpgas::CAUReduce<T_NI>::_dispatch_id;

template <class T_NI>
void xlpgas::CAUReduce<T_NI>::
register_dispatch(int* dispatch_id, lapi_handle_t lh){
  _dispatch_id = (*dispatch_id)--;
  LapiImpl::Context *cp = (LapiImpl::Context *)_Lapi_port[lh];
  internal_rc_t rc = (cp->*(cp->pDispatchSet))(_dispatch_id,
                                               (void * )xlpgas::CAUReduce<T_NI>::recv_reduce,
                                               NULL,
                                               null_dispatch_hint,
                                               INTERFACE_LAPI);
  assert(rc == SUCCESS);
}

template <class T_NI>
xlpgas::CAUReduce<T_NI>::
CAUReduce (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset,void* device_info, T_NI* ni) :
  CollExchange<T_NI> (ctxt, comm, kind, tag, offset, ni)
{
  reduce_sent=0;
  reduce_received=0;
  _done = false;
  //set device specific
  this->_device_info = device_info;
  lapi_handle = ((device_info_type*)device_info)->lapi_handle();
  base_group_id = ((device_info_type*)device_info)->cau_group();
}

template <class T_NI>
void xlpgas::CAUReduce<T_NI>::reset (int rootindex,
			       const void         * sbuf,
			       void               * dbuf,
			       xlpgas_ops_t       op,
			       xlpgas_dtypes_t    dt,
			       unsigned           nelems,
			       user_func_t*       uf)
{
  cau_op = xlpgas::cau_op_dtype(op,dt);
  mcast_data  = (int64_t*)dbuf;
  reduce_data = (int64_t*)sbuf;
  this->_sbufln[0] = nelems;
  //spread the data if not  64 bit (8byte) long
  this->spread = (nelems>1) && (xlpgas::cau_dtype_size(cau_op)<8);
}

template <class T_NI>
void* xlpgas::CAUReduce<T_NI>::recv_reduce(lapi_handle_t *hndl, void *uhdr, uint *uhdr_len,
				     ulong *msg_len, compl_hndlr_t **comp_h, void **uinfo)
{
    lapi_return_info_t &ret_info = *(lapi_return_info_t *)msg_len;
    reduce_hdr_t  &reduce_hdr = *(reduce_hdr_t *)uhdr;
    assert( *uhdr_len == sizeof(reduce_hdr) );

    xlpgas::CollectiveManager<T_NI> *cm = (xlpgas::CollectiveManager<T_NI> *)__global._id_to_collmgr_table[*hndl];
    void * base = cm->find (reduce_hdr.kind,reduce_hdr.tag);
    if (base == NULL){
      xlpgas_fatalerror (-1, "%d: incoming: cannot find coll=<%d,%d>",
			 XLPGAS_MYNODE, reduce_hdr.kind, reduce_hdr.tag);
    }
    xlpgas::CAUReduce<T_NI> * b = (xlpgas::CAUReduce<T_NI> * ) ((char *)base);
    bool exec_cb=false;
    MUTEX_LOCK(&b->_mutex);
    ++(b->reduce_received); //reduce_hdr.seq;
    int64_t *data = (int64_t *)ret_info.udata_one_pkt_ptr;
    //fprintf(stderr, "L%d reduce CB invoked  %d %d \n",b->ordinal(),b->reduce_received, b->instance_id);
    if (b->reduce_received <= b->instance_id){
      //data already arrived; first reduce with local data
      xlpgas::sparse_reduce_op(b->mcast_data, data, reduce_hdr.op, b->_sbufln[0]);
      //compact if necessary
      if(b->spread){
	xlpgas::compact_data(b->mcast_data, b->_sbufln[0], b->cau_op);
      }
      exec_cb=true;
    }
    else {
      memcpy(b->temp_reduce_data, data, b->_sbufln[0]*sizeof(int64_t));
    }
    MUTEX_UNLOCK(&b->_mutex);
    if (exec_cb && b->_cb_complete)
	b->_cb_complete ((void*)&(b->_ctxt), b->_arg, PAMI_SUCCESS);
    return NULL;
}

template <class T_NI>
void xlpgas::CAUReduce<T_NI>::on_reduce_sent(lapi_handle_t *hndl, void *cookie)
{
  xlpgas::CAUReduce<T_NI> *b = (xlpgas::CAUReduce<T_NI>*)cookie;
  ++(b->reduce_sent);
}

template <class T_NI>
void  xlpgas::CAUReduce<T_NI>::kick(void){
  unsigned int ROOT=0;
  int nelems = this->_sbufln[0];
  cau_reduce_op_t ncau_op = cau_op;
  if(this->spread) {
    //spread the data in place; we know the input and output has space
    //for up to 8 64 bit elements also the input buf has been zeroed
    //out by the reset function of the hybrid algo
    xlpgas::spread_data(reduce_data, nelems, cau_op);
    if(cau_op.operand_type ==  CAU_SIGNED_INT) ncau_op.operand_type =  CAU_SIGNED_LONGLONG;
    if(cau_op.operand_type ==  CAU_UNSIGNED_INT) ncau_op.operand_type =  CAU_UNSIGNED_LONGLONG;
  }

  if(this->ordinal() != ROOT) {
    //fprintf(stderr, "Non Root sends DID=%d\n", _dispatch_id);
    reduce_hdr.op     = cau_op;
    reduce_hdr.kind    = this->_header[0].kind;
    reduce_hdr.tag    = this->_header[0].tag;
    instance_id++;
    RC0( LAPI_Cau_reduce(lapi_handle, base_group_id, 
			 this->_dispatch_id, &reduce_hdr, sizeof(reduce_hdr),
			 reduce_data, 
			 nelems * sizeof(int64_t), ncau_op,
			 CAUReduce<T_NI>::on_reduce_sent, (void *)this) );
    ++reduce_received;
  }

  if(this->ordinal() == ROOT) {
    //fprintf(stderr, "L%d Reduce root wait for results cg DID=%d\n", this->ordinal(), _dispatch_id);
    //Root receives
    bool exec_cb=false;
    MUTEX_LOCK(&this->_mutex);
    instance_id++;
    ++reduce_sent; 
    memcpy(mcast_data, reduce_data, this->_sbufln[0]*sizeof(int64_t));
    if (reduce_received >= instance_id){
      //data already arrived; reduce with local
      xlpgas::sparse_reduce_op(mcast_data, temp_reduce_data, ncau_op, nelems);
      // compact if necessary
      if(this->spread){
	xlpgas::compact_data(this->mcast_data, nelems, cau_op);
      }
      exec_cb=true;
    }
    MUTEX_UNLOCK(&this->_mutex);
    if (exec_cb && this->_cb_complete)
      this->_cb_complete ((void*)&(this->_ctxt), this->_arg, PAMI_SUCCESS);
  }
}//end kick()

template <class T_NI>
inline bool xlpgas::CAUReduce<T_NI>::isdone() const
{
  if(this->ordinal() != 0) {
    return (reduce_sent >= instance_id);
  }
  else{
    return (reduce_received >= instance_id);
  }
}

/////////////////////////////////////////// CAUBcast
template <class T_NI>
int xlpgas::CAUBcast<T_NI>::_dispatch_id;

template <class T_NI>
void xlpgas::CAUBcast<T_NI>::
register_dispatch(int* dispatch_id, lapi_handle_t lh){
  _dispatch_id = (*dispatch_id)--;
  LapiImpl::Context *cp = (LapiImpl::Context *)_Lapi_port[lh];
  internal_rc_t rc = (cp->*(cp->pDispatchSet))(_dispatch_id,
                                               (void * )xlpgas::CAUBcast<T_NI>::recv_mcast,
                                               NULL,
                                               null_dispatch_hint,
                                               INTERFACE_LAPI);
  assert(rc == SUCCESS);
}

template <class T_NI>
xlpgas::CAUBcast<T_NI>::
CAUBcast (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset,void* device_info, T_NI* ni) :
  CollExchange<T_NI> (ctxt, comm, kind, tag, offset, ni)
{
  instance_id=0;
  mcast_sent=0;
  mcast_received=0;
  _done =false;
  //set device specific
  this->_device_info = device_info;
  lapi_handle = ((device_info_type*)device_info)->lapi_handle();
  base_group_id = ((device_info_type*)device_info)->cau_group();
}

template <class T_NI>
void xlpgas::CAUBcast<T_NI>::reset (int rootindex,
                                    const void         * sbuf,
                                    void               * dbuf,
                                    unsigned           nbytes){
  _done =false;
  this->_sbufln[0] = nbytes;
  mcast_data = (int64_t*)dbuf;
}

template <class T_NI>
void xlpgas::CAUBcast<T_NI>::on_mcast_sent(lapi_handle_t *hndl, void *cookie)
{
  xlpgas::CAUBcast<T_NI> *b = (xlpgas::CAUBcast<T_NI>*)cookie;
  ++(b->mcast_sent);
}

template <class T_NI>
void* xlpgas::CAUBcast<T_NI>::recv_mcast(lapi_handle_t *hndl, void *uhdr, uint *uhdr_len,
        ulong *msg_len, compl_hndlr_t **comp_h, void **uinfo)
{
    lapi_return_info_t &ret_info = *(lapi_return_info_t *)msg_len;
    mcast_hdr_t  &mcast_hdr = *(mcast_hdr_t *)uhdr;
    assert( *uhdr_len == sizeof(mcast_hdr) );
    xlpgas::CollectiveManager<T_NI> *cm = (xlpgas::CollectiveManager<T_NI> *)__global._id_to_collmgr_table[*hndl];
    void * base = cm->find (mcast_hdr.kind,mcast_hdr.tag);
    if (base == NULL){
      xlpgas_fatalerror (-1, "%d: incoming: cannot find coll=<%d,%d>",
			 XLPGAS_MYNODE, mcast_hdr.kind, mcast_hdr.tag);
    }
    xlpgas::CAUBcast<T_NI> * b = (xlpgas::CAUBcast<T_NI> * ) ((char *)base);

    ++(b->mcast_received);

    int64_t *data = (int64_t *)ret_info.udata_one_pkt_ptr;
    memcpy(b->mcast_data, data, b->_sbufln[0]);

    if (b->_cb_complete)
      b->_cb_complete ((void*)&(b->_ctxt), b->_arg, PAMI_SUCCESS);

    return NULL;
}

template <class T_NI>
void  xlpgas::CAUBcast<T_NI>::kick(void){
  //BCAST
  unsigned int ROOT=0;
  // initialize multicast header
  mcast_hdr.kind    = this->_header[0].kind;
  mcast_hdr.tag    = this->_header[0].tag;

  //int dd = *((int*)mcast_data);
  //fprintf(stderr, "L%d MCAST mcast_data=%p size=%ld \n",this->ordinal(), mcast_data, this->_sbufln[0]*sizeof(int64_t));
  if(this->ordinal() == ROOT){
    ++mcast_received;
    RC0( LAPI_Cau_multicast(lapi_handle, base_group_id, 
			    _dispatch_id, &mcast_hdr, sizeof(mcast_hdr),
			    mcast_data, this->_sbufln[0],
			    CAUBcast<T_NI>::on_mcast_sent, (void *)this) );

    //this move inside on_mcast_sent
    if (this->_cb_complete)
      this->_cb_complete ((void*)&(this->_ctxt), this->_arg, PAMI_SUCCESS);
  }
  else {
    ++mcast_sent;
  }
}

template <class T_NI>
inline bool xlpgas::CAUBcast<T_NI>::isdone() const
{
  if(this->ordinal() == 0){
    return (mcast_sent >= instance_id);
  }
  else{
    return (mcast_received >= instance_id);
  }
}
