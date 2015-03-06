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
 * \file api/extension/Extension.h
 * \brief All registered PAMI extensions
 */
#ifndef __api_extension_Extension_h__
#define __api_extension_Extension_h__

#ifdef __pami_extension_dynamic__
#include <dlfcn.h>
#endif

#include "util/common.h"

namespace PAMI
{
  class Extension
  {
    protected:

      typedef void * (*open_extension_fn)  (pami_client_t client, const char * name, pami_result_t & result);
      typedef void   (*close_extension_fn) (void * cookie, pami_result_t & result);
      typedef void * (*query_extension_fn) (const char * fn, void * cookie);

      pami_client_t        _client;
      const char         * _name;
      void               * _cookie;
      close_extension_fn   _close_fn;
      query_extension_fn   _query_fn;

      ///
      /// \brief Open an extension
      ///
      template <unsigned T>
      static void * openExtension (pami_client_t   client,
                                   const char    * name,
                                   pami_result_t & result)
      {
        result = PAMI_UNIMPL;
#ifdef __pami_extension_dynamic__
        const char * dlext = ".so";
        char * dlname;
	pami_result_t rc;
	rc = __global.heap_mm->memalign((void **)&dlname, 0, strlen (name) + strlen (dlext) + 1);
	PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc dlname");
        sprintf (dlname, "%s%s", name, dlext);
        void * dlhandle = dlopen (dlname, RTLD_NOW);
        __global.heap_mm->free (dlname);

        if (dlhandle)
          result = PAMI_SUCCESS;

        return dlhandle;
#else
        return NULL;
#endif
      };

      ///
      /// \brief Close an extension
      ///
      template <unsigned T>
      static void closeExtension (void * cookie, pami_result_t & result)
      {
        result = PAMI_SUCCESS;
#ifdef __pami_extension_dynamic__
        dlclose (cookie);
#endif
        return;
      };

      ///
      /// \brief Query an extension
      ///
      template <unsigned T>
      static void * queryExtension (const char * name, void * cookie)
      {
#ifdef __pami_extension_dynamic__
        pami_result_t result = PAMI_UNIMPL;
        void * dlhandle = cookie;
        open_extension_fn fn = (open_extension_fn) dlsym (dlhandle, "queryFunction");
        return fn (name, result);
#else
        return NULL;
#endif
      };

      inline Extension (pami_client_t        client,
                        const char         * name,
                        open_extension_fn    open_fn,
                        close_extension_fn   close_fn,
                        query_extension_fn   query_fn,
                        pami_result_t      & result) :
          _client (client),
          _name (name),
          _cookie (open_fn(client, name, result)),
          _close_fn (close_fn),
          _query_fn (query_fn)
      {
      };

    public:

      template <unsigned T>
      static pami_result_t open (pami_client_t      client,
                                 const char       * name,
                                 pami_extension_t & extension)
      {
        pami_result_t result = PAMI_UNIMPL;

        size_t length = strlen (name) + 2;
        Extension * ext;
	pami_result_t rc;
	rc = __global.heap_mm->memalign((void **)&ext, 0, sizeof(*ext) + length);
	PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc ext");
        char * n = (char *) ext + 1;
        strncpy (n, name, length);

        open_extension_fn  open_fn  = (open_extension_fn)  PAMI::Extension::openExtension<T>;
        close_extension_fn close_fn = (close_extension_fn) PAMI::Extension::closeExtension<T>;
        query_extension_fn query_fn = (query_extension_fn) PAMI::Extension::queryExtension<T>;
        extension = (pami_extension_t) new (ext) Extension (client, n, open_fn, close_fn, query_fn, result);

        return result;
      };


      inline void * function (const char * name)
      {
        return _query_fn (name, _cookie);
      };

      inline pami_result_t close ()
      {
        pami_result_t result = PAMI_ERROR;
        _close_fn (_cookie, result);
        return result;
      };
  };
};

#endif // __api_extension_extension_h__
