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
 * \file test/api/p2p/put/simple_put_func.c
 * \brief Simple point-to-point PAMI_Put() test
 *
 * This test implements a very simple "ping-ack" communication and
 * depends on a functional PAMI_Send_immediate() function.
 */

#include <pami.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define PI 3.14159265
#define E  2.71828183

#define DISPATCH_ID 10

#define BUFFERSIZE 1024*16

#undef TRACE_ERR
#ifndef TRACE_ERR
#define TRACE_ERR(x)  /*fprintf x */
#endif

volatile size_t  _done;
double           _buffer[BUFFERSIZE];
struct iovec     _dst[2];

static void decrement (pami_context_t context, void * cookie, pami_result_t result)
{
  size_t * value = (size_t *) cookie;
  --*(value);
}

static void dispatch_send (
  pami_context_t       context,      /**< IN: origin context */
  void               * cookie,       /**< IN: dispatch cookie */
  const void         * header_addr,  /**< IN: header address */
  size_t               header_size,  /**< IN: header size */
  const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
  size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t      origin,       /**< IN: endpoint that originated the send */
  pami_recv_t        * recv)         /**< OUT: receive message structure */
{
  struct iovec * src = (struct iovec *) pipe_addr;
  _dst[0] = src[0];
  _dst[1] = src[1];

  _done = 1;

  fprintf (stderr, "dispatch_send(), {{%zu, %p}, {%zu, %p}}\n", src[0].iov_len, src[0].iov_base, src[1].iov_len, src[1].iov_base);

  return;
}


