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
 * \file test/internals/bgq/mu2/mrate.cc
 * \brief ???
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "Global.h"
#include <pami.h>

#include "../../../api/coll_util.h"

//#define ITERATIONS 10
//#define ITERATIONS 1000
//#define ITERATIONS 100
#define ITERATIONS 1000

#define WARMUP

#ifndef BUFSIZE
//#define BUFSIZE 2048
//#define BUFSIZE 1024*256
#define BUFSIZE 256
//#define BUFSIZE 64
//#define BUFSIZE 2
//#define BUFSIZE 1024
#endif

#ifndef MAX_BUFSIZE
#define MAX_BUFSIZE 2048
#endif


#undef TRACE_ERR
#ifndef TRACE_ERR
#define TRACE_ERR(x)  //fprintf x
#endif

volatile int      _recv_active = 0;
char              _recv_buffer[MAX_BUFSIZE] __attribute__ ((__aligned__(16)));

size_t         _dispatch[100];
unsigned       _dispatch_count;

size_t _my_rank;

pami_xfer_t          barrier;
volatile unsigned    poll_flag = 0;

typedef struct
{
  size_t sndlen;
} header_t;

/* --------------------------------------------------------------- */
static void test_dispatch (
    pami_context_t        context,      /**< IN: PAMI context */
    void               * cookie,       /**< IN: dispatch cookie */
    const void         * header_addr,  /**< IN: header address */
    size_t               header_size,  /**< IN: header size */
    const void         * pipe_addr,    /**< IN: address of PAMI pipe buffer */
    size_t               pipe_size,    /**< IN: size of PAMI pipe buffer */
    pami_endpoint_t origin,
pami_recv_t         * recv)        /**< OUT: receive message structure */
{
  unsigned * value = (unsigned *) cookie;
  TRACE_ERR((stderr, "(%zu) short recv:  decrement cookie = %p, %d => %d\n", _my_rank, cookie, *value, *value-1));
  --*value;
}

#define WINDOW_SIZE 64

void send (pami_context_t context, pami_send_immediate_t * parameters)
{
  TRACE_ERR((stderr, "(%zu) before send_immediate()  \n", _my_rank));
  //pami_result_t result =
  for (int i = 0; i < WINDOW_SIZE; i ++)
    PAMI_Send_immediate (context, parameters);
  TRACE_ERR((stderr, "(%zu) after send_immediate()  \n", _my_rank));
}

void recv (pami_context_t context)
{
  TRACE_ERR((stderr, "(%zu) recv_once() Before advance\n", _my_rank));
  while (_recv_active > 0) PAMI_Context_advance (context, 100);  
  TRACE_ERR((stderr, "(%zu) recv_once()  After advance\n", _my_rank));
}

unsigned long long test (pami_client_t client, pami_context_t context, size_t dispatch, size_t hdrlen, size_t sndlen, pami_task_t myrank, pami_endpoint_t target)
{
  TRACE_ERR((stderr, "(%u) Do test ... sndlen = %zu\n", myrank, sndlen));
  char metadata[MAX_BUFSIZE];
  char buffer[MAX_BUFSIZE];

  header_t header;
  header.sndlen = sndlen;

  pami_send_immediate_t parameters;
  parameters.dispatch = dispatch;
  parameters.header.iov_base = metadata;
  parameters.header.iov_len = hdrlen;
  parameters.data.iov_base  = buffer;
  parameters.data.iov_len = sndlen;

//  barrier ();

  unsigned i;
  unsigned long long t1 = PAMI_Wtimebase(client);

  parameters.dest = target;
  for (i = 0; i < ITERATIONS; i++)
    {
      //fprintf(stderr, "(%u) Starting Iteration %d of size %zu\n", myrank, i, sndlen);
      send (context, &parameters);
      _recv_active += WINDOW_SIZE;
      recv (context);
    }

  blocking_coll(context, &barrier, &poll_flag);
  unsigned long long t2 = PAMI_Wtimebase(client);
  
  return ((t2-t1)/ITERATIONS)/2;
}


