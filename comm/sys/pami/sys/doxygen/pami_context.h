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
 * \file sys/doxygen/pami_context.h
 * \brief Detailed doxygen documentation for pami multi-context interface
 */
#ifndef __doxygen_pami_context_h__
#define __doxygen_pami_context_h__



/**
 * \addtogroup contexts_and_endpoints
 *
 * Contexts are parallel "threading points" that an application may use to
 * optimize concurrent communications.
 *
 * Contexts have the following features:
 * - Contexts are a local resource for, and created by, each task
 * - Every context within a client has equivalent connectivity, functionality,
 *   and semantics
 * - Communication initiated by the local task will use a \ref context
 *   "context object" to identify the specific threading point that will
 *   complete the operation
 * - Communication that addresses a destination task will use an \ref endpoint
 *   "endpoint object" to identify the destination thread point that will
 *   receive the dispatch
 * <b>Endpoint usage</b>
 *
 * All communication functions require a context to perform the
 * operation on the dedicated hardware resource(s) and an endpoint
 * to address the destination. The array of context opaque objects is initialized
 * when the contexts are created. To obtain an endpoint opaque
 * object the application must invoke one of the two endpoint functions.
 *
 * - The application must invoke PAMI_Endpoint_create() to obtain an endpoint to
 *   address a specific context on a destination task.
 *
 * <b>Dispatch</b>
 *
 * A dispatch is specific to a context and contexts are specific
 * to a client. The dispatch function does not need the client as an input
 * parameter because the local endpoint opaque object may contain back-pointers
 * to client information.
 *
 * Contexts and endpoints are both opaque object types and not integers.
 * Applications may prefer to use a unique, monotonically increasing integer to
 * identify a context associated with an endpoint for the client in a registered
 * dispatch function. This can be
 * accomplished by storing the application-defined endpoint identifier in the
 * dispatch function cookie for each dispatch registration.
 */

#endif /* __doxygen_pami_context_h__ */
