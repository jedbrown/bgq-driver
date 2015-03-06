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
 * \file api/extension/c/async_progress/ProgressExtension.h
 * \brief PAMI extension "progress extension" interface
 */
#ifndef __api_extension_c_async_progress_ProgressExtension_h__
#define __api_extension_c_async_progress_ProgressExtension_h__

#include <pami.h>

namespace PAMI
{

  class ProgressExtension
  {

    public:

      typedef void (*pamix_async_function) (pami_context_t context, void * cookie);

      typedef enum
      {
        PAMI_ASYNC_ALL                = 0,
        PAMI_ASYNC_RECV_INTERRUPT     = 1,
        PAMI_ASYNC_TIMER              = 2,
        PAMI_ASYNC_EXT                = 1000
      } pamix_async_t;

      ProgressExtension();

      static pami_result_t context_async_progress_register(pami_context_t         context,
                                                           pamix_async_function   progress_fn,
                                                           pamix_async_function   suspend_fn,
                                                           pamix_async_function   resume_fn,
                                                           void                 * cookie);

      static pami_result_t context_async_progress_enable (pami_context_t context,
                                                          pamix_async_t  event_type);

      static pami_result_t context_async_progress_disable (pami_context_t context,
                                                           pamix_async_t  event_type);

  }; // class ProgressExtension

}; // namespace PAMI

#endif // __api_extension_c_async_progress_ProgressExtension_h__
