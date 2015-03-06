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
 * \file algorithms/geometry/FCACollRegistration.h
 * \brief ???
 */

#ifndef __algorithms_geometry_FCACollRegistration_h__
#define __algorithms_geometry_FCACollRegistration_h__

#include "sys/pami.h"

#ifdef PAMI_USE_FCA

#include "lapi_itrace.h"
#include "algorithms/geometry/FCAWrapper.h"
#include "algorithms/interfaces/GeometryInterface.h"
namespace PAMI{namespace CollRegistration{namespace FCA{
namespace G = PAMI::Geometry;
template <class T_Geometry>
class FCARegistration : public CollRegistration <FCARegistration<T_Geometry>, T_Geometry >
{
public:
  typedef PAMI::MemoryAllocator<sizeof(GeometryInfo),16> GIAllocator;

  // Composite definitions
  typedef FCAReduceExec<T_Geometry>        ReduceExec;
  typedef FCAAllreduceExec<T_Geometry>     AllReduceExec;
  typedef FCABroadcastExec<T_Geometry>     BroadcastExec;
  typedef FCAAllgatherExec<T_Geometry>     AllgatherExec;
  typedef FCAAllgathervIntExec<T_Geometry> AllgathervIntExec;
  typedef FCABarrierExec<T_Geometry>       BarrierExec;

  // Factory definitions
  typedef FCAFactory<ReduceExec>        ReduceFactory;
  typedef FCAFactory<AllReduceExec>     AllReduceFactory;
  typedef FCAFactory<BroadcastExec>     BroadcastFactory;
  typedef FCAFactory<AllgatherExec>     AllgatherFactory;
  typedef FCAFactory<AllgathervIntExec> AllgathervIntFactory;
  typedef FCAFactory<BarrierExec>       BarrierFactory;

  typedef PAMI::Topology                Topology;
  typedef void                          (*FCA_progress_func)(void *);

  inline FCARegistration(pami_client_t                        client,
                         pami_context_t                       context,
                         size_t                               context_id,
                         size_t                               client_id,
                         size_t                               num_contexts,
                         FCA_progress_func                    func):
    CollRegistration <FCARegistration<T_Geometry>, T_Geometry > (),
    _client(client),
    _context(context),
    _context_id(context_id),
    _num_contexts(num_contexts),
    _client_id(client_id),
    _enabled(false),
    _reduce_f(context,context_id,mapidtogeometry),
    _allreduce_f(context,context_id,mapidtogeometry),
    _broadcast_f(context,context_id,mapidtogeometry),
    _allgather_f(context,context_id,mapidtogeometry),
    _allgatherv_int_f(context,context_id,mapidtogeometry),
    _barrier_f(context,context_id,mapidtogeometry)
  {
    if ( strcasecmp(_Lapi_env.collective_offload, "yes") == 0 || 
         strcasecmp(_Lapi_env.collective_offload, "fca") == 0 ) { 
      if (num_contexts > 1) {
        if (_Lapi_env.MP_infolevel >= 2) {
          fprintf(stderr, "ATTENTION: FCA is not supported with multi-endpoint.\n");  
        }
        return;
      }  
      if (((Context*)context)->is_udp) {
        if (_Lapi_env.MP_infolevel >= 2) {
          fprintf(stderr, "ATTENTION: FCA is disabled for IP jobs.\n");  
        }
        return;
      }  
    } else {
      return;  
    }
    void * rc = FCA_Dlopen(0);
    if(!rc) return;
    else
      {
        // print the FCA Banner and check FCA version
        ITRC(IT_FCA, "FCA version %lu [%s]\n", 
                FCA_Get_version(),
                FCA_Get_version_string());
        // Fill in FCA Init Spec
        // use default config for now
        // provide service variable override
        char *fca_spec_file = getenv("MP_S_FCA_SPEC_FILE");
        _fca_init_spec = FCA_Parse_spec_file(fca_spec_file);
        if(!_fca_init_spec)
        {
          ITRC(IT_FCA, "FCA_Parse_spec_file failed\n");
          return;
        }
        _fca_init_spec->job_id        = _Lapi_env.MP_partition;
        _fca_init_spec->rank_id       = _Lapi_env.MP_child;
        _fca_init_spec->progress.func = func;
        _fca_init_spec->progress.arg  = (void*)context;


        int ret = FCA_Init(_fca_init_spec, &_fca_context);
        if (ret < 0)
          {
            ITRC(IT_FCA, "FCA_Init failed with rc %d [%s]\n",
                    ret, FCA_Strerror(ret));
            return;
          }

        ITRC(IT_FCA, "FCA_Init succeeded with fca context at %p\n",
                _fca_context);

        _fca_rank_info = FCA_Get_rank_info(_fca_context, &_fca_rank_info_sz);
        if (_fca_rank_info == NULL)
          {
            fprintf(stderr, "FCA_Get_rank_info failed with ptr %p\n", _fca_rank_info);
            ITRC(IT_FCA, "FCA_Get_rank_info failed with ptr %p\n", _fca_rank_info);
            FCA_Cleanup(_fca_context);  
            exit(0);
          }
        ITRC(IT_FCA, "FCA_Get_rank_info succeeded with rank_info at %p\n",
                _fca_rank_info);
                
        _enabled=true;
        if (_Lapi_env.MP_infolevel >= 2) {
          fprintf(stderr, "FCA (Ver. %s) OFFLOAD is enabled and ready.\n", 
                  FCA_Get_version_string());  
        }

        _my_endpoint = PAMI_ENDPOINT_INIT(_client_id,
                                          __global.mapping.task(),
                                          _context_id);
      }
  }

