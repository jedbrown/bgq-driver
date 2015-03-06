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
 * \file test/api/context/create.c
 * \brief Simple PAMI_Context_createv() test
 */

#include <pami.h>
#include <stdio.h>
#include "../pami_util.h"

int main (int argc, char ** argv)
{
  pami_client_t client;
  pami_context_t context[2048];
  pami_configuration_t configuration;
  char                  cl_string[] = "TEST";

  pami_result_t result = PAMI_ERROR;

  result = PAMI_Client_create (cl_string, &client, NULL, 0);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to initialize pami client. result = %d\n", result);
      return 1;
    }

  configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query (client, &configuration, 1);

  if (result != PAMI_SUCCESS)
    fprintf (stderr, "Unable to query configuration attribute PAMI_CLIENT_TASK_ID; result = %d\n", result);

  size_t task = configuration.value.intval;

  configuration.name = PAMI_CLIENT_NUM_CONTEXTS;
  result = PAMI_Client_query (client, &configuration, 1);

  if (result != PAMI_SUCCESS)
    fprintf (stderr, "Unable to query configuration attribute PAMI_CLIENT_NUM_CONTEXTS; result = %d\n", result);
  else if (task == 0)
    fprintf (stdout, "PAMI_CLIENT_NUM_CONTEXTS = %zu\n", configuration.value.intval);


  int num = 1;

  if (argc > 1)
    num = atoi(argv[1]);

  if (task == 0)
    fprintf (stdout, "Create %d pami contexts\n", num);

  {  result = PAMI_Context_createv(client, NULL, 0, context, num); }

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to create pami context. result = %d\n", result);
      return 1;
    }

  delayTest(1);

  result = PAMI_Context_destroyv(context, num);;

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to destroy pami context. result = %d\n", result);
      return 1;
    }

  result = PAMI_Client_destroy(&client);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to finalize pami client. result = %d\n", result);
      return 1;
    }

  if (task == 0)
    fprintf (stdout, "Success.\n");

  return 0;
};
