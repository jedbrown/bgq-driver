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
/// \file test/internals/bgq/mu2/geom_classroutes.cc
/// \brief ???
///

#include <stdio.h>
#include <pami.h>

#include "Global.h"

static void g_done_fn(pami_context_t ctx, void *cookie, pami_result_t err) {
	int *done = (int *)cookie;
	if (err) {
		*done = -err;
	} else {
		*done = 1;
	}
}

// All members of 'parent' must call this (and no others).
// Assumes only one context.
void do_one_geometry(pami_client_t client, pami_task_t my_task,
		pami_geometry_range_t *range, pami_geometry_t parent,
		pami_context_t *contexts, size_t ncontexts) {

	static unsigned id = 300;
	pami_result_t status;
	pami_geometry_t geom = NULL, *gp;
        pami_configuration_t configuration;
	configuration.name = PAMI_GEOMETRY_OPTIMIZE;

	int g_done = 0;
	// hack to avoid barrier callbacks with NULL contexts...

	if (my_task < range->lo || my_task > range->hi) {
		gp = NULL;
	} else {
		gp = &geom;
	}
	// HACK: don't optimize in create to avoid callbacks with NULL context (Trac #240)
	status = PAMI_Geometry_create_taskrange(client,0, NULL, 0, gp,
				parent, id, range, 1, contexts[0], g_done_fn, &g_done);
	++id;
	if (status != PAMI_SUCCESS) {
		fprintf (stderr, "Error. Failed to create geom %d. result = %d\n",
									id, status);
		return;
	}
	while (!g_done) {
		PAMI_Context_advance(contexts[0], 100);
	}
	if (g_done < 0) {
		fprintf(stderr, "Failed to create geometry %u [%d..%d] %d\n",
						id, range->lo, range->hi, -g_done);
		return;
	}
	if (!geom) {
		// Not part of new geometry... we're done
		return;
	}
	g_done = 0;
	configuration.value.intval = 1;
	status = PAMI_Geometry_update(geom, &configuration, 1,
					contexts[0], g_done_fn, &g_done);
        if (status != PAMI_SUCCESS) {
	fprintf (stderr, "Error. Failed to optimize geom %d. result = %d\n", id, status);
		return;
	}
	while (!g_done) {
		PAMI_Context_advance(contexts[0], 100);
	}
	if (g_done < 0) {
		fprintf(stderr, "Failed to optimize geometry %u [%d..%d] %d\n",
						id, range->lo, range->hi, -g_done);
		return;
	}
	status = PAMI_Geometry_query(geom, &configuration, 1);
	if (status != PAMI_SUCCESS) {
		fprintf (stderr, "Error. Unable query to geom %d configuration (%d). "
				"result = %d\n", id, configuration.name, status);
		return;
	}
	if (my_task == range->lo) {
		printf("Geometry %d [%d..%d] is%s optimized (%zx)\n",
					id, range->lo, range->hi,
					configuration.value.intval ? "" : " not",
					configuration.value.intval);
	}
}

int main(int argc, char ** argv) {
        pami_context_t context;
        size_t task_id;
        size_t num_tasks;

        pami_client_t client;
        pami_result_t status = PAMI_ERROR;
        status = PAMI_Client_create("TEST", &client, NULL, 0);
        if (status != PAMI_SUCCESS) {
                fprintf (stderr, "Error. Unable to initialize pami client. result = %d\n", status);
                return 1;
        }

        status = PAMI_Context_createv(client, NULL, 0, &context, 1);
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
        task_id = configuration.value.intval;
        //fprintf(stderr, "My task id = %zu\n", task_id);

        configuration.name = PAMI_CLIENT_NUM_TASKS;
        status = PAMI_Client_query(client, &configuration, 1);
        if (status != PAMI_SUCCESS) {
                fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, status);
                return 1;
        }
        num_tasks = configuration.value.intval;
        if (task_id == 0) fprintf(stderr, "Number of tasks = %zu\n", num_tasks);


// END standard setup
// ------------------------------------------------------------------------

	pami_geometry_t world_geometry;
	status = PAMI_Geometry_world(client, &world_geometry);

	configuration.name = PAMI_GEOMETRY_OPTIMIZE;
        status = PAMI_Geometry_query(world_geometry, &configuration, 1);
        if (status != PAMI_SUCCESS) {
                fprintf (stderr, "Error. Unable query to geom configuration (%d). result = %d\n", configuration.name, status);
                return 1;
        }
	if (task_id == 0) {
		printf("World geometry is%s optimized (%zx)\n",
			configuration.value.intval ? "" : " not",
			configuration.value.intval);
	}

	pami_geometry_range_t range;

	size_t ntasks = num_tasks;
	while (ntasks > 1) {
		range.lo = 0;
		range.hi = ntasks - 1;
		do_one_geometry(client, task_id, &range, world_geometry, &context, 1);
		if (ntasks <= num_tasks / 2) {
			range.lo = num_tasks - ntasks;
			range.hi = num_tasks - 1;
			do_one_geometry(client, task_id, &range, world_geometry, &context, 1);
		}

		ntasks /= 2;
	}

// ------------------------------------------------------------------------
        status = PAMI_Context_destroyv(&context, 1);
        if (status != PAMI_SUCCESS) {
                fprintf(stderr, "Error. Unable to finalize pami contexts. result = %d\n", status);
                return 1;
        }
        status = PAMI_Client_destroy(&client);
        if (status != PAMI_SUCCESS) {
                fprintf(stderr, "Error. Unable to finalize pami client. result = %d\n", status);
                return 1;
        }

        return 0;
}
