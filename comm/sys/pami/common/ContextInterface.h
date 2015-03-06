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
/// \file common/ContextInterface.h
/// \brief PAMI context interface.
///
#ifndef __common_ContextInterface_h__
#define __common_ContextInterface_h__

#include <stdlib.h>
#include <string.h>

#include <pami.h>

namespace PAMI
{
  namespace Interface
  {
    template <class T_Context>
    class Context
    {
      public:
        inline Context (pami_client_t client, size_t id) {}

        inline pami_client_t getClient ();

        inline size_t getId ();

        inline pami_result_t destroy ();

        inline pami_result_t post (pami_work_t *state, pami_work_function work_fn, void * cookie);

        inline size_t advance (size_t maximum, pami_result_t & result);

        inline pami_result_t lock ();

        inline pami_result_t trylock ();

        inline pami_result_t unlock ();

        inline pami_result_t send (pami_send_t * parameters);

        inline pami_result_t send (pami_send_immediate_t * parameters);

        inline pami_result_t send (pami_send_typed_t * parameters);

        inline pami_result_t put (pami_put_simple_t * parameters);

        inline pami_result_t put_typed (pami_put_typed_t * parameters);

        inline pami_result_t get (pami_get_simple_t * parameters);

        inline pami_result_t get_typed (pami_get_typed_t * parameters);

        inline pami_result_t rmw (pami_rmw_t * parameters);

        inline pami_result_t memregion_create (void             * address,
                                               size_t             bytes_in,
                                               size_t           * bytes_out,
                                               pami_memregion_t * memregion);

        inline pami_result_t memregion_destroy (pami_memregion_t * memregion);

        inline pami_result_t rput (pami_rput_simple_t * parameters);

        inline pami_result_t rput_typed (pami_rput_typed_t * parameters);

        inline pami_result_t rget (pami_rget_simple_t * parameters);

        inline pami_result_t rget_typed (pami_rget_typed_t * parameters);

        inline pami_result_t purge_totask (pami_endpoint_t *dest, size_t count);

        inline pami_result_t resume_totask (pami_endpoint_t *dest, size_t count);

        inline pami_result_t fence_begin ();

        inline pami_result_t fence_end ();

        inline pami_result_t fence_all (pami_event_function   done_fn,
                                       void               * cookie);

        inline pami_result_t fence_endpoint (pami_event_function   done_fn,
                                             void                * cookie,
                                             pami_endpoint_t       endpoint);

        inline pami_result_t collective (pami_xfer_t * parameters);

        inline pami_result_t dispatch (size_t                          dispatch,
                                       pami_dispatch_callback_function fn,
                                       void                          * cookie,
                                       pami_dispatch_hint_t            options);

        inline pami_result_t amcollective_dispatch(pami_algorithm_t            algorithm,
                                                  size_t                     dispatch,
                                                  pami_dispatch_callback_function fn,
                                                  void                     * cookie,
                                                  pami_collective_hint_t      options);
      inline pami_result_t dispatch_query(size_t                dispatch,
                                          pami_configuration_t  configuration[],
                                          size_t                num_configs);

      inline pami_result_t dispatch_update(size_t                dispatch,
                                           pami_configuration_t  configuration[],
                                           size_t                num_configs);

      inline pami_result_t query(pami_configuration_t  configuration[],
                                 size_t                num_configs);

      inline pami_result_t update(pami_configuration_t  configuration[],
                                  size_t                num_configs);

      inline void registerUnexpBarrier (unsigned     comm,
                                        pami_quad_t &info,
                                        unsigned     peer,
                                        unsigned     algorithm);
    }; // end class PAMI::Context::Context

    template <class T_Context>
    pami_client_t Context<T_Context>::getClient ()
    {
      return static_cast<T_Context*>(this)->getClient_impl();
    }

