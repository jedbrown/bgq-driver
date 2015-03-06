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

////////////////////////////////////////////////////////////////////////////////
///
/// \file components/devices/bgq/mu2/global/ResourceManager.h
///
/// \brief MU Resource Manager Definitions
///
/// The MU Resource Manager controls resources associated with the MU Device.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef __components_devices_bgq_mu2_global_ResourceManager_h__
#define __components_devices_bgq_mu2_global_ResourceManager_h__

#undef TRACE_CLASSROUTES
#undef MU_CR_DEBUG
#ifdef __FWEXT__

  #include <firmware/include/fwext/fwext.h>
  #include <firmware/include/Firmware.h>
  #include <hwi/include/bqc/A2_core.h>
  #include <hwi/include/bqc/A2_inlines.h>
  #include <firmware/include/fwext/fwext_lib.h>
  #include <firmware/include/fwext/fwext_nd.h>
  #include <firmware/include/fwext/fwext_mu.h>

#endif // __FWEXT__

#include <stdio.h>
#include <stdlib.h>
#include <hwi/include/bqc/MU_Macros.h>
#include "components/devices/bgq/mu2/CollectiveNetwork.h"
#include <hwi/include/bqc/mu_dcr.h>
#include <hwi/include/bqc/nd_500_dcr.h>
#include <hwi/include/bqc/nd_x2_dcr.h>
#include <spi/include/mu/InjFifo.h>
#include <spi/include/mu/RecFifo.h>
#include <spi/include/mu/Addressing.h>
#include <spi/include/mu/Addressing_inlines.h>
#include <spi/include/kernel/MU.h>
#include <common/bgq/ResourceManager.h>
#include <common/bgq/BgqPersonality.h>
#include <components/devices/bgq/mu2/InjGroup.h>
#include <components/devices/generic/Device.h>

#include "algorithms/geometry/Geometry.h"
#include "components/atomic/bgq/L2Mutex.h"
#include "components/atomic/indirect/IndirectMutex.h"
#include "components/devices/misc/AtomicMutexMsg.h"
typedef PAMI::Mutex::BGQ::IndirectL2 MUCR_mutex_t;
typedef PAMI::Device::SharedAtomicMutexMdl<MUCR_mutex_t> MUCR_mutex_model_t;
#include <spi/include/kernel/collective.h>
#include <spi/include/mu/Classroute_inlines.h>
#include <spi/include/mu/GIBarrier.h>
#include <spi/include/kernel/gi.h>

#include <agents/include/comm/commagent.h>
#include <agents/include/comm/rgetpacing.h>
#include <agents/include/comm/fence.h>

#ifdef TRACE_CLASSROUTES
  #include "trace_classroutes.h"
#endif // TRACE_CLASSROUTES

#include "util/trace.h"
#undef  DO_TRACE_ENTEREXIT
#define DO_TRACE_ENTEREXIT 0
#undef  DO_TRACE_DEBUG
#define DO_TRACE_DEBUG     0

#ifndef abs_x
  #define abs_x(x) ((x^(x>>31)) - (x>>31))
#endif

#define RETRY_THRESHOLD 8 

