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
 * \file test/api/context/advance.c
 * \brief Simple PAMI_Context_advance() test
 */

#include <pami.h>
#include <stdio.h>


int main (int argc, char ** argv)
{
  pami_client_t client;
  pami_context_t context;
  pami_configuration_t * configuration = NULL;
  char                  cl_string[] = "TEST";
  pami_result_t result = PAMI_ERROR;

  result = PAMI_Client_create (cl_string, &client, NULL, 0);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to initialize pami client. result = %d\n", result);
    return 1;
  }

        {  result = PAMI_Context_createv(client, configuration, 0, &context, 1); }
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to create pami context. result = %d\n", result);
    return 1;
  }

  if (result == PAMI_SUCCESS)
  {
    fprintf (stdout, "Before PAMI_Context_advance()\n");
    result = PAMI_Context_advance (context, 1);
    fprintf (stdout, " After PAMI_Context_advance(), result = %d\n", result);

    result = PAMI_Context_destroyv(&context, 1);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to destroy pami context. result = %d\n", result);
      return 1;
    }
  }

  result = PAMI_Client_destroy(&client);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to finalize pami client. result = %d\n", result);
    return 1;
  }

  return 0;
};
