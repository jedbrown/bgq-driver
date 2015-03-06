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
 * \file test/api/p2p/fence_all/simple_fence_all_func.cc
 * \brief Simple point-to-point PAMI_Fence_all() test
 *
 * This test implements a very simple test using PAMI_Fence_all
 * to ensure the data integrity.  In the test, one task uses
 * PAMI_Put() to send variable-sized data to several endpoints in
 * a streaming way, and then issues PAMI_Fence_all().  After the 
 * done_fn of the PAMI_Fence_all() is called, the same task isuues
 * PAMI_Get() to get the data we sent using PAMI_Put from remote
 * endpoints and verify it is the same as what we sent.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pami.h>

/* data type for test */
typedef int elem_t;

const size_t MAX_ELEM_PER_MSG = (1<<16); /* 2^16 */
/* max. buffer size; it should be big enough to hold the array of size
 * sizeof(elem_t) + 2*sizeof(elem_t) + 2^2*sizeof(elem_t) + ... +
 * 2^16*sizeof(elem_t) bytes */
const size_t MAX_ELEM_CNT = 2 * MAX_ELEM_PER_MSG - 1;

elem_t send_buf [MAX_ELEM_CNT];
elem_t recv_buf [MAX_ELEM_CNT];

elem_t** remote_buf_addr;

pami_client_t     client;
pami_context_t    context;
pami_send_hint_t  null_send_hint;
pami_dispatch_hint_t  null_dispatch_hint;
pami_endpoint_t  *endpts;

typedef struct
{
  elem_t*     addr;
  pami_task_t tsk;
} info_t;

enum {
  DISPATCH_ADDR_RECV = 1,
  DISPATCH_TEST_DONE
};

static void dispatch_addr_recv (
    pami_context_t       context,      /**< IN: PAMI context */
    void               * cookie,       /**< IN: dispatch cookie */
    const void         * header_addr,  /**< IN: header address */
    size_t               header_size,  /**< IN: header size */
    const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
    size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
    pami_endpoint_t      origin,
    pami_recv_t        * recv)        /**< OUT: receive message structure */
{
  volatile size_t * ready = (volatile size_t *) cookie;
  info_t*           info  = (info_t*) header_addr;

  remote_buf_addr[info->tsk] = info->addr;

  (*ready)++;
#ifdef DEBUG
  fprintf (stderr, "'addr_recv' dispatch function. ready = %zu\n", *ready);
#endif /* DEBUG */

  return;
}

static void dispatch_test_done (
    pami_context_t       context,      /**< IN: PAMI context */
    void               * cookie,       /**< IN: dispatch cookie */
    const void         * header_addr,  /**< IN: header address */
    size_t               header_size,  /**< IN: header size */
    const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
    size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
    pami_endpoint_t      origin,
    pami_recv_t        * recv)        /**< OUT: receive message structure */
{
  volatile size_t * ready = (volatile size_t *) cookie;

  (*ready)++;
  fprintf (stderr, "'test_done' dispatch function. ready = %zu\n", *ready);

  return;
}

static void fence_done (
    pami_context_t context,
    void          *cookie,
    pami_result_t  result) 
{
  assert (result == PAMI_SUCCESS);

  volatile bool *_done = (volatile bool *)cookie;
  (*_done) = true;
  fprintf (stderr, "'fence_done' dispatch function. \n");
}

int main (int argc, char* argv[])
{
  char            cl_string[] = "pami";
  pami_result_t   result      = PAMI_ERROR;
  pami_task_t     tsk;
  volatile size_t ready       = 1;

  result = PAMI_Client_create (cl_string, &client, NULL, 0);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to create pami client. result = %d\n", result);
    return 1;
  }

  result = PAMI_Context_createv(client, NULL, 0, &context, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to create pami context(s). result = %d\n", result);
    return 1;
  }

  pami_configuration_t configuration;

  configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query(client, &configuration,1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, result);
    return 1;
  }
  pami_task_t task_id = configuration.value.intval;
  fprintf (stderr, "My task id = %d\n", task_id);

  pami_dispatch_callback_function fn;

  fn.p2p = dispatch_addr_recv;
  result = PAMI_Dispatch_set (context,
      DISPATCH_ADDR_RECV,
      fn,
      (void *)&ready,
      null_dispatch_hint);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable register pami dispatch. result = %d\n", result);
    return 1;
  }

  fn.p2p = dispatch_test_done;
  result = PAMI_Dispatch_set (context,
      DISPATCH_TEST_DONE,
      fn,
      (void *)&ready,
      null_dispatch_hint);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable register pami dispatch. result = %d\n", result);
    return 1;
  }

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration,1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, result);
    return 1;
  }

  size_t num_tasks = configuration.value.intval;
  fprintf (stderr, "Number of tasks = %zu\n", num_tasks);
  if (num_tasks < 2)
  {
    fprintf (stderr, "Error. This test requires at least two tasks\n");
    return 1;
  }

  /* initialize null_send_hint */
  memset(&null_send_hint, 0, sizeof(null_send_hint));

  /* initialize send_buf */
  elem_t start_val = (elem_t)task_id;
  for (size_t i = 0; i < MAX_ELEM_CNT; ++i)
  {
    send_buf[i] = start_val + i;
  }
