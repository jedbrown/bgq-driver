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
/// \file p2p/protocols/send/eager/EagerSimple_data_impl.h
/// \brief Eager protocol 'data flow' implementation.
///
#ifndef __p2p_protocols_send_eager_EagerSimple_data_impl_h__
#define __p2p_protocols_send_eager_EagerSimple_data_impl_h__

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
template <bool T_ContiguousCopy>
inline pami_result_t EagerSimple<T_Model, T_Option>::send_data (eager_state_t     * state,
                                                                pami_task_t         task,
                                                                size_t              offset,
                                                                pami_send_typed_t * parameters)
{
  TRACE_FN_ENTER();

  if (T_ContiguousCopy == true)
    {
      _data_model.postMultiPacket (state->origin.eager.data.state[0],
                                   send_complete,
                                   (void *) state,
                                   task,
                                   offset,
                                   (void *) & _origin,
                                   sizeof (pami_endpoint_t),
                                   parameters->send.data.iov_base,
                                   parameters->send.data.iov_len);
    }
  else
    {
      Type::TypeCode * type = (Type::TypeCode *) parameters->typed.type;
      PAMI_assert_debugf(type != NULL, "parameters->typed.type == NULL !");

#ifdef ERROR_CHECKS

      if (! type->IsCompleted())
        {
          //RETURN_ERR_PAMI(PAMI_INVAL, "Using an incompleted type.");
          TRACE_FN_EXIT();
          return PAMI_ERROR;
        }

#endif

      // Construct a type machine for this transfer.
      Type::TypeMachine * machine = (Type::TypeMachine *) state->origin.eager.data.machine;
      new (machine) Type::TypeMachine (type);
      machine->SetCopyFunc (parameters->typed.data_fn, parameters->typed.data_cookie);
      machine->MoveCursor (parameters->typed.offset);

      const size_t atom_size = type->GetAtomSize();

      if (T_Model::packet_model_payload_bytes % atom_size != 0)
        {
          // partial packet data not supported.
          PAMI_abort();
          return PAMI_ERROR;
        }

      state->origin.eager.data.base_addr =
        parameters->send.data.iov_base;
        
      state->origin.eager.data.bytes_remaining =
        parameters->send.data.iov_len;

      const size_t bytes_remaining = parameters->send.data.iov_len;

      if (bytes_remaining <= sizeof(data_pipeline_t))
        {
          // "short", non-pipelined, pack-n-send
          machine->Pack (state->origin.eager.data.pipeline[0],
                         state->origin.eager.data.base_addr,
                         bytes_remaining);

          _data_model.postMultiPacket (state->origin.eager.data.state[0],
                                       send_complete,
                                       (void *) state,
                                       task,
                                       offset,
                                       (void *) & _origin,
                                       sizeof (pami_endpoint_t),
                                       state->origin.eager.data.pipeline[0],
                                       bytes_remaining);
        }
      else if (bytes_remaining <= (sizeof(data_pipeline_t) << 1))
        {
          // pack-n-send the first full pipeline width _without_ a
          // completion callback.
          machine->Pack (state->origin.eager.data.pipeline[0],
                         state->origin.eager.data.base_addr,
                         sizeof(data_pipeline_t));

          _data_model.postMultiPacket (state->origin.eager.data.state[0],
                                       NULL,
                                       (void *) NULL,
                                       task,
                                       offset,
                                       (void *) & _origin,
                                       sizeof (pami_endpoint_t),
                                       state->origin.eager.data.pipeline[0],
                                       sizeof(data_pipeline_t));

          // pack-n-send the second (and also last) pipeline width
          // with a normal data completion callback.
          machine->Pack (state->origin.eager.data.pipeline[1],
                         state->origin.eager.data.base_addr,
                         bytes_remaining - sizeof(data_pipeline_t));

          _data_model.postMultiPacket (state->origin.eager.data.state[1],
                                       send_complete,
                                       (void *) state,
                                       task,
                                       offset,
                                       (void *) & _origin,
                                       sizeof (pami_endpoint_t),
                                       state->origin.eager.data.pipeline[1],
                                       bytes_remaining - sizeof(data_pipeline_t));
        }
      else
        {
          state->origin.eager.data.start_count = 2;

          // pack-n-send the first full pipeline width with a data pipeline
          // completion callback.
          machine->Pack (state->origin.eager.data.pipeline[0],
                         state->origin.eager.data.base_addr,
                         sizeof(data_pipeline_t));

          _data_model.postMultiPacket (state->origin.eager.data.state[0],
                                       complete_data,
                                       (void *) state,
                                       task,
                                       offset,
                                       (void *) & _origin,
                                       sizeof (pami_endpoint_t),
                                       state->origin.eager.data.pipeline[0],
                                       sizeof(data_pipeline_t));

          // pack-n-send the second full pipeline width with a data pipeline
          // completion callback.
          machine->Pack (state->origin.eager.data.pipeline[1],
                         state->origin.eager.data.base_addr,
                         sizeof(data_pipeline_t));

          _data_model.postMultiPacket (state->origin.eager.data.state[1],
                                       complete_data,
                                       (void *) state,
                                       task,
                                       offset,
                                       (void *) & _origin,
                                       sizeof (pami_endpoint_t),
                                       state->origin.eager.data.pipeline[1],
                                       sizeof(data_pipeline_t));
        }
    }

  TRACE_FN_EXIT();
  return PAMI_SUCCESS;
};

