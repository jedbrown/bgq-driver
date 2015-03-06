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
/// \file common/bgp/Context.h
/// \brief PAMI BGP specific context implementation.
///
#ifndef __common_bgp_Context_h__
#define __common_bgp_Context_h__

#include <stdlib.h>
#include <string.h>

#include <pami.h>
#include "common/ContextInterface.h"

#include "components/devices/generic/Device.h"
#include "components/devices/misc/ProgressFunctionMsg.h"
#include "components/devices/misc/AtomicBarrierMsg.h"
#include "components/devices/workqueue/WQRingReduceMsg.h"
#include "components/devices/workqueue/WQRingBcastMsg.h"
#include "components/devices/workqueue/LocalAllreduceWQMessage.h"
#include "components/devices/workqueue/LocalReduceWQMessage.h"
#include "components/devices/workqueue/LocalBcastWQMessage.h"

// BGP-specific devices...
#include "components/devices/bgp/global_interrupt/GIBarrierMsg.h"
//#include "components/devices/bgp/collective_network/CNAllreduceMsg.h"
//#include "components/devices/bgp/collective_network/CNAllreduceShortMsg.h"
//#include "components/devices/bgp/collective_network/CNAllreducePPMsg.h"
//#include "components/devices/bgp/collective_network/CNAllreduceSum2PMsg.h"
//#include "components/devices/bgp/collective_network/CNBroadcastMsg.h"

#include "components/devices/shmem/ShmemDevice.h"
#include "components/devices/shmem/ShmemPacketModel.h"
#include "components/devices/shmem/ShmemDmaModel.h"
#include "components/devices/shmem/shaddr/BgpShaddr.h"
#include "components/fifo/FifoPacket.h"
#include "components/fifo/linear/LinearFifo.h"

//#include "components/atomic/bgp/BgpAtomic.h"
#include "components/atomic/bgp/LockBoxCounter.h"
#include "components/atomic/counter/CounterMutex.h"
#include "components/atomic/native/NativeCounter.h"
#include "components/atomic/indirect/IndirectCounter.h"

#include "components/memory/MemoryAllocator.h"
#include "components/memory/MemoryManager.h"

#include "Memregion.h"

#include "p2p/protocols/rget/GetRdma.h"
#include "p2p/protocols/rput/PutRdma.h"
#include "p2p/protocols/send/eager/Eager.h"
//#include "p2p/protocols/send/adaptive/Adaptive.h"
//#include "p2p/protocols/send/datagram/Datagram.h"

#include "TypeDefs.h"

#undef TRACE_ERR
#define TRACE_ERR(x) // fprintf x

namespace PAMI
{
  typedef PAMI::Mutex::Counter<PAMI::Counter::Native>  ContextLock;

  typedef Fifo::FifoPacket <sizeof(void*)*4, 256> ShmemPacket;
  typedef Fifo::LinearFifo<ShmemPacket, Counter::Indirect<Counter::Native> > ShmemFifo;
  typedef Device::ShmemDevice<ShmemFifo,Counter::Indirect<Counter::Native>,Device::Shmem::BgpShaddr> ShmemDevice;
  //typedef Device::ShmemDevice<ShmemFifo> ShmemDevice;
  typedef Device::Shmem::PacketModel<ShmemDevice> ShmemPacketModel;

  //
  // >> Point-to-point protocol typedefs and dispatch registration.
  //typedef PAMI::Protocol::Send::Eager <ShmemModel, ShmemDevice> EagerShmem;
  // << Point-to-point protocol typedefs and dispatch registration.
  //

  typedef MemoryAllocator<1024, 16> ProtocolAllocator;

/**
 * \brief Class containing all devices used on this platform.
 *
 * This container object governs creation (allocation of device objects),
 * initialization of device objects, and advance of work. Note, typically
 * the devices advance routine is very short - or empty - since it only
 * is checking for received messages (if the device even has reception).
 *
 * The generic device is present in all platforms. This is how context_post
 * works as well as how many (most/all) devices enqueue work.
 */
  class PlatformDeviceList {
  public:
    PlatformDeviceList() { }

