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
 * \file algorithms/protocols/alltoall/All2All.h
 * \brief Simple composite based on multicast
 */
#ifndef __algorithms_protocols_alltoall_All2All_h__
#define __algorithms_protocols_alltoall_All2All_h__

#include "algorithms/composite/Composite.h"
#include "util/ccmi_util.h"
#include "components/devices/ManytomanyModel.h"

namespace CCMI
{
namespace Adaptor
{
class All2AllProtocol: public CCMI::Executor::Composite
{
protected:
    Interfaces::NativeInterface *_native;
    PAMI_GEOMETRY_CLASS   * _geometry;
    pami_manytomanybuf_t    _send;
    pami_manytomanybuf_t    _recv;
    pami_manytomany_t       _m2m_info;
    pami_callback_t         _my_cb_done;
    pami_callback_t         _app_cb_done;
    //size_t _my_index;
    size_t                * _sendinit;
    size_t                * _recvinit;
    void                  * _initbuf;
    void                  * _sbuf;
    int                     _in_place;
    unsigned                _donecount;
    PAMI::M2MPipeWorkQueueT<size_t, 1>  _sendpwq;
    PAMI::M2MPipeWorkQueueT<size_t, 1>  _recvpwq;
    //CollHeaderData          _metadata;
public:
    All2AllProtocol() {};
    All2AllProtocol(Interfaces::NativeInterface *mInterface,
                    CCMI::ConnectionManager::CommSeqConnMgr *cmgr,
                    pami_geometry_t g,
                    pami_xfer_t *coll,
                    pami_callback_t cb_done):
        CCMI::Executor::Composite(),
        _native(mInterface),
        _geometry((PAMI_GEOMETRY_CLASS*)g),
        _app_cb_done(cb_done),
        _in_place(0)
    {
        (void)cmgr;
        //pami_task_t self = __global.mapping.task();
        PAMI::Topology * all = (PAMI::Topology *)_geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);
        size_t topo_size = all->size();
        TRACE_ADAPTOR((stderr, "<%p>All2AllProtocol size %zu, stypecount %zu, rtypecount %zu\n", this, topo_size, coll->cmd.xfer_alltoall.stypecount, coll->cmd.xfer_alltoall.rtypecount));

        if(coll->cmd.xfer_alltoall.sndbuf == PAMI_IN_PLACE)
          _in_place = 1;
        TypeCode * stype = _in_place?
                           (TypeCode *)coll->cmd.xfer_alltoall.rtype:
                           (TypeCode *)coll->cmd.xfer_alltoall.stype;
        TypeCode * rtype = (TypeCode *)coll->cmd.xfer_alltoall.rtype;

        _my_cb_done.function = a2aDone;
        _my_cb_done.clientdata = this;
        _donecount = 0;

        /// \todo only supporting PAMI_TYPE_BYTE right now
        //PAMI_assert(coll->cmd.xfer_alltoall.stype == PAMI_TYPE_BYTE);

        pami_result_t rc;
        rc = __global.heap_mm->memalign(&_initbuf, 0, 2 * sizeof(size_t) * topo_size);
        _sendinit =  (size_t *) _initbuf;
        _recvinit =  (size_t *) ((char *)_initbuf + sizeof(size_t) * topo_size);

        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _sendinit");

        size_t scount;
        if(_in_place)
        {
          scount = coll->cmd.xfer_alltoall.rtypecount;
          _sbuf  = __global.heap_mm->malloc(stype->GetDataSize() * scount * topo_size);
          PAMI_assert_alwaysf(_sbuf != NULL, "Failed to allocate memory at %s %d",__FILE__,__LINE__);
          /* Using a memcpy here since we don't support non-contig. Once non-contig is supported
             this should be modified to be PAMI_Type_transform_data */
          memcpy(_sbuf, coll->cmd.xfer_alltoall.rcvbuf, stype->GetDataSize() * scount * topo_size);
        }
        else
        {
          scount = coll->cmd.xfer_alltoall.stypecount;
          _sbuf  = coll->cmd.xfer_alltoall.sndbuf;
        }
        for (size_t i = 0; i < topo_size; ++i)
        {
            _sendinit[i] = scount * stype->GetDataSize();
            _recvinit[i] = 0;
        }

        _send.type   = PAMI::M2M_SINGLE;
        _send.buffer = &_sendpwq;
        _sendpwq.configure(
            (char*)_sbuf,
            topo_size,
            (pami_type_t*)&stype,
            scount * stype->GetDataSize(),
            scount,
            _sendinit);

        _send.participants = all;

        TRACE_ADAPTOR((stderr, "<%p>All2AllProtocol rcvbuf %p,topo_size %zu,_recvinit %p/%zu\n", this,coll->cmd.xfer_alltoall.rcvbuf,topo_size,_recvinit,_recvinit[0]));

