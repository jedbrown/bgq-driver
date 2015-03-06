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
 * \file algorithms/geometry/P2PCCMIRegistration.h
 * \brief ???
 */

#ifndef __algorithms_geometry_P2PCCMIRegistration_h__
#define __algorithms_geometry_P2PCCMIRegistration_h__

#include "algorithms/geometry/P2PCCMIRegInfo.h"

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#ifdef CCMI_TRACE_ALL
#define DO_TRACE_ENTEREXIT 1
#define DO_TRACE_DEBUG     1
#else
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0
#endif

#ifndef addCollectiveCheckNonContig
// If non_contig is supported then add a query protocol, otherwise it's always works protocol
#define addCollectiveCheckNonContig(geometry,xfer,factory,context,id) \
{                                                                     \
  if(noncontig_support)                                               \
    geometry->addCollectiveCheck(xfer,  factory, context, id);        \
  else                                                                \
    geometry->addCollective(xfer,  factory, context, id);             \
}
#endif
// Collective Registration for CCMI protocols for p2p
namespace PAMI
{
  namespace CollRegistration
  {
    namespace P2P
    {
      template < class T_Geometry,
        class T_Allocator,
        class T_BinomialBarrier,
        class T_BinomialBarrierFactory,
        int   T_Support_One_Task=1>
      class CCMIRegistration :
        public CollRegistration < PAMI::CollRegistration::P2P::CCMIRegistration < T_Geometry,
        T_Allocator, 
        T_BinomialBarrier,
        T_BinomialBarrierFactory,
        T_Support_One_Task>, 
        T_Geometry >
      {

        class GeometryInfo
        {
        public:
          inline GeometryInfo(CCMI::Executor::Composite *ueb_composite,
                              void                      *geom_allocator):
            _ueb_composite(ueb_composite),
            _geom_allocator(geom_allocator)
            {
            }
          CCMI::Executor::Composite    *_ueb_composite;
          void                         *_geom_allocator;
        };

      protected:

      public:
        inline CCMIRegistration(pami_client_t                        client,
                                pami_context_t                       context,
                                size_t                               client_id,
                                size_t                               context_id,
                                T_Allocator                         &allocator,
                                size_t                               global_size,
                                size_t                               local_size,
                                int                                 *dispatch_id,
                                std::map<unsigned, pami_geometry_t> *geometry_map,
                                CCMI::Interfaces::NativeInterfaceFactory *nifactory,
                                CCMI::Interfaces::NativeInterfaceFactory *nifactory_amc):
              CollRegistration < PAMI::CollRegistration::P2P::CCMIRegistration < T_Geometry,
          T_Allocator, 
          T_BinomialBarrier,
          T_BinomialBarrierFactory,
          T_Support_One_Task>, 	      
          T_Geometry > (),
          _client(client),
          _context(context),
          _context_id(context_id),
          _client_id(client_id),
          _geometry_map(geometry_map),
          _ni_factory (nifactory),
          _ni_factory_amc (nifactory_amc),
          //_reduce_val(0),
          _dispatch_id(dispatch_id),
          _allocator(allocator),
          _binomial_barrier_composite(),	
          _cg_connmgr(65535),
          _color_connmgr(),
	  _sconnmgr(),	  
	  _sconnmgr1(1),	
          _rbconnmgr(),
          _csconnmgr(),
          _rsconnmgr(),
          _binomial_barrier_factory(NULL),       
          _onetask_barrier_factory(NULL),
          //_onetask_fence_factory(NULL),
          _onetask_broadcast_factory(NULL),
          _onetask_reduce_factory(NULL),
          _onetask_allreduce_factory(NULL),
          _onetask_allgather_factory(NULL),
          _onetask_allgatherv_factory(NULL),
          _onetask_allgatherv_int_factory(NULL),
          _onetask_scatter_factory(NULL),
          _onetask_scatterv_factory(NULL),
          _onetask_scatterv_int_factory(NULL),
          _onetask_gather_factory(NULL),
          _onetask_gatherv_factory(NULL),
          _onetask_gatherv_int_factory(NULL),
          _onetask_alltoall_factory(NULL),
          _onetask_alltoallv_factory(NULL),
          _onetask_alltoallv_int_factory(NULL),
          _onetask_scan_factory(NULL),
          _onetask_reduce_scatter_factory(NULL),
          _onetask_ambroadcast_factory(NULL),
          _onetask_amscatter_factory(NULL),
          _onetask_amgather_factory(NULL),
          _onetask_amreduce_factory(NULL),
#ifdef ENABLE_X0_PROTOCOLS // Experimental (X0:) protocols
          _rectangle_broadcast_factory(NULL),
          _rectangle_1color_broadcast_factory(NULL),
#endif
          _binomial_broadcast_factory(NULL),         
          _ring_broadcast_factory(NULL),          
          _binomial_broadcast_singleth_factory(NULL),         
          _ring_broadcast_singleth_factory(NULL),          
          _sync_2nary_broadcast_factory(NULL),
          _asrb_binomial_broadcast_factory(NULL),
          _ascs_binomial_broadcast_factory(NULL),
          _2nomial_broadcast_factory(NULL),
          _3nomial_broadcast_factory(NULL),
          _4nomial_broadcast_factory(NULL),
          _2nary_broadcast_factory(NULL),
          _3nary_broadcast_factory(NULL),
          _4nary_broadcast_factory(NULL),
          _active_binomial_broadcast_factory(NULL),
          _binomial_allreduce_factory(NULL),
          _binomial4_allreduce_factory(NULL),
          _binomial8_allreduce_factory(NULL),
#ifdef ENABLE_X0_PROTOCOLS // Experimental (X0:) protocols
          _ring_allreduce_factory(NULL),
#endif
          _ascs_binomial_allreduce_factory(NULL),
          _ascs_binomial_reduce_factory(NULL),
          _active_binomial_amreduce_factory(NULL),
          _ascs_binomial_scatter_factory(NULL),
          _ascs_flat_scatter_factory(NULL),
          _ascs_scatterv_factory(NULL),
          _ascs_scatterv_int_factory(NULL),
          _active_binomial_amscatter_factory(NULL),
          _ascs_binomial_scan_factory(NULL),
          _ascs_reduce_scatter_factory(NULL),
          _ascs_binomial_gather_factory(NULL),
          _ascs_flat_gather_factory(NULL),
          _active_binomial_gather_factory(NULL),
          _ascs_gatherv_factory(NULL),
          _ascs_gatherv_int_factory(NULL),
          _ascs_binomial_allgather_factory(NULL),
          _ascs_ring_allgather_factory(NULL),
          _ascs_ring_allgatherv_factory(NULL),
          _ascs_ring_allgatherv_int_factory(NULL),
          _ascs_pairwise_alltoall_factory(NULL),
          _ascs_pairwise_alltoallv_int_factory(NULL),
          _ascs_pairwise_alltoallv_factory(NULL),
          _alltoall_factory(NULL),
          _alltoallv_factory(NULL),
          _alltoallv_factory_int(NULL)
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "<%p>client_id %zu, context_id %zu", this, client_id, context_id);
          TRACE_FORMAT( "<%p>NI Factory %p, local_size %zu, global_size %zu", this, nifactory, local_size, global_size);
          if(T_Support_One_Task) setupOneTaskFactories();
    
          setupFactories ();

          TRACE_FN_EXIT();
        }

       static inline void cleanupCallback(pami_context_t ctxt, void *data, pami_result_t res)
         {
           GeometryInfo *gi = (GeometryInfo*) data;
           CCMI::Executor::Composite *ueb_composite = (CCMI::Executor::Composite *)gi->_ueb_composite;
           PAMI::MemoryAllocator<sizeof(GeometryInfo),16>  *geom_allocator =
                 (PAMI::MemoryAllocator<sizeof(GeometryInfo),16>  *)gi->_geom_allocator;

           T_BinomialBarrierFactory::cleanup_done_fn(ctxt, ueb_composite, res);
           //ueb_composite->_factory->_alloc.returnObject(ueb_composite);

           geom_allocator->returnObject(gi);
         }


        inline pami_result_t analyze_impl(size_t context_id, T_Geometry *geometry, int phase, uint64_t*)
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT(  "<%p>CCMIRegistration::analyze_impl() context_id %zu, geometry %p",
                           this,
                           context_id,
                           geometry);
            if (phase != 0) {TRACE_FN_EXIT(); return PAMI_SUCCESS;}
            bool noncontig_support = false; // Used by addCollectiveCheckNonContig()
            {
              void *val = geometry->getKey(context_id, PAMI::Geometry::CKEY_NONCONTIG_DISABLE);
              TRACE_FORMAT(" CKEY_NONCONTIG_DISABLE %p",val);
              if(!val)      // If noncontig is not disabled
                noncontig_support = true; // (default for not found)
            }

