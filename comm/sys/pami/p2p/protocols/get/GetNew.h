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
/// \file p2p/protocols/get/GetNew.h
/// \brief Get protocol devices that implement the 'dma' interface.
///
/// The Get class defined in this file uses C++ templates
/// and the "dma" device interface which also uses C++ templates.
///
/// C++ templates require all source code to be #include'd from a header file
/// which can result in a single very large file.
///
#ifndef __p2p_protocols_get_GetNew_h__
#define __p2p_protocols_get_GetNew_h__

#include <string.h>

#include "p2p/protocols/Get.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf x
#endif

namespace PAMI
{
  namespace Protocol
  {
    namespace Get
    {
      template <class T_Model, class T_Device>
      class Get : public Get
      {
        protected:

          typedef uint8_t msg_t[T_Model::dma_model_state_bytes];

          typedef struct get_state
          {
            msg_t                   msg;
            pami_event_function     fn;
            void                  * cookie;
            Get<T_Model,T_Device> * protocol;
//            MemoryAllocator < sizeof(get_state_t), 16 > * allocator;
          } get_state_t;

          ///
          /// \brief Local get completion event callback.
          ///
          /// This callback will invoke the application local completion
          /// callback function and free the transfer state memory.
          ///
          static void complete (pami_context_t   context,
                                void           * cookie,
                                pami_result_t    result)
          {
            get_state_t * state = (get_state_t *) cookie;

            if (state->fn != NULL)
              {
                state->fn (context, state->cookie, PAMI_SUCCESS);
              }

            state->protocol->_allocator.returnObject ((void *) state);

            return;
          }

        public:

          template <class T_Allocator>
          static Get * generate (T_Device      & device,
                                 T_Allocator   & allocator,
                                 pami_result_t & status)
          {
            TRACE_ERR((stderr, ">> Get::generate()\n"));
            COMPILE_TIME_ASSERT(sizeof(Get) <= T_Allocator::objsize);

            Get * get = (Get *) allocator.allocateObject ();
            new ((void *)get) Get (device, status);
            if (status != PAMI_SUCCESS)
            {
              allocator.returnObject (get);
              get = NULL;
            }

            TRACE_ERR((stderr, "<< Get::generate(), get = %p, status = %d\n", get, status));
            return get;
          }


          inline Get (T_Device & device, pami_result_t & status) :
              _model (device, status),
              _context (device.getContext())
          {
            COMPILE_TIME_ASSERT(T_Model::dma_model_va_supported == true);
          }

          ///
          /// \brief Start a new contiguous get operation
          ///
          inline pami_result_t get (pami_get_simple_t * parameters)
          {
            pami_task_t task;
            size_t offset;
            PAMI_ENDPOINT_INFO(parameters->rma.dest, task, offset);

            if (_model.postDmaGet (task,
                                   parameters->rma.bytes,
                                   parameters->addr.local,
                                   parameters->addr.remote))
              {
                if (parameters->rma.done_fn)
                  parameters->rma.done_fn (_context, parameters->rma.cookie, PAMI_SUCCESS);

                return PAMI_SUCCESS;
              }

            // Allocate memory to maintain the state of the get operation.
            get_state_t * state =
              (get_state_t *) _allocator.allocateObject();

            state->fn       = parameters->rma.done_fn;
            state->cookie   = parameters->rma.cookie;
            state->protocol = this;

            _model.postDmaGet (state->msg,
                               complete,
                               (void *) state,
                               task,
                               parameters->rma.bytes,
                               parameters->addr.local,
                               parameters->addr.remote);

            return PAMI_SUCCESS;
          };

        protected:

          T_Model                                   _model;
          MemoryAllocator < sizeof(get_state), 16 > _allocator;
          pami_context_t                            _context;
      };
    };
  };
};
#undef TRACE_ERR
#endif /* __p2p_protocols_get_Get_h__ */

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
