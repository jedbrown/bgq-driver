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
 * \file test/internals/bgq/mu2/manytomany.cc
 * \brief ???
 */
#include "sys/pami.h"

#include "common/bgq/Global.h"

#include "common/bgq/BgqPersonality.h"
#include "common/bgq/Mapping.h"
#include "common/bgq/BgqMapCache.h"

#include "components/devices/bgq/mu2/global/Global.h"
#include "components/devices/bgq/mu2/global/ResourceManager.h"

#include "components/devices/bgq/mu2/Context.h"
#include "components/devices/bgq/mu2/model/ManytomanyModel.h"

typedef PAMI::Device::MU::Context MuContext;

extern void  *__mu_context_cache;

#define MAX_ITER 1000
#define WARMUP  10

pami_manytomanybuf_t   rbuf[2];
pami_callback_t        rcbdone;
PAMI::M2MPipeWorkQueueT<size_t, 0>  spwq;
PAMI::M2MPipeWorkQueueT<size_t, 0>  rpwq[2];

void dispatch_m2m (pami_context_t          ctxt,
		   void                  * arg,
		   unsigned                connid,
		   pami_quad_t           * msginfo,
		   unsigned                msgcount,
		   pami_manytomanybuf_t ** recv,
		   pami_callback_t       * cb_done)
{
  PAMI_assert (msgcount == 0);
  *recv    = &rbuf[connid];
  *cb_done = rcbdone;
  return;
}


void done_fn       (pami_context_t   context,
                    void           * cookie,
                    pami_result_t    result)
{
  //fprintf (stderr, "done_fn() %zu -> %zu\n", *((size_t *)cookie), *((size_t *)cookie) - 1);
  (*((size_t *)cookie))--;
}

#define MAX_BUF_SIZE  16384
#define MAX_RANKS     512

char        sm2mbuf  [MAX_RANKS * MAX_BUF_SIZE] __attribute__((__aligned__(64)));
char        rm2mbuf  [MAX_RANKS * MAX_BUF_SIZE] __attribute__((__aligned__(64)));
size_t      sizes    [MAX_RANKS];
size_t      indices  [MAX_RANKS];
size_t      sendinit [MAX_RANKS];
size_t      recvinit [2][MAX_RANKS];
pami_task_t ranks    [MAX_RANKS];

int main(int argc, char ** argv)
{
  pami_client_t client;

  size_t bytes = MAX_BUF_SIZE;
  if (argc > 1)
    bytes = (size_t) atoi(argv[1]);

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
  //unsigned myrank = configuration.value.intval;

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration,1);
  size_t num_tasks = configuration.value.intval;

  MuContext *mu0 = (MuContext *)(__mu_context_cache); 
  assert (mu0 != NULL);

  uint8_t model_buf[sizeof(PAMI::Device::MU::ManytomanyModel<size_t, 0>)] __attribute__((__aligned__(32)));
  pami_result_t status;
  uint8_t state [PAMI::Device::MU::ManytomanyModel<size_t, 0>::sizeof_msg];
  PAMI::Device::MU::ManytomanyModel<size_t, 0> &model = 
    *(new (model_buf) PAMI::Device::MU::ManytomanyModel<size_t, 0>(client, context, *mu0, status));
  model.registerManytomanyRecvFunction_impl (0, dispatch_m2m, NULL);

  fprintf (stderr, "After model constructors\n");

  size_t done_count = 2;
  pami_manytomany_t       m2m;
  PAMI::Topology          topo;

  size_t i = 0;
  for (i = 0; i < num_tasks; i++)
    ranks[i] = i;
  new (&topo) PAMI::Topology (ranks, num_tasks); 
  
  for (i = 0; i < MAX_BUF_SIZE * MAX_RANKS; i++) 
    sm2mbuf[i] = 0xff;
  
  for (i = 0; i < MAX_RANKS; i++)
    indices[i] = i * bytes;
  
  for (i = 0; i < MAX_RANKS; i++)
    sizes[i] = bytes;
  
  memset (&spwq, 0, sizeof(spwq));
  new (&spwq) PAMI::M2MPipeWorkQueueT<size_t, 0>();
  pami_type_t dgsp = PAMI_TYPE_BYTE;
  spwq.configure(sm2mbuf, num_tasks, &dgsp, indices, sizes, sendinit);

  memset (&rpwq, 0, sizeof(rpwq));
  new (&rpwq[0]) PAMI::M2MPipeWorkQueueT<size_t, 0>();
  rpwq[0].configure(rm2mbuf, num_tasks, &dgsp, indices, sizes, recvinit[0]);

  new (&rpwq[1]) PAMI::M2MPipeWorkQueueT<size_t, 0>();
  rpwq[1].configure(rm2mbuf, num_tasks, &dgsp, indices, sizes, recvinit[1]);

  rcbdone.function = done_fn;
  rcbdone.clientdata = &done_count;

  memset(&m2m, 0, sizeof(m2m));
  m2m.client              = (size_t) client;
  m2m.context             = (size_t) context;  
  m2m.connection_id       = 0;
  m2m.cb_done.function    = done_fn;
  m2m.cb_done.clientdata  = &done_count;
  m2m.send.buffer         = &spwq;
  m2m.send.type           = PAMI::M2M_VECTOR_LONG;
  m2m.send.participants   = &topo;

  rbuf[0].buffer = &rpwq[0];
  rbuf[0].participants = &topo;
  rbuf[0].type = PAMI::M2M_VECTOR_LONG;
  rbuf[1].buffer = &rpwq[1];
  rbuf[1].participants = &topo;
  rbuf[1].type = PAMI::M2M_VECTOR_LONG;

  Delay (100000);

  for (i = 0; i < num_tasks; ++i) {
    sendinit[i] = bytes;
    recvinit[0][i] = 0;
    recvinit[1][i] = 0;
  }  
  model.postManytomany (state, &m2m, NULL);
  
  while (done_count > 0) 
    PAMI_Context_advance(context, 100);
  
  //printf ("Finished 1st iteration\n");
  
  uint64_t start = GetTimeBase();

  for (int j = 0; j < MAX_ITER; j++) {
    done_count += 2;
    
    int cid = (j+1)&0x1;
    m2m.connection_id = cid;
    for (i = 0; i < num_tasks; ++i) {
      sendinit[i] = bytes;
      recvinit[j&0x1][i] = 0;
    }  
    model.postManytomany (state, &m2m, NULL);
    
    while (done_count > 0) 
      PAMI_Context_advance(context, 100);
  }

  uint64_t end = GetTimeBase();
  
  printf ("Alltoall time on %ld nodes with %ld bytes %ld cycles / iter\n", num_tasks, bytes, (end - start)/MAX_ITER);

  return 0;
}
