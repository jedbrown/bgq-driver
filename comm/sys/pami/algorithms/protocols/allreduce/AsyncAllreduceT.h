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
 * \file algorithms/protocols/allreduce/AsyncAllreduceT.h
 * \brief ???
 */

#ifndef __algorithms_protocols_allreduce_AsyncAllreduceT_h__
#define __algorithms_protocols_allreduce_AsyncAllreduceT_h__

#include "algorithms/ccmi.h"
#include "algorithms/connmgr/CommSeqConnMgr.h"
#include "algorithms/protocols/CollectiveProtocolFactory.h"
#include "algorithms/protocols/CollOpT.h"
#include "algorithms/protocols/allreduce/ReduceFunctions.h"

//  #define DEBUG(x) fprintf x
#define DEBUG(x)

namespace CCMI
{
namespace Adaptor
{
namespace Allreduce
{

template <class T_reduce_type>
inline void _get_xfer_and_root(T_reduce_type **xfer, pami_collective_t *coll, unsigned &root)
{
// Clang performs syntax checks even for non-intantianated templates.
// This assertion fail needs to be deferred from syntax-checking to a possible instantianation attempt.
    COMPILE_TIME_ASSERT(sizeof(T_reduce_type) == 0);
}

template <>
inline void _get_xfer_and_root<pami_reduce_t>(pami_reduce_t **xfer, pami_collective_t *coll, unsigned &root)
{
    pami_reduce_t *lxfer   = &(coll->xfer_reduce);

    if (root == (unsigned) - 1)
        root = lxfer->root; // in case coll is not initialized
    else
        lxfer->root = root;

    *xfer = lxfer;
}

template <>
inline void _get_xfer_and_root<pami_allreduce_t>(pami_allreduce_t **xfer, pami_collective_t *coll, unsigned &root)
{
    *xfer   =  &(coll->xfer_allreduce);
    root   = (unsigned) - 1;
}

///
/// \brief Asyc Allreduce Composite. It is single color right now
///
template <class T_Schedule, class T_Conn, class T_reduce_type>
class AsyncAllreduceT : public CCMI::Executor::Composite
{
protected:
    CCMI::Executor::AllreduceBaseExec<T_Conn>  _executor __attribute__((__aligned__(16)));
    T_Schedule                                 _schedule;
    T_Conn                                     *_cmgr;
    unsigned                                   _bytes;

public:

    ///
    /// \brief Constructor
    ///
    AsyncAllreduceT () {};
    AsyncAllreduceT (pami_context_t               ctxt,
                     size_t                       ctxt_id,
                     Interfaces::NativeInterface   * native,
                     T_Conn                        * cmgr,
                     pami_callback_t                  cb_done,
                     PAMI_GEOMETRY_CLASS            * geometry,
                     void                           *cmd) :
        Executor::Composite(),
        _executor (native, cmgr, geometry->comm()),
        _schedule (native->endpoint(), (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX), 0),
        _cmgr(cmgr)
    {
        TRACE_ADAPTOR ((stderr, "<%p>Allreduce::AsyncAllreduceT() \n", this));

        unsigned root = (unsigned) - 1;
        coremath func;
        unsigned sizeOfType;
        T_reduce_type *a_xfer;
        _get_xfer_and_root(&a_xfer, (pami_collective_t *)cmd, root);
        _executor.setRoot(root);

        uintptr_t op, dt;
        PAMI::Type::TypeFunc::GetEnums(a_xfer->stype,
                                       a_xfer->op,
                                       dt,op);

        CCMI::Adaptor::Allreduce::getReduceFunction((pami_dt)dt, (pami_op)op, sizeOfType, func);
        sizeOfType = ((TypeCode *)a_xfer->stype)->GetAtomSize();
        unsigned bytes = a_xfer->stypecount * sizeOfType;
        // unsigned bytes = sizeOfType * a_xfer->stypecount;
        _executor.setBuffers (a_xfer->sndbuf, a_xfer->rcvbuf, a_xfer->stypecount, a_xfer->stypecount, (TypeCode *)a_xfer->stype, (TypeCode *)a_xfer->rtype);
        _executor.setDoneCallback (cb_done.function, cb_done.clientdata);

        COMPILE_TIME_ASSERT(sizeof(_schedule) >= sizeof(T_Schedule));
        _executor.setSchedule (&_schedule, 0);
        _executor.setReduceInfo(a_xfer->stypecount,
                                bytes,
                                sizeOfType, func, (TypeCode *)a_xfer->stype, (TypeCode *)a_xfer->rtype,
                                (pami_op)op, (pami_dt)dt);

        _executor.reset();
    }

