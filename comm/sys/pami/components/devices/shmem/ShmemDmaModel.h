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
 * \file components/devices/shmem/ShmemDmaModel.h
 * \brief ???
 */

#ifndef __components_devices_shmem_ShmemDmaModel_h__
#define __components_devices_shmem_ShmemDmaModel_h__

#include <errno.h>
#include <sys/uio.h>

#include "Arch.h"
#include "Memregion.h"

#include <pami.h>

#include "components/devices/DmaInterface.h"
#include "components/devices/shmem/ShmemDevice.h"
#include "components/devices/shmem/ShmemDmaMessage.h"
#include "components/devices/shmem/ShmemPacket.h"
#include "components/devices/shmem/ShmemPacketMessage.h"
#include "components/devices/shmem/ShmemFenceMessage.h"
#include "components/devices/shmem/shaddr/ShaddrInterface.h"
#include "components/devices/shmem/shaddr/SystemShaddr.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf x
#endif

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {
      template <class T_Device>
      class ReadOnlyPutMessage : public PacketMessage<T_Device, PacketWriter<void>, true, false>
      {
        public:

          inline ReadOnlyPutMessage (pami_event_function   local_fn,
                                     void                * cookie,
                                     T_Device            * device,
                                     size_t                fnum,
                                     Memregion           * local_memregion,
                                     Memregion           * remote_memregion,
                                     size_t                local_offset,
                                     size_t                remote_offset,
                                     size_t                bytes) :
              PacketMessage<T_Device, PacketWriter<void>, true, false>
              (local_fn, cookie, device, fnum, device->shaddr.system_ro_put_dispatch,
               NULL, 0, (void *) &_info, sizeof(Shaddr::SystemShaddrInfo)),
              _info (local_memregion, remote_memregion,
                     local_offset, remote_offset, bytes)
          {
          };

          inline ~ReadOnlyPutMessage () {};

          Shaddr::SystemShaddrInfo _info;
      };

      template <class T_Device>
      class DmaModelState
      {
        private:
          union
          {
            uint8_t   ro_put_state[sizeof(ReadOnlyPutMessage<T_Device>)];
          };
      };

      ///
      /// \brief Dma model interface implementation for shared memory.
      ///
      template < class T_Device, bool T_Ordered = false >
      class DmaModel : public Interface::DmaModel < DmaModel<T_Device, T_Ordered> >
      {
        public:

          typedef T_Device Device;

          ///
          /// \brief Construct a shared memory dma model.
          ///
          /// \param[in] device  Shared memory device
          ///
          DmaModel (T_Device & device, pami_result_t & status) :
              Interface::DmaModel < DmaModel<T_Device, T_Ordered> >
              (device, status),
              _device (device),
              _context (device.getContext())
          {
            status = PAMI_ERROR;

            if (T_Device::shaddr_mr_supported &&
                (T_Device::shaddr_read_supported ||
                 T_Device::shaddr_write_supported) &&
                device.shaddr.isEnabled())
              {
                status = PAMI_SUCCESS;
              }

            return;
          };

        protected:

          /////////////////////////////////////////////////////////////////////
          //
          // PAMI::Interface::DmaModel interface implementation -- BEGIN
          //
          /////////////////////////////////////////////////////////////////////
          friend class Interface::DmaModel < DmaModel<T_Device, T_Ordered> >;

        public:

          static const size_t dma_model_state_bytes = sizeof(DmaModelState<T_Device>);
          static const bool dma_model_va_supported  = T_Device::shaddr_va_supported;
          static const bool dma_model_mr_supported  = T_Device::shaddr_mr_supported;
          static const bool dma_model_fence_supported = T_Ordered;

        protected:
          

          inline bool postDmaPut_impl (size_t   task,
                                       size_t   bytes,
                                       void   * local,
                                       void   * remote)
          {
#if 0

            // This constant-expression branch will be optimized out by the compiler
            if (! T_Device::shaddr_write_supported)
              PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);

            if (T_Ordered == false)
              {
                _device.shaddr.write (remote, local, bytes, task);
                return true;
              }

#else
            PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
            return false;
#endif
          };

          template <unsigned T_StateBytes>
          inline bool postDmaPut_impl (uint8_t               (&state)[T_StateBytes],
                                       pami_event_function   local_fn,
                                       void                * cookie,
                                       size_t                target,
                                       size_t                target_offset,
                                       size_t                bytes,
                                       void                * local,
                                       void                * remote)
          {
#if 0

            // This constant-expression branch will be optimized out by the compiler
            if (! T_Device::shaddr_write_supported)
              PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);

            if (T_Ordered == false)
              {
                _device.shaddr.write (remote, local, bytes, task);

                if (local_fn) local_fn (_context, cookie, PAMI_SUCCESS);

                return true;
              }

            //
            // Check to see if there are any active or pending messages to
            // the destination, if so add to a "pending dma" queue.
            //
            // Otherwise, simply do the shared address write.
            if (something)
              {
                // Construct a put message and post it to the queue
                PutDmaMessage<T_Device, T_Shaddr, T_Ordered> * msg =
                  (PutDmaMessage<T_Device, T_Shaddr, T_Ordered> *) & state[0];
                new (msg) PutDmaMessage<T_Device, T_Shaddr, T_Ordered> (_device.getQS(0), local_fn, cookie, _device, 0,
                                                                        local, remote, bytes);

                _device.post(0, msg);
                return false
                     }
                   else
              {
                T_Shaddr::write (task, local, remote, bytes);

                if (local_fn) local_fn (_context, cookie, PAMI_SUCCESS);

                return true;
              }

            // Should never get here ...
            PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
            return false;
#else
            PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
            return false;
#endif
          };

          inline bool postDmaPut_impl (size_t                target_task,
                                       size_t                target_offset,
                                       size_t                bytes,
                                       Memregion           * local_memregion,
                                       size_t                local_offset,
                                       Memregion           * remote_memregion,
                                       size_t                remote_offset)
          {
            TRACE_ERR((stderr, ">> postDmaPut_impl():%d\n", __LINE__));

            if (T_Device::shaddr_write_supported)
              {
                size_t fnum = _device.fnum (_device.task2peer(target_task), target_offset);

                if ((T_Ordered == false) ||
                    ((_device.isSendQueueEmpty (fnum)) &&
                     (_device.activePackets(fnum) == false)))
                  {
                    TRACE_ERR((stderr, "   postDmaPut_impl():%d\n", __LINE__));
                    _device.shaddr.write (remote_memregion, remote_offset, local_memregion, local_offset, bytes);
                    TRACE_ERR((stderr, "<< postDmaPut_impl():%d .. return true\n", __LINE__));
                    return true;
                  }
                else
                  {
                    // Ordered writes are required but either the send queue
                    // has pending messages or the fifo has active packets
                    // so the write must wait. Since this is a not-non-blocking
                    // interface it must return false (put not accomplished).
                  }
              }
            else // T_Device::shaddr_write_supported == false
              {
                // Need to perform a "reverse get", a.k.a. "rendezvous" transfer.
                // The origin task must wait for the target task to complete the
                // transfer, which means it must be non-blocking, which means
                // that this method must return false (put not accomplished).
                //
                // Alternatively, this "immediate" put could be accomplished via
                // a "put-over-send" operation IF bytes <= packet_payload.
              }

            TRACE_ERR((stderr, "<< postDmaPut_impl():%d .. return false\n", __LINE__));
            return false;
          };

          template <unsigned T_StateBytes>
          inline bool postDmaPut_impl (uint8_t               (&state)[T_StateBytes],
                                       pami_event_function   local_fn,
                                       void                * cookie,
                                       size_t                target_task,
                                       size_t                target_offset,
                                       size_t                bytes,
                                       Memregion           * local_memregion,
                                       size_t                local_offset,
                                       Memregion           * remote_memregion,
                                       size_t                remote_offset)
          {
            TRACE_ERR((stderr, ">> postDmaPut_impl():%d\n", __LINE__));

            if (! T_Device::shaddr_write_supported)
              {
                // Attempt to inject a "request to reverse get" packet into the
                // fifo. The target task will receive this packet, perform a
                // shared address read, then "consume" the packet from the
                // fifo. At this point the "last reception sequence identifier"
                // will be less than or equal to the sequence identifier of the
                // "request to reverse get" packet, the put operation will be
                // complete, and the origin task will invoke the completion
                // callback.
                size_t fnum = _device.fnum (_device.task2peer(target_task), target_offset);

                if (_device.isSendQueueEmpty (fnum))
                  {
                    // It is safe to place this object on the stack because the
                    // object is no longer needed after the "write single packet"
                    // returns true.  If the packet could not be written the
                    // object will be copied into a pending send message.
                    COMPILE_TIME_ASSERT(sizeof(Shaddr::SystemShaddrInfo) <= T_Device::payload_size);
                    Shaddr::SystemShaddrInfo info(local_memregion, remote_memregion, local_offset, remote_offset, bytes);
                    PacketWriter<void> writer (_device.shaddr.system_ro_put_dispatch);

                    writer.setMetadata (NULL, 0);
                    writer.setData ((void *)&info, sizeof(Shaddr::SystemShaddrInfo));

                    if (_device._fifo[fnum].producePacket(writer))
                      {
                        if (likely(local_fn != NULL))
                          {
                            // Create a "completion message" on the done queue and wait
                            // until the target task has completed the put operation.
                            COMPILE_TIME_ASSERT(T_Device::completion_work_size <= T_StateBytes);

                            size_t sequence = _device._fifo[fnum].lastPacketProduced();
                            array_t<uint8_t, T_Device::completion_work_size> * resized =
                              (array_t<uint8_t, T_Device::completion_work_size> *) state;
                            _device.postCompletion (resized->array, local_fn, cookie, fnum, sequence);

                            TRACE_ERR((stderr, "<< postDmaPut_impl():%d .. return false\n", __LINE__));
                            return false;
                          }

                        // The put operation is considered complete because the
                        // "request to reverse get" packet was successfully written
                        // into the fifo and no completion function was specified.
                        TRACE_ERR((stderr, "<< postDmaPut_impl():%d .. return true\n", __LINE__));
                        return true;
                      }
                  }

                COMPILE_TIME_ASSERT(sizeof(ReadOnlyPutMessage<T_Device>) <= T_StateBytes);

                ReadOnlyPutMessage<T_Device> * msg = (ReadOnlyPutMessage<T_Device> *) state;
                new (msg) ReadOnlyPutMessage<T_Device> (local_fn, cookie, &_device, fnum,
                                                        local_memregion, remote_memregion,
                                                        local_offset, remote_offset, bytes);

                _device.post (fnum, msg);

                return false;
              }
            else // T_Device::shaddr_write_supported == true
              {
                if (T_Ordered == false)
                  {
                    // Perform a shared-address write operation and return
                    _device.shaddr.write (remote_memregion, remote_offset,
                                          local_memregion, local_offset,
                                          bytes);

                    if (likely(local_fn != NULL))
                      local_fn (_context, cookie, PAMI_SUCCESS);

                    return true;
                  }
                else
                  {
                    // Block at head of send queue, then perform a shared
                    // address write operation.

                    size_t fnum = _device.fnum (_device.task2peer(target_task), target_offset);

                    COMPILE_TIME_ASSERT(sizeof(DmaMessage<T_Device>) <= T_StateBytes);

                    DmaMessage<T_Device> * msg =
                      (DmaMessage<T_Device> *) state;
                    new (msg) DmaMessage<T_Device> (local_fn,
                                                    cookie,
                                                    &_device,
                                                    fnum,
                                                    local_memregion,
                                                    local_offset,
                                                    remote_memregion,
                                                    remote_offset,
                                                    bytes,
                                                    false);
                    _device.post (fnum, msg);

                    return false;
                  }
              }

            // Should never get here ...
            return false;
          };

          inline bool postDmaGet_impl (size_t   target_task,
                                       size_t   target_offset,
                                       size_t   bytes,
                                       void   * local,
                                       void   * remote)
          {
#if 0

            // This constant-expression branch will be optimized out by the compiler
            if (! T_Device::shaddr_read_supported)
              PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);

            if (T_Ordered == false)
              {
                _device.shaddr.read (local, remote, bytes, task);
                return true;
              }

