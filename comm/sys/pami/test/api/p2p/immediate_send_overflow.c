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
 * \file test/api/p2p/immediate_send_overflow.c
 * \brief Simple point-topoint PAMI_Send_immediate() test
*/

#include <pami.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

static const char *optString = "DdfFpPSXh?";

/*char device_str[3][50] = {"DEFAULT", "SHMem", "MU"};*/
char hint_str[3][50] = {"PAMI_HINT_DEFAULT", "PAMI_HINT_ENABLE", "PAMI_HINT_DISABLE"};
char xtalk_str[2][50] = {"no crosstalk", "crosstalk"};
char debug_str[500];

size_t debug = 0;
size_t _my_task;
uint8_t __recv_buffer[2048];
char recv_str[2048];               /* used to print __recv_buffer as string */
size_t __recv_size;
size_t __header_errors = 0;
size_t __data_errors = 0;
uint8_t reset_value[2] ={0, 255};  /* reset value for each byte of __recv_buffer ...all 0's or all 1's (255) */
size_t reset_elements = 2;         /* total number of reset values */
size_t r = 0;                      /* used to loop over reset values */

unsigned validate (const void * addr, size_t bytes, size_t test_n_plus_minus1)
{
  unsigned status = 0;
  uint8_t * byte = (uint8_t *) addr;
  uint8_t expected_value = 0;
  size_t total_bytes = 0;
  size_t i, j = 0;

  /* Verify data received as well as 0-minus-1 and n-plus-1 bytes */
  if (test_n_plus_minus1) {
    total_bytes = bytes+2;
  } else { /* Only verify data received */
    total_bytes = bytes;
  }

  /* Skip validation if buffer size is 0 and not checking the +/- 1 bit */
  if (total_bytes == 0) {
    return 2;
  }   

  /* Loop through recv_buffer */
  for (i=0; i<total_bytes; i++) {

    /* Determine expected value */
    if (test_n_plus_minus1) {
      /* Ensure 0-minus-1 and n-plus-1 bytes equal the reset value */
      if ( (i == 0) || (i == total_bytes-1) ) {
	expected_value = reset_value[r];
      } else { /* Validate received data (__recv_buffer[1:bytes]) */
	expected_value = (uint8_t)(i-1);
      }
    } else {
      expected_value = (uint8_t)i;
    }

    /* Verify current value */
    if (byte[i] != expected_value) {

      fprintf (stderr, "ERROR (E):validate(%p,%zu):  byte[%zu] != %d (&byte[%zu] = %p, value is %d)\n", addr, total_bytes, i, expected_value, i, &byte[i], byte[i]);

      status = 1;
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
  } /* end loop thru recv_buffer */

  /* Print __recv_buffer */
  if (debug) {
    fprintf(stderr, "recv buffer[0:%zu] after send: %s\n", total_bytes-1, recv_str);
  }

  return status;
}
/* ---------------------------------------------------------------*/

static void recv_done (pami_context_t   context,
                       void          * cookie,
                       pami_result_t    result)
{
  volatile size_t * active = (volatile size_t *) cookie;
  unsigned validateRC = 0;

  if (debug) {
    fprintf (stderr, "Called recv_done function.  active(%p): %zu -> %zu, __recv_size = %zu\n", active, *active, *active-1, __recv_size);
  }

  /* Check payload data integrity */
  validateRC = validate(__recv_buffer, __recv_size, 1);

  switch (validateRC) {

  case 0: /* Validation passed */
    if (debug) {
      fprintf (stderr, ">>> Payload validated.\n");
    }
    break;
  case 1: /* Validation FAILED */
    __data_errors++;
    fprintf (stderr, ">>> ERROR (E):recv_done:  Validate payload FAILED!\n");
    break;
  case 2: /* Validation skipped */
    if (debug) {
      fprintf (stderr, ">>> Skipping payload validation (payload size = %zu).\n", __recv_size);
    }
    break;
  }

  (*active)--;
}
/* ---------------------------------------------------------------*/

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
  volatile size_t * active = (volatile size_t *) cookie;
  unsigned validateRC = 0;

  if (debug) {
    fprintf (stderr, "Called dispatch function.  cookie = %p, active: %zu, header_size = %zu, pipe_size = %zu, recv = %p\n", cookie, *active, header_size, pipe_size, recv);
  }
  /*(*active)--; */
  /*fprintf (stderr, "... dispatch function.  active = %zu\n", *active); */

  /* Check header data integrity */
  validateRC = validate(header_addr, header_size, 0);

  switch (validateRC) {

  case 0: /* Validation passed */
    if (debug) {
      fprintf (stderr, ">>> Header validated.\n");
    }
    break;
  case 1: /* Validation FAILED */
    __data_errors++;
    fprintf (stderr, ">>> ERROR (E):test_dispatch:  Validate header FAILED!\n");
    break;
  case 2: /* Validation skipped */
    if (debug) {
      fprintf (stderr, ">>> Skipping header validation (header size = %zu).\n", header_size);
    }
    break;
  }
 
  /* Check payload data integrity */
  if (recv == NULL) { 
    /* This is an 'immediate' receive */
    /* header + payload < receive immediate max for this context and dispatch ID */

    __recv_size = pipe_size;
    memcpy(&__recv_buffer[1], pipe_addr, pipe_size);
    recv_done (context, cookie, PAMI_SUCCESS);
  } else {
    /* This is an 'asynchronous' receive */

    __recv_size = pipe_size;

    recv->local_fn = recv_done;
    recv->cookie   = cookie;
    recv->type     = PAMI_TYPE_BYTE;
    recv->addr     = &__recv_buffer[1];
    recv->offset   = 0;
    recv->data_fn  = PAMI_DATA_COPY;
    recv->data_cookie = NULL;
    /*fprintf (stderr, "... dispatch function.  recv->local_fn = %p\n", recv->local_fn); */
  }

  return;
}
/* ---------------------------------------------------------------*/

