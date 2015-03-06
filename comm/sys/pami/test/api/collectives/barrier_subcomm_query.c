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
 * \file test/api/collectives/barrier_subcomm_query.c
 * \brief Simple Barrier test on sub-geometries using "query" algorithms
 */


/* see setup_env() for environment variable overrides               */
#include "../pami_util.h"

int main(int argc, char*argv[])
{
  pami_client_t        client;
  pami_context_t      *context;
  pami_task_t          task_id, local_task_id=0, root = 0;
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

  if (num_tasks == 1)
  {
    fprintf(stderr, "No barrier subcomms on 1 node\n");
    return 0;
  }

  assert(task_id >= 0);
  assert(task_id < num_tasks);

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
                               num_algorithm,
                               &always_works_algo,
                               &always_works_md,
                               &must_query_algo,
                               &must_query_md);

    if (rc == 1)
      return 1;

    /*  Create the subgeometry */
    pami_geometry_range_t *range;
    int                    rangecount;
    pami_geometry_t        newgeometry;
    size_t                 newbar_num_algo[2];
    pami_algorithm_t      *newbar_algo        = NULL;
    pami_metadata_t       *newbar_md          = NULL;
    pami_algorithm_t      *q_newbar_algo      = NULL;
    pami_metadata_t       *q_newbar_md        = NULL;

    pami_xfer_t            newbarrier;

    size_t                 set[2];
    int                    id, non_root[2], timeDelay = 0;

    range     = (pami_geometry_range_t *)malloc(((num_tasks + 1) / 2) * sizeof(pami_geometry_range_t));

    get_split_method(&num_tasks, task_id, &rangecount, range, &local_task_id, set, &id, &root, non_root);


    if (root)
    {
      timeDelay = 3; /* Need to stagger barriers between subcomm's */
    }

    /* Delay root tasks, and emulate that he's doing "other"
       message passing.  This will cause the geometry_create
       request from other nodes to be unexpected when doing
       parentless geometries and won't affect parented.      */
    if (task_id == root)
    {
      delayTest(1);
      unsigned ii = 0;

      for (; ii < gNum_contexts; ++ii)
        PAMI_Context_advance (context[ii], 1000);
    }

    rc |= create_and_query_geometry(client,
                                    context[0],
                                    context[iContext],
                                    gParentless ? PAMI_GEOMETRY_NULL : world_geometry,
                                    &newgeometry,
                                    range,
                                    rangecount,
                                    id + iContext, /* Unique id for each context */
                                    barrier_xfer,
                                    newbar_num_algo,
                                    &newbar_algo,
                                    &newbar_md,
                                    &q_newbar_algo,
                                    &q_newbar_md);

    if (rc == 1)
      return 1;


    /*  Set up world barrier */
    barrier.cb_done   = cb_done;
    barrier.cookie    = (void*) & poll_flag;
    barrier.algorithm = always_works_algo[0];

    /*  Set up sub geometry barrier */
    newbarrier.cb_done   = cb_done;
    newbarrier.cookie    = (void*) & poll_flag;

    rc |= blocking_coll(context[iContext], &barrier, &poll_flag);

    if (rc == 1)
      return 1;

    int  k;

    for (k = 1; k >= 0; k--)
    {
      if (set[k])
      {

        for (nalg = 0; nalg < newbar_num_algo[1]; nalg++)
        {
          metadata_result_t result = {0};
          double ti, tf, usec;
          newbarrier.algorithm = q_newbar_algo[nalg];

          if (task_id == root)
          {
            printf("# Test set(%u):  Barrier Test -- context = %d, root = %d, protocol: %s (%d of %zd algorithms)\n",
                   k, iContext, root, q_newbar_md[nalg].name, nalg + 1, newbar_num_algo[1]);
            printf("# -------------------------------------------------------------------\n");
          }

          if (((strstr(q_newbar_md[nalg].name,gSelected) == NULL) && gSelector) ||
              ((strstr(q_newbar_md[nalg].name,gSelected) != NULL) && !gSelector))  continue;

          unsigned checkrequired = q_newbar_md[nalg].check_correct.values.checkrequired; /*must query every time */
          assert(!checkrequired || q_newbar_md[nalg].check_fn); /* must have function if checkrequired. */

          if (q_newbar_md[nalg].check_fn)
            result = q_newbar_md[nalg].check_fn(&newbarrier);

          if (q_newbar_md[nalg].check_correct.values.nonlocal)
          {
            /* \note We currently ignore check_correct.values.nonlocal
               because these tests should not have nonlocal differences (so far). */
            result.check.nonlocal = 0;
          }

          if (result.bitmask) continue;

          /* Do two functional runs with different delaying ranks*/
          int j;

          for (j = 0; j < 2; ++j)
          {
            if (task_id == root)
            {
              delayTest(timeDelay);
              fprintf(stderr, "Test set(%u):  Barrier protocol(%s) Correctness (%d of %zd algorithms)\n", k,
                      q_newbar_md[nalg].name, nalg + 1, newbar_num_algo[1]);
              ti = timer();
              blocking_coll(context[iContext], &newbarrier, &poll_flag);
              tf = timer();
              usec = tf - ti;

              if ((usec < 1800000.0 || usec > 2200000.0) && (num_tasks > 1))
              {
                rc = 1;
                fprintf(stderr, "%s FAIL: usec=%f want between %f and %f!\n", q_newbar_md[nalg].name,
                        usec, 1800000.0, 2200000.0);
              }
              else
                fprintf(stderr, "%s PASS: Barrier correct!\n", q_newbar_md[nalg].name);
            }
            else
            {
              /* Try to vary where the delay comes from... by picking first and last (non-roots) we
                 *might* be getting same node/different node delays.
              */
              if (task_id == non_root[j])
                delayTest(2 + timeDelay);

              blocking_coll(context[iContext], &newbarrier, &poll_flag);
            }
          }

          int niter = gNiterlat;
          blocking_coll(context[iContext], &newbarrier, &poll_flag);

          ti = timer();
          int i;

          for (i = 0; i < niter; i++)
          {
            if (checkrequired) /* must query every time */
            {
              result = q_newbar_md[nalg].check_fn(&newbarrier);

              if (result.bitmask) continue;
            }

            blocking_coll(context[iContext], &newbarrier, &poll_flag);
          }

          tf = timer();
          usec = tf - ti;

          if (task_id == root)
            fprintf(stderr, "Test set(%u): Barrier protocol(%s) Performance: time=%f usec\n", k,
                    newbar_md[nalg].name, usec / (double)niter);
        }
      }
    }

    blocking_coll(context[iContext], &barrier, &poll_flag);

    free(always_works_algo);
    free(always_works_md);
    free(must_query_algo);
    free(must_query_md);

  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

  rc |= pami_shutdown(&client, context, &gNum_contexts);

  return rc;
}