////////////////////////////////////////////////////////////////////////////////
/// \env{mudevice,PAMI_TRACE_CLASSROUTES}
///
/// Enables tracing for the
/// collective network classroutes, if compiled-in.
/// Presence of variable enables tracing, with value
/// 'g' causing output in DOT graph format, 'R' using
/// ranks instead of coords in graph, and 'B' adding
/// links for both directions. Any combination may be
/// used, however 'g' is implied by all.
///
/// \env{mudevice,PAMI_GI_INIT_TIMEOUT}
///
/// Specifies timeout value,
/// in cycles, for the MU GI Barrier Init, second phase.
/// This phase essentially waits for all members of
/// the classroute to complete initialization of the GI.
/// If all members do not complete init by the specified
/// number of cycles, an error is reported and the operation
/// aborts.
////////////////////////////////////////////////////////////////////////////////

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      static const size_t NumClassRoutes  = 16;

      static const size_t numTorusDirections           = BGQ_TDIMS<<1; // 10 directions
      static const size_t numFifoPinIndices            = 16;
      static const size_t optimalNumInjFifosPerContext = numTorusDirections;
      static const size_t optimalNumRecFifosPerContext = 1;

      ///
      /// \brief Point-to-Point Injection Fifo Pin Map Values
      ///
      /// There are 10 pinInfoEntries, one for each "number of inj fifos in the context".
      /// The 1st entry is for when there is 1 inj fifo in the context.
      /// The 2nd entry is for when there are 2 inj fifos in the context.
      /// ...
      /// The 10th entry is for when there are 10 inj fifos in the context.
      ///
      /// Within each entry, there are two arrays of 16 elements each.
      /// The torusInjFifoMaps array contains torus inj fifo map bits, 10
      /// for the torus directions, and 6 local transfers.
      /// The injFifoIds array contains inj fifo numbers to be used for the 10
      /// torus directions and 6 locals.
      /// The reason these are paired like this is to optimize context
      /// pinFifo()'s access to this info.  We want all of this info to be in
      /// one L2 cache line.  Each context will "get" one of these pairs
      /// and use it in its pinFifo().
      ///
      /// The Context PinFifo function chooses an inj fifo (0..9) that is
      /// optimal for sending to the destination, assuming there are 10
      /// inj fifos in the context.  Then, it indexes into
      /// the appropriate injFifoIds array, based on how many inj fifos are actually in
      /// the context, to get the actual fifo number to pin to.  It then indexes
      /// into the torusInjFifoMaps array to get that value for the descriptor.
      ///
      /// For torus transfers, the array index is 0..9, corresponding to A-,A+,...,E-,E+.
      /// For local transfers, the array index is 10..15, derived from the destination's
      /// T coordinate (to spread the local transfers among as many of the actual
      /// imFifos fifos as possible.
      /// In either case, the value in the injFifoIds array at that index is the inj fifo
      /// number to use to inject a descriptor for that transfer.
      ///
      /// One additional twist:
      /// For the 10 torus imFifos (1st 10 array elements), the A- maps to element
      /// 0, A+ to element 1, etc.  But, when there are less than 10 actual imFifos,
      /// the remaining directions must map to imFifos that have already been
      /// used for a direction.  We want this to be spread evenly so that all imFifos
      /// are kept evenly busy, so the values in the array will be adjusted during
      /// runtime initialization based on our T coordinate.
      ///
      /// For example, if there are 8 actual imFifos, the first 8 fifopin array values
      /// would be 0,1,2,3,4,5,6,7, saying that for messages traveling in the
      /// A-,A+,B-,B+,C-,C+,D-,D+ directions, those imFifos will be used.
      /// But, for messages needing to travel in the E- or E+ direction, which
      /// imFifos do we use?  We set those 2 values to 0 and 1, implying that
      /// imFifos 0 and 1 will take those messages.  But, this puts added pressure
      /// on imFifos 0 and 1.  To alleviate this, those 2 values will be
      /// adjusted based on our T coordinate.  We add our 2*T coordinate (2 being
      /// the number of over-stressed fifos) to the values in the table (0,1)
      /// and mod it with the number of fifos (8) to give a value in the range 0..7,
      /// depending on our T.  Thus, T=0 will use imFifos 0 and 1, while T=1 will
      /// use imFifos 2 and 3, and so on.
      ///
      /// For the last 6 elements of the injFifoIds array (for local transfers), we do a
      /// similar distribution among 6 of the imFifos.  Note we are limited to
      /// 6 for local transfers since there are only 6 more slots in the 16 element
      /// array left for us to use...the fifoPin value we store in the map cache
      /// can only be in the range 0..15.  We try to evenly distribute the local
      /// traffic among all of the imFifos so that the imFifos chosen for local
      /// traffic in each process are not always the same, thereby more evenly
      /// impacting the effect of local traffic across all of the directions.
      /// So, we adjust the last 6 values in the array by adding 6*T to each array
      /// element and then mod'ing the result with #fifos.  For example, if there
      /// are 10 imFifos, the array elements initially are set to 0,1,2,3,4,5.
      /// For a process with T=0, those are the imFifos that are used to send
      /// local messages to the different T destinations.  For a process with T=1,
      /// they are set to 6,7,8,9,0,1.
      ///
      // There will be numTorusDirections of these structures packed in an array...
      typedef struct pinInfoEntry
      {
        uint16_t injFifoIds[numFifoPinIndices];
        uint16_t torusInjFifoMaps[numFifoPinIndices];
      } __attribute__((__packed__)) pinInfoEntry_t;

      // Later, this will be used to initialize the structures by copying it into them...
      static const uint16_t pinInfo[2*numTorusDirections][numFifoPinIndices] =
      {
        // 1 inj fifo in the context
        { 0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0},   // injFifoIds array
        { MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1}, // torus inj fifo map array
        // 2 inj fifo in the context
        { 0,1,0,1,0,1,0,1,0,1,  0,1,0,1,0,1},   // injFifoIds array
        { MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1}, // torus inj fifo map array
        // 3 inj fifo in the context
        { 0,1,2,0,1,2,0,1,2,0,  1,2,0,1,2,0},   // injFifoIds array
        { MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1}, // torus inj fifo map array
        // 4 inj fifo in the context
        { 0,1,2,3,0,1,2,3,0,1,  2,3,0,1,2,3},   // injFifoIds array
        { MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1}, // torus inj fifo map array
        // 5 inj fifo in the context
        { 0,1,2,3,4,0,1,2,3,4,  0,1,2,3,4,0},   // injFifoIds array
        { MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1}, // torus inj fifo map array
        // 6 inj fifo in the context
        { 0,1,2,3,4,5,0,1,2,3,  4,5,0,1,2,3},   // injFifoIds array
        { MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1}, // torus inj fifo map array
        // 7 inj fifo in the context
        { 0,1,2,3,4,5,6,0,1,2,  3,4,5,6,0,1},   // injFifoIds array
        { MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1}, // torus inj fifo map array
        // 8 inj fifo in the context
        { 0,1,2,3,4,5,6,7,0,1,  2,3,4,5,6,7},   // injFifoIds array
        { MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1}, // torus inj fifo map array
        // 9 inj fifo in the context
        { 0,1,2,3,4,5,6,7,8,0,  1,2,3,4,5,6},   // injFifoIds array
        { MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1}, // torus inj fifo map array
        // 10 inj fifo in the context
        { 0,1,2,3,4,5,6,7,8,9,  0,1,2,3,4,5},    // injFifoIds array
        { MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0,
          MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL1} // torus inj fifo map array
      };

      typedef struct muResources
      {
        size_t numInjFifos;
        size_t numRecFifos;
        size_t numBatIds;
      } muResources_t;

      typedef struct pamiResources
      {
        size_t numContexts;
      } pamiResources_t;

      typedef struct injFifoResources
      {
        MUSPI_InjFifoSubGroup_t  *subgroups;
        char                    **fifoPtrs;
        uint32_t                 *globalFifoIds;
        char                    **lookAsidePayloadPtrs;
        uint64_t                 *lookAsidePayloadPAs;
        Kernel_MemoryRegion_t    *lookAsidePayloadMemoryRegions;
        pami_event_function     **lookAsideCompletionFnPtrs;
        void                   ***lookAsideCompletionCookiePtrs;
      } injFifoResources_t;

      typedef struct recFifoResources
      {
        MUSPI_RecFifoSubGroup_t  *subgroups;
        char                    **fifoPtrs;
        uint32_t                 *globalFifoIds;
      } recFifoResources_t;

      typedef struct batResources
      {
        MUSPI_BaseAddressTableSubGroup_t *subgroups;
        uint32_t                         *globalBatIds;
        uint32_t                          status; // 1 bit per BATid, starting with high-order
        // bit 0.  1=free, 0=allocated.
      } batResources_t;

      typedef struct clientResources
      {
        size_t              numContexts;
        uint32_t           *startingSubgroupIds; // One entry per context
        uint32_t           *endingSubgroupIds;   // One entry per context
        injFifoResources_t *injResources;        // One entry per context
        recFifoResources_t *recResources;        // One entry per context
        batResources_t     *batResources;        // One entry per context
        uint16_t           *pinRecFifo;          // 2D array [tcoord][ctxOffset] containing
                                                 // globalRecFifoId.
        uint16_t          **pinRecFifoPtrs;      // Pointers into the context-specific
        // section of the pinRecFifo array,
        // one pointer per context.
        uint16_t           *pinBatId;            // 2D array [tcoord][ctxOffset] containing
                                                 // globalBatId of first BatId in the group.
      } clientResources_t;

      class ResourceManager
      {
      private:
        struct cr_shared
        {
          uint32_t ncncr, cnfirst, cnused;
          uint32_t ngicr, gifirst, giused;
          uint32_t cn_crs[BGQ_COLL_CLASS_MAX_CLASSROUTES];
          uint32_t gi_crs[BGQ_COLL_CLASS_MAX_CLASSROUTES];
        };
        // This is a mm init function. Only the first alloc will call this,
        // and all subsequent allocs will wait until this completes.
        static void cr_init_shm(void *mem, size_t bytes, const char *key,
                                unsigned attrs, void *cookie)
        {
          //ResourceManager *thus = (ResourceManager *)cookie;
          struct cr_shared *sh_crs = (struct cr_shared *)mem;

          int rc;
          uint32_t N;
          uint32_t nresv = 0;

          char *s = getenv("MUSPI_NUMCLASSROUTES");
          if(s)
          {
            nresv = strtoul(s, NULL, 0);
          }
          rc = Kernel_QueryCollectiveClassRoutes(&sh_crs->ncncr,
                                                 sh_crs->cn_crs, sizeof(sh_crs->cn_crs));
          PAMI_assert_alwaysf(rc == 0, "Kernel_QueryCollectiveClassRoutes failed %d", rc);
          // we take the "last" N ids, where "N" is some number we get from a
          // resource manager - TBD.
          if(sh_crs->ncncr < nresv)
          {
            N = 0;
          }
          else
          {
            N = sh_crs->ncncr - nresv;
          }

          sh_crs->cnfirst = sh_crs->ncncr - N;
          sh_crs->cnused = N;
          uint32_t x;
          for(x = sh_crs->cnfirst; x < sh_crs->cnfirst + sh_crs->cnused; ++x)
          {
            rc = Kernel_AllocateCollectiveClassRoute(sh_crs->cn_crs[x]);
#ifdef MU_CR_DEBUG
            if(rc) fprintf(stderr, "Kernel_AllocateCollectiveClassRoute(%d) failed %d %d\n", sh_crs->cn_crs[x], rc, errno);
#else // !MU_CR_DEBUG
            PAMI_assert_alwaysf(rc == 0, "Kernel_AllocateCollectiveClassRoute(%d) failed %d %d", sh_crs->cn_crs[x], rc, errno);
#endif // !MU_CR_DEBUG
          }


          rc = Kernel_QueryGlobalInterruptClassRoutes(&sh_crs->ngicr,
                                                      sh_crs->gi_crs, sizeof(sh_crs->gi_crs));
          PAMI_assert_alwaysf(rc == 0, "Kernel_QueryCollectiveClassRoutes failed %d", rc);

          // we take the "last" N ids, where "N" is some number we get from a
          // resource manager - TBD.
          if(sh_crs->ngicr < nresv)
          {
            N = 0;
          }
          else
          {
            N = sh_crs->ngicr - nresv;
          }

          sh_crs->gifirst = sh_crs->ngicr - N;
          sh_crs->giused = N;

          for(x = sh_crs->gifirst; x < sh_crs->gifirst + sh_crs->giused; ++x)
          {
            //rc = Kernel_AllocateGlobalInterruptClassRoute(sh_crs->gi_crs[x], NULL);
            rc = Kernel_AllocateGlobalInterruptClassRoute(sh_crs->gi_crs[x], NULL);
#ifdef MU_CR_DEBUG
            if(rc) fprintf(stderr, "Kernel_AllocateGlobalInterruptClassRoute(%d) failed %d %d\n", sh_crs->gi_crs[x], rc, errno);
#else // !MU_CR_DEBUG
            PAMI_assert_alwaysf(rc == 0, "Kernel_AllocateGlobalInterruptClassRoute(%d) failed %d %d", sh_crs->gi_crs[x], rc, errno);
#endif // !MU_CR_DEBUG
          }
        }

      public:

        //////////////////////////////////////////////////////////////////////////
        ///
        /// \brief PAMI Resource Manager Default Constructor
        ///
        /////////////////////////////////////////////////////////////////////////
        ResourceManager ( PAMI::ResourceManager &pamiRM,
                          PAMI::Mapping         &mapping,
                          PAMI::BgqJobPersonality  &pers,
                          PAMI::Memory::MemoryManager   &mm )  :
        _pamiRM( pamiRM ),
        _mapping( mapping ),
        _pers( pers ),
        _cncrdata(NULL),
        _gicrdata(NULL),
        _inited(NULL),
        _tSize( mapping.tSize() ),
        _myT( mapping.t() ),
        _perProcessMaxPamiResources( NULL ),
        _perProcessOptimalPamiResources( NULL ),
        _globalRecSubGroups( NULL )
        {
          TRACE_FN_ENTER();
          // Only initialize global MU resources if we are using the MU
          // Otherwise, machine checks will result when touching the
          // MU MMIO storage.
          if( __global.useMU() )
          {
            // Verify that the MU and ND are not in reset.
            // They must be out of reset in order to set up the MU resources,
            // or a machine check will occur.
            uint64_t val1, val2, val3;
            val1 = DCRReadUser(MU_DCR(RESET) );
            val2 = DCRReadUser(ND_X2_DCR(RESET));
            val3 = DCRReadUser(ND_500_DCR(RESET));
            if( ( MU_DCR__RESET__DCRS_OUT_get(val1) ) ||
                ( ND_X2_DCR__RESET__DCRS_OUT_get(val2) ) ||
                ( ND_500_DCR__RESET__DCRS_OUT_get(val3) ) )
              PAMI_assertf( 0, "The ND and/or MU are still in reset.  Ensure you have +ND and +MU in your svchost file");

            // For each task, cache the optimal fifo pin in the mapcache.
            initFifoPin();

            // Set up the global resources
            allocateGlobalResources();
            TRACE_STRING("MU ResourceManager: Done allocating global resources");

            TRACE_FORMAT("GlobalBatId=%u, SharedCounterBatId=%u, ShortCollectiveBatId=%u, ThroughputCollectiveBufferBatId=%u, ThroughputCollectiveCounterBatId=%u",getGlobalBatId(),getSharedCounterBatId(),getShortCollectiveBatId(),getThroughputCollectiveBufferBatId(), getThroughputCollectiveCounterBatId());

            // might want more shmem here, to use for coordinating locals in VN.
            // possibly changing this to a structure.
            pami_result_t prc;
            prc = mm.memalign((void **)&_lowest_geom_id, sizeof(void *), 1 * sizeof(void *),
                              "/pami-mu-rm-cr-_lowest_geom_id");
            PAMI_assert_alwaysf(prc == PAMI_SUCCESS, "Failed to get shmem for _lowest_geom_id");
            *_lowest_geom_id = 0xffffffff;

            TRACE_STRING("MU ResourceManager: Inside ENABLE_MU_CLASSROUTES code");
            // This init code is performed ONCE per process and is shared by
            // all clients and their contexts...
            // 
            // I'm intentionally not converting:
            //   __global.topology_master.convertTopology(PAMI_COORD_TOPOLOGY)
            // from rank list to coord because there isn't a good recovery if it doesn't work.  
            // So we will use our own copy in _node_topo and not directly use __global.topology_master
            // 
            // This makes a copy of the ranklist
            //  _node_topo = __global.topology_master;
            // 
            // We'd rather share the same ranklist:
            pami_task_t *mrank_list ;
            __global.topology_master.rankList(&mrank_list);
            new (&_node_topo) Topology(mrank_list,__global.topology_master.size(),0,1);
            _node_topo.convertTopology(PAMI_COORD_TOPOLOGY); /// \todo What if this fails??

            // don't know where 'this' was allocated, so can't call MUCR_mutex_t::checkCtorMm

            // should this mutex be in the WAC?
            PAMI::Memory::MemoryManager *mxmm = &__global.l2atomicFactory.__procscoped_mm;
            _cr_mtx.init(mxmm, "/pami-mu-rm-cr-lk");
            prc = __global.heap_mm->memalign((void **)&_cr_mtx_mdls,
                                             sizeof(void *), _pamiRM.getNumClients() * sizeof(*_cr_mtx_mdls));
            PAMI_assert_alwaysf(prc == PAMI_SUCCESS, "Failed to alloc mem for CR mutex models");

            // Note, we NEVER use BGQ_CLASS_INPUT_VC_USER. Only BGQ_CLASS_INPUT_VC_SUBCOMM.
            struct cr_shared *sh_crs;
            prc = mm.memalign((void **)&sh_crs, sizeof(void *), sizeof(*sh_crs),
                              "/pami-mu-rm-cr-shm", cr_init_shm, this);
            PAMI_assert_alwaysf(prc == PAMI_SUCCESS, "Failed to alloc mem for CR shared init");

            int i;
            char *s;
#ifdef MU_CR_DEBUG
            static char buf[4096];
            s = buf;
            s += sprintf(s, "Got CN classroutes");
            for(i = sh_crs->cnfirst; i < (int)(sh_crs->cnfirst + sh_crs->cnused); ++i)
            {
              s += sprintf(s, " %d", sh_crs->cn_crs[i]);
            }
            s += sprintf(s, "\nGot GI classroutes");
            for(i = sh_crs->gifirst; i < (int)(sh_crs->gifirst + sh_crs->giused); ++i)
            {
              s += sprintf(s, " %d", sh_crs->gi_crs[i]);
            }
            fprintf(stderr, "%s\n", buf);
#endif // MU_CR_DEBUG

            // Only one process on each node will actually reserve the classroutes,
            // and then will share the info with all the rest.

            // locally "block-out" our reserved classroutes...
            MUSPI_InitClassrouteIds(&sh_crs->cn_crs[sh_crs->cnfirst], sh_crs->cnused,
                                    0, &_cncrdata);

            // locally "block-out" our reserved classroutes...
            MUSPI_InitClassrouteIds(&sh_crs->gi_crs[sh_crs->gifirst], sh_crs->giused,
                                    0, &_gicrdata);

            mm.free(sh_crs);

            // next, add classroute #0 (user comm-world) which is
            // pre-fabricated by controlsystem and CNK (not in free list, above).
            // but, we need to cnostruct the rectangle for it ourself...

            // this code should be very similar to classroute_test.c
            // Get all the rectangles and coords we need to describe the system.

            _pers.blockRectangle(*CR_RECT_LL(&_refcomm), *CR_RECT_UR(&_refcomm));
            _pers.blockCoord(_mycoord);

            MUSPI_PickWorldRoot(&_refcomm, NULL, &_refroot, &_pri_dim);
            /// \todo #warning need authoritative CNK default mapping constant
            s = (char*)"ABCDET";
            i = 0;
            if(s)
            {
              char *t = s + strlen(s) - 1;
              const char *x;
              const char *m = CR_DIM_NAMES;
              for(; t >= s && i < CR_NUM_DIMS; --t)
              {
                if(*t == 'T') continue;
                x = strchr(m, *t);
                if(!x) break;
                _map[i++] = (x - m);
              }
            }
            // if we did not get all dimensions set, punt.
            if(i != CR_NUM_DIMS)
            {
              // error - invalid map string, or an actual map file...
              // no classroutes supported (no MU collectives at all?)
              // \todo #warning need to handle "usable" mapfiles somehow

              // for now, just use simple 1:1 mapping
              for(i = 0; i < CR_NUM_DIMS; ++i) _map[i] = (CR_NUM_DIMS - i - 1);
            }

            _pers.jobRectangle(*CR_RECT_LL(&_communiv), *CR_RECT_UR(&_communiv));

            size_t univz = __MUSPI_rect_size(&_communiv);
            // now, factor in any -np...
            _np = __global.mapping.numActiveNodes();
            prc = __global.heap_mm->memalign((void **)&_excluded, 0,
                                             (univz - _np + 1) * sizeof(CR_COORD_T));
            PAMI_assert_alwaysf(prc == PAMI_SUCCESS, "alloc of _excluded failed");

            // Note, this discards previous _pri_dim... is that ok?
            MUSPI_MakeNpRectMap(&_communiv, _np, _map,
                                &_commworld, _excluded, &_nexcl, &_pri_dim);
#ifdef MU_CR_DEBUG
            s = buf;
            s += sprintf(s, "Block rectangle (%zd,%zd,%zd,%zd,%zd):(%zd,%zd,%zd,%zd,%zd)",
                         CR_COORD_DIM(CR_RECT_LL(&_refcomm),0),
                         CR_COORD_DIM(CR_RECT_LL(&_refcomm),1),
                         CR_COORD_DIM(CR_RECT_LL(&_refcomm),2),
                         CR_COORD_DIM(CR_RECT_LL(&_refcomm),3),
                         CR_COORD_DIM(CR_RECT_LL(&_refcomm),4),
                         CR_COORD_DIM(CR_RECT_UR(&_refcomm),0),
                         CR_COORD_DIM(CR_RECT_UR(&_refcomm),1),
                         CR_COORD_DIM(CR_RECT_UR(&_refcomm),2),
                         CR_COORD_DIM(CR_RECT_UR(&_refcomm),3),
                         CR_COORD_DIM(CR_RECT_UR(&_refcomm),4));
            bool inside = true;
            for(i = 0; i < CR_NUM_DIMS; ++i) inside = (inside &&
                                                       CR_COORD_DIM(&_mycoord,i) >= CR_COORD_DIM(CR_RECT_LL(&_commworld),i) &&
                                                       CR_COORD_DIM(&_mycoord,i) <= CR_COORD_DIM(CR_RECT_UR(&_commworld),i)
                                                      );
            s += sprintf(s, "\nBlock Root (%zd,%zd,%zd,%zd,%zd), My coord (%zd,%zd,%zd,%zd,%zd)%s, Mapping ",
                         CR_COORD_DIM(&_refroot,0),
                         CR_COORD_DIM(&_refroot,1),
                         CR_COORD_DIM(&_refroot,2),
                         CR_COORD_DIM(&_refroot,3),
                         CR_COORD_DIM(&_refroot,4),
                         CR_COORD_DIM(&_mycoord,0),
                         CR_COORD_DIM(&_mycoord,1),
                         CR_COORD_DIM(&_mycoord,2),
                         CR_COORD_DIM(&_mycoord,3),
                         CR_COORD_DIM(&_mycoord,4), !inside ? "*" : "");
            for(i = 0; i < CR_NUM_DIMS; ++i) *s++ = "ABCDEFGHIJKL"[_map[i]];
            *s++ = '\0';
            fprintf(stderr, "%s\n", buf);

            fprintf(stderr, "Universe rectangle (%zd,%zd,%zd,%zd,%zd):(%zd,%zd,%zd,%zd,%zd)\n",
                    CR_COORD_DIM(CR_RECT_LL(&_communiv),0),
                    CR_COORD_DIM(CR_RECT_LL(&_communiv),1),
                    CR_COORD_DIM(CR_RECT_LL(&_communiv),2),
                    CR_COORD_DIM(CR_RECT_LL(&_communiv),3),
                    CR_COORD_DIM(CR_RECT_LL(&_communiv),4),
                    CR_COORD_DIM(CR_RECT_UR(&_communiv),0),
                    CR_COORD_DIM(CR_RECT_UR(&_communiv),1),
                    CR_COORD_DIM(CR_RECT_UR(&_communiv),2),
                    CR_COORD_DIM(CR_RECT_UR(&_communiv),3),
                    CR_COORD_DIM(CR_RECT_UR(&_communiv),4));
            fprintf(stderr, "World rectangle (%zd,%zd,%zd,%zd,%zd):(%zd,%zd,%zd,%zd,%zd)\n",
                    CR_COORD_DIM(CR_RECT_LL(&_commworld),0),
                    CR_COORD_DIM(CR_RECT_LL(&_commworld),1),
                    CR_COORD_DIM(CR_RECT_LL(&_commworld),2),
                    CR_COORD_DIM(CR_RECT_LL(&_commworld),3),
                    CR_COORD_DIM(CR_RECT_LL(&_commworld),4),
                    CR_COORD_DIM(CR_RECT_UR(&_commworld),0),
                    CR_COORD_DIM(CR_RECT_UR(&_commworld),1),
                    CR_COORD_DIM(CR_RECT_UR(&_commworld),2),
                    CR_COORD_DIM(CR_RECT_UR(&_commworld),3),
                    CR_COORD_DIM(CR_RECT_UR(&_commworld),4));
            s = buf;
            s += sprintf(s, "NP=%zd", _np);
            if(_nexcl)
            {
              s += sprintf(s, " Excluding nodes:");
              for(i = 0; i < _nexcl; ++i)
              {
                bool exclude_me = __MUSPI_eq_coords(&_mycoord, &_excluded[i]);
                s += sprintf(s, "\n\t(%zd,%zd,%zd,%zd,%zd)%s",
                             CR_COORD_DIM(&_excluded[i],0),
                             CR_COORD_DIM(&_excluded[i],1),
                             CR_COORD_DIM(&_excluded[i],2),
                             CR_COORD_DIM(&_excluded[i],3),
                             CR_COORD_DIM(&_excluded[i],4), exclude_me ? "*" : "");
              }
            }
            fprintf(stderr, "%s\n", buf);
#endif // MU_CR_DEBUG
            // could do: new (t) Topology(CR_RECT_LL(&_commworld),
            //                              CR_RECT_UR(&_commworld), ... );
            //
            // Must be able to determine if a geometry is "comm world"
            // (taking into account excluded nodes), but otherwise every
            // classroute will be a full rectangle.
            // (how will commworld (topology) be represented in those cases?)

            // Caution: this is abnormal use of the classroute API.
            // This sets up our tables for a pre-existing classroute for the
            // entire job. So, we only update our table, we don't write DCRs
            // or any such thing.
            MUSPI_SetClassrouteId(0, PAMI_MU_CR_SPI_VC, &_commworld, &_cncrdata);
            MUSPI_SetClassrouteId(0, PAMI_MU_CR_SPI_VC, &_commworld, &_gicrdata);
#ifdef TRACE_CLASSROUTES
            _digraph = 0;
            _trace_cr = 0;

            s = getenv("PAMI_TRACE_CLASSROUTES");
            if(s)
            {
              _trace_cw.rect = _refcomm;
              _trace_cw.root = _refroot;
              _trace_cw.map = _map;
              _trace_cw.pri_dim = _pri_dim;
              _trace_cr = 1;
              while(*s)
              {
                switch(*s)
                {
                case 'B': _digraph |= (CRTEST_GRAPH | CRTEST_GRAPH_BI_DIR); break;
                case 'R': _digraph |= (CRTEST_GRAPH | CRTEST_GRAPH_RANKS); break;
                case 'g': _digraph |= CRTEST_GRAPH; break;
                }
                ++s;
              }
            }
#endif // TRACE_CLASSROUTES
            _gi_init_to = 1000;
            s = getenv("PAMI_GI_INIT_TIMEOUT");
            if(s)
            {
              uint64_t v = strtoul(s, NULL, 0);
              if(v) _gi_init_to = v;
            }
            TRACE_FORMAT("_gi_init_to %llu",(long long)_gi_init_to);
            // do we need to place this rectangle someplace? like the "world geom" topology?

            // Now, we should be ready to get requests for classroutes...
            // via the geomOptimize() method below...
            TRACE_STRING("MU ResourceManager: Exiting constructor");
          } // End: UseMU
          TRACE_FN_EXIT();
        } // End: ResourceManager Default Constructor
        ~ResourceManager()
        {
        }
        // Note, we NEVER use BGQ_CLASS_INPUT_VC_USER. Only BGQ_CLASS_INPUT_VC_SUBCOMM.
#define CR_ALLREDUCE_DT_CT 5
        struct cr_cookie
        {
          ResourceManager *thus;
          PAMI::Geometry::Common *geom;
          bool master;
          unsigned geom_id;
          PAMI::Topology topo;
          CR_RECT_T rect;
          size_t   client;
          size_t   context;
          pami_multisync_t msync;
          MUCR_mutex_model_t *cr_mtx_mdl;
          uint8_t mbuf[MUCR_mutex_model_t::sizeof_msg];
          pami_xfer_t xfer;
          uint64_t abuf[CR_ALLREDUCE_DT_CT];
          uint64_t bbuf[CR_ALLREDUCE_DT_CT];
          pami_work_t post;
          pami_callback_t cb_done;
          uint32_t id; // only used by GI classroute hw init
          ClassRoute_t cr;
        };

        // this code should be very similar to classroute_test.c,
        // however, this must be non-blocking code so we have to
        // do this as work posted to context(s).
        // we only need one attempt per geometry, so use a special value
        // to indicate "can't". Note, this routine will be called several times
        // for a given geometry, during create. Once for each potential algorithm.
        // but the answer we get is valid for all algorithms, so don't keep trying.
        inline pami_result_t geomOptimize(PAMI::Geometry::Common *geom,
                                          size_t clientid, size_t contextid, pami_context_t context,
                                          pami_event_function fn, void *clientdata)
        {
          // we always try to optimize, since this is called only once
          // per geometry (per change in PAMI_GEOMETRY_OPTIMIZE config)
          // however, if already fully optimized, don't bother.
          if(geom->getKey(0,PAMI::Geometry::CKEY_MCAST_CLASSROUTEID) &&
             geom->getKey(0,PAMI::Geometry::CKEY_MSYNC_CLASSROUTEID))
          {
            if(fn) fn(context, clientdata, PAMI_SUCCESS);
            return PAMI_SUCCESS;
          }

          // check for comm-world and just use well-known classroute id "0"...

          // simple check - works for all numbers of processes-per-node
          if(geom->size() == __global.topology_global.size())
          {
            geom->setKey(0,PAMI::Geometry::CKEY_MCAST_CLASSROUTEID, (void *)(0 + 1));
            geom->setKey(0,PAMI::Geometry::CKEY_MCOMB_CLASSROUTEID, (void *)(0 + 1));
            geom->setKey(0,PAMI::Geometry::CKEY_MSYNC_CLASSROUTEID, (void *)(0 + 1));
            if(fn) fn(context, clientdata, PAMI_SUCCESS);
            return PAMI_SUCCESS;
          }

          /// \todo #warning needs to be modified to work with -np comm-worlds

          PAMI::Topology *topo = (PAMI::Topology *)geom->getTopology(PAMI::Geometry::COORDINATE_TOPOLOGY_INDEX);
          // try to convert to a rectangle... will NO-OP if already rectangle...
          // this will change the topology in the geometry... should be OK but
          // if there are other ways to optimize we need to avoid thrashing.
          (void)topo->convertTopology(PAMI_COORD_TOPOLOGY);

          TRACE_FORMAT(" geom->size() %zd,topo->type() %d",geom->size(),topo->type())
          // for now, just bail-out if not a rectangle...
          // we could try to convert to rectangle, or see if subTopologyNthGlobal
          // produces a rectangle, etc.
          if(topo->type() != PAMI_COORD_TOPOLOGY)
          {
            if(fn) fn(context, clientdata, PAMI_SUCCESS);
            return PAMI_SUCCESS; // not really failure, just can't/won't do it.
          }

          // a more-exhaustive check for "comm-world" equivalent classroute...
          PAMI::Topology node_topo, local_topo;
          topo->subTopologyNthGlobal(&node_topo, 0);
          //	  if (node_topo.size() < 2) {
          //	    if (fn) fn(context, clientdata, PAMI_SUCCESS);
          //	    return PAMI_SUCCESS; // not really failure, just won't do it.
          //	  }
          topo->subTopologyLocalToMe(&local_topo);
          // assert(local_topo.size() > 0);
          bool master = (__global.mapping.task() == local_topo.index2Rank(0));
          /// \todo #warning must confirm that T==0 exists in rectangle...

          // since 'topo' is rectangle, 'node_topo' must also be.
          size_t tdim = __global.mapping.torusDims(); // T dim is after last torus dim
          CR_RECT_T rect1, rect2;
          node_topo.rectSeg(CR_RECT_LL(&rect1), CR_RECT_UR(&rect1));
          if(CR_COORD_DIM(CR_RECT_LL(&rect1),tdim) != 0)
          {
            // does not include T=0, so no MU optimization.
            if(fn) fn(context, clientdata, PAMI_SUCCESS);
            return PAMI_SUCCESS; // not really failure, just can't/won't do it.
          }
          _node_topo.rectSeg(CR_RECT_LL(&rect2), CR_RECT_UR(&rect2));
          if(__MUSPI_rect_compare(&rect1, &rect2) == 0)
          {
            // this topo includes all the same nodes as GEOMETRY_WORLD.
            geom->setKey(0,PAMI::Geometry::CKEY_MCAST_CLASSROUTEID, (void *)(0 + 1));
            geom->setKey(0,PAMI::Geometry::CKEY_MCOMB_CLASSROUTEID, (void *)(0 + 1));
            geom->setKey(0,PAMI::Geometry::CKEY_MSYNC_CLASSROUTEID, (void *)(0 + 1));
            if(fn) fn(context, clientdata, PAMI_SUCCESS);
            return PAMI_SUCCESS;
          }

          /// \todo Only the primary task of the node should actually alter MU DCRs...
          // Need to also ensure that no one uses this classroute until all nodes'
          // DCRs have been set. This requires a "global" barrier after this,
          // which means two barriers total in the case of geometry_create.

          // we could do even more checking and allow more geometries...

          unsigned geom_id = geom->comm();

          TRACE_FORMAT(" ----- geom_id:%d", geom_id)
          // from this point, we must have a valid context.
          PAMI_assert_alwaysf(context != NULL, "geomOptimize called on non-world w/o context");

          // This topology is part of the geometry, so we know it won't
          // "go away" after this method returns...
          cr_cookie *cookie;
          pami_result_t prc;
          prc = __global.heap_mm->memalign((void **)&cookie, 0, sizeof(*cookie));
          PAMI_assert_alwaysf(prc == PAMI_SUCCESS, "alloc of _excluded failed");
          cookie->thus = this;
          cookie->master = master;
          cookie->cb_done = (pami_callback_t)
          {
            fn, clientdata
          };
          cookie->topo = node_topo;
          cookie->rect = rect1;
          cookie->cr_mtx_mdl = &_cr_mtx_mdls[clientid][contextid];
          cookie->client = clientid;
          cookie->context = contextid;
          cookie->geom = geom;
          cookie->geom_id = geom_id;
          cookie->abuf[1] = geom_id;
          cookie->abuf[2] = ~geom_id;
          cookie->xfer.cmd.xfer_allreduce.sndbuf = (char *)&cookie->abuf[0];
          cookie->xfer.cmd.xfer_allreduce.stype = PAMI_TYPE_UNSIGNED_LONG_LONG;
          cookie->xfer.cmd.xfer_allreduce.stypecount = sizeof(cookie->abuf)/8;
          cookie->xfer.cmd.xfer_allreduce.rcvbuf = (char *)&cookie->bbuf[0];
          cookie->xfer.cmd.xfer_allreduce.rtype =  PAMI_TYPE_UNSIGNED_LONG_LONG;
          cookie->xfer.cmd.xfer_allreduce.rtypecount = sizeof(cookie->bbuf)/8;
          cookie->xfer.cmd.xfer_allreduce.op = PAMI_DATA_BAND;
          // offset rectangle by job corner...
          int d;
          for(d = 0; d < CR_NUM_DIMS; ++d)
          {
            CR_COORD_DIM(CR_RECT_LL(&cookie->rect), d) +=
            CR_COORD_DIM(CR_RECT_LL(&_communiv), d);
            CR_COORD_DIM(CR_RECT_UR(&cookie->rect), d) +=
            CR_COORD_DIM(CR_RECT_LL(&_communiv), d);
          }
          // tell geometry completion to wait for us...
          TRACE_FORMAT(" adding completion d:%d", d)
          geom->addCompletion();
          TRACE_FORMAT(" starting over d:%d", d)
          start_over(context, cookie, PAMI_SUCCESS);
          return PAMI_SUCCESS;
        }

        // a.k.a. Geometry_destroy... this happens to be immediate and local...
        // but, do we need the MU Coll device mutex? (_cr_mtx)
        inline pami_result_t geomDeoptimize(PAMI::Geometry::Common *geom)
        {
          TRACE_FN_ENTER();
          void *val = geom->getKey(0,PAMI::Geometry::CKEY_MCAST_CLASSROUTEID);
          if(val && val != PAMI_CR_CKEY_FAIL)
          {
            int id = (int)((uintptr_t)val & 0xffffffff) - 1;
            if(id != 0) // never free classroute 0 - a.k.a. comm-world
            {
              TRACE_FORMAT("Giving back CN classroute ID %d", id);
              int last = MUSPI_ReleaseClassrouteId(id, PAMI_MU_CR_SPI_VC,
                                                   NULL, &_cncrdata);
              if(last)
              {
                // This "frees" the id to others... don't want that.
                // rc = Kernel_DeallocateCollectiveClassRoute(id);
                // it is probably OK to leave the old one around,
                // else need to be sure what value we can set it to
                // and not have CNK think it is "free"...
                // rc = Kernel_SetEmptyCollectiveClassRoute(id);
                // for now, this code will just overwrite the bits next
                // time this id gets used.
              }
            }
          }
          val = geom->getKey(0,PAMI::Geometry::CKEY_MSYNC_CLASSROUTEID);
          if(val && val != PAMI_CR_CKEY_FAIL)
          {
            int id = (int)((uintptr_t)val & 0xffffffff) - 1;
            if(id != 0) // never free classroute 0 - a.k.a. comm-world
            {
              int last = MUSPI_ReleaseClassrouteId(id, PAMI_MU_CR_SPI_VC,
                                                   NULL, &_gicrdata);
              _inited[id] = 0;
              if(last)
              {
                // see Coll case above...
                // Since GI bits have conflicts with id+8, need to actually
                // make this classroute appear "empty" so it won't conflict
                // with "id-8"...
                (void) Kernel_SetEmptyGlobalInterruptClassRoute(id);

                TRACE_FORMAT("Clearing GI classroute bits for ID %d", id);
              }
            }
          }
          geom->setKey(0,PAMI::Geometry::CKEY_MCAST_CLASSROUTEID, NULL);
          geom->setKey(0,PAMI::Geometry::CKEY_MCOMB_CLASSROUTEID, NULL);
          geom->setKey(0,PAMI::Geometry::CKEY_MSYNC_CLASSROUTEID, NULL);
          TRACE_FN_EXIT();
          return PAMI_SUCCESS;
        }
        void getGITable(uint8_t** tbl)
        {
          *tbl = _inited;
        }
      private:
        static void start_over(pami_context_t ctx, void *cookie, pami_result_t result)
        {
          cr_cookie *crck = (cr_cookie *)cookie;
          if(result != PAMI_SUCCESS && result != PAMI_EAGAIN)
          {
            if(crck->cb_done.function) crck->cb_done.function(ctx, crck->cb_done.clientdata, result);
            // tell geometry completion "we're done"...
            crck->geom->rmCompletion(ctx, result);
            __global.heap_mm->free(cookie);
            return; // mutex error?!
          }
          crck->msync.cb_done = (pami_callback_t)
          {
            got_mutex, crck
          };
          crck->msync.roles = MUCR_mutex_model_t::LOCK_ROLE;
          pami_result_t rc = crck->cr_mtx_mdl->postMultisync(crck->mbuf, crck->client, crck->context, &crck->msync);
          if(rc != PAMI_SUCCESS)
          {
            // this frees 'cookie' if needed...
            // this also tells geom we're done.
            got_mutex(ctx, cookie, rc); // should this retry?
            return;
          }
        }

        inline void CheckGICRConflicts(uint32_t &mask, CR_RECT_T *rect)
        {
          uint32_t rc;
          ClassRoute_t cr;
          MUSPI_BuildNodeClassroute(&_refcomm, &_refroot, &_mycoord, rect,
                                    _map, _pri_dim, &cr);
          uint32_t msk = mask & ~CR_MATCH_RECT_FLAG; // just 0x0ffff ?
          int x = 0;
          while(msk)
          {
            if(msk & 1)
            {
              rc = Kernel_CheckGlobalInterruptClassRoute(x, &cr);
              // only care about conflicts with id+8 here...
              // but if there's an error now, we know it will
              // also be an error later so we might as well
              // just eliminate all GI classroutes and do the
              // CN ones only.
              if(rc != 0)
              {
                mask &= ~(1 << x);
              }
            }
            ++x;
            msk >>= 1;
          }
        }

        static void got_mutex(pami_context_t ctx, void *cookie, pami_result_t result)
        {
          TRACE_FN_ENTER();
          // we now "own" this device class instance, on the entire node.
          cr_cookie *crck = (cr_cookie *)cookie;
          if(result != PAMI_SUCCESS)
          {
            *crck->thus->_lowest_geom_id = 0xffffffff;
            if(crck->cb_done.function) crck->cb_done.function(ctx, crck->cb_done.clientdata, result);
            // tell geometry completion "we're done"...
            crck->geom->rmCompletion(ctx, result);
            crck->thus->release_mutex(ctx, cookie, PAMI_SUCCESS);
            __global.heap_mm->free(cookie);
            TRACE_FN_EXIT();
            return; // mutex error?! cleanup?
          }
          if(crck->geom_id > *crck->thus->_lowest_geom_id)
          {
            // we should not be trying right now, let another...
            // We should exit here or we may end up having an infinite recursion
            *crck->thus->_lowest_geom_id = 0xffffffff;
            if(crck->cb_done.function) crck->cb_done.function(ctx, crck->cb_done.clientdata, result);
            // tell geometry completion "we're done"...
            crck->geom->rmCompletion(ctx, result);
            crck->thus->release_mutex(ctx, cookie, PAMI_SUCCESS);
            __global.heap_mm->free(cookie);
            TRACE_FN_EXIT();
            return;
          }
          *crck->thus->_lowest_geom_id = crck->geom_id;

          // for now, this is only for true rectangles... (_nexcl == 0)
          /// \todo #warning _cncrdata (_gicrdata) must be in shared memory!
          uint32_t mask1 = 0, mask2 = 0;

          void *val = crck->geom->getKey(0,PAMI::Geometry::CKEY_MCAST_CLASSROUTEID);
          TRACE_FORMAT("MCAST CLASSROUTE %p, %d",val, val ? ((int)((uintptr_t)val & 0xffffffff) - 1) : -1);
          if(!val || val == PAMI_CR_CKEY_FAIL)
          {
            mask1 = MUSPI_GetClassrouteIds(PAMI_MU_CR_SPI_VC,
                                           &crck->rect, &crck->thus->_cncrdata);
          }
          val = crck->geom->getKey(0,PAMI::Geometry::CKEY_MSYNC_CLASSROUTEID);
          TRACE_FORMAT("MSYNC CLASSROUTE %p, %d",val, val ? ((int)((uintptr_t)val & 0xffffffff) - 1) : -1);
          if(!val || val == PAMI_CR_CKEY_FAIL)
          {
            mask2 = MUSPI_GetClassrouteIds(PAMI_MU_CR_SPI_VC,
                                           &crck->rect, &crck->thus->_gicrdata);
            crck->thus->CheckGICRConflicts(mask2, &crck->rect);
          }
          // is this true? can we be sure ALL nodes got the same result?
          // if so, and we are re-using existing classroute, then we can skip
          // the allreduce...
          crck->abuf[0] = ((uint64_t)mask1 & ~CR_MATCH_RECT_FLAG) |
                          (((uint64_t)mask2 & ~CR_MATCH_RECT_FLAG) << 32);
          TRACE_FORMAT("CLASSROUTE %#llX, %#X, %#X",(long long)crck->abuf[0], mask1, mask2);
#ifdef TRACE_CLASSROUTES
          if(crck->thus->_trace_cr && __global.mapping.task() == 0)
          {
            digraph = crck->thus->_digraph;
            print_prefix(&crck->thus->_trace_cw, &crck->rect);
          }
#endif // TRACE_CLASSROUTES
          // Now, must perform an "allreduce(&mask, 1, PAMI_LONGLONG, PAMI_DATA_BAND)"
          pami_algorithm_t algo0 = PAMI_ALGORITHM_NULL;
          pami_algorithm_t algo1 = PAMI_ALGORITHM_NULL;
          PAMI_Geometry_algorithms_query(crck->geom, PAMI_XFER_ALLREDUCE,
                                         &algo0, NULL, 1,
                                         &algo1, NULL, 1); 
          if(algo0==PAMI_ALGORITHM_NULL && algo1==PAMI_ALGORITHM_NULL)
          {
            fprintf(stderr, "PAMI_Geometry_algorithms_query() failed\n");
            cr_allreduce_done(ctx, cookie, PAMI_ERROR);
            TRACE_FN_EXIT();
            return;
          }
          crck->xfer.cb_done = cr_allreduce_done;
          crck->xfer.cookie = crck;
	  crck->xfer.algorithm = algo1==PAMI_ALGORITHM_NULL? algo0:algo1;
          // because of circular dependencies, can't dereference the context here,
          // so we must use the C API. Would be best to directly setup MU Coll
          // descriptor and inject, but this is currently on the new geom which
          // has no classroute, and if we use the parent then we have to do something
          // special to get the non-member nodes involved, since the parent members
          // that are not part of the sub-geometry don't even call analyze...
          TRACE_STRING("Allreduce");
          pami_result_t rc = PAMI_Collective(ctx, &crck->xfer);
          if(rc != PAMI_SUCCESS)
          {
            fprintf(stderr, "PAMI_Collective(ALLREDUCE) "
                    "failed to start (%d)\n", rc);
            // this frees 'cookie' if needed...
            // this also tells geom we're done.
            cr_allreduce_done(ctx, cookie, rc); // should this retry?
            TRACE_FN_EXIT();
            return;
          }
          TRACE_FN_EXIT();
        }

        static void cr_allreduce_done(pami_context_t ctx, void *cookie, pami_result_t result)
        {
          TRACE_FN_ENTER();
          static int retry_counter = 0;
          cr_cookie *crck = (cr_cookie *)cookie;
          if(result != PAMI_SUCCESS)
          {
            *crck->thus->_lowest_geom_id = 0xffffffff;
            if(crck->cb_done.function) crck->cb_done.function(ctx, crck->cb_done.clientdata, result);
            // tell geometry completion "we're done"...
            crck->geom->rmCompletion(ctx, result);
            crck->thus->release_mutex(ctx, cookie, PAMI_SUCCESS);
            __global.heap_mm->free(cookie); // don't do this if it retries...
            // release mutex?!
            TRACE_FN_EXIT();
            return; // need cleanup??? retry?
          }

          if(crck->bbuf[1] != crck->abuf[1] ||
             crck->bbuf[2] != crck->abuf[2])
          {
            TRACE_FORMAT("Start over %#llX != %#llX, %#llX != %#llX", 
                         (long long)crck->bbuf[1],(long long)crck->abuf[1],
                         (long long)crck->bbuf[2],(long long)crck->abuf[2]);
            // failed (collided), must reset and start over...
            // this probably won't happen, we'll just hang as there two nodes
            // (or more) waiting on different participant sets to complete.
            crck->thus->release_mutex(ctx, cookie, PAMI_SUCCESS);
            start_over(ctx, cookie, PAMI_EAGAIN);
            TRACE_FN_EXIT();
            return;
          }
          else if(!((crck->bbuf[0] >> 32) & 0x0000ffff) ||
                  !(crck->bbuf[0] & 0x0000ffff))
          {
            retry_counter++;
            if(retry_counter < RETRY_THRESHOLD)
            {
              crck->thus->release_mutex(ctx, cookie, PAMI_SUCCESS);
              start_over(ctx, cookie, PAMI_EAGAIN);
            }
            else
            {
               retry_counter = 0;
               *crck->thus->_lowest_geom_id = 0xffffffff;
               if(crck->cb_done.function) crck->cb_done.function(ctx, crck->cb_done.clientdata, result);
               // tell geometry completion "we're done"...
               crck->geom->rmCompletion(ctx, result);
               crck->thus->release_mutex(ctx, cookie, PAMI_SUCCESS);
               __global.heap_mm->free(cookie); // don't do this if it retries...
            }
            TRACE_FN_EXIT();
            return;
          }
          retry_counter = 0;

          memset(&crck->cr,0x00, sizeof(crck->cr));
          int irc = 0;
          irc += crck->thus->set_classroute(crck->bbuf[0] & 0x0000ffff, crck);


          irc += crck->thus->set_classroute_gi(ctx,(crck->bbuf[0] >> 32) & 0x0000ffff, crck);
          if(irc)
          {
            // message already printed...
            cr_gi_init_dummy_barrier_done(ctx, cookie, PAMI_ERROR);
            TRACE_FN_EXIT();
            return;
          }
          TRACE_FN_EXIT();
        }

        static void cr_gi_init_dummy_barrier_done(pami_context_t ctx, void *cookie, pami_result_t result)
        {
          TRACE_FN_ENTER();
          cr_cookie *crck = (cr_cookie *)cookie;
          pami_result_t rc = crck->geom->default_barrier(cr_barrier_done, cookie,
                                                         crck->context, ctx);
          if(rc != PAMI_SUCCESS)
          {
            fprintf(stderr, "Default barrier after skipping MUSPI_GIBarrierInitMU2 "
                    "failed to start (%d)\n", rc);
            cr_barrier_done(ctx, cookie, rc);
          }
          TRACE_FN_EXIT();
        }

        static void cr_gi_init_barrier_done(pami_context_t ctx, void *cookie, pami_result_t result)
        {
          cr_cookie *crck = (cr_cookie *)cookie;
          TRACE_FORMAT("MUSPI_GIBarrierInitMU2(%d, %ld) next",crck->id, (uint64_t) -1);
          int32_t irc = MUSPI_GIBarrierInitMU2(crck->id, -1);
          TRACE_FORMAT("MUSPI_GIBarrierInitMU2(%d, %ld) = %d",crck->id, crck->thus->_gi_init_to,irc);
          if(irc != 0)
          {
            fprintf(stderr, "MUSPI_GIBarrierInitMU2(%d, %ld) failed with %d\n",
                    crck->id, crck->thus->_gi_init_to, irc);
/* Unexpected error...what to do... try the regular default_barrier to recover from this...
   because that's probably what non-error nodes will do...can't hang any worse...
 
            cr_barrier_done(ctx, cookie, PAMI_ERROR);
            return;
 */
          }
          pami_result_t rc = crck->geom->default_barrier(cr_barrier_done, cookie,
                                                         crck->context, ctx);
          if(rc != PAMI_SUCCESS)
          {
            fprintf(stderr, "Default barrier after MUSPI_GIBarrierInitMU2 "
                    "failed to start (%d)\n", rc);
            cr_barrier_done(ctx, cookie, rc);
            return;
          }
        }

        static void cr_barrier_done(pami_context_t ctx, void *cookie, pami_result_t result)
        {
          TRACE_FN_ENTER();
          cr_cookie *crck = (cr_cookie *)cookie;
          if(crck->cb_done.function) crck->cb_done.function(ctx, crck->cb_done.clientdata, result);
#ifdef TRACE_CLASSROUTES
          if(crck->thus->_trace_cr && __global.mapping.task() == 0)
          {
            digraph = crck->thus->_digraph;
            print_suffix(&crck->thus->_trace_cw, NULL);
          }
#endif // TRACE_CLASSROUTES
          // tell geometry completion "we're done"...
          crck->geom->rmCompletion(ctx, result);
          __global.heap_mm->free(cookie);
          TRACE_FN_EXIT();
        }

        inline void release_mutex(pami_context_t ctx, void *cookie, pami_result_t result)
        {
          cr_cookie *crck = (cr_cookie *)cookie;
          crck->msync.cb_done = (pami_callback_t)
          {
            NULL, NULL
          };
          crck->msync.roles = MUCR_mutex_model_t::UNLOCK_ROLE;
          pami_result_t rc = crck->cr_mtx_mdl->postMultisync(crck->mbuf, crck->client, crck->context, &crck->msync);
          rc = rc;
          // no such thing as failure... nore is there any delay...
          // mutex is now unlocked, so we are "done" in every way that matters.
        }


        // Only holder of mutex calls this... thread safe in process.
        // GI only (extra allreduce)
        inline int set_classroute_gi(pami_context_t ctx, uint32_t mask, cr_cookie *crck)
        {
          TRACE_FN_ENTER();
          uint32_t id = ffs(mask);
          TRACE_FORMAT("mask %#X, id %u, cookie %p, classroute %p",mask,id,crck, &crck->cr);
          void **envpp = &_gicrdata;
          if(id)
          {
            --id; // ffs() returns bit# + 1
            TRACE_FORMAT("Set Class route ID id %u, cookie %p, classroute %p",id, crck, &crck->cr);
            int first = MUSPI_SetClassrouteId(id, PAMI_MU_CR_SPI_VC,
                                              &crck->rect, envpp);
            // Now allreduce 'first' because there are windows where the 
            // different ranks/node may not agree.
          crck->abuf[3] = (long long) first;
          crck->abuf[4] = (long long) !first;
          // Now, must perform an "allreduce(&mask, 1, PAMI_LONGLONG, PAMI_DATA_BAND)"
          // 
          // We reuse the same allreduce (with new cbdone) but only look at [3] & [4].
          // The previous id allreduce ignored [3] & [4].
          // 
          // If the two values match it's because someone didn't agree on 'first'
          // 
          // 1) all first = true , abuf[3]= true , abuf[4] = false --> bbuf[3] = true , bbuf[4] = false
          // 2) all first = false, abuf[3]= false, abuf[4] = true  --> bbuf[3] = false, bbuf[4] = true
          // 3) some first = true , abuf[3]= true , abuf[4] = false
          //    some first = false, abuf[3]= false, abuf[4] = true 
          //    result --> bbuf[3] = false, bbuf[4] = false

          crck->xfer.cb_done = cr_allreduce_gi_done;
          crck->xfer.cookie = crck;
          TRACE_FORMAT("Set GI Class route ID id %u, first %u (%zd,%zd), master %u", id, first,crck->abuf[3],crck->abuf[4], crck->master);
          pami_result_t rc = PAMI_Collective(ctx, &crck->xfer);
          if(rc != PAMI_SUCCESS)
          {
            fprintf(stderr, "PAMI_Collective(ALLREDUCE) "
                      "failed to start (%d)\n", rc);
            // this frees 'cookie' if needed...
            // this also tells geom we're done.
            cr_allreduce_gi_done(ctx, crck, rc); // should this retry?
            TRACE_FN_EXIT();
            return 0;
          }
          }

          TRACE_FN_EXIT();
          return 0;
        }
        static void cr_allreduce_gi_done(pami_context_t ctx, void *cookie, pami_result_t result)
        {
          TRACE_FN_ENTER();
          pami_event_function fn = cr_gi_init_dummy_barrier_done;

          cr_cookie *crck = (cr_cookie *)cookie;
          if(result != PAMI_SUCCESS)
          {
            *crck->thus->_lowest_geom_id = 0xffffffff;
            if(crck->cb_done.function) crck->cb_done.function(ctx, crck->cb_done.clientdata, result);
            // tell geometry completion "we're done"...
            crck->geom->rmCompletion(ctx, result);
            crck->thus->release_mutex(ctx, cookie, PAMI_SUCCESS);
            __global.heap_mm->free(cookie); // don't do this if it retries...
            // release mutex?!
            TRACE_FN_EXIT();
            return; // need cleanup??? retry?
          }

          if(crck->bbuf[3] == crck->bbuf[4])
          {
            TRACE_FORMAT("Failed first allreduce match %zu(%zu) != %zu(%zu)\n",crck->bbuf[3],crck->abuf[3],crck->bbuf[4],crck->abuf[4]);
            // failed , must reset and start over...
            // do we need to undo the failing class route?.... YESSSS \todo TODO
            // However, when this happens we will start over and hit the check
            // got_mutex about mask1==0 and mask2==0 allreduce values. Eventually
            // this will return after so many retries.
            // this probably won't happen, we'll just hang as there two nodes
            // (or more) waiting on different participant sets to complete.
            crck->thus->release_mutex(ctx, cookie, PAMI_SUCCESS);
            start_over(ctx, cookie, PAMI_EAGAIN);
            TRACE_FN_EXIT();
            return;
          }
          ClassRoute_t *cr = &crck->cr;
          long long first = crck->bbuf[3];
          uint32_t id = ffs((crck->bbuf[0] >> 32) & 0x0000ffff);
          if (id) --id; // ffs() returns bit# + 1
            // Note: need to detect if classroute was already programmed...
            if(crck->master && first)
            {
              int rc;
              TRACE_FORMAT("Taking GI classroute ID %d", id);
                rc = Kernel_SetGlobalInterruptClassRoute(id, cr);
                if(rc != 0)
                {
                  fprintf(stderr, "Kernel_SetGlobalInterruptClassRoute(%d, %08x) failed with %d\n",
                          id, (cr->input << 16) | cr->output, rc);
                  TRACE_FN_EXIT();
                  return;
                }
                rc = MUSPI_GIBarrierInitMU1(id);
                TRACE_FORMAT("MUSPI_GIBarrierInitMU1(%d) = %d",id,rc);
                if(rc != 0)
                {
                  fprintf(stderr, "MUSPI_GIBarrierInitMU1(%d) failed with %d\n",
                          id, rc);
                  TRACE_FN_EXIT();
                  return;
                }
                crck->id = id;
                fn = cr_gi_init_barrier_done;
            }
            crck->geom->setKey(0,PAMI::Geometry::CKEY_MSYNC_CLASSROUTEID, (void *)(id + 1));
#ifdef TRACE_CLASSROUTES
          if(crck->thus->_trace_cr)
          {
            digraph = crck->thus->_digraph;
            print_classroute(&crck->thus->_trace_cw, &crck->rect, &crck->thus->_mycoord,
                             cr, (int)__global.mapping.task());
          }
#endif // TRACE_CLASSROUTES
          // we got the answer we needed... no more trying...
          *crck->thus->_lowest_geom_id = 0xffffffff;
          crck->thus->release_mutex(ctx, cookie, PAMI_SUCCESS);
          TRACE_STRING("Another barrier");
          pami_result_t rc = crck->geom->default_barrier(fn, cookie,
                                                         crck->context, ctx);
          if(rc != PAMI_SUCCESS)
          {
            fprintf(stderr, "Default barrier after setting classroute ID "
                    "failed to start (%d)\n", rc);
            fn(ctx, cookie, rc);
            TRACE_FN_EXIT();
            return;
          }
          TRACE_FN_EXIT();
          return;
        }
        // Only holder of mutex calls this... thread safe in process.
        // Not GI classroutes
        inline int set_classroute(uint32_t mask, cr_cookie *crck)
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("mask %#X, cookie %p, classroute %p",mask,crck, &crck->cr);
          uint32_t id = ffs(mask);
          void **envpp = &_cncrdata;
          if(id)
          {
            --id; // ffs() returns bit# + 1
            int first = MUSPI_SetClassrouteId(id, PAMI_MU_CR_SPI_VC,
                                              &crck->rect, envpp);
            TRACE_FORMAT("Set Class route ID id %u, first %u, master %u", id, first, crck->master);
            // Note: need to detect if classroute was already programmed...
            if(crck->master && first)
            {
              if(!crck->cr.input)
              {
                MUSPI_BuildNodeClassroute(&_refcomm, &_refroot, &_mycoord, &crck->rect,
                                          _map, _pri_dim, &crck->cr);
                crck->cr.input |= BGQ_CLASS_INPUT_LINK_LOCAL;
                crck->cr.input |= PAMI_MU_CR_SPI_VC;
              }
              int rc;

              TRACE_FORMAT("Taking CN classroute ID %d", id);
                rc = Kernel_SetCollectiveClassRoute(id, &crck->cr);
                if(rc != 0)
                {
                  fprintf(stderr, "Kernel_SetCollectiveClassRoute(%d, %08x) failed with %d\n",
                          id, (crck->cr.input << 16) | crck->cr.output, rc);
                  TRACE_FN_EXIT();
                  return 1;
                }
            }
            crck->geom->setKey(0,PAMI::Geometry::CKEY_MCAST_CLASSROUTEID, (void *)(id + 1));
            crck->geom->setKey(0,PAMI::Geometry::CKEY_MCOMB_CLASSROUTEID, (void *)(id + 1));
          }
          TRACE_FN_EXIT();
          return 0;
        }

      public:

        // \brief Initialize for a Client
        // This occurs before any contexts are created.
        inline void initClient( size_t RmClientId );

        // \brief Get Per Process PAMI Max Number of Contexts For A Client
        inline size_t getPerProcessMaxPamiResources ( size_t RmClientId )
        {
          return _perProcessMaxPamiResources[RmClientId].numContexts;
        }

        // \brief Get Per Process PAMI Max Number of Contexts Across All Clients
        inline size_t getPerProcessMaxPamiResources ( )
        {
          TRACE_FN_ENTER();
          size_t rmClientId;
          size_t numClients       = _pamiRM.getNumClients();
          size_t totalNumContexts = 0;

          for( rmClientId=0; rmClientId<numClients; rmClientId++ )
            totalNumContexts += _perProcessMaxPamiResources[rmClientId].numContexts;

          TRACE_FORMAT("MU Context: getPerProcessMaxPamiResourcesAcrossAllClients = %zu",totalNumContexts);
          TRACE_FN_EXIT();
          return totalNumContexts;
        }

        // \brief Get Per Process PAMI Optimal Number of Contexts For A Client
        inline size_t getPerProcessOptimalPamiResources ( size_t RmClientId )
        {
          return _perProcessOptimalPamiResources[RmClientId].numContexts;
        }

        // \brief Get Per Process PAMI Optimal Number of Contexts Across All Clients
        inline size_t getPerProcessOptimalPamiResources ( )
        {
          TRACE_FN_ENTER();
          size_t rmClientId;
          size_t numClients       = _pamiRM.getNumClients();
          size_t totalNumContexts = 0;

          for( rmClientId=0; rmClientId<numClients; rmClientId++ )
            totalNumContexts += _perProcessOptimalPamiResources[rmClientId].numContexts;

          TRACE_FORMAT("MU Context: getPerProcessOptimalPamiResourcesAcrossAllClients = %zu",totalNumContexts);
          TRACE_FN_EXIT();
          return totalNumContexts;
        }

        inline uint32_t getGlobalCommAgentRecFifoId()
        {
          return _globalCommAgentRecFifoId;
        }

        inline bool isCommAgentActive()
        {
          return _commAgentActive;
        }

        inline void commAgent_AllocateWorkRequest( CommAgent_WorkRequest_t **workPtrAddress,
                                                   uint64_t                 *uniqueIDaddress )
        {
          int rc;
          do
          {
            rc = CommAgent_AllocateWorkRequest( _commAgentControl,
                                                workPtrAddress,
                                                uniqueIDaddress );
          } while(rc==EAGAIN);
          PAMI_assertf( rc == 0, "CommAgent work request allocation failed with rc=%d\n",rc );
        }

        inline int commAgent_RemoteGetPacing_SubmitWorkRequest( CommAgent_RemoteGetPacing_WorkRequest_t *workPtr )
        {
          return CommAgent_RemoteGetPacing_SubmitWorkRequest( _commAgentControl,
                                                              0, /* handle */
                                                              workPtr );
        }

        inline MUSPI_InjFifo_t * getGlobalCombiningInjFifoPtr ()
        {
          uint32_t subgroup = (_globalCombiningInjFifo.globalFifoIds[0] / BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP) - 64;
          uint32_t fifoId   = _globalCombiningInjFifo.globalFifoIds[0] % BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP;
          return &(_globalCombiningInjFifo.subgroups[subgroup]._injfifos[fifoId]);
        }

        inline uint32_t getGlobalCombiningInjFifoId ()
        {
          return _globalCombiningInjFifo.globalFifoIds[0];
        }

        inline char **getGlobalCombiningLookAsidePayloadBufferVAs()
        {
          return _globalCombiningInjFifo.lookAsidePayloadPtrs;
        }

        inline uint64_t *getGlobalCombiningLookAsidePayloadBufferPAs()
        {
          return _globalCombiningInjFifo.lookAsidePayloadPAs;
        }

        inline pami_event_function **getGlobalCombiningLookAsideCompletionFnPtrs()
        {
          return _globalCombiningInjFifo.lookAsideCompletionFnPtrs;
        }

        inline void                ***getGlobalCombiningLookAsideCompletionCookiePtrs()
        {
          return _globalCombiningInjFifo.lookAsideCompletionCookiePtrs;
        }

        /// \brief Get Global Base Address Table Id
        ///
        /// This base address table entry has a zero value stored in it so
        /// that the physical address can be used as a put offset or counter offset.
        ///
        inline uint32_t getGlobalBatId ()
        {
          return _globalBatIds[0];
        }

        /// \brief Get Shared Counter Base Address Table Id
        ///
        /// This base address table entry has an atomic address of a
        /// reception counter that is shared...the counter's value is
        /// not useful (ignored).  The counter offset in the descriptor
        /// can be zero.
        inline uint32_t getSharedCounterBatId ()
        {
          return _globalBatIds[1];
        }

        /// \brief Get Short Collective  Base Address Table Id
        ///
        inline uint32_t getShortCollectiveBatId ()
        {
          return _globalBatIds[2];
        }

        /// \brief Get Throughput Collective  Buffer Base Address Table Id
        ///
        inline uint32_t getThroughputCollectiveBufferBatId ()
        {
          return _globalBatIds[3];
        }

        /// \brief Get Throughput Collective  Counter Base Address Table Id
        ///
        inline uint32_t getThroughputCollectiveCounterBatId ()
        {
          return _globalBatIds[4];
        }

        /// \brief Get Collective network +Shmem Buffer Base Address Table Id
        ///
        inline uint32_t getCNShmemCollectiveBufferBatId ()
        {
          return _globalBatIds[5];
        }

        /// \brief Get Collective network +Shmem Counter Base Address Table Id
        ///
        inline uint32_t getCNShmemCollectiveCounterBatId ()
        {
          return _globalBatIds[6];
        }


        /// \brief Set Short Collective  Base Address Table Entry
        ///
        inline int32_t setShortCollectiveBatEntry ( uint64_t value )
        {
          uint32_t batSubgroup = ( _globalBatIds[2] / BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP ) - 64;
          uint8_t  batId       = _globalBatIds[2] % BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP;
          return MUSPI_SetBaseAddress ( &_globalBatSubGroups[batSubgroup],
                                        batId,
                                        value );
        }

        /// \brief Set Throughput Collective  Buffer Base Address Table Entry
        ///
        inline int32_t setThroughputCollectiveBufferBatEntry ( uint64_t value )
        {
          uint32_t batSubgroup = ( _globalBatIds[3] / BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP ) - 64;
          uint8_t  batId       = _globalBatIds[3] % BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP;
          return MUSPI_SetBaseAddress ( &_globalBatSubGroups[batSubgroup],
                                        batId,
                                        value );
        }

        /// \brief Set Throughput Collective  Counter Base Address Table Entry
        ///
        inline int32_t setThroughputCollectiveCounterBatEntry ( uint64_t value )
        {
          uint32_t batSubgroup = ( _globalBatIds[4] / BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP ) - 64;
          uint8_t  batId       = _globalBatIds[4] % BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP;
          return MUSPI_SetBaseAddress ( &_globalBatSubGroups[batSubgroup],
                                        batId,
                                        value );
        }

        /// \brief Set CN+Shmem Collective  Buffer Base Address Table Entry
        ///
        inline int32_t setCNShmemCollectiveBufferBatEntry ( uint64_t value )
        {
          uint32_t batSubgroup = ( _globalBatIds[5] / BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP ) - 64;
          uint8_t  batId       = _globalBatIds[5] % BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP;
          return MUSPI_SetBaseAddress ( &_globalBatSubGroups[batSubgroup],
                                        batId,
                                        value );
        }

        /// \brief Set CN+Shmem Collective  Counter Base Address Table Entry
        ///
        inline int32_t setCNShmemCollectiveCounterBatEntry ( uint64_t value )
        {
          uint32_t batSubgroup = ( _globalBatIds[6] / BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP ) - 64;
          uint8_t  batId       = _globalBatIds[6] % BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP;
          return MUSPI_SetBaseAddress ( &_globalBatSubGroups[batSubgroup],
                                        batId,
                                        value );
        }

        /// \brief Query the Number of Free BAT IDs Within A Context
        ///
        /// \retval  numFree
        inline uint32_t queryFreeBatIdsForContext( size_t    rmClientId,
                                                   size_t    contextOffset );

        /// \brief Allocate (reserve) BAT IDs Within A Context
        ///
        /// \retval  0  Success
        /// \retval  -1 Failed to allocate the specified number of BatIds.
        ///             No IDs were actually allocated.
        inline int32_t allocateBatIdsForContext( size_t    rmClientId,
                                                 size_t    contextOffset,
                                                 size_t    numBatIds,
                                                 uint16_t *globalBatIds );

        /// \brief Free (unreserve) BAT IDs Within A Context
        ///
        inline void freeBatIdsForContext( size_t    rmClientId,
                                          size_t    contextOffset,
                                          size_t    numBatIds,
                                          uint16_t *globalBatIds );

        inline int32_t setBatEntryForContext ( size_t    rmClientId,
                                               size_t    contextOffset,
                                               uint16_t  globalBatId,
                                               uint64_t  value );

        inline void initializeContexts( size_t rmClientId,
                                        size_t numContexts,
                                        PAMI::Device::Generic::Device * devices );
        inline void init(size_t rmClientId, size_t rmContextId, PAMI::Device::Generic::Device * devices);

        inline void getNumResourcesPerContext( size_t  rmClientId,
                                               size_t *numInjFifos,
                                               size_t *numRecFifos,
                                               size_t *numBatIds );

        inline pinInfoEntry_t *getPinInfo( size_t numInjFifos )
        {
          return &(_pinInfo[numInjFifos-1]);
        }

        inline pinInfoEntry_t *getRgetPinInfo()
        {
          TRACE_FN_ENTER();
          // Fill in the global rget fifo IDs.
          // Couldn't do this during global init without a barrier,
          // so doing it here instead, after the barrier.
          uint32_t i;
          for( i=0; i<numFifoPinIndices; i++ )
          {
            _rgetPinInfo->injFifoIds[i] =
            _globalRgetInjFifoIds[ _pinInfo[9].injFifoIds[i] ];
          }

          TRACE_FORMAT("MU Context: getRgetPinInfo: _rgetPinInfo->injFifoIds[] = %u,%u,%u,%u,%u,%u,%u,%u,%u,%u, %u,%u,%u,%u,%u,%u",_rgetPinInfo->injFifoIds[0],_rgetPinInfo->injFifoIds[1],_rgetPinInfo->injFifoIds[2],_rgetPinInfo->injFifoIds[3],_rgetPinInfo->injFifoIds[4],_rgetPinInfo->injFifoIds[5],_rgetPinInfo->injFifoIds[6],_rgetPinInfo->injFifoIds[7],_rgetPinInfo->injFifoIds[8],_rgetPinInfo->injFifoIds[9],_rgetPinInfo->injFifoIds[10],_rgetPinInfo->injFifoIds[11],_rgetPinInfo->injFifoIds[12],_rgetPinInfo->injFifoIds[13],_rgetPinInfo->injFifoIds[14],_rgetPinInfo->injFifoIds[15]);

          TRACE_FN_EXIT();
          return _rgetPinInfo;
        }

        /// \brief Get Pin Rec Fifo Handle for a Client
        ///
        /// The specified client ID is mapped to a pointer
        /// to an internal data structure.  This is returned to the caller, who
        /// is likely a context init routine, so it may be cached.
        /// During runtime pinFifo, the context will pass this handle into
        /// getPinRecFifo() so it can do a quick array lookup in this
        /// internal structure.  This is all to save a few cycles by doing
        /// the mapping at init time rather than at runtime.
        inline void *getPinRecFifoHandle ( size_t rmClientId )
        {
          return(void*)&_clientResources[rmClientId];
        }

        /// \brief Get Pin Rec Fifo Handle for a Client and Context
        ///
        /// The specified client ID and context offset are mapped to a pointer
        /// to an internal data structure.  This is returned to the caller, who
        /// is likely a context init routine, so it may be cached.
        /// During runtime pinFifo, the context will pass this handle into
        /// getPinRecFifo() so it can do a quick array lookup in this
        /// internal structure.  This is all to save a few cycles by doing
        /// the mapping at init time rather than at runtime.
        inline void *getPinRecFifoHandle ( size_t rmClientId, 
                                           size_t contextOffset )
        {
          return(void*)&_clientResources[rmClientId].pinRecFifo[contextOffset*_tSize];
        }

        /// \brief Get Rec Fifo Number, given a handle for a client and context.
        ///
        /// \param[in]  handle  Pointer to the recFifo info for a given client
        ///                     and context.  Obtained from calling 
        ///                     getPinRecFifoHandle ( size_t rmClientId,
        ///  				              size_t contextOffset )
        /// \param[in]  t       The destination's T coord.
        inline uint16_t getPinRecFifo( void *handle,
                                       size_t t )
        {
          uint16_t *pinRecFifoPtr = (uint16_t*)handle;
          return pinRecFifoPtr[t]; 
        }

        /// \brief Get Rec Fifo Number, given a handle for a client.
        ///
        /// \param[in]  handle  Pointer to the recFifo info for a given client.
        ///                     Obtained from calling 
        ///                     getPinRecFifoHandle ( size_t rmClientId )
        /// \param[in]  contextOffset  The destination's context.
        /// \param[in]  t              The destination's T coord.
        inline uint16_t getPinRecFifo( void  *handle,
                                       size_t contextOffset,
                                       size_t t )
        {
          clientResources_t *clientResourcesPtr = (clientResources_t*)handle;
          uint16_t *pinRecFifoPtr = clientResourcesPtr->pinRecFifoPtrs[contextOffset];
          return pinRecFifoPtr[t];
        }


        inline uint16_t getPinBatId( size_t rmClientId,
                                     size_t contextOffset,
                                     size_t t,
                                     uint16_t globalBatId )
        {
          uint16_t myRelativeGlobalBatId = globalBatId - _clientResources[rmClientId].pinBatId[(contextOffset*_tSize) + _myT];
          return _clientResources[rmClientId].pinBatId[(contextOffset*_tSize) + t] + myRelativeGlobalBatId;
        }

        /// \brief Get the Core Affinity for a Context
        ///
        /// \param[in]  rmClientId  Resource Manager client Id
        /// \param[in]  contextOffset  The context Id, relative to the client
        ///
        /// \retval  coreId  The core number that the specified context is affiliated with.
        ///
        inline uint32_t getAffinity( size_t rmClientId,
                                     size_t contextOffset )
        {
          return _clientResources[rmClientId].startingSubgroupIds[contextOffset] /
          BGQ_MU_NUM_FIFO_SUBGROUPS;
        }


        // \brief Get Interrupt Mask
        //
        // Construct a interrupt bitmask indicating which interrupts to clear for the
        // specified context.  These indicate which fifos in the group are for
        // this context.
        // 64 bits:
        // - Bits  0 through 31 clear injection fifo threshold crossing
        // - Bits 32 through 47 clear reception fifo threshold crossing
        // - Bits 48 through 63 clear reception fifo packet arrival
        //
        inline uint64_t getInterruptMask ( size_t rmClientId,
                                           size_t contextOffset );


        inline MUSPI_RecFifoSubGroup_t *getRecFifoSubgroup ( size_t rmClientId,
                                                             size_t contextOffset )
        {
          return &(_clientResources[rmClientId].recResources[contextOffset].subgroups[0]);
        }

        inline void getInjFifosForContext( size_t            rmClientId,
                                           size_t            contextOffset,
                                           size_t            numInjFifos,
                                           MUSPI_InjFifo_t **injFifoPtrs,
                                           uint32_t         *globalFifoIds );

        inline void getRecFifosForContext( size_t            rmClientId,
                                           size_t            contextOffset,
                                           size_t            numRecFifos,
                                           MUSPI_RecFifo_t **recFifoPtrs,
                                           uint32_t         *globalFifoIds );

        inline char **getLookAsidePayloadBufferVAs( size_t rmClientId,
                                                    size_t contextOffset )
        {
          return _clientResources[rmClientId].injResources[contextOffset].lookAsidePayloadPtrs;
        }

        inline uint64_t *getLookAsidePayloadBufferPAs( size_t rmClientId,
                                                       size_t contextOffset )
        {
          return _clientResources[rmClientId].injResources[contextOffset].lookAsidePayloadPAs;
        }

        inline pami_event_function **getLookAsideCompletionFnPtrs( size_t rmClientId,
                                                                   size_t contextOffset )
        {
          return _clientResources[rmClientId].injResources[contextOffset].lookAsideCompletionFnPtrs;
        }

        inline void                ***getLookAsideCompletionCookiePtrs( size_t rmClientId,
                                                                        size_t contextOffset )
        {
          return _clientResources[rmClientId].injResources[contextOffset].lookAsideCompletionCookiePtrs;
        }
        inline size_t getMaxNumDescInInjFifo()
        {
          return( _pamiRM.getInjFifoSize() / sizeof( MUHWI_Descriptor_t ) ) -1;
        }

        inline uint32_t *getRgetInjFifoIds()
        {
          return _globalRgetInjFifoIds;
        }

      private:
        inline uint16_t chooseFifo (uint64_t dimMinusCutoff,
                                    uint64_t dimPlusCutoff,
                                    size_t   sourceCoordInDim,
                                    size_t   destCoordInDim,
                                    bool     isTorus);
        inline uint16_t pinFifo( size_t task );
        inline void initFifoPin();
        inline void calculatePerCoreMUResourcesBasedOnAvailability();
        inline void calculatePerCoreMUResourcesBasedOnConfig();
        inline void calculatePerCoreMUResources();
        inline void calculatePerCorePerProcessMUResources();
        inline void dividePAMIResourceAmongClients( pamiResources_t &resourceValue,
                                                    size_t           numClients,
                                                    pamiResources_t *clientResource );
        inline void divideMUResourceAmongClients( muResources_t &resourceValue,
                                                  size_t         numClients,
                                                  muResources_t *clientResource );
        inline void calculatePerProcessMaxPamiResources();
        inline void calculatePerProcessOptimalPamiResources();
        inline void calculatePerCorePerProcessPerClientMUResources();
        inline void setupSharedMemory();
        inline void allocateMemory( const char *key,
                                    void ** memptr,
                                    size_t  alignment,
                                    size_t  bytes );
        inline uint32_t setupInjFifos( uint32_t startingSubgroup,
                                       uint32_t endingSubgroup,
                                       size_t   numFifos,
                                       size_t   fifoSize,
                                       Kernel_InjFifoAttributes_t  *fifoAttr,
                                       const char *key,
                                       bool                         enableInterrupts,
                                       MUSPI_InjFifoSubGroup_t    **subgroups,
                                       char                      ***fifoPtrs,
                                       uint32_t                   **globalFifoIds );
        inline uint32_t setupRecFifos( uint32_t startingSubgroup,
                                       uint32_t endingSubgroup,
                                       size_t   numFifos,
                                       size_t   fifoSize,
                                       Kernel_RecFifoAttributes_t  *fifoAttr,
                                       const char *key,
                                       MUSPI_RecFifoSubGroup_t    **subgroups,
                                       char                      ***fifoPtrs,
                                       uint32_t                   **globalFifoIds );
        inline uint32_t setupBatIds(
                                   uint32_t                           startingSubgroup,
                                   uint32_t                           endingSubgroup,
                                   uint32_t                           numBatIds,
                                   const char *key,
                                   MUSPI_BaseAddressTableSubGroup_t **subgroups,
                                   uint32_t                         **globalBatIds);
        inline void allocateGlobalResources();
        inline void allocateGlobalInjFifos();
        inline void allocateGlobalBaseAddressTableEntries();
        inline void allocateGlobalRecFifos();
        inline void allocateGlobalCommAgent();
        inline void allocateContextResources( size_t rmClientId,
                                              size_t contextOffset );
        inline void allocateLookasideResources(
                                              size_t                   numInjFifos,
                                              size_t                   injFifoSize,
                                              char                  ***lookAsidePayloadPtrs,
                                              Kernel_MemoryRegion_t  **lookAsidePayloadMemoryRegions,
                                              uint64_t               **lookAsidePayloadPAs,
                                              pami_event_function   ***lookAsideCompletionFnPtrs,
                                              void                 ****lookAsideCompletionCookiePtrs);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Member data:
        ///
        /////////////////////////////////////////////////////////////////////////

        PAMI::ResourceManager &_pamiRM;
        PAMI::Mapping         &_mapping;
        PAMI::BgqJobPersonality  &_pers;
        MUCR_mutex_t _cr_mtx; // each context has MUCR_mutex_model_t pointing to this
        MUCR_mutex_model_t **_cr_mtx_mdls;
        PAMI::Topology _node_topo;
        CR_RECT_T _refcomm;
        CR_COORD_T _refroot;
        CR_COORD_T _mycoord;
        int _pri_dim;
        int _map[CR_NUM_DIMS];
        CR_RECT_T _communiv;
        size_t _np;
        CR_RECT_T _commworld;
        CR_COORD_T *_excluded;
        int _nexcl;
