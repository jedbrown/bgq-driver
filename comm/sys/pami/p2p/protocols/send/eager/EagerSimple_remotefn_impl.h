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
/// \file p2p/protocols/send/eager/EagerSimple_remotefn_impl.h
/// \brief Eager 'remote completion event function' protocol implementation.
///
#ifndef __p2p_protocols_send_eager_EagerSimple_remotefn_impl_h__
#define __p2p_protocols_send_eager_EagerSimple_remotefn_impl_h__

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
inline void EagerSimple<T_Model, T_Option>::send_remotefn (pami_task_t           task,
                                                           size_t                offset,
                                                           pami_event_function   fn,
                                                           void                * cookie)
{
  TRACE_FN_ENTER();
  ack_metadata_t metadata;
  metadata.remote_fn = fn;
  metadata.cookie = cookie;
  metadata.context = _context;
  metadata.invoke = false;

  struct iovec iov[1] = {{&metadata, sizeof(ack_metadata_t)}};
  bool result =
    _ack_model.postPacket (task, offset, &_origin, sizeof(pami_endpoint_t), iov);

  if (result != true)
    {
      // Allocate memory to maintain the state of the send.
      eager_state_t * state = allocateState ();

      state->origin.protocol = this;
      state->origin.ack.metadata = metadata;

      _ack_model.postPacket (state->origin.ack.state,
                             complete_remotefn, (void *) state,
                             task, offset,
                             &_origin, sizeof(pami_endpoint_t),
                             (void *) &(state->origin.ack.metadata),
                             sizeof(ack_metadata_t));
    }

  TRACE_FN_EXIT();
}

template < class T_Model, configuration_t T_Option >
inline int EagerSimple<T_Model, T_Option>::dispatch_remotefn (void   * metadata,
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

  ack_metadata_t * ack = (ack_metadata_t *) payload;
  TRACE_FORMAT("ack->remote_fn = %p, ack->cookie = %p, ack->context = %p, ack->invoke = %d", ack->remote_fn, ack->cookie, ack->context, ack->invoke);

  if (ack->invoke)
    {
      // This is a 'pong' ack message .. invoke the callback function
      // and return.
      ack->remote_fn (ack->context, ack->cookie, PAMI_SUCCESS);
      TRACE_FN_EXIT();
      return 0;
    }

  pami_task_t task;
  size_t offset;
  PAMI_ENDPOINT_INFO(*((pami_endpoint_t*)metadata), task, offset);

  // Attempt an 'immediate' packet post. If this fails, then a send
  // state must be allocated for this 'pong' ack message.
  ack->invoke = true;
  struct iovec iov[1] = {{payload, sizeof(ack_metadata_t)}};
  bool result = eager->_ack_model.postPacket(task, offset, NULL, 0, iov);

  if (unlikely(result != true))
    {
      TRACE_STRING("immediate post failed. Allocate state memory for non-blocking send.");
      // Allocate memory to maintain the state of the send.
      eager_state_t * state = eager->allocateState ();

      state->origin.protocol = eager;
      state->origin.ack.metadata.remote_fn = ack->remote_fn;
      state->origin.ack.metadata.cookie  = ack->cookie;
      state->origin.ack.metadata.context = ack->context;
      state->origin.ack.metadata.invoke  = true;

      eager->_ack_model.postPacket (state->origin.ack.state,
                                    complete_remotefn, (void *) state,
                                    task, offset,
                                    NULL, 0,
                                    &(state->origin.ack.metadata),
                                    sizeof(ack_metadata_t));
    }

  TRACE_FN_EXIT();
  return 0;
};

template < class T_Model, configuration_t T_Option >
inline void EagerSimple<T_Model, T_Option>::complete_remotefn (pami_context_t   context,
                                                               void           * cookie,
                                                               pami_result_t    result)
{
  TRACE_FN_ENTER();

  eager_state_t * state = (eager_state_t *) cookie;
  EagerSimpleProtocol * eager = (EagerSimpleProtocol *) state->origin.protocol;
  eager->freeState (state);

  TRACE_FN_EXIT();
}

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif // __p2p_protocols_send_eager_EagerSimple_remotefn_impl_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
