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
 * \file api/c/pami.cc
 * \brief PAMI C interface common implementation
 */
#ifndef PAMI_LAPI_IMPL
#include "config.h"
#endif //PAMI_LAPI_IMPL

////////////////////////////////////////////////////////////////////////////////
/// \addtogroup env_vars_ Environment Variables
/// \envs{,pami,PAMI}
///
/// \internal
/// Environment variable documentation uses the custom doxygen tags "\envs",
/// "\env", and "\default". These tags are defined as the following ALIASES in
/// the Doxyfile:
///
/// \code
///   ALIASES  = default="\n \par Default:\n"
///   ALIASES += envs{3}="\addtogroup env_vars_\2 \3 \n \ingroup env_vars_\1"
///   ALIASES += env{2}="\addtogroup env_vars_\1 \n \anchor \2 \section \2 "
/// \endcode
///
/// \endinternal
////////////////////////////////////////////////////////////////////////////////

#include "Global.h"
#include "Client.h"
#include "Context.h"
#include <pami.h>
#include "util/common.h"
#include "algorithms/protocols/allreduce/ReduceFunctions.h" // For dt_query

extern "C"
{
  pami_client_t    PAMI_CLIENT_NULL   =0;
  pami_type_t      PAMI_TYPE_NULL     =0;
  pami_context_t   PAMI_CONTEXT_NULL  =0;
  pami_geometry_t  PAMI_GEOMETRY_NULL =0;
  pami_algorithm_t PAMI_ALGORITHM_NULL=0;
  size_t           PAMI_ALL_CONTEXTS  =(size_t) -1;
#ifndef PAMI_ENDPOINT_NULL
  pami_endpoint_t PAMI_ENDPOINT_NULL = (pami_endpoint_t) -1;
#endif
}

////////////////////////////////////////////////////////////////////////////////
// Functions from pami_misc.h                                                  //
////////////////////////////////////////////////////////////////////////////////

extern "C" size_t PAMI_Error_text (char * string, size_t length)
{
#ifdef PAMI_LAPI_IMPL
    if (length == 0)
        return 0;

    const char *err_msg = ReturnErr::_get_err_msg();
    if (err_msg == NULL) {
        string[0] = '\0';
        return 0;
    }

    size_t msg_len = strlen(err_msg);
    strncpy(string, err_msg, length - 1);
    string[length - 1] = '\0';

    if (msg_len > length - 1)
        msg_len = length - 1;
    return msg_len;
#else
    return 0; // unimplemented
#endif
}

extern "C" double PAMI_Wtime (pami_client_t client)
{
    PAMI::Client * _client = (PAMI::Client *) client;
    return _client->wtime (); 
}

extern "C" unsigned long long PAMI_Wtimebase(pami_client_t client)
{
    PAMI::Client * _client = (PAMI::Client *) client;
    return _client->wtimebase (); 
}

#ifndef PAMI_Endpoint_create
extern "C" pami_result_t PAMI_Endpoint_create (pami_client_t     client,
                                               pami_task_t       task,
                                               size_t            offset,
                                               pami_endpoint_t * endpoint)
{
  (void)client;
  pami_result_t rc = PAMI_SUCCESS;
  *endpoint = PAMI_ENDPOINT_INIT(client,task,offset);
  return rc;
}
#endif

#ifndef PAMI_Endpoint_query
extern "C" pami_result_t PAMI_Endpoint_query (pami_endpoint_t   endpoint,
                                              pami_task_t     * task,
                                              size_t          * offset)
{
  pami_result_t rc = PAMI_SUCCESS;
  PAMI_ENDPOINT_INFO(endpoint,*task,*offset);
  return rc;
}
#endif

extern "C" pami_result_t PAMI_Context_post (pami_context_t        context,
                                          pami_work_t         * work,
                                          pami_work_function    fn,
                                          void               * cookie)
{
  PAMI::Context * ctx = (PAMI::Context *) context;

  return ctx->post (work, fn, cookie);
}

////////////////////////////////////////////////////////////////////////////////
// Functions from pami_fence.h                                                 //
////////////////////////////////////////////////////////////////////////////////

extern "C" pami_result_t PAMI_Fence_begin (pami_context_t      context)

{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->fence_begin ();
}

