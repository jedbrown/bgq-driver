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
/// \file p2p/protocols/send/adaptive/AdaptiveSimple.h
/// \brief Simple eager send protocol for reliable devices.
///
/// The EagerSimple class defined in this file uses C++ templates and
/// the device "message" interface - which also uses C++ templates.
///
/// C++ templates require all source code to be #include'd from a header file.
///
#ifndef __p2p_protocols_send_adaptive_AdaptiveSimple_h__
#define __p2p_protocols_send_adaptive_AdaptiveSimple_h__

#include "components/memory/MemoryAllocator.h"

#include "p2p/protocols/send/adaptive/AdaptiveConnection.h"
#include "util/queue/Queue.h"

#define WINDOW 8

#ifndef TRACE_ERR
#define TRACE_ERR(x) fprintf x
#endif

namespace PAMI
{
  namespace Protocol
  {
    namespace Send
    {

      ///
      /// \brief Adaptive simple send protocol class for reliable network devices.
      ///
      /// \tparam T_Model   Template packet model class
      /// \tparam T_Device  Template packet device class
      /// \tparam T_Message Template packet message class
      ///
      /// \see PAMI::Device::Interface::PacketModel
      /// \see PAMI::Device::Interface::PacketDevice
      ///
      template <class T_Model, class T_Device, bool T_LongHeader>
      class AdaptiveSimple
      {
        protected:

          //typedef uint8_t pkt_t[T_Model::packet_model_state_bytes];

                  typedef uint8_t pkt_t[T_Model::packet_model_state_bytes] ;
          //forward declaration
          class  send_state_t;
          struct recv_state_t;


          //cts_info
          struct cts_info_t
          {
            send_state_t *                   va_send;   ///virtual address sender
            recv_state_t *                   va_recv;   ///virtual address receiver
            size_t                           bytes;	   ///total bytes to send
            pami_task_t                       destRank;  /// destination rank
          };


            //rts_ack header (metadata)
          struct __attribute__((__packed__)) header_rts_ack_t
          {
            recv_state_t *            va_recv;     ///virtual address receiver
            send_state_t *            va_send;     ///virtual address receiver
            size_t                    destRank;    ///Offset
        };

          //receiver status
          struct recv_state_t
          {
            cts_info_t          cts;                               /// cts info
                        header_rts_ack_t    header_rts;                         /// header used by rts_ack
            pkt_t               pkt;                            /// Message Object
            AdaptiveSimple<T_Model, T_Device, T_LongHeader> * adaptive;  /// Pointer to protocol
            pami_recv_t     info;                                   /// Application receive information.
            size_t                           mbytes;                /// Message info bytes
            pami_task_t                       fromRank;             ///origen rank
            char *                           msgbuff;              /// RTS data buffer
            size_t                           msgbytes;		     /// RTS bytes received
                        void                             * ackinfo;   ///< a.k.a. send_state_t *
          };


          //data header (metadata)
          struct __attribute__((__packed__)) header_metadata_t
          {
            recv_state_t *            va_recv;   ///virtual address receiver
                        size_t                    offset;    ///Offset
            size_t                    bsend;     ///bytes send
                        //size_t                    gap;     ///bytes send
                        //size_t                    gap1;     ///bytes send
        };





          //rts_info
          struct rts_info_t
          {
            send_state_t *               va_send;	  ///virtual address sender
            size_t                       bytes;	  ///total bytes to send
            pami_task_t                   fromRank;  ///origen rank
            pami_task_t                   destRank;  ///target rank
            size_t                       mbytes;     /// application metadata bytes
                        void                        * ackinfo;   ///< a.k.a. send_state_t *

          };


          struct package_t
          {
            header_metadata_t             header;
            pkt_t                         pkt;               ///Array of pkt_t to send
            struct iovec                  iov[2];
          };


          struct window_t
          {
            send_state_t *               va_send;
            package_t                    pkg[WINDOW];               ///Array of pkt_t to send
          };


          //send_state_class

          class send_state_t: public PAMI::Queue::Element
          {
            public:

              size_t               sendlen;         /// Number of bytes to send from the origin rank.
              size_t               next_offset;	    /// Next offset
              recv_state_t *       va_recv;         /// Receiver Virtual Address
              size_t               send_bytes;      /// Total bytes to send
              char *               send_buffer;     /// Send Buffer address
              pkt_t                pkt;             /// packet
              rts_info_t           rts;             /// RTS struct info
              struct iovec         iov[2];          /// iovec array used to post the rts and data packets
              header_metadata_t    header;          /// header_metadata
              pami_event_function   cb_data;         /// Callback to execute when data have been sent
              pami_event_function   cb_rts;          /// Callback to execute when rts have been sent
              void   *             pf;              /// Pointer to receiver parameters
              void                 * msginfo;       /// Message info
              pami_event_function   local_fn;        /// Local Completion callback.
              pami_event_function   remote_fn;       /// Remote Completion callback.
              void                 * cookie;        /// Cookie

              AdaptiveSimple<T_Model, T_Device, T_LongHeader> * adaptive;

              window_t        window[2];               ///Array of pkt_t to send
          };

        public:

          ///
          /// \brief Adaptive simple send protocol constructor.
          ///
          /// \param[in]  dispatch     Dispatch identifier
          /// \param[in]  dispatch_fn  Dispatch callback function
          /// \param[in]  cookie       Opaque application dispatch data
          /// \param[in]  device       Device that implements the message interface
          /// \param[in]  origin_task  Origin task identifier
          /// \param[in]  context      Communication context
          /// \param[out] status       Constructor status
          ///
          inline AdaptiveSimple (size_t                     dispatch,
                                 pami_dispatch_callback_function dispatch_fn,
                                 void                     * cookie,
                                 T_Device                 & device,
                                 pami_task_t                 origin_task,
                                 pami_client_t              client,
                                 size_t                     contextid,
                                 pami_result_t             & status) :
              _rts_model (device, client, contextid),
              _rts_ack_model (device, client, contextid),
              _rts_data_model (device, client, contextid),
              _data_model (device, client, contextid),
              _cts_model (device, client, contextid),
              _ack_model (device, client, contextid),
              _device (device),
              _fromRank (origin_task),
              _client (client),
              _contextid (contextid),
              _dispatch_fn (dispatch_fn),
              _cookie (cookie),
              _connection ((void **)NULL),
              _connection_manager (device),
              _counter(0)
          {
            // ----------------------------------------------------------------
            // Compile-time assertions
            // ----------------------------------------------------------------

            // This protocol only works with reliable networks.
            //COMPILE_TIME_ASSERT(T_Model::reliable_packet_model == true);


            // Assert that the size of the packet payload area is large
            // enough to transfer a single virtual address. This is used in
            // the postPacket() calls to transfer the ack information.
            COMPILE_TIME_ASSERT(sizeof(void *) <= T_Model::packet_model_payload_bytes);

            // ----------------------------------------------------------------
            // Compile-time assertions
            // ----------------------------------------------------------------


            _connection = _connection_manager.getConnectionArray (client, contextid);

            //No tested
            //Queue Setup
            //allocate memory
	    pami_result_t prc;
	    prc = __global.heap_mm->memalign((void **)&_queue, 0,
						sizeof(*_queue) * _device.peers());
	    PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _queue failed");

            //Initializing queue
            for (size_t i = 0; i < _device.peers(); i++)
              new (&_queue[i]) Queue();

            TRACE_ERR((stderr, "AdaptiveSimple() [0]\n"));
            status = _rts_model.init (dispatch,
                                      dispatch_rts_direct, this,
                                      dispatch_rts_read, this);

            TRACE_ERR((stderr, "AdaptiveSimple() [1] status = %d\n", status));

            if (status == PAMI_SUCCESS)
              {
                status = _rts_ack_model.init (dispatch,
                                              dispatch_rts_ack_direct, this,
                                              dispatch_rts_ack_read, this);
                TRACE_ERR((stderr, "AdaptiveSimple() [2] status = %d\n", status));

                if (status == PAMI_SUCCESS)
                  {
                    status = _rts_data_model.init (dispatch,
                                                   dispatch_rts_data_direct, this,
                                                   dispatch_rts_data_read, this);
                    TRACE_ERR((stderr, "AdaptiveSimple() [3] status = %d\n", status));

                    if (status == PAMI_SUCCESS)
                      {
                        status = _data_model.init (dispatch,
                                                   dispatch_data_direct, this,
                                                   dispatch_data_read, this);
                        TRACE_ERR((stderr, "AdaptiveSimple() [4] status = %d\n", status));

                        if (status == PAMI_SUCCESS)
                          {
                            status = _cts_model.init (dispatch,
                                                      dispatch_cts_direct, this,
                                                      dispatch_cts_read, this);
                            TRACE_ERR((stderr, "AdaptiveSimple() [5] status = %d\n", status));


                            if (status == PAMI_SUCCESS)
                              {
                                status = _ack_model.init (dispatch,
                                                          dispatch_ack_direct, this,
                                                          dispatch_ack_read, this);
                                TRACE_ERR((stderr, "AdaptiveSimple() [6] status = %d\n", status));
                              }

                          }
                      }
                  }
              }
          }




          ///
          /// Function to PostRTS
          ///


