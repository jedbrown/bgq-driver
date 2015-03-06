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
 * \file test/api/client/hello.c
 * \brief Simple hello after PAMI_Client_create()
 */

#include <pami.h>
#include <stdio.h>


int main (int argc, char ** argv)
{
  pami_client_t client;
  pami_result_t result = PAMI_ERROR;
  char         cl_string[] = "TEST";

  fprintf (stdout, "Before PAMI_Client_create()\n");
  result = PAMI_Client_create (cl_string, &client, NULL, 0);
  fprintf (stdout, "After PAMI_Client_create(), result = %d\n", result);

  fprintf (stdout, "Before PAMI_Client_destroy()\n");
  result = PAMI_Client_destroy(&client);
  fprintf (stdout, "After PAMI_Client_destroy(), result = %d\n", result);

  return 0;
};
