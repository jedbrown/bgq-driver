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
/*
 * \file common/bgp/NativeInterface.h
 * \brief ???
 */

#ifndef __common_bgp_NativeInterface_h__
#define __common_bgp_NativeInterface_h__

#include "Global.h"
#include "algorithms/interfaces/NativeInterface.h"
#include "util/ccmi_util.h"

#include "components/devices/MulticastModel.h"
#include "components/devices/MultisyncModel.h"
#include "components/devices/MulticombineModel.h"

#include "components/memory/MemoryAllocator.h"

#ifndef TRACE_ERR
  #define TRACE_ERR(x) //fprintf x
#endif
extern PAMI::Global __global;

#define DISPATCH_START 0x10

namespace PAMI
{


  template <class T_Mcast, class T_Msync, class T_Mcomb>
  class BGPNativeInterfaceAS : public CCMI::Interfaces::NativeInterface
  {
  public:
    inline BGPNativeInterfaceAS(T_Mcast *mcast, T_Msync *msync, T_Mcomb *mcomb, pami_client_t client, pami_context_t context, size_t context_id, size_t client_id);
    ///
    /// \brief Virtual destructors make compilers happy.
    ///
    virtual inline ~BGPNativeInterfaceAS() {};

    /// Virtual interfaces (from base \see CCMI::Interfaces::NativeInterface)
    virtual inline pami_result_t multicast    (pami_multicast_t    *,void *devinfo=NULL);
    virtual inline pami_result_t multisync    (pami_multisync_t    *,void *devinfo=NULL);
    virtual inline pami_result_t multicombine (pami_multicombine_t *,void *devinfo=NULL);
    virtual inline pami_result_t manytomany (pami_manytomany_t *,void *devinfo=NULL)
    {
      PAMI_abort();
      return PAMI_ERROR;
    }

    // Model-specific interfaces
    inline pami_result_t multicast    (uint8_t (&)[T_Mcast::sizeof_msg], pami_multicast_t    *,void *devinfo=NULL);
    inline pami_result_t multisync    (uint8_t (&)[T_Msync::sizeof_msg], pami_multisync_t    *,void *devinfo=NULL);
    inline pami_result_t multicombine (uint8_t (&)[T_Mcomb::sizeof_msg], pami_multicombine_t *,void *devinfo=NULL);

    static const size_t multicast_sizeof_msg     = T_Mcast::sizeof_msg;
    static const size_t multisync_sizeof_msg     = T_Msync::sizeof_msg;
    static const size_t multicombine_sizeof_msg  = T_Mcomb::sizeof_msg;

  protected:
    /// \brief NativeInterface done function - free allocation and call client's done
    static void ni_client_done(pami_context_t  context,
                               void          *rdata,
                               pami_result_t   res);

    /// Allocation object to store state and user's callback
    class allocObj
    {
    public:
      union
      {
        uint8_t             _mcast[T_Mcast::sizeof_msg];
        uint8_t             _msync[T_Msync::sizeof_msg];
        uint8_t             _mcomb[T_Mcomb::sizeof_msg];
      } _state;
      BGPNativeInterfaceAS *_ni;
      pami_callback_t       _user_callback;
    };

    PAMI::MemoryAllocator < sizeof(allocObj), 16 > _allocator;  // Allocator
    T_Mcast                 _mcast;
    T_Msync                 _msync;
    T_Mcomb                 _mcomb;

    unsigned                _dispatch;
    pami_client_t           _client;
    pami_context_t          _context;
    size_t                  _contextid;
    size_t                  _clientid;
  }; // class BGPNativeInterfaceAS

  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb>
  class BGPNativeInterface : public BGPNativeInterfaceAS<T_Mcast,T_Msync, T_Mcomb>
  {
  public:
    inline BGPNativeInterface(T_Device &device, pami_client_t client, pami_context_t context, size_t context_id, size_t client_id);
    ///
    /// \brief Virtual destructors make compilers happy.
    ///
    virtual inline ~BGPNativeInterface() {};

    /// \brief this call is called when the native interface is
    /// initialized to set the mcast dispatch
    virtual inline pami_result_t setMulticastDispatch (pami_dispatch_multicast_function fn, void *cookie);
    virtual inline pami_result_t setManytomanyDispatch(pami_dispatch_manytomany_function fn, void *cookie)
    {
      PAMI_abort();
      return PAMI_ERROR;
    }

  private:

    pami_result_t              _mcast_status;
    pami_result_t              _msync_status;
    pami_result_t              _mcomb_status;

