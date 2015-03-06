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
 * \file api/extension/c/collsel/Benchmark.h
 */
#ifndef __api_extension_c_collsel_Benchmark_h__
#define __api_extension_c_collsel_Benchmark_h__

#include "api/extension/c/collsel/CollselExtension.h"
#include <stdio.h>
#include <assert.h>
#include <sys/time.h>

#define CACHE_WARMUP_ITERS 100

int cutoff[PAMI_XFER_COUNT];
int _g_verify;
int _g_verbose;

namespace PAMI{

enum opNum
{
  OP_COPY,
  OP_NOOP,
  OP_MAX,
  OP_MIN,
  OP_SUM,
  OP_PROD,
  OP_LAND,
  OP_LOR,
  OP_LXOR,
  OP_BAND,
  OP_BOR,
  OP_BXOR,
  OP_MAXLOC,
  OP_MINLOC,
  OP_COUNT,
};
static const int    op_count = OP_COUNT;
pami_data_function  op_array[OP_COUNT];
const char         *op_array_str[OP_COUNT];
static const char  *xfer_type_str[PAMI_XFER_COUNT];

enum dtNum
{
  DT_NULL,
  DT_BYTE,
  DT_SIGNED_CHAR,
  DT_UNSIGNED_CHAR,
  DT_SIGNED_SHORT,
  DT_UNSIGNED_SHORT,
  DT_SIGNED_INT,
  DT_UNSIGNED_INT,
  DT_SIGNED_LONG,
  DT_UNSIGNED_LONG,
  DT_SIGNED_LONG_LONG,
  DT_UNSIGNED_LONG_LONG,
  DT_FLOAT,
  DT_DOUBLE,
  DT_LONG_DOUBLE,
  DT_LOGICAL1,
  DT_LOGICAL2,
  DT_LOGICAL4,
  DT_LOGICAL8,
  DT_SINGLE_COMPLEX,
  DT_DOUBLE_COMPLEX,
  DT_LOC_2INT,
  DT_LOC_2FLOAT,
  DT_LOC_2DOUBLE,
  DT_LOC_SHORT_INT,
  DT_LOC_FLOAT_INT,
  DT_LOC_DOUBLE_INT,
  DT_LOC_LONG_INT,
  DT_LOC_LONGDOUBLE_INT,
  DT_COUNT,
};
int          dt_count = DT_COUNT;
pami_type_t  dt_array[DT_COUNT];
const char * dt_array_str[DT_COUNT];

pami_task_t         my_task_id;
size_t              num_tasks;
char * _g_buffer;
int                 _gRc = PAMI_SUCCESS;
char * _g_recv_buffer;
char * _g_send_buffer;
validation_t           *_g_val_buffer;
volatile unsigned am_total_count = 0;
int scatter_check_rcvbuf(void *rbuf,
                         int bytes,
                         pami_task_t task);
int bcast_check_rcvbuf(void *rbuf,
                       int bytes,
                       int root);
int gather_check_rcvbuf(size_t num_ep,
                        void *buf,
                        int bytes);
int reduce_check_rcvbuf(void *buf,
                        int count,
                        pami_data_function op,
                        pami_type_t dt,
                        int task_id,
                        int num_tasks);
void scatter_initialize_sndbuf(void *sbuf,
                               int bytes,
                               size_t ntasks);
void bcast_initialize_sndbuf(void *sbuf,
                             int bytes,
                             int root);
void gather_initialize_sndbuf(int ep_id,
                              void *buf,
                              int bytes);
void reduce_initialize_sndbuf(void *buf,
                              size_t count,
                              pami_data_function op,
                              pami_type_t dt,
                              size_t task_id,
                              size_t num_tasks);

int gFull_test = 0;


void init_tables()
{
  op_array[0]=PAMI_DATA_COPY;
  op_array[1]=PAMI_DATA_NOOP;
  op_array[2]=PAMI_DATA_MAX;
  op_array[3]=PAMI_DATA_MIN;
  op_array[4]=PAMI_DATA_SUM;
  op_array[5]=PAMI_DATA_PROD;
  op_array[6]=PAMI_DATA_LAND;
  op_array[7]=PAMI_DATA_LOR;
  op_array[8]=PAMI_DATA_LXOR;
  op_array[9]=PAMI_DATA_BAND;
  op_array[10]=PAMI_DATA_BOR;
  op_array[11]=PAMI_DATA_BXOR;
  op_array[12]=PAMI_DATA_MAXLOC;
  op_array[13]=PAMI_DATA_MINLOC;

  op_array_str[0]="PAMI_DATA_COPY";
  op_array_str[1]="PAMI_DATA_NOOP";
  op_array_str[2]="PAMI_DATA_MAX";
  op_array_str[3]="PAMI_DATA_MIN";
  op_array_str[4]="PAMI_DATA_SUM";
  op_array_str[5]="PAMI_DATA_PROD";
  op_array_str[6]="PAMI_DATA_LAND";
  op_array_str[7]="PAMI_DATA_LOR";
  op_array_str[8]="PAMI_DATA_LXOR";
  op_array_str[9]="PAMI_DATA_BAND";
  op_array_str[10]="PAMI_DATA_BOR";
  op_array_str[11]="PAMI_DATA_BXOR";
  op_array_str[12]="PAMI_DATA_MAXLOC";
  op_array_str[13]="PAMI_DATA_MINLOC";

  dt_array[0]=PAMI_TYPE_NULL;
  dt_array[1]=PAMI_TYPE_BYTE;
  dt_array[2]=PAMI_TYPE_SIGNED_CHAR;
  dt_array[3]=PAMI_TYPE_UNSIGNED_CHAR;
  dt_array[4]=PAMI_TYPE_SIGNED_SHORT;
  dt_array[5]=PAMI_TYPE_UNSIGNED_SHORT;
  dt_array[6]=PAMI_TYPE_SIGNED_INT;
  dt_array[7]=PAMI_TYPE_UNSIGNED_INT;
  dt_array[8]=PAMI_TYPE_SIGNED_LONG;
  dt_array[9]=PAMI_TYPE_UNSIGNED_LONG;
  dt_array[10]=PAMI_TYPE_SIGNED_LONG_LONG;
  dt_array[11]=PAMI_TYPE_UNSIGNED_LONG_LONG;
  dt_array[12]=PAMI_TYPE_FLOAT;
  dt_array[13]=PAMI_TYPE_DOUBLE;
  dt_array[14]=PAMI_TYPE_LONG_DOUBLE;
  dt_array[15]=PAMI_TYPE_LOGICAL1;
  dt_array[16]=PAMI_TYPE_LOGICAL2;
  dt_array[17]=PAMI_TYPE_LOGICAL4;
  dt_array[18]=PAMI_TYPE_LOGICAL8;
  dt_array[19]=PAMI_TYPE_SINGLE_COMPLEX;
  dt_array[20]=PAMI_TYPE_DOUBLE_COMPLEX;
  dt_array[21]=PAMI_TYPE_LOC_2INT;
  dt_array[22]=PAMI_TYPE_LOC_2FLOAT;
  dt_array[23]=PAMI_TYPE_LOC_2DOUBLE;
  dt_array[24]=PAMI_TYPE_LOC_SHORT_INT;
  dt_array[25]=PAMI_TYPE_LOC_FLOAT_INT;
  dt_array[26]=PAMI_TYPE_LOC_DOUBLE_INT;
  dt_array[27]=PAMI_TYPE_LOC_LONG_INT;
  dt_array[28]=PAMI_TYPE_LOC_LONGDOUBLE_INT;

  dt_array_str[0]="PAMI_TYPE_NULL";
  dt_array_str[1]="PAMI_TYPE_BYTE";
  dt_array_str[2]="PAMI_TYPE_SIGNED_CHAR";
  dt_array_str[3]="PAMI_TYPE_UNSIGNED_CHAR";
  dt_array_str[4]="PAMI_TYPE_SIGNED_SHORT";
  dt_array_str[5]="PAMI_TYPE_UNSIGNED_SHORT";
  dt_array_str[6]="PAMI_TYPE_SIGNED_INT";
  dt_array_str[7]="PAMI_TYPE_UNSIGNED_INT";
  dt_array_str[8]="PAMI_TYPE_SIGNED_LONG";
  dt_array_str[9]="PAMI_TYPE_UNSIGNED_LONG";
  dt_array_str[10]="PAMI_TYPE_SIGNED_LONG_LONG";
  dt_array_str[11]="PAMI_TYPE_UNSIGNED_LONG_LONG";
  dt_array_str[12]="PAMI_TYPE_FLOAT";
  dt_array_str[13]="PAMI_TYPE_DOUBLE";
  dt_array_str[14]="PAMI_TYPE_LONG_DOUBLE";
  dt_array_str[15]="PAMI_TYPE_LOGICAL1";
  dt_array_str[16]="PAMI_TYPE_LOGICAL2";
  dt_array_str[17]="PAMI_TYPE_LOGICAL4";
  dt_array_str[18]="PAMI_TYPE_LOGICAL8";
  dt_array_str[19]="PAMI_TYPE_SINGLE_COMPLEX";
  dt_array_str[20]="PAMI_TYPE_DOUBLE_COMPLEX";
  dt_array_str[21]="PAMI_TYPE_LOC_2INT";
  dt_array_str[22]="PAMI_TYPE_LOC_2FLOAT";
  dt_array_str[23]="PAMI_TYPE_LOC_2DOUBLE";
  dt_array_str[24]="PAMI_TYPE_LOC_SHORT_INT";
  dt_array_str[25]="PAMI_TYPE_LOC_FLOAT_INT";
  dt_array_str[26]="PAMI_TYPE_LOC_DOUBLE_INT";
  dt_array_str[27]="PAMI_TYPE_LOC_LONG_INT";
  dt_array_str[28]="PAMI_TYPE_LOC_LONGDOUBLE_INT";

  xfer_type_str[0]="Broadcast";
  xfer_type_str[1]="Allreduce";
  xfer_type_str[2]="Reduce";
  xfer_type_str[3]="Allgather";
  xfer_type_str[4]="Allgatherv";
  xfer_type_str[5]="Allgatherv_int";
  xfer_type_str[6]="Scatter";
  xfer_type_str[7]="Scatterv";
  xfer_type_str[8]="Scatterv_int";
  xfer_type_str[9]="Gather";
  xfer_type_str[10]="Gatherv";
  xfer_type_str[11]="Gatherv_int";
  xfer_type_str[12]="Barrier";
  xfer_type_str[13]="Alltoall";
  xfer_type_str[14]="Alltoallv";
  xfer_type_str[15]="Alltoallv_int";
  xfer_type_str[16]="Scan";
  xfer_type_str[17]="Reduce_Scatter";
  xfer_type_str[18]="AMBroadcast";
  xfer_type_str[19]="AMScatter";
  xfer_type_str[20]="AMGather";
  xfer_type_str[21]="AMReduce";
}

void cb_done (void *ctxt, void * clientdata, pami_result_t err)
{
  int * active = (int *) clientdata;
  (*active)--;
}

int query_geometry_algorithms_num(pami_geometry_t   geometry,
                                  pami_xfer_type_t  xfer_type,
                                  size_t           *num_algorithm)
{
  pami_result_t result = PAMI_SUCCESS;

  result = PAMI_Geometry_algorithms_num(geometry,
                                        xfer_type,
                                        num_algorithm);

  if (result != PAMI_SUCCESS || num_algorithm[0]==0)
    {
      fprintf (stderr, "Error. Unable to query algorithm, or "
                       "no algorithms available result = %d\n", result);
      result = PAMI_ERROR;
    }
  return result;
}

int query_geometry_algorithms(pami_client_t      client,
                              pami_context_t     context,
                              pami_geometry_t    geometry,
                              pami_xfer_type_t   xfer_type,
                              size_t            *num_algorithm,
                              pami_algorithm_t  *always_works_alg,
                              pami_metadata_t   *always_works_md,
                              pami_algorithm_t  *must_query_alg,
                              pami_metadata_t   *must_query_md)
{
  pami_result_t result = PAMI_SUCCESS;

  result = PAMI_Geometry_algorithms_query(geometry,
                                          xfer_type,
                                          always_works_alg,
                                          always_works_md,
                                          num_algorithm[0],
                                          must_query_alg,
                                          must_query_md,
                                          num_algorithm[1]);
  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to query algorithm. result = %d\n", result);
      result = PAMI_ERROR;
    }
  return result;
}

