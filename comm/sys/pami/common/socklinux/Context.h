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
/// \file common/socklinux/Context.h
/// \brief PAMI Sockets on Linux specific context implementation.
///
#ifndef __common_socklinux_Context_h__
#define __common_socklinux_Context_h__

#define ENABLE_SHMEM_DEVICE
//#define ENABLE_UDP_DEVICE

#include <stdlib.h>
#include <string.h>

#include <pami.h>
#include "common/ContextInterface.h"
#include "common/default/Context.h"

#ifdef ENABLE_UDP_DEVICE
#include "components/devices/udp/UdpDevice.h"
#include "components/devices/udp/UdpModel.h"
#include "components/devices/udp/UdpMessage.h"
#endif

#ifdef ENABLE_SHMEM_DEVICE
#include "components/devices/shmem/ShmemDevice.h"
#include "components/devices/shmem/ShmemPacketModel.h"
#include "components/fifo/FifoPacket.h"
#include "components/fifo/linear/LinearFifo.h"
#endif

#include "components/atomic/native/NativeCounter.h"
#include "components/atomic/indirect/IndirectCounter.h"
//#include "components/atomic/pthread/Pthread.h"

#include "components/memory/MemoryAllocator.h"

#include "components/memory/MemoryManager.h"
//#include "Memregion.h"

#include "p2p/protocols/send/eager/Eager.h"
#ifdef ENABLE_UDP_DEVICE
#include "p2p/protocols/send/datagram/Datagram.h"
#endif
#include "p2p/protocols/send/composite/Composite.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x) // fprintf x
#endif

namespace PAMI
{
#ifdef ENABLE_UDP_DEVICE
  typedef Device::UDP::UdpDevice UdpDevice;
  typedef Device::UDP::UdpModel<UdpDevice,Device::UDP::UdpSendMessage> UdpModel;
  typedef PAMI::Protocol::Send::Datagram < UdpModel, UdpDevice > DatagramUdp;
#endif

#ifdef ENABLE_SHMEM_DEVICE
  typedef Fifo::FifoPacket <sizeof(void*)*4, 240> ShmemPacket;
  typedef Fifo::LinearFifo<ShmemPacket, Counter::Indirect<Counter::Native> > ShmemFifo;
  typedef Device::ShmemDevice<ShmemFifo,Counter::Indirect<Counter::Native> > ShmemDevice;
  typedef Device::Shmem::PacketModel<ShmemDevice> ShmemModel;
#endif

