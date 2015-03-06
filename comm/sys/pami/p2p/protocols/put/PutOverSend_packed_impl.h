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
/// \file p2p/protocols/put/PutOverSend_packed_impl.h
/// \brief ??
///
#ifndef __p2p_protocols_put_PutOverSend_packed_impl_h__
#define __p2p_protocols_put_PutOverSend_packed_impl_h__

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
inline void PutOverSend<T_Model>::send_packed (pami_put_simple_t * parameters,
                                               pami_task_t         task,
                                               size_t              offset,
                                               state_t           * state)
{
  TRACE_FN_ENTER();

  if (sizeof(metadata_packed_t) <= T_Model::packet_model_metadata_bytes)
    {
      // Send the "destination" virtual address and data length in the
      // packet header.

      metadata_packed_t metadata;
      metadata.remote_addr = (uintptr_t) parameters->addr.remote;
      metadata.data_bytes  = parameters->rma.bytes;
      metadata.type_bytes  = 0; // no serialized target type == contiguous transfer
      
      _packed_model.postPacket (state->origin.packed.state,
                                complete_origin, state,
                                task, offset,
                                (void *) & metadata,
                                sizeof (metadata_packed_t),
                                parameters->addr.local,
                                parameters->rma.bytes);
    }
  else
    {
      // Send the "destination" virtual address, data length, and source data
      // in the packet payload.
      
      state->origin.packed.metadata.remote_addr = (uintptr_t) parameters->addr.remote;
      state->origin.packed.metadata.data_bytes  = parameters->rma.bytes;
      state->origin.packed.metadata.type_bytes  = 0; // no serialized target type == contiguous transfer
      
      struct iovec iov[2];
      iov[0].iov_base = &(state->origin.packed.metadata);
      iov[0].iov_len  = sizeof(metadata_packed_t);
      iov[1].iov_base = parameters->addr.local;
      iov[1].iov_len  = parameters->rma.bytes;

      _packed_model.postPacket (state->origin.packed.state,
                                complete_origin, state,
                                task, offset,
                                (void *) NULL, 0,
                                iov);
    }

  TRACE_FN_EXIT();
};

template <class T_Model>
inline void PutOverSend<T_Model>::send_packed (pami_put_typed_t * parameters,
                                               pami_task_t        task,
                                               size_t             offset,
                                               state_t          * state)
{
  TRACE_FN_ENTER();

  Type::TypeCode * remote_type = (Type::TypeCode *) parameters->type.remote;

  if (sizeof(metadata_packed_t) <= T_Model::packet_model_metadata_bytes)
    {
      // Send the "destination" virtual address, data length, and serialized
      // target type size in the packet header.
      //
      // Send the serialized target type and the packed source data in the
      // packet payload.

      metadata_packed_t metadata;
      metadata.remote_addr = (uintptr_t) parameters->addr.remote;
      metadata.data_bytes  = parameters->rma.bytes;
      metadata.type_bytes  = remote_type->GetCodeSize();

      // Pack the data into a temporary packet payload
      Type::TypeMachine machine (remote_type);
      machine.Pack ((uint8_t *) state->origin.packed.packet,
                    parameters->addr.local,
                    parameters->rma.bytes);                   

      struct iovec iov[2];
      iov[0].iov_base = remote_type->GetCodeAddr();
      iov[0].iov_len  = remote_type->GetCodeSize();
      iov[1].iov_base = (void *) state->origin.packed.packet;
      iov[1].iov_len  = parameters->rma.bytes;
      
      _packed_model.postPacket (state->origin.packed.state,
                                complete_origin, state,
                                task, offset,
                                (void *) & metadata,
                                sizeof (metadata_packed_t),
                                iov);
    }
  else
    {
      // Send the "destination" virtual address, data length, serialized target
      // type size, serialized target type and the packed source data in the
      // packet payload.
      
      state->origin.packed.metadata.remote_addr = (uintptr_t) parameters->addr.remote;
      state->origin.packed.metadata.data_bytes  = parameters->rma.bytes;
      state->origin.packed.metadata.type_bytes  = remote_type->GetCodeSize();
      
      struct iovec iov[3];
      iov[0].iov_base = &(state->origin.packed.metadata);
      iov[0].iov_len  = sizeof(metadata_packed_t);
      iov[1].iov_base = remote_type->GetCodeAddr();
      iov[1].iov_len  = remote_type->GetCodeSize();
      iov[2].iov_base = (void *) state->origin.packed.packet;
      iov[2].iov_len  = parameters->rma.bytes;

      _packed_model.postPacket (state->origin.packed.state,
                                complete_origin, state,
                                task, offset,
                                (void *) NULL, 0,
                                iov);
    }

  TRACE_FN_EXIT();
};

template <class T_Model>
int PutOverSend<T_Model>::dispatch_packed (void   * metadata,
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
      protocol->_packed_model.device.read (payload, bytes, cookie);
    }
    
  metadata_packed_t * mdata           = NULL;
  uint8_t           * serialized_type = NULL;
  
  if (sizeof(metadata_packed_t) <= T_Model::packet_model_metadata_bytes)
    {
      mdata = (metadata_packed_t *) metadata;
      if (mdata->type_bytes == 0) // => contiguous data transfer
        {
          Core_memcpy ((void *) mdata->remote_addr, payload, mdata->data_bytes);
          TRACE_FN_EXIT();
          return 0;
        }
      serialized_type = (uint8_t *) payload;
    }
  else
    {
      mdata = (metadata_packed_t *) payload;
      if (mdata->type_bytes == 0) // => contiguous data transfer
        {
          Core_memcpy ((void *) mdata->remote_addr, (void *) (mdata+1), mdata->data_bytes);
          TRACE_FN_EXIT();
          return 0;
        }
      serialized_type = (uint8_t *) (mdata+1);
    }
    
  // De-serialize the type and construct a type machine.
  Type::TypeCode type_obj(serialized_type, mdata->type_bytes);
  Type::TypeMachine machine (&type_obj);

  // Unpack the source data into the destination type.  
  void * packed_data = serialized_type + mdata->type_bytes;
  machine.Unpack ((void *) mdata->remote_addr, packed_data, mdata->data_bytes);
    
  TRACE_FN_EXIT();
  return 0;
};

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG
#endif // __p2p_protocols_put_PutOverSend_packed_impl_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
