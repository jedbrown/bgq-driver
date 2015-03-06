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
 * \file test/api/collectives/alltoall_multiep.c
 * \brief Simple Alltoall test on endpoint geometries
 */

/* Use arg -h or see setup_env() for environment variable overrides  */
#define COUNT     (524288)   /* see envvar TEST_BYTES for overrides */

#define MAX_THREADS 128
#include "../pami_util.h"
#include <pthread.h>

static void *alltoall_test(void*);
pami_geometry_t      newgeometry;
pami_task_t          task_id;
size_t               num_tasks;
size_t               num_ep;
pami_context_t      *context;
pami_client_t        client;


THREAD_LOCAL char *sbuf = NULL;
THREAD_LOCAL char *rbuf = NULL;

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
  pami_geometry_t      world_geometry;
  size_t               num_algorithm[2];
  pami_algorithm_t    *always_works_algo = NULL;
  pami_metadata_t     *always_works_md   = NULL;
  pami_algorithm_t    *must_query_algo   = NULL;
  pami_metadata_t     *must_query_md     = NULL;
  pami_xfer_type_t     barrier_xfer      = PAMI_XFER_BARRIER;
  pami_xfer_t          barrier;
  volatile unsigned    poll_flag         = 0;
  int                  num_threads       = gNum_contexts;

  assert(gNum_contexts > 0);
  context = (pami_context_t*)malloc(sizeof(pami_context_t) * gNum_contexts);
  thread_data_t *td = (thread_data_t*)malloc(sizeof(thread_data_t) * gNum_contexts);
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

  num_ep = num_tasks *gNum_contexts;

  assert(task_id >= 0);
  assert(task_id < num_tasks);

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

  /*  Create the range, all context, endpoint geometry */
  pami_geometry_range_t *range;
  int                    rangecount;

  rangecount = 1;
  range     = (pami_geometry_range_t *)malloc(((rangecount)) * sizeof(pami_geometry_range_t));

  /*init range */
  range[0].lo = 0;
  range[0].hi = num_tasks-1;

  /*init range geometry; this will allocate an endpoint for each collective*/
  if(task_id == 0 && gVerbose>2) printf("# %s:  Creating All Context World Geometry\n", argv[0]);
  pami_geometry_t parent = (gNum_contexts>1)?PAMI_GEOMETRY_NULL:world_geometry;
  rc |= create_all_ctxt_geometry(client,
                                 context,
                                 gNum_contexts,
                                 parent,
                                 &newgeometry,
                                 range,
                                 rangecount,
                                 1);

  if(rc != PAMI_SUCCESS)
    return 1;

  /*  Set up world barrier */
  barrier.cb_done   = cb_done;
  barrier.cookie    = (void*) & poll_flag;
  barrier.algorithm = always_works_algo[0];

  rc |= blocking_coll_advance_all(0, context, &barrier, &poll_flag);
  if(rc != PAMI_SUCCESS) return 1;
  int t;

  assert(gNum_contexts >= num_threads);
  if(task_id == 0) printf("# %s:  Tasks:%zu Threads/task:%d Contexts/task:%zu\n",
                          argv[0],num_tasks,num_threads, gNum_contexts);
  for(t=0; t<num_threads; t++)
  {
    rc = pthread_create(&threads[t], NULL, alltoall_test, (void*)(&td[t]));
    if(rc)
    {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }

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

  blocking_coll_advance_all(0, context, &barrier, &poll_flag);

  free(always_works_algo);
  free(always_works_md);
  free(must_query_algo);
  free(must_query_md);
  rc |= pami_shutdown(&client, context, &gNum_contexts);
  return rc;
}


