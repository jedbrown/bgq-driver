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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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

// ml_ map library

#include <stdio.h> // printf()
#include <string.h> // memset(), memcpy()
#include <assert.h>

#if !defined(__ASSEMBLY__) && defined(__cplusplus)
extern "C" {
#endif


// returns index[0] + index[1]*dim_len[0] + index[2]*dim_len[0]*dim_len[1] + ...
int ml_ind2seq(int num_dims, int dim_len[], int index[]){
  int seq = 0;
  int prod = 1;
  int d;
  for (d=0; d<num_dims; d++) {
    seq += index[d]*prod;
    prod *= dim_len[d];
  }
  return seq;
}

// returns:
// index[0] = seq % dim_len[0]
// index[1] = ( (seq - index[0])/dim_len[0] ) % dim_len[1]
// index[2] = . . .
void ml_seq2ind(int num_dims, int dim_len[], int seq, int index[]){
  int rem = seq;
  int d;
  for (d=0; d<num_dims; d++) {
    index[d] = rem%dim_len[d];
    rem = (rem - index[d])/dim_len[d];
  }
  return;
}


// returns index[0]*index[1]*index[2]*...
int ml_num_elements(int num_dims, int dim_len[]){
  int prod = 1;
  int d;
  for (d=0; d<num_dims; d++) {
    prod *= dim_len[d];
  }
  return prod;
}

char *ml_sprintf_indices(char *string, int num_dims, int index[]) {
  char *s = string;
  int d;
  for (d=0; d<num_dims; d++) {
    s += sprintf(s,"%d,",index[d]);
  }
  *(s-1) = '\0'; // Delete trailing comma.
  return string;
}


void ml_verify_seq2ind(int num_dims, int dim_len[], int verbose) {
  int index[num_dims];
  int seq, verify_seq;
  int num_elements = ml_num_elements(num_dims, dim_len);
  for (seq=0; seq<num_elements; seq++) {
    ml_seq2ind(num_dims, dim_len, seq, index);
    verify_seq = ml_ind2seq(num_dims, dim_len, index);
    if (verbose) {
      char sd[100], si[100];
      ml_sprintf_indices(sd, num_dims, dim_len);
      ml_sprintf_indices(si, num_dims, index);
      printf("num_dims=%d dim_len=%s index=%s seq=%d, verify_seq=%d\n", num_dims, sd, si, seq, verify_seq);
    }
    assert(seq == verify_seq);
  }
  return;
}

void ml_demo_verify_seq2ind(void) {
  int dim_len[] = {2,3,5,7,1,11}; // {1,2,3,5,7};
  int num_dims = sizeof(dim_len)/sizeof(dim_len[0]);
  ml_verify_seq2ind(num_dims, dim_len, 1);
  return;
}



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
void ml_zero_hg(int num_dims, int num_elems, ml_hg_type hg[]) {
  int e,d;
  for (e=0; e<num_elems;  e++) {
    hg[e].host_seq = -1;
    hg[e].guest_seq = -1;
    for (d=0; d<num_dims;  d++) {
      hg[e].link_occupancy[d][ML_PLUS_DIR] = 0;
      hg[e].link_occupancy[d][ML_MINUS_DIR] = 0;
    }
  }
}

// Input num_dims, num_elems, source[]. Output is dest[].
void ml_copy_hg(int num_dims, int num_elems, ml_hg_type source[], ml_hg_type dest[]) {
  int e,d;
  for (e=0; e<num_elems;  e++) {
    dest[e].host_seq = source[e].host_seq;
    dest[e].guest_seq = source[e].guest_seq;
    for (d=0; d<num_dims;  d++) {
      dest[e].link_occupancy[d][ML_PLUS_DIR]  = source[e].link_occupancy[d][ML_PLUS_DIR];
      dest[e].link_occupancy[d][ML_MINUS_DIR] = source[e].link_occupancy[d][ML_MINUS_DIR];
    }
  }
}

// Input num_dims, source[]. Output is dest[].
void ml_copy_dim_len(int num_dims, int source[], int dest[]) {
  int d;
  for (d=0; d<num_dims; d++) {
    dest[d] = source[d];
  }
}

void ml_identity_hg(ml_hg_type hg[], int num_dims, int num_elems) {
  int e,d;
  for (e=0; e<num_elems;  e++) {
    hg[e].host_seq  = e;
    hg[e].guest_seq = e;
    for (d=0; d<num_dims;  d++) {
      hg[e].link_occupancy[d][ML_PLUS_DIR]  = 1;
      hg[e].link_occupancy[d][ML_MINUS_DIR] = 1;
    }
  }
}


// Input host_seq. Returns guest_seq and guest_dim_len[].
// arg[0] = dimension of length 4 to be flattened to length 1.
// arg[1] = first dimension to be doubled
// arg[2] = second dimension to be doubled
// Example: num_dims=5, host_dim_len[]={3,5,7,11,4}, args[]={4,1,3} returns guest_dim_len={3,10,7,22,1}
int ml_map_1node_guest2N2P1_hostNP4(int args[], int num_dims, int host_dim_len[], int host_seq, int guest_dim_len[]) {
  int flatten_dim       = args[0];
  int  first_double_dim = args[1];
  int second_double_dim = args[2];
  assert(num_dims >= 3);
  assert(host_dim_len[flatten_dim] == 4);
  assert(flatten_dim      !=  first_double_dim);
  assert(flatten_dim      != second_double_dim);
  assert(first_double_dim != second_double_dim);

  int d;
  for (d=0; d<num_dims; d++) {
    guest_dim_len[d] = host_dim_len[d];
  }
  guest_dim_len[      flatten_dim] = 1;
  guest_dim_len[ first_double_dim] = 2 * host_dim_len[ first_double_dim];
  guest_dim_len[second_double_dim] = 2 * host_dim_len[second_double_dim];

  int host_index[num_dims];
  int guest_index[num_dims];

  ml_seq2ind(num_dims, host_dim_len, host_seq, host_index);

  for (d=0; d<num_dims; d++) {
    guest_index[d] = host_index[d];
  }
  guest_index[      flatten_dim] = 0;

  if (host_index[flatten_dim]==0 || host_index[flatten_dim]==3) {
    guest_index[first_double_dim] = host_index[first_double_dim];
  } else {
    guest_index[first_double_dim] = guest_dim_len[first_double_dim] - 1 - host_index[first_double_dim];
  }
  if (host_index[flatten_dim]==0 || host_index[flatten_dim]==1) {
    guest_index[second_double_dim] = host_index[second_double_dim];
  } else {
    guest_index[second_double_dim] = guest_dim_len[second_double_dim] - 1 - host_index[second_double_dim];
  }
  return ml_ind2seq(num_dims, guest_dim_len, guest_index);

}


// Input host_seq. Returns guest_seq and guest_dim_len[].
// arg[0] = dimension to be flattened to length 1. This length must be even.
// arg[1] = dimension to be multiplied by arg[0]
// Example: num_dims=5, host_dim_len[]={3,5,7,11,6}, args[]={4,1} returns guest_dim_len={3,30,7,22,1}
int ml_map_1node_guestNxP1_hostNP(int args[], int num_dims, int host_dim_len[], int host_seq, int guest_dim_len[]) {
  int flatten_dim  = args[0];
  int increase_dim = args[1];
  assert(num_dims >= 2);
  assert(host_dim_len[flatten_dim]%2 == 0);
  assert(flatten_dim      !=  increase_dim);

  int d;
  for (d=0; d<num_dims; d++) {
    guest_dim_len[d] = host_dim_len[d];
  }
  guest_dim_len[      flatten_dim] = 1;
  guest_dim_len[increase_dim] = host_dim_len[flatten_dim] * host_dim_len[ increase_dim];

  int host_index[num_dims];
  int guest_index[num_dims];

  ml_seq2ind(num_dims, host_dim_len, host_seq, host_index);

  for (d=0; d<num_dims; d++) {
    guest_index[d] = host_index[d];
  }
  guest_index[      flatten_dim] = 0;


  // Following snakes the 1D guest[increase_dim] in the 2D host[flatten_dim]*host[increase_dim]
  guest_index[increase_dim] = host_index[flatten_dim]*host_dim_len[increase_dim];
  if (host_index[flatten_dim]%2 == 0) {
    guest_index[increase_dim] += host_index[increase_dim];
  } else {
    guest_index[increase_dim] += host_dim_len[increase_dim] - 1 - host_index[increase_dim];
  }

  return ml_ind2seq(num_dims, guest_dim_len, guest_index);

}

typedef int (*ml_map_1node_type)(int args[], int num_dims, int host_dim_len[], int host_seq, int guest_dim_len[]);

// Returns guest_dim_len and hg
void ml_map_allnodes_function(ml_map_1node_type map_1node, int args[], int num_dims, int host_dim_len[], ml_hg_type old_hg[], int guest_dim_len[], ml_hg_type hg[]) {
  int host_elements = ml_num_elements(num_dims, host_dim_len);

  ml_zero_hg(num_dims, host_elements, hg);

  int host_seq, guest_seq;
  for (host_seq=0; host_seq<host_elements; host_seq++) {
    guest_seq = (*map_1node)(args, num_dims, host_dim_len, host_seq, guest_dim_len);

    int guest_elements = ml_num_elements(num_dims, guest_dim_len);
    assert(host_elements == guest_elements);

    if (hg[guest_seq].host_seq != -1)
    {
      char hd[100]; ml_sprintf_indices(hd, num_dims,  host_dim_len);
      char gd[100]; ml_sprintf_indices(gd, num_dims, guest_dim_len);
      printf("num_dims=%d, host_seq=%d, host_dim_len=%s, guest_seq=%d, guest_dim_len=%s\n",num_dims, host_seq, hd, guest_seq, gd);
    }
    assert(hg[guest_seq].host_seq == -1);
    //    hg[guest_seq].host_seq = host_seq;
    hg[guest_seq].host_seq = old_hg[host_seq].host_seq;
    assert(hg[hg[guest_seq].host_seq].guest_seq == -1);
    hg[hg[guest_seq].host_seq].guest_seq = guest_seq;

  }
  return;
}



// Returns new_hg and guest_dim_len
void ml_multimap_allnodes_function(int num_maps, ml_map_1node_type map_1node[], int args[][ML_MAX_ARGS], int num_dims, int host_dim_len[], ml_hg_type old_hg[], int guest_dim_len[], ml_hg_type hg[]) {
  int intern_dim_len[num_dims];
  int num_elems = ml_num_elements(num_dims, host_dim_len);
  ml_hg_type intern_hg[num_elems];
  int m;
  for (m=0; m<num_maps; m++) {
    if (m==0) {
      // copy input map into internal
      ml_copy_hg(num_dims, num_elems, old_hg, intern_hg);
      ml_copy_dim_len(num_dims, host_dim_len, intern_dim_len);
    } else {
      // copy previous result into internal
      ml_copy_hg(num_dims, num_elems,     hg, intern_hg);
      ml_copy_dim_len(num_dims, guest_dim_len, intern_dim_len);
    }
    ml_map_allnodes_function(map_1node[m], args[m], num_dims, intern_dim_len, intern_hg, guest_dim_len, hg);
  }

}

// Returns shortest number of hops on a torus dimension.
int ml_hops_on_a_dim(int dim_len, int a, int b) {
  int q = (a-b+dim_len)%dim_len;
  int r = (b-a+dim_len)%dim_len;
  return q<r ? q : r;
}

// Returns true==non-0 if plus is next direction using shortest path on torus
int ml_next_direction_plus(int dim_len, int start, int dest) {
  int  plus = ml_hops_on_a_dim(dim_len, (start+1        )%dim_len, dest);
  int minus = ml_hops_on_a_dim(dim_len, (start-1+dim_len)%dim_len, dest);

  if        ( plus < minus) {
    return 1;
  } else if (minus <  plus) {
    return 0;
  } else if (start%2 == 0) {
    return 1;  // On tie, even start goes  plus.
  }
    return 0;  // On tie,  odd start goes minus.
}

// Returns next index using shortest path on torus
int ml_next_index(int dim_len, int start, int dest) {
  if (ml_next_direction_plus(dim_len, start, dest)) {
    return (start+1        )%dim_len;
  }
    return (start-1+dim_len)%dim_len;
}

// Using dim-ordered routing, moves path_index[] one hop towards dest_index[].
// Fills hg[].link_occupancy
// Returns 0 when returned path_index is same as dest_index.
// dest_index[]==path_index[] allowed on entry, then nothing happens.
int ml_dim_ordered_1hop(int num_dims, int dim_len[], int dest_index[], int path_index[], ml_hg_type hg[]) {
  int again = 0;
  int d;
  int next;
  int direction_plus;

  for(d=0; d<num_dims; d++) {
    if (path_index[d] != dest_index[d]) {
      again = 1;
      next = ml_next_index(dim_len[d], path_index[d], dest_index[d]);
      direction_plus = ml_next_direction_plus(dim_len[d], path_index[d], dest_index[d]);
      int plus_seq, minus_seq;
      if (direction_plus) {
	// Using  plus link from old path_index[d]
	plus_seq = ml_ind2seq(num_dims, dim_len, path_index);
	path_index[d] = next;
	// Using minus link from new path_index[d]
	minus_seq = ml_ind2seq(num_dims, dim_len, path_index);
      } else{
	// Using minus link from old path_index[d]
	minus_seq = ml_ind2seq(num_dims, dim_len, path_index);
	path_index[d] = next;
	// Using plus link from new path_index[d]
	plus_seq = ml_ind2seq(num_dims, dim_len, path_index);
      }
      ++hg[ plus_seq].link_occupancy[d][ ML_PLUS_DIR];
      ++hg[minus_seq].link_occupancy[d][ML_MINUS_DIR];
      break; // Only move one node at a time.
    }
  }

  return again;
}


// Returns hist_hops, hist_dim_diffs and hist_occupancy
//   hist_dim_diffs = number of different host dimensions from each guest node to each guest neighbor in plus direction.
//   hist_hops = number of host hops from each guest node to each guest neighbor in plus direction.
//   hist_occupancy = host links used from each guest node to all guest neighbors 
// Fills in hg[].link_occupancy assuming dim-ordered routing.
// Input perfect_neighbors as true==non-0 to verify.
void ml_nn_guest_stats(int num_dims, int host_dim_len[], ml_hg_type hg[], int guest_dim_len[], int perfect_neighbors, int hist_hops[], int hist_dim_diffs[], int hist_occupancy[]) {

  int guest_index[num_dims], gneighbor_index[num_dims];;
  int host_index[num_dims], hneighbor_index[num_dims];;
  int host_seq, hneighbor_seq;

  int guest_elements = ml_num_elements(num_dims, guest_dim_len);
  int  host_elements = ml_num_elements(num_dims,  host_dim_len);
  assert(guest_elements == host_elements);

  int check_dim;
  int guest_seq, gneighbor_seq;

  // Visit each guest node.
  for (guest_seq=0; guest_seq<guest_elements; guest_seq++) {
    ml_seq2ind(num_dims, guest_dim_len, guest_seq, guest_index);
    host_seq = hg[guest_seq].host_seq;
    ml_seq2ind(num_dims, host_dim_len, host_seq, host_index);

    // Visit each neighbor of each guest node.
    for (check_dim=0; check_dim<num_dims; check_dim++) {
      if (guest_dim_len[check_dim] == 1) continue;
      ml_seq2ind(num_dims, guest_dim_len, guest_seq, gneighbor_index);
      gneighbor_index[check_dim] = ((gneighbor_index[check_dim] + 1)) % guest_dim_len[check_dim];

      gneighbor_seq = ml_ind2seq(num_dims, guest_dim_len, gneighbor_index);
      hneighbor_seq = hg[gneighbor_seq].host_seq;
      ml_seq2ind(num_dims, host_dim_len, hneighbor_seq, hneighbor_index);

      int h, dim_diffs=0, hops=0;
      for ( h=0;  h<num_dims;  h++) {
        if (host_index[h] != hneighbor_index[h]) {
          ++dim_diffs;
	  int this_dim_hops = ml_hops_on_a_dim(host_dim_len[h],host_index[h],hneighbor_index[h]);
	  if (perfect_neighbors) {
            if(this_dim_hops!=1) {
	      printf("this_dim_hops=%d, host_dim_len[%d]=%d, host_index[%d]=%d, hneighbor_index[%d]=%d\n",this_dim_hops, h,host_dim_len[h], h,host_index[h], h,hneighbor_index[h]);
            }
  	  }
	  assert(!perfect_neighbors || this_dim_hops==1);
	  hops += this_dim_hops;
	}
      }
      assert(hops < ML_MAX_HOPS);
      ++hist_hops[hops];
      assert(dim_diffs < ML_MAX_DIMS);
      ++hist_dim_diffs[dim_diffs];
      assert(!perfect_neighbors || hops==1);
      assert(!perfect_neighbors || dim_diffs==1);


      int hpath_index[num_dims]; memcpy(hpath_index,host_index,sizeof(hpath_index));
      //      printf("%d\n",sizeof(hpath_index));
      while (ml_dim_ordered_1hop(num_dims,host_dim_len,hneighbor_index,hpath_index,hg)) {
	// Do nothing;
#if 0
      char sd[100], sh[100], sn[100], sp[100];
      ml_sprintf_indices(sd, num_dims, host_dim_len);
      ml_sprintf_indices(sh, num_dims, host_index);
      ml_sprintf_indices(sn, num_dims, hneighbor_index);
      ml_sprintf_indices(sp, num_dims, hpath_index);
      printf("dim_len=%s host_index=%s hneighbor_index=%s hpath_index=%s\n", sd, sh, sn, sp);
#endif
      }

    }
  }

  int dim_gt1 = 0;
  for (check_dim=0; check_dim<num_dims; check_dim++) {
    if (guest_dim_len[check_dim] > 1) {
      ++dim_gt1;
    }
  }


  // Have checked each node to each of its neighbors.
  // Only care about dim_len[]>1
  if (perfect_neighbors) {
    if (hist_hops[1] != dim_gt1*guest_elements) {
      printf("hist_hops[1]=%d, dim_gt1*guest_elements=%d*%d\n", hist_hops[1], dim_gt1, guest_elements);
    }
    assert(hist_hops[1] == dim_gt1*guest_elements);
    assert(hist_dim_diffs[1] == dim_gt1*guest_elements);
  }

  // Visit each link of each host node.
  for (host_seq=0; host_seq<host_elements; host_seq++) {
    int node_occupancy = 0;
    for (check_dim=0; check_dim<num_dims; check_dim++) {
      assert(ML_MAX_OCCUPANCY>hg[host_seq].link_occupancy[check_dim][ ML_PLUS_DIR]);
      ++hist_occupancy[       hg[host_seq].link_occupancy[check_dim][ ML_PLUS_DIR]];
      assert(ML_MAX_OCCUPANCY>hg[host_seq].link_occupancy[check_dim][ ML_MINUS_DIR]);
      ++hist_occupancy[       hg[host_seq].link_occupancy[check_dim][ ML_MINUS_DIR]];
      node_occupancy +=   hg[host_seq].link_occupancy[check_dim][ ML_PLUS_DIR]
                        + hg[host_seq].link_occupancy[check_dim][ML_MINUS_DIR];
    }
    if (perfect_neighbors) {
      if (node_occupancy != 2*dim_gt1) {
	printf("node_occupancy=%d 2*dim_gt1=2*%d\n",node_occupancy,dim_gt1);
      }
      assert(node_occupancy == 2*dim_gt1);
    }

  }
  
  int hops, occ;
  int total_hops = 0;
  int total_occ = 0;
  for ( occ = 0;  occ < ML_MAX_OCCUPANCY;  occ++) total_occ  += occ*hist_occupancy[occ];
  for (hops = 0; hops < ML_MAX_HOPS     ; hops++) total_hops += hops*hist_hops[hops];
  if (2*total_hops != total_occ) {
    printf("hist_hops[1]=%d, dim_gt1*guest_elements=%d*%d\n", hist_hops[1], dim_gt1, guest_elements);
    printf("total_hops=%d total_occ=%d\n",total_hops, total_occ);
  }
  assert(2*total_hops == total_occ);

}


void ml_print_hist(char *name, int hist[], int len) {
  int last, i, total=0;
  for (last=len-1; last>=0; last--) if (hist[last]) break;
  printf("%s ",name);
  for (i=0; i<=last; i++) {
    printf("%d*%d=%d,",i,hist[i],i*hist[i]);
    total += i*hist[i];
  }
  printf("total=%d\n",total);
}


int ml_examples(int which_example) {

  int host_dim_len[]  = {5,5,6,4};

  int num_dims = sizeof(host_dim_len)/sizeof(host_dim_len[0]);
  int num_elems = ml_num_elements(num_dims, host_dim_len);
  ml_hg_type hg[num_elems];

  ml_hg_type identity_hg[num_elems];
  ml_identity_hg(identity_hg, num_dims, num_elems);

  int guest_dim_len[num_dims];

  if (which_example==1) {
    // Example with 1 remapping.
    // Works: 4 8 8 from 4 4 4 4
    // Works: 5 8 8 from 5 4 4 4
    // Works: 5 10 8 from 5 5 4 4
    // Works: 5 10 12 from 5 5 6 4
    int args[] = {3,1,2};
    ml_map_allnodes_function(ml_map_1node_guest2N2P1_hostNP4, args, num_dims, host_dim_len, identity_hg, guest_dim_len, hg);
  } else if (which_example==2) {
    // Example with 2 remappings, with user-managed intermediate map.
    int host_dim_len1[num_dims];
    int args1[] = {2,0};
    ml_hg_type hg1[num_elems];
    ml_map_allnodes_function(ml_map_1node_guestNxP1_hostNP, args1, num_dims, host_dim_len,  identity_hg, host_dim_len1, hg1);
    int args[] = {3,0};
    ml_map_allnodes_function(ml_map_1node_guestNxP1_hostNP, args, num_dims, host_dim_len1,  hg1, guest_dim_len, hg);
  } else if (which_example==3) {
    // Example with 3 remappings.
    ml_map_1node_type multi_mappings[] = {ml_map_1node_guestNxP1_hostNP, ml_map_1node_guestNxP1_hostNP, ml_map_1node_guestNxP1_hostNP};
    int multi_args[][ML_MAX_ARGS] = {{2,0}, {3,0}, {0,1}};
    ml_multimap_allnodes_function(3, multi_mappings, multi_args, num_dims, host_dim_len,  identity_hg, guest_dim_len, hg);
  } else {
    printf("NOTHING\n");
  }

      char sh[100], sg[100];
      ml_sprintf_indices(sh, num_dims, host_dim_len);
      ml_sprintf_indices(sg, num_dims, guest_dim_len);
      printf("num_dims=%d host_dim_len=%s guest_dim_len=%s\n", num_dims, sh, sg);


  int perfect_neighbors = 1;
  int      hist_hops[ML_MAX_HOPS]; memset(hist_hops     , 0, sizeof(hist_hops     ));
  int hist_dim_diffs[ML_MAX_DIMS]; memset(hist_dim_diffs, 0, sizeof(hist_dim_diffs));
  int hist_occupancy[ML_MAX_OCCUPANCY]; memset(hist_occupancy, 0, sizeof(hist_occupancy));
  ml_nn_guest_stats(num_dims, host_dim_len, hg, guest_dim_len, perfect_neighbors, hist_hops, hist_dim_diffs, hist_occupancy);

  ml_print_hist((char *)"hist_hops", hist_hops, ML_MAX_HOPS);
  ml_print_hist((char *)"hist_dim_diffs", hist_dim_diffs, ML_MAX_DIMS);
  ml_print_hist((char *)"hist_occupancy", hist_occupancy, ML_MAX_OCCUPANCY);

  return 0;
}

#if !defined(__ASSEMBLY__) && defined(__cplusplus)
}
#endif

