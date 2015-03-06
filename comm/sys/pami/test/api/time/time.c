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
 *\file test/api/time/time.c
 *\brief Simple PAMI_Wtime() test before PAMI_Client_create()
 */

#include <pami.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
int main (int argc, char ** argv)
{
  pami_result_t rc;
  pami_client_t client;
  rc = PAMI_Client_create("TEST", &client, NULL, 0);
  assert(rc == PAMI_SUCCESS);

  fprintf (stdout, "Before PAMI_Wtime()\n");
  double t0 = PAMI_Wtime (client);
  sleep(4);
  fprintf (stdout, "After first PAMI_Wtime, value = %g\n", t0);
  double t1 = PAMI_Wtime (client);
  fprintf (stdout, "After second PAMI_Wtime, value = %g\n", t1);

  fprintf (stdout, "Elapsed time, value = %g\n", t1-t0);

  return 0;
};
