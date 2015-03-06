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
 *  \file test/api/context/advance-perf.c
 *  \brief Simple performance test of empty advance (no communications active).
 */

#include <pami.h>
#include <stdio.h>
#include <assert.h>

#define MAX_NCONTEXTS	16
#define NITER		10000

int main(int argc, char **argv) {
        pami_client_t client; 
        pami_context_t contexts[MAX_NCONTEXTS];
	size_t ncontexts = 1;
        pami_result_t rc;
	int x, y;

        rc = PAMI_Client_create("TEST", &client, NULL, 0);
        assert(rc == PAMI_SUCCESS);
        rc = PAMI_Context_createv(client, NULL, 0, contexts, ncontexts);
        assert(rc == PAMI_SUCCESS);

	unsigned long long t0 = PAMI_Wtimebase(client);
	for (x = 0; x < NITER; ++x) {
		for (y = 0; y < ncontexts; ++y) {
			PAMI_Context_advance(contexts[y], 1);
		}
	}
	double d = PAMI_Wtimebase(client) - t0;
	fprintf(stderr, "Empty Advance %d times, %zd contexts, %g cycles each\n",
			NITER, ncontexts, d / (NITER * ncontexts));

        rc = PAMI_Context_destroyv(contexts, 1);
        assert(rc == PAMI_SUCCESS);
        rc = PAMI_Client_destroy(&client);
        assert(rc == PAMI_SUCCESS);

	exit(0);
}
