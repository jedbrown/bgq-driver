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
 * \file components/devices/bgq/mu2/msg/InjectDescriptorMessage.h
 * \brief ???
 */

#ifndef __components_devices_bgq_mu2_msg_InjectDescriptorMessage_h__
#define __components_devices_bgq_mu2_msg_InjectDescriptorMessage_h__

#include "components/devices/bgq/mu2/InjChannel.h"
#include "components/devices/bgq/mu2/msg/MessageQueue.h"

#include "util/trace.h"
#undef DO_TRACE_ENTEREXIT 
#undef DO_TRACE_DEBUG     

#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      ///
      /// \brief Inject one or more descriptors into an inject fifo
      ///
      /// \tparam T_Num        Number of descriptors that will be injected
      /// \tparam T_Completion Perform completion processing
      ///
      template < unsigned T_Num, bool T_Completion = true >
      class InjectDescriptorMessage : public MessageQueue::Element
      {
        public:

          ///
          /// \brief Inject descriptor(s) into a specific injection fifo
          ///
          /// \param[in] injfifo Specific injection fifo for the descriptor(s)
          ///
          inline InjectDescriptorMessage (InjChannel & channel) :
              _channel (channel),
              _next (0),
              _nactive(T_Num),
              _fn (NULL),
              _cookie (NULL)
          {
            TRACE_FN_ENTER();

            TRACE_FN_EXIT();
          };

          inline InjectDescriptorMessage (InjChannel          & channel,
                                          pami_event_function   fn,
                                          void                * cookie) :
              _channel (channel),
              _next (0),
              _nactive(T_Num),
              _fn (fn),
              _cookie (cookie)
          {
            TRACE_FN_ENTER();
            COMPILE_TIME_ASSERT(T_Completion == true);
            TRACE_FN_EXIT();
          };

          inline ~InjectDescriptorMessage () {};

          ///
          /// \brief Inject descriptor message virtual advance implementation
          ///
          /// The inject descriptor message is complete only after all
          /// descriptors have been injected into the injection fifo.
          ///
          /// The number of descriptors to inject is specified as a template
          /// parameter for the class.
          ///
          /// \see MU::MessageQueue::Element::advance
          ///
          /// \retval true  Message complete
          /// \retval false Message incomplete and must remain on the queue
          ///
          virtual bool advance ()
          {
            TRACE_FN_ENTER();

            size_t ndesc = _channel.getFreeDescriptorCountWithUpdate ();
	    TRACE_FORMAT("InjectDescriptorMessage:  advance:  ndesc=%zu\n",ndesc);

            // Clone the message descriptors directly into the injection fifo.
            MUSPI_DescriptorBase * d =
              (MUSPI_DescriptorBase *) _channel.getNextDescriptor ();

            size_t i;
            uint64_t sequence = 0;

            for (i = 0; i < ndesc && (_next + i) < _nactive; i++)
              {
                desc[_next + i].clone (d[i]);
                TRACE_FORMAT("inject descriptor (%p) from message (%p)", &desc[_next+i], this);

                // Advance the injection fifo tail pointer. This will be
                // moved outside the loop when an "advance multiple" is
                // available.
                sequence = _channel.injFifoAdvanceDesc ();
              }

            _next += i;

            bool done = (_next == _nactive);

            if ((T_Completion == true) && done && likely(_fn != NULL))
              {
                _channel.addCompletionEvent (_state, _fn, _cookie, sequence);
              }

            TRACE_FORMAT("success = %d, _next = %zu, T_Num = %d, _nactive = %zu", (_next == T_Num), _next, T_Num, _nactive);
            TRACE_FN_EXIT();
            return done;
          }

          ///
          /// \brief Reset the internal state of the message
          ///
          /// \note Only used for message reuse.
          ///
          inline void reset () { _next = 0; };

          ///
          /// \brief Set the number of active descriptors
          ///
          /// \note T_Num is the max - but we can choose to use fewer
          ///
          inline void setNumberActiveDescriptors(size_t nactive) { PAMI_assert(nactive <= T_Num); _nactive = nactive; }

          MUSPI_DescriptorBase   desc[T_Num];

        protected:

          InjChannel          & _channel;
          size_t                _next;
          size_t                _nactive; // number of active descriptors <= T_Num 
          pami_event_function   _fn;
          void                * _cookie;
          uint8_t               _state[InjChannel::completion_event_state_bytes];

      }; // class     PAMI::Device::MU::InjectDescriptorMessage

#if 0
      ///
      /// \brief Single descriptor advance with completion template specialization
      ///
      template <>
      bool InjectDescriptorMessage<1, true>::advance ()
      {
        TRACE_FN_ENTER();

        size_t ndesc = _channel.getFreeDescriptorCountWithUpdate ();

        if (likely(ndesc > 0))
          {
            // There is at least one descriptor slot available in the injection
            // fifo before a fifo-wrap event.
            TRACE_FORMAT("inject descriptor (%p) from message (%p)", &desc[0], this);

            // Clone the message descriptor directly into the injection fifo.
            MUSPI_DescriptorBase * d =
              (MUSPI_DescriptorBase *) _channel.getNextDescriptor ();
            desc[0].clone (*d);

            // Finally, advance the injection fifo tail pointer. This action
            // completes the injection operation.
            _channel.injFifoAdvanceDesc ();

            if (likely(_fn != NULL))
              {
                _channel.setInjectionDescriptorNotification (_fn, _cookie, d);
              }

            TRACE_FN_EXIT();
            return true;
          }

        TRACE_FN_EXIT();
        return false;
      };

      ///
      /// \brief Single descriptor advance without completion template specialization
      ///
      template <>
      bool InjectDescriptorMessage<1, false>::advance ()
      {
        TRACE_FN_ENTER();

        size_t ndesc = _channel.getFreeDescriptorCountWithUpdate ();

        if (likely(ndesc > 0))
          {
            // There is at least one descriptor slot available in the injection
            // fifo before a fifo-wrap event.
            TRACE_FORMAT("inject descriptor (%p) from message (%p)", &desc[0], this);

            // Clone the message descriptor directly into the injection fifo.
            MUSPI_DescriptorBase * d =
              (MUSPI_DescriptorBase *) _channel.getNextDescriptor ();
            desc[0].clone (*d);

            // Finally, advance the injection fifo tail pointer. This action
            // completes the injection operation.
            _channel.injFifoAdvanceDesc ();

            TRACE_FN_EXIT();
            return true;
          }

        TRACE_FN_EXIT();
        return false;
      };
#endif
    };   // namespace PAMI::Device::MU
  };     // namespace PAMI::Device
};       // namespace PAMI

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#endif // __components_devices_bgq_mu2_msg_InjectDescriptorMessage_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
