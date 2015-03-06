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
/// \file components/devices/bgq/mu2/model/CollectiveMcomb2Device.h
/// \brief ???
///
#ifndef __components_devices_bgq_mu2_model_CollectiveMcomb2Device_h__
#define __components_devices_bgq_mu2_model_CollectiveMcomb2Device_h__

#include "components/devices/bgq/mu2/Context.h"
#include "components/devices/bgq/mu2/model/Collective2DeviceBase.h"
#include "components/devices/bgq/mu2/model/MU_Collective_OP_DT_Table.h"
#include "sys/pami.h"

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      //Model classes to do the two device protocol..
      static const size_t mcomb_state_bytes_2dev = 0;
      class CollectiveMcomb2Device: public Collective2DeviceBase,
          public Interface::MulticombineModel < CollectiveMcomb2Device, MU::Context, mcomb_state_bytes_2dev >
      {
        public:
          static const size_t sizeof_msg = mcomb_state_bytes_2dev;

          CollectiveMcomb2Device (pami_client_t    client,
                                          pami_context_t   context,
                                          MU::Context                 & device,
                                          pami_result_t               & status) :
              Collective2DeviceBase(device, status),
              Interface::MulticombineModel<CollectiveMcomb2Device, MU::Context, mcomb_state_bytes_2dev>  (device, status)
          {

          }

          pami_result_t postMulticombineImmediate_impl(size_t                   client,
                                                       size_t                   context,
                                                       pami_multicombine_t    * mcombine,
                                                       void                   * devinfo = NULL)
          {
            unsigned sizeoftype =  mu_collective_size_table[mcombine->dtype];
            unsigned bytes      =  mcombine->count * sizeoftype;
            unsigned op = mu_collective_op_table[mcombine->dtype][mcombine->optor];
            pami_op opcode = mcombine->optor;

            //op = mu_collective_op_table[PAMI_DOUBLE][PAMI_MAX];
            if (op == unsupported_operation)
              PAMI_abortf("Unsupported operation %X\n", mcombine->optor);
              //return PAMI_ERROR; //Unsupported operation

            unsigned classroute = 0;

            PAMI_assert(devinfo);
            classroute = ((uint32_t)(uint64_t)devinfo) - 1;

            PipeWorkQueue *spwq = (PipeWorkQueue *) mcombine->data;
            PipeWorkQueue *dpwq = (PipeWorkQueue *) mcombine->results;
            

            return Collective2DeviceBase::postCollective (bytes,
                                                           spwq,
                                                           dpwq,
                                                           mcombine->cb_done.function,
                                                           mcombine->cb_done.clientdata,
                                                           op,
                                                           opcode,                                    
                                                           sizeoftype,
                                                          classroute);
          }


          pami_result_t postMulticombine_impl(uint8_t (&state)[mcomb_state_bytes_2dev],
                                              size_t               client,
                                              size_t               context,
                                              pami_multicombine_t *mcombine,
                                              void                *devinfo = NULL)
          {
            return PAMI_ERROR;
          }
      };

      // This class is empty..
      static const size_t mcast_state_bytes_2dev = 0;
      class CollectiveMcast2Device: public Collective2DeviceBase,
          public Interface::MulticastModel < CollectiveMcast2Device, MU::Context, mcast_state_bytes_2dev >
      {
        public:
          static const size_t sizeof_msg = mcast_state_bytes_2dev;

          CollectiveMcast2Device (pami_client_t    client,
              pami_context_t   context,
              MU::Context                 & device,
              pami_result_t               & status) :
            Interface::MulticastModel<CollectiveMcast2Device, MU::Context, mcast_state_bytes_2dev>  (device, status)
        { }

          pami_result_t postMulticastImmediate_impl(size_t                client,
              size_t                context,
              pami_multicast_t    * mcast,
              void                * devinfo = NULL)
          { 
            return PAMI_ERROR; 
          }
          pami_result_t postMulticast_impl(uint8_t (&state)[mcast_state_bytes_2dev],
              size_t            client,
              size_t            context,
              pami_multicast_t *mcast,
              void             *devinfo = NULL)
          {
            //TRACE_FN_ENTER();
            return PAMI_ERROR;
            //TRACE_FN_EXIT();
          }

      };

      // This class is empty..
      class CollectiveMsync2Device : public Interface::MultisyncModel<CollectiveMsync2Device, MU::Context, 0>
      {
      public:
	static const size_t sizeof_msg = 0;

	CollectiveMsync2Device (pami_client_t     client,
			  pami_context_t    context,
			  MU::Context     & mucontext,
			  pami_result_t   & status):
	Interface::MultisyncModel<CollectiveMsync2Device, MU::Context, 0> (mucontext, status)
	  {	    
	  } 
	
	/// \see PAMI::Device::Interface::MultisyncModel::postMultisync
	pami_result_t postMultisyncImmediate_impl(size_t            client,
						  size_t            context, 
						  pami_multisync_t *msync,
						  void             *devinfo = NULL) 
	{
	  return PAMI_ERROR;
	}

	pami_result_t postMultisync_impl(uint8_t (&state)[0],
					 size_t            client,
					 size_t            context, 
					 pami_multisync_t *msync,
					 void             *devinfo = NULL) 
	{
	  return PAMI_ERROR;
	}
      };

    };
  };
};

#endif
