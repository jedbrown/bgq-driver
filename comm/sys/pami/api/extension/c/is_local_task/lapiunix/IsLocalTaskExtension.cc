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
 * \file api/extension/c/is_local_task/lapiunix/IsLocalTaskExtension.cc
 * \brief PAMI extension "is local task" lapiunix implementation
 */

#include <stdio.h>
#include <pami.h>

#include "api/extension/c/is_local_task/IsLocalTaskExtension.h"

PAMI::IsLocalTaskExtension::IsLocalTaskExtension (pami_client_t client, pami_result_t & result)
{
  int i;  
  int n_shm_tasks = 0;
  int n_tasks     = 0;

  /* Not sure how to avoid these up-calls */
  pami_configuration_t config;

  config.name = PAMI_CLIENT_NUM_LOCAL_TASKS;
  result = PAMI_Client_query (client, &config, 1);

  if (result != PAMI_SUCCESS)
    {
      fprintf(stderr, " PAMI_Client_query w/PAMI_CLIENT_NUM_LOCAL_TASKS rc=%d\n", result);
      fflush(stderr);
      return;
    }

  n_shm_tasks = config.value.intval;

  config.name = PAMI_CLIENT_NUM_TASKS;
  result = PAMI_Client_query (client, &config, 1);

  if (result != PAMI_SUCCESS)
    {
      fprintf(stderr, " PAMI_Client_query w/PAMI_CLIENT_NUM_TASKS rc=%d\n", result);
      fflush(stderr);
      return;
    }

  n_tasks = config.value.intval;

  config.name = PAMI_CLIENT_LOCAL_TASKS;
  result = PAMI_Client_query (client, &config, 1);

  if (result != PAMI_SUCCESS)
    {
      fprintf(stderr, "PAMI_Client_query w/PAMI_CLIENT_LOCAL_TASKS rc=%d\n", result);
      fflush(stderr);
      return;
    }

  base = malloc (n_tasks * sizeof(uint8_t));

  if (base == NULL)
    {
      result = PAMI_ERROR;
      return;
    }

  uint8_t * is_local = (uint8_t *) base;

  for (i = 0; i < n_tasks; i++)
    is_local[i] = 0;

  for (i = 0; i < n_shm_tasks; i++)
    is_local[config.value.intarray[i]] = 1;

  stride  = sizeof(uint8_t);
  bitmask = 0x01;

  result = PAMI_SUCCESS;
};

PAMI::IsLocalTaskExtension::~IsLocalTaskExtension ()
{
  free (base);
  base = NULL;
};