    T_Mcast                   _mcast_obj;
    T_Msync                   _msync_obj;
    T_Mcomb                   _mcomb_obj;
  }; // class BGPNativeInterface

  ///////////////////////////////////////////////////////////////////////////////
  // Inline implementations
  ///////////////////////////////////////////////////////////////////////////////
  template <class T_Mcast, class T_Msync, class T_Mcomb>
  inline BGPNativeInterfaceAS<T_Mcast, T_Msync, T_Mcomb>::BGPNativeInterfaceAS(T_Mcast *mcast, T_Msync *msync, T_Mcomb *mcomb, pami_client_t client, pami_context_t context, size_t context_id, size_t client_id):
  CCMI::Interfaces::NativeInterface(__global.mapping.task(),
                                    __global.mapping.size()),
  _allocator(),
  _mcast(*mcast),
  _msync(*msync),
  _mcomb(*mcomb),
  _dispatch(DISPATCH_START),
  _client(client),
  _context(context),
  _contextid(context_id),
  _clientid(client_id)
  {
    TRACE_ERR((stderr, "<%p>%s\n", this, __PRETTY_FUNCTION__));

  };

  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb>
  BGPNativeInterface<T_Device,T_Mcast,T_Msync,T_Mcomb>::BGPNativeInterface(T_Device      &device,
                                                                           pami_client_t  client,
                                                                           pami_context_t context,
                                                                           size_t         context_id,
                                                                           size_t         client_id):
  BGPNativeInterfaceAS<T_Mcast, T_Msync, T_Mcomb>(&_mcast_obj, &_msync_obj, &_mcomb_obj, client, context, context_id, client_id),
  _mcast_status(PAMI_SUCCESS),
  _msync_status(PAMI_SUCCESS),
  _mcomb_status(PAMI_SUCCESS),

  _mcast_obj(device, _mcast_status),
  _msync_obj(device, _msync_status),
  _mcomb_obj(device, _mcomb_status)
  {
    TRACE_ERR((stderr, "<%p>%s %d %d %d\n", this, __PRETTY_FUNCTION__,
               _mcast_status, _msync_status, _mcomb_status));

    PAMI_assert(_mcast_status == PAMI_SUCCESS);
    PAMI_assert(_msync_status == PAMI_SUCCESS);
    PAMI_assert(_mcomb_status == PAMI_SUCCESS);
  };

  template <class T_Mcast, class T_Msync, class T_Mcomb>
  inline void BGPNativeInterfaceAS<T_Mcast, T_Msync, T_Mcomb>::ni_client_done(pami_context_t  context,
                                                                         void          *rdata,
                                                                         pami_result_t   res)
  {
    allocObj             *obj = (allocObj*)rdata;
    BGPNativeInterfaceAS *ni   = obj->_ni;

    TRACE_ERR((stderr, "<%p>BGPNativeInterface::ni_client_done(%p, %p, %d) calling %p(%p)\n",
               ni, context, rdata, res,
               obj->_user_callback.function, obj->_user_callback.clientdata));

    if (obj->_user_callback.function)
      obj->_user_callback.function(context,
                                   obj->_user_callback.clientdata,
                                   res);

    ni->_allocator.returnObject(obj);
  }

  /// \brief this call is called when the native interface is initialized
  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb>
  inline pami_result_t BGPNativeInterface<T_Device, T_Mcast, T_Msync, T_Mcomb>::setMulticastDispatch (pami_dispatch_multicast_function fn, void *cookie)
  {
    static size_t dispatch = DISPATCH_START;

    TRACE_ERR((stderr, "<%p>BGPNativeInterface::setDispatch(%p, %p) id=%zu\n",
               this, fn,  cookie,  dispatch));

    pami_result_t result = this->_mcast.registerMcastRecvFunction(dispatch, fn, cookie);

    this->_dispatch = dispatch;
    dispatch ++;
    return result;
  }

  template <class T_Mcast, class T_Msync, class T_Mcomb>
  inline pami_result_t BGPNativeInterfaceAS<T_Mcast, T_Msync, T_Mcomb>::multicast (pami_multicast_t *mcast, void *devinfo)
  {
    allocObj *req          = (allocObj *)_allocator.allocateObject();
    req->_ni               = this;
    req->_user_callback    = mcast->cb_done;
    TRACE_ERR((stderr, "<%p>BGPNativeInterface::multicast(%p/%p) connection id %u, msgcount %u, bytes %zu\n", this, mcast, req, mcast->connection_id, mcast->msgcount, mcast->bytes));
    DO_DEBUG((templateName<T_Mcast>()));

    //  \todo:  this copy will cause a latency hit, maybe we need to change postMultisync
    //          interface so we don't need to copy
    pami_multicast_t  m     = *mcast;

    m.dispatch =  _dispatch; // \todo ? Not really used in C++ objects?

    m.cb_done.function     =  ni_client_done;
    m.cb_done.clientdata   =  req;

    return _mcast.postMulticast(req->_state._mcast, &m);
  }


