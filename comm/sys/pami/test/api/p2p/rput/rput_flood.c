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
 * \file test/api/p2p/rput/rput_flood.c
 * \brief PAMI_Rput() 'flood' test
 **/

#include <pami.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>

#include "../../init_util.h"

#define ITERATIONS 1024
#define BUFFERSIZE 1024*32

typedef struct
{
  pami_memregion_t mr;
  size_t           bytes;
} mr_t;

typedef struct
{
  volatile size_t counter;
  mr_t            task[0];
} mr_exchange_t;


/* --------------------------------------------------------------- */

static void decrement (pami_context_t   context,
                       void           * cookie,
                       pami_result_t    result)
{
  unsigned * value = (unsigned *) cookie;
  /*fprintf (stderr, "decrement() cookie = %p, %d => %d\n", cookie, *value, *value - 1);*/
  --*value;
}

/* --------------------------------------------------------------- */

/**
 * \brief memory region exchange dispatch function
 */
void mr_exchange_fn (
  pami_context_t    context,      /**< IN: PAMI context */
  void            * cookie,       /**< IN: dispatch cookie */
  const void      * header,       /**< IN: header address */
  size_t            header_size,  /**< IN: header size */
  const void      * data,         /**< IN: address of PAMI pipe buffer */
  size_t            data_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t   origin,
  pami_recv_t     * recv)         /**< OUT: receive message structure */
{
  pami_task_t id;
  size_t offset;
  PAMI_Endpoint_query (origin, &id, &offset);

  mr_exchange_t * exchange = (mr_exchange_t *) cookie;
  memcpy(&exchange->task[id].mr, data, sizeof(pami_memregion_t));
  exchange->task[id].bytes = *((size_t *) header);
  exchange->counter--;

  return;
}


int main (int argc, char ** argv)
{
  pami_client_t        client;
  pami_context_t       context;
  size_t               num_contexts = 1;
  pami_task_t          me;
  size_t               num_tasks;
  pami_result_t        result;

  int rc = pami_init (&client,        /* Client             */
                      &context,       /* Context            */
                      NULL,           /* Clientname=default */
                      &num_contexts,  /* num_contexts       */
                      NULL,           /* null configuration */
                      0,              /* no configuration   */
                      &me,            /* task id            */
                      &num_tasks);    /* number of tasks    */

  if (rc == 1)
    return 1;

  mr_exchange_t * exchange =
    (mr_exchange_t *) malloc (sizeof(mr_exchange_t) + sizeof(mr_t) * num_tasks);

  /* Initialize the exchange information */
  exchange->counter = num_tasks;

  mr_t * info = exchange->task;

  size_t dispatch = 10;
  pami_dispatch_callback_function fn;
  fn.p2p = mr_exchange_fn;
  pami_dispatch_hint_t options = {};

  result = PAMI_Dispatch_set (context,
                              dispatch,
                              fn,
                              (void *) exchange,
                              options);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable register pami dispatch. result = %d\n", result);
      return 1;
    }


  /* Allocate and intialize the local data buffer for the test. */
  uint8_t * local = (uint8_t *) malloc (BUFFERSIZE);

  /* Create a memory region for the local data buffer. */
  size_t bytes;
  pami_memregion_t mr;
  result = PAMI_Memregion_create (context, (void *) local, BUFFERSIZE,
                                  &bytes, &mr);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to create memory region. result = %d\n", result);
      return 1;
    }
  else if (bytes < BUFFERSIZE)
    {
      fprintf (stderr, "Error. Unable to create memory region of a large enough size. result = %d\n", result);
      return 1;
    }


  /* Broadcast the data location to all tasks */
  size_t i;

  for (i = 0; i < num_tasks; i++)
    {
      pami_send_immediate_t parameters;
      parameters.dispatch        = dispatch;
      parameters.header.iov_base = (void *) & bytes;
      parameters.header.iov_len  = sizeof(size_t);
      parameters.data.iov_base   = (void *) & mr;
      parameters.data.iov_len    = sizeof(pami_memregion_t);
      PAMI_Endpoint_create (client, i, 0, &parameters.dest);

      result = PAMI_Send_immediate (context, &parameters);
    }

  /* Wait until all tasks have exchanged the memory region information */
  while (exchange->counter > 0)
    PAMI_Context_advance (context, 100);




  /* **************************************************************************
   * Begin the test - all tasks, including the 'root' task, will use PAMI_Rput
   * to flood the 'root' task and (hopefully) fill network fifos and force
   * a message queue event.
   * **************************************************************************/
  volatile unsigned active;

  pami_rput_simple_t parameters;
  parameters.rma.hints          = (pami_send_hint_t) {0};

  parameters.rma.bytes          = info[0].bytes;

  if (parameters.rma.bytes < info[me].bytes)
    parameters.rma.bytes        = info[0].bytes;

  parameters.rma.cookie         = (void *) & active;
  parameters.rma.done_fn        = decrement;
  parameters.rdma.local.mr      = &info[me].mr;
  parameters.rdma.local.offset  = 0;
  parameters.rdma.remote.mr     = &info[0].mr;
  parameters.rdma.remote.offset = 0;
  parameters.put.rdone_fn       = NULL;
  PAMI_Endpoint_create (client, 0, 0, &parameters.rma.dest);

  active = ITERATIONS;

  for (i = 0; i < ITERATIONS; i++)
    PAMI_Rput (context, &parameters);

  while (active > 0)
    PAMI_Context_advance (context, 100);

  fprintf (stdout, "task %4d done.\n", me);

  rc = pami_shutdown(&client, context, &num_contexts);

  if (rc == 1)
    return 1;

  return 0;
};
