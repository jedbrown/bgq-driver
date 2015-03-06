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
 * \file test/api/p2p/sendimmediate_latency.c
 * \brief ???
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include "../init_util.h"

#define ITERATIONS 100000

#define WARMUP

#ifndef BUFSIZE
#define BUFSIZE 256
#endif

#ifndef MAX_BUFSIZE
#define MAX_BUFSIZE 1024*16
#endif

#define RC(statement) \
{ \
    int rc = statement; \
    if (rc != PAMI_SUCCESS) { \
        printf(#statement " rc = %d, line %d\n", rc, __LINE__); \
        exit(-1); \
    } \
}

#undef TRACE_ERR
#ifndef TRACE_ERR
#define TRACE_ERR(x) /* fprintf x */
#endif


volatile unsigned _send_active;
volatile unsigned _recv_active;
volatile unsigned _recv_iteration;
char              _recv_buffer[MAX_BUFSIZE] __attribute__ ((__aligned__(16)));

pami_task_t _my_task;

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
  RC( PAMI_Send_immediate (context, parameters) );
  TRACE_ERR((stderr, "(%zu) after send_immediate()  \n", _my_task));
}

void recv_once (pami_context_t context)
{
  TRACE_ERR((stderr, "(%zu) recv_once() Before advance\n", _my_task));

  while (_recv_active) PAMI_Context_advance (context, 100);

  _recv_active = 1;
  TRACE_ERR((stderr, "(%zu) recv_once()  After advance\n", _my_task));
}

unsigned long long test (pami_client_t client, pami_context_t context,
                         size_t dispatch, size_t hdrlen, size_t sndlen,
                         pami_task_t mytask, pami_endpoint_t origin, pami_endpoint_t target)
{
  TRACE_ERR((stderr, "(%zu) Do test ... sndlen = %zu\n", _my_task, sndlen));
  _recv_active = 1;
  _recv_iteration = 0;
  _send_active = 1;

  pami_task_t origin_task = (pami_task_t) -1;
  size_t origin_offset    = (size_t) -1;
  RC( PAMI_Endpoint_query (origin, &origin_task, &origin_offset) );

  pami_task_t target_task = (pami_task_t) -1;
  size_t target_offset    = (size_t) -1;
  RC( PAMI_Endpoint_query (target, &target_task, &target_offset) );

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
  unsigned long long t1 = PAMI_Wtimebase(client);

  if (mytask == origin_task)
    {
      parameters.dest = target;

      for (i = 0; i < ITERATIONS; i++)
        {
          TRACE_ERR((stderr, "(%zu) Starting Iteration %d of size %zu\n", _my_task, i, sndlen));
          send_once (context, &parameters);
          _send_active = 1;
          recv_once (context);
          _recv_active = 1;
        }
    }
  else if (mytask == target_task)
    {
      parameters.dest = origin;

      for (i = 0; i < ITERATIONS; i++)
        {
          TRACE_ERR((stderr, "(%zu) Starting Iteration %d of size %zu\n", _my_task, i, sndlen));
          recv_once (context);
          _recv_active = 1;
          send_once (context, &parameters);
          _send_active = 1;
        }
    }

  unsigned long long t2 = PAMI_Wtimebase(client);

  return ((t2 - t1) / ITERATIONS) / 2;
}

