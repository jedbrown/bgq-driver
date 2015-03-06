
/* ------------------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                                      */
/* Blue Gene/Q 5765-PER 5765-PRP                                             */
/* © Copyright IBM Corp. 2012 All Rights Reserved                            */
/* US Government Users Restricted Rights - Use, duplication or disclosure    */
/*   restricted by GSA ADP Schedule Contract with IBM Corp.                  */
/*                                                                           */
/* This software is available to you under the Eclipse Public License (EPL). */
/* ------------------------------------------------------------------------- */

#ifndef __simple_query_h__
#define __simple_query_h__

#include <pami.h>

/**
 * \brief Query the task identifier of the calling process in the specified client
 *
 * \param [in] client The PAMI client
 */
pami_task_t task (pami_client_t client);

/**
 * \brief Query the number of tasks in the specified client
 *
 * \param [in] client The PAMI client
 */
size_t size (pami_client_t client);

/**
 * \brief Query the maximum number of contexts supported for the specified client
 *
 * \param [in] client The PAMI client
 *
 * \return Maximum number of contexts that can be created
 */
size_t max_contexts (pami_client_t client);

#endif /* __simple_query_h__ */