  typedef MemoryAllocator<1152, 16> ProtocolAllocator;

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
#ifdef ENABLE_UDP_DEVICE
        _udp = UdpDevice::Factory::generate(clientid, num_ctx, mm, _generics);
#endif
        _progfunc = PAMI::Device::ProgressFunctionDev::Factory::generate(clientid, num_ctx, mm, _generics);
        _atombarr = PAMI::Device::AtomicBarrierDev::Factory::generate(clientid, num_ctx, mm, _generics);
        _wqringreduce = PAMI::Device::WQRingReduceDev::Factory::generate(clientid, num_ctx, mm, _generics);
        _wqringbcast = PAMI::Device::WQRingBcastDev::Factory::generate(clientid, num_ctx, mm, _generics);
        _localallreduce = PAMI::Device::LocalAllreduceWQDevice::Factory::generate(clientid, num_ctx, mm, _generics);
        _localbcast = PAMI::Device::LocalBcastWQDevice::Factory::generate(clientid, num_ctx, mm, _generics);
        _localreduce = PAMI::Device::LocalReduceWQDevice::Factory::generate(clientid, num_ctx, mm, _generics);
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
#ifdef ENABLE_UDP_DEVICE
        UdpDevice::Factory::init(_udp, clientid, contextid, clt, ctx, mm, _generics);
#endif
        PAMI::Device::ProgressFunctionDev::Factory::init(_progfunc, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::AtomicBarrierDev::Factory::init(_atombarr, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::WQRingReduceDev::Factory::init(_wqringreduce, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::WQRingBcastDev::Factory::init(_wqringbcast, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::LocalAllreduceWQDevice::Factory::init(_localallreduce, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::LocalBcastWQDevice::Factory::init(_localbcast, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::LocalReduceWQDevice::Factory::init(_localreduce, clientid, contextid, clt, ctx, mm, _generics);
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
#ifdef ENABLE_UDP_DEVICE
        events += UdpDevice::Factory::advance(_udp, clientid, contextid);
#endif
        events += PAMI::Device::ProgressFunctionDev::Factory::advance(_progfunc, clientid, contextid);
        events += PAMI::Device::AtomicBarrierDev::Factory::advance(_atombarr, clientid, contextid);
        events += PAMI::Device::WQRingReduceDev::Factory::advance(_wqringreduce, clientid, contextid);
        events += PAMI::Device::WQRingBcastDev::Factory::advance(_wqringbcast, clientid, contextid);
        events += PAMI::Device::LocalAllreduceWQDevice::Factory::advance(_localallreduce, clientid, contextid);
        events += PAMI::Device::LocalBcastWQDevice::Factory::advance(_localbcast, clientid, contextid);
        events += PAMI::Device::LocalReduceWQDevice::Factory::advance(_localreduce, clientid, contextid);
        return events;
    }

    PAMI::Device::Generic::Device *_generics; // need better name...
#ifdef ENABLE_SHMEM_DEVICE
    ShmemDevice *_shmem;
#endif
#ifdef ENABLE_UDP_DEVICE
    UdpDevice *_udp;
#endif
    PAMI::Device::ProgressFunctionDev *_progfunc;
    PAMI::Device::AtomicBarrierDev *_atombarr;
    PAMI::Device::WQRingReduceDev *_wqringreduce;
    PAMI::Device::WQRingBcastDev *_wqringbcast;;
    PAMI::Device::LocalAllreduceWQDevice *_localallreduce;
    PAMI::Device::LocalBcastWQDevice *_localbcast;
    PAMI::Device::LocalReduceWQDevice *_localreduce;
  }; // class PlatformDeviceList

  class Context : public Interface::Context<PAMI::Context>,
                  public Common::Context<PlatformDeviceList>
  {
    public:

      friend class Interface::Context<PAMI::Context>;

      inline Context (pami_client_t client, size_t clientid, size_t contextid, size_t num,
                      PlatformDeviceList *devices,
			PAMI::Memory::MemoryManager * pmm, size_t bytes) :
          Interface::Context<PAMI::Context> (client, contextid),
          Common::Context<PlatformDeviceList> (client, clientid, contextid, num, devices)
      {
        TRACE_ERR((stderr, ">> Context::Context()\n"));
        // ----------------------------------------------------------------
        // Compile-time assertions
        // ----------------------------------------------------------------

        // ----------------------------------------------------------------
        // Compile-time assertions
        // ----------------------------------------------------------------
	char key[PAMI::Memory::MMKEYSIZE];
	sprintf(key, "/pami-clt%zd-ctx%zd-mm", clientid, contextid);
	_mm.init(pmm, bytes, 16, 16, 0, key);
        _devices->init(_clientid, _contextid, _client, _context, &_mm);

        TRACE_ERR((stderr, "<< Context::Context()\n"));
      }

      inline pami_result_t dispatch_impl (size_t                          id,
                                          pami_dispatch_callback_function fn,
                                          void                          * cookie,
                                          pami_dispatch_hint_t            options)
      {
        pami_result_t result = PAMI_ERROR;
        TRACE_ERR((stderr, ">> socklinux::dispatch_impl .. _dispatch[%zu] = %p, result = %d\n", id, _dispatch[id], result));

        // Return an error for invalid / unimplemented 'hard' hints.
        if (
            options.use_rdma             == PAMI_HINT_ENABLE  ||
            options.use_shmem            == PAMI_HINT_DISABLE ||
            false)
          {
            return PAMI_ERROR;
          }

        pami_endpoint_t self = PAMI_ENDPOINT_INIT(_clientid, __global.mapping.task(), _contextid);

        using namespace Protocol::Send;

        Send * send =
          Eager <ShmemModel>::generate (id, fn.p2p, cookie,
                                        ShmemDevice::Factory::getDevice(_devices->_shmem, _clientid, _contextid),
                                        self, _context, options,
                                        __global.heap_mm, result);

        _dispatch.set (id, send, send);
/*
        if (_dispatch[id] == NULL)
          {
            bool no_shmem  = (options.use_shmem == PAMI_HINT3_FORCE_OFF);
#ifndef ENABLE_SHMEM_DEVICE
            no_shmem = true;
#endif

            bool use_shmem = (options.use_shmem == PAMI_HINT3_FORCE_ON);
#ifdef ENABLE_UDP_DEVICE
            use_shmem = true;
#endif

            if (no_shmem == 1)
            {
              // Register only the "udp" datagram protocol
              //
              // This udp datagram protocol code should be changed to respect the
              // "long header" option
              //
#ifdef ENABLE_UDP_DEVICE
              _dispatch[id] = (Protocol::Send::Send *)
                DatagramUdp::generate (id, fn, cookie, _devices->_udp[_contextid], _protocol, result);
#else
              PAMI_abortf("No non-shmem protocols available.");
#endif
            }
            else if (options.use_shmem == PAMI_HINT3_FORCE_ON)
            {
              // Register only the "shmem" eager protocol
#ifdef ENABLE_SHMEM_DEVICE
              if (options.no_long_header == 1)
                {
                  _dispatch[id] = (Protocol::Send::Send *)
                    Protocol::Send::Eager <ShmemModel, ShmemDevice, false>::
                      generate (id, fn.p2p, cookie, ShmemDevice::Factory::getDevice(_devices->_shmem, _clientid, _contextid), self, _context, _protocol, result);
                }
              else
                {
                  _dispatch[id] = (Protocol::Send::Send *)
                    Protocol::Send::Eager <ShmemModel, ShmemDevice, true>::
                      generate (id, fn.p2p, cookie, ShmemDevice::Factory::getDevice(_devices->_shmem, _clientid, _contextid), self, _context, _protocol, result);
                }
#else
              PAMI_abortf("No shmem protocols available.");
#endif
            }
#if defined(ENABLE_SHMEM_DEVICE) && defined(ENABLE_UDP_DEVICE)
            else
            {
              // Register both the "udp" datagram and the "shmem" eager protocols
              //
              // This udp datagram protocol code should be changed to respect the
              // "long header" option
              //
              DatagramUdp * datagram =
                DatagramUdp::generate (id, fn, cookie, _devices->_udp[_contextid], _protocol, result);

              if (options.no_long_header == 1)
                {
                  Protocol::Send::Eager <ShmemModel, ShmemDevice, false> * eager =
                    Protocol::Send::Eager <ShmemModel, ShmemDevice, false>::
                      generate (id, fn, cookie, _devices->_shmem[_contextid], self, _protocol, result);

                  _dispatch[id] = (Protocol::Send::Send *) Protocol::Send::Factory::
                      generate (eager, datagram, _protocol, result);
                }
              else
                {
                  Protocol::Send::Eager <ShmemModel, ShmemDevice, true> * eager =
                    Protocol::Send::Eager <ShmemModel, ShmemDevice, true>::
                      generate (id, fn, cookie, _devices->_shmem[_contextid], self, _protocol, result);

                  _dispatch[id] = (Protocol::Send::Send *) Protocol::Send::Factory::
                      generate (eager, datagram, _protocol, result);
                }
            }
#endif
          }
*/
        TRACE_ERR((stderr, "<< socklinux::dispatch_impl .. result = %d\n", result));
        return result;
      }

    private:

      PAMI::Memory::GenMemoryManager _mm;

      MemoryAllocator<1024, 16> _request;
      ProtocolAllocator _protocol;

  }; // end PAMI::Context
}; // end namespace PAMI
#undef TRACE_ERR
#endif // __common_socklinux_Context_h__
