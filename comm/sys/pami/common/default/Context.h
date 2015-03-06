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
/// \file common/default/Context.h
/// \brief PAMI default context implementation.
///
#ifndef __common_default_Context_h__
#define __common_default_Context_h__

#define ENABLE_SHMEM_DEVICE
//#define ENABLE_UDP_DEVICE

#include <stdlib.h>
#include <string.h>

#include <pami.h>
#include "common/default/Dispatch.h"

#include "components/devices/generic/Device.h"
#include "components/devices/misc/ProgressFunctionMsg.h"
#include "components/devices/misc/AtomicBarrierMsg.h"
#include "components/devices/workqueue/WQRingReduceMsg.h"
#include "components/devices/workqueue/WQRingBcastMsg.h"
#include "components/devices/workqueue/LocalAllreduceWQMessage.h"
#include "components/devices/workqueue/LocalReduceWQMessage.h"
#include "components/devices/workqueue/LocalBcastWQMessage.h"

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
//#include "components/atomic/pthread/Pthread.h"

#include "components/memory/MemoryAllocator.h"
#include "components/memory/MemoryManager.h"
//#include "Memregion.h"

#include "p2p/protocols/send/eager/Eager.h"
#ifdef ENABLE_UDP_DEVICE
#include "p2p/protocols/send/datagram/Datagram.h"
#endif
#include "p2p/protocols/send/composite/Composite.h"

#include "p2p/protocols/get/Get.h"
#ifndef TRACE_ERR
#define TRACE_ERR(x) // fprintf x
#endif

namespace PAMI
{

namespace Common
{
  template <class T_DeviceList>
  class Context
  {
    public:
      inline Context (pami_client_t   client,
                      size_t          clientid,
                      size_t          contextid,
                      size_t          num,
                      T_DeviceList  * devices) :
          _client (client),
          _context ((pami_context_t)this),
          _clientid (clientid),
          _contextid (contextid),
          _dispatch (),
          _devices (devices)
      {
        // ----------------------------------------------------------------
        // Compile-time assertions
        // ----------------------------------------------------------------

        // ----------------------------------------------------------------
        // Compile-time assertions
        // ----------------------------------------------------------------
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
        //return PAMI_UNIMPL;
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

        for (i = 0; i < maximum && events == 0; i++)
          {
                events += _devices->advance(_clientid, _contextid);
          }

        if (events > 0) result = PAMI_SUCCESS;

        return events;
      }

      inline pami_result_t lock_impl ()
      {
        //_lock.acquire ();
        return PAMI_SUCCESS;
      }

      inline pami_result_t trylock_impl ()
      {
        //if (_lock.tryAcquire ()) {
                return PAMI_SUCCESS;
        //}
        //return PAMI_EAGAIN;
      }

      inline pami_result_t unlock_impl ()
      {
        //_lock.release ();
        return PAMI_SUCCESS;
      }

      inline pami_result_t send_impl (pami_send_t * parameters)
      {
        return _dispatch.start (parameters);
      }

      inline pami_result_t send_impl (pami_send_immediate_t * parameters)
      {
        return _dispatch.start (parameters);
      }

      inline pami_result_t send_impl (pami_send_typed_t * parameters)
      {
        //return _dispatch.start (parameters);
        return PAMI_UNIMPL;
      }

      inline pami_result_t put_impl (pami_put_simple_t * parameters)
      {
        //return _dispatch.start (parameters);
        return PAMI_UNIMPL;
      }

      inline pami_result_t put_typed_impl (pami_put_typed_t * parameters)
      {
        //return _dispatch.start (parameters);
        return PAMI_UNIMPL;
      }

      inline pami_result_t get_impl (pami_get_simple_t * parameters)
      {
        //return _dispatch.start (parameters);
        return PAMI_UNIMPL;
      }

      inline pami_result_t get_typed_impl (pami_get_typed_t * parameters)
      {
        //return _dispatch.start (parameters);
        return PAMI_UNIMPL;
      }

