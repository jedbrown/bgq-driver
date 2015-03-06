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
 * \file api/extension/c/hfi_extension/Extension.h
 * \brief PAMI "HFI" extension interface template specialization
 */
#ifndef __api_extension_c_hfi_extension_Extension_h__
#define __api_extension_c_hfi_extension_Extension_h__
///
/// This extension specific #define is created during configure with the
/// \c --with-pami-extension=hfi_extension option
///
/// \todo Change this to check for the #define specific to the extension. The
///       format is \c __pami_extension_{name}__
///
#ifdef __pami_extension_hfi_extension__ // configure --with-pami-extension=hfi_extension

#include "api/extension/Extension.h"
#include "HfiExtension.h"
#include "lapi_env.h"

namespace PAMI
{
  template <>
  void * Extension::openExtension<3000> (pami_client_t   client,
                                         const char    * name,
                                         pami_result_t & result)
  {
    (void)client;(void)name;
    if (_Lapi_env.use_hfi) {
      PAMI::HfiExtension * x;
      pami_result_t rc;
      rc = __global.heap_mm->memalign((void **)&x, 0, sizeof(*x));
      PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc PAMI::HfiExtension");
      new (x) PAMI::HfiExtension();

      result = PAMI_SUCCESS;
      return (void *) x;
    } else {
      if (_Lapi_env.MP_s_enable_err_print) {
        printf("ERROR: HFI extension cannot be used for non-HFI job\n");
      }
      result = PAMI_UNIMPL;
      return NULL;
    }
  }

  ///
  /// \brief Close the extension for use by a client
  ///
  /// The extension cookie was originally provided when the extension was
  /// opened.
  ///
  template <>
  void Extension::closeExtension<3000> (void * cookie, pami_result_t & result)
  {
    PAMI::HfiExtension * x = (PAMI::HfiExtension *) cookie;
    __global.heap_mm->free (x);

    result = PAMI_SUCCESS;
    return;
  }

  ///
  /// \brief Query the extension for a named function
  ///
  /// Returns a function pointer to the static function associated with the
  /// input parameter name.
  ///
  template <>
  void * Extension::queryExtension<3000> (const char * name, void * cookie)
  {
    (void)cookie;
    if (strcasecmp (name, "hfi_pkt_counters") == 0)
      return (void *) PAMI::HfiExtension::hfi_pkt_counters;

    if (strcasecmp (name, "hfi_remote_update") == 0)
      return (void *) PAMI::HfiExtension::hfi_remote_update;

    return NULL;
  };
};

#endif // __pami_extension_hfi_extension__
#endif // __api_extension_c_hfi_extension_Extension_h__
