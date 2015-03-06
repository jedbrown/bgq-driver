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
 * \file algorithms/protocols/onetask/OneTaskT.h
 * \brief ???
 */

#ifndef __algorithms_protocols_onetask_OneTaskT_h__
#define __algorithms_protocols_onetask_OneTaskT_h__

#include "common/type/TypeMachine.h"
#include "algorithms/connmgr/SimpleConnMgr.h"
#include "algorithms/protocols/CollectiveProtocolFactoryT.h"
#include "algorithms/executor/Barrier.h"

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#ifdef CCMI_TRACE_ALL
#define DO_TRACE_ENTEREXIT 1
#define DO_TRACE_DEBUG     1
#else
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0
#endif

#if 0
// This CHECK_ROOT code cannot check the root
// It needs the endpoint, which geometry does not provide
#define CHECK_ROOT    {                      \
   pami_task_t me = (pami_task_t)xfer->root; \
   if( me != _geometry->rank())              \
     return PAMI_ERROR;                      \
}
#endif
#define CHECK_ROOT


namespace CCMI
{
namespace Adaptor
{
namespace OneTask
{

typedef PAMI::Type::TypeCode    TypeCode;
typedef PAMI::Type::TypeMachine TypeMachine;

// OneTask Factory for generate routine
// generate
//
template < class T_Composite, MetaDataFn get_metadata, class T_Conn >
class OneTaskFactoryT : public CollectiveProtocolFactoryT<T_Composite, get_metadata, T_Conn>
{
public:
    OneTaskFactoryT(pami_context_t               ctxt,
                    size_t                       ctxt_id,
                    pami_mapidtogeometry_fn      cb_geometry,
                    T_Conn                      *cmgr,
                    Interfaces::NativeInterface *native,
                    pami_dispatch_multicast_function cb_head = NULL):
      CollectiveProtocolFactoryT<T_Composite, get_metadata, T_Conn>(ctxt,ctxt_id,cb_geometry,cmgr, native, cb_head)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT( "<%p> ni %p",this, native);
      TRACE_FN_EXIT();
    }

