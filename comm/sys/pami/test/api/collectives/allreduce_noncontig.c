/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* ---------------------------------------------------------------- */
/* (C)Copyright IBM Corp.  2009, 2010                               */
/* IBM CPL License                                                  */
/* ---------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file test/api/collectives/allreduce_noncontig.c
 * \brief Simple Allreduce on world geometry with non-contiguous datatypes
 */

/* see setup_env() for environment variable overrides               */
#include "../pami_util.h"

void initialize_sndbuf(void *buf, int count, int task_id, int num_tasks)
{

  int i;

  double *dbuf = (double *)  buf;

  memset(buf, 0, count * sizeof(double) * 2);
  for (i = 0; i < count * 2; i++)
  {
    dbuf[i] = 1.0 * i;
  }
}

int check_rcvbuf(void *buf, int count, int task_id, int num_tasks)
{

  int i;

  int err = 0;

  double *rcvbuf = (double *)  buf;

    for (i = 0; i < count * 2; i+=2)
    {
      if (rcvbuf[i+1] != 1.0 * i * num_tasks)
      {
        fprintf(stderr, "%s:Check DATA_SUM(%d) failed rcvbuf[%d] %f != %f\n", gProtocolName, count, i+1, rcvbuf[i+1], (double)1.0*i*num_tasks);
        err = -1;
        return err;
      }

    }
  /*for (i = 0; i < count * 2; i++)
    fprintf(stderr, "rcvbuf[%d] %f != %f\n",i, rcvbuf[i], (double)1.0*i*num_tasks);*/

  return err;
}


int main(int argc, char*argv[])
{
  pami_client_t        client;
  pami_context_t      *context;
  pami_task_t          task_id, task_zero = 0;
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

  /* Allreduce variables */
  size_t               allreduce_num_algorithm[2];
  pami_algorithm_t    *allreduce_always_works_algo = NULL;
  pami_metadata_t     *allreduce_always_works_md = NULL;
  pami_algorithm_t    *allreduce_must_query_algo = NULL;
  pami_metadata_t     *allreduce_must_query_md = NULL;
  pami_xfer_type_t     allreduce_xfer = PAMI_XFER_ALLREDUCE;
  volatile unsigned    allreduce_poll_flag = 0;

  int                  i, j, nalg = 0;
  double               ti, tf, usec;
  pami_xfer_t          barrier;
  pami_xfer_t          allreduce;
  pami_type_t          pami_stype = 0;
  pami_type_t          pami_rtype = 0;
  pami_result_t        ret;

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

  ret = PAMI_Type_create(&pami_stype);
  if(ret != PAMI_SUCCESS)
    return 1;

  ret = PAMI_Type_create(&pami_rtype);
  if(ret != PAMI_SUCCESS)
    return 1;

  PAMI_Type_add_typed(pami_stype, PAMI_TYPE_DOUBLE, 0, 1, sizeof(double)*2);
  PAMI_Type_add_typed(pami_rtype, PAMI_TYPE_DOUBLE, sizeof(double), 1, sizeof(double));

  ret = PAMI_Type_complete(pami_stype, sizeof(double));
  if(ret != PAMI_SUCCESS){
    printf("Invalid atom size for stype\n");
    return 1;
  }

  ret = PAMI_Type_complete(pami_rtype, sizeof(double));
  if(ret != PAMI_SUCCESS){
    printf("Invalid atom size for rtype\n");
    return 1;
  }
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

    /*  Query the world geometry for allreduce algorithms */
    rc |= query_geometry_world(client,
                               context[iContext],
                               &world_geometry,
                               allreduce_xfer,
                               allreduce_num_algorithm,
                               &allreduce_always_works_algo,
                               &allreduce_always_works_md,
                               &allreduce_must_query_algo,
                               &allreduce_must_query_md);

    if (rc == 1)
      return 1;

    for (nalg = 0; nalg < allreduce_num_algorithm[0]; nalg++)
    {
      if (task_id == task_zero)
      {
        printf("# Allreduce Bandwidth Test -- context = %d, protocol: %s\n",
               iContext, allreduce_always_works_md[nalg].name);
        printf("# Size(bytes)           cycles    bytes/sec    usec\n");
        printf("# -----------      -----------    -----------    ---------\n");
      }

      if (((strstr(allreduce_always_works_md[nalg].name, gSelected) == NULL) && gSelector) ||
          ((strstr(allreduce_always_works_md[nalg].name, gSelected) != NULL) && !gSelector))  continue;

      gProtocolName = allreduce_always_works_md[nalg].name;

      allreduce.cb_done   = cb_done;
      allreduce.cookie    = (void*) & allreduce_poll_flag;
      allreduce.algorithm = allreduce_always_works_algo[nalg];
      allreduce.cmd.xfer_allreduce.sndbuf    = sbuf;
      allreduce.cmd.xfer_allreduce.rcvbuf    = rbuf;
      allreduce.cmd.xfer_allreduce.rtype     = PAMI_TYPE_BYTE;
      allreduce.cmd.xfer_allreduce.rtypecount = 0;

      if (task_id == task_zero)
        printf("Running Allreduce: Non-contiguous data type, PAMI_DATA_SUM\n");

      for (i = 1; i <= gMax_byte_count/(sizeof(double)*2); i *= 2)
      {
        size_t sz=sizeof(double)*2;
        size_t  dataSent = i * sz;
        int niter;
        if (dataSent < CUTOFF)
          niter = gNiterlat;
        else
          niter = NITERBW;

        allreduce.cmd.xfer_allreduce.stypecount = i;
        allreduce.cmd.xfer_allreduce.rtypecount = i;
        allreduce.cmd.xfer_allreduce.stype = pami_stype;
        allreduce.cmd.xfer_allreduce.rtype = pami_rtype;
        allreduce.cmd.xfer_allreduce.op    = PAMI_DATA_SUM;

        initialize_sndbuf (sbuf, i, task_id, num_tasks);
        memset(rbuf, 0xFF, i*2*sizeof(double));

        /* We aren't testing barrier itself, so use context 0. */
        blocking_coll(context[0], &barrier, &bar_poll_flag);
        ti = timer();

        for (j = 0; j < niter; j++)
        {
          blocking_coll(context[iContext], &allreduce, &allreduce_poll_flag);
        }

        tf = timer();
        /* We aren't testing barrier itself, so use context 0. */
        blocking_coll(context[0], &barrier, &bar_poll_flag);

        int rc_check;
        rc |= rc_check = check_rcvbuf (rbuf, i, task_id, num_tasks);

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

    free(allreduce_always_works_algo);
    free(allreduce_always_works_md);
    free(allreduce_must_query_algo);
    free(allreduce_must_query_md);

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