extern "C" pami_result_t PAMI_Fence_end (pami_context_t      context)

{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->fence_end ();
}

extern "C" pami_result_t PAMI_Fence_all (pami_context_t        context,
                                       pami_event_function   done_fn,
                                       void               * cookie)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->fence_all (done_fn, cookie);
}

extern "C" pami_result_t PAMI_Fence_endpoint (pami_context_t        context,
                                              pami_event_function   done_fn,
                                              void                * cookie,
                                              pami_endpoint_t       endpoint)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->fence_endpoint (done_fn, cookie, endpoint);
}

////////////////////////////////////////////////////////////////////////////////
// Functions from pami_collectives.h                                           //
////////////////////////////////////////////////////////////////////////////////


extern "C" pami_result_t PAMI_Geometry_create_taskrange (pami_client_t           client,
                                                         size_t                  context_offset,
                                                         pami_configuration_t    configuration[],
                                                         size_t                  num_configs,
                                                         pami_geometry_t       * geometry,
                                                         pami_geometry_t         parent,
                                                         unsigned                id,
                                                         pami_geometry_range_t * task_slices,
                                                         size_t                  slice_count,
                                                         pami_context_t          context,
                                                         pami_event_function     fn,
                                                         void                  * cookie)
{
  PAMI::Client * _client = (PAMI::Client *) client;
  return _client->geometry_create_taskrange (geometry,
                                             context_offset,
                                             configuration,
                                             num_configs,
                                             parent,
                                             id,
                                             task_slices,
                                             slice_count,
                                             context,
                                             fn,
                                             cookie);
}

extern "C" pami_result_t PAMI_Geometry_create_tasklist (pami_client_t               client,
                                                        size_t                      context_offset,
                                                        pami_configuration_t        configuration[],
                                                        size_t                      num_configs,
                                                        pami_geometry_t           * geometry,
                                                        pami_geometry_t             parent,
                                                        unsigned                    id,
                                                        pami_task_t               * tasks,
                                                        size_t                      task_count,
                                                        pami_context_t              context,
                                                        pami_event_function         fn,
                                                        void                      * cookie)
{
  PAMI::Client * _client = (PAMI::Client *) client;
  return _client->geometry_create_tasklist (geometry,
                                            context_offset,
                                            configuration,
                                            num_configs,
                                            parent,
                                            id,
                                            tasks,
                                            task_count,
                                            context,
                                            fn,
                                            cookie);
}

extern "C" pami_result_t PAMI_Geometry_create_endpointlist (pami_client_t               client,
                                                            pami_configuration_t        configuration[],
                                                            size_t                      num_configs,
                                                            pami_geometry_t           * geometry,
                                                            unsigned                    id,
                                                            pami_endpoint_t           * endpoints,
                                                            size_t                      endpoint_count,
                                                            pami_context_t              context,
                                                            pami_event_function         fn,
                                                            void                      * cookie)
{
  PAMI::Client * _client = (PAMI::Client *) client;
  return _client->geometry_create_endpointlist (geometry,
                                                configuration,
                                                num_configs,
                                                id,
                                                endpoints,
                                                endpoint_count,
                                                context,
                                                fn,
                                                cookie);
}


extern "C" pami_result_t PAMI_Geometry_query (pami_geometry_t       geometry,
                                              pami_configuration_t  configuration[],
                                              size_t                num_configs)
{
  PAMI_GEOMETRY_CLASS* _geometry = (PAMI_GEOMETRY_CLASS *) geometry;
  PAMI::Client * _client = (PAMI::Client *)_geometry->getClient();
  return _client->geometry_query(geometry, configuration, num_configs);
}

extern "C"  pami_result_t PAMI_Geometry_update (pami_geometry_t       geometry,
                                                pami_configuration_t  configuration[],
                                                size_t                num_configs,
                                                pami_context_t        context,
                                                pami_event_function   fn,
                                                void                 *cookie)
{
  PAMI_GEOMETRY_CLASS* _geometry = (PAMI_GEOMETRY_CLASS *) geometry;
  PAMI::Client * _client = (PAMI::Client *)_geometry->getClient();
  return _client->geometry_update(geometry, configuration, num_configs, context, fn, cookie);
}

