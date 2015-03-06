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
 * \file api/extension/c/is_local_task/bgq/IsLocalTaskExtension.cc
 * \brief PAMI extension "is local task" bgq implementation
 */
#include <pami.h>

#include "api/extension/c/is_local_task/IsLocalTaskExtension.h"
#include "Global.h"

PAMI::IsLocalTaskExtension::IsLocalTaskExtension (pami_client_t client, pami_result_t & result)
{
  base    = __global.getMapCache()->torus.task2coords;
  stride  = sizeof(bgq_coords_t);
  bitmask = 0x40;

  result = PAMI_SUCCESS;
};

PAMI::IsLocalTaskExtension::~IsLocalTaskExtension ()
{
  base = NULL;
};
