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
 * \file components/devices/bgq/mu2/global/trace_classroutes.h
 * \brief Library header for tests on classroute SPIs
 */
#ifndef __components_devices_bgq_mu2_global_trace_classroutes_h__
#define __components_devices_bgq_mu2_global_trace_classroutes_h__

#include <stdio.h>

#ifndef __INLINE__
#define __INLINE__	static inline
#endif /* __INLINE__ */
#include "spi/include/mu/Classroute_inlines.h"

#include <ctype.h>
#ifdef __cplusplus
extern "C" {
#endif

static const char *dim_names = CR_DIM_NAMES; /**< dimension names currently in use */
static const char *dir_names = CR_DIR_NAMES; /**< direction names */
static int digraph = 0; /**< boolean indicating that output should be formatted for "dot" */

/**
 * @defgroup DIGRAPH_OUTPUT_FLAGS flags for 'digraph'...
 * @{
 */
#define CRTEST_GRAPH		0x01	/**< enable (default) digraph output */
#define CRTEST_GRAPH_BI_DIR	0x02	/**< show up-tree links (in addition to down-tree) */
#define CRTEST_GRAPH_RANKS	0x04	/**< use ranks as node label (instead of coords) */
/** @} */

/**
 * \brief Test structure container for data related to classroute
 */
typedef struct {
	CR_RECT_T rect;	 /**< classroute (bounding) rectangle */
	CR_COORD_T root; /**< classroute root */
	int *map;	 /**< classroute dimension mapping (order) */
	int pri_dim;	 /**< classroute primary dimension */
} commworld_t;

/**
 * \brief compute neighbor coords given axis and direction
 *
 * \param[in] comm	Communicator to operatin within
 * \param[in] me	Caller (reference) node
 * \param[in] dim	Dimension (axis) prosective neighbor is on
 * \param[in] sgn	Direction (sign) of prosective neighbor
 * \param[out] nb	Neighbor coordinates (if return 0)
 * \return	-1 if no neighbor exists in given communicator (0 == success)
 */
static inline int compute_neighbor(CR_RECT_T *comm, CR_COORD_T *me, int dim, int sgn, CR_COORD_T *nb) {
	static int signs[] = {
#ifdef __cplusplus
		1, -1
#else
		[CR_SIGN_POS] = 1,
		[CR_SIGN_NEG] = -1,
#endif
	};

	CR_COORD_T c0 = *me;
	CR_COORD_DIM(&c0,dim) += signs[sgn];
	if (CR_COORD_DIM(&c0,dim) < CR_COORD_DIM(CR_RECT_LL(comm),dim) ||
	    CR_COORD_DIM(&c0,dim) > CR_COORD_DIM(CR_RECT_UR(comm),dim)) {
		return -1; // out of bounds
	}
	*nb = c0;
	return 0;
}

/**
 * \brief sprintf-style printing of coordinate
 *
 * \param[out] buf	character representation of coordinate
 * \param[in] co	coordinate to print
 * \return	Number of characters put into 'buf'
 */
static inline int sprint_coord(char *buf, CR_COORD_T *co) {
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

/**
 * \brief sprintf-style printing of torus links (not IO or LOCAL)
 *
 * \param[out] buf	character representation of links
 * \param[in] link	Bitmap of torus links (as in CR_LINK)
 * \return	Number of characters put into 'buf'
 */
static inline int sprint_links(char *buf, uint32_t link) {
	char *s = buf;
	int x;
	for (x = 0; x < CR_NUM_DIMS; ++x) {
		if (link & CR_LINK(x,CR_SIGN_POS)) {
			if (s != buf) *s++ = ' ';
			*s++ = dim_names[x];
			*s++ = dir_names[CR_SIGN_POS];
		}
		if (link & CR_LINK(x,CR_SIGN_NEG)) {
			if (s != buf) *s++ = ' ';
			*s++ = dim_names[x];
			*s++ = dir_names[CR_SIGN_NEG];
		}
	}
	if (s == buf) { *s++ = ' '; *s++ = ' '; }
	*s = '\0';
	return s - buf;
}

/**
 * \brief sprintf-style printing of links for use by 'dot' graphing programs
 *
 * \param[out] buf	character representation of links
 * \param[in] cw	commworld_t struct for reference
 * \param[in] comm	communicator (rectangle) to work in
 * \param[in] me	current node coordinates
 * \param[in] node	current node string (rank or coordinates in string format)
 * \param[in] root	boolean indicating this node is the root
 * \param[in] dim	dimension (axis) being examined
 * \param[in] sgn	direction being examined
 * \param[in] color	string representation of color to use for graph line
 * \return	Number of characters put into 'buf'
 */
static inline int sprint_dot_link(char *buf, commworld_t *cw, CR_RECT_T *comm, CR_COORD_T *me,
			char *node, int root, int dim, int sgn, const char *color) {
	char *s = buf;
	CR_COORD_T c0;
	static char neig[256];
	if (compute_neighbor(comm, me, dim, sgn, &c0)) {
		s += sprintf(s, "\"%s\" -> \"XXXXX\" [ color=\"red\",", node);
	} else {
		if (digraph & CRTEST_GRAPH_RANKS) {
			pami_task_t task = 0;
			pami_network net;
			__global.mapping.network2task(&c0, &task, &net);
			sprintf(neig, "%d", task);
		} else {
			sprint_coord(neig, &c0);
		}
		s += sprintf(s, "\"%s\" -> \"%s\" [ color=\"%s\",", node, neig, color);
	}
	s += sprintf(s, " label=\"%c%c\" ]\n", dim_names[dim], dir_names[sgn]);
	return s - buf;
}

/**
 * \brief sprintf-style printing of classroute for use by 'dot' graphing programs
 *
 * \param[out] buf	character representation of links
 * \param[in] cw	commworld_t struct for reference
 * \param[in] comm	communicator (rectangle) to work in
 * \param[in] me	current node coordinates
 * \param[in] cr	Classroute to print
 * \param[in] rank	Rank of 'me'
 * \return	Number of characters put into 'buf'
 */
static inline int sprint_dot(char *buf, commworld_t *cw, CR_RECT_T *comm, CR_COORD_T *me, ClassRoute_t *cr, int rank) {
	char *s = buf;
	int x, g;
	int root = (cr->output == 0 && cr->input != 0);
	static char node[256];
	*s = '\0';
	if (digraph & CRTEST_GRAPH_RANKS) {
		sprintf(node, "%d", rank);
	} else {
		sprint_coord(node, me);
	}
	if (root) {
		s += sprintf(s, "\"%s\" [ style=filled,fillcolor=\"yellow\",root=1 ];\n", node);
		s += sprintf(s, " root=\"%s\";\n", node);
	}
	if (cr->input && !(cr->input & BGQ_CLASS_INPUT_LINK_LOCAL)) {
		s += sprintf(s, "\"%s\" [ style=filled,fillcolor=\"gray\" ];\n", node);
	}
	for (x = 0; x < CR_NUM_DIMS; ++x) {
		for (g = 0; g < CR_NUM_DIRS; ++g) {
			unsigned m = CR_LINK(x, g);
			if ((cr->input & m) && (digraph & CRTEST_GRAPH_BI_DIR)) {
				s += sprint_dot_link(s, cw, comm, me, node, root, x, g, "black");
			}
			if (cr->output & m) {
				s += sprint_dot_link(s, cw, comm, me, node, root, x, g, "green");
			}
		}
	}
	return s - buf;
}

/**
 * \brief sprintf-style printing of rectangle
 *
 * \param[out] buf	character representation of rectangle
 * \param[in] r		rectangle to print
 * \return	Number of characters put into 'buf'
 */
static inline int sprint_rect(char *buf, CR_RECT_T *r) {
	char *s = buf;

	s += sprint_coord(s, CR_RECT_LL(r));
	*s++ = ':';
	s += sprint_coord(s, CR_RECT_UR(r));
	return s - buf;
}

/**
 * \brief Print a classroute to stdout
 *
 * Observes 'digraph' flag and formats output for 'dot' appropriately.
 * Ignores flags (previously discovered errors in classroute).
 * Prints LOCAL link, if set, but ignores IO links.
 *
 * \param[in] cw	commworld_t reference info
 * \param[in] cm	communicator
 * \param[in] me	current node coordinates
 * \param[in] cr	classroute to print
 * \param[in] rank	rank of current node
 */
static inline void print_classroute(commworld_t *cw, CR_RECT_T *cm, CR_COORD_T *me, ClassRoute_t *cr, int rank) {
	static char buf[1024];
	char *s = buf;
	if (digraph) {
		s += sprint_dot(s, cw, cm, me, cr, rank);
	} else {
		s += sprint_coord(s, me);
		//s += sprintf(s, "[%d]", rank);
		s += sprintf(s, " up: ");
		s += sprint_links(s, cr->output);
		s += sprintf(s, " dn: ");
		s += sprint_links(s, cr->input);
		if (cr->input & BGQ_CLASS_INPUT_LINK_LOCAL) {
			*s++ = ' ';
			*s++ = 'L';
		}
		*s++ = '\n';
		*s++ = '\0';
	}
	printf("%s", buf);
}

static inline void print_prefix(commworld_t *cw, CR_RECT_T *comm) {
	static char buf[1024];
	char *s = buf;
	int e;
	if (digraph) {
		s += sprint_rect(s, comm);
		s += sprintf(s, " world ");
		s += sprint_rect(s, &cw->rect);
		s += sprintf(s, " root ");
		s += sprint_coord(s, &cw->root);
		if (cw->map) {
			s += sprintf(s, " map ");
			for (e = 0; e < CR_NUM_DIMS; ++e) {
				*s++ = dim_names[cw->map[e]];
			}
			*s = '\0';
		}
		s += sprintf(s, " pri %c", dim_names[cw->pri_dim]);
		printf("digraph \"%s\" {\n", buf);
		printf(" label=\"%s\";\n", buf);
		printf(" overlap=\"scale\";\n");
	} else {
		s += sprintf(s, "Classroute for comm ");
		s += sprint_rect(s, comm);
		s += sprintf(s, "\n\tin world ");
		s += sprint_rect(s, &cw->rect);
		s += sprintf(s, "\n\twith root ");
		s += sprint_coord(s, &cw->root);
		if (cw->map) {
			s += sprintf(s, " map ");
			for (e = 0; e < CR_NUM_DIMS; ++e) {
				*s++ = dim_names[cw->map[e]];
			}
			*s = '\0';
		}
		printf("%s\n\tand primary dimension '%c':\n", buf, dim_names[cw->pri_dim]);
	}
}

static inline void print_suffix(commworld_t *cw, CR_RECT_T *comm) {
	if (digraph) {
		printf("};\n");
	}
}

#ifdef __cplusplus
};
#endif

#endif /* __experimental_bgq_cnclassroute_testlib_h__ */
