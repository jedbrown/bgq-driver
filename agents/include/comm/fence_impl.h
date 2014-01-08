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

#ifndef  __COMM_AGENT_FENCE_IMPL__
#define  __COMM_AGENT_FENCE_IMPL__


/**
 * \file fence_impl.h
 *
 * \brief C Header File containing fence Inline Implementations
 *        This header is included by fence.h.
 */


#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/MU_Addressing.h>
#include <spi/include/kernel/memory.h>
#include <spi/include/kernel/location.h>
#include <spi/include/l1p/flush.h>
#include <agents/include/comm/commagent_impl.h>

#ifdef TRACE
#undef TRACE
#endif
#define TRACE(x) //fprintf x


/**
 * \brief Internal Fence Work Request
 *
 * This is an internal view of what is placed in the request queue to describe
 * a fence work request to the agent.
 */
typedef struct CommAgent_Fence_InternalWorkRequest
{
  union /* Info */
  {
    union /* Control info overlaying the first byte of the request */
    {
      volatile char type; /**< This first byte of a work request indicates whether the work
			   *   request is ready for the agent to process.  It also indicates
			   *   the type of the work request.  
			   *   - COMM_AGENT_WORK_REQUEST_TYPE_NONE indicates the request
			   *      is NOT READY and has a value of binary zero.
			   *   - COMM_AGENT_WORK_REQUEST_TYPE_FENCE_INIT indicates the work
			   *     request is ready for the agent to process and indicates
			   *     the type of the work request.  It should be set AFTER the
			   *     entire work request has been initialized.  Note that this
			   *     overlays the first byte of the work request.
			   *   After allocating a slot in the request queue, the work request
			   *   should be copied into the slot.  The first byte of the work
			   *   request must be binary zero, such that as the work request is
			   *   being copied it will still indicate NOT_READY until the entire
			   *   work request has been copied to the slot.  Then, this "state"
			   *   should be set to one of the READY type values so the agent
			   *   knows it is ready to be processed and the type of request.
			   *   CommAgent_Fence_SubmitWorkRequest() set this state
			   *   to the READY value, and the agent sets it to NOT_READY
			   *   after extracting the work request from the work queue.
			   */
    } control;

    CommAgent_FenceInit_WorkRequest_t fence;    /**< Fence request to be processed */
  } info;

} CommAgent_Fence_InternalWorkRequest_t;


/**
 * \brief Initialize for Fence
 *
 * This function must be called after CommAgent_Init() to initialize the agent
 * for fence operations before calling any other fence functions.
 *
 * This function only needs to be called once per job by one process on the node.
 * It will not hurt anything if it is called multiple times, although the
 * subsequent calls are essentially ignored.
 *
 * \param[in]  control              The control structure initialized by
 *                                  CommAgent_Init().
 *
 * \retval  0       Successful.  The agent is initialized and ready for fence
 *                  requests.
 * \retval  errno   Indicates the reason for failure.
 */
__INLINE__
int CommAgent_Fence_Init ( CommAgent_Control_t control )
{
  volatile int rc = 0;
  uint64_t uniqueID;

  /* Pass the fence init request to the commm agent so it activates its "fence"
   * feature.
   */
  CommAgent_WorkRequest_t *commInitRequestPtr;

  do
    {
      rc = CommAgent_AllocateWorkRequest ( control,
					   &commInitRequestPtr,
					   &uniqueID );
    } while (rc==EAGAIN);

  if ( rc == 0 )
    {
      CommAgent_Fence_InternalWorkRequest_t *fenceInitRequestPtr =
	(CommAgent_Fence_InternalWorkRequest_t *)commInitRequestPtr;

      L1P_FlushRequests(); /* Make sure everything has been stored up to this point */
      
      /* Mark the request ready, and indicate its type. */
      fenceInitRequestPtr->info.control.type  = COMM_AGENT_WORK_REQUEST_TYPE_FENCE_INIT;
      
      /* The comm agent will process this init request and begin
       * processing fence packets received.
       */
    }
  return rc;
}


/**
 * \brief Get Fence Reception Fifo Dispatch Id
 *
 * This function returns the reception fifo dispatch ID used for fence operations.
 * It should be placed into software byte 0 of the fence descriptor.
 *
 * \retval  dispatchId  The reception fifo dispatch ID for fence operations.
 */
__INLINE__
uint8_t CommAgent_Fence_GetRecFifoDispatchId ()
{
  return COMM_AGENT_FENCE_DISPATCH_ID;
}


#undef TRACE

#endif
