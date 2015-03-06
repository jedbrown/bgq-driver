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
 * \file test/api/p2p/send/mmps.c
 * \brief A threaded message-rate benchmark
 */

/* There simply aren't enough cores on a BGP node to make the threaded version work */
#ifndef __pami_target_bgp__
/* USE_THREADS will try to use pthreads or comm-threads to
 * asynchronously advance the contexts.  If not defined, everything
 * will be done on the main thread
 */
#define USE_THREADS
#endif
/* Defining NDEBUG causes assert checks to not run, so the app runs faster */
/* #define NDEBUG */
/* Trace mode spews lots of debug info */
/* #define TRACE_ON */
/* There is a "pthread" mode which uses explicate pthreads to advance
 * the contexts instead of comm-threads.  It is the default on non-BGQ
 * systems, and can be forced by uncommenting the following line.
 */
/* #undef __pami_target_bgq__ */

#define NCONTEXTS  64      /* The maximum number of contexts */
#define ITERATIONS (1<<12)  /* The number of windows exectuted */
#define WINDOW     (1<<4)  /* The number of sends/recvs before a "wait" */
#define HEADER     16      /* Size of header */
#define DATA       0       /* Size of data */
#define MAX_SIZE   32      /* The number of processes that can participate */
#define DISPATCH   0       /* Dispatch ID */


#define MIN(a,b) ((a<b)?a:b)

#ifdef TRACE_ON
#ifdef __GNUC__
#define TRACE_ALL(fd, format, ...) fprintf(fd, "(rank=%02zu,line=%03u,func=%s) " format, rank, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define TRACE_OUT(format, ...) TRACE_ALL(stdout, format, ##__VA_ARGS__)
#define TRACE_ERR(format, ...) TRACE_ALL(stderr, format, ##__VA_ARGS__)
#else
#define TRACE_OUT(format...) fprintf(stdout, format)
#define TRACE_ERR(format...) fprintf(stderr, format)
#endif
#else
#define TRACE_OUT(format...)
#define TRACE_ERR(format...)
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <pami.h>

#if defined(__pami_target_bgq__) && defined(USE_THREADS)
#include "test/async_progress.h"
#endif


static pami_client_t  client;
static pami_context_t contexts[NCONTEXTS];
static volatile size_t recv_list[MAX_SIZE] = {0};
static volatile size_t send_list[MAX_SIZE] = {0};
static size_t ncontexts = NCONTEXTS;
static size_t rank=99, size=(size_t)-1;

#if !defined(__pami_target_bgq__) && defined(USE_THREADS)
#include <pthread.h>
static pthread_t threads[NCONTEXTS];

static void*
advance(void* arg)
{
  pami_result_t rc;
  pami_context_t context = *(pami_context_t*)arg;
  TRACE_ERR("context=%p\n", context);
  for (;;) {
    rc = PAMI_Context_advance(context, (size_t)-1);
    assert((rc == PAMI_SUCCESS) || (rc == PAMI_EAGAIN));
  }
  return NULL;
}
#endif


static pami_result_t
send(pami_context_t context, void *_dest)
{
  const size_t mod = ncontexts>>1;
  pami_result_t rc;
  size_t dest = (size_t)_dest;
  TRACE_ERR("running posted work  dest=%zu[%zu] arg=%p\n", dest, (rank%mod)+mod, _dest);

  struct
  {
    char header[HEADER];
    char data[DATA];
  } buffer;

  pami_send_immediate_t params = {
  header : {
    iov_base : &buffer.header,
    iov_len  : HEADER,
  },
  data : {
    iov_base : &buffer.data,
    iov_len  : DATA,
  },
  dispatch : DISPATCH,
  };
  /* The remote context is set to the upper half of the context space */
  rc = PAMI_Endpoint_create(client, dest, (rank%mod)+mod, &params.dest);
  assert(rc == PAMI_SUCCESS);
  rc = PAMI_Send_immediate(context, &params);
  assert(rc == PAMI_SUCCESS);

  ++send_list[dest];
  return PAMI_SUCCESS;
}


