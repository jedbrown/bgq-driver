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
 * \file test/api/p2p/send/send_to_self_perf/send_to_self_perf.c
 * \brief Test PAMI_Send(), sending via loopback to ourself.
 *
 * The test starts with message size 0, and increases it up to the
 * BUFSIZE, printing out cycles and microseconds.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <pami.h>
/*#define ITERATIONS 2*/
#define ITERATIONS 1000
/*#define ITERATIONS 100 */

#ifndef BUFSIZE
/*#define BUFSIZE 2048 */
/*#define BUFSIZE 1024*256 */
/*#define BUFSIZE 16 */
#define BUFSIZE 1024
#endif

#define TRACE_ERR(x) /*fprintf x*/

size_t _my_rank;

uint8_t _rbuf[BUFSIZE];
uint8_t _sbuf[BUFSIZE];
pami_client_t  _g_client;
pami_context_t _g_context;

volatile size_t _send_active;
volatile size_t _recv_active;

size_t dispatch = 0;

char testcase_str[10240];

unsigned long long T_RECV_DONE, T_DISPATCH, T_SEND_DONE_LOCAL, T_SEND_DONE_REMOTE, T_SEND_START, T_ADVANCE_DONE;

/* /\* --------------------------------------------------------------- *\/ */

/* static void decrement (void * clientdata, PAMI_Error_t * err) */
/* { */
/*   unsigned* _clientdata = (unsigned *)clientdata; */
/*   TRACE_ERR((stderr, "(%zu) decrement() clientdata = %p, %d => %d\n", _my_rank, _clientdata, *_clientdata, *_clientdata-1)); */
/*   --*_clientdata; */
/*   if (*_clientdata < 0) exit(-1); */
/* } */

/* /\* --------------------------------------------------------------- *\/ */

/* static void cb_recv_new_short (void           * clientdata, */
/*                                const PAMIQuad   * msginfo, */
/*                                unsigned         count, */
/*                                size_t           peer, */
/*                                const char     * src, */
/*                                size_t           bytes) */
/* { */
/*   unsigned* _clientdata = (unsigned *)clientdata; */
/*   if (*_clientdata <= 0) abort(); */
/*   TRACE_ERR((stderr, "(%zu) cb_recv_new_short() clientdata = %p, %d => %d\n", _my_rank, _clientdata, *_clientdata, *_clientdata-1)); */
/*   memcpy(_rbuf, src, bytes); */
/*   --*_clientdata; */
/* } */

/* /\* --------------------------------------------------------------- *\/ */

/* static PAMI_Request_t * cb_recv_new (void                 * clientdata, */
/*                                      const PAMIQuad         * msginfo, */
/*                                      unsigned               count, */
/*                                      size_t                 senderID, */
/*                                      size_t                 sndlen, */
/*                                      size_t               * rcvlen, */
/*                                      char                ** rcvbuf, */
/*                                      PAMI_Callback_t      * cb_info) */
/* { */
/*   TRACE_ERR((stderr, "(%zu) cb_recv_new() clientdata = %p, sndlen = %zu\n", _my_rank, clientdata, sndlen)); */
/*   * rcvbuf = (char *) _rbuf; */
/*   * rcvlen = sndlen>0?sndlen:1; */
/*   cb_info->function   = decrement; */
/*   cb_info->clientdata = clientdata; */

/*   return &_recv_request; */
/* } */

static void recv_done (pami_context_t   context,
                       void          * cookie,
                       pami_result_t    result)
{
  T_RECV_DONE = PAMI_Wtimebase(_g_client);
  volatile size_t * active = (volatile size_t *) cookie;
  TRACE_ERR((stderr, "Called recv_done function.  cookie=%p, active: %zu -> %zu\n", cookie, *active, *active-1));
  (*active)--;
}

