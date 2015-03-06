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
 * \file algorithms/protocols/allreduce/AsyncReduceScatterT.h
 * \brief ???
 */

#ifndef __algorithms_protocols_allreduce_AsyncReduceScatterT_h__
#define __algorithms_protocols_allreduce_AsyncReduceScatterT_h__

#include "algorithms/ccmi.h"
#include "algorithms/executor/Scatter.h"
#include "algorithms/connmgr/CommSeqConnMgr.h"
#include "algorithms/protocols/CollectiveProtocolFactory.h"
#include "algorithms/protocols/CollOpT.h"

#ifdef DEBUG
#undef DEBUG
#define DEBUG(x)  // fprintf x
#else
#define DEBUG(x)  // fprintf x
#endif

namespace CCMI
{
namespace Adaptor
{
namespace Allreduce
{

///
/// \brief Asyc Allreduce Composite. It is single color right now
///
template <class T_Reduce_Schedule, class T_Scatter_Schedule, class T_Conn, SFunc<PAMI_GEOMETRY_CLASS>::ScheduleFn create_schedule>
class AsyncReduceScatterT : public CCMI::Executor::Composite
{

protected:
    CCMI::Executor::AllreduceBaseExec<T_Conn>  _reduce_executor;
    T_Reduce_Schedule                          _reduce_schedule;
    CCMI::Executor::ScatterExec<T_Conn, T_Scatter_Schedule, pami_scatterv_t>
    _scatter_executor;
    T_Scatter_Schedule                         _scatter_schedule;
    T_Conn                                     *_cmgr;
    size_t                                     *_sdispls;
    size_t                                     *_scounts;
    char                                       *_tmpbuf;
    char                                       *_relbuf;
    int                                        _scatter_done;
    int                                        _reduce_done;

public:

    ///
    /// \brief Constructor
    ///
    AsyncReduceScatterT () {};
    AsyncReduceScatterT (pami_context_t               ctxt,
                         size_t                       ctxt_id,
                         Interfaces::NativeInterface   * native,
                         T_Conn                        * cmgr,
                         pami_callback_t                  cb_done,
                         PAMI_GEOMETRY_CLASS            * geometry,
                         void                           *cmd) :
        Executor::Composite(),
        _reduce_executor (native, cmgr, geometry->comm()),
        _reduce_schedule (native->endpoint(), (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX), 0),
        _scatter_executor (native, cmgr, geometry->comm(), (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX)),
        _cmgr(cmgr),
        _sdispls(NULL),
        _scounts(NULL),
        _tmpbuf(NULL),
        _scatter_done(0),
        _reduce_done(0)
    {
        TRACE_ADAPTOR ((stderr, "<%p>Allreduce::AsyncReduceScatterT() \n", this));

        PAMI::Topology *topo = (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);
        unsigned root = topo->index2Endpoint(0);
        coremath func;
        unsigned sizeOfType;
        pami_reduce_scatter_t *a_xfer = (pami_reduce_scatter_t *) & (((pami_xfer_t *)cmd)->cmd.xfer_reduce_scatter);
        uintptr_t op, dt;
        TypeCode * stype = (TypeCode *)a_xfer->stype;
        TypeCode * rtype = (TypeCode *)a_xfer->rtype;
        PAMI::Type::TypeFunc::GetEnums(a_xfer->stype,
                                       a_xfer->op,
                                       dt,op);

        if(dt == PAMI_BYTE)
          sizeOfType = sizeof(unsigned char);
        else
          CCMI::Adaptor::Allreduce::getReduceFunction((pami_dt)dt, (pami_op)op, sizeOfType, func);

        //SSS: I actually need size of type to be based on size of stype not size of dt for non-contigous support
        sizeOfType = stype->GetAtomSize();

        unsigned bytes = sizeOfType * a_xfer->stypecount;

        _reduce_executor.setRoot(root);

        prepReduceBuffers(a_xfer->sndbuf, a_xfer->rcvbuf, stype->GetExtent() * a_xfer->stypecount, native->endpoint() == root, stype, rtype);

        _reduce_executor.setDoneCallback (cb_done.function, cb_done.clientdata);

        COMPILE_TIME_ASSERT(sizeof(_reduce_schedule) >= sizeof(T_Reduce_Schedule));
        _reduce_executor.setSchedule (&_reduce_schedule, 0);
        _reduce_executor.setReduceInfo(a_xfer->stypecount, bytes, sizeOfType, func, stype, rtype, (pami_op)op, (pami_dt)dt);

        _reduce_executor.reset();

        _scatter_executor.setRoot(root);

        COMPILE_TIME_ASSERT(sizeof(_scatter_schedule) >= sizeof(T_Scatter_Schedule));
        create_schedule(&_scatter_schedule, sizeof(_scatter_schedule), root, native, geometry);
        _scatter_executor.setSchedule (&_scatter_schedule);
    }