    AsyncAllreduceT (pami_context_t                 ctxt,
                     size_t                         ctxt_id,
                     Interfaces::NativeInterface  * native,
                     T_Conn                       * cmgr,
                     pami_callback_t                cb_done,
                     PAMI_GEOMETRY_CLASS          * geometry,
                     char                         * sndbuf,
                     char                         * rcvbuf,
                     unsigned                       root,
                     size_t                         dt_count,
                     pami_dt                        dt,
                     pami_op                        op,
                     TypeCode                     * stype,
                     TypeCode                     * rtype) :
        Executor::Composite(),
        _executor (native, cmgr, geometry->comm()),
        _schedule (native->endpoint(), (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX), 0),
        _cmgr(cmgr)
    {
        TRACE_ADAPTOR ((stderr, "<%p>Allreduce::AsyncAllreduceT() \n", this));
        unsigned sizeOfType;
        coremath func;

        _executor.setRoot(root);

        // ??? Why would getReduceFunction need data size ? use dt_count for now
        CCMI::Adaptor::Allreduce::getReduceFunction(dt, op, sizeOfType, func);
        // SSS: We need sizeOfType based on stype/rtype not primitive type dt for non-contigous support;
        // however, most probably, this constructor will be called on EA and stype will not be known so leave
        // sizeOfType as is here.
        unsigned bytes = dt_count * sizeOfType;
        _executor.setBuffers (sndbuf, rcvbuf, bytes, bytes, stype, rtype);
        _executor.setDoneCallback (cb_done.function, cb_done.clientdata);

        COMPILE_TIME_ASSERT(sizeof(_schedule) >= sizeof(T_Schedule));
        _executor.setSchedule (&_schedule, 0);
        _executor.setReduceInfo(dt_count, bytes, sizeOfType, func, stype, rtype, op, dt);

        _executor.reset();
    }


    CCMI::Executor::AllreduceBaseExec<T_Conn> &executor()
    {
        return _executor;
    }

    T_Conn *connmgr()
    {
        return _cmgr;
    }

    void setContext (pami_context_t context) {}

}; //- AsyncAllreduceT

template <class T_Composite, MetaDataFn get_metadata, class T_Conn, class T_reduce_type, ConnectionManager::GetKeyFn getKey>
class AsyncAllreduceFactoryT: public CollectiveProtocolFactory
{
protected:
    ///
    /// \brief get geometry from comm id
    ///
    pami_mapidtogeometry_fn      _cb_geometry;

    ///
    /// \brief free memory pool for async allreduce operation objects
    ///
    CCMI::Adaptor::CollOpPoolT<pami_xfer_t,  T_Composite>   _free_pool;

    ///
    /// \brief memory allocator for early arrival descriptors
    ///
    PAMI::MemoryAllocator < sizeof(EADescriptor), 16 > _ead_allocator;

    ///
    /// \brief memory allocator for early arrival buffers
    ///
    PAMI::MemoryAllocator<32768, 16>                 _eab_allocator;

    T_Conn                                        * _cmgr;
    Interfaces::NativeInterface                   * _native;

public:
  AsyncAllreduceFactoryT (pami_context_t               ctxt,
                          size_t                       ctxt_id,
                          pami_mapidtogeometry_fn      cb_geometry,
                          T_Conn                      *cmgr,
                            Interfaces::NativeInterface *native):
        CollectiveProtocolFactory(ctxt,ctxt_id,cb_geometry),
        _cmgr(cmgr),
        _native(native)
    {
        native->setMulticastDispatch(cb_async, this);
    }

    virtual ~AsyncAllreduceFactoryT ()
    {
    }

    /// NOTE: This is required to make "C" programs link successfully with virtual destructors
    void operator delete(void * p)
    {
        CCMI_abort();
    }

    virtual void metadata(pami_metadata_t *mdata, pami_geometry_t geometry = PAMI_GEOMETRY_NULL)
    {
        // TRACE_ADAPTOR((stderr,"%s\n", __PRETTY_FUNCTION__));
        DO_DEBUG((templateName<MetaDataFn>()));
        get_metadata(mdata);
        if(_native) _native->metadata(mdata,PAMI_XFER_ALLREDUCE);
    }

    T_Conn *getConnMgr()
    {
        return _cmgr;
    }

    //Override the connection manager in this call
    /*
    unsigned getKey   (unsigned                 root,
     unsigned                 connid,
     PAMI_GEOMETRY_CLASS    * geometry,
     T_Conn                ** connmgr)
    {
    CCMI_abort();
    return root;
    }
    */

