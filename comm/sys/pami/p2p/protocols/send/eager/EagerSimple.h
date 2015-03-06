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
/// \file p2p/protocols/send/eager/EagerSimple.h
/// \brief Simple eager send protocol for reliable devices.
///
/// The EagerSimple class defined in this file uses C++ templates and
/// the device "message" interface - which also uses C++ templates.
///
/// C++ templates require all source code to be #include'd from a header file.
///
/// Definitons:
///
/// \b 'metadata' is the additional information added to the packet as required
///    by the eager send protocol.
///
/// \b 'header' is the additional application infomration sent with the message.
///
/// \b 'data' is the actual application data to be sent as a message.
///
/// \b 'immediate' is a type of transfer where the metadata, header, and data
///     are "immediately" injected as a single packet.
///
/// \b 'packed' is a type of transfer where the metadata, header, and data are
///    copied and transfered in a single packet.
///
/// \b 'eager' is a type of transfer where the header and data are transfered in
///    one or more separate packets.
///
/// \b 'eager envelope' is the portion of an eager transfer that sends and
///    receives the header information.
///
/// \b 'eager data' is the portion of an eager transfer that sends and receives
///    the source data information.
///
///
#ifndef __p2p_protocols_send_eager_EagerSimple_h__
#define __p2p_protocols_send_eager_EagerSimple_h__

#include <limits.h>

#include "components/devices/PacketInterface.h"
#include "components/memory/MemoryAllocator.h"
#include "math/Memcpy.x.h"
#include "p2p/protocols/Send.h"

#include "common/type/TypeMachine.h"

#ifndef RETURN_ERR_PAMI
#define RETURN_ERR_PAMI(code, ...) return (code)
#endif

