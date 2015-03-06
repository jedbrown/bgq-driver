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
 * \file test/api/init.c
 * \brief Simple Initialization Test
 */

#include "pami_util.h"

int main (int argc, char ** argv)
{
  pami_client_t        client;
  pami_context_t       context;
  size_t               num_contexts=1;
  pami_task_t          task_id=0;
  size_t               num_tasks=0;
  double               ti, tf;

  ti=timer();
  /* Docs01:  Initialize PAMI Clients and Contexts */
  int rc = pami_init(&client,        /* Client             */
                     &context,       /* Context            */
                     NULL,           /* Clientname=default */
                     &num_contexts,  /* num_contexts       */
                     NULL,           /* null configuration */
                     0,              /* no configuration   */
                     &task_id,       /* task id            */
                     &num_tasks);    /* number of tasks    */
  if(rc==1)
    return 1;
  /* Docs02:  Initialize PAMI Clients and Contexts */

  /* Docs03:  Print job information */
  fprintf(stderr, "Hello PAMI World:  task_id=%d, size=%zd\n",
          task_id,
          num_tasks);
  /* Docs04:  Print job information */

  /* Docs05:  Shutdown PAMI Clients and Contexts */
  rc = pami_shutdown(&client,&context,&num_contexts);
  if(rc==1)
    return 1;
  /* Docs06:  Shutdown PAMI Clients and Contexts */

  tf = timer();

  return 0;
};
