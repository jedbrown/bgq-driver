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
 * \file algorithms/protocols/amcollectives/AMGatherT.h
 * \brief ???
 */

#ifndef __algorithms_protocols_amcollectives_AMGatherT_h__
#define __algorithms_protocols_amcollectives_AMGatherT_h__

#include "algorithms/ccmi.h"
#include "algorithms/executor/Gather.h"
#include "algorithms/connmgr/CommSeqConnMgr.h"
#include "algorithms/protocols/CollectiveProtocolFactoryT.h"
#include "algorithms/protocols/CollOpT.h"

#if defined DEBUG
#undef DEBUG
#define DEBUG(x)  //fprintf x
#else
#define DEBUG(x)  //fprintf x
#endif

namespace CCMI
{
  namespace Adaptor
  {
    namespace AMGather
    {
///
/// \brief Asyc Gather Composite.
///
      template < class T_Scatter_Schedule, class T_Gather_Schedule, class T_Conn,
      SFunc<PAMI_GEOMETRY_CLASS>::ScheduleFn create_scatter_schedule,
      SFunc<PAMI_GEOMETRY_CLASS>::ScheduleFn create_gather_schedule >
      class AMGatherT : public CCMI::Executor::Composite
      {
        protected:
          CCMI::Executor::ScatterExec<T_Conn, T_Scatter_Schedule, pami_scatter_t, AMCollHeaderData>    _scatter_executor __attribute__((__aligned__(16)));
          CCMI::Executor::GatherExec<T_Conn, T_Gather_Schedule, pami_gather_t>                         _gather_executor __attribute__((__aligned__(16)));
          T_Scatter_Schedule                     _scatter_schedule;
          T_Gather_Schedule                      _gather_schedule;

        public:
          size_t   _sndlen; // Size of buffer allocated for headers

          ///
          /// \brief Constructor
          ///
          AMGatherT ()
          {
          }

          AMGatherT (pami_context_t                         ctxt,
                     size_t                                 ctxt_id,
                     Interfaces::NativeInterface          * native,
                     T_Conn                               * cmgr,
                     pami_callback_t                        cb_scatter_done,
                     pami_callback_t                        cb_gather_done,
                     PAMI_GEOMETRY_CLASS                  * geometry,
                     unsigned                               root):
              Executor::Composite(),
              _scatter_executor (native, cmgr, geometry->comm(), (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX)),
              _gather_executor (native, cmgr, geometry->comm(), (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX)),
              _sndlen(0)
          {
            DEBUG((stderr, "<%p>AMGather::AMGatherT()\n", this));
            _scatter_executor.setRoot(root);
            _scatter_executor.setDoneCallback (cb_scatter_done.function, cb_scatter_done.clientdata);
            COMPILE_TIME_ASSERT(sizeof(_scatter_schedule) >= sizeof(T_Scatter_Schedule));
            create_scatter_schedule(&_scatter_schedule, sizeof(_scatter_schedule),
                                    root, native, geometry);
            _scatter_executor.setSchedule (&_scatter_schedule);

            _gather_executor.setRoot(root);
            _gather_executor.setDoneCallback (cb_gather_done.function, cb_gather_done.clientdata);
            COMPILE_TIME_ASSERT(sizeof(_gather_schedule) >= sizeof(T_Gather_Schedule));
            create_gather_schedule(&_gather_schedule, sizeof(_gather_schedule),
                                   root, native, geometry);
            _gather_executor.setSchedule (&_gather_schedule);
          }

          void start()
          {
            DEBUG((stderr, "<%p>AMGatherT::start()\n", this));
            _scatter_executor.start();
          }

          CCMI::Executor::ScatterExec<T_Conn, T_Scatter_Schedule, pami_scatter_t, AMCollHeaderData> &scatterExecutor()
          {
            return _scatter_executor;
          }

          CCMI::Executor::GatherExec<T_Conn, T_Gather_Schedule, pami_gather_t> &gatherExecutor()
          {
            return _gather_executor;
          }

      };

      template <class T_Composite, MetaDataFn get_metadata, class T_Conn, ConnectionManager::GetKeyFn getKey>
      class AMGatherFactoryT: public CollectiveProtocolFactory
      {
        protected:
          ///
          /// \brief free memory pool for async AMGather operation objects
          ///
          CCMI::Adaptor::CollOpPoolT<pami_xfer_t,  T_Composite>   _free_pool;

          ///
          /// \brief memory allocator for storing the user header
          ///
          PAMI::MemoryAllocator<8192, 16> _header_allocator;