        _recv.type   = PAMI::M2M_SINGLE;
        _recv.buffer = &_recvpwq;
        _recvpwq.configure(
            coll->cmd.xfer_alltoall.rcvbuf,
            topo_size,
            &coll->cmd.xfer_alltoall.rtype,
            coll->cmd.xfer_alltoall.rtypecount * rtype->GetDataSize(),/// \todo only supporting PAMI_TYPE_BYTE so offset=length
            coll->cmd.xfer_alltoall.rtypecount,
            _recvinit);

        _recv.participants = all;
        TRACE_ADAPTOR((stderr, "<%p>All2AllProtocol _recv %p, _recv.buffer %p, _recv.participants %p\n", this,&_recv, _recv.buffer, _recv.participants));

        _m2m_info.send = _send;

        // only comm is used in the header
        //_metadata._root  = -1U;
        //_metadata._comm  = _geometry->comm();
        //_metadata._count = -1U;
        //_metadata._phase = 0;
        //_metadata._iteration  = 0;
        //_metadata._op    = 0;
        //_metadata._dt    = 0;

        _m2m_info.msginfo = NULL; //(pami_quad_t*) & _metadata;
        _m2m_info.msgcount = 0;   //1;

        _m2m_info.roles = -1U;

        _m2m_info.client = 0; /// \todo does NOT support multiclient
        _m2m_info.context = 0; /// \todo does NOT support multicontext

        unsigned comm = _geometry->comm();
        _m2m_info.connection_id =  comm; //cmgr->getConnectionId_impl(comm, -1, 0, 0, -1);
        TRACE_ADAPTOR((stderr, "<%p>All2AllvProtocol::All2AllvProtocol() connection_id %u\n", this, _m2m_info.connection_id));
        //cmgr->updateConnectionId(comm);

        _m2m_info.cb_done.function   = a2aDone;
        _m2m_info.cb_done.clientdata = this;

    }

    virtual void start()
    {
        TRACE_ADAPTOR((stderr, "<%p>All2AllProtocol::start()\n", this));

        // Start the barrier. When it completes, it will start the m2m
        CCMI::Executor::Composite *barrier = (CCMI::Executor::Composite *)
          _geometry->getKey(_native->contextid(),
                            PAMI::Geometry::CKEY_OPTIMIZEDBARRIERCOMPOSITE);
        CCMI_assert(barrier != NULL);
        barrier->setDoneCallback (cb_barrier_done, this);
        //barrier->setConsistency (consistency);
        barrier->start();

    }
    void startA2A()
    {
        TRACE_ADAPTOR((stderr, "<%p>All2AllProtocol::startA2A()\n", this));
        _native->manytomany(&_m2m_info);
    }
    static void cb_barrier_done(pami_context_t context,
                                void *arg,
                                pami_result_t err)
    {
        (void)context;(void)err;
        TRACE_ADAPTOR((stderr, "<%p>All2AllProtocol::cb_barrier_done\n", arg));
        All2AllProtocol *a2a = (All2AllProtocol*) arg;
        CCMI_assert(a2a != NULL);

        // Barrier is done, start the active message a2a
        a2a->startA2A();
    }


    void notifyRecv(pami_manytomanybuf_t **recv,
                    pami_callback_t *cb_done)
    {
        TRACE_ADAPTOR((stderr, "<%p>All2AllProtocol::notifyRecv() recv %p\n", this, recv));
        *recv = & _recv;
        *cb_done = _my_cb_done;
    }

    static void a2aDone(pami_context_t context,
                        void *arg,
                        pami_result_t err)
    {
        TRACE_ADAPTOR((stderr, "<%p>All2AllProtocol::a2aDone()\n", arg));
        All2AllProtocol *a2a = (All2AllProtocol *) arg;
        CCMI_assert(a2a != NULL);

        a2a->done(context, err);
    }

    void done(pami_context_t context,
              pami_result_t err)
    {
        TRACE_ADAPTOR((stderr, "<%p>All2AllProtocol::done() count %u\n", this, _donecount));
        _donecount++;

        if ((_donecount == 2) && (_app_cb_done.function))
        {
            _app_cb_done.function (context,
                                   _app_cb_done.clientdata,
                                   err);
            /// \todo allocator?  reuse from factory?
            __global.heap_mm->free(_initbuf);
            if(_in_place) __global.heap_mm->free(_sbuf);
        }
    }

};


template <class T_Composite, MetaDataFn get_metadata, class T_Conn>
class All2AllFactoryT: public CollectiveProtocolFactory
{
protected:
    T_Conn *_cmgr;
    pami_dispatch_manytomany_function _fn;
    Interfaces::NativeInterface       *_native;
    CCMI::Adaptor::CollOpPoolT<pami_xfer_t, T_Composite> _free_pool;
public:
    All2AllFactoryT(pami_context_t               ctxt,
                    size_t                       ctxt_id,
                    pami_mapidtogeometry_fn      cb_geometry,
                    T_Conn                      *cmgr,
                    Interfaces::NativeInterface *native):
        CollectiveProtocolFactory(ctxt,ctxt_id,cb_geometry),
        _cmgr(cmgr),
        _native(native)
    {
        TRACE_ADAPTOR((stderr, "<%p>All2AllFactoryT\n", this));
        _fn = cb_manytomany;
        _native->setManytomanyDispatch(_fn, this);
    }

