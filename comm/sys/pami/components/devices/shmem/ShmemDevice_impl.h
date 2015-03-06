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
 * \file components/devices/shmem/ShmemDevice_impl.h
 * \brief ???
 */

#ifndef __components_devices_shmem_ShmemDevice_impl_h__
#define __components_devices_shmem_ShmemDevice_impl_h__

#include "Platform.h"
#include "Global.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf x
#endif

namespace PAMI
{
  namespace Device
  {
    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    pami_result_t ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::init (pami_client_t  client,
                                                                    pami_context_t context)
    {
      TRACE_ERR((stderr, "(%zu) ShmemDevice::init ()\n", __global.mapping.task()));
      _client   = client;
      _context  = context;

      return PAMI_SUCCESS;
    }

    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    bool ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::isInit_impl ()
    {
      return true;
    }

    /// \see PAMI::Device::Interface::BaseDevice::peers()
    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    size_t ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::peers_impl ()
    {
      return _npeers;
    }

    /// \see PAMI::Device::Interface::BaseDevice::task2peer()
    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    size_t ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::task2peer_impl (size_t task)
    {
      PAMI::Interface::Mapping::nodeaddr_t address;
      TRACE_ERR((stderr, ">> ShmemDevice::task2peer_impl(%zu)\n", task));
      __global.mapping.task2node (task, address);
      TRACE_ERR((stderr, "   ShmemDevice::task2peer_impl(%zu), address = {%zu, %zu}\n", task, address.global, address.local));

      size_t peer = 0;
      __global.mapping.node2peer (address, peer);
      TRACE_ERR((stderr, "<< ShmemDevice::task2peer_impl(%zu), peer = %zu\n", task, peer));

      return peer;
    }

    /// \see PAMI::Device::Interface::BaseDevice::isPeer()
    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    bool ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::isPeer_impl (size_t task)
    {
      return __global.mapping.isLocal(task);
    };

    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    pami_result_t ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::registerRecvFunction (size_t                      set,
        Interface::RecvFunction_t   recv_func,
        void                      * recv_func_parm,
        uint16_t                  & id)
    {
      return _dispatch.registerUserDispatch (set, recv_func, recv_func_parm, id);
    };


    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    pami_result_t ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::registerSystemRecvFunction (Interface::RecvFunction_t   recv_func,
        void                      * recv_func_parm,
        uint16_t                  & id)
    {
      return _dispatch.registerSystemDispatch (recv_func, recv_func_parm, id);
    };

    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    void ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::dispatch (uint16_t id,
                                                               void * metadata,
                                                               void * payload,
                                                               size_t bytes)
    {
      _dispatch.dispatch(id, metadata, payload, bytes);
    };

    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    pami_result_t ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::post (size_t fnum, Shmem::SendQueue::Message * msg)
    {
      TRACE_ERR((stderr, ">> (%zu) ShmemDevice::post(%zu, %p)\n", __global.mapping.task(), fnum, msg));
      PAMI_assert_debug (fnum < _nfifos);
      _sendQ[fnum].post(msg);
      TRACE_ERR((stderr, "<< (%zu) ShmemDevice::post(%zu, %p)\n", __global.mapping.task(), fnum, msg));
      return PAMI_SUCCESS;
    };

    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    template <unsigned T_StateBytes>
    pami_result_t ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::postCompletion
    (uint8_t               (&state)[T_StateBytes],
     pami_event_function   local_fn,
     void                * cookie,
     size_t                fnum,
     size_t                sequence)
    {
      TRACE_ERR((stderr, ">> (%zu) ShmemDevice::postCompletion(%p,%p,%p,%zu,%zu), _local_progress_device = %p\n", __global.mapping.task(), state, local_fn, cookie, fnum, sequence, _local_progress_device));

      COMPILE_TIME_ASSERT(sizeof(Shmem::RecPacketWork<T_Fifo>) <= T_StateBytes);

      Shmem::RecPacketWork<T_Fifo> * work = (Shmem::RecPacketWork<T_Fifo> *) state;
      new (work) Shmem::RecPacketWork<T_Fifo> (_fifo[fnum], local_fn, cookie, sequence);
      _local_progress_device->postThread (work);

      TRACE_ERR((stderr, "<< (%zu) ShmemDevice::postCompletion(%p,%p,%p,%zu,%zu)\n", __global.mapping.task(), state, local_fn, cookie, fnum, sequence));
      return PAMI_SUCCESS;
    };

#ifdef SHM_USE_COLLECTIVE_FIFO
    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    char * ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::getUniqueString ()
    {
      return _unique_str;
    };
#endif





#ifdef SHM_USE_COLLECTIVE_FIFO
    template <class T_Fifo, class T_Atomic, class T_Shaddr, unsigned T_FifoCount, unsigned T_SetCount>
    pami_result_t ShmemDevice<T_Fifo, T_Atomic, T_Shaddr, T_FifoCount, T_SetCount>::getShmemWorldDesc
    (typename CollectiveFifo::Descriptor ** desc)
      {
        unsigned desc_index;
        typename CollectiveFifo::Descriptor * next_free_desc = _desc_fifo.next_free_descriptor(desc_index);

        if (likely(next_free_desc != NULL))
        {
          if (likely(next_free_desc->get_my_seq_id()  == next_free_desc->get_seq_id()))
          {
            *desc = _desc_fifo.fetch_descriptor();
            TRACE_ERR((stderr, "Found descriptor pair \n"));
            return PAMI_SUCCESS;
          }
        }
        return PAMI_EAGAIN;
      };
#endif



  };
};
#undef TRACE_ERR

#endif

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
