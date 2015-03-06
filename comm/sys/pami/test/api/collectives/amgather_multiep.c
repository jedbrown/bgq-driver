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
 * \file test/api/collectives/amgather_multiep.c
 * \brief Simple AMGather test on world geometry
 */

#define COUNT       524288
#define MAX_THREADS 128
/*
#define OFFSET     0
#define NITERBW    MIN(10, niterlat/100+1)
#define CUTOFF     65536
*/

#include "../pami_util.h"
#include <pthread.h>

#define AMDEBUG 0

#if AMDEBUG == 1
#define DEBUG(x) fprintf x
#else
#define DEBUG(x) 
#endif

typedef struct
{
  void *cookie;
  int bytes;
  pami_endpoint_t root_ep;
} validation_t;

typedef struct
{
  char         *sbuf;
  char         *rbuf;
  validation_t *val;
  void         *cookie;
} amcookie_t;

typedef struct
{
  unsigned dst_rank;
} user_header_t;

pami_geometry_t      newgeometry;
pami_task_t          task_id;
size_t               num_tasks;
size_t               num_ep;
pami_context_t      *context;
pami_client_t        client;
int                 _gRc = PAMI_SUCCESS;

/**
 *  Check if we have a valid context
 */
void check_context(pami_context_t ctxt)
{
  if(!ctxt) fprintf(stderr,
                    "%s: Error(tid=%d). Null context received in callback.\n",
                    gProtocolName,
                    gThreadId);
  if(gContext != ctxt) fprintf(stderr,
                               "%s: Context Error(tid=%d/%d) want:%p got:%p\n",
                               gProtocolName,
                               (int)pthread_self(),
                               gThreadId,
                               gContext,
                               ctxt);

#ifdef PAMI_TEST_STRICT
  assert(ctxt);
  assert(gContext==ctxt);
#endif

  pami_configuration_t configs;
  configs.name         = PAMI_CONTEXT_DISPATCH_ID_MAX;
  configs.value.intval = -1;

  pami_result_t rc;
  rc = PAMI_Context_query (ctxt,&configs,1);

  if(rc != PAMI_SUCCESS && rc != PAMI_INVAL)
    fprintf(stderr,"%s: Error. Could not query the context(%u).\n",gProtocolName,rc);
#ifdef PAMI_TEST_STRICT
  assert(rc == PAMI_SUCCESS || rc == PAMI_INVAL);
#endif
}

/**
 *  Completion callback
 */
void cb_amgather_done (void *ctxt, void * clientdata, pami_result_t err)
{
  validation_t *v = (validation_t*) clientdata;
  amcookie_t *amc = (amcookie_t *) v->cookie;
  volatile unsigned *active = (volatile unsigned *) amc->cookie;
  DEBUG((stderr, "%d:tid(%d): cb_amgather_done(): cookie value=%u\n", task_id, gThreadId, *active));
  if(gVerbose)
  {
    check_context((pami_context_t) ctxt);
  }
  pami_endpoint_t my_ep;
  PAMI_Endpoint_create(client, task_id, gThreadId, &my_ep);

  if(my_ep == v->root_ep)
  {
    int rc_check;
    _gRc |= rc_check = gather_check_rcvbuf (num_ep, amc->rbuf, v->bytes);
    if (rc_check) fprintf(stderr, "%d:tid(%d): %s FAILED validation\n", task_id, gThreadId, gProtocolName);
  }
  (*active)++;
}
/**
 *  User dispatch function
 */
void cb_amgather_send(pami_context_t         ctxt,         /**< IN:  communication context which invoked the dispatch function */
                      void                 * cookie,       /**< IN:  dispatch cookie */
                      const void           * header_addr,  /**< IN:  header address  */
                      size_t                 header_size,  /**< IN:  header size     */
                      size_t                 data_size,    /**< IN:  data size       */
                      pami_endpoint_t        origin,       /**< IN:  root initiating endpoint */
                      pami_geometry_t        geometry,     /**< IN:  Geometry */
                      pami_recv_t          * send)         /**< OUT: receive message structure */
{
  amcookie_t * amc = (amcookie_t *) cookie;
  if(gVerbose)
  {
    check_context(ctxt);
  }

  pami_task_t     task;
  size_t          offset;
  _gRc |= PAMI_Endpoint_query (origin,
                              &task,
                              &offset);

  DEBUG((stderr,"%d:tid(%d): cb_amgather_send(): header_addr=%p  header_size=%zu origin_task=%u origin_offset=%zu data_size=%zu dst_rank=%u cookie=%p cookieval=%u\n",
         task_id, gThreadId, header_addr, header_size, task, offset, data_size, ((user_header_t *)header_addr)->dst_rank, cookie, *((volatile unsigned int *)amc->cookie)));

  validation_t *v = amc->val + task + offset;
  v->cookie  = cookie;
  v->bytes   = data_size;
  v->root_ep = origin;
  gather_initialize_sndbuf(gThreadId + task_id * gNum_contexts, amc->sbuf, data_size);


  send->cookie      = (void*)v;
  send->local_fn    = cb_amgather_done;
  send->addr        = amc->sbuf;
  send->type        = PAMI_TYPE_BYTE;
  send->offset      = 0;
  send->data_fn     = PAMI_DATA_COPY;
  send->data_cookie = NULL;
}

