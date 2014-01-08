/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2011, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */


/**
 * \file fence.c
 *
 * \brief C File containing Comm Agent Fence Implementation
 */


#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <malloc.h>
#include <hwi/include/bqc/MU_Addressing.h>
#include <hwi/include/bqc/MU_Fifo.h>
#include <hwi/include/bqc/MU_Macros.h>
#include <spi/include/kernel/memory.h>
#include <spi/include/l2/atomic.h>
#include <spi/include/mu/Descriptor.h>
#include <spi/include/kernel/MU.h>
#include <agents/include/comm/commagent.h>
#include <agents/include/comm/fence.h>
#include "fence_internal.h"
#include "commagent_internal.h"

#ifdef TRACE
#undef TRACE
#endif
#define TRACE(x) //fprintf x



/**
 * \brief Global variables
 *
 */

typedef enum fenceState
  {
    FENCE_STATE_UNINITIALIZED,
    FENCE_STATE_INITIALIZED,
    FENCE_STATE_DISABLED
  } fenceState_t;
static fenceState_t _fenceState = FENCE_STATE_UNINITIALIZED;

static int _numInitRequests = 0; /* Number of fence init requests seen. */


/**
 * \brief Fence Dispatch Function
 *
 * This function receives control when a fence packet arrives.
 */
int fenceDispatchFn( void                 *recv_func_parm,
		     MUHWI_PacketHeader_t *packet_ptr,
		     uint32_t              packet_bytes
		   )
{
  TRACE((stderr,"%s() [%s:%d]: >> fenceDispatchFn()\n",__FUNCTION__,__FILE__,__LINE__));

  /* The descriptor to be injected is in the packet payload */
  MUHWI_Descriptor_t *desc = (MUHWI_Descriptor_t*)(packet_ptr + 1);

  /* Inject the descriptor.
   * \todo If the inj fifo is full, don't spin.  Rather, add a work item
   *       to a queue and inject it later.  For now, keep trying to inject
   *       until the fifo is not full anymore.
   */
  uint64_t seqNum;
  do {
    seqNum = MUSPI_InjFifoInject ( _ififo,
				   desc );
  } while (seqNum == (uint64_t)-1 );

  TRACE((stderr,"%s() [%s:%d]: << fenceDispatchFn()\n",__FUNCTION__,__FILE__,__LINE__));

  return 0;
}


/**
 * \brief Process a Fence Init Work Request
 *
 * This function initializes the comm agent for handling fence
 * work requests.
 *
 * \param[in]  workRequestPtr  Pointer to the work request in the request queue.
 */
void processFenceInitWorkRequest( 
       CommAgent_FenceInit_WorkRequest_t *workRequestPtr )
{
  TRACE((stderr,"%s() [%s:%d]: processFenceInitWorkRequest()\n",__FUNCTION__,__FILE__,__LINE__));

  /* Ignore subsequent init requests. */
  if ( _numInitRequests++ > 0 ) return; 

  /* Perform init:
   * - Register fence dispatch routine
   */
  commAgent_RegisterDispatch( COMM_AGENT_FENCE_DISPATCH_ID,
			      fenceDispatchFn,
			      NULL /* Cookie */ );

  _fenceState = FENCE_STATE_INITIALIZED;
}


#undef TRACE
