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
 * \file test/api/collectives/gather_subcomm.c
 * \brief Simple Gather test on sub-geometries (only gathers bytes)
 */

/* see setup_env() for environment variable overrides               */
#define COUNT     (524288/16)

#include "../pami_util.h"

int main(int argc, char*argv[])
{
  pami_client_t        client;
  pami_context_t      *context;
  pami_task_t          task_id, local_task_id=0, root_zero=0;
  size_t               num_tasks, subgeometry_num_tasks;
  pami_geometry_t      world_geometry;

  /* Barrier variables */
  size_t               barrier_num_algorithm[2];
  pami_algorithm_t    *bar_always_works_algo = NULL;
  pami_metadata_t     *bar_always_works_md   = NULL;
  pami_algorithm_t    *bar_must_query_algo   = NULL;
  pami_metadata_t     *bar_must_query_md     = NULL;
  pami_xfer_type_t     barrier_xfer = PAMI_XFER_BARRIER;
  volatile unsigned    bar_poll_flag = 0;
  volatile unsigned    newbar_poll_flag = 0;

  /* Gather variables */
  size_t               gather_num_algorithm[2];
  pami_algorithm_t    *gather_always_works_algo = NULL;
  pami_metadata_t     *gather_always_works_md = NULL;
  pami_algorithm_t    *gather_must_query_algo = NULL;
  pami_metadata_t     *gather_must_query_md = NULL;
  pami_xfer_type_t     gather_xfer = PAMI_XFER_GATHER;
  volatile unsigned    gather_poll_flag = 0;

  int                  nalg = 0;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          gather;

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
  subgeometry_num_tasks = num_tasks;
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
  err = posix_memalign(&buf, 128, (gMax_byte_count) + gBuffer_offset);
  assert(err == 0);
  buf = (char*)buf + gBuffer_offset;

  void* rbuf = NULL;
  err = posix_memalign(&rbuf, 128, (gMax_byte_count * subgeometry_num_tasks) + gBuffer_offset);
  assert(err == 0);
  rbuf = (char*)rbuf + gBuffer_offset;


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
  pami_algorithm_t      *newbar_algo        = NULL;
  pami_metadata_t       *newbar_md          = NULL;
  pami_algorithm_t      *q_newbar_algo      = NULL;
  pami_metadata_t       *q_newbar_md        = NULL;

  pami_xfer_t            newbarrier;

  size_t                 set[2];
  int                    id;


  range     = (pami_geometry_range_t *)malloc(((num_tasks + 1) / 2) * sizeof(pami_geometry_range_t));

  int unused_non_root[2];
  get_split_method(&subgeometry_num_tasks, task_id, &rangecount, range, &local_task_id, set, &id, &root_zero,unused_non_root);

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

    /*  Query the sub geometry for Gather algorithms */
    rc |= query_geometry(client,
                         context[iContext],
                         newgeometry,
                         gather_xfer,
                         gather_num_algorithm,
                         &gather_always_works_algo,
                         &gather_always_works_md,
                         &gather_must_query_algo,
                         &gather_must_query_md);

    if (rc == 1)
      return 1;

    /*  Set up sub geometry barrier */
    newbarrier.cb_done   = cb_done;
    newbarrier.cookie    = (void*) & newbar_poll_flag;
    newbarrier.algorithm = newbar_algo[0];

    for (nalg = 0; nalg < gather_num_algorithm[0]; nalg++)
    {
      gather.cb_done    = cb_done;
      gather.cookie     = (void*) & gather_poll_flag;
      gather.algorithm  = gather_always_works_algo[nalg];
      gather.cmd.xfer_gather.sndbuf     = buf;
      gather.cmd.xfer_gather.stype      = PAMI_TYPE_BYTE;
      gather.cmd.xfer_gather.stypecount = 0;
      gather.cmd.xfer_gather.rcvbuf     = rbuf;
      gather.cmd.xfer_gather.rtype      = PAMI_TYPE_BYTE;
      gather.cmd.xfer_gather.rtypecount = 0;

      int             k;

      gProtocolName = gather_always_works_md[nalg].name;

      for (k = 1; k >= 0; k--)
      {
        if (set[k])
        {
          if (task_id == root_zero)
          {
            printf("# Gather Bandwidth Test(size:%zu) -- context = %d, protocol: %s\n",subgeometry_num_tasks,
                   iContext, gProtocolName);
            printf("# Size(bytes)      iterations     bytes/sec      usec\n");
            printf("# -----------      -----------    -----------    ---------\n");
          }

          if (((strstr(gather_always_works_md[nalg].name, gSelected) == NULL) && gSelector) ||
              ((strstr(gather_always_works_md[nalg].name, gSelected) != NULL) && !gSelector))  continue;

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

            blocking_coll(context[iContext], &newbarrier, &newbar_poll_flag);
            ti = timer();

            gather.cmd.xfer_gather.stypecount = i;
            gather.cmd.xfer_gather.rtypecount = i;

            pami_endpoint_t root_ep;
            for (j = 0; j < niter; j++)
            {
              PAMI_Endpoint_create(client, root_zero, 0, &root_ep);
              gather.cmd.xfer_gather.root = root_ep;

              gather_initialize_sndbuf (local_task_id, buf, i);
              if (task_id == root_zero)
              {
                memset(rbuf, 0xFF, i*subgeometry_num_tasks);
              }
              blocking_coll(context[iContext], &gather, &gather_poll_flag);

              if (task_id == root_zero)
              {
                int rc_check;
                rc |= rc_check = gather_check_rcvbuf(subgeometry_num_tasks, rbuf, i);
                if (rc_check) fprintf(stderr, "%s FAILED validation\n", gProtocolName);
              }
              get_next_root(num_tasks, &root_zero);
            }

            tf = timer();
            blocking_coll(context[iContext], &newbarrier, &newbar_poll_flag);

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
    free(gather_always_works_algo);
    free(gather_always_works_md);
    free(gather_must_query_algo);
    free(gather_must_query_md);
  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

  buf = (char*)buf - gBuffer_offset;
  free(buf);

  rbuf = (char*)rbuf - gBuffer_offset;
  free(rbuf);

  rc |= pami_shutdown(&client, context, &gNum_contexts);
  return rc;
}
