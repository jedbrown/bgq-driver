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
/// \file p2p/protocols/send/eager/EagerSimple_envelope_impl.h
/// \brief Eager 'header envelope' protocol implementation.
///
#ifndef __p2p_protocols_send_eager_EagerSimple_envelope_impl_h__
#define __p2p_protocols_send_eager_EagerSimple_envelope_impl_h__

#ifndef __p2p_protocols_send_eager_EagerSimple_h__
#error "implementation #include'd before definition"
#endif

#include "util/trace.h"
#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0


using namespace PAMI::Protocol::Send;

template < class T_Model, configuration_t T_Option >
inline pami_result_t EagerSimple<T_Model, T_Option>::send_envelope (eager_state_t       * state,
                                                                    pami_task_t           task,
                                                                    size_t                offset,
                                                                    struct iovec        & header,
                                                                    pami_event_function   done_fn)
{
  TRACE_FN_ENTER();

  TRACE_FORMAT("sizeof(longheader_metadata_t) = %zu", sizeof(longheader_metadata_t));

#ifdef ERROR_CHECKS

  if ((T_Option & LONG_HEADER_DISABLE) && (header.iov_len > maximum_eager_packet_payload))
    {
      // 'long header' support is disabled, yet the application
      // header will not fit in a single packet.
      TRACE_STRING("Application error: 'long header' support is disabled.");
      TRACE_FN_EXIT();
      return PAMI_INVAL;
    }

#endif

  // ----------------------------------------------------------------
  // Check for a "long header" send protocol
  // ----------------------------------------------------------------
  if (unlikely(!(T_Option & LONG_HEADER_DISABLE) &&
               (header.iov_len > maximum_eager_packet_payload)))
    {
      // Specify the protocol metadata to send with the application
      // metadata in the envelope packet.
      state->origin.eager.envelope.multi.metadata.bytes =
        state->origin.eager.envelope.single.metadata.bytes;
        
      state->origin.eager.envelope.multi.metadata.metabytes =
        header.iov_len;
        
      state->origin.eager.envelope.multi.metadata.origin =
        _origin;

      TRACE_STRING("long header special case, protocol metadata does not fit in the packet metadata");
      _longheader_envelope_model.postPacket (state->origin.eager.envelope.multi.state[0],
                                             NULL, NULL,
                                             task, offset,
                                             (void *) NULL, 0,
                                             (void *) &(state->origin.eager.envelope.multi.metadata),
                                             sizeof (longheader_metadata_t));

      _longheader_message_model.postMultiPacket (state->origin.eager.envelope.multi.state[1],
                                                 done_fn,
                                                 (void *) state,
                                                 task,
                                                 offset,
                                                 (void *) &(state->origin.eager.envelope.multi.metadata.origin),
                                                 sizeof (pami_endpoint_t),
                                                 header.iov_base,
                                                 header.iov_len);
    }
  else
    {
      //
      // Send the protocol metadata and the application metadata in the envelope packet
      //
      // This branch should be resolved at compile time and optimized out.
      if (sizeof(shortheader_metadata_t) <= T_Model::packet_model_metadata_bytes)
        {
          TRACE_STRING("protocol metadata fits in the packet metadata");

          _envelope_model.postPacket (state->origin.eager.envelope.single.state,
                                      done_fn,
                                      (void *) state,
                                      task, offset,
                                      (void *) &(state->origin.eager.envelope.single.metadata),
                                      sizeof (shortheader_metadata_t),
                                      header.iov_base,
                                      header.iov_len);
        }
      else
        {
          TRACE_STRING("protocol metadata does not fit in the packet metadata");

          state->origin.eager.envelope.single.v[0].iov_base =
            (void *) & (state->origin.eager.envelope.single.metadata);
            
          state->origin.eager.envelope.single.v[0].iov_len =
            sizeof (shortheader_metadata_t);
            
          state->origin.eager.envelope.single.v[1].iov_base =
            header.iov_base;
            
          state->origin.eager.envelope.single.v[1].iov_len =
            header.iov_len;

          _envelope_model.postPacket (state->origin.eager.envelope.single.state,
                                      done_fn,
                                      (void *) state,
                                      task, offset,
                                      NULL, 0,
                                      state->origin.eager.envelope.single.v);
        }
    }

  TRACE_FN_EXIT();
  return PAMI_SUCCESS;
};

