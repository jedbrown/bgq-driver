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
 * \file common/bgq/NativeInterface.h
 * \brief ???
 */

#ifndef __common_bgq_NativeInterface_h__
#define __common_bgq_NativeInterface_h__

#include "Global.h"
#include "algorithms/interfaces/NativeInterface.h"
#include "util/ccmi_debug.h"
#include "util/ccmi_util.h"

#include "components/devices/MulticastModel.h"
#include "components/devices/MultisyncModel.h"
#include "components/devices/MulticombineModel.h"
#include "components/memory/MemoryAllocator.h"
#include "components/devices/generic/Device.h"

#include "util/trace.h"

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#ifdef CCMI_TRACE_ALL
#define DO_TRACE_ENTEREXIT 1
#define DO_TRACE_DEBUG     1
#else
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0
#endif


extern PAMI::Global __global;

namespace PAMI
{


  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, class T_Allocator>
  class BGQNativeInterfaceAS : public CCMI::Interfaces::NativeInterface
  {
    public:
      /// \brief ctor that takes pointers to multi* objects
      inline BGQNativeInterfaceAS(T_Mcast *mcast, T_Msync *msync, T_Mcomb *mcomb, T_Allocator &allocator, pami_client_t client, pami_context_t context, size_t context_id, size_t client_id, int *dispatch_id);

      /// \brief ctor that constructs multi* objects internally
      inline BGQNativeInterfaceAS(T_Device &device, T_Allocator &allocator, pami_client_t client, pami_context_t context, size_t context_id, size_t client_id, int *dispatch_id);

      /// Virtual interfaces (from base \see CCMI::Interfaces::NativeInterface)
      virtual inline pami_result_t destroy      ( ) { return PAMI_SUCCESS; };
      virtual inline pami_result_t multicast    (pami_multicast_t    *, void *devinfo = NULL);
      virtual inline pami_result_t multisync    (pami_multisync_t    *, void *devinfo = NULL);
      virtual inline pami_result_t multicombine (pami_multicombine_t *, void *devinfo = NULL);
      virtual inline pami_result_t manytomany (pami_manytomany_t *, void *devinfo = NULL)
      {
        PAMI_abort();
        return PAMI_ERROR;
      }

      virtual void postWork (pami_context_t         context,
			     int                    color,
			     pami_work_t          * work, 
			     pami_work_function    fn,
			     void                 * clientdata) 
      {
	//PAMI_Context_post(context, work, fn, clientdata);
	Device::Generic::Device *gd = _device.getProgressDevice(color);
        Device::Generic::GenericThread *thread;
	thread = new (work) Device::Generic::GenericThread(fn, clientdata);
	gd->postThread(thread);
      }


      // Model-specific interfaces
      inline pami_result_t multicast    (uint8_t (&)[T_Mcast::sizeof_msg], pami_multicast_t    *, void *devinfo = NULL);
      inline pami_result_t multisync    (uint8_t (&)[T_Msync::sizeof_msg], pami_multisync_t    *, void *devinfo = NULL);
      inline pami_result_t multicombine (uint8_t (&)[T_Mcomb::sizeof_msg], pami_multicombine_t *, void *devinfo = NULL);

      static const size_t multicast_sizeof_msg     = T_Mcast::sizeof_msg;
      static const size_t multisync_sizeof_msg     = T_Msync::sizeof_msg;
      static const size_t multicombine_sizeof_msg  = T_Mcomb::sizeof_msg;

      inline T_Msync          & getMsyncModel () { return _msync; }

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
          BGQNativeInterfaceAS *_ni;
          pami_callback_t       _user_callback;
      };
      T_Allocator &_allocator;  // Allocator

      pami_result_t              _mcast_status;
      pami_result_t              _msync_status;
      pami_result_t              _mcomb_status;

      T_Mcast                 _mcast;
      T_Msync                 _msync;
      T_Mcomb                 _mcomb;

