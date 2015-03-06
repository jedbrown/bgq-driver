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
/// \file common/bgq/Client.h
/// \brief PAMI client interface specific for the Blue Gene\Q platform.
///
#ifndef __common_bgq_Client_h__
#define __common_bgq_Client_h__

#include <stdlib.h>

#include "common/ClientInterface.h"

#include "Context.h"
#include "Memregion.h"
#include <errno.h>

#ifdef USE_COMMTHREADS
  #include "components/devices/bgq/commthread/CommThreadFactory.h"
#endif // USE_COMMTHREADS

#include "util/trace.h"
#undef  DO_TRACE_ENTEREXIT
#define DO_TRACE_ENTEREXIT 0
#undef  DO_TRACE_DEBUG
#define DO_TRACE_DEBUG     0

#include "algorithms/geometry/GeometryOptimizer.h"
#include "common/bgq/ResourceManager.h"
extern PAMI::ResourceManager __pamiRM;

namespace PAMI
{
  class MultiepCookie
  {
  public:
    MultiepCookie(BGQGeometry*   geometry,
                  pami_context_t master_context,
                  pami_context_t cb_context,
                  pami_event_function cb_done,
                  void*          cookie):
    _geometry(geometry),
    _master_context(master_context),
    _cb_context(cb_context),
    _cb_done(cb_done),
    _cookie(cookie)
    {
    }

    static pami_result_t user_done(pami_context_t context, void *cookie)
    {
      TRACE_FN_ENTER();
//      PAMI::Context *myContext;
      MultiepCookie                *myCookie    = (MultiepCookie*) cookie;
      pami_event_function      u_done = myCookie->getUserDone();
      void                    *u_cookie  = myCookie->getUserCookie();
      PAMI_assert_debug(u_done);
      TRACE_FORMAT("<%p>MultiepCookie context %p, cookie %p, u_done %p, u_cookie %p",cookie, context, cookie, u_done, u_cookie);
      if(u_done)
        u_done(context, u_cookie, PAMI_SUCCESS);
      __global.heap_mm->free((void *)myCookie);              //free cookie
      TRACE_FN_EXIT();
      return PAMI_SUCCESS;
    }
    static void barrier_done(pami_context_t context, void *cookie, pami_result_t result)
    {
      TRACE_FN_ENTER();
      PAMI::Context *myContext   = (PAMI::Context *)context;
      MultiepCookie *myCookie    = (MultiepCookie*) cookie;
      PAMI::Context *masterContext = (PAMI::Context *) myCookie->getMasterContext();
      TRACE_FORMAT("<%p>MultiepCookie Context %p/%zu, Master context %p/%zu, cookie %p",cookie, myContext, myContext->getId(), masterContext, masterContext->getId(),cookie);
      if(myContext == masterContext)
      {
        PAMI::Context *userContext = (PAMI::Context *)myCookie->getUserContext();
        TRACE_FORMAT("<%p>MultiepCookie User context %p/%zu",cookie,userContext,userContext->getId());
        if(myContext == userContext)
        {
          TRACE_FN_EXIT();
          user_done(context,cookie);
          return;
        }
        else // post to user's context to call user_done
          userContext->post(&myCookie->_work,
                            user_done,
                            cookie);
      }
      else
        __global.heap_mm->free((void*)cookie);              //free cookie
      TRACE_FN_EXIT();
      return;
    }
    static pami_result_t barrier(pami_context_t context, void *cookie)
    {
      TRACE_FN_ENTER();
      Context                      *myContext      = (Context*) context;
      MultiepCookie                *myCookie    = (MultiepCookie*) cookie;
      BGQGeometry                  *geometry =  myCookie->getGeometry();
      TRACE_FORMAT("<%p>MultiepCookie Context %p/%zu, cookie %p, geometry %p",cookie, myContext, myContext->getId(),cookie,geometry);

      geometry->processUnexpBarrier(&myContext->_ueb_queue,
                                    &myContext->_ueb_allocator);
      geometry->ue_barrier(barrier_done,
                           cookie,
                           myContext->getId(),
                           context);

      TRACE_FN_EXIT();
      return PAMI_SUCCESS;
    }
    BGQGeometry*          getGeometry()
    {
      return _geometry;
    }
    pami_context_t        getMasterContext()
    {
      return _master_context;
    }
    pami_context_t        getUserContext()
    {
      return _cb_context;
    }
    void*                 getUserCookie()
    {
      return _cookie;
    }
    pami_event_function   getUserDone()
    {
      return _cb_done;
    }
    pami_work_t*          getWork()
    {
      return &_work;
    }

  protected:
    pami_work_t              _work;           // work area for post
    BGQGeometry             *_geometry;       // new geometry
    pami_context_t           _master_context; // master context

    pami_context_t           _cb_context;     // user's context 
    pami_event_function      _cb_done;        // user's callback
    void                    *_cookie;         // user's cookie
  };

  class DestroyCookie
  {
  public:
    DestroyCookie(BGQGeometry*        geometry,
		  BGQGeometry*        world_geometry,
                  pami_context_t      master_context,
                  pami_context_t      user_context,
                  pami_event_function user_done,
                  void*               user_cookie):
    _geometry(geometry),
    _world_geometry(world_geometry),
    _master_context(master_context),
    _user_context(user_context),
    _user_done(user_done),
    _user_cookie(user_cookie)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT("<%p> Geoometry %p, master_context %p, user_context %p",this, geometry, master_context, user_context)
      TRACE_FN_EXIT();
    }

    static pami_result_t user_done(pami_context_t context, void *cookie)
    {
      TRACE_FN_ENTER();
//      PAMI::Context *myContext;
      DestroyCookie           *myCookie  =(DestroyCookie*) cookie;
      BGQGeometry             *geometry  = myCookie->getGeometry();
      BGQGeometry             *world_geometry  = myCookie->getWorldGeometry();
      pami_event_function      u_done    = myCookie->getUserDone();
      void                    *u_cookie  = myCookie->getUserCookie();
      TRACE_FORMAT("<%p> Geometry %p, context %p, cookie %p, u_done %p, u_cookie %p",cookie, geometry, context, cookie, u_done, u_cookie);

      /* Destroy and free the geometry */
      __MUGlobal.getMuRM().geomDeoptimize(geometry);
      geometry->~BGQGeometry();
      if(world_geometry != geometry)
        __global.heap_mm->free(geometry);

      /* Call user's callback */
      if(u_done)
        u_done(context, u_cookie, PAMI_SUCCESS);

      /* Free cookie */
      __global.heap_mm->free((void *)myCookie);              

      TRACE_FN_EXIT();
      return PAMI_SUCCESS;
    }
    static void barrier_done(pami_context_t context, void *cookie, pami_result_t result)
    {
      TRACE_FN_ENTER();
      PAMI::Context *myContext   = (PAMI::Context *)context;
      DestroyCookie *myCookie    = (DestroyCookie*) cookie;
      PAMI::Context *masterContext = (PAMI::Context *) myCookie->getMasterContext();
      TRACE_FORMAT("<%p> Context %p/%zu, Master context %p/%zu, cookie %p",cookie, myContext, myContext->getId(), masterContext, masterContext->getId(),cookie);
      if(myContext == masterContext)
      { // Master must post to user's context to call user_done
        PAMI::Context *userContext = (PAMI::Context *)myCookie->getUserContext();
        if(myContext == userContext)
        {  
          TRACE_FORMAT("<%p>Call user done with user context %p/%zu",cookie,userContext,userContext->getId());
          user_done((pami_context_t)userContext, (void*)cookie);
        }
        else
        {  
          TRACE_FORMAT("<%p>Post user done with user context %p/%zu",cookie,userContext,userContext->getId());
          userContext->post(&myCookie->_work,
                            user_done,
                            cookie);
        }
      }
      else
        __global.heap_mm->free((void*)myCookie);            //free cookie
      TRACE_FN_EXIT();
      return;
    }
    static pami_result_t barrier(pami_context_t context, void *cookie)
    {
      TRACE_FN_ENTER();
      Context                      *myContext      = (Context*) context;
      DestroyCookie                *myCookie    = (DestroyCookie*) cookie;
      BGQGeometry                  *geometry =  myCookie->getGeometry();
      TRACE_FORMAT("<%p> Context %p/%zu, cookie %p, geometry %p",cookie, myContext, myContext->getId(),cookie,geometry);

      geometry->default_barrier(barrier_done,
                                cookie,
                                myContext->getId(),
                                context);

      TRACE_FN_EXIT();
      return PAMI_SUCCESS;
    }
    BGQGeometry*          getGeometry()
    {
      return _geometry;
    }
    BGQGeometry*          getWorldGeometry()
    {
      return _world_geometry;
    }
    pami_context_t        getMasterContext()
    {
      return _master_context;
    }
    pami_context_t        getUserContext()
    {
      return _user_context;
    }
    void*                 getUserCookie()
    {
      return _user_cookie;
    }
    pami_event_function   getUserDone()
    {
      return _user_done;
    }
    pami_work_t*          getWork()
    {
      return &_work;
    }

