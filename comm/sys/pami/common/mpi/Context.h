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
/// \file common/mpi/Context.h
/// \brief PAMI MPI specific context implementation.
///
#ifndef __common_mpi_Context_h__
#define __common_mpi_Context_h__

#define ENABLE_GENERIC_DEVICE
#define ENABLE_SHMEM_DEVICE

#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "common/ContextInterface.h"
#include "algorithms/geometry/Geometry.h"
#include "WakeupManager.h"
#include "components/devices/mpi/mpidevice.h"
#include "components/devices/mpi/mpipacketmodel.h"
#include "components/devices/mpi/mpimessage.h"

#include "p2p/protocols/send/eager/Eager.h"
#include "p2p/protocols/send/composite/Composite.h"

#include "components/devices/generic/Device.h"
#include "components/devices/misc/ProgressFunctionMsg.h"
#include "components/devices/misc/AtomicBarrierMsg.h"
#include "components/devices/workqueue/WQRingReduceMsg.h"
#include "components/devices/workqueue/WQRingBcastMsg.h"
#include "components/devices/workqueue/LocalAllreduceWQMessage.h"
#include "components/devices/workqueue/LocalReduceWQMessage.h"
#include "components/devices/workqueue/LocalBcastWQMessage.h"
#include "components/devices/workqueue/WQRingBcastMsg.h"
#include "components/devices/workqueue/LocalBcastWQMessage.h"
#include "components/devices/mpi/MPIBcastMsg.h"
#include "components/devices/mpi/MPISyncMsg.h"
#include "algorithms/geometry/PGASCollRegistration.h"
#include "algorithms/geometry/P2PCCMIRegistration.h"
#include "algorithms/geometry/ClassRouteId.h"

#include "Mapping.h"
#include <new>
#include <map>
#include "components/atomic/indirect/IndirectCounter.h"
#include "components/atomic/counter/CounterMutex.h"
#include "components/atomic/native/NativeCounter.h"
#include <sched.h>

#ifdef ENABLE_SHMEM_DEVICE
#include "components/devices/shmem/ShmemDevice.h"
#include "components/devices/shmem/ShmemPacketModel.h"
#include "components/fifo/FifoPacket.h"
#include "components/fifo/linear/LinearFifo.h"
#endif

#include "components/devices/mpi/mpimulticastprotocol.h"
#include "components/devices/mpi/mpimulticastmodel.h"
#include "components/devices/mpi/mpimultisyncmodel.h"
#include "components/devices/mpi/mpimulticombinemodel.h"
#include "components/devices/mpi/mpimanytomanymodel.h"

extern PAMI::Device::MPIDevice _g_mpi_device;

#undef TRACE_ERR
#define TRACE_ERR(x) //fprintf x

namespace PAMI
{
    // This should work with XL
    typedef PAMI::Mutex::Counter<PAMI::Counter::Native>  ContextLock;
    typedef Device::MPIMessage MPIMessage;
    typedef Device::MPIDevice MPIDevice;
    typedef Device::MPIPacketModel<MPIDevice,MPIMessage> MPIPacketModel;
    typedef PAMI::Protocol::Send::Eager <MPIPacketModel> MPIEagerBase;
    typedef PAMI::Protocol::Send::SendWrapperPWQ < MPIEagerBase >       MPIEager;

    // \todo #warning I do not distinguish local vs non-local so no eager shmem protocol here... just MPIEagerBase
  typedef PAMI::Protocol::MPI::P2PMcastProto<MPIDevice,
                                            MPIEagerBase,
                                            PAMI::Device::MPIBcastMdl,
                                            PAMI::Device::MPIBcastDev> P2PMcastProto;

#ifdef ENABLE_SHMEM_DEVICE
    typedef Fifo::FifoPacket <64, 1024>                            ShmemPacket;
    typedef Fifo::LinearFifo<ShmemPacket, Counter::Indirect<Counter::Native> > ShmemFifo;
    typedef Device::ShmemDevice<ShmemFifo, Counter::Indirect<Counter::Native> >                         ShmemDevice;
    typedef Device::Shmem::PacketModel<ShmemDevice>                ShmemPacketModel;
    typedef Protocol::Send::Eager <ShmemPacketModel>               ShmemEagerBase;
    typedef PAMI::Protocol::Send::SendWrapperPWQ < ShmemEagerBase >       ShmemEager;
#endif

