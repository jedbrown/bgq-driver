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
 * \file test/internals/pamihello.cc
 * \brief ???
 */

#include <stdio.h>
#include <pami.h>
#include "Global.h"

int main(int argc, char ** argv) {
        pami_client_t client;
        pami_context_t context;
        pami_result_t status = PAMI_ERROR;
        const char *name = "multicombine test";

        status = PAMI_Client_create(name, &client, NULL, 0);
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

        configuration.name = PAMI_CLIENT_NUM_TASKS;
        status = PAMI_Client_query(client, &configuration, 1);
        if (status != PAMI_SUCCESS) {
                fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, status);
                return 1;
        }
        size_t num_tasks = configuration.value.intval;

        char buf[256];
        char *s = buf;
        char p = '(';
        size_t d;
        pami_coord_t c;
        status = __global.mapping.task2network(task_id, &c, PAMI_N_TORUS_NETWORK);
        if (status == PAMI_SUCCESS) {
                for (d = 0; d < __global.mapping.globalDims(); ++d) {
                        s += sprintf(s, "%c%zu", p, c.u.n_torus.coords[d]);
                        p = ',';
                }
                *s++ = ')';
        }
        *s++ = '\0';
        fprintf(stderr, "Hello world from PAMI rank %zu of %zu %s\n", task_id, num_tasks, buf);

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
