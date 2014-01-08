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

#ifndef  __COMM_AGENT__
#define  __COMM_AGENT__


/**
 * \file commagent.h
 *
 * \brief C Header File containing Comm Agent Definitions and
 *        interfaces.  These are common to all types of requests
 *        that can be submitted to the agent.
 *
 * The following header files contain further definitions for the
 * specific types of requests that can be submitted to the comm agent.
 *
 * rgetpacing.h
 * fence.h
 *
 * The comm agent is a BG/Q program that runs on one of the threads of the 17th
 * core of each node.  It is loaded automatically by CNK from
 * /bgsys/drivers/ppcfloor/agents/bin/comm.elf.  This pathname may be
 * overridden by BG_APPAGENTCOMM=<pathname>.
 *
 * There are several interfaces that are used by an application to use the
 * comm agent:
 *
 * CommAgent_Init()
 *
 * Both the comm agent and the application processes begin running in parallel.
 * The comm agent initializes its shared memory segment that is used for
 * communicating work requests from the application to the comm agent.  When 
 * the comm agent is ready to receive work requests from the application, it
 * changes a field in the shared memory from zero to its version number.
 * When the application calls CommAgent_Init(), it waits for this field
 * to change, so upon return from CommAgent_Init(), the application knows the
 * comm agent is ready to receive work requests.
 *
 * CommAgent_RemoteGetPacing_Init()
 * CommAgent_Fence_Init()
 *
 * There are different types of work requests.  Before sending work requests
 * to the comm agent, the comm agent must be initialized for each type of
 * work request.
 *
 * CommAgent_AllocateWorkRequest()
 *
 * The application calls CommAgent_AllocateWorkRequest() to allocate space
 * for a work request in the comm agent's queue located in the comm agent's
 * shared memory.  A pointer to the space is returned, along with an ID
 * that is unique to this work request.  More on this ID is discussed in
 * rgetpacing.h.
 *
 * CommAgent_RemoteGetPacing_SubmitWorkRequest()
 *
 * The application builds a work request in the space allocated by
 * CommAgent_AllocateWorkRequest() and calls the "submit" function to submit
 * it to the comm agent.
 *
 * CommAgent_GetRecFifoId()
 * CommAgent_GetVersion()
 * CommAgent_Fence_GetRecFifoDispatchId()
 *
 * These are other useful interfaces.  Refer to their definition for more
 * information.
 *
 */


#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <spi/include/mu/Util.h>
#include <agents/include/comm/commagentWorkRequests.h>


__BEGIN_DECLS


/**
 * \brief Comm Agent Control Structure
 *
 * This is a control structure initialized by CommAgent_Init() and 
 * passed into subsequent calls to CommAgent_XXXX() functions.
 * It is used internally within the CommAgent_XXXX() functions to
 * store internal information.
 */
typedef struct 
{
  void* pOpaqueObject;
} CommAgent_Control_t;


/**
 * \brief Comm Agent State
 *
 * This is used to set a field in the comm agent's shared memory.
 * It has 2 purposes:
 * 1. Change the field from zero (uninitialized) to non-zero (initialized) so
 *    the application knows when the comm agent is ready.
 * 2. A version indicator.  The value of this field indicates the version of the
 *    comm agent.  After initialization, the caller can get this version and
 *    compare it to what it is expecting.
 */
typedef enum CommAgentState
{
  COMM_AGENT_STATE_UNINITIALIZED = 0,
  COMM_AGENT_STATE_INITIALIZED_VERSION_1   = 1,
  COMM_AGENT_STATE_INITIALIZED_VERSION_2   = 2

} CommAgent_State_t;


/**
 * \brief Initialize the Comm Agent
 *
 * This function must be called to initialize the comm agent control structure
 * before calling any other comm agent functions.
 * - If multiple processes on the node are sharing the control structure
 *   (e.g. in shared memory), this only needs to be called by one of those
 *   processes.
 * - If each process on the node has its own control structure (for convenience),
 *   this should be called by each of those processes.
 *
 * \param[out] controlPtr           Pointer to the control structure.  The
 *                                  structure is initialized by this function,
 *                                  and it must be passed into subsequent calls
 *                                  to CommAgent_XXXX() functions unchanged.
 *
 * \retval  0       Successful.  The agent is initialized and ready to receive
 *                  comm requests.
 * \retval  errno   Indicates the reason for failure.
 *                  ENOENT means the Comm Agent is not running.
 */
int CommAgent_Init ( CommAgent_Control_t *controlPtr );


/**
 * \brief Allocate Storage for a Comm Agent Work Request
 *
 * Obtain a pointer to storage where a comm agent work request can be
 * built.  Upon successful return, the returned pointer points to storage
 * large enough to contain a CommAgent_WorkRequest_t.  The app
 * should fill in the storage as described in that structure's definition
 * and call the appropriate CommAgent_XXXXX_SubmitWorkRequest() function to
 * submit the request.
 *
 * This request to allocate storage may fail with EAGAIN.  This means the
 * queue is full, and the app must retry until the agent removes request(s)
 * from the queue to free up storage.
 *
 * \param[in]  control                The control structure initialized by
 *                                    CommAgent_Init().
 * \param[out]  workRequestPtrAddress  The address of a pointer where the pointer
 *                                     to the work request storage is returned.
 *                                     The pointer will be NULL if no storage is
 *                                     available.
 * \param[out]  uniqueID               Pointer to storage where an ID that is
 *                                     unique across the node of this particular
 *                                     work request is returned.  This can
 *                                     later be passed to the comm agent inside
 *                                     rgetpacing work requests to tie them 
 *                                     together as peers, ensuring that the
 *                                     work items are processed sequentially.
 *
 * \retval  0       Successful.  The request has been queued.
 * \retval  EAGAIN  The queue is full.  Try again later.
 */
__INLINE__
int CommAgent_AllocateWorkRequest (
                CommAgent_Control_t       control,
                CommAgent_WorkRequest_t **workRequestPtrAddress,
		uint64_t                 *uniqueID );


/**
 * \brief Get Comm Agent Reception Fifo Id
 *
 * This function returns the global reception FIFO Id used by the comm agent.
 * It should be called after CommAgent_Init() successfully completes.
 * The return value is undefined otherwise.
 *
 * This is useful for programming this fifo Id into descriptors targeted
 * for the comm agent's reception fifo, such as for Fence.
 *
 * \param[in]  control              The control structure initialized by
 *                                  CommAgent_Init().
 *
 * \retval  recFifoId  The reception fifo Id.
 */
__INLINE__
uint32_t CommAgent_GetRecFifoId ( CommAgent_Control_t control );


/**
 * \brief Get Comm Agent's Version Number
 *
 * This function can be called after CommAgent_Init() to get the version number of
 * the comm agent.
 *
 *
 * \param[in]  control              The control structure initialized by
 *                                  CommAgent_Init().
 *
 * \retval  version  The version number of the comm agent.
 */
__INLINE__
CommAgent_State_t CommAgent_GetVersion ( CommAgent_Control_t control );


/* Include the implementations */
#include <agents/include/comm/commagent_impl.h>


__END_DECLS


#endif
