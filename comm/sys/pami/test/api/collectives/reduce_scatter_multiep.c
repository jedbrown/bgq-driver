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
 * \file test/api/collectives/reduce_scatter_multiep.c
 * \brief Simple Reduce_scatter test on endpoint geometries
 */

/* see setup_env() for environment variable overrides               */
#define MAX_THREADS 128
#define COUNT      65536
#define NITERLAT   10

#include "../pami_util.h"
#include <pthread.h>

static void *reduce_scatter_test(void*);
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
  if (rc != PAMI_SUCCESS)
    return 1;

  if(num_tasks > 32)
  {
    fprintf(stderr, "Fix the hardcoded size_t rcounts[32]; before running > 32 tasks\n");
    abort();
  }
  num_ep = num_tasks *gNum_contexts;

  assert(task_id >= 0);
  assert(task_id < num_tasks);

  int i=0;
  printf("Contexts: [ ");
  for(i=0;i<num_threads;i++)
    {
      td[i].context = context[i];
      td[i].tid     = i;
      td[i].logical_rank   = task_id*num_threads+i;
      printf("(%d|%d)%p ", i, td[i].logical_rank, context[i]);
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
    rc = pthread_create(&threads[t], NULL, reduce_scatter_test, (void*)(&td[t]));
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


static void * reduce_scatter_test(void* p)
{
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

  /* Reduce_Scatter variables */
  size_t               reduce_scatter_num_algorithm[2];
  pami_algorithm_t    *reduce_scatter_always_works_algo = NULL;
  pami_metadata_t     *reduce_scatter_always_works_md   = NULL;
  pami_algorithm_t    *reduce_scatter_must_query_algo   = NULL;
  pami_metadata_t     *reduce_scatter_must_query_md     = NULL;
  pami_xfer_type_t     reduce_scatter_xfer = PAMI_XFER_REDUCE_SCATTER;
  volatile unsigned    reduce_scatter_poll_flag = 0;

  int                  nalg= 0;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          reduce_scatter;
  int rc = 0;

  /*  Allocate buffer(s) */
  int err = 0;
  void* sbuf = NULL;
  err = posix_memalign(&sbuf, 128, MAX(num_tasks*gMax_datatype_sz,gMax_byte_count) + gBuffer_offset);
  assert(err == 0);
  sbuf = (char*)sbuf + gBuffer_offset;
  void* rbuf = NULL;
  err = posix_memalign(&rbuf, 128, MAX(num_tasks*gMax_datatype_sz,gMax_byte_count) + gBuffer_offset);
  assert(err == 0);
  rbuf = (char*)rbuf + gBuffer_offset;

  size_t rcounts[32];

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


  /*  Query the world geometry for reduce_scatter algorithms */
  rc |= query_geometry(client,
		       myContext,
		       newgeometry,
		       reduce_scatter_xfer,
		       reduce_scatter_num_algorithm,
		       &reduce_scatter_always_works_algo,
		       &reduce_scatter_always_works_md,
		       &reduce_scatter_must_query_algo,
		       &reduce_scatter_must_query_md);
  barrier.cb_done   = cb_done;
  barrier.cookie    = (void*) & bar_poll_flag;
  barrier.algorithm = bar_always_works_algo[0];

  blocking_coll(myContext, &barrier, &bar_poll_flag);

  pami_endpoint_t my_ep, zero_ep;
  PAMI_Endpoint_create(client,task_id,td->tid,&my_ep);
  PAMI_Endpoint_create(client,0,0,&zero_ep);
  for (nalg = 0; nalg < reduce_scatter_num_algorithm[0]; nalg++)
    {
      if (my_ep == zero_ep)
      {
        printf("# Reduce Scatter Bandwidth Test(size:%zu) -- context = %d,protocol: %s\n",num_tasks,
               td->tid, reduce_scatter_always_works_md[nalg].name);
        printf("# Size(bytes)      iterations     bytes/sec      usec\n");
        printf("# -----------      -----------    -----------    ---------\n");
      }

      if (((strstr(reduce_scatter_always_works_md[nalg].name, gSelected) == NULL) && gSelector) ||
          ((strstr(reduce_scatter_always_works_md[nalg].name, gSelected) != NULL) && !gSelector))  continue;

      gProtocolName = reduce_scatter_always_works_md[nalg].name;

      reduce_scatter.cb_done   = cb_done;
      reduce_scatter.cookie    = (void*)&reduce_scatter_poll_flag;
      reduce_scatter.algorithm = reduce_scatter_always_works_algo[nalg];
      reduce_scatter.cmd.xfer_reduce_scatter.sndbuf    = sbuf;
      reduce_scatter.cmd.xfer_reduce_scatter.stype     = PAMI_TYPE_BYTE;
      reduce_scatter.cmd.xfer_reduce_scatter.stypecount= 0;
      reduce_scatter.cmd.xfer_reduce_scatter.rcvbuf    = rbuf;
      reduce_scatter.cmd.xfer_reduce_scatter.rtype     = PAMI_TYPE_BYTE;

      int op, dt;
      for (dt=0; dt<dt_count; dt++)
      {
        for (op=0; op<op_count; op++)
        {
          if (gValidTable[op][dt])
          {
            size_t sz=get_type_size(dt_array[dt]);
            if (my_ep == zero_ep)
              printf("Running Reduce_scatter: %s, %s\n",dt_array_str[dt], op_array_str[op]);
            int i;
            for (i = MAX(num_ep,gMin_byte_count/sz); i <= MAX(num_ep,gMax_byte_count/sz); i *= 2)
            {
              size_t  dataSent = i * sz;
              int niter;

              if (dataSent < CUTOFF)
                niter = gNiterlat;
              else
                niter = NITERBW;
              int ind;
              for (ind =0; ind < num_ep; ++ind) rcounts[ind] = i / num_ep;

              reduce_scatter.cmd.xfer_reduce_scatter.stypecount=i;
              reduce_scatter.cmd.xfer_reduce_scatter.stype=dt_array[dt];
              reduce_scatter.cmd.xfer_reduce_scatter.rtype=dt_array[dt];
              reduce_scatter.cmd.xfer_reduce_scatter.op=op_array[op];
              reduce_scatter.cmd.xfer_reduce_scatter.rcounts=&rcounts[0];

              reduce_scatter_initialize_sndbuf (sbuf, i, op, dt, td->logical_rank);

              blocking_coll(myContext, &barrier, &bar_poll_flag);
              ti = timer();
              int j;
              for (j=0; j<niter; j++)
              {
                blocking_coll(myContext, &reduce_scatter, &reduce_scatter_poll_flag);
              }
              tf = timer();
              blocking_coll(myContext, &barrier, &bar_poll_flag);

              int rc_check;
              rc |= rc_check = reduce_scatter_check_rcvbuf (rbuf, i, op, dt, num_ep, td->logical_rank);

              if (rc_check) fprintf(stderr, "%s FAILED validation\n", gProtocolName);
              usec = (tf - ti)/(double)niter;
              if (my_ep == zero_ep)
              {
                printf("  %11lld %16d %14.1f %12.2f\n",
                       (long long)dataSent,
                       niter,
                       (double)1e6*(double)dataSent/(double)usec,
                       usec);
                fflush(stdout);
              }
            }
          }
        }
      }
    }

  free(bar_always_works_algo);
  free(bar_always_works_md);
  free(bar_must_query_algo);
  free(bar_must_query_md);
  free(reduce_scatter_always_works_algo);
  free(reduce_scatter_always_works_md);
  free(reduce_scatter_must_query_algo);
  free(reduce_scatter_must_query_md);

  sbuf = (char*)sbuf - gBuffer_offset;
  free(sbuf);
  rbuf = (char*)rbuf - gBuffer_offset;
  free(rbuf);

  pthread_exit(NULL);
}
