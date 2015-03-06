
/* ------------------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                                      */
/* Blue Gene/Q 5765-PER 5765-PRP                                             */
/* © Copyright IBM Corp. 2012 All Rights Reserved                            */
/* US Government Users Restricted Rights - Use, duplication or disclosure    */
/*   restricted by GSA ADP Schedule Contract with IBM Corp.                  */
/*                                                                           */
/* This software is available to you under the Eclipse Public License (EPL). */
/* ------------------------------------------------------------------------- */

#include <assert.h>

#include "simple_query.h"

/**
 * \brief Query the task identifier of the calling process in the specified client
 *
 * \param [in] client The PAMI client
 */
pami_task_t task (pami_client_t client)
{
  pami_configuration_t configuration;
  configuration.name = PAMI_CLIENT_TASK_ID;

  pami_result_t result = PAMI_ERROR;
  result = PAMI_Client_query (client, &configuration, 1);
  assert (result == PAMI_SUCCESS);

  return configuration.value.intval;
}

/**
 * \brief Query the number of tasks in the specified client
 *
 * \param [in] client The PAMI client
 */
size_t size (pami_client_t client)
{
  pami_configuration_t configuration;
  configuration.name = PAMI_CLIENT_NUM_TASKS;

  pami_result_t result = PAMI_ERROR;
  result = PAMI_Client_query (client, &configuration, 1);
  assert (result == PAMI_SUCCESS);

  return configuration.value.intval;
}

/**
 * \brief Query the maximum number of contexts supported for the specified client
 *
 * \param [in] client The PAMI client
 *
 * \return Maximum number of contexts that can be created
 */
size_t max_contexts (pami_client_t client)
{
  pami_configuration_t configuration;
  configuration.name = PAMI_CLIENT_NUM_CONTEXTS;

  pami_result_t result = PAMI_ERROR;
  result = PAMI_Client_query (client, &configuration, 1);
  assert (result == PAMI_SUCCESS);

  return configuration.value.intval;
}



