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
 * \file api/extension/c/bgq_l2atomic/Extension.h
 * \brief PAMI "template" extension interface template specialization
 *
 * \todo Update doxygen and preprocessor directives
 */
#ifndef __api_extension_c_bgq_l2atomic_Extension_h__
#define __api_extension_c_bgq_l2atomic_Extension_h__
///
/// This extension specific #define is created during configure with the
/// \c --with-pami-extension=bgq_l2atomic option
///
/// \todo Change this to check for the #define specific to the extension. The
///       format is \c __pami_extension_{name}__
///
#ifdef __pami_extension_bgq_l2atomic__ // configure --with-pami-extension=bgq_l2atomic

#include "api/extension/Extension.h"
#include "BGQL2AtomicExtension.h"

namespace PAMI
{
  ///
  /// \brief Open the extension for use by a client
  ///
  /// An extension cookie is returned after the extension is opened which is
  /// provided as an input parameter when the extension is closed.
  ///
  /// Each extension implementation may define the extension cookie differently.
  /// Often the cookie is a pointer to allocated memory which may contain an
  /// instance of a class that provides the extension capabilities. Any defined
  /// extension classes are private to the extension and are not visible to
  /// objects external to the extension implementation.
  ///
  /// \todo Replace the template parameter with the unique extension identifier
  ///       assigned to the specific extension
  ///
  template <>
  void * Extension::openExtension<9000>(pami_client_t client, const char *name,
                                         pami_result_t &result)
  {
	PAMI::BGQL2AtomicExtension *x;
	pami_result_t rc;
	rc = __global.heap_mm->memalign((void **)&x, 0, sizeof(*x));
	PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc PAMI::ExtensionBGQL2Atomic");
	new (x) PAMI::BGQL2AtomicExtension();
	result = PAMI_SUCCESS;
	return (void *)x;
  }

  ///
  /// \brief Close the extension for use by a client
  ///
  /// The extension cookie was originally provided when the extension was
  /// opened.
  ///
  /// \todo Replace the template parameter with the unique extension identifier
  ///       assigned to the specific extension
  ///
  template <>
  void Extension::closeExtension<9000>(void *cookie, pami_result_t &result)
  {
	PAMI::BGQL2AtomicExtension *x = (PAMI::BGQL2AtomicExtension *)cookie;
	__global.heap_mm->free(x);
	result = PAMI_SUCCESS;
  }

  ///
  /// \brief Query the extension for a named function
  ///
  /// Returns a function pointer to the static function associated with the
  /// input parameter name.
  ///
  /// \todo Replace the template parameter with the unique extension identifier
  ///       assigned to the specific extension
  ///
  template <>
  void *Extension::queryExtension<9000>(const char *name, void *cookie)
  {
    if (strcasecmp(name, "node_memalign") == 0)
      return (void *)PAMI::BGQL2AtomicExtension::node_memalign;

    if (strcasecmp(name, "node_free") == 0)
      return (void *)PAMI::BGQL2AtomicExtension::node_free;

    if (strcasecmp(name, "proc_memalign") == 0)
      return (void *)PAMI::BGQL2AtomicExtension::proc_memalign;

    if (strcasecmp(name, "proc_free") == 0)
      return (void *)PAMI::BGQL2AtomicExtension::proc_free;

    return NULL;
  };
};

#endif // __pami_extension_bgq_l2atomic__
#endif // __api_extension_c_bgq_l2atomic_Extension_h__