      unsigned                _dispatch;
      pami_client_t           _client;
      pami_context_t          _context;
      size_t                  _contextid;
      size_t                  _clientid;
      int                    *_dispatch_id;
      T_Device               &_device;
  }; // class BGQNativeInterfaceAS

  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, class T_Allocator>
  class BGQNativeInterface : public BGQNativeInterfaceAS<T_Device, T_Mcast, T_Msync, T_Mcomb, T_Allocator>
  {
    public:
      /// \brief ctor that constructs multi* objects internally
      inline BGQNativeInterface(T_Device &device, T_Allocator &allocator, pami_client_t client, pami_context_t context, size_t context_id, size_t client_id, int *dispatch_id);

      /// \brief this call is called when the native interface is
      /// initialized to set the mcast dispatch
      virtual inline pami_result_t setMulticastDispatch (pami_dispatch_multicast_function fn, void *cookie);
      virtual inline pami_result_t setManytomanyDispatch(pami_dispatch_manytomany_function fn, void *cookie)
      {
        PAMI_abort();
        return PAMI_ERROR;
      }

  }; // class BGQNativeInterface

  ///////////////////////////////////////////////////////////////////////////////
  // Inline implementations
  ///////////////////////////////////////////////////////////////////////////////
  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, class T_Allocator>
    inline BGQNativeInterfaceAS<T_Device, T_Mcast, T_Msync, T_Mcomb, T_Allocator>::BGQNativeInterfaceAS(T_Mcast *mcast, T_Msync *msync, T_Mcomb *mcomb, T_Allocator &allocator, pami_client_t client, pami_context_t context, size_t context_id, size_t client_id, int *dispatch_id):
      CCMI::Interfaces::NativeInterface(context_id,PAMI_ENDPOINT_INIT(client,__global.mapping.task(),context_id)),
      _allocator(allocator),
      _mcast(*mcast),
      _msync(*msync),
      _mcomb(*mcomb),
      _dispatch(-1U),
      _client(client),
      _context(context),
      _contextid(context_id),
      _clientid(client_id),
      _dispatch_id(dispatch_id),
      _device(*(T_Device*)NULL)
  {
    TRACE_FN_ENTER();
    COMPILE_TIME_ASSERT(T_Allocator::objsize >= sizeof(allocObj));
    DO_DEBUG((templateName<T_Device>()));
    DO_DEBUG((templateName<T_Mcast>()));
    DO_DEBUG((templateName<T_Msync>()));
    DO_DEBUG((templateName<T_Mcomb>()));
    TRACE_FN_EXIT();
  };

  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, class T_Allocator>
  BGQNativeInterfaceAS<T_Device, T_Mcast, T_Msync, T_Mcomb, T_Allocator>::BGQNativeInterfaceAS(T_Device      &device, 
      T_Allocator   &allocator,
      pami_client_t  client,
      pami_context_t context,
      size_t         context_id,
      size_t         client_id,
      int           *dispatch_id):
      CCMI::Interfaces::NativeInterface(context_id,PAMI_ENDPOINT_INIT(client,__global.mapping.task(),context_id)),
      _allocator(allocator),

      _mcast_status(PAMI_SUCCESS),
      _msync_status(PAMI_SUCCESS),
      _mcomb_status(PAMI_SUCCESS),

      _mcast(client, context, device, _mcast_status),
      _msync(client, context, device, _msync_status),
      _mcomb(client, context, device, _mcomb_status),

      _dispatch(-1U),
      _client(client),
      _context(context),
      _contextid(context_id),
      _clientid(client_id),
      _dispatch_id(dispatch_id),
      _device(device)
  {
    TRACE_FN_ENTER();
    DO_DEBUG((templateName<T_Device>()));
    DO_DEBUG((templateName<T_Mcast>()));
    DO_DEBUG((templateName<T_Msync>()));
    DO_DEBUG((templateName<T_Mcomb>()));
    TRACE_FORMAT( "<%p>%s %d %d %d", this, __PRETTY_FUNCTION__,
                  _mcast_status, _msync_status, _mcomb_status);

    CCMI::Interfaces::NativeInterface::_status = (pami_result_t) (_mcast_status | _msync_status | _mcomb_status);

    TRACE_FN_EXIT();
  };

  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, class T_Allocator>
  BGQNativeInterface<T_Device, T_Mcast, T_Msync, T_Mcomb, T_Allocator>::BGQNativeInterface(T_Device      &device,
											   T_Allocator   &allocator,
                                                                              pami_client_t  client,
                                                                              pami_context_t context,
                                                                              size_t         context_id,
                                                                              size_t         client_id,
                                                                              int           *dispatch_id) :
      BGQNativeInterfaceAS<T_Device, T_Mcast, T_Msync, T_Mcomb, T_Allocator>(device, allocator, client, context, context_id, client_id, dispatch_id)
  {
    TRACE_FN_ENTER();
    DO_DEBUG((templateName<T_Device>()));
    DO_DEBUG((templateName<T_Mcast>()));
    DO_DEBUG((templateName<T_Msync>()));
    DO_DEBUG((templateName<T_Mcomb>()));
    TRACE_FN_EXIT();
  }


  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, class T_Allocator>
    inline void BGQNativeInterfaceAS<T_Device, T_Mcast, T_Msync, T_Mcomb, T_Allocator>::ni_client_done(pami_context_t  context,
      void          *rdata,
      pami_result_t   res)
  {
    TRACE_FN_ENTER();
    allocObj             *obj = (allocObj*)rdata;
    BGQNativeInterfaceAS *ni   = obj->_ni;

    TRACE_FORMAT( "<%p> %p, %p, %d, calling %p(%p)",
                  ni, context, rdata, res,
                  obj->_user_callback.function, obj->_user_callback.clientdata);

    if (obj->_user_callback.function)
      obj->_user_callback.function(context,
                                   obj->_user_callback.clientdata,
                                   res);

    ni->_allocator.returnObject(obj);
    TRACE_FN_EXIT();
  }

  /// \brief this call is called when the native interface is initialized
  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, class T_Allocator>
    inline pami_result_t BGQNativeInterface<T_Device, T_Mcast, T_Msync, T_Mcomb, T_Allocator>::setMulticastDispatch (pami_dispatch_multicast_function fn, void *cookie)
  {
    TRACE_FN_ENTER();
    this->_dispatch = (*this->_dispatch_id)--;

    TRACE_FORMAT( "<%p> %p, %p id=%u",
                  this, fn,  cookie,  this->_dispatch);
    DO_DEBUG((templateName<T_Mcast>()));

    pami_result_t result = this->_mcast.registerMcastRecvFunction(this->_dispatch, fn, cookie);

    PAMI_assert(result == PAMI_SUCCESS);

    TRACE_FN_EXIT();
    return result;
  }

  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, class T_Allocator>
    inline pami_result_t BGQNativeInterfaceAS<T_Device, T_Mcast, T_Msync, T_Mcomb, T_Allocator>::multicast (pami_multicast_t *mcast, void *devinfo)
  {
    TRACE_FN_ENTER();

    pami_result_t rc = PAMI_ERROR;
    //printf("In Multicast\n");    

    rc = _mcast.postMulticastImmediate (_clientid, _contextid, mcast, devinfo);
    //printf ("After Multicast Immediate %d\n", rc);
    if (rc == PAMI_SUCCESS)
      {
        TRACE_FN_EXIT();
        return rc;
      }   

    if (T_Mcast::sizeof_msg > 0) {
      PAMI_assertf(sizeof(allocObj) <= _allocator.objsize,"%zu <= %zu\n",sizeof(allocObj),_allocator.objsize);
      COMPILE_TIME_ASSERT(sizeof(allocObj) <= T_Allocator::objsize);
      allocObj *req          = (allocObj *)_allocator.allocateObject();
      req->_ni               = this;
      req->_user_callback    = mcast->cb_done;
      TRACE_FORMAT( "<%p> %p/%p connection id %u, msgcount %u, bytes %zu, devinfo %p", this, mcast, req, mcast->connection_id, mcast->msgcount, mcast->bytes, devinfo);
      DO_DEBUG((templateName<T_Mcast>()));
      
      //  \todo:  this copy will cause a latency hit, maybe we need to change postMultisync
      //          interface so we don't need to copy
      
      pami_multicast_t  m     = *mcast;
      m.dispatch =  _dispatch; // \todo ? Not really used in C++ objects? /*dispatch must be set via register fn */
      //m.client   =  _clientid;   // \todo ? Why doesn't caller set this?
      //m.context  =  _contextid;// \todo ? Why doesn't caller set this?
    
      m.cb_done.function     =  ni_client_done;
      m.cb_done.clientdata   =  req;
      
      TRACE_FN_EXIT();
      return _mcast.postMulticast_impl(req->_state._mcast, _clientid, _contextid, &m, devinfo);
    }
    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }


  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, class T_Allocator>
    inline pami_result_t BGQNativeInterfaceAS<T_Device, T_Mcast, T_Msync, T_Mcomb, T_Allocator>::multisync(pami_multisync_t *msync, void *devinfo)
  {
    TRACE_FN_ENTER();
    TRACE_FORMAT( "<%p> msync %p, connection id %u, devinfo %p", this, msync, msync->connection_id, devinfo);
    int rc = _msync.postMultisyncImmediate(_clientid, _contextid, msync, devinfo);
    if (rc == PAMI_SUCCESS)
      {
        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }

    if (T_Msync::sizeof_msg > 0) {
      PAMI_assertf(sizeof(allocObj) <= _allocator.objsize,"%zu <= %zu\n",sizeof(allocObj),_allocator.objsize);
      COMPILE_TIME_ASSERT(sizeof(allocObj) <= T_Allocator::objsize);
      allocObj *req          = (allocObj *)_allocator.allocateObject();
      req->_ni               = this;
      req->_user_callback    = msync->cb_done;
      TRACE_FORMAT( "<%p> req %p, cb_done %p/%p", this, req,msync->cb_done.function,msync->cb_done.clientdata);
      DO_DEBUG((templateName<T_Msync>()));
      
      pami_multisync_t  m     = *msync;

      //m.client   =  _clientid;
      //m.context  =  _contextid;

      m.cb_done.function     =  ni_client_done;
      m.cb_done.clientdata   =  req;
      _msync.postMultisync(req->_state._msync, _clientid, _contextid, &m, devinfo);
    }
    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }


  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, class T_Allocator>
    inline pami_result_t BGQNativeInterfaceAS<T_Device, T_Mcast, T_Msync, T_Mcomb, T_Allocator>::multicombine (pami_multicombine_t *mcomb, void *devinfo)
  {
    TRACE_FN_ENTER();
    
    pami_result_t rc = _mcomb.postMulticombineImmediate(_clientid, _contextid, mcomb, devinfo);
    if (rc == PAMI_SUCCESS)
      {
        TRACE_FN_EXIT();
        return rc;
      }
    
    if (T_Mcomb::sizeof_msg > 0) {
      PAMI_assertf(sizeof(allocObj) <= _allocator.objsize,"%zu <= %zu\n",sizeof(allocObj),_allocator.objsize);
      COMPILE_TIME_ASSERT(sizeof(allocObj) <= T_Allocator::objsize);
      allocObj *req          = (allocObj *)_allocator.allocateObject();
      req->_ni               = this;
      req->_user_callback    = mcomb->cb_done;
      TRACE_FORMAT( "<%p> %p/%p connection id %u, count %zu, dt %#X, op %#X, devinfo %p", this, mcomb, req, mcomb->connection_id, mcomb->count, mcomb->dtype, mcomb->optor, devinfo);
      DO_DEBUG((templateName<T_Mcomb>()));
      
      pami_multicombine_t  m     = *mcomb;
      //m.client   =  _clientid;
      //m.context  =  _contextid;
      
      m.cb_done.function     =  ni_client_done;
      m.cb_done.clientdata   =  req;
      
      TRACE_FN_EXIT();
      return _mcomb.postMulticombine(req->_state._mcomb, _clientid, _contextid, &m, devinfo);
    }
    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }

  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, class T_Allocator>
    inline pami_result_t BGQNativeInterfaceAS<T_Device, T_Mcast, T_Msync, T_Mcomb, T_Allocator>::multicast (uint8_t (&state)[T_Mcast::sizeof_msg],
      pami_multicast_t *mcast, void *devinfo)
  {
    TRACE_FN_ENTER();
    TRACE_FORMAT( "<%p> %p,%p connection id %u, msgcount %u, bytes %zu, devinfo %p", this, &state, mcast, mcast->connection_id, mcast->msgcount, mcast->bytes, devinfo);
    DO_DEBUG((templateName<T_Mcast>()));

    mcast->dispatch =  _dispatch;

    TRACE_FN_EXIT();
    return _mcast.postMulticast_impl(state, _clientid, _contextid, mcast, devinfo);
  }

  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, class T_Allocator>
    inline pami_result_t BGQNativeInterfaceAS<T_Device, T_Mcast, T_Msync, T_Mcomb, T_Allocator>::multisync (uint8_t (&state)[T_Msync::sizeof_msg],
											       pami_multisync_t *msync, 
											       void *devinfo)
  {
    TRACE_FN_ENTER();
    TRACE_FORMAT( "<%p> %p,%p connection id %u, devinfo %p", this, &state, msync, msync->connection_id, devinfo);
    DO_DEBUG((templateName<T_Msync>()));

    TRACE_FN_EXIT();
    return _msync.postMultisync_impl(state, _clientid, _contextid, msync, devinfo);
  }

  template <class T_Device, class T_Mcast, class T_Msync, class T_Mcomb, class T_Allocator>
    inline pami_result_t BGQNativeInterfaceAS<T_Device, T_Mcast, T_Msync, T_Mcomb, T_Allocator>::multicombine (uint8_t (&state)[T_Mcomb::sizeof_msg],
      pami_multicombine_t *mcomb, void *devinfo)
  {
    TRACE_FN_ENTER();
    TRACE_FORMAT( "<%p> %p,%p connection id %u, count %zu, dt %#X, op %#X, devinfo %p", this, &state, mcomb, mcomb->connection_id, mcomb->count, mcomb->dtype, mcomb->optor, devinfo);
    DO_DEBUG((templateName<T_Mcomb>()));

    TRACE_FN_EXIT();
    return _mcomb.postMulticombine_impl(state, this->_clientid, this->_contextid, mcomb, devinfo);
  }


  template <class T_Device1, class T_Device2, class T_Mcast1, class T_Mcast2, class T_Msync, class T_Mcomb, class T_Allocator, bool T_Axial=true>
    class BGQNativeInterfaceASMultiDevice : public BGQNativeInterfaceAS <T_Device1, T_Mcast1, T_Msync, T_Mcomb, T_Allocator>
  {
      /// Allocation object to store state and user's callback
      class allocObjMD
      {
      public:
	int                                done_count;
	int                                completion_count;
	unsigned                           total_bytes;
	PAMI::PipeWorkQueue             *  src;
	BGQNativeInterfaceASMultiDevice * _ni;
	pami_callback_t                   _user_callback;	  

	struct _state
	{
	  uint8_t             _mcast1[T_Mcast1::sizeof_msg];
	  uint8_t             _mcast2[T_Mcast2::sizeof_msg];
	} _state;
      };

      /// \brief NativeInterface done function - free allocation and
      /// call client's done
      static void ni_multi_client_done(pami_context_t  context,
                                       void          *rdata,
                                       pami_result_t   res);

      bool                       _advanceLocal;
      bool                       _advanceNetwork;
      bool                       _polling;
      pami_result_t              _mcast2_status;
      T_Device1                & _device1;
      T_Mcast2                   _mcast2;
      pami_work_t                _work;

    public:

      BGQNativeInterfaceASMultiDevice(T_Device1      &device1,
                                      T_Device2      &device2,  
				      T_Allocator    &allocator,
				      pami_client_t  client,
                                      pami_context_t context,
                                      size_t         context_id,
                                      size_t         client_id,
                                      int           *dispatch_id);

      BGQNativeInterfaceASMultiDevice(T_Device1      &device1,
				      T_Allocator    &allocator,
				      pami_client_t  client,
                                      pami_context_t context,
                                      size_t         context_id,
                                      size_t         client_id,
                                      int           *dispatch_id):
      BGQNativeInterfaceAS<T_Device1, T_Mcast1, T_Msync, T_Mcomb, T_Allocator>(device1, allocator, client, context, context_id, client_id, dispatch_id), 
      _device1(device1),
      _mcast2(client, context, *(T_Device2*)NULL, _mcast2_status)
      {
	//TRACE_FORMAT( "Dummy constructor\n");
      }

      virtual pami_result_t multicast (pami_multicast_t *mcast, void *devinfo);

      static pami_result_t advance (pami_context_t     context,
				    void             * cookie) 
      {
	BGQNativeInterfaceASMultiDevice *ni = 
	  (BGQNativeInterfaceASMultiDevice *) cookie;

	int rc = 0;	
	if (ni->_advanceNetwork) 
	  rc |= (int)T_Mcast1::advance (context, &ni->_mcast);	  
	if (ni->_advanceLocal)
	  rc |= (int)T_Mcast2::advance(context, &ni->_mcast2);
	
	if (rc == (int)PAMI_SUCCESS) {
	  ni->_polling        = false;
	  ni->_advanceLocal   = false;
	  ni->_advanceNetwork = false;
	}
	
	return (pami_result_t)rc;
      }

  };

  template <class T_Device1, class T_Device2, class T_Mcast1, class T_Mcast2, class T_Msync, class T_Mcomb, class T_Allocator, bool T_Axial>
    BGQNativeInterfaceASMultiDevice<T_Device1, T_Device2, T_Mcast1, T_Mcast2, T_Msync, T_Mcomb, T_Allocator, T_Axial>::
  BGQNativeInterfaceASMultiDevice(T_Device1      &device1,
                                  T_Device2      &device2,     
				  T_Allocator    &allocator,
				  pami_client_t  client,
                                  pami_context_t context,
                                  size_t         context_id,
                                  size_t         client_id,
                                  int           *dispatch_id)
    : BGQNativeInterfaceAS<T_Device1, T_Mcast1, T_Msync, T_Mcomb, T_Allocator>(device1, allocator, client, context, context_id, client_id, dispatch_id),
      _advanceLocal(false),
      _advanceNetwork(false),
      _polling(false),
      _mcast2_status(PAMI_SUCCESS),
      _device1(device1),
      _mcast2(client, context, device2, _mcast2_status)
  {
    TRACE_FN_ENTER();
    TRACE_FORMAT( "<%p> status %u", this, _mcast2_status);
    this->_mcast.setLocalMulticast(false);    
    this->_mcast.callConsumeBytesOnMaster(false);

    this->CCMI::Interfaces::NativeInterface::_status = (pami_result_t) (this->CCMI::Interfaces::NativeInterface::_status | _mcast2_status);

    this->_mcast.disableAdvance();
    this->_mcast2.disableAdvance();
    
    new (&_work) Device::Generic::GenericThread(advance, this);

    TRACE_FN_EXIT();
  }

  template <class T_Device1, class T_Device2, class T_Mcast1, class T_Mcast2, class T_Msync, class T_Mcomb, class T_Allocator, bool T_Axial>
    inline pami_result_t BGQNativeInterfaceASMultiDevice<T_Device1, T_Device2, T_Mcast1, T_Mcast2, T_Msync, T_Mcomb, T_Allocator, T_Axial>::multicast (pami_multicast_t *mcast, void *devinfo)
  {
    TRACE_FN_ENTER();
    PAMI_assertf(sizeof(allocObjMD) <= this->_allocator.objsize,"%zu <= %zu\n",sizeof(allocObjMD),this->_allocator.objsize);
    COMPILE_TIME_ASSERT(sizeof(allocObjMD) <= T_Allocator::objsize);
    TRACE_FORMAT( "<%p> %p/%p connection id %u, msgcount %u, bytes %zu, devinfo %p", this, mcast, (void*)0, mcast->connection_id, mcast->msgcount, mcast->bytes, devinfo);
    DO_DEBUG((templateName<T_Mcast1>()));
    DO_DEBUG((templateName<T_Mcast2>()));

    /* Check for the master/root */
    PAMI::Topology *root_topo = (PAMI::Topology*)mcast->src_participants;
    PAMI_assert(root_topo);
    TRACE_FORMAT( "<%p> root %p/%u myrank %u", this, root_topo, root_topo ? root_topo->index2Rank(0) : -1, this->endpoint());

    pami_task_t root = root_topo->index2Rank(0);
    pami_multicast_t  m_global;

    //When the root is the same as this->endpoint, then
    //we post multicast to both local and network as its assumed
    //to be a send operation. When they are different its a
    //recv and only posted to where the srctopology points
    if (root == this->endpoint())
    {      
      unsigned any_local_dst_procs = 0, any_nw_dst_procs = 0;
      pami_multicast_t *nmcast = mcast;            
      PAMI::Topology *dst_topology = (PAMI::Topology *)mcast->dst_participants;
      PAMI_assert (dst_topology);

      if (dst_topology->type() == PAMI_AXIAL_TOPOLOGY) {
	pami_coord_t *ll = NULL;
	pami_coord_t *ur = NULL;
	pami_coord_t *ref = NULL;
	unsigned char *isTorus = NULL;
	
	pami_result_t result = PAMI_SUCCESS;
	result = dst_topology->axial(&ll, &ur, &ref, &isTorus);
	PAMI_assert(result == PAMI_SUCCESS);
	
	size_t local_size = ur->u.n_torus.coords[LOCAL_DIM] - 
	  ll->u.n_torus.coords[LOCAL_DIM] + 1;      
	//check for -ve numbers
	any_local_dst_procs  = (1 - local_size) >> 63;	 
	//check for -ve numbers
	any_nw_dst_procs = (local_size - dst_topology->size()) >> 63;  	
      }
      else {
	//PAMI_assert (dst_topology->type() == PAMI_LIST_TOPOLOGY);	
	pami_task_t  rstorage[128], *rlist=rstorage;
	size_t nranks = dst_topology->size();
	dst_topology->list((void**)&rlist);  // endpoint or ranks, don't care since always context 0
	if(rlist==NULL)
	{
	  PAMI_assert(128>=dst_topology->size());
	  dst_topology->getRankList(128, rlist, &nranks);	 
	}

	int mytask = __global.mapping.task();
	for (size_t r = 0; r < nranks; ++r) {
	  if (__global.mapping.isPeer(mytask, rlist[r]))
	    any_local_dst_procs = 1;
	  else
	    any_nw_dst_procs = 1;
	}
      }	

      int ncomplete = any_nw_dst_procs + any_local_dst_procs;  
      if (ncomplete == 2) {
	allocObjMD *req = (allocObjMD *)(this->_allocator).allocateObject(); 
	req->_ni               = this;
	req->_user_callback    = mcast->cb_done;
	req->done_count = 0;
	req->total_bytes = mcast->bytes;
	req->src = (PAMI::PipeWorkQueue*) mcast->src;
	req->completion_count = 2;  
	m_global    = *mcast;
	m_global.dispatch =  this->_dispatch;       
	m_global.cb_done.function =  ni_multi_client_done;
	m_global.cb_done.clientdata =  req;
	nmcast = &m_global;
      }

      if (any_nw_dst_procs)
      {
	_advanceNetwork = true;
	this->_mcast.postMulticastImmediate_impl(this->_clientid, 
						 this->_contextid, 
						 nmcast, devinfo);
      }

      if (any_local_dst_procs)
      {
	_advanceLocal = true;
	_mcast2.postMulticastImmediate_impl(this->_clientid, 
					    this->_contextid, 
					    nmcast, 
					    devinfo);	      
      }      
    }
    else /* is not the master/root */
    {
      bool nw_flag = true;

      if (T_Axial) {
	//This assumes we are the root's tcoord peer on another node
	PAMI::Interface::Mapping::nodeaddr_t rootnode;
	__global.mapping.task2node(root, rootnode);      
	nw_flag = (rootnode.local == __global.mapping.t());
      }
      else 
	nw_flag = !(__global.mapping.isPeer(__global.mapping.task(),
					    root));	
      if (nw_flag)
      {
	_advanceNetwork = true;
	this->_mcast.postMulticastImmediate_impl(this->_clientid, 
						 this->_contextid, 
						 mcast, 
						 devinfo);	
      }
      else //shmem
      {
	_advanceLocal = true;
	_mcast2.postMulticastImmediate_impl(this->_clientid, 
					    this->_contextid, 
					    mcast, 
					    devinfo);	      
      }
    }

    if (!_polling) {
      _polling = true;
      PAMI::Device::Generic::GenericThread *work = (PAMI::Device::Generic::GenericThread *)&_work;
      _device1.getProgressDevice()->postThread(work);    
    }

    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }


  template <class T_Device1, class T_Device2, class T_Mcast1, class T_Mcast2, class T_Msync, class T_Mcomb, class T_Allocator, bool T_Axial>
    inline void BGQNativeInterfaceASMultiDevice<T_Device1, T_Device2, T_Mcast1, T_Mcast2, T_Msync, T_Mcomb, T_Allocator, T_Axial>::ni_multi_client_done(pami_context_t  context,
      void          *rdata,
      pami_result_t   res)
  {
    TRACE_FN_ENTER();
    allocObjMD             *obj = (allocObjMD*)rdata;
    BGQNativeInterfaceASMultiDevice *ni   = obj->_ni;

    TRACE_FORMAT( "<%p> %p, %p, %d calling %p(%p)",
                  ni, context, rdata, res,
                  obj->_user_callback.function, obj->_user_callback.clientdata);

    ++obj->done_count;
    TRACE_FORMAT("calling ni_multi_client_done:%d, %d\n", 
		 obj->done_count, obj->completion_count);
    
    if (obj->done_count == obj->completion_count)
      {
        //Call pipework queue consume bytes
        if (obj->src) obj->src->consumeBytes(obj->total_bytes);

        if (obj->_user_callback.function)
	  obj->_user_callback.function(context,
				       obj->_user_callback.clientdata,
				       res);

        ni->_allocator.returnObject(obj);
      }

    TRACE_FN_EXIT();
  }

  template <class T_Device, class T_Model>
  class BGQNativeInterfaceM2M : public CCMI::Interfaces::NativeInterface {
  protected:
    pami_result_t              _m2m_status;
    
    T_Model                 _model;
    int                     _dispatch;
    pami_client_t           _client;
    pami_context_t          _context;
    size_t                  _contextid;
    size_t                  _clientid;
    int                   * _dispatch_id;

    /// Allocation object to store state and user's callback
    struct allocObj
    {
      uint8_t                 _m2mstate[T_Model::sizeof_msg];
      BGQNativeInterfaceM2M * _ni;
      pami_callback_t         _user_callback;
    };
    PAMI::MemoryAllocator < sizeof(allocObj), 16 > _allocator;  // Allocator    

  public:

    BGQNativeInterfaceM2M<T_Device, T_Model> (T_Device &device, pami_client_t client, pami_context_t context, size_t context_id, size_t client_id, int *dispatch_id):
    CCMI::Interfaces::NativeInterface(context_id,PAMI_ENDPOINT_INIT(client,__global.mapping.task(),context_id)),
      _m2m_status(PAMI_SUCCESS),
      _model(client, context, device, _m2m_status),
      _dispatch(-1U),
      _client(client),
      _context(context),
      _contextid(context_id),
      _clientid(client_id),
      _dispatch_id(dispatch_id),
      _allocator()
    {
      TRACE_FN_ENTER();
      TRACE_FN_EXIT();
    };

    /// Virtual interfaces (from base \see CCMI::Interfaces::NativeInterface)
    virtual inline pami_result_t destroy      ( ) { return PAMI_SUCCESS; };
    virtual pami_result_t setMulticastDispatch (pami_dispatch_multicast_function fn, void *cookie)
    {
      PAMI_abort();
      return PAMI_ERROR;
    }

    //Set the dispatch
    virtual pami_result_t setManytomanyDispatch(pami_dispatch_manytomany_function fn, void *cookie)
    {
      this->_dispatch = (*this->_dispatch_id)--;
      return _model.registerManytomanyRecvFunction_impl(_dispatch, fn, cookie);
    } 
       
    virtual pami_result_t multicast    (pami_multicast_t    *, void *devinfo = NULL)
    {
      PAMI_abort();
      return PAMI_ERROR;
    }    
    
    virtual pami_result_t multisync    (pami_multisync_t    *, void *devinfo = NULL)
    {
      PAMI_abort();
      return PAMI_ERROR;
    }    
    virtual pami_result_t multicombine (pami_multicombine_t *, void *devinfo = NULL)
    {
      PAMI_abort();
      return PAMI_ERROR;
    }        

    virtual pami_result_t manytomany (pami_manytomany_t *m2m, void *devinfo = NULL)
    {
      PAMI_assertf(sizeof(allocObj) <= _allocator.objsize,"%zu <= %zu\n",sizeof(allocObj),_allocator.objsize);
      allocObj *req          = (allocObj *)_allocator.allocateObject();
      req->_ni               = this;
      req->_user_callback    = m2m->cb_done;
      pami_manytomany_t  m   = *m2m;
      m.cb_done.function     =  ni_client_done;
      m.cb_done.clientdata   =  req;

      return _model.postManytomany_impl(req->_m2mstate, &m, devinfo);
    }    

    static void ni_client_done(pami_context_t      context,
			       void              * rdata,
			       pami_result_t       res)
    {
      TRACE_FN_ENTER();
      allocObj             *obj = (allocObj*)rdata;
      BGQNativeInterfaceM2M *ni   = obj->_ni;
      
      if (obj->_user_callback.function)
	obj->_user_callback.function(context,
				     obj->_user_callback.clientdata,
				     res);
      
      ni->_allocator.returnObject(obj);
      TRACE_FN_EXIT();
    }

  };
