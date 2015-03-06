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
/// \file p2p/protocols/Fence.h
/// \brief Defines base class interface for fence operations.
///
#ifndef __p2p_protocols_Fence_h__
#define __p2p_protocols_Fence_h__

#include <pami.h>

#include "components/memory/MemoryAllocator.h"

#include "util/trace.h"

#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  namespace Protocol
  {
    namespace Fence
    {

      // Forward declaration
      template <class T0, class T1> class CompositeFence;

      template <class T0, class T1, class T_Allocator>
      static CompositeFence<T0, T1> * generate (T0            * fence0,
                                                T1            * fence1,
                                                T_Allocator   & allocator,
                                                pami_result_t & result)
      {
        TRACE_FN_ENTER();
        COMPILE_TIME_ASSERT(sizeof(CompositeFence<T0, T1>) <= T_Allocator::objsize);

        void * composite = allocator.allocateObject ();
        new (composite) CompositeFence<T0, T1> (fence0, fence1, result);

        if (result != PAMI_SUCCESS)
          {
            allocator.returnObject (composite);
            composite = NULL;
          }

        TRACE_FN_EXIT();
        return (CompositeFence<T0, T1> *) composite;
      };

      ///
      /// \brief Base class for point-to-point fence implementations.
      ///
      class Fence
      {
        protected:

          ///
          /// \brief Base class constructor for point-to-point fence operations.
          ///
          inline Fence () {};

          ///
          /// \brief Base class destructor.
          ///
          /// \note Any class with virtual functions must define a virtual
          ///       destructor.
          ///
          virtual ~Fence () {};

          /// \note This is required to make "C" programs link successfully with virtual destructors
          inline void operator delete(void * p)
          {
            PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
          }

        public:

          ///
          /// \brief Start a new fence operation to a single endpoint
          ///
          /// \param [in] done_fn Completion callback function
          /// \param [in] cookie  Completion callback cookie
          /// \param [in] target  Fence target endpoint
          ///
          virtual pami_result_t endpoint (pami_event_function   done_fn,
                                          void                * cookie,
                                          pami_endpoint_t       target) = 0;

          ///
          /// \brief Start a new fence operation to all endpoints
          ///
          /// \param [in] done_fn Completion callback function
          /// \param [in] cookie  Completion callback cookie
          ///
          virtual pami_result_t all (pami_event_function   done_fn,
                                     void                * cookie) = 0;

      }; // PAMI::Protocol::Fence::Fence class

      class Error : public Fence
      {
        public:

          inline Error () {};

          virtual ~Error () {};

          virtual pami_result_t endpoint (pami_event_function   done_fn,
                                          void                * cookie,
                                          pami_endpoint_t       target)
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT( "return PAMI_ERROR (%d)", PAMI_ERROR);
            TRACE_FN_EXIT();
            return PAMI_ERROR;
          };

          virtual pami_result_t all (pami_event_function   done_fn,
                                     void                * cookie)
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT( "return PAMI_ERROR (%d)", PAMI_ERROR);
            TRACE_FN_EXIT();
            return PAMI_ERROR;
          };

      }; // PAMI::Protocol::Fence::Error class

      template <class T0, class T1>
      class CompositeFence : public Fence
      {
        protected:

          typedef struct
          {
            volatile unsigned     active;
            pami_event_function   done_fn;
            void                * cookie;
            MemoryAllocator<32, 16> * allocator;
          } state_t;


          static void fence_done (pami_context_t   context,
                                  void           * cookie,
                                  pami_result_t    result)
          {
            TRACE_FN_ENTER();

            state_t * state = (state_t *) cookie;

            TRACE_FORMAT( "state->active: %d -> %d", state->active, state->active - 1);
            state->active--;

            if (state->active == 0)
              {
                pami_event_function f = state->done_fn;
                void * c = state->cookie;

                TRACE_FORMAT( "before allocator->returnObject(%p)", cookie);
                state->allocator->returnObject(cookie);

                TRACE_FORMAT( "before f(%p,%p,%d)", context, c, PAMI_SUCCESS);

                if (f != NULL)
                  f (context, c, PAMI_SUCCESS);
              }

            TRACE_FN_EXIT();
            return;
          };

          MemoryAllocator<32, 16> _allocator;
          T0 * _fence0;
          T1 * _fence1;


        public:

          inline CompositeFence (T0 * fence0, T1 * fence1, pami_result_t & result) :
              _allocator(),
              _fence0 (fence0),
              _fence1 (fence1)
          {
            result = PAMI_SUCCESS;
          };

          virtual ~CompositeFence () {};

          virtual pami_result_t endpoint (pami_event_function   done_fn,
                                          void                * cookie,
                                          pami_endpoint_t       target)
          {
            TRACE_FN_ENTER();

            pami_result_t result = PAMI_ERROR;

            state_t * state = (state_t *) _allocator.allocateObject();

            state->active    = 2;
            state->done_fn   = done_fn;
            state->cookie    = cookie;
            state->allocator = & _allocator;

            result = _fence0->endpoint (fence_done, (void *) state, target);

            if (result != PAMI_SUCCESS)
              {
                TRACE_FORMAT( "_fence0->endpoint() returned %d", result);
                TRACE_FN_EXIT();
                return result;
              }

            result = _fence1->endpoint (fence_done, (void *) state, target);

            if (result != PAMI_SUCCESS)
              {
                TRACE_FORMAT( "_fence1->endpoint() returned %d", result);
                TRACE_FN_EXIT();
                return result;
              }

            TRACE_FN_EXIT();
            return result;
          };

          virtual pami_result_t all (pami_event_function   done_fn,
                                     void                * cookie)
          {
            TRACE_FN_ENTER();

            pami_result_t result = PAMI_ERROR;

            state_t * state = (state_t *) _allocator.allocateObject();

            state->active    = 2;
            state->done_fn   = done_fn;
            state->cookie    = cookie;
            state->allocator = & _allocator;

            result = _fence0->all (fence_done, (void *) state);

            if (result != PAMI_SUCCESS)
              {
                TRACE_FORMAT( "_fence0->endpoint() returned %d", result);
                TRACE_FN_EXIT();
                return result;
              }

            result = _fence1->all (fence_done, (void *) state);

            if (result != PAMI_SUCCESS)
              {
                TRACE_FORMAT( "_fence1->endpoint() returned %d", result);
                TRACE_FN_EXIT();
                return result;
              }

            TRACE_FN_EXIT();
            return result;
          };

      }; // PAMI::Protocol::Fence::CompositeFence class
    };   // PAMI::Protocol::Fence namespace
  };     // PAMI::Protocol namespace
};       // PAMI namespace
#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif // __p2p_protocols_Fence_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
