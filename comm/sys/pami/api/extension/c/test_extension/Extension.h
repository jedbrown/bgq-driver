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
 * \file api/extension/c/test_extension/Extension.h
 * \brief PAMI "test" extension interface template specialization
 */
#ifndef __api_extension_c_test_extension_Extension_h__
#define __api_extension_c_test_extension_Extension_h__

#ifdef __pami_extension_test_extension__ // configure --with-pami-extension=test_extension

#include "api/extension/Extension.h"

namespace PAMI
{
  class ExtensionTest
  {
    public:

      typedef struct
      {
        size_t line;
        char   func[128];
      } info_t;

      static void foo ()
      {
        fprintf (stderr, "This is function 'foo' of extension 'test'. %s() [%s:%d]\n",
                 __FUNCTION__, __FILE__, __LINE__);
      };

      static void bar (info_t * info)
      {
        fprintf (stderr, "This is function 'bar' of extension 'test'.\n");
        info->line = __LINE__;
        snprintf (info->func, 127, "%s", __FUNCTION__);
      };
  };

  template <>
  void * Extension::openExtension<1> (pami_client_t   client,
                                      const char    * name,
                                      pami_result_t & result)
  {
    result = PAMI_SUCCESS;
    return NULL;
  }

  template <>
  void Extension::closeExtension<1> (void * cookie, pami_result_t & result)
  {
    result = PAMI_SUCCESS;
    return;
  }

  template <>
  void * Extension::queryExtension<1> (const char * name, void * cookie)
  {
    if (strcasecmp (name, "foo") == 0)
      return (void *) PAMI::ExtensionTest::foo;

    if (strcasecmp (name, "bar") == 0)
      return (void *) PAMI::ExtensionTest::bar;

    return NULL;
  };
};

#endif // __pami_extension_test_extension__
#endif // __api_extension_c_test_extension_Extension_h__
