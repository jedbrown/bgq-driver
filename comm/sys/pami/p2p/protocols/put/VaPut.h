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
/// \file p2p/protocols/put/VaPut.h
/// \brief Put protocol devices that implement the 'dma' interface.
///
/// The Put class defined in this file uses C++ templates
/// and the "dma" device interface which also uses C++ templates.
///
/// C++ templates require all source code to be #include'd from a header file
/// which can result in a single very large file.
///
#ifndef __p2p_protocols_put_VaPut_h__
#define __p2p_protocols_put_VaPut_h__

#include <string.h>

#include "p2p/protocols/Put.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf x
#endif

namespace PAMI
{
  namespace Protocol
  {
    namespace Put
    {
      template <class T_Model, class T_Device>
      class VaPut : public Put
      {
        protected:

          typedef uint8_t msg_t[T_Model::dma_model_state_bytes];

          typedef struct put_state
          {
            msg_t                   msg;
            pami_event_function     fn;
            void                  * cookie;
            VaPut<T_Model,T_Device> * protocol;
//            MemoryAllocator < sizeof(put_state_t), 16 > * allocator;
          } put_state_t;

          ///
          /// \brief Local put completion event callback.
          ///
          /// This callback will invoke the application local completion
          /// callback function and free the transfer state memory.
          ///
          static void complete (pami_context_t   context,
                                void           * cookie,
                                pami_result_t    result)
          {
            put_state_t * state = (put_state_t *) cookie;

            if (state->fn != NULL)
              {
                state->fn (context, state->cookie, PAMI_SUCCESS);
              }

            state->protocol->_allocator.returnObject ((void *) state);

            return;
          }

        public:

          template <class T_Allocator>
          static Put * generate (T_Device      & device,
                                 T_Allocator   & allocator,
                                 pami_result_t & status)
          {
            TRACE_ERR((stderr, ">> Put::generate()\n"));
            COMPILE_TIME_ASSERT(sizeof(Put) <= T_Allocator::objsize);

            Put * put = (Put *) allocator.allocateObject ();
            new ((void *)put) VaPut (device, status);
            if (status != PAMI_SUCCESS)
            {
              allocator.returnObject (put);
              put = NULL;
            }

            TRACE_ERR((stderr, "<< Put::generate(), put = %p, status = %d\n", put, status));
            return put;
          }


          inline VaPut (T_Device & device, pami_result_t & status) :
              _model (device, status),
              _context (device.getContext())
          {
            COMPILE_TIME_ASSERT(T_Model::dma_model_va_supported == true);
          }

          ///
          /// \brief Start a new contiguous put operation
          ///
          inline pami_result_t simple (pami_put_simple_t * parameters)
          {
            pami_task_t task;
            size_t offset;
            PAMI_ENDPOINT_INFO(parameters->rma.dest, task, offset);

            if (_model.postDmaPut (task,
                                   parameters->rma.bytes,
                                   parameters->addr.local,
                                   parameters->addr.remote))
              {
                if (parameters->rma.done_fn)
                  parameters->rma.done_fn (_context, parameters->rma.cookie, PAMI_SUCCESS);

                return PAMI_SUCCESS;
              }

            // Allocate memory to maintain the state of the put operation.
            put_state_t * state =
              (put_state_t *) _allocator.allocateObject();

            state->fn       = parameters->rma.done_fn;
            state->cookie   = parameters->rma.cookie;
            state->protocol = this;

            _model.postDmaPut (state->msg,
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
          MemoryAllocator < sizeof(put_state), 16 > _allocator;
          pami_context_t                            _context;
      };
    };
  };
};
#undef TRACE_ERR
#endif /* __p2p_protocols_put_VaPut_h__ */

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
