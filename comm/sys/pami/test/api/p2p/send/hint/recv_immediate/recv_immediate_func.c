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
 * \file test/api/p2p/send/hint/recv_immediate/recv_immediate_func.c
 * \brief ???
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#include <pami.h>

#define DISPATCH_ID_DEFAULT_EXPECT_IMMEDIATE  10
#define DISPATCH_ID_DEFAULT_EXPECT_ASYNC      11
#define DISPATCH_ID_ENABLE                    12
#define DISPATCH_ID_DISABLE                   13

#define EXPECT_IMMEDIATE 0
#define EXPECT_ASYNC     1

volatile size_t __test_errors;
volatile size_t __test_recvs;

uint8_t __junk[1024*16];

pami_task_t client_task (pami_client_t client)
{
  pami_configuration_t configuration;
  configuration.name = PAMI_CLIENT_TASK_ID;
  pami_result_t result = PAMI_ERROR;
  result = PAMI_Client_query (client, &configuration, 1);
  if (result == PAMI_SUCCESS)
    {
      return configuration.value.intval;
    }
  else
    {
      fprintf (stderr, "Error. Unable to query task id. result = %d\n", result);
      exit (1);
    }
  
  return 0;
}

size_t client_size (pami_client_t client)
{
  pami_configuration_t configuration;
  configuration.name = PAMI_CLIENT_NUM_TASKS;
  pami_result_t result = PAMI_ERROR;
  result = PAMI_Client_query (client, &configuration, 1);
  if (result == PAMI_SUCCESS)
    {
      return configuration.value.intval;
    }
  else
    {
      fprintf (stderr, "Error. Unable to query client size. result = %d\n", result);
      exit (1);
    }
  
  return 0;
}

size_t recv_immediate_max (pami_context_t context, size_t id)
{
  pami_configuration_t configuration;
  configuration.name = PAMI_DISPATCH_RECV_IMMEDIATE_MAX;
  pami_result_t result = PAMI_ERROR;
  result = PAMI_Dispatch_query (context, id, &configuration, 1);
  if (result == PAMI_SUCCESS)
    {
      return configuration.value.intval;
    }
  else
    {
      fprintf (stderr, "Error. Unable to query dispatch %zu. result = %d\n", id, result);
      exit (1);
    }
  
  return 0;
}

size_t send_immediate_max (pami_context_t context, size_t id)
{
  pami_configuration_t configuration;
  configuration.name = PAMI_DISPATCH_SEND_IMMEDIATE_MAX;
  pami_result_t result = PAMI_ERROR;
  result = PAMI_Dispatch_query (context, id, &configuration, 1);
  if (result == PAMI_SUCCESS)
    {
      return configuration.value.intval;
    }
  else
    {
      fprintf (stderr, "Error. Unable to query dispatch %zu. result = %d\n", id, result);
      exit (1);
    }
  
  return 0;
}


/* --------------------------------------------------------------- */

static void decrement (pami_context_t   context,
                       void          * cookie,
                       pami_result_t    result)
{
  volatile size_t * value = (volatile size_t *) cookie;
  --*value;
}

/* --------------------------------------------------------------- */

static void dispatch_fn (
  pami_context_t       context,      /**< IN: PAMI context */
  void               * cookie,       /**< IN: dispatch cookie */
  const void         * header_addr,  /**< IN: header address */
  size_t               header_size,  /**< IN: header size */
  const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
  size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t      origin,
  pami_recv_t        * recv)         /**< OUT: receive message structure */
{
  uintptr_t expect = (uintptr_t) cookie;
  
  if (expect == EXPECT_IMMEDIATE)
  {
    if (pipe_addr == NULL) __test_errors++;    
    if (recv != NULL)      __test_errors++;
  }
  else if (expect == EXPECT_ASYNC)
  {
    if (pipe_addr != NULL) __test_errors++;    
    if (recv == NULL)      __test_errors++;
  }
  else
  {
    /* Doh! */
    exit (1);
  }
  
  if (recv)
    {
      recv->local_fn    = NULL;
      recv->cookie      = NULL;
      recv->type        = PAMI_TYPE_BYTE;
      recv->addr        = __junk;
      recv->offset      = 0;
      recv->data_fn     = PAMI_DATA_COPY;
      recv->data_cookie = NULL;
    }
}

