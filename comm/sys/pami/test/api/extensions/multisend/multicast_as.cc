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
 * \file test/api/extensions/multisend/multicast_as.cc
 * \brief Simple all-sided multicast tests. DEPRECATED
 */

#define DEBUG_RANK (unsigned)-1

#include "Buffer.h"

#include "Global.h"
#include <unistd.h>
#ifndef TEST_BUF_SIZE
  #define TEST_BUF_SIZE	32
#endif // TEST_BUF_SIZE

#warning DEPRECATED. Update your build

static PAMI::Test::Buffer<TEST_BUF_SIZE> _buffer;

static int           _doneCountdown;
pami_callback_t       _cb_done;
const pami_quad_t     _msginfo = {0,1,2,3};

void _done_cb(pami_context_t context, void *cookie, pami_result_t err)
{
  PAMI_assertf(_doneCountdown > 0,"doneCountdown %d\n",_doneCountdown);
  volatile int *doneCountdown = (volatile int*) cookie;
  DBG_FPRINTF((stderr, "%s:%s done %d/%d \n",__FILE__,__PRETTY_FUNCTION__, *doneCountdown,_doneCountdown));
  --*doneCountdown;
}

int main(int argc, char ** argv)
{
  pami_client_t client;
  pami_context_t context;
  pami_result_t status = PAMI_ERROR;

  status = PAMI_Client_create("multicast test", &client, NULL, 0);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to initialize pami client. result = %d\n", status);
    return 1;
  }
  DBG_FPRINTF((stderr,"Client %p\n",client));

  status = PAMI_Context_createv(client, NULL, 0, &context, 1);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to create pami context. result = %d\n", status);
    return 1;
  }

  DBG_FPRINTF((stderr,"Context %p\n",context));
  pami_configuration_t configuration;

  configuration.name = PAMI_CLIENT_TASK_ID;
  status = PAMI_Client_query(client, &configuration,1);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, status);
    return 1;
  }
  size_t task_id = configuration.value.intval;
  DBG_FPRINTF((stderr, "My task id = %zu\n", task_id));

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  status = PAMI_Client_query(client, &configuration,1);
  if(status != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, status);
    return 1;
  }
  size_t num_tasks = configuration.value.intval;
  if(task_id == 0) fprintf(stderr, "Number of tasks = %zu\n", num_tasks);

// END standard setup
// ------------------------------------------------------------------------
  _cb_done.function   = &_done_cb;
  _cb_done.clientdata = &_doneCountdown;

  size_t                     dispatch = 2;

  pami_dispatch_callback_function fn;

  fn.multicast = NULL; // all-sided, no recv callback

  pami_dispatch_hint_t        options;
  memset(&options, 0x00, sizeof(options));

  options.type = PAMI_MULTICAST;

  options.config = NULL;

  options.hint.multicast.global = 1;
  options.hint.multicast.all_sided = 1;
  options.hint.multicast.active_message = 0;

  status = PAMI_Dispatch_set_new(context,
                                dispatch,
                                fn,
                                NULL,
                                options);

  //For testing ease, I'm assuming rank list topology, so convert them
  PAMI::Topology topology_global = __global.topology_global;
  topology_global.convertTopology(PAMI_LIST_TOPOLOGY);

  PAMI::Topology topology_local  = __global.topology_local;
  topology_local.convertTopology(PAMI_LIST_TOPOLOGY);

  // global topology variables
  pami_task_t  gRoot    = topology_global.index2Rank(0);
  pami_task_t *gRankList=NULL; topology_global.rankList(&gRankList);
  size_t  gSize    = topology_global.size();

  PAMI::Topology src_participants;
  PAMI::Topology dst_participants;

  pami_multicast_t mcast;
  memset(&mcast, 0x00, sizeof(mcast));

  mcast.dispatch = dispatch;
  mcast.connection_id = 0xB;
  mcast.msginfo = &_msginfo;
  mcast.msgcount = 1;
  mcast.src_participants = (pami_topology_t *)&src_participants;
  mcast.dst_participants = (pami_topology_t *)&dst_participants;

  mcast.src = (pami_pipeworkqueue_t *)_buffer.srcPwq();
  mcast.dst = (pami_pipeworkqueue_t *)_buffer.dstPwq();

  mcast.client = 0;
  mcast.context = 0;
  mcast.roles = -1;
  mcast.bytes = TEST_BUF_SIZE;

  mcast.cb_done = _cb_done;

