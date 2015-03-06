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
 * \file test/internals/BG/bgq/classroutes/classroute_test.h
 * \brief ???
 */

#ifndef __test_internals_BG_bgq_classroutes_classroute_test_h__
#define __test_internals_BG_bgq_classroutes_classroute_test_h__

#include <spi/include/kernel/process.h>

#include <spi/include/kernel/collective.h>
#include "spi/include/mu/Classroute.h"
#undef __INLINE__
#define __INLINE__	static inline

#include <spi/include/kernel/location.h>
#include "spi/include/mu/Classroute_inlines.h"

#undef BUILD_CLASSROUTES

int sprint_coord(char *buf, CR_COORD_T *co) {
        char *s = buf;
        char c = '(';
        int x;
        for (x = 0; x < CR_NUM_DIMS; ++x) {
                s += sprintf(s, "%c%zd", c, CR_COORD_DIM(co,x));
                c = ',';
        }
        *s++ = ')';
        *s = '\0';
        return s - buf;
}

int sprint_rect(char *buf, CR_RECT_T *r) {
        char *s = buf;

        s += sprint_coord(s, CR_RECT_LL(r));
        *s++ = ':';
        s += sprint_coord(s, CR_RECT_UR(r));
        return s - buf;
}

int sprint_map(char *buf, int *map) {
	char *s = buf;
	int i;
	for (i = 0; i < CR_NUM_DIMS; ++i) {
		*s++ = CR_DIM_NAMES[map[i]];
	}
	*s = '\0';
	return (s - buf);
}

int coord2rank(int *map, CR_RECT_T *comm, CR_COORD_T *coord) {
	int rank = 0;
	int x, d;
	for (x = 0; x < CR_NUM_DIMS; ++x) {
		d = map[CR_NUM_DIMS - x - 1];
		rank *= (CR_COORD_DIM(CR_RECT_UR(comm),d) - CR_COORD_DIM(CR_RECT_LL(comm),d) + 1);
		rank += (CR_COORD_DIM(coord,d) - CR_COORD_DIM(CR_RECT_LL(comm),d));
	}
	return rank;
}

#ifdef BUILD_CLASSROUTES
static int is_inside_rect(CR_RECT_T *comm, CR_COORD_T *co) {
	int d;
	for (d = 0; d < CR_NUM_DIMS; ++d) {
		if (CR_COORD_DIM(co,d) < CR_COORD_DIM(CR_RECT_LL(comm),d)) return 0;
		if (CR_COORD_DIM(co,d) > CR_COORD_DIM(CR_RECT_UR(comm),d)) return 0;
	}
	return 1;
}

static void allreduce(int id, void *sbuf, void *rbuf, int count, pami_dt dt, pami_op op) {
}

void get_comm(int index, CR_RECT_T *world, CR_COORD_T *wexcl, int wnexcl,
				int *dims, int dim0,
				CR_RECT_T *comm, CR_COORD_T **excl, int *nexcl) {
	if (index == 0) {
		// Build this node's classroute for (dup of) comm-world:
		*comm = *world;
		*excl = wexcl;
		*nexcl = wnexcl;
	} else if (index == 1) {
		// split into multiple disjoint comms... ???
		// result: N disjoint comms (8?)
	} else if ((index & 1) != 0) {
		// split comm on specified dimension, if >= 2 in size
		// result: 2 disjoint comms
	} else {
		// trim comm by 1 on specified dimensions...
		// result: 1 comm excluding some nodes
	}
}
#endif // BUILD_CLASSROUTES

#endif // __test_BG_bgq_classroutes_classroute_test_h__
