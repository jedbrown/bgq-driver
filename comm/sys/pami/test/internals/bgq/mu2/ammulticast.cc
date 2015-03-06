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
 * \file test/internals/bgq/mu2/ammulticast.cc
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
#include "components/devices/bgq/mu2/model/AMMulticastModel.h"

typedef PAMI::Device::MU::Context MuContext;

extern void  *__mu_context_cache;

#define MAX_ITER 100

size_t done_count = 1;

void done_fn       (pami_context_t   context,
                    void           * cookie,
                    pami_result_t    result)
{
  //fprintf (stderr, "done_fn() %zu -> %zu\n", *((size_t *)cookie), *((size_t *)cookie) - 1);
  (*((size_t *)cookie))--;
}

PAMI::PipeWorkQueue  srcp;

void   dispatch_fn (pami_context_t        ctxt,
		    const pami_quad_t     *msginfo,
		    unsigned              msgcount,
		    unsigned              connection_id,
		    size_t                root,
		    size_t                sndlen,
		    void                 *clientdata,
		    size_t               *rcvlen,
		    pami_pipeworkqueue_t **rcvpwq,
		    pami_callback_t       *cb_done)  
{  
  //  fprintf (stderr, "In Packet Dispatch\n");

  srcp.reset();
  *rcvpwq = (pami_pipeworkqueue_t *) &srcp;
  cb_done->function   = done_fn;
  cb_done->clientdata = &done_count;
  *rcvlen = sndlen;
}


#define MAX_BUF_SIZE  64
#define CHUNK_SIZE    64
#define MAX_RANKS     1024

char bcastbuf [MAX_BUF_SIZE];
pami_task_t ranks[MAX_RANKS];

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

  uint8_t model_buf[sizeof(PAMI::Device::MU::ShortAMMulticastModel)] __attribute__((__aligned__(32)));
  pami_result_t status;
  uint8_t buf [PAMI::Device::MU::ShortAMMulticastModel::sizeof_msg];
  PAMI::Device::MU::ShortAMMulticastModel &model = *(new (model_buf) PAMI::Device::MU::ShortAMMulticastModel(client, context, mu0, status));
  PAMI_assert (status == PAMI_SUCCESS);

  model.registerMcastRecvFunction (0, dispatch_fn, NULL);
  fprintf (stderr, "After model constructors\n");

  Delay (10000);

  pami_multicast_t     mcast;
  PAMI::Topology       srct (__global.mapping.task());
  size_t nranks = __global.mapping.size();
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

  //  mcast.client = (size_t) client;
  // mcast.context = (size_t) context;
  mcast.bytes   = MAX_BUF_SIZE;
  mcast.connection_id = 0;
  mcast.cb_done.function = done_fn;
  mcast.cb_done.clientdata = &done_count;

  mcast.src = (pami_pipeworkqueue_t *)&srcp;
  mcast.dst = (pami_pipeworkqueue_t *)&srcp;

  int bytes = CHUNK_SIZE;
  if (myrank == 0) {
    mcast.src_participants = (pami_topology_t *)&srct;
    for (size_t i = 0; i < nranks-1; i++)
      ranks[i] = i+1;
    new (&dstt) PAMI::Topology (ranks, nranks-1); 
    
    mcast.dst_participants = (pami_topology_t *)&dstt;
    srcp.produceBytes (bytes);
    model.postMulticast (buf, 0, 0 , &mcast, NULL);
  }
  
  while (done_count) {    
    PAMI_Context_advancev(&context, 1, 1);
  }
  
  for (int i = 0; i < MAX_BUF_SIZE; i++)
    PAMI_assert (bcastbuf[i] == (i&0xff));

  for (int i = 0; i < MAX_ITER; i++) {
    done_count ++;
    if (myrank == 0) {
      srcp.reset();
      srcp.produceBytes (bytes);
      model.postMulticast (buf, 0, 0, &mcast, NULL);
    }
    
    while (done_count > 0) {
      PAMI_Context_advancev(&context, 1, 1);
    }
  }
  
  return 0;
}
