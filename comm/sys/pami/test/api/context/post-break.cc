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
 * \file test/api/context/post-break.cc
 * \brief ???
 */

#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>

#include <pami.h>

#ifndef MAX_PTHREADS
#define MAX_PTHREADS    8
#endif // MAX_PTHREADS

#define MAX_NCONTEXTS	16

template <int T_RandMask = 0, int T_BackoffNS = 0>
class QueueTest {
public:
	static const size_t num_ctx = 1;

	typedef struct {
		pami_work_t state;
		QueueTest *thus;
		int run_count;
	} element_t;

	typedef struct {
		int tid;
		QueueTest *thus;
		element_t *elem_mem;
	} threadinfo_t;

	double base_t;
	const char *name;
	int pthreads;
	int elements;
	pami_context_t *contexts;
	int seed;
	int done;
	void *elem_mem[MAX_PTHREADS];

	QueueTest(const char *n, int pth, int elem, int s, pami_context_t *ctxs) :
	name(n),
	pthreads(pth),
	elements(elem),
	contexts(ctxs),
	seed(s)
	{
		size_t x;
		unsigned long long t1, t0 = PAMI_Wtimebase();
		for (x = 0; x < 10000; ++x) {
			t1 = PAMI_Wtimebase() - t0;
		}
		t1 = PAMI_Wtimebase() - t0;
		double d = t1;
		base_t = d / x;
	}

	// This is executed in the (single) dequeuer() thread so does
	// not require mutex nor atomic opertions.
	static pami_result_t work(pami_context_t ctx, void *cookie) {
		element_t *e = (element_t *)cookie;
		QueueTest *thus = e->thus;
		++thus->done;
		if (e->run_count-- <= 0) {
			memset(&e->state, 0xff, sizeof(e->state));
			return PAMI_SUCCESS;
		} else {
			return PAMI_EAGAIN;
		}
	}

	static void *enqueuers(void *v) {
		threadinfo_t *ti = (threadinfo_t *)v;
		QueueTest *thus = ti->thus;
		int num = thus->elements;
		int x;
		element_t *e;
		unsigned long long t0, t = 0;
		timespec tv = {0, T_BackoffNS};
		ti->elem_mem = (element_t *)malloc(num * sizeof(*e));
		assert(ti->elem_mem);

		fprintf(stderr, "%d: starting %d enqueues\n", ti->tid, num);
		for (x = 0; x < num; ++x) {
			if (T_BackoffNS) nanosleep(&tv, NULL);
			e = &ti->elem_mem[x];
			e->thus = thus;
			e->run_count = (rand() & T_RandMask);
			t0 = PAMI_Wtimebase();
			PAMI_Context_post(thus->contexts[0], &e->state, work, e);
			t += PAMI_Wtimebase() - t0;
		}
		double d = t;
		fprintf(stderr, "%d: finished %d enqueues (%g cycles each)\n",
				ti->tid, num, (d / num) - thus->base_t);
		return NULL;
	}

	static void *dequeuer(void *v) {
		threadinfo_t *ti = (threadinfo_t *)v;
		QueueTest *thus = ti->thus;
		int num = thus->elements * thus->num_ctx * (thus->pthreads - 1);
		size_t x;
		unsigned long long t0, t = 0;
		int wkcnt = 0;

		fprintf(stderr, "%d: looking for %d dequeues\n", ti->tid, num);
		while (thus->done < num) {
			int did = thus->done;
			t0 = PAMI_Wtimebase();
			for (x = 0; x < thus->num_ctx; ++x) {
				PAMI_Context_advance(thus->contexts[x], 1);
			}
			if (did != thus->done) {
				t += PAMI_Wtimebase() - t0;
				++wkcnt;
			}
		}

		double d = t;
		fprintf(stderr, "%d: finished %d dequeues (%g cycles ea, %d calls)\n",
				ti->tid, num, d / num, wkcnt);
		return NULL;
	}

	pthread_attr_t attr[MAX_PTHREADS];
	pthread_t thread[MAX_PTHREADS];
	threadinfo_t threadinfo[MAX_PTHREADS];

	int run_test(void) {
		int x;
		fprintf(stderr, "main: starting %s test with %d elements per run, %d pthreads enqueueing\n",
				name, elements, pthreads);
		done = 0;
		int status;
		for (x = 1; x < pthreads; ++x) {
			pthread_attr_init(&attr[x]);
			pthread_attr_setscope(&attr[x], PTHREAD_SCOPE_SYSTEM);
			threadinfo[x].tid = x;
			threadinfo[x].thus = this;
			threadinfo[x].elem_mem = NULL;
			status = pthread_create(&thread[x], &attr[x],
						enqueuers, (void *)&threadinfo[x]);
			/* don't care about status? just reap threads below? */
		}
		threadinfo[0].tid = 0;
		threadinfo[0].thus = this;
		threadinfo[0].elem_mem = NULL;
		(void)dequeuer((void *)&threadinfo[0]);
		for (x = 1; x < pthreads; ++x) {
			pthread_join(thread[x], NULL);
			if (threadinfo[x].elem_mem) {
				free(threadinfo[x].elem_mem);
			}
		}
		fprintf(stderr, "test %s main done. (status unknown)\n",
			name);
		return 0;
	}

}; // class QueueTest

int main(int argc, char **argv) {
	int x;
	int pthreads = 4;
	int elements = 100000;
	int seed = 1;

	//extern int optind;
	extern char *optarg;

	while ((x = getopt(argc, argv, "e:p:s:")) != EOF) {
		switch(x) {
		case 'e':
			elements = strtol(optarg, NULL, 0);
			break;
		case 'p':
			pthreads = strtol(optarg, NULL, 0);
			if (pthreads > MAX_PTHREADS) {
				fprintf(stderr, "Max num pthreads is compiled as %d\n", MAX_PTHREADS);
				exit(1);
			}
			break;
		case 's':
			seed = strtol(optarg, NULL, 0);
			srand(seed);
			break;
		}
	}

	pami_client_t client;
	pami_context_t contexts[MAX_NCONTEXTS];
	pami_result_t rc;

	rc = PAMI_Client_create("TEST", &client, NULL, 0);
	assert(rc == PAMI_SUCCESS);
	rc = PAMI_Context_createv(client, NULL, 0, contexts, QueueTest<7>::num_ctx);
	assert(rc == PAMI_SUCCESS);

	int ret = 0;
	QueueTest<7> test1("default", pthreads, elements, seed, contexts);
	ret = test1.run_test();

	rc = PAMI_Context_destroyv(contexts, QueueTest<7>::num_ctx);
	assert(rc == PAMI_SUCCESS);
	rc = PAMI_Client_destroy(&client);
	assert(rc == PAMI_SUCCESS);

	exit(ret);
}