    typedef MemoryAllocator<1024, 16> ProtocolAllocator;

  // Collective Types
  typedef Geometry::Common                                           MPIGeometry;
  typedef Device::MPIMsyncMessage                                    MPIMsyncMessage;
  typedef Device::MPIMultisyncModel<MPIDevice,MPIMsyncMessage>       MPIMultisyncModel;
  typedef Device::MPIMcastMessage                                    MPIMcastMessage;
  typedef Device::MPIMulticastModel<MPIDevice,MPIMcastMessage>       MPIMulticastModel;
  typedef Device::MPIMcombineMessage                                 MPIMcombineMessage;
  typedef Device::MPIMulticombineModel<MPIDevice,
                                       MPIMcombineMessage>           MPIMulticombineModel;
  typedef Device::MPIM2MMessage                                      MPIM2MMessage;
  typedef Device::MPIManytomanyModel<MPIDevice,MPIM2MMessage>        MPIManytomanyModel;

  // "New" CCMI Typedefs/Coll Registration
  typedef PAMI::NativeInterfaceActiveMessage<MPIEager>   MPIEagerNI_AM;
  typedef PAMI::NativeInterfaceAllsided<MPIEager>        MPIEagerNI_AS;
  typedef PAMI::NativeInterfaceActiveMessage<ShmemEager> ShmemEagerNI_AM;
  typedef PAMI::NativeInterfaceAllsided<ShmemEager>      ShmemEagerNI_AS;
  typedef PAMI::NativeInterfaceActiveMessage< Protocol::Send::SendPWQ< Protocol::Send::Send> > CompositeNI_AM;
  typedef PAMI::NativeInterfaceAllsided< Protocol::Send::SendPWQ< Protocol::Send::Send> > CompositeNI_AS;



  typedef PAMI::MPINativeInterface<MPIDevice,
                                  MPIMulticastModel,
                                  MPIMultisyncModel,
                                  MPIMulticombineModel,
                                  OneSided>              DefaultNativeInterface;

  typedef PAMI::MPINativeInterface<MPIDevice,
                                  MPIMulticastModel,
                                  MPIMultisyncModel,
                                  MPIMulticombineModel,
                                  AllSided>              DefaultNativeInterfaceAS;

  typedef NativeInterfaceCommon::NativeInterfaceFactory <ProtocolAllocator,  MPIEagerNI_AM, MPIEagerNI_AS, MPIEager, MPIDevice> MPINIFactory;
  typedef CollRegistration::P2P::CCMIRegistration<MPIGeometry,
                                                 ProtocolAllocator,
                                                 CCMI::Adaptor::P2PBarrier::BinomialBarrier,
                                                 CCMI::Adaptor::P2PBarrier::BinomialBarrierFactory > P2PCCMICollreg;


  // PGAS RT Typedefs/Coll Registration
  typedef xlpgas::CollectiveManager<CompositeNI_AM> MPINBCollManager;
  typedef CollRegistration::PGASRegistration<MPIGeometry,
                                             CompositeNI_AM,
                                             ProtocolAllocator,
                                             MPIEager,
                                             ShmemEager,
                                             MPIDevice,
                                             ShmemDevice,
                                             MPINBCollManager> PGASCollreg;
  