#endif
            // Should never get here ...
            PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
            return false;
          };

          template <unsigned T_StateBytes>
          inline bool postDmaGet_impl (uint8_t               (&state)[T_StateBytes],
                                       pami_event_function   local_fn,
                                       void                * cookie,
                                       size_t                target_task,
                                       size_t                target_offset,
                                       size_t                bytes,
                                       void                * local,
                                       void                * remote)
          {
#if 0

            // This constant-expression branch will be optimized out by the compiler
            if (! T_Device::shaddr_read_supported)
              PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);

            if (T_Ordered == false)
              {
                _device.shaddr.read (local, remote, bytes, task);

                if (local_fn) local_fn (_context, cookie, PAMI_SUCCESS);

                return true;
              }

            //
            // Check to see if there are any active or pending messages to
            // the destination, if so add to a "pending dma" queue.
            //
            // Otherwise, simply do the shared address write.
            if (something)
              {
                // Construct a put message and post it to the queue
                GetDmaMessage<T_Device, T_Shaddr, T_Ordered> * msg =
                  (GetDmaMessage<T_Device, T_Shaddr, T_Ordered> *) & state[0];
                new (msg) GetDmaMessage<T_Device, T_Shaddr, T_Ordered> (_device.getQS(0), local_fn, cookie, _device, 0,
                                                                        local, remote, bytes);

                _device.post(0, msg);
                return false
                     }
                   else
              {
                T_Shaddr::read (local, remote, bytes, task);

                if (local_fn) local_fn (_context, cookie, PAMI_SUCCESS);

                return true;
              }

            // Should never get here ...
            PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
            return false;
