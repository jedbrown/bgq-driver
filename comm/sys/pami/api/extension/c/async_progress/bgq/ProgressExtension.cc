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
 * \file api/extension/c/async_progress/bgq/ProgressExtension.cc
 * \brief PAMI extension "async progress" BGQ implementation
 */

#include "api/extension/c/async_progress/ProgressExtension.h"
#include "Global.h"
#include "Context.h"
#include "components/devices/bgq/commthread/CommThreadFactory.h"

PAMI::ProgressExtension::ProgressExtension() {
}

pami_result_t PAMI::ProgressExtension::context_async_progress_register(
				pami_context_t context,
				pamix_async_function progress_fn,
				pamix_async_function suspend_fn,
				pamix_async_function resume_fn,
				void *cookie) {
	if (progress_fn) {
		return PAMI_ERROR;
	}
	pami_result_t rc;
	rc = __commThreads.registerAsync(context, NULL, suspend_fn, resume_fn, cookie);
	return rc;
}

pami_result_t PAMI::ProgressExtension::context_async_progress_enable(
				pami_context_t context,
				pamix_async_t options) {
	if (options != PAMI_ASYNC_ALL) {
		return PAMI_ERROR;
	}

        if (__global.mapping.tSize() == 64)
        {
          PAMI::Context *ctx = (PAMI::Context *) context;
          pamix_async_function progress = NULL;
          pamix_async_function suspend  = NULL;
          pamix_async_function resume   = NULL;
          void * cookie = NULL;

          ctx->getAsyncRegs (&progress, &suspend, &resume, &cookie);

          if (resume)  resume  (context, cookie);
          if (suspend) suspend (context, cookie);

          return PAMI_SUCCESS;
        }

	pami_result_t rc;
	rc = __commThreads.addContext(context);
	return rc;
}

pami_result_t PAMI::ProgressExtension::context_async_progress_disable(
				pami_context_t context,
				pamix_async_t options) {
	if (options != PAMI_ASYNC_ALL) {
		return PAMI_ERROR;
	}
	pami_result_t rc;
	__commThreads.registerAsync(context, NULL, NULL, NULL, NULL);
	rc = __commThreads.rmContexts(context, 1);
	return rc;
}
