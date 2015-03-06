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
/// \file p2p/protocols/get/GetOverSend_data_impl.h
/// \brief ??
///
#ifndef __p2p_protocols_get_GetOverSend_data_impl_h__
#define __p2p_protocols_get_GetOverSend_data_impl_h__

#ifndef __p2p_protocols_get_GetOverSend_h__
#error "implementation #include'd before definition"
#endif

#include <stdint.h>

#include "common/type/TypeCode.h"
#include "common/type/TypeMachine.h"

#include "util/trace.h"
#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

using namespace PAMI::Protocol::Get;

template <class T_Model>
inline void GetOverSend<T_Model>::send_data (uintptr_t     metadata,
                                             void        * data_addr,
                                             size_t        data_bytes,
                                             pami_task_t   task,
                                             size_t        offset,
                                             state_t     * state)
{
  TRACE_FN_ENTER();

  // Set the data information needed by the complete_data callback.
  state->target.data.protocol = this;
  state->target.data.bytes_remaining = 0;
  state->target.data.is_contiguous   = true;

  TRACE_FORMAT("metadata = %p", (void *) metadata);

  // Send the contiguous source buffer to the origin endpoint.
  _data_model.postMultiPacket (state->target.data.state[0],
                               complete_data, state,
                               task, offset,
                               (void *) & metadata,
                               sizeof(uintptr_t),
                               (void *) data_addr,
                               data_bytes);

  TRACE_FN_EXIT();
  return;
}

template <class T_Model>
inline void GetOverSend<T_Model>::send_data (state_t * state)
{
  TRACE_FN_ENTER();

  state->target.data.protocol = this;

  Type::TypeMachine * machine = (Type::TypeMachine *) state->target.data.machine;

  const size_t bytes_remaining = state->target.data.bytes_remaining;

  TRACE_FORMAT("bytes_remaining = %zu (%ld)", bytes_remaining, sizeof(model_packet_t));
  if (bytes_remaining <= sizeof(model_packet_t))
    {
      // "short", non-pipelined, pack-n-send
      machine->Pack (state->target.data.packet[0],
                     state->target.data.base_addr,
                     bytes_remaining);

      _data_model.postMultiPacket (state->target.data.state[0],
                                   complete_data, state,
                                   state->target.data.origin_task,
                                   state->target.data.origin_offset,
                                   (void *) & state->target.data.metadata,
                                   sizeof (uintptr_t),
                                   (void *) state->target.data.packet[0],
                                   bytes_remaining);
    }
  else if (bytes_remaining <= (sizeof(model_packet_t) << 1))
    {
      // pack-n-send the first full pipeline width _without_ a completion callback.
      machine->Pack (state->target.data.packet[0],
                     state->target.data.base_addr,
                     sizeof(model_packet_t));

      _data_model.postMultiPacket (state->target.data.state[0],
                                   NULL, NULL,
                                   state->target.data.origin_task,
                                   state->target.data.origin_offset,
                                   (void *) & state->target.data.metadata,
                                   sizeof (uintptr_t),
                                   (void *) state->target.data.packet[0],
                                   sizeof(model_packet_t));

      // pack-n-send the second (and also last) pipeline width with a normal
      // data completion callback.
      machine->Pack (state->target.data.packet[1],
                     state->target.data.base_addr,
                     bytes_remaining - sizeof(model_packet_t));

      _data_model.postMultiPacket (state->target.data.state[1],
                                   complete_data, state,
                                   state->target.data.origin_task,
                                   state->target.data.origin_offset,
                                   (void *) & state->target.data.metadata,
                                   sizeof (uintptr_t),
                                   (void *) state->target.data.packet[1],
                                   bytes_remaining - sizeof(model_packet_t));
    }
  else
    {
      state->target.data.start_count = 2;
      state->target.data.bytes_remaining -= sizeof(model_packet_t) * 2;

      // pack-n-send the first full pipeline width with a data pipeline
      // completion callback.
      machine->Pack (state->target.data.packet[0],
                     state->target.data.base_addr,
                     sizeof(model_packet_t));

      _data_model.postMultiPacket (state->target.data.state[0],
                                   complete_data, state,
                                   state->target.data.origin_task,
                                   state->target.data.origin_offset,
                                   (void *) & state->target.data.metadata,
                                   sizeof (uintptr_t),
                                   (void *) state->target.data.packet[0],
                                   sizeof(model_packet_t));

      // pack-n-send the second full pipeline width with a data pipeline
      // completion callback.
      machine->Pack (state->target.data.packet[1],
                     state->target.data.base_addr,
                     sizeof(model_packet_t));

      _data_model.postMultiPacket (state->target.data.state[1],
                                   complete_data, state,
                                   state->target.data.origin_task,
                                   state->target.data.origin_offset,
                                   (void *) & state->target.data.metadata,
                                   sizeof (uintptr_t),
                                   (void *) state->target.data.packet[1],
                                   sizeof(model_packet_t));
    }

  TRACE_FN_EXIT();
  return;
};

