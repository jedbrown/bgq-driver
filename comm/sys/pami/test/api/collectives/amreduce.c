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
 * \file test/api/collectives/amreduce.c
 * \brief Simple AMReduce test on world geometry
 */

/* see setup_env() for environment variable overrides               */
#define COUNT      65536
#define NITERLAT   10

#include "../pami_util.h"

#define AMDEBUG 0

#if AMDEBUG == 1
#define DEBUG(x) fprintf x
#else
#define DEBUG(x)
#endif

typedef struct
{
  void *cookie;
  int   bytes;
  int   root;
  int   op;
  int   dt;
} validation_t;

typedef struct
{
  int op;
  int dt;
} user_header_t;

pami_context_t          lgContext = NULL;
pami_task_t             my_task_id;
size_t                  num_tasks;

char                   *_g_recv_buffer;
char                   *_g_send_buffer;
validation_t           *_g_val_buffer;
int                     _gRc = PAMI_SUCCESS;

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
void cb_amreduce_done (void *context, void * clientdata, pami_result_t err)
{
  validation_t *v = (validation_t*)clientdata;
  volatile unsigned *active = (volatile unsigned *) v->cookie;
  DEBUG((stderr, "cb_amreduce_done(): cookie= %p value=%u\n", active, *active));
  if(gVerbose)
  {
    check_context((pami_context_t)context);
  }

  if(my_task_id == v->root)
  {
    int rc_check;
    size_t type_sz = get_type_size(dt_array[v->dt]);
    _gRc |= rc_check = reduce_check_rcvbuf (_g_recv_buffer, v->bytes/type_sz, v->op, v->dt, my_task_id, num_tasks);
    if (rc_check) fprintf(stderr, "%s FAILED validation\n", gProtocolName);
  }
  (*active)++;
}
/**
 *  User dispatch function
 */
void cb_amreduce_send(pami_context_t         context,      /**< IN:  communication context which invoked the dispatch function */
                      void                 * cookie,       /**< IN:  dispatch cookie */
                      const void           * header_addr,  /**< IN:  header address  */
                      size_t                 header_size,  /**< IN:  header size     */
                      size_t                 data_size,    /**< IN:  data size       */
                      pami_endpoint_t        origin,       /**< IN:  root initiating endpoint */
                      pami_geometry_t        geometry,     /**< IN:  Geometry */
                      pami_data_function   * op,           /**< OUT: PAMI math operation to perform on the datatype */
                      pami_recv_t          * send)         /**< OUT: receive message structure */
{
  user_header_t *hdr;
  hdr = (user_header_t *) header_addr;
  DEBUG((stderr,"cb_amreduce_send(): header_addr=%p  header_size=%zu op=%d dt=%d cookie=%p\n",
         header_addr, header_size, hdr->op, hdr->dt, cookie));
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
  v->cookie = cookie;
  v->bytes  = data_size;
  v->op     = hdr->op;
  v->dt     = hdr->dt;
  v->root = task;

  send->cookie      = (void*)v;
  send->local_fn    = cb_amreduce_done;
  send->addr        = _g_send_buffer;
  send->type        = dt_array[hdr->dt];
  send->offset      = 0;
  send->data_fn     = PAMI_DATA_COPY;
  send->data_cookie = NULL;
  *op = op_array[hdr->op];
}

