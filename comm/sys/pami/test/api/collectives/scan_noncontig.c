/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* ---------------------------------------------------------------- */
/* (C)Copyright IBM Corp.  2009, 2010                               */
/* IBM CPL License                                                  */
/* ---------------------------------------------------------------- */
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


void initialize_sndbuf(void *buf, int count, int task_id)
{

  int i;

  double *dbuf = (double *)  buf;

  memset(buf, 0, count * sizeof(double) * 2);
  for (i = 0; i < count * 2; i++)
  {
    dbuf[i] = 1.0 * i;
  }
}

int check_rcvbuf(void *buf, int count, int task_id, int exclusive)
{

  int i, err = 0;
  double *dbuf = (double *)  buf;
  double d;

    if(exclusive == 1)
      {
        /* Receive buffer is not valid on task 0 for exclusive scan */
        if(task_id == 0)
          return 0;
        else
          d = (double)task_id * 1.0;
      }
    else
      {
        d = ((double)task_id * 1.0) + 1.0;
      }
    for (i = 0; i < count * 2; i+=2)
    {
      if (dbuf[i+1] != ((double)i * d))
      {
        fprintf(stderr,"Check(%d) failed rbuf[%d] %f != %f\n",count,i,dbuf[i+1],i*d);
        err = -1;
        return err;
      }
    }

  return err;
}


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

  if (rc==1)
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
          printf("# %s Scan Bandwidth Test -- context = %d, task_zero = %d protocol: %s\n",
                 scan_type, iContext, task_zero, scan_always_works_md[nalg].name);
          printf("# Size(bytes)           cycles    bytes/sec    usec\n");
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

        if (task_id == task_zero)
          printf("Running Scan: Non Contiguous Data type, PAMI_DATA_SUM\n");
        for (i = 0; i <= gMax_byte_count/(sizeof(double)*2); i *= 2)
        {
          size_t sz=sizeof(double)*2;
          size_t  dataSent = i * sz;
          int niter;
          if (dataSent < CUTOFF)
            niter = gNiterlat;
          else
            niter = NITERBW;
          scan.cmd.xfer_scan.stypecount = i;
          scan.cmd.xfer_scan.rtypecount = i;
          scan.cmd.xfer_scan.stype      = pami_stype;
          scan.cmd.xfer_scan.rtype      = pami_rtype;
          scan.cmd.xfer_scan.op         = PAMI_DATA_SUM;
          initialize_sndbuf (sbuf, i, task_id);
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
          rc |= rc_check = check_rcvbuf (rbuf, i, task_id, exclusive);
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
          if(!i)i++;
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
