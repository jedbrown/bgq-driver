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
 * \file test/api/p2p/send/immediate/data_integrity.c
 * \brief Functional test to verify data integrity of send immediate operations
 *
 * This test requires ordered messages.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#include <pami.h>

#define ITERATIONS 1000

typedef struct
{
  size_t                 id;
  pami_dispatch_hint_t   options;
  char                 * name;
  pami_result_t          result;
} dispatch_info_t;

pami_task_t _my_task;
size_t      _num_tasks;

size_t _errors;
size_t _messages_received;

/* --------------------------------------------------------------- */

static void decrement (pami_context_t   context,
                       void           * cookie,
                       pami_result_t    result)
{
  size_t * value = (size_t *) cookie;
  --*value;
}

/* --------------------------------------------------------------- */
static void increment_dispatch_fn (
  pami_context_t        context,      /**< IN: PAMI context */
  void               * cookie,       /**< IN: dispatch cookie */
  const void         * header_addr,  /**< IN: header address */
  size_t               header_size,  /**< IN: header size */
  const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
  size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t origin,
  pami_recv_t         * recv)        /**< OUT: receive message structure */
{
  size_t * value = (size_t *) cookie;
  (*value)++;
}

/* --------------------------------------------------------------- */
static void test_dispatch (
  pami_context_t        context,      /**< IN: PAMI context */
  void               * cookie,       /**< IN: dispatch cookie */
  const void         * header_addr,  /**< IN: header address */
  size_t               header_size,  /**< IN: header size */
  const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
  size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t origin,
  pami_recv_t         * recv)        /**< OUT: receive message structure */
{
  /* Only 'immediate' receives are expected. */
  assert (pipe_addr != NULL);
  assert (recv == NULL);

  size_t * counter = (size_t *) cookie;

  pami_task_t task;
  size_t      offset;
  PAMI_Endpoint_query (origin, &task, &offset);

  /*
   * Error check
   */
  size_t value = *((size_t *) pipe_addr);

  if (counter[task]++ != value)
    {
      _errors++;
      /*fprintf (stdout, "Data integrity error!\n");*/
    }

  _messages_received++;
}

/**
 * \brief Send several messages to { task 0, context 0} == "endpoint 0"
 */
void send (pami_client_t client, pami_context_t context, size_t dispatch, pami_task_t origin, size_t * data)
{
  pami_send_immediate_t parameters;
  parameters.dispatch = dispatch;
  parameters.header.iov_base = NULL;
  parameters.header.iov_len  = 0;
  parameters.data.iov_base   = (void *) data;
  parameters.data.iov_len    = sizeof(size_t);
  PAMI_Endpoint_create (client, 0, 0, &parameters.dest);

  size_t iteration;

  for (iteration = 0; iteration < ITERATIONS; iteration++)
    {
      pami_result_t result = PAMI_ERROR;
      result = PAMI_Send_immediate (context, &parameters);

      assert (result == PAMI_SUCCESS);

      (*data)++;
    }

  return;
}

