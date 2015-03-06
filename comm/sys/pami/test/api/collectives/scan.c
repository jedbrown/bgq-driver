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
 * \file test/api/collectives/scan.c
 * \brief Simple scan on world geometry
 */

/* see setup_env() for environment variable overrides               */
#define COUNT      65536
#define NITERLAT   10

#include "../pami_util.h"

int main(int argc, char*argv[])
{
  pami_client_t        client;
  pami_context_t      *context;
  pami_task_t          task_id,task_zero=0;
  size_t               num_tasks;
  pami_geometry_t      world_geometry;

  /* Barrier variables */
  size_t               barrier_num_algorithm[2];
  pami_algorithm_t    *bar_always_works_algo = NULL;
  pami_metadata_t     *bar_always_works_md   = NULL;
  pami_algorithm_t    *bar_must_query_algo   = NULL;
  pami_metadata_t     *bar_must_query_md     = NULL;
  pami_xfer_type_t     barrier_xfer = PAMI_XFER_BARRIER;
  volatile unsigned    bar_poll_flag=0;

  /* Scan variables */
  size_t               scan_num_algorithm[2];
  pami_algorithm_t    *scan_always_works_algo = NULL;
  pami_metadata_t     *scan_always_works_md = NULL;
  pami_algorithm_t    *scan_must_query_algo = NULL;
  pami_metadata_t     *scan_must_query_md = NULL;
  pami_xfer_type_t     scan_xfer = PAMI_XFER_SCAN;
  volatile unsigned    scan_poll_flag=0;

  int                  i, j, exclusive, nalg = 0;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          scan;

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

  if (rc==1)
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

  if (rc==1)
    return 1;

  barrier.cb_done   = cb_done;
  barrier.cookie    = (void*) & bar_poll_flag;
  barrier.algorithm = bar_always_works_algo[0];

  unsigned iContext = 0;

  for (; iContext < gNum_contexts; ++iContext)
  {

    if (task_id == 0)
      printf("# Context: %u\n", iContext);

    /*  Query the world geometry for scan algorithms */
    rc |= query_geometry_world(client,
                               context[iContext],
                               &world_geometry,
                               scan_xfer,
                               scan_num_algorithm,
                               &scan_always_works_algo,
                               &scan_always_works_md,
                               &scan_must_query_algo,
                               &scan_must_query_md);
    if (rc==1)
      return 1;

    char * scan_type = "Inclusive";
    for (exclusive = 0; exclusive < 2; exclusive++)
    {
      if(exclusive == 1)
        scan_type = "Exclusive";
      for (nalg = 0; nalg < scan_num_algorithm[0]; nalg++)
      {
        if (task_id == task_zero) /* root not set yet */
        {
          printf("# %s Scan Bandwidth Test(size:%zu) -- context = %d, task_zero = %d protocol: %s\n",
                 scan_type,num_tasks, iContext, task_zero, scan_always_works_md[nalg].name);
          printf("# Size(bytes)      iterations     bytes/sec      usec\n");
          printf("# -----------      -----------    -----------    ---------\n");
        }

        if (((strstr(scan_always_works_md[nalg].name, gSelected) == NULL) && gSelector) ||
            ((strstr(scan_always_works_md[nalg].name, gSelected) != NULL) && !gSelector))  continue;

        gProtocolName = scan_always_works_md[nalg].name;

        scan.cb_done   = cb_done;
        scan.cookie    = (void*)&scan_poll_flag;
        scan.algorithm = scan_always_works_algo[nalg];
        scan.cmd.xfer_scan.sndbuf    = sbuf;
        scan.cmd.xfer_scan.rcvbuf    = rbuf;
        scan.cmd.xfer_scan.rtype     = PAMI_TYPE_BYTE;
        scan.cmd.xfer_scan.rtypecount= 0;
        scan.cmd.xfer_scan.exclusive = exclusive;

        int op, dt;

        for (dt=0; dt<dt_count; dt++)
        {
          for (op=0; op<op_count; op++)
          {
            if (gValidTable[op][dt])
            {
              if (task_id == task_zero)
                printf("Running Scan: %s, %s\n",dt_array_str[dt], op_array_str[op]);
              for (i = MAX(1,gMin_byte_count/get_type_size(dt_array[dt])); i <= gMax_byte_count/get_type_size(dt_array[dt]); i *= 2)
              {
                size_t sz=get_type_size(dt_array[dt]);
                size_t  dataSent = i * sz;
                int niter;

                if (dataSent < CUTOFF)
                  niter = gNiterlat;
                else
                  niter = NITERBW;

                scan.cmd.xfer_scan.stypecount=i;
                scan.cmd.xfer_scan.rtypecount=dataSent;
                scan.cmd.xfer_scan.stype     =dt_array[dt];
                scan.cmd.xfer_scan.op        =op_array[op];

                scan_initialize_sndbuf (sbuf, i, op, dt, task_id);
                memset(rbuf, 0xFF, dataSent);

                /* We aren't testing barrier itself, so use context 0. */
                blocking_coll(context[0], &barrier, &bar_poll_flag);
                ti = timer();
                for (j=0; j<1; j++)
                {
                  blocking_coll(context[iContext], &scan, &scan_poll_flag);
                }
                tf = timer();
                /* We aren't testing barrier itself, so use context 0. */
                blocking_coll(context[0], &barrier, &bar_poll_flag);

                int rc_check;
                rc |= rc_check = scan_check_rcvbuf (rbuf, i, op, dt, num_tasks, task_id, exclusive);

                if (rc_check) fprintf(stderr, "%s scan %s FAILED validation\n", scan_type, gProtocolName);

                usec = (tf - ti)/(double)niter;
                if (task_id == task_zero)
                {
                  printf("  %11lld %16d %14.1f %12.2f\n",
                         (long long)dataSent,
                         niter,
                         (double)1e6*(double)dataSent/(double)usec,
                         usec);
                  fflush(stdout);
                }
              }
            }
          }
        }
      }
    }
    free(scan_always_works_algo);
    free(scan_always_works_md);
    free(scan_must_query_algo);
    free(scan_must_query_md);

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
