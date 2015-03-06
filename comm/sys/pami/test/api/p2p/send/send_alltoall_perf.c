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
 * \file test/api/p2p/send/send_alltoall_perf.c
 * \brief ???
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <pami.h>

/*#define DO_FENCE*/

volatile unsigned   _recv_active[2];
char               *_recvbuffer;

static void shuffle(pami_endpoint_t *array, size_t n)
{
  if (n > 1)
  {
    size_t i;
    for (i = 0; i < n - 1; i++)
    {
      size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
      pami_endpoint_t t = array[j];
      array[j]          = array[i];
      array[i]            = t;
    }
  }
}

/* --------------------------------------------------------------- */
static void decrement (pami_context_t  context,
                       void           *cookie,
                       pami_result_t   result)
{
  unsigned * value = (unsigned *) cookie;
  --*value;
}

/* --------------------------------------------------------------- */
static void alltoall_dispatch (pami_context_t       context,      /**< IN: PAMI context */
                               void               * cookie,       /**< IN: dispatch cookie */
                               const void         * header_addr,  /**< IN: header address */
                               size_t               header_size,  /**< IN: header size */
                               const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
                               size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
                               pami_endpoint_t      origin,
                               pami_recv_t        * recv)         /**< OUT: receive message structure */
{
  unsigned   *value    = (unsigned *) cookie;
  size_t      contextid;
  pami_task_t sender;
  PAMI_Endpoint_query(origin, &sender, &contextid);
  int* chunksize = (int*)header_addr;

  if (!recv)
  {
    memcpy(&_recvbuffer[sender*(*chunksize)], pipe_addr, pipe_size);
    --*value;
    return;
  }
  recv->cookie      = cookie;
  recv->local_fn    = decrement;
  recv->addr        = (void*)&_recvbuffer[sender*(*chunksize)];
  recv->type        = PAMI_TYPE_BYTE;
  recv->offset      = 0;
  recv->data_fn     = PAMI_DATA_COPY;
  recv->data_cookie = cookie;
}


void alltoall(pami_client_t    client,
              pami_context_t   context,
              size_t           dispatch,
              char            *sendheader,
              size_t           headersize,
              char            *sendbuffer,
              size_t           chunksize,
              pami_endpoint_t *endpoints,
              size_t           num_endpoints)
{
  unsigned i;
  volatile unsigned send_active = num_endpoints;
  _recv_active[dispatch] += num_endpoints;

  pami_send_t parameters = {};
  memset(&parameters, 0, sizeof(parameters));
  parameters.send.dispatch        = dispatch;
  parameters.send.header.iov_len  = headersize;
  parameters.send.data.iov_len    = chunksize;
  parameters.events.cookie        = (void *) &send_active;
  parameters.events.local_fn      = decrement;
  parameters.events.remote_fn     = NULL;

  for (i=0; i<num_endpoints; i++)
  {
    pami_task_t t;
    size_t      contextid;
    PAMI_Endpoint_query(endpoints[i], &t, &contextid);

    int  header_offset = t*headersize;
    int *header_data   = (int*) &sendheader[header_offset];
    (*header_data)     = chunksize;

    parameters.send.dest            =  endpoints[i];
    parameters.send.header.iov_base = &sendheader[header_offset];
    parameters.send.data.iov_base   = &sendbuffer[t*chunksize];
    PAMI_Send (context, &parameters);
  }
  while (_recv_active[dispatch] > 0 || send_active > 0) PAMI_Context_advance (context, 100);
}

int main (int argc, char ** argv)
{
  pami_client_t  client;
  pami_context_t context;
  char clientname[]="TEST";
  PAMI_Client_create (clientname, &client, NULL, 0);
  PAMI_Context_createv (client, NULL, 0, &context, 1);

  pami_dispatch_callback_function fn;
  fn.p2p = alltoall_dispatch;
  pami_dispatch_hint_t options={};
  memset(&options, 0, sizeof(options));
  size_t dispatch = 0;
  pami_result_t        result = PAMI_Dispatch_set (context,
                                                   dispatch,
                                                   fn,
                                                   (void *)&_recv_active[0],
                                                   options);
  dispatch        = 1;
  pami_result_t        result1 = PAMI_Dispatch_set (context,
                                                    dispatch,
                                                    fn,
                                                    (void *)&_recv_active[1],
                                                    options);
  if (result != PAMI_SUCCESS || result1 != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable register pami dispatch. result = %d\n", result);
    return 1;
  }

  pami_configuration_t configuration;
  configuration.name   = PAMI_CLIENT_TASK_ID;
  result               = PAMI_Client_query(client, &configuration,1);
  size_t     my_taskid = configuration.value.intval;
  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration,1);
  size_t num_tasks = configuration.value.intval;

  int i,chunksize = 128;
  int niter = 1;
  for ( i=1 ; i < argc ; ++i )
  {
    if ( argv[i][0] != '-' )
      continue;
    switch(argv[i][1])
    {
        case 'm':
          chunksize = atoi(argv[++i]);
          break;
        case 'i':
          niter = atoi(argv[++i]);
          break;
        default:
          fprintf(stderr, "Unrecognized argument %s\n", argv[i]);fflush(stderr);
          PAMI_Context_destroyv(&context, 1);
          PAMI_Client_destroy(&client);
          return -1;
    }
  }

  if(my_taskid == 0)
    fprintf(stderr, "Alltoall:  msgsize=%d, memory=%ld\n",
            chunksize,2*num_tasks*chunksize);

  _recvbuffer                    = (char*)malloc(num_tasks*chunksize);
  char            *sendheader    = (char*)malloc(num_tasks*sizeof(int));
  char            *sendbuffer    = (char*)malloc(num_tasks*chunksize);
  pami_endpoint_t *endpoints     = (pami_endpoint_t*)malloc(num_tasks*sizeof(pami_endpoint_t));
  assert(_recvbuffer && sendheader && sendbuffer && endpoints);
  int              headersize    = sizeof(int);
  size_t           num_endpoints = num_tasks;
  for (i=0; i<num_tasks; i++)
    PAMI_Endpoint_create (client, i, 0, &endpoints[i]);

  /*  Randomize the ranks */
  srand(my_taskid);
  shuffle(endpoints, num_endpoints);

  /* First Alltoall */
  dispatch = 0;
  alltoall (client, context, dispatch,
            sendheader, headersize,
            sendbuffer, chunksize,
            endpoints, num_endpoints);
  
  /* Second Alltoall */
  double t  = PAMI_Wtime(client);
  for(i=1; i <= niter; i++)
    alltoall (client, context, i%2,
              sendheader, headersize,
              sendbuffer, chunksize,
              endpoints, num_endpoints);
  t = PAMI_Wtime(client) - t;

  if(my_taskid == 0)
    fprintf(stderr, "Time=%f s/alltoall iter=%d Tasks=%ld\n", t/niter, niter,num_tasks);

#ifdef DO_FENCE
  volatile unsigned fence_flag = 1;
  PAMI_Fence_all(context,decrement,(void*)&fence_flag);
  while (fence_flag > 0) PAMI_Context_advance (context, 100);
#endif

  free(sendheader);
  free(sendbuffer);
  free(endpoints);

  PAMI_Context_destroyv(&context, 1);
  PAMI_Client_destroy(&client);

  return 0;
}