  typedef Geometry::ClassRouteId<MPIGeometry> MPIClassRouteId;


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
     * \param[in] clientid     Client ID (index)
     * \param[in] contextid    Context ID (index)
     */
    inline pami_result_t generate(size_t clientid, size_t num_ctx, Memory::MemoryManager &mm) {
        // these calls create (allocate and construct) each element.
        // We don't know how these relate to contexts, they are semi-opaque.
        _generics = PAMI::Device::Generic::Device::Factory::generate(clientid, num_ctx, mm, NULL);
#ifdef ENABLE_SHMEM_DEVICE
        _shmem = ShmemDevice::Factory::generate(clientid, num_ctx, mm, _generics);
#endif
        _progfunc = PAMI::Device::ProgressFunctionDev::Factory::generate(clientid, num_ctx, mm, _generics);
        _atombarr = PAMI::Device::AtomicBarrierDev::Factory::generate(clientid, num_ctx, mm, _generics);
        _wqringreduce = PAMI::Device::WQRingReduceDev::Factory::generate(clientid, num_ctx, mm, _generics);
        _wqringbcast = PAMI::Device::WQRingBcastDev::Factory::generate(clientid, num_ctx, mm, _generics);
        _localallreduce = PAMI::Device::LocalAllreduceWQDevice::Factory::generate(clientid, num_ctx, mm, _generics);
        _localbcast = PAMI::Device::LocalBcastWQDevice::Factory::generate(clientid, num_ctx, mm, _generics);
        _localreduce = PAMI::Device::LocalReduceWQDevice::Factory::generate(clientid, num_ctx, mm, _generics);
        _mpimsync = PAMI::Device::MPISyncDev::Factory::generate(clientid, num_ctx, mm, _generics);
        _mpimcast = PAMI::Device::MPIBcastDev::Factory::generate(clientid, num_ctx, mm, _generics);
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
     * \param[in] clientid     Client ID (index)
     * \param[in] num_ctx      Number of contexts in this client
     * \param[in] ctx          Context opaque entity
     * \param[in] contextid    Context ID (index)
     */
    inline pami_result_t init(size_t clientid, size_t contextid, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm) {
        PAMI::Device::Generic::Device::Factory::init(_generics, clientid, contextid, clt, ctx, mm, _generics);
#ifdef ENABLE_SHMEM_DEVICE
        ShmemDevice::Factory::init(_shmem, clientid, contextid, clt, ctx, mm, _generics);
#endif
        PAMI::Device::ProgressFunctionDev::Factory::init(_progfunc, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::AtomicBarrierDev::Factory::init(_atombarr, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::WQRingReduceDev::Factory::init(_wqringreduce, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::WQRingBcastDev::Factory::init(_wqringbcast, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::LocalAllreduceWQDevice::Factory::init(_localallreduce, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::LocalBcastWQDevice::Factory::init(_localbcast, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::LocalReduceWQDevice::Factory::init(_localreduce, clientid, contextid, clt, ctx, mm, _generics);

        PAMI::Device::MPISyncDev::Factory::init(_mpimsync, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::MPIBcastDev::Factory::init(_mpimcast, clientid, contextid, clt, ctx, mm, _generics);
        return PAMI_SUCCESS;
    }

    /**
     * \brief advance all devices
     *
     * since device arrays are semi-opaque (we don't know how many
     * elements each has) we call a more-general interface here.
     *
     * \param[in] clientid     Client ID (index)
     * \param[in] contextid    Context ID (index)
     */
    inline size_t advance(size_t clientid, size_t contextid) {
        size_t events = 0;
        events += PAMI::Device::Generic::Device::Factory::advance(_generics, clientid, contextid);
#ifdef ENABLE_SHMEM_DEVICE
        events += ShmemDevice::Factory::advance(_shmem, clientid, contextid);
#endif
#if 0
        events += PAMI::Device::ProgressFunctionDev::Factory::advance(_progfunc, clientid, contextid);
        events += PAMI::Device::AtomicBarrierDev::Factory::advance(_atombarr, clientid, contextid);
        events += PAMI::Device::WQRingReduceDev::Factory::advance(_wqringreduce, clientid, contextid);
        events += PAMI::Device::WQRingBcastDev::Factory::advance(_wqringbcast, clientid, contextid);
        events += PAMI::Device::LocalAllreduceWQDevice::Factory::advance(_localallreduce, clientid, contextid);
        events += PAMI::Device::LocalBcastWQDevice::Factory::advance(_localbcast, clientid, contextid);
        events += PAMI::Device::LocalReduceWQDevice::Factory::advance(_localreduce, clientid, contextid);
        events += PAMI::Device::MPISyncDev::Factory::advance(_mpimsync, clientid, contextid);
        events += PAMI::Device::MPIBcastDev::Factory::advance(_mpimcast, clientid, contextid);
#endif
        return events;
    }

    PAMI::Device::Generic::Device *_generics; // need better name...
#ifdef ENABLE_SHMEM_DEVICE
    ShmemDevice *_shmem;
#endif
    PAMI::Device::ProgressFunctionDev *_progfunc;
    PAMI::Device::AtomicBarrierDev *_atombarr;
    PAMI::Device::WQRingReduceDev *_wqringreduce;
    PAMI::Device::WQRingBcastDev *_wqringbcast;
    PAMI::Device::LocalAllreduceWQDevice *_localallreduce;
    PAMI::Device::LocalBcastWQDevice *_localbcast;
    PAMI::Device::LocalReduceWQDevice *_localreduce;
    PAMI::Device::MPISyncDev *_mpimsync;
    PAMI::Device::MPIBcastDev *_mpimcast;
  }; // class PlatformDeviceList

    class Context : public Interface::Context<PAMI::Context>
    {
    public:
      inline Context (pami_client_t          client,
                      size_t                 clientid,
                      size_t                 id,
                      size_t                 num,
                      PlatformDeviceList *devices,
                      Memory::MemoryManager *mm,
                      size_t                 bytes,
                      MPIGeometry           *world_geometry,
                      std::map<unsigned, pami_geometry_t> *geometry_map) :
        Interface::Context<PAMI::Context> (client, id),
        _client (client),
        _context(this),
        _clientid (clientid),
        _contextid (id),
        _geometry_map(geometry_map),
        _dispatch_id(255),
        _mm (),
        _lock (),
        _mpi(&_g_mpi_device),
        _world_geometry(world_geometry),
        _minterface(*_mpi, client, this, _contextid, clientid),
        _empty_advance(0),
        _devices(devices)
        {
	  char mmkey[PAMI::Memory::MMKEYSIZE];
	  char *mms;
	  mms = mmkey + sprintf(mmkey, "/pami-client%zd-context%zd", clientid, id);

	  strcpy(mms, "-mm");
	  _mm.init(mm, bytes, 16, 16, 0, mmkey);

          // dispatch_impl relies on the table being initialized to NULL's.
          memset(_dispatch, 0x00, sizeof(_dispatch));


#ifdef USE_WAKEUP_VECTORS
          _wakeupManager.init(1, 0x57550000 | id); // check errors?
#endif // USE_WAKEUP_VECTORS
          _devices->init(_clientid, _contextid, _client, _context, &_mm);
          _mpi->init(&_mm, _clientid, num, (pami_context_t)this, id);
	  strcpy(mms, "-lk");
          //_lock.init(&_mm, mmkey);

          // this barrier is here because the shared memory init
          // needs to be synchronized
          // we shoudl find a way to remove this
          MPI_Barrier(MPI_COMM_WORLD);

          pami_result_t rc;
          rc = __global.heap_mm->memalign((void **)&_pgas_collreg, 0,
                                          sizeof(*_pgas_collreg));
          PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc PGASCollreg");
          new(_pgas_collreg) PGASCollreg(client,(pami_context_t)this,clientid,id,_protocol,*_mpi,
                                         _devices->_shmem[_contextid], &_dispatch_id, _geometry_map,false);
          _world_geometry->resetUEBarrier(); // Reset so pgas will select the UE barrier
          _pgas_collreg->analyze(_contextid,_world_geometry);
          _pgas_collreg->setGenericDevice(&_devices->_generics[_contextid]);

          // We have not enabled shmem or shmem/mpi composite CCMI, only mpi (ni factory)
          MPINIFactory *ni_factory;
          rc = __global.heap_mm->memalign((void **)&ni_factory, 0,
                                          sizeof(*ni_factory)); 
          PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc NI Factory");
          new (ni_factory) MPINIFactory (_client, _context, _clientid, _contextid, *_mpi, _protocol);

          rc = __global.heap_mm->memalign((void **)&_p2p_ccmi_collreg, 0,
                                          sizeof(*_p2p_ccmi_collreg));
          PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc P2PCCMICollreg");
          new(_p2p_ccmi_collreg) P2PCCMICollreg(_client,
                                                _context,
                                                _contextid,
                                                _clientid,
                                                _protocol,
                                                __global.topology_global.size(),
                                                __global.topology_local.size(),
                                                &_dispatch_id,
                                                _geometry_map,
                                                ni_factory);
          _p2p_ccmi_collreg->analyze(_contextid, _world_geometry);

          MPI_Barrier(MPI_COMM_WORLD);
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
          // Do not call finalize because if we do
          // it is not valid to call init again
          // per the MPI spec.
          return PAMI_SUCCESS;
        }

      inline pami_result_t post_impl (pami_work_t *state, pami_work_function work_fn, void * cookie)
        {
          PAMI::Device::Generic::GenericThread *work;
          COMPILE_TIME_ASSERT(sizeof(*state) >= sizeof(*work));
          work = new (state) PAMI::Device::Generic::GenericThread(work_fn, cookie);
          _devices->_generics[_contextid].postThread(work);
          return PAMI_SUCCESS;
        }

      inline size_t advance_impl (size_t maximum, pami_result_t & result)
        {
          result = PAMI_SUCCESS;
          size_t events = 0;

          unsigned i;
          for (i=0; i<maximum && events==0; i++)
              {
/// \todo #warning Does this _mpi device still belong here?
        events += _mpi->advance_impl();
        events += _devices->advance(_clientid, _contextid);

                if(events == 0)
                  _empty_advance++;
                else
                  _empty_advance=0;
              }
          if(_empty_advance==10)
              {
                sched_yield();
                _empty_advance=0;
              }

#ifdef USE_WAKEUP_VECTORS
          // this is only for advances that are allowed to sleep...
          if (!events) {
                  _wakeupManager.sleep(xxx); // check errors?
                goto repeat;
          }
#endif // USE_WAKEUP_VECTORS

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
        TRACE_ERR((stderr, ">> Context::send_impl('simple'), _dispatch[%zu][0] = %p\n", id, _dispatch[id][0]));
        PAMI_assert_debug (_dispatch[id][0] != NULL);

        PAMI::Protocol::Send::Send * send =
          (PAMI::Protocol::Send::Send *) _dispatch[id][0];


        TRACE_ERR((stderr, "<< Context::send_impl('simple')\n"));
        return send->simple (parameters);
        }

      inline pami_result_t send_impl (pami_send_immediate_t * parameters)
        {
        size_t id = (size_t)(parameters->dispatch);
        TRACE_ERR((stderr, ">> Context::send_impl('immediate'), _dispatch[%zu][0] = %p\n", id, _dispatch[id][0]));
        PAMI_assert_debug (_dispatch[id][0] != NULL);

        PAMI::Protocol::Send::Send * send =
          (PAMI::Protocol::Send::Send *) _dispatch[id][0];


        TRACE_ERR((stderr, "<< Context::send_impl('immediate')\n"));
        return send->immediate (parameters);
        }

      inline pami_result_t send_impl (pami_send_typed_t * parameters)
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }

      inline pami_result_t put_impl (pami_put_simple_t * parameters)
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }

      inline pami_result_t put_typed_impl (pami_put_typed_t * parameters)
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }

      inline pami_result_t get_impl (pami_get_simple_t * parameters)
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }

      inline pami_result_t get_typed_impl (pami_get_typed_t * parameters)
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }

      inline pami_result_t rmw_impl (pami_rmw_t * parameters)
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }

      inline pami_result_t memregion_create_impl (void             * address,
                                                  size_t             bytes_in,
                                                  size_t           * bytes_out,
                                                  pami_memregion_t * memregion)
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }

      inline pami_result_t memregion_destroy_impl (pami_memregion_t * memregion)
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }

