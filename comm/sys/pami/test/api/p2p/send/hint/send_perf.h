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
 * \file test/api/p2p/send/hint/send_perf.h
 * \brief ???
 */

#ifndef __test_api_p2p_send_hint_send_perf_h__
#define __test_api_p2p_send_hint_send_perf_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#include <pami.h>

#define MPI_MSGINFO_SIZE 16

#define ITERATIONS 1000

#ifndef BUFSIZE
#define BUFSIZE 4*1024
#endif

#define WARMUP

#undef TRACE_ERR
#ifndef TRACE_ERR
#define TRACE_ERR(x) /*  fprintf x */
#endif

typedef struct
{
  size_t                 id;
  pami_dispatch_hint_t   options;
  char                 * name;
  
  pami_result_t          result;
  pami_client_t          client;
  pami_context_t         context;
  pami_task_t            task;
  volatile unsigned      recv_active;
  void                 * recv_buffer;
  size_t                 sndlen_min;
  size_t                 sndlen_max;
} dispatch_info_t;



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


/* --------------------------------------------------------------- */

static void decrement (pami_context_t   context,
                       void          * cookie,
                       pami_result_t    result)
{
  volatile unsigned * value = (volatile unsigned *) cookie;
  TRACE_ERR((stderr, "(%zu) decrement() cookie = %p, %d => %d\n", _my_task, cookie, *value, *value - 1));
  --*value;
}

/* --------------------------------------------------------------- */
static void test_dispatch (
  pami_context_t        context,      /**< IN: PAMI context */
  void               * cookie,       /**< IN: dispatch cookie */
  const void         * header_addr,  /**< IN: header address */
  size_t               header_size,  /**< IN: header size */
  const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
  size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t      origin,
  pami_recv_t         * recv)        /**< OUT: receive message structure */
{
  dispatch_info_t * dispatch = (dispatch_info_t *) cookie;
  
  if (recv)
    {
      TRACE_ERR((stderr, "(%zu) test_dispatch() async recv:  cookie = %p, pipe_size = %zu\n", dispatch->task, cookie, pipe_size));
      recv->local_fn    = decrement;
      recv->cookie      = (void *) & dispatch->recv_active;
      recv->type        = PAMI_TYPE_BYTE;
      recv->addr        = dispatch->recv_buffer;
      recv->offset      = 0;
      recv->data_fn     = PAMI_DATA_COPY;
      recv->data_cookie = NULL;
    }
  else
    {
      TRACE_ERR((stderr, "(%zu) test_dispatch() short recv:  decrement: %d => %d\n", dispatch->task, dispatch->recv_active, dispatch->recv_active - 1));
      --(dispatch->recv_active);
    }
}

unsigned long long test_single (dispatch_info_t * dispatch, size_t hdrsize, size_t sndlen)
{
  pami_task_t task = PAMIX_Client_task (dispatch->client);
  size_t      size = PAMIX_Client_size (dispatch->client);

  /* Use task 0 to last task (arbitrary) */
  pami_task_t origin_task = 0;
  pami_task_t target_task = size - 1;

  pami_endpoint_t origin, target;
  PAMI_Endpoint_create (dispatch->client, origin_task, 0, &origin);
  PAMI_Endpoint_create (dispatch->client, target_task, 0, &target);
  
  TRACE_ERR((stderr, "(%u) Do test ... sndlen = %zu\n", test, sndlen));

  char metadata[BUFSIZE];
  char buffer[BUFSIZE];

  volatile unsigned send_active = 1;

  pami_send_t parameters;
  parameters.send.dispatch        = dispatch->id;
  parameters.send.header.iov_base = metadata;
  parameters.send.header.iov_len  = hdrsize;
  parameters.send.data.iov_base   = buffer;
  parameters.send.data.iov_len    = sndlen;
  parameters.events.cookie        = (void *) & send_active;
  parameters.events.local_fn      = decrement;
  parameters.events.remote_fn     = NULL;
  memset(&parameters.send.hints, 0, sizeof(parameters.send.hints));

  unsigned i;
  pami_context_t context = dispatch->context;
  unsigned long long t1 = PAMI_Wtimebase(dispatch->client);


  if (task == origin_task)
    {
      parameters.send.dest = target;

      for (i = 0; i < ITERATIONS; i++)
        {
          TRACE_ERR((stderr, "(%u) Starting Iteration %d of size %zu\n", task, i, sndlen));
          
          send_active = 1;
          PAMI_Send (context, &parameters);
          TRACE_ERR((stderr, "(%zu) send_once() Before advance\n", task));
          while (send_active) PAMI_Context_advance (context, 100);
          TRACE_ERR((stderr, "(%zu) send_once()  After advance\n", task));

          TRACE_ERR((stderr, "(%zu) recv_once() Before advance\n", task));
          while (dispatch->recv_active) PAMI_Context_advance (context, 100);
          dispatch->recv_active = 1;
          TRACE_ERR((stderr, "(%zu) recv_once()  After advance\n", task));
        }
    }
  else if (task == target_task)
    {
      parameters.send.dest = origin;

      for (i = 0; i < ITERATIONS; i++)
        {
          TRACE_ERR((stderr, "(%u) Starting Iteration %d of size %zu\n", task, i, sndlen));
          
          TRACE_ERR((stderr, "(%zu) recv_once() Before advance\n", task));
          while (dispatch->recv_active) PAMI_Context_advance (context, 100);
          dispatch->recv_active = 1;
          TRACE_ERR((stderr, "(%zu) recv_once()  After advance\n", task));

          send_active = 1;
          PAMI_Send (context, &parameters);
          TRACE_ERR((stderr, "(%zu) send_once() Before advance\n", task));
          while (send_active) PAMI_Context_advance (context, 100);
          TRACE_ERR((stderr, "(%zu) send_once()  After advance\n", task));
        }
    }

  unsigned long long t2 = PAMI_Wtimebase(dispatch->client);

  return ((t2 - t1) / ITERATIONS) / 2;
}

