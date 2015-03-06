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
 * \file test/api/collectives/alltoallv_int_subcomm.c
 * \brief Simple alltoallv_int test on sub-geometries (only bytes)
 */

/* see setup_env() for environment variable overrides               */
#define COUNT     (4096)

#include "../pami_util.h"


int *sndlens = NULL;
int *sdispls = NULL;
int *rcvlens = NULL;
int *rdispls = NULL;

int main(int argc, char*argv[])
{
  pami_client_t        client;
  pami_context_t      *context;
  pami_task_t          task_id, local_task_id=0, task_zero = 0;
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
  volatile unsigned    newbar_poll_flag = 0;

  /* alltoallv_int variables */
  size_t               alltoallv_int_num_algorithm[2];
  pami_algorithm_t    *alltoallv_int_always_works_algo = NULL;
  pami_metadata_t     *alltoallv_int_always_works_md = NULL;
  pami_algorithm_t    *alltoallv_int_must_query_algo = NULL;
  pami_metadata_t     *alltoallv_int_must_query_md = NULL;
  pami_xfer_type_t     alltoallv_int_xfer = PAMI_XFER_ALLTOALLV_INT;
  volatile unsigned    alltoallv_int_poll_flag = 0;

  int                  nalg = 0;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          alltoallv_int;

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

  assert(task_id >= 0);
  assert(task_id < num_tasks);

  /*  Allocate buffer(s) */
  int err = 0;
  void* sbuf = NULL;
  err = posix_memalign((void*) & sbuf, 128, (gMax_byte_count * num_tasks) + gBuffer_offset);
  assert(err == 0);
  sbuf = (char*)sbuf + gBuffer_offset;

  void* rbuf = NULL;
  err = posix_memalign((void*) & rbuf, 128, (gMax_byte_count * num_tasks) + gBuffer_offset);
  assert(err == 0);
  rbuf = (char*)rbuf + gBuffer_offset;

  sndlens = (int*) malloc(num_tasks * sizeof(int));
  assert(sndlens);
  sdispls = (int*) malloc(num_tasks * sizeof(int));
  assert(sdispls);
  rcvlens = (int*) malloc(num_tasks * sizeof(int));
  assert(rcvlens);
  rdispls = (int*) malloc(num_tasks * sizeof(int));
  assert(rdispls);

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

  int unused_non_task_zero[2];
  get_split_method(&num_tasks, task_id, &rangecount, range, &local_task_id, set, &id, &task_zero,unused_non_task_zero);

  unsigned iContext = 0;

  for (; iContext < gNum_contexts; ++iContext)
  {

    if (task_id == task_zero)
      printf("# Context: %u\n", iContext);

    /* Delay task_zero tasks, and emulate that he's doing "other"
       message passing.  This will cause the geometry_create
       request from other nodes to be unexpected when doing
       parentless geometries and won't affect parented.      */
    if (task_id == task_zero)
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

    /*  Query the sub geometry for alltoallv algorithms */
    rc |= query_geometry(client,
                         context[iContext],
                         newgeometry,
                         alltoallv_int_xfer,
                         alltoallv_int_num_algorithm,
                         &alltoallv_int_always_works_algo,
                         &alltoallv_int_always_works_md,
                         &alltoallv_int_must_query_algo,
                         &alltoallv_int_must_query_md);

    if (rc == 1)
      return 1;

    /*  Set up sub geometry barrier */
    newbarrier.cb_done   = cb_done;
    newbarrier.cookie    = (void*) & newbar_poll_flag;
    newbarrier.algorithm = newbar_algo[0];

    for (nalg = 0; nalg < alltoallv_int_num_algorithm[0]; nalg++)
    {
      alltoallv_int.cb_done    = cb_done;
      alltoallv_int.cookie     = (void*) & alltoallv_int_poll_flag;
      alltoallv_int.algorithm  = alltoallv_int_always_works_algo[nalg];
      alltoallv_int.cmd.xfer_alltoallv_int.sndbuf        = sbuf;
      alltoallv_int.cmd.xfer_alltoallv_int.stype         = PAMI_TYPE_BYTE;
      alltoallv_int.cmd.xfer_alltoallv_int.stypecounts   = sndlens;
      alltoallv_int.cmd.xfer_alltoallv_int.sdispls       = sdispls;
      alltoallv_int.cmd.xfer_alltoallv_int.rcvbuf        = rbuf;
      alltoallv_int.cmd.xfer_alltoallv_int.rtype         = PAMI_TYPE_BYTE;
      alltoallv_int.cmd.xfer_alltoallv_int.rtypecounts   = rcvlens;
      alltoallv_int.cmd.xfer_alltoallv_int.rdispls       = rdispls;

      int             k;

      gProtocolName = alltoallv_int_always_works_md[nalg].name;

      for (k = 1; k >= 0; k--)
      {
        if (set[k])
        {
          if (task_id == task_zero)
          {
            printf("# Alltoallv_int Bandwidth Test(size:%zu) -- context = %d, task_zero = %d, protocol: %s\n",
                   num_tasks, iContext, task_zero, gProtocolName);
            printf("# Size(bytes)      iterations    bytes/sec      usec\n");
            printf("# -----------      -----------    -----------    ---------\n");
          }

          if (((strstr(alltoallv_int_always_works_md[nalg].name, gSelected) == NULL) && gSelector) ||
              ((strstr(alltoallv_int_always_works_md[nalg].name, gSelected) != NULL) && !gSelector))  continue;

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

            for (j = 0; j < num_tasks; j++)
            {
              sndlens[j] = rcvlens[j] = i;
              sdispls[j] = rdispls[j] = i * j;

              alltoallv_int_initialize_bufs(sbuf, rbuf, sndlens, rcvlens, sdispls, rdispls, j);
            }

            blocking_coll(context[iContext], &newbarrier, &newbar_poll_flag);

            /* Warmup */
            blocking_coll(context[iContext], &alltoallv_int, &alltoallv_int_poll_flag);

            blocking_coll(context[iContext], &newbarrier, &newbar_poll_flag);

            ti = timer();

            for (j = 0; j < niter; j++)
            {
              blocking_coll(context[iContext], &alltoallv_int, &alltoallv_int_poll_flag);
            }

            tf = timer();
            blocking_coll(context[iContext], &newbarrier, &newbar_poll_flag);

            int rc_check;
            rc |= rc_check = alltoallv_int_check_rcvbuf(rbuf, rcvlens, rdispls, num_tasks, local_task_id);

            if (rc_check) fprintf(stderr, "%s FAILED validation\n", gProtocolName);

            usec = (tf - ti) / (double)niter;

            if (task_id == task_zero)
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
    free(alltoallv_int_always_works_algo);
    free(alltoallv_int_always_works_md);
    free(alltoallv_int_must_query_algo);
    free(alltoallv_int_must_query_md);
  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

  sbuf = (char*)sbuf - gBuffer_offset;
  free(sbuf);

  rbuf = (char*)rbuf - gBuffer_offset;
  free(rbuf);

  free(sndlens);
  free(sdispls);
  free(rcvlens);
  free(rdispls);

  rc |= pami_shutdown(&client, context, &gNum_contexts);
  return rc;
}
