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

#ifndef  __COMM_AGENT_WORK_REQUESTS__
#define  __COMM_AGENT_WORK_REQUESTS__


/**
 * \file commagentWorkRequests.h
 *
 * \brief C Header File containing Comm Agent Work Request Definitions.
 *
 */


#include <stdint.h>
#include <stdlib.h>
#include <hwi/include/bqc/MU_Descriptor.h>


__BEGIN_DECLS


/**
 * \brief Work Request Types
 *
 * These are the types of work requests that can be submitted to the
 * comm agent.  The first type (NONE) must be zero.
 * New types should be added immediately before 
 * COMM_AGENT_NUMBER_OF_WORK_REQUEST_TYPES so the work request type
 * values don't change.
 * COMM_AGENT_NUMBER_OF_WORK_REQUEST_TYPES should always be last
 * because it counts the number of types.
 */
typedef enum CommAgent_WorkRequestType
{
  COMM_AGENT_WORK_REQUEST_TYPE_NONE = 0,
  COMM_AGENT_WORK_REQUEST_TYPE_RGET_INIT,
  COMM_AGENT_WORK_REQUEST_TYPE_RGET_REQUEST,
  COMM_AGENT_WORK_REQUEST_TYPE_FENCE_INIT,
  COMM_AGENT_NUMBER_OF_WORK_REQUEST_TYPES

} CommAgent_WorkRequestType_t;


/**
 * \brief Remote Get Pacing Init Work Request
 *
 * This is a work request for the comm agent of type
 * COMM_AGENT_WORK_REQUEST_TYPE_RGET_INIT.
 * 
 * This is an internal request submitted to the agent within
 * CommAgent_RemoteGetPacing_Init().
 *
 * The request initializes the remote get pacing feature of the comm agent.
 * It tells the agent how to access to the app's shared memory.  This is used
 * when the app needs completion notification.
 */
typedef struct CommAgent_RemoteGetPacingInit_WorkRequest
{
  char     dummy;                 /**< Initialize to zero.  Not used otherwise.
				   */
  char     unused[7];             /**< For alignment. */

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
  char      sharedMemoryName[128]; /**< String containing the pathname of the 
				        app's shared memory file.
				   */
} __attribute__((__packed__)) CommAgent_RemoteGetPacingInit_WorkRequest_t;


/**
 * \brief Remote Get Pacing Work Request
 *
 * This is a work request for the comm agent of type
 * COMM_AGENT_WORK_REQUEST_TYPE_RGET_REQUEST.  The request contains
 * a remote get descriptor and one or two payload descriptors.  The payload
 * descriptors can be either direct put or memory fifo transfers.
 *
 * - The rgetDescriptor is required.
 * - The payloadDescriptor1 is required.
 * - The payloadDescriptor2 is optional.
 *
 * The Message_Length in the rgetDescriptor must be 64 (decimal) if there is no
 * payloadDescriptor2.  The Message_Length must be 128 (decimal) if 
 * payloadDescriptor2 is present.
 *
 * When completion notification is desired for a direct put payloadDescriptor,
 * the Counter_Offset in the payloadDescriptor must be the offset of a completion
 * counter in the app's shared memory.  The Counter_Offset is relative to the
 * starting location of the counter region specified during
 * RemoteGetPacing_Init().  This counter will be decremented by the direct
 * put's message length upon completion of the direct put transfer.
 * A Counter_Offset of 0xFFFFFFFFFFFFFFFF means that no completion notification
 * is desired for the direct put.
 *
 * Note that the Reception Counter Base Address Table Id in the direct put
 * payload descriptor is ignored when using the remote get pacing agent.
 */
typedef struct CommAgent_RemoteGetPacing_WorkRequest
{
  char     dummy;          /**< Initialize to zero.  Not used otherwise.         */
  char     unused1[7];     /**< For alignment.                                   */

  uint16_t globalInjFifo;  /**< Global inj fifo ID where this rget
			        would have been injected.                        */

  char     unused2[14];    /**< For alignment.                                   */

  uint64_t peerID;         /**< Optional unique ID that can be specified in
			        multiple rgetpacing work requests to tie them
				together as peers such that they are processed 
				sequentially.  The "uniqueID" returned from 
			        CommAgent_AllocateWorkRequest) may be used for 
				this.
				Zeros means no ID is specified, so this work
			        request is not tied to any others.               */

  MUHWI_Descriptor_t rgetDescriptor;    /**< Remote Get request to be processed  */
  MUHWI_Descriptor_t payloadDescriptor; /**< Remote Get payload descriptor       */

} __attribute__((__packed__)) CommAgent_RemoteGetPacing_WorkRequest_t;


/**
 * \brief Fence Init Work Request
 *
 * This is a work request for the comm agent of type
 * COMM_AGENT_WORK_REQUEST_TYPE_FENCE_INIT.
 * 
 * This is an internal request submitted to the agent within
 * CommAgent_Fence_Init().
 *
 * The request contains no information.  It tells the comm agent to enable
 * fence requests to be processed.
 */
typedef struct CommAgent_FenceInit_WorkRequest
{
  char dummy; /**< Initialize to zero.  Not used otherwise. */

} __attribute__((__packed__)) CommAgent_FenceInit_WorkRequest_t;


/**
 * \brief Comm Agent Work Request
 *
 * This is a work request for the comm agent.
 */
typedef struct CommAgent_WorkRequest
{
  union
  {
    CommAgent_RemoteGetPacingInit_WorkRequest_t rgetInit;
    CommAgent_RemoteGetPacing_WorkRequest_t     rget;
    CommAgent_FenceInit_WorkRequest_t           fenceInit;
  } request;

} __attribute__((__packed__, __aligned__(32))) CommAgent_WorkRequest_t;


__END_DECLS


#endif
