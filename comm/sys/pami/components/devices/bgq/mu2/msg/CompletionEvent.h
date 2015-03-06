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
/**
 * \file components/devices/bgq/mu2/msg/CompletionEvent.h
 * \brief ???
 */

#ifndef __components_devices_bgq_mu2_msg_CompletionEvent_h__
#define __components_devices_bgq_mu2_msg_CompletionEvent_h__

#include "util/queue/Queue.h"

#include "util/trace.h"
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      class CompletionEvent : public PAMI::Queue::Element
      {
        public:

          ///
          /// \brief Track completion of an injection fifo sequence number
          ///
          /// \param[in] fn         Completion event function
          /// \param[in] inj_cookie Completion event injection fifo cookie (a.k.a. pami_context_t)
          /// \param[in] fn_cookie  Completion event function cookie
          /// \param[in] sequence   Injection fifo sequence number
          ///
          inline CompletionEvent (pami_event_function    fn,
                                  void                 * inj_cookie,
                                  void                 * fn_cookie,
                                  uint64_t               sequence) :
              _fn (fn),
              _inj_cookie (inj_cookie),
              _fn_cookie (fn_cookie),
              _sequence (sequence)
          {
            TRACE_FN_ENTER();

            TRACE_FN_EXIT();
          };

          inline ~CompletionEvent () {};


          ///
          /// \brief Determine completion state
          ///
          /// \param[in] current Injection fifo sequence number to compare
          ///
          /// \retval true  Completion event "done"
          /// \retval false Completion event "not done"
          ///
          inline bool isDone (uint64_t current)
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT("current = %ld, _sequence = %ld",current,_sequence);
            TRACE_FN_EXIT();
            return (_sequence <= current);
          };

          ///
          /// \brief Determine completion state and invoke function if needed
          ///
          /// \param[in] current Injection fifo sequence number to compare
          ///
          /// \retval true  Completion function invoked
          /// \retval false Completion function \b not invoked
          ///
          inline void invoke ()
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT("_inj_cookie = %p, _fn_cookie = %p",_inj_cookie,_fn_cookie);
            _fn (_inj_cookie, _fn_cookie, PAMI_SUCCESS);
            TRACE_FN_EXIT();
          };

        protected:

          pami_event_function   _fn;
          void                * _inj_cookie;
          void                * _fn_cookie;
          uint64_t              _sequence;

      }; // class     PAMI::Device::MU::CompletionEvent
    };   // namespace PAMI::Device::MU
  };     // namespace PAMI::Device
};       // namespace PAMI

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#endif // __components_devices_bgq_mu2_msg_CompletionEvent_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
