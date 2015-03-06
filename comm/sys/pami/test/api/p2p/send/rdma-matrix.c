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
 * \file test/api/p2p/send/rdma-matrix.c
 * \Matrixed "rdma" point-to-point PAMI_send() test
 */

#include "pami.h"
#include <stdio.h>
#include <string.h>

/* PAMI_HINT3_DEFAULT   = 0, This hint leaves the option up to the PAMI implementation to choose. */
/* PAMI_HINT3_FORCE_ON  = 1, This allows the user to force an option to be used. */
/* PAMI_HINT3_FORCE_OFF = 2, The user can force the implementation to not use this option. */
/* PAMI_HINT3_MAX       = 3, The largest hint value; it cannot actually be used. */

/*#define ENABLE_TRACE */

#ifdef ENABLE_TRACE
#define TRACE(x) fprintf x
#else
#define TRACE(x)
#endif

uint8_t __recv_buffer[2048];
char recv_str[2048];               /* used to print __recv_buffer as string */
size_t __recv_size;
size_t __header_errors;
size_t __data_errors;
uint8_t reset_value[2] ={0, 255};  /* reset value for each byte of __recv_buffer ...all 0's or all 1's (255) */
size_t rv = 0;                      /* used to loop over reset values */

unsigned validate (const void * addr, size_t bytes, size_t test_n_plus_1)
{
  unsigned status = 1;
  uint8_t * byte = (uint8_t *) addr;
  uint8_t expected_value = 0;
  size_t total_bytes = 0;
  size_t i, j = 0;

  /* Verify data received as well as 0-minus-1 and n-plus-1 bytes */
  if (test_n_plus_1) {
    total_bytes = bytes+2;
  } else { /* Only verify data received */
    total_bytes = bytes;
  }

  /* Loop through recv_buffer */
  for (i=0; i<total_bytes; i++) {

    /* Determine expected value */
    if (test_n_plus_1) {
      /* Ensure 0-minus-1 and n-plus-1 bytes equal the reset value */
      if ( (i == 0) || (i == total_bytes-1) ) {
	expected_value = reset_value[rv];
      } else { /* Validate received data (__recv_buffer[1:bytes]) */
	expected_value = (uint8_t)(i-1);
      }
    } else {
      expected_value = (uint8_t)i;
    }

    /* Verify current value */
    if (byte[i] != expected_value) {

      fprintf (stderr, "validate(%p,%zu) .. ERROR .. byte[%zu] != %d (&byte[%zu] = %p, value is %d)\n", addr, total_bytes, i, expected_value, i, &byte[i], byte[i]);

      status = 0;
    }

    /* Print element to string to print later if desired */
    sprintf(&recv_str[j], "%d", byte[i]);
    if (byte[i] < 10) {
      j++;
    } else if (byte[i] < 100) {
      j+=2;
    } else {
      j+=3;
    }
  }

  /* Print __recv_buffer */
  fprintf(stdout, "recv buffer[0:%zu] after send: %s\n", total_bytes-1, recv_str);

  return status;
}


static void recv_done (pami_context_t   context,
                       void          * cookie,
                       pami_result_t    result)
{
  volatile size_t * active = (volatile size_t *) cookie;
  fprintf (stderr, "Called recv_done function.  active(%p): %zu -> %zu, __recv_size = %zu\n", active, *active, *active-1, __recv_size);

  if (!validate(__recv_buffer, __recv_size, 1))
  {
    __data_errors++;
    fprintf (stderr, "validate data ERROR!\n");
  }

  (*active)--;
}

