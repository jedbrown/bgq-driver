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
 * \file algorithms/geometry/PGASCollRegistration.h
 * \brief ???
 */

#ifndef __algorithms_geometry_PGASCollRegistration_h__
#define __algorithms_geometry_PGASCollRegistration_h__

#include <map>
#include <list>
#include <vector>
#include "algorithms/interfaces/CollRegistrationInterface.h"
#include "algorithms/geometry/PGASWrapper.h"
#include "common/NativeInterface.h"

#ifndef SETUPNI_P2P_SHMEM
#define SETUPNI_P2P_SHMEM(NI_PTR) result =                                      \
  NativeInterfaceCommon::constructNativeInterface                               \
  <T_Allocator, T_NI, T_Protocol1, T_Device_P2P, T_Protocol2, T_Device_SHMEM,   \
   NativeInterfaceCommon::P2P_ONLY>(                                            \
                                         _proto_alloc,                          \
                                         _dev_p2p,                              \
                                         _dev_shmem,                            \
                                         NI_PTR,                                \
                                         _client,                               \
                                         _context,                              \
                                         _context_id,                           \
                                         _client_id,                            \
                                         _dispatch_id)

#define SETUPNI_P2P_P2P(NI_PTR) result =                                        \
  NativeInterfaceCommon::constructNativeInterface                               \
  <T_Allocator, T_NI, T_Protocol1, T_Device_P2P, T_Protocol1, T_Device_P2P,     \
   NativeInterfaceCommon::P2P_ONLY>(                                            \
                                         _proto_alloc,                          \
                                         _dev_p2p,                              \
                                         _dev_p2p,                              \
                                         NI_PTR,                                \
                                         _client,                               \
                                         _context,                              \
                                         _context_id,                           \
                                         _client_id,                            \
                                         _dispatch_id)

#define SETUPNI_P2P(NI_PTR) result =                                            \
  NativeInterfaceCommon::constructNativeInterface                               \
  <T_Allocator, T_NI, T_Protocol1, T_Device_P2P,                                \
   NativeInterfaceCommon::P2P_ONLY>(                                            \
                                         _proto_alloc,                          \
                                         _dev_p2p,                              \
                                         NI_PTR,                                \
                                         _client,                               \
                                         _context,                              \
                                         _context_id,                           \
                                         _client_id,                            \
                                         _dispatch_id)
#endif
namespace PAMI
{
  namespace CollRegistration
  {
    static const char BarrierString[]        = "I1:Barrier:P2P:P2P";
    static const char BroadcastString[]      = "I1:Broadcast:P2P:P2P";
    static const char AllgatherString[]      = "I1:Allgather:P2P:P2P";
    static const char AllgathervString[]     = "I1:Allgatherv:P2P:P2P";
    static const char ScatterString[]        = "I1:Scatter:P2P:P2P";
    static const char ScattervString[]       = "I1:Scatterv:P2P:P2P";
    static const char GatherString[]         = "I1:Gather:P2P:P2P";
    static const char AlltoallString[]       = "I1:Alltoall:P2P:P2P";
    static const char AlltoallvString[]      = "I1:Alltoallv:P2P:P2P";
    static const char AlltoallvintString[]   = "I1:Alltoallv_int:P2P:P2P";
    static const char AllreduceString[]      = "I1:Allreduce:P2P:P2P";
    static const char ScanString[]           = "I1:Scan:P2P:P2P";
    static const char ShortAllreduceString[] = "I1:ShortAllreduce:P2P:P2P";

#ifdef XLPGAS_PAMI_CAU
    //hybrid pgas
    static const char HybridAllreduceString[] = "I1:HybridShortAllreduce:SHMEM:CAU";
    static const char HybridBroadcastString[] = "I1:HybridBroadcast:SHMEM:P2P";
#endif

