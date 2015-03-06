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
 * \file test/internals/bgq/mu2/multicast_2dev.cc
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
#include "components/devices/bgq/mu2/model/MulticastDmaModel.h"
#include "components/devices/bgq/mu2/msg/InjectDPutMulticast.h"

#include "components/devices/bgq/mu2/model/Multisync.h"
#include "components/devices/bgq/mu2/model/Multicombine.h"

#include "components/devices/shmem/ShmemDevice.h"
#include "components/devices/shmem/ShmemCollDesc.h"
#include "components/devices/shmem/ShmemMcstModelWorld.h"
#include "components/atomic/bgq/L2Counter.h"

#include "common/bgq/NativeInterface.h"

typedef PAMI::Device::Shmem::ShmemCollDesc <PAMI::Counter::Native> ShmemCollDesc;
//typedef PAMI::Device::Shmem::ShmemCollDesc <PAMI::Counter::BGQ::L2NodeCounter> ShmemCollDesc;
typedef PAMI::Device::ShmemDevice <ShmemCollDesc> ShmemCollDevice;
typedef PAMI::Device::Shmem::ShmemMcstModelWorld <ShmemCollDevice, ShmemCollDesc> ShmemMcstModel;


typedef PAMI::Device::MU::Context MuContext;

extern void  *__mu_context_cache;

#define MAX_ITER 1000
#define WARMUP  10

void done_fn       (pami_context_t   context,
                    void           * cookie,
                    pami_result_t    result)
{
  fprintf (stderr, "done_fn() %zu -> %zu\n", *((size_t *)cookie), *((size_t *)cookie) - 1);
  (*((size_t *)cookie))--;
}

#define MAX_BUF_SIZE  1024*1024
#define CHUNK_SIZE    1024
#define MSG_SIZE      1

char bcastbuf [MAX_BUF_SIZE] __attribute__((__aligned__(128)));

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
  
  Delay (1000);

  pami_configuration_t configuration;
  configuration.name = PAMI_CLIENT_TASK_ID;
  pami_result_t result = PAMI_Client_query(client, &configuration,1);
  unsigned myrank = configuration.value.intval;

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration,1);
  //  unsigned num_tasks = configuration.value.intval;

  MuContext &mu0 = *(MuContext *)(__mu_context_cache); 

  fprintf (stderr, "After mu init\n");

  /*uint8_t model_buf[sizeof(PAMI::Device::MU::MulticastDmaModel)] __attribute__((__aligned__(32)));
  pami_result_t status;
  uint8_t buf [PAMI::Device::MU::MulticastDmaModel::sizeof_msg];
  PAMI::Device::MU::MulticastDmaModel &model = *(new (model_buf) PAMI::Device::MU::MulticastDmaModel(mu0, status));
	*/
  fprintf (stderr, "After model constructors\n");

  PAMI::Device::Generic::Device* _generics;
  ShmemCollDevice* _shmem;

  fprintf (stderr, "before generating the generic device\n");
  _generics = PAMI::Device::Generic::Device::Factory::generate(0, 1, __global.mm, NULL);
  fprintf (stderr, "after generating the generic device\n");
  _shmem = ShmemCollDevice::Factory::generate(0, 1, __global.mm, _generics);
  fprintf (stderr, "after generating the shmem device\n");

  fprintf (stderr, "After generating the devices\n");
  PAMI::Device::Generic::Device::Factory::init(_generics, 0, 0, NULL, (pami_context_t)1, &__global.mm, _generics);
  fprintf (stderr, "After initing the generic device\n");
  ShmemCollDevice::Factory::init(_shmem, 0, 0, NULL, (pami_context_t)1, &__global.mm, _generics);
  fprintf (stderr, "After initing the shmem device\n");

  Delay (1000);

