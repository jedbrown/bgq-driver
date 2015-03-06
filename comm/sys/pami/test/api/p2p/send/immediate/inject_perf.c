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
 * \file test/api/p2p/send/immediate/inject_perf.c
 * \brief Performance test to measure the latency of send immediate injection
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#include <pami.h>

#define ITERATIONS 10000

#define SEND_IMMEDIATE_EAGAIN

#define WARMUP

#ifndef BUFSIZE
#define BUFSIZE 256
#endif

#ifndef MAX_BUFSIZE
#define MAX_BUFSIZE 1024*16
#endif


#undef TRACE_ERR
#define TRACE_ERR(x) /*fprintf x*/

volatile unsigned _send_active;
volatile unsigned _recv_active;
volatile unsigned _recv_iteration;
char              _recv_buffer[MAX_BUFSIZE] __attribute__ ((__aligned__(16)));

size_t         _dispatch[100];
unsigned       _dispatch_count;

size_t _my_task;

typedef struct
{
  size_t sndlen;
} header_t;

/* --------------------------------------------------------------- */

static void decrement (pami_context_t   context,
                       void          * cookie,
                       pami_result_t    result)
{
  unsigned * value = (unsigned *) cookie;
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
  pami_endpoint_t origin,
  pami_recv_t         * recv)        /**< OUT: receive message structure */
{
  unsigned * value = (unsigned *) cookie;
  TRACE_ERR((stderr, "(%zu) short recv:  decrement cookie = %p, %d => %d\n", _my_task, cookie, *value, *value - 1));
  --*value;
  _recv_iteration++;
}

void send_once (pami_context_t context, pami_send_immediate_t * parameters)
{
  TRACE_ERR((stderr, "(%zu) before send_immediate()  \n", _my_task));

  pami_result_t result = PAMI_ERROR;
  result = PAMI_Send_immediate (context, parameters);

#ifdef SEND_IMMEDIATE_EAGAIN

  while (result == PAMI_EAGAIN)
    {
      PAMI_Context_advance (context, 1);
      result = PAMI_Send_immediate (context, parameters);
    };

#endif

  TRACE_ERR((stderr, "(%zu) after send_immediate()  \n", _my_task));
}

void recv_many (pami_context_t context)
{
  TRACE_ERR((stderr, "(%zu) recv_once() Before advance\n", _my_task));

  while (_recv_active > 0) PAMI_Context_advance (context, 100);

  _recv_active = 1;
  TRACE_ERR((stderr, "(%zu) recv_once()  After advance\n", _my_task));
}

unsigned long long test (pami_client_t client, pami_context_t context, size_t dispatch, size_t hdrlen, size_t sndlen, pami_task_t myrank, pami_endpoint_t origin, pami_endpoint_t target)
{
  TRACE_ERR((stderr, "(%zu) Do test ... sndlen = %zu\n", _my_task, sndlen));
  _recv_active = ITERATIONS;
  _recv_iteration = 0;
  _send_active = 1;

  char metadata[MAX_BUFSIZE];
  char buffer[MAX_BUFSIZE];

  header_t header;
  header.sndlen = sndlen;

  pami_send_immediate_t parameters;
  parameters.dispatch = dispatch;
  parameters.header.iov_base = metadata;
  parameters.header.iov_len = hdrlen;
  parameters.data.iov_base  = buffer;
  parameters.data.iov_len = sndlen;

  unsigned i;
  unsigned long long t1 = 0;
  unsigned long long t2 = 0;

  pami_task_t origin_task;
  size_t origin_offset;
  PAMI_Endpoint_query (origin, &origin_task, &origin_offset);

  pami_task_t target_task;
  size_t target_offset;
  PAMI_Endpoint_query (target, &target_task, &target_offset);

  if (myrank == origin_task)
    {
      parameters.dest = target;

      t1 = PAMI_Wtimebase(client);

      for (i = 0; i < ITERATIONS; i++)
        {
          send_once (context, &parameters);
          _send_active = 1;
        }

      t2 = PAMI_Wtimebase(client);

      recv_many (context);
      _recv_active = ITERATIONS;
    }
  else if (myrank == target_task)
    {
      parameters.dest = origin;

      recv_many (context);
      _recv_active = ITERATIONS;

      for (i = 0; i < ITERATIONS; i++)
        {
          send_once (context, &parameters);
          _send_active = 1;
        }
    }

  return ((t2 - t1) / ITERATIONS);
}