// ------------------------------------------------------------------------
// simple mcast to all except root
// ------------------------------------------------------------------------
  {
    _doneCountdown = 1;
    //sleep(5); // instead of syncing

    new (&src_participants) PAMI::Topology(gRoot); // global root
    new (&dst_participants) PAMI::Topology(gRankList+1, (gSize-1)); // everyone except root in dst_participants
    if(gRoot == task_id)
      _buffer.reset(true); // isRoot = true
    else
      _buffer.reset(false);  // isRoot = false

    status = PAMI_Multicast(&mcast);

    if(task_id == DEBUG_RANK)
    {
      DBG_FPRINTF((stderr,"sleep\n"));
      sleep(5);
    }
    DBG_FPRINTF((stderr,"Before advance\n"));

    while(_doneCountdown)
    {
      status = PAMI_Context_advance (context, 10);
    }

    size_t
    bytesConsumed = 0,
    bytesProduced = 0;

    if(gRoot == task_id)
    {
      _buffer.validate(bytesConsumed,
                       bytesProduced,
                       true,   // isRoot = true
                       false); // isDest = false
      if((bytesConsumed != TEST_BUF_SIZE) ||
         (bytesProduced != 0))
      {
        fprintf(stderr, "FAIL bytesConsumed = %zu, bytesProduced = %zu\n", bytesConsumed, bytesProduced);
      }
      else
        fprintf(stderr, "PASS bytesConsumed = %zu, bytesProduced = %zu\n", bytesConsumed, bytesProduced);
    }
    else
    {
      _buffer.validate(bytesConsumed,
                       bytesProduced);
      if((bytesConsumed != 0) ||
         (bytesProduced != TEST_BUF_SIZE))
      {
        fprintf(stderr, "FAIL bytesConsumed = %zu, bytesProduced = %zu\n", bytesConsumed, bytesProduced);
      }
      else
        fprintf(stderr, "PASS bytesConsumed = %zu, bytesProduced = %zu\n", bytesConsumed, bytesProduced);
    }
  }
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
// simple mcast to all except root with null parms
// ------------------------------------------------------------------------
  {
    _doneCountdown = 1;
    //sleep(5); // instead of syncing

    if(gRoot == task_id)
    {
      new (&src_participants) PAMI::Topology(gRoot); // global root
      mcast.src_participants = (pami_topology_t *)&src_participants;

      new (&dst_participants) PAMI::Topology(gRankList+1, (gSize-1)); // everyone except root in dst_participants
      mcast.dst_participants = (pami_topology_t *)&dst_participants;

      mcast.src = (pami_pipeworkqueue_t *)_buffer.srcPwq();
      mcast.dst = (pami_pipeworkqueue_t *)NULL;
      _buffer.reset(true); // isRoot = true
    }
    else
    {
      mcast.src_participants = (pami_topology_t *)NULL;
      new (&dst_participants) PAMI::Topology(task_id); // just me as far as I'm concerned or care
      mcast.dst_participants = (pami_topology_t *)&dst_participants;

      mcast.src = (pami_pipeworkqueue_t *)NULL;
      mcast.dst = (pami_pipeworkqueue_t *)_buffer.dstPwq();
      _buffer.reset(false);  // isRoot = false
    }

    status = PAMI_Multicast(&mcast);

    if(task_id == DEBUG_RANK)
    {
      DBG_FPRINTF((stderr,"sleep\n"));
      sleep(5);
    }
    DBG_FPRINTF((stderr,"Before advance\n"));

    while(_doneCountdown)
    {
      status = PAMI_Context_advance (context, 10);
    }

    size_t
    bytesConsumed = 0,
    bytesProduced = 0;

    if(gRoot == task_id)
    {
      _buffer.validate(bytesConsumed,
                       bytesProduced,
                       true,   // isRoot = true
                       false); // isDest = false
      if((bytesConsumed != TEST_BUF_SIZE) ||
         (bytesProduced != 0))
      {
        fprintf(stderr, "FAIL bytesConsumed = %zu, bytesProduced = %zu\n", bytesConsumed, bytesProduced);
      }
      else
        fprintf(stderr, "PASS bytesConsumed = %zu, bytesProduced = %zu\n", bytesConsumed, bytesProduced);
    }
    else
    {
      _buffer.validate(bytesConsumed,
                       bytesProduced);
      if((bytesConsumed != 0) ||
         (bytesProduced != TEST_BUF_SIZE))
      {
        fprintf(stderr, "FAIL bytesConsumed = %zu, bytesProduced = %zu\n", bytesConsumed, bytesProduced);
      }
      else
        fprintf(stderr, "PASS bytesConsumed = %zu, bytesProduced = %zu\n", bytesConsumed, bytesProduced);
    }
  }