  protected:
    pami_work_t              _work;           // work area for post
    BGQGeometry             *_geometry;       // geometry to destroy
    BGQGeometry             *_world_geometry; // client's world geometry
    pami_context_t           _master_context; // master context

    pami_context_t           _user_context;   // user's context 
    pami_event_function      _user_done;      // user's callback
    void                    *_user_cookie;   // user's cookie
  };
  class Client : public Interface::Client<PAMI::Client>
  {
  public:
    inline Client (const char * name, pami_result_t &result) :
    Interface::Client<PAMI::Client>(name, result),
    _client ((pami_client_t) this),
    _references (1),
    _ncontexts (0),
    _world_geometry((BGQGeometry*)_world_geometry_storage),
    _mm ()
    {
      TRACE_FN_ENTER();
      pami_result_t rc;

      // Set the client name string.
      memset ((void *)_name, 0x00, sizeof(_name));
      strncpy (_name, name, sizeof(_name) - 1);

      // "allocate" the client.  This makes sure the specified client name is one of the valid
      // names configured on PAMI_CLIENTNAMES, that the name has not already been allocated,
      // and returns the proper clientId that matches the order of the client names specified on
      // the PAMI_CLIENTNAMES env var.
      rc = __pamiRM.allocateClient( _name,
                                    &_clientid );

      TRACE_FORMAT("<%p:%zu> %s, allocate rc=%d", this, _clientid, _name, rc);

      if( rc == PAMI_SUCCESS )
      {
        // PAMI_assert(_clientid < PAMI_MAX_NUM_CLIENTS);

        // Call the MU resource manager to perform init before creating any clients
        __MUGlobal.getMuRM().initClient( _clientid );

        // Get some shared memory for this client
        initializeMemoryManager ();

        _world_range.lo = 0;
        _world_range.hi = __global.mapping.size() - 1;

        // If the glocal is a coordinate topology, use it internally for efficiency (but we still need the range)
        PAMI::Topology* coord = &__global.topology_global;
        if(coord->type() !=  PAMI_COORD_TOPOLOGY) coord = NULL;

        // Also pass the local topo for efficiency instead of recreating it internally
        new(_world_geometry_storage) BGQGeometry(_client, NULL, &__global.mapping, 0, 1, 
                                                 &_world_range, 
                                                 coord, 
                                                 &__global.topology_local, 
                                                 __global.topology_master.type()==PAMI_EMPTY_TOPOLOGY?NULL:&__global.topology_master,
                                                 &_geometry_map, 0, 1);

        // This must return immediately (must not enqueue non-blocking ops).
        // Passing a NULL context should ensure that.
        __MUGlobal.getMuRM().geomOptimize(_world_geometry, _clientid, 0, NULL, NULL, NULL);
        // Now, subsequent 'analyze' done on this geom will know that MU Coll is avail.

        result = PAMI_SUCCESS;
      }
      else
        result = rc;

      TRACE_FN_EXIT();
    }

    inline ~Client ()
    {
      if(_contexts) (void)destroyContext_impl(NULL, _ncontexts);
    }

    // Parse Client Configuration Values.  Only PAMI_CLIENT_MEMORY_OPTIMIZE or PAMI_CLIENT_NONCONTIG supported
    inline pami_result_t parseClientConfig(pami_configuration_t configuration[],
                                           size_t               num_configs,
                                           size_t               *noncontig_support,
                                           size_t               *memory_optimize)

    {
      TRACE_FN_ENTER();
      pami_result_t result = PAMI_SUCCESS;
      // Set both member data (to remember for geometry parsing) and the user's parameter
      _memory_optimize = *memory_optimize = 0;     // Memory optimization default is level 0 (off)
      _noncontig_support = *noncontig_support = 1; // Noncontig default is ON/true/1
      // Configuration values are optional.  Only PAMI_CLIENT_MEMORY_OPTIMIZE or PAMI_CLIENT_NONCONTIG supported
      // Others cause PAMI_INVAL to be returned but known values are still parsed/honored.
      if(num_configs > 0)
      {
        size_t zi;
        for(zi=0; zi<num_configs; ++zi)
        {
          if(configuration[zi].name == PAMI_CLIENT_NONCONTIG)
          {
            _noncontig_support = *noncontig_support = configuration[zi].value.intval? 1:0; // bool: 0 or 1
          }
          else if(configuration[zi].name == PAMI_CLIENT_MEMORY_OPTIMIZE)
          { 
            _memory_optimize = *memory_optimize = configuration[zi].value.intval; // size_t: level N
          }
          else // other configs not supported, return PAMI_INVAL when we're done.
          {
            TRACE_FORMAT("<%p:%zu> Unsupported config %u", this,_clientid, configuration[zi].name);
            result = PAMI_INVAL;
          }
        }
      }
#if 0 // TESTING ONLY
      // PAMI_CLIENT_MEMORY_OPTIMIZE environment variable sets the value for PAMI_CLIENT_MEMORY_OPTIMIZE.
      // 
      // Enable/disable default memory optimization level.
      // 0 -- normal (default)
      // n > 0 -- memory optimize to level n
      char* senv = getenv("PAMI_CLIENT_MEMORY_OPTIMIZE");
      TRACE_FORMAT("<%p:%zu> PAMI_CLIENT_MEMORY_OPTIMIZE %s", this, _clientid,senv);
      if (senv)
        _memory_optimize = *memory_optimize = atoi(senv);
      else ; // Use default/config value

      // PAMI_CLIENT_NONCONTIG environment variable sets the value for PAMI_CLIENT_NONCONTIG.
      // 
      // Enable/disable noncontiguous collectives
      // 0 -- no noncontiguous data support in collectives
      // 1 -- noncontiguous data support in collectives (default)
      senv = getenv("PAMI_CLIENT_NONCONTIG");
      TRACE_FORMAT("<%p:%zu> PAMI_CLIENT_NONCONTIG %s", this, _clientid,senv);
      if (senv)
        _noncontig_support = *noncontig_support = atoi(senv);
      else ; // Use default/config value
#endif 
      TRACE_FORMAT("<%p:%zu> PAMI_CLIENT_NONCONTIG %zu", this,_clientid, *noncontig_support);
      TRACE_FORMAT("<%p:%zu> PAMI_CLIENT_MEMORY_OPTIMIZE %zu", this,_clientid,*memory_optimize);
      TRACE_FN_EXIT();
      return result;
    }
    static pami_result_t generate_impl (const char * name, pami_client_t * client,
                                        pami_configuration_t   configuration[],
                                        size_t                 num_configs)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT("<%p>", client);
      pami_result_t result;

      // If a client with this name is not already initialized...
      PAMI::Client * clientp = NULL;
      result = __global.heap_mm->memalign((void **)&clientp, 16, sizeof(*clientp));
      PAMI_assertf(result == PAMI_SUCCESS, "Failed to alloc PAMI::Client"); // just return?

      memset ((void *)clientp, 0x00, sizeof(PAMI::Client));
      new (clientp) PAMI::Client (name, result);
      *client = clientp;

      BGQGeometry *world_geometry=NULL;
      clientp->geometry_world_impl((pami_geometry_t *)&world_geometry);
      size_t       noncontig_support, memory_optimize; // configuration options
      // Parse configuration values and ignore errors since it will set defaults
      clientp->parseClientConfig(configuration, num_configs, &noncontig_support, &memory_optimize);
      world_geometry->setKey(0, PAMI::Geometry::CKEY_MEMORY_OPTIMIZE, (void*)memory_optimize);
      world_geometry->setKey(0, PAMI::Geometry::CKEY_NONCONTIG_DISABLE,(void*)(noncontig_support?0:1));

      TRACE_FN_EXIT();
      return result;
    }

    static void destroy_impl (pami_client_t client)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT("<%p>", client);
      PAMI::Client *clt = (PAMI::Client *)client;

      // Deallocate this client
      pami_result_t rc;
      rc = __pamiRM.deallocateClient( clt->getClientId() );
      if( rc == PAMI_SUCCESS )
      {
        // ensure contexts are destroyed first???
        clt->~Client();
        __global.heap_mm->free((void *)client);
      }
      TRACE_FN_EXIT();
    }

    inline size_t maxContexts_impl ()
    {
      return _platdevs.maxContexts(_clientid, _mm);
    }

    inline char * getName_impl ()
    {
      return _name;
    }

    ////////////////////////////////////////////////////////////////////////////
    /// \env{pami,PAMI_CONTEXT_SHMEMSIZE}
    /// Number of bytes allocated from shared memory to every context in each
    /// client. May use the 'K' and 'k' suffix as a 1024 multiplier, or the
    /// 'M' and 'm' suffix as a 1024*1024 multiplier.
    ///
    /// \default 135K
    ////////////////////////////////////////////////////////////////////////////

    inline pami_result_t createContext_impl (pami_configuration_t   configuration[],
                                             size_t                count,
                                             pami_context_t       * context,
                                             size_t                ncontexts)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT("<%p:%zu> ncontexts %zu", this, _clientid, ncontexts);
      size_t n = ncontexts;

      if(_ncontexts != 0)
      {
        TRACE_FN_EXIT();
        return PAMI_ERROR;
      }

      if(ncontexts > maxContexts_impl())
      {
        TRACE_FORMAT(" error, ncontexts (%zu) is greater than max_contexts (%zu)", ncontexts, maxContexts_impl());
        TRACE_FN_EXIT();
        return PAMI_INVAL;
      }

      pami_result_t rc;
      rc = __global.heap_mm->memalign((void **)&_contexts, 16, sizeof(*_contexts) * n);
      PAMI_assertf(rc == PAMI_SUCCESS, "alloc failed for _contexts[%zu], errno=%d\n", n, errno);
      TRACE_FORMAT(" mm available %zu", _mm.available());

      _platdevs.generate(_clientid, n, _mm); // _mm is the client-scoped shared memory manager

      // This memset has been removed due to the amount of cycles it takes
      // on simulators.  Lower level initializers should be setting the
      // relevant fields of the context, so this memset should not be
      // needed anyway.
      //memset((void *)_contexts, 0, sizeof(PAMI::Context) * n);
      size_t bytes = 135 * 1024;

      char *env = getenv("PAMI_CONTEXT_SHMEMSIZE");
      if(env)
      {
        char *s = NULL;
        bytes = strtoull(env, &s, 0);
        if(*s == 'm' || *s == 'M') bytes *= 1024 * 1024;
        else if(*s == 'k' || *s == 'K') bytes *= 1024;
      }
      TRACE_FORMAT(" mm bytes %zu", bytes);
      char key[PAMI::Memory::MMKEYSIZE];
      sprintf(key, "/pami-clt%zd-ctx-mm", _clientid);
      rc = _xmm.init(__global.shared_mm, bytes * n, 0, 0, 0, key);
      PAMI_assertf(rc == PAMI_SUCCESS, "Failed to create \"%s\" mm for %zd bytes",
                   key, bytes * n);

      Context::shared_data_t data = {-1};
      for(size_t x = 0; x < n; ++x)
      {
        context[x] = (pami_context_t) & _contexts[x];
#ifdef USE_COMMTHREADS
        // Note, this is not inializing each comm thread but rather
        // initializing comm threads for each context. context[x] is not usable yet,
        // but it won't matter since this object can't do anything with it anyway.
        // This must initialize before the context, so that MemoryManagers are
        // setup.
        __commThreads.initContext(x, context[x]);
#endif // USE_COMMTHREADS
        new (&_contexts[x]) PAMI::Context(this->getClient(), _clientid, x, n,
                                          &_platdevs, &_xmm, bytes, _world_geometry, &_geometry_map, &data);
      }

      _ncontexts = (size_t)n;
      TRACE_FORMAT(" ncontexts %zu exit",  _ncontexts);
      TRACE_FN_EXIT();
      return PAMI_SUCCESS;
    }

    // DEPRECATED!
    inline pami_result_t destroyContext_impl (pami_context_t context)
    {
      PAMI_abortf("single context destroy not supported");
      return PAMI_ERROR;
    }
    inline pami_result_t destroyContext_impl (pami_context_t *context, size_t ncontexts)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT("<%p:%zu> ncontexts %zu", this, _clientid, ncontexts);
      PAMI_assertf(ncontexts == _ncontexts, "destroyContext(%p,%zu) called without all contexts (expected %zu contexts)", context, ncontexts, _ncontexts);
      // for (i = 0.._ncontexts) PAMI_assertf(context[i] == &_contexts[i], "...");
