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
///
/// \file test/internals/BG/bgq/commthreads/commthread_test.h
/// \brief Simple test for basic commthread functionality
///

#ifndef __test_internals_BG_bgq_commthreads_commthread_test_h__
#define __test_internals_BG_bgq_commthreads_commthread_test_h__

#include <pami.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "misc-util.h"

typedef struct post_info {
	pami_work_t state;
	volatile unsigned value;
	unsigned ctx;
	unsigned seq;
	pami_send_t send; // not used by do_work()
	unsigned long long t0;
	pami_client_t client;
} post_info_t;

#ifndef NUM_CONTEXTS
#define NUM_CONTEXTS	4
#endif /* ! NUM_CONTEXTS */

#ifndef NUM_TESTRUNS
#define NUM_TESTRUNS	8
#endif /* ! NUM_TESTRUNS */

post_info_t _info[NUM_CONTEXTS];

#ifdef EVICT_BUF_SIZE
#include <assert.h>
uint64_t *evict_buf = NULL;
#ifndef EVICT_ITER_SIZE
#define EVICT_ITER_SIZE	(EVICT_BUF_SIZE / 8)
#endif // !EVICT_ITER_SIZE
#endif // EVICT_BUF_SIZE

void test_init() {
#ifdef EVICT_BUF_SIZE
	if (!evict_buf) {
		posix_memalign((void **)&evict_buf, sizeof(*evict_buf), EVICT_BUF_SIZE * sizeof(*evict_buf));
		assert(evict_buf);
	}
#endif // EVICT_BUF_SIZE
}

void do_sleep(pami_client_t client, char *buf, int bufl, char *end, size_t number) {
	buf[0] = 'S'; buf[1] = 'p';
	write(2, buf, bufl);
#ifdef EVICT_BUF_SIZE
	static int evx = 0;
#endif // EVICT_BUF_SIZE
	{unsigned long long t0 = PAMI_Wtimebase(client);
	unsigned long long some_num = number * 100000;
	while (PAMI_Wtimebase(client) - t0 < some_num) {
#ifdef EVICT_BUF_SIZE
		int evz = evx;
		int evy = evz + EVICT_ITER_SIZE;
		if (evy >= EVICT_BUF_SIZE) evy -= EVICT_BUF_SIZE;
		for (; evz != evy;) {
			++evict_buf[evz];
			if (++evz >= EVICT_BUF_SIZE) evz -= EVICT_BUF_SIZE;
		}
		evx = evz;
#endif // EVICT_BUF_SIZE
	}}
	buf[0] = end[0]; buf[1] = end[1];
	write(2, buf, bufl);
}

pami_result_t do_work(pami_context_t context, void *cookie) {
	post_info_t *info = (post_info_t *)cookie;
	char buf[128];
	sprintf(buf, "do_work(%d) by %ld on context %d: cookie = %p, %d -> %d (%lld cy)\n",
			info->seq, pthread_self(), info->ctx, cookie,
			info->value, info->value - 1,
			PAMI_Wtimebase(info->client) - info->t0);
	write(2, buf, strlen(buf));
	// [f]printf() is susceptible to context switches...
	//fprintf(stderr, "do_work(%d) by %ld on context %d: cookie = %p, %d -> %d\n",
	//			info->seq, pthread_self(), info->ctx, cookie, info->value, info->value - 1);
	--info->value;
	return PAMI_SUCCESS;
}

void done_send(pami_context_t context, void *cookie, pami_result_t err) {
	post_info_t *info = (post_info_t *)cookie;
	--info->value;
}

pami_result_t do_send(pami_context_t context, void *cookie) {
	post_info_t *info = (post_info_t *)cookie;
	char buf[128];
	sprintf(buf, "do_send(%d) by %ld on context %d: cookie = %p, %d -> %d\n",
				info->seq, pthread_self(), info->ctx, cookie, info->value, info->value - 1);
	write(2, buf, strlen(buf));
	pami_result_t rc = PAMI_Send(context, &info->send);
	if (rc != PAMI_SUCCESS) {
fprintf(stderr, "failed sending %d\n", info->seq);
	}
	return PAMI_SUCCESS;
}

void do_recv(pami_context_t context,
             void *cookie,
             const void *hdr,
             size_t hdrlen,
             const void *pipe,
             size_t datlen,
             pami_endpoint_t origin,
             pami_recv_t *recv) {
	// assert that it is immediate recv...
	post_info_t *info = (post_info_t *)cookie;
	//assert(pipe == NULL && datlen == 0 && hdrlen == sizeof(info->seq));
	char buf[128];
	sprintf(buf, "do_recv(%d) by %ld on context %d: cookie = %p, %d\n",
                info->seq, pthread_self(), info->ctx, cookie,
                *((__typeof__(info->seq) *)hdr)
          );
	write(2, buf, strlen(buf));
	--info->value;
if (info->value == (unsigned)-1) fprintf(stderr, "fault\n");
}

