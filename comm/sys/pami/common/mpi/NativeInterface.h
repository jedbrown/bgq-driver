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
 * \file common/mpi/NativeInterface.h
 * \brief
 */

#ifndef __common_mpi_NativeInterface_h__
#define __common_mpi_NativeInterface_h__

#include "Global.h"
#include "algorithms/interfaces/NativeInterface.h"
#include "util/ccmi_util.h"

#include "components/devices/MulticastModel.h"
#include "components/devices/MultisyncModel.h"
#include "components/devices/MulticombineModel.h"

#include "components/memory/MemoryAllocator.h"

#undef TRACE_ERR
  #define TRACE_ERR(x) //fprintf x

extern PAMI::Global __global;

#define DISPATCH_START 0x10

namespace PAMI
{
  typedef enum MPINativeInterfaceSemantics
  {
    OneSided=0,
    AllSided,
  }MPINativeInterfaceSemantics;
  static size_t        __g_mpi_dispatch = DISPATCH_START-1;




  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb,  int T_Semantics=OneSided>
  class MPINativeInterface : public CCMI::Interfaces::NativeInterface
  {
  public:
    inline MPINativeInterface(T_Device &device, pami_client_t client, pami_context_t context, size_t context_id, size_t client_id);

    /// \brief this call is called when the native interface is
    /// initialized to set the mcast dispatch
    virtual inline pami_result_t setMulticastDispatch (pami_dispatch_multicast_function fn, void *cookie);
    virtual inline pami_result_t setManytomanyDispatch(pami_dispatch_manytomany_function fn, void *cookie)
    {
      //PAMI_abort();
      return PAMI_ERROR;
    }
    virtual inline pami_result_t setSendDispatch(pami_dispatch_p2p_function fn,
                                                 void *cookie)
      {
        PAMI_abort();
        return PAMI_ERROR;
      }
    virtual inline pami_result_t setSendPWQDispatch(pami_dispatch_pwq_function fn,
                                                    void *cookie)
      {
        PAMI_abort();
        return PAMI_ERROR;
      }
    /// Virtual interfaces (from base \see CCMI::Interfaces::NativeInterface)
    virtual inline pami_result_t multicast    (pami_multicast_t    *, void* devInfo=NULL);
    virtual inline pami_result_t multisync    (pami_multisync_t    *, void* devInfo=NULL);
    virtual inline pami_result_t multicombine (pami_multicombine_t *, void* devInfo=NULL);
    virtual inline pami_result_t manytomany (pami_manytomany_t *, void* devInfo=NULL)
    {
      PAMI_abort();
      return PAMI_ERROR;
    }
    virtual inline pami_result_t send (pami_send_t * parameters)
      {
        PAMI_abort();
        return PAMI_ERROR;
      }
    virtual inline pami_result_t sendPWQ(pami_context_t       context,
                                           pami_endpoint_t      dest,
                                           unsigned             connection_Id,
                                           size_t               header_length,
                                           void                *header,
                                           size_t               length,
                                           PAMI::PipeWorkQueue *pwq,
                                           pami_send_event_t   *events)
      {
        PAMI_abort();
        return PAMI_ERROR;
      }

    // Model-specific interfaces
    inline pami_result_t multicast    (uint8_t (&)[T_Mcast::sizeof_msg], pami_multicast_t    *, void* devInfo=NULL);
    inline pami_result_t multisync    (uint8_t (&)[T_Msync::sizeof_msg], pami_multisync_t    *, void* devInfo=NULL);
    inline pami_result_t multicombine (uint8_t (&)[T_Mcomb::sizeof_msg], pami_multicombine_t *, void* devInfo=NULL);

    static const size_t multicast_sizeof_msg     = T_Mcast::sizeof_msg;
    static const size_t multisync_sizeof_msg     = T_Msync::sizeof_msg;
    static const size_t multicombine_sizeof_msg  = T_Mcomb::sizeof_msg;

