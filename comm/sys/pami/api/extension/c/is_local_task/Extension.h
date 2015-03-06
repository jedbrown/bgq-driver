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
 * \file api/extension/c/is_local_task/Extension.h
 * \brief PAMI extension "is local task" interface template specialization
 */
#ifndef __api_extension_c_is_local_task_Extension_h__
#define __api_extension_c_is_local_task_Extension_h__

#ifdef __pami_extension_is_local_task__ // configure --with-pami-extension=is_local_task

#include "api/extension/Extension.h"
#include "IsLocalTaskExtension.h"
#include <pami.h>

namespace PAMI
{
  template <>
  void * Extension::openExtension<9101> (pami_client_t   client,
                                         const char    * name,
                                         pami_result_t & result)
  {
    PAMI::IsLocalTaskExtension * x;
    result = __global.heap_mm->memalign((void **)&x, 0, sizeof(*x));
    PAMI_assertf(result == PAMI_SUCCESS, "Failed to alloc PAMI::IsLocalTaskExtension");
    new (x) PAMI::IsLocalTaskExtension(client, result);
    return (void *) x;
  }

  template <>
  void Extension::closeExtension<9101> (void * cookie, pami_result_t & result)
  {
    PAMI::IsLocalTaskExtension * x = (PAMI::IsLocalTaskExtension *) cookie;
    x->~IsLocalTaskExtension();
    __global.heap_mm->free (x);

    result = PAMI_SUCCESS;
    return;
  }

  template <>
  void * Extension::queryExtension<9101> (const char * name, void * cookie)
  {
    IsLocalTaskExtension * extension = (IsLocalTaskExtension *) cookie;

    if (strcasecmp (name, "base") == 0)
      return (void *) extension->base;

    if (strcasecmp (name, "stride") == 0)
      return (void *) extension->stride;

    if (strcasecmp (name, "bitmask") == 0)
      return (void *) extension->bitmask;

    return NULL;
  };
};


#endif // __pami_extension_is_local_task__
#endif // __api_extension_c_is_local_task_Extension_h__
