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


/**
 * \brief Simple 'unsigned int' decrementer callback function
 */
void decrement (pami_context_t context, void * cookie, pami_result_t result)
{
  unsigned * value_ptr = (unsigned *) cookie;
  (*value_ptr)--;
}


/**
 * \brief Accumulate header information.
 */
typedef struct accumulate_header
{
  simple_memregion_registry_data_t destination;
  pami_data_function               data_fn;
  size_t                           serialized_type_size;
  uint8_t                          serialized_type[512-sizeof(simple_memregion_registry_data_t)-sizeof(size_t)-sizeof(pami_data_function)];
} accumulate_header_t;


/**
 * \brief The accumulate receive done callback.
 *
 * The only purpose of this callback is to destroy the type that was
 * deserialized in the initial receive handler.
 */
void accumulate_test_done_cb (pami_context_t   context,
                              void           * cookie,
                              pami_result_t    result)
{
  pami_type_t * type = (pami_type_t *) cookie;
  fprintf (stdout, "(%03d) end accumulate test .. destroy type (result = %d).\n", __LINE__, result);

  PAMI_Type_destroy (type);
  fprintf (stdout, "(%03d) type is destroyed.\n", __LINE__);
}


/**
 * \brief One-sided accumulate receive callback
 *
 * All data necessary to perform the accumulate is provided by the accumulate
 * originator.
 */
void accumulate_test_recv_cb (pami_context_t    context,
                              void            * cookie,
                              const void      * header_addr,
                              size_t            header_size,
                              const void      * pipe_addr,
                              size_t            data_size,
                              pami_endpoint_t   origin,
                              pami_recv_t     * recv)
{
  accumulate_header_t * header = (accumulate_header_t *) header_addr;

  fprintf (stdout, "(%03d) begin accumulate test at vaddr: %p\n", __LINE__, header->destination.vaddr);

  /*
   * Deserialize the type. This also completes the type.
   *
   * A pointer to the type is specified as the receive done callback so that
   * the type can be destroyed.
   */
  
  pami_type_t type;
  pami_result_t result = PAMI_ERROR;
  result = PAMI_Type_deserialize (&type, header->serialized_type, header->serialized_type_size);

  recv->cookie      = (void *) type;
  recv->local_fn    = accumulate_test_done_cb;
  recv->type        = type;
  recv->addr        = header->destination.vaddr;
  recv->offset      = 0;
  recv->data_fn     = header->data_fn;
  recv->data_cookie = NULL;

  return;
}