          static int PostRTS(send_state_t * send)
          {
            if (send->rts.bytes == 0)
              {
                // This branch should be resolved at compile time and optimized out.
                if (sizeof(rts_info_t) <= T_Model::packet_model_metadata_bytes)
                  {
                    TRACE_ERR((stderr, "AdaptiveSimple::simple_impl() .. zero-byte data special case, protocol rts_info_t fits in the packet metadata\n"));

                    if (send->rts.mbytes > T_Model::packet_model_payload_bytes)
                      {
                        TRACE_ERR((stderr, "AdaptiveSimple::simple_impl() .. zero-byte data special case, protocol rts_info_t fits in the packet metadata, application metadata does not fit in a single packet payload\n"));


                        TRACE_ERR((stderr, "AdaptiveSimple::simple_impl() .. zero-byte data special case, protocol rts_info_t fits in the packet metadata, application metadata does not fit in a single packet payload, pami_task_t does fit in the message metadata\n"));

                        //send RTS info without message info
                        send->adaptive->_rts_model.postPacket (send->pkt,         ///T_Message to send
                                                               NULL,                     ///Callback to execute when done
                                                               (void *) send,            /// Cookie
                                                               send->rts.destRank,                     ///Target Rank
                                                               (void *)&send->rts,       ///rts_info_t struct  metadata
                                                               sizeof(rts_info_t),       ///sizeof rts_info_t  metadata
                                                               (void *) NULL, 0);

                      }
                    else
                      {

                        TRACE_ERR((stderr, "AdaptiveSimple::simple_impl() .. zero-byte data special case, protocol rts_info_t fits in the packet metadata, application metadata fit in a single packet payload\n"));
                        //Everything OK                               ///Post rts package
                        pami_event_function fn = NULL;
                        if (unlikely(send->remote_fn == NULL))
                          fn = send_complete;

                        send->adaptive->_rts_model.postPacket (send->pkt,             ///T_Message to send
                                                               fn,                     ///Callback to execute when done
                                                               (void *) send,             /// Cookie
                                                               send->rts.destRank,                        ///Target Rank
                                                               (void *)&send->rts,          ///rts_info_t struct metadata
                                                               sizeof(rts_info_t),          ///sizeof rts_info_t metadata
                                                               (void *)send->msginfo,
                                                               send->rts.mbytes);                     ///Number of bytes
                      }
                  }
                else  //sizeof(rts_info_t) > T_Model::packet_model_metadata_bytes
                  {
                    TRACE_ERR((stderr, "AdaptiveSimple::simple_impl() .. zero-byte data special case, protocol rts_info_t does not fit in the packet metadata\n"));

                    if (send->rts.mbytes  > (T_Model::packet_model_payload_bytes - sizeof(rts_info_t)))
                      {
                        TRACE_ERR((stderr, "AdaptiveSimple::simple_impl() .. zero-byte data special case, protocol rts_info_t does not fit in the packet metadata, application metadata does not fit in a single packet payload\n"));



                        TRACE_ERR((stderr, "AdaptiveSimple::simple_impl() .. zero-byte data special case, protocol rts_info_t does not fit in the packet metadata, application metadata does not fit in a single packet payload, pami_task_t does fit in the message metadata\n"));

                        send->adaptive->_rts_model.postPacket (send->pkt,         ///T_Message to send
                                                               NULL,                     ///Callback to execute when done
                                                               (void *) send,            /// Cookie
                                                               send->rts.destRank,                     ///Target Rank
                                                               (void *)NULL,             ///Metadata
                                                               0,                        ///Metadata size
                                                               (void *) & send->rts,
                                                               sizeof(rts_info_t));

                      }
                    else
                      {
                        TRACE_ERR((stderr, "AdaptiveSimple::simple_impl() .. zero-byte data special case, protocol rts_info_t does not fit in the packet metadata, application metadata fits in a single packet payload\n"));

                        //OK CASE
                        send->iov[0].iov_base = (void *) & send->rts;
                        send->iov[0].iov_len  = sizeof(rts_info_t);
                        send->iov[1].iov_base = (void *)send->msginfo;
                        send->iov[1].iov_len  = send->rts.mbytes;

                        pami_event_function fn = NULL;
                        if (unlikely(send->remote_fn == NULL))
                          fn = send_complete;

                        send->adaptive->_rts_model.postPacket (send->pkt,       ///T_Message to send
                                                               fn,                    ///Callback to execute when done
                                                               (void *) send,             /// Cookie
                                                               send->rts.destRank,                        ///Target Rank
                                                               (void *)NULL,                ///Metadata
                                                               0,                           ///Metadata size
                                                               send->iov);
                        //End Sender side, 0 bytes
                      }
                  }
              }
            else
              {
                if (sizeof(rts_info_t) <= T_Model::packet_model_metadata_bytes)
                  {
                    TRACE_ERR((stderr, "AdaptiveSimple::simple_impl() .. with data, protocol rts_info_t fits in the packet metadata\n"));

                    if (send->rts.mbytes  > T_Model::packet_model_payload_bytes)
                      {
                        TRACE_ERR((stderr, "AdaptiveSimple::simple_impl() .. with data, protocol rts_info_t fits in the packet metadata, application metadata does not fit in a single packet payload\n"));


                        TRACE_ERR((stderr, "AdaptiveSimple::simple_impl() .. with data, protocol rts_info_t fits in the packet metadata, application metadata does not fit in a single packet payload, pami_task_t does fit in the message metadata\n"));

                        send->adaptive->_rts_model.postPacket (send->pkt,           ///T_Message to send
                                                               NULL,                       ///Callback to execute when done
                                                               (void *) send,              /// Cookie
                                                               send->rts.destRank,                       ///Target Rank
                                                               (void *)&send->rts,         ///rts_info_t struct metadata
                                                               sizeof(rts_info_t),         ///sizeof rts_info_t metadata
                                                               (void *) NULL,
                                                               0);
                      }
                    else
                      {
                        TRACE_ERR((stderr, "AdaptiveSimple::simple_impl() .. with data, protocol rts_info_t fits in the packet metadata, application metadata fit in a single packet payload\n"));

                        send->iov[0].iov_base = (void *) & send->rts;
                        send->iov[0].iov_len  = sizeof(rts_info_t);
                        send->iov[1].iov_base = (void *)send->msginfo;
                        send->iov[1].iov_len  = send->rts.mbytes;

                        //Everything OK
                        send->adaptive->_rts_model.postPacket (send->pkt,
                                                               NULL,
                                                               send->cookie,
                                                               send->rts.destRank,
                                                               (void *)&send->rts,
                                                               sizeof(rts_info_t),
                                                               send->iov);
                      }
                  }
                else
                  {
                    TRACE_ERR((stderr, "AdaptiveSimple::simple_impl() .. with data, protocol rts_info_t does not fit in the packet metadata\n"));

                    if (send->rts.mbytes > (T_Model::packet_model_payload_bytes - sizeof(rts_info_t)))
                      {
                        TRACE_ERR((stderr, "AdaptiveSimple::simple_impl() .. with data, protocol rts_info_t does not fit in the packet metadata, application metadata does not fit in a single packet payload\n"));


                        TRACE_ERR((stderr, "AdaptiveSimple::simple_impl() .. with data, protocol rts_info_t does not fit in the packet metadata, application metadata does not fit in a single packet payload, pami_task_t does fit in the message metadata\n"));

                        send->adaptive->_rts_model.postPacket (send->pkt,         ///T_Message to send
                                                               NULL,                     ///Callback to execute when done
                                                               (void *) send,            ///Cookie
                                                               send->rts.destRank,                     ///Target Rank
                                                               (void *)NULL,             ///Metadata
                                                               0,                        ///Metadata size
                                                               (void *) & send->rts,
                                                               sizeof(rts_info_t));
                      }
                    else
                      {

                        TRACE_ERR((stderr, "AdaptiveSimple::simple_impl() .. with data, protocol rts_info_t does not fit in the packet metadata, application metadata fits in a single packet payload\n"));

                        send->iov[0].iov_base = (void *) & send->rts;
                        send->iov[0].iov_len  = sizeof(rts_info_t);
                        send->iov[1].iov_base = (void *)send->msginfo;
                        send->iov[1].iov_len  = send->rts.mbytes;

                        send->adaptive->_rts_model.postPacket (send->pkt,
                                                               NULL,
                                                               (void *)send->cookie,
                                                               send->rts.destRank,
                                                               (void *)NULL,
                                                               0,
                                                               send->iov);
                      }
                  }
              }

            return 0;
          }

