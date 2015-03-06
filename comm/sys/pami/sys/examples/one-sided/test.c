/* ------------------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                                      */
/* Blue Gene/Q 5765-PER 5765-PRP                                             */
/* © Copyright IBM Corp. 2012 All Rights Reserved                            */
/* US Government Users Restricted Rights - Use, duplication or disclosure    */
/*   restricted by GSA ADP Schedule Contract with IBM Corp.                  */
/*                                                                           */
/* This software is available to you under the Eclipse Public License (EPL). */
/* ------------------------------------------------------------------------- */

#include <string.h>
#include <stdio.h>

#include <pami.h>

#include "simple_barrier.h"
#include "simple_async_progress.h"
#include "simple_memregion_registry.h"

#include "accumulate_data_functions.h"

#define ACCUMULATE_TEST_DISPATCH_ID    21

#define SIMPLE_MEMREGION_REGISTRY_HASH 1234

#define ASYNC_PROGRESS

void decrement (pami_context_t context, void * cookie, pami_result_t result)
{
  unsigned * value_ptr = (unsigned *) cookie;
  (*value_ptr)--;
}

typedef enum
{
  ACCUMULATE_TEST_SCALAR_SUM = 0,
  ACCUMULATE_TEST_VECTOR_SUM,
  ACCUMULATE_TEST_SCALAR_SUBTRACT,
  ACCUMULATE_TEST_VECTOR_SUBTRACT,
  ACCUMULATE_TEST_VECTOR_MAX_SUM,
  ACCUMULATE_TEST_VECTOR_MIN_SUM,
  ACCUMULATE_TEST_COUNT
} accumulate_test_t;

char * accumulate_test_name[] = {
  "ACCUMULATE_TEST_SCALAR_SUM",
  "ACCUMULATE_TEST_VECTOR_SUM",
  "ACCUMULATE_TEST_SCALAR_SUBTRACT",
  "ACCUMULATE_TEST_VECTOR_SUBTRACT",
  "ACCUMULATE_TEST_VECTOR_MAX_SUM",
  "ACCUMULATE_TEST_VECTOR_MIN_SUM",
  "ACCUMULATE_TEST_COUNT"
};


typedef struct accumulate_test_information
{
  pami_data_function data_fn[ACCUMULATE_TEST_COUNT];
  void *             data_cookie[ACCUMULATE_TEST_COUNT];
  struct iovec       data_buffer;
  double             scalar;
} accumulate_test_information_t;


void accumulate_test_done_cb (pami_context_t   context,
                              void           * cookie,
                              pami_result_t    result)
{
  accumulate_test_t test = (accumulate_test_t) cookie;
  fprintf (stdout, "(%03d) end accumulate test \"%s\"\n", __LINE__, accumulate_test_name[test]);
}


void accumulate_test_recv_cb (pami_context_t    context,
                              void            * cookie,
                              const void      * header_addr,
                              size_t            header_size,
                              const void      * pipe_addr,
                              size_t            data_size,
                              pami_endpoint_t   origin,
                              pami_recv_t     * recv)
{
  accumulate_test_t test = *((accumulate_test_t *) header_addr);
  fprintf (stdout, "(%03d) begin accumulate test \"%s\"\n", __LINE__, accumulate_test_name[test]);

  accumulate_test_information_t * info = (accumulate_test_information_t *) cookie;

  // ...

  recv->cookie      = (void *) test;
  recv->local_fn    = accumulate_test_done_cb;
  recv->type        = PAMI_TYPE_DOUBLE;
  recv->addr        = info->data_buffer.iov_base;
  recv->offset      = 0;
  recv->data_fn     = info->data_fn[test];
  recv->data_cookie = info->data_cookie[test];


  return;
}