#if 1
  template <class T_Device, class T_CNShmem>
  class BGQNativeInterfaceCNShmem : public CCMI::Interfaces::NativeInterface
  {
    public:
      /// \brief ctor that takes pointers to multi* objects
      inline BGQNativeInterfaceCNShmem(T_CNShmem *cn_shmem_coll, pami_client_t client, pami_context_t context, size_t context_id, size_t client_id, int *dispatch_id):
        CCMI::Interfaces::NativeInterface(context_id,PAMI_ENDPOINT_INIT(client,__global.mapping.task(),context_id)),
        _cn_shmem_coll(*cn_shmem_coll),
        _dispatch(-1U),
        _client(client),
        _context(context),
        _contextid(context_id),
        _clientid(client_id),
        _dispatch_id(dispatch_id)
    {
      TRACE_FN_ENTER();
      DO_DEBUG((templateName<T_Device>()));
      DO_DEBUG((templateName<T_CNShmem>()));
      TRACE_FN_EXIT();
    };

      /// \brief ctor that constructs multi* objects internally
      inline BGQNativeInterfaceCNShmem(T_Device &device,  pami_client_t client, pami_context_t context, size_t context_id, size_t client_id, int *dispatch_id):
        CCMI::Interfaces::NativeInterface(context_id,PAMI_ENDPOINT_INIT(client,__global.mapping.task(),context_id)),
        _status(PAMI_SUCCESS),
        _cn_shmem_coll(client, context, device, _status),
        _dispatch(-1U),
        _client(client),
        _context(context),
        _contextid(context_id),
        _clientid(client_id),
        _dispatch_id(dispatch_id)
        {
          TRACE_FN_ENTER();
          DO_DEBUG((templateName<T_Device>()));
          DO_DEBUG((templateName<T_CNShmem>()));
          TRACE_FORMAT( "<%p>%s %d ", this, __PRETTY_FUNCTION__, _status);

          CCMI::Interfaces::NativeInterface::_status = (pami_result_t) (_status);

          TRACE_FN_EXIT();
        };

      /// Virtual interfaces (from base \see CCMI::Interfaces::NativeInterface)
      virtual inline pami_result_t destroy      ( ) { return PAMI_SUCCESS; };
      virtual inline pami_result_t multicast    (pami_multicast_t  *mcast, void *devinfo = NULL)
      {
        TRACE_FN_ENTER();

        pami_result_t rc = _cn_shmem_coll.postMulticastImmediate(_clientid, _contextid, mcast, devinfo);
        if (rc == PAMI_SUCCESS)
        {
          TRACE_FN_EXIT();
          return rc;
        }

        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }

      virtual inline pami_result_t multisync    (pami_multisync_t  *msync, void *devinfo = NULL)
      {
        PAMI_abort();
        return PAMI_ERROR;
      }

      virtual inline pami_result_t multicombine (pami_multicombine_t *mcomb, void *devinfo = NULL)
      {
        TRACE_FN_ENTER();

        pami_result_t rc = _cn_shmem_coll.postMulticombineImmediate(_clientid, _contextid, mcomb, devinfo);
        TRACE_FN_EXIT();
        return rc;
      }
      virtual inline pami_result_t manytomany (pami_manytomany_t *, void *devinfo = NULL)
      {
        PAMI_abort();
        return PAMI_ERROR;
      }
#if 0
      // Model-specific interfaces
      inline pami_result_t multicast    (uint8_t (&)[T_Mcast::sizeof_msg], pami_multicast_t    *, void *devinfo = NULL);
      inline pami_result_t multisync    (uint8_t (&)[T_Msync::sizeof_msg], pami_multisync_t    *, void *devinfo = NULL);
      inline pami_result_t multicombine (uint8_t (&)[T_Mcomb::sizeof_msg], pami_multicombine_t *, void *devinfo = NULL);

      static const size_t multicast_sizeof_msg     = T_Mcast::sizeof_msg;
      static const size_t multisync_sizeof_msg     = T_Msync::sizeof_msg;
      static const size_t multicombine_sizeof_msg  = T_Mcomb::sizeof_msg;

      inline T_Msync          & getMsyncModel () { return _msync; }

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
          BGQNativeInterfaceCNShmem *_ni;
          pami_callback_t       _user_callback;
      };

      /// \brief NativeInterface done function - free allocation and call client's done
      static void ni_client_done(pami_context_t  context,
          void          *rdata,
          pami_result_t   res);

      T_Allocator &_allocator;  // Allocator
#endif
    protected:

      pami_result_t              _status;
      T_CNShmem                 _cn_shmem_coll;
      unsigned                _dispatch;
      pami_client_t           _client;
      pami_context_t          _context;
      size_t                  _contextid;
      size_t                  _clientid;
      int                    *_dispatch_id;
  }; // class BGQNativeInterfaceCNShmem
#endif
};

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