          T_Conn                         *_cmgr;
          Interfaces::NativeInterface    *_native;
        public:
          AMGatherFactoryT (pami_context_t               ctxt,
                            size_t                       ctxt_id,
                            pami_mapidtogeometry_fn      cb_geometry,
                            T_Conn                      *cmgr,
                            Interfaces::NativeInterface *native):
            CollectiveProtocolFactory(ctxt, ctxt_id, cb_geometry),
            _cmgr(cmgr), _native(native)
          {
            native->setMulticastDispatch(cb_head, this);
          }

          virtual ~AMGatherFactoryT ()
          {
          }

          /// NOTE: This is required to make "C" programs link successfully with virtual destructors
          void operator delete(void * p)
          {
            CCMI_abort();
          }

          virtual void metadata(pami_metadata_t *mdata, pami_geometry_t geometry = PAMI_GEOMETRY_NULL)
          {
            DO_DEBUG((templateName<MetaDataFn>()));
            get_metadata(mdata);
            if(_native) _native->metadata(mdata,PAMI_XFER_AMGATHER);
          }

          char *allocateBuffer (unsigned size)
          {
            if (size <= 8192)
              return (char *)_header_allocator.allocateObject();

            char *buf;
            pami_result_t prc;
            prc = __global.heap_mm->memalign((void **) & buf, 0, size);
            PAMI_assertf(prc == PAMI_SUCCESS, "Failed to allocate %u async buffer\n", size);
            return prc == PAMI_SUCCESS ? buf : NULL;
          }

          void freeBuffer (unsigned size, char *buf)
          {
            if (size <= 8192)
              return _header_allocator.returnObject(buf);

            __global.heap_mm->free(buf);
          }

          virtual Executor::Composite * generate(pami_geometry_t              g,
                                                 void                      * cmd)
          {
            T_Composite* a_composite = NULL;
            CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *co = NULL;
            pami_xfer_t *xfer = (pami_xfer_t *) cmd;
            pami_amgather_t *amg_xfer = &(xfer->cmd.xfer_amgather);

            PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *)g;
            PAMI::Topology *topo = (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);
            T_Conn *cmgr = _cmgr;
            DEBUG((stderr, "AMGatherFactoryT::generate()\n"));
            unsigned key = getKey(this->_native->endpoint(),
                                  (unsigned) - 1,
                                  geometry,
                                  (ConnectionManager::BaseConnectionManager**) & cmgr);

            co = _free_pool.allocate(key);

            PAMI::Geometry::DispatchInfo *dispatch = geometry->getDispatch(_context_id, amg_xfer->dispatch);
            PAMI_assertf(dispatch != NULL, "Invalid dispatch ID: %zu\n", amg_xfer->dispatch);

            pami_recv_t send = {0,};
            TypeCode *rtype  = (TypeCode *) amg_xfer->rtype;
            unsigned bytes   = amg_xfer->rtypecount * rtype->GetDataSize();
            // Invoke the dispatch function 
            dispatch->fn.amgather (
                 this->_context,           // context
                 dispatch->cookie,         // user cookie
                 (char *)amg_xfer->headers + topo->endpoint2Index(this->_native->endpoint()) * amg_xfer->headerlen, // User header
                 amg_xfer->headerlen,      // User header size
                 bytes,                    // Number of bytes of message data
                 this->_native->endpoint(),// origin (root)
                 g,                        // Geometry
                 &send);                   // recv info

            // Set the completion callback and cookie passed in by the user
            xfer->cb_done = send.local_fn;
            xfer->cookie  = send.cookie;

            pami_callback_t  cb_scatter_exec_done;
            cb_scatter_exec_done.function   = scatter_exec_done;
            cb_scatter_exec_done.clientdata = co;
            pami_callback_t  cb_gather_exec_done;
            cb_gather_exec_done.function   = gather_exec_done;
            cb_gather_exec_done.clientdata = co;

            a_composite = new (co->getComposite())
            T_Composite ( this->_context,
                          this->_context_id,
                          this->_native,
                          cmgr,
                          cb_scatter_exec_done,
                          cb_gather_exec_done,
                          geometry,
                          this->_native->endpoint());

            co->setXfer((pami_xfer_t *)cmd);
            co->setFactory(this);
            co->setGeometry((PAMI_GEOMETRY_CLASS *)g);

            // Use a custom Collective header as we need to send the data size (for gather operation)
            AMCollHeaderData hdr;
            hdr._root  = this->_native->endpoint();
            hdr._comm  = geometry->comm();
            hdr._count = -1;
            hdr._phase = 0;
            hdr._data_size  = bytes;
            hdr._dispatch   = amg_xfer->dispatch;
#if !defined(__64BIT__)
            hdr.unused[0]  = 0;
            hdr.unused[1]  = 0;
#endif