pami_client_t initialize (dispatch_info_t dispatch[], size_t n, unsigned test_using_send)
{
  TRACE_ERR((stderr, "Initialize test.\n"));

  char clientname[] = "TEST";
  TRACE_ERR((stderr, "... before PAMI_Client_create()\n"));
  pami_client_t client;
  PAMI_Client_create (clientname, &client, NULL, 0);
  TRACE_ERR((stderr, "...  after PAMI_Client_create()\n"));
  TRACE_ERR((stderr, "... before PAMI_Context_createv()\n"));
  pami_context_t context;
  PAMI_Context_createv (client, NULL, 0, &context, 1);
  TRACE_ERR((stderr, "...  after PAMI_Context_createv()\n"));

  pami_task_t task = PAMIX_Client_task (client);
  size_t      size = PAMIX_Client_size (client);

  pami_dispatch_callback_function fn;
  fn.p2p = test_dispatch;

  size_t i;

  for (i = 0; i < n; i++)
    {
      dispatch[i].recv_active = 1;
      dispatch[i].client = client;
      dispatch[i].context = context;
      dispatch[i].task = task;
      dispatch[i].recv_buffer = (void *) malloc (sizeof(uint8_t) * BUFSIZE);
      dispatch[i].sndlen_min = 0;
      dispatch[i].sndlen_max = BUFSIZE;
      dispatch[i].result = PAMI_Dispatch_set (dispatch[i].context,
                                              dispatch[i].id,
                                              fn,
                                              (void *) & dispatch[i],
                                              dispatch[i].options);

    }

  if (test_using_send)
  {
  /* Use task 0 to last task (arbitrary) */
  pami_task_t origin_task = 0;
  pami_task_t target_task = size - 1;

  pami_endpoint_t origin, target;
  PAMI_Endpoint_create (client, origin_task, 0, &origin);
  PAMI_Endpoint_create (client, target_task, 0, &target);

  for (i = 0; i < n; i++)
    {
      if ((dispatch[i].result == PAMI_SUCCESS) &&
          (task == origin_task || task == target_task))
        {

          volatile unsigned pretest_active = 1;

          pami_send_t parameters;
          parameters.send.dispatch        = dispatch[i].id;
          parameters.send.dest            = (task == origin_task) ? target : origin;
          parameters.send.header.iov_base = (void *) & parameters;
          parameters.send.header.iov_len  = 8;
          parameters.send.data.iov_base   = (void *) & parameters;
          parameters.send.data.iov_len    = sizeof(pami_send_t);
          parameters.events.cookie        = (void *) & pretest_active;
          parameters.events.local_fn      = decrement;
          parameters.events.remote_fn     = NULL;
          memset(&parameters.send.hints, 0, sizeof(parameters.send.hints));

          dispatch[i].result = PAMI_Send (dispatch[i].context, &parameters);

          if (dispatch[i].result == PAMI_SUCCESS)
            {
              dispatch[i].recv_active++;

              while (pretest_active) PAMI_Context_advance (dispatch[i].context, 100);
            }
        }
    }
  }
  
  return client;
}

