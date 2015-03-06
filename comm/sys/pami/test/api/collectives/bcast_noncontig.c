/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* ---------------------------------------------------------------- */
/* (C)Copyright IBM Corp.  2009, 2010                               */
/* IBM CPL License                                                  */
/* ---------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file test/api/collectives/bcast.c
 * \brief Simple Bcast test on world geometry
 */

/* see setup_env() for environment variable overrides               */
#define COUNT     (1048576*8)   /* see envvar TEST_COUNT for overrides */
#define NITERLAT   10

#include "../pami_util.h"

void initialize_sndbuf (void *sbuf, int bytes, int root)
{

  double d = (double)((double)root * 1.0);
  int i = bytes*2;
  double *dbuf = (double *)  sbuf;

  for (; i; i--)
  {
    dbuf[i-1] = d;
    d += 1.0;
  }
}

int check_rcvbuf (void *rbuf, int bytes, int root)
{
  double d = (double)((double)root * 1.0) + 1.0;
  int i = bytes*2;
  double *dbuf = (double *)  rbuf;

  for (; i; i-=2)
  {
    if (dbuf[i-1] != d)
    {
      fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%.2f != %.2f \n", gProtocolName, bytes, rbuf, i - 1, dbuf[i-1], d);
      return 1;
    }

    d+=2.0;
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
  pami_type_t          pami_stype = 0;
  pami_type_t          pami_rtype = 0;
  pami_result_t        ret;

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

  if(gNumRoots > num_tasks) gNumRoots = num_tasks;

  /*  Allocate buffer(s) */
  int err = 0;
  void* buf = NULL;
  err = posix_memalign(&buf, 128, (gMax_byte_count * 2)+ gBuffer_offset);
  assert(err == 0);
  buf = (char*)buf + gBuffer_offset;


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

    if (rc == 1)
      return 1;

    barrier.cb_done   = cb_done;
    barrier.cookie    = (void*) & bar_poll_flag;
    barrier.algorithm = bar_always_works_algo[0];
    blocking_coll(context[iContext], &barrier, &bar_poll_flag);

    total_alg = bcast_num_algorithm[0]+bcast_num_algorithm[1];
    for (nalg = 0; nalg < total_alg; nalg++)
    {
      metadata_result_t result = {0};
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
      broadcast.cmd.xfer_broadcast.type      = pami_stype;
      broadcast.cmd.xfer_broadcast.typecount = 0;

      gProtocolName = next_md->name;

      int k;
      for (k=0; k< gNumRoots; k++)
      {
        pami_endpoint_t    root_ep;
        pami_task_t root_task = (pami_task_t)k;
        PAMI_Endpoint_create(client, root_task, 0, &root_ep);
        broadcast.cmd.xfer_broadcast.root = root_ep;
        broadcast.cmd.xfer_broadcast.type = (task_id == root_task) ? pami_stype : pami_rtype;
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

        int i, j;

        unsigned checkrequired = next_md->check_correct.values.checkrequired; /*must query every time */
        assert(!checkrequired || next_md->check_fn); /* must have function if checkrequired. */

        for (i = 0; i <= gMax_byte_count/sizeof(double); i *= 2)
        {
          size_t  dataSent = i;
          int          niter;

          if (dataSent < CUTOFF)
            niter = gNiterlat;
          else
            niter = NITERBW;

          broadcast.cmd.xfer_broadcast.typecount = i;

          if (task_id == root_task)
            initialize_sndbuf (buf, i, root_task);
          else
            memset(buf, 0xFF, i*sizeof(double)*2);
          if(query_protocol)
          {
            result = check_metadata(*next_md,
                                    broadcast,
                                    pami_stype,
                                    dataSent, /* metadata uses bytes i, */
                                    broadcast.cmd.xfer_broadcast.buf,
                                    PAMI_TYPE_BYTE,
                                    dataSent,
                                    broadcast.cmd.xfer_broadcast.buf);
            if (next_md->check_correct.values.nonlocal)
            {
              /* \note We currently ignore check_correct.values.nonlocal
                        because these tests should not have nonlocal differences (so far). */
              result.check.nonlocal = 0;
            }

            if (result.bitmask)
            {
              if(!i)i++; /* Can't continue the for loop without this */
              continue;
            }
          }

          blocking_coll(context[iContext], &barrier, &bar_poll_flag);
          ti = timer();

          for (j = 0; j < niter; j++)
          {
                  if (checkrequired) /* must query every time */
                  {
                    result = next_md->check_fn(&broadcast);
                    if (result.bitmask) 
                    {
                      if(!i)i++;
                      continue;
                    }
                  }
            blocking_coll (context[iContext], &broadcast, &bcast_poll_flag);
          }

          blocking_coll(context[iContext], &barrier, &bar_poll_flag);
          tf = timer();
          int rc_check=0;
          if(task_id != root_task)
            rc |= rc_check = check_rcvbuf (buf, i, root_task);

          if (rc_check) fprintf(stderr, "%s FAILED validation\n", gProtocolName);

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
          if(!i)i++;
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

  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

  buf = (char*)buf - gBuffer_offset;
  free(buf);

  rc |= pami_shutdown(&client, context, &gNum_contexts);
  return rc;
}
