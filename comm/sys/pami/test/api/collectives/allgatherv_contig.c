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
 * \file test/api/collectives/allgatherv_contig.c
 * \brief Simple Allgatherv test on world geometry with contiguous datatypes
 */

/* see setup_env() for environment variable overrides               */
#include "../pami_util.h"

int main (int argc, char ** argv)
{
  pami_client_t        client;
  pami_context_t       context;
  size_t               num_contexts = 1;
  pami_task_t          task_id, task_zero=0;;
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
  pami_algorithm_t    *next_algo = NULL;
  pami_metadata_t     *next_md= NULL;
  pami_algorithm_t    *allgatherv_always_works_algo = NULL;
  pami_metadata_t     *allgatherv_always_works_md = NULL;
  pami_algorithm_t    *allgatherv_must_query_algo = NULL;
  pami_metadata_t     *allgatherv_must_query_md = NULL;
  pami_xfer_type_t     allgatherv_xfer = PAMI_XFER_ALLGATHERV;

  volatile unsigned    allgatherv_poll_flag = 0;

  int                  nalg= 0, total_alg;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          allgatherv;

  setup_env();


  /*  Initialize PAMI */
  int rc = pami_init(&client,        /* Client             */
                     &context,       /* Context            */
                     NULL,           /* Clientname=default */
                     &num_contexts,  /* num_contexts       */
                     NULL,           /* null configuration */
                     0,              /* no configuration   */
                     &task_id,       /* task id            */
                     &num_tasks);    /* number of tasks    */

  if (rc == 1)
    return 1;

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

  /*  Query the world geometry for allgatherv algorithms */
  rc |= query_geometry_world(client,
                            context,
                            &world_geometry,
                            allgatherv_xfer,
                            allgatherv_num_algorithm,
                            &allgatherv_always_works_algo,
                            &allgatherv_always_works_md,
                            &allgatherv_must_query_algo,
                            &allgatherv_must_query_md);

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
  size_t *displs    = (size_t*)malloc(num_tasks * sizeof(size_t));
  barrier.cb_done   = cb_done;
  barrier.cookie    = (void*) & bar_poll_flag;
  barrier.algorithm = bar_always_works_algo[0];
  blocking_coll(context, &barrier, &bar_poll_flag);

  {
    total_alg = allgatherv_num_algorithm[0]+allgatherv_num_algorithm[1];

    for (nalg = 0; nalg < total_alg; nalg++)
    {
      metadata_result_t result = {0};
      unsigned query_protocol;
      if(nalg < allgatherv_num_algorithm[0])
      {
        query_protocol = 0;
        next_algo = &allgatherv_always_works_algo[nalg];
        next_md  = &allgatherv_always_works_md[nalg];
      }
      else
      {
        query_protocol = 1;
        next_algo = &allgatherv_must_query_algo[nalg-allgatherv_num_algorithm[0]];
        next_md  = &allgatherv_must_query_md[nalg-allgatherv_num_algorithm[0]];
      }
      allgatherv.cb_done    = cb_done;
      allgatherv.cookie     = (void*) & allgatherv_poll_flag;
      allgatherv.algorithm  = *next_algo;
      allgatherv.cmd.xfer_allgatherv.sndbuf     = buf;
      allgatherv.cmd.xfer_allgatherv.rcvbuf     = rbuf;
      allgatherv.cmd.xfer_allgatherv.rtypecounts = lengths;
      allgatherv.cmd.xfer_allgatherv.rdispls     = displs;

      gProtocolName = next_md->name;

      if (task_id == 0)
      {
        printf("# Allgatherv Bandwidth Test(size:%zu) -- protocol: %s, Metadata: range %zu <-> %zd, mask %#X\n",num_tasks,
               gProtocolName,
               next_md->range_lo,(ssize_t)next_md->range_hi,
               next_md->check_correct.bitmask_correct);
        printf("# Size(bytes)      iterations     bytes/sec      usec\n");
        printf("# -----------      -----------    -----------    ---------\n");
      }
      if (((strstr(next_md->name, gSelected) == NULL) && gSelector) ||
          ((strstr(next_md->name, gSelected) != NULL) && !gSelector))  continue;

      unsigned checkrequired = next_md->check_correct.values.checkrequired; /*must query every time */
      assert(!checkrequired || next_md->check_fn); /* must have function if checkrequired. */

      unsigned i, j, k;

      int dt,op=4/*SUM*/;

      for (dt = 0; dt < dt_count; dt++)
      {
          if ((gFull_test && ((dt != DT_NULL) && (dt != DT_BYTE))) || gValidTable[op][dt])
          {
            if (task_id == task_zero)
              printf("Running Allgatherv: %s\n", dt_array_str[dt]);

              for (i = MAX(1,gMin_byte_count/get_type_size(dt_array[dt])); i <= gMax_byte_count/get_type_size(dt_array[dt]); i *= 2)
            {
              size_t dataSent = i * get_type_size(dt_array[dt]);
              int          niter;

              if (dataSent < CUTOFF)
                niter = gNiterlat;
              else
                niter = NITERBW;

              for (k = 0; k < num_tasks; k++)lengths[k] = i;
              for (k = 0; k < num_tasks; k++)displs[k]  = k*i;

              allgatherv.cmd.xfer_allgatherv.stypecount       = i;
              allgatherv.cmd.xfer_allgatherv.stype            = dt_array[dt];
              allgatherv.cmd.xfer_allgatherv.rtype            = dt_array[dt];

              gather_initialize_sndbuf_dt (buf, i, task_id, dt);
              memset(rbuf, 0xFF, i);
              if(query_protocol)
              {
                size_t sz=get_type_size(dt_array[dt])*i;
                /* Must initialize all of cmd for metadata */
                result = check_metadata(*next_md,
                                        allgatherv,
                                        dt_array[dt],
                                        sz, /* metadata uses bytes i, */
                                        allgatherv.cmd.xfer_allgatherv.sndbuf,
                                        dt_array[dt],
                                        sz,
                                        allgatherv.cmd.xfer_allgatherv.rcvbuf);
                if (next_md->check_correct.values.nonlocal)
                {
                  /* \note We currently ignore check_correct.values.nonlocal
                    because these tests should not have nonlocal differences (so far). */
                  result.check.nonlocal = 0;
                }

                if (result.bitmask) continue;
              }

              /* Do one 'in-place' collective and validate it */
              {
                memset(rbuf, 0xFF, i);
                allgatherv.cmd.xfer_allgatherv.rcvbuf     = rbuf;
                allgatherv.cmd.xfer_allgatherv.sndbuf     = PAMI_IN_PLACE;
                gather_initialize_sndbuf_dt ((char*)rbuf + dataSent*task_id, i, task_id, dt);
                if (checkrequired) /* must query every time */
                {
                  result = next_md->check_fn(&allgatherv);
                  if (result.bitmask) continue;
                }
                blocking_coll(context, &allgatherv, &allgatherv_poll_flag);
                int rc_check;
                rc |= rc_check = gather_check_rcvbuf_dt (num_tasks, rbuf, i, dt);
  
                if (rc_check) fprintf(stderr, "%s FAILED IN PLACE validation on %s\n", gProtocolName, dt_array_str[dt]);
              }

              /* Iterate (and time) with separate buffers, not in-place */
              allgatherv.cmd.xfer_allgatherv.rcvbuf     = rbuf;
              allgatherv.cmd.xfer_allgatherv.sndbuf     = buf;
              gather_initialize_sndbuf_dt (buf, i, task_id, dt);
              memset(rbuf, 0xFF, i);

              blocking_coll(context, &barrier, &bar_poll_flag);
              ti = timer();
              for (j = 0; j < niter; j++)
              {
                if (checkrequired) /* must query every time */
                {
                  result = next_md->check_fn(&allgatherv);
                  if (result.bitmask) continue;
                }
                blocking_coll(context, &allgatherv, &allgatherv_poll_flag);
              }

              tf = timer();
              blocking_coll(context, &barrier, &bar_poll_flag);

              int rc_check;
              rc |= rc_check = gather_check_rcvbuf_dt (num_tasks, rbuf, i, dt);

              if (rc_check) fprintf(stderr, "%s FAILED validation on %s\n", gProtocolName, dt_array_str[dt]);

              usec = (tf - ti) / (double)niter;

              if (task_id == 0)
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
  rc |= pami_shutdown(&client, &context, &num_contexts);
  free(bar_always_works_algo);
  free(bar_always_works_md);
  free(bar_must_query_algo);
  free(bar_must_query_md);
  free(allgatherv_always_works_algo);
  free(allgatherv_always_works_md);
  free(allgatherv_must_query_algo);
  free(allgatherv_must_query_md);

  return rc;
};
