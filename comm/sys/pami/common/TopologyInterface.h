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
/// \file common/TopologyInterface.h
/// \brief PAMI topology implementation.
///

#ifndef __common_TopologyInterface_h__
#define __common_TopologyInterface_h__

#include <stdlib.h>
#include <string.h>

#include <pami.h>
#include "common/MultisendInterface.h"

namespace PAMI
{
  namespace Interface
  {
    template <class T_Topology>
      class Topology
    {
    public:
      /// \brief default constructor (PAMI_EMPTY_TOPOLOGY)
      ///
      inline Topology() {}
      /// \brief rectangular segment with torus (PAMI_COORD_TOPOLOGY)
      ///
      /// Assumes no torus links if no 'tl' param.
      ///
      /// \param[in] ll lower-left coordinate
      /// \param[in] ur upper-right coordinate
      /// \param[in] tl optional, torus links flags
      ///
      inline Topology(pami_coord_t *ll, pami_coord_t *ur,
                      unsigned char *tl = NULL) {}

      /// \brief axial neighborhood constructor(PAMI_AXIAL_TOPOLOGY)
      ///
      /// Define a set of axes from a reference task and the
      /// enclosing rectangular seqment.  The neighborhood is all
      /// ranks on those axes.  Optionally, define each dimension
      /// as being a torus (and a direction) or not (default).
      ///
      /// Assumes no torus links if no 'tl' param.
      ///
      /// \param[in] ll lower-left coordinate
      /// \param[in] ur upper-right coordinate
      /// \param[in] ref reference rank
      /// \param[in] tl optional, torus links flags
      ///
      inline Topology(pami_coord_t *ll,
                      pami_coord_t *ur,
                      pami_coord_t *ref,
                      unsigned char *tl = NULL) {}

      /// \brief single rank constructor (PAMI_SINGLE_TOPOLOGY)
      ///
      /// \param[in] rank The rank
      ///
      inline Topology(pami_task_t rank) {}

      /// \brief rank range constructor (PAMI_RANGE_TOPOLOGY)
      ///
      /// \param[in] rank0  first rank in range
      /// \param[in] rankn  last rank in range
      ///
      inline Topology(pami_task_t rank0, pami_task_t rankn) {}

      /// \brief rank list constructor (PAMI_LIST_TOPOLOGY)
      ///
      /// caller must not free ranks[]!
      ///
      /// \param[in] ranks  array of ranks
      /// \param[in] nranks size of array
      ///
      /// \todo create destructor to free list, or establish rules
      ///
      inline Topology(pami_task_t *ranks, size_t nranks) {}

      /// \brief accessor for size of a Topology object
      /// \return size of PAMI::Topology
      inline unsigned size_of();

      /// \brief number of ranks in topology
      /// \return number of ranks
      inline size_t size();

      /// \brief type of topology
      /// \return topology type
      inline pami_topology_type_t type();

      /// \brief Nth rank in topology
      ///
      /// \param[in] ix Which rank to select
      /// \return Nth rank or (size_t)-1 if does not exist
      ///
      inline pami_task_t index2Rank(size_t ix);

      /// \brief Nth permuted index in topology
      ///
      /// \param[in] ix	Which index to select
      /// \return	Nth permuted index or (size_t)-1 if does not exist
      ///
      inline size_t index2PermutedIndex(size_t index);

      /// \brief determine index of rank in topology
      ///
      /// This is the inverse function to index2Rank(ix) above.
      ///
      /// \param[in] rank Which rank to get index for
      /// \return index of rank (rank(ix) == rank) or (size_t)-1
      ///
      inline size_t rank2Index(pami_task_t rank);

      /// \brief return range
      ///
      /// \param[out] first Where to put first rank in range
      /// \param[out] last  Where to put last rank in range
      /// \return PAMI_SUCCESS, or PAMI_UNIMPL if not a range topology
      ///
      inline pami_result_t rankRange(pami_task_t *first, pami_task_t *last);


