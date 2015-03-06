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
 * \file test/api/collectives/bcast_query.c
 * \brief Simple Bcast test on world geometry using "query" algorithms
 */

/* see setup_env() for environment variable overrides               */
#define COUNT     (1048576*8)

#include "../pami_util.h"

int main(int argc, char*argv[])
{
  pami_client_t        client;
  pami_context_t      *context;
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
  volatile unsigned    bar_poll_flag = 0;

  /* Bcast variables */
  size_t               bcast_num_algorithm[2];
  pami_algorithm_t    *bcast_always_works_algo = NULL;
  pami_metadata_t     *bcast_always_works_md = NULL;
  pami_algorithm_t    *bcast_must_query_algo = NULL;
  pami_metadata_t     *bcast_must_query_md = NULL;
  pami_xfer_type_t     bcast_xfer = PAMI_XFER_BROADCAST;
  volatile unsigned    bcast_poll_flag = 0;

  int                  nalg= 0;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          broadcast;

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

  if (rc != PAMI_SUCCESS)
    return 1;
  int o;
  for(o = -1; o <= gOptimize ; o++) /* -1 = default, 0 = de-optimize, 1 = optimize */
  {

    pami_configuration_t configuration[1];
    configuration[0].name = PAMI_GEOMETRY_OPTIMIZE;
    configuration[0].value.intval = o; /* de/optimize */
    if(o == -1) ; /* skip update, use defaults */
    else
      rc |= update_geometry(client,
                            context[0],
                            world_geometry,
                            configuration,
                            1);

    if (rc != PAMI_SUCCESS)
    return 1;

  if(gNumRoots > num_tasks) gNumRoots = num_tasks;

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

    if (rc != PAMI_SUCCESS)
      return 1;

    /*  Query the world geometry for broadcast algorithms */
    rc |= query_geometry_world(client,
                               context[iContext],
                               &world_geometry,
                               bcast_xfer,
                               bcast_num_algorithm,
                               &bcast_always_works_algo,
                               &bcast_always_works_md,
                               &bcast_must_query_algo,
                               &bcast_must_query_md);

      if (rc != PAMI_SUCCESS)
      return 1;

    barrier.cb_done   = cb_done;
    barrier.cookie    = (void*) & bar_poll_flag;
    barrier.algorithm = bar_always_works_algo[0];
    blocking_coll(context[iContext], &barrier, &bar_poll_flag);

    for (nalg = 0; nalg < bcast_num_algorithm[1]; nalg++)
    {
      broadcast.cb_done                      = cb_done;
      broadcast.cookie                       = (void*) & bcast_poll_flag;
      broadcast.algorithm                    = bcast_must_query_algo[nalg];
      broadcast.cmd.xfer_broadcast.buf       = buf;
      broadcast.cmd.xfer_broadcast.type      = PAMI_TYPE_BYTE;
      broadcast.cmd.xfer_broadcast.typecount = 0;

      gProtocolName = bcast_must_query_md[nalg].name;

      metadata_result_t result = {0};

      int k;
      for (k=0; k< gNumRoots; k++)
      {
      pami_endpoint_t   root_ep;
        pami_task_t root_task = (pami_task_t)k;
      PAMI_Endpoint_create(client, root_task, 0, &root_ep);
      broadcast.cmd.xfer_broadcast.root = root_ep;

      if (task_id == root_task)
      {
        printf("# Broadcast Bandwidth Test(size:%zu) -- context = %d, optimize = %d, root = %d  protocol: %s, Metadata: range %zu <-> %zd, mask %#X\n",num_tasks,
               iContext, o, root_task, gProtocolName,
               bcast_must_query_md[nalg].range_lo, bcast_must_query_md[nalg].range_hi,
               bcast_must_query_md[nalg].check_correct.bitmask_correct);
        printf("# Size(bytes)      iterations     bytes/sec      usec\n");
        printf("# -----------      -----------    -----------    ---------\n");
      }

      if (((strstr(bcast_must_query_md[nalg].name,gSelected) == NULL) && gSelector) ||
          ((strstr(bcast_must_query_md[nalg].name,gSelected) != NULL) && !gSelector))  continue;

      unsigned checkrequired = bcast_must_query_md[nalg].check_correct.values.checkrequired; /*must query every time */
      assert(!checkrequired || bcast_must_query_md[nalg].check_fn); /* must have function if checkrequired. */

      int i, j;

      for (i = gMin_byte_count; i <= gMax_byte_count; i *= 2)
      {
        size_t  dataSent = i;
        int          niter;

        if (dataSent < CUTOFF)
          niter = gNiterlat;
        else
          niter = NITERBW;

        broadcast.cmd.xfer_broadcast.typecount = i;

        result = check_metadata(bcast_must_query_md[nalg],
                                broadcast,
                                PAMI_TYPE_BYTE,
                                dataSent, /* metadata uses bytes i, */
                                broadcast.cmd.xfer_broadcast.buf,
                                PAMI_TYPE_BYTE,
                                dataSent,
                                broadcast.cmd.xfer_broadcast.buf);


        if (bcast_must_query_md[nalg].check_correct.values.nonlocal)
        {
          /* \note We currently ignore check_correct.values.nonlocal
                   because these tests should not have nonlocal differences (so far). */
          result.check.nonlocal = 0;
        }

        if (result.bitmask) continue;

        if (task_id == root_task)
          bcast_initialize_sndbuf (buf, i, root_task);
        else
          memset(buf, 0xFF, i);
        blocking_coll(context[iContext], &barrier, &bar_poll_flag);
        ti = timer();

        for (j = 0; j < niter; j++)
        {
          if (checkrequired) /* must query every time */
          {
            result = bcast_must_query_md[nalg].check_fn(&broadcast);
            if (result.bitmask) continue;
          }

          blocking_coll (context[iContext], &broadcast, &bcast_poll_flag);
        }

        blocking_coll(context[iContext], &barrier, &bar_poll_flag);
        tf = timer();
        int rc_check;
        rc |= rc_check = bcast_check_rcvbuf (buf, i, root_task);

        if (rc_check) fprintf(stderr, "%s FAILED validation\n", gProtocolName);

        usec = (tf - ti) / (double)niter;

        if (task_id == root_task)
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

    free(bar_always_works_algo);
    free(bar_always_works_md);
    free(bar_must_query_algo);
    free(bar_must_query_md);
    free(bcast_always_works_algo);
    free(bcast_always_works_md);
    free(bcast_must_query_algo);
    free(bcast_must_query_md);

  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/
  buf = (char*)buf - gBuffer_offset;
  free(buf);
  } /* optimize loop */

  rc |= pami_shutdown(&client, context, &gNum_contexts);
  return rc;
}