  ~FCARegistration()
  {
    if(_enabled) {
      FCA_Free_rank_info(_fca_rank_info);  
      FCA_Cleanup(_fca_context);  
    }
  }

  // Return the number of integers to reduce
  inline uint64_t analyze_count_impl(size_t      context_id,
                              T_Geometry *geometry)
  {
    if(!_enabled)
      {
        return 0;
      }
    Topology *master_topo = (Topology *) (geometry->getTopology(G::MASTER_TOPOLOGY_INDEX));
    size_t    bufsz       = (master_topo->size()*_fca_rank_info_sz)+sizeof(fca_comm_desc_t)+sizeof(uint64_t);
    size_t    extrapad    = (bufsz%sizeof(uint64_t))?1:0;
    return (bufsz/sizeof(uint64_t))+extrapad;
  }
  inline pami_result_t analyze_impl(size_t      context_id,
                                    T_Geometry *geometry,
                                    uint64_t   *inout_val,
                                    int        *inout_nelem,
                                    int         phase)
    {
      PAMI_assert(context_id == _context_id);
      if(!_enabled) return PAMI_SUCCESS;
      int rc;

      // TODO:  Conditionally insert these algorithms into the list
      // TODO:  Implement metadata and strings
      Topology        *topo             = (Topology *) (geometry->getTopology(G::DEFAULT_TOPOLOGY_INDEX));

      if(topo->size() == 1)
        return PAMI_SUCCESS;

      uint             numtasks         = topo->size();

      Topology        *local_topo       = (Topology *) (geometry->getTopology(G::LOCAL_TOPOLOGY_INDEX));
      if(numtasks == local_topo->size())
        return PAMI_SUCCESS;

      //Topology        *my_master_topo   = (Topology *) (geometry->getTopology(G::MASTER_TOPOLOGY_INDEX));
      Topology        *master_topo      = (Topology *) (geometry->getTopology(G::MASTER_TOPOLOGY_INDEX));

      uint             num_local_tasks  = local_topo->size();
      pami_endpoint_t  master_ep        = local_topo->index2Endpoint(0);
      pami_endpoint_t  root_ep          = master_topo->index2Endpoint(0);
      uint             master_index     = master_topo->endpoint2Index(master_ep);
      uint             num_master_tasks = master_topo->size();
      bool             amLeader         = master_topo->isEndpointMember(_my_endpoint);
      bool             amRoot           = (root_ep == _my_endpoint);

      ITRC(IT_FCA, "PHASE %d> my ep %d master ep %d root ep %d "
           "master index %d number of tasks %d number of locals %d "
           "number of masters %d AmLeader %d AmRoot %d\n",
           phase,
           _my_endpoint, master_ep, root_ep,
           master_index, numtasks, num_local_tasks,
           num_master_tasks, amLeader, amRoot);

      switch (phase)
      {
          case 0:
          {
            ITRC(IT_FCA, "FCACollReg Analyze 0 <<<<<-----\n");
            // Allocate Per Geometry Information
            GeometryInfo *gi = (GeometryInfo *)_geom_allocator.allocateObject();
            new(gi) GeometryInfo(this);
            geometry->setKey(_context_id, G::CKEY_FCAGEOMETRYINFO, gi);
            geometry->setCleanupCallback(cleanupCallback, gi);
            ITRC(IT_FCA, "setCleanupCallback with func at %p and data %p\n",
                    cleanupCallback, gi);
            // Set up the first phase reduction buffer
            uint64_t *uptr      = (uint64_t*)inout_val;
            uint64_t  count     = analyze_count_impl(context_id, geometry);
            // clear the 8 bytes used to indicate FCA errors encountered
            uptr[0] = 0x0ULL;
            for(int i=1; i<(int)count; i++)
              uptr[i] = 0xFFFFFFFFFFFFFFFFULL;
            if(amLeader)
            {
              // master_index is my index in the master topology
              // for a leader (leader is master)
              char *ptr       = ((char*)inout_val)+sizeof(uint64_t)+(master_index*_fca_rank_info_sz);
              memcpy(ptr, _fca_rank_info, _fca_rank_info_sz);
            }
            return PAMI_SUCCESS;
          }
          break;
          case 1:
          {
            char *ptr       = ((char*)inout_val)+sizeof(uint64_t)+(master_topo->size()*_fca_rank_info_sz);
            memset(ptr, 0xFF,sizeof(fca_comm_desc_t));
            ITRC(IT_FCA, "PHASE 1:  ptr is at %p\n", ptr);
            if(amRoot)
            {
              GeometryInfo   *gi = (GeometryInfo*)geometry->getKey(_context_id,
                                                                   G::CKEY_FCAGEOMETRYINFO);
              fca_comm_new_spec_t cs;
              cs.rank_info     = (void*)(&(inout_val[1]));
              cs.rank_count    = num_master_tasks;
              // set is_comm_world to 1 for world geometry
              cs.is_comm_world = (geometry->comm() == 0)?1:0;
              fca_comm_desc_t *comm_desc = (fca_comm_desc_t*) ptr;
              ITRC(IT_FCA, "PHASE 1 root: (%d:%s) before FCA_Comm_new rank info %p "
                   "count %d fca context %p "
                   "new spec at %p comm desc at %p\n",
                   geometry->comm(),
                   ((geometry->comm())? "SUB-GEO":"WORLD"),
                   cs.rank_info,
                   cs.rank_count, _fca_context,
                   &cs, comm_desc);
              rc = FCA_Comm_new(_fca_context,
                                &cs,
                                comm_desc);
              ITRC(IT_FCA, "PHASE 1 root: after FCA_Comm_new desc %p rc %d\n",
                   comm_desc, rc);
              if (rc < 0)
              {
                ITRC(IT_FCA, "FCA_Comm_new failed with rc %d [%s]\n",
                     rc, FCA_Strerror(rc));
                // to indicate the failure of FCA_Comm_new
                ((char*)(inout_val))[0] = 1; 
                return PAMI_ERROR;
              }

              ITRC(IT_FCA, "FCA_Comm_new succeeded.\n");
              gi->_amRoot = true;
            }
          }
          return PAMI_SUCCESS;
          break;
          case 2:
          {
            if (((char*)(inout_val))[0] == 0) {
              fca_comm_init_spec is;
              char            *ptr       = ((char*)inout_val)+sizeof(uint64_t)+(num_master_tasks*_fca_rank_info_sz);
              fca_comm_desc_t *d         = (fca_comm_desc_t*)ptr;
              GeometryInfo    *gi        = (GeometryInfo*)geometry->getKey(_context_id,
                                                                           G::CKEY_FCAGEOMETRYINFO);
              gi->_fca_comm_desc         = *d;
              is.desc                    = *d;
              is.rank                    = topo->endpoint2Index(_my_endpoint);
              is.size                    = numtasks;
              is.proc_idx                = local_topo->endpoint2Index(_my_endpoint);
              is.num_procs               = num_local_tasks;
 
              if(num_local_tasks > 1)/* SSS: We need to update local_topo idx if ordering is descending */
                if(topo->endpoint2Index(local_topo->index2Endpoint(0)) > topo->endpoint2Index(local_topo->index2Endpoint(1)))
                  is.proc_idx = num_local_tasks - local_topo->endpoint2Index(_my_endpoint) - 1;
 
              rc = FCA_Comm_init(_fca_context,
                                 &is,
                                 &gi->_fca_comm);
              if (rc < 0) {
                ITRC(IT_FCA, "FCA_Comm_init failed with rc %d [%s]\n", 
                     rc, FCA_Strerror(rc));
                return PAMI_ERROR;
              }

              ITRC(IT_FCA, "FCA_Comm_init succeeded.\n");
           
              gi->_ep                    = _my_endpoint;  
              geometry->addCollectiveCheck(PAMI_XFER_REDUCE,
                                           &_reduce_f,
                                           _context,
                                           _context_id);
              geometry->addCollectiveCheck(PAMI_XFER_ALLREDUCE,
                                           &_allreduce_f,
                                           _context,
                                           _context_id);
              geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,
                                           &_broadcast_f,
                                           _context,
                                           _context_id);
              geometry->addCollectiveCheck(PAMI_XFER_ALLGATHER,
                                           &_allgather_f,
                                           _context,
                                           _context_id);
              geometry->addCollectiveCheck(PAMI_XFER_ALLGATHERV_INT,
                                           &_allgatherv_int_f,
                                           _context,
                                           _context_id);
              geometry->addCollective(PAMI_XFER_BARRIER,
                                      &_barrier_f,
                                      _context,
                                      _context_id);
              return PAMI_SUCCESS;
            } else
              return PAMI_ERROR;  
        }
        default:
        return PAMI_SUCCESS;
      }

  }
  inline fca_t *getFCAContext()
    {
      return _fca_context;
    }

