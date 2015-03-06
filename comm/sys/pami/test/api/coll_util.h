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
 * \file test/api/coll_util.h
 * \brief ???
 */

#ifndef __test_api_coll_util_h__
#define __test_api_coll_util_h__

/* Define to assert on failures */
/* #define PAMI_TEST_STRICT     */

#include<assert.h>
#include<pthread.h>
#include "init_util.h"

#define THREAD_LOCAL __thread
char *gProtocolName = (char*)""; /* Global protocol name, some tests set it for error msgs   */
static size_t get_type_size(pami_type_t intype);
THREAD_LOCAL pami_context_t      gContext;
THREAD_LOCAL int                 gThreadId = 0;
THREAD_LOCAL volatile int        gGeom_init;

/* Docs09:  Done/Decrement call */
void cb_done (void *ctxt, void * clientdata, pami_result_t err)
{
  if(gVerbose)
  {
    if(!ctxt) fprintf(stderr,
                      "%s: Error(tid=%d). Null context received on cb_done.\n",
                      gProtocolName,
                      gThreadId);
    if(gContext != ctxt) fprintf(stderr,
                                 "%s: Context Error(tid=%d) want:%p got:%p\n",
                                 gProtocolName,
                                 gThreadId,
                                 gContext,
                                 ctxt);
    assert(ctxt);
    assert(gContext==ctxt);

    pami_configuration_t configs;
    configs.name         = PAMI_CONTEXT_DISPATCH_ID_MAX;
    configs.value.intval = -1;

    pami_result_t rc;
    rc = PAMI_Context_query (ctxt,&configs,1);

    if(rc != PAMI_SUCCESS && rc != PAMI_INVAL) fprintf(stderr,"%s: Error. Could not query the context(%u).\n",gProtocolName,rc);
#ifdef PAMI_TEST_STRICT
    assert(rc == PAMI_SUCCESS || rc == PAMI_INVAL);
#endif
    if(gVerbose>2)
      fprintf(stderr,
              "#%u: cb_done(%p/%p, %p(%d))\n",
              gThreadId,
              ctxt, gContext,clientdata, *(int*)clientdata);    
  }

  int * active = (int *) clientdata;
  (*active)--;
}
/* Docs10:  Done/Decrement call */

/* Docs07:  Blocking Collective Call */
int blocking_coll (pami_context_t      context,
                   pami_xfer_t        *coll,
                   volatile unsigned  *active)
{
  pami_result_t result;
  gContext = context;
  (*active)++;
  if(gVerbose>2)
    fprintf(stderr,
            "#%u: blocking_coll(%p/%p, %p, %p(%d))\n",
            gThreadId,
            context,gContext,coll, active, *(int*)active);    
  result = PAMI_Collective(context, coll);
  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to issue  collective. result = %d\n", result);
      gContext = NULL;
      return 1;
    }
  if(gVerbose>2)
  {
    int i=10000;
    while (*active && --i)
      result = PAMI_Context_advance (context, 1);
    fprintf(stderr,
            "#%u: blocking_coll(%p/%p, %p, %p(%d))\n",
            gThreadId,
            context, gContext,coll, active, *(int*)active);    
  }
  while (*active)
    result = PAMI_Context_advance (context, 1);
  gContext = NULL;
  return 0;
}

int blocking_coll_advance_all (unsigned             myctxt_idx,
                               pami_context_t*      contexts,
                               pami_xfer_t         *coll,
                               volatile unsigned   *active)
{
  pami_result_t result;
  gContext = contexts[myctxt_idx];
  (*active)++;
  if(gVerbose>2)
    fprintf(stderr,
            "#%u: blocking_coll_advance_all(%d/%p/%p, %p, %p(%d))\n",
            gThreadId,
            myctxt_idx,contexts[myctxt_idx], gContext,coll, active, *(int*)active);    
  result = PAMI_Collective(contexts[myctxt_idx], coll);
  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to issue  collective. result = %d\n", result);
      gContext = NULL;
      return 1;
    }
  if(gVerbose>2)
  {
    int i=10000;
    unsigned counter = myctxt_idx;
    while (*active && --i)
    {
      result = PAMI_Context_advance (contexts[counter], 1);
      counter = (counter+1) % gNum_contexts;
    }
    fprintf(stderr,
            "#%u: blocking_coll_advance_all(%d/%p/%p, %p, %p(%d))\n",
            gThreadId,
            myctxt_idx, contexts[myctxt_idx], gContext,coll, active, *(int*)active);    
  }
  unsigned counter = myctxt_idx;
  while (*active)
  {
    result = PAMI_Context_advance (contexts[counter], 1);
    counter = (counter+1) % gNum_contexts;
  }
  gContext = NULL;
  return 0;
}

