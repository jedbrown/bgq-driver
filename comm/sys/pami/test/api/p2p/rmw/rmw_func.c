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
 * \file test/api/p2p/rmw/rmw_func.c
 * \brief Simple point-to-point PAMI_Rmw() functional test
 **/

#include <pami.h>
#include <stdio.h>
#include <stddef.h>

#include "../../init_util.h"

typedef struct
{
  unsigned int counter0;
  unsigned int slot0;
  
  unsigned long long localbits1;
  unsigned long long globalbits1;
  
  signed long rmwbarrier;

  unsigned long long localbits3;
  
} rmw_data_t;


/* --------------------------------------------------------------- */

static void decrement (pami_context_t   context,
                       void           * cookie,
                       pami_result_t    result)
{
  unsigned * value = (unsigned *) cookie;
/*  TRACE_ERR((stderr, "(%zu) decrement() cookie = %p, %d => %d\n", _my_task, cookie, *value, *value - 1));*/
  --*value;
}

/* --------------------------------------------------------------- */

/**
 * \brief rmw virtual address exchange dispatch function
 */
void dispatch_fn (
  pami_context_t    context,      /**< IN: PAMI context */
  void            * cookie,       /**< IN: dispatch cookie */
  const void      * header,       /**< IN: header address */
  size_t            header_size,  /**< IN: header size */
  const void      * data,         /**< IN: address of PAMI pipe buffer */
  size_t            data_size,    /**< IN: size of PAMI pipe buffer */
  pami_endpoint_t   origin,
  pami_recv_t     * recv)         /**< OUT: receive message structure */
{
  pami_task_t task;
  size_t offset;
  PAMI_Endpoint_query (origin, &task, &offset);
  
  uintptr_t * data_location = (uintptr_t *) header;

  /*fprintf (stdout, "recv'd rmw data location %p from task %d\n", (void *) *data_location, task); */
  
  uintptr_t * rmw = (uintptr_t *) cookie;
  rmw[task] = *data_location;

  return;
}


