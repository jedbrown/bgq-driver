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
/// \file p2p/protocols/rput/PutRdma.h
/// \brief Put protocol devices that implement the 'dma' interface.
///
/// The Put class defined in this file uses C++ templates
/// and the "dma" device interface which also uses C++ templates.
///
/// C++ templates require all source code to be #include'd from a header file
/// which can result in a single very large file.
///
#ifndef __p2p_protocols_rput_PutRdma_h__
#define __p2p_protocols_rput_PutRdma_h__

#include <string.h>

#include "p2p/protocols/RPut.h"

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
    namespace Put
    {
      template <class T_Model, class T_Device>
      class PutRdma : public RPut
      {
        protected:

          typedef PutRdma<T_Model, T_Device> Protocol;

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
            model_state_t         state;
            pami_event_function   rdone_fn;
            void                * cookie;
            Protocol            * protocol;
          } fence_t;

          typedef struct
          {
            model_state_t         state[2][10];
            uint8_t               enumerator[sizeof(Type::TypeEnumerator)];
            size_t                pipeline_start_count;
            size_t                pipeline_completion_count;
            size_t                bytes_remaining;
            void                * cookie;
            pami_event_function   done_fn;
            pami_event_function   rdone_fn;
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
            fence_t               fence;
          } state_t;

          void start_fence (pami_task_t           task,
                            size_t                offset,
                            pami_event_function   rdone_fn,
                            void                * cookie,
                            pami_memregion_t    * local_mr,
                            pami_memregion_t    * remote_mr)
          {
            TRACE_FN_ENTER();

            // Allocate memory to maintain the state of the zero-byte put
            // 'fence' operation.
            state_t * put = (state_t *) _allocator.allocateObject();

            put->fence.rdone_fn = rdone_fn;
            put->fence.cookie   = cookie;
            put->fence.protocol = this;

            _model.postDmaPut (put->fence.state,
                               complete_fence, (void *) put,
                               task, offset, 0,
                               (Memregion *) local_mr, 0,
                               (Memregion *) remote_mr, 0);

            TRACE_FN_EXIT();
          };

          static void complete_fence (pami_context_t   context,
                                      void           * cookie,
                                      pami_result_t    result)
          {
            TRACE_FN_ENTER();

            state_t * put = (state_t *) cookie;

            PAMI_assert_debug(put->fence.rdone_fn != NULL);

            put->fence.rdone_fn (context, put->fence.cookie, PAMI_SUCCESS);
            put->fence.protocol->_allocator.returnObject ((void *) put);

            TRACE_FN_EXIT();
          };

          static void complete_simple (pami_context_t   context,
                                       void           * cookie,
                                       pami_result_t    result)
          {
            TRACE_FN_ENTER();

            state_t * put = (state_t *) cookie;

            if (put->simple.done_fn != NULL)
              put->simple.done_fn (context, put->simple.cookie, PAMI_SUCCESS);

            put->simple.protocol->_allocator.returnObject ((void *) put);

            TRACE_FN_EXIT();
          };

          static void complete_typed (pami_context_t   context,
                                      void           * cookie,
                                      pami_result_t    result)
          {
            TRACE_FN_ENTER();

            state_t * put = (state_t *) cookie;

            // Increment the pipeline completion count. If all bytes have been
            // sent and the pipline start count equals the pipeline completion
            // count, then the entire typed transfer is complete.
            put->typed.pipeline_completion_count++;

            TRACE_FORMAT("put->typed.pipeline_start_count:      %zu", put->typed.pipeline_start_count);
            TRACE_FORMAT("put->typed.pipeline_completion_count: %zu", put->typed.pipeline_completion_count);
            TRACE_FORMAT("put->typed.bytes_remaining:           %zu", put->typed.bytes_remaining);

            if (unlikely(put->typed.bytes_remaining == 0))
              {
                if (put->typed.pipeline_completion_count ==
                    put->typed.pipeline_start_count)
                  {
                    TRACE_FORMAT("put->typed.done_fn: %p", put->typed.done_fn);

                    if (put->typed.done_fn != NULL)
                      put->typed.done_fn (context, put->typed.cookie, PAMI_SUCCESS);

                    TRACE_FORMAT("put->typed.rdone_fn: %p", put->typed.rdone_fn);

                    if (put->typed.rdone_fn != NULL)
                      put->typed.protocol->start_fence (put->typed.task,
                                                        put->typed.offset,
                                                        put->typed.rdone_fn,
                                                        put->typed.cookie,
                                                        put->typed.local.mr,
                                                        put->typed.remote.mr);

                    put->typed.protocol->_allocator.returnObject ((void *) put);
                  }

                TRACE_FN_EXIT();
                return;
              }

            put->typed.protocol->send_next_pipeline (put);

            TRACE_FN_EXIT();
          };

          inline bool send_next_pipeline (state_t * put)
          {
            TRACE_FN_ENTER();

            const size_t task   = put->typed.task;
            const size_t offset = put->typed.offset;

            Memregion * local_mr  = (Memregion *) put->typed.local.mr;
            Memregion * remote_mr = (Memregion *) put->typed.remote.mr;

            const size_t local_offset  = put->typed.local.offset;
            const size_t remote_offset = put->typed.remote.offset;

            Type::TypeEnumerator * tmp = (Type::TypeEnumerator *) put->typed.enumerator;
            Type::TypeEnumerator & enumerator = *tmp;

            size_t & bytes_remaining = put->typed.bytes_remaining;

            // Determine the pipeline that completed and update the start count
            // value for the next send_next_pipeline().
            unsigned pipeline = ++(put->typed.pipeline_start_count) & 0x01;

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
                    _model.postDmaPut (put->typed.state[pipeline][block],
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
                    _model.postDmaPut (put->typed.state[pipeline][block],
                                       complete_typed, (void *) put,
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
            _model.postDmaPut (put->typed.state[pipeline][block],
                               complete_typed, (void *) put,
                               task, offset, block_size,
                               local_mr, local_offset + block_local,
                               remote_mr, remote_offset + block_remote);

            TRACE_FN_EXIT();
            return (bytes_remaining != 0);
          }

        public:

          template <class T_MemoryManager>
          static PutRdma * generate (T_Device        & device,
                                     pami_context_t    context,
                                     T_MemoryManager * mm,
                                     pami_result_t   & status)
          {
            TRACE_FN_ENTER();

            void * protocol = NULL;
            status = mm->memalign((void **) & protocol, 16, sizeof(PutRdma));
            PAMI_assert_alwaysf(status == PAMI_SUCCESS, "Failed to get memory for rdma put protocol");
            new (protocol) PutRdma (device, context, status);

            TRACE_FN_EXIT();

            return (PutRdma *) protocol;;
          }


          inline PutRdma (T_Device & device, pami_context_t context, pami_result_t & status) :
              _model (device, status),
              _device (device),
              _context (context)
          {
            TRACE_FN_ENTER();

            COMPILE_TIME_ASSERT(T_Model::dma_model_mr_supported == true);

            TRACE_FN_EXIT();
          }

          ///
          /// \brief Start a new contiguous rdma put operation
          ///
          virtual pami_result_t simple (pami_rput_simple_t * parameters)
          {
            TRACE_FN_ENTER();

            pami_task_t task;
            size_t offset;
            PAMI_ENDPOINT_INFO(parameters->rma.dest, task, offset);

            // Verify that this task is addressable by this dma device
            if (unlikely(_device.isPeer(task) == false)) return PAMI_ERROR;

            // Attempt an immediate post of the put operation
            if (_model.postDmaPut (task, offset, parameters->rma.bytes,
                                   (Memregion *) parameters->rdma.local.mr,
                                   parameters->rdma.local.offset,
                                   (Memregion *) parameters->rdma.remote.mr,
                                   parameters->rdma.remote.offset))
              {
                if (parameters->rma.done_fn)
                  parameters->rma.done_fn (_context, parameters->rma.cookie, PAMI_SUCCESS);
              }
            else
              {
                // The immediate post was not successful. Allocate memory to
                // maintain the state of the non-blocking put operation.
                state_t * put = (state_t *) _allocator.allocateObject();

                put->simple.done_fn  = parameters->rma.done_fn;
                put->simple.cookie   = parameters->rma.cookie;
                put->simple.protocol = this;

                _model.postDmaPut (put->simple.state,
                                   complete_simple, (void *) put,
                                   task, offset, parameters->rma.bytes,
                                   (Memregion *) parameters->rdma.local.mr,
                                   parameters->rdma.local.offset,
                                   (Memregion *) parameters->rdma.remote.mr,
                                   parameters->rdma.remote.offset);
              }


            if (unlikely(parameters->put.rdone_fn != NULL))
              start_fence (task, offset,
                           parameters->put.rdone_fn,
                           parameters->rma.cookie,
                           parameters->rdma.local.mr,
                           parameters->rdma.remote.mr);

            TRACE_FN_EXIT();

            return PAMI_SUCCESS;
          };

          ///
          /// \brief Start a new non-contiguous rput operation
          ///
          virtual pami_result_t typed (pami_rput_typed_t * parameters)
          {
            TRACE_FN_ENTER();

            pami_task_t task;
            size_t offset;
            PAMI_ENDPOINT_INFO(parameters->rma.dest, task, offset);

            // Verify that this task is addressable by this dma device
            if (unlikely(_device.isPeer(task) == false)) return PAMI_ERROR;

            state_t * put = (state_t *) _allocator.allocateObject();

            TRACE_STRING("before enumerator constructor.");

            Type::TypeEnumerator * enumerator =
              (Type::TypeEnumerator *) put->typed.enumerator;
            new (enumerator) Type::TypeEnumerator ((Type::TypeCode *) parameters->type.local,
                                                   (Type::TypeCode *) parameters->type.remote);

            // Initialize the put state;
            put->typed.pipeline_start_count      = 0;
            put->typed.pipeline_completion_count = 0;
            put->typed.bytes_remaining           = parameters->rma.bytes;

            put->typed.cookie                    = parameters->rma.cookie;
            put->typed.done_fn                   = parameters->rma.done_fn;
            put->typed.rdone_fn                  = parameters->put.rdone_fn;
            put->typed.local.mr                  = parameters->rdma.local.mr;
            put->typed.local.offset              = parameters->rdma.local.offset;
            put->typed.remote.mr                 = parameters->rdma.remote.mr;
            put->typed.remote.offset             = parameters->rdma.remote.offset;

            put->typed.protocol                  = this;
            put->typed.task                      = task;
            put->typed.offset                    = offset;

            TRACE_STRING("before first pipeline start.");

            // Start the pipelined transfer
            if (likely(send_next_pipeline (put)))
              {
                TRACE_STRING("before second pipeline start.");
                send_next_pipeline (put);
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

#endif /* __p2p_protocols_rput_PutRdma_h__ */

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
