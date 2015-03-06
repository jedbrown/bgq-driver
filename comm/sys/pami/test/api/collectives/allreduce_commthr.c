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
 * \file test/api/collectives/allreduce.c
 * \brief Simple Allreduce on world geometry
 */

/* see setup_env() for environment variable overrides               */
#include "../pami_util.h"

int async_progress_init (pami_client_t     client,
			 pami_context_t  * contexts,
			 int               ncontexts);

int async_progress_finalize (pami_client_t     client,
			     pami_context_t  * contexts,
			     int               ncontexts);

pami_result_t async_coll_handoff(pami_context_t context, void *cookie)
{
  pami_xfer_t *coll = (pami_xfer_t *)cookie;
  pami_result_t rc = PAMI_Collective (context, coll);
  return rc;
}

void async_cb_done (void *ctxt, void * clientdata, pami_result_t err)
{
  int * active = (int *) clientdata;
  (*active)--;
}

int async_blocking_coll (pami_context_t      context,
			 pami_xfer_t        *coll,
			 volatile unsigned  *active)
{
  pami_result_t result;
  gContext = context;
  (*active)++;

  pami_work_t work;
  result = PAMI_Context_post(context, &work, async_coll_handoff, coll);
  
  while (*active);  //wait for collective to finish
  gContext = NULL;
  return 0;
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

  if (rc != PAMI_SUCCESS)
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

  if (rc != PAMI_SUCCESS)
    return 1;

  async_progress_init(client, context, gNum_contexts);

  barrier.cb_done   = async_cb_done;
  barrier.cookie    = (void*) & bar_poll_flag;
  barrier.algorithm = bar_always_works_algo[0];

  unsigned iContext = 0;

  for (; iContext < 1 /*gNum_contexts*/; ++iContext)
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

    if (rc != PAMI_SUCCESS)
      return 1;
    
    for (nalg = 0; nalg < allreduce_num_algorithm[0]; nalg++)
    {
      if (task_id == task_zero)
      {
        printf("# Allreduce Bandwidth Test(size:%zu) -- context = %d, optimize = %d, protocol: %s\n",num_tasks,
               iContext, o, allreduce_always_works_md[nalg].name);
        printf("# Size(bytes)      iterations     bytes/sec      usec\n");
        printf("# -----------      -----------    -----------    ---------\n");
      }

      if (((strstr(allreduce_always_works_md[nalg].name, gSelected) == NULL) && gSelector) ||
          ((strstr(allreduce_always_works_md[nalg].name, gSelected) != NULL) && !gSelector))  continue;

      gProtocolName = allreduce_always_works_md[nalg].name;

      allreduce.cb_done   = async_cb_done;
      allreduce.cookie    = (void*) & allreduce_poll_flag;
      allreduce.algorithm = allreduce_always_works_algo[nalg];
      allreduce.cmd.xfer_allreduce.sndbuf    = sbuf;
      allreduce.cmd.xfer_allreduce.rcvbuf    = rbuf;
      allreduce.cmd.xfer_allreduce.rtype     = PAMI_TYPE_BYTE;
      allreduce.cmd.xfer_allreduce.rtypecount = 0;

      int op, dt;

      for (dt = 0; dt < dt_count; dt++)
      {
        for (op = 0; op < op_count; op++)
        {
          if (gValidTable[op][dt])
          {
            if (task_id == task_zero)
              printf("Running Allreduce: %s, %s\n", dt_array_str[dt], op_array_str[op]);

            for (i = MAX(1,gMin_byte_count/get_type_size(dt_array[dt])); i <= gMax_byte_count/get_type_size(dt_array[dt]); i *= 2)
            {
              size_t sz=get_type_size(dt_array[dt]);
              size_t  dataSent = i * sz;
              int niter;

              if (dataSent < CUTOFF)
                niter = gNiterlat;
              else
                niter = (gNiterlat/10) ? (gNiterlat/10) : 1;

              allreduce.cmd.xfer_allreduce.stypecount = i;
              allreduce.cmd.xfer_allreduce.rtypecount = dataSent;
              allreduce.cmd.xfer_allreduce.stype = dt_array[dt];
              allreduce.cmd.xfer_allreduce.op    = op_array[op];

              reduce_initialize_sndbuf (sbuf, i, op, dt, task_id, num_tasks);

              /* We aren't testing barrier itself, so use context 0. */
              async_blocking_coll(context[0], &barrier, &bar_poll_flag);

              for (j = 0; j < niter; j++)
              {
                async_blocking_coll(context[0], &allreduce, &allreduce_poll_flag);
              }

              /* We aren't testing barrier itself, so use context 0. */
              async_blocking_coll(context[0], &barrier, &bar_poll_flag);

              ti = timer();

              for (j = 0; j < niter; j++)
              {
                async_blocking_coll(context[0], &allreduce, &allreduce_poll_flag);
              }

              tf = timer();
              /* We aren't testing barrier itself, so use context 0. */
              async_blocking_coll(context[0], &barrier, &bar_poll_flag);

              int rc_check = 0;
              rc |= rc_check = reduce_check_rcvbuf (rbuf, i, op, dt, task_id, num_tasks);

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
        }
      }
    }

    free(allreduce_always_works_algo);
    free(allreduce_always_works_md);
    free(allreduce_must_query_algo);
    free(allreduce_must_query_md);

  } /* optimize loop */
  } /*for(unsigned iContext = 0; iContext < gNum_contexts; ++iContexts)*/

  free(bar_always_works_algo);
  free(bar_always_works_md);
  free(bar_must_query_algo);
  free(bar_must_query_md);

  sbuf = (char*)sbuf - gBuffer_offset;
  free(sbuf);
  rbuf = (char*)rbuf - gBuffer_offset;
  free(rbuf);
  
  async_progress_finalize(client, context, gNum_contexts);

  rc |= pami_shutdown(&client, context, &gNum_contexts);
  return rc;
}


