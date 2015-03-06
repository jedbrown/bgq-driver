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
/* * \file components/devices/NativeInterface.h
 * \brief ???
 */

#ifndef __components_devices_NativeInterface_h__
#define __components_devices_NativeInterface_h__

#include "algorithms/interfaces/NativeInterface.h"
#include "util/ccmi_util.h"

#include "components/devices/MulticastModel.h"
#include "components/devices/MultisyncModel.h"
#include "components/devices/MulticombineModel.h"
#include "components/devices/ManytomanyModel.h"
#include "components/memory/MemoryAllocator.h"

#undef TRACE_ERR
#define TRACE_ERR(x) //fprintf x

namespace PAMI
{
  namespace Device
  {
    template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb>
    class DeviceNativeInterface : public CCMI::Interfaces::NativeInterface
    {
    public:
      inline DeviceNativeInterface(T_Device       &device,
                                   pami_client_t   client,
                                   pami_context_t  context,
                                   size_t          context_id,
                                   size_t          client_id,
                                   int            *dispatch_id);

      virtual inline pami_result_t setMulticastDispatch (pami_dispatch_multicast_function fn,
                                                         void                       *cookie)
        {
          _my_dispatch_id = (*_dispatch_id)--;
          return _mcast.registerMcastRecvFunction(_my_dispatch_id,fn,cookie);
        }

      virtual inline pami_result_t setManytomanyDispatch(pami_dispatch_manytomany_function fn, void *cookie)
      {
        PAMI_abort();
        return PAMI_ERROR;
      }
      virtual inline pami_result_t setSendDispatch(pami_dispatch_p2p_function fn,
                                                   void                      *cookie)
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
      virtual inline pami_result_t destroy      ( );
      virtual inline pami_result_t multicast    (pami_multicast_t    *, void *devinfo=NULL);
      virtual inline pami_result_t multisync    (pami_multisync_t    *, void *devinfo=NULL);
      virtual inline pami_result_t multicombine (pami_multicombine_t *, void *devinfo=NULL);
      virtual inline pami_result_t manytomany   (pami_manytomany_t   *, void *devinfo=NULL)
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
      inline pami_result_t multicast    (uint8_t (&)[T_Mcast::sizeof_msg], pami_multicast_t    *, void *devinfo=NULL);
      inline pami_result_t multisync    (uint8_t (&)[T_Msync::sizeof_msg], pami_multisync_t    *, void *devinfo=NULL);
      inline pami_result_t multicombine (uint8_t (&)[T_Mcomb::sizeof_msg], pami_multicombine_t *, void *devinfo=NULL);
      static const size_t  multicast_sizeof_msg     = T_Mcast::sizeof_msg;
      static const size_t  multisync_sizeof_msg     = T_Msync::sizeof_msg;
      static const size_t  multicombine_sizeof_msg  = T_Mcomb::sizeof_msg;

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
          uint8_t              _mcast[T_Mcast::sizeof_msg];
          uint8_t              _msync[T_Msync::sizeof_msg];
          uint8_t              _mcomb[T_Mcomb::sizeof_msg];
        } _state;
        DeviceNativeInterface *_ni;
        pami_callback_t        _user_callback;
      };

      PAMI::MemoryAllocator<sizeof(allocObj),16> _allocator;  // Allocator

      pami_result_t              _mcast_status;
      pami_result_t              _msync_status;
      pami_result_t              _mcomb_status;

      T_Mcast                    _mcast;
      T_Msync                    _msync;
      T_Mcomb                    _mcomb;

