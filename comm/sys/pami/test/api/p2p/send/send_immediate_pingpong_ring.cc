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
 * \file test/api/p2p/send/send_immediate_pingpong_ring.cc
 * \brief ???
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#include <pami.h>

size_t ITERATIONS;
size_t WARMUP;
size_t START;
size_t END;

#define START_DISPATCH_ID 10

#undef TRACE_ERR
#ifndef TRACE_ERR
#define TRACE_ERR(x) // fprintf x
#endif



typedef struct
{
  size_t id;
  volatile size_t counter;
} info_t;

typedef struct
{
  info_t ping;
  info_t pong;
} dispatch_t;

#define BUFSIZE 10240
uint8_t         _recv_buffer[BUFSIZE] __attribute__ ((__aligned__(16)));

dispatch_t      _dispatch[100];
unsigned        _dispatch_count;

pami_task_t _task;
size_t      _size;



/* --------------------------------------------------------------- */
static void dispatch_ping (pami_context_t    context,
                           void            * cookie,
                           const void      * header,
                           size_t            hdrlen,
                           const void      * data,
                           size_t            sndlen,
                           pami_endpoint_t   origin,
                           pami_recv_t     * recv)
{
  TRACE_ERR((stderr, ">> dispatch_ping()\n"));
  /* Copy the data "somewhere" to simulate an application environment */
  memcpy (_recv_buffer, data, sndlen);

  size_t n = (size_t) cookie;

  /* Send a "pong" response */
  pami_send_immediate_t parameters;
  parameters.dispatch        = _dispatch[n].pong.id;
  parameters.header.iov_base = &sndlen;
  parameters.header.iov_len  = sizeof(size_t);
  parameters.data.iov_base   = NULL;
  parameters.data.iov_len    = 0;
  parameters.dest            = origin;

  PAMI_Send_immediate (context, &parameters);

  /* Increment the counter */
  TRACE_ERR((stderr, "   dispatch_ping(), _dispatch[%zu].ping.counter = %zu -> %zu\n", n, _dispatch[n].ping.counter, _dispatch[n].ping.counter+1));
  _dispatch[n].ping.counter++;
  TRACE_ERR((stderr, "<< dispatch_ping()\n"));
}

/* --------------------------------------------------------------- */
static void dispatch_pong (pami_context_t    context,
                           void            * cookie,
                           const void      * header,
                           size_t            hdrlen,
                           const void      * data,
                           size_t            sndlen,
                           pami_endpoint_t   origin,
                           pami_recv_t     * recv)
{
  TRACE_ERR((stderr, ">> dispatch_pong()\n"));
  /* Copy the data "somewhere" to simulate an application environment */
  memcpy (_recv_buffer, data, sndlen);

  size_t n = (size_t) cookie;

  /* Increment the counter */
  TRACE_ERR((stderr, "   dispatch_pong(), _dispatch[%zu].pong.counter = %zu -> %zu\n", n, _dispatch[n].pong.counter, _dispatch[n].pong.counter+1));
  _dispatch[n].pong.counter++;
  TRACE_ERR((stderr, "<< dispatch_pong()\n"));
}

/**
 * \brief Perform a specific pingpong ring test
 *
 *
 * \param[in] client   The communcation client
 * \param[in] context  The communication context
 * \param[in] n        Send protocol id to test
 * \param[in] hdrlen   Number of bytes of header data to transfer
 * \param[in] sndlen   Number of bytes of source data to transfer
 *
 * \return Average number of cycles for a half ping-pong
 */
