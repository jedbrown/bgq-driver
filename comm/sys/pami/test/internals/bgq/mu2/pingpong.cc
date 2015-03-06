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
 * \file test/internals/bgq/mu2/pingpong.cc
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
#include "components/devices/bgq/mu2/model/DmaModel.h"
#include "components/devices/bgq/mu2/model/DmaModelMemoryFifoCompletion.h"
#include "components/devices/bgq/mu2/model/PacketModel.h"
//#include "components/devices/bgq/mu2/model/PacketModelMemoryFifoCompletion.h"
#include "components/devices/bgq/mu2/model/PacketModelDeposit.h"
#include "components/devices/bgq/mu2/model/PacketModelInterrupt.h"

#include "p2p/protocols/send/eager/Eager.h"

typedef PAMI::Device::MU::Context MuContext;

extern void *__mu_context_cache;

typedef PAMI::Device::MU::PacketModel MuPacketModel;
//typedef PAMI::Device::MU::PacketModelMemoryFifoCompletion MuPacketModel;
//typedef PAMI::Device::MU::PacketModelDeposit MuPacketModel;
//typedef PAMI::Device::MU::PacketModelInterrupt MuPacketModel;

//typedef PAMI::Device::MU::DmaModel MuDmaModel;
typedef PAMI::Device::MU::DmaModelMemoryFifoCompletion MuDmaModel;

typedef PAMI::Protocol::Send::Eager<MuPacketModel>::EagerImpl<PAMI::Protocol::Send::DEFAULT, false> MuEager;

#define MAX_ITER 1000
#define WARMUP   10
int npackets = 0;

void done_fn       (pami_context_t   context,
                    void           * cookie,
                    pami_result_t    result)
{
  //fprintf (stderr, "done_fn() %zu -> %zu\n", *((size_t *)cookie), *((size_t *)cookie) - 1);
  (*((size_t *)cookie))--;
}

int dispatch_fn    (void   * metadata,
                    void   * payload,
                    size_t   bytes,
                    void   * recv_func_parm,
                    void   * cookie)
{
  //fprintf(stderr, "Received packet: recv_func_parm = %zu (%p), MAX_ITER = %d, npackets = %d -> %d\n", (size_t) recv_func_parm, recv_func_parm, MAX_ITER, npackets, npackets+1);
  npackets ++;
  return 0;
}

void recv (
  pami_context_t       context,      /**< IN:  communication context which invoked the dispatch function */
  void               * cookie,       /**< IN:  dispatch cookie */
  const void         * header_addr,  /**< IN:  header address  */
  size_t               header_size,  /**< IN:  header size     */
  const void         * pipe_addr,    /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
  size_t               data_size,    /**< IN:  number of byts of message data, valid regardless of message type */
  pami_endpoint_t      origin,
  pami_recv_t        * recv)        /**< OUT: receive message structure, only needed if addr is non-NULL */
{
  npackets ++;
}

#define MAX_BUF_SIZE  1024
#define MSG_SIZE      1

// PAMI::Global __myGlobal;
// PAMI::ResourceManager __pamiRM;
// PAMI::Device::MU::Global __MUGlobal ( __pamiRM, __myGlobal.mapping, __myGlobal.personality );

