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
 * \file test/api/p2p/send/type/send-typed.c
 * \brief Simple point-to-point PAMI_Send_typed() test
*/

#include <pami.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

static const char *optString = "DdMSh?";

char device_str[3][50] = {"DEFAULT", "SHMem", "MU"};
char hint_str[3][50] = {"PAMI_HINT_DEFAULT", "PAMI_HINT_ENABLE", "PAMI_HINT_DISABLE"};
char xtalk_str[2][50] = {"no crosstalk", "crosstalk"};
char callback_str[2][50] = {"no callback", "callback"};
char longheader_str[2][50] = {"no long header", "long header"};
char debug_str[500];

size_t debug = 0;
size_t task_id;
double __recv_buffer[4096];
size_t __recv_size;
size_t __header_errors = 0;
size_t __data_errors = 0;

pami_type_t __recv_type;
pami_type_t __send_type;

#define PI 3.14159265
#define E  2.71828183




static void decrement (pami_context_t   context,
                       void           * cookie,
                       pami_result_t    result)
{
  int * value = (int *) cookie;
  (*value)--;
};

static void decrement_dispatch (
  pami_context_t       context,
  void               * cookie,
  const void         * header_addr,
  size_t               header_size,
  const void         * pipe_addr,
  size_t               pipe_size,
  pami_endpoint_t      origin,
  pami_recv_t        * recv)
{
  if (recv != NULL) exit(1);

  decrement (context, cookie, PAMI_SUCCESS);
};

typedef struct
{
  pami_client_t client;
  pami_context_t * context;
  unsigned ncontext;
  size_t dispatch_id;
  volatile int counter;
  size_t ntasks;
} barrier_t;

void barrier_init (pami_client_t client, pami_context_t * context, unsigned ncontext,
                   size_t dispatch_id, size_t ntasks,
                   barrier_t * handle)
{
  handle->client = client;
  handle->context = context;
  handle->ncontext = ncontext;
  handle->dispatch_id = dispatch_id;
  handle->ntasks = ntasks;
  handle->counter = ncontext * ntasks;

  pami_dispatch_callback_function fn;
  fn.p2p = decrement_dispatch;

  unsigned i;

  for (i = 0; i < ncontext; i++)
    {
      PAMI_Dispatch_set (context[i], dispatch_id, fn, (void *) & handle->counter, (pami_dispatch_hint_t) {0});
    }
};

void barrier (barrier_t * handle)
{
  volatile int active = handle->ncontext * handle->ntasks;

  pami_send_t parameters;
  parameters.send.header.iov_base = &parameters;
  parameters.send.header.iov_len  = 1;
  parameters.send.data.iov_base   = NULL;
  parameters.send.data.iov_len    = 0;
  parameters.events.cookie        = (void *) & active;
  parameters.events.local_fn      = decrement;
  parameters.send.dispatch        = handle->dispatch_id;
  memset(&parameters.send.hints, 0, sizeof(parameters.send.hints));

  unsigned i, j;

  for (i = 0; i < (handle->ncontext); i++)
    {
      for (j = 0; j < handle->ntasks; j++)
        {
          PAMI_Endpoint_create (handle->client, j, i, &parameters.send.dest);
          PAMI_Send (handle->context[i], &parameters);
        }
    }

  while (active > 0 && handle->counter > 0)
    {
      for (i = 0; i < handle->ncontext; i++)
        {
          PAMI_Context_advance (handle->context[i], 100);
        }
    }
};


unsigned validate_header (const void * addr, size_t bytes)
{
  unsigned status = 0;
  size_t i, ndoubles = bytes / sizeof(double);
  double * array = (double *) addr;

  /* Skip validation if buffer size is 0 */
  if (bytes == 0)
    {
      return 2;
    }

  for (i = 0; i < ndoubles; i++)
    {
      if (array[i] != PI*(i + 1))
        {
          fprintf (stderr, "ERROR (E)::validate_header(%p,%zu): array[%zu] != %f (&array[%zu] = %p, value is %f)\n", addr, bytes, i, PI*(i + 1), i, &array[i], array[i]);
          status = 1;
        }

      /* reset the buffer */
      array[i] = E;
    }

  return status;
}

