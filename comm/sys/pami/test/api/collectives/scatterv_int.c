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
 * \file test/api/collectives/scatterv_int.c
 * \brief Simple scatterv_int test
 */

/* see setup_env() for environment variable overrides               */
#define COUNT     (524288)

#include "../pami_util.h"

int main(int argc, char*argv[])
{
  pami_client_t        client;
  pami_context_t      *context;
  pami_task_t          task_id, root_zero=0;
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

  /* Scatterv variables */
  pami_algorithm_t    *next_algo = NULL;
  pami_metadata_t     *next_md= NULL;
  size_t               scatterv_num_algorithm[2];
  pami_algorithm_t    *scatterv_always_works_algo = NULL;
  pami_metadata_t     *scatterv_always_works_md = NULL;
  pami_algorithm_t    *scatterv_must_query_algo = NULL;
  pami_metadata_t     *scatterv_must_query_md = NULL;
  pami_xfer_type_t     scatterv_xfer = PAMI_XFER_SCATTERV_INT;
  volatile unsigned    scatterv_poll_flag = 0;

  int                  nalg= 0, total_alg;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          scatterv;

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
  err = posix_memalign(&buf, 128, (gMax_byte_count * num_tasks) + gBuffer_offset);
  assert(err == 0);
  buf = (char*)buf + gBuffer_offset;

  void* rbuf = NULL;
  err = posix_memalign(&rbuf, 128, (gMax_byte_count * num_tasks) + gBuffer_offset);
  assert(err == 0);
  rbuf = (char*)rbuf + gBuffer_offset;

  int *lengths   = (int*)malloc(num_tasks * sizeof(int));
  assert(lengths);
  int *displs    = (int*)malloc(num_tasks * sizeof(int));
  assert(displs);


  unsigned iContext = 0;

  for (; iContext < gNum_contexts; ++iContext)
  {

    if (task_id == root_zero)
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

  /*  Query the world geometry for scatterv algorithms */
  rc |= query_geometry_world(client,
                            context[iContext],
                            &world_geometry,
                            scatterv_xfer,
                            scatterv_num_algorithm,
                            &scatterv_always_works_algo,
                            &scatterv_always_works_md,
                            &scatterv_must_query_algo,
                            &scatterv_must_query_md);

  if (rc == 1)
    return 1;

  barrier.cb_done   = cb_done;
  barrier.cookie    = (void*) & bar_poll_flag;
  barrier.algorithm = bar_always_works_algo[0];
  blocking_coll(context[iContext], &barrier, &bar_poll_flag);

    total_alg = scatterv_num_algorithm[0]+scatterv_num_algorithm[1];
    for (nalg = 0; nalg < total_alg; nalg++)
    {
      metadata_result_t result = {0};
      unsigned query_protocol;
      if(nalg < scatterv_num_algorithm[0])
      {  
        query_protocol = 0;
        next_algo = &scatterv_always_works_algo[nalg];
        next_md  = &scatterv_always_works_md[nalg];
      }
      else
      {  
        query_protocol = 1;
        next_algo = &scatterv_must_query_algo[nalg-scatterv_num_algorithm[0]];
        next_md  = &scatterv_must_query_md[nalg-scatterv_num_algorithm[0]];
      }

      pami_endpoint_t    root_ep;
      pami_task_t root_task = 0;
      PAMI_Endpoint_create(client, root_task, 0, &root_ep);
      scatterv.cmd.xfer_scatterv_int.root        = root_ep;
      scatterv.cb_done                           = cb_done;
      scatterv.cookie                            = (void*) & scatterv_poll_flag;
      scatterv.algorithm                     = *next_algo;
      scatterv.cmd.xfer_scatterv_int.sndbuf      = buf;
      scatterv.cmd.xfer_scatterv_int.stype       = PAMI_TYPE_BYTE;
      scatterv.cmd.xfer_scatterv_int.stypecounts = lengths;
      scatterv.cmd.xfer_scatterv_int.sdispls     = displs;
      scatterv.cmd.xfer_scatterv_int.rcvbuf      = rbuf;
      scatterv.cmd.xfer_scatterv_int.rtype       = PAMI_TYPE_BYTE;
      scatterv.cmd.xfer_scatterv_int.rtypecount  = 0;

      gProtocolName = next_md->name;

      if (task_id == root_task)
      {
        printf("# Scatterv_int Bandwidth Test(size:%zu) -- context = %d, protocol: %s, Metadata: range %zu <-> %zd, mask %#X\n",num_tasks,
               iContext, gProtocolName,
               next_md->range_lo,(ssize_t)next_md->range_hi,
               next_md->check_correct.bitmask_correct);
        printf("# Size(bytes)      iterations     bytes/sec      usec\n");
        printf("# -----------      -----------    -----------    ---------\n");
      }

      if (((strstr(next_md->name, gSelected) == NULL) && gSelector) ||
          ((strstr(next_md->name, gSelected) != NULL) && !gSelector))  continue;


      size_t i, j;

      unsigned checkrequired = next_md->check_correct.values.checkrequired; /*must query every time */
      assert(!checkrequired || next_md->check_fn); /* must have function if checkrequired. */

      for (i = gMin_byte_count; i <= gMax_byte_count; i *= 2)
      {
        size_t  dataSent = i;
        int          niter;
        size_t           k = 0;

        lengths[0] = i;
        displs[0]  = 0;
        for (k = 1; k < num_tasks; k++)
        {
          lengths[k] = i;
          displs[k]  = lengths[k-1];
        }

        if (dataSent < CUTOFF)
          niter = gNiterlat;
        else
          niter = NITERBW;

        if(query_protocol)
        {  
          result = check_metadata(*next_md,
                                  scatterv,
                                  scatterv.cmd.xfer_scatterv_int.stype,
                                  scatterv.cmd.xfer_scatterv_int.stypecounts[0], /* \todo Does range metadata make sense? */
                                  scatterv.cmd.xfer_scatterv_int.sndbuf,
                                  scatterv.cmd.xfer_scatterv_int.rtype,
                                  scatterv.cmd.xfer_scatterv_int.rtypecount,
                                  scatterv.cmd.xfer_scatterv_int.rcvbuf);
          if (next_md->check_correct.values.nonlocal)
          {
            /* \note We currently ignore check_correct.values.nonlocal
              because these tests should not have nonlocal differences (so far). */
            result.check.nonlocal = 0;
          }

          if (result.bitmask) continue;
        }

        blocking_coll(context[iContext], &barrier, &bar_poll_flag);
        ti = timer();

        for (j = 0; j < niter; j++)
        {
          scatterv.cmd.xfer_scatterv_int.rtypecount = i;
          if (checkrequired) /* must query every time */
          {
            result = next_md->check_fn(&scatterv);
            if (result.bitmask) continue;
          }
          blocking_coll(context[iContext], &scatterv, &scatterv_poll_flag);
        }

        tf = timer();
        blocking_coll(context[iContext], &barrier, &bar_poll_flag);

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

  free(bar_always_works_algo);
  free(bar_always_works_md);
  free(bar_must_query_algo);
  free(bar_must_query_md);
  free(scatterv_always_works_algo);
  free(scatterv_always_works_md);
  free(scatterv_must_query_algo);
  free(scatterv_must_query_md);
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