  private:
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
      MPINativeInterface *_ni;
      pami_callback_t      _user_callback;
    };

    PAMI::MemoryAllocator<sizeof(allocObj),16> _allocator;  // Allocator

    pami_result_t              _mcast_status;
    pami_result_t              _msync_status;
    pami_result_t              _mcomb_status;

    T_Mcast                   _mcast;
    T_Msync                   _msync;
    T_Mcomb                   _mcomb;

    unsigned                  _dispatch;
    pami_client_t              _client;
    pami_context_t             _context;
    size_t                    _contextid;
    size_t                    _clientid;
  }; // class MPINativeInterface

  ///////////////////////////////////////////////////////////////////////////////
  // Inline implementations
  ///////////////////////////////////////////////////////////////////////////////
  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, int T_Semantics>
  MPINativeInterface<T_Device,T_Mcast,T_Msync,T_Mcomb, T_Semantics>::MPINativeInterface(T_Device      &device,
                       pami_client_t   client,
                       pami_context_t  context,
                       size_t         context_id,
                       size_t         client_id):
      CCMI::Interfaces::NativeInterface(__global.mapping.task(),
                                        __global.mapping.size()),
  _allocator(),

  _mcast_status(PAMI_SUCCESS),
  _msync_status(PAMI_SUCCESS),
  _mcomb_status(PAMI_SUCCESS),

  _mcast(device,_mcast_status),
  _msync(device,_msync_status),
  _mcomb(device,_mcomb_status),

      _dispatch(0),
      _client(client),
      _context(context),
      _contextid(context_id),
      _clientid(client_id)
      {
    TRACE_ERR((stderr, "<%p>%s %d %d %d\n", this, __PRETTY_FUNCTION__,
               _mcast_status, _msync_status, _mcomb_status));
    PAMI_assert(_mcast_status == PAMI_SUCCESS);
    PAMI_assert(_msync_status == PAMI_SUCCESS);
    PAMI_assert(_mcomb_status == PAMI_SUCCESS);
  }

  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, int T_Semantics>
  inline void MPINativeInterface<T_Device,T_Mcast,T_Msync,T_Mcomb, T_Semantics>::ni_client_done(pami_context_t  context,
                                                                                   void          *rdata,
                                                                                   pami_result_t   res)
  {
    allocObj           *obj = (allocObj*)rdata;
    MPINativeInterface *ni   = obj->_ni;

    TRACE_ERR((stderr, "<%p>MPINativeInterface::ni_client_done(%p, %p, %d) calling %p(%p)\n",
               ni, context, rdata, res,
               obj->_user_callback.function, obj->_user_callback.clientdata));

    if (obj->_user_callback.function)
      obj->_user_callback.function(context,
                                   obj->_user_callback.clientdata,
                                   res);
    ni->_allocator.returnObject(obj);
      }

    /// \brief this call is called when the native interface is initialized
  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, int T_Semantics>
  inline pami_result_t MPINativeInterface<T_Device,T_Mcast,T_Msync,T_Mcomb, T_Semantics>::setMulticastDispatch (pami_dispatch_multicast_function fn, void *cookie)
      {
      __g_mpi_dispatch++;
      _dispatch=__g_mpi_dispatch;
    TRACE_ERR((stderr, "<%p>MPINativeInterface::setDispatch(%p, %p) id=%zu\n",
               this, fn,  cookie,  _dispatch));

    pami_result_t result = _mcast.registerMcastRecvFunction(_dispatch, fn, cookie);

    return result;
    }

  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, int T_Semantics>
  inline pami_result_t MPINativeInterface<T_Device,T_Mcast,T_Msync,T_Mcomb, T_Semantics>::multicast (pami_multicast_t *mcast, void *devinfo)
      {

    allocObj *req          = (allocObj *)_allocator.allocateObject();
    req->_ni               = this;
    req->_user_callback    = mcast->cb_done;
    TRACE_ERR((stderr, "<%p>MPINativeInterface::multicast(%p) %p\n",
               this, mcast, req));

    //  \todo:  this copy will cause a latency hit, maybe we need to change postMultisync
    //          interface so we don't need to copy
    pami_multicast_t  m     = *mcast;

    m.dispatch =  _dispatch; // \todo ? Not really used in C++ objects?

    m.cb_done.function     =  ni_client_done;
    m.cb_done.clientdata   =  req;

    return _mcast.postMulticast(req->_state._mcast, _clientid, _contextid, &m, devinfo);
      }


  // Multisync Code

  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, int T_Semantics>
  inline pami_result_t MPINativeInterface<T_Device,T_Mcast,T_Msync,T_Mcomb, T_Semantics>::multisync(pami_multisync_t *msync, void *devinfo)
      {
    allocObj *req          = (allocObj *)_allocator.allocateObject();
        req->_ni               = this;
        req->_user_callback    = msync->cb_done;
    TRACE_ERR((stderr, "<%p>MPINativeInterface::multisync(%p) %p\n", this, msync, req));

        pami_multisync_t  m     = *msync;

    m.cb_done.function     =  ni_client_done;
        m.cb_done.clientdata   =  req;
    _msync.postMultisync(req->_state._msync, _clientid, _contextid, &m, devinfo);
        return PAMI_SUCCESS;
      }


  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, int T_Semantics>
  inline pami_result_t MPINativeInterface<T_Device,T_Mcast,T_Msync,T_Mcomb, T_Semantics>::multicombine (pami_multicombine_t *mcomb, void *devinfo)
  {
    allocObj *req          = (allocObj *)_allocator.allocateObject();
    req->_ni               = this;
    req->_user_callback    = mcomb->cb_done;
    TRACE_ERR((stderr, "<%p>MPINativeInterface::multicombine(%p) %p\n",
               this, mcomb, req));

    pami_multicombine_t  m     = *mcomb;

    m.cb_done.function     =  ni_client_done;
    m.cb_done.clientdata   =  req;

    return _mcomb.postMulticombine(req->_state._mcomb, _clientid, _contextid, &m, devinfo);
  }

  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, int T_Semantics>
  inline pami_result_t  MPINativeInterface<T_Device,T_Mcast,T_Msync,T_Mcomb, T_Semantics>::multicast (uint8_t (&state)[T_Mcast::sizeof_msg],
                                                                                                      pami_multicast_t *mcast, void *devinfo)
      {
    TRACE_ERR((stderr, "<%p>MPINativeInterface::multicast(%p, %p)\n",
               this, &state, mcast));
    mcast->dispatch =  _dispatch;

    return _mcast.postMulticast_impl(state, _clientid, _contextid, mcast, devinfo);
      }

  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, int T_Semantics>
  inline pami_result_t  MPINativeInterface<T_Device,T_Mcast,T_Msync,T_Mcomb, T_Semantics>::multisync (uint8_t (&state)[T_Msync::sizeof_msg],
                                                                                                      pami_multisync_t *msync, void *devinfo)
  {
    TRACE_ERR((stderr, "<%p>MPINativeInterface::multisync(%p, %p)\n",
               this, &state, msync));

    return _msync.postMultisync_impl(state, _clientid, _contextid, msync, devinfo);
  }

  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, int T_Semantics>
  inline pami_result_t  MPINativeInterface<T_Device,T_Mcast,T_Msync,T_Mcomb, T_Semantics>::multicombine (uint8_t (&state)[T_Mcomb::sizeof_msg],
                                                                                                         pami_multicombine_t *mcomb, void *devinfo)
  {
    TRACE_ERR((stderr, "<%p>MPINativeInterface::multicombine(%p, %p)\n",
               this, &state, mcomb));

    return _mcomb.postMulticombine_impl(state, _clientid, _contextid, mcomb, devinfo);
  }




};

#endif