extern "C" pami_result_t PAMI_Geometry_world (pami_client_t                client,
                                 pami_geometry_t            * world_geometry)
{
  PAMI::Client * _client = (PAMI::Client *) client;
  return _client->geometry_world (world_geometry);
}

extern "C" pami_result_t PAMI_Geometry_destroy(pami_client_t          client,
                                               pami_geometry_t      * geometry,
                                               pami_context_t         context,
                                               pami_event_function    fn,
                                               void                 * cookie)
{
  PAMI::Client * _client = (PAMI::Client *) client;
  pami_result_t rc = _client->geometry_destroy (*geometry, context, fn, cookie);
  *geometry = NULL;
  return rc;
}

extern "C" pami_result_t PAMI_Collective (pami_context_t   context,
                                        pami_xfer_t     *parameters)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->collective (parameters);
}

extern "C" pami_result_t PAMI_Geometry_algorithms_num (pami_geometry_t  geometry,
                                                       pami_xfer_type_t coll_type,
                                                       size_t           lists_lengths[2])
{
  PAMI_GEOMETRY_CLASS *_geometry = (PAMI_GEOMETRY_CLASS *) geometry;
  return _geometry->algorithms_num(coll_type,lists_lengths);
}

extern "C"  pami_result_t PAMI_Geometry_algorithms_query (pami_geometry_t geometry,
                                                          pami_xfer_type_t   colltype,
                                                          pami_algorithm_t  *algs0,
                                                          pami_metadata_t   *mdata0,
                                                          size_t             num0,
                                                          pami_algorithm_t  *algs1,
                                                          pami_metadata_t   *mdata1,
                                                          size_t               num1)
{
  PAMI_GEOMETRY_CLASS *_geometry = (PAMI_GEOMETRY_CLASS *) geometry;
  return _geometry->algorithms_info(colltype,
                                            algs0,
                                            mdata0,
                                            num0,
                                            algs1,
                                            mdata1,
                                            num1);
}

////////////////////////////////////////////////////////////////////////////////
// Geometry Utility functions                                                 //
////////////////////////////////////////////////////////////////////////////////
pami_geometry_t mapidtogeometry (pami_context_t context, int comm)
{
  PAMI::Context * ctx    = (PAMI::Context *) context;
  PAMI::Client  * client = (PAMI::Client *)  ctx->getClient ();
  return client->mapidtogeometry(comm);
}
void registerunexpbarrier(pami_context_t context,
                          unsigned       comm,
                          pami_quad_t   &info,
                          unsigned       peer,
                          unsigned       algorithm)
{
  PAMI::Context * ctx    = (PAMI::Context *) context;
  ctx->registerUnexpBarrier(comm,info,peer,algorithm);
}

///

////////////////////////////////////////////////////////////////////////////////
// Functions from pami_dispatch.h                                              //
////////////////////////////////////////////////////////////////////////////////

extern "C" pami_result_t PAMI_AMCollective_dispatch_set(pami_context_t              context,
                                                      pami_algorithm_t            algorithm,
                                                      size_t                     dispatch,
                                                      pami_dispatch_callback_function fn,
                                                      void                     * cookie,
                                                      pami_collective_hint_t      options)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->amcollective_dispatch (algorithm, dispatch, fn, cookie, options);
}

////////////////////////////////////////////////////////////////////////////////
// Type function implementations                                              //
////////////////////////////////////////////////////////////////////////////////

#include "pami_type.cc"


extern "C" pami_result_t PAMI_Client_create (const char           *name,
                                             pami_client_t        *client,
                                             pami_configuration_t  configuration[],
                                             size_t                num_configs)
{
  return PAMI::Client::generate (name, client, configuration, num_configs);
}

extern "C" pami_result_t PAMI_Client_destroy (pami_client_t *client)
{
  PAMI::Client::destroy ((PAMI::Client *) *client);
  *client = NULL;
  return PAMI_SUCCESS;
}



extern "C" pami_result_t PAMI_Context_createv (pami_client_t          client,
                                            pami_configuration_t    configuration[],
                                            size_t                 count,
                                            pami_context_t        * context,
                                            size_t                 ncontexts)
{
  pami_result_t result;
  PAMI::Client * pami = (PAMI::Client *) client;

  result = pami->createContext (configuration, count, context, ncontexts);

  return result;
}

