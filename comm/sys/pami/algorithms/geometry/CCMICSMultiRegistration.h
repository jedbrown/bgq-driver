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
 * \file algorithms/geometry/CCMICSMultiRegistration.h
 * \brief Shared memory collectives over multi* interface
 */

#ifndef __algorithms_geometry_CCMICSMultiRegistration_h__
#define __algorithms_geometry_CCMICSMultiRegistration_h__

#include <map>
#include <vector>
#include "components/memory/MemoryManager.h"
#include "TypeDefs.h"
#include "components/devices/cshmem/CollShmDevice.h"
#include "components/devices/NativeInterface.h"
#include "algorithms/interfaces/CollRegistrationInterface.h"
#include "algorithms/protocols/allreduce/CSMultiCombineComposite.h"
#include "algorithms/protocols/broadcast/CSMultiCastComposite.h"
#include "algorithms/protocols/barrier/CSMultiSyncComposite.h"
#include "algorithms/protocols/AllSidedCSProtocolFactoryT.h"

#undef TRACE_ERR
#define TRACE_ERR(x) //fprintf x

namespace CCMI
{
  namespace Adaptor
  {
    namespace Allreduce
    {

      void csmcomb_reduce_md(pami_metadata_t *m)
      {
        // \todo:  fill in other metadata
        strcpy(&m->name[0],"ShmemMultiCombConverged<");
      }

      typedef CCMI::Adaptor::AllSidedCSProtocolFactoryT<CSMultiCombineComposite, csmcomb_reduce_md> CSMultiCombineFactory;
    }; // Allreduce

    namespace Broadcast
    {

      void csmcast_broadcast_md(pami_metadata_t *m)
      {
        // \todo:  fill in other metadata
        strcpy(&m->name[0],"ShmemMultiCastConverged");
      }

      typedef CCMI::Adaptor::AllSidedCSProtocolFactoryT<CSMultiCastComposite, csmcast_broadcast_md> CSMultiCastFactory;
    }; // Broadcast

    namespace Barrier
    {

      void csmsync_barrier_md(pami_metadata_t *m)
      {
        // \todo:  fill in other metadata
        strcpy(&m->name[0],"ShmemMultiSyncConverged");
      }

      typedef CCMI::Adaptor::AllSidedCSProtocolFactoryT<CSMultiSyncComposite, csmsync_barrier_md> CSMultiSyncFactory;
    }; // Barrier

  }; // Adaptor
}; // CCMI


namespace PAMI
{
  namespace CollRegistration
  {

    template <class T_Geometry, class T_CSNativeInterface, class T_CSMemoryManager, class T_CSModel>
    class CCMICSMultiRegistration :
    public CollRegistration<PAMI::CollRegistration::CCMICSMultiRegistration<T_Geometry,
                                      T_CSNativeInterface, T_CSMemoryManager, T_CSModel>, T_Geometry>
    {
    public:
      inline CCMICSMultiRegistration( pami_client_t                        client,
                                      size_t                               client_id,
                                      pami_context_t                       context,
                                      size_t                               context_id,
                                      PAMI::Device::Generic::Device       &devs,
                                      std::map<unsigned, pami_geometry_t> *geometry_map):
      CollRegistration<PAMI::CollRegistration::CCMICSMultiRegistration<T_Geometry, T_CSNativeInterface,
                                                     T_CSMemoryManager, T_CSModel>, T_Geometry> (),
      _client(client),
      _client_id(client_id),
      _context(context),
      _context_id(context_id),
      _geometry_map(geometry_map),
      _devs(devs)
      {
        TRACE_ERR((stderr, "<%p>%s\n", this, __PRETTY_FUNCTION__));
        //set the mapid functions
        _msync_reg.setMapIdToGeometry(mapidtogeometry);
        // To initialize shared memory, we need to provide the task offset into the
        // local nodes, and the total number of nodes we have locally
        size_t                         task  = __global.mapping.task();
        size_t                         peer;
        size_t                         numpeers;
        __global.mapping.task2peer(task, peer);
        __global.mapping.nodePeers(numpeers);
        TRACE_ERR((stderr, "<%p>CCMICSMultiRegistration() task %zu, peer %zu, numpeers %zu\n", this, task, peer, numpeers));
        _csmm.init(peer,numpeers);
      }

      inline pami_result_t register_local_impl(size_t context_id,T_Geometry *geometry, uint64_t *out, int &n)
      {
        TRACE_ERR((stderr, "<%p>CCMICSMultiRegistration::analyze_local_impl() context %zu, geometry %p, out %p\n", this, context_id, geometry, out));
	n  = 0;
        // only support single context for now
        if (context_id != 0) return PAMI_SUCCESS;

        // This is where we define our contribution to the allreduce
         _csmm.getSGCtrlStrVec(geometry, out);
         return PAMI_SUCCESS;
      }

      inline pami_result_t receive_global_impl(size_t context_id,T_Geometry *geometry, uint64_t *in, int n)
      {
        TRACE_ERR((stderr, "<%p>CCMICSMultiRegistration::analyze_global_impl() context %zu, geometry %p, in %p\n", this, context_id, geometry, in));
        // only support single context for now
        if (context_id != 0) return PAMI_SUCCESS;

        // This is where we get our reduction result back from the geometry create operation
        PAMI::Topology *local_master_topo = (PAMI::Topology *) (geometry->getTopology(PAMI::Geometry::MASTER_TOPOLOGY_INDEX));
        PAMI::Topology *local_topo        = (PAMI::Topology *)geometry->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX);

        uint master_rank   = local_topo->index2Endpoint(0);
        uint master_index  = local_master_topo->rank2Index(master_rank);
        void *ctrlstr      = (void *)in[master_index];
        if (ctrlstr == NULL) ctrlstr = (void *)_csmm.getWGCtrlStr();

        geometry->setKey(PAMI::Geometry::GKEY_GEOMETRYCSNI, ctrlstr);

        // Complete the final analysis and population of the geometry structure
        // with the algorithm list
        return analyze(context_id, geometry, 1);
      }

