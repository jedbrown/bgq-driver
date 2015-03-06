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
 * \file sys/examples/init_coll.h
 * \brief PAMI code examples
 */
#ifndef __examples_init_coll_h__
#define __examples_init_coll_h__

/**
 * \example init.c
 *
 * <b>A PAMI Initialization Test Case</b>
 *
 * This test case uses common utility routines for context creation
 * For the common initialization routines, see the following:
 *
 * \ref init_util.h
 * \ref init.c
 *
 * For collective initialization routines, see the following:
 *
 * \ref coll_util.h
 * \ref init_coll.c
 *
 * \dontinclude init.c
 *
 * This simple example demonstrates how to initialize PAMI
 * and how to shutdown.  This test case doesn't actually do any
 * communication, but can be used as an example of how to initialize
 * the PAMI library.
 * The first thing we do is initialize the PAMI context and the PAMI
 * client.  As with most parallel applications, the most useful bits
 * of information are "Who am I?" and "How many tasks can I communicate
 * with?".  This routine will return the task_id and the number of tasks
 * in the job.
 *
 * \par
 * \skipline Docs01
 * \until    Docs02
 *
 * Next, we print out our task_id and the total size of the job.
 * Note that every task in the job will print this information.
 *
 * \par
 * \skipline Docs03
 * \until    Docs04
 *
 * Now we have our simple "Hello World" PAMI application, and have printed
 * out rudimentary job information.  We can now shutdown the library.
 *
 * \par
 * \skipline Docs05
 * \until    Docs06
 *
 *
 * We can dig a bit deeper into what these utility routines are doing.
 * The pami_init routine listed above will initialize the clients and
 * contexts and query the PAMI library to determine task_id and job size
 *
 * This routine first creates the client, which is a collection of network
 * and hardware resources.  The client name uniquely identifies the client
 * created, and can be used as a hint to PAMI to pair resources to a resource
 * manager for the client being created.
 *
 * \dontinclude init_util.h
 * \par
 * \skipline Docs01
 * \until    Docs02
 *
 * Now we can ask the client for information like task_id and the maximum
 * number of contexts available to create.  This code queries for the max number
 * of contexts, the task id, and the total number of tasks in this client's
 * resource allocation.  The task id is the unique task number for this job
 * The task number can be used for geometry and endpoint creation.
 *
 * \par
 * \skipline Docs03
 * \until    Docs04
 *
 * Next, we create the contexts, which are subgroupings of the resources
 * available.  These subgroups of resources are optimized for multithreading
 * and concurrency.
 *
 * \par
 * \skipline Docs05
 * \until    Docs06
 *
 * The pami_shutdown routine destroys contexts and clients via the PAMI API
 *
 * \par
 * \skipline Docs07
 * \until    Docs08
 *
  *
 *
 * <b>Full example source code</b>
 *
 */

#endif /* __sys_examples_init_h__ */
