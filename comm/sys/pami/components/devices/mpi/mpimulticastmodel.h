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
 * \file components/devices/mpi/mpimulticastmodel.h
 * \brief ???
 */

#ifndef __components_devices_mpi_mpimulticastmodel_h__
#define __components_devices_mpi_mpimulticastmodel_h__

#include <pami.h>
#include "components/devices/MulticastModel.h"
#include "components/devices/mpi/mpimessage.h"

namespace PAMI
{
    namespace Device
    {
        template <class T_Device, class T_Message>
    class MPIMulticastModel :
      public Interface::AMMulticastModel<MPIMulticastModel<T_Device, T_Message>,T_Device, sizeof(T_Message)>
        {
        public:
      static const size_t mcast_model_state_bytes = sizeof(T_Message);
      static const size_t sizeof_msg              = sizeof(T_Message);
      static const bool   is_active_message       = true;


      MPIMulticastModel (T_Device & device, pami_result_t &status) :
        Interface::AMMulticastModel < MPIMulticastModel<T_Device, T_Message>, T_Device, sizeof(T_Message) > (device, status),
        _device(device)
                {
          status = PAMI_SUCCESS;
        };

      inline pami_result_t registerMcastRecvFunction_impl (int                        dispatch_id,
                                                          pami_dispatch_multicast_function recv_func,
                                                          void                      *async_arg)
                {
          _device.registerMcastRecvFunction (dispatch_id,recv_func, async_arg);
          return PAMI_SUCCESS;
                }

      inline pami_result_t postMulticast_impl (uint8_t (&state)[mcast_model_state_bytes],
					       size_t           client,
					       size_t           context, 
                                               pami_multicast_t *mcast,
                                               void *devinfo)
                {
          pami_result_t      rc         = PAMI_SUCCESS;
          MPIMcastMessage  *msg        = (MPIMcastMessage *) state;
          unsigned          myrank     = __global.mapping.task();
          msg->_cb_done                = mcast->cb_done;
          msg->_p2p_msg._connection_id = mcast->connection_id;
          msg->_srcranks               = (PAMI::Topology*)mcast->src_participants;
          msg->_dstranks               = (PAMI::Topology*)mcast->src_participants;
          msg->_srcpwq                 = (PAMI::PipeWorkQueue*)mcast->src;
          msg->_dstpwq                 = (PAMI::PipeWorkQueue*)mcast->dst;
          msg->_root                   = msg->_srcranks->index2Rank(0);
          msg->_bytes                  = mcast->bytes;


          PAMI_abort();

          if(msg->_dstpwq)
                {
                msg->_dstranks->rankList(&msg->_ranks);
                msg->_numRanks = msg->_dstranks->size();
                }

          if(msg->_srcpwq)
                {
                msg->_currBytes = msg->_srcpwq->bytesAvailableToConsume();
                msg->_currBuf   = msg->_srcpwq->bufferToConsume();
                }

          if(msg->_root == myrank)
                {

                }
          return rc;
                }
      T_Device                  &_device;
      pami_dispatch_multicast_function _cb_async_head;
      void                      *_async_arg;
        };
    };
};
#endif // __components_devices_mpi_mpimulticastmodel_h__
