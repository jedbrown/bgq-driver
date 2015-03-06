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
/// \file p2p/protocols/send/adaptive/AdaptiveImmediate.h
/// \brief Immediate adaptive send protocol for reliable devices.
///
/// The AdaptiveImmediate class defined in this file uses C++ templates and
/// the device "message" interface - which also uses C++ templates.
///
/// C++ templates require all source code to be #include'd from a header file.
///
#ifndef __p2p_protocols_send_adaptive_AdaptiveImmediate_h__
#define __p2p_protocols_send_adaptive_AdaptiveImmediate_h__

#include "components/memory/MemoryAllocator.h"

#include "util/common.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf x
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
      ///
      /// \see PAMI::Device::Interface::PacketModel
      /// \see PAMI::Device::Interface::PacketDevice
      ///
      template <class T_Model, class T_Device>
      class AdaptiveImmediate
      {
        protected:

          typedef uint8_t pkt_t[T_Model::packet_model_state_bytes];

          ///
          /// \brief Sender-side state structure for immediate sends.
          ///
          /// If the immediate post to the device fails due to unavailable
          /// network resources, memory is allocated for this structure to
          /// pack the source metadata and source data into a single
          /// contiguous buffer which can be posted to the device as a
          /// non-blocking transfer.
          ///
          typedef struct send
          {
            uint8_t                    data[T_Model::packet_model_payload_bytes]; ///< Packed data
            pkt_t                      pkt;
            AdaptiveImmediate < T_Model,
            T_Device > * pf; ///< Adaptive immediate protocol
          } send_t;

          ///
          /// \brief Protocol metadata structure for immediate sends
          ///
          typedef struct __attribute__((__packed__)) protocol_metadata
          {
            pami_task_t     fromRank;  ///< Origin task id
            uint16_t       databytes; ///< Number of bytes of data
            uint16_t       metabytes; ///< Number of bytes of metadata
        } protocol_metadata_t;

        public:

          ///
          /// \brief Adaptive immediate send protocol constructor.
          ///
          /// \param[in]  dispatch     Dispatch identifier
          /// \param[in]  dispatch_fn  Dispatch callback function
          /// \param[in]  cookie       Opaque application dispatch data
          /// \param[in]  device       Device that implements the message interface
          /// \param[in]  origin_task  Origin task identifier
          /// \param[in]  context      Communication context
          /// \param[out] status       Constructor status
          ///
          inline AdaptiveImmediate (size_t                     dispatch,
                                    pami_dispatch_callback_function dispatch_fn,
                                    void                     * cookie,
                                    T_Device                 & device,
                                    pami_task_t                 origin_task,
                                    pami_client_t              client,
                                    size_t                     contextid,
                                    pami_result_t             & status) :
              _send_model (device, client, contextid),
              _fromRank (origin_task),
              _client (client),
              _contextid (contextid),
              _dispatch_fn (dispatch_fn),
              _cookie (cookie),
              _device (device)
          {
            // ----------------------------------------------------------------
            // Compile-time assertions
            // ----------------------------------------------------------------

            // This protocol only works with reliable networks.
            //COMPILE_TIME_ASSERT(T_Model::reliable_packet_model == true);


            // ----------------------------------------------------------------
            // Compile-time assertions
            // ----------------------------------------------------------------

            TRACE_ERR((stderr, "AdaptiveImmediate() [0]\n"));
            status = _send_model.init (dispatch,
                                       dispatch_send_direct, this,
                                       dispatch_send_read, this);
            TRACE_ERR((stderr, "AdaptiveImmediate() [1] status = %d\n", status));
          }

          ///
          /// \brief Start a new simple send adaptive operation.
          ///
          /// \see PAMI::Protocol::Send::immediate
          ///
          inline pami_result_t immediate_impl (pami_send_immediate_t * parameters)
          {
            TRACE_ERR((stderr, "AdaptiveImmediate::immediate_impl() >>\n"));

            // Specify the protocol metadata to send with the application
            // metadata in the packet. This metadata is copied
            // into the network by the device and, therefore, can be placed
            // on the stack.
            protocol_metadata_t metadata;
            metadata.fromRank  = _fromRank;
            metadata.databytes = parameters->data.iov_len;
            metadata.metabytes = parameters->header.iov_len;

            pami_task_t task;
            size_t offset;
            PAMI_ENDPOINT_INFO(parameters->dest,task,offset);

            TRACE_ERR((stderr, "AdaptiveImmediate::immediate_impl() .. before _send_model.postPacket() .. bytes = %zu\n", bytes));
            bool posted =
              _send_model.postPacket (task, offset,
                                      (void *) & metadata,
                                      sizeof (protocol_metadata_t),
                                      parameters->iov);

            if (unlikely(!posted))
              {
                // For some reason the packet could not be immediately posted.
                // Allocate memory, pack the user data and metadata, and attempt
                // a regular (non-blocking) post.
                send_t * send = (send_t *) _allocator.allocateObject ();
                send->pf = this;
                memcpy (&(send->data[0]), parameters->header.iov_base, parameters->header.iov_len);
                memcpy (&(send->data[parameters->header.iov_len]), parameters->data.iov_base, parameters->data.iov_len);

                struct iovec v[1];
                v[0].iov_base = (void *) & (send->data[0]);
                v[0].iov_len  = metadata.databytes + metadata.metabytes;
                _send_model.postPacket (send->pkt,
                                        send_complete,
                                        send,
                                        task, offset,
                                        (void *) &metadata,
                                        sizeof (protocol_metadata_t),
                                        v);
              }

            TRACE_ERR((stderr, "AdaptiveImmediate::immediate_impl() <<\n"));
            return PAMI_SUCCESS;
          };

        protected:

          inline void freeSendState (send_t * object)
          {
            _allocator.returnObject ((void *) object);
          }

          MemoryAllocator < sizeof(send_t), 16 > _allocator;

          T_Model         _send_model;
          pami_task_t      _fromRank;

          pami_client_t              _client;
          size_t                     _contextid;
          pami_dispatch_callback_function _dispatch_fn;
          void                     * _cookie;
          T_Device                 & _device;

          ///
          /// \brief Direct multi-packet send envelope packet dispatch.
          ///
          /// The adaptive simple send protocol will register this dispatch
          /// function if and only if the device \b does provide direct access
          /// to data which has already been read from the network by the
          /// device.
          ///
          /// The envelope dispatch function is invoked by the message device
          /// to process the first packet of a multi-packet message. The adaptive
          /// simple send protocol transfers protocol metadata and application
          /// metadata in a single packet. Application data will arrive in
          /// subsequent adaptive simple send data packets and will be processed
          /// by the data dispatch function.
          ///
          /// \see PAMI::Device::Interface::RecvFunction_t
          ///
          static int dispatch_send_direct (void   * metadata,
                                           void   * payload,
                                           size_t   bytes,
                                           void   * recv_func_parm,
                                           void   * cookie)
          {
            protocol_metadata_t * m = (protocol_metadata_t *) metadata;

            TRACE_ERR ((stderr, ">> AdaptiveImmediate::dispatch_send_direct(), m->fromRank = %d, m->databytes = %d, m->metabytes = %d\n", m->fromRank, m->databytes, m->metabytes));

            AdaptiveImmediate<T_Model, T_Device> * send =
              (AdaptiveImmediate<T_Model, T_Device> *) recv_func_parm;

            uint8_t * data = (uint8_t *)payload;
            pami_recv_t recv; // used only to provide a non-null recv object to the dispatch function.

            // Invoke the registered dispatch function.
            send->_dispatch_fn.p2p (send->_client,   // Communication context handle
                                    send->_contextid, // Communication context id
                                    send->_cookie,    // Dispatch cookie
                                    m->fromRank,      // Origin (sender) rank
                                    (void *) data,    // Application metadata
                                    m->metabytes,     // Metadata bytes
                                    (void *) (data + m->metabytes),  // payload data
                                    m->databytes,     // Total number of bytes
                                    (pami_recv_t *) &recv);

            TRACE_ERR ((stderr, "<< AdaptiveImmediate::dispatch_send_direct()\n"));
            return 0;
          };

          ///
          /// \brief Read-access multi-packet send envelope packet callback.
          ///
          /// Reads incoming bytes from the device to a temporary buffer and
          /// invokes the registered receive callback.
          ///
          /// The adaptive packet factory will register this dispatch function
          /// if and only if the device \b does \b not provide access to
          /// data which has already been read from the network by the device.
          ///
          /// The dispatch function must read the data onto the stack and
          /// then provide 16-byte (CMQuad) aligned data pointer(s) to the
          /// registered user callback.
          ///
          /// The envelope dispatch function is invoked by the packet device
          /// to process the first packet of a multi-packet message. The CDI
          /// adaptive protocol sends two iovec elements in the first packet for
          /// the number of data bytes to follow and the msginfo metadata.
          ///
          /// \note The iov_base fields in the iovec structure will be \c NULL
          ///
          /// \see dispatch_data_read
          ///
          static int dispatch_send_read (void   * metadata,
                                         void   * payload,
                                         size_t   bytes,
                                         void   * recv_func_parm,
                                         void   * cookie)
          {
            TRACE_ERR((stderr, ">> AdaptiveImmediate::dispatch_send_read()\n"));

            AdaptiveImmediate<T_Model, T_Device> * pf =
              (AdaptiveImmediate<T_Model, T_Device> *) recv_func_parm;

            // This packet device DOES NOT provide the data buffer(s) for the
            // message and the data must be read on to the stack before the
            // recv callback is invoked.
            PAMI_assert_debugf(payload == NULL, "The 'read only' packet device did not invoke dispatch with payload == NULL (%p)\n", payload);

            uint8_t stackData[T_Model::packet_model_payload_bytes];
            void * p = (void *) & stackData[0];
            pf->_device.read (p, bytes, cookie);

            dispatch_send_direct (metadata, p, bytes, recv_func_parm, cookie);

            TRACE_ERR((stderr, "<< AdaptiveImmediate::dispatch_send_read()\n"));
            return 0;
          };

          ///
          /// \brief Local send completion event callback.
          ///
          /// This callback will free the send state memory.
          ///
          static void send_complete (pami_context_t   context,
                                     void          * cookie,
                                     pami_result_t    result)
          {
            TRACE_ERR((stderr, "AdaptiveImmediate::send_complete() >> \n"));
            send_t * state = (send_t *) cookie;

            AdaptiveImmediate<T_Model, T_Device> * pf =
              (AdaptiveImmediate<T_Model, T_Device> *) state->pf;

            pf->freeSendState (state);

            TRACE_ERR((stderr, "AdaptiveImmediate::send_complete() << \n"));
            return;
          }
      };
    };
  };
};
#undef TRACE_ERR
#endif // __pami_p2p_protocol_send_adaptive_adaptiveimmediate_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