template < class T_Model, configuration_t T_Option >
template <bool T_Contiguous, bool T_Copy>
inline int EagerSimple<T_Model, T_Option>::dispatch_envelope_packed (void   * metadata,
                                                                     void   * payload,
                                                                     size_t   bytes,
                                                                     void   * recv_func_parm,
                                                                     void   * cookie)
{
  TRACE_FN_ENTER();
  EagerSimpleProtocol * eager = (EagerSimpleProtocol *) recv_func_parm;
  uint8_t stack[T_Model::packet_model_payload_bytes];

  if (T_Model::read_is_required_packet_model)
    {
      payload = (void *) & stack[0];
      eager->_short_model.device.read (payload, bytes, cookie);
    }

  shortheader_metadata_t * m;
  void * p;

  // This branch should be resolved at compile time and optimized out.
  if (sizeof(shortheader_metadata_t) > T_Model::packet_model_metadata_bytes)
    {
      m = (shortheader_metadata_t *) payload;
      p = (void *) (m + 1);
    }
  else
    {
      m = (shortheader_metadata_t *) metadata;
      p = payload;
    }

  TRACE_FORMAT("origin = 0x%08x, m->bytes = %zu", m->origin, m->bytes);

  // Allocate and initialize recv state object!
  eager_state_t * state = eager->allocateState ();
  state->target.info.local_fn = NULL;

  // This is a short header envelope .. all application metadata
  // has been received.
  eager->process_envelope<T_Contiguous, T_Copy> ((uint8_t *)p, m->metabytes, m->bytes, m->origin, state);

  if (unlikely(m->bytes == 0))
    {
      // No data packets will be received. Invoke the receive
      // completion callback function and return the receive state.

      if (unlikely(state->target.info.local_fn != NULL))
        state->target.info.local_fn (eager->_context, state->target.info.cookie, PAMI_SUCCESS);

      eager->freeState (state);

      TRACE_FN_EXIT();
      return 0;
    }

  // Complete the receive state initialization
  state->target.eager         = eager;
  state->target.data.bytes_received      = 0;
  state->target.data.bytes_total     = m->bytes;
  state->target.header.bytes_total   = m->metabytes;

  // Set the eager connection.
  pami_task_t task;
  size_t offset;
  PAMI_ENDPOINT_INFO(m->origin, task, offset);
  eager->_envelope_model.device.setConnection ((void *)state, task, offset);

  TRACE_FN_EXIT();
  return 0;
};

template < class T_Model, configuration_t T_Option >
inline int EagerSimple<T_Model, T_Option>::dispatch_envelope_metadata (void   * metadata,
                                                                       void   * payload,
                                                                       size_t   bytes,
                                                                       void   * recv_func_parm,
                                                                       void   * cookie)
{
  TRACE_FN_ENTER();
  EagerSimpleProtocol * eager = (EagerSimpleProtocol *) recv_func_parm;
  uint8_t stack[T_Model::packet_model_payload_bytes];

  if (T_Model::read_is_required_packet_model)
    {
      payload = (void *) & stack[0];
      eager->_short_model.device.read (payload, bytes, cookie);
    }

  longheader_metadata_t * m = (longheader_metadata_t *) payload;

  TRACE_FORMAT("origin = 0x%08x, m->bytes = %zu", m->origin, m->bytes);

  // Allocate a recv state object!
  eager_state_t * state = eager->allocateState ();
  state->target.eager                 = eager;
  state->target.data.bytes_received   = 0;
  state->target.data.bytes_total      = m->bytes;
  state->target.header.bytes_total    = m->metabytes;
  state->target.header.bytes_received = 0;

  // Set the eager connection.
  pami_task_t task;
  size_t offset;
  PAMI_ENDPOINT_INFO(m->origin, task, offset);
  eager->_longheader_envelope_model.device.setConnection ((void *)state, task, offset);

  pami_result_t prc;
  prc = __global.heap_mm->memalign((void **) & state->target.header.addr, 0,
                                   state->target.header.bytes_total);
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of state->target.header.addr failed");

  TRACE_FN_EXIT();
  return 0;
};