extern "C" pami_result_t PAMI_Context_destroyv (pami_context_t* contexts,
                                                size_t          ncontexts)
{
  PAMI_assert(contexts != NULL);
  pami_result_t result = PAMI_SUCCESS;

  // have to fudge a client object out of this, since the API is not symmetrical
  if (ncontexts >= 1)
  {
    PAMI::Context * ctx = (PAMI::Context *) contexts[0];
    PAMI::Client  * client = (PAMI::Client *) ctx->getClient ();
    result = client->destroyContext (contexts, ncontexts);
  }
  return result;
}

extern "C" pami_result_t PAMI_Client_query (pami_client_t         client,
                                            pami_configuration_t  configuration[],
                                            size_t                num_configs)
{
  PAMI::Client * cln = (PAMI::Client *) client;

  return cln->query(configuration,num_configs);
}

extern "C" pami_result_t PAMI_Client_update (pami_client_t         client,
                                             pami_configuration_t  configuration[],
                                             size_t                num_configs)
{
  PAMI::Client * cln = (PAMI::Client *) client;
  return cln->update(configuration,num_configs);
}

extern "C" pami_result_t PAMI_Dispatch_query (pami_context_t        context,
                                              size_t                dispatch,
                                              pami_configuration_t  configuration[],
                                              size_t                num_configs)
{
    PAMI::Context * ctx = (PAMI::Context *) context;
    return ctx->dispatch_query (dispatch, configuration, num_configs);
}

extern "C" pami_result_t PAMI_Dispatch_update (pami_context_t        context,
                                               size_t                dispatch,
                                               pami_configuration_t  configuration[],
                                               size_t                num_configs)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->dispatch_update(dispatch, configuration, num_configs);
}



extern "C" pami_result_t PAMI_Context_query (pami_context_t        context,
                                             pami_configuration_t  configuration[],
                                             size_t                num_configs)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->query(configuration, num_configs);
}

extern "C" pami_result_t PAMI_Context_update (pami_context_t        context,
                                              pami_configuration_t  configuration[],
                                              size_t                num_configs)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->update(configuration, num_configs);
}



extern "C" pami_result_t PAMI_Dispatch_set (pami_context_t             context,
                                            size_t                     dispatch,
                                            pami_dispatch_callback_function fn,
                                            void                     * cookie,
                                            pami_dispatch_hint_t       options)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->dispatch (dispatch, fn, cookie, options);
}


extern "C" pami_result_t PAMI_Context_lock (pami_context_t context)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->lock ();
}

extern "C" pami_result_t PAMI_Context_trylock (pami_context_t context)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->trylock ();
}

extern "C" pami_result_t PAMI_Context_unlock (pami_context_t context)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->unlock ();
}


extern "C" pami_result_t PAMI_Context_advance (pami_context_t context, size_t maximum)
{
  pami_result_t result;
  PAMI::Context * ctx = (PAMI::Context *) context;
  ctx->advance (maximum, result);

  return result;
}

extern "C" pami_result_t PAMI_Context_advancev (pami_context_t context[],
                                                size_t         count,
                                                size_t         maximum)
{
  unsigned m, c;
  PAMI::Context * ctx;

  pami_result_t result = PAMI_SUCCESS;
  size_t events = 0;

  for (m=0; m<maximum && events==0; m++)
  {
    for (c=0; c<count; c++)
    {
      ctx = (PAMI::Context *) context[c];
      events += ctx->advance (1, result);
      if (result != PAMI_SUCCESS && result != PAMI_EAGAIN)
         return result;
    }
  }

  return result;
}

extern "C" pami_result_t PAMI_Context_trylock_advancev (pami_context_t context[],
                                                        size_t         count,
                                                        size_t         maximum)
{
  unsigned m, c;
  PAMI::Context * ctx;

  pami_result_t result = PAMI_SUCCESS;
  size_t events = 0;

  for (m=0; m<maximum && events==0 && result==PAMI_SUCCESS; m++)
  {
    for (c=0; c<count && result==PAMI_SUCCESS; c++)
    {
      ctx = (PAMI::Context *)context[c];
      if (unlikely(ctx->trylock() == PAMI_SUCCESS))
        {
          events += ctx->advance(1, result);
          ctx->unlock();
        }
    }
  }

  return result;
}