template <typename T_Model, typename T_Protocol>
void test (MuContext & mu0, T_Model & model, T_Protocol & protocol, const char * label = "")
{
  char metadata[4];
  char buf[MAX_BUF_SIZE];

  memset (metadata, 0, sizeof(metadata));
  memset (buf, 0, sizeof(buf));

  struct iovec iov[1];
  iov[0].iov_base = buf;
  iov[0].iov_len  = MSG_SIZE;

  volatile size_t active = 0;
  npackets = 0;

  unsigned long start = 0, end = 0;

  Personality_t pers;
  Kernel_GetPersonality(&pers, sizeof(pers));

  int neighbor = __global.mapping.task();
  if (__global.mapping.size() > 0) {
    if (__global.mapping.task() == 0)
      neighbor = 1;
    else if (__global.mapping.task() == 1)
      neighbor = 0;
    else return; //This is a 2 process test
  }

  // -------------------------------------------------------------------
  // Test immediate packet model
  // -------------------------------------------------------------------
  for (int i = 0; i < MAX_ITER + WARMUP; i++)
    {
      if (i == WARMUP)
        start = GetTimeBase();

      model.postPacket (neighbor,
                        0,
                        (void *)metadata,
                        4,
                        iov);

      while (npackets == 0) {
	mu0.advance();
      }
      npackets = 0;
    }

  end = GetTimeBase();

  printf ("[%s] immediate pingpong time = %d cycles\n", label, (int)((end - start) / MAX_ITER));

  // -------------------------------------------------------------------
  // Test non-blocking packet model
  // -------------------------------------------------------------------
  typedef uint8_t state_t[MuPacketModel::packet_model_state_bytes];
  state_t state[MAX_ITER];
  npackets = 0;
  start = 0;
  end = 0;

  for (int i = 0; i < MAX_ITER+WARMUP; i++)
    {
      if (i == WARMUP)
        start = GetTimeBase();

      model.postPacket (state[i],
                        done_fn,
                        (void *)&active,
                        neighbor, //__global.mapping.task(),
                        0,
                        (void *)metadata,
                        4,
                        iov);
      while (npackets == 0) {
	mu0.advance();
      }
      npackets = 0;
    }

  end = GetTimeBase();

  printf ("[%s] nonblocking pingpong time = %d cycles\n", label, (int)((end - start) / MAX_ITER));

  // -------------------------------------------------------------------
  // Test non-blocking send protocol
  // -------------------------------------------------------------------
#if 0
  npackets = 0;
  start = 0;
  end = 0;

  for (int i = 0; i <= MAX_ITER; i++)
    {
      if (i == 1)
        start = GetTimeBase();

      protocol.simple ();
      pkt.postPacket (state[i],
                      done_fn,
                      (void *)&active,
                      __global.mapping.task(),
                      0,
                      (void *)metadata,
                      4,
                      iov);
    }

  while (npackets != MAX_ITER+1) mu.advance();

  end = GetTimeBase();

  printf ("[%s] nonblocking pingpong time = %d cycles\n", label, (int)((end - start) / MAX_ITER));
#endif
}

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

  // Initialize the MU resources for all contexts for this client
  //__MUGlobal.getMuRM().initializeContexts( 0 /*id_client*/, 2 /*id_count*/, NULL /* generic::Devices */ );

  //MuContext mu0 (__global.mapping, 0, 0, 2);

  MuContext &mu0 = *(MuContext *)(__mu_context_cache); 

  fprintf (stderr, "After mu init\n");

//  uint8_t model00_buf[sizeof(PAMI::Device::MU::PacketModelMemoryFifoCompletion)] __attribute__((__aligned__(32)));
//  PAMI::Device::MU::PacketModelMemoryFifoCompletion &model00 = *(new (model00_buf) PAMI::Device::MU::PacketModelMemoryFifoCompletion(mu0));

//  uint8_t model01_buf[sizeof(PAMI::Device::MU::PacketModelMemoryFifoCompletion)] __attribute__((__aligned__(32)));
//  PAMI::Device::MU::PacketModelMemoryFifoCompletion &model01 = *(new (model01_buf) PAMI::Device::MU::PacketModelMemoryFifoCompletion(mu1));

  uint8_t model10_buf[sizeof(PAMI::Device::MU::PacketModel)] __attribute__((__aligned__(32)));
  PAMI::Device::MU::PacketModel &model10 = *(new (model10_buf) PAMI::Device::MU::PacketModel(mu0));

  fprintf (stderr, "After model constructors\n");

  //pami_result_t result;
  //MuDmaModel dma (mu, result);

//  model00.init (100, dispatch_fn, (void *) 100, NULL, NULL);
//  model01.init (100, dispatch_fn, (void *) 101, NULL, NULL);

  bool rc  = 0 ;
  rc = model10.init (0, dispatch_fn, (void *) 0);
  PAMI_assert (rc == PAMI_SUCCESS);

  fprintf (stderr, "After model init\n");

  pami_result_t result;
  MuEager eager (0,      // dispatch set id
                 recv,    //dispatch function
                 NULL,    // dispatch cookie
                 mu0,      // "packet" device reference
                 mu0,      // "packet" device reference
                 (pami_endpoint_t) 0,       // origin endpoint
                 (pami_context_t) NULL,
                 (pami_dispatch_hint_t){0}, // hints
                 result);

  fprintf (stderr, "After eager constructor\n");


  test (mu0, model10, eager, "completion array");

  return 0;
}
