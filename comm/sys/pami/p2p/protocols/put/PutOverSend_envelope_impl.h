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
/// \file p2p/protocols/put/PutOverSend_typed_envelope_impl.h
/// \brief ??
///
#ifndef __p2p_protocols_put_PutOverSend_typed_envelope_impl_h__
#define __p2p_protocols_put_PutOverSend_typed_envelope_impl_h__

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
inline void PutOverSend<T_Model>::send_envelope (pami_put_simple_t * parameters,
                                                 pami_task_t         task,
                                                 size_t              offset,
                                                 state_t           * state)
{
  TRACE_FN_ENTER();

  // Send the "destination" virtual address and data length, the origin endpoint
  // to establish an eager-style connection, and the serialized target type size
  // (zero represents 'contiguous') in the payload of an envelope packet.
    
  state->origin.envelope.header.metadata.remote_addr =
    (uintptr_t) parameters->addr.remote;
    
  state->origin.envelope.header.metadata.data_bytes =
    parameters->rma.bytes;
    
  state->origin.envelope.header.metadata.origin = _origin;
  state->origin.envelope.header.metadata.type_bytes = 0;
  
  _header_model.postPacket (state->origin.envelope.header.state,
                            NULL, NULL,
                            task, offset,
                            (void *) NULL, 0,
                            (void *) &(state->origin.envelope.header.metadata),
                            sizeof (metadata_header_t));

  TRACE_FN_EXIT();
};

template <class T_Model>
inline void PutOverSend<T_Model>::send_envelope (pami_put_typed_t * parameters,
                                                 pami_task_t        task,
                                                 size_t             offset,
                                                 state_t          * state)
{
  TRACE_FN_ENTER();

  // Send the "destination" virtual address and data length, the origin endpoint
  // to establish an eager-style connection, and the serialized target type size
  // in the payload of an envelope packet.
    
  state->origin.envelope.header.metadata.remote_addr =
    (uintptr_t) parameters->addr.remote;

  state->origin.envelope.header.metadata.data_bytes =
    parameters->rma.bytes;

  state->origin.envelope.header.metadata.origin = _origin;

  Type::TypeCode * remote_type = (Type::TypeCode *) parameters->type.remote;
  state->origin.envelope.header.metadata.type_bytes = remote_type->GetCodeSize();

  state->origin.envelope.header.metadata.machine_bytes = Type::TypeMachine::cursor_size * remote_type->GetDepth();

  _header_model.postPacket (state->origin.envelope.header.state,
                              NULL, NULL,
                              task, offset,
                              NULL, 0,
                              (void *) &(state->origin.envelope.header.metadata),
                              sizeof (metadata_header_t));


  _type_model.postMultiPacket (state->origin.envelope.type.state,
                               NULL, NULL,
                               task, offset,
                               (void *) & _origin, sizeof(pami_endpoint_t),
                               remote_type->GetCodeAddr(),
                               remote_type->GetCodeSize());

  TRACE_FN_EXIT();
};

template <class T_Model>
int PutOverSend<T_Model>::dispatch_header (void   * metadata,
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
      protocol->_header_model.device.read (payload, bytes, cookie);
    }

  metadata_header_t * mdata = (metadata_header_t *) payload;
  
  TRACE_FORMAT("metadata.remote_addr   = %p", (void *) mdata->remote_addr);
  TRACE_FORMAT("metadata.data_bytes    = %zu", mdata->data_bytes);
  TRACE_FORMAT("metadata.origin        = 0x%08x", mdata->origin);
  TRACE_FORMAT("metadata.type_bytes    = %zu", mdata->type_bytes);
  TRACE_FORMAT("metadata.machine_bytes = %zu", mdata->machine_bytes);

  // Allocate target state memory and set connection.
  state_t * state = (state_t *) protocol->_allocator.allocateObject();
  pami_task_t task;
  size_t offset;
  PAMI_ENDPOINT_INFO(mdata->origin, task, offset);
  protocol->_header_model.device.setConnection ((void *) state, task, offset);
  
  state->target.envelope.type.machine_bytes    = mdata->machine_bytes;
  state->target.envelope.type.type_bytes       = mdata->type_bytes;
  state->target.envelope.type.bytes_remaining  = mdata->type_bytes;
  state->target.data.bytes_remaining           = mdata->data_bytes;
  state->target.data.base_addr                 = (void *) mdata->remote_addr;
  state->target.data.is_contiguous             = (mdata->type_bytes == 0);
  
  if (! state->target.data.is_contiguous)
    {
      // Allocate memory to hold the serialized target type and type machine
      state->target.data.type_buffer = malloc (mdata->type_bytes +
                                               mdata->machine_bytes);
    }
  
  TRACE_FN_EXIT();
  return 0;
};

template <class T_Model>
int PutOverSend<T_Model>::dispatch_type (void   * metadata,
                                         void   * payload,
                                         size_t   bytes,
                                         void   * recv_func_parm,
                                         void   * cookie)
{
  TRACE_FN_ENTER();

  PutOverSendProtocol * protocol = (PutOverSendProtocol *) recv_func_parm;

  // Get target state memory from connection.
  pami_task_t task;
  size_t offset;
  PAMI_ENDPOINT_INFO(*((pami_endpoint_t *) metadata), task, offset);
  state_t * state = (state_t *) protocol->_type_model.device.getConnection (task, offset);

  const size_t bytes_to_copy = MIN(bytes, state->target.envelope.type.bytes_remaining);
  const size_t data_offset = state->target.envelope.type.type_bytes - state->target.envelope.type.bytes_remaining;
  void * ptr = (void *)((uint8_t *) state->target.data.type_buffer + data_offset);

  if (T_Model::read_is_required_packet_model)
    protocol->_type_model.device.read (ptr, bytes_to_copy, cookie);
  else
    Core_memcpy (ptr, payload, bytes_to_copy);

  state->target.envelope.type.bytes_remaining -= bytes_to_copy;
  
  if (state->target.envelope.type.bytes_remaining == 0)
    {
      // Construct the type machine.
      Type::TypeCode * machine = (Type::TypeCode *) state->target.data.machine;
      new (machine) Type::TypeMachine (state->target.data.type_buffer,
                                       state->target.envelope.type.type_bytes,
                                       ((uint8_t *) state->target.data.type_buffer) + state->target.envelope.type.type_bytes,
                                       state->target.envelope.type.machine_bytes);
    }

  TRACE_FN_EXIT();
  return 0;
};

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG
#endif // __p2p_protocols_put_PutOverSend_typed_envelope_impl_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
