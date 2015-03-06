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
 * \file test/internals/queue/gendev_stress.cc
 * \brief ???
 */

//#define _POSIX_C_SOURCE 199309
#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#ifndef MAX_PTHREADS
#define MAX_PTHREADS    8
#endif // MAX_PTHREADS

#include "gendev_setup.h"

template <int T_RandMask = 0, int T_BackoffNS = 0>
class QueueTest {
	static const size_t num_ctx = 1;
public:

	PAMI::Memory::GenMemoryManager mm;

	PAMI::Device::Generic::Device *queue;
	typedef PAMI::Device::Generic::GenericThread element_t;

	double base_t;
	const char *name;
	int pthreads;
	int elements;
	int seed;
	int done;

	QueueTest(const char *n, int pth, int elem, int s) :
	name(n),
	pthreads(pth),
	elements(elem),
	seed(s)
	{
		size_t size = 32*1024;
		mm.init(__global.heap_mm, size);

		size_t x;
		unsigned long long t1, t0 = PAMI_Wtimebase();
		for (x = 0; x < 10000; ++x) {
			t1 = PAMI_Wtimebase() - t0;
		}
		t1 = PAMI_Wtimebase() - t0;
		double d = t1;
		base_t = d / x;

		queue = PAMI::Device::Generic::Device::Factory::generate(0, num_ctx, mm, NULL);
		for (x = 0; x < num_ctx; ++x) {
			PAMI::Device::Generic::Device::Factory::init(queue, 0, x,
							(pami_client_t)(x+1),
							(pami_context_t)(x*2+1),
							&mm, queue);
		}
	}

	static pami_result_t work(pami_context_t ctx, void *cookie) {
		QueueTest *thus = (QueueTest *)cookie;
		if ((rand() & T_RandMask) == 0) {
			++thus->done;
			return PAMI_SUCCESS;
		} else {
			return PAMI_EAGAIN;
		}
	}

	static void *enqueuers(void *v) {
		QueueTest *thus = (QueueTest *)v;
		int num = thus->elements;
		int x;
		element_t *e;
		unsigned long long t0, t = 0;
		// timespec tv = {0, T_BackoffNS};

		fprintf(stderr, "%d: starting %d enqueues\n", gettid(), num);
		for (x = 0; x < num; ++x) {
			// if (T_BackoffNS) nanosleep(&tv, NULL);
			e = (element_t *)malloc(sizeof(*e));
			assert(e);
			e->setStatus(PAMI::Device::Ready);
			e->setFunc(work, thus);

			PAMI::Device::Generic::Device *gd =
				&PAMI::Device::Generic::Device::Factory::getDevice(thus->queue, 0, 0);
			t0 = PAMI_Wtimebase();
			gd->postThread(e);
			t += PAMI_Wtimebase() - t0;
		}
		double d = t;
		fprintf(stderr, "%d: finished %d enqueues (%g cycles each)\n",
				gettid(), num, (d / num) - thus->base_t);
		return NULL;
}

	static void *dequeuer(void *v) {
		QueueTest *thus = (QueueTest *)v;
		int num = thus->elements * thus->num_ctx * (thus->pthreads - 1);
		size_t x;
		unsigned long long t0, t = 0;

		fprintf(stderr, "%d: looking for %d dequeues\n", gettid(), num);
		t0 = PAMI_Wtimebase();
		while (thus->done < num) {
			for (x = 0; x < thus->num_ctx; ++x) {
				PAMI::Device::Generic::Device::Factory::advance(thus->queue, 0, x);
			}
		}
		t += PAMI_Wtimebase() - t0;

		double d = t;
		fprintf(stderr, "%d: finished %d dequeues (%g cycles ea)\n",
				gettid(), num, d / num);
		return NULL;
	}

	pthread_attr_t attr[MAX_PTHREADS];
	pthread_t thread[MAX_PTHREADS];

	int run_test(void) {
		int x;
		fprintf(stderr, "main: starting %s test with %d elements per run, %d pthreads enqueueing\n",
				name, elements, pthreads);
		done = 0;
		int status;
		for (x = 1; x < pthreads; ++x) {
			pthread_attr_init(&attr[x]);
			pthread_attr_setscope(&attr[x], PTHREAD_SCOPE_SYSTEM);
			status = pthread_create(&thread[x], &attr[x],
						enqueuers, (void *)this);
			/* don't care about status? just reap threads below? */
		}
		(void)dequeuer((void *)this);
		for (x = 1; x < pthreads; ++x) {
			pthread_join(thread[x], NULL);
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
	int ret = 0;
	QueueTest<3> test1(QUEUE_NAME, pthreads, elements, seed);
	ret = test1.run_test();
	exit(ret);
}
