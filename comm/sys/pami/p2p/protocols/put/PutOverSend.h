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
/// \file p2p/protocols/put/PutOverSend.h
/// \brief Put protocol devices that implement the 'packet' interface.
///
/// The Put class defined in this file uses C++ templates
/// and the "packet" device interface which also uses C++ templates.
///
/// C++ templates require all source code to be #include'd from a header file
/// which can result in a single very large file.
///
#ifndef __p2p_protocols_put_PutOverSend_h__
#define __p2p_protocols_put_PutOverSend_h__

#include <string.h>

#include "pami.h"

#include "components/devices/PacketInterface.h"
#include "components/memory/MemoryAllocator.h"
#include "math/Memcpy.x.h"
#include "p2p/protocols/Put.h"

#include "common/type/TypeCode.h"
#include "common/type/TypeMachine.h"


#include "util/trace.h"
#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  namespace Protocol
  {
    namespace Put
    {
      template <class T_Model>
      class PutOverSend : public Put
      {
        protected:

          typedef PutOverSend<T_Model> PutOverSendProtocol;

          typedef uint8_t model_state_t[T_Model::packet_model_state_bytes];
          typedef uint8_t model_packet_t[T_Model::packet_model_payload_bytes];



          typedef struct
          {
            uintptr_t             remote_addr;
            uint16_t              type_bytes;
            uint16_t              data_bytes;
          } metadata_packed_t;

          typedef struct
          {
            model_state_t         state;
            model_packet_t        packet;
            metadata_packed_t     metadata;
          } origin_packed_t;



          typedef struct
          {
            uintptr_t             remote_addr;
            size_t                data_bytes;
            pami_endpoint_t       origin;
            size_t                type_bytes; // => 0 iff contiguous
            size_t                machine_bytes;
          } metadata_header_t;

          typedef struct
          {
            struct
            {
              model_state_t       state;
            } type;

            struct
            {
              model_state_t       state;
              metadata_header_t   metadata;
            } header;

          } origin_envelope_t;

          typedef struct
          {
            struct
            {
              size_t             type_bytes;
              size_t             machine_bytes;
              size_t             bytes_remaining;
            } type;


          } target_envelope_t;



          typedef union
          {
            struct
            {
              model_state_t       state;
            } contig;

            struct
            {
              model_state_t       state[2];
              model_packet_t      packet[2];
              uint8_t             machine[sizeof(Type::TypeMachine)];
              void              * base_addr;
              size_t              bytes_remaining;
              size_t              start_count;
            } typed;

          } origin_data_t;

          typedef struct
          {
            uint8_t               machine[sizeof(Type::TypeMachine)];
            void                * type_buffer;
            size_t                bytes_remaining;
            void                * base_addr;
            bool                  is_contiguous;
          } target_data_t;



          typedef struct
          {
            pami_event_function   remote_fn;
            void                * cookie;
            bool                  invoke;
          } metadata_remotefn_t;




          typedef struct
          {
            origin_packed_t         packed;
            origin_envelope_t       envelope;
            origin_data_t           data;
            pami_event_function     done_fn;
            pami_event_function     rdone_fn;
            void                  * cookie;
            PutOverSendProtocol   * protocol;
            pami_task_t             target_task;
            size_t                  target_offset;
            
          } origin_state_t;

          typedef struct
          {
            target_envelope_t       envelope;
            target_data_t           data;
          } target_state_t;

          typedef struct
          {
            model_state_t         state;
            metadata_remotefn_t   metadata;
            PutOverSendProtocol * protocol;
          } remotefn_state_t;



          typedef union
          {
            origin_state_t        origin;
            target_state_t        target;
            remotefn_state_t      remotefn;
          } state_t;


          static const size_t maximum_packed_packet_payload =
            T_Model::packet_model_payload_bytes -
            (sizeof(metadata_packed_t) > T_Model::packet_model_metadata_bytes) * sizeof(metadata_packed_t);


        public:

          template <class T_Device, class T_MemoryManager>
          static PutOverSend * generate (T_Device        & device,
                                         T_MemoryManager * mm)
          {
            TRACE_FN_ENTER();

            pami_result_t result = PAMI_ERROR;
            void * protocol = NULL;
            result = mm->memalign((void **) & protocol, 16, sizeof(PutOverSend));
            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for put-over-send protocol");
            new (protocol) PutOverSend (device);

            TRACE_FN_EXIT();
            return (PutOverSend *) protocol;
          };

          template <class T_Device>
          inline PutOverSend (T_Device & device) :
              _remotefn_model (device),
              _packed_model (device),
              _header_model (device),
              _type_model (device),
              _data_model (device)
          {
            TRACE_FN_ENTER();

            // ----------------------------------------------------------------
            // Compile-time assertions (begin)
            // ----------------------------------------------------------------

            // This protocol only works with reliable networks.
            COMPILE_TIME_ASSERT(T_Model::reliable_packet_model == true);

            // This protocol only works with deterministic models.
            COMPILE_TIME_ASSERT(T_Model::deterministic_packet_model == true);

            // Assert that the size of the packet metadata area is large
            // enough to transfer the connection information. This is used by
            // postMultiPacket() to transfer data messages.
            COMPILE_TIME_ASSERT(sizeof(pami_endpoint_t) <= T_Model::packet_model_multi_metadata_bytes);

            // Assert that the size of the packet payload area is large
            // enough to transfer a single virtual address. This is used in
            // the postPacket() calls to transfer the ack information.
            COMPILE_TIME_ASSERT(sizeof(void *) <= T_Model::packet_model_payload_bytes);

            // ----------------------------------------------------------------
            // Compile-time assertions (end)
            // ----------------------------------------------------------------

            TRACE_FN_EXIT();
          }

          pami_result_t initialize (size_t          dispatch,
                                    pami_endpoint_t origin,
                                    pami_context_t  context)
          {
            TRACE_FN_ENTER();

            _origin      = origin;
            _context     = context;

            pami_result_t status = PAMI_ERROR;

            // The models must be registered in reverse order of use in case
            // the remote side is delayed in its registrations and must save
            // unexpected packets until dispatch registration.

            TRACE_FORMAT("register remotefn model dispatch %zu", dispatch);
            status = _remotefn_model.init (dispatch, dispatch_remotefn, this);

            if (status != PAMI_SUCCESS)
              {
                TRACE_STRING("remotefn model registration failed.");
                TRACE_FN_EXIT();
                return status;
              }

            TRACE_STRING("remotefn model registration successful.");
            TRACE_FORMAT("register packed model dispatch %zu", dispatch);
            status = _packed_model.init (dispatch, dispatch_packed, this);

            if (status != PAMI_SUCCESS)
              {
                TRACE_STRING("packed model registration failed.");
                TRACE_FN_EXIT();
                return status;
              }

            TRACE_STRING("packed model registration successful.");
            TRACE_FORMAT("register data model dispatch %zu", dispatch);
            status = _data_model.init (dispatch, dispatch_data, this);

            if (status != PAMI_SUCCESS)
              {
                TRACE_STRING("data model registration failed.");
                TRACE_FN_EXIT();
                return status;
              }

            TRACE_STRING("data model registration successful.");
            TRACE_FORMAT("register type model dispatch %zu", dispatch);
            status = _type_model.init (dispatch, dispatch_type, this);

            if (status != PAMI_SUCCESS)
              {
                TRACE_STRING("type model registration failed.");
                TRACE_FN_EXIT();
                return status;
              }

            TRACE_STRING("type model registration successful.");
            TRACE_FORMAT("register header model dispatch %zu", dispatch);
            status = _header_model.init (dispatch, dispatch_header, this);

            if (status != PAMI_SUCCESS)
              {
                TRACE_STRING("header model registration failed.");
                TRACE_FN_EXIT();
                return status;
              }

            TRACE_STRING("header model registration successful.");

            TRACE_FN_EXIT();
            return status;
          }

          ///
          /// \brief Start a new contiguous put operation
          ///
          virtual pami_result_t simple (pami_put_simple_t * parameters)
          {
            TRACE_FN_ENTER();

            pami_result_t result = PAMI_ERROR;
            result = start_internal (parameters);

            TRACE_FN_EXIT();
            return result;
          };

          ///
          /// \brief Start a new non-contiguous put operation
          ///
          virtual pami_result_t typed (pami_put_typed_t * parameters)
          {
            TRACE_FN_ENTER();

            pami_result_t result = PAMI_ERROR;
            result = start_internal (parameters);

            TRACE_FN_EXIT();
            return result;
          }

        protected:

          ///
          /// \brief Start a new put operation
          ///
          template <typename T>
          inline pami_result_t start_internal (T * parameters)
          {
            TRACE_FN_ENTER();

            pami_task_t task;
            size_t offset;
            PAMI_ENDPOINT_INFO(parameters->rma.dest, task, offset);

            // Verify that this task is addressable by this packet device
            if (unlikely(_packed_model.device.isPeer (task) == false))
              {
                TRACE_FN_EXIT();
                return PAMI_INVAL;
              }

            // Allocate memory to maintain the origin state of the put operation.
            state_t * state = (state_t *) _allocator.allocateObject();
            TRACE_FORMAT("state = %p", state);
            state->origin.done_fn       = parameters->rma.done_fn;
            state->origin.rdone_fn      = parameters->put.rdone_fn;
            state->origin.cookie        = parameters->rma.cookie;
            state->origin.protocol      = this;
            state->origin.target_task   = task;
            state->origin.target_offset = offset;

            // ----------------------------------------------------------------
            // Check for a "short" put protocol
            // ----------------------------------------------------------------
            if (parameters->rma.bytes <= maximum_packed_packet_payload)
              {
                TRACE_FORMAT("'single packet' put protocol: parameters->rma.bytes = %zu, maximum_packed_packet_payload = %zu", parameters->rma.bytes, maximum_packed_packet_payload);

                send_packed (parameters, task, offset, state);
              }
            else
              {
                TRACE_FORMAT("'multi packet' put protocol: parameters->rma.bytes = %zu, maximum_packed_packet_payload = %zu", parameters->rma.bytes, maximum_packed_packet_payload);

                send_envelope (parameters, task, offset, state);
                send_data (parameters, task, offset, state);
              }

            TRACE_FN_EXIT();
            return PAMI_SUCCESS;
          };

          ///
          /// \brief Origin put completion event callback.
          ///
          /// This callback will invoke the application local completion
          /// callback function, free the transfer state memory, and initiate a
          /// 'remotefn' communication flow - if needed.
          ///
          static void complete_origin (pami_context_t   context,
                                       void           * cookie,
                                       pami_result_t    result)
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT("cookie = %p", cookie);

            state_t * state = (state_t *) cookie;
            TRACE_FORMAT("state->origin.done_fn = %p, state->origin.cookie = %p", state->origin.done_fn, state->origin.cookie);

            if (state->origin.done_fn != NULL)
              {
                state->origin.done_fn (context, state->origin.cookie, PAMI_SUCCESS);
              }

            PutOverSendProtocol * protocol = (PutOverSendProtocol *) state->origin.protocol;
            TRACE_FORMAT("protocol = %p", protocol);

            if (state->origin.rdone_fn != NULL)
              {
                protocol->send_remotefn (state->origin.target_task,
                                         state->origin.target_offset,
                                         state->origin.rdone_fn,
                                         state->origin.cookie);
              }

            protocol->_allocator.returnObject (cookie);

            TRACE_FN_EXIT();
            return;
          }


          // ##################################################################
          // 'packed' code which sends the source data and the serialized
          // target type in a single packet.
          // ##################################################################

          inline void send_packed (pami_put_simple_t * parameters,
                                   pami_task_t         task,
                                   size_t              offset,
                                   state_t           * state);

          inline void send_packed (pami_put_typed_t * parameters,
                                   pami_task_t        task,
                                   size_t             offset,
                                   state_t          * state);

          static int dispatch_packed (void   * metadata,
                                      void   * payload,
                                      size_t   bytes,
                                      void   * recv_func_parm,
                                      void   * cookie);


          // ##################################################################
          // 'envelope' code which sends the protocol metadata in a 'header'
          // packet, and the serialized type (if non-contiguous) in one or more
          // 'type' packets.
          // ##################################################################

          inline void send_envelope (pami_put_simple_t * parameters,
                                     pami_task_t         task,
                                     size_t              offset,
                                     state_t           * state);

          inline void send_envelope (pami_put_typed_t * parameters,
                                     pami_task_t        task,
                                     size_t             offset,
                                     state_t          * state);

          static int dispatch_header (void   * metadata,
                                      void   * payload,
                                      size_t   bytes,
                                      void   * recv_func_parm,
                                      void   * cookie);

          static int dispatch_type (void   * metadata,
                                    void   * payload,
                                    size_t   bytes,
                                    void   * recv_func_parm,
                                    void   * cookie);


          // ##################################################################
          // 'data' code which sends one or more data packets for contiguous
          // source data, or packs the non-contiguous source data, in a
          // pipelined manner, one packet at a time.
          // ##################################################################

          inline void send_data (pami_put_simple_t * parameters,
                                 pami_task_t         task,
                                 size_t              offset,
                                 state_t           * state);

          inline void send_data (pami_put_typed_t * parameters,
                                 pami_task_t        task,
                                 size_t             offset,
                                 state_t          * state);

          static int dispatch_data (void   * metadata,
                                    void   * payload,
                                    size_t   bytes,
                                    void   * recv_func_parm,
                                    void   * cookie);

          static void complete_data (pami_context_t   context,
                                     void           * cookie,
                                     pami_result_t    result);


          // ##################################################################
          // 'acknowledgment' code which provides the 'remote callback'
          // functionality without impacting the normal, performance-critical
          // code path.
          // ##################################################################

          ///
          /// \brief Initiate an independent 'remotefn' communication flow
          ///
          /// Used only by the remotefn request originator.
          ///
          /// \param [in] task   Target task
          /// \param [in] offset Target context offset
          /// \param [in] fn     Event function to be invoked on the origin
          /// \param [in] cookie Event function cookie
          ///
          inline void send_remotefn (pami_task_t           task,
                                     size_t                offset,
                                     pami_event_function   fn,
                                     void                * cookie);

          ///
          /// \brief Process an incomming 'remotefn' packet
          ///
          /// This dispatch function is invoked on both the target and origin
          /// tasks of a remotefn acknowledgement communication flow.
          ///
          /// A dispatch on the target task will inject the remotefn 'pong'
          /// packet, while a dispatch on the origin task will examine the
          /// remotefn packet metadata and invoke the completion event function.
          ///
          /// \see PAMI::Device::Interface::RecvFunction_t
          ///
          static int dispatch_remotefn (void   * metadata,
                                        void   * payload,
                                        size_t   bytes,
                                        void   * recv_func_parm,
                                        void   * cookie);

          ///
          /// \brief Deallocate the send object used to inject a remotefn packet
          ///
          static void complete_remotefn (pami_context_t   context,
                                         void           * cookie,
                                         pami_result_t    result);



          T_Model                                   _remotefn_model;
          T_Model                                   _packed_model;
          T_Model                                   _header_model;
          T_Model                                   _type_model;
          T_Model                                   _data_model;
          MemoryAllocator < sizeof(state_t), 16 >   _allocator;
          pami_context_t                            _context;
          pami_endpoint_t                           _origin;
      };
    };
  };
};
#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#include "p2p/protocols/put/PutOverSend_packed_impl.h"
#include "p2p/protocols/put/PutOverSend_envelope_impl.h"
#include "p2p/protocols/put/PutOverSend_data_impl.h"
#include "p2p/protocols/put/PutOverSend_remotefn_impl.h"

#endif // __p2p_protocols_put_PutOverSend_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