            PAMI_assert_debugf(context_id == _context_id,"%zu == %zu\n",context_id,_context_id);

            if(geometry->size() == 1)//if onetask geometry
            {
              if(T_Support_One_Task==0) {TRACE_FN_EXIT(); return PAMI_SUCCESS;}

              pami_xfer_t xfer = {0};
              if (_onetask_barrier_factory == NULL) // nothing setup?
                ; // then do nothing - no shmem on 1 process per node (and other protocol is disabled)
              else
              {
                TRACE_FORMAT( "<%p>CCMIRegistration::analyze() add",this);
                _onetask_barrier_composite = (CCMI::Adaptor::P2POneTask::OneTaskBarrier *)
                _onetask_barrier_factory->generate(geometry, &xfer);

                geometry->addCollective(PAMI_XFER_BARRIER,
                                        _onetask_barrier_factory,
                                        _context,
                                        _context_id);

                geometry->resetUEBarrier(context_id);
                pami_result_t ueResult = geometry->setUEBarrier(_onetask_barrier_factory,context_id);
                if(ueResult == PAMI_SUCCESS) /// \todo multi-context support
                {
                  geometry->setKey(context_id,
                                   PAMI::Geometry::CKEY_UEBARRIERCOMPOSITE1,
                                   (void*)_onetask_barrier_factory);
                }
                geometry->resetDefaultBarrier(context_id);
                ueResult = geometry->setDefaultBarrier(_onetask_barrier_factory,context_id);

                geometry->setCleanupCallback(CCMI::Adaptor::P2POneTask::OneTaskBarrierFactory::cleanup_done_fn, _onetask_barrier_composite);
              }

              //geometry->addCollective(PAMI_XFER_FENCE,
              //                        _onetask_fence_factory,
              //                        _context_id);

              geometry->addCollective(PAMI_XFER_BROADCAST,
                                      _onetask_broadcast_factory,
                                      _context,
                                      _context_id);
              geometry->addCollective(PAMI_XFER_REDUCE,
                                      _onetask_reduce_factory,
                                      _context,
                                      _context_id);
              geometry->addCollective(PAMI_XFER_ALLGATHER,
                                      _onetask_allgather_factory,
                                      _context,
                                      _context_id);
              geometry->addCollective(PAMI_XFER_ALLGATHERV,
                                      _onetask_allgatherv_factory,
                                      _context,
                                      _context_id);
              geometry->addCollective(PAMI_XFER_ALLGATHERV_INT,
                                      _onetask_allgatherv_int_factory,
                                      _context,
                                      _context_id);
              geometry->addCollective(PAMI_XFER_SCATTER,
                                      _onetask_scatter_factory,
                                      _context,
                                      _context_id);
              geometry->addCollective(PAMI_XFER_SCATTERV,
                                      _onetask_scatterv_factory,
                                      _context,
                                      _context_id);
              geometry->addCollective(PAMI_XFER_SCATTERV_INT,
                                      _onetask_scatterv_int_factory,
                                      _context,
                                      _context_id);
              geometry->addCollective(PAMI_XFER_GATHER,
                                      _onetask_gather_factory,
                                      _context,
                                      _context_id);
              geometry->addCollective(PAMI_XFER_GATHERV,
                                      _onetask_gatherv_factory,
                                      _context,
                                      _context_id);
              geometry->addCollective(PAMI_XFER_GATHERV_INT,
                                      _onetask_gatherv_int_factory,
                                      _context,
                                      _context_id);
              geometry->addCollectiveCheck(PAMI_XFER_ALLTOALL,
                                      _onetask_alltoall_factory,
                                      _context,
                                      _context_id);
              geometry->addCollectiveCheck(PAMI_XFER_ALLTOALLV,
                                      _onetask_alltoallv_factory,
                                      _context,
                                      _context_id);
              geometry->addCollectiveCheck(PAMI_XFER_ALLTOALLV_INT,
                                      _onetask_alltoallv_int_factory,
                                      _context,
                                      _context_id);
              geometry->addCollective(PAMI_XFER_ALLREDUCE,
                                      _onetask_allreduce_factory,
                                      _context,
                                      _context_id);
              geometry->addCollective(PAMI_XFER_SCAN,
                                      _onetask_scan_factory,
                                      _context,
                                      _context_id);
              geometry->addCollective(PAMI_XFER_REDUCE_SCATTER,
                                      _onetask_reduce_scatter_factory,
                                      _context,
                                      _context_id);
              geometry->addCollective(PAMI_XFER_AMBROADCAST,
                                      _onetask_ambroadcast_factory,
                                      _context,
                                      _context_id);
              geometry->addCollective(PAMI_XFER_AMSCATTER,
                                      _onetask_amscatter_factory,
                                      _context,
                                      _context_id);
              geometry->addCollective(PAMI_XFER_AMGATHER,
                                      _onetask_amgather_factory,
                                      _context,
                                      _context_id);
              geometry->addCollective(PAMI_XFER_AMREDUCE,
                                      _onetask_amreduce_factory,
                                      _context,
                                      _context_id);
            }//End if onetask geometry
            else//More than one task
            {
              int flag=0, flag_amc=0;
              unsigned op_query_needed = 0, all_query_needed=0;
              unsigned op_query_needed_amc = 0, all_query_needed_amc=0;
              pami_result_t result_amc = PAMI_UNIMPL;
              // Let the NI analyze the geometry and pass back flags
              if(_ni_factory_amc) {
                result_amc = _ni_factory_amc->analyze(context_id, geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX),phase,&flag_amc);
                if(result_amc == PAMI_SUCCESS); // no analyze changes, all ok
                else if(result_amc == PAMI_OTHER) // check flag for must-query protocol
                {
                  /// \todo define flags
                  if(flag_amc == 1) op_query_needed_amc = 1; // only dt/op must query (reduce-like collectives)
                  else op_query_needed_amc=all_query_needed_amc=1; // query all collectives
                }
                else PAMI_assertf(result_amc==PAMI_SUCCESS,"result %u, flag %u",result_amc,flag_amc); // undefined/unexpected
                TRACE_FORMAT( "<%p>CCMIRegistration::analyze() add op_query_amc(%u)all_query_amc(%u)",this,op_query_needed_amc,all_query_needed_amc);
              }
              pami_result_t result = _ni_factory->analyze(context_id, geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX),phase,&flag);
              if(result == PAMI_SUCCESS); // no analyze changes, all ok
              else if(result == PAMI_UNIMPL && result_amc == PAMI_UNIMPL) {TRACE_FN_EXIT(); return PAMI_SUCCESS;} // not supported, don't add algorithms
              else if(result == PAMI_OTHER) // check flag for must-query protocol
              {
                /// \todo define flags
                if(flag == 1) op_query_needed = 1; // only dt/op must query (reduce-like collectives)
                else op_query_needed=all_query_needed=1; // query all collectives
              }
              else PAMI_assertf(result==PAMI_SUCCESS,"result %u, flag %u",result,flag); // undefined/unexpected

              // Add collectives 
              TRACE_FORMAT( "<%p>CCMIRegistration::analyze() add op_query(%u)all_query(%u)",this,op_query_needed,all_query_needed);
              pami_xfer_t xfer = {0};

              if (_binomial_barrier_factory) 
              {
                TRACE_FORMAT( "<%p>CCMIRegistration::analyze() add",this);
                _binomial_barrier_composite = (T_BinomialBarrier *)
                  _binomial_barrier_factory->generate(geometry, &xfer);

                GeometryInfo    *gi = (GeometryInfo*)_geom_allocator.allocateObject();
                new(gi) GeometryInfo(_binomial_barrier_composite, &_geom_allocator);

                _binomial_barrier_composite->getExecutor()->setContext(_context);
                geometry->setKey(context_id,
                                 PAMI::Geometry::CKEY_OPTIMIZEDBARRIERCOMPOSITE,
                                 (void*)_binomial_barrier_composite);


                _csconnmgr.setSequence(geometry->comm());

                geometry->addCollective(PAMI_XFER_BARRIER,
                                        _binomial_barrier_factory,
                                        _context,
                                        _context_id);

                geometry->resetUEBarrier(context_id);
                pami_result_t ueResult = geometry->setUEBarrier(_binomial_barrier_factory, context_id);
                if(ueResult == PAMI_SUCCESS)
                {
                  // Set geometry-wide, across contexts, UE barrier
                  geometry->setKey(context_id,
                                   PAMI::Geometry::CKEY_UEBARRIERCOMPOSITE1,
                                   (void*)_binomial_barrier_composite);
                }
                geometry->setDefaultBarrier(_binomial_barrier_factory, context_id);

                geometry->setCleanupCallback(cleanupCallback, gi);
              }

#ifdef PAMI_ENABLE_X0_PROTOCOLS // Experimental (X0:) protocols
              if (_rectangle_broadcast_factory) {
                PAMI::Topology * rectangle = (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::COORDINATE_TOPOLOGY_INDEX);
                if((rectangle->type() == PAMI_COORD_TOPOLOGY) &&   // could be EMPTY
                   (__global.mapping.torusDims() > 1))             /// \todo problems on pseudo-torus platforms so disable it on tdim == 1
                {
                  if(all_query_needed)
                    geometry->addCollectiveCheck();
                  else if(result == PAMI_SUCCESS)
                    geometry->addCollective(PAMI_XFER_BROADCAST,
                                          _rectangle_broadcast_factory,
                                            _context,
                                          _context_id);
                  /// \todo 1 color doesn't work on sub-communicators, so disable it
                  if(rectangle->size() == __global.topology_global.size())
                  {
                  if(all_query_needed)
                    geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                            _rectangle_1color_broadcast_factory,
                                            _context,
                                            _context_id);
                  else if(result == PAMI_SUCCESS)
                    geometry->addCollective(PAMI_XFER_BROADCAST,
                                            _rectangle_1color_broadcast_factory,
                                            _context,
                                            _context_id);
                  }
                }
              }