int main (int argc, char ** argv)
{
  pami_client_t client;
  pami_context_t context;
  pami_task_t task;
  size_t size;
  pami_dispatch_callback_function fn;
  pami_dispatch_hint_t options;
  
  pami_result_t result = PAMI_ERROR;

  /* ====== INITIALIZE ====== */

  result = PAMI_Client_create ("TEST", &client, NULL, 0);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to initialize pami client. result = %d\n", result);
    return 1;
  }

  task = client_task (client);
  size = client_size (client);
  
  result = PAMI_Context_createv (client, NULL, 0, &context, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to create pami context. result = %d\n", result);
    return 1;
  }
  
  
  fn.p2p = dispatch_fn;
  
  options.recv_immediate = PAMI_HINT_DEFAULT;
  result = PAMI_Dispatch_set (context,
                              DISPATCH_ID_DEFAULT_EXPECT_IMMEDIATE,
                              fn,
                              (void *) EXPECT_IMMEDIATE,
                              options);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to register DISPATCH_ID_DEFAULT_EXPECT_IMMEDIATE. result = %d\n", result);
    return 1;
  }

  options.recv_immediate = PAMI_HINT_DEFAULT;
  result = PAMI_Dispatch_set (context,
                              DISPATCH_ID_DEFAULT_EXPECT_ASYNC,
                              fn,
                              (void *) EXPECT_ASYNC,
                              options);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to register DISPATCH_ID_DEFAULT_EXPECT_ASYNC. result = %d\n", result);
    return 1;
  }

  options.recv_immediate = PAMI_HINT_ENABLE;
  result = PAMI_Dispatch_set (context,
                              DISPATCH_ID_ENABLE,
                              fn,
                              (void *) EXPECT_IMMEDIATE,
                              options);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to register DISPATCH_ID_ENABLE. result = %d\n", result);
    return 1;
  }

  options.recv_immediate = PAMI_HINT_DISABLE;
  result = PAMI_Dispatch_set (context,
                              DISPATCH_ID_DISABLE,
                              fn,
                              (void *) EXPECT_ASYNC,
                              options);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to register DISPATCH_ID_DISABLE. result = %d\n", result);
    return 1;
  }


  /* ====== START TEST ====== */
  
  __test_errors = 0;
  __test_recvs  = 0;
  
  size_t test_count = 0;
  
  volatile size_t send_active = 0;
  
  pami_send_t parameters;
  parameters.send.header.iov_base = __junk;
  parameters.send.header.iov_len  = 0;
  parameters.send.data.iov_base   = __junk;
  parameters.send.data.iov_len    = 0;
  parameters.send.dispatch        = 0;
  parameters.events.cookie        = (void *) & send_active;
  parameters.events.local_fn      = decrement;
  parameters.events.remote_fn     = NULL;

  result = PAMI_Endpoint_create (client, 0, 0, &parameters.send.dest);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error:  PAMI_Endpoint_create() failed for task 0, context 0 with %d.\n", result);
	  return 1;
  }
  
  /* ===================================================================
   * 'recv_immediate' default
   * 
   * (header+data) > recv_immediate_max MUST be an asynchronous receive
   * 
   * A zero-byte send will \b always result in an immediate receive.
   * \see pami_dispatch_p2p_function
   * 
   * Data sizes to test:
   *  - recv_immediate_max + 1
   *  - 0
   */

  parameters.send.data.iov_len    = recv_immediate_max (context, DISPATCH_ID_DEFAULT_EXPECT_ASYNC) + 1;
  parameters.send.dispatch        = DISPATCH_ID_DEFAULT_EXPECT_ASYNC;
  
  test_count++;
  if (task == 1)
  {
    send_active++;
    result = PAMI_Send (context, &parameters);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error:  Unable to send to 0x%08x using dispatch %zu with %d.\n", parameters.send.dest, parameters.send.dispatch, result);
	    return 1;
    }
  }

  parameters.send.data.iov_len    = 0;
  parameters.send.dispatch        = DISPATCH_ID_DEFAULT_EXPECT_IMMEDIATE;
  
  test_count++;
  if (task == 1)
  {
    send_active++;
    result = PAMI_Send (context, &parameters);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error:  Unable to send to 0x%08x using dispatch %zu with %d.\n", parameters.send.dest, parameters.send.dispatch, result);
	    return 1;
    }
  }

  /* ===================================================================
   * 'recv_immediate' enabled
   * 
   * All receives are 'immediate'. (header+data) > recv_immediate_max is
   * invalid, but may not neccesarily return an error.
   * 
   * Data sizes to test:
   *  - 0
   *  - recv_immediate_max
   *  - recv_immediate_max + 1   ...... ?
   */
  parameters.send.data.iov_len    = 0;
  parameters.send.dispatch        = DISPATCH_ID_ENABLE;
  
  test_count++;
  if (task == 1)
  {
    send_active++;
    result = PAMI_Send (context, &parameters);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error:  Unable to send to 0x%08x using dispatch %zu with %d.\n", parameters.send.dest, parameters.send.dispatch, result);
	    return 1;
    }
  }

  parameters.send.data.iov_len    = recv_immediate_max (context, DISPATCH_ID_DEFAULT_EXPECT_ASYNC);
  parameters.send.dispatch        = DISPATCH_ID_ENABLE;
  
  test_count++;
  if (task == 1)
  {
    send_active++;
    result = PAMI_Send (context, &parameters);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error:  Unable to send to 0x%08x using dispatch %zu with %d.\n", parameters.send.dest, parameters.send.dispatch, result);
	    return 1;
    }
  }

