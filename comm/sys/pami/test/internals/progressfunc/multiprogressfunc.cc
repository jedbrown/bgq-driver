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
 * \file test/internals/progressfunc/multiprogressfunc.cc
 * \brief Test the generic progress function feature with multiple functions
 */

#include <stdio.h>
#include <pami.h>
//#include "Client.h"
#include "Global.h"
#include "components/devices/misc/ProgressFunctionMsg.h"

#ifndef NUM_MULTI
#define NUM_MULTI	10
#endif // NUM_MULTI

PAMI::Device::ProgressFunctionMdl _model;

char msgbuf[NUM_MULTI][PAMI::Device::ProgressFunctionMdl::sizeof_msg];

struct my_work {
        unsigned long long count;
        unsigned long long t0;
        unsigned long v;
        unsigned long u;
};

unsigned done = 0;

static pami_result_t my_func(pami_context_t ctx, void *cd) {
        struct my_work *w = (struct my_work *)cd;
        if (w->t0 == 0) {
                w->t0 = __global.time.timebase();
//		fprintf(stderr, "Starting %lu work at tick %llu, waiting until %llu\n",
//			w->u, w->t0, w->t0 + w->count);
                w->v = 0;
        }
        unsigned long long t1 = __global.time.timebase();
        if (t1 - w->t0 >= w->count) {
                fprintf(stderr, "Finished %lu at tick %llu (%ld calls), started at %llu\n", w->u, t1, w->v, w->t0);
                ++done;
                return PAMI_SUCCESS;
        }
        ++w->v;
        return PAMI_EAGAIN;
}

struct my_work work[NUM_MULTI];

int main(int argc, char **argv) {
        unsigned x;
        pami_client_t client;
        pami_context_t context;
        pami_result_t status = PAMI_ERROR;

        status = PAMI_Client_create("multiprogressfunc test", &client, NULL, 0);
        if (status != PAMI_SUCCESS) {
                fprintf (stderr, "Error. Unable to initialize pami client. result = %d\n", status);
                return 1;
        }

        {  status = PAMI_Context_createv(client, NULL, 0, &context, 1); }
        if (status != PAMI_SUCCESS) {
                fprintf (stderr, "Error. Unable to create pami context. result = %d\n", status);
                return 1;
        }

        pami_configuration_t configuration;

        configuration.name = PAMI_CLIENT_TASK_ID;
        status = PAMI_Client_query(client, &configuration, 1);
        if (status != PAMI_SUCCESS) {
                fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, status);
                return 1;
        }
        size_t task_id = configuration.value.intval;
        //fprintf(stderr, "My task id = %zu\n", task_id);

        configuration.name = PAMI_CLIENT_NUM_TASKS;
        status = PAMI_Client_query(client, &configuration, 1);
        if (status != PAMI_SUCCESS) {
                fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, status);
                return 1;
        }
        size_t num_tasks = configuration.value.intval;
        if (task_id == 0) fprintf(stderr, "Number of tasks = %zu\n", num_tasks);

// END standard setup
// ------------------------------------------------------------------------
        PAMI_ProgressFunc_t pf;
        unsigned long long count;

        if (argc > 1) {
                count = strtoull(argv[1], NULL, 0);
        } else {
                count = 300000ULL;	// the printf takes time, ensure we don't
        }				// finish too soon.

        for (x = 0; x < NUM_MULTI; ++x) {
                work[x].t0 = 0;
                work[x].count = count;
                work[x].u = x + 1;
                pf.client = 0;
                pf.context = 0;
                pf.request = &msgbuf[x];
                pf.func = my_func;
                pf.clientdata = &work[x];
                bool rc = _model.postWork(&pf);
                if (!rc) {
                        fprintf(stderr, "Failed to generateMessage on progress function\n");
                        exit(1);
                }
        }
        while (done < NUM_MULTI) {
                PAMI_Context_advance(context, 100);
        }
        fprintf(stderr, "Test completed\n");

// ------------------------------------------------------------------------

        status = PAMI_Context_destroyv(&context, 1);
        if (status != PAMI_SUCCESS) {
                fprintf(stderr, "Error. Unable to destroy pami context. result = %d\n", status);
                return 1;
        }

        status = PAMI_Client_destroy(&client);
        if (status != PAMI_SUCCESS) {
                fprintf(stderr, "Error. Unable to finalize pami client. result = %d\n", status);
                return 1;
        }

        return 0;
}
