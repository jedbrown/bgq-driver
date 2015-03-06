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
 * \file components/devices/bgq/mu2/model/DmaModelBase.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_model_DmaModelBase_h__
#define __components_devices_bgq_mu2_model_DmaModelBase_h__

#include <spi/include/mu/DescriptorBaseXX.h>

#include "Global.h"
#include "components/devices/DmaInterface.h"
#include "components/devices/bgq/mu2/Context.h"
#include "components/devices/bgq/mu2/model/MemoryFifoRemoteCompletion.h"
#include "components/devices/bgq/mu2/CounterPool.h"

#include "util/trace.h"
#include "components/memory/MemoryAllocator.h"

#include <stdlib.h>

#define SPLIT_E_CUTOFF 2048 // 4 packets

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
      template <class T_Model, unsigned T_PayloadSize>
      class DmaModelBase : public Interface::DmaModel < MU::DmaModelBase<T_Model,T_PayloadSize> >
      {
        private :

          // This structure is used to track the status when we split an rget
          // into two and send it on opposite E links. Also, it is used to
          // track multiple completions in the fence method.
          typedef struct
          {
            pami_event_function     fn;
            void                  * cookie;
            uint32_t                active;
            void                  * toBeFreed; // Pointer to storage to be freed
                                               // when "active" hits zero.
            PAMI::Memory::MemoryManager *mm;   // Pointer to memory manager for the free.
          } multi_complete_t;

          typedef struct
          {
            uint64_t msg[(sizeof(InjectDescriptorMessage<1>) >> 3) + 1];
          } put_state_t;

          typedef struct
          {
            union
            {
              uint64_t msg1[(sizeof(InjectDescriptorMessage<1, false>) >> 3) + 1];
              uint64_t msg2[(sizeof(InjectDescriptorMessage<2, false>) >> 3) + 1];
            };
            uint8_t e_minus_payload[T_PayloadSize];
            uint8_t e_plus_payload[T_PayloadSize];
            multi_complete_t split_e_state;
          } get_state_t;

          typedef struct
          {
            uint64_t msg[(sizeof(InjectDescriptorMessage<2, false>) >> 3) + 1];
            uint8_t e_minus_payload[T_PayloadSize];
            uint8_t e_plus_payload[T_PayloadSize];
          } split_t;

          typedef struct
          {
            uint64_t msg[(sizeof(InjectDescriptorMessage<1, false>) >> 3) + 1];
            uint64_t payload[(sizeof(MUSPI_DescriptorBase) >> 3) + 1];
          } single_t;

          typedef struct
          {
            multi_complete_t multi;
            uint8_t memfifo_remote_completion_state[sizeof(MemoryFifoRemoteCompletion::state_t)];
            union
            {
              split_t  split;
              single_t single;
            };
            // Array of fence structures, one per counter pool.  Each structure may be
            // added to its corresponding CounterPool to 1) snapshot the current counter
            // state, 2) monitor that state until the counters have completed, and
            // 3) invoke the multiComplete fn when each CounterPool has finished 
            // its monitoring.
            CounterPool::fenceInfo_t *counterPoolInfo;
          } fence_state_t;

          typedef union
          {
            MemoryFifoRemoteCompletion::state_t memfifo_remote_completion_state;
            uint8_t injchannel_completion_state[InjChannel::completion_event_state_bytes];
            put_state_t put_state;
            get_state_t get_state;
            fence_state_t fence_state;
          } state_t;

        public:

          typedef MU::Context Device;

        protected :

          /////////////////////////////////////////////////////////////////////
          //
          // PAMI::Interface::DmaModel interface implementation -- BEGIN
          //
          /////////////////////////////////////////////////////////////////////

          friend class Interface::DmaModel < MU::DmaModelBase<T_Model,T_PayloadSize> >;

          /// \see PAMI::Device::Interface::DmaModel::DmaModel
          /// \todo set base address table information int he direct put descriptor(s)
          DmaModelBase (MU::Context & device, pami_result_t & status);

          /// \see PAMI::Device::Interface::DmaModel::DmaModel
          ~DmaModelBase ();

        public:

          /// \see Device::Interface::DmaModel::getVirtualAddressSupported
          static const bool dma_model_va_supported = false;

          /// \see Device::Interface::DmaModel::getMemoryRegionSupported
          static const bool dma_model_mr_supported = true;

          /// \see Device::Interface::DmaModel::getDmaTransferStateBytes
          static const size_t dma_model_state_bytes  = sizeof(state_t);

          static const bool dma_model_fence_supported = false;

       protected:

          inline bool postDmaPut_impl (size_t                target_task,
                                       size_t                target_offset,
                                       size_t                bytes,
                                       void                * local,
                                       void                * remote);

          template <unsigned T_StateBytes>
          inline bool postDmaPut_impl (uint8_t               (&state)[T_StateBytes],
                                       pami_event_function   local_fn,
                                       void                * cookie,
                                       size_t                target_task,
                                       size_t                target_offset,
                                       size_t                bytes,
                                       void                * local,
                                       void                * remote);

          inline bool postDmaPut_impl (size_t                target_task,
                                       size_t                target_offset,
                                       size_t                bytes,
                                       Memregion           * local_memregion,
                                       size_t                local_offset,
                                       Memregion           * remote_memregion,
                                       size_t                remote_offset);

          template <unsigned T_StateBytes>
          inline bool postDmaPut_impl (uint8_t               (&state)[T_StateBytes],
                                       pami_event_function   local_fn,
                                       void                * cookie,
                                       size_t                target_task,
                                       size_t                target_offset,
                                       size_t                bytes,
                                       Memregion           * local_memregion,
                                       size_t                local_offset,
                                       Memregion           * remote_memregion,
                                       size_t                remote_offset);

          inline bool postDmaGet_impl (size_t                target_task,
                                       size_t                target_offset,
                                       size_t                bytes,
                                       void                * local,
                                       void                * remote);

          template <unsigned T_StateBytes>
          inline bool postDmaGet_impl (uint8_t               (&state)[T_StateBytes],
                                       pami_event_function   local_fn,
                                       void                * cookie,
                                       size_t                target_task,
                                       size_t                target_offset,
                                       size_t                bytes,
                                       void                * local,
                                       void                * remote);

          inline bool postDmaGet_impl (size_t                target_task,
                                       size_t                target_offset,
                                       size_t                bytes,
                                       Memregion           * local_memregion,
                                       size_t                local_offset,
                                       Memregion           * remote_memregion,
                                       size_t                remote_offset);

          template <unsigned T_StateBytes>
          inline bool postDmaGet_impl (uint8_t               (&state)[T_StateBytes],
                                       pami_event_function   local_fn,
                                       void                * cookie,
                                       size_t                target_task,
                                       size_t                target_offset,
                                       size_t                bytes,
                                       Memregion           * local_memregion,
                                       size_t                local_offset,
                                       Memregion           * remote_memregion,
                                       size_t                remote_offset);

          template <unsigned T_StateBytes>
          inline bool postDmaFence_impl (uint8_t              (&state)[T_StateBytes],
                                         pami_event_function   local_fn,
                                         void                * cookie,
                                         size_t                target_task,
                                         size_t                target_offset);

          /////////////////////////////////////////////////////////////////////
          //
          // PAMI::Interface::DmaModel interface implementation -- END
          //
          /////////////////////////////////////////////////////////////////////

          static const size_t payload_size = sizeof(MUSPI_DescriptorBase);
          static const bool fence_supported = false;

          inline size_t initializeRemoteGetPayload (void                * vaddr,
                                                    uint64_t              local_dst_pa,
                                                    uint64_t              remote_src_pa,
                                                    size_t                bytes,
                                                    size_t                from_task,
                                                    size_t                from_offset,
                                                    pami_event_function   local_fn,
                                                    void                * cookie);

          MUSPI_DescriptorBase   _dput; // "direct put" used for postDmaPut_impl()
          MUSPI_DescriptorBase   _rget; // "remote get" used for postDmaGet_impl()
          MUSPI_DescriptorBase   _rput; // "remote put" _rget payload descriptor
          MemoryFifoRemoteCompletion _remoteCompletion; // Class that does a
          // memory fifo completion
          // ping pong.

          MU::Context          & _context;
          MUHWI_Destination_t  * _myCoords;
          size_t                 _rgetPacingSize;

          ///
          /// \brief Local completion event callback for multiple sub-events
          ///
          /// This callback is invoked when completing a split rget.
          /// It will be called twice, once for each half of the rget.
          /// The first time, the counter will be two, so it just decrements
          /// the counter and returns.  The second time, the counter will be
          /// 1, so it invokes the application local completion
          /// callback function.
          ///
          /// Similarly, this callback is used for the fence operation.
          ///
          static void multiComplete (pami_context_t   context,
                                     void           * cookie,
                                     pami_result_t    result)
          {
            multi_complete_t * state = (multi_complete_t*) cookie;

            state->active--;

            if ( state->active == 0 )
              {
                // If there is storage to be freed, do it now.
                if ( state->toBeFreed )
                {
                  state->mm->free ( state->toBeFreed );
                }

                if ( state->fn != NULL)
                  {
                    state->fn (context, state->cookie, PAMI_SUCCESS);
                  }
              }

            return;
          }

          /// \brief Determine if Dest is a Nearest Neighbor in E
          ///
          /// Returns true if the specified dest is only linked with our
          /// node via E links.  No ABCD links.
          ///
          /// \param[in]  dest  ABCDE coords of the destination node
          ///
          /// \retval  true  Dest is nearest neighbor in E
          /// \retval  false Dest is NOT nearest neighbor in E
          ///
          inline bool nearestNeighborInE ( MUHWI_Destination_t dest )
          {
            if ( ( ( dest.Destination.Destination & 0xffffffc0 ) ==
                   ( _myCoords->Destination.Destination & 0xffffffc0 ) ) && // Same ABCD coords?
                 dest.Destination.Destination !=
                 _myCoords->Destination.Destination )      // And different E coords?
              return true;
            else
              return false;
          }

          /// \brief Determine if Dest is on a Network Line
          ///
          /// Our node's ABCDE coords are compared with the dest node's ABCDE coords
          /// to determine if the two nodes are on a line.  That is, they only differ
          /// in one dimension.
          ///
          /// \retval  0  Not on a line.
          /// \retval  1  On a line.
          ///
          inline uint32_t getOnALine ( MUHWI_Destination_t dest )
          {
            uint32_t destNodeCoords = dest.Destination.Destination;

            static const uint32_t a_coord_mask = 0x3f000000; // mask off the non-a-coord bits
            static const uint32_t b_coord_mask = 0x00fc0000; // mask off the non-b-coord bits
            static const uint32_t c_coord_mask = 0x0003f000; // mask off the non-c-coord bits
            static const uint32_t d_coord_mask = 0x00000fc0; // mask off the non-d-coord bits
            static const uint32_t e_coord_mask = 0x0000003f; // mask off the non-e-coord bits

            // tmp will have 0's in a coordinate where source and dest match.
            uint32_t tmp = _myCoords->Destination.Destination ^ destNodeCoords;

            uint32_t numDifferentDimensions =
              ( ( tmp & a_coord_mask ) != 0 ) +
              ( ( tmp & b_coord_mask ) != 0 ) +
              ( ( tmp & c_coord_mask ) != 0 ) +
              ( ( tmp & d_coord_mask ) != 0 ) +
              ( ( tmp & e_coord_mask ) != 0 );

            return numDifferentDimensions == 1;
          }

      };

      template <class T_Model, unsigned T_PayloadSize>
      DmaModelBase<T_Model,T_PayloadSize>::DmaModelBase (MU::Context & device, pami_result_t & status) :
          Interface::DmaModel < MU::DmaModelBase<T_Model,T_PayloadSize> > (device, status),
          _remoteCompletion (device),
          _context (device)
      {
        COMPILE_TIME_ASSERT(sizeof(MUSPI_DescriptorBase) <= MU::Context::immediate_payload_size);

        TRACE_FORMAT("sizeof(state_t) == %ld, sizeof(get_state_t) = %ld, sizeof(put_state_t) = %ld, T_Model::payload_size = %zu", sizeof(state_t), sizeof(get_state_t), sizeof(put_state_t), T_Model::payload_size);

        _myCoords = __global.mapping.getMuDestinationSelf();

        _rgetPacingSize = __global.getRgetPacingSize();

        // Zero-out the descriptor models before initialization
        memset((void *)&_dput, 0, sizeof(_dput));
        memset((void *)&_rget, 0, sizeof(_rget));
        memset((void *)&_rput, 0, sizeof(_rput));

        // --------------------------------------------------------------------
        // Set the common base descriptor fields
        //
        // For the remote get packet, send it using the high priority torus
        // fifo map.  Everything else uses non-priority torus fifos, pinned
        // later based on destination.  This is necessary to avoid deadlock
        // when the remote get fifo fills.  Note that this is in conjunction
        // with using the high priority virtual channel (set elsewhere).
        // --------------------------------------------------------------------
        MUSPI_BaseDescriptorInfoFields_t base;
        memset((void *)&base, 0, sizeof(base));

        base.Pre_Fetch_Only  = MUHWI_DESCRIPTOR_PRE_FETCH_ONLY_NO;

        _dput.setBaseFields (&base);

        _rput.setBaseFields (&base);

        base.Torus_FIFO_Map = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_PRIORITY;
	base.Message_Length = sizeof(MUHWI_Descriptor_t);
        _rget.setBaseFields (&base);


        // --------------------------------------------------------------------
        // Set the common point-to-point descriptor fields
        //
        // For the remote get packet, send it on the high priority virtual
        // channel.  Everything else is on the deterministic virtual channel.
        // This is necessary to avoid deadlock when the remote get fifo fills.
        // Note that this is in conjunction with setting the high priority
        // torus fifo map (set elsewhere).
        // --------------------------------------------------------------------
        MUSPI_Pt2PtDescriptorInfoFields_t pt2pt;
        memset((void *)&pt2pt, 0, sizeof(pt2pt));

        pt2pt.Misc1 =
          MUHWI_PACKET_USE_DETERMINISTIC_ROUTING |
          MUHWI_PACKET_DO_NOT_DEPOSIT |
          MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE;
        pt2pt.Misc2 =
          MUHWI_PACKET_VIRTUAL_CHANNEL_DETERMINISTIC;
	pt2pt.Skip       = 8; // Don't link checksum dput and rput packet header

        _dput.setDataPacketType (MUHWI_PT2PT_DATA_PACKET_TYPE);
        _dput.PacketHeader.NetworkHeader.pt2pt.Byte8.Size = 16;
        _dput.setPt2PtFields (&pt2pt);
        _rput.setDataPacketType (MUHWI_PT2PT_DATA_PACKET_TYPE);
        _rput.PacketHeader.NetworkHeader.pt2pt.Byte8.Size = 16;
        _rput.setPt2PtFields (&pt2pt);
        _rget.setDataPacketType (MUHWI_PT2PT_DATA_PACKET_TYPE);
        _rget.PacketHeader.NetworkHeader.pt2pt.Byte8.Size = 16;
        pt2pt.Misc2 =
          MUHWI_PACKET_VIRTUAL_CHANNEL_HIGH_PRIORITY;
	pt2pt.Skip       = 136; // Don't link checksum entire rget packet
        _rget.setPt2PtFields (&pt2pt);


        // --------------------------------------------------------------------
        // Set the remote get descriptor fields
        // --------------------------------------------------------------------
        MUSPI_RemoteGetDescriptorInfoFields_t rget;
        memset((void *)&rget, 0, sizeof(rget));

        rget.Type = MUHWI_PACKET_TYPE_GET;
        _rget.setRemoteGetFields (&rget);


        // --------------------------------------------------------------------
        // Set the direct put descriptor fields
        // --------------------------------------------------------------------
        MUSPI_DirectPutDescriptorInfoFields dput;
        memset((void *)&dput, 0, sizeof(dput));

        dput.Rec_Payload_Base_Address_Id = _context.getGlobalBatId();
        dput.Rec_Payload_Offset          = 0;
        dput.Rec_Counter_Base_Address_Id = _context.getSharedCounterBatId();
        dput.Rec_Counter_Offset          = 0;
        dput.Pacing                      = MUHWI_PACKET_DIRECT_PUT_IS_NOT_PACED;

        _dput.setDirectPutFields (&dput);

        _dput.setMessageUnitPacketType (MUHWI_PACKET_TYPE_PUT);


        // --------------------------------------------------------------------
        // Set the remote put descriptor fields
        // --------------------------------------------------------------------
        _rput.setDirectPutFields (&dput);

        _rput.setDestination (*(_context.getMuDestinationSelf()));
        _rput.setMessageUnitPacketType (MUHWI_PACKET_TYPE_PUT);
      };

      template <class T_Model, unsigned T_PayloadSize>
      DmaModelBase<T_Model,T_PayloadSize>::~DmaModelBase ()
      {
      };

      template <class T_Model, unsigned T_PayloadSize>
      inline size_t DmaModelBase<T_Model,T_PayloadSize>::initializeRemoteGetPayload (
        void                * vaddr,
        uint64_t              local_dst_pa,
        uint64_t              remote_src_pa,
        size_t                bytes,
        size_t                from_task,
        size_t                from_offset,
        pami_event_function   local_fn,
        void                * cookie)
      {
        // Retreive the route information back to mu context "self"
        uint64_t map;

        _context.pinInformation (from_task,
                                 from_offset,
                                 map);

        // Clone the remote direct put model descriptor into the payload
        MUSPI_DescriptorBase * rput = (MUSPI_DescriptorBase *) vaddr;
        _rput.clone (*rput);

        //MUSPI_DescriptorBase * rput = (MUSPI_DescriptorBase *) & clone->desc[0];

        // Set the payload of the direct put descriptor to be the physical
        // address of the source buffer on the remote node (from the user's
        // memory region).
        rput->setPayload (remote_src_pa, bytes);

        // Set the destination buffer address for the remote direct put.
        rput->setRecPayloadBaseAddressInfo (0, local_dst_pa);

        rput->setTorusInjectionFIFOMap (map);

	//fifo map should be non zero
	PAMI_assert(map != 0);

// !!!!
// Allocate completion counter, set counter in rput descriptor, set completion function and cookie
// !!!!

        //MUSPI_DescriptorDumpHex((char *)"Remote Put", rput);

        return sizeof(MUHWI_Descriptor_t);
      };



      template <class T_Model, unsigned T_PayloadSize>
      bool DmaModelBase<T_Model,T_PayloadSize>::postDmaPut_impl (size_t                target_task,
                                                   size_t                target_offset,
                                                   size_t                bytes,
                                                   void                * local,
                                                   void                * remote)
      {
        PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
        return false;
      }

      template <class T_Model, unsigned T_PayloadSize>
      template <unsigned T_StateBytes>
      bool DmaModelBase<T_Model,T_PayloadSize>::postDmaPut_impl (uint8_t               (&state)[T_StateBytes],
                                                   pami_event_function   local_fn,
                                                   void                * cookie,
                                                   size_t                target_task,
                                                   size_t                target_offset,
                                                   size_t                bytes,
                                                   void                * local,
                                                   void                * remote)
      {
        PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
        return false;
      }

      template <class T_Model, unsigned T_PayloadSize>
      bool DmaModelBase<T_Model,T_PayloadSize>::postDmaPut_impl (size_t                target_task,
                                                   size_t                target_offset,
                                                   size_t                bytes,
                                                   Memregion           * local_memregion,
                                                   size_t                local_offset,
                                                   Memregion           * remote_memregion,
                                                   size_t                remote_offset)
      {
        // The MU device *could* immediately complete the put operation
        // *if* the source data is copied into a payload lookaside buffer
        // as is done for immediate packet operations.
        //
        // For now return 'false' until this optimization is implemented.
        return false;
      }

      template <class T_Model, unsigned T_PayloadSize>
      template <unsigned T_StateBytes>
      bool DmaModelBase<T_Model,T_PayloadSize>::postDmaPut_impl (uint8_t               (&state)[T_StateBytes],
                                                   pami_event_function   local_fn,
                                                   void                * cookie,
                                                   size_t                target_task,
                                                   size_t                target_offset,
                                                   size_t                bytes,
                                                   Memregion           * local_memregion,
                                                   size_t                local_offset,
                                                   Memregion           * remote_memregion,
                                                   size_t                remote_offset)
      {
        TRACE_FN_ENTER();

        COMPILE_TIME_ASSERT(sizeof(put_state_t) <= T_StateBytes);
        put_state_t * put_state = (put_state_t *) state;

        if (unlikely(bytes == 0)) // eliminate this branch with a template parameter?
          {
#if 0
            // A zero-byte put is defined to be complete after a dma pingpong. This
            // is accomplished via a zero-byte get operation.
            return postDmaGet_impl (state, local_fn, cookie, target_task, target_offset, 0,
                                    local_memregion, local_offset,
                                    remote_memregion, remote_offset);
            // Note: This technique is turned-off because it won't work with the MU.
            // The postDmaGet() implementation is to send a remote-get to the remote
            // node.  When that DmaGet completes, we know the previous put operations
            // have been received at the remote node.  This remote get must be
            // deterministically routed on the same virtual channel as the previous
            // put operations so it flows after the put operations.  But, to avoid
            // deadlocks, the MU requires the remote get to flow on the high-priority
            // virtual channel, which is different from the virtual channel that the
            // data flows on.  Thus, we can't use this remote get tecnhique.
#else
            // A zero-byte put is defined to be complete after a memory fifo pingpong
            // with the remote node.  The ping must be injected into the same injection
            // fifo and torus injection fifo, and must be deterministically routed so it
            // follows behind the previous deterministically-routed put operations.
            // When the ping is received at the remote node, it sends a pong back to our
            // node, causing the local_fn callback to be invoked, indicating completion.

            // Before injecting the ping descriptor, determine the destination,
            // torus fifo map, and injection channel to use to get to the
            // destination.
            MUHWI_Destination_t   dest;
            uint16_t              rfifo;
            uint64_t              map;

            size_t fnum = _context.pinFifo (target_task,
                                            target_offset,
                                            dest,
                                            rfifo,
                                            map);

            InjChannel & channel = _context.injectionGroup.channel[fnum];

            _remoteCompletion.inject( state,
                                      channel,
                                      local_fn,
                                      cookie,
                                      map,
                                      dest.Destination.Destination);

            TRACE_FN_EXIT();
            return true;
#endif
          }

        MUHWI_Destination_t   dest;
        uint16_t              rfifo; // not needed for direct put ?
        uint64_t              map;

        size_t fnum = _context.pinFifo (target_task,
                                        target_offset,
                                        dest,
                                        rfifo,
                                        map);

        InjChannel & channel = _context.injectionGroup.channel[fnum];
        size_t ndesc = channel.getFreeDescriptorCountWithUpdate ();

        if (likely(channel.isSendQueueEmpty() && ndesc > 0))
          {
            // There is at least one descriptor slot available in the injection
            // fifo before a fifo-wrap event.

            MUHWI_Descriptor_t * desc = channel.getNextDescriptor ();

            // Clone the direct-put model descriptor into the injection fifo
            MUSPI_DescriptorBase * dput = (MUSPI_DescriptorBase *) desc;
            _dput.clone (*dput);

            // Initialize the injection fifo descriptor in-place.
            uint64_t local_pa  = (uint64_t) local_memregion->getBasePhysicalAddress ();
            uint64_t remote_pa = (uint64_t) remote_memregion->getBasePhysicalAddress ();

            dput->setDestination (dest);
            dput->setTorusInjectionFIFOMap (map);
            dput->setPayload (local_pa + local_offset, bytes);

            // The global BAT id is constant .. should only need to set
            // the "offset" (second parameter) here.
            dput->setRecPayloadBaseAddressInfo (_context.getGlobalBatId(), remote_pa + remote_offset);

            TRACE_HEXDATA(dput, sizeof(MUHWI_Descriptor_t));

            // Advance the injection fifo tail pointer. This action
            // completes the injection operation.
            uint64_t sequence = channel.injFifoAdvanceDesc ();

            // Set up completion notification if required
            if (likely(local_fn != NULL))
              {
                // The descriptor is not done (or was not checked). Save state
                // information so that the progress of the decriptor can be checked
                // later and the callback will be invoked when the descriptor is
                // complete.

                // Add a completion event for the sequence number associated with
                // the descriptor that was injected.
                channel.addCompletionEvent (state, local_fn, cookie, sequence);
              }
          }
        else
          {
            // Create a simple single-descriptor message
            InjectDescriptorMessage<1> * msg =
              (InjectDescriptorMessage<1> *) put_state->msg;
            new (msg) InjectDescriptorMessage<1> (channel, local_fn, cookie);

            // Clone the single-packet descriptor model into the message
            _dput.clone (msg->desc[0]);

            // Initialize the injection fifo descriptor in-place.
            uint64_t local_pa  = (uint64_t) local_memregion->getBasePhysicalAddress ();
            uint64_t remote_pa = (uint64_t) remote_memregion->getBasePhysicalAddress ();

            msg->desc[0].setDestination (dest);
            msg->desc[0].setTorusInjectionFIFOMap (map);
            msg->desc[0].setPayload (local_pa + local_offset, bytes);

            // The global BAT id is constant .. should only need to set
            // the "offset" (second parameter) here.
            msg->desc[0].setRecPayloadBaseAddressInfo (_context.getGlobalBatId(), remote_pa + remote_offset);

            TRACE_HEXDATA(&msg->desc[0], sizeof(MUHWI_Descriptor_t));

            // Post the message to the injection channel
            channel.post (msg);
          }

        TRACE_FN_EXIT();
        return true;
      };

      template <class T_Model, unsigned T_PayloadSize>
      bool DmaModelBase<T_Model,T_PayloadSize>::postDmaGet_impl (size_t                target_task,
                                                   size_t                target_offset,
                                                   size_t                bytes,
                                                   void                * local,
                                                   void                * remote)
      {
        PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
        return false;
      }

      template <class T_Model, unsigned T_PayloadSize>
      template <unsigned T_StateBytes>
      bool DmaModelBase<T_Model,T_PayloadSize>::postDmaGet_impl (uint8_t               (&state)[T_StateBytes],
                                                   pami_event_function   local_fn,
                                                   void                * cookie,
                                                   size_t                target_task,
                                                   size_t                target_offset,
                                                   size_t                bytes,
                                                   void                * local,
                                                   void                * remote)
      {
        PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
        return false;
      }

      template <class T_Model, unsigned T_PayloadSize>
      bool DmaModelBase<T_Model,T_PayloadSize>::postDmaGet_impl (size_t                target_task,
                                                   size_t                target_offset,
                                                   size_t                bytes,
                                                   Memregion           * local_memregion,
                                                   size_t                local_offset,
                                                   Memregion           * remote_memregion,
                                                   size_t                remote_offset)
      {
        // MU remote get can never be completed "immediately".
        return false;
      }

      template <class T_Model, unsigned T_PayloadSize>
      template <unsigned T_StateBytes>
      bool DmaModelBase<T_Model,T_PayloadSize>::postDmaGet_impl (uint8_t               (&state)[T_StateBytes],
                                                   pami_event_function   local_fn,
                                                   void                * cookie,
                                                   size_t                target_task,
                                                   size_t                target_offset,
                                                   size_t                bytes,
                                                   Memregion           * local_memregion,
                                                   size_t                local_offset,
                                                   Memregion           * remote_memregion,
                                                   size_t                remote_offset)
      {
        TRACE_FN_ENTER();
        /* Local transfers are implemented as a 'reverse direct put'. */
        if ( unlikely ( __global.mapping.isLocal ( target_task ) ) )
	  {
	    return postDmaPut_impl (state, local_fn, cookie,
				    target_task, target_offset, bytes,
				    remote_memregion, remote_offset,
				    local_memregion, local_offset);
	  }

        COMPILE_TIME_ASSERT(sizeof(get_state_t) <= T_StateBytes);
        get_state_t * get_state = (get_state_t *) state;

        uint64_t remote_src_pa = (uint64_t) remote_memregion->getBasePhysicalAddress ();
        remote_src_pa += remote_offset;

        uint64_t local_dst_pa = (uint64_t) local_memregion->getBasePhysicalAddress ();
        local_dst_pa += local_offset;

        MUHWI_Destination_t   dest;
        uint16_t              rfifo; // not needed for direct put ?
        uint64_t              map;
	uint32_t              paceRgetsToThisDest;
        uint32_t              routingIndex;

        size_t fnum = _context.pinFifo (target_task,
                                        target_offset,
                                        dest,
                                        rfifo,
                                        map,
					paceRgetsToThisDest,
                                        routingIndex);

        InjChannel & channel = _context.injectionGroup.channel[fnum];

        // When the dest is on a line in the E dimension, split the message into
        // two rgets, and flow one DPut in the E+ and the other in the E- to
        // optimize bandwidth.  This is possible because dest and our node are
        // linked via the two E links.
        // Note: For E-dimension transfers, there is no need for dynamic routing,
        // so the DPut is deterministically routed and the original memory fifo 
        // completion is used.
        //
        // This will be disabled because it causes issues with non-contig data
        //if ( unlikely ( nearestNeighborInE ( dest ) &&	( bytes >= SPLIT_E_CUTOFF ) ) )
        if(0)
          { // Special E dimension case
            // Get pointers to 2 descriptor slots, if they are available.
            MUHWI_Descriptor_t * desc[2];
            uint64_t descNum = channel.getNextDescriptorMultiple ( 2, desc );

            // If the send queue is empty and there is space in the fifo for
            // both descriptors, continue.
            if (likely( channel.isSendQueueEmpty() && (descNum != (uint64_t) - 1 )))
              {
                // Clone the remote inject model descriptors into the injection fifo slots.
                MUSPI_DescriptorBase * rgetMinus = (MUSPI_DescriptorBase *) desc[0];
                MUSPI_DescriptorBase * rgetPlus  = (MUSPI_DescriptorBase *) desc[1];
                _rget.clone (*rgetMinus);
                _rget.clone (*rgetPlus);

                // Initialize the destination in the rget descriptors.
                rgetMinus->setDestination (dest);
                rgetPlus ->setDestination (dest);

                // Set the remote injection fifo identifiers to E- and E+ respectively.
                uint32_t *rgetInjFifoIds = _context.getRgetInjFifoIds ();
                rgetMinus->setRemoteGetInjFIFOId ( rgetInjFifoIds[8] );
                rgetPlus ->setRemoteGetInjFIFOId ( rgetInjFifoIds[9] );

                // The "split e state" contains
                // - A completion counter that will count how many completion messages
                //   have been received (we are done when two have been received),
                // - The original callback function and cookie.
                multi_complete_t * multiCookie = & get_state->split_e_state;
                multiCookie->fn                = local_fn;
                multiCookie->cookie            = cookie;
                multiCookie->active            = 2;
                multiCookie->toBeFreed         = NULL; // Nothing to free.

                // Initialize the rget payload descriptor(s) for the E- rget
                void * vaddr;
                uint64_t paddr;
                size_t bytes0 = bytes >> 1;
                size_t bytes1 = bytes - bytes0;

                channel.getDescriptorPayload (desc[0], vaddr, paddr);
                size_t pbytes = static_cast<T_Model*>(this)->
                                initializeRemoteGetPayload (vaddr,
                                                            local_dst_pa,
                                                            remote_src_pa,
                                                            bytes0,
                                                            MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM,
                                                            MUHWI_PACKET_HINT_EM,
                                                            multiComplete,
                                                            multiCookie);
                rgetMinus->setPayload (paddr, pbytes);

                /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Rget0",desc[0]); */
                /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Dput0",(MUHWI_Descriptor_t*)vaddr); */
                /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Memfifo-Completion0",((MUHWI_Descriptor_t*)vaddr)+1); */

                // Initialize the rget payload descriptor(s) for the E+ rget
                channel.getDescriptorPayload (desc[1], vaddr, paddr);
                pbytes = static_cast<T_Model*>(this)->
                         initializeRemoteGetPayload (vaddr,
                                                     local_dst_pa + bytes0,
                                                     remote_src_pa + bytes0,
                                                     bytes1,
                                                     MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP,
                                                     MUHWI_PACKET_HINT_EP,
                                                     multiComplete,
                                                     multiCookie);
                rgetPlus->setPayload (paddr, pbytes);

                /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Rget1",desc[1]); */
                /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Dput1",(MUHWI_Descriptor_t*)vaddr); */
                /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Memfifo-Completion1",((MUHWI_Descriptor_t*)vaddr)+1); */

                // Finally, advance the injection fifo tail pointer. This action
                // completes the injection operation.
                channel.injFifoAdvanceDescMultiple(2);

                // The completion callback was not invoked; return false.
                TRACE_FN_EXIT();
                return false;
              }

            // Construct and post a message
            // Create a simple dual-descriptor message
            InjectDescriptorMessage<2, false> * msg =
              (InjectDescriptorMessage<2, false> *) get_state->msg2;
            new (msg) InjectDescriptorMessage<2, false> (channel);

            // Clone the remote inject model descriptors into the injection fifo slots.
            MUSPI_DescriptorBase * rgetMinus = (MUSPI_DescriptorBase *) & msg->desc[0];
            MUSPI_DescriptorBase * rgetPlus  = (MUSPI_DescriptorBase *) & msg->desc[1];
            _rget.clone (*rgetMinus);
            _rget.clone (*rgetPlus);

            // Initialize the destination in the rget descriptors.
            rgetMinus->setDestination (dest);
            rgetPlus ->setDestination (dest);

            // Set the remote injection fifo identifiers to E- and E+ respectively.
            uint32_t *rgetInjFifoIds = _context.getRgetInjFifoIds ();
            rgetMinus->setRemoteGetInjFIFOId ( rgetInjFifoIds[8] );
            rgetPlus ->setRemoteGetInjFIFOId ( rgetInjFifoIds[9] );

            // The "split e state" contains
            // - A completion counter that will count how many completion messages
            //   have been received (we are done when two have been received),
            // - The original callback function and cookie.
            multi_complete_t * multiCookie = & get_state->split_e_state;
            multiCookie->fn                = local_fn;
            multiCookie->cookie            = cookie;
            multiCookie->active            = 2;
            multiCookie->toBeFreed         = NULL; // Nothing to free.

            // Initialize the rget payload descriptor(s) for the E- rget
            void * e_minus_payload_vaddr = (void *) get_state->e_minus_payload;
            size_t bytes0 = bytes >> 1;
            size_t bytes1 = bytes - bytes0;

            size_t pbytes0 = static_cast<T_Model*>(this)->
                             initializeRemoteGetPayload (e_minus_payload_vaddr,
                                                         local_dst_pa,
                                                         remote_src_pa,
                                                         bytes0,
                                                         MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM,
                                                         MUHWI_PACKET_HINT_EM,
                                                         multiComplete,
                                                         multiCookie);
            // Determine the physical address of the rget payload buffer from
            // the model state memory.
            Kernel_MemoryRegion_t memRegion;
            uint32_t rc;
            rc = Kernel_CreateMemoryRegion (&memRegion, e_minus_payload_vaddr, T_Model::payload_size * 2);
            PAMI_assert_debug ( rc == 0 );
            uint64_t paddr = (uint64_t)memRegion.BasePa +
                             ((uint64_t)e_minus_payload_vaddr - (uint64_t)memRegion.BaseVa);
            TRACE_FORMAT("e_minus_payload_vaddr = %p, paddr = %ld (%p)", e_minus_payload_vaddr, paddr, (void *)paddr);

            rgetMinus->setPayload (paddr, pbytes0);

            /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Rget0",desc[0]); */
            /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Dput0",(MUHWI_Descriptor_t*)vaddr); */
            /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Memfifo-Completion0",((MUHWI_Descriptor_t*)vaddr)+1); */

            // Initialize the rget payload descriptor(s) for the E+ rget
            void * e_plus_payload_vaddr =  (void *) get_state->e_plus_payload;

            size_t pbytes1 = static_cast<T_Model*>(this)->
                             initializeRemoteGetPayload (e_plus_payload_vaddr,
                                                         local_dst_pa + bytes0,
                                                         remote_src_pa + bytes0,
                                                         bytes1,
                                                         MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP,
                                                         MUHWI_PACKET_HINT_EP,
                                                         multiComplete,
                                                         multiCookie);
            rgetPlus->setPayload (paddr + pbytes0, pbytes1);

            /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Rget1",desc[1]); */
            /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Dput1",(MUHWI_Descriptor_t*)vaddr); */
            /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Memfifo-Completion1",((MUHWI_Descriptor_t*)vaddr)+1); */

            TRACE_HEXDATA(&msg->desc[0], sizeof(MUHWI_Descriptor_t));
            TRACE_HEXDATA(&msg->desc[1], sizeof(MUHWI_Descriptor_t));

            // Post the message to the injection channel
            channel.post (msg);

            // The completion callback was not invoked; return false.
            TRACE_FN_EXIT();
            return false;
          } // End: Special E dimension case
        else
          { // Not special E dimension case
	    
            // Determine if this destination is on a network line
            uint32_t onALine = getOnALine ( dest );
            
            // If rget pacing is eligible between these two nodes, and the message size
            // exceeds the _rgetPacingSize threshold,  route the rget requests 
            // to the agent for processing.
	    if ( unlikely ( (paceRgetsToThisDest) && (bytes > _rgetPacingSize ) ) )
	      {
		int rc;
		// Need to pace this rget.  Prepare the descriptors in the 
		// comm agent's queue and post them to the comm agent.

		// An rget is submitted to the agent that flows to the dest node.
		// Its payload is a dput descriptor that gets injected in the
		// dest node's rget inj fifo.  The data flows back to our node.
		// There is no completion notification to anybody for this rget.
		
		// If there is a local_fn, the caller wants completion 
		// notification.  To do this, a 2nd rget is submitted to the
		// agent whose payload is a memfifo descriptor targeted for
		// our local reception fifo.  When that memfifo descriptor
		// arrives, it invokes the local_fn callback, notifying the
		// caller of the completion.  This 2nd rget does not need
		// to flow to the dest node.  Rather, it is a local transfer.
		// This 2nd rget is marked with a "peerID" so the agent
		// processes it after the 1st rget is complete...after its
		// dput data has fully arrived.  So, the 2nd rget does not
		// need to flow to the dest node.

		// 1. Allocate a slot in the agent's queue for the DPut RGet
		CommAgent_WorkRequest_t * workPtr;
		uint64_t uniqueID;
		_context.commAgent_AllocateWorkRequest( &workPtr, &uniqueID );

                // 2. Clone the rget model descriptor into the work request
                MUSPI_DescriptorBase * rget = (MUSPI_DescriptorBase *) &workPtr->request.rget.rgetDescriptor;
                _rget.clone (*rget);

                // 3. Initialize the rget descriptor in-place.
                rget->setDestination (dest);

                // 4. Determine the remote pinning information
                size_t rfifo =
                  _context.pinFifoToSelf (target_task, map);

                // 5. Set the remote injection fifo identifier
                rget->setRemoteGetInjFIFOId (rfifo);

                /* MUSPI_DescriptorDumpHex((char *)"Remote Get (DPut)", (MUHWI_Descriptor_t*)rget); */

                // 6. Initialize the DPut descriptor
		//    The DPut descriptor goes into the work request
		MUSPI_DescriptorBase * dput = 
		  (MUSPI_DescriptorBase *)&workPtr->request.rget.payloadDescriptor;

                static_cast<T_Model*>(this)->
		  initializeRemoteGetPayload1ForCommAgent (dput,
							   local_dst_pa,
							   remote_src_pa,
							   bytes,
							   map,
                                                           routingIndex,
                                                           onALine);

                /* MUSPI_DescriptorDumpHex((char *)"Pacing DPut (DPut)", (MUHWI_Descriptor_t*)dput); */

		// 7. Set the uniqueID into the request so this request and the next (see below)
		//    are tied together and processed sequentially.
		workPtr->request.rget.peerID = uniqueID;

		// 8. Set the global injection fifo ID into the request to be used by the agent
		//    as part of a uniquifier to order requests.
		uint32_t globalInjFifoId = channel.getGlobalFnum();
		workPtr->request.rget.globalInjFifo = globalInjFifoId;

		// 9. Post the rget with the dput descriptor to the comm agent.
		rc = _context.commAgent_RemoteGetPacing_SubmitWorkRequest( &workPtr->request.rget );
		PAMI_assert_debug ( rc == 0 );

		// 10. If there is a callback function, post a second rget with a memory fifo
		//     descriptor payload to handle the completion.
		if ( likely ( (uint64_t)local_fn ) )
		  {
		    uint64_t dummyUniqueID;
		    // 11. Allocate a slot in the agent's queue for the Memfifo RGet
		    _context.commAgent_AllocateWorkRequest( &workPtr, &dummyUniqueID );

		    // 12. Clone the rget model descriptor into the work request
		    rget = (MUSPI_DescriptorBase *) &workPtr->request.rget.rgetDescriptor;
		    _rget.clone (*rget);

		    // 13. Initialize the rget descriptor in-place.
		    rget->setDestination (*_myCoords);
		    //     - Set the pinned fifo/map information to local
		    rget->setTorusInjectionFIFOMap (MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1);

		    // 14. Set the remote injection fifo identifier
		    rget->setRemoteGetInjFIFOId (rfifo);
		    
		    /* MUSPI_DescriptorDumpHex((char *)"Remote Get (MemFifo)", (MUHWI_Descriptor_t*)rget); */
		    
		    // 15. Initialize the Memfifo descriptor
		    //    The Memfifo descriptor goes into the work request
		    MUSPI_DescriptorBase * memfifo = 
		      (MUSPI_DescriptorBase *)&workPtr->request.rget.payloadDescriptor;

		    static_cast<T_Model*>(this)->
		      initializeRemoteGetPayload2ForCommAgent (memfifo,
							       local_fn,
							       cookie);

		    /* MUSPI_DescriptorDumpHex((char *)"MemFifo", (MUHWI_Descriptor_t*)memfifo); */

		    // 16. Set the uniqueID into the request so this request and the previous (see above)
		    //    are tied together and processed sequentially.
		    workPtr->request.rget.peerID = uniqueID;

		    // 17. Set the global injection fifo ID into the request to be used by the agent
		    //     as part of a uniquifier to order requests.
		    workPtr->request.rget.globalInjFifo = globalInjFifoId;
		
		    // 18. Post the rget with the memfifo descriptor to the comm agent.
		    rc = _context.commAgent_RemoteGetPacing_SubmitWorkRequest( &workPtr->request.rget );
		    PAMI_assert_debug ( rc == 0 );
		  }
		
                TRACE_FN_EXIT();
                return false;
	      } // End: Pacing this rget.
	    else
              // Not pacing this rget.  Inject the rget descriptor ourselves.
              // This will use a hybrid form of routing and completion as follows:
              // 1. If there is no space in the inj fifo, a deterministically 
              //    routed DPut with memory fifo completion is queued.
              // 2. If there is space in the inj fifo,
              //    a. If there is no counter available, or this is DD1 hardware
              //       which doesn't support dynamic routing, the DPut is built with
              //       deterministic routing and memory fifo completion.
              //    b. If there is a counter available,, or this is DD2 hardware, 
              //       the DPut is built with dynamic routing and counter completion.
	      {
	       size_t ndesc = channel.getFreeDescriptorCountWithUpdate ();
	       if (likely(channel.isSendQueueEmpty() && ndesc > 0))
	       {
                // There is at least one descriptor slot available in the injection
                // fifo before a fifo-wrap event.
                MUHWI_Descriptor_t * desc = channel.getNextDescriptor ();

                // Clone the remote inject model descriptor into the injection fifo
                MUSPI_DescriptorBase * rget = (MUSPI_DescriptorBase *) desc;
                _rget.clone (*rget);

                // Initialize the injection fifo descriptor in-place.
                rget->setDestination (dest);

                // Determine the remote pinning information
                size_t rfifo =
                  _context.pinFifoToSelf (target_task, map);

                // Set the remote injection fifo identifier
                rget->setRemoteGetInjFIFOId (rfifo);

                //MUSPI_DescriptorDumpHex((char *)"Remote Get", desc);

                // Initialize the rget payload descriptor(s)
                void * vaddr;
                uint64_t paddr;
                channel.getDescriptorPayload (desc, vaddr, paddr);
                size_t pbytes = static_cast<T_Model*>(this)->
                          initializeRemoteGetPayloadHybrid (vaddr,
                                                            local_dst_pa,
                                                            remote_src_pa,
                                                            bytes,
                                                            map,
                                                            MUHWI_PACKET_HINT_E_NONE,
                                                            local_fn,
                                                            cookie,
                                                            routingIndex,
                                                            onALine);

                rget->setPayload (paddr, pbytes);
                            /* MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Rget",desc); */
                            /* MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Dput",(MUHWI_Descriptor_t*)vaddr); */
                            /* MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Memfifo-Completion",((MUHWI_Descriptor_t*)vaddr)+1); */

                // Finally, advance the injection fifo tail pointer. This action
                // completes the injection operation.
                channel.injFifoAdvanceDesc();

                // The completion callback was not invoked; return false.
                TRACE_FN_EXIT();
                return false;
              }

            // Construct and post a message
            // Create a simple single-descriptor message
            InjectDescriptorMessage<1, false> * msg =
              (InjectDescriptorMessage<1, false> *) get_state->msg1;
            new (msg) InjectDescriptorMessage<1, false> (channel);

            // Clone the remote inject model descriptor into the message
            MUSPI_DescriptorBase * rget = (MUSPI_DescriptorBase *) & msg->desc[0];
            _rget.clone (msg->desc[0]);

            // Determine the remote pinning information
            size_t rfifo =
              _context.pinFifoToSelf (target_task, map);

            // Set the remote injection fifo identifier
            rget->setRemoteGetInjFIFOId (rfifo);

            //MUSPI_DescriptorDumpHex((char *)"Remote Get", desc);

            // Initialize the rget payload descriptor(s)
            void * vaddr = (void *) get_state->e_minus_payload;
            size_t pbytes = static_cast<T_Model*>(this)->
                            initializeRemoteGetPayload (vaddr,
                                                        local_dst_pa,
                                                        remote_src_pa,
                                                        bytes,
                                                        map,
                                                        MUHWI_PACKET_HINT_E_NONE,
                                                        local_fn,
                                                        cookie);

            // Determine the physical address of the rget payload buffer from
            // the model state memory.
            Kernel_MemoryRegion_t memRegion;
            uint32_t rc;
            rc = Kernel_CreateMemoryRegion (&memRegion, vaddr, pbytes);
            PAMI_assert_debug ( rc == 0 );
            uint64_t paddr = (uint64_t)memRegion.BasePa +
                             ((uint64_t)vaddr - (uint64_t)memRegion.BaseVa);
            TRACE_FORMAT("vaddr = %p, paddr = %ld (%p)", vaddr, paddr, (void *)paddr);

            rget->setPayload (paddr, pbytes);
            /*             MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Rget",desc); */
            /*             MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Dput",(MUHWI_Descriptor_t*)vaddr); */
            /*             MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Memfifo-Completion",((MUHWI_Descriptor_t*)vaddr)+1); */

            TRACE_HEXDATA(&msg->desc[0], sizeof(MUHWI_Descriptor_t));

            // Post the message to the injection channel
            channel.post (msg);

            // The completion callback was not invoked; return false.
            TRACE_FN_EXIT();
            return false;
	   } // End: Not pacing this rget
          } // End: Not special E dimension case
      }

      template <class T_Model, unsigned T_PayloadSize>
      template <unsigned T_StateBytes>
      bool DmaModelBase<T_Model,T_PayloadSize>::postDmaFence_impl (uint8_t              (&state)[T_StateBytes],
                                                     pami_event_function   local_fn,
                                                     void                * cookie,
                                                     size_t                target_task,
                                                     size_t                target_offset)
      {
        TRACE_FN_ENTER();

        COMPILE_TIME_ASSERT(sizeof(fence_state_t) <= T_StateBytes);
        fence_state_t * fence_state = (fence_state_t *) state;

        ////////////////////////////////////////////////////////////////////////
        // Fence mu direct put operations
        ////////////////////////////////////////////////////////////////////////

        // A put-fence is defined to be complete after a memory fifo pingpong
        // with the remote node.  The ping must be injected into the same injection
        // fifo and torus injection fifo, and must be deterministically routed so it
        // follows behind the previous deterministically-routed put operations.
        // When the ping is received at the remote node, it sends a pong back to our
        // node, causing the local_fn callback to be invoked, indicating completion.

        // Before injecting the ping descriptor, determine the destination,
        // torus fifo map, and injection channel to use to get to the
        // destination.
        MUHWI_Destination_t   dest;
        uint16_t              rfifo;
        uint64_t              map;
	uint32_t              paceRgetsToThisDest;
        uint32_t              routingIndex;

        size_t fnum = _context.pinFifo (target_task,
                                        target_offset,
                                        dest,
                                        rfifo,
                                        map,
					paceRgetsToThisDest,
                                        routingIndex);

        InjChannel & channel = _context.injectionGroup.channel[fnum];

        //const bool is_nearest_neighbor_in_e = nearestNeighborInE (dest);

        fence_state->multi.fn        = local_fn;
        fence_state->multi.cookie    = cookie;
        fence_state->multi.toBeFreed = NULL;
        fence_state->counterPoolInfo = NULL;

        // We are expecting the following completion callbacks as a result of
        // the fence activity:
        // - Dput completion (always) (active initialized to 1).
        // - if nearestNeighborInE, E+ and E- completion (active incremented by 2).
        // - else
        //   - if dest is eligible for pacing, commAgent completion (active incremented by 1).
        //   - memory fifo completion (active incremented by 1).
        //   - if there are dynamic routing counters still active, counter completion
        //     (active incremented by number of counter pools with active counters).
        // So, the following calculates how many completions are expected (counter
        // completion added in later).

        fence_state->multi.active = 1;
        //if ( is_nearest_neighbor_in_e ) fence_state->multi.active += 2;
        //else
        {
          if ( paceRgetsToThisDest ) fence_state->multi.active++;
          fence_state->multi.active++;
        }

        // Dput Completion....
        _remoteCompletion.inject( fence_state->memfifo_remote_completion_state,
                                  channel,
                                  multiComplete,
                                  (void *) & fence_state->multi,
                                  map,
                                  dest.Destination.Destination);

        ////////////////////////////////////////////////////////////////////////
        // Fence mu remote get operations
        ////////////////////////////////////////////////////////////////////////

        // When the dest is on a line in the E dimension, the message is split into
        // two rgets, and flow one DPut in the E+ and the other in the E- to
        // optimize bandwidth.  This is possible because dest and our node are
        // linked via the two E links.
        //if ( unlikely ( is_nearest_neighbor_in_e ) )
        if(0)
          { // Special E dimension case
            // Get pointers to 2 descriptor slots, if they are available.
            MUHWI_Descriptor_t * desc[2];
            uint64_t descNum = channel.getNextDescriptorMultiple ( 2, desc );

            // If the send queue is empty and there is space in the fifo for
            // both descriptors, continue.
            if (likely( channel.isSendQueueEmpty() && (descNum != (uint64_t) - 1 )))
              {
                // Clone the remote inject model descriptors into the injection fifo slots.
                MUSPI_DescriptorBase * rgetMinus = (MUSPI_DescriptorBase *) desc[0];
                MUSPI_DescriptorBase * rgetPlus  = (MUSPI_DescriptorBase *) desc[1];
                _rget.clone (*rgetMinus);
                _rget.clone (*rgetPlus);

                // Initialize the destination in the rget descriptors.
                rgetMinus->setDestination (dest);
                rgetPlus ->setDestination (dest);

                // Set the remote injection fifo identifiers to E- and E+ respectively.
                uint32_t *rgetInjFifoIds = _context.getRgetInjFifoIds ();
                rgetMinus->setRemoteGetInjFIFOId ( rgetInjFifoIds[8] );
                rgetPlus ->setRemoteGetInjFIFOId ( rgetInjFifoIds[9] );

                // ----------------------------------------------------------------
                // Initialize the "ack to self" descriptor in the rget payload
                // ----------------------------------------------------------------
                void * vaddr;
                uint64_t paddr;

                channel.getDescriptorPayload (desc[0], vaddr, paddr);
                MUSPI_DescriptorBase * memfifo = (MUSPI_DescriptorBase *) vaddr;
                _remoteCompletion.initializeNotifySelfDescriptor (*memfifo, multiComplete, & fence_state->multi);
                memfifo->setTorusInjectionFIFOMap (MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM);
                memfifo->setHints (0, MUHWI_PACKET_HINT_EM);
                TRACE_HEXDATA(desc, 128);
                rgetMinus->setPayload (paddr, sizeof(MUHWI_Descriptor_t));

                /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Rget0",desc[0]); */
                /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Dput0",(MUHWI_Descriptor_t*)vaddr); */
                /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Memfifo-Completion0",((MUHWI_Descriptor_t*)vaddr)+1); */

                channel.getDescriptorPayload (desc[1], vaddr, paddr);
                memfifo = (MUSPI_DescriptorBase *) vaddr;
                _remoteCompletion.initializeNotifySelfDescriptor (*memfifo, multiComplete, & fence_state->multi);
                memfifo->setTorusInjectionFIFOMap (MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP);
                memfifo->setHints (0, MUHWI_PACKET_HINT_EP);
                TRACE_HEXDATA(desc, 128);
                rgetPlus->setPayload (paddr, sizeof(MUHWI_Descriptor_t));

                /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Rget1",desc[1]); */
                /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Dput1",(MUHWI_Descriptor_t*)vaddr); */
                /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Memfifo-Completion1",((MUHWI_Descriptor_t*)vaddr)+1); */

                // Finally, advance the injection fifo tail pointer. This action
                // completes the injection operation.
                channel.injFifoAdvanceDescMultiple(2);

                // The completion callback was not invoked; return false.
                TRACE_FN_EXIT();
                return false;
              }

            // Construct and post a message
            // Create a simple dual-descriptor message
            COMPILE_TIME_ASSERT((sizeof(InjectDescriptorMessage<2, false>) + (T_Model::payload_size*2)) <= T_StateBytes);

            InjectDescriptorMessage<2, false> * msg =
              (InjectDescriptorMessage<2, false> *) fence_state->split.msg;
            new (msg) InjectDescriptorMessage<2, false> (channel);

            // Clone the remote inject model descriptors into the injection fifo slots.
            MUSPI_DescriptorBase * rgetMinus = (MUSPI_DescriptorBase *) & msg->desc[0];
            MUSPI_DescriptorBase * rgetPlus  = (MUSPI_DescriptorBase *) & msg->desc[1];
            _rget.clone (*rgetMinus);
            _rget.clone (*rgetPlus);

            // Initialize the destination in the rget descriptors.
            rgetMinus->setDestination (dest);
            rgetPlus ->setDestination (dest);

            // Set the remote injection fifo identifiers to E- and E+ respectively.
            uint32_t *rgetInjFifoIds = _context.getRgetInjFifoIds ();
            rgetMinus->setRemoteGetInjFIFOId ( rgetInjFifoIds[8] );
            rgetPlus ->setRemoteGetInjFIFOId ( rgetInjFifoIds[9] );

            // Determine the physical address of the rget payload buffer from
            // the model state memory.
            void * e_minus_payload_vaddr = (void *) fence_state->split.e_minus_payload;
            void * e_plus_payload_vaddr = (void *) fence_state->split.e_plus_payload;
            Kernel_MemoryRegion_t memRegion;
            uint32_t rc;
            rc = Kernel_CreateMemoryRegion (&memRegion, e_minus_payload_vaddr, T_Model::payload_size * 2);
            PAMI_assert_debug ( rc == 0 );
            uint64_t paddr = (uint64_t)memRegion.BasePa +
                             ((uint64_t)e_minus_payload_vaddr - (uint64_t)memRegion.BaseVa);
            TRACE_FORMAT("e_minus_payload_vaddr = %p, paddr = %ld (%p)", e_minus_payload_vaddr, paddr, (void *)paddr);

            // Initialize the rget payload descriptor(s) for the E- rget
            MUSPI_DescriptorBase * memfifo = (MUSPI_DescriptorBase *) e_minus_payload_vaddr;
            _remoteCompletion.initializeNotifySelfDescriptor (*memfifo, multiComplete, & fence_state->multi);
            memfifo->setTorusInjectionFIFOMap (MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM);
            memfifo->setHints (0, MUHWI_PACKET_HINT_EM);
            rgetMinus->setPayload (paddr, sizeof(MUHWI_Descriptor_t));

            /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Rget0",desc[0]); */
            /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Dput0",(MUHWI_Descriptor_t*)vaddr); */
            /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Memfifo-Completion0",((MUHWI_Descriptor_t*)vaddr)+1); */

            // Initialize the rget payload descriptor(s) for the E+ rget
            memfifo = (MUSPI_DescriptorBase *) e_plus_payload_vaddr;
            _remoteCompletion.initializeNotifySelfDescriptor (*memfifo, multiComplete, & fence_state->multi);
            memfifo->setTorusInjectionFIFOMap (MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP);
            memfifo->setHints (0, MUHWI_PACKET_HINT_EP);
            rgetPlus->setPayload (paddr + sizeof(MUHWI_Descriptor_t), sizeof(MUHWI_Descriptor_t));

            /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Rget1",desc[1]); */
            /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Dput1",(MUHWI_Descriptor_t*)vaddr); */
            /* 		MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Memfifo-Completion1",((MUHWI_Descriptor_t*)vaddr)+1); */

            TRACE_HEXDATA(&msg->desc[0], sizeof(MUHWI_Descriptor_t));
            TRACE_HEXDATA(&msg->desc[1], sizeof(MUHWI_Descriptor_t));

            // Post the message to the injection channel
            channel.post (msg);

            // The completion callback was not invoked; return false.
            TRACE_FN_EXIT();
            return false;
          } // End: Special E dimension case
        else
          {// Not special E dimension case
	   if ( unlikely ( paceRgetsToThisDest ) )
	   {
	     // If rget pacing is eligible between these two nodes,  route an rget request
	     // to the agent for processing.  This rget request must be processed after
	     // any previous rget requests in the same ordering have completed.  In order
	     // to ensure this, this rget request must have the same attributes as the
	     // previous rget requests.  Specifically, 
	     // - The same global injection fifo ID
	     // - The same remote rget fifo ID
	     // - The same dest coords.
	     // This means the rget must flow to the dest, and its memfifo payload 
	     // descriptor will be injected in the same fifos as the previous rgets.

	     CommAgent_WorkRequest_t * workPtr;
	     uint64_t dummyUniqueID;

	     // 1. Allocate a slot in the agent's queue for the Memfifo RGet
	     _context.commAgent_AllocateWorkRequest( &workPtr, &dummyUniqueID );

	     // 2. Clone the rget model descriptor into the work request
	     MUSPI_DescriptorBase * rget = (MUSPI_DescriptorBase *) &workPtr->request.rget.rgetDescriptor;
	     _rget.clone (*rget);

	     // 3. Initialize the rget descriptor in-place.
	     rget->setDestination (dest);

	     // 4. Set the remote injection fifo identifier
	     size_t rfifo = _context.pinFifoToSelf (target_task, map);
	     rget->setRemoteGetInjFIFOId (rfifo);
		    
	     /* MUSPI_DescriptorDumpHex((char *)"Remote Get (MemFifo)", (MUHWI_Descriptor_t*)rget); */
		    
	     // 5. Initialize the Memfifo descriptor
	     //    The Memfifo descriptor goes into the work request
	     MUSPI_DescriptorBase * memfifo = 
	       (MUSPI_DescriptorBase *)&workPtr->request.rget.payloadDescriptor;

	     static_cast<T_Model*>(this)->
	       initializeRemoteGetPayload3ForCommAgent (memfifo,
							multiComplete, 
							& fence_state->multi,
							map);

	     /* MUSPI_DescriptorDumpHex((char *)"MemFifo", (MUHWI_Descriptor_t*)memfifo); */

	     // 6. Set the global injection fifo ID into the request to be used by the agent
	     //     as part of a uniquifier to order requests.
	     workPtr->request.rget.globalInjFifo =  channel.getGlobalFnum();
		
	     // 7. Post the rget with the memfifo descriptor to the comm agent.
	     int rc;
	     rc = _context.commAgent_RemoteGetPacing_SubmitWorkRequest( &workPtr->request.rget );
	     PAMI_assert_debug ( rc == 0 );
	   }

	   {
            
            // Set up to monitor counter completion, if there are any counters
            // waiting for completion.  Do this by 
            // 1. Allocating space for CounterPool fence state structures
            // 2. Adding those structures to the appropriate CounterPool object
            //    so it can monitor for completion of the active counters
            // 3. Incrementing the number of calls to multiComplete that are
            //    expected (only when there are counters waiting for completion).
            pami_result_t prc;
            uint64_t numCounterPools = _context.getNumCounterPools();

            // If CounterPools exist, monitor for any messages using counters.
            if ( numCounterPools )
            {
              prc = __global.heap_mm->memalign((void **)&fence_state->counterPoolInfo, 0,
                                               numCounterPools * sizeof(*fence_state->counterPoolInfo));
              PAMI_assertf(prc == PAMI_SUCCESS, "Not enough memory for fence state objects");

              uint64_t poolID;
              int rc2;
              for ( poolID=0; poolID<numCounterPools; poolID++)
              {
                // Register this fence operation with this CounterPool.
                rc2 = _context.addFenceOperation ( poolID,
                                                   &fence_state->counterPoolInfo[poolID],
                                                   multiComplete, 
                                                   & fence_state->multi );
                // If there are active counters in this pool, increment the number
                // of active fence sub-operations being monitored.
                if ( rc2 == 0 ) fence_state->multi.active++;
              }
              // Remember to free the fence_state counterPoolInfo.
              fence_state->multi.toBeFreed = fence_state->counterPoolInfo;
              fence_state->multi.mm        = __global.heap_mm;
            }
            // Memfifo completion....
            // Determine the remote pinning information
            size_t rfifo = _context.pinFifoToSelf (target_task, map);

            size_t ndesc = channel.getFreeDescriptorCountWithUpdate ();

            if (likely(channel.isSendQueueEmpty() && ndesc > 0))
              {
                // There is at least one descriptor slot available in the injection
                // fifo before a fifo-wrap event.
                MUHWI_Descriptor_t * desc = channel.getNextDescriptor ();

                // Clone the remote inject model descriptor into the injection fifo
                MUSPI_DescriptorBase * rget = (MUSPI_DescriptorBase *) desc;
                _rget.clone (*rget);

                // Initialize the injection fifo descriptor in-place.
                rget->setDestination (dest);

                // Set the remote injection fifo identifier
                rget->setRemoteGetInjFIFOId (rfifo);

                //MUSPI_DescriptorDumpHex((char *)"Remote Get", desc);

                // Initialize the rget payload descriptor(s)
                void * vaddr;
                uint64_t paddr;
                channel.getDescriptorPayload (desc, vaddr, paddr);

                // The "immediate" payload contains the remote descriptor
                MUSPI_DescriptorBase * memfifo = (MUSPI_DescriptorBase *) vaddr;

                // ----------------------------------------------------------------
                // Initialize the "ack to self" descriptor in the rget payload
                // ----------------------------------------------------------------
                _remoteCompletion.initializeNotifySelfDescriptor (*memfifo, multiComplete, & fence_state->multi);

                // Set the pinned fifo/map information
                memfifo->setTorusInjectionFIFOMap (map);

                // Set the ABCD hint bits to zero, and the E hint bits to the caller's
                memfifo->setHints (0, MUHWI_PACKET_HINT_E_NONE);

                //MUSPI_DescriptorDumpHex((char *)"Fifo Completion", memfifo);

                TRACE_HEXDATA(desc, 128);

                rget->setPayload (paddr, sizeof(MUHWI_Descriptor_t));
                /*             MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Rget",memfifo); */
                /*             MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Memfifo-Completion",(MUHWI_Descriptor_t*)vaddr); */

                // Finally, advance the injection fifo tail pointer. This action
                // completes the injection operation.
                channel.injFifoAdvanceDesc();

                // The completion callback was not invoked; return false.
                TRACE_FN_EXIT();
                return false;
              }

            // Construct and post a message
            // Create a simple single-descriptor message
            //COMPILE_TIME_ASSERT((sizeof(InjectDescriptorMessage<1, false>) + T_Model::payload_size) <= T_StateBytes);

            InjectDescriptorMessage<1, false> * msg =
              (InjectDescriptorMessage<1, false> *) fence_state->single.msg;
            new (msg) InjectDescriptorMessage<1, false> (channel);

            // Clone the remote inject model descriptor into the message
            MUSPI_DescriptorBase * rget = (MUSPI_DescriptorBase *) & msg->desc[0];
            _rget.clone (msg->desc[0]);

            // Set the remote injection fifo identifier
            rget->setRemoteGetInjFIFOId (rfifo);

            //MUSPI_DescriptorDumpHex((char *)"Remote Get", rget);

            // Initialize the rget payload descriptor(s)
            void * vaddr = (void *) fence_state->single.payload;

            // The "immediate" payload contains the remote descriptor
            MUSPI_DescriptorBase * memfifo = (MUSPI_DescriptorBase *) vaddr;

            // ----------------------------------------------------------------
            // Initialize the "ack to self" descriptor in the rget payload
            // ----------------------------------------------------------------
            _remoteCompletion.initializeNotifySelfDescriptor (*memfifo, multiComplete, & fence_state->multi);

            // Set the pinned fifo/map information
            memfifo->setTorusInjectionFIFOMap (map);

            // Set the ABCD hint bits to zero, and the E hint bits to the caller's
            memfifo->setHints (0, MUHWI_PACKET_HINT_E_NONE);

            //MUSPI_DescriptorDumpHex((char *)"Fifo Completion", memfifo);

            // Determine the physical address of the rget payload buffer from
            // the model state memory.
            Kernel_MemoryRegion_t memRegion;
            uint32_t rc;
            rc = Kernel_CreateMemoryRegion (&memRegion, vaddr, 0);
            PAMI_assert_debug ( rc == 0 );
            uint64_t paddr = (uint64_t)memRegion.BasePa +
                             ((uint64_t)vaddr - (uint64_t)memRegion.BaseVa);
            TRACE_FORMAT("vaddr = %p, paddr = %ld (%p)", vaddr, paddr, (void *)paddr);

            rget->setPayload (paddr, sizeof(MUHWI_Descriptor_t));
            /*             MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Rget",rget); */
            /*             MUSPI_DescriptorDumpHex((char*)"DmaModelBase-Memfifo-Completion",(MUHWI_Descriptor_t*)vaddr); */

            TRACE_HEXDATA(&msg->desc[0], sizeof(MUHWI_Descriptor_t));

            // Post the message to the injection channel
            channel.post (msg);

            // The completion callback was not invoked; return false.
            TRACE_FN_EXIT();
            return false;
	   } // End: Counter and Memfifo Completion
          } // End: Not special E dimension case

        TRACE_FN_EXIT();
        return true;
      }
    };   // PAMI::Device::MU namespace
  };     // PAMI::Device namespace
};       // PAMI namespace

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif // __components_devices_bgq_mu2_DmaModelBase_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
