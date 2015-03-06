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
 * \file test/api/client/multi-client.c
 * \brief Simple hello after PAMI_Client_create()
 */

#include <pami.h>
#include <stdio.h>

#define MAX_CLIENTS 2

int main (int argc, char ** argv)
{
  pami_client_t client[MAX_CLIENTS];
  pami_result_t result = PAMI_ERROR;
  char         cl_string[] = "TEST";
  
  int max_clients=MAX_CLIENTS,i, active_clients=0;
  for(i=0;i<max_clients;i++)
  {
    fprintf (stdout, "Creating Client %d of %d\n", i, max_clients);
    result = PAMI_Client_create (cl_string, &client[i], NULL, 0);
    if(result!=PAMI_SUCCESS)
      fprintf(stderr, "--->error creating client %d\n", i);
    else active_clients++;
  }
  for(i=0;i<active_clients;i++)
  {
    fprintf (stdout, "Destroying Client %d of %d\n", i, max_clients);
    result = PAMI_Client_destroy(&client[i]);
    if(result!=PAMI_SUCCESS)
      fprintf(stderr, "--->error destroying client %d\n", i);
  }
  return 0;
};
