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
 * \file test/api/collectives/bcast_contig.c
 * \brief Simple Bcast test on world geometry with contiguous datatypes
 */

/* Use arg -h or see setup_env() for environment variable overrides  */
/* This test allocates COUNT*16 unlike other bcast's,so reduce the default*/
#define COUNT     (1048576*8/16)   /* see envvar TEST_BYTES for overrides */

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

  /* Bcast variables */
  size_t               bcast_num_algorithm[2];
  pami_algorithm_t    *next_algo = NULL;
  pami_metadata_t     *next_md= NULL;
  pami_algorithm_t    *bcast_always_works_algo = NULL;
  pami_metadata_t     *bcast_always_works_md   = NULL;
  pami_algorithm_t    *bcast_must_query_algo   = NULL;
  pami_metadata_t     *bcast_must_query_md     = NULL;
  pami_xfer_type_t     bcast_xfer = PAMI_XFER_BROADCAST;
  volatile unsigned    bcast_poll_flag = 0;

  int                  nalg= 0, total_alg;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          broadcast;

  /* Process environment variables and setup globals */
  if(argc > 1 && argv[1][0] == '-' && (argv[1][1] == 'h' || argv[1][1] == 'H') ) setup_env_internal(1);
  else setup_env();

  if(begin_test() != PAMI_SUCCESS)
    return 1;

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

  if (rc != PAMI_SUCCESS)
    return 1;

  if (gNumRoots > num_tasks) gNumRoots = num_tasks;

  /*  Allocate buffer(s) */
  int err = 0;
  void* buf = NULL;
  err = posix_memalign(&buf, 128, gMax_byte_count + gBuffer_offset);
  assert(err == 0);
  buf = (char*)buf + gBuffer_offset;


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

    if (rc != PAMI_SUCCESS)
    return 1;

    int o;
    for(o = -1; o <= gOptimize ; o++) /* -1 = default, 0 = de-optimize, 1 = optimize */
    {

      pami_configuration_t configuration[1];
      configuration[0].name = PAMI_GEOMETRY_OPTIMIZE;
      configuration[0].value.intval = o; /* de/optimize */
      if(o == -1) ; /* skip update, use defaults */
      else
        rc |= update_geometry(client,
                              context[iContext],
                              world_geometry,
                              configuration,
                              1);

      if (rc != PAMI_SUCCESS)
      return 1;

    /*  Query the world geometry for broadcast algorithms */
    rc |= query_geometry_world(client,
                               context[iContext],
                               &world_geometry,
                               bcast_xfer,
                               bcast_num_algorithm,
                               &bcast_always_works_algo,
                               &bcast_always_works_md,
                               &bcast_must_query_algo,
                               &bcast_must_query_md);

      if (rc != PAMI_SUCCESS)
      return 1;

    barrier.cb_done   = cb_done;
    barrier.cookie    = (void*) & bar_poll_flag;
    barrier.algorithm = bar_always_works_algo[0];
    blocking_coll(context[iContext], &barrier, &bar_poll_flag);

    total_alg = bcast_num_algorithm[0]+bcast_num_algorithm[1];
    for (nalg = 0; nalg < total_alg; nalg++)
    {
      metadata_result_t result = {0};
      unsigned barrier_required = 0; /* assume no asyncflowctl is needed. */
      unsigned sleep_required = (gAsync_skew && (task_id == num_tasks-1)) ? gAsync_skew : 0; /* Only one rank (highest) sleeps if requested. */
      unsigned query_protocol;
      if(nalg < bcast_num_algorithm[0])
      {  
        query_protocol = 0;
        next_algo = &bcast_always_works_algo[nalg];
        next_md  = &bcast_always_works_md[nalg];
      }
      else
      {  
        query_protocol = 1;
        next_algo = &bcast_must_query_algo[nalg-bcast_num_algorithm[0]];
        next_md  = &bcast_must_query_md[nalg-bcast_num_algorithm[0]];
      }
      broadcast.cb_done                      = cb_done;
      broadcast.cookie                       = (void*) & bcast_poll_flag;
      broadcast.algorithm                    = *next_algo;
      broadcast.cmd.xfer_broadcast.buf       = buf;
      broadcast.cmd.xfer_broadcast.type      = PAMI_TYPE_BYTE;
      broadcast.cmd.xfer_broadcast.typecount = 0;

      gProtocolName = next_md->name;

      int k;
      for (k=0; k< gNumRoots; k++)
      {
        pami_endpoint_t    root_ep;
        pami_task_t root_task = (pami_task_t)k;
        PAMI_Endpoint_create(client, root_task, 0, &root_ep);
        broadcast.cmd.xfer_broadcast.root = root_ep;
        if (task_id == root_task)
        {
          printf("# Broadcast Bandwidth Test(size:%zu) -- context = %d, root = %d  protocol: %s, Metadata: range %zu <-> %zd, mask %#X\n",num_tasks,
                 iContext, root_task, gProtocolName,
                 next_md->range_lo,(ssize_t)next_md->range_hi,
                 next_md->check_correct.bitmask_correct);
          printf("# Size(bytes)      iterations     bytes/sec      usec\n");
          printf("# -----------      -----------    -----------    ---------\n");
        }
        if (((strstr(next_md->name, gSelected) == NULL) && gSelector) ||
            ((strstr(next_md->name, gSelected) != NULL) && !gSelector))  continue;

        if((gVerbose>1) && sleep_required)
          fprintf(stderr,"gAsync_skew -- sleep(%d)\n",sleep_required);

        int i, j;

        unsigned checkrequired = next_md->check_correct.values.checkrequired; /*must query every time */
        assert(!checkrequired || next_md->check_fn); /* must have function if checkrequired. */

        int dt,op=4/*SUM*/;

        for (dt = 0; dt < dt_count; dt++)
        {
          if ((gFull_test && ((dt != DT_NULL) && (dt != DT_BYTE))) || gValidTable[op][dt])
          {
              if (task_id == 0)
                printf("Running Broadcast: %s\n", dt_array_str[dt]);

              for (i = MAX(1,gMin_byte_count/get_type_size(dt_array[dt])); i <= gMax_byte_count/get_type_size(dt_array[dt]); i *= 2)
              {
                size_t dataSent = i * get_type_size(dt_array[dt]);
                int          niter;

                if (dataSent < CUTOFF)
                  niter = gNiterlat;
                else
                  niter = NITERBW;

                broadcast.cmd.xfer_broadcast.typecount = i;
                broadcast.cmd.xfer_broadcast.type      = dt_array[dt];

                if (task_id == root_task)
                  bcast_initialize_sndbuf_dt (buf, i, root_task, dt);
                else
                  memset(buf, 0xFF, i);
                if(query_protocol)
                {  
                  size_t sz=get_type_size(dt_array[dt])*i;
                /* Must initialize all of cmd for metadata */
                  result = check_metadata(*next_md,
                                          broadcast,
                                          dt_array[dt],
                                          sz, /* metadata uses bytes i, */
                                          broadcast.cmd.xfer_broadcast.buf,
                                          PAMI_TYPE_BYTE,
                                          sz,
                                          broadcast.cmd.xfer_broadcast.buf);
                  if (next_md->check_correct.values.nonlocal)
                  {
                    /* \note We currently ignore check_correct.values.nonlocal
                      because these tests should not have nonlocal differences (so far). */
                    result.check.nonlocal = 0;
                  }

                  if (result.bitmask) continue;
                  /* Honor asyncflowctl with a barrier before every bcast */
                  if(gAsync_flow_ctl &&  next_md->check_correct.values.asyncflowctl) barrier_required = 1; 
                }
                blocking_coll(context[iContext], &barrier, &bar_poll_flag);
                ti = timer();

                for (j = 0; j < niter; j++)
                {
                  if (checkrequired) /* must query every time */
                  {
                    result = next_md->check_fn(&broadcast);
                    if (result.bitmask) continue;
                  }
                  /* Iteration data validation */
                  if (task_id == root_task && j == (niter-1))
                    bcast_initialize_sndbuf_dt (buf, i, root_task, dt);
                  else ((char*)buf)[0]=(char)j;
                  if((gVerbose>1) && barrier_required && (task_id == root_task) && (j==0))
                    fprintf(stderr,"gAsync_flow_ctl -- barrier required\n");
                  if(barrier_required)
                    blocking_coll(context[iContext], &barrier, &bar_poll_flag);
                  if(sleep_required && (j==0))
                    sleep(sleep_required);
                  blocking_coll (context[iContext], &broadcast, &bcast_poll_flag);
                  /* Iteration data validation */
                  if (task_id != root_task && j < (niter-1) && (((char*)buf)[0]!=(char)j))
                  {  
                    rc |= 1;
                    fprintf(stderr, "%s(%d) FAILED validation on %s on iteration %d with %#2.2X \n", gProtocolName, i, dt_array_str[dt],j,((char*)buf)[0]);
                  }
                }

                blocking_coll(context[iContext], &barrier, &bar_poll_flag);
                tf = timer();
                int rc_check;
                rc |= rc_check = bcast_check_rcvbuf_dt (buf, i, root_task, dt);

                if (rc_check) fprintf(stderr, "%s FAILED validation on %s\n", gProtocolName, dt_array_str[dt]);

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
                /* Iteration based validations.  Write something different (and validate) every iteration*/
                for (j = 0; j < niter; j++)
                {
                  if (checkrequired) /* must query every time */
                  {
                    result = next_md->check_fn(&broadcast);
                    if (result.bitmask) continue;
                  }
                  /* Iteration data validation */
                  if (task_id == root_task)
                    ((char*)buf)[0]=(char)j;
                  if(barrier_required)
                    blocking_coll(context[iContext], &barrier, &bar_poll_flag);
                  if(sleep_required && (j==0))
                    sleep(sleep_required);
                  blocking_coll (context[iContext], &broadcast, &bcast_poll_flag);
                  /* Iteration data validation */
                  if ((task_id != root_task) && (((char*)buf)[0]!=(char)j))
                  {  
                    rc |= 1;
                    fprintf(stderr, "%s(%d) FAILED validation on %s on iteration %d with %#2.2X \n", gProtocolName, i, dt_array_str[dt],j,((char*)buf)[0]);
                  }
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
    free(bcast_always_works_algo);
    free(bcast_always_works_md);
    free(bcast_must_query_algo);
    free(bcast_must_query_md);

    } /* optimize loop */
  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

  buf = (char*)buf - gBuffer_offset;
  free(buf);

  rc |= pami_shutdown(&client, context, &gNum_contexts);

  rc |= end_test();

  return rc;
}