      /// \brief return rank list
      ///
      /// \param[out] list  pointer to list stored here
      /// \return PAMI_SUCCESS, or PAMI_UNIMPL if not a list topology
      ///
      inline pami_result_t rankList(pami_task_t **list);

      /// \brief return internal list (rank or endpoint)
      ///
      /// \param[out] list  pointer to list stored here
      /// \return PAMI_SUCCESS, or PAMI_UNIMPL if not a list topology
      ///
      inline pami_result_t list(void **list);

      /// \brief return axial neighborhood
      ///
      /// Warning! This returns pointers to the Topology internals!
      /// This can result in corruption of a topology if mis-used.
      ///
      /// \param[in] ll lower-left coordinate
      /// \param[in] ur upper-right coordinate
      /// \param[in] ref reference rank
      /// \param[in] tl  torus links flags
      ///
      /// \return PAMI_SUCCESS, or PAMI_UNIMPL if not an axial topology
      ///
      inline pami_result_t axial(pami_coord_t **ll, pami_coord_t **ur,
                                  pami_coord_t **ref,
                                  unsigned char **tl);

      /// \brief return axial neighborhood
      ///
      /// This method copies data to callers buffers. It is safer
      /// as the caller cannot directly modify the topology.
      ///
      /// \param[in] ll lower-left coordinate
      /// \param[in] ur upper-right coordinate
      /// \param[in] ref  reference rank
      /// \param[in] tl torus links flags
      ///
      /// \return PAMI_SUCCESS, or PAMI_UNIMPL if not an axial topology
      ///
      inline pami_result_t axial(pami_coord_t *ll, pami_coord_t *ur,
                                  pami_coord_t *ref,
                                  unsigned char *tl);

      /// \brief return rectangular segment coordinates
      ///
      /// Warning! This returns pointers to the Topology internals!
      /// This can result in corruption of a topology if mis-used.
      ///
      /// \param[out] ll  lower-left coord pointer storage
      /// \param[out] ur  upper-right coord pointer storage
      /// \param[out] tl  optional, torus links flags
      /// \return PAMI_SUCCESS, or PAMI_UNIMPL if not a coord topology
      ///
      inline pami_result_t rectSeg(pami_coord_t **ll, pami_coord_t **ur,
                                  unsigned char **tl = NULL);

      /// \brief return rectangular segment coordinates
      ///
      /// This method copies data to callers buffers. It is safer
      /// as the caller cannot directly modify the topology.
      ///
      /// \param[out] ll  lower-left coord pointer storage
      /// \param[out] ur  upper-right coord pointer storage
      /// \param[out] tl  optional, torus links flags
      /// \return PAMI_SUCCESS, or PAMI_UNIMPL if not a coord topology
      ///
      inline pami_result_t rectSeg(pami_coord_t *ll, pami_coord_t *ur,
                                  unsigned char *tl = NULL);

      /// \brief does topology consist entirely of ranks local to eachother
      ///
      /// \return boolean indicating locality of ranks
      ///
      inline bool isLocal();

      /// \brief does topology consist entirely of ranks local to self
      ///
      /// \return boolean indicating locality of ranks
      ///
      inline bool isLocalToMe();

      /// \brief does topology consist entirely of ranks that do not share nodes
      ///
      /// \return boolean indicating locality of ranks
      ///
      inline bool isGlobal();

      /// \brief is topology a rectangular segment
      ///
      /// \return boolean indicating rect seg topo
      ///
      inline bool isRectSeg();

      /// \brief extract Nth dimensions from coord topology
      ///
      /// \param[in] n  Which dim to extract
      /// \param[out] c0  lower value for dim range
      /// \param[out] cn  upper value for dim range
      /// \param[out] tl  optional, torus link flag
      /// \return PAMI_SUCCESS, or PAMI_UNIMPL if not a coord/axial topology
      ///
      inline pami_result_t getNthDims(unsigned n, unsigned *c0, unsigned *cn,
                             unsigned char *tl = NULL);

      /// \brief is rank in topology
      ///
      /// \param[in] rank Rank to test
      /// \return boolean indicating rank is in topology
      ///
      inline bool isRankMember(pami_task_t rank);