    template <class T_Context>
    size_t Context<T_Context>::getId ()
    {
      return static_cast<T_Context*>(this)->getId_impl();
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::destroy ()
    {
      return static_cast<T_Context*>(this)->destroy_impl();
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::post (pami_work_t *state, pami_work_function work_fn, void * cookie)
    {
      return static_cast<T_Context*>(this)->post_impl(state, work_fn, cookie);
    }

    template <class T_Context>
    size_t Context<T_Context>::advance (size_t maximum, pami_result_t & result)
    {
      return static_cast<T_Context*>(this)->advance_impl(maximum, result);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::lock ()
    {
      return static_cast<T_Context*>(this)->lock_impl();
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::trylock ()
    {
      return static_cast<T_Context*>(this)->trylock_impl();
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::unlock ()
    {
      return static_cast<T_Context*>(this)->unlock_impl();
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::send (pami_send_t * parameters)
    {
      return static_cast<T_Context*>(this)->send_impl(parameters);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::send (pami_send_immediate_t * parameters)
    {
      return static_cast<T_Context*>(this)->send_impl(parameters);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::send (pami_send_typed_t * parameters)
    {
      return static_cast<T_Context*>(this)->send_impl(parameters);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::put (pami_put_simple_t * parameters)
    {
      return static_cast<T_Context*>(this)->put_impl(parameters);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::put_typed (pami_put_typed_t * parameters)
    {
      return static_cast<T_Context*>(this)->put_typed_impl(parameters);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::get (pami_get_simple_t * parameters)
    {
      return static_cast<T_Context*>(this)->get_impl(parameters);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::get_typed (pami_get_typed_t * parameters)
    {
      return static_cast<T_Context*>(this)->get_typed_impl(parameters);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::rmw (pami_rmw_t * parameters)
    {
      return static_cast<T_Context*>(this)->rmw_impl(parameters);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::memregion_create (void             * address,
                                                        size_t             bytes_in,
                                                        size_t           * bytes_out,
                                                        pami_memregion_t * memregion)
    {
      return static_cast<T_Context*>(this)->memregion_create_impl(address,
                                                                  bytes_in,
                                                                  bytes_out,
                                                                  memregion);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::memregion_destroy (pami_memregion_t * memregion)
    {
      return static_cast<T_Context*>(this)->memregion_destroy_impl(memregion);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::rput (pami_rput_simple_t * parameters)
    {
      return static_cast<T_Context*>(this)->rput_impl(parameters);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::rput_typed (pami_rput_typed_t * parameters)
    {
      return static_cast<T_Context*>(this)->rput_typed_impl(parameters);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::rget (pami_rget_simple_t * parameters)
    {
      return static_cast<T_Context*>(this)->rget_impl(parameters);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::rget_typed (pami_rget_typed_t * parameters)
    {
      return static_cast<T_Context*>(this)->rget_typed_impl(parameters);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::purge_totask (pami_endpoint_t *dest, size_t count)
    {
      return static_cast<T_Context*>(this)->purge_totask_impl(dest, count);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::resume_totask (pami_endpoint_t *dest, size_t count)
    {
      return static_cast<T_Context*>(this)->resume_totask_impl(dest, count);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::fence_begin ()
    {
      return static_cast<T_Context*>(this)->fence_begin_impl();
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::fence_end ()
    {
      return static_cast<T_Context*>(this)->fence_end_impl();
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::fence_all (pami_event_function   done_fn,
                                                void               * cookie)
    {
      return static_cast<T_Context*>(this)->fence_all_impl(done_fn, cookie);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::fence_endpoint (pami_event_function   done_fn,
                                                      void                * cookie,
                                                      pami_endpoint_t       endpoint)
    {
      return static_cast<T_Context*>(this)->fence_endpoint_impl(done_fn, cookie, endpoint);
    }


    template <class T_Context>
    pami_result_t Context<T_Context>::collective(pami_xfer_t * parameters)
    {
      return static_cast<T_Context*>(this)->collective_impl(parameters);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::dispatch (size_t                          dispatch,
                                                pami_dispatch_callback_function fn,
                                                void                          * cookie,
                                                pami_dispatch_hint_t            options)
    {
        return static_cast<T_Context*>(this)->dispatch_impl(dispatch,fn,cookie,options);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::amcollective_dispatch(pami_algorithm_t            algorithm,
                                                           size_t                     dispatch,
                                                           pami_dispatch_callback_function fn,
                                                           void                     * cookie,
                                                           pami_collective_hint_t      options)
    {
      return static_cast<T_Context*>(this)->amcollective_dispatch_impl(algorithm,
                                                                       dispatch,
                                                                       fn,
                                                                       cookie,
                                                                       options);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::dispatch_query(size_t                dispatch,
                                                     pami_configuration_t  configuration[],
                                                     size_t                num_configs)
    {
      return static_cast<T_Context*>(this)->dispatch_query_impl(dispatch,
                                                                configuration,
                                                                num_configs);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::dispatch_update(size_t                dispatch,
                                                      pami_configuration_t  configuration[],
                                                      size_t                num_configs)
    {
      return static_cast<T_Context*>(this)->dispatch_update_impl(dispatch,
                                                                 configuration,
                                                                 num_configs);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::query(pami_configuration_t  configuration[],
                                            size_t                num_configs)
    {
      return static_cast<T_Context*>(this)->query_impl(configuration,
                                                       num_configs);
    }

    template <class T_Context>
    pami_result_t Context<T_Context>::update(pami_configuration_t  configuration[],
                                             size_t                num_configs)
    {
      return static_cast<T_Context*>(this)->update_impl(configuration,
                                                        num_configs);
    }

    template <class T_Context>
    void Context<T_Context>::registerUnexpBarrier (unsigned     comm,
                                                   pami_quad_t &info,
                                                   unsigned     peer,
                                                   unsigned     algorithm)
    {
      static_cast<T_Context*>(this)->registerUnexpBarrier_impl(comm,
                                                               info,
                                                               peer,
                                                               algorithm);
    }


  }; // end namespace Interface
}; // end namespace PAMI

#endif // __components_context_context_h__