            a_composite->setContext (this->_context);
            a_composite->scatterExecutor().setCollHeader (hdr);
            a_composite->scatterExecutor().setBuffers ((char *)amg_xfer->headers,
                                                       (char *)amg_xfer->headers + topo->endpoint2Index(this->_native->endpoint()) * amg_xfer->headerlen, // src==dst in final memcpy
                                                       amg_xfer->headerlen, amg_xfer->headerlen,
                                                       (TypeCode *) PAMI_TYPE_BYTE, (TypeCode *) PAMI_TYPE_BYTE);
            a_composite->gatherExecutor().setBuffers ((char *)send.addr,
                                                      (char *)amg_xfer->rcvbuf,
                                                      amg_xfer->rtypecount,
                                                      (TypeCode *) send.type,
                                                      rtype);

            // Use the Key as the connection ID
            if (cmgr == NULL)
              {
                a_composite->scatterExecutor().setConnectionID(key);
                a_composite->gatherExecutor().setConnectionID(key);
              }

            geometry->asyncCollectivePostQ(_native->contextid()).pushTail(co);
            a_composite->scatterExecutor().start();
            a_composite->gatherExecutor().start();
            return NULL;
          }

          static void   cb_head
          (pami_context_t         ctxt,
           const pami_quad_t    * info,
           unsigned               count,
           unsigned               conn_id,
           size_t                 peer,
           size_t                 sndlen,
           void                 * arg,
           size_t               * rcvlen,
           pami_pipeworkqueue_t ** rcvpwq,
           pami_callback_t       * cb_done)
          {
            AMGatherFactoryT *factory = (AMGatherFactoryT *) arg;
            CollHeaderData *cdata = (CollHeaderData *) info;
            T_Composite* a_composite = NULL;

            PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *) factory->getGeometry (ctxt, cdata->_comm);
            T_Conn *cmgr = factory->_cmgr;

            unsigned key;
            key = getKey (cdata->_root, conn_id, geometry, (ConnectionManager::BaseConnectionManager **) & cmgr);
            CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *co =
              (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *) geometry->asyncCollectivePostQ(factory->_native->contextid()).find(key);

            DEBUG((stderr, "AMGatherFactoryT::cb_head() quad_count=%u sndlen=%zu cdata->_count=%u cdata->_root=%u conn_id=%u\n",
                   count, sndlen, cdata->_count, cdata->_root, conn_id));

            if (!co)
              {
                // Phase 1 - Complete the scatter operation (user header)
                AMCollHeaderData *amcdata = (AMCollHeaderData *) cdata;

                DEBUG((stderr, "AMGatherFactoryT::cb_head() : Phase 1, key=%u _data_size=%zu _dispatch=%zu\n",
                       key, amcdata->_data_size, amcdata->_dispatch));
                co = factory->_free_pool.allocate(key);

                pami_xfer_t a_xfer;
                a_xfer.algorithm = (size_t) - 1;  ///Not used by the protocols
                // Allocate space to store the user header(s)
                if (sndlen)
                  a_xfer.cmd.xfer_amgather.headers = factory->allocateBuffer(sndlen);
                else
                  a_xfer.cmd.xfer_amgather.headers = NULL;

                a_xfer.cmd.xfer_amgather.headerlen   = amcdata->_count;
                a_xfer.cmd.xfer_amgather.rtype       = PAMI_TYPE_BYTE;
                a_xfer.cmd.xfer_amgather.rtypecount  = amcdata->_data_size;
                a_xfer.cmd.xfer_amgather.rcvbuf      = NULL;
                a_xfer.cmd.xfer_amgather.dispatch    = amcdata->_dispatch;

                pami_callback_t  cb_scatter_exec_done;
                cb_scatter_exec_done.function   = scatter_exec_done;
                cb_scatter_exec_done.clientdata = co;
                pami_callback_t  cb_gather_exec_done;
                cb_gather_exec_done.function   = gather_exec_done;
                cb_gather_exec_done.clientdata = co;

                a_composite = new (co->getComposite())
                T_Composite ( ctxt,
                              factory->getContextId(),
                              factory->_native,
                              cmgr,
                              cb_scatter_exec_done,
                              cb_gather_exec_done,
                              geometry,
                              cdata->_root);

                co->setFactory (factory);
                co->setGeometry(geometry);
                co->setXfer(&a_xfer);
                a_composite->_sndlen = sndlen;// Need this information to free/return buffer
                a_composite->setContext (ctxt);
                a_composite->scatterExecutor().setCollHeader (*amcdata);
                a_composite->scatterExecutor().setBuffers ((char *)NULL,
                                                           (char *)a_xfer.cmd.xfer_amgather.headers,
                                                           a_xfer.cmd.xfer_amgather.headerlen,
                                                           a_xfer.cmd.xfer_amgather.headerlen,
                                                           (TypeCode *) PAMI_TYPE_BYTE,
                                                           (TypeCode *) PAMI_TYPE_BYTE);

                // We do not have the send buffer and type information at this point
                a_composite->gatherExecutor().setBuffers((char *)NULL,
                                                         (char *)NULL,
                                                         a_xfer.cmd.xfer_amgather.rtypecount,
                                                         (TypeCode *) PAMI_TYPE_BYTE,
                                                         (TypeCode *) PAMI_TYPE_BYTE);
                if (cmgr == NULL)
                  {
                    a_composite->scatterExecutor().setConnectionID(key);
                    a_composite->gatherExecutor().setConnectionID(key);
                  }

                geometry->asyncCollectivePostQ(factory->_native->contextid()).pushTail(co);
                a_composite->start();
                a_composite->scatterExecutor().notifyRecv(peer, *info, (PAMI::PipeWorkQueue **)rcvpwq, cb_done);
              }
            else
              {
                // Phase 2 - Complete the Gather operation
                DEBUG((stderr, "AMGatherFactoryT::cb_head() : Phase 2, key=%u\n", key));
                CCMI_assert(count != 2);
                a_composite = (T_Composite *) co->getComposite();
                a_composite->gatherExecutor().notifyRecv(peer, *info, (PAMI::PipeWorkQueue **)rcvpwq, cb_done);
              }

