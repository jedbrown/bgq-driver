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
 * \file test/api/p2p/send/hint/dispatch.c
 * \brief ???
 */

#include "send_perf.h"

int main (int argc, char ** argv)
{

  dispatch_info_t dispatch[100];
  size_t n = 0;
  
  dispatch[n].id = n+10;
  dispatch[n].name = "default";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  n++;

  /* *****************************************************************
   * Test the 'multicontext' hint
   * *****************************************************************/
  dispatch[n].id = n+10;
  dispatch[n].name = "+ multicontext";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.multicontext = PAMI_HINT_ENABLE;
  n++;

  dispatch[n].id = n+10;
  dispatch[n].name = "- multicontext";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.multicontext = PAMI_HINT_DISABLE;
  n++;


  /* *****************************************************************
   * Test the 'long_header' hint
   * *****************************************************************/
  dispatch[n].id = n+10;
  dispatch[n].name = "+ long_header";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.long_header = PAMI_HINT_ENABLE;
  n++;

  dispatch[n].id = n+10;
  dispatch[n].name = "- long_header";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.long_header = PAMI_HINT_DISABLE;
  n++;


  /* *****************************************************************
   * Test the 'recv_contiguous' hint
   * *****************************************************************/
  dispatch[n].id = n+10;
  dispatch[n].name = "+ recv_contiguous";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.recv_contiguous = PAMI_HINT_ENABLE;
  n++;

  dispatch[n].id = n+10;
  dispatch[n].name = "- recv_contiguous";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.recv_contiguous = PAMI_HINT_DISABLE;
  n++;


  /* *****************************************************************
   * Test the 'recv_copy' hint
   * *****************************************************************/
  dispatch[n].id = n+10;
  dispatch[n].name = "+ recv_copy";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.recv_copy = PAMI_HINT_ENABLE;
  n++;

  dispatch[n].id = n+10;
  dispatch[n].name = "- recv_copy";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.recv_copy = PAMI_HINT_DISABLE;
  n++;


  /* *****************************************************************
   * Test the 'recv_immediate' hint
   * *****************************************************************/
  dispatch[n].id = n+10;
  dispatch[n].name = "+ recv_immediate";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.recv_immediate = PAMI_HINT_ENABLE;
  n++;

  dispatch[n].id = n+10;
  dispatch[n].name = "- recv_immediate";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.recv_immediate = PAMI_HINT_DISABLE;
  n++;


  /* *****************************************************************
   * Test the 'consistency' hint
   * *****************************************************************/
  dispatch[n].id = n+10;
  dispatch[n].name = "+ consistency";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.consistency = PAMI_HINT_ENABLE;
  n++;

  dispatch[n].id = n+10;
  dispatch[n].name = "- consistency";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.consistency = PAMI_HINT_DISABLE;
  n++;


  /* *****************************************************************
   * Test the 'buffer_registered' hint
   * *****************************************************************/
  dispatch[n].id = n+10;
  dispatch[n].name = "+ buffer_registered";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.buffer_registered = PAMI_HINT_ENABLE;
  n++;

  dispatch[n].id = n+10;
  dispatch[n].name = "- buffer_registered";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.buffer_registered = PAMI_HINT_DISABLE;
  n++;


  /* *****************************************************************
   * Test the 'remote_async_progress' hint
   * *****************************************************************/
  dispatch[n].id = n+10;
  dispatch[n].name = "+ remote_async_progress";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.remote_async_progress = PAMI_HINT_ENABLE;
  n++;

  dispatch[n].id = n+10;
  dispatch[n].name = "- remote_async_progress";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.remote_async_progress = PAMI_HINT_DISABLE;
  n++;


  /* *****************************************************************
   * Test the 'use_rdma' hint
   * *****************************************************************/
  dispatch[n].id = n+10;
  dispatch[n].name = "+ use_rdma";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.use_rdma = PAMI_HINT_ENABLE;
  n++;

  dispatch[n].id = n+10;
  dispatch[n].name = "- use_rdma";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.use_rdma = PAMI_HINT_DISABLE;
  n++;


  /* *****************************************************************
   * Test the 'use_shmem' hint
   * *****************************************************************/
  dispatch[n].id = n+10;
  dispatch[n].name = "+ use_shmem";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.use_shmem = PAMI_HINT_ENABLE;
  n++;

  dispatch[n].id = n+10;
  dispatch[n].name = "- use_shmem";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.use_shmem = PAMI_HINT_DISABLE;
  n++;


  /* *****************************************************************
   * Test the 'queue_immediate' hint
   * *****************************************************************/
  dispatch[n].id = n+10;
  dispatch[n].name = "+ queue_immediate";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.queue_immediate = PAMI_HINT_ENABLE;
  n++;

  dispatch[n].id = n+10;
  dispatch[n].name = "- queue_immediate";
  dispatch[n].options = (pami_dispatch_hint_t) {0};
  dispatch[n].options.queue_immediate = PAMI_HINT_DISABLE;
  n++;





  pami_client_t client = initialize (dispatch, n, 0);

  if (PAMIX_Client_task (client) == 0)
    {
      fprintf (stdout, "\n");
      fprintf (stdout, "PAMI_Dispatch_set() 'hint' implementation functional test\n");
      fprintf (stdout, "\n");
    }
  
  unsigned i, errors = 0;
  for (i=0; i<n; i++)
  {
    switch (dispatch[i].result)
    {
      case PAMI_SUCCESS:
        if (PAMIX_Client_task (client) == 0)
          fprintf (stdout, "[PASSED] hint '%s'\n", dispatch[i].name);
        break;
      case PAMI_UNIMPL:
        if (PAMIX_Client_task (client) == 0)
          fprintf (stdout, "[FAILED] hint '%s' is not implemented\n", dispatch[i].name);
        errors++;
        break;
      default:
        if (PAMIX_Client_task (client) == 0)
          fprintf (stdout, "[FAILED] hint '%s' returned an unknown error: %d\n", dispatch[i].name, dispatch[i].result);
        errors++;
        break;
    }
  }
  
  if (PAMIX_Client_task (client) == 0)
    {
      fprintf (stdout, "\n");
      fprintf (stdout, "Test %s, errors = %d\n", errors>0?"FAILED":"PASSED", errors);
      fprintf (stdout, "\n");
    }
  

  return (errors>0);
}