  template <class T_Mcast, class T_Msync, class T_Mcomb>
  inline pami_result_t BGPNativeInterfaceAS<T_Mcast, T_Msync, T_Mcomb>::multisync(pami_multisync_t *msync, void *devinfo)
  {
    allocObj *req          = (allocObj *)_allocator.allocateObject();
    req->_ni               = this;
    req->_user_callback    = msync->cb_done;
    TRACE_ERR((stderr, "<%p>BGPNativeInterface::multisync(%p/%p) connection id %u\n", this, msync, req, msync->connection_id));
    DO_DEBUG((templateName<T_Msync>()));

    pami_multisync_t  m     = *msync;

    m.cb_done.function     =  ni_client_done;
    m.cb_done.clientdata   =  req;
    _msync.postMultisync(req->_state._msync, &m);
    return PAMI_SUCCESS;
  }


  template <class T_Mcast, class T_Msync, class T_Mcomb>
  inline pami_result_t BGPNativeInterfaceAS<T_Mcast, T_Msync, T_Mcomb>::multicombine (pami_multicombine_t *mcomb, void *devinfo)
  {
    allocObj *req          = (allocObj *)_allocator.allocateObject();
    req->_ni               = this;
    req->_user_callback    = mcomb->cb_done;
    TRACE_ERR((stderr, "<%p>BGPNativeInterface::multicombine(%p/%p) connection id %u, count %zu, dt %#X, op %#X\n", this, mcomb, req, mcomb->connection_id, mcomb->count, mcomb->dtype, mcomb->optor));
    DO_DEBUG((templateName<T_Mcomb>()));

    pami_multicombine_t  m     = *mcomb;

    m.cb_done.function     =  ni_client_done;
    m.cb_done.clientdata   =  req;

    return _mcomb.postMulticombine(req->_state._mcomb, &m);
  }

  template <class T_Mcast, class T_Msync, class T_Mcomb>
  inline pami_result_t BGPNativeInterfaceAS<T_Mcast, T_Msync, T_Mcomb>::multicast (uint8_t (&state)[T_Mcast::sizeof_msg],
                                                                     pami_multicast_t *mcast, void *devinfo)
  {
    TRACE_ERR((stderr, "<%p>BGPNativeInterface::multicast(%p,%p) connection id %u, msgcount %u, bytes %zu\n", this, &state, mcast, mcast->connection_id, mcast->msgcount, mcast->bytes));
    DO_DEBUG((templateName<T_Mcast>()));

    mcast->dispatch =  _dispatch;

    return _mcast.postMulticast_impl(state, mcast);
  }

  template <class T_Mcast, class T_Msync, class T_Mcomb>
  inline pami_result_t BGPNativeInterfaceAS<T_Mcast, T_Msync, T_Mcomb>::multisync (uint8_t (&state)[T_Msync::sizeof_msg],
                                                                     pami_multisync_t *msync,  void *devinfo)
  {
    TRACE_ERR((stderr, "<%p>BGPNativeInterface::multisync(%p,%p) connection id %u\n", this, &state, msync, msync->connection_id));
    DO_DEBUG((templateName<T_Msync>()));

    return _msync.postMultisync_impl(state, msync);
  }

  template <class T_Mcast, class T_Msync, class T_Mcomb>
  inline pami_result_t BGPNativeInterfaceAS<T_Mcast, T_Msync, T_Mcomb>::multicombine (uint8_t (&state)[T_Mcomb::sizeof_msg],
                                                                        pami_multicombine_t *mcomb,  void *devinfo)
  {
    TRACE_ERR((stderr, "<%p>BGPNativeInterface::multicombine(%p,%p) connection id %u, count %zu, dt %#X, op %#X\n", this, &state, mcomb, mcomb->connection_id, mcomb->count, mcomb->dtype, mcomb->optor));
    DO_DEBUG((templateName<T_Mcomb>()));

    return _mcomb.postMulticombine_impl(state, mcomb);
  }


};

#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
