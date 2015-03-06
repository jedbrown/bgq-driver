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
 * \file test/api/collectives/scatterv_multiep.c
 * \brief Simple Scatterv test on endpoint geometries
 */

/* see setup_env() for environment variable overrides               */
#define COUNT     (1048576*8)   /* see envvar TEST_BYTES for overrides */

#define MAX_THREADS 128
#include "../pami_util.h"
#include <pthread.h>

static void *scatterv_test(void*);

pami_geometry_t      newgeometry;
pami_task_t          task_id;
size_t               num_tasks;
size_t               num_ep;
pami_context_t      *context;
pami_client_t        client;

typedef struct thread_data_t
{
  pami_context_t context;
  int            tid;
  int            logical_rank;
} thread_data_t;


int main(int argc, char*argv[])
{
  setup_env();
  pthread_t threads[MAX_THREADS];
  pami_geometry_t      world_geometry;
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
  thread_data_t *td = (thread_data_t*)malloc(sizeof(thread_data_t) * gNum_contexts);
  int rc = pami_init(&client,        /* Client             */
                     context,        /* Context            */
                     NULL,           /* Clientname=default */
                     &gNum_contexts, /* gNum_contexts       */
                     NULL,           /* null configuration */
                     0,              /* no configuration   */
                     &task_id,       /* task id            */
                     &num_tasks);    /* number of tasks    */

  if (rc != PAMI_SUCCESS)
    return 1;

  assert(task_id >= 0);
  assert(task_id < num_tasks);

  num_ep = num_tasks *gNum_contexts;

  int i=0;
  printf("Contexts: [ ");
  for(i=0;i<num_threads;i++)
    {
      printf("%p ", context[i]);
      td[i].context      = context[i];
      td[i].tid          = i;
      td[i].logical_rank = task_id*num_threads+i;
    }
  printf("]\n");

  if(task_id == 0) printf("%s:  Querying World Geometry\n", argv[0]);
  rc |= query_geometry_world(client,
                             context[0],
                             &world_geometry,
                             barrier_xfer,
                             num_algorithm,
                             &always_works_algo,
                             &always_works_md,
                             &must_query_algo,
                             &must_query_md);

  if (rc != PAMI_SUCCESS)
    return 1;

  /*  Create the range geometry */
  pami_geometry_range_t *range;
  int                    rangecount;

  rangecount = 1;
  range     = (pami_geometry_range_t *)malloc(((rangecount)) * sizeof(pami_geometry_range_t));

  /*init range */
  range[0].lo = 0;
  range[0].hi = num_tasks-1;

  /*init range geometry; this will allocate an endpoint for each collective*/
  if(task_id == 0) printf("%s:  Creating All Context World Geometry\n", argv[0]);
  pami_geometry_t parent = (gNum_contexts>1)?PAMI_GEOMETRY_NULL:world_geometry;
  rc |= create_all_ctxt_geometry(client,
                                 context,
                                 gNum_contexts,
                                 parent,
                                 &newgeometry,
                                 range,
                                 rangecount,
                                 1);

  if (rc != PAMI_SUCCESS)
    return 1;

  /*  Set up world barrier */
  barrier.cb_done   = cb_done;
  barrier.cookie    = (void*) & poll_flag;
  barrier.algorithm = always_works_algo[0];

  rc |= blocking_coll_advance_all(0, context, &barrier, &poll_flag);
  if (rc != PAMI_SUCCESS) return 1;
  int t;

  assert(gNum_contexts >= num_threads);
  if(task_id == 0) printf("%s:  Tasks:%zu Threads/task:%d Contexts/task:%zu\n",
                          argv[0],num_tasks,num_threads, gNum_contexts);
  for(t=0; t<num_threads; t++){
    rc = pthread_create(&threads[t], NULL, scatterv_test, (void*)(&td[t]));
    if (rc){
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }

  void* status;
  for(t=0; t<num_threads; t++) {
    rc = pthread_join(threads[t], &status);
    if (rc) {
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


static void * scatterv_test(void* p) {
  thread_data_t       *td        = (thread_data_t*)p;
  pami_context_t       myContext = (pami_context_t)td->context;

  /* Barrier variables */
  size_t               barrier_num_algorithm[2];
  pami_algorithm_t    *bar_always_works_algo = NULL;
  pami_metadata_t     *bar_always_works_md   = NULL;
  pami_algorithm_t    *bar_must_query_algo   = NULL;
  pami_metadata_t     *bar_must_query_md     = NULL;
  pami_xfer_type_t     barrier_xfer = PAMI_XFER_BARRIER;
  volatile unsigned    bar_poll_flag = 0;

  /* Scatterv variables */
  size_t               scatterv_num_algorithm[2];
  pami_algorithm_t    *scatterv_always_works_algo = NULL;
  pami_metadata_t     *scatterv_always_works_md   = NULL;
  pami_algorithm_t    *scatterv_must_query_algo   = NULL;
  pami_metadata_t     *scatterv_must_query_md     = NULL;
  pami_xfer_type_t     scatterv_xfer = PAMI_XFER_SCATTERV;
  volatile unsigned    scatterv_poll_flag = 0;

  int                  nalg= 0;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          scatterv;
  int rc = 0;

  if(gNumRoots == -1) gNumRoots = num_ep;

  /*  Allocate buffer(s) */
  int err = 0;
  void* buf = NULL;
  err = posix_memalign(&buf, 128, (gMax_byte_count * num_ep) + gBuffer_offset);
  assert(err == 0);
  buf = (char*)buf + gBuffer_offset;

  void* rbuf = NULL;
  err = posix_memalign(&rbuf, 128, (gMax_byte_count * num_ep) + gBuffer_offset);
  assert(err == 0);
  rbuf = (char*)rbuf + gBuffer_offset;

  size_t *lengths   = (size_t*)malloc(num_ep * sizeof(size_t));
  assert(lengths);
  size_t *displs    = (size_t*)malloc(num_ep * sizeof(size_t));
  assert(displs);

  /*  Query the world geometry for barrier algorithms */
  rc |= query_geometry(client,
		       myContext,
		       newgeometry,
		       barrier_xfer,
		       barrier_num_algorithm,
		       &bar_always_works_algo,
		       &bar_always_works_md,
		       &bar_must_query_algo,
		       &bar_must_query_md);


  /*  Query the world geometry for scatterv algorithms */
  rc |= query_geometry(client,
		       myContext,
		       newgeometry,
		       scatterv_xfer,
		       scatterv_num_algorithm,
		       &scatterv_always_works_algo,
		       &scatterv_always_works_md,
		       &scatterv_must_query_algo,
		       &scatterv_must_query_md);
  barrier.cb_done   = cb_done;
  barrier.cookie    = (void*) & bar_poll_flag;
  barrier.algorithm = bar_always_works_algo[0];


  blocking_coll(myContext, &barrier, &bar_poll_flag);

  pami_endpoint_t my_ep, zero_ep;
  PAMI_Endpoint_create(client,task_id,td->tid,&my_ep);
  PAMI_Endpoint_create(client,0,0,&zero_ep);

  for (nalg = 0; nalg < scatterv_num_algorithm[0]; nalg++)
    {
      pami_endpoint_t    root_ep;
      pami_task_t root_task = 0;
      PAMI_Endpoint_create(client, root_task, 0, &root_ep);
      scatterv.cmd.xfer_scatterv.root        = root_ep;
      scatterv.cb_done                       = cb_done;
      scatterv.cookie                        = (void*) & scatterv_poll_flag;
      scatterv.algorithm                     = scatterv_always_works_algo[nalg];
      scatterv.cmd.xfer_scatterv.sndbuf      = buf;
      scatterv.cmd.xfer_scatterv.stype       = PAMI_TYPE_BYTE;
      scatterv.cmd.xfer_scatterv.stypecounts = lengths;
      scatterv.cmd.xfer_scatterv.sdispls     = displs;
      scatterv.cmd.xfer_scatterv.rcvbuf      = rbuf;
      scatterv.cmd.xfer_scatterv.rtype       = PAMI_TYPE_BYTE;
      scatterv.cmd.xfer_scatterv.rtypecount  = 0;

      gProtocolName = scatterv_always_works_md[nalg].name;

      if (my_ep == root_ep)
      {
        printf("# Scatterv Bandwidth Test(size:%zu) -- context = %d, protocol: %s\n",num_tasks,
               td->tid, gProtocolName);
        printf("# Size(bytes)      iterations     bytes/sec      usec\n");
        printf("# -----------      -----------    -----------    ---------\n");
      }

      if (((strstr(scatterv_always_works_md[nalg].name,gSelected) == NULL) && gSelector) ||
          ((strstr(scatterv_always_works_md[nalg].name,gSelected) != NULL) && !gSelector))  continue;


      size_t i, j;

      for (i = gMin_byte_count; i <= gMax_byte_count; i *= 2)
      {
        size_t  dataSent = i;
        int          niter;
        size_t           k = 0;

        for (k = 0; k < num_ep; k++)
        {
          lengths[k] = i;
          displs[k]  = 0;
        }

        if (dataSent < CUTOFF)
          niter = gNiterlat;
        else
          niter = NITERBW;

        blocking_coll(myContext, &barrier, &bar_poll_flag);
        ti = timer();

        for (j = 0; j < niter; j++)
        {
          scatterv.cmd.xfer_scatterv.rtypecount = i;
          blocking_coll(myContext, &scatterv, &scatterv_poll_flag);
        }

        tf = timer();
        blocking_coll(myContext, &barrier, &bar_poll_flag);

        usec = (tf - ti) / (double)niter;

        if (my_ep == root_ep)
        {
          printf("  %11lld %16d %14.1f %12.2f\n",
                 (long long)dataSent,
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
  free(scatterv_always_works_algo);
  free(scatterv_always_works_md);
  free(scatterv_must_query_algo);
  free(scatterv_must_query_md);

  buf = (char*)buf - gBuffer_offset;
  free(buf);

  rbuf = (char*)rbuf - gBuffer_offset;
  free(rbuf);

  free(lengths);
  free(displs);

  pthread_exit(NULL);
}