#ifdef USE_COMMTHREADS
      // This removes all contexts... only needs to be called once.
      __commThreads.rmContexts(_contexts, _ncontexts);
#endif // USE_COMMTHREADS
      pami_result_t res = PAMI_SUCCESS;
      size_t i;

      for(i = 0; i < _ncontexts; ++i)
      {
        if(context != NULL)
          context[i] = NULL;

        PAMI::Context * ctx = &_contexts[i];
        ctx->~Context();
        pami_result_t rc = ctx->destroy ();

        //_context_list->unlock ();
        if(rc != PAMI_SUCCESS) res = rc;
      }

      __global.heap_mm->free(_contexts);
      _contexts = NULL;
      _ncontexts = 0;
      TRACE_FN_EXIT();
      return res;
    }

    inline pami_result_t query_impl (pami_configuration_t configuration[],
                                     size_t               num_configs)
    {
      pami_result_t result = PAMI_SUCCESS;
      size_t i;

      for(i = 0; i < num_configs; i++)
      {
        switch(configuration[i].name)
        {
        case PAMI_CLIENT_NUM_CONTEXTS:
          configuration[i].value.intval = maxContexts_impl();
          break;
        case PAMI_CLIENT_CONST_CONTEXTS:
          configuration[i].value.intval = true;
          break;
        case PAMI_CLIENT_TASK_ID:
          configuration[i].value.intval = __global.mapping.task();
          break;
        case PAMI_CLIENT_NUM_TASKS:
          configuration[i].value.intval = __global.mapping.size();
          break;
        case PAMI_CLIENT_NUM_LOCAL_TASKS:
          __global.mapping.nodePeers (configuration[i].value.intval);
          break;
        case PAMI_CLIENT_LOCAL_TASKS:
          configuration[i].value.intarray = __global.mapping.getPeer2TaskArray();
          break;
        case PAMI_CLIENT_CLOCK_MHZ:
        case PAMI_CLIENT_WTIMEBASE_MHZ:
          configuration[i].value.intval = __global.time.clockMHz();
          break;
        case PAMI_CLIENT_WTICK:
          configuration[i].value.doubleval = __global.time.tick();
          break;
        case PAMI_CLIENT_PROCESSOR_NAME:
          {
            int rc;
            char* pn = __global.processor_name;
            size_t coords[6];
            size_t me = __global.mapping.task();
            __global.mapping.task2global (me, coords);
            rc = snprintf(pn, 128, "Task %zu of %zu (%zu,%zu,%zu,%zu,%zu,%zu)  %s", me, __global.mapping.size(), coords[0], coords[1], coords[2], coords[3], coords[4], coords[5], __global.mapping.getLocation());
            pn[128-1] = 0;
            configuration[i].value.chararray = pn;

            if(rc <= 0)
              result = PAMI_INVAL;
          }
          break;
        case PAMI_CLIENT_HWTHREADS_AVAILABLE:
          configuration[i].value.intval = 64 / __global.mapping.tSize();
          break;
        case PAMI_CLIENT_MEM_SIZE:
          configuration[i].value.intval = __global.personality.memSize();
          break;
        case PAMI_CLIENT_MEMREGION_SIZE:
          configuration[i].value.intval = sizeof(Memregion);
          break;
        case PAMI_CLIENT_NONCONTIG:
          { 
            // Get the key from first context keystore
            void *valP = _world_geometry->getFirstKey(PAMI::Geometry::CKEY_NONCONTIG_DISABLE);
            // If they set a value it was disabled
            configuration[i].value.intval = valP ? 0:1; 
            break;
          }
        case PAMI_CLIENT_MEMORY_OPTIMIZE:
          { 
            // Get the key from first context keystore
            void *valP = _world_geometry->getFirstKey(PAMI::Geometry::CKEY_MEMORY_OPTIMIZE);
            // If no value was set, then level = 0 (default), else level = value set
            configuration[i].value.intval = (size_t)valP ; 
            break;
          }
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





    // the friend clause is actually global, but this helps us remember why...
    //friend class PAMI::Device::Generic::Device;
    //friend class pami.cc

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

    static void _geom_newopt_start(pami_context_t context, void *cookie, pami_result_t err)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT(" cookie %p, context %p, error %u", cookie, context, err);
      PAMI_assertf(context, "Geometry create barrier callback with NULL context");

      if(err != PAMI_SUCCESS)
      {
        _geom_newopt_finish(context, cookie, err);
        TRACE_FN_EXIT();
        return;
      }

      Geometry::GeometryOptimizer<BGQGeometry> *go = (Geometry::GeometryOptimizer<BGQGeometry> *) cookie;
      BGQGeometry *gp = go->geometry();

      PAMI::Context *ctxt = (PAMI::Context *)context;
      Client *thus = (Client *)ctxt->getClient();
      pami_result_t rc = __MUGlobal.getMuRM().geomOptimize(gp, thus->_clientid,
                                                           ctxt->getId(), context, _geom_newopt_finish, cookie);

      if(rc != PAMI_SUCCESS)
      {
        _geom_newopt_finish(context, cookie, rc);
        TRACE_FN_EXIT();
        return;
      }
      TRACE_FN_EXIT();
    }

    static void _geom_opt_finish(pami_context_t context, void *cookie, pami_result_t err)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT(" cookie %p, context %p, error %u", cookie, context, err);
      BGQGeometry *gp = (BGQGeometry *)cookie;

      if(context)   // HACK! until no one calls completion with NULL context!
      {
        PAMI::Context *ctxt = (PAMI::Context *)context;

        /// \todo #warning must destroy the new geometry on error
        if(err == PAMI_SUCCESS)
        {
          Client *thus = (Client *)ctxt->getClient();

          for(size_t n = 0; n < thus->_ncontexts; n++)
          {
            thus->_contexts[n].analyze(n, gp, 1);
          }
        }
      }

      // non-fatal errors - do not destroy geometry (caller might have different plans)
      gp->rmCompletion(context, err);
      TRACE_FN_EXIT();
    }

    static void _geom_newopt_finish(pami_context_t context, void *cookie, pami_result_t err)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT(" cookie %p, context %p, error %u", cookie, context, err);
      Geometry::GeometryOptimizer<BGQGeometry> *go = (Geometry::GeometryOptimizer<BGQGeometry> *) cookie;
      BGQGeometry *gp = go->geometry();

      if(err == PAMI_SUCCESS)
      {
        gp->addCompletion();
        _geom_opt_finish(context, gp, err);
        gp->rmCompletion(context, err); // completion happens here instead of
        // inside _geom_opt_finish().
        // trivial diff, right now.
        __global.heap_mm->free(go);
        TRACE_FN_EXIT();
        return;
      }

      /// \todo #warning must destroy the new geometry on error
      // but, how to do that while we're standing on it...
      gp->rmCompletion(context, err);
      // we have to assume geomOptimize() cleaned up after itself, but
      // what else do we need to cleanup? What might the analyze phases
      // have done? Right now, nothing does proper cleanup...
      // gp->~BGQGeometry(); ???
      __global.heap_mm->free(go);
      TRACE_FN_EXIT();
    }

    // Parse Geometry Configuration Values.  Only PAMI_GEOMETRY_OPTIMIZE, PAMI_GEOMETRY_NONCONTIG, PAMI_GEOMETRY_MEMORY_OPTIMIZE supported
    inline pami_result_t parseConfig(pami_configuration_t configuration[],
                                     size_t               num_configs,
                                     size_t               *geometry_optimized,
                                     size_t               *noncontig_support,
                                     size_t               *memory_optimize)

    {
      TRACE_FN_ENTER();
      pami_result_t result = PAMI_SUCCESS;
      *memory_optimize = _memory_optimize;     // Defaults to CLIENT value
      *geometry_optimized = 0;                 // Defaults to off (?)
      *noncontig_support = _noncontig_support; // Defaults to CLIENT value
      // Configuration values are optional.  
      // Only PAMI_GEOMETRY_OPTIMIZE, PAMI_GEOMETRY_NONCONTIG, PAMI_GEOMETRY_MEMORY_OPTIMIZE supported.
      // Others cause PAMI_INVAL to be returned but known values are still parsed/honored.
      if(num_configs > 3)
      {
        TRACE_FORMAT("<%p:%zu> Unsupported num_config %zu", this,_clientid, num_configs);
        result = PAMI_INVAL;
      }
      if(num_configs > 0)
      {
        size_t zi;
        for(zi=0; zi<num_configs; ++zi)
        {
          if(configuration[zi].name == PAMI_GEOMETRY_OPTIMIZE)
          {
            *geometry_optimized = configuration[zi].value.intval;
          }
          else if(configuration[zi].name == PAMI_GEOMETRY_NONCONTIG)
          {
            *noncontig_support = configuration[zi].value.intval? 1:0; // bool: 0 or 1
          }
          else if(configuration[zi].name == PAMI_GEOMETRY_MEMORY_OPTIMIZE)
          { 
            *memory_optimize = configuration[zi].value.intval; // size_t: level N
          }
          else // other configs not supported
          {
            TRACE_FORMAT("<%p:%zu> Unsupported config %u", this,_clientid, configuration[zi].name);
            result = PAMI_INVAL;
          }
        }
      }
#if 0 // TESTING ONLY
      // PAMI_GEOMETRY_MEMORY_OPTIMIZE environment variable overrides the value for PAMI_GEOMETRY_MEMORY_OPTIMIZE.
      // 
      // Enable/disable default memory optimization level.
      // 0 -- normal (default)
      // n > 0 -- memory optimize to level n
      char* senv = getenv("PAMI_GEOMETRY_MEMORY_OPTIMIZE");
      TRACE_FORMAT("<%p:%zu> PAMI_GEOMETRY_MEMORY_OPTIMIZE %s", this, _clientid,senv);
      if (senv)
        *memory_optimize = atoi(senv);
      else ; // Use default/config value

      // PAMI_GEOMETRY_NONCONTIG environment variable overrides the value for PAMI_GEOMETRY_NONCONTIG.
      // 
      // Enable/disable noncontiguous collectives
      // 0 -- no noncontiguous data support in collectives
      // 1 -- noncontiguous data support in collectives (default)
      senv = getenv("PAMI_GEOMETRY_NONCONTIG");
      TRACE_FORMAT("<%p:%zu> PAMI_GEOMETRY_NONCONTIG %s", this, _clientid,senv);
      if (senv)
        *noncontig_support = atoi(senv);
      else ; // Use default/config value
#endif 
      TRACE_FORMAT("<%p:%zu> PAMI_GEOMETRY_OPTIMIZE %zu", this,_clientid, *geometry_optimized);
      TRACE_FORMAT("<%p:%zu> PAMI_GEOMETRY_NONCONTIG %zu", this,_clientid, *noncontig_support);
      TRACE_FORMAT("<%p:%zu> PAMI_GEOMETRY_MEMORY_OPTIMIZE %zu", this,_clientid,*memory_optimize);
      TRACE_FN_EXIT();
      return result;
    }
    inline pami_result_t geometry_create_taskrange_impl(pami_geometry_t       * geometry,
                                                        size_t                  context_offset,
                                                        pami_configuration_t   configuration[],
                                                        size_t                 num_configs,
                                                        pami_geometry_t         parent,
                                                        unsigned               id,
                                                        pami_geometry_range_t * rank_slices,
                                                        size_t                 slice_count,
                                                        pami_context_t          context,
                                                        pami_event_function     fn,
                                                        void                 * cookie)
    {
      TRACE_FN_ENTER();
      size_t         nctxt     = (PAMI_ALL_CONTEXTS == context_offset) ? _ncontexts : 1;
      size_t         start_off = (PAMI_ALL_CONTEXTS == context_offset) ? 0 : context_offset;
      size_t         ctxt_arr_sz = (PAMI_ALL_CONTEXTS == context_offset)?_ncontexts:context_offset+1;
#ifndef PAMI_ENABLE_COLLECTIVE_MULTICONTEXT
      PAMI_assertf(PAMI_ALL_CONTEXTS != context_offset,"No support for PAMI_ALL_CONTEXTS");
      PAMI_assertf(start_off == 0, "No support for context %d collectives",start_off);
#endif
      TRACE_FORMAT("<%p:%zu> geometry %p/%p, context_offset %zu, nctxt %zu, start_off %zu, ctxt_arr_sz %zu", this, _clientid, geometry, *geometry,context_offset,nctxt,start_off,ctxt_arr_sz);
      size_t       geometry_optimized, noncontig_support, memory_optimize; // configuration options
      // Parse configuration values and return error if unsupported.
      if(parseConfig(configuration, num_configs, &geometry_optimized, &noncontig_support, &memory_optimize) != PAMI_SUCCESS)
      {
        TRACE_FN_EXIT();
        return PAMI_INVAL;
      }

      BGQGeometry *new_geometry;
      BGQGeometry *bargeom = (BGQGeometry *)parent;
      PAMI::Context *ctxt = (PAMI::Context *)context;

      if(geometry != NULL)
      {
        pami_result_t rc;
        rc = __global.heap_mm->memalign((void **)&new_geometry, 0,
                                        sizeof(*new_geometry)); /// \todo use allocator
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc new_geometry");
        TRACE_FORMAT("<%p> geometry %p, sizeof geometry %zu ", this, new_geometry, sizeof(*new_geometry));
        new (new_geometry) BGQGeometry(_client,
                                       (PAMI::Geometry::Common *)parent,
                                       &__global.mapping,
                                       id,
                                       slice_count,
                                       rank_slices,
                                       &_geometry_map,
                                       context_offset, /* use parameter which may be PAMI_ALL_CONTEXTS */
                                       nctxt);

        TRACE_FORMAT(" analyze %zu - %zu/%zu geometry %p", start_off, nctxt,_ncontexts, new_geometry);
        for(size_t n = start_off; n < ctxt_arr_sz; n++)
        {
          TRACE_FORMAT(" analyze %p geometry %p", &_contexts[n], new_geometry);
          new_geometry->setKey(n, PAMI::Geometry::CKEY_MEMORY_OPTIMIZE, (void*)memory_optimize);
          new_geometry->setKey(n, PAMI::Geometry::CKEY_NONCONTIG_DISABLE,(void*)(noncontig_support?0:1));
          _contexts[n].analyze(context_offset, /* use parameter which may be PAMI_ALL_CONTEXTS */
                               new_geometry, 0);
        }
      }

      if(start_off == 0 && nctxt == 1)
      { // Only context 0, single context geometry uses optimized protocols 
        if(geometry != NULL)
        {
          *geometry = (pami_geometry_t) new_geometry;

          /// \todo  deliver completion to the appropriate context
          new_geometry->setCompletion(fn, cookie);
          new_geometry->addCompletion(); // ensure completion doesn't happen until
                                         // all have been analyzed (_geom_opt_finish).

          // Start the barrier (and then the global analyze and (maybe) the optimize ...
          start_barrier(bargeom, new_geometry,
                        ctxt->getId(), context,
                        geometry_optimized? PAMI_GEOMETRY_OPTIMIZE: (pami_attribute_name_t)-1);
          new_geometry->processUnexpBarrier(&ctxt->_ueb_queue,
                                            &ctxt->_ueb_allocator);
        }
        else if(bargeom)
        {
          // non-participant members of parent won't know if new geom exists...
          bargeom->default_barrier(fn, cookie, ctxt->getId(), context);
        }
      }
#ifdef PAMI_ENABLE_COLLECTIVE_MULTICONTEXT
      else // multicontext (endpoint) or non-0 context geometry are not optimized so just barrier on [n] contexts
      {
        if(geometry != NULL)
        {
          *geometry = (pami_geometry_t) new_geometry;
          for(ssize_t n=(ssize_t)ctxt_arr_sz-1; n>=(ssize_t)start_off; n--)
          {
            MultiepCookie *mcookie;
            pami_result_t rc; 
            rc = __global.heap_mm->memalign((void **)&mcookie,0,sizeof(MultiepCookie));
            PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc multiep cookie");
            new(mcookie) MultiepCookie(new_geometry,
                                       (pami_context_t)&_contexts[start_off], 
                                       (pami_context_t)&_contexts[n], 
                                       fn,
                                       cookie);
            pami_work_t* work = mcookie->getWork();
            TRACE_FORMAT(" post context work on context %zu(%zu,%zu), cookie %p",  n,ctxt_arr_sz,start_off, mcookie);
            _contexts[n].post(work,
                              MultiepCookie::barrier,
                              mcookie);
            //_contexts[n].advance_impl(10,rc);
          }
        }
        else if(bargeom)
        {
          // non-participant members of parent won't know if new geom exists...
          bargeom->default_barrier(fn, cookie, ctxt->getId(), context);
        }
      }
#endif

      TRACE_FORMAT(" exit geometry %p/%p",  geometry, *geometry);
      TRACE_FN_EXIT();
      return PAMI_SUCCESS;
    }


    inline pami_result_t geometry_create_topology_impl(pami_geometry_t       *geometry,
                                                       pami_configuration_t   configuration[],
                                                       size_t                 num_configs,
                                                       pami_geometry_t        parent,
                                                       unsigned               id,
                                                       pami_topology_t       *topology,
                                                       pami_context_t         context,
                                                       pami_event_function    fn,
                                                       void                  *cookie)
    {
      TRACE_FN_ENTER();
      PAMI::Context *ctxt = (PAMI::Context *)context;
      size_t         nctxt     = 0;
      size_t         start_off = 0;
      BGQGeometry *new_geometry;
      BGQGeometry *bargeom = (BGQGeometry *)parent;

      size_t       geometry_optimized, noncontig_support, memory_optimize; // configuration options
      // Parse configuration values and return error if unsupported.
      if(parseConfig(configuration, num_configs, &geometry_optimized, &noncontig_support, &memory_optimize) != PAMI_SUCCESS)
      {
        TRACE_FN_EXIT();
        return PAMI_INVAL;
      }

      if(geometry != NULL)
      {
        pami_result_t rc;
        rc = __global.heap_mm->memalign((void **)&new_geometry, 0,
                                        sizeof(*new_geometry)); /// \todo use allocator
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc new_geometry");
        TRACE_FORMAT("<%p> geometry %p, sizeof geometry %zu ", this, new_geometry, sizeof(*new_geometry));
        new (new_geometry) BGQGeometry(_client,
                                       (PAMI::Geometry::Common *)parent,
                                       &__global.mapping,
                                       id,
                                       (PAMI::Topology *)topology,
                                       &_geometry_map,
                                       PAMI_ALL_CONTEXTS,
                                       nctxt);

        PAMI::Topology *local_topology  = (PAMI::Topology *)new_geometry->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX);
        // Find endpoints that are in my task to calculate the total number of contexts participating
        // as well as the indexes of those contexts
        int index_array[MAX_CONTEXTS];
        int num_local_ep       = 0;
        int master_context_idx = -1;
        for(size_t n=0; n<local_topology->size(); n++)
        {
          pami_endpoint_t ep    = local_topology->index2Endpoint(n);
          pami_task_t     task;
          size_t          offset;
          PAMI_ENDPOINT_INFO(ep, task, offset);
          if(task == __global.mapping.task())
          {
            if(ctxt->getId() == offset)
              master_context_idx = num_local_ep;
            index_array[num_local_ep++] = offset;
          }
        }
        PAMI_assertf(master_context_idx != -1, "Fatal:  Geometry create posted context is not represented in the endpoint list");
        TRACE_FORMAT("<%p:%zu> geometry %p/%p, nctxt %zu, start_off %zu", this, _clientid, geometry, *geometry,nctxt,start_off);

        TRACE_FORMAT(" analyze %zu - %zu/%zu geometry %p", start_off, nctxt,_ncontexts, new_geometry);

        for(size_t n=0; n<(size_t)num_local_ep;n++)
        {
          size_t idx = index_array[n];
          TRACE_FORMAT("%s analyze %p geometry %p", __PRETTY_FUNCTION__, &_contexts[idx], new_geometry);
          new_geometry->setKey(idx, PAMI::Geometry::CKEY_MEMORY_OPTIMIZE, (void*)memory_optimize);
          new_geometry->setKey(idx, PAMI::Geometry::CKEY_NONCONTIG_DISABLE,(void*)(noncontig_support?0:1));
          _contexts[idx].analyze(idx, new_geometry, 0);
        }
        *geometry = (pami_geometry_t) new_geometry;
        for(size_t n=0; n<(size_t)num_local_ep;n++)
        {
          size_t idx = index_array[n];
          MultiepCookie *mcookie;
          pami_result_t rc; 
          rc = __global.heap_mm->memalign((void **)&mcookie,0,sizeof(MultiepCookie));
          PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc multiep cookie");
          new(mcookie) MultiepCookie(new_geometry,
                                     (pami_context_t)&_contexts[index_array[0]], 
                                     (pami_context_t)&_contexts[idx], 
                                     fn,
                                     cookie);
          pami_work_t* work = mcookie->getWork();
          TRACE_FORMAT(" post context work on context %zu, cookie %p",  idx, mcookie);
          _contexts[n].post(work,
                            MultiepCookie::barrier,
                            mcookie);
          //_contexts[n].advance_impl(10,rc);
        }
      }
      else if(bargeom)
      {
        // non-participant members of parent won't know if new geom exists...
        bargeom->default_barrier(fn, cookie, ctxt->getId(), context);
      }

      TRACE_FORMAT(" exit geometry %p/%p",  geometry, *geometry);
      TRACE_FN_EXIT();
      return PAMI_SUCCESS;
    }


    inline pami_result_t geometry_create_tasklist_impl(pami_geometry_t       * geometry,
                                                       size_t                  context_offset,
                                                       pami_configuration_t   configuration[],
                                                       size_t                 num_configs,
                                                       pami_geometry_t         parent,
                                                       unsigned               id,
                                                       pami_task_t           * tasks,
                                                       size_t                 task_count,
                                                       pami_context_t          context,
                                                       pami_event_function     fn,
                                                       void                 * cookie)
    {
      TRACE_FN_ENTER();
      size_t         nctxt     = (PAMI_ALL_CONTEXTS == context_offset) ? _ncontexts : 1;
      size_t         start_off = (PAMI_ALL_CONTEXTS == context_offset) ? 0 : context_offset;
      size_t         ctxt_arr_sz = (PAMI_ALL_CONTEXTS == context_offset)?_ncontexts:context_offset+1;
#ifndef PAMI_ENABLE_COLLECTIVE_MULTICONTEXT
      PAMI_assertf(PAMI_ALL_CONTEXTS != context_offset,"No support for PAMI_ALL_CONTEXTS");
      PAMI_assertf(start_off == 0, "No support for context %d collectives",start_off);
#endif
      TRACE_FORMAT("<%p:%zu> geometry %p/%p, context_offset %zu, nctxt %zu, start_off %zu, ctxt_arr_sz %zu", this, _clientid, geometry, *geometry,context_offset,nctxt,start_off,ctxt_arr_sz);
      size_t       geometry_optimized, noncontig_support, memory_optimize; // configuration options
      // Parse configuration values and return error if unsupported.
      if(parseConfig(configuration, num_configs, &geometry_optimized, &noncontig_support, &memory_optimize) != PAMI_SUCCESS)
      {
        TRACE_FN_EXIT();
        return PAMI_INVAL;
      }

      BGQGeometry *new_geometry;
      BGQGeometry *bargeom = (BGQGeometry *)parent;
      PAMI::Context *ctxt = (PAMI::Context *)context;

      if(geometry != NULL)
      {
        pami_result_t rc;
        rc = __global.heap_mm->memalign((void **)&new_geometry, 0,
                                        sizeof(*new_geometry)); /// \todo use allocator
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc new_geometry");
        TRACE_FORMAT("<%p> geometry %p, sizeof geometry %zu ", this, new_geometry, sizeof(*new_geometry));
        new (new_geometry) BGQGeometry(_client,
                                       (PAMI::Geometry::Common *)parent,
                                       &__global.mapping,
                                       id,
                                       task_count,
                                       tasks,
                                       &_geometry_map,
                                       context_offset, /* use parameter which may be PAMI_ALL_CONTEXTS */
                                       nctxt);

        TRACE_FORMAT(" analyze %zu - %zu/%zu geometry %p", start_off, nctxt,_ncontexts, new_geometry);
        for(size_t n = start_off; n < ctxt_arr_sz; n++)
        {
          TRACE_FORMAT(" analyze %p geometry %p", &_contexts[n], new_geometry);
          new_geometry->setKey(n, PAMI::Geometry::CKEY_MEMORY_OPTIMIZE, (void*)memory_optimize);
          new_geometry->setKey(n, PAMI::Geometry::CKEY_NONCONTIG_DISABLE,(void*)(noncontig_support?0:1));
          _contexts[n].analyze(context_offset, /* use parameter which may be PAMI_ALL_CONTEXTS */
                               new_geometry, 0);
        }
      }

      if(start_off == 0 && nctxt == 1)
      { // Only context 0, single context geometry uses optimized protocols 
        if(geometry != NULL)
        {
          *geometry = (pami_geometry_t) new_geometry;

          /// \todo  deliver completion to the appropriate context
          new_geometry->setCompletion(fn, cookie);
          new_geometry->addCompletion(); // ensure completion doesn't happen until
                                         // all have been analyzed (_geom_opt_finish).

          // Start the barrier (and then the global analyze and (maybe) the optimize ...
          start_barrier(bargeom, new_geometry,
                        ctxt->getId(), context,
                        geometry_optimized? PAMI_GEOMETRY_OPTIMIZE: (pami_attribute_name_t)-1);
          new_geometry->processUnexpBarrier(&ctxt->_ueb_queue,
                                            &ctxt->_ueb_allocator);
        }
        else if(bargeom)
        {
          // non-participant members of parent won't know if new geom exists...
          bargeom->default_barrier(fn, cookie, ctxt->getId(), context);
        }
      }
#ifdef PAMI_ENABLE_COLLECTIVE_MULTICONTEXT
      else // multicontext (endpoint) or non-0 context geometry are not optimized so just barrier on [n] contexts
      {
        if(geometry != NULL)
        {
          *geometry = (pami_geometry_t) new_geometry;
          for(ssize_t n=(ssize_t)ctxt_arr_sz-1; n>=(ssize_t)start_off; n--)
          {
            MultiepCookie *mcookie;
            pami_result_t rc; 
            rc = __global.heap_mm->memalign((void **)&mcookie,0,sizeof(MultiepCookie));
            PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc multiep cookie");
            new(mcookie) MultiepCookie(new_geometry,
                                       (pami_context_t)&_contexts[start_off], 
                                       (pami_context_t)&_contexts[n], 
                                       fn,
                                       cookie);
            pami_work_t* work = mcookie->getWork();
            TRACE_FORMAT(" post context work on context %zu(%zu,%zu), cookie %p",  n,ctxt_arr_sz,start_off, mcookie);
            _contexts[n].post(work,
                              MultiepCookie::barrier,
                              mcookie);
            //_contexts[n].advance_impl(10,rc);
          }
        }
        else if(bargeom)
        {
          // non-participant members of parent won't know if new geom exists...
          bargeom->default_barrier(fn, cookie, ctxt->getId(), context);
        }
      }
#endif

      TRACE_FORMAT(" exit geometry %p/%p",  geometry, *geometry);
      TRACE_FN_EXIT();
      return PAMI_SUCCESS;
    }

    inline pami_result_t geometry_create_endpointlist_impl(pami_geometry_t       * geometry,
                                                           pami_configuration_t    configuration[],
                                                           size_t                  num_configs,
                                                           unsigned                id,
                                                           pami_endpoint_t       * endpoints,
                                                           size_t                  endpoint_count,
                                                           pami_context_t          context,
                                                           pami_event_function     fn,
                                                           void                  * cookie)
    {
      TRACE_FN_ENTER();
      PAMI::Context *ctxt = (PAMI::Context *)context;
      size_t         nctxt     = 0;
      size_t         start_off = 0;
      BGQGeometry *new_geometry;

      if(geometry != NULL)
      {
        pami_result_t rc;
        rc = __global.heap_mm->memalign((void **)&new_geometry, 0,
                                        sizeof(*new_geometry)); /// \todo use allocator
        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc new_geometry");
        TRACE_FORMAT("<%p> geometry %p, sizeof geometry %zu ", this, new_geometry, sizeof(*new_geometry));
        new (new_geometry) BGQGeometry(_client,
                                       (BGQGeometry*)PAMI_GEOMETRY_NULL,
                                       &__global.mapping,
                                       id,
                                       endpoint_count,
                                       endpoints,
                                       &_geometry_map,
                                       true);

        PAMI::Topology *local_topology  = (PAMI::Topology *)new_geometry->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX);
        // Find endpoints that are in my task to calculate the total number of contexts participating
        // as well as the indexes of those contexts
        int index_array[MAX_CONTEXTS];
        int num_local_ep       = 0;
        int master_context_idx = -1;
        for(size_t n=0; n<local_topology->size(); n++)
        {
          pami_endpoint_t ep    = local_topology->index2Endpoint(n);
          pami_task_t     task;
          size_t          offset;
          PAMI_ENDPOINT_INFO(ep, task, offset);
          if(task == __global.mapping.task())
          {
            if(ctxt->getId() == offset)
              master_context_idx = num_local_ep;
            index_array[num_local_ep++] = offset;
          }
        }
        PAMI_assertf(master_context_idx != -1, "Fatal:  Geometry create posted context is not represented in the endpoint list");
        TRACE_FORMAT("<%p:%zu> geometry %p/%p, nctxt %zu, start_off %zu", this, _clientid, geometry, *geometry,nctxt,start_off);
        size_t       geometry_optimized, noncontig_support, memory_optimize; // configuration options
        // Parse configuration values and return error if unsupported.
        if(parseConfig(configuration, num_configs, &geometry_optimized, &noncontig_support, &memory_optimize) != PAMI_SUCCESS)
        {
          TRACE_FN_EXIT();
          return PAMI_INVAL;
        }

        TRACE_FORMAT(" analyze %zu - %zu/%zu geometry %p", start_off, nctxt,_ncontexts, new_geometry);

        for(size_t n=0; n<(size_t)num_local_ep;n++)
        {
          size_t idx = index_array[n];
          TRACE_FORMAT("%s analyze %p geometry %p", __PRETTY_FUNCTION__, &_contexts[idx], new_geometry);
          new_geometry->setKey(idx, PAMI::Geometry::CKEY_MEMORY_OPTIMIZE, (void*)memory_optimize);
          new_geometry->setKey(idx, PAMI::Geometry::CKEY_NONCONTIG_DISABLE,(void*)(noncontig_support?0:1));
          _contexts[idx].analyze(idx, new_geometry, 0);
        }
        *geometry = (pami_geometry_t) new_geometry;
        for(size_t n=0; n<(size_t)num_local_ep;n++)
        {
          size_t idx = index_array[n];
          MultiepCookie *mcookie;
          pami_result_t rc; 
          rc = __global.heap_mm->memalign((void **)&mcookie,0,sizeof(MultiepCookie));
          PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc multiep cookie");
          new(mcookie) MultiepCookie(new_geometry,
                                     (pami_context_t)&_contexts[index_array[0]], 
                                     (pami_context_t)&_contexts[idx], 
                                     fn,
                                     cookie);
          pami_work_t* work = mcookie->getWork();
          TRACE_FORMAT(" post context work on context %zu, cookie %p",  idx, mcookie);
          _contexts[idx].post(work,
                              MultiepCookie::barrier,
                              mcookie);
          //_contexts[n].advance_impl(10,rc);
        }
      }

      TRACE_FORMAT(" exit geometry %p/%p",  geometry, *geometry);
      TRACE_FN_EXIT();
      return PAMI_SUCCESS;
    }

    inline pami_result_t geometry_query_impl(pami_geometry_t geometry,
                                             pami_configuration_t configuration[],
                                             size_t num_configs)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT("<%p:%zu>", this, _clientid);

      BGQGeometry *geom = (BGQGeometry *)geometry;
      // Only PAMI_GEOMETRY_OPTIMIZE or PAMI_GEOMETRY_NONCONTIG supported
      if(num_configs > 3)
      {
        TRACE_FN_EXIT();
        return PAMI_INVAL;
      }
      if(num_configs > 0)
      {
        size_t zi;
        for(zi=0; zi<num_configs; ++zi)
        {
          if(configuration[zi].name == PAMI_GEOMETRY_OPTIMIZE)
          {
            // is it stored in geometry? or just implied by key/vals?
            void *v1 = geom->getFirstKey( PAMI::Geometry::CKEY_MCAST_CLASSROUTEID);
            void *v2 = geom->getFirstKey( PAMI::Geometry::CKEY_MSYNC_CLASSROUTEID);
            int b1 = (v1 != PAMI_CR_CKEY_FAIL ? (int)((uintptr_t)v1 & 0x0ff) : 0);
            int b2 = (v2 != PAMI_CR_CKEY_FAIL ? (int)((uintptr_t)v2 & 0x0ff) : 0);
            configuration[zi].value.intval = b1 | (b2 << 8);
          }
          else if(configuration[zi].name == PAMI_GEOMETRY_NONCONTIG)
          { 
            // Get the key from first context keystore
            void *valP = geom->getFirstKey(PAMI::Geometry::CKEY_NONCONTIG_DISABLE);
            // If they set a value it was disabled
            configuration[zi].value.intval = (valP?0:1);
          }
          else if(configuration[zi].name == PAMI_GEOMETRY_MEMORY_OPTIMIZE)
          { 
            // Arbitrarily use context id 0 to get the key.  They should all be the same
            // and 0 key store always exists.
            void *valP = geom->getFirstKey(PAMI::Geometry::CKEY_MEMORY_OPTIMIZE);
            // If no value was set, then level = 0 (default), else level = value set
            configuration[zi].value.intval = (valP == PAMI_CR_CKEY_FAIL)? 0: (size_t)valP ; 
          }
          else // other configs not supported
          {
            TRACE_FN_EXIT();
            return PAMI_INVAL;
          }
        }
      }
      TRACE_FN_EXIT();
      return PAMI_SUCCESS;
    }

    inline pami_result_t geometry_update_impl(pami_geometry_t geometry,
                                              pami_configuration_t configuration[],
                                              size_t num_configs,
                                              pami_context_t context,
                                              pami_event_function fn,
                                              void *cookie)
    {
      pami_result_t rc = PAMI_SUCCESS;
      TRACE_FN_ENTER();
      TRACE_FORMAT("<%p:%zu>", this, _clientid);
      // Only PAMI_GEOMETRY_OPTIMIZE or PAMI_GEOMETRY_NONCONTIG supported
      if(num_configs > 3)
      {
        TRACE_FN_EXIT();
        return PAMI_INVAL;
      }
      if(num_configs > 0)
      {
        BGQGeometry *geom = (BGQGeometry *)geometry;
        size_t zi;
        for(zi=0; zi<num_configs; ++zi)
        {
          if(configuration[zi].name == PAMI_GEOMETRY_OPTIMIZE)
          {
            BGQGeometry *gp = (BGQGeometry *)geometry;
            TRACE_FORMAT("<%p:%zu> PAMI_GEOMETRY_OPTIMIZE %zu", this, _clientid, configuration[zi].value.intval);

            if(configuration[zi].value.intval != 0) // Optimize
            {
              PAMI::Context *ctxt = (PAMI::Context *)context;
              // First remove optimized algorithms in case it was previously optimized
              for(size_t n = 0; n < _ncontexts; n++)
              {
                _contexts[n].analyze(n, (BGQGeometry *)geometry, -1);
              }

              gp->setCompletion(fn, cookie);
              gp->addCompletion();        // ensure completion doesn't happen until
              //  all have been analyzed (_geom_opt_finish).
              /// \todo should this be _geom_newopt_finish?
              rc = __MUGlobal.getMuRM().geomOptimize(gp, _clientid,
                                                     ctxt->getId(), context, _geom_opt_finish, (void *)gp);
            }
            else // De-optimize
            {
              rc = __MUGlobal.getMuRM().geomDeoptimize(gp);

              if(rc == PAMI_SUCCESS)
              {
                for(size_t n = 0; n < _ncontexts; n++)
                {
                  _contexts[n].analyze(n, (BGQGeometry *)geometry, -1);
                }
              }
            }
          }
          else if(configuration[zi].name == PAMI_GEOMETRY_NONCONTIG)
          { 
            TRACE_FORMAT("<%p:%zu> PAMI_GEOMETRY_NONCONTIG %zu", this, _clientid, configuration[zi].value.intval);
            // Set the new key in all contexts used by the geometry
            geom->setKeyAll(PAMI::Geometry::CKEY_NONCONTIG_DISABLE,(void*)(configuration[zi].value.intval?0:1));
          }
          else if(configuration[zi].name == PAMI_GEOMETRY_MEMORY_OPTIMIZE)
          { 
            TRACE_FORMAT("<%p:%zu> PAMI_GEOMETRY_MEMORY_OPTIMIZE %zu", this, _clientid, configuration[zi].value.intval);
            // If the user picks the one 'special value' level, change it
            if(configuration[zi].value.intval == (size_t)PAMI_CR_CKEY_FAIL) configuration[zi].value.intval = 999; 
            // Set the new key in all contexts used by the geometry
            geom->setKeyAll(PAMI::Geometry::CKEY_MEMORY_OPTIMIZE,(void*)(configuration[zi].value.intval));
          }
          else // other configs not supported
          {
            TRACE_FN_EXIT();
            return PAMI_INVAL;
          }
        }
      }

      if(fn)
      {
        fn(context, cookie, rc);
      }
      TRACE_FN_EXIT();
      return rc;
    }

    inline pami_result_t geometry_algorithms_num_impl (pami_geometry_t geometry,
                                                       pami_xfer_type_t colltype,
                                                       size_t *lists_lengths)
    {
      BGQGeometry *_geometry = (BGQGeometry*) geometry;
      return _geometry->algorithms_num(colltype, lists_lengths);
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
      BGQGeometry *_geometry = (BGQGeometry*) geometry;
      return _geometry->algorithms_info(colltype,
                                        algs0,
                                        mdata0,
                                        num0,
                                        algs1,
                                        mdata1,
                                        num1);
    }



    inline pami_result_t geometry_destroy_impl (pami_geometry_t      geometry,
                                                pami_context_t       context,
                                                pami_event_function  fn,
                                                void                *cookie)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT("<%p:%zu> geometry %p ", this, _clientid, (BGQGeometry *)geometry);

      /* We simply need to barrier before we destroy anything (and call the user's callback) so
         that we know all resources are freed (like GI class routes across nodes) */
      PAMI::Topology   *local_topology = (PAMI::Topology *)((BGQGeometry*)geometry)->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX);

      int master_context_idx = -1; /* only one master context handles the user callback */

      for(size_t n=0; n<local_topology->size(); n++)
      {
        pami_endpoint_t ep    = local_topology->index2Endpoint(n);
        pami_task_t     task;
        size_t          offset;
        PAMI_ENDPOINT_INFO(ep, task, offset);

        if(task == __global.mapping.task()) 
        {
          TRACE_FORMAT("Found endpoint %u, task %u on context %zu",ep,task,offset);
          if(master_context_idx == -1) /* Use first context as master... */
            master_context_idx = offset;
          if(context == (pami_context_t)&_contexts[offset])  /* unless/until we find the user's context */
          {  
            TRACE_FORMAT("Found context _context[%zu]=%p",offset,&_contexts[offset]);
            master_context_idx = offset;
            break;
          }
        }
      }
      for(size_t n=0; n<local_topology->size(); n++)
      {
        pami_endpoint_t ep    = local_topology->index2Endpoint(n);
        pami_task_t     task;
        size_t          offset;
        PAMI_ENDPOINT_INFO(ep, task, offset);

        /* start the barrier on every local (this task) endpoint using that endpoint's context */
        if(task == __global.mapping.task()) 
        {
          TRACE_FORMAT("Found endpoint %u, task %u on context %zu",ep,task,offset);
          DestroyCookie *mcookie;
          pami_result_t rc; 
          rc = __global.heap_mm->memalign((void **)&mcookie,0,sizeof(DestroyCookie));
          PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc destroy cookie");
          new(mcookie) DestroyCookie((BGQGeometry *)geometry, /* geometry to destroy */
				     (BGQGeometry *)_world_geometry, /* client's world geometry */
                                     (pami_context_t)&_contexts[master_context_idx], /* master context */
                                     (pami_context_t)context, /* user's context for callback */
                                     fn,                      /* user's fn for callback */
                                     cookie);                 /* user's cookie for callback */
          pami_work_t* work = mcookie->getWork();
          TRACE_FORMAT("post barrier work on context %zu, cookie %p",  offset, mcookie);
          _contexts[offset].post(work,
                                 DestroyCookie::barrier,
                                 mcookie);
          /*_contexts[offset].advance_impl(10,rc); */
        }
      }
      TRACE_FN_EXIT();
      return PAMI_SUCCESS;
    }

    inline pami_geometry_t mapidtogeometry_impl (int comm)
    {
      TRACE_FN_ENTER();
      pami_geometry_t g = _geometry_map[comm];
      TRACE_FORMAT("<%p>%s", g, __PRETTY_FUNCTION__);
      TRACE_FN_EXIT();
      return g;
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

    pami_client_t _client;
    size_t _clientid;

    size_t       _references;
    size_t       _ncontexts;
    PAMI::Context *_contexts;
    PAMI::PlatformDeviceList _platdevs;
    char         _name[256];
    BGQGeometry                  *_world_geometry;
    uint8_t                       _world_geometry_storage[sizeof(BGQGeometry)];
    pami_geometry_range_t         _world_range;
    // This is a map of geometries to geometry id's
    std::map<unsigned, pami_geometry_t>          _geometry_map;


    Memory::GenMemoryManager _mm;
    Memory::GenMemoryManager _xmm; // used to fill context mm, from single OS alloc.
    //  Unexpected Barrier allocator
    unsigned                _memory_optimize; // env value for memory optimized
    unsigned                _noncontig_support; // env value for noncontiguous collectives
    ////////////////////////////////////////////////////////////////////////////
    /// \env{pami,PAMI_CLIENT_SHMEMSIZE}
    /// Number of bytes allocated from shared memory to each client. May use
    /// the 'K' and 'k' suffix as a 1024 multiplier, or the 'M' and 'm' suffix
    /// as a 1024*1024 multiplier.
    ///
    /// \default 4M if more than one task is on the node; otherwise 0
    ////////////////////////////////////////////////////////////////////////////
    inline void initializeMemoryManager ()
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT("<%p:%zu>", this, _clientid);
      char   shmemfile[PAMI::Memory::MMKEYSIZE];
#if 0
      size_t num_ctx = __MUGlobal.getMuRM().getPerProcessMaxPamiResources();
      // may need to factor in others such as shmem?
#else
      size_t num_ctx = MAX_CONTEXTS;
#endif
      size_t bytes = 0;
      if(__global.useshmem())
      {
        bytes = (64*1024) * num_ctx; // 64k for each context in the client
      }

      char *env = getenv("PAMI_CLIENT_SHMEMSIZE");
      if(env)
      {
        char *s = NULL;
        bytes = strtoull(env, &s, 0);
        if(*s == 'm' || *s == 'M') bytes *= 1024 * 1024;
        else if(*s == 'k' || *s == 'K') bytes *= 1024;
      }

      snprintf (shmemfile, sizeof(shmemfile) - 1, "/pami-client-%s", _name);

      // Round up to the page size
      //size_t size = (bytes + pagesize - 1) & ~(pagesize - 1);

      //fprintf (stderr, "initialize client shmem to %zu bytesg", bytes);
      pami_result_t rc;
      rc = _mm.init(__global.shared_mm, bytes, 1, 1, 0, shmemfile);
      PAMI_assert_alwaysf(rc == PAMI_SUCCESS, "Failed to create \"%s\" mm for %zd bytes",
                          shmemfile, bytes);
      TRACE_FN_EXIT();
      return;
    }

    void start_barrier(BGQGeometry          *bargeom,
                       BGQGeometry          *new_geometry,
                       size_t                context_id,
                       pami_context_t        context,
                       pami_attribute_name_t optimize)
    {
      TRACE_FN_ENTER();
      PAMI_assertf(context_id == 0,"Only optimize context 0\n");
      // Assume no optimize needed
      pami_event_function done_fn = _geom_newopt_finish;

      pami_algorithm_t  alg   = PAMI_ALGORITHM_NULL;
      pami_metadata_t   mdata;
      memset(&mdata, 0, sizeof(mdata));
      if(new_geometry->size() == 1)
      {  
        new_geometry->algorithms_info(PAMI_XFER_ALLREDUCE,
                                      &alg, /* making an assumption here about OneTask collectives */
                                      &mdata,
                                      1,
                                      NULL,
                                      NULL,
                                      0);
      }
      else
      {  
        new_geometry->algorithms_info(PAMI_XFER_ALLREDUCE,
                                      NULL,
                                      NULL,
                                      0,
                                      &alg, /* making an assumption here about short working now */
                                      &mdata,
                                      1);
        // > 1 size can be optimized
        if(optimize == PAMI_GEOMETRY_OPTIMIZE)
          done_fn = _geom_newopt_start;
      }

      TRACE_FORMAT("<%p> geometry %p, algorithm %s", this,new_geometry, mdata.name);
      Geometry::Algorithm<BGQGeometry> *ar_algo = &(*((std::map<size_t,Geometry::Algorithm<BGQGeometry> > *)alg))[context_id];


      Geometry::GeometryOptimizer<BGQGeometry> *go = NULL;
      int rc  = 0;
      rc = __global.heap_mm->memalign((void **)&go, 0, sizeof(*go));
      PAMI_assertf(rc == PAMI_SUCCESS, "alloc failed for GlobalAnalyzer<BGQGeometry> %zd", sizeof(*go));

      new (go) Geometry::GeometryOptimizer<BGQGeometry>(context,
                                                        new_geometry,
                                                        ar_algo,
                                                        done_fn,
                                                        (void*)go);

//      for(size_t i = 0; i < _ncontexts; ++i)
//      {
//        _contexts[i].registerWithOptimizer(go);
//      }
      _contexts[context_id].registerWithOptimizer(go);

      TRACE_FORMAT("<%p:%zu>bargeom->size() %zu, new_geometry->size() %zu, context_id %zu, context %p  %s", this, _clientid,bargeom? bargeom->size():(size_t)-1,new_geometry?new_geometry->size():(size_t)-1, context_id, context, optimize == PAMI_GEOMETRY_OPTIMIZE? "Optimized":" ");

      if(bargeom)
        if(bargeom->size() == 1)
          Geometry::GeometryOptimizer<BGQGeometry>::optimizer_start(context, (void *)go, PAMI_SUCCESS);
        else
          bargeom->default_barrier(Geometry::GeometryOptimizer<BGQGeometry>::optimizer_start, (void *)go, context_id, context);
      else if(new_geometry->size() == 1)
        Geometry::GeometryOptimizer<BGQGeometry>::optimizer_start(context, (void *)go, PAMI_SUCCESS);
      else
        new_geometry->ue_barrier(Geometry::GeometryOptimizer<BGQGeometry>::optimizer_start, (void *)go, context_id, context);
      TRACE_FN_EXIT();
    }