int main (int argc, char ** argv)
{
  TRACE_ERR((stderr, "Start test ...\n"));
  //	printf("sizeof size_t:%d\n", sizeof(size_t));
  size_t hdrcnt = argc;
  size_t hdrsize[1024];
  hdrsize[0] = 0;

  size_t nranks = 0;
  int arg = 0;

  for (arg=1; arg<argc; arg++)
  {
    hdrsize[arg] = (size_t) strtol (argv[arg], NULL, 10);
  }

  pami_client_t client;
  char clientname[]="PAMI";
  TRACE_ERR((stderr, "... before PAMI_Client_create()\n"));
  PAMI_Client_create (clientname, &client, NULL, 0);
  TRACE_ERR((stderr, "...  after PAMI_Client_create()\n"));
  pami_context_t context;
  TRACE_ERR((stderr, "... before PAMI_Context_createv()\n"));
  { size_t _n = 1; PAMI_Context_createv (client, NULL, 0, &context, _n); }
  TRACE_ERR((stderr, "...  after PAMI_Context_createv()\n"));

//  TRACE_ERR((stderr, "... before barrier_init()\n"));
//  barrier_init (client, context, 0);
//  TRACE_ERR((stderr, "...  after barrier_init()\n"));

//  printf("size of size_t:%d\n", sizeof(size_t));

  /* Register the protocols to test */
  _dispatch_count = 0;

  _dispatch[_dispatch_count] = _dispatch_count + 1;

  pami_dispatch_callback_function fn;
  fn.p2p = test_dispatch;
  pami_dispatch_hint_t options={};
  TRACE_ERR((stderr, "Before PAMI_Dispatch_set() .. &_recv_active = %p, recv_active = %u\n", &_recv_active, _recv_active));
  pami_result_t result = PAMI_Dispatch_set (context,
					    _dispatch[_dispatch_count++],
					    fn,
					    (void *)&_recv_active,
					    options);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable register pami dispatch. result = %d\n", result);
    return 1;
  }

  pami_configuration_t configuration;

  configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query(client, &configuration,1);
  _my_rank = configuration.value.intval;

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration,1);
  nranks = configuration.value.intval;

  configuration.name = PAMI_CLIENT_WTICK;
  result = PAMI_Client_query(client, &configuration,1);
  double tick = configuration.value.doubleval;

  configuration.name = PAMI_DISPATCH_SEND_IMMEDIATE_MAX;
  result = PAMI_Dispatch_query(context, _dispatch[0], &configuration,1);
  size_t send_immediate_max = 16;
  if (result == PAMI_SUCCESS)
  {
    send_immediate_max = configuration.value.intval;
  }
  else
  {
    fprintf (stderr, "Warning. Unable to query PAMI_DISPATCH_SEND_IMMEDIATE_MAX, using test default value %zu. result = %d\n", send_immediate_max, result);
  }

  if (send_immediate_max > 16)
    send_immediate_max = 16;

  /* Display some test header information */
  if (_my_rank == 0)
  {
    char str[2][1024];
    int index[2];
    index[0] = 0;
    index[1] = 0;

    index[0] += sprintf (&str[0][index[0]], "#          ");
    index[1] += sprintf (&str[1][index[1]], "#    bytes ");

    fprintf (stdout, "# PAMI_Send_immediate() nearest-neighor half-pingpong blocking latency performance test\n");
    fprintf (stdout, "#\n");

    unsigned i;
    for (i=0; i<hdrcnt; i++)
    {
      if (i==0)
        fprintf (stdout, "# testcase %d : header bytes = %3zd\n", i, hdrsize[i]);
      else
        fprintf (stdout, "# testcase %d : header bytes = %3zd (argv[%d])\n", i, hdrsize[i], i);
      index[0] += sprintf (&str[0][index[0]], "[- testcase %d -] ", i);
      index[1] += sprintf (&str[1][index[1]], " cycles    usec  ");
    }

    fprintf (stdout, "#\n");
    fprintf (stdout, "%s\n", str[0]);
    fprintf (stdout, "%s\n", str[1]);
    fflush (stdout);
  }

