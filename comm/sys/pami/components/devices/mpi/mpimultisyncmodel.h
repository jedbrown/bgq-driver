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
 * \file components/devices/mpi/mpimultisyncmodel.h
 * \brief ???
 */

#ifndef __components_devices_mpi_mpimultisyncmodel_h__
#define __components_devices_mpi_mpimultisyncmodel_h__

#include <pami.h>
#include "components/devices/MultisyncModel.h"
#include "components/devices/mpi/mpimessage.h"

namespace PAMI
{
  namespace Device
  {
    template <class T_Device, class T_Message>
    class MPIMultisyncModel :
      public Interface::MultisyncModel<MPIMultisyncModel<T_Device, T_Message>,T_Device,sizeof(T_Message)>
    {
    public:
      static const size_t msync_model_state_bytes = sizeof(T_Message);
      static const size_t sizeof_msg              = sizeof(T_Message);
      MPIMultisyncModel (T_Device &device, pami_result_t &status) :
        Interface::MultisyncModel<MPIMultisyncModel<T_Device, T_Message>,T_Device,sizeof(T_Message)>(device, status),
        _device(device)
        {
          status = PAMI_SUCCESS;
        }

      pami_result_t postMultisync (uint8_t         (&state)[msync_model_state_bytes],
				   size_t           client,
				   size_t           context,
                                   pami_multisync_t *msync,
                                   void             *devinfo=NULL)
        {
          pami_result_t      rc     = PAMI_SUCCESS;
          MPIMsyncMessage *msg     = (MPIMsyncMessage *) state;
          PAMI::Topology   *topo    = (PAMI::Topology *)msync->participants;
          pami_task_t      *ranks_h = NULL;
          pami_task_t      *ranks   = NULL;
          size_t           size    = topo->size();
          size_t          myidx    = topo->rank2Index(__global.mapping.task());
          topo->rankList(&ranks_h);
          ranks = ranks_h;
          msg->_cb_done            = msync->cb_done;
          msg->_numphases          = -1;
          for (int n=2*size-1;n>0;n>>=1)msg->_numphases++;
          msg->_sendcomplete       = 0;
          msg->_phase              = 0;
          msg->_sendStarted        = false;
          msg->_sendDone           = false;
          msg->_recvDone           = false;
          for(size_t i=0;i<msg->_numphases;i++)
              {
                msg->_dests[i] = topo->index2Rank((myidx+(1<<i))%size);
                msg->_srcs[i]  = topo->index2Rank((myidx-(1<<i)+size)%size);
              }
          _device.enqueue(msg);
          return rc;
        }

      T_Device                     &_device;
      size_t                        _dispatch_id;
      void                         *_async_arg;
    };
  };
};
#endif // __components_devices_mpi_mpimultisyncmodel_h__