void test (dispatch_info_t dispatch[], size_t n, size_t header_size[], size_t header_count)
{
  pami_task_t task = PAMIX_Client_task (dispatch[0].client);
  size_t      size = PAMIX_Client_size (dispatch[0].client);

  /* Use task 0 to last task (arbitrary) */
  pami_task_t origin_task = 0;
  pami_task_t target_task = size - 1;

  pami_endpoint_t origin, target;
  PAMI_Endpoint_create (dispatch[0].client, origin_task, 0, &origin);
  PAMI_Endpoint_create (dispatch[0].client, target_task, 0, &target);
  
  /* Display some test header information */
  if (task == origin_task)
    {
      char str[3][1024];
      int index[3];
      index[0] = 0;
      index[1] = 0;
      index[2] = 0;

      index[0] += sprintf (&str[0][index[0]], "#          ");
      index[1] += sprintf (&str[1][index[1]], "#          ");
      index[2] += sprintf (&str[2][index[2]], "#    bytes ");

      fprintf (stdout, "# PAMI_Send() half-pingpong blocking latency performance test. 'ping' task = %d, 'pong' task = %d\n", origin_task, target_task);
      fprintf (stdout, "#\n");
      unsigned h, i;

      for (h = 0; h < header_count; h++)
        {
          fprintf (stdout, "# testcase %d : header bytes = %3zd\n", h, header_size[h]);

          if (n == 1)
          {
            index[0] += sprintf (&str[0][index[0]], "[-- testcase %d --] ", h);
          }
          else
          {
            size_t num_dashes = (n-1)*18;
            
            index[0] += sprintf (&str[0][index[0]], "[");
            for (i=0; i<(num_dashes/2); i++)
              index[0] += sprintf (&str[0][index[0]], "-");
            
            index[0] += sprintf (&str[0][index[0]], "--- testcase %d ---", h);
            
            for (i=0; i<(num_dashes/2); i++)
              index[0] += sprintf (&str[0][index[0]], "-");
            index[0] += sprintf (&str[0][index[0]], "] ");
          }
          
          for (i=0; i<n; i++)
          {
            index[1] += sprintf (&str[1][index[1]], "[%16.16s] ", dispatch[i].name);
            index[2] += sprintf (&str[2][index[2]], "  cycles     usec  ");
          }
        }

      fprintf (stdout, "#\n");
      fprintf (stdout, "%s\n", str[0]);
      fprintf (stdout, "%s\n", str[1]);
      fprintf (stdout, "%s\n", str[2]);
      fflush (stdout);
    }

  pami_configuration_t configuration;
  configuration.name = PAMI_CLIENT_WTICK;
  pami_result_t result = PAMI_ERROR;
  result = PAMI_Client_query(dispatch[0].client, &configuration, 1);
  double tick = configuration.value.doubleval;

  unsigned long long cycles;
  double usec;

  char str[10240];


  size_t sndlen = 0;

  for (; sndlen < BUFSIZE; sndlen = sndlen * 3 / 2 + 1)
    {
      int index = 0;
      index += sprintf (&str[index], "%10zd ", sndlen);

      unsigned i;

      for (i = 0; i < header_count; i++)
        {
          unsigned j;

          for (j = 0; j < n; j++)
            {
              if ((dispatch[j].result == PAMI_SUCCESS) &&
                  (sndlen <= dispatch[j].sndlen_max) &&
                  (sndlen >= dispatch[j].sndlen_min))
                {
#ifdef WARMUP
                  test_single (&dispatch[j], header_size[i], sndlen);
#endif
                  cycles = test_single (&dispatch[j], header_size[i], sndlen);
                  usec   = cycles * tick * 1000000.0;
                  index += sprintf (&str[index], "%8lld %8.4f  ", cycles, usec);
                }
              else
                {
                  index += sprintf (&str[index], "    ----     ----  ");
                }
            }
        }

      if (task == origin_task)
        fprintf (stdout, "%s\n", str);
    }
}

#undef TRACE_ERR
#endif /* __test_api_p2p_send_hint_send_perf_h__ */