//  barrier ();

  unsigned i = 0;
  for (i = 0; i < 10000000;i++){};

  unsigned long long cycles = 0;
  double usec;

  pami_geometry_t world_geometry;
  size_t               num_algorithm[2];
  pami_algorithm_t    *always_works_algo = NULL;
  pami_metadata_t     *always_works_md;
  pami_algorithm_t    *must_query_algo;
  pami_metadata_t     *must_query_md;
  pami_xfer_type_t     barrier_xfer = PAMI_XFER_BARRIER;

  query_geometry_world(client,
		       context,
		       &world_geometry,
		       barrier_xfer,
		       num_algorithm,
		       &always_works_algo,
		       &always_works_md,
		       &must_query_algo,
		       &must_query_md); 

  barrier.cb_done   = cb_done;
  barrier.cookie    = (void*) & poll_flag;
  barrier.algorithm = always_works_algo[0];

  int do_test = 0;
  pami_task_t neighbor = 0;
  pami_network network = PAMI_DEFAULT_NETWORK;
  uint tsize = __global.mapping.tSize();

  size_t dims[5] = {0};
  size_t n_active_dims = 0;

  if (__global.personality.aSize() > 1)
    dims[n_active_dims ++] = 0;
  
  if (__global.personality.bSize() > 1)  
    dims[n_active_dims ++] = 1;
  
  if (__global.personality.cSize() > 1) 
    dims[n_active_dims ++] = 2;
  
  if (__global.personality.dSize() > 1) 
    dims[n_active_dims ++] = 3;

  if(__global.personality.eSize() >  1)
    dims[n_active_dims ++] = 4;

  size_t ranks_per_node = (tsize / n_active_dims) + ((tsize % n_active_dims) > 0);  
  if (_my_rank == 0)
    printf ("Num Active Links %ld, ranks per node %ld\n", n_active_dims, ranks_per_node);

  if ( __global.mapping.a() == 0 &&
       __global.mapping.b() == 0 &&
       __global.mapping.c() == 0 &&
       __global.mapping.d() == 0 &&
       __global.mapping.e() == 0 ) 
    {
      int tcoord = __global.mapping.t();
      int neighbor_dim = tcoord / ranks_per_node; // ceil(32/5) = 7

      pami_coord_t  coord;
      memset (&coord, 0, sizeof(coord));
      coord.u.n_torus.coords[dims[neighbor_dim]] = 1;
      coord.u.n_torus.coords[5] = tcoord % ranks_per_node;

      __global.mapping.network2task (&coord, &neighbor, &network);
      do_test = 1;
    }
  else if ( (__global.mapping.a()  +
	     __global.mapping.b()  +
	     __global.mapping.c()  +
	     __global.mapping.d()  +
	     __global.mapping.e()) == 1 ) {

    pami_coord_t  coord;
    memset (&coord, 0, sizeof(coord));
    size_t mycoords[5];
    mycoords[0] = __global.mapping.a();
    mycoords[1] = __global.mapping.b();
    mycoords[2] = __global.mapping.c();
    mycoords[3] = __global.mapping.d();
    mycoords[4] = __global.mapping.e();

    size_t rn = 0;
    for (i = 0; i < n_active_dims; i++) {
      coord.u.n_torus.coords[5] += mycoords[dims[i]] * rn;
      rn += ranks_per_node;
    }
    coord.u.n_torus.coords[5] +=__global.mapping.t();  

    if (__global.mapping.t() < ranks_per_node && 
	coord.u.n_torus.coords[5] < tsize) {
      do_test = 1;
      __global.mapping.network2task (&coord, &neighbor, &network);      
    }
  }

  pami_endpoint_t target = PAMI_ENDPOINT_NULL;
  if (do_test) {
    int targetrank = neighbor;
    PAMI_Endpoint_create (client, targetrank, 0, &target);

    //printf ("do test %d, src %ld, dst %d\n", do_test, _my_rank, neighbor);
  }  

  char str[10240];
  //	fprintf(stdout,"starting the test\n") ;
  size_t sndlen;
  for (sndlen = 0; sndlen < send_immediate_max; sndlen = sndlen*3/2+1)
  //sndlen = 40;
  {
    //if (_my_rank == 0)
    int index = 0;
    index += sprintf (&str[index], "%10zd ", sndlen);    

    for (unsigned i=0; i<hdrcnt; i++)
    {
#ifdef WARMUP
      if (do_test)
	test (client, context, _dispatch[0], hdrsize[i], sndlen, _my_rank, target);
      else
	blocking_coll(context, &barrier, &poll_flag);
#endif
      if (do_test)
	cycles = test (client, context, _dispatch[0], hdrsize[i], sndlen, _my_rank, target);
      else
	blocking_coll(context, &barrier, &poll_flag);
      blocking_coll(context, &barrier, &poll_flag);
      cycles /= WINDOW_SIZE;
      usec   = cycles * tick * 1000000.0;
      //index += sprintf (&str[index], "%7lld %7.4f  ", cycles, usec);
      index += sprintf (&str[index], "%7lld  ", cycles);
    }

    if (_my_rank == 0) {
      fprintf (stdout, "%s\n", str);
      fprintf (stdout, "Message Rate %5.3g MMPS\n", (1600.0 * tsize / cycles));
    }
  }
  
  PAMI_Client_destroy(&client);

  return 0;
}
#undef TRACE_ERR
