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
 * \file test/api/p2p/send/send_unexpected_func.c
 * \brief ???
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#include <pami.h>

#define MSGCOUNT 1
/*#define MSGCOUNT 16 */

#undef TRACE_ERR
#ifndef TRACE_ERR
#define TRACE_ERR(x) fprintf x
#endif

volatile unsigned _recv_active;
uint8_t _tmpbuffer[10240];
size_t _my_rank;

/* --------------------------------------------------------------- */

static void decrement (pami_context_t   context,
                       void          * cookie,
                       pami_result_t    result)
{
  unsigned * value = (unsigned *) cookie;
  TRACE_ERR((stderr, "(%zu) decrement() cookie = %p, %d => %d\n", _my_rank, cookie, *value, *value-1));
  --*value;
}

/* --------------------------------------------------------------- */
static void test_dispatch (
    pami_context_t       context,      /**< IN: PAMI context */
    void               * cookie,       /**< IN: dispatch cookie */
    const void         * header_addr,  /**< IN: header address */
    size_t               header_size,  /**< IN: header size */
    const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
    size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
    pami_endpoint_t origin,
pami_recv_t         * recv)        /**< OUT: receive message structure */
{
  unsigned * value = (unsigned *) cookie;
  if (pipe_addr != NULL)
  {
    TRACE_ERR((stderr, "(%zu) short recv:  decrement cookie = %p, %d => %d\n", _my_rank, cookie, *value, *value-1));
    memcpy((void *)_tmpbuffer, pipe_addr, pipe_size);
    --*value;
    return;
  }

  TRACE_ERR((stderr, "(%zu) long recvn", _my_rank));
  recv->local_fn = decrement;
  recv->cookie   = cookie;
  recv->type     = PAMI_TYPE_BYTE;
  recv->addr     = (void *)_tmpbuffer;
  recv->offset   = 0;
}

int main (int argc, char ** argv)
{
  TRACE_ERR((stderr, "Start test ...\n"));

  pami_client_t client;
  char clientname[]="TEST";
  TRACE_ERR((stderr, "... before PAMI_Client_create()\n"));
  PAMI_Client_create (clientname, &client, NULL, 0);
  TRACE_ERR((stderr, "...  after PAMI_Client_create()\n"));
  pami_context_t context;
  TRACE_ERR((stderr, "... before PAMI_Context_createv()\n"));
  PAMI_Context_createv (client, NULL, 0, &context, 1);
  TRACE_ERR((stderr, "...  after PAMI_Context_createv()\n"));

  pami_configuration_t configuration;

  configuration.name = PAMI_CLIENT_TASK_ID;
  pami_result_t result = PAMI_Client_query(client, &configuration, 1);
  _my_rank = configuration.value.intval;

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration,1);
  size_t num_tasks = configuration.value.intval;

  _recv_active = MSGCOUNT * (num_tasks - 1);
  sleep(1);


  /* Delay task 0 to force unexpected packets */
  size_t i;
  if (_my_rank == 0)
    for (i=0; i<1000; i++)
      PAMI_Context_advance (context, 100);

  /* Register the protocols to test */
  size_t dispatch = 1;
  pami_dispatch_callback_function fn;
  fn.p2p = test_dispatch;
  pami_dispatch_hint_t options={};
  TRACE_ERR((stderr, "Before PAMI_Dispatch_set() .. &_recv_active = %p, recv_active = %u\n", &_recv_active, _recv_active));
  result = PAMI_Dispatch_set (context,
                              dispatch,
                              fn,
                              (void *)&_recv_active,
                              options);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable register pami dispatch. result = %d\n", result);
    return 1;
  }

  if (_my_rank == 0)
  {
    while (_recv_active > 0) PAMI_Context_advance (context, 100);
  }
  else
  {
    char metadata[10240];
    char buffer[10240];

    volatile unsigned send_active = MSGCOUNT * 2;

    pami_send_t parameters;
    PAMI_Endpoint_create (client, 0, 0, &parameters.send.dest);
    parameters.send.dispatch        = dispatch;
    parameters.send.header.iov_base = metadata;
    parameters.send.header.iov_len  = 32;
    parameters.send.data.iov_base   = buffer;
    parameters.send.data.iov_len    = 2048;
    parameters.events.cookie        = (void *) &send_active;
    parameters.events.local_fn      = decrement;
    parameters.events.remote_fn     = decrement;

    size_t i;
    for (i=0; i<MSGCOUNT; i++)
    {
      PAMI_Send (context, &parameters);
    }

    while (send_active > 0) PAMI_Context_advance (context, 100);
  }

  PAMI_Client_destroy(&client);

  return 0;
}
#undef TRACE_ERR