      /// \brief is endpoint in topology
      ///
      /// \param[in] endpoint Endpoint to test
      /// \return boolean indicating endpoint is in topology
      ///
      inline bool isEndpointMember(pami_endpoint_t endpoint);

      /// \brief is this topology a single offset (n)
      ///
      /// \param[in] slice Context offset
      /// \return boolean indicating context is single context n topology
      ///
      inline bool isContextOffset(size_t slice);

      ///
      /// \param[in] c0 Coord to test
      /// \return boolean indicating coord is a member of topology
      ///
      inline bool isCoordMember(pami_coord_t *c0);

      /// \brief create topology of ranks local to self
      ///
      /// \param[out] _new  Where to build topology
      ///
      inline void subTopologyLocalToMe(T_Topology *_new);

      /// \brief create topology from all Nth ranks globally
      ///
      /// \param[out] _new  Where to build topology
      /// \param[in] n  Which local rank to select on each node
      ///
      inline void subTopologyNthGlobal(T_Topology *_new, int n);

      /// \brief reduce dimensions of topology (cube -> plane, etc)
      ///
      /// The 'fmt' param is a pattern indicating which dimensions
      /// to squash, and what coord to squash into. A dim in 'fmt'
      /// having "-1" will be preserved, while all others will be squashed
      /// into a dimension of size 1 having the value specified.
      ///
      /// \param[out] _new  where to build new topology
      /// \param[in] fmt  how to reduce dimensions
      ///
      inline void subTopologyReduceDims(T_Topology *_new, pami_coord_t *fmt);

      /// \brief Return list of ranks representing contents of topology
      ///
      /// This always returns a list regardless of topology type.
      /// Caller must allocate space for list, and determine an
      /// appropriate size for that space. Note, there might be a
      /// number larger than 'max' returned in 'nranks', but there
      /// are never more than 'max' ranks put into the array.
      /// If the caller sees that 'nranks' exceeds 'max' then it
      /// should assume it did not get the whole list, and could
      /// allocate a larger array and try again.
      ///
      /// \param[in] max  size of caller-allocated array
      /// \param[out] ranks array where rank list is placed
      /// \param[out] nranks  actual number of ranks put into array
      ///
      inline void getRankList(size_t max, pami_task_t *ranks, size_t *nranks);

      /// \brief check if rank range or list can be converted to rectangle
      ///
      /// Since a rectangular segment is consider the optimal state, no
      /// other analysis is done. A PAMI_SINGLE_TOPOLOGY cannot be optimized,
      /// either. Optimization levels:
      ///
      /// PAMI_SINGLE_TOPOLOGY (most)
      /// PAMI_COORD_TOPOLOGY
      /// PAMI_RANGE_TOPOLOGY
      /// PAMI_LIST_TOPOLOGY (least)
      ///
      /// \return 'true' if topology was changed
      ///
      inline bool analyzeTopology();

      /// \brief check if topology can be converted to type
      ///
      /// Does not differentiate between invalid conversions and
      /// 'null' conversions (same type).
      ///
      /// \param[in] new_type Topology type to try and convert into
      /// \return 'true' if topology was changed
      ///
      inline bool convertTopology(pami_topology_type_t new_type);

      /// \brief produce the union of two topologies
      ///
      /// produces: _new = this .U. other
      ///
      /// \param[out] _new  New topology created there
      /// \param[in] other  The other topology
      ///
      inline void unionTopology(T_Topology *_new, T_Topology *other);

      /// \brief produce the intersection of two topologies
      ///
      /// produces: _new = this ./\. other
      ///
      /// \param[out] _new  New topology created there
      /// \param[in] other  The other topology
      ///
      inline void intersectTopology(T_Topology *_new, T_Topology *other);

      /// \brief produce the difference of two topologies
      ///
      /// produces: _new = this .-. other
      ///
      /// \param[out] _new  New topology created there
      /// \param[in] other  The other topology
      ///
      inline void subtractTopology(T_Topology *_new, T_Topology *other);


      /// Associate a client with the topology
      void setClient(pami_client_t);