int main (int argc, char ** argv)
{
  pami_result_t result = PAMI_ERROR;

  pami_client_t client;
  result = PAMI_Client_create ("TEST", &client, NULL, 0);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stdout, "Failed to create client.\n");
      return 1;
    }

  pami_configuration_t configuration;

  configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query(client, &configuration, 1);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stdout, "Failed to query client.\n");
      PAMI_Client_destroy (&client);
      return 1;
    }

  _my_task = configuration.value.intval;

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration, 1);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stdout, "Failed to query client.\n");
      PAMI_Client_destroy (&client);
      return 1;
    }

  _num_tasks = configuration.value.intval;

  if (_num_tasks == 1)
    {
      fprintf (stdout, "This test requires > 1 task.\n");
      PAMI_Client_destroy (&client);
      return 1;
    }



  pami_context_t context;
  result = PAMI_Context_createv (client, NULL, 0, &context, 1);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stdout, "Failed to create context.\n");
      PAMI_Client_destroy (&client);
      return 1;
    }

  size_t n = 0;
  size_t counter[_num_tasks*2];

  for (n = 0; n < _num_tasks*2; n++)
    counter[n] = 0;


  /* Register the protocols to test */
  size_t dispatch_count = 0;
  dispatch_info_t dispatch[2];

  dispatch[dispatch_count].id = 11;
  dispatch[dispatch_count].options = (pami_dispatch_hint_t) {0};
  dispatch[dispatch_count].name = " no shmem ";
  dispatch[dispatch_count].options.use_shmem   = PAMI_HINT_DISABLE;
  dispatch[dispatch_count].options.consistency = PAMI_HINT_ENABLE;
  dispatch[dispatch_count].result = PAMI_ERROR;
  dispatch_count++;

  /* Test the "only shmem" dispatch hint if all tasks are local */
  configuration.name = PAMI_CLIENT_NUM_LOCAL_TASKS;
  result = PAMI_Client_query(client, &configuration, 1);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stdout, "Failed to query client.\n");
      PAMI_Client_destroy (&client);
      return 1;
    }

  if (configuration.value.intval == _num_tasks)
    {
      dispatch[dispatch_count].id = 12;
      dispatch[dispatch_count].options = (pami_dispatch_hint_t) {0};
      dispatch[dispatch_count].name = "only shmem";
      dispatch[dispatch_count].options.use_shmem   = PAMI_HINT_ENABLE;
      dispatch[dispatch_count].options.consistency = PAMI_HINT_ENABLE;
      dispatch[dispatch_count].result = PAMI_ERROR;
      dispatch_count++;
    }

  pami_dispatch_callback_function fn;
  fn.p2p = test_dispatch;
  size_t i;

  for (i = 0; i < dispatch_count; i++)
    {
      dispatch[i].result = PAMI_Dispatch_set (context,
                                              dispatch[i].id,
                                              fn,
                                              (void *) & counter[_num_tasks*i],
                                              dispatch[i].options);
    }

  fn.p2p = increment_dispatch_fn;
  size_t done = 0;
  PAMI_Dispatch_set (context, 0, fn, (void *) &done, (pami_dispatch_hint_t) {0});

  sleep (1);

  _errors = 0;
  _messages_received = 0;
  size_t messages_expected = 0;

  size_t data = 0;


  if (_my_task == 0)
    {
      for (i = 0; i < dispatch_count; i++)
        {
          if (dispatch[i].result == PAMI_SUCCESS)
            {
              messages_expected += ((_num_tasks - 1) * ITERATIONS);
            }
        }

      while (_messages_received < messages_expected)
        {
          /*fprintf (stdout, "do advance(), _messages_received = %zu, messages_expected = %zu\n", _messages_received, messages_expected);*/
          PAMI_Context_advance (context, 100);
        }

      /* Send the "all done" message to all other tasks */
      volatile size_t active = _num_tasks - 1;

      pami_send_t parameters;
      parameters.send.dispatch = 0;
      parameters.send.header.iov_base = NULL;
      parameters.send.header.iov_len  = 0;
      parameters.send.data.iov_base   = NULL;
      parameters.send.data.iov_len    = 0;
      parameters.events.cookie        = (void *) & active;
      parameters.events.local_fn      = decrement;
      parameters.events.remote_fn     = NULL;

      for (i = 1; i < _num_tasks; i++)
        {
          PAMI_Endpoint_create (client, i, 0, &parameters.send.dest);
          PAMI_Send (context, &parameters);
        }

      while (active > 0)
        PAMI_Context_advance (context, 1000000);

    }
  else
    {
      for (i = 0; i < dispatch_count; i++)
        {
          data = 0;
          if (dispatch[i].result == PAMI_SUCCESS)
            {
              send (client, context, dispatch[i].id, _my_task, &data);
            }

        }

      /* Wait for the "all done" message from task 0 */
      while (!done)
        PAMI_Context_advance (context, 1000000);
    }

  PAMI_Context_destroyv (&context, 1);
  PAMI_Client_destroy(&client);

  sleep (1);

  if (_my_task == 0)
    {
      if (_errors)
        fprintf (stdout, "Test FAILED with %zu errors.\n", _errors);
      else
        fprintf (stdout, "Test PASSED.\n");
    }

  return _errors > 0;
}
