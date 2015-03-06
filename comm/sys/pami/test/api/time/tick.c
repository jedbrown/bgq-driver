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
 * \file test/api/time/tick.c
 * \brief Simple PAMI_Wtick() test before PAMI_Client_create()
 */

#include <pami.h>
#include <stdio.h>

int main (int argc, char ** argv)
{
  pami_client_t client;
  size_t num=1;
  pami_context_t context;
  pami_configuration_t query = {name:PAMI_CLIENT_WTICK};
  double value;

  PAMI_Client_create("TEST", &client, NULL, 0);
  PAMI_Context_createv(client, NULL, 0, &context, num);

  fprintf (stdout, "Before PAMI_Wtick()\n");

  PAMI_Client_query(client, &query,1);
  value = query.value.doubleval;

  fprintf (stdout, "After PAMI_Wtick, value = %g\n", value);

  return 0;
};