////////////////////////////////////////////////////////////////////////////////
// Functions from pami_p2p.h                                                   //
////////////////////////////////////////////////////////////////////////////////

extern "C" pami_result_t PAMI_Send (pami_context_t   context,
                                  pami_send_t    * parameters)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->send (parameters);
}

extern "C" pami_result_t PAMI_Send_immediate (pami_context_t          context,
                                            pami_send_immediate_t * parameters)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->send (parameters);
}

extern "C" pami_result_t PAMI_Send_typed (pami_context_t      context,
                                        pami_send_typed_t * parameters)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->send (parameters);
}


extern "C" pami_result_t PAMI_Put (pami_context_t      context,
                                 pami_put_simple_t * parameters)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->put (parameters);
}


extern "C" pami_result_t PAMI_Put_typed (pami_context_t      context,
                                       pami_put_typed_t  * parameters)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->put_typed (parameters);
}

extern "C" pami_result_t PAMI_Get (pami_context_t      context,
                                 pami_get_simple_t * parameters)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->get (parameters);
}


extern "C" pami_result_t PAMI_Get_typed (pami_context_t      context,
                                       pami_get_typed_t  * parameters)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->get_typed (parameters);
}


extern "C" pami_result_t PAMI_Rmw (pami_context_t      context,
                                 pami_rmw_t * parameters)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->rmw (parameters);
}

extern "C" pami_result_t PAMI_Memregion_create (pami_context_t     context,
                                                void             * address,
                                                size_t             bytes_in,
                                                size_t           * bytes_out,
                                                pami_memregion_t * memregion)
{
    PAMI::Context   * ctx = (PAMI::Context *) context;
    return ctx->memregion_create (address, bytes_in, bytes_out, memregion);
}

extern "C" pami_result_t PAMI_Memregion_destroy (pami_context_t     context,
                                                 pami_memregion_t * memregion)
{
    PAMI::Context   * ctx = (PAMI::Context *) context;
    return ctx->memregion_destroy (memregion);
}

extern "C" pami_result_t PAMI_Rput (pami_context_t      context,
                                 pami_rput_simple_t * parameters)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->rput (parameters);
}


extern "C" pami_result_t PAMI_Rput_typed (pami_context_t      context,
                                       pami_rput_typed_t  * parameters)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->rput_typed (parameters);
}

extern "C" pami_result_t PAMI_Rget (pami_context_t      context,
                                 pami_rget_simple_t * parameters)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->rget (parameters);
}


extern "C" pami_result_t PAMI_Rget_typed (pami_context_t      context,
                                        pami_rget_typed_t  * parameters)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->rget_typed (parameters);
}


extern "C" pami_result_t PAMI_Purge(pami_context_t    context,
                                    pami_endpoint_t * dest,
                                    size_t            count)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->purge_totask (dest, count);
}


extern "C"   pami_result_t PAMI_Resume (pami_context_t    context,
                                        pami_endpoint_t * dest,
                                        size_t            count)
{
  PAMI::Context * ctx = (PAMI::Context *) context;
  return ctx->resume_totask (dest, count);
}

#include "api/extension/Extension.h"
#include "api/extension/registry.h"

extern "C" pami_result_t PAMI_Extension_open (pami_client_t      client,
                                              const char       * name,
                                              pami_extension_t * extension)
{
  #define PAMI_EXTENSION_DEFINE(a,n,x,y)                    \
    if (strcmp(#a, name) == 0)                              \
      return PAMI::Extension::open<n>(client, #a, *extension);

  #include "api/extension/registry.def"

  #undef PAMI_EXTENSION_DEFINE

  return PAMI::Extension::open<0>(client, name, *extension);
}

extern "C" pami_result_t PAMI_Extension_close (pami_extension_t extension)
{
  PAMI::Extension * x = (PAMI::Extension *) extension;
  return x->close();
}

extern "C" void * PAMI_Extension_symbol (pami_extension_t   extension,
                                           const char       * fn)
{
  PAMI::Extension * x = (PAMI::Extension *) extension;
  return x->function (fn);
}


//
//
// Temporary! Move this someplace else!
//
//
//#include "api/c/pami_ext_impl.h"
