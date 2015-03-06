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
 * \file test/internals/util.h
 * \brief ???
 */

#ifndef __test_internals_util_h__
#define __test_internals_util_h__

#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include <pami.h>
#include "util/common.h"

#include <unistd.h>

#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf x
#endif


unsigned __barrier_active[2];
size_t __barrier_phase;
size_t __barrier_size;
size_t __barrier_task;
size_t __barrier_next_task;
pami_endpoint_t __barrier_next_endpoint;

size_t         __barrier_dispatch;
pami_context_t  __barrier_context;
pami_client_t   __barrier_client;

/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */
static void barrier_dispatch_function (
    pami_context_t        context,      /**< IN: PAMI context */
    void               * cookie,       /**< IN: dispatch cookie */
    const void         * header_addr,  /**< IN: header address */
    size_t               header_size,  /**< IN: header size */
    const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
    size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
    pami_endpoint_t origin,
pami_recv_t         * recv)        /**< OUT: receive message structure */
{
  size_t phase = *((size_t *) header_addr);

  TRACE_ERR((stderr, ">>> barrier_dispatch_function(), __barrier_active[%zu] = %u\n", phase, __barrier_active[phase]));

  --__barrier_active[phase];

  // Forward this barrier notification to the next task.
  //TRACE_ERR((stderr, "    barrier_dispatch_function(), __barrier_active[%zu].send %% __barrier_size (%zu) = %zu\n", index, __barrier_size, __barrier_active[index].send % __barrier_size));
}

/* ************************************************************************* */
/* ************************************************************************* */
/* ************************************************************************* */

void barrier ()
{
  size_t i;
  TRACE_ERR((stderr, "#### enter barrier(),  ...\n"));

  __barrier_active[__barrier_phase] = __barrier_size-1;
  __barrier_phase = __barrier_phase^1;

  pami_send_immediate_t parameters;
  parameters.dispatch        = __barrier_dispatch;
  parameters.header.iov_base = &__barrier_phase;
  parameters.header.iov_len  = sizeof (__barrier_phase);
  parameters.data.iov_base   = NULL;
  parameters.data.iov_len    = 0;

  __barrier_next_task = __barrier_task; // start with me (will increment immediately below)

  for(i=1; i< __barrier_size; ++i)  /// \todo This doesn't scale but it's simple
  {
    __barrier_next_task = (__barrier_next_task + 1) % __barrier_size;
    PAMI_Endpoint_create (__barrier_client, __barrier_next_task, 0, &__barrier_next_endpoint);
    parameters.dest            = __barrier_next_endpoint;

    TRACE_ERR((stderr, "     barrier(), before send, phase = %zu, __barrier_active[%zu] = %u, parameters.dest = 0x%08x\n", __barrier_phase, __barrier_phase, __barrier_active[__barrier_phase], parameters.dest));
    //pami_result_t result =
    PAMI_Send_immediate (__barrier_context, &parameters);
  }

  TRACE_ERR((stderr, " barrier() Before recv advance\n"));
  while (__barrier_active[__barrier_phase]  != 0)
    PAMI_Context_advance (__barrier_context, 100);

  TRACE_ERR((stderr, "####  exit barrier(), \n"));
  return;
}

void barrier_init (pami_client_t client, pami_context_t context, size_t dispatch)
{
  TRACE_ERR((stderr, "enter barrier_init() ...\n"));

  __barrier_client = client;

  pami_configuration_t configuration;

  configuration.name = PAMI_CLIENT_TASK_ID;
  pami_result_t result = PAMI_Client_query(client, &configuration,1);
  __barrier_task = configuration.value.intval;

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration, 1);
  __barrier_size = configuration.value.intval;

  TRACE_ERR((stderr,"__barrier_size:%zu __barrier_task:%zu\n",__barrier_size, __barrier_task));

  __barrier_context  = context;
  __barrier_dispatch = dispatch;
  __barrier_phase = 0;
  //__barrier_active[0].send = __barrier_size-1;
  __barrier_active[0] = __barrier_size-1;
  //__barrier_active[1].send = 0;
  __barrier_active[1] = __barrier_size-1;

  pami_dispatch_callback_function fn;
  fn.p2p = barrier_dispatch_function;
  pami_send_hint_t options={};
  TRACE_ERR((stderr, "Before PAMI_Dispatch_set() ...\n"));
  result = PAMI_Dispatch_set (context,
                             dispatch,
                             fn,
                             (void *)__barrier_active,
                             options);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable register pami dispatch. result = %d\n", result);
    PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
  }
  barrier();
  TRACE_ERR((stderr, "... exit barrier_init()\n"));
}



#undef TRACE_ERR
#endif // __tests_util_h__