#ifdef TRACE_CLASSROUTES
        size_t _trace_cr;
        commworld_t _trace_cw;
        unsigned _digraph;
#endif // TRACE_CLASSROUTES
        uint64_t _gi_init_to;
        unsigned *_lowest_geom_id; // in shared memory! (protected by _cr_mtx)
        void *_cncrdata; // used by MUSPI routines to keep track of
        // classroute assignments - persistent!
        void *_gicrdata; // (ditto)
        uint8_t             *_inited; // shared GIBarrier inited flag table
        // _pinInfo is an array of entries.  Each entry has info for when there
        // are a specific number of inj fifos in the context.
        // There are 10 entries, for 1 fifo, 2 fifos, ..., 10 fifos.
        pinInfoEntry_t *_pinInfo;

        // _rgetPinInfo is a single entry set of global rget inj fifo Ids.
        // The first 10 ids are for torus transfers.
        // The last 6 are for local transfers.
        pinInfoEntry_t *_rgetPinInfo;

        size_t  _tSize;
        size_t  _myT;
        bool    _isTorusA;
        bool    _isTorusB;
        bool    _isTorusC;
        bool    _isTorusD;
        bool    _isTorusE;

        // MU Resources
        muResources_t _perCoreMUResourcesBasedOnAvailability;
        muResources_t _perCoreMUResourcesBasedOnConfig;
        muResources_t _perCoreMUResources;
        muResources_t _perCorePerProcessMUResources;

        // Per client resource arrays
        pamiResources_t   *_perProcessMaxPamiResources;
        pamiResources_t   *_perProcessOptimalPamiResources;
        muResources_t     *_perCorePerProcessPerClientMUResources;
        pamiResources_t   *_perCorePerProcessPamiResources;
        muResources_t     *_perContextMUResources;
        clientResources_t *_clientResources;

        // Shared Memory Manager
        PAMI::Memory::GenMemoryManager        _mm;
        size_t                             _memSize;

        // Node-wide resources
        unsigned int                       _calculateSizeOnly;      // 1 = Calculate _memSize
        // 0 = Allocate _memSize
        unsigned int                       _allocateOnly;           // 1 = Allocate, no init
        // 0 = Allocate and init

        MUSPI_InjFifoSubGroup_t           *_globalRgetInjSubGroups; // Rget Subgroups 64 and 65.
        char                             **_globalRgetInjFifoPtrs;  // RgetPointers to fifos.
        uint32_t                          *_globalRgetInjFifoIds;   // Rget Ids.

        injFifoResources_t                 _globalCombiningInjFifo; // Global Inj Fifo (for
                                                                    // combining collectives)
                                                                    // in subgroup 64 or 65.

        MUSPI_BaseAddressTableSubGroup_t  *_globalBatSubGroups;     // BAT Subgroups 64 and 64.
        uint32_t                          *_globalBatIds;           // BAT ids.

        CommAgent_Control_t                _commAgentControl;       // Comm Agent control struct.
        bool                               _commAgentActive;        // Indicates whether the comm
        // agent is active.
        uint32_t                           _globalCommAgentRecFifoId; // Comm Agent's global
        // rec fifo ID.

        // Process-wide resources
        MUSPI_RecFifoSubGroup_t           *_globalRecSubGroups;     // Subgroups for cores 0..15.

        // Cutoff registers for fifo pinning on a torus.
        uint64_t                           _AminusCutoff;
        uint64_t                           _AplusCutoff;
        uint64_t                           _BminusCutoff;
        uint64_t                           _BplusCutoff;
        uint64_t                           _CminusCutoff;
        uint64_t                           _CplusCutoff;
        uint64_t                           _DminusCutoff;
        uint64_t                           _DplusCutoff;
        uint64_t                           _EminusCutoff;
        uint64_t                           _EplusCutoff;

      }; // ResourceManager class
    }; // MU     namespace
  };   // Device namespace
};     // PAMI   namespace


