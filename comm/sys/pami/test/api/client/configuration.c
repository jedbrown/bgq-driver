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
 * \file test/api/client/configuration.c
 * \brief Simple client configuration test
 */

#include <pami.h>
#include <stdio.h>


int main (int argc, char ** argv)
{
  pami_client_t client;
  pami_result_t result = PAMI_ERROR;
  char         cl_string[] = "TEST";

  size_t errors = 0;

  result = PAMI_Client_create (cl_string, &client, NULL, 0);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Unable to create client '%s'; result = %d\n", cl_string, result);
    return 1;
  }

  pami_configuration_t configuration;

  configuration.name = PAMI_CLIENT_CLOCK_MHZ;
  result = PAMI_Client_query (client, &configuration, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Unable to query configuration attribute PAMI_CLIENT_CLOCK_MHZ; result = %d\n", result);
    errors++;
  }
  else
    fprintf (stdout, "PAMI_CLIENT_CLOCK_MHZ = %zu\n", configuration.value.intval);

  configuration.name = PAMI_CLIENT_CONST_CONTEXTS;
  result = PAMI_Client_query (client, &configuration, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Unable to query configuration attribute PAMI_CLIENT_CONST_CONTEXTS; result = %d\n", result);
    errors++;
  }
  else
    fprintf (stdout, "PAMI_CLIENT_CONST_CONTEXTS = %zu\n", configuration.value.intval);

  configuration.name = PAMI_CLIENT_HWTHREADS_AVAILABLE;
  result = PAMI_Client_query (client, &configuration, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Unable to query configuration attribute PAMI_CLIENT_HWTHREADS_AVAILABLE; result = %d\n", result);
    errors++;
  }
  else
    fprintf (stdout, "PAMI_CLIENT_HWTHREADS_AVAILABLE = %zu\n", configuration.value.intval);

  configuration.name = PAMI_CLIENT_MEMREGION_SIZE;
  result = PAMI_Client_query (client, &configuration, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Unable to query configuration attribute PAMI_CLIENT_MEMREGION_SIZE; result = %d\n", result);
    errors++;
  }
  else
    fprintf (stdout, "PAMI_CLIENT_MEMREGION_SIZE = %zu\n", configuration.value.intval);

  configuration.name = PAMI_CLIENT_MEM_SIZE;
  result = PAMI_Client_query (client, &configuration, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Unable to query configuration attribute PAMI_CLIENT_MEM_SIZE; result = %d\n", result);
    errors++;
  }
  else
    fprintf (stdout, "PAMI_CLIENT_MEM_SIZE = %zu\n", configuration.value.intval);

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query (client, &configuration, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Unable to query configuration attribute PAMI_CLIENT_NUM_TASKS; result = %d\n", result);
    errors++;
  }
  else
    fprintf (stdout, "PAMI_CLIENT_NUM_TASKS = %zu\n", configuration.value.intval);

  configuration.name = PAMI_CLIENT_NUM_CONTEXTS;
  result = PAMI_Client_query (client, &configuration, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Unable to query configuration attribute PAMI_CLIENT_NUM_CONTEXTS; result = %d\n", result);
    errors++;
  }
  else
    fprintf (stdout, "PAMI_CLIENT_NUM_CONTEXTS = %zu\n", configuration.value.intval);

  configuration.name = PAMI_CLIENT_PROCESSOR_NAME;
  result = PAMI_Client_query (client, &configuration, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Unable to query configuration attribute PAMI_CLIENT_PROCESSOR_NAME; result = %d\n", result);
    errors++;
  }
  else
    fprintf (stdout, "PAMI_CLIENT_PROCESSOR_NAME = %s\n", configuration.value.chararray);

  configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query (client, &configuration, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Unable to query configuration attribute PAMI_CLIENT_TASK_ID; result = %d\n", result);
    errors++;
  }
  else
    fprintf (stdout, "PAMI_CLIENT_TASK_ID = %zu\n", configuration.value.intval);

  configuration.name = PAMI_CLIENT_WTIMEBASE_MHZ;
  result = PAMI_Client_query (client, &configuration, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Unable to query configuration attribute PAMI_CLIENT_WTIMEBASE_MHZ; result = %d\n", result);
    errors++;
  }
  else
    fprintf (stdout, "PAMI_CLIENT_WTIMEBASE_MHZ = %zu\n", configuration.value.intval);

  configuration.name = PAMI_CLIENT_WTICK;
  result = PAMI_Client_query (client, &configuration, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Unable to query configuration attribute PAMI_CLIENT_WTICK; result = %d\n", result);
    errors++;
  }
  else
    fprintf (stdout, "PAMI_CLIENT_WTICK = %f\n", configuration.value.doubleval);

  size_t num_local_tasks = 0;
  configuration.name = PAMI_CLIENT_NUM_LOCAL_TASKS;
  result = PAMI_Client_query (client, &configuration, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Unable to query configuration attribute PAMI_CLIENT_NUM_LOCAL_TASKS; result = %d\n", result);
    errors++;
  }
  else
  {
    num_local_tasks = configuration.value.intval;
    fprintf (stdout, "PAMI_CLIENT_NUM_LOCAL_TASKS = %zu\n", configuration.value.intval);
  }
  
  configuration.name = PAMI_CLIENT_LOCAL_TASKS;
  result = PAMI_Client_query (client, &configuration, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Unable to query configuration attribute PAMI_CLIENT_LOCAL_TASKS; result = %d\n", result);
    errors++;
  }
  else
  {
    size_t i;
    for (i=0; i<num_local_tasks; i++)
    {
      fprintf (stdout, "PAMI_CLIENT_LOCAL_TASKS[%zu] = %zu\n", i, configuration.value.intarray[i]);
    }
  }





  result = PAMI_Client_destroy(&client);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Unable to destroy client '%s'; result = %d\n", cl_string, result);
    errors++;
  }

  if (errors > 0)
  {
    fprintf (stderr, "FAILED (%zu errors)\n", errors);
    return 1;
  }

  fprintf (stderr, "SUCCESS\n");
  return 0;
};
