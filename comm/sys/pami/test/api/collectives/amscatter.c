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
 * \file test/api/collectives/amscatter.c
 * \brief Simple AMScatter test on world geometry
 */

/* see setup_env() for environment variable overrides               */
#define COUNT      524288

#include "../pami_util.h"

#define AMDEBUG 0

#if AMDEBUG == 1
#define DEBUG(x) fprintf x
#else
#define DEBUG(x) 
#endif

typedef struct
{
  void *rbuf;
  void *cookie;
  int bytes;
  int root;
} validation_t;

typedef struct
{
  unsigned dst_rank;
} user_header_t;

pami_context_t      lgContext = NULL;
pami_task_t         my_task_id;
size_t              num_tasks;
int                 _gRc = PAMI_SUCCESS;

char                   *_g_recv_buffer;
char                   *_g_send_buffer;
validation_t           *_g_val_buffer;

/**
 *  Check if we have a valid context
 */
void check_context(pami_context_t context)
{
  if(!context)
    fprintf(stderr, "%s: Error. Null context received in callback.\n",gProtocolName);
  if(lgContext != context)
    fprintf(stderr, "%s: Error. Unexpected context received in callback %p != %p.\n",gProtocolName,lgContext,context);
#ifdef PAMI_TEST_STRICT
  assert(context);
  assert(lgContext==context);
#endif

  pami_configuration_t configs;
  configs.name         = PAMI_CONTEXT_DISPATCH_ID_MAX;
  configs.value.intval = -1;

  pami_result_t rc;
  rc = PAMI_Context_query (context,&configs,1);

  if(rc != PAMI_SUCCESS && rc != PAMI_INVAL)
    fprintf(stderr,"%s: Error. Could not query the context(%u).\n",gProtocolName,rc);
#ifdef PAMI_TEST_STRICT
  assert(rc == PAMI_SUCCESS || rc == PAMI_INVAL);
#endif
}

/**
 *  Completion callback
 */
void cb_amscatter_done (void *context, void * clientdata, pami_result_t err)
{
  validation_t *v = (validation_t*)clientdata;
  volatile unsigned *active = (volatile unsigned *) v->cookie;
  DEBUG((stderr, "cb_amscatter_done(): cookie= %p value=%u\n", active, *active));
  if(gVerbose)
  {
    check_context((pami_context_t) context);
  }

  int rc_check;
  _gRc |= rc_check = scatter_check_rcvbuf (_g_recv_buffer, v->bytes, my_task_id);
  if (rc_check) fprintf(stderr, "%s FAILED validation\n", gProtocolName);

  (*active)++;
}
/**
 *  User dispatch function
 */
void cb_amscatter_recv(pami_context_t        context,      /**< IN:  communication context which invoked the dispatch function */
                      void                 * cookie,       /**< IN:  dispatch cookie */
                      const void           * header_addr,  /**< IN:  header address  */
                      size_t                 header_size,  /**< IN:  header size     */
                      const void           * pipe_addr,    /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
                      size_t                 data_size,    /**< IN:  data size       */
                      pami_endpoint_t        origin,       /**< IN:  root initiating endpoint */
                      pami_geometry_t        geometry,     /**< IN:  Geometry */
                      pami_recv_t          * recv)         /**< OUT: receive message structure */
{
  DEBUG((stderr,"cb_amscatter_recv(): header_addr=%p  header_size=%zu dst_rank=%u cookie=%p\n",
         header_addr, header_size,((user_header_t *)header_addr)->dst_rank, cookie));
  if(gVerbose)
  {
    check_context(context);
  }

  pami_task_t     task;
  size_t          offset;
  _gRc |= PAMI_Endpoint_query (origin,
                              &task,
                              &offset);

  validation_t *v = _g_val_buffer + task;
  v->rbuf   = _g_recv_buffer;
  v->cookie = cookie;
  v->bytes  = data_size;
  v->root   = task;

  recv->cookie      = (void*)v;
  recv->local_fn    = cb_amscatter_done;
  recv->addr        = v->rbuf;
  recv->type        = PAMI_TYPE_BYTE;
  recv->offset      = 0;
  recv->data_fn     = PAMI_DATA_COPY;
  recv->data_cookie = NULL;
}