    template <class T_Geometry,
              class T_NI,
              class T_Allocator,
              class T_Protocol1,
              class T_Protocol2,
              class T_Device_P2P,
              class T_Device_SHMEM,
              class T_NBCollMgr,
              class T_CSMemoryManager >
    class PGASRegistration :
       public PAMI::CollRegistration::CollRegistration<PAMI::CollRegistration::PGASRegistration<T_Geometry,
                                                                       T_NI,
                                                                       T_Allocator,
                                                                       T_Protocol1,
                                                                       T_Protocol2,
                                                                       T_Device_P2P,
                                                                       T_Device_SHMEM,
                                                                       T_NBCollMgr,
                                                                       T_CSMemoryManager>,
                              T_Geometry>
      {
        typedef PGBarrierExec<T_Geometry,xlpgas::Barrier<T_NI>, T_NI, T_Device_P2P> BarExec;
        typedef PGBroadcastExec<T_Geometry,xlpgas::Broadcast<T_NI>, T_NI, T_Device_P2P> BroadcastExec;
        typedef PGAllgatherExec<T_Geometry,xlpgas::Allgather<T_NI>, T_NI, T_Device_P2P> AllgatherExec;
        typedef PGAllgathervExec<T_Geometry,xlpgas::Allgatherv<T_NI>, T_NI, T_Device_P2P> AllgathervExec;
        typedef PGScatterExec<T_Geometry,xlpgas::Scatter<T_NI>, T_NI, T_Device_P2P,xlpgas::Barrier<T_NI> > ScatterExec;
        typedef PGGatherExec<T_Geometry,xlpgas::Gather<T_NI>, T_NI, T_Device_P2P,xlpgas::Barrier<T_NI> > GatherExec;
        typedef PGAlltoallExec<T_Geometry,xlpgas::Alltoall<T_NI>, T_NI, T_Device_P2P,xlpgas::Barrier<T_NI> > AlltoallExec;
        typedef PGAlltoallvExec<T_Geometry,xlpgas::Alltoallv<T_NI,size_t>, T_NI, T_Device_P2P,xlpgas::Barrier<T_NI> > AlltoallvExec;
	typedef PGAlltoallvintExec<T_Geometry,xlpgas::Alltoallv<T_NI,int>, T_NI, T_Device_P2P,xlpgas::Barrier<T_NI> > AlltoallvintExec;
        typedef PGAllreduceExec<T_Geometry,xlpgas::Allreduce::Long<T_NI>, T_NI, T_Device_P2P> AllreduceExec;
        typedef PGScanExec<T_Geometry,xlpgas::PrefixSums<T_NI>, T_NI, T_Device_P2P> ScanExec;
        typedef PGAllreduceExec<T_Geometry,xlpgas::Allreduce::Short<T_NI>, T_NI, T_Device_P2P> ShortAllreduceExec;
#ifdef XLPGAS_PAMI_CAU
	//hybrid all reduce
        typedef PGAllreduceExec<T_Geometry,xlpgas::ShmCauAllReduce<T_NI,T_Device_P2P>, T_NI, T_Device_P2P> HybridAllreduceExec;
        typedef PGBroadcastExec<T_Geometry,xlpgas::ShmHybridPipelinedBcast<T_NI,T_Device_P2P>, T_NI, T_Device_P2P> HybridBroadcastExec;
#endif
        typedef PGFactory<xlpgas::Barrier<T_NI>,T_NI,T_Device_P2P,BarExec> BarrierFactory;
        typedef PGFactory<xlpgas::Broadcast<T_NI>,T_NI,T_Device_P2P,BroadcastExec> BroadcastFactory;
        typedef PGFactory<xlpgas::Allgather<T_NI>,T_NI,T_Device_P2P,AllgatherExec>  AllgatherFactory;
        typedef PGFactory<xlpgas::Allgatherv<T_NI>,T_NI,T_Device_P2P,AllgathervExec> AllgathervFactory;
        typedef PGFactory<xlpgas::Scatter<T_NI>,T_NI,T_Device_P2P,ScatterExec, xlpgas::Barrier<T_NI> > ScatterFactory;
        typedef PGFactory<xlpgas::Gather<T_NI>,T_NI,T_Device_P2P,GatherExec, xlpgas::Barrier<T_NI> > GatherFactory;
        typedef PGFactory<xlpgas::Alltoall<T_NI>,T_NI,T_Device_P2P,AlltoallExec, xlpgas::Barrier<T_NI> > AlltoallFactory;
        typedef PGFactory<xlpgas::Alltoallv<T_NI,size_t>,T_NI,T_Device_P2P,AlltoallvExec, xlpgas::Barrier<T_NI> > AlltoallvFactory;
        typedef PGFactory<xlpgas::Alltoallv<T_NI,int>,T_NI,T_Device_P2P,AlltoallvintExec, xlpgas::Barrier<T_NI> > AlltoallvintFactory;
        typedef PGFactory<xlpgas::Allreduce::Long<T_NI>,T_NI,T_Device_P2P,AllreduceExec>  AllreduceFactory;
        typedef PGFactory<xlpgas::PrefixSums<T_NI>,T_NI,T_Device_P2P,ScanExec>  ScanFactory;
        typedef PGFactory<xlpgas::Allreduce::Short<T_NI>,T_NI,T_Device_P2P,ShortAllreduceExec> ShortAllreduceFactory;
#ifdef XLPGAS_PAMI_CAU
	//hybrid all reduce - three shm only and two distributed hybrid
	typedef PGFactory<xlpgas::ShmCauAllReduce<T_NI,T_Device_P2P>,T_NI,T_Device_P2P,HybridAllreduceExec> HybridAllreduceFactory;
	typedef PGFactory<xlpgas::ShmHybridPipelinedBcast<T_NI,T_Device_P2P>,T_NI,T_Device_P2P,HybridBroadcastExec> HybridBroadcastFactory;
#endif
        typedef union Factories
        {
          char barrier_blob[sizeof(BarrierFactory)];
          char broadcast_blob[sizeof(BroadcastFactory)];
          char allgather_blob[sizeof(AllgatherFactory)];
          char allgatherv_blob[sizeof(AllgathervFactory)];
          char scatter_blob[sizeof(ScatterFactory)];
          char gather_blob[sizeof(GatherFactory)];
          char alltoall_blob[sizeof(AlltoallFactory)];
          char alltoallv_blob[sizeof(AlltoallvFactory)];
          char alltoallvint_blob[sizeof(AlltoallvintFactory)];
          char allreduce_blob[sizeof(AllreduceFactory)];
          char scan_blob[sizeof(ScanFactory)];
          char shortallreduce_blob[sizeof(ShortAllreduceFactory)];
#ifdef XLPGAS_PAMI_CAU
	  //shm/hybrid
	  char hybridshortallreduce_blob[sizeof(HybridAllreduceFactory)];
	  char hybridbroadcast_blob[sizeof(HybridBroadcastFactory)];
#endif
        }Factories;

        class GeometryInfo
        {
        public:
          inline GeometryInfo(void *factory_allocator,
                              void *geom_allocator):
            _factory_allocator(factory_allocator),
            _geom_allocator(geom_allocator), _mm(NULL), _device_info(NULL)
            {
            }
          std::list<Factories*>   _f_list;
          std::list<void *>       _nbcoll_list;
          void                   *_factory_allocator;
          void                   *_geom_allocator;
	  size_t                 _pgas_shmem_offset;
	  T_CSMemoryManager      *_mm;
          void                   *_device_info;
        };

      public:
      /// \brief The one device (p2p) pgas constructor.
      ///
      /// It takes two devices because this class uses references to the device so *something* has
      /// to be passed in even if it's ignored.
      ///
      inline PGASRegistration(pami_client_t                        client,
                              pami_context_t                       context,
                              size_t                               client_id,
                              size_t                               context_id,
                              T_Allocator                         &proto_alloc,
                              T_Device_P2P                        &dev_p2p,
                              T_Device_SHMEM                      &dev_shmem,
			      void                                *comm_handle, // For lapi client
                              int                                 *dispatch_id,
                              std::map<unsigned, pami_geometry_t> *geometry_map):
        CollRegistration<PAMI::CollRegistration::PGASRegistration<T_Geometry,
                                                                  T_NI,
                                                                  T_Allocator,
                                                                  T_Protocol1,
                                                                  T_Protocol2,
                                                                  T_Device_P2P,
                                                                  T_Device_SHMEM,
	                                                          T_NBCollMgr,
	                                                          T_CSMemoryManager>,
                         T_Geometry> (),
        _client(client),
        _context(context),
        _client_id(client_id),
        _context_id(context_id),
        _mgr(_context_id),
        _reduce_val(0),
	_comm_handle(comm_handle),
        _dispatch_id(dispatch_id),
        _geometry_map(geometry_map),
        _dev_p2p(dev_p2p),
        _dev_shmem(dev_shmem),
        _proto_alloc(proto_alloc),
	_mm(NULL),
        _allgather(NULL),
        _allgatherv(NULL),
        _scatter_s(NULL),
        _scatter_b(NULL),
        _gather_s(NULL),
        _gather_b(NULL),
        _alltoall_s(NULL),
        _alltoall_b(NULL),
        _alltoallv_s(NULL),
        _alltoallv_b(NULL),	
        _alltoallvint(NULL),  
        _allreduce(NULL),
        _shortallreduce(NULL),
        _barrier(NULL),
        _broadcast(NULL)
#ifdef XLPGAS_PAMI_CAU
	,
	_shm_shortreduce(NULL),
	_shm_shortbcast(NULL),
	_shm_largebcast(NULL),
	_cau_shortreduce(NULL),
	_cau_shortbcast(NULL),
	_leaders_bcast(NULL),
	_hybrid_shortallreduce(NULL),
	_hybrid_bcast(NULL),
        _hybrid_pipelined_bcast(NULL)
#endif
          {
            pami_result_t       result   = PAMI_SUCCESS;

            SETUPNI_P2P(_allgather);
            SETUPNI_P2P(_allgatherv);
            SETUPNI_P2P(_scatter_s);
            SETUPNI_P2P(_scatter_b);
            SETUPNI_P2P(_gather_s);
            SETUPNI_P2P(_gather_b);
            SETUPNI_P2P(_alltoall_s);
            SETUPNI_P2P(_alltoall_b);
            SETUPNI_P2P(_alltoallv_s);
            SETUPNI_P2P(_alltoallv_b);
            SETUPNI_P2P(_alltoallvint);
            SETUPNI_P2P(_allreduce);
            SETUPNI_P2P(_scan);
            SETUPNI_P2P(_shortallreduce);
            SETUPNI_P2P(_barrier);
            SETUPNI_P2P(_broadcast);
#ifdef XLPGAS_PAMI_CAU
	    // ???
	    SETUPNI_P2P_P2P(_shm_shortreduce);
	    SETUPNI_P2P_P2P(_shm_shortbcast);
	    SETUPNI_P2P_P2P(_shm_largebcast);
	    SETUPNI_P2P_P2P(_cau_shortreduce);
	    SETUPNI_P2P_P2P(_cau_shortbcast);
	    SETUPNI_P2P_P2P(_leaders_bcast);
	    SETUPNI_P2P_P2P(_hybrid_shortallreduce);
	    SETUPNI_P2P_P2P(_hybrid_bcast);
            SETUPNI_P2P_P2P(_hybrid_pipelined_bcast);
            lapi_handle_t hdl = *((lapi_handle_t*)_comm_handle);            
            _mgr.InitializeLapi(hdl,&_mgr); // Initialize for 1 context
#endif
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::AllgatherKind,      _allgather);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::AllgathervKind,     _allgatherv);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::ScatterKind,        _scatter_s);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::BarrierKind,        _scatter_b);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::GatherKind,         _gather_s);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::BarrierKind,        _gather_b);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::AlltoallKind,       _alltoall_s);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::BarrierKind,        _alltoall_b);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::AlltoallvKind,      _alltoallv_s);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::AlltoallvintKind,   _alltoallvint);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::BarrierKind,        _alltoallv_b);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::LongAllreduceKind,  _allreduce);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::PrefixKind,         _scan);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::ShortAllreduceKind, _shortallreduce);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::BarrierKind,        _barrier);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::BcastKind,          _broadcast);
#ifdef XLPGAS_PAMI_CAU
	    //shm / hybrid
	    _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::SHMReduceKind,         _shm_shortreduce);
	    _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::SHMBcastKind,          _shm_shortbcast);
	    _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::SHMLargeBcastKind,     _shm_largebcast);
	    _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::CAUReduceKind,         _cau_shortreduce, _dispatch_id, _comm_handle);
	    _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::CAUBcastKind,          _cau_shortbcast, _dispatch_id, _comm_handle);
	    _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::LeadersBcastKind,      _leaders_bcast);
	    _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::ShmCauAllReduceKind,   _hybrid_shortallreduce);
	    _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::ShmHybridBcastKind,    _hybrid_bcast);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::ShmHybridPipelinedBcastKind,    _hybrid_pipelined_bcast);