/* Docs08:  Blocking Collective Call */

int query_geometry_world(pami_client_t       client,
                         pami_context_t      context,
                         pami_geometry_t    *world_geometry,
                         pami_xfer_type_t    xfer_type,
                         size_t             *num_algorithm,
                         pami_algorithm_t  **always_works_alg,
                         pami_metadata_t   **always_works_md,
                         pami_algorithm_t  **must_query_alg,
                         pami_metadata_t   **must_query_md)
{

  pami_result_t     result           = PAMI_SUCCESS;
  /* Docs01:  Get the World Geometry */
  result = PAMI_Geometry_world (client,world_geometry);
  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to get world geometry: result = %d\n", result);
      return 1;
    }
  /* Docs02:  Get the World Geometry */

  /* Docs03:  Query the number of algorithms */
  result = PAMI_Geometry_algorithms_num(*world_geometry,
                                        xfer_type,
                                        num_algorithm);

  if (result != PAMI_SUCCESS || (num_algorithm[0]==0 && num_algorithm[1]==0))
    {
      fprintf (stderr,
               "Error. Unable to query algorithm, or no algorithms available result = %d\n",
               result);
      return 1;
    }
  /* Docs04:  Query the number of algorithms */

  *always_works_alg = (pami_algorithm_t*)malloc(sizeof(pami_algorithm_t)*num_algorithm[0]);
  *always_works_md  = (pami_metadata_t*)malloc(sizeof(pami_metadata_t)*num_algorithm[0]);
  *must_query_alg   = (pami_algorithm_t*)malloc(sizeof(pami_algorithm_t)*num_algorithm[1]);
  *must_query_md    = (pami_metadata_t*)malloc(sizeof(pami_metadata_t)*num_algorithm[1]);

  /* Docs05:  Query the algorithm lists */
  result = PAMI_Geometry_algorithms_query(*world_geometry,
                                          xfer_type,
                                          *always_works_alg,
                                          *always_works_md,
                                          num_algorithm[0],
                                          *must_query_alg,
                                          *must_query_md,
                                          num_algorithm[1]);
  /* Docs06:  Query the algorithm lists */
  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to get query algorithm. result = %d\n", result);
      return 1;
    }
  return 0;
}


int destroy_geometry(pami_client_t    client,
                     pami_context_t   context,
                     pami_geometry_t *geometry)
{
  pami_result_t     rc             = PAMI_SUCCESS;
  volatile unsigned geom_poll_flag = 1;
  gContext = context;
  rc = PAMI_Geometry_destroy(client,
			     geometry,
			     context,
			     cb_done,
			     (void*)&geom_poll_flag);
  
  while(geom_poll_flag)
    rc = PAMI_Context_advance (context, 1);

  return rc;
}

int update_geometry(pami_client_t    client,
                    pami_context_t   context,
                    pami_geometry_t  geometry,
                    pami_configuration_t  configuration[],
                    size_t                num_configs)
{
  pami_result_t     rc             = PAMI_SUCCESS;
  volatile unsigned geom_poll_flag = 1;
  gContext = context;
  rc = PAMI_Geometry_update (geometry,
                             configuration,
                             num_configs,
                             context,
                             cb_done,
                             (void*)&geom_poll_flag);
  if(rc) ; /* failed */
  else  
    while(geom_poll_flag)
      PAMI_Context_advance (context, 1);

  return rc;
}


