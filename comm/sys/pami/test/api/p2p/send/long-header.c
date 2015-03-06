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
 * \file test/api/p2p/send/long-header.c
 * \brief "Long header" point-to-point PAMI_send() test
 */

#include <pami.h>
#include <stdio.h>
#include <string.h>

/*#define ENABLE_TRACE*/ 

#ifdef ENABLE_TRACE
#define TRACE(x) fprintf x
#else
#define TRACE(x)
#endif

uint8_t _garbage[1024];

static void recv_done (pami_context_t   context,
                       void          * cookie,
                       pami_result_t    result)
{
  volatile size_t * active = (volatile size_t *) cookie;
  TRACE((stderr, "Called recv_done function.  active: %zu -> %zu\n", *active, *active-1));
  (*active)--;
}

static void test_dispatch (
    pami_context_t        context,      /**< IN: PAMI context */
    void               * cookie,       /**< IN: dispatch cookie */
    const void         * header_addr,  /**< IN: header address */
    size_t               header_size,  /**< IN: header size */
    const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
    size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
    pami_endpoint_t origin,
pami_recv_t         * recv)        /**< OUT: receive message structure */
{
  TRACE((stderr, "Called dispatch function.  cookie = %p, active: %zu, header_addr = %p, header_size = %zu\n", cookie,  *((volatile size_t *) cookie), header_addr, header_size));

  if (pipe_size == 0)
  {
    recv_done (context, cookie, PAMI_SUCCESS);
    return;
  }
  else if (pipe_size > 1024)
  {
    TRACE((stderr, "... dispatch function.  Too much data! pipe_size = %zu\n", pipe_size));
    exit(1);
  }
  else if (recv == NULL)
  {
    /* This is an 'immediate' receive */

    memcpy(_garbage, pipe_addr, pipe_size);
    recv_done (context, cookie, PAMI_SUCCESS);
  }
  else
  {
    /* This is an 'asynchronous' receive */

    recv->local_fn = recv_done;
    recv->cookie   = cookie;
    recv->type     = PAMI_TYPE_BYTE;
    recv->addr     = _garbage;
    recv->offset   = 0;
    TRACE((stderr, "... dispatch function.  recv->local_fn = %p\n", recv->local_fn));
  }

  return;
}

static void send_done_local (pami_context_t   context,
                             void          * cookie,
                             pami_result_t    result)
{
  volatile size_t * active = (volatile size_t *) cookie;
  TRACE((stderr, "Called send_done_local function.  active: %zu -> %zu\n", *active, *active-1));
  (*active)--;
}

static void send_done_remote (pami_context_t   context,
                              void          * cookie,
                              pami_result_t    result)
{
  volatile size_t * active = (volatile size_t *) cookie;
  TRACE((stderr, "Called send_done_remote function.  active: %zu -> %zu\n", *active, *active-1));
  (*active)--;
}

