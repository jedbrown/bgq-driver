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
 * \file test/async_progress.h
 * \brief ???
 */
#ifndef __test_async_progress_h__
#define __test_async_progress_h__

static pami_extension_t async_prog_ext;
typedef void (*pamix_async_function) (pami_context_t context, void * cookie);
typedef enum { PAMI_ASYNC_ALL = 0 } pamix_async_t;
static pami_result_t (*async_prog_register)(pami_context_t context,
					pamix_async_function progress_fn,
					pamix_async_function suspend_fn,
					pamix_async_function resume_fn,
					void *cookie);
static pami_result_t (*async_prog_enable)(pami_context_t context,
					pamix_async_t event_type);
static pami_result_t (*async_prog_disable)(pami_context_t context,
					pamix_async_t event_type);

static inline pami_result_t init_async_prog() {
	pami_result_t rc;
	rc = PAMI_Extension_open(NULL, "EXT_async_progress", &async_prog_ext);
	if (rc != PAMI_SUCCESS) {
		fprintf(stderr, "Required PAMI extension not present: EXT_async_progress\n");
		return rc;
	}
	async_prog_register = PAMI_Extension_symbol(async_prog_ext,
						"register");
	if (async_prog_register == NULL) {
		fprintf(stderr, "PAMI extension EXT_async_progress has no 'register' func\n");
		return PAMI_ERROR;
	}
	async_prog_enable = PAMI_Extension_symbol(async_prog_ext,
						"enable");
	if (async_prog_enable == NULL) {
		fprintf(stderr, "PAMI extension EXT_async_progress has no 'enable' func\n");
		return PAMI_ERROR;
	}
	async_prog_disable = PAMI_Extension_symbol(async_prog_ext,
						"disable");
	if (async_prog_disable == NULL) {
		fprintf(stderr, "PAMI extension EXT_async_progress has no 'disable' func\n");
		return PAMI_ERROR;
	}
	return PAMI_SUCCESS;
}

#endif // __test_async_progress_h__
