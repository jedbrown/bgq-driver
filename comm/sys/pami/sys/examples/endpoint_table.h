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
 * \file sys/examples/endpoint_table.h
 * \brief PAMI code examples
 */

#ifndef __examples_endpoint_table_h__
#define __examples_endpoint_table_h__

/**
 * \example endpoint_table.c
 *
 * <b>Alternative endpoint table addressing example</b>
 *
 * This example demonstrates how to construct an endpoint table containing
 * all endpoints in the system. The endpoint table can then be used, in
 * concert with a wrapper function, as a way to address all endpoints as
 * monotonically increasing integers instead of directly using the
 * pami_endpoint_t opaque type.
 *
 * \dontinclude endpoint_table.c
 *
 * The endpoint table must be initialized for each task after the client
 * and contexts are created.
 *
 * In this example the global endpoint table, \c _endpoint, is initialized
 * in the \b createEndpointTable() function. The endpoint table is allocated from
 * memory to contain a pami_endpoint_t element for each endpoint in the system.
 *
 * \note Tasks that are located on the same node and have access to the
 *       same shared memory segment may place this table in shared memory
 *       to reduce the amount of memory consumed by each process.
 *
 * In this example each task in the client creates an identical number of
 * contexts (in this case 4). This allows the endpoint table to be created
 * without communication with any other endpoints.
 *
 * The PAMI_Endpoint_create() function is used to initialize the endpoint
 * table in a loop over all tasks in the system.
 *
 * \par
 * \skip _endpoint;
 * \until };
 *
 * After the endpoint table has been initialized the task may send to any
 * endpoint in the system by specifying the \em index of the destination
 * endpoint in the endpoint table to a wrapper send function.
 *
 * In this example the wrapper send function simply accesses the global
 * pointer to the endpoint table in order to determine the actual destination
 * endpoint for the send operation.
 *
 * \par
 * \skip send_endpoint
 * \until };
 *
 * The client application software may now send to an endpoint using an
 * integer identifier instead of the pami_endpoint_t opaque type identifier.
 * All other PAMI configuration parameters remain standard.
 *
 * \par
 * \skip _lock
 * \until _unlock
 *
 * <b>Full example source code</b>
 *
 */

#endif /* __sys_examples_pami_endpoint_table_h__ */