#if 0
static void send_done_local (pami_context_t   context,
                             void          * cookie,
                             pami_result_t    result)
{
  volatile size_t * active = (volatile size_t *) cookie;
  if (debug) {
    fprintf (stderr, "Called send_done_local function.  active(%p): %zu -> %zu\n", active, *active, *active-1);
  }
  (*active)--;
}
/* ---------------------------------------------------------------*/

static void send_done_remote (pami_context_t   context,
                              void          * cookie,
                              pami_result_t    result)
{
  volatile size_t * active = (volatile size_t *) cookie;
  if (debug) {
    fprintf (stderr, "Called send_done_remote function.  active(%p): %zu -> %zu\n", active, *active, *active-1);
  }
  (*active)--;
  /*fprintf (stderr, "... send_done_remote function.  active = %zu\n", *active); */
}
/* ---------------------------------------------------------------*/
#endif

void display_usage( void )
{
    
  printf("This is the help text for immediate_send_overflow.c:\n");
  printf("immediate_send_overflow.c by default will attempt to pass messages of varying header and payload sizes between rank 0 and all other ranks (requires >= 2 ranks) using all available use_shmem hint values.\n");
  printf("\n");
  printf("The user can select a subset of the available hints by using the parms below:\n");
  printf("-D | --D PAMI_HINT_DEFAULT\n");
  printf("-S | --S PAMI_HINT_ENABLE\n");
  printf("-X | --X PAMI_HINT_DISABLE (BGQ)\n");
  printf("\n");
  printf("-F | --F Skip tests that should fail\n");
  printf("-P | --P Skip tests that should pass\n");
  printf("-f | --f Only run tests that should fail\n");
  printf("-p | --p Only run tests that should pass\n");
  printf("\n");
  printf("-d | --debug Enable error tracing messages to stderr for debug.\n");
  printf("\n");
  printf("Parms can be provided separately or together.\n");
  printf("\tex:  immediate_send_overflow.cnk -D -S --debug\n");
  printf("\tex:  immediate_send_overflow.cnk --XS\n");
  printf("\n");
}
/* ---------------------------------------------------------------*/

