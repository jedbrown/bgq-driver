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
/// \file p2p/protocols/send/eager/EagerSimple_packed_impl.h
/// \brief Eager 'short' protocol implementation.
///
#ifndef __p2p_protocols_send_eager_EagerSimple_packed_impl_h__
#define __p2p_protocols_send_eager_EagerSimple_packed_impl_h__

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
inline bool EagerSimple<T_Model, T_Option>::send_packed (pami_task_t             task,
                                                         size_t                  offset,
                                                         const size_t            header_bytes,
                                                         const size_t            data_bytes,
                                                         pami_send_immediate_t * parameters)
{
  TRACE_FN_ENTER();

  // Specify the protocol metadata to send with the application
  // metadata in the packet. This metadata is copied
  // into the network by the device and, therefore, can be placed
  // on the stack.
  packed_metadata_t packed_metadata;
  packed_metadata.data_bytes   = data_bytes;
  packed_metadata.header_bytes = header_bytes;
  packed_metadata.origin       = _origin;

  TRACE_FORMAT("before _send_model.postPacket() .. parameters->header.iov_len = %zu, parameters->data.iov_len = %zu dest:%x", parameters->header.iov_len, parameters->data.iov_len, parameters->dest);

  TRACE_FORMAT("before _send_model.postPacket() .. task = %d, offset = %zu", task, offset);

  bool posted = false;

  // This branch should be resolved at compile time and optimized out.
  if (sizeof(packed_metadata_t) <= T_Model::packet_model_metadata_bytes)
    {
      // This allows the header+data iovec elements to be treated as a
      // two-element array of iovec structures, and therefore allows the
      // packet model to implement template specialization.
      array_t<struct iovec, 2> * iov = (array_t<struct iovec, 2> *) parameters;

      posted = _short_model.postPacket (task, offset,
                                        (void *) & packed_metadata,
                                        sizeof (packed_metadata_t),
                                        iov->array);
    }
  else
    {
      iovec iov[3];
      iov[0].iov_base = (void *) & packed_metadata;
      iov[0].iov_len  = sizeof (packed_metadata_t);
      iov[1].iov_base = parameters->header.iov_base;
      iov[1].iov_len  = parameters->header.iov_len;
      iov[2].iov_base = parameters->data.iov_base;
      iov[2].iov_len  = parameters->data.iov_len;

      posted = _short_model.postPacket (task, offset,
                                        NULL, 0,
                                        iov);
    }

  TRACE_FN_EXIT();
  return posted;
};


template < class T_Model, configuration_t T_Option >
inline pami_result_t EagerSimple<T_Model, T_Option>::send_packed (eager_state_t * state,
                                                                  pami_task_t     task,
                                                                  size_t          offset,
                                                                  pami_send_t   * parameters)
{
  TRACE_FN_ENTER();

  // This branch should be resolved at compile time and optimized out.
  if (sizeof(packed_metadata_t) <= T_Model::packet_model_metadata_bytes)
    {
      TRACE_FORMAT("protocol metadata (%ld bytes) fits in the packet metadata (%zu bytes)", (long)sizeof(packed_metadata_t), T_Model::packet_model_metadata_bytes);

      // Initialize the short protocol metadata on the stack to copy
      // into the packet metadata.
      packed_metadata_t metadata;
      metadata.data_bytes   = parameters->send.data.iov_len;
      metadata.header_bytes = parameters->send.header.iov_len;
      metadata.origin       = _origin;

      array_t<iovec, 2> * resized = (array_t<iovec, 2> *) & (parameters->send.header);

      _short_model.postPacket (state->origin.packed.state,
                               send_complete, (void *) state,
                               task, offset,
                               (void *) & metadata,
                               sizeof (packed_metadata_t),
                               resized->array);
    }
  else
    {
      TRACE_STRING("protocol metadata does not fit in the packet metadata");

      // Initialize the short protocol metadata to be sent with the
      // application metadata and application data in the packet payload.
      state->origin.packed.metadata.data_bytes   = parameters->send.data.iov_len;
      state->origin.packed.metadata.header_bytes = parameters->send.header.iov_len;
      state->origin.packed.metadata.origin       = _origin;

      state->origin.packed.v[0].iov_base = (void *) & (state->origin.packed.metadata);
      state->origin.packed.v[0].iov_len  = sizeof (packed_metadata_t);
      state->origin.packed.v[1].iov_base = parameters->send.header.iov_base;
      state->origin.packed.v[1].iov_len  = parameters->send.header.iov_len;
      state->origin.packed.v[2].iov_base = parameters->send.data.iov_base;
      state->origin.packed.v[2].iov_len  = parameters->send.data.iov_len;

      _short_model.postPacket (state->origin.packed.state,
                               send_complete, (void *) state,
                               task, offset,
                               NULL, 0,
                               &state->origin.packed.v, 3);
    }

  TRACE_FN_EXIT();
  return PAMI_SUCCESS;
};