void test_fn (int argc, char * argv[], pami_client_t client, pami_context_t context[])
{
  pami_task_t my_task_id = task (client);
  size_t num_tasks = size (client);
  pami_task_t target_task_id = num_tasks - 1;
  pami_task_t origin_task_id = 0;

  int k_chunks  = 16;
  int n_doubles = 16;
  int stride_m  = (n_doubles * 2);

  char * program_name = argv[0];
  while (argc > 1)
  {
    if (strcmp ("--chunks", argv[1]) == 0)
    {
      argc--; argv++;
      if (argc == 1)
      {
        if (my_task_id == 0)
          fprintf (stderr, "Error. No value specified for '--chunks' parameter.\n");
        exit (1);
      }
      k_chunks = atoi (argv[1]);
    }
    else if (strcmp ("--doubles", argv[1]) == 0)
    {
      argc--; argv++;
      if (argc == 1)
      {
        if (my_task_id == 0)
          fprintf (stderr, "Error. No value specified for '--doubles' parameter.\n");
        exit (1);
      }
      n_doubles = atoi (argv[1]);
    }
    else if (strcmp ("--stride", argv[1]) == 0)
    {
      argc--; argv++;
      if (argc == 1)
      {
        if (my_task_id == 0)
          fprintf (stderr, "Error. No value specified for '--stride' parameter.\n");
        exit (1);
      }
      stride_m = atoi (argv[1]);
    }
    else
    {
      if (my_task_id == 0)
      {
        fprintf (stderr, "Unknown option: '%s'\n", argv[1]);
        fprintf (stderr, "\n");
        fprintf (stderr, "Usage:\n");
        fprintf (stderr, "  %s [--chunks k] [--doubles n] [--stride m]\n", program_name);
        exit (1);
      }
    }
  }

  /*
   * Create the non-contiguous type. The type is created for a specific count
   * of doubles, at a specific byte stride. In this example the atom size is a
   * double so that the accumulate test can perform a "double sum" operation.
   */
  pami_type_t type;
  {
    pami_result_t result = PAMI_ERROR;

    result = PAMI_Type_create (&type);
    if (result != PAMI_SUCCESS)
      fprintf (stderr, "(%03d) result = %d\n", __LINE__, result);

    result = PAMI_ERROR;
    result = PAMI_Type_add_typed (type, PAMI_TYPE_DOUBLE, 0, n_doubles, stride_m);
    if (result != PAMI_SUCCESS)
      fprintf (stderr, "(%03d) result = %d\n", __LINE__, result);

    result = PAMI_ERROR;
    result = PAMI_Type_complete (type, sizeof(double));
    if (result != PAMI_SUCCESS)
      fprintf (stderr, "(%03d) result = %d\n", __LINE__, result);
  }

  /*
   * Register the accumulate dispatch; needed on both
   * contexts to enable "crosstalk".
   */
  pami_dispatch_hint_t acc_hint = {0};
  acc_hint.recv_immediate = PAMI_HINT_DISABLE;
  pami_dispatch_callback_function acc_dispatch;
  acc_dispatch.p2p = accumulate_test_recv_cb;
  PAMI_Dispatch_set (context[0], ACCUMULATE_TEST_DISPATCH_ID, acc_dispatch, NULL, acc_hint);
  PAMI_Dispatch_set (context[1], ACCUMULATE_TEST_DISPATCH_ID, acc_dispatch, NULL, acc_hint);


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

  size_t extent = stride_m * k_chunks;
  void * local_data_addr = malloc (sizeof(double) * extent);
  double * double_ptr = (double *) local_data_addr;
  {
    unsigned i;
    for (i = 0; i < extent; i++)
      double_ptr[i] = -1.0;
  }
  

  simple_memregion_registry_add (simple_memregion_registry,
                                 context, 0,
                                 SIMPLE_MEMREGION_REGISTRY_HASH,
                                 local_data_addr,
                                 sizeof(double) * extent,
                                 & local_memregion[0]);

  simple_memregion_registry_add (simple_memregion_registry,
                                 context, 1,
                                 SIMPLE_MEMREGION_REGISTRY_HASH,
                                 local_data_addr,
                                 sizeof(double) * extent,
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
      simple_memregion_registry_data_t * remote_memregion_data = NULL;
      pami_endpoint_t remote_endpoint;
      PAMI_Endpoint_create (client, target_task_id, 1, & remote_endpoint);

      while (remote_memregion_data == NULL)
      {
        PAMI_Context_advance (context[0], 1);
        remote_memregion_data =
          simple_memregion_registry_query (simple_memregion_registry,
                                           remote_endpoint,
                                           SIMPLE_MEMREGION_REGISTRY_HASH);
      }

      {
        /*
         * Use rdma put to initialize the remote non-contiguous data with the
         * local contiguous data.
         */

        double contiguous_data[n_doubles*k_chunks];
        unsigned i;
        for (i = 0; i < (n_doubles*k_chunks); i++)
          contiguous_data[i] = i / 10.0;

        size_t bytes_out = 0;
        pami_memregion_t contiguous_memregion;
        PAMI_Memregion_create (context[0], (void *) contiguous_data,
                               sizeof(contiguous_data), &bytes_out,
                               &contiguous_memregion);

        volatile unsigned rput_active = 1;
        pami_rput_typed_t rput_parameters = {0};
        rput_parameters.rma.dest = remote_endpoint;
        rput_parameters.rma.bytes = sizeof(contiguous_data);
        rput_parameters.rdma.local.mr = & contiguous_memregion;
        rput_parameters.rdma.local.offset = 0;
        rput_parameters.rdma.remote.mr = & remote_memregion_data->mr;
        rput_parameters.rdma.remote.offset = 0;
        rput_parameters.rma.cookie = (void *) & rput_active;
        rput_parameters.type.local = PAMI_TYPE_DOUBLE;
        rput_parameters.type.remote = type;

        /*
         * A 'remote done' function is specified for the put operation
         * so the put does not complete locally until all of the data
         * has been received on the remote. This has the same effect
         * as explicitly calling the PAMI_Fence function.
         */
        rput_parameters.put.rdone_fn = decrement;

        PAMI_Rput_typed (context[0], & rput_parameters);

        while (rput_active > 0)
          PAMI_Context_advance (context[0], 1);
      }

      {
        /*
         * Initialize a contiguous data buffer on the stack to accumulate
         * on to the non-contiguous remote location.
         */
        double contiguous_data[n_doubles*k_chunks];
        unsigned i;
        for (i = 0; i < (n_doubles*k_chunks); i++)
          contiguous_data[i] = i / 10.0;

        /*
         * Serialize the type in order to send it over to the remote endpoint.
         */
        void * serialized_type_addr = NULL;
        size_t serialized_type_size = 0;
        pami_result_t result = PAMI_ERROR;
        result = PAMI_Type_serialize (type, &serialized_type_addr, &serialized_type_size);
        if (result != PAMI_SUCCESS)
          fprintf (stderr, "result = %d, serialized_type_addr = %p, serialized_type_size = %zu\n",
                   result, serialized_type_addr, serialized_type_size);

        /*
         * The header data for the accumulate operation contains the destination
         * memory region region information as well as the serialized type and
         * data function to apply on the remote endpoint when receiving the data
         */
        accumulate_header_t header;
        header.destination = *remote_memregion_data;
        header.data_fn = PAMI_DATA_SUM;
        header.serialized_type_size = serialized_type_size;
        memcpy ((void *) header.serialized_type, serialized_type_addr, serialized_type_size);
        
        volatile unsigned send_active = 1;
        pami_send_t send_parameters = {0};
        send_parameters.send.dest = remote_endpoint;
        send_parameters.send.dispatch = ACCUMULATE_TEST_DISPATCH_ID;
        send_parameters.send.header.iov_len = sizeof (accumulate_header_t);
        send_parameters.send.header.iov_base = (void *) & header;
        send_parameters.send.data.iov_base = contiguous_data;
        send_parameters.send.data.iov_len = sizeof(contiguous_data);
        send_parameters.events.remote_fn = decrement;
        send_parameters.events.cookie = (void *) & send_active;

        fprintf (stdout, "(%03d) sending data buffer for accumulate test.\n", __LINE__);
        PAMI_Send (context[0], & send_parameters);

        fprintf (stdout, "(%03d) waiting for remote completion of data buffer sent for accumulate test.\n", __LINE__);
        while (send_active > 0)
          PAMI_Context_advance (context[0], 1);
        fprintf (stdout, "(%03d) data buffer received on remote for accumulate test.\n", __LINE__);
      }

      {
        /*
         * Use rdma get to retrieve the remote non-contiguous data into the
         * local contiguous data destination and compare results.
         */
        volatile unsigned rget_active = 1;
        pami_rget_typed_t rget_parameters = {0};
        rget_parameters.rma.dest = remote_endpoint;
        rget_parameters.rma.done_fn = decrement;
        rget_parameters.rma.cookie = (void *) & rget_active;
        rget_parameters.rma.bytes = sizeof(double) * n_doubles;
        rget_parameters.rdma.local.mr = & local_memregion[0];
        rget_parameters.rdma.local.offset = 0;
        rget_parameters.rdma.remote.mr = & remote_memregion_data->mr;
        rget_parameters.rdma.remote.offset = 0;
        rget_parameters.type.local = PAMI_TYPE_DOUBLE;
        rget_parameters.type.remote = type;

        PAMI_Rget_typed (context[0], & rget_parameters);

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
