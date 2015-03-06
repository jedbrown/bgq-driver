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
 * \file test/api/p2p/rput/simple_rput_func.c
 * \brief Simple point-to-point PAMI_Rget() test
 *
 * This test implements a very simple "rendezvous" communication and
 * depends on a functional PAMI_Send_immediate() function.
 *
 * Note: This example only works when RDMA is used.
 */

#include <pami.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/*#define TEST_CROSSTALK */

/*#define USE_SHMEM_OPTION */
/*#define NO_SHMEM_OPTION */

#define DISPATCH_ID_MEMREGION 10

#define BUFFERSIZE 16
#define MAXTASKS 64

#undef TRACE_ERR
#define TRACE_ERR(x) /* fprintf x */


static pami_send_hint_t null_send_hint;

typedef struct
{
  volatile size_t  ping[MAXTASKS];
  volatile size_t  pong[MAXTASKS];
  pami_memregion_t mr[MAXTASKS];
  volatile size_t  exchange;
  volatile size_t  active;
  size_t           zero;
} info_t;

info_t  _info[2];

static void decrement (pami_context_t   context,
                       void          * cookie,
                       pami_result_t    result)
{
  size_t * value = (size_t *) cookie;
  fprintf (stderr, "  decrement() cookie = %p, %zu => %zu\n", cookie, *value, *value-1);
  --*value;
}

void print_data (void * addr, size_t bytes)
{
  uint8_t * p = (uint8_t *) addr;
  size_t i;
  for (i=0; i<bytes; i+=4)
    {
      fprintf (stderr, "  addr[%04zu] (%p): %02x %02x %02x %02x\n", i, &p[i], p[i], p[i+1], p[i+2], p[i+3]);
    }
}

static void dispatch_mr (
    pami_context_t       context,      /**< IN: PAMI context */
    void               * cookie,       /**< IN: dispatch cookie */
    const void         * header_addr,  /**< IN: header address */
    size_t               header_size,  /**< IN: header size */
    const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
    size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
    pami_endpoint_t origin,
pami_recv_t        * recv)        /**< OUT: receive message structure */
{
  size_t cid = (size_t) cookie;
  fprintf (stderr, ">> 'mr' dispatch function %zu.\n", cid);

  pami_task_t tid = *((pami_task_t *) header_addr);
  fprintf (stderr, "   'mr' dispatch function %zu. tid = %d, header_addr = %p, header_size = %zu, pipe_addr = %p, pipe_size = %zu\n", cid, tid, header_addr, header_size, pipe_addr, pipe_size);
  memcpy ((void *) &_info[cid].mr[tid], pipe_addr, sizeof(pami_memregion_t));
  fprintf (stderr, "   'mr' dispatch function %zu. tid = %d\n", cid, tid);

  _info[cid].exchange--;

  fprintf (stderr, "<< 'mr' dispatch function %zu.\n", cid);

  return;
}


int main (int argc, char ** argv)
{
  pami_client_t client;
  pami_context_t context[2];

  char                  cl_string[] = "TEST";
  pami_result_t result = PAMI_ERROR;

  result = PAMI_Client_create (cl_string, &client, NULL, 0);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to create pami client. result = %d\n", result);
    return 1;
  }

#ifdef TEST_CROSSTALK
  size_t num = 2;
#else
  size_t num = 1;
#endif
  result = PAMI_Context_createv(client, NULL, 0, context, num);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to create pami context(s). result = %d\n", result);
    return 1;
  }

  pami_configuration_t configuration;

  configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query(client, &configuration,1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, result);
    return 1;
  }
  pami_task_t task_id = configuration.value.intval;
  fprintf (stderr, "My task id = %d\n", task_id);

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration,1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, result);
    return 1;
  }
  size_t num_tasks = configuration.value.intval;
  fprintf (stderr, "Number of tasks = %zu\n", num_tasks);
  if (num_tasks < 2)
  {
    fprintf (stderr, "Error. This test requires at least 2 tasks. Number of tasks in this job: %zu\n", num_tasks);
    return 1;
  }
  if (num_tasks > MAXTASKS)
  {
    fprintf (stderr, "Warning. This test uses at most %d tasks. The extra tasks will be ignored. Number of tasks in this job: %zu\n", MAXTASKS, num_tasks);
    return 1;
  }

  /* Initialize memory region exchange structure and local memory region */
  size_t i = 0;
#ifdef TEST_CROSSTALK
  for (; i<2; i++)