template < class T_Model, configuration_t T_Option >
inline pami_result_t EagerSimple<T_Model, T_Option>::send_packed (eager_state_t     * state,
                                                                  pami_task_t         task,
                                                                  size_t              offset,
                                                                  pami_send_typed_t * parameters)
{
  TRACE_FN_ENTER();

  // Construct a type machine for this transfer.
  Type::TypeCode * type = (Type::TypeCode *) parameters->typed.type;
  Type::TypeMachine machine (type);
  machine.SetCopyFunc (parameters->typed.data_fn, parameters->typed.data_cookie);
  machine.MoveCursor (parameters->typed.offset);

  struct iovec iov[1];
  iov[0].iov_base = (void *) state->origin.packed.packet;
  iov[0].iov_len  = parameters->send.header.iov_len + parameters->send.data.iov_len;

  // This branch should be resolved at compile time and optimized out.
  if (sizeof(packed_metadata_t) <= T_Model::packet_model_metadata_bytes)
    {
      TRACE_FORMAT("protocol metadata (%ld bytes) fits in the packet header (%zu bytes)", (long)sizeof(packed_metadata_t), T_Model::packet_model_metadata_bytes);

      // Initialize the short protocol metadata on the stack to copy
      // into the packet metadata.
      packed_metadata_t metadata;
      metadata.data_bytes   = parameters->send.data.iov_len;
      metadata.header_bytes = parameters->send.header.iov_len;
      metadata.origin       = _origin;

      TRACE_FORMAT("protocol metadata: origin = 0x%08x, header_bytes = %d, data_bytes = %d", metadata.origin, metadata.header_bytes, metadata.data_bytes);
      TRACE_HEXDATA(&metadata, sizeof(packed_metadata_t));

      // Pack the header and data into a temporary packet payload
      uint8_t * ptr = (uint8_t *) state->origin.packed.packet;
      memcpy (ptr, parameters->send.header.iov_base, parameters->send.header.iov_len);
      ptr += parameters->send.header.iov_len;
      machine.Pack (ptr, parameters->send.data.iov_base, parameters->send.data.iov_len);

      TRACE_HEXDATA(state->origin.packed.packet, iov[0].iov_len);

      _short_model.postPacket (state->origin.packed.state,
                               send_complete, (void *) state,
                               task, offset,
                               (void *) & metadata,
                               sizeof (packed_metadata_t),
                               iov);
    }
  else
    {
      TRACE_FORMAT("protocol metadata (%ld bytes) does not fit in the packet header (%zu bytes)", (long)sizeof(packed_metadata_t), T_Model::packet_model_metadata_bytes);

      // Initialize the metadata, to be sent with the header and data, in the
      // temporary packet payload.
      packed_metadata_t * mdata = (packed_metadata_t *) state->origin.packed.packet;
      mdata->data_bytes   = parameters->send.data.iov_len;
      mdata->header_bytes = parameters->send.header.iov_len;
      mdata->origin       = _origin;

      // Pack the header and data into the temporary packet payload
      uint8_t * ptr = (uint8_t *) (mdata + 1);
      memcpy (ptr, parameters->send.header.iov_base, parameters->send.header.iov_len);
      ptr += parameters->send.header.iov_len;
      machine.Pack (ptr, parameters->send.data.iov_base, parameters->send.data.iov_len);

      iov[0].iov_len  += sizeof(packed_metadata_t);

      TRACE_HEXDATA(state->origin.packed.packet, iov[0].iov_len);

      _short_model.postPacket (state->origin.packed.state,
                               send_complete, (void *) state,
                               task, offset,
                               NULL, 0,
                               iov);
    }

  TRACE_FN_EXIT();
  return PAMI_SUCCESS;
};