#endif
          }

      /// \brief The two device (p2p+shmem) pgas constructor with a bool flag to enable it.
      inline PGASRegistration(pami_client_t                        client,
                              pami_context_t                       context,
                              size_t                               client_id,
                              size_t                               context_id,
                              T_Allocator                         &proto_alloc,
                              T_Device_P2P                        &dev_p2p,
                              T_Device_SHMEM                      &dev_shmem,
			      void                                *comm_handle,
                              int                                 *dispatch_id,
                              std::map<unsigned, pami_geometry_t> *geometry_map,
                              bool                                 use_shmem):
        CollRegistration<PAMI::CollRegistration::PGASRegistration<T_Geometry,
                                                                  T_NI,
                                                                  T_Allocator,
                                                                  T_Protocol1,
                                                                  T_Protocol2,
                                                                  T_Device_P2P,
                                                                  T_Device_SHMEM,
	                                                          T_NBCollMgr,
                                                                  T_CSMemoryManager>,
                         T_Geometry> (),
        _client(client),
        _context(context),
        _client_id(client_id),
        _context_id(context_id),
        _mgr(_context_id),
        _reduce_val(0),
	_comm_handle(comm_handle),
        _dispatch_id(dispatch_id),
        _geometry_map(geometry_map),
        _dev_p2p(dev_p2p),
        _dev_shmem(dev_shmem),
        _proto_alloc(proto_alloc),
	_mm(NULL),
        _allgather(NULL),
        _allgatherv(NULL),
        _scatter_s(NULL),
        _scatter_b(NULL),
        _gather_s(NULL),
        _gather_b(NULL),
        _alltoall_s(NULL),
        _alltoall_b(NULL),
        _alltoallv_s(NULL),
        _alltoallv_b(NULL),
        _alltoallvint(NULL),
        _allreduce(NULL),
        _scan(NULL),
        _shortallreduce(NULL),
        _barrier(NULL),
        _broadcast(NULL)
#ifdef XLPGAS_PAMI_CAU
	,
	_shm_shortreduce(NULL),
	_shm_shortbcast(NULL),
	_shm_largebcast(NULL),
	_cau_shortreduce(NULL),
	_cau_shortbcast(NULL),
	_leaders_bcast(NULL),
	_hybrid_shortallreduce(NULL),
	_hybrid_bcast(NULL),
        _hybrid_pipelined_bcast(NULL)
