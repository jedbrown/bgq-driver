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
/// \file p2p/protocols/get/GetOverSend.h
/// \brief Get protocol devices that implement the 'packet' interface.
///
/// The GetOverSend class defined in this file uses C++ templates
/// and the "packet" device interface which also uses C++ templates.
///
/// C++ templates require all source code to be #include'd from a header file
/// which can result in a single very large file.
///
#ifndef __p2p_protocols_get_GetOverSend_h__
#define __p2p_protocols_get_GetOverSend_h__

#include <string.h>

#include "pami.h"

#include "components/devices/PacketInterface.h"
#include "math/Memcpy.x.h"
#include "p2p/protocols/Get.h"

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
    namespace Get
    {
      template <class T_Model>
      class GetOverSend : public Get
      {
        protected:

          typedef GetOverSend<T_Model> GetOverSendProtocol;

          typedef uint8_t model_state_t[T_Model::packet_model_state_bytes];
          typedef uint8_t model_packet_t[T_Model::packet_model_payload_bytes];



          typedef struct
          {
            uintptr_t             remote_addr;
            size_t                data_bytes;
            pami_endpoint_t       origin;
            size_t                type_bytes; // => 0 iff contiguous
            size_t                machine_bytes;
            uintptr_t             origin_state;
          } metadata_header_t;

          typedef struct
          {
            void                * type_buffer;
            size_t                machine_bytes;
            size_t                type_bytes;
            size_t                bytes_remaining;
          } target_envelope_t;

          typedef struct
          {
            model_state_t         state[2];
            model_packet_t        packet[2];
            uint8_t               machine[sizeof(Type::TypeMachine)];
            void                * base_addr;
            size_t                bytes_remaining;
            size_t                start_count;
            size_t                origin_task;
            size_t                origin_offset;

            uintptr_t             metadata;
            GetOverSendProtocol * protocol;

            bool                  is_contiguous;
          } target_data_t;

          typedef struct
          {
            target_envelope_t     envelope;
            target_data_t         data;
          } target_t;



          typedef struct
          {
            size_t                bytes_remaining;
            void                * base_addr;
            pami_event_function   done_fn;
            void                * cookie;
            uint8_t               machine[sizeof(Type::TypeMachine)];
            bool                  is_contiguous;
            struct
            {
              model_state_t       state;
              metadata_header_t   metadata;
            } header;
            struct
            {
              model_state_t       state;
            } type;
          } origin_t;



          typedef union
          {
            origin_t              origin;
            target_t              target;
          } state_t;

        public:

          template <class T_Device, class T_MemoryManager>
          static GetOverSend * generate (T_Device        & device,
                                         T_MemoryManager * mm)
          {
            TRACE_FN_ENTER();

            pami_result_t result = PAMI_ERROR;
            void * protocol = NULL;
            result = mm->memalign((void **) & protocol, 16, sizeof(GetOverSend));
            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for get-over-send protocol");
            new (protocol) GetOverSend (device);

            TRACE_FN_EXIT();
            return (GetOverSend *) protocol;
          };

          template <class T_Device>
          inline GetOverSend (T_Device & device) :
              _data_model (device),
              _type_model (device),
              _header_model (device)
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
            // enough to transfer the virtual address of the origin state object.
            // This is used by postMultiPacket() to transfer data messages.
            COMPILE_TIME_ASSERT(sizeof(uintptr_t) <= T_Model::packet_model_multi_metadata_bytes);

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

            _context = context;
            _origin  = origin;

            pami_result_t status = PAMI_ERROR;

            // The models must be registered in reverse order of use in case
            // the remote side is delayed in its registrations and must save
            // unexpected packets until dispatch registration.

            TRACE_FORMAT("register data model dispatch %zu", dispatch);
            status = _data_model.init (dispatch, dispatch_data, this);

            if (status != PAMI_SUCCESS)
              {
                TRACE_FORMAT("data model registration failed. status = %d", status);
                TRACE_FN_EXIT();
                return status;
              }

            TRACE_STRING("data model registration successful.");

            TRACE_FORMAT("register type model dispatch %zu", dispatch);
            status = _type_model.init (dispatch, dispatch_type, this);

            if (status != PAMI_SUCCESS)
              {
                TRACE_FORMAT("type model registration failed. status = %d", status);
                TRACE_FN_EXIT();
                return status;
              }

            TRACE_STRING("type model registration successful.");

            TRACE_FORMAT("register header model dispatch %zu", dispatch);
            status = _header_model.init (dispatch, dispatch_header, this);

            if (status != PAMI_SUCCESS)
              {
                TRACE_FORMAT("header model registration failed. status = %d", status);
                TRACE_FN_EXIT();
                return status;
              }

            TRACE_STRING("header model registration successful.");


            TRACE_FN_EXIT();
            return status;
          }

          ///
          /// \brief Start a new contiguous get operation
          ///
          virtual pami_result_t get (pami_get_simple_t * parameters)
          {
            TRACE_FN_ENTER();

            pami_task_t task;
            size_t offset;
            PAMI_ENDPOINT_INFO(parameters->rma.dest, task, offset);

            // Verify that this task is addressable by this packet device
            if (unlikely(_data_model.device.isPeer (task) == false))
              {
                TRACE_FN_EXIT();
                return PAMI_INVAL;
              }

            // Allocate memory to maintain the origin state of the operation.
            state_t * state = (state_t *) _allocator.allocateObject();
            TRACE_FORMAT("state = %p", state);

            state->origin.bytes_remaining = parameters->rma.bytes;
            state->origin.base_addr       = parameters->addr.local;
            state->origin.done_fn         = parameters->rma.done_fn;
            state->origin.cookie          = parameters->rma.cookie;
            state->origin.is_contiguous   = true;

            send_envelope (parameters, task, offset, state);

            TRACE_FN_EXIT();
            return PAMI_SUCCESS;
          };

          ///
          /// \brief Start a new non-contiguous get operation
          ///
          virtual pami_result_t get (pami_get_typed_t * parameters)
          {
            TRACE_FN_ENTER();

            pami_task_t task;
            size_t offset;
            PAMI_ENDPOINT_INFO(parameters->rma.dest, task, offset);

            // Verify that this task is addressable by this packet device
            if (unlikely(_data_model.device.isPeer (task) == false))
              {
                TRACE_FN_EXIT();
                return PAMI_INVAL;
              }

            // Allocate memory to maintain the origin state of the operation.
            state_t * state = (state_t *) _allocator.allocateObject();
            TRACE_FORMAT("state = %p", state);

            // Construct the type machine.
            // The machine will be destroyed when the last data packet has
            // been received from the target endpoint.
            Type::TypeCode * type_obj = (Type::TypeCode *) parameters->type.local;
            Type::TypeCode * machine = (Type::TypeCode *) state->origin.machine;
            new (machine) Type::TypeMachine (type_obj);

            state->origin.bytes_remaining = parameters->rma.bytes;
            state->origin.base_addr       = parameters->addr.local;
            state->origin.done_fn         = parameters->rma.done_fn;
            state->origin.cookie          = parameters->rma.cookie;
            state->origin.is_contiguous   = type_obj->IsContiguous();

            send_envelope (parameters, task, offset, state);

            TRACE_FN_EXIT();
            return PAMI_SUCCESS;
          };

        protected:

          // ##################################################################
          // 'envelope' code which sends the protocol metadata in a 'header'
          // packet, and the serialized type (if non-contiguous) in one or more
          // 'type' packets.
          // ##################################################################

          inline void send_envelope (pami_get_simple_t * parameters,
                                     pami_task_t         task,
                                     size_t              offset,
                                     state_t           * state);

          inline void send_envelope (pami_get_typed_t * parameters,
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

          inline void send_data (uintptr_t     metadata,
                                 void        * data_addr,
                                 size_t        data_bytes,
                                 pami_task_t   task,
                                 size_t        offset,
                                 state_t     * state);

          inline void send_data (state_t * state);

          static void complete_data (pami_context_t   context,
                                     void           * cookie,
                                     pami_result_t    result);

          static int dispatch_data (void   * metadata,
                                    void   * payload,
                                    size_t   bytes,
                                    void   * recv_func_parm,
                                    void   * cookie);


          T_Model                                 _data_model;
          T_Model                                 _type_model;
          T_Model                                 _header_model;
          MemoryAllocator < sizeof(state_t), 16 > _allocator;
          pami_context_t                          _context;
          pami_endpoint_t                         _origin;
      };
    };
  };
};
#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#include "p2p/protocols/get/GetOverSend_envelope_impl.h"
#include "p2p/protocols/get/GetOverSend_data_impl.h"

#endif // __p2p_protocols_get_GetOverSend_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
