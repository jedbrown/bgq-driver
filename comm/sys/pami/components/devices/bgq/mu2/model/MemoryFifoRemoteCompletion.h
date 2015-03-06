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
 * \file components/devices/bgq/mu2/model/MemoryFifoRemoteCompletion.h
 * \brief MU Memory Fifo Remote Completion Model
 *
 * This MU model injects a completion descriptor targeted for the
 * remote node's comm agent reception fifo.  The payload is a
 * memory fifo descriptor that the comm agent will inject, targeted
 * for our reception fifo, causing the completion callback to be
 * invoked on our node.
 *
 * The completion descriptor is deterministically routed and injected
 * into the same fifo as previous data descriptors.  Thus, when it
 * arrives at the remote node's comm agent, we know the previous data
 * has been received by the remote node, so when our node receives
 * the completion message and callback, we know the previous data
 * has been received by the remote node.  This is essentially a
 * fence.
 */
#ifndef __components_devices_bgq_mu2_model_MemoryFifoRemoteCompletion_h__
#define __components_devices_bgq_mu2_model_MemoryFifoRemoteCompletion_h__

#include "components/devices/bgq/mu2/msg/InjectDescriptorMessage.h"

#include <agents/include/comm/fence.h>

#include "util/common.h"

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
      class MemoryFifoRemoteCompletion
      {
        private :

          typedef struct
          {
            uint64_t pong[sizeof(MUSPI_DescriptorBase) >> 3];
          } inject_t;

          typedef struct
          {
            uint64_t msg[(sizeof(InjectDescriptorMessage<1,false>) >> 3) + 1];
            uint64_t pong[sizeof(MUSPI_DescriptorBase) >> 3];
          } simple_t;

          MU::Context & _context;

          MUSPI_Pt2PtMemoryFIFODescriptor _pingDesc; // The ping descriptor model.

        public :

          typedef union
          {
            inject_t inject;
            simple_t simple;
          } state_t;

          static const size_t state_bytes = sizeof(state_t);

          ///
          inline MemoryFifoRemoteCompletion (MU::Context & context) :
              _context (context),
              _pingDesc ()
          {
            TRACE_FN_ENTER();

            // Set up to construct the ping descriptor
            MUSPI_Pt2PtMemoryFIFODescriptorInfo_t info;

            info.Base.Pre_Fetch_Only  = MUHWI_DESCRIPTOR_PRE_FETCH_ONLY_NO;
            info.Base.Payload_Address = 0; // To be set at runtime.
            info.Base.Message_Length  = sizeof(MUHWI_Descriptor_t);
            info.Base.Torus_FIFO_Map  = 0; // To be set at runtime.
            info.Base.Dest.Destination.Destination = 0; // To be set at runtime.

            info.Pt2Pt.Hints_ABCD =
              MUHWI_PACKET_HINT_A_NONE |
              MUHWI_PACKET_HINT_B_NONE |
              MUHWI_PACKET_HINT_C_NONE |
              MUHWI_PACKET_HINT_D_NONE;
            info.Pt2Pt.Misc1 =
              MUHWI_PACKET_HINT_E_NONE |
              MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE |
              MUHWI_PACKET_USE_DETERMINISTIC_ROUTING |
              MUHWI_PACKET_DO_NOT_DEPOSIT;
            info.Pt2Pt.Misc2 =
              MUHWI_PACKET_VIRTUAL_CHANNEL_DETERMINISTIC;
            info.Pt2Pt.Skip = 136; // Skip entire packet for checksum.

            memset(info.MemFIFO.SoftwareBytes, 0x00, sizeof(info.MemFIFO.SoftwareBytes));
            info.MemFIFO.Rec_FIFO_Id      = _context.getGlobalCommAgentRecFifoId();
            info.MemFIFO.Rec_Put_Offset   = 0;
            info.MemFIFO.Interrupt        = MUHWI_DESCRIPTOR_INTERRUPT_ON_PACKET_ARRIVAL;
            info.MemFIFO.SoftwareBit      = 0;
            info.MemFIFO.SoftwareBytes[0] = CommAgent_Fence_GetRecFifoDispatchId();

            // Construct the ping descriptor.
            new ( &_pingDesc ) MUSPI_Pt2PtMemoryFIFODescriptor( &info );
            TRACE_FN_EXIT();
          };

          ///
          inline ~MemoryFifoRemoteCompletion () {};

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
          inline void setupPingPongDescs (MUSPI_Pt2PtMemoryFIFODescriptor *ping,
                                          MUSPI_DescriptorBase            *pong,
                                          pami_event_function              fn,
                                          void                            *cookie,
                                          uint64_t                         torusFIFOMap,
                                          uint32_t                         dest)
          {
            // Clone the ping model descriptor into the injection fifo.
            _pingDesc.clone( *ping );

            // In the ping descriptor:

            // 1. Set payload address in the ping descriptor to point into
            //    where the pong descriptor will be constructed.
            //    Determine the physical address of the payload buffer.
            Kernel_MemoryRegion_t memRegion;
            int32_t rc;
            rc = Kernel_CreateMemoryRegion ( &memRegion,
                                             pong,
                                             sizeof(MUHWI_Descriptor_t) );
            PAMI_assert ( rc == 0 );
            uint64_t paddr = (uint64_t)memRegion.BasePa +
                             ((uint64_t)pong - (uint64_t)memRegion.BaseVa);
            ping->Pa_Payload = paddr;

            // 2. Set the torus fifo map to use to get to the dest node.
            ping->setTorusInjectionFIFOMap ( torusFIFOMap );

            // 3. Set the destination coordinates.
            ping->setDestination ( dest );

            // Clone the "notify self" descriptor into the payload and update
            // it with the callback function and cookie to be given control
            // when the comm agent on the remote node sends this packet
            // back to our context.
            initializeNotifySelfDescriptor ( *pong,
                                             fn,
                                             cookie );

            // Use the same torus fifo map as we used to get to the dest.
            // We could reverse it, but that would require an "if" statement
            // that may cause an extra branch.
            (*pong).setTorusInjectionFIFOMap ( torusFIFOMap );

            TRACE_FORMAT("Ping,Pong descriptors at %p,%p:\n", ping, pong);
            TRACE_HEXDATA(ping, 64);
            TRACE_HEXDATA(pong, 64);
          }


          ///
          /// \brief Inject the memory fifo completion descriptor into the injection channel.
          ///
          /// \param[in] state   Memory which will, potentially, contain a message object
          /// \param[in] channel Injection channel to inject into
          /// \param[in] fn      Completion function to invoke
          /// \param[in] cookie  Completion cookie
          ///
          template <unsigned T_State>
          inline void inject (uint8_t                (&state)[T_State],
                              InjChannel           & channel,
                              pami_event_function    fn,
                              void                 * cookie,
                              uint64_t               torusFIFOMap,
                              uint32_t               dest)
          {
            TRACE_FN_ENTER();

            TRACE_FORMAT("Inject: state=%p, sizeof(state)=%zu, fn=%p, cookie=%p, map=0x%lx, dest=0x%08x\n", &state, sizeof(state), fn, cookie, torusFIFOMap, dest);

            /// \todo Remove this check when the comm agent is always started.
            PAMI_assertf(_context.isCommAgentActive(), "Aborting:  Cannot complete one-sided-put-fence operation because the Messaging App Agent (Comm Agent) is not running.\n");

            size_t ndesc = channel.getFreeDescriptorCountWithUpdate ();
            TRACE_FORMAT("MemoryFifoRemoteCompletion: inject(): ndesc = %zu", ndesc);

            if (likely(channel.isSendQueueEmpty() && ndesc > 0))
              {
                // There is nothing queued, and there is space in the
                // injection fifo for the ping descriptor.
                // Get a pointer to the next descriptor slot.
                // That is where the ping descriptor is to be constructed.
                MUHWI_Descriptor_t *desc = channel.getNextDescriptor ();

                MUSPI_Pt2PtMemoryFIFODescriptor *ping =
                  (MUSPI_Pt2PtMemoryFIFODescriptor*)desc;

                // The payload (the pong descriptor) goes in the "state" storage.
                COMPILE_TIME_ASSERT((sizeof(inject_t) <= T_State));
                inject_t * inject = (inject_t *) state;
                MUSPI_DescriptorBase *pong = (MUSPI_DescriptorBase *) inject->pong;

                // Construct the ping and pong descriptors at the specified locations.
                setupPingPongDescs( ping, pong, fn, cookie, torusFIFOMap, dest );

                // Advance the injection fifo tail pointer. This action
                // completes the injection operation.
                channel.injFifoAdvanceDesc ();

                TRACE_FN_EXIT();

                return;
              }

            // There is not enough space in the injection fifo for the ping
            // descriptor.  Create a message and post it to the injection
            // channel so it gets injected on a future advance().

            channel.post ( createSimpleMessage (state,
                                                channel,
                                                fn,
                                                cookie,
                                                torusFIFOMap,
                                                dest));
            TRACE_FN_EXIT();
          }


          ///
          /// \brief Create a simple message containing the ping descriptor and
          ///        its payload (the pong descriptor).
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
                                                                  void                 * cookie,
                                                                  uint64_t               torusFIFOMap,
                                                                  uint32_t               dest)
          {
            TRACE_FN_ENTER();
            COMPILE_TIME_ASSERT((sizeof(simple_t) <= T_State));

            simple_t * simple = (simple_t *) state;

            InjectDescriptorMessage<1, false> * msg =
              (InjectDescriptorMessage<1, false> *) simple->msg;
            new (msg) InjectDescriptorMessage<1, false> (channel);

            // The ping descriptor is to be constructed within the
            // InjectDescriptorMessage.
            MUSPI_Pt2PtMemoryFIFODescriptor *ping =
              (MUSPI_Pt2PtMemoryFIFODescriptor*)(&msg->desc[0]);

            // The payload (the pong descriptor) goes after the
            // InjectDescriptorMessage.
            MUSPI_DescriptorBase *pong = (MUSPI_DescriptorBase *) simple->pong;

            // Construct the ping and pong descriptors at the specified locations.
            setupPingPongDescs( ping, pong, fn, cookie, torusFIFOMap, dest );

            TRACE_FN_EXIT();
            return (MU::MessageQueue::Element *) msg;
          };

      }; // PAMI::Device::MU::MemoryFifoRemoteCompletion class
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
