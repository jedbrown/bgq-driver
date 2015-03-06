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
///
/// \file common/mpi/Client.h
/// \brief PAMI client interface specific for the MPI platform.
///
#ifndef __common_mpi_Client_h__
#define __common_mpi_Client_h__

#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "common/ClientInterface.h"
#include "Context.h"

namespace PAMI
{
  class Client : public Interface::Client<PAMI::Client>
  {
  public:

    static void shutdownfunc()
      {
        MPI_Finalize();
      }
    inline Client (const char           *name,
                   pami_result_t        &result,
                   pami_configuration_t  configuration[],
                   size_t                num_configs) :
      Interface::Client<PAMI::Client>(name, result),
      _client ((pami_client_t) this),
      _references (1),
      _ncontexts (0),
      _mm ()
      {
        static size_t next_client_id = 0;
        // Set the client name string.
        memset ((void *)_name, 0x00, sizeof(_name));
        strncpy (_name, name, sizeof(_name) - 1);

        _clientid = next_client_id++;
        // PAMI_assert(_clientid < PAMI_MAX_NUM_CLIENTS);

        // Get some shared memory for this client
        initializeMemoryManager ();


        // Initialize the world geometry
        MPI_Comm_rank(MPI_COMM_WORLD,&_myrank);
        MPI_Comm_size(MPI_COMM_WORLD,&_mysize);
	pami_result_t rc;
	rc = __global.heap_mm->memalign((void **)&_world_geometry, 0,
							sizeof(*_world_geometry));
	PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _world_geometry");
        _world_range.lo=0;
        _world_range.hi=_mysize-1;
        new(_world_geometry) MPIGeometry(_client, NULL, &__global.mapping,0, 1,&_world_range, &_geometry_map);
        result = PAMI_SUCCESS;
      }

    inline ~Client ()
      {
      }

    static pami_result_t generate_impl (const char           *name,
                                        pami_client_t        *client,
                                        pami_configuration_t  configuration[],
                                        size_t                num_configs)
      {
        pami_result_t res;
        PAMI::Client * clientp;
	res = __global.heap_mm->memalign((void **)&clientp, 0, sizeof(*clientp));
        PAMI_assertf(res == PAMI_SUCCESS, "Failed to alloc PAMI::Client");
        memset ((void *)clientp, 0x00, sizeof(PAMI::Client));
        new (clientp) PAMI::Client (name, res, configuration, num_configs);
        *client = (pami_client_t) clientp;
        return PAMI_SUCCESS;
      }

    static void destroy_impl (pami_client_t client)
      {
        __global.heap_mm->free(client);
      }

    inline char * getName_impl ()
      {
        return _name;
      }

    inline pami_result_t createContext_impl (pami_configuration_t configuration[],
                                             size_t              count,
                                             pami_context_t     * context,
                                             size_t              ncontexts)
      {
        //_context_list->lock ();
        int n = ncontexts;
        if (_ncontexts != 0) {
          return PAMI_ERROR;
        }
        if (_ncontexts + n > 4) {
          n = 4 - _ncontexts;
        }
        if (n <= 0) { // impossible?
          return PAMI_ERROR;
        }

        pami_result_t rc;
        rc = __global.heap_mm->memalign((void **)&_contexts, 16, sizeof(PAMI::Context) * n);
        PAMI_assertf(rc == PAMI_SUCCESS, "posix_memalign failed for _contexts[%d], errno=%d\n", n, errno);
        _platdevs.generate(_clientid, n, _mm);

        // This memset has been removed due to the amount of cycles it takes
        // on simulators.  Lower level initializers should be setting the
        // relevant fields of the context, so this memset should not be
        // needed anyway.
        //memset((void *)_contexts, 0, sizeof(PAMI::Context) * n);
        size_t bytes = (1*1024*1024) / n;
	char *env = getenv("PAMI_CLIENT_SHMEMSIZE");
	if (env) {
		bytes = strtoull(env, NULL, 0) * 1024 * 1024;
	}
        int x;
        for (x = 0; x < n; ++x) {
          context[x] = (pami_context_t)&_contexts[x];
          new (&_contexts[x]) PAMI::Context(this, _clientid, x, n,
                                   &_platdevs, __global.shared_mm, bytes, _world_geometry, &_geometry_map);
          //_context_list->pushHead((QueueElem *)&context[x]);
          //_context_list->unlock();
          _ncontexts = n;
        }
        return PAMI_SUCCESS;
      }