#ifdef DEBUG
  {
    fprintf(stderr, "MAX_ELEM_PER_MSG=%zu\n", MAX_ELEM_PER_MSG);
    fprintf(stderr, "MAX_ELEM_CNT=%zu\n", MAX_ELEM_CNT);
    fprintf(stderr, "Init send buf:\n");
    for (size_t i = 1, loc=0; i <= MAX_ELEM_PER_MSG; i = 2 * i)
    {
      fprintf(stderr, "loc=%zu(0x%p) 0x", loc, &send_buf[loc]);
      for (size_t j = 0; j < i; ++j, loc++)
      {
        fprintf(stderr, "%02x ", send_buf[loc]);
      }
      fprintf(stderr, "\n");
    }
  }
#endif /* DEBUG */

  /* initialize recv_buf */
  memset(&recv_buf, 0, MAX_ELEM_CNT * sizeof (elem_t));

  /* allocate endpoint array; assuming one endpoint per task now */
  endpts = (pami_endpoint_t*) malloc (sizeof(pami_endpoint_t) * num_tasks);
  if (NULL == endpts)
  {
    fprintf(stderr, "Error. Out of memory\n");
    return 1;
  }

  /* initialize endpts */
  for (tsk = 0; tsk < num_tasks; ++tsk)
  {
    result = PAMI_Endpoint_create(client, tsk, 0, &endpts[tsk]);
    if (PAMI_SUCCESS != result)
    {
      fprintf(stderr, "Error. PAMI_Endpoint_create failed with %d\n", result);
      return 1;
    }
  }

  /* gather remote destination buffer addresses */
  remote_buf_addr = (elem_t**) malloc (sizeof(elem_t*) * num_tasks);
  if (NULL == remote_buf_addr)
  {
    fprintf(stderr, "Error. Out of memory\n");
    return 1;
  }

  remote_buf_addr[task_id] = recv_buf;

  if (task_id == 0)
  {
    while (num_tasks > ready)
      PAMI_Context_advance(context, 100);
  }
  else
  {
    info_t                info = {recv_buf, task_id};
    pami_send_immediate_t parameters;
    parameters.dest            = endpts[0];
    parameters.dispatch        = DISPATCH_ADDR_RECV;
    parameters.header.iov_base = &info;
    parameters.header.iov_len  = sizeof(info);
    parameters.data.iov_base   = NULL;
    parameters.data.iov_len    = 0;
    parameters.hints           = null_send_hint;
    result = PAMI_Send_immediate (context, &parameters);
    if (PAMI_SUCCESS != result)
    {
      fprintf(stderr,
          "Error. PAMI_Send_immediate failed with result %d\n", result);
      return 1;
    }
  }

  if (task_id == 0) {
    result = PAMI_Fence_begin(context);
    if (result != PAMI_SUCCESS)
    {
      fprintf(stderr, "Error. PAMI_Fence_begin() failed. result = %d\n", result);
      return 1;
    }

    /* Send data from task 0 to all endpoints */
    size_t offset = 0;
    for (size_t elem_cnt = 1; elem_cnt <= MAX_ELEM_PER_MSG; elem_cnt *= 2)
    {
      pami_put_simple_t parameters;
      memset(&parameters, 0, sizeof(parameters));
      parameters.rma.hints   = null_send_hint;
      parameters.rma.bytes   = elem_cnt * sizeof(elem_t);
      parameters.rma.cookie  = NULL;
      parameters.rma.done_fn = NULL;
      parameters.addr.local  = send_buf + offset;
      fprintf(stderr, "Sending %zu element message\n", elem_cnt);
      for (tsk = 1; tsk < num_tasks; ++tsk)
      {
        parameters.rma.dest  = endpts[tsk];
        parameters.addr.remote = remote_buf_addr[tsk] + offset;
        result = PAMI_Put(context, &parameters);
        if (result != PAMI_SUCCESS)
        {
          fprintf(stderr, "Error. PAMI_Put() failed. result = %d\n", result);
          return 1;
        }
      }
      offset += elem_cnt;
    }

    /* use PAMI_Fence_all to ensure data arriving */
    volatile bool _done = false;
    result = PAMI_Fence_all(context, fence_done, (void*)&_done);
    if (result != PAMI_SUCCESS) {
      fprintf(stderr, "Error. PAMI_Fence_all returns %d\n", result);
      return 1;
    }

    while (!_done)
    {
      PAMI_Context_advance(context, 100);
    }

    /* use PAMI_Get to get remote data */
    tsk = 1;
    offset = 0;
    for (size_t elem_cnt = 1; elem_cnt <= MAX_ELEM_PER_MSG; elem_cnt *= 2)
    {
      pami_get_simple_t parameters;
      parameters.rma.dest  = endpts[tsk];
      parameters.rma.hints = null_send_hint;
      parameters.rma.bytes = elem_cnt * sizeof(elem_t);
      parameters.rma.cookie= NULL;
      parameters.rma.done_fn = NULL;
      parameters.addr.local  = recv_buf + offset;
      parameters.addr.remote = remote_buf_addr[tsk] + offset;
      result = PAMI_Get(context, &parameters);
      if (result != PAMI_SUCCESS)
      {
        fprintf(stderr, "Error. PAMI_Get() failed. result = %d\n", result);
        return 1;
      }
      offset += elem_cnt;
      tsk = ((tsk + 1)%num_tasks);
      if (tsk == 0) 
        tsk++;
    }

    /* another fence to ensure we got the remote data */
    _done = false;
    result = PAMI_Fence_all(context, fence_done, (void*)&_done);
    if (result != PAMI_SUCCESS) {
      fprintf(stderr, "Error. PAMI_Fence_all returns %d\n", result);
      return 1;
    }

    while (!_done)
    {
      PAMI_Context_advance(context, 100);
    }

    result = PAMI_Fence_end(context);
    if (result != PAMI_SUCCESS)
    {
      fprintf(stderr, "Error. PAMI_Fence_end() failed. result = %d\n", result);
      return 1;
    }
#ifdef DEBUG
    if (task_id == 0)
    {
      fprintf(stderr, "final recv buf:\n");
      for (size_t i = 1, loc=0; i <= MAX_ELEM_PER_MSG; i = 2 * i)
      {
        fprintf(stderr, "0x");
        for (size_t j = 0; j < i; ++j, loc++)
        {
          fprintf(stderr, "%02x ", recv_buf[loc]);
        }
        fprintf(stderr, "\n");
      }
    }
#endif /* DEBUG */
    /* verify data */
    if (0 != memcmp(send_buf, recv_buf, sizeof(elem_t) * MAX_ELEM_CNT))
    {
      fprintf(stderr, "Error. Data verification failed\n");
      return 1;
    }

    /* send signal to the other tasks */
    for (tsk = 1; tsk < num_tasks; ++tsk)
    {
      pami_send_immediate_t parameters;
      parameters.dest            = endpts[tsk];
      parameters.dispatch        = DISPATCH_TEST_DONE;
      parameters.header.iov_base = NULL;
      parameters.header.iov_len  = 0;
      parameters.data.iov_base   = NULL;
      parameters.data.iov_len    = 0;
      parameters.hints           = null_send_hint;
      result = PAMI_Send_immediate (context, &parameters);
      if (PAMI_SUCCESS != result)
      {
        fprintf(stderr,
            "Error. PAMI_Send_immediate failed with result %d\n", result);
        return 1;
      }
    }
  } else {
    /* wait for signal to continue */
    while (1 == ready) {
      PAMI_Context_advance(context, 100);
    }
  }

  printf("PASSED\n");
  result = PAMI_Context_destroyv(&context, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to destroy pami context. result = %d\n", result);
    return 1;
  }

  result = PAMI_Client_destroy(&client);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to destroy pami client. result = %d\n", result);
    return 1;
  }
  free (remote_buf_addr);
  free (endpts);
  return 0;
}