          ///
          /// \brief Start a new simple send adaptive operation.
          ///
          /// \see PAMI::Protocol::Send:simple
          ///
          inline pami_result_t simple_impl (pami_send_t * parameters)
          {
            TRACE_ERR((stderr, "*** Adpative_AdaptiveSimple implemented , msginfo_bytes = %d  , bytes =%d ***\n", parameters->send.header.iov_len, parameters->send.data.iov_len));
            TRACE_ERR((stderr, " T_Model::packet_model_metadata_bytes = %d\n T_Model::packet_model_payload_bytes = %d \n sizeof(rts_info_t) = %d\n sizeof(cts_info_t) = %d\n sizeof(send_state_t) = %d\n sizeof(recv_state_t) = %d\n sizeof(header_metadata_t) = %d\n sizeof(header_rts_ack_t) = %d\n", T_Model::packet_model_metadata_bytes, T_Model::packet_model_payload_bytes , sizeof(rts_info_t), sizeof(cts_info_t), sizeof(send_state_t), sizeof(recv_state_t), sizeof(header_metadata_t), sizeof(header_rts_ack_t)));

            // Allocate memory to maintain the state of the send.
            send_state_t * send = allocateSendState ();

            //Save data in send_state_t
            send->cookie    = parameters->events.cookie;    // save cookie
            send->local_fn  = parameters->events.local_fn;  // save Callback function to execute after local done
            send->remote_fn = parameters->events.remote_fn; // save Callback function to execute after remote done
            send->adaptive  = this;                         // Save pointer to protocol

            send->send_buffer =
              (char *) parameters->send.data.iov_base;      // sender buffer address
            send->next_offset = 0;                          // initialized next_offset for RTS

            send->rts.fromRank = _fromRank;                 // Origen Rank
            send->rts.bytes = parameters->send.data.iov_len;// Total bytes to send
            send->rts.va_send = send;                       // Virtual Address sender

            pami_task_t task;
            size_t offset;
            PAMI_ENDPOINT_INFO(parameters->send.dest,task,offset);

            send->rts.destRank = task;     // Target Rank
            send->rts.mbytes = parameters->send.header.iov_len; // Number of  Quads
            send->msginfo = parameters->send.header.iov_base;  // Message info

            send->sendlen = parameters->send.data.iov_len;  // Total bytes to send

            send->cb_data  =  cb_data_send;                 // register Callback function to execute after data package have been sent
            send->cb_rts  =  cb_rts_send;                     // register Callback function to execute after rts package have been sent

            send->header.offset = 0;                           //initialized RTS offset
            send->header.bsend = 0;                           //initialized RTS bytes
            send->window[0].va_send = send;
            send->window[1].va_send = send;

            // Set the acknowledgement information to the virtual address of
            // send state object on the origin task if a local callback of the
            // remote receive completion event is requested. If this is set to
            // NULL no acknowledgement will be received by the origin task.
            if (parameters->events.remote_fn != NULL)
              {
                send->rts.ackinfo = send;
                send->remote_fn = parameters->events.remote_fn;
              }
            else
              {
                send->rts.ackinfo = NULL;
                send->remote_fn = NULL;
              }


            TRACE_ERR((stderr, "\n		Adpative_AdaptiveSimple Info sender  cookie=%p , send =%p ,local_fn =%p , remote_fn =%p,  Sender rank  = %d, msinfo= %p , msgbytes = %d , bytes = %d \n", parameters->events.cookie, send, parameters->events.local_fn , parameters->events.remote_fn, parameters->send.dest,   parameters->send.header.iov_base,  parameters->send.header.iov_len, parameters->send.data.iov_len));


            //Send if queue is empty
            if (_queue[_fromRank].isEmpty())
              {
                PostRTS(send);
              }

            //Save in queue object to send
            if (parameters->send.data.iov_len != 0)
              _queue[_fromRank].pushTail(send);

            return PAMI_SUCCESS;
          };




        protected:
          inline send_state_t * allocateSendState ()
          {
            return (send_state_t *) _send_allocator.allocateObject ();
          }

          inline void freeSendState (send_state_t * object)
          {
            _send_allocator.returnObject ((void *) object);
          }

          inline recv_state_t * allocateRecvState ()
          {
            return (recv_state_t *) _recv_allocator.allocateObject ();
          }

          inline void freeRecvState (recv_state_t * object)
          {
            _recv_allocator.returnObject ((void *) object);
          }

          inline void setConnection (pami_task_t task, void * arg)
          {
            size_t peer = _device.task2peer (task);
            TRACE_ERR((stderr, ">> AdaptiveSimple::setConnection(%zu, %p) .. _connection[%zu] = %p\n", task, arg, peer, _connection[peer]));
            PAMI_assert(_connection[peer] == NULL);
            _connection[peer] = arg;
            TRACE_ERR((stderr, "<< AdaptiveSimple::setConnection(%zu, %p)\n", task, arg));
          }

          inline void * getConnection (pami_task_t task)
          {
            size_t peer = _device.task2peer (task);
            TRACE_ERR((stderr, ">> AdaptiveSimple::getConnection(%zu) .. _connection[%zu] = %p\n", task, peer, _connection[peer]));
            PAMI_assert(_connection[peer] != NULL);
            TRACE_ERR((stderr, "<< AdaptiveSimple::getConnection(%zu) .. _connection[%zu] = %p\n", task, peer, _connection[peer]));
            return _connection[peer];
          }

          inline void clearConnection (pami_task_t task)
          {
            size_t peer = _device.task2peer (task);
            TRACE_ERR((stderr, ">> AdaptiveSimple::clearConnection(%zu) .. _connection[%zu] = %p\n", task, peer, _connection[peer]));
            _connection[peer] = NULL;
            TRACE_ERR((stderr, "<< AdaptiveSimple::clearConnection(%zu) .. _connection[%zu] = %p\n", task, peer, _connection[peer]));
          }


          MemoryAllocator < sizeof(send_state_t), 16 > _send_allocator;
          MemoryAllocator < sizeof(recv_state_t), 16 > _recv_allocator;

          T_Model         _rts_model;
          T_Model         _rts_ack_model;
          T_Model         _rts_data_model;
          T_Model         _data_model;
          T_Model         _cts_model;
          T_Model         _ack_model;
          T_Device      & _device;
          pami_task_t      _fromRank;
          pami_client_t   _client;
          size_t      _contextid;
          pami_dispatch_callback_function _dispatch_fn;
          void                     * _cookie;
          void ** _connection;
          // Support up to 100 unique contexts.
          //static adaptive_connection_t _adaptive_connection[];
          AdaptiveConnection<T_Device> _connection_manager;

          static Queue *    _queue;

          size_t _counter;
          ///
          /// \brief Direct send rts packet dispatch.
          ///

          static int dispatch_rts_direct (void         * metadata,
                                          void         * payload,
                                          size_t         bytes,
                                          void         * recv_func_parm,
                                          void         * cookie)
          {

            TRACE_ERR((stderr, ">> AdaptiveSimple::process_rts()\n"));

            void * msginfo;
            rts_info_t * send;
            size_t total_payload = 0;

            AdaptiveSimple<T_Model, T_Device, T_LongHeader> * adaptive =
              (AdaptiveSimple<T_Model, T_Device, T_LongHeader> *) recv_func_parm;

            // Allocate memory to maintain the state of the rcv.
            recv_state_t * rcv = adaptive->allocateRecvState ();

            //save pointer to protocol
            rcv->adaptive = adaptive;




            if (sizeof(rts_info_t) <= T_Model::packet_model_metadata_bytes)
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts() .. rts_info_t coming inside metadata\n"));
                send = (rts_info_t *) metadata;
                total_payload = T_Model::packet_model_payload_bytes;
                msginfo = (void *) payload;

              }
            else
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts() .. rts_info_t coming inside payload\n"));
                send = (rts_info_t *) payload;
                total_payload = T_Model::packet_model_payload_bytes - sizeof(rts_info_t);
                msginfo = (void *) (send + 1);
              }





            //Save data in recv_state_t
            rcv->cts.va_send  = send->va_send;             ///Virtual address sender
            rcv->cts.destRank = send->destRank;            ///Target Rank
            rcv->cts.va_recv = rcv;                        ///Virual Address Receiver
            rcv->cts.bytes = send->bytes;                  ///Total Bytes to send
            rcv->fromRank  = send->fromRank;              ///Origin Rank
            rcv->mbytes  = send->mbytes;                    ///Metadata application bytes

	    pami_result_t prc;
	    prc = __global.heap_mm->memalign((void **)&rcv->msgbuff, 0,
		sizeof(*rcv->msgbuff) * send->mbytes);  ///Allocate buffer for Metadata
	    PAMI_assertf(prc == PAMI_SUCCESS, "alloc of buffer for Metadata failed");
            rcv->msgbytes = 0;                                            ///Initalized received bytes

            rcv->ackinfo = send->ackinfo;



            TRACE_ERR((stderr, "\n   AdaptiveSimple::process_rts()  rcv =%p , Sender rank  = %d, msinfo= %p , msgbytes = %d , bytes = %d \n", rcv , send->fromRank,  msginfo,  send->mbytes, send->bytes));


            if (send->mbytes <= total_payload)
              {

                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts() .. Application Metadata sent in only one package\n"));



                // Invoke the registered dispatch function.
                adaptive->_dispatch_fn.p2p (adaptive->_client,  // Communication context
                                            adaptive->_contextid,                //context id
                                            adaptive->_cookie,                   // Dispatch cookie
                                            send->fromRank,                   // Origin (sender) rank
                                            msginfo,                          // Application metadata
                                            send->mbytes,                      // Metadata bytes
                                            NULL,                             // No payload data
                                            send->bytes,                      // Number of msg bytes
                                            (pami_recv_t *) &(rcv->info));     //Recv_state struct



                // Only handle simple receives .. until the non-contiguous support
                // is available
                PAMI_assert(rcv->info.kind == PAMI_AM_KIND_SIMPLE);

                if (send->bytes == 0) // Move this special case to another dispatch funtion to improve latency in the common case.
                  {

                    TRACE_ERR((stderr, "   AdaptiveSimple::process_rts() .. Application Metadata send in only one package, 0 bytes received\n"));

                    // No data packets will follow this envelope packet. Invoke the
                    // recv done callback and, if an acknowledgement packet was
                    // requested send the acknowledgement. Otherwise return the recv
                    // state memory which was allocated above.


                    if (rcv->ackinfo != NULL)
                      {
                        TRACE_ERR((stderr, "   AdaptiveSimple::process_rts() ..  ACK was sent \n"));

                        adaptive->_ack_model.postPacket (rcv->pkt,
                                                         receive_complete,
                                                         (void *) rcv,
                                                         rcv->fromRank,
                                                         (void *) NULL,
                                                         0,
                                                         (void *) & (rcv->ackinfo),
                                                         sizeof (send_state_t *));

                      }
                    else
                      {
                        // Otherwise, return the receive state object memory to
                        // the memory pool.
                        receive_complete(PAMI_Client_getcontext(adaptive->_client, adaptive->_contextid), (void *) rcv, PAMI_SUCCESS); //Terminate


                      }


                    return 0;
                  }

                //Post CTS package info
                if (sizeof(cts_info_t) <= T_Model::packet_model_metadata_bytes)
                  {
                    TRACE_ERR((stderr, "   AdaptiveSimple::process_rts() .. protocol cts_info_t fits in the packet metadata\n"));


                    TRACE_ERR((stderr, "   AdaptiveSimple::process_rts() ..  protocol cts_info_t fits in the packet metadata, pami_task_t  fits in the message metadata\n"));

                    //Post cts package
                    adaptive->_cts_model.postPacket (rcv->pkt,
                                                     NULL,
                                                     rcv->info.cookie,
                                                     send->fromRank,
                                                     &rcv->cts,             ///rts_info_t struct
                                                     sizeof (cts_info_t),   ///rts_info_t size
                                                     (void *) NULL, 0);
                  }
                else
                  {
                    TRACE_ERR((stderr, "   AdaptiveSimple::process_rts() .. protocol cts_info_t does not fit in the packet metadata\n"));

                    TRACE_ERR((stderr, "   AdaptiveSimple::process_rts() ..  protocol cts_info_t does not fit in the packet metadata, pami_task_t  fits in the message metadata\n"));

                    //Post cts package
                    adaptive->_cts_model.postPacket (rcv->pkt,
                                                     NULL,
                                                     rcv->info.cookie,
                                                     send->fromRank,
                                                     NULL,
                                                     0,
                                                     (void *) & rcv->cts,
                                                     sizeof (cts_info_t));
                  }

                return 0;
              }
            else

              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts() .. Application Metadata will be send on multiple packages\n"));