    // DEPRECATED!
    inline pami_result_t destroyContext_impl (pami_context_t context)
      {
        PAMI_abortf("destroyContext for single context is not supported");
        //free(context); // this can't work?!? it was allocated as one big array!
        return PAMI_SUCCESS;
      }
    inline pami_result_t destroyContext_impl (pami_context_t *context, size_t ncontexts)
      {
        PAMI_assertf(ncontexts == _ncontexts, "destroyContext called without all contexts");
        pami_result_t res = PAMI_SUCCESS;
        size_t i;
        for (i = 0; i < _ncontexts; ++i)
          {
            context[i] = NULL;
            PAMI::Context * ctx = &_contexts[i];
            pami_result_t rc = ctx->destroy ();
            if (rc != PAMI_SUCCESS) res = rc;
          }
        __global.heap_mm->free(_contexts);
        _contexts = NULL;
        _ncontexts = 0;
        return res;
      }

    inline pami_result_t query_impl (pami_configuration_t configuration[],
                                     size_t               num_configs)
      {
        size_t i;
        pami_result_t result = PAMI_SUCCESS;
        for(i=0; i<num_configs; i++)
          {
            switch (configuration[i].name)
              {
                case PAMI_CLIENT_NUM_CONTEXTS:
                  configuration[i].value.intval = 1; // real value TBD
                  break;
                case PAMI_CLIENT_CONST_CONTEXTS:
                  configuration[i].value.intval = 1; // real value TBD
                  break;
                case PAMI_CLIENT_TASK_ID:
                  configuration[i].value.intval = __global.mapping.task();
                  break;
                case PAMI_CLIENT_NUM_TASKS:
                  configuration[i].value.intval = __global.mapping.size();
                  break;
                case PAMI_CLIENT_CLOCK_MHZ:
                case PAMI_CLIENT_WTIMEBASE_MHZ:
                  configuration[i].value.intval = __global.time.clockMHz();
                  break;
                case PAMI_CLIENT_WTICK:
                  configuration[i].value.doubleval =__global.time.tick();
                  break;
                case PAMI_CLIENT_MEM_SIZE:
                case PAMI_CLIENT_PROCESSOR_NAME:
                default:
                  result = PAMI_INVAL;
              }
          }
        return result;
      }

    inline pami_result_t update_impl (pami_configuration_t configuration[],
                                      size_t               num_configs)
      {
        return PAMI_INVAL;
      }

    inline size_t getNumContexts()
      {
        return _ncontexts;
      }

    inline PAMI::Context *getContext(size_t ctx)
      {
        return _contexts + ctx;
      }
    inline PAMI::Context *getContexts()
      {
        return _contexts;
      }

    inline size_t getClientId()
      {
        return _clientid;
      }

    inline pami_result_t geometry_world_impl (pami_geometry_t * world_geometry)
      {
        *world_geometry = _world_geometry;
        return PAMI_SUCCESS;
      }

        template <class T_Geometry>
    class PostedClassRoute : public PAMI::Geometry::ClassRouteId<T_Geometry>
    {
    public:
      typedef  PAMI::Geometry::ClassRouteId<T_Geometry> ClassRouteId;
      typedef  typename ClassRouteId::cr_event_function cr_event_function;
      typedef  PAMI::Geometry::Algorithm<T_Geometry>    Algorithm;
      typedef  PAMI::Device::Generic::GenericThread     GenericThread;

      static void _allreduce_done(pami_context_t context, void *cookie, pami_result_t result)
        {
          TRACE((stderr, "%p  Allreduce is Done!\n", cookie));
          PostedClassRoute  *classroute = (PostedClassRoute *)cookie;
          classroute->_done               = true;
        }

      static pami_result_t _do_classroute(pami_context_t context, void *cookie)
        {
          PostedClassRoute *classroute = (PostedClassRoute*)cookie;
          if(classroute->_started == false)
          {
            // Start the Allreduce
            TRACE((stderr, "%p Starting Allreduce\n", cookie));
            classroute->startAllreduce(_allreduce_done, classroute);
            classroute->_started = true;
          }
          if(classroute->_done == true)
          {
              classroute->_result_cb_done(context,
                                          classroute->_result_cookie,
                                          classroute->_bitmask,
                                          classroute->_geometry,
                                          PAMI_SUCCESS);
              classroute->_user_cb_done(context,
                                        classroute->_user_cookie,
                                        PAMI_SUCCESS);
              TRACE((stderr, "%p Classroute is done, Dequeueing\n", classroute));
              if(classroute->_free_bitmask)
                __global.heap_mm->free(classroute->_bitmask);
              __global.heap_mm->free(cookie);
              return PAMI_SUCCESS;
          }
          else
          {
            TRACE((stderr, "%p classroute allreduce busy: %d %d\n",
                    classroute, classroute->_started, classroute->_done));
            return PAMI_EAGAIN;
          }
        }
      static void phase_done(pami_context_t context, void *cookie, pami_result_t result)
        {
          TRACE((stderr, "%p phase_done\n", cookie));
          PostedClassRoute<MPIGeometry> *classroute = (PostedClassRoute<MPIGeometry> *)cookie;
          classroute->start();
        }