static void test_dispatch (
    pami_context_t       context,      /**< IN: PAMI context */
    void               * cookie,       /**< IN: dispatch cookie */
    const void         * header_addr,  /**< IN: header address */
    size_t               header_size,  /**< IN: header size */
    const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
    size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
    pami_endpoint_t      origin,       /**< IN:  Endpoint that originated the transfer */
    pami_recv_t        * recv)         /**< OUT: receive message structure */
{
  T_DISPATCH = PAMI_Wtimebase(_g_client);
  TRACE_ERR((stderr, "Called dispatch function.  cookie = %p, active: %zu, header_addr = %p, pipe_addr = %p\n", cookie,  *((volatile size_t *) cookie), header_addr, pipe_addr));

  if (recv == NULL)
  {
    /* This is an 'immediate' receive */

    memcpy(_rbuf, pipe_addr, pipe_size);
    recv_done (context, cookie, PAMI_SUCCESS);
  }
  else
  {
    /* This is an 'asynchronous' receive */

    recv->local_fn = recv_done;
    recv->cookie   = cookie;
    recv->type     = PAMI_TYPE_BYTE;
    recv->addr     = _rbuf;
    recv->offset   = 0;
    recv->data_fn  = PAMI_DATA_COPY;
    recv->data_cookie = NULL;
    TRACE_ERR((stderr, "... dispatch function.  recv->local_fn = %p\n", recv->local_fn));
  }

  return;
}

static void send_done_local (pami_context_t   context,
                             void          * cookie,
                             pami_result_t    result)
{
  T_SEND_DONE_LOCAL = PAMI_Wtimebase(_g_client);
  volatile size_t * active = (volatile size_t *) cookie;
  TRACE_ERR((stderr, "Called send_done_local function.  cookie=%p, active: %zu -> %zu\n", cookie, *active, *active-1));
  (*active)--;
}

#ifdef TEST_REMOTE_COMPLETION
static void send_done_remote (pami_context_t   context,
                              void          * cookie,
                              pami_result_t    result)
{
  T_SEND_DONE_REMOTE = PAMI_Wtimebase(_g_client);
  volatile size_t * active = (volatile size_t *) cookie;
  TRACE_ERR((stderr, "Called send_done_remote function.  cookie=%p, active: %zu -> %zu\n", cookie, *active, *active-1));
  (*active)--;
}
#endif

unsigned long long test (size_t sndlen, size_t myrank)
{
  typedef size_t quad_t[4];

  quad_t msginfo;
  pami_result_t result = PAMI_ERROR;
  pami_send_t parameters;
  parameters.send.dispatch        = dispatch;
  parameters.send.header.iov_base = (void *)&msginfo; /* send *something* */
  parameters.send.header.iov_len  = sizeof(msginfo);
  parameters.send.data.iov_base   = (void *)_sbuf; /* send *something* */
  parameters.send.data.iov_len    = sndlen;
  parameters.events.cookie    = (void *) &_send_active;
  parameters.events.local_fn  = send_done_local;
#ifdef TEST_REMOTE_COMPLETION
  parameters.events.remote_fn = send_done_remote;
#else
  parameters.events.remote_fn = NULL;
#endif

  unsigned i;
  unsigned long long t1 = 0;
  /*Allow warmup of 1 iteration */
  for (i = 0; i <= ITERATIONS; i++)
  {
    PAMI_Endpoint_create (_g_client, myrank, 0, &parameters.send.dest);
    TRACE_ERR((stderr, "(%zu)\n(%zu) Starting Iteration %d of size %zu\n", _my_rank, _my_rank, i, sndlen));
    if (i == 1)
      t1 = PAMI_Wtimebase(_g_client);
#ifdef TEST_REMOTE_COMPLETION
    _send_active = 2;
#else
    _send_active = 1;
#endif
    _recv_active = 1;
    TRACE_ERR((stderr,"test():  Calling PAMI_Send\n"));

    T_SEND_START = PAMI_Wtimebase(_g_client);
    result = PAMI_Send (_g_context, &parameters);
    TRACE_ERR((stderr,"test():  Back from PAMI_Send\n"));

    while (_send_active || _recv_active)
      {
        TRACE_ERR((stderr,"test():  Calling Advance\n"));
        result = PAMI_Context_advance (_g_context, 100);
        TRACE_ERR((stderr,"test():  Back from Advance\n"));
      }
    T_ADVANCE_DONE = PAMI_Wtimebase(_g_client);
  }
  unsigned long long t2 = PAMI_Wtimebase(_g_client);

  return ((t2-t1)/ITERATIONS);
}