                rcv->header_rts.va_recv = rcv;              //Receiver Virtual Address
                rcv->header_rts.va_send = send->va_send;    //Sender Virtual Address
                rcv->header_rts.destRank = send->destRank;  // Target Rank

                if (sizeof(header_rts_ack_t) <= T_Model::packet_model_metadata_bytes)
                  {
                    TRACE_ERR((stderr, "   AdaptiveSimple::process_rts() .. header_rts_ack_t fits in the packet metadata\n"));


                    TRACE_ERR((stderr, "   AdaptiveSimple::process_rts() ..  protocol header_rts_ack_t fits in the packet metadata, pami_task_t  fits in the message metadata\n"));

                    //Post rts_ack package
                    adaptive->_rts_ack_model.postPacket (rcv->pkt,
                                                         NULL,
                                                         rcv->info.cookie,
                                                         send->fromRank,
                                                         (void *)&rcv->header_rts,
                                                         sizeof (header_rts_ack_t),
                                                         (void *) NULL, 0);

                  }
                else
                  {
                    TRACE_ERR((stderr, "   AdaptiveSimple::process_rts() .. protocol header_rts_ack_t does not fit in the packet metadata\n"));


                    TRACE_ERR((stderr, "   AdaptiveSimple::process_rts() ..  protocol header_rts_ack_t does not fit in the packet metadata, pami_task_t  fits in the message metadata\n"));

                    //Post rts_ack package
                    adaptive->_rts_ack_model.postPacket (rcv->pkt,
                                                         NULL,
                                                         rcv->info.cookie,
                                                         send->fromRank,
                                                         NULL,
                                                         0,
                                                         (void *) & rcv->header_rts,
                                                         sizeof (header_rts_ack_t));
                  }
              }

            TRACE_ERR((stderr, ">> AdaptiveSimple::process_rts() Done\n"));

            return 0;
          };


          ///
          /// \brief Direct send rts_ack   packet dispatch.
          ///
          static int dispatch_rts_ack_direct (void         * metadata,
                                              void         * payload,
                                              size_t         bytes,
                                              void         * recv_func_parm,
                                              void         * cookie)
          {
            TRACE_ERR((stderr, ">> AdaptiveSimple::process_rts_ack()\n"));

            header_rts_ack_t * ack;

            if (sizeof(header_rts_ack_t) <= T_Model::packet_model_metadata_bytes)
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_ack() .. header_rts_ack_t coming inside metadata\n"));
                //Pointer to cts_info_t
                ack = (header_rts_ack_t *) metadata;

                //Calculate rts first package bytes
                if ((ack->va_send->rts.mbytes % T_Model::packet_model_payload_bytes) == 0)
                  ack->va_send->header.bsend = T_Model::packet_model_payload_bytes;
                else
                  ack->va_send->header.bsend = (ack->va_send->rts.mbytes % T_Model::packet_model_payload_bytes);
              }
            else
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_ack() .. header_rts_ack_t coming inside payload\n"));
                //Pointer to cts_info_t
                ack = (header_rts_ack_t *) payload;