static void
recv(pami_context_t    context,
     void            * _contextid,
     const void      * _msginfo,
     size_t            msginfo_size,
     const void      * sndbuf,
     size_t            sndlen,
     pami_endpoint_t   _sender,
     pami_recv_t     * recv)
{
  assert(recv == NULL);

  pami_result_t rc;
  pami_task_t sender;
  size_t contextid;

  rc = PAMI_Endpoint_query(_sender, &sender, &contextid);
  assert(rc == PAMI_SUCCESS);

  TRACE_ERR("dest=%zu[%zu] contextid=%zu\n", (size_t)sender, contextid, (size_t)_contextid);
  ++recv_list[sender];
}


static void
check_complete_list(volatile size_t *count, size_t iteration)
{
  while(*count < WINDOW*(iteration+1)) {
#ifndef USE_THREADS
    PAMI_Context_advancev(contexts, ncontexts, 1);
#endif
  }
}


static void
check_complete(size_t dest, size_t iteration)
{
  TRACE_ERR("           iteration=%02zu dest=%02zu\n", iteration, dest);
  check_complete_list(&send_list[dest], iteration);
  TRACE_ERR("send done  iteration=%02zu dest=%02zu\n", iteration, dest);
  check_complete_list(&recv_list[dest], iteration);
  TRACE_ERR("recv done  iteration=%02zu dest=%02zu\n", iteration, dest);
}


static void
post_work(size_t dest, size_t iteration, size_t window, pami_work_t *work)
{
  const size_t mod = ncontexts>>1;
  pami_result_t rc;

  /* The local context is set to the lower half of the context space */
  size_t contextid = dest%mod;
  pami_context_t context = contexts[contextid];
  TRACE_ERR("Posting work  contextid=%zu dest=%zu arg=%p iteration=%zu window=%zu\n", contextid, dest, (void*)dest, iteration, window);
  rc = PAMI_Context_post(context, work, send, (void*)dest);
  assert(rc == PAMI_SUCCESS);
}


static void
master()
{
  const size_t msgs = ITERATIONS*WINDOW*(size-1);
  printf(">>> Test: ITERATIONS=%u  WINDOW=%u  Remotes=%zu  Bi-Dir Msgs=%zu  HEADER=%u  DATA=%u\n",
         ITERATIONS, WINDOW, size-1, msgs, HEADER, DATA);
  double start, time;
  start = PAMI_Wtime(client);

  size_t dest;
  size_t iteration, i;
  pami_work_t work_list[WINDOW][size];

  for (iteration=0; iteration<ITERATIONS; ++iteration) {
    TRACE_ERR("starting sends  iteration=%zu\n", iteration);
    for (i=0; i<WINDOW; ++i) {
      for (dest=1; dest<size; ++dest) {
        post_work(dest, iteration, i, &work_list[i][dest]);
      }
    }

    TRACE_ERR("Starting completion check  iteration=%zu\n", iteration);
    /* Check that everything is done */
    for (dest=1; dest<size; ++dest) {
      check_complete(dest, iteration);
    }
  }

  time = PAMI_Wtime(client)-start;
  printf("::: Communication complete on process %zu (%g seconds)\n", rank, time);
  printf(">>> Results: %zu bi-directional messages in %g seconds is %g MMPS\n",
         msgs, time, msgs/(time*1e6));
}


static void
worker()
{
  TRACE_ERR("Starting worker\n");
  double start, time;
  start = PAMI_Wtime(client);

  const size_t dest = 0;
  size_t iteration, i;
  pami_work_t work_list[WINDOW][1];

  for (iteration=0; iteration<ITERATIONS; ++iteration) {
    TRACE_ERR("starting sends  iteration=%zu\n", iteration);
    for (i=0; i<WINDOW; ++i) {
      {
        post_work(dest, iteration, i, &work_list[i][dest]);
      }
    }

    /* Check that everything is done */
    TRACE_ERR("Starting completion check  iteration=%zu\n", iteration);
    {
      check_complete(dest, iteration);
    }
  }

  time = PAMI_Wtime(client) - start;
  printf("::: Communication complete on process %zu (%g seconds)\n", rank, time);
}


static pami_configuration_t
client_query(pami_client_t         client,
             pami_attribute_name_t name)
{
  pami_result_t rc;
  pami_configuration_t query;
  query.name = name;
  rc = PAMI_Client_query(client, &query, 1);
  assert(rc == PAMI_SUCCESS);
  return query;
}