#else
            PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
            return false;
#endif
          };

          inline bool postDmaGet_impl (size_t                target_task,
                                       size_t                target_offset,
                                       size_t                bytes,
                                       Memregion           * local_memregion,
                                       size_t                local_offset,
                                       Memregion           * remote_memregion,
                                       size_t                remote_offset)
          {
            TRACE_ERR((stderr, ">> Shmem::DmaModel<T_Ordered=%d>::postDmaGet_impl('memregion')\n", T_Ordered));

            // This constant-expression branch will be optimized out by the compiler
            if (! T_Device::shaddr_read_supported)
              PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);

            size_t fnum = _device.fnum (_device.task2peer(target_task), target_offset);

            // If ordering dosn't matter, or if ordering does matter and the
            // send queue is empty and there are no active packets injected
            // from this device ... then do an "immediate" shared address read.
            //
            if ((T_Ordered == false) ||
                ((_device.isSendQueueEmpty (fnum)) &&
                 (_device.activePackets(fnum) == false)))
              {
                TRACE_ERR((stderr, "   Shmem::DmaModel<T_Ordered=%d>::postDmaGet_impl('memregion'), do an 'unordered' shared address read.\n", T_Ordered));
                size_t bytes_copied =
                  _device.shaddr.read (local_memregion, local_offset, remote_memregion, remote_offset, bytes);

                if (likely(bytes_copied == bytes))
                  {
                    TRACE_ERR((stderr, "<< Shmem::DmaModel::postDmaGet_impl('non-blocking memregion'), return true\n"));
                    return true;
                  }
              }

            // Unable to do an "immediate" shared address read.

            TRACE_ERR((stderr, "<< Shmem::DmaModel::postDmaGet_impl('memregion'), return false\n"));
            return false;
          };

          template <unsigned T_StateBytes>
          inline bool postDmaGet_impl (uint8_t               (&state)[T_StateBytes],
                                       pami_event_function   local_fn,
                                       void                * cookie,
                                       size_t                target_task,
                                       size_t                target_offset,
                                       size_t                bytes,
                                       Memregion           * local_memregion,
                                       size_t                local_offset,
                                       Memregion           * remote_memregion,
                                       size_t                remote_offset)
          {
            TRACE_ERR((stderr, ">> Shmem::DmaModel<T_Ordered=%d>::postDmaGet_impl('non-blocking memregion')\n", T_Ordered));

            // This constant-expression branch will be optimized out by the compiler
            if (! T_Device::shaddr_read_supported)
              PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);

            size_t fnum = _device.fnum (_device.task2peer(target_task), target_offset);

            // If ordering dosn't matter, or if ordering does matter and the
            // send queue is empty and there are no active packets injected
            // from this device ... then do an "immediate" shared address read.
            //
            if ((T_Ordered == false) ||
                ((_device.isSendQueueEmpty (fnum)) &&
                 (_device.activePackets(fnum) == false)))
              {
                TRACE_ERR((stderr, "   Shmem::DmaModel<T_Ordered=%d>::postDmaGet_impl('memregion'), do an 'unordered' shared address read.\n", T_Ordered));
                size_t bytes_copied =
                  _device.shaddr.read (local_memregion, local_offset, remote_memregion, remote_offset, bytes);

                if (likely(bytes_copied == bytes))
                  {
                    if (local_fn) local_fn (_context, cookie, PAMI_SUCCESS);
                    TRACE_ERR((stderr, "<< Shmem::DmaModel::postDmaGet_impl('non-blocking memregion'), return true\n"));
                    return true;
                  }
                else
                  {
                    bytes         -= bytes_copied;
                    local_offset  += bytes_copied;
                    remote_offset += bytes_copied;
                  }
              }

            // Block at the head of the send queue, then perform a shared
            // address read operation.
            COMPILE_TIME_ASSERT(sizeof(DmaMessage<T_Device>) <= T_StateBytes);

            DmaMessage<T_Device> * msg =
              (DmaMessage<T_Device> *) state;
            new (msg) DmaMessage<T_Device> (local_fn,
                                            cookie,
                                            &_device,
                                            fnum,
                                            local_memregion,
                                            local_offset,
                                            remote_memregion,
                                            remote_offset,
                                            bytes,
                                            true);
            _device.post (fnum, msg);

            TRACE_ERR((stderr, "<< Shmem::DmaModel::postDmaGet_impl('non-blocking memregion'), return false\n"));
            return false;
          };

          template <unsigned T_StateBytes>
          inline bool postDmaFence_impl (uint8_t               (&state)[T_StateBytes],
                                         pami_event_function   local_fn,
                                         void                * cookie,
                                         size_t                target_task,
                                         size_t                target_offset)
          {
            size_t fnum = _device.fnum (_device.task2peer(target_task), target_offset);

            // If the send queue is empty and there are no active packets
            // injected from this device ... then the fence is already complete.
            if ((_device.isSendQueueEmpty (fnum)) &&
                (_device.activePackets(fnum) == false))
              {
                if (local_fn) local_fn (_context, cookie, PAMI_SUCCESS);
                return true;
              }

            // Post a fence message to the send queue. The fence message simply
            // blocks at the head of the send queue until there are no active
            // packets from this device in the fifo.
            COMPILE_TIME_ASSERT(sizeof(FenceMessage<T_Device>) <= T_StateBytes);
            FenceMessage<T_Device> * msg =
              (FenceMessage<T_Device> *) state;
            new (msg) FenceMessage<T_Device> (local_fn,
                                              cookie,
                                              &_device,
                                              fnum);
            _device.post (fnum, msg);

            return false;
          };

          /////////////////////////////////////////////////////////////////////
          //
          // PAMI::Interface::DmaModel interface implementation -- END
          //
          /////////////////////////////////////////////////////////////////////

          T_Device       & _device;
          pami_context_t   _context;

      };  // PAMI::Device::Shmem::DmaModel class
    };    // PAMI::Device::Shmem namespace
  };      // PAMI::Device namespace
};        // PAMI namespace
#undef TRACE_ERR
#endif // __components_devices_shmem_ShmemDmaModel_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
