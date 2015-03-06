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

#ifndef __common_default_Topology_h__
#define __common_default_Topology_h__

////////////////////////////////////////////////////////////////////////
/// \file common/default/Topology.h
///
/// \brief Coordinate/Address Topology Class
///
/// Some Basic Assertions:
///	* We never have a PAMI_COORD_TOPOLOGY unless we also have PAMI_N_TORUS_NETWORK
///
////////////////////////////////////////////////////////////////////////

#include "common/TopologyInterface.h"
#include "Mapping.h"
#include "components/memory/MemoryManager.h"
#include "util/common.h"

#define net_coord(n)	u.n_torus.coords[n]

// returns pointer to pami_coord_t
#define MY_COORDS	&my_coords

#define RANK2COORDS(rank, coords)                               \
  mapping->task2network(rank, coords, PAMI_N_TORUS_NETWORK)

static pami_network __dummy_net; // never really used

#define COORDS2RANK(coords, rank)                       \
  mapping->network2task(coords, rank, &__dummy_net)

#define IS_LOCAL_PEER(rank)	mapping->isPeer(mapping->task(), rank)

namespace PAMI
{
  class tag_eplist
  {
  };

  class Topology : public Interface::Topology<PAMI::Topology>
  {
    static PAMI::Mapping *mapping;
    static pami_coord_t my_coords;

#define b0000	0
#define b0001	1
#define b0010	2
#define b0011	3
#define b0100	4
#define b0101	5
#define b0110	6
#define b0111	7
#define b1000	8
#define b1001	9
#define b1010	10
#define b1011	11
#define b1100	12
#define b1101	13
#define b1110	14
#define b1111	15

    typedef struct
    {
      pami_coord_t _llcorner; ///< lower-left coord
      pami_coord_t _urcorner; ///< upper-right coord
      unsigned char _istorus[PAMI_MAX_DIMS]; ///< existence of torus links
    }rectseg;

    /// \brief how a topology represents each type
    union topology_u
    {
      pami_task_t _rank;   ///< PAMI_SINGLE_TOPOLOGY - the rank

      struct            ///< PAMI_RANGE_TOPOLOGY
      {
        pami_task_t _first;///< first rank in range
        pami_task_t _last;  ///< last rank in range
      } _rankrange;

      pami_task_t *_ranklist;  ///< PAMI_LIST_TOPOLOGY - the rank array

      pami_endpoint_t *_eplist; ///< PAMI_EPLIST_TOPOLOGY

      rectseg _rectseg;   ///< PAMI_COORD_TOPOLOGY

      struct              ///< PAMI_AXIAL_TOPOLOGY
      {
        rectseg _rect;    ///< the rectangle segment containing the axial
        pami_coord_t _ref_task;///< the task where axes cross
      } _axial;
    };
    /// \brief these defines simplify access to above union, and
    ///        also isolate the code below from changes above.
#define topo_rank	  __topo._rank
#define topo_first	  __topo._rankrange._first
#define topo_last	  __topo._rankrange._last
#define topo_range(n)	  __topo._rankrange._first + n
#define topo_ranklist 	  __topo._ranklist
#define topo_list(n)	  __topo._ranklist[n]
#define topo_endplist	  __topo._eplist
#define topo_eplist(n)	  __topo._eplist[n]
#define topo_llcoord	  __topo._rectseg._llcorner
#define topo_urcoord	  __topo._rectseg._urcorner
#define topo_istorus	  __topo._rectseg._istorus
#define topo_lldim(n)	  __topo._rectseg._llcorner.net_coord(n)
#define topo_urdim(n)	  __topo._rectseg._urcorner.net_coord(n)
#define topo_hastorus(n)  __topo._rectseg._istorus[n]
#define topo_axial        __topo._axial
#define topo_axial_center __topo._axial._ref_task
#define topo_axial_llcoord	  __topo._axial._rect._llcorner
#define topo_axial_urcoord	  __topo._axial._rect._urcorner
#define topo_axial_istorus	  __topo._axial._rect._istorus
#define topo_axial_lldim(n)	  __topo._axial._rect._llcorner.net_coord(n)
#define topo_axial_urdim(n)	  __topo._axial._rect._urcorner.net_coord(n)
#define topo_axial_hastorus(n)  __topo._axial._rect._istorus[n]

    /// \brief set the torus link flags 
    ///
    /// \param[in] isTorus - torus links flags
    ///
    inline void set_istorus(pami_coord_t ll, pami_coord_t ur, unsigned char isTorus[PAMI_MAX_DIMS])
    {
      mapping->torusInformation(ll, ur, isTorus);
    }

    /// \brief are the two coords located on the same node
    ///
    /// \param[in] c0	first coordinate
    /// \param[in] c1	second coordinate
    /// \return	boolean indicate locality of two coords
    ///
    bool __isLocalCoord(const pami_coord_t *c0,
                        const pami_coord_t *c1)
    {
      unsigned x;
      for(x = 0; x < mapping->torusDims(); ++x)
      {
        if(c0->net_coord(x) != c1->net_coord(x))
        {
          return false;
        }
      }
      return true;
    }

    /// \brief is the coordinate in this topology
    ///
    /// \param[in] c0	coordinate
    /// \param[in] ndims	number of dimensions that are significant
    /// \return	boolean indicating if the coordinate lies in this
    ///		topology, according to significant dimensions.
    ///
    bool __isMemberCoord(const pami_coord_t *c0, unsigned ndims)
    {
      unsigned x;
      for(x = 0; x < ndims; ++x)
      {
        if(c0->net_coord(x) < topo_lldim(x) ||
           c0->net_coord(x) > topo_urdim(x))
        {
          return false;
        }
      }
      return true;
    }