int main (int argc, char ** argv)
{

  int opt = 0;

  size_t hint_limit = 3;
  size_t available_hints = 7; /* (bit 0 = Default, bit 1 = S, bit 2 = M) */
  size_t hints_to_test = 0;   /* (bit 0 = Default, bit 1 = S, bit 2 = M) */
  size_t i, hint, n = 0;
  size_t tests = 3; /* 11: run passing tests = 1, run send fail tests = 1 */
  volatile size_t recv_active = 1;

  __data_errors = 0;

  pami_client_t client;
  pami_configuration_t configuration;
  pami_context_t context[2];

  char                  cl_string[] = "TEST";
  pami_result_t result = PAMI_ERROR;

  /* See if user passed in any args */
  opt = getopt( argc, argv, optString );
  while( opt != -1 ) {
    switch( opt ) {
    case 'D':
      hints_to_test = hints_to_test | 1;	  
      break;
  
    case 'f': /* only run tests that should fail */
      tests = 1;
      break;
                
    case 'F': /* don't run tests that should fail */
      tests = tests & 2;
      break;
              
    case 'd':
      debug = 1;
      break;
               
    case 'p': /* only run tests that should pass */
      tests = 2;
      break;
                
    case 'P': /* don't run tests that should pass */
      tests = tests & 1;
      break;                

    case 'S':
      hints_to_test = hints_to_test | 2;
      break;
               
    case 'X':
      hints_to_test = hints_to_test | 4;
      break;
 
    case 'h':   /* fall-through is intentional */
    case '?':
      if (_my_task == 0) {
	display_usage();
	return 1;
      }
    break;
                
    default:
      /* You won't actually get here. */
      break;
    }
        
    opt = getopt( argc, argv, optString );
  }

  result = PAMI_Client_create (cl_string, &client, NULL, 0);
  if (result != PAMI_SUCCESS) {
    fprintf (stderr, "ERROR (E):  Unable to initialize PAMI client. result = %d\n", result);
    return 1;
  }

  configuration.name = PAMI_CLIENT_NUM_CONTEXTS;
  result = PAMI_Client_query(client, &configuration,1);
  if (result != PAMI_SUCCESS) {
    fprintf (stderr, "ERROR (E):  Unable to query configuration (%d). result = %d\n", configuration.name, result);
    return 1;
  }
  size_t max_contexts = configuration.value.intval;
  if (max_contexts > 0) {
    fprintf (stdout, "Max number of contexts = %zu\n", max_contexts);
  } else {
    fprintf (stderr, "ERROR (E):  Max number of contexts (%zu) <= 0. Exiting\n", max_contexts);
    PAMI_Client_destroy(client);
    return 1;
  }

  size_t num_contexts = 1;
  if (max_contexts > 1) {
    num_contexts = 2; /* allows for cross talk */
  }

  result = PAMI_Context_createv(client, NULL, 0, context, num_contexts);
  if (result != PAMI_SUCCESS) {
    fprintf (stderr, "ERROR (E):  Unable to create PAMI context. result = %d\n", result);
    return 1;
  }

  configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query(client, &configuration,1);
  if (result != PAMI_SUCCESS) {
    fprintf (stderr, "ERROR (E):  Unable to query configuration (%d). result = %d\n", configuration.name, result);
    return 1;
  }
  _my_task = configuration.value.intval;
  fprintf (stderr, "My task id = %zu\n", _my_task);

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration,1);
  if (result != PAMI_SUCCESS) {
    fprintf (stderr, "ERROR (E):  Unable to query configuration (%d). result = %d\n", configuration.name, result);
    return 1;
  }
  size_t num_tasks = configuration.value.intval;
  fprintf (stderr, "Number of tasks = %zu\n", num_tasks);

  if (num_tasks < 2) {
    fprintf(stderr, "ERROR (E):  This test requires >= 2 tasks. Number of tasks in this job: %zu\n", num_tasks);
    return 1;
  }

  size_t num_local_tasks = 0;
  configuration.name = PAMI_CLIENT_NUM_LOCAL_TASKS;
  result = PAMI_Client_query (client, &configuration, 1);
  if (result != PAMI_SUCCESS) {
    fprintf (stderr, "ERROR (E):  Unable to query configuration attribute PAMI_CLIENT_NUM_LOCAL_TASKS; result = %d\n", result);
    return 1;
  } else {
    num_local_tasks = configuration.value.intval;
    fprintf (stdout, "PAMI_CLIENT_NUM_LOCAL_TASKS = %zu\n", configuration.value.intval);
  }

  /* Create an array for storing device addressabilty based on hint value*/
  /* row 0 (DEFAULT), row 1 (SHMem) and row 2 (SHMem disabled) */
  size_t addressable_by_me[3][num_tasks]; 

  /* Init device addressability array */
  for ( hint = 0; hint < 3; hint++ ) {
    for ( n = 0; n < num_tasks; n++ ) {
      if ( hint == 1) { /* SHMem */	
	addressable_by_me[hint][n] = 0;
      } else { /* hint = 0 (DEFAULT) or 2 (SHMem disabled) */
	addressable_by_me[hint][n] = 1;
      }
    }
  }

  configuration.name = PAMI_CLIENT_LOCAL_TASKS;
  result = PAMI_Client_query (client, &configuration, 1);
  if (result != PAMI_SUCCESS) {
    fprintf (stderr, "ERROR (E):  Unable to query configuration attribute PAMI_CLIENT_LOCAL_TASKS; result = %d\n", result);
    return 1;
  } else {
    /* Set local tasks in device addressability array */
    if (debug) {   
      sprintf (debug_str, "Tasks local to task %zu:", _my_task);
    }

    for (i=0; i<num_local_tasks; i++) {
      if (debug) {   
	if (i == 0) { /* First local task */
	  sprintf (debug_str, "%s  %zu", debug_str, configuration.value.intarray[i]);
	} else if (i == (num_local_tasks - 1)) {
	  sprintf (debug_str, "%s & %zu", debug_str, configuration.value.intarray[i]);	
	} else {
	  sprintf (debug_str, "%s, %zu", debug_str, configuration.value.intarray[i]);
	}
      }

      addressable_by_me[1][configuration.value.intarray[i]] = 1;
    }

    if (debug) {   
      fprintf (stderr, "%s\n", debug_str);
    }
  }

  if (debug) {
    sprintf (debug_str, "task %zu SHMem addressable tasks:  ", _my_task);

    for ( n = 0; n < num_tasks; n++ ) {
      if (addressable_by_me[1][n] == 1) {
	sprintf (debug_str, "%s %zu", debug_str, n);
      }
    }

    fprintf (stderr, "%s\n", debug_str);
  }

  /* Determine which Device(s) are initialized based on PAMI_DEVICE env var */
  /*
  char * device;  

  if (debug) {
    fprintf (stderr, "Before device check ...\n");
  }

  device = getenv("PAMI_DEVICE");
  
  if ( device != NULL ) {*/ /* Passing NULL to strcmp will result in a segfault */
  /*    if ( strcmp(device, "M") == 0 ) {
      if (_my_task == 0) {
	fprintf (stdout, "Only the %s device is initialized.\n", &device_str[2][0]);
      }
      available_hints = 5;
    } else if ( strcmp(device, "S") == 0 ) {
      if (_my_task == 0) {
	fprintf (stdout, "Only the SHMem device is initialized.\n");
      }
      available_hints = 3;
    } else if ( strcmp(device, "B") == 0 ) {
      if (_my_task == 0) {
	fprintf (stdout, "Both the %s and SHMem devices are initialized.\n", &device_str[2][0]);
      }
    } else {
      if (_my_task == 0) {
	fprintf (stderr, "ERROR (E):  PAMI_DEVICE = %s is unsupported. Valid values are:  M (%s only), S (SHMem only) & [ B | unset ] (both %s & SHMem)\n", device, &device_str[2][0], &device_str[2][0]);
      }
      return 1;
    } 
  } else {
    if (_my_task == 0) {
      fprintf (stderr, "Both the %s and SHMem devices are initialized.\n", &device_str[2][0]);
    }
  }
  
  if (debug) {
    fprintf (stderr, "After device check ...\n");
  }
  */

  /* Default to test all available hints */
  if ( hints_to_test == 0 ) {
    hints_to_test = available_hints;
  } else { /* only test hints selected by user that are available */
    hints_to_test = available_hints & hints_to_test;
  }

  /* Print table of available hints and if they'll be tested */
  if (_my_task == 0) {
    fprintf (stdout, "use_shmem Value(s)\tAVAILABLE\tUNDER TEST\n"); 
    for  (hint = 0; hint < 3; hint++) {
      fprintf(stdout, "%s\t", &hint_str[hint][0]);

      if ( (available_hints >> hint) & 1 ) {
	fprintf(stdout, "    Y\t\t");
      } else {
	fprintf(stdout, "    N\t\t");
      }

      if ( (hints_to_test >> hint) & 1 ) {
	fprintf(stdout, "     Y\n");
      } else {
	fprintf(stdout, "     N\n");
      }
    }
  }

  /* Create 3 dispatch_info_t structures (1 for each use_shmem hint value) */
  typedef struct
  {
    size_t                 id;
    pami_dispatch_hint_t   options;
    char                 * name;
    pami_result_t          result;
  } dispatch_info_t;

  dispatch_info_t dispatch[3];

  dispatch[0].id = 0;
  dispatch[0].options = (pami_dispatch_hint_t) {0};
  dispatch[0].name = "  DEFAULT ";
  dispatch[0].options.use_shmem = PAMI_HINT_DEFAULT;

  dispatch[1].id = 1;
  dispatch[1].options = (pami_dispatch_hint_t) {0};
  dispatch[1].name = "ONLY SHMEM";
  dispatch[1].options.use_shmem = PAMI_HINT_ENABLE;

  dispatch[2].id = 2;
  dispatch[2].options = (pami_dispatch_hint_t) {0};
  dispatch[2].name = " NO SHMEM ";
  dispatch[2].options.use_shmem = PAMI_HINT_DISABLE;

  pami_dispatch_callback_function fn;
  fn.p2p = test_dispatch;

  size_t dispatch_send_immediate_max[max_contexts][3];
  size_t dispatch_recv_immediate_max[max_contexts][3];
  
  uint8_t header[1024];
  uint8_t data[1024];

  for (i=0; i<1024; i++) {
    header[i] = (uint8_t)i;
    data[i]   = (uint8_t)i;
  }

  size_t header_bytes[16];
  size_t data_bytes[16];

  size_t max_msg_size = 0;
  size_t scenarios = 0;
  size_t index = 0;

  /* PAMI_Dispatch_set for functional tests */

  if (debug) {
    fprintf (stderr, "Before PAMI_Dispatch_set() .. &recv_active = %p, recv_active = %zu\n", &recv_active, recv_active);
  }

  for (i = 0; i < num_contexts; i++) {
    /* For each context: */
    /* Set up dispatch ID 0 for DEFAULT (use_shmem = 0 | PAMI_HINT_DEFAULT) */
    /* Set up dispatch ID 1 for SHMEM   (use_shmem = 1 | PAMI_HINT_ENABLE)  */
    /* Set up dispatch ID 2 for HFI/MU  (use_shmem = 2 | PAMI_HINT_DISABLE) */

    for (hint = 0; hint < hint_limit; hint++) {
      
      if (debug) {
      fprintf (stderr, "rank:  %zu\tnum local tasks:  %zu\n", _my_task, num_local_tasks);
      }

      /* Can't create shmem dispatch ID if there aren't any other local tasks */
      if ( (hint == 1) && (num_local_tasks == 1) ) {
	fprintf(stderr, "WARNING (W):  No other local tasks exist with Rank %zu.  Skipping creation of \"SHMem\" dispatch ID ...\n", _my_task);
	continue;
      }

      dispatch[hint].result = PAMI_Dispatch_set (context[i],
                                              dispatch[hint].id,
                                              fn,
                                              (void *)&recv_active,
                                              dispatch[hint].options); 

      if (dispatch[hint].result != PAMI_SUCCESS) {
	fprintf (stderr, "ERROR (E):  Unable to register pami dispatch ID %zu on context %zu. result = %d\n", dispatch[hint].id, i, dispatch[hint].result);
	return 1;
      }
      
      /* Find max immediate send size for this context/use_shmem combo */
      configuration.name = PAMI_DISPATCH_SEND_IMMEDIATE_MAX;
      dispatch[hint].result = PAMI_Dispatch_query(context[i], dispatch[hint].id, &configuration,1);

      if (dispatch[hint].result != PAMI_SUCCESS) {
	fprintf (stderr, "ERROR (E):  Unable to query configuration (%d). result = %d\n", configuration.name, dispatch[hint].result);
	return 1;
      }

      dispatch_send_immediate_max[i][hint] = configuration.value.intval;

      if (_my_task == 0) {
	fprintf (stdout, "Send immediate maximum for context %zu dispatch ID %zu = %zu\n", i, dispatch[hint].id, dispatch_send_immediate_max[i][hint]);
      }

      /* Find max immediate recv size for this context/use_shmem combo */
      configuration.name = PAMI_DISPATCH_RECV_IMMEDIATE_MAX;
      dispatch[hint].result = PAMI_Dispatch_query(context[i], dispatch[hint].id, &configuration,1);

      if (dispatch[hint].result != PAMI_SUCCESS) {
	fprintf (stderr, "ERROR (E):  Unable to query configuration (%d). result = %d\n", configuration.name, dispatch[hint].result);
	return 1;
      }

      dispatch_recv_immediate_max[i][hint] = configuration.value.intval;

      if (_my_task == 0) {
	fprintf (stdout, "Recv immediate maximum for context %zu dispatch ID %zu = %zu\n", i, dispatch[hint].id, dispatch_recv_immediate_max[i][hint]);
      }
    } /* end hint loop */
  } /* end context loop */

  size_t xtalk = 0;

  /* Time to run various headers and payloads */
  pami_send_immediate_t im_parameters;
  im_parameters.header.iov_base = header;
  im_parameters.data.iov_base   = data;

  /* ======== Combinations of header and payload sizes that should pass  ======== */
  if ( (tests >> 1) & 1 ) { 
    if (_my_task == 0)
      {

	fprintf(stdout, "====== Combinations of header and payload sizes that should pass ======\n");

	for(hint = 0; hint < hint_limit; hint++) {             /* hint/device loop */

	  /* Skip hints that are not under test */
	  if ( ! ((hints_to_test >> hint) & 1) ) {
	    continue;
	  }

	  im_parameters.dispatch = hint;

	  for(xtalk = 0; xtalk < num_contexts; xtalk++) {      /* xtalk loop */

	    /* Determine max message size */
	    if ( dispatch_send_immediate_max[xtalk][hint] < dispatch_recv_immediate_max[xtalk][hint] ) {
	      max_msg_size = dispatch_send_immediate_max[xtalk][hint];
	    } else {
	      max_msg_size = dispatch_recv_immediate_max[xtalk][hint];
	    }

	    /* Run the following scenarios:            */
	    /* header = 0             payload = max    */
	    /* header = max/2         payload = max/2  */
	    /* header = max           payload = 0      */

	    /* Populate header and payload size arrays */
	    scenarios = 0;
	    header_bytes[scenarios++] = 0;
	    header_bytes[scenarios++] = max_msg_size/2;
	    header_bytes[scenarios++] = max_msg_size;

	    scenarios = 0;
	    data_bytes[scenarios++] = max_msg_size;
	    data_bytes[scenarios++] = max_msg_size/2;
	    data_bytes[scenarios++] = 0;

	    for (index=0; index < scenarios; index++) {         /* scenario loop */
	      im_parameters.header.iov_len = header_bytes[index];
	      im_parameters.data.iov_len = data_bytes[index];
	    
	      /* Communicate with each task */
	      for (n = 1; n < num_tasks; n++) {

		/* Skip sends to tasks that are unaddressable by rank 0 with current hint value */
		if ( addressable_by_me[hint][n] != 1 ) {

		  /* If this is first send, inform user of skip */
		  if ( ! ( xtalk || index ) ) {
		    fprintf(stderr, "WARNING (W):  Rank %zu is unaddressable by Rank %zu when use_shmem = %s. Skipping until next use_shmem hint ...\n", n, _my_task, &hint_str[hint][0]);
		  }

		  /* Determine reset value  */
		  /* reset value = 0 for even scenarios, 255 for odd scenarios */

		  if (n == (num_tasks - 1)) { /* index is going to increment */ 
		    /* base reset value on next index value */
		    if (index == (scenarios - 1)) { /* index is going to reset */
		      r = 0;
		    } else { 
		      r = (index+1) % 2; 
		    }
		  } else {
		    r = index % 2; /* base reset value on current index value */
		  }

		  /* Reset __recv_buffer for next payload */
		  for (i = 0; i < 2048; i++) {
		    __recv_buffer[i] = reset_value[r];
		  }

		  continue;

		}

		result = PAMI_Endpoint_create (client, n, xtalk, &im_parameters.dest);
		if (result != PAMI_SUCCESS) {
		  fprintf (stderr, "ERROR (E):  PAMI_Endpoint_create failed for task ID %zu, context %zu with %d.\n", n, xtalk, result);
		  return 1;
		}

		fprintf (stderr, "===== PAMI_Send_immediate() FUNCTIONAL Test [use_shmem=%s][%s][+/-1 bit = %d] %zu %zu (%zu, 0) -> (%zu, %zu) =====\n\n", &hint_str[hint][0], &xtalk_str[xtalk][0], __recv_buffer[0], header_bytes[index], data_bytes[index], _my_task, n, xtalk);

		if (debug) {
		  fprintf (stderr, "before PAMI_Send_immediate ...\n");
		}

		result = PAMI_Send_immediate (context[0], &im_parameters);
		
		if (debug) {
		  fprintf (stderr, "... after PAMI_Send_immediate.\n");
		}

		if (result != PAMI_SUCCESS) {
		  fprintf (stderr, "ERROR (E):  PAMI_Send_immediate failed with rc = %d\n", result);
		  return 1;
		}


		if (debug) {
		  fprintf (stderr, "before recv advance loop ... &recv_active = %p\n", &recv_active);
		}

		while (recv_active) {
		  result = PAMI_Context_advance (context[0], 100);

		  if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) ) {
		    fprintf (stderr, "ERROR (E):  Unable to advance pami context 0. result = %d\n", result);
		    return 1;
		  }
		}

		if (debug) {
		  fprintf (stderr, "... after recv advance loop\n");
		}

		if (recv_active == 0) {

		  /* Determine reset value  */
		  /* reset value = 0 for even scenarios, 255 for odd scenarios */
		  if (n == (num_tasks - 1)) { /* index is going to increment */
		    /* base reset value on next index value */
		    if (index == (scenarios - 1)) { /* index is going to reset */
		      r = 0;
		    } else { 
		      r = (index+1) % 2; 
		    }
		  } else {
		    r = index % 2; /* base reset value on current p value */
		  }

		  /* Reset __recv_buffer for next payload */
		  for (i = 0; i < 2048; i++) {
		    __recv_buffer[i] = reset_value[r];
		  }
		    
		  recv_active = 1;
		}
	      } /* end task id loop */
	    } /* end scenario loop */
	  } /* end xtalk loop */
	} /* end device loop */
      } /* end task = 0 */
    else {
      for(hint = 0; hint < hint_limit; hint++) {      /* hint/device loop */

	/* Skip hints that are not under test */
	if ( ! ((hints_to_test >> hint) & 1) ) {
	  continue;
	}

	/* Skip sends to task 0 that are unaddressable by current rank with current hint value */
	if ( addressable_by_me[hint][0] != 1 ) {
	  if (debug) {
	    fprintf(stderr, "WARNING (W):  Rank 0 is unaddressable by Rank %zu when use_shmem = %s. Skipping to next use_shmem hint ...\n", _my_task, &hint_str[hint][0]);
	  }
	  continue;
	}

	im_parameters.dispatch = hint;

	for(xtalk = 0; xtalk < num_contexts; xtalk++) {        /* xtalk loop */

	  result = PAMI_Endpoint_create (client, 0, 0, &im_parameters.dest);
	  if (result != PAMI_SUCCESS) {
	    fprintf (stderr, "ERROR (E):  PAMI_Endpoint_create failed for task ID 0, context 0 with %d.\n", result);
	    return 1;
	  }
	       

	  /* Determine max message size */
	  if ( dispatch_send_immediate_max[xtalk][hint] < dispatch_recv_immediate_max[xtalk][hint] ) {
	    max_msg_size = dispatch_send_immediate_max[xtalk][hint];
	  } else {
	    max_msg_size = dispatch_recv_immediate_max[xtalk][hint];
	  }

	  /* Run the following scenarios:            */
	  /* header = 0             payload = max    */
	  /* header = max/2         payload = max/2  */
	  /* header = max           payload = 0      */

	  /* Populate header and payload size arrays */
	  scenarios = 0;
	  header_bytes[scenarios++] = 0;
	  header_bytes[scenarios++] = max_msg_size/2;
	  header_bytes[scenarios++] = max_msg_size;

	  scenarios = 0;
	  data_bytes[scenarios++] = max_msg_size;
	  data_bytes[scenarios++] = max_msg_size/2;
	  data_bytes[scenarios++] = 0;

	  for (index=0; index < scenarios; index++) {         /* test loop */
	    im_parameters.header.iov_len = header_bytes[index];
	    im_parameters.data.iov_len = data_bytes[index];

	    uint8_t task0_buffer_default = __recv_buffer[0]; /* save for later */

	    if (debug) {
	      fprintf (stderr, "before recv advance loop ... &recv_active = %p\n", &recv_active);
	    }

	    while (recv_active != 0) {
	      result = PAMI_Context_advance (context[xtalk], 100);
	      if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) ) {
		fprintf (stderr, "ERROR (E):  Unable to advance pami context %zu. result = %d\n", xtalk, result);
		return 1;
	      }
	    
	      /*fprintf (stderr, "------ recv advance loop ... &recv_active = %p\n", &recv_active);*/
	    }

	    if (debug) {
	      fprintf (stderr, "... after recv advance loop\n");
	    }

	    if (recv_active == 0) {

	      /* Determine reset value */
	      /* reset value = 0 for even scenarios, 255 for odd scenarios */
	      if (index == (scenarios - 1)) { /* index is going to reset */
		r = 0;
	      } else { 
		r = (index+1) % 2; /* base reset value on next index value */
	      }
	      
	      /* Reset __recv_buffer for next payload */
	      for (i = 0; i < 2048; i++) {
		__recv_buffer[i] = reset_value[r];
	      }

	      recv_active = 1;
	    }

	    fprintf (stderr, "===== PAMI_Send_immediate() FUNCTIONAL Test [use_shmem=%s][%s][+/-1 bit = %d] %zu %zu (%zu, %zu) -> (0, 0) =====\n\n", &hint_str[hint][0], &xtalk_str[xtalk][0], task0_buffer_default, header_bytes[index], data_bytes[index], _my_task, xtalk);

	    if (debug) {
	      fprintf (stderr, "before PAMI_Send_immediate ...\n");
	    }

	    result = PAMI_Send_immediate (context[xtalk], &im_parameters);

	    if (debug) {
	      fprintf (stderr, "... after PAMI_Send_immediate.\n");
	    }

	    if (result != PAMI_SUCCESS) {
	      fprintf (stderr, "ERROR (E):  PAMI_Send_immediate failed with rc = %d\n", result);
	      return 1;

	    }
	  } /* end scenario loop */
	} /* end xtalk loop */
      } /* end device loop */
    } /* end task id != 0 */
  } /* end run tests expected to pass */

  /* ======== Combinations of header and payload sizes that should result in send FAILS  ======== */
  if ( tests & 1 ) { 
    pami_result_t send_fail = PAMI_INVAL; /* In case it changes, only have to change it here */

    if (_my_task == 0)
      {

	fprintf(stdout, "====== Combinations of header and payload sizes that should result in send FAILS ======\n");

	for(hint = 0; hint < hint_limit; hint++) {             /* hint/device loop */

	  /* Skip hints that are not under test */
	  if ( ! ((hints_to_test >> hint) & 1) ) {
	    continue;
	  }

	  im_parameters.dispatch = hint;

	  for(xtalk = 0; xtalk < num_contexts; xtalk++) {      /* xtalk loop */

	    /* Run the following scenarios:               */
	    /* header = 0             payload = max + 1   */
	    /* header = max/2         payload = max/2 + 1 */
	    /* header = max + 1       payload = 0         */

	    /* Populate header and payload size arrays */
	    scenarios = 0;
	    header_bytes[scenarios++] = 0;
	    header_bytes[scenarios++] = dispatch_send_immediate_max[xtalk][hint]/2;
	    header_bytes[scenarios++] = dispatch_send_immediate_max[xtalk][hint] + 1;

	    scenarios = 0;
	    data_bytes[scenarios++] = dispatch_send_immediate_max[xtalk][hint] + 1;
	    data_bytes[scenarios++] = dispatch_send_immediate_max[xtalk][hint]/2 + 1;
	    data_bytes[scenarios++] = 0;

	    for (index=0; index < scenarios; index++) {         /* scenario loop */
	      im_parameters.header.iov_len = header_bytes[index];
	      im_parameters.data.iov_len = data_bytes[index];
	    
	      /* Communicate with each task */
	      for (n = 1; n < num_tasks; n++) {

		/* Skip sends to tasks that are unaddressable by rank 0 with current hint value */
		if ( addressable_by_me[hint][n] != 1 ) {

		  /* If this is first send, inform user of skip */
		  if ( ! ( xtalk || index ) ) {
		    fprintf(stderr, "WARNING (W):  Rank %zu is unaddressable by Rank %zu when use_shmem = %s. Skipping until next use_shmem hint ...\n", n, _my_task, &hint_str[hint][0]);
		  }

		  continue;

		}

		result = PAMI_Endpoint_create (client, n, xtalk, &im_parameters.dest);
		if (result != PAMI_SUCCESS) {
		  fprintf (stderr, "ERROR (E):  PAMI_Endpoint_create failed for task ID %zu, context %zu with %d.\n", n, xtalk, result);
		  return 1;
		}

		fprintf (stderr, "===== PAMI_Send_immediate() FUNCTIONAL Test [use_shmem=%s][%s] %zu %zu (%zu, 0) -> (%zu, %zu) =====\n\n", &hint_str[hint][0], &xtalk_str[xtalk][0], header_bytes[index], data_bytes[index], _my_task, n, xtalk);

		if (debug) {
		  fprintf (stderr, "before PAMI_Send_immediate ...\n");
		}

		result = PAMI_Send_immediate (context[0], &im_parameters);
		
		if (debug) {
		  fprintf (stderr, "... after PAMI_Send_immediate.\n");
		}

		if (result != send_fail) {
		  fprintf (stderr, "ERROR (E):  PAMI_Send_immediate passed with rc = %d ...should have FAILED with rc = %d!!\n", result, send_fail);
		  return 1;
		}
	      } /* end task id loop */
	    } /* end scenario loop */
	  } /* end xtalk loop */
	} /* end device loop */
      } /* end task = 0 */
    else {
      for(hint = 0; hint < hint_limit; hint++) {      /* hint/device loop */

	/* Skip hints that are not under test */
	if ( ! ((hints_to_test >> hint) & 1) ) {
	  continue;
	}

	/* Skip sends to task 0 that are unaddressable by current rank with current hint value */
	if ( addressable_by_me[hint][0] != 1 ) {
	  if (debug) {
	    fprintf(stderr, "WARNING (W):  Rank 0 is unaddressable by Rank %zu when use_shmem = %s. Skipping to next use_shmem hint ...\n", _my_task, &hint_str[hint][0]);
	  }
	  continue;
	}

	im_parameters.dispatch = hint;

	for(xtalk = 0; xtalk < num_contexts; xtalk++) {        /* xtalk loop */

	  result = PAMI_Endpoint_create (client, 0, 0, &im_parameters.dest);
	  if (result != PAMI_SUCCESS) {
	    fprintf (stderr, "ERROR (E):  PAMI_Endpoint_create failed for task ID 0, context 0 with %d.\n", result);
	    return 1;
	  }
	       
	  /* Run the following scenarios:               */
	  /* header = 0             payload = max + 1   */
	  /* header = max/2         payload = max/2 + 1 */
	  /* header = max + 1       payload = 0         */

	  /* Populate header and payload size arrays */
	  scenarios = 0;
	  header_bytes[scenarios++] = 0;
	  header_bytes[scenarios++] = dispatch_send_immediate_max[xtalk][hint]/2;
	  header_bytes[scenarios++] = dispatch_send_immediate_max[xtalk][hint] + 1;

	  scenarios = 0;
	  data_bytes[scenarios++] = dispatch_send_immediate_max[xtalk][hint] + 1;
	  data_bytes[scenarios++] = dispatch_send_immediate_max[xtalk][hint]/2 + 1;
	  data_bytes[scenarios++] = 0;

	  for (index=0; index < scenarios; index++) {         /* test loop */
	    im_parameters.header.iov_len = header_bytes[index];
	    im_parameters.data.iov_len = data_bytes[index];

	    fprintf (stderr, "===== PAMI_Send_immediate() FUNCTIONAL Test [use_shmem=%s][%s] %zu %zu (%zu, %zu) -> (0, 0) =====\n\n", &hint_str[hint][0], &xtalk_str[xtalk][0], header_bytes[index], data_bytes[index], _my_task, xtalk);

	    if (debug) {
	      fprintf (stderr, "before PAMI_Send_immediate ...\n");
	    }

	    result = PAMI_Send_immediate (context[xtalk], &im_parameters);

	    if (debug) {
	      fprintf (stderr, "... after PAMI_Send_immediate.\n");
	    }

	    if (result != send_fail) {
	      fprintf (stderr, "ERROR (E):  PAMI_Send_immediate passed with rc = %d ...should have FAILED with rc = %d!!\n", result, send_fail);
	      return 1;
	    }
	  } /* end scenario loop */
	} /* end xtalk loop */
      } /* end device loop */
    } /* end task id != 0 */
  } /* end run tests expected to fail */
  /* ====== CLEANUP ====== */

  result = PAMI_Context_destroyv(context, num_contexts);
  if (result != PAMI_SUCCESS) {
    fprintf (stderr, "ERROR (E):  Unable to destroy context(s), result = %d\n", result);
    return 1;
  }

  result = PAMI_Client_destroy(&client);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "ERROR (E):  Unable to destroy pami client. result = %d\n", result);
    return 1;
  }

  if ( (__header_errors > 0) || (__data_errors > 0) ) {
    fprintf (stdout, "ERROR (E):  immediate_send_overflow FAILED with %zu header errors and %zu data errors on task %zu!!\n", __header_errors, __data_errors, _my_task);
    return 1;
  }
  else
  {
    fprintf (stdout, "Success (%zu)\n", _my_task);
    return 0;
  }
}
