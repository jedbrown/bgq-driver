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
 * \file test/api/p2p/send/shmem-matrix.c
 * \Matrixed "shmem" point-to-point PAMI_send() test
 */

#include <pami.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

static const char *optString = "DSXdhu?";

/*
PAMI_HINT_DEFAULT   = 0, This hint leaves the option up to the PAMI implementation to choose.
PAMI_HINT_ENABLE    = 1, This allows the user to force an option to be used.
PAMI_HINT_DISABLE   = 2, The user can force the implementation to not use this option.
PAMI_HINT_INVALID   = 3, The largest hint value; it cannot actually be used.
*/

/*
use_shmem multi-value table:

TC = 4-digit binary, based on 2-digit send hard hit and 2-digit send soft hint (0bhhss). ex: send hint set to 1 in disptach ID (hard hint) and 2 using soft hint (send hint = 1->2 => 0b0110 = 6)

dpID = 4-digit binary, based on 2-digit send hard hint and 2-digit recv hard hint (0bssrr). ex: send hint = 1->2 => 01 (since hard hint = 1), recv hint = 1 => 01, results in 0b0101 = 5.  

NOTE:  dpID in the table is based on rank 0's view. Therefore, there are "paired" dpIDs to test the same TC in both directions. ex: When rank 0 sends to rank n, dpID 4 (0b0100 => PAMI_HINT_ENABLE for rank 0, and PAMI_HINT_DEFAULT for rank n) is used.  For the return message (rank n -> rank 0), dpID 1 (0b0001 => PAMI_HINT_DEFAULT for rank 0 and PAMI_HINT_ENABLE for rank n) is used.  
  
send hints 	
(hard)->(soft)  recv hints	TC	dpID 	shmem	exp result
==========      ==========      ==      ====    =====   ==========
0   	   	0   	   	0	0	maybe	pass
0->1   	    	0   	      	1	0	maybe	pass
0->2   		0   		2	0	maybe	pass
0->3   		0   		3	0	maybe	pass
1   	    	0    	      	4	4	yes	pass 
1->2   	    	0    	      	6	4	yes	pass 
1->3   	    	0    	      	7	4	yes	pass 
2	   	0    	   	8	8	no	pass 
2->1	   	0    	   	9	8	no	pass 
2->3	   	0    	   	11	8	no	pass 

0   	   	1   	   	0	1	maybe	unknown 
0->1   	    	1   	      	1	1	maybe	unknown 
0->2   		1  		2	1	maybe	unknown 
0->3   		1   		3	1	maybe	unknown 
1   	    	1    	      	4	5	yes	pass
1->2   	    	1    	      	6	5	yes	pass
1->3   	    	1    	      	7	5	yes	pass
2	   	1    	   	8	9	no      recv FAIL 
2->1	   	1    	   	9	9	no	recv FAIL 
2->3	   	1    	   	11	9	no	recv FAIL 

0   	   	2   	   	0	2	maybe	unknown 
0->1   	    	2   	      	1	2	maybe	unknown
0->2   		2   		2	2	maybe	unknown
0->3   		2   		3	2	maybe	unknown
1   	    	2    	      	4	6	yes	recv FAIL
1->2   	    	2    	      	6	6	yes	recv FAIL
1->3   	    	2    	      	7	6	yes	recv FAIL
2	   	2    	   	8	10	no	pass
2->1	   	2    	   	9	10	no	pass
2->3	   	2    	   	11	10	no	pass

3               D/C		D/C	3	D/C	FAIL

use_shmem >= 3 should fail on PAMI_Dispatch_set with PAMI_INVAL
*/

/*char device_str[3][50] = {"DEFAULT", "SHMem", "MU"};*/
char hint_str[4][50] = {"PAMI_HINT_DEFAULT", "PAMI_HINT_ENABLE", "PAMI_HINT_DISABLE", "PAMI_HINT_INVALID"};
char exp_str[5][50] = {"PASS", "SEND FAIL", "RECV FAIL", "UNPREDICTABLE", "INVALID"};
char xtalk_str[2][50] = {"no crosstalk", "crosstalk"};
char callback_str[2][50] = {"no callback", "callback"};
char longheader_str[2][50] = {"no long header", "long header"};
char debug_str[500];

size_t debug = 0;
size_t task_id;
uint8_t __recv_buffer[2048];
size_t __recv_size;
size_t __header_errors = 0;
size_t __data_errors = 0;
uint8_t reset_value[2] = {0, 255}; /* reset value for each byte of __recv_buffer ...all 0's or all 1's (255) */

unsigned validate (const void * addr, size_t bytes)
{
  unsigned status = 0;
  uint8_t * byte = (uint8_t *) addr;
  size_t i;

  /* Skip validation if buffer size is 0 */
  if (bytes == 0) {
    return 2;
  }

  for (i=0; i<bytes; i++) {
    if (byte[i] != (uint8_t)i) {
      fprintf (stderr, "ERROR (E):validate(%p,%zu):  byte[%zu] != %d (&byte[%zu] = %p, value is %d)\n", addr, bytes, i, (uint8_t)i, i, &byte[i], byte[i]);
      status = 1;
    }
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
  validateRC = validate(__recv_buffer, __recv_size);

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
  validateRC = validate(header_addr, header_size);

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
    memcpy(__recv_buffer, pipe_addr, pipe_size);
    recv_done (context, cookie, PAMI_SUCCESS);
  } else {
    /* This is an 'asynchronous' receive */

    __recv_size = pipe_size;

    recv->local_fn = recv_done;
    recv->cookie   = cookie;
    recv->type     = PAMI_TYPE_BYTE;
    recv->addr     = __recv_buffer;
    recv->offset   = 0;
    recv->data_fn  = PAMI_DATA_COPY;
    recv->data_cookie = NULL;
    /*fprintf (stderr, "... dispatch function.  recv->local_fn = %p\n", recv->local_fn); */
  }

  return;
}
/* ---------------------------------------------------------------*/

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

void display_usage( void )
{
    
  printf("This is the help text for shmem-matrix.c:\n");
  printf("shmem-matrix.c by default will attempt to pass messages of varying header and payload sizes between rank 0 and all other ranks (requires >= 2 ranks) using all available use_shmem hint values.\n");
  printf("\n");
  printf("The user can select a subset of the available hints by using the parms below:\n");
  printf("-D | --D PAMI_HINT_DEFAULT\n");
  printf("-S | --S PAMI_HINT_ENABLE\n");
  printf("-X | --X PAMI_HINT_DISABLE\n");
  printf("\n");
/*  printf("NOTE:  For BGQ, available use_shmem hint values also vary based on PAMI_DEVICE value.\n");
  printf("\tPAMI_DEVICE\tAvailable use_shmem values\n");
  printf("\t===========\t==========================\n");
  printf("\tB or unset\tPAMI_HINT_DEFAULT, PAMI_HINT_ENABLE & PAMI_HINT_DISABLE\n");
  printf("\tM\t\tPAMI_HINT_DEFAULT & PAMI_HINT_DISABLE (BGQ)\n");
  printf("\tS\t\tPAMI_HINT_DEFAULT & PAMI_HINT_ENABLE\n");
  printf("\n");*/
  printf("-d  Enable error tracing messages to stderr for debug.\n");
  printf("\n");
  printf("-u  Run testcases with mismatched hints resulting in unpredictable outcomes.\n");
  printf("\n");
  printf("Parms can be provided separately or together.\n");
  printf("\tex:  shmem-matrix.cnk -D -S --debug\n");
  printf("\tex:  shmem-matrix.cnk --XS\n");
  printf("\n");
}
/* ---------------------------------------------------------------*/