#endif
          {
            pami_result_t       result   = PAMI_SUCCESS;

            if(use_shmem == true)
              {
                SETUPNI_P2P_SHMEM(_allgather);
                SETUPNI_P2P_SHMEM(_allgatherv);
                SETUPNI_P2P_SHMEM(_scatter_s);
                SETUPNI_P2P_SHMEM(_scatter_b);
                SETUPNI_P2P_SHMEM(_gather_s);
                SETUPNI_P2P_SHMEM(_gather_b);
                SETUPNI_P2P_SHMEM(_alltoall_s);
                SETUPNI_P2P_SHMEM(_alltoall_b);
                SETUPNI_P2P_SHMEM(_alltoallv_s);
                SETUPNI_P2P_SHMEM(_alltoallv_b);
		SETUPNI_P2P_SHMEM(_alltoallvint);
                SETUPNI_P2P_SHMEM(_allreduce);
                SETUPNI_P2P_SHMEM(_scan);
                SETUPNI_P2P_SHMEM(_shortallreduce);
                SETUPNI_P2P_SHMEM(_barrier);
                SETUPNI_P2P_SHMEM(_broadcast);
#ifdef XLPGAS_PAMI_CAU
		SETUPNI_P2P_SHMEM(_shm_shortreduce);
		SETUPNI_P2P_SHMEM(_shm_shortbcast);
		SETUPNI_P2P_SHMEM(_shm_largebcast);
		SETUPNI_P2P_SHMEM(_cau_shortreduce);
		SETUPNI_P2P_SHMEM(_cau_shortbcast);
		SETUPNI_P2P_SHMEM(_leaders_bcast);
		SETUPNI_P2P_SHMEM(_hybrid_shortallreduce);
		SETUPNI_P2P_SHMEM(_hybrid_bcast);
		SETUPNI_P2P_SHMEM(_hybrid_pipelined_bcast);
#endif
              }
            else
              {
                SETUPNI_P2P_P2P(_allgather);
                SETUPNI_P2P_P2P(_allgatherv);
                SETUPNI_P2P_P2P(_scatter_s);
                SETUPNI_P2P_P2P(_scatter_b);
                SETUPNI_P2P_P2P(_gather_s);
                SETUPNI_P2P_P2P(_gather_b);
                SETUPNI_P2P_P2P(_alltoall_s);
                SETUPNI_P2P_P2P(_alltoall_b);
                SETUPNI_P2P_P2P(_alltoallv_s);
                SETUPNI_P2P_P2P(_alltoallv_b);
                SETUPNI_P2P_P2P(_alltoallvint);
                SETUPNI_P2P_P2P(_allreduce);
                SETUPNI_P2P_P2P(_scan);
                SETUPNI_P2P_P2P(_shortallreduce);
                SETUPNI_P2P_P2P(_barrier);
                SETUPNI_P2P_P2P(_broadcast);
#ifdef XLPGAS_PAMI_CAU
		// ???
		SETUPNI_P2P_P2P(_shm_shortreduce);
		SETUPNI_P2P_P2P(_shm_shortbcast);
		SETUPNI_P2P_P2P(_shm_largebcast);
		SETUPNI_P2P_P2P(_cau_shortreduce);
		SETUPNI_P2P_P2P(_cau_shortbcast);
		SETUPNI_P2P_P2P(_leaders_bcast);
		SETUPNI_P2P_P2P(_hybrid_shortallreduce);
		SETUPNI_P2P_P2P(_hybrid_bcast);
                SETUPNI_P2P_P2P(_hybrid_pipelined_bcast);
#endif
              }
#ifdef XLPGAS_PAMI_CAU            
            lapi_handle_t hdl = *((lapi_handle_t*)_comm_handle);            
            _mgr.InitializeLapi(hdl,&_mgr); // Initialize for 1 context
#endif
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::AllgatherKind,      _allgather);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::AllgathervKind,     _allgatherv);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::ScatterKind,        _scatter_s);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::BarrierKind,        _scatter_b);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::GatherKind,         _gather_s);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::BarrierKind,        _gather_b);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::AlltoallKind,       _alltoall_s);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::BarrierKind,        _alltoall_b);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::AlltoallvKind,      _alltoallv_s);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::AlltoallvintKind,   _alltoallvint);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::BarrierKind,        _alltoallv_b);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::LongAllreduceKind,  _allreduce);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::PrefixKind,         _scan);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::ShortAllreduceKind, _shortallreduce);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::BarrierKind,        _barrier);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::BcastKind,          _broadcast);
#ifdef XLPGAS_PAMI_CAU
	    //shm / hybrid
	    _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::SHMReduceKind,         _shm_shortreduce);
	    _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::SHMBcastKind,          _shm_shortbcast);
	    _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::SHMLargeBcastKind,     _shm_largebcast);
	    _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::CAUReduceKind,         _cau_shortreduce, _dispatch_id, _comm_handle);
	    _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::CAUBcastKind,          _cau_shortbcast, _dispatch_id, _comm_handle );
	    _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::LeadersBcastKind,      _leaders_bcast);
	    _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::ShmCauAllReduceKind,   _hybrid_shortallreduce);
	    _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::ShmHybridBcastKind,    _hybrid_bcast);
            _mgr.template multisend_reg<xlpgas::base_coll_defs<T_NI, T_Device_P2P> >(xlpgas::ShmHybridPipelinedBcastKind,    _hybrid_pipelined_bcast);
