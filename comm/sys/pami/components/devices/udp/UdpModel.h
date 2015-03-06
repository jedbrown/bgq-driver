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
 * \file components/devices/udp/UdpModel.h
 * \brief Provides the implementation of the PacketModel for the UDP device
 */

#ifndef __components_devices_udp_UdpModel_h__
#define __components_devices_udp_UdpModel_h__

#include <sys/uio.h>
#include <pami.h>
#include "components/devices/PacketInterface.h"
#include "components/devices/udp/UdpMessage.h"
#include "errno.h"

namespace PAMI
{
  namespace Device
  {
  namespace UDP
  {
    template <class T_Device, class T_Message>
    class UdpModel : public Interface::PacketModel<UdpModel<T_Device, T_Message>, T_Device, sizeof(T_Message)>
    {
    public:
       UdpModel (T_Device & device) :
            Interface::PacketModel < UdpModel<T_Device, T_Message>, T_Device, sizeof(T_Message) > (device),
            _device (device),
            _context (device.getContext())
        {};

       // PacketInterface implements isPacket* to return these variables
      static const bool   deterministic_packet_model   = false;
      static const bool   reliable_packet_model        = false;
      static const bool   read_is_required_packet_model = false;

      static const size_t packet_model_metadata_bytes       = T_Device::metadata_size;
      static const size_t packet_model_multi_metadata_bytes = T_Device::metadata_size;
      static const size_t packet_model_payload_bytes        = T_Device::payload_size;
      static const size_t packet_model_immediate_bytes      = 0;
      static const size_t packet_model_state_bytes          = sizeof(T_Message);

      pami_result_t init_impl (size_t                     dispatch,
                              Interface::RecvFunction_t   recv_func,
                              void                      * recv_func_parm)
        {
         return _device.setDispatchFunc (dispatch, recv_func, recv_func_parm, _device_dispatch_id);
        };

      inline bool postPacket_impl (uint8_t              (&state)[UdpModel::packet_model_state_bytes],
                                   pami_event_function   fn,
                                   void               * cookie,
                                   pami_task_t           target,
                                   size_t               contextid,
                                   void               * metadata,
                                   size_t               metasize,
                                   struct iovec       * iov,
                                   size_t               niov)
        {
          T_Message * msg = (T_Message *) & state[0];
          new (msg) T_Message (_context, fn, cookie, _device_dispatch_id,
                               metadata, metasize, iov, niov, false);
          _device.post(target, msg );
          return false;
        };

      template <unsigned T_Niov>
      inline bool postPacket_impl (uint8_t              (&state)[UdpModel::packet_model_state_bytes],
                                   pami_event_function   fn,
                                   void               * cookie,
                                   pami_task_t           target,
                                   size_t               contextid,
                                   void               * metadata,
                                   size_t               metasize,
                                   struct iovec         (&iov)[T_Niov])
        {
            return postPacket(state, fn, cookie, target, contextid, metadata, metasize, iov, T_Niov );
        };

      template <unsigned T_Niov>
      inline bool postPacket_impl (pami_task_t     target,
                                   size_t         contextid,
                                   void         * metadata,
                                   size_t         metasize,
                                   struct iovec   (&iov)[T_Niov])
        {
          // Cannot ensure that this will send right away, so just indicate this doesn't work
          return false;

        }

      template <unsigned T_Niov>
      inline bool postMultiPacket_impl (uint8_t              (&state)[UdpModel::packet_model_state_bytes],
                                        pami_event_function   fn,
                                        void               * cookie,
                                        pami_task_t           target,
                                        size_t               contextid,
                                        void               * metadata,
                                        size_t               metasize,
                                        struct iovec         (&iov)[T_Niov])
        {
         T_Message * msg = (T_Message *) & state[0];
          new (msg) T_Message (_context, fn, cookie, _device_dispatch_id, metadata,
                               metasize, iov, T_Niov, true);
          _device.post(target, msg );
          return false;
        }

       inline int read_impl(void * dst, size_t bytes, void * cookie )
       {
         return _device.read(dst, bytes, cookie);
       }

    protected:
      T_Device                   & _device;
      pami_context_t                _context;
      uint32_t                     _usr_dispatch_id;
      size_t                       _device_dispatch_id;

    };
  };
  };
};
#endif // __components_devices_udp_udppacketmodel_h__
