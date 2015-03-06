/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* ---------------------------------------------------------------- */
/* (C)Copyright IBM Corp.  2009, 2010                               */
/* IBM CPL License                                                  */
/* ---------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file test/api/collectives/scatter.c
 * \brief Simple Scatter test on world geometry (only scatters bytes)
 */

/* see setup_env() for environment variable overrides               */
#define COUNT     (524288)

#include "../pami_util.h"

void initialize_sndbuf (void *sbuf, int counts, size_t ntasks)
{
  size_t i,j;
  double *dbuf = (double *)  sbuf;

  for (i = 0; i < ntasks; i++)
  {
    double d = (double)((double)i * 1.0);
    for(j = 0; j < counts*2; j++)
     dbuf[i*counts*2+j] = d;
  }
}

int check_rcvbuf (void *rbuf, int counts, pami_task_t task)
{
  int i;
  double *dbuf = (double *)  rbuf;

  double d = (double)((double)task * 1.0);

  for (i = 0; i < counts*2; i+=2)
    if (dbuf[i+1] != d)
    {
      fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%.2f != %.2f \n", gProtocolName, counts, dbuf, i, dbuf[i], d);
      return 1;
    }
  return 0;
}

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
  pami_type_t          pami_stype = 0;
  pami_type_t          pami_rtype = 0;
  pami_result_t        ret;

  /* Process environment variables and setup globals */
  setup_env();

  assert(gNum_contexts > 0);
  context = (pami_context_t*)malloc(sizeof(pami_context_t) * gNum_contexts);

  /* \note Test environment variable" TEST_ROOT=N, defaults to 0.*/
  char* sRoot = getenv("TEST_ROOT");
  int root = 0;
  /* Override ROOT */
  if (sRoot) root = atoi(sRoot);


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

  if (task_id == root)
  {
    err = posix_memalign(&buf, 128, (gMax_byte_count * num_tasks) + gBuffer_offset);
    assert(err == 0);
    buf = (char*)buf + gBuffer_offset;
  }

  void* rbuf = NULL;
  err = posix_memalign(&rbuf, 128, gMax_byte_count + gBuffer_offset);
  assert(err == 0);
  rbuf = (char*)rbuf + gBuffer_offset;

  ret = PAMI_Type_create(&pami_stype);
  if(ret != PAMI_SUCCESS)
    return 1;

  ret = PAMI_Type_create(&pami_rtype);
  if(ret != PAMI_SUCCESS)
    return 1;

  PAMI_Type_add_simple(pami_stype, sizeof(double), 0, 1, sizeof(double)*2);
  PAMI_Type_add_simple(pami_rtype, sizeof(double), sizeof(double), 1, sizeof(double));

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

    if (rc == 1)
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

    if (rc == 1)
      return 1;

    barrier.cb_done   = cb_done;
    barrier.cookie    = (void*) & bar_poll_flag;
    barrier.algorithm = bar_always_works_algo[0];
    blocking_coll(context[iContext], &barrier, &bar_poll_flag);

    total_alg = scatter_num_algorithm[0]+scatter_num_algorithm[1];
    for (nalg = 0; nalg < total_alg; nalg++)
    {
      metadata_result_t result = {0};
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
      scatter.cmd.xfer_scatter.sndbuf     = buf;
      scatter.cmd.xfer_scatter.stype      = pami_stype;
      scatter.cmd.xfer_scatter.stypecount = 0;
      scatter.cmd.xfer_scatter.rcvbuf     = rbuf;
      scatter.cmd.xfer_scatter.rtype      = pami_rtype;
      scatter.cmd.xfer_scatter.rtypecount = 0;

      gProtocolName = next_md->name;

      if (task_id == root)
      {
        printf("# Scatter Bandwidth Test(size:%zu) -- context = %d, protocol: %s, Metadata: range %zu <-> %zd, mask %#X\n",num_tasks,
               iContext, gProtocolName,
               next_md->range_lo,(ssize_t)next_md->range_hi,
               next_md->check_correct.bitmask_correct);
        printf("# Size(bytes)      iterations     bytes/sec      usec\n");
        printf("# -----------      -----------    -----------    ---------\n");
      }

      if (((strstr(next_md->name, gSelected) == NULL) && gSelector) ||
          ((strstr(next_md->name, gSelected) != NULL) && !gSelector))  continue;

      int i, j;

      unsigned checkrequired = next_md->check_correct.values.checkrequired; /*must query every time */
      assert(!checkrequired || next_md->check_fn); /* must have function if checkrequired. */

      for (i = 0; i <= gMax_byte_count/(sizeof(double)*2); i *= 2)
      {
        size_t  dataSent = i;
        int          niter;

        if (dataSent < CUTOFF)
          niter = gNiterlat;
        else
          niter = NITERBW;

        scatter.cmd.xfer_scatter.stypecount = i;
        scatter.cmd.xfer_scatter.rtypecount = i;

        if (task_id == root)
          initialize_sndbuf (buf, i, num_tasks);

        memset(rbuf, 0xFF, i*2*sizeof(double));
            if(query_protocol)
            {
              size_t sz=get_type_size(pami_stype)*i;
              size_t rsz=get_type_size(pami_rtype)*i;
              result = check_metadata(*next_md,
                                      scatter,
                                          pami_stype,
                                      sz, /* metadata uses bytes i, */
                                      scatter.cmd.xfer_scatter.sndbuf,
                                          pami_rtype,
                                      rsz,
                                      scatter.cmd.xfer_scatter.rcvbuf);
              if (next_md->check_correct.values.nonlocal)
              {
                /* \note We currently ignore check_correct.values.nonlocal
                        because these tests should not have nonlocal differences (so far). */
                result.check.nonlocal = 0;
              }

              if (result.bitmask) 
              {
                if(!i)i++;
                continue;
              }
            }

        blocking_coll(context[iContext], &barrier, &bar_poll_flag);
        ti = timer();

        for (j = 0; j < niter; j++)
        {
              if (checkrequired) /* must query every time */
              {
                result = next_md->check_fn(&scatter);
                if (result.bitmask) 
                {
                  if(!i)i++;
                  continue;
                }
              }
          blocking_coll(context[iContext], &scatter, &scatter_poll_flag);
        }

        tf = timer();
        blocking_coll(context[iContext], &barrier, &bar_poll_flag);

        int rc_check;
        rc |= rc_check = check_rcvbuf (rbuf, i, task_id);

        if (rc_check) fprintf(stderr, "%s FAILED validation\n", gProtocolName);

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
        if(!i)i++;
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

  if (task_id == root)
  {
    buf = (char*)buf - gBuffer_offset;
    free(buf);
  }

  rbuf = (char*)rbuf - gBuffer_offset;
  free(rbuf);

  rc |= pami_shutdown(&client, context, &gNum_contexts);
  return rc;
}
