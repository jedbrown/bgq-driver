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
 * \file algorithms/protocols/allgather/AsyncAllgatherT.h
 * \brief ???
 */

#ifndef __algorithms_protocols_allgather_AsyncAllgatherT_h__
#define __algorithms_protocols_allgather_AsyncAllgatherT_h__

#include "Global.h"
#include "algorithms/ccmi.h"
#include "algorithms/executor/Allgather.h"
#include "algorithms/connmgr/CommSeqConnMgr.h"
#include "algorithms/protocols/CollectiveProtocolFactory.h"
#include "algorithms/protocols/CollOpT.h"

#ifdef DEBUG
#undef DEBUG
#define DEBUG(x) // fprintf x
#else
#define DEBUG(x) // fprintf x
#endif

namespace CCMI
{
namespace Adaptor
{
namespace Allgather
{

///
/// \brief Asyc Allgather Composite.
///
template <class T_Schedule, class T_Conn, SFunc<PAMI_GEOMETRY_CLASS>::ScheduleFn create_schedule>
class AsyncAllgatherT : public CCMI::Executor::Composite
{

protected:
    CCMI::Executor::AllgatherExec<T_Conn, T_Schedule>  _executor __attribute__((__aligned__(16)));
    T_Schedule                                         _schedule;
    T_Conn                                            *_cmgr;

public:

    ///
    /// \brief Constructor
    ///
    AsyncAllgatherT () {};
    AsyncAllgatherT (pami_context_t               ctxt,
                     size_t                       ctxt_id,
                     Interfaces::NativeInterface   * native,
                     T_Conn                        * cmgr,
                     pami_callback_t                 cb_done,
                     PAMI_GEOMETRY_CLASS           * geometry,
                     void                          * cmd) :
        Executor::Composite(),
        _executor (native, cmgr, geometry->comm(), (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX)),
        _cmgr(cmgr)
    {
        TRACE_ADAPTOR ((stderr, "<%p>Allgather::AsyncAllgatherT() \n", this));
        pami_allgather_t *a_xfer = &((pami_xfer_t *)cmd)->cmd.xfer_allgather;
        PAMI::Type::TypeCode * stype = (PAMI::Type::TypeCode *)a_xfer->stype;
        PAMI::Type::TypeCode * rtype = (PAMI::Type::TypeCode *)a_xfer->rtype;

        COMPILE_TIME_ASSERT(sizeof(_schedule) >= sizeof(T_Schedule));
        create_schedule(&_schedule, sizeof(_schedule), (unsigned) - 1, native, geometry);
        _executor.setSchedule (&_schedule);
        _executor.setBuffers (a_xfer->sndbuf, a_xfer->rcvbuf, a_xfer->rtypecount, stype, rtype);
        _executor.setDoneCallback (cb_done.function, cb_done.clientdata);
    }

    AsyncAllgatherT (pami_context_t               ctxt,
                     size_t                       ctxt_id,
                     Interfaces::NativeInterface   * native,
                     T_Conn                        * cmgr,
                     pami_callback_t                  cb_done,
                     PAMI_GEOMETRY_CLASS            * geometry,
                     char                           * sndbuf,
                     pami_type_t                    stype,
                     size_t                         stypecount,
                     char                           * rcvbuf,
                     pami_type_t                    rtype,
                     size_t                         rtypecount) :
        Executor::Composite(),
        _executor (native, cmgr, geometry->comm(), (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX)),
        _cmgr(cmgr)
    {
        TRACE_ADAPTOR ((stderr, "<%p>Allgather::AsyncAllgatherT() \n", this));
        PAMI::Type::TypeCode * sndtype = (PAMI::Type::TypeCode *)stype;
        PAMI::Type::TypeCode * rcvtype = (PAMI::Type::TypeCode *)rtype;

        COMPILE_TIME_ASSERT(sizeof(_schedule) >= sizeof(T_Schedule));
        create_schedule(&_schedule, sizeof(_schedule), (unsigned) - 1, native, geometry);
        _executor.setSchedule (&_schedule);
        _executor.setBuffers (sndbuf, rcvbuf, rtypecount, sndtype, rcvtype);
        _executor.setDoneCallback (cb_done.function, cb_done.clientdata);

    }

    CCMI::Executor::AllgatherExec<T_Conn, T_Schedule> &executor()
    {
        return _executor;
    }

    T_Conn *connmgr()
    {
        return _cmgr;
    }

}; //- AsyncAllgatherT

template <class T_Composite, MetaDataFn get_metadata, class T_Conn, ConnectionManager::GetKeyFn getKey>
class AsyncAllgatherFactoryT: public CollectiveProtocolFactory
{
protected:
    ///
    /// \brief get geometry from comm id
    ///
    pami_mapidtogeometry_fn      _cb_geometry;

