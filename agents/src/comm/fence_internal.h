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

#ifndef  __COMM_AGENT_FENCE_INTERNAL__
#define  __COMM_AGENT_FENCE_INTERNAL__


/**
 * \file fence_internal.h
 *
 * \brief C File containing Comm Agent Fence Internal Declarations
 */


__BEGIN_DECLS


/**
 * \brief Process a Fence Init Work Request
 *
 * This function initializes the comm agent for handling fence
 * work requests.
 *
 * \param[in]  workRequestPtr  Pointer to the work request in the request queue.
 */
void processFenceInitWorkRequest( 
       CommAgent_FenceInit_WorkRequest_t *workRequestPtr );


__END_DECLS


#endif
