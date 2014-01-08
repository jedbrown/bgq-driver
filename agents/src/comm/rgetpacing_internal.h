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

#ifndef  __COMM_AGENT_RGET_PACING_INTERNAL__
#define  __COMM_AGENT_RGET_PACING_INTERNAL__


/**
 * \file rgetpacing_internal.h
 *
 * \brief C File containing Comm Agent Remote Get Pacing Internal Declarations
 */


#include <agents/include/comm/rgetpacing.h>


__BEGIN_DECLS


/**
 * \brief Maximum Number of Bytes Allowed In The Network
 */
#define COMM_AGENT_REMOTE_GET_PACING_MAX_BYTES_IN_NETWORK (32768)


/**
 * \brief Sub-message Size
 */
#define COMM_AGENT_REMOTE_GET_PACING_SUB_MESSAGE_SIZE (8192)


/**
 * \brief Message Size for Pacing
 *
 * This is the default message size threshold for a request to be paced.
 * Message sizes larger than this value will be paced.
 */
#define COMM_AGENT_REMOTE_GET_PACING_MESSAGE_SIZE (8192)


/**
 * \brief Number of Work Items in a Block of Heap
 */
#define COMM_AGENT_REMOTE_GET_PACING_NUM_WORK_ITEMS_IN_HEAP_BLOCK (16)


/**
 * \brief Number of Counters For Remote Gets
 *
 * Includes both paced and non-paced remote gets.
 */
#define COMM_AGENT_REMOTE_GET_PACING_NUM_COUNTERS (256)


/**
 * \brief Process a Remote Get Pacing Work Request
 *
 * This function copies the work request from the request queue into a
 * work item and queues it onto the remote get pacing work item list.
 *
 * \param[in]  workRequestPtr  Pointer to the work request in the request queue.
 */
void processRgetpacingWorkRequest( 
       CommAgent_RemoteGetPacing_InternalWorkRequest_t *workRequestPtr );


/**
 * \brief Process a Remote Get Pacing Init Work Request
 *
 * This function initializes the comm agent for handling remote get
 * work requests.
 *
 * \param[in]  workRequestPtr  Pointer to the work request in the request queue.
 */
void processRgetpacingInitWorkRequest( 
       CommAgent_RemoteGetPacingInit_WorkRequest_t *workRequestPtr );


/**
 * \brief Perform Remote Get Pacing Work
 *
 * This function makes a pass through the remote get pacing work items,
 * performing whatever work can be done.
 */
void doRemoteGetPacingWork();


__END_DECLS


#endif
