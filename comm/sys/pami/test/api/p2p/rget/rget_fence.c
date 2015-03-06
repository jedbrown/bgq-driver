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
 * \file test/api/p2p/rget/rget_fence.c
 * \brief PAMI_Rget() throughput test using PAMI_Fence_endpoint
 **/

#include <pami.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>

#include "../../init_util.h"

#define ITERATIONS 100
#define BUFFERSIZE 10240*32
#define TEST_START_NBYTES 512
#define WARMUP
#define TEST_COMPLETE_DISPATCH_ID 10
#define MR_EXCHANGE_DISPATCH_ID   11

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

typedef struct
{
  pami_task_t task;
  char        name[1024];
} testinfo_t;

/* --------------------------------------------------------------- */
typedef struct pami_extension_torus_information
{
  size_t   dims;
  size_t * coord;
  size_t * size;
  size_t * torus;
} pami_extension_torus_information_t;

typedef const pami_extension_torus_information_t * (*pami_extension_torus_information_fn) ();
typedef pami_result_t (*pami_extension_torus_task2torus_fn) (pami_task_t, size_t[]);
typedef pami_result_t (*pami_extension_torus_torus2task_fn) (size_t[], pami_task_t *);
/* --------------------------------------------------------------- */


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
 * \brief test completion dispatch function
 */
