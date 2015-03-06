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
 * \file test/api/collectives/barrier.c
 * \brief Simple Barrier test on world geometry
 */


/* see setup_env() for environment variable overrides               */
#include "../pami_util.h"

int main(int argc, char*argv[])
{
  pami_client_t        client;
  pami_context_t      *context;
  pami_task_t          task_id, non_root[2];
  size_t               num_tasks;
  pami_geometry_t      world_geometry;

  /* Barrier variables */
  size_t               num_algorithm[2];
  pami_algorithm_t    *always_works_algo = NULL;
  pami_metadata_t     *always_works_md = NULL;
  pami_algorithm_t    *must_query_algo = NULL;
  pami_metadata_t     *must_query_md = NULL;
  pami_xfer_type_t     barrier_xfer = PAMI_XFER_BARRIER;
  pami_xfer_t          barrier;
  volatile unsigned    poll_flag = 0;

  int                  nalg;

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

  non_root[0] = 1;            /* first non-root rank in the comm  */
  non_root[1] = num_tasks - 1;/* last rank in the comm  */

  unsigned iContext = 0;

  for (; iContext < gNum_contexts; ++iContext)
  {

    if (task_id == 0)
      printf("# Context: %u\n", iContext);

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

    /*  Query the world geometry for barrier algorithms */
    rc |= query_geometry_world(client,
                               context[iContext],
                               &world_geometry,
                               barrier_xfer,
                               num_algorithm,
                               &always_works_algo,
                               &always_works_md,
                               &must_query_algo,
                               &must_query_md);

    if (rc == 1)
      return 1;

    barrier.cb_done   = cb_done;
    barrier.cookie    = (void*) & poll_flag;
    barrier.algorithm = always_works_algo[0];

    if (!(((strstr(always_works_md[0].name, gSelected) == NULL) && gSelector) ||
          ((strstr(always_works_md[0].name, gSelected) != NULL) && !gSelector)))
    {
      if (!task_id)
        fprintf(stderr, "(Optimize %d) Test Default Barrier(%s)\n", o, always_works_md[0].name);

      rc |= blocking_coll(context[iContext], &barrier, &poll_flag);

      if (rc == 1)
        return 1;

      if (!task_id)
        fprintf(stderr, "(Optimize %d) Barrier Done (%s)\n", o, always_works_md[0].name);
    }

    for (nalg = 0; nalg < num_algorithm[0]; nalg++)
    {
      double ti, tf, usec;
      barrier.algorithm = always_works_algo[nalg];

      if (!task_id)
      {
        printf("# Barrier Test -- context = %d, optimize = %d, protocol: %s (%d of %zd algorithms)\n",
               iContext, o, always_works_md[nalg].name, nalg + 1, num_algorithm[0]);
        printf("# -------------------------------------------------------------------\n");
      }

      if (((strstr(always_works_md[nalg].name, gSelected) == NULL) && gSelector) ||
          ((strstr(always_works_md[nalg].name, gSelected) != NULL) && !gSelector))  continue;

      /* Do two functional runs with different delaying ranks*/
      int j;

      for (j = 0; j < 2; ++j)
      {
        if (!task_id)
        {
          fprintf(stderr, "Test Barrier optimize = %d, protocol: %s correctness (%d of %zd algorithms)\n",
                  o, always_works_md[nalg].name, nalg + 1, num_algorithm[0]);
          ti = timer();
          blocking_coll(context[iContext], &barrier, &poll_flag);
          tf = timer();
          usec = tf - ti;

          if ((usec < 1800000.0 || usec > 2200000.0) && (num_tasks > 1))
          {
            rc = 1;
            fprintf(stderr, "%s FAIL: usec=%f want between %f and %f!\n", always_works_md[nalg].name,
                    usec, 1800000.0, 2200000.0);
          }
          else
            fprintf(stderr, "%s PASS: Barrier correct!\n", always_works_md[nalg].name);
        }
        else
        {
          /* Try to vary where the delay comes from... by picking first and last (non-roots) we
             *might* be getting same node/different node delays.
          */
          if (task_id == non_root[j])
            delayTest(2);

          blocking_coll(context[iContext], &barrier, &poll_flag);
        }
      }

      int niter = gNiterlat;
      blocking_coll(context[iContext], &barrier, &poll_flag);

      ti = timer();
      int i;

      for (i = 0; i < niter; i++)
        blocking_coll(context[iContext], &barrier, &poll_flag);

      tf = timer();
      usec = tf - ti;

      if (!task_id)
      {
        fprintf(stderr, "Test Barrier protocol(%s) Performance: time=%f usec\n",
                always_works_md[nalg].name, usec / (double)niter);
        delayTest(2);
      }

      blocking_coll(context[iContext], &barrier, &poll_flag);
    }

    free(always_works_algo);
    free(always_works_md);
    free(must_query_algo);
    free(must_query_md);

    } /* optimize loop */
  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

  rc |= pami_shutdown(&client, context, &gNum_contexts);

  return rc;
}
