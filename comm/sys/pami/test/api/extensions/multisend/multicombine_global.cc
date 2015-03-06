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
 * \file test/api/extensions/multisend/multicombine_global.cc
 * \brief Simple multicombine tests. DEPRECATED
 */


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

void _done_cb(pami_context_t context, void *cookie, pami_result_t err)
{
  PAMI_assertf(_doneCountdown > 0,"doneCountdown %d\n",_doneCountdown);
  volatile int *doneCountdown = (volatile int*) cookie;
  DBG_FPRINTF((stderr, "%s:%s doneCountdown %d/%d \n",__FILE__,__PRETTY_FUNCTION__, *doneCountdown,_doneCountdown));
  --*doneCountdown;
}

int main(int argc, char ** argv)
{
  pami_client_t client;
  pami_context_t context;
  pami_result_t status = PAMI_ERROR;

  status = PAMI_Client_create("multicombine test", &client, NULL, 0);
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

  PAMI::Topology topology_global = __global.topology_global;

  pami_multicombine_t multicombine;
  memset(&multicombine, 0x00, sizeof(multicombine));

  multicombine.connection_id = 0xB0BC; // arbitrary
  multicombine.data_participants = (pami_topology_t *)&topology_global;
  multicombine.results_participants = (pami_topology_t *)&topology_global;
  multicombine.count = TEST_BUF_SIZE/sizeof(unsigned);
  multicombine.data = (pami_pipeworkqueue_t*) _buffer.srcPwq();
  multicombine.dtype = PAMI_UNSIGNED_INT;
  multicombine.optor = PAMI_MIN;
  multicombine.results = (pami_pipeworkqueue_t*) _buffer.dstPwq();
  multicombine.client = (size_t) client;	// client ID
  multicombine.context = 0;	// context ID
  multicombine.roles = -1;

  multicombine.cb_done = _cb_done;
// ------------------------------------------------------------------------
// simple multicombine
// ------------------------------------------------------------------------
  _buffer.reset(true); // isRoot = true


  _doneCountdown = 1;
  //sleep(5); // instead of combineing

  status = PAMI_Multicombine(&multicombine);

  while(_doneCountdown)
  {
    status = PAMI_Context_advance (context, 10);
  }

  size_t
  bytesConsumed = 0,
  bytesProduced = 0;

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

// ------------------------------------------------------------------------
// simple multicombine root to all with one contributing '0' to the MIN allreduce
// ------------------------------------------------------------------------
  if(task_id == topology_global.index2Rank(0))
  {
    _buffer.resetMIN0(true); // isRoot = true so set to 0's
  }
  else
  {
    _buffer.resetMIN0();
  }


  _doneCountdown = 1;

  status = PAMI_Multicombine(&multicombine);

  while(_doneCountdown)
  {
    status = PAMI_Context_advance (context, 10);
  }

  bytesConsumed = 0,
  bytesProduced = 0;

  _buffer.validateMIN0(bytesConsumed,
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
