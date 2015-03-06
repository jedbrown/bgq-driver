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
 * \file test/api/p2p/put/typed_put_func.c
 * \brief Simple point-to-point PAMI_Put() test
 *
 * This test depends on a functional PAMI_Send_immediate() function.
 */

#include <pami.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define PI 3.14159265
#define E  2.71828183

#define EXCHANGE_DISPATCH_ID 10
#define NOTIFY_DISPATCH_ID 11
#define MAX_TASKS 2
#define BUFFERSIZE 1024*16

#undef TRACE_ERR
#ifndef TRACE_ERR
#define TRACE_ERR(x)  /*fprintf x */
#endif

typedef struct
{
  uintptr_t        base;
  unsigned         active;
} info_t;


static void decrement (pami_context_t context, void * cookie, pami_result_t result)
{
  size_t * value = (size_t *) cookie;
  --*(value);
}

static void dispatch_notify (
  pami_context_t       context,      /**< IN: origin context */
  void               * cookie,       /**< IN: dispatch cookie */
  const void         * header_addr,  /**< IN: header address */
  size_t               header_size,  /**< IN: header size */
  const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
  size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t      origin,       /**< IN: endpoint that originated the send */
  pami_recv_t        * recv)         /**< OUT: receive message structure */
{
  unsigned * value = (unsigned *) cookie;
  ++*(value);

  return;
}

static void dispatch_exchange (
  pami_context_t       context,      /**< IN: origin context */
  void               * cookie,       /**< IN: dispatch cookie */
  const void         * header_addr,  /**< IN: header address */
  size_t               header_size,  /**< IN: header size */
  const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
  size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t      origin,       /**< IN: endpoint that originated the send */
  pami_recv_t        * recv)         /**< OUT: receive message structure */
{
  
  info_t * exchange = (info_t *) cookie;

  pami_task_t task;
  size_t offset;
  PAMI_Endpoint_query (origin, &task, &offset);
  
  exchange[task].base   = *((uintptr_t *)pipe_addr);
  exchange[task].active = 1;

  return;
}