      ///returns an endpoint corresponding to the given index
      pami_endpoint_t index2Endpoint(size_t ordinal);

      ///returns the index corresponding to the given endpoint
      size_t endpoint2Index(const pami_endpoint_t& ep);

      ///returns an endpoint corresponding to the given rank and offset index
      pami_endpoint_t rankContext2Endpoint(pami_task_t task, size_t offset);
    }; // end class PAMI::Interface::Topology

    template <class T_Topology>
      unsigned Topology<T_Topology>::size_of()
    {
      return static_cast<T_Topology*>(this)->size_of_impl();
    }

    template <class T_Topology>
      size_t Topology<T_Topology>::size()
    {
      return static_cast<T_Topology*>(this)->size_impl();
    }

    template <class T_Topology>
      pami_topology_type_t Topology<T_Topology>::type()
    {
      return static_cast<T_Topology*>(this)->type_impl();
    }

   template <class T_Topology>
      pami_task_t Topology<T_Topology>::index2Rank(size_t ix)
    {
      return static_cast<T_Topology*>(this)->index2Rank_impl(ix);
    }

   template <class T_Topology>
      size_t Topology<T_Topology>::index2PermutedIndex(size_t ix)
    {
      return static_cast<T_Topology*>(this)->index2PermutedIndex_impl(ix);
    }

    template <class T_Topology>
      size_t Topology<T_Topology>::rank2Index(pami_task_t rank)
    {
      return static_cast<T_Topology*>(this)->rank2Index_impl(rank);
    }

    template <class T_Topology>
      pami_result_t Topology<T_Topology>::rankRange(pami_task_t *first, pami_task_t *last)
    {
      return static_cast<T_Topology*>(this)->rankRange_impl(first,last);
    }

    template <class T_Topology>
      pami_result_t Topology<T_Topology>::rankList(pami_task_t **list)
    {
      return static_cast<T_Topology*>(this)->rankList_impl(list);
    }

    template <class T_Topology>
      pami_result_t Topology<T_Topology>::list(void **list)
    {
      return static_cast<T_Topology*>(this)->list_impl(list);
    }

    template <class T_Topology>
      pami_result_t Topology<T_Topology>::axial(pami_coord_t **ll, pami_coord_t **ur,
                                               pami_coord_t **ref,
                                               unsigned char **tl)
    {
      return static_cast<T_Topology*>(this)->axial_impl(ll,ur,ref,tl);
    }

    template <class T_Topology>
      pami_result_t Topology<T_Topology>::axial(pami_coord_t *ll, pami_coord_t *ur,
                                               pami_coord_t *ref,
                                               unsigned char *tl)
    {
      return static_cast<T_Topology*>(this)->axial_impl(ll,ur,ref,tl);
    }

    template <class T_Topology>
      pami_result_t Topology<T_Topology>::rectSeg(pami_coord_t **ll, pami_coord_t **ur,
                                                 unsigned char **tl)
    {
      return static_cast<T_Topology*>(this)->rectSeg_impl(ll,ur,tl);
    }

    template <class T_Topology>
      pami_result_t Topology<T_Topology>::rectSeg(pami_coord_t *ll, pami_coord_t *ur,
                                                 unsigned char *tl)
    {
      return static_cast<T_Topology*>(this)->rectSeg_impl(ll, ur, tl);
    }

    template <class T_Topology>
      bool Topology<T_Topology>::isLocal()
    {
      return static_cast<T_Topology*>(this)->isLocal_impl();
    }

    template <class T_Topology>
      bool Topology<T_Topology>::isLocalToMe()
    {
      return static_cast<T_Topology*>(this)->isLocalToMe_impl();
    }

    template <class T_Topology>
      bool Topology<T_Topology>::isGlobal()
    {
      return static_cast<T_Topology*>(this)->isGlobal_impl();
    }

    template <class T_Topology>
      bool Topology<T_Topology>::isRectSeg()
    {
      return static_cast<T_Topology*>(this)->isRectSeg_impl();
    }