int main (int argc, char ** argv)
{

  int opt = 0;

  size_t i, j;
  size_t run_unpredictable = 0;   /* run mismatched hint TCs that have unpredictable results */

  size_t hint_limit = 3;
  size_t available_hints = 7; /* (bit 0 = Default, bit 1 = S, bit 2 = M) */
  size_t hints_to_test = 0;   /* (bit 0 = Default, bit 1 = S, bit 2 = M) */
  size_t hint, n = 0;

  volatile size_t send_active = 1;
  volatile size_t recv_active = 1;


  pami_client_t client;
  pami_context_t context[2]; /* in case we do xtalk in the future */       
  char                  cl_string[] = "TEST";
  pami_result_t result = PAMI_ERROR;

  /* See if user passed in any args */
  opt = getopt( argc, argv, optString );
  while( opt != -1 ) {
    switch( opt ) {
    case 'D':
      hints_to_test = hints_to_test | 1;	  
      break;
                
    case 'd':
      debug = 1;
      break;
                
    case 'M':
      hints_to_test = hints_to_test | 4;
      break;
                
    case 'S':
      hints_to_test = hints_to_test | 2;
      break;
                
    case 'u':
      run_unpredictable = 1;
      break;

    case 'h':   /* fall-through is intentional */
    case '?':
      if (task_id == 0) {
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
  if ( (result != PAMI_SUCCESS) && debug ) {
    fprintf(stderr, "ERROR (E): Unable to initialize pami client. result = %d\n", result);
    
    return 1;
  }

  pami_configuration_t configuration;

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
  /*
  if (max_contexts > 1) {
    num_contexts = 2;
  }
  */

  result = PAMI_Context_createv(client, NULL, 0, context, num_contexts);
  if ( (result != PAMI_SUCCESS) && debug ) {
    fprintf(stderr, "ERROR (E): Unable to create pami context. result = %d\n", result);
    return 1;
  }
 
  /* Get my task ID */
  configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query(client, &configuration,1);

  if ( (result != PAMI_SUCCESS) && debug ) {
    fprintf(stderr, "ERROR (E): Unable query configuration (%d). result = %d\n", configuration.name, result);
    return 1;
  }

  size_t task_id = configuration.value.intval;
  if (debug) {
    fprintf(stderr, "My task id = %zu\n", task_id);
  }

  /* Ensure we have >= 2 tasks */
  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration,1);

  if ( (result != PAMI_SUCCESS) && debug ) {
    fprintf(stderr, "ERROR (E): Unable query configuration (%d). result = %d\n", configuration.name, result);
    return 1;
  }

  size_t num_tasks = configuration.value.intval;
  if (debug) {
    fprintf(stderr, "Number of tasks = %zu\n", num_tasks);
  }

  if (num_tasks < 2) {
    fprintf(stderr, "ERROR (E): This test requires >= 2 tasks. Number of tasks in this job: %zu\n", num_tasks);
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

  /* Create an array for storing device addressabilty based on hint value */
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
      sprintf (debug_str, "Tasks local to task %zu:", task_id);
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
    sprintf (debug_str, "task %zu SHMem addressable tasks:  ", task_id);

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
      if (task_id == 0) {
	fprintf (stdout, "Only the %s device is initialized.\n", &device_str[2][0]);
      }
      available_hints = 5;
    } else if ( strcmp(device, "S") == 0 ) {
      if (task_id == 0) {
	fprintf (stdout, "Only the SHMem device is initialized.\n");
      }
      available_hints = 3;
    } else if ( strcmp(device, "B") == 0 ) {
      if (task_id == 0) {
	fprintf (stdout, "Both the %s and SHMem devices are initialized.\n", &device_str[2][0]);
      }
    } else {
      if (task_id == 0) {
	fprintf (stderr, "ERROR (E):  PAMI_DEVICE = %s is unsupported. Valid values are:  M (%s only), S (SHMem only) & [ B | unset ] (both %s & SHMem)\n", device, &device_str[2][0], &device_str[2][0]);
      }
      return 1;
    } 
  } else {
    if (task_id == 0) {
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
  if (task_id == 0) {
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

  /* Create/Init  header and data buffers */ 
  uint8_t header[1024];
  uint8_t data[1024];

  for (i=0; i<1024; i++) {
    header[i] = (uint8_t)i;
    data[i]   = (uint8_t)i;
  }

  size_t h, hsize = 0;
  size_t header_bytes[16];
  header_bytes[hsize++] = 0;
  header_bytes[hsize++] = 16;
  header_bytes[hsize++] = 32;

  size_t p, psize = 0;
  size_t data_bytes[16];
  /*data_bytes[psize++] = 0; */
  /*data_bytes[psize++] = 16; */
  /*data_bytes[psize++] = 32; */
  /*data_bytes[psize++] = 64; */
  data_bytes[psize++] = 128;
  data_bytes[psize++] = 256;
  data_bytes[psize++] = 512;
  data_bytes[psize++] = 1024;

  size_t xtalk = 0;
  size_t remote_cb = 0;

  /* PAMI_Dispatch_set for addressability tests */

  /* Create 3 dispatch_info_t structures (1 for each use_shmem hint value) */
  typedef struct
  {
    size_t                 id;
    pami_dispatch_hint_t   options;
    char                 * name;
    pami_result_t          result;
  } dispatch_info_t;

  dispatch_info_t dispatch[3];

  dispatch[0].id = 20;
  dispatch[0].options = (pami_dispatch_hint_t) {0};
  dispatch[0].name = "  DEFAULT ";
  dispatch[0].options.use_shmem = PAMI_HINT_DEFAULT;

  dispatch[1].id = 21;
  dispatch[1].options = (pami_dispatch_hint_t) {0};
  dispatch[1].name = "ONLY SHMEM";
  dispatch[1].options.use_shmem = PAMI_HINT_ENABLE;

  dispatch[2].id = 22;
  dispatch[2].options = (pami_dispatch_hint_t) {0};
  dispatch[2].name = " NO SHMEM ";
  dispatch[2].options.use_shmem = PAMI_HINT_DISABLE;

  pami_dispatch_callback_function fn;
  fn.p2p = test_dispatch;

  size_t dispatch_recv_immediate_max[max_contexts][3];

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
      fprintf (stderr, "rank:  %zu\tnum local tasks:  %zu\n", task_id, num_local_tasks);
      }

      /* Can't create shmem dispatch ID if there aren't any other local tasks */
      if ( (hint == 1) && (num_local_tasks == 1) ) {
	fprintf(stderr, "WARNING (W):  No other local tasks exist with Rank %zu.  Skipping creation of \"SHMem\" dispatch ID ...\n", task_id);
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
      
      /* Find recv immediate max for this context/use_shmem combo */
      configuration.name = PAMI_DISPATCH_RECV_IMMEDIATE_MAX;
      dispatch[hint].result = PAMI_Dispatch_query(context[i], dispatch[hint].id, &configuration,1);
      if (dispatch[hint].result != PAMI_SUCCESS) {
	fprintf (stderr, "ERROR (E):  Unable to query configuration (%d). result = %d\n", configuration.name, dispatch[hint].result);
	return 1;
      }

      dispatch_recv_immediate_max[i][hint] = configuration.value.intval;

      if (task_id == 0) {
	fprintf (stdout, "Receive immediate maximum for context %zu dispatch ID %zu = %zu\n", i, dispatch[hint].id, dispatch_recv_immediate_max[i][hint]);
      }
    } /* end hint loop */
  } /* end context loop */

  /* PAMI_Dispatch_set for functional tests (valid for all tasks) */
  pami_dispatch_hint_t options = (pami_dispatch_hint_t) {0};

  for (i = 0; i < num_contexts; i++) { /* context loop */
    options.use_shmem = PAMI_HINT_DEFAULT;
    if (debug) {
      fprintf(stderr, "Before context %zu, ID 0 PAMI_Dispatch_set() ...&recv_active = %p, recv_active = %zu\n", i, &recv_active, recv_active);
    }
    result = PAMI_Dispatch_set (context[i],
				0,
				fn,
				(void *)&recv_active,
				options);
    if (result != PAMI_SUCCESS)
      {
	fprintf(stderr, "ERROR (E): Unable to register PAMI dispatch ID 0 on context %zu. result = %d\n", i, result);
	return 1;
      }

    options.use_shmem = PAMI_HINT_ENABLE;
    if (debug) {
      fprintf(stderr, "Before context %zu, ID 5 PAMI_Dispatch_set() ...&recv_active = %p, recv_active = %zu\n", i, &recv_active, recv_active);
    }

    if (debug) {
      fprintf (stderr, "rank:  %zu\tnum local tasks:  %zu\n", task_id, num_local_tasks);
    }

    /* Can't create shmem dispatch ID if there aren't any other local tasks */
    if ((options.use_shmem == PAMI_HINT_ENABLE) && (num_local_tasks == 1)) {
      fprintf(stderr, "WARNING (W):  No other local tasks exist with Rank %zu.  Skipping creation of \"SHMem\" dispatch ID ...\n", task_id);
    } else { /* create ID 5 */

      result = PAMI_Dispatch_set (context[i],
				  5,
				  fn,
				  (void *)&recv_active,
				  options);
      if (result != PAMI_SUCCESS)
	{
	  fprintf(stderr, "ERROR (E): Unable to register PAMI dispatch ID 5 on context %zu. result = %d\n", i, result);
	  return 1;
	}
    }

    options.use_shmem = PAMI_HINT_DISABLE;
    if (debug) {
      fprintf(stderr, "Before context %zu, ID 10 PAMI_Dispatch_set() ...&recv_active = %p, recv_active = %zu\n", i, &recv_active, recv_active);
    }
    result = PAMI_Dispatch_set (context[i],
				10,
				fn,
				(void *)&recv_active,
				options);
    if (result != PAMI_SUCCESS)
      {
	fprintf(stderr, "ERROR (E): Unable to register PAMI dispatch ID 10 on context %zu. result = %d\n", i, result);
	return 1;
      }

    /* Commenting this section out, since calling PAMI_Dispatch_set with PAMI_HINT_INVALID is A) not supported and B) results in a seg fault */
    /* From Mike Blocksome: */  
    /* We don't error-check for 'invalid' hints .. so anything could happen I guess */
    /* PAMI_HINT_INVALID is just there for enum completeness since we have two bits but only need 3 values */

    /*    options.use_shmem = PAMI_HINT_INVALID;*/
    /*    if (debug) {*/
    /*      fprintf(stderr, "Before context %zu, ID 3 PAMI_Dispatch_set() ...&recv_active = %p, recv_active = %zu\n", i, &recv_active, recv_active);*/
    /*    }*/
    /*    result = PAMI_Dispatch_set (context,*/
    /*				3,*/
    /*				fn,*/
    /*				(void *)&recv_active,*/
    /*				options);*/

    /*    if (result != PAMI_INVAL) {*/
      /* Make this a warning till it's implemented */
    /*      if (i == 0) {*/
    /*	fprintf(stderr, "WARNING (W): Should not be able to set use_shmem >= 3. result = %d\n", result);*/
    /*      }*/
      /*return 1; */
    /*    }*/
  } /* end context loop */

  /* Setup hint vars */
  size_t rv = 0;                             /* recv_buffer reset value */
  size_t s = 0;                              /* controls send hints loop */
  size_t r = 0;                              /* controls recv hints loop */
  size_t sh = 0;                             /* use this for shifted s value (send hard hint) */
  size_t send_hard_use_shmem_hint = 0;       /* hard hint value of sending task */
  size_t send_soft_use_shmem_hint = 0;       /* soft hint value of sending task */
  size_t recv_use_shmem_hint = 0;            /* hard hint value of receiving task */
  


  /* Create dispatch arrays */

  size_t dispatch_ary_0[3][12] = {{0, 0, 0, 0, 4, 0, 4, 4, 8, 8, 0, 8},
                                  {1, 1, 1, 1, 5, 0, 5, 5, 9, 9, 0, 9},
                                  {2, 2, 2, 2, 6, 0, 6, 6, 10, 10, 0, 10}};
  size_t dispatch_ary_n[3][12] = {{0, 0, 0, 0, 1, 0, 1, 1, 2, 2, 0, 2},
                                  {4, 4, 4, 4, 5, 0, 5, 5, 6, 6, 0, 6},
                                  {8, 8, 8, 8, 9, 0, 9, 9, 10, 10, 0, 10}};

  /*
 Expected outcome/enable array when both PAMI engines are enabled: 
 0 = pass, 1 = send fail, 2 = recv fail, 3 = unpredictable, 4 = invalid 
*/
  size_t exp_ary[3][12] =       {{0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 4, 0},
                                  {3, 3, 3, 3, 0, 4, 0, 0, 2, 2, 4, 2},
                                  {3, 3, 3, 3, 2, 4, 2, 2, 0, 0, 4, 0}};

  pami_send_t parameters;
  parameters.send.header.iov_base = header;
  parameters.send.data.iov_base   = data;
  parameters.events.cookie        = (void *) &send_active;
  parameters.events.local_fn      = send_done_local;
  memset(&parameters.send.hints, 0, sizeof(parameters.send.hints));
  /*  parameters.events.remote_fn     = send_done_remote;*/

  /* loop overrides */
  size_t remote_cb_init = 1; 
  size_t h_init = 1; hsize = 2; /* header_bytes[1] = 16 bytes */
  size_t p_init = 2; psize = 3; /* payload_bytes[2] = 512 bytes */

  /* ======== Combinations of use_shmem hints that should pass  ======== */

  if (task_id == 0)
  {

    fprintf(stderr, "======== Combinations of use_shmem hints that should pass  ========\n");

    /* Create task unique dispatch sets for each context */
    for (i = 0; i < num_contexts; i++) { /* context loop */
      options.use_shmem = PAMI_HINT_DEFAULT;
      if (debug) {
	fprintf(stderr, "Before context %zu, ID 1 PAMI_Dispatch_set() ...&recv_active = %p, recv_active = %zu\n", i, &recv_active, recv_active);
      }
      result = PAMI_Dispatch_set (context[i],
				  1,
				  fn,
				  (void *)&recv_active,
				  options);
      if (result != PAMI_SUCCESS)
	{
	  fprintf(stderr, "ERROR (E): Unable to register PAMI dispatch ID 1 on context %zu. result = %d\n", i, result);
	  return 1;
	}

      options.use_shmem = PAMI_HINT_DEFAULT;
      if (debug) {
	fprintf(stderr, "Before context %zu, ID 2 PAMI_Dispatch_set() ...&recv_active = %p, recv_active = %zu\n", i, &recv_active, recv_active);
      }
      result = PAMI_Dispatch_set (context[i],
				  2,
				  fn,
				  (void *)&recv_active,
				  options);
      if (result != PAMI_SUCCESS)
	{
	  fprintf(stderr, "ERROR (E): Unable to register PAMI dispatch ID 2 on context %zu. result = %d\n", i, result);
	  return 1;
	}

      options.use_shmem = PAMI_HINT_ENABLE;
      if (debug) {
	fprintf(stderr, "Before context %zu, ID 4 PAMI_Dispatch_set() ...&recv_active = %p, recv_active = %zu\n", i, &recv_active, recv_active);
      }

      if (debug) {
	fprintf (stderr, "rank:  %zu\tnum local tasks:  %zu\n", task_id, num_local_tasks);
      }

      /* Can't create shmem dispatch ID if there aren't any other local tasks */
      if ((options.use_shmem == PAMI_HINT_ENABLE) && (num_local_tasks == 1)) {
	fprintf(stderr, "WARNING (W):  No other local tasks exist with Rank %zu.  Skipping creation of \"SHMem\" dispatch ID ...\n", task_id);
      } else { /* create IDs 4 & 6 */

	result = PAMI_Dispatch_set (context[i],
				    4,
				    fn,
				    (void *)&recv_active,
				    options);
	if (result != PAMI_SUCCESS)
	  {
	    fprintf(stderr, "ERROR (E): Unable to register PAMI dispatch ID 4 on context %zu. result = %d\n", i, result);
	    return 1;
	  }
   

	options.use_shmem = PAMI_HINT_ENABLE;
	if (debug) {
	  fprintf(stderr, "Before context %zu, ID 6 PAMI_Dispatch_set() ...&recv_active = %p, recv_active = %zu\n", i, &recv_active, recv_active);
	}
	result = PAMI_Dispatch_set (context[i],
				    6,
				    fn,
				    (void *)&recv_active,
				    options);
	if (result != PAMI_SUCCESS)
	  {
	    fprintf(stderr, "ERROR (E): Unable to register PAMI dispatch ID 6 on context %zu. result = %d\n", i, result);
	    return 1;
	  }
      }

      options.use_shmem = PAMI_HINT_DISABLE;
      if (debug) {
	fprintf(stderr, "Before context %zu, ID 8 PAMI_Dispatch_set() ...&recv_active = %p, recv_active = %zu\n", i, &recv_active, recv_active);
      }
      result = PAMI_Dispatch_set (context[i],
				  8,
				  fn,
				  (void *)&recv_active,
				  options);
      if (result != PAMI_SUCCESS)
	{
	  fprintf(stderr, "ERROR (E): Unable to register PAMI dispatch ID 8 on context %zu. result = %d\n", i, result);
	  return 1;
	}

      options.use_shmem = PAMI_HINT_DISABLE;
      if (debug) {
	fprintf(stderr, "Before context %zu, ID 9 PAMI_Dispatch_set() ...&recv_active = %p, recv_active = %zu\n", i, &recv_active, recv_active);
      }
      result = PAMI_Dispatch_set (context[i],
				  9,
				  fn,
				  (void *)&recv_active,
				  options);
      if (result != PAMI_SUCCESS)
	{
	  fprintf(stderr, "ERROR (E): Unable to register PAMI dispatch ID 9 on context %zu. result = %d\n", i, result);
	  return 1;
	}
    } /* end context loop */


    for(xtalk = 0; xtalk < num_contexts; xtalk++) {        /* xtalk loop */

      for (remote_cb = remote_cb_init; remote_cb < 2; remote_cb++) {    /* remote callback loop */
	if (remote_cb) {
	  parameters.events.remote_fn     = send_done_remote;
	} else {
	  parameters.events.remote_fn     = NULL;
	}

	for (r = 0; r < 3; r++) {
	  for (s = 0; s < 12; s++) {

	    /* Determine hint values */
	    sh = s;
	    send_hard_use_shmem_hint = (sh >> 2) & 3;
	    send_soft_use_shmem_hint = s & 3;
	    recv_use_shmem_hint = r;

	    /* Skip mismatched scenarios with unpredictable results, and hints not under test */
	    if (exp_ary[r][s] || ! ((hints_to_test >> send_hard_use_shmem_hint) & 1) || ! ((hints_to_test >> recv_use_shmem_hint) & 1) ) {

	      if (debug) {
		fprintf(stderr, "WARNING (W):  Skipping testcase (s = %zu, r = %zu, dispatch ID = %zu, send hard hint = %s, recv hint = %s, expectation = %s\n", s, r, dispatch_ary_0[r][s], &hint_str[send_hard_use_shmem_hint][0], &hint_str[recv_use_shmem_hint][0], &exp_str[exp_ary[r][s]][0]);
	      }

	      /* Reset __recv_buffer based on next send hint TC */
	      /* 0's for even TCs and 255's for odd TCs */
	      rv = (s+1) % 2;

	      for (i = 0; i < 2048; i++) {
		__recv_buffer[i] = reset_value[rv];
	      }

	      continue;
	    }

	    parameters.send.dispatch        = dispatch_ary_0[r][s];

	    for (h=h_init; h<hsize; h++) {                          /* header loop */
	      parameters.send.header.iov_len = header_bytes[h];
	      for (p=p_init; p<psize; p++) {                        /* payload loop */
		parameters.send.data.iov_len = data_bytes[p];

		/* Communicate with each task */
		for (n = 1; n < num_tasks; n++) {

		  /* Skip sends to tasks that are unaddressable by rank 0 with current hint value */
		  /* We'll cover failing scenarios in the next section */
		  if ( addressable_by_me[send_hard_use_shmem_hint][n] != 1 ) {

		    /* If this is first send, inform user of skip */
		    if ( (xtalk == 0) && (remote_cb == remote_cb_init) && (h == h_init) && (p == p_init) ) {
		      fprintf(stderr, "WARNING (W):  Rank %zu is unaddressable by Rank %zu when use_shmem = %s. Skipping ...\n", n, task_id, &hint_str[send_hard_use_shmem_hint][0]);
		    }

		    continue;
		  }

		  result = PAMI_Endpoint_create (client, n, 0, &parameters.send.dest);
		  if (result != PAMI_SUCCESS) {
		    fprintf (stderr, "ERROR (E):  PAMI_Endpoint_create FAILED for %zu with %d.\n", n, result);
		    return 1;
		  }

		  fprintf (stderr, "===== PAMI_Send() FUNCTIONAL Test [%s][%s] %zu %zu (%zu, 0) -> (%zu, %zu) =====\n\t\ttask %zu use_shmem hard hint = %s\n\t\ttask %zu use_shmem soft hint = %s\n\t\ttask %zu use shmem hard hint = %s\n", &xtalk_str[xtalk][0], &callback_str[remote_cb][0], header_bytes[h], data_bytes[p], task_id, n, xtalk, task_id, &hint_str[send_hard_use_shmem_hint][0], task_id, &hint_str[send_soft_use_shmem_hint][0], n, &hint_str[recv_use_shmem_hint][0]);

		  if (remote_cb) {
		    send_active++;
		  }

		  if (debug) {
		    fprintf (stderr, "before PAMI_Send ...\n");
		  }

		  result = PAMI_Send (context[xtalk], &parameters);

		  if (debug) {
		    fprintf (stderr, "... after PAMI_Send.\n");
		  }

		  if (result != PAMI_SUCCESS)
		    {
		      fprintf(stderr, "ERROR (E): [%s][%s] %zu %zu PAMI_Send from (%zu, 0) (use_shmem hard hint = %s, use_shmem soft hint = %s) to (%zu, %zu) (use_shmem hard hint = %s) FAILED with rc = %d\n", &xtalk_str[xtalk][0], &callback_str[remote_cb][0], header_bytes[h], data_bytes[p], task_id, &hint_str[send_hard_use_shmem_hint][0], &hint_str[send_soft_use_shmem_hint][0], n, xtalk, &hint_str[recv_use_shmem_hint][0], result);

		      return 1;
		    }

		  if (debug) {
		    fprintf(stderr, "before send-recv advance loop ...\n");
		  }

		  while (send_active || recv_active)
		    {
		      result = PAMI_Context_advance (context[0], 100);
		      if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
			{
			  fprintf(stderr, "ERROR (E): Unable to advance pami context 0. result = %d\n", result);
			  return 1;
			}
		    }
	  
		  if (debug) {
		    fprintf(stderr, "... after send-recv advance loop\n");
		  }

		  send_active = 1;
		  recv_active = 1;

		} /* end task loop */

		/* Reset __recv_buffer based on next send hint TC */
		/* 0's for even TCs and 255's for odd TCs */
		if ( (h == (hsize - 1)) && (p == (psize - 1)) ) { /* last header/payload combo for this TC */
		  rv = s % 2;
		} else {
		  rv = (s+1) % 2;
		}

		for (i = 0; i < 2048; i++) {
		  __recv_buffer[i] = reset_value[rv];
		}
	      } /* end payload loop */
	    } /* end header loop */
	  } /* end send hint loop */
	} /* end recv hint loop */
      } /* end remote callback loop */
    } /* end context loop */
  } /* end task = 0 */
  else { /* task id > 0 */

    /* Create task unique dispatch sets for each context */
    for (i = 0; i < num_contexts; i++) { /* context loop */

      options.use_shmem = PAMI_HINT_ENABLE;
      if (debug) {
	fprintf(stderr, "Before context %zu, ID 1 PAMI_Dispatch_set() ...&recv_active = %p, recv_active = %zu\n", i, &recv_active, recv_active);
      }

      if (debug) {
	fprintf (stderr, "rank:  %zu\tnum local tasks:  %zu\n", task_id, num_local_tasks);
      }

      /* Can't create shmem dispatch ID if there aren't any other local tasks */
      if ((options.use_shmem == PAMI_HINT_ENABLE) && (num_local_tasks == 1)) {
	fprintf(stderr, "WARNING (W):  No other local tasks exist with Rank %zu.  Skipping creation of \"SHMem\" dispatch ID ...\n", task_id);
      } else { /* create ID 1 */

	result = PAMI_Dispatch_set (context[i],
				    1,
				    fn,
				    (void *)&recv_active,
				    options);
	if (result != PAMI_SUCCESS)
	  {
	    fprintf(stderr, "ERROR (E): Unable to register PAMI dispatch ID 1 on context %zu. result = %d\n", i, result);
	    return 1;
	  }
      }

      options.use_shmem = PAMI_HINT_DISABLE;
      if (debug) {
	fprintf(stderr, "Before context %zu, ID 2 PAMI_Dispatch_set() ...&recv_active = %p, recv_active = %zu\n", i, &recv_active, recv_active);
      }
      result = PAMI_Dispatch_set (context[i],
				  2,
				  fn,
				  (void *)&recv_active,
				  options);
      if (result != PAMI_SUCCESS)
	{
	  fprintf(stderr, "ERROR (E): Unable to register PAMI dispatch ID 2 on context %zu. result = %d\n", i, result);
	  return 1;
	}

      options.use_shmem = PAMI_HINT_DEFAULT;
      if (debug) {
	fprintf(stderr, "Before context %zu, ID 4 PAMI_Dispatch_set() ...&recv_active = %p, recv_active = %zu\n", i, &recv_active, recv_active);
      }    
      result = PAMI_Dispatch_set (context[i],
				  4,
				  fn,
				  (void *)&recv_active,
				  options);
      if (result != PAMI_SUCCESS)
	{
	  fprintf(stderr, "ERROR (E): Unable to register PAMI dispatch ID 4 on context %zu. result = %d\n", i, result);
	  return 1;
	}

      options.use_shmem = PAMI_HINT_DISABLE;
      if (debug) {
	fprintf(stderr, "Before context %zu, ID 6 PAMI_Dispatch_set() ...&recv_active = %p, recv_active = %zu\n", i, &recv_active, recv_active);
      }
      result = PAMI_Dispatch_set (context[i],
				  6,
				  fn,
				  (void *)&recv_active,
				  options);
      if (result != PAMI_SUCCESS)
	{
	  fprintf(stderr, "ERROR (E): Unable to register PAMI dispatch ID 6 on context %zu. result = %d\n", i, result);
	  return 1;
	}

      options.use_shmem = PAMI_HINT_DEFAULT;
      if (debug) {
	fprintf(stderr, "Before context %zu, ID 8 PAMI_Dispatch_set() ...&recv_active = %p, recv_active = %zu\n", i, &recv_active, recv_active);
      }    
      result = PAMI_Dispatch_set (context[i],
				  8,
				  fn,
				  (void *)&recv_active,
				  options);
      if (result != PAMI_SUCCESS)
	{
	  fprintf(stderr, "ERROR (E): Unable to register PAMI dispatch ID 8 on context %zu. result = %d\n", i, result);
	  return 1;
	}

      options.use_shmem = PAMI_HINT_ENABLE;
      if (debug) {
	fprintf(stderr, "Before context %zu, ID 9 PAMI_Dispatch_set() ...&recv_active = %p, recv_active = %zu\n", i, &recv_active, recv_active);
      }

      if (debug) {
	fprintf (stderr, "rank:  %zu\tnum local tasks:  %zu\n", task_id, num_local_tasks);
      }

      /* Can't create shmem dispatch ID if there aren't any other local tasks */
      if ((options.use_shmem == PAMI_HINT_ENABLE) && (num_local_tasks == 1)) {
	fprintf(stderr, "WARNING (W):  No other local tasks exist with Rank %zu.  Skipping creation of \"SHMem\" dispatch ID ...\n", task_id);
      } else { /* create ID 9 */

	result = PAMI_Dispatch_set (context[i],
				    9,
				    fn,
				    (void *)&recv_active,
				    options);
	if (result != PAMI_SUCCESS)
	  {
	    fprintf(stderr, "ERROR (E): Unable to register PAMI dispatch ID 9 on context %zu. result = %d\n", i, result);
	    return 1;
	  }
      }
    } /* end context loop */

    result = PAMI_Endpoint_create (client, 0, 0, &parameters.send.dest);
    if (result != PAMI_SUCCESS) {
      fprintf (stderr, "ERROR:  PAMI_Endpoint_create failed with %d.\n", result);
      return 1;
    }

    for(xtalk = 0; xtalk < num_contexts; xtalk++) {        /* xtalk loop */

      for (remote_cb = remote_cb_init; remote_cb < 2; remote_cb++) {    /* remote callback loop */
	if (remote_cb) {
	  parameters.events.remote_fn     = send_done_remote;
	} else {
	  parameters.events.remote_fn     = NULL;
	}

	for (r = 0; r < 3; r++) {
	  for (s = 0; s < 12; s++) {

	    /* Determine hint values */
	    sh = s;
	    send_hard_use_shmem_hint = (sh >> 2) & 3;
	    send_soft_use_shmem_hint = s & 3;
	    recv_use_shmem_hint = r;

	    /* Skip mismatched scenarios with unpredictable results, hints not under test, and unaddressable ranks */
	    if (exp_ary[r][s] || ! ((hints_to_test >> send_hard_use_shmem_hint) & 1) || ! ((hints_to_test >> recv_use_shmem_hint) & 1) || (addressable_by_me[send_hard_use_shmem_hint][0] != 1) ) {

	      if (debug) {
		fprintf(stderr, "WARNING (W):  Skipping testcase (s = %zu, r = %zu, dispatch ID = %zu, send hard hint = %s, recv hint = %s, expectation = %s\n", s, r, dispatch_ary_n[r][s], &hint_str[send_hard_use_shmem_hint][0], &hint_str[recv_use_shmem_hint][0], &exp_str[exp_ary[r][s]][0]);
	      }

	      /* Reset __recv_buffer based on next send hint TC */
	      /* 0's for even TCs and 255's for odd TCs */
	      rv = (s+1) % 2;

	      for (i = 0; i < 2048; i++) {
		__recv_buffer[i] = reset_value[rv];
	      }

	      continue;
	    }

	    for (h=h_init; h<hsize; h++) {                          /* header loop */
	      parameters.send.header.iov_len = header_bytes[h];
	      for (p=p_init; p<psize; p++) {                        /* payload loop */
		parameters.send.data.iov_len = data_bytes[p];

		if (debug) {
		  fprintf(stderr, "before recv advance loop ...\n");
		}

		while (recv_active != 0)
		  {
		    result = PAMI_Context_advance (context[xtalk], 100);
		    if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
		      {
			fprintf(stderr, "ERROR (E): Unable to advance pami context %zu. result = %d\n", xtalk, result);
			return 1;
		      }
		  }
	
		if (debug) {
		  fprintf(stderr, "... after recv advance loop\n");
		}

		/* Reset __recv_buffer based on next send hint TC */
		if (recv_active == 0) {

		  /* 0's for even TCs and 255's for odd TCs */
		  if ( (h == (hsize - 1)) && (p == (psize - 1)) ) { /* last header/payload combo for this TC */
		    rv = s % 2;
		  } else {
		    rv = (s+1) % 2;
		  }

		  for (i = 0; i < 2048; i++) {
		    __recv_buffer[i] = reset_value[rv];
		  }

		  recv_active = 1;
		}

		if (remote_cb) {
		  send_active++;
		}

		if (debug) {
		  fprintf(stderr, "before PAMI_Send ...\n");
		}

		parameters.send.dispatch        = dispatch_ary_n[r][s];

		fprintf (stderr, "===== PAMI_Send() FUNCTIONAL Test [%s][%s] %zu %zu (%zu, %zu) -> (0, 0) =====\n\t\ttask %zu use_shmem hard hint = %s\n\t\ttask %zu use_shmem soft hint = %s\n\t\ttask 0 use shmem hard hint = %s\n", &xtalk_str[xtalk][0], &callback_str[remote_cb][0], header_bytes[h], data_bytes[p], task_id, xtalk, task_id, &hint_str[send_hard_use_shmem_hint][0], task_id, &hint_str[send_soft_use_shmem_hint][0], &hint_str[recv_use_shmem_hint][0]);

		result = PAMI_Send (context[0], &parameters);
		if (result != PAMI_SUCCESS)
		  {
		    fprintf(stderr, "ERROR (E): [%s][%s] %zu %zu PAMI_Send from (%zu, %zu) (use_shmem hard hint = %s, use_shmem soft hint = %s) to (0, 0) (use_shmem hard hint = %s) FAILED with rc = %d\n", &xtalk_str[xtalk][0], &callback_str[remote_cb][0], header_bytes[h], data_bytes[p], task_id, xtalk, &hint_str[send_hard_use_shmem_hint][0], &hint_str[send_soft_use_shmem_hint][0], &hint_str[recv_use_shmem_hint][0], result);
		    return 1;
		  }

		if (debug) {
		  fprintf(stderr, "... after PAMI_Send.\n");
		}

		if (debug) {
		  fprintf(stderr, "before send advance loop ...\n");
		}
	
		while (send_active)
		  {
		    result = PAMI_Context_advance (context[xtalk], 100);
		    if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
		      {
			fprintf(stderr, "ERROR (E): Unable to advance pami context %zu. result = %d\n", xtalk, result);
			return 1;
		      }
		  }
	
		if (debug) {
		  fprintf(stderr, "... after send advance loop\n");
		}

		send_active = 1;
	      } /* end payload loop */
	    } /* end header loop */
	  } /* end send hint loop */
	} /* end recv hint loop */
      } /* end remote_cb loop */
    } /* end xtalk loop */
  } /* end task != 0  */

  /* ======== Combinations of mismatched use_shmem hints with unpredictable results ======== */
  if ( run_unpredictable == 1) {

    /* Enable unpredictable mismatched scenarios only ...skip all other scenarios */
    /* Disable all test scenarios */
    for (i = 0; i < 4; i++) {
      for(j = 0; j < 12; j++) {
	exp_ary[i][j] = 1;
      }
    }

    /* Execute send hard hint = 0, recv hard hint = 1 */
    exp_ary[1][0] = 0; exp_ary[1][1] = 0; exp_ary[1][2] = 0; exp_ary[1][3] = 0;
    /* Execute send hard hint = 2, recv hard hint = 1 */
    exp_ary[1][8] = 0; exp_ary[1][9] = 0; exp_ary[1][11] = 0;
    /* Execute send hard hint = 0, recv hard hint = 2 */
    exp_ary[2][0] = 0; exp_ary[2][1] = 0; exp_ary[2][2] = 0; exp_ary[2][3] = 0;
    /* Execute send hard hint = 1, recv hard hint = 2 */
    exp_ary[2][4] = 0; exp_ary[2][6] = 0; exp_ary[2][7] = 0;

    if (task_id == 0) {
      for(xtalk = 0; xtalk < num_contexts; xtalk++) {        /* xtalk loop */

	for (remote_cb = remote_cb_init; remote_cb < 2; remote_cb++) {    /* remote callback loop */
	  if (remote_cb) {
	    parameters.events.remote_fn     = send_done_remote;
	  } else {
	    parameters.events.remote_fn     = NULL;
	  }

	  for (r = 0; r < 3; r++) {
	    for (s = 0; s < 12; s++) {

	      /* Determine hint values */
	      sh = s;
	      send_hard_use_shmem_hint = (sh >> 2) & 3;
	      send_soft_use_shmem_hint = s & 3;
	      recv_use_shmem_hint = r;

	      /* Skip mismatched scenarios with unpredictable results, and hints not under test */
	      if (exp_ary[r][s] || ! ((hints_to_test >> send_hard_use_shmem_hint) & 1) || ! ((hints_to_test >> recv_use_shmem_hint) & 1) ) {

		if (debug) {
		  fprintf(stderr, "WARNING (W):  Skipping testcase (s = %zu, r = %zu, dispatch ID = %zu, send hard hint = %s, recv hint = %s, expectation = %s\n", s, r, dispatch_ary_0[r][s], &hint_str[send_hard_use_shmem_hint][0], &hint_str[recv_use_shmem_hint][0], &exp_str[exp_ary[r][s]][0]);
		}

		/* Reset __recv_buffer based on next send hint TC */
		/* 0's for even TCs and 255's for odd TCs */
		rv = (s+1) % 2;

		for (i = 0; i < 2048; i++) {
		  __recv_buffer[i] = reset_value[rv];
		}

		continue;
	      }

	      parameters.send.dispatch        = dispatch_ary_0[r][s];

	      for (h=h_init; h<hsize; h++) {                          /* header loop */
		parameters.send.header.iov_len = header_bytes[h];
		for (p=p_init; p<psize; p++) {                        /* payload loop */
		  parameters.send.data.iov_len = data_bytes[p];

		  /* Communicate with each task */
		  for (n = 1; n < num_tasks; n++) {

		    /* Skip sends to tasks that are unaddressable by rank 0 with current hint value */
		    /* We'll cover failing scenarios in the next section */
		    if ( addressable_by_me[send_hard_use_shmem_hint][n] != 1 ) {

		      /* If this is first send, inform user of skip */
		      if ( (xtalk == 0) && (remote_cb == remote_cb_init) && (h == h_init) && (p == p_init) ) {
			fprintf(stderr, "WARNING (W):  Rank %zu is unaddressable by Rank %zu when use_shmem = %s. Skipping ...\n", n, task_id, &hint_str[send_hard_use_shmem_hint][0]);
		      }

		      continue;
		    }

		    result = PAMI_Endpoint_create (client, n, 0, &parameters.send.dest);
		    if (result != PAMI_SUCCESS) {
		      fprintf (stderr, "ERROR (E):  PAMI_Endpoint_create FAILED for %zu with %d.\n", n, result);
		      return 1;
		    }

		    fprintf (stderr, "===== PAMI_Send() FUNCTIONAL Test [%s][%s] %zu %zu (%zu, 0) -> (%zu, %zu) =====\n\t\ttask %zu use_shmem hard hint = %s\n\t\ttask %zu use_shmem soft hint = %s\n\t\ttask %zu use shmem hard hint = %s\n", &xtalk_str[xtalk][0], &callback_str[remote_cb][0], header_bytes[h], data_bytes[p], task_id, n, xtalk, task_id, &hint_str[send_hard_use_shmem_hint][0], task_id, &hint_str[send_soft_use_shmem_hint][0], n, &hint_str[recv_use_shmem_hint][0]);

		    if (remote_cb) {
		      send_active++;
		    }

		    if (debug) {
		      fprintf (stderr, "before PAMI_Send ...\n");
		    }

		    result = PAMI_Send (context[xtalk], &parameters);

		    if (debug) {
		      fprintf (stderr, "... after PAMI_Send.\n");
		    }

		    if (result != PAMI_SUCCESS)
		      {
			fprintf(stderr, "ERROR (E): [%s][%s] %zu %zu PAMI_Send from (%zu, 0) (use_shmem hard hint = %s, use_shmem soft hint = %s) to (%zu, %zu) (use_shmem hard hint = %s) FAILED with rc = %d\n", &xtalk_str[xtalk][0], &callback_str[remote_cb][0], header_bytes[h], data_bytes[p], task_id, &hint_str[send_hard_use_shmem_hint][0], &hint_str[send_soft_use_shmem_hint][0], n, xtalk, &hint_str[recv_use_shmem_hint][0], result);

			return 1;
		      }

		    if (debug) {
		      fprintf(stderr, "before send-recv advance loop ...\n");
		    }

		    while (send_active || recv_active)
		      {
			result = PAMI_Context_advance (context[0], 100);
			if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
			  {
			    fprintf(stderr, "ERROR (E): Unable to advance pami context 0. result = %d\n", result);
			    return 1;
			  }
		      }
	  
		    if (debug) {
		      fprintf(stderr, "... after send-recv advance loop\n");
		    }

		    send_active = 1;
		    recv_active = 1;

		  } /* end task loop */

		  /* Reset __recv_buffer based on next send hint TC */
		  /* 0's for even TCs and 255's for odd TCs */
		  if ( (h == (hsize - 1)) && (p == (psize - 1)) ) { /* last header/payload combo for this TC */
		    rv = s % 2;
		  } else {
		    rv = (s+1) % 2;
		  }

		  for (i = 0; i < 2048; i++) {
		    __recv_buffer[i] = reset_value[rv];
		  }
		} /* end payload loop */
	      } /* end header loop */
	    } /* end send hint loop */
	  } /* end recv hint loop */
	} /* end remote callback loop */
      } /* end context loop */
    } /* end task = 0 */
    else { /* task id > 0 */

      result = PAMI_Endpoint_create (client, 0, 0, &parameters.send.dest);
      if (result != PAMI_SUCCESS) {
	fprintf (stderr, "ERROR:  PAMI_Endpoint_create failed with %d.\n", result);
	return 1;
      }

      for(xtalk = 0; xtalk < num_contexts; xtalk++) {        /* xtalk loop */

	for (remote_cb = remote_cb_init; remote_cb < 2; remote_cb++) {    /* remote callback loop */
	  if (remote_cb) {
	    parameters.events.remote_fn     = send_done_remote;
	  } else {
	    parameters.events.remote_fn     = NULL;
	  }

	  for (r = 0; r < 3; r++) {
	    for (s = 0; s < 12; s++) {

	      /* Determine hint values */
	      sh = s;
	      send_hard_use_shmem_hint = (sh >> 2) & 3;
	      send_soft_use_shmem_hint = s & 3;
	      recv_use_shmem_hint = r;

	      /* Skip mismatched scenarios with unpredictable results, hints not under test and unaddressable ranks */
	      if (exp_ary[r][s] || ! ((hints_to_test >> send_hard_use_shmem_hint) & 1) || ! ((hints_to_test >> recv_use_shmem_hint) & 1) || (addressable_by_me[send_hard_use_shmem_hint][0] != 1) ) {

		if (debug) {
		  fprintf(stderr, "WARNING (W):  Skipping testcase (s = %zu, r = %zu, dispatch ID = %zu, send hard hint = %s, recv hint = %s, expectation = %s\n", s, r, dispatch_ary_n[r][s], &hint_str[send_hard_use_shmem_hint][0], &hint_str[recv_use_shmem_hint][0], &exp_str[exp_ary[r][s]][0]);
		}

		/* Reset __recv_buffer based on next send hint TC */
		/* 0's for even TCs and 255's for odd TCs */
		rv = (s+1) % 2;

		for (i = 0; i < 2048; i++) {
		  __recv_buffer[i] = reset_value[rv];
		}

		continue;
	      }

	      for (h=h_init; h<hsize; h++) {                          /* header loop */
		parameters.send.header.iov_len = header_bytes[h];
		for (p=p_init; p<psize; p++) {                        /* payload loop */
		  parameters.send.data.iov_len = data_bytes[p];

		  if (debug) {
		    fprintf(stderr, "before recv advance loop ...\n");
		  }

		  while (recv_active != 0)
		    {
		      result = PAMI_Context_advance (context[xtalk], 100);
		      if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
			{
			  fprintf(stderr, "ERROR (E): Unable to advance pami context %zu. result = %d\n", xtalk, result);
			  return 1;
			}
		    }
	
		  if (debug) {
		    fprintf(stderr, "... after recv advance loop\n");
		  }

		  /* Reset __recv_buffer based on next send hint TC */
		  if (recv_active == 0) {

		    /* 0's for even TCs and 255's for odd TCs */
		    if ( (h == (hsize - 1)) && (p == (psize - 1)) ) { /* last header/payload combo for this TC */
		      rv = s % 2;
		    } else {
		      rv = (s+1) % 2;
		    }

		    for (i = 0; i < 2048; i++) {
		      __recv_buffer[i] = reset_value[rv];
		    }

		    recv_active = 1;
		  }

		  if (remote_cb) {
		    send_active++;
		  }

		  if (debug) {
		    fprintf(stderr, "before PAMI_Send ...\n");
		  }

		  parameters.send.dispatch        = dispatch_ary_n[r][s];

		  fprintf (stderr, "===== PAMI_Send() FUNCTIONAL Test [%s][%s] %zu %zu (%zu, %zu) -> (0, 0) =====\n\t\ttask %zu use_shmem hard hint = %s\n\t\ttask %zu use_shmem soft hint = %s\n\t\ttask 0 use shmem hard hint = %s\n", &xtalk_str[xtalk][0], &callback_str[remote_cb][0], header_bytes[h], data_bytes[p], task_id, xtalk, task_id, &hint_str[send_hard_use_shmem_hint][0], task_id, &hint_str[send_soft_use_shmem_hint][0], &hint_str[recv_use_shmem_hint][0]);

		  result = PAMI_Send (context[0], &parameters);
		  if (result != PAMI_SUCCESS)
		    {
		      fprintf(stderr, "ERROR (E): [%s][%s] %zu %zu PAMI_Send from (%zu, %zu) (use_shmem hard hint = %s, use_shmem soft hint = %s) to (0, 0) (use_shmem hard hint = %s) FAILED with rc = %d\n", &xtalk_str[xtalk][0], &callback_str[remote_cb][0], header_bytes[h], data_bytes[p], task_id, xtalk, &hint_str[send_hard_use_shmem_hint][0], &hint_str[send_soft_use_shmem_hint][0], &hint_str[recv_use_shmem_hint][0], result);
		      return 1;
		    }

		  if (debug) {
		    fprintf(stderr, "... after PAMI_Send.\n");
		  }

		  if (debug) {
		    fprintf(stderr, "before send advance loop ...\n");
		  }
	
		  while (send_active)
		    {
		      result = PAMI_Context_advance (context[xtalk], 100);
		      if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
			{
			  fprintf(stderr, "ERROR (E): Unable to advance pami context %zu. result = %d\n", xtalk, result);
			  return 1;
			}
		    }
	
		  if (debug) {
		    fprintf(stderr, "... after send advance loop\n");
		  }

		  send_active = 1;
		} /* end payload loop */
	      } /* end header loop */
	    } /* end send hint loop */
	  } /* end recv hint loop */
	} /* end remote_cb loop */
      } /* end xtalk loop */
    } /* end task != 0  */
  } /* end mismatched hint test */

  /* ======== Cleanup ======== */

  result = PAMI_Context_destroyv(context, num_contexts);
  if (result != PAMI_SUCCESS)
  {
    fprintf(stderr, "ERROR (E): Unable to destroy pami context(s). result = %d\n", result);
    return 1;
  }

  result = PAMI_Client_destroy(&client);
  if (result != PAMI_SUCCESS)
  {
    fprintf(stderr, "ERROR (E): Unable to finalize pami client. result = %d\n", result);
    return 1;
  }

  if ( __header_errors || __data_errors ) {
    fprintf(stderr, "ERROR (E): shmem_matrix FAILED with %zu header errors and %zu data errors. \n", __header_errors, __data_errors );
    return 1;
  } else {
    return 0;
  }
}
