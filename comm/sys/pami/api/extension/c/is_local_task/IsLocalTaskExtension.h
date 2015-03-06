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
 * \file api/extension/c/is_local_task/IsLocalTaskExtension.h
 * \brief PAMI extension "is local task" interface
 */
#ifndef __api_extension_c_is_local_task_IsLocalTaskExtension_h__
#define __api_extension_c_is_local_task_IsLocalTaskExtension_h__

#include <pami.h>
#include <stdint.h>

namespace PAMI
{
  class IsLocalTaskExtension
  {
    public:

      void      * base;
      uintptr_t   stride;
      uintptr_t   bitmask;

      IsLocalTaskExtension (pami_client_t client, pami_result_t & result);

      ~IsLocalTaskExtension ();
  };
};

#endif // __api_extension_c_is_local_task_IsLocalTaskExtension_h__