    /**
     * \brief initialize this platform device list
     *
     * This creates arrays (at least 1 element) for each device used in this platform.
     * Note, in some cases there may be only one device instance for the entire
     * process (all clients), but any handling of that (mutexing, etc) is hidden.
     *
     * Device arrays are semi-opaque (we don't know how many
     * elements each has).
     *
     * \param[in] clientid  Client ID (index)
     * \param[in] num_ctx Number of contexts being created
     * \param[in] mm    MemeoryManager for use in generating devices
     */
    inline pami_result_t generate(size_t clientid, size_t num_ctx, Memory::MemoryManager &mm) {
      TRACE_ERR((stderr, "%s\n", __PRETTY_FUNCTION__));
        // these calls create (allocate and construct) each element.
        // We don't know how these relate to contexts, they are semi-opaque.
        _generics = PAMI::Device::Generic::Device::Factory::generate(clientid, num_ctx, mm, NULL);
        _shmem = ShmemDevice::Factory::generate(clientid, num_ctx, mm, _generics);
        _progfunc = PAMI::Device::ProgressFunctionDev::Factory::generate(clientid, num_ctx, mm, _generics);
        _atombarr = PAMI::Device::AtomicBarrierDev::Factory::generate(clientid, num_ctx, mm, _generics);
        _wqringreduce = PAMI::Device::WQRingReduceDev::Factory::generate(clientid, num_ctx, mm, _generics);
        _wqringbcast = PAMI::Device::WQRingBcastDev::Factory::generate(clientid, num_ctx, mm, _generics);
        _localallreduce = PAMI::Device::LocalAllreduceWQDevice::Factory::generate(clientid, num_ctx, mm, _generics);
        _localbcast = PAMI::Device::LocalBcastWQDevice::Factory::generate(clientid, num_ctx, mm, _generics);
        _localreduce = PAMI::Device::LocalReduceWQDevice::Factory::generate(clientid, num_ctx, mm, _generics);
        // BGP-specific devices...
        _gibarr = PAMI::Device::BGP::giDevice::Factory::generate(clientid, num_ctx, mm, _generics);
        //_cnallred = PAMI::Device::BGP::CNAllreduceDevice::Factory::generate(clientid, num_ctx, mm, _generics);
        //_cnppallred = PAMI::Device::BGP::CNAllreducePPDevice::Factory::generate(clientid, num_ctx, mm, _generics);
        //_cn2pallred = PAMI::Device::BGP::CNAllreduce2PDevice::Factory::generate(clientid, num_ctx, mm, _generics);
        //_cnbcast = PAMI::Device::BGP::CNBroadcastDevice::Factory::generate(clientid, num_ctx, mm, _generics);
        return PAMI_SUCCESS;
    }

