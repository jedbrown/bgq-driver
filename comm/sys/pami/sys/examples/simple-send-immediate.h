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
 * \file sys/examples/simple-send-immediate.h
 * \brief PAMI code examples
 */

#ifndef __examples_simple_send_immediate_h__
#define __examples_simple_send_immediate_h__

/**
 * \example simple-send-immediate.c
 *
 * <b>Send immediate point-to-point active message example</b>
 *
 * This example demonstrates how to send a small message to a destination
 * endpoint and receive a response message in return. If more than one
 * context can be created for the client, this example will perform a
 * "crosstalk" communication pattern where context 0 on task 0 sends to
 * context \b 1 on task 1. In a "non crosstalk" communication pattern all
 * contexts send only to remote endpoints corresponding to the the same
 * local context \em id.
 *
 * This example uses common utility routines for context creation
 * For the common initialization routines, see the following example for
 * more information:
 *
 * \par
 * \ref init.c
 *
 * \dontinclude simple-send-immediate.c
 *
 * Before any communcation can occur, the point-to-point dispatch functions
 * that will receive an incoming data stream must be set for each context.
 * The dispatch registration specifies an identifier that is used for
 * point-to-point operations.
 *
 * This example uses the dispatch id \b 10 to identify both the dispatch
 * function that will receive the data stream and the local context
 * resources that will initiate the transfer. For example simplification,
 * the dispatch function registered context is the same, \c test_dispatch(),
 * which is merely a decrementer with simple data validation.
 *
 * \par
 * \skipline [example dispatch function]
 * \until    [example dispatch function]
 *
 * The dispatch function cookie is a pointer to the receive counter
 * specific to each context that was created. This means that \c recv_active[n]
 * will only be decremented when \c context[n] is advanced.
 *
 * \par
 * \skipline [example dispatch set for all contexts]
 * \until    [example dispatch set for all contexts]
 *
 * The example can begin communication after the dispatch functions have
 * been set for all contexts. At this point the example flow diverges based
 * on the task identifier.
 *
 * Task 0 will send a 'ping' message to task 1, and
 * then advance the context until task 1 sends a 'pong' acknowledgement
 * message
 *
 * \par
 * \skipline [example 'ping' from task 0]
 * \until    [example 'ping' from task 0]
 *
 * Task 1 will advance the context until it receives the 'ping' message
 * from task 0, and then send a 'pong' acknowledgement message to task 0
 *
 * \par
 * \skipline [example 'pong' from task 1]
 * \until    [example 'pong' from task 1]
 *
 * After task 0 and task 1 complete the ping-pong operation all processes
 * will destroy the client and context(s) that were created. Again, see the
 * following example for more information:
 *
 * \par
 * \ref init.c
 *
 * The send immediate point-to-point active message example is now complete.
 *
 * <b>Full example source code</b>
 *
 */

#endif /* __sys_examples_simple_send_immediate_h__ */
