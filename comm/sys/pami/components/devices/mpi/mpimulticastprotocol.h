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
///
/// \file components/devices/mpi/mpimulticastprotocol.h
/// \brief Defines protocol classes for multicast
///
#ifndef __components_devices_mpi_mpimulticastprotocol_h__
#define __components_devices_mpi_mpimulticastprotocol_h__

#include <pami.h>
#include "components/memory/MemoryAllocator.h"
#include "PipeWorkQueue.h"
#include "Topology.h"
#include "Global.h"
#include "components/devices/MulticastModel.h"
//#include <list>
//#include "components/devices/mpi/mpicollectiveheader.h"
#include "components/devices/mpi/mpidevice.h"
extern PAMI::Device::MPIDevice _g_mpi_device;

#undef TRACE_DEVICE
#ifndef TRACE_DEVICE
  #define TRACE_DEVICE(x) //fprintf x
#endif

namespace PAMI
{
  namespace Protocol
  {
    namespace MPI
    {

      ///
      /// \brief 1-sided multicast protocol built on a p2p dispatch and all-sided multicast
      ///
      template <
        class T_P2P_DEVICE,
        class T_P2P_PROTOCOL,
        class T_MULTICAST_MODEL,
        class T_MULTICAST_DEVICE>
      class P2PMcastProto:
        public PAMI::Device::Interface::AMMulticastModel<P2PMcastProto<T_P2P_DEVICE,T_P2P_PROTOCOL, T_MULTICAST_MODEL,T_MULTICAST_DEVICE>, T_P2P_DEVICE, T_MULTICAST_MODEL::sizeof_msg+sizeof(PAMI::Topology)+sizeof(pami_callback_t)+sizeof(void*) >
      {
        ///
        /// Point-to-point dispatch header.
        ///
        typedef struct _dispatch_header_
        {
          pami_task_t root;        /// multicast root
          unsigned connection_id; /// multicast connection id
          size_t   bytes;         /// total bytes being multicast
        } dispatch_hdr_t;

        ///
        /// Allocation for multicast. Passed on cb_done client data so it can be processed and freed
        ///
        typedef struct _allocation_
        {
          uint8_t                  request[T_MULTICAST_MODEL::sizeof_msg]; /// request storage for the message
          PAMI::Topology            topology; ///  storage for src_participants
          pami_callback_t           cb_done;  ///  original user's cb_done
          P2PMcastProto<T_P2P_DEVICE,T_P2P_PROTOCOL, T_MULTICAST_MODEL,T_MULTICAST_DEVICE>
          *protocol;/// this protocol object - to retrieve allocator
        } allocation_t;
      public:
        static const int NUM_ROLES = 2;
        static const int REPL_ROLE = 1;
        static const size_t sizeof_msg = sizeof(allocation_t);

        ///
        /// \brief Base class constructor
        ///
        inline P2PMcastProto(pami_result_t             & status) :
          PAMI::Device::Interface::AMMulticastModel<P2PMcastProto<T_P2P_DEVICE,T_P2P_PROTOCOL, T_MULTICAST_MODEL,T_MULTICAST_DEVICE>,T_P2P_DEVICE,sizeof(allocation_t) >(status),
          _dst_participants(__global.mapping.task()), // default dst is this task when dispatched from another src
          _dispatch_id(0),
          _dispatch_fn(NULL),
//          _task_id(origin_task),
          _client(NULL),
          _context(NULL),
          _contextid(0),
          _clientid(0),
          _task_id((size_t)__global.mapping.task()),
          _cookie(NULL),
          _multicast_model(status)
          {
            TRACE_DEVICE((stderr,"<%p>P2PMcastProto(status)  allocator size %zu\n",this,_allocator.objsize));
          }
        inline P2PMcastProto(size_t                     dispatch_id,
                             pami_dispatch_multicast_function dispatch,
                             void                     * cookie,
                             T_P2P_DEVICE             & p2p_device,
                             T_MULTICAST_DEVICE       & mcast_dev,
                             //size_t                     origin_task,
                             pami_client_t               client,
                             pami_context_t              context,
                             size_t                     contextid,
                             size_t                     clientid,
                             pami_result_t             & status) :
          PAMI::Device::Interface::AMMulticastModel<P2PMcastProto<T_P2P_DEVICE,T_P2P_PROTOCOL, T_MULTICAST_MODEL,T_MULTICAST_DEVICE>,T_P2P_DEVICE,sizeof(allocation_t) >(p2p_device, status),
          _dst_participants(__global.mapping.task()), // default dst is this task when dispatched from another src
          _dispatch_id(dispatch_id),
          _dispatch_fn(dispatch),
//          _task_id(origin_task),
          _client(client),
          _context(context),
          _contextid(contextid),
          _clientid(clientid),
          _task_id((size_t)__global.mapping.task()),
          _cookie(cookie),
          _multicast_model(mcast_dev, status)
          {
            TRACE_DEVICE((stderr,"<%p>P2PMcastProto.  allocator size %zu\n",this,_allocator.objsize));
            // Construct a p2p protocol for dispatching
            pami_dispatch_callback_function fn;
            fn.p2p = dispatch_p2p;
            pami_endpoint_t origin = PAMI_ENDPOINT_INIT(_clientid, _task_id, _contextid);
            new (&_p2p_protocol) T_P2P_PROTOCOL(dispatch_id, fn.p2p, (void*)this,
                                                p2p_device, origin, context, status);
            TRACE_DEVICE((stderr,"<%p>P2PMcastProto status %d\n",this,status));
          }

        pami_result_t registerMcastRecvFunction_impl(int dispatch_id,
                                                    pami_dispatch_multicast_function dispatch,
                                                    void                         *cookie)
          {
            TRACE_DEVICE((stderr,"<%p>P2PMcastProto::register id %zu, fn %p, cookie %p\n",this,dispatch_id, dispatch, cookie));
            pami_result_t status = PAMI_SUCCESS;
            _dispatch_id=dispatch_id;
            _dispatch_fn=dispatch;
            _cookie=cookie;
            // Construct a p2p protocol for dispatching
            pami_dispatch_callback_function fn;
            fn.p2p = dispatch_p2p;
            new (&_p2p_protocol) T_P2P_PROTOCOL(_dispatch_id, fn, (void*)this,
                                                _g_mpi_device,
                                                status);
            PAMI_assertf(status == PAMI_SUCCESS,"<%p>P2PMcastProto::register status=%d\n",this,status);
            return status;
          }
        ///
        /// \brief Base class destructor.
        ///
        /// \note Any class with virtual functions must define a virtual
        ///       destructor.
        ///
//        virtual ~P2PMcastProto ()
//        {
//        };

        ///
        /// \brief Start a new multicast message.
        ///
        /// \param[in] mcast
        ///
        pami_result_t multicast(uint8_t (&state)[sizeof_msg],
                               pami_multicast_t *mcast) // \todo deprecated - remove
          {
            postMulticast_impl(state, mcast);
            return PAMI_SUCCESS;
          };
        pami_result_t postMulticast_impl(uint8_t (&state)[sizeof_msg],
                                        pami_multicast_t *mcast)
          {
            TRACE_DEVICE((stderr,"<%p>P2PMcastProto::multicast() id %zu, connection_id %d\n",this,mcast->dispatch,mcast->connection_id));

            // First, send (p2p) the header/msgdata to dispatch destinations.  They will start all-sided multicasts when dispatched.
            dispatch_hdr_t header;
            header.root  = _task_id;
            header.bytes = mcast->bytes;
            header.connection_id = mcast->connection_id;

            // Use sendi so we don't need to allocate storage. \todo does msgdata always fit in immediate send? Probably need a check.
            pami_send_immediate_t sendi;
            sendi.dispatch        = _dispatch_id;
            sendi.header.iov_base = (void*)&header;
            sendi.header.iov_len  = sizeof(header);
            sendi.data.iov_base   = (void*)mcast->msginfo;
            sendi.data.iov_len    = mcast->msgcount*sizeof(pami_quad_t);

            // \todo indexToRank() doesn't always work so convert a local copy to a list topology...
            PAMI::Topology l_dst_participants = *((PAMI::Topology*)mcast->dst_participants);
            l_dst_participants.convertTopology(PAMI_LIST_TOPOLOGY);
            pami_task_t *rankList=NULL;  l_dst_participants.rankList(&rankList);
            size_t  size    = l_dst_participants.size();
            for(unsigned i = 0; i< size; ++i)
                {
                  if(rankList[i]==_task_id) continue; // don't dispatch myself

                  sendi.dest = PAMI_ENDPOINT_INIT(_client, rankList[i],0);

                  // Dispatch over p2p
                  TRACE_DEVICE((stderr,"<%p>P2PMcastProto::multicast() send dispatch task_id[%d] %zu\n",
                                this, i, rankList[i]));
                  ((T_P2P_PROTOCOL*)&_p2p_protocol)->immediate(&sendi);
                }

            // No data? We're done.
            if(mcast->bytes == 0)
                {
/** \todo fix or remove this hack */
                  // call original done
                  if(mcast->cb_done.function)
                    (mcast->cb_done.function)(NULL,//PAMI_Client_getcontext(mcast->client,mcast->context),
                                              mcast->cb_done.clientdata, PAMI_SUCCESS);
                  return PAMI_SUCCESS;
                }

            allocation_t *allocation = (allocation_t *) _allocator.allocateObject();
            allocation->protocol = this; // so we can free it later
            TRACE_DEVICE((stderr,"<%p>P2PMcastProto::multicast() allocated %p, mcast %p, cb_done %p, client data %p\n",
                          this, allocation, mcast, mcast->cb_done.function, mcast->cb_done.clientdata));

            // work with a local copy of mcast and override cb_done and (maybe) src topology since all-sided requires it.
            pami_multicast_t l_mcast = *mcast;
            if(l_mcast.src_participants == NULL) // all-sided model expects a src/root topology
                {
                  new (&allocation->topology) PAMI::Topology(_task_id);
                  l_mcast.src_participants = (pami_topology_t *) &allocation->topology;
                }
            // Save the caller's cb_done and set our own so we can free the allocation
            allocation->cb_done = l_mcast.cb_done;
            l_mcast.cb_done.clientdata = (void*) allocation;
            l_mcast.cb_done.function = &done;

            //This is an all-sided multicast


            return _multicast_model.postMulticast(allocation->request,&l_mcast);
          }
        ///
        /// \brief multicast is done, free the allocation and call user cb_done
        ///
        /// \param[in] context
        /// \param[in] cookie (allocation pointer)
        /// \param[in] result
        ///
        static void done(pami_context_t   context,
                         void          * cookie,
                         pami_result_t    result )
          {
            allocation_t  *allocation = (allocation_t *) cookie;

            // get the original done cb (saved in allocation)
            pami_callback_t cb_done = allocation->cb_done;

            // Find (this) protocol and it's associated allocator
            P2PMcastProto<T_P2P_DEVICE,T_P2P_PROTOCOL, T_MULTICAST_MODEL,T_MULTICAST_DEVICE>  *protocol = allocation->protocol;

            TRACE_DEVICE((stderr,"<%p>P2PMcastProto::done() free allocation %p, cb_done %p, client data %p\n",
                          protocol, cookie, cb_done.function, cb_done.clientdata));

            memset(cookie, 0x00, sizeof(allocation_t)); // cleanup for debug
            protocol->_allocator.returnObject(cookie);  // and release storage

            // call original done
/** \todo fix or remove this hack */
            if(cb_done.function)
              (cb_done.function)(NULL,//PAMI_Client_getcontext(protocol->_client,protocol->_contextid),
                                 cb_done.clientdata, result);

            return;
          }


        ///
        /// \brief Received a p2p dispatch from another src (static function).  Call the member function on the protocol.
        ///
        static void dispatch_p2p(pami_context_t       context_hdl,  /**< IN:  communication context handle */
                                 void               * cookie,       /**< IN:  dispatch cookie (pointer to protocol object)*/
                                 const void         * header,       /**< IN:  header address  */
                                 size_t               header_size,  /**< IN:  header size     */
                                 const void         * data,         /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
                                 size_t               data_size,    /**< IN:  number of byts of message data, valid regarldless of message type */
                                 pami_endpoint_t      origin,
                                 pami_recv_t        * recv)         /**< OUT: receive message structure, only needed if addr is non-NULL */
          {
            TRACE_DEVICE((stderr,"<%p>P2PMcastProto::dispatch_p2p header size %zu, data size %zu\n",cookie, header_size, data_size));
            P2PMcastProto<T_P2P_DEVICE,T_P2P_PROTOCOL,T_MULTICAST_MODEL,T_MULTICAST_DEVICE> *p = (P2PMcastProto<T_P2P_DEVICE,T_P2P_PROTOCOL,T_MULTICAST_MODEL,T_MULTICAST_DEVICE> *)cookie;
            p->dispatch(context_hdl,
                        header,
                        header_size,
                        data,
                        data_size,
                        origin,
                        recv);
          }
        ///
        /// \brief Received a p2p dispatch from another src (member function).
        /// Call user's dispatch, allocate some storage and start all-sided multicast.
        ///
        void dispatch(pami_context_t       context_hdl,  /**< IN:  communication context handle */
                      const void         * header,       /**< IN:  header address  */
                      size_t               header_size,  /**< IN:  header size     */
                      const void         * data,         /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
                      size_t               data_size,    /**< IN:  number of byts of message data, valid regarldless of message type */
                      pami_endpoint_t      origin,
                      pami_recv_t        * recv)         /**< OUT: receive message structure, only needed if addr is non-NULL */
          {
            TRACE_DEVICE((stderr,"<%p>P2PMcastProto::dispatch() header size %zu, data size %zu\n",this, header_size, data_size));

            // Call user's dispatch to get receive pwq and cb_done.
            pami_multicast_t mcast;
            mcast.connection_id = ((dispatch_hdr_t*)header)->connection_id;
            mcast.bytes         = ((dispatch_hdr_t*)header)->bytes;
            _dispatch_fn((pami_quad_t*)data, (unsigned)data_size/sizeof(pami_quad_t), mcast.connection_id, (size_t)(((dispatch_hdr_t*)header)->root), mcast.bytes, _cookie, &mcast.bytes, &mcast.dst, &mcast.cb_done);

            // No data? We're done.
            if(mcast.bytes == 0)
                {
                  // call original done
                  /** \todo fix or remove this hack */
                  if(mcast.cb_done.function)
                    (mcast.cb_done.function)(NULL,//PAMI_Client_getcontext(_client,_contextid),
                                             mcast.cb_done.clientdata, PAMI_SUCCESS);
                  return;
                }

            // Allocate storage and call all-sided multicast.

            allocation_t *allocation = (allocation_t *) _allocator.allocateObject();
            allocation->protocol = this; // so we can free it later
            TRACE_DEVICE((stderr,"<%p>P2PMcastProto::dispatch_p2p() allocated %p, cb_done %p, client data %p\n",
                          this, allocation, mcast.cb_done.function, mcast.cb_done.clientdata));

            mcast.src_participants = NULL; // I'm a dst so all-sided mcast src can be null

            // Save the caller's cb_done and set our own so we can free the allocation
            allocation->cb_done = mcast.cb_done;
            mcast.cb_done.clientdata = (void*) allocation;
            mcast.cb_done.function = &done;

            mcast.dispatch = _dispatch_id;
            //mcast.hints = 0;
            mcast.roles = 0;

            mcast.src = NULL;
            mcast.dst_participants = (pami_topology_t *) &_dst_participants; // this task is dst
            mcast.msginfo = NULL;
            mcast.msgcount = 0;

            _multicast_model.postMulticast(allocation->request,
                                           &mcast);


          }
        PAMI::Topology                 _dst_participants; // default dst is this task when dispatched
        size_t                        _dispatch_data_id;
        size_t                        _dispatch_header_id;
        size_t                        _dispatch_id;
        pami_dispatch_multicast_function _dispatch_fn;
        pami_client_t                  _client;
        pami_context_t                 _context;
        size_t                        _contextid;
        size_t                        _clientid;
        size_t                        _task_id;
        void                         *_cookie;
        char                          _p2p_protocol[sizeof(T_P2P_PROTOCOL)]; // p2p send protocol
        T_MULTICAST_MODEL             _multicast_model; // all-sided model
        MemoryAllocator < sizeof(allocation_t), 16 > _allocator;
      }; // PAMI::Protocol::P2PMcastProto class
    };   // PAMI::Protocol::MPI namespace
  };   // PAMI::Protocol namespace
};     // PAMI namespace
#undef TRACE_DEVICE
#endif //__devices_mpi_mpimulticastprotocol_h__