static void * alltoall_test(void* p)
{
  thread_data_t       *td        = (thread_data_t*)p;
  pami_context_t       myContext = (pami_context_t)td->context;

  gThreadId = td->tid;

  /* Barrier variables */
  size_t               barrier_num_algorithm[2];
  pami_algorithm_t    *bar_always_works_algo = NULL;
  pami_metadata_t     *bar_always_works_md   = NULL;
  pami_algorithm_t    *bar_must_query_algo   = NULL;
  pami_metadata_t     *bar_must_query_md     = NULL;
  pami_xfer_type_t     barrier_xfer = PAMI_XFER_BARRIER;
  volatile unsigned    bar_poll_flag = 0;

  /* Alltoall variables */
  size_t               alltoall_num_algorithm[2];
  pami_algorithm_t    *alltoall_always_works_algo = NULL;
  pami_metadata_t     *alltoall_always_works_md   = NULL;
  pami_algorithm_t    *alltoall_must_query_algo   = NULL;
  pami_metadata_t     *alltoall_must_query_md     = NULL;
  pami_xfer_type_t     alltoall_xfer = PAMI_XFER_ALLTOALL;
  volatile unsigned    alltoall_poll_flag = 0;

  int                  nalg= 0;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          alltoall;
  int rc = 0;

  if(gVerbose>2 || (gVerbose && (task_id == 0)))
    printf("#%u: Enter thread %d on logical rank %d\n",gThreadId,td->tid, td->logical_rank);

  /*  Allocate buffer(s) */
  int err = 0;
  err = posix_memalign((void*)&sbuf, 128, (gMax_byte_count*num_ep)+gBuffer_offset);
  assert(err == 0);
  sbuf = (char*)sbuf + gBuffer_offset;

  err = posix_memalign((void*)&rbuf, 128, (gMax_byte_count*num_ep)+gBuffer_offset);
  assert(err == 0);
  rbuf = (char*)rbuf + gBuffer_offset;

  /*  Query the endpoint geometry for barrier algorithms */
  rc |= query_geometry(client,
                       myContext,
                       newgeometry,
                       barrier_xfer,
                       barrier_num_algorithm,
                       &bar_always_works_algo,
                       &bar_always_works_md,
                       &bar_must_query_algo,
                       &bar_must_query_md);


  /*  Query the endpoint geometry for alltoall algorithms */
  rc |= query_geometry(client,
                       myContext,
                       newgeometry,
                       alltoall_xfer,
                       alltoall_num_algorithm,
                       &alltoall_always_works_algo,
                       &alltoall_always_works_md,
                       &alltoall_must_query_algo,
                       &alltoall_must_query_md);

  barrier.cb_done   = cb_done;
  barrier.cookie    = (void*) & bar_poll_flag;
  barrier.algorithm = bar_always_works_algo[0];

  blocking_coll(myContext, &barrier, &bar_poll_flag);


  alltoall.cb_done    = cb_done;
  alltoall.cookie     = (void*) & alltoall_poll_flag;

  pami_endpoint_t my_ep, zero_ep;
  PAMI_Endpoint_create(client,task_id,td->tid,&my_ep);
  PAMI_Endpoint_create(client,0,0,&zero_ep);
  int total_alg = alltoall_num_algorithm[0]+alltoall_num_algorithm[1];
  for(nalg = 0; nalg < total_alg; nalg++)
  {
    pami_algorithm_t    *next_algo = NULL;
    pami_metadata_t     *next_md= NULL;
    size_t i, j;
    metadata_result_t result = {0};
    unsigned query_protocol;
    if(nalg < alltoall_num_algorithm[0])
    {
      query_protocol = 0;
      next_algo = &alltoall_always_works_algo[nalg];
      next_md  = &alltoall_always_works_md[nalg];
    }
    else
    {
      query_protocol = 1;
      next_algo = &alltoall_must_query_algo[nalg-alltoall_num_algorithm[0]];
      next_md  = &alltoall_must_query_md[nalg-alltoall_num_algorithm[0]];
    }

    gProtocolName = next_md->name;
    if((my_ep == zero_ep) || (gVerbose>2))
    {
      printf("#%u: Alltoall Bandwidth Test(size:%zu)  -- context = %d, protocol: %s\n",gThreadId,
             num_ep, td->tid, gProtocolName);
    }
    if(my_ep == zero_ep)
    {
      printf("#%u: Size(bytes)  iterations    bytes/sec      usec\n",gThreadId);
      printf("#%u: -----------      -----------    -----------    ---------\n",gThreadId);
    }

    if(((strstr(gProtocolName, gSelected) == NULL) && gSelector) ||
       ((strstr(gProtocolName, gSelected) != NULL) && !gSelector))  continue;

    alltoall.algorithm  = *next_algo;

    for(i = gMin_byte_count; i <= gMax_byte_count; i *= 2)
    {
      size_t  dataSent = i;
      int          niter;

      if(dataSent < CUTOFF)
        niter = gNiterlat;
      else
        niter = NITERBW;

      for(j = 0; j < num_ep; j++)
      {
        alltoall_initialize_bufs(sbuf, rbuf, i, j );
      }

      blocking_coll(myContext, &barrier, &bar_poll_flag);

      /* Warmup */
      alltoall.cmd.xfer_alltoall.sndbuf        = sbuf;
      alltoall.cmd.xfer_alltoall.stype         = PAMI_TYPE_UNSIGNED_CHAR;
      alltoall.cmd.xfer_alltoall.stypecount    = i;
      alltoall.cmd.xfer_alltoall.rcvbuf        = rbuf;
      alltoall.cmd.xfer_alltoall.rtype         = PAMI_TYPE_UNSIGNED_CHAR;
      alltoall.cmd.xfer_alltoall.rtypecount    = i;

      if(query_protocol)
      {
        if(gVerbose>=2 && (task_id == 0))
          fprintf(stderr, "#%u:  check_metadata\n",td->logical_rank);
        result = check_metadata(*next_md,
                                alltoall,
                                PAMI_TYPE_UNSIGNED_CHAR,
                                i, /* metadata uses bytes i, */
                                sbuf,
                                PAMI_TYPE_UNSIGNED_CHAR,
                                i,
                                rbuf);
        if(next_md->check_correct.values.nonlocal)
        {
          /* \note We currently ignore check_correct.values.nonlocal
                        because these tests should not have nonlocal differences (so far). */
          result.check.nonlocal = 0;
        }
        if(result.bitmask) continue;
      }
      blocking_coll(myContext, &alltoall, &alltoall_poll_flag);
      blocking_coll(myContext, &alltoall, &alltoall_poll_flag);

      blocking_coll(myContext, &barrier, &bar_poll_flag);

      ti = timer();

      for(j = 0; j < niter; j++)
      {
        alltoall.cmd.xfer_alltoall.sndbuf        = sbuf;
        alltoall.cmd.xfer_alltoall.stype         = PAMI_TYPE_UNSIGNED_CHAR;
        alltoall.cmd.xfer_alltoall.stypecount    = i;
        alltoall.cmd.xfer_alltoall.rcvbuf        = rbuf;
        alltoall.cmd.xfer_alltoall.rtype         = PAMI_TYPE_UNSIGNED_CHAR;
        alltoall.cmd.xfer_alltoall.rtypecount    = i;
        blocking_coll(myContext, &alltoall, &alltoall_poll_flag);
      }

      tf = timer();

      int rc_check;
      rc |= rc_check = alltoall_check_rcvbuf(rbuf, i, num_ep, td->logical_rank);
      if(rc_check)
      {
        fprintf(stderr, "#%u: %s FAILED validation\n",gThreadId, gProtocolName);
      }

      blocking_coll(myContext, &barrier, &bar_poll_flag);

      usec = (tf - ti) / (double)niter;

      if(my_ep == zero_ep)
      {

        printf(" %u:   %11zu %16d %14.1f %12.2f\n",gThreadId,
               dataSent,
               niter,
               (double)1e6*(double)dataSent / (double)usec,
               usec);
        fflush(stdout);
      }
    }
  }


  free(bar_always_works_algo);
  free(bar_always_works_md);
  free(bar_must_query_algo);
  free(bar_must_query_md);
  free(alltoall_always_works_algo);
  free(alltoall_always_works_md);
  free(alltoall_must_query_algo);
  free(alltoall_must_query_md);

  sbuf = (char*)sbuf - gBuffer_offset;
  free(sbuf);

  rbuf = (char*)rbuf - gBuffer_offset;
  free(rbuf);

  if(gVerbose>2 || (gVerbose && (task_id == 0)))
    printf("#%u:  Exit thread %d on logical rank %d\n",gThreadId,td->tid, td->logical_rank);
  pthread_exit(NULL);
}
