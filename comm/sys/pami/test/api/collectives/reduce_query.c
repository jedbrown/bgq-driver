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
 * \file test/api/collectives/reduce_query.c
 * \brief Simple reduce on world geometry using "query" algorithms
 */

/* see setup_env() for environment variable overrides               */
#include "../pami_util.h"

int main(int argc, char*argv[])
{
  pami_client_t        client;
  pami_context_t      *context;
  pami_task_t          task_id, root=0;
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

  /* Reduce variables */
  size_t               reduce_num_algorithm[2];
  pami_algorithm_t    *reduce_always_works_algo = NULL;
  pami_metadata_t     *reduce_always_works_md = NULL;
  pami_algorithm_t    *reduce_must_query_algo = NULL;
  pami_metadata_t     *reduce_must_query_md = NULL;
  pami_xfer_type_t     reduce_xfer = PAMI_XFER_REDUCE;
  volatile unsigned    reduce_poll_flag = 0;

  int                  i, j, nalg = 0;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          reduce;

  /* Process environment variables and setup globals */
  setup_env();

  assert(gNum_contexts > 0);
  context = (pami_context_t*)malloc(sizeof(pami_context_t) * gNum_contexts);

  /*  Allocate buffer(s) */
  int err = 0;
  void* sbuf = NULL;
  err = posix_memalign(&sbuf, 128, gMax_byte_count + gBuffer_offset);
  assert(err == 0);
  sbuf = (char*)sbuf + gBuffer_offset;
  void* rbuf = NULL;
  err = posix_memalign(&rbuf, 128, gMax_byte_count + gBuffer_offset);
  assert(err == 0);
  rbuf = (char*)rbuf + gBuffer_offset;

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

  barrier.cb_done   = cb_done;
  barrier.cookie    = (void*) & bar_poll_flag;
  barrier.algorithm = bar_always_works_algo[0];

  unsigned iContext = 0;

  for (; iContext < gNum_contexts; ++iContext)
  {

    if (task_id == 0)
      printf("# Context: %u\n", iContext);

    /*  Query the world geometry for reduce algorithms */
    rc |= query_geometry_world(client,
                               context[iContext],
                               &world_geometry,
                               reduce_xfer,
                               reduce_num_algorithm,
                               &reduce_always_works_algo,
                               &reduce_always_works_md,
                               &reduce_must_query_algo,
                               &reduce_must_query_md);

    if (rc == 1)
      return 1;

    for (nalg = 0; nalg < reduce_num_algorithm[1]; nalg++)
    {
      metadata_result_t result = {0};

      if (task_id == root) 
      {
        printf("# Reduce Bandwidth Test(size:%zu) -- context = %d, root = %d protocol: %s, Metadata: range %zu <-> %zd, mask %#X\n", num_tasks,
               iContext,root, reduce_must_query_md[nalg].name,
               reduce_must_query_md[nalg].range_lo,(ssize_t)reduce_must_query_md[nalg].range_hi,
               reduce_must_query_md[nalg].check_correct.bitmask_correct);
        printf("# Size(bytes)      iterations     bytes/sec      usec\n");
        printf("# -----------      -----------    -----------    ---------\n");
      }
      if (((strstr(reduce_must_query_md[nalg].name,gSelected) == NULL) && gSelector) ||
          ((strstr(reduce_must_query_md[nalg].name,gSelected) != NULL) && !gSelector))  continue;

            gProtocolName = reduce_must_query_md[nalg].name;

      barrier.cb_done   = cb_done;
      barrier.cookie    = (void*) & bar_poll_flag;
      barrier.algorithm = bar_always_works_algo[0];
      blocking_coll(context[0], &barrier, &bar_poll_flag);
      unsigned checkrequired = reduce_must_query_md[nalg].check_correct.values.checkrequired; /*must query every time */
      assert(!checkrequired || reduce_must_query_md[nalg].check_fn); /* must have function if checkrequired. */


      reduce.cb_done   = cb_done;
      reduce.cookie    = (void*) & reduce_poll_flag;
      reduce.algorithm = reduce_must_query_algo[nalg];
      reduce.cmd.xfer_reduce.sndbuf    = sbuf;
      reduce.cmd.xfer_reduce.rcvbuf    = rbuf;
      reduce.cmd.xfer_reduce.rtype     = PAMI_TYPE_BYTE;
      reduce.cmd.xfer_reduce.rtypecount = 0;

      int op, dt;

      for (dt = 0; dt < dt_count; dt++)
      {
        for (op = 0; op < op_count; op++)
        {
          if (gValidTable[op][dt])
          {
            if (task_id == root)
              printf("Running Reduce: %s, %s\n", dt_array_str[dt], op_array_str[op]);

            for (i = MAX(1,gMin_byte_count/get_type_size(dt_array[dt])); i <= gMax_byte_count/get_type_size(dt_array[dt]); i *= 2)
            {
              size_t sz=get_type_size(dt_array[dt]);
              size_t  dataSent = i * sz;
              int niter;

              if (dataSent < CUTOFF)
                niter = gNiterlat;
              else
                niter = NITERBW;

              reduce.cmd.xfer_reduce.stypecount = i;
              reduce.cmd.xfer_reduce.rtypecount = dataSent;
              reduce.cmd.xfer_reduce.stype      = dt_array[dt];
              reduce.cmd.xfer_reduce.op         = op_array[op];

              pami_endpoint_t root_ep;
              PAMI_Endpoint_create(client, root, 0, &root_ep);
              reduce.cmd.xfer_reduce.root    = root_ep;
              if (task_id == root)
                reduce.cmd.xfer_reduce.rcvbuf    = rbuf;
              else
                reduce.cmd.xfer_reduce.rcvbuf    = NULL;

              result = check_metadata(reduce_must_query_md[nalg],
                                      reduce,
                                      dt_array[dt],
                                      dataSent, /* metadata uses bytes i, */
                                      reduce.cmd.xfer_allreduce.sndbuf,
                                      PAMI_TYPE_BYTE,
                                      dataSent,
                                      reduce.cmd.xfer_allreduce.rcvbuf);


              if (reduce_must_query_md[nalg].check_correct.values.nonlocal)
              {
                /* \note We currently ignore check_correct.values.nonlocal
                   because these tests should not have nonlocal differences (so far). */
                result.check.nonlocal = 0;
              }

              if (result.bitmask) continue;

              reduce_initialize_sndbuf (sbuf, i, op, dt, task_id, num_tasks);
              memset(rbuf, 0xFF, dataSent);

              /* We aren't testing barrier itself, so use context 0. */
              blocking_coll(context[0], &barrier, &bar_poll_flag);
              ti = timer();

              for (j = 0; j < niter; j++)
              {
                if (checkrequired) /* must query every time */
                {
                  result = reduce_must_query_md[nalg].check_fn(&reduce);
                  if (result.bitmask) continue;
                }

                blocking_coll(context[iContext], &reduce, &reduce_poll_flag);
              }

              tf = timer();
              /* We aren't testing barrier itself, so use context 0. */
              blocking_coll(context[0], &barrier, &bar_poll_flag);

              int rc_check=0;
              if (task_id == root)
              {
                rc |= rc_check = reduce_check_rcvbuf (rbuf, i, op, dt, task_id, num_tasks);
                if (rc_check) fprintf(stderr, "%s FAILED validation\n", gProtocolName);
              }
              usec = (tf - ti) / (double)niter;

              if (task_id == root)
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
    }

    free(reduce_always_works_algo);
    free(reduce_always_works_md);
    free(reduce_must_query_algo);
    free(reduce_must_query_md);

  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

    free(bar_always_works_algo);
    free(bar_always_works_md);
    free(bar_must_query_algo);
    free(bar_must_query_md);

  sbuf = (char*)sbuf - gBuffer_offset;
  free(sbuf);
  rbuf = (char*)rbuf - gBuffer_offset;
  free(rbuf);

  rc |= pami_shutdown(&client, context, &gNum_contexts);
    return rc;
  }