void test_fn (int argc, char * argv[], pami_client_t client, pami_context_t context[])
{
  int num_doubles = 16;

  if (argc > 1) num_doubles = atoi (argv[1]);

  size_t num_tasks = size (client);
  pami_task_t my_task_id = task (client);
  pami_task_t target_task_id = num_tasks - 1;
  pami_task_t origin_task_id = 0;


  /*
   * Initialize the accumulate test information.  This information is specified
   * as the cookie for the accumulate test dispatch for use by the accumulate
   * test receive callback.
   */
  accumulate_test_information_t test_info;

  test_info.data_buffer.iov_base = malloc (sizeof(double) * num_doubles);
  test_info.data_buffer.iov_len = sizeof(double) * num_doubles;
  test_info.scalar = 1.2;

  test_info.data_fn[ACCUMULATE_TEST_SCALAR_SUM] = accumulate_scalar_sum_data_function;
  test_info.data_cookie[ACCUMULATE_TEST_SCALAR_SUM] = (void *) & test_info.scalar;

  test_info.data_fn[ACCUMULATE_TEST_VECTOR_SUM] = accumulate_vector_sum_data_function;
  test_info.data_cookie[ACCUMULATE_TEST_VECTOR_SUM] = malloc (sizeof(double) * num_doubles);

  test_info.data_fn[ACCUMULATE_TEST_SCALAR_SUBTRACT] = accumulate_scalar_subtract_data_function;
  test_info.data_cookie[ACCUMULATE_TEST_SCALAR_SUBTRACT] = (void *) & test_info.scalar;

  test_info.data_fn[ACCUMULATE_TEST_VECTOR_SUBTRACT] = accumulate_vector_subtract_data_function;
  test_info.data_cookie[ACCUMULATE_TEST_VECTOR_SUBTRACT] = malloc (sizeof(double) * num_doubles);

  test_info.data_fn[ACCUMULATE_TEST_VECTOR_MAX_SUM] = accumulate_vector_max_sum_data_function;
  test_info.data_cookie[ACCUMULATE_TEST_VECTOR_MAX_SUM] = malloc (sizeof(double) * num_doubles);

  test_info.data_fn[ACCUMULATE_TEST_VECTOR_MIN_SUM] = accumulate_vector_min_sum_data_function;
  test_info.data_cookie[ACCUMULATE_TEST_VECTOR_MIN_SUM] = malloc (sizeof(double) * num_doubles);


  /*
   * Register the accumulate dispatch; needed on both
   * contexts to enable "crosstalk".
   */
  pami_dispatch_hint_t acc_hint = {0};
  acc_hint.recv_immediate = PAMI_HINT_DISABLE;
  pami_dispatch_callback_function acc_dispatch;
  acc_dispatch.p2p = accumulate_test_recv_cb;
  PAMI_Dispatch_set (context[0], ACCUMULATE_TEST_DISPATCH_ID, acc_dispatch, (void *) & test_info, acc_hint);
  PAMI_Dispatch_set (context[1], ACCUMULATE_TEST_DISPATCH_ID, acc_dispatch, (void *) & test_info, acc_hint);


  /*
   * Create a memory region registry for managing memory region 'collections',
   * or 'windows',
   *
   * The first context will be used to manage memory regions for all endpoints.
   */
  simple_memregion_registry_t * simple_memregion_registry =
    simple_memregion_registry_init (client, context[0], 0);


  /*
   * Create a local memregion for each context.
   *
   * Note that both memregions will describe the same memory location. This is
   * necessary when writing portable, platform independent code as the physical
   * hardware underlying the contexts may, or may not, require separate memory
   * pinning.
   */
  pami_memregion_t local_memregion[2];

  void * local_data_addr = malloc (sizeof(double) * num_doubles);

  simple_memregion_registry_add (simple_memregion_registry,
                                 context, 0,
                                 SIMPLE_MEMREGION_REGISTRY_HASH,
                                 local_data_addr,
                                 sizeof(double) * num_doubles,
                                 & local_memregion[0]);

  simple_memregion_registry_add (simple_memregion_registry,
                                 context, 1,
                                 SIMPLE_MEMREGION_REGISTRY_HASH,
                                 local_data_addr,
                                 sizeof(double) * num_doubles,
                                 & local_memregion[1]);


  /*
   * Open the async progress extension and enable async progress only on
   * the second context.
   */
  pami_extension_t async_progress_extension = simple_async_progress_open (client);
  simple_async_progress_enable (async_progress_extension, context[1]);


  /*
   * Perform a simple, blocking, 'world' barrier before the accumulate
   * test begins using the first context. Recall that the first context
   * does NOT have async progress enabled.
   */
  simple_barrier(client, context[0]);


  if (my_task_id == target_task_id)
    {
      /*
       * This is the "passive target" task.
       *
       * Do "something" besides communication for a little bit.
       */
      fprintf (stdout, "(%03d) 'passive target' begin sleep\n", __LINE__);
      sleep(2);
      fprintf (stdout, "(%03d) 'passive target' end sleep\n", __LINE__);
    }
  else if (my_task_id == origin_task_id)
    {
      /*
       * This is the "active origin" task.
       *
       * Query the memregion registry for the remote memregion. Advance the
       * context until the memregion information is received from the remote.
       */
      pami_memregion_t * remote_memregion = NULL;
      pami_endpoint_t remote_endpoint;
      PAMI_Endpoint_create (client, target_task_id, 1, & remote_endpoint);

      while (remote_memregion == NULL)
      {
        PAMI_Context_advance (context[0], 1);
        remote_memregion =
          simple_memregion_registry_query (simple_memregion_registry,
                                           remote_endpoint,
                                           SIMPLE_MEMREGION_REGISTRY_HASH);
      }

      {
        /*
         * Use rdma put to initialize the remote buffer with the local data.
         */
        volatile unsigned rput_active = 1;
        pami_rput_simple_t rput_parameters = {0};
        rput_parameters.rma.dest = remote_endpoint;
        rput_parameters.rma.bytes = num_doubles * sizeof(double);
        rput_parameters.rdma.local.mr = & local_memregion[0];
        rput_parameters.rdma.local.offset = 0;
        rput_parameters.rdma.remote.mr = remote_memregion;
        rput_parameters.rdma.remote.offset = 0;
        rput_parameters.put.rdone_fn = decrement;
        rput_parameters.rma.cookie = (void *) & rput_active;

        PAMI_Rput (context[0], & rput_parameters);

        while (rput_active > 0)
          PAMI_Context_advance (context[0], 1);
      }

      {
        volatile unsigned send_active = 0;
        accumulate_test_t test_id = ACCUMULATE_TEST_SCALAR_SUM;
        pami_send_t send_parameters = {0};
        PAMI_Endpoint_create (client, target_task_id, 1, & send_parameters.send.dest);
        send_parameters.send.dispatch = ACCUMULATE_TEST_DISPATCH_ID;
        send_parameters.send.header.iov_len = sizeof (accumulate_test_t);
        send_parameters.send.header.iov_base = (void *) & test_id;
        send_parameters.send.data.iov_base = test_info.data_buffer.iov_base;
        send_parameters.send.data.iov_len = test_info.data_buffer.iov_len;
        send_parameters.events.remote_fn = decrement;
        send_parameters.events.cookie = (void *) & send_active;

        for (test_id = ACCUMULATE_TEST_SCALAR_SUM; test_id < ACCUMULATE_TEST_COUNT; test_id++)
          {
            send_active = 1;

            fprintf (stdout, "(%03d) sending data buffer for accumulate test \"%s\"\n", __LINE__, accumulate_test_name[test_id]);
            PAMI_Send (context[0], & send_parameters);

            fprintf (stdout, "(%03d) waiting for remote completion of data buffer sent for accumulate test \"%s\"\n", __LINE__, accumulate_test_name[test_id]);
            while (send_active > 0)
              PAMI_Context_advance (context[0], 1);
            fprintf (stdout, "(%03d) data buffer received on remote for accumulate test \"%s\"\n", __LINE__, accumulate_test_name[test_id]);
          }
      }

      {
        /*
         * Use rdma get to retrieve the remote buffer and compare results.
         */
        volatile unsigned rget_active = 1;
        pami_rget_simple_t rget_parameters = {0};
        rget_parameters.rma.dest = remote_endpoint;
        rget_parameters.rma.done_fn = decrement;
        rget_parameters.rma.cookie = (void *) & rget_active;
        rget_parameters.rma.bytes = sizeof(double) * num_doubles;
        rget_parameters.rdma.local.mr = & local_memregion[0];
        rget_parameters.rdma.local.offset = 0;
        rget_parameters.rdma.remote.mr = remote_memregion;
        rget_parameters.rdma.remote.offset = 0;

        PAMI_Rget (context[0], & rget_parameters);

        while (rget_active > 0)
          PAMI_Context_advance (context[0], 1);
      }

    }
  else
    {
      /*
       * All other tasks, if any, do nothing and simply enter the barrier.
       */
    }

  simple_barrier (client, context[0]);


  /*
   * Disable async progress on the second context and close the extension.
   */
  simple_async_progress_disable (async_progress_extension, context[1]);
  simple_async_progress_close (async_progress_extension);


  /*
   * Do cleanup ?
   */


  return;
}
