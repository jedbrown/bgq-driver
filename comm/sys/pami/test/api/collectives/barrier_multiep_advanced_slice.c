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
 * \file test/api/collectives/barrier_multiep_advanced_slice.c
 * \brief Simple Barrier test on endpoint geometries
 */

/* Use arg -h or see setup_env() for environment variable overrides  */
#define MAX_THREADS 128
#include "../pami_util.h"
#include <pthread.h>

static void *barrier_test(void*);

pami_geometry_t      world_geometry;
pami_task_t          task_id;
size_t               num_tasks;
pami_context_t      *context;
pami_client_t        client;
pami_task_t          non_root[2];

typedef struct thread_data_t
{
  pami_context_t context;
  int            tid;
  int            logical_rank;
} thread_data_t;


int main(int argc, char*argv[])
{
  /* Process environment variables and setup globals */
  if(argc > 1 && argv[1][0] == '-' && (argv[1][1] == 'h' || argv[1][1] == 'H') ) setup_env_internal(1);
  else setup_env();
  pthread_t threads[MAX_THREADS];
  size_t               num_algorithm[2];
  pami_algorithm_t    *always_works_algo = NULL;
  pami_metadata_t     *always_works_md = NULL;
  pami_algorithm_t    *must_query_algo = NULL;
  pami_metadata_t     *must_query_md = NULL;
  pami_xfer_type_t     barrier_xfer = PAMI_XFER_BARRIER;
  pami_xfer_t          barrier;
  volatile unsigned    poll_flag = 0;
  int                  num_threads    = gNum_contexts;

  assert(gNum_contexts > 0);
  context = (pami_context_t*)malloc(sizeof(pami_context_t) * gNum_contexts);

  /*  Initialize PAMI; This will also create gNum_contexts contexts*/
  thread_data_t *td = (thread_data_t*)malloc(sizeof(thread_data_t) * gNum_contexts);
  assert(context);
  assert(td);
  int rc = pami_init(&client,        /* Client             */
                     context,        /* Context            */
                     NULL,           /* Clientname=default */
                     &gNum_contexts, /* gNum_contexts       */
                     NULL,           /* null configuration */
                     0,              /* no configuration   */
                     &task_id,       /* task id            */
                     &num_tasks);    /* number of tasks    */
  if(rc != PAMI_SUCCESS)
    return 1;

  assert(task_id >= 0);
  assert(task_id < num_tasks);

  /*  Query the world geometry for barrier algorithms */
  int i=0;
  if(gVerbose>2)
    printf("# Contexts: [ ");
  for(i=0;i<num_threads;i++)
  {
    td[i].context = context[i];
    td[i].tid     = i;
    td[i].logical_rank   = task_id*num_threads+i;
    if(gVerbose>2)
      printf("(%d|%d)%p ", i, td[i].logical_rank, context[i]);
  }
  if(gVerbose>2)
  printf("]\n");


  if(task_id == 0 && gVerbose>2) printf("# %s:  Querying World Geometry\n", argv[0]);
  rc |= query_geometry_world(client,
                             context[0],
                             &world_geometry,
                             barrier_xfer,
                             num_algorithm,
                             &always_works_algo,
                             &always_works_md,
                             &must_query_algo,
                             &must_query_md);

  if(rc != PAMI_SUCCESS)
    return 1;


  /* variables to control the delay for barrier correctness */
  non_root[0] = 1;                          /* first non-root ordinal in the comm  */
  non_root[1] = num_tasks - 1;/* last ordinal in the comm  */

  /*  Set up world barrier */
  barrier.cb_done   = cb_done;
  barrier.cookie    = (void*) & poll_flag;
  barrier.algorithm = always_works_algo[0];

  gContext = context[0];
  rc |= blocking_coll_advance_all(0, context, &barrier, &poll_flag);
  if(rc != PAMI_SUCCESS) return 1;
  int t;
  /* here we start a number of threads; each of them will query geometry and run an algo*/

  assert(gNum_contexts >= num_threads);
  if(task_id == 0) printf("# %s:  Tasks:%zu Threads/task:%d Contexts/task:%zu\n",
                          argv[0],num_tasks,num_threads, gNum_contexts);
  for(t=0; t<num_threads; t++)
  {
    rc = pthread_create(&threads[t], NULL, barrier_test, (void*)(&td[t]));
    if(rc)
    {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }

  /* Free attribute and wait for the other threads */
  void* status;
  for(t=0; t<num_threads; t++)
  {
    rc = pthread_join(threads[t], &status);
    if(rc)
    {
      printf("ERROR; return code from pthread_join() is %d\n", rc);
      exit(-1);
    }
  }
  gContext = context[0];
  blocking_coll_advance_all(0, context, &barrier, &poll_flag);

  free(always_works_algo);
  free(always_works_md);
  free(must_query_algo);
  free(must_query_md);
  rc |= pami_shutdown(&client, context, &gNum_contexts);
  return rc;
}


static void * barrier_test(void* p)
{
  thread_data_t     *td        = (thread_data_t*)p;
  pami_context_t     myContext = (pami_context_t)td->context;

  gThreadId = td->tid;

  /* Barrier variables */
  size_t                 newbar_num_algo[2];
  pami_algorithm_t      *newbar_algo        = NULL;
  pami_metadata_t       *newbar_md          = NULL;
  pami_algorithm_t      *q_newbar_algo      = NULL;
  pami_metadata_t       *q_newbar_md        = NULL;
  pami_xfer_type_t     barrier_xfer = PAMI_XFER_BARRIER;
  pami_xfer_t            newbarrier;
  int rc = 0;
  volatile unsigned    poll_flag = 0;
  int nalg;

  gContext  = td->context;
  gThreadId = td->tid;

  if(gVerbose>2 || (gVerbose && (task_id == 0)))
    printf("#%u: Enter thread %d on logical rank %d\n",gThreadId,td->tid, td->logical_rank);

  pami_geometry_range_t *range;
  int                    rangecount;
  pami_geometry_t        newgeometry;

  rangecount = 1;
  range     = (pami_geometry_range_t *)malloc(((rangecount)) * sizeof(pami_geometry_range_t));

  /*init range */
  range[0].lo = 0;
  range[0].hi = num_tasks-1;

  /*init range geometry; this will allocate an endpoint for each collective*/
  if(task_id == 0 && gVerbose>2) printf("#%u:  Creating Slice %d Geometry\n", gThreadId, td->tid);
  pami_geometry_t parent = (gNum_contexts>1)?PAMI_GEOMETRY_NULL:world_geometry;
  rc |= create_and_query_geometry_slice(client,
                                        (size_t)td->tid, /* slice (context) offset */
                                        myContext,
                                        myContext,
                                        parent,
                                        &newgeometry,
                                        range,
                                        rangecount,
                                        (unsigned)td->tid + 1, /* arbitrary (non-zero) unique id */
                                        barrier_xfer,
                                        newbar_num_algo,
                                        &newbar_algo,
                                        &newbar_md, 
                                        &q_newbar_algo,
                                        &q_newbar_md);

  if(rc != PAMI_SUCCESS)
  {
    if(gVerbose>2 || (gVerbose && (task_id == 0)))
      printf("#%u: Error exit thread %d on logical rank %d\n",gThreadId, td->tid, td->logical_rank);
    pthread_exit(NULL);
  }

  /*  Set up sub geometry barrier */
  newbarrier.cb_done   = cb_done;
  newbarrier.cookie    = (void*) & poll_flag;

  int total_alg = newbar_num_algo[0]+newbar_num_algo[1];
  for(nalg = 0; nalg < total_alg; nalg++)
  {
    pami_algorithm_t    *next_algo = NULL;
    pami_metadata_t     *next_md= NULL;
    unsigned query_protocol;
    if(nalg < newbar_num_algo[0])
    {
      query_protocol = 0;
      next_algo = &newbar_algo[nalg];
      next_md  = &newbar_md[nalg];
    }
    else
    {
      query_protocol = 1;
      next_algo = &q_newbar_algo[nalg-newbar_num_algo[0]];
      next_md  = &q_newbar_md[nalg-newbar_num_algo[0]];
    }

    gProtocolName = next_md->name;
    double ti, tf, usec;
    newbarrier.algorithm = *next_algo;

    if(task_id == 0)
    {
      printf("#%u:  Test:  Barrier Test -- context = %d, task_zero = %d, protocol: %s (%d of %d algorithms)\n",gThreadId,
             td->tid, task_id, gProtocolName, nalg + 1, total_alg);
      printf("#%u:  -------------------------------------------------------------------\n",gThreadId);
    }

    if(((strstr(gProtocolName,gSelected) == NULL) && gSelector) ||
       ((strstr(gProtocolName,gSelected) != NULL) && !gSelector))  continue;


    /* Do two functional runs with different delaying ranks*/
    int j;

    for(j = 0; j < 2; ++j)
    {
      if(task_id==0)
      {
        fprintf(stderr, "#%u: Test Barrier protocol(%s) Correctness (%d of %d algorithms)\n",gThreadId,
                gProtocolName, nalg + 1, total_alg);
        ti = timer();
        blocking_coll(myContext, &newbarrier, &poll_flag);
        tf = timer();
        usec = tf - ti;

        if((usec < 1800000.0 || usec > 2200000.0) && (num_tasks > 1))
        {
          rc = 1;
          fprintf(stderr, "#%u: %s FAIL: usec=%f want between %f and %f!\n",gThreadId, gProtocolName,
                  usec, 1800000.0, 2200000.0);
        }
        else
          fprintf(stderr, "#%u: %s PASS: Barrier correct!\n",gThreadId, gProtocolName);
      }
      else
      {
        /* Try to vary where the delay comes from... by picking first and last (non-roots) we
           *might* be getting same node/different node delays.
        */
        if(task_id == non_root[j])
          delayTest(2);

        blocking_coll(myContext, &newbarrier, &poll_flag);
      }
    }

    int niter = gNiterlat;
    blocking_coll(myContext, &newbarrier, &poll_flag);

    ti = timer();
    int i;

    for(i = 0; i < niter; i++)
      blocking_coll(myContext, &newbarrier, &poll_flag);

    tf = timer();
    usec = tf - ti;

    if(!task_id)
    {
      fprintf(stderr, "#%u: Test Barrier protocol(%s) Performance: time=%f usec\n",gThreadId,
              gProtocolName, usec / (double)niter);
      delayTest(2);
    }

    blocking_coll(myContext, &newbarrier, &poll_flag);
  }/*for all algos*/

  free(newbar_algo);
  free(newbar_md);
  free(q_newbar_algo);
  free(q_newbar_md);

  if(gVerbose>2 || (gVerbose && (task_id == 0)))
    printf("#%u:  Exit thread %d on logical rank %d\n",gThreadId,td->tid, td->logical_rank);
  pthread_exit(NULL);
}