      inline pami_result_t analyze_impl(size_t context_id, T_Geometry *geometry, int phase)
      {
        TRACE_ERR((stderr, "<%p>CCMICSMultiRegistration::analyze_impl() context %zu, geometry %p, phase %u\n", this, context_id, geometry, phase));

        if (phase != 1) return PAMI_SUCCESS; // only after analyze_global \todo clean this up

        // only support single context for now
        if (context_id != 0) return PAMI_SUCCESS;

        // Get the topology for the local nodes
        // and the topology for the "distributed masters" for the global communication
        PAMI::Topology *local_topo        = (PAMI::Topology *) (geometry->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX));
        PAMI::Topology *local_master_topo = (PAMI::Topology *) (geometry->getTopology(PAMI::Geometry::MASTER_TOPOLOGY_INDEX));
        PAMI_assert(local_topo->size() != 0);
        PAMI_assert(local_master_topo->size() != 0);

        void *ctrlstr  = (void *) geometry->getKey(PAMI::Geometry::GKEY_GEOMETRYCSNI);

        // Allocate the local models
        T_CSModel *cs_model = new T_CSModel(&_devs, geometry->comm(), local_topo, &_csmm, ctrlstr);

        // Allocate the local native interface
        T_CSNativeInterface *ni = new T_CSNativeInterface(*cs_model, _client, _client_id, _context,
                                       _context_id, local_topo->rank2Index(__global.mapping.task()),
                                                          local_topo->size());

        geometry->setKey(PAMI::Geometry::GKEY_GEOMETRYCSNI, ni);

        geometry->addCollective(PAMI_XFER_BARRIER,&_msync_reg,context_id);
        geometry->addCollective(PAMI_XFER_BROADCAST,&_mcast_reg,context_id);
        geometry->addCollective(PAMI_XFER_REDUCE,&_mcomb_reg,context_id);

        return PAMI_SUCCESS;
      }

    public:
      pami_client_t                                            _client;
      size_t                                                   _client_id;
      pami_context_t                                           _context;
      size_t                                                   _context_id;
      std::map<unsigned, pami_geometry_t>                     *_geometry_map;
      PAMI::Device::Generic::Device                            &_devs;
      CCMI::Adaptor::Barrier::CSMultiSyncFactory               _msync_reg;
      CCMI::Adaptor::Broadcast::CSMultiCastFactory             _mcast_reg;
      CCMI::Adaptor::Allreduce::CSMultiCombineFactory          _mcomb_reg;
      T_CSMemoryManager                                        _csmm;
    }; // CCMICSMultiRegistration
  }; // CollRegistration
}; // PAMI

#endif