    AsyncReduceScatterT (pami_context_t               ctxt,
                         size_t                       ctxt_id,
                         Interfaces::NativeInterface    * native,
                         T_Conn                         * cmgr,
                         pami_callback_t                  cb_done,
                         PAMI_GEOMETRY_CLASS            * geometry,
                         char                           * sndbuf,
                         char                           * rcvbuf,
                         size_t                           dt_count,
                         pami_dt                          dt,
                         pami_op                          op,
                         TypeCode                       * stype,
                         TypeCode                       * rtype) :
        Executor::Composite(),
        _reduce_executor (native, cmgr, geometry->comm()),
        _reduce_schedule (native->endpoint(), (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX), 0),
        _scatter_executor (native, cmgr, geometry->comm(), (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX)),
        _cmgr(cmgr),
        _sdispls(NULL),
        _scounts(NULL),
        _tmpbuf(NULL),
        _scatter_done(0),
        _reduce_done(0)
    {
        TRACE_ADAPTOR ((stderr, "<%p>Allreduce::AsyncReduceScatterT() \n", this));
        unsigned sizeOfType;
        coremath func;
        PAMI::Topology *topo = (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);
        unsigned root = topo->index2Endpoint(0);

        _reduce_executor.setRoot(root);

        CCMI::Adaptor::Allreduce::getReduceFunction(dt, op, sizeOfType, func);
        // SSS: We need sizeOfType based on stype/rtype not primitive type dt for non-contigous support;
        // however, most probably, this constructor will be called on EA and stype will not be known so leave
        // sizeOfType as is here.
        unsigned bytes = dt_count * sizeOfType;
        _reduce_executor.setBuffers (sndbuf, rcvbuf, bytes, bytes, stype, rtype);
        _reduce_executor.setDoneCallback (cb_done.function, cb_done.clientdata);

        COMPILE_TIME_ASSERT(sizeof(_reduce_schedule) >= sizeof(T_Reduce_Schedule));
        _reduce_executor.setSchedule (&_reduce_schedule, 0);
        _reduce_executor.setReduceInfo(dt_count, bytes, sizeOfType, func, stype, rtype, op, dt);

        _reduce_executor.reset();

        _scatter_executor.setRoot(root);

        COMPILE_TIME_ASSERT(sizeof(_scatter_schedule) >= sizeof(T_Scatter_Schedule));
        create_schedule(&_scatter_schedule, sizeof(_scatter_schedule), root, native, geometry);
        _scatter_executor.setSchedule (&_scatter_schedule);

    }

    ~AsyncReduceScatterT ()
    {
        __global.heap_mm->free(_sdispls);
        __global.heap_mm->free(_scounts);
        __global.heap_mm->free(_tmpbuf);
    }


    CCMI::Executor::AllreduceBaseExec<T_Conn> &getReduceExecutor()
    {
        return _reduce_executor;
    }

    CCMI::Executor::ScatterExec<T_Conn, T_Scatter_Schedule,  pami_scatterv_t> &getScatterExecutor()
    {
        return _scatter_executor;
    }

    void prepReduceBuffers(char *sndbuf, char *rcvbuf, size_t bytes, bool root, TypeCode * stype, TypeCode * rtype)
    {
        if (root)
        {
            pami_result_t rc;
            rc = __global.heap_mm->memalign((void **)&_tmpbuf, 0, bytes);
            PAMI_assertf(rc == PAMI_SUCCESS, "Failed to allocate %zu reduce buffers\n",bytes);
            _relbuf = rcvbuf;
            _reduce_executor.setBuffers (sndbuf, _tmpbuf, bytes, bytes, stype, rtype);
			
        }
        else
        {
            _reduce_executor.setBuffers(sndbuf, rcvbuf, bytes, bytes, stype, rtype);
        }
		
    }

    void setContext (pami_context_t context) {}	