int query_geometry(pami_client_t       client,
                   pami_context_t      context,
                   pami_geometry_t     geometry,
                   pami_xfer_type_t    xfer_type,
                   size_t             *num_algorithm,
                   pami_algorithm_t  **always_works_alg,
                   pami_metadata_t   **always_works_md,
                   pami_algorithm_t  **must_query_alg,
                   pami_metadata_t   **must_query_md)
{
  pami_result_t     result           = PAMI_SUCCESS;

  result = PAMI_Geometry_algorithms_num(geometry,
                                        xfer_type,
                                        num_algorithm);

  if (result != PAMI_SUCCESS || (num_algorithm[0]==0 && num_algorithm[1]==0))
    {
      fprintf (stderr,
               "Error. query, Unable to query algorithm, or no algorithms available result = %d\n",
               result);
      return 1;
    }

  *always_works_alg = (pami_algorithm_t*)malloc(sizeof(pami_algorithm_t)*num_algorithm[0]);
  *always_works_md  = (pami_metadata_t*)malloc(sizeof(pami_metadata_t)*num_algorithm[0]);
  *must_query_alg   = (pami_algorithm_t*)malloc(sizeof(pami_algorithm_t)*num_algorithm[1]);
  *must_query_md    = (pami_metadata_t*)malloc(sizeof(pami_metadata_t)*num_algorithm[1]);
  result = PAMI_Geometry_algorithms_query(geometry,
                                          xfer_type,
                                          *always_works_alg,
                                          *always_works_md,
                                          num_algorithm[0],
                                          *must_query_alg,
                                          *must_query_md,
                                          num_algorithm[1]);
  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to get query algorithm. result = %d\n", result);
      return 1;
    }
  return 0;
}


int create_all_ctxt_geometry(pami_client_t           client,
                             pami_context_t         *contexts,
                             size_t                  num_contexts,
                             pami_geometry_t         parent_geometry,
                             pami_geometry_t        *new_geometry,
                             pami_geometry_range_t  *range,
                             unsigned                numranges,
                             unsigned                id)
{
  pami_result_t result;
  int           geom_init=1;
  gContext = contexts[0];

  size_t num_configs=3;
  pami_configuration_t config[3];
  config[0].name = PAMI_GEOMETRY_OPTIMIZE;
  config[0].value.intval = 1;
  config[1].name = PAMI_GEOMETRY_NONCONTIG;
  config[1].value.intval = gNonContig;
  config[2].name = PAMI_GEOMETRY_MEMORY_OPTIMIZE;
  config[2].value.intval = gMemoryOptimize;

  result = PAMI_Geometry_create_taskrange (client,
                                           PAMI_ALL_CONTEXTS,
                                           config, /*NULL*/
                                           num_configs,
                                           new_geometry,
                                           parent_geometry,
                                           id,
                                           range,
                                           numranges,
                                           contexts[0],
                                           cb_done,
                                           &geom_init);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to create a new geometry. result = %d\n", result);
      return 1;
    }
    unsigned long long cnt=0;
    while (geom_init)
    {
      result = PAMI_Context_advance (contexts[cnt%num_contexts], 1);
      cnt++;
    }
    gContext = NULL;
    if (*new_geometry == NULL)
    {
      fprintf (stderr, "Error. Unable to create a new geometry (%p).\n", new_geometry);
      return 1;
    }
    return 0;
}



