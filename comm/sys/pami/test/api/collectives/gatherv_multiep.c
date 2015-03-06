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
 * \file test/api/collectives/gatherv_multiep.c
 * \brief Simple Gatherv test on endpoint geometries
 */

/* see setup_env() for environment variable overrides               */
#define COUNT     (1048576*8)   /* see envvar TEST_BYTES for overrides */

#define MAX_THREADS 128
#include "../pami_util.h"
#include <pthread.h>

static void *gatherv_test(void*);

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
    rc = pthread_create(&threads[t], NULL, gatherv_test, (void*)(&td[t]));
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


static void * gatherv_test(void* p) {
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

  /* Gatherv variables */
  size_t               gatherv_num_algorithm[2];
  pami_algorithm_t    *gatherv_always_works_algo = NULL;
  pami_metadata_t     *gatherv_always_works_md   = NULL;
  pami_algorithm_t    *gatherv_must_query_algo   = NULL;
  pami_metadata_t     *gatherv_must_query_md     = NULL;
  pami_xfer_type_t     gatherv_xfer = PAMI_XFER_GATHERV;
  volatile unsigned    gatherv_poll_flag = 0;

  int                  nalg= 0;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          gatherv;
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


  /*  Query the world geometry for gatherv algorithms */
  rc |= query_geometry(client,
		       myContext,
		       newgeometry,
		       gatherv_xfer,
		       gatherv_num_algorithm,
		       &gatherv_always_works_algo,
		       &gatherv_always_works_md,
		       &gatherv_must_query_algo,
		       &gatherv_must_query_md);
  barrier.cb_done   = cb_done;
  barrier.cookie    = (void*) & bar_poll_flag;
  barrier.algorithm = bar_always_works_algo[0];


  blocking_coll(myContext, &barrier, &bar_poll_flag);

  pami_endpoint_t my_ep, zero_ep;
  PAMI_Endpoint_create(client,task_id,td->tid,&my_ep);
  PAMI_Endpoint_create(client,0,0,&zero_ep);

  for (nalg = 0; nalg < gatherv_num_algorithm[0]; nalg++)
    {
      gatherv.cb_done                      = cb_done;
      gatherv.cookie                       = (void*) & gatherv_poll_flag;
      gatherv.algorithm                    = gatherv_always_works_algo[nalg];
      gatherv.cmd.xfer_gatherv.sndbuf      = buf;
      gatherv.cmd.xfer_gatherv.stype       = PAMI_TYPE_BYTE;
      gatherv.cmd.xfer_gatherv.stypecount  = 0;
      gatherv.cmd.xfer_gatherv.rcvbuf      = rbuf;
      gatherv.cmd.xfer_gatherv.rtype       = PAMI_TYPE_BYTE;
      gatherv.cmd.xfer_gatherv.rtypecounts = lengths;
      gatherv.cmd.xfer_gatherv.rdispls     = displs;

      gProtocolName = gatherv_always_works_md[nalg].name;

      if (my_ep == zero_ep)
      {
        printf("# Gatherv Bandwidth Test(size:%zu) -- context = %d, protocol: %s\n",num_tasks,
               td->tid, gProtocolName);
        printf("# Size(bytes)      iterations     bytes/sec      usec\n");
        printf("# -----------      -----------    -----------    ---------\n");
      }

      if (((strstr(gatherv_always_works_md[nalg].name,gSelected) == NULL) && gSelector) ||
          ((strstr(gatherv_always_works_md[nalg].name,gSelected) != NULL) && !gSelector))  continue;


      size_t i, j;

      for (i = gMin_byte_count; i <= gMax_byte_count; i *= 2)
      {
        size_t  dataSent = i;
        int          niter;
        size_t           k = 0;

        for (k = 0; k < num_ep; k++)
        {
          lengths[k] = i;
          displs[k]  = k * i;
        }
        lengths[k-1] = 0;

        if (dataSent < CUTOFF)
          niter = gNiterlat;
        else
          niter = NITERBW;

        blocking_coll(myContext, &barrier, &bar_poll_flag);
        ti = timer();
        int         ctxt_id   = 0;
        pami_task_t root_task = 0;
        for (j = 0; j < niter; j++)
        {
          pami_endpoint_t root_ep;
          PAMI_Endpoint_create(client, root_task, ctxt_id, &root_ep);
          gatherv.cmd.xfer_gatherv.root        = root_ep;

          gather_initialize_sndbuf(td->logical_rank, buf, i);
          if (root_ep == zero_ep)
            memset(rbuf, 0xFF, i*num_ep);

          if (td->logical_rank != num_ep - 1)
            gatherv.cmd.xfer_gatherv.stypecount  = i;
          blocking_coll(myContext, &gatherv, &gatherv_poll_flag);

          if (my_ep == zero_ep)
          {
            int rc_check;
            rc |= rc_check = gather_check_rcvbuf(num_tasks-1, rbuf, i);
            if (rc_check) fprintf(stderr, "%s FAILED validation\n", gProtocolName);
          }
          ctxt_id   = (ctxt_id + 1)%gNum_contexts;
          if(ctxt_id == 0)
            root_task = (root_task +1)%num_tasks;
        }

        tf = timer();
        blocking_coll(myContext, &barrier, &bar_poll_flag);

        usec = (tf - ti) / (double)niter;

        if (my_ep == zero_ep)
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
  free(gatherv_always_works_algo);
  free(gatherv_always_works_md);
  free(gatherv_must_query_algo);
  free(gatherv_must_query_md);

  buf = (char*)buf - gBuffer_offset;
  free(buf);

  rbuf = (char*)rbuf - gBuffer_offset;
  free(rbuf);

  free(lengths);
  free(displs);

  pthread_exit(NULL);
}
