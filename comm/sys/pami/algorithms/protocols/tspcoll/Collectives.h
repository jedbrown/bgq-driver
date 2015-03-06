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
 * \file algorithms/protocols/tspcoll/Collectives.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_Collectives_h__
#define __algorithms_protocols_tspcoll_Collectives_h__

#include "algorithms/protocols/tspcoll/CollExchange.h"
#include "algorithms/protocols/tspcoll/Barrier.h"
#include "algorithms/protocols/tspcoll/Allreduce.h"
#include "algorithms/protocols/tspcoll/Broadcast.h"
#include "algorithms/protocols/tspcoll/QuadnomBcast.h"
#include "algorithms/protocols/tspcoll/Allgather.h"
#include "algorithms/protocols/tspcoll/Allgatherv.h"
#include "algorithms/protocols/tspcoll/Alltoall.h"
#include "algorithms/protocols/tspcoll/Alltoallv.h"
#include "algorithms/protocols/tspcoll/Gather.h"
#include "algorithms/protocols/tspcoll/Scatter.h"
#include "algorithms/protocols/tspcoll/Permute.h"
#include "algorithms/protocols/tspcoll/PrefixSums.h"
#ifdef XLPGAS_PAMI_CAU
#include "algorithms/protocols/tspcoll/SHMReduceBcast.h"
#include "algorithms/protocols/tspcoll/ShmCauAllReduce.h"
#include "algorithms/protocols/tspcoll/ShmHybridBcast.h"
#endif
//#include "algorithms/protocols/tspcoll/AMExchange.h"


namespace xlpgas{
  template <class T_NI, class T_Device>
struct base_coll_defs{
  typedef xlpgas::Barrier<T_NI>    barrier_type;
  typedef xlpgas::Broadcast<T_NI>  broadcast_type;
  typedef xlpgas::Allreduce::Long<T_NI> allreduce_type;
  typedef xlpgas::Allreduce::Short<T_NI> short_allreduce_type;
  typedef xlpgas::Allgather<T_NI>  allgather_type;
  typedef xlpgas::Allgatherv<T_NI> allgatherv_type;
  typedef xlpgas::Alltoall<T_NI>   alltoall_type;
  typedef xlpgas::Alltoallv<T_NI,size_t>  alltoallv_type;
  typedef xlpgas::Alltoallv<T_NI,int>  alltoallvint_type;
  typedef xlpgas::Gather<T_NI>     gather_type;
  typedef xlpgas::Scatter<T_NI>    scatter_type;
  typedef xlpgas::Permute<T_NI>    permute_type;
  typedef xlpgas::PrefixSums<T_NI> prefixsums_type;
  //shm/cau/hybrid
#ifdef XLPGAS_PAMI_CAU
  typedef xlpgas::SHMReduce<T_NI>  shm_reduce_type;
  typedef xlpgas::SHMBcast<T_NI>   shm_broadcast_type;
  typedef xlpgas::CAUReduce<T_NI>  cau_reduce_type;
  typedef xlpgas::CAUBcast<T_NI>   cau_broadcast_type;
  typedef xlpgas::SHMLargeBcast<T_NI>   shm_large_broadcast_type;
  typedef xlpgas::Broadcast<T_NI>   leaders_broadcast_type;
  typedef xlpgas::ShmCauAllReduce<T_NI, T_Device> shm_cau_allreduce_type;
  typedef xlpgas::ShmHybridBcast<T_NI, T_Device> shm_hybrid_broadcast_type;
  typedef xlpgas::ShmHybridPipelinedBcast<T_NI, T_Device> shm_hybrid_pipelined_broadcast_type;
#endif
  //on certain platforms(BG, PERCS) the operations above may be specialized
  //but for certain configurations (geometry, operation, etc) they may not work;
  //In such situations the generic ones using point to point are used;
  typedef xlpgas::Barrier<T_NI>    barrier_pp_type;
#if 0
  typedef xlpgas::QuadnomBcast<T_NI>  broadcast_pp_type;
#else
  typedef xlpgas::Broadcast<T_NI>  broadcast_pp_type;
#endif
  typedef xlpgas::Broadcast<T_NI>  broadcast_tree_type;
  typedef xlpgas::Allreduce::Long<T_NI> allreduce_pp_type;
  typedef xlpgas::Allgather<T_NI>  allgather_pp_type;
  typedef xlpgas::Alltoall<T_NI>   alltoall_pp_type;
};

}//end namespace
#endif
