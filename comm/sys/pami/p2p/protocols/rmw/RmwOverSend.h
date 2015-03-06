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
/// \file p2p/protocols/rmw/RmwOverSend.h
/// \brief RMW protocol devices that implement the 'packet' interface.
///
/// The RMW class defined in this file uses C++ templates
/// and the "packet" device interface which also uses C++ templates.
///
/// C++ templates require all source code to be #include'd from a header file
/// which can result in a single very large file.
///
#ifndef __p2p_protocols_rmw_RmwOverSend_h__
#define __p2p_protocols_rmw_RmwOverSend_h__

#include <string.h>
#include <stdint.h>

#include "pami.h"

#include "components/devices/PacketInterface.h"
#include "components/memory/MemoryAllocator.h"
#include "math/Memcpy.x.h"
#include "p2p/protocols/Rmw.h"

#include "util/trace.h"
#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  namespace Protocol
  {
    namespace Rmw
    {
      template <class T_Model>
      class RmwOverSend : public Rmw
      {
        protected:

          typedef RmwOverSend<T_Model> RmwOverSendProtocol;

          typedef uint8_t msg_t[T_Model::packet_model_state_bytes];

          typedef struct
          {
            void            * dst;
            pami_atomic_t     operation;
            pami_endpoint_t   origin; // only if 'fetch'
            uintptr_t         state;  // only if 'fetch', -> state_t
          } envelope_t;

          typedef union
          {
            struct
            {
              msg_t                 msg;
              RmwOverSendProtocol * protocol;
              pami_event_function   done_fn;
              void                * cookie;

              envelope_t            envelope;
              struct iovec          iov[3]; // 0:envelope, 1:value, 2:test

              void                * local;
            } origin;

            struct
            {
              msg_t                 msg;
              RmwOverSendProtocol * protocol;

              struct
              {
                uintptr_t           metadata;
                uint32_t            payload_32b;
                uint64_t            payload_64b;
              } ack;

            } target;

          } state_t;

        public:

          template <class T_Device, class T_MemoryManager>
          static RmwOverSend * generate (T_Device        & device,
                                         T_MemoryManager * mm)
          {
            TRACE_FN_ENTER();

            void * protocol = NULL;
            pami_result_t result = mm->memalign((void **)&protocol, 16, sizeof(RmwOverSend));
            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for rmw-over-send protocol");
            new (protocol) RmwOverSend (device);

            TRACE_FN_EXIT();
            return (RmwOverSend *) protocol;
          };

          template <class T_Device>
          inline RmwOverSend (T_Device & device) :
              _ack_32b_model (device),
              _ack_64b_model (device),
              _signed_int_model (device),
              _unsigned_int_model (device),
              _signed_long_model (device),
              _unsigned_long_model (device),
              _signed_long_long_model (device),
              _unsigned_long_long_model (device)
          {
            TRACE_FN_ENTER();

            TRACE_FN_EXIT();
          }

          pami_result_t initialize (size_t          dispatch,
                                    pami_endpoint_t origin,
                                    pami_context_t  context)
          {
            TRACE_FN_ENTER();

            // ----------------------------------------------------------------
            // Compile-time assertions (begin)
            // ----------------------------------------------------------------

            // This protocol only works with reliable networks.
            COMPILE_TIME_ASSERT(T_Model::reliable_packet_model == true);

            // This protocol only works with deterministic models.
            COMPILE_TIME_ASSERT(T_Model::deterministic_packet_model == true);

            // ----------------------------------------------------------------
            // Compile-time assertions (end)
            // ----------------------------------------------------------------

            _origin      = origin;
            _context     = context;

            pami_result_t status = PAMI_ERROR;

            // The models must be registered in reverse order of use in case
            // the remote side is delayed in its registrations and must save
            // unexpected packets until dispatch registration.

            TRACE_FORMAT("register ack 32b model dispatch %zu", dispatch);
            status = _ack_32b_model.init (dispatch, dispatch_ack_fn<uint32_t>, this);

            if (status != PAMI_SUCCESS)
              {
                TRACE_FORMAT("ack 32b model registration failed. status = %d", status);
                TRACE_FN_EXIT();
                return status;
              }

            TRACE_FORMAT("register ack 64b model dispatch %zu", dispatch);
            status = _ack_64b_model.init (dispatch, dispatch_ack_fn<uint64_t>, this);

            if (status != PAMI_SUCCESS)
              {
                TRACE_FORMAT("ack 64b model registration failed. status = %d", status);
                TRACE_FN_EXIT();
                return status;
              }

            TRACE_FORMAT("register signed int model dispatch %zu", dispatch);
            status = _signed_int_model.init (dispatch, dispatch_fn<signed int>, this);

            if (status != PAMI_SUCCESS)
              {
                TRACE_FORMAT("signed int model registration failed. status = %d", status);
                TRACE_FN_EXIT();
                return status;
              }

            TRACE_FORMAT("register unsigned int model dispatch %zu", dispatch);
            status = _unsigned_int_model.init (dispatch, dispatch_fn<unsigned int>, this);

            if (status != PAMI_SUCCESS)
              {
                TRACE_FORMAT("unsigned int model registration failed. status = %d", status);
                TRACE_FN_EXIT();
                return status;
              }

            TRACE_FORMAT("register signed long model dispatch %zu", dispatch);
            status = _signed_long_model.init (dispatch, dispatch_fn<signed long>, this);

            if (status != PAMI_SUCCESS)
              {
                TRACE_FORMAT("signed long model registration failed. status = %d", status);
                TRACE_FN_EXIT();
                return status;
              }

            TRACE_FORMAT("register unsigned long model dispatch %zu", dispatch);
            status = _unsigned_long_model.init (dispatch, dispatch_fn<unsigned long>, this);

            if (status != PAMI_SUCCESS)
              {
                TRACE_FORMAT("unsigned long model registration failed. status = %d", status);
                TRACE_FN_EXIT();
                return status;
              }

            TRACE_FORMAT("register signed long long model dispatch %zu", dispatch);
            status = _signed_long_long_model.init (dispatch, dispatch_fn<signed long long>, this);

            if (status != PAMI_SUCCESS)
              {
                TRACE_FORMAT("signed long long model registration failed. status = %d", status);
                TRACE_FN_EXIT();
                return status;
              }

            TRACE_FORMAT("register unsigned long long model dispatch %zu", dispatch);
            status = _unsigned_long_long_model.init (dispatch, dispatch_fn<unsigned long long>, this);

            if (status != PAMI_SUCCESS)
              {
                TRACE_FORMAT("unsigned long long model registration failed. status = %d", status);
                TRACE_FN_EXIT();
                return status;
              }


            TRACE_FN_EXIT();
            return status;
          }

          ///
          /// \brief Start a new read-modify-write operation
          ///
          virtual pami_result_t start (pami_rmw_t * parameters)
          {
            TRACE_FN_ENTER();

            pami_task_t task;
            size_t offset;
            PAMI_ENDPOINT_INFO(parameters->dest, task, offset);

            // Verify that this task is addressable by this packet device
            if (unlikely(_ack_32b_model.device.isPeer (task) == false))
              {
                TRACE_FN_EXIT();
                return PAMI_INVAL;
              }

            // Allocate memory to maintain the origin state of the rmw operation.
            state_t * state = (state_t *) _allocator.allocateObject();
            TRACE_FORMAT("state = %p", state);

            state->origin.done_fn  = parameters->done_fn;
            state->origin.cookie   = parameters->cookie;
            state->origin.protocol = this;

            pami_event_function   fn     = NULL;
            void                * cookie = NULL;
            TRACE_FORMAT("parameters->operation & PAMI_ATOMIC_FETCH = %d", parameters->operation & PAMI_ATOMIC_FETCH);

            if (parameters->operation & PAMI_ATOMIC_FETCH)
              {
                // The rmw operation is complete when the ack containing the
                // original remote value is dispatched.
                state->origin.local = parameters->local;
                TRACE_FORMAT("parameters->local = %p, state->origin.local = %p", parameters->local, state->origin.local);
              }
            else
              {
                // The rmw operation is complete when the postPacket completes.
                fn     = origin_done;
                cookie = (void *) state;
              }

            // Initialize the envelope information
            state->origin.envelope.dst       = parameters->remote;
            state->origin.envelope.operation = parameters->operation;
            state->origin.envelope.origin    = _origin;
            state->origin.envelope.state     = (uintptr_t) state;

            TRACE_FORMAT("state->origin.envelope.state = %p", (void *) state->origin.envelope.state);
            TRACE_HEXDATA((void *) & state->origin.envelope, sizeof(envelope_t));

            // Initialize the iovec array for the packet payload
            state->origin.iov[0].iov_base = & (state->origin.envelope);
            state->origin.iov[0].iov_len  = sizeof (envelope_t);
            state->origin.iov[1].iov_base = parameters->value;
            state->origin.iov[2].iov_base = parameters->test;

            TRACE_FORMAT("parameters->operation & PAMI_ATOMIC_COMPARE = %d", parameters->operation & PAMI_ATOMIC_COMPARE);

            const pami_type_t t = parameters->type;

            if (t == PAMI_TYPE_SIGNED_INT)
              {
                state->origin.iov[1].iov_len = sizeof (signed int);
                state->origin.iov[2].iov_len = sizeof (signed int);

                _signed_int_model.postPacket (state->origin.msg,
                                              fn, cookie,
                                              task, offset,
                                              NULL, NULL,
                                              state->origin.iov);
              }
            else if (t == PAMI_TYPE_UNSIGNED_INT)
              {
                state->origin.iov[1].iov_len = sizeof (unsigned int);
                state->origin.iov[2].iov_len = sizeof (unsigned int);

                _unsigned_int_model.postPacket (state->origin.msg,
                                                fn, cookie,
                                                task, offset,
                                                NULL, NULL,
                                                state->origin.iov);
              }
            else if (t == PAMI_TYPE_SIGNED_LONG)
              {
                state->origin.iov[1].iov_len = sizeof (signed long);
                state->origin.iov[2].iov_len = sizeof (signed long);

                _signed_long_model.postPacket (state->origin.msg,
                                               fn, cookie,
                                               task, offset,
                                               NULL, NULL,
                                               state->origin.iov);
              }
            else if (t == PAMI_TYPE_UNSIGNED_LONG)
              {
                state->origin.iov[1].iov_len = sizeof (unsigned long);
                state->origin.iov[2].iov_len = sizeof (unsigned long);

                _unsigned_long_model.postPacket (state->origin.msg,
                                                 fn, cookie,
                                                 task, offset,
                                                 NULL, NULL,
                                                 state->origin.iov);
              }
            else if (t == PAMI_TYPE_SIGNED_LONG_LONG)
              {
                state->origin.iov[1].iov_len = sizeof (signed long long);
                state->origin.iov[2].iov_len = sizeof (signed long long);

                _signed_long_long_model.postPacket (state->origin.msg,
                                                    fn, cookie,
                                                    task, offset,
                                                    NULL, NULL,
                                                    state->origin.iov);
              }
            else if (t == PAMI_TYPE_UNSIGNED_LONG_LONG)
              {
                state->origin.iov[1].iov_len = sizeof (unsigned long long);
                state->origin.iov[2].iov_len = sizeof (unsigned long long);

                _unsigned_long_long_model.postPacket (state->origin.msg,
                                                      fn, cookie,
                                                      task, offset,
                                                      NULL, NULL,
                                                      state->origin.iov);
              }
            else
              {
                // error!
                TRACE_FORMAT("unsupported rmw type = %p", (void *) parameters->type);
                _allocator.returnObject (state);

                TRACE_FN_EXIT();
                return PAMI_INVAL;
              }

            TRACE_FN_EXIT();
            return PAMI_SUCCESS;
          };

        protected:

          ///
          /// \brief rmw dispatch function
          ///
          template <typename T>
          static int dispatch_fn (void   * metadata,
                                  void   * payload,
                                  size_t   bytes,
                                  void   * recv_func_parm,
                                  void   * cookie)
          {
            TRACE_FN_ENTER();

            TRACE_HEXDATA(payload, 64);

            RmwOverSendProtocol * protocol = (RmwOverSendProtocol *) recv_func_parm;

            typedef struct
            {
              envelope_t envelope;
              T          value;
              T          test;
            } rmw_payload_t;

            rmw_payload_t * rmw = (rmw_payload_t *) payload;

            const pami_atomic_t op = rmw->envelope.operation;
            T * dst = (T *) rmw->envelope.dst;

            const T original_value = *dst;
            const T input_value    = rmw->value;

            // If the rmw is not a compare, then do the data operation.
            // Otherwise, the rmw *is* a compare and if the 'test' value is
            // equal to the value at the dst address, then do the data operation

            TRACE_FORMAT("rmw operation = %p, input_value = %p, rmw->test = %p, dst = %p, *dst = %p", (void *) op, (void *) input_value, (void *) rmw->test, dst, (void *) *dst);

            if (!(op & PAMI_ATOMIC_COMPARE) || (rmw->test == *dst))
              {
                const unsigned tmp = op & 0x01c;

                if (tmp == PAMI_ATOMIC_SET)
                  {
                    TRACE_STRING("modify op = PAMI_ATOMIC_SET");
                    *dst = input_value;
                  }
                else if (tmp == PAMI_ATOMIC_ADD)
                  {
                    TRACE_STRING("modify op = PAMI_ATOMIC_ADD");
                    *dst = original_value + input_value;
                  }
                else if (tmp == PAMI_ATOMIC_OR)
                  {
                    TRACE_STRING("modify op = PAMI_ATOMIC_OR");
                    *dst = original_value | input_value;
                    //TRACE_FORMAT("sizeof(T) = %zu, original_value = %lld, input_value = %lld, new value = %lld", sizeof(T), (long long unsigned)original_value, (long long unsigned)input_value, (long long unsigned)*dst);
                  }
                else if (tmp == PAMI_ATOMIC_AND)
                  {
                    TRACE_STRING("modify op = PAMI_ATOMIC_AND");
                    *dst = original_value & input_value;
                  }
                else if (tmp == PAMI_ATOMIC_XOR)
                  {
                    TRACE_STRING("modify op = PAMI_ATOMIC_XOR");
                    *dst = original_value ^ input_value;
                  }
                else
                  {
                    TRACE_STRING("modify op = \"noop\"");
                  }
              }

            if (op & PAMI_ATOMIC_FETCH)
              {
                // Allocate memory to maintain the target-side state of the
                // rmw operation.
                state_t * state = (state_t *) protocol->_allocator.allocateObject();
                TRACE_FORMAT("state = %p", state);

                state->target.protocol     = protocol;
                state->target.ack.metadata = rmw->envelope.state;

                TRACE_FORMAT("rmw->envelope.state = %p, state->target.ack.metadata = %p", (void *) rmw->envelope.state, (void *) state->target.ack.metadata);

                pami_task_t task;
                size_t offset;
                PAMI_ENDPOINT_INFO(rmw->envelope.origin, task, offset);

                if (sizeof(T) == 4)
                  {
                    // The _ack_32b_model invokes dispatch_ack_fn<uint32_t>
                    state->target.ack.payload_32b  = (uint32_t) original_value;
                    protocol->_ack_32b_model.postPacket (state->target.msg,
                                                         target_done, (void *) state,
                                                         task, offset,
                                                         (void *) &(state->target.ack.metadata),
                                                         sizeof (uintptr_t),
                                                         (void *) &(state->target.ack.payload_32b),
                                                         sizeof (T));
                  }
                else
                  {
                    // The _ack_64b_model invokes dispatch_ack_fn<uint64_t>
                    state->target.ack.payload_64b  = (uint64_t) original_value;
                    protocol->_ack_64b_model.postPacket (state->target.msg,
                                                         target_done, (void *) state,
                                                         task, offset,
                                                         (void *) &(state->target.ack.metadata),
                                                         sizeof (uintptr_t),
                                                         (void *) &(state->target.ack.payload_64b),
                                                         sizeof (T));
                  }
              }

            TRACE_FN_EXIT();
            return 0;
          }

          ///
          /// \brief Deallocate the target-side state object
          ///
          static void target_done (pami_context_t   context,
                                   void           * cookie,
                                   pami_result_t    result)
          {
            TRACE_FN_ENTER();

            state_t * state = (state_t *) cookie;
            state->target.protocol->_allocator.returnObject (cookie);

            TRACE_FN_EXIT();
          }

          ///
          /// \brief Invoke the completion callback and deallocate the origin-side state object
          ///
          static void origin_done (pami_context_t   context,
                                   void           * cookie,
                                   pami_result_t    result)
          {
            TRACE_FN_ENTER();

            state_t * state = (state_t *) cookie;

            if (state->origin.done_fn)
              state->origin.done_fn (state->origin.protocol->_context,
                                     state->origin.cookie,
                                     PAMI_SUCCESS);

            state->origin.protocol->_allocator.returnObject (cookie);

            TRACE_FN_EXIT();
          }

          template <typename T>
          static int dispatch_ack_fn (void   * metadata,
                                      void   * payload,
                                      size_t   bytes,
                                      void   * recv_func_parm,
                                      void   * cookie)
          {
            TRACE_FN_ENTER();

            RmwOverSendProtocol * protocol = (RmwOverSendProtocol *) recv_func_parm;
            state_t * state = (state_t *) * ((uintptr_t *) metadata);

            TRACE_FORMAT("state = %p", state);
            TRACE_FORMAT("state->origin.local = %p", state->origin.local);

            *((T *) state->origin.local) = *((T *) payload);

            if (state->origin.done_fn)
              state->origin.done_fn (protocol->_context,
                                     state->origin.cookie,
                                     PAMI_SUCCESS);

            protocol->_allocator.returnObject (state);

            TRACE_FN_EXIT();
            return 0;
          }

          T_Model                                 _ack_32b_model;
          T_Model                                 _ack_64b_model;
          T_Model                                 _signed_int_model;
          T_Model                                 _unsigned_int_model;
          T_Model                                 _signed_long_model;
          T_Model                                 _unsigned_long_model;
          T_Model                                 _signed_long_long_model;
          T_Model                                 _unsigned_long_long_model;

          MemoryAllocator < sizeof(state_t), 16 > _allocator;
          pami_context_t                          _context;
          pami_endpoint_t                         _origin;
      };
    };
  };
};
#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG
#endif // __p2p_protocols_rmw_RmwOverSend_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