    template <class T_Topology>
      pami_result_t Topology<T_Topology>::getNthDims(unsigned n, unsigned *c0, unsigned *cn,
                                            unsigned char *tl)
    {
      return static_cast<T_Topology*>(this)->getNthDims_impl(n,c0,cn,tl);
    }

    template <class T_Topology>
      bool Topology<T_Topology>::isRankMember(pami_task_t rank)
    {
      return static_cast<T_Topology*>(this)->isRankMember_impl(rank);
    }

    template <class T_Topology>
      bool Topology<T_Topology>::isEndpointMember(pami_endpoint_t endpoint)
    {
      return static_cast<T_Topology*>(this)->isEndpointMember_impl(endpoint);
    }

    template <class T_Topology>
      bool Topology<T_Topology>::isContextOffset(size_t slice)
    {
      return static_cast<T_Topology*>(this)->isContextOffset_impl(slice);
    }

    template <class T_Topology>
      bool Topology<T_Topology>::isCoordMember(pami_coord_t *c0)
    {
      return static_cast<T_Topology*>(this)->isCoordMember_impl(c0);
    }

    template <class T_Topology>
      void Topology<T_Topology>::subTopologyLocalToMe(T_Topology *_new)
    {
      return static_cast<T_Topology*>(this)->subTopologyLocalToMe_impl(_new);
    }

    template <class T_Topology>
      void Topology<T_Topology>::subTopologyNthGlobal(T_Topology *_new, int n)
    {
      return static_cast<T_Topology*>(this)->subTopologyNthGlobal_impl(_new, n);
    }

    template <class T_Topology>
      void Topology<T_Topology>::subTopologyReduceDims(T_Topology *_new, pami_coord_t *fmt)
    {
      return static_cast<T_Topology*>(this)->subTopologyReduceDims_impl(_new, fmt);
    }

    template <class T_Topology>
      void Topology<T_Topology>::getRankList(size_t max, pami_task_t *ranks, size_t *nranks)
    {
      return static_cast<T_Topology*>(this)->getRankList_impl(max,ranks,nranks);
    }

    template <class T_Topology>
      bool Topology<T_Topology>::analyzeTopology()
    {
      return static_cast<T_Topology*>(this)->analyzeTopology_impl();
    }

    template <class T_Topology>
      bool Topology<T_Topology>::convertTopology(pami_topology_type_t new_type)
    {
      return static_cast<T_Topology*>(this)->convertTopology_impl(new_type);
    }

    template <class T_Topology>
      void Topology<T_Topology>::unionTopology(T_Topology *_new, T_Topology *other)
    {
      return static_cast<T_Topology*>(this)->unionTopology_impl(_new, other);
    }

    template <class T_Topology>
      void Topology<T_Topology>::intersectTopology(T_Topology *_new, T_Topology *other)
    {
      return static_cast<T_Topology*>(this)->intersectTopology_impl(_new, other);
    }

    template <class T_Topology>
      void Topology<T_Topology>::subtractTopology(T_Topology *_new, T_Topology *other)
    {
      return static_cast<T_Topology*>(this)->subtractTopology_impl(_new, other);
    }

    template <class T_Topology>
      void Topology<T_Topology>::setClient(pami_client_t pc)
    {
      static_cast<T_Topology*>(this)->setClient_impl(pc);
    }

    template <class T_Topology>
    pami_endpoint_t Topology<T_Topology>::index2Endpoint(size_t ordinal)
    {
      return static_cast<T_Topology*>(this)->index2Endpoint_impl(ordinal);
    }

    template <class T_Topology>
    size_t Topology<T_Topology>::endpoint2Index(const pami_endpoint_t& ep)
    {
      return static_cast<T_Topology*>(this)->endpoint2Index_impl(ep);
    }

    template <class T_Topology>
    pami_endpoint_t Topology<T_Topology>::rankContext2Endpoint(pami_task_t task, size_t offset)
    {
      return static_cast<T_Topology*>(this)->rankContext2Endpoint_impl(task,offset);
    }

  }; // end namespace Interface
}; // end namespace PAMI

#endif // __pami_topology_h__