#endif
  {
    size_t n;
    for (n=0; n<num_tasks; n++)
    {
      _info[i].ping[n] = 1;
      _info[i].pong[n] = 1;
    }
    _info[i].exchange = num_tasks - 1;
    _info[i].zero = 0;

    size_t bytes;
    result = PAMI_Memregion_create (context[i],
                                    (void *) &_info[i],
                                    sizeof(info_t),
                                    &bytes,
                                    &_info[i].mr[task_id]);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to create memory region for context %zu. result = %d\n", i, result);
      return 1;
    }
    else if (bytes < sizeof(info_t))
    {
      fprintf (stderr, "Error. Unable to create memory region of a large enough size for context %zu. result = %d\n", i, result);
      return 1;
    }
    else
    {
      _info[i].ping[task_id] = 0;
      _info[i].pong[task_id] = 0;
    }
  }




  pami_dispatch_hint_t options={};
#ifdef USE_SHMEM_OPTION
  options.use_shmem = PAMI_HINT_ENABLE;
  fprintf (stderr, "##########################################\n");
  fprintf (stderr, "shared memory optimizations forced ON\n");
  fprintf (stderr, "##########################################\n");
#elif defined(NO_SHMEM_OPTION)
  options.use_shmem = PAMI_HINT_DISABLE;
  fprintf (stderr, "##########################################\n");
  fprintf (stderr, "shared memory optimizations forced OFF\n");
  fprintf (stderr, "##########################################\n");
#endif

  i = 0;
#ifdef TEST_CROSSTALK
  for (i=0; i<2; i++)
#endif
  {
    pami_dispatch_callback_function fn;

    fprintf (stderr, "Before PAMI_Dispatch_set(%d) .. &_info[%zu].exchange = %p\n", DISPATCH_ID_MEMREGION, i, &_info[i].exchange);
    fn.p2p = dispatch_mr;
    result = PAMI_Dispatch_set (context[i],
                                DISPATCH_ID_MEMREGION,
                                fn,
                                (void *)i,
                                options);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable register pami dispatch. result = %d\n", result);
      return 1;
    }
  }


  /* Exchange the memory regions */
  size_t cid = 0;
#ifdef TEST_CROSSTALK
  for (; cid<2; cid++)