#include "util/trace.h"
#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  namespace Protocol
  {
    namespace Send
    {

      ///
      /// \brief Eager simple send protocol class for reliable network devices.
      ///
      /// \tparam T_Model      Packet model class
      /// \tparam T_Option     Send options
      ///
      /// \see PAMI::Device::Interface::PacketModel
      ///
      template < class T_Model, configuration_t T_Option >
      class EagerSimple
      {
        protected:

          typedef EagerSimple<T_Model, T_Option> EagerSimpleProtocol;

          typedef uint8_t model_state_t[T_Model::packet_model_state_bytes];
          typedef uint8_t model_packet_t[T_Model::packet_model_payload_bytes];



          ///
          /// \note The 'short' protocol metadata limits the maximum size of
          ///       the application header + application data to \c USHRT_MAX
          ///       bytes. The effective maximum application header + application
          ///       data size for the short protocol is the minimum of \c USHRT_MAX and
          ///       \c T_Model::packet_model_payload_bytes less the size of the
          ///       \c packed_metadata_t structure \em if the size of the
          ///       \c packed_metadata_t structure is greater than
          ///       \c T_Model::packet_model_metadata_bytes
          ///
          typedef struct
          {
            uint16_t                data_bytes;
            uint16_t                header_bytes;
            pami_endpoint_t         origin;
          } packed_metadata_t;

          typedef struct
          {
            model_state_t           state;
            model_packet_t          packet;
            packed_metadata_t       metadata;
          } immediate_t;

          typedef struct
          {
            model_state_t           state;
            model_packet_t          packet;
            packed_metadata_t       metadata;
            struct iovec            v[3];
          } packed_t;




          ///
          /// \note The 'eager' protocol metadata structure limits the maximum
          ///       size of the application header to \c USHRT_MAX bytes. The
          ///       effective maximum application header size for the eager
          ///       protocol is the minimum of \c USHRT_MAX and
          ///       \c T_Model::packet_model_payload_bytes less the size of the
          ///       \c shortheader_metadata_t metadata structure \em if the size of the
          ///       \c shortheader_metadata_t metadata structure is greater than
          ///       \c T_Model::packet_model_metadata_bytes
          ///
          typedef struct
          {
            size_t                  bytes;
            uint16_t                metabytes;
            pami_endpoint_t         origin;
          } shortheader_metadata_t;

          typedef struct
          {
            model_state_t           state;
            shortheader_metadata_t  metadata;
            struct iovec            v[2];
          } short_header_t;



          typedef struct
          {
            size_t                  bytes;
            size_t                  metabytes;
            pami_endpoint_t         origin;
          } longheader_metadata_t;

          typedef struct
          {
            model_state_t           state[2];
            longheader_metadata_t   metadata;
          } long_header_t;



          typedef model_packet_t data_pipeline_t;

          typedef struct
          {
            model_state_t           state[2];
            data_pipeline_t         pipeline[2];
            uint8_t                 machine[sizeof(Type::TypeMachine)];

            void                  * base_addr;
            size_t                  bytes_remaining;
            size_t                  start_count;
          } data_t;



          typedef struct
          {
            pami_event_function     remote_fn;
            void                  * cookie;
            pami_context_t          context;
            bool                    invoke;
          } ack_metadata_t;

          typedef struct
          {
            model_state_t           state;
            ack_metadata_t          metadata;
          } ack_t;


          typedef struct
          {
            union
            {
              short_header_t        single;
              long_header_t         multi;
            } envelope;
            data_t                  data;
          } eager_t;

          typedef struct
          {
            union
            {
              immediate_t           immediate;
              packed_t              packed;
              eager_t               eager;
              ack_t                 ack;
            };

            pami_task_t             target_task;
            size_t                  target_offset;
            pami_event_function     local_fn;
            pami_event_function     remote_fn;
            void                  * cookie;
            EagerSimpleProtocol   * protocol;
          } origin_t;

          typedef struct
          {
            struct
            {
              uint8_t               machine[sizeof(Type::TypeMachine)];
              bool                  is_contiguous_copy_recv;
              size_t                bytes_received;
              size_t                bytes_total;
            } data;

            struct
            {
              size_t                bytes_received;
              size_t                bytes_total;
              void                * addr;
            } header;

            pami_recv_t             info;
            EagerSimpleProtocol   * eager;
          } target_t;

          typedef union
          {
            origin_t origin;
            target_t target;
          } eager_state_t;

          static const size_t maximum_short_packet_payload =
            T_Model::packet_model_payload_bytes -
            (sizeof(packed_metadata_t) > T_Model::packet_model_metadata_bytes) * sizeof(packed_metadata_t);

          static const size_t maximum_eager_packet_payload =
            T_Model::packet_model_payload_bytes -
            (sizeof(shortheader_metadata_t) > T_Model::packet_model_metadata_bytes) * sizeof(shortheader_metadata_t);

        public:

          static const size_t recv_immediate_max = MIN(maximum_short_packet_payload, maximum_eager_packet_payload);

          ///
          /// \brief Eager simple send protocol constructor.
          ///
          /// \param[in]  dispatch     Dispatch identifier
          /// \param[in]  dispatch_fn  Point-to-point dispatch callback function
          /// \param[in]  cookie       Opaque application dispatch data
          /// \param[in]  device       Device that implements the message interface
          /// \param[in]  origin       Origin endpoint
          /// \param[in]  context      Origin communcation context
          /// \param[out] status       Constructor status
          ///
          template <class T_Device>
          inline EagerSimple (T_Device & device) :
              _envelope_model (device),
              _longheader_envelope_model (device),
              _longheader_message_model (device),
              _data_model (device),
              _ack_model (device),
              _short_model (device)
          {
            // ----------------------------------------------------------------
            // Compile-time assertions
            // ----------------------------------------------------------------

            // This protocol only works with reliable networks.
            COMPILE_TIME_ASSERT(T_Model::reliable_packet_model == true);

            // This protocol only works with deterministic models.
            COMPILE_TIME_ASSERT(T_Model::deterministic_packet_model == true);

            // Assert that the size of the packet metadata area is large
            // enough to transfer the eager match information. This is used in the
            // various postMultiPacket() calls to transfer long header and data
            // messages.
            COMPILE_TIME_ASSERT(sizeof(pami_endpoint_t) <= T_Model::packet_model_multi_metadata_bytes);

            // Assert that the size of the packet payload area is large
            // enough to transfer a single virtual address. This is used in
            // the postPacket() calls to transfer the ack information.
            COMPILE_TIME_ASSERT(sizeof(void *) <= T_Model::packet_model_payload_bytes);

            // Assert that the size of the packet payload area not larger than
            // the maximum number of bytes that can be sent in a single packet
            // transfer. This is a restriction based on the data type used for
            // the packed_metadata_t::header_bytes field.
            COMPILE_TIME_ASSERT(T_Model::packet_model_payload_bytes <= USHRT_MAX);

            // ----------------------------------------------------------------
            // Compile-time assertions (end)
            // ----------------------------------------------------------------
          }


          pami_result_t initialize (size_t                       dispatch,
                                    pami_dispatch_p2p_function   dispatch_fn,
                                    void                       * cookie,
                                    pami_endpoint_t              origin,
                                    pami_context_t               context,
                                    pami_dispatch_hint_t         hint)
          {
            TRACE_FN_ENTER();

            _dispatch_fn = dispatch_fn;
            _cookie      = cookie;
            _origin      = origin;
            _context     = context;

            pami_result_t status = PAMI_ERROR;

            // The models must be registered in reverse order of use in case
            // the remote side is delayed in it's registrations and must save
            // unexpected packets until dispatch registration.
            TRACE_FORMAT("register ack model dispatch %zu", dispatch);

            status = _ack_model.init (dispatch, dispatch_remotefn, this);
            TRACE_FORMAT("ack model status = %d", status);

            if (status == PAMI_SUCCESS)
              {
                TRACE_FORMAT("register short model dispatch %zu", dispatch);
                status = _short_model.init (dispatch, dispatch_packed, this);
                TRACE_FORMAT("short model status = %d", status);

                if (status == PAMI_SUCCESS)
                  {
                    TRACE_FORMAT("register data model dispatch %zu", dispatch);

                    if (hint.recv_contiguous == PAMI_HINT_ENABLE &&
                        hint.recv_copy       == PAMI_HINT_ENABLE)
                      {
                        // Only allow contiguous copy receives
                        status = _data_model.init (dispatch, dispatch_data<true>, this);
                      }
                    else
                      {
                        // Allow all typed receives
                        status = _data_model.init (dispatch, dispatch_data<false>, this);
                      }

                    TRACE_FORMAT("data model status = %d", status);

                    if (status == PAMI_SUCCESS)
                      {
                        TRACE_FORMAT("register envelope model dispatch %zu", dispatch);

                        if (hint.recv_contiguous == PAMI_HINT_ENABLE)
                          {
                            if (hint.recv_copy == PAMI_HINT_ENABLE)
                              {
                                status = _envelope_model.init (dispatch, dispatch_envelope_packed<true, true>, this);
                              }
                            else
                              {
                                status = _envelope_model.init (dispatch, dispatch_envelope_packed<true, false>, this);
                              }
                          }
                        else
                          {
                            if (hint.recv_copy == PAMI_HINT_ENABLE)
                              {
                                status = _envelope_model.init (dispatch, dispatch_envelope_packed<false, true>, this);
                              }
                            else
                              {
                                status = _envelope_model.init (dispatch, dispatch_envelope_packed<false, false>, this);
                              }
                          }

                        TRACE_FORMAT("envelope model status = %d", status);

                        if (status == PAMI_SUCCESS && !(T_Option & LONG_HEADER_DISABLE))
                          {
                            TRACE_FORMAT("register 'long header' envelope model dispatch %zu", dispatch);
                            status = _longheader_envelope_model.init (dispatch, dispatch_envelope_metadata, this);
                            TRACE_FORMAT("'long header' envelope model status = %d", status);

                            if (status == PAMI_SUCCESS)
                              {

                                TRACE_FORMAT("register 'long header' message model dispatch %zu", dispatch);

                                if (hint.recv_contiguous == PAMI_HINT_ENABLE)
                                  {
                                    if (hint.recv_copy == PAMI_HINT_ENABLE)
                                      {
                                        status = _longheader_message_model.init (dispatch, dispatch_envelope_header<true, true>, this);
                                      }
                                    else
                                      {
                                        status = _longheader_message_model.init (dispatch, dispatch_envelope_header<true, false>, this);
                                      }
                                  }
                                else
                                  {
                                    if (hint.recv_copy == PAMI_HINT_ENABLE)
                                      {
                                        status = _longheader_message_model.init (dispatch, dispatch_envelope_header<false, true>, this);
                                      }
                                    else
                                      {
                                        status = _longheader_message_model.init (dispatch, dispatch_envelope_header<false, false>, this);
                                      }
                                  }

                                TRACE_FORMAT("'long header' message model status = %d", status);
                              }
                          }
                      }
                  }
              }

            TRACE_FN_EXIT();
            return status;
          }

          inline pami_result_t immediate_impl (pami_send_immediate_t * parameters)
          {
            TRACE_FN_ENTER();

#ifdef ERROR_CHECKS

            if (T_Model::packet_model_immediate_bytes <
                (parameters->data.iov_len + parameters->header.iov_len))
              {
                TRACE_FORMAT("Oops! ... %zu < (%zu+%zu) ???", T_Model::packet_model_immediate_bytes, parameters->data.iov_len, parameters->header.iov_len);
                TRACE_FN_EXIT();
                return PAMI_INVAL;
              }

#endif

            pami_task_t task;
            size_t offset;
            PAMI_ENDPOINT_INFO(parameters->dest, task, offset);

            // Verify that this task is addressable by this packet device
            if (unlikely(_short_model.device.isPeer(task) == false))
              {
                TRACE_FN_EXIT();
                return PAMI_INVAL;
              }

            TRACE_FORMAT("before _send_model.postPacket() .. parameters->header.iov_len = %zu, parameters->data.iov_len = %zu dest:%x", parameters->header.iov_len, parameters->data.iov_len, parameters->dest);

            TRACE_FORMAT("before _send_model.postPacket() .. task = %d, offset = %zu", task, offset);

            const size_t header_bytes = parameters->header.iov_len;
            const size_t data_bytes   = parameters->data.iov_len;

            bool posted =
              send_packed (task, offset, header_bytes, data_bytes, parameters);

            if ((T_Option & QUEUE_IMMEDIATE_DISABLE) && !posted)
              {
                TRACE_FN_EXIT();
                return PAMI_EAGAIN;
              }

            if (unlikely(!posted))
              {
                // For some reason the packet could not be immediately posted.
                // Allocate memory, pack the user data and metadata, and attempt
                // a regular (non-blocking) post.
                TRACE_STRING("immediate post packet unsuccessful.");

                // Allocate memory to maintain the state of the send.
                eager_state_t * state = allocateState ();

                state->origin.cookie        = NULL;
                state->origin.local_fn      = NULL;
                state->origin.remote_fn     = NULL;
                state->origin.target_task   = task;
                state->origin.target_offset = offset;
                state->origin.protocol      = this;

                // Specify the protocol metadata to send with the application
                // metadata in the envelope packet.
                //state->immediate.packed_metadata = packed_metadata;

                // This branch should be resolved at compile time and optimized out.
                if (sizeof(packed_metadata_t) <= T_Model::packet_model_metadata_bytes)
                  {
                    TRACE_STRING("'short' protocol special case, protocol metadata fits in the packet metadata");

                    // Specify the protocol metadata to send with the application
                    // metadata in the packet. This metadata is copied
                    // into the network by the device and, therefore, can be placed
                    // on the stack.
                    packed_metadata_t packed_metadata;
                    packed_metadata.data_bytes   = data_bytes;
                    packed_metadata.header_bytes = header_bytes;
                    packed_metadata.origin       = _origin;

                    uint8_t * ptr = (uint8_t *) state->origin.immediate.packet;
                    memcpy (ptr, parameters->header.iov_base, parameters->header.iov_len);
                    ptr += parameters->header.iov_len;
                    memcpy (ptr, parameters->data.iov_base, parameters->data.iov_len);

                    struct iovec iov[1];
                    iov[0].iov_base = (void *) state->origin.immediate.packet;
                    iov[0].iov_len  = parameters->header.iov_len + parameters->data.iov_len;

                    _short_model.postPacket (state->origin.immediate.state,
                                             send_complete, (void *) state,
                                             task, offset,
                                             (void *) & packed_metadata,
                                             sizeof (packed_metadata_t),
                                             iov);
                  }
                else
                  {
                    TRACE_STRING("'short' protocol special case, protocol metadata does not fit in the packet metadata");

                    packed_metadata_t * mdata = (packed_metadata_t *) state->origin.immediate.packet;
                    mdata->header_bytes = header_bytes;
                    mdata->data_bytes   = data_bytes;
                    mdata->origin       = _origin;

                    uint8_t * ptr = (uint8_t *) (mdata + 1);
                    memcpy (ptr, parameters->header.iov_base, parameters->header.iov_len);
                    ptr += parameters->header.iov_len;
                    memcpy (ptr, parameters->data.iov_base, parameters->data.iov_len);

                    struct iovec iov[1];
                    iov[0].iov_base = (void *) state->origin.immediate.packet;
                    iov[0].iov_len  = sizeof (packed_metadata_t) +
                                      parameters->header.iov_len +
                                      parameters->data.iov_len;

                    _short_model.postPacket (state->origin.immediate.state,
                                             send_complete, (void *) state,
                                             task, offset,
                                             NULL, 0,
                                             iov);
                  }
              }
            else
              {
                TRACE_STRING("immediate post packet successful.");
              }

            TRACE_FN_EXIT();
            return PAMI_SUCCESS;
          };

          inline pami_result_t simple_impl (pami_send_t * parameters)
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT("T_Model::packet_model_metadata_bytes = %zu", T_Model::packet_model_metadata_bytes);

            pami_task_t task;
            size_t offset;
            PAMI_ENDPOINT_INFO(parameters->send.dest, task, offset);

            // Verify that this task is addressable by this packet device
            if (unlikely(_short_model.device.isPeer (task) == false))
              {
                TRACE_FN_EXIT();
                return PAMI_INVAL;
              }

            const size_t header_bytes = parameters->send.header.iov_len;

            const size_t data_bytes   = parameters->send.data.iov_len;

            const size_t total_bytes  = header_bytes + data_bytes;

            // ----------------------------------------------------------------
            // Check for a "short" send protocol
            // ----------------------------------------------------------------
#ifdef ERROR_CHECKS

            if ((T_Option & RECV_IMMEDIATE_FORCEON) && (total_bytes > maximum_short_packet_payload))
              {
                // 'receive immediate' is forced ON, yet the application
                // header + data will not fit in a single packet.
                TRACE_STRING("Application error: 'recv immediate' forced on.");
                TRACE_FN_EXIT();
                return PAMI_INVAL;
              }

#endif

            //
            //   'immediate receives' are forced on ('long header' is irrelevant),
            //      OR
            //   'immediate receives' are not forced off AND the header + data fit in a single packet
            //
//fprintf (stderr, "(T_Option & RECV_IMMEDIATE_FORCEON) = %d .. (T_Option & RECV_IMMEDIATE_FORCEOFF) = %d .. !(T_Option & RECV_IMMEDIATE_FORCEOFF) = %d\n", (T_Option & RECV_IMMEDIATE_FORCEON), (T_Option & RECV_IMMEDIATE_FORCEOFF), !(T_Option & RECV_IMMEDIATE_FORCEOFF));
//fprintf (stderr, "!(T_Option & RECV_IMMEDIATE_FORCEOFF) && (%zu <= %zu) == %d\n", total_bytes, maximum_short_packet_payload, !(T_Option & RECV_IMMEDIATE_FORCEOFF) && total_bytes);
            if ((T_Option & RECV_IMMEDIATE_FORCEON) ||
                (!(T_Option & RECV_IMMEDIATE_FORCEOFF) &&
                 (total_bytes <= maximum_short_packet_payload)))
              {
                // Allocate memory to maintain the state of the send.
                eager_state_t * state = allocateState ();

                state->origin.cookie        = parameters->events.cookie;
                state->origin.local_fn      = parameters->events.local_fn;
                state->origin.remote_fn     = parameters->events.remote_fn;
                state->origin.target_task   = task;
                state->origin.target_offset = offset;
                state->origin.protocol      = this;

                pami_result_t result = send_packed (state, task, offset, (pami_send_t *) parameters);

                TRACE_FN_EXIT();
                return result;
              }

#ifdef ERROR_CHECKS

            if ((T_Option & LONG_HEADER_DISABLE) && (header_bytes > maximum_eager_packet_payload))
              {
                // 'long header' support is disabled, yet the application
                // header will not fit in a single packet.
                TRACE_STRING("Application error: 'long header' support is disabled.");
                TRACE_FN_EXIT();
                return PAMI_INVAL;
              }

#endif

            // ----------------------------------------------------------------
            // Send a single-packet envelope eager message
            // ----------------------------------------------------------------

            // Allocate memory to maintain the state of the send.
            eager_state_t * state = allocateState ();

            state->origin.cookie        = parameters->events.cookie;
            state->origin.local_fn      = parameters->events.local_fn;
            state->origin.remote_fn     = parameters->events.remote_fn;
            state->origin.target_task   = task;
            state->origin.target_offset = offset;
            state->origin.protocol      = this;

            // Specify the protocol metadata to send with the application
            // metadata in the envelope packet.
            state->origin.eager.envelope.single.metadata.bytes        = data_bytes;
            state->origin.eager.envelope.single.metadata.metabytes    = header_bytes;
            state->origin.eager.envelope.single.metadata.origin       = _origin;

            TRACE_FORMAT("parameters->send.header.iov_len = %zu, parameters->send.data.iov_len = %zu", header_bytes, data_bytes);


            if (unlikely(data_bytes == 0))
              {
                send_envelope (state, task, offset, parameters->send.header, send_complete);
              }
            else
              {
                send_envelope (state, task, offset, parameters->send.header, NULL);
                send_data<true> (state, task, offset, (pami_send_typed_t *) parameters);
              }

            TRACE_FN_EXIT();
            return PAMI_SUCCESS;
          };

          inline pami_result_t typed_impl (pami_send_typed_t * parameters)
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT("T_Model::packet_model_metadata_bytes = %zu", T_Model::packet_model_metadata_bytes);

            pami_task_t task;
            size_t offset;
            PAMI_ENDPOINT_INFO(parameters->send.dest, task, offset);

            // Verify that this task is addressable by this packet device
            if (unlikely(_short_model.device.isPeer (task) == false))
              {
                TRACE_FN_EXIT();
                return PAMI_INVAL;
              }

            const size_t header_bytes = parameters->send.header.iov_len;

            const size_t data_bytes   = parameters->send.data.iov_len;

            const size_t total_bytes  = header_bytes + data_bytes;

            // ----------------------------------------------------------------
            // Check for a "short" send protocol
            // ----------------------------------------------------------------
#ifdef ERROR_CHECKS

            if ((T_Option & RECV_IMMEDIATE_FORCEON) && (total_bytes > maximum_short_packet_payload))
              {
                // 'receive immediate' is forced ON, yet the application
                // header + data will not fit in a single packet.
                TRACE_STRING("Application error: 'recv immediate' forced on.");
                TRACE_FN_EXIT();
                return PAMI_INVAL;
              }

#endif

            //
            //   'immediate receives' are forced on ('long header' is irrelevant),
            //      OR
            //   'immediate receives' are not forced off AND the header + data fit in a single packet
            //
//fprintf (stderr, "(T_Option & RECV_IMMEDIATE_FORCEON) = %d .. (T_Option & RECV_IMMEDIATE_FORCEOFF) = %d .. !(T_Option & RECV_IMMEDIATE_FORCEOFF) = %d\n", (T_Option & RECV_IMMEDIATE_FORCEON), (T_Option & RECV_IMMEDIATE_FORCEOFF), !(T_Option & RECV_IMMEDIATE_FORCEOFF));
//fprintf (stderr, "!(T_Option & RECV_IMMEDIATE_FORCEOFF) && (%zu <= %zu) == %d\n", total_bytes, maximum_short_packet_payload, !(T_Option & RECV_IMMEDIATE_FORCEOFF) && total_bytes);
            if ((T_Option & RECV_IMMEDIATE_FORCEON) ||
                (!(T_Option & RECV_IMMEDIATE_FORCEOFF) &&
                 (total_bytes <= maximum_short_packet_payload)))
              {
                // Allocate memory to maintain the state of the send.
                eager_state_t * state = allocateState ();

                state->origin.cookie        = parameters->events.cookie;
                state->origin.local_fn      = parameters->events.local_fn;
                state->origin.remote_fn     = parameters->events.remote_fn;
                state->origin.target_task   = task;
                state->origin.target_offset = offset;
                state->origin.protocol      = this;

                pami_result_t result = send_packed (state, task, offset, parameters);

                TRACE_FN_EXIT();
                return result;
              }

#ifdef ERROR_CHECKS

            if ((T_Option & LONG_HEADER_DISABLE) && (header_bytes > maximum_eager_packet_payload))
              {
                // 'long header' support is disabled, yet the application
                // header will not fit in a single packet.
                TRACE_STRING("Application error: 'long header' support is disabled.");
                TRACE_FN_EXIT();
                return PAMI_INVAL;
              }

#endif

            // ----------------------------------------------------------------
            // Send a single-packet envelope eager message
            // ----------------------------------------------------------------

            // Allocate memory to maintain the state of the send.
            eager_state_t * state = allocateState ();

            state->origin.cookie        = parameters->events.cookie;
            state->origin.local_fn      = parameters->events.local_fn;
            state->origin.remote_fn     = parameters->events.remote_fn;
            state->origin.target_task   = task;
            state->origin.target_offset = offset;
            state->origin.protocol      = this;

            // Specify the protocol metadata to send with the application
            // metadata in the envelope packet.
            state->origin.eager.envelope.single.metadata.bytes        = data_bytes;
            state->origin.eager.envelope.single.metadata.metabytes    = header_bytes;
            state->origin.eager.envelope.single.metadata.origin       = _origin;

            TRACE_FORMAT("parameters->send.header.iov_len = %zu, parameters->send.data.iov_len = %zu", header_bytes, data_bytes);


            if (unlikely(data_bytes == 0))
              {
                send_envelope (state, task, offset, parameters->send.header, send_complete);
              }
            else
              {
                send_envelope (state, task, offset, parameters->send.header, NULL);
                send_data<false> (state, task, offset, parameters);
              }


            TRACE_FN_EXIT();
            return PAMI_SUCCESS;
          };

        protected:

          MemoryAllocator < sizeof(eager_state_t), 16 > _state_allocator;

          T_Model          _envelope_model;
          T_Model          _longheader_envelope_model;
          T_Model          _longheader_message_model;
          T_Model          _data_model;
          T_Model          _ack_model;
          T_Model          _short_model;
          pami_endpoint_t  _origin;
          pami_context_t   _context;

          pami_dispatch_p2p_function _dispatch_fn;
          void                     * _cookie;

          inline eager_state_t * allocateState ()
          {
            TRACE_FN_ENTER();
            eager_state_t * state = (eager_state_t *) _state_allocator.allocateObject ();
            TRACE_FN_EXIT();
            return state;
          }

          inline void freeState (eager_state_t * state)
          {
            _state_allocator.returnObject ((void *) state);
          }

          // ##################################################################
          // The "remote fn" capability, as required by pami.h, is implemented
          // as a completely separate communication flow in order to delay
          // the remote function logic so that it is outside of the critical
          // code path for latency performance.
          //
          // A 'ping' packet containing the "remote fn" event function
          // information is sent to the target task and then echoed back to the
          // origin task in a 'pong' packet.  When the origin task receives the
          // 'pong' packet it invokes the "remote fn" event function callback.
          // ##################################################################

          ///
          /// \brief Initiate a "remote fn" pingpong
          ///
          /// The 'ping' packet to the target task will transfer the origin
          /// endpoint and the completion event function and cookie to be sent
          /// back to the origin within the 'pong' packet.
          ///
          /// \param [in] task   Target task identifier
          /// \param [in] offset Target context offset identifier
          /// \param [in] fn     Origin completion function
          /// \param [in] cookie Origin completion cookie
          ///
          void send_remotefn (pami_task_t           task,
                              size_t                offset,
                              pami_event_function   fn,
                              void                * cookie);

          ///
          /// \brief Receive the "remote fn" ping and pong packets
          ///
          /// If a 'ping' packet is received, which may only happen on the
          /// target task, send a 'pong' packet back to the origin endpoint
          /// containing the origin completion function and the origin
          /// completion cookie.
          ///
          /// If a 'pong' packet is received, which may only happen on the
          /// origin task, invoke the completion function contained within the
          /// pong packet with the completion cookie contained within the pong
          /// packet.
          ///
          /// \see PAMI::Device::Interface::RecvFunction_t
          ///
          static int dispatch_remotefn (void   * metadata,
                                        void   * payload,
                                        size_t   bytes,
                                        void   * recv_func_parm,
                                        void   * cookie);

          ///
          /// \brief Inject "remote fn" ping or pong completion callback
          ///
          /// This completion function will return any resources allocated to
          /// conduct the \e send of a 'ping' or 'pong' packet.
          ///
          /// \see pami_event_function
          ///
          static void complete_remotefn (pami_context_t   context,
                                         void           * cookie,
                                         pami_result_t    result);

          // ##################################################################
          // 'packed' code section
          // ##################################################################

          ///
          /// \brief Initiate an 'immediate' packed transfer
          ///
          /// The protocol metadata, application header, and application data
          /// will be transfered in a single packet to the target task.
          ///
          /// \note The zero-byte application data case is handled as a 'packed'
          ///       eager transfer if the application header will fit in a
          ///       single packet, or as a 'multi-packet envelope' transfer
          ///       without any data packets to follow.
          ///
          /// \note Does not support non-contiguous source data transfers.
          ///
          /// \param [in] task         Target task identifier
          /// \param [in] offset       Target context identifier
          /// \param [in] header_bytes Number of application header bytes to transfer
          /// \param [in] data_bytes   Number of application data bytes to transfer
          /// \param [in] parameters   Send immediate parameter structure
          ///
          /// \see dispatch_packed
          ///
          inline bool send_packed (pami_task_t             task,
                                   size_t                  offset,
                                   const size_t            header_bytes,
                                   const size_t            data_bytes,
                                   pami_send_immediate_t * parameters);

          ///
          /// \brief Initiate a packed, or 'short', contigous data transfer
          ///
          /// The protocol metadata, application header, and application data
          /// will be transfered in a single packet to the target task.
          ///
          /// \note The zero-byte application data case is handled as a 'packed'
          ///       eager transfer if the application header will fit in a
          ///       single packet, or as a 'multi-packet envelope' transfer
          ///       without any data packets to follow.
          ///
          /// \param [in] state      Eager state structure to track the transfer
          /// \param [in] task       Target task identifier
          /// \param [in] offset     Target context identifier
          /// \param [in] parameters Send parameter structure
          ///
          /// \see dispatch_packed
          ///
          inline pami_result_t send_packed (eager_state_t * state,
                                            pami_task_t     task,
                                            size_t          offset,
                                            pami_send_t   * parameters);

          ///
          /// \brief Initiate a packed, or 'short', non-contiguous data transfer
          ///
          /// The protocol metadata, application header, and application data
          /// will be transfered in a single packet to the target task.
          ///
          /// \note The zero-byte application data case is handled as a 'packed'
          ///       eager transfer if the application header will fit in a
          ///       single packet, or as a 'multi-packet envelope' transfer
          ///       without any data packets to follow.
          ///
          /// \param [in] state      Eager state structure to track the transfer
          /// \param [in] task       Target task identifier
          /// \param [in] offset     Target context identifier
          /// \param [in] parameters Send typed parameter structure
          ///
          /// \see dispatch_packed
          ///
          inline pami_result_t send_packed (eager_state_t     * state,
                                            pami_task_t         task,
                                            size_t              offset,
                                            pami_send_typed_t * parameters);

          ///
          /// \brief Receive a packed single-packet, or 'short', packet.
          ///
          /// The packed dispatch function is invoked by the packet device
          /// to process a single-packet message. The message will contain the
          /// protocol metadata, application header, and application data in a
          /// single packet.
          ///
          /// \see PAMI::Device::Interface::RecvFunction_t
          ///
          static int dispatch_packed (void   * metadata,
                                      void   * payload,
                                      size_t   bytes,
                                      void   * recv_func_parm,
                                      void   * cookie);

          // ##################################################################
          // 'envelope' code section
          // ##################################################################

          ///
          /// \brief Initiate an eager message envelope transfer
          ///
          /// The protocol metadata and application header will be transfered
          /// to the target task. If possible, the metadata and header will be
          /// sent in a single packet. Otherwise, depending on the configuration
          /// specified in the T_Option template parameter, the protocol
          /// metadata will be sent in a single packet, followed by one or more
          /// packets containing the application header.
          ///
          /// \note The zero-byte application data case is handled as a 'packed'
          ///       eager transfer if the application header will fit in a
          ///       single packet, or as a 'multi-packet envelope' transfer
          ///       without any data packets to follow.
          ///
          /// \param [in] state      Eager state structure to track the transfer
          /// \param [in] task       Target task identifier
          /// \param [in] offset     Target context identifier
          /// \param [in] header     Application header to transfer
          /// \param [in] done_fn    Origin task completion function
          ///
          /// \see dispatch_envelope_packed
          /// \see dispatch_envelope_metadata
          /// \see dispatch_envelope_header
          ///
          inline pami_result_t send_envelope (eager_state_t        * state,
                                              pami_task_t           task,
                                              size_t                offset,
                                              struct iovec        & header,
                                              pami_event_function   done_fn);

          ///
          /// \brief Receive a single-packet, or 'packed', eager envelope.
          ///
          /// The protocol metadata and the \e entire application header are
          /// contained in a single 'packed' eager envelope packet. This
          /// protocol dispatch function will invoke the application dispatch
          /// function to obtain the destination address for the data packets to
          /// follow.
          ///
          /// \tparam T_Contiguous The data will be received into a contiguous
          ///                      destination buffer
          /// \tparam T_Copy       The data will be copied from the incoming
          ///                      data packets into the destination buffer
          ///
          /// \see process_envelope
          /// \see PAMI::Device::Interface::RecvFunction_t
          ///
          template <bool T_Contiguous, bool T_Copy>
          static int dispatch_envelope_packed (void   * metadata,
                                               void   * payload,
                                               size_t   bytes,
                                               void   * recv_func_parm,
                                               void   * cookie);

          ///
          /// \brief Receive the protocol metadata for a multi-packet envelope
          ///
          /// The protocol metadata is contained in a single packet and
          /// describes the application header and application data packets to
          /// follow.
          ///
          /// \see PAMI::Device::Interface::RecvFunction_t
          ///
          static int dispatch_envelope_metadata (void   * metadata,
                                                 void   * payload,
                                                 size_t   bytes,
                                                 void   * recv_func_parm,
                                                 void   * cookie);

          ///
          /// \brief Receive one application header packet of a multi-packet envelope
          ///
          /// The application dispatch function which provides the destination
          /// address and type information for the data packets to follow will
          /// not be invoked until the \e entire application header has been
          /// received.
          ///
          /// \tparam T_Contiguous The data will be received into a contiguous
          ///                      destination buffer
          /// \tparam T_Copy       The data will be copied from the incoming
          ///                      data packets into the destination buffer
          ///
          /// \see process_envelope
          /// \see PAMI::Device::Interface::RecvFunction_t
          ///
          template <bool T_Contiguous, bool T_Copy>
          static int dispatch_envelope_header (void   * metadata,
                                               void   * payload,
                                               size_t   bytes,
                                               void   * recv_func_parm,
                                               void   * cookie);

          ///
          /// \brief Invoke the application dispatch function and initialize the data receive
          ///
          /// The application dispatch function is invoked to provide the
          /// destination address and type information for the data packets to
          /// follow. Initialize the receive state information to either copy
          /// that data into a contiguous destination buffer, or invoke a type
          /// machine to 'transform' the data into the destination buffer.
          ///
          /// \tparam T_Contiguous The data will be received into a contiguous
          ///                      destination buffer
          /// \tparam T_Copy       The data will be copied from the incoming
          ///                      data packets into the destination buffer
          ///
          /// \param [in] header       Address of the received application header
          /// \param [in] header_bytes Number of bytes of application header
          /// \param [in] data_bytes   Number of bytes of application data
          /// \param [in] origin       Endpoint that originated the transfer
          /// \param [in] state        Eager state structure to track the transfer
          ///
          template <bool T_Contiguous, bool T_Copy>
          inline void process_envelope (void               * header,
                                        size_t               header_bytes,
                                        size_t               data_bytes,
                                        pami_endpoint_t      origin,
                                        eager_state_t      * state);

          // ##################################################################
          // 'data' code section
          // ##################################################################

          ///
          /// \brief Initiate an eager message data transfer
          ///
          /// The protocol metadata and application header will have been
          /// previously transfered to the target task.
          ///
          /// If the template parameter \c T_Contiguous is \c true, or if the
          /// typed send parameter structure specifies a contiguous copy, then
          /// the source data will be directly transfered to the target task.
          /// Otherwise a type machine must be instatiated to transform the
          /// source data into one or more temporary data buffer(s) to be
          /// transfered to the target task.
          ///
          /// \note The zero-byte application data case is handled as a 'packed'
          ///       eager transfer if the application header will fit in a
          ///       single packet, or as a 'multi-packet envelope' transfer
          ///       without any data packets to follow. Consequently, this
          ///       method is not needed to transfer the zero bytes.
          ///
          /// \tparam T_ContiguousCopy Force a contiguous copy data transfer
          ///
          /// \param [in] state      Eager state structure to track the transfer
          /// \param [in] task       Target task identifier
          /// \param [in] offset     Target context identifier
          /// \param [in] header     Application header to transfer
          /// \param [in] done_fn    Origin task completion function
          ///
          /// \see send_packed
          /// \see send_envelope
          /// \see dispatch_data
          ///
          template <bool T_ContiguousCopy>
          inline pami_result_t send_data (eager_state_t        * state,
                                          pami_task_t           task,
                                          size_t                offset,
                                          pami_send_typed_t   * parameters);

          ///
          /// \brief Receive one application data packet
          ///
          /// The portion of the entire application data contained in each data
          /// packet that is received is either copied into the destination
          /// data buffer, or, if configured by the application dispatch
          /// function, transformed into the destnation data buffer using the
          /// type machine.
          ///
          /// The application dispatch completion function will not be invoked
          /// until all application data bytes have been received.
          ///
          /// \tparam T_ContiguousCopy If \c true the data will be copied into a
          ///                          contiguous destination buffer, otherwise
          ///                          the type machine will transform the data
          ///                          into the destination buffer.
          ///
          /// \see PAMI::Device::Interface::RecvFunction_t
          ///
          template <bool T_ContiguousCopy>
          static int dispatch_data   (void   * metadata,
                                      void   * payload,
                                      size_t   bytes,
                                      void   * recv_func_parm,
                                      void   * cookie);

          ///
          /// \brief Data pipeline completion event callback, invoke on the origin.
          ///
          /// Only invoked for non-contiguous source data transfers.
          ///
          /// \see pami_event_function
          ///
          static void complete_data (pami_context_t   context,
                                     void           * cookie,
                                     pami_result_t    result);

          ///
          /// \brief Local send completion event callback.
          ///
          /// This callback will invoke the application local completion
          /// callback function and, if notification of remote receive
          /// completion is not required, free the send state memory.
          ///
          static void send_complete (pami_context_t   context,
                                     void           * cookie,
                                     pami_result_t    result)
          {
            TRACE_FN_ENTER();

            eager_state_t * state = (eager_state_t *) cookie;
            EagerSimpleProtocol * eager = (EagerSimpleProtocol *) state->origin.protocol;

            if (likely(state->origin.local_fn != NULL))
              {
                state->origin.local_fn (eager->_context, state->origin.cookie, PAMI_SUCCESS);
              }

            eager->freeState (state);

            if (unlikely(state->origin.remote_fn != NULL))
              {
                eager->send_remotefn (state->origin.target_task,
                                      state->origin.target_offset,
                                      state->origin.remote_fn,
                                      state->origin.cookie);
              }

            TRACE_FN_EXIT();
            return;
          }

      };
    };
  };
};

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#include "p2p/protocols/send/eager/EagerSimple_data_impl.h"
#include "p2p/protocols/send/eager/EagerSimple_envelope_impl.h"
#include "p2p/protocols/send/eager/EagerSimple_packed_impl.h"
#include "p2p/protocols/send/eager/EagerSimple_remotefn_impl.h"

#endif // __pami_p2p_protocol_send_eager_eagersimple_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
