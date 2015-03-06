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
 * \file test/api/collectives/scatter_contig.c
 * \brief Simple Scatter test on world geometry with contiguous datatypes
 */

/* Use arg -h or see setup_env() for environment variable overrides  */

#define COUNT     (524288)

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

  /* Scatter variables */
  pami_algorithm_t    *next_algo = NULL;
  pami_metadata_t     *next_md= NULL;
  size_t               scatter_num_algorithm[2];
  pami_algorithm_t    *scatter_always_works_algo = NULL;
  pami_metadata_t     *scatter_always_works_md = NULL;
  pami_algorithm_t    *scatter_must_query_algo = NULL;
  pami_metadata_t     *scatter_must_query_md = NULL;
  pami_xfer_type_t     scatter_xfer = PAMI_XFER_SCATTER;
  volatile unsigned    scatter_poll_flag = 0;

  int                  nalg= 0, total_alg;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          scatter;

  /* Process environment variables and setup globals */
  if(argc > 1 && argv[1][0] == '-' && (argv[1][1] == 'h' || argv[1][1] == 'H') ) setup_env_internal(1);
  else setup_env();

  if(begin_test() != PAMI_SUCCESS)
    return 1;

  assert(gNum_contexts > 0);
  context = (pami_context_t*)malloc(sizeof(pami_context_t) * gNum_contexts);

  /* \note Test environment variable" TEST_ROOT=N, defaults to 0.*/
  char* sRoot = getenv("TEST_ROOT");
  int root = 0;
  /* Override ROOT */
  if(sRoot) root = atoi(sRoot);


  /*  Initialize PAMI */
  int rc = pami_init(&client,        /* Client             */
                     context,        /* Context            */
                     NULL,           /* Clientname=default */
                     &gNum_contexts, /* gNum_contexts       */
                     NULL,           /* null configuration */
                     0,              /* no configuration   */
                     &task_id,       /* task id            */
                     &num_tasks);    /* number of tasks    */

  if(rc != PAMI_SUCCESS)
    return 1;

  /*  Allocate buffer(s) */
  int err = 0;
  void* buf = NULL;

  if(task_id == root)
  {
    err = posix_memalign(&buf, 128, (gMax_byte_count * num_tasks) + gBuffer_offset);
    assert(err == 0);
    buf = (char*)buf + gBuffer_offset;
  }

  void* rbuf = NULL;
  err = posix_memalign(&rbuf, 128, gMax_byte_count + gBuffer_offset);
  assert(err == 0);
  rbuf = (char*)rbuf + gBuffer_offset;


  unsigned iContext = 0;

  for(; iContext < gNum_contexts; ++iContext)
  {

    if(task_id == 0)
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

    if(rc != PAMI_SUCCESS)
      return 1;

    /*  Query the world geometry for scatter algorithms */
    rc |= query_geometry_world(client,
                               context[iContext],
                               &world_geometry,
                               scatter_xfer,
                               scatter_num_algorithm,
                               &scatter_always_works_algo,
                               &scatter_always_works_md,
                               &scatter_must_query_algo,
                               &scatter_must_query_md);

    if(rc != PAMI_SUCCESS)
      return 1;

    barrier.cb_done   = cb_done;
    barrier.cookie    = (void*) & bar_poll_flag;
    barrier.algorithm = bar_always_works_algo[0];
    blocking_coll(context[iContext], &barrier, &bar_poll_flag);

    total_alg = scatter_num_algorithm[0]+scatter_num_algorithm[1];
    for(nalg = 0; nalg < total_alg; nalg++)
    {
      metadata_result_t result = {0};
      unsigned barrier_required = 0; /* assume no asyncflowctl is needed. */
      unsigned sleep_required = (gAsync_skew && (task_id == num_tasks-1)) ? gAsync_skew : 0; /* Only one rank (highest) sleeps if requested. */
      unsigned query_protocol;
      if(nalg < scatter_num_algorithm[0])
      {
        query_protocol = 0;
        next_algo = &scatter_always_works_algo[nalg];
        next_md  = &scatter_always_works_md[nalg];
      }
      else
      {
        query_protocol = 1;
        next_algo = &scatter_must_query_algo[nalg-scatter_num_algorithm[0]];
        next_md  = &scatter_must_query_md[nalg-scatter_num_algorithm[0]];
      }

      pami_task_t root = 0;
      pami_endpoint_t root_ep;
      PAMI_Endpoint_create(client, root, 0, &root_ep);
      scatter.cmd.xfer_scatter.root       = root_ep;

      scatter.cb_done    = cb_done;
      scatter.cookie     = (void*) & scatter_poll_flag;
      scatter.algorithm  = *next_algo;
      scatter.cmd.xfer_scatter.stype      = PAMI_TYPE_BYTE;
      scatter.cmd.xfer_scatter.stypecount = 0;
      scatter.cmd.xfer_scatter.rtype      = PAMI_TYPE_BYTE;
      scatter.cmd.xfer_scatter.rtypecount = 0;

      gProtocolName = next_md->name;

      if(task_id == root)
      {
        printf("# Scatter Bandwidth Test(size:%zu) -- context = %d, protocol: %s, Metadata: range %zu <-> %zd, mask %#X\n",num_tasks,
               iContext, gProtocolName,
               next_md->range_lo,(ssize_t)next_md->range_hi,
               next_md->check_correct.bitmask_correct);
        printf("# Size(bytes)      iterations     bytes/sec      usec\n");
        printf("# -----------      -----------    -----------    ---------\n");
      }

      if(((strstr(next_md->name, gSelected) == NULL) && gSelector) ||
         ((strstr(next_md->name, gSelected) != NULL) && !gSelector))  continue;

      if((gVerbose>1) && sleep_required)
        fprintf(stderr,"gAsync_skew -- sleep(%d)\n",sleep_required);

      int i, j;

      unsigned checkrequired = next_md->check_correct.values.checkrequired; /*must query every time */
      assert(!checkrequired || next_md->check_fn); /* must have function if checkrequired. */

      int dt,op=4/*SUM*/;

      for(dt = 0; dt < dt_count; dt++)
      {
        if((gFull_test && ((dt != DT_NULL) && (dt != DT_BYTE))) || gValidTable[op][dt])
        {
          if(task_id == 0)
            printf("Running scatter: %s\n", dt_array_str[dt]);

          for( i = gMin_byte_count? MAX(1,gMin_byte_count/get_type_size(dt_array[dt])) : 0; /*clumsy, only want 0 if hardcoded to 0, othersize min 1 */
             i <= gMax_byte_count/get_type_size(dt_array[dt]); 
             i = i ? i*2 : 1 /* handle zero min */)
          {
            size_t dataSent = i * get_type_size(dt_array[dt]);
            int          niter;

            if(dataSent < CUTOFF)
              niter = gNiterlat;
            else
              niter = NITERBW;

            scatter.cmd.xfer_scatter.stypecount = i;
            scatter.cmd.xfer_scatter.stype      = dt_array[dt];
            scatter.cmd.xfer_scatter.rtypecount = i;
            scatter.cmd.xfer_scatter.rtype      = dt_array[dt];



            if(query_protocol)
            {
              size_t sz=get_type_size(dt_array[dt])*i;
              /* Must initialize all of cmd for metadata */
              scatter.cmd.xfer_scatter.sndbuf     = buf;
              scatter.cmd.xfer_scatter.rcvbuf     = rbuf;
              result = check_metadata(*next_md,
                                      scatter,
                                      dt_array[dt],
                                      sz, /* metadata uses bytes i, */
                                      buf,
                                      dt_array[dt],
                                      sz,
                                      rbuf);
              if(next_md->check_correct.values.nonlocal)
              {
                /* \note We currently ignore check_correct.values.nonlocal
                        because these tests should not have nonlocal differences (so far). */
                result.check.nonlocal = 0;
              }

              if(result.bitmask) continue;
              /* Honor asyncflowctl with a barrier before every bcast */
              if(gAsync_flow_ctl &&  next_md->check_correct.values.asyncflowctl) barrier_required = 1;
            }

            /* Do one 'in-place' collective and validate it */
            if(gTestMpiInPlace && (!query_protocol || (query_protocol && next_md->check_correct.values.inplace)))
            {
              if(task_id == root)
              {
                scatter.cmd.xfer_scatter.sndbuf     = buf;
                scatter.cmd.xfer_scatter.rcvbuf     = PAMI_IN_PLACE;
                scatter_initialize_sndbuf_dt (buf, i, num_tasks, dt);
              }
              else
              {
                scatter.cmd.xfer_scatter.sndbuf     = buf;
                scatter.cmd.xfer_scatter.rcvbuf     = rbuf;
                memset(rbuf, 0xFF, i);
              }
              if(checkrequired) /* must query every time */
              {
                result = next_md->check_fn(&scatter);
                if(result.bitmask) continue;
              }
              blocking_coll(context[iContext], &scatter, &scatter_poll_flag);
              int rc_check;
              rc |= rc_check = scatter_check_rcvbuf_dt ((task_id==root)?(char*)buf + dataSent*task_id:rbuf, i, task_id, dt);

              if(rc_check) fprintf(stderr, "%s FAILED IN PLACE validation on %s\n", gProtocolName, dt_array_str[dt]);
            }
            else if(gTestMpiInPlace && gVerbose>=2 && (task_id == 0)) printf("%s does not support IN PLACE buffering\n", gProtocolName);

            /* Iterate (and time) with separate buffers, not in-place */
            scatter.cmd.xfer_scatter.sndbuf     = buf;
            scatter.cmd.xfer_scatter.rcvbuf     = rbuf;

            if(task_id == root)
              scatter_initialize_sndbuf_dt (buf, i, num_tasks, dt);

            memset(rbuf, 0xFF, i);

            blocking_coll(context[iContext], &barrier, &bar_poll_flag);
            ti = timer();

            for(j = 0; j < niter; j++)
            {
              if(checkrequired) /* must query every time */
              {
                result = next_md->check_fn(&scatter);
                if(result.bitmask) continue;
              }
              if((gVerbose>1) && barrier_required && (task_id == root) && (j==0))
                fprintf(stderr,"gAsync_flow_ctl -- barrier required\n");
              if(barrier_required)
                blocking_coll(context[iContext], &barrier, &bar_poll_flag);
              if(sleep_required && (j==0))
                sleep(sleep_required);
              blocking_coll(context[iContext], &scatter, &scatter_poll_flag);
            }

            blocking_coll(context[iContext], &barrier, &bar_poll_flag);
            tf = timer();
            int rc_check;
            rc |= rc_check = scatter_check_rcvbuf_dt (rbuf, i, task_id, dt);

            if(rc_check) fprintf(stderr, "%s FAILED validation on %s\n", gProtocolName, dt_array_str[dt]);

            usec = (tf - ti) / (double)niter;

            if(task_id == root)
            {
              printf("  %11lld %16d %14.1f %12.2f\n",
                     (long long)dataSent,
                     niter,
                     (double)1e6*(double)dataSent / (double)usec,
                     usec);
              fflush(stdout);
            }
            /* Iteration based validations.  Write something different (and validate) every iteration*/
            for(j = 0; j < niter; j++)
            {
              if(checkrequired) /* must query every time */
              {
                result = next_md->check_fn(&scatter);
                if(result.bitmask) continue;
              }
              /* Iteration data validation */
              if(task_id == root)
                memset(buf,(char)j,dataSent*num_tasks);
              if(barrier_required)
                blocking_coll(context[iContext], &barrier, &bar_poll_flag);
              if(sleep_required && (j==0))
                sleep(sleep_required);
              blocking_coll(context[iContext], &scatter, &scatter_poll_flag);
              /* Iteration data validation */
              if((task_id != root) && (((char*)rbuf)[0]!=(char)j))
              {
                rc |= 1;
                fprintf(stderr, "%s(%d) FAILED validation on %s on iteration %d with buf[%d]=%#2.2X \n", gProtocolName, i, dt_array_str[dt],j,0,((char*)rbuf)[0]);
              }
            }
          }
        }
      }
    }
    free(bar_always_works_algo);
    free(bar_always_works_md);
    free(bar_must_query_algo);
    free(bar_must_query_md);
    free(scatter_always_works_algo);
    free(scatter_always_works_md);
    free(scatter_must_query_algo);
    free(scatter_must_query_md);

  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

  if(task_id == root)
  {
    buf = (char*)buf - gBuffer_offset;
    free(buf);
  }

  rbuf = (char*)rbuf - gBuffer_offset;
  free(rbuf);

  rc |= pami_shutdown(&client, context, &gNum_contexts);
  return rc;
}
