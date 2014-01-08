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

#ifndef  __COMM_AGENT_FENCE__
#define  __COMM_AGENT_FENCE__


/**
 * \file fence.h
 *
 * \brief C Header File containing Fence Definitions and
 *        interfaces.
 *
 * The "fence" function provided by the comm agent receives a packet
 * into the comm agent's reception fifo, extracts a Message Unit
 * descriptor from the packet's payload, and injects that descriptor.
 *
 * This is useful for implementing a fence operation where node A
 * sends messages to processes on node B and wants to know when those
 * messages have arrived at node B.  Node A does a "fence" where it
 * sends this "ping" packet to the comm agent on node B.  The comm agent
 * sends the "pong" back, and when that is received, node A knows that
 * the previous messages have been received.
 *
 * Note that the "ping" packet must be a memory-fifo packet.
 *
 * CommAgent_Fence_GetRecFifoDispatchId ()
 *
 * This returns the comm agent's dispatch ID that must be placed
 * into the "ping" packet header, software byte 0.
 *
 * CommAgent_GetRecFifoId()
 *
 * This returns the comm agent's global reception fifo ID, that
 * must be placed into the "ping" packet header, Rec Fifo ID.
 */


#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <spi/include/mu/Util.h>
#include <spi/include/mu/Descriptor.h>
#include <agents/include/comm/commagent.h>


__BEGIN_DECLS


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
int CommAgent_Fence_Init ( CommAgent_Control_t control );


/**
 * \brief Get Fence Reception Fifo Dispatch Id
 *
 * This function returns the reception fifo dispatch ID used for fence operations.
 * It should be placed into software byte 0 of the fence descriptor.
 *
 * \retval  dispatchId  The reception fifo dispatch ID for fence operations.
 */
uint8_t CommAgent_Fence_GetRecFifoDispatchId ();


/* Include the implementations */
#include <agents/include/comm/fence_impl.h>


__END_DECLS


#endif
