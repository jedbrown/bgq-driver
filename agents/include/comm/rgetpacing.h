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

#ifndef  __COMM_AGENT_RGET_PACING__
#define  __COMM_AGENT_RGET_PACING__


/**
 * \file rgetpacing.h
 *
 * \brief C Header File containing Remote Get Pacing Definitions and
 *        interfaces.
 *
 * The remote get pacing function of the comm agent is driven by work requests
 * sent to the comm agent from the application processes on the node.
 * The comm agent must first be initialized using CommAgent_Init() and
 * CommAgent_RemoteGetPacing_Init().
 * 
 * Then, for each remote get request, the application calls 
 * CommAgent_AllocateWorkRequest() to allocate space for the work request,
 * fills in the remote get work request, and then calls 
 * CommAgent_RemoteGetPacing_SubmitWorkRequest().
 *
 * Completion of the remote get request is done by either:
 * 1. Submitting a second remote get request to the comm agent that 
 *    contains a memory fifo completion descriptor that will arrive
 *    back on the calling node.
 * 2. Identifying to the comm agent the location of a Message Unit
 *    counter that is set to zero upon completion.
 *    Note: This method is not implemented at this time.
 *
 * For completion type 1 (the memory fifo completion descriptor), the two
 * requests (the remote get data request, and the remote get completion
 * request) must be associated with one another.  This association is
 * accomplished by using the unique ID returned from
 * CommAgent_AllocateWorkRequest().  This ID returned from the first call
 * is set into the "peerID" field of both work requests.  This ensures
 * that the second remote get is not processed before the first is
 * complete.
 *
 * Remote get requests are ordered by the comm agent as they would be
 * if they were submitted by the application processes themselves.
 * The ordering is determined by three things:
 * 1.  Global injection fifo number where this rget would have been injected by
 *     the caller.
 * 2.  Global rget fifo number on the destination node where the payload will be
 *     injected.  The destination node has a set of rget fifos.  We achieve the
 *     most parallelization by making each of these fifos have a unique ID so
 *     that rget requests targeted for different fifos are processed in parallel.
 * 3.  Destination node's coordinates.
 * All of this is extracted from the work request.
 *
 * Refer to commagentWorkRequests.h for details about the contents of
 * the remote get pacing work request. 
 *
 * Deciding When To Pace
 *
 * The application decides whether messages to a given destination are eligible
 * for pacing or not.  If the application decides "yes", then all remote gets
 * sent to that destination must be sent to the comm agent.  The comm agent
 * uses environment variable COMMAGENT_RGETPACINGSIZE to determine whether the
 * message is large enough to pace.  The comm agent will always process each
 * remote get request whether it is paced or not.  If a remote get is not paced,
 * the agent sends only one rget for the message.  If the remote get is paced,
 * the agent breaks the rget up into sub-messages.  Either way, the comm agent
 * maintains proper ordering of the messages, which is why all remote gets
 * must be sent to the comm agent for a given destination.
 *
 */


/**
 * \addtogroup env_vars_ Environment Variables
 *
 * \internal
 * Environment variable documentation uses the custom doxygen tags "\envs",
 * "\env", and "\default". These tags are defined as the following ALIASES in
 * the Doxyfile:
 *
 * \code
 *   ALIASES  = default="\n \par Default:\n"
 *   ALIASES += envs{3}="\addtogroup env_vars_\2 \3 \n \ingroup env_vars_\1"
 *   ALIASES += env{2}="\addtogroup env_vars_\1 \n \anchor \2 \section \2 "
 * \endcode
 * 
 * \endinternal
 * 
 * \envs{,commagent,Communication Agent}
 * The following environment variables control rget pacing. The size of the
 * injection fifo and the reception fifo used by the communication agent can be
 * configured using the \ref MUSPI_INJFIFOSIZE and \ref MUSPI_RECFIFOSIZE
 * environment variables.
 *
 * \env{commagent,COMMAGENT_REQUESTQUEUESIZE}
 * The size of the request queue.
 * This is the number of requests that can be in the queue before it fills.
 * \default 64 requests
 *
 * \env{commagent,COMMAGENT_RGETPACINGSUBSIZE}
 * The size, in bytes, of a sub-message used
 * for remote get pacing.  The pacing logic breaks a large remote get into
 * sub-messages of this size.
 * \default Depends on the number of nodes in the block as follows:
 *
 * \verbatim
 * BLOCK SIZE (RACKS)   COMMAGENT_RGETPACINGSUBSIZE
 * ------------------   ---------------------------
 *        Racks <  2                16384
 *   2 <= Racks <  4                16384
 *   4 <= Racks <  8                 8192
 *   8 <= Racks < 16                 8192
 *  16 <= Racks < 32                 8192
 *  32 <= Racks < 48                 8192
 *  48 <= Racks < 64                 8192
 *  64 <= Racks < 80                 8192
 *  80 <= Racks < 96                 8192
 *  96 <= Racks                      8192
 * \endverbatim
 *
 * \env{commagent,COMMAGENT_RGETPACINGMAX}
 * The maximum number of bytes allowed into be in
 * the network at once as a result of paced remote gets from each node.
 * Must be a multiple of \ref COMMAGENT_RGETPACINGSUBSIZE.
 * \default depends on the number of nodes in the block as follows:
 *
 * \verbatim
 * BLOCK SIZE (RACKS)   COMMAGENT_RGETPACINGMAX
 * ------------------   -----------------------
 *        Racks <  2            65536
 *   2 <= Racks <  4            65536
 *   4 <= Racks <  8            32768
 *   8 <= Racks < 16            32768
 *  16 <= Racks < 32            32768
 *  32 <= Racks < 48            32768
 *  48 <= Racks < 64            32768
 *  64 <= Racks < 80            32768
 *  80 <= Racks < 96            32768
 *  96 <= Racks                 32768
 * \endverbatim
 *
 * \env{commagent,COMMAGENT_NUMRGETS}
 * The maximum number of remote gets that can be in
 * the network from this node, paced or not paced.
 * \default 256
 *
 * \env{commagent,COMMAGENT_RGETPACING}
 * Specifies whether or not to pace the remote gets
 * sent to the comm agent.
 * - 0 - Do not pace any remote gets.
 * - 1 - Pace all remote gets sent to the comm agent that meet the pacing
 *       criteria.
 * \default 1
 *
 * \env{commagent,COMMAGENT_WAKEUP}
 * Specifies whether or not the comm agent should use
 * the wakeup unit when waiting for work.
 * - 0 - Do not use the wakeup unit.  Poll continuously.
 * - 1 - Use the wakeup unit.  It will awaken when there is work to do.
 * \default 1
 */


