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

#ifndef  __COMM_AGENT_RGET_PACING_IMPL__
#define  __COMM_AGENT_RGET_PACING_IMPL__


/**
 * \file rgetpacing_impl.h
 *
 * \brief C Header File containing Remote Get Pacing Inline Implementations
 *        This header is included by rgetpacing.h.
 */


#include <string.h>
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


#ifdef TRACE
#undef TRACE
#endif
#define TRACE(x) //fprintf x


/**
 * \brief Internal Remote Get Pacing Work Request
 *
 * This is an internal view of what is placed in the request queue to describe
 * a remote get pacing work request to the agent.
 */
typedef struct CommAgent_RemoteGetPacing_InternalWorkRequest
{
  union /* Info */
  {
    union /* Control info overlaying the first 4 bytes of the request */
    {
      volatile char type; /**< This first byte of a work request indicates whether the work
			   *   request is ready for the agent to process.  It also indicates
			   *   the type of the work request.  
			   *   - COMM_AGENT_WORK_REQUEST_TYPE_NONE indicates the request
			   *      is NOT READY and has a value of binary zero.
			   *   - COMM_AGENT_WORK_REQUEST_TYPE_RGET or
			   *     COMM_AGENT_WORK_REQUEST_TYPE_RGET_COMPLETION_NOTIFICATION_SETUP
			   *     indicates the work request is ready for the agent to
			   *     process and indicates the type of the work request.  It
			   *     should be set AFTER the entire work request has been
			   *     initialized.  Note that this overlays the first
			   *     byte of the work request.  
			   *   After allocating a slot in the request queue, the work request
			   *   should be copied into the slot.  The first byte of the work
			   *   request must be binary zero, such that as the work request is
			   *   being copied it will still indicate NOT_READY until the entire
			   *   work request has been copied to the slot.  Then, this "state"
			   *   should be set to one of the READY type values so the agent
			   *   knows it is ready to be processed and the type of request.
			   *   CommAgent_RemoteGetPacing_Init() and 
			   *   CommAgent_RemoteGetPacing_SubmitWorkRequest() set this state
			   *   to one of the READY values, and the agent sets it to NOT_READY
			   *   after extracting the work request from the work queue.
			   */
      int handle; /**< The handle initialized by RemoteGetPacing_Init() and
		   *   passed into RemoteGetPacing_SubmitWorkRequest().
		   */
    } control;

    union
    {
      CommAgent_RemoteGetPacing_WorkRequest_t rget;    /**< Request 1: Remote Get request 
							    to be processed 
							*/
      CommAgent_RemoteGetPacingInit_WorkRequest_t rgetInit; /**< Request 2:  Remote Get
							         init request to be
								 processed
							    */
    } request;

  } info;

} CommAgent_RemoteGetPacing_InternalWorkRequest_t;


/**
 * \brief Initialize for Remote Get Pacing
 *
 * This function must be called after CommAgent_Init() to initialize the remote
 * get pacing data structures before calling any other remote get pacing functions.
 *
 * If a sharedMemoryInfoPtr is provided, the value of the handle must be initialized
 * to zero by the app prior to calling  RemoteGetPacing_Init().  Then, after
 * calling RemoteGetPacing_Init(), the app should poll the handle value until it
 * becomes non-zero to ensure the handle has been initialized before using it.
 *
 * - If multiple processes on the node are sharing the completion counters,
 *   this only needs to be called by one of those processes.
 * - If each process on the node has its own set of completion counters, 
 *   this should be called by each of those processes.
 *
 * \param[in]  control              The control structure initialized by
 *                                  CommAgent_Init().
 * \param[in]  sharedMemoryInfoPtr  Pointer to a the shared memory info structure.
 *                                  Specifying NULL indicates that no completion
 *                                  notification will be performed by the agent
 *                                  on any remote gets submitted for remote get
 *                                  pacing.  When completion notification is
 *                                  desired on any remote gets, this structure
 *                                  must be specified here, and the structure's
 *                                  fields must be filled in as described in the
 *                                  structure's definition.
 *
 * \retval  0       Successful.  The agent is initialized and ready to receive
 *                  pacing requests.
 * \retval  errno   Indicates the reason for failure.
 * \retval  EAGAIN  The queue is full.  Try again later.
 */