template < class T_Model, configuration_t T_Option >
inline int EagerSimple<T_Model, T_Option>::dispatch_packed (void   * metadata,
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

  packed_metadata_t * packed_metadata;
  void * header;

  // This branch should be resolved at compile time and optimized out.
  if (sizeof(packed_metadata_t) > T_Model::packet_model_metadata_bytes)
    {
      TRACE_FORMAT("protocol metadata (%ld bytes) is in the packet header (%zu bytes)", (long)sizeof(packed_metadata_t), T_Model::packet_model_metadata_bytes);
      packed_metadata = (packed_metadata_t *) payload;
      header = (void *) (packed_metadata + 1);
    }
  else
    {
      TRACE_FORMAT("protocol metadata (%ld bytes) is in the packet payload (packet header is %zu bytes)",(long) sizeof(packed_metadata_t), T_Model::packet_model_metadata_bytes);
      packed_metadata = (packed_metadata_t *) metadata;
      header = payload;
    }

  void * data = (void *) (((uint8_t *) header) + packed_metadata->header_bytes);

  TRACE_FORMAT("protocol metadata: origin = 0x%08x, header_bytes = %d, data_bytes = %d", packed_metadata->origin, packed_metadata->header_bytes, packed_metadata->data_bytes);

  // Invoke the registered dispatch function.
  if (T_Option & RECV_IMMEDIATE_FORCEOFF)
    {
      pami_recv_t info = {0};
      eager->_dispatch_fn (eager->_context,               // Communication context
                           eager->_cookie,                // Dispatch cookie
                           header,                        // Application metadata
                           packed_metadata->header_bytes, // Application metadata bytes
                           data,                          // Application data
                           packed_metadata->data_bytes,   // Application data bytes
                           packed_metadata->origin,       // Origin endpoint for the transfer
                           &info);                        // Asynchronous receive information

      TRACE_FORMAT("packed_metadata->data bytes = %d, info.type = %p, info.local_fn = %p, info.cookie = %p", packed_metadata->data_bytes, info.type, info.local_fn, info.cookie);

      if (likely(packed_metadata->data_bytes > 0))
        {
          if ((info.type == PAMI_TYPE_BYTE) && (info.data_fn == PAMI_DATA_COPY))
            {
              Core_memcpy ((uint8_t *) info.addr, data, packed_metadata->data_bytes);
            }
          else
            {
              Type::TypeCode * type = (Type::TypeCode *) info.type;
              PAMI_assert_debugf(type != NULL, "info.type == NULL !");

#ifdef ERROR_CHECKS

              if (! type->IsCompleted())
                {
                  //RETURN_ERR_PAMI(PAMI_INVAL, "Using an incompleted type.");
                  if (info.local_fn != NULL)
                    info.local_fn (eager->_context, info.cookie, PAMI_INVAL);

                  TRACE_FN_EXIT();
                  return 1;
                }

#endif

              // Use a type machine to unpack the source data.
              Type::TypeMachine machine(type);
              machine.SetCopyFunc (info.data_fn, info.data_cookie);
              machine.MoveCursor (info.offset);
              machine.Unpack (info.addr, data, packed_metadata->data_bytes);
            }
        }

      if (likely(info.local_fn != NULL))
        info.local_fn (eager->_context, info.cookie, PAMI_SUCCESS);
    }
  else
    {
      eager->_dispatch_fn (eager->_context,               // Communication context
                           eager->_cookie,                // Dispatch cookie
                           header,                        // Application metadata
                           packed_metadata->header_bytes, // Application metadata bytes
                           data,                          // Application data
                           packed_metadata->data_bytes,   // Application data bytes
                           packed_metadata->origin,       // Origin endpoint for the transfer
                           (pami_recv_t *) NULL);         // Synchronous receive
    }

  TRACE_FN_EXIT();
  return 0;
};

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif // __p2p_protocols_send_eager_EagerSimple_packed_impl_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
