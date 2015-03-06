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
 * \file test/api/context/multi-advance.c
 * \brief Simple PAMI_Context_advance() multiple test
 */

#include <pami.h>
#include <stdio.h>


int main (int argc, char ** argv)
{
  pami_client_t client;
  pami_context_t * context;
  pami_configuration_t configuration;
  char                  cl_string[] = "TEST";
  pami_result_t result = PAMI_ERROR;

  result = PAMI_Client_create (cl_string, &client, NULL, 0);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to initialize pami client. result = %d\n", result);
    return 1;
  }

  configuration.name = PAMI_CLIENT_NUM_CONTEXTS;
  result = PAMI_Client_query(client, &configuration, 1);
  size_t num = configuration.value.intval;

  fprintf (stderr, "PAMI_CLIENT_NUM_CONTEXTS = %zu\n", num);
  context = (pami_context_t *) malloc(sizeof(pami_context_t)*num);

  size_t tmp = num;
  result = PAMI_Context_createv (client, &configuration, 0, context, num);
  if (result != PAMI_SUCCESS || num != tmp)
  {
    fprintf (stderr, "Error. Unable to create %zu pami context(s). result = %d\n", tmp, result);
    return 1;
  }

  size_t i;
  for (i=0; i<num; i++)
  {
    if ((result == PAMI_SUCCESS) || (result == PAMI_EAGAIN))
    {
      fprintf (stdout, "Before PAMI_Context_advance(), context = %zu\n", i);
      result = PAMI_Context_advance (context[i], 1);
      fprintf (stdout, " After PAMI_Context_advance(), context = %zu, result = %d\n", i, result);
    }
  }

  result = PAMI_Context_destroyv (context, num);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to destroy first pami context. result = %d\n", result);
    return 1;
  }

  result = PAMI_Client_destroy(&client);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to finalize pami client. result = %d\n", result);
    return 1;
  }

  fprintf (stderr, "Success.\n");

  return 0;
};
