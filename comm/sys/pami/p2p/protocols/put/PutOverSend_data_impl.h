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
/// \file p2p/protocols/put/PutOverSend_data_impl.h
/// \brief ??
///
#ifndef __p2p_protocols_put_PutOverSend_data_impl_h__
#define __p2p_protocols_put_PutOverSend_data_impl_h__

#ifndef __p2p_protocols_put_PutOverSend_h__
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


using namespace PAMI::Protocol::Put;

template <class T_Model>
inline void PutOverSend<T_Model>::send_data (pami_put_simple_t * parameters,
                                             pami_task_t         task,
                                             size_t              offset,
                                             state_t           * state)
{
  TRACE_FN_ENTER();

  _data_model.postMultiPacket (state->origin.data.contig.state,
                               complete_origin, state,
                               task, offset,
                               (void *) & _origin,
                               sizeof(pami_endpoint_t),
                               parameters->addr.local,
                               parameters->rma.bytes);
  
  TRACE_FN_EXIT();
}


template <class T_Model>
inline void PutOverSend<T_Model>::send_data (pami_put_typed_t * parameters,
                                             pami_task_t        task,
                                             size_t             offset,
                                             state_t          * state)
{
  TRACE_FN_ENTER();

  // Construct the local (source) type machine.
  Type::TypeCode * local_type = (Type::TypeCode *) parameters->type.local;
  Type::TypeMachine * machine = (Type::TypeMachine *) state->origin.data.typed.machine;
  new (machine) Type::TypeMachine (local_type);
  
  const size_t atom_size = local_type->GetAtomSize();

  if (T_Model::packet_model_payload_bytes % atom_size != 0)
    {
      // Partial packet data not supported.
      PAMI_abort();
      return;
    }

  state->origin.data.typed.base_addr = parameters->addr.local;
  state->origin.data.typed.bytes_remaining = parameters->rma.bytes;

  const size_t bytes_remaining = state->origin.data.typed.bytes_remaining;

  TRACE_FORMAT("bytes_remaining = %zu, sizeof(model_packet_t) = %ld", bytes_remaining, sizeof(model_packet_t));

  if (bytes_remaining <= sizeof(model_packet_t))
    {
      // "short", non-pipelined, pack-n-send
      TRACE_STRING("'short' pack-n-send.");
      machine->Pack (state->origin.data.typed.packet[0],
                     state->origin.data.typed.base_addr,
                     bytes_remaining);

      _data_model.postMultiPacket (state->origin.data.typed.state[0],
                                   complete_origin, state,
                                   task, offset,
                                   (void *) & _origin, sizeof (pami_endpoint_t),
                                   state->origin.data.typed.packet[0],
                                   bytes_remaining);
    }
  else if (bytes_remaining <= (sizeof(model_packet_t) << 1))
    {
      // pack-n-send the first full pipeline width _without_ a completion callback.
      machine->Pack (state->origin.data.typed.packet[0],
                     state->origin.data.typed.base_addr,
                     sizeof(model_packet_t));

      TRACE_FORMAT("'non pipelined' pack-n-send. bytes to send = %zu", sizeof(model_packet_t));
      _data_model.postMultiPacket (state->origin.data.typed.state[0],
                                   NULL, NULL,
                                   task, offset,
                                   (void *) & _origin, sizeof (pami_endpoint_t),
                                   state->origin.data.typed.packet[0],
                                   sizeof(model_packet_t));

      // pack-n-send the second (and also last) pipeline width with a normal
      // data completion callback.
      machine->Pack (state->origin.data.typed.packet[1],
                     state->origin.data.typed.base_addr,
                     bytes_remaining - sizeof(model_packet_t));

      TRACE_FORMAT("'non pipelined' pack-n-send. bytes to send = %zu", bytes_remaining - sizeof(model_packet_t));
      _data_model.postMultiPacket (state->origin.data.typed.state[1],
                                   complete_origin, state,
                                   task, offset,
                                   (void *) & _origin, sizeof (pami_endpoint_t),
                                   state->origin.data.typed.packet[1],
                                   bytes_remaining - sizeof(model_packet_t));
    }
  else
    {
      state->origin.data.typed.start_count = 2;
      state->origin.data.typed.bytes_remaining -= sizeof(model_packet_t) * 2;

      // pack-n-send the first full pipeline width with a data pipeline
      // completion callback.
      machine->Pack (state->origin.data.typed.packet[0],
                     state->origin.data.typed.base_addr,
                     sizeof(model_packet_t));

      TRACE_FORMAT("'pipelined' pack-n-send. bytes to send = %zu", sizeof(model_packet_t));
      _data_model.postMultiPacket (state->origin.data.typed.state[0],
                                   complete_data, state,
                                   task, offset,
                                   (void *) & _origin, sizeof (pami_endpoint_t),
                                   state->origin.data.typed.packet[0],
                                   sizeof(model_packet_t));

      // pack-n-send the second full pipeline width with a data pipeline
      // completion callback.
      machine->Pack (state->origin.data.typed.packet[1],
                     state->origin.data.typed.base_addr,
                     sizeof(model_packet_t));

      TRACE_FORMAT("'pipelined' pack-n-send. bytes to send = %zu", sizeof(model_packet_t));
      _data_model.postMultiPacket (state->origin.data.typed.state[1],
                                   complete_data, state,
                                   task, offset,
                                   (void *) & _origin, sizeof (pami_endpoint_t),
                                   state->origin.data.typed.packet[1],
                                   sizeof(model_packet_t));
    }

  TRACE_FN_EXIT();
};


