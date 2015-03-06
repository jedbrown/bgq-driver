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
 * \file api/extension/c/pe_extension/Extension.h
 * \brief PAMI PE extension interface template specialization
 *
 * \todo Update doxygen and preprocessor directives
 */
#ifndef __api_extension_c_pe_extension_Extension_h__
#define __api_extension_c_pe_extension_Extension_h__
///
/// This extension specific #define is created during configure with the
/// \c --with-pami-extension=pe_extension option
///
/// \todo Change this to check for the #define specific to the extension. The
///       format is \c __pami_extension_{name}__
///
#ifdef __pami_extension_pe_extension__ // configure --with-pami-extension=pe_extension

#include "api/extension/Extension.h"
#include "PeExtension.h"

namespace PAMI
{
  template <>
  void * Extension::openExtension<2000> (pami_client_t   client,
                                         const char    * name,
                                         pami_result_t & result)
  {
    (void)client;(void)name;
    result = PAMI_SUCCESS;
    return (void *) 0;
  }

  template <>
  void Extension::closeExtension<2000> (void * cookie, pami_result_t & result)
  {
    (void)cookie;
    result = PAMI_SUCCESS;
    return;
  }

  template <>
  void * Extension::queryExtension<2000> (const char * name, void * cookie)
  {
    (void)cookie;
    if (strcasecmp (name, "itrace") == 0)
      return (void *) PAMI::PeExtension::itrace;

    if (strcasecmp (name, "itrace_read_masks") == 0)
      return (void *) PAMI::PeExtension::itrace_read_masks;

    if (strcasecmp (name, "global_query") == 0)
      return (void *) PAMI::PeExtension::global_query;

    if (strcasecmp (name, "mutex_getowner") == 0)
      return NULL;
//      return (void *) PAMI_Context_mutex_getowner;

    if (strcasecmp (name, "cond_create") == 0)
      return NULL;
//     return (void *) PAMI_Context_cond_create;

    if (strcasecmp (name, "cond_wait") == 0)
      return NULL;
//      return (void *) PAMI_Context_cond_wait;

    if (strcasecmp (name, "cond_timedwait") == 0)
      return NULL;
//      return (void *) PAMI_Context_cond_timedwait;

    if (strcasecmp (name, "cond_signal") == 0)
      return NULL;
//      return (void *) PAMI_Context_cond_signal;

    if (strcasecmp (name, "cond_broadcast") == 0)
      return NULL;
//      return (void *) PAMI_Context_cond_broadcast;

    if (strcasecmp (name, "cond_destroy") == 0)
      return NULL;
//      return (void *) PAMI_Context_cond_destroy;

    return NULL;
  };
};

#endif // __pami_extension_pe_extension__
#endif // __api_extension_c_pe_extension_Extension_h__
