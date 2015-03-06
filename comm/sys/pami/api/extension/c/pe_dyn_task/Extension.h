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
 * \file api/extension/c/pe_dyn_task/Extension.h
 * \brief PAMI extension "dynamic tasking" interface template specialization
 */
#ifndef __api_extension_c_pe_dyn_task_Extension_h__
#define __api_extension_c_pe_dyn_task_Extension_h__

#ifdef __pami_extension_dyn_task__  // configure --with-pami-extension=dyn_task

#include "api/extension/Extension.h"
#include "DynTaskExtension.h"
#include <pami.h>

namespace PAMI
{

  template <>
  void * Extension::openExtension<6000> (pami_client_t   client,
                                         const char    * name,
                                         pami_result_t & result)
  {
    (void)client;(void)name;
    result = PAMI_SUCCESS;
    return NULL;
  }

  template <>
  void Extension::closeExtension<6000> (void          * cookie,
                                        pami_result_t & result)
  {
    (void)cookie;
    result = PAMI_SUCCESS;
    return;
  }

  template <>
  void * Extension::queryExtension<6000> (const char * name,
                                          void       * cookie)
  {
    (void)cookie;
    if (strcasecmp(name, "query") == 0)
      {
        return (void *)PAMI::DynTaskExtension::client_dyn_task_query;
      }

    return NULL;
  };

}; // namespace PAMI


#endif // __pami_extension_dyn_task__
#endif // __api_extension_c_pe_dyn_task_Extension_h__
