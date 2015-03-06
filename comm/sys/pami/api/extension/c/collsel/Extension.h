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
 * \file api/extension/c/collsel/Extension.h
 * \brief PAMI "template" extension interface template specialization
 *
 * \todo Update doxygen and preprocessor directives
 */
#ifndef __api_extension_c_collsel_Extension_h__
#define __api_extension_c_collsel_Extension_h__
#ifdef __pami_extension_collsel__

#include "api/extension/c/collsel/CollselExtension.h"

namespace PAMI{
  template <>
  void * Extension::openExtension<5000>(pami_client_t   client,
                                        const char    * name,
                                        pami_result_t & result)
  {
    result = PAMI_SUCCESS;
    return NULL;
  }

  template <>
  void Extension::closeExtension<5000> (void          * cookie,
                                        pami_result_t & result)
  {
    result = PAMI_SUCCESS;
    return;
  }
  template <>
  void * Extension::queryExtension<5000> (const char * name,
                                          void       * cookie)
  {
    if (strcasecmp (name, "Collsel_init_fn") == 0)
      return (void *) PAMI::CollselExtension::Collsel_init_fn;

    if (strcasecmp (name, "Collsel_destroy_fn") == 0)
      return (void *) PAMI::CollselExtension::Collsel_destroy_fn;

    if (strcasecmp (name, "Collsel_table_generate_fn") == 0)
      return (void *) PAMI::CollselExtension::Collsel_table_generate_fn;

    if (strcasecmp (name, "Collsel_table_load_fn") == 0)
      return (void *) PAMI::CollselExtension::Collsel_table_load_fn;

    if (strcasecmp (name, "Collsel_table_unload_fn") == 0)
      return (void *) PAMI::CollselExtension::Collsel_table_unload_fn;

    if (strcasecmp (name, "Collsel_query_fn") == 0)
      return (void *) PAMI::CollselExtension::Collsel_query_fn;

    if (strcasecmp (name, "Collsel_advise_fn") == 0)
      return (void *) PAMI::CollselExtension::Collsel_advise_fn;

    return NULL;
  };
};

#endif // __pami_extension_collsel__
#endif // __api_extension_c_collsel_Extension_h__
