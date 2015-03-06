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
 * \file test/api/extensions/multisend/multicast_local.cc
 * \brief Simple multicast tests on local topology. DEPRECATED
 */

#include "Buffer.h"

#include "Global.h"
#include <unistd.h>
#ifndef TEST_BUF_SIZE
  #define TEST_BUF_SIZE	32
#endif // TEST_BUF_SIZE

#warning DEPRECATED. Update your build

static PAMI::Test::Buffer<TEST_BUF_SIZE> _buffer;

static   void       *_cookie=(void*)"HI COOKIE";
static int           _doneCountdown;
pami_callback_t       _cb_done;
const pami_quad_t     _msginfo = {0,1,2,3};

void dispatch_multicast_fn(const pami_quad_t     *msginfo,
                           unsigned              msgcount,
                           unsigned              connection_id,
                           size_t                root,
                           size_t                sndlen,
                           void                 *clientdata,
                           size_t               *rcvlen,
                           pami_pipeworkqueue_t **rcvpwq,
                           pami_callback_t       *cb_done)
{
  DBG_FPRINTF((stderr,"%s:%s msgcount %d, connection_id %d, root %zu, sndlen %zu, cookie %s\n",
               __FILE__,__PRETTY_FUNCTION__,msgcount, connection_id, root, sndlen, (char*) clientdata));
  PAMI_assertf(_doneCountdown > 0,"doneCountdown %d\n",_doneCountdown);
  PAMI_assertf(sndlen <= TEST_BUF_SIZE,"sndlen %zu\n",sndlen);
  PAMI_assertf(msgcount == 1,"msgcount %d",msgcount);
  PAMI_assertf(msginfo->w0 == _msginfo.w0,"msginfo->w0=%d\n",msginfo->w0);
  PAMI_assertf(msginfo->w1 == _msginfo.w1,"msginfo->w1=%d\n",msginfo->w1);
  PAMI_assertf(msginfo->w2 == _msginfo.w2,"msginfo->w2=%d\n",msginfo->w2);
  PAMI_assertf(msginfo->w3 == _msginfo.w3,"msginfo->w3=%d\n",msginfo->w3);

  PAMI::PipeWorkQueue * pwq;
  pwq = _buffer.dstPwq();
  DBG_FPRINTF((stderr,"%s:%s bytesAvailable (%p) %zu, %zu done out of %zu\n",__FILE__,__PRETTY_FUNCTION__,
               pwq,pwq->bytesAvailableToProduce(),pwq->getBytesProduced(),sndlen));

  *rcvlen = sndlen;

  *rcvpwq = (pami_pipeworkqueue_t*) pwq;

  *cb_done = _cb_done;

}

void _done_cb(pami_context_t context, void *cookie, pami_result_t err)
{
  PAMI_assertf(_doneCountdown > 0,"doneCountdown %d\n",_doneCountdown);
  volatile int *doneCountdown = (volatile int*) cookie;
  DBG_FPRINTF((stderr, "%s:%s done %d \n",__FILE__,__PRETTY_FUNCTION__, *doneCountdown));
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

  fn.multicast = &dispatch_multicast_fn;

  pami_dispatch_hint_t        options;
  memset(&options, 0x00, sizeof(options));

  options.type = PAMI_MULTICAST;

  options.config = NULL;

  options.hint.multicast.local = 1;
  options.hint.multicast.one_sided = 1;
  options.hint.multicast.active_message = 1;

  status = PAMI_Dispatch_set_new(context,
                                dispatch,
                                fn,
                                _cookie,
                                options);

  //For testing ease, I'm assuming rank list topology, so convert them
  PAMI::Topology topology_global = __global.topology_global;
  topology_global.convertTopology(PAMI_LIST_TOPOLOGY);

  PAMI::Topology topology_local  = __global.topology_local;
  topology_local.convertTopology(PAMI_LIST_TOPOLOGY);

  // local topology variables
  pami_task_t  lRoot    = topology_local.index2Rank(0);
  pami_task_t *lRankList=NULL; topology_local.rankList(&lRankList);
  size_t  lSize   =  topology_local.size();

  PAMI::Topology src_participants;
  PAMI::Topology dst_participants;

  pami_multicast_t mcast;
  memset(&mcast, 0x00, sizeof(mcast));
  if(lRoot == task_id)
  {

    mcast.dispatch = dispatch;
    mcast.connection_id = task_id; //0xB;
    mcast.msginfo = &_msginfo;
    mcast.msgcount = 1;
    mcast.src_participants = (pami_topology_t *)&src_participants;
    mcast.dst_participants = (pami_topology_t *)&dst_participants;

    mcast.src = (pami_pipeworkqueue_t *)_buffer.srcPwq();
    mcast.dst = (pami_pipeworkqueue_t *)NULL;

        mcast.client = 0;
        mcast.context = 0;
    mcast.roles = -1;
    mcast.bytes = TEST_BUF_SIZE;

    mcast.cb_done = _cb_done;
  }

// ------------------------------------------------------------------------
// simple local mcast to all except root
// ------------------------------------------------------------------------
  {
    _doneCountdown = 1;
    //sleep(5); // instead of syncing

    new (&src_participants) PAMI::Topology(lRoot); // local root
    new (&dst_participants) PAMI::Topology(lRankList+1, (lSize-1)); // everyone except root in dst_participants
    if(lRoot == task_id)
    {
      _buffer.reset(true); // isRoot = true

      status = PAMI_Multicast(&mcast);
    }

    while(_doneCountdown)
    {
      status = PAMI_Context_advance (context, 10);
    }
    size_t
    bytesConsumed = 0,
    bytesProduced = 0;

    if(lRoot == task_id)
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
// simple local mcast to all including root
// ------------------------------------------------------------------------
  {
    _doneCountdown = 1;
    //sleep(5); // instead of syncing

    new (&src_participants) PAMI::Topology(lRoot); // local root
    new (&dst_participants) PAMI::Topology(lRankList, lSize); // include root in dst_participants
    if(lRoot == task_id)
    {
      _buffer.reset(true); // isRoot = true
      // need a non-null dst pwq since I'm now including myself as a dst
      mcast.dst = (pami_pipeworkqueue_t *)_buffer.dstPwq();

      status = PAMI_Multicast(&mcast);
    }

    while(_doneCountdown)
    {
      status = PAMI_Context_advance (context, 10);
    }

    size_t
    bytesConsumed = 0,
    bytesProduced = 0;

    if(lRoot == task_id)
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