    static void cr_func(pami_context_t  context,
                        void           *cookie,
                        uint64_t       *reduce_result,
                        MPIGeometry   *g,
                        pami_result_t   result )
      {
        TRACE((stderr, "%p CR FUNC 1\n", cookie));
        int count = 1;
        PostedClassRoute<MPIGeometry> *classroute = (PostedClassRoute<MPIGeometry> *)cookie;
        classroute->_context->_pgas_collreg->receive_global(0,classroute->_geometry,
                                                            &reduce_result[0],1);
        classroute->_context->_p2p_ccmi_collreg->receive_global(0,classroute->_geometry,
                                                                &reduce_result[1],1);
      }

    static void cr_func2(pami_context_t  context,
                        void            *cookie,
                        uint64_t        *reduce_result,
                        MPIGeometry    *g,
                        pami_result_t    result )
      {
        TRACE((stderr, "%p CR FUNC 2\n", cookie));
        int count=1;
        PostedClassRoute<MPIGeometry> *classroute = (PostedClassRoute<MPIGeometry> *)cookie;
      }

    static void create_classroute(pami_context_t context, void *cookie, pami_result_t result)
      {
        // Barrier is done, start phase 1
        TRACE((stderr, "%p create_classroute\n", cookie));
        PostedClassRoute<MPIGeometry> *classroute = (PostedClassRoute<MPIGeometry>*)cookie;
        classroute->start();
      }
    public:
      PostedClassRoute(Context             *context,
                       Algorithm           *ar_algo,
                       T_Geometry          *geometry,
                       uint64_t            *bitmask,
                       size_t               count,
                       cr_event_function    result_cb_done,
                       void                *result_cookie,
                       pami_event_function  user_cb_done,
                       void                *user_cookie,
                       bool                 free_bitmask = false):
        ClassRouteId(ar_algo, geometry, bitmask, count,
                     result_cb_done, result_cookie, user_cb_done,
                     user_cookie),
        _context(context),
        _started(false),
        _done(false),
        _free_bitmask(free_bitmask),
        _work(_do_classroute, this)
        {
        }
      inline void start()
        {
          TRACE((stderr, "%p Posting work to GD id=%d work=%p\n", this, _context->getId(), &_work))
          _context->_devices->_generics[_context->getId()].postThread(&_work);
        }
      Context        *_context;
      volatile bool   _started;
      volatile bool   _done;
      bool            _free_bitmask;
      GenericThread   _work;
    };


    
    inline pami_result_t geometry_create_taskrange_impl(pami_geometry_t       * geometry,
                                                        pami_configuration_t    configuration[],
                                                        size_t                  num_configs,
                                                        pami_geometry_t         parent,
                                                        unsigned                id,
                                                        pami_geometry_range_t * rank_slices,
                                                        size_t                  slice_count,
                                                        pami_context_t          context,
                                                        pami_event_function     fn,
                                                        void                  * cookie)
      {
        MPIGeometry              *new_geometry = NULL;
        uint64_t                  *to_reduce;
        uint                      to_reduce_count;
	pami_result_t rc;
        // If our new geometry is NOT NULL, we will create a new geometry
        // for this client.  This new geometry will be populated with a
        // set of algorithms.
        if(geometry != NULL)
          {
            rc = __global.heap_mm->memalign((void **)&new_geometry, 0,
                                            sizeof(*new_geometry));
	    PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc new_geometry");

            new(new_geometry)MPIGeometry((pami_client_t)this,
                                          (MPIGeometry*)parent,
                                          &__global.mapping,
                                          id,
                                          slice_count,
                                          rank_slices,
                                          &_geometry_map);
            
            PAMI::Topology *local_master_topology  = (PAMI::Topology *)new_geometry->getTopology(PAMI::Geometry::MASTER_TOPOLOGY_INDEX);
            to_reduce_count = 3 + local_master_topology->size();
            rc = __global.heap_mm->memalign((void **)&to_reduce, 0,
                                            to_reduce_count * sizeof(uint64_t));
            PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc to_reduce");
            for(size_t n=0; n<_ncontexts; n++)
              {
                new_geometry->resetUEBarrier(); // Reset so pgas will select the UE barrier
		int nc = 0;
		int ncur = 0;
                _contexts[n]._pgas_collreg->register_local(n,new_geometry,&to_reduce[0], ncur);
		nc+= ncur;
		ncur = 0;
                _contexts[n]._p2p_ccmi_collreg->register_local(n,new_geometry,&to_reduce[1], ncur);
              }
	    new_geometry->processUnexpBarrier(&_ueb_queue,
                                              &_ueb_allocator);
            *geometry=(MPIGeometry*) new_geometry;
          }

        // Now we must take care of the synchronization of this new geometry
        // First, if a new geometry is created, we will perform an allreduce
        // on the new geometry.
        // If we have a parent geometry, we perform synchronization on that
        // geometry.  If we don't have a parent geometry, we perform an
        // "unexpected" barrier on only the new geometry.  In either case,
        // any node creating a new geometry will allocate a class route.
        // When the synchronization is completed (barrier or uebarrier), it
        // will chain into an allreduce operation, which performs the allocation
        // of the classroute.  When the classroute has been allocated,
        // The "done" event is delivered to the user.
        MPIGeometry      *bargeom = (MPIGeometry*)parent;
        PAMI::Context    *ctxt    = (PAMI::Context *)context;
        if(new_geometry)
          {
            pami_algorithm_t  alg;
            new_geometry->algorithms_info(PAMI_XFER_ALLREDUCE,
                                          &alg,
                                          NULL,
                                          1,
                                          NULL,
                                          NULL,
                                          0,
                                          ctxt->getId());
            Geometry::Algorithm<MPIGeometry> *ar_algo = (Geometry::Algorithm<MPIGeometry> *)alg;
            PostedClassRoute<MPIGeometry> *cr[2];
	    rc = __global.heap_mm->memalign((void **)&cr[0], 0, sizeof(PostedClassRoute<MPIGeometry>));
	    PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc PostedClassRoute<MPIGeometry>");
            rc = __global.heap_mm->memalign((void **)&cr[1], 0, sizeof(PostedClassRoute<MPIGeometry>));
	    PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc PostedClassRoute<MPIGeometry>");
            new(cr[0])PostedClassRoute<MPIGeometry>(ctxt,ar_algo,new_geometry,to_reduce,
                                                     to_reduce_count,PostedClassRoute<MPIGeometry>::cr_func,
                                                     cr[0],PostedClassRoute<MPIGeometry>::phase_done,cr[1]);
            new(cr[1])PostedClassRoute<MPIGeometry>(ctxt,ar_algo,new_geometry,to_reduce,
                                                     to_reduce_count,PostedClassRoute<MPIGeometry>::cr_func2,
                                                     cr[1],fn,cookie, true);
            TRACE((stderr, "Allocated Classroutes:  %p %p\n", cr[0], cr[1]));
            if(bargeom)
              bargeom->default_barrier(PostedClassRoute<MPIGeometry>::create_classroute, cr[0], ctxt->getId(), context);
            else
              new_geometry->ue_barrier(PostedClassRoute<MPIGeometry>::create_classroute, cr[0], ctxt->getId(), context);
          }
        else
          {
            if(bargeom)
              bargeom->default_barrier(fn, cookie, ctxt->getId(), context);
            else
              {
                // Null parent and null new geometry?  Why are you here?
                return PAMI_INVAL;
              }
          }
        return PAMI_SUCCESS;
      }