    char *allocateBuffer (unsigned size)
    {
        if (size <= 32768)
            return (char *)_eab_allocator.allocateObject();

        char *buf;
        pami_result_t rc;
        rc = __global.heap_mm->memalign((void **)&buf, 0, size);
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to allocate %u async buffer\n",size);
        return rc == PAMI_SUCCESS ? buf : NULL;
    }

    void freeBuffer (unsigned size, char *buf)
    {
        if (size <= 32768)
            return _eab_allocator.returnObject(buf);

        __global.heap_mm->free(buf);
    }

    virtual Executor::Composite * generate(pami_geometry_t              g,
                                           void                      * cmd)
    {
        T_Composite* a_composite = NULL;
        CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *co = NULL;
        T_reduce_type *a_xfer;
        unsigned root = (unsigned) - 1;
        _get_xfer_and_root(&a_xfer, &(((pami_xfer_t*)cmd)->cmd), root);

        PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *)g;
        T_Conn *cmgr = _cmgr;
        unsigned key;
        key = getKey(root, (unsigned) - 1, (PAMI_GEOMETRY_CLASS*)g, (ConnectionManager::BaseConnectionManager **) & cmgr);

        co = (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *)geometry->asyncCollectiveUnexpQ(_native->contextid()).findAndDelete(key);

        /// Try to match in unexpected queue
        if (co)
        {
            unsigned sizeOfType;
            coremath func;
            uintptr_t op, dt;
            PAMI::Type::TypeFunc::GetEnums(a_xfer->stype,
                                           a_xfer->op,
                                           dt,op);

            CCMI::Adaptor::Allreduce::getReduceFunction((pami_dt)dt, (pami_op)op, sizeOfType, func);
            sizeOfType = ((TypeCode *)a_xfer->stype)->GetAtomSize();
            unsigned bytes = a_xfer->stypecount * sizeOfType;

            DEBUG((stderr, "key = %d, found early arrival in unexpected queue\n", key);)
            CCMI_assert(co->getFlags() & EarlyArrival);

            co->setXfer((pami_xfer_t*)cmd);
            co->setFlag(LocalPosted);

            a_composite = co->getComposite();
            a_composite->executor().setBuffers(a_xfer->sndbuf, a_xfer->rcvbuf, 0, 0, (TypeCode *)a_xfer->stype, (TypeCode *)a_xfer->rtype); // need number of bytes ???
            a_composite->executor().setReduceConnectionManager(_cmgr);
            a_composite->executor().setBroadcastConnectionManager(_cmgr);
            a_composite->executor().setReduceInfo(a_xfer->stypecount, bytes, sizeOfType, func, (TypeCode *)a_xfer->stype, (TypeCode *)a_xfer->rtype, (pami_op)op, (pami_dt)dt);
            a_composite->executor().reset();
        }
        /// not found posted CollOp object, create a new one and
        /// queue it in active queue
        else
        {
            DEBUG((stderr, "key = %d, no early arrival in unexpected queue, create new co\n", key);)

            co = _free_pool.allocate(key);
            pami_callback_t  cb_exec_done;
            cb_exec_done.function   = exec_done;
            cb_exec_done.clientdata = co;

            a_composite = new (co->getComposite())
            T_Composite ( this->_context,
                          this->_context_id,
                          _native,
                          cmgr,
                          cb_exec_done,
                          (PAMI_GEOMETRY_CLASS *)g,
                          &(((pami_xfer_t *)cmd)->cmd));

            co->setXfer((pami_xfer_t*)cmd);
            co->setFlag(LocalPosted);
            co->setFactory(this);
            co->setGeometry((PAMI_GEOMETRY_CLASS *)g);
            //Use the Key as the connection ID
            // if (cmgr == NULL)
            //   a_composite->executor().setConnectionID(key);

        }

        geometry->asyncCollectivePostQ(_native->contextid()).pushTail(co);
        DEBUG((stderr, "key = %d, start executor in generate()\n", key););
        a_composite->executor().start();
		    
        return NULL;
    }

    static void cb_async
    (pami_context_t          ctxt,
     const pami_quad_t     * info,
     unsigned                count,
     unsigned                conn_id,
     size_t                  peer,
     size_t                  sndlen,
     void                  * arg,
     size_t                * rcvlen,
     pami_pipeworkqueue_t ** rcvpwq,
     pami_callback_t       * cb_done)
    {
        AsyncAllreduceFactoryT *factory = (AsyncAllreduceFactoryT *) arg;
        //fprintf(stderr, "%d: <%#.8X>Allreduce::AsyncAllreduceFactoryT::cb_async() connid %d\n",factory->_native->myrank(), (int)factory, conn_id);

        ExtCollHeaderData *cdata = (ExtCollHeaderData *) info;
        T_Composite* a_composite = NULL;

        int comm = cdata->_comm;
        PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *) factory->getGeometry (ctxt, comm);

