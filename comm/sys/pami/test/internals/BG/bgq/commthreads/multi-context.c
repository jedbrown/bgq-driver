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
 * \file test/internals/BG/bgq/commthreads/multi-context.c
 * \brief Simple test for basic commthread functionality
 */

#include <assert.h>
#include "commthread_test.h"

#include "test/async_progress.h"

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

		result = run_test(client, context, NUM_CONTEXTS);
		if (result != PAMI_SUCCESS) {
			fprintf(stderr, "Error. Unable to run commthread test. "
							"result = %d\n", result);
			return 1;
		}

		if (y + 1 < NUM_TESTRUNS) {
			fprintf(stderr, "Sleeping...\n");
			do_sleep(client, buf, bufl, "Wa", 5);
		}
	}

	do_sleep(client, buf, bufl, "Fi", 5);

	result = PAMI_Context_destroyv(context, NUM_CONTEXTS);
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
