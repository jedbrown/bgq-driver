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
 * \file components/devices/bgq/mu2/model/CollectiveMulticastDmaModel.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_model_CollectiveMulticastDmaModel_h__
#define __components_devices_bgq_mu2_model_CollectiveMulticastDmaModel_h__

#include "components/devices/bgq/mu2/Context.h"
#include "components/devices/bgq/mu2/model/CollectiveDmaModelBase.h"
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
#define ZERO_BYTES 65536

      static const size_t mcast_state_bytes = 1;
      class CollectiveMulticastDmaModel: public CollectiveDmaModelBase,
          public Interface::MulticastModel < CollectiveMulticastDmaModel, MU::Context, mcast_state_bytes >
      {
          size_t               _myrank;
          static char        * _zeroBuf;
	  static uint64_t      _zeroBufPA;

        public:
          static const size_t sizeof_msg = mcast_state_bytes;
          static const size_t immediate_bytes = TEMP_BUF_SIZE;

          CollectiveMulticastDmaModel (pami_client_t    client,
                                       pami_context_t   context,
                                       MU::Context                 & device,
                                       pami_result_t               & status) :
              CollectiveDmaModelBase(context, device, status),
              Interface::MulticastModel<CollectiveMulticastDmaModel, MU::Context, mcast_state_bytes >  (device, status), _myrank(__global.mapping.task())
          {
            TRACE_FN_ENTER();

            if (_zeroBuf == NULL)
            {
	      _zeroBuf = (char *) malloc (ZERO_BYTES * sizeof(char));
	      memset (_zeroBuf, 0, ZERO_BYTES);
	      Kernel_MemoryRegion_t memRegion;
	      uint32_t rc;
	      rc = Kernel_CreateMemoryRegion (&memRegion, _zeroBuf, ZERO_BYTES);
	      PAMI_assert ( rc == 0 );
	      _zeroBufPA = (uint64_t)memRegion.BasePa + ((uint64_t)_zeroBuf - (uint64_t)memRegion.BaseVa); 
	    }
            TRACE_FN_EXIT();
          }

          pami_result_t postMulticastImmediate_impl(size_t                client,
                                                    size_t                context,
                                                    pami_multicast_t    * mcast,
                                                    void                * devinfo = NULL)
          {
            TRACE_FN_ENTER();
            pami_result_t result = PAMI_ERROR;
            pami_task_t rank_0 =((Topology *)mcast->src_participants)->index2Rank(0);
            bool isroot = (rank_0 == _myrank);

            unsigned classroute = 0;
            PAMI_assert(devinfo);
            classroute = ((uint32_t)(uint64_t)devinfo) - 1;

            if ( likely(mcast->bytes <= ZERO_BYTES) )
            {
	      char *src = NULL;
	      PipeWorkQueue *dst = NULL;

	      if (isroot)
              {
		PipeWorkQueue *spwq = (PipeWorkQueue *) mcast->src;
		src = spwq->bufferToConsume();
		PAMI_assert(mcast->bytes == spwq->bytesAvailableToConsume());
	      }
	      else
              {
		src = _zeroBuf;
		dst = (PipeWorkQueue*) mcast->dst;
	      }

	      if ( likely (mcast->bytes <= immediate_bytes) ) {
		result = CollectiveDmaModelBase::postShortCollective (MUHWI_COLLECTIVE_OP_CODE_OR,
								  4,
								  mcast->bytes,
								  src,
								  dst,
								  mcast->cb_done.function,
								  mcast->cb_done.clientdata,
								  classroute);
	      }
	      else {
		uint64_t src_pa;
		uint64_t dst_pa;
		
		if (isroot) {
		  Kernel_MemoryRegion_t memRegion;
		  uint32_t rc;
		  rc = Kernel_CreateMemoryRegion (&memRegion, src, mcast->bytes);
		  PAMI_assert ( rc == 0 );
		  uint64_t paddr = (uint64_t)memRegion.BasePa +
		    ((uint64_t)src - (uint64_t)memRegion.BaseVa);
		  
		  src_pa = dst_pa = paddr;
		}
		else {
		  src_pa = _zeroBufPA;
		  Kernel_MemoryRegion_t memRegion;
		  uint32_t rc;
		  char *dstbuf = dst->bufferToProduce();
		  rc = Kernel_CreateMemoryRegion (&memRegion, dstbuf, mcast->bytes);
		  PAMI_assert ( rc == 0 );
		  dst_pa = (uint64_t)memRegion.BasePa +
		    ((uint64_t)dstbuf - (uint64_t)memRegion.BaseVa);		  
		}

		result = CollectiveDmaModelBase::postMidCollective (MUHWI_COLLECTIVE_OP_CODE_OR,
								4,
								mcast->bytes,
								src_pa,
								dst_pa,
								dst,
								mcast->cb_done.function,
								mcast->cb_done.clientdata,	
								classroute);  
	      }
	    }
	    
            TRACE_FN_EXIT();
            if (result == PAMI_SUCCESS)
              return result;
	    
            return CollectiveDmaModelBase::postBroadcast (mcast->bytes,
                                                          (PipeWorkQueue *) mcast->src,
                                                          (PipeWorkQueue *) mcast->dst,
                                                          mcast->cb_done.function,
                                                          mcast->cb_done.clientdata,
                                                          _zeroBuf,
                                                          ZERO_BYTES,
                                                          isroot,
                                                          classroute);
          }

          /// \see PAMI::Device::Interface::MulticastModel::postMulticast
          pami_result_t postMulticast_impl(uint8_t (&state)[mcast_state_bytes],
                                           size_t            client,
                                           size_t            context,
                                           pami_multicast_t *mcast,
                                           void             *devinfo = NULL)
          {
            TRACE_FN_ENTER();
            TRACE_FN_EXIT();
            return PAMI_ERROR;
          }
      };
    };
  };
};
#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG


#endif