#endif
          }

      static inline void cleanupCallback(pami_context_t ctxt, void *data, pami_result_t res)
          {
            GeometryInfo *gi = (GeometryInfo*) data;
            PAMI::MemoryAllocator<sizeof(Factories),16,16>  *factory_allocator =
              (PAMI::MemoryAllocator<sizeof(Factories),16,16>  *)gi->_factory_allocator;
            PAMI::MemoryAllocator<sizeof(GeometryInfo),16>  *geom_allocator =
              (PAMI::MemoryAllocator<sizeof(GeometryInfo),16>  *)gi->_geom_allocator;

            // First, free the factories
            int sz = gi->_f_list.size();
            for(int i=0; i<sz; i++)
            {
              Factories *f = gi->_f_list.front();
              factory_allocator->returnObject(f);
              gi->_f_list.pop_front();
            }
            // Now free the pgas allocations
            sz = gi->_nbcoll_list.size();
            for(int i=0; i<sz; i++)
            {
              xlpgas::Collective<T_NI> *nbcoll = (xlpgas::Collective<T_NI>*)gi->_nbcoll_list.front();
              nbcoll->~Collective();
              __global.heap_mm->free(nbcoll);
              gi->_nbcoll_list.pop_front();
            }
#ifdef XLPGAS_PAMI_CAU
            if(gi->_device_info != NULL) free (gi->_device_info);
#endif
            gi->~GeometryInfo();
            geom_allocator->returnObject(gi);
          }

        inline pami_result_t analyze_impl(size_t context_id,T_Geometry *geometry, int phase, uint64_t *inout_val=NULL)
        {
          PAMI_assertf(context_id == _context_id, "FATAL:  pgas registration analyze:  want=%zd, got=%zd\n", _context_id, context_id);
	  //in phase 0 we init all but hybrid collectives which
	  //requires a shared memory region available only in phase 1
          if (phase == 0) {
	    _gi = (GeometryInfo*)_geom_allocator.allocateObject();
	    new(_gi) GeometryInfo(&_allocator, &_geom_allocator);

      _team         = (xlpgas::Team*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);
	    _local_team   = (xlpgas::Team*)geometry->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX);
	    _leaders_team = (xlpgas::Team*)geometry->getTopology(PAMI::Geometry::MASTER_TOPOLOGY_INDEX);

	    _nb_barrier         = (xlpgas::Barrier<T_NI>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_team, xlpgas::BarrierKind, geometry->comm(),NULL,_barrier);
#ifdef __bgq__
      // Support a PGAS 'lite' like analyze even when the full PGAS (this reqgistration) was created.  Only do barrier if 'lite'.
      void *val = geometry->getFirstKey(PAMI::Geometry::CKEY_NONCONTIG_DISABLE);
      if(!val)      // If noncontig is not disalbed
        ; /* continue normally with full PGAS/non-contig support*/
      else /* no noncontig support? Finish the barrier analyze and exit with no other collectives */
      {  
        _gi->_nbcoll_list.push_back(_nb_barrier);
        BarrierFactory        *_barrier_reg        = (BarrierFactory*)_allocator.allocateObject();        _gi->_f_list.push_back((Factories*)_barrier_reg);
        new(_barrier_reg)        BarrierFactory(_context,_context_id,mapidtogeometry,&_dev_p2p, _barrier, _nb_barrier, BarrierString);
        geometry->setDefaultBarrier(_barrier_reg,context_id);
        geometry->addCollective(PAMI_XFER_BARRIER,
                                (CCMI::Adaptor::CollectiveProtocolFactory*)_barrier_reg,
                                _context,
                                _context_id);
        return PAMI_SUCCESS;
      }
#endif
	    _nb_broadcast       = (xlpgas::Broadcast<T_NI>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_team, xlpgas::BcastKind, geometry->comm(),NULL,_broadcast);
	    _nb_allgather       = (xlpgas::Allgather<T_NI>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_team, xlpgas::AllgatherKind, geometry->comm(),NULL,_allgather);
	    _nb_allgatherv      = (xlpgas::Allgatherv<T_NI>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_team, xlpgas::AllgathervKind, geometry->comm(),NULL,_allgatherv);
	    _nb_short_allreduce = (xlpgas::Allreduce::Short<T_NI>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_team, xlpgas::ShortAllreduceKind, geometry->comm(),NULL, _shortallreduce);
	    _nb_long_allreduce  = (xlpgas::Allreduce::Long<T_NI>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_team, xlpgas::LongAllreduceKind, geometry->comm(),NULL, _allreduce);
	    _nb_scan            = (xlpgas::PrefixSums<T_NI>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_team, xlpgas::PrefixKind, geometry->comm(),NULL,_scan);
	    _nb_scatter         = (xlpgas::Scatter<T_NI>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_team, xlpgas::ScatterKind, geometry->comm(),NULL,_scatter_s);
	    _nb_gather          = (xlpgas::Gather<T_NI>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_team, xlpgas::GatherKind, geometry->comm(),NULL,_gather_s);
	    _nb_alltoall        = (xlpgas::Alltoall<T_NI>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_team, xlpgas::AlltoallKind, geometry->comm(),NULL,_alltoall_s);
	    _nb_alltoallv       = (xlpgas::Alltoallv<T_NI,size_t>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_team, xlpgas::AlltoallvKind, geometry->comm(),NULL,_alltoallv_s);
	    _nb_alltoallvint       = (xlpgas::Alltoallv<T_NI,int>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_team, xlpgas::AlltoallvintKind, geometry->comm(),NULL,_alltoallvint);
	    _gi->_nbcoll_list.push_back(_nb_barrier);
	    _gi->_nbcoll_list.push_back(_nb_broadcast);
	    _gi->_nbcoll_list.push_back(_nb_allgather);
	    _gi->_nbcoll_list.push_back(_nb_allgatherv);
	    _gi->_nbcoll_list.push_back(_nb_short_allreduce);
	    _gi->_nbcoll_list.push_back(_nb_long_allreduce);
	    _gi->_nbcoll_list.push_back(_nb_scan);
	    _gi->_nbcoll_list.push_back(_nb_scatter);
	    _gi->_nbcoll_list.push_back(_nb_gather);
	    _gi->_nbcoll_list.push_back(_nb_alltoall);
	    _gi->_nbcoll_list.push_back(_nb_alltoallv);
	    _gi->_nbcoll_list.push_back(_nb_alltoallvint);

	    // todo:  free these on geometry destroy, maybe use KVS
	    BarrierFactory        *_barrier_reg        = (BarrierFactory*)_allocator.allocateObject();        _gi->_f_list.push_back((Factories*)_barrier_reg);
	    BroadcastFactory      *_broadcast_reg      = (BroadcastFactory*)_allocator.allocateObject();      _gi->_f_list.push_back((Factories*)_broadcast_reg);
	    AllgatherFactory      *_allgather_reg      = (AllgatherFactory*)_allocator.allocateObject();      _gi->_f_list.push_back((Factories*)_allgather_reg);
	    AllgathervFactory     *_allgatherv_reg     = (AllgathervFactory*)_allocator.allocateObject();     _gi->_f_list.push_back((Factories*)_allgatherv_reg);
	    ScatterFactory        *_scatter_reg        = (ScatterFactory*)_allocator.allocateObject();        _gi->_f_list.push_back((Factories*)_scatter_reg);
	    GatherFactory         *_gather_reg         = (GatherFactory*)_allocator.allocateObject();         _gi->_f_list.push_back((Factories*)_gather_reg);
	    AlltoallFactory       *_alltoall_reg       = (AlltoallFactory*)_allocator.allocateObject();       _gi->_f_list.push_back((Factories*)_alltoall_reg);
	    AlltoallvFactory      *_alltoallv_reg      = (AlltoallvFactory*)_allocator.allocateObject();      _gi->_f_list.push_back((Factories*)_alltoallv_reg);
	    AlltoallvintFactory   *_alltoallvint_reg   = (AlltoallvintFactory*)_allocator.allocateObject();   _gi->_f_list.push_back((Factories*)_alltoallvint_reg);
	    AllreduceFactory      *_allreduce_reg      = (AllreduceFactory*)_allocator.allocateObject();      _gi->_f_list.push_back((Factories*)_allreduce_reg);
	    ScanFactory           *_scan_reg           = (ScanFactory*)_allocator.allocateObject();           _gi->_f_list.push_back((Factories*)_scan_reg);
	    ShortAllreduceFactory *_shortallreduce_reg = (ShortAllreduceFactory*)_allocator.allocateObject(); _gi->_f_list.push_back((Factories*)_shortallreduce_reg);

            assert(_context !=NULL);
	    new(_barrier_reg)        BarrierFactory(_context,_context_id,mapidtogeometry,&_dev_p2p, _barrier, _nb_barrier, BarrierString);
	    new(_broadcast_reg)      BroadcastFactory(_context,_context_id,mapidtogeometry,&_dev_p2p, _broadcast, _nb_broadcast, BroadcastString);
	    new(_allgather_reg)      AllgatherFactory(_context,_context_id,mapidtogeometry,&_dev_p2p, _allgather, _nb_allgather, AllgatherString);
	    new(_allgatherv_reg)     AllgathervFactory(_context,_context_id,mapidtogeometry,&_dev_p2p, _allgatherv, _nb_allgatherv, AllgathervString);
	    new(_scatter_reg)        ScatterFactory(_context,_context_id,mapidtogeometry,&_dev_p2p, _scatter_s, _nb_scatter, ScatterString, _nb_barrier, _scatter_b);
	    new(_gather_reg)         GatherFactory(_context,_context_id,mapidtogeometry,&_dev_p2p, _gather_s, _nb_gather, GatherString, _nb_barrier, _gather_b);
	    new(_alltoall_reg)       AlltoallFactory(_context,_context_id,mapidtogeometry,&_dev_p2p, _alltoall_s, _nb_alltoall, AlltoallString, _nb_barrier, _alltoall_b);
	    new(_alltoallv_reg)      AlltoallvFactory(_context,_context_id,mapidtogeometry,&_dev_p2p, _alltoallv_s, _nb_alltoallv, AlltoallvString, _nb_barrier, _alltoallv_b);
	    new(_alltoallvint_reg)   AlltoallvintFactory(_context,_context_id,mapidtogeometry,&_dev_p2p, _alltoallvint, _nb_alltoallvint, AlltoallvintString, _nb_barrier, _alltoallv_b); //  v_int
	    new(_allreduce_reg)      AllreduceFactory(_context,_context_id,mapidtogeometry,&_dev_p2p, _allreduce, _nb_long_allreduce, AllreduceString);
	    new(_scan_reg)           ScanFactory(_context,_context_id,mapidtogeometry,&_dev_p2p, _scan, _nb_scan, ScanString);
	    new(_shortallreduce_reg) ShortAllreduceFactory(_context,_context_id,mapidtogeometry,&_dev_p2p, _shortallreduce,_nb_short_allreduce, ShortAllreduceString);


            geometry->setDefaultBarrier(_barrier_reg,context_id);
	    geometry->addCollective(PAMI_XFER_BARRIER,
				    (CCMI::Adaptor::CollectiveProtocolFactory*)_barrier_reg,
                                    _context,
				    _context_id);

	    geometry->addCollective(PAMI_XFER_BROADCAST,
				    (CCMI::Adaptor::CollectiveProtocolFactory*)_broadcast_reg,
                                    _context,
				    _context_id);

	    geometry->addCollective(PAMI_XFER_ALLGATHER,
				    (CCMI::Adaptor::CollectiveProtocolFactory*)_allgather_reg,
                                    _context,
				    _context_id);

	    geometry->addCollective(PAMI_XFER_ALLGATHERV,
				    (CCMI::Adaptor::CollectiveProtocolFactory*)_allgatherv_reg,
                                    _context,
				    _context_id);
#if 0
	    // Scatter broken now in pgas rt
	    geometry->addCollective(PAMI_XFER_SCATTER,
				    (CCMI::Adaptor::CollectiveProtocolFactory*)_scatter_reg,
                                    _context,
				    _context_id);
	    // Gather broken now in pgas rt
	    geometry->addCollective(PAMI_XFER_GATHER,
				    (CCMI::Adaptor::CollectiveProtocolFactory*)_gather_reg,
                                    _context,
				    _context_id);
#endif
	    geometry->addCollective(PAMI_XFER_ALLTOALL,
				    (CCMI::Adaptor::CollectiveProtocolFactory*)_alltoall_reg,
                                    _context,
				    _context_id);

	    geometry->addCollective(PAMI_XFER_ALLTOALLV,
				    (CCMI::Adaptor::CollectiveProtocolFactory*)_alltoallv_reg,
                                    _context,
				    _context_id);

	    geometry->addCollective(PAMI_XFER_ALLTOALLV_INT,
				    (CCMI::Adaptor::CollectiveProtocolFactory*)_alltoallvint_reg,
                                    _context,
				    _context_id);

	    geometry->addCollective(PAMI_XFER_ALLREDUCE,
				    (CCMI::Adaptor::CollectiveProtocolFactory*)_allreduce_reg,
                                    _context,
				    _context_id);

	    geometry->addCollective(PAMI_XFER_SCAN,
				    (CCMI::Adaptor::CollectiveProtocolFactory*)_scan_reg,
                                    _context,
				    _context_id);

	    geometry->addCollectiveCheck(PAMI_XFER_ALLREDUCE,
					 (CCMI::Adaptor::CollectiveProtocolFactory*)_shortallreduce_reg,
                                         _context,
					 _context_id);

	    geometry->setCleanupCallback(cleanupCallback, _gi);

	  }//if phase 0
