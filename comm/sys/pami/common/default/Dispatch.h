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
 * \file common/default/Dispatch.h
 * \brief ???
 */

#ifndef __common_default_Dispatch_h__
#define __common_default_Dispatch_h__

#include <pami.h>

#include "p2p/protocols/Send.h"
#include "p2p/protocols/Put.h"
#include "p2p/protocols/Get.h"
#include "p2p/protocols/RPut.h"
#include "p2p/protocols/RGet.h"

#include "p2p/protocols/Fence.h"

#include "util/trace.h"

#undef DO_TRACE_ENTEREXIT 
#undef DO_TRACE_DEBUG     
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  template <unsigned T_DispatchCount>
  class Dispatch
  {
    protected:

      Protocol::Fence::Error   _default_fence;
      Protocol::Put::NoPut     _default_put;
      Protocol::Get::NoGet     _default_get;
      Protocol::Put::NoRPut    _default_rput;
      Protocol::Get::NoRGet    _default_rget;
      Protocol::Send::Error    _default_send;

      Protocol::Fence::Fence * _fence;

      Protocol::Put::Put     * _fence_supported_put;
      Protocol::Put::Put     * _fence_unsupported_put;
      Protocol::Put::Put     * _put;

      Protocol::Get::Get     * _fence_supported_get;
      Protocol::Get::Get     * _fence_unsupported_get;
      Protocol::Get::Get     * _get;

      Protocol::Put::RPut    * _fence_supported_rput;
      Protocol::Put::RPut    * _fence_unsupported_rput;
      Protocol::Put::RPut    * _rput;

      Protocol::Get::RGet    * _fence_supported_rget;
      Protocol::Get::RGet    * _fence_unsupported_rget;
      Protocol::Get::RGet    * _rget;

      Protocol::Send::Send  *  _fence_unsupported_send[T_DispatchCount];
      Protocol::Send::Send  *  _fence_supported_send[T_DispatchCount];
      Protocol::Send::Send  ** _send;

    public:

      int id;

      inline Dispatch (pami_context_t context) :
          _default_fence (),
          _default_put (context),
          _default_get (context),
          _default_rput (),
          _default_rget (),
          _default_send (),

          _fence (& _default_fence),
          _fence_supported_put (& _default_put),
          _fence_unsupported_put (& _default_put),
          _put (_fence_unsupported_put),
          _fence_supported_get (& _default_get),
          _fence_unsupported_get (& _default_get),
          _get (_fence_unsupported_get),
          _fence_supported_rput (& _default_rput),
          _fence_unsupported_rput (& _default_rput),
          _rput (_fence_unsupported_rput),
          _fence_supported_rget (& _default_rget),
          _fence_unsupported_rget (& _default_rget),
          _rget (_fence_unsupported_rget),
          id (T_DispatchCount - 1)
      {
        TRACE_FN_ENTER();
        size_t i;

        for (i = 0; i < T_DispatchCount; i++)
          {
            _fence_unsupported_send[i] = & _default_send;
            _fence_supported_send[i]  = & _default_send;
          };

        _send = _fence_unsupported_send;

        TRACE_FORMAT( "_send = %p, _put = %p, _get = %p, _rput = %p, _rget = %p", _send, _put, _get, _rput, _rget);
        TRACE_FN_EXIT();
      };

      inline ~Dispatch () {};

      inline void init (Protocol::Fence::Fence * fence_protocol)
      {
        TRACE_FN_ENTER();

        _fence = fence_protocol;

        TRACE_FN_EXIT();
      };

      inline void init (Protocol::Put::Put     * fence_supported_put,
                        Protocol::Put::Put     * fence_unsupported_put = NULL)
      {
        TRACE_FN_ENTER();

        // Determine if there is a current fence region
        bool enabled = (_put==_fence_supported_put);

        _fence_supported_put = fence_supported_put;

        if (fence_unsupported_put == NULL)
          _fence_unsupported_put = fence_supported_put;
        else
          _fence_unsupported_put = fence_unsupported_put;

        // Set the protocol to the appropriate pointer based on the fence region
        if (enabled)
          _put = _fence_supported_put;
        else
          _put = _fence_unsupported_put;

        TRACE_FN_EXIT();
      };

      inline void init (Protocol::Get::Get     * fence_supported_get,
                        Protocol::Get::Get     * fence_unsupported_get = NULL)
      {
        TRACE_FN_ENTER();

        // Determine if there is a current fence region
        bool enabled = (_get==_fence_supported_get);

        _fence_supported_get = fence_supported_get;

        if (fence_unsupported_get == NULL)
          _fence_unsupported_get = fence_supported_get;
        else
          _fence_unsupported_get = fence_unsupported_get;

        // Set the protocol to the appropriate pointer based on the fence region
        if (enabled)
          _get = _fence_supported_get;
        else
          _get = _fence_unsupported_get;

        TRACE_FN_EXIT();
      };

      inline void init (Protocol::Put::RPut    * fence_supported_rput,
                        Protocol::Put::RPut    * fence_unsupported_rput = NULL)
      {
        TRACE_FN_ENTER();

        // Determine if there is a current fence region
        bool enabled = (_rput==_fence_supported_rput);

        _fence_supported_rput = fence_supported_rput;

        if (fence_unsupported_rput == NULL)
          _fence_unsupported_rput = fence_supported_rput;
        else
          _fence_unsupported_rput = fence_unsupported_rput;

        // Set the protocol to the appropriate pointer based on the fence region
        if (enabled)
          _rput = _fence_supported_rput;
        else
          _rput = _fence_unsupported_rput;

        TRACE_FN_EXIT();
      };

      inline void init (Protocol::Get::RGet    * fence_supported_rget,
                        Protocol::Get::RGet    * fence_unsupported_rget = NULL)
      {
        TRACE_FN_ENTER();

        // Determine if there is a current fence region
        bool enabled = (_rget==_fence_supported_rget);

        _fence_supported_rget   = fence_supported_rget;

        if (fence_unsupported_rget == NULL)
          _fence_unsupported_rget = fence_supported_rget;
        else
          _fence_unsupported_rget = fence_unsupported_rget;

        // Set the protocol to the appropriate pointer based on the fence region
        if (enabled)
          _rget = _fence_supported_rget;
        else
          _rget = _fence_unsupported_rget;

        TRACE_FN_EXIT();
      };

      inline pami_result_t query (size_t                dispatch_id,
                                  pami_configuration_t  configuration[],
                                  size_t                num_configs)
      {
        TRACE_FN_ENTER();

        if (dispatch_id >= T_DispatchCount)
          {
            TRACE_FORMAT( "Error. dispatch_id (%zu) is not >= T_DispatchCount (%d)", dispatch_id, T_DispatchCount);
            TRACE_FN_EXIT();
            return PAMI_INVAL;
          }

        TRACE_FORMAT( "_send[%zu] = %p", dispatch_id, _send[dispatch_id]);
        pami_result_t result = _send[dispatch_id]->getAttributes (configuration, num_configs);

        TRACE_FN_EXIT();
        return result;
      };

      inline pami_result_t update (size_t                dispatch_id,
                                   pami_configuration_t  configuration[],
                                   size_t                num_configs)
      {
        TRACE_FN_ENTER();

        if (dispatch_id >= T_DispatchCount)
          {
            TRACE_FORMAT( "Error. dispatch_id (%zu) is not >= T_DispatchCount (%d)", dispatch_id, T_DispatchCount);
            TRACE_FN_EXIT();
            return PAMI_INVAL;
          }

        TRACE_FN_EXIT();
        return PAMI_INVAL;
      };

      inline void fence (bool enable)
      {
        TRACE_FN_ENTER();

        if (enable)
          {
            _send = _fence_supported_send;
            _put  = _fence_supported_put;
            _get  = _fence_supported_get;
            _rput = _fence_supported_rput;
            _rget = _fence_supported_rget;
          }
        else
          {
            _send = _fence_unsupported_send;
            _put  = _fence_unsupported_put;
            _get  = _fence_unsupported_get;
            _rput = _fence_unsupported_rput;
            _rget = _fence_unsupported_rget;
          }

        TRACE_FORMAT( "enable = %s, _send = %p, _put = %p, _get = %p, _rput = %p, _rget = %p", enable==true?"true":"false", _send, _put, _get, _rput, _rget);
        TRACE_FN_EXIT();
      };

      /// "fence all"
      inline pami_result_t fence (pami_event_function   done_fn,
                                  void                * cookie)
      {
        TRACE_FN_ENTER();

#ifdef ERROR_CHECKS

        if (_send != _fence_supported_send)
          {
            TRACE_STRING( "Error. Not in an active fence region");
            TRACE_FN_EXIT();
            return PAMI_INVAL;
          }

#endif

        pami_result_t result = _fence->all (done_fn, cookie);
        TRACE_FORMAT("return result=%d",result);
        TRACE_FN_EXIT();
        return result;
      };

      /// "fence endpoint"
      inline pami_result_t fence (pami_event_function   done_fn,
                                  void                * cookie,
                                  pami_endpoint_t       endpoint)
      {
        TRACE_FN_ENTER();

#ifdef ERROR_CHECKS

        if (_send != _fence_supported_send)
          {
            TRACE_STRING( "Error. Not in an active fence region");
            TRACE_FN_EXIT();
            return PAMI_INVAL;
          }

#endif
        pami_result_t result = _fence->endpoint (done_fn, cookie, endpoint);
        TRACE_FN_EXIT();
        return result;
      };

      inline pami_result_t set (size_t                 dispatch_id,
                                Protocol::Send::Send * fence_supported_send,
                                Protocol::Send::Send * fence_unsupported_send = NULL)
      {
        TRACE_FN_ENTER();

        if (dispatch_id >= T_DispatchCount)
          {
            TRACE_FORMAT( "Error. dispatch_id (%zu) is not >= T_DispatchCount (%d)", dispatch_id, T_DispatchCount);
            TRACE_FN_EXIT();
            return PAMI_INVAL;
          }

        if (_fence_supported_send[dispatch_id]  != (& _default_send))
          {
            TRACE_FORMAT( "Error. dispatch_id (%zu) was previously set", dispatch_id);
            TRACE_FN_EXIT();
            return PAMI_INVAL;
          }

        if (_fence_unsupported_send[dispatch_id] != (& _default_send))
          {
            TRACE_FORMAT( "Error. dispatch_id (%zu) was previously set", dispatch_id);
            TRACE_FN_EXIT();
            return PAMI_INVAL;
          }

        _fence_supported_send[dispatch_id]  = fence_supported_send;
        TRACE_FORMAT( "_fence_supported_send[%zu] = %p, &_fence_supported_send[%zu] = %p", dispatch_id, fence_supported_send, dispatch_id, _fence_supported_send[dispatch_id]);

        if (fence_unsupported_send == NULL)
          _fence_unsupported_send[dispatch_id] = fence_supported_send;
        else
          _fence_unsupported_send[dispatch_id] = fence_unsupported_send;
        TRACE_FORMAT( "_fence_unsupported_send[%zu] = %p, &_fence_unsupported_send[%zu] = %p", dispatch_id, fence_unsupported_send, dispatch_id, _fence_unsupported_send[dispatch_id]);

        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      };

      inline pami_result_t send (pami_send_t * parameters)
      {
        TRACE_FN_ENTER();

        size_t dispatch_id = (size_t)(parameters->send.dispatch);
        PAMI_assert_debug (dispatch_id < T_DispatchCount);
        pami_result_t result = _send[dispatch_id]->simple (parameters);

        TRACE_FN_EXIT();
        return result;
      };

      inline pami_result_t send (pami_send_immediate_t * parameters)
      {
        TRACE_FN_ENTER();

        size_t dispatch_id = (size_t)(parameters->dispatch);
        PAMI_assert_debug (dispatch_id < T_DispatchCount);
        pami_result_t result =  _send[dispatch_id]->immediate (parameters);

        TRACE_FN_EXIT();
        return result;
      };

      inline pami_result_t send (pami_send_typed_t * parameters)
      {
        TRACE_FN_ENTER();

        size_t dispatch_id = (size_t)(parameters->send.dispatch);
        PAMI_assert_debug (dispatch_id < T_DispatchCount);
        pami_result_t result =  _send[dispatch_id]->typed (parameters);

        TRACE_FN_EXIT();
        return result;
      }

      inline pami_result_t put (pami_put_simple_t * parameters)
      {
        TRACE_FN_ENTER();
        pami_result_t result = _put->simple (parameters);
        TRACE_FORMAT("result = %d", result);
        TRACE_FN_EXIT();
        return result;
      }

      inline pami_result_t put (pami_put_typed_t * parameters)
      {
        TRACE_FN_ENTER();
        pami_result_t result = _put->typed (parameters);
        TRACE_FORMAT("result = %d", result);
        TRACE_FN_EXIT();
        return result;
      }

      inline pami_result_t get (pami_get_simple_t * parameters)
      {
        TRACE_FN_ENTER();
        pami_result_t result = _get->get (parameters);
        TRACE_FORMAT("result = %d", result);
        TRACE_FN_EXIT();
        return result;
      }

      inline pami_result_t get (pami_get_typed_t * parameters)
      {
        TRACE_FN_ENTER();
        pami_result_t result = _get->get (parameters);
        TRACE_FORMAT("result = %d", result);
        TRACE_FN_EXIT();
        return result;
      }

      inline pami_result_t rput (pami_rput_simple_t * parameters)
      {
        TRACE_FN_ENTER();
        pami_result_t result = _rput->simple (parameters);
        TRACE_FORMAT("result = %d", result);
        TRACE_FN_EXIT();
        return result;
      }

      inline pami_result_t rput (pami_rput_typed_t * parameters)
      {
        TRACE_FN_ENTER();
        pami_result_t result = _rput->typed (parameters);
        TRACE_FORMAT("result = %d", result);
        TRACE_FN_EXIT();
        return result;
      }

      inline pami_result_t rget (pami_rget_simple_t * parameters)
      {
        TRACE_FN_ENTER();
        pami_result_t result = _rget->simple (parameters);
        TRACE_FORMAT("result = %d", result);
        TRACE_FN_EXIT();
        return result;
      }

      inline pami_result_t rget (pami_rget_typed_t * parameters)
      {
        TRACE_FN_ENTER();
        pami_result_t result = _rget->typed (parameters);
        TRACE_FORMAT("result = %d", result);
        TRACE_FN_EXIT();
        return result;
      }
  };
};
#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif // __components_dispatch_Dispatch_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