int query_geometry_algorithm_aw(pami_client_t    client,
                                pami_context_t   context,
                                pami_geometry_t  geometry,
                                pami_xfer_type_t xfer_type,
                                pami_xfer_t     *coll)
{
  size_t num_algo[2] = {1, 0}; //We need only one always works algorithm

  pami_algorithm_t      algo[1];
  pami_algorithm_t      q_algo[1];
  int rc = query_geometry_algorithms(client,
                                     context,
                                     geometry,
                                     xfer_type,
                                     num_algo,
                                     algo,
                                     NULL,
                                     q_algo,
                                     NULL);
  coll->algorithm = algo[0];
  return rc;
}


double timer()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return 1e6*(double)tv.tv_sec + (double)tv.tv_usec;
}


pami_result_t create_geometry(size_t *subgeometry_num_tasks,
	                      pami_task_t *local_task_id,
	                      pami_task_t task_id,
                              pami_task_t *root,
			      size_t geo_size,
                              int id,
			      pami_geometry_t *new_geometry,
                              pami_client_t client,
			      pami_geometry_t world_geometry,
		              pami_context_t context)
{
  pami_geometry_range_t range;
  int rangecount = 1;
  range.lo = 0;
  range.hi = geo_size - 1;

  if(task_id >= geo_size)
  {
    *new_geometry = PAMI_GEOMETRY_NULL;
    return PAMI_SUCCESS;
  }

  *local_task_id = task_id;
  *subgeometry_num_tasks = geo_size;
  *root = 0;

  pami_configuration_t config;
  config.name = PAMI_GEOMETRY_OPTIMIZE;
  int geom_init = 1;
  pami_result_t result;
  result = PAMI_Geometry_create_taskrange(client,
                                          0,
                                          &config,
                                          1,
                                          new_geometry,
                                          world_geometry,
                                          id,
                                          &range,
                                          rangecount,
                                          context,
                                          cb_done,
                                          &geom_init);

  PAMI_assertf(result == PAMI_SUCCESS, "Failed to create geometry");
  while (geom_init)
    result = PAMI_Context_advance (context, 1);

  return result;
}

int destroy_geometry(pami_client_t    client,
                     pami_context_t   context,
                     pami_geometry_t *geometry)
{
  pami_result_t     rc             = PAMI_SUCCESS;
  volatile unsigned geom_poll_flag = 1;
  rc = PAMI_Geometry_destroy(client,
                             geometry,
                             context,
                             cb_done,
                             (void*)&geom_poll_flag);

  PAMI_assertf(rc == PAMI_SUCCESS, "Failed to destroy geometry");

  while(geom_poll_flag)
    rc = PAMI_Context_advance (context, 1);

  return rc;
}

size_t get_type_size(pami_type_t intype)
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
  return sz;
}


/**
 *  *  *  Completion callback
 *   *   */
void cb_amscatter_done (void *context, void * clientdata, pami_result_t err)
{
  validation_t *v = (validation_t*)clientdata;
  volatile unsigned *active = (volatile unsigned *) v->cookie;

  if(_g_verify)
    _gRc |= scatter_check_rcvbuf (_g_recv_buffer, v->bytes, my_task_id);

  (*active)++;
}
/**
 *  *  *  User dispatch function
 *   *   */
void cb_amscatter_recv(pami_context_t        context,      /**< IN:  communication context which invoked the dispatch function */
                      void                 * cookie,       /**< IN:  dispatch cookie */
                      const void           * header_addr,  /**< IN:  header address  */
                      size_t                 header_size,  /**< IN:  header size     */
                      const void           * pipe_addr,    /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
                      size_t                 data_size,    /**< IN:  data size       */
                      pami_endpoint_t        origin,       /**< IN:  root initiating endpoint */
                      pami_geometry_t        geometry,     /**< IN:  Geometry */
                      pami_recv_t          * recv)         /**< OUT: receive message structure */
{

  pami_task_t     task;
  size_t          offset;
  _gRc |= PAMI_Endpoint_query (origin,
                              &task,
                              &offset);

  validation_t *v = _g_val_buffer + task;
  v->buf    = _g_recv_buffer;
  v->cookie = cookie;
  v->bytes  = data_size;
  v->root   = task;

  recv->cookie      = (void*)v;
  recv->local_fn    = cb_amscatter_done;
  recv->addr        = v->buf;
  recv->type        = PAMI_TYPE_BYTE;
  recv->offset      = 0;
  recv->data_fn     = PAMI_DATA_COPY;
  recv->data_cookie = NULL;
}


void cb_ambcast_done (void *context, void * clientdata, pami_result_t err)
{
  validation_t *v = (validation_t*)clientdata;
  volatile unsigned *active = (volatile unsigned *) v->cookie;

  if(_g_verify)
    if(my_task_id != (unsigned)v->root)
    {
      _gRc |= bcast_check_rcvbuf (_g_buffer, v->bytes, v->root);
    }


  (*active)++;
}

void cb_ambcast_recv(pami_context_t        context,      /**< IN:  communication context which invoked the dispatch function */
                      void                 * cookie,       /**< IN:  dispatch cookie */
                      const void           * header_addr,  /**< IN:  header address  */
                      size_t                 header_size,  /**< IN:  header size     */
                      const void           * pipe_addr,    /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
                      size_t                 data_size,    /**< IN:  data size       */
                      pami_endpoint_t        origin,       /**< IN:  root initiating endpoint */
                      pami_geometry_t        geometry,     /**< IN:  Geometry */
                      pami_recv_t          * recv)         /**< OUT: receive message structure */
{
  pami_task_t     task;
  size_t          offset;
  PAMI_Endpoint_query (origin,
                      &task,
                      &offset);

  validation_t *v = _g_val_buffer + task;
  v->buf    = _g_buffer;
  v->cookie = cookie;
  v->bytes  = data_size;
  v->root   = task;

  recv->cookie      = (void*)v;
  recv->local_fn    = cb_ambcast_done;
  recv->addr        = v->buf;
  recv->type        = PAMI_TYPE_BYTE;
  recv->offset      = 0;
  recv->data_fn     = PAMI_DATA_COPY;
  recv->data_cookie = NULL;
}


void cb_amgather_done (void *context, void * clientdata, pami_result_t err)
{
  validation_t *v = (validation_t*)clientdata;
  volatile unsigned *active = (volatile unsigned *) v->cookie;

  if(_g_verify)
    if(my_task_id == (unsigned)v->root)
    {
      _gRc |= gather_check_rcvbuf (num_tasks, _g_recv_buffer, v->bytes);
    }

  (*active)++;
}
/**
 *  User dispatch function
 */
void cb_amgather_send(pami_context_t         context,      /**< IN:  communication context which invoked the dispatch function */
                      void                 * cookie,       /**< IN:  dispatch cookie */
                      const void           * header_addr,  /**< IN:  header address  */
                      size_t                 header_size,  /**< IN:  header size     */
                      size_t                 data_size,    /**< IN:  data size       */
                      pami_endpoint_t        origin,       /**< IN:  root initiating endpoint */
                      pami_geometry_t        geometry,     /**< IN:  Geometry */
                      pami_recv_t          * send)         /**< OUT: receive message structure */
{
  pami_task_t     task;
  size_t          offset;
  _gRc |= PAMI_Endpoint_query (origin,
                              &task,
                              &offset);

  validation_t *v = _g_val_buffer + task;
  v->buf    = _g_send_buffer;
  v->cookie = cookie;
  v->bytes  = data_size;
  v->root   = task;

  send->cookie      = (void*)v;
  send->local_fn    = cb_amgather_done;
  send->addr        = v->buf;
  send->type        = PAMI_TYPE_BYTE;
  send->offset      = 0;
  send->data_fn     = PAMI_DATA_COPY;
  send->data_cookie = NULL;
}


/**
 *  Completion callback
 */
void cb_amreduce_done (void *context, void * clientdata, pami_result_t err)
{
  validation_t *v = (validation_t*)clientdata;
  volatile unsigned *active = (volatile unsigned *) v->cookie;


  if(_g_verify && my_task_id == (unsigned)v->root)
  {
    _gRc |= reduce_check_rcvbuf (_g_recv_buffer, v->bytes, op_array[v->op], dt_array[v->dt], my_task_id, num_tasks);
  }
  (*active)++;
}
/**
 *  User dispatch function
 */