      inline pami_result_t rput_impl (pami_rput_simple_t * parameters)
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }

      inline pami_result_t rput_typed_impl (pami_rput_typed_t * parameters)
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }

      inline pami_result_t rget_impl (pami_rget_simple_t * parameters)
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }

      inline pami_result_t rget_typed_impl (pami_rget_typed_t * parameters)
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }

      inline pami_result_t purge_totask_impl (pami_endpoint_t * dest, size_t count)
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }

      inline pami_result_t resume_totask_impl (pami_endpoint_t * dest, size_t count)
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }

      inline pami_result_t fence_begin_impl ()
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }

      inline pami_result_t fence_end_impl ()
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }

      inline pami_result_t fence_all_impl (pami_event_function   done_fn,
                                          void               * cookie)
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }

      inline  pami_result_t fence_endpoint_impl (pami_event_function   done_fn,
                                                 void                * cookie,
                                                 pami_endpoint_t       endpoint)
        {
          PAMI_abort();
          return PAMI_UNIMPL;
        }


    inline pami_result_t collective_impl (pami_xfer_t * parameters)
      {
        Geometry::Algorithm<MPIGeometry> *algo = (Geometry::Algorithm<MPIGeometry> *)parameters->algorithm;
        algo->setContext((pami_context_t) this);
        return algo->generate(parameters);
        }

    inline pami_result_t amcollective_dispatch_impl (pami_algorithm_t            algorithm,
                                                    size_t                     dispatch,
                                                    pami_dispatch_callback_function fn,
                                                    void                     * cookie,
                                                    pami_collective_hint_t      options)
      {
        Geometry::Algorithm<MPIGeometry> *algo = (Geometry::Algorithm<MPIGeometry> *)algorithm;
        return algo->dispatch_set(_contextid, dispatch, fn, cookie, options);
      }

      inline pami_result_t dispatch_impl (size_t                         id,
                                         pami_dispatch_callback_function fn,
                                         void                          * cookie,
                                         pami_dispatch_hint_t            options)
      {
        pami_result_t result = PAMI_ERROR;
        TRACE_ERR((stderr, ">> Context::dispatch_impl .. _dispatch[%zu][0] = %p, result = %d\n", id, _dispatch[id][0], result));

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
            if (options.use_shmem == PAMI_HINT_DISABLE)
              {
                _dispatch[id][0] =
                  Eager <MPIPacketModel>::generate (id, fn.p2p, cookie,
                                                    *_mpi,
                                                    self, _context, options,
                                                    __global.heap_mm, result);
              }
#ifdef ENABLE_SHMEM_DEVICE
            else if (options.use_shmem == PAMI_HINT_ENABLE)
              {
                _dispatch[id][0] =
                  Eager <ShmemPacketModel>::generate (id, fn.p2p, cookie,
                                                             _devices->_shmem[_contextid],
                                                             self, _context, options,
                                                             __global.heap_mm, result);
              }
#endif
            else
              {
                _dispatch[id][0] =
#ifdef ENABLE_SHMEM_DEVICE
                  Eager <ShmemPacketModel, MPIPacketModel>::generate (id, fn.p2p, cookie,
                                                             _devices->_shmem[_contextid],
                                                             *_mpi,
                                                             self, _context, options,
                                                             __global.heap_mm, result);
#else
                  Eager <MPIPacketModel>::generate (id, fn.p2p, cookie,
                                                    *_mpi,
                                                    self, _context, options,
                                                    __global.heap_mm, result);
#endif
              }
          } // end dispatch[id][0]==null

        TRACE_ERR((stderr, "<< Context::dispatch_impl .. result = %d\n", result));
        return result;
      }


      inline pami_result_t dispatch_query_impl(size_t                dispatch,
                                               pami_configuration_t  configuration[],
                                               size_t                num_configs)
        {
        PAMI::Protocol::Send::Send * send =
          (PAMI::Protocol::Send::Send *) _dispatch[dispatch][0];
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
      std::map <unsigned, pami_geometry_t>   _geometry_id;
      pami_client_t              _client;
      pami_context_t             _context;
      size_t                    _clientid;
      size_t                    _contextid;
      std::map<unsigned, pami_geometry_t> *_geometry_map;
      void                     *_dispatch[1024][2];
      int                       _dispatch_id;
      ProtocolAllocator         _protocol;
      Memory::GenMemoryManager  _mm;
      ContextLock _lock;

#ifdef USE_WAKEUP_VECTORS
      PAMI::WakeupManager _wakeupManager;
#endif /* USE_WAKEUP_VECTORS */
      MemoryAllocator<4096,16>  _request;
      MPIDevice                *_mpi;
  public:
      P2PCCMICollreg            *_p2p_ccmi_collreg;
      PGASCollreg               *_pgas_collreg;
      MPIGeometry               *_world_geometry;
      DefaultNativeInterface     _minterface;
      unsigned                   _empty_advance;
      unsigned                  *_ranklist;
      PlatformDeviceList        *_devices;
    }; // end PAMI::Context
}; // end namespace PAMI

#undef TRACE_ERR

#endif // __pami_mpi_mpicontext_h__
