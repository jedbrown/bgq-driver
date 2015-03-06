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
 * \file components/devices/bgq/mu2/model/DmaModelMemoryFifoCompletion.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_model_DmaModelMemoryFifoCompletion_h__
#define __components_devices_bgq_mu2_model_DmaModelMemoryFifoCompletion_h__

#include "components/devices/bgq/mu2/model/DmaModelBase.h"
#include "components/devices/bgq/mu2/model/MemoryFifoCompletion.h"
#include "components/devices/bgq/mu2/CounterPool.h"
#include "spi/include/kernel/location.h"
#include "hwi/include/bqc/A2_core.h"

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
      class DmaModelMemoryFifoCompletion : public MU::DmaModelBase<DmaModelMemoryFifoCompletion,sizeof(MUSPI_DescriptorBase)*2>
      {
        public :

          friend class MU::DmaModelBase<DmaModelMemoryFifoCompletion,sizeof(MUSPI_DescriptorBase)*2>;

          static const bool dma_model_fence_supported = true;
          static const size_t payload_size = sizeof(MUSPI_DescriptorBase)*2;

          /// \see PAMI::Device::Interface::DmaModel::DmaModel
          inline DmaModelMemoryFifoCompletion (MU::Context & context, pami_result_t & status) :
              MU::DmaModelBase<DmaModelMemoryFifoCompletion,sizeof(MUSPI_DescriptorBase)*2> (context, status),
              _completion (context)
          {
            // Determine if this is running on DD2 hardware
            uint32_t pvr; // Processor version register
            int rc;
            rc = Kernel_GetPVR( &pvr );
            assert(rc==0);
            if ( pvr == SPRN_PVR_DD1 )
              _isDD2 = false;
            else
              _isDD2 = true;
            
            unsigned int *routingOptions;
            routingOptions = __global.getFlexabilityMetricRoutingOptions();
            _flexabilityMetricRoutingOptions[0] = 
              ( (routingOptions[0] == 0) ? MUHWI_PACKET_ZONE_ROUTING_0 :
                (routingOptions[0] == 1) ? MUHWI_PACKET_ZONE_ROUTING_1 :
                (routingOptions[0] == 2) ? MUHWI_PACKET_ZONE_ROUTING_2 :
                (routingOptions[0] == 3) ? MUHWI_PACKET_ZONE_ROUTING_3 :
                0xFF /*deterministic */  );
            _flexabilityMetricRoutingOptions[1] = 
              ( (routingOptions[1] == 0) ? MUHWI_PACKET_ZONE_ROUTING_0 :
                (routingOptions[1] == 1) ? MUHWI_PACKET_ZONE_ROUTING_1 :
                (routingOptions[1] == 2) ? MUHWI_PACKET_ZONE_ROUTING_2 :
                (routingOptions[1] == 3) ? MUHWI_PACKET_ZONE_ROUTING_3 :
                0xFF /*deterministic */  );

            unsigned int smallRoutingOption = __global.getSmallRouting();
            _smallRoutingSize = __global.getSmallRoutingSize();
            _smallRouting =
              ( (smallRoutingOption == 0) ? MUHWI_PACKET_ZONE_ROUTING_0 :
                (smallRoutingOption == 1) ? MUHWI_PACKET_ZONE_ROUTING_1 :
                (smallRoutingOption == 2) ? MUHWI_PACKET_ZONE_ROUTING_2 :
                (smallRoutingOption == 3) ? MUHWI_PACKET_ZONE_ROUTING_3 :
                0xFF /*deterministic */  );

            COMPILE_TIME_ASSERT((sizeof(MUSPI_DescriptorBase)*2) <= MU::Context::immediate_payload_size);
          };

          /// \see PAMI::Device::Interface::DmaModel::~DmaModel
          inline ~DmaModelMemoryFifoCompletion () {};

	  /// \brief Initialize Remote Get Payload
	  ///
	  /// Initializes two descriptors in the remote get payload:
	  /// 1. A direct put descriptor that sends the data.  This is deterministically routed
	  ///    so the second descriptor's packet containing the completion indicator follows
	  ///    the data and arrives after all of the data has arrived.
	  /// 2. A memory fifo completion descriptor.
	  ///
          inline size_t initializeRemoteGetPayload (void                * vaddr,
                                                    uint64_t              local_dst_pa,
                                                    uint64_t              remote_src_pa,
                                                    size_t                bytes,
                                                    uint64_t              map,
                                                    uint8_t               hintsE,
                                                    pami_event_function   local_fn,
                                                    void                * cookie)
          {
            TRACE_FN_ENTER();

            // Retreive the route information back to mu context "self"


            // The "immediate" payload contains the remote descriptors
            MUSPI_DescriptorBase * desc = (MUSPI_DescriptorBase *) vaddr;

            // ----------------------------------------------------------------
            // Initialize the "data mover" descriptor in the rget payload
            // ----------------------------------------------------------------
            // Clone the remote direct put model descriptor into the payload
            _rput.clone (desc[0]);

            // Set the payload of the direct put descriptor to be the physical
            // address of the source buffer on the remote node (from the user's
            // memory region).
            desc[0].setPayload (remote_src_pa, bytes);

            // Set the destination buffer address for the remote direct put.
            //
            // The global BAT id is constant .. should only need to set
            // the "offset" (second parameter) here.
            desc[0].setRecPayloadBaseAddressInfo (_context.getGlobalBatId(), local_dst_pa);

            // Set the pinned fifo/map information
            desc[0].setTorusInjectionFIFOMap (map);

            // Set the ABCD hint bits to zero, and the E hint bits to the caller's
            desc[0].setHints (0, hintsE);

            if (unlikely(local_fn == NULL))
              {
                TRACE_FN_EXIT();
                return sizeof(MUHWI_Descriptor_t);
              }

            // ----------------------------------------------------------------
            // Initialize the "ack to self" descriptor in the rget payload
            // ----------------------------------------------------------------
            _completion.initializeNotifySelfDescriptor (desc[1], local_fn, cookie);

            // Set the pinned fifo/map information
            desc[1].setTorusInjectionFIFOMap (map);

            // Set the ABCD hint bits to zero, and the E hint bits to the caller's
            desc[1].setHints (0, hintsE);

            //MUSPI_DescriptorDumpHex((char *)"Remote Put", &desc[0]);
            //MUSPI_DescriptorDumpHex((char *)"Fifo Completion", &desc[1]);

            TRACE_HEXDATA(desc, 128);
            TRACE_FN_EXIT();

            return sizeof(MUHWI_Descriptor_t) * 2;
          };


          /// \brief Initialize Remote Get Payload (Hybrid)
          ///
          /// Initializes the remote get payload in one of two ways:
          ///
          /// 1. If this is DD2 hardware and there is an MU counter available, 
          ///    the remote get payload contains one descriptor:  a direct put
          ///    that is routed according to the routing configured for this system
          ///    size (dynamic zone 0,1,2,or 3, or deterministic).
          ///
          /// 2. If this is DD1 hardware (dynamic routing not supported), or no
          ///    MU counter is available, the remote get payload contains two
          ///    descriptors that are deterministically routed (see initializeRemoteGetPayload()).
          ///
          /// This essentially provides a hybrid approach where the data can be dynamically
          /// routed most of the time, but when all MU counters are in use, it reverts back to the
          /// deterministically routed memory fifo completion approach.
          ///
          inline size_t initializeRemoteGetPayloadHybrid (void                * vaddr,
                                                          uint64_t              local_dst_pa,
                                                          uint64_t              remote_src_pa,
                                                          size_t                bytes,
                                                          uint64_t              map,
                                                          uint8_t               hintsE,
                                                          pami_event_function   local_fn,
                                                          void                * cookie,
                                                          uint32_t              routingIndex,
                                                          uint32_t              onALine)
          {
            TRACE_FN_ENTER();

            // Determine the routing value (deterministic or dynamic zone 0,1,2,or 3).
            // When the dest is on a line with us, use deterministic.
            // Otherwise, when the msg size <= smallRoutingSizeThreshold, use the
            // small routing value (see PAMI_ROUTING).
            // Otherwise, use the routing determined by the flexability metric.
            uint8_t routingValue=0xFF; // Set deterministic.  Note that determinisitic is
                                       // already set into the descriptor.
            if ( onALine == 0 ) // Not on a line?
            {
              if ( bytes <= _smallRoutingSize )
              {
                routingValue = _smallRouting;
              }
              else
              {
                routingValue = _flexabilityMetricRoutingOptions[routingIndex];
              }
            }

            uint64_t numCounterPools = _context.getNumCounterPools();
            uint64_t poolID = 0;
            int64_t  counterNum = -1;

            // If dynamic routing and DD2 hardware and CounterPools exist, allocate a counter.
            if ( ( likely ( ( routingValue != 0xFF ) &&
                            ( _isDD2 ) &&  
                            ( numCounterPools != 0 ) ) ) )
            {
              // It is DD2 hardware.  Try to get a MU counter.
              while ( poolID < numCounterPools )
              {
                counterNum = _context.allocateCounter( poolID );
                if ( counterNum >= 0 ) break;
                poolID++;
              }
            }

            // If the counter was successfully allocated, build a dynamically routed
	    // dput descriptor in the rget payload.
	    if ( counterNum >= 0 )
	      {
		// Set the counter info
		_context.setCounter ( poolID,
                                      counterNum,
				      bytes,
				      local_fn,
				      cookie );

		// The "immediate" payload contains the remote descriptor
		MUSPI_DescriptorBase * desc = (MUSPI_DescriptorBase *) vaddr;

		// ----------------------------------------------------------------
		// Initialize the "data mover" descriptor in the rget payload
		// ----------------------------------------------------------------
		// Clone the remote direct put model descriptor into the payload
		_rput.clone (desc[0]);

		// Set the payload of the direct put descriptor to be the physical
		// address of the source buffer on the remote node (from the user's
		// memory region).
		desc[0].setPayload (remote_src_pa, bytes);

		// Set the destination buffer address for the remote direct put.
		//
		// The global BAT id is constant .. should only need to set
		// the "offset" (second parameter) here.
		desc[0].setRecPayloadBaseAddressInfo ( _context.getGlobalBatId(), 
						       local_dst_pa);

                // Set the torus injection fifo map.  Only 1 bit is set so the hardware
                // uses 1 fifo.  This eliminates disturbances of packets using the other
                // fifos.
                desc[0].setTorusInjectionFIFOMap ( map );
		
		// Set routing and zone.
                desc[0].setRouting ( MUHWI_PACKET_USE_DYNAMIC_ROUTING );
                desc[0].setPt2PtVirtualChannel ( MUHWI_PACKET_VIRTUAL_CHANNEL_DYNAMIC );
                desc[0].setZoneRouting ( routingValue );
#if 0
                  // When the following code is enabled to set all torus fifo map bits,
                  // the linktest benchmark that sends from 1 node to 10 others that are
                  // 1 hop away in all 10 directions degrades significantly.
                  // Turning this off for now.
                  
                  // When not a local transfer, set the pinned fifo/map information to use
                  // all 10 directional fifos to maximize performance when using dynamic routing.
                  
                  if ( ( map &
                         ( MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0 |
                           MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1 ) ) == 0 )
		  {
		    desc[0].setTorusInjectionFIFOMap ( MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AM |
						       MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AP |
						       MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BM |
						       MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BP |
						       MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CM |
						       MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CP |
						       MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM |
						       MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP |
						       MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM |
						       MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP );
		  }
#endif

		// Set the reception counter atomic address
		desc[0].setRecCounterBaseAddressInfo ( _context.getGlobalBatId(),
						       _context.getCounterAtomicOffset( poolID,
                                                                                        counterNum) );

		// Set the ABCD hint bits to zero, and the E hint bits to the caller's
		desc[0].setHints (0, hintsE);

		TRACE_HEXDATA(desc, 64);
		TRACE_FN_EXIT();

		return sizeof(MUHWI_Descriptor_t);
	      }

	    // If the counter was not successfully allocated, build a deterministically
	    // routed dput descriptor and a memfifo completion descriptor in the
	    // rget payload.
	    else
	      {
		return initializeRemoteGetPayload (vaddr,
						   local_dst_pa,
						   remote_src_pa,
						   bytes,
						   map,
						   hintsE,
						   local_fn,
						   cookie);
	      }
          };

          inline void initializeRemoteGetPayload1ForCommAgent (MUSPI_DescriptorBase * desc,
							       uint64_t              local_dst_pa,
							       uint64_t              remote_src_pa,
							       size_t                bytes,
							       uint64_t              map,
                                                               uint32_t              routingIndex,
                                                               uint32_t              onALine)
          {
            TRACE_FN_ENTER();

            // ----------------------------------------------------------------
            // Initialize the "data mover" descriptor in the rget payload at "desc"
            // ----------------------------------------------------------------

            // Clone the remote direct put model descriptor into the payload
            _rput.clone (*desc);

            // Set the payload of the direct put descriptor to be the physical
            // address of the source buffer on the remote node (from the user's
            // memory region).
            desc->setPayload (remote_src_pa, bytes);

            // Set the destination buffer address for the remote direct put.
            //
            // The global BAT id is constant .. should only need to set
            // the "offset" (second parameter) here.
            desc->setRecPayloadBaseAddressInfo (_context.getGlobalBatId(), local_dst_pa);

            // Set the pinned fifo/map information
            desc->setTorusInjectionFIFOMap (map);

            // Determine the routing value (deterministic or dynamic zone 0,1,2,or 3).
            // When the dest is on a line with us, use deterministic.
            // Otherwise, when the msg size <= smallRoutingSizeThreshold, use the
            // small routing value (see PAMI_ROUTING).
            // Otherwise, use the routing determined by the flexability metric.
            uint8_t routingValue=0xFF; // Set deterministic.  Note that determinisitic is
                                       // already set into the descriptor.
            if ( onALine == 0 ) // Not on a line?
            {
              if ( bytes <= _smallRoutingSize )
                routingValue = _smallRouting;
              else
                routingValue = _flexabilityMetricRoutingOptions[routingIndex];
            }

            if ( routingValue != 0xFF ) // Not deterministic routing?
            {
              desc->setRouting ( MUHWI_PACKET_USE_DYNAMIC_ROUTING );
              desc->setPt2PtVirtualChannel ( MUHWI_PACKET_VIRTUAL_CHANNEL_DYNAMIC );
              desc->setZoneRouting ( routingValue );
#if 0
              // When the following code is enabled to set all torus fifo map bits,
              // the linktest benchmark that sends from 1 node to 10 others that are
              // 1 hop away in all 10 directions degrades significantly.
              // Turning this off for now.
              
              // When not a local transfer, set the pinned fifo/map information to use
              // all 10 directional fifos to maximize performance when using dynamic routing.
              
              if ( ( map &
                     ( MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0 |
                       MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1 ) ) == 0 )
              {
                desc->setTorusInjectionFIFOMap ( MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AM |
                                                 MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AP |
                                                 MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BM |
                                                 MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BP |
                                                 MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CM |
                                                 MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CP |
                                                 MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM |
                                                 MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP |
                                                 MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM |
                                                 MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP );
              }
#endif
            }
          };

	  // This builds a memfifo descriptor targeted for our node.
	  // It is set up as a local transfer.
          inline void initializeRemoteGetPayload2ForCommAgent (MUSPI_DescriptorBase * desc,
							       pami_event_function   local_fn,
							       void                * cookie)
          {
            TRACE_FN_ENTER();

            // ----------------------------------------------------------------
            // Initialize the "ack to self memfifo completion" descriptor in the rget payload
            // ----------------------------------------------------------------
            _completion.initializeNotifySelfDescriptor (*desc, local_fn, cookie);

            // Set the pinned fifo/map information
            desc->setTorusInjectionFIFOMap (MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1);

            TRACE_FN_EXIT();
          };

	  // This builds a memfifo descriptor targeted for our node.
	  // It is set up as a remote transfer.
          inline void initializeRemoteGetPayload3ForCommAgent (MUSPI_DescriptorBase * desc,
							       pami_event_function   local_fn,
							       void                * cookie,
							       uint64_t              map)
          {
            TRACE_FN_ENTER();

            // ----------------------------------------------------------------
            // Initialize the "ack to self memfifo completion" descriptor in the rget payload
            // ----------------------------------------------------------------
            _completion.initializeNotifySelfDescriptor (*desc, local_fn, cookie);

            // Set the pinned fifo/map information
            desc->setTorusInjectionFIFOMap (map);

	    // Clear the hints
	    desc->setHints (0, MUHWI_PACKET_HINT_E_NONE);

            TRACE_FN_EXIT();
          };

        protected:

          MemoryFifoCompletion _completion;
          bool                 _isDD2;

          // Routing options based on the flexability metric.
          // The constructor converts the 0,1,2,3,4 values from __global
          // into the actual values passed into the setZoneRouting() descriptor
          // setter as follows:
          // __global value    _flexabilityMetricRoutingOptions Value
          // --------------    --------------------------------------
          //       0                MUHWI_PACKET_ZONE_ROUTING_0
          //       1                MUHWI_PACKET_ZONE_ROUTING_1
          //       2                MUHWI_PACKET_ZONE_ROUTING_2
          //       3                MUHWI_PACKET_ZONE_ROUTING_3
          //       4                0xFF (Deterministic Routing)
          uint8_t _flexabilityMetricRoutingOptions[2];

          // Routing based on message size.
          size_t  _smallRoutingSize;
          uint8_t _smallRouting;

      }; // PAMI::Device::MU::DmaModelMemoryFifoCompletion class
    };   // PAMI::Device::MU namespace
  };     // PAMI::Device namespace
};       // PAMI namespace

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif // __components_devices_bgq_mu2_DmaModelMemoryFifoCompletion_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