template < class T_Model, configuration_t T_Option >
template <bool T_Contiguous, bool T_Copy>
inline int EagerSimple<T_Model, T_Option>::dispatch_envelope_header (void   * metadata,
                                                                     void   * payload,
                                                                     size_t   bytes,
                                                                     void   * recv_func_parm,
                                                                     void   * cookie)
{
  TRACE_FN_ENTER();
  EagerSimpleProtocol * eager = (EagerSimpleProtocol *) recv_func_parm;
  uint8_t stack[T_Model::packet_model_payload_bytes];

  if (T_Model::read_is_required_packet_model)
    {
      payload = (void *) & stack[0];
      eager->_short_model.device.read (payload, bytes, cookie);
    }

  pami_endpoint_t origin = *((pami_endpoint_t *) metadata);

  TRACE_FORMAT("origin = 0x%08x, bytes = %zu", origin, bytes);

  pami_task_t task;
  size_t offset;
  PAMI_ENDPOINT_INFO(origin, task, offset);
  eager_state_t * state = (eager_state_t *) eager->_longheader_message_model.device.getConnection (task, offset);

  size_t n = MIN(bytes, state->target.header.bytes_total - state->target.header.bytes_received);

  eager->_longheader_message_model.device.read ((void *)((uintptr_t)state->target.header.addr + state->target.header.bytes_received), n, cookie);
  state->target.header.bytes_received += n;

  if (state->target.header.bytes_total == state->target.header.bytes_received)
    {
      // The entire application header has been received. Invoke the
      // registered pami point-to-point dispatch function.

      if (state->target.data.bytes_total == 0)
        {
          // No data packets will follow this 'long header' message.
          // Invoke the dispatch function as an 'immediate' receive.
          eager->_dispatch_fn (eager->_context,            // Communication context
                               eager->_cookie,             // Dispatch cookie
                               state->target.header.addr,  // Application header
                               state->target.header.bytes_total, // Application header bytes
                               NULL,                       // No application data
                               0,                          // Number of msg bytes
                               origin,                     // Origin endpoint for the transfer
                               (pami_recv_t *) NULL);

          // No data packets will be received on this connection.
          // Clear the connection data and prepare for the next message.
          eager->_longheader_message_model.device.clearConnection (task, offset);

          // Free the malloc'd longheader buffer now that it has been
          // delivered to the application.
          __global.heap_mm->free (state->target.header.addr);

          eager->freeState (state);
        }
      else
        {
          eager->process_envelope<T_Contiguous, T_Copy> (state->target.header.addr,
                                                         state->target.header.bytes_total,
                                                         state->target.data.bytes_total,
                                                         origin,
                                                         state);

          // Free the malloc'd longheader buffer now that it has been
          // delivered to the application.
          __global.heap_mm->free (state->target.header.addr);
        }
    }

  TRACE_FN_EXIT();
  return 0;
};

template < class T_Model, configuration_t T_Option >
template <bool T_Contiguous, bool T_Copy>
inline void EagerSimple<T_Model, T_Option>::process_envelope (void            * header,
                                                              size_t            header_bytes,
                                                              size_t            data_bytes,
                                                              pami_endpoint_t   origin,
                                                              eager_state_t   * state)
{
  TRACE_FN_ENTER();
  TRACE_FORMAT("origin = 0x%08x, header = %p, header bytes = %zu", origin, header, header_bytes);

  state->target.info.type = (pami_type_t) PAMI_TYPE_BYTE;

  // Invoke the registered dispatch function.
  _dispatch_fn (_context,            // Communication context
                _cookie,             // Dispatch cookie
                header,              // Application metadata
                header_bytes,        // Application metadata bytes
                NULL,                // No payload data
                data_bytes,          // Number of msg bytes
                origin,              // Origin endpoint for the transfer
                (pami_recv_t *) &(state->target.info));

  TRACE_FORMAT("data bytes = %zu, state->target.info.type = %p", data_bytes, state->target.info.type);

  if (T_Contiguous && T_Copy)
    {
      TRACE_STRING("'contiguous copy' receive only.");
      TRACE_FN_EXIT();
      return;
    }
  else if (T_Contiguous)
    {
      state->target.info.type   = (pami_type_t) PAMI_TYPE_BYTE;
      state->target.info.offset = 0;
    }
  else if (T_Copy)
    {
      state->target.info.data_fn     = PAMI_DATA_COPY;
      state->target.info.data_cookie = NULL;
    }

  Type::TypeCode * type = (Type::TypeCode *) state->target.info.type;
  PAMI_assert_debugf(type != NULL, "state->target.info.type == NULL !");

#ifdef ERROR_CHECKS

  if (! type->IsCompleted())
    {
      //RETURN_ERR_PAMI(PAMI_INVAL, "Using an incompleted type.");
      if (state->target.info.local_fn != NULL)
        state->target.info.local_fn (_context, state->target.info.cookie, PAMI_INVAL);

      TRACE_FN_EXIT();
      return;
    }

#endif
  state->target.data.is_contiguous_copy_recv =
    (state->target.info.type == PAMI_TYPE_BYTE) &&
    (state->target.info.data_fn == PAMI_DATA_COPY);
    
  if (unlikely(!state->target.data.is_contiguous_copy_recv))
    {
      // Construct a type machine for this transfer.
      new (&(state->target.data.machine)) Type::TypeMachine (type);
      Type::TypeMachine * machine = (Type::TypeMachine *) state->target.data.machine;
      machine->SetCopyFunc (state->target.info.data_fn, state->target.info.data_cookie);
      machine->MoveCursor (state->target.info.offset);
    }

  TRACE_FN_EXIT();
  return;
};

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif // __p2p_protocols_send_eager_EagerSimple_envelope_impl_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