int main (int argc, char ** argv)
{
  TRACE_ERR((stderr, "Start test ...\n"));
  size_t hdrcnt = argc + 1;
  size_t hdrsize[1024];
  hdrsize[0] = 0;
  hdrsize[1] = 16; /* MPI msginfo size */

  int arg;

  for (arg = 1; arg < argc; arg++)
    {
      hdrsize[arg+1] = (size_t) strtol (argv[arg], NULL, 10);
    }

  pami_client_t client;
  char clientname[] = "PAMI";
  TRACE_ERR((stderr, "... before PAMI_Client_create()\n"));
  PAMI_Client_create (clientname, &client, NULL, 0);
  TRACE_ERR((stderr, "...  after PAMI_Client_create()\n"));
  pami_context_t context;
  TRACE_ERR((stderr, "... before PAMI_Context_createv()\n"));
  { size_t _n = 1; PAMI_Context_createv (client, NULL, 0, &context, _n); }
  TRACE_ERR((stderr, "...  after PAMI_Context_createv()\n"));


  /* Register the protocols to test */
  _dispatch_count = 0;

  pami_configuration_t configuration;
  pami_result_t result;

  configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query(client, &configuration, 1);
  _my_task = configuration.value.intval;

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration, 1);
  size_t num_tasks = configuration.value.intval;

  usleep(100*_my_task);
  /* fprintf (stdout, "# task %zu of %zu\n", _my_task, num_tasks); */
  usleep(100*(num_tasks - _my_task + 1) + 1000);

  configuration.name = PAMI_CLIENT_WTICK;
  result = PAMI_Client_query(client, &configuration, 1);
  double tick = configuration.value.doubleval;

  /* Use task 0 to last task (arbitrary) */
  pami_task_t origin_task = 0;
  pami_task_t target_task = num_tasks - 1;

  pami_endpoint_t origin, target;
  PAMI_Endpoint_create (client, origin_task, 0, &origin);
  PAMI_Endpoint_create (client, target_task, 0, &target);

  typedef struct
  {
    size_t                 id;
    pami_dispatch_hint_t   options;
    char                 * name;
    pami_result_t          result;
    size_t                 send_immediate_max;
  } dispatch_info_t;

  dispatch_info_t dispatch[3];

  dispatch[0].id = 10;
  dispatch[0].options = (pami_dispatch_hint_t) {0};
  dispatch[0].name = "  default ";
#ifdef SEND_IMMEDIATE_EAGAIN
  dispatch[0].options.queue_immediate = PAMI_HINT_DISABLE;
#endif
  dispatch[0].send_immediate_max = 0;

  dispatch[1].id = 11;
  dispatch[1].options = (pami_dispatch_hint_t) {0};
  dispatch[1].name = "only shmem";
  dispatch[1].options.use_shmem = PAMI_HINT_ENABLE;
#ifdef SEND_IMMEDIATE_EAGAIN
  dispatch[1].options.queue_immediate = PAMI_HINT_DISABLE;
#endif
  dispatch[1].send_immediate_max = 0;

  dispatch[2].id = 12;
  dispatch[2].options = (pami_dispatch_hint_t) {0};
  dispatch[2].name = " no shmem ";
  dispatch[2].options.use_shmem = PAMI_HINT_DISABLE;
#ifdef SEND_IMMEDIATE_EAGAIN
  dispatch[2].options.queue_immediate = PAMI_HINT_DISABLE;
