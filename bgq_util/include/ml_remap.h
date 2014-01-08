/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2011                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#ifndef __ML_REMAP_H__
#define __ML_REMAP_H__

// ml_remap remaps an X-dimensional torus as a Y-dimensional torus

#include <stdio.h> // printf()
#include <string.h> // memset(), memcpy()
#include <assert.h>

#include <hwi/include/common/compiler_support.h> // __BEGIN_DECLS

__BEGIN_DECLS


// returns index[0] + index[1]*dim_len[0] + index[2]*dim_len[0]*dim_len[1] + ...
int ml_ind2seq(int num_dims, int dim_len[], int index[]);

// returns:
// index[0] = seq % dim_len[0]
// index[1] = ( (seq - index[0])/dim_len[0] ) % dim_len[1]
// index[2] = . . .
void ml_seq2ind(int num_dims, int dim_len[], int seq, int index[]);

// returns index[0]*index[1]*index[2]*...
int ml_num_elements(int num_dims, int dim_len[]);

char *ml_sprintf_indices(char *string, int num_dims, int index[]);

void ml_verify_seq2ind(int num_dims, int dim_len[], int verbose);

void ml_demo_verify_seq2ind(void);


#define ML_MAX_ARGS 10
#define ML_MAX_DIMS 10
#define ML_MAX_HOPS 1000
#define ML_MAX_OCCUPANCY 100
#define  ML_PLUS_DIR 0
#define ML_MINUS_DIR 1

// For the mapping:
// . host is the physical network
// . guest is the application network to be mapped onto the host network
// . For each node:
//   . hg[guest_seq].host_seq == host_seq
//   . hg[host_seq].guest_seq == guest_seq

// Example
// ml_hg_type hg[num_elements];
// . hg[guest_seq].host_seq // Maps guest_seq to host_seq
// . hg[host_seq].guest_seq // Maps host_seq to guest_seq
// . hg[host_seq].link_occupancy[dim][dir] // Number of guest nearest neighbor paths on tthis link
typedef struct {
  int host_seq;
  int guest_seq;
  // Link occupancy is use by all nearest-neighbor guest nodes.
  signed char link_occupancy[ML_MAX_DIMS][2];
} ml_hg_type;

// Input num_dims, num_elems. Output is zeroed hg[].
void ml_zero_hg(int num_dims, int num_elems, ml_hg_type hg[]);

// Input num_dims, num_elems, source[]. Output is dest[].
void ml_copy_hg(int num_dims, int num_elems, ml_hg_type source[], ml_hg_type dest[]);

// Input num_dims, source[]. Output is dest[].
void ml_copy_dim_len(int num_dims, int source[], int dest[]);

void ml_identity_hg(ml_hg_type hg[], int num_dims, int num_elems);

// Input host_seq. Returns guest_seq and guest_dim_len[].
// arg[0] = dimension of length 4 to be flattened to length 1.
// arg[1] = first dimension to be doubled
// arg[2] = second dimension to be doubled
// Example: num_dims=5, host_dim_len[]={3,5,7,11,4}, args[]={4,1,3} returns guest_dim_len={3,10,7,22,1}
int ml_map_1node_guest2N2P1_hostNP4(int args[], int num_dims, int host_dim_len[], int host_seq, int guest_dim_len[]);

// Input host_seq. Returns guest_seq and guest_dim_len[].
// arg[0] = dimension to be flattened to length 1. This length must be even.
// arg[1] = dimension to be multiplied by arg[0]
// Example: num_dims=5, host_dim_len[]={3,5,7,11,6}, args[]={4,1} returns guest_dim_len={3,30,7,22,1}
int ml_map_1node_guestNxP1_hostNP(int args[], int num_dims, int host_dim_len[], int host_seq, int guest_dim_len[]);

typedef int (*ml_map_1node_type)(int args[], int num_dims, int host_dim_len[], int host_seq, int guest_dim_len[]);

// Returns guest_dim_len and hg
void ml_map_allnodes_function(ml_map_1node_type map_1node, int args[], int num_dims, int host_dim_len[], ml_hg_type old_hg[], int guest_dim_len[], ml_hg_type hg[]);



// Returns new_hg and guest_dim_len
void ml_multimap_allnodes_function(int num_maps, ml_map_1node_type map_1node[], int args[][ML_MAX_ARGS], int num_dims, int host_dim_len[], ml_hg_type old_hg[], int guest_dim_len[], ml_hg_type hg[]);

// Returns shortest number of hops on a torus dimension.
int ml_hops_on_a_dim(int dim_len, int a, int b);

// Returns true==non-0 if plus is next direction using shortest path on torus
int ml_next_direction_plus(int dim_len, int start, int dest);

// Returns next index using shortest path on torus
int ml_next_index(int dim_len, int start, int dest);

// Using dim-ordered routing, moves path_index[] one hop towards dest_index[].
// Fills hg[].link_occupancy
// Returns 0 when returned path_index is same as dest_index.
// dest_index[]==path_index[] allowed on entry, then nothing happens.
int ml_dim_ordered_1hop(int num_dims, int dim_len[], int dest_index[], int path_index[], ml_hg_type hg[]);


// Returns hist_hops, hist_dim_diffs and hist_occupancy
//   hist_dim_diffs = number of different host dimensions from each guest node to each guest neighbor in plus direction.
//   hist_hops = number of host hops from each guest node to each guest neighbor in plus direction.
//   hist_occupancy = host links used from each guest node to all guest neighbors 
// Fills in hg[].link_occupancy assuming dim-ordered routing.
// Input perfect_neighbors as true==non-0 to verify.
void ml_nn_guest_stats(int num_dims, int host_dim_len[], ml_hg_type hg[], int guest_dim_len[], int perfect_neighbors, int hist_hops[], int hist_dim_diffs[], int hist_occupancy[]);


void ml_print_hist(char *name, int hist[], int len);

int ml_examples(int which_example);

__END_DECLS

#endif // __ML_REMAP_H__
