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
 * \file api/extension/c/async_progress/power/ProgressExtension.cc
 * \brief PAMI extension "async progress" POWER implementation
 */

#include "api/extension/c/async_progress/ProgressExtension.h"
#include "../lapi/include/Context.h"


PAMI::ProgressExtension::ProgressExtension() {
}

pami_result_t PAMI::ProgressExtension::context_async_progress_register(
				pami_context_t context,
				pamix_async_function progress_fn,
				pamix_async_function suspend_fn,
				pamix_async_function resume_fn,
				void *cookie) {
  LapiImpl::Context* ctx = (LapiImpl::Context*)context;
  // suspend func and resume func have to be either both NULL or both
  // implemented
  if (suspend_fn != NULL && resume_fn != NULL) {
    ctx->suspend_func = suspend_fn;
    ctx->resume_func = resume_fn;
  } else  {
    if (suspend_fn != NULL || resume_fn != NULL) 
        return PAMI_INVAL;
  }
  if (cookie != NULL)
    ctx->async_cookie = cookie;
  if (progress_fn != NULL)
    ctx->progress_func = progress_fn;

  return PAMI_SUCCESS;
}

pami_result_t PAMI::ProgressExtension::context_async_progress_enable(
				pami_context_t context,
				pamix_async_t options) {
  LapiImpl::Context* ctx = (LapiImpl::Context*)context;
  ctx->UpdatePamiAsyncProgress(options, true);
  return PAMI_SUCCESS;
}

pami_result_t PAMI::ProgressExtension::context_async_progress_disable(
				pami_context_t context,
				pamix_async_t options) {
  LapiImpl::Context* ctx = (LapiImpl::Context*)context;
  ctx->UpdatePamiAsyncProgress(options, false);
  return PAMI_SUCCESS;
}
