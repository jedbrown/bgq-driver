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
 * \file components/devices/bgq/mu2/Factory.h
 * \brief ???
 */

#ifndef __components_devices_bgq_mu2_Factory_h__
#define __components_devices_bgq_mu2_Factory_h__

#include "Global.h"

#include "components/devices/FactoryInterface.h"
#include "components/devices/bgq/mu2/global/Global.h"
#include "components/devices/bgq/mu2/Context.h"
#include "components/devices/generic/Device.h"
#include "components/atomic/native/NativeCounter.h"
#include "components/atomic/counter/CounterBarrier.h"
#include "components/atomic/indirect/IndirectBarrier.h"
#include "spi/include/kernel/gi.h"
#include "spi/include/mu/GIBarrier.h"

#ifdef TRACE
#undef TRACE
#endif
#define TRACE(x) //fprintf x

extern PAMI::Device::MU::Global     __MUGlobal;
extern void * __mu_context_cache;

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      class Factory : public Interface::FactoryInterface<MU::Factory, MU::Context, PAMI::Device::Generic::Device>
      {
        public:

          static inline size_t getInstanceMemoryRequired_impl (size_t clientid,
                                                               Memory::MemoryManager & mm)
          {
            return 0;
          };

          static inline size_t getInstanceMaximum_impl (size_t clientid, Memory::MemoryManager &mm)
          {
            ////////////////////////////////////////////////////////////////////////////////
            /// \envs{pami,mudevice,MU Device}
            ///
            /// \env{mudevice,PAMI_MU_RESOURCES}
            /// Determines if PAMI calculates the number
            /// of available contexts based on an 'optimal' or a 'minimal'
            /// allocation of MU resources to each context.
            ///
            /// - An \e optimial allocation of MU resources to each context will
            ///   limit the maximum number contexts that may be created, and
            ///   each context will be allocated sufficient MU resources to
            ///   fully utilize the MU hardware and torus network.
            /// - A \e minimal allocation of MU resources to each context will
            ///   allow the \e maximum number of contexts to be created regardless
            ///   of MU hardware and torus network considerations.
            ///
            /// Supported environment variable values are case insensitive
            /// and include
            /// - Optimal
            /// - Minimal
            ///
            /// \default Optimal
            char *env = getenv("PAMI_MU_RESOURCES");

            if (env)
              {
                if (env[0] == 'm' || env[0] == 'M')
                  return __MUGlobal.getMuRM().getPerProcessMaxPamiResources (clientid);
              }

            return __MUGlobal.getMuRM().getPerProcessOptimalPamiResources (clientid);
          };

          ///
          /// \copydoc Interface::Factory::generate
          ///
          static inline MU::Context * generate_impl (size_t                          id_client,
                                                     size_t                          id_count,
                                                     Memory::MemoryManager         & mm,
                                                     PAMI::Device::Generic::Device * devices)
          {
            int32_t rc;
            size_t myA, myB, myC, myD, myE, myT, T, tSize;
            myA = __global.mapping.a();
            myB = __global.mapping.b();
            myC = __global.mapping.c();
            myD = __global.mapping.d();
            myE = __global.mapping.e();
            myT = __global.mapping.t();
            tSize = __global.mapping.tSize();
            size_t addr[BGQ_TDIMS + BGQ_LDIMS];
            addr[0] = myA;
            addr[1] = myB;
            addr[2] = myC;
            addr[3] = myD;
            addr[4] = myE;
            // Note: addr[5] is the T coord and will be set later.

            // Set up a local barrier (needed below).
            bool master;
            size_t numLocalTasks = __global.topology_local.size();
            ( myT == __global.mapping.lowestT() ) ? master = true : master = false;

            TRACE((stderr, "MU::Context::generate_impl: myCoords=(%zu,%zu,%zu,%zu,%zu,%zu)\n", myA, myB, myC, myD, myE, myT));
            TRACE((stderr, "MU::Context::generate_impl: Initializing local barrier, size=%zu, master=%d\n", numLocalTasks, master));

            PAMI::Barrier::IndirectCounter<PAMI::Counter::Indirect<PAMI::Counter::Native> > barrier(numLocalTasks, master);
            char key[PAMI::Memory::MMKEYSIZE];
            sprintf(key, "/pami-mu2-rm%zd", id_client);
            barrier.init(&__global.mm, key);//,

            //
            // The MU resource manager requires that only one task on the local node
            // initialize their contexts at a time to prevent race conditions
            // in the MU SPIs.  To enforce this, loop through each task on the
            // local node performing a local barrier.  During each loop iteration,
            // only one task performs the init.

            TRACE((stderr, "MU Factory: Entering local barrier before context create loop\n"));
            barrier.enter();
            TRACE((stderr, "MU Factory: Exiting  local barrier before context create loop\n"));

            for ( T = 0; T < tSize; T++)
              {
                // Check if current T exists.  If not, move to next T
                pami_result_t rc;
                size_t dummyTask;
                addr[5] = T;
                rc = __global.mapping.global2task ( addr, dummyTask );

                if ( rc != PAMI_SUCCESS ) continue; // Move to next T...this one does not have a task.

                // We found a T that has a task on it.  If it is our task, initialize it.
                if ( myT == T ) // Is it my turn to initialize?
                  {
                    TRACE((stderr, "MU Factory: Initializing contexts for T=%zu\n", T));
                    // Initialize the MU resources for all contexts for this client
                    __MUGlobal.getMuRM().initializeContexts( id_client, id_count, devices );
                  }

                TRACE((stderr, "MU Factory: Entering local barrier. Initializing T=%zu\n", T));
                barrier.enter();
                TRACE((stderr, "MU Factory: Exiting  local barrier. Initializing T=%zu\n", T));
              }

            // Allocate an array of mu contexts, one for each pami context
            // in this _task_ (from heap, not from shared memory)
            MU::Context * mu;
            pami_result_t res;
            res = __global.heap_mm->memalign((void **) & mu, 64, sizeof(*mu) * id_count);
            PAMI_assertf(res == PAMI_SUCCESS, "alloc failed for mu[%zu]\n", id_count);

// !!!!
// need to determine base context identifier
// !!!!
            size_t id_offset, id_base = 0;

            // Instantiate the mu context objects
            for (id_offset = 0; id_offset < id_count; ++id_offset)
              {
                new (&mu[id_offset])
                MU::Context (__global.mapping, id_base, id_offset, id_count);
              }

            // Barrier among all nodes to ensure that the resources for this client
            // are ready on all of the nodes.
            // - Barrier among the processes on this node
            // - Barrier among other nodes
            // - Barrier among the processes on this node
            TRACE((stderr, "MU Factory: Entering local barrier after creating contexts\n"));
            barrier.enter();
            TRACE((stderr, "MU Factory: Exiting  local barrier after creating contexts\n"));


            // If multi-node, and master, need to barrier
            if ( master && (__global.mapping.numActiveNodes() > 1) )
              {
#ifdef ENABLE_MAMBO_WORKAROUNDS

                if (!__global.personality._is_mambo)
#endif
                  {
                    // Do an MU barrier
                    uint32_t classRouteId = 0;
                    MUSPI_GIBarrier_t commworld_barrier;
                    rc = Kernel_GetGlobalBarrierUserClassRouteId( &classRouteId );
                    PAMI_assert(rc == 0);
                    rc = MUSPI_GIBarrierInit ( &commworld_barrier,
                                               classRouteId );
                    PAMI_assert(rc == 0);
                    TRACE((stderr, "MU Factory: enter global barrier on class route %u\n", classRouteId));
                    rc = MUSPI_GIBarrierEnterAndWait ( &commworld_barrier );
                    PAMI_assert(rc == 0);
                  }
#ifdef ENABLE_MAMBO_WORKAROUNDS
                else
                  {
                    double seconds = 20; // wait 20 pseudo-seconds
                    double dseconds = ((double)seconds) / 1000; //mambo seconds are loooong.
                    double start = PAMI_Wtime (), d = 0;
                    TRACE((stderr, "%s sleep - %.0f,start %f < %f\n", __PRETTY_FUNCTION__, d, start, start + dseconds));

                    while (PAMI_Wtime() < (start + dseconds))
                      {
                        for (int i = 0; i < 200000; ++i) ++d;

                        TRACE((stderr, "%s sleep - %.0f, %f < %f\n", __PRETTY_FUNCTION__, d, PAMI_Wtime(), start + dseconds));
                      }

                    TRACE((stderr, "%s sleep - %.0f, start %f, end %f\n", __PRETTY_FUNCTION__, d, start, PAMI_Wtime()));
                  }

#endif
                TRACE((stderr, "MU Factory: exit global barier\n"));
              }

            TRACE((stderr, "MU Factory: Entering Local Barrier after global barrier\n"));
            barrier.enter();
            TRACE((stderr, "MU Factory: Exiting Local Barrier after global barrier\n"));

            __mu_context_cache = mu;

            return mu;
          };

          ///
          /// \copydoc Interface::Factory::init
          ///
          static inline pami_result_t init_impl (MU::Context           * devices,
                                                 size_t                  clientid,
                                                 size_t                  contextid,
                                                 pami_client_t           client,
                                                 pami_context_t          context,
                                                 Memory::MemoryManager * mm,
                                                 Generic::Device       * progress)
          {
            return getDevice_impl (devices, clientid, contextid).init (clientid, contextid, context, progress);
          };

          ///
          /// \copydoc Interface::Factory::advance
          ///
          static inline size_t advance_impl (MU::Context * devices,
                                             size_t        clientid,
                                             size_t        contextid)
          {
            return getDevice_impl (devices, clientid, contextid).advance ();
          };

          ///
          /// \copydoc Interface::Factory::getDevice
          ///
          static inline MU::Context & getDevice_impl (MU::Context * devices,
                                                      size_t     clientid,
                                                      size_t     contextid)
          {
            return devices[contextid];
          };
      }; // class     PAMI::Device::MU::Factory
    };   // namespace PAMI::Device::MU
  };     // namespace PAMI::Device
};       // namespace PAMI


#endif // __components_devices_bgq_mu2_Factory_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