typedef void (*pamix_progress_function) (pami_context_t context, void *cookie);
typedef pami_result_t (*pamix_progress_register_fn) 
  (pami_context_t            context,
   pamix_progress_function   progress_fn,
   pamix_progress_function   suspend_fn,
   pamix_progress_function   resume_fn,
   void                     * cookie);
typedef pami_result_t (*pamix_progress_enable_fn)(pami_context_t   context,
						  int              event_type);
typedef pami_result_t (*pamix_progress_disable_fn)(pami_context_t  context,
						   int             event_type);
#define PAMI_EXTENSION_OPEN(client, name, ext)  \
({                                              \
  pami_result_t rc;                             \
  rc = PAMI_Extension_open(client, name, ext);  \
  assert (rc == PAMI_SUCCESS);      \
})
#define PAMI_EXTENSION_FUNCTION(type, name, ext)        \
({                                                      \
  void* fn;                                             \
  fn = PAMI_Extension_symbol(ext, name);                \
  assert (fn != NULL);				\
  (type)fn;                                             \
})

pami_extension_t            coll_ext_progress;
pamix_progress_register_fn  coll_progress_register;
pamix_progress_enable_fn    coll_progress_enable;
pamix_progress_disable_fn   coll_progress_disable;

int async_progress_init (pami_client_t     client,
			 pami_context_t  * contexts,
			 int               ncontexts) 
{
  PAMI_EXTENSION_OPEN(client,"EXT_async_progress",&coll_ext_progress);
  coll_progress_register = PAMI_EXTENSION_FUNCTION(pamix_progress_register_fn, "register",coll_ext_progress);
  coll_progress_enable   = PAMI_EXTENSION_FUNCTION(pamix_progress_enable_fn,   "enable",  coll_ext_progress);
  coll_progress_disable  = PAMI_EXTENSION_FUNCTION(pamix_progress_disable_fn,  "disable", coll_ext_progress);
  
  int i = 0;
  for (i = 0; i < ncontexts; ++i) {
    //fprintf(stderr, "Enabling progress on context %d\n", i);
    coll_progress_register (contexts[i], 
			    NULL, 
			    NULL, 
			    NULL, NULL);
    coll_progress_enable   (contexts[i], 0 /*progress all*/);  
  }

  return 0;
}

int async_progress_finalize (pami_client_t     client,
			    pami_context_t  * contexts,
			    int               ncontexts) 
{
  int i = 0;
  for (i = 0; i < ncontexts; ++i) 
    coll_progress_disable  (contexts[i], 0 /*progress all*/);    
  PAMI_Extension_close (coll_ext_progress);

  return 0;
}