pami_result_t run_test(pami_client_t client, pami_context_t *ctx, size_t nctx) {
	pami_result_t result;
	int x;

	for (x = 0; x < nctx; ++x) {
		_info[x].value = 1;
		_info[x].ctx = x;
		_info[x].client = client;
		_info[x].t0 = PAMI_Wtimebase(client);

		/* Post some work to the contexts */
		result = PAMI_Context_post(ctx[x], &_info[x].state, do_work, (void *)&_info[x]);
		if (result != PAMI_SUCCESS) {
			fprintf(stderr, "Error. Unable to post work to pami context[%d]. "
					"result = %d (%d)\n", x, result, errno);
			return result;
		}
	}

	const unsigned long long timeout = 500000; // abort after 10x of these, cycles
	unsigned long long t0, t1, t2;
	t0 = t1 = t2 = PAMI_Wtimebase(client);
	int busy;
	int stuck = 0;
	do {
		for (busy = 0, x = 0; x < nctx; ++x) busy += _info[x].value;
		if (!busy) break;

		// should complete without ever calling advance...
		t2 = PAMI_Wtimebase(client);
		if (t2 - t1 >= timeout) {
			static char buf[1024];
			char *s = buf;
			for (x = 0; x < nctx; ++x) {
				s += sprintf(s, " [%d]=%d", _info[x].seq, _info[x].value);
			}
			fprintf(stderr, "No progress after %lld cycles...? %s\n",
									t2 - t0, buf);
			// abort... ?
			if (++stuck > 10) return PAMI_ERROR;
			t1 = t2;
		}
	} while (busy);
	if (stuck > 0) {
		t2 = PAMI_Wtimebase(client);
		fprintf(stderr, "Unstuck after %lld cycles.\n", t2 - t0);
	}
	return PAMI_SUCCESS;
}

static size_t disp_id[NUM_CONTEXTS];

pami_result_t init_test_send(pami_client_t client, pami_context_t *ctx, size_t nctx) {
	int x;
	pami_dispatch_hint_t h = {0};
	test_init();
	for (x = 0; x < nctx; ++x) {
		disp_id[x] = 0;
		pami_result_t rc = PAMI_Dispatch_set(ctx[x], disp_id[x], (pami_dispatch_callback_function){do_recv}, (void *)&_info[x], h);
		if (rc != PAMI_SUCCESS) {
			fprintf(stderr, "Failed to set dispatch for context %d\n", x);
		}
	}
	return PAMI_SUCCESS;
}

pami_result_t run_test_send(pami_client_t client, pami_context_t *ctx, size_t nctx, int role) {
	pami_result_t result;
	int x;
	size_t ix;
	pami_send_hint_t h = {0};
	pami_configuration_t configuration;
	configuration.name = PAMI_CLIENT_TASK_ID;
	result = PAMI_Client_query(client, &configuration, 1);
	pami_task_t task = configuration.value.intval;

	// even index sends, odd receives...
	if (role) { // receiver...
		for (x = 0; x < nctx; ++x) {
			++_info[x].value; // expecting a receive ...
		}

	} else { // sender...

		for (x = 0; x < nctx; ++x) {

			// assert(_info[x].value == 0)
			_info[x].ctx = x;
			ix = task ^ 1;
			pami_task_t task = TEST_Global_index2task(ix);
			if (task == (pami_task_t)-1) continue; // never?

			size_t targ = x;

			++_info[x].value; // expecting send
			_info[x].send.send.header.iov_base = &_info[x].seq;
			_info[x].send.send.header.iov_len = sizeof(_info[x].seq);
			_info[x].send.send.data.iov_base = NULL;
			_info[x].send.send.data.iov_len = 0;
			_info[x].send.send.dispatch = disp_id[x];
			_info[x].send.send.hints = h;
			_info[x].send.events.cookie = (void *)&_info[x];
			_info[x].send.events.local_fn = done_send;
			_info[x].send.events.remote_fn = NULL;
			(void)PAMI_Endpoint_create(client, task, targ, &_info[x].send.send.dest);

//			if (PAMI_Context_trylock(ctx[x])) {
//				result = PAMI_Send(ctx[x], &_info[x].send);
//			} else {
				result = PAMI_Context_post(ctx[x], &_info[x].state, do_send, (void *)&_info[x]);
//			}
			if (result != PAMI_SUCCESS) {
				fprintf(stderr, "Error. Unable to post work to pami context[%d]. "
						"result = %d (%d)\n", x, result, errno);
				return result;
			}
		}
	}

	const unsigned long long timeout = 50000000; // abort after 10x of these, cycles
	unsigned long long t0, t1, t2;
	t0 = t1 = PAMI_Wtimebase(client);
	int busy;
	int stuck = 0;
	do {
		for (busy = 0, x = 0; x < nctx; ++x) busy += _info[x].value;
		if (!busy) break;

		// should complete without ever calling advance...
		t2 = PAMI_Wtimebase(client);
		if (t2 - t1 >= timeout) {
			static char buf[1024];
			char *s = buf;
			for (x = 0; x < nctx; ++x) {
				s += sprintf(s, " [%d]=%d", _info[x].seq, _info[x].value);
			}
			fprintf(stderr, "No progress after %lld cycles...? %s\n",
									t2 - t0, buf);
			// abort... ?
			if (++stuck > 10) return PAMI_ERROR;
			t1 = t2;
		}
	} while (busy);
	return PAMI_SUCCESS;
}

#endif // __test_internals_BG_bgq_commthreads_commthread_test_h__