void cb_amreduce_send(pami_context_t         context,      /**< IN:  communication context which invoked the dispatch function */
                      void                 * cookie,       /**< IN:  dispatch cookie */
                      const void           * header_addr,  /**< IN:  header address  */
                      size_t                 header_size,  /**< IN:  header size     */
                      size_t                 data_size,    /**< IN:  data size       */
                      pami_endpoint_t        origin,       /**< IN:  root initiating endpoint */
                      pami_geometry_t        geometry,     /**< IN:  Geometry */
                      pami_data_function   * op,           /**< OUT: PAMI math operation to perform on the datatype */
                      pami_recv_t          * send)         /**< OUT: receive message structure */
{
  reduce_user_header_t *hdr;
  hdr = (reduce_user_header_t *) header_addr;

  pami_task_t     task;
  size_t          offset;
  _gRc |= PAMI_Endpoint_query (origin,
                              &task,
                              &offset);

  validation_t *v = _g_val_buffer + task;
  v->cookie = cookie;
  v->bytes  = data_size;
  v->op     = hdr->op;
  v->dt     = hdr->dt;
  v->root = task;

  send->cookie      = (void*)v;
  send->local_fn    = cb_amreduce_done;
  send->addr        = _g_send_buffer;
  send->type        = PAMI_TYPE_UNSIGNED_CHAR;
  send->offset      = 0;
  send->data_fn     = PAMI_DATA_COPY;
  send->data_cookie = NULL;
  *op = op_array[hdr->op];
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

size_t ** alloc2DContig(int nrows, int ncols)
{
  int i;
  size_t **array;
  array        = (size_t**)malloc(nrows * sizeof(size_t*));
  assert(array);
  array[0]     = (size_t *)calloc(nrows * ncols, sizeof(size_t));
  assert(array[0]);

  for (i = 1; i < nrows; i++)
      array[i]   = array[0] + i * ncols;

  return array;
}

void setup_op_dt(size_t ** validTable,char* sDt, char* sOp)
{

  /* \todo We *could* change gMax_datatype_sz based on dt selected */

  int i,j;
  unsigned force = 0; /* don't force the dt/op selected */

  if (gFull_test)
  {
    for (i = 0; i < op_count; i++)
      for (j = 0; j < dt_count; j++)
        validTable[i][j] = 1;
      
  }
  else if (sDt && sOp)
  {
    force = 1; /* force the dt/op*/
    for (i = 0; i < op_count; i++)
      for (j = 0; j < dt_count; j++)
          if (!strcmp(sDt, dt_array_str[j]) &&
              !strcmp(sOp, op_array_str[i]))
            validTable[i][j] = 1;
          else
            validTable[i][j] = 0;
  } 
  else if (sOp)
  {
    for (i = 0; i < op_count; i++)
      for (j = 0; j < dt_count; j++)
        if (!strcmp(sOp, op_array_str[i]))
          validTable[i][j] = 1;
        else
          validTable[i][j] = 0;
  }
  else if (sDt)
  {
    for (i = 0; i < op_count; i++)
      for (j = 0; j < dt_count; j++)
        if (!strcmp(sDt, dt_array_str[j]))
          validTable[i][j] = 1;
        else
          validTable[i][j] = 0;
  }
  else  /* minimal/default test */
  {
    for (i = 0; i < op_count; i++)
      for (j = 0; j < dt_count; j++)
        validTable[i][j] = 0;
  
      validTable[OP_SUM][DT_SIGNED_INT] = 1;
      validTable[OP_MAX][DT_SIGNED_INT] = 1;
      validTable[OP_MIN][DT_SIGNED_INT] = 1;
      validTable[OP_SUM][DT_UNSIGNED_INT] = 1;
      validTable[OP_MAX][DT_UNSIGNED_INT] = 1;
      validTable[OP_MIN][DT_UNSIGNED_INT] = 1;
      validTable[OP_SUM][DT_DOUBLE] = 1;
      validTable[OP_MAX][DT_DOUBLE] = 1;
      validTable[OP_MIN][DT_DOUBLE] = 1;
  
  }
  if(!force) /* not forcing the op/dt*/
  {
    /*--------------------------------------*/
    /* Disable unsupported ops on complex   */
    /* Only sum, prod                       */
    for (i = 0, j = DT_SINGLE_COMPLEX; i < OP_COUNT; i++)if(i!=OP_SUM && i!=OP_PROD) validTable[i][j] = 0;
    for (i = 0, j = DT_DOUBLE_COMPLEX; i < OP_COUNT; i++)if(i!=OP_SUM && i!=OP_PROD) validTable[i][j] = 0; 
      
    /*--------------------------------------*/
    /* Disable NULL and byte operations     */
    for (i = 0, j = DT_NULL; i < OP_COUNT; i++) validTable[i][j] = 0;
    for (i = 0, j = DT_BYTE; i < OP_COUNT; i++) validTable[i][j] = 0;
    for (j = 0, i = OP_COPY; j < DT_COUNT; j++) validTable[i][j] = 0;
    for (j = 0, i = OP_NOOP; j < DT_COUNT; j++) validTable[i][j] = 0;
    
    /*--------------------------------------*/
    /* Disable non-LOC ops on LOC dt's      */
    for (i = 0, j = DT_LOC_2INT      ; i < OP_MAXLOC; i++)validTable[i][j] = 0;
    for (i = 0, j = DT_LOC_SHORT_INT ; i < OP_MAXLOC; i++)validTable[i][j] = 0;
    for (i = 0, j = DT_LOC_FLOAT_INT ; i < OP_MAXLOC; i++)validTable[i][j] = 0;
    for (i = 0, j = DT_LOC_DOUBLE_INT; i < OP_MAXLOC; i++)validTable[i][j] = 0;
    for (i = 0, j = DT_LOC_LONG_INT  ; i < OP_MAXLOC; i++)validTable[i][j] = 0;
    for (i = 0, j = DT_LOC_LONGDOUBLE_INT; i < OP_MAXLOC; i++)validTable[i][j] = 0;
    for (i = 0, j = DT_LOC_2FLOAT    ; i < OP_MAXLOC; i++)validTable[i][j] = 0;
    for (i = 0, j = DT_LOC_2DOUBLE   ; i < OP_MAXLOC; i++)validTable[i][j] = 0;
      
    /*--------------------------------------*/
    /* Disable LOC ops on non-LOC dt's      */
    for (j = 0, i = OP_MAXLOC; j < DT_LOC_2INT; j++) validTable[i][j] = 0;
    for (j = 0, i = OP_MINLOC; j < DT_LOC_2INT; j++) validTable[i][j] = 0;
      
    /*---------------------------------------*/
    /* Disable unsupported ops on logical dt */
    /* Only land, lor, lxor, band, bor, bxor */
    for (i = 0,         j = DT_LOGICAL1; i < OP_LAND ; i++) validTable[i][j] = 0;
    for (i = OP_BXOR+1, j = DT_LOGICAL1; i < OP_COUNT; i++) validTable[i][j] = 0;
    for (i = 0,         j = DT_LOGICAL2; i < OP_LAND ; i++) validTable[i][j] = 0;
    for (i = OP_BXOR+1, j = DT_LOGICAL2; i < OP_COUNT; i++) validTable[i][j] = 0;
    for (i = 0,         j = DT_LOGICAL4; i < OP_LAND ; i++) validTable[i][j] = 0;
    for (i = OP_BXOR+1, j = DT_LOGICAL4; i < OP_COUNT; i++) validTable[i][j] = 0;
    for (i = 0,         j = DT_LOGICAL8; i < OP_LAND ; i++) validTable[i][j] = 0;
    for (i = OP_BXOR+1, j = DT_LOGICAL8; i < OP_COUNT; i++) validTable[i][j] = 0;
    
    /*---------------------------------------*/
    /* Disable unsupported ops on long double*/
    /* Only max,min,sum,prod                 */
    for (i = OP_PROD+1, j = DT_LONG_DOUBLE; i < OP_COUNT; i++) validTable[i][j] = 0;
  }
}

metadata_result_t check_metadata(pami_metadata_t md,
                                 pami_xfer_t xfer,
                                 pami_type_t s_dt, size_t s_size, char* s_buffer,
                                 pami_type_t r_dt, size_t r_size, char* r_buffer)
{
  metadata_result_t result;
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
      result.check.align_send_buffer = (((size_t)s_buffer & (size_t)mask) == 0) ? 0:1;
    }
    if(md.check_correct.values.recvminalign)
    {
      mask  = md.recv_min_align - 1;
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
  return result;
}


size_t get_msg_thresh(size_t byte_thresh, pami_xfer_type_t coll_xfer, size_t ntasks)
{
  size_t ret = 0;
  switch ( coll_xfer )
  {
    case PAMI_XFER_BROADCAST:
    {
      ret = byte_thresh;
      break;
    }
    case PAMI_XFER_ALLREDUCE:
    {
      ret = byte_thresh / 2;
      break;
    }
    case PAMI_XFER_REDUCE:
    {
      ret = byte_thresh / 2;
      break;
    }
    case PAMI_XFER_ALLGATHER:
    {
      ret = byte_thresh / (ntasks + 1);
      break;
    }
    case PAMI_XFER_ALLGATHERV:
    {
      ret = byte_thresh / (ntasks + 1);
      break;
    }
    case PAMI_XFER_ALLGATHERV_INT:
    {
      ret = byte_thresh / (ntasks + 1);
      break;
    }
    case PAMI_XFER_SCATTER:
    {
      ret = byte_thresh / (ntasks + 1);
      break;
    }
    case PAMI_XFER_SCATTERV:
    {
      ret = byte_thresh / (ntasks + 1);
      break;
    }
    case PAMI_XFER_SCATTERV_INT:
    {
      ret = byte_thresh / (ntasks + 1);
      break;
    }
    case PAMI_XFER_GATHER:
    {
      ret = byte_thresh / (ntasks + 1);
      break;
    }
    case PAMI_XFER_GATHERV:
    {
      ret = byte_thresh / (ntasks + 1);
      break;
    }
    case PAMI_XFER_GATHERV_INT:
    {
      ret = byte_thresh / (ntasks + 1);
      break;
    }
    case PAMI_XFER_BARRIER:
    {
      ret = 0;
      break;
    }
    case PAMI_XFER_ALLTOALL:
    {
      ret = byte_thresh / ntasks / 2;
      break;
    }
    case PAMI_XFER_ALLTOALLV:
    {
      ret = byte_thresh / ntasks / 2;
      break;
    }
    case PAMI_XFER_ALLTOALLV_INT:
    {
      ret = byte_thresh / ntasks / 2;
      break;
    }
    case  PAMI_XFER_SCAN:
    {
      ret = byte_thresh / 2;
      break;
    }
    case PAMI_XFER_REDUCE_SCATTER:
    {
      ret = byte_thresh / 2;
      break;
    }
    case PAMI_XFER_AMBROADCAST:
    {
      ret = byte_thresh;
      break;
    }
    case PAMI_XFER_AMSCATTER:
    {
      ret = byte_thresh / (ntasks + 1);
      break;
    }
    case PAMI_XFER_AMGATHER:
    {
      ret = byte_thresh / (ntasks + 1);
      break;
    }
    case PAMI_XFER_AMREDUCE:
    {
      ret = byte_thresh / 2;
      break;
    }
    default:
    {
      printf("Invalid pami_xfer_type %d\n", coll_xfer);
      break;
    }
  }//end switch
  return ret;
}

void coll_mem_alloc(pami_xfer_t * coll, pami_xfer_type_t coll_xfer, size_t msg_size, size_t ntasks)
{
  switch ( coll_xfer )
  {
    case PAMI_XFER_BROADCAST:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void* buf = NULL;
      err = posix_memalign(&buf, 128, msg_size);
      assert(err == 0);

      coll[0].cmd.xfer_broadcast.buf = (char *)buf;
      break;
    }
    case PAMI_XFER_ALLREDUCE:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void* sbuf = NULL;
      err = posix_memalign(&sbuf, 128, msg_size);
      assert(err == 0);
      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, msg_size);
      assert(err == 0);

      coll[0].cmd.xfer_allreduce.sndbuf = (char *)sbuf;
      coll[0].cmd.xfer_allreduce.rcvbuf = (char *)rbuf;
      break;
    }
    case PAMI_XFER_REDUCE:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void* sbuf = NULL;
      err = posix_memalign(&sbuf, 128, msg_size);
      assert(err == 0);
      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, msg_size);
      assert(err == 0);
   
      coll[0].cmd.xfer_reduce.sndbuf = (char *)sbuf;
      coll[0].cmd.xfer_reduce.rcvbuf = (char *)rbuf;
      break;
    }
    case PAMI_XFER_ALLGATHER:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void* sbuf = NULL;
      err = posix_memalign(&sbuf, 128, msg_size);
      assert(err == 0);
      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, (msg_size * ntasks));
      assert(err == 0);
   
      coll[0].cmd.xfer_allgather.sndbuf = (char *)sbuf;
      coll[0].cmd.xfer_allgather.rcvbuf = (char *)rbuf;
      break;
    }
    case PAMI_XFER_ALLGATHERV:
    {
       /*  Allocate buffer(s) */
      int err = 0;
      void* buf = NULL;
      err = posix_memalign(&buf, 128, msg_size);
      assert(err == 0);

      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, (msg_size * ntasks));
      assert(err == 0);

      size_t *lengths   = (size_t*)malloc(ntasks * sizeof(size_t));
      assert(lengths);
      size_t    *displs    = (size_t*)malloc(ntasks * sizeof(size_t));
      assert(displs);

      coll[0].cmd.xfer_allgatherv.sndbuf     = (char *)buf;
      coll[0].cmd.xfer_allgatherv.rcvbuf     = (char *)rbuf;
      coll[0].cmd.xfer_allgatherv.rtypecounts = lengths;
      coll[0].cmd.xfer_allgatherv.rdispls     = displs;
      break;
    }
    case PAMI_XFER_ALLGATHERV_INT:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void* buf = NULL;
      err = posix_memalign(&buf, 128, (msg_size));
      assert(err == 0);

      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, (msg_size * ntasks));
      assert(err == 0);

      int *lengths   = (int *)malloc(ntasks * sizeof(int));
      assert(lengths);
      int *displs    = (int *)malloc(ntasks * sizeof(int));
      assert(displs);

      coll[0].cmd.xfer_allgatherv_int.sndbuf     = (char *)buf;
      coll[0].cmd.xfer_allgatherv_int.rcvbuf     = (char *)rbuf;
      coll[0].cmd.xfer_allgatherv_int.rtypecounts = lengths;
      coll[0].cmd.xfer_allgatherv_int.rdispls     = displs;
      break;
    }
    case PAMI_XFER_SCATTER:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void* sbuf = NULL;
      err = posix_memalign(&sbuf, 128, (msg_size * ntasks));
      assert(err == 0);
      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, msg_size);
      assert(err == 0);

      coll[0].cmd.xfer_scatter.sndbuf = (char *)sbuf;
      coll[0].cmd.xfer_scatter.rcvbuf = (char *)rbuf;
      break;
    }
    case PAMI_XFER_SCATTERV:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void* buf = NULL;
      err = posix_memalign(&buf, 128, (msg_size * ntasks));
      assert(err == 0);

      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, msg_size * (ntasks));
      assert(err == 0);

      size_t *lengths   = (size_t*)malloc(ntasks * sizeof(size_t));
      assert(lengths);
      size_t *displs    = (size_t*)malloc(ntasks * sizeof(size_t));
      assert(displs);

    
      coll[0].cmd.xfer_scatterv.sndbuf      = (char *)buf;
      coll[0].cmd.xfer_scatterv.stypecounts = lengths;
      coll[0].cmd.xfer_scatterv.sdispls     = displs;
      coll[0].cmd.xfer_scatterv.rcvbuf      = (char *)rbuf;
      break;
    }
    case PAMI_XFER_SCATTERV_INT:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void* buf = NULL;
      err = posix_memalign(&buf, 128, (msg_size * ntasks));
      assert(err == 0);

      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, (msg_size * ntasks));
      assert(err == 0);

      int *lengths   = (int*)malloc(ntasks * sizeof(int));
      assert(lengths);
      int *displs    = (int*)malloc(ntasks * sizeof(int));
      assert(displs);


      coll[0].cmd.xfer_scatterv_int.sndbuf      = (char *)buf;
      coll[0].cmd.xfer_scatterv_int.stypecounts = lengths;
      coll[0].cmd.xfer_scatterv_int.sdispls     = displs;
      coll[0].cmd.xfer_scatterv_int.rcvbuf      = (char *)rbuf;
      break;
    }
    case PAMI_XFER_GATHER:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void* sbuf = NULL;
      err = posix_memalign(&sbuf, 128, msg_size);
      assert(err == 0);
      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, (msg_size * ntasks));
      assert(err == 0);

      coll[0].cmd.xfer_gather.sndbuf = (char *)sbuf;
      coll[0].cmd.xfer_gather.rcvbuf = (char *)rbuf;
      break;
    }
    case PAMI_XFER_GATHERV:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void* buf = NULL;
      err = posix_memalign(&buf, 128, (msg_size * ntasks));
      assert(err == 0);

      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, (msg_size * ntasks));
      assert(err == 0);

      size_t *lengths   = (size_t*)malloc(ntasks * sizeof(size_t));
      assert(lengths);
      size_t *displs    = (size_t*)malloc(ntasks * sizeof(size_t));
      assert(displs);


      coll[0].cmd.xfer_gatherv.sndbuf      = (char *)buf;
      coll[0].cmd.xfer_gatherv.rcvbuf      = (char *)rbuf;
      coll[0].cmd.xfer_gatherv.rtypecounts = lengths;
      coll[0].cmd.xfer_gatherv.rdispls     = displs;
      break;
    }
    case PAMI_XFER_GATHERV_INT:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void* buf = NULL;
      err = posix_memalign(&buf, 128, (msg_size * ntasks));
      assert(err == 0);

      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, (msg_size * ntasks));
      assert(err == 0);

      int *lengths   = (int *)malloc(ntasks * sizeof(int));
      assert(lengths);
      int *displs    = (int*)malloc(ntasks * sizeof(int));
      assert(displs);


      coll[0].cmd.xfer_gatherv_int.sndbuf      = (char *)buf;
      coll[0].cmd.xfer_gatherv_int.rcvbuf      = (char *)rbuf;
      coll[0].cmd.xfer_gatherv_int.rtypecounts = lengths;
      coll[0].cmd.xfer_gatherv_int.rdispls     = displs;
      break;
    }
    case PAMI_XFER_BARRIER:
    {
      break;
    }
    case PAMI_XFER_ALLTOALL:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void* sbuf = NULL;
      err = posix_memalign(&sbuf, 128, (msg_size*ntasks));
      assert(err == 0);
      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, (msg_size*ntasks));
      assert(err == 0);
    
      coll[0].cmd.xfer_alltoall.sndbuf = (char *)sbuf;
      coll[0].cmd.xfer_alltoall.rcvbuf = (char *)rbuf;
      break;
    }
    case PAMI_XFER_ALLTOALLV:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void* sbuf = NULL;
      err = posix_memalign(&sbuf, 128, (msg_size * ntasks));
      assert(err == 0);

      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, (msg_size * ntasks));
      assert(err == 0);

      size_t *sndlens = (size_t*) malloc(ntasks * sizeof(size_t));
      assert(sndlens);
      size_t *sdispls = (size_t*) malloc(ntasks * sizeof(size_t));
      assert(sdispls);
      size_t *rcvlens = (size_t*) malloc(ntasks * sizeof(size_t));
      assert(rcvlens);
      size_t *rdispls = (size_t*) malloc(ntasks * sizeof(size_t));
      assert(rdispls);

   
      coll[0].cmd.xfer_alltoallv.sndbuf        = (char *)sbuf;
      coll[0].cmd.xfer_alltoallv.stypecounts   = sndlens;
      coll[0].cmd.xfer_alltoallv.sdispls       = sdispls;
      coll[0].cmd.xfer_alltoallv.rcvbuf        = (char *)rbuf;
      coll[0].cmd.xfer_alltoallv.rtypecounts   = rcvlens;
      coll[0].cmd.xfer_alltoallv.rdispls       = rdispls;
      break;
    }
    case PAMI_XFER_ALLTOALLV_INT:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void* sbuf = NULL;
      err = posix_memalign(&sbuf, 128, (msg_size * ntasks));
      assert(err == 0);

      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, (msg_size * ntasks));
      assert(err == 0);

      int *sndlens = (int*) malloc(ntasks * sizeof(int));
      assert(sndlens);
      int *sdispls = (int*) malloc(ntasks * sizeof(int));
      assert(sdispls);
      int *rcvlens = (int*) malloc(ntasks * sizeof(int));
      assert(rcvlens);
      int *rdispls = (int*) malloc(ntasks * sizeof(int));
      assert(rdispls);
 

      coll[0].cmd.xfer_alltoallv_int.sndbuf        = (char *)sbuf;
      coll[0].cmd.xfer_alltoallv_int.stypecounts   = sndlens;
      coll[0].cmd.xfer_alltoallv_int.sdispls       = sdispls;
      coll[0].cmd.xfer_alltoallv_int.rcvbuf        = (char *)rbuf;
      coll[0].cmd.xfer_alltoallv_int.rtypecounts   = rcvlens;
      coll[0].cmd.xfer_alltoallv_int.rdispls       = rdispls;
      break;
    }
    case  PAMI_XFER_SCAN:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void* sbuf = NULL;
      err = posix_memalign(&sbuf, 128, msg_size);
      assert(err == 0);
      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, msg_size);
      assert(err == 0);
    
      coll[0].cmd.xfer_scan.sndbuf = (char *)sbuf;
      coll[0].cmd.xfer_scan.rcvbuf = (char *)rbuf;
      break;
    }
    case PAMI_XFER_REDUCE_SCATTER:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void* sbuf = NULL;
      err = posix_memalign(&sbuf, 128, ntasks*msg_size);
      assert(err == 0);
      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, ntasks*msg_size);
      assert(err == 0);

      size_t *rcounts = (size_t*)malloc(sizeof(size_t)*ntasks);
      assert(rcounts != NULL);

      coll[0].cmd.xfer_reduce_scatter.sndbuf = (char *)sbuf;
      coll[0].cmd.xfer_reduce_scatter.rcvbuf = (char *)rbuf;
      coll[0].cmd.xfer_reduce_scatter.rcounts = &rcounts[0];
      break;
    }
    case PAMI_XFER_AMBROADCAST:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void *buf = NULL;
      err = posix_memalign(&buf, 128, msg_size);
      assert(err == 0);
      _g_buffer = (char *)buf;
      coll[0].cmd.xfer_ambroadcast.sndbuf = (char*)buf;
      user_header_t *header = NULL;
      err = posix_memalign((void**)&header, 128, sizeof(user_header_t));
      header->cookie = 100;
      coll[0].cmd.xfer_ambroadcast.user_header = header;
      validation_t *v = NULL;
      err = posix_memalign((void **)&v, 128, sizeof(validation_t));
      coll[0].cookie = v;

      void *validation = NULL;
      err = posix_memalign((void **)&validation, 128, (ntasks * sizeof(validation_t)));
      _g_val_buffer  = (validation_t *)validation;

      break;
    }
    case PAMI_XFER_AMSCATTER:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void *sbuf = NULL;
      err = posix_memalign(&sbuf, 128, (msg_size * ntasks));
      assert(err == 0);

      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, msg_size);
      assert(err == 0);

      void *headers = NULL;
      err = posix_memalign((void **)&headers, 128, (ntasks * sizeof(user_header_t)));

      void *validation = NULL;
      err = posix_memalign((void **)&validation, 128, (ntasks * sizeof(validation_t)));

      /* Initialize the headers */
      size_t i;
      for(i = 0; i < ntasks; ++i)
      {
        ((user_header_t *)headers)[i].cookie = i;
      }

      _g_recv_buffer = (char *)rbuf;
      _g_send_buffer = (char  *)sbuf;
      _g_val_buffer  = (PAMI::validation_t*)validation;
      coll[0].cmd.xfer_amscatter.headers      = headers;
 
      break;
    }
    case PAMI_XFER_AMGATHER:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void *sbuf = NULL;
      err = posix_memalign(&sbuf, 128, msg_size);
      assert(err == 0);

      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, (msg_size * ntasks));
      assert(err == 0);

      void *headers = NULL;
      err = posix_memalign((void **)&headers, 128, (ntasks * sizeof(user_header_t)));

      void *validation = NULL;
      err = posix_memalign((void **)&validation, 128, (ntasks * sizeof(validation_t)));

      /* Initialize the headers */
      size_t i;
      for(i = 0; i < ntasks; ++i)
      {
        ((user_header_t *)headers)[i].cookie = i;
      }

      _g_recv_buffer = (char *)rbuf;
      _g_send_buffer = (char  *)sbuf;
      _g_val_buffer  = (PAMI::validation_t*)validation;
      coll[0].cmd.xfer_amgather.headers      = headers;
      break;
    }
    case PAMI_XFER_AMREDUCE:
    {
      /*  Allocate buffer(s) */
      int err = 0;
      void *sbuf = NULL;
      err = posix_memalign(&sbuf, 128, msg_size);
      assert(err == 0);

      void* rbuf = NULL;
      err = posix_memalign(&rbuf, 128, (msg_size * ntasks));
      assert(err == 0);

      void *user_header = NULL;
      err = posix_memalign((void **)&user_header, 128, sizeof(reduce_user_header_t));

      void *validation = NULL;
      err = posix_memalign((void **)&validation, 128, (ntasks * sizeof(validation_t)));


      _g_recv_buffer = (char *)rbuf;
      _g_send_buffer = (char  *)sbuf;
      _g_val_buffer  = (PAMI::validation_t*)validation;
      coll[0].cmd.xfer_amreduce.user_header      = user_header;
      break;
    }
    default:
    {
      printf("Invalid pami_xfer_type %d\n", coll_xfer);
      break;
    }
  }//end switch

}