            //We only support sndlen == rcvlen
            *rcvlen  = sndlen;
          }

          static void scatter_exec_done (pami_context_t context, void *clientdata, pami_result_t err)
          {
            CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *co =
              (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *) clientdata;
            T_Composite* a_composite = (T_Composite *) co->getComposite();

            AMGatherFactoryT *factory = (AMGatherFactoryT *)co->getFactory();
            DEBUG((stderr, "AMGatherFactoryT::scatter_exec_done() : key = %d\n", co->key()));

            unsigned root = a_composite->scatterExecutor().getRoot();
            // Invoke the user callback (root has already done this)
            if (factory->_native->endpoint() != root)
              {
                pami_xfer_t *a_xfer = co->getXfer();
                pami_recv_t send = {0,};

                PAMI::Geometry::DispatchInfo *dispatch =
                  co->getGeometry()->getDispatch(factory->_context_id, a_xfer->cmd.xfer_amgather.dispatch);
                PAMI_assertf(dispatch != NULL, "Invalid dispatch ID: %zu\n",
                             a_xfer->cmd.xfer_amgather.dispatch);
                TypeCode *rtype  = (TypeCode *) a_xfer->cmd.xfer_amgather.rtype;
                unsigned bytes   = a_xfer->cmd.xfer_amgather.rtypecount * rtype->GetDataSize();
                dispatch->fn.amgather (
                 a_composite->getContext() ? a_composite->getContext() : factory->_context, // context
                 dispatch->cookie,                      // user cookie
                 a_xfer->cmd.xfer_amgather.headers,     // User header
                 a_xfer->cmd.xfer_amgather.headerlen,   // User header size
                 bytes,                                 // Number of bytes of message data
                 root,                                  // origin (root)
                 co->getGeometry(),                     // Geometry
                 &send);                                // recv info

                // Set the completion callback and cookie passed in by the user
                a_xfer->cb_done = send.local_fn;
                a_xfer->cookie  = send.cookie;
                if(a_xfer->cmd.xfer_amgather.headers)
                  factory->freeBuffer(a_composite->_sndlen,
                                      (char *)a_xfer->cmd.xfer_amgather.headers);
                // Set the user provided source buffer/type & start the Gather operation
                a_composite->gatherExecutor().updateBuffers((char *)send.addr,
                                                            (char *)NULL,
                                                            a_xfer->cmd.xfer_amgather.rtypecount,
                                                            (TypeCode *) send.type,
                                                            (TypeCode *) a_xfer->cmd.xfer_amgather.rtype);
                a_composite->gatherExecutor().updatePWQ();
                a_composite->gatherExecutor().start();
              }
          }

          static void gather_exec_done (pami_context_t context, void *clientdata, pami_result_t err)
          {
            CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> * co =
              (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *)clientdata;
            DEBUG((stderr, "AMGatherFactoryT::gather_exec_done() : key=%u\n", co->key()));

            PAMI_GEOMETRY_CLASS *geometry = co->getGeometry();
            pami_xfer_t *xfer = co->getXfer();
            AMGatherFactoryT *factory = (AMGatherFactoryT *)co->getFactory();

            // activate user callback
            if (xfer->cb_done)
              xfer->cb_done(co->getComposite()->getContext() ? co->getComposite()->getContext() : factory->_context,
                            xfer->cookie, PAMI_SUCCESS);

            // must be on the posted queue, dequeue it
            geometry->asyncCollectivePostQ(factory->_native->contextid()).deleteElem(co);

            // free the CollOp object
            factory->_free_pool.free(co);
          }
      }; //- AMGatherFactoryT
    };  //- end namespace AMGather
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