template <class T_Model>
int PutOverSend<T_Model>::dispatch_data (void   * metadata,
                                         void   * payload,
                                         size_t   bytes,
                                         void   * recv_func_parm,
                                         void   * cookie)
{
  TRACE_FN_ENTER();

  PutOverSendProtocol * protocol = (PutOverSendProtocol *) recv_func_parm;
  uint8_t stack[T_Model::packet_model_payload_bytes];

  // Get target state memory from connection.
  pami_task_t task;
  size_t offset;
  PAMI_ENDPOINT_INFO(*((pami_endpoint_t *) metadata), task, offset);
  state_t * state = (state_t *) protocol->_data_model.device.getConnection (task, offset);

  TRACE_FORMAT("state->target.data.bytes_remaining = %zu", state->target.data.bytes_remaining);
  TRACE_FORMAT("state->target.data.is_contiguous   = %d", state->target.data.is_contiguous);

  const size_t bytes_to_copy = MIN(bytes, state->target.data.bytes_remaining);

  if (likely(state->target.data.is_contiguous))
    {
      TRACE_FORMAT("copy payload data to %p", state->target.data.base_addr);
      if (T_Model::read_is_required_packet_model)
        protocol->_data_model.device.read (state->target.data.base_addr, bytes_to_copy, cookie);
      else
        Core_memcpy (state->target.data.base_addr, payload, bytes_to_copy);
        
      state->target.data.base_addr =
        ((uint8_t *)state->target.data.base_addr) + bytes_to_copy;
    }
  else
    {
      if (T_Model::read_is_required_packet_model)
        {
          payload = (void *) & stack[0];
          protocol->_data_model.device.read (payload, bytes, cookie);
        }

      Type::TypeMachine * machine = (Type::TypeMachine *) state->target.data.machine;
      machine->Unpack (state->target.data.base_addr, payload, bytes_to_copy);
    }

  state->target.data.bytes_remaining -= bytes_to_copy;

  if (state->target.data.bytes_remaining == 0)
    {
      // No more data packets will be received on this connection.

      if (! state->target.data.is_contiguous)
        {
          // Destroy the target type and machine.
          Type::TypeMachine * machine = (Type::TypeMachine *) state->target.data.machine;
          machine->~TypeMachine();

          // Free the temporary serialized target type memory.
          free (state->target.data.type_buffer);
        }
      
      // Clear the connection data and prepare for the next message.
      protocol->_data_model.device.clearConnection (task, offset);

      // Return the target state object memory to the memory pool.
      protocol->_allocator.returnObject(state);

      TRACE_FORMAT("origin task = %d ... receive completed", task);
      TRACE_FN_EXIT();
      return 0;
    }

  TRACE_FORMAT("origin task = %d ... wait for more data", task);
  TRACE_FN_EXIT();
  return 0;
};