void fill_coll(pami_client_t client,
               pami_context_t context,
	       pami_xfer_t *coll,
	       pami_xfer_type_t coll_xfer,
	       size_t *low,
	       size_t *high,
	       size_t l,
	       size_t msg_size,
               size_t task,
	       size_t ntasks,
               size_t root_zero,
	       size_t *col_num_algo,
               metadata_result_t *result,
	       pami_algorithm_t *col_algo,
	       pami_metadata_t *col_md,
	       pami_algorithm_t *q_col_algo,
	       pami_metadata_t *q_col_md)
{
  coll[0].cb_done = cb_done;
  pami_metadata_t *coll_metadata = NULL;
  my_task_id = task;

  if(l<col_num_algo[0])
  {
    coll[0].algorithm = col_algo[l];
    coll_metadata = &col_md[l];
    *low = col_md[l].range_lo;
    *high = col_md[l].range_hi;
  }
  else
  {
    size_t index = l - col_num_algo[0];
    coll[0].algorithm = q_col_algo[index];
    coll_metadata = &q_col_md[index];
    *low = q_col_md[index].range_lo;
    *high = q_col_md[index].range_hi;
  }

  pami_endpoint_t root_ep;
  PAMI_Endpoint_create(client, root_zero, 0, &root_ep); //TODO: Use the correct context offset
  coll[2].cmd.xfer_reduce.root = root_ep;

  switch ( coll_xfer )
  {
    case PAMI_XFER_BROADCAST:
    {
      coll[0].cmd.xfer_broadcast.type = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_broadcast.typecount = msg_size;
      coll[0].cmd.xfer_broadcast.root = root_ep;
      *result = check_metadata(*coll_metadata,
		               coll[0],
		               coll[0].cmd.xfer_broadcast.type,
			       coll[0].cmd.xfer_broadcast.typecount,
	                       coll[0].cmd.xfer_broadcast.buf,
			       coll[0].cmd.xfer_broadcast.type,
			       coll[0].cmd.xfer_broadcast.typecount,
	                       coll[0].cmd.xfer_broadcast.buf);
      break;
    }
    case PAMI_XFER_ALLREDUCE:
    {
      coll[0].cmd.xfer_allreduce.stype = PAMI_TYPE_SIGNED_CHAR;
      coll[0].cmd.xfer_allreduce.rtype = PAMI_TYPE_SIGNED_CHAR;
      coll[0].cmd.xfer_allreduce.stypecount = msg_size;
      coll[0].cmd.xfer_allreduce.rtypecount = msg_size;
      coll[0].cmd.xfer_allreduce.op = PAMI_DATA_SUM;
      *result = check_metadata(*coll_metadata,
		               coll[0],
		               coll[0].cmd.xfer_allreduce.stype,
			       coll[0].cmd.xfer_allreduce.stypecount,
	                       coll[0].cmd.xfer_allreduce.sndbuf,
			       coll[0].cmd.xfer_allreduce.rtype,
			       coll[0].cmd.xfer_allreduce.rtypecount,
	                       coll[0].cmd.xfer_allreduce.rcvbuf);
      break;
    }
    case PAMI_XFER_REDUCE:
    {
      coll[0].cmd.xfer_reduce.stype = PAMI_TYPE_SIGNED_CHAR;
      coll[0].cmd.xfer_reduce.rtype = PAMI_TYPE_SIGNED_CHAR;
      coll[0].cmd.xfer_reduce.rtypecount = msg_size;
      coll[0].cmd.xfer_reduce.stypecount = msg_size;
      coll[0].cmd.xfer_reduce.op = PAMI_DATA_SUM;
      coll[0].cmd.xfer_reduce.root    = root_ep;
      *result = check_metadata(*coll_metadata,
		               coll[0],
		               coll[0].cmd.xfer_reduce.stype,
			       coll[0].cmd.xfer_reduce.stypecount,
	                       coll[0].cmd.xfer_reduce.sndbuf,
			       coll[0].cmd.xfer_reduce.rtype,
			       coll[0].cmd.xfer_reduce.rtypecount,
	                       coll[0].cmd.xfer_reduce.rcvbuf);
      break;
    }
    case PAMI_XFER_ALLGATHER:
    {
      coll[0].cmd.xfer_allgather.stype = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_allgather.rtype = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_allgather.stypecount = msg_size;
      coll[0].cmd.xfer_allgather.rtypecount = msg_size;
      *result = check_metadata(*coll_metadata,
		               coll[0],
		               coll[0].cmd.xfer_allgather.stype,
			       coll[0].cmd.xfer_allgather.stypecount,
	                       coll[0].cmd.xfer_allgather.sndbuf,
			       coll[0].cmd.xfer_allgather.rtype,
			       coll[0].cmd.xfer_allgather.rtypecount,
	                       coll[0].cmd.xfer_allgather.rcvbuf);
      break;
    }
    case PAMI_XFER_ALLGATHERV:
    {
      coll[0].cmd.xfer_allgatherv.stype      = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_allgatherv.stypecount = msg_size;
      coll[0].cmd.xfer_allgatherv.rtype      = PAMI_TYPE_BYTE;

      size_t k;
      for (k = 0; k < ntasks; k++)
        coll[0].cmd.xfer_allgatherv.rtypecounts[k] = msg_size;
      for (k = 0; k < ntasks; k++)
        coll[0].cmd.xfer_allgatherv.rdispls[k]  = k*msg_size;

      *result = check_metadata(*coll_metadata,
		               coll[0],
		               coll[0].cmd.xfer_allgatherv.stype,
			       coll[0].cmd.xfer_allgatherv.stypecount,
	                       coll[0].cmd.xfer_allgatherv.sndbuf,
			       coll[0].cmd.xfer_allgatherv.rtype,
			       coll[0].cmd.xfer_allgatherv.stypecount,
	                       coll[0].cmd.xfer_allgatherv.rcvbuf);
   
      break;
    }
    case PAMI_XFER_ALLGATHERV_INT:
    {
      coll[0].cmd.xfer_allgatherv_int.stype      = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_allgatherv_int.stypecount = msg_size;
      coll[0].cmd.xfer_allgatherv_int.rtype      = PAMI_TYPE_BYTE;

      size_t k;
      for (k = 0; k < ntasks; k++)
        coll[0].cmd.xfer_allgatherv_int.rtypecounts[k] = msg_size;
      for (k = 0; k < ntasks; k++)
        coll[0].cmd.xfer_allgatherv_int.rdispls[k]  = k*msg_size;

      *result = check_metadata(*coll_metadata,
		               coll[0],
		               coll[0].cmd.xfer_allgatherv_int.stype,
			       coll[0].cmd.xfer_allgatherv_int.stypecount,
	                       coll[0].cmd.xfer_allgatherv_int.sndbuf,
			       coll[0].cmd.xfer_allgatherv_int.rtype,
			       coll[0].cmd.xfer_allgatherv_int.stypecount,
	                       coll[0].cmd.xfer_allgatherv_int.rcvbuf);
      break;
    }
    case PAMI_XFER_SCATTER:
    {
      coll[0].cmd.xfer_scatter.stype = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_scatter.rtype = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_scatter.stypecount = msg_size;
      coll[0].cmd.xfer_scatter.rtypecount = msg_size;
      coll[0].cmd.xfer_scatter.root = root_ep;
      *result = check_metadata(*coll_metadata,
		               coll[0],
		               coll[0].cmd.xfer_scatter.stype,
			       coll[0].cmd.xfer_scatter.stypecount,
	                       coll[0].cmd.xfer_scatter.sndbuf,
			       coll[0].cmd.xfer_scatter.rtype,
			       coll[0].cmd.xfer_scatter.rtypecount,
	                       coll[0].cmd.xfer_scatter.rcvbuf);
      break;
    }
    case PAMI_XFER_SCATTERV:
    {
      coll[0].cmd.xfer_scatterv.stype       = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_scatterv.rtype       = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_scatterv.rtypecount  = msg_size;
      coll[0].cmd.xfer_scatterv.root = root_ep;
      size_t k;
      for (k = 0; k < ntasks; k++)
      {
        coll[0].cmd.xfer_scatterv.stypecounts[k] = msg_size;
        coll[0].cmd.xfer_scatterv.sdispls[k]  = k * msg_size;
      }

      *result = check_metadata(*coll_metadata,
		               coll[0],
		               coll[0].cmd.xfer_scatterv.stype,
			       coll[0].cmd.xfer_scatterv.stypecounts[0],
	                       coll[0].cmd.xfer_scatterv.sndbuf,
			       coll[0].cmd.xfer_scatterv.rtype,
			       coll[0].cmd.xfer_scatterv.rtypecount,
	                       coll[0].cmd.xfer_scatterv.rcvbuf);
      break;
    }
    case PAMI_XFER_SCATTERV_INT:
    {
      coll[0].cmd.xfer_scatterv_int.stype       = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_scatterv_int.rtype       = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_scatterv_int.rtypecount  = msg_size;
      coll[0].cmd.xfer_scatterv_int.root = root_ep;
      size_t k;
      for (k = 0; k < ntasks; k++)
      {
        coll[0].cmd.xfer_scatterv_int.stypecounts[k] = msg_size;
        coll[0].cmd.xfer_scatterv_int.sdispls[k]  = k * msg_size;
      }

      *result = check_metadata(*coll_metadata,
		               coll[0],
		               coll[0].cmd.xfer_scatterv_int.stype,
			       coll[0].cmd.xfer_scatterv_int.stypecounts[0],
	                       coll[0].cmd.xfer_scatterv_int.sndbuf,
			       coll[0].cmd.xfer_scatterv_int.rtype,
			       coll[0].cmd.xfer_scatterv_int.rtypecount,
	                       coll[0].cmd.xfer_scatterv_int.rcvbuf);
      break;
    }
    case PAMI_XFER_GATHER:
    {
      coll[0].cmd.xfer_gather.stype = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_gather.rtype = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_gather.stypecount = msg_size;
      coll[0].cmd.xfer_gather.rtypecount = msg_size;
      coll[0].cmd.xfer_gather.root = root_ep;
      *result = check_metadata(*coll_metadata,
		               coll[0],
		               coll[0].cmd.xfer_gather.stype,
			       coll[0].cmd.xfer_gather.stypecount,
	                       coll[0].cmd.xfer_gather.sndbuf,
			       coll[0].cmd.xfer_gather.rtype,
			       coll[0].cmd.xfer_gather.rtypecount,
	                       coll[0].cmd.xfer_gather.rcvbuf);
      break;
    }
    case PAMI_XFER_GATHERV:
    {
      coll[0].cmd.xfer_gatherv.stype       = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_gatherv.stypecount  = msg_size;
      coll[0].cmd.xfer_gatherv.rtype       = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_gatherv.root = root_ep;

      size_t k = 0;
      for (k = 0; k < ntasks; k++)
      {
        coll[0].cmd.xfer_gatherv.rtypecounts[k] = msg_size;
        coll[0].cmd.xfer_gatherv.rdispls[k]  = k * msg_size;
      }

      *result = check_metadata(*coll_metadata,
		               coll[0],
		               coll[0].cmd.xfer_gatherv.stype,
			       coll[0].cmd.xfer_gatherv.stypecount,
	                       coll[0].cmd.xfer_gatherv.sndbuf,
			       coll[0].cmd.xfer_gatherv.rtype,
			       coll[0].cmd.xfer_gatherv.stypecount,
	                       coll[0].cmd.xfer_gatherv.rcvbuf);
      break;
    }
    case PAMI_XFER_GATHERV_INT:
    {
      coll[0].cmd.xfer_gatherv_int.stype       = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_gatherv_int.stypecount  = msg_size;
      coll[0].cmd.xfer_gatherv_int.rtype       = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_gatherv.root = root_ep;

      size_t k = 0;
      for (k = 0; k < ntasks; k++)
      {
        coll[0].cmd.xfer_gatherv_int.rtypecounts[k] = msg_size;
        coll[0].cmd.xfer_gatherv_int.rdispls[k]  = k * msg_size;
      }

      *result = check_metadata(*coll_metadata,
		               coll[0],
		               coll[0].cmd.xfer_gatherv_int.stype,
			       coll[0].cmd.xfer_gatherv_int.stypecount,
	                       coll[0].cmd.xfer_gatherv_int.sndbuf,
			       coll[0].cmd.xfer_gatherv_int.rtype,
			       coll[0].cmd.xfer_gatherv_int.stypecount,
	                       coll[0].cmd.xfer_gatherv_int.rcvbuf);
      break;
    }
    case PAMI_XFER_BARRIER:
    {
      break;
    }
    case PAMI_XFER_ALLTOALL:
    {
      coll[0].cmd.xfer_alltoall.stype = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_alltoall.rtype = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_alltoall.stypecount = msg_size;
      coll[0].cmd.xfer_alltoall.rtypecount = msg_size;
      *result = check_metadata(*coll_metadata,
		               coll[0],
		               coll[0].cmd.xfer_alltoall.stype,
			       coll[0].cmd.xfer_alltoall.stypecount,
	                       coll[0].cmd.xfer_alltoall.sndbuf,
			       coll[0].cmd.xfer_alltoall.rtype,
			       coll[0].cmd.xfer_alltoall.rtypecount,
	                       coll[0].cmd.xfer_alltoall.rcvbuf);
      break;
    }
    case PAMI_XFER_ALLTOALLV:
    {
      coll[0].cmd.xfer_alltoallv.stype         = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_alltoallv.rtype         = PAMI_TYPE_BYTE;
	  
      size_t j;
      for (j = 0; j < ntasks; j++)
      {
        coll[0].cmd.xfer_alltoallv.stypecounts[j] = 
          coll[0].cmd.xfer_alltoallv.rtypecounts[j] = msg_size;
        coll[0].cmd.xfer_alltoallv.sdispls[j] = 
          coll[0].cmd.xfer_alltoallv.rdispls[j] = msg_size * j;
      }

      *result = check_metadata(*coll_metadata,
		               coll[0],
		               coll[0].cmd.xfer_alltoallv.stype,
			       msg_size,
	                       coll[0].cmd.xfer_alltoallv.sndbuf,
			       coll[0].cmd.xfer_alltoallv.rtype,
			       msg_size,
	                       coll[0].cmd.xfer_alltoallv.rcvbuf);
      break;
    }
    case PAMI_XFER_ALLTOALLV_INT:
    {
      coll[0].cmd.xfer_alltoallv_int.stype     = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_alltoallv_int.rtype     = PAMI_TYPE_BYTE;

      size_t j;
      for (j = 0; j < ntasks; j++)
      {
        coll[0].cmd.xfer_alltoallv_int.stypecounts[j] = 
          coll[0].cmd.xfer_alltoallv_int.rtypecounts[j] = msg_size;
        coll[0].cmd.xfer_alltoallv_int.sdispls[j] = 
          coll[0].cmd.xfer_alltoallv_int.rdispls[j] = msg_size * j;
      }

      *result = check_metadata(*coll_metadata,
		               coll[0],
		               coll[0].cmd.xfer_alltoallv_int.stype,
 			       msg_size,
	                       coll[0].cmd.xfer_alltoallv_int.sndbuf,
			       coll[0].cmd.xfer_alltoallv_int.rtype,
			       msg_size,
	                       coll[0].cmd.xfer_alltoallv_int.rcvbuf);
      break;
    }
    case  PAMI_XFER_SCAN:
    {
      coll[0].cmd.xfer_scan.stype = PAMI_TYPE_SIGNED_CHAR;
      coll[0].cmd.xfer_scan.rtype = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_scan.stypecount = msg_size;
      coll[0].cmd.xfer_scan.rtypecount = msg_size;
      coll[0].cmd.xfer_scan.op = PAMI_DATA_SUM;
      coll[0].cmd.xfer_scan.exclusive = 0;
      //*result = coll_metadata->check_fn(&coll[0]);
      //result->check.nonlocal = 0;
      *result = check_metadata(*coll_metadata,
		               coll[0],
		               coll[0].cmd.xfer_scan.stype,
			       coll[0].cmd.xfer_scan.stypecount,
	                       coll[0].cmd.xfer_scan.sndbuf,
			       coll[0].cmd.xfer_scan.rtype,
			       coll[0].cmd.xfer_scan.rtypecount,
	                       coll[0].cmd.xfer_scan.rcvbuf);
      break;
    }
    case PAMI_XFER_REDUCE_SCATTER:
    {
      size_t ind;
      size_t * rcounts = coll[0].cmd.xfer_reduce_scatter.rcounts;
      for(ind = 0; ind<ntasks; ++ind) rcounts[ind] = msg_size;
      coll[0].cmd.xfer_reduce_scatter.stype = PAMI_TYPE_SIGNED_CHAR;
      coll[0].cmd.xfer_reduce_scatter.rtype = PAMI_TYPE_SIGNED_CHAR;
      coll[0].cmd.xfer_reduce_scatter.stypecount = ntasks*msg_size;
      coll[0].cmd.xfer_reduce_scatter.op = PAMI_DATA_SUM;
      *result = check_metadata(*coll_metadata,
		               coll[0],
		               coll[0].cmd.xfer_reduce_scatter.stype,
			       coll[0].cmd.xfer_reduce_scatter.stypecount,
	                       coll[0].cmd.xfer_reduce_scatter.sndbuf,
			       coll[0].cmd.xfer_reduce_scatter.rtype,
			       ntasks*msg_size,
	                       coll[0].cmd.xfer_reduce_scatter.rcvbuf);
      break;
    }
    case PAMI_XFER_AMBROADCAST:
    {
      am_total_count = 0;

      validation_t *v = (validation_t*)coll[0].cookie;
      v->root   = task;
      v->cookie = (void *)&am_total_count;
      coll[0].cb_done = cb_ambcast_done;
      coll[0].cmd.xfer_ambroadcast.headerlen    = sizeof(user_header_t);
      coll[0].cmd.xfer_ambroadcast.stype        = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_ambroadcast.stypecount   = 0;

      _gRc = PAMI_SUCCESS;
      pami_collective_hint_t h = {0};
      pami_dispatch_callback_function fn;
      fn.ambroadcast = cb_ambcast_recv;
      PAMI_AMCollective_dispatch_set(context,
                                     coll[0].algorithm,
                                     0,/* Set the dispatch id, can be any arbitrary value */
                                     fn,
                                     (void*) &am_total_count,
                                     h);
      coll[0].cmd.xfer_ambroadcast.dispatch = 0;
      if(task == root_zero)
      {
        coll[0].cmd.xfer_ambroadcast.stypecount = msg_size;
        bcast_initialize_sndbuf(coll[0].cmd.xfer_ambroadcast.sndbuf,
                              coll[0].cmd.xfer_ambroadcast.stypecount,
                              task);
      }
      else
        memset(coll[0].cmd.xfer_ambroadcast.sndbuf, 0xFF, msg_size);
      break;
    }
    case PAMI_XFER_AMSCATTER:
    {
      am_total_count = 0;

      coll[0].cmd.xfer_amscatter.headerlen    = sizeof(user_header_t);
      coll[0].cmd.xfer_amscatter.sndbuf       = _g_send_buffer;
      coll[0].cmd.xfer_amscatter.stype        = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_amscatter.stypecount   = 0;

      _gRc = PAMI_SUCCESS;
      pami_collective_hint_t h = {0};
      pami_dispatch_callback_function fn;
      fn.amscatter = cb_amscatter_recv;
      PAMI_AMCollective_dispatch_set(context,
                                     coll[0].algorithm,
                                     root_zero,/* Set the dispatch id, can be any arbitrary value */
                                     fn,
                                     (void*) &am_total_count,
                                     h);
      coll[0].cmd.xfer_amscatter.dispatch = root_zero;
      if(task == root_zero)
      {
        coll[0].cmd.xfer_amscatter.stypecount = msg_size;
        scatter_initialize_sndbuf(coll[0].cmd.xfer_amscatter.sndbuf,
                                coll[0].cmd.xfer_amscatter.stypecount,
                                ntasks);
      }
      else
        memset(_g_recv_buffer,
           0xFF,
           msg_size);
      break;
    }
    case PAMI_XFER_AMGATHER:
    {
      am_total_count = 0;

      coll[0].cmd.xfer_amgather.headerlen    = sizeof(user_header_t);
      coll[0].cmd.xfer_amgather.rcvbuf       = _g_recv_buffer;
      coll[0].cmd.xfer_amgather.rtype        = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_amgather.rtypecount   = 0;

      num_tasks      = ntasks;
      gather_initialize_sndbuf(my_task_id, _g_send_buffer, msg_size);
      _gRc = PAMI_SUCCESS;
      pami_collective_hint_t h = {0};
      pami_dispatch_callback_function fn;
      fn.amgather = cb_amgather_send;
      PAMI_AMCollective_dispatch_set(context,
                                     coll[0].algorithm,
                                     root_zero,/* Set the dispatch id, can be any arbitrary value */
                                     fn,
                                     (void*) &am_total_count,
                                     h);
      coll[0].cmd.xfer_amgather.dispatch = root_zero;
      if(task == root_zero)
        coll[0].cmd.xfer_amgather.rtypecount = msg_size;
      break;
    }
    case PAMI_XFER_AMREDUCE:
    {
      am_total_count = 0;

      coll[0].cmd.xfer_amreduce.headerlen    = sizeof(reduce_user_header_t);
      coll[0].cmd.xfer_amreduce.rcvbuf       = _g_recv_buffer;
      coll[0].cmd.xfer_amreduce.rtype        = PAMI_TYPE_BYTE;
      coll[0].cmd.xfer_amreduce.rtypecount   = 0;

      num_tasks      = ntasks;
      pami_collective_hint_t h = {0};
      pami_dispatch_callback_function fn;
      fn.amreduce = cb_amreduce_send;
      PAMI_AMCollective_dispatch_set(context,
                                     coll[0].algorithm,
                                     root_zero,/* Set the dispatch id, can be any arbitrary value */
                                     fn,
                                     (void*) &am_total_count,
                                     h);
      coll[0].cmd.xfer_amreduce.dispatch   = root_zero;
      coll[0].cmd.xfer_amreduce.rtypecount = msg_size;
      reduce_user_header_t* ruser_header = (reduce_user_header_t*)coll[0].cmd.xfer_amreduce.user_header;
      ruser_header->op = 4;/*Corresponding to PAMI_DATA_SUM in the array*/
      ruser_header->dt = 3;/*Corresponding to PAMI_TYPE_UNSIGNED_CHAR in the array*/
      reduce_initialize_sndbuf(_g_send_buffer,
                             msg_size,
                             PAMI_DATA_SUM,
                             PAMI_TYPE_UNSIGNED_CHAR,
                             my_task_id,
                             num_tasks);

      break;
    }
    default:
    {
      printf("Wrong pami_xfer_type %d\n", coll_xfer);
      break;
    }
  }//end switch
}