    static void cleanup_done_fn(pami_context_t  context,
                          void           *clientdata,
                          pami_result_t   res)
    {
      TRACE_FN_ENTER();
      T_Composite *obj = (T_Composite *)clientdata;
      typedef typename CollectiveProtocolFactoryT<T_Composite, get_metadata, T_Conn>::collObj collObj;
      collObj *cobj = (collObj*)obj->getCollObj();
      cobj->~collObj();
      cobj->_factory->_alloc.returnObject(cobj);
      TRACE_FN_EXIT();
    }

};




//Based on PAMI_Type_transform_data in pami_type.cc
inline pami_result_t copyData (void               * src_addr,
                               TypeCode           * src_type_obj,
                               void               * dst_addr,
                               TypeCode           * dst_type_obj,
                               size_t               dst_type_count,
                               size_t               dst_displs,
                               size_t               src_displs)
{
    if(src_addr == PAMI_IN_PLACE) 
    {
      src_addr = dst_addr;
      src_type_obj = dst_type_obj; 
    }
    else if(dst_addr == PAMI_IN_PLACE) 
    {
      dst_addr = src_addr;
      dst_type_obj = src_type_obj; 
    }
    size_t size = dst_type_obj->GetDataSize() * dst_type_count;
    if (!src_type_obj->IsCompleted() || !dst_type_obj->IsCompleted()) {
        RETURN_ERR_PAMI(PAMI_INVAL, "Using incomplete type.\n");
    }

    if (src_type_obj->IsContiguous()) {
        // unpacking: contiguous to non-contiguous (or contiguous)
        TypeMachine unpacker(dst_type_obj);
        unpacker.SetCopyFunc(PAMI_DATA_COPY, NULL);
        unpacker.MoveCursor(dst_displs);
        unpacker.Unpack(dst_addr, (char*)src_addr + src_displs, size);

    } else if (dst_type_obj->IsContiguous()) {
        // packing: non-contiguous to contiguous
        TypeMachine packer(src_type_obj);
        packer.SetCopyFunc(PAMI_DATA_COPY, NULL);
        packer.MoveCursor(src_displs);
        packer.Pack((char*)dst_addr + dst_displs, src_addr, size);

    } else {
        // generic: non-contiguous to non-contiguous
        TypeMachine packer(src_type_obj);
        packer.MoveCursor(src_displs);

        TypeMachine unpacker(dst_type_obj);
        unpacker.SetCopyFunc(PAMI_DATA_COPY, NULL);
        unpacker.MoveCursor(dst_displs);

        // use a temporary buffer to copy in and out data
        const size_t TMP_BUF_SIZE = 8192;
        char tmp_buf[TMP_BUF_SIZE];

        for (size_t offset = 0; offset < size; offset += TMP_BUF_SIZE) {
            size_t bytes_to_copy = std::min(size - offset, TMP_BUF_SIZE);
            packer.Pack(tmp_buf, (char *)src_addr + offset, bytes_to_copy);
            unpacker.Unpack((char *)dst_addr + offset, tmp_buf, bytes_to_copy);
        }
    }

    return PAMI_SUCCESS;
}

template <class T_Collective_type>
inline void doDispatch(pami_xfer_t                  *xfer,
                       pami_context_t                ctxt,
                       size_t                        ctxt_id,
                       pami_endpoint_t               endpoint,
                       pami_geometry_t               g,
                       pami_recv_t                  *recv)
{
// Clang performs syntax checks even for non-intantianated templates.
// This assertion fail needs to be deferred from syntax-checking to a possible instantianation attempt.
    COMPILE_TIME_ASSERT(sizeof(T_Collective_type) == 0);
}

template <>
inline void doDispatch<pami_ambroadcast_t>(pami_xfer_t                  *xfer,
                                           pami_context_t                ctxt,
                                           size_t                        ctxt_id,
                                           pami_endpoint_t               endpoint,
                                           pami_geometry_t               g,
                                           pami_recv_t                  *recv)
{
  (void)ctxt;(void)endpoint;(void)g;
  recv->local_fn = xfer->cb_done;
  recv->cookie   = xfer->cookie;
}

template <>
inline void doDispatch<pami_amscatter_t>(pami_xfer_t                  *xfer,
                                         pami_context_t                ctxt,
                                         size_t                        ctxt_id,
                                         pami_endpoint_t               endpoint,
                                         pami_geometry_t               g,
                                         pami_recv_t                  *recv)
{
  PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *)g;
  pami_amscatter_t *ams_xfer = &(xfer->cmd.xfer_amscatter);
  PAMI::Geometry::DispatchInfo *dispatch = geometry->getDispatch(ctxt_id, ams_xfer->dispatch);
  PAMI_assertf(dispatch != NULL, "Invalid dispatch ID: %zu\n", ams_xfer->dispatch);

  PAMI::Topology *topo = (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);
  TypeCode *stype  = (TypeCode *) ams_xfer->stype;
  size_t bytes     = ams_xfer->stypecount * stype->GetDataSize();

  // Invoke the dispatch function 
  dispatch->fn.amscatter (
       ctxt,                     // context
       dispatch->cookie,         // user cookie
       (char *)ams_xfer->headers + topo->endpoint2Index(endpoint) * ams_xfer->headerlen, // User header
       ams_xfer->headerlen,      // User header size
       NULL,
       bytes,                    // Number of bytes of message data
       endpoint,                 // origin (root)
       g,                        // Geometry
       recv);                    // recv info

  TypeCode *rtype = (TypeCode*)recv->type;
  copyData((void*)ams_xfer->sndbuf, stype, (void*)recv->addr, rtype, bytes/rtype->GetDataSize(), 0, 0);
}

template <>
inline void doDispatch<pami_amgather_t>(pami_xfer_t                  *xfer,
                                        pami_context_t                ctxt,
                                        size_t                        ctxt_id,
                                        pami_endpoint_t               endpoint,
                                        pami_geometry_t               g,
                                        pami_recv_t                  *send)
{
  PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *)g;
  pami_amgather_t *amg_xfer = &(xfer->cmd.xfer_amgather);
  PAMI::Geometry::DispatchInfo *dispatch = geometry->getDispatch(ctxt_id, amg_xfer->dispatch);
  PAMI_assertf(dispatch != NULL, "Invalid dispatch ID: %zu\n", amg_xfer->dispatch);

  PAMI::Topology *topo = (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);
  TypeCode *rtype  = (TypeCode *) amg_xfer->rtype;
  size_t bytes     = amg_xfer->rtypecount * rtype->GetDataSize();

