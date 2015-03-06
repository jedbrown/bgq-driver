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
 * \file test/internals/topology/topology_test.h
 * \brief Test the generic topology features
 */

#ifndef __test_internals_topology_topology_test_h__
#define __test_internals_topology_topology_test_h__

#include <pami.h>
#include "Topology.h"
#include "Global.h"

const char *topo_types[] = {
        "EMPTY",
        "SINGLE",
        "RANGE",
        "LIST",
        "COORD",
};

void dump(PAMI::Topology *topo, const char *title) {
        static char buf[4096];
        char *s = buf;
        pami_task_t f = 0, l = 0;
        pami_coord_t *ll, *ur;
        pami_result_t rc;
        size_t x;

        size_t z = topo->size();
        s += sprintf(s, "%s: %s (%zu) {", title, topo_types[topo->type()], z);
        switch (topo->type()) {
        case PAMI_EMPTY_TOPOLOGY:
                s += sprintf(s, "void");
                break;
        case PAMI_SINGLE_TOPOLOGY:
        case PAMI_LIST_TOPOLOGY:
                for (x = 0; x < z; ++x) {
                        s += sprintf(s, " %u", topo->index2Rank(x));
                }
                *s++ = ' ';
                break;
        case PAMI_RANGE_TOPOLOGY:
                topo->rankRange(&f, &l);
                s += sprintf(s, " %u..%u ", f, l);
                break;
        case PAMI_COORD_TOPOLOGY:
                rc = topo->rectSeg(&ll, &ur);
                if (rc) {
                        // not possible? we already know its COORD...
                } else {
                        s += sprintf(s, " (");
                        for (x = 0; x < __global.mapping.globalDims(); ++x) {
                                if (x) *s++ = ',';
                                s += sprintf(s, "%zu", ll->u.n_torus.coords[x]);
                        }
                        s += sprintf(s, ")..(");
                        for (x = 0; x < __global.mapping.globalDims(); ++x) {
                                if (x) *s++ = ',';
                                s += sprintf(s, "%zu", ur->u.n_torus.coords[x]);
                        }
                        s += sprintf(s, ") ");
                }
                break;
        default:
                break;
        }
        s += sprintf(s, "}\n");

        fprintf(stderr, buf);
}

#endif // __test_internals_topology_topology_test_h__