    inline pami_result_t geometry_create_topology_impl(pami_geometry_t       * geometry,
                                                       pami_configuration_t    configuration[],
                                                       size_t                  num_configs,
                                                       pami_geometry_t         parent,
                                                       unsigned                id,
                                                       pami_topology_t       * topology,
                                                       pami_context_t          context,
                                                       pami_event_function     fn,
                                                       void                  * cookie)
      {
        // todo:  implement this routine
        PAMI_abort();

        return PAMI_SUCCESS;
      }

    inline pami_result_t geometry_create_tasklist_impl(pami_geometry_t       * geometry,
                                                       pami_configuration_t    configuration[],
                                                       size_t                  num_configs,
                                                       pami_geometry_t         parent,
                                                       unsigned                id,
                                                       pami_task_t           * tasks,
                                                       size_t                  task_count,
                                                       pami_context_t          context,
                                                       pami_event_function     fn,
                                                       void                  * cookie)
      {
        // todo:  implement this routine
        MPIGeometry              *new_geometry = NULL;
        uint64_t                  *to_reduce;
        uint                      to_reduce_count;
        pami_result_t             rc; 
        // If our new geometry is NOT NULL, we will create a new geometry
        // for this client.  This new geometry will be populated with a
        // set of algorithms.
        if(geometry != NULL)
          {
	    rc = __global.heap_mm->memalign((void **)&new_geometry, 0,
                                            sizeof(*new_geometry));
	    PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc new_geometry");
            new(new_geometry)MPIGeometry((pami_client_t)this,
                                          (MPIGeometry*)parent,
                                          &__global.mapping,
                                          id,
                                          task_count,
                                          tasks,
                                          &_geometry_map);

            PAMI::Topology *local_master_topology  = (PAMI::Topology *)new_geometry->getTopology(PAMI::Geometry::MASTER_TOPOLOGY_INDEX);
            to_reduce_count = 3 + local_master_topology->size();
            rc = __global.heap_mm->memalign((void **)&to_reduce, 0,
                                            to_reduce_count * sizeof(uint64_t));
            PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc to_reduce");
            for(size_t n=0; n<_ncontexts; n++)
              {
                new_geometry->resetUEBarrier(); // Reset so pgas will select the UE barrier
		int nc = 0;
		int ncur = 0;
                _contexts[n]._pgas_collreg->register_local(n,new_geometry,&to_reduce[0], ncur);
		nc+=ncur;
		ncur=0;
                _contexts[n]._p2p_ccmi_collreg->register_local(n,new_geometry,&to_reduce[1], ncur);
              }
            new_geometry->processUnexpBarrier(&_ueb_queue,
                                              &_ueb_allocator);
            *geometry=(MPIGeometry*) new_geometry;
          }

        // Now we must take care of the synchronization of this new geometry
        // First, if a new geometry is created, we will perform an allreduce
        // on the new geometry.
        // If we have a parent geometry, we perform synchronization on that
        // geometry.  If we don't have a parent geometry, we perform an
        // "unexpected" barrier on only the new geometry.  In either case,
        // any node creating a new geometry will allocate a class route.
        // When the synchronization is completed (barrier or uebarrier), it
        // will chain into an allreduce operation, which performs the allocation
        // of the classroute.  When the classroute has been allocated,
        // The "done" event is delivered to the user.
        MPIGeometry      *bargeom = (MPIGeometry*)parent;
        PAMI::Context    *ctxt    = (PAMI::Context *)context;

        if(new_geometry)
          {
            pami_algorithm_t  alg;
            pami_metadata_t   md;
            pami_result_t     rc;
            new_geometry->algorithms_info(PAMI_XFER_ALLREDUCE,
                                          &alg,
                                          &md,
                                          1,
                                          NULL,
                                          NULL,
                                          0,
                                          ctxt->getId());
            Geometry::Algorithm<MPIGeometry> *ar_algo = (Geometry::Algorithm<MPIGeometry> *)alg;
            PostedClassRoute<MPIGeometry> *cr[2];
            rc = __global.heap_mm->memalign((void **)&cr[0], 0, sizeof(PostedClassRoute<MPIGeometry>));
            PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc PostedClassRoute<MPIGeometry>");
            rc = __global.heap_mm->memalign((void **)&cr[1], 0, sizeof(PostedClassRoute<MPIGeometry>));
            PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc PostedClassRoute<MPIGeometry>");
            to_reduce[0] = 0;
            to_reduce[1] = 0;
            new(cr[0])PostedClassRoute<MPIGeometry>(ctxt,ar_algo,new_geometry,to_reduce,
                                                     to_reduce_count,PostedClassRoute<MPIGeometry>::cr_func,
                                                     cr[0],PostedClassRoute<MPIGeometry>::phase_done,cr[1]);
            new(cr[1])PostedClassRoute<MPIGeometry>(ctxt,ar_algo,new_geometry,to_reduce,
                                                     to_reduce_count,PostedClassRoute<MPIGeometry>::cr_func2,
                                                     cr[1],fn,cookie, true);
            if(bargeom)
              bargeom->default_barrier(PostedClassRoute<MPIGeometry>::create_classroute, cr[0], ctxt->getId(), context);
            else
              new_geometry->ue_barrier(PostedClassRoute<MPIGeometry>::create_classroute, cr[0], ctxt->getId(), context);
          }
        else
          {
            if(bargeom)
              bargeom->default_barrier(fn, cookie, ctxt->getId(), context);
            else
              {
                // Null parent and null new geometry?  Why are you here?
                return PAMI_INVAL;
              }
          }
        return PAMI_SUCCESS;
      }