#if 0
  parameters.send.data.iov_len    = recv_immediate_max (context, DISPATCH_ID_DEFAULT_EXPECT_ASYNC) + 1;
  parameters.send.dispatch        = DISPATCH_ID_ENABLE;
  
  test_count++;
  if (task == 1)
  {
    send_active++;
    result = PAMI_Send (context, &parameters);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error:  Unable to send to 0x%08x using dispatch %d with %d.\n", parameters.send.dest, parameters.send.dispatch, result);
	    return 1;
    }
  }
#endif

  /* ===================================================================
   * 'recv_immediate' disabled
   * 
   * All receives are NOT 'immediate' - even "zero byte data"
   * 
   * Data sizes to test:
   *  - 0
   *  - recv_immediate_max
   *  - recv_immediate_max + 1
   */
  parameters.send.data.iov_len    = 0;
  parameters.send.dispatch        = DISPATCH_ID_DISABLE;
  
  test_count++;
  if (task == 1)
  {
    send_active++;
    result = PAMI_Send (context, &parameters);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error:  Unable to send to 0x%08x using dispatch %zu with %d.\n", parameters.send.dest, parameters.send.dispatch, result);
	    return 1;
    }
  }

  parameters.send.data.iov_len    = recv_immediate_max (context, DISPATCH_ID_DEFAULT_EXPECT_ASYNC);
  parameters.send.dispatch        = DISPATCH_ID_DISABLE;
  
  test_count++;
  if (task == 1)
  {
    send_active++;
    result = PAMI_Send (context, &parameters);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error:  Unable to send to 0x%08x using dispatch %zu with %d.\n", parameters.send.dest, parameters.send.dispatch, result);
	    return 1;
    }
  }

  parameters.send.data.iov_len    = recv_immediate_max (context, DISPATCH_ID_DEFAULT_EXPECT_ASYNC) + 1;
  parameters.send.dispatch        = DISPATCH_ID_DISABLE;
  
  test_count++;
  if (task == 1)
  {
    send_active++;
    result = PAMI_Send (context, &parameters);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error:  Unable to send to 0x%08x using dispatch %zu with %d.\n", parameters.send.dest, parameters.send.dispatch, result);
	    return 1;
    }
  }


  /* ====== WAIT FOR COMMUNICATION COMPLETION ====== */
  
  if (task == 0)
  {
    while (__test_recvs < test_count)
      PAMI_Context_advance (context, 1000);
  }
  else if (task == 1)
  {
    while (send_active)
      PAMI_Context_advance (context, 1000);
  }


  /* ====== CLEANUP ====== */

  result = PAMI_Context_destroyv (&context, 1);
  if (result != PAMI_SUCCESS) {
    fprintf (stderr, "Error. Unable to destroy context, result = %d\n", result);
    return 1;
  }

  result = PAMI_Client_destroy (&client);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to destroy pami client. result = %d\n", result);
    return 1;
  }

  
  /* ====== REPORT ERRORS ====== */

  if (__test_errors > 0)
  {
    fprintf (stderr, "Error. Non-compliant PAMI receive immediate implementation! error count = %zu\n", __test_errors);
    return 1;
  }
  
  return 0;
}