                //Calculate rts first package bytes
                if ((ack->va_send->rts.mbytes % (T_Model::packet_model_payload_bytes - sizeof(header_rts_ack_t))) == 0)
                  ack->va_send->header.bsend = T_Model::packet_model_payload_bytes - sizeof(header_rts_ack_t);
                else
                  ack->va_send->header.bsend = (ack->va_send->rts.mbytes % (T_Model::packet_model_payload_bytes - sizeof(header_rts_ack_t)));
              }

            //Save receiver VA
            ack->va_send->header.va_recv = ack->va_recv;

            //Pointer to Protocol object
            AdaptiveSimple<T_Model, T_Device, T_LongHeader> * adaptive =
              (AdaptiveSimple<T_Model, T_Device, T_LongHeader> *) recv_func_parm;

            TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_ack() .. Application Metadata bytes= %d  , send->next_offset = %d , offset=%d , bsend=%d \n", ack->va_send->rts.mbytes, ack->va_send->next_offset, ack->va_send->header.offset , ack->va_send->header.bsend));

            //Update next offset
            ack->va_send->next_offset += ack->va_send->header.bsend;

            //TRACE_ERR((stderr,"   AdaptiveSimple::process_rts_ack(), cookie =%p , destination rank =%d, header=%p , offset = %d, msginfo =%p , bytes to send =%d \n",cookie , send->rts.destRank, &send->header, send->header.offset, send->msginfo, send->header.bsend));

            if (sizeof(header_metadata_t) <= T_Model::packet_model_metadata_bytes)
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_ack() .. protocol header_metadata_t fits in the packet metadata\n"));


                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_ack() ..  protocol header_metadata_t fits in the packet metadata, pami_task_t  fits in the message metadata\n"));

                //Post rts_data package
                adaptive->_rts_data_model.postPacket (ack->va_send->pkt,
                                                      ack->va_send->cb_rts,
                                                      (void *) ack->va_send,
                                                      ack->va_send->rts.destRank,
                                                      (void *) &ack->va_send->header,   //header_metadata
                                                      sizeof (header_metadata_t),       //bytes
                                                      (void *)((char *)ack->va_send->msginfo + ack->va_send->header.offset),//Pointer to metadata receiver buffer
                                                      (size_t)ack->va_send->header.bsend);                                   //Send bytes
              }
            else
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_ack() .. protocol header_metadata_t does not fit in the packet metadata\n"));

                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_ack() ..  protocol header_metadata_t does not fit in the packet metadata, pami_task_t  fits in the message metadata\n"));

                //Post rts_data package
                ack->va_send->iov[0].iov_base = (void *) & ack->va_send->header;  //header_metadata
                ack->va_send->iov[0].iov_len  = sizeof (header_metadata_t);       //bytes
                ack->va_send->iov[1].iov_base = (void *)((char *)ack->va_send->msginfo + ack->va_send->header.offset); //Pointer to metadata receiver buffer
                ack->va_send->iov[1].iov_len  = (size_t)ack->va_send->header.bsend; //Send bytes

                adaptive->_rts_data_model.postPacket (ack->va_send->pkt,
                                                      ack->va_send->cb_rts,
                                                      (void *) ack->va_send ,
                                                      ack->va_send->rts.destRank,      // target rank
                                                      (void *) NULL,
                                                      0,
                                                      ack->va_send->iov);                                   //Send bytes

              }

            TRACE_ERR((stderr, ">> AdaptiveSimple::process_rts_ack() Done\n"));
            return 0;
          }


          ///
          /// \brief Direct send rts_data   packet dispatch.
          ///
          static int dispatch_rts_data_direct (void         * metadata,
                                               void         * payload,
                                               size_t         bytes,
                                               void         * recv_func_parm,
                                               void         * cookie)
          {
            TRACE_ERR((stderr, ">> AdaptiveSimple::process_rts_data()\n"));

            header_metadata_t * header;
            void * msginfo;

            //Pointer to protocol
            AdaptiveSimple<T_Model, T_Device, T_LongHeader> * adaptive =
              (AdaptiveSimple<T_Model, T_Device, T_LongHeader> *) recv_func_parm;

            if (sizeof(header_metadata_t) <= T_Model::packet_model_metadata_bytes)
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_data() .. header_metadata_t coming inside metadata\n"));

                //Pointer to metadata
                header = (header_metadata_t *) metadata;

                //copy to buffer
                memcpy ((char *)(header->va_recv->msgbuff) + header->offset, payload, header->bsend);
              }
            else
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_data() .. header_metadata_t coming inside payload\n"));
                //Pointer to metadata
                header = (header_metadata_t *) payload;

                //copy to buffer
                memcpy ((char *)(header->va_recv->msgbuff) + header->offset, (header + 1), header->bsend);
              }

            //Update total of bytes received
            header->va_recv->msgbytes += header->bsend;

            //TRACE_ERR((stderr,"   AdaptiveSimple::process_rts_data() .. received bytes =%d, offset = %d, bsend = %d\n", header->va_recv->msgbytes , header->offset, header->bsend));

            //Terminate after receiving all the bytes
            if (header->va_recv->msgbytes != header->va_recv->mbytes)
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_data() .. received packet\n"));
                return 0;
              }
            else
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_data() .. all metadadata received\n"));
                msginfo = header->va_recv->msgbuff;
              }

            //TRACE_ERR((stderr,"   AdaptiveSimple::process_rts_data() ...  header->va_recv->msgbuff  = %p , header->offest = %d , payload = %p , header->bsend = %d \n",header->va_recv->msgbuff, header->offset, payload, header->bsend));

            TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_data() ..before  _dispatch_fn.p2p  adaptive->_context = %p , adaptive->_contextid =%d , adaptive->_cookie =%p , header->va_recv->fromRank =%d,   msginfo =%p , header->va_recv->mbytes = %d, header->va_recv->cts.bytes = %d , header->va_recv->info =%p ,adaptive = %p ,_dispatch_fn.p2p = %p \n", adaptive->_client, adaptive->_contextid, adaptive->_cookie, header->va_recv->fromRank, msginfo, header->va_recv->mbytes,  header->va_recv->cts.bytes,  (pami_recv_t *) &(header->va_recv->info), adaptive ,  adaptive->_dispatch_fn.p2p));

            // Invoke the registered dispatch function.
            adaptive->_dispatch_fn.p2p (adaptive->_client,              // Communication context
                                        adaptive->_contextid,              // contextid
                                        adaptive->_cookie,                 // Dispatch cookie
                                        header->va_recv->fromRank,      // Origin (sender) rank
                                        msginfo,                        // Application metadata
                                        header->va_recv->mbytes,         // Metadata bytes
                                        NULL,                           // No payload data
                                        header->va_recv->cts.bytes,     // Number of msg bytes
                                        (pami_recv_t *) &(header->va_recv->info));   //recv_struct

            //free received buffer
            __global.heap_mm->free(header->va_recv->msgbuff);

            TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_data().. after dispatch_rts_data after p2p rcv->info.data.simple.addr  = %p , header->offest = %d , header->bsend = %d , rcv->info.data.simple.bytes =%d\n", header->va_recv->info.data.simple.addr, header->offset, header->bsend, header->va_recv->info.data.simple.bytes));

            // Only handle simple receives .. until the non-contiguous support
            // is available
            PAMI_assert(header->va_recv->info.kind == PAMI_AM_KIND_SIMPLE);

            if (header->va_recv->cts.bytes == 0) // Move this special case to another dispatch funtion to improve latency in the common case.
              {
                // No data packets will follow this envelope packet. Invoke the
                // recv done callback and, if an acknowledgement packet was
                // requested send the acknowledgement. Otherwise return the recv
                // state memory which was allocated above.

                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_data().. Done, 0 bytes received\n"));

                if (header->va_recv->ackinfo != NULL)
                  {
                    TRACE_ERR((stderr, "   AdaptiveSimple::process_data() ..  ACK was sent \n"));
                    adaptive->_ack_model.postPacket (header->va_recv->pkt,
                                                     receive_complete,
                                                     (void *) header->va_recv,
                                                     header->va_recv->fromRank,
                                                     (void *) NULL,
                                                     0,
                                                     (void *) & (header->va_recv->ackinfo),
                                                     sizeof (send_state_t *));
                  }
                else
                  {
                    // Otherwise, return the receive state object memory to
                    // the memory pool.
                    receive_complete(PAMI_Client_getcontext(adaptive->_client, adaptive->_contextid), (void *) header->va_recv, PAMI_SUCCESS); //Terminate
                  }

                return 0;
              }

            if (sizeof(cts_info_t) <= T_Model::packet_model_metadata_bytes)
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_data() .. protocol cts_info_t fits in the packet metadata\n"));
                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_data() ..  protocol cts_info_t fits in the packet metadata, pami_task_t  fits in the message metadata\n"));

                //Post cts package
                adaptive->_cts_model.postPacket (header->va_recv->pkt,
                                                 NULL,
                                                 header->va_recv->info.cookie,
                                                 header->va_recv->fromRank,
                                                 &header->va_recv->cts,               //cts structure
                                                 sizeof (cts_info_t),
                                                 (void *) NULL, 0);

              }
            else
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_data() .. protocol cts_info_t does not fit in the packet metadata\n"));
                TRACE_ERR((stderr, "   AdaptiveSimple::process_rts_data() ..  protocol cts_info_t does not fit in the packet metadata, pami_task_t  fits in the message metadata\n"));

                //Post cts package
                adaptive->_cts_model.postPacket (header->va_recv->pkt,
                                                 NULL,
                                                 header->va_recv->info.cookie,
                                                 header->va_recv->fromRank,
                                                 NULL,
                                                 0,
                                                 &header->va_recv->cts,
                                                 sizeof (cts_info_t));
              }

            TRACE_ERR((stderr, ">> AdaptiveSimple::process_rts_data() Done\n"));
            return 0;
          }


          ///
          /// \brief Direct send cts_data   packet dispatch.
          ///

          static int dispatch_cts_direct (void         * metadata,
                                          void         * payload,
                                          size_t         bytes,
                                          void         * recv_func_parm,
                                          void         * cookie)
          {
            TRACE_ERR((stderr, ">> AdaptiveSimple::process_cts()\n"));

            cts_info_t * rcv;


            if (sizeof(cts_info_t) <= T_Model::packet_model_metadata_bytes)
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_cts() .. cts_info_t coming inside metadata\n"));
                //Pointer to cts_info_t
                rcv = (cts_info_t *) metadata;
              }
            else
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_cts() .. cts_info_t coming inside payload\n"));
                //Pointer to cts_info_t
                rcv = (cts_info_t *) payload;
              }


            //Define bytes to send
            if (sizeof(header_metadata_t) <= T_Model::packet_model_metadata_bytes)
              {
                //Determine short package
                if ((rcv->bytes % T_Model::packet_model_payload_bytes) == 0)
                  rcv->va_send->header.bsend = T_Model::packet_model_payload_bytes;     ///Bytes to send in the package
                else
                  rcv->va_send->header.bsend = (rcv->bytes % T_Model::packet_model_payload_bytes);   ///Bytes to send in the package

              }
            else
              {

                if ((rcv->bytes % (T_Model::packet_model_payload_bytes - sizeof(header_metadata_t))) == 0)
                  rcv->va_send->header.bsend = T_Model::packet_model_payload_bytes - sizeof(header_metadata_t);    ///Bytes to send in the package
                else
                  rcv->va_send->header.bsend = (rcv->bytes % (T_Model::packet_model_payload_bytes - sizeof(header_metadata_t)));  ///Bytes to send in the package
              }





            //Pointer to protocol
            AdaptiveSimple<T_Model, T_Device, T_LongHeader> * adaptive =
              (AdaptiveSimple<T_Model, T_Device, T_LongHeader> *) recv_func_parm;

            TRACE_ERR((stderr, "   AdaptiveSimple::process_cts() .. va_recv = %p , to_send = %d , payload_bytes = %d, next_offset= %d , total of bytes=%d\n", rcv->va_recv, rcv->va_send->header.bsend, T_Model::packet_model_payload_bytes, rcv->va_send->next_offset, rcv->va_send->sendlen));

            rcv->va_send->header.va_recv  = rcv->va_recv;    ///Virtual address reciver
            rcv->va_send->header.offset  = 0;                ///Initialize offset to zero
            rcv->va_send->next_offset = rcv->va_send->header.bsend;///Next offset value
            rcv->va_send->pf = recv_func_parm;               ///Save pointer to recv_func_param

            if (sizeof(header_metadata_t) <= T_Model::packet_model_metadata_bytes)
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_cts() .. protocol header_metadata_t fits in the packet metadata\n"));

                TRACE_ERR((stderr, "   AdaptiveSimple::process_cts() ..  protocol header_metadata_t fits in the packet metadata, pami_task_t  fits in the message metadata\n"));

                //Post First Data package
                //TRACE_ERR((stderr, "   AdaptiveSimple::process_cts() .. v[0].iov_base = %p ,v[0].iov_len=%d\n", v[0].iov_base, v[0].iov_len));

                adaptive->_data_model.postPacket   (rcv->va_send->pkt,          ///T_message to send
                                                    NULL,         ///Callback to execute when done
                                                    (void *)rcv->va_send,
                                                    rcv->destRank,                 ///target Rank
                                                    &rcv->va_send->header,         ///metadata
                                                    sizeof (header_metadata_t),    ///metadata size
                                                    (void *)(rcv->va_send->send_buffer),
                                                    rcv->va_send->header.bsend);

                cb_data_send(PAMI_Client_getcontext(adaptive->_client, adaptive->_contextid), (void *) &rcv->va_send->window[0], PAMI_SUCCESS);  //call function immediatly
                TRACE_ERR((stderr, "   AdaptiveSimple::process_cts() ..  rcv->va_send->window[0]= %p\n", &rcv->va_send->window[0] ));

                //cb_data_send(adaptive->_context, (void *) &rcv->va_send->window[1], PAMI_SUCCESS);  //call function immediatly
                //TRACE_ERR((stderr, "   AdaptiveSimple::process_cts() ..  rcv->va_send->window[1]= %p\n",&rcv->va_send->window[1] ));

              }
            else
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_cts() .. protocol header_metadata_t does not fit in the packet metadata\n"));


                TRACE_ERR((stderr, "   AdaptiveSimple::process_cts() ..  protocol header_metadata_t does not fit in the packet metadata, pami_task_t  fits in the message metadata\n"));

                //Post Fisrt data package
                rcv->va_send->iov[0].iov_base = &rcv->va_send->header;         // metadata
                rcv->va_send->iov[0].iov_len  = sizeof (header_metadata_t);    // metadata size
                rcv->va_send->iov[1].iov_base = (void *)(rcv->va_send->send_buffer );
                rcv->va_send->iov[1].iov_len  = rcv->va_send->header.bsend;

                //call callback
                adaptive->_data_model.postPacket   ( rcv->va_send->pkt,          ///T_message to send
                                                     NULL,         ///Callback to execute when done
                                                     (void *)rcv->va_send,
                                                     rcv->destRank,                 ///target Rank
                                                     (void *)NULL,
                                                     0,
                                                     rcv->va_send->iov);

                //call cb_data send
                cb_data_send(PAMI_Client_getcontext(adaptive->_client, adaptive->_contextid), (void *) &rcv->va_send->window[0], PAMI_SUCCESS);  //call function immediatly
                //cb_data_send(adaptive->_context, (void *) &rcv->va_send->window[1], PAMI_SUCCESS);  //call function immediatly


              }

            //Remove Head from queue
            adaptive->_queue[adaptive->_fromRank].popHead();

            //Check queue is not empty
            if (!adaptive->_queue[adaptive->_fromRank].isEmpty())
              {
                //recover next object to send
                send_state_t * send = (send_state_t *) (adaptive->_queue[adaptive->_fromRank].peekHead());


                PostRTS(send);

              }

            TRACE_ERR((stderr, ">> AdaptiveSimple::process_cts()\n"));


            //Terminate Sender
            // if (rcv->va_send->remote_fn == NULL){

            //        send_complete(PAMI_Client_getcontext(adaptive->_client, adaptive->_contextid), (void *) rcv->va_send, PAMI_SUCCESS); //Terminate

            //}

            return 0;
          }


          ///
          /// \brief Direct send data   packet dispatch.
          ///
          static int dispatch_data_direct   (void         * metadata,
                                             void         * payload,
                                             size_t         bytes,
                                             void         * recv_func_parm,
                                             void         * cookie)
          {
            TRACE_ERR((stderr, ">> AdaptiveSimple::process_data()\n"));

            header_metadata_t * header;
            void * msginfo;


            if (sizeof(header_metadata_t) <= T_Model::packet_model_metadata_bytes)
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_data() .. header_metadata_t coming inside metadata\n"));

                //Pointer to metadata
                header = (header_metadata_t *) metadata;

                TRACE_ERR((stderr, "   AdaptiveSimple::process_data() .. before memcpy buffer address =%p ,  bytes for receiving =%d, offset = %d, bsend = %d\n", header->va_recv->info.data.simple.addr, header->va_recv->info.data.simple.bytes , header->offset, header->bsend));


                //copy data to buffer
                memcpy ((char *)(header->va_recv->info.data.simple.addr) + header->offset, payload, header->bsend);
              }
            else
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_data() .. header_metadata_t coming inside payload\n"));

                //Pointer to metadata
                header = (header_metadata_t *) payload;

                //copy data to buffer