    /// \brief create topology of only ranks local to self
    ///
    /// \param[out] _new	place to construct new topology
    /// \return	nothing, but _new may be PAMI_EMPTY_TOPOLOGY
    ///
    void __subTopologyLocalToMe(PAMI::Topology *_new)
    {
      if(likely(__type == PAMI_COORD_TOPOLOGY))
      {
        if(__isMemberCoord(MY_COORDS,
                           mapping->torusDims()))
        {
          _new->__type = PAMI_COORD_TOPOLOGY;
          _new->topo_llcoord = *MY_COORDS;
          _new->topo_urcoord = *MY_COORDS;
          // might be able to get better torus info from mapping
          memset(_new->topo_istorus, 0, mapping->torusDims());
          size_t s = 1;
          unsigned x;
          for(x = mapping->torusDims(); x < mapping->globalDims(); ++x)
          {
            _new->topo_lldim(x) = topo_lldim(x);
            _new->topo_urdim(x) = topo_urdim(x);
            _new->topo_hastorus(x) = topo_hastorus(x);
            s *= (topo_urdim(x) - topo_lldim(x) + 1);
          }
          _new->__size = s;
          _new->__free_ranklist =false;
          return;
        }
      }
      else
      {
        if(__type == PAMI_AXIAL_TOPOLOGY)
        {
          if(__isMemberCoord(MY_COORDS,
                             mapping->torusDims()))
          {
            _new->__type = PAMI_AXIAL_TOPOLOGY;
            // this isn't quite right.. am I even in the axial? Are all local
            // processes always on the axis?
            _new->topo_axial_llcoord = *MY_COORDS; // topo_axial_center;
            _new->topo_axial_urcoord = *MY_COORDS; // topo_axial_center;
            // might be able to get better torus info from mapping
            memset(_new->topo_axial_istorus, 0, mapping->torusDims());
            size_t s = 0;
            unsigned x;
            for(x = mapping->torusDims(); x < mapping->globalDims(); ++x)
            {
              _new->topo_axial_lldim(x) = topo_axial_lldim(x);
              _new->topo_axial_urdim(x) = topo_axial_urdim(x);
              _new->topo_axial_hastorus(x) = topo_axial_hastorus(x);
              if(!s) s = 1;
              s *= (topo_urdim(x) - topo_lldim(x) + 1);
            }
            if(!s) _new->__type = PAMI_EMPTY_TOPOLOGY;
            _new->__size = s;
            _new->__free_ranklist =false;
            return;
          }
        }
        else
          // the hard way...
          if(__type == PAMI_SINGLE_TOPOLOGY)
        {
          if(IS_LOCAL_PEER(topo_rank))
          {
            *_new = *this;
            return;
          }
        }
        else
        {
          size_t s = 0;
          size_t z;
          mapping->nodePeers(z);
          pami_task_t *rl=NULL;
          pami_result_t rc;

          //if endpoint list topology; here we generate the list of endpoints
          pami_endpoint_t *epl=NULL;

          rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                             (void **)&rl, 0, z * sizeof(*rl));
          PAMI_assertf(rc == PAMI_SUCCESS, "temp ranklist[%zd] alloc failed", z);
          pami_task_t *rp = rl;
          if(__type == PAMI_RANGE_TOPOLOGY)
          {
            pami_task_t r;
            for(r = topo_first; r <= topo_last; ++r)
            {
              if(IS_LOCAL_PEER(r))
              {
                ++s;
                *rp++ = r;
              }
            }
          }
          else if(__type == PAMI_LIST_TOPOLOGY)
          {
            unsigned i;
            for(i = 0; i < __size; ++i)
            {
              if(IS_LOCAL_PEER(topo_list(i)))
              {
                ++s;
                *rp++ = topo_list(i);
              }
            }
          }
          else if( __type == PAMI_EPLIST_TOPOLOGY)
          {
            // for endpoint list topologies; it uses the locality of the
            // tasks of the endpoints;
            z = 0;
            //first count how many endpoints
            for(size_t i = 0; i < __size; ++i)
            {
              pami_task_t    task; size_t          offset;
              PAMI_ENDPOINT_INFO(topo_eplist(i),task,offset);
              if(IS_LOCAL_PEER(task))
              {
                ++z;
              }
            }
            //alloc space for the new ep list
            rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                               (void **)&epl, 0, z * sizeof(*epl));
            pami_endpoint_t *epl_iter=epl;
            PAMI_assertf(rc == PAMI_SUCCESS, "temp eplist[%zd] alloc failed", z);
            //pami_endpoint_t *ep_iter = epl;
            for(size_t i = 0; i < __size; ++i)
            {
              pami_task_t    task; size_t          offset;
              PAMI_ENDPOINT_INFO(topo_eplist(i),task,offset);
              if(IS_LOCAL_PEER(task))
              {
                *epl_iter++ = topo_eplist(i);
              }
            }
          }//end build endpoints
          if(epl != NULL)
          {
            //build endpoints topology
            _new->__type = PAMI_EPLIST_TOPOLOGY;
            _new->__size          = z;
            _new->__free_ranklist = true;
            _new->topo_endplist   = epl;
            _new->__offset        = 0;
            _new->__all_contexts  = false;
            return;
          }
          if(s)
          {
            // convert "rl" to range if possible.
            // ...or even coords?
            _new->__type = PAMI_LIST_TOPOLOGY;
            _new->__size = s;
            _new->__free_ranklist =true;
            _new->topo_ranklist = rl;
            //added for endpoints/multicontext
            _new->__offset = this->__offset;             //endpoints per task
            _new->__all_contexts = this->__all_contexts;
            return;
          }
          else
          {
            PAMI::Memory::MemoryManager::heap_mm->free(rl);
          }
        }
      }
      _new->__type = PAMI_EMPTY_TOPOLOGY;
      _new->__size = 0;
      _new->__free_ranklist =false;
    }

    /// \brief create topology of all Nth ranks on all nodes
    ///
    /// This implemention is only for PAMI_COORD_TOPOLOGY.
    /// Essentially a horizontal slice of ranks. The resulting
    /// topology will have no two ranks sharing the same node.
    ///
    /// \param[out] _new	space to construct new topology
    /// \param[in] n	which local rank to select
    /// \return	nothing, but _new may be PAMI_EMPTY_TOPOLOGY
    ///
    void __subTopologyNthGlobal(PAMI::Topology *_new, int n)
    {
      // What order do we sequence multiple "local" dimensions???
      *_new = *this;
      size_t s = __sizeRange(&topo_llcoord,
                             &topo_urcoord,
                             mapping->torusDims());
      unsigned x;
      for(x = mapping->torusDims(); x < mapping->globalDims(); ++x)
      {
        unsigned ll = topo_lldim(x);
        unsigned ur = topo_urdim(x);
        int nn = ur - ll + 1;
        if(likely(n < nn))
        {
          _new->topo_lldim(x) =
          _new->topo_urdim(x) = ll + n;
          n = 0;
        }
        else
        {
          _new->topo_lldim(x) =
          _new->topo_urdim(x) = ll + n % nn;
          n /= nn;
        }
      }
      if(n == 0) // no overflow on num local nodes...
      {
        _new->__size = s;
        _new->__free_ranklist =false;
        return;
      }
      _new->__type = PAMI_EMPTY_TOPOLOGY;
      _new->__size = 0;
      _new->__free_ranklist =false;
    }

    /// \brief Create new Rectangular Segment topology from subset of this
    ///
    /// preserve coord if "fmt" coord is "-1",
    /// otherwise squash coord into 1-dim at "fmt" coord.
    ///
    /// \param[out] _new	Storage for new topology
    /// \param[in] fmt	Coords to collapse/preserve
    ///
    void __subTopologyReduceDims(PAMI::Topology *_new, pami_coord_t *fmt)
    {
      *_new = *this;
      size_t s = 1;
      unsigned x;
      for(x = 0; x < mapping->globalDims(); ++x)
      {
        if(fmt->net_coord(x) == (size_t)-1)
        {
          _new->topo_lldim(x) = topo_lldim(x);
          _new->topo_urdim(x) = topo_urdim(x);
          _new->topo_hastorus(x) = topo_hastorus(x);
          s *= (topo_urdim(x) - topo_lldim(x) + 1);
        }
        else
        {
          PAMI_assert_debugf(fmt->net_coord(x) >= topo_lldim(x) &&
                             fmt->net_coord(x) <= topo_urdim(x), "selected coordinate is out of range\n");
          _new->topo_lldim(x) =
          _new->topo_urdim(x) = fmt->net_coord(x);
          _new->topo_hastorus(x) = 0;
        }
      }
      // cannot result in empty...
      _new->__size = s;
      _new->__free_ranklist =false;
    }

    /// \brief iterate to next coord, [0] is most-significant dim
    ///
    /// \param[in,out] c0	coordinate to "increment"
    /// \return	boolean indicating overflow (end of count)
    ///
    bool __nextCoord(pami_coord_t *c0, unsigned ndims)
    {
      unsigned x;
      for(x = ndims; x > 0;)
      {
        --x;
        c0->net_coord(x) += 1;
        if(c0->net_coord(x) <= topo_urdim(x))
        {
          return true;
        }
        c0->net_coord(x) = topo_lldim(x);
      }
      return false; // wrapped all dims - end of iteration
    }

    /// \brief initialize a coordinate range to a coord
    ///
    /// \param[out] ll	lower-left corner of range
    /// \param[out] ur	upper-right corner of range
    /// \param[in] c0	initial coordinate
    ///
    static void __initRange(pami_coord_t *ll, pami_coord_t *ur,
                            pami_coord_t *c0, unsigned ndims)
    {
      unsigned x;
      for(x = 0; x < ndims; ++x)
      {
        ur->net_coord(x) = ll->net_coord(x) = c0->net_coord(x);
      }
      ll->network = ur->network = PAMI_N_TORUS_NETWORK;
    }

    /// \brief compute size of rectangular segment
    ///
    /// \param[in] ll	lower-left corner of range
    /// \param[in] ur	upper-right corner of range
    /// \param[in] ndims	number of significant dimensions
    /// \return	size of rectangular segment
    ///
    static size_t __sizeRange(pami_coord_t *ll, pami_coord_t *ur,
                              unsigned ndims)
    {
      size_t s = 1;
      unsigned x;
      for(x = 0; x < ndims; ++x)
      {
        unsigned n = ur->net_coord(x) - ll->net_coord(x) + 1;
        s *= n;
      }
      return s;
    }

    /// \brief expand range to include coord
    ///
    /// \param[in,out] ll	lower-left corner of range
    /// \param[in,out] ur	upper-right corner of range
    /// \param[in] c0	coord to be included
    ///
    static void __bumpRange(pami_coord_t *ll, pami_coord_t *ur,
                            pami_coord_t *c0, unsigned ndims)
    {
      unsigned x;
      for(x = 0; x < ndims; ++x)
      {
        if(c0->net_coord(x) < ll->net_coord(x))
        {
          ll->net_coord(x) = c0->net_coord(x);
        }
        else if(c0->net_coord(x) > ur->net_coord(x))
        {
          ur->net_coord(x) = c0->net_coord(x);
        }
      }
    }

    /// \brief Analyze a Rank Range to see if it is a rectangular segment.
    ///
    /// \return 'true' means "this" was altered!
    inline void no_unused_warning(pami_coord_t *c)
    {
      c->network = PAMI_DEFAULT_NETWORK;
      c->u.no_network = 0;
    }
    bool __analyzeCoordsRange()
    {
      pami_result_t rc;
      pami_coord_t ll, ur;
      pami_coord_t c0;
      no_unused_warning(&c0);
      pami_task_t r = topo_first;
      rc = RANK2COORDS(r, &c0);
      __initRange(&ll, &ur, &c0, mapping->globalDims());
      for(r += 1; r <= topo_last; ++r)
      {
        RANK2COORDS(r, &c0);
        __bumpRange(&ll, &ur, &c0, mapping->globalDims());
      }
      size_t s = __sizeRange(&ll, &ur, mapping->globalDims());
      if(s == __size)
      {
        __type = PAMI_COORD_TOPOLOGY;
        topo_llcoord = ll;
        topo_urcoord = ur;
        set_istorus(topo_llcoord,topo_urcoord,topo_istorus);
        return true;
      }
      return false;
    }

    /// \brief Analyze a Rank List to see if it is a rectangular segment.
    ///
    /// Assumes that rank list contains no duplicates.
    ///
    /// \return 'true' means "this" was altered!
    bool __analyzeCoordsList()
    {
      pami_result_t rc;
      pami_coord_t ll, ur;
      pami_coord_t c0;
      no_unused_warning(&c0);
      unsigned i = 0;
      rc = RANK2COORDS(topo_list(i), &c0);
      __initRange(&ll, &ur, &c0, mapping->globalDims());
      for(i += 1; i < __size; ++i)
      {
        RANK2COORDS(topo_list(i), &c0);
        __bumpRange(&ll, &ur, &c0, mapping->globalDims());
      }
      size_t s = __sizeRange(&ll, &ur, mapping->globalDims());
      if(s == __size)
      {
        // LIST -> COORDS, drop ranklist and turn off flag
        if(__free_ranklist)
        {
          PAMI::Memory::MemoryManager::heap_mm->free(topo_ranklist);
          __free_ranklist = false;
        }
        __type = PAMI_COORD_TOPOLOGY;
        topo_llcoord = ll;
        topo_urcoord = ur;
        set_istorus(topo_llcoord,topo_urcoord,topo_istorus);  
        return true;
      }
      return false;
    }

    /// \brief Analyze a Rank List to see if it is a range.
    ///
    /// Assumes that rank list contains no duplicates.
    ///
    /// \return 'true' means "this" was altered!
    bool __analyzeRangeList()
    {
      pami_task_t min, max;
      unsigned i = 0;
      min = max = topo_list(i);
      for(i += 1; i < __size; ++i)
      {
        if(topo_list(i) < min)
        {
          min = topo_list(i);
        }
        else if(topo_list(i) > max)
        {
          max = topo_list(i);
        }
      }
      if(__size == max - min + 1)
      {
        // LIST -> RANGE, drop ranklist and turn off flag
        if(__free_ranklist)
        {
          PAMI::Memory::MemoryManager::heap_mm->free(topo_ranklist);
          __free_ranklist = false;
        }
        __type = PAMI_RANGE_TOPOLOGY;
        topo_first = min;
        topo_last = max;
        return true;
      }
      return false;
    }

    /// \brief compare coords for less-than
    ///
    /// checks if 'c0' is entirely less-than 'c1'
    ///
    /// \param[in] c0	Left-hand coord to test
    /// \param[in] c1	Right-hand coord to test
    /// \param[in] ndims	Number of significant dimensions
    /// \return	boolean indicating [c0] < [c1]
    ///
    static bool __coordLT(pami_coord_t *c0, pami_coord_t *c1,
                          unsigned ndims)
    {
      unsigned x;
      for(x = 0; x < ndims; ++x)
      {
        if(c0->net_coord(x) >= c1->net_coord(x))
        {
          return false;
        }
      }
      return true;
    }

    /// \brief return MAX of two coords
    ///
    /// _new = MAX(c0, c1)
    ///
    /// \param[in] _new	Result coord
    /// \param[in] c0	First coord
    /// \param[in] c1	Second coord
    /// \param[in] ndims	Number of dimension to use
    ///
    /// Leaves un-used dimensions uninitialized
    ///
    static void __coordMAX(pami_coord_t *_new,
                           pami_coord_t *c0, pami_coord_t *c1,
                           unsigned ndims)
    {
      unsigned x;
      for(x = 0; x < ndims; ++x)
      {
        _new->net_coord(x) = (c0->net_coord(x) > c1->net_coord(x) ?
                              c0->net_coord(x) : c1->net_coord(x));
      }
    }

    /// \brief return MIN of two coords
    ///
    /// _new = MIN(c0, c1)
    ///
    /// \param[in] _new	Result coord
    /// \param[in] c0	First coord
    /// \param[in] c1	Second coord
    /// \param[in] ndims	Number of dimension to use
    ///
    /// Leaves un-used dimensions uninitialized
    ///
    static void __coordMIN(pami_coord_t *_new,
                           pami_coord_t *c0, pami_coord_t *c1,
                           unsigned ndims)
    {
      unsigned x;
      for(x = 0; x < ndims; ++x)
      {
        _new->net_coord(x) = (c0->net_coord(x) < c1->net_coord(x) ?
                              c0->net_coord(x) : c1->net_coord(x));
      }
    }

  public:
    static void static_init(PAMI::Mapping *map)
    {
      COMPILE_TIME_ASSERT(sizeof(Topology) <= sizeof(pami_topology_t));
      mapping = map;
      RANK2COORDS(mapping->task(), &my_coords);
    }

    // hack on top of hack on top of hack.
    void finalize()
    {
      if(__free_ranklist)
      {
        PAMI::Memory::MemoryManager::heap_mm->free(topo_ranklist);
        topo_ranklist=NULL;
        __free_ranklist = false;
      }
    }


    ~Topology()
    {
      finalize();
    }

    /// \brief default constructor (PAMI_EMPTY_TOPOLOGY)
    ///
    Topology()
    {
      __type = PAMI_EMPTY_TOPOLOGY;
      __size = 0;
      __free_ranklist = false;
      __offset = 0;
      __all_contexts = false;
    }

    /// \brief rectangular segment with torus (PAMI_COORD_TOPOLOGY)
    ///
    /// Assumes no torus links if no 'tl' param.
    ///
    /// \param[in] ll	lower-left coordinate
    /// \param[in] ur	upper-right coordinate
    /// \param[in] tl	optional, torus links flags
    ///
    Topology(pami_coord_t *ll, pami_coord_t *ur,
             unsigned char *tl = NULL)
    {
      __type = PAMI_COORD_TOPOLOGY;
      topo_llcoord = *ll;
      topo_urcoord = *ur;
      topo_llcoord.network = topo_urcoord.network = PAMI_N_TORUS_NETWORK;
      if(tl)
      {
        memcpy(topo_istorus, tl, mapping->globalDims());
      }
      else
      {
        set_istorus(topo_llcoord,topo_urcoord,topo_istorus);  
      }
      __size = __sizeRange(ll, ur, mapping->globalDims());
      __free_ranklist =false;
      __offset = 0;
      __all_contexts = false;
    }

    /// \brief Construct axial neighborhood (PAMI_AXIAL_TOPOLOGY)
    ///
    /// Assumes no torus links if no 'tl' param.
    ///
    /// \param[in] ll	lower-left coordinate
    /// \param[in] ur	upper-right coordinate
    /// \param[in] ref  reference rank
    /// \param[in] tl	optional, torus links flags
    ///
    Topology(pami_coord_t *ll, pami_coord_t *ur, pami_coord_t *ref,
             unsigned char *tl = NULL)
    {
      int i;
      PAMI_assert(mapping->globalDims() <= PAMI_MAX_DIMS);

      __size = 1;

      __type = PAMI_AXIAL_TOPOLOGY;
      topo_axial_center  = *ref;
      topo_axial_llcoord = *ll;
      topo_axial_urcoord = *ur;

      topo_axial_llcoord.network = topo_axial_urcoord.network = PAMI_N_TORUS_NETWORK;

      if(tl)
      {
        memcpy(topo_axial_istorus, tl, mapping->globalDims());
      }
      else
      {
        set_istorus(topo_axial_llcoord,topo_axial_urcoord,topo_axial_istorus);  
      }

      for(i = 0; (size_t) i < mapping->globalDims(); i++)
      {
        __size += (mapping->torusSize(i) + topo_axial_urdim(i) - topo_axial_lldim(i)) % mapping->torusSize(i);
      }
      __free_ranklist =false;
      __offset = 0;
      __all_contexts = false;
    }
    /// \brief single rank constructor (PAMI_SINGLE_TOPOLOGY)
    ///
    /// \param[in] rank	The rank
    ///
    Topology(pami_task_t rank)
    {
      __type = PAMI_SINGLE_TOPOLOGY;
      __size = 1;
      __topo._rank = rank;
      __free_ranklist =false;
      __offset = 0;
      __all_contexts = false;
    }



    /// \brief rank range constructor (PAMI_RANGE_TOPOLOGY)
    ///
    /// \param[in] rank0       first rank in range
    /// \param[in] rankn       last rank in range
    ///
    Topology(pami_task_t rank0, pami_task_t rankn) {
      __type = PAMI_RANGE_TOPOLOGY;
      __size = rankn - rank0 + 1;
      __topo._rankrange._first = rank0;
      __topo._rankrange._last = rankn;
      __free_ranklist =false;
      __offset = 0;
      __all_contexts = false;
      // should we do this automatically, or let caller?
      // (void)__analyzeCoordsRange();
    }

    /// --------------------------------------------------------- ADDED FOR EP
    /// \brief rank range constructor (PAMI_EPRANGE_TOPOLOGY)
    ///
    /// \param[in] rank0	first rank in range
    /// \param[in] rankn	last rank in range
    /// \param[in] ncontexts	the number of contexts per task
    ///  This topology will consist of nranks*ncontexts endpoints;
    Topology(pami_task_t rank0, pami_task_t rankn, size_t context_offset, size_t ncontexts)
    {
      __type = PAMI_RANGE_TOPOLOGY;
      __size = rankn - rank0 + 1;
      __topo._rankrange._first = rank0;
      __topo._rankrange._last = rankn;
      __free_ranklist =false;

      if(context_offset==PAMI_ALL_CONTEXTS)
      {
        __all_contexts = true;//this is flagging that for all tasks in the topology
                              // we will have __offset contexts
        __offset = ncontexts;
      }
      else
      {
        __all_contexts = false;
        __offset = context_offset;
      }
    }

    /// \brief rank list constructor (PAMI_LIST_TOPOLOGY)
    ///
    /// caller must not free ranks[]!
    ///
    /// \param[in] ranks	array of ranks
    /// \param[in] nranks	size of array
    ///
    /// \todo create destructor to free list, or establish rules
    ///
    Topology(pami_task_t *ranks, size_t nranks, size_t context_offset=0, size_t ncontexts=1)
    {
      ////    FIXME  toproperly consider PAMI_ALL_CONTEXTS
      __type = PAMI_LIST_TOPOLOGY;
      __size = nranks;
      topo_ranklist = ranks;
      __free_ranklist =false;

      if(context_offset==PAMI_ALL_CONTEXTS)
      {
        __all_contexts = true;//this is flagging that for all tasks in the topology
                              // we will have __offset contexts
        __offset = ncontexts;
      }
      else
      {
        __all_contexts = false;
        __offset = context_offset;
      }
      // should we do this automatically, or let caller?
      // (void)__analyzeCoordsList();
    }

    /// ------------------------------------------------------- ADDED for EP LIST
    /// \brief rank list constructor (PAMI_LIST_TOPOLOGY)
    ///
    /// caller must not free eplist[]!
    ///
    /// \param[in] eplist	array of end points
    /// \param[in] nranks	size of array
    ///
    /// \todo create destructor to free list, or establish rules
    ///
    Topology(pami_endpoint_t *eps, size_t neps, tag_eplist tag)
    {
      (void)tag;
      __type = PAMI_EPLIST_TOPOLOGY;
      __size = neps;
      topo_endplist = eps;
      __free_ranklist =false;
      __offset = 0;
      __all_contexts = false;
    }

    /// \brief clone constructor (all)
    ///
    /// Creates a fully-independent copy of a topology (re-mallocs any storage)
    ///
    /// \param[in] topo   Topology to replicate
    ///
    Topology(Topology *topo)
    {
      memcpy(this, topo, sizeof(*topo));
      __free_ranklist = false;
      // right now, the only type that allocates additional storage is PAMI_LIST_TOPOLOGY
      if(topo->__type == PAMI_LIST_TOPOLOGY)
      {
        pami_result_t rc;
        rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                           (void **)&topo_ranklist, 0, __size * sizeof(*topo_ranklist));
        PAMI_assertf(rc == PAMI_SUCCESS, "ranklist[%zd] alloc failed", __size);
        memcpy(topo_ranklist, topo->topo_ranklist, __size * sizeof(*topo_ranklist));
        __free_ranklist =true;
      }
      if(topo->__type == PAMI_EPLIST_TOPOLOGY)
      {
        pami_result_t rc;
        rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                           (void **)&topo_endplist, 0, __size * sizeof(*topo_endplist));
        PAMI_assertf(rc == PAMI_SUCCESS, "eplist[%zd] alloc failed", __size);
        memcpy(topo_endplist, topo->topo_endplist, __size * sizeof(*topo_endplist));
        __free_ranklist =true;//though it is called ranklist th eeplist use the same flag to free mem
      }
    }
    /// \brief Copy constructor (all)
    ///
    /// Creates a fully-independent copy of a topology (re-mallocs any storage)
    ///
    /// \param[in] rtopo   Topology to replicate
    ///
    Topology(const Topology &rtopo):
    Interface::Topology<PAMI::Topology>()
    {
      const Topology *topo = &rtopo ;
      memcpy(this, topo, sizeof(*topo));
      __free_ranklist = false;
      // right now, the only type that allocates additional storage is PAMI_LIST_TOPOLOGY
      if(topo->__type == PAMI_LIST_TOPOLOGY)
      {
        pami_result_t rc;
        rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                           (void **)&topo_ranklist, 0, __size * sizeof(*topo_ranklist));
        PAMI_assertf(rc == PAMI_SUCCESS, "ranklist[%zd] alloc failed", __size);
        memcpy(topo_ranklist, topo->topo_ranklist, __size * sizeof(*topo_ranklist));
        __free_ranklist =true;
      }
      if(topo->__type == PAMI_EPLIST_TOPOLOGY)
      {
        pami_result_t rc;
        rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                           (void **)&topo_endplist, 0, __size * sizeof(*topo_endplist));
        PAMI_assertf(rc == PAMI_SUCCESS, "eplist[%zd] alloc failed", __size);
        memcpy(topo_endplist, topo->topo_endplist, __size * sizeof(*topo_endplist));
        __free_ranklist =true;//though it is called ranklist th eeplist use the same flag to free mem
      }
    }
    /// \brief Assignment operator (all)
    ///
    /// Creates a fully-independent copy of a topology (re-mallocs any storage)
    ///
    /// \param[in] rtopo   Topology to replicate
    ///
    Topology& operator= (const Topology &rtopo)
    {
      const Topology *topo = &rtopo ;
      memcpy(this, topo, sizeof(*topo));
      __free_ranklist = false;
      // right now, the only type that allocates additional storage is PAMI_LIST_TOPOLOGY
      if(topo->__type == PAMI_LIST_TOPOLOGY)
      {
        pami_result_t rc;
        rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                           (void **)&topo_ranklist, 0, __size * sizeof(*topo_ranklist));
        PAMI_assertf(rc == PAMI_SUCCESS, "ranklist[%zd] alloc failed", __size);
        memcpy(topo_ranklist, topo->topo_ranklist, __size * sizeof(*topo_ranklist));
        __free_ranklist =true;
      }
      if(topo->__type == PAMI_EPLIST_TOPOLOGY)
      {
        pami_result_t rc;
        rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                           (void **)&topo_endplist, 0, __size * sizeof(*topo_endplist));
        PAMI_assertf(rc == PAMI_SUCCESS, "eplist[%zd] alloc failed", __size);
        memcpy(topo_endplist, topo->topo_endplist, __size * sizeof(*topo_endplist));
        __free_ranklist =true;
      }
      return(*this);
    }

    /// \brief accessor for size of a Topology object
    /// \return	size of PAMI::Topology
    static unsigned size_of_impl()
    {
      return sizeof(Topology);
    }

    /// \brief number of ranks in topology
    /// \return	number of ranks
    size_t size_impl()
    {
      if(__all_contexts) return __size * __offset;
      else return __size;
    }

    /// \brief type of topology
    /// \return	topology type
    pami_topology_type_t type_impl()
    {
      return __type;
    }

    /// \brief Nth rank in topology
    ///
    /// \param[in] ix	Which rank to select
    /// \return	Nth rank or (size_t)-1 if does not exist
    ///
    pami_task_t index2Rank_impl(size_t ix)
    {
      pami_task_t rank = 0;
      pami_coord_t c0;
      pami_result_t rc;
      unsigned x;

      pami_task_t   task;
      size_t        offset;

      if(__all_contexts && __type != PAMI_EPLIST_TOPOLOGY)
      {
        ix = ix / __offset;
      }

      if(ix < __size) switch(__type)
        {
        
        case PAMI_EPLIST_TOPOLOGY:
          PAMI_ENDPOINT_INFO(topo_eplist(ix),task,offset);
          return task;
          break;
        case PAMI_SINGLE_TOPOLOGY:
          return topo_rank;
          break;
        case PAMI_RANGE_TOPOLOGY:
          return topo_range(ix);
          break;
        case PAMI_LIST_TOPOLOGY:
          return topo_list(ix);
          break;

        case PAMI_AXIAL_TOPOLOGY:

	  c0 = topo_axial_center;	  

	  for (x = 0; x < mapping->globalDims() && ix > 0; x++)
          {
	    unsigned ll = topo_axial_lldim(x);
	    unsigned ur = topo_axial_urdim(x);
	    unsigned nn = (mapping->torusSize(x) + ur - topo_axial_center.net_coord(x)) % mapping->torusSize(x);
	    
	    //printf("ix %ld dim %d ll %d ur %d center %ld nn %d\n", ix, x, ll, 
	    //   ur, topo_axial_center.net_coord(x), nn);

	    if (ix <= nn)
            {
	      c0.net_coord(x) = (c0.net_coord(x) + ix) % mapping->torusSize(x);
	      break;
	    }
	    else
	      ix -= nn;
	    
	    nn = (mapping->torusSize(x) + topo_axial_center.net_coord(x) - ll) %
	      mapping->torusSize(x);
	    if (ix <= nn)
	    {
	      c0.net_coord(x) = (mapping->torusSize(x) + c0.net_coord(x) - ix)
		% mapping->torusSize(x);
	      break;
	    }
	    else
	      ix -= nn;
	  }
	  
	  rc = COORDS2RANK(&c0, &rank);
	  return rank;
	  break;

        case PAMI_COORD_TOPOLOGY:
          // probably not used?
          // assume last dim is least-significant
          c0 = topo_llcoord;

          for(x = mapping->globalDims(); x > 0 && ix > 0;)
          {
            --x;
            unsigned ll = topo_lldim(x);
            unsigned ur = topo_urdim(x);
            unsigned nn = ur - ll + 1;
            c0.net_coord(x) = ll + ix % nn;
            ix /= nn;
          }

          rc = COORDS2RANK(&c0, &rank);
          return rank;
          break;
        case PAMI_EMPTY_TOPOLOGY:
        default:
          break;
        }
      return(pami_task_t)-1;
    }
    /// \brief Nth permuted index in topology
    ///
    /// \param[in] index Which index to select
    /// \return	Nth permuted index or (size_t)-1 if does not exist
    ///
    size_t index2PermutedIndex_impl(size_t index)
    {
      // a simple permutation - offset by my own index
      if(likely(index < __size))
      {
        index += rank2Index_impl(mapping->task());
        if(index < __size);
        else index -= __size;
      }
      else index = (size_t)-1;
      return index;
    }

    /// \brief determine index of rank in topology
    ///
    /// This is the inverse function to index2Rank(ix) above.
    ///
    /// \param[in] rank	Which rank to get index for
    /// \return	index of rank (rank(ix) == rank) or (size_t)-1
    ///
    size_t rank2Index_impl(pami_task_t rank)
    {
      size_t x, ix, nn;
      pami_coord_t c0;
      pami_result_t rc;
      no_unused_warning(&c0);
      switch(__type)
      {
      case PAMI_SINGLE_TOPOLOGY:
        if(topo_rank == rank)
        {
          return 0;
        }
        break;
      case PAMI_RANGE_TOPOLOGY:
        if(rank >= topo_first && rank <= topo_last)
        {
          return rank - topo_first;
        }
        break;
      case PAMI_LIST_TOPOLOGY:
        for(x = 0; x < __size; ++x)
        {
          if(rank == topo_list(x))
          {
            return x;
          }
        }
        break;

      case PAMI_AXIAL_TOPOLOGY:
        rc = RANK2COORDS(rank, &c0);
        ix = 0;
        nn = 0;
        for(x = 0; x < mapping->globalDims(); x++)
        {
          unsigned ll = topo_axial_lldim(x);
          unsigned ur = topo_axial_urdim(x);
          if(c0.net_coord(x) == topo_axial_center.net_coord(x))
            ix += (ur - ll);
          else
          {
            if(c0.net_coord(x) > topo_axial_center.net_coord(x))
              ix += c0.net_coord(x) - topo_axial_center.net_coord(x);
            else
              ix += (ur - topo_axial_center.net_coord(x)) +
                    (topo_axial_center.net_coord(x) - c0.net_coord(x));
            return ix;
          }
        }
        return 0; // return index of center, which is always 0
        break;

      case PAMI_COORD_TOPOLOGY:
        // probably not used?
        // assume last dim is least-significant
        rc = RANK2COORDS(rank, &c0);
        ix = 0;
        nn = 0;
        for(x = 0; x < mapping->globalDims(); ++x)
        {
          unsigned ll = topo_lldim(x);
          unsigned ur = topo_urdim(x);
          if(c0.net_coord(x) < ll || c0.net_coord(x) > ur)
          {
            return(size_t)-1;
          }
          nn = ur - ll + 1;
          ix *= nn;
          ix += c0.net_coord(x) - ll;
        }
        return ix;
        break;
      case PAMI_EMPTY_TOPOLOGY:
      default:
        break;
      }
      return(size_t)-1;
    }

    /// \brief return range
    ///
    /// \param[out] first	Where to put first rank in range
    /// \param[out] last	Where to put last rank in range
    /// \return	PAMI_SUCCESS, or PAMI_UNIMPL if not a range topology
    ///
    pami_result_t rankRange_impl(pami_task_t *first, pami_task_t *last)
    {
      if(__type != PAMI_RANGE_TOPOLOGY)
      {
        return PAMI_UNIMPL;
      }
      *first = topo_first;
      *last = topo_last;
      return PAMI_SUCCESS;
    }

    /// \brief return rank list
    ///
    /// \param[out] list	pointer to list stored here
    /// \return	PAMI_SUCCESS, or PAMI_UNIMPL if not a list topology
    ///
    pami_result_t rankList_impl(pami_task_t **list)
    {
      if(__type == PAMI_LIST_TOPOLOGY)
      {
        *list = topo_ranklist;
      }
      else if(__type == PAMI_SINGLE_TOPOLOGY)
      {
        *list = &topo_rank;
      }
      else if(__type == PAMI_EPLIST_TOPOLOGY)
      {
	if(__all_contexts || __offset)
        {
	  fprintf(stderr, "Topology::rankList_impl() -- Probably not good -- requesting rank list from ep list, size %zu, offset %zu, all %u, free %u\n",
                __size,__offset, __all_contexts, __free_ranklist);
	  PAMI_abort();
	  *list = NULL;
	  return PAMI_UNIMPL;
	}
	else
	  *list = topo_ranklist; /* a context offset 0 eplist works as a ranklist ? */
      }
      else
      {
        *list = NULL;
        return PAMI_UNIMPL;
      }
      return PAMI_SUCCESS;
    }

    /// \brief return internal list (rank or endpoint)
    ///
    /// \param[out] list	pointer to list stored here
    /// \return	PAMI_SUCCESS, or PAMI_UNIMPL if not a list topology
    ///
    pami_result_t list_impl(void **list)
    {
      if(__type == PAMI_EPLIST_TOPOLOGY)
      {
        *list = (void *)topo_endplist;
      }
      else if(__type == PAMI_LIST_TOPOLOGY)
      {
        *list = (void *)topo_ranklist;
      }
      else if(__type == PAMI_SINGLE_TOPOLOGY)
      {
        *list = (void *)&topo_rank;
      }
      else
      {
        *list = (void *)NULL;
        return PAMI_UNIMPL;
      }
      return PAMI_SUCCESS;
    }

    /// \brief return axial neighborhood
    ///
    /// Warning! This returns pointers to the Topology internals!
    /// This can result in corruption of a topology if mis-used.
    ///
    /// \param[in] ll	lower-left coordinate
    /// \param[in] ur	upper-right coordinate
    /// \param[in] ref reference rank
    /// \param[in] tl	 torus links flags
    ///
    /// \return	PAMI_SUCCESS, or PAMI_UNIMPL if not an axial topology
    ///
    pami_result_t axial_impl(pami_coord_t **ll, pami_coord_t **ur,
                             pami_coord_t **ref,
                             unsigned char **tl)
    {
      if(__type != PAMI_AXIAL_TOPOLOGY)
      {
        return PAMI_UNIMPL;
      }
      *ll  = &topo_axial_llcoord;
      *ur  = &topo_axial_urcoord;
      *ref = &topo_axial_center;
      *tl  = topo_axial_istorus;
      return PAMI_SUCCESS;
    }

    /// \brief return axial neighborhood
    ///
    /// This method copies data to callers buffers. It is safer
    /// as the caller cannot directly modify the topology.
    ///
    /// \param[in] ll	lower-left coordinate
    /// \param[in] ur	upper-right coordinate
    /// \param[in] ref reference rank
    /// \param[in] tl	 torus links flags
    ///
    /// \return	PAMI_SUCCESS, or PAMI_UNIMPL if not an axial topology
    ///
    pami_result_t axial_impl(pami_coord_t *ll, pami_coord_t *ur,
                             pami_coord_t *ref,
                             unsigned char *tl)
    {
      if(__type != PAMI_AXIAL_TOPOLOGY)
      {
        return PAMI_UNIMPL;
      }
      *ll  = topo_axial_llcoord;
      *ur  = topo_axial_urcoord;
      *ref = topo_axial_center;
      memcpy(tl, topo_axial_istorus, mapping->globalDims());
      return PAMI_SUCCESS;
    }

    /// \brief return rectangular segment coordinates
    ///
    /// Warning! This returns pointers to the Topology internals!
    /// This can result in corruption of a topology if mis-used.
    ///
    /// \param[out] ll	lower-left coord pointer storage
    /// \param[out] ur	upper-right coord pointer storage
    /// \param[out] tl	optional, torus links flags
    /// \return	PAMI_SUCCESS, or PAMI_UNIMPL if not a coord topology
    ///
    pami_result_t rectSeg_impl(pami_coord_t **ll, pami_coord_t **ur,
                               unsigned char **tl = NULL)
    {
      if(__type != PAMI_COORD_TOPOLOGY)
      {
        return PAMI_UNIMPL;
      }
      *ll = &topo_llcoord;
      *ur = &topo_urcoord;
      if(tl)
      {
        *tl = topo_istorus;
      }
      return PAMI_SUCCESS;
    }

    /// \brief return rectangular segment coordinates
    ///
    /// This method copies data to callers buffers. It is safer
    /// as the caller cannot directly modify the topology.
    ///
    /// \param[out] ll	lower-left coord pointer storage
    /// \param[out] ur	upper-right coord pointer storage
    /// \param[out] tl	optional, torus links flags
    /// \return	PAMI_SUCCESS, or PAMI_UNIMPL if not a coord topology
    ///
    pami_result_t rectSeg_impl(pami_coord_t *ll, pami_coord_t *ur,
                               unsigned char *tl = NULL)
    {
      if(__type != PAMI_COORD_TOPOLOGY)
      {
        return PAMI_UNIMPL;
      }
      *ll = topo_llcoord;
      *ur = topo_urcoord;
      if(tl)
      {
        memcpy(tl, topo_istorus, mapping->globalDims());
      }
      return PAMI_SUCCESS;
    }

    /// \brief does topology consist entirely of ranks local to eachother
    ///
    /// \return boolean indicating locality of ranks
    ///
    bool isLocal_impl()
    {
      pami_result_t rc;
      if(likely(__type == PAMI_COORD_TOPOLOGY))
      {
        return __isLocalCoord(&topo_llcoord,
                              &topo_urcoord);
      }
      else
      {
        // the hard way...
        if(__type == PAMI_SINGLE_TOPOLOGY)
        {
          // a rank is always local to itself...
          return true;
        }
        else if(__type == PAMI_RANGE_TOPOLOGY)
        {
          // would it be faster to just compare ranks in vnpeers list?
          // can't do that here since this wants to know if
          // ranks are all local to each other... not self.
          pami_coord_t c0, c1;
          rc = RANK2COORDS(topo_first, &c0);
          PAMI_assert_debugf(rc == PAMI_SUCCESS, "RANK2COORDS failed\n");
          pami_task_t r;
          for(r = topo_first + 1; r <= topo_last; ++r)
          {
            RANK2COORDS(r, &c1);
            if(!__isLocalCoord(&c0, &c1))
            {
              return false;
            }
          }
          return true;
        }
        else if(__type == PAMI_LIST_TOPOLOGY)
        {
          pami_coord_t c0, c1;
          rc = RANK2COORDS(topo_list(0), &c0);
          PAMI_assert_debugf(rc == PAMI_SUCCESS, "RANK2COORDS failed\n");
          unsigned i;
          for(i = 1; i < __size; ++i)
          {
            RANK2COORDS(topo_list(i), &c1);
            if(!__isLocalCoord(&c0, &c1))
            {
              return false;
            }
          }
          return true;
        }
      }
      // i.e. PAMI_EMPTY_TOPOLOGY
      return false;
    }

    /// \brief does topology consist entirely of ranks local to self
    ///
    /// \return boolean indicating locality of ranks
    ///
    bool isLocalToMe_impl()
    {
      if(likely(__type == PAMI_COORD_TOPOLOGY))
      {
        // does mapping have "me" cached as a pami_coord_t?
        return __isLocalCoord(&topo_llcoord,
                              &topo_urcoord) &&
        __isLocalCoord(MY_COORDS,
                       &topo_llcoord);
      }
      else
      {
        // the hard way...
        if(__type == PAMI_SINGLE_TOPOLOGY)
        {
          return IS_LOCAL_PEER(__topo._rank);
        }
        else if(__type == PAMI_RANGE_TOPOLOGY)
        {
          pami_task_t r;
          for(r = topo_first; r <= topo_last; ++r)
          {
            if(!IS_LOCAL_PEER(r))
            {
              return false;
            }
          }
          return true;
        }
        else if(__type == PAMI_LIST_TOPOLOGY)
        {
          unsigned i;
          for(i = 0; i < __size; ++i)
          {
            if(!IS_LOCAL_PEER(topo_list(i)))
            {
              return false;
            }
          }
          return true;
        }
      }
      // i.e. PAMI_EMPTY_TOPOLOGY
      return false;
    }

    /// \brief does topology consist entirely of ranks that do not share nodes
    ///
    /// \return boolean indicating locality of ranks
    ///
    bool isGlobal_impl()
    {
      // is this too difficult/expensive? is it needed?
      // Note, this is NOT simply "!isLocal()"...
      PAMI_abortf("Topology::isGlobal not implemented\n");
      return false;
    }

    /// \brief is topology a rectangular segment
    /// \return	boolean indicating rect seg topo
    bool isRectSeg_impl()
    {
      return(__type == PAMI_COORD_TOPOLOGY);
    }

    /// \brief extract Nth dimensions from coord topology
    ///
    /// \param[in] n	Which dim to extract
    /// \param[out] c0	lower value for dim range
    /// \param[out] cn	upper value for dim range
    /// \param[out] tl	optional, torus link flag
    ///
    pami_result_t getNthDims_impl(unsigned n, unsigned *c0, unsigned *cn,
                                  unsigned char *tl = NULL)
    {
      if(__type == PAMI_COORD_TOPOLOGY)
      {
        *c0 = topo_lldim(n);
        *cn = topo_urdim(n);
        if(tl)
        {
          *tl = topo_hastorus(n);
        }
      }
      else if(__type == PAMI_COORD_TOPOLOGY)
      {
        *c0 = topo_axial_lldim(n);
        *cn = topo_axial_urdim(n);
        if(tl)
        {
          *tl = topo_axial_hastorus(n);
        }
      }
      else return PAMI_UNIMPL; 

      return PAMI_SUCCESS; 
    }

    bool isEndpointMember_impl(pami_endpoint_t endpoint)
    {
      for(unsigned i=0; i<size(); i++)
        if(endpoint == index2Endpoint(i))
          return true;
      return false;
    }

    bool isContextOffset_impl(size_t slice)
    {
      /* PAMI_EPLIST_TOPLOGY's *could* be single slice but we're not
         going to try to detect that here.  Only simple, single
         slice contexts will return true. */
      return(__offset == slice && __type != PAMI_EPLIST_TOPOLOGY);
    }

    /// \brief is rank in topology
    ///
    /// \param[in] rank	Rank to test
    /// \return	boolean indicating rank is in topology
    ///
    bool isRankMember_impl(pami_task_t rank)
    {
      pami_result_t rc;
      if(unlikely(__type == PAMI_COORD_TOPOLOGY || __type == PAMI_AXIAL_TOPOLOGY))
      {
        pami_coord_t c0;
        rc = RANK2COORDS(rank, &c0);
        PAMI_assert_debugf(rc == PAMI_SUCCESS, "RANK2COORDS failed\n");
        return __isMemberCoord(&c0, mapping->globalDims());
      }
      else
      {
        if(__type == PAMI_SINGLE_TOPOLOGY)
        {
          return(rank == __topo._rank);
        }
        else if(__type == PAMI_RANGE_TOPOLOGY)
        {
          return(rank >= topo_first && rank <= topo_last);
        }
        else if(__type == PAMI_LIST_TOPOLOGY)
        {
          unsigned i;
          for(i = 0; i < __size; ++i)
          {
            if(rank == topo_list(i))
            {
              return true;
            }
          }
          return false;
        }
        else if(__type == PAMI_EPLIST_TOPOLOGY)
        { // this is dumb, find a rank in an ep list?
          pami_endpoint_t ep;
          if(__all_contexts)
          {
            ep = PAMI_ENDPOINT_INIT(__client,rank,0);
          }
          else
            ep = PAMI_ENDPOINT_INIT(__client,rank,__offset);
          unsigned i;
          for(i = 0; i < __size; ++i)
          {
            if(ep == topo_eplist(i))
            {
              return true;
            }
          }
        }
      }
      // i.e. PAMI_EMPTY_TOPOLOGY
      return false;
    }

    /// \brief is coordinate in topology
    ///
    /// \param[in] c0	Coord to test
    /// \return	boolean indicating coord is a member of topology
    ///
    bool isCoordMember_impl(pami_coord_t *c0)
    {
      pami_result_t rc;
      if(likely(__type == PAMI_COORD_TOPOLOGY))
      {
        return __isMemberCoord(c0, mapping->globalDims());
      }
      else if(__type == PAMI_EMPTY_TOPOLOGY)
      {
        return false;
      }
      else
      {
        // the hard way...
        pami_task_t rank = 0;
        // PAMI_assert(c0->network == PAMI_N_TORUS_NETWORK);
        rc = COORDS2RANK(c0, &rank);
        return isRankMember(rank);
      }
    }

    /// \brief create topology of ranks local to self
    ///
    /// \param[out] _new	Where to build topology
    ///
    void subTopologyLocalToMe_impl(PAMI::Topology *_new)
    {
      __subTopologyLocalToMe(_new);
    }

    /// \brief create topology from all Nth ranks globally
    ///
    /// \todo #warning This is broken - depends on existence of task2node().local == n
    ///
    /// \param[out] _new	Where to build topology
    /// \param[in] n	Which local rank to select on each node
    ///
    void subTopologyNthGlobal_impl(PAMI::Topology *_new, int n)
    {
      if(likely(__type == PAMI_COORD_TOPOLOGY))
      {
        __subTopologyNthGlobal(_new, n);
        // may produce empty topology, if "n" is out of range.
      }
      else if(__type == PAMI_EPLIST_TOPOLOGY)
      {
        //igtanase:: nth endpoint in each task
        //similar with the existing code for rank list topolgy  below
        _new->__offset       = 0;
        _new->__all_contexts = false;
        size_t s = __size;
        typedef size_t tb_t[3]; //{global address, count of ranks found for this address, index into ranklist of Nth rank }
        pami_endpoint_t *epl, *epl_free;
        tb_t  *tb;
        pami_result_t rc;
        PAMI_assert(s != 0);
        size_t num_nodes = s <= mapping->numActiveNodes()? s : mapping->numActiveNodes();
        size_t sz_to_malloc =  num_nodes * sizeof(*epl); /* guess max size of 1 rank per node if possible */
        rc = PAMI::Memory::MemoryManager::heap_mm->memalign((void **)&epl, 0, sz_to_malloc);
        PAMI_assertf(rc == PAMI_SUCCESS, "temp eplist[%zd] alloc failed", s);
        rc = PAMI::Memory::MemoryManager::heap_mm->memalign((void **)&tb, 0, s * sizeof(*tb));
        PAMI_assertf(rc == PAMI_SUCCESS, "temp tb-list[%zd] alloc failed (endpoints)", s);
        memset(tb, 0, s * sizeof(*tb));
        size_t k = 0;
        pami_endpoint_t ep_r;
        pami_task_t r;
        size_t         offset;
        PAMI::Interface::Mapping::nodeaddr_t a;
        size_t i, j, l = 0;
        for(i = 0; i < s; ++i)
        {
          ep_r = index2Endpoint(i);
          PAMI_ENDPOINT_INFO(ep_r,r,offset);
          // PAMI_assert(r != -1);
          mapping->task2node(r, a);
          for(j = 0; j <= l; ++j)
          {
            if(j == l)
            {
              tb[j][0] = a.global;
              tb[j][2] = -1UL; // haven't found a rank yet
              ++l;
            }
            if(a.global == tb[j][0])
            {
              if(a.local == (size_t)n) 
              {
                // Prefer to slice Nth based on local address not an arbitrary counter, so overwrite if one was already stored at tb[j][2] index.
                //fprintf(stderr,"rank %u, a.global %zu, a.local %zu, tb[%zu][1] (%zu) == (size_t)n (%u)), k %zu, found k %zd \n",r, a.global, a.local, j, tb[j][1], n, k, tb[j][2]);
                if(tb[j][2] == -1UL)   // didn't already find by Nth counter
                  tb[j][2] = k++; // Inserting into eplist at 'k' 
                epl[tb[j][2]] = ep_r;
              }
              if((tb[j][1] == (size_t)n) && // Nth (arbitrary counter) rank found and
                 (tb[j][2] == -1UL))          // didn't find by local address
              {
                //fprintf(stderr,"rank %u, a.global %zu, a.local %zu, tb[%zu][1] (%zu) == (size_t)n (%u)), k %zu, found k %zd \n",r, a.global, a.local, j, tb[j][1], n, k, tb[j][2]);
                tb[j][2] = k; // Inserting into eplist at 'k' 
                epl[k++] = ep_r;
              }
              ++tb[j][1];
              break;
            }
          }
        }
        PAMI::Memory::MemoryManager::heap_mm->free(tb);
        if(k > 0)
        {
          // The eplist could be too big... e.g. on BGQ with 64 PPN this may be 64 x ncontexts too large and that 
          // could be significant at 96K nodes (x 64 ranks per node x ncontexts x 8 bytes)
          epl_free = epl;
          rc = PAMI::Memory::MemoryManager::heap_mm->memalign((void **)&epl, 0, k * sizeof(*epl));
          //PAMI_assertf(rc == PAMI_SUCCESS, "realloc'd eplist[%zd] alloc failed", k);
          if(rc==PAMI_SUCCESS)
          {
            memcpy(epl, epl_free, k*sizeof(*epl));
            PAMI::Memory::MemoryManager::heap_mm->free(epl_free);
          }
          else epl = epl_free; /* just use the big one if we couldn't malloc the smaller one */
          _new->__type = PAMI_EPLIST_TOPOLOGY;
          _new->topo_ranklist = epl;
          _new->__size = k;
          _new->__free_ranklist = true;
          return;
        }
        //here something not quite right;
        _new->__type = PAMI_EMPTY_TOPOLOGY;
        _new->__size = 0;
        _new->__free_ranklist =false;
        PAMI::Memory::MemoryManager::heap_mm->free(epl);
      }
      else
      {
        // the hard way... impractical?
        _new->__offset = 0;
        _new->__all_contexts=false;
        size_t s = size();
        typedef size_t tb_t[3]; //{global address, count of ranks found for this address, index into ranklist of Nth rank }
        pami_task_t *rl, *rl_free;
        tb_t *tb;
        pami_result_t rc;
        size_t num_nodes = s <= mapping->numActiveNodes()? s : mapping->numActiveNodes();
        size_t sz_to_malloc =  num_nodes * sizeof(*rl); /* guess max size of 1 rank per node if possible */
        rc = PAMI::Memory::MemoryManager::heap_mm->memalign((void **)&rl, 0, sz_to_malloc);
        PAMI_assertf(rc == PAMI_SUCCESS, "temp ranklist[%zd] alloc failed", s);
        rc = PAMI::Memory::MemoryManager::heap_mm->memalign((void **)&tb, 0, s * sizeof(*tb));
        PAMI_assertf(rc == PAMI_SUCCESS, "temp tb-list[%zd] alloc failed", s);
        memset(tb, 0, s * sizeof(*tb));
        size_t k = 0;
        pami_task_t r;
        PAMI::Interface::Mapping::nodeaddr_t a;
        size_t i, j, l = 0;
        for(i = 0; i < s; ++i)
        {
          r = index2Rank(i);
          mapping->task2node(r, a);
          for(j = 0; j <= l; ++j)
          {
            if(j == l)
            {
              tb[j][0] = a.global;
              tb[j][2] = -1UL; // haven't found a rank yet
              ++l;
            }
            if(a.global == tb[j][0])
            {
              if(a.local == (size_t)n) 
              {
                // Prefer to slice Nth based on local address not an arbitrary counter, so overwrite if one was already stored at tb[j][2] index.
                //fprintf(stderr,"rank %u, a.global %zu, a.local %zu, tb[%zu][1] (%zu) == (size_t)n (%u)), k %zu, found k %zd \n",r, a.global, a.local, j, tb[j][1], n, k, tb[j][2]);
                if(tb[j][2] == -1UL)   // didn't already find by Nth counter
                  tb[j][2] = k++; // Inserting into ranklist at 'k' 
                rl[tb[j][2]] = r;
              }
              if((tb[j][1] == (size_t)n) && // Nth (arbitrary counter) rank found and
                 (tb[j][2] == -1UL))          // didn't find by local address
              {
                //fprintf(stderr,"rank %u, a.global %zu, a.local %zu, tb[%zu][1] (%zu) == (size_t)n (%u)), k %zu, found k %zd \n",r, a.global, a.local, j, tb[j][1], n, k, tb[j][2]);
                tb[j][2] = k; // Inserting into ranklist at 'k' 
                rl[k++] = r;
              }
              ++tb[j][1];
              break;
            }
          }
        }
        PAMI::Memory::MemoryManager::heap_mm->free(tb);
        if(k == 1)
        {
          _new->__type = PAMI_SINGLE_TOPOLOGY;
          _new->__size = 1;
          _new->__free_ranklist =false;
          _new->topo_rank = rl[0];
          PAMI::Memory::MemoryManager::heap_mm->free(rl);
          return;
        }
        if(k > 1)
        {
          // The ranklist could be too big... e.g. on BGQ with 64 PPN this may be 64x too large and that 
          // could be significant at 96K nodes (x 64 ranks per node x 8 bytes)
          rl_free = rl;
          rc = PAMI::Memory::MemoryManager::heap_mm->memalign((void **)&rl, 0, k * sizeof(*rl));
          //PAMI_assertf(rc == PAMI_SUCCESS, "realloc'd ranklist[%zd] alloc failed", k);
          if(rc==PAMI_SUCCESS)
          {
            memcpy(rl, rl_free, k*sizeof(*rl));
            PAMI::Memory::MemoryManager::heap_mm->free(rl_free);
          }
          else rl = rl_free; /* just use the big one if we couldn't malloc the smaller one */
          _new->__type = PAMI_LIST_TOPOLOGY;
          _new->topo_ranklist = rl;
          _new->__size = k;
          _new->__free_ranklist = true;
          return;
        }
        _new->__type = PAMI_EMPTY_TOPOLOGY;
        _new->__size = 0;
        _new->__free_ranklist =false;
        PAMI::Memory::MemoryManager::heap_mm->free(rl);
      }
    }

    /// \brief reduce dimensions of topology (cube -> plane, etc)
    ///
    /// The 'fmt' param is a pattern indicating which dimensions
    /// to squash, and what coord to squash into. A dim in 'fmt'
    /// having "-1" will be preserved, while all others will be squashed
    /// into a dimension of size 1 having the value specified.
    ///
    /// \param[out] _new	where to build new topology
    /// \param[in] fmt	how to reduce dimensions
    ///
    void subTopologyReduceDims_impl(PAMI::Topology *_new, pami_coord_t *fmt)
    {
      if(likely(__type == PAMI_COORD_TOPOLOGY))
      {
        __subTopologyReduceDims(_new, fmt);
      }
      else
      {
        // the really hard way... impractical?
        _new->__type = PAMI_EMPTY_TOPOLOGY;
        _new->__size = 0;
        _new->__free_ranklist =false;
      }
    }

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
    /// \param[in] max	size of caller-allocated array
    /// \param[out] ranks	array where rank list is placed
    /// \param[out] nranks	actual number of ranks put into array
    ///
    void getRankList_impl(size_t max, pami_task_t *ranks, size_t *nranks)
    {
      pami_result_t rc;
      *nranks = __size; // might exceed "max" - caller detects error.
      PAMI_assert_debugf(max != 0, "getRankList called with no array space\n");
      if(likely(__type == PAMI_LIST_TOPOLOGY))
      {
        unsigned x;
        for(x = 0; x < __size && x < max; ++x)
        {
          ranks[x] = topo_list(x);
        }
      }
      else if(likely(__type == PAMI_EPLIST_TOPOLOGY))
      {
        // Expect odd results if __all_contexts or truly an ep list, we're
        // not going to try to eliminate dup ranks here...
        unsigned x;
        for(x = 0; x < __size && x < max; ++x)
        {
          ranks[x]=index2Rank_impl(x);
        }
      }
      else if(__type == PAMI_COORD_TOPOLOGY)
      {
        // the hard way...
        pami_coord_t c0;
        pami_task_t rank = 0;
        unsigned x;
        // c0 = llcorner;
        c0.network = PAMI_N_TORUS_NETWORK;
        for(x = 0; x < mapping->globalDims(); ++x)
        {
          c0.net_coord(x) = topo_lldim(x);
        }
        x = 0;
        do
        {
          rc = COORDS2RANK(&c0, &rank);
          ranks[x] = rank;
          ++x;
        } while(x < max && __nextCoord(&c0, mapping->globalDims()));
      }
      else if(__type == PAMI_SINGLE_TOPOLOGY)
      {
        ranks[0] = __topo._rank;
      }
      else if(__type == PAMI_RANGE_TOPOLOGY)
      {
        pami_task_t r;
        unsigned x;
        for(x = 0, r = topo_first; r <= topo_last && x < max; ++x, ++r)
        {
          ranks[x] = r;
        }
      }
    }

    /// \brief check if rank range or list can be converted to rectangle
    ///
    /// Since a rectangular segment is consider the optimal state, no
    /// other analysis is done. A PAMI_SINGLE_TOPOLOGY cannot be optimized,
    /// either. Optimization levels:
    ///
    ///	PAMI_SINGLE_TOPOLOGY (most)
    ///	PAMI_COORD_TOPOLOGY
    ///	PAMI_RANGE_TOPOLOGY
    ///	PAMI_LIST_TOPOLOGY (least)
    ///
    /// \return	'true' if topology was changed
    ///
    bool analyzeTopology_impl()
    {
      if(__size == 1 && __type != PAMI_SINGLE_TOPOLOGY)
      {
        if(__type == PAMI_LIST_TOPOLOGY)
        {
          // it might not always be desirable to
          // convert a coord to single rank...
          // maybe shouldn't at all...
          pami_task_t rank = 0;
          COORDS2RANK(&topo_llcoord, &rank);
          __type = PAMI_SINGLE_TOPOLOGY;
          topo_rank = rank;
          return true;
        }
        else if(__type == PAMI_RANGE_TOPOLOGY)
        {
          __type = PAMI_SINGLE_TOPOLOGY;
          topo_rank = topo_first;
          return true;
        }
        else if(__type == PAMI_LIST_TOPOLOGY)
        {
          __type = PAMI_SINGLE_TOPOLOGY;
          topo_rank = topo_list(0);
          return true;
        }
      }
      else if(__type == PAMI_LIST_TOPOLOGY)
      {
        // Note, might be able to convert to range...
        return __analyzeCoordsList() ||
        __analyzeRangeList();
      }
      else if(__type == PAMI_RANGE_TOPOLOGY)
      {
        return __analyzeCoordsRange();
      }
      return false;
    }

    /// \brief check if topology can be converted to type
    ///
    /// Does not differentiate between invalid conversions and
    /// 'null' conversions (same type).
    ///
    /// \param[in] new_type	Topology type to try and convert into
    /// \return	'true' if topology was changed
    ///
    bool convertTopology_impl(pami_topology_type_t new_type)
    {
      pami_result_t rc;
      pami_coord_t c0;
      pami_task_t rank = 0;
      pami_task_t *rl, *rp;
      pami_task_t min, max;
      // __free_ranklist flag is unchanged unless conversion dictates
      switch(__type)
      {
      case PAMI_SINGLE_TOPOLOGY:
        switch(new_type)
        {
        case PAMI_COORD_TOPOLOGY:
          pami_coord_t c0;
          rc = RANK2COORDS(topo_rank, &c0);
          __type = PAMI_COORD_TOPOLOGY;
          topo_llcoord = c0;
          topo_urcoord = c0;
          set_istorus(topo_llcoord,topo_urcoord,topo_istorus);  
          return true;
          break;
        case PAMI_RANGE_TOPOLOGY:
          __type = PAMI_RANGE_TOPOLOGY;
          topo_first =
          topo_last = topo_rank;
          return true;
          break;
        case PAMI_LIST_TOPOLOGY:
          // SINGLE -> LIST, malloc ranklist and turn on flag
          rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                             (void **)&rl, 0, sizeof(*rl));
          PAMI_assertf(rc == PAMI_SUCCESS, "temp ranklist[1] alloc failed");
          __type = PAMI_LIST_TOPOLOGY;
          *rl = topo_rank;
          topo_ranklist = rl;
          __free_ranklist =true;
          return true;
          break;
        case PAMI_SINGLE_TOPOLOGY:
          break;
        default:
          break;
        }
        break;
      case PAMI_RANGE_TOPOLOGY:
        switch(new_type)
        {
        case PAMI_COORD_TOPOLOGY:
          return __analyzeCoordsRange();
          break;
        case PAMI_SINGLE_TOPOLOGY:
          if(__size == 1)
          {
            __type = PAMI_SINGLE_TOPOLOGY;
            topo_rank = topo_first;
            return true;
          }
          break;
        case PAMI_LIST_TOPOLOGY:
          // RANGE to LIST, malloc ranklist and turn on flag
          rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                             (void **)&rl, 0, __size * sizeof(*rl));
          PAMI_assertf(rc == PAMI_SUCCESS, "temp ranklist[%zd] alloc failed", __size);
          rp = rl;
          __type = PAMI_LIST_TOPOLOGY;
          pami_task_t r;
          for(r = topo_first; r <= topo_last; ++r)
          {
            *rp++ = r;
          }
          topo_ranklist = rl;
          __free_ranklist =true;
          return true;
          break;
        case PAMI_RANGE_TOPOLOGY:
          break;
        default:
          break;
        }
        break;
      case PAMI_LIST_TOPOLOGY:
        switch(new_type)
        {
        case PAMI_COORD_TOPOLOGY:
          // LIST -> COORDS, drop ranklist and turn off flag
          return __analyzeCoordsList();
          break;
        case PAMI_SINGLE_TOPOLOGY:
          if(__size == 1)
          {
            // LIST -> SINGLE, drop ranklist and turn off flag
            if(__free_ranklist)
            {
              PAMI::Memory::MemoryManager::heap_mm->free(topo_ranklist);
              __free_ranklist = false;
            }
            __type = PAMI_SINGLE_TOPOLOGY;
            topo_rank = topo_list(0);
            return true;
          }
          break;
        case PAMI_LIST_TOPOLOGY:
          break;
        case PAMI_RANGE_TOPOLOGY:
          // LIST -> RANGE, drop ranklist and turn off flag
          return __analyzeRangeList();
          break;
        default:
          break;
        }
        break;
      case PAMI_COORD_TOPOLOGY:
        switch(new_type)
        {
        case PAMI_COORD_TOPOLOGY:
          break;
        case PAMI_SINGLE_TOPOLOGY:
          if(__size == 1)
          {
            rc = COORDS2RANK(&topo_llcoord, &rank);
            __type = PAMI_SINGLE_TOPOLOGY;
            topo_rank = rank;
            return true;
          }
          break;
        case PAMI_LIST_TOPOLOGY:
          // COORD -> LIST, malloc ranklist and turn on flag
          rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                             (void **)&rl, 0, __size * sizeof(*rl));
          PAMI_assertf(rc == PAMI_SUCCESS, "temp ranklist[%zd] alloc failed", __size);
          rp = rl;
          c0 = topo_llcoord;
          do
          {
            rc = COORDS2RANK(&c0, &rank);
            *rp++ = rank;
          } while(__nextCoord(&c0, mapping->globalDims()));
          __type = PAMI_LIST_TOPOLOGY;
          topo_ranklist = rl;
          __free_ranklist =true;
          return true;
          break;
        case PAMI_RANGE_TOPOLOGY:
          min = (pami_task_t)-1;
          max = 0;
          c0 = topo_llcoord;
          do
          {
            rc = COORDS2RANK(&c0, &rank);
            if(rank < min) min = rank;
            if(rank > max) max = rank;
          } while(__nextCoord(&c0, mapping->globalDims()));
          if(__size == max - min + 1)
          {
            __type = PAMI_RANGE_TOPOLOGY;
            topo_first = min;
            topo_last = max;
            return true;
          }
          break;
        default:
          break;
        }
        break;
      default:
        break;
      }
      return false; // no change - possible error
    }

    /// \brief produce the union of two topologies
    ///
    /// produces: _new = this .U. other
    ///
    /// \param[out] _new	New topology created there
    /// \param[in] other	The other topology
    ///
    void unionTopology_impl(Topology *_new, Topology *other)
    {
      (void)_new;(void)other;
      // for now, assume this isn't used/needed.
      PAMI_abortf("Topology::unionTopology not implemented\n");
#if 0
      if(likely(__type == other->__type))
      {
        // size_t s;
        // size_t i, j, k;
        switch(__type)
        {
        case PAMI_COORD_TOPOLOGY:
          // in many cases we must fall-back to rank lists...
          // for now, just always fall-back.
          break;
        case PAMI_SINGLE_TOPOLOGY:
          break;
        case PAMI_RANGE_TOPOLOGY:
          break;
        case PAMI_LIST_TOPOLOGY:
          /// \todo keep this from being O(n^2)
          break;
        case PAMI_EMPTY_TOPOLOGY:
        default:
          break;
        }
      }
      else if(__type == PAMI_EMPTY_TOPOLOGY)
      {
        *_new = *other;
        return;
      }
      else if(other->__type == PAMI_EMPTY_TOPOLOGY)
      {
        *_new = *this;
        return;
      }
      else if(__type == PAMI_SINGLE_TOPOLOGY)
      {
      }
      else if(other->__type == PAMI_SINGLE_TOPOLOGY)
      {
      }
      else
      {
        // more complicated scenarios - TBD
        switch(__type)
        {
        case PAMI_COORD_TOPOLOGY:
          switch(other->__type)
          {
          case PAMI_LIST_TOPOLOGY:
            break;
          case PAMI_RANGE_TOPOLOGY:
            break;
          case PAMI_SINGLE_TOPOLOGY: // already handled
          case PAMI_COORD_TOPOLOGY:  // already handled
          default:
            break;
          }
        case PAMI_LIST_TOPOLOGY:
          switch(other->__type)
          {
          case PAMI_COORD_TOPOLOGY:
            break;
          case PAMI_RANGE_TOPOLOGY:
            break;
          case PAMI_SINGLE_TOPOLOGY: // already handled
          case PAMI_LIST_TOPOLOGY:   // already handled
          default:
            break;
          }
        case PAMI_RANGE_TOPOLOGY:
          switch(other->__type)
          {
          case PAMI_COORD_TOPOLOGY:
            break;
          case PAMI_LIST_TOPOLOGY:
            break;
          case PAMI_SINGLE_TOPOLOGY: // already handled
          case PAMI_RANGE_TOPOLOGY:  // already handled
          default:
            break;
          }
        case PAMI_SINGLE_TOPOLOGY: // already handled
        default:
          break;
        }
      }
#endif
    }

    /// \brief produce the intersection of two topologies
    ///
    /// produces: _new = this ./\. other
    ///
    /// \param[out] _new	New topology created there
    /// \param[in] other	The other topology
    ///
    void intersectTopology_impl(Topology *_new, Topology *other)
    {
      if(likely(__type == other->__type))
      {
        size_t s;
        size_t i, j, k;
        pami_task_t *rl;
        pami_result_t rc;
        switch(__type)
        {
        case PAMI_COORD_TOPOLOGY:
          // This always results in a rectangle...
          // first, check for disjoint
          if(__coordLT(&topo_urcoord,
                       &other->topo_llcoord,
                       mapping->globalDims()) ||
             __coordLT(&other->topo_urcoord,
                       &topo_llcoord,
                       mapping->globalDims()))
          {
            break;
          }
          _new->__type = PAMI_COORD_TOPOLOGY;
          __coordMAX(&_new->topo_llcoord,
                     &topo_llcoord,
                     &other->topo_llcoord,
                     mapping->globalDims());
          __coordMIN(&_new->topo_urcoord,
                     &topo_urcoord,
                     &other->topo_urcoord,
                     mapping->globalDims());
          _new->__size = __sizeRange(&_new->topo_llcoord,
                                     &_new->topo_urcoord, mapping->globalDims());
          _new->__free_ranklist =false;
          set_istorus(_new->topo_llcoord,_new->topo_urcoord,_new->topo_istorus);  
          return;
          break;
        case PAMI_SINGLE_TOPOLOGY:
          if(other->topo_rank == topo_rank)
          {
            _new->__type = PAMI_SINGLE_TOPOLOGY;
            _new->__size = 1;
            _new->__free_ranklist =false;
            _new->topo_rank = topo_rank;
            return;
          }
          break;
        case PAMI_RANGE_TOPOLOGY:
          // This always results in a range...
          if(topo_last < other->topo_first ||
             other->topo_last < topo_first)
          {
            // disjoint ranges
            break;
          }
          _new->__type = PAMI_RANGE_TOPOLOGY;
          _new->topo_first = (topo_first > other->topo_first ?
                              topo_first : other->topo_first);
          _new->topo_last = (topo_last < other->topo_last ?
                             topo_last : other->topo_last);
          _new->__size = _new->topo_last - _new->topo_first + 1;
          _new->__free_ranklist =false;
          return;
          break;
        case PAMI_LIST_TOPOLOGY:
          /// \todo keep this from being O(n^2)
          // guess at size: smallest topology.
          s = (__size < other->__size ?
               __size : other->__size);
          rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                             (void **)&rl, 0, s * sizeof(*rl));
          PAMI_assertf(rc == PAMI_SUCCESS, "temp ranklist[%zd] alloc failed", s);
          k = 0;
          for(i = 0; i < __size; ++i)
          {
            for(j = 0; j < other->__size; ++j)
            {
              if(topo_list(i) == other->topo_list(j))
              {
                rl[k++] = topo_list(i);
              }
            }
          }
          if(k)
          {
            _new->__type = PAMI_LIST_TOPOLOGY;
            _new->__size = k;
            _new->__free_ranklist =true;
            _new->topo_ranklist = rl;
            return;
          }
          PAMI::Memory::MemoryManager::heap_mm->free(rl);
          break;
        case PAMI_EMPTY_TOPOLOGY:
        default:
          break;
        }
      }
      else if(__type == PAMI_SINGLE_TOPOLOGY)
      {
        if(other->isRankMember(topo_rank))
        {
          _new->__type = PAMI_SINGLE_TOPOLOGY;
          _new->__size = 1;
          _new->__free_ranklist =false;
          _new->topo_rank = topo_rank;
          return;
        }
      }
      else if(other->__type == PAMI_SINGLE_TOPOLOGY)
      {
        if(isRankMember(other->topo_rank))
        {
          _new->__type = PAMI_SINGLE_TOPOLOGY;
          _new->__size = 1;
          _new->__free_ranklist =false;
          _new->topo_rank = other->topo_rank;
          return;
        }
      }
      else if(__type != PAMI_EMPTY_TOPOLOGY &&
              other->__type != PAMI_EMPTY_TOPOLOGY)
      {
        // more complicated scenarios - TBD
        switch(__type)
        {
        case PAMI_COORD_TOPOLOGY:
          switch(other->__type)
          {
          case PAMI_LIST_TOPOLOGY:
            break;
          case PAMI_RANGE_TOPOLOGY:
            break;
          case PAMI_SINGLE_TOPOLOGY: // already handled
          case PAMI_COORD_TOPOLOGY:  // already handled
          default:
            break;
          }
          break;
        case PAMI_LIST_TOPOLOGY:
          switch(other->__type)
          {
          case PAMI_COORD_TOPOLOGY:
            break;
          case PAMI_RANGE_TOPOLOGY:
            break;
          case PAMI_SINGLE_TOPOLOGY: // already handled
          case PAMI_LIST_TOPOLOGY:   // already handled
          default:
            break;
          }
          break;
        case PAMI_RANGE_TOPOLOGY:
          switch(other->__type)
          {
          case PAMI_COORD_TOPOLOGY:
            break;
          case PAMI_LIST_TOPOLOGY:
            break;
          case PAMI_SINGLE_TOPOLOGY: // already handled
          case PAMI_RANGE_TOPOLOGY:  // already handled
          default:
            break;
          }
          break;
        case PAMI_SINGLE_TOPOLOGY: // already handled
        default:
          break;
        }
      }
      _new->__type = PAMI_EMPTY_TOPOLOGY;
      _new->__size = 0;
      _new->__free_ranklist =false;
    }

    /// \brief produce the difference of two topologies
    ///
    /// produces: _new = this .-. other
    ///
    /// \param[out] _new	New topology created there
    /// \param[in] other	The other topology
    ///
    void subtractTopology_impl(Topology *_new, Topology *other)
    {
      pami_result_t rc;
      pami_coord_t ll, ur, c0;
      pami_task_t rank = 0;
      pami_task_t min = 0, max = 0;
      size_t s;
      size_t i, j, k;
      pami_task_t *rl;
      unsigned flag;
      if(likely(__type == other->__type))
      {
        switch(__type)
        {
        case PAMI_COORD_TOPOLOGY:
          // This results in coord range in special cases
          // that we don't check for. We just create a
          // list and then try to convert it to coords.
          s = __size;
          rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                             (void **)&rl, 0, s * sizeof(*rl));
          PAMI_assertf(rc == PAMI_SUCCESS, "temp ranklist[%zd] alloc failed", s);
          k = 0;
          c0 = topo_llcoord;
          __initRange(&ll, &ur, &c0, mapping->globalDims());
          min = max = rank;
          do
          {
            if(other->__isMemberCoord(&c0,
                                      mapping->globalDims()))
            {
              continue;
            }
            // keep it
            rc = COORDS2RANK(&c0, &rank);
            __bumpRange(&ll, &ur, &c0, mapping->globalDims());
            if(rank < min) min = rank;
            if(rank > max) max = rank;
            rl[k++] = rank;
          } while(__nextCoord(&c0, mapping->globalDims()));
          if(k == 0)
          {
            PAMI::Memory::MemoryManager::heap_mm->free(rl);
            break;
          }
          _new->__size = k;
          _new->__free_ranklist =false;
          s = __sizeRange(&ll, &ur, mapping->globalDims());
          if(s == k)
          {
            _new->__type = PAMI_COORD_TOPOLOGY;
            _new->topo_llcoord = ll;
            _new->topo_urcoord = ur;
            set_istorus(_new->topo_llcoord,_new->topo_urcoord,_new->topo_istorus);  
            PAMI::Memory::MemoryManager::heap_mm->free(rl);
          }
          else if(max - min + 1 == k)
          {
            _new->__type = PAMI_RANGE_TOPOLOGY;
            _new->topo_first = min;
            _new->topo_last = max;
            PAMI::Memory::MemoryManager::heap_mm->free(rl);
          }
          else
          {
            _new->__type = PAMI_LIST_TOPOLOGY;
            _new->topo_ranklist = rl;
            _new->__free_ranklist = true;
          }
          return;
          break;
        case PAMI_SINGLE_TOPOLOGY:
          if(other->topo_rank != topo_rank)
          {
            *_new = *this;
            return;
          }
          break;
        case PAMI_RANGE_TOPOLOGY:
          // This mostly results in a range...
          flag =  ((topo_first >= other->topo_first) << 3) |
                  ((topo_first <= other->topo_last) << 2) |
                  ((topo_last >= other->topo_first) << 1) |
                  ((topo_last <= other->topo_last) << 0);
          switch(flag)
          {
          case b0000:
          case b0001:
          case b0010:
          case b0011:
          case b0100:
          case b0101:
          case b1000:
          case b1001:
          case b1010:
          case b1011:
          case b1100:
          case b1101:
          default:
            // disjoint ranges, nothing removed
            *_new = *this;
            return;
            break;
          case b0110:
            // split into two ranges...
            // convert to list...
            _new->__type = PAMI_LIST_TOPOLOGY;
            s = other->topo_first - topo_first +
                topo_last - other->topo_last;
            PAMI_assert_debugf(s != 0, "subtraction results in empty topology\n");
            rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                               (void **)&rl, 0, s * sizeof(*rl));
            PAMI_assertf(rc == PAMI_SUCCESS, "temp ranklist[%zd] alloc failed", s);
            k = 0;
            for(j = topo_first; j < other->topo_first; ++j)
            {
              rl[k++] = j;
            }
            for(j = other->topo_last + 1; j <= topo_last; ++j)
            {
              rl[k++] = j;
            }
            PAMI_assert_debug(k == s);
            _new->__size = s;
            _new->__free_ranklist =false;
            return;
            break;
          case b0111:
            // remove top of range...
            _new->__type = PAMI_RANGE_TOPOLOGY;
            _new->topo_first = topo_first;
            _new->topo_last = other->topo_first - 1;
            _new->__size = _new->topo_last - _new->topo_first + 1;
            _new->__free_ranklist =false;
            PAMI_assert_debugf(_new->__size != 0, "subtraction results in empty topology\n");
            return;
            break;
          case b1110:
            // remove bottom of range...
            _new->__type = PAMI_RANGE_TOPOLOGY;
            _new->topo_first = other->topo_last + 1;
            _new->topo_last = topo_last;
            _new->__size = _new->topo_last - _new->topo_first + 1;
            _new->__free_ranklist =false;
            PAMI_assert_debugf(_new->__size != 0, "subtraction results in empty topology\n");
            return;
            break;
          case b1111:
            // remove all...
            break;
          }
          break;
        case PAMI_LIST_TOPOLOGY:
          /// \todo keep this from being O(n^2)
          s = __size;
          rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                             (void **)&rl, 0, s * sizeof(*rl));
          PAMI_assertf(rc == PAMI_SUCCESS, "temp ranklist[%zd] alloc failed", s);
          k = 0;
          for(i = 0; i < s; ++i)
          {
            if(other->isRankMember(topo_list(i)))
            {
              continue;
            }
            // keep it...
            rl[k++] = topo_list(i);
          }
          if(k == 0)
          {
            PAMI::Memory::MemoryManager::heap_mm->free(rl);
            _new->__type = PAMI_EMPTY_TOPOLOGY;
            _new->__size = 0;
            _new->__free_ranklist =false;
            break;
          }
          _new->__type = PAMI_LIST_TOPOLOGY;
          _new->__size = k;
          _new->__free_ranklist =true;
          _new->topo_ranklist = rl;
          return;
          break;
        case PAMI_EMPTY_TOPOLOGY:
          break;
        default:
          break;
        }
      }
      else if(__type == PAMI_SINGLE_TOPOLOGY)
      {
        if(other->isRankMember(topo_rank))
        {
          _new->__type = PAMI_SINGLE_TOPOLOGY;
          _new->__size = 1;
          _new->__free_ranklist =false;
          _new->topo_rank = topo_rank;
          return;
        }
      }
      else if(other->__type == PAMI_SINGLE_TOPOLOGY)
      {
        switch(__type)
        {
        case PAMI_COORD_TOPOLOGY:
          if(isRankMember(other->topo_rank))
          {
            // convert rectangle to list and remove one...
            s = __size;
            rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                               (void **)&rl, 0, s * sizeof(*rl));
            PAMI_assertf(rc == PAMI_SUCCESS, "temp ranklist[%zd] alloc failed", s);
            k = 0;
            c0 = topo_llcoord;
            do
            {
              rc = COORDS2RANK(&c0, &rank);
              if(rank == other->topo_rank)
              {
                continue;
              }
              // could try to make a range...
              rl[k++] = rank;
            } while(__nextCoord(&c0, mapping->globalDims()));
            if(k == 0)
            {
              PAMI::Memory::MemoryManager::heap_mm->free(rl);
              break;
            }
            else if(k == 1)
            {
              _new->__type = PAMI_SINGLE_TOPOLOGY;
              _new->__size = 1;
              _new->__free_ranklist =false;
              _new->topo_rank = rl[0];
              PAMI::Memory::MemoryManager::heap_mm->free(rl);
              return;
            }
            _new->__type = PAMI_LIST_TOPOLOGY;
            _new->__size = k;
            _new->__free_ranklist =true;
            _new->topo_ranklist = rl;
            return;
          }
          else
          {
            *_new = *this;
            return;
          }
          break;
        case PAMI_RANGE_TOPOLOGY:
          if(isRankMember(other->topo_rank))
          {
            // convert range into list...
            s = __size;
            rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                               (void **)&rl, 0, s * sizeof(*rl));
            PAMI_assertf(rc == PAMI_SUCCESS, "temp ranklist[%zd] alloc failed", s);
            k = 0;
            for(i = topo_first; i < other->topo_rank; ++i)
            {
              rl[k++] = i;
            }
            for(++i; i <= topo_last; ++i)
            {
              rl[k++] = i;
            }
            if(k == 0) // probably never
            {
              PAMI::Memory::MemoryManager::heap_mm->free(rl);
              break;
            }
            else if(k == 1)
            {
              _new->__type = PAMI_SINGLE_TOPOLOGY;
              _new->__size = 1;
              _new->__free_ranklist =false;
              _new->topo_rank = rl[0];
              PAMI::Memory::MemoryManager::heap_mm->free(rl);
              return;
            }
            _new->__type = PAMI_LIST_TOPOLOGY;
            _new->__size = k;
            _new->__free_ranklist =true;
            _new->topo_ranklist = rl;
            return;
          }
          else
          {
            *_new = *this;
            return;
          }
          break;
        case PAMI_LIST_TOPOLOGY:
          s = __size;
          rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
                                                             (void **)&rl, 0, s * sizeof(*rl));
          PAMI_assertf(rc == PAMI_SUCCESS, "temp ranklist[%zd] alloc failed", s);
          k = 0;
          for(i = 0; i < s; ++i)
          {
            if(topo_list(i) != other->topo_rank)
            {
              rl[k++] = topo_list(i);
            }
          }
          if(k == 0) // probably never
          {
            PAMI::Memory::MemoryManager::heap_mm->free(rl);
            break;
          }
          else if(k == 1)
          {
            _new->__type = PAMI_SINGLE_TOPOLOGY;
            _new->__size = 1;
            _new->__free_ranklist =false;
            _new->topo_rank = rl[0];
            PAMI::Memory::MemoryManager::heap_mm->free(rl);
            return;
          }
          _new->__type = PAMI_LIST_TOPOLOGY;
          _new->__size = k;
          _new->__free_ranklist =true;
          _new->topo_ranklist = rl;
          return;
          break;
        case PAMI_EMPTY_TOPOLOGY:
          break;
        case PAMI_SINGLE_TOPOLOGY: // cannot happen - already checked
        default:
          break;
        }
      }
      else if(__type != PAMI_EMPTY_TOPOLOGY &&
              other->__type != PAMI_EMPTY_TOPOLOGY)
      {
        // more complicated scenarios - TBD
        switch(__type)
        {
        case PAMI_COORD_TOPOLOGY:
          switch(other->__type)
          {
          case PAMI_LIST_TOPOLOGY:
            break;
          case PAMI_RANGE_TOPOLOGY:
            break;
          case PAMI_SINGLE_TOPOLOGY: // already handled
          case PAMI_COORD_TOPOLOGY:  // already handled
          default:
            break;
          }
          break;
        case PAMI_LIST_TOPOLOGY:
          switch(other->__type)
          {
          case PAMI_COORD_TOPOLOGY:
            break;
          case PAMI_RANGE_TOPOLOGY:
            break;
          case PAMI_SINGLE_TOPOLOGY: // already handled
          case PAMI_LIST_TOPOLOGY:   // already handled
          default:
            break;
          }
          break;
        case PAMI_RANGE_TOPOLOGY:
          switch(other->__type)
          {
          case PAMI_COORD_TOPOLOGY:
            break;
          case PAMI_LIST_TOPOLOGY:
            break;
          case PAMI_SINGLE_TOPOLOGY: // already handled
          case PAMI_RANGE_TOPOLOGY:  // already handled
          default:
            break;
          }
          break;
        case PAMI_SINGLE_TOPOLOGY: // already handled
        default:
          break;
        }
      }
      _new->__type = PAMI_EMPTY_TOPOLOGY;
      _new->__size = 0;
    }

    void setClient_impl(pami_client_t c)
    {
      __client = c;
    }

    size_t endpoint2Index_impl(const pami_endpoint_t& ep)
    {
      size_t x;
      switch(__type)
      {
      case PAMI_EPLIST_TOPOLOGY:
        for(x = 0; x < __size; ++x)
        {
          if(ep == topo_eplist(x))
          {
            return x;
          }
        }
        break;
      default:
        pami_task_t    task;
        size_t          offset;
        PAMI_ENDPOINT_INFO(ep,task,offset);
        if(__all_contexts)
        {
          return rank2Index_impl(task) * __offset + offset;
        }
        else return rank2Index_impl(task);
      }
      return -1;
    }

    pami_endpoint_t index2Endpoint_impl(size_t ordinal)
    {
      if(__type == PAMI_EPLIST_TOPOLOGY)
      {
        //if we store explicitly the list of endpoints
        return __topo._eplist[ordinal];
      }
      pami_task_t task = this->index2Rank_impl(ordinal);
      pami_endpoint_t ep;
      if(__all_contexts)
      {
        size_t offset = ordinal % __offset;
        ep = PAMI_ENDPOINT_INIT(__client,task,offset);
      }
      else
        ep = PAMI_ENDPOINT_INIT(__client,task,__offset);
      return ep;
    }

    pami_endpoint_t rankContext2Endpoint_impl(pami_task_t task, size_t offset)
    {
      pami_endpoint_t ep;
      ep = PAMI_ENDPOINT_INIT(__client,task,offset);
      return ep;
    }

  private:
    size_t           __size;    ///< number of endpoints in this topology
    pami_topology_type_t __type;  ///< type of topology this is
    size_t           __offset; ///< context offset or number of contexts
    bool             __all_contexts; ///< flag to specify single or all contexts.
    union topology_u __topo;///< topoloy info
    bool             __free_ranklist;
    pami_client_t    __client;
  }; // class Topology

}; // namespace PAMI

#endif // __components_topology_default_bgqtopology_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