      inline pami_result_t rmw_impl (pami_rmw_t * parameters)
      {
        //return _dispatch.start (parameters);
        return PAMI_UNIMPL;
      }

      inline pami_result_t memregion_create_impl (void             * address,
                                                  size_t             bytes_in,
                                                  size_t           * bytes_out,
                                                  pami_memregion_t * memregion)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t memregion_destroy_impl (pami_memregion_t * memregion)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t rput_impl (pami_rput_simple_t * parameters)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t rput_typed_impl (pami_rput_typed_t * parameters)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t rget_impl (pami_rget_simple_t * parameters)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t rget_typed_impl (pami_rget_typed_t * parameters)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t purge_totask_impl (pami_endpoint_t *dest, size_t count)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t resume_totask_impl (pami_endpoint_t *dest, size_t count)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t fence_begin_impl ()
      {
        _dispatch.fence (true);
        return PAMI_SUCCESS;
      }

      inline pami_result_t fence_end_impl ()
      {
        _dispatch.fence (false);
        return PAMI_SUCCESS;
      }

      inline pami_result_t fence_all_impl (pami_event_function   done_fn,
                                           void                * cookie)
      {
        return _dispatch.fence (done_fn, cookie);
      }

      inline pami_result_t fence_endpoint_impl (pami_event_function   done_fn,
                                                void                * cookie,
                                                pami_endpoint_t       endpoint)
      {
        return _dispatch.fence (done_fn, cookie, endpoint);
      }

      inline pami_result_t geometry_initialize_impl (pami_geometry_t       * geometry,
                                                     unsigned                id,
                                                     pami_geometry_range_t * rank_slices,
                                                     size_t                  slice_count)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t geometry_world_impl (pami_geometry_t * world_geometry)
      {
        return PAMI_UNIMPL;
      }


      inline pami_result_t geometry_finalize_impl (pami_geometry_t geometry)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t collective_impl (pami_xfer_t * parameters)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t amcollective_dispatch_impl (pami_algorithm_t            algorithm,
                                                       size_t                      dispatch,
                                                       pami_dispatch_callback_function fn,
                                                       void                      * cookie,
                                                       pami_collective_hint_t      options)
      {
        PAMI_abort();
        return PAMI_SUCCESS;
      }

      inline pami_result_t geometry_algorithms_num_impl (pami_geometry_t geometry,
                                                         pami_xfer_type_t ctype,
                                                         size_t *lists_lengths)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t geometry_algorithms_info_impl (pami_geometry_t geometry,
                                                          pami_xfer_type_t colltype,
                                                          pami_algorithm_t  *algs0,
                                                          pami_metadata_t   *mdata0,
                                                          size_t               num0,
                                                          pami_algorithm_t  *algs1,
                                                          pami_metadata_t   *mdata1,
                                                          size_t               num1)
      {
        PAMI_abort();
        return PAMI_SUCCESS;
      }

      inline pami_result_t dispatch_query_impl(size_t                id,
                                               pami_configuration_t  configuration[],
                                               size_t                num_configs)
      {
        return _dispatch.query (id, configuration, num_configs);
      }

      inline pami_result_t dispatch_update_impl(size_t                id,
                                                pami_configuration_t  configuration[],
                                                size_t                num_configs)
      {
        return _dispatch.update (id, configuration, num_configs);
      }

      inline pami_result_t query_impl(pami_configuration_t  configuration[],
                                      size_t                num_configs)
      {
        return PAMI_UNIMPL;
      }

      inline pami_result_t update_impl(pami_configuration_t  configuration[],
                                       size_t                num_configs)
      {
        return PAMI_UNIMPL;
      }

    protected:

      pami_client_t  _client;
      pami_context_t _context;
      size_t         _clientid;
      size_t         _contextid;

      Dispatch _dispatch;
      T_DeviceList *_devices;

    }; // end class   PAMI::Common::Context
  }; // end namespace PAMI::Common
}; // end namespace   PAMI
#undef TRACE_ERR
#endif // __common_socklinux_Context_h__