#endif
  {
    pami_send_immediate_t parameters;
    parameters.dispatch        = DISPATCH_ID_MEMREGION;
    parameters.header.iov_base = (void *) &task_id;
    parameters.header.iov_len  = sizeof(pami_task_t);
    parameters.data.iov_base   = &_info[cid].mr[task_id];
    parameters.data.iov_len    = sizeof(pami_memregion_t);

    size_t tid;
    for (tid=0; tid<num_tasks; tid++)
    {
      if (tid != task_id)
      {
        PAMI_Endpoint_create (client, tid, cid, &parameters.dest);
        fprintf (stderr, "Before PAMI_Send_immediate()\n");
        PAMI_Send_immediate (context[cid], &parameters);
      }
    }

    fprintf (stderr, "Wait to receive all memory regions for context %zu\n", cid);
    while (_info[cid].exchange != 0)
    {
      result = PAMI_Context_advance (context[0], 100);
      if (result != PAMI_SUCCESS && result != PAMI_EAGAIN)
      {
        fprintf (stderr, "Error. Unable to advance pami context %zu. result = %d\n", cid, result);
        return 1;
      }
    }
  }


  if (task_id == 0)
  {
#ifdef TEST_CROSSTALK
    fprintf (stdout, "PAMI_Rput('simple') functional test [crosstalk]\n");
    fprintf (stdout, "\n");
#else
    fprintf (stdout, "PAMI_Rput('simple') functional test\n");
    fprintf (stdout, "\n");
#endif

    /* put a 'zero' to all 'ping' locations for all contexts on other tasks */
    cid = 0;
#ifdef TEST_CROSSTALK
    for (; cid<2; cid++)
#endif
    {
      /* set the number of rput operations to be done */
      _info[cid].active = num_tasks - 1;

      /* initialize common rput parameters */
      pami_rput_simple_t parameters;
      parameters.rma.hints          = null_send_hint;
      parameters.rma.bytes          = sizeof(size_t);
      parameters.rma.cookie         = (void *) &_info[cid].active;
      parameters.rma.done_fn        = decrement;
      parameters.put.rdone_fn       = NULL;
      parameters.rdma.local.mr      = &_info[0].mr[task_id];

      /* first, determine the offset to the 'zero' field for context 0 */
      parameters.rdma.local.offset  = ((size_t)&_info[0].zero) - ((size_t)&_info[0]);

      /* put from context 0 memregion to all destination contexts */
      size_t tid;
      for (tid = 0; tid<num_tasks; tid++)
      {
        /* next, determine the offset to the 'ping' field */
        parameters.rdma.remote.offset = ((size_t)&_info[cid].ping[tid]) - ((size_t)&_info[cid]);
        if (tid != task_id)
        {
          PAMI_Endpoint_create (client, tid, cid, &parameters.rma.dest);
          parameters.rdma.remote.mr = &_info[cid].mr[tid];
          pami_result_t result = PAMI_Rput (context[0], &parameters);
          if (result != PAMI_SUCCESS)
          {
            exit(1);
          }
        }
      }

      fprintf (stderr, "Wait to complete all 'ping' rput operations for context %zu\n", cid);
      while (_info[cid].active != 0)
      {
        result = PAMI_Context_advance (context[0], 100);
        if (result != PAMI_SUCCESS && result != PAMI_EAGAIN)
        {
          fprintf (stderr, "Error. Unable to advance pami context %zu. result = %d\n", cid, result);
          return 1;
        }
      }
    }

    cid = 0;
#ifdef TEST_CROSSTALK
    for (; cid<2; cid++)
#endif
    {
      fprintf (stderr, "Wait for all 'pong' rput operations to complete for context %zu\n", cid);
      size_t tid;
      for (tid = 0; tid<num_tasks; tid++)
      {
        while (_info[cid].pong[tid] != 0)
        {
          result = PAMI_Context_advance (context[0], 100);
          if (result != PAMI_SUCCESS && result != PAMI_EAGAIN)
          {
            fprintf (stderr, "Error. Unable to advance pami context %zu. result = %d\n", cid, result);
            return 1;
          }
        }
      }
    }

    /* All done ! */
  }
  else if (task_id < MAXTASKS)
  {
    cid = 0;
#ifdef TEST_CROSSTALK
    for (; cid<2; cid++)
#endif
    {
      fprintf (stderr, "Wait for the 'ping' rput operation to complete for context %zu\n", cid);
      while (_info[cid].ping[task_id] != 0)
      {
        PAMI_Context_advance (context[cid], 100);
        if (result != PAMI_SUCCESS)
        {
          fprintf (stderr, "Error. Unable to advance pami context %zu. result = %d\n", cid, result);
          return 1;
        }
      }

      fprintf (stderr, "Do a 'pong' rput operation to {0,0} from {%d,%zu}\n", task_id, cid);
      _info[cid].active = 1;

      /* initialize common rput parameters */
      pami_rput_simple_t parameters;
      parameters.rma.hints          = null_send_hint; 
      parameters.rma.bytes          = sizeof(size_t);
      parameters.rma.cookie         = (void *) &_info[cid].active;
      parameters.rma.done_fn        = decrement;
      parameters.put.rdone_fn       = NULL;
      parameters.rdma.local.mr      = &_info[cid].mr[task_id];

      /* first, determine the offset to the 'zero' field for context 0 */
      parameters.rdma.local.offset  = ((size_t)&_info[cid].zero) - ((size_t)&_info[cid]);

      /* next, determine the offset to the 'pong' field */
      parameters.rdma.remote.offset = ((size_t)&_info[0].pong[task_id]) - ((size_t)&_info[0]);
      parameters.rdma.remote.mr = &_info[0].mr[0];
      PAMI_Endpoint_create (client, 0, 0, &parameters.rma.dest);

      pami_result_t result = PAMI_Rput (context[cid], &parameters);
      if (result != PAMI_SUCCESS)
      {
        exit(1);
      }

      fprintf (stderr, "Wait to complete 'pong' rput operation to {0,0} from {%d,%zu}\n", task_id, cid);
      while (_info[cid].active != 0)
      {
        result = PAMI_Context_advance (context[cid], 100);
        if (result != PAMI_SUCCESS && result != PAMI_EAGAIN)
        {
          fprintf (stderr, "Error. Unable to advance pami context %zu. result = %d\n", cid, result);
          return 1;
        }
      }
    }
  }

  fprintf (stderr, "Test completed .. cleanup\n");

  cid = 0;
#ifdef TEST_CROSSTALK
  for (; cid<2; cid++)
#endif
  {
    /* Destroy the local memory region */
    PAMI_Memregion_destroy (context[cid], &_info[cid].mr[task_id]);
  }


  result = PAMI_Context_destroyv (context, num);
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

  fprintf (stdout, "Success (%d)\n", task_id);

  return 0;
};