// ------------------------------------------------------------------------

  // reset the mcast structure to defaults
  mcast.src_participants = (pami_topology_t *)&src_participants;
  mcast.dst_participants = (pami_topology_t *)&dst_participants;

  mcast.src = (pami_pipeworkqueue_t *)_buffer.srcPwq();
  mcast.dst = (pami_pipeworkqueue_t *)_buffer.dstPwq();
// ------------------------------------------------------------------------
// simple mcast to all including root
// ------------------------------------------------------------------------
  {
    _doneCountdown = 1;
    //sleep(5); // instead of syncing

    new (&src_participants) PAMI::Topology(gRoot); // global root
    new (&dst_participants) PAMI::Topology(gRankList, gSize); // include root in dst_participants
    if(gRoot == task_id)
    {
      _buffer.reset(true); // isRoot = true
      // need a non-null dst pwq since I'm now including myself as a dst
      mcast.dst = (pami_pipeworkqueue_t *)_buffer.dstPwq();
    }
    else _buffer.reset(false);  // isRoot = false

    status = PAMI_Multicast(&mcast);

    if(task_id == DEBUG_RANK)
    {
      DBG_FPRINTF((stderr,"sleep\n"));
      sleep(5);
    }
    DBG_FPRINTF((stderr,"Before advance\n"));

    while(_doneCountdown)
    {
      status = PAMI_Context_advance (context, 10);
    }

    size_t
    bytesConsumed = 0,
    bytesProduced = 0;

    if(gRoot == task_id)
    {
      _buffer.validate(bytesConsumed,
                       bytesProduced,
                       true,   // isRoot = true
                       true);  // isDest = true
      if((bytesConsumed != TEST_BUF_SIZE) ||
         (bytesProduced != TEST_BUF_SIZE))
      {
        fprintf(stderr, "FAIL bytesConsumed = %zu, bytesProduced = %zu\n", bytesConsumed, bytesProduced);
      }
      else
        fprintf(stderr, "PASS bytesConsumed = %zu, bytesProduced = %zu\n", bytesConsumed, bytesProduced);
    }
    else
    {
      _buffer.validate(bytesConsumed,
                       bytesProduced);
      if((bytesConsumed != 0) ||
         (bytesProduced != TEST_BUF_SIZE))
      {
        fprintf(stderr, "FAIL bytesConsumed = %zu, bytesProduced = %zu\n", bytesConsumed, bytesProduced);
      }
      else
        fprintf(stderr, "PASS bytesConsumed = %zu, bytesProduced = %zu\n", bytesConsumed, bytesProduced);
    }
  }
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
// simple mcast to all except root, no ndata
// (Note all-sided, no active message doesn't support metadata so this
//  really does nothing)
// ------------------------------------------------------------------------
  {
    _doneCountdown = 1;
    //sleep(5); // instead of syncing

    new (&src_participants) PAMI::Topology(gRoot); // global root
    new (&dst_participants) PAMI::Topology(gRankList+1, (gSize-1)); // everyone except root in dst_participants

    mcast.connection_id = 1;

    mcast.src = (pami_pipeworkqueue_t *)NULL;
    mcast.dst = (pami_pipeworkqueue_t *)NULL;

    mcast.bytes = 0;

    status = PAMI_Multicast(&mcast);

    while(_doneCountdown)
    {
      status = PAMI_Context_advance (context, 10);
    }
  }
// ------------------------------------------------------------------------

  //sleep(5);

// ------------------------------------------------------------------------
  DBG_FPRINTF((stderr, "PAMI_Context_destroyv(&context, 1);\n"));
  status = PAMI_Context_destroyv(&context, 1);
  if(status != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable to destroy pami context. result = %d\n", status);
    return 1;
  }

  DBG_FPRINTF((stderr, "PAMI_Client_destroy(&client);\n"));
  status = PAMI_Client_destroy(&client);
  if(status != PAMI_SUCCESS)
  {
    fprintf(stderr, "Error. Unable to finalize pami client. result = %d\n", status);
    return 1;
  }

  DBG_FPRINTF((stderr, "return 0;\n"));
  return 0;
}
