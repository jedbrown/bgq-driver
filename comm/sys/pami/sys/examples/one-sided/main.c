
/* ------------------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                                      */
/* Blue Gene/Q 5765-PER 5765-PRP                                             */
/* © Copyright IBM Corp. 2012 All Rights Reserved                            */
/* US Government Users Restricted Rights - Use, duplication or disclosure    */
/*   restricted by GSA ADP Schedule Contract with IBM Corp.                  */
/*                                                                           */
/* This software is available to you under the Eclipse Public License (EPL). */
/* ------------------------------------------------------------------------- */

#include <stdio.h>
#include <assert.h>

#include <pami.h>

#include "simple_query.h"

#include "test.h"




int main (int argc, char * argv[])
{
  pami_result_t result;
  pami_client_t client;
  pami_context_t context[2];

  /* Create the PAMI client */
  result = PAMI_ERROR;
  result = PAMI_Client_create ("ClientName", & client, NULL, 0);
  assert (result == PAMI_SUCCESS);


  /*
   * Determine the number of tasks in the client for the current job. This test
   * requires > 1 tasks
   */
  if (task (client) == 0)
    fprintf (stdout, "(%03d) Number of tasks in the job: %d\n", __LINE__, size (client));
  if (size (client) < 2)
  {
    if (task (client) == 0)
      fprintf (stdout, "(%03d) This test requires > 1 task\n", __LINE__);
    return 1;
  } 

  /*
   * Determine the maximum number of contexts supported by this client for the
   * current job configuration.
   */
  if (task (client) == 0)
    fprintf (stdout, "(%03d) Maximum number of contexts allowed: %d\n", __LINE__, max_contexts (client));
  if (max_contexts (client) < 2)
  {
    if (task (client) == 0)
      fprintf (stdout, "(%03d) This test requires > 1 context; On Blue Gene/Q, set the environment variable 'PAMI_MU_RESOURCES=Minimal' to increase the number of supported contexts for this job configuration.\n", __LINE__);
    return 1;
  } 
 
  /*
   * Create the PAMI communication contexts. The first context will be used
   * directly for better latency; the second context will have async progress
   * enabled.
   */
  result = PAMI_ERROR;
  result = PAMI_Context_createv (client, NULL, 0, context, 2);
  assert (result == PAMI_SUCCESS);


  fprintf (stdout, "(%03d) before test_fn()\n", __LINE__);
  test_fn (argc, argv, client, context);
  fprintf (stdout, "(%03d) after test_fn()\n", __LINE__);

  /* Destroy the context */
  result = PAMI_ERROR;
  result = PAMI_Context_destroyv (context, 2);
  assert (result == PAMI_SUCCESS);

  /* Destroy the client */
  result = PAMI_ERROR;
  PAMI_Client_destroy (client);
fprintf (stderr, "FIXME: status is not set on success! result = %d\n", result);
//  assert (result == PAMI_SUCCESS);

  return 0;
}