  // Invoke the dispatch function 
  dispatch->fn.amgather (
       ctxt,                     // context
       dispatch->cookie,         // user cookie
       (char *)amg_xfer->headers + topo->endpoint2Index(endpoint) * amg_xfer->headerlen, // User header
       amg_xfer->headerlen,      // User header size
       bytes,                    // Number of bytes of message data
       endpoint,                 // origin (root)
       g,                        // Geometry
       send);                    // recv info

  copyData((void*)send->addr, (TypeCode*)send->type, (void*)amg_xfer->rcvbuf, rtype, amg_xfer->rtypecount, 0, 0);
}

template <>
inline void doDispatch<pami_amreduce_t>(pami_xfer_t                  *xfer,
                                        pami_context_t                ctxt,
                                        size_t                        ctxt_id,
                                        pami_endpoint_t               endpoint,
                                        pami_geometry_t               g,
                                        pami_recv_t                  *send)
{
  PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *)g;
  pami_amreduce_t *amr_xfer = &(xfer->cmd.xfer_amreduce);
  PAMI::Geometry::DispatchInfo *dispatch = geometry->getDispatch(ctxt_id, amr_xfer->dispatch);
  PAMI_assertf(dispatch != NULL, "Invalid dispatch ID: %zu\n", amr_xfer->dispatch);

  pami_data_function reduce_fn;
  TypeCode *rtype  = (TypeCode *) amr_xfer->rtype;
  size_t bytes     = amr_xfer->rtypecount * rtype->GetDataSize();

  // Invoke the dispatch function 
  dispatch->fn.amreduce (
       ctxt,                     // context
       dispatch->cookie,         // user cookie
       (char *)amr_xfer->user_header, // User header
       amr_xfer->headerlen,      // User header size
       bytes,                    // Number of bytes of message data
       endpoint,                 // origin (root)
       g,                        // Geometry
       &reduce_fn,               // PAMI math operation
       send);                    // recv info

  copyData((void*)send->addr, (TypeCode*)send->type, (void*)amr_xfer->rcvbuf, rtype, amr_xfer->rtypecount, 0, 0);
}

// OneTask Active Message Factory
template < typename T_Collective_type, MetaDataFn get_metadata, class T_Conn >
class OneTaskAMFactoryT : public CollectiveProtocolFactory
{
protected:
    Interfaces::NativeInterface    *_native;

public:
    OneTaskAMFactoryT(pami_context_t             ctxt,
                    size_t                       ctxt_id,
                    pami_mapidtogeometry_fn      cb_geometry,
                    T_Conn                      *cmgr,
                    Interfaces::NativeInterface *native):
      CollectiveProtocolFactory(ctxt,ctxt_id,cb_geometry),
      _native(native)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT( "<%p> ni %p",this, native);
      TRACE_FN_EXIT();
    }

    virtual void metadata(pami_metadata_t *mdata, pami_geometry_t geometry = PAMI_GEOMETRY_NULL)
    {
      DO_DEBUG((templateName<MetaDataFn>()));
      get_metadata(mdata);
      // Use arbitrary PAMI_XFER_COUNT
      if(_native) _native->metadata(mdata,PAMI_XFER_COUNT);
    }

    virtual Executor::Composite * generate(pami_geometry_t              g,
                                           void                      * cmd)
    {
      pami_recv_t recv = {0,};
      doDispatch<T_Collective_type>((pami_xfer_t *)cmd, _context, _context_id, _native->endpoint(), g, &recv);
      if(recv.local_fn)
        recv.local_fn (_context, recv.cookie, PAMI_SUCCESS);

      return NULL;
    }
};

template <class T_Collective_type>
inline pami_result_t doAction(T_Collective_type *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
// Clang performs syntax checks even for non-intantianated templates.
// This assertion fail needs to be deferred from syntax-checking to a possible instantianation attempt.
    COMPILE_TIME_ASSERT(sizeof(T_Collective_type) == 0);
    return PAMI_SUCCESS;
}

template <>
inline pami_result_t doAction<pami_barrier_t>(pami_barrier_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)xfer;(void)_geometry;
   //no op
   return PAMI_SUCCESS;
}

template <>
inline pami_result_t doAction<pami_allreduce_t>(pami_allreduce_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)_geometry;
   return copyData((void*)xfer->sndbuf, (TypeCode*)xfer->stype, (void*)xfer->rcvbuf, (TypeCode*)xfer->rtype, xfer->rtypecount, 0, 0);
}