    void setScatterExecutor (char *sbuf, char *rbuf, size_t *stypecounts, size_t endpointoffset,
                             TypeCode * stype, unsigned counts, bool isRoot, unsigned root,
                             pami_callback_t  cb_done)
    {

        pami_scatterv_t s_xfer;

        pami_result_t rc;
        rc = __global.heap_mm->memalign((void **)&_sdispls, 0, counts * sizeof(*_sdispls));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _sdispls");
        rc = __global.heap_mm->memalign((void **)&_scounts, 0, counts * sizeof(*_scounts));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _scounts");
        _sdispls[0] = 0;
        _scounts[0] = stypecounts[0];

        for (unsigned i = 0; i < counts-1; ++i)
        {
            _scounts[i+1] = stypecounts[i+1];
            _sdispls[i+1] = _sdispls[i] + stypecounts[i];
        }

        s_xfer.stypecounts = _scounts;
        s_xfer.sdispls     = _sdispls;
        s_xfer.stype       = stype;
        s_xfer.rtype       = stype;//In this case of scatter, stype and rtype are the reduce_scatter.rtype



        _scatter_executor.setConnmgr(_cmgr);
        _scatter_executor.setRoot (root);
        _scatter_executor.setSchedule (&_scatter_schedule);
        _scatter_executor.setVectors (&s_xfer);

        if (isRoot)
        {
            _scatter_executor.setBuffers(_tmpbuf, _relbuf, stypecounts[endpointoffset], stypecounts[endpointoffset], stype, stype);
        }
        else
        {
            _scatter_executor.setBuffers(sbuf, rbuf, stypecounts[endpointoffset], stypecounts[endpointoffset], stype, stype);
        }



        _scatter_executor.setDoneCallback (cb_done.function, cb_done.clientdata);

    }

    int &scatter_done_flag()
    {
        return _scatter_done;
    }

    int &reduce_done_flag()
    {
        return _reduce_done;
    }

    T_Conn *connmgr()
    {
        return _cmgr;
    }

    void connmgr(T_Conn *cmgr)
    {
        _cmgr = cmgr;
    }

}; //- AsyncReduceScatterT

template <class T_Composite, MetaDataFn get_metadata, class T_Conn, ConnectionManager::GetKeyFn getKey>
class AsyncReduceScatterFactoryT: public CollectiveProtocolFactory
{
protected:
    ///
    /// \brief get geometry from comm id
    ///
    pami_mapidtogeometry_fn      _cb_geometry;

    ///
    /// \brief free memory pool for async reduce scatter operation objects
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
     AsyncReduceScatterFactoryT (pami_context_t               ctxt,
                                 size_t                       ctxt_id,
                                 pami_mapidtogeometry_fn      cb_geometry,
                                 T_Conn                      *cmgr,
                                Interfaces::NativeInterface *native):
        CollectiveProtocolFactory(ctxt, ctxt_id, cb_geometry),
        _cmgr(cmgr),
        _native(native)
    {
        native->setMulticastDispatch(cb_async, this);
    }

    virtual ~AsyncReduceScatterFactoryT ()
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
        if(_native) _native->metadata(mdata,PAMI_XFER_REDUCE_SCATTER);
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
        return buf;
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
        PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *)g;
        T_Composite* a_composite = NULL;
        CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *co = NULL;
        pami_reduce_scatter_t *a_xfer = (pami_reduce_scatter_t *) & (((pami_xfer_t *)cmd)->cmd.xfer_reduce_scatter);
        PAMI::Topology *topo = (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);
        unsigned root = topo->index2Endpoint(0);
        size_t indexInTopo = topo->endpoint2Index(_native->endpoint());
        size_t numRanksInTopo = topo->size();
        TypeCode * stype = (TypeCode *)a_xfer->stype;
        TypeCode * rtype = (TypeCode *)a_xfer->rtype;

        T_Conn *cmgr = _cmgr;
        unsigned key;
        key = getKey(root, (unsigned) - 1, (PAMI_GEOMETRY_CLASS*)g, (ConnectionManager::BaseConnectionManager **) & cmgr);

        key = key << 1;
        cmgr = new T_Conn(key);

        co = (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *)geometry->asyncCollectiveUnexpQ(_native->contextid()).findAndDelete(key);