//  void start_barrier_no_opt(BGQGeometry          *bargeom,
//                            BGQGeometry          *new_geometry,
//                            size_t                context_id,
//                            pami_context_t        context)
//  {
//    TRACE_FN_ENTER();
//    TRACE_FORMAT("<%p>BGQ::Client::start_barrier_no_opt() algorithm %s", this);
//
//    pami_event_function done_fn = _geom_newopt_finish;
//
//
//    if (bargeom)
//      if (bargeom->size() == 1)
//        Geometry::GeometryOptimizer<BGQGeometry>::optimizer_start(context, (void *)go, PAMI_SUCCESS);
//      else
//        bargeom->default_barrier(Geometry::GeometryOptimizer<BGQGeometry>::optimizer_start, (void *)go, context_id, context);
//    else
//      if (new_geometry->size() == 1)
//      Geometry::GeometryOptimizer<BGQGeometry>::optimizer_start(context, (void *)go, PAMI_SUCCESS);
//    else
//      new_geometry->ue_barrier(Geometry::GeometryOptimizer<BGQGeometry>::optimizer_start, (void *)go, context_id, context);
//    TRACE_FN_EXIT();
//  }

  }; // end class PAMI::Client

}; // end namespace PAMI

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#endif // __components_client_bgq_bgqclient_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
