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
/// \file p2p/protocols/send/eager/Eager.h
/// \brief Eager send protocol for reliable devices.
///
/// The Eager class defined in this file uses C++ templates and
/// the device "message" interface - which also uses C++ templates.
///
/// C++ templates require all source code to be #include'd from a header file.
///
#ifndef __p2p_protocols_send_eager_Eager_h__
#define __p2p_protocols_send_eager_Eager_h__

#include "p2p/protocols/Send.h"
#include "p2p/protocols/send/eager/EagerSimple.h"

#include "util/trace.h"

#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  namespace Protocol
  {
    namespace Send
    {
      ///
      /// \brief Eager send protocol factory for deterministic, reliable network devices.
      ///
      /// \tparam T_Model Packet model interface implementation class
      ///
      /// \see PAMI::Device::Interface::PacketModel
      ///
      template < class T_ModelPrimary, class T_ModelSecondary = T_ModelPrimary >
      class Eager
      {
        public:

          template <configuration_t T_Option, bool T_Composite >
          class EagerImpl : public PAMI::Protocol::Send::Send
          {
            public:

              ///
              /// \brief Eager send protocol constructor.
              ///
              /// \param [in]  dispatch    Dispatch identifier
              /// \param [in]  dispatch_fn Point-to-point dispatch callback function
              /// \param [in]  cookie      Opaque application dispatch data
              /// \param [in]  device      Device that implements the message interface
              /// \param [in]  origin      Origin endpoint
              /// \param [in]  context     Origin context
              /// \param [in]  hint        Dispatch 'hard' hints
              /// \param [out] status      Constructor status
              ///
              template <class T_DevicePrimary, class T_DeviceSecondary>
              inline EagerImpl (size_t                       dispatch,
                                pami_dispatch_p2p_function   dispatch_fn,
                                void                       * cookie,
                                T_DevicePrimary            & device0,
                                T_DeviceSecondary          & device1,
                                pami_endpoint_t              origin,
                                pami_context_t               context,
                                pami_dispatch_hint_t         hint,
                                pami_result_t              & status) :
                  PAMI::Protocol::Send::Send (),
                  _primary (device0),
                  _secondary (device1),
                  _pwqAllocator (NULL)
              {
                TRACE_FN_ENTER();
                status = _primary.initialize (dispatch, dispatch_fn, cookie, origin, context, hint);

                if (T_Composite && status == PAMI_SUCCESS)
                  {
                    status = _secondary.initialize (dispatch, dispatch_fn, cookie, origin, context, hint);
                  }

                TRACE_FN_EXIT();
              };

              virtual ~EagerImpl () {};

              /// \note This is required to make "C" programs link successfully
              ///       with virtual destructors
              inline void operator delete(void * p)
              {
                PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
              }

              ///
              /// \brief Set the pointer to the mem allocator used by PWQ
              ///
              /// SendPWQ uses this memory allocator. Since SendPWQ class
              /// can't be constructed, we construct the allocator in NI and
              /// pass it to SendPWQ class through the composite.
              ///
              /// \see PAMI::Protocol::Send::setPWQAllocator
              ///
              virtual void setPWQAllocator(void * allocator)
              {
                 _pwqAllocator = allocator;
              }

              ///
              /// \brief Return a pointer to the mem allocator used by PWQ
              ///
              /// \see PAMI::Protocol::Send::getPWQAllocator
              ///
              virtual void * getPWQAllocator()
              {
                return _pwqAllocator;
              }

              ///
              /// \brief Query the value of one or more attributes
              ///
              /// \see PAMI::Protocol::Send::getAttributes
              ///
              virtual pami_result_t getAttributes (pami_configuration_t  configuration[],
                                                   size_t                num_configs)
              {
                TRACE_FN_ENTER();
                size_t i;

                for (i = 0; i < num_configs; i++)
                  {
                    TRACE_FORMAT( "configuration[%zu].name = %d", i, configuration[i].name);

                    switch (configuration[i].name)
                      {
                        case PAMI_DISPATCH_RECV_IMMEDIATE_MAX:
                          configuration[i].value.intval = EagerSimple<T_ModelPrimary, T_Option>::recv_immediate_max;

                          if (T_Composite)
                            {
                              if (EagerSimple<T_ModelSecondary, T_Option>::recv_immediate_max < configuration[i].value.intval)
                                configuration[i].value.intval = EagerSimple<T_ModelSecondary, T_Option>::recv_immediate_max;
                            }

                          TRACE_FORMAT( "configuration[%zu].value.intval = %zu", i, configuration[i].value.intval);
                          break;

                        case PAMI_DISPATCH_SEND_IMMEDIATE_MAX:
                          configuration[i].value.intval = T_ModelPrimary::packet_model_immediate_bytes;

                          if (T_Composite)
                            {
                              if (T_ModelSecondary::packet_model_immediate_bytes < configuration[i].value.intval)
                                configuration[i].value.intval = T_ModelSecondary::packet_model_immediate_bytes;
                            }

                          TRACE_FORMAT( "configuration[%zu].value.intval = %zu", i, configuration[i].value.intval);
                          break;

                        default:
                          TRACE_FN_EXIT();
                          return PAMI_INVAL;
                          break;
                      };
                  };

                TRACE_FN_EXIT();

                return PAMI_SUCCESS;
              };

              ///
              /// \brief Start a new immediate send operation.
              ///
              /// \see PAMI::Protocol::Send::immediate
              ///
              virtual pami_result_t immediate (pami_send_immediate_t * parameters)
              {
                TRACE_FN_ENTER();
                pami_result_t result = _primary.immediate_impl (parameters);

                if (T_Composite && result != PAMI_SUCCESS)
                  {
                    result = _secondary.immediate_impl (parameters);

                  }

                TRACE_FN_EXIT();
                return result;
              };

              ///
              /// \brief Start a new simple send operation.
              ///
              /// \see PAMI::Protocol::Send::simple
              ///
              virtual pami_result_t simple (pami_send_t * parameters)
              {
                TRACE_FN_ENTER();
                pami_result_t result = _primary.simple_impl (parameters);

                if (T_Composite && result != PAMI_SUCCESS)
                  {
                    result = _secondary.simple_impl (parameters);
                  }

                TRACE_FN_EXIT();
                return result;
              };

              ///
              /// \brief Start a new typed send operation.
              ///
              /// \see PAMI::Protocol::Send::typed
              ///
              virtual pami_result_t typed (pami_send_typed_t * parameters)
              {
                TRACE_FN_ENTER();
                pami_result_t result = _primary.typed_impl (parameters);

                if (T_Composite && result != PAMI_SUCCESS)
                  {
                    result = _secondary.typed_impl (parameters);
                  }

                TRACE_FN_EXIT();
                return result;
              };

            protected:

              EagerSimple<T_ModelPrimary, T_Option>   _primary;
              EagerSimple<T_ModelSecondary, T_Option> _secondary;
              void *                                  _pwqAllocator;

          }; // PAMI::Protocol::Send::EagerImpl class

        protected:

          template <class T_MemoryManager, class T_DevicePrimary, class T_DeviceSecondary>
          static Send * generate (size_t                       dispatch,
                                  pami_dispatch_p2p_function   dispatch_fn,
                                  void                       * cookie,
                                  T_DevicePrimary            & device0,
                                  T_DeviceSecondary          & device1,
                                  pami_endpoint_t              origin,
                                  pami_context_t               context,
                                  pami_dispatch_hint_t         options,
                                  T_MemoryManager            * mm,
                                  pami_result_t              & result,
                                  bool                         composite)
          {
            TRACE_FN_ENTER();

            // Return an error for invalid / unimplemented 'hard' hints.
            if (
              options.remote_async_progress == PAMI_HINT_ENABLE  ||
              options.use_rdma              == PAMI_HINT_ENABLE  ||
              false)
              {
                result = PAMI_ERROR;
                TRACE_FN_ENTER();
                return (Send *) NULL;
              }

            void * eager = NULL;

            if (options.queue_immediate == PAMI_HINT_DISABLE)
              {
                const configuration_t queue_immediate = (configuration_t) (QUEUE_IMMEDIATE_DISABLE);

                if (options.long_header == PAMI_HINT_DISABLE)
                  {
                    const configuration_t long_header = (configuration_t) (queue_immediate | LONG_HEADER_DISABLE);

                    if (options.recv_immediate == PAMI_HINT_ENABLE)
                      {
                        const configuration_t hint = (configuration_t) (long_header | RECV_IMMEDIATE_FORCEON);

                        if (composite)
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, true>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, true> (dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                        else
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, false>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, false>(dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                      }
                    else if (options.recv_immediate == PAMI_HINT_DISABLE)
                      {
                        const configuration_t hint = (configuration_t) (long_header | RECV_IMMEDIATE_FORCEOFF);

                        if (composite)
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, true>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, true> (dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                        else
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, false>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, false>(dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                      }
                    else
                      {
                        const configuration_t hint = (configuration_t) (long_header);

                        if (composite)
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, true>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, true> (dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                        else
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, false>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, false>(dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                      }
                  }
                else
                  {
                    const configuration_t long_header = (configuration_t) (DEFAULT);

                    if (options.recv_immediate == PAMI_HINT_ENABLE)
                      {
                        const configuration_t hint = (configuration_t) (long_header | RECV_IMMEDIATE_FORCEON);

                        if (composite)
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, true>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, true> (dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                        else
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, false>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, false>(dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                      }
                    else if (options.recv_immediate == PAMI_HINT_DISABLE)
                      {
                        const configuration_t hint = (configuration_t) (long_header | RECV_IMMEDIATE_FORCEOFF);

                        if (composite)
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, true>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, true> (dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                        else
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, false>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, false>(dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                      }
                    else
                      {
                        const configuration_t hint = (configuration_t) (long_header);

                        if (composite)
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, true>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, true> (dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                        else
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, false>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, false>(dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                      }
                  }

              }
            else
              {
                const configuration_t queue_immediate = (configuration_t) (DEFAULT);

                if (options.long_header == PAMI_HINT_DISABLE)
                  {
                    const configuration_t long_header = (configuration_t) (queue_immediate | LONG_HEADER_DISABLE);

                    if (options.recv_immediate == PAMI_HINT_ENABLE)
                      {
                        const configuration_t hint = (configuration_t) (long_header | RECV_IMMEDIATE_FORCEON);

                        if (composite)
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, true>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, true> (dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                        else
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, false>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, false>(dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                      }
                    else if (options.recv_immediate == PAMI_HINT_DISABLE)
                      {
                        const configuration_t hint = (configuration_t) (long_header | RECV_IMMEDIATE_FORCEOFF);

                        if (composite)
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, true>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, true> (dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                        else
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, false>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, false>(dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                      }
                    else
                      {
                        const configuration_t hint = (configuration_t) (long_header);

                        if (composite)
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, true>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, true> (dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                        else
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, false>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, false>(dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                      }
                  }
                else
                  {
                    const configuration_t long_header = (configuration_t) (DEFAULT);

                    if (options.recv_immediate == PAMI_HINT_ENABLE)
                      {
                        const configuration_t hint = (configuration_t) (long_header | RECV_IMMEDIATE_FORCEON);

                        if (composite)
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, true>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, true> (dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                        else
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, false>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, false>(dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                      }
                    else if (options.recv_immediate == PAMI_HINT_DISABLE)
                      {
                        const configuration_t hint = (configuration_t) (long_header | RECV_IMMEDIATE_FORCEOFF);

                        if (composite)
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, true>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, true> (dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                        else
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, false>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, false>(dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                      }
                    else
                      {
                        const configuration_t hint = (configuration_t) (long_header);

                        if (composite)
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, true>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, true> (dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                        else
                          {
                            result = mm->memalign((void **) & eager, 16, sizeof(EagerImpl<hint, false>));
                            PAMI_assert_alwaysf(result == PAMI_SUCCESS, "Failed to get memory for eager send protocol");
                            new (eager) EagerImpl<hint, false>(dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, result);
                          }
                      }
                  }

              }

            if (result != PAMI_SUCCESS)
              {
                if (eager != NULL)
                  mm->free(eager);

                eager = NULL;
              }

            TRACE_FORMAT( "eager = %p, result = %d, dispatch = %zu", eager, result, dispatch);
            TRACE_FN_EXIT();
            return (Send *) eager;
          };

        public:

          template <class T_MemoryManager, class T_DevicePrimary, class T_DeviceSecondary>
          static Send * generate (size_t                       dispatch,
                                  pami_dispatch_p2p_function   dispatch_fn,
                                  void                       * cookie,
                                  T_DevicePrimary            & device0,
                                  T_DeviceSecondary          & device1,
                                  pami_endpoint_t              origin,
                                  pami_context_t               context,
                                  pami_dispatch_hint_t         options,
                                  T_MemoryManager            * mm,
                                  pami_result_t              & result)
          {
            return generate (dispatch, dispatch_fn, cookie, device0, device1, origin, context, options, mm, result, true);
          };

          template <class T_MemoryManager, class T_Device>
          static Send * generate (size_t                       dispatch,
                                  pami_dispatch_p2p_function   dispatch_fn,
                                  void                       * cookie,
                                  T_Device                   & device,
                                  pami_endpoint_t              origin,
                                  pami_context_t               context,
                                  pami_dispatch_hint_t         options,
                                  T_MemoryManager            * mm,
                                  pami_result_t              & result)
          {
            return generate (dispatch, dispatch_fn, cookie, device, device, origin, context, options, mm, result, false);
          };
      };     // PAMI::Protocol::Send::Eager class
    };       // PAMI::Protocol::Send namespace
  };         // PAMI::Protocol namespace
};           // PAMI namespace
#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif // __pami_p2p_protocol_send_eager_eager_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
