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
 * \file test/api/collectives/geometry.c
 * \brief Test for large geometry create counts
 */

/* see setup_env() for environment variable overrides               */
#define NITER   10000
#define DO_BCAST
#define DO_BARRIER

#include "../pami_util.h"

int main(int argc, char*argv[])
{
  pami_client_t        client;
  pami_context_t      *context;
  pami_task_t          task_id, local_task_id=0, task_real_zero=0, task_zero=0;
  size_t               num_tasks;
  pami_geometry_t      world_geometry;

  /* Barrier variables */
  size_t               num_algorithm[2];
  pami_algorithm_t    *always_works_algo = NULL;
  pami_metadata_t     *always_works_md = NULL;
  pami_algorithm_t    *must_query_algo = NULL;
  pami_metadata_t     *must_query_md = NULL;
  pami_xfer_type_t     barrier_xfer = PAMI_XFER_BARRIER;

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

    size_t                 newbcast_num_algo[2];
    pami_algorithm_t      *newbcast_algo      = NULL;
    pami_metadata_t       *newbcast_md        = NULL;
    pami_algorithm_t      *q_newbcast_algo    = NULL;
    pami_metadata_t       *q_newbcast_md      = NULL;

    size_t                 set[2];
    int                    id, non_root[2];

    range     = (pami_geometry_range_t *)malloc(((num_tasks + 1) / 2) * sizeof(pami_geometry_range_t));

    get_split_method(&num_tasks, task_id, &rangecount, range, &local_task_id, set, &id, &task_zero,non_root);

    int k;
    double ti=timer(), timeElapsed=0, timeIteration=0;
    for(k=0; k<NITER; k++)
    {
      if (task_id == k%num_tasks)
      {
        unsigned ii = 0;
        for (; ii < gNum_contexts; ++ii)
          PAMI_Context_advance (context[ii], 1000);
      }
      if(k==0 && task_id == task_zero)
         fprintf(stdout, "Entering First Geometry Create\n");
         
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

      if(k==0 && task_id == task_zero)
         fprintf(stdout, "Entering Geometry Query\n");
         
      /*  Query the sub geometry for bcast algorithms */
      pami_xfer_type_t     bcast_xfer = PAMI_XFER_BROADCAST;
      /*pami_xfer_type_t     bar_xfer   = PAMI_XFER_BARRIER;*/
      pami_xfer_t          newbcast;
      pami_xfer_t          newbar;
      rc |= query_geometry(client,
                           context[iContext],
                           newgeometry,
                           bcast_xfer,
                           newbcast_num_algo,
                           &newbcast_algo,
                           &newbcast_md,
                           &q_newbcast_algo,
                           &q_newbcast_md);

      if (rc == 1)
        return 1;

      if(k==0 && task_id == task_zero)
         fprintf(stdout, "Broadcasting\n");

      int buf[256];
      memset(buf, 0xFF, sizeof(buf));

      /*  Bcast Operation */
      pami_endpoint_t    root_ep;
      volatile unsigned    bcast_poll_flag = 0;
      volatile unsigned    bar_poll_flag = 0;
#ifdef DO_BCAST
      PAMI_Endpoint_create(client, task_zero, 0, &root_ep);
      newbcast.cmd.xfer_broadcast.root = root_ep;
      newbcast.cb_done                      = cb_done;
      newbcast.cookie                       = (void*) & bcast_poll_flag;
      newbcast.algorithm                    = newbcast_algo[0];
      newbcast.cmd.xfer_broadcast.buf       = (char*)&buf[0];
      newbcast.cmd.xfer_broadcast.type      = PAMI_TYPE_BYTE;
      newbcast.cmd.xfer_broadcast.typecount = sizeof(buf);
      newbcast.algorithm                    = newbcast_algo[(k%newbcast_num_algo[0])];
      blocking_coll(context[iContext], &newbcast, &bcast_poll_flag);
#endif
      
#ifdef DO_BARRIER
      /*  Barrier Operation */
      newbar.cb_done                      = cb_done;
      newbar.cookie                       = (void*) & bar_poll_flag;
      newbar.algorithm                    = newbar_algo[0];
      newbar.algorithm                    = newbar_algo[(k%newbar_num_algo[0])];
      blocking_coll(context[iContext], &newbar, &bar_poll_flag);
#endif
      
      if(k==0 && task_id == task_zero)
         fprintf(stdout, "Destroying Geometry\n");

      destroy_geometry(client,context[iContext], &newgeometry);

      if(k==0 && task_id == task_zero)
         fprintf(stdout, "Done....entering iterative phase\n");
   
      free(newbcast_algo);
      free(newbcast_md);
      free(q_newbcast_algo);
      free(q_newbcast_md);
      free(newbar_algo);
      free(newbar_md);
      free(q_newbar_algo);
      free(q_newbar_md);

      if((k==0 && task_id == task_real_zero)
         || (k!=0 && (k%(100))==0 && task_id == task_real_zero))
      {
        double tf     = timer();
        timeIteration = timeElapsed;
        timeElapsed   = tf - ti;
        timeIteration = timeElapsed - timeIteration;
        fprintf(stdout, "Iteration %d of %d time=%f usec (numbcast=%zu numbar=%zu)\n",
                k, NITER, timeIteration/(double)(NITER/10.0),
                newbcast_num_algo[0],newbar_num_algo[0]);
      }
    }

    if(task_id == task_real_zero)
      fprintf(stdout, "Done with iteration %d of %d\n",
              NITER, NITER);

  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

  rc |= pami_shutdown(&client, context, &gNum_contexts);

  return rc;
}