// \brief Initialize for a Client
// This occurs before any contexts are created.
void PAMI::Device::MU::ResourceManager::initClient( size_t RmClientId )
{
  // If this is the first client, perform init.
  if( _perProcessMaxPamiResources == NULL ) /* First client to initialize? */
  {
    // Calculate the following for each client
    calculatePerProcessMaxPamiResources();
    calculatePerProcessOptimalPamiResources();
    calculatePerCorePerProcessPerClientMUResources();
  }
}


/// \brief Choose Fifo
///
/// Select an injection fifo (0..9) based on inputs.
///
uint16_t PAMI::Device::MU::ResourceManager::chooseFifo (uint64_t dimMinusCutoff,
                                                        uint64_t dimPlusCutoff,
                                                        size_t   sourceCoordInDim,
                                                        size_t   destCoordInDim,
                                                        bool     isTorus)
{
  // If it is a torus, choose the direction that is within the number of hops
  // allowed by the configuration set up for the hardware cutoffs.
  if( isTorus )
  {
    if( sourceCoordInDim < destCoordInDim )
    {
      if( destCoordInDim <= dimPlusCutoff )
        return 0;  // Use +.
      else
        return 1;  // Use -.
    }
    if( destCoordInDim < sourceCoordInDim )
    {
      if( dimMinusCutoff <= destCoordInDim )
        return 1;  // Use -.
      else
        return 0;  // Use +.
    }
    PAMI_assert_always(0);
  }
  // It is a mesh.  Go in natural direction.
  else return( destCoordInDim > sourceCoordInDim ) ? 0 : 1;

} // End: chooseFifo()


/// \brief Pin Fifo
///
/// For the specified task, determine the optimal injection fifo to use
/// to communicate with that task.  This is based on there being an
/// optimal number of injection fifos.
///
uint16_t PAMI::Device::MU::ResourceManager::pinFifo( size_t task )
{
  TRACE_FN_ENTER();
  // Get our torus coords
  size_t ourA = _mapping.a();
  size_t ourB = _mapping.b();
  size_t ourC = _mapping.c();
  size_t ourD = _mapping.d();
  size_t ourE = _mapping.e();

  // Get the destination task's torus coords (addr[0..4]) and t coordinate (addr[5]).
  size_t addr[BGQ_TDIMS + BGQ_LDIMS];
  _mapping.task2global( task, addr );

  TRACE_FORMAT("MU ResourceManager: pinFifo: task=%zu,destA=%zu, B=%zu, C=%zu, D=%zu, E=%zu",task,addr[0],addr[1],addr[2],addr[3],addr[4]);

  // Calculate the signed number of hops between our task and the destination task
  // in each dimension
  ssize_t dA = addr[0] - ourA;
  ssize_t dB = addr[1] - ourB;
  ssize_t dC = addr[2] - ourC;
  ssize_t dD = addr[3] - ourD;
  ssize_t dE = addr[4] - ourE;

  TRACE_FORMAT("MU ResourceManager: pinFifo: ourA=%zu,ourB=%zu,ourC=%zu,ourD=%zu,ourE=%zu, dA=%zd,dB=%zd,dC=%zd,dD=%zd,dE=%zd,t=%zd",ourA,ourB,ourC,ourD,ourE,dA,dB,dC,dD,dE,addr[5]);

  // If local, select the fifo based on the T coordinate.
  if( dA == 0 && dB == 0 && dC == 0 && dD == 0 && dE == 0 )
  {
    TRACE_FN_EXIT();
    return( 10 + (addr[5] % 6) );
  }

  // If communicating only along the A dimension, select either the A- or A+ fifo
  if( dB == 0 && dC == 0 && dD == 0 && dE == 0 )
  {
    TRACE_FN_EXIT();
    return( chooseFifo (_AminusCutoff,
                        _AplusCutoff,
                        ourA,
                        addr[0],
                        _isTorusA) ? 0 : 1 ); // Return A- if chooseFifo
    // returns 1, else return A+.
  }

  // If communicating only along the B dimension, select either the B- or B+ fifo
  if( dA == 0 && dC == 0 && dD == 0 && dE == 0 )
  {
    TRACE_FN_EXIT();
    return( chooseFifo (_BminusCutoff,
                        _BplusCutoff,
                        ourB,
                        addr[1],
                        _isTorusB) ? 2 : 3 ); // Return B- if chooseFifo
                                              // returns 1, else return B+.
  }

  // If communicating only along the C dimension, select either the C- or C+ fifo
  if( dA == 0 && dB == 0 && dD == 0 && dE == 0 )
  {
    TRACE_FN_EXIT();
    return( chooseFifo (_CminusCutoff,
                        _CplusCutoff,
                        ourC,
                        addr[2],
                        _isTorusC) ? 4 : 5 ); // Return C- if chooseFifo
    // returns 1, else return C+.
  }

  // If communicating only along the D dimension, select either the D- or D+ fifo
  if( dA == 0 && dB == 0 && dC == 0 && dE == 0 )
  {
    TRACE_FN_EXIT();
    return( chooseFifo (_DminusCutoff,
                        _DplusCutoff,
                        ourD,
                        addr[3],
                        _isTorusD) ? 6 : 7 ); // Return D- if chooseFifo
    // returns 1, else return D+.
  }

  // If communicating only along the E dimension, select either the E- or E+ fifo
  if( dA == 0 && dB == 0 && dC == 0 && dD == 0 )
  {
    TRACE_FN_EXIT();
    return( chooseFifo (_EminusCutoff,
                        _EplusCutoff,
                        ourE,
                        addr[4],
                        _isTorusE) ? 8 : 9 ); // Return E- if chooseFifo
    // returns 1, else return E+.
  }

  // Communicating along a diagonal or to ourself.
  // Sum the distance between our node and the destination node, and map that
  // number to one of the fifos.  This is essentially a random fifo that is
  // chosen.
  uint16_t loc = abs_x(dA) + abs_x(dB) + abs_x(dC) + abs_x(dD) + abs_x(dE);
  TRACE_FN_EXIT();
  return loc % numTorusDirections;
} // End: pinFifo()




/// \brief Init Fifo Pin
///
/// For each task, calculate the optimal injection fifo to use when sending to
/// that task, and cache it in the mapcache.  This will be used later during
/// fifo pinning.
///
/// The high-order bit of the A,B,C, and D coordinate in each map cache entry
/// will house this optimal fifo number.  This gives us 4 bits for a total of
/// 16 possible values.
/// - Values 0-9 for torus transfers, indicating which of the 10 inj fifos to use.
/// - Values 10-15 for local transfers, indicating which of 6 inj fifos to use
///   (ran out of bits, or it would also be 10 values).
///
/// In addition to calculating the above "fifoPin" value for the mapcache,
/// adjust the pinInjFifoMap array based on our T coordinate.  Refer to the
/// details in the comments where each of these arrays is delcared.
///
void PAMI::Device::MU::ResourceManager::initFifoPin()
{
  TRACE_FN_ENTER();
  size_t numTasks = _mapping.size();
  size_t i;
  size_t startingAdjustmentIndex;
  size_t tcoord = _mapping.t();
  size_t task;
  size_t numFifos;

  // Get the torus cutoff register containing the cutoffs for each 
  // dimension and direction.  For example, A- cutoff is always >=0, 
  // A+ cutoff is always <= sizeOfAdimension-1.  It contains the
  // last dest coord in that direction to go.  If dest coord is
  // outside that limit, go in the other direction.  The cutoffs are
  // set such that ties are broken in the minus direction if the sum
  // of the source coordinates is odd.
  uint64_t dcr_value;
  dcr_value = DCRReadUser(ND_500_DCR(CTRL_CUTOFFS));
  _AminusCutoff = ND_500_DCR__CTRL_CUTOFFS__A_MINUS_get( dcr_value );
  _AplusCutoff  = ND_500_DCR__CTRL_CUTOFFS__A_PLUS_get ( dcr_value );
  _BminusCutoff = ND_500_DCR__CTRL_CUTOFFS__B_MINUS_get( dcr_value );
  _BplusCutoff  = ND_500_DCR__CTRL_CUTOFFS__B_PLUS_get ( dcr_value );
  _CminusCutoff = ND_500_DCR__CTRL_CUTOFFS__C_MINUS_get( dcr_value );
  _CplusCutoff  = ND_500_DCR__CTRL_CUTOFFS__C_PLUS_get ( dcr_value );
  _DminusCutoff = ND_500_DCR__CTRL_CUTOFFS__D_MINUS_get( dcr_value );
  _DplusCutoff  = ND_500_DCR__CTRL_CUTOFFS__D_PLUS_get ( dcr_value );
  _EminusCutoff = ND_500_DCR__CTRL_CUTOFFS__E_MINUS_get( dcr_value );
  _EplusCutoff  = ND_500_DCR__CTRL_CUTOFFS__E_PLUS_get ( dcr_value );

  TRACE_FORMAT("Cutoffs: A: %zu %zu, B: %zu %zu, C: %zu %zu, D: %zu %zu, E: %zu %zu",_AminusCutoff,_AplusCutoff,_BminusCutoff,_BplusCutoff,_CminusCutoff,_CplusCutoff,_DminusCutoff,_DplusCutoff,_EminusCutoff,_EplusCutoff);


  _isTorusA    = _pers.isTorusA();
  _isTorusB    = _pers.isTorusB();
  _isTorusC    = _pers.isTorusC();
  _isTorusD    = _pers.isTorusD();
  _isTorusE    = _pers.isTorusE();

  TRACE_FORMAT("MU ResourceManager:: initFifoPin:  aT=%d,bT=%d,cT=%d,dT=%d,eT=%d",_isTorusA,_isTorusB,_isTorusC,_isTorusD,_isTorusE);

  for( task=0; task<numTasks; task++ )
  {
    uint16_t fifo = pinFifo( task );
    PAMI_assert( fifo < 16 );
    TRACE_FORMAT("MU ResourceManager: initFifoPin: s(%zu,%zu,%zu,%zu,%zu), task=%zu, fifo=%u",_pers.aCoord(),_pers.bCoord(),_pers.cCoord(),_pers.dCoord(),_pers.eCoord(),task,fifo);

    _mapping.setFifoPin( task, fifo );
  }

  // Copy the pinInfo arrary into this process' storage.  These contain
  // initial fifo pin values that will be modified later.
  pami_result_t rc;
  rc = __global.heap_mm->memalign((void **)&_pinInfo, 64,
                                  numTorusDirections * sizeof(pinInfoEntry_t));
  PAMI_assertf(rc == PAMI_SUCCESS, "The heap is full.\n" );

  memcpy ( _pinInfo, pinInfo, numTorusDirections * sizeof(pinInfoEntry_t) );

  // Adjust the local injFifoIds (array elements 10-15) based on our T coordinate.
  // This is an attempt to spread the local traffic among all of the inj fifos.
  // Refer to comments where pinInfo is declared.
  for( numFifos=1; numFifos <= numTorusDirections; numFifos++ )
  {
    for( i=10; i<16; i++ )
    {
      _pinInfo[numFifos-1].injFifoIds[i] =
      ( _pinInfo[numFifos-1].injFifoIds[i] + ( 6*tcoord ) ) % numFifos;
    }
  }
  // Adjust the torus injFifoIds so the directions that don't have
  // their own inj fifo map to one of the other fifos based on our T coordinate.
  // Refer to comments where pinInjFifoMap is declared.
  TRACE_STRING("MU ResourceManager: initFifoPin: Adjusted pinInjFifoMap values:");
  for( numFifos=1; numFifos <= numTorusDirections; numFifos++ )
  {
    // Only adjust the directions that are unbalanced.
    // For example, if there are 4 inj fifos, the first 8 values are
    // balanced (0,1,2,3,0,1,2,3) but the last 2 are not, and need to be
    // adjusted.  T=0 will set them to (0,1), T=1 will set them to (2,3),
    // T=2 will set them to (0,1), and so on.  In this case, the
    // startingAdjustmentIndex is 8, so that only indices 8 and 9 are adjusted.
    startingAdjustmentIndex =
    numTorusDirections -
    ( numTorusDirections -
      ( (numTorusDirections / numFifos) * numFifos ) );
    for( i=startingAdjustmentIndex; i<numTorusDirections; i++ )
    {
      // Note:  numTorusDirections-startingAdjustmentIndex is the number of overstressed fifos.
      _pinInfo[numFifos-1].injFifoIds[i] =
      ( _pinInfo[numFifos-1].injFifoIds[i] +
        ( (numTorusDirections-startingAdjustmentIndex)*tcoord ) ) % numFifos;
    }
    TRACE_FORMAT("For %zu fifos: %u,%u,%u,%u,%u,%u,%u,%u,%u,%u, %u,%u,%u,%u,%u,%u",
           numFifos,
           _pinInfo[numFifos-1].injFifoIds[0],
           _pinInfo[numFifos-1].injFifoIds[1],
           _pinInfo[numFifos-1].injFifoIds[2],
           _pinInfo[numFifos-1].injFifoIds[3],
           _pinInfo[numFifos-1].injFifoIds[4],
           _pinInfo[numFifos-1].injFifoIds[5],
           _pinInfo[numFifos-1].injFifoIds[6],
           _pinInfo[numFifos-1].injFifoIds[7],
           _pinInfo[numFifos-1].injFifoIds[8],
           _pinInfo[numFifos-1].injFifoIds[9],
           _pinInfo[numFifos-1].injFifoIds[10],
           _pinInfo[numFifos-1].injFifoIds[11],
           _pinInfo[numFifos-1].injFifoIds[12],
           _pinInfo[numFifos-1].injFifoIds[13],
           _pinInfo[numFifos-1].injFifoIds[14],
           _pinInfo[numFifos-1].injFifoIds[15]);
  }
  TRACE_FN_EXIT();
} // End: initFifoPin()


