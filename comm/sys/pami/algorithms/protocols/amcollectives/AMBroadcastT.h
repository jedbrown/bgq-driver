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
 * \file algorithms/protocols/amcollectives/AMBroadcastT.h
 * \brief ???
 */

#ifndef __algorithms_protocols_amcollectives_AMBroadcastT_h__
#define __algorithms_protocols_amcollectives_AMBroadcastT_h__

#include "algorithms/ccmi.h"
#include "algorithms/executor/Broadcast.h"
#include "algorithms/connmgr/RankSeqConnMgr.h"
#include "algorithms/protocols/CollectiveProtocolFactory.h"
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
    namespace AMBroadcast
    {
///
/// \brief Asyc Broadcast Composite.
///
      template < class T_Broadcast_Schedule, class T_Conn,
      SFunc<PAMI_GEOMETRY_CLASS>::ScheduleFn create_broadcast_schedule >
      class AMBroadcastT : public CCMI::Executor::Composite
      {
        protected:
          CCMI::Executor::BroadcastExec<T_Conn, AMCollHeaderData>    _hdr_broadcast_executor __attribute__((__aligned__(16)));
          CCMI::Executor::BroadcastExec<T_Conn>                      _data_broadcast_executor __attribute__((__aligned__(16)));
          T_Broadcast_Schedule                     _hdr_broadcast_schedule;
          T_Broadcast_Schedule                     _data_broadcast_schedule;

        public:

          ///
          /// \brief Constructor
          ///
          AMBroadcastT ()
          {
          }

          AMBroadcastT (pami_context_t                             context,
                        size_t                                     ctxt_id,
                        Interfaces::NativeInterface              * native,
                        T_Conn                                   * cmgr,
                        pami_callback_t                            cb_hdr_broadcast_done,
                        pami_callback_t                            cb_data_broadcast_done,
                        PAMI_GEOMETRY_CLASS                      * geometry,
                        unsigned                                   root):
              Executor::Composite(),
              _hdr_broadcast_executor (native, cmgr, geometry->comm()),
              _data_broadcast_executor (native, cmgr, geometry->comm())
          {
            DEBUG((stderr, "<%p>AMBroadcast::AMBroadcastT()\n", this));
            _hdr_broadcast_executor.setRoot(root);
            _hdr_broadcast_executor.setDoneCallback (cb_hdr_broadcast_done.function, cb_hdr_broadcast_done.clientdata);
            COMPILE_TIME_ASSERT(sizeof(_hdr_broadcast_schedule) >= sizeof(T_Broadcast_Schedule));
            create_broadcast_schedule(&_hdr_broadcast_schedule, sizeof(_hdr_broadcast_schedule),
                                      root, native, geometry);
            _hdr_broadcast_executor.setSchedule (&_hdr_broadcast_schedule, 0);

            _data_broadcast_executor.setRoot(root);
            _data_broadcast_executor.setDoneCallback (cb_data_broadcast_done.function, cb_data_broadcast_done.clientdata);
            COMPILE_TIME_ASSERT(sizeof(_data_broadcast_schedule) >= sizeof(T_Broadcast_Schedule));
            create_broadcast_schedule(&_data_broadcast_schedule, sizeof(_data_broadcast_schedule),
                                      root, native, geometry);
            _data_broadcast_executor.setSchedule (&_data_broadcast_schedule, 0);
          }

          void start()
          {
            DEBUG((stderr, "<%p>AMBroadcastT::start()\n", this));
            _hdr_broadcast_executor.start();
          }

          CCMI::Executor::BroadcastExec<T_Conn, AMCollHeaderData> &headerExecutor()
          {
            return _hdr_broadcast_executor;
          }

          CCMI::Executor::BroadcastExec<T_Conn> &dataExecutor()
          {
            return _data_broadcast_executor;
          }
      };

      template <class T_Composite, MetaDataFn get_metadata, class T_Conn, ConnectionManager::GetKeyFn getKey, int T_hdr_size = 8192, int T_eab_size = 32768>
      class AMBroadcastFactoryT: public CollectiveProtocolFactory
      {
        protected:
          ///
          /// \brief free memory pool for async AMBroadcast operation objects
          ///
          CCMI::Adaptor::CollOpPoolT<pami_xfer_t,  T_Composite>   _free_pool;

          ///
          /// \brief memory allocator for storing the user header
          ///
          PAMI::MemoryAllocator<T_hdr_size , 16> _header_allocator;

          ///
          /// \brief memory allocator for early arrival buffers
          ///
          PAMI::MemoryAllocator<T_eab_size, 16> _eab_allocator;

          ///
          /// \brief memory allocator for early arrival descriptors
          ///
          PAMI::MemoryAllocator < sizeof(EADescriptor), 16 > _ead_allocator;

          T_Conn                         *_cmgr;

          Interfaces::NativeInterface    *_native;

        public:
          AMBroadcastFactoryT (pami_context_t               ctxt,
                               size_t                       ctxt_id,
                               pami_mapidtogeometry_fn      cb_geometry,
                               T_Conn                      *cmgr,
                               Interfaces::NativeInterface *native):
              CollectiveProtocolFactory(ctxt, ctxt_id, cb_geometry),
              _cmgr(cmgr), _native(native)
          {
            native->setMulticastDispatch(cb_head, this);
          }

          virtual ~AMBroadcastFactoryT ()
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
            if(_native) _native->metadata(mdata, PAMI_XFER_AMBROADCAST);
          }

          char *allocateHeader (unsigned size)
          {
            if (size <= T_hdr_size)
              return (char *)_header_allocator.allocateObject();

            char *buf;
            pami_result_t prc;
            prc = __global.heap_mm->memalign((void **) & buf, 0, size);
            PAMI_assertf(prc == PAMI_SUCCESS, "Failed to allocate %u async buffer\n", size);
            return prc == PAMI_SUCCESS ? buf : NULL;
          }

          void freeHeader (unsigned size, char *buf)
          {
            if (size <= T_hdr_size)
              return _header_allocator.returnObject(buf);

            __global.heap_mm->free(buf);
          }

          char *allocateBuffer (unsigned size)
          {
            if (size <= T_eab_size)
              return (char *)_eab_allocator.allocateObject();

            char *buf;
            pami_result_t prc;
            prc = __global.heap_mm->memalign((void **) & buf, 0, size);
            PAMI_assertf(prc == PAMI_SUCCESS, "Failed to allocate %u async buffer\n", size);
            return prc == PAMI_SUCCESS ? buf : NULL;
          }

          void freeBuffer (unsigned size, char *buf)
          {
            if (size <= T_eab_size)
              return _eab_allocator.returnObject(buf);

            __global.heap_mm->free(buf);
          }

          virtual Executor::Composite * generate(pami_geometry_t              g,
                                                 void                      * cmd)
          {
            T_Composite* composite = NULL;
            CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *co = NULL;
            pami_xfer_t *xfer = (pami_xfer_t *) cmd;
            pami_ambroadcast_t *amb_xfer = &(xfer->cmd.xfer_ambroadcast);

            PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *)g;
            T_Conn *cmgr = _cmgr;
            DEBUG((stderr, "AMBroadcastFactoryT::generate()\n"));
            unsigned key = getKey(this->_native->endpoint(),
                                  (unsigned) - 1,
                                  geometry,
                                  (ConnectionManager::BaseConnectionManager**) & cmgr);

            co = _free_pool.allocate(key);

            TypeCode *stype  = (TypeCode *) amb_xfer->stype;
            unsigned bytes   = amb_xfer->stypecount * stype->GetDataSize();

            pami_callback_t  cb_broadcast_header_done;
            cb_broadcast_header_done.function   = broadcast_header_done;
            cb_broadcast_header_done.clientdata = co;

            pami_callback_t  cb_broadcast_data_done;
            cb_broadcast_data_done.function   = broadcast_data_done;
            cb_broadcast_data_done.clientdata = co;

            composite = new (co->getComposite())
            T_Composite ( this->_context,
                          this->_context_id,
                          this->_native,
                          cmgr,
                          cb_broadcast_header_done,
                          cb_broadcast_data_done,
                          geometry,
                          this->_native->endpoint());

            co->setXfer((pami_xfer_t *)cmd);
            co->setFactory(this);
            co->setGeometry((PAMI_GEOMETRY_CLASS *)g);
            co->setFlag(LocalPosted);

            // Use a custom Collective header as we need to send the data size (for broadcast operation)
            AMCollHeaderData hdr;
            hdr._root  = this->_native->endpoint();
            hdr._comm  = geometry->comm();
            hdr._count = amb_xfer->headerlen; // Broadcast executor does not set count, so set manually
            hdr._phase = 0;
            hdr._data_size  = bytes;
            hdr._dispatch   = amb_xfer->dispatch;
#if !defined(__64BIT__)
            hdr.unused[0]=0;
            hdr.unused[1]=0;
#endif
            composite->setContext (this->_context);
            composite->headerExecutor().setCollHeader (hdr);
            composite->headerExecutor().setBuffers ((char *)amb_xfer->user_header,
                                                    (char *)amb_xfer->user_header,
                                                    amb_xfer->headerlen, amb_xfer->headerlen,
                                                    (TypeCode *) PAMI_TYPE_BYTE, (TypeCode *) PAMI_TYPE_BYTE);
            composite->dataExecutor().setBuffers ((char *)amb_xfer->sndbuf,
                                                  (char *)amb_xfer->sndbuf,
                                                  bytes, bytes, stype, stype);

            // Use the Key as the connection ID
            if (cmgr == NULL)
              {
                composite->headerExecutor().setConnectionID(key);
                composite->dataExecutor().setConnectionID(key);
              }

            geometry->asyncCollectivePostQ(_native->contextid()).pushTail(co);
            composite->headerExecutor().start();
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
            AMBroadcastFactoryT *factory = (AMBroadcastFactoryT *) arg;
            CollHeaderData *cdata = (CollHeaderData *) info;
            T_Composite* composite = NULL;

            PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *) factory->getGeometry (ctxt, cdata->_comm);
            T_Conn *cmgr = factory->_cmgr;

            unsigned key;
            key = getKey (cdata->_root, conn_id, geometry, (ConnectionManager::BaseConnectionManager **) & cmgr);
            CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *co =
              (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *) geometry->asyncCollectivePostQ(factory->_native->contextid()).find(key);

            DEBUG((stderr, "AMBroadcastFactoryT::cb_head() quad_count=%u sndlen=%zu cdata->_count=%u cdata->_root=%u peer=%zu conn_id=%u\n",
                   count, sndlen, cdata->_count, cdata->_root, peer, conn_id));

            if (!co)
              {
                co = factory->_free_pool.allocate(key);

                pami_callback_t  cb_broadcast_header_done;
                cb_broadcast_header_done.function   = broadcast_header_done;
                cb_broadcast_header_done.clientdata = co;

                pami_callback_t  cb_broadcast_data_done;
                cb_broadcast_data_done.function   = broadcast_data_done;
                cb_broadcast_data_done.clientdata = co;

                composite = new (co->getComposite())
                T_Composite ( ctxt,
                              factory->getContextId(),
                              factory->_native,
                              cmgr,
                              cb_broadcast_header_done,
                              cb_broadcast_data_done,
                              geometry,
                              cdata->_root);

                co->setFactory (factory);
                co->setGeometry(geometry);
                composite->setContext (ctxt);
                // Use the count to distinguish header packet from data packet for now
                if(count != 1)
                {
                  // Header message - Complete the broadcast operation (user header)
                  prepareHeaderExecutor(co, (AMCollHeaderData *)cdata, co->getXfer());
                  geometry->asyncCollectivePostQ(factory->_native->contextid()).pushTail(co);
                  composite->headerExecutor().start();
                  composite->headerExecutor().notifyRecv(peer, *info, (PAMI::PipeWorkQueue **)rcvpwq, cb_done);
                }
                else
                {
                  // Early arrival - data arrived before header
                  DEBUG((stderr, "AMBroadcastFactoryT::cb_async() : Early arrival - Data arrived before header. key = %u\n", co->key()));
                  prepareDataExecutor(co, cdata, sndlen);
                  geometry->asyncCollectivePostQ(factory->_native->contextid()).pushTail(co);
                  composite->dataExecutor().notifyRecv(peer, *info, (PAMI::PipeWorkQueue **)rcvpwq, cb_done);
                }
              }
            else
              {
                composite = (T_Composite *) co->getComposite();

                if(count != 1)
                {
                  // User header arrived after data
                  DEBUG((stderr, "AMBroadcastFactoryT::cb_async() : Late arrival - Header arrived after data. key = %u\n", co->key()));
                  prepareHeaderExecutor(co, (AMCollHeaderData *)cdata, co->getXfer());
                  composite->headerExecutor().start();
                  composite->headerExecutor().notifyRecv(peer, *info, (PAMI::PipeWorkQueue **)rcvpwq, cb_done);
                }
                else
                {
                  // Prepare the Data executor to receive payload
                  prepareDataExecutor(co, cdata, sndlen);
                  composite->dataExecutor().notifyRecv(peer, *info, (PAMI::PipeWorkQueue **)rcvpwq, cb_done);
                }
              }

            //We only support sndlen == rcvlen
            *rcvlen  = sndlen;
          }

          static inline void prepareHeaderExecutor(CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *co,
                                                   AMCollHeaderData *amcdata,
                                                   pami_xfer_t *a_xfer)
          {
            T_Composite *composite = co->getComposite();
            AMBroadcastFactoryT *factory = (AMBroadcastFactoryT *) co->getFactory();

            a_xfer->algorithm = (size_t) - 1;  ///Not used by the protocols
            // Allocate space to store the user header(s)
            if (amcdata->_count)
              a_xfer->cmd.xfer_ambroadcast.user_header = factory->allocateHeader(amcdata->_count);
            else
              a_xfer->cmd.xfer_ambroadcast.user_header = NULL;

            a_xfer->cmd.xfer_ambroadcast.headerlen   = amcdata->_count;
            a_xfer->cmd.xfer_ambroadcast.stype       = PAMI_TYPE_BYTE;
            a_xfer->cmd.xfer_ambroadcast.stypecount  = amcdata->_data_size;
            a_xfer->cmd.xfer_ambroadcast.sndbuf      = NULL;
            a_xfer->cmd.xfer_ambroadcast.dispatch    = amcdata->_dispatch;
#if !defined(__64BIT__)
            // Avoid compiler uninitialized warnings
            amcdata->unused[0]=0;
            amcdata->unused[1]=0;
#endif
            composite->headerExecutor().setCollHeader (*amcdata);
            composite->headerExecutor().setBuffers ((char *)a_xfer->cmd.xfer_ambroadcast.user_header,
                                                    (char *)a_xfer->cmd.xfer_ambroadcast.user_header,
                                                    a_xfer->cmd.xfer_ambroadcast.headerlen,
                                                    a_xfer->cmd.xfer_ambroadcast.headerlen,
                                                    (TypeCode *) PAMI_TYPE_BYTE,
                                                    (TypeCode *) PAMI_TYPE_BYTE);
            composite->headerExecutor().setConnectionID(co->key());
          }

          static inline void prepareDataExecutor(CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *co,
                                                 CollHeaderData                                   *cdata,
                                                 size_t                                            sndlen)
          {
            T_Composite* composite = (T_Composite *) co->getComposite();
            unsigned flag = co->getFlags();

            if(!(flag & LocalPosted))
            {
              DEBUG((stderr, "AMBroadcastFactoryT::prepareDataExecutor() : Early arrival. key = %u\n", co->key()));
              // Handle early arrival - receive buffer isn't available yet
              AMBroadcastFactoryT *factory = (AMBroadcastFactoryT *) co->getFactory();
              EADescriptor * ead = (EADescriptor *) factory->_ead_allocator.allocateObject();

              ead->flag  = EASTARTED;
              ead->bytes = sndlen;
              if (sndlen)
              {
                ead->buf   = (char *)factory->allocateBuffer(sndlen);
              }
              co->getEAQ()->pushTail(ead);
              co->setFlag(EarlyArrival);

              composite->dataExecutor().setBuffers (ead->buf, ead->buf, sndlen, sndlen,
                                                    (TypeCode *) PAMI_TYPE_BYTE,
                                                    (TypeCode *) PAMI_TYPE_BYTE);
            }
            composite->dataExecutor().setConnectionID(co->key());
          }

          static void broadcast_header_done (pami_context_t context, void *clientdata, pami_result_t err)
          {
            CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *co =
              (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *) clientdata;
            T_Composite* composite = (T_Composite *) co->getComposite();

            AMBroadcastFactoryT *factory = (AMBroadcastFactoryT *)co->getFactory();
            DEBUG((stderr, "AMBroadcastFactoryT::broadcast_header_done() : key = %u\n", co->key()));

            unsigned root = composite->headerExecutor().getRoot();
            // Invoke the user callback (root has already done this)
            if (factory->_native->endpoint() != root)
              {
                pami_xfer_t *a_xfer = co->getXfer();
                pami_recv_t recv = {0,};

                PAMI::Geometry::DispatchInfo *dispatch =
                  co->getGeometry()->getDispatch(factory->_context_id, a_xfer->cmd.xfer_ambroadcast.dispatch);
                PAMI_assertf(dispatch != NULL, "Invalid dispatch ID: %zu\n",
                             a_xfer->cmd.xfer_ambroadcast.dispatch);

                dispatch->fn.ambroadcast (
                 composite->getContext() ? composite->getContext() : factory->_context, // context
                 dispatch->cookie,                         // user cookie
                 a_xfer->cmd.xfer_ambroadcast.user_header, // User header
                 a_xfer->cmd.xfer_ambroadcast.headerlen,   // User header size
                 NULL,
                 a_xfer->cmd.xfer_ambroadcast.stypecount,  // Number of bytes of message data
                 root,                                     // origin (root)
                 co->getGeometry(),                        // Geometry
                 &recv);                                   // recv info

                // Set the completion callback and cookie passed in by the user
                a_xfer->cb_done = recv.local_fn;
                a_xfer->cookie  = recv.cookie;
                // We store the receive buffer as we need to memcpy the data to
                // this buffer in case  an Early arrival is currently in progress
                a_xfer->cmd.xfer_ambroadcast.sndbuf = (char *)recv.addr;

                if(a_xfer->cmd.xfer_ambroadcast.user_header)
                  factory->freeHeader(a_xfer->cmd.xfer_ambroadcast.headerlen,
                                      (char *)a_xfer->cmd.xfer_ambroadcast.user_header);

                unsigned flags = co->getFlags();
                if(flags & EarlyArrival)
                {
                  EADescriptor *ead = (EADescriptor *) co->getEAQ()->peekTail();
                  if(ead->flag == EACOMPLETED)
                  {
                    DEBUG((stderr, "AMBroadcastFactoryT::broadcast_header_done() : Early arrival completed. key = %u\n", co->key()));
                    if (a_xfer->cmd.xfer_ambroadcast.stypecount)
                    {
                        char *eab = ead->buf;
                        CCMI_assert(eab != NULL);
                        memcpy (recv.addr, eab, a_xfer->cmd.xfer_ambroadcast.stypecount);
                        factory->freeBuffer(ead->bytes, eab);
                    }

                    ead->flag = EANODATA;
                    co->getEAQ()->popTail();
                    factory->_ead_allocator.returnObject(ead);

                    if (a_xfer->cb_done)
                        a_xfer->cb_done(composite->getContext()?composite->getContext():factory->_context,
                                        a_xfer->cookie, PAMI_SUCCESS);

                    co->getGeometry()->asyncCollectivePostQ(factory->_native->contextid()).deleteElem(co);
                    factory->_free_pool.free(co);
                  }
                  else
                  {
                    DEBUG((stderr, "AMBroadcastFactoryT::broadcast_header_done() : Early arrival started. key = %u\n", co->key()));
                    co->setFlag(LocalPosted);
                  }
                }
                else
                {
                  // Set the user provided source buffer/type
                  composite->dataExecutor().setBuffers((char *)recv.addr,
                                                       (char *)recv.addr,
                                                       a_xfer->cmd.xfer_ambroadcast.stypecount,
                                                       a_xfer->cmd.xfer_ambroadcast.stypecount,
                                                       (TypeCode *) recv.type,
                                                       (TypeCode *) a_xfer->cmd.xfer_ambroadcast.stype);
                  co->setFlag(LocalPosted);
                }
              }
              else
              {
                  // Root - Start data broadcast
                  composite->dataExecutor().start();
              }
          }

          static void broadcast_data_done (pami_context_t context, void *clientdata, pami_result_t err)
          {
            CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> * co =
              (CCMI::Adaptor::CollOpT<pami_xfer_t, T_Composite> *)clientdata;
            T_Composite *composite = co->getComposite();

            DEBUG((stderr, "AMBroadcastFactoryT::broadcast_data_done() : key=%u\n", co->key()));

            unsigned flag = co->getFlags();
            if(flag & LocalPosted)
            {
              PAMI_GEOMETRY_CLASS *geometry = co->getGeometry();
              pami_xfer_t *xfer = co->getXfer();
              AMBroadcastFactoryT *factory = (AMBroadcastFactoryT *)co->getFactory();
              EADescriptor *ead = (EADescriptor *) co->getEAQ()->popTail();

              if (flag & EarlyArrival)
              {
                CCMI_assert(ead != NULL);

                if (xfer->cmd.xfer_ambroadcast.stypecount)
                {
                    char *eab  = ead->buf;
                    // sndbuf contains the locally posted receive buffer(on non-root tasks)
                    char *rbuf = (char *)xfer->cmd.xfer_ambroadcast.sndbuf;
                    CCMI_assert(eab != NULL);
                    memcpy (rbuf, eab, xfer->cmd.xfer_ambroadcast.stypecount);
                    factory->freeBuffer(ead->bytes, eab);
                }

                ead->flag = EANODATA;
                ead->buf  = NULL;
                factory->_ead_allocator.returnObject(ead);
              }
              else
              {
                CCMI_assert(ead == NULL);
              }

              // activate user callback
              if (xfer->cb_done)
                xfer->cb_done(composite->getContext() ? composite->getContext() : factory->_context,
                              xfer->cookie, PAMI_SUCCESS);

              // must be on the posted queue, dequeue it
              geometry->asyncCollectivePostQ(factory->_native->contextid()).deleteElem(co);

              // free the CollOp object
              factory->_free_pool.free(co);
            }
            else if (flag & EarlyArrival)
            {
              DEBUG((stderr, "AMBroadcastFactoryT::broadcast_data_done() : EACOMPLETED key=%u\n", co->key()));
              EADescriptor *ead = (EADescriptor *) co->getEAQ()->peekTail();
              ead->flag = EACOMPLETED;
            }
            else
            {
              CCMI_assert(0);
            }
          }
      }; //- AMBroadcastFactoryT
    };  //- end namespace AMBroadcast
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
