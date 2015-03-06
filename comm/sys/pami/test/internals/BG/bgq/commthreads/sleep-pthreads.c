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
 * \file test/internals/BG/bgq/commthreads/sleep-pthreads.c
 * \brief Simple test for sleeping commthread functionality
 *
 * Run with PAMI_COMMTHREAD_SLEEP=10u and --ranks-per-node 1
 */

#include "commthread_test.h"

#ifdef __pami_target_bgq__
#include "spi/include/kernel/process.h"
#include "spi/include/kernel/location.h"
#endif

#include <assert.h>
#include "test/async_progress.h"

volatile int run = 0;
struct thread_data {
	pami_context_t context;
	pthread_mutex_t mutex;
	pthread_t thread;
	volatile int active;
};

void *user_pthread(void *cookie) {
	struct thread_data *dat = (struct thread_data *)cookie;
	int lrc;
	int last = 0;
	char buf[64];
	sprintf(buf, "Ra %ld\n", pthread_self());
	int bufl = strlen(buf);
	write(2, buf, bufl);
	while (run) {
		while (run && dat->active) {
			if (last == 0) {
				buf[0] = 'T'; buf[1] = 'o';
				write(2, buf, bufl);
				last = 1;
			}
			// just spin...
		}
		last = 0;
		buf[0] = 'G'; buf[1] = 'b';
		write(2, buf, bufl);
		lrc = pthread_mutex_lock(&dat->mutex);
		// now, active == 1 || run == 0
		pthread_mutex_unlock(&dat->mutex);
	}
	buf[0] = 'A'; buf[1] = 'd';
	write(2, buf, bufl);
	return NULL;
}

struct thread_data thr_data[NUM_CONTEXTS];

int main(int argc, char ** argv) {
	pami_client_t client;
	char cl_string[] = "TEST";
	pami_result_t result = PAMI_ERROR;
	pami_context_t context[NUM_CONTEXTS];
	int x, y;
	char buf[64];
	sprintf(buf, "St %ld\n", pthread_self());
	int bufl = strlen(buf);

	result = PAMI_Client_create(cl_string, &client, NULL, 0);
	if (result != PAMI_SUCCESS) {
		fprintf(stderr, "Error. Unable to initialize pami client. "
						"result = %d\n", result);
		return 1;
	}
	pami_configuration_t configuration;
	configuration.name = PAMI_CLIENT_NUM_TASKS;
	result = PAMI_Client_query(client, &configuration,1);
	pami_task_t ntasks = configuration.value.intval;
	if (ntasks != 1) {
		fprintf(stderr, "Must be run with one task\n");
		exit(1);
	}

	result = init_async_prog();
	assert(result == PAMI_SUCCESS);

	result = PAMI_Context_createv(client, NULL, 0, &context[0], NUM_CONTEXTS);
	if (result != PAMI_SUCCESS) {
		fprintf(stderr, "Error. Unable to create %d pami context. "
						"result = %d\n", NUM_CONTEXTS, result);
		return 1;
	}
	test_init();
	write(2, buf, bufl);
	for (x = 0; x < NUM_CONTEXTS; ++x) {
		result = async_prog_enable(context[x], PAMI_ASYNC_ALL);
		if (result != PAMI_SUCCESS) {
			fprintf(stderr, "Error. Unable to add commthread to context[%d]. "
							"result = %d (%d)\n", x, result, errno);
			return 1;
		}
	}
	for (y = 0; y < NUM_TESTRUNS; ++y) {
		for (x = 0; x < NUM_CONTEXTS; ++x) {
			_info[x].seq = y * NUM_CONTEXTS + x + 1;
		}

		if (y & 1) {
			if (run) {
				if (y & 2) {
					for (x = 0; x < NUM_CONTEXTS; ++x) {
						thr_data[x].active = 1;
						pthread_mutex_unlock(&thr_data[x].mutex);
					}
				} else {
					for (x = 0; x < NUM_CONTEXTS; ++x) {
						// we shouldn't wait at all...
						pthread_mutex_lock(&thr_data[x].mutex);
						thr_data[x].active = 0;
					}
				}
			} else {
				run = 1;
				for (x = 0; x < NUM_CONTEXTS; ++x) {
					pthread_attr_t attr;
					thr_data[x].context = context[x];
					thr_data[x].active = 0;
					pthread_mutex_init(&thr_data[x].mutex, NULL);
					pthread_mutex_lock(&thr_data[x].mutex);
					pthread_attr_init(&attr);
#ifdef __pami_target_bgq__
					// need better way to predict commthread assignment.
					// this works for simple cases.
					int p = Kernel_ProcessorCount();// 1..64
					int i = Kernel_ProcessorID();	// 0..63
					i = (i + p - 1) - x * 4;
					cpu_set_t cpu_mask;
					CPU_ZERO(&cpu_mask);
					CPU_SET(i, &cpu_mask);
					pthread_attr_setaffinity_np(&attr,
																			sizeof(cpu_set_t),
																			&cpu_mask);
#endif /* __pami_target_bgq__ */
					int rc = pthread_create(&thr_data[x].thread, &attr,
							user_pthread, (void *)&thr_data[x]);
					if (rc == -1) perror("pthread_create");
#ifdef __pami_target_bgq__
fprintf(stderr, "++ %ld %d:%d\n", thr_data[x].thread, i / 4, i % 4);
#endif /* __pami_target_bgq__ */
					pthread_attr_destroy(&attr);
				}
			}
		}

		result = run_test(client, context, NUM_CONTEXTS);
		if (result != PAMI_SUCCESS) {
			fprintf(stderr, "Error. Unable to run commthread test. "
							"result = %d\n", result);
			//return 1;
			break;
		}

		if (y + 1 < NUM_TESTRUNS) {
			fprintf(stderr, "Sleeping...\n");
			do_sleep(client, buf, bufl, "Wa", 5);
		}
	}
	run = 0;

	do_sleep(client, buf, bufl, "Fi", 5);

	for (x = 0; x < NUM_CONTEXTS; ++x) {
		pthread_mutex_unlock(&thr_data[x].mutex);
	}
	result = PAMI_Context_destroyv(context, NUM_CONTEXTS);
	buf[0] = 'D'; buf[1] = 'v';
	write(2, buf, bufl);
	for (x = 0; x < NUM_CONTEXTS; ++x) {
		pthread_join(thr_data[x].thread, NULL);
		fprintf(stderr, "Jo %ld\n", thr_data[x].thread);
	}
	result = PAMI_Client_destroy(&client);
	if (result != PAMI_SUCCESS) {
		fprintf(stderr, "Error. Unable to finalize pami client. "
						"result = %d\n", result);
		return 1;
	}

	buf[0] = 'S'; buf[1] = 'u';
	write(2, buf, bufl);
	fprintf(stderr, "Success.\n");

	return 0;
}
