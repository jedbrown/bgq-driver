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
 * \file test/internals/topology/topology_index.cc
 * \brief Test the generic topology features
 */

#include "topology_test.h"

void check_index2rank(PAMI::Topology *topo, const char *str) {
        size_t x;
        for (x = 0; x < topo->size(); ++x) {
                pami_task_t r = topo->index2Rank(x);
                size_t i = topo->rank2Index(r);
                pami_coord_t c;
                __global.mapping.task2network(r, &c, PAMI_N_TORUS_NETWORK);
                static char buf[1024];
                char *s = buf;
                char comma = '(';
                for (size_t y = 0; y < __global.mapping.globalDims(); ++y) {
                        s += sprintf(s, "%c%zu", comma, c.u.n_torus.coords[y]);
                        comma = ',';
                }
                *s++ = ')';
                *s++ = '\0';
                fprintf(stderr, "%s.index2Rank(%zu) => %d => %zu %s\n", str, x, r, i, buf);
        }
}

int main(int argc, char **argv) {
        unsigned x;
        pami_client_t client;
        pami_context_t context;
        pami_result_t status = PAMI_ERROR;

        status = PAMI_Client_create("topology_index test", &client, NULL, 0);
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
        pami_task_t *ranks = (pami_task_t *)malloc(num_tasks * sizeof(*ranks));

        dump(&__global.topology_global, "world");
        check_index2rank(&__global.topology_global, "topology_global");

        for (x = 0; x < num_tasks; ++x) {
                ranks[x] = x;
        }
        PAMI::Topology topo_list;
        new (&topo_list) PAMI::Topology(ranks, num_tasks);
        if (topo_list.convertTopology(PAMI_COORD_TOPOLOGY)) {
                dump(&topo_list, "topo_list(coords)");
                check_index2rank(&topo_list, "topo_list(coords)");
        } else {
                fprintf(stderr, "Failed to convert topo_list to coords.\n");
        }

        if (__global.topology_global.type() == PAMI_COORD_TOPOLOGY) {
                memcpy(&topo_list, &__global.topology_global, sizeof(PAMI::Topology));
                if (topo_list.convertTopology(PAMI_LIST_TOPOLOGY)) {
                        dump(&topo_list, "topo_list(list)");
                        check_index2rank(&topo_list, "topo_list(list)");
                } else {
                        fprintf(stderr, "Failed to convert topo_list to list.\n");
                }
        }

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