template <>
inline pami_result_t doAction<pami_broadcast_t>(pami_broadcast_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)xfer;(void)_geometry;
   CHECK_ROOT
   return PAMI_SUCCESS;
}

template <>
inline pami_result_t doAction<pami_reduce_t>(pami_reduce_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)_geometry;
   CHECK_ROOT
   return copyData((void*)xfer->sndbuf, (TypeCode*)xfer->stype, (void*)xfer->rcvbuf, (TypeCode*)xfer->rtype, xfer->rtypecount, 0, 0);
}

template <>
inline pami_result_t doAction<pami_allgather_t>(pami_allgather_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)_geometry;
   return copyData((void*)xfer->sndbuf, (TypeCode*)xfer->stype, (void*)xfer->rcvbuf, (TypeCode*)xfer->rtype, xfer->rtypecount, 0, 0);
}

template <>
inline pami_result_t doAction<pami_allgatherv_t>(pami_allgatherv_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)_geometry;
   if(xfer->rtypecounts == NULL || xfer->rdispls == NULL)
     return PAMI_ERROR;
   return copyData((void*)xfer->sndbuf, (TypeCode*)xfer->stype, (void*)xfer->rcvbuf, (TypeCode*)xfer->rtype, xfer->rtypecounts[0], xfer->rdispls[0], 0);
}

template <>
inline pami_result_t doAction<pami_allgatherv_int_t>(pami_allgatherv_int_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)_geometry;
   if(xfer->rtypecounts == NULL || xfer->rdispls == NULL)
     return PAMI_ERROR;
   return copyData((void*)xfer->sndbuf, (TypeCode*)xfer->stype, (void*)xfer->rcvbuf, (TypeCode*)xfer->rtype, xfer->rtypecounts[0], xfer->rdispls[0], 0);
}

template <>
inline pami_result_t doAction<pami_scatter_t>(pami_scatter_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)_geometry;
   CHECK_ROOT
   return copyData((void*)xfer->sndbuf, (TypeCode*)xfer->stype, (void*)xfer->rcvbuf, (TypeCode*)xfer->rtype, xfer->rtypecount, 0, 0);
}

template <>
inline pami_result_t doAction<pami_scatterv_t>(pami_scatterv_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)_geometry;
   CHECK_ROOT
   if(xfer->stypecounts == NULL || xfer->sdispls == NULL)
     return PAMI_ERROR;
   return copyData((void*)xfer->sndbuf, (TypeCode*)xfer->stype, (void*)xfer->rcvbuf, (TypeCode*)xfer->rtype, xfer->rtypecount, 0, xfer->sdispls[0]);
}

template <>
inline pami_result_t doAction<pami_scatterv_int_t>(pami_scatterv_int_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)_geometry;
   CHECK_ROOT
   if(xfer->stypecounts == NULL || xfer->sdispls == NULL)
     return PAMI_ERROR;
   return copyData((void*)xfer->sndbuf, (TypeCode*)xfer->stype, (void*)xfer->rcvbuf, (TypeCode*)xfer->rtype, xfer->rtypecount, 0, xfer->sdispls[0]);
}

template <>
inline pami_result_t doAction<pami_gather_t>(pami_gather_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)_geometry;
   CHECK_ROOT
   return copyData((void*)xfer->sndbuf, (TypeCode*)xfer->stype, (void*)xfer->rcvbuf, (TypeCode*)xfer->rtype, xfer->rtypecount, 0, 0);
}

template <>
inline pami_result_t doAction<pami_gatherv_t>(pami_gatherv_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)_geometry;
   CHECK_ROOT
   if(xfer->rtypecounts == NULL || xfer->rdispls == NULL)
     return PAMI_ERROR;
   return copyData((void*)xfer->sndbuf, (TypeCode*)xfer->stype, (void*)xfer->rcvbuf, (TypeCode*)xfer->rtype, xfer->rtypecounts[0], xfer->rdispls[0], 0);
}

template <>
inline pami_result_t doAction<pami_gatherv_int_t>(pami_gatherv_int_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)_geometry;
   CHECK_ROOT
   if(xfer->rtypecounts == NULL || xfer->rdispls == NULL)
     return PAMI_ERROR;
   return copyData((void*)xfer->sndbuf, (TypeCode*)xfer->stype, (void*)xfer->rcvbuf, (TypeCode*)xfer->rtype, xfer->rtypecounts[0], xfer->rdispls[0], 0);
}