int main(int argc, char*argv[])
{
  pami_client_t        client;
  pami_context_t      *context;
  pami_geometry_t      world_geometry;
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

  /* Amscatter variables */
  size_t               amscatter_num_algorithm[2];
  pami_algorithm_t    *amscatter_always_works_algo = NULL;
  pami_metadata_t     *amscatter_always_works_md = NULL;
  pami_algorithm_t    *amscatter_must_query_algo = NULL;
  pami_metadata_t     *amscatter_must_query_md = NULL;
  pami_xfer_type_t     amscatter_xfer = PAMI_XFER_AMSCATTER;
  pami_xfer_t          amscatter;
  volatile unsigned    amscatter_total_count = 0;

  int                  nalg = 0, i;
  double               ti, tf, usec;

  /* Process environment variables and setup globals */
  setup_env();

  assert(gNum_contexts > 0);
  context = (pami_context_t*)malloc(sizeof(pami_context_t) * gNum_contexts);

  /* \note Test environment variable" TEST_ROOT=N, defaults to 0.*/
  char* sRoot = getenv("TEST_ROOT");
  /* Override ROOT */
  if (sRoot) root_task = (pami_task_t) atoi(sRoot);

  /*  Initialize PAMI */
  int rc = pami_init(&client,        /* Client             */
                     context,        /* Context            */
                     NULL,           /* Clientname=default */
                     &gNum_contexts, /* gNum_contexts      */
                     NULL,           /* null configuration */
                     0,              /* no configuration   */
                     &my_task_id,    /* task id            */
                     &num_tasks);    /* number of tasks    */

  if (rc == 1)
    return 1;

  if (gNumRoots > num_tasks) gNumRoots = num_tasks;

  /*  Allocate buffer(s) */
  int err = 0;
  void *sbuf = NULL;
  err = posix_memalign(&sbuf, 128, (gMax_byte_count * num_tasks) + gBuffer_offset);
  assert(err == 0);
  sbuf = (char*)sbuf + gBuffer_offset;

  void* rbuf = NULL;
  err = posix_memalign(&rbuf, 128, gMax_byte_count + gBuffer_offset);
  assert(err == 0);
  rbuf = (char*)rbuf + gBuffer_offset;

  void *headers = NULL;
  err = posix_memalign((void **)&headers, 128, (num_tasks * sizeof(user_header_t)) + gBuffer_offset);
  headers = (char*)headers + gBuffer_offset;

  void *validation = NULL;
  err = posix_memalign((void **)&validation, 128, (num_tasks * sizeof(validation_t)) + gBuffer_offset);
  validation = (char*)validation + gBuffer_offset;

  /* Initialize the headers */
  for(i = 0; i < num_tasks; ++i)
  {
    ((user_header_t *)headers)[i].dst_rank = i;
  }

  unsigned iContext = 0;
  for (; iContext < gNum_contexts; ++iContext)
  {
    if (my_task_id == 0)
      printf("# Context: %u\n", iContext);

    /*  Query the world geometry for barrier algorithms */
    rc |= query_geometry_world(client,
                               context[iContext],
                               &world_geometry,
                               barrier_xfer,
                               barrier_num_algorithm,
                               &bar_always_works_algo,
                               &bar_always_works_md,
                               &bar_must_query_algo,
                               &bar_must_query_md);

    if (rc == 1)
      return 1;

    /*  Query the world geometry for amscatter algorithms */
    rc |= query_geometry_world(client,
                               context[iContext],
                               &world_geometry,
                               amscatter_xfer,
                               amscatter_num_algorithm,
                               &amscatter_always_works_algo,
                               &amscatter_always_works_md,
                               &amscatter_must_query_algo,
                               &amscatter_must_query_md);

    if (rc == 1)
      return 1;

    _g_recv_buffer = rbuf;
    _g_send_buffer = sbuf;
    _g_val_buffer  = validation;


    barrier.cb_done     = cb_done;
    barrier.cookie      = (void*) & bar_poll_flag;
    barrier.algorithm   = bar_always_works_algo[0];
    blocking_coll(context[iContext], &barrier, &bar_poll_flag);


    amscatter.algorithm = amscatter_always_works_algo[0];
    amscatter.cmd.xfer_amscatter.headers      = headers;
    amscatter.cmd.xfer_amscatter.headerlen    = sizeof(user_header_t);
    amscatter.cmd.xfer_amscatter.sndbuf       = sbuf;
    amscatter.cmd.xfer_amscatter.stype        = PAMI_TYPE_BYTE;
    amscatter.cmd.xfer_amscatter.stypecount   = 0;

    for (nalg = 0; nalg < amscatter_num_algorithm[0]; nalg++)
    {
      gProtocolName = amscatter_always_works_md[nalg].name;

      if (my_task_id == root_task)
      {
        printf("# AMScatter Bandwidth Test(size:%zu) -- context = %d, root = %d, protocol: %s\n",num_tasks,
               iContext, root_task, amscatter_always_works_md[nalg].name);
        printf("# Size(bytes)      iterations     bytes/sec      usec\n");
        printf("# -----------      -----------    -----------    ---------\n");
        fflush(stdout);
      }

      if (((strstr(amscatter_always_works_md[nalg].name,gSelected) == NULL) && gSelector) ||
          ((strstr(amscatter_always_works_md[nalg].name,gSelected) != NULL) && !gSelector))  continue;

      int j;
      pami_collective_hint_t h = {0};
      pami_dispatch_callback_function fn;
      lgContext = context[iContext];
      fn.amscatter = cb_amscatter_recv;
      PAMI_AMCollective_dispatch_set(context[iContext],
                                     amscatter_always_works_algo[nalg],
                                     root_task,/* Set the dispatch id, can be any arbitrary value */
                                     fn,
                                     (void*) &amscatter_total_count,
                                     h);
      amscatter.cmd.xfer_amscatter.dispatch = root_task;
      amscatter.algorithm = amscatter_always_works_algo[nalg];

      volatile unsigned *nscatter = &amscatter_total_count;
      for (i = gMin_byte_count; i <= gMax_byte_count; i *= 2)
      {
        size_t  dataSent = i;
        int     niter;
        pami_result_t result;

        if (dataSent < CUTOFF)
          niter = gNiterlat;
        else
          niter = NITERBW;

        *nscatter = 0;
        memset(rbuf, 0xFF, i);
        scatter_initialize_sndbuf (sbuf, i, num_tasks);

        blocking_coll(context[iContext], &barrier, &bar_poll_flag);
        ti = timer();

        for (j = 0; j < niter; j++)
        {
          root_task = (root_task + num_tasks - 1) % num_tasks;
          if (my_task_id == root_task)
          {
            amscatter.cmd.xfer_amscatter.stypecount = i;
            result = PAMI_Collective(context[iContext], &amscatter);
            if (result != PAMI_SUCCESS)
              {
                fprintf (stderr, "Error. Unable to issue collective. result = %d\n", result);
                return 1;
              }
          }
          while (*nscatter <= j)
            result = PAMI_Context_advance (context[iContext], 1);

          rc |= _gRc; /* validation return code done in cb_amscatter_done */
        }

        assert(*nscatter == niter);
        tf = timer();
        blocking_coll(context[iContext], &barrier, &bar_poll_flag);

        usec = (tf - ti) / (double)niter;
        if(my_task_id == root_task)
        {
          printf("  %11lld %16d %14.1f %12.2f\n",
                 (long long)dataSent,
                 niter,
                 (double)1e6*(double)dataSent / (double)usec,
                 usec);
          fflush(stdout);
        }
      }
      lgContext = NULL;
    }
    free(bar_always_works_algo);
    free(bar_always_works_md);
    free(bar_must_query_algo);
    free(bar_must_query_md);
    free(amscatter_always_works_algo);
    free(amscatter_always_works_md);
    free(amscatter_must_query_algo);
    free(amscatter_must_query_md);
  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

  sbuf = (char*)sbuf - gBuffer_offset;
  free(sbuf);

  rbuf = (char*)rbuf - gBuffer_offset;
  free(rbuf);

  headers = (char*)headers - gBuffer_offset;
  free(headers);

  validation = (char*)validation - gBuffer_offset;
  free(validation);

  rc |= pami_shutdown(&client, context, &gNum_contexts);
  return rc;
}