        /// Try to match in unexpected queue
        if (co)
        {
            DEBUG((stderr, "key = %d, found early arrival in unexpected queue\n", key);)

            CCMI_assert(co->getFlags() & EarlyArrival);

            co->setXfer((pami_xfer_t*)cmd);
            co->setFlag(LocalPosted);

            a_composite = co->getComposite();
            coremath func;
            uintptr_t op, reduce_dt;
            PAMI::Type::TypeFunc::GetEnums(a_xfer->stype,
                                           a_xfer->op,
                                           reduce_dt,op);
            unsigned sizeOfType;
            CCMI::Adaptor::Allreduce::getReduceFunction((pami_dt)reduce_dt, (pami_op)op, sizeOfType, func);
            sizeOfType = stype->GetAtomSize();
            unsigned bytes = sizeOfType * a_xfer->stypecount;
            a_composite->prepReduceBuffers(a_xfer->sndbuf, a_xfer->rcvbuf, stype->GetExtent() * a_xfer->stypecount, _native->endpoint() == root, stype, rtype);


            // previous connection manager may need cleanup
            CCMI_assert(co->getComposite()->connmgr() != ((AsyncReduceScatterFactoryT *)co->getFactory())->getConnMgr());
            delete co->getComposite()->connmgr();
            // and set new connection manager
            co->getComposite()->connmgr(cmgr);

            a_composite->getReduceExecutor().setReduceConnectionManager(cmgr);
            a_composite->getReduceExecutor().setBroadcastConnectionManager(cmgr);
            a_composite->getReduceExecutor().setReduceInfo(a_xfer->stypecount, bytes, sizeOfType, func, stype, rtype, (pami_op)op, (pami_dt)reduce_dt);

            a_composite->getReduceExecutor().reset();

            pami_callback_t  cb_exec_done;
            cb_exec_done.function   = scatter_exec_done;
            cb_exec_done.clientdata = co;

            a_composite->setScatterExecutor(a_xfer->sndbuf, a_xfer->rcvbuf, a_xfer->rcounts, indexInTopo, rtype, numRanksInTopo, _native->endpoint() == root, root, cb_exec_done);
            a_composite->getScatterExecutor().setConnectionID(key + 1);
        }
        /// not found posted CollOp object, create a new one and
        /// queue it in active queue
        else
        {
            DEBUG((stderr, "key = %d, no early arrival in unexpected queue, create new co\n", key);)

            co = _free_pool.allocate(key);
            pami_callback_t  cb_exec_done;
            cb_exec_done.function   = reduce_exec_done;
            cb_exec_done.clientdata = co;

            a_composite = new (co->getComposite())
            T_Composite ( this->_context,
                          this->_context_id,
                          _native,
                          cmgr,
                          cb_exec_done,
                          (PAMI_GEOMETRY_CLASS *)g,
                          cmd );
            cb_exec_done.function = scatter_exec_done;


            a_composite->setScatterExecutor(a_xfer->sndbuf, a_xfer->rcvbuf, a_xfer->rcounts, indexInTopo, rtype, numRanksInTopo, _native->endpoint() == root, root, cb_exec_done);

            co->setXfer((pami_xfer_t*)cmd);
            co->setFlag(LocalPosted);
            co->setFactory(this);
            co->setGeometry((PAMI_GEOMETRY_CLASS *)g);

            //Use the Key as the connection ID
            // if (cmgr == NULL) {
            //  a_composite->getReduceExecutor().setConnectionID(key);
            a_composite->getScatterExecutor().setConnectionID(key + 1);
            //}

        }

        geometry->asyncCollectivePostQ(_native->contextid()).pushTail(co);

        if (root != _native->endpoint())
        {
            DEBUG((stderr, "key = %d, start scatter executor in generate()\n", key);)
            a_composite->getScatterExecutor().start();
        }

        DEBUG((stderr, "key = %d, start reduce executor in generate()\n", key);)
        a_composite->getReduceExecutor().start();

        return NULL;
    }

    static void cb_async
    (pami_context_t          ctxt,
     const pami_quad_t     * info,
     unsigned                count,
     unsigned                org_conn_id,
     size_t                  peer,
     size_t                  sndlen,
     void                  * arg,
     size_t                * rcvlen,
     pami_pipeworkqueue_t ** rcvpwq,
     pami_callback_t       * cb_done)
    {

        unsigned conn_id = org_conn_id;
        conn_id = (conn_id >> 1) << 1;

        AsyncReduceScatterFactoryT *factory = (AsyncReduceScatterFactoryT *) arg;

        ExtCollHeaderData *cdata = (ExtCollHeaderData *) info;
        T_Composite* a_composite = NULL;

        int comm = cdata->_comm;
        PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *) factory->getGeometry (ctxt, comm);

        PAMI::Topology *topo = (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);
        unsigned root = topo->index2Endpoint(0);
        CCMI_assert(cdata->_root == root);

        T_Conn *cmgr;
        CCMI_assert(conn_id != -1U);
        unsigned key = getKey (root, conn_id, geometry, (ConnectionManager::BaseConnectionManager **) & cmgr);
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
            cmgr = new T_Conn(key);

            DEBUG((stderr, "key = %d, from %d, no local post or early arrival, create new co\n", key, peer);)
            co = factory->_free_pool.allocate(key);
            pami_callback_t cb_exec_done;
            cb_exec_done.function = reduce_exec_done;
            cb_exec_done.clientdata = co;