unsigned long long test (pami_client_t  client,
                         pami_context_t context,
                         size_t         n,
                         size_t         hdrlen,
                         size_t         sndlen)
{
  char metadata[BUFSIZE];
  char buffer[BUFSIZE];

  pami_send_immediate_t parameters;
  parameters.dispatch        = _dispatch[n].ping.id;
  parameters.header.iov_base = metadata;
  parameters.header.iov_len  = hdrlen;
  parameters.data.iov_base   = buffer;
  parameters.data.iov_len    = sndlen;

  PAMI_Endpoint_create (client, (_task+1)%_size, 0, &parameters.dest);

  size_t sent = 0;
  unsigned long long t0, t1;

  /* This handles the "send to self" case */
  if (_size == 1) _dispatch[n].ping.counter = ITERATIONS;

  if (_task == 0)
  {
    /* Initiate send pingpong test to the next task, then block until
     * this task receives all "pings" from a remote task */
    t0 = PAMI_Wtimebase(client);
    for (sent = 1; sent <= ITERATIONS; sent++)
    {
      PAMI_Send_immediate (context, &parameters);
      while (_dispatch[n].pong.counter < sent)
      {
        PAMI_Context_advance (context, 100);
      }
    }
    t1 = PAMI_Wtimebase(client);

    while (_dispatch[n].ping.counter < ITERATIONS)
      PAMI_Context_advance (context, 100);
  }
  else
  {
    /* Block until this task receives all "pings" from a remote task,
     * then initiate send pingpong test to the next task */
    while (_dispatch[n].ping.counter < ITERATIONS)
      PAMI_Context_advance (context, 100);

    t0 = PAMI_Wtimebase(client);
    for (sent = 1; sent <= ITERATIONS; sent++)
    {
      PAMI_Send_immediate (context, &parameters);
      while (_dispatch[n].pong.counter < sent)
      {
        PAMI_Context_advance (context, 100);
      }
    }
    t1 = PAMI_Wtimebase(client);
  }

  _dispatch[n].pong.counter = 0;
  _dispatch[n].ping.counter = 0;

  return ((t1-t0)/ITERATIONS)/2;
}

int main (int argc, char ** argv)
{
  ITERATIONS = 20;
  WARMUP     = 0;
  START      = 0;
  END        = 256;

  {
    int foo;
    for (foo=0; foo<argc; foo++)
      fprintf (stderr, "argv[%d] = \"%s\"\n", foo, argv[foo]);
  }

  extern char *optarg;

  int x;
  while ((x = getopt(argc, argv, "i:ws:e:")) != EOF)
  {
    fprintf(stderr, "x = %d ('%c')\n", x, x);
    switch (x)
    {
      case 'i':
        ITERATIONS = strtoul (optarg, NULL, 0);
        break;
      case 'w':
        WARMUP = 1;
        break;
      case 's':
        START = strtoul (optarg, NULL, 0);
        break;
      case 'e':
        END = strtoul (optarg, NULL, 0);
        break;
      default:
        fprintf (stderr, "Usage: %s [-i iterations][-s start][-e end][-w]\n", argv[0]);
        exit (1);
        break;
    }
  }


  TRACE_ERR((stderr, "Start test ...\n"));
  size_t hdrcnt = 1;
  size_t hdrlen[1024];
  hdrlen[0] = 0;

  pami_client_t client;
  char clientname[]="TEST";
  TRACE_ERR((stderr, "... before PAMI_Client_create()\n"));
  PAMI_Client_create (clientname, &client, NULL, 0);
  TRACE_ERR((stderr, "...  after PAMI_Client_create()\n"));
  pami_context_t context;
  TRACE_ERR((stderr, "... before PAMI_Context_createv()\n"));
  { size_t _n = 1; PAMI_Context_createv (client, NULL, 0, &context, _n); }
  TRACE_ERR((stderr, "...  after PAMI_Context_createv()\n"));


  pami_result_t result;
  pami_dispatch_callback_function fn;
  pami_dispatch_hint_t options={0};
  _dispatch_count = 0;

  pami_configuration_t configuration;

  /* Register the protocols to test */

  /* --- test default dispatch, no hints --- */
  _dispatch[_dispatch_count].ping.id = START_DISPATCH_ID + _dispatch_count;
  _dispatch[_dispatch_count].ping.counter = 0;
  fn.p2p = dispatch_ping;
  result = PAMI_Dispatch_set (context,
                              _dispatch[_dispatch_count].ping.id,
                              fn,
                              (void *) _dispatch_count,
                              options);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable register pami dispatch. result = %d\n", result);
    return 1;
  }

  configuration.name = PAMI_DISPATCH_SEND_IMMEDIATE_MAX;
  result = PAMI_Dispatch_query(context, _dispatch[_dispatch_count].ping.id, &configuration,1);
  size_t send_max = configuration.value.intval;


  _dispatch[_dispatch_count].pong.id = START_DISPATCH_ID + _dispatch_count + 1;
  _dispatch[_dispatch_count].pong.counter = 0;
  fn.p2p = dispatch_pong;
  result = PAMI_Dispatch_set (context,
                              _dispatch[_dispatch_count].pong.id,
                              fn,
                              (void *) _dispatch_count,
                              options);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable register pami dispatch. result = %d\n", result);
    return 1;
  }

  configuration.name = PAMI_DISPATCH_SEND_IMMEDIATE_MAX;
  result = PAMI_Dispatch_query(context, _dispatch[_dispatch_count].pong.id, &configuration,1);
  
  if (configuration.value.intval < send_max)
    send_max = configuration.value.intval;

  _dispatch_count += 2;
  /* --- test default dispatch, no hints --- */



  configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query(client, &configuration,1);
  _task = configuration.value.intval;

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration,1);
  _size = configuration.value.intval;

  configuration.name = PAMI_CLIENT_WTICK;
  result = PAMI_Client_query(client, &configuration,1);
  double tick = configuration.value.doubleval;