      int                       *_dispatch_id;
      int                        _my_dispatch_id;
      pami_client_t              _client;
      pami_context_t             _context;
      size_t                     _contextid;
      size_t                     _clientid;
    }; // class DeviceNativeInterface

    ///////////////////////////////////////////////////////////////////////////////
    // Inline implementations
    ///////////////////////////////////////////////////////////////////////////////
    template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb>
    DeviceNativeInterface<T_Device,T_Mcast,T_Msync,T_Mcomb>::DeviceNativeInterface(T_Device       &device,
                                                                                                pami_client_t   client,
                                                                                                pami_context_t  context,
                                                                                                size_t          context_id,
                                                                                                size_t          client_id,
                                                                                                int            *dispatch_id):
      CCMI::Interfaces::NativeInterface(context_id, PAMI_ENDPOINT_INIT(client, __global.mapping.task(), context_id)),
      _allocator(),
      _mcast_status(PAMI_SUCCESS),
      _msync_status(PAMI_SUCCESS),
      _mcomb_status(PAMI_SUCCESS),
      _mcast(device,_mcast_status),
      _msync(device,_msync_status),
      _mcomb(device,_mcomb_status),
      _dispatch_id(dispatch_id),
      _client(client),
      _context(context),
      _contextid(context_id),
      _clientid(client_id)
    {
    }

    template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb>
    inline void DeviceNativeInterface<T_Device,
                                      T_Mcast,
                                      T_Msync,
                                      T_Mcomb>::ni_client_done(pami_context_t  context,
                                                               void          *rdata,
                                                               pami_result_t   res)
    {
      allocObj              *obj  = (allocObj*)rdata;
      DeviceNativeInterface *ni   = obj->_ni;
      if (obj->_user_callback.function)
        obj->_user_callback.function(context,
                                     obj->_user_callback.clientdata,
                                     res);
      ni->_allocator.returnObject(obj);
    }

    template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb>
    inline pami_result_t DeviceNativeInterface<T_Device,
                                               T_Mcast,
                                               T_Msync,
                                               T_Mcomb>::destroy ( )
    {
      this->~DeviceNativeInterface();
      return PAMI_SUCCESS;
    }

    template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb>
    inline pami_result_t DeviceNativeInterface<T_Device,
                                               T_Mcast,
                                               T_Msync,
                                               T_Mcomb>::multicast (pami_multicast_t *mcast,
                                                                    void             *devinfo)
    {
      COMPILE_TIME_ASSERT((sizeof(allocObj) <= PAMI::MemoryAllocator < sizeof(allocObj), 16 >::objsize));
      allocObj *req          = (allocObj *)_allocator.allocateObject();
      req->_ni               = this;
      req->_user_callback    = mcast->cb_done;

      pami_multicast_t  m    = *mcast;
      m.dispatch             =  _my_dispatch_id;
      //m.client               =  _clientid;
      //m.context              =  _contextid;

      m.cb_done.function     =  ni_client_done;
      m.cb_done.clientdata   =  req;
      return _mcast.postMulticast(req->_state._mcast, _clientid, _contextid, &m, devinfo);
    }

    template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb>
    inline pami_result_t DeviceNativeInterface<T_Device,
                                               T_Mcast,
                                               T_Msync,
                                               T_Mcomb>::multisync(pami_multisync_t *msync,
                                                                   void             *devinfo)
    {
      COMPILE_TIME_ASSERT((sizeof(allocObj) <= PAMI::MemoryAllocator < sizeof(allocObj), 16 >::objsize));
      allocObj         *req   = (allocObj *)_allocator.allocateObject();
      req->_ni                = this;
      req->_user_callback     = msync->cb_done;

      pami_multisync_t  m     = *msync;
      //m.client                =  _clientid;
      //m.context               =  _contextid;
      m.cb_done.function      =  ni_client_done;
      m.cb_done.clientdata    =  req;
      _msync.postMultisync(req->_state._msync, _clientid, _contextid, &m, devinfo);
      return PAMI_SUCCESS;
    }


    template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb>
    inline pami_result_t DeviceNativeInterface<T_Device,
                                               T_Mcast,
                                               T_Msync,
                                               T_Mcomb>::multicombine (pami_multicombine_t *mcomb,
                                                                       void                *devinfo)
    {
      COMPILE_TIME_ASSERT((sizeof(allocObj) <= PAMI::MemoryAllocator < sizeof(allocObj), 16 >::objsize));
      allocObj            *req = (allocObj *)_allocator.allocateObject();
      req->_ni                 = this;
      req->_user_callback      = mcomb->cb_done;

      pami_multicombine_t  m   = *mcomb;
      //m.client                 =  _clientid;
      //m.context                =  _contextid;
      m.cb_done.function       =  ni_client_done;
      m.cb_done.clientdata     =  req;
      return _mcomb.postMulticombine(req->_state._mcomb, _clientid, _contextid, &m, devinfo);
    }

    template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb>
    inline pami_result_t  DeviceNativeInterface<T_Device,T_Mcast,T_Msync,T_Mcomb>::multicast (uint8_t (&state)[T_Mcast::sizeof_msg],
                                                                                                           pami_multicast_t *mcast,
                                                                                                           void             *devinfo)
    {
      PAMI_assert(_mcast_status == PAMI_SUCCESS);
      mcast->dispatch =  _my_dispatch_id;
      return _mcast.postMulticast_impl(state, _clientid, _contextid, mcast, devinfo);
    }

    template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb>
    inline pami_result_t  DeviceNativeInterface<T_Device,T_Mcast,T_Msync,T_Mcomb>::multisync (uint8_t (&state)[T_Msync::sizeof_msg],
                                                                                                           pami_multisync_t *msync,
                                                                                                           void             *devinfo)
    {
      PAMI_assert(_msync_status == PAMI_SUCCESS);
      return _msync.postMultisync_impl(state, _clientid, _contextid, msync, devinfo);
    }

    template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb>
    inline pami_result_t  DeviceNativeInterface<T_Device,T_Mcast,T_Msync,T_Mcomb>::multicombine (uint8_t (&state)[T_Mcomb::sizeof_msg],
                                                                                                              pami_multicombine_t *mcomb,
                                                                                                              void                *devinfo)
    {
      PAMI_assert(_mcomb_status == PAMI_SUCCESS);
      return _mcomb.postMulticombine_impl(state, _clientid, _contextid, mcomb, devinfo);
    }





    template <class T_Model>
    class CSNativeInterface : public CCMI::Interfaces::NativeInterface
    {
    public:
      inline CSNativeInterface(T_Model       &model,
                               pami_client_t  client,
                               size_t         client_id,
                               pami_context_t context,
                               size_t         context_id,
                               pami_task_t    tasks,
                               size_t         num_tasks);
      ~CSNativeInterface();
      virtual inline pami_result_t destroy       ();
      virtual inline pami_result_t multicast    (pami_multicast_t    *, void *devinfo=NULL);
      virtual inline pami_result_t multisync    (pami_multisync_t    *, void *devinfo=NULL);
      virtual inline pami_result_t multicombine (pami_multicombine_t *, void *devinfo=NULL);
      virtual inline pami_result_t manytomany (pami_manytomany_t *, void *devinfo=NULL)
        {
          PAMI_abort();
          return PAMI_ERROR;
        }
      inline pami_result_t multicast    (uint8_t (&)[T_Model::sizeof_multicast_msg], pami_multicast_t    *, void *devinfo=NULL);
      inline pami_result_t multisync    (uint8_t (&)[T_Model::sizeof_multisync_msg], pami_multisync_t    *, void *devinfo=NULL);
      inline pami_result_t multicombine (uint8_t (&)[T_Model::sizeof_multicombine_msg], pami_multicombine_t *, void *devinfo=NULL);

      static const size_t sizeof_multicast_msg     = T_Model::sizeof_multicast_msg;
      static const size_t sizeof_multisync_msg     = T_Model::sizeof_multisync_msg;
      static const size_t sizeof_multicombine_msg  = T_Model::sizeof_multicombine_msg;

    protected:
      static void ni_client_done(pami_context_t  context,
                                 void          *rdata,
                                 pami_result_t   res);
      class allocObj
      {
      public:
        union
        {
          uint8_t             _mcast[T_Model::sizeof_multicast_msg];
          uint8_t             _msync[T_Model::sizeof_multisync_msg];
          uint8_t             _mcomb[T_Model::sizeof_multicombine_msg];
        } _state;
        CSNativeInterface     *_ni;
        pami_callback_t       _user_callback;
      };

      PAMI::MemoryAllocator < sizeof(allocObj), 16 > _allocator;
      T_Model                 &_model;
      pami_client_t           _client;
      pami_context_t          _context;
      size_t                  _contextid;
      size_t                  _clientid;
    }; // class CSNativeInterface

    ///////////////////////////////////////////////////////////////////////////////
    // Inline implementations
    ///////////////////////////////////////////////////////////////////////////////
    template <class T_Model>
    inline CSNativeInterface<T_Model>::CSNativeInterface(T_Model         &model,
                                                         pami_client_t     client,
                                                         size_t            client_id,
                                                         pami_context_t    context,
                                                         size_t            context_id,
                                                         pami_task_t       task_id,
                                                         size_t            num_tasks):
      CCMI::Interfaces::NativeInterface(context_id, PAMI_ENDPOINT_INIT(client, __global.mapping.task(), context_id)),
      _allocator(),
      _model(model),
      _client(client),
      _context(context),
      _contextid(context_id),
      _clientid(client_id)
    {
    };

    template <class T_Model>
    inline CSNativeInterface<T_Model>::~CSNativeInterface()
    {
    }

    template <class T_Model>
    inline void CSNativeInterface<T_Model>::ni_client_done(pami_context_t  context,
                                                           void          *rdata,
                                                           pami_result_t   res)
    {
      allocObj             *obj = (allocObj*)rdata;
      CSNativeInterface    *ni   = obj->_ni;

      if (obj->_user_callback.function)
        obj->_user_callback.function(context,
                                     obj->_user_callback.clientdata,
                                     res);
      ni->_allocator.returnObject(obj);
    }

    template <class T_Model>
    inline pami_result_t CSNativeInterface<T_Model>::destroy ( )
    {
      this->~CSNativeInterface();

      return PAMI_SUCCESS;
    }

    template <class T_Model>
    inline pami_result_t CSNativeInterface<T_Model>::multicast (pami_multicast_t *mcast,
                                                                void             *devinfo)
    {
      COMPILE_TIME_ASSERT((sizeof(allocObj) <= PAMI::MemoryAllocator < sizeof(allocObj), 16 >::objsize));
      allocObj *req          = (allocObj *)_allocator.allocateObject();
      req->_ni               = this;
      req->_user_callback    = mcast->cb_done;
      DO_DEBUG((templateName<T_Model>()));
      //  \todo:  this copy will cause a latency hit, maybe we need to change postMultisync
      //          interface so we don't need to copy
      // pami_multicast_t  m     = *mcast;
      pami_multicast_t&  m     = *mcast;

      //m.client   =  _clientid;   // \todo ? Why doesn't caller set this?
      //m.context  =  _contextid;// \todo ? Why doesn't caller set this?

      m.cb_done.function     =  ni_client_done;
      m.cb_done.clientdata   =  req;

      return _model.postMulticast(req->_state._mcast, _clientid, _contextid, &m, devinfo);
    }


    template <class T_Model>
    inline pami_result_t CSNativeInterface<T_Model>::multisync(pami_multisync_t *msync,
                                                               void             *devinfo)
    {
      COMPILE_TIME_ASSERT((sizeof(allocObj) <= PAMI::MemoryAllocator < sizeof(allocObj), 16 >::objsize));
      allocObj *req          = (allocObj *)_allocator.allocateObject();
      req->_ni               = this;
      req->_user_callback    = msync->cb_done;
      DO_DEBUG((templateName<T_Model>()));

      // pami_multisync_t  m     = *msync;
      pami_multisync_t&  m     = *msync;

      //m.client   =  _clientid;
      //m.context  =  _contextid;

      m.cb_done.function     =  ni_client_done;
      m.cb_done.clientdata   =  req;
      _model.postMultisync(req->_state._msync, _clientid, _contextid, &m, devinfo);
      return PAMI_SUCCESS;
    }


    template <class T_Model>
    inline pami_result_t CSNativeInterface<T_Model>::multicombine (pami_multicombine_t *mcomb,
                                                                   void                *devinfo)
    {
      COMPILE_TIME_ASSERT((sizeof(allocObj) <= PAMI::MemoryAllocator < sizeof(allocObj), 16 >::objsize));
      allocObj *req          = (allocObj *)_allocator.allocateObject();
      req->_ni               = this;
      req->_user_callback    = mcomb->cb_done;
      DO_DEBUG((templateName<T_Model>()));

      // pami_multicombine_t&  m     = *mcomb;
      pami_multicombine_t&  m     = *mcomb;

      //m.client   =  _clientid;
      //m.context  =  _contextid;

      m.cb_done.function     =  ni_client_done;
      m.cb_done.clientdata   =  req;

      return _model.postMulticombine(req->_state._mcomb, _clientid, _contextid, &m, devinfo);
    }

    template <class T_Model>
    inline pami_result_t CSNativeInterface<T_Model>::multicast (uint8_t (&state)[T_Model::sizeof_multicast_msg],
                                                                pami_multicast_t *mcast,
                                                                void *devinfo)
    {
      DO_DEBUG((templateName<T_Model>()));
      return _model.postMulticast_impl(state, _clientid, _contextid, mcast, devinfo);
    }

    template <class T_Model>
    inline pami_result_t CSNativeInterface<T_Model>::multisync (uint8_t (&state)[T_Model::sizeof_multisync_msg],
                                                                pami_multisync_t *msync,
                                                                void             *devinfo)
    {
      DO_DEBUG((templateName<T_Model>()));

      return _model.postMultisync_impl(state, _clientid, _contextid, msync,devinfo);
    }

    template <class T_Model>
    inline pami_result_t CSNativeInterface<T_Model>::multicombine (uint8_t (&state)[T_Model::sizeof_multicombine_msg],
                                                                   pami_multicombine_t *mcomb,
                                                                   void                *devinfo)
    {
      DO_DEBUG((templateName<T_Model>()));

      return _model.postMulticombine_impl(state, _clientid, _contextid, mcomb, devinfo);
    }
  };
};

#endif
