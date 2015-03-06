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
 * \file test/api/p2p/simple-send-immediate.c
 * \brief Simple point-to-point PAMI_Send_immediate() test
 **/

#include <stdio.h>

#include "../init_util.h"

#define RC(statement) \
{ \
    int rc = statement; \
    if (rc != PAMI_SUCCESS) { \
        printf(#statement " rc = %d, line %d\n", rc, __LINE__); \
        exit(-1); \
    } \
}

unsigned validate (const void * addr, size_t bytes)
{
  unsigned status = 1;
  uint8_t * byte = (uint8_t *) addr;
  uint8_t i;

  for (i = 0; i < bytes; i++)
    {
      if (byte[i] != i)
        {
          fprintf (stderr, "validate(%p,%zu) .. ERROR .. byte[%d] != %d (value is %d)\n",
                   addr, bytes, i, i, byte[i]);
          status = 0;
        }
    }

  return status;
}

/**
 * \brief dispatch function for immediate sends
 *
 * \see pami_dispatch_p2p_function
 */
/* [example dispatch function] */
void test_dispatch (
  pami_context_t    context,      /**< IN: PAMI context */
  void            * cookie,       /**< IN: dispatch cookie */
  const void      * header,       /**< IN: header address */
  size_t            header_size,  /**< IN: header size */
  const void      * data,         /**< IN: address of PAMI pipe buffer */
  size_t            data_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t   origin,
  pami_recv_t     * recv)         /**< OUT: receive message structure */
{
  volatile size_t * active = (volatile size_t *) cookie;
  (*active)--;

  if (validate (header, header_size))
    fprintf (stderr, ">>> header validated.\n");
  else
    fprintf (stderr, ">>> header ERROR !!\n");

  if (validate (data, data_size))
    fprintf (stderr, ">>> payload validated.\n");
  else
    fprintf (stderr, ">>> payload ERROR !!\n");

  return;
}
/* [example dispatch function] */


int main (int argc, char ** argv)
{
  pami_client_t        client;
  pami_context_t       context[1];
  size_t               num_contexts = 1;
  pami_task_t          task_id;
  size_t               num_tasks;
  pami_result_t        result;

  RC( pami_init (&client,        /* Client             */
                 context,       /* Context            */
                 NULL,           /* Clientname=default */
                 &num_contexts,  /* num_contexts       */
                 NULL,           /* null configuration */
                 0,              /* no configuration   */
                 &task_id,       /* task id            */
                 &num_tasks) );  /* number of tasks    */

  /* [example dispatch set for all contexts] */
  volatile size_t recv_active[2];
  recv_active[0] = 1;
  recv_active[1] = 1;

  size_t dispatch = 10;
  pami_dispatch_callback_function fn;
  fn.p2p = test_dispatch;
  pami_dispatch_hint_t options = {};

  size_t i;

  for (i = 0; i < num_contexts; i++)
    {
      result = PAMI_Dispatch_set (context[i],
                                  dispatch,
                                  fn,
                                  (void *) & recv_active[i],
                                  options);

      if (result != PAMI_SUCCESS)
        {
          fprintf (stderr, "Error. Unable register pami dispatch. result = %d\n", result);
          return 1;
        }
    }

  /* [example dispatch set for all contexts] */

  uint8_t header[1024];
  uint8_t data[1024];

  for (i = 0; i < 1024; i++)
    {
      header[i] = i;
      data[i]   = i;
    }

  /* [example 'ping' from task 0] */
  if (task_id == 0)
    {
      size_t target = 1 % num_contexts;

      pami_send_immediate_t parameters;
      parameters.dispatch        = dispatch;
      parameters.header.iov_base = header;
      parameters.header.iov_len  = 4;
      parameters.data.iov_base   = data;
      parameters.data.iov_len    = 32;
      RC( PAMI_Endpoint_create (client, 1, target, &parameters.dest) );

      fprintf (stdout, "PAMI_Send_immediate() functional test [%scrosstalk]\n", (num_contexts == 1) ? "no " : "");
      fprintf (stdout, "\n");

      RC( PAMI_Send_immediate (context[0], &parameters) );

      while (recv_active[0] != 0)
        {
          result = PAMI_Context_advance (context[0], 100);

          if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
            {
              fprintf (stderr, "Error. Unable to advance pami context. result = %d\n", result);
              return 1;
            }
        }
    }

  /* [example 'ping' from task 0] */

  /* [example 'pong' from task 1] */
  else if (task_id == 1)
    {
      size_t target = 1 % num_contexts;

      while (recv_active[target] != 0)
        {
          result = PAMI_Context_advance (context[target], 100);

          if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
            {
              fprintf (stderr, "Error. Unable to advance pami context. result = %d\n", result);
              return 1;
            }
        }

      pami_send_immediate_t parameters;
      parameters.dispatch        = dispatch;
      parameters.header.iov_base = header;
      parameters.header.iov_len  = 4;
      parameters.data.iov_base   = data;
      parameters.data.iov_len    = 32;
      RC( PAMI_Endpoint_create (client, 0, 0, &parameters.dest) );

      result = PAMI_Send_immediate (context[target], &parameters);

      if (result != PAMI_SUCCESS)
        {
          fprintf (stderr, "Error. Unable to send immediate. result = %d\n", result);
          return 1;
        }

      result = PAMI_Context_advance (context[target], 100);

      if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
        {
          fprintf (stderr, "Error. Unable to advance pami context. result = %d\n", result);
          return 1;
        }
    }

  /* [example 'pong' from task 1] */

  RC( pami_shutdown(&client, context, &num_contexts) );

  return 0;
};
