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
/// \file p2p/protocols/rget/GetRdma.h
/// \brief Get protocol devices that implement the 'dma' interface.
///
/// The Get class defined in this file uses C++ templates
/// and the "dma" device interface which also uses C++ templates.
///
/// C++ templates require all source code to be #include'd from a header file
/// which can result in a single very large file.
///
#ifndef __p2p_protocols_rget_GetRdma_h__
#define __p2p_protocols_rget_GetRdma_h__

#include <string.h>

#include "p2p/protocols/RGet.h"

#include "common/type/TypeEnumerator.h"

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
      template <class T_Model, class T_Device>
      class GetRdma : public RGet
      {
        protected:

          typedef GetRdma<T_Model, T_Device> Protocol;

          typedef uint8_t model_state_t[T_Model::dma_model_state_bytes];

          typedef struct
          {
            model_state_t         state;
            pami_event_function   done_fn;
            void                * cookie;
            Protocol            * protocol;
          } simple_t;

          typedef struct
          {
            model_state_t         state[2][10];
            uint8_t               enumerator[sizeof(Type::TypeEnumerator)];
            size_t                pipeline_start_count;
            size_t                pipeline_completion_count;
            size_t                bytes_remaining;
            void                * cookie;
            pami_event_function   done_fn;
            pami_rma_mr_t         local;
            pami_rma_mr_t         remote;
            Protocol            * protocol;
            size_t                task;
            size_t                offset;
          } typed_t;

          typedef union
          {
            simple_t              simple;
            typed_t               typed;
          } state_t;

          ///
          /// \brief Local get completion event callback.
          ///
          /// This callback will invoke the application local completion
          /// callback function and free the transfer state memory.
          ///
          static void complete_simple (pami_context_t   context,
                                       void           * cookie,
                                       pami_result_t    result)
          {
            TRACE_FN_ENTER();

            state_t * get = (state_t *) cookie;

            if (likely(get->simple.done_fn != NULL))
              get->simple.done_fn (context, get->simple.cookie, PAMI_SUCCESS);

            get->simple.protocol->_allocator.returnObject ((void *) get);

            TRACE_FN_EXIT();
          };

          static void complete_typed (pami_context_t   context,
                                      void           * cookie,
                                      pami_result_t    result)
          {
            TRACE_FN_ENTER();

            state_t * get = (state_t *) cookie;

            // Increment the pipeline completion count. If all bytes have been
            // transferred and the pipline start count equals the pipeline
            // completion count, then the entire typed transfer is complete.
            get->typed.pipeline_completion_count++;

            TRACE_FORMAT("get->typed.pipeline_start_count:      %zu", get->typed.pipeline_start_count);
            TRACE_FORMAT("get->typed.pipeline_completion_count: %zu", get->typed.pipeline_completion_count);
            TRACE_FORMAT("get->typed.bytes_remaining:           %zu", get->typed.bytes_remaining);

            if (unlikely(get->typed.bytes_remaining == 0))
              {
                if (get->typed.pipeline_completion_count ==
                    get->typed.pipeline_start_count)
                  {
                    TRACE_FORMAT("get->typed.done_fn: %p", get->typed.done_fn);

                    if (likely(get->typed.done_fn != NULL))
                      get->typed.done_fn (context, get->typed.cookie, PAMI_SUCCESS);

                    get->typed.protocol->_allocator.returnObject ((void *) get);
                  }

                TRACE_FN_EXIT();
                return;
              }

            get->typed.protocol->next_pipeline (get);

            TRACE_FN_EXIT();
          };

          inline bool next_pipeline (state_t * get)
          {
            TRACE_FN_ENTER();

            const size_t task   = get->typed.task;
            const size_t offset = get->typed.offset;

            Memregion * local_mr  = (Memregion *) get->typed.local.mr;
            Memregion * remote_mr = (Memregion *) get->typed.remote.mr;

            const size_t local_offset  = get->typed.local.offset;
            const size_t remote_offset = get->typed.remote.offset;

            Type::TypeEnumerator * tmp = (Type::TypeEnumerator *) get->typed.enumerator;
            Type::TypeEnumerator & enumerator = *tmp;

            size_t & bytes_remaining = get->typed.bytes_remaining;

            // Determine the pipeline that completed and update the start count
            // value for the next send_next_pipeline().
            unsigned pipeline = ++(get->typed.pipeline_start_count) & 0x01;

            size_t block_local, block_remote, block_size;
            unsigned block;

            for (block = 0; block < 9; block++)
              {
                enumerator.next (block_local, block_remote, block_size);

                TRACE_FORMAT("block = %d, %zu, %zu, %zu", block, block_local, block_remote, block_size);

                if (unlikely(block_size > bytes_remaining))
                  block_size = bytes_remaining;

                bytes_remaining -= block_size;

                TRACE_FORMAT("bytes_remaining: %zu, pipeline: %d", bytes_remaining, pipeline);

                if (likely(bytes_remaining > 0))
                  {
                    _model.postDmaGet (get->typed.state[pipeline][block],
                                       NULL, NULL,
                                       task, offset, block_size,
                                       local_mr, local_offset + block_local,
                                       remote_mr, remote_offset + block_remote);
                  }
                else
                  {
                    // This is the last block in the entire transfer operation -
                    // even though it is not at the end of a pipeline block it
                    // must specify a completion callback.
                    TRACE_FORMAT("block_size: %zu, pipeline: %d", block_size, pipeline);
                    _model.postDmaGet (get->typed.state[pipeline][block],
                                       complete_typed, (void *) get,
                                       task, offset, block_size,
                                       local_mr, local_offset + block_local,
                                       remote_mr, remote_offset + block_remote);

                    // There are no bytes remaining to be sent.
                    TRACE_FN_EXIT();
                    return false;
                  }
              }

            // Send the last block in this pipeline with a completion callback.
            enumerator.next (block_local, block_remote, block_size);
            TRACE_FORMAT("block = %d, %zu, %zu, %zu", block, block_local, block_remote, block_size);

            bytes_remaining -= block_size;
            TRACE_FORMAT("bytes_remaining: %zu, pipeline: %d", bytes_remaining, pipeline);
            _model.postDmaGet (get->typed.state[pipeline][block],
                               complete_typed, (void *) get,
                               task, offset, block_size,
                               local_mr, local_offset + block_local,
                               remote_mr, remote_offset + block_remote);

            TRACE_FN_EXIT();
            return (bytes_remaining != 0);
          }

        public:

          template <class T_Allocator>
          static GetRdma * generate (T_Device       & device,
                                     pami_context_t   context,
                                     T_Allocator    & allocator,
                                     pami_result_t  & status)
          {
            TRACE_FN_ENTER();
            COMPILE_TIME_ASSERT(sizeof(GetRdma) <= T_Allocator::objsize);

            GetRdma * get = (GetRdma *) allocator.allocateObject ();
            new ((void *)get) GetRdma (device, context, status);

            if (status != PAMI_SUCCESS)
              {
                allocator.returnObject (get);
                get = NULL;
              }

            TRACE_FORMAT("get = %p, status = %d", get, status);
            TRACE_FN_EXIT();
            return get;
          }

          inline GetRdma (T_Device & device, pami_context_t context, pami_result_t & status) :
              _model (device, status),
              _device (device),
              _context (context)
          {
            TRACE_FN_ENTER();
            COMPILE_TIME_ASSERT(T_Model::dma_model_mr_supported == true);
            TRACE_FN_EXIT();
          }

          ///
          /// \brief Start a new contiguous rdma get operation
          ///
          virtual pami_result_t simple (pami_rget_simple_t * parameters)
          {
            TRACE_FN_ENTER();

            pami_task_t task;
            size_t offset;
            PAMI_ENDPOINT_INFO(parameters->rma.dest, task, offset);

            // Verify that this task is addressable by this dma device
            if (unlikely(_device.isPeer(task) == false))
              {
                TRACE_FN_EXIT();
                return PAMI_ERROR;
              }

            TRACE_STRING("attempt an 'immediate' rget transfer.");

            if (_model.postDmaGet (task, offset,
                                   parameters->rma.bytes,
                                   (Memregion *) parameters->rdma.local.mr,
                                   parameters->rdma.local.offset,
                                   (Memregion *) parameters->rdma.remote.mr,
                                   parameters->rdma.remote.offset))
              {
                TRACE_STRING("'immediate' rget transfer was successful, invoke callback.");

                if (parameters->rma.done_fn)
                  parameters->rma.done_fn (_context, parameters->rma.cookie, PAMI_SUCCESS);

                TRACE_FN_EXIT();
                return PAMI_SUCCESS;
              }

            TRACE_STRING("'immediate' rget transfer was not successful, allocate rdma get state memory.");

            // Allocate memory to maintain the state of the get operation.
            state_t * get = (state_t *) _allocator.allocateObject();

            get->simple.done_fn  = parameters->rma.done_fn;
            get->simple.cookie   = parameters->rma.cookie;
            get->simple.protocol = this;

            TRACE_STRING("attempt a 'non-blocking' rget transfer.");
            _model.postDmaGet (get->simple.state,
                               complete_simple,
                               (void *) get,
                               task, offset,
                               parameters->rma.bytes,
                               (Memregion *) parameters->rdma.local.mr,
                               parameters->rdma.local.offset,
                               (Memregion *) parameters->rdma.remote.mr,
                               parameters->rdma.remote.offset);

            TRACE_FN_EXIT();
            return PAMI_SUCCESS;
          };

          ///
          /// \brief Start a new non-contiguous rdma get operation
          ///
          virtual pami_result_t typed (pami_rget_typed_t * parameters)
          {
            TRACE_FN_ENTER();

            pami_task_t task;
            size_t offset;
            PAMI_ENDPOINT_INFO(parameters->rma.dest, task, offset);

            // Verify that this task is addressable by this dma device
            if (unlikely(_device.isPeer(task) == false)) return PAMI_ERROR;

            state_t * get = (state_t *) _allocator.allocateObject();

            TRACE_STRING("before enumerator constructor.");

            Type::TypeEnumerator * enumerator =
              (Type::TypeEnumerator *) get->typed.enumerator;
            new (enumerator) Type::TypeEnumerator ((Type::TypeCode *) parameters->type.local,
                                                   (Type::TypeCode *) parameters->type.remote);

            // Initialize the transfer state;
            get->typed.pipeline_start_count      = 0;
            get->typed.pipeline_completion_count = 0;
            get->typed.bytes_remaining           = parameters->rma.bytes;

            get->typed.cookie                    = parameters->rma.cookie;
            get->typed.done_fn                   = parameters->rma.done_fn;
            get->typed.local.mr                  = parameters->rdma.local.mr;
            get->typed.local.offset              = parameters->rdma.local.offset;
            get->typed.remote.mr                 = parameters->rdma.remote.mr;
            get->typed.remote.offset             = parameters->rdma.remote.offset;

            get->typed.protocol                  = this;
            get->typed.task                      = task;
            get->typed.offset                    = offset;

            TRACE_STRING("before first pipeline start.");

            // Start the pipelined transfer
            if (likely(next_pipeline (get)))
              {
                TRACE_STRING("before second pipeline start.");
                next_pipeline (get);
              }

            TRACE_FN_EXIT();

            return PAMI_SUCCESS;
          }

        protected:

          T_Model                                   _model;
          T_Device                                & _device;
          MemoryAllocator < sizeof(state_t), 16 >   _allocator;
          pami_context_t                            _context;
      };
    };
  };
};
#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif /* __p2p_protocols_rget_GetRdma_h__ */

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