void release_coll(pami_xfer_t *coll,
	       pami_xfer_type_t coll_xfer)
{
  switch(coll_xfer)
  {
    case PAMI_XFER_BROADCAST:
    {
      free(coll[0].cmd.xfer_broadcast.buf);
      break;
    }
    case PAMI_XFER_ALLREDUCE:
    {
      free(coll[0].cmd.xfer_allreduce.sndbuf);
      free(coll[0].cmd.xfer_allreduce.rcvbuf);
      break;
    }
    case PAMI_XFER_REDUCE:
    {
      free(coll[0].cmd.xfer_reduce.sndbuf);
      free(coll[0].cmd.xfer_reduce.rcvbuf);
      break;
    }
    case PAMI_XFER_ALLGATHER:
    {
      free(coll[0].cmd.xfer_allgather.sndbuf);
      free(coll[0].cmd.xfer_allgather.rcvbuf);
      break;
    }
    case PAMI_XFER_ALLGATHERV:
    {
      free(coll[0].cmd.xfer_allgatherv.sndbuf);
      free(coll[0].cmd.xfer_allgatherv.rcvbuf);
      free(coll[0].cmd.xfer_allgatherv.rtypecounts);
      free(coll[0].cmd.xfer_allgatherv.rdispls);
      break;
    }
    case PAMI_XFER_ALLGATHERV_INT:
    {
      free(coll[0].cmd.xfer_allgatherv_int.sndbuf);
      free(coll[0].cmd.xfer_allgatherv_int.rcvbuf);
      free(coll[0].cmd.xfer_allgatherv_int.rtypecounts);
      free(coll[0].cmd.xfer_allgatherv_int.rdispls);
      break;
    }
    case PAMI_XFER_SCATTER:
    {
      free(coll[0].cmd.xfer_scatter.sndbuf);
      free(coll[0].cmd.xfer_scatter.rcvbuf);
      break;
    }
    case PAMI_XFER_SCATTERV:
    {
      free(coll[0].cmd.xfer_scatterv.sndbuf);
      free(coll[0].cmd.xfer_scatterv.stypecounts);
      free(coll[0].cmd.xfer_scatterv.sdispls);
      free(coll[0].cmd.xfer_scatterv.rcvbuf);
      break;
    }
    case PAMI_XFER_SCATTERV_INT:
    {
      free(coll[0].cmd.xfer_scatterv_int.sndbuf);
      free(coll[0].cmd.xfer_scatterv_int.stypecounts);
      free(coll[0].cmd.xfer_scatterv_int.sdispls);
      free(coll[0].cmd.xfer_scatterv_int.rcvbuf);
      break;
    }
    case PAMI_XFER_GATHER:
    {
      free(coll[0].cmd.xfer_gather.sndbuf);
      free(coll[0].cmd.xfer_gather.rcvbuf);
      break;
    }
    case PAMI_XFER_GATHERV:
    {
      free(coll[0].cmd.xfer_gatherv.sndbuf);
      free(coll[0].cmd.xfer_gatherv.rcvbuf);
      free(coll[0].cmd.xfer_gatherv.rtypecounts);
      free(coll[0].cmd.xfer_gatherv.rdispls);
      break;
    }
    case PAMI_XFER_GATHERV_INT:
    {
      free(coll[0].cmd.xfer_gatherv_int.sndbuf);
      free(coll[0].cmd.xfer_gatherv_int.rcvbuf);
      free(coll[0].cmd.xfer_gatherv_int.rtypecounts);
      free(coll[0].cmd.xfer_gatherv_int.rdispls);
      break;
    }
    case PAMI_XFER_BARRIER:
    {
      break;
    }
    case PAMI_XFER_ALLTOALL:
    {
      free(coll[0].cmd.xfer_alltoall.sndbuf);
      free(coll[0].cmd.xfer_alltoall.rcvbuf);
      break;
    }
    case PAMI_XFER_ALLTOALLV:
    {
      free(coll[0].cmd.xfer_alltoallv.sndbuf);
      free(coll[0].cmd.xfer_alltoallv.stypecounts);
      free(coll[0].cmd.xfer_alltoallv.sdispls);
      free(coll[0].cmd.xfer_alltoallv.rcvbuf);
      free(coll[0].cmd.xfer_alltoallv.rtypecounts);
      free(coll[0].cmd.xfer_alltoallv.rdispls);
      break;
    }
    case PAMI_XFER_ALLTOALLV_INT:
    {
      free(coll[0].cmd.xfer_alltoallv_int.sndbuf);
      free(coll[0].cmd.xfer_alltoallv_int.stypecounts);
      free(coll[0].cmd.xfer_alltoallv_int.sdispls);
      free(coll[0].cmd.xfer_alltoallv_int.rcvbuf);
      free(coll[0].cmd.xfer_alltoallv_int.rtypecounts);
      free(coll[0].cmd.xfer_alltoallv_int.rdispls);
      break;
    }
    case PAMI_XFER_SCAN:
    {
      free(coll[0].cmd.xfer_scan.sndbuf);
      free(coll[0].cmd.xfer_scan.rcvbuf);
      break;
    }
    case PAMI_XFER_REDUCE_SCATTER:
    {
      free(coll[0].cmd.xfer_reduce_scatter.sndbuf);
      free(coll[0].cmd.xfer_reduce_scatter.rcvbuf);
      free(coll[0].cmd.xfer_reduce_scatter.rcounts);
      break;
    }
    case PAMI_XFER_AMBROADCAST:
    {
      free(coll[0].cmd.xfer_ambroadcast.sndbuf);
      free(coll[0].cmd.xfer_ambroadcast.user_header);
      free(coll[0].cookie);
      free(_g_val_buffer);
      break;
    }
    case PAMI_XFER_AMSCATTER:
    {
      free(_g_val_buffer);
      free(_g_recv_buffer);
      free(coll[0].cmd.xfer_amscatter.sndbuf);
      free(coll[0].cmd.xfer_amscatter.headers);
      break;
    }
    case PAMI_XFER_AMGATHER:
    {
      free(_g_val_buffer);
      free(_g_send_buffer);
      free(coll[0].cmd.xfer_amgather.rcvbuf);
      free(coll[0].cmd.xfer_amgather.headers);
      break;
    }
    case PAMI_XFER_AMREDUCE:
    {
      free(_g_val_buffer);
      free(_g_send_buffer);
      free(coll[0].cmd.xfer_amreduce.rcvbuf);
      free(coll[0].cmd.xfer_amreduce.user_header);
      break;
    }
    case PAMI_XFER_COUNT:
    {
      printf("wrong pami_xfer_number %d\n", PAMI_XFER_COUNT);
      break;
    }
    case PAMI_XFER_TYPE_EXT:
    {
      printf("wrong pami_xfer_number %d\n", PAMI_XFER_TYPE_EXT);
      break;
    }
  }//end switch
}