int main(int argc, char*argv[])
{
  pami_client_t        client;
  pami_context_t      *context;
  pami_result_t        result = PAMI_ERROR;
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

  /* Amreduce variables */
  size_t               amreduce_num_algorithm[2];
  pami_algorithm_t    *amreduce_always_works_algo = NULL;
  pami_metadata_t     *amreduce_always_works_md = NULL;
  pami_algorithm_t    *amreduce_must_query_algo = NULL;
  pami_metadata_t     *amreduce_must_query_md = NULL;
  pami_xfer_type_t     amreduce_xfer = PAMI_XFER_AMREDUCE;
  pami_xfer_t          amreduce;
  volatile unsigned    amreduce_total_count = 0;

  int                  nalg = 0, i;
  double               ti, tf, usec;

  /* Process environment variables and setup globals */
  setup_env();

  assert(gNum_contexts > 0);
  context = (pami_context_t*)malloc(sizeof(pami_context_t) * gNum_contexts);

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
  err = posix_memalign(&sbuf, 128, gMax_byte_count + gBuffer_offset);
  assert(err == 0);
  sbuf = (char*)sbuf + gBuffer_offset;

  void* rbuf = NULL;
  err = posix_memalign(&rbuf, 128, gMax_byte_count + gBuffer_offset);
  assert(err == 0);
  rbuf = (char*)rbuf + gBuffer_offset;

  void *validation = NULL;
  err = posix_memalign((void **)&validation, 128, (num_tasks * sizeof(validation_t)) + gBuffer_offset);
  validation = (char*)validation + gBuffer_offset;

  user_header_t user_header;

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

    /*  Query the world geometry for amreduce algorithms */
    rc |= query_geometry_world(client,
                               context[iContext],
                               &world_geometry,
                               amreduce_xfer,
                               amreduce_num_algorithm,
                               &amreduce_always_works_algo,
                               &amreduce_always_works_md,
                               &amreduce_must_query_algo,
                               &amreduce_must_query_md);

    if (rc == 1)
      return 1;

    _g_recv_buffer = rbuf;
    _g_send_buffer = sbuf;
    _g_val_buffer  = validation;


    barrier.cb_done     = cb_done;
    barrier.cookie      = (void*) & bar_poll_flag;
    barrier.algorithm   = bar_always_works_algo[0];
    blocking_coll(context[iContext], &barrier, &bar_poll_flag);


    amreduce.algorithm = amreduce_always_works_algo[0];
    amreduce.cmd.xfer_amreduce.user_header  = &user_header;
    amreduce.cmd.xfer_amreduce.headerlen    = sizeof(user_header_t);
    amreduce.cmd.xfer_amreduce.rcvbuf       = rbuf;
    amreduce.cmd.xfer_amreduce.rtype        = PAMI_TYPE_BYTE;
    amreduce.cmd.xfer_amreduce.rtypecount   = 0;

    for (nalg = 0; nalg < amreduce_num_algorithm[0]; nalg++)
    {
      gProtocolName = amreduce_always_works_md[nalg].name;

      if (my_task_id == root_task)
      {
        printf("# AMReduce Bandwidth Test(size:%zu) -- context = %d, root = %d, protocol: %s\n",num_tasks,
               iContext, root_task, amreduce_always_works_md[nalg].name);
        printf("# Size(bytes)      iterations     bytes/sec      usec\n");
        printf("# -----------      -----------    -----------    ---------\n");
        fflush(stdout);
      }

      if (((strstr(amreduce_always_works_md[nalg].name,gSelected) == NULL) && gSelector) ||
          ((strstr(amreduce_always_works_md[nalg].name,gSelected) != NULL) && !gSelector))  continue;

      int j;
      pami_collective_hint_t h = {0};
      pami_dispatch_callback_function fn;
      lgContext = context[iContext];
      fn.amreduce = cb_amreduce_send;
      PAMI_AMCollective_dispatch_set(context[iContext],
                                     amreduce_always_works_algo[nalg],
                                     root_task,/* Set the dispatch id, can be any arbitrary value */
                                     fn,
                                     (void*) &amreduce_total_count,
                                     h);
      amreduce.cmd.xfer_amreduce.dispatch = root_task;
      amreduce.algorithm = amreduce_always_works_algo[nalg];

      volatile unsigned *nreduce = &amreduce_total_count;
      int op, dt;

      for (dt = 0; dt < dt_count; dt++)
      {
        for (op = 0; op < op_count; op++)
        {
          if (gValidTable[op][dt])
          {
            if (my_task_id == root_task)
              printf("Running Reduce: %s, %s\n", dt_array_str[dt], op_array_str[op]);


            for (i = MAX(1,gMin_byte_count/get_type_size(dt_array[dt])); i <= gMax_byte_count/get_type_size(dt_array[dt]); i *= 2)
            {
              size_t sz = get_type_size(dt_array[dt]);
              size_t dataSent = i * sz;
              int    niter;

              if (dataSent < CUTOFF)
                niter = gNiterlat;
              else
                niter = NITERBW;

              amreduce.cmd.xfer_amreduce.rtypecount = dataSent;
              user_header.op = op;
              user_header.dt = dt;

              *nreduce = 0;
              reduce_initialize_sndbuf (sbuf, i , op, dt, my_task_id, num_tasks);
              memset(rbuf, 0xFF, dataSent);

              blocking_coll(context[iContext], &barrier, &bar_poll_flag);
              ti = timer();

              for (j = 0; j < niter; j++)
              {
                root_task = (root_task + num_tasks - 1) % num_tasks;
                if (my_task_id == root_task)
                {
                  result = PAMI_Collective(context[iContext], &amreduce);
                  if (result != PAMI_SUCCESS)
                    {
                      fprintf (stderr, "Error. Unable to issue  collective. result = %d\n", result);
                      return 1;
                    }
                }
                while (*nreduce <= j)
                  result = PAMI_Context_advance (context[iContext], 1);

                rc |= _gRc; /* validation return code done in cb_amreduce_done */
              }

              assert(*nreduce == niter);
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
          }
        }
      }
      lgContext = NULL;
    }
    free(bar_always_works_algo);
    free(bar_always_works_md);
    free(bar_must_query_algo);
    free(bar_must_query_md);
    free(amreduce_always_works_algo);
    free(amreduce_always_works_md);
    free(amreduce_must_query_algo);
    free(amreduce_must_query_md);
  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

  sbuf = (char*)sbuf - gBuffer_offset;
  free(sbuf);

  rbuf = (char*)rbuf - gBuffer_offset;
  free(rbuf);

  rc |= pami_shutdown(&client, context, &gNum_contexts);
  return rc;
}