    ///
    /// \brief free memory pool for async allgather operation objects
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
    AsyncAllgatherFactoryT (pami_context_t               ctxt,
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

    virtual ~AsyncAllgatherFactoryT ()
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
        if(_native) _native->metadata(mdata,PAMI_XFER_ALLGATHER);
    }

    T_Conn *getConnMgr()
    {
        return _cmgr;
    }

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
        pami_allgather_t *a_xfer =  &((pami_xfer_t *)cmd)->cmd.xfer_allgather;
        PAMI::Type::TypeCode * stype = (PAMI::Type::TypeCode *)a_xfer->stype;
        PAMI::Type::TypeCode * rtype = (PAMI::Type::TypeCode *)a_xfer->rtype;

        PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *)g;
        T_Conn *cmgr = _cmgr;
        unsigned key;
        key = getKey((unsigned) - 1, (unsigned) - 1, (PAMI_GEOMETRY_CLASS*)g, (ConnectionManager::BaseConnectionManager **) & cmgr);
        co = (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *)geometry->asyncCollectiveUnexpQ(_native->contextid()).findAndDelete(key);

        /// Try to match in unexpected queue
        if (co)
        {

            DEBUG((stderr, "key = %d, found early arrival in unexpected queue\n", key);)

            CCMI_assert(co->getFlags() & EarlyArrival);

            co->setXfer((pami_xfer_t*)cmd);
            co->setFlag(LocalPosted);

            a_composite = co->getComposite();
            // update send buffer pointer and, at root, receive buffer pointers
            a_composite->executor().updateBuffers(a_xfer->sndbuf, a_xfer->rcvbuf, a_xfer->rtypecount, stype, rtype);
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
                          (void *)cmd);

            co->setXfer((pami_xfer_t*)cmd);
            co->setFlag(LocalPosted);
            co->setFactory(this);
            co->setGeometry((PAMI_GEOMETRY_CLASS *)g);

            //Use the Key as the connection ID
            if (cmgr == NULL)
                a_composite->executor().setConnectionID(key);

        }

        geometry->asyncCollectivePostQ(_native->contextid()).pushTail(co);
        DEBUG((stderr, "key = %d, start executor in generate()\n", key);)
        a_composite->executor().start();

        return NULL;
    }

    static void cb_async
    (pami_context_t         ctxt,
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
        AsyncAllgatherFactoryT *factory = (AsyncAllgatherFactoryT *) arg;
        CollHeaderData *cdata = (CollHeaderData *) info;
        T_Composite* a_composite = NULL;

        int comm = cdata->_comm;
        PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *) factory->getGeometry (ctxt, comm);

        T_Conn *cmgr = factory->getConnMgr();
        unsigned key;
        CCMI_assert(conn_id != -1U);
        key = getKey (cdata->_root, conn_id, geometry, (ConnectionManager::BaseConnectionManager **) & cmgr);
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
            DEBUG((stderr, "key = %d no local post or early arrival, create new co\n", key);)
            co = factory->_free_pool.allocate(key);
            pami_callback_t cb_exec_done;
            cb_exec_done.function = exec_done;
            cb_exec_done.clientdata = co;

            a_composite = new (co->getComposite())
            T_Composite ( ctxt,
                          factory->getContextId(),
                          factory->_native,
                          cmgr,
                          cb_exec_done,
                          geometry,
                          NULL,
                          PAMI_TYPE_BYTE,
                          cdata->_count,
                          NULL,
                          PAMI_TYPE_BYTE,
                          cdata->_count);

            if (cmgr == NULL)
                a_composite->executor().setConnectionID(key);

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

        DEBUG((stderr, "key = %d, calling notifyRecv in cb_async()\n", key);)

        *rcvlen = sndlen;
        a_composite->executor().notifyRecv(peer, *info, (PAMI::PipeWorkQueue **)rcvpwq, cb_done);
        return;
    }

    static void exec_done (pami_context_t context, void *cd, pami_result_t err)
    {
        CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> * co =
            (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *)cd;
        DEBUG((stderr, "key = %d, execution done, clean up\n", co->key());)

        PAMI_GEOMETRY_CLASS *geometry = co->getGeometry();
        unsigned                 flag;
        flag = co->getFlags();

        CCMI_assert(flag & LocalPosted);

        pami_xfer_t *xfer = co->getXfer();
        AsyncAllgatherFactoryT *factory = (AsyncAllgatherFactoryT *)co->getFactory();

        // activate user callback
        if (xfer->cb_done)
            xfer->cb_done(co->getComposite()->getContext()?co->getComposite()->getContext():factory->_context, 
                          xfer->cookie, PAMI_SUCCESS);

        // must be on the posted queue, dequeue it
        geometry->asyncCollectivePostQ(factory->_native->contextid()).deleteElem(co);

        // free the CollOp object
        factory->_free_pool.free(co);
    }

}; //- Async Composite Factory
}  //- end namespace Allgather
}  //- end namespace Adaptor
}  //- end CCMI


#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