template < class T_Model, configuration_t T_Option >
template <bool T_ContiguousCopy>
inline int EagerSimple<T_Model, T_Option>::dispatch_data   (void   * metadata,
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
  pami_task_t task;
  size_t offset;
  PAMI_ENDPOINT_INFO(origin, task, offset);

  TRACE_FORMAT("origin task = %d, origin offset = %zu, bytes = %zu", task, offset, bytes);

  eager_state_t * state = (eager_state_t *) eager->_data_model.device.getConnection (task, offset);

  // Number of bytes received so far.
  const size_t nbyte = state->target.data.bytes_received;

  // Number of bytes left to copy into the destination buffer
  const size_t nleft = state->target.data.bytes_total - nbyte;

  TRACE_FORMAT("bytes received so far = %zu, bytes yet to receive = %zu, total bytes to receive = %zu", state->target.data.bytes_received, nleft, state->target.data.bytes_total);

  // Copy data from the packet payload into the destination buffer
  const size_t ncopy = MIN(nleft, bytes);

  if (T_ContiguousCopy || likely(state->target.data.is_contiguous_copy_recv))
    Core_memcpy ((uint8_t *)(state->target.info.addr) + nbyte, payload, ncopy);
  else
    {
      Type::TypeMachine * machine = (Type::TypeMachine *) state->target.data.machine;
      machine->Unpack (state->target.info.addr, payload, ncopy);
    }

  state->target.data.bytes_received += ncopy;

  TRACE_FORMAT("nbyte = %zu, bytes = %zu, state->target.data.bytes_total = %zu", nbyte, bytes, state->target.data.bytes_total);

  if ((nbyte + bytes) >= state->target.data.bytes_total)
    {
      // No more data packets will be received on this connection.
      // Clear the connection data and prepare for the next message.
      eager->_data_model.device.clearConnection (task, offset);

      // No more data is to be written to the receive buffer.
      // Invoke the receive done callback.
      if (state->target.info.local_fn)
        state->target.info.local_fn (eager->_context,
                              state->target.info.cookie,
                              PAMI_SUCCESS);

      // Return the receive state object memory to the memory pool.
      eager->freeState (state);

      TRACE_FORMAT("origin task = %d ... receive completed", task);
      TRACE_FN_EXIT();
      return 0;
    }

  TRACE_FORMAT("origin task = %d ... wait for more data", task);
  TRACE_FN_EXIT();
  return 0;
};

template < class T_Model, configuration_t T_Option >
inline void EagerSimple<T_Model, T_Option>::complete_data (pami_context_t   context,
                                                           void           * cookie,
                                                           pami_result_t    result)
{
  TRACE_FN_ENTER();

  eager_state_t * state = (eager_state_t *) cookie;
  EagerSimpleProtocol * eager = (EagerSimpleProtocol *) state->origin.protocol;

  // Determine which pipline completion invoked this callback, and increment the
  // start count for the next pipeline completion.
  const size_t which = (state->origin.eager.data.start_count++) & 0x01;

  const size_t bytes_remaining = state->origin.eager.data.bytes_remaining;

  Type::TypeMachine * machine =
    (Type::TypeMachine *) state->origin.eager.data.machine;

  if (bytes_remaining <= sizeof(data_pipeline_t))
    {
      // pack-n-send the last pipeline width with a _send_ completion callback.
      machine->Pack (state->origin.eager.data.pipeline[which],
                     state->origin.eager.data.base_addr,
                     bytes_remaining);

      eager->_data_model.postMultiPacket (state->origin.eager.data.state[which],
                                          send_complete,
                                          (void *) state,
                                          state->origin.target_task,
                                          state->origin.target_offset,
                                          (void *) & eager->_origin,
                                          sizeof (pami_endpoint_t),
                                          state->origin.eager.data.pipeline[which],
                                          bytes_remaining);
    }
  else if (bytes_remaining <= (sizeof(data_pipeline_t) << 1))
    {
      // pack-n-send this next-to-last full pipeline width _without_ a
      // completion callback. This is allowed because the next pipeline width is
      // also the last pipeline width and this last pipeline width will specify
      // a send completion callback (see if clause above).
      machine->Pack (state->origin.eager.data.pipeline[which],
                     state->origin.eager.data.base_addr,
                     sizeof(data_pipeline_t));

      state->origin.eager.data.bytes_remaining -= sizeof(data_pipeline_t);

      eager->_data_model.postMultiPacket (state->origin.eager.data.state[which],
                                          NULL,
                                          (void *) NULL,
                                          state->origin.target_task,
                                          state->origin.target_offset,
                                          (void *) & eager->_origin,
                                          sizeof (pami_endpoint_t),
                                          state->origin.eager.data.pipeline[which],
                                          sizeof(data_pipeline_t));
    }
  else
    {
      // pack-n-send a full pipeline width with a data pipeline completion callback.
      machine->Pack (state->origin.eager.data.pipeline[which],
                     state->origin.eager.data.base_addr,
                     sizeof(data_pipeline_t));

      state->origin.eager.data.bytes_remaining -= sizeof(data_pipeline_t);

      eager->_data_model.postMultiPacket (state->origin.eager.data.state[which],
                                          complete_data,
                                          (void *) state,
                                          state->origin.target_task,
                                          state->origin.target_offset,
                                          (void *) & eager->_origin,
                                          sizeof (pami_endpoint_t),
                                          state->origin.eager.data.pipeline[which],
                                          sizeof(data_pipeline_t));

    }

  TRACE_FN_EXIT();
  return;
};

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif // __p2p_protocols_send_eager_EagerSimple_data_impl_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