    /**
     * \brief initialize devices for specific context
     *
     * Called once per context, after context object is initialized.
     * Devices must handle having init() called multiple times, using
     * clientid and contextid to ensure initialization happens to the correct
     * instance and minimizing redundant initialization. When each is called,
     * the 'this' pointer actually points to the array - each device knows whether
     * that is truly an array and how many elements it contains.
     *
     * \param[in] clientid  Client ID (index)
     * \param[in] contextid Context ID (index)
     * \param[in] clt   Client opaque entity
     * \param[in] ctx   Context opaque entity
     */
    inline pami_result_t init(size_t clientid, size_t contextid, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm) {
      TRACE_ERR((stderr, "%s\n", __PRETTY_FUNCTION__));
        PAMI::Device::Generic::Device::Factory::init(_generics, clientid, contextid, clt, ctx, mm, _generics);
        ShmemDevice::Factory::init(_shmem, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::ProgressFunctionDev::Factory::init(_progfunc, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::AtomicBarrierDev::Factory::init(_atombarr, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::WQRingReduceDev::Factory::init(_wqringreduce, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::WQRingBcastDev::Factory::init(_wqringbcast, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::LocalAllreduceWQDevice::Factory::init(_localallreduce, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::LocalBcastWQDevice::Factory::init(_localbcast, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::LocalReduceWQDevice::Factory::init(_localreduce, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::BGP::giDevice::Factory::init(_gibarr, clientid, contextid, clt, ctx, mm, _generics);
        //PAMI::Device::BGP::CNAllreduceDevice::Factory::init(_cnallred, clientid, contextid, clt, ctx, mm, _generics);
        //PAMI::Device::BGP::CNAllreducePPDevice::Factory::init(_cnppallred, clientid, contextid, clt, ctx, mm, _generics);
        //PAMI::Device::BGP::CNAllreduce2PDevice::Factory::init(_cn2pallred, clientid, contextid, clt, ctx, mm, _generics);
        //PAMI::Device::BGP::CNBroadcastDevice::Factory::init(_cnbcast, clientid, contextid, clt, ctx, mm, _generics);
        return PAMI_SUCCESS;
    }

    /**
     * \brief advance all devices
     *
     * since device arrays are semi-opaque (we don't know how many
     * elements each has) we call a more-general interface here.
     *
     * \param[in] clientid  Client ID (index)
     * \param[in] contextid Context ID (index)
     */
    inline size_t advance(size_t clientid, size_t contextid) {
        size_t events = 0;
        events += PAMI::Device::Generic::Device::Factory::advance(_generics, clientid, contextid);
        events += ShmemDevice::Factory::advance(_shmem, clientid, contextid);
        events += PAMI::Device::ProgressFunctionDev::Factory::advance(_progfunc, clientid, contextid);
        events += PAMI::Device::AtomicBarrierDev::Factory::advance(_atombarr, clientid, contextid);
        events += PAMI::Device::WQRingReduceDev::Factory::advance(_wqringreduce, clientid, contextid);
        events += PAMI::Device::WQRingBcastDev::Factory::advance(_wqringbcast, clientid, contextid);
        events += PAMI::Device::LocalAllreduceWQDevice::Factory::advance(_localallreduce, clientid, contextid);
        events += PAMI::Device::LocalBcastWQDevice::Factory::advance(_localbcast, clientid, contextid);
        events += PAMI::Device::LocalReduceWQDevice::Factory::advance(_localreduce, clientid, contextid);
        // BGP-specific devices...
        events += PAMI::Device::BGP::giDevice::Factory::advance(_gibarr, clientid, contextid);
        //events += PAMI::Device::BGP::CNAllreduceDevice::Factory::advance(_cnallred, clientid, contextid);
        //events += PAMI::Device::BGP::CNAllreducePPDevice::Factory::advance(_cnppallred, clientid, contextid);
        //events += PAMI::Device::BGP::CNAllreduce2PDevice::Factory::advance(_cn2pallred, clientid, contextid);
        //events += PAMI::Device::BGP::CNBroadcastDevice::Factory::advance(_cnbcast, clientid, contextid);
        return events;
    }

    PAMI::Device::Generic::Device *_generics; // need better name...
    ShmemDevice *_shmem;
    PAMI::Device::ProgressFunctionDev *_progfunc;
    PAMI::Device::AtomicBarrierDev *_atombarr;
    PAMI::Device::WQRingReduceDev *_wqringreduce;
    PAMI::Device::WQRingBcastDev *_wqringbcast;;
    PAMI::Device::LocalAllreduceWQDevice *_localallreduce;
    PAMI::Device::LocalBcastWQDevice *_localbcast;
    PAMI::Device::LocalReduceWQDevice *_localreduce;
    // BGP-specific devices...
    PAMI::Device::BGP::giDevice *_gibarr;
    //PAMI::Device::BGP::CNAllreduceDevice *_cnallred;
    //PAMI::Device::BGP::CNAllreducePPDevice *_cnppallred;
    //PAMI::Device::BGP::CNAllreduce2PDevice *_cn2pallred;
    //PAMI::Device::BGP::CNBroadcastDevice *_cnbcast;
  }; // class PlatformDeviceList


  class Context : public Interface::Context<PAMI::Context>
  {
    public:
      inline Context (pami_client_t client, size_t clientid, size_t id, size_t num,
                      PlatformDeviceList *devices,
                      PAMI::Memory::MemoryManager * pmm, size_t bytes,
		      BGPGeometry *world_geometry,
		      std::map<unsigned, pami_geometry_t> *geometry_map) :
          Interface::Context<PAMI::Context> (client, id),
          _client (client),
          _context ((pami_context_t)this),
          _clientid (clientid),
          _contextid (id),
          _lock (),
          _world_geometry(world_geometry),
	  _geometry_map(geometry_map),
          _devices(devices)
      {
	char mmkey[PAMI::Memory::MMKEYSIZE];
	char *mms;
	mms = mmkey + sprintf(mmkey, "/pami-client%d-context%d", clientid, id);

        TRACE_ERR((stderr, "%s\n", __PRETTY_FUNCTION__));
        // ----------------------------------------------------------------
        // Compile-time assertions
        // ----------------------------------------------------------------

        // Make sure the memory allocator is large enough for all
        // protocol classes.
        //COMPILE_TIME_ASSERT(sizeof(EagerShmem) <= ProtocolAllocator::objsize);

        // ----------------------------------------------------------------
        // Compile-time assertions
        // ----------------------------------------------------------------
	strcpy(mms, "-mm");
	_mm.init(pmm, bytes, 16, 16, 0, mmkey);

	strcpy(mms, "-lk");
        //_lock.init(&_mm, mmkey);
        _devices->init(_clientid, _contextid, _client, _context, &_mm);
        _local_generic_device = & PAMI::Device::Generic::Device::Factory::getDevice(_devices->_generics, clientid, id);

        // ----------------------------------------------------------------
        // Initialize the rdma protocol(s)
        // ----------------------------------------------------------------
#if 0
        pami_result_t result = PAMI_ERROR;
        _rget = Protocol::Get::GetRdma <Device::Shmem::DmaModel<ShmemDevice,false>, ShmemDevice>::
          generate (_devices->_shmem[_contextid], _context, _protocol, result);
        TRACE_ERR((stderr, "%s<%u>  result = %d\n", __PRETTY_FUNCTION__,__LINE__, result));
        if (result != PAMI_SUCCESS)
#endif
          _rget = Protocol::Get::NoRGet::generate (_protocol);
#if 0
        result = PAMI_ERROR;
        _rput = Protocol::Put::PutRdma <Device::Shmem::DmaModel<ShmemDevice,false>, ShmemDevice>::
          generate (_devices->_shmem[_contextid], _context, _protocol, result);
        TRACE_ERR((stderr, "%s<%u>  result = %d\n", __PRETTY_FUNCTION__,__LINE__, result));
        if (result != PAMI_SUCCESS)
#endif
          _rput = Protocol::Put::NoRPut::generate (_protocol);



        // dispatch_impl relies on the table being initialized to NULL's.
        memset(_dispatch, 0x00, sizeof(_dispatch));
      }

      inline pami_client_t getClient_impl ()
      {
        return _client;
      }

      inline size_t getId_impl ()
      {
        return _contextid;
      }

      inline pami_result_t destroy_impl ()
      {
        return PAMI_SUCCESS;
      }

      inline pami_result_t post_impl (pami_work_t *state, pami_work_function work_fn, void * cookie)
      {
        TRACE_ERR((stderr, ">> Context::post_impl(%p, %p, %p)\n", state, work_fn, cookie));
        PAMI::Device::Generic::GenericThread *work;
        COMPILE_TIME_ASSERT(sizeof(*state) >= sizeof(*work));
        TRACE_ERR((stderr, "   Context::post_impl(%p, %p, %p) .. 0\n", state, work_fn, cookie));
        work = new (state) PAMI::Device::Generic::GenericThread(work_fn, cookie);
        TRACE_ERR((stderr, "   Context::post_impl(%p, %p, %p) .. 1\n", state, work_fn, cookie));
        //_devices->_generics[_contextid].postThread(work);
        _local_generic_device->postThread(work);
        TRACE_ERR((stderr, "<< Context::post_impl(%p, %p, %p)\n", state, work_fn, cookie));
        return PAMI_SUCCESS;
      }

      inline size_t advance_impl (size_t maximum, pami_result_t & result)
      {
//          result = PAMI_EAGAIN;
        result = PAMI_SUCCESS;
        size_t events = 0;

        unsigned i;

        for (i = 0; i < maximum && events == 0; i++)
          {
            events += _devices->advance(_clientid, _contextid);
          }

        //if (events > 0) result = PAMI_SUCCESS;

        return events;
      }

      inline pami_result_t lock_impl ()
      {
        TRACE_ERR((stderr, ">> lock_impl()\n"));
        _lock.acquire ();
        TRACE_ERR((stderr, "<< lock_impl()\n"));
        return PAMI_SUCCESS;
      }

      inline pami_result_t trylock_impl ()
      {
        TRACE_ERR((stderr, ">> trylock_impl()\n"));

        if (_lock.tryAcquire ())
          {
            TRACE_ERR((stderr, "<< trylock_impl(), PAMI_SUCCESS\n"));
            return PAMI_SUCCESS;
          }

        TRACE_ERR((stderr, "<< trylock_impl(), PAMI_EAGAIN\n"));
        return PAMI_EAGAIN;
      }

      inline pami_result_t unlock_impl ()
      {
        TRACE_ERR((stderr, ">> release_impl()\n"));
        _lock.release ();
        TRACE_ERR((stderr, "<< release_impl()\n"));
        return PAMI_SUCCESS;
      }

      inline pami_result_t send_impl (pami_send_t * parameters)
      {
        size_t id = (size_t)(parameters->send.dispatch);
        TRACE_ERR((stderr, ">> send_impl('simple'), _dispatch[%zu] = %p\n", id, _dispatch[id]));
        PAMI_assert_debug (_dispatch[id] != NULL);

        PAMI::Protocol::Send::Send * send =
          (PAMI::Protocol::Send::Send *) _dispatch[id];
        pami_result_t result = send->simple (parameters);

        TRACE_ERR((stderr, "<< send_impl('simple')\n"));
        return result;
      }

      inline pami_result_t send_impl (pami_send_immediate_t * parameters)
      {
        size_t id = (size_t)(parameters->dispatch);
        TRACE_ERR((stderr, ">> send_impl('immediate'), _dispatch[%zu] = %p\n", id, _dispatch[id]));
        PAMI_assert_debug (_dispatch[id] != NULL);

        PAMI::Protocol::Send::Send * send =
          (PAMI::Protocol::Send::Send *) _dispatch[id];
        pami_result_t result = send->immediate (parameters);

        TRACE_ERR((stderr, "<< send_impl('immediate')\n"));
        return result;
      }

      inline pami_result_t send_impl (pami_send_typed_t * parameters)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t put (pami_put_simple_t * parameters)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t put_typed (pami_put_typed_t * parameters)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t get (pami_get_simple_t * parameters)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t get_typed (pami_get_typed_t * parameters)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t rmw (pami_rmw_t * parameters)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t memregion_create_impl (void            * address,
                                                  size_t             bytes_in,
                                                  size_t           * bytes_out,
                                                  pami_memregion_t * memregion)
      {
        COMPILE_TIME_ASSERT(sizeof(Memregion) <= sizeof(pami_memregion_t));

        new ((void *) memregion) Memregion ();
        Memregion * mr = (Memregion *) memregion;
        return mr->createMemregion (bytes_out, bytes_in, address, 0);
      }

      inline pami_result_t memregion_destroy_impl (pami_memregion_t * memregion)
      {
        // Memory regions do not need to be deregistered on BG/Q so this
        // interface is implemented as a noop.
        return PAMI_SUCCESS;
      }

      inline pami_result_t rput (pami_rput_simple_t * parameters)
      {
        TRACE_ERR((stderr, ">> rput_impl('simple')\n"));

        pami_result_t rc = _rput->simple (parameters);

        TRACE_ERR((stderr, "<< rput_impl('simple') rc = %d\n",rc));
        return rc;
      }

      inline pami_result_t rput_typed (pami_rput_typed_t * parameters)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t rget (pami_rget_simple_t * parameters)
      {
        TRACE_ERR((stderr, ">> rget_impl('simple')\n"));

        pami_result_t rc = _rget->simple (parameters);

        TRACE_ERR((stderr, "<< rget_impl('simple') rc = %d\n",rc));
        return rc;
      }

      inline pami_result_t rget_typed (pami_rget_typed_t * parameters)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t purge_totask (pami_endpoint_t *dest, size_t count)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t resume_totask (pami_endpoint_t *dest, size_t count)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t fence_begin ()
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t fence_end ()
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t fence_all (pami_event_function   done_fn,
                                     void               * cookie)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t fence_endpoint (pami_event_function   done_fn,
                                           void                * cookie,
                                           pami_endpoint_t       endpoint)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t collective_impl (pami_xfer_t * parameters)
      {
        Geometry::Algorithm<BGPGeometry> *algo = (Geometry::Algorithm<BGPGeometry> *)parameters->algorithm;
        algo->setContext((pami_context_t) this);
        return algo->generate(parameters);
      }

    inline pami_result_t amcollective_dispatch_impl (pami_algorithm_t            algorithm,
                                                    size_t                     dispatch,
                                                    pami_dispatch_callback_function fn,
                                                    void                     * cookie,
                                                    pami_collective_hint_t      options)
      {
        PAMI_abort();
        return PAMI_SUCCESS;
      }


      inline pami_result_t dispatch_impl (size_t                          id,
                                          pami_dispatch_callback_function fn,
                                          void                          * cookie,
                                          pami_dispatch_hint_t            options)
      {
        pami_result_t result = PAMI_ERROR;
        TRACE_ERR((stderr, ">> dispatch_impl(), _dispatch[%zu] = %p\n", id, _dispatch[id]));

        // Return an error for invalid / unimplemented 'hard' hints.
        if (
            options.use_rdma              == PAMI_HINT_ENABLE  ||
            false)
          {
            return PAMI_ERROR;
          }

        pami_endpoint_t self = PAMI_ENDPOINT_INIT(_clientid, __global.mapping.task(), _contextid);

        using namespace Protocol::Send;

        if (_dispatch[id] == NULL)
          {
            if (options.use_shmem != PAMI_HINT_DISABLE)
              {
                _dispatch[id] =
                  Eager <ShmemPacketModel>::generate (id, fn.p2p, cookie,
                                                             ShmemDevice::Factory::getDevice(_devices->_shmem, _clientid, _contextid),
                                                             self, _context, options,
                                                             __global.heap_mm, result);
              }
          }

        TRACE_ERR((stderr, "<< dispatch_impl(), result = %zu, _dispatch[%zu] = %p\n", result, id, _dispatch[id]));
        return result;
      }

      inline pami_result_t analyze(size_t         context_id,
                                  BGPGeometry    *geometry)
      {
        return PAMI_SUCCESS;
      }


      inline pami_result_t dispatch_query_impl(size_t                dispatch,
                                               pami_configuration_t  configuration[],
                                               size_t                num_configs)
      {
        PAMI::Protocol::Send::Send * send =
          (PAMI::Protocol::Send::Send *) _dispatch[dispatch];
        return send->getAttributes (configuration, num_configs);
      }

      inline pami_result_t dispatch_update_impl(size_t                dispatch,
                                                pami_configuration_t  configuration[],
                                                size_t                num_configs)
      {
        return PAMI_INVAL;
      }

      inline pami_result_t query_impl(pami_configuration_t  configuration[],
                                      size_t                num_configs)
        {
          pami_result_t result = PAMI_SUCCESS;
          size_t i;
          for(i=0; i<num_configs; i++)
            {
              switch (configuration[i].name)
                {
                  case PAMI_CONTEXT_DISPATCH_ID_MAX:
                  default:
                    result = PAMI_INVAL;
                }
            }
          return result;
        }

      inline pami_result_t update_impl(pami_configuration_t  configuration[],
                                       size_t                num_configs)
        {
          return PAMI_INVAL;
        }
  private:

      pami_client_t  _client;
      pami_context_t _context;
      size_t        _clientid;
      size_t        _contextid;

      Protocol::Get::RGet         * _rget;
      Protocol::Put::RPut         * _rput;
      PAMI::Memory::GenMemoryManager   _mm;

      // devices...
      ContextLock _lock;

      void * _dispatch[1024];
    public:
      BGPGeometry                 *_world_geometry;
      std::map<unsigned, pami_geometry_t> *_geometry_map;
    private:
      ProtocolAllocator _protocol;
      PlatformDeviceList *_devices;

      PAMI::Device::Generic::Device * _local_generic_device;
  }; // end PAMI::Context
}; // end namespace PAMI

#undef TRACE_ERR

#endif // __components_context_bgp_bgpcontext_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