//fprintf (stderr, "dispatch_data_direct() .. header = %p, header+1 = %p, header->bsend = %zu, header->offset = %zu\n", header, header+1, header->bsend, header->offset);
//fprintf (stderr, "dispatch_data_direct() .. header->va_recv = %p\n", header->va_recv);
//fprintf (stderr, "dispatch_data_direct() .. header->va_recv->info.data.simple.addr = %p\n", header->va_recv->info.data.simple.addr);

                memcpy ((char *)(header->va_recv->info.data.simple.addr) + header->offset, (header + 1), header->bsend);
              }

            AdaptiveSimple<T_Model, T_Device, T_LongHeader> * adaptive =
              (AdaptiveSimple<T_Model, T_Device, T_LongHeader> *) header->va_recv->adaptive;

            //Update total of bytes received
            header->va_recv->info.data.simple.bytes -= header->bsend;

            TRACE_ERR((stderr, "   AdaptiveSimple::process_data() .. buffer address =%p ,  bytes for receiving =%d, offset = %d, bsend = %d\n", header->va_recv->info.data.simple.addr, header->va_recv->info.data.simple.bytes , header->offset, header->bsend));

            //Terminate after receiving all the bytes
            if (header->va_recv->info.data.simple.bytes <= 0)
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::process_data() ..  all data Received \n"));


                if (header->va_recv->ackinfo != NULL)
                  {
                    TRACE_ERR((stderr, "   AdaptiveSimple::process_data() ..  ACK was sent \n"));
                    adaptive->_ack_model.postPacket (header->va_recv->pkt,
                                                     receive_complete,
                                                     (void *) header->va_recv,
                                                     header->va_recv->fromRank,
                                                     (void *) NULL,
                                                     0,
                                                     (void *) & (header->va_recv->ackinfo),
                                                     sizeof (send_state_t *));
                  }
                else
                  {
                    // Otherwise, return the receive state object memory to
                    // the memory pool.
                    receive_complete(PAMI_Client_getcontext(adaptive->_client, adaptive->_contextid), (void *) header->va_recv, PAMI_SUCCESS); //Terminate
                  }

                return 0;
              }

            TRACE_ERR((stderr, "<<   AdaptiveSimple::process_data() ..  data package Received\n"));

            return 0;
          };


          ///
          /// \brief Callback invoked after send a rts_data packet.
          ///

          static void cb_rts_send (pami_context_t   context,
                                   void          * cookie,
                                   pami_result_t    result)
          {

            TRACE_ERR((stderr, ">> AdaptiveSimple::Callback_rts_send()\n"));

            //Fix Bug Share Memory Device
            //usleep(200);

            //Pointer to send state
            send_state_t * send = (send_state_t *) cookie;

            //Pointer to Protocol object
            AdaptiveSimple<T_Model, T_Device, T_LongHeader> * adaptive =
              (AdaptiveSimple<T_Model, T_Device, T_LongHeader> *) send->adaptive;

            //Define bytes to send
            if (sizeof(header_metadata_t) <= T_Model::packet_model_metadata_bytes)
              {
                send->header.bsend = T_Model::packet_model_payload_bytes;
              }
            else
              {
                send->header.bsend = (T_Model::packet_model_payload_bytes - sizeof(header_metadata_t));
              }

            //Check if data to send
            if (send->rts.mbytes <= send->next_offset )
              {

                TRACE_ERR((stderr, ">>   AdaptiveSimple::Callback_rts_send() .. all rts_data was sent\n"));

                if (send->sendlen == 0)
                  {

                    TRACE_ERR((stderr, ">>   AdaptiveSimple::Callback_rts_send() .. Done, 0 bytes to send\n"));


                    if (send->remote_fn == NULL)
                      {

                        send_complete(PAMI_Client_getcontext(adaptive->_client, adaptive->_contextid), (void *) send, PAMI_SUCCESS); //Terminate

                      }


                  }

                return ;
              }

            TRACE_ERR((stderr, "   AdaptiveSimple::Callback_rts_send() .. Metadata bytes= %d  , send->next_offset = %d , offset=%d , bsend=%d \n", send->rts.mbytes, send->next_offset, send->header.offset , send->header.bsend));

            send->header.offset = send->next_offset;
            send->next_offset += send->header.bsend;

            //TRACE_ERR((stderr,"   AdaptiveSimple::Callback_rts_send() .. cookie =%p , destination rank =%d, header=%p , offset = %d, msginfo =%p , send =%d \n",cookie , send->rts.destRank, &send->header, send->header.offset, send->msginfo, send->header.bsend));

            if (sizeof(header_metadata_t) <= T_Model::packet_model_metadata_bytes)
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::Callback_rts_send() .. protocol header_metadata_t fits in the packet metadata\n"));


                TRACE_ERR((stderr, "   AdaptiveSimple::Callback_rts_send() ..  protocol header_metadata_t fits in the packet metadata, pami_task_t  fits in the message metadata\n"));

                //Post rts_data package
                adaptive->_rts_data_model.postPacket (send->pkt,
                                                      send->cb_rts,
                                                      (void *) send,
                                                      send->rts.destRank,
                                                      (void *) &send->header,
                                                      sizeof (header_metadata_t),
                                                      (void *)((char *)send->msginfo + send->header.offset),
                                                      (size_t)send->header.bsend);

              }
            else
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::Callback_rts_send .. protocol header_metadata_t does not fit in the packet metadata\n"));


                TRACE_ERR((stderr, "   AdaptiveSimple::Callback_rts_send ..  protocol header_metadata_t does not fit in the packet metadata, pami_task_t  fits in the message metadata\n"));

                //Post rts_data package
                send->iov[0].iov_base = (void *) & send->header;
                send->iov[0].iov_len  = sizeof (header_metadata_t);
                send->iov[1].iov_base = (void *)((char *)send->msginfo + send->header.offset);
                send->iov[1].iov_len  = (size_t)send->header.bsend;
                adaptive->_rts_data_model.postPacket (send->pkt,
                                                      send->cb_rts,
                                                      (void *) send ,
                                                      send->rts.destRank,
                                                      (void *) NULL,
                                                      0,
                                                      send->iov);

              }

            TRACE_ERR((stderr, "<<   AdaptiveSimple::Callback_rts_send() .. posted rts_data packet\n"));

            return;
          }





          ///
          /// \brief function to send window
          ///

          static int send_window (window_t * window,  size_t iolen)
          {

            //struct iovec v1[1];
            //struct iovec v2[2];
            size_t i = 0;


            for ( i = 0; i < WINDOW - 1 ; i++)
              {
                //sleep(1);

                //update data
                if (sizeof(header_metadata_t) <= T_Model::packet_model_metadata_bytes)
                  {
                    window->va_send->next_offset += T_Model::packet_model_payload_bytes;                 ///Update next Offset
                    window->va_send->header.bsend = T_Model::packet_model_payload_bytes;                ///Update bytes to window->va_send
                  }
                else
                  {
                    window->va_send->next_offset += (T_Model::packet_model_payload_bytes - sizeof(header_metadata_t));               ///Update next Offset
                    window->va_send->header.bsend = (T_Model::packet_model_payload_bytes - sizeof(header_metadata_t));              ///Update bytes to window->va_send
                  }

                //TRACE_ERR((stderr,"   AdaptiveSimple::send_window() .. cookie= %p , window->va_send->cts->va_window->va_send = %p ,window->va_send->cb_data = %p\n" ,cookie,window->va_send->rts.va_window->va_send, window->va_send->cb_data));

                //Update header
                window->va_send->header.offset = window->va_send->next_offset;

                //update package header
                window->pkg[i].header.va_recv = window->va_send->header.va_recv;
                window->pkg[i].header.offset = window->va_send->header.offset;
                window->pkg[i].header.bsend = window->va_send->header.bsend;

                //Check if data to send
                if (window->va_send->sendlen < window->va_send->next_offset)
                  {
                    TRACE_ERR((stderr, "     AdaptiveSimple::send_window() .. all data was sent\n"));

                    if (window->va_send->remote_fn == NULL)
                      {

                        send_complete(PAMI_Client_getcontext(window->va_send->adaptive->_client, window->va_send->adaptive->_contextid), (void *) window->va_send, PAMI_SUCCESS); //Terminate

                      }

                    return 0;
                  }



                if (iolen == 2)
                  {
                    window->pkg[i].iov[0].iov_base = (void *) & window->pkg[i].header;
                    window->pkg[i].iov[0].iov_len  = sizeof (header_metadata_t);
                    window->pkg[i].iov[1].iov_base = (void *)(window->va_send->send_buffer + window->pkg[i].header.offset);
                    window->pkg[i].iov[1].iov_len  = (size_t)window->pkg[i].header.bsend;
                  }
                else
                  {
                    //v1[0].iov_base = (void *)(window->va_send->send_buffer + window->pkg[i].header.offset);
                    //v1[0].iov_len  = (size_t)window->pkg[i].header.bsend;
                  }



                TRACE_ERR((stderr, "   AdaptiveSimple::send_window() .. window pkt = %p , pktno=%d\n", &window->pkg[i].pkt, i));



                if (iolen == 1)
                  {
                    TRACE_ERR((stderr, "   AdaptiveSimple::send_window() .. payload = %p, length=%d\n", (void *)(window->va_send->send_buffer + window->pkg[i].header.offset), (size_t)window->pkg[i].header.bsend));

                    window->va_send->adaptive->_data_model.postPacket (window->pkg[i].pkt,
                                                                       NULL ,
                                                                       (void *)NULL,
                                                                       window->va_send->rts.destRank,
                                                                       (void *) &window->pkg[i].header,
                                                                       sizeof (header_metadata_t),
                                                                       (void *)(window->va_send->send_buffer + window->pkg[i].header.offset),
                                                                       (size_t)window->pkg[i].header.bsend);


                  }
                else if (iolen == 2)
                  {
                    TRACE_ERR((stderr, "   AdaptiveSimple::send_window() .. window->pkg[%zu].iov[1].iov_base = %p ,window->pkg[%zu].iov[1].iov_len=%d\n", i, window->pkg[i].iov[1].iov_base, i, window->pkg[i].iov[1].iov_len));

                    window->va_send->adaptive->_data_model.postPacket (window->pkg[i].pkt,
                                                                       NULL,
                                                                       (void *)NULL,
                                                                       window->va_send->rts.destRank,
                                                                       (void *)NULL,
                                                                       0,
                                                                       window->pkg[i].iov);
                  }

                TRACE_ERR((stderr, "   AdaptiveSimple::send_window() pkt(%d).. data total bytes= %d  ,window = %p , window->va_send->next_offset = %d , offset=%d , bsend=%d \n", i, window->va_send->sendlen,  window, window->va_send->next_offset, window->pkg[i].header.offset, window->pkg[i].header.bsend));

              }

            // update data
            if (sizeof(header_metadata_t) <= T_Model::packet_model_metadata_bytes)
              {
                window->va_send->next_offset += T_Model::packet_model_payload_bytes;                 ///Update next Offset
                window->va_send->header.bsend = T_Model::packet_model_payload_bytes;                ///Update bytes to window->va_send
              }
            else
              {
                window->va_send->next_offset += (T_Model::packet_model_payload_bytes - sizeof(header_metadata_t));               ///Update next Offset
                window->va_send->header.bsend = (T_Model::packet_model_payload_bytes - sizeof(header_metadata_t));              ///Update bytes to window->va_send
              }




            TRACE_ERR((stderr, "   AdaptiveSimple::send_window() .. data total bytes= %d  ,window = %p , window->va_send->next_offset = %d , offset=%d , bsend=%d \n", window->va_send->sendlen,  window, window->va_send->next_offset, window->va_send->header.offset, window->va_send->header.bsend));
            //TRACE_ERR((stderr,"   AdaptiveSimple::send_window() .. cookie= %p , window->va_send->cts->va_window->va_send = %p ,window->va_send->cb_data = %p\n" ,cookie,window->va_send->rts.va_window->va_send, window->va_send->cb_data));

            window->va_send->header.offset = window->va_send->next_offset;

            window->pkg[i].header.va_recv = window->va_send->header.va_recv;
            window->pkg[i].header.offset = window->va_send->header.offset;
            window->pkg[i].header.bsend = window->va_send->header.bsend;

            //Check if data to send
            if (window->va_send->sendlen < window->va_send->next_offset)
              {
                TRACE_ERR((stderr, "     AdaptiveSimple::send_window() .. all data was sent\n"));

                if (window->va_send->remote_fn == NULL)
                  {

                    send_complete(PAMI_Client_getcontext(window->va_send->adaptive->_client, window->va_send->adaptive->_contextid), (void *) window->va_send, PAMI_SUCCESS); //Terminate

                  }

                return 0;
              }


            if (iolen == 2)
              {
                window->pkg[i].iov[0].iov_base = (void *) & window->pkg[i].header;
                window->pkg[i].iov[0].iov_len  = sizeof (header_metadata_t);
                window->pkg[i].iov[1].iov_base = (void *)(window->va_send->send_buffer + window->pkg[i].header.offset);
                window->pkg[i].iov[1].iov_len  = (size_t)window->pkg[i].header.bsend;
              }
            else
              {
                //v1[0].iov_base = (void *)(window->va_send->send_buffer + window->pkg[i].header.offset);
                //v1[0].iov_len  = (size_t)window->pkg[i].header.bsend;
              }


            //TRACE_ERR((stderr, "   AdaptiveSimple::send_window() .. after window->va_send->fblock= %s\n", (window->va_send->fblock)?"true":"false"));


            TRACE_ERR((stderr, "   AdaptiveSimple::send_window() .. window pkt = %p , pktno=%d\n", &window->pkg[i].pkt, i));

            if (iolen == 1)
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::send_window() .. payload = %p ,length=%d\n", (void *)(window->va_send->send_buffer + window->pkg[i].header.offset), (size_t)window->pkg[i].header.bsend));
                window->va_send->adaptive->_data_model.postPacket (window->pkg[i].pkt,
                                                                   cb_data_send,
                                                                   (void *)window,
                                                                   window->va_send->rts.destRank,
                                                                   (void *) &window->pkg[i].header,
                                                                   sizeof (header_metadata_t),
                                                                   (void *)(window->va_send->send_buffer + window->pkg[i].header.offset),
                                                                   (size_t)window->pkg[i].header.bsend);

              }
            else if (iolen == 2)
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::send_window() .. window->pkg[%zu].iov[1].iov_base = %p ,window->pkg[%zu].iov[1].iov_len=%d\n", i, window->pkg[i].iov[1].iov_base, i, window->pkg[i].iov[1].iov_len));

                window->va_send->adaptive->_data_model.postPacket (window->pkg[i].pkt,
                                                                   cb_data_send,
                                                                   (void *)window,
                                                                   window->va_send->rts.destRank,
                                                                   (void *) NULL,
                                                                   0,
                                                                   window->pkg[i].iov);

              }

            return 0;
          }


          ///
          /// \brief Callback invoked after send a data_send packet.
          ///

          static void cb_data_send (pami_context_t   context,
                                    void          * cookie,
                                    pami_result_t    result)
          {


            //Pointer to send state
            window_t * window = (window_t *) cookie;

            //Pointer to Protocol
            AdaptiveSimple<T_Model, T_Device, T_LongHeader> * adaptive =
              (AdaptiveSimple<T_Model, T_Device, T_LongHeader> *) window->va_send->pf;

            TRACE_ERR((stderr, ">> AdaptiveSimple::Callback_data_send(%d)\n", ++adaptive->_counter));




            if (sizeof(header_metadata_t) <= T_Model::packet_model_metadata_bytes)
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::Callback_data_send() .. protocol header_metadata_t fits in the packet metadata\n"));


                TRACE_ERR((stderr, "   AdaptiveSimple::Callback_data_send() ..  protocol header_metadata_t fits in the packet metadata, pami_task_t  fits in the message metadata\n"));


                send_window (window, 1);





              }
            else
              {
                TRACE_ERR((stderr, "   AdaptiveSimple::Callback_data_send .. protocol header_metadata_t does not fit in the packet metadata\n"));


                TRACE_ERR((stderr, "   AdaptiveSimple::Callback_data_send ..  protocol header_metadata_t does not fit in the packet metadata, pami_task_t  fits in the message metadata\n"));

                //Post data package

                send_window (window, 2);




              }

            TRACE_ERR((stderr, "<<   AdaptiveSimple::Callback_data_send() .. posted data packet\n"));

            return;
          }





          static int dispatch_ack_direct (void   * metadata,
                                          void   * payload,
                                          size_t   bytes,
                                          void   * recv_func_parm,
                                          void   * cookie)
          {
            TRACE_ERR((stderr, ">> AdaptiveSimple::dispatch_ack_direct()\n"));
            send_state_t * send = *((send_state_t **) payload);


            pami_event_function   remote_fn = send->remote_fn;
            pami_event_function   local_fn = send->local_fn;
            void               * fn_cookie = send->cookie;

            TRACE_ERR((stderr, "   AdaptiveSimple::dispatch_ack_direct() .. send = %p, cookie =%p , remote_fn = %p\n", send, fn_cookie, remote_fn));

            AdaptiveSimple<T_Model, T_Device, T_LongHeader> * adaptive =
              (AdaptiveSimple<T_Model, T_Device, T_LongHeader> *) recv_func_parm;

            adaptive->freeSendState (send);

            if (local_fn != NULL)
              {
                local_fn (PAMI_Client_getcontext(adaptive->_client, adaptive->_contextid), fn_cookie, PAMI_SUCCESS);
              }


            if (remote_fn) remote_fn (PAMI_Client_getcontext(adaptive->_client, adaptive->_contextid), fn_cookie, PAMI_SUCCESS);

            TRACE_ERR((stderr, "<< AdaptiveSimple::dispatch_ack_direct()\n"));
            return 0;
          }


          ///
          /// \brief Local receive completion event callback.
          ///
          /// This callback will invoke the application local receive
          /// completion callback and free the receive state object
          /// memory.
          ///
          static void receive_complete (pami_context_t   context,
                                        void          * cookie,
                                        pami_result_t    result)
          {
            TRACE_ERR((stderr, "AdaptiveSimple::receive_complete() >> \n"));

            recv_state_t * rcv = (recv_state_t *) cookie;

            pami_event_function   local_fn = rcv->info.local_fn;
            void               * fn_cookie = rcv->info.cookie;

            AdaptiveSimple<T_Model, T_Device, T_LongHeader> * adaptive =
              (AdaptiveSimple<T_Model, T_Device, T_LongHeader> *) rcv->adaptive;


            adaptive->freeRecvState (rcv);

            if (local_fn)
              local_fn (PAMI_Client_getcontext(adaptive->_client, adaptive->_contextid),
                        fn_cookie,
                        PAMI_SUCCESS);





            TRACE_ERR((stderr, "AdaptiveSimple::receive_complete() << \n"));
            return;
          };



          ///
          /// \brief Local send completion event callback.
          ///
          /// This callback will invoke the application local completion
          /// callback function and, if notification of remote receive
          /// completion is not required, free the send state memory.
          ///
          static void send_complete (pami_context_t   context,
                                     void          * cookie,
                                     pami_result_t    result)
          {
            TRACE_ERR((stderr, "AdaptiveSimple::send_complete() >> \n"));
            send_state_t * send = (send_state_t *) cookie;

            AdaptiveSimple<T_Model, T_Device, T_LongHeader> * adaptive =
              (AdaptiveSimple<T_Model, T_Device, T_LongHeader> *) send->adaptive;

            pami_event_function   local_fn = send->local_fn;
            void               * fn_cookie = send->cookie;


            adaptive->freeSendState (send);

            if (local_fn != NULL)
              {
                local_fn (PAMI_Client_getcontext(adaptive->_client, adaptive->_contextid), fn_cookie, PAMI_SUCCESS);
              }




            TRACE_ERR((stderr, "AdaptiveSimple::send_complete() << \n"));
            return;
          }




          ///
          /// \brief Read-access  send rts packet callback.
          ///

          static int dispatch_rts_read (void         * metadata,
                                        void         * payload,
                                        size_t         bytes,
                                        void         * recv_func_parm,
                                        void         * cookie)
          {
#if 0
            TRACE_ERR((stderr, "(%zu) AdaptiveFactory::dispatch_rts_read() .. \n"));

            AdaptiveFactory<T_Model, T_Device, T_LongHeader> * pf =
              (AdaptiveFactory<T_Model, T_Device, T_LongHeader> *) recv_func_parm;

            // This packet device DOES NOT provide the data buffer(s) for the
            // message and the data must be read on to the stack before the
            // recv callback is invoked.

            uint8_t stackData[pf->getDevice()->getPacketPayloadSize()];
            void * p = (void *) & stackData[0];
            pf->getDevice()->readData(channel, (char *) p, bytes);

            dispatch_rts_direct (channel, metadata, p, bytes, recv_func_parm);
#else
            PAMI_abort();
#endif
            return 0;
          };

          ///
          /// \brief Read-access  send rts_ack packet callback.
          ///


          static int dispatch_rts_ack_read (void         * metadata,
                                            void         * payload,
                                            size_t         bytes,
                                            void         * recv_func_parm,
                                            void         * cookie)
          {
#if 0
            TRACE_ERR((stderr, "(%zu) AdaptiveFactory::dispatch_rts_ack_read() .. \n"));

            AdaptiveFactory<T_Model, T_Device, T_LongHeader> * pf =
              (AdaptiveFactory<T_Model, T_Device, T_LongHeader> *) recv_func_parm;

            // This packet device DOES NOT provide the data buffer(s) for the
            // message and the data must be read on to the stack before the
            // recv callback is invoked.

            uint8_t stackData[pf->getDevice()->getPacketPayloadSize()];
            void * p = (void *) & stackData[0];
            pf->getDevice()->readData(channel, (char *) p, bytes);

            dispatch_rts_ack_direct (channel, metadata, p, bytes, recv_func_parm);
#else
            PAMI_abort();
#endif
            return 0;
          };



          ///
          /// \brief Read-access  send rts_data packet callback.
          ///

          static int dispatch_rts_data_read (void         * metadata,
                                             void         * payload,
                                             size_t         bytes,
                                             void         * recv_func_parm,
                                             void         * cookie)
          {
#if 0
            TRACE_ERR((stderr, "(%zu) AdaptiveFactory::dispatch_rts_data_read() .. \n"));

            AdaptiveFactory<T_Model, T_Device, T_LongHeader> * pf =
              (AdaptiveFactory<T_Model, T_Device, T_LongHeader> *) recv_func_parm;

            // This packet device DOES NOT provide the data buffer(s) for the
            // message and the data must be read on to the stack before the
            // recv callback is invoked.

            uint8_t stackData[pf->getDevice()->getPacketPayloadSize()];
            void * p = (void *) & stackData[0];
            pf->getDevice()->readData(channel, (char *) p, bytes);

            dispatch_rts_data_direct (channel, metadata, p, bytes, recv_func_parm);
#else
            PAMI_abort();
#endif
            return 0;
          };

          ///
          /// \brief Read-access  send ack packet callback.
          ///
          static int dispatch_ack_read (void   * metadata,
                                        void   * payload,
                                        size_t   bytes,
                                        void   * recv_func_parm,
                                        void   * cookie)
          {
            TRACE_ERR((stderr, ">> AdaptiveSimple::dispatch_ack_read()\n"));

            AdaptiveSimple<T_Model, T_Device, T_LongHeader> * pf =
              (AdaptiveSimple<T_Model, T_Device, T_LongHeader> *) recv_func_parm;

            // This packet device DOES NOT provide the data buffer(s) for the
            // message and the data must be read on to the stack before the
            // recv callback is invoked.
            PAMI_assert_debugf(payload == NULL, "The 'read only' packet device did not invoke dispatch with payload == NULL (%p)\n", payload);

            uint8_t stackData[T_Model::packet_model_payload_bytes];
            void * p = (void *) & stackData[0];
            pf->_device.read (p, bytes, cookie);

            dispatch_ack_direct (metadata, p, bytes, recv_func_parm, cookie);

            TRACE_ERR((stderr, "<< AdaptiveSimple::dispatch_ack_read()\n"));
            return 0;
          }






          ///
          /// \brief Read-access  send data packet callback.
          ///

          static int dispatch_data_read     (void         * metadata,
                                             void         * payload,
                                             size_t         bytes,
                                             void         * recv_func_parm,
                                             void         * cookie)
          {
#if 0
            TRACE_ERR((stderr, "AdaptiveFactory::dispatch_data_read() .. \n"));

            AdaptiveFactory<T_Model, T_Device, T_LongHeader> * pf =
              (AdaptiveFactory<T_Model, T_Device, T_LongHeader> *) recv_func_parm;

            // This packet device DOES NOT provide the data buffer(s) for the
            // message and the data must be read on to the stack before the
            // recv callback is invoked.

            uint8_t stackData[pf->getDevice()->getPacketPayloadSize()];
            void * p = (void *) & stackData[0];
            pf->getDevice()->readData(channel, (char *) p, bytes);

            dispatch_data_read (channel, metadata, p, bytes, recv_func_parm);
#else
            PAMI_abort();
#endif
            return 0;
          };



          ///
          /// \brief Read-access  send cts packet callback.
          ///

          static int dispatch_cts_read (void         * metadata,
                                        void         * payload,
                                        size_t         bytes,
                                        void         * recv_func_parm,
                                        void         * cookie)
          {
#if 0
            TRACE_ERR((stderr, "AdaptiveFactory::dispatch_cts_read() .. \n"));
            send_state_t * state = (send_state_t *) metadata;

            pami_event_function * local_fn  = state->local_fn;
            pami_event_function * remote_fn = state->remote_fn;
            void               * cookie    = state->cookie;

            Adaptive<T_Model, T_Device, T_LongHeader> * adaptive =
              (Adaptive<T_Model, T_Device, T_LongHeader> *) recv_func_parm;
            adaptive->freeSendState (state);

            if (local_fn)  local_fn  (0, cookie, PAMI_SUCCESS);

            if (remote_fn) remote_fn (0, cookie, PAMI_SUCCESS);

#endif
            return 0;
          }


      };
      template <class T_Model, class T_Device, bool T_LongHeader >
      Queue * AdaptiveSimple< T_Model,  T_Device, T_LongHeader>::_queue = NULL;
    };
  };
};
#undef TRACE_ERR
#endif // __pami_p2p_protocol_send_adaptive_adaptivesimple_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
