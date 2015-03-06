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
 * \file test/api/collectives/ambcast_contig.c
 * \brief Simple AMBcast test on world geometry with contiguous datatypes
 */

/* see setup_env() for environment variable overrides               */
#define COUNT      262144

#include "../pami_util.h"

volatile unsigned       _g_total_broadcasts;
char                   *_g_recv_buffer;
int                     _gRc = PAMI_SUCCESS;
typedef struct
{
  void *rbuf;
  int count;
  int root;
  int dt;
  char buffer;
} validation_t;

void cb_ambcast_done (void *context, void * clientdata, pami_result_t err)
{
  _g_total_broadcasts++;
  int rc_check;
  validation_t *v = (validation_t*)clientdata;

  _gRc |= rc_check = bcast_check_rcvbuf_dt (v->rbuf, v->count, v->root, v->dt);
  if (rc_check) fprintf(stderr, "%s FAILED validation on %s\n", gProtocolName, dt_array_str[v->dt]);

  free(clientdata);
}

void cb_ambcast_recv  (pami_context_t         context,      /**< IN:  communication context which invoked the dispatch function */
                       void                 * cookie,       /**< IN:  dispatch cookie */
                       const void           * header_addr,  /**< IN:  header address  */
                       size_t                 header_size,  /**< IN:  header size     */
                       const void           * pipe_addr,    /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
                       size_t                 data_size,    /**< IN:  number of bytes of message data */
                       pami_endpoint_t        origin,       /**< IN:  root initiating endpoint */
                       pami_geometry_t        geometry,     /**< IN:  Geometry */
                       pami_recv_t          * recv)         /**< OUT: receive message structure, only needed if addr is non-NULL */
{
  if (gVerbose && !context)
    fprintf(stderr, "Error. Null context received on cb_done.\n");

  validation_t *v =  (validation_t *)malloc(data_size+sizeof(validation_t));

  void* rcvbuf = v->rbuf = &v->buffer;
  v->dt    = *((int *)header_addr);
  v->count = data_size/get_type_size(dt_array[v->dt]);
  pami_task_t     task;
  size_t          offset;
  _gRc |= PAMI_Endpoint_query (origin,
                              &task,
                              &offset);

  v->root = task;

  if (!recv)
  {
    memcpy(rcvbuf, pipe_addr, data_size);
    return;
  }

  recv->cookie      = (void*)v;
  recv->local_fn    = cb_ambcast_done;
  recv->addr        = rcvbuf;
  recv->type        = PAMI_TYPE_BYTE;
  recv->offset      = 0;
  recv->data_fn     = PAMI_DATA_COPY;
  recv->data_cookie = NULL;
}

