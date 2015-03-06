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
 * \file api/extension/c/torus_network/Extension.h
 * \brief PAMI extension "torus network" interface template specialization
 */
#ifndef __api_extension_c_torus_network_Extension_h__
#define __api_extension_c_torus_network_Extension_h__

#ifdef __pami_extension_torus_network__ // configure --with-pami-extension=torus_network

#include "api/extension/Extension.h"
#include "TorusExtension.h"
#include <pami.h>

namespace PAMI
{
  template <>
  void * Extension::openExtension<1000> (pami_client_t   client,
                                         const char    * name,
                                         pami_result_t & result)
  {
    PAMI::TorusExtension * x;
    pami_result_t rc;
    rc = __global.heap_mm->memalign((void **)&x, 0, sizeof(*x));
    PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc PAMI::TorusExtension");
    new (x) PAMI::TorusExtension();

    result = PAMI_SUCCESS;
    return (void *) x;
  }

  template <>
  void Extension::closeExtension<1000> (void * cookie, pami_result_t & result)
  {
    PAMI::TorusExtension * x = (PAMI::TorusExtension *) cookie;
    __global.heap_mm->free (x);

    result = PAMI_SUCCESS;
    return;
  }

  template <>
  void * Extension::queryExtension<1000> (const char * name, void * cookie)
  {
    if (strcasecmp (name, "information") == 0)
      return (void *) PAMI::TorusExtension::information;

    if (strcasecmp (name, "task2torus") == 0)
      return (void *) PAMI::TorusExtension::task2torus;

    if (strcasecmp (name, "torus2task") == 0)
      return (void *) PAMI::TorusExtension::torus2task;

    return NULL;
  };
};


#endif // __pami_extension_torus_network__
#endif // __api_extension_c_torus_network_Extension_h__