// \brief Calculate Per Core MU Resources Based On Availability
//
// Determine how many MU resources have already been consumed by the SPI user.
// The core with the minimum number of available MU resources is our model.
// Calculate the available MU resources per core:
// - number of inj fifos available per core
// - number of rec fifos available per core
void PAMI::Device::MU::ResourceManager::calculatePerCoreMUResourcesBasedOnAvailability()
{
  TRACE_FN_ENTER();
  size_t minFreeInjFifosPerCore = BGQ_MU_NUM_INJ_FIFOS_PER_GROUP;
  size_t minFreeRecFifosPerCore = BGQ_MU_NUM_REC_FIFOS_PER_GROUP;
  size_t minFreeBatIdsPerCore   = BGQ_MU_NUM_DATA_COUNTERS_PER_GROUP;
  size_t i;
  int32_t rc;
  uint32_t injFifoIds[BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP];
  uint32_t recFifoIds[BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP];
  uint32_t batIds[BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP];

  // Examine each core's MU resources (not 17th core), and determine the
  // minimum over all of the cores.
  uint32_t group;        // Global group number
  uint32_t subgroup = 0; // Global subgroup number
  for(group=0; group<(BGQ_MU_NUM_FIFO_GROUPS-1); group++)
  {
    size_t numFreeInjFifosInGroup = 0;
    size_t numFreeRecFifosInGroup = 0;
    size_t numFreeBatIdsInGroup   = 0;

    // Loop through the subgroups in this group
    for(i=0; i<BGQ_MU_NUM_FIFO_SUBGROUPS; i++)
    {
      uint32_t numFreeResourcesInSubgroup;

      rc = Kernel_QueryInjFifos( subgroup,
                                 &numFreeResourcesInSubgroup,
                                 injFifoIds );
      PAMI_assertf( rc == 0, "Kernel_QueryInjFifos failed with rc=%d.\n",rc );

      numFreeInjFifosInGroup += numFreeResourcesInSubgroup;

      rc = Kernel_QueryRecFifos( subgroup,
                                 &numFreeResourcesInSubgroup,
                                 recFifoIds );
      PAMI_assertf( rc == 0, "Kernel_QueryRecFifos failed with rc=%d.\n",rc);

      numFreeRecFifosInGroup += numFreeResourcesInSubgroup;

      rc = Kernel_QueryBaseAddressTable ( subgroup,
                                          &numFreeResourcesInSubgroup,
                                          batIds );
      PAMI_assertf( rc == 0, "Kernel_QueryBaseAddressTable failed with rc=%d.\n",rc);

      numFreeBatIdsInGroup += numFreeResourcesInSubgroup;

      subgroup++;
    }
    if( numFreeInjFifosInGroup < minFreeInjFifosPerCore )
      minFreeInjFifosPerCore = numFreeInjFifosInGroup;
    if( numFreeRecFifosInGroup < minFreeRecFifosPerCore )
      minFreeRecFifosPerCore = numFreeRecFifosInGroup;
    if( numFreeBatIdsInGroup < minFreeBatIdsPerCore )
      minFreeBatIdsPerCore = numFreeBatIdsInGroup;
  }
  _perCoreMUResourcesBasedOnAvailability.numInjFifos = minFreeInjFifosPerCore;
  _perCoreMUResourcesBasedOnAvailability.numRecFifos = minFreeRecFifosPerCore;
  _perCoreMUResourcesBasedOnAvailability.numBatIds   = minFreeBatIdsPerCore;

  TRACE_FORMAT("MU ResourceManager: _perCoreMUResourcesBasedOnAvailability.numInjFifos = %zu, _perCoreMUResourcesBasedOnAvailability.numRecFifos = %zu, _perCoreMUResourcesBasedOnAvailability.numBatIds = %zu",_perCoreMUResourcesBasedOnAvailability.numInjFifos, _perCoreMUResourcesBasedOnAvailability.numRecFifos, _perCoreMUResourcesBasedOnAvailability.numBatIds);
  TRACE_FN_EXIT();
} // End: calculatePerCoreMUResourcesBasedOnAvailability()


// \brief Calculate Per Core MU Resources Based On Configuration
//
// Determine how many resources per core are reserved for the SPI user based on
// the specified configuration info.
// Calculate the available resources per core:
// - number of inj fifos available per core
// - number of rec fifos available per core
void PAMI::Device::MU::ResourceManager::calculatePerCoreMUResourcesBasedOnConfig()
{
  TRACE_FN_ENTER();
  size_t numSpiUserInjFifosPerProcess = _pamiRM.getNumSpiUserInjFifosPerProcess();
  size_t numSpiUserInjFifos =
  numSpiUserInjFifosPerProcess / _pamiRM.getNumCoresPerProcess();
  if( ( numSpiUserInjFifos == 0 ) && ( numSpiUserInjFifosPerProcess > 0 ) )
    numSpiUserInjFifos = 1;

  numSpiUserInjFifos = numSpiUserInjFifos * _pamiRM.getNumProcessesPerCore();

  size_t numSpiUserRecFifosPerProcess = _pamiRM.getNumSpiUserRecFifosPerProcess();
  size_t numSpiUserRecFifos =
  numSpiUserRecFifosPerProcess / _pamiRM.getNumCoresPerProcess();
  if( ( numSpiUserRecFifos == 0 ) && ( numSpiUserRecFifosPerProcess > 0 ) )
    numSpiUserRecFifos = 1;

  numSpiUserRecFifos = numSpiUserRecFifos * _pamiRM.getNumProcessesPerCore();

  size_t numSpiUserBatIdsPerProcess = _pamiRM.getNumSpiUserBatIdsPerProcess();
  size_t numSpiUserBatIds =
  numSpiUserBatIdsPerProcess / _pamiRM.getNumCoresPerProcess();
  if( ( numSpiUserBatIds == 0 ) && ( numSpiUserBatIdsPerProcess > 0 ) )
    numSpiUserBatIds = 1;

  numSpiUserBatIds = numSpiUserBatIds * _pamiRM.getNumProcessesPerCore();

  _perCoreMUResourcesBasedOnConfig.numInjFifos = BGQ_MU_NUM_INJ_FIFOS_PER_GROUP - numSpiUserInjFifos;

  _perCoreMUResourcesBasedOnConfig.numRecFifos = BGQ_MU_NUM_REC_FIFOS_PER_GROUP - numSpiUserRecFifos;

  _perCoreMUResourcesBasedOnConfig.numBatIds   = BGQ_MU_NUM_DATA_COUNTERS_PER_GROUP - numSpiUserBatIds;

  TRACE_FORMAT("MU ResourceManager: _perCoreMUResourcesBasedOnConfig.numInjFifos = %zu, _perCoreMUResourcesBasedOnConfig.numRecFifos = %zu, _perCoreMUResourcesBasedOnConfig.numBatIds = %zu", _perCoreMUResourcesBasedOnConfig.numInjFifos, _perCoreMUResourcesBasedOnConfig.numRecFifos, _perCoreMUResourcesBasedOnConfig.numBatIds);
  TRACE_FN_EXIT();
} // End: calculatePerCoreMUResourcesBasedOnConfig()


// \brief Calculate Per Core MU Resources
//
// Calculate the availability of resources based on what has already been
// allocated by the SPI user, and what has been specified in the configuration
// options.
void PAMI::Device::MU::ResourceManager::calculatePerCoreMUResources()
{
  TRACE_FN_ENTER();
  calculatePerCoreMUResourcesBasedOnAvailability();
  calculatePerCoreMUResourcesBasedOnConfig();

  // Our available resources is the minimum of the available resources and what
  // has been specified in configuration options.

  _perCoreMUResources.numInjFifos = _perCoreMUResourcesBasedOnAvailability.numInjFifos;
  if( _perCoreMUResourcesBasedOnConfig.numInjFifos < _perCoreMUResources.numInjFifos )
    _perCoreMUResources.numInjFifos = _perCoreMUResourcesBasedOnConfig.numInjFifos;

  _perCoreMUResources.numRecFifos = _perCoreMUResourcesBasedOnAvailability.numRecFifos;
  if( _perCoreMUResourcesBasedOnConfig.numRecFifos < _perCoreMUResources.numRecFifos )
    _perCoreMUResources.numRecFifos = _perCoreMUResourcesBasedOnConfig.numRecFifos;

  _perCoreMUResources.numBatIds   = _perCoreMUResourcesBasedOnAvailability.numBatIds;
  if( _perCoreMUResourcesBasedOnConfig.numBatIds < _perCoreMUResources.numBatIds )
    _perCoreMUResources.numBatIds = _perCoreMUResourcesBasedOnConfig.numBatIds;

  TRACE_FORMAT("MU ResourceManager: _perCoreMUResources.numInjFifos = %zu, _perCoreMUResources.numRecFifos = %zu, _perCoreMUResources.numBatIds = %zu", _perCoreMUResources.numInjFifos, _perCoreMUResources.numRecFifos, _perCoreMUResources.numBatIds);
  TRACE_FN_EXIT();
} // End: calculatePerCoreMUResources()


// \brief Calculate Per Core Per Process MU Resources
//
// Based on the availability of resources per core, calculate the availability
// of resources per process per core.
void PAMI::Device::MU::ResourceManager::calculatePerCorePerProcessMUResources()
{
  TRACE_FN_ENTER();
  calculatePerCoreMUResources();

  size_t numProcessesPerCore = _pamiRM.getNumProcessesPerCore();

  _perCorePerProcessMUResources.numInjFifos =
  _perCoreMUResources.numInjFifos / numProcessesPerCore;

  PAMI_assertf( _perCorePerProcessMUResources.numInjFifos > 0, "No injection fifos available for PAMI\n" );

  _perCorePerProcessMUResources.numRecFifos =
  _perCoreMUResources.numRecFifos / numProcessesPerCore;

  PAMI_assertf( _perCorePerProcessMUResources.numRecFifos > 0, "No reception fifos available for PAMI\n" );

  _perCorePerProcessMUResources.numBatIds =
  _perCoreMUResources.numBatIds / numProcessesPerCore;

  // We don't assert if there are no BATids.  The protocols will query this resource manager
  // and not enable their protocol if there aren't enough BATids.

  TRACE_FORMAT("MU ResourceManager: _perCorePerProcessMUResources.numInjFifos = %zu, _perCorePerProcessMUResources.numRecFifos = %zu, _perCorePerProcessMUResources.numBatIds = %zu",_perCorePerProcessMUResources.numInjFifos,_perCorePerProcessMUResources.numRecFifos,_perCorePerProcessMUResources.numBatIds);
  TRACE_FN_EXIT();
}


void PAMI::Device::MU::ResourceManager::dividePAMIResourceAmongClients(
                                                                      pamiResources_t &resourceValue,
                                                                      size_t           numClients,
                                                                      pamiResources_t *clientResource )
{
  size_t resourceRemaining = resourceValue.numContexts;
  size_t i;
  for(i=0; i<numClients; i++)
  {
    double numContexts = ( (double)resourceValue.numContexts *
                           (double)_pamiRM.getClientWeight(i) ) / (double)100;
    size_t numContexts_int = numContexts;
    if( ( numContexts - (double)numContexts_int ) >= 0.5 ) numContexts_int++; // Round up.
    if( numContexts_int == 0 ) numContexts_int = 1; // Don't let it be zero.
    while( ( resourceRemaining - numContexts_int ) <
           ( numClients - i - 1 ) ) // Not enough remain to go around?
      numContexts_int--;
    if( numContexts_int > resourceRemaining )
      numContexts_int = resourceRemaining;
    resourceRemaining -= numContexts_int;
    clientResource[i].numContexts = numContexts_int;
  }
}


void PAMI::Device::MU::ResourceManager::divideMUResourceAmongClients(
                                                                    muResources_t &resourceValue,
                                                                    size_t         numClients,
                                                                    muResources_t *clientResource )
{
  // Divide Number of Injection Fifos
  size_t resourceRemaining = resourceValue.numInjFifos;
  size_t i;
  for(i=0; i<numClients; i++)
  {
    double numInjFifos = ( (double)resourceValue.numInjFifos *
                           (double)_pamiRM.getClientWeight(i) ) / (double)100;
    size_t numInjFifos_int = numInjFifos;
    if( ( numInjFifos - (double)numInjFifos_int ) >= 0.5 ) numInjFifos_int++; // Round up.
    if( numInjFifos_int == 0 ) numInjFifos_int = 1; // Don't let it be zero.
    while( ( resourceRemaining - numInjFifos_int ) <
           ( numClients - i - 1 ) ) // Not enough remain to go around?
      numInjFifos_int--;
    if( numInjFifos_int > resourceRemaining )
      numInjFifos_int = resourceRemaining;
    resourceRemaining -= numInjFifos_int;
    clientResource[i].numInjFifos = numInjFifos_int;
  }

  // Divide Number of Reception Fifos
  resourceRemaining = resourceValue.numRecFifos;
  for(i=0; i<numClients; i++)
  {
    double numRecFifos = ( (double)resourceValue.numRecFifos *
                           (double)_pamiRM.getClientWeight(i) ) / (double)100;
    size_t numRecFifos_int = numRecFifos;
    if( ( numRecFifos - (double)numRecFifos_int ) >= 0.5 ) numRecFifos_int++; // Round up.
    if( numRecFifos_int == 0 ) numRecFifos_int = 1; // Don't let it be zero.
    while( ( resourceRemaining - numRecFifos_int ) <
           ( numClients - i - 1 ) ) // Not enough remain to go around?
      numRecFifos_int--;
    if( numRecFifos_int > resourceRemaining )
      numRecFifos_int = resourceRemaining;
    resourceRemaining -= numRecFifos_int;
    clientResource[i].numRecFifos = numRecFifos_int;
  }

  // Divide Number of Base Address Table Entries
  // There could be no BAT entries available, in which case all clients get 0.
  // There could be fewer than the number of clients, in which case the first
  // clients get 1 each until it runs out, and the rest get 0.
  resourceRemaining = resourceValue.numBatIds;
  for(i=0; i<numClients; i++)
  {
    if( resourceRemaining == 0 ) clientResource[i].numBatIds = 0;
    else
    {
      if( resourceRemaining <= (numClients-i) ) // Not enough to go around?
      {
        clientResource[i].numBatIds = 1; // Clients get 1 until it runs out
        resourceRemaining--;
      }
      else
      {
        double numBatIds = ( (double)resourceValue.numBatIds *
                             (double)_pamiRM.getClientWeight(i) ) / (double)100;
        size_t numBatIds_int = numBatIds;
        if( ( numBatIds - (double)numBatIds_int ) >= 0.5 ) numBatIds_int++; // Round up.
        if( numBatIds_int == 0 ) numBatIds_int = 1; // Don't let it be zero.
        while( ( resourceRemaining - numBatIds_int ) <
               ( numClients - i - 1 ) ) // Not enough remain to go around?
          numBatIds_int--;
        if( numBatIds_int > resourceRemaining )
          numBatIds_int = resourceRemaining;
        resourceRemaining -= numBatIds_int;
        clientResource[i].numBatIds = numBatIds_int;
      }
    }
  }
}


void PAMI::Device::MU::ResourceManager::calculatePerProcessMaxPamiResources()
{
  TRACE_FN_ENTER();
  size_t i;
  calculatePerCorePerProcessMUResources();

  size_t numClients = _pamiRM.getNumClients();

  // Allocate space for the array of MaxPamiResources.  Free any previous.
  if(_perProcessMaxPamiResources) __global.heap_mm->free(_perProcessMaxPamiResources);
  pami_result_t prc;
  prc = __global.heap_mm->memalign((void **)&_perProcessMaxPamiResources, 0,
                                   numClients * sizeof(*_perProcessMaxPamiResources));
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _perProcessMaxPamiResources failed");

  // Compute the max number of contexts per process as the minimum of the
  // number of inj and rec fifos.
  size_t totalNumInjFifosPerProcess = _perCorePerProcessMUResources.numInjFifos *
                                      _pamiRM.getNumCoresPerProcess();
  size_t totalNumRecFifosPerProcess = _perCorePerProcessMUResources.numRecFifos *
                                      _pamiRM.getNumCoresPerProcess();

  pamiResources_t maxContextsPerProcess;
  maxContextsPerProcess.numContexts = totalNumInjFifosPerProcess;
  if( totalNumRecFifosPerProcess < maxContextsPerProcess.numContexts )
    maxContextsPerProcess.numContexts = totalNumRecFifosPerProcess;

  dividePAMIResourceAmongClients( maxContextsPerProcess,
                                  numClients,
                                  _perProcessMaxPamiResources );

  for(i=0; i<numClients; i++)
  {
    TRACE_FORMAT("MU ResourceManager: _perProcessMaxPamiResources[%lu].numContexts = %lu",i,_perProcessMaxPamiResources[i].numContexts);
  }
  TRACE_FN_EXIT();
} // End: calculatePerProcessMaxPamiResources()


void PAMI::Device::MU::ResourceManager::calculatePerProcessOptimalPamiResources()
{
  TRACE_FN_ENTER();
  size_t numClients = _pamiRM.getNumClients();
  size_t numInjContextsPerCore;
  size_t numRecContextsPerCore;
  size_t numContextsPerCore;
  size_t i;
  size_t numCoresPerProcess = _pamiRM.getNumCoresPerProcess();

  // Allocate space for the array of OptimalPamiResources.  Free any previous.
  if(_perProcessOptimalPamiResources) __global.heap_mm->free(_perProcessOptimalPamiResources);
  pami_result_t prc;
  prc = __global.heap_mm->memalign((void **)&_perProcessOptimalPamiResources, 0,
                                   numClients * sizeof(*_perProcessOptimalPamiResources));
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _perProcessOptimalPamiResources failed");
  PAMI_assertf ( _perProcessOptimalPamiResources != NULL, "The heap is full.\n" );

  // Calculate optimal number of Inj contexts per process
  size_t clientOptimalNumInjFifosPerContext = optimalNumInjFifosPerContext;
  size_t numInjFifosPerCore = _perCorePerProcessMUResources.numInjFifos;
  while( clientOptimalNumInjFifosPerContext > 0 )
  {
    numInjContextsPerCore = numInjFifosPerCore / clientOptimalNumInjFifosPerContext;
    if( numInjContextsPerCore > 0 ) break;
    clientOptimalNumInjFifosPerContext--;
  }
  PAMI_assertf ( clientOptimalNumInjFifosPerContext > 0, "Not enough injection fifos are available.\n" );

  // Calculate optimal number of Rec contexts per process
  size_t clientOptimalNumRecFifosPerContext = optimalNumRecFifosPerContext;
  size_t numRecFifosPerCore = _perCorePerProcessMUResources.numRecFifos;
  while(1)
  {
    numRecContextsPerCore = numRecFifosPerCore / clientOptimalNumRecFifosPerContext;
    if( numRecContextsPerCore > 0 ) break;
    clientOptimalNumRecFifosPerContext--;
  }
  PAMI_assertf ( clientOptimalNumRecFifosPerContext > 0, "Not enough reception fifos are available.\n" );

  // Calculate optimal number of contexts per process as the minimum of the
  // Inj and Rec.
  numContextsPerCore = numInjContextsPerCore;
  if( numRecContextsPerCore < numContextsPerCore ) numContextsPerCore = numRecContextsPerCore;

  pamiResources_t numContextsPerProcess;
  numContextsPerProcess.numContexts = numContextsPerCore * numCoresPerProcess;

  dividePAMIResourceAmongClients( numContextsPerProcess,
                                  numClients,
                                  _perProcessOptimalPamiResources );

  for(i=0; i<numClients; i++)
  {
    TRACE_FORMAT("MU ResourceManager: _perProcessOptimalPamiResources[%lu].numContexts = %lu",i,_perProcessOptimalPamiResources[i].numContexts);
  }
  TRACE_FN_EXIT();
} // End: calculatePerProcessOptimalPamiResources()


void PAMI::Device::MU::ResourceManager::calculatePerCorePerProcessPerClientMUResources()
{
  TRACE_FN_ENTER();
  size_t i;
  size_t numClients = _pamiRM.getNumClients();

  // Allocate space for the client array
  pami_result_t prc;
  prc = __global.heap_mm->memalign((void **)&_perCorePerProcessPerClientMUResources, 0,
                                   numClients * sizeof(*_perCorePerProcessPerClientMUResources));
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _perCorePerProcessPerClientMUResources failed");

  divideMUResourceAmongClients( _perCorePerProcessMUResources,
                                numClients,
                                _perCorePerProcessPerClientMUResources );

  for(i=0; i<numClients; i++)
  {
    TRACE_FORMAT("MU ResourceManager: _perCorePerProcessPerClientMUResources[%lu].numInjFifos = %lu, .numRecFifos = %lu, .numBatIds = %lu",i,_perCorePerProcessPerClientMUResources[i].numInjFifos,_perCorePerProcessPerClientMUResources[i].numRecFifos, _perCorePerProcessPerClientMUResources[i].numBatIds);
  }
  TRACE_FN_EXIT();
} // End: calculatePerCorePerProcessPerClientMUResources()


// \brief Get Interrupt Mask
//
// Construct a interrupt bitmask indicating which interrupts to clear for the
// specified context.  These indicate which fifos in the group are for
// this context.
// 64 bits:
// - Bits  0 through 31 clear injection fifo threshold crossing
// - Bits 32 through 47 clear reception fifo threshold crossing
// - Bits 48 through 63 clear reception fifo packet arrival
//
uint64_t PAMI::Device::MU::ResourceManager::getInterruptMask ( size_t rmClientId,
                                                               size_t contextOffset )
{
  TRACE_FN_ENTER();
  uint32_t fifo, numFifos;
  uint64_t mask = 0;

  // Determine which inj fifos are in this context
  numFifos = _perContextMUResources[rmClientId].numInjFifos;

  for( fifo=0; fifo<numFifos; fifo++ )
  {
    mask |= 0x8000000000000000ULL >> (_clientResources[rmClientId].injResources[contextOffset].globalFifoIds[fifo] % BGQ_MU_NUM_INJ_FIFOS_PER_GROUP);
  }

  // Determine which rec fifos are in this context
  numFifos = _perContextMUResources[rmClientId].numRecFifos;

  for( fifo=0; fifo<numFifos; fifo++ )
  {
    mask |= 0x80000000ULL >> (_clientResources[rmClientId].recResources[contextOffset].globalFifoIds[fifo] % BGQ_MU_NUM_REC_FIFOS_PER_GROUP);
    mask |= 0x8000ULL >> (_clientResources[rmClientId].recResources[contextOffset].globalFifoIds[fifo] % BGQ_MU_NUM_REC_FIFOS_PER_GROUP);
  }
  TRACE_FORMAT("MU ResourceManager: getInterruptMask: Client %zu, Context %zu, Interrupt mask=0x%lx",rmClientId,contextOffset,mask);

  TRACE_FN_EXIT();
  return mask;
}


// \brief Setup Shared Memory
//
// Set up a shared memory area for global resources to use.
//
void PAMI::Device::MU::ResourceManager::setupSharedMemory()
{
  const char *shmemfile = "/unique-pami-globalRM-shmem-file";

  _mm.init(__global.shared_mm, _memSize, 1, 1, 0, shmemfile);

} // End: setupSharedMemory()


void PAMI::Device::MU::ResourceManager::allocateMemory( const char * key,
                                                        void ** memptr,
                                                        size_t  alignment,
                                                        size_t  bytes )
{
  pami_result_t rc;
  PAMI::Memory::MemoryManager *mm = key ? &_mm : __global.heap_mm;
  rc = mm->memalign( memptr, alignment, bytes, key );
  PAMI_assertf(rc == PAMI_SUCCESS, "out of memory asking for %zd aligned %zd, key %s", bytes, alignment, key ? key : "(nil)");
} // End:  allocateMemory()