        T_Conn *cmgr = factory->getConnMgr();
        unsigned key;
        unsigned root = cdata->_root;
        pami_collective_t cmd;
        cmd.xfer_reduce.root = (unsigned)-1;
        T_reduce_type *a_xfer;
        // a_xfer->root gets updated by the call if the operation is reduce
        _get_xfer_and_root(&a_xfer, &cmd, root);

        CCMI_assert(conn_id != -1U);
        key = getKey (root, conn_id, geometry, (ConnectionManager::BaseConnectionManager **) & cmgr);
        CCMI_assert(cmgr == NULL); // ? Why rely on getkey to null it?

        CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *co =
          (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *) geometry->asyncCollectivePostQ(factory->_native->contextid()).find(key);

        if (!co)
        {
            // it is still possible that there are other early arrivals
            DEBUG((stderr, "key = %d, no local post, try early arrival\n", key);)
              co = (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *)geometry->asyncCollectiveUnexpQ(factory->_native->contextid()).find(key);
        }

        if (!co)
        {
            //Use the Key as the connection ID
            cmgr = new T_Conn(key); // Remember to dtor EarlyArrival cmgr later

            DEBUG((stderr, "key = %d no local post or early arrival, create new co\n", key);)
            co = factory->_free_pool.allocate(key);
            pami_callback_t cb_exec_done;
            cb_exec_done.function = exec_done;
            cb_exec_done.clientdata = co;

            CCMI_assert ( ctxt == factory->getContext() );//FIXME:: This need to be true
            a_composite = new (co->getComposite())
            T_Composite ( ctxt,
                          factory->getContextId(),
                          factory->_native,
                          cmgr,
                          cb_exec_done,
                          geometry,
                          NULL, NULL,
                          (unsigned)cdata->_root,
                          cdata->_count,
                          (pami_dt) cdata->_dt,
                          (pami_op) cdata->_op,
                          (TypeCode *) PAMI_TYPE_BYTE,
                          (TypeCode *) PAMI_TYPE_BYTE);//SSS: This will be reset to the correct datatype when posted

            co->setFlag(EarlyArrival);
            co->setFactory (factory);
            co->setGeometry(geometry);

            geometry->asyncCollectiveUnexpQ(factory->_native->contextid()).pushTail(co);
        }
        else
        {
            DEBUG((stderr, "key = %d, found existing co\n", key);)
            a_composite = (T_Composite *) co->getComposite();
            // CCMI_assert (a_composite->executor().getBytes() == sndlen);
        }

        DEBUG((stderr, "key = %d, calling notifyRecvHead in cb_async()\n", key);)
        a_composite->executor().notifyRecvHead(info, count,
                                               conn_id, peer, sndlen, arg, rcvlen,
                                               rcvpwq, cb_done);
        return;
    }

    static void exec_done (pami_context_t context, void *cd, pami_result_t err)
    {
        CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> * co =
            (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *)cd;

        //fprintf (stderr, "%d: exec_done for key %d\n", ((AsyncAllreduceFactoryT *)co->getFactory())->_native->myrank(), co->key());

        DEBUG((stderr, "key = %d, execution done, clean up\n", co->key());)

        PAMI_GEOMETRY_CLASS *geometry = co->getGeometry();
        unsigned                 flag = co->getFlags();

        CCMI_assert(flag & LocalPosted);

        pami_xfer_t *xfer = co->getXfer();
        AsyncAllreduceFactoryT *factory = (AsyncAllreduceFactoryT *)co->getFactory();

        // acviate user callback
        if (xfer->cb_done)
            xfer->cb_done(co->getComposite()->getContext()?co->getComposite()->getContext():factory->_context,
                          xfer->cookie, PAMI_SUCCESS);

        // must be on the posted queue, dequeue it
        geometry->asyncCollectivePostQ(factory->_native->contextid()).deleteElem(co);

        // connection manager may need cleanup because we ctor it in cb_async
        if (flag & EarlyArrival)
        {
            CCMI_assert(co->getComposite()->connmgr() != factory->getConnMgr());
            delete co->getComposite()->connmgr();
        }

        // free the CollOp object
        factory->_free_pool.free(co);
    }

}; //- Async Composite Factory
};  //- end namespace Allreduce
};  //- end namespace Adaptor
};  //- end CCMI


#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