int create_and_query_geometry_slice(pami_client_t           client,
                                    size_t                  context_id,  /* context slice */
                                    pami_context_t          context, /* context for create callback */
                                    pami_context_t          contextq, /* context for query */
                                    pami_geometry_t         parent_geometry,
                                    pami_geometry_t        *new_geometry,
                                    pami_geometry_range_t  *range,
                                    unsigned                numranges,
                                    unsigned                id,
                                    pami_xfer_type_t        xfer_type,
                                    size_t                 *num_algorithm,
                                    pami_algorithm_t      **always_works_alg,
                                    pami_metadata_t       **always_works_md,
                                    pami_algorithm_t      **must_query_alg,
                                    pami_metadata_t       **must_query_md)
{
  pami_result_t result;
  gContext = context;
  gGeom_init = 1;
  size_t num_configs=3;
  pami_configuration_t config[3];
  config[0].name = PAMI_GEOMETRY_OPTIMIZE;
  config[0].value.intval = 1;
  config[1].name = PAMI_GEOMETRY_NONCONTIG;
  config[1].value.intval = gNonContig;
  config[2].name = PAMI_GEOMETRY_MEMORY_OPTIMIZE;
  config[2].value.intval = gMemoryOptimize;

  if(gVerbose>2) 
    fprintf(stderr,"#%u: PAMI_Geometry_create_taskrange (client          %p\n"
                   "                                context_id      %zu\n" 
                   "                                &config         %p=%zu,%zu\n"
                   "                                num_configs     %zd\n"
                   "                                new_geometry    %p\n"
                   "                                parent_geometry %p\n"
                   "                                id              %u\n"
                   "                                range           %p\n"
                   "                                numranges       %u\n"
                   "                                context         %p\n"
                   "                                cb_done         %p\n"
                   "                                &geom_init      %p/%d);\n",
            gThreadId,client,context_id,config,config[0].value.intval,config[1].value.intval,num_configs,new_geometry,parent_geometry,id,range,numranges,context,cb_done,&gGeom_init,gGeom_init);     
  
  result = PAMI_Geometry_create_taskrange (client,
                                           context_id, 
                                           config, /*NULL*/
                                           num_configs,
                                           new_geometry,
                                           parent_geometry,
                                           id,
                                           range,
                                           numranges,
                                           context,
                                           cb_done,
                                           (void*)&gGeom_init);
    if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to create a new geometry. result = %d\n", result);
      return 1;
    }
    if(gVerbose>2)
    {
      int i=100;
      while (gGeom_init && --i)
        result = PAMI_Context_advance (context, 1);
      fprintf(stderr,
              "#%u: blocking_coll_advance_all(%zu/%p/%p,%d)\n",
              gThreadId,
              context_id, context, gContext, gGeom_init);    
    }
    while (gGeom_init)
      result = PAMI_Context_advance (context, 1);

    gContext = NULL;

    if (*new_geometry == NULL)
    {
      fprintf (stderr, "Error. Unable to create a new geometry (%p).\n", new_geometry);
      return 1;
    }

    return query_geometry(client,
                          contextq,
                          *new_geometry,
                          xfer_type,
                          num_algorithm,
                          always_works_alg,
                          always_works_md,
                          must_query_alg,
                          must_query_md);
}
int create_and_query_geometry(pami_client_t           client,
                              pami_context_t          context,  /* context for create callback */
                              pami_context_t          contextq, /* context for query */
                              pami_geometry_t         parent_geometry,
                              pami_geometry_t        *new_geometry,
                              pami_geometry_range_t  *range,
                              unsigned                numranges,
                              unsigned                id,
                              pami_xfer_type_t        xfer_type,
                              size_t                 *num_algorithm,
                              pami_algorithm_t      **always_works_alg,
                              pami_metadata_t       **always_works_md,
                              pami_algorithm_t      **must_query_alg,
                              pami_metadata_t       **must_query_md)
{
  return create_and_query_geometry_slice(client,
                                         0,        /* always slice 0 */
                                         context,
                                         contextq,
                                         parent_geometry,
                                         new_geometry,
                                         range,
                                         numranges,
                                         id,
                                         xfer_type,
                                         num_algorithm,
                                         always_works_alg,
                                         always_works_md,
                                         must_query_alg,
                                         must_query_md);
}

/* If this cannot be inlined, this function definition
   should be moved into its own .c file
*/
static inline size_t get_type_size(pami_type_t intype)
{
  pami_result_t        res;
  pami_configuration_t config;


  config.name=PAMI_TYPE_DATA_SIZE;
  res        =PAMI_Type_query (intype,&config,1);
  if(res != PAMI_SUCCESS)
  {
    fprintf(stderr, "Fatal:  error querying size of type:  rc=%d\n",res);
    exit(0);
  }

  size_t sz =(size_t)config.value.intval;
  assert(sz <= gMax_datatype_sz); /* We alloc based on an assumed max sz so assert it now */
  return sz;
}

unsigned primitive_dt(pami_type_t dt)
{
  unsigned found = 0,i;
  for(i = 0; i < DT_COUNT; ++i)
  {
    if(dt_array[i] == dt) found = 1;
  }
  return found;
}