// \brief Setup Injection Fifos
//
// Allocates and enables the specified injection fifos.  It will get them
// from the specified range of subgroups.
//
// \param[in]  startingSubgroup  Starting subgroup number
// \param[in]  endingSubgroup    Ending subgroup number
// \param[in]  numFifos          Number of fifos to set up
// \param[in]  fifoSize          The size of the fifos (all are same)
// \param[in]  fifoAttr          Pointer to the attributes of the fifos (all are same)
// \param[in]  key               SharedMemory id or NULL if heap.
// \param[in]  enableInterrupts  Boolean indicating whether to enable MU interrupts
//                               (true) or not (false)
// \param[out] subgroups         Pointer to an array of subgroup structures.
//                               This array is malloc'd and initialized
//                               by this function, and this pointer is set
//                               to point to that array.
// \param[out] fifoPtrs          Pointer to an array of injection fifos.
//                               This array is malloc'd and the fifos are
//                               malloc'd and anchored in thie array.
//                               by this function, and this pointer is set
//                               to point to that array.
// \param[out] globalFifoIds     Pointer to an array of fifo ids.
//                               This array is malloc'd and initialized
//                               by this function, and this pointer is set
//                               to point to that array.
//
// \retval  numFifosSetUp
//
uint32_t PAMI::Device::MU::ResourceManager::setupInjFifos(
                                                         uint32_t startingSubgroup,
                                                         uint32_t endingSubgroup,
                                                         size_t   numFifos,
                                                         size_t   fifoSize,
                                                         Kernel_InjFifoAttributes_t  *fifoAttr,
                                                         const char *key,
                                                         bool                         enableInterrupts,
                                                         MUSPI_InjFifoSubGroup_t    **subgroups,
                                                         char                      ***fifoPtrs,
                                                         uint32_t                   **globalFifoIds)
{
  TRACE_FN_ENTER();
  uint32_t subgroup, fifo;
  int rc;
  uint32_t numFree;
  uint32_t freeIds[BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP];

  size_t outputStructuresSize = (endingSubgroup-startingSubgroup+1) *
                                sizeof(MUSPI_InjFifoSubGroup_t) + 16;
  size_t fifoPtrsArraySize    = numFifos * sizeof(char*) + 16;
  size_t fifosSize            = numFifos * (fifoSize + 64);
  size_t fifoIdsSize          = numFifos * sizeof(uint32_t) + 16;

  if( _calculateSizeOnly == 1 )
  {
    if( key )
    {
      _memSize += outputStructuresSize +
                  fifoPtrsArraySize  +
                  fifosSize +
                  fifoIdsSize;
      TRACE_FORMAT("MU ResourceManager: setupInjFifos: _memSize = %zu",_memSize);
    }
    TRACE_FN_EXIT();
    return 0;

  } // End: _calculateOnly

  size_t keylen = 0;
  char mkey[PAMI::Memory::MMKEYSIZE];
  if(key)
  {
    keylen = strlen(key);
    PAMI_assertf(keylen < sizeof(mkey), "shmem key length overflow");
    strcpy(mkey, key);
    key = mkey;
  }
  // Allocate space for the output structures
  if(key) strcpy(mkey + keylen, "-subgroups");
  allocateMemory( key, (void **)(subgroups), 16, outputStructuresSize );
  TRACE_FORMAT("MU ResourceManager: setupInjFifos: subgroups ptr = %p",*subgroups);
  if( key )
  {
    PAMI_assertf( *subgroups != NULL, "Shared memory is full.\n" );
  }
  else
  {
    PAMI_assertf( *subgroups != NULL, "The heap is full.\n" );
  }

  // pointers need to be in private memory
  //if (key) strcpy(mkey + keylen, "-fifoPtrs");
  allocateMemory( NULL, (void **)(fifoPtrs), 16, fifoPtrsArraySize );
  TRACE_FORMAT("MU ResourceManager: setupInjFifos: fifoPtrs ptr = %p",*fifoPtrs);
  if( key )
  {
    PAMI_assertf ( *fifoPtrs != NULL, "Shared memory is full.\n" );
  }
  else
  {
    PAMI_assertf( *fifoPtrs != NULL, "The heap is full.\n" );
  }

  for( fifo=0; fifo<numFifos; fifo++ )
  {
    if(key) sprintf(mkey + keylen, "-fifoPtrs-%d", fifo);
    allocateMemory( key, (void **)&((*fifoPtrs)[fifo]), 64, fifoSize );
    TRACE_FORMAT("MU ResourceManager: setupInjFifos: fifo ptr address = %p, fifoptr = %p",&((*fifoPtrs)[fifo]), (*fifoPtrs)[fifo]);
    if( key )
    {
      PAMI_assertf ( (*fifoPtrs)[fifo] != NULL, "Shared memory is full.\n" );
    }
    else
    {
      PAMI_assertf ( (*fifoPtrs)[fifo] != NULL, "The heap is full.\n" );
    }
  }

  if(key) strcpy(mkey + keylen, "-globalFifoIds");
  allocateMemory( key, (void **)(globalFifoIds), 16, fifoIdsSize );
  TRACE_FORMAT("MU ResourceManager: setupInjFifos: globalFifoIds ptr = %p",*globalFifoIds);
  if( key )
  {
    PAMI_assertf ( *globalFifoIds != NULL, "Shared memory is full.\n" );
  }
  else
  {
    PAMI_assertf ( *globalFifoIds != NULL, "The heap is full.\n" );
  }

  if( _allocateOnly == 1 ) 
  {
    TRACE_FN_EXIT();
    return 0;
  }

  uint32_t numLeftToAllocate = numFifos;
  uint32_t subgroupIndex     = 0;
  uint32_t fifoIndex         = 0;

  // Loop through each subgroup
  for( subgroup=startingSubgroup; subgroup<=endingSubgroup; subgroup++ )
  {
    // Determine the free fifos in this subgroup.
    rc = Kernel_QueryInjFifos( subgroup,
                               &numFree,
                               freeIds );
    PAMI_assertf( rc==0, "Kernel_QueryInjFifos failed with rc=%d.\n",rc );
    TRACE_FORMAT("MU ResourceManager: setupInjFifos: subgroup = %u, numFree = %u",subgroup,numFree);

    if( numFree == 0 )
    {
      subgroupIndex++;
      continue; // Nothing free in this subgroup?  Go to next.
    }

    if( numFree > numLeftToAllocate ) numFree = numLeftToAllocate;

    Kernel_InjFifoAttributes_t *fifoAttrs;
    pami_result_t prc;
    prc = __global.heap_mm->memalign((void **)&fifoAttrs, 0,
                                     numFree * sizeof(*fifoAttrs));
    PAMI_assertf(prc == PAMI_SUCCESS, "alloc of fifoAttrs failed");
    for(fifo=0; fifo<numFree; fifo++)
      memcpy(&fifoAttrs[fifo],fifoAttr,sizeof(Kernel_InjFifoAttributes_t));

    // Allocate the fifos
    rc = Kernel_AllocateInjFifos( subgroup,
                                  &((*subgroups)[subgroupIndex]),
                                  numFree,
                                  freeIds,
                                  fifoAttrs );
    PAMI_assertf( rc == 0, "Kernel_AllocateInjFifos failed with rc=%d\n",rc );
    TRACE_FORMAT("MU ResourceManager: setupInjFifos: Allocated subgroup ptr = %p",&((*subgroups)[subgroupIndex]));

    __global.heap_mm->free(fifoAttrs); fifoAttrs=NULL;

    // Init the MU MMIO for the fifos.
    for(fifo=0; fifo<numFree; fifo++)
    {
      Kernel_MemoryRegion_t memRegion;
      rc = Kernel_CreateMemoryRegion ( &memRegion,
                                       (*fifoPtrs)[fifoIndex+fifo],
                                       fifoSize );
      PAMI_assertf( rc == 0, "Kernel_CreateMemoryRegion(&memRegion, %p, %zd) [%d] failed with rc=%d, errno=%d\n", (*fifoPtrs)[fifoIndex+fifo], fifoSize, fifoIndex+fifo, rc, errno );

      rc = Kernel_InjFifoInit( &((*subgroups)[subgroupIndex]),
                               freeIds[fifo],
                               &memRegion,
                               (uint64_t)(*fifoPtrs)[fifoIndex+fifo] -
                               (uint64_t)memRegion.BaseVa,
                               fifoSize-1 );
      PAMI_assertf( rc == 0, "Kernel_InjFifoInit failed with rc=%d\n",rc );

      (*globalFifoIds)[fifoIndex+fifo] =
      subgroup * BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP + freeIds[fifo];
      TRACE_FORMAT("MU ResourceManager: setupInjFifos: Initialized subgroup ptr = %p, id=%u, fifoSize=%zu, globalId = %u, fifostruct=%p, hw_injfifo=%p",&((*subgroups)[subgroupIndex]), freeIds[fifo], fifoSize, (*globalFifoIds)[fifoIndex+fifo], &(((*subgroups)[subgroupIndex])._injfifos[freeIds[fifo]]), (((*subgroups)[subgroupIndex])._injfifos[freeIds[fifo]]).hw_injfifo );
    }

    // Activate the fifos.
    rc = Kernel_InjFifoActivate( &((*subgroups)[subgroupIndex]),
                                 numFree,
                                 freeIds,
                                 KERNEL_INJ_FIFO_ACTIVATE );
    PAMI_assertf( rc == 0, "Kernel_InjFifoActivate failed with rc=%d\n",rc );

    if( enableInterrupts )
    {
      // Enable interrupts.
      Kernel_InjFifoInterrupts_t *fifoInterrupts;
      prc = __global.heap_mm->memalign((void **)&fifoInterrupts, 0,
                                       numFree * sizeof(*fifoInterrupts));
      PAMI_assertf(prc == PAMI_SUCCESS, "alloc of fifoInterrupts failed");
      PAMI_assertf( fifoInterrupts != NULL, "The heap is full.\n" );
      for(fifo=0; fifo<numFree; fifo++)
      {
        memset(&fifoInterrupts[fifo],0x00,sizeof(Kernel_InjFifoInterrupts_t));
        fifoInterrupts[fifo].Threshold_Crossing = 1;
      }

      rc = Kernel_ConfigureInjFifoInterrupts( &((*subgroups)[subgroupIndex]),
                                              numFree,
                                              freeIds,
                                              fifoInterrupts);
      PAMI_assertf( rc == 0, "ConfigureInjFifoInterrupts failed with rc=%d\n",rc);
      __global.heap_mm->free( fifoInterrupts);
    }

    fifoIndex         += numFree;

    numLeftToAllocate -= numFree;
    if( numLeftToAllocate == 0 ) break;

    subgroupIndex++;

  } // End: Loop through subgroups

  TRACE_FN_EXIT();
  return( fifoIndex );
} // End: setupInjFifos()


// \brief Allocate Global Injection Fifos
//
// Allocate resources needed before main().  These include:
// -  1 collective combining injection fifo (somewhere in subgroup 64 or 65).
// - 10 remote get injection fifos (somewhere in subgroups 64 and 65).
//
void PAMI::Device::MU::ResourceManager::allocateGlobalInjFifos()
{
  TRACE_FN_ENTER();
  uint32_t numFifosSetup;
  Kernel_InjFifoAttributes_t  fifoAttr;

  // Set up the combining collective fifo

  memset( &fifoAttr, 0x00, sizeof(fifoAttr) );

  numFifosSetup = setupInjFifos( 64, // Starting subgroup
                                 65, // Ending subgroup
                                 1,  // Number of fifos
                                 _pamiRM.getInjFifoSize(),
                                 &fifoAttr,
                                 NULL, // Do not use shared memory
                                 false, // Do not enable interrupts
                                 &(_globalCombiningInjFifo.subgroups),
                                 &(_globalCombiningInjFifo.fifoPtrs),
                                 &(_globalCombiningInjFifo.globalFifoIds) );
  PAMI_assertf( (_calculateSizeOnly == 1) || (_allocateOnly == 1) || (numFifosSetup == 1), "Only %u injection fifos were set up.  Expected 1.\n",numFifosSetup );

  if( (_calculateSizeOnly == 0) && (_allocateOnly == 0) )
  {
    allocateLookasideResources(
                              1, /* numInjFifos */
                              _pamiRM.getRgetInjFifoSize(),
                              &(_globalCombiningInjFifo.lookAsidePayloadPtrs),
                              &(_globalCombiningInjFifo.lookAsidePayloadMemoryRegions),
                              &(_globalCombiningInjFifo.lookAsidePayloadPAs),
                              &(_globalCombiningInjFifo.lookAsideCompletionFnPtrs),
                              &(_globalCombiningInjFifo.lookAsideCompletionCookiePtrs));
  }

  // Set up the remote get fifos

  memset( &fifoAttr, 0x00, sizeof(fifoAttr) );

  fifoAttr.RemoteGet = 1;

  numFifosSetup = setupInjFifos( 64, // Starting subgroup
                                 65, // Ending subgroup
                                 10, // Number of fifos
                                 _pamiRM.getRgetInjFifoSize(),
                                 &fifoAttr,
                                 "allocateGlobalInjFifos-fifoAttr", // Use shared memory
                                 false,// Do not enable interrupts
                                 &_globalRgetInjSubGroups,
                                 &_globalRgetInjFifoPtrs,
                                 &_globalRgetInjFifoIds );
  PAMI_assertf( (_calculateSizeOnly == 1) || (_allocateOnly == 1) || (numFifosSetup == 10), "Only %u injection fifos were set up.  Expected 10.\n",numFifosSetup );

  // Set up the remote get fifo pin information entry.
  // 0.  Allocate space for the entry.
  // 1.  Copy the torus_inj_fifo_map array to the entry.
  // 2.  Initialize the fifo numbers to the global rget fifo numbers.
  //     Use the same relative rget fifos as the 9 normal fifos.
  if( _calculateSizeOnly == 1 ) 
  {
    TRACE_FN_EXIT();
    return;
  }

  allocateMemory( NULL /*do not useSharedMemory*/,
                  (void **)(&_rgetPinInfo),
                  64,
                  sizeof(*_rgetPinInfo) );
  TRACE_FORMAT("MU ResourceManager: allocateGlobalInjFifos: _rgetPinInfo = %p",_rgetPinInfo);
  PAMI_assertf ( _rgetPinInfo != NULL, "The heap is full.\n" );

  memcpy ( &_rgetPinInfo->torusInjFifoMaps[0],
           &_pinInfo[9].torusInjFifoMaps[0],
           sizeof(_rgetPinInfo->torusInjFifoMaps) );

  TRACE_FORMAT("MU ResourceManager: allocateGlobalInjFifos: _rgetPinInfo->torusInjFifoMaps{} = 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x, 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x",_rgetPinInfo->torusInjFifoMaps[0],_rgetPinInfo->torusInjFifoMaps[1],_rgetPinInfo->torusInjFifoMaps[2],_rgetPinInfo->torusInjFifoMaps[3],_rgetPinInfo->torusInjFifoMaps[4],_rgetPinInfo->torusInjFifoMaps[5],_rgetPinInfo->torusInjFifoMaps[6],_rgetPinInfo->torusInjFifoMaps[7],_rgetPinInfo->torusInjFifoMaps[8],_rgetPinInfo->torusInjFifoMaps[9],_rgetPinInfo->torusInjFifoMaps[10],_rgetPinInfo->torusInjFifoMaps[11],_rgetPinInfo->torusInjFifoMaps[12],_rgetPinInfo->torusInjFifoMaps[13],_rgetPinInfo->torusInjFifoMaps[14],_rgetPinInfo->torusInjFifoMaps[15]);
  TRACE_FN_EXIT();
} // End: allocateGlobalInjFifos()


// \brief Setup Base Address Table Ids
//
// Allocates the specified Base Address Table Ids.  It will get them
// from the specified range of subgroups.
//
// \param[in]  startingSubgroup  Starting subgroup number
// \param[in]  endingSubgroup    Ending subgroup number
// \param[in]  numBatIds         Number of BAT ids to set up
// \param[in]  key               shared memory key or NULL for heap.
// \param[out] subgroups         Pointer to an array of subgroup structures.
//                               This array is malloc'd and initialized
//                               by this function, and this pointer is set
//                               to point to that array.
// \param[out] globalBatIds      Pointer to an array of BAT ids.
//                               This array is malloc'd and initialized
//                               by this function, and this pointer is set
//                               to point to that array.
//
// \retval  numBatIdssSetUp
//
uint32_t PAMI::Device::MU::ResourceManager::setupBatIds(
                                                       uint32_t                           startingSubgroup,
                                                       uint32_t                           endingSubgroup,
                                                       uint32_t                           numBatIds,
                                                       const char                        *key,
                                                       MUSPI_BaseAddressTableSubGroup_t **subgroups,
                                                       uint32_t                         **globalBatIds)
{
  TRACE_FN_ENTER();
  uint32_t subgroup, batId;
  int rc;
  uint32_t numFree;
  uint32_t freeIds[BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP];

  size_t outputStructuresSize = (endingSubgroup-startingSubgroup+1) *
                                sizeof(MUSPI_BaseAddressTableSubGroup_t) + 16;
  size_t batIdsSize           = numBatIds * sizeof(uint32_t) + 16;

  if( _calculateSizeOnly == 1 )
  {
    _memSize += outputStructuresSize + batIdsSize;
    TRACE_FORMAT("MU ResourceManager: setupBatIds: _memSize = %zu",_memSize);
    TRACE_FN_EXIT();
    return 0;

  } // End: _calculateOnly

  // Allocate space for the output structures
  size_t keylen = 0;
  char mkey[PAMI::Memory::MMKEYSIZE];
  if(key)
  {
    keylen = strlen(key);
    PAMI_assertf(keylen < sizeof(mkey), "shmem key length overflow");
    strcpy(mkey, key);
    key = mkey;
  }
  if(key) strcpy(mkey + keylen, "-subgroups");
  allocateMemory( key, (void **)(subgroups), 16, outputStructuresSize );
  TRACE_FORMAT("MU ResourceManager: setupBatIds: subgroups ptr = %p",*subgroups);
  if( key )
  {
    PAMI_assertf( *subgroups != NULL, "Shared memory is full.\n" );
  }
  else
  {
    PAMI_assertf( *subgroups != NULL, "The heap is full.\n" );
  }

  if(key) strcpy(mkey + keylen, "-globalBatIds");
  allocateMemory( key, (void **)(globalBatIds), 16, batIdsSize );
  TRACE_FORMAT("MU ResourceManager: setupBatIds: globalBatIds ptr = %p",*globalBatIds);
  if( key )
  {
    PAMI_assertf ( *globalBatIds != NULL, "Shared memory is full.\n" );
  }
  else
  {
    PAMI_assertf ( *globalBatIds != NULL, "The heap is full.\n" );
  }

  if( _allocateOnly == 1 ) 
  {
    TRACE_FN_EXIT();
    return 0;
  }

  uint32_t numLeftToAllocate = numBatIds;
  uint32_t subgroupIndex     = 0;
  uint32_t batIdIndex        = 0;

  // Loop through each subgroup
  for( subgroup=startingSubgroup; subgroup<=endingSubgroup; subgroup++ )
  {
    // Determine the free BAT Ids in this subgroup.
    rc = Kernel_QueryBaseAddressTable( subgroup,
                                       &numFree,
                                       freeIds );
    PAMI_assertf( rc==0, "Kernel_QueryBaseAddressTable failed with rc=%d\n",rc );
    TRACE_FORMAT("MU ResourceManager: setupBatIds: subgroup = %u, numFree = %u",subgroup,numFree);

    if( numFree == 0 )
    {
      subgroupIndex++;
      continue; // Nothing free in this subgroup?  Go to next.
    }

    if( numFree > numLeftToAllocate ) numFree = numLeftToAllocate;

    // Allocate the BAT Ids
    rc = Kernel_AllocateBaseAddressTable( subgroup,
                                          &((*subgroups)[subgroupIndex]),
                                          numFree,
                                          freeIds,
                                          0 /* "User" access */);
    PAMI_assertf( rc == 0, "Kernel_AllocateBaseAddressTable failed with rc=%d\n",rc );
    TRACE_FORMAT("MU ResourceManager: setupBatIds: Allocated subgroup ptr = %p",&((*subgroups)[subgroupIndex]));

    // Calculate the global BAT ID and return it.
    for( batId=0; batId<numFree; batId++ )
    {
      (*globalBatIds)[batIdIndex+batId] =
      subgroup * BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP + freeIds[batId];
      TRACE_FORMAT("MU ResourceManager: setupBatIds: Initialized subgroup ptr = %p, id=%u, globalId = %u",&((*subgroups)[subgroupIndex]), freeIds[batId], (*globalBatIds)[batIdIndex+batId] );
    }

    batIdIndex += numFree;

    numLeftToAllocate -= numFree;
    if( numLeftToAllocate == 0 ) break;

    subgroupIndex++;

  } // End: Loop through subgroups

  TRACE_FN_EXIT();
  return( batIdIndex );
} // End: setupBatIds()


// \brief Allocate Global Base Address Table Entry
//
// Allocate resources needed before main().  These include:
// -  1 base address table entry in subgroup 64 or 65, initialized to 0.
// -  1 base address table entry in subgroup 64 or 65, initialized to the atomic
//    address of a shared counter, whose value is ignored.
// -  3 base address table entries in subgroup 64 or 65 for collective broadcasts.
//
void PAMI::Device::MU::ResourceManager::allocateGlobalBaseAddressTableEntries()
{
  TRACE_FN_ENTER();
  uint32_t numBatIdsSetup;
  int32_t  rc;

  numBatIdsSetup = setupBatIds( 64, // Starting subgroup
                                65, // Ending subgroup
                                //5,  // Number of BAT ids (see above list)
                                7,  // Number of BAT ids (see above list)
                                "allocateGlobalBaseAddressTableEntries-_globalBatIds", // Use shared memory
                                &_globalBatSubGroups,
                                &_globalBatIds );
  //PAMI_assertf( (_calculateSizeOnly == 1) || (_allocateOnly == 1) || (numBatIdsSetup == 5), "Only %u base address Ids were set up.  Expected 5.\n",numBatIdsSetup );
  PAMI_assertf( (_calculateSizeOnly == 1) || (_allocateOnly == 1) || (numBatIdsSetup == 7), "Only %u base address Ids were set up.  Expected 5.\n",numBatIdsSetup );

  // Incorporate the size of the shared counter into the shared memory space requirement.
  if( _calculateSizeOnly == 1 )
  {
    _memSize += sizeof(uint64_t) + 16;
    TRACE_FORMAT("MU ResourceManager: allocateGlobalBaseAddressTableEntries: _memSize = %zu",_memSize);
    TRACE_FN_EXIT();
    return;

  } // End: _calculateOnly

  // Allocate space for the shared counter in shared memory.
  uint64_t *sharedCounterPtr;
  allocateMemory( "allocateGlobalBaseAddressTableEntries-sharedCounterPtr" /*useSharedMemory*/, (void **)(&sharedCounterPtr), 8, sizeof(uint64_t) );
  TRACE_FORMAT("MU ResourceManager: allocateGlobalBaseAddressTableEntries: Shared counter address = %p",sharedCounterPtr);
  PAMI_assertf( sharedCounterPtr != NULL, "Shared memory is full.\n" );

  if( _allocateOnly == 1 ) 
  {
    TRACE_FN_EXIT();
    return;
  }

  // Set the first BAT slot to a 0 physical address.
  uint32_t batSubgroup = ( _globalBatIds[0] / BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP ) - 64;
  uint8_t  batId       = _globalBatIds[0] % BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP;
  rc = MUSPI_SetBaseAddress ( &_globalBatSubGroups[batSubgroup],
                              batId,
                              0 /* MUHWI_BaseAddress_t */ );
  TRACE_FORMAT("MU ResourceManager: allocateGlobalBaseAddressTableEntry: Zero BAT entry: Relative batSubgroup=%u, Relative batId=%u, Global batId=%u, rc=%d",batSubgroup, batId, _globalBatIds[0], rc);
  PAMI_assertf( rc == 0, "MUSPI_SetBaseAddress failed with rc=%d\n",rc );

  // Set the second BAT slot to the atomic physical address of the shared counter.
  Kernel_MemoryRegion_t memRegion;
  rc = Kernel_CreateMemoryRegion ( &memRegion,
                                   sharedCounterPtr,
                                   sizeof(uint64_t) );
  PAMI_assertf( rc == 0, "Kernel_CreateMemoryRegion failed with rc=%d\n",rc );

  uint64_t sharedCounterPA = (uint64_t)memRegion.BasePa +
                             ((uint64_t)sharedCounterPtr - (uint64_t)memRegion.BaseVa);

  batSubgroup = ( _globalBatIds[1] / BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP ) - 64;
  batId       = _globalBatIds[1] % BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP;

  uint64_t sharedCounterBATvalue = (uint64_t)MUSPI_GetAtomicAddress (
                                                                    sharedCounterPA,
                                                                    MUHWI_ATOMIC_OPCODE_STORE_ADD_COHERENCE_ON_ZERO);

  rc = MUSPI_SetBaseAddress ( &_globalBatSubGroups[batSubgroup],
                              batId,
                              sharedCounterBATvalue );
  TRACE_FORMAT("MU ResourceManager: allocateGlobalBaseAddressTableEntry: Shared Counter BAT entry: Relative batSubgroup=%u, Relative batId=%u, Global batId=%u, rc=%d, Shared Counter PA=0x%lx, BAT value = 0x%lx",batSubgroup, batId, _globalBatIds[1], rc, sharedCounterPA, sharedCounterBATvalue );
  PAMI_assertf( rc == 0, "MUSPI_SetBaseAddress failed with rc=%d\n",rc );

  // Don't set the next 3 BAT slots (for collective bcasts) to anything.
  TRACE_FN_EXIT();
} // End: allocateGlobalBaseAddressTableEntry()


