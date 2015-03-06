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
 * \file components/devices/shmem/ShmemPacketModel.h
 * \brief ???
 */

#ifndef __components_devices_shmem_ShmemPacketModel_h__
#define __components_devices_shmem_ShmemPacketModel_h__

#include <errno.h>
#include <sys/uio.h>

#include "Arch.h"

#include <pami.h>

#include "components/devices/PacketInterface.h"
#include "components/devices/shmem/ShmemDevice.h"
#include "components/devices/shmem/ShmemPacket.h"
#include "components/devices/shmem/ShmemPacketMessage.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x) // fprintf x
#endif

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {
      ///
      /// \brief Packet model interface implementation for shared memory.
      ///
      /// The shared memory packet model contains the shared memory
      /// packet header which is block-copied into a shared memory message
      /// when it is generated.
      ///
      /// The model also registers the dispatch function with the shared
      /// memory device and stores the dispatch id in the packet header.
      ///
      /// \see ShmemPacketDevice
      ///
      template <class T_Device>
      class PacketModel : public Interface::PacketModel < PacketModel<T_Device> >
      {
        public:

          typedef T_Device Device;

          ///
          /// \brief Construct a shared memory device packet model.
          ///
          /// \param [in] device  Shared memory device
          ///
          PacketModel (T_Device & d) :
              Interface::PacketModel < PacketModel<T_Device> > (d),
              device (d)
          {
            //COMPILE_TIME_ASSERT(sizeof(Shmem::PacketMessage<T_Device>) == sizeof(MultiPacketMessage<T_Device>));
          };

          static const bool   reliable_packet_model             = T_Device::reliable;
          static const bool   deterministic_packet_model        = T_Device::deterministic;
          static const bool   read_is_required_packet_model     = false;

          static const size_t packet_model_metadata_bytes       = T_Device::metadata_size;
          static const size_t packet_model_multi_metadata_bytes = T_Device::metadata_size;
          static const size_t packet_model_payload_bytes        = T_Device::payload_size;
          static const size_t packet_model_immediate_bytes      = T_Device::payload_size;
          static const size_t packet_model_state_bytes          = 512;

          pami_result_t init_impl (size_t                      dispatch,
                                   Interface::RecvFunction_t   recv_func,
                                   void                      * recv_func_parm)
          {
            return device.registerRecvFunction (dispatch, recv_func, recv_func_parm, _dispatch_id);
          };

          template <unsigned T_StateBytes>
          inline bool postPacket_impl (uint8_t               (&state)[T_StateBytes],
                                       pami_event_function   fn,
                                       void                * cookie,
                                       size_t                target_task,
                                       size_t                target_offset,
                                       void                * metadata,
                                       size_t                metasize,
                                       struct iovec_t      * iov,
                                       size_t                niov)
          {
            TRACE_ERR((stderr, ">> PacketModel::postPacket_impl(1)\n"));
            size_t fnum = device.fnum (device.task2peer(target_task), target_offset);
            PacketWriter<struct iovec_t> writer (_dispatch_id);
            writer.setMetadata (metadata, metasize);
            writer.setData (iov, niov);

            if (device.isSendQueueEmpty (fnum))
              {
                if (device._fifo[fnum].producePacket(writer))
                  {
                    if (fn) fn (_context, cookie, PAMI_SUCCESS);

                    TRACE_ERR((stderr, "<< PacketModel::postPacket_impl(1), return true\n"));
                    return true;
                  }
              }

            // Send queue is not empty or not all packets were written to the
            // fifo. Construct a message and post to device
            TRACE_ERR((stderr, "   PacketModel::postPacket_impl(1), post message to device\n"));
            COMPILE_TIME_ASSERT((sizeof(PacketMessage<T_Device, PacketWriter<struct iovec_t> >) + packet_model_metadata_bytes) <= packet_model_state_bytes);

            PacketMessage<T_Device, PacketWriter<struct iovec_t> > * msg =
              (PacketMessage<T_Device, PacketWriter<struct iovec_t> > *) & state[0];

            if (likely(metadata != NULL))
              {
                memcpy ((void *)(msg+1), metadata, packet_model_metadata_bytes);
                writer.setMetadata ((void *)(msg+1), metasize);
              }

            new (msg) PacketMessage<T_Device, PacketWriter<struct iovec_t> > (fn, cookie, &device, fnum, writer);
            device.post (fnum, msg);

            TRACE_ERR((stderr, "<< PacketModel::postPacket_impl(1), return false\n"));
            return false;
          };

          template <unsigned T_StateBytes, unsigned T_Niov>
          inline bool postPacket_impl (uint8_t               (&state)[T_StateBytes],
                                       pami_event_function   fn,
                                       void                * cookie,
                                       size_t                target_task,
                                       size_t                target_offset,
                                       void                * metadata,
                                       size_t                metasize,
                                       struct iovec          (&iov)[T_Niov])
          {
            TRACE_ERR((stderr, ">> PacketModel::postPacket_impl(2), T_Niov = %d\n", T_Niov));
            size_t fnum = device.fnum (device.task2peer(target_task), target_offset);
            PacketIovecWriter<T_Niov> writer (_dispatch_id);
            writer.setMetadata (metadata, metasize);
            writer.setData (iov);

            if (device.isSendQueueEmpty (fnum))
              {
                if (device._fifo[fnum].producePacket(writer))
                  {
                    if (fn) fn (_context, cookie, PAMI_SUCCESS);

                    return true;
                  }
              }

            // Send queue is not empty or not all packets were written to the
            // fifo. Construct a message and post to device
            TRACE_ERR((stderr, "   PacketModel::postPacket_impl(2), post message to device\n"));
            COMPILE_TIME_ASSERT((sizeof(PacketMessage<T_Device, PacketIovecWriter<T_Niov> >) + packet_model_metadata_bytes) <= packet_model_state_bytes);

            PacketMessage<T_Device, PacketIovecWriter<T_Niov> > * msg =
              (PacketMessage<T_Device, PacketIovecWriter<T_Niov> > *) & state[0];

            if (likely(metadata != NULL))
              {
                memcpy ((void *)(msg+1), metadata, packet_model_metadata_bytes);
                writer.setMetadata ((void *)(msg+1), metasize);
              }

            new (msg) PacketMessage<T_Device, PacketIovecWriter<T_Niov> > (fn, cookie, &device, fnum, writer);
            device.post (fnum, msg);

            TRACE_ERR((stderr, "<< PacketModel::postPacket_impl(2), return false\n"));
            return false;
          };

          template <unsigned T_StateBytes>
          inline bool postPacket_impl (uint8_t              (&state)[T_StateBytes],
                                       pami_event_function  fn,
                                       void               * cookie,
                                       size_t               target_task,
                                       size_t               target_offset,
                                       void               * metadata,
                                       size_t               metasize,
                                       void               * payload,
                                       size_t               length)
          {
            TRACE_ERR((stderr, ">> PacketModel::postPacket_impl(0)\n"));

            pami_task_t    t_task;
            size_t         t_offset;
            PAMI_ENDPOINT_INFO(target_task,t_task,t_offset);
            size_t fnum = device.fnum (device.task2peer(target_task), target_offset);
            PacketWriter<void> writer (_dispatch_id);
            writer.setMetadata (metadata, metasize);
            writer.setData (payload, length);

            if (device.isSendQueueEmpty (fnum))
              {
                if (device._fifo[fnum].producePacket(writer))
                  {
                    if (fn) fn (_context, cookie, PAMI_SUCCESS);

                    TRACE_ERR((stderr, "<< PacketModel::postPacket_impl(0), return true\n"));
                    return true;
                  }
              }

            // Send queue is not empty or not all packets were written to the
            // fifo. Construct a message and post to device
            TRACE_ERR((stderr, "   PacketModel::postPacket_impl(0), post message to device\n"));
            COMPILE_TIME_ASSERT((sizeof(PacketMessage<T_Device, PacketWriter<void> >) + packet_model_metadata_bytes) <= packet_model_state_bytes);

            PacketMessage<T_Device, PacketWriter<void> > * msg =
              (PacketMessage<T_Device, PacketWriter<void> > *) & state[0];

            if (likely(metadata != NULL))
              {
                memcpy ((void *)(msg+1), metadata, packet_model_metadata_bytes);
                writer.setMetadata ((void *)(msg+1), metasize);
              }

            new (msg) PacketMessage<T_Device, PacketWriter<void> > (fn, cookie, &device, fnum, writer);
            device.post (fnum, msg);

            TRACE_ERR((stderr, "<< PacketModel::postPacket_impl(0), return false\n"));
            return false;
          };

          template <unsigned T_Niov>
          inline bool postPacket_impl (size_t         target_task,
                                       size_t         target_offset,
                                       void         * metadata,
                                       size_t         metasize,
                                       struct iovec   (&iov)[T_Niov])
          {
            TRACE_ERR((stderr, ">> PacketModel::postPacket_impl(\"immediate\")\n"));
            size_t fnum = device.fnum (device.task2peer(target_task), target_offset);

            if (device.isSendQueueEmpty (fnum))
              {
                PacketIovecWriter<T_Niov> writer (_dispatch_id);
                writer.setMetadata (metadata, metasize);
                writer.setData (iov);
                bool result = device._fifo[fnum].producePacket(writer);
                
                TRACE_ERR((stderr, ">> PacketModel::postPacket_impl(\"immediate\"), return %d\n", result));
                return result;
              }

            TRACE_ERR((stderr, ">> PacketModel::postPacket_impl(\"immediate\"), return false\n"));
            return false;
          };

          template <unsigned T_StateBytes>
          inline bool postMultiPacket_impl (uint8_t               (&state)[T_StateBytes],
                                            pami_event_function   fn,
                                            void                * cookie,
                                            size_t                target_task,
                                            size_t                target_offset,
                                            void                * metadata,
                                            size_t                metasize,
                                            void                * payload,
                                            size_t                length)
          {
            TRACE_ERR((stderr, ">> PacketModel::postMultiPacket_impl()\n"));
            size_t fnum = device.fnum (device.task2peer(target_task), target_offset);
            MultiPacketWriter<void> writer (_dispatch_id);
            writer.setMetadata (metadata, metasize);
            writer.setData (payload, length);

            if (device.isSendQueueEmpty (fnum))
              {
                bool packet_was_produced = false;
                do
                {
                  packet_was_produced = device._fifo[fnum].producePacket(writer);
                  TRACE_ERR((stderr, "   PacketModel::postMultiPacket_impl(), packet_was_produced = %d\n", packet_was_produced));
                  TRACE_ERR((stderr, "   PacketModel::postMultiPacket_impl(), &writer = %p, writer.isDone() = %d\n", &writer, writer.isDone()));

                  if (writer.isDone())
                  {
                    if (fn) fn (_context, cookie, PAMI_SUCCESS);

                    TRACE_ERR((stderr, "<< PacketModel::postMultiPacket_impl(), return true\n"));
                    return true;
                  }

                } while (packet_was_produced);
              }

            // Send queue is not empty or not all packets were written to the
            // fifo. Construct a message and post to device
            TRACE_ERR((stderr, "   PacketModel::postMultiPacket_impl(), post message to device\n"));
            COMPILE_TIME_ASSERT((sizeof(PacketMessage<T_Device, MultiPacketWriter<void> >) + packet_model_metadata_bytes) <= packet_model_state_bytes);

            PacketMessage<T_Device, MultiPacketWriter<void> > * msg =
              (PacketMessage<T_Device, MultiPacketWriter<void> > *) & state[0];

            if (likely(metadata != NULL))
              {
                memcpy ((void *)(msg+1), metadata, packet_model_metadata_bytes);
                writer.setMetadata ((void *)(msg+1), metasize);
              }

            new (msg) PacketMessage<T_Device, MultiPacketWriter<void> > (fn, cookie, &device, fnum, writer);
            device.post (fnum, msg);

            TRACE_ERR((stderr, "<< PacketModel::postMultiPacket_impl(), return false\n"));
            return false;
          };

          ///
          /// \brief Non-public-interface method to retrieve the shared memory device dispatch id.
          ///
          inline uint16_t getDispatchId ()
          {
            return _dispatch_id;
          }

          T_Device       & device;

        protected:

          uint16_t         _dispatch_id;
          pami_context_t   _context;

      };  // PAMI::Device::Shmem::PacketModel class
    };    // PAMI::Device::Shmem namespace
  };      // PAMI::Device namespace
};        // PAMI namespace
#undef TRACE_ERR
#endif // __components_devices_shmem_ShmemPacketModel_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