#if 0
  /* Display some test header information */
  if (_my_rank == 0)
  {
    char str[2][1024];
    int index[2];
    index[0] = 0;
    index[1] = 0;

    index[0] += sprintf (&str[0][index[0]], "#          ");
    index[1] += sprintf (&str[1][index[1]], "#    bytes ");

    fprintf (stdout, "# PAMI_Send_immediate() nearest-neighor half-pingpong blocking latency performance test\n");
    fprintf (stdout, "#\n");

    unsigned i;
    for (i=0; i<hdrcnt; i++)
    {
      if (i==0)
        fprintf (stdout, "# testcase %d : header bytes = %3zd\n", i, hdrsize[i]);
      else
        fprintf (stdout, "# testcase %d : header bytes = %3zd (argv[%d])\n", i, hdrsize[i], i);
      index[0] += sprintf (&str[0][index[0]], "[- testcase %d -] ", i);
      index[1] += sprintf (&str[1][index[1]], " cycles    usec  ");
    }

    fprintf (stdout, "#\n");
    fprintf (stdout, "%s\n", str[0]);
    fprintf (stdout, "%s\n", str[1]);
    fflush (stdout);
  }
#endif

  for (unsigned i = 0; i < 10000000;i++){};

  unsigned long long cycles;
  double usec;

  char str[10240];

  if (send_max < END)
    END = send_max;

  size_t sndlen;
  for (sndlen = START; sndlen < END; sndlen = sndlen*3/2+1)
  {
    int index = 0;
    index += sprintf (&str[index], "%10zd ", sndlen);

    unsigned hdr;
    for (hdr=0; hdr<hdrcnt; hdr++)
    {
      if (WARMUP)
        test (client, context, 0, hdrlen[hdr], sndlen);

      cycles = test (client, context, 0, hdrlen[hdr], sndlen);
      usec   = cycles * tick * 1000000.0;
      index += sprintf (&str[index], "%7lld %7.4f  ", cycles, usec);
      //index += sprintf (&str[index], "%7lld  ", cycles);
    }

    fprintf (stdout, "[task %02u] %s\n", _task, str);
  }

  PAMI_Client_destroy (&client);

  return 0;
}
#undef TRACE_ERR