template <class T_Model>
void GetOverSend<T_Model>::complete_data (pami_context_t   context,
                                          void           * cookie,
                                          pami_result_t    result)
{
  TRACE_FN_ENTER();

  state_t * state = (state_t *) cookie;
  GetOverSendProtocol * protocol = (GetOverSendProtocol *) state->target.data.protocol;

  const size_t bytes_remaining = state->target.data.bytes_remaining;

  TRACE_FORMAT("bytes_remaining = %zu", bytes_remaining);

  if (bytes_remaining == 0)
    {
      // No more data to send. clean up.

      if (! state->target.data.is_contiguous)
        {
          // Destroy the target type machine.
          Type::TypeMachine * machine = (Type::TypeMachine *) state->target.data.machine;
          machine->~TypeMachine();

          // Free the temporary serialized target type memory
          free (state->target.envelope.type_buffer);
        }

      // Return the target state object memory to the memory pool.
      protocol->_allocator.returnObject(state);

      TRACE_STRING("target send data completed");
      TRACE_FN_EXIT();
      return;
    }

  // Determine which pipline completion invoked this callback, and increment the
  // start count for the next pipeline completion.
  const size_t which = (state->target.data.start_count++) & 0x01;

  Type::TypeMachine * machine = (Type::TypeMachine *) state->target.data.machine;

  TRACE_FORMAT("which = %zu", which);
  if (bytes_remaining <= sizeof(model_packet_t))
    {
      // pack-n-send the last pipeline width with a data completion callback.
      machine->Pack (state->target.data.packet[which],
                     state->target.data.base_addr,
                     bytes_remaining);

      state->target.data.bytes_remaining -= bytes_remaining;

      protocol->_data_model.postMultiPacket (state->target.data.state[which],
                                             complete_data, state,
                                             state->target.data.origin_task,
                                             state->target.data.origin_offset,
                                             (void *) & state->target.data.metadata,
                                             sizeof (uintptr_t),
                                             state->target.data.packet[which],
                                             bytes_remaining);
    }
  else if (bytes_remaining <= (sizeof(model_packet_t) * 2))
    {
      // pack-n-send this next-to-last full pipeline width _without_ a
      // completion callback. This is allowed because the next pipeline width is
      // also the last pipeline width and this last pipeline width will specify
      // a data completion callback (see if clause above).
      machine->Pack (state->target.data.packet[which],
                     state->target.data.base_addr,
                     sizeof(model_packet_t));

      state->target.data.bytes_remaining -= sizeof(model_packet_t);

      protocol->_data_model.postMultiPacket (state->target.data.state[which],
                                             NULL, NULL,
                                             state->target.data.origin_task,
                                             state->target.data.origin_offset,
                                             (void *) & state->target.data.metadata,
                                             sizeof (uintptr_t),
                                             state->target.data.packet[which],
                                             sizeof(model_packet_t));
    }
  else
    {
      // pack-n-send a full pipeline width with a data pipeline completion callback.
      machine->Pack (state->target.data.packet[which],
                     state->target.data.base_addr,
                     sizeof(model_packet_t));

      state->target.data.bytes_remaining -= sizeof(model_packet_t);

      protocol->_data_model.postMultiPacket (state->target.data.state[which],
                                             complete_data, state,
                                             state->target.data.origin_task,
                                             state->target.data.origin_offset,
                                             (void *) & state->target.data.metadata,
                                             sizeof (uintptr_t),
                                             state->target.data.packet[which],
                                             sizeof(model_packet_t));
    }

  TRACE_FN_EXIT();
  return;
};

template <class T_Model>
int GetOverSend<T_Model>::dispatch_data (void   * metadata,
                                         void   * payload,
                                         size_t   bytes,
                                         void   * recv_func_parm,
                                         void   * cookie)
{
  TRACE_FN_ENTER();

  GetOverSendProtocol * protocol = (GetOverSendProtocol *) recv_func_parm;
  uint8_t stack[T_Model::packet_model_payload_bytes];

  // Get origin state memory from packet metadata.
  state_t * state = (state_t *) * ((uintptr_t *) metadata);

  TRACE_FORMAT("state = %p", state);
  TRACE_FORMAT("state->origin.base_addr       = %p", state->origin.base_addr);
  TRACE_FORMAT("state->origin.bytes_remaining = %zu", state->origin.bytes_remaining);
  TRACE_FORMAT("state->origin.is_contiguous   = %d", state->origin.is_contiguous);

  const size_t bytes_to_copy = MIN(bytes, state->origin.bytes_remaining);

  if (likely(state->origin.is_contiguous == true))
    {
      TRACE_FORMAT("copy payload data to %p", state->origin.base_addr);

      if (T_Model::read_is_required_packet_model)
        protocol->_data_model.device.read (state->origin.base_addr, bytes_to_copy, cookie);
      else
        Core_memcpy (state->origin.base_addr, payload, bytes_to_copy);

      state->origin.base_addr =
        ((uint8_t *)state->origin.base_addr) + bytes_to_copy;
    }
  else
    {
      if (T_Model::read_is_required_packet_model)
        {
          payload = (void *) & stack[0];
          protocol->_data_model.device.read (payload, bytes, cookie);
        }

      TRACE_FORMAT("unpack to %p", state->origin.base_addr);
      TRACE_FORMAT("unpack from %p", payload);
      TRACE_FORMAT("unpack bytes %zu", bytes_to_copy);
      Type::TypeMachine * machine = (Type::TypeMachine *) state->origin.machine;
      machine->Unpack (state->origin.base_addr, payload, bytes_to_copy);
    }

  state->origin.bytes_remaining -= bytes_to_copy;

  if (state->origin.bytes_remaining == 0)
    {
      // No more data packets will be received on this connection.

      if (! state->origin.is_contiguous)
        {
          // Destroy the origin machine.
          Type::TypeMachine * machine = (Type::TypeMachine *) state->origin.machine;
          machine->~TypeMachine();
        }

      // Invoke the application completion callback
      if (state->origin.done_fn != NULL)
        state->origin.done_fn (protocol->_context, state->origin.cookie, PAMI_SUCCESS);

      // Return the origin state object memory to the memory pool.
      protocol->_allocator.returnObject (state);

      TRACE_STRING("origin task ... data receive completed");
      TRACE_FN_EXIT();
      return 0;
    }

  TRACE_STRING("origin task ... wait for more data");
  TRACE_FN_EXIT();
  return 0;
};

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG
#endif // __p2p_protocols_get_GetOverSend_data_impl_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