static void
init()
{
  size_t i;
  pami_result_t rc;
  pami_configuration_t configuration;

  rc = PAMI_Client_create("TEST", &client, NULL, 0);
  assert(rc == PAMI_SUCCESS);

  rank = client_query(client, PAMI_CLIENT_TASK_ID).value.intval;
  size = client_query(client, PAMI_CLIENT_NUM_TASKS).value.intval;
  assert(size > 1);
  size = MIN(size, MAX_SIZE);

  assert(client_query(client, PAMI_CLIENT_CONST_CONTEXTS).value.intval != 0);

  size_t query;
  query = client_query(client, PAMI_CLIENT_NUM_CONTEXTS).value.intval;
  ncontexts = MIN(ncontexts, query);
#ifdef USE_THREADS
  /* We don't care about other cores if we aren't in threaded mode */
  query = client_query(client, PAMI_CLIENT_HWTHREADS_AVAILABLE).value.intval;
  assert(query > 2); /* This requires a send helper and a recv helper */
#ifndef __pami_target_bgq__
  ncontexts = MIN(ncontexts, query-1);
#else
  rc = init_async_prog();
  assert(rc == PAMI_SUCCESS);
  /** \todo Remove this when trac #247 is fixed, since it shows a bug
   *   when having more contexts than active comm-threads.  While the
   *   code is correct for comm-thread mode (but not pthreads, where
   *   we would over-subscribe the HW Threads), we don't actually want
   *   to put more than one context per comm-thread.
   */
  ncontexts = MIN(ncontexts, query);
#endif
#endif

  ncontexts &= ~(size_t)1;  /* Make it even */
  assert(ncontexts>1); /* There must be at least 2 */
  assert((ncontexts&1) == 0); /* There must be an even number */

  pami_configuration_t params = {
  name  : PAMI_CLIENT_CONST_CONTEXTS,
  value : { intval : 1, },
  };
  rc = PAMI_Context_createv(client, &params, 1, contexts, ncontexts);
  printf("%d = PAMI_Context_createv(client=%p, {PAMI_CLIENT_CONST_CONTEXTS = 1}, contexts=%p, ncontexts=%zu);\n", rc, client, contexts, ncontexts);
  assert(rc == PAMI_SUCCESS);

  for (i=0; i<ncontexts; ++i) {
  /** \todo Add back in when trac 242 is done */
#ifdef TRAC_242_IS_FIXED
    configuration.name = PAMI_CONTEXT_DISPATCH_ID_MAX;
    rc = PAMI_Context_query(contexts[i], &configuration, 1);
    assert(rc == PAMI_SUCCESS);
    assert(configuration.value.intval >= DISPATCH);
#endif

    pami_dispatch_callback_function _recv = {p2p:recv};
    pami_dispatch_hint_t options = {
    consistency: PAMI_HINT_ENABLE,
    long_header: PAMI_HINT_DISABLE,
    };
    PAMI_Dispatch_set(contexts[i],
                      DISPATCH,
                      _recv,
                      (void*)i,
                      options);

    configuration.name = PAMI_DISPATCH_RECV_IMMEDIATE_MAX;
    rc = PAMI_Dispatch_query(contexts[i], DISPATCH, &configuration, 1);
    assert(rc == PAMI_SUCCESS);
    assert(configuration.value.intval >= (HEADER+DATA));
    configuration.name = PAMI_DISPATCH_SEND_IMMEDIATE_MAX;
    rc = PAMI_Dispatch_query(contexts[i], DISPATCH, &configuration, 1);
    assert(rc == PAMI_SUCCESS);
    assert(configuration.value.intval >= (HEADER+DATA));

#ifdef USE_THREADS
#ifdef __pami_target_bgq__
      rc = async_prog_enable(contexts[i], PAMI_ASYNC_ALL);
      assert(rc == PAMI_SUCCESS);
#else
      int result;
      result = pthread_create(&threads[i], NULL, advance, &contexts[i]);
      assert(result == 0);
#endif
#endif
  }
}


int
main(int argc, char **argv)
{
  TRACE_ERR("Before init\n");
  init();


  TRACE_ERR("after init, starting master/worker\n");
  if (rank==0)
    master();
  else if (rank < MAX_SIZE)
    worker();
  else
    ;

#if defined(__pami_target_bgq__) || !defined(USE_THREADS)
  pami_result_t rc;

  rc = PAMI_Context_destroyv(contexts, ncontexts);
  assert(rc == PAMI_SUCCESS);

  rc = PAMI_Client_destroy(&client);
  assert(rc == PAMI_SUCCESS);
#endif

  return 0;
};
