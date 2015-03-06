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
 * \file test/internals/bgq/mu2/collmulticast.cc
 * \brief Simple standalone MU software device test.
 */

#include "sys/pami.h"

#include "common/bgq/Global.h"

#include "common/bgq/BgqPersonality.h"
#include "common/bgq/Mapping.h"
#include "common/bgq/BgqMapCache.h"

#include "components/devices/bgq/mu2/global/Global.h"
#include "components/devices/bgq/mu2/global/ResourceManager.h"

#include "components/devices/bgq/mu2/Context.h"
#include "components/devices/bgq/mu2/model/CollectiveMulticastDmaModel.h"

typedef PAMI::Device::MU::Context MuContext;

extern void  *__mu_context_cache;

#define MAX_ITER 20
#define WARMUP  5

void done_fn       (pami_context_t   context,
                    void           * cookie,
                    pami_result_t    result)
{
  //fprintf (stderr, "done_fn() %zu -> %zu\n", *((size_t *)cookie), *((size_t *)cookie) - 1);
  (*((size_t *)cookie))--;
}

#define MAX_BUF_SIZE   1024*1024
#define CHUNK_SIZE     1024
#define SHORT_BUF_SIZE 1

char bcastbuf [MAX_BUF_SIZE];


int main(int argc, char ** argv)
{
  pami_client_t client;
  char clientname[]="PAMI";
  //TRACE_ERR((stderr, "... before PAMI_Client_create()\n"));
  PAMI_Client_create (clientname, &client, NULL, 0);
  //TRACE_ERR((stderr, "...  after PAMI_Client_create()\n"));
  pami_context_t context;
  //TRACE_ERR((stderr, "... before PAMI_Context_createv()\n"));
  { size_t _n = 1; PAMI_Context_createv (client, NULL, 0, &context, _n); }
  //TRACE_ERR((stderr, "...  after PAMI_Context_createv()\n"));
  
  pami_configuration_t configuration;
  configuration.name = PAMI_CLIENT_TASK_ID;
  pami_result_t result = PAMI_Client_query(client, &configuration,1);
  unsigned myrank = configuration.value.intval;

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration,1);
  //  unsigned num_tasks = configuration.value.intval;

  MuContext &mu0 = *(MuContext *)(__mu_context_cache); 

  fprintf (stderr, "After mu init\n");

  uint8_t model_buf[sizeof(PAMI::Device::MU::CollectiveMulticastDmaModel)] __attribute__((__aligned__(32)));
  pami_result_t status;
  PAMI::Device::MU::CollectiveMulticastDmaModel &model = *(new (model_buf) PAMI::Device::MU::CollectiveMulticastDmaModel(client, context, mu0, status));

  fprintf (stderr, "After model constructors\n");

  size_t done_count = 1;
  pami_multicast_t     mcast;
  PAMI::PipeWorkQueue  srcp;
  PAMI::Topology       srct;
  PAMI::Topology       dstt;

  memset (&srcp, 0, sizeof(srcp));
  new (&srcp) PAMI::PipeWorkQueue();
  srcp.configure(bcastbuf, MAX_BUF_SIZE, 0);

  if (myrank == 0) {
    for (int i = 0; i < MAX_BUF_SIZE; i++)
      bcastbuf[i] = i & 0xff;
  }
  else {
    for (int i = 0; i < MAX_BUF_SIZE; i++)
      bcastbuf[i] = 0xff;
  }

  memset(&mcast, 0, sizeof(mcast));

  //mcast.client = (size_t) client;
  //mcast.context = (size_t) context;
  mcast.bytes   = MAX_BUF_SIZE;
  mcast.connection_id = 0;
  mcast.cb_done.function = done_fn;
  mcast.cb_done.clientdata = &done_count;

  mcast.src = (pami_pipeworkqueue_t *)&srcp;
  mcast.dst = (pami_pipeworkqueue_t *)&srcp;

  new (&srct) PAMI::Topology ((pami_task_t)0);
  mcast.src_participants = (pami_topology_t *)&srct;

  int bytes = CHUNK_SIZE;
  if (myrank == 0) {    
    unsigned char istorus[7] = {0,0,0,0,0,0,0};
    pami_coord_t ll, ur;
    memset(ll.u.n_torus.coords, 0, sizeof(ll.u.n_torus.coords));
    memset(ur.u.n_torus.coords, 0, sizeof(ur.u.n_torus.coords));
    
    //ur.u.n_torus.coords[0] = 1;
    __global.mapping.task2network(1, &ur, PAMI_N_TORUS_NETWORK);

#if 0
    printf ("Target coords %ld %ld %ld %ld %ld %ld\n", 
	    ur.u.n_torus.coords[0],
	    ur.u.n_torus.coords[1],
	    ur.u.n_torus.coords[2],
	    ur.u.n_torus.coords[3],
	    ur.u.n_torus.coords[4],
	    ur.u.n_torus.coords[5]);
#endif

    memset (&dstt, 0, sizeof(dstt));
    new (&dstt) PAMI::Topology (&ll, &ur, &ll, istorus); //Root is the ll node too
    
    mcast.dst_participants = (pami_topology_t *)&dstt;

    srcp.produceBytes (MAX_BUF_SIZE);
    model.postMulticastImmediate (0, 0, &mcast,(void*)1);
  }
  else  {
    //fprintf (stderr, "Calling Post Multicast");
    model.postMulticastImmediate (0, 0, &mcast,(void*)1);
  }
  
  while (done_count) {
    //if (myrank == 0 && (srcp.getBytesProduced() < MAX_BUF_SIZE)) {
    //srcp.produceBytes (bytes);
    //}
    
    PAMI_Context_advancev(&context, 1, 1);
  }
  
  for (int i = 0; i < MAX_BUF_SIZE; i++)
    PAMI_assert (bcastbuf[i] == (i&0xff));

  //Test Pipelining
  for (int i = 0; i < MAX_ITER; i++) {
    done_count = 1;
    srcp.reset();
    model.postMulticastImmediate (0, 0, &mcast, (void*)1);
    
    while (done_count) {
      if (myrank == 0 && (srcp.getBytesProduced() < MAX_BUF_SIZE)) {
	srcp.produceBytes (bytes);
      }
      
      PAMI_Context_advancev(&context, 1, 1);
    }
  }
  
  uint64_t s_start = GetTimeBase();

  //Short Performance test
  mcast.bytes = SHORT_BUF_SIZE;
  for (int i = 0; i < MAX_ITER; i++) {
    done_count = 1;
    srcp.reset();
    if (myrank == 0)
      srcp.produceBytes (SHORT_BUF_SIZE);
    
    model.postMulticastImmediate (0, 0, &mcast, (void*)1);
    
    while (done_count) 
      PAMI_Context_advancev(&context, 1, 1);
  }

  uint64_t s_end = GetTimeBase();

  uint64_t l_start = GetTimeBase();
  
  //Large Performance test
  mcast.bytes = MAX_BUF_SIZE;
  for (int i = 0; i < MAX_ITER; i++) {
    done_count = 1;
    srcp.reset();
    if (myrank == 0)
      srcp.produceBytes (MAX_BUF_SIZE);
    
    model.postMulticastImmediate (0, 0, &mcast,(void*)1);
    
    while (done_count) 
      PAMI_Context_advancev(&context, 1, 1);
  }

  uint64_t l_end = GetTimeBase();

  printf ("Broadcast %d byte latency %ld cycles, %d byte latency %ld cycles", 
	  SHORT_BUF_SIZE, (s_end - s_start)/MAX_ITER, MAX_BUF_SIZE, (l_end - l_start)/MAX_ITER);

  return 0;
}
