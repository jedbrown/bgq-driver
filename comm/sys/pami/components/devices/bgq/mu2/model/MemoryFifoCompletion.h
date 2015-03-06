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
 * \file components/devices/bgq/mu2/model/MemoryFifoCompletion.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_model_MemoryFifoCompletion_h__
#define __components_devices_bgq_mu2_model_MemoryFifoCompletion_h__

#include "components/devices/bgq/mu2/msg/InjectDescriptorMessage.h"

#include "util/trace.h"
#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      class MemoryFifoCompletion
      {
        private :

          MU::Context & _context;

        public :

          /// \see PAMI::Device::Interface::PacketModel::PacketModel
          inline MemoryFifoCompletion (MU::Context & context) :
              _context (context)
          {
            TRACE_FN_ENTER();
            TRACE_FN_EXIT();
          };

          /// \see PAMI::Device::Interface::PacketModel::~PacketModel
          inline ~MemoryFifoCompletion () {};

          ///
          /// \brief Clone the "send to self" memfifo descriptor and set the notify information
          ///
          /// \param[in] desc   Descriptor to initialize
          /// \param[in] fn     Notification event function
          /// \param[in] cookie Notification event cookie
          ///
          inline void initializeNotifySelfDescriptor (MUSPI_DescriptorBase & desc,
                                                      pami_event_function    fn,
                                                      void                 * cookie)
          {
            TRACE_FN_ENTER();
            _context.receptionChannel.initializeNotifySelfDescriptor (desc, fn, cookie);
            TRACE_FN_EXIT();
          };


          ///
          /// \brief Inject the data mover descriptor(s) and the memory fifo
          ///        completion descriptor into the injection channel.
          ///
          /// \param[in] state   Memory which will, potentially, contain a message object
          /// \param[in] channel Injection channel to use inject into
          /// \param[in] fn      Completion function to invoke
          /// \param[in] cookie  Completion cookie
          /// \param[in] desc    Array of initialized "data mover" descriptors
          ///
          /// \todo Need a single MU SPI to advance the tail by *2* (or more)
          ///       descriptors
          ///
          template <unsigned T_State, unsigned T_Desc>
          inline void inject (uint8_t                (&state)[T_State],
                              InjChannel           & channel,
                              pami_event_function    fn,
                              void                 * cookie,
                              MUSPI_DescriptorBase   (&desc)[T_Desc])
          {
            TRACE_FN_ENTER();
            size_t ndesc = channel.getFreeDescriptorCount ();

            TRACE_FORMAT("ndesc = %zu, T_Desc = %d", ndesc, T_Desc);
            if (likely(ndesc > T_Desc))
              {
                // There is enough space in the injection fifo to add the
                // "ack to self" memory fifo descriptor after the data mover
                // descriptor(s)

                // Clone the completion model descriptor into the injection fifo
                MUSPI_DescriptorBase * d = (MUSPI_DescriptorBase *) desc;
                initializeNotifySelfDescriptor (d[T_Desc], fn, cookie);
                TRACE_HEXDATA(d,64*(T_Desc+1));

                // Advance the injection fifo tail pointer. This action
                // completes the injection operation.
                size_t i;

                for (i = 0; i <= T_Desc; i++)
                  channel.injFifoAdvanceDesc (); // see todo

                TRACE_FN_EXIT();
                return;
              }

            // There is not enough space after the data mover descriptor(s) to
            // inject the "ack to self" descriptor.
            //
            // Inject the data mover descriptor(s) and create an "ack to self"
            // message and post it to the channel.
            size_t i;

            for (i = 0; i < T_Desc; i++)
              channel.injFifoAdvanceDesc (); // see todo

            channel.post (createSimpleMessage (state, channel, fn, cookie));
            TRACE_FN_EXIT();
          }

          ///
          /// \brief Create a message containing one or more data mover descriptors
          ///        followed by an "ack to self" memory fifo descriptor.
          ///
          /// \param[in] state   Memory which will contain the message object
          /// \param[in] channel Injection channel to use during message advance
          /// \param[in] fn      Completion function to invoke
          /// \param[in] cookie  Completion cookie
          /// \param[in] desc    Array of initialized "data mover" descriptors
          ///
          /// \return Message to be posted to an injection channel
          ///
          template <unsigned T_State, unsigned T_Desc>
          inline MU::MessageQueue::Element * createDescriptorMessage (uint8_t                (&state)[T_State],
                                                                      InjChannel           & channel,
                                                                      pami_event_function    fn,
                                                                      void                 * cookie,
                                                                      MUSPI_DescriptorBase   (&desc)[T_Desc])
          {
            TRACE_FN_ENTER();
            COMPILE_TIME_ASSERT(sizeof(InjectDescriptorMessage < T_Desc + 1, false > ) <= T_State);

            InjectDescriptorMessage < T_Desc + 1, false > * msg =
              (InjectDescriptorMessage < T_Desc + 1, false > *) state;
            new (msg) InjectDescriptorMessage < T_Desc + 1, false > (channel);

            // Copy the "data mover" descriptor(s) into the message.
            size_t i;

            for (i = 0; i < T_Desc; i++)
              desc[i].clone (msg->desc[i]);

            // Copy the "completion" descriptor into the message and initialize
            // the completion function+cookie in the packet header.
            initializeNotifySelfDescriptor (msg->desc[T_Desc], fn, cookie);

            TRACE_FN_EXIT();
            return (MU::MessageQueue::Element *) msg;
          };

          ///
          /// \brief Create a simple message containing only an "ack to self"
          ///        memory fifo descriptor.
          ///
          /// \param[in] state   Memory which will contain the message object
          /// \param[in] channel Injection channel to use during message advance
          /// \param[in] fn      Completion function to invoke
          /// \param[in] cookie  Completion cookie
          ///
          /// \return Message to be posted to an injection channel
          ///
          template <unsigned T_State>
          inline MU::MessageQueue::Element * createSimpleMessage (uint8_t                (&state)[T_State],
                                                                  InjChannel           & channel,
                                                                  pami_event_function    fn,
                                                                  void                 * cookie)
          {
            TRACE_FN_ENTER();
            COMPILE_TIME_ASSERT(sizeof(InjectDescriptorMessage<1, false>) <= T_State);

            InjectDescriptorMessage<1, false> * msg =
              (InjectDescriptorMessage<1, false> *) state;
            new (msg) InjectDescriptorMessage<1, false> (channel);

            // Copy the "completion" descriptor into the message and initialize
            // the completion function+cookie in the packet header.
            initializeNotifySelfDescriptor (msg->desc[0], fn, cookie);

            TRACE_FN_EXIT();
            return (MU::MessageQueue::Element *) msg;
          };

      }; // PAMI::Device::MU::MemoryFifoCompletion class
    };   // PAMI::Device::MU namespace
  };     // PAMI::Device namespace
};       // PAMI namespace

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#endif // __components_devices_bgq_mu2_MemoryFifoCompletion_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