int blocking_coll (pami_context_t      context,
                   pami_xfer_t        *coll,
                   volatile unsigned  *active)
{

  pami_result_t result;

  (*active)++;
  result = PAMI_Collective(context, coll);
  if (result != PAMI_SUCCESS)
    {
      fprintf (stderr, "Error. Unable to issue  collective. result = %d\n", result);
      return 1;
    }
  while (*active)
    result = PAMI_Context_advance (context, 1);
  return 0;
}

void alltoall_initialize_bufs(char *sbuf,
                              char *rbuf,
                              size_t l,
                              size_t r)
{
  size_t k;
  size_t d = l * r;

  for (k = 0; k < l; k++)
  {
    sbuf[ d + k ] = ((r + k) & 0xff);
    rbuf[ d + k ] = 0xff;
  }
}

void alltoall_initialize_bufs_dt(char *sbuf,
                                 char *rbuf,
                                 size_t l,
                                 size_t r,
                                 pami_type_t dt)
{
}

void allgather_initialize_sndbuf(void *sbuf,
                                 int bytes,
                                 int ep_id)
{
  unsigned char c = 0xFF & ep_id;
  memset(sbuf,c,bytes);
}

void scatter_initialize_sndbuf(void *sbuf,
                               int bytes,
                               size_t ntasks)
{
  size_t i;
  unsigned char *cbuf = (unsigned char *)  sbuf;

  for (i = 0; i < ntasks; i++)
  {
    unsigned char c = 0xFF & i;
    memset(cbuf + (i*bytes), c, bytes);
  }
}