#endif
  dispatch[2].send_immediate_max = 0;

  pami_dispatch_callback_function fn;
  fn.p2p = test_dispatch;
  TRACE_ERR((stderr, "Before PAMI_Dispatch_set() .. &_recv_active = %p, recv_active = %u\n", &_recv_active, _recv_active));

  size_t i;

  for (i = 0; i < 3; i++)
    {
      dispatch[i].result = PAMI_Dispatch_set (context,
                                              dispatch[i].id,
                                              fn,
                                              (void *) & _recv_active,
                                              dispatch[i].options);

    }

  for (i = 0; i < 3; i++)
    {
      if ((dispatch[i].result == PAMI_SUCCESS) &&
          (_my_task == origin_task || _my_task == target_task))
        {
          configuration.name = PAMI_DISPATCH_SEND_IMMEDIATE_MAX;
          result = PAMI_Dispatch_query(context, dispatch[i].id, &configuration, 1);

          if (result == PAMI_SUCCESS)
            {
              dispatch[i].send_immediate_max = configuration.value.intval;
            }
          else
            {
              fprintf (stderr, "Warning. Unable to query PAMI_DISPATCH_SEND_IMMEDIATE_MAX, using test default value %zu. result = %d\n", dispatch[i].send_immediate_max, result);
            }

          volatile unsigned pretest_active = 1;

          pami_send_t parameters;
          parameters.send.dispatch        = dispatch[i].id;
          parameters.send.dest            = (_my_task == origin_task) ? target : origin;
          parameters.send.header.iov_base = (void *) & parameters;
          parameters.send.header.iov_len  = 8;
          parameters.send.data.iov_base   = (void *) & parameters;
          parameters.send.data.iov_len    = sizeof(pami_send_t);
          parameters.events.cookie        = (void *) & pretest_active;
          parameters.events.local_fn      = decrement;
          parameters.events.remote_fn     = NULL;
          memset(&parameters.send.hints, 0, sizeof(parameters.send.hints));

          dispatch[i].result = PAMI_Send (context, &parameters);

          if (dispatch[i].result == PAMI_SUCCESS)
            {
              _recv_active++;
              _recv_iteration--;

              while (pretest_active) PAMI_Context_advance (context, 100);
            }
        }
    }

  while (_recv_active) PAMI_Context_advance (context, 100);

  /* Display some test header information */
  if (_my_task == origin_task)
    {
      char str[3][1024];
      int index[3];
      index[0] = 0;
      index[1] = 0;
      index[2] = 0;

      index[0] += sprintf (&str[0][index[0]], "#          ");
      index[1] += sprintf (&str[1][index[1]], "#          ");
      index[2] += sprintf (&str[2][index[2]], "#    bytes ");

      fprintf (stdout, "#\n");
      fprintf (stdout, "# PAMI_Send_immediate() task %d -> task %d simple injection performance test\n", origin_task, target_task);
      fprintf (stdout, "#\n");
      unsigned i;

      for (i = 0; i < hdrcnt; i++)
        {
          if (i <= 1)
            fprintf (stdout, "# testcase %d : header bytes = %3zd\n", i, hdrsize[i]);
          else
            fprintf (stdout, "# testcase %d : header bytes = %3zd (argv[%d])\n", i, hdrsize[i], i - 1);

          index[0] += sprintf (&str[0][index[0]], "[--------------------- testcase %d ---------------------] ", i);
          index[1] += sprintf (&str[1][index[1]], "[-- %s --] [-- %s --] [-- %s --] ", dispatch[0].name, dispatch[1].name, dispatch[2].name);
          index[2] += sprintf (&str[2][index[2]], "  cycles     usec    cycles     usec    cycles     usec  ");
        }

      fprintf (stdout, "#\n");
      fprintf (stdout, "%s\n", str[0]);
      fprintf (stdout, "%s\n", str[1]);
      fprintf (stdout, "%s\n", str[2]);
      fflush (stdout);
    }

  for (i = 0; i < 10000000; i++) {};

  char str[10240];

  size_t j, max_send_immediate_max = 0;

  for (j = 0; j < 3; j++)
    {
      if (dispatch[j].send_immediate_max > max_send_immediate_max)
        max_send_immediate_max = dispatch[j].send_immediate_max;
    }


  size_t sndlen;

  for (sndlen = 0; sndlen < max_send_immediate_max; sndlen = sndlen * 3 / 2 + 1)
    {
      int index = 0;
      index += sprintf (&str[index], "%10zd ", sndlen);

      unsigned i = 0;

      for (i = 0; i < hdrcnt; i++)
        {
          unsigned j;

          for (j = 0; j < 3; j++)
            {
              if (dispatch[j].result == PAMI_SUCCESS &&
                  sndlen <= dispatch[j].send_immediate_max)
                {
#ifdef WARMUP
                  test (client, context,  dispatch[j].id, hdrsize[i], sndlen, _my_task, origin, target);
#endif
                  unsigned long long cycles = test (client, context,  dispatch[j].id, hdrsize[i], sndlen, _my_task, origin, target);
                  double usec   = cycles * tick * 1000000.0;
                  index += sprintf (&str[index], "%8lld %8.4f  ", cycles, usec);
                }
              else
                {
                  index += sprintf (&str[index], "    ----     ----  ");
                }
            }
        }

      if (_my_task == origin_task)
        fprintf (stdout, "%s\n", str);
    }

  PAMI_Client_destroy(&client);

  sleep (1);
  return 0;
}
#undef TRACE_ERR