int main (int argc, char ** argv)
{
  _done = 0;

  pami_client_t client;
  pami_context_t context[2];
  pami_task_t task_id;
  size_t num_tasks = 0;
  size_t ncontexts = 0;
  size_t errors = 0;

  pami_result_t result = PAMI_ERROR;

  { /* init */
    result = PAMI_Client_create ("TEST", &client, NULL, 0);

    if (result != PAMI_SUCCESS)
      {
        fprintf (stderr, "Error. Unable to create pami client. result = %d\n", result);
        return 1;
      }

    pami_configuration_t configuration;

    configuration.name = PAMI_CLIENT_TASK_ID;
    result = PAMI_Client_query(client, &configuration, 1);

    if (result != PAMI_SUCCESS)
      {
        fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, result);
        return 1;
      }

    task_id = configuration.value.intval;
    fprintf (stderr, "My task id = %d\n", task_id);

    configuration.name = PAMI_CLIENT_NUM_TASKS;
    result = PAMI_Client_query(client, &configuration, 1);

    if (result != PAMI_SUCCESS)
      {
        fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, result);
        return 1;
      }

    num_tasks = configuration.value.intval;
    if (task_id == 0)
      fprintf (stderr, "Number of tasks = %zu\n", num_tasks);

    if (num_tasks == 1)
      {
        fprintf (stderr, "Error. This test requires at least 2 tasks. Number of tasks in this job: %zu\n", num_tasks);
        return 1;
      }

    configuration.name = PAMI_CLIENT_NUM_CONTEXTS;
    result = PAMI_Client_query(client, &configuration, 1);

    if (result != PAMI_SUCCESS)
      {
        fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, result);
        return 1;
      }

    ncontexts = (configuration.value.intval < 2) ? 1 : 2;
    if (task_id == 0)
      fprintf (stderr, "maximum contexts = %zu, number of contexts used in this test = %zu\n", configuration.value.intval, ncontexts);

    result = PAMI_Context_createv(client, NULL, 0, context, ncontexts);

    if (result != PAMI_SUCCESS)
      {
        fprintf (stderr, "Error. Unable to create pami context(s). result = %d\n", result);
        return 1;
      }


    pami_dispatch_hint_t options = {};

    size_t i = 0;

    for (i = 0; i < ncontexts; i++)
      {
        pami_dispatch_callback_function fn;

        fprintf (stderr, "Before PAMI_Dispatch_set(%d) .. &_done = %p, _done = %zu\n", DISPATCH_ID, &_done, _done);
        fn.p2p = dispatch_send;
        result = PAMI_Dispatch_set (context[i],
                                    DISPATCH_ID,
                                    fn,
                                    NULL,
                                    options);

        if (result != PAMI_SUCCESS)
          {
            fprintf (stderr, "Error. Unable register pami dispatch. result = %d\n", result);
            return 1;
          }
      }
  } /* init done */

  if (task_id == 0)
    {
      fprintf (stdout, "PAMI_Put('simple') functional test %s\n", (ncontexts < 2) ? "" : "[crosstalk]");
      fprintf (stdout, "\n");

      _done = 0;
      unsigned i;

      for (i = 0; i < BUFFERSIZE; i++)
        _buffer[i] = PI * E * -1.0;

      struct iovec iov[2];
      iov[0].iov_base = (void *) & _done;
      iov[0].iov_len  = sizeof(size_t);
      iov[1].iov_base = (void *) _buffer;
      iov[1].iov_len  = BUFFERSIZE;

      pami_send_immediate_t parameters;
      parameters.dispatch        = DISPATCH_ID;
      parameters.header.iov_base = NULL;
      parameters.header.iov_len  = 0;
      parameters.data.iov_base   = iov;
      parameters.data.iov_len    = sizeof(struct iovec) * 2;
      PAMI_Endpoint_create (client, num_tasks - 1, ncontexts - 1, &parameters.dest);

      fprintf (stderr, "Before PAMI_Send_immediate()\n");
      PAMI_Send_immediate (context[0], &parameters);

      /* wait for the 'ack' */
      fprintf (stderr, "Wait for 'ack', _done (%p) = %zu\n", &_done, _done);

      while (!_done)
          PAMI_Context_advance (context[0], 100);

      unsigned n, loop = 0;
      unsigned offset = 0;

      for (n = 1; n < (BUFFERSIZE >> 1); n = n << 1)
        {
          double expect = ((loop++&0x01) == 0) ? PI : E;
          fprintf (stderr, "== validate %d byte (%d double) put ==\n", n*8, n);

          unsigned j;

          for (j = 0; j < n; j++)
            if (_buffer[offset+j] != expect)
              {
                fprintf (stderr, "_buffer[%04d] = %g .. <---- ERROR.  expected %g, n=%d\n", offset + j, _buffer[offset+j], expect, n);
                errors++;
              }
#if 0
            else
              {
                fprintf (stderr, "_buffer[%04d] = %g\n", offset + j, _buffer[offset+j]);
              }
#endif
          offset += n;
        }
    }
  else if (task_id == (num_tasks - 1))
    {
      /* wait for the 'put destination virtual address' */
      fprintf (stderr, "Wait for message, _done = %zu, contextid = %zu\n", _done, ncontexts - 1);

      while (!_done)
          PAMI_Context_advance (context[ncontexts-1], 100);

      unsigned i = 0;

      for (i = 0; i < (BUFFERSIZE >> 1); i++)
        {
          _buffer[i] = PI;
          _buffer[i + (BUFFERSIZE>>1)] = E;
        }


      volatile size_t active = 0;
      pami_put_simple_t parameters;
      memset(&parameters, 0, sizeof (parameters));
      parameters.rma.cookie  = (void *) & active;
      parameters.rma.done_fn = decrement;
      parameters.addr.remote = _dst[1].iov_base;
      PAMI_Endpoint_create (client, 0, 0, &parameters.rma.dest);

      /* put the source data to the destination buffer on the remote task */
      unsigned n, loop = 0;

      for (n = 1; n < (BUFFERSIZE >> 1); n = n << 1)
        {
          active++;

          parameters.rma.bytes   = sizeof(double) * n;
          parameters.addr.local  = (void *) & _buffer[(BUFFERSIZE>>1)*(loop++&0x01)]; /* selects PI or E source */

          fprintf (stderr, "before PAMI_Put(), parameters.rma.bytes = %zu, parameters.addr.remote = %p\n", parameters.rma.bytes, parameters.addr.remote);
          PAMI_Put (context[ncontexts - 1], &parameters);

          parameters.addr.remote = (void *)((uintptr_t) parameters.addr.remote + parameters.rma.bytes);
        }

      while (active > 0)
          PAMI_Context_advance (context[ncontexts-1], 100);

      /* put the 'ack' value to complete the test. */
      active++;

      size_t nonzero = 1;
      parameters.rma.bytes   = sizeof(size_t);
      parameters.addr.local  = (void *) & nonzero;
      parameters.addr.remote = _dst[0].iov_base;
      PAMI_Put (context[ncontexts - 1], &parameters);

      while (active > 0)
          PAMI_Context_advance (context[ncontexts-1], 100);

    }

  { /* cleanup */
    result = PAMI_Context_destroyv(context, ncontexts);

    if (result != PAMI_SUCCESS)
      {
        fprintf (stderr, "Error. Unable to destroy pami context. result = %d\n", result);
        return 1;
      }

    result = PAMI_Client_destroy(&client);

    if (result != PAMI_SUCCESS)
      {
        fprintf (stderr, "Error. Unable to destroy pami client. result = %d\n", result);
        return 1;
      }

    if (task_id == 0)
      {
        if (errors)
          fprintf (stdout, "Test completed with errors (%zu)\n", errors);
        else
          fprintf (stdout, "Test completed with success\n");
      }

  } /* cleanup done */

  return (errors != 0);
};