void gather_initialize_sndbuf(int ep_id,
                              void *buf,
                              int bytes)
{
  unsigned char *cbuf = (unsigned char *)  buf;
  unsigned char c = 0x00 + ep_id;
  int i = bytes;

  for (; i; i--)
  {
    cbuf[i-1] = c++;
  }
}

void gather_initialize_sndbuf_dt(void *sbuf,
                                 int count,
                                 int taskid,
                                 pami_type_t dt)
{
}
void reduce_initialize_sndbuf(void *buf,
                              size_t count,
                              pami_data_function op,
                              pami_type_t dt,
                              size_t task_id,
                              size_t num_tasks)
{
}

void bcast_initialize_sndbuf(void *sbuf,
                             int bytes,
                             int root)
{
  unsigned char c = root;
  int i = bytes;
  unsigned char *cbuf = (unsigned char *)  sbuf;

  for (; i; i--)
  {
    cbuf[i-1] = (c++);
  }
}

void bcast_initialize_sndbuf_dt(void *sbuf,
                                int count,
                                int root,
                                pami_type_t dt)
{
}

void scatter_initialize_sndbuf_dt(void *sbuf,
                                  size_t counts,
                                  size_t ntasks,
                                  pami_type_t dt)
{
}

void scan_initialize_sndbuf(void *buf,
                            int count,
                            pami_data_function op,
                            pami_type_t dt,
                            int task_id)
{
  int i;
  /* if (op == PAMI_SUM && dt == PAMI_UNSIGNED_INT) { */
  if (op == PAMI_DATA_SUM && dt == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *ibuf = (unsigned int *)  buf;
    for (i = 0; i < count; i++)
    {
      ibuf[i] = i;
    }
  }
  else
  {
    size_t sz=get_type_size(dt);
    memset(buf,  task_id,  count * sz);
  }
}

void reduce_scatter_initialize_sndbuf(void *buf, int count,
                                      pami_data_function op,
                                      pami_type_t dt,
                                      int task_id)
{
}


int alltoall_check_rcvbuf(char *rbuf,
                          size_t l,
                          size_t nranks,
                          size_t myrank)
{
  size_t r, k;

  for (r = 0; r < nranks; r++)
  {
    size_t d = l * r;
    for (k = 0; k < l; k++)
    {
      if (rbuf[ d + k ] != (char)((myrank + k) & 0xff))
      {
        printf("%zu: (E) rbuf[%zu]:%02x instead of %02zx (r:%zu)\n",
               myrank,
               d + k,
               rbuf[ d + k ],
               ((r + k) & 0xff),
               r );
        return 1;
      }
    }
  }
  return 0;
}

int alltoall_check_rcvbuf_dt(char * rbuf,
                             size_t l,
                             size_t nranks,
                             size_t myrank,
                             pami_type_t dt)
{
  return 0;
}

int allgather_check_rcvbuf(void *rbuf,
                           size_t bytes,
                           size_t num_ep)
{
  size_t i,j;
  unsigned char *cbuf = (unsigned char *)  rbuf;

  for (j=0; j<num_ep; j++)
  {
    unsigned char c = 0xFF & j;
    for (i=j*bytes; i<(j+1)*bytes; i++)
      if (cbuf[i] != c)
      {
        fprintf(stderr, "Check(%zu) failed <%p>rbuf[%zu]=%.2u != %.2u \n",
                bytes, cbuf, i, cbuf[i], c);
        return 1;
      }
  }
  return 0;
}

int scatter_check_rcvbuf(void *rbuf,
                         int bytes,
                         pami_task_t task)
{
  int i;
  unsigned char *cbuf = (unsigned char *)  rbuf;

  unsigned char c = 0xFF & task;

  for (i = 0; i < bytes; i++)
    if (cbuf[i] != c)
    {
      fprintf(stderr, "Check(%d) failed <%p>rbuf[%d]=%.2u != %.2u \n", bytes, cbuf, i, cbuf[i], c);
      return 1;
    }

  return 0;
}

int gather_check_rcvbuf(size_t num_ep,
                        void *buf,
                        int bytes)
{
  size_t j;
  for (j = 0; j < num_ep; j++)
  {
    unsigned char *cbuf = (unsigned char *)  buf + j *bytes;
    unsigned char c = 0x00 + j;
    int i = bytes;
    for (; i; i--)
    {
      if (cbuf[i-1] != c)
      {
        fprintf(stderr, "Check(%d) failed <%p> buf[%d]=%.2u != %.2u \n",
                bytes,buf, i-1, cbuf[i-1], c);
        return -1;
      }

      c++;
    }
  }
  return 0;
}

int gather_check_rcvbuf_dt(size_t num_tasks,
                           void *buf,
                           int counts,
                           pami_type_t dt)
{
  return 0;
}

int reduce_check_rcvbuf(void *buf,
                        int count,
                        pami_data_function op,
                        pami_type_t dt,
                        int task_id,
                        int num_tasks)
{
  return 0;
}

int bcast_check_rcvbuf(void *rbuf,
                       int bytes,
                       int root)
{
  unsigned char c = root;
  int i = bytes;
  unsigned char *cbuf = (unsigned char *)  rbuf;

  for (; i; i--)
  {
    if (cbuf[i-1] != c)
    {
      fprintf(stderr, "Check(%d) failed <%p>rbuf[%d]=%.2u != %.2u \n", bytes, rbuf, i - 1, cbuf[i-1], c);
      return 1;
    }

    c++;
  }
  return 0;
}

int bcast_check_rcvbuf_dt(void *rbuf,
                          int count,
                          int root,
                          pami_type_t dt)
{
  return 0;
}

int scatter_check_rcvbuf_dt(void *rbuf,
                            int counts,
                            pami_task_t task,
                            pami_type_t dt)
{
  return 0;
}

int scan_check_rcvbuf(void *buf,
                      int count,
                      pami_data_function op,
                      pami_type_t dt,
                      int num_tasks,
                      int task_id,
                      int exclusive)
{
  int i, err = 0;
  /*  if (op == PAMI_SUM && dt == PAMI_UNSIGNED_INT) { */
  if (op == PAMI_DATA_SUM && dt == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *rbuf = (unsigned int *)  buf;
    unsigned int x;

    if(exclusive == 1)
      {
        /* Receive buffer is not valid on task 0 for exclusive scan */
        if(task_id == 0)
          return 0;
        else
          x = task_id;
      }
    else
      {
        x = task_id +1;
      }
    for (i = 0; i < count; i++)
    {
      if (rbuf[i] != i * x)
      {
        fprintf(stderr,"Check(%d) failed rbuf[%d] %u != %u\n",count,i,rbuf[i],i*x);
        err = -1;
        return err;    
      }
    }
  }
  return err;
}

int reduce_scatter_check_rcvbuf(void *buf,
                                size_t count,
                                pami_data_function op,
                                pami_type_t dt,
                                size_t num_tasks,
                                size_t task_id)
{
  return 0;
}


int check_results(pami_collective_t cmd, bench_setup *bench)
{

  if (bench->xfer == PAMI_XFER_BROADCAST)
  {
    return bcast_check_rcvbuf(cmd.xfer_broadcast.buf,
                              cmd.xfer_broadcast.typecount,
                              cmd.xfer_broadcast.root);

  }
  else if (bench->xfer == PAMI_XFER_ALLREDUCE)
  {
    return reduce_check_rcvbuf(cmd.xfer_allreduce.rcvbuf,
                               cmd.xfer_allreduce.stypecount,
                               cmd.xfer_allreduce.op,
                               cmd.xfer_allreduce.stype,
                               bench->task_id,
                               bench->np);
  }
  else if (bench->xfer == PAMI_XFER_REDUCE)
  {
    return reduce_check_rcvbuf(cmd.xfer_reduce.rcvbuf,
                               cmd.xfer_reduce.stypecount,
                               cmd.xfer_reduce.op,
                               cmd.xfer_reduce.stype,
                               bench->task_id,
                               bench->np);
  }
  else if (bench->xfer == PAMI_XFER_ALLGATHER)
  {
    return allgather_check_rcvbuf(cmd.xfer_allgather.rcvbuf,
                                  cmd.xfer_allgather.stypecount,
                                  bench->np);
  }
  else if (bench->xfer == PAMI_XFER_ALLGATHERV)
  {
    return allgather_check_rcvbuf(cmd.xfer_allgatherv.rcvbuf,
                                  cmd.xfer_allgatherv.stypecount,
                                  bench->np);
  }
  else if (bench->xfer == PAMI_XFER_ALLGATHERV_INT)
  {
    return allgather_check_rcvbuf(cmd.xfer_allgatherv_int.rcvbuf,
                                  cmd.xfer_allgatherv_int.stypecount,
                                  bench->np);
  }
  else if (bench->xfer == PAMI_XFER_SCATTER)
  {
    return scatter_check_rcvbuf(cmd.xfer_scatter.rcvbuf,
                                cmd.xfer_scatter.stypecount,
                                bench->np);
  }
  else if (bench->xfer == PAMI_XFER_GATHER)
  {
    return gather_check_rcvbuf(bench->np,
                               cmd.xfer_gather.rcvbuf,
                               cmd.xfer_gather.stypecount);
  }
  else if (bench->xfer == PAMI_XFER_ALLTOALL)
  {
    return alltoall_check_rcvbuf(cmd.xfer_alltoall.rcvbuf,
                                 cmd.xfer_alltoall.stypecount,
                                 bench->np,
                                 bench->task_id);

  }
  else if (bench->xfer == PAMI_XFER_SCAN)
  {
    int scan_type = 0; //inclusive
    return scan_check_rcvbuf(cmd.xfer_scan.rcvbuf,
                             cmd.xfer_scan.stypecount,
                             cmd.xfer_scan.op,
                             cmd.xfer_scan.stype,
                             bench->np,
                             bench->task_id,
                             scan_type);
  }
  else if (bench->xfer == PAMI_XFER_REDUCE_SCATTER)
  {
    return  reduce_scatter_check_rcvbuf(cmd.xfer_reduce_scatter.rcvbuf,
                                        cmd.xfer_reduce_scatter.stypecount,
                                        cmd.xfer_reduce_scatter.op,
                                        cmd.xfer_reduce_scatter.stype,
                                        bench->np,
                                        bench->task_id);
  }
  else if (bench->xfer == PAMI_XFER_AMBROADCAST)
    return _gRc;
  else if (bench->xfer == PAMI_XFER_AMSCATTER)
    return _gRc;
  else if (bench->xfer == PAMI_XFER_AMGATHER)
    return _gRc;

  return PAMI_SUCCESS;
}