void test_complete_fn (
  pami_context_t    context,      /**< IN: PAMI context */
  void            * cookie,       /**< IN: dispatch cookie */
  const void      * header,       /**< IN: header address */
  size_t            header_size,  /**< IN: header size */
  const void      * data,         /**< IN: address of PAMI pipe buffer */
  size_t            data_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t   origin,
  pami_recv_t     * recv)         /**< OUT: receive message structure */
{
  unsigned * value = (unsigned *) cookie;
  /*fprintf (stderr, "test_complete_fn() cookie = %p, %d => %d\n", cookie, *value, *value - 1);*/
  --*value;
  return;
}

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
  setbuf (stdout, NULL);
  setbuf (stderr, NULL);

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

  testinfo_t target[1024];

  int arg, ntargets = 0;

  for (arg = 1; (arg < 1024) && (arg < argc); arg++)
    {
      target[ntargets].task = (size_t) strtol (argv[arg], NULL, 10);
      target[ntargets].name[0] = 0;

      if (target[ntargets].task < num_tasks)
        ntargets++;
      else if (me == 0)
        fprintf (stderr, "Skipping invalid target task parameter (%d) specified on the command line .. Only %zu tasks in this job.\n", target[ntargets].task, num_tasks);
    }

  if (argc == 1)
    {
      /* no arguments .. set default */
      ntargets = 1;
      target[0].task = num_tasks - 1;
      target[0].name[0] = 0;
    }

  pami_configuration_t configuration;
  configuration.name = PAMI_CLIENT_WTICK;
  PAMI_Client_query(client, &configuration, 1);
  double tick = configuration.value.doubleval;


  /* ------------------------------------------------------------------------ */
  pami_extension_t extension;
  pami_result_t torus_extension_status;
  torus_extension_status = PAMI_Extension_open (client, "EXT_torus_network", &extension);

  if (torus_extension_status == PAMI_SUCCESS)
    {
      pami_extension_torus_information_fn pamix_torus_info =
        (pami_extension_torus_information_fn) PAMI_Extension_symbol (extension, "information");

      if (pamix_torus_info == (void *)NULL)
        {
          fprintf (stderr, "Error. The \"EXT_torus_network\" extension function \"information\" is not implemented.\n");
          return 1;
        }

      const pami_extension_torus_information_t * info = pamix_torus_info ();

      pami_extension_torus_task2torus_fn pamix_torus_task2torus =
        (pami_extension_torus_task2torus_fn) PAMI_Extension_symbol (extension, "task2torus");

      if (pamix_torus_task2torus == (void *)NULL)
        {
          fprintf (stderr, "Error. The \"EXT_torus_network\" extension function \"task2torus\" is not implemented.\n");
          return 1;
        }

      unsigned n;

      for (n = 0; n < ntargets; n++)
        {
          size_t coord[1024];
          pamix_torus_task2torus (target[n].task, coord);
          unsigned nchars, i;

          for (nchars = i = 0; i < (info->dims - 1); i++)
            nchars += snprintf (&target[n].name[nchars], 1023 - nchars, "%zu,", coord[i]);

          nchars += snprintf (&target[n].name[nchars], 1023 - nchars, "%zu", coord[i]);
        }
    }

  /* ------------------------------------------------------------------------ */


  /* ------------------------------------------------------------------------ */
  /* Set up the 'test completion' dispatch function.                          */
  /* ------------------------------------------------------------------------ */
  volatile unsigned test_active = (me == 0) ? num_tasks - 1 : 1;
  pami_dispatch_callback_function fn;
  fn.p2p = test_complete_fn;
  pami_dispatch_hint_t options = {};

  result = PAMI_Dispatch_set (context,
                              TEST_COMPLETE_DISPATCH_ID,
                              fn,
                              (void *) & test_active,
                              options);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable register pami dispatch function \"test_complete_fn()\" with dispatch id %d. result = %d\n", TEST_COMPLETE_DISPATCH_ID, result);
      return 1;
    }

  /* ------------------------------------------------------------------------ */



  mr_exchange_t * exchange =
    (mr_exchange_t *) malloc (sizeof(mr_exchange_t) + sizeof(mr_t) * num_tasks);

  /* Initialize the exchange information */
  exchange->counter = num_tasks;

  mr_t * info = exchange->task;

  fn.p2p = mr_exchange_fn;
  result = PAMI_Dispatch_set (context,
                              MR_EXCHANGE_DISPATCH_ID,
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
      parameters.dispatch        = MR_EXCHANGE_DISPATCH_ID;
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
   * Begin the test - Task 0 will use PAMI_Rget to performance test against
   * the target tasks specified on the command line.
   * **************************************************************************/

  /* Display some test header information */
  if (me == 0)
    {
      char str[3][1024];
      int index[3];
      index[0] = 0;
      index[1] = 0;
      index[2] = 0;

      index[0] += sprintf (&str[0][index[0]], "#          ");
      index[1] += sprintf (&str[1][index[1]], "#          ");
      index[2] += sprintf (&str[2][index[2]], "#%9s ", "bytes");

      for (i = 0; i < ntargets; i++)
        {
          index[0] += sprintf (&str[0][index[0]], "[--- task %3d ---] ", target[i].task);

          if (target[i].name[0] != 0)
            {
              index[1] += sprintf (&str[1][index[1]], "[   %11s  ] ", target[i].name);
            }

          index[2] += sprintf (&str[2][index[2]], "%8s %8s  ", "cycles", "usec");
        }

      fprintf (stdout, "#\n");
      fprintf (stdout, "# PAMI_Rget() non-blocking throughput performance test\n");
      fprintf (stdout, "#\n");
      fprintf (stdout, "# The origin task 0 invokes PAMI_Rget() many times to the\n");
      fprintf (stdout, "# target task followed by a single PAMI_Fence_endpoint().\n");
      fprintf (stdout, "#\n");
      fprintf (stdout, "# The target task(s) are specified on the command line.\n");
      fprintf (stdout, "# If no target task(s) are specified, then the default target\n");
      fprintf (stdout, "# task is the number of global tasks - 1.\n");
      fprintf (stdout, "#\n");
      fprintf (stdout, "%s\n", str[0]);

      if (index[1] > 11)
        fprintf (stdout, "%s\n", str[1]);

      fprintf (stdout, "%s\n", str[2]);
      fflush (stdout);

      volatile unsigned active;

      pami_rget_simple_t parameters;
      parameters.rma.hints          = (pami_send_hint_t) {0};
      parameters.rma.cookie         = (void *) & active;
      parameters.rma.done_fn        = NULL;
      parameters.rdma.local.mr      = &info[me].mr;
      parameters.rdma.local.offset  = 0;
      parameters.rdma.remote.offset = 0;

      unsigned long long t0, t1, cycles;
      double usec;

      size_t nbytes = TEST_START_NBYTES;

      for (; nbytes <= BUFFERSIZE; nbytes = nbytes * 3 / 2 + 1)
        {
          char str[10240];
          int index = 0;
          index += sprintf (&str[index], "%10zd ", nbytes);

          parameters.rma.bytes = nbytes;

          unsigned n;

          for (n = 0; n < ntargets; n++)
            {
              parameters.rdma.remote.mr = &info[target[n].task].mr;
              PAMI_Endpoint_create (client, target[n].task, 0, &parameters.rma.dest);

#ifdef WARMUP
	      result = PAMI_Fence_begin (context);
	      if (result != PAMI_SUCCESS)
		{
		  fprintf (stderr, "Error. PAMI_Fence_begin failed with result = %d\n", result);
		  return 1;
		}

              for (i = 0; i < ITERATIONS; i++)
                {
                  PAMI_Rget (context, &parameters);
                }

              active = 1;
              result = PAMI_Fence_endpoint (context, decrement, (void *) & active, parameters.rma.dest);
	      if (result != PAMI_SUCCESS)
		{
		  fprintf (stderr, "Error. PAMI_Fence_endpoint failed with result = %d\n", result);
		  return 1;
		}

              while (active > 0)
                PAMI_Context_advance (context, 100);

	      result = PAMI_Fence_end (context);
	      if (result != PAMI_SUCCESS)
		{
		  fprintf (stderr, "Error. PAMI_Fence_end failed with result = %d\n", result);
		  return 1;
		}

#endif
              t0 = PAMI_Wtimebase (client);

	      result = PAMI_Fence_begin (context);
	      if (result != PAMI_SUCCESS)
		{
		  fprintf (stderr, "Error. PAMI_Fence_begin failed with result = %d\n", result);
		  return 1;
		}

              for (i = 0; i < ITERATIONS; i++)
                {
                  PAMI_Rget (context, &parameters);
                }
                
              active = 1;
              result = PAMI_Fence_endpoint (context, decrement, (void *) & active, parameters.rma.dest);
	      if (result != PAMI_SUCCESS)
		{
		  fprintf (stderr, "Error. PAMI_Fence_endpoint failed with result = %d\n", result);
		  return 1;
		}

              while (active > 0)
                PAMI_Context_advance (context, 100);

	      result = PAMI_Fence_end (context);
	      if (result != PAMI_SUCCESS)
		{
		  fprintf (stderr, "Error. PAMI_Fence_end failed with result = %d\n", result);
		  return 1;
		}

              t1 = PAMI_Wtimebase(client);

              cycles = (t1 - t0) / ITERATIONS;
              usec   = cycles * tick * 1000000.0;
              index += sprintf (&str[index], "%8lld %8.4f  ", cycles, usec);
            }

          fprintf (stdout, "%s\n", str);


        }

      {
        size_t i;

        pami_send_t parameters;
        parameters.send.dispatch        = TEST_COMPLETE_DISPATCH_ID;
        parameters.send.header.iov_base = (void *) & test_active;
        parameters.send.header.iov_len  = 1;
        parameters.send.data.iov_base   = (void *) & test_active;
        parameters.send.data.iov_len    = 1;
        parameters.events.cookie        = (void *) & test_active;
        parameters.events.local_fn      = decrement;
        parameters.events.remote_fn     = (void *) NULL;

        for (i = 1; i < num_tasks; i++)
          {
            PAMI_Endpoint_create (client, i, 0, &parameters.send.dest);
            result = PAMI_Send (context, &parameters);
          }
      }
    }

  /* Wait until the test is completed */
  while (test_active)
    PAMI_Context_advance (context, 100);

  rc = pami_shutdown(&client, context, &num_contexts);

  if (rc == 1)
    return 1;

  return 0;
};
