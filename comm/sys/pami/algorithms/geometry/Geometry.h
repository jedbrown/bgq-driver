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
 * \file algorithms/geometry/Geometry.h
 * \brief ???
 */

#ifndef __algorithms_geometry_Geometry_h__
#define __algorithms_geometry_Geometry_h__

#include "Global.h"
#include "Topology.h"
#include "Mapping.h"
#include "algorithms/interfaces/GeometryInterface.h"
#include "algorithms/geometry/Algorithm.h"
#include "util/common.h"
#include <map>
#include <list>

#include "algorithms/geometry/UnexpBarrierQueueElement.h"
#include "components/atomic/native/NativeCounter.h"
#include "components/memory/MemoryAllocator.h"
#include "util/trace.h"

#undef  DO_TRACE_ENTEREXIT
#define DO_TRACE_ENTEREXIT 0
#undef  DO_TRACE_DEBUG
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  namespace Geometry
  {
    static inline void*** allocKVS(size_t nrows, size_t ncols)
    {
      size_t             i;
      pami_result_t    rc;
      void          ***array;
      rc = __global.heap_mm->memalign((void**)&array,0,nrows*sizeof(void**));
      PAMI_assertf(array && (rc == PAMI_SUCCESS), "Unable to allocate KVS\n");
      rc = __global.heap_mm->memalign((void**)&array[0],0,nrows*ncols*sizeof(void*));
      PAMI_assertf(array[0] && (rc == PAMI_SUCCESS), "Unable to allocate KVS row\n");
      memset(array[0], 0, nrows*ncols*sizeof(void*));
      for(i = 0; i < nrows; i++)
        array[i]   = array[0] + i * ncols;
      return array;
    }
    static inline void freeKVS(void*** array, size_t nrows)
    {
      return;
      for(size_t i=0; i<nrows; i++)
        __global.heap_mm->free(array[i]);
      __global.heap_mm->free(array);
    }
    static void resetFactoryCache (pami_context_t   ctxt,
                                   void           * factory,
                                   pami_result_t    result)
    {
      (void)ctxt;(void)result;
      CCMI::Adaptor::CollectiveProtocolFactory *cf =
      (CCMI::Adaptor::CollectiveProtocolFactory *) factory;
      cf->clearCache();
    }

    typedef bool (*CheckpointCb)(void *data);
    class CheckpointFunction
    {
    public:
      CheckpointFunction(CheckpointCb ckpt,
                         CheckpointCb resume,
                         CheckpointCb restart,
                         void *data):
      _checkpoint_fn(ckpt),
      _resume_fn(resume),
      _restart_fn(restart),
      _cookie(data)
      {
      };
      CheckpointCb  _checkpoint_fn;
      CheckpointCb  _resume_fn;
      CheckpointCb  _restart_fn;
      void         *_cookie;
    };

    class Common :
    public Geometry<Common>
    {
    public:
      typedef Algorithm<Geometry<Common> >        AlgorithmT;
      typedef std::map<size_t, AlgorithmT>        ContextMap;
      typedef std::map<uint32_t, ContextMap>      HashMap;
      typedef std::map<uint32_t, HashMap>         AlgoMap;
      typedef std::list<ContextMap*>              AlgoList;
      typedef std::map<unsigned, pami_geometry_t> GeometryMap;
      typedef PAMI::Counter::Native               GeomCompCtr;
      typedef std::list<pami_event_function>      CleanupFunctions;
      typedef std::list<void*>                    CleanupDatas;
      typedef std::list<CheckpointFunction>       CheckpointFunctions;
      typedef std::map <size_t, DispatchInfo>     DispatchMap;

      inline ~Common()
      {
        TRACE_FN_ENTER();
        freeAllocations_impl();
        TRACE_FN_EXIT();
      }

      inline Common(pami_client_t  client,
                    Common        *parent,
                    Mapping       *mapping,
                    unsigned       comm,
                    pami_task_t    nranks,
                    pami_task_t   *ranks,
                    GeometryMap   *geometry_map,
                    size_t         context_offset, /* may be PAMI_ALL_CONTEXTS */
                    size_t         ncontexts):
      Geometry<Common>(parent,
                       mapping,
                       comm,
                       nranks,
                       ranks),
      _generation_id(0),
      _ue_barrier(NULL),
      _default_barrier(NULL),
      _commid(comm),
      _client(client),
      _rank(mapping->task()),
      _ranks_malloc(false),
      _ranks(ranks),
      _geometry_map(geometry_map),
      _checkpointed(false),
      _cb_result(PAMI_EAGAIN),
      _allcontexts(context_offset==PAMI_ALL_CONTEXTS),
      _ctxt_offset(context_offset==PAMI_ALL_CONTEXTS?0:context_offset),
      _ctxt_arr_sz(context_offset==PAMI_ALL_CONTEXTS?ncontexts:context_offset+1)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>",this);
        // this creates the topology including all subtopologies
        new(&_topos[DEFAULT_TOPOLOGY_INDEX]) PAMI::Topology(_ranks, nranks);
        buildSpecialTopologies();

        // Initialize remaining members
        (*_geometry_map)[_commid] = this;

        for(size_t n=0; n<MAX_CONTEXTS; n++)
        {
          _allreduce[n][0] = _allreduce[n][1] = NULL;
          _allreduce_async_mode[n]      = 1;
          _allreduce_iteration[n]       = 0;
        }

        _cb_done = (pami_callback_t)
        {
          NULL, NULL
        };

        _kvcstore = allocKVS(NUM_CKEYS,ncontexts);

        TRACE_FORMAT("<%p>%d: _allcontexts %d, context_offset %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu, _ctxt_offset %zu\n",this,__LINE__,_allcontexts, context_offset, _ctxt_offset, _ctxt_arr_sz, _ctxt_offset);
        pami_result_t rc = __global.heap_mm->memalign((void **)&_ue_barrier,
                                                      0,
                                                      _ctxt_arr_sz*sizeof(*_ue_barrier));
        (void)rc; //unused warnings in assert case
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _ue_barier");

        rc = __global.heap_mm->memalign((void **)&_default_barrier,
                                        0,
                                        _ctxt_arr_sz*sizeof(*_default_barrier));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _temp_topo");

        // Allocate per context UE queues
        rc = __global.heap_mm->memalign((void **)&_ue,
                                        0,
                                        _ctxt_arr_sz*sizeof(MatchQueue<>));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _temp_topo");

        // Allocate per context Posted queues
        rc = __global.heap_mm->memalign((void **)&_post,
                                        0,
                                        _ctxt_arr_sz*sizeof(MatchQueue<>));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _temp_topo");

        // Allocate per context active message dispatch information
        rc = __global.heap_mm->memalign((void **)&_dispatch,
                                        0,
                                        _ctxt_arr_sz*sizeof(DispatchMap));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _dispatch");



        TRACE_FORMAT("<%p>%d: _allcontexts %d, context_offset %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu, _ctxt_offset %zu\n",this,__LINE__,_allcontexts, context_offset, _ctxt_offset, _ctxt_arr_sz, _ctxt_offset);
        for(size_t n=_ctxt_offset; n<_ctxt_arr_sz; n++)
        {
          new(&_ue_barrier[n]) AlgorithmT(NULL,NULL);
          new(&_default_barrier[n]) AlgorithmT(NULL,NULL);
          new(&_ue[n]) MatchQueue<>();
          new(&_post[n]) MatchQueue<>();
          new(&_dispatch[n]) DispatchMap();
          resetUEBarrier_impl(n);
          resetDefaultBarrier_impl(n);
        }
        TRACE_FN_EXIT();
      }

      /**
         Construct a geometry based on a list of endpoints
      */
      inline Common (pami_client_t    client,
                     Common          *parent,
                     Mapping         *mapping,
                     unsigned         comm,
                     pami_endpoint_t  neps,
                     pami_endpoint_t *eps,
                     GeometryMap     *geometry_map,
                     bool):
      Geometry<Common>(parent,
                       mapping,
                       comm,
                       neps,
                       eps),
      _generation_id(0),
      _ue_barrier(NULL),
      _default_barrier(NULL),
      _commid(comm),
      _client(client),
      _rank(mapping->task()),
      _ranks_malloc(false),
      _endpoints(eps),
      _geometry_map(geometry_map),
      _checkpointed(false),
      _cb_result(PAMI_EAGAIN),
      _allcontexts(true),
      _ctxt_offset(MAX_CONTEXTS), // will be reset to lowest context found
      _ctxt_arr_sz(MAX_CONTEXTS)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>", this);
        // this creates the topology including all subtopologies
        new(&_topos[DEFAULT_TOPOLOGY_INDEX]) PAMI::Topology(_endpoints, neps, PAMI::tag_eplist());

        buildSpecialTopologies();
        size_t topo_size =  _topos[LOCAL_TOPOLOGY_INDEX].size();
        for(unsigned i=0; i < topo_size; i++)
          {
            pami_endpoint_t ep    = _topos[LOCAL_TOPOLOGY_INDEX].index2Endpoint(i);
            pami_task_t     task;
            size_t          offset;
            PAMI_ENDPOINT_INFO(ep, task, offset);
            if(offset < _ctxt_offset) _ctxt_offset = offset;
          }

#if DO_TRACE_DEBUG
        TRACE_FORMAT("<%p>LOCAL_TOPOLOGY_INDEX size %zu, found lowest context %zu", this,topo_size, _ctxt_offset);
        TRACE_FORMAT("<%p>EP DEFAULT Topology sz=%zu:", this,_topos[DEFAULT_TOPOLOGY_INDEX].size());
        for(size_t i=0; i< _topos[DEFAULT_TOPOLOGY_INDEX].size(); i++)
        {
          pami_endpoint_t ep    = _topos[DEFAULT_TOPOLOGY_INDEX].index2Endpoint(i);
          pami_task_t     task;
          size_t          offset;
          PAMI_ENDPOINT_INFO(ep, task, offset);
          TRACE_FORMAT("<%p>   --->(ep=%d task=%d offset=%ld)", this,ep, task, offset);
        }

        TRACE_FORMAT("<%p>EP LOCAL Topology sz=%zu", this,_topos[LOCAL_TOPOLOGY_INDEX].size());
        for(size_t i=0; i< _topos[LOCAL_TOPOLOGY_INDEX].size(); i++)
        {
          pami_endpoint_t ep    = _topos[LOCAL_TOPOLOGY_INDEX].index2Endpoint(i);
          pami_task_t     task;
          size_t          offset;
          PAMI_ENDPOINT_INFO(ep, task, offset);
          TRACE_FORMAT("<%p>   --->(ep=%d task=%d offset=%ld)", this,ep, task, offset);
        }

        TRACE_FORMAT("<%p>EP MASTER Topology: sz=%zu", this,_topos[MASTER_TOPOLOGY_INDEX].size());
        for(size_t i=0; i< _topos[MASTER_TOPOLOGY_INDEX].size(); i++)
        {
          pami_endpoint_t ep    = _topos[MASTER_TOPOLOGY_INDEX].index2Endpoint(i);
          pami_task_t     task;
          size_t          offset;
          PAMI_ENDPOINT_INFO(ep, task, offset);
          TRACE_FORMAT("<%p>   --->(ep=%d task=%d offset=%ld)", this,ep, task, offset);
        }
#endif



        // Initialize remaining members
        (*_geometry_map)[_commid] = this;

        for(size_t n=0; n<_ctxt_arr_sz; n++)
        {
          _allreduce[n][0] = _allreduce[n][1] = NULL;
          _allreduce_async_mode[n]      = 1;
          _allreduce_iteration[n]       = 0;
        }

        _cb_done = (pami_callback_t)
        {
          NULL, NULL
        };

        _kvcstore = allocKVS(NUM_CKEYS,MAX_CONTEXTS);
        TRACE_FORMAT("<%p>%d: _allcontexts %d, _ctxt_arr_sz %zu, _ctxt_offset %zu\n",this,__LINE__,_allcontexts, _ctxt_arr_sz, _ctxt_offset);
        pami_result_t rc = __global.heap_mm->memalign((void **)&_ue_barrier,
                                                      0,
                                                      _ctxt_arr_sz*sizeof(*_ue_barrier));
        (void)rc; //unused warnings in assert case
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _ue_barrier");

        rc = __global.heap_mm->memalign((void **)&_default_barrier,
                                        0,
                                        _ctxt_arr_sz*sizeof(*_default_barrier));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _default_barrier");

        // Allocate per context UE queues
        rc = __global.heap_mm->memalign((void **)&_ue,
                                        0,
                                        _ctxt_arr_sz*sizeof(MatchQueue<>));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _temp_topo");

        // Allocate per context Posted queues
        rc = __global.heap_mm->memalign((void **)&_post,
                                        0,
                                        _ctxt_arr_sz*sizeof(MatchQueue<>));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _temp_topo");

        // Allocate per context active message dispatch information
        rc = __global.heap_mm->memalign((void **)&_dispatch,
                                        0,
                                        _ctxt_arr_sz*sizeof(DispatchMap));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _dispatch");


        TRACE_FORMAT("<%p>%d: _allcontexts %d, _ctxt_arr_sz %zu, _ctxt_offset %zu\n",this,__LINE__,_allcontexts, _ctxt_arr_sz, _ctxt_offset);
        for(size_t n=0; n<_ctxt_arr_sz; n++)
        {
          new(&_ue_barrier[n]) AlgorithmT(NULL,NULL);
          new(&_default_barrier[n]) AlgorithmT(NULL,NULL);
          new(&_ue[n]) MatchQueue<>();
          new(&_post[n]) MatchQueue<>();
          new(&_dispatch[n]) DispatchMap();
          resetUEBarrier_impl(n);
          resetDefaultBarrier_impl(n);
        }
        TRACE_FN_EXIT();
      }

      inline Common (pami_client_t          client,
                     Common                *parent,
                     Mapping               *mapping,
                     unsigned               comm,
                     int                    numranges,
                     pami_geometry_range_t  rangelist[],
                     GeometryMap           *geometry_map,
                     size_t                 context_offset,/* may be PAMI_ALL_CONTEXTS */
                     size_t                 ncontexts):
      Geometry<Common>(parent,
                       mapping,
                       comm,
                       numranges,
                       rangelist),
      _generation_id(0),
      _ue_barrier(NULL),
      _default_barrier(NULL),
      _commid(comm),
      _client(client),
      _rank(mapping->task()),
      _ranks_malloc(false),
      _ranks(NULL),
      _geometry_map(geometry_map),
      _checkpointed(false),
      _cb_result(PAMI_EAGAIN),
      _allcontexts(context_offset==PAMI_ALL_CONTEXTS),
      _ctxt_offset(context_offset==PAMI_ALL_CONTEXTS?0:context_offset),
      _ctxt_arr_sz(context_offset==PAMI_ALL_CONTEXTS?ncontexts:context_offset+1)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p> numranges %d, context_offset %zu, ncontexts %zu", this, numranges, context_offset, ncontexts);
        pami_result_t rc;
        if (numranges == 1)
        {
          new(&_topos[DEFAULT_TOPOLOGY_INDEX]) PAMI::Topology(rangelist[0].lo, rangelist[0].hi,context_offset, ncontexts);
        }
        else // build a rank list from N ranges
        {
          pami_task_t nranks = 0;
          int i, j, k;

          for(i = 0; i < numranges; i++)
            nranks += (rangelist[i].hi - rangelist[i].lo + 1);

          _ranks_malloc = true;
          rc = __global.heap_mm->memalign((void **)&_ranks, 0, nranks * sizeof(pami_task_t));
          PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _ranks");

          for(k = 0, i = 0; i < numranges; i++)
          {
            int size = rangelist[i].hi - rangelist[i].lo + 1;

            for(j = 0; j < size; j++, k++)
              _ranks[k] = rangelist[i].lo + j;
          }

          new(&_topos[DEFAULT_TOPOLOGY_INDEX]) PAMI::Topology(_ranks, nranks,context_offset,ncontexts);
        }

        buildSpecialTopologies();

        if((_topos[LIST_TOPOLOGY_INDEX].type() == PAMI_LIST_TOPOLOGY) &&
           (_ranks_malloc == false)) /* Don't overwrite our original _ranks */
        {
          pami_result_t rc = PAMI_SUCCESS;
          rc = _topos[LIST_TOPOLOGY_INDEX].rankList(&_ranks);
          PAMI_assert(rc == PAMI_SUCCESS);
        }
        // Initialize remaining members

        (*_geometry_map)[_commid] = this;

        for(size_t n=0; n<MAX_CONTEXTS; n++)
        {
          _allreduce[n][0] = _allreduce[n][1] = NULL;
          _allreduce_async_mode[n]      = 1;
          _allreduce_iteration[n]       = 0;
        }

        _cb_done = (pami_callback_t)
        {
          NULL, NULL
        };

        _kvcstore = allocKVS(NUM_CKEYS,ncontexts);

        rc               = __global.heap_mm->memalign((void **)&_ue_barrier,
                                                      0,
                                                      _ctxt_arr_sz*sizeof(*_ue_barrier));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _ue_barrier");

        rc               = __global.heap_mm->memalign((void **)&_default_barrier,
                                                      0,
                                                      _ctxt_arr_sz*sizeof(*_default_barrier));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _default_barrier");

        // Allocate per context UE queues
        rc = __global.heap_mm->memalign((void **)&_ue,
                                        0,
                                        _ctxt_arr_sz*sizeof(MatchQueue<>));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _temp_topo");

        // Allocate per context Posted queues
        rc = __global.heap_mm->memalign((void **)&_post,
                                        0,
                                        _ctxt_arr_sz*sizeof(MatchQueue<>));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _temp_topo");

        // Allocate per context active message dispatch information
        rc = __global.heap_mm->memalign((void **)&_dispatch,
                                        0,
                                        _ctxt_arr_sz*sizeof(DispatchMap));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _dispatch");


        TRACE_FORMAT("<%p>%d: _allcontexts %d, context_offset %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu, _ctxt_offset %zu\n",this,__LINE__,_allcontexts, context_offset, _ctxt_offset, _ctxt_arr_sz, _ctxt_offset);
        for(size_t n=_ctxt_offset; n<_ctxt_arr_sz; n++)
        {
          new(&_ue_barrier[n]) AlgorithmT(NULL,NULL);
          new(&_default_barrier[n]) AlgorithmT(NULL,NULL);
          new(&_ue[n]) MatchQueue<>();
          new(&_post[n]) MatchQueue<>();
          new(&_dispatch[n]) DispatchMap();
          resetUEBarrier_impl(n);
          resetDefaultBarrier_impl(n);
        }
        TRACE_FN_EXIT();
      }

      inline Common (pami_client_t   client,
                     Common         *parent,
                     Mapping        *mapping,
                     unsigned        comm,
                     PAMI::Topology *topology,
                     GeometryMap    *geometry_map,
                     size_t          context_offset,/* may be PAMI_ALL_CONTEXTS */
                     size_t          ncontexts):
      Geometry<Common>(parent,
                       mapping,
                       comm,
                       topology),
      _generation_id(0),
      _ue_barrier(NULL),
      _default_barrier(NULL),
      _commid(comm),
      _client(client),
      _rank(mapping->task()),
      _ranks_malloc(false),
      _ranks(NULL),
      _geometry_map(geometry_map),
      _checkpointed(false),
      _cb_result(PAMI_EAGAIN),
      _ctxt_offset(context_offset==PAMI_ALL_CONTEXTS?0:context_offset),
      _ctxt_arr_sz(context_offset==PAMI_ALL_CONTEXTS?ncontexts:context_offset+1)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>", this);

        _topos[DEFAULT_TOPOLOGY_INDEX] = *topology;

        if(_topos[DEFAULT_TOPOLOGY_INDEX].type() == PAMI_LIST_TOPOLOGY)
        {
          _topos[DEFAULT_TOPOLOGY_INDEX].rankList(&_ranks);
        }
        buildSpecialTopologies();

        // Initialize remaining members

        (*_geometry_map)[_commid] = this;

        for(size_t n=0; n<MAX_CONTEXTS; n++)
        {
          _allreduce[n][0] = _allreduce[n][1] = NULL;
          _allreduce_async_mode[n]      = 1;
          _allreduce_iteration[n]       = 0;
        }

        _cb_done = (pami_callback_t)
        {
          NULL, NULL
        };

        _kvcstore = allocKVS(NUM_CKEYS,ncontexts);

        TRACE_FORMAT("<%p>%d: _allcontexts %d, context_offset %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu, _ctxt_offset %zu\n",this,__LINE__,_allcontexts, context_offset, _ctxt_offset, _ctxt_arr_sz, _ctxt_offset);
        pami_result_t rc = __global.heap_mm->memalign((void **)&_ue_barrier,
                                                      0,
                                                      _ctxt_arr_sz*sizeof(*_ue_barrier));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _ue_barrier");
        (void)rc; //to avoid unused warnings in non-assert

        rc = __global.heap_mm->memalign((void **)&_default_barrier,
                                        0,
                                        _ctxt_arr_sz*sizeof(*_default_barrier));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _default_barrier");

        // Allocate per context UE queues
        rc = __global.heap_mm->memalign((void **)&_ue,
                                        0,
                                        _ctxt_arr_sz*sizeof(MatchQueue<>));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _temp_topo");

        // Allocate per context Posted queues
        rc = __global.heap_mm->memalign((void **)&_post,
                                        0,
                                        _ctxt_arr_sz*sizeof(MatchQueue<>));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _temp_topo");

        // Allocate per context active message dispatch information
        rc = __global.heap_mm->memalign((void **)&_dispatch,
                                        0,
                                        _ctxt_arr_sz*sizeof(DispatchMap));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _dispatch");


        TRACE_FORMAT("<%p>%d: _allcontexts %d, context_offset %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu, _ctxt_offset %zu\n",this,__LINE__,_allcontexts, context_offset, _ctxt_offset, _ctxt_arr_sz, _ctxt_offset);
        for(size_t n=_ctxt_offset; n<_ctxt_arr_sz; n++)
        {
          new(&_ue_barrier[n]) AlgorithmT(NULL,NULL);
          new(&_default_barrier[n]) AlgorithmT(NULL,NULL);
          new(&_ue[n]) MatchQueue<>();
          new(&_post[n]) MatchQueue<>();
          new(&_dispatch[n]) DispatchMap();
          resetUEBarrier_impl(n);
          resetDefaultBarrier_impl(n);
        }
        TRACE_FN_EXIT();
      }


      inline Common (pami_client_t          client,
                     Common                *parent,
                     Mapping               *mapping,
                     unsigned               comm,
                     int                    numranges,
                     pami_geometry_range_t  rangelist[],
                     PAMI::Topology        *coord,
                     PAMI::Topology        *local,
                     PAMI::Topology        *master,
                     GeometryMap           *geometry_map,
                     size_t                 context_offset,/* may be PAMI_ALL_CONTEXTS */
                     size_t                 ncontexts):
      Geometry<Common>(parent,
                       mapping,
                       comm,
                       numranges,
                       rangelist),
      _generation_id(0),
      _ue_barrier(NULL),
      _default_barrier(NULL),
      _commid(comm),
      _client(client),
      _rank(mapping->task()),
      _ranks_malloc(false),
      _ranks(NULL),
      _geometry_map(geometry_map),
      _checkpointed(false),
      _cb_result(PAMI_EAGAIN),
      _allcontexts(context_offset==PAMI_ALL_CONTEXTS),
      _ctxt_offset(context_offset==PAMI_ALL_CONTEXTS?0:context_offset),
      _ctxt_arr_sz(context_offset==PAMI_ALL_CONTEXTS?ncontexts:context_offset+1)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p> numranges %d, context_offset %zu, ncontexts %zu", this, numranges, context_offset, ncontexts);
        pami_result_t rc;
        if (numranges == 1)
        {
          new(&_topos[DEFAULT_TOPOLOGY_INDEX]) PAMI::Topology(rangelist[0].lo, rangelist[0].hi,context_offset, ncontexts);
        }
        else // build a rank list from N ranges
        {
          pami_task_t nranks = 0;
          int i, j, k;

          for(i = 0; i < numranges; i++)
            nranks += (rangelist[i].hi - rangelist[i].lo + 1);

          _ranks_malloc = true;
          rc = __global.heap_mm->memalign((void **)&_ranks, 0, nranks * sizeof(pami_task_t));
          PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _ranks");

          for(k = 0, i = 0; i < numranges; i++)
          {
            int size = rangelist[i].hi - rangelist[i].lo + 1;

            for(j = 0; j < size; j++, k++)
              _ranks[k] = rangelist[i].lo + j;
          }

          new(&_topos[DEFAULT_TOPOLOGY_INDEX]) PAMI::Topology(_ranks, nranks,context_offset,ncontexts);
        }

        buildSpecialTopologies(coord,local,master);

        if((_topos[LIST_TOPOLOGY_INDEX].type() == PAMI_LIST_TOPOLOGY) &&
           (_ranks_malloc == false)) /* Don't overwrite our original _ranks */
        {
          pami_result_t rc = PAMI_SUCCESS;
          rc = _topos[LIST_TOPOLOGY_INDEX].rankList(&_ranks);
          PAMI_assert(rc == PAMI_SUCCESS);
        }
        // Initialize remaining members

        (*_geometry_map)[_commid] = this;

        for(size_t n=0; n<MAX_CONTEXTS; n++)
        {
          _allreduce[n][0] = _allreduce[n][1] = NULL;
          _allreduce_async_mode[n]      = 1;
          _allreduce_iteration[n]       = 0;
        }

        _cb_done = (pami_callback_t)
        {
          NULL, NULL
        };

        _kvcstore = allocKVS(NUM_CKEYS,ncontexts);

        TRACE_FORMAT("<%p>%d: _allcontexts %d, context_offset %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu, _ctxt_offset %zu\n",this,__LINE__,_allcontexts, context_offset, _ctxt_offset, _ctxt_arr_sz, _ctxt_offset);
        rc               = __global.heap_mm->memalign((void **)&_ue_barrier,
                                                      0,
                                                      _ctxt_arr_sz*sizeof(*_ue_barrier));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _ue_barrier");

        rc               = __global.heap_mm->memalign((void **)&_default_barrier,
                                                      0,
                                                      _ctxt_arr_sz*sizeof(*_default_barrier));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _default_barrier");

        // Allocate per context UE queues
        rc = __global.heap_mm->memalign((void **)&_ue,
                                        0,
                                        _ctxt_arr_sz*sizeof(MatchQueue<>));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _temp_topo");

        // Allocate per context Posted queues
        rc = __global.heap_mm->memalign((void **)&_post,
                                        0,
                                        _ctxt_arr_sz*sizeof(MatchQueue<>));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _temp_topo");

        // Allocate per context active message dispatch information
        rc = __global.heap_mm->memalign((void **)&_dispatch,
                                        0,
                                        _ctxt_arr_sz*sizeof(DispatchMap));
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _dispatch");


        TRACE_FORMAT("<%p>%d: _allcontexts %d, context_offset %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu, _ctxt_offset %zu\n",this,__LINE__,_allcontexts, context_offset, _ctxt_offset, _ctxt_arr_sz, _ctxt_offset);
        for(size_t n=_ctxt_offset; n<_ctxt_arr_sz; n++)
        {
          new(&_ue_barrier[n]) AlgorithmT(NULL,NULL);
          new(&_default_barrier[n]) AlgorithmT(NULL,NULL);
          new(&_ue[n]) MatchQueue<>();
          new(&_post[n]) MatchQueue<>();
          new(&_dispatch[n]) DispatchMap();
          resetUEBarrier_impl(n);
          resetDefaultBarrier_impl(n);
        }
        TRACE_FN_EXIT();
      }

      bool isValidChild(pami_geometry_t g)
      {
        bool rc = true;
        Common * geometry = (Common *)g;
        size_t topo_size =  geometry->_topos[LOCAL_TOPOLOGY_INDEX].size();
        for(unsigned i=0; i < topo_size; i++)
          {
            pami_endpoint_t ep    = geometry->_topos[LOCAL_TOPOLOGY_INDEX].index2Endpoint(i);
            if(this->_topos[LOCAL_TOPOLOGY_INDEX].isEndpointMember(ep) == false)
            {
              rc = false;
              break;
            }
          }
        return rc;

      }

      void                             buildSpecialTopologies(PAMI::Topology* coord=NULL, PAMI::Topology* local=NULL, PAMI::Topology* master=NULL)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>Default type %X, coord %p, local %p",this,_topos[DEFAULT_TOPOLOGY_INDEX].type(),coord,local);
        // build local and global topos
        /* for efficiancy/BGQ-torus reasons, we might want to build master/local topologies from a coordinate
           topology and not from the input/default topology... determine which to use */
        topologyIndex_t BASE_INDEX = DEFAULT_TOPOLOGY_INDEX; /* assume we use the input/default topology */

        if (!_allcontexts && _ctxt_offset==0) {
          if(coord) 
          {
            _topos[COORDINATE_TOPOLOGY_INDEX] = *coord;
            BASE_INDEX = COORDINATE_TOPOLOGY_INDEX;
          }
          else      
          {
            _topos[COORDINATE_TOPOLOGY_INDEX] = _topos[DEFAULT_TOPOLOGY_INDEX];  // first copy it
            if(_topos[COORDINATE_TOPOLOGY_INDEX].type() !=  PAMI_COORD_TOPOLOGY) // convert it?
            {
              // Attempt to create a coordinate topo (result may be EMPTY)
              _topos[COORDINATE_TOPOLOGY_INDEX].convertTopology(PAMI_COORD_TOPOLOGY);
              TRACE_FORMAT("<%p>Default type %X, Coord type %X",this,_topos[DEFAULT_TOPOLOGY_INDEX].type(),_topos[COORDINATE_TOPOLOGY_INDEX].type());

              // Was it successful?
              if(_topos[COORDINATE_TOPOLOGY_INDEX].type() == PAMI_COORD_TOPOLOGY)
              {
                BASE_INDEX = COORDINATE_TOPOLOGY_INDEX; 
              }
              else
                new(&_topos[COORDINATE_TOPOLOGY_INDEX]) PAMI::Topology(); // Empty is better than questionable topology leftover in this entry

            }
          }
        }
        else
          new(&_topos[COORDINATE_TOPOLOGY_INDEX]) PAMI::Topology(); // Initialize to an empty topology

        if(master)// && (BASE_INDEX != COORDINATE_TOPOLOGY_INDEX)) 
        {
            _topos[MASTER_TOPOLOGY_INDEX] = *master;
        }
        else
        {
          /* Use whatever topology we picked above as the basis for Nth masters and local */
          TRACE_FORMAT("<%p>Base index for Nth master and local %u",this,BASE_INDEX);
          _topos[BASE_INDEX].subTopologyNthGlobal(&_topos[MASTER_TOPOLOGY_INDEX], 0);
        }

        /* Copy or create the local topo */
        if(local) _topos[LOCAL_TOPOLOGY_INDEX] = *local;
        else      _topos[BASE_INDEX].subTopologyLocalToMe(&_topos[LOCAL_TOPOLOGY_INDEX]);

        // Find master participant on the tree/cau network
        _topos[MASTER_TOPOLOGY_INDEX].subTopologyLocalToMe(&_topos[LOCAL_MASTER_TOPOLOGY_INDEX]);