int main (int argc, char ** argv)
{
  pami_client_t        client;
  pami_context_t       context;
  size_t               num_contexts = 1;
  pami_task_t          task_id;
  size_t               num_tasks;
  pami_result_t        result;

  volatile unsigned active;
  pami_rmw_t parameters;
  parameters.hints     = (pami_send_hint_t) {0};
  parameters.cookie    = (void *) &active;
  parameters.done_fn   = decrement;

  unsigned int ui_zero = 0;
  unsigned int ui_one  = 1;
  unsigned long long ull_zero = 0;
  signed long sl_one = 1;
  signed long sl_zero = 0;

  int rc = pami_init (&client,        /* Client             */
                      &context,       /* Context            */
                      NULL,           /* Clientname=default */
                      &num_contexts,  /* num_contexts       */
                      NULL,           /* null configuration */
                      0,              /* no configuration   */
                      &task_id,       /* task id            */
                      &num_tasks);    /* number of tasks    */

  if (rc == 1)
    return 1;
    
  if (num_tasks > 64)
    {
      fprintf (stdout, "This test, %s, requires num_tasks <= 64 (num_tasks is %zu).\n", argv[0], num_tasks);
      return 1;
    }


  uintptr_t * rmw = (uintptr_t *) malloc (sizeof(uintptr_t) * num_tasks);
  size_t i;
  for (i=0; i<num_tasks; i++)
    rmw[i] = 0;


  size_t dispatch = 10;
  pami_dispatch_callback_function fn;
  fn.p2p = dispatch_fn;
  pami_dispatch_hint_t options = {};

  result = PAMI_Dispatch_set (context,
                              dispatch,
                              fn,
                              (void *) rmw,
                              options);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable register pami dispatch. result = %d\n", result);
      return 1;
    }
    
  
  /* Allocate and intialize the local rmw data buffer for the test. */
  rmw_data_t data;
  data.counter0 = 0;
  data.slot0 = 0;
  data.globalbits1 = 0;
  data.localbits1 = 0;
  data.rmwbarrier = 0;
  
  /* Broadcast the data location to all tasks */
  for (i = 0; i < num_tasks; i++)
    {
      uintptr_t data_location = (uintptr_t) &data;
      
      pami_send_immediate_t parameters;
      parameters.dispatch        = dispatch;
      parameters.header.iov_base = (void *) &data_location;
      parameters.header.iov_len  = sizeof(uintptr_t);
      parameters.data.iov_base   = NULL;
      parameters.data.iov_len    = 0;
      PAMI_Endpoint_create (client, i, 0, &parameters.dest);

      /*fprintf (stdout, "send rmw data location %p (%p) to task %zu\n", parameters.header.iov_base, (void *) data_location, i); */

      result = PAMI_Send_immediate (context, &parameters);
    }
    
  /* Wait until all tasks have exchanged the rmw information */
  for (i = 0; i < num_tasks; i++)
    {
      /*fprintf (stdout, "waiting for task %zu to send rmw data.\n", i); */
      while (rmw[i] == 0)
        PAMI_Context_advance (context, 100);
    }
    
  /* **************************************************************************
   * TEST 0
   * 
   * Obtain a slot id from task 0. Each task will receive a number in the range
   * of [0 .. num_tasks-1]
   * 
   * This uses PAMI_ATOMIC_FETCH_ADD and PAMI_TYPE_UNSIGNED_INT
   * **************************************************************************/
  active = 1;
  
  parameters.local     = (void *) & data.slot0;
  parameters.remote    = (void *) (rmw[0] + offsetof(rmw_data_t, counter0));
  parameters.value     = (void *) & ui_one;
  parameters.test      = (void *) & ui_zero;
  parameters.operation = PAMI_ATOMIC_FETCH_ADD;
  parameters.type      = PAMI_TYPE_UNSIGNED_INT;
  
  PAMI_Endpoint_create (client, 0, 0, &parameters.dest);
  
  PAMI_Rmw (context, &parameters);
  
  if (task_id == 0)
    while (data.counter0 != num_tasks)
      PAMI_Context_advance (context, 100);

  while (active)
    PAMI_Context_advance (context, 100);
    
  fprintf (stdout, "data.counter0 = %d, data.slot0 = %d\n", data.counter0, data.slot0);

  /* **************************************************************************
   * TEST 1
   * 
   * Verify that all tasks received a unique slot id by flipping a bit in the
   * slot assigned to each task.
   * 
   * This uses PAMI_ATOMIC_FETCH_OR and PAMI_TYPE_UNSIGNED_LONG_LONG
   * **************************************************************************/
  {
    active = 1;
    /*
     * The value "0x01" has an implicit type of "signed int" by at least one
     * compiler. This causes a left-shift of 31 or more bits to sign-extend
     * *before* the assignment into the 'unsigned long long' variable .. hence,
     * the need to cast.
     */
    unsigned long long value = ((unsigned long long) 0x01) << data.slot0;
  
    parameters.local     = (void *) & data.localbits1;
    parameters.remote    = (void *) (rmw[0] + offsetof(rmw_data_t, globalbits1));
    parameters.value     = (void *) & value;
    parameters.test      = (void *) & ull_zero;
    parameters.operation = PAMI_ATOMIC_FETCH_OR;
    parameters.type      = PAMI_TYPE_UNSIGNED_LONG_LONG;
  
    PAMI_Endpoint_create (client, 0, 0, &parameters.dest);
  
    PAMI_Rmw (context, &parameters);
  
    if (task_id == 0)
    {
      unsigned long long test = (unsigned long long) -1;
      test = test >> (64 - num_tasks);

      fprintf (stdout, "task 0: advance until data.globalbits1 == %lld\n", test);
      
      while (data.globalbits1 != test)
        PAMI_Context_advance (context, 100);
    }

    while (active)
      PAMI_Context_advance (context, 100);
    
    fprintf (stdout, "data.globalbits1 = %lld, data.localbits1 = %lld\n", data.globalbits1, data.localbits1);
  }

  /* **************************************************************************
   * TEST 2
   * 
   * rmw barrier
   * 
   * This uses PAMI_ATOMIC_ADD and PAMI_TYPE_SIGNED_LONG, then
   * PAMI_ATOMIC_FETCH and PAMI_TYPE_SIGNED_LONG
   * **************************************************************************/
  {
    /* ----------------------------------------------------------------------
     * enter the barrier - add one to the counter
     * ---------------------------------------------------------------------- */
    active = 1;
    signed long tmp = 0;
  
    parameters.local     = (void *) & tmp;
    parameters.remote    = (void *) (rmw[0] + offsetof(rmw_data_t, rmwbarrier));
    parameters.value     = (void *) & sl_one;
    parameters.test      = (void *) & sl_zero;
    parameters.operation = PAMI_ATOMIC_ADD;
    parameters.type      = PAMI_TYPE_SIGNED_LONG;
  
    PAMI_Endpoint_create (client, 0, 0, &parameters.dest);
  
    PAMI_Rmw (context, &parameters);

    while (active)
      PAMI_Context_advance (context, 100);

    /* ----------------------------------------------------------------------
     * fetch the counter until all tasks have entered the barrier
     * ---------------------------------------------------------------------- */
    parameters.operation = PAMI_ATOMIC_FETCH;
    while (tmp != num_tasks)
    {
      active = 1;
      PAMI_Rmw (context, &parameters);
      while (active)
        PAMI_Context_advance (context, 100);
    }
  }

  /* **************************************************************************
   * TEST 3
   * 
   * Clear the bits set in "test 1", one at a time and in order.
   * 
   * This uses PAMI_ATOMIC_FETCH_COMPARE_XOR and PAMI_TYPE_UNSIGNED_LONG_LONG
   * **************************************************************************/
  {
    /*
     * The value "0x01" has an implicit type of "signed int" by at least one
     * compiler. This causes a left-shift of 31 or more bits to sign-extend
     * *before* the assignment into the 'unsigned long long' variable .. hence,
     * the need to cast.
     */
    unsigned long long value = ((unsigned long long) 0x01) << data.slot0;
    unsigned long long test  = ((unsigned long long) -1 ) >> (64 - data.slot0 - 1);
  
    data.localbits3 = (unsigned long long) 0;

    parameters.local     = (void *) & data.localbits3;
    parameters.remote    = (void *) (rmw[0] + offsetof(rmw_data_t, globalbits1));
    parameters.value     = (void *) & value;
    parameters.test      = (void *) & test;
    parameters.operation = PAMI_ATOMIC_FETCH_COMPARE_XOR;
    parameters.type      = PAMI_TYPE_UNSIGNED_LONG_LONG;
  
    PAMI_Endpoint_create (client, 0, 0, &parameters.dest);
  
    fprintf (stdout, "test3: before rmw loop .. data.localbits3 = %lld, test = %lld, value = %lld\n", data.localbits3, test, value);
  
    while (data.localbits3 != test)
    {
      active = 1;
  
      PAMI_Rmw (context, &parameters);
  
      while (active)
        PAMI_Context_advance (context, 100);
        
      fprintf (stdout, "test3: inside rmw loop .. data.localbits3 = %lld, test = %lld, value = %lld\n", data.localbits3, test, value);
    }
    
    fprintf (stdout, "data.globalbits1 = %lld, data.localbits3 = %lld (%lld)\n", data.globalbits1, data.localbits3, test);
    
    /* ----------------------------------------------------------------------
     * The 'root' must advance until data.globalbits1 == 0
     * ---------------------------------------------------------------------- */
    if (task_id == 0)
    {
      fprintf (stdout, "task 0: advance until data.globalbits1 == 0 (currently %lld)\n", data.globalbits1);
      while (data.globalbits1 != 0)
        PAMI_Context_advance (context, 100);
    }
  }













  rc = pami_shutdown(&client, context, &num_contexts);

  if (rc == 1)
    return 1;

  return 0;
};
