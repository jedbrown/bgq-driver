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
   \file test/api/collectives/gather.c
   \brief Simple gather test 
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

  if (rc == 1)
    return 1;

  /*  Allocate buffer(s) */
  int err = 0;
  void* buf = NULL;
  err = posix_memalign(&buf, 128, gMax_byte_count + gBuffer_offset);
  assert(err == 0);
  buf = (char*)buf + gBuffer_offset;

  void* rbuf = NULL;
  err = posix_memalign(&rbuf, 128, (gMax_byte_count * num_tasks) + gBuffer_offset);
  assert(err == 0);
  rbuf = (char*)rbuf + gBuffer_offset;


  unsigned iContext = 0;

  for (; iContext < gNum_contexts; ++iContext)
  {

    if (task_id == root_zero)
      printf("# Context: %u\n", iContext);

  /*  Query the world geometry for barrier algorithms */
  rc |= query_geometry_world(client,
                            context,
                            &world_geometry,
                            barrier_xfer,
                            barrier_num_algorithm,
                            &bar_always_works_algo,
                            &bar_always_works_md,
                            &bar_must_query_algo,
                            &bar_must_query_md);

  if (rc == 1)
    return 1;

  /*  Query the world geometry for gather algorithms */
  rc |= query_geometry_world(client,
                            context[iContext],
                            &world_geometry,
                            gather_xfer,
                            gather_num_algorithm,
                            &gather_always_works_algo,
                            &gather_always_works_md,
                            &gather_must_query_algo,
                            &gather_must_query_md);

  if (rc == 1)
    return 1;

  barrier.cb_done   = cb_done;
  barrier.cookie    = (void*) & bar_poll_flag;
  barrier.algorithm = bar_always_works_algo[0];
  blocking_coll(context[iContext], &barrier, &bar_poll_flag);

    for (nalg = 0; nalg < gather_num_algorithm[0]; nalg++)
    {
      root_zero = 0;

      gather.cb_done    = cb_done;
      gather.cookie     = (void*) & gather_poll_flag;
      gather.algorithm  = gather_always_works_algo[nalg];

      gather.cmd.xfer_gather.sndbuf     = buf;
      gather.cmd.xfer_gather.stype      = PAMI_TYPE_BYTE;
      gather.cmd.xfer_gather.stypecount = 0;
      gather.cmd.xfer_gather.rcvbuf     = rbuf;
      gather.cmd.xfer_gather.rtype      = PAMI_TYPE_BYTE;
      gather.cmd.xfer_gather.rtypecount = 0;

      gProtocolName = gather_always_works_md[nalg].name;

      if (task_id == root_zero)
      {
        printf("# Gather Bandwidth Test(size:%zu) -- context = %d, protocol: %s\n",num_tasks,
               iContext, gProtocolName);
        printf("# Size(bytes)      iterations     bytes/sec      usec\n");
        printf("# -----------      -----------    -----------    ---------\n");
      }

      if (((strstr(gather_always_works_md[nalg].name,gSelected) == NULL) && gSelector) ||
          ((strstr(gather_always_works_md[nalg].name,gSelected) != NULL) && !gSelector))  continue;


      int i, j;

      for (i = gMin_byte_count; i <= gMax_byte_count; i *= 2)
      {
        size_t  dataSent = i;
        int          niter;
        if (dataSent < CUTOFF)
          niter = gNiterlat;
        else
          niter = NITERBW;

        blocking_coll(context[iContext], &barrier, &bar_poll_flag);
        ti = timer();

        gather.cmd.xfer_gather.stypecount = i;
        gather.cmd.xfer_gather.rtypecount = i;

        for (j = 0; j < niter; j++)
        {
          root_zero = (root_zero + num_tasks - 1) % num_tasks; 
          pami_endpoint_t root_ep;
          PAMI_Endpoint_create(client, root_zero, 0, &root_ep);
          gather.cmd.xfer_gather.root       = root_ep;

          gather_initialize_sndbuf (task_id, buf, i);
          if (task_id == root_zero)
            memset(rbuf, 0xFF, i*num_tasks);
          blocking_coll(context[iContext], &gather, &gather_poll_flag);

          if (task_id == root_zero)
          {
            int rc_check;
            rc |= rc_check = gather_check_rcvbuf(num_tasks, rbuf, i);
            if (rc_check) fprintf(stderr, "%s FAILED validation\n", gProtocolName);
          }

        }

        tf = timer();
        blocking_coll(context[iContext], &barrier, &bar_poll_flag);

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

  free(bar_always_works_algo);
  free(bar_always_works_md);
  free(bar_must_query_algo);
  free(bar_must_query_md);
  free(gather_always_works_algo);
  free(gather_always_works_md);
  free(gather_must_query_algo);
  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

  buf = (char*)buf - gBuffer_offset;
  free(buf);

  rbuf = (char*)rbuf - gBuffer_offset;
  free(rbuf);

 rc |= pami_shutdown(&client, context, &gNum_contexts);
  return rc;
}