metadata_result_t check_metadata(pami_metadata_t md,
                                 pami_xfer_t xfer,
                                 pami_type_t s_dt, size_t s_size, char* s_buffer,
                                 pami_type_t r_dt, size_t r_size, char* r_buffer
                                 )
{
  metadata_result_t result;
  if(gVerbose>=2 && gTaskId == 0)
      fprintf(stderr,"#%u: Check: s_dt %p, s_size %zu, s_buffer %p,  r_dt %p, r_size %zu, r_buffer %p\n",gThreadId,s_dt,s_size,s_buffer,r_dt,r_size,r_buffer);
  if (md.check_fn)
  {
    result = md.check_fn(&xfer);
  }
  else /* Must check parameters ourselves... */
  {
    uint64_t  mask=0;
    result.bitmask = 0;
    if(md.check_correct.values.sendminalign)
    {
      mask  = md.send_min_align - 1;
      if(s_buffer != PAMI_IN_PLACE)
        result.check.align_send_buffer = (((size_t)s_buffer & (size_t)mask) == 0) ? 0:1;
    }
    if(md.check_correct.values.recvminalign)
    {
      mask  = md.recv_min_align - 1;
      if(r_buffer != PAMI_IN_PLACE)
        result.check.align_recv_buffer = (((size_t)r_buffer & (size_t)mask) == 0) ? 0:1;
    }
    if(md.check_correct.values.rangeminmax)
    {
      result.check.range  = !((s_size <= md.range_hi) &&
                              (s_size >= md.range_lo));
      result.check.range |= !((r_size <= md.range_hi) &&
                              (r_size >= md.range_lo));
    }
    /* Very basic checks (primitives only) for continuous/contiguous */
    if(md.check_correct.values.contigsflags)
      result.check.contiguous_send = !primitive_dt(s_dt);
    if(md.check_correct.values.contigrflags)
      result.check.contiguous_recv = !primitive_dt(r_dt);
    if(md.check_correct.values.continsflags)
      result.check.continuous_send = !primitive_dt(s_dt);
    if(md.check_correct.values.continrflags)
      result.check.continuous_recv = !primitive_dt(r_dt);
  }
  if(gVerbose>=2 && gTaskId == 0)
  {
      fprintf(stderr,"#%u: Metadata: "
             "checkrequired %d, "
             "check_fn      %p, \n"
   "#%u:           rangeminmax   %d, "
             "range_lo      %zd, "
             "range_hi      %zd,\n"
   "#%u:           recvminalign  %d, "
             "sendminalign  %d,\n"
   "#%u:           alldtop       %d,\n"
   "#%u:           asyncflowctl  %d, "
             "blocking      %d, "
             "global_order  %d, "
             "inplace       %d,\n"
   "#%u:           continrflags  %d, "
             "continsflags  %d, "
             "contigrflags  %d, "
             "contigsflags  %d\n"
   "#%u:           oneatatime    %d, "
             "nonlocal      %d,\n",gThreadId,
             md.check_correct.values.checkrequired,
                                  md.check_fn,
             gThreadId,
             md.check_correct.values.rangeminmax,
                                  (ssize_t)md.range_lo,
                                  (ssize_t)md.range_hi,
             gThreadId,
             md.check_correct.values.recvminalign,
             md.check_correct.values.sendminalign,
             gThreadId,
             md.check_correct.values.alldtop,
             gThreadId,
             md.check_correct.values.asyncflowctl,
             md.check_correct.values.blocking,
             md.check_correct.values.global_order,
             md.check_correct.values.inplace,
             gThreadId,
             md.check_correct.values.continrflags,
             md.check_correct.values.continsflags,
             md.check_correct.values.contigrflags,
             md.check_correct.values.contigsflags,
             gThreadId,
             md.check_correct.values.oneatatime,
             md.check_correct.values.nonlocal
             );
      fprintf(stderr,"#%u: Query Result: unspec=%u range=%u align_rbuf=%u align_sbuf=%u dt_op=%u cgs=%u cgr=%u cts=%u ctr=%u nonlocal=%u\n",gThreadId,
             result.check.unspecified,
             result.check.range,
             result.check.align_send_buffer,
             result.check.align_recv_buffer,
             result.check.datatype_op,
             result.check.contiguous_send,
             result.check.contiguous_recv,
             result.check.continuous_send,
             result.check.continuous_recv,
             result.check.nonlocal);
  }
  return result;
}

#endif /* __test_api_coll_util_h__*/