int main (int argc, char ** argv)
{
  TRACE_ERR((stderr, "Start test ...\n"));
  size_t hdrcnt = argc;
  size_t hdrsize[1024];
  hdrsize[0] = 0;

  int arg;

  for (arg = 1; arg < argc; arg++)
    {
      hdrsize[arg] = (size_t) strtol (argv[arg], NULL, 10);
    }

  pami_client_t         client;
  pami_context_t        context;
  size_t                num_contexts = 1;
  size_t                num_tasks;

  RC( pami_init (&client,        /* Client             */
                 &context,       /* Context            */
                 NULL,           /* Clientname=default */
                 &num_contexts,  /* num_contexts       */
                 NULL,           /* null configuration */
                 0,              /* no configuration   */
                 &_my_task,      /* task id            */
                 &num_tasks) );  /* number of tasks    */
  
  pami_configuration_t configuration;

  configuration.name = PAMI_CLIENT_NUM_LOCAL_TASKS;
  RC( PAMI_Client_query(client, &configuration, 1) );
  size_t num_local_tasks = configuration.value.intval;

  int num_dispatch = (num_tasks == num_local_tasks) ? 3 : 2;
  if (_my_task == 0)
      printf("%u dispatches will be tested (%zu tasks:%zu local tasks)\n",
              num_dispatch,
              num_tasks,
              num_local_tasks);

  configuration.name = PAMI_CLIENT_WTICK;
  RC( PAMI_Client_query(client, &configuration, 1) );
  double tick = configuration.value.doubleval;

  /* Use task 0 to last task (arbitrary) */
  pami_task_t origin_task = 0;
  pami_task_t target_task = num_tasks - 1;

  pami_endpoint_t origin, target;
  RC( PAMI_Endpoint_create (client, origin_task, 0, &origin) );
  RC( PAMI_Endpoint_create (client, target_task, 0, &target) );

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
  dispatch[0].send_immediate_max = 0;

  dispatch[1].id = 11;
  dispatch[1].options = (pami_dispatch_hint_t) {0};
  dispatch[1].name = " no shmem ";
  dispatch[1].options.use_shmem = PAMI_HINT_DISABLE;

  dispatch[2].id = 12;
  dispatch[2].options = (pami_dispatch_hint_t) {0};
  dispatch[2].name = "only shmem";
  dispatch[2].options.use_shmem = PAMI_HINT_ENABLE;

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

  sleep(1); /* work-around for ticket #385 */

  for (i = 0; i < num_dispatch; i++)
    {
      if ((dispatch[i].result == PAMI_SUCCESS) &&
          (_my_task == origin_task || _my_task == target_task))
        {
          configuration.name = PAMI_DISPATCH_SEND_IMMEDIATE_MAX;
          pami_result_t result = PAMI_Dispatch_query(context, dispatch[i].id, &configuration, 1);

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

/*fprintf (stderr, "_recv_active = %d, & _recv_active = %p >>\n", _recv_active, &_recv_active);
 *_recv_active = 1;
*/
  while (_recv_active) PAMI_Context_advance (context, 100);

/*fprintf (stderr, "_recv_active = %d, & _recv_active = %p <<\n", _recv_active, &_recv_active);*/

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

      fprintf (stdout, "# PAMI_Send_immediate() task %d -> task %d half-pingpong blocking latency performance test\n", origin_task, target_task);
      fprintf (stdout, "#\n");
      unsigned i;

      for (i = 0; i < hdrcnt; i++)
        {
          if (i == 0)
            fprintf (stdout, "# testcase %d : header bytes = %3zd\n", i, hdrsize[i]);
          else
            fprintf (stdout, "# testcase %d : header bytes = %3zd (argv[%d])\n", i, hdrsize[i], i);

          index[0] += sprintf (&str[0][index[0]], "[-- testcase %d --] [-- testcase %d --] [-- testcase %d --] ", i, i, i);
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

          for (j = 0; j < num_dispatch; j++)
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

  unsigned long long t1 = PAMI_Wtimebase(client);
  for(i=0; i<ITERATIONS; i++)
    PAMI_Context_advance (context, 1);
  if (_my_task == origin_task)
  {
    unsigned long long cycles = PAMI_Wtimebase(client) - t1;
    cycles = cycles/ITERATIONS;
    double             usec   = cycles*tick*1000000.0;
    fprintf (stdout, "# Empty Advance Time:  %8lld %8.4f\n", cycles, usec);    
  }
  RC( pami_shutdown(&client, &context, &num_contexts) );

  return 0;
}
#undef TRACE_ERR