    inline pami_result_t geometry_query_impl (pami_geometry_t        geometry,
					      pami_configuration_t   configuration[],
					      size_t                 num_configs)
      {
	return PAMI_UNIMPL;
      }

    inline pami_result_t geometry_update_impl (pami_geometry_t        geometry,
					       pami_configuration_t   configuration[],
					       size_t                 num_configs,
					       pami_context_t         context,
					       pami_event_function    fn,
					       void                 * cookie)
      {
	return PAMI_UNIMPL;
      }


    inline pami_result_t geometry_algorithms_num_impl (pami_geometry_t geometry,
						       pami_xfer_type_t colltype,
						       size_t *lists_lengths)
    {
      MPIGeometry *_geometry = (MPIGeometry*) geometry;
      return _geometry->algorithms_num(colltype, lists_lengths, 0);
    }

    inline pami_result_t geometry_algorithms_info_impl (pami_geometry_t geometry,
							pami_xfer_type_t colltype,
							pami_algorithm_t  *algs0,
							pami_metadata_t   *mdata0,
							size_t               num0,
							pami_algorithm_t  *algs1,
							pami_metadata_t   *mdata1,
							size_t               num1)
      {
        MPIGeometry *_geometry = (MPIGeometry*) geometry;
        return _geometry->algorithms_info(colltype,
                                          algs0,
                                          mdata0,
                                          num0,
                                          algs1,
                                          mdata1,
                                          num1,
                                          0);
      }