#ifdef XLPGAS_PAMI_CAU
	  //                                hybrid / shm / cau
	  if(phase==1){

            //for right now the hybrid collectives are restricted to
            //the global geometry only; the algorithms should work on
            //subgeometries as well but it needs to be tested first;
	    // the shared memory region is initialized only for complete geometries in CAUCollRegistration; we use the data offset as a flag;
	    
	    //set the shared memory allocator; it can be null if not avail;
            _mm = (T_CSMemoryManager*)(geometry->getKey(_context_id,
                                                        Geometry::CKEY_GEOMETRYSHMEM));

            //get the offset of the shared memory buffer to be used;
            //this is the result of a reduction and it is found inside
            //inout_val arg
            PAMI::Topology *local_master_topo = (PAMI::Topology *) (geometry->getTopology(PAMI::Geometry::MASTER_TOPOLOGY_INDEX));
            uint num_master_tasks  = local_master_topo->size();
            uint master_rank   = ((PAMI::Topology *)geometry->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX))->index2Endpoint(0);
            uint master_index  = local_master_topo->rank2Index(master_rank);

            //marking in the geometry the shared memory region to be used by pgas hybrid
            uint64_t data_offset = inout_val[master_index+1+2*num_master_tasks];
	    uint64_t invalid_offset;
	    memset(&invalid_offset, 0xFF, sizeof(uint64_t));
	    //if data offset not valid that we disable pgas hybrid colllectives
	    if(data_offset  ==  invalid_offset){
	      return PAMI_SUCCESS;
	    }

            lapi_handle_t l_lapi_handle = *((lapi_handle_t*)_comm_handle);

            //set the cau group info
            bool *p_use_cau = ((bool*)geometry->getKey(_context_id,
                                                       Geometry::CKEY_GEOMETRYUSECAU));
            bool use_cau;
            PAMI::Device::CAUGeometryInfo *cau_gi;
            if(p_use_cau != NULL){
              use_cau = *p_use_cau;
              cau_gi = (PAMI::Device::CAUGeometryInfo *)(geometry->getKey(_context_id,
                                                                          Geometry::CKEY_MCAST_CLASSROUTEID));
              if(cau_gi == NULL) _cau_group = -1;
              else _cau_group = cau_gi->_cau_id;
            }
            else {
              use_cau = false;
              cau_gi = NULL;
            }

            // set the shared mem buf to be properly deallocated;
            // this is done by only one thred in teh local topology
            PAMI::Topology *local_topo    = (PAMI::Topology *)geometry->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX);
            uint local_index              =  local_topo->rank2Index(__global.mapping.task());
            if(local_index == 0){
              _gi->_mm = _mm;
              _gi->_pgas_shmem_offset = data_offset;
            }

            //alocate device info type using placement new
            typedef xlpgas::cau_device_info<T_NI> device_info_type;
            device_info_type *device_info = (device_info_type *)__global.heap_mm->malloc (sizeof(device_info_type));
            assert (device_info != NULL);
            memset (device_info, 0, sizeof(device_info_type));
            new (device_info) device_info_type(_dispatch_id,
                                               _cau_group,
                                               l_lapi_handle,
                                               geometry,
                                               _mm,
                                               data_offset
                                               );
            _gi->_device_info = device_info; //to be freed

	    if(device_info->shm_buffers().valid() && (_local_team->size() == _team->size() || (use_cau && _local_team->size() < _team->size())) )
	    {
	      HybridAllreduceFactory   *_hybrid_shortallreduce_reg;
	      HybridBroadcastFactory   *_hybrid_broadcast_reg;
	      _nb_shm_short_reduce= (xlpgas::SHMReduce<T_NI>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_local_team, xlpgas::SHMReduceKind, geometry->comm(), (void*)device_info,_shm_shortreduce);
	      _nb_shm_short_bcast = (xlpgas::SHMBcast<T_NI>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_local_team, xlpgas::SHMBcastKind, geometry->comm(), (void*)device_info,_shm_shortbcast);
	      _nb_shm_large_bcast = (xlpgas::SHMLargeBcast<T_NI>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_local_team, xlpgas::SHMLargeBcastKind, geometry->comm(), (void*)device_info,_shm_largebcast);
	      _nb_cau_short_reduce= (xlpgas::CAUReduce<T_NI>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_leaders_team, xlpgas::CAUReduceKind, geometry->comm(), (void*)device_info,_cau_shortreduce);
	      _nb_cau_short_bcast = (xlpgas::CAUBcast<T_NI>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_leaders_team, xlpgas::CAUBcastKind, geometry->comm(),(void*)device_info,_cau_shortbcast);
	      _nb_leaders_bcast   = (xlpgas::Broadcast<T_NI>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_leaders_team, xlpgas::LeadersBcastKind, geometry->comm(), (void*)device_info,_leaders_bcast);

	      _nb_hybrid_short_allreduce= (xlpgas::ShmCauAllReduce<T_NI,T_Device_P2P>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_team, xlpgas::ShmCauAllReduceKind, geometry->comm(),(void*)device_info,_hybrid_shortallreduce);
	      //set internal collectives used by the hybrid algo
	      _nb_hybrid_short_allreduce->set_internal_coll(_nb_shm_short_reduce,
							    _nb_shm_short_bcast,
							    _nb_cau_short_reduce,
							    _nb_cau_short_bcast);

	      _nb_hybrid_bcast = (xlpgas::ShmHybridBcast<T_NI,T_Device_P2P>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_team, xlpgas::ShmHybridBcastKind, geometry->comm(), (void*)device_info,_hybrid_bcast);
	      //next we set the internal collectives used by the hybrid algo
	      _nb_hybrid_bcast->set_internal_coll(_nb_leaders_bcast,  _nb_shm_large_bcast);

	      _nb_hybrid_pipelined_bcast = (xlpgas::ShmHybridPipelinedBcast<T_NI,T_Device_P2P>*)_mgr.template allocate<xlpgas::base_coll_defs<T_NI, T_Device_P2P> > (_team, xlpgas::ShmHybridPipelinedBcastKind, geometry->comm(), (void*)device_info,_hybrid_pipelined_bcast);
	      // set internal collective to be used; 
	      _nb_hybrid_pipelined_bcast->set_internal_coll(_nb_hybrid_bcast);

	      _gi->_nbcoll_list.push_back(_nb_shm_short_reduce);
	      _gi->_nbcoll_list.push_back(_nb_shm_short_bcast);
	      _gi->_nbcoll_list.push_back(_nb_cau_short_reduce);
	      _gi->_nbcoll_list.push_back(_nb_cau_short_bcast);
	      _gi->_nbcoll_list.push_back(_nb_hybrid_short_allreduce);
              _gi->_nbcoll_list.push_back(_nb_hybrid_bcast);
              _gi->_nbcoll_list.push_back(_nb_hybrid_pipelined_bcast);

	      //create allreduce factory and add to the list of collectives
	      _hybrid_shortallreduce_reg    = (HybridAllreduceFactory*)_allocator.allocateObject(); _gi->_f_list.push_back((Factories*)_hybrid_shortallreduce_reg);
	      new(_hybrid_shortallreduce_reg) HybridAllreduceFactory(_context,_context_id,mapidtogeometry,&_dev_p2p, _hybrid_shortallreduce,_nb_hybrid_short_allreduce, HybridAllreduceString);

	      geometry->addCollectiveCheck(PAMI_XFER_ALLREDUCE,
					   (CCMI::Adaptor::CollectiveProtocolFactory*)_hybrid_shortallreduce_reg,
                                           _context,
					   _context_id);

	      //add bcast only if there is shared memory left outside of the control structures
	      if(device_info->shm_buffers()._bcast_buf_sz > 0)
		{
		_hybrid_broadcast_reg    = (HybridBroadcastFactory*)_allocator.allocateObject(); _gi->_f_list.push_back((Factories*)_hybrid_broadcast_reg);
		new(_hybrid_broadcast_reg) HybridBroadcastFactory(_context,_context_id,mapidtogeometry,&_dev_p2p, _hybrid_pipelined_bcast,_nb_hybrid_pipelined_bcast, HybridBroadcastString);
		geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
					(CCMI::Adaptor::CollectiveProtocolFactory*)_hybrid_broadcast_reg,
					_context,
					_context_id);
	      }

	      // set the shared mem buf to be properly deallocated;
              // this is done by only one thred in teh local topology
              PAMI::Topology *local_topo    = (PAMI::Topology *)geometry->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX);
              uint local_index              =  local_topo->rank2Index(__global.mapping.task());
              if(local_index == 0){
                _gi->_mm = _mm;
                _gi->_pgas_shmem_offset = data_offset;
              }
	    }//if hybrid can be used
	  }//if phase 1
