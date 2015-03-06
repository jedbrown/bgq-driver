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
 * \file test/api/collectives/bcast_subcomm_query.c
 * \brief Simple Bcast test on sub-geometries using "query" algorithms
 */

/* see setup_env() for environment variable overrides               */
#define COUNT     (1048576*8)   /* see envvar TEST_BYTES for overrides */

#include "../pami_util.h"

int main(int argc, char*argv[])
{
  pami_client_t        client;
  pami_context_t      *context;
  pami_task_t          task_id, local_task_id;
  size_t               num_tasks;
  pami_geometry_t      world_geometry;
  int                  nalg = 0;

  /* Barrier variables */
  size_t               barrier_num_algorithm[2];
  pami_algorithm_t    *bar_always_works_algo = NULL;
  pami_metadata_t     *bar_always_works_md   = NULL;
  pami_algorithm_t    *bar_must_query_algo   = NULL;
  pami_metadata_t     *bar_must_query_md     = NULL;
  pami_xfer_type_t     barrier_xfer = PAMI_XFER_BARRIER;
  pami_xfer_t          barrier;
  volatile unsigned    bar_poll_flag = 0;
  volatile unsigned    newbar_poll_flag = 0;

  /* Bcast variables */
  pami_xfer_type_t     bcast_xfer = PAMI_XFER_BROADCAST;
  volatile unsigned    bcast_poll_flag = 0;

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

  if (num_tasks == 1)
  {
    fprintf(stderr, "No subcomms on 1 node\n");
    return 0;
  }

  /*  Allocate buffer(s) */
  int err = 0;
  void* buf = NULL;
  err = posix_memalign(&buf, 128, gMax_byte_count + gBuffer_offset);
  assert(err == 0);
  buf = (char*)buf + gBuffer_offset;


  /*  Query the world geometry for barrier algorithms */
  rc |= query_geometry_world(client,
                             context[0],
                             &world_geometry,
                             barrier_xfer,
                             barrier_num_algorithm,
                             &bar_always_works_algo,
                             &bar_always_works_md,
                             &bar_must_query_algo,
                             &bar_must_query_md);

  if (rc == 1)
    return 1;

  /*  Set up world barrier */
  barrier.cb_done   = cb_done;
  barrier.cookie    = (void*) & bar_poll_flag;
  barrier.algorithm = bar_always_works_algo[0];

  unsigned iContext = 0;

  /*  Create the subgeometry */
  pami_geometry_range_t *range;
  int                    rangecount;
  pami_geometry_t        newgeometry;
  size_t                 newbar_num_algo[2];
  size_t                 newbcast_num_algo[2];
  pami_algorithm_t      *newbar_algo        = NULL;
  pami_metadata_t       *newbar_md          = NULL;
  pami_algorithm_t      *q_newbar_algo      = NULL;
  pami_metadata_t       *q_newbar_md        = NULL;

  pami_algorithm_t      *newbcast_algo      = NULL;
  pami_metadata_t       *newbcast_md        = NULL;
  pami_algorithm_t      *q_newbcast_algo    = NULL;
  pami_metadata_t       *q_newbcast_md      = NULL;
  pami_xfer_t            newbarrier;
  pami_xfer_t            newbcast;

  size_t                 set[2];
  int                    id;

  pami_task_t            root_zero;
  range     = (pami_geometry_range_t *)malloc(((num_tasks + 1) / 2) * sizeof(pami_geometry_range_t));

  int unused_non_root[2];
  get_split_method(&num_tasks, task_id, &rangecount, range, &local_task_id, set, &id, &root_zero, unused_non_root);

  for (; iContext < gNum_contexts; ++iContext)
  {

    if (task_id == root_zero)
      printf("# Context: %u\n", iContext);

    /* Delay root tasks, and emulate that he's doing "other"
       message passing.  This will cause the geometry_create
       request from other nodes to be unexpected when doing
       parentless geometries and won't affect parented.      */
    if (task_id == root_zero)
    {
      delayTest(1);
      unsigned ii = 0;

      for (; ii < gNum_contexts; ++ii)
        PAMI_Context_advance (context[ii], 1000);
    }

    rc |= create_and_query_geometry(client,
                                    context[0],
                                    context[iContext],
                                    gParentless ? PAMI_GEOMETRY_NULL : world_geometry,
                                    &newgeometry,
                                    range,
                                    rangecount,
                                    id + iContext, /* Unique id for each context */
                                    barrier_xfer,
                                    newbar_num_algo,
                                    &newbar_algo,
                                    &newbar_md,
                                    &q_newbar_algo,
                                    &q_newbar_md);

    if (rc == 1)
      return 1;

    /*  Query the sub geometry for bcast algorithms */
    rc |= query_geometry(client,
                         context[iContext],
                         newgeometry,
                         bcast_xfer,
                         newbcast_num_algo,
                         &newbcast_algo,
                         &newbcast_md,
                         &q_newbcast_algo,
                         &q_newbcast_md);

    if (rc == 1)
      return 1;

    /*  Set up sub geometry barrier */
    newbarrier.cb_done   = cb_done;
    newbarrier.cookie    = (void*) & newbar_poll_flag;
    newbarrier.algorithm = newbar_algo[0];


    for (nalg = 0; nalg < newbcast_num_algo[1]; nalg++)
    {
      metadata_result_t result = {0};
      pami_endpoint_t    root_ep;
      PAMI_Endpoint_create(client, root_zero, 0, &root_ep);
      newbcast.cmd.xfer_broadcast.root = root_ep;


      /*  Set up sub geometry bcast */
      newbcast.cb_done                      = cb_done;
      newbcast.cookie                       = (void*) & bcast_poll_flag;
      newbcast.algorithm                    = q_newbcast_algo[nalg];
      newbcast.cmd.xfer_broadcast.buf       = buf;
      newbcast.cmd.xfer_broadcast.type      = PAMI_TYPE_BYTE;


      int             k;

      gProtocolName = q_newbcast_md[nalg].name;

      for (k = 1; k >= 0; k--)
      {
        if (set[k])
        {
          if (task_id == root_zero)
          {
            printf("# Broadcast Bandwidth Test(size:%zu) -- context = %d, root = %d  protocol: %s, Metadata: range %zu <-> %zd, mask %#X\n",num_tasks,
                   iContext, root_zero, gProtocolName,
                   q_newbcast_md[nalg].range_lo, q_newbcast_md[nalg].range_hi,
                   q_newbcast_md[nalg].check_correct.bitmask_correct);
            printf("# Size(bytes)      iterations     bytes/sec      usec\n");
            printf("# -----------      -----------    -----------    ---------\n");
          }

          if (((strstr(q_newbcast_md[nalg].name, gSelected) == NULL) && gSelector) ||
              ((strstr(q_newbcast_md[nalg].name, gSelected) != NULL) && !gSelector))  continue;

          unsigned checkrequired = q_newbcast_md[nalg].check_correct.values.checkrequired; /*must query every time */
          assert(!checkrequired || q_newbcast_md[nalg].check_fn); /* must have function if checkrequired. */

          blocking_coll(context[iContext], &newbarrier, &newbar_poll_flag);

          int i, j;

          for (i = gMin_byte_count; i <= gMax_byte_count; i *= 2)
          {
            size_t  dataSent = i;
            int          niter;

            if (dataSent < CUTOFF)
              niter = gNiterlat;
            else
              niter = NITERBW;

            newbcast.cmd.xfer_broadcast.typecount = i;

            result = check_metadata(q_newbcast_md[nalg],
                                    newbcast,
                                    PAMI_TYPE_BYTE,
                                    dataSent, /* metadata uses bytes i, */
                                    newbcast.cmd.xfer_broadcast.buf,
                                    PAMI_TYPE_BYTE,
                                    dataSent,
                                    newbcast.cmd.xfer_broadcast.buf);


            if (q_newbcast_md[nalg].check_correct.values.nonlocal)
            {
              /* \note We currently ignore check_correct.values.nonlocal
                 because these tests should not have nonlocal differences (so far). */
              result.check.nonlocal = 0;
            }

            if (result.bitmask) continue;

            if (task_id == root_zero)
              bcast_initialize_sndbuf (buf, i, root_zero);
            else
              memset(buf, 0xFF, i);

            blocking_coll(context[iContext], &newbarrier, &newbar_poll_flag);
            ti = timer();

            for (j = 0; j < niter; j++)
            {
              if (checkrequired) /* must query every time */
              {
                result = q_newbcast_md[nalg].check_fn(&newbcast);

                if (result.bitmask) continue;
              }

              blocking_coll(context[iContext], &newbcast, &bcast_poll_flag);
            }

            tf = timer();
            blocking_coll(context[iContext], &newbarrier, &newbar_poll_flag);
            int rc_check;
            rc |= rc_check = bcast_check_rcvbuf (buf, i, root_zero);

            if (rc_check) fprintf(stderr, "%s FAILED validation\n", gProtocolName);

            usec = (tf - ti) / (double)niter;

            if (task_id == root_zero)
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

        blocking_coll(context[iContext], &newbarrier, &newbar_poll_flag);
        fflush(stderr);
      }
    }

    /* We aren't testing world barrier itself, so use context 0.*/
    blocking_coll(context[0], &barrier, &bar_poll_flag);

    free(bar_always_works_algo);
    free(bar_always_works_md);
    free(bar_must_query_algo);
    free(bar_must_query_md);

    free(newbar_algo);
    free(newbar_md);
    free(q_newbar_algo);
    free(q_newbar_md);

    free(newbcast_algo);
    free(newbcast_md);
    free(q_newbcast_algo);
    free(q_newbcast_md);

  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

  buf = (char*)buf - gBuffer_offset;
  free(buf);

  rc |= pami_shutdown(&client, context, &gNum_contexts);
  return rc;
}
