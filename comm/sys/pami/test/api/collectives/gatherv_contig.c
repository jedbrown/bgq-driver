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
   \file test/api/collectives/gatherv_contig.c
   \brief Simple gatherv test on world geometry with contiguous datatypes
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

  /* Gatherv variables */
  size_t               gatherv_num_algorithm[2];
  pami_algorithm_t    *next_algo = NULL;
  pami_metadata_t     *next_md= NULL;
  pami_algorithm_t    *gatherv_always_works_algo = NULL;
  pami_metadata_t     *gatherv_always_works_md = NULL;
  pami_algorithm_t    *gatherv_must_query_algo = NULL;
  pami_metadata_t     *gatherv_must_query_md = NULL;
  pami_xfer_type_t     gatherv_xfer = PAMI_XFER_GATHERV;
  volatile unsigned    gatherv_poll_flag = 0;

  int                  nalg= 0, total_alg;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          gatherv;

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

  size_t *lengths   = (size_t*)malloc(num_tasks * sizeof(size_t));
  assert(lengths);
  size_t *displs    = (size_t*)malloc(num_tasks * sizeof(size_t));
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

    /*  Query the world geometry for gatherv algorithms */
    rc |= query_geometry_world(client,
                            context[iContext],
                            &world_geometry,
                            gatherv_xfer,
                            gatherv_num_algorithm,
                            &gatherv_always_works_algo,
                            &gatherv_always_works_md,
                            &gatherv_must_query_algo,
                            &gatherv_must_query_md);

    if (rc == 1)
      return 1;

    barrier.cb_done   = cb_done;
    barrier.cookie    = (void*) & bar_poll_flag;
    barrier.algorithm = bar_always_works_algo[0];
    blocking_coll(context[iContext], &barrier, &bar_poll_flag);

    total_alg = gatherv_num_algorithm[0]+gatherv_num_algorithm[1];
    for (nalg = 0; nalg < total_alg; nalg++)
    {
      metadata_result_t result = {0};
      unsigned query_protocol;
      if(nalg < gatherv_num_algorithm[0])
      {  
        query_protocol = 0;
        next_algo = &gatherv_always_works_algo[nalg];
        next_md  = &gatherv_always_works_md[nalg];
      }
      else
      {  
        query_protocol = 1;
        next_algo = &gatherv_must_query_algo[nalg-gatherv_num_algorithm[0]];
        next_md  = &gatherv_must_query_md[nalg-gatherv_num_algorithm[0]];
      }

      root_zero = 0;

      gatherv.cb_done                       = cb_done;
      gatherv.cookie                        = (void*) & gatherv_poll_flag;
      gatherv.algorithm  = *next_algo;

      gatherv.cmd.xfer_gatherv.sndbuf      = buf;
      gatherv.cmd.xfer_gatherv.stype       = PAMI_TYPE_BYTE;
      gatherv.cmd.xfer_gatherv.stypecount  = 0;
      gatherv.cmd.xfer_gatherv.rcvbuf      = rbuf;
      gatherv.cmd.xfer_gatherv.rtype       = PAMI_TYPE_BYTE;
      gatherv.cmd.xfer_gatherv.rtypecounts = lengths;
      gatherv.cmd.xfer_gatherv.rdispls     = displs;

      gProtocolName = next_md->name;

      if (task_id == root_zero)
      {
        printf("# Gatherv Bandwidth Test(size:%zu) -- context = %d, protocol: %s, Metadata: range %zu <-> %zd, mask %#X\n",num_tasks,
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

      int dt,op=4/*SUM*/;

      for (dt = 0; dt < dt_count; dt++)
      {
        if ((gFull_test && ((dt != DT_NULL) && (dt != DT_BYTE))) || gValidTable[op][dt])
        {
          if (task_id == 0)
            printf("Running gatherv: %s\n", dt_array_str[dt]);

          for (i = MAX(1,gMin_byte_count/get_type_size(dt_array[dt])); i <= gMax_byte_count/get_type_size(dt_array[dt]); i *= 2)
          {
            size_t dataSent = i * get_type_size(dt_array[dt]);
            int          niter;
            size_t           k = 0;

            for (k = 0; k < num_tasks; k++)
            {
              lengths[k] = i;
              displs[k]  = k * i;
            }
            lengths[k-1] = 0;

            if (dataSent < CUTOFF)
              niter = gNiterlat;
            else
              niter = NITERBW;

                if(query_protocol)
                {  
                  size_t sz=get_type_size(dt_array[dt])*i;
                /* Must initialize all of cmd for metadata */
                  gatherv.cmd.xfer_gatherv.root        = 0;
                  result = check_metadata(*next_md,
                                          gatherv,
                                          dt_array[dt],
                                          sz, /* metadata uses bytes i, */
                                          gatherv.cmd.xfer_gatherv.sndbuf,
                                          dt_array[dt],
                                          sz,
                                          gatherv.cmd.xfer_gatherv.rcvbuf);
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
              root_zero = (root_zero + num_tasks - 1) % num_tasks;
              pami_endpoint_t root_ep;
              PAMI_Endpoint_create(client, root_zero, 0, &root_ep);
              gatherv.cmd.xfer_gatherv.root        = root_ep;

              gather_initialize_sndbuf_dt (buf, i, task_id, dt);
              if (task_id == root_zero)
                memset(rbuf, 0xFF, i*num_tasks);

              if (task_id != num_tasks - 1)
              {
                gatherv.cmd.xfer_gatherv.stypecount  = i;
              }
              gatherv.cmd.xfer_gatherv.stype       = dt_array[dt];
              gatherv.cmd.xfer_gatherv.rtype       = dt_array[dt];
              if (checkrequired) /* must query every time */
              {
                result = next_md->check_fn(&gatherv);
                if (result.bitmask) continue;
              }
              blocking_coll(context[iContext], &gatherv, &gatherv_poll_flag);

              if (task_id == root_zero)
              {
                int rc_check;
                rc |= rc_check = gather_check_rcvbuf_dt(num_tasks-1, rbuf, i, dt);
                if (rc_check) fprintf(stderr, "%s FAILED validation on %s\n", gProtocolName, dt_array_str[dt]);
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
      }
    }
    free(bar_always_works_algo);
    free(bar_always_works_md);
    free(bar_must_query_algo);
    free(bar_must_query_md);
    free(gatherv_always_works_algo);
    free(gatherv_always_works_md);
    free(gatherv_must_query_algo);
    free(gatherv_must_query_md);
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