void init_buffs(pami_collective_t cmd, bench_setup *bench)
{
  if (bench->xfer == PAMI_XFER_BROADCAST)
  {
    if (bench->task_id == cmd.xfer_broadcast.root)
      bcast_initialize_sndbuf(cmd.xfer_broadcast.buf,
                              cmd.xfer_broadcast.typecount,
                              cmd.xfer_broadcast.root);
    else
      memset(cmd.xfer_broadcast.buf, 0xFF, cmd.xfer_broadcast.typecount);
  }
  else if (bench->xfer == PAMI_XFER_ALLREDUCE)
  {
    reduce_initialize_sndbuf(cmd.xfer_allreduce.sndbuf,
                             cmd.xfer_allreduce.stypecount,
                             cmd.xfer_allreduce.op,
                             cmd.xfer_allreduce.stype,
                             bench->task_id,
                             bench->np);
  }
  else if (bench->xfer == PAMI_XFER_REDUCE)
  {
    reduce_initialize_sndbuf(cmd.xfer_reduce.sndbuf,
                             cmd.xfer_reduce.stypecount,
                             cmd.xfer_reduce.op,
                             cmd.xfer_reduce.stype,
                             bench->task_id,
                             bench->np);
    memset(cmd.xfer_reduce.rcvbuf,
           0xFF,
           cmd.xfer_reduce.stypecount * get_type_size(cmd.xfer_reduce.stype));
  }
  else if (bench->xfer == PAMI_XFER_ALLGATHER)
  {
    allgather_initialize_sndbuf(cmd.xfer_allgather.sndbuf,
                                cmd.xfer_allgather.stypecount,
                                bench->task_id);
    memset(cmd.xfer_allgather.rcvbuf,
           0xFF,
           cmd.xfer_allgather.stypecount);
  }
  else if (bench->xfer == PAMI_XFER_ALLGATHERV)
  {
    allgather_initialize_sndbuf(cmd.xfer_allgatherv.sndbuf,
                                cmd.xfer_allgatherv.stypecount,
                                bench->task_id);
    memset(cmd.xfer_allgatherv.rcvbuf,
           0xFF,
           cmd.xfer_allgatherv.stypecount);
  }
  else if (bench->xfer == PAMI_XFER_ALLGATHERV_INT)
  {
    allgather_initialize_sndbuf(cmd.xfer_allgatherv_int.sndbuf,
                                cmd.xfer_allgatherv_int.stypecount,
                                bench->task_id);
    memset(cmd.xfer_allgatherv_int.rcvbuf,
           0xFF,
           cmd.xfer_allgatherv_int.stypecount);
  }
  else if (bench->xfer == PAMI_XFER_SCATTER)
  {
    if (bench->task_id == cmd.xfer_scatter.root)
    {
      scatter_initialize_sndbuf(cmd.xfer_scatter.sndbuf,
                                cmd.xfer_scatter.stypecount,
                                bench->np);
    }
    memset(cmd.xfer_scatter.rcvbuf,
           0xFF,
           cmd.xfer_scatter.stypecount);
  }
  else if (bench->xfer == PAMI_XFER_GATHER)
  {
    if (bench->task_id == cmd.xfer_gather.root)
    {
      gather_initialize_sndbuf(bench->task_id,
                               cmd.xfer_gather.sndbuf,
                               cmd.xfer_gather.stypecount);
    }
    memset(cmd.xfer_gather.rcvbuf,
           0xFF,
           bench->np * cmd.xfer_scatter.stypecount);

  }
  else if (bench->xfer == PAMI_XFER_ALLTOALL)
  {
    int i;
    for (i = 0; i < bench->np; i++)
    {
      alltoall_initialize_bufs(cmd.xfer_alltoall.sndbuf,
                               cmd.xfer_alltoall.rcvbuf,
                               cmd.xfer_alltoall.stypecount,
                               i);
    }

  }
  else if (bench->xfer == PAMI_XFER_SCAN)
  {
    scan_initialize_sndbuf(cmd.xfer_scan.sndbuf,
                           cmd.xfer_scan.stypecount,
                           cmd.xfer_scan.op,
                           cmd.xfer_scan.stype,
                           bench->task_id);

    memset(cmd.xfer_scan.rcvbuf,
           0xFF,
           get_type_size(cmd.xfer_scan.stype) * cmd.xfer_scan.stypecount);
  }
  else if (bench->xfer == PAMI_XFER_REDUCE_SCATTER)
  {
    reduce_scatter_initialize_sndbuf(cmd.xfer_reduce_scatter.sndbuf,
                                     cmd.xfer_reduce_scatter.stypecount,
                                     cmd.xfer_reduce_scatter.op,
                                     cmd.xfer_reduce_scatter.stype,
                                     bench->task_id);
  }

}

void init_cutoff_tables()
{
  cutoff[PAMI_XFER_BROADCAST]= 65536;
  cutoff[PAMI_XFER_ALLREDUCE]= 65536;
  cutoff[PAMI_XFER_REDUCE]= 65536;
  cutoff[PAMI_XFER_ALLGATHER]= 65536;
  cutoff[PAMI_XFER_ALLGATHERV]= 65536;
  cutoff[PAMI_XFER_ALLGATHERV_INT]= 65536;
  cutoff[PAMI_XFER_SCATTER]= 65536;
  cutoff[PAMI_XFER_SCATTERV]= 65536;
  cutoff[PAMI_XFER_SCATTERV_INT]= 65536;
  cutoff[PAMI_XFER_GATHER]= 65536;
  cutoff[PAMI_XFER_GATHERV]= 65536;
  cutoff[PAMI_XFER_GATHERV_INT]= 65536;
  cutoff[PAMI_XFER_BARRIER]= 65536;
  cutoff[PAMI_XFER_ALLTOALL]= 65536;
  cutoff[PAMI_XFER_ALLTOALLV]= 65536;
  cutoff[PAMI_XFER_ALLTOALLV_INT]= 65536;
  cutoff[PAMI_XFER_SCAN]= 65536;
  cutoff[PAMI_XFER_REDUCE_SCATTER]= 65536;
  cutoff[PAMI_XFER_AMBROADCAST]= 65536;
  cutoff[PAMI_XFER_AMSCATTER]= 65536;
  cutoff[PAMI_XFER_AMGATHER]= 65536;
  cutoff[PAMI_XFER_AMREDUCE]= 65536;
}


void measure_collective(pami_context_t   context,
                        pami_xfer_t      *colls,
                        bench_setup      *bench)
{
  // colls[0] is collective to benchmark
  // colls[1] is barrier to synch nodes
  // colls[2] is reduce for timing
  volatile unsigned  poll_flag[3] = {0};
  volatile unsigned *nAMCollective =  NULL;

  double t0, total = 0;
  int i, iters;
  int rc_check;

  pami_xfer_type_t xfer_type = bench->xfer;
  
  /* To Be removed once AMReduce is fixed */
/*  if(xfer_type == PAMI_XFER_AMREDUCE){
    bench->times[0] = bench->times[1] = bench->times[2] = 0.0;
	return;
  }
*/  
  
  iters = bench->iters; 
  if (bench->bytes >= cutoff[bench->xfer])
  {
    iters = MIN(640, iters);
    iters /= (bench->bytes/cutoff[bench->xfer]);
  }

  if(xfer_type < PAMI_XFER_AMBROADCAST)
  {
    colls[0].cookie    = (void*) &poll_flag[0];
  }
  colls[1].cb_done   = cb_done;
  colls[1].cookie    = (void*) &poll_flag[1];
 
  if (bench->data_check)
    init_buffs(colls[0].cmd, bench);

  pami_result_t result;
  if(xfer_type >= PAMI_XFER_AMBROADCAST && xfer_type <= PAMI_XFER_AMREDUCE)
    nAMCollective = &am_total_count;
  for (i = 0; i < MIN(iters, CACHE_WARMUP_ITERS); i++)
  {
    if(xfer_type < PAMI_XFER_AMBROADCAST)
    {
      blocking_coll(context, &colls[0], &poll_flag[0]);
    }
    else
    {
      if(bench->isRoot)
      {
        result = PAMI_Collective(context, &colls[0]);
        if (result != PAMI_SUCCESS)
        {
          fprintf (stderr, "Error. Unable to issue collective. result = %d\n", result);
          return;
        }
      }
      while((int)*nAMCollective <= i)
        result = PAMI_Context_advance(context, 1);
    }
  }

  blocking_coll(context, &colls[1], &poll_flag[1]);
  if(xfer_type >= PAMI_XFER_AMBROADCAST && xfer_type <= PAMI_XFER_AMREDUCE)
    *nAMCollective = 0;
  if(xfer_type < PAMI_XFER_AMBROADCAST)/* Have the if outside time loop for accuracy */
  {
    t0 = timer();
    for (i = 0; i < iters; i++)
    {
      blocking_coll(context, &colls[0], &poll_flag[0]);
    }
    total = timer() - t0;
  }
  else
  {
    t0 = timer();
    for (i = 0; i < iters; i++)
    {
      if(bench->isRoot)
      {
        result = PAMI_Collective(context, &colls[0]);
        if (result != PAMI_SUCCESS)
        {
          fprintf (stderr, "Error. Unable to issue collective. result = %d\n", result);
          return;
        }
      }
      while((int)*nAMCollective <= i)
        result = PAMI_Context_advance(context, 1);
    }
    total = timer() - t0;
    assert(*nAMCollective == (unsigned)iters);
  }
  blocking_coll(context, &colls[1], &poll_flag[1]);

  if (bench->data_check)
  {
   bench->passed_data_check = 1;
   rc_check = check_results(colls[0].cmd, bench);
    if (rc_check)
    {
     bench->passed_data_check = 0;
      if (bench->verbose)
        fprintf(stderr, "collective failed data check\n");
    }
    else
    {
     if (bench->verbose)
        fprintf(stderr, "collective passed data check\n");
    }
  }
  total /= (double) iters;

  colls[2].cb_done                    = cb_done;
  colls[2].cookie                     = (void*) &poll_flag[2];
  colls[2].cmd.xfer_reduce.stype      = PAMI_TYPE_DOUBLE;
  colls[2].cmd.xfer_reduce.rtype      = PAMI_TYPE_DOUBLE;
  colls[2].cmd.xfer_reduce.stypecount = 1;
  colls[2].cmd.xfer_reduce.rtypecount = 1;

  colls[2].cmd.xfer_reduce.sndbuf    = (char *) (&total);

  colls[2].cmd.xfer_reduce.rcvbuf    = (char *) (&(bench->times[0]));
  colls[2].cmd.xfer_reduce.op = PAMI_DATA_MIN;
  blocking_coll(context, &colls[2], &poll_flag[2]);

  colls[2].cmd.xfer_reduce.rcvbuf    = (char *) (&(bench->times[1]));
  colls[2].cmd.xfer_reduce.op = PAMI_DATA_MAX;
  blocking_coll(context, &colls[2], &poll_flag[2]);

  colls[2].cmd.xfer_reduce.rcvbuf    = (char *) (&(bench->times[2]));
  colls[2].cmd.xfer_reduce.op = PAMI_DATA_SUM;
  blocking_coll(context, &colls[2], &poll_flag[2]);

  bench->times[2] /= (double) bench->np;
}

size_t ilog2(register size_t x)
{
  register unsigned int l=0;
  if(x >= 1<<16) { x>>=16; l|=16; }
  if(x >= 1<<8) { x>>=8; l|=8; }
  if(x >= 1<<4) { x>>=4; l|=4; }
  if(x >= 1<<2) { x>>=2; l|=2; }
  if(x >= 1<<1) l|=1;
  return l;
}

size_t ipow2(register size_t x)
{
  size_t rt = 1;
  if (!x) return 1;
  while(x){ rt*=2; x--;}
  return rt;
}


}

#endif // __api_extension_c_collsel_Benchmark_h__