int main ()
{
  printf("Start test ...\n");

  char          cl_string[] = "TEST";
  pami_result_t  result = PAMI_ERROR;

  result = PAMI_Client_create (cl_string, &_g_client, NULL, 0);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to initialize client. result = %d\n", result);
    return 1;
  }

  { size_t _n = 1; result = PAMI_Context_createv(_g_client, NULL, 0, &_g_context, _n); }
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to create context. result = %d\n", result);
    return 1;
  }

  pami_configuration_t configuration;

  configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query (_g_client, &configuration,1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable query configuration task ID (%d). result = %d\n", configuration.name, result);
    return 1;
  }
  size_t task_id = configuration.value.intval;
  fprintf (stderr, "My task id = %zu\n", task_id);
  _my_rank = task_id;

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query (_g_client, &configuration,1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable query configuration NumTasks (%d). result = %d\n", configuration.name, result);
    return 1;
  }
  size_t num_tasks = configuration.value.intval;
  printf("(%zu) after Initializing client and context.  num_tasks=%zu\n", task_id, num_tasks);

  pami_dispatch_callback_function fn;
  fn.p2p = test_dispatch;
  pami_dispatch_hint_t options={};
  TRACE_ERR((stderr, "Before PAMI_Dispatch_set() .. &_recv_active = %p, _recv_active = %lu\n", &_recv_active, _recv_active));
  result = PAMI_Dispatch_set (_g_context,
                             dispatch,
                             fn,
                             (void *)&_recv_active,
                             options);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable register pami dispatch. result = %d\n", result);
    return 1;
  }

  double clockMHz = 1600.0;
  unsigned int _protocol_count = 1;

  if (_my_rank == 0)
  {
    char hdrstr[2][1024];
    int index[2];
    index[0] = 0;
    index[1] = 0;

    index[0] += sprintf (&hdrstr[0][index[0]], "#          ");
    index[1] += sprintf (&hdrstr[1][index[1]], "#    bytes ");

    unsigned i;
    for (i=0; i<_protocol_count; i++)
    {
      index[0] += sprintf (&hdrstr[0][index[0]], "[--- testcase %d ---] ", i);
      index[1] += sprintf (&hdrstr[1][index[1]], "    cycles     usec  ");
    }

    fprintf (stdout, "# PAMI_Send() send-to-self blocking latency performance test\n");
    fprintf (stdout, "#\n");
    fprintf (stdout, "%s", testcase_str);
    fprintf (stdout, "#\n");
    fprintf (stdout, "%s\n", hdrstr[0]);
    fprintf (stdout, "%s\n", hdrstr[1]);
    fflush (stdout);

    unsigned long long cycles;
    double usec;

    char str[10240];

    /* Init the send buffer. */
    unsigned j;
    for (j=0; j<BUFSIZE; j++)
      {
        _sbuf[j]=j*5+3;
      }

    size_t sndlen;
    for (sndlen = 0; sndlen < BUFSIZE; sndlen = sndlen*3/2+1)
    {
      int index = 0;
      index += sprintf (&str[index], "%10zd ", sndlen);

      unsigned i;
      for (i=0; i<_protocol_count; i++)
      {
        /* warmup */
        /*test (&_protocol[i], sndlen, _my_rank); */

        cycles = test (sndlen, _my_rank);
        usec   = cycles/clockMHz;

        /* Check the buffer. */
        unsigned j;
        for (j=0; j<sndlen; j++)
        {
          if (_sbuf[j] != _rbuf[j]) printf("Data Miscompare at size %zu, _sbuf[%u] = 0x%02x, _rbuf = 0x%02x\n",sndlen, j, _sbuf[j], _rbuf[j]);
        }

        TRACE_ERR((stderr, "sndlen=%zu, START=%llu, SEND_DONE_LOCAL=%llu, DISPATCH=%llu, RECV_DONE=%llu, SEND_DONE_REMOTE=%llu, ADVANCE_DONE=%llu\n",
                   sndlen, T_SEND_START,T_SEND_DONE_LOCAL,T_DISPATCH,T_RECV_DONE,T_SEND_DONE_REMOTE,T_ADVANCE_DONE));
        index += sprintf (&str[index], "%10lld %8.4f  ", cycles, usec);
      }

      fprintf (stdout, "%s\n", str);
    }
  }
  return 0;
}
