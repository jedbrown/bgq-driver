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
 * \file test/api/collectives/alltoall.c
 * \brief Simple Alltoall test on world geometry (only bytes)
 */

/* see setup_env() for environment variable overrides               */
#define COUNT     (4096)

#include "../pami_util.h"

char *sbuf = NULL;
char *rbuf = NULL;

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

  /* Alltoall variables */
  size_t               alltoall_num_algorithm[2];
  pami_algorithm_t    *alltoall_always_works_algo = NULL;
  pami_metadata_t     *alltoall_always_works_md = NULL;
  pami_algorithm_t    *alltoall_must_query_algo = NULL;
  pami_metadata_t     *alltoall_must_query_md = NULL;
  pami_xfer_type_t     alltoall_xfer = PAMI_XFER_ALLTOALL;
  volatile unsigned    alltoall_poll_flag = 0;

  int                  nalg = 0;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          alltoall;

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

  /*  Allocate buffer(s) */
  int err = 0;
  err = posix_memalign((void*)&sbuf, 128, (gMax_byte_count*num_tasks)+gBuffer_offset);
  assert(err == 0);
  sbuf = (char*)sbuf + gBuffer_offset;

  err = posix_memalign((void*)&rbuf, 128, (gMax_byte_count*num_tasks)+gBuffer_offset);
  assert(err == 0);
  rbuf = (char*)rbuf + gBuffer_offset;

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

    /*  Query the world geometry for alltoallv algorithms */
    rc |= query_geometry_world(client,
                               context[iContext],
                               &world_geometry,
                               alltoall_xfer,
                               alltoall_num_algorithm,
                               &alltoall_always_works_algo,
                               &alltoall_always_works_md,
                               &alltoall_must_query_algo,
                               &alltoall_must_query_md);

    if (rc == 1)
      return 1;

    barrier.cb_done   = cb_done;
    barrier.cookie    = (void*) & bar_poll_flag;
    barrier.algorithm = bar_always_works_algo[0];

    alltoall.cb_done    = cb_done;
    alltoall.cookie     = (void*) & alltoall_poll_flag;


    for (nalg = 0; nalg < alltoall_num_algorithm[0]; nalg++)
    {
      size_t i, j;
      gProtocolName = alltoall_always_works_md[nalg].name;
      if (task_id == 0)
      {
        printf("# Alltoall Bandwidth Test(size:%zu) %p, protocol: %s\n", num_tasks, cb_done, alltoall_always_works_md[nalg].name);
        printf("# Size(bytes)  iterations    bytes/sec      usec\n");
        printf("# -----------      -----------    -----------    ---------\n");
      }

      if (((strstr(alltoall_always_works_md[nalg].name, gSelected) == NULL) && gSelector) ||
          ((strstr(alltoall_always_works_md[nalg].name, gSelected) != NULL) && !gSelector))  continue;

      alltoall.algorithm  = alltoall_always_works_algo[nalg];

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
          alltoall_initialize_bufs(sbuf, rbuf, i, j );
        }

        blocking_coll(context[iContext], &barrier, &bar_poll_flag);

        /* Warmup */
        alltoall.cmd.xfer_alltoall.sndbuf        = sbuf;
        alltoall.cmd.xfer_alltoall.stype         = PAMI_TYPE_BYTE;
        alltoall.cmd.xfer_alltoall.stypecount    = i;
        alltoall.cmd.xfer_alltoall.rcvbuf        = rbuf;
        alltoall.cmd.xfer_alltoall.rtype         = PAMI_TYPE_BYTE;
        alltoall.cmd.xfer_alltoall.rtypecount    = i;

        blocking_coll(context[iContext], &alltoall, &alltoall_poll_flag);
        blocking_coll(context[iContext], &alltoall, &alltoall_poll_flag);

        blocking_coll(context[iContext], &barrier, &bar_poll_flag);

        ti = timer();

        for (j = 0; j < niter; j++)
        {
          alltoall.cmd.xfer_alltoall.sndbuf        = sbuf;
          alltoall.cmd.xfer_alltoall.stype         = PAMI_TYPE_BYTE;
          alltoall.cmd.xfer_alltoall.stypecount    = i;
          alltoall.cmd.xfer_alltoall.rcvbuf        = rbuf;
          alltoall.cmd.xfer_alltoall.rtype         = PAMI_TYPE_BYTE;
          alltoall.cmd.xfer_alltoall.rtypecount    = i;
          blocking_coll(context[iContext], &alltoall, &alltoall_poll_flag);
        }

        tf = timer();


        int rc_check;
        rc |= rc_check = alltoall_check_rcvbuf(rbuf, i, num_tasks, task_id);
        if (rc_check) fprintf(stderr, "%s FAILED validation\n", gProtocolName);

        blocking_coll(context[iContext], &barrier, &bar_poll_flag);

        usec = (tf - ti) / (double)niter;

        if (task_id == 0)
        {

          printf("  %11zu %16d %14.1f %12.2f\n",
                 dataSent,
                 niter,
                 (double)1e6*(double)dataSent / (double)usec,
                 usec);
          fflush(stdout);
        }
      }
    }
    rc |= pami_shutdown(&client, context, &gNum_contexts);
    free(bar_always_works_algo);
    free(bar_always_works_md);
    free(bar_must_query_algo);
    free(bar_must_query_md);
    free(alltoall_always_works_algo);
    free(alltoall_always_works_md);
    free(alltoall_must_query_algo);
    free(alltoall_must_query_md);
  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

  sbuf = (char*)sbuf - gBuffer_offset;
  free(sbuf);

  rbuf = (char*)rbuf - gBuffer_offset;
  free(rbuf);

  return rc;
}
