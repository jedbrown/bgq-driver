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
 * \file test/api/collectives/allgatherv.c
 * \brief Simple Allgatherv test on world geometry (only gathers bytes)
 */

/* see setup_env() for environment variable overrides               */
#define COUNT     (524288)

#include "../pami_util.h"

int main(int argc, char*argv[])
{
  pami_client_t        client;
  pami_context_t      *context;
  pami_task_t          task_id, task_zero=0;
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

  /* Allgatherv variables */
  size_t               allgatherv_num_algorithm[2];
  pami_algorithm_t    *allgatherv_always_works_algo = NULL;
  pami_metadata_t     *allgatherv_always_works_md = NULL;
  pami_algorithm_t    *allgatherv_must_query_algo = NULL;
  pami_metadata_t     *allgatherv_must_query_md = NULL;
  pami_xfer_type_t     allgatherv_xfer = PAMI_XFER_ALLGATHERV;
  volatile unsigned    allgatherv_poll_flag = 0;

  int                  nalg = 0;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          allgatherv;

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
  void* buf = NULL;
  err = posix_memalign(&buf, 128, (gMax_byte_count) + gBuffer_offset);
  assert(err == 0);
  buf = (char*)buf + gBuffer_offset;

  void* rbuf = NULL;
  err = posix_memalign(&rbuf, 128, (gMax_byte_count * num_tasks) + gBuffer_offset);
  assert(err == 0);
  rbuf = (char*)rbuf + gBuffer_offset;

  size_t *lengths   = (size_t*)malloc(num_tasks * sizeof(size_t));
  assert(lengths);
  size_t *displs    = (size_t*)malloc(num_tasks * sizeof(size_t));
  assert(displs);


  unsigned iContext = 0;

  for (; iContext < gNum_contexts; ++iContext)
  {

    if (task_id == task_zero)
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

  /*  Query the world geometry for allgatherv algorithms */
  rc |= query_geometry_world(client,
                            context[iContext],
                            &world_geometry,
                            allgatherv_xfer,
                            allgatherv_num_algorithm,
                            &allgatherv_always_works_algo,
                            &allgatherv_always_works_md,
                            &allgatherv_must_query_algo,
                            &allgatherv_must_query_md);

  if (rc == 1)
    return 1;

  barrier.cb_done   = cb_done;
  barrier.cookie    = (void*) & bar_poll_flag;
  barrier.algorithm = bar_always_works_algo[0];
  blocking_coll(context[iContext], &barrier, &bar_poll_flag);

    for (nalg = 0; nalg < allgatherv_num_algorithm[0]; nalg++)
    {
      allgatherv.cb_done    = cb_done;
      allgatherv.cookie     = (void*) & allgatherv_poll_flag;
      allgatherv.algorithm  = allgatherv_always_works_algo[nalg];
      allgatherv.cmd.xfer_allgatherv.sndbuf     = buf;
      allgatherv.cmd.xfer_allgatherv.stype      = PAMI_TYPE_BYTE;
      allgatherv.cmd.xfer_allgatherv.stypecount = 0;
      allgatherv.cmd.xfer_allgatherv.rcvbuf     = rbuf;
      allgatherv.cmd.xfer_allgatherv.rtype      = PAMI_TYPE_BYTE;
      allgatherv.cmd.xfer_allgatherv.rtypecounts = lengths;
      allgatherv.cmd.xfer_allgatherv.rdispls     = displs;

      gProtocolName = allgatherv_always_works_md[nalg].name;

      if (task_id == task_zero)
      {
        printf("# Allgatherv Bandwidth Test(size:%zu) -- context = %d, protocol: %s\n",num_tasks,
               iContext, gProtocolName);
        printf("# Size(bytes)      iterations     bytes/sec      usec\n");
        printf("# -----------      -----------    -----------    ---------\n");
      }

      if (((strstr(allgatherv_always_works_md[nalg].name,gSelected) == NULL) && gSelector) ||
          ((strstr(allgatherv_always_works_md[nalg].name,gSelected) != NULL) && !gSelector))  continue;

      int i, j,k;

      for (i = gMin_byte_count; i <= gMax_byte_count; i *= 2)
      {
        size_t  dataSent = i;
        int          niter;

        for (k = 0; k < num_tasks; k++)lengths[k] = i;
        for (k = 0; k < num_tasks; k++)displs[k]  = k*i;
        if (dataSent < CUTOFF)
          niter = gNiterlat;
        else
          niter = NITERBW;

        allgatherv.cmd.xfer_allgatherv.stypecount       = i;

        allgather_initialize_sndbuf (buf, i, task_id);
        memset(rbuf, 0xFF, i);

        blocking_coll(context[iContext], &barrier, &bar_poll_flag);
        ti = timer();

        for (j = 0; j < niter; j++)
        {
          blocking_coll(context[iContext], &allgatherv, &allgatherv_poll_flag);
        }

        tf = timer();
        blocking_coll(context[iContext], &barrier, &bar_poll_flag);

        int rc_check;
        rc |= rc_check = allgather_check_rcvbuf (rbuf, i, num_tasks);

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

  free(bar_always_works_algo);
  free(bar_always_works_md);
  free(bar_must_query_algo);
  free(bar_must_query_md);
  free(allgatherv_always_works_algo);
  free(allgatherv_always_works_md);
  free(allgatherv_must_query_algo);
  free(allgatherv_must_query_md);
  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

  buf = (char*)buf - gBuffer_offset;
  free(buf);

  rbuf = (char*)rbuf - gBuffer_offset;
  free(rbuf);

  free(lengths);
  free(displs);

 rc |= pami_shutdown(&client, context, &gNum_contexts);
  return rc;
}