#endif
          return PAMI_SUCCESS;
        }

      inline pami_result_t register_local_impl(size_t context_id,T_Geometry *geometry, uint64_t *out, int &n)
          {
	    n = 0;
            //*out = _reduce_val;
            return analyze(context_id, geometry, 0);
          }

      inline pami_result_t receive_global_impl(size_t context_id,T_Geometry *geometry, uint64_t *in, int n)
          {
            return PAMI_SUCCESS;
          }


        inline pami_result_t setGenericDevice(PAMI::Device::Generic::Device *g)
          {
            _mgr.setGenericDevice(g);
            return PAMI_SUCCESS;
          }

    public:
      pami_client_t               _client;
      pami_context_t              _context;
      size_t                      _client_id;
      size_t                      _context_id;
      T_NBCollMgr                 _mgr;
      uint64_t                    _reduce_val;
      void                       *_comm_handle;
      int                        *_dispatch_id;
      xlpgas::Team*               _team;
      xlpgas::Team*               _local_team;
      xlpgas::Team*               _leaders_team;
#ifdef XLPGAS_PAMI_CAU
      // Collective shared memory manager
      int                         _cau_group;
#endif
      // Map of geometry id's to geometry for this client
      std::map<unsigned, pami_geometry_t> *_geometry_map;

      // Native Interface
      T_Device_P2P               &_dev_p2p;
      T_Device_SHMEM             &_dev_shmem;
      T_Allocator                &_proto_alloc;
      GeometryInfo               *_gi;
      T_CSMemoryManager          *_mm;

      T_NI                   *_allgather;
      T_NI                   *_allgatherv;
      T_NI                   *_scatter_s;
      T_NI                   *_scatter_b;
      T_NI                   *_gather_s;
      T_NI                   *_gather_b;
      T_NI                   *_alltoall_s;
      T_NI                   *_alltoall_b;
      T_NI                   *_alltoallv_s;
      T_NI                   *_alltoallv_b;
      T_NI                   *_alltoallvint;
      T_NI                   *_allreduce;
      T_NI                   *_scan;
      T_NI                   *_shortallreduce;
      T_NI                   *_barrier;
      T_NI                   *_broadcast;