#if DO_TRACE_DEBUG
        unsigned j;
        for (j = 0; j < _topos[DEFAULT_TOPOLOGY_INDEX].size(); ++j)    TRACE_FORMAT("<%p>   DEFAULT_TOPOLOGY[%u]=%zu, size %zu",this, j, (size_t)_topos[DEFAULT_TOPOLOGY_INDEX].index2Endpoint(j),    _topos[DEFAULT_TOPOLOGY_INDEX].size());
        for (j = 0; j < _topos[MASTER_TOPOLOGY_INDEX].size(); ++j)     TRACE_FORMAT("<%p>    MASTER_TOPOLOGY[%u]=%zu, size %zu",this, j, (size_t)_topos[MASTER_TOPOLOGY_INDEX].index2Endpoint(j),     _topos[MASTER_TOPOLOGY_INDEX].size());
        for (j = 0; j < _topos[LOCAL_TOPOLOGY_INDEX].size(); ++j)      TRACE_FORMAT("<%p>     LOCAL_TOPOLOGY[%u]=%zu, size %zu",this, j, (size_t)_topos[LOCAL_TOPOLOGY_INDEX].index2Endpoint(j),      _topos[LOCAL_TOPOLOGY_INDEX].size());
        for (j = 0; j < _topos[COORDINATE_TOPOLOGY_INDEX].size(); ++j) TRACE_FORMAT("<%p>COORDINATE_TOPOLOGY[%u]=%zu, size %zu",this, j, (size_t)_topos[COORDINATE_TOPOLOGY_INDEX].index2Endpoint(j), _topos[COORDINATE_TOPOLOGY_INDEX].size());
