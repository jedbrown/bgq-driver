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
 * \file components/devices/shmem/shaddr/BgqShaddrPacketModel.h
 * \brief A shmem device packet model with bgq shared address optimization
 *
 * This class overloads a 'standard' shared memory packet model in order
 * to seamlessly provide a shared address optimization for the
 * postMultiPacket() interface. Instead of copying the source data into
 * multiple packets in the fifo, this model copies the bgq shared address
 * information of the source data into multiple packets. Then, the receiver
 * reads the data directly from the address space of the other process.
 */

#ifndef __components_devices_shmem_shaddr_BgqShaddrPacketModel_h__
#define __components_devices_shmem_shaddr_BgqShaddrPacketModel_h__

#include <errno.h>
#include <sys/uio.h>

#include "Arch.h"

#include <pami.h>

#include "components/devices/PacketInterface.h"
#include "components/devices/shmem/ShmemDevice.h"
#include "components/devices/shmem/ShmemPacket.h"
#include "components/devices/shmem/ShmemPacketMessage.h"

#include "components/devices/shmem/shaddr/BgqShaddr.h"

#undef TRACE_ERR
#define TRACE_ERR(x) //fprintf x

#undef TRACE_ERR2
#define TRACE_ERR2(x) //fprintf x

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {
      ///
      /// \brief Packet model interface implementation for shared address.
      ///
      template <class T_Device>
      class BgqShaddrPacketModel : public Interface::PacketModel < BgqShaddrPacketModel<T_Device> >
      {
        public:

          typedef T_Device Device;

          ///
          /// \brief Construct a shared address packet model.
          ///
          /// \param [in] device  Shared memory device
          ///
          BgqShaddrPacketModel (T_Device & device) :
              Interface::PacketModel < BgqShaddrPacketModel<T_Device> > (device),
              _shmem_model (device),
              device (device)
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
          static const size_t packet_model_state_bytes          = PacketModel<T_Device>::packet_model_state_bytes;

          pami_result_t init_impl (size_t                      dispatch,
                                   Interface::RecvFunction_t   recv_func,
                                   void                      * recv_func_parm)
          {
            TRACE_ERR2((stderr, "BgqShaddrPacketModel::init_impl(%zu, %p, %p)\n", dispatch, recv_func, recv_func_parm));
            return _shmem_model.init (dispatch,
                                      recv_func,
                                      recv_func_parm);
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
            TRACE_ERR((stderr, ">> BgqShaddrPacketModel::postPacket_impl(1)\n"));

            bool result =
              _shmem_model.postPacket (state, fn, cookie,
                                       target_task, target_offset,
                                       metadata, metasize,
                                       iov, niov);

            TRACE_ERR((stderr, "<< BgqShaddrPacketModel::postPacket_impl(1), return %d\n", result));
            return result;
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
            TRACE_ERR((stderr, ">> BgqShaddrPacketModel::postPacket_impl(2), T_Niov = %d\n", T_Niov));

            bool result =
              _shmem_model.postPacket (state, fn, cookie,
                                       target_task, target_offset,
                                       metadata, metasize, iov);

            TRACE_ERR((stderr, "<< BgqShaddrPacketModel::postPacket_impl(2), return %d\n", result));
            return result;
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
            TRACE_ERR((stderr, ">> BgqShaddrPacketModel::postPacket_impl(0)\n"));

            bool result =
              _shmem_model.postPacket (state, fn, cookie,
                                       target_task, target_offset,
                                       metadata, metasize,
                                       payload, length);

            TRACE_ERR((stderr, "<< BgqShaddrPacketModel::postPacket_impl(0), return %d\n", result));
            return result;
          };

          template <unsigned T_Niov>
          inline bool postPacket_impl (size_t         target_task,
                                       size_t         target_offset,
                                       void         * metadata,
                                       size_t         metasize,
                                       struct iovec   (&iov)[T_Niov])
          {
            TRACE_ERR((stderr, ">> BgqShaddrPacketModel::postPacket_impl(\"immediate\")\n"));

            bool result =
              _shmem_model.postPacket (target_task, target_offset,
                                       metadata, metasize, iov);

            TRACE_ERR((stderr, "<< BgqShaddrPacketModel::postPacket_impl(\"immediate\"), return %d\n", result));
            return result;
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
            TRACE_ERR((stderr, ">> BgqShaddrPacketModel::postMultiPacket_impl()\n"));

            size_t fnum = device.fnum (device.task2peer(target_task), target_offset);

            COMPILE_TIME_ASSERT(sizeof(BgqShaddr::PhysicalAddress::PacketMessage<T_Device>) <= T_StateBytes);
            COMPILE_TIME_ASSERT(sizeof(BgqShaddr::PhysicalAddress::PacketMessage<T_Device>) <= packet_model_state_bytes);


            BgqShaddr::PhysicalAddress::PacketMessage<T_Device> * msg =
              (BgqShaddr::PhysicalAddress::PacketMessage<T_Device> *) state;

            new (msg) BgqShaddr::PhysicalAddress::PacketMessage<T_Device> (fn, cookie, &device, fnum,
                                                                           device.shaddr.system_paddr_dispatch,
                                                                           _shmem_model.getDispatchId(),
                                                                           metadata, metasize, payload, length);

            if (unlikely(device.isSendQueueEmpty (fnum)))
              {
                if (msg->advance() == PAMI_SUCCESS)
                  return true;
              }

            // Send queue is not empty or not all packets were written to the
            // fifo. Post the message to the device
            TRACE_ERR((stderr, "   BgqShaddrPacketModel::postMultiPacket_impl(), post message to device\n"));

            device.post (fnum, msg);

            TRACE_ERR((stderr, "<< BgqShaddrPacketModel::postMultiPacket_impl(), return false\n"));
            return false;
          };

        protected:

          PAMI::Device::Shmem::PacketModel<T_Device>   _shmem_model;

        public:

          T_Device                                   & device;

      };  // PAMI::Device::Shmem::BgqShaddrPacketModel class
    };    // PAMI::Device::Shmem namespace
  };      // PAMI::Device namespace
};        // PAMI namespace
#undef TRACE_ERR2
#undef TRACE_ERR
#endif // __components_devices_shmem_shaddr_BgqShaddrPacketModel_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