unsigned validate (const void * addr, size_t bytes)
{
  unsigned status = 0;
  size_t i, ndoubles = bytes / sizeof(double);
  double * array = (double *) addr;

  /* Skip validation if buffer size is 0 */
  if (bytes == 0)
    {
      return 2;
    }

  for (i = 0; i < ndoubles; i++)
    {
      if (array[i*2] != PI*(i + 1))
        {
          fprintf (stderr, "ERROR (E)::validate(%p,%zu): array[%zu] != %f (&array[%zu] = %p, value is %f)\n", addr, bytes, i*2, PI*(i + 1), i*2, &array[i*2], array[i*2]);
          status = 1;
        }

      if (array[i*2+1] != E)
        {
          fprintf (stderr, "ERROR (E)::validate(%p,%zu): array[%zu] != %f (&array[%zu] = %p, value is %f)\n", addr, bytes, i*2 + 1, E, i*2 + 1, &array[i*2+1], array[i*2+1]);
          status = 1;
        }

      /* reset the buffer */
      array[i*2] = E;
      array[i*2+1] = E;
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

  if (debug)
    {
      fprintf (stderr, "Called recv_done function.  active(%p): %zu -> %zu, __recv_size = %zu\n", active, *active, *active - 1, __recv_size);
    }

  /* Check payload data integrity */
  validateRC = validate(__recv_buffer, __recv_size);

  switch (validateRC)
    {

      case 0: /* Validation passed */

        if (debug)
          {
            fprintf (stderr, ">>> Payload validated.\n");
          }

        break;
      case 1: /* Validation FAILED */
        __data_errors++;
        fprintf (stderr, ">>> ERROR (E)::recv_done:  Validate payload FAILED!\n");
        break;
      case 2: /* Validation skipped */

        if (debug)
          {
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

  if (debug)
    {
      fprintf (stderr, "Called dispatch function.  cookie = %p, active: %zu, header_size = %zu, pipe_size = %zu, recv = %p\n", cookie, *active, header_size, pipe_size, recv);
    }

  /*(*active)--; */
  /*fprintf (stderr, "... dispatch function.  active = %zu\n", *active); */

  /* Check header data integrity */
  validateRC = validate_header(header_addr, header_size);

  switch (validateRC)
    {

      case 0: /* Validation passed */

        if (debug)
          {
            fprintf (stderr, ">>> Header validated.\n");
          }

        break;
      case 1: /* Validation FAILED */
        __data_errors++;
        fprintf (stderr, ">>> ERROR (E)::test_dispatch:  Validate header FAILED!\n");
        break;
      case 2: /* Validation skipped */

        if (debug)
          {
            fprintf (stderr, ">>> Skipping header validation (header size = %zu).\n", header_size);
          }

        break;
    }

  /* Check payload data integrity */
  if (recv == NULL)
    {
      /* This is an 'immediate' receive */
      /* header + payload < receive immediate max for this context and dispatch ID */

      __recv_size = pipe_size;

      PAMI_Type_transform_data ((void *) pipe_addr, PAMI_TYPE_BYTE, 0, __recv_buffer, __recv_type, 0, pipe_size, PAMI_DATA_COPY, NULL);

      recv_done (context, cookie, PAMI_SUCCESS);
    }
  else
    {
      /* This is an 'asynchronous' receive */

      __recv_size = pipe_size;

      recv->local_fn = recv_done;
      recv->cookie   = cookie;
      recv->type     = __recv_type;
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

  if (debug)
    {
      fprintf (stderr, "Called send_done_local function.  active(%p): %zu -> %zu\n", active, *active, *active - 1);
    }

  (*active)--;
}
/* ---------------------------------------------------------------*/

static void send_done_remote (pami_context_t   context,
                              void          * cookie,
                              pami_result_t    result)
{
  volatile size_t * active = (volatile size_t *) cookie;

  if (debug)
    {
      fprintf (stderr, "Called send_done_remote function.  active(%p): %zu -> %zu\n", active, *active, *active - 1);
    }

  (*active)--;
  /* fprintf (stderr, "... send_done_remote function.  active = %zu\n", *active); */
}
/* ---------------------------------------------------------------*/

void display_usage(int argc, char ** argv)
{

  printf("This is the help text for default-send.c:\n");
  printf("default-send.c by default will attempt to pass messages of varying header and payload sizes between rank 0 and all other ranks (requires >= 2 ranks) using all available use_shmem hint values.  Available use_shmem hint values vary based on PAMI_DEVICE value.\n");
  printf("PAMI_DEVICE\tAvailable use_shmem values\n");
  printf("===========\t==========================\n");
  printf("B or unset\tPAMI_HINT_DEFAULT, PAMI_HINT_ENABLE & PAMI_HINT_DISABLE\n");
  printf("M\t\tPAMI_HINT_DEFAULT & PAMI_HINT_DISABLE\n");
  printf("S\t\tPAMI_HINT_DEFAULT & PAMI_HINT_ENABLE\n");
  printf("\n");
  printf("The user can also select a subset of the available hints by using the parms below:\n");
  printf("-D | --D PAMI_HINT_DEFAULT\n");
  printf("-M | --M PAMI_HINT_DISABLE\n");
  printf("-S | --S PAMI_HINT_ENABLE\n");
  printf("\n");
  printf("-d | --debug Enable error tracing messages to stderr for debug.\n");
  printf("\n");
  printf("Parms can be provided separately or together.\n");
  printf("\tex:  %s -D -S --debug\n", argv[0]);
  printf("\tex:  %s --MS\n", argv[0]);
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

  volatile size_t send_active = 1;
  volatile size_t recv_active = 1;

  pami_client_t client;
  pami_configuration_t configuration;
  pami_context_t context[2];

  char                  cl_string[] = "TEST";
  pami_result_t result = PAMI_ERROR;

  /* See if user passed in any args */
  opt = getopt( argc, argv, optString );

  while ( opt != -1 )
    {
      switch ( opt )
        {
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

          case 'h':   /* fall-through is intentional */
          case '?':

            if (task_id == 0)
              {
                display_usage(argc, argv);
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

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "ERROR (E):  Unable to initialize PAMI client. result = %d\n", result);
      return 1;
    }

  configuration.name = PAMI_CLIENT_NUM_CONTEXTS;
  result = PAMI_Client_query(client, &configuration, 1);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "ERROR (E):  Unable to query configuration (%d). result = %d\n", configuration.name, result);
      return 1;
    }

  size_t max_contexts = configuration.value.intval;

  if (max_contexts > 0)
    {
      fprintf (stdout, "Max number of contexts = %zu\n", max_contexts);
    }
  else
    {
      fprintf (stderr, "ERROR (E):  Max number of contexts (%zu) <= 0. Exiting\n", max_contexts);
      PAMI_Client_destroy(client);
      return 1;
    }

  size_t num_contexts = 1;

  if (max_contexts > 1)
    {
      num_contexts = 2; /* allows for cross talk */
    }

  result = PAMI_Context_createv(client, NULL, 0, context, num_contexts);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "ERROR (E):  Unable to create PAMI context. result = %d\n", result);
      return 1;
    }

  configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query(client, &configuration, 1);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "ERROR (E):  Unable to query configuration (%d). result = %d\n", configuration.name, result);
      return 1;
    }

  task_id = configuration.value.intval;
  fprintf (stderr, "My task id = %zu\n", task_id);

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration, 1);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "ERROR (E):  Unable to query configuration (%d). result = %d\n", configuration.name, result);
      return 1;
    }

  size_t num_tasks = configuration.value.intval;
  fprintf (stderr, "Number of tasks = %zu\n", num_tasks);

  if (num_tasks < 2)
    {
      fprintf(stderr, "ERROR (E):  This test requires >= 2 tasks. Number of tasks in this job: %zu\n", num_tasks);
      return 1;
    }


  /* ************************************************************************
   * Initialize a simple point-to-point barrier
   * ***********************************************************************/
  barrier_t handle;
  barrier_init (client, context, num_contexts, 42, num_tasks, & handle);


  size_t num_local_tasks = 0;
  configuration.name = PAMI_CLIENT_NUM_LOCAL_TASKS;
  result = PAMI_Client_query (client, &configuration, 1);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "ERROR (E):  Unable to query configuration attribute PAMI_CLIENT_NUM_LOCAL_TASKS; result = %d\n", result);
      return 1;
    }
  else
    {
      num_local_tasks = configuration.value.intval;
      fprintf (stdout, "PAMI_CLIENT_NUM_LOCAL_TASKS = %zu\n", configuration.value.intval);
    }

  /* Create an array for storing device addressabilty based on hint value */
  /* row 0 (DEFAULT), row 1 (SHMem) and row 2 (MU) */
  size_t addressable_by_me[3][num_tasks];

  /* Init device addressability array */
  for ( hint = 0; hint < 3; hint++ )
    {
      for ( n = 0; n < num_tasks; n++ )
        {
          if ( hint == 1)   /* SHMem */
            {
              addressable_by_me[hint][n] = 0;
            }
          else   /* hint = 0 (DEFAULT) or 2 (MU) */
            {
              addressable_by_me[hint][n] = 1;
            }
        }
    }

  configuration.name = PAMI_CLIENT_LOCAL_TASKS;
  result = PAMI_Client_query (client, &configuration, 1);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "ERROR (E):  Unable to query configuration attribute PAMI_CLIENT_LOCAL_TASKS; result = %d\n", result);
      return 1;
    }
  else
    {
      /* Set local tasks in device addressability array */
      if (debug)
        {
          sprintf (debug_str, "Tasks local to task %zu:", task_id);
        }

      for (i = 0; i < num_local_tasks; i++)
        {
          if (debug)
            {
              if (i == 0)   /* First local task */
                {
                  sprintf (debug_str, "%s  %zu", debug_str, configuration.value.intarray[i]);
                }
              else if (i == (num_local_tasks - 1))
                {
                  sprintf (debug_str, "%s & %zu", debug_str, configuration.value.intarray[i]);
                }
              else
                {
                  sprintf (debug_str, "%s, %zu", debug_str, configuration.value.intarray[i]);
                }
            }

          addressable_by_me[1][configuration.value.intarray[i]] = 1;
        }

      if (debug)
        {
          fprintf (stderr, "%s\n", debug_str);
        }
    }

  if (debug)
    {
      sprintf (debug_str, "task %zu SHMem addressable tasks:  ", task_id);

      for ( n = 0; n < num_tasks; n++ )
        {
          if (addressable_by_me[1][n] == 1)
            {
              sprintf (debug_str, "%s %zu", debug_str, n);
            }
        }

      fprintf (stderr, "%s\n", debug_str);
    }

  /* Determine which Device(s) are initialized based on PAMI_DEVICE env var */

  char * device;

  if (debug)
    {
      fprintf (stderr, "Before device check ...\n");
    }

  device = getenv("PAMI_DEVICE");

  if ( device != NULL )   /* Passing NULL to strcmp will result in a segfault */
    {
      if ( strcmp(device, "M") == 0 )
        {
          if (task_id == 0)
            {
              fprintf (stdout, "Only the MU device is initialized.\n");
            }

          available_hints = 5;
        }
      else if ( strcmp(device, "S") == 0 )
        {
          if (task_id == 0)
            {
              fprintf (stdout, "Only the SHMem device is initialized.\n");
            }

          available_hints = 3;
        }
      else if ( strcmp(device, "B") == 0 )
        {
          if (task_id == 0)
            {
              fprintf (stdout, "Both the MU and SHMem devices are initialized.\n");
            }
        }
      else
        {
          if (task_id == 0)
            {
              fprintf (stderr, "ERROR (E):  PAMI_DEVICE = %s is unsupported. Valid values are:  M (MU only), S (SHMem only) & [ B | unset ] (both MU & SHMem)\n", device);
            }

          return 1;
        }
    }
  else
    {
      if (task_id == 0)
        {
          fprintf (stderr, "Both the MU and SHMem devices are initialized.\n");
        }
    }

  if (debug)
    {
      fprintf (stderr, "After device check ...\n");
    }

  /* Default to test all available hints */
  if ( hints_to_test == 0 )
    {
      hints_to_test = available_hints;
    }
  else   /* only test hints selected by user that are available */
    {
      hints_to_test = available_hints & hints_to_test;
    }

  /* Print table of available hints and if they'll be tested */
  if (task_id == 0)
    {
      fprintf (stdout, "use_shmem Value(s)\tAVAILABLE\tUNDER TEST\n");

      for  (hint = 0; hint < 3; hint++)
        {
          fprintf(stdout, "%s\t", &hint_str[hint][0]);

          if ( (available_hints >> hint) & 1 )
            {
              fprintf(stdout, "    Y\t\t");
            }
          else
            {
              fprintf(stdout, "    N\t\t");
            }

          if ( (hints_to_test >> hint) & 1 )
            {
              fprintf(stdout, "     Y\n");
            }
          else
            {
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
  dispatch[0].name = "  default ";
  dispatch[0].options.use_shmem = PAMI_HINT_DEFAULT;

  dispatch[1].id = 1;
  dispatch[1].options = (pami_dispatch_hint_t) {0};
  dispatch[1].name = "only shmem";
  dispatch[1].options.use_shmem = PAMI_HINT_ENABLE;

  dispatch[2].id = 2;
  dispatch[2].options = (pami_dispatch_hint_t) {0};
  dispatch[2].name = " no shmem ";
  dispatch[2].options.use_shmem = PAMI_HINT_DISABLE;

  pami_dispatch_callback_function fn;
  fn.p2p = test_dispatch;

  /* ***********************************
   * Initialize the recv buffer
   ************************************/
  for (i = 0; i < 4096; i++)
    __recv_buffer[i] = E;

  /* ***********************************
   * Create the pami types
   ************************************/
  result = PAMI_Type_create (&__recv_type);
  result = PAMI_Type_add_simple (__recv_type,
                                 sizeof(double),    /* bytes */
                                 0,                 /* offset */
                                 2048,              /* count */
                                 sizeof(double) * 2); /* stride */
  result = PAMI_Type_complete (__recv_type, sizeof(double));


  /* This noncontiguous 'send type' is composed of two contiguous doubles,
   * then skips two doubles, then repeats.
   */ 
  result = PAMI_Type_create (&__send_type);
  result = PAMI_Type_add_simple (__send_type,
                                 sizeof(double) * 2,  /* bytes */
                                 0,                   /* offset */
                                 1024,                /* count */
                                 sizeof(double) * 4); /* stride */
  result = PAMI_Type_complete (__send_type, sizeof(double));




  double header[2048*2];
  double data[2048*2];

  /* ***********************************
   * Initialize the send buffer
   ************************************/
  for (i = 0; i< 2048*2; i++)
    {
      header[i] = E;
      data[i]   = E;
    }
  unsigned offset = 0;
  for (i = 0; i < 2048; i++)
    {
      header[i] = PI * (i + 1); /* header is CONTIGUOUS */
      data[i+offset]   = PI * (i + 1);
      offset += ((i & 0x01) << 1);
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

  /* PAMI_Dispatch_set */

  if (debug)
    {
      fprintf (stderr, "Before PAMI_Dispatch_set() .. &recv_active = %p, recv_active = %zu\n", &recv_active, recv_active);
    }

  for (i = 0; i < num_contexts; i++)
    {
      /* For each context: */
      /* Set up dispatch ID 0 for DEFAULT (use_shmem = 0 | PAMI_HINT_DEFAULT) */
      /* Set up dispatch ID 1 for SHMEM   (use_shmem = 1 | PAMI_HINT_ENABLE)  */
      /* Set up dispatch ID 2 for MU      (use_shmem = 2 | PAMI_HINT_DISABLE) */

      for (hint = 0; hint < hint_limit; hint++)
        {

          if (debug)
            {
              fprintf (stderr, "rank:  %zu\tnum local tasks:  %zu\n", task_id, num_local_tasks);
            }

          /* Can't create shmem dispatch ID if there aren't any other local tasks */
          if ( (hint == 1) && (num_local_tasks == 1) )
            {
              fprintf(stderr, "WARNING (W):  No other local tasks exist with Rank %zu.  Skipping creation of \"SHMem\" dispatch ID ...\n", task_id);
              continue;
            }

          dispatch[hint].result = PAMI_Dispatch_set (context[i],
                                                     dispatch[hint].id,
                                                     fn,
                                                     (void *) & recv_active,
                                                     dispatch[hint].options);

          if (dispatch[hint].result != PAMI_SUCCESS)
            {
              fprintf (stderr, "ERROR (E):  Unable to register pami dispatch ID %zu on context %zu. result = %d\n", dispatch[hint].id, i, dispatch[hint].result);
              return 1;
            }
        } /* end hint loop */
    } /* end context loop */

  size_t xtalk = 0;
  size_t remote_cb = 0;
  /*
  pami_endpoint_t origin, target;
  PAMI_Endpoint_create (client, origin_task, 0, &origin);
  PAMI_Endpoint_create (client, target_task, 0, &target);
  */



  /* Time to run various headers and payloads */
  pami_send_typed_t parameters;
  parameters.send.header.iov_base = header;
  parameters.send.data.iov_base   = data;
  parameters.events.cookie        = (void *) & send_active;
  parameters.events.local_fn      = send_done_local;
  parameters.typed.type           = __send_type;
  parameters.typed.offset         = 0;
  parameters.typed.data_fn        = PAMI_DATA_COPY;
  parameters.typed.data_cookie    = NULL;
  memset(&parameters.send.hints, 0, sizeof(parameters.send.hints));

  if (task_id == 0)
    {
      for (hint = 0; hint < hint_limit; hint++)                /* hint/device loop */
        {

          /* Skip hints that are not under test */
          if ( ! ((hints_to_test >> hint) & 1) )
            {
              continue;
            }

          parameters.send.dispatch = hint;

          for (xtalk = 0; xtalk < num_contexts; xtalk++)         /* xtalk loop */
            {

              for (remote_cb = 0; remote_cb < 2; remote_cb++)      /* remote callback loop */
                {
                  if (remote_cb)
                    {
                      parameters.events.remote_fn     = send_done_remote;
                    }
                  else
                    {
                      parameters.events.remote_fn     = NULL;
                    }

                  for (h = 0; h < hsize; h++)                        /* header loop */
                    {
                      parameters.send.header.iov_len = header_bytes[h];

                      for (p = 0; p < psize; p++)                      /* payload loop */
                        {
                          parameters.send.data.iov_len = data_bytes[p];

                          /* Communicate with each task */
                          for (n = 1; n < num_tasks; n++)
                            {

                              /* Skip sends to tasks that are unaddressable by rank 0 with current hint value */
                              if ( addressable_by_me[hint][n] != 1 )
                                {

                                  /* If this is first send, inform user of skip */
                                  if ( ! ( xtalk || remote_cb || h || p ) )
                                    {
                                      fprintf(stderr, "WARNING (W):  Rank %zu is unaddressable by Rank %zu when use_shmem = %s. Skipping until next use_shmem hint ...\n", n, task_id, &hint_str[hint][0]);
                                    }

                                  continue;

                                }

                              result = PAMI_Endpoint_create (client, n, xtalk, &parameters.send.dest);

                              if (result != PAMI_SUCCESS)
                                {
                                  fprintf (stderr, "ERROR (E):  PAMI_Endpoint_create FAILED for task_id %zu, context %zu with %d.\n", n, xtalk, result);
                                  return 1;
                                }

                              fprintf (stderr, "===== PAMI_Send_typed() FUNCTIONAL Test [%s][%s][%s] %zu %zu (%zu, 0) -> (%zu, %zu) =====\n\n", &device_str[hint][0], &xtalk_str[xtalk][0], &callback_str[remote_cb][0], header_bytes[h], data_bytes[p], task_id, n, xtalk);

                              if (remote_cb)
                                {
                                  send_active++;
                                }

                              if (debug)
                                {
                                  fprintf (stderr, "before PAMI_Send_typed() ...\n");
                                }

                              result = PAMI_Send_typed (context[0], &parameters);

                              if (debug)
                                {
                                  fprintf (stderr, "... after PAMI_Send_typed().\n");
                                }

                              if (result != PAMI_SUCCESS)
                                {
                                  fprintf (stderr, "ERROR (E):  PAMI_Send_typed failed with rc = %d\n", result);
                                  return 1;
                                }


                              if (debug)
                                {
                                  fprintf (stderr, "before send-recv advance loop ... &send_active = %p, &recv_active = %p\n", &send_active, &recv_active);
                                }

                              while (send_active || recv_active)
                                {
                                  result = PAMI_Context_advance (context[0], 100);

                                  if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
                                    {
                                      fprintf (stderr, "ERROR (E):  Unable to advance pami context 0. result = %d\n", result);
                                      return 1;
                                    }
                                }

                              send_active = 1;
                              recv_active = 1;

                              if (debug)
                                {
                                  fprintf (stderr, "... after send-recv advance loop\n");
                                }
                            } /* end task id loop */
                        } /* end payload loop */
                    } /* end header loop */
                } /* end remote callback loop */
            } /* end xtalk loop */
        } /* end device loop */
    } /* end task = 0 */
  else
    {
      for (hint = 0; hint < hint_limit; hint++)       /* hint/device loop */
        {

          /* Skip hints that are not under test */
          if ( ! ((hints_to_test >> hint) & 1) )
            {
              continue;
            }

          /* Skip sends to task 0 that are unaddressable by current rank with current hint value */
          if ( addressable_by_me[hint][0] != 1 )
            {
              if (debug)
                {
                  fprintf(stderr, "WARNING (W):  Rank 0 is unaddressable by Rank %zu when use_shmem = %s. Skipping to next use_shmem hint ...\n", task_id, &hint_str[hint][0]);
                }

              continue;
            }

          parameters.send.dispatch = hint;

          for (xtalk = 0; xtalk < num_contexts; xtalk++)         /* xtalk loop */
            {

              result = PAMI_Endpoint_create (client, 0, 0, &parameters.send.dest);

              if (result != PAMI_SUCCESS)
                {
                  fprintf (stderr, "ERROR (E):  PAMI_Endpoint_create failed for task_id 0, context 0 with %d.\n", result);
                  return 1;
                }

              for (remote_cb = 0; remote_cb < 2; remote_cb++)      /* remote callback loop */
                {

                  if (remote_cb)
                    {
                      parameters.events.remote_fn     = send_done_remote;
                    }
                  else
                    {
                      parameters.events.remote_fn     = NULL;
                    }

                  for (h = 0; h < hsize; h++)                        /* header loop */
                    {
                      parameters.send.header.iov_len = header_bytes[h];

                      for (p = 0; p < psize; p++)                      /* payload loop */
                        {
                          parameters.send.data.iov_len = data_bytes[p];

                          if (debug)
                            {
                              fprintf (stderr, "before recv advance loop ... &recv_active = %p\n", &recv_active);
                            }

                          while (recv_active != 0)
                            {
                              result = PAMI_Context_advance (context[xtalk], 100);

                              if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
                                {
                                  fprintf (stderr, "ERROR (E):  Unable to advance pami context %zu. result = %d\n", xtalk, result);
                                  return 1;
                                }

                              /*fprintf (stderr, "------ recv advance loop ... &recv_active = %p\n", &recv_active);*/
                            }

                          recv_active = 1;

                          if (debug)
                            {
                              fprintf (stderr, "... after recv advance loop\n");
                            }

                          fprintf (stderr, "===== PAMI_Send_typed() FUNCTIONAL Test [%s][%s][%s] %zu %zu (%zu, %zu) -> (0, 0) =====\n\n", &device_str[hint][0], &xtalk_str[xtalk][0], &callback_str[remote_cb][0], header_bytes[h], data_bytes[p], task_id, xtalk);

                          if (remote_cb)
                            {
                              send_active++;
                            }

                          if (debug)
                            {
                              fprintf (stderr, "before send ...\n");
                            }

                          result = PAMI_Send_typed (context[xtalk], &parameters);

                          if (debug)
                            {
                              fprintf (stderr, "... after send.\n");
                            }

                          if (result != PAMI_SUCCESS)
                            {
                              fprintf (stderr, "ERROR (E):  PAMI_Send_typed() failed with rc = %d\n", result);
                              return 1;

                            }

                          if (debug)
                            {
                              fprintf (stderr, "before send advance loop ... &send_active = %p\n", &send_active);
                            }

                          while (send_active)
                            {
                              result = PAMI_Context_advance (context[xtalk], 100);

                              if ( (result != PAMI_SUCCESS) && (result != PAMI_EAGAIN) )
                                {
                                  fprintf (stderr, "ERROR (E):  Unable to advance pami context %zu. result = %d\n", xtalk, result);
                                  return 1;
                                }

                              /* fprintf (stderr, "------ send advance loop ... &send_active = %p\n", &send_active); */
                            }

                          send_active = 1;

                          if (debug)
                            {
                              fprintf (stderr, "... after send advance loop\n");
                            }
                        } /* end payload loop */
                    } /* end header loop */
                } /* end remote callback loop */
            } /* end xtalk loop */
        } /* end device loop */
    } /* end task id != 0 */

  /* ====== CLEANUP ====== */

  barrier (&handle);

  result = PAMI_Context_destroyv(context, num_contexts);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "ERROR (E):  Unable to destroy context(s), result = %d\n", result);
      return 1;
    }

  result = PAMI_Client_destroy(&client);

  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "ERROR (E):  Unable to destroy pami client. result = %d\n", result);
      return 1;
    }

  if ( (__header_errors > 0) || (__data_errors > 0) )
    {
      fprintf (stdout, "ERROR (E):  default-send FAILED with %zu header errors and %zu data errors on task %zu!!\n", __header_errors, __data_errors, task_id);
      return 1;
    }
  else
    {
      fprintf (stdout, "Success (%zu)\n", task_id);
      return 0;
    }
}