#endif
        // If we already have a rank list, set the special topology, otherwise
        // leave it EMPTY unless needed because it will require a new rank list allocation
        if(_topos[DEFAULT_TOPOLOGY_INDEX].type() != PAMI_LIST_TOPOLOGY)
          new(&_topos[LIST_TOPOLOGY_INDEX]) PAMI::Topology();
        else // Default is a list topology, use the same ranklist storage
        {
          pami_task_t  nranks = _topos[DEFAULT_TOPOLOGY_INDEX].size();
          pami_task_t *ranks;
          _topos[DEFAULT_TOPOLOGY_INDEX].rankList(&ranks);
          new(&_topos[LIST_TOPOLOGY_INDEX]) PAMI::Topology(ranks, nranks);
        }
        TRACE_FN_EXIT();
      }

      inline pami_topology_t*          getTopology_impl(topologyIndex_t topo_num)
      {
        return(pami_topology_t*)&_topos[topo_num];
      }

      inline void                      setAsyncAllreduceMode_impl(size_t   context_id,
                                                                  unsigned value)
      {
        _allreduce_async_mode[context_id] = value;
      }
      inline unsigned                  getAsyncAllreduceMode_impl(size_t context_id)
      {
        return _allreduce_async_mode[context_id];
      }
      inline unsigned                  incrementAllreduceIteration_impl(size_t context_id)
      {
        _allreduce_iteration[context_id] ^= _allreduce_async_mode[context_id]; // "increment" with defined mode
        return _allreduce_iteration[context_id];
      }

      inline unsigned                  comm_impl()
      {
        return _commid;
      }

      inline unsigned                  getAllreduceIteration_impl(size_t context_id)
      {
        return _allreduce_iteration[context_id];
      }
      inline void                      freeAllocations_impl()
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>%d: _allcontexts %d, _ctxt_offset %zu\n",this,__LINE__,_allcontexts, _ctxt_offset);
        int sz = _cleanupFcns.size();
        for(int i=0; i<sz; i++)
        {
          pami_event_function  fn = _cleanupFcns.front();  _cleanupFcns.pop_front();
          void                *cd = _cleanupDatas.front(); _cleanupDatas.pop_front();
          if(fn) fn(NULL, cd, PAMI_SUCCESS);
        }

        if(_ranks_malloc) __global.heap_mm->free(_ranks);

        _ranks = NULL;
        _ranks_malloc = false;

        __global.heap_mm->free(_ue_barrier);
        __global.heap_mm->free(_default_barrier);
        __global.heap_mm->free(_ue);
        __global.heap_mm->free(_post);

        freeKVS(_kvcstore, NUM_CKEYS);

        (*_geometry_map)[_commid] = NULL;

        TRACE_FN_EXIT();
        return;
      }
      inline MatchQueue<>             &asyncCollectivePostQ_impl(size_t ctxt_id)
      {
        PAMI_assertf(((ctxt_id >= _ctxt_offset) && (ctxt_id < _ctxt_arr_sz)),"Out of bounds ctxt_id %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu\n",ctxt_id,_ctxt_offset, _ctxt_arr_sz);
        return _post[ctxt_id];
      }
      inline MatchQueue<>             &asyncCollectiveUnexpQ_impl(size_t ctxt_id)
      {
        PAMI_assertf(((ctxt_id >= _ctxt_offset) && (ctxt_id < _ctxt_arr_sz)),"Out of bounds ctxt_id %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu\n",ctxt_id,_ctxt_offset, _ctxt_arr_sz);
        return _ue[ctxt_id];
      }

      inline COMPOSITE_TYPE            getAllreduceComposite_impl(size_t   context_id,
                                                                  unsigned i)
      {
        return _allreduce[context_id][i];
      }
      inline void                      setAllreduceComposite_impl(size_t         context_id,
                                                                  COMPOSITE_TYPE c)
      {
        _allreduce[context_id][_allreduce_iteration[context_id]] = c;

        if(c) incrementAllreduceIteration_impl(context_id);
      }
      inline void                      setAllreduceComposite_impl(size_t         context_id,
                                                                  COMPOSITE_TYPE c,
                                                                  unsigned       i)
      {
        _allreduce[context_id][i] = c;
      }

      inline COMPOSITE_TYPE            getAllreduceComposite_impl(size_t context_id)
      {
        return _allreduce[context_id][_allreduce_iteration[context_id]];
      }

      inline void                      processUnexpBarrier_impl (MatchQueue<> * ueb_queue,
                                                                 MemoryAllocator < sizeof(PAMI::Geometry::UnexpBarrierQueueElement), 16 > *ueb_allocator)
      {
        UnexpBarrierQueueElement *ueb = NULL;

        while((ueb = (UnexpBarrierQueueElement *)ueb_queue->findAndDelete(_commid)) != NULL)
        {
          CCMI::Executor::Composite *c = (CCMI::Executor::Composite *) getKey(ueb->getContextId(),
                                                                              (ckeys_t)ueb->getAlgorithm());
          c->notifyRecv (ueb->getSrcRank(), ueb->getInfo(), NULL, NULL, NULL);
          ueb_allocator->returnObject(ueb);
        }
      }

      inline size_t               size_impl(void)
      {
        return _topos[DEFAULT_TOPOLOGY_INDEX].size();
      }
      inline pami_task_t               rank_impl(void)
      {
        return _rank;
      }

      inline size_t ordinal_impl (pami_endpoint_t ep)
      {
        return _topos[DEFAULT_TOPOLOGY_INDEX].endpoint2Index(ep);
      }
      inline pami_endpoint_t endpoint_impl (size_t ordinal)
      {
        return _topos[DEFAULT_TOPOLOGY_INDEX].index2Endpoint(ordinal);
      }

      inline void  setDispatch_impl(size_t ctxt_id, size_t key, DispatchInfo *value)
      {
        PAMI_assertf(((ctxt_id >= _ctxt_offset) && (ctxt_id < _ctxt_arr_sz)),"Out of bounds ctxt_id %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu\n",ctxt_id,_ctxt_offset, _ctxt_arr_sz);
        _dispatch[ctxt_id][key] = *value;
      }

      inline DispatchInfo  * getDispatch_impl(size_t ctxt_id, size_t key)
      {
        PAMI_assertf(((ctxt_id >= _ctxt_offset) && (ctxt_id < _ctxt_arr_sz)),"Out of bounds ctxt_id %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu\n",ctxt_id,_ctxt_offset, _ctxt_arr_sz);
        DispatchMap::iterator iter = _dispatch[ctxt_id].find(key);

        if(unlikely(iter == _dispatch[ctxt_id].end()))
        {
          return(DispatchInfo *)NULL;
        }

        return &iter->second;
      }

      inline void  * getKey_impl(size_t context_id, ckeys_t key)
      {
        TRACE_FN_ENTER();
        PAMI_assert(key < NUM_CKEYS);
        PAMI_assert(context_id != -1UL);
        /* kvcstore only allocated enough cols for actual number of contexts used from starting context (_ctxt_offset) to max context (_ctxt_arr_sz) or all contexts */
        PAMI_assertf(((context_id >= _ctxt_offset) && (context_id < _ctxt_arr_sz)),"Out of bounds context_id %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu\n",context_id,_ctxt_offset, _ctxt_arr_sz);
        size_t ctxt_id = _allcontexts? context_id: context_id - _ctxt_offset; 
        void * value = _kvcstore[key][ctxt_id];
        TRACE_FORMAT("<%p>(k=%d, val=%p, ctxt=%zu)",this, key, value,ctxt_id);
        TRACE_FN_EXIT();
        return value;
      }

      inline void                      setKey_impl(size_t context_id, ckeys_t key, void*value)
      {
        TRACE_FN_ENTER();
        PAMI_assert(key < NUM_CKEYS);
        PAMI_assert(context_id != -1UL);
        /* kvcstore only allocated enough cols for actual number of contexts used from starting context (_ctxt_offset) to max context (_ctxt_arr_sz) or all contexts */
        PAMI_assertf(((context_id >= _ctxt_offset) && (context_id < _ctxt_arr_sz)),"Out of bounds context_id %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu\n",context_id,_ctxt_offset, _ctxt_arr_sz);
        size_t ctxt_id = _allcontexts? context_id: context_id - _ctxt_offset; 
        TRACE_FORMAT("<%p>(k=%d, v=%p,ctxt=%zu)", this, key, value,ctxt_id);
        _kvcstore[key][ctxt_id] = value;
        TRACE_FN_EXIT();
      }

      /* Get first key is a poor replacement for the old global key store.  Some key/values aren't particularly
         associated to a context so it's ok to just get the 'first' one (used to be the 'global' one).   */
      inline void  * getFirstKey(ckeys_t key) // Get the key on first available context
      {
        return getKey_impl(_ctxt_offset, key);
      }

      /* Set all keys is a poor replacement for the old global key store.  Some key/values aren't particularly
         associated to a context so it's ok to just set all context key/values (used to be a global key/value). */
      inline void                      setKeyAll(ckeys_t key, void*value) // Set the key in all contexts.
      {
        TRACE_FN_ENTER();
        PAMI_assert(key < NUM_CKEYS);
        /* kvcstore only allocated enough cols for actual number of contexts used from starting context (_ctxt_offset) to max context (_ctxt_arr_sz)  */
        size_t ncontexts = _ctxt_arr_sz - _ctxt_offset; 
        for(size_t ctxt_id = 0; ctxt_id < ncontexts; ctxt_id++)
        {  
          TRACE_FORMAT("<%p>(k=%d, v=%p,ctxt=%zu)", this, key, value,ctxt_id);
          _kvcstore[key][ctxt_id] = value;
        }
        TRACE_FN_EXIT();
      }

      inline pami_result_t             addCollective_impl(pami_xfer_type_t  colltype,
                                                          Factory          *factory,
                                                          pami_context_t    context,
                                                          size_t            context_id)
      {
        TRACE_FN_ENTER();
        (void)context;
        uint32_t hash = factory->nameHash(_generation_id++, (pami_geometry_t)this);
        Algorithm<Geometry<Common> >*elem = &_algoTable[colltype][hash][context_id];
        new(elem) Algorithm<Geometry<Common> >(factory, this);
        setCleanupCallback(resetFactoryCache, factory);
#if DO_TRACE_DEBUG
//        if(PAMI_XFER_ALLREDUCE == colltype)
        {
          pami_metadata_t m;
          factory->metadata(&m);
          TRACE_FORMAT("<%p>%s",this,m.name);
          TRACE_FORMAT("<%p>num algorithms %zu",this,_algoTable[colltype].size());
        }
#endif
        //PAMI_assert_debug(_algoTable[colltype][hash].count(0) > 0); // There must be a context 0 entry.
        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }

      inline pami_result_t             rmCollective_impl(pami_xfer_type_t  colltype,
                                                         Factory          *factory,
                                                         pami_context_t    context,
                                                         size_t            context_id)
      {
        TRACE_FN_ENTER();
        (void)context;
        if(!factory)
        {
          TRACE_FN_EXIT();
          return PAMI_SUCCESS;
        }
        resetCleanupCallback(resetFactoryCache, factory);
        uint32_t hash = factory->nameHash(-1, (pami_geometry_t)this);
        _algoTable[colltype][hash].erase(context_id);
        _algoTable[colltype].erase(hash);
#if DO_TRACE_DEBUG
//        if(PAMI_XFER_ALLREDUCE == colltype)
        {
          pami_metadata_t m;
          factory->metadata(&m);
          TRACE_FORMAT("<%p>%s",this,m.name);
          TRACE_FORMAT("<%p>num algorithms %zu",this,_algoTable[colltype].size());
        }
#endif
        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }

      inline pami_result_t             rmCollectiveCheck_impl(pami_xfer_type_t  colltype,
                                                              Factory          *factory,
                                                              pami_context_t    context,
                                                              size_t            context_id)
      {
        TRACE_FN_ENTER();
        (void)context;
        resetCleanupCallback(resetFactoryCache, factory);
        uint32_t hash = factory->nameHash(-1, (pami_geometry_t)this);
        _algoTableCheck[colltype][hash].erase(context_id);
        _algoTableCheck[colltype].erase(hash);
#if DO_TRACE_DEBUG
//        if(PAMI_XFER_ALLREDUCE == colltype)
        {
          pami_metadata_t m;
          factory->metadata(&m);
          TRACE_FORMAT("<%p>%s",this,m.name);
          TRACE_FORMAT("<%p>num algorithms %zu",this,_algoTableCheck[colltype].size());
        }
#endif
        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }

      inline pami_result_t             addCollectiveCheck_impl(pami_xfer_type_t                            colltype,
                                                               Factory          *factory,
                                                               pami_context_t    context,
                                                               size_t                                     context_id)
      {
        TRACE_FN_ENTER();
        (void)context;
        uint32_t hash = factory->nameHash(_generation_id++, (pami_geometry_t)this);
        Algorithm<Geometry<Common> >*elem = &_algoTableCheck[colltype][hash][context_id];
        new(elem) Algorithm<Geometry<Common> >(factory, this);
        setCleanupCallback(resetFactoryCache, factory);
#if DO_TRACE_DEBUG
//        if(PAMI_XFER_ALLREDUCE == colltype)
        {
          pami_metadata_t m;
          factory->metadata(&m);
          TRACE_FORMAT("<%p>%s",this,m.name);
          TRACE_FORMAT("<%p>num algorithms %zu",this,_algoTableCheck[colltype].size());
        }
#endif
        //PAMI_assert_debug(_algoTableCheck[colltype][hash].count(0) > 0);  // There must be a context 0 entry.
        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }

      pami_result_t                    algorithms_num_impl(pami_xfer_type_t  colltype,
                                                           size_t             *lengths)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>colltype %u, lengths %p",this,colltype, lengths);
        lengths[0] = _algoTable[colltype].size();
        lengths[1] = _algoTableCheck[colltype].size();
        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }

      static bool compare(ContextMap *cm0, ContextMap *cm1)
        {
          // Sort the list based on context 0
          if(cm0 == NULL || cm1 == NULL) return true;
          AlgorithmT a0;
          AlgorithmT a1;
          
          a0 = cm0->begin()->second;
          a1 = cm1->begin()->second;
//          a0 = (*cm0)[0];
//          a1 = (*cm1)[0];
          if(a0._factory->getGenerationId() < a1._factory->getGenerationId())
            return true;
          else
            return false;
        }

      inline pami_result_t             algorithms_info_impl (pami_xfer_type_t   colltype,
                                                             pami_algorithm_t  *algs0,
                                                             pami_metadata_t   *mdata0,
                                                             size_t             num0,
                                                             pami_algorithm_t  *algs1,
                                                             pami_metadata_t   *mdata1,
                                                             size_t             num1)
      {
        TRACE_FN_ENTER();
        HashMap *m = &_algoTable[colltype];
        HashMap::iterator iter;
        size_t i;

        // Sort the algorithm list in insertion order
        AlgoList v0, v1;
        for(i=0,iter=m->begin();iter!=m->end() && i<num0;iter++,i++)
          v0.push_back(&iter->second);

        m = &_algoTableCheck[colltype];
        for(i=0,iter=m->begin();iter!=m->end() && i<num1;iter++,i++)
          v1.push_back(&iter->second);

        v0.sort(compare);
        v1.sort(compare);

        AlgoList::iterator alist_iter;
        AlgoList          *al = &v0;
        for(i=0,alist_iter=al->begin();alist_iter!=al->end() && i<num0;alist_iter++,i++)
        {
          ContextMap *cm = (*alist_iter);
          if(algs0)
            algs0[i] = (pami_algorithm_t) cm;
          if(mdata0)
          {
            ContextMap *cm = (*alist_iter);
            if(algs0)
              algs0[i] = (pami_algorithm_t) cm;
            if(mdata0)
              {
                AlgorithmT *tmp_a = &((*cm)[_ctxt_offset]);
                tmp_a->metadata(&mdata0[i]);
                TRACE_FORMAT("<%p>sorted algorithms_info() %zu out of %zu/%zu %s",this,i,al->size(),num0,mdata0[i].name);
              }
            /* Try to validate the protocol is some way.  Does one exist for every context? */
            size_t iz = _ctxt_offset;
            for(;iz<_ctxt_arr_sz; ++iz)
            {
              if(mdata0)
              {  
                TRACE_FORMAT("<%p>sorted algorithms_info() %zu out of %zu/%zu %s",this,i,al->size(),num0,mdata0[i].name);
              }
              else 
              {  
                TRACE_STRING("no mdata0");
              }
              TRACE_FORMAT("<%p>[%zu]count[%zu] = %zu %s",this,i,iz,(*cm).count(iz),mdata0[i].name);
              PAMI_assert_debugf((*cm).count(iz),"[%zu]count[%zu]=%zu for %s\n",i,iz,(*cm).count(iz),mdata0[i].name); // There must be an context iz entry.
            }
          }
        }
        al = &v1;
        for(i=0,alist_iter=al->begin();alist_iter!=al->end() && i<num1;alist_iter++,i++)
        {
          ContextMap *cm = (*alist_iter);
          if(algs1)
            algs1[i] = (pami_algorithm_t) cm;
          if(mdata1)
          {
            ContextMap *cm = (*alist_iter);
            if(algs1)
              algs1[i] = (pami_algorithm_t) cm;
            if(mdata1)
              {
                AlgorithmT *tmp_a = &((*cm)[_ctxt_offset]);
                tmp_a->metadata(&mdata1[i]);
                TRACE_FORMAT("<%p>sorted algorithms_info(check) %zu out of %zu/%zu %s",this,i,al->size(),num1,mdata1[i].name);
              }
            /* Try to validate the protocol is some way.  Does one exist for every context? */
            size_t iz = _ctxt_offset;
            for(;iz<_ctxt_arr_sz; ++iz)
            {
              if(mdata1)
              {  
                TRACE_FORMAT("<%p>sorted algorithms_info(check) %zu out of %zu/%zu %s",this,i,al->size(),num1,mdata1[i].name);
              }
              else 
              {  
                TRACE_STRING("no mdata1");
              }
              TRACE_FORMAT("<%p>[%zu]count[%zu] = %zu %s",this,i,iz,(*cm).count(iz),mdata1[i].name);
              PAMI_assert_debugf((*cm).count(iz),"[%zu]count[%zu]=%zu for %s\n",i,iz,(*cm).count(iz),mdata1[i].name); // There must be an context iz entry.
            }
          }
        }
        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }

      pami_result_t default_barrier_impl(pami_event_function  cb_done,
                                         void                *cookie,
                                         size_t               ctxt_id,
                                         pami_context_t       context)
      {
        TRACE_FN_ENTER();
        (void)context;
        TRACE_FORMAT("<%p> cb_done %p, cookie %p, ctxt_id %zu, context %p", this, cb_done, cookie, ctxt_id, context);
        PAMI_assert (_default_barrier[ctxt_id]._factory != NULL);
        PAMI_assertf(((ctxt_id >= _ctxt_offset) && (ctxt_id < _ctxt_arr_sz)),"Out of bounds ctxt_id %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu\n",ctxt_id,_ctxt_offset, _ctxt_arr_sz);
        pami_xfer_t cmd;
        cmd.cb_done = cb_done;
        cmd.cookie  = cookie;
        TRACE_FN_EXIT();
        return _default_barrier[ctxt_id].generate(&cmd);
      }

      void resetDefaultBarrier_impl(size_t ctxt_id)
      {
        TRACE_FORMAT("<%p> id %zu, factory %p, geometry %p",this,ctxt_id,_default_barrier[ctxt_id]._factory,_default_barrier[ctxt_id]._geometry);
        PAMI_assertf(((ctxt_id >= _ctxt_offset) && (ctxt_id < _ctxt_arr_sz)),"Out of bounds ctxt_id %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu\n",ctxt_id,_ctxt_offset, _ctxt_arr_sz);
        _default_barrier[ctxt_id]._factory  = (Factory*)NULL;
        _default_barrier[ctxt_id]._geometry = (Common*)NULL;
      }

      pami_result_t setDefaultBarrier_impl(Factory *f,
                                           size_t   ctxt_id)
      {
        TRACE_FORMAT("<%p> id %zu, factory %p, geometry %p",this,ctxt_id,_default_barrier[ctxt_id]._factory,_default_barrier[ctxt_id]._geometry);
        PAMI_assertf(((ctxt_id >= _ctxt_offset) && (ctxt_id < _ctxt_arr_sz)),"Out of bounds ctxt_id %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu\n",ctxt_id,_ctxt_offset, _ctxt_arr_sz);
        if(_default_barrier[ctxt_id]._factory == (Factory*)NULL)
        {
          _default_barrier[ctxt_id]._factory  = f;
          _default_barrier[ctxt_id]._geometry = this;
          TRACE_FORMAT("<%p> id %zu, factory %p, geometry %p",this,ctxt_id,_default_barrier[ctxt_id]._factory,_default_barrier[ctxt_id]._geometry);
          return PAMI_SUCCESS;
        }
        return PAMI_EAGAIN;  // can't set again unless you reset first.
      }

      pami_result_t                    ue_barrier_impl(pami_event_function     cb_done,
                                                       void                   *cookie,
                                                       size_t                  ctxt_id,
                                                       pami_context_t          context)
      {
        TRACE_FN_ENTER();
        (void)context;
        TRACE_FORMAT("<%p> cb_done %p, cookie %p, ctxt_id %zu, context %p", this, cb_done, cookie, ctxt_id, context);
        PAMI_assert (_ue_barrier[ctxt_id]._factory != NULL);
        PAMI_assertf(((ctxt_id >= _ctxt_offset) && (ctxt_id < _ctxt_arr_sz)),"Out of bounds ctxt_id %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu\n",ctxt_id,_ctxt_offset, _ctxt_arr_sz);
        pami_xfer_t cmd;
        cmd.cb_done = cb_done;
        cmd.cookie = cookie;
        TRACE_FN_EXIT();
        return _ue_barrier[ctxt_id].generate(&cmd);
      }

      void resetUEBarrier_impl(size_t ctxt_id)
      {
        TRACE_FORMAT("<%p> ctxt_id %zu factory %p, geometry %p",this, ctxt_id, _ue_barrier[ctxt_id]._factory,_ue_barrier[ctxt_id]._geometry);
        PAMI_assertf(((ctxt_id >= _ctxt_offset) && (ctxt_id < _ctxt_arr_sz)),"Out of bounds ctxt_id %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu\n",ctxt_id,_ctxt_offset, _ctxt_arr_sz);
        _ue_barrier[ctxt_id]._factory  = (Factory*)NULL;
        _ue_barrier[ctxt_id]._geometry = (Common*)NULL;
      }

      pami_result_t setUEBarrier_impl(Factory *f,
                                      size_t   ctxt_id)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p> ctxt_id %zu factory %p, geometry %p",this, ctxt_id, _ue_barrier[ctxt_id]._factory,_ue_barrier[ctxt_id]._geometry);
        PAMI_assertf(((ctxt_id >= _ctxt_offset) && (ctxt_id < _ctxt_arr_sz)),"Out of bounds ctxt_id %zu, _ctxt_offset %zu, _ctxt_arr_sz %zu\n",ctxt_id,_ctxt_offset, _ctxt_arr_sz);
        if(_ue_barrier[ctxt_id]._factory == (Factory*)NULL)
        {
          _ue_barrier[ctxt_id]._factory  = f;
          _ue_barrier[ctxt_id]._geometry = this;
          TRACE_FORMAT("<%p>(ctxt_id=%zu) ue_barrier() %p, %p/%p, %p",
                        this,ctxt_id,
                        &_ue_barrier[ctxt_id],
                        f,
                        _ue_barrier[ctxt_id]._factory,
                        _ue_barrier[ctxt_id]._geometry);
          TRACE_FN_EXIT();
          return PAMI_SUCCESS;
        }
        TRACE_FN_EXIT();
        return PAMI_EAGAIN;  // can't set again unless you reset first.
      }

      pami_client_t                 getClient_impl()
      {
        return _client;
      }

      void setCleanupCallback_impl(pami_event_function fcn, void *data)
      {
        _cleanupFcns.push_back(fcn);
        _cleanupDatas.push_back(data);
      }
      void resetCleanupCallback_impl(pami_event_function fcn, void *data)
      {
        CleanupFunctions::iterator itFcn  = _cleanupFcns.begin();
        CleanupDatas::iterator     itData = _cleanupDatas.begin();
        for(; (itFcn != _cleanupFcns.end()) && (itData != _cleanupDatas.end()); itFcn++,itData++)
        {
          pami_event_function  fn = *itFcn;
          void                *cd = *itData;

          if((cd == data) && (fn == fcn))
          {
            if(fn) fn(NULL, cd, PAMI_SUCCESS);
            _cleanupFcns.erase(itFcn);
            _cleanupDatas.erase(itData);
            return;
          }
        }
      }
      void setCkptCallback(CheckpointCb ckptfcn, CheckpointCb resumefcn,
                           CheckpointCb restartfcn, void *data)
      {
        ITRC(IT_INITTERM, "LapiGeometry 0x%p setCkptCallback()\n", this);
        _ckptFcns.push_back(CheckpointFunction(ckptfcn, resumefcn, restartfcn, data));
      }

      bool Checkpoint()
      {
        ITRC(IT_INITTERM, "LapiGeometry 0x%p _commid=%u: Checkpoint() enters ckptFncs.size()=%d\n",
             this, _commid, _ckptFcns.size());
        CheckpointFunctions::iterator itFcn = _ckptFcns.begin();

        /* invoke all registered checkpoint callbacks */
        for(; itFcn != _ckptFcns.end(); itFcn++)
        {
          CheckpointCb cb   = itFcn->_checkpoint_fn;
          void        *data = itFcn->_cookie;

          bool rc = (*cb)(data);
          if(!rc)
            return false;
        }

        ITRC(IT_INITTERM, "LapiGeometry 0x%p _commid=%u: Checkpoint() exits\n", this, _commid);
        _checkpointed = true;
        return true;
      }

      bool Restart()
      {
        ITRC(IT_INITTERM, "LapiGeometry 0x%p _commid=%u: Restart() enters _ckptFcns.size()=%d\n",
             this, _commid, _ckptFcns.size());
        assert(_checkpointed);
        CheckpointFunctions::iterator itFcn = _ckptFcns.begin();

        /* invoke all registered restart callbacks */
        for(; itFcn != _ckptFcns.end(); itFcn++)
        {
          CheckpointCb cb   = itFcn->_restart_fn;
          void        *data = itFcn->_cookie;

          bool rc = (*cb)(data);
          if(!rc)
            return false;
        }
        ITRC(IT_INITTERM, "LapiGeometry 0x%p _commid=%u: Restart() exits\n", this, _commid);
        _checkpointed = false;
        return true;
      }

      bool Resume()
      {
        ITRC(IT_INITTERM, "LapiGeometry 0x%p _commid=%u: Resume() enters\n", this, _commid);
        assert(_checkpointed);
        CheckpointFunctions::iterator itFcn = _ckptFcns.begin();

        /* invoke all registered resume callbacks */
        for(; itFcn != _ckptFcns.end(); itFcn++)
        {
          CheckpointCb cb   = itFcn->_resume_fn;
          void        *data = itFcn->_cookie;

          bool rc = (*cb)(data);
          if(!rc)
            return false;
        }
        ITRC(IT_INITTERM, "LapiGeometry 0x%p _commid=%u: Resume() exits\n", this, _commid);
        _checkpointed = false;
        return true;
      }

    private:
      AlgoMap                _algoTable;
      AlgoMap                _algoTableCheck;
      int                    _generation_id;
      AlgorithmT            *_ue_barrier;
      AlgorithmT            *_default_barrier;
      void                ***_kvcstore;
      int                    _commid;
      pami_client_t          _client;
      pami_task_t            _rank;
      MatchQueue<>          *_ue;
      MatchQueue<>          *_post;
      bool                   _ranks_malloc;
      pami_task_t           *_ranks;
      pami_task_t           *_endpoints;
      GeometryMap           *_geometry_map;
      void                  *_allreduce[MAX_CONTEXTS][2];
      unsigned               _allreduce_async_mode[MAX_CONTEXTS];
      unsigned               _allreduce_iteration[MAX_CONTEXTS];
      PAMI::Topology         _topos[MAX_NUM_TOPOLOGIES];
      bool                   _checkpointed;
      pami_callback_t        _cb_done;
      pami_result_t          _cb_result;
      bool                   _allcontexts;
      size_t                 _ctxt_offset;
      size_t                 _ctxt_arr_sz;
      GeomCompCtr            _comp;
      CleanupFunctions       _cleanupFcns;
      CleanupDatas           _cleanupDatas;
      CheckpointFunctions    _ckptFcns;
      DispatchMap           *_dispatch;
    public:
      /// Blue Gene/Q Specific functions

      /// \brief Convenience callback used by geometry completion sub-events
      ///
      /// Each sub-event must invoke this when finished, and the final (user) completion
      /// will be invoked once all have checked in.
      ///
      /// \param[in] ctx       The context on which competion is ocurring
      /// \param[in] cookie    The geometry object
      /// \param[in] result    Status of this completion (error or success)
      ///
      static void                      _done_cb(pami_context_t ctx, void *cookie, pami_result_t result)
      {
        Common *thus = (Common *)cookie;
        thus->rmCompletion(ctx, result);
      }

      /// \brief Setup completion for geometry create
      ///
      /// Note: this is valid for both the parent and (new) sub-geometry,
      /// during the create. If any of the geometry event items returns an
      /// error, this callback will be invoked with that error (last error seen).
      ///
      /// \param[in] fn                The completion function to call
      /// \param[in] cookie    Opaque user data for callback
      ///
      inline void                      setCompletion(pami_event_function fn, void *cookie)
      {
        _cb_done = (pami_callback_t)
        {
          fn, cookie
        };
        _cb_result = PAMI_SUCCESS;
      }

      /// \brief Add one completion event to geometry
      ///
      /// This is called each time some work has been started on which
      /// geometry create completion depends (must wait).
      ///
      inline void                      addCompletion()
      {
        _comp.fetch_and_inc();
      }

      /// \brief Remove one completion event (cancel or "done")
      ///
      /// Note: this is valid for both the parent and (new) sub-geometry,
      /// during the create.
      ///
      /// \param[in] ctx       The context on which competion is ocurring
      /// \param[in] result    Status of this completion (error or success)
      ///
      inline void                      rmCompletion(pami_context_t ctx, pami_result_t result)
      {
        if(result != PAMI_SUCCESS) _cb_result = result;

        if(_comp.fetch_and_dec() == 1)
        {
          if(_cb_done.function)
          {
            _cb_done.function(ctx, _cb_done.clientdata, _cb_result);
          }
        }
      }
    }; // class Geometry
  };  // namespace Geometry
}; // namespace PAMI

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#endif