    virtual ~All2AllFactoryT()
    {
        TRACE_ADAPTOR((stderr, "<%p>~All2AllFactoryT\n", this));
    }


    void operator delete(void * p)
    {
        CCMI_abort();
    }

    unsigned getKey(PAMI_GEOMETRY_CLASS *g, T_Conn **cmgr)
    {
        return g->comm();
    }

    virtual void metadata(pami_metadata_t *mdata, pami_geometry_t geometry = PAMI_GEOMETRY_NULL)
    {
        get_metadata(mdata);
        if(_native) _native->metadata(mdata,PAMI_XFER_ALLTOALL);
    }

    virtual Executor::Composite * generate(pami_geometry_t g,
                                           void *op)
    {
        TRACE_ADAPTOR((stderr, "<%p>All2AllFactoryT::generate()\n", this));
        T_Composite *a2a = NULL;
        pami_callback_t cb_exec_done;
        PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *) g;
        CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *coll_object = NULL;

        unsigned key = getKey((PAMI_GEOMETRY_CLASS*) g, &_cmgr);
        coll_object = _free_pool.allocate(key);

        cb_exec_done.function = exec_done;
        cb_exec_done.clientdata = (void *) coll_object;

        a2a = new (coll_object->getComposite())
        T_Composite(_native,
                    _cmgr,
                    (PAMI_GEOMETRY_CLASS *) g,
                    (pami_xfer_t *)op,
                    cb_exec_done);

        coll_object->setXfer((pami_xfer_t *)op);
        coll_object->setFlag(LocalPosted);
        coll_object->setFactory(this);
        geometry->asyncCollectivePostQ(_native->contextid()).pushTail(coll_object);
        TRACE_ADAPTOR((stderr, "<%p>All2AllFactoryT::generate() key %u, coll_object %p, a2a %p\n", this, key, coll_object, a2a));
        return a2a;
    }

    static void cb_manytomany(pami_context_t ctxt,
                              void *arg,
                              unsigned conn_id,
                              pami_quad_t *msginfo,
                              unsigned msgcount,
                              pami_manytomanybuf_t **recv,
                              pami_callback_t *cb_done)
    {
        TRACE_ADAPTOR((stderr, "<%p>All2AllFactoryT::cb_manytomany() conn_id %u, msginfo %p, msgcount %u, recv %p\n", arg, conn_id, msginfo, msgcount, recv));
        All2AllFactoryT *factory = (All2AllFactoryT *) arg;
        //CollHeaderData *md = (CollHeaderData *) msginfo;
        //PAMI_assert(msgcount >= sizeof(CollHeaderData) / (sizeof(pami_quad_t)));
        All2AllProtocol *a2a = NULL;
        int comm = conn_id; //md->_comm;

        PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *) factory->getGeometry(ctxt, comm);

        T_Conn *cmgr = factory->_cmgr;
        unsigned key = factory->getKey(geometry, &cmgr);

        CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *coll_object =
            (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *)
          geometry->asyncCollectivePostQ(factory->_native->contextid()).findAndDelete(key);
        a2a = (T_Composite *) coll_object->getComposite();
        TRACE_ADAPTOR((stderr, "<%p>All2AllFactoryT::cb_manytomany() key %u, coll_object %p, a2a %p\n", arg, key, coll_object, a2a));
        a2a->notifyRecv(recv, cb_done);
    }


    static void exec_done(pami_context_t context,
                          void *coll_obj,
                          pami_result_t err)
    {
        TRACE_ADAPTOR((stderr, "<%p>All2AllFactoryT::exec_done()\n", coll_obj));
        CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *coll_object =
            (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *) coll_obj;
        unsigned flag = coll_object->getFlags();

        if (flag & LocalPosted)
        {
            pami_xfer_t *xfer = coll_object->getXfer();

            if (xfer->cb_done)
                xfer->cb_done(context, xfer->cookie, PAMI_SUCCESS);

            All2AllFactoryT *factory = (All2AllFactoryT *)
                                       coll_object->getFactory();
            factory->_free_pool.free(coll_object);
        }
    }
}; //- All2AllFactoryT


};
};
#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