#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <spi/include/mu/Util.h>
#include <spi/include/mu/Descriptor.h>
#include <agents/include/comm/commagent.h>


__BEGIN_DECLS


/**
 * \brief Remote Get Pacing Shared Memory Info Structure
 *
 * This structure is only used when the app wants to be notified of the
 * completion of at least some of the direct puts to be submitted to the agent.
 * The notification is provided via "completion counter".  The counter is 
 * decremented by the length of the direct put message.
 *
 * During RemoteGetPacing_Init(), the app uses this structure to inform
 * the agent of the location of a pool of counters to be used for completion
 * notification.  Each direct put requiring completion notification submitted
 * to the agent specifies one of these counters.
 *
 * The counter pool must be in the app's shared memory so the agent can 
 * access the counter.  If the app is using the wakeup unit to wake up when
 * one of these counters changes or hits zero, the counter pool must also be
 * in the app's wake-address-compare region.
 *
 * This info structure provides the necessary information for the agent to
 * perform this notification function.  The structure contains a pointer
 * to the name of the app's shared memory file.  This is needed because the
 * agent must shm_open() this file to access the counters.  In addition, the
 * offset into the app's shared memory region of the start of the pool of 
 * counters and the pool's size is needed.
 *
 * Finally, during RemoteGetPacing_Init(), the agent sets all of this up
 * and passes a handle back to the app.  The handle is located in the app's
 * shared memory.  The app provides the offset of that handle into the
 * app's shared memory in this info structure.  This handle must accompany all 
 * requests submitted to the agent whenever completion notification is desired.
 */
typedef struct CommAgent_RemoteGetPacing_SharedMemoryInfo
{
  char     *sharedMemoryNamePtr; /**< Name of shared memory file.  This is the
				      app's shared memory where completion
				      counters are stored.  The agent supports
				      a 160 character name (including NULL 
				      terminator) or less.
				 */
  uint64_t  handleOffset;        /**< Offset into the app's shared memory
				      of an "int" where a handle is stored.
				      This handle is initialized by the
				      agent during RemoteGetPacing_Init().
                                      This handle is also passed into the
                                      interface that submits a remote get
                                      request to the agent.  See
				      RemoteGetPacing_SubmitWorkRequest().
				      The value of the handle must be initialized
				      to zero by the app prior to calling
				      RemoteGetPacing_Init().  Then, after
				      calling RemoteGetPacing_Init(), the app
				      should poll the handle value until it
				      becomes non-zero to ensure the handle has
				      been initialized before using it.
				 */
  uint64_t  counterRegionOffset; /**< Starting offset into the app's shared
				      memory where the completion counters are
				      located.                                   
				 */
  size_t    counterRegionSize;   /**< The size, in bytes, of the completion
				      counter region.                           
				 */

} CommAgent_RemoteGetPacing_SharedMemoryInfo_t;


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
int CommAgent_RemoteGetPacing_Init ( 
		  CommAgent_Control_t                           control,
		  CommAgent_RemoteGetPacing_SharedMemoryInfo_t *sharedMemoryInfoPtr );


/**
 * \brief Submit A Remote Get Pacing Work Request
 *
 * \param[in]  control              The control structure initialized by
 *                                  CommAgent_Init().
 * \param[in]  handle               The handle previously initialized by
 *                                  RemoteGetPacing_Init().  If completion
 *                                  notification is not needed for the
 *                                  direct put(s) associated with this request,
 *                                  then this value is not used.
 * \param[in]  workRequestPtr       Pointer to the work request to be submitted.
 *                                  This must be a pointer returned from 
 *                                  CommAgent_AllocateWorkRequest().
 *
 * \retval  0  Successful.  The request has been queued.
 */
int CommAgent_RemoteGetPacing_SubmitWorkRequest ( 
		CommAgent_Control_t                      control,
		int                                      handle,
		CommAgent_RemoteGetPacing_WorkRequest_t *workRequestPtr );


/* Include the implementations */
#include <agents/include/comm/rgetpacing_impl.h>


__END_DECLS


#endif