int main(int argc, char*argv[])
{
  pami_client_t        client;
  pami_context_t      *context;
  pami_result_t        result = PAMI_ERROR;
  pami_task_t          task_id;
  size_t               num_tasks;
  pami_geometry_t      world_geometry;

  /* Barrier variables */
  size_t               barrier_num_algorithm[2];
  pami_algorithm_t    *bar_always_works_algo = NULL;
  pami_metadata_t     *bar_always_works_md   = NULL;
  pami_algorithm_t    *bar_must_query_algo   = NULL;
  pami_metadata_t     *bar_must_query_md     = NULL;
  pami_xfer_type_t     barrier_xfer = PAMI_XFER_BARRIER;
  pami_xfer_t          barrier;
  volatile unsigned    bar_poll_flag = 0;

  /* Ambcast variables */
  pami_algorithm_t    *next_algo = NULL;
  pami_metadata_t     *next_md= NULL;
  size_t               ambcast_num_algorithm[2];
  pami_algorithm_t    *ambcast_always_works_algo = NULL;
  pami_metadata_t     *ambcast_always_works_md = NULL;
  pami_algorithm_t    *ambcast_must_query_algo = NULL;
  pami_metadata_t     *ambcast_must_query_md = NULL;
  pami_xfer_type_t     ambcast_xfer = PAMI_XFER_AMBROADCAST;
  pami_xfer_t          ambroadcast;
  volatile unsigned    ambcast_poll_flag = 0;

  int                  nalg= 0, total_alg;
  double               ti, tf, usec;

  /* Process environment variables and setup globals */
  setup_env();

  assert(gNum_contexts > 0);
  context = (pami_context_t*)malloc(sizeof(pami_context_t) * gNum_contexts);

  /*  Initialize PAMI */
  int rc = pami_init(&client,        /* Client             */
                     context,        /* Context            */
                     NULL,           /* Clientname=default */
                     &gNum_contexts, /* gNum_contexts       */
                     NULL,           /* null configuration */
                     0,              /* no configuration   */
                     &task_id,       /* task id            */
                     &num_tasks);    /* number of tasks    */

  if (rc == 1)
    return 1;

  if (gNumRoots > num_tasks) gNumRoots = num_tasks;

  /*  Allocate buffer(s) */
  int err = 0;
  void* buf = NULL;
  err = posix_memalign(&buf, 128, gMax_byte_count + gBuffer_offset);
  assert(err == 0);
  buf = (char*)buf + gBuffer_offset;


  unsigned iContext = 0;

  for (; iContext < gNum_contexts; ++iContext)
  {

    if (task_id == 0)
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

    /*  Query the world geometry for ambroadcast algorithms */
    rc |= query_geometry_world(client,
                               context[iContext],
                               &world_geometry,
                               ambcast_xfer,
                               ambcast_num_algorithm,
                               &ambcast_always_works_algo,
                               &ambcast_always_works_md,
                               &ambcast_must_query_algo,
                               &ambcast_must_query_md);

    if (rc == 1)
      return 1;

    _g_recv_buffer = (char *)buf;


    barrier.cb_done     = cb_done;
    barrier.cookie      = (void*) & bar_poll_flag;
    barrier.algorithm   = bar_always_works_algo[0];
    blocking_coll(context[iContext], &barrier, &bar_poll_flag);


    ambroadcast.cb_done   = cb_done;
    ambroadcast.cookie    = (void*) & ambcast_poll_flag;
    ambroadcast.cmd.xfer_ambroadcast.user_header  = NULL;
    ambroadcast.cmd.xfer_ambroadcast.headerlen    = 0;
    ambroadcast.cmd.xfer_ambroadcast.sndbuf       = buf;
    ambroadcast.cmd.xfer_ambroadcast.stype        = PAMI_TYPE_BYTE;
    ambroadcast.cmd.xfer_ambroadcast.stypecount   = 0;

    total_alg = ambcast_num_algorithm[0]+ambcast_num_algorithm[1];
    for (nalg = 0; nalg < total_alg; nalg++)
    {
      metadata_result_t mresult = {0};
      unsigned query_protocol;
      if(nalg < ambcast_num_algorithm[0])
      {  
        query_protocol = 0;
        next_algo = &ambcast_always_works_algo[nalg];
        next_md  = &ambcast_always_works_md[nalg];
      }
      else
      {  
        query_protocol = 1;
        next_algo = &ambcast_must_query_algo[nalg-ambcast_num_algorithm[0]];
        next_md  = &ambcast_must_query_md[nalg-ambcast_num_algorithm[0]];
      }

      gProtocolName = next_md->name;

      int k;
      for (k=0; k< gNumRoots; k++)
      {
        pami_task_t root_task = (pami_task_t)k;
        if (task_id == root_task)
        {
          printf("# AMBroadcast Bandwidth Test(size:%zu) -- context = %d, root = %d  protocol: %s, Metadata: range %zu <-> %zd, mask %#X\n",num_tasks,
                 iContext, root_task, gProtocolName,
                 next_md->range_lo,(ssize_t)next_md->range_hi,
                 next_md->check_correct.bitmask_correct);
          printf("# Size(bytes)      iterations     bytes/sec      usec\n");
          printf("# -----------      -----------    -----------    ---------\n");
        }

        if (((strstr(next_md->name, gSelected) == NULL) && gSelector) ||
            ((strstr(next_md->name, gSelected) != NULL) && !gSelector))  continue;

        unsigned checkrequired = next_md->check_correct.values.checkrequired; /*must query every time */
        assert(!checkrequired || next_md->check_fn); /* must have function if checkrequired. */

        int i, j;
        pami_collective_hint_t h = {0};
        pami_dispatch_callback_function fn;
        fn.ambroadcast = cb_ambcast_recv;
        PAMI_AMCollective_dispatch_set(context[iContext],
                                       *next_algo,
                                       k,
                                       fn,
                                       NULL,
                                       h);
        ambroadcast.algorithm = *next_algo;
        ambroadcast.cmd.xfer_ambroadcast.dispatch = k;
        memset(buf, 0xFF, gMax_byte_count);
        blocking_coll(context[iContext], &barrier, &bar_poll_flag);

        int dt,op=4/*SUM*/;

        for (dt = 0; dt < dt_count; dt++)
        {
          if ((gFull_test && ((dt != DT_NULL) && (dt != DT_BYTE))) || gValidTable[op][dt])
          {
              if (task_id == 0)
                printf("Running Broadcast: %s\n", dt_array_str[dt]);
              
              ambroadcast.cmd.xfer_ambroadcast.stype = dt_array[dt];
              ambroadcast.cmd.xfer_ambroadcast.user_header = &dt;
              ambroadcast.cmd.xfer_ambroadcast.headerlen   = sizeof(int);

              for (i = MAX(1,gMin_byte_count/get_type_size(dt_array[dt])); i <= gMax_byte_count/get_type_size(dt_array[dt]); i *= 2)
              {
                size_t dataSent = i * get_type_size(dt_array[dt]);
                int          niter;

                if (dataSent < CUTOFF)
                  niter = gNiterlat;
                else
                  niter = NITERBW;

                ambroadcast.cmd.xfer_ambroadcast.stypecount = i;

                if(query_protocol)
                {  
                  size_t sz=get_type_size(dt_array[dt])*i;
                /* Must initialize all of cmd for metadata */
                  mresult = check_metadata(*next_md,
                                          ambroadcast,
                                          dt_array[dt],
                                          sz, /* metadata uses bytes i, */
                                          ambroadcast.cmd.xfer_ambroadcast.sndbuf,
                                          PAMI_TYPE_BYTE,
                                          sz,
                                          ambroadcast.cmd.xfer_ambroadcast.sndbuf);
                  if (next_md->check_correct.values.nonlocal)
                  {
                    /* \note We currently ignore check_correct.values.nonlocal
                      because these tests should not have nonlocal differences (so far). */
                    mresult.check.nonlocal = 0;
                  }

                  if (mresult.bitmask) continue;
                }

                if (task_id == root_task)
                {
                  ambroadcast.cmd.xfer_ambroadcast.stypecount = i;
                  bcast_initialize_sndbuf_dt (buf, i, root_task, dt);
                  ti = timer();

                  for (j = 0; j < niter; j++)
                  {
                    if (checkrequired) /* must query every time */
                    {
                      mresult = next_md->check_fn(&ambroadcast);
                      /* \todo  This is a problem since destinations will expect niter bcasts...
                        if we had an example protocol that had checkrequired... */
                      if (mresult.bitmask) continue;
                    }
                    blocking_coll (context[iContext], &ambroadcast, &ambcast_poll_flag);
                  }

                  while (ambcast_poll_flag)
                    result = PAMI_Context_advance (context[iContext], 1);

                  blocking_coll(context[iContext], &barrier, &bar_poll_flag);
                  tf = timer();
                  usec = (tf - ti) / (double)niter;
                  printf("  %11lld %16lld %14.1f %12.2f\n",
                   (long long)dataSent,
                   0LL,
                   (double)1e6*(double)dataSent / (double)usec,
                   usec);
                  fflush(stdout);
                }
                else
                {
                  while (_g_total_broadcasts < niter)
                    result = PAMI_Context_advance (context[iContext], 1);

                  rc |= _gRc; /* validation return code done in cb_ambcast_done */

                  _g_total_broadcasts = 0;
                  blocking_coll(context[iContext], &barrier, &bar_poll_flag);
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
    free(ambcast_always_works_algo);
    free(ambcast_always_works_md);
    free(ambcast_must_query_algo);
  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

  buf = (char*)buf - gBuffer_offset;
  free(buf);

  rc |= pami_shutdown(&client, context, &gNum_contexts);
  return rc;
}
