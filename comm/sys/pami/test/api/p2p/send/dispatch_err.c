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
 * \file test/api/p2p/send/dispatch_err.c
 * \brief Error path point-to-point PAMI_Dispatch_set() test
 */

#include <pami.h>
#include <stdio.h>

pami_task_t PAMIX_Client_task (pami_client_t client)
{
  pami_configuration_t configuration;
  configuration.name = PAMI_CLIENT_TASK_ID;
  
  pami_result_t result = PAMI_ERROR;
  
  result = PAMI_Client_query(client, &configuration, 1);
  
  if (result != PAMI_SUCCESS) exit(1);
  
  return configuration.value.intval;
};

size_t PAMIX_Client_size (pami_client_t client)
{
  pami_configuration_t configuration;
  configuration.name = PAMI_CLIENT_NUM_TASKS;
  
  pami_result_t result = PAMI_ERROR;
  
  result = PAMI_Client_query(client, &configuration, 1);
  
  if (result != PAMI_SUCCESS) exit(1);
  
  return configuration.value.intval;
};

static void decrement (pami_context_t   context,
                       void           * cookie,
                       pami_result_t    result)
{
  volatile unsigned * active = (volatile unsigned *) cookie;
  (*active)--;
}

static void test_dispatch (
  pami_context_t    context,      /**< IN: PAMI context */
  void            * cookie,       /**< IN: dispatch cookie */
  const void      * header_addr,  /**< IN: header address */
  size_t            header_size,  /**< IN: header size */
  const void      * pipe_addr,    /**< IN: address of PAMI pipe buffer */
  size_t            pipe_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t   origin,
  pami_recv_t     * recv)         /**< OUT: receive message structure */
{
  volatile unsigned * active = (volatile unsigned *) cookie;
  (*active)--;
}

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
    
  pami_task_t task = PAMIX_Client_task (client);
  size_t      size = PAMIX_Client_size (client);

  result = PAMI_Context_createv (client, configuration, 0, &context, 1);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to create pami context. result = %d\n", result);
      return 1;
    }

  /* Attempt to send using a dispatch id that has not been registered. */
  char metadata[1024];
  char buffer[1024];

  volatile unsigned recv_active = 1;
  volatile unsigned send_active = 1;
  
  pami_send_t parameters;
  parameters.send.dispatch        = 10;
  parameters.send.header.iov_base = metadata;
  parameters.send.header.iov_len  = 8;
  parameters.send.data.iov_base   = buffer;
  parameters.send.data.iov_len    = 8;
  parameters.events.cookie        = (void *) & send_active;
  parameters.events.local_fn      = decrement;
  parameters.events.remote_fn     = NULL;

  PAMI_Endpoint_create (client, size-1, 0, &parameters.send.dest);

  if (task == 0)
  {
    result = PAMI_Send (context, &parameters);
    if (result == PAMI_SUCCESS)
    {
      fprintf (stderr, "Test failure. Expected error when using an unregistered dispatch id.\n");
      return 1;
    }
  }

  size_t dispatch = 10;
  pami_dispatch_callback_function fn;
  fn.p2p = test_dispatch;
  pami_dispatch_hint_t options = {};
  result = PAMI_Dispatch_set (context, dispatch, fn, (void *) & recv_active, options);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable register pami dispatch. result = %d\n", result);
      return 1;
    }

  if (task == 0)
  {
    PAMI_Endpoint_create (client, size-1, 0, &parameters.send.dest);
    
    result = PAMI_Send (context, &parameters);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to send using a registered dispatch id.\n");
      return 1;
    }
    
    while (send_active)
    {
      result = PAMI_Context_advance (context, 1000);

      if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
        {
          fprintf (stderr, "Error. Unable to advance pami context. result = %d\n", result);
          return 1;
        }
    }
    
    while (recv_active)
    {
      result = PAMI_Context_advance (context, 1000);

      if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
        {
          fprintf (stderr, "Error. Unable to advance pami context. result = %d\n", result);
          return 1;
        }
    }
  }
  else if (task == (size-1))
  {
    PAMI_Endpoint_create (client, 0, 0, &parameters.send.dest);
  
    while (recv_active)
    {
      result = PAMI_Context_advance (context, 1000);

      if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
        {
          fprintf (stderr, "Error. Unable to advance pami context. result = %d\n", result);
          return 1;
        }
    }
    
    result = PAMI_Send (context, &parameters);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to send using a registered dispatch id.\n");
      return 1;
    }
    
    while (send_active)
    {
      result = PAMI_Context_advance (context, 1000);

      if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
        {
          fprintf (stderr, "Error. Unable to advance pami context. result = %d\n", result);
          return 1;
        }
    }    
  }
  
  result = PAMI_Context_destroyv(&context, 1);

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

  return 0;
};