#endif
              if(op_query_needed)
              {
                if(_binomial_allreduce_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_ALLREDUCE,
                                               _binomial_allreduce_factory,
                                               _context,
                                               _context_id);
                if(_binomial4_allreduce_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_ALLREDUCE,
                                               _binomial4_allreduce_factory,
                                               _context,
                                               _context_id);
                if(_binomial8_allreduce_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_ALLREDUCE,
                                        _binomial8_allreduce_factory,
                                               _context,
                                        _context_id);

#ifdef ENABLE_X0_PROTOCOLS // Experimental (X0:) protocols
                if(_ring_allreduce_factory)
		  {
		    PAMI::Topology * topo = (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);
		    if(topo->isContextOffset(0))
		      geometry->addCollectiveCheck(PAMI_XFER_ALLREDUCE,
						   _ring_allreduce_factory,
						   _context,
						   _context_id);
		  }
#endif
		if(_ascs_binomial_allreduce_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_ALLREDUCE,
                                        _ascs_binomial_allreduce_factory,
                                               _context,
                                        _context_id);
                if(_ascs_binomial_reduce_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_REDUCE,
                                        _ascs_binomial_reduce_factory,
                                               _context,
                                        _context_id);
                if(_ascs_reduce_scatter_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_REDUCE_SCATTER,
                                       _ascs_reduce_scatter_factory,
                                               _context,
                                       _context_id);
              }
              else if(result == PAMI_SUCCESS)
              {
                if(_binomial_allreduce_factory)
                  geometry->addCollective(PAMI_XFER_ALLREDUCE,
                                        _binomial_allreduce_factory,
                                          _context,
                                        _context_id);
                if(_binomial4_allreduce_factory)
                  geometry->addCollective(PAMI_XFER_ALLREDUCE,
                                          _binomial4_allreduce_factory,
                                          _context,
                                          _context_id);
                if(_binomial8_allreduce_factory)
                  geometry->addCollective(PAMI_XFER_ALLREDUCE,
                                        _binomial8_allreduce_factory,
                                          _context,
                                        _context_id);

#ifdef ENABLE_X0_PROTOCOLS // Experimental (X0:) protocols
                if(_ring_allreduce_factory)
		  {
		    PAMI::Topology * topo = (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);
		    if(topo->isContextOffset(0))
		      geometry->addCollectiveCheck(PAMI_XFER_ALLREDUCE,
						   _ring_allreduce_factory,
						   _context,
						   _context_id);
		  }
#endif
                if(_ascs_binomial_allreduce_factory)
                  geometry->addCollective(PAMI_XFER_ALLREDUCE,
                                        _ascs_binomial_allreduce_factory,
                                          _context,
                                        _context_id);
                if(_ascs_binomial_reduce_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_REDUCE,
                                        _ascs_binomial_reduce_factory,
                                          _context,
                                        _context_id);
                if(_ascs_reduce_scatter_factory)
                  geometry->addCollective(PAMI_XFER_REDUCE_SCATTER,
                                       _ascs_reduce_scatter_factory,
                                          _context,
                                       _context_id);
              }

              if(all_query_needed)
              {  
                if(_binomial_broadcast_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                        _binomial_broadcast_factory,
                                               _context,
                                        _context_id);
                if(_ring_broadcast_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                               _ring_broadcast_factory,
                                               _context,
                                               _context_id);
                if(_sync_2nary_broadcast_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                               _sync_2nary_broadcast_factory,
                                               _context,
                                               _context_id);
                if(_ascs_binomial_broadcast_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                        _ascs_binomial_broadcast_factory,
                                               _context,
                                        _context_id);
                if(_2nomial_broadcast_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                        _2nomial_broadcast_factory,
                                               _context,
                                        _context_id);
                if(_3nomial_broadcast_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                        _3nomial_broadcast_factory,
                                               _context,
                                        _context_id);
                if(_4nomial_broadcast_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                        _4nomial_broadcast_factory,
                                               _context,
                                        _context_id);
                if(_2nary_broadcast_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                        _2nary_broadcast_factory,
                                               _context,
                                        _context_id);
                if(_3nary_broadcast_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                        _3nary_broadcast_factory,
                                               _context,
                                        _context_id);
                if(_4nary_broadcast_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                        _4nary_broadcast_factory,
                                               _context,
                                        _context_id);
                if(_asrb_binomial_broadcast_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                        _asrb_binomial_broadcast_factory,
                                               _context,
                                        _context_id);
                if(_ascs_binomial_scatter_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_SCATTER,
                                        _ascs_binomial_scatter_factory,
                                               _context,
                                        _context_id);
                if(_ascs_flat_scatter_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_SCATTER,
                                        _ascs_flat_scatter_factory,
                                               _context,
                                        _context_id);
                if(_ascs_scatterv_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_SCATTERV,
                                        _ascs_scatterv_factory,
                                               _context,
                                        _context_id);
                if(_ascs_scatterv_int_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_SCATTERV_INT,
                                       _ascs_scatterv_int_factory,
                                               _context,
                                       _context_id);
                if(_ascs_binomial_scan_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_SCAN,
                                       _ascs_binomial_scan_factory,
                                               _context,
                                       _context_id);
                if(_ascs_binomial_gather_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_GATHER,
                                        _ascs_binomial_gather_factory,
                                               _context,
                                        _context_id);
                if(_ascs_flat_gather_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_GATHER,
                                        _ascs_flat_gather_factory,
                                               _context,
                                        _context_id);
                if(_ascs_gatherv_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_GATHERV,
                                        _ascs_gatherv_factory,
                                               _context,
                                        _context_id);
                if(_ascs_gatherv_int_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_GATHERV_INT,
                                        _ascs_gatherv_int_factory,
                                               _context,
                                        _context_id);
                if(_ascs_binomial_allgather_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_ALLGATHER,
                                        _ascs_binomial_allgather_factory,
                                               _context,
                                        _context_id);
                if(_ascs_ring_allgather_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_ALLGATHER,
                                        _ascs_ring_allgather_factory,
                                               _context,
                                        _context_id);
                if(_ascs_ring_allgatherv_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_ALLGATHERV,
                                        _ascs_ring_allgatherv_factory,
                                               _context,
                                        _context_id);
                if(_ascs_ring_allgatherv_int_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_ALLGATHERV_INT,
                                        _ascs_ring_allgatherv_int_factory,
                                               _context,
                                        _context_id);
                if(_ascs_pairwise_alltoall_factory)
                  addCollectiveCheckNonContig(geometry,PAMI_XFER_ALLTOALL,
                                        _ascs_pairwise_alltoall_factory,
                                               _context,
                                        _context_id);
                if(_ascs_pairwise_alltoallv_int_factory)
                  addCollectiveCheckNonContig(geometry,PAMI_XFER_ALLTOALLV_INT,
                                        _ascs_pairwise_alltoallv_int_factory,
                                               _context,
                                        _context_id);
                if(_ascs_pairwise_alltoallv_factory)
                  addCollectiveCheckNonContig(geometry,PAMI_XFER_ALLTOALLV,
                                        _ascs_pairwise_alltoallv_factory,
                                               _context,
                                        _context_id);
                if(_alltoall_factory)
                  addCollectiveCheckNonContig(geometry,PAMI_XFER_ALLTOALL,
                                        _alltoall_factory,
                                               _context,
                                        _context_id);
                if(_alltoallv_factory)
                  addCollectiveCheckNonContig(geometry,PAMI_XFER_ALLTOALLV,
                                        _alltoallv_factory,
                                               _context,
                                        _context_id);
                if(_alltoallv_factory_int)
                  addCollectiveCheckNonContig(geometry,PAMI_XFER_ALLTOALLV_INT,
                                        _alltoallv_factory_int,
                                               _context,
                                        _context_id);
              }
              else if(result == PAMI_SUCCESS)
              {  
                if(_binomial_broadcast_factory)
                  geometry->addCollective(PAMI_XFER_BROADCAST,
                                          _binomial_broadcast_factory,
                                          _context,
                                          _context_id);
                if(_binomial_broadcast_singleth_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                          _binomial_broadcast_singleth_factory,
                                          _context,
                                          _context_id);

                if(_ring_broadcast_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                          _ring_broadcast_factory,
                                          _context,
                                          _context_id);
                if(_ring_broadcast_singleth_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                          _ring_broadcast_singleth_factory,
                                          _context,
                                          _context_id);		
                if(_sync_2nary_broadcast_factory)
                  geometry->addCollective(PAMI_XFER_BROADCAST,
                                          _sync_2nary_broadcast_factory,
                                          _context,
                                          _context_id);
                if(_ascs_binomial_broadcast_factory)
                  geometry->addCollective(PAMI_XFER_BROADCAST,
                                        _ascs_binomial_broadcast_factory,
                                          _context,
                                        _context_id);
                if(_2nomial_broadcast_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                        _2nomial_broadcast_factory,
                                          _context,
                                        _context_id);
                if(_3nomial_broadcast_factory)
                  geometry->addCollective(PAMI_XFER_BROADCAST,
                                        _3nomial_broadcast_factory,
                                          _context,
                                        _context_id);
                if(_4nomial_broadcast_factory)
                  geometry->addCollective(PAMI_XFER_BROADCAST,
                                        _4nomial_broadcast_factory,
                                          _context,
                                        _context_id);
                if(_2nary_broadcast_factory)
                  geometry->addCollective(PAMI_XFER_BROADCAST,
                                        _2nary_broadcast_factory,
                                          _context,
                                        _context_id);
                if(_3nary_broadcast_factory)
                  geometry->addCollective(PAMI_XFER_BROADCAST,
                                        _3nary_broadcast_factory,
                                          _context,
                                        _context_id);
                if(_4nary_broadcast_factory)
                  geometry->addCollective(PAMI_XFER_BROADCAST,
                                        _4nary_broadcast_factory,
                                          _context,
                                        _context_id);
                if(_asrb_binomial_broadcast_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                        _asrb_binomial_broadcast_factory,
                                          _context,
                                        _context_id);
                if(_ascs_binomial_scatter_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_SCATTER,
                                        _ascs_binomial_scatter_factory,
                                          _context,
                                        _context_id);
                if(_ascs_flat_scatter_factory)
                  geometry->addCollective(PAMI_XFER_SCATTER,
                                        _ascs_flat_scatter_factory,
                                          _context,
                                        _context_id);
                if(_ascs_scatterv_factory)
                  geometry->addCollective(PAMI_XFER_SCATTERV,
                                        _ascs_scatterv_factory,
                                          _context,
                                        _context_id);
                if(_ascs_scatterv_int_factory)
                  geometry->addCollective(PAMI_XFER_SCATTERV_INT,
                                       _ascs_scatterv_int_factory,
                                          _context,
                                       _context_id);
                if(_ascs_binomial_scan_factory)
                  geometry->addCollective(PAMI_XFER_SCAN,
                                       _ascs_binomial_scan_factory,
                                          _context,
                                       _context_id);
                if(_ascs_binomial_gather_factory)
                  geometry->addCollective(PAMI_XFER_GATHER,
                                        _ascs_binomial_gather_factory,
                                          _context,
                                        _context_id);
                if(_ascs_flat_gather_factory)
                  geometry->addCollective(PAMI_XFER_GATHER,
                                        _ascs_flat_gather_factory,
                                          _context,
                                        _context_id);
                if(_ascs_gatherv_factory)
                  geometry->addCollective(PAMI_XFER_GATHERV,
                                        _ascs_gatherv_factory,
                                          _context,
                                        _context_id);
                if(_ascs_gatherv_int_factory)
                  geometry->addCollective(PAMI_XFER_GATHERV_INT,
                                        _ascs_gatherv_int_factory,
                                          _context,
                                        _context_id);
                if(_ascs_binomial_allgather_factory)
                  geometry->addCollective(PAMI_XFER_ALLGATHER,
                                        _ascs_binomial_allgather_factory,
                                          _context,
                                        _context_id);
                if(_ascs_ring_allgather_factory)
                  geometry->addCollective(PAMI_XFER_ALLGATHER,
                                        _ascs_ring_allgather_factory,
                                          _context,
                                        _context_id);
                if(_ascs_ring_allgatherv_factory)
                  geometry->addCollective(PAMI_XFER_ALLGATHERV,
                                        _ascs_ring_allgatherv_factory,
                                          _context,
                                        _context_id);
                if(_ascs_ring_allgatherv_int_factory)
                  geometry->addCollective(PAMI_XFER_ALLGATHERV_INT,
                                        _ascs_ring_allgatherv_int_factory,
                                          _context,
                                        _context_id);
                if(_ascs_pairwise_alltoall_factory)
                  addCollectiveCheckNonContig(geometry,PAMI_XFER_ALLTOALL,
                                        _ascs_pairwise_alltoall_factory,
                                          _context,
                                        _context_id);
                if(_ascs_pairwise_alltoallv_int_factory)
                  addCollectiveCheckNonContig(geometry,PAMI_XFER_ALLTOALLV_INT,
                                        _ascs_pairwise_alltoallv_int_factory,
                                          _context,
                                        _context_id);
                if(_ascs_pairwise_alltoallv_factory)
                  addCollectiveCheckNonContig(geometry,PAMI_XFER_ALLTOALLV,
                                        _ascs_pairwise_alltoallv_factory,
                                          _context,
                                        _context_id);
                if(_alltoall_factory)
                  addCollectiveCheckNonContig(geometry,PAMI_XFER_ALLTOALL,
                                        _alltoall_factory,
                                          _context,
                                        _context_id);
                if(_alltoallv_factory)
                  addCollectiveCheckNonContig(geometry,PAMI_XFER_ALLTOALLV,
                                        _alltoallv_factory,
                                          _context,
                                        _context_id);
                if(_alltoallv_factory_int)
                  addCollectiveCheckNonContig(geometry,PAMI_XFER_ALLTOALLV_INT,
                                        _alltoallv_factory_int,
                                          _context,
                                        _context_id);
              }
              if(op_query_needed_amc)
              {
                if(_active_binomial_amreduce_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_AMREDUCE,
                                        _active_binomial_amreduce_factory,
                                        _context,
                                        _context_id);
              }
              else if(result_amc == PAMI_SUCCESS)
              {
                if(_active_binomial_amreduce_factory)
                  geometry->addCollective(PAMI_XFER_AMREDUCE,
                                        _active_binomial_amreduce_factory,
                                          _context,
                                        _context_id);
              }
              if(all_query_needed_amc)
              {
                if(_active_binomial_gather_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_AMGATHER,
                                               _active_binomial_gather_factory,
                                               _context,
                                               _context_id);
                if(_active_binomial_amscatter_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_AMSCATTER,
                                               _active_binomial_amscatter_factory,
                                               _context,
                                               _context_id);
                if(_active_binomial_broadcast_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_AMBROADCAST,
                                               _active_binomial_broadcast_factory,
                                               _context,
                                               _context_id);
              }
              else if(result_amc == PAMI_SUCCESS)
              {
                if(_active_binomial_gather_factory)
                  geometry->addCollective(PAMI_XFER_AMGATHER,
                                          _active_binomial_gather_factory,
                                          _context,
                                          _context_id);
                if(_active_binomial_amscatter_factory)
                  geometry->addCollective(PAMI_XFER_AMSCATTER,
                                          _active_binomial_amscatter_factory,
                                          _context,
                                          _context_id);
                if(_active_binomial_broadcast_factory)
                  geometry->addCollective(PAMI_XFER_AMBROADCAST,
                                          _active_binomial_broadcast_factory,
                                          _context,
                                          _context_id);
              }
            }
            TRACE_FN_EXIT();
            return PAMI_SUCCESS;
          }

          inline pami_result_t register_local_impl(size_t context_id,T_Geometry *geometry, uint64_t *out, int &n)
          {
            n  = 0;
            //*out = _reduce_val;
            return analyze(context_id, geometry, 0);
          }

          inline pami_result_t receive_global_impl(size_t context_id,T_Geometry *geometry, uint64_t *in, int n)
          {
            return PAMI_SUCCESS;
          }

        private:
          template<class T_Factory>
            pami_result_t  setupFactory(CCMI::Interfaces::NativeInterface                 *& ni, 
                                        T_Factory                                         *& factory, 
                                        CCMI::Interfaces::NativeInterfaceFactory::NISelect   niselect,
                                        CCMI::Interfaces::NativeInterfaceFactory::NIType     nitype,
                                        size_t                                               nconnections=-1,
                                        bool                                                 is_amcoll_factory=false)
          {
            TRACE_FN_ENTER();
            DO_DEBUG((templateName<T_Factory>()));
            TRACE_FORMAT("<%p> ni factory %p", this, _ni_factory);            
            pami_result_t       result   = PAMI_ERROR;
            if(is_amcoll_factory)
            {
              // If native interface factory is NULL, skip adding the AM Collective
              if(_ni_factory_amc == NULL)
              {
                TRACE_FN_EXIT();
                return PAMI_INVAL;
              }

              result = _ni_factory_amc->generate ( _dispatch_id,
                                                  niselect,
                                                  nitype,
                                                  nconnections,
                                                  ni);
            }
            else
            {
              result = _ni_factory->generate ( _dispatch_id,
                                               niselect,
                                               nitype,
                                               nconnections,
                                               ni);
            }
            
            //PAMI_assert(result == PAMI_SUCCESS);
            COMPILE_TIME_ASSERT(sizeof(T_Factory) <= T_Allocator::objsize);
            TRACE_FORMAT("Allocator:  sizeof(T_Factory) %zu, T_Allocator::objsize %zu",sizeof(T_Factory),T_Allocator::objsize);
            if (result == PAMI_SUCCESS)
              factory = (T_Factory*) _allocator.allocateObject ();
            
            TRACE_FORMAT("<%p> niselect %u/nitype %u, ni %p, factory %p, result %u",
                         this, niselect, nitype, ni, factory,  result);            
            TRACE_FN_EXIT();
            return result;
          }

          template<class T_Factory>
          void setupFactory(T_Factory *&factory)
          {
            TRACE_FN_ENTER();
            DO_DEBUG((templateName<T_Factory>()));
            COMPILE_TIME_ASSERT(sizeof(T_Factory) <= T_Allocator::objsize);
            TRACE_FORMAT("Allocator:  sizeof(T_Factory) %zu, T_Allocator::objsize %zu",sizeof(T_Factory),T_Allocator::objsize);
            factory = (T_Factory*) _allocator.allocateObject ();
            TRACE_FORMAT("<%p> factory %p", this, factory);            
            TRACE_FN_EXIT();
          }


          void setupOneTaskFactories()
          {
            TRACE_FN_ENTER();
            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskBarrierFactory>(_onetask_barrier_factory);
            new ((void*)_onetask_barrier_factory) CCMI::Adaptor::P2POneTask::OneTaskBarrierFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            //setupFactory<CCMI::Adaptor::P2POneTask::OneTaskFenceFactory>(_onetask_fence_factory);
            //new ((void*)_onetask_fence_factory) CCMI::Adaptor::P2POneTask::OneTaskFenceFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskBroadcastFactory>(_onetask_broadcast_factory);
            new ((void*)_onetask_broadcast_factory) CCMI::Adaptor::P2POneTask::OneTaskBroadcastFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskReduceFactory>(_onetask_reduce_factory);
            new ((void*)_onetask_reduce_factory) CCMI::Adaptor::P2POneTask::OneTaskReduceFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskAllreduceFactory>(_onetask_allreduce_factory);
            new ((void*)_onetask_allreduce_factory) CCMI::Adaptor::P2POneTask::OneTaskAllreduceFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskAllgatherFactory>(_onetask_allgather_factory);
            new ((void*)_onetask_allgather_factory) CCMI::Adaptor::P2POneTask::OneTaskAllgatherFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskAllgathervFactory>(_onetask_allgatherv_factory);
            new ((void*)_onetask_allgatherv_factory) CCMI::Adaptor::P2POneTask::OneTaskAllgathervFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskAllgathervIntFactory>(_onetask_allgatherv_int_factory);
            new ((void*)_onetask_allgatherv_int_factory) CCMI::Adaptor::P2POneTask::OneTaskAllgathervIntFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskScatterFactory>(_onetask_scatter_factory);
            new ((void*)_onetask_scatter_factory) CCMI::Adaptor::P2POneTask::OneTaskScatterFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskScattervFactory>(_onetask_scatterv_factory);
            new ((void*)_onetask_scatterv_factory) CCMI::Adaptor::P2POneTask::OneTaskScattervFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskScattervIntFactory>(_onetask_scatterv_int_factory);
            new ((void*)_onetask_scatterv_int_factory) CCMI::Adaptor::P2POneTask::OneTaskScattervIntFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskGatherFactory>(_onetask_gather_factory);
            new ((void*)_onetask_gather_factory) CCMI::Adaptor::P2POneTask::OneTaskGatherFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskGathervFactory>(_onetask_gatherv_factory);
            new ((void*)_onetask_gatherv_factory) CCMI::Adaptor::P2POneTask::OneTaskGathervFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskGathervIntFactory>(_onetask_gatherv_int_factory);
            new ((void*)_onetask_gatherv_int_factory) CCMI::Adaptor::P2POneTask::OneTaskGathervIntFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskAlltoallFactory>(_onetask_alltoall_factory);
            new ((void*)_onetask_alltoall_factory) CCMI::Adaptor::P2POneTask::OneTaskAlltoallFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskAlltoallvFactory>(_onetask_alltoallv_factory);
            new ((void*)_onetask_alltoallv_factory) CCMI::Adaptor::P2POneTask::OneTaskAlltoallvFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskAlltoallvIntFactory>(_onetask_alltoallv_int_factory);
            new ((void*)_onetask_alltoallv_int_factory) CCMI::Adaptor::P2POneTask::OneTaskAlltoallvIntFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskScanFactory>(_onetask_scan_factory);
            new ((void*)_onetask_scan_factory) CCMI::Adaptor::P2POneTask::OneTaskScanFactory(_context,_context_id,mapidtogeometry,NULL,NULL);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskReduceScatterFactory>(_onetask_reduce_scatter_factory);
            new ((void*)_onetask_reduce_scatter_factory) CCMI::Adaptor::P2POneTask::OneTaskReduceScatterFactory(_context,_context_id,mapidtogeometry,NULL,NULL);


            // OneTaskAMFactory needs a native interface, so that we have access to the endpoint
            // So create one NI and use it across all OneTaskAMFactory instances
            CCMI::Interfaces::NativeInterface  *ni = NULL;
            pami_result_t rc;
            rc = setupFactory<CCMI::Adaptor::P2POneTask::OneTaskAMBroadcastFactory>(ni, _onetask_ambroadcast_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if(rc == PAMI_SUCCESS) new ((void*)_onetask_ambroadcast_factory) CCMI::Adaptor::P2POneTask::OneTaskAMBroadcastFactory(_context,_context_id,mapidtogeometry,NULL,ni);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskAMScatterFactory>(_onetask_amscatter_factory);
            if(rc == PAMI_SUCCESS) new ((void*)_onetask_amscatter_factory) CCMI::Adaptor::P2POneTask::OneTaskAMScatterFactory(_context,_context_id,mapidtogeometry,NULL,ni);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskAMGatherFactory>(_onetask_amgather_factory);
            if(rc == PAMI_SUCCESS) new ((void*)_onetask_amgather_factory) CCMI::Adaptor::P2POneTask::OneTaskAMGatherFactory(_context,_context_id,mapidtogeometry,NULL,ni);

            setupFactory<CCMI::Adaptor::P2POneTask::OneTaskAMReduceFactory>(_onetask_amreduce_factory);
            if(rc == PAMI_SUCCESS) new ((void*)_onetask_amreduce_factory) CCMI::Adaptor::P2POneTask::OneTaskAMReduceFactory(_context,_context_id,mapidtogeometry,NULL,ni);

            TRACE_FN_EXIT();
          }

          void setupFactories()
          {
            TRACE_FN_ENTER();
            CCMI::Interfaces::NativeInterface  *ni = NULL;

            // ----------------------------------------------------
            // Setup and Construct a binomial barrier factory from active message ni and p2p protocol
            pami_result_t rc = setupFactory<T_BinomialBarrierFactory>(ni, _binomial_barrier_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS)
              new ((void*)_binomial_barrier_factory) T_BinomialBarrierFactory(_context,_context_id,mapidtogeometry,&_sconnmgr, ni, T_BinomialBarrierFactory::cb_head);
            // ----------------------------------------------------

#ifdef PAMI_ENABLE_X0_PROTOCOLS // Experimental (X0:) protocols
            // ----------------------------------------------------
            // Setup and Construct a rectangle broadcast factory from allsided ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PBroadcast::RectangleBroadcastFactory>(ni, _rectangle_broadcast_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED);
            if (rc == PAMI_SUCCESS) new ((void*)_rectangle_broadcast_factory) CCMI::Adaptor::P2PBroadcast::RectangleBroadcastFactory(_context,_context_id,mapidtogeometry,&_color_connmgr, ni);
            // ----------------------------------------------------
            // ----------------------------------------------------
            // Setup and Construct a rectangle broadcast factory from allsided ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PBroadcast::Rectangle1ColorBroadcastFactory>(ni, _rectangle_1color_broadcast_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED);
            if (rc == PAMI_SUCCESS) new ((void*)_rectangle_1color_broadcast_factory) CCMI::Adaptor::P2PBroadcast::Rectangle1ColorBroadcastFactory(_context,_context_id,mapidtogeometry,&_color_connmgr, ni);
            // ----------------------------------------------------
#endif
            // ----------------------------------------------------
            // Setup and Construct a binomial broadcast factory from allsided ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PBroadcast::BinomialBroadcastFactory>(ni, _binomial_broadcast_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED);
            if (rc == PAMI_SUCCESS) new ((void*)_binomial_broadcast_factory) CCMI::Adaptor::P2PBroadcast::BinomialBroadcastFactory(_context,_context_id,mapidtogeometry,&_cg_connmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct a binomial broadcast factory
            // single threaded from allsided ni 
            rc = setupFactory<CCMI::Adaptor::P2PBroadcast::BinomialBroadcastSingleThFactory>(ni, _binomial_broadcast_singleth_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED, 1);
            if (rc == PAMI_SUCCESS) new ((void*)_binomial_broadcast_singleth_factory) CCMI::Adaptor::P2PBroadcast::BinomialBroadcastSingleThFactory(_context,_context_id,mapidtogeometry,&_color_connmgr, ni);
            // ----------------------------------------------------
            // ----------------------------------------------------
            // Setup and Construct a ring broadcast factory from
            // allsided ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PBroadcast::RingBroadcastFactory>(ni, _ring_broadcast_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED);
            if (rc == PAMI_SUCCESS) new ((void*)_ring_broadcast_factory) CCMI::Adaptor::P2PBroadcast::RingBroadcastFactory(_context,_context_id,mapidtogeometry,&_cg_connmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct a ring broadcast factory from allsided ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PBroadcast::RingBroadcastSingleThFactory>(ni, _ring_broadcast_singleth_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED, 1);
            if (rc == PAMI_SUCCESS) new ((void*)_ring_broadcast_singleth_factory) CCMI::Adaptor::P2PBroadcast::RingBroadcastSingleThFactory(_context,_context_id,mapidtogeometry,&_color_connmgr, ni);

            //This scheme only works ...??
            rc = setupFactory<CCMI::Adaptor::P2PBroadcast::TwoNaryBroadcastFactory>(ni, _sync_2nary_broadcast_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED, 1);
            if (rc == PAMI_SUCCESS) new ((void*)_sync_2nary_broadcast_factory) CCMI::Adaptor::P2PBroadcast::TwoNaryBroadcastFactory(_context,_context_id, mapidtogeometry, &_color_connmgr, ni);

            // ----------------------------------------------------
            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num binomial broadcast factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PBroadcast::AsyncCSBinomialBroadcastFactory>(ni, _ascs_binomial_broadcast_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_binomial_broadcast_factory) CCMI::Adaptor::P2PBroadcast::AsyncCSBinomialBroadcastFactory(_context,_context_id, mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num binomial broadcast factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PBroadcast::Async2nomialBroadcastFactory>(ni, _2nomial_broadcast_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_2nomial_broadcast_factory) CCMI::Adaptor::P2PBroadcast::Async2nomialBroadcastFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num 3nomial broadcast factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PBroadcast::Async3nomialBroadcastFactory>(ni, _3nomial_broadcast_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_3nomial_broadcast_factory) CCMI::Adaptor::P2PBroadcast::Async3nomialBroadcastFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num 4nomial broadcast factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PBroadcast::Async4nomialBroadcastFactory>(ni,  _4nomial_broadcast_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_4nomial_broadcast_factory) CCMI::Adaptor::P2PBroadcast::Async4nomialBroadcastFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num 2nary broadcast factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PBroadcast::Async2naryBroadcastFactory>(ni,  _2nary_broadcast_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_2nary_broadcast_factory) CCMI::Adaptor::P2PBroadcast::Async2naryBroadcastFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num 3nary broadcast factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PBroadcast::Async3naryBroadcastFactory>(ni, _3nary_broadcast_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_3nary_broadcast_factory) CCMI::Adaptor::P2PBroadcast::Async3naryBroadcastFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num 4nary broadcast factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PBroadcast::Async4naryBroadcastFactory>(ni, _4nary_broadcast_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_4nary_broadcast_factory) CCMI::Adaptor::P2PBroadcast::Async4naryBroadcastFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            
            // ----------------------------------------------------
            // Setup and Construct an asynchronous, rank based binomial broadcast factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PBroadcast::AsyncRBBinomialBroadcastFactory>(ni,_asrb_binomial_broadcast_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_asrb_binomial_broadcast_factory) CCMI::Adaptor::P2PBroadcast::AsyncRBBinomialBroadcastFactory(_context,_context_id,mapidtogeometry,&_rbconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct a rank based binomial active message broadcast factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PAMBroadcast::Binomial::Factory>(ni, _active_binomial_broadcast_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE, -1, true);
            if (rc == PAMI_SUCCESS) new ((void*)_active_binomial_broadcast_factory) CCMI::Adaptor::P2PAMBroadcast::Binomial::Factory(_context, _context_id, mapidtogeometry, &_rsconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct a binomial allreducefactory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PAllreduce::Binomial::Factory>(ni,  _binomial_allreduce_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) {
              typedef CCMI::Adaptor::P2PAllreduce::Binomial::Composite Composite;
              typedef CCMI::Adaptor::P2PAllreduce::Binomial::Factory   Factory;
              typedef CCMI::Adaptor::P2PAllreduce::Binomial::OAT       OAT;
              new ((void*)_binomial_allreduce_factory) Factory(_context,_context_id,mapidtogeometry,&_rbconnmgr, ni, OAT::cb_async_OAT_receiveHead);
              _binomial_allreduce_factory->setAsync();
	    }

            // ----------------------------------------------------
            rc = setupFactory<CCMI::Adaptor::P2PAllreduce::Binomial::Factory4>(ni,  _binomial4_allreduce_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) {
              typedef CCMI::Adaptor::P2PAllreduce::Binomial::Composite4 Composite4;
              typedef CCMI::Adaptor::P2PAllreduce::Binomial::Factory4   Factory4;
              typedef CCMI::Adaptor::P2PAllreduce::Binomial::OAT4       OAT4;
              new ((void*)_binomial4_allreduce_factory) Factory4(_context,_context_id,mapidtogeometry,&_rbconnmgr, ni, OAT4::cb_async_OAT_receiveHead);
              _binomial4_allreduce_factory->setAsync();
	    }


            // ----------------------------------------------------
            rc = setupFactory<CCMI::Adaptor::P2PAllreduce::Binomial::Factory8>(ni,  _binomial8_allreduce_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) {
              typedef CCMI::Adaptor::P2PAllreduce::Binomial::Composite8 Composite8;
              typedef CCMI::Adaptor::P2PAllreduce::Binomial::Factory8   Factory8;
              typedef CCMI::Adaptor::P2PAllreduce::Binomial::OAT8       OAT8;
              new ((void*)_binomial8_allreduce_factory) Factory8(_context,_context_id,mapidtogeometry,&_rbconnmgr, ni, OAT8::cb_async_OAT_receiveHead);
              _binomial8_allreduce_factory->setAsync();
	    }
	      
            // ----------------------------------------------------
#ifdef ENABLE_X0_PROTOCOLS // Experimental (X0:) protocols
            rc = setupFactory<CCMI::Adaptor::P2PAllreduce::Ring::Factory>(ni, _ring_allreduce_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED, 1);
            if (rc == PAMI_SUCCESS) new ((void*)_ring_allreduce_factory) CCMI::Adaptor::P2PAllreduce::Ring::Factory(_context,_context_id,mapidtogeometry,&_sconnmgr, ni, NULL, &_sconnmgr1);
#endif
            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num binomial  allreduce factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PAllreduce::Binomial::AsyncCSBinomAllreduceFactory>(ni, _ascs_binomial_allreduce_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_binomial_allreduce_factory) CCMI::Adaptor::P2PAllreduce::Binomial::AsyncCSBinomAllreduceFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num binomial  allreduce factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PAllreduce::Binomial::AsyncCSBinomReduceFactory>(ni, _ascs_binomial_reduce_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_binomial_reduce_factory) CCMI::Adaptor::P2PAllreduce::Binomial::AsyncCSBinomReduceFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct a rank based binomial active message reduce factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PAMReduce::Binomial::Factory>(ni, _active_binomial_amreduce_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE, -1, true);
            if (rc == PAMI_SUCCESS) new ((void*)_active_binomial_amreduce_factory) CCMI::Adaptor::P2PAMReduce::Binomial::Factory(_context,_context_id,mapidtogeometry,&_rsconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num binomial scatter factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PScatter::Binomial::Factory>(ni, _ascs_binomial_scatter_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_binomial_scatter_factory) CCMI::Adaptor::P2PScatter::Binomial::Factory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num flat scatter factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PScatter::Flat::Factory>(ni, _ascs_flat_scatter_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_flat_scatter_factory) CCMI::Adaptor::P2PScatter::Flat::Factory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num scatterv factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PScatterv::Factory>(ni, _ascs_scatterv_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_scatterv_factory) CCMI::Adaptor::P2PScatterv::Factory(_context,_context_id, mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num scatterv_int factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PScatterv::IntFactory>(ni, _ascs_scatterv_int_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_scatterv_int_factory) CCMI::Adaptor::P2PScatterv::IntFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct a rank based binomial active message scatter factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PAMScatter::Binomial::Factory>(ni, _active_binomial_amscatter_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE, -1, true);
            if (rc == PAMI_SUCCESS) new ((void*)_active_binomial_amscatter_factory) CCMI::Adaptor::P2PAMScatter::Binomial::Factory(_context,_context_id,mapidtogeometry,&_rsconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num scan factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PScan::Binomial::Factory>(ni, _ascs_binomial_scan_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_binomial_scan_factory) CCMI::Adaptor::P2PScan::Binomial::Factory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num reduce_scatter factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PReduceScatter::AsyncCSReduceScatterFactory>(ni, _ascs_reduce_scatter_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_reduce_scatter_factory) CCMI::Adaptor::P2PReduceScatter::AsyncCSReduceScatterFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num binomial gather factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PGather::Binomial::Factory>(ni, _ascs_binomial_gather_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_binomial_gather_factory) CCMI::Adaptor::P2PGather::Binomial::Factory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num flat gather factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PGather::Flat::Factory>(ni, _ascs_flat_gather_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_flat_gather_factory) CCMI::Adaptor::P2PGather::Flat::Factory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct a rank based binomial active message gather factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PAMGather::Binomial::Factory>(ni, _active_binomial_gather_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE, -1, true);
            if (rc == PAMI_SUCCESS) new ((void*)_active_binomial_gather_factory) CCMI::Adaptor::P2PAMGather::Binomial::Factory(_context,_context_id,mapidtogeometry,&_rsconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num gatherv factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PGatherv::Factory>(ni, _ascs_gatherv_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_gatherv_factory) CCMI::Adaptor::P2PGatherv::Factory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num gatherv_int factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PGatherv::IntFactory>(ni, _ascs_gatherv_int_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_gatherv_int_factory) CCMI::Adaptor::P2PGatherv::IntFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num allgather factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PAllgather::Binomial::Factory>(ni, _ascs_binomial_allgather_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_binomial_allgather_factory) CCMI::Adaptor::P2PAllgather::Binomial::Factory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num allgather factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PAllgatherv::Ring::AllgatherFactory>(ni, _ascs_ring_allgather_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_ring_allgather_factory) CCMI::Adaptor::P2PAllgatherv::Ring::AllgatherFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num allgatherv factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PAllgatherv::Ring::AllgathervFactory>(ni, _ascs_ring_allgatherv_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_ring_allgatherv_factory) CCMI::Adaptor::P2PAllgatherv::Ring::AllgathervFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num allgatherv factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PAllgatherv::Ring::AllgathervIntFactory>(ni, _ascs_ring_allgatherv_int_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_ring_allgatherv_int_factory) CCMI::Adaptor::P2PAllgatherv::Ring::AllgathervIntFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num alltoall factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PAlltoallv::Pairwise::AlltoallFactory>(ni, _ascs_pairwise_alltoall_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_pairwise_alltoall_factory) CCMI::Adaptor::P2PAlltoallv::Pairwise::AlltoallFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num alltoallv factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PAlltoallv::Pairwise::AlltoallvIntFactory>(ni, _ascs_pairwise_alltoallv_int_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_pairwise_alltoallv_int_factory) CCMI::Adaptor::P2PAlltoallv::Pairwise::AlltoallvIntFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // ----------------------------------------------------
            // Setup and Construct an asynchronous, comm_id/seq_num alltoallv factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PAlltoallv::Pairwise::AlltoallvFactory>(ni, _ascs_pairwise_alltoallv_factory, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_ascs_pairwise_alltoallv_factory) CCMI::Adaptor::P2PAlltoallv::Pairwise::AlltoallvFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // Setup and Construct an alltoall factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PAlltoall::All2AllFactory>(ni, _alltoall_factory, CCMI::Interfaces::NativeInterfaceFactory::MANYTOMANY, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_alltoall_factory) CCMI::Adaptor::P2PAlltoall::All2AllFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // Setup and Construct an alltoall factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PAlltoallv::All2AllvFactory>(ni, _alltoallv_factory, CCMI::Interfaces::NativeInterfaceFactory::MANYTOMANY, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_alltoallv_factory) CCMI::Adaptor::P2PAlltoallv::All2AllvFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            // Setup and Construct an alltoall factory from active message ni and p2p protocol
            rc = setupFactory<CCMI::Adaptor::P2PAlltoallv::All2AllvFactory_int>(ni, _alltoallv_factory_int, CCMI::Interfaces::NativeInterfaceFactory::MANYTOMANY, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE);
            if (rc == PAMI_SUCCESS) new ((void*)_alltoallv_factory_int) CCMI::Adaptor::P2PAlltoallv::All2AllvFactory_int(_context,_context_id,mapidtogeometry,&_csconnmgr, ni);
            // ----------------------------------------------------

            TRACE_FN_EXIT();
          }

      private:
          /// \todo use allocator instead of _storage?  Since they aren't always constructed, we waste memory now.
          pami_client_t                                                _client;
          pami_context_t                                               _context;
          size_t                                                       _context_id;
          size_t                                                       _client_id;
          std::map<unsigned, pami_geometry_t>                        * _geometry_map;
          uint64_t                                                     _reduce_val;

          CCMI::Interfaces::NativeInterfaceFactory                   * _ni_factory; //the native interface factory to build the native interfaces
          CCMI::Interfaces::NativeInterfaceFactory                   * _ni_factory_amc; //the native interface factory for AM collectives 

          // This is a pointer to the current dispatch id of the context
          // This will be decremented by the ConstructNativeInterface routines
          int                                                         *_dispatch_id;

          T_Allocator                                                 &_allocator;
          PAMI::MemoryAllocator<sizeof(GeometryInfo),16>               _geom_allocator;

          // Barrier Storage and Native Interface
          T_BinomialBarrier                                           *_binomial_barrier_composite;

          // OneTask Barrier Storage
          CCMI::Adaptor::P2POneTask::OneTaskBarrier                   *_onetask_barrier_composite;

          // CCMI Connection Manager Class
          CCMI::ConnectionManager::ColorGeometryConnMgr                _cg_connmgr;
          CCMI::ConnectionManager::ColorConnMgr                        _color_connmgr;
	  CCMI::ConnectionManager::SimpleConnMgr                       _sconnmgr;
	  CCMI::ConnectionManager::SimpleConnMgr                       _sconnmgr1;  
	  CCMI::ConnectionManager::RankBasedConnMgr                    _rbconnmgr;
          CCMI::ConnectionManager::CommSeqConnMgr                      _csconnmgr;
          CCMI::ConnectionManager::RankSeqConnMgr                      _rsconnmgr;

          // CCMI Barrier Interface
          T_BinomialBarrierFactory                                        *_binomial_barrier_factory;

          // CCMI OneTask Barrier Interface
          CCMI::Adaptor::P2POneTask::OneTaskBarrierFactory                *_onetask_barrier_factory;


          //OneTask Factories
          //CCMI::Adaptor::P2POneTask::OneTaskFenceFactory                  *_onetask_fence_factory;
          CCMI::Adaptor::P2POneTask::OneTaskBroadcastFactory              *_onetask_broadcast_factory;
          CCMI::Adaptor::P2POneTask::OneTaskReduceFactory                 *_onetask_reduce_factory;
          CCMI::Adaptor::P2POneTask::OneTaskAllreduceFactory              *_onetask_allreduce_factory;
          CCMI::Adaptor::P2POneTask::OneTaskAllgatherFactory              *_onetask_allgather_factory;
          CCMI::Adaptor::P2POneTask::OneTaskAllgathervFactory             *_onetask_allgatherv_factory;
          CCMI::Adaptor::P2POneTask::OneTaskAllgathervIntFactory          *_onetask_allgatherv_int_factory;
          CCMI::Adaptor::P2POneTask::OneTaskScatterFactory                *_onetask_scatter_factory;
          CCMI::Adaptor::P2POneTask::OneTaskScattervFactory               *_onetask_scatterv_factory;
          CCMI::Adaptor::P2POneTask::OneTaskScattervIntFactory            *_onetask_scatterv_int_factory;
          CCMI::Adaptor::P2POneTask::OneTaskGatherFactory                 *_onetask_gather_factory;
          CCMI::Adaptor::P2POneTask::OneTaskGathervFactory                *_onetask_gatherv_factory;
          CCMI::Adaptor::P2POneTask::OneTaskGathervIntFactory             *_onetask_gatherv_int_factory;
          CCMI::Adaptor::P2POneTask::OneTaskAlltoallFactory               *_onetask_alltoall_factory;
          CCMI::Adaptor::P2POneTask::OneTaskAlltoallvFactory              *_onetask_alltoallv_factory;
          CCMI::Adaptor::P2POneTask::OneTaskAlltoallvIntFactory           *_onetask_alltoallv_int_factory;
          CCMI::Adaptor::P2POneTask::OneTaskScanFactory                   *_onetask_scan_factory;
          CCMI::Adaptor::P2POneTask::OneTaskReduceScatterFactory          *_onetask_reduce_scatter_factory;
          CCMI::Adaptor::P2POneTask::OneTaskAMBroadcastFactory            *_onetask_ambroadcast_factory;
          CCMI::Adaptor::P2POneTask::OneTaskAMScatterFactory              *_onetask_amscatter_factory;
          CCMI::Adaptor::P2POneTask::OneTaskAMGatherFactory               *_onetask_amgather_factory;
          CCMI::Adaptor::P2POneTask::OneTaskAMReduceFactory               *_onetask_amreduce_factory;

          // CCMI Broadcasts
#ifdef PAMI_ENABLE_X0_PROTOCOLS // Experimental (X0:) protocols
          CCMI::Adaptor::P2PBroadcast::RectangleBroadcastFactory          *_rectangle_broadcast_factory;
          CCMI::Adaptor::P2PBroadcast::Rectangle1ColorBroadcastFactory    *_rectangle_1color_broadcast_factory;
#endif
          CCMI::Adaptor::P2PBroadcast::BinomialBroadcastFactory           *_binomial_broadcast_factory;
          CCMI::Adaptor::P2PBroadcast::RingBroadcastFactory               *_ring_broadcast_factory;          
          CCMI::Adaptor::P2PBroadcast::BinomialBroadcastSingleThFactory   *_binomial_broadcast_singleth_factory;
          CCMI::Adaptor::P2PBroadcast::RingBroadcastSingleThFactory       *_ring_broadcast_singleth_factory;          
          CCMI::Adaptor::P2PBroadcast::TwoNaryBroadcastFactory            *_sync_2nary_broadcast_factory;
          CCMI::Adaptor::P2PBroadcast::AsyncRBBinomialBroadcastFactory    *_asrb_binomial_broadcast_factory;
          CCMI::Adaptor::P2PBroadcast::AsyncCSBinomialBroadcastFactory    *_ascs_binomial_broadcast_factory;
          CCMI::Adaptor::P2PBroadcast::Async2nomialBroadcastFactory       *_2nomial_broadcast_factory;
          CCMI::Adaptor::P2PBroadcast::Async3nomialBroadcastFactory       *_3nomial_broadcast_factory;
          CCMI::Adaptor::P2PBroadcast::Async4nomialBroadcastFactory       *_4nomial_broadcast_factory;
          CCMI::Adaptor::P2PBroadcast::Async2naryBroadcastFactory         *_2nary_broadcast_factory;
          CCMI::Adaptor::P2PBroadcast::Async3naryBroadcastFactory         *_3nary_broadcast_factory;
          CCMI::Adaptor::P2PBroadcast::Async4naryBroadcastFactory         *_4nary_broadcast_factory;
          CCMI::Adaptor::P2PAMBroadcast::Binomial::Factory                *_active_binomial_broadcast_factory;

          // CCMI Binomial Allreduce
          CCMI::Adaptor::P2PAllreduce::Binomial::Factory                  *_binomial_allreduce_factory;
          CCMI::Adaptor::P2PAllreduce::Binomial::Factory4                 *_binomial4_allreduce_factory;
          CCMI::Adaptor::P2PAllreduce::Binomial::Factory8                 *_binomial8_allreduce_factory;

#ifdef ENABLE_X0_PROTOCOLS // Experimental (X0:) protocols
          CCMI::Adaptor::P2PAllreduce::Ring::Factory                      *_ring_allreduce_factory;
#endif

          // CCMI Async [All]Reduce
          CCMI::Adaptor::P2PAllreduce::Binomial::AsyncCSBinomAllreduceFactory  *_ascs_binomial_allreduce_factory;
          CCMI::Adaptor::P2PAllreduce::Binomial::AsyncCSBinomReduceFactory     *_ascs_binomial_reduce_factory;
          CCMI::Adaptor::P2PAMReduce::Binomial::Factory                        *_active_binomial_amreduce_factory;

          // CCMI Async Gather/Scatter
          CCMI::Adaptor::P2PScatter::Binomial::Factory                    *_ascs_binomial_scatter_factory;
          CCMI::Adaptor::P2PScatter::Flat::Factory                        *_ascs_flat_scatter_factory;
          CCMI::Adaptor::P2PScatterv::Factory                             *_ascs_scatterv_factory;
          CCMI::Adaptor::P2PScatterv::IntFactory                          *_ascs_scatterv_int_factory;
          CCMI::Adaptor::P2PAMScatter::Binomial::Factory                  *_active_binomial_amscatter_factory;
          CCMI::Adaptor::P2PScan::Binomial::Factory                       *_ascs_binomial_scan_factory;
          CCMI::Adaptor::P2PReduceScatter::AsyncCSReduceScatterFactory    *_ascs_reduce_scatter_factory;
          CCMI::Adaptor::P2PGather::Binomial::Factory                     *_ascs_binomial_gather_factory;
          CCMI::Adaptor::P2PGather::Flat::Factory                         *_ascs_flat_gather_factory;
          CCMI::Adaptor::P2PAMGather::Binomial::Factory                   *_active_binomial_gather_factory;
          CCMI::Adaptor::P2PGatherv::Factory                              *_ascs_gatherv_factory;
          CCMI::Adaptor::P2PGatherv::IntFactory                           *_ascs_gatherv_int_factory;
          CCMI::Adaptor::P2PAllgather::Binomial::Factory                  *_ascs_binomial_allgather_factory;
          CCMI::Adaptor::P2PAllgatherv::Ring::AllgatherFactory            *_ascs_ring_allgather_factory;
          CCMI::Adaptor::P2PAllgatherv::Ring::AllgathervFactory           *_ascs_ring_allgatherv_factory;
          CCMI::Adaptor::P2PAllgatherv::Ring::AllgathervIntFactory        *_ascs_ring_allgatherv_int_factory;
          CCMI::Adaptor::P2PAlltoallv::Pairwise::AlltoallFactory          *_ascs_pairwise_alltoall_factory;
          CCMI::Adaptor::P2PAlltoallv::Pairwise::AlltoallvIntFactory      *_ascs_pairwise_alltoallv_int_factory;
          CCMI::Adaptor::P2PAlltoallv::Pairwise::AlltoallvFactory         *_ascs_pairwise_alltoallv_factory;

          // CCMI Alltoall
          CCMI::Adaptor::P2PAlltoall::All2AllFactory                      *_alltoall_factory;
          // CCMI Alltoallv
          CCMI::Adaptor::P2PAlltoallv::All2AllvFactory                    *_alltoallv_factory;
          CCMI::Adaptor::P2PAlltoallv::All2AllvFactory_int                *_alltoallv_factory_int;
      };
    }; // P2P
  }; // CollRegistration
}; // PAMI

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
