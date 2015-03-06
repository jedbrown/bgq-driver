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
 * \file components/devices/bgq/mu2/model/CollectiveMulticombineDmaModel.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_model_CollectiveMulticombineDmaModel_h__
#define __components_devices_bgq_mu2_model_CollectiveMulticombineDmaModel_h__

#include "components/devices/bgq/mu2/Context.h"
#include "components/devices/bgq/mu2/model/CollectiveDmaModelBase.h"
#include "components/devices/bgq/mu2/model/MU_Collective_OP_DT_Table.h"
#include "sys/pami.h"

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      static const size_t mcomb_state_bytes = 1;
      class CollectiveMulticombineDmaModel: public CollectiveDmaModelBase,
          public Interface::MulticombineModel < CollectiveMulticombineDmaModel, MU::Context, mcomb_state_bytes >
      {
        public:
          static const size_t sizeof_msg = mcomb_state_bytes;

          CollectiveMulticombineDmaModel (pami_client_t    client,
                                          pami_context_t   context,
                                          MU::Context                 & device,
                                          pami_result_t               & status) :
              CollectiveDmaModelBase(context, device, status),
              Interface::MulticombineModel<CollectiveMulticombineDmaModel, MU::Context, mcomb_state_bytes>  (device, status)
          {
            TRACE_FN_ENTER();
            TRACE_FN_EXIT();
          }

          pami_result_t postMulticombineImmediate_impl(size_t                   client,
                                                       size_t                   context,
                                                       pami_multicombine_t    * mcombine,
                                                       void                   * devinfo = NULL)
          {
            unsigned sizeoftype =  mu_collective_size_table[mcombine->dtype];
            unsigned bytes      =  mcombine->count * sizeoftype;
            unsigned op = mu_collective_op_table[mcombine->dtype][mcombine->optor];
            TRACE_FN_ENTER();

            if (op == unsupported_operation)
              PAMI_abortf("Unsupported operation %X\n", mcombine->optor);
              //return PAMI_ERROR; //Unsupported operation

            unsigned classroute = 0;

            PAMI_assert(devinfo);
            classroute = ((uint32_t)(uint64_t)devinfo) - 1;

            PipeWorkQueue *spwq = (PipeWorkQueue *) mcombine->data;
            PipeWorkQueue *dpwq = (PipeWorkQueue *) mcombine->results;
            char *src = spwq->bufferToConsume();
            uint32_t sbytes = spwq->bytesAvailableToConsume();

            pami_result_t result = PAMI_ERROR;
	    
            if ( likely(sbytes == bytes) )
              {
		if (likely (bytes <= CollectiveDmaModelBase::_collstate._tempSize)) {
		  result = CollectiveDmaModelBase::postShortCollective (op,
									sizeoftype,
									bytes,
									src,
									dpwq,
									mcombine->cb_done.function,
									mcombine->cb_done.clientdata,
									classroute);
		}
		else {
		  size_t src_pa, dst_pa;
		  Kernel_MemoryRegion_t memRegion;
		  uint32_t rc = 0;
		  rc = Kernel_CreateMemoryRegion (&memRegion, src, bytes);
		  PAMI_assert ( rc == 0 );
		  uint64_t paddr = (uint64_t)memRegion.BasePa +
		    ((uint64_t)src - (uint64_t)memRegion.BaseVa);		  
		  src_pa = paddr;

		  char *dst = dpwq->bufferToProduce();
		  PAMI_assert (dst != NULL);
		  rc = Kernel_CreateMemoryRegion (&memRegion, dst, bytes);
		  PAMI_assert ( rc == 0 );
		  paddr = (uint64_t)memRegion.BasePa + ((uint64_t)dst - (uint64_t)memRegion.BaseVa);		  
		  dst_pa = paddr;
		  
		  result = CollectiveDmaModelBase::postMidCollective (op,
								      sizeoftype,
								      bytes,
								      src_pa,
								      dst_pa,
								      dpwq,
								      mcombine->cb_done.function,
								      mcombine->cb_done.clientdata,
								      classroute);   
		}
              }
	    
            TRACE_FN_EXIT();
            if (result == PAMI_SUCCESS)
              return result;

            return CollectiveDmaModelBase::postCollective (bytes,
                                                           spwq,
                                                           dpwq,
                                                           mcombine->cb_done.function,
                                                           mcombine->cb_done.clientdata,
                                                           op,
                                                           sizeoftype,
                                                           classroute);	    
          }


          /// \see PAMI::Device::Interface::MulticombineModel::postMulticombine
          pami_result_t postMulticombine_impl(uint8_t (&state)[mcomb_state_bytes],
                                              size_t               client,
                                              size_t               context,
                                              pami_multicombine_t *mcombine,
                                              void                *devinfo = NULL)
          {
            TRACE_FN_ENTER();
            TRACE_FN_EXIT();
            // Get the source data buffer/length and validate (assert) inputs
            return PAMI_ERROR;
          }
      };
    };
  };
};
#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG


#endif