static void usage ()
{
  fprintf(stdout, "long-header.cnk tests the no_long_header hint between task 0 and task 1 using a long header size.\n");

  fprintf(stdout, "\nNOTE:  Hints are read as \"assert <hint>\" when the hint is \'1\' .. if it is \'0\' then it is considered \"unspecified\" (as opposed to \"unset\").  In other words, the only hard hints and soft hints that matter are 1\'s. Only sends can have soft hints.\n");

  fprintf(stdout, "Passing SEND scenario:\n");
  fprintf(stdout, "  send hints       recv hints     header\n");
  fprintf(stdout, "  long header  |   long header     size    dispatch ID\n");
  fprintf(stdout, "============== | ============== | ====== | ===========\n");
  fprintf(stdout, "   1 (hard)    |    1 (hard)    |  long  |      0\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "Failing SEND scenario:\n");
  fprintf(stdout, "  send hints       recv hints     header\n");
  fprintf(stdout, "  long header  |   long header     size    dispatch ID\n");
  fprintf(stdout, "============== | ============== | ====== | ===========\n");
  fprintf(stdout, "   2 (hard)    |    2 (hard)    |  long  |      1\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "Default is to run both the PASSING and FAILING SEND scenarios.\n\n");
  fprintf(stdout, "Testcase options:\n");
  fprintf(stdout, "-h   | --help            This help text.\n\n");
  fprintf(stdout, "-p   | --pass-only       Only run passing send scenarios.\n\n");
  fprintf(stdout, "-np  | --no-pass         Skip passing send scenarios.\n\n");
  fprintf(stdout, "-sf  | --send-fail-only  Only run failing send scenarios.\n\n");
  fprintf(stdout, "-nsf | --no-send-fail    Skip failing send scenarios.\n\n");
  exit(0);
}

int main (int argc, char ** argv)
{
  volatile size_t send_active = 2;
  volatile size_t recv_active = 1;

  size_t i = 0;

  pami_client_t client;
  pami_context_t context;
  char                  cl_string[] = "TEST";
  pami_result_t result = PAMI_ERROR;

  result = PAMI_Client_create (cl_string, &client, NULL, 0);
  if (result != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable to initialize pami client. result = %d\n", result);
    return 1;
  }

        {  result = PAMI_Context_createv(client, NULL, 0, &context, 1); }
  if (result != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable to create pami context. result = %d\n", result);
    return 1;
  }

  pami_configuration_t configuration;

  configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query(client, &configuration,1);
  if (result != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, result);
    return 1;
  }
  size_t task_id = configuration.value.intval;
  TRACE((stderr, "My task id = %zu\n", task_id));

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration,1);
  if (result != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, result);
    return 1;
  }
  size_t num_tasks = configuration.value.intval;
  TRACE((stderr, "Number of tasks = %zu\n", num_tasks));
  if (num_tasks != 2)
  {
    fprintf(stderr, "Error. This test requires 2 tasks. Number of tasks in this job: %zu\n", num_tasks);
    return 1;
  }

  size_t tests = 3; /* 11: run passing tests = 1, run send fail tests = 1 */ 

  for (i = 1; i < argc; i++) { /* Skip argv[0] (program name). */
    if ( (strcmp(argv[i], "-p") == 0) || (strcmp(argv[i], "--pass-only") == 0) ) {
      tests = 2;
    }
    if ( (strcmp(argv[i], "-np") == 0) || (strcmp(argv[i], "--no-pass") == 0) ) {
      tests = tests & 1;
    }
    if ( (strcmp(argv[i], "-sf") == 0) || (strcmp(argv[i], "--send-fail-only") == 0) ) {
      tests = 1;
    }
    if ( (strcmp(argv[i], "-nsf") == 0) || (strcmp(argv[i], "--no-send-fail") == 0) ) {
      tests = tests & 2;
    }
    if ( (strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0) ) {
      if (task_id == 0) {
	usage();
      } else {
	exit(0);
      }
    }
  }

  pami_dispatch_hint_t options;
  pami_dispatch_callback_function fn;
  fn.p2p = test_dispatch;

  options.long_header = PAMI_HINT_ENABLE;
  TRACE((stderr, "Before PAMI_Dispatch_set() .. &recv_active = %p, recv_active = %zu\n", &recv_active, recv_active));
  result = PAMI_Dispatch_set (context,
                             1,
                             fn,
                             (void *)&recv_active,
                             options);
  if (result != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable register pami dispatch. result = %d\n", result);
    return 1;
  }

  options.long_header = PAMI_HINT_DISABLE;
  TRACE((stderr, "Before PAMI_Dispatch_set() .. &recv_active = %p, recv_active = %zu\n", &recv_active, recv_active));
  result = PAMI_Dispatch_set (context,
                             2,
                             fn,
                             (void *)&recv_active,
                             options);
  if (result != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable register pami dispatch. result = %d\n", result);
    return 1;
  }

  uint8_t header[10240];

  pami_send_t parameters;
  parameters.send.dispatch        = 1;
  parameters.send.header.iov_base = (void *) header;
  parameters.send.header.iov_len  = 10240;
  parameters.send.data.iov_base   = NULL;
  parameters.send.data.iov_len    = 0;
  parameters.events.cookie        = (void *) &send_active;
  parameters.events.local_fn      = send_done_local;
  parameters.events.remote_fn     = send_done_remote;

  if ( (tests >> 1) & 1 ) {
    if (task_id == 0)
      {

	fprintf(stderr, "Testing good path ...\n");

	TRACE((stderr, "before send ...\n"));
	PAMI_Endpoint_create (client, 1, 0, &parameters.send.dest);
	result = PAMI_Send (context, &parameters);
	if (result != PAMI_SUCCESS)
	  {
	    fprintf(stderr, "Error. Send using dispatch configured to enable long header support failed. result = %d\n", result);
	    return 1;
	  }
	TRACE((stderr, "... after send.\n"));

	TRACE((stderr, "before send-recv advance loop ...\n"));
	while (send_active || recv_active)
	  {
	    result = PAMI_Context_advance (context, 100);
	    if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
	      {
		fprintf(stderr, "Error. Unable to advance pami context. result = %d\n", result);
		return 1;
	      }
	  }
	TRACE((stderr, "... after send-recv advance loop\n"));
      }
    else /* task id > 0 */
      {
	TRACE((stderr, "before recv advance loop ...\n"));
	while (recv_active != 0)
	  {
	    result = PAMI_Context_advance (context, 100);
	    if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
	      {
		fprintf(stderr, "Error. Unable to advance pami context. result = %d\n", result);
		return 1;
	      }
	  }
	TRACE((stderr, "... after recv advance loop\n"));

	TRACE((stderr, "before send ...\n"));
	PAMI_Endpoint_create (client, 0, 0, &parameters.send.dest);
	result = PAMI_Send (context, &parameters);
	if (result != PAMI_SUCCESS)
	  {
	    fprintf(stderr, "Error. Send using dispatch configured to enable long header support failed. result = %d\n", result);
	    return 1;
	  }
	TRACE((stderr, "... after send.\n"));

	TRACE((stderr, "before send advance loop ...\n"));
	while (send_active)
	  {
	    result = PAMI_Context_advance (context, 100);
	    if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
	      {
		fprintf(stderr, "Error. Unable to advance pami context. result = %d\n", result);
		return 1;
	      }
	  }
	TRACE((stderr, "... after send advance loop\n"));
      }
  } /* end run passing tests */

  /* ********************
   * Test error path .. dispatch configured without long header support used
   * to send a long header message.
   * *******************/
  if ( tests & 1 ) {
    if (task_id == 0)
      {

	fprintf(stderr, "Testing error path ...\n");

	TRACE((stderr, "before send ...\n"));
	PAMI_Endpoint_create (client, 1, 0, &parameters.send.dest);
	parameters.send.dispatch = 2;
	result = PAMI_Send (context, &parameters);
	if (result != PAMI_INVAL)
	  {
	    fprintf(stderr, "Error. Long header send using dispatch configured to disable long header support did not return an error as expected. result = %d\n", result);
	    return 1;
	  }
	TRACE((stderr, "... after send.\n"));
      }
  } /* end run error path */

  result = PAMI_Context_destroyv(&context, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable to destroy pami context. result = %d\n", result);
    return 1;
  }

  result = PAMI_Client_destroy(&client);
  if (result != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable to finalize pami client. result = %d\n", result);
    return 1;
  }

  return 0;
};