template <>
inline pami_result_t doAction<pami_alltoall_t>(pami_alltoall_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)_geometry;
   return copyData((void*)xfer->sndbuf, (TypeCode*)xfer->stype, (void*)xfer->rcvbuf, (TypeCode*)xfer->rtype, xfer->rtypecount, 0, 0);
}

template <>
inline pami_result_t doAction<pami_alltoallv_t>(pami_alltoallv_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)_geometry;
   if(xfer->rtypecounts == NULL || xfer->rdispls == NULL || xfer->stypecounts == NULL || xfer->sdispls == NULL)
     return PAMI_ERROR;
   return copyData((void*)xfer->sndbuf, (TypeCode*)xfer->stype, (void*)xfer->rcvbuf, (TypeCode*)xfer->rtype, xfer->rtypecounts[0], xfer->rdispls[0], xfer->sdispls[0]);
}

template <>
inline pami_result_t doAction<pami_alltoallv_int_t>(pami_alltoallv_int_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)_geometry;
   if(xfer->rtypecounts == NULL || xfer->rdispls == NULL || xfer->stypecounts == NULL || xfer->sdispls == NULL)
     return PAMI_ERROR;
   return copyData((void*)xfer->sndbuf, (TypeCode*)xfer->stype, (void*)xfer->rcvbuf, (TypeCode*)xfer->rtype, xfer->rtypecounts[0], xfer->rdispls[0], xfer->sdispls[0]);
}

template <>
inline pami_result_t doAction<pami_scan_t>(pami_scan_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)_geometry;
   return copyData((void*)xfer->sndbuf, (TypeCode*)xfer->stype, (void*)xfer->rcvbuf, (TypeCode*)xfer->rtype, xfer->rtypecount, 0, 0);
}

template <>
inline pami_result_t doAction<pami_reduce_scatter_t>(pami_reduce_scatter_t *xfer, PAMI_GEOMETRY_CLASS *_geometry)
{
  (void)_geometry;
   return copyData((void*)xfer->sndbuf, (TypeCode*)xfer->stype, (void*)xfer->rcvbuf, (TypeCode*)xfer->rtype, xfer->rcounts[0], 0, 0);
}

///
/// \brief OneTaskT class
///
template < typename T_Collective_type >
class OneTaskT : public CCMI::Executor::Composite
{
public:

    ///
    /// \brief Constructor for OneTaskT protocol.
    ///
    /// \param[in] geometry    Geometry object
    ///
    OneTaskT ( pami_context_t               ctxt,
               size_t                       ctxt_id,
               Interfaces::NativeInterface          * mInterface,
               ConnectionManager::SimpleConnMgr     * cmgr,
               pami_geometry_t                         geometry,
               void                                 * cmd,
               pami_event_function                     fn,
               void                                 * cookie)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT( "<%p> ni %p",this, mInterface);

        CCMI_assert( mInterface == NULL );
        CCMI_assert( cmgr       == NULL );
        pami_xfer_t *xfer = (pami_xfer_t*)cmd;
        _geometry = (PAMI_GEOMETRY_CLASS*)geometry;
        _cmd      = *xfer;
        _fn       = fn;
        _cookie   = cookie;
        _res      = PAMI_SUCCESS;

        this->_collObj = cookie;
        setDoneCallback(xfer->cb_done, xfer->cookie);

      TRACE_FN_EXIT();
    }


    virtual void start()
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT( "<%p>",this);
        T_Collective_type *coll_xfer = (T_Collective_type*)&_cmd.cmd;

        _res = doAction<T_Collective_type>(coll_xfer, _geometry);

        _fn( NULL, _cookie, _res);
      TRACE_FN_EXIT();
    }

    inline void * getCollObj()
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT("<%p> %p",this,_collObj);
      TRACE_FN_EXIT();
      return _collObj;
    }

protected:

    PAMI_GEOMETRY_CLASS *_geometry;
    pami_xfer_t          _cmd;
    pami_event_function  _fn;
    void                *_cookie;
    pami_result_t        _res;
    void                *_collObj;
}; //-OneTaskT


//////////////////////////////////////////////////////////////////////////////
};
};
};  //namespace CCMI::Adaptor::OneTask

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
