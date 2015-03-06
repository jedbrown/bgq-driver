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
 * \file algorithms/protocols/amcollectives/AMReduceT.h
 * \brief ???
 */

#ifndef __algorithms_protocols_amcollectives_AMReduceT_h__
#define __algorithms_protocols_amcollectives_AMReduceT_h__

#include "algorithms/ccmi.h"
#include "algorithms/executor/AllreduceBaseExec.h"
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
    namespace AMReduce
    {
///
/// \brief Async AMReduce Composite.
///
      template < class T_Broadcast_Schedule, class T_Reduce_Schedule, class T_Conn,
      SFunc<PAMI_GEOMETRY_CLASS>::ScheduleFn create_broadcast_schedule,
      SFunc<PAMI_GEOMETRY_CLASS>::ScheduleFn create_reduce_schedule >
      class AMReduceT : public CCMI::Executor::Composite
      {
        protected:
          T_Conn                                     _connmgr;
          CCMI::Executor::BroadcastExec<T_Conn, AMCollHeaderData>      _broadcast_executor __attribute__((__aligned__(16)));
          CCMI::Executor::AllreduceBaseExec<T_Conn>                    _reduce_executor __attribute__((__aligned__(16)));
          T_Broadcast_Schedule                       _broadcast_schedule;
          T_Reduce_Schedule                          _reduce_schedule;

        public:
          ///
          /// \brief Constructor
          ///
          AMReduceT ()
          {
          }

          AMReduceT (pami_context_t                         ctxt,
                     size_t                                 ctxt_id,
                     Interfaces::NativeInterface              * native,
                     pami_callback_t                            cb_done,
                     PAMI_GEOMETRY_CLASS                      * geometry,
                     unsigned                                   root,
                     unsigned                                   key = (unsigned)-1):
              Executor::Composite(),
              _connmgr(key),
              _broadcast_executor (native, &_connmgr, geometry->comm()),
              _reduce_executor (native, &_connmgr, geometry->comm(), (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX))
          {
            DEBUG((stderr, "<%p>AMReduce::AMReduceT()\n", this));
            _broadcast_executor.setRoot (root);
            _broadcast_executor.setDoneCallback (cb_done.function, cb_done.clientdata);
            COMPILE_TIME_ASSERT(sizeof(_broadcast_schedule) >= sizeof(T_Broadcast_Schedule));
            create_broadcast_schedule(&_broadcast_schedule, sizeof(_broadcast_schedule),
                                      root, native, geometry);
            _broadcast_executor.setSchedule (&_broadcast_schedule, 0);

            _reduce_executor.setRoot (root);
            COMPILE_TIME_ASSERT(sizeof(_reduce_schedule) >= sizeof(T_Reduce_Schedule));
            create_reduce_schedule(&_reduce_schedule, sizeof(_reduce_schedule),
                                   root, native, geometry);
            _reduce_executor.setSchedule (&_reduce_schedule);
          }

          void start()
          {
            DEBUG((stderr, "<%p>AMReduceT::start()\n", this));
            _broadcast_executor.start();
          }

          CCMI::Executor::BroadcastExec<T_Conn, AMCollHeaderData> &broadcastExecutor()
          {
            return _broadcast_executor;
          }

          CCMI::Executor::AllreduceBaseExec<T_Conn> &reduceExecutor()
          {
            return _reduce_executor;
          }

          T_Conn *connmgr()
          {
            return &_connmgr;
          }

      };

      template <class T_Composite, MetaDataFn get_metadata, class T_Conn, ConnectionManager::GetKeyFn getKey>
      class AMReduceFactoryT: public CollectiveProtocolFactory
      {
        protected:
          ///
          /// \brief free memory pool for async AMReduce operation objects
          ///
          CCMI::Adaptor::CollOpPoolT<pami_xfer_t,  T_Composite>   _free_pool;

          ///
          /// \brief memory allocator for storing the user header
          ///
          PAMI::MemoryAllocator<8192, 16> _header_allocator;

          T_Conn                         *_cmgr;

          Interfaces::NativeInterface    *_native;
        public:
          AMReduceFactoryT (pami_context_t               ctxt,
                            size_t                       ctxt_id,
                            pami_mapidtogeometry_fn      cb_geometry,
                            T_Conn                      *cmgr,
                            Interfaces::NativeInterface *native):
            CollectiveProtocolFactory(ctxt, ctxt_id, cb_geometry),
            _cmgr(cmgr),_native(native)
          {
            native->setMulticastDispatch(cb_head, this);
          }

          virtual ~AMReduceFactoryT ()
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
            if(_native) _native->metadata(mdata,PAMI_XFER_AMREDUCE);
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
            pami_amreduce_t *amr_xfer = &(xfer->cmd.xfer_amreduce);

            PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *)g;
            T_Conn *cmgr = _cmgr;
            DEBUG((stderr, "AMReduceFactoryT::generate()\n"));
            unsigned key = getKey(this->_native->endpoint(),
                                  (unsigned) - 1,
                                  geometry,
                                  (ConnectionManager::BaseConnectionManager**) & cmgr);

            co = _free_pool.allocate(key);
            pami_callback_t  cb_exec_done;
            cb_exec_done.function   = broadcast_exec_done;
            cb_exec_done.clientdata = co;

            PAMI::Geometry::DispatchInfo *dispatch = geometry->getDispatch(_context_id, amr_xfer->dispatch);
            PAMI_assertf(dispatch != NULL, "Invalid dispatch ID: %zu\n", amr_xfer->dispatch);

            pami_recv_t send = {0,};
            TypeCode *rtype  = (TypeCode *) amr_xfer->rtype;
            // rtype is always specified at the root, use that to compute message size
            unsigned bytes   = amr_xfer->rtypecount * rtype->GetDataSize();
            pami_data_function reduce_fn;
            // Invoke the dispatch function
            dispatch->fn.amreduce (
                 this->_context,              // context
                 dispatch->cookie,      // user cookie
                 amr_xfer->user_header, // User header
                 amr_xfer->headerlen,   // User header size
                 bytes,                 // Number of bytes of message data
                 this->_native->endpoint(),     // origin (root)
                 g,                     // Geometry
                 &reduce_fn,            // PAMI math operation to perform on the datatype
                 &send);                // recv info

            // Set the completion callback and cookie passed in by the user
            xfer->cb_done = send.local_fn;
            xfer->cookie  = send.cookie;

            a_composite = new (co->getComposite())
            T_Composite ( this->_context,
                          this->_context_id,
                          this->_native,
                          cb_exec_done,
                          geometry,
                          this->_native->endpoint(),
                          key);

            co->setXfer((pami_xfer_t *)cmd);
            co->setFactory(this);
            co->setGeometry((PAMI_GEOMETRY_CLASS *)g);

            uintptr_t op, dt;
            PAMI::Type::TypeFunc::GetEnums(send.type,
                                           reduce_fn,
                                           dt, op);
            // Use a custom Collective header as we need to send the data size (for reduce operation)
            AMCollHeaderData hdr;
            hdr._root  = this->_native->endpoint();
            hdr._comm  = geometry->comm();
            hdr._count = amr_xfer->headerlen; // Broadcast executor does not set count, so set manually
            hdr._phase = 0;
            hdr._iteration = 0;
            hdr._data_size  = bytes;
            hdr._dispatch   = amr_xfer->dispatch;
            hdr._dt    = (pami_dt) dt;
            hdr._op    = (pami_op) op;
#if !defined(__64BIT__)
            hdr.unused[0] = 0;
            hdr.unused[1] = 0;
#endif
            a_composite->setContext (this->_context);
            CCMI::Executor::BroadcastExec<T_Conn, AMCollHeaderData> &bexec = a_composite->broadcastExecutor();
            bexec.setCollHeader(hdr);
            bexec.setBuffers ((char *)amr_xfer->user_header,
                              (char *)amr_xfer->user_header,
                              amr_xfer->headerlen,
                              amr_xfer->headerlen,
                              (TypeCode *) PAMI_TYPE_BYTE,
                              (TypeCode *) PAMI_TYPE_BYTE);

            CCMI::Executor::AllreduceBaseExec<T_Conn> &rexec = a_composite->reduceExecutor();
            rexec.setBuffers((char *)send.addr,
                             (char *)amr_xfer->rcvbuf,
                             bytes,
                             bytes,
                             (TypeCode *) send.type,
                             rtype);

            unsigned sizeOfType;
            coremath func;

            CCMI::Adaptor::Allreduce::getReduceFunction((pami_dt)dt, (pami_op)op, sizeOfType, func);

            size_t stypecount = bytes/sizeOfType;
            rexec.setReduceInfo(stypecount, bytes, sizeOfType, func,
                                (TypeCode *)send.type, (TypeCode *)amr_xfer->rtype, (pami_op)op, (pami_dt)dt);
            rexec.setDoneCallback (reduce_exec_done, co);
            rexec.reset();

            if (cmgr == NULL)
              {
                bexec.setConnectionID(key);
              }

            geometry->asyncCollectivePostQ(_native->contextid()).pushTail(co);
            // Broadcast followed by Reduce
            bexec.start();
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
            AMReduceFactoryT *factory = (AMReduceFactoryT *) arg;
            ExtCollHeaderData *cdata = (ExtCollHeaderData *) info;
            T_Composite* composite = NULL;

            PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *) factory->getGeometry (ctxt, cdata->_comm);
            T_Conn *cmgr = factory->_cmgr;

            unsigned key;
            key = getKey (cdata->_root, conn_id, geometry, (ConnectionManager::BaseConnectionManager **) & cmgr);
            CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *co =
              (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *) geometry->asyncCollectivePostQ(factory->_native->contextid()).find(key);

            DEBUG((stderr, "AMReduceFactoryT::cb_head() quad_count=%u sndlen=%zu cdata->_count=%u cdata->_root=%u conn_id=%u\n",
                   count, sndlen, cdata->_count, cdata->_root, conn_id));

            if (!co)
              {
                DEBUG((stderr, "AMReduceFactoryT::cb_head() collop not found. key=%u\n",key));
                // Allocate the collective op
                co = factory->_free_pool.allocate(key);

                pami_callback_t cb_exec_done;
                cb_exec_done.function   = broadcast_exec_done;
                cb_exec_done.clientdata = co;

                composite = new (co->getComposite())
                T_Composite ( ctxt,
                              factory->getContextId(),
                              factory->_native,
                              cb_exec_done,
                              geometry,
                              cdata->_root,
                              key);

                co->setFactory (factory);
                co->setGeometry(geometry);
                composite->setContext (ctxt);

                if(count != 1)
                {
                  // We received a Broadcast header. Reduce header may arrive before Broadcast
                  // Complete the broadcast operation (user header) & prepare for Reduce
                  AMCollHeaderData *amcdata = (AMCollHeaderData *) cdata;
                  prepareBroadcastExecutor(co, amcdata, co->getXfer());
                  prepareReduceExecutor(co, amcdata, amcdata->_data_size, true);

                  // Override the connection ID
                  if (cmgr == NULL)
                  {
                    composite->broadcastExecutor().setConnectionID(key);
                  }
                  geometry->asyncCollectivePostQ(factory->_native->contextid()).pushTail(co);
                  composite->broadcastExecutor().start();
                  composite->broadcastExecutor().notifyRecv(peer, *info, (PAMI::PipeWorkQueue **)rcvpwq, cb_done);
                }
                else
                {
                  // We received an early Reduce header
                  prepareReduceExecutor(co, cdata, cdata->_count, false);
                  geometry->asyncCollectivePostQ(factory->_native->contextid()).pushTail(co);
                  composite->reduceExecutor().notifyRecvHead(info, count, conn_id,
                                                             peer, sndlen, arg, rcvlen,
                                                             rcvpwq, cb_done);
                  return;
                }
              }
            else
              {
                DEBUG((stderr, "AMReduceFactoryT::cb_head() collop found. key=%u\n",key));
                composite = (T_Composite *) co->getComposite();

                if(count != 1)
                {
                  // We received a Broadcast header. Reduce header has arrived before Broadcast
                  AMCollHeaderData *amcdata = (AMCollHeaderData *) cdata;
                  prepareBroadcastExecutor(co, amcdata, co->getXfer());

                  // Override the connection ID
                  if (cmgr == NULL)
                  {
                    composite->broadcastExecutor().setConnectionID(key);
                  }
                  composite->broadcastExecutor().start();
                  composite->broadcastExecutor().notifyRecv(peer, *info, (PAMI::PipeWorkQueue **)rcvpwq, cb_done);
                }
                else
                {
                  composite->reduceExecutor().notifyRecvHead(info, count, conn_id,
                                                             peer, sndlen, arg, rcvlen,
                                                             rcvpwq, cb_done);
                  return;
                }
              }

            //We only support sndlen == rcvlen
            *rcvlen  = sndlen;
          }

          static inline void prepareBroadcastExecutor(CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *co,
                                                      AMCollHeaderData *amcdata,
                                                      pami_xfer_t *a_xfer)
          {
            DEBUG((stderr, "AMReduceFactoryT::prepareBroadcastExecutor(): cdata->_data_size=%zu cdata->_dispatch=%zu\n",
                   amcdata->_data_size, amcdata->_dispatch));

            T_Composite *composite = co->getComposite();
            AMReduceFactoryT *factory = (AMReduceFactoryT *) co->getFactory();

            a_xfer->algorithm = (size_t) - 1;  // Not used by the protocols
            // Allocate space to store the user header
            if (amcdata->_count)
              a_xfer->cmd.xfer_amreduce.user_header = factory->allocateBuffer(amcdata->_count);
            else
              a_xfer->cmd.xfer_amreduce.user_header = NULL;

            a_xfer->cmd.xfer_amreduce.headerlen   = amcdata->_count;
            a_xfer->cmd.xfer_amreduce.rtype       = PAMI_TYPE_BYTE;
            a_xfer->cmd.xfer_amreduce.rtypecount  = amcdata->_data_size;
            a_xfer->cmd.xfer_amreduce.rcvbuf      = NULL;
            a_xfer->cmd.xfer_amreduce.dispatch    = amcdata->_dispatch;

            CCMI::Executor::BroadcastExec<T_Conn, AMCollHeaderData> &bexec = composite->broadcastExecutor();
            bexec.setCollHeader(*amcdata);

            bexec.setBuffers ((char *)a_xfer->cmd.xfer_amreduce.user_header,
                              (char *)a_xfer->cmd.xfer_amreduce.user_header,
                              a_xfer->cmd.xfer_amreduce.headerlen,
                              a_xfer->cmd.xfer_amreduce.headerlen,
                              (TypeCode *) PAMI_TYPE_BYTE,
                              (TypeCode *) PAMI_TYPE_BYTE);
          }

          static inline void prepareReduceExecutor(CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *co,
                                                   ExtCollHeaderData *cdata,
                                                   size_t bytes,
                                                   bool   is_broadcast)
          {
            DEBUG((stderr, "AMReduceFactoryT::prepareReduceExecutor(): is_broadcast=%d bytes=%zu\n",
                   (int)is_broadcast, bytes));

            T_Composite *composite = co->getComposite();
            unsigned sizeOfType;
            coremath func;
            CCMI::Adaptor::Allreduce::getReduceFunction((pami_dt)cdata->_dt, (pami_op)cdata->_op, sizeOfType, func);

            // We do not have the send buffer and type information at this point
            CCMI::Executor::AllreduceBaseExec<T_Conn> &rexec = composite->reduceExecutor();

            size_t dt_count;
            if(is_broadcast)
            {
              // We send the actual message size in the Broadcast header
              dt_count = bytes / sizeOfType;
            }
            else
            {
              // For Reduce headers, the header has the send type count
              dt_count = bytes;
              bytes = dt_count * sizeOfType;
            }
            rexec.setBuffers((char *)NULL,
                             (char *)NULL,
                             bytes,
                             bytes,
                             (TypeCode *) PAMI_TYPE_BYTE,
                             (TypeCode *) PAMI_TYPE_BYTE);

            rexec.setReduceInfo(dt_count, bytes, sizeOfType,
                                func, (TypeCode *)PAMI_TYPE_BYTE, (TypeCode *)PAMI_TYPE_BYTE,
                                (pami_op)cdata->_op, (pami_dt)cdata->_dt);

            rexec.setDoneCallback (reduce_exec_done, co);
            rexec.reset();
          }

          static void broadcast_exec_done (pami_context_t context, void *clientdata, pami_result_t err)
          {
            CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *co =
              (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *) clientdata;
            T_Composite* a_composite = (T_Composite *) co->getComposite();

            AMReduceFactoryT *factory = (AMReduceFactoryT *)co->getFactory();
            DEBUG((stderr, "AMReduceFactoryT::broadcast_exec_done() : key = %d\n", co->key()));

            CCMI::Executor::AllreduceBaseExec<T_Conn> &rexec = a_composite->reduceExecutor();
            unsigned root = rexec.getRoot();
            // Invoke the user callback (root has already done this)
            if (factory->_native->endpoint() != root)
              {
                pami_data_function reduce_fn;
                pami_xfer_t *a_xfer = co->getXfer();
                pami_recv_t send = {0,};

                PAMI::Geometry::DispatchInfo *dispatch =
                  co->getGeometry()->getDispatch(factory->_context_id, a_xfer->cmd.xfer_amreduce.dispatch);
                PAMI_assertf(dispatch != NULL, "Invalid dispatch ID: %zu\n", a_xfer->cmd.xfer_amreduce.dispatch);

                dispatch->fn.amreduce (
                 a_composite->getContext() ? a_composite->getContext() : factory->_context, // context
                 dispatch->cookie,                      // user cookie
                 a_xfer->cmd.xfer_amreduce.user_header, // User header
                 a_xfer->cmd.xfer_amreduce.headerlen,   // User header size
                 a_xfer->cmd.xfer_amreduce.rtypecount,  // Number of bytes of message data
                 root,                                  // origin (root)
                 co->getGeometry(),                     // Geometry
                 &reduce_fn,                            // PAMI math operation to perform on the datatype
                 &send);                                // recv info

                // Set the completion callback and cookie passed in by the user
                a_xfer->cb_done = send.local_fn;
                a_xfer->cookie  = send.cookie;
                if(a_xfer->cmd.xfer_amreduce.user_header)
                  factory->freeBuffer(a_xfer->cmd.xfer_amreduce.headerlen,
                                    (char *)a_xfer->cmd.xfer_amreduce.user_header);
                // Set the user provided source buffer/type & start the Reduce operation
                rexec.setBuffers((char *)send.addr,
                                 (char *)NULL,
                                 a_xfer->cmd.xfer_amreduce.rtypecount,
                                 a_xfer->cmd.xfer_amreduce.rtypecount,
                                 (TypeCode *) send.type,
                                 (TypeCode *) a_xfer->cmd.xfer_amreduce.rtype);

                uintptr_t op, dt;
                PAMI::Type::TypeFunc::GetEnums(send.type,
                                               reduce_fn,
                                               dt, op);
                unsigned sizeOfType;
                coremath func;

                CCMI::Adaptor::Allreduce::getReduceFunction((pami_dt)dt,
                                                            (pami_op)op,
                                                            sizeOfType, func);
                // Always use size of the send data type
                sizeOfType = ((TypeCode *)send.type)->GetDataSize();

                size_t stypecount = a_xfer->cmd.xfer_amreduce.rtypecount/sizeOfType;
                rexec.setReduceInfo(stypecount,
                                    a_xfer->cmd.xfer_amreduce.rtypecount,
                                    sizeOfType, func,
                                    (TypeCode *)send.type, (TypeCode *)send.type,
                                    (pami_op)op, (pami_dt)dt);
                rexec.reset();
              }
              rexec.start();
          }

          static void reduce_exec_done (pami_context_t context, void *clientdata, pami_result_t err)
          {
            CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> * co =
              (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *)clientdata;
            DEBUG((stderr, "AMReduceFactoryT::reduce_exec_done() : key=%u\n", co->key()));

            PAMI_GEOMETRY_CLASS *geometry = co->getGeometry();
            pami_xfer_t *xfer = co->getXfer();
            AMReduceFactoryT *factory = (AMReduceFactoryT *)co->getFactory();

            // activate user callback
            if (xfer->cb_done)
              xfer->cb_done(co->getComposite()->getContext() ? co->getComposite()->getContext() : factory->_context,
                            xfer->cookie, PAMI_SUCCESS);

            // must be on the posted queue, dequeue it
            geometry->asyncCollectivePostQ(factory->_native->contextid()).deleteElem(co);

            // free the CollOp object
            factory->_free_pool.free(co);
          }
      }; //- AMReduceFactoryT
    };  //- end namespace AMReduce
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
