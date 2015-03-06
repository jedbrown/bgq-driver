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
/// \file p2p/protocols/put/PutOverSend_remotefn_impl.h
/// \brief ??
///
#ifndef __p2p_protocols_put_PutOverSend_remotefn_impl_h__
#define __p2p_protocols_put_PutOverSend_remotefn_impl_h__

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
inline void PutOverSend<T_Model>::send_remotefn (pami_task_t           task,
                                                 size_t                offset,
                                                 pami_event_function   fn,
                                                 void                * cookie)
{
  TRACE_FN_ENTER();

  metadata_remotefn_t metadata;
  metadata.remote_fn = fn;
  metadata.cookie    = cookie;
  metadata.invoke    = false;

  // It is safe to place this iovec array on the stack as it is only
  // used by the 'immediate' post packet function below.
  struct iovec iov[1] = {{&metadata, sizeof(metadata_remotefn_t)}};

  bool result =
    _remotefn_model.postPacket (task, offset, &_origin, sizeof(pami_endpoint_t), iov);

  if (result != true)
    {
      // The 'immediate' post packet failed. Allocate memory to
      // maintain the origin state of the ack. The state object is
      // deallocated after the ack request packet is injected.
      state_t * state = (state_t *) _allocator.allocateObject();

      state->remotefn.protocol = this;
      state->remotefn.metadata = metadata;

      _remotefn_model.postPacket (state->remotefn.state,
                                  complete_remotefn, (void *) state,
                                  task, offset,
                                  &_origin, sizeof(pami_endpoint_t),
                                  (void *) &(state->remotefn.metadata),
                                  sizeof(metadata_remotefn_t));
    }

  TRACE_FN_EXIT();
}

template <class T_Model>
int PutOverSend<T_Model>::dispatch_remotefn (void   * metadata,
                                             void   * payload,
                                             size_t   bytes,
                                             void   * recv_func_parm,
                                             void   * cookie)
{
  TRACE_FN_ENTER();

  PutOverSendProtocol * protocol = (PutOverSendProtocol *) recv_func_parm;
  uint8_t stack[T_Model::packet_model_payload_bytes];

  if (T_Model::read_is_required_packet_model)
    {
      payload = (void *) & stack[0];
      protocol->_remotefn_model.device.read (payload, bytes, cookie);
    }

  metadata_remotefn_t * mdata = (metadata_remotefn_t *) payload;

  if (mdata->invoke)
    {
      // This is a 'pong' ack message .. invoke the callback function
      // and return.
      mdata->remote_fn (protocol->_context, mdata->cookie, PAMI_SUCCESS);

      TRACE_FN_EXIT();
      return 0;
    }

  pami_task_t task;
  size_t offset;
  PAMI_ENDPOINT_INFO(*((pami_endpoint_t*)metadata), task, offset);

  // Send the remotefn metadata back to the origin, however change the value of
  // the 'invoke' field so that the origin dispatch will invoke the event function.
  mdata->invoke = true;

  // Attempt an 'immediate' packet post. If this fails, then state memory must
  // be allocated for this 'pong' remotefn message.
  struct iovec iov[1] = {{payload, sizeof(metadata_remotefn_t)}};
  bool result = protocol->_remotefn_model.postPacket(task, offset, NULL, 0, iov);

  if (result != true)
    {
      // Allocate memory to maintain the state of the send.
      state_t * state = (state_t *) protocol->_allocator.allocateObject();

      state->remotefn.protocol = protocol;
      state->remotefn.metadata = *mdata;

      protocol->_remotefn_model.postPacket (state->remotefn.state,
                                            complete_remotefn, (void *) state,
                                            task, offset,
                                            NULL, 0,
                                            (void *) &(state->remotefn.metadata),
                                            sizeof(metadata_remotefn_t));
    }

  TRACE_FN_EXIT();
  return 0;
};

template <class T_Model>
void PutOverSend<T_Model>::complete_remotefn (pami_context_t   context,
                                              void           * cookie,
                                              pami_result_t    result)
{
  TRACE_FN_ENTER();

  state_t * state = (state_t *) cookie;
  state->remotefn.protocol->_allocator.returnObject (cookie);

  TRACE_FN_EXIT();
}


#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG
#endif // __p2p_protocols_put_PutOverSend_remotefn_impl_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
