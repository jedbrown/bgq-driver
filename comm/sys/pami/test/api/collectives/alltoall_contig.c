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
 * \file test/api/collectives/alltoall_contig.c
 * \brief Simple Alltoall test on world geometry with contiguous datatypes
 */

/* Use arg -h or see setup_env() for environment variable overrides  */

#define COUNT     (4096)

#include "../pami_util.h"

void *sbuf = NULL;
void *rbuf = NULL;


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
  pami_algorithm_t    *next_algo = NULL;
  pami_metadata_t     *next_md= NULL;
  pami_algorithm_t    *alltoall_always_works_algo = NULL;
  pami_metadata_t     *alltoall_always_works_md = NULL;
  pami_algorithm_t    *alltoall_must_query_algo = NULL;
  pami_metadata_t     *alltoall_must_query_md = NULL;
  pami_xfer_type_t     alltoall_xfer = PAMI_XFER_ALLTOALL;
  volatile unsigned    alltoall_poll_flag = 0;

  int                  nalg= 0, total_alg;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          alltoall;

  /* Process environment variables and setup globals */
  if(argc > 1 && argv[1][0] == '-' && (argv[1][1] == 'h' || argv[1][1] == 'H') ) setup_env_internal(1);
  else setup_env();

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

      if (rc != PAMI_SUCCESS)
      return 1;

    barrier.cb_done   = cb_done;
    barrier.cookie    = (void*) & bar_poll_flag;
    barrier.algorithm = bar_always_works_algo[0];

    alltoall.cb_done    = cb_done;
    alltoall.cookie     = (void*) & alltoall_poll_flag;


    total_alg = alltoall_num_algorithm[0]+alltoall_num_algorithm[1];
    for (nalg = 0; nalg < total_alg; nalg++)
    {
      size_t i, j;
      metadata_result_t result = {0};
      unsigned query_protocol;
      if(nalg < alltoall_num_algorithm[0])
      {  
        query_protocol = 0;
        next_algo = &alltoall_always_works_algo[nalg];
        next_md  = &alltoall_always_works_md[nalg];
      }
      else
      {  
        query_protocol = 1;
        next_algo = &alltoall_must_query_algo[nalg-alltoall_num_algorithm[0]];
        next_md  = &alltoall_must_query_md[nalg-alltoall_num_algorithm[0]];
      }

      gProtocolName = next_md->name;

      if (task_id == 0)
      {
        printf("# Alltoall Bandwidth Test(size:%zu) -- context = %d, optimize = %d, protocol: %s, Metadata: range %zu <-> %zd, mask %#X\n",num_tasks,
               iContext, o, gProtocolName,
               next_md->range_lo,(ssize_t)next_md->range_hi,
               next_md->check_correct.bitmask_correct);
        printf("# Size(bytes)  iterations    bytes/sec      usec\n");
        printf("# -----------      -----------    -----------    ---------\n");
      }

      if (((strstr(next_md->name, gSelected) == NULL) && gSelector) ||
          ((strstr(next_md->name, gSelected) != NULL) && !gSelector))  continue;

      alltoall.algorithm  = *next_algo;

      unsigned checkrequired = next_md->check_correct.values.checkrequired; /*must query every time */
      assert(!checkrequired || next_md->check_fn); /* must have function if checkrequired. */

      int dt,op=4/*SUM*/;

      for (dt = 0; dt < dt_count; dt++)
      {
          if ((gFull_test && ((dt != DT_NULL) && (dt != DT_BYTE))) || gValidTable[op][dt])
          {
            if (task_id == 0)
              printf("Running Alltoall: %s\n", dt_array_str[dt]);

            for ( i = gMin_byte_count? MAX(1,gMin_byte_count/get_type_size(dt_array[dt])) : 0; /*clumsy, only want 0 if hardcoded to 0, othersize min 1 */
                  i <= gMax_byte_count/get_type_size(dt_array[dt]); 
                  i = i ? i*2 : 1 /* handle zero min */)
            {
              size_t dataSent = i * get_type_size(dt_array[dt]);
              int          niter;

              if (dataSent < CUTOFF)
                niter = gNiterlat;
              else
                niter = NITERBW;

              alltoall.cmd.xfer_alltoall.stype         = dt_array[dt];
              alltoall.cmd.xfer_alltoall.stypecount    = i;
              alltoall.cmd.xfer_alltoall.rtype         = dt_array[dt];
              alltoall.cmd.xfer_alltoall.rtypecount    = i;

              if(query_protocol)
              {  
                size_t sz=get_type_size(dt_array[dt])*i;
                /* Must initialize all of cmd for metadata */
                alltoall.cmd.xfer_alltoall.sndbuf        = sbuf;
                alltoall.cmd.xfer_alltoall.rcvbuf        = rbuf;
                result = check_metadata(*next_md,
                                        alltoall,
                                        dt_array[dt],
                                        sz, /* metadata uses bytes i, */
                                        sbuf,
                                        dt_array[dt],
                                        sz,
                                        rbuf);
                if (next_md->check_correct.values.nonlocal)
                {
                  /* \note We currently ignore check_correct.values.nonlocal
                        because these tests should not have nonlocal differences (so far). */
                  result.check.nonlocal = 0;
                }

                if (result.bitmask) continue;
              }
              /* Do one 'in-place' collective and validate it */
              if(gTestMpiInPlace && (!query_protocol || (query_protocol && next_md->check_correct.values.inplace)))
              {
                for (j = 0; j < num_tasks; j++)
                {
                  alltoall_initialize_bufs_dt(sbuf, rbuf, i, j, dt); /*rbuf not used here*/
                }
                alltoall.cmd.xfer_alltoall.sndbuf        = PAMI_IN_PLACE;
                alltoall.cmd.xfer_alltoall.rcvbuf        = sbuf;
                if (checkrequired) /* must query every time */
                {
                  result = next_md->check_fn(&alltoall);
                  if (result.bitmask) continue;
                }
                blocking_coll(context[iContext], &alltoall, &alltoall_poll_flag);
                int rc_check;
                rc |= rc_check = alltoall_check_rcvbuf_dt(sbuf, i, num_tasks, task_id, dt);
                
                if (rc_check) fprintf(stderr, "%s FAILED IN PLACE validation on %s\n", gProtocolName, dt_array_str[dt]);
              }
              else if(gTestMpiInPlace && gVerbose>=2 && (task_id == 0)) printf("%s does not support IN PLACE buffering\n", gProtocolName);
  
              /* Iterate (and time) with separate buffers, not in-place */
              for (j = 0; j < num_tasks; j++)
              {
                alltoall_initialize_bufs_dt(sbuf, rbuf, i, j, dt);
              }
              alltoall.cmd.xfer_alltoall.sndbuf        = sbuf;
              alltoall.cmd.xfer_alltoall.rcvbuf        = rbuf;
  
              blocking_coll(context[iContext], &barrier, &bar_poll_flag);

              ti = timer();

              for (j = 0; j < niter; j++)
              {
                if (checkrequired) /* must query every time */
                {
                  result = next_md->check_fn(&alltoall);
                  if (result.bitmask) continue;
                }
                blocking_coll(context[iContext], &alltoall, &alltoall_poll_flag);
              }

              tf = timer();


              int rc_check;
              rc |= rc_check = alltoall_check_rcvbuf_dt(rbuf, i, num_tasks, task_id, dt);
              if (rc_check) fprintf(stderr, "%s FAILED validation on %s\n", gProtocolName, dt_array_str[dt]);

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
    } /* optimize loop */
  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

  sbuf = (char*)sbuf - gBuffer_offset;
  free(sbuf);

  rbuf = (char*)rbuf - gBuffer_offset;
  free(rbuf);

  return rc;
}