// \brief Setup Reception Fifos
//
// Allocates and enables the specified reception fifos.  It will get them
// from the specified range of subgroups.
//
// \param[in]  startingSubgroup  Starting subgroup number
// \param[in]  endingSubgroup    Ending subgroup number
// \param[in]  numFifos          Number of fifos to set up
// \param[in]  fifoSize          The size of the fifos (all are same)
// \param[in]  fifoAttr          Pointer to the attributes of the fifos (all are same)
// \param[in]  key               shared memory key or NULL for heap.
// \param[out] subgroups         Pointer to an array of subgroup structures.
//                               This array is malloc'd and initialized
//                               by this function, and this pointer is set
//                               to point to that array.
// \param[out] fifoPtrs          Pointer to an array of reception fifos.
//                               This array is malloc'd and the fifos are
//                               malloc'd and anchored in thie array.
//                               by this function, and this pointer is set
//                               to point to that array.
// \param[out] globalFifoIds     Pointer to an array of fifo ids.
//                               This array is malloc'd and initialized
//                               by this function, and this pointer is set
//                               to point to that array.
//
// \retval  numFifosSetUp
//
uint32_t PAMI::Device::MU::ResourceManager::setupRecFifos(
                                                         uint32_t startingSubgroup,
                                                         uint32_t endingSubgroup,
                                                         size_t   numFifos,
                                                         size_t   fifoSize,
                                                         Kernel_RecFifoAttributes_t  *fifoAttr,
                                                         const char *key,
                                                         MUSPI_RecFifoSubGroup_t    **subgroups,
                                                         char                      ***fifoPtrs,
                                                         uint32_t                   **globalFifoIds)
{
  TRACE_FN_ENTER();
  uint32_t subgroup, fifo;
  int rc;
  uint32_t numFree;
  uint32_t freeIds[BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP];

  size_t outputStructuresSize = (endingSubgroup-startingSubgroup+1) *
                                sizeof(MUSPI_RecFifoSubGroup_t) + 16;
  size_t fifoPtrsArraySize    = numFifos * sizeof(char*) + 16;
  size_t fifosSize            = numFifos * (fifoSize + 32);
  size_t fifoIdsSize          = numFifos * sizeof(uint32_t) + 16;

  if( _calculateSizeOnly == 1 )
  {
    _memSize += outputStructuresSize +
                fifoPtrsArraySize  +
                fifosSize +
                fifoIdsSize;
    TRACE_FORMAT("MU ResourceManager: setupRecFifos: _memSize = %zu",_memSize);
    TRACE_FN_EXIT();
    return 0;

  } // End: _calculateOnly

  // Allocate space for the output structures
  size_t keylen = 0;
  char mkey[PAMI::Memory::MMKEYSIZE];
  if(key)
  {
    keylen = strlen(key);
    PAMI_assertf(keylen < sizeof(mkey), "shmem key length overflow");
    strcpy(mkey, key);
    key = mkey;
  }
  if(key) strcpy(mkey + keylen, "-subgroups");
  allocateMemory( key, (void **)(subgroups), 16, outputStructuresSize );
  TRACE_FORMAT("MU ResourceManager: setupRecFifos: subgroups ptr = %p",*subgroups);
  if( key )
  {
    PAMI_assertf( *subgroups != NULL, "Shared memory is full.\n" );
  }
  else
  {
    PAMI_assertf( *subgroups != NULL, "The heap is full.\n" );
  }

  // pointers need to be in private memory
  //if (key) strcpy(mkey + keylen, "-fifoPtrs");
  allocateMemory( NULL, (void **)(fifoPtrs), 16, fifoPtrsArraySize );
  TRACE_FORMAT("MU ResourceManager: setupRecFifos: fifoPtrs ptr = %p",*fifoPtrs);
  if( key )
  {
    PAMI_assertf ( *fifoPtrs != NULL, "Shared memory is full.\n" );
  }
  else
  {
    PAMI_assertf( *fifoPtrs != NULL, "The heap is full.\n" );
  }

  for( fifo=0; fifo<numFifos; fifo++ )
  {
    if(key) sprintf(mkey + keylen, "-fifoPtrs-%d", fifo);
    allocateMemory( key, (void **)&((*fifoPtrs)[fifo]), 32, fifoSize );
    TRACE_FORMAT("MU ResourceManager: setupRecFifos: fifo ptr address = %p, fifoptr = %p",&((*fifoPtrs)[fifo]), (*fifoPtrs)[fifo]);
    if( key )
    {
      PAMI_assertf ( (*fifoPtrs)[fifo] != NULL, "Shared memory is full.\n" );
    }
    else
    {
      PAMI_assertf ( (*fifoPtrs)[fifo] != NULL, "The heap is full.\n" );
    }
  }

  if(key) strcpy(mkey + keylen, "-globalFifoIds");
  allocateMemory( key, (void **)(globalFifoIds), 16, fifoIdsSize );
  TRACE_FORMAT("MU ResourceManager: setupRecFifos: globalFifoIds ptr = %p",*globalFifoIds);
  if( key )
  {
    PAMI_assertf ( *globalFifoIds != NULL, "Shared memory is full.\n" );
  }
  else
  {
    PAMI_assertf ( *globalFifoIds != NULL, "The heap is full.\n" );
  }

  if( _allocateOnly == 1 ) 
  {
    TRACE_FN_EXIT();
    return 0;
  }

  uint32_t numLeftToAllocate = numFifos;
  uint32_t subgroupIndex     = 0;
  uint32_t fifoIndex         = 0;

  // Loop through each subgroup
  for( subgroup=startingSubgroup; subgroup<=endingSubgroup; subgroup++ )
  {
    // Determine the free fifos in this subgroup.
    rc = Kernel_QueryRecFifos( subgroup,
                               &numFree,
                               freeIds );
    PAMI_assertf( rc==0, "Kernel_QueryRecFifos failed with rc=%d.\n",rc );
    TRACE_FORMAT("MU ResourceManager: setupRecFifos: subgroup = %u, numFree = %u",subgroup,numFree);

    if( numFree == 0 )
    {
      subgroupIndex++;
      continue; // Nothing free in this subgroup?  Go to next.
    }

    if( numFree > numLeftToAllocate ) numFree = numLeftToAllocate;

    Kernel_RecFifoAttributes_t *fifoAttrs;
    pami_result_t prc;
    prc = __global.heap_mm->memalign((void **)&fifoAttrs, 0,
                                     numFree * sizeof(*fifoAttrs));
    PAMI_assertf(prc == PAMI_SUCCESS, "alloc of fifoAttrs failed");
    for(fifo=0; fifo<numFree; fifo++)
      memcpy(&fifoAttrs[fifo],fifoAttr,sizeof(Kernel_RecFifoAttributes_t));

    // Allocate the fifos
    rc = Kernel_AllocateRecFifos( subgroup,
                                  &((*subgroups)[subgroupIndex]),
                                  numFree,
                                  freeIds,
                                  fifoAttrs );
    PAMI_assertf( rc == 0, "Kernel_AllocateRecFifos failed with rc=%d.\n",rc );
    TRACE_FORMAT("MU ResourceManager: setupRecFifos: Allocated subgroup ptr = %p",&((*subgroups)[subgroupIndex]));

    __global.heap_mm->free(fifoAttrs); fifoAttrs=NULL;

    uint64_t enableBits = 0;

    // Init the MU MMIO for the fifos.
    for(fifo=0; fifo<numFree; fifo++)
    {
      Kernel_MemoryRegion_t memRegion;
      rc = Kernel_CreateMemoryRegion ( &memRegion,
                                       (*fifoPtrs)[fifoIndex+fifo],
                                       fifoSize );
      PAMI_assertf( rc == 0, "Kernel_CreateMemoryRegion failed with rc=%d\n",rc );

      rc = Kernel_RecFifoInit( &((*subgroups)[subgroupIndex]),
                               freeIds[fifo],
                               &memRegion,
                               (uint64_t)(*fifoPtrs)[fifoIndex+fifo] -
                               (uint64_t)memRegion.BaseVa,
                               fifoSize-1 );
      PAMI_assertf( rc == 0, "Kernel_RecFifoInit failed with rc=%d\n",rc );

      (*globalFifoIds)[fifoIndex+fifo] =
      subgroup * BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP + freeIds[fifo];
      TRACE_FORMAT("MU ResourceManager: setupRecFifos: Initialized subgroup ptr = %p, id=%u, fifoSize=%zu, globalId = %u",&((*subgroups)[subgroupIndex]), freeIds[fifo], fifoSize, (*globalFifoIds)[fifoIndex+fifo] );

      enableBits |= 0x8000ULL >>
                    ( (subgroup % BGQ_MU_NUM_REC_FIFO_SUBGROUPS)*BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP + freeIds[fifo] );
    }

    // Enable interrupts.
    Kernel_RecFifoInterrupts_t *fifoInterrupts;
    prc = __global.heap_mm->memalign((void **)&fifoInterrupts, 0,
                                     numFree * sizeof(*fifoInterrupts));
    PAMI_assertf(prc == PAMI_SUCCESS, "alloc of fifoInterrupts failed");
    for(fifo=0; fifo<numFree; fifo++)
    {
      memset(&fifoInterrupts[fifo],0x00,sizeof(Kernel_RecFifoInterrupts_t));
      fifoInterrupts[fifo].Threshold_Crossing = 1;
      fifoInterrupts[fifo].Packet_Arrival     = 1;
    }

    rc = Kernel_ConfigureRecFifoInterrupts( &((*subgroups)[subgroupIndex]),
                                            numFree,
                                            freeIds,
                                            fifoInterrupts);
    PAMI_assertf( rc == 0, "ConfigureRecFifoInterrupts failed with rc=%d\n",rc);
    __global.heap_mm->free( fifoInterrupts);

    // Enable the fifos.
    TRACE_FORMAT("MU ResourceManager: setupRecFifos: Enabling RecFifos in group %u, bits 0x%lx",subgroup/BGQ_MU_NUM_REC_FIFO_SUBGROUPS,enableBits);
    rc = Kernel_RecFifoEnable( subgroup/BGQ_MU_NUM_REC_FIFO_SUBGROUPS,
                               enableBits );
    PAMI_assertf( rc == 0, "Kernel_RecFifoEnable failed with rc=%d\n",rc );

    fifoIndex         += numFree;

    numLeftToAllocate -= numFree;
    if( numLeftToAllocate == 0 ) break;

    subgroupIndex++;

  } // End: Loop through subgroups

  TRACE_FN_EXIT();
  return( fifoIndex );
} // End: setupRecFifos()


// \brief Allocate Global Reception Fifos
//
// Allocate resources needed before main().  These include:
// - Configuring the reception fifo threshold, which is global across the node.
//
void PAMI::Device::MU::ResourceManager::allocateGlobalRecFifos()
{
} // End: allocateGlobalRecFifos()


void PAMI::Device::MU::ResourceManager::allocateLookasideResources(
                                                                  size_t                   numInjFifos,
                                                                  size_t                   injFifoSize,
                                                                  char                  ***lookAsidePayloadPtrs,
                                                                  Kernel_MemoryRegion_t  **lookAsidePayloadMemoryRegions,
                                                                  uint64_t               **lookAsidePayloadPAs,
                                                                  pami_event_function   ***lookAsideCompletionFnPtrs,
                                                                  void                 ****lookAsideCompletionCookiePtrs)
{
  TRACE_FN_ENTER();
  int32_t  rc;
  uint32_t fifo;
  size_t lookAsidePayloadBufferSize = (injFifoSize / sizeof(MUHWI_Descriptor_t)) *
                                      sizeof(InjGroup::immediate_payload_t);
  size_t lookAsideCompletionFnArraySize = (injFifoSize / sizeof(MUHWI_Descriptor_t)) *
                                          sizeof(pami_event_function);
  size_t lookAsideCompletionCookieArraySize = (injFifoSize / sizeof(MUHWI_Descriptor_t)) *
                                              sizeof(void *);

  // Set up array of pointers to the lookaside payload buffers.  There is pointer per inj fifo.
  char **_lookAsidePayloadPtrs;
  allocateMemory( NULL, // Use heap
                  (void **)(&_lookAsidePayloadPtrs),
                  16,
                  numInjFifos * sizeof(char**) );
  TRACE_FORMAT("MU ResourceManager: allocateLookasideResources: lookAsidePayloadPtrs = %p",_lookAsidePayloadPtrs);
  PAMI_assertf( _lookAsidePayloadPtrs != NULL, "The heap is full.\n" );
  *lookAsidePayloadPtrs = _lookAsidePayloadPtrs;

  // Allocate each of the lookaside payload buffers.  They have the same number of slots
  // as the inj fifo.
  for( fifo=0; fifo<numInjFifos; fifo++ )
  {
    allocateMemory( NULL, // Use heap
                    (void **)&(_lookAsidePayloadPtrs[fifo]),
                    32,
                    lookAsidePayloadBufferSize );
    TRACE_FORMAT("MU ResourceManager: allocateLookasideResources: lookAsidePayloadPtr[%u] = %p",fifo,_lookAsidePayloadPtrs[fifo]);
    PAMI_assertf ( _lookAsidePayloadPtrs[fifo] != NULL, "The heap is full.\n" );
  }

  // Set up array of memory regions corresponding to the lookaside payload buffers.
  // We need this to get the physical addresses of the lookaside payload buffers.
  // There is 1 memory region per lookaside buffer (ie. per inj fifo).
  Kernel_MemoryRegion_t *_lookAsidePayloadMemoryRegions;
  allocateMemory( NULL, // Use heap
                  (void **)(&_lookAsidePayloadMemoryRegions),
                  16,
                  numInjFifos * sizeof(Kernel_MemoryRegion_t) );
  TRACE_FORMAT("MU ResourceManager: allocateLookasideResources: lookAsidePayloadMemoryRegions = %p",_lookAsidePayloadMemoryRegions);
  PAMI_assertf( _lookAsidePayloadMemoryRegions != NULL, "The heap is full.\n" );
  *lookAsidePayloadMemoryRegions = _lookAsidePayloadMemoryRegions;

  // Initialize each of the lookaside payload memory regions.
  for( fifo=0; fifo<numInjFifos; fifo++ )
  {
    rc = Kernel_CreateMemoryRegion ( &_lookAsidePayloadMemoryRegions[fifo],
                                     _lookAsidePayloadPtrs[fifo],
                                     lookAsidePayloadBufferSize );
    PAMI_assertf( rc==0, "Kernel_CreateMemoryRegion failed with rc=%d\n",rc);
  }

  // Set up array of lookaside payload buffer physical addresses.
  // There is 1 PA for each lookaside buffer (ie. for each inj fifo).
  uint64_t *_lookAsidePayloadPAs;
  allocateMemory( NULL, // Use heap
                  (void **)(&_lookAsidePayloadPAs),
                  16,
                  numInjFifos * sizeof(uint64_t) );
  TRACE_FORMAT("MU ResourceManager: allocateLookasideResources: lookAsidePayloadPAs = %p",_lookAsidePayloadPAs);
  PAMI_assertf( _lookAsidePayloadPAs != NULL, "The heap is full.\n" );
  *lookAsidePayloadPAs = _lookAsidePayloadPAs;

  // Calculate the lookaside payload PAs.
  for( fifo=0; fifo<numInjFifos; fifo++ )
  {
    _lookAsidePayloadPAs[fifo] =
    (uint64_t)_lookAsidePayloadPtrs[fifo] -
    (uint64_t)_lookAsidePayloadMemoryRegions[fifo].BaseVa +
    (uint64_t)_lookAsidePayloadMemoryRegions[fifo].BasePa;
    TRACE_FORMAT("MU ResourceManager: allocateLookasideResources: lookAsidePayloadPAs[%u] = 0x%lx",fifo,_lookAsidePayloadPAs[fifo]);
  }

  // Set up array of pointers to lookaside completion function arrays.
  // There is 1 pointer for each inj fifo.
  pami_event_function **_lookAsideCompletionFnPtrs;
  allocateMemory( NULL, // Use heap
                  (void **)(&_lookAsideCompletionFnPtrs),
                  16,
                  numInjFifos * sizeof(pami_event_function *) );
  TRACE_FORMAT("MU ResourceManager: allocateLookasideResources: lookAsideCompletionFnPtrs = %p",_lookAsideCompletionFnPtrs);
  PAMI_assertf( _lookAsideCompletionFnPtrs != NULL, "The heap is full.\n" );
  *lookAsideCompletionFnPtrs = _lookAsideCompletionFnPtrs;

  // Allocate each of the lookaside completion function arrays.  They have the same number of slots
  // as the inj fifo.
  for( fifo=0; fifo<numInjFifos; fifo++ )
  {
    allocateMemory( NULL, // Use heap
                    (void **)&(_lookAsideCompletionFnPtrs[fifo]),
                    16,
                    lookAsideCompletionFnArraySize );
    TRACE_FORMAT("MU ResourceManager: allocateLookasideResources: lookAsideCompletionFnPtrs[%u] = %p",fifo,_lookAsideCompletionFnPtrs[fifo]);
    PAMI_assertf ( _lookAsideCompletionFnPtrs[fifo] != NULL, "The heap is full.\n" );
    memset( _lookAsideCompletionFnPtrs[fifo], 0x00, lookAsideCompletionFnArraySize );
  }

  // Set up array of pointers to lookaside completion cookie arrays.
  // There is 1 pointer for each inj fifo.
  void ***_lookAsideCompletionCookiePtrs;
  allocateMemory( NULL, // Use heap
                  (void **)(&_lookAsideCompletionCookiePtrs),
                  16,
                  numInjFifos * sizeof(void **) );
  TRACE_FORMAT("MU ResourceManager: allocateLookasideResources: lookAsideCompletionCookiePtrs = %p",_lookAsideCompletionCookiePtrs);
  PAMI_assertf( _lookAsideCompletionCookiePtrs != NULL, "The heap is full.\n" );
  *lookAsideCompletionCookiePtrs = _lookAsideCompletionCookiePtrs;

  // Allocate each of the lookaside completion cookie arrays.  They have the same number of slots
  // as the inj fifo.
  for( fifo=0; fifo<numInjFifos; fifo++ )
  {
    allocateMemory( NULL, // Use heap
                    (void **)&(_lookAsideCompletionCookiePtrs[fifo]),
                    16,
                    lookAsideCompletionCookieArraySize );
    TRACE_FORMAT("MU ResourceManager: allocateLookasideResources: lookAsideCompletionCookiePtrs[%u] = %p",fifo,_lookAsideCompletionCookiePtrs[fifo]);
    PAMI_assertf ( _lookAsideCompletionCookiePtrs[fifo] != NULL, "The heap is full.\n" );
    memset( _lookAsideCompletionCookiePtrs[fifo], 0x00, lookAsideCompletionCookieArraySize );
  }
  TRACE_FN_EXIT();
} // End: allocateLookasideResources()


/// \brief Initialize the Comm Agent
void PAMI::Device::MU::ResourceManager::allocateGlobalCommAgent()
{
  int rc;

  _commAgentActive  = __global.isCommAgentRunning();

  _commAgentControl = __global.getCommAgentControlStruct();

  if( _commAgentActive )
  {
    rc = CommAgent_RemoteGetPacing_Init ( _commAgentControl,
                                          (CommAgent_RemoteGetPacing_SharedMemoryInfo_t *)NULL );
    PAMI_assertf(rc == 0, "CommAgent_RemoteGetPacing_Init failed with rc=%d\n",rc);

    rc = CommAgent_Fence_Init ( _commAgentControl );
    PAMI_assertf(rc == 0, "CommAgent_Fence_Init failed with rc=%d\n",rc);

    // Save away the comm agent's reception fifo ID.
    _globalCommAgentRecFifoId = CommAgent_GetRecFifoId( _commAgentControl );
  }
}


// \brief Allocate Global Resources
//
// Allocate resources needed before main().  These include:
// - 10 remote get injection fifos (8 in subgroup 64, 2 in subgroup 65).
// -  1 collective combining injection fifo (in subgroup 65).
// -  1 base address table entry in subgroup 64 or 65, initialized to 0.
// -  1 base address table entry in subgroup 64 or 65, initialized to the atomic
//    address of a shared counter, whose value is ignored.
// -  3 base address table entries in subgroup 64 or 65 for collective broadcasts.
// -  N reception fifos, where N is in the range 1..256.  All available reception
//    fifos will be allocated at this time.
// -  Array of clientResources_t structures, one per client.
// -  Comm Agent
//
void PAMI::Device::MU::ResourceManager::allocateGlobalResources()
{
  TRACE_FN_ENTER();
  size_t numClients = _pamiRM.getNumClients();

  // Prepare to allocate resources that are global to the node
  // - Calculates the amount of memory needed - no resources are allocated.
  //   Then, shared memory is set up so it can be allocated-from.
  _calculateSizeOnly = 1;
  allocateGlobalInjFifos();
  allocateGlobalBaseAddressTableEntries();
  setupSharedMemory();

  // The master process allocates space for AND initializes the resources, while
  // all other processes just allocate space for the resources.  The idea is that
  // all processes on the node perform the allocation so they know what the
  // addresses are...since they allocate the resources in the same sequence, they
  // all get the same addresses.  Only the master actually initializes that storage.
  // When the storage is shared memory, there are multiple processes.  The
  // shared memory allocator doesn't actually write into shared memory to track
  // the storage like malloc does.  It just calculates and returns addresses.
  // Whereas when there is only one process (SMP mode), there is no shared memory,
  // and malloc is used to allocate off the heap.  Since there is only one process,
  // that process will use malloc and initialize the storage.  There are no
  // processes that only allocate the storage.

  _calculateSizeOnly = 0;
  if( _mapping.t() == _mapping.lowestT() ) // Master?
    _allocateOnly = 0;
  else
    _allocateOnly = 1;

  // Initialize the comm agent.  Do this before allocating PAMI global resources
  // so the comm agent's inj fifos and rec fifos get allocated first on all
  // nodes.  This way, the comm agent's fifos get the same IDs on all nodes,
  // so we can target them.
  allocateGlobalCommAgent();

  allocateGlobalInjFifos();
  allocateGlobalBaseAddressTableEntries();

  // Allocate process-scoped resources.
  _calculateSizeOnly = 0;
  _allocateOnly      = 0;
  allocateGlobalRecFifos();

  // Allocate space for the client resources array
  pami_result_t prc;
  prc = __global.heap_mm->memalign((void **)&_clientResources, 0,
                                   numClients * sizeof(*_clientResources));
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _clientResources failed");

  // Allocate space for the perCorePerProcessPamiResources array
  prc = __global.heap_mm->memalign((void **)&_perCorePerProcessPamiResources, 0,
                                   numClients * sizeof(*_perCorePerProcessPamiResources));
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _perCorePerProcessPamiResources failed");

  // Allocate space for the perContextMUResources array
  prc = __global.heap_mm->memalign((void **)&_perContextMUResources, 0,
                                   numClients * sizeof(*_perContextMUResources));
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _perContextMUResources failed");

  // If we are the master process, set the inj and rec fifo thresholds.
  // Do this after waiting for the comm agent to initialize so both of us are not
  // trying to set these at the same time.

  if( _mapping.t() == _mapping.lowestT() ) // Master?
  {
    // If the MU Inj Fifo Interrupt Threshold has not been set yet (by a non-PAMI user),
    // then set it.  We want the setting to be 95% of the fifo's size so the interrupt
    // only fires when the fifo is nearly empty.  The interrupt will fire when the 
    // fifo head or tail is moved AND the free space exceeds this threshold.
    uint64_t threshold;
    int32_t  rc;
    rc = Kernel_GetInjFifoThresholds( &threshold,
                                      NULL /* remoteGetThreshold */ );
    PAMI_assertf( rc == 0, "Kernel_GetInjFifoThresholds failed with rc=%d\n",rc);

    if( threshold == 0 ) // Not set yet?
    {
      // Calculate threshold in terms of number of descriptors.
      threshold = _pamiRM.getInjFifoSize() * 0.95 / sizeof(MUHWI_Descriptor_t);
      rc = Kernel_ConfigureInjFifoThresholds( &threshold,
                                              NULL /* remoteGetThreshold */ );
      PAMI_assertf( rc == 0, "Kernel_ConfigureInjFifoThresholds failed with rc=%d\n",rc);
    }
    TRACE_FORMAT("MU ResourceManager: allocateGlobalResources: InjFifoThreshold is set to %lu",threshold);

    // If the MU Rec Fifo Interrupt Threshold has not been set yet (by a non-PAMI user),
    // then set it.  We want it such that if the free space drops below this
    // threshold, an interrupt will fire so the packets can be processed.
    // Set it to two-thirds the size of the rec fifo (in units of number of 32-byte chunks).

    rc = Kernel_GetRecFifoThreshold( &threshold );
    PAMI_assertf( rc == 0, "Kernel_GetRecFifoThresholds failed with rc=%d\n",rc);

    if( threshold == 0 ) // Not set yet?
    {
      threshold = (_pamiRM.getRecFifoSize() * 2) / (3 * 32);
      rc = Kernel_ConfigureRecFifoThreshold( threshold );
      PAMI_assertf( rc == 0, "Kernel_ConfigureRecFifoThreshold failed with rc=%d\n",rc);
    }
    TRACE_FORMAT("MU ResourceManager: allocateGlobalResources:  RecFifoThreshold is set to %lu",threshold);
  }

  allocateMemory( "GIBarrier_inited_table" /*useSharedMemory*/, (void **)(&_inited), 8, sizeof(uint8_t)*NumClassRoutes );
  PAMI_assertf( _inited != NULL, "Shared memory is full.\n" );
  //memset (_inited, 0, sizeof(uint8_t)*NumClassRoutes);
  TRACE_FN_EXIT();
} // End: allocateGlobalResources()