#ifdef XLPGAS_PAMI_CAU
      // for hybrid collectives;
      T_NI                   *_shm_shortreduce;
      T_NI                   *_shm_shortbcast;
      T_NI                   *_shm_largebcast;
      T_NI                   *_cau_shortreduce;
      T_NI                   *_cau_shortbcast;
      T_NI                   *_leaders_bcast;
      T_NI                   *_hybrid_shortallreduce;
      T_NI                   *_hybrid_bcast;
      T_NI                   *_hybrid_pipelined_bcast;
#endif
      PAMI::MemoryAllocator<sizeof(Factories),16,16>  _allocator;
      PAMI::MemoryAllocator<sizeof(GeometryInfo),16>  _geom_allocator;
      xlpgas::Barrier<T_NI>                  *_nb_barrier;
      xlpgas::Broadcast<T_NI>                *_nb_broadcast;
      xlpgas::Allgather<T_NI>                *_nb_allgather;
      xlpgas::Allgatherv<T_NI>               *_nb_allgatherv;
      xlpgas::Allreduce::Short<T_NI>         *_nb_short_allreduce;
      xlpgas::Allreduce::Long<T_NI>          *_nb_long_allreduce;
      xlpgas::PrefixSums<T_NI>               *_nb_scan;
      xlpgas::Scatter<T_NI>                  *_nb_scatter;
      xlpgas::Gather<T_NI>                   *_nb_gather;
      xlpgas::Alltoall<T_NI>                 *_nb_alltoall;
      xlpgas::Alltoallv<T_NI,size_t>         *_nb_alltoallv;
      xlpgas::Alltoallv<T_NI,int>            *_nb_alltoallvint;
#ifdef XLPGAS_PAMI_CAU
      // for hybrid collectives;
      xlpgas::SHMReduce<T_NI>                *_nb_shm_short_reduce;
      xlpgas::SHMBcast<T_NI>                 *_nb_shm_short_bcast;
      xlpgas::SHMLargeBcast<T_NI>            *_nb_shm_large_bcast;
      xlpgas::CAUReduce<T_NI>                *_nb_cau_short_reduce;
      xlpgas::CAUBcast<T_NI>                 *_nb_cau_short_bcast;
      xlpgas::Broadcast<T_NI>                *_nb_leaders_bcast;
      xlpgas::ShmCauAllReduce<T_NI, T_Device_P2P> *_nb_hybrid_short_allreduce;
      xlpgas::ShmHybridBcast<T_NI, T_Device_P2P>  *_nb_hybrid_bcast;
      xlpgas::ShmHybridPipelinedBcast<T_NI, T_Device_P2P>  *_nb_hybrid_pipelined_bcast;
#endif
    };
  };
};
#endif