  inline void freeGeomInfo(GeometryInfo *gi)
  {
    if (gi->_fca_comm != NULL)
    {
      FCA_Comm_destroy(gi->_fca_comm);
      ITRC(IT_FCA, "FCA_Comm_destroy is invoked.\n");
      if(gi->_amRoot == true)
      {
        FCARegistration *f = (FCARegistration *)gi->_registration;
        int ret = FCA_Comm_end(f->getFCAContext(),
                               gi->_fca_comm_desc.comm_id);
        if (ret < 0)
        {
          ITRC(IT_FCA, "FCA_Comm_end failed with rc=%d [%s]\n",
               ret, FCA_Strerror(ret));
        }
        ITRC(IT_FCA, "FCA_Comm_end succeeded.\n");
      }
    }
    else
    {
      ITRC(IT_FCA, "FCA Communicator is not created. No need to destroy.\n");
    }
    _geom_allocator.returnObject(gi);
  }
  static inline void cleanupCallback(pami_context_t ctxt,
                                     void          *data,
                                     pami_result_t  res)
  {
    GeometryInfo    *gi = (GeometryInfo*) data;
    FCARegistration *f  = (FCARegistration *)gi->_registration;
    ITRC(IT_FCA, "FCACollRegistration cleanupCallback invoked with gi %p f %p\n", 
            gi, f);
    f->freeGeomInfo(gi);
  }
private:
  // Client, Context, and Utility variables
  pami_client_t        _client;
  pami_context_t       _context;
  size_t               _context_id;
  size_t               _num_contexts;
  size_t               _client_id;
  pami_endpoint_t      _my_endpoint;
  bool                 _enabled;
  GIAllocator          _geom_allocator;
  fca_init_spec       *_fca_init_spec;
  fca_t               *_fca_context;
  void                *_fca_rank_info;
  int                  _fca_rank_info_sz;
  ReduceFactory        _reduce_f;
  AllReduceFactory     _allreduce_f;
  BroadcastFactory     _broadcast_f;
  AllgatherFactory     _allgather_f;
  AllgathervIntFactory _allgatherv_int_f;
  BarrierFactory       _barrier_f;
}; // FCARegistration
}; // FCA
}; // CollRegistration
}; // PAMI

#else //PAMI_USE_FCA
typedef void                          (*FCA_progress_func)(void *);

namespace PAMI{namespace CollRegistration{namespace FCA{
template <class T_Geometry>
class FCARegistration : public CollRegistration <FCARegistration<T_Geometry>, T_Geometry >
{
public:
  inline FCARegistration(pami_client_t                        client,
                         pami_context_t                       context,
                         size_t                               context_id,
                         size_t                               client_id,
                         size_t                               num_contexts,
                         FCA_progress_func                    func):
    CollRegistration <FCARegistration<T_Geometry>, T_Geometry > ()
  {
    return;
  }

  inline uint64_t analyze_count_impl(size_t         context_id,
                                     T_Geometry    *geometry)
  {
    return PAMI_SUCCESS;
  }
  inline pami_result_t analyze_impl(size_t         context_id,
                                    T_Geometry    *geometry,
                                    uint64_t      *inout_val,
                                    int           *inout_nelem,
                                    int            phase)
  {
    return PAMI_SUCCESS;
  }
}; // FCARegistration
}; // FCA
}; // CollRegistration
}; // PAMI

#endif //PAMI_USE_FCA
#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