    inline pami_result_t geometry_destroy_impl (pami_geometry_t geometry)
      {
        PAMI_abort();
        return PAMI_UNIMPL;
      }
    inline pami_geometry_t mapidtogeometry_impl (int comm)
      {
        pami_geometry_t g = _geometry_map[comm];
        return g;
      }
    inline void registerUnexpBarrier_impl (unsigned     comm,
                                           pami_quad_t &info,
                                           unsigned     peer,
                                           unsigned     algorithm)
      {
        Geometry::UnexpBarrierQueueElement *ueb =
          (Geometry::UnexpBarrierQueueElement *) _ueb_allocator.allocateObject();
        new (ueb) Geometry::UnexpBarrierQueueElement (comm, info, peer, algorithm);
        _ueb_queue.pushTail(ueb);
      }

    inline double wtime_impl ()
      {
        return __global.time.time();
      }

    inline unsigned long long wtimebase_impl ()
      {
        return __global.time.timebase();
      }

    

  protected:

    inline pami_client_t getClient () const
      {
        return _client;
      }

  private:
    pami_client_t              _client;
    size_t                     _clientid;
    size_t                     _references;
    size_t                     _ncontexts;
    PAMI::Context             *_contexts;
    PAMI::PlatformDeviceList   _platdevs;
    char                       _name[256];
    int                        _myrank;
    int                        _mysize;
    MPIGeometry               *_world_geometry;
    pami_geometry_range_t      _world_range;
    std::map<unsigned, pami_geometry_t> _geometry_map;
    Memory::GenMemoryManager      _mm;
    //  Unexpected Barrier allocator
    MemoryAllocator <sizeof(PAMI::Geometry::UnexpBarrierQueueElement), 16> _ueb_allocator;

    //  Unexpected Barrier match queue
    MatchQueue<>                                                           _ueb_queue;

    
    inline void initializeMemoryManager ()
      {
        char   shmemfile[PAMI::Memory::MMKEYSIZE];
        size_t bytes     = 1024*1024;
        size_t pagesize  = 4096;
	char *env = getenv("PAMI_CLIENT_SHMEMSIZE");
	if (env) {
		bytes = strtoull(env, NULL, 0) * 1024 * 1024;
	}

        snprintf (shmemfile, sizeof(shmemfile) - 1, "/pami-client-%s", _name);
        // Round up to the page size
        size_t size = (bytes + pagesize - 1) & ~(pagesize - 1);
	_mm.init(__global.shared_mm, size, 1, 1, 0, shmemfile);
        return;
      }

  }; // end class PAMI::Client
}; // end namespace PAMI


#endif
// __pami_mpi_mpiclient_h__
