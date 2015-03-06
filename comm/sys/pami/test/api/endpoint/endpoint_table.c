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
 * \file test/api/endpoint/endpoint_table.c
 * \brief ???
 */

#include <pami.h>
#include <stdio.h>

pami_endpoint_t * _endpoint;
uint8_t _tmpbuffer[10240];

static void createEndpointTable (pami_client_t client, int num)
{
  pami_configuration_t configuration;
  configuration.name = PAMI_CLIENT_NUM_TASKS;
  pami_result_t result =
    PAMI_Client_query(client, &configuration,1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Unable to query PAMI_CLIENT_NUM_TASKS\n");
    abort();
  }

  size_t global_tasks = configuration.value.intval;

  _endpoint =
    (pami_endpoint_t *) malloc (sizeof(pami_endpoint_t) *
                                global_tasks * num);

  size_t i, j;
  for (i=0; i<global_tasks; i++)
  {
    for (j=0; j<num; j++)
    {
      PAMI_Endpoint_create (client, i, j, &_endpoint[i*num+j]);
      fprintf (stderr, "_endpoint[%zu] = 0x%08x\n", i*num+j, _endpoint[i*num+j]);
    }
  }
};

static void decrement (pami_context_t   context,
                       void           * cookie,
                       pami_result_t    result)
{
  volatile size_t * value = (volatile size_t *) cookie;
  (*value)--;
};

static void test_dispatch (
    pami_context_t       context,      /**< IN: PAMI context */
    void               * cookie,       /**< IN: dispatch cookie */
    const void         * header_addr,  /**< IN: header address */
    size_t               header_size,  /**< IN: header size */
    const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
    size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
    pami_endpoint_t      origin,
    pami_recv_t        * recv)         /**< OUT: receive message structure */
{
  fprintf (stderr, "recv'd message from endpoint 0x%08x.\n", origin);
  volatile size_t * expect = (volatile size_t *) cookie;

  if (pipe_addr != NULL)
  {
    fprintf (stderr, "short recv:  decrement cookie = %p, %zu => %zu\n", cookie, *expect, *expect-1);
    (*expect)--;
    return;
  }

  fprintf (stderr, "long recv");
  recv->local_fn = decrement;
  recv->cookie   = cookie;
  recv->type     = PAMI_TYPE_BYTE;
  recv->addr     = (void *)_tmpbuffer;
  recv->offset   = 0;

  return;
}



static pami_result_t send_endpoint (pami_context_t   context,
                                    size_t           target,
                                    pami_send_t    * parameters)
{
  parameters->send.dest = _endpoint[target];
  fprintf (stderr, "send message to endpoint 0x%08x.\n", parameters->send.dest);
  return PAMI_Send (context, parameters);
};

int main ()
{
  pami_client_t client;
  pami_context_t *context;
  pami_result_t result;
  pami_configuration_t configuration;
  
  PAMI_Client_create ("TEST", &client, NULL, 0);
  
  configuration.name = PAMI_CLIENT_NUM_CONTEXTS;
  result = PAMI_Client_query(client, &configuration, 1);
  size_t num = configuration.value.intval;
  context = (pami_context_t*) malloc (num*sizeof(pami_context_t));
  
  /* Create four contexts - every task creates the same number */
  PAMI_Context_createv (client, NULL, 0, context, num);

  createEndpointTable (client, num);


  pami_dispatch_callback_function fn;
  fn.p2p = test_dispatch;
  pami_dispatch_hint_t options = {0};
  pami_send_hint_t hints = {0};
  volatile size_t expect = 0;

  size_t i;
  for (i=0; i<num; i++)
  {
    PAMI_Context_lock (context[i]);
    result = PAMI_Dispatch_set (context[i], 0, fn, (void *)&expect, options);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable register pami dispatch. result = %d\n", result);
      return 1;
    }
  }

  configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query(client, &configuration, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, result);
    return 1;
  }
  pami_task_t task_id = configuration.value.intval;
  fprintf (stderr, "My task id = %d\n", task_id);

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, result);
    return 1;
  }
  size_t num_tasks = configuration.value.intval;
  fprintf (stderr, "Number of tasks = %zu\n", num_tasks);

  uint8_t header[16];
  uint8_t data[1024];
  volatile size_t active = 1;

  pami_send_t parameters;
  parameters.send.dispatch        = 0;
  parameters.send.header.iov_base = header;
  parameters.send.header.iov_len  = 16;
  parameters.send.data.iov_base   = data;
  parameters.send.data.iov_len    = 1024;
  parameters.send.hints           = hints;
  parameters.events.cookie        = (void *) &active;
  parameters.events.local_fn      = decrement;
  parameters.events.remote_fn     = NULL;

  /* Send a message to endpoint "num_tasks * num_contexts - 1" */
  pami_task_t target_task = (pami_task_t) -1;
  size_t      target_offset = (size_t) -1;
  PAMI_Endpoint_query (_endpoint[num*num_tasks-1], &target_task, &target_offset);
  if (task_id == target_task) expect += num_tasks;
  send_endpoint (context[0], num*num_tasks-1, &parameters);

  fprintf (stdout, "before advance, active = %zu, expect = %zu\n", active, expect);
  while ((active + expect) > 0) PAMI_Context_advancev (context, num, 100);

  for (i=0; i<num; i++) PAMI_Context_unlock (context[i]);

  result = PAMI_Context_destroyv (context, num);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to destroy pami context. result = %d\n", result);
    return 1;
  }

  result = PAMI_Client_destroy (&client);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to finalize pami client. result = %d\n", result);
    return 1;
  }

  fprintf (stdout, "Success (%d)\n", task_id);

  return 0;
};