void PAMI::Device::MU::ResourceManager::allocateContextResources( size_t rmClientId,
                                                                  size_t contextOffset )
{
  TRACE_FN_ENTER();
  uint32_t numFifosSetup;
  size_t   numInjFifos = _perContextMUResources[rmClientId].numInjFifos;
  size_t   numRecFifos = _perContextMUResources[rmClientId].numRecFifos;
  size_t   numBatIds   = _perContextMUResources[rmClientId].numBatIds;
  Kernel_InjFifoAttributes_t  injFifoAttr;

  // Set up the injection fifos for this context

  memset( &injFifoAttr, 0x00, sizeof(injFifoAttr) );

  numFifosSetup = setupInjFifos(
                               _clientResources[rmClientId].startingSubgroupIds[contextOffset],
                               _clientResources[rmClientId].endingSubgroupIds[contextOffset],
                               numInjFifos,
                               _pamiRM.getInjFifoSize(),
                               &injFifoAttr,
                               NULL, // Do not use shared memory...use heap.
                               true,  // Enable interrupts.
                               &(_clientResources[rmClientId].injResources[contextOffset].subgroups),
                               &(_clientResources[rmClientId].injResources[contextOffset].fifoPtrs),
                               &(_clientResources[rmClientId].injResources[contextOffset].globalFifoIds) );
  PAMI_assertf( numFifosSetup == numInjFifos, "Only %u injection fifos were set up.  Expected %zu.\n",numFifosSetup,numInjFifos );

  // Set up the reception fifos for this context

  Kernel_RecFifoAttributes_t  recFifoAttr;
  memset( &recFifoAttr, 0x00, sizeof(recFifoAttr) );

  numFifosSetup = setupRecFifos(
                               _clientResources[rmClientId].startingSubgroupIds[contextOffset],
                               _clientResources[rmClientId].endingSubgroupIds[contextOffset],
                               numRecFifos,
                               _pamiRM.getRecFifoSize(),
                               &recFifoAttr,
                               NULL, // Do not use shared memory...use heap.
                               &(_clientResources[rmClientId].recResources[contextOffset].subgroups),
                               &(_clientResources[rmClientId].recResources[contextOffset].fifoPtrs),
                               &(_clientResources[rmClientId].recResources[contextOffset].globalFifoIds) );
  PAMI_assertf( numFifosSetup == numRecFifos, "Only %u reception fifos were set up.  Expected %zu.\n",numFifosSetup,numRecFifos );

  // Set up the base address table entries for this context.
  // Get as many as possible up to the max for this context.
  // There may be none available to set up...that's ok.

  if( numBatIds )
  {
    uint32_t numBatIdsSetup;

    numBatIdsSetup = setupBatIds(
                                _clientResources[rmClientId].startingSubgroupIds[contextOffset],
                                _clientResources[rmClientId].endingSubgroupIds[contextOffset],
                                numBatIds,  // Number of BAT ids
                                false, // Do not use shared memory
                                &(_clientResources[rmClientId].batResources[contextOffset].subgroups),
                                &(_clientResources[rmClientId].batResources[contextOffset].globalBatIds) );

    PAMI_assertf( numBatIdsSetup == numBatIds, "Only %u base address Ids were set up.  Expected %zu.\n",numBatIdsSetup, numBatIds);

    // Set the status of the BATids (free vs allocated) to be free.
    // Turn ON a bit in a 32-bit field for each BATid, indicating free.
    _clientResources[rmClientId].batResources[contextOffset].status =
    (_BN(63-numBatIds) - 1) << (32-numBatIds);
    TRACE_FORMAT("%zu BATids set up, status=0x%08x",numBatIds,_clientResources[rmClientId].batResources[contextOffset].status);
  }

  // For each tcoord, store the adjusted
  // - global rec fifo id in the pinRecFifo array
  // - global bat id in the pinBatId array.  We only store the first global bat id in the group
  //   associated with this context.

  // 1. Find the first subgroup in our process
  size_t tSize = _mapping.tSize();
  size_t t;
  uint64_t threadMask = Kernel_ThreadMask( _mapping.t() );
  PAMI_assertf ( threadMask, "Kernel_ThreadMask returned no threads\n" ); // Cannot be zero
  uint32_t myStartingSubgroup = 0;
  uint64_t mask = 0x8000000000000000ULL;
  while( (threadMask & mask) == 0 )
  {
    myStartingSubgroup++;
    mask = mask >> 1;
  }

  uint16_t myRelativeGlobalFifoId = _clientResources[rmClientId].recResources[contextOffset].globalFifoIds[0] - ( myStartingSubgroup * BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP);

  uint16_t myRelativeGlobalBatId=0;
  if( numBatIds )
    myRelativeGlobalBatId = _clientResources[rmClientId].batResources[contextOffset].globalBatIds[0]  - ( myStartingSubgroup * BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP );

  TRACE_FORMAT("MU ResourceManager: allocateContextResources: rmClientId=%zu, contextOffset=%zu, globalFifoIds[0]=%u, myStartingSubgroup=%u, myRelativeRecFifoGlobalFifoId=%u, myRelativeGlobalBatId=%u",rmClientId,contextOffset,_clientResources[rmClientId].recResources[contextOffset].globalFifoIds[0],myStartingSubgroup,myRelativeGlobalFifoId,myRelativeGlobalBatId);

  for( t=0; t<tSize; t++ )
  {
    // Obtain the list of HW threads for this process (corresponding to subgroups) from the kernel
    // We will use this to determine the starting subgroup for each tcoord.
    // Starting Subgroup * number of rec fifos per subgroup + relativeGlobalFifoId is the
    // global rec fifo id for that tcoord.

    uint64_t threadMask = Kernel_ThreadMask( t );
    PAMI_assertf ( threadMask, "Kernel_ThreadMask returned no threads\n" ); // Cannot be zero

    // Find first subgroup for this t
    uint32_t startingSubgroup = 0;
    uint64_t mask = 0x8000000000000000ULL;
    while( (threadMask & mask) == 0 )
    {
      startingSubgroup++;
      mask = mask >> 1;
    }

    _clientResources[rmClientId].pinRecFifo[(contextOffset*_tSize)+t] =
    (startingSubgroup * BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP) + myRelativeGlobalFifoId;

    if( numBatIds)
      _clientResources[rmClientId].pinBatId[(contextOffset*_tSize)+t] =
      (startingSubgroup * BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP) + myRelativeGlobalBatId;

    TRACE_FORMAT("MU ResourceManager: allocateContextResources: startingSubgroup=%u, pinRecFifo,pinBatId[t=%zu][contextOffset=%zu] = %u,%u",startingSubgroup,t,contextOffset,_clientResources[rmClientId].pinRecFifo[(contextOffset*_tSize) + t],_clientResources[rmClientId].pinBatId[(contextOffset*_tSize) + t]);
  }

  allocateLookasideResources( numInjFifos,
                              _pamiRM.getInjFifoSize(),

                              &(_clientResources[rmClientId].
                                injResources[contextOffset].
                                lookAsidePayloadPtrs),

                              &(_clientResources[rmClientId].
                                injResources[contextOffset].
                                lookAsidePayloadMemoryRegions),

                              &(_clientResources[rmClientId].
                                injResources[contextOffset].
                                lookAsidePayloadPAs),

                              &(_clientResources[rmClientId].
                                injResources[contextOffset].
                                lookAsideCompletionFnPtrs),

                              &(_clientResources[rmClientId].
                                injResources[contextOffset].
                                lookAsideCompletionCookiePtrs));
  TRACE_FN_EXIT();
} // End: allocateContextResources()


void PAMI::Device::MU::ResourceManager::initializeContexts( size_t rmClientId,
                                                            size_t numContexts,
                                                            PAMI::Device::Generic::Device * devices )
{
  TRACE_FN_ENTER();
  size_t i;

  _clientResources[rmClientId].numContexts = numContexts;

  pami_result_t prc;
  prc = __global.heap_mm->memalign((void **)&_clientResources[rmClientId].pinRecFifo, 0,
                                   _mapping.tSize() * numContexts * sizeof(uint16_t));
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _clientResources[%zd].pinRecFifo failed", rmClientId);

  prc = __global.heap_mm->memalign((void **)&_clientResources[rmClientId].pinRecFifoPtrs, 0,
                                   numContexts * sizeof(uint16_t*));
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _clientResources[%zd].pinRecFifoPtrs failed", rmClientId);

  prc = __global.heap_mm->memalign((void **)&_clientResources[rmClientId].startingSubgroupIds, 0,
                                   numContexts * sizeof(*_clientResources[rmClientId].startingSubgroupIds));
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _clientResources[%zd].startingSubgroupIds failed", rmClientId);

  prc = __global.heap_mm->memalign((void **)&_clientResources[rmClientId].endingSubgroupIds, 0,
                                   numContexts * sizeof(*_clientResources[rmClientId].endingSubgroupIds));
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _clientResources[%zd].endingSubgroupIds failed", rmClientId);

  prc = __global.heap_mm->memalign((void **)&_clientResources[rmClientId].injResources, 0,
                                   numContexts * sizeof(*_clientResources[rmClientId].injResources));
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _clientResources[%zd].injResources failed", rmClientId);

  prc = __global.heap_mm->memalign((void **)&_clientResources[rmClientId].recResources, 0,
                                   numContexts * sizeof(*_clientResources[rmClientId].recResources));
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _clientResources[%zd].recResources failed", rmClientId);

  prc = __global.heap_mm->memalign((void **)&_clientResources[rmClientId].batResources, 0,
                                   numContexts * sizeof(batResources_t) );
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _clientResources[%zd].batResources failed", rmClientId);

  // Determine the number of contexts per core needed by this client.
  size_t numCoresPerProcess = _pamiRM.getNumCoresPerProcess();
  size_t numContextsPerCore = ( numContexts + numCoresPerProcess -1 ) / numCoresPerProcess;
  _perCorePerProcessPamiResources[rmClientId].numContexts = numContextsPerCore;

  TRACE_FORMAT("MU ResourceManager: initializeContexts: RmClientId=%zu, numCoresPerProcess = %zu, numContextsPerCore = %zu",rmClientId,numCoresPerProcess,numContextsPerCore);

  _perContextMUResources[rmClientId].numInjFifos =
  _perCorePerProcessPerClientMUResources[rmClientId].numInjFifos / numContextsPerCore;
  _perContextMUResources[rmClientId].numRecFifos =
  _perCorePerProcessPerClientMUResources[rmClientId].numRecFifos / numContextsPerCore;
  _perContextMUResources[rmClientId].numBatIds =
  _perCorePerProcessPerClientMUResources[rmClientId].numBatIds / numContextsPerCore;

  PAMI_assertf( _perContextMUResources[rmClientId].numInjFifos, "Not enough injection fifos are available\n" );
  PAMI_assertf( _perContextMUResources[rmClientId].numRecFifos, "Not enough reception fifos are available\n" );
  // Don't assert for numBatIds, because we allow it to be zero

  TRACE_FORMAT("MU ResourceManager: initializeContexts: Each context for client %zu gets %zu injFifos, %zu recFifos, %zu BatIds",rmClientId, _perContextMUResources[rmClientId].numInjFifos, _perContextMUResources[rmClientId].numRecFifos,_perContextMUResources[rmClientId].numBatIds);

  // Adjust the number of fifos needed if they exceed the max that we need.
  if( _perContextMUResources[rmClientId].numInjFifos > optimalNumInjFifosPerContext )
    _perContextMUResources[rmClientId].numInjFifos = optimalNumInjFifosPerContext;
  if( _perContextMUResources[rmClientId].numRecFifos > optimalNumRecFifosPerContext )
    _perContextMUResources[rmClientId].numRecFifos = optimalNumRecFifosPerContext;

  TRACE_FORMAT("MU ResourceManager: initializeContexts: Each context for client %zu gets %zu injFifos and %zu recFifos after lowering to needed limits",rmClientId, _perContextMUResources[rmClientId].numInjFifos, _perContextMUResources[rmClientId].numRecFifos);

  prc = __global.heap_mm->memalign((void **)&_clientResources[rmClientId].pinBatId, 0,
                                   _mapping.tSize() * numContexts * sizeof(uint16_t) );
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _clientResources[%zd].pinBatId failed", rmClientId);

  // Obtain the list of HW threads for this process (corresponding to subgroups) from the kernel
  // We will use this to determine which subgroups to allocate resources from.
  // Start with the last core in this process and round robin backwards from there.

  uint64_t threadMask = Kernel_ThreadMask( _mapping.t() );
  PAMI_assertf ( threadMask, "Kernel_ThreadMask returned no threads\n" ); // Cannot be zero

  // Round robin among the cores in this process, creating one context each time.

  uint64_t subgroupMask     = 0x1ULL; // Start with the last thread on core 15.
  uint32_t startingSubgroup, endingSubgroup=63;

  for( i=0; i<numContexts; i++ )
  {
    // Save away the pointer to the pinRecFifo array for each context.
    // This will improve performance during fifo pin so these calculations
    // don't need to be done at runtime.
    _clientResources[rmClientId].pinRecFifoPtrs[i] = 
    &_clientResources[rmClientId].pinRecFifo[i*_tSize];

    // Find the next subgroup where the context is to be created.
    while( ( threadMask & subgroupMask ) == 0 )
    {
      endingSubgroup--;
      subgroupMask = subgroupMask << 1;  // Shift to next subgroup.
      if( subgroupMask == 0 )
      {
        endingSubgroup = 63;
        subgroupMask = 0x1ULL; // Wrap
      }
    }
    // Determine starting subgroup.  Don't go beyond the core boundary (multiple of 4).
    subgroupMask = subgroupMask << 1; // Shift to next subgroup.
    startingSubgroup = endingSubgroup - 1;
    while( (( startingSubgroup & 0x3 ) != 3) && ( threadMask & subgroupMask ) )
    {
      startingSubgroup--;
      subgroupMask = subgroupMask << 1;
    }
    startingSubgroup++;  // Adjust, since we went 1 beyond.

    _clientResources[rmClientId].startingSubgroupIds[i] = startingSubgroup;
    _clientResources[rmClientId].endingSubgroupIds[i]   = endingSubgroup;

    TRACE_FORMAT("MU ResourceManager: initializeContexts: Creating context %zu in starting subgroup %u, endingSubgroup %u",i,startingSubgroup,endingSubgroup);

    allocateContextResources( rmClientId, i );

    // Move to next core
    if( subgroupMask == 0)
    {
      endingSubgroup = 63;
      subgroupMask = 0x1ULL; // Wrap
    }
    else
    {
      endingSubgroup = startingSubgroup - 1; // Note: subgroupMask is already here.
      while( ( endingSubgroup & 0x3 ) != 3 )
      {
        endingSubgroup--;
        subgroupMask = subgroupMask << 1;
      }
      if( subgroupMask == 0)
      {
        endingSubgroup = 63;
        subgroupMask = 0x1ULL; // Wrap
      }
    }
  }
  prc = __global.heap_mm->memalign((void **)&_cr_mtx_mdls[rmClientId], sizeof(void *),
                                   numContexts * sizeof(*_cr_mtx_mdls[rmClientId]));
  PAMI_assert_alwaysf(prc == PAMI_SUCCESS, "Failed to allocate space for classroute mutex models");
  TRACE_FN_EXIT();
} // End: initializeContexts()

void PAMI::Device::MU::ResourceManager::init(size_t rmClientId,
                                             size_t rmContextId,
                                             PAMI::Device::Generic::Device * devices)
{
  pami_result_t status = PAMI_ERROR;
  AtomicMutexDev &device = PAMI::Device::AtomicMutexDev::Factory::getDevice((AtomicMutexDev *)devices, rmClientId, rmContextId);
  new (&_cr_mtx_mdls[rmClientId][rmContextId]) MUCR_mutex_model_t(device, &_cr_mtx, status);
  PAMI_assert_alwaysf(status == PAMI_SUCCESS, "Failed to construct non-blocking mutex client %zd context %zd", rmClientId, rmContextId);
}


void PAMI::Device::MU::ResourceManager::getNumResourcesPerContext( size_t  rmClientId,
                                                                   size_t *numInjFifos,
                                                                   size_t *numRecFifos,
                                                                   size_t *numBatIds )
{
  *numInjFifos = _perContextMUResources[rmClientId].numInjFifos;
  *numRecFifos = _perContextMUResources[rmClientId].numRecFifos;
  *numBatIds   = _perContextMUResources[rmClientId].numBatIds;

} // End: getNumResourcesPerContext()


void PAMI::Device::MU::ResourceManager::getInjFifosForContext( size_t            rmClientId,
                                                               size_t            contextOffset,
                                                               size_t            numInjFifos,
                                                               MUSPI_InjFifo_t **injFifoPtrs,
                                                               uint32_t         *globalFifoIds )
{
  TRACE_FN_ENTER();
  PAMI_assert( numInjFifos <= _perContextMUResources[rmClientId].numInjFifos );

  size_t fifo;
  for( fifo=0; fifo<numInjFifos; fifo++ )
  {
    uint32_t globalFifoId     =
    _clientResources[rmClientId].injResources[contextOffset].globalFifoIds[fifo];
    uint32_t globalSubgroup   = globalFifoId / BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP;
    uint32_t relativeSubgroup = globalSubgroup -
                                _clientResources[rmClientId].startingSubgroupIds[contextOffset];
    uint32_t relativeFifo     = globalFifoId -
                                (globalSubgroup * BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP);

    injFifoPtrs[fifo] =
    &( _clientResources[rmClientId].
       injResources[contextOffset].
       subgroups[relativeSubgroup].
       _injfifos[relativeFifo] );

    globalFifoIds[fifo] = globalFifoId;

    TRACE_FORMAT("MU ResourceManager: getInjFifosForContext: context=%zu, globalFifoId=%u, globalSubgroup=%u, relativeSubgroup=%u, startingSubgroupId=%u, relativeFifo=%u, injFifoStructurePtr=%p, hwinjfifo=%p",contextOffset,globalFifoId,globalSubgroup,relativeSubgroup,_clientResources[rmClientId].startingSubgroupIds[contextOffset],relativeFifo,injFifoPtrs[fifo],injFifoPtrs[fifo]->hw_injfifo);
  }
  TRACE_FN_EXIT();
} // End: getInjFifosForContext()


void PAMI::Device::MU::ResourceManager::getRecFifosForContext( size_t            rmClientId,
                                                               size_t            contextOffset,
                                                               size_t            numRecFifos,
                                                               MUSPI_RecFifo_t **recFifoPtrs,
                                                               uint32_t         *globalFifoIds )
{
  TRACE_FN_ENTER();
  PAMI_assert( numRecFifos <= _perContextMUResources[rmClientId].numRecFifos );

  size_t fifo;
  for( fifo=0; fifo<numRecFifos; fifo++ )
  {
    uint32_t globalFifoId     =
    _clientResources[rmClientId].recResources[contextOffset].globalFifoIds[fifo];
    uint32_t globalSubgroup   = globalFifoId / BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP;
    uint32_t relativeSubgroup = globalSubgroup - _clientResources[rmClientId].startingSubgroupIds[contextOffset];
    uint32_t relativeFifo     = globalFifoId -
                                (globalSubgroup * BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP);

    recFifoPtrs[fifo] =
    &( _clientResources[rmClientId].
       recResources[contextOffset].
       subgroups[relativeSubgroup].
       _recfifos[relativeFifo] );

    globalFifoIds[fifo] = globalFifoId;

    TRACE_FORMAT("MU ResourceManager: getRecFifosForContext: globalFifoId=%u, globalSubgroup=%u, relativeSubgroup=%u, relativeFifo=%u, recFifoStructurePtr=%p, globalFifoId=%u",globalFifoId,globalSubgroup,relativeSubgroup,relativeFifo,recFifoPtrs[fifo],globalFifoIds[fifo]);
  }
  TRACE_FN_EXIT();
} // End: getRecFifosForContext()


/// \brief Query the Number of Free BAT IDs Within A Context
///
/// \retval  numFree
uint32_t PAMI::Device::MU::ResourceManager::queryFreeBatIdsForContext( size_t    rmClientId,
                                                                       size_t    contextOffset )
{
  TRACE_FN_ENTER();
  size_t numBatIds     = _perContextMUResources[rmClientId].numBatIds;
  size_t numFreeBatIds = 0;
  size_t i;
  uint32_t status = _clientResources[rmClientId].batResources[contextOffset].status;

  for( i=0; i<numBatIds; i++ )
  {
    if( status & 0x80000000 ) numFreeBatIds++;
    status = status << 1;
  }

  TRACE_FORMAT("MU ResourceManager: queryFreeBatIdsForContext: numBatIds=%zu, numFreeBatIds=%zu",numBatIds, numFreeBatIds);

  TRACE_FN_EXIT();
  return numFreeBatIds;
}// End: queryFreeBatIdsForContext()


/// \brief Allocate (reserve) BAT IDs Within A Context
///
/// \retval  0  Success
/// \retval  -1 Failed to allocate the specified number of BatIds.
///             No IDs were actually allocated.
int32_t PAMI::Device::MU::ResourceManager::allocateBatIdsForContext( size_t    rmClientId,
                                                                     size_t    contextOffset,
                                                                     size_t    numBatIds,
                                                                     uint16_t *globalBatIds )
{
  TRACE_FN_ENTER();
  size_t numContextBatIds = _perContextMUResources[rmClientId].numBatIds;

  size_t numFreeBatIds = queryFreeBatIdsForContext( rmClientId,
                                                    contextOffset );

  if( numFreeBatIds < numBatIds ) 
  {
    TRACE_FN_EXIT();
    return -1; // Not enough free BAT IDs.
  }

  size_t i, j=0;
  uint32_t status = _clientResources[rmClientId].batResources[contextOffset].status;

  for( i=0; (i<numContextBatIds) && (numBatIds>0); i++ )
  {
    if( status & 0x80000000 ) // Is the i'th BAT ID free?
    {
      _clientResources[rmClientId].batResources[contextOffset].status &= ~(1<<(31-i)); // Mark allocated.
      globalBatIds[j++] =  _clientResources[rmClientId].batResources[contextOffset].globalBatIds[i];
      numBatIds--;

      TRACE_FORMAT("MU ResourceManager: allocateBatIdsForContext: allocated batId %zu, globalBatId=%u, newStatus=0x%08x",i,globalBatIds[j-1],_clientResources[rmClientId].batResources[contextOffset].status);
    }
    status = status << 1;
  }
  TRACE_FN_EXIT();
  return 0;
} // End: allocateBatIdsForContext()


/// \brief Free (unreserve) BAT IDs Within A Context
///
void PAMI::Device::MU::ResourceManager::freeBatIdsForContext( size_t    rmClientId,
                                                              size_t    contextOffset,
                                                              size_t    numBatIds,
                                                              uint16_t *globalBatIds )
{
  TRACE_FN_ENTER();
  size_t numContextBatIds = _perContextMUResources[rmClientId].numBatIds;

  PAMI_assert( numBatIds <= numContextBatIds );

  size_t i, j;

  for( i=0; i<numBatIds; i++ ) // Loop through the caller's IDs to free
  {
    for( j=0; j<numContextBatIds; j++ ) // Loop through the context's BAT IDs.
    {
      if( _clientResources[rmClientId].batResources[contextOffset].globalBatIds[j] ==
          globalBatIds[i] ) // Found it?
      {
        _clientResources[rmClientId].batResources[contextOffset].status |= _BN(j)>>32; // Mark free
        TRACE_FORMAT("MU ResourceManager: freeBatIdsForContext: freed BATid %zu, globalBatId=%u, newStatus=0x%08x",j,globalBatIds[i],_clientResources[rmClientId].batResources[contextOffset].status);
      }
    }
  }
  TRACE_FN_EXIT();
} // End: freeBatIdsForContext()


int32_t PAMI::Device::MU::ResourceManager::setBatEntryForContext ( size_t    rmClientId,
                                                                   size_t    contextOffset,
                                                                   uint16_t  globalBatId,
                                                                   uint64_t  value )
{
  TRACE_FN_ENTER();
  uint32_t startingSubgroupForContext =
  _clientResources[rmClientId].startingSubgroupIds[contextOffset];
  uint32_t relativeSubgroupIndex = ( (globalBatId -
                                      (startingSubgroupForContext *
                                       BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP)) /
                                     BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP );
  uint8_t  relativeBatId = globalBatId % BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP;

  TRACE_FORMAT("MU ResourceManager: setBatEntryForContext: Setting globalBatId %u to 0x%lx, startingSubgroup=%u, subgroupIndex=%u, relativeBatId=%u",globalBatId,value,startingSubgroupForContext,relativeSubgroupIndex,relativeBatId);

  TRACE_FN_EXIT();
  return MUSPI_SetBaseAddress ( &_clientResources[rmClientId].batResources[contextOffset].subgroups[relativeSubgroupIndex],
                                relativeBatId,
                                value );
} // End: setBatEntryForContext()


#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#endif   // __components_devices_bgq_mu_global_ResourceManager_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