template <class T_Model>
void PutOverSend<T_Model>::complete_data (pami_context_t   context,
                                          void           * cookie,
                                          pami_result_t    result)
{
  TRACE_FN_ENTER();

  state_t * state = (state_t *) cookie;
  PutOverSendProtocol * protocol = (PutOverSendProtocol *) state->origin.protocol;

  // Determine which pipline completion invoked this callback, and increment the
  // start count for the next pipeline completion.
  const size_t which = (state->origin.data.typed.start_count++) & 0x01;

  const size_t bytes_remaining = state->origin.data.typed.bytes_remaining;

  Type::TypeMachine * machine =
    (Type::TypeMachine *) state->origin.data.typed.machine;

  TRACE_FORMAT("bytes_remaining = %zu", bytes_remaining);
  if (bytes_remaining <= sizeof(model_packet_t))
    {
      // pack-n-send the last pipeline width with a _send_ completion callback.
      machine->Pack (state->origin.data.typed.packet[which],
                     state->origin.data.typed.base_addr,
                     bytes_remaining);

      TRACE_STRING("send last pipeline with a completion callback.");
      protocol->_data_model.postMultiPacket (state->origin.data.typed.state[which],
                                             complete_origin,
                                             (void *) state,
                                             state->origin.target_task,
                                             state->origin.target_offset,
                                             (void *) & protocol->_origin,
                                             sizeof (pami_endpoint_t),
                                             state->origin.data.typed.packet[which],
                                             bytes_remaining);
    }
  else if (bytes_remaining <= (sizeof(model_packet_t) << 1))
    {
      // pack-n-send this next-to-last full pipeline width _without_ a
      // completion callback. This is allowed because the next pipeline width is
      // also the last pipeline width and this last pipeline width will specify
      // a send completion callback (see if clause above).
      machine->Pack (state->origin.data.typed.packet[which],
                     state->origin.data.typed.base_addr,
                     sizeof(model_packet_t));

      state->origin.data.typed.bytes_remaining -= sizeof(model_packet_t);

      TRACE_STRING("send next-to-last pipeline without a completion callback.");
      protocol->_data_model.postMultiPacket (state->origin.data.typed.state[which],
                                             NULL,
                                             (void *) NULL,
                                             state->origin.target_task,
                                            state->origin.target_offset,
                                          (void *) & protocol->_origin,
                                          sizeof (pami_endpoint_t),
                                          state->origin.data.typed.packet[which],
                                          sizeof(model_packet_t));
    }
  else
    {
      // pack-n-send a full pipeline width with a data pipeline completion callback.
      machine->Pack (state->origin.data.typed.packet[which],
                     state->origin.data.typed.base_addr,
                     sizeof(model_packet_t));

      state->origin.data.typed.bytes_remaining -= sizeof(model_packet_t);

      TRACE_STRING("send next pipeline with a completion callback.");
      protocol->_data_model.postMultiPacket (state->origin.data.typed.state[which],
                                             complete_data,
                                          (void *) state,
                                          state->origin.target_task,
                                          state->origin.target_offset,
                                          (void *) & protocol->_origin,
                                          sizeof (pami_endpoint_t),
                                          state->origin.data.typed.packet[which],
                                          sizeof(model_packet_t));

    }

  TRACE_FN_EXIT();
  return;
};


#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG
#endif // __p2p_protocols_put_PutOverSend_data_impl_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
