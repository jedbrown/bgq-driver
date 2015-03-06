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
 * \file test/api/context/post-multithreaded.c
 * \brief Multithreaded PAMI_Context_post() test
 */

#include <pami.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

/*#define ENABLE_TRACE */

#ifdef ENABLE_TRACE
#define TRACE(x) fprintf x
#else
#define TRACE(x)
#endif


typedef struct endpoint
{
  pami_context_t   context;
  volatile size_t recv;
} endpoint_t;

typedef struct work
{
  void            * addr;
  size_t            bytes;
  volatile size_t   active;
  size_t            from;
  size_t            to;
} work_t;


endpoint_t _endpoint[2];

pami_result_t do_work (pami_context_t   context,
              void          * cookie)
{
  TRACE((stderr, ">> do_work (%0x08x, %p)\n", (unsigned)context, cookie));
  work_t * work = (work_t *) cookie;

  /* copy the data to a local buffer. */
  uint8_t local_buffer[1024];
  size_t  n = work->bytes<1024?work->bytes:1024;
  size_t to = work->to;
  TRACE((stderr, "   do_work (), work->bytes = %zu, work->addr = %p, to = %zu, n = %zu\n", work->bytes, work->addr, to, n));
  memcpy ((void *) local_buffer, work->addr, n);

  /* Notify the 'sender' that the 'receive' is complete. */
  work->active = 0;

  /* decrement the recv flag */
  TRACE((stderr, "   do_work (), _endpoint[%zu].recv = %zu -> %zu\n", to, _endpoint[to].recv, _endpoint[to].recv - 1));
  _endpoint[to].recv--;

  TRACE((stderr, "<< do_work ()\n"));
  return PAMI_SUCCESS;
}

void * endpoint (void * arg)
{
  size_t id = (size_t) arg;
  TRACE((stderr, ">> endpoint (%zu)\n", id));

  /* Lock this context */
  pami_result_t result = PAMI_Context_lock (_endpoint[id].context);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to lock the pami context. result = %d\n", result);
    exit(1);
  }

  uint8_t send[1024];
  uint8_t recv[1024];
  size_t i;
  for (i=0; i<1024; i++)
  {
    send[i] = (uint8_t) -1;
    recv[i] = 0;
  }

  if (id == 0)
  {
    /* send a message to endpoint 10 */
    pami_work_t state;
    work_t work;
    work.addr = (void *) send;
    work.bytes = 1024;
    work.active = 1;
    work.from = 0;
    work.to   = 1;

    result = PAMI_Context_post (_endpoint[1].context, &state, do_work, (void *)&work);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to post work to the pami context. result = %d\n", result);
      exit(1);
    }

    /* block until the work has been "received" */
    TRACE((stderr, "   endpoint(%zu), before blocking 'advance', work.active = %zu\n", id, work.active));
    while (work.active);
    TRACE((stderr, "   endpoint(%zu),  after blocking 'advance', work.active = %zu\n", id, work.active));

    /* wait to "receive" a message from endpoint 1 */
    TRACE((stderr, "   endpoint(%zu), before blocking advance for recv, _endpoint[0].recv = %zu\n", id, _endpoint[0].recv));
    while (_endpoint[0].recv)
    {
      result = PAMI_Context_advance (_endpoint[0].context, 1);
      if ((result != PAMI_SUCCESS) && (result != PAMI_EAGAIN))
      {
        fprintf (stderr, "Error. Unable to advance the pami context. result = %d\n", result);
        exit(1);
      }
    }
    TRACE((stderr, "   endpoint(%zu),  after blocking advance for recv, _endpoint[0].recv = %zu\n", id, _endpoint[0].recv));
  }
  else if (id == 1)
  {
    /* wait to "receive" a message from endpoint 0 */
    TRACE((stderr, "   endpoint(%zu), before blocking advance for recv, _endpoint[1].recv = %zu\n", id, _endpoint[1].recv));
    while (_endpoint[1].recv)
    {
      result = PAMI_Context_advance (_endpoint[1].context, 1);
      if ((result != PAMI_SUCCESS) && (result != PAMI_EAGAIN))
      {
        fprintf (stderr, "Error. Unable to advance the pami context. result = %d\n", result);
        exit(1);
      }
    }
    TRACE((stderr, "   endpoint(%zu),  after blocking advance for recv, _endpoint[1].recv = %zu\n", id, _endpoint[1].recv));

    /* send a message to endpoint 1 */
    pami_work_t state;
    work_t work;
    work.addr = (void *) send;
    work.bytes = 1024;
    work.active = 1;
    work.from = 1;
    work.to   = 0;

    result = PAMI_Context_post (_endpoint[0].context, &state, do_work, (void *)&work);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to post work to the pami context. result = %d\n", result);
      exit(1);
    }

    /* block until the work has been "received" */
    TRACE((stderr, "   endpoint(%zu), before blocking 'advance', work.active = %zu\n", id, work.active));
    while (work.active);
    TRACE((stderr, "   endpoint(%zu),  after blocking 'advance', work.active = %zu\n", id, work.active));
  }

  /* Unlock the context and exit */
  result = PAMI_Context_unlock (_endpoint[id].context);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to unlock the pami context. result = %d\n", result);
    exit(1);
  }

  TRACE((stderr, "<< endpoint(%zu)\n", id));
  return NULL;
}


int main (int argc, char ** argv)
{
  pami_client_t client;
  pami_context_t context[2];
  pami_configuration_t configuration;
  char                  cl_string[] = "TEST";
  pami_result_t result = PAMI_ERROR;

  result = PAMI_Client_create (cl_string, &client, NULL, 0);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to initialize pami client. result = %d\n", result);
    return 1;
  }

  configuration.name = PAMI_CLIENT_NUM_CONTEXTS;
  result = PAMI_Client_query(client, &configuration, 1);
  if (configuration.value.intval < 2)
  {
    fprintf (stderr, "Error. Multi-context not supported. PAMI_CLIENT_NUM_CONTEXTS = %zu\n", configuration.value.intval);
    return 1;
  }

  _endpoint[0].recv = 1;
  result = PAMI_Context_createv (client, &configuration, 0, context, 2);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to create pami contexts. result = %d\n", result);
    return 1;
  }
  _endpoint[0].context = context[0];
  _endpoint[1].context = context[1];

  _endpoint[1].recv = 1;

  /* Create the "helper" or "endpoint" thread */
  pthread_t thread;
  int rc = pthread_create (&thread, NULL, endpoint, (void *)1);
  if (rc != 0)
  {
    fprintf (stderr, "Error. Unable to create the second pthread. rc = %d\n", rc);
    return 1;
  }

  /* enter the "endpoint function" for the main thread */
  endpoint ((void *)0);


  result = PAMI_Context_destroyv (context, 2);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to destroy first pami context. result = %d\n", result);
    return 1;
  }

  result = PAMI_Client_destroy(&client);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to finalize pami client. result = %d\n", result);
    return 1;
  }

  fprintf (stderr, "Success.\n");

  return 0;
};
