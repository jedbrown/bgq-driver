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
 *  \file test/internals/BG/bgq/classroutes/classroute_test.c
 * \brief Simple test for basic commthread functionality
 */

#include "classroute_test.h"

int main(int argc, char **argv) {
	CR_RECT_T refcomm;
	CR_COORD_T me;
	static char buf[4096];
	char *s;
	Personality_t pers;

	int rc = Kernel_GetPersonality(&pers, sizeof(pers));
	if (rc) {
		perror("Kernel_GetPersonality()");
		exit(1);
	}
#if 0 /* not currently needed? */
fprintf(stderr, "pers: (%d,%d,%d,%d,%d)\n",
pers.Network_Config.Anodes,
pers.Network_Config.Bnodes,
pers.Network_Config.Cnodes,
pers.Network_Config.Dnodes,
pers.Network_Config.Enodes);
	if (!pers.Network_Config.Anodes) pers.Network_Config.Anodes = 1;
	if (!pers.Network_Config.Bnodes) pers.Network_Config.Bnodes = 1;
	if (!pers.Network_Config.Cnodes) pers.Network_Config.Cnodes = 1;
	if (!pers.Network_Config.Dnodes) pers.Network_Config.Dnodes = 1;
	if (!pers.Network_Config.Enodes) pers.Network_Config.Enodes = 1;
#endif

	// initialize entire-block rectange.
	// Note: only a subset of these nodes may actually be running.
	*CR_RECT_LL(&refcomm) = CR_COORD_INIT(0,0,0,0,0);
	*CR_RECT_UR(&refcomm) = CR_COORD_INIT(
		pers.Network_Config.Anodes - 1,
		pers.Network_Config.Bnodes - 1,
		pers.Network_Config.Cnodes - 1,
		pers.Network_Config.Dnodes - 1,
		pers.Network_Config.Enodes - 1
	);

	// get this node's coordinates:
	me = CR_COORD_INIT(
		pers.Network_Config.Acoord,
		pers.Network_Config.Bcoord,
		pers.Network_Config.Ccoord,
		pers.Network_Config.Dcoord,
		pers.Network_Config.Ecoord
	);

	CR_RECT_T communiv;
	CR_COORD_T refroot;
	int pri_dim;
	int is_subblockjob;

	// Pick a reasonable root node for comm-world:
	// TBD: based on booted block or sub-block job?
	// \todo Get dimension-order mapping instead of NULL.
	MUSPI_PickWorldRoot(&refcomm, NULL, &refroot, &pri_dim);

	size_t np = (size_t)-1;
	size_t vn = 1;
	s = getenv("BG_NP");
	if (s) {
		np = strtoul(s, NULL, 0);
	}
	s = getenv("BG_PROCESSESPERNODE");
	if (s) {
		vn = strtoul(s, NULL, 0);
	}
	int map[CR_NUM_DIMS];
	s = getenv("BG_MAPFILE");
	int i = 0;
	if (s) {
		// if [[ ${s} == +([A-ET]) ]]
		// if (strlen(s) == CR_NUM_DIMS + 1)
		char *x, *m = CR_DIM_NAMES;
		for (; *s; ++s) {
			// do we have to worry about where T is in map?
			if (*s == 'T') continue;
			x = strchr(m, *s);
			if (!x) break;
			map[i++] = (x - m);
		}
	}
	if (i != CR_NUM_DIMS) {
		// either bogus BG_MAPPING or none at all...
		// Use simple 1:1 mapping
		for (i = 0; i < CR_NUM_DIMS; ++i) map[i] = i;
	}

	BG_JobCoords_t subblk;
	is_subblockjob = Kernel_JobCoords(&subblk);
	if (is_subblockjob) { /* no sub-block == use entire block */
		if (subblk.shape.core < 16) { /* sub-node job... not supported */
			fprintf(stderr, "Sub-node jobs are not supported\n");
			exit(1);
		}
	}
#if 0 /* not currently needed? */
fprintf(stderr, "subblk: (%d,%d,%d,%d,%d)\n",
subblk.shape.a,
subblk.shape.b,
subblk.shape.c,
subblk.shape.d,
subblk.shape.e);
	if (!subblk.shape.a) subblk.shape.a = 1;
	if (!subblk.shape.b) subblk.shape.b = 1;
	if (!subblk.shape.c) subblk.shape.c = 1;
	if (!subblk.shape.d) subblk.shape.d = 1;
	if (!subblk.shape.e) subblk.shape.e = 1;
#endif

	*CR_RECT_LL(&communiv) = CR_COORD_INIT(
		subblk.corner.a,
		subblk.corner.b,
		subblk.corner.c,
		subblk.corner.d,
		subblk.corner.e
	);
	*CR_RECT_UR(&communiv) = CR_COORD_INIT(
		subblk.corner.a + subblk.shape.a - 1,
		subblk.corner.b + subblk.shape.b - 1,
		subblk.corner.c + subblk.shape.c - 1,
		subblk.corner.d + subblk.shape.d - 1,
		subblk.corner.e + subblk.shape.e - 1
	);
	// Note: still may not be running every node in rectangle...
	int rank = coord2rank(map, &communiv, &me);
	int univz = __MUSPI_rect_size(&communiv);
	if (np != (size_t)-1) {
		np = (np + vn - 1) / vn;
	} else {
		np = univz;
	}

	CR_RECT_T commworld;
	CR_COORD_T *excluded = NULL;
	int nexcl = 0;
	if (np < univz) {
		excluded = malloc((univz - np) * sizeof(CR_COORD_T));
		// assert(excluded != NULL);

		// \todo really discard previous pri_dim here?
		MUSPI_MakeNpRectMap(&communiv, np, map,
				&commworld, excluded, &nexcl, &pri_dim);
	} else {
		commworld = communiv;
	}

	// if (verbose) {...
	if (rank == 0) {
		s = buf;
		s += sprintf(s, "Booted block = ");
		s += sprint_rect(s, &refcomm);
		fprintf(stderr, "%s\n", buf);

		s = buf;
		s += sprintf(s, "Job -np %zd circumscribing rectangle = ", np);
		s += sprint_rect(s, &communiv);
		fprintf(stderr, "%s\n", buf);

		s = buf;
		s += sprintf(s, "Comm-world = ");
		s += sprint_rect(s, &commworld);
		if (nexcl > 0) {
			s += sprintf(s, "\n\texcluding\t");
			for (i = 0; i < nexcl; ++i) {
				if (i) {
					s += sprintf(s, "\n\t\t\t");
				}
				s += sprint_coord(s, &excluded[i]);
			}
		}
		fprintf(stderr, "%s\n", buf);
	}
	s = buf;
	s += sprintf(s, "I am %d ", rank);
	s += sprint_coord(s, &me);
	s += sprintf(s, " using mapping ");
	s += sprint_map(s, map);
{ char *_s = getenv("BG_SUB_BLOCK_SHAPE");
sprintf(s, "\nBG_SUB_BLOCK_SHAPE=\"%s\"", _s ? _s : "");
}
	fprintf(stderr, "%s\n", buf);
	// ...}

#ifdef BUILD_CLASSROUTES
	int world_id = 0; /* where did CNK setup our commworld classroute? */
	int nClassRoutes = 16; /* how many are really available to us? */

	// At this point, 'commworld' should be the circumscribing rectangle for the
	// active nodes and 'excluded' (if not NULL and nexcl > 0) is the array of
	// excluded nodes from 'commworld'.
	// Note: it was agreed that all sub-comms will be full rectangles, and
	// commworld was setup by CNK using -np, so we really don't care about
	// 'excluded' and 'nexcl'.

	// NOTE: commworld already setup by CNK. Do we even care about
	// that rectangle any more?
	// Or do we only care about the booted block and it's root?

	ClassRoute_t cr;
	void *crdata = NULL; /* used by routines to keep track of classroute assignments */

	CR_RECT_T subcomm;
	CR_COORD_T *subexcl;
	int subnexcl;
	uint32_t mask;
	for (i = 0; i < nClassRoutes; ++i) {
		get_comm(i, &commworld, excluded, nexcl,
				map, pri_dim,
				&subcomm, &subexcl, &subnexcl);
		if (is_inside_rect(&subcomm, &me)) {
			if (subnexcl > 0) {
				MUSPI_BuildNodeClassrouteSparse(&refcomm, &refroot, &me,
							&commworld, subexcl, subnexcl,
							NULL, pri_dim, &cr);
			} else {
				MUSPI_BuildNodeClassroute(&refcomm, &refroot, &me,
							&commworld,
							NULL, pri_dim, &cr);
			}

			// assume we are contributing data. sparse rectangles might need an if-check here.
			cr.input |= BGQ_CLASS_INPUT_LINK_LOCAL;

			// we use the "sub-comm" VC since there are actually only two.
			cr.input |= BGQ_CLASS_INPUT_VC_SUBCOMM;

			// Use SUBCOMM VC to avoid conflicts with SYSTEM.
			mask = MUSPI_GetClassrouteIds(BGQ_CLASS_INPUT_VC_SUBCOMM,
							&commworld, &crdata);
		} else {
			// if I am not in this sub-comm, just allow any id...
			mask = (uint64_t)-1;
			cr.input = 0;
			cr.output = 0;
		}

		allreduce(world_id, &mask, &mask,
					1, PAMI_UNSIGNED_LONG_LONG, PAMI_BAND);

		if (is_inside_rect(&subcomm, &me)) {
			int id;
			id = ffs(mask);
			if (id == 0) {
				// fatal error - no classroute ids available.
				// we could just 'break' here, but all nodes must follow suit...
			}
			--id; /* ffs() returns bit# + 1 */
			classRouteIds[i] = id;
			classRoutes[i] = cr;
			classRouteSize[i] = __MUSPI_rect_size(&subcomm);
			(void)MUSPI_SetClassrouteId(classRouteIds[i],
						BGQ_CLASS_INPUT_VC_SUBCOMM,
						&commworld, &crdata);
		} else {
			//classRouteIds[i] = -1;
			//classRoutes[i] = cr;
			classRouteSize[i] = 0;
		}
	}

	//=======================================================================

	// Initialize all of the collective class routes
	for (i = 0; i < nClassRoutes; ++i) {
		rc = Kernel_SetCollectiveClassRoute(classRouteIds[i], &classRoutes[i]);
		if (rc) {
			printf("Kernel_SetCollectiveClassRoute for class route %u returned rc = %u\n",i, rc);
			exit(1);
		}
	}
	//=======================================================================

	// Now test all the classroutes...
	for (i = 0; i < nClassRoutes; ++i) {
		if (classRouteSize[i] == 0) continue; /* barrier needed? */

		unsigned long long rbuf, sbuf;
		sbuf = 1;
		rbuf = -1;
		allreduce(classRouteIds[i], &sbuf, &rbuf,
					1, PAMI_UNSIGNED_LONG_LONG, PAMI_SUM);
		if (rbuf != classRouteSize[i] * 1) {
			// report error
		}
	}

	//=======================================================================

	//=======================================================================

	// Deallocate all of the collective class routes
	for (i = 0; i < nClassRoutes; ++i) {
		MUSPI_ReleaseClassrouteId(classRouteIds[i], BGQ_CLASS_INPUT_VC_SUBCOMM,
							&commworld, &crdata);
		rc = Kernel_DeallocateCollectiveClassRoute(classRouteIds[i]);
		if (rc) {
			printf("Kernel_DeallocateCollectiveClassRoute for class route %u returned rc = %u\n",i, rc);
			exit(1);
		}
	}
#endif /* BUILD_CLASSROUTES */
	exit(0);
}