int main (int argc, char ** argv)
{
  pami_client_t client;
  pami_context_t context[2];
  pami_task_t task_id;
  size_t num_tasks = 0;
  size_t ncontexts = 0;
  size_t errors = 0;

  pami_result_t result = PAMI_ERROR;



  pami_type_t subtype;
  pami_type_t compound_type;
  pami_type_t simple_type;

  info_t exchange[MAX_TASKS];
  double data[BUFFERSIZE];
  volatile unsigned ready;

  { /* init */


    ready = 0;

    unsigned i;

    for (i = 0; i < MAX_TASKS; i++)
      exchange[i].active = 0;

    for (i = 0; i < BUFFERSIZE; i++)
      data[i] = E;


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
    /*fprintf (stderr, "My task id = %d\n", task_id);*/

    if (task_id == 0)
      {
        for (i = 0; i < 320; i++)
          data[i] = PI;
      }

    configuration.name = PAMI_CLIENT_NUM_TASKS;
    result = PAMI_Client_query(client, &configuration, 1);

    if (result != PAMI_SUCCESS)
      {
        fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, result);
        return 1;
      }

    num_tasks = configuration.value.intval;

    /*if (task_id == 0)
      fprintf (stderr, "Number of tasks = %zu\n", num_tasks);*/

    if ((num_tasks < 2) || (num_tasks > MAX_TASKS))
      {
        fprintf (stderr, "Error. This test requires 2-%d tasks. Number of tasks in this job: %zu\n", MAX_TASKS, num_tasks);
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

    /*if (task_id == 0)
      fprintf (stderr, "maximum contexts = %zu, number of contexts used in this test = %zu\n", configuration.value.intval, ncontexts);*/

    result = PAMI_Context_createv(client, NULL, 0, context, ncontexts);

    if (result != PAMI_SUCCESS)
      {
        fprintf (stderr, "Error. Unable to create pami context(s). result = %d\n", result);
        return 1;
      }


    pami_dispatch_hint_t options = {};

    for (i = 0; i < ncontexts; i++)
      {
        pami_dispatch_callback_function fn;

        fn.p2p = dispatch_exchange;
        result = PAMI_Dispatch_set (context[i],
                                    EXCHANGE_DISPATCH_ID,
                                    fn,
                                    (void *) exchange,
                                    options);

        if (result != PAMI_SUCCESS)
          {
            fprintf (stderr, "Error. Unable register pami 'exchange' dispatch. result = %d\n", result);
            return 1;
          }

        fn.p2p = dispatch_notify;
        result = PAMI_Dispatch_set (context[i],
                                    NOTIFY_DISPATCH_ID,
                                    fn,
                                    (void *) & ready,
                                    options);

        if (result != PAMI_SUCCESS)
          {
            fprintf (stderr, "Error. Unable register pami 'notify' dispatch. result = %d\n", result);
            return 1;
          }
      }


    /* ***********************************
     * Create the pami types
     ************************************/

    /* This compound noncontiguous type is composed of one double, skips a double,
     * two doubles, skips a double, three doubles, skips a double, five doubles,
     * skips a double, six doubles, skips a double, seven doubles, skips a double,
     * eight doubles, then skips two doubles.
     *
     * This results in a type with 32 doubles that is 40 doubles
     * 'wide'.
     */
    PAMI_Type_create (&subtype);
    PAMI_Type_add_simple (subtype,
                          sizeof(double),      /* bytes */
                          0,                   /* offset */
                          1,                   /* count */
                          sizeof(double) * 2); /* stride */
    PAMI_Type_add_simple (subtype,
                          sizeof(double) * 2,  /* bytes */
                          0,                   /* offset */
                          1,                   /* count */
                          sizeof(double) * 3); /* stride */
    PAMI_Type_add_simple (subtype,
                          sizeof(double) * 3,  /* bytes */
                          0,                   /* offset */
                          1,                   /* count */
                          sizeof(double) * 4); /* stride */
    PAMI_Type_add_simple (subtype,
                          sizeof(double) * 5,  /* bytes */
                          0,                   /* offset */
                          1,                   /* count */
                          sizeof(double) * 6); /* stride */
    PAMI_Type_add_simple (subtype,
                          sizeof(double) * 6,  /* bytes */
                          0,                   /* offset */
                          1,                   /* count */
                          sizeof(double) * 7); /* stride */
    PAMI_Type_add_simple (subtype,
                          sizeof(double) * 7,  /* bytes */
                          0,                   /* offset */
                          1,                   /* count */
                          sizeof(double) * 8);/* stride */
    PAMI_Type_add_simple (subtype,
                          sizeof(double) * 8,  /* bytes */
                          0,                   /* offset */
                          1,                   /* count */
                          sizeof(double) * 10);/* stride */
    PAMI_Type_complete (subtype, sizeof(double));

    /* This noncontiguous type is composed of the above compound type, repeated
     * ten times with no stride.
     *
     * This results in a type with 320 doubles that is 400 doubles
     * 'wide'.
     */
    PAMI_Type_create (&compound_type);
    PAMI_Type_add_typed (compound_type,
                         subtype,                  /* subtype */
                         0,                        /* offset */
                         10,                       /* count */
                         sizeof(double) * 32);     /* stride */
    PAMI_Type_complete (compound_type, sizeof(double));


    /* This simple noncontiguous type is composed of eight contiguous doubles,
     * then skips a _single_ double, repeated 40 times.
     *
     * This results in a type with 320 doubles that is 360 doubles 'wide'.
     */
    PAMI_Type_create (&simple_type);
    PAMI_Type_add_simple (simple_type,
                          sizeof(double) * 8,  /* bytes */
                          0,                   /* offset */
                          40,                  /* count */
                          sizeof(double) * 9); /* stride */
    PAMI_Type_complete (simple_type, sizeof(double));



    /* Broadcast the memory buffer vaddr to all tasks - including self. */
    for (i = 0; i < num_tasks; i++)
      {
        uintptr_t base = (uintptr_t) &data[0];
        
        pami_send_immediate_t parameters;
        parameters.dispatch        = EXCHANGE_DISPATCH_ID;
        parameters.header.iov_base = NULL;
        parameters.header.iov_len  = 0;
        parameters.data.iov_base   = (void *) & base;
        parameters.data.iov_len    = sizeof(uintptr_t);
        PAMI_Endpoint_create (client, i, 0, &parameters.dest);

        result = PAMI_Send_immediate (context[0], &parameters);
      }

    /* Advance until all memory regions have been received. */
    for (i = 0; i < num_tasks; i++)
      {
        while (exchange[i].active == 0)
          PAMI_Context_advance (context[0], 100);
      }


  } /* init done */


  pami_send_immediate_t notify;
  notify.dispatch        = NOTIFY_DISPATCH_ID;
  notify.header.iov_base = NULL;
  notify.header.iov_len  = 0;
  notify.data.iov_base   = NULL;
  notify.data.iov_len    = 0;

  volatile size_t active = 1;

  pami_put_typed_t parameters;
  parameters.rma.hints          = (pami_send_hint_t) {0};
  parameters.rma.cookie         = (void *) & active;
  parameters.rma.done_fn        = NULL;
  parameters.put.rdone_fn       = decrement;
  parameters.rma.bytes          = 320 * sizeof(double);

  if (task_id == 0)
    {
      fprintf (stdout, "PAMI_Rput('typed') functional test %s\n", (ncontexts < 2) ? "" : "[crosstalk]");
      fprintf (stdout, "\n");

      PAMI_Endpoint_create (client, num_tasks - 1, ncontexts - 1, &parameters.rma.dest);
      PAMI_Endpoint_create (client, num_tasks - 1, ncontexts - 1, &notify.dest);
    }
  else
    {
      PAMI_Endpoint_create (client, 0, 0, &parameters.rma.dest);
      PAMI_Endpoint_create (client, 0, 0, &notify.dest);
    }


  /* ******************************************************************** */

  /* contiguous -> contiguous transfer test                               */

  /* ******************************************************************** */
  if (task_id == 0)
    {
      parameters.addr.local         = (void *) exchange[0].base;
      parameters.addr.remote        = (void *) exchange[num_tasks - 1].base;
      parameters.type.local         = PAMI_TYPE_DOUBLE;
      parameters.type.remote        = PAMI_TYPE_DOUBLE;

      active = 1;
      PAMI_Put_typed (context[0], &parameters);

      while (active > 0)
        PAMI_Context_advance (context[0], 100);

      /* Notify the remote task that the data has been transfered. */
      PAMI_Send_immediate (context[0], &notify);
    }
  else if (task_id == num_tasks - 1)
    {
      /* Wait for notification that the data has been transfered. */
      while (ready == 0)
        PAMI_Context_advance (context[ncontexts - 1], 100);

      ready = 0;
    }

  /* ******************************************************************** */

  /* contiguous -> non-contiguous transfer test                               */

  /* ******************************************************************** */
  if (task_id == num_tasks - 1)
    {
      parameters.addr.local         = (void *) exchange[num_tasks - 1].base;
      parameters.addr.remote        = (uint8_t *) exchange[0].base + (4 * 1024);
      parameters.type.local         = PAMI_TYPE_DOUBLE;
      parameters.type.remote        = simple_type;

      active = 1;
      PAMI_Put_typed (context[ncontexts - 1], &parameters);

      while (active > 0)
        PAMI_Context_advance (context[ncontexts - 1], 100);

      /* Notify the remote task that the data has been transfered. */
      PAMI_Send_immediate (context[ncontexts - 1], &notify);
    }
  else if (task_id == 0)
    {
      /* Wait for notification that the data has been transfered. */
      while (ready == 0)
        PAMI_Context_advance (context[0], 100);

      ready = 0;
    }

  /* ******************************************************************** */

  /* non-contiguous -> non-contiguous transfer test                       */

  /* ******************************************************************** */
  if (task_id == 0)
    {
      parameters.addr.local         = (uint8_t *) exchange[0].base + (4 * 1024);
      parameters.addr.remote        = (uint8_t *) exchange[num_tasks - 1].base + (4 * 1024);
      parameters.type.local         = simple_type;
      parameters.type.remote        = compound_type;

      active = 1;
      PAMI_Put_typed (context[0], &parameters);

      while (active > 0)
        PAMI_Context_advance (context[0], 100);

      /* Notify the remote task that the data has been transfered. */
      PAMI_Send_immediate (context[0], &notify);
    }
  else if (task_id == num_tasks - 1)
    {
      /* Wait for notification that the data has been transfered. */
      while (ready == 0)
        PAMI_Context_advance (context[ncontexts - 1], 100);

      ready = 0;
    }

  /* ******************************************************************** */

  /* non-contiguous -> contiguous transfer test                           */

  /* ******************************************************************** */
  if (task_id == num_tasks - 1)
    {
      parameters.addr.local         = (uint8_t *) exchange[num_tasks - 1].base + (4 * 1024);
      parameters.addr.remote        = (uint8_t *) exchange[0].base + (8 * 1024);
      parameters.type.local         = compound_type;
      parameters.type.remote        = PAMI_TYPE_DOUBLE;

      active = 1;
      PAMI_Put_typed (context[ncontexts - 1], &parameters);

      while (active > 0)
        PAMI_Context_advance (context[ncontexts - 1], 100);

      /* Notify the remote task that the data has been transfered. */
      PAMI_Send_immediate (context[ncontexts - 1], &notify);
    }
  else if (task_id == 0)
    {
      /* Wait for notification that the data has been transfered. */
      while (ready == 0)
        PAMI_Context_advance (context[0], 100);

      ready = 0;
    }


  /* ******************************************************************** */

  /* VERIFY data buffers                                                  */

  /* ******************************************************************** */
  if (task_id == 0)
    {
      if (task_id == 0)
        {
          unsigned i = 0;

          for (; i < 320; i++)
            {
              if (data[i] != PI)
                {
                  errors++;
                  fprintf (stderr, "Error. data[%d] != %g ..... (%g)\n", i, PI, data[i]);
                }
            }

          for (; i < 512; i++)
            {
              if (data[i] != E)
                {
                  errors++;
                  fprintf (stderr, "Error. data[%d] != %g ..... (%g)\n", i, E, data[i]);
                }
            }

          unsigned j = 0;

          for (; j < 40; j++)
            {
              unsigned n = 0;

              for (; n < 8; n++)
                {
                  if (data[i] != PI)
                    {
                      errors++;
                      fprintf (stderr, "Error. data[%d] != %g ..... (%g)\n", i, PI, data[i]);
                    }

                  i++;
                }

              if (data[i] != E)
                {
                  errors++;
                  fprintf (stderr, "Error. data[%d] != %g ..... (%g)\n", i, E, data[i]);
                }

              i++;
            }

          for (; i < 1024; i++)
            {
              if (data[i] != E)
                {
                  errors++;
                  fprintf (stderr, "Error. data[%d] != %g ..... (%g)\n", i, E, data[i]);
                }
            }


          for (; i < 1024 + 320; i++)
            {
              if (data[i] != PI)
                {
                  errors++;
                  fprintf (stderr, "Error. data[%d] != %g ..... (%g)\n", i, PI, data[i]);
                }
            }

          for (; i < BUFFERSIZE; i++)
            {
              if (data[i] != E)
                {
                  errors++;
                  fprintf (stderr, "Error. data[%d] != %g ..... (%g)\n", i, E, data[i]);
                }
            }
        }
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