__INLINE__
int CommAgent_RemoteGetPacing_Init ( 
		  CommAgent_Control_t                           control,
		  CommAgent_RemoteGetPacing_SharedMemoryInfo_t *sharedMemoryInfoPtr )
{
  volatile int rc = 0;
  uint64_t uniqueID;

  CommAgent_WorkRequest_t *commInitRequestPtr;

  do
    {
      rc = CommAgent_AllocateWorkRequest ( control,
					   &commInitRequestPtr,
					   &uniqueID );
    } while (rc==EAGAIN);

  if ( rc == 0 )
    {
      memset( commInitRequestPtr, 0x00, sizeof(CommAgent_WorkRequest_t) );

      CommAgent_RemoteGetPacing_InternalWorkRequest_t *rgetInitRequestPtr =
	(CommAgent_RemoteGetPacing_InternalWorkRequest_t *)commInitRequestPtr;

      if ( sharedMemoryInfoPtr )
	{
	  rgetInitRequestPtr->info.request.rgetInit.handleOffset = 
	    sharedMemoryInfoPtr->handleOffset;
	  rgetInitRequestPtr->info.request.rgetInit.counterRegionOffset =
	    sharedMemoryInfoPtr->counterRegionOffset;
	  rgetInitRequestPtr->info.request.rgetInit.counterRegionSize =
	    sharedMemoryInfoPtr->counterRegionSize;
	  strcpy( rgetInitRequestPtr->info.request.rgetInit.sharedMemoryName,
		  sharedMemoryInfoPtr->sharedMemoryNamePtr );
	}
      
      L1P_FlushRequests(); /* Make sure everything has been stored up to this point */
      
      /* Mark the request ready, and indicate its type. */
      rgetInitRequestPtr->info.control.type  = COMM_AGENT_WORK_REQUEST_TYPE_RGET_INIT;
      
      /* The comm agent will process this init request.  When done, it will set
       * the handle at the handleOffset to non-zero indicating it is ready
       * for rget requests to be submitted.
       */
    }
  return rc;
}


/**
 * \brief Submit A Remote Get Pacing Work Request
 *
 * The request queue slot has been primed by the app with the request.
 * The first byte of this request is special.
 * Initially, when the work request is being built into the request queue, 
 * this value is zero (COMM_AGENT_WORK_REQUEST_TYPE_NONE).
 * This submit function changes the first byte to one of the other values,
 * indicating
 * 1. The request is ready to be seen by the agent.
 * 2. The type of request being submitted.
 * When the agent sees this byte change from zero to non-zero, it begins
 * to process the request.
 *
 * \param[in]  control              The control structure initialized by
 *                                  RemoteGetPacing_Init().
 * \param[in]  handle               The handle previously initialized by
 *                                  RemoteGetPacing_Init().  If completion
 *                                  notification is not needed for the
 *                                  direct put(s) associated with this request,
 *                                  then this value is not used.
 * \param[in]  workRequestPtr       Pointer to the work request to be submitted.
 *                                  This must be a pointer returned from 
 *                                  RemoteGetPacing_AllocateWorkRequest().
 *
 * \retval  0  Successful.  The request has been queued.
 */
__INLINE__
int CommAgent_RemoteGetPacing_SubmitWorkRequest ( 
		CommAgent_Control_t                      control,
		int                                      handle,
		CommAgent_RemoteGetPacing_WorkRequest_t *workRequestPtr )
{
  CommAgent_RemoteGetPacing_InternalWorkRequest_t *internalWorkRequestPtr = 
    (CommAgent_RemoteGetPacing_InternalWorkRequest_t*)workRequestPtr;

  internalWorkRequestPtr->info.control.handle = handle;

  L1P_FlushRequests(); /* Make sure everything has been stored up to this point */

  /* Mark the request ready, and indicate its type. */
  internalWorkRequestPtr->info.control.type  = COMM_AGENT_WORK_REQUEST_TYPE_RGET_REQUEST;

  return 0;
}

#undef TRACE

#endif
