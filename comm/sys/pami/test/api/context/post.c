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
 * \file test/api/context/post.c
 * \brief Simple PAMI_Context_post() test
 */

#include <pami.h>
#include <stdio.h>

typedef struct post_info
{
  pami_work_t        state;
  volatile unsigned value;
} post_info_t;

post_info_t _info[2];

pami_result_t do_work (pami_context_t   context,
              void          * cookie)
{
  post_info_t *info = (post_info_t *)cookie;
  fprintf (stderr, "do_work() cookie = %p, %d -> %d\n", cookie, info->value, info->value-1);
  info->value--;
  return PAMI_SUCCESS;
}

int main (int argc, char ** argv)
{
  pami_client_t client;
  pami_context_t context[2];
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
  if (configuration.value.intval < 2)
  {
    fprintf (stderr, "Error. Multi-context not supported. PAMI_CLIENT_NUM_CONTEXTS = %zu\n", configuration.value.intval);
    return 1;
  }

  size_t num = 2;
  result = PAMI_Context_createv (client, &configuration, 0, context, num);
  if (result != PAMI_SUCCESS || num != 2)
  {
    fprintf (stderr, "Error. Unable to create two pami context. result = %d\n", result);
    return 1;
  }

  _info[0].value = 1;
  _info[1].value = 1;



  /* Lock both contexts */
  result = PAMI_Context_lock (context[0]);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to lock the first pami context. result = %d\n", result);
    return 1;
  }
  result = PAMI_Context_lock (context[1]);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to lock the second pami context. result = %d\n", result);
    return 1;
  }



  /* Post some work to the contexts */
  result = PAMI_Context_post (context[0], &_info[0].state, do_work, (void *)&_info[0]);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to post work to the first pami context. result = %d\n", result);
    return 1;
  }
  result = PAMI_Context_post (context[1], &_info[1].state, do_work, (void *)&_info[1]);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to post work to the second pami context. result = %d\n", result);
    return 1;
  }


  while (_info[0].value || _info[1].value)
  {
    result = PAMI_Context_advance (context[0], 1);
    if ((result != PAMI_SUCCESS) && (result != PAMI_EAGAIN))
    {
      fprintf (stderr, "Error. Unable to advance the first pami context. result = %d\n", result);
      return 1;
    }
    result = PAMI_Context_advance (context[1], 1);
    if ((result != PAMI_SUCCESS) && (result != PAMI_EAGAIN))
    {
      fprintf (stderr, "Error. Unable to advance the second pami context. result = %d\n", result);
      return 1;
    }
  }

  result = PAMI_Context_destroyv(context, num);
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