static void test_dispatch (
    pami_context_t       context,      /**< IN: PAMI context */
    void               * cookie,       /**< IN: dispatch cookie */
    const void         * header_addr,  /**< IN: header address */
    size_t               header_size,  /**< IN: header size */
    const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
    size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
    pami_endpoint_t      origin,
    pami_recv_t        * recv)         /**< OUT: receive message structure */
{
  volatile size_t * active = (volatile size_t *) cookie;
  fprintf (stderr, "Called dispatch function.  cookie = %p, active: %zu, header_size = %zu, pipe_size = %zu\n", cookie, *active, header_size, pipe_size);
  /*(*active)--; */
  /*fprintf (stderr, "... dispatch function.  active = %zu\n", *active); */

  if (!validate(header_addr, header_size, 0))
    __header_errors++;
    fprintf (stderr, "validate header ERROR!\n");

  if (pipe_size == 0)
  {
    (*active)--;
  }
  else
  {
    __recv_size = pipe_size;

    recv->local_fn = recv_done;
    recv->cookie   = cookie;
    recv->type     = PAMI_TYPE_BYTE;
    recv->addr     = &__recv_buffer[1];
    recv->offset   = 0;
    /*    fprintf (stderr, "... dispatch function.  recv->local_fn = %p\n", recv->local_fn); */
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

int main (int argc, char ** argv)
{

  size_t i, j;
  size_t run_all = 0;   /* run with alled hints */

  for (i = 1; i < argc; i++){ /* Skip argv[0] (program name). */

    if ( (strcmp(argv[i], "-a") == 0) || (strcmp(argv[i], "--all") == 0) ) {
      run_all = 1;
    }
  }

  volatile size_t send_active = 2;
  volatile size_t recv_active = 1;


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

  /* Ensure we have >= 2 tasks */
  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration,1);
  if (result != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, result);
    return 1;
  }
  size_t num_tasks = configuration.value.intval;
  TRACE((stderr, "Number of tasks = %zu\n", num_tasks));
  if (num_tasks < 2) {
    fprintf(stderr, "Error. This test requires >= 2 tasks. Number of tasks in this job: %zu\n", num_tasks);
    return 1;
  }

  pami_dispatch_hint_t options;
  pami_dispatch_callback_function fn;
  fn.p2p = test_dispatch;

  options.use_rdma = PAMI_HINT_DEFAULT;
  TRACE((stderr, "Before PAMI_Dispatch_set() .. &recv_active = %p, recv_active = %zu\n", &recv_active, recv_active));
  result = PAMI_Dispatch_set (context,
                             0,
                             fn,
                             (void *)&recv_active,
                             options);
  if (result != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable register pami dispatch. result = %d\n", result);
    return 1;
  }

  options.use_rdma = PAMI_HINT_ENABLE;
  TRACE((stderr, "Before PAMI_Dispatch_set() .. &recv_active = %p, recv_active = %zu\n", &recv_active, recv_active));
  result = PAMI_Dispatch_set (context,
                             5,
                             fn,
                             (void *)&recv_active,
                             options);
  if (result != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable register pami dispatch. result = %d\n", result);
    return 1;
  }

  options.use_rdma = PAMI_HINT_DISABLE;
  TRACE((stderr, "Before PAMI_Dispatch_set() .. &recv_active = %p, recv_active = %zu\n", &recv_active, recv_active));
  result = PAMI_Dispatch_set (context,
                             10,
                             fn,
                             (void *)&recv_active,
                             options);
  if (result != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable register pami dispatch. result = %d\n", result);
    return 1;
  }



  options.use_rdma = PAMI_HINT_INVALID;
  TRACE((stderr, "Before PAMI_Dispatch_set() .. &recv_active = %p, recv_active = %zu\n", &recv_active, recv_active));
  result = PAMI_Dispatch_set (context,
                             3,
                             fn,
                             (void *)&recv_active,
                             options);

  if (result != PAMI_INVAL) {
    /* Make this a warning till it's implemented */
    fprintf(stderr, "Warning. Should not be able to set use_rdma >= 3. result = %d\n", result);

    /*return 1; */
  }

  /* Setup header and data vars */
  uint8_t header[1024];
  uint8_t data[1024];
  for (i=0; i<1024; i++)
  {
    header[i] = (uint8_t)i;
    data[i]   = (uint8_t)i;
  }

  size_t hsize = 0;
  size_t header_bytes[16];
  header_bytes[hsize++] = 0;
  header_bytes[hsize++] = 16;
  header_bytes[hsize++] = 32;

  size_t psize = 0;
  size_t data_bytes[16];
  /*data_bytes[psize++] = 0; */
  /*data_bytes[psize++] = 16; */
  /*data_bytes[psize++] = 32; */
  /*data_bytes[psize++] = 64; */
  data_bytes[psize++] = 128;
  data_bytes[psize++] = 256;
  data_bytes[psize++] = 512;
  data_bytes[psize++] = 1024;

  /* Setup hint vars */
  size_t n = 0;                              /* controls task loop */
  size_t s = 0;                              /* controls send hints loop */
  size_t r = 0;                              /* controls recv hints loop */
  size_t send_hard_use_rdma_hint = 0;       /* hard hint value of sending task */
  size_t send_soft_use_rdma_hint = 0;       /* soft hint value of sending task */
  size_t recv_use_rdma_hint = 0;            /* hard hint value of receiving task */


  /* Create dispatch arrays */

  size_t dispatch_ary_0[3][12] = {{0, 0, 0, 0, 4, 0, 4, 4, 8, 8, 0, 8},
                                  {1, 1, 1, 1, 5, 0, 5, 5, 9, 9, 0, 9},
                                  {2, 2, 2, 2, 6, 0, 6, 6, 10, 10, 0, 10}};
  size_t dispatch_ary_n[3][12] = {{0, 0, 0, 0, 1, 0, 1, 1, 2, 2, 0, 2},
                                  {4, 4, 4, 4, 5, 0, 5, 5, 6, 6, 0, 6},
                                  {8, 8, 8, 8, 9, 0, 9, 9, 10, 10, 0, 10}};

  /* Init to skip alled scenarios ...enable later if requested */
  size_t skip_ary[3][12] =       {{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0},
                                  {1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1},
                                  {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0}};

  pami_send_t parameters;
  parameters.send.header.iov_base = header;
  parameters.send.header.iov_len  = header_bytes[1]; /* 16 bytes */
  parameters.send.data.iov_base   = data;
  parameters.send.data.iov_len    = data_bytes[2];   /* 512 bytes */
  parameters.events.cookie        = (void *) &send_active;
  parameters.events.local_fn      = send_done_local;
  parameters.events.remote_fn     = send_done_remote;

  /* ======== Combinations of use_rdma hints that should pass  ======== */

  if (task_id == 0)
  {

    fprintf(stderr, "======== Combinations of use_rdma hints that should pass  ========\n");

    /* Create task unique dispatch sets */
    options.use_rdma = PAMI_HINT_DEFAULT;
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

    options.use_rdma = PAMI_HINT_DEFAULT;
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

    options.use_rdma = PAMI_HINT_ENABLE;
    TRACE((stderr, "Before PAMI_Dispatch_set() .. &recv_active = %p, recv_active = %zu\n", &recv_active, recv_active));
    result = PAMI_Dispatch_set (context,
				4,
				fn,
				(void *)&recv_active,
				options);
    if (result != PAMI_SUCCESS)
    {
      fprintf(stderr, "Error. Unable register pami dispatch. result = %d\n", result);
      return 1;
    }

    options.use_rdma = PAMI_HINT_ENABLE;
    TRACE((stderr, "Before PAMI_Dispatch_set() .. &recv_active = %p, recv_active = %zu\n", &recv_active, recv_active));
    result = PAMI_Dispatch_set (context,
				6,
				fn,
				(void *)&recv_active,
				options);
    if (result != PAMI_SUCCESS)
    {
      fprintf(stderr, "Error. Unable register pami dispatch. result = %d\n", result);
      return 1;
    }

    options.use_rdma = PAMI_HINT_DISABLE;
    TRACE((stderr, "Before PAMI_Dispatch_set() .. &recv_active = %p, recv_active = %zu\n", &recv_active, recv_active));
    result = PAMI_Dispatch_set (context,
				8,
				fn,
				(void *)&recv_active,
				options);
    if (result != PAMI_SUCCESS)
    {
      fprintf(stderr, "Error. Unable register pami dispatch. result = %d\n", result);
      return 1;
    }

    options.use_rdma = PAMI_HINT_DISABLE;
    TRACE((stderr, "Before PAMI_Dispatch_set() .. &recv_active = %p, recv_active = %zu\n", &recv_active, recv_active));
    result = PAMI_Dispatch_set (context,
				9,
				fn,
				(void *)&recv_active,
				options);
    if (result != PAMI_SUCCESS)
    {
      fprintf(stderr, "Error. Unable register pami dispatch. result = %d\n", result);
      return 1;
    }

    for (r = 0; r < 3; r++) {
      for (s = 0; s < 12; s++) {

	/* Skip mismatched scenarios with unpredictable results */
	if (skip_ary[r][s]) {

	  /* Reset __recv_buffer based on next send hint TC */
	  /* 0's for even TCs and 255's for odd TCs */
	  rv = (s+1) % 2;

	  for (i = 0; i < 2048; i++) {
	    __recv_buffer[i] = reset_value[rv];
	  }

	  continue;
	}

	/* Determine hint values */
	send_hard_use_rdma_hint = (s >> 2) & 3;
	send_soft_use_rdma_hint = s & 3;
	recv_use_rdma_hint = r;

	parameters.send.dispatch        = dispatch_ary_0[r][s];

	/* Communicate with each task */
	for (n = 1; n < num_tasks; n++) {

	  TRACE((stderr, "before send ...\n"));

 	  result = PAMI_Endpoint_create (client, n, 0, &parameters.send.dest);
 	  if (result != PAMI_SUCCESS) {
 	    fprintf (stderr, "ERROR:  PAMI_Endpoint_create failed with %d.\n", result);
 	    return 1;
 	  }

	  fprintf(stderr, "Sending %zu byte header and %zu byte data from task %zu -> %zu:\n\t\ttask %zu use_rdma hard hint = %zu\n\t\ttask %zu use_rdma soft hint = %zu\n\t\ttask %zu use rdma hard hint = %zu\n", header_bytes[1], data_bytes[2], task_id, n, task_id, send_hard_use_rdma_hint, task_id, send_soft_use_rdma_hint, n, recv_use_rdma_hint);

	  result = PAMI_Send (context, &parameters);
	  if (result != PAMI_SUCCESS)
	  {
	    fprintf(stderr, "Error. Sent %zu byte header and %zu byte data from task %zu (use_rdma hard hint = %zu, use_rdma soft hint = %zu) to task %zu (use_rdma hard hint = %zu) and FAILED wth rc = %d\n",header_bytes[1], data_bytes[2], task_id, send_hard_use_rdma_hint, send_soft_use_rdma_hint, n, recv_use_rdma_hint, result);
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
	  send_active = 2;
	  recv_active = 1;

	} /* end task loop */

	/* Reset __recv_buffer based on next send hint TC */
	/* 0's for even TCs and 255's for odd TCs */
	rv = (s+1) % 2;

	for (i = 0; i < 2048; i++) {
	  __recv_buffer[i] = reset_value[rv];
	}

      } /* end send hint loop */
    } /* end recv hint loop */
  } /* end task = 0 */
  else { /* task id > 0 */

    /* Create task unique dispatch sets */
    options.use_rdma = PAMI_HINT_ENABLE;
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

    options.use_rdma = PAMI_HINT_DISABLE;
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

    options.use_rdma = PAMI_HINT_DEFAULT;
    TRACE((stderr, "Before PAMI_Dispatch_set() .. &recv_active = %p, recv_active = %zu\n", &recv_active, recv_active));
    result = PAMI_Dispatch_set (context,
				4,
				fn,
				(void *)&recv_active,
				options);
    if (result != PAMI_SUCCESS)
    {
      fprintf(stderr, "Error. Unable register pami dispatch. result = %d\n", result);
      return 1;
    }

    options.use_rdma = PAMI_HINT_DISABLE;
    TRACE((stderr, "Before PAMI_Dispatch_set() .. &recv_active = %p, recv_active = %zu\n", &recv_active, recv_active));
    result = PAMI_Dispatch_set (context,
				6,
				fn,
				(void *)&recv_active,
				options);
    if (result != PAMI_SUCCESS)
    {
      fprintf(stderr, "Error. Unable register pami dispatch. result = %d\n", result);
      return 1;
    }

    options.use_rdma = PAMI_HINT_DEFAULT;
    TRACE((stderr, "Before PAMI_Dispatch_set() .. &recv_active = %p, recv_active = %zu\n", &recv_active, recv_active));
    result = PAMI_Dispatch_set (context,
				8,
				fn,
				(void *)&recv_active,
				options);
    if (result != PAMI_SUCCESS)
    {
      fprintf(stderr, "Error. Unable register pami dispatch. result = %d\n", result);
      return 1;
    }

    options.use_rdma = PAMI_HINT_ENABLE;
    TRACE((stderr, "Before PAMI_Dispatch_set() .. &recv_active = %p, recv_active = %zu\n", &recv_active, recv_active));
    result = PAMI_Dispatch_set (context,
				9,
				fn,
				(void *)&recv_active,
				options);
    if (result != PAMI_SUCCESS)
    {
      fprintf(stderr, "Error. Unable register pami dispatch. result = %d\n", result);
      return 1;
    }

    result = PAMI_Endpoint_create (client, 0, 0, &parameters.send.dest);
    if (result != PAMI_SUCCESS) {
      fprintf (stderr, "ERROR:  PAMI_Endpoint_create failed with %d.\n", result);
      return 1;
    }

    for (r = 0; r < 3; r++) {
      for (s = 0; s < 12; s++) {

	/* Skip mismatched scenarios with unpredictable results */
	if (skip_ary[r][s]) {

	  /* Reset __recv_buffer based on next send hint TC */
	  /* 0's for even TCs and 255's for odd TCs */
	  rv = (s+1) % 2;

	  for (i = 0; i < 2048; i++) {
	    __recv_buffer[i] = reset_value[rv];
	  }

	  continue;
	}

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

	/* Reset __recv_buffer based on next send hint TC */
	if (recv_active == 0) {

	  /* 0's for even TCs and 255's for odd TCs */
	  rv = (s+1) % 2;

	  for (i = 0; i < 2048; i++) {
	    __recv_buffer[i] = reset_value[rv];
	  }

	  recv_active = 1;
	}

	TRACE((stderr, "before send ...\n"));

	/* Determine hint values */
	send_hard_use_rdma_hint = (s >> 2) & 3;
	send_soft_use_rdma_hint = (s & 3);
	recv_use_rdma_hint = r;

	parameters.send.dispatch        = dispatch_ary_n[r][s];

	fprintf(stderr, "Sending %zu byte header and %zu byte data from task %zu -> 0:\n\t\ttask %zu use_rdma hard hint = %zu\n\t\ttask %zu use_rdma soft hint = %zu\n\t\ttask 0 use rdma hard hint = %zu\n", header_bytes[1], data_bytes[2], task_id, task_id, send_hard_use_rdma_hint, task_id, send_soft_use_rdma_hint, recv_use_rdma_hint);

	result = PAMI_Send (context, &parameters);
	if (result != PAMI_SUCCESS)
	{
	  fprintf(stderr, "Error. Sent %zu byte header and %zu byte data from task %zu (use_rdma hard hint = %zu, use_rdma soft hint = %zu) to task 0 (use_rdma hard hint = %zu) and FAILED wth rc = %d\n",header_bytes[1], data_bytes[2], task_id, send_hard_use_rdma_hint, send_soft_use_rdma_hint, recv_use_rdma_hint, result);
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
	send_active = 2;
      } /* end send hint loop */
    } /* end recv hint loop */
  } /* end task != 0  */

  /* ======== Combinations of mismatched use_rdma hints with unpredicatble results ======== */
  if ( run_all == 1) {
    /* Enable unpredictable mismatched scenarios only ...skip expected passing scenarios */

    /* Disable all test scenarios */
    for (i = 0; i < 4; i++) {
      for(j = 0; j < 12; j++) {
	skip_ary[i][j] = 1;
      }
    }

    /* Execute send hard hint = 0, recv hard hint = 1 */
    skip_ary[1][0] = 0; skip_ary[1][1] = 0; skip_ary[1][2] = 0; skip_ary[1][3] = 0;
    /* Execute send hard hint = 2, recv hard hint = 1 */
    skip_ary[1][8] = 0; skip_ary[1][9] = 0; skip_ary[1][11] = 0;
    /* Execute send hard hint = 0, recv hard hint = 2 */
    skip_ary[2][0] = 0; skip_ary[2][1] = 0; skip_ary[2][2] = 0; skip_ary[2][3] = 0;
    /* Execute send hard hint = 1, recv hard hint = 2 */
    skip_ary[2][4] = 0; skip_ary[2][6] = 0; skip_ary[2][7] = 0;


    if (task_id == 0) {

      fprintf(stderr, "======== Combinations of mismatched rdma hints with unpredictable results ========\n");

      for (r = 0; r < 3; r++) {
	for (s = 0; s < 12; s++) {

	  /* Skip expected passing and failing scenarios */
	  if (skip_ary[r][s]) {

	    /* Reset __recv_buffer based on next send hint TC */
	    /* 0's for even TCs and 255's for odd TCs */
	    rv = (s+1) % 2;

	    for (i = 0; i < 2048; i++) {
	      __recv_buffer[i] = reset_value[rv];
	    }

	    continue;
	  }

	  /* Determine hint values */
	  send_hard_use_rdma_hint = (s >> 2) & 3;
	  send_soft_use_rdma_hint = s & 3;
	  recv_use_rdma_hint = r;

	  parameters.send.dispatch        = dispatch_ary_0[r][s];

	  /* Communicate with each task */
	  for (n = 1; n < num_tasks; n++) {

	    TRACE((stderr, "before send ...\n"));

	    result = PAMI_Endpoint_create (client, n, 0, &parameters.send.dest);
	    if (result != PAMI_SUCCESS) {
	      fprintf (stderr, "ERROR:  PAMI_Endpoint_create failed with %d.\n", result);
	      return 1;
	    }

	    fprintf(stderr, "Sending %zu byte header and %zu byte data from task %zu -> %zu:\n\t\ttask %zu use_rdma hard hint = %zu\n\t\ttask %zu use_rdma soft hint = %zu\n\t\ttask %zu use rdma hard hint = %zu\n", header_bytes[1], data_bytes[2], task_id, n, task_id, send_hard_use_rdma_hint, task_id, send_soft_use_rdma_hint, n, recv_use_rdma_hint);

	    result = PAMI_Send (context, &parameters);
	    if (result != PAMI_SUCCESS) {
	      fprintf(stderr, "Error. Sent %zu byte header and %zu byte data from task %zu (use_rdma hard hint = %zu, use_rdma soft hint = %zu) to task %zu (use_rdma hard hint = %zu) and FAILED wth rc = %d\n",header_bytes[1], data_bytes[2], task_id, send_hard_use_rdma_hint, send_soft_use_rdma_hint, n, recv_use_rdma_hint, result);
	      return 1;
	    }
	    TRACE((stderr, "... after send.\n"));


	    TRACE((stderr, "before send-recv advance loop ...\n"));
	    while (send_active || recv_active) {
	      result = PAMI_Context_advance (context, 100);
	      if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) ) {
		fprintf(stderr, "Error. Unable to advance pami context. result = %d\n", result);
		return 1;
	      }
	    }
	    TRACE((stderr, "... after send-recv advance loop\n"));

	    send_active = 2;
	    recv_active = 1;
	  } /* end task loop */

	  /* Reset __recv_buffer based on next send hint TC */
	  /* 0's for even TCs and 255's for odd TCs */
	  rv = (s+1) % 2;

	  for (i = 0; i < 2048; i++) {
	    __recv_buffer[i] = reset_value[rv];
	  }

	} /* end send hint loop */
      } /* end recv hint loop */
    } /* end task = 0 */
    else { /* task id > 0 */

      result = PAMI_Endpoint_create (client, 0, 0, &parameters.send.dest);
      if (result != PAMI_SUCCESS) {
	fprintf (stderr, "ERROR:  PAMI_Endpoint_create failed with %d.\n", result);
	return 1;
      }

      for (r = 0; r < 3; r++) {
	for (s = 0; s < 12; s++) {

	  /* Skip scenarios that are expected to pass and fail */
	  if (skip_ary[r][s]) {

	    /* Reset __recv_buffer based on next send hint TC */
	    /* 0's for even TCs and 255's for odd TCs */
	    rv = (s+1) % 2;

	    for (i = 0; i < 2048; i++) {
	      __recv_buffer[i] = reset_value[rv];
	    }

	    continue;
	  }

	  TRACE((stderr, "before recv advance loop ...\n"));
	  while (recv_active != 0) {
	    result = PAMI_Context_advance (context, 100);
	    if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) ) {
	      fprintf(stderr, "Error. Unable to advance pami context. result = %d\n", result);
	      return 1;
	    }
	  }
	  TRACE((stderr, "... after recv advance loop\n"));

	  /* Reset __recv_buffer based on next send hint TC */
	  if (recv_active == 0) {

	    /* 0's for even TCs and 255's for odd TCs */
	    rv = (s+1) % 2;

	    for (i = 0; i < 2048; i++) {
	      __recv_buffer[i] = reset_value[rv];
	    }

	    recv_active = 1;
	  }

	  TRACE((stderr, "before send ...\n"));

	  /* Determine hint values */
	  send_hard_use_rdma_hint = (s >> 2) & 3;
	  send_soft_use_rdma_hint = s & 3;
	  recv_use_rdma_hint = r;

	  parameters.send.dispatch        = dispatch_ary_n[r][s];

	  fprintf(stderr, "Sending %zu byte header and %zu byte data from task %zu -> 0:\n\t\ttask %zu use_rdma hard hint = %zu\n\t\ttask %zu use_rdma soft hint = %zu\n\t\ttask 0 use rdma hard hint = %zu\n", header_bytes[1], data_bytes[2], task_id, task_id, send_hard_use_rdma_hint, task_id, send_soft_use_rdma_hint, recv_use_rdma_hint);

	  result = PAMI_Send (context, &parameters);
	  if (result != PAMI_SUCCESS) {
	    fprintf(stderr, "Error. Sent %zu byte header and %zu byte data from task %zu (use_rdma hard hint = %zu, use_rdma soft hint = %zu) to task 0 (use_rdma hard hint = %zu) and FAILED wth rc = %d\n",header_bytes[1], data_bytes[2], task_id, send_hard_use_rdma_hint, send_soft_use_rdma_hint, recv_use_rdma_hint, result);
	    return 1;
	  }

	  TRACE((stderr, "... after send.\n"));

	  TRACE((stderr, "before send advance loop ...\n"));
	  while (send_active) {
	    result = PAMI_Context_advance (context, 100);
	    if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) ) {
	      fprintf(stderr, "Error. Unable to advance pami context. result = %d\n", result);
	      return 1;
	    }
	  }
	  TRACE((stderr, "... after send advance loop\n"));
	  send_active = 2;
	} /* end header loop */
      } /* end testcase loop */
    } /* end task != 0 */
  } /* end mismatched hint test */

  /* ======== Cleanup ======== */

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

  if ( __header_errors || __data_errors ) {
    fprintf(stderr, "Error. rdma_matrix FAILED with %zu header errors and %zu data errors. \n", __header_errors, __data_errors );
    return 1;
  } else {
    return 0;
  }
}