            a_composite = new (co->getComposite())
            T_Composite ( ctxt,
                          factory->getContextId(),
                          factory->_native,
                          cmgr,
                          cb_exec_done,
                          geometry,
                          NULL, NULL,
                          cdata->_count,
                          (pami_dt) cdata->_dt,
                          (pami_op) cdata->_op,
                          (TypeCode *) PAMI_TYPE_BYTE,
                          (TypeCode *) PAMI_TYPE_BYTE);

            co->setFlag(EarlyArrival);
            co->setFactory (factory);
            co->setGeometry(geometry);

            geometry->asyncCollectiveUnexpQ(factory->_native->contextid()).pushTail(co);
        }
        else
        {
            DEBUG((stderr, "key = %d, peer = %d, found existing co\n", key, peer);)
            a_composite = (T_Composite *) co->getComposite();
            // CCMI_assert (a_composite->executor().getBytes() == sndlen);
        }

        if (org_conn_id % 2)
        {
            CCMI_assert(peer == root);
            DEBUG((stderr, "key = %d, calling notifyRecv in cb_async()\n", key);)
            a_composite->getScatterExecutor().notifyRecv(peer, *info, (PAMI::PipeWorkQueue **)rcvpwq, cb_done);
            *rcvlen = sndlen;
        }
        else
        {
            DEBUG((stderr, "key = %d, calling notifyRecvHead in cb_async()\n", key);)
            a_composite->getReduceExecutor().notifyRecvHead(info, count,
                    conn_id, peer, sndlen, arg, rcvlen,
                    rcvpwq, cb_done);
        }

        return;
    }


    static void reduce_exec_done(pami_context_t context, void *cd, pami_result_t err)
    {
        CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> * co =
            (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *)cd;

        DEBUG((stderr, "key = %d, reduce execution done, clean up\n", co->key());)

        T_Composite * a_composite = co->getComposite();

        CCMI_assert(a_composite->reduce_done_flag() == 0);
        a_composite->reduce_done_flag() = 1;

        AsyncReduceScatterFactoryT *factory = (AsyncReduceScatterFactoryT *)co->getFactory();

        if (a_composite->scatter_done_flag() == 1)
        {
            factory->exec_done(context, cd, err);
            return;
        }

        PAMI_GEOMETRY_CLASS *geometry = co->getGeometry();
        PAMI::Topology *topo = (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);
        unsigned root = topo->index2Endpoint(0);

        if (factory->_native->endpoint() == root)
            a_composite->getScatterExecutor().start();

        return;

    }


    static void scatter_exec_done (pami_context_t context, void *cd, pami_result_t err)
    {
        CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> * co =
            (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *)cd;

        //fprintf (stderr, "%d: exec_done for key %d\n", ((AsyncReduceScatterFactoryT *)co->getFactory())->_native->myrank(), co->key());

        DEBUG((stderr, "key = %d, scatter execution done, clean up\n", co->key());)

        T_Composite * a_composite = co->getComposite();

        CCMI_assert(a_composite->scatter_done_flag() == 0);
        a_composite->scatter_done_flag() = 1;

        AsyncReduceScatterFactoryT *factory = (AsyncReduceScatterFactoryT *)co->getFactory();

        if (a_composite->reduce_done_flag() == 1) factory->exec_done(context, cd, err);

        return;

    }

    static void exec_done (pami_context_t context, void *cd, pami_result_t err)
    {
        CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> * co =
            (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *)cd;

        //fprintf (stderr, "%d: exec_done for key %d\n", ((AsyncReduceScatterFactoryT *)co->getFactory())->_native->myrank(), co->key());

        DEBUG((stderr, "key = %d, scatter execution done, clean up\n", co->key());)

        PAMI_GEOMETRY_CLASS *geometry = co->getGeometry();
        unsigned flag;
        flag = co->getFlags();

        CCMI_assert(flag & LocalPosted);

        pami_xfer_t *xfer = co->getXfer();
        AsyncReduceScatterFactoryT *factory = (AsyncReduceScatterFactoryT *)co->getFactory();

        // actviate user callback
        if (xfer->cb_done)
            xfer->cb_done(co->getComposite()->getContext()?co->getComposite()->getContext():factory->_context,
                          xfer->cookie, PAMI_SUCCESS);

        // must be on the posted queue, dequeue it
        geometry->asyncCollectivePostQ(factory->_native->contextid()).deleteElem(co);

        // connection manager may need cleanup
        CCMI_assert(co->getComposite()->connmgr() != factory->getConnMgr());
        delete co->getComposite()->connmgr();
        // }

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