/*  PAMI::BGQNativeInterface<PAMI::Device::MU::MUCollDevice,
  PAMI::Device::MU::MUMulticastModel,
  PAMI::Device::MU::MUMultisyncModel,
  PAMI::Device::MU::MUMulticombineModel>  nativeInterface(mu, client, context, 0, 0);
*/

  PAMI::BGQNativeInterfaceASMultiDevice<MuContext,
                          ShmemCollDevice,
                          PAMI::Device::MU::MulticastDmaModel,
                          ShmemMcstModel,
		 				  PAMI::Device::MU::MultisyncModel<false, false>,
                          PAMI::Device::MU::MulticombineModel<PAMI::Device::MU::AllreducePacketModel, false, false> > 
						  nativeInterface(mu0, *_shmem, client, context, 0, 0,0);

 fprintf(stderr,"after multidevice native interface construction\n");
  //Delay (1000);


  size_t done_count = 1;
  pami_multicast_t     mcast;
  PAMI::PipeWorkQueue  srcp;
  PAMI::Topology       srct;
  PAMI::Topology       dstt;

  memset (&srcp, 0, sizeof(srcp));
  new (&srcp) PAMI::PipeWorkQueue();
  srcp.configure(bcastbuf, MAX_BUF_SIZE, 0);
	
  /*if (myrank == 0)
  srcp.configure(bcastbuf, MAX_BUF_SIZE, MAX_BUF_SIZE);
  else
  srcp.configure(bcastbuf, MAX_BUF_SIZE, 0);
	*/

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

  srcp.reset();
  printf("bytes available to consume from srcp :%zu\n", srcp.bytesAvailableToConsume());


  //int bytes = CHUNK_SIZE;

  if (myrank == 0) {
    mcast.src_participants = (pami_topology_t *)&srct;
    
    unsigned char istorus[7] = {0,0,0,0,0,0,0};
    pami_coord_t ll, ur;
    memset(ll.u.n_torus.coords, 0, sizeof(ll.u.n_torus.coords));
    memset(ur.u.n_torus.coords, 0, sizeof(ur.u.n_torus.coords));
    
    ur.u.n_torus.coords[0] = 1;
    //PAMI_Task2Network (1, &ur);

    printf ("Target coords %ld %ld %ld %ld %ld %ld\n", 
	    ur.u.n_torus.coords[0],
	    ur.u.n_torus.coords[1],
	    ur.u.n_torus.coords[2],
	    ur.u.n_torus.coords[3],
	    ur.u.n_torus.coords[4],
	    ur.u.n_torus.coords[5]);

    memset (&dstt, 0, sizeof(dstt));
    new (&dstt) PAMI::Topology (&ll, &ur, &ll, istorus); //Root is the ll node too
    
    mcast.dst_participants = (pami_topology_t *)&dstt;
    //model.postMulticast (buf, &mcast, NULL);
    Delay (5000);
  	nativeInterface.multicast(&mcast, NULL);	
    Delay (5000);
  }
  else  //receivers
    //model.postMulticast (buf, &mcast, NULL);
  	nativeInterface.multicast(&mcast, NULL);	

/*
  PAMI::Topology       itopo;
  PAMI::Topology       otopo;

  size_t root = __global.topology_local.index2Rank(0);
  fprintf(stderr, "Number of local tasks = %zu root: %zu \n", __global.topology_local.size(), root);

  new (&itopo) PAMI::Topology(root);
  __global.topology_local.subtractTopology(&otopo, &itopo);
	 
   mcast.src_participants = (pami_topology_t *)&itopo;
   mcast.dst_participants = (pami_topology_t *)&otopo;

  nativeInterface.multicast(&mcast, NULL);	
*/


  while (done_count) {
    //if (myrank == 0 && (srcp.getBytesProduced() < MAX_BUF_SIZE)) {
    if (myrank == 0) {
      srcp.produceBytes (MAX_BUF_SIZE);
    }
    
    PAMI_Context_advancev(&context, 1, 1);
	PAMI::Device::Generic::Device::Factory::advance(_generics, 0, 0);
    ShmemCollDevice::Factory::advance(_shmem, 0, 0);

  }
  
  Delay (1000);
  while(1){};

  for (int i = 0; i < MAX_BUF_SIZE; i++)
    PAMI_assert (bcastbuf[i] == (i&0xff));
#if 0
  for (int i = 0; i < MAX_ITER; i++) {
    done_count = 1;
    srcp.reset();
    if (myrank == 0) {
      model.postMulticast (buf, &mcast, NULL);
      Delay (1000);
    }
    else  
      model.postMulticast (buf, &mcast, NULL);
    
    while (done_count) {
      if (myrank == 0 && (srcp.getBytesProduced() < MAX_BUF_SIZE)) {
	srcp.produceBytes (bytes);
      }
      
      PAMI_Context_advancev(&context, 1, 1);
    }
  }
#endif  
  //test (mu0, model, eager, "completion array");
  return 0;
}
