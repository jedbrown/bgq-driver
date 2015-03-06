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
 * \file test/api/context/post-multithreaded-traffic.c
 * \brief Simple PAMI_Context_post() test
 */

#include <assert.h>
#include <pami.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>

#define NUM_CONTEXT    1
#define NUM_LOOP       1024*8
#define POLL_CNT       10000
#define MPCI_HDR_SIZE  24

int myid;
int num_tasks;
int num_recved = 0;
int recv_count[NUM_CONTEXT];
char recv_buf[NUM_LOOP*2];
char send_buf[NUM_CONTEXT][NUM_LOOP*2];

#define RC0(statement) \
{ \
    int rc = statement; \
    if (rc != 0) { \
        printf(#statement " rc = %d, line %d\n", rc, __LINE__); \
        exit(-1); \
    } \
}
void
pami_am_complete(pami_context_t context, void *cookie, pami_result_t result)
{
    /* do nothing */
}
void pami_recv_handler(pami_context_t context, void *cookie,
        const void *header_addr, size_t header_size, 
        const void *pipe_addr, size_t data_size, pami_endpoint_t src, pami_recv_t *recv)
{
    assert(header_size == sizeof(int));
    int context_id = *(int*)header_addr;
    
    pami_result_t  result = PAMI_SUCCESS;

    if (pipe_addr) {
        assert(recv == NULL);
        memcpy(recv_buf, pipe_addr, data_size);
        pami_am_complete(context, NULL, result );
    } else {
        assert(recv != NULL);
        bzero(recv, sizeof(*recv));
        recv->local_fn = pami_am_complete;
        recv->addr     = recv_buf;
        recv->type     = PAMI_TYPE_BYTE;
        recv->cookie   = NULL;
        recv->offset   = 0;
        recv->hints.inline_completion = 0;
    }

    recv_count[context_id]++;
}

typedef struct post_info
{
  pami_work_t        state;
  volatile unsigned value;
} post_info_t;

typedef struct machine_info
{
  pami_context_t *context;
  int             context_id;
} machine_info_t;  

post_info_t _info[NUM_CONTEXT];

void ping_pong(pami_context_t* context, int context_id)
{
  int dest = myid^1;  
  char             msg_hdr[MPCI_HDR_SIZE];
  pami_send_t psend;
  bzero(&psend, sizeof(psend));
  psend.send.dispatch        = 2;
  psend.send.dest            = dest;
  psend.send.header.iov_base = &context_id;
  psend.send.header.iov_len  = sizeof(context_id);
  psend.send.data.iov_base   = &(send_buf[context_id]);
  psend.events.local_fn      = NULL;
  psend.events.remote_fn     = NULL;
  psend.events.cookie        = NULL;

  recv_count[context_id] = 0;

  if ( (myid & 1) == 0) {
    for (int i = 0; i < NUM_LOOP; i ++) {
      psend.send.data.iov_len    = i+1;
      RC0( PAMI_Send(*context, &psend) );
      while (recv_count[context_id] <= i) {
          PAMI_Context_advance(*context, POLL_CNT);  
      }  
#if 0
      if (i%100 == 0)
          fprintf(stderr, "***PINGPONG_LOOP***%u-th iteration on task %u\n", 
                  i, myid);
#endif
    }
    while (recv_count[context_id] != NUM_LOOP)
        PAMI_Context_advance(*context, POLL_CNT);  
  } else {
    for (int i = 0; i < NUM_LOOP; i ++) {
      while (recv_count[context_id] <= i) {
          PAMI_Context_advance(*context, POLL_CNT);  
      }  
      psend.send.data.iov_len    = i+1;
      RC0( PAMI_Send(*context, &psend) );
#if 0
      if (i%100 == 0)
          fprintf(stderr, "***PINGPONG_LOOP***%u-th iteration on task %u\n", 
                  i, myid);
#endif
    }
    while (recv_count[context_id] != NUM_LOOP)
        PAMI_Context_advance(*context, POLL_CNT);  
  }
  fprintf(stderr, "***PINGPONG_LOOP***Succeeded with %u iterations!\n",
          NUM_LOOP);
}

pami_result_t do_work (pami_context_t   context,
              void          * cookie)
{
  post_info_t *info = (post_info_t *)cookie;
  
  fprintf (stderr, "do_work() cookie = %p, %d -> %d\n", cookie, info->value, info->value-1);
  info->value--;

  return PAMI_SUCCESS;
}

void *post_machine (void * arg)
{
  machine_info_t *minfo = (machine_info_t*)arg;

  ping_pong(minfo->context, minfo->context_id);

  /* Post some work to the contexts */
  pami_result_t result = PAMI_Context_post(*(minfo->context),
          &_info[minfo->context_id].state, do_work, (void *)&_info[minfo->context_id]);
  if (result != PAMI_SUCCESS)
  {
      fprintf (stderr, "Error. Unable to post work to the %u-th pami context. result = %d\n", 
              minfo->context_id, result);
      return NULL;
  }
}

int main (int argc, char ** argv)
{
  for (int i = 0; i < NUM_CONTEXT; i++) {
      for (int j = 0; j < 1024; j ++)
          send_buf[i][j] = 48;
  }

  pami_client_t client;
  pami_context_t context[NUM_CONTEXT];
  pami_configuration_t configuration;
  char                  cl_string[] = "lapi";
  pami_result_t result = PAMI_ERROR;
  pthread_t     post_thread[NUM_CONTEXT];
  machine_info_t minfo[NUM_CONTEXT];
  int           res;

  result = PAMI_Client_create (cl_string, &client, NULL, 0);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to initialize pami client. result = %d\n", result);
    return 1;
  }

  configuration.name = PAMI_CLIENT_NUM_CONTEXTS;
  result = PAMI_Client_query(client, &configuration, 1);
  if (configuration.value.intval < NUM_CONTEXT)
  {
    fprintf (stderr, "Error. Multi-context not supported. PAMI_CLIENT_NUM_CONTEXTS = %zu\n", configuration.value.intval);
    return 1;
  }

  configuration.name = PAMI_CLIENT_TASK_ID;
  result = PAMI_Client_query(client, &configuration, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to query TASK_ID. result = %d\n", result);
    return 1;
  }
  myid = configuration.value.intval;

  configuration.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query(client, &configuration, 1);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to query NUM_TASKS. result = %d\n", result);
    return 1;
  }
  num_tasks = configuration.value.intval;
  
  size_t num = NUM_CONTEXT;
  result = PAMI_Context_createv (client, &configuration, 0, context, num);
  if (result != PAMI_SUCCESS || num != NUM_CONTEXT)
  {
    fprintf (stderr, "Error. Unable to create two pami context. result = %d\n", result);
    return 1;
  }


  pami_send_hint_t null_send_hint = {0};

  for (int i = 0; i < NUM_CONTEXT; i++) {
      
      pami_dispatch_callback_fn  recv_callback;
      recv_callback.p2p = pami_recv_handler;
      RC0( PAMI_Dispatch_set(context[i], 2, recv_callback, NULL, null_send_hint) );

      _info[i].value = 1;
      minfo[i].context = &context[i];
      minfo[i].context_id = i; 
      if (res = pthread_create(&post_thread[i], NULL, post_machine, &minfo[i])) {
          fprintf (stderr, "Error. Unable to create a thread for the %u-th pami context. result = %u\n", 
                  i, res);
          return 1;
      }
  }

  volatile unsigned info_value;

  do 
  {
      info_value = 0;

      for (int i = 0; i < NUM_CONTEXT; i ++) {
          result = PAMI_Context_advance (context[i], 1);
          if (result != PAMI_SUCCESS && result != PAMI_EAGAIN)
          {
              fprintf (stderr, "Error. Unable to advance the %u-th pami context. result = %d\n", 
                      i, result);
              return 1;
          }
          info_value += _info[i].value;
      }

  } while (info_value);

  result = PAMI_Context_destroyv(context, num);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to destroy first pami context. result = %d\n", result);
    return 1;
  }

  result = PAMI_Client_destroy(&client);
  if (result != PAMI_SUCCESS)
  {
    fprintf (stderr, "Error. Unable to finalize pami client. result = %d\n", result);
    return 1;
  }

  fprintf (stderr, "Success.\n");

  return 0;
};