static void *amgather_test(void *);

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
  context = (pami_context_t*) malloc(sizeof(pami_context_t) * gNum_contexts);
  thread_data_t *td = (thread_data_t*)malloc(sizeof(thread_data_t) * gNum_contexts);
  int rc = pami_init(&client,        /* Client             */
                     context,        /* Context            */
                     NULL,           /* Clientname=default */
                     &gNum_contexts, /* gNum_contexts       */
                     NULL,           /* null configuration */
                     0,              /* no configuration   */
                     &task_id,    /* task id            */
                     &num_tasks);    /* number of tasks    */
  if (rc != PAMI_SUCCESS)
    return 1;

  num_ep = num_tasks *gNum_contexts;

  assert(task_id >= 0);
  assert(task_id < num_tasks);

  int i=0;
  printf("Contexts: [ ");
  for(i=0;i<num_threads;i++)
    {
      td[i].context = context[i];
      td[i].tid     = i;
      td[i].logical_rank   = task_id * num_threads + i;
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
    rc = pthread_create(&threads[t], NULL, amgather_test, (void*)(&td[t]));
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

static void *amgather_test(void *p)
{
  thread_data_t       *td        = (thread_data_t*)p;
  pami_context_t       myContext = (pami_context_t)td->context;
  pami_task_t root_task = 0;

  /* Barrier variables */
  size_t               barrier_num_algorithm[2];
  pami_algorithm_t    *bar_always_works_algo = NULL;
  pami_metadata_t     *bar_always_works_md   = NULL;
  pami_algorithm_t    *bar_must_query_algo   = NULL;
  pami_metadata_t     *bar_must_query_md     = NULL;
  pami_xfer_type_t     barrier_xfer = PAMI_XFER_BARRIER;
  pami_xfer_t          barrier;
  volatile unsigned    bar_poll_flag = 0;

  /* Amgather variables */
  size_t               amgather_num_algorithm[2];
  pami_algorithm_t    *amgather_always_works_algo = NULL;
  pami_metadata_t     *amgather_always_works_md = NULL;
  pami_algorithm_t    *amgather_must_query_algo = NULL;
  pami_metadata_t     *amgather_must_query_md = NULL;
  pami_xfer_type_t     amgather_xfer = PAMI_XFER_AMGATHER;
  pami_xfer_t          amgather;
  volatile unsigned    amgather_total_count = 0;

  int                  nalg = 0, i;
  double               ti, tf, usec;
  int                  rc = 0;

  if(gNumRoots == -1) gNumRoots = num_ep;

  /*  Allocate buffer(s) */
  int err = 0;
  void *sbuf = NULL;
  err = posix_memalign(&sbuf, 128, gMax_byte_count + gBuffer_offset);
  assert(err == 0);
  sbuf = (char*)sbuf + gBuffer_offset;

  void* rbuf = NULL;
  err = posix_memalign(&rbuf, 128, (gMax_byte_count * num_ep) + gBuffer_offset);
  assert(err == 0);
  rbuf = (char*)rbuf + gBuffer_offset;

  void *headers = NULL;
  err = posix_memalign((void **)&headers, 128, (num_ep * sizeof(user_header_t)) + gBuffer_offset);
  headers = (char*)headers + gBuffer_offset;

  void *validation = NULL;
  err = posix_memalign((void **)&validation, 128, (num_ep * sizeof(validation_t)) + gBuffer_offset);
  validation = (char*)validation + gBuffer_offset;

  /* Initialize the headers */
  for(i = 0; i < num_ep; ++i)
  {
    ((user_header_t *)headers)[i].dst_rank = i;
  }

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

  /*  Query the world geometry for amgather algorithms */
  rc |= query_geometry(client,
                       myContext,
                       newgeometry,
                       amgather_xfer,
                       amgather_num_algorithm,
                       &amgather_always_works_algo,
                       &amgather_always_works_md,
                       &amgather_must_query_algo,
                       &amgather_must_query_md);

  barrier.cb_done     = cb_done;
  barrier.cookie      = (void*) & bar_poll_flag;
  barrier.algorithm   = bar_always_works_algo[0];
  blocking_coll(myContext, &barrier, &bar_poll_flag);

  pami_endpoint_t my_ep, zero_ep;
  PAMI_Endpoint_create(client,task_id,td->tid,&my_ep);
  PAMI_Endpoint_create(client,0,0,&zero_ep);

  amgather.algorithm = amgather_always_works_algo[0];
  amgather.cmd.xfer_amgather.headers      = headers;
  amgather.cmd.xfer_amgather.headerlen    = sizeof(user_header_t);
  amgather.cmd.xfer_amgather.rcvbuf       = rbuf;
  amgather.cmd.xfer_amgather.rtype        = PAMI_TYPE_BYTE;
  amgather.cmd.xfer_amgather.rtypecount   = 0;

  amcookie_t amcookie;
  amcookie.sbuf     = sbuf;
  amcookie.rbuf     = rbuf;
  amcookie.val      = validation;
  amcookie.cookie   = (void *) &amgather_total_count;
  gThreadId = td->tid;

  for (nalg = 0; nalg < amgather_num_algorithm[0]; nalg++)
  {
    gProtocolName = amgather_always_works_md[nalg].name;

    if (my_ep == zero_ep)
    {
      printf("# AMGather Multi-Endpoint Bandwidth Test -- context = %d, protocol: %s\n",
             td->tid, amgather_always_works_md[nalg].name);
      printf("# Size(bytes)           cycles    bytes/sec    usec\n");
      printf("# -----------      -----------    -----------    ---------\n");
      fflush(stdout);
    }

    if (((strstr(amgather_always_works_md[nalg].name,gSelected) == NULL) && gSelector) ||
        ((strstr(amgather_always_works_md[nalg].name,gSelected) != NULL) && !gSelector))  continue;

    int j;
    pami_collective_hint_t h = {0};
    pami_dispatch_callback_function fn;
    fn.amgather = cb_amgather_send;

    PAMI_AMCollective_dispatch_set(myContext,
                                   amgather_always_works_algo[nalg],
                                   root_task,/* Set the dispatch id, can be any arbitrary value */
                                   fn,
                                   (void*) &amcookie,
                                   h);
    amgather.cmd.xfer_amgather.dispatch = root_task;
    amgather.algorithm = amgather_always_works_algo[nalg];

    volatile unsigned *ngather = &amgather_total_count;
    for (i = 1; i <= gMax_byte_count; i *= 2)
    {
      size_t  dataSent = i;
      int     niter;
      pami_result_t result;

      if (dataSent < CUTOFF)
        niter = gNiterlat;
      else
        niter = NITERBW;

      *ngather = 0;
      amgather.cmd.xfer_amgather.rtypecount = i;

      blocking_coll(myContext, &barrier, &bar_poll_flag);
      gContext  = myContext;
      ti = timer();

      int ctxt_id = 0;
      for (j = 0; j < niter; j++)
      {
        pami_endpoint_t root_ep;
        PAMI_Endpoint_create(client, root_task, ctxt_id, &root_ep);

        if (my_ep == root_ep)
        {
          memset(rbuf, 0xFF, i * num_ep);
          rc |= result = PAMI_Collective(myContext, &amgather);
          if (result != PAMI_SUCCESS)
            {
              fprintf (stderr, "Error. Unable to issue  collective. result = %d\n", result);
              goto fn_exit;
            }
        }
        while (*ngather <= j)
          result = PAMI_Context_advance (myContext, 1);

        ctxt_id   = (ctxt_id + 1) % gNum_contexts;
        if(ctxt_id == 0)
          root_task = (root_task + 1) % num_tasks;

        rc |= _gRc; /* validation return code done in cb_amgather_done */
      }

      assert(*ngather == niter);
      tf = timer();
      blocking_coll(myContext, &barrier, &bar_poll_flag);

      usec = (tf - ti) / (double)niter;
      if(my_ep == zero_ep)
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

fn_exit:
  free(bar_always_works_algo);
  free(bar_always_works_md);
  free(bar_must_query_algo);
  free(bar_must_query_md);
  free(amgather_always_works_algo);
  free(amgather_always_works_md);
  free(amgather_must_query_algo);
  free(amgather_must_query_md);

  sbuf = (char*)sbuf - gBuffer_offset;
  free(sbuf);

  rbuf = (char*)rbuf - gBuffer_offset;
  free(rbuf);

  headers = (char*)headers - gBuffer_offset;
  free(headers);

  validation = (char*)validation - gBuffer_offset;
  free(validation);

  pthread_exit(NULL);
}
