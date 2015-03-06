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
#ifndef __common_bgp_BgpMapCache_h__
#define __common_bgp_BgpMapCache_h__
///
/// \file common/bgp/BgpMapCache.h
/// \brief Blue Gene/P Map- and Rank-Cache Object
///

#include "common/bgp/BgpPersonality.h"
#include "components/memory/MemoryManager.h"

/// \brief Creates valid index into _rankcache[].
///
/// It is imperative that all who create an index into _rankcache[]
/// do it in the identical way. Thus all should use this macro.
///
#define ESTIMATED_TASK(x,y,z,t,xSize,ySize,zSize,tSize) \
        (((t * zSize + z) * ySize + y) * xSize + x)

namespace PAMI
{
    class BgpMapCache
    {
      public:

        inline BgpMapCache () {};

        inline ~BgpMapCache () {};

        inline void init(BgpPersonality & personality,
                            PAMI::Memory::MemoryManager &mm
                            )
        {
          // This structure anchors pointers to the map cache and rank cache.
          // It is created in the static portion of shared memory in this
          // constructor, but exists there only for the duration of this
          // constructor.  It communicates mapping initialization information
          // to the other tasks running on this physical node.
          typedef struct cacheAnchors
          {
            volatile size_t * mapCachePtr; // Pointer to map cache
            volatile size_t *rankCachePtr; // Pointer to rank cache
            volatile size_t done[4];       // Indicators as to when each of the t coordinates
            // on our physical node are done extracting the
            // cache pointers from this structure in shared
            // memory (0 = not done, 1 = done.
            volatile size_t numActiveRanksLocal; // Number of ranks on our physical node.
            volatile size_t numActiveRanksGlobal;// Number of ranks in the partition.
            volatile size_t numActiveNodesGlobal;// Number of nodes in the partition.
            volatile size_t maxRank;       // Largest valid rank
            volatile size_t minRank;       // Smallest valid rank
            volatile pami_coord_t activeLLCorner;
            volatile pami_coord_t activeURCorner;
          } cacheAnchors_t;

          //size_t myRank, mySize;

          volatile cacheAnchors_t * cacheAnchorsPtr;
          bool meMaster = false;
          size_t tt, num_t = 0, rank, size;
          int err;

          //_processor_id = rts_get_processor_id();
          //_pers = & __global_personality;
          //_myNetworkCoord.network = PAMI_N_TORUS_NETWORK;
          //unsigned ix = 0;
          //_myNetworkCoord.n_torus.coords[ix++] = x();
          //_myNetworkCoord.n_torus.coords[ix++] = y();
          //_myNetworkCoord.n_torus.coords[ix++] = z();
          //_numGlobalDims = ix;
          //_myNetworkCoord.n_torus.coords[ix++] = t();
          //_numDims = ix;

          //PAMI_assert_debug(_numDims <= CM_MAX_DIMS);
          size_t tCoord = personality.tCoord ();
          size_t tsize  = personality.tSize ();

          // Calculate the number of potential ranks in this partition.
	  size_t fullSize = personality.xSize() *
				personality.ySize() *
				personality.zSize() *
				personality.tSize();

          // Use the static portion of the shared memory area (guaranteed to be zeros
          // at this point when it is in shared memory) to anchor pointers to
          // the caches.  Only the master rank will initialize these caches and set the
          // pointers into this structure.  When the non-master ranks on this physical
          // node see the non-zero pointers, they can begin to use them.
          pami_result_t result;
          result = mm.memalign((void **)&cacheAnchorsPtr, 16,
			sizeof(cacheAnchors_t), "/bgp-cacheAnchors");
	  PAMI_assertf(result == PAMI_SUCCESS, "alloc failed for cacheAnchorsPtr");

          // Determine if we are the master rank on our physical node.  Do this
          // by finding the lowest t coordinate on our node, and if it is us,
          // then we are the master.
          for (tt = 0; tt < tsize; tt++)
            {
              // See if there is a rank on our xyz with this t.
              int rc = Kernel_Coord2Rank (personality.xCoord(),
                                          personality.yCoord(),
                                          personality.zCoord(),
                                          tt, &rank, &size);

              if (rc == 0) // Found a t.
                {
                  // If this t is the first one we found, and it is us, then we are the
                  // master.
                  if ( (num_t == 0) && (tt == tCoord) )
                    {
                      meMaster = true;
                      cacheAnchorsPtr->done[tt] = 1; // Indicate this t is done so we
                      // (the master) don't wait on it
                      // later.
                    }

                  // If this t is us, save the rank and size in the mapping object.
                  if ( tt == tCoord )
                    {
                      _task = rank; // Save our task rank in the mapping object.
                      _size = size; // Save the size in the mapping object.
                    }

                  // Count the t's on our physical node.
                  num_t++;
                }
              // If there is no rank on this t, indicate that this t is done so we
              // don't wait on it later.
              else
                {
                  cacheAnchorsPtr->done[tt] = 1;
                }
            }
          // Note:  All nodes allocate the map and rank caches.  When in DUAL or VN modes,
          //        the allocator will return the same address in shared memory,
          //        effectively allocating the physically same buffer.  When in SMP mode,
          //        there is only one node that will allocate out of the heap.

          // Allocate the map cache from shared memory (in DUAL or VN modes) or from
          // heap (in SMP mode).
          result = mm.memalign((void **)&_mapcache, 16,
			sizeof(kernel_coords_t) * fullSize, "/bgp-_mapcache");
	  PAMI_assertf(result == PAMI_SUCCESS, "alloc failed for _mapcache");



          // Allocate the rank cache from shared memory (in DUAL or VN modes) or from
          // heap (in SMP mode).
          result = mm.memalign((void **)&_rankcache, 16,
			sizeof(size_t) * fullSize, "/bgp-_rankcache");
	  PAMI_assertf(result == PAMI_SUCCESS, "alloc failed for _rankcache");

          _max_rank = 0;
          _min_rank = (size_t) - 1;

          // If we are the master, then initialize the caches.
          // Then, set the cache pointers into the shared memory area for the other
          // ranks on this node to see, and wait for them to see it.
          if ( meMaster )
            {
              uint16_t rarray [72][32][32]; /* Full 72 rack partition */
              memset(rarray, 0, sizeof(rarray));

              memset(_rankcache, -1, sizeof(size_t) * fullSize);

              /* Fill in the _mapcache array in a single syscall.
               * It is indexed by rank, dimensioned to be the full size of the
               * partition (ignoring -np), and filled in with the xyzt
               * coordinates of each rank packed into a single 4 byte int.
               * Non-active ranks (-np) have x, y, z, and t equal to 255, such
               * that the entire 4 byte int is -1.
               */
              int rc = Kernel_Ranks2Coords((kernel_coords_t *)_mapcache, fullSize);

              /* The above system call is new in V1R3M0.  If it works, obtain info
               * from the returned _mapcache.
               */
              if (rc == 0)
                {
                  _ll.network = _ur.network = PAMI_N_TORUS_NETWORK;
                  _ll.u.n_torus.coords[0] = _ur.u.n_torus.coords[0] = personality.xCoord();
                  _ll.u.n_torus.coords[1] = _ur.u.n_torus.coords[1] = personality.yCoord();
                  _ll.u.n_torus.coords[2] = _ur.u.n_torus.coords[2] = personality.zCoord();
                  _ll.u.n_torus.coords[3] = _ur.u.n_torus.coords[3] = personality.tCoord();

                  /* Obtain the following information from the _mapcache:
                   * 1. Number of active ranks in the partition.
                   * 2. Number of active compute nodes in the partition.
                   * 3. _rankcache (the reverse of _mapcache).  It is indexed by
                   *    coordinates and contains the rank.
                   * 4. Number of active ranks on each compute node.
                   */
                  size_t i;

                  for (i = 0; i < fullSize; i++)
                    {
                      if ( (int)_mapcache[i] != -1 )
                        {
                          size_t x, y, z, t;
                          kernel_coords_t mapCacheElement = *(kernel_coords_t*) & _mapcache[i];
                          x = mapCacheElement.x;
                          y = mapCacheElement.y;
                          z = mapCacheElement.z;
                          t = mapCacheElement.t;

                          // Increment the rank count on this node.
                          rarray[x][y][z]++;

                          // Increment the number of global ranks.
                          cacheAnchorsPtr->numActiveRanksGlobal++;

                          // If the rank count on this node is '1', this is the first
                          // rank encountered on this node. Increment the number of active nodes.
                          if (rarray[x][y][z] == 1) cacheAnchorsPtr->numActiveNodesGlobal++;

                          size_t estimated_rank = ESTIMATED_TASK(x,y,z,t,personality.xSize(),personality.ySize(),personality.zSize(),personality.tSize());

                          _rankcache[estimated_rank] = i;

                          // because of "for (i..." this will give us MAX after loop.
                          _max_rank = i;

                          if (_min_rank == (size_t) - 1) _min_rank = i;

                          if (x < _ll.u.n_torus.coords[0]) _ll.u.n_torus.coords[0] = x;
                          if (y < _ll.u.n_torus.coords[1]) _ll.u.n_torus.coords[1] = y;
                          if (z < _ll.u.n_torus.coords[2]) _ll.u.n_torus.coords[2] = z;
                          if (t < _ll.u.n_torus.coords[3]) _ll.u.n_torus.coords[3] = t;
                          if (x > _ur.u.n_torus.coords[0]) _ur.u.n_torus.coords[0] = x;
                          if (y > _ur.u.n_torus.coords[1]) _ur.u.n_torus.coords[1] = y;
                          if (z > _ur.u.n_torus.coords[2]) _ur.u.n_torus.coords[2] = z;
                          if (t > _ur.u.n_torus.coords[3]) _ur.u.n_torus.coords[3] = t;

                        }
                    }

                  cacheAnchorsPtr->maxRank = _max_rank;
                  cacheAnchorsPtr->minRank = _min_rank;
                  // why can't this just be assigned???
                  memcpy((void *)&cacheAnchorsPtr->activeLLCorner, &_ll, sizeof(_ll));
                  memcpy((void *)&cacheAnchorsPtr->activeURCorner, &_ur, sizeof(_ur));
                }
              /* If the system call fails, assume the kernel is older and does not
               * have this system call.  Use the original system call, one call per
               * rank (which is slower than the single new system call) to obtain
               * the information necessary to fill in the _mapcache, etc.
               */
              else
                {
                  size_t i;

                  for (i = 0; i < fullSize; i++)
                    {
                      unsigned x, y, z, t;
                      err = Kernel_Rank2Coord ((int)i, &x, &y, &z, &t);

                      if (err == 0)
                        {
                          _mapcache[i] = ((x & 0xFF) << 24) | ((y & 0xFF) << 16) | ((z & 0xFF) << 8) | (t & 0xFF);
                          rarray[x][y][z]++;
                          cacheAnchorsPtr->numActiveRanksGlobal++;

                          if (rarray[x][y][z] == 1)
                            cacheAnchorsPtr->numActiveNodesGlobal++;

                          size_t estimated_rank = ESTIMATED_TASK(x,y,z,t,personality.xSize(),personality.ySize(),personality.zSize(),personality.tSize());
                          _rankcache[estimated_rank] = i;
                        }
                      else
                        {
                          _mapcache[i] = (unsigned) - 1;
                        }
                    }

                  cacheAnchorsPtr->numActiveRanksLocal =
                    rarray[personality.xCoord()][personality.yCoord()][personality.zCoord()];
                }

              // Now that the map and rank caches have been initialized,
              // store their pointers into the shared memory cache pointer area so the
              // other nodes see these pointers.
              cacheAnchorsPtr->mapCachePtr  = _mapcache;
              cacheAnchorsPtr->rankCachePtr = _rankcache;

              // Copy the rank counts into the mapping object.
              //_numActiveRanksLocal  = cacheAnchorsPtr->numActiveRanksLocal;
              //_numActiveRanksGlobal = cacheAnchorsPtr->numActiveRanksGlobal;
              //_numActiveNodesGlobal = cacheAnchorsPtr->numActiveNodesGlobal;

              _bgp_mbar();  // Ensure that stores to memory are in the memory.

              // Wait until the other t's on our physical node have seen the cache
              // pointers.
              for (tt = 0; tt < tsize; tt++)
                {
                  while ( cacheAnchorsPtr->done[tt] == 0 )
                    {
                      _bgp_msync();
                    }
                }

              // Now that all nodes have seen the cache pointers, zero out the cache
              // anchor structure for others who expect this area to be zero.
              memset ((void*)cacheAnchorsPtr, 0x00, sizeof(cacheAnchors_t));

            } // End: Allocate an initialize the map and rank caches.


          // We are not the master t on our physical node.  Wait for the master t to
          // initialize the caches.  Then grab the pointers and rank
          // counts, and then indicate we have seen them.
          else
            {
              while ( cacheAnchorsPtr->mapCachePtr == NULL )
                {
                  _bgp_msync();
                }

              _mapcache = (unsigned*)(cacheAnchorsPtr->mapCachePtr);

              while ( cacheAnchorsPtr->rankCachePtr == NULL )
                {
                  _bgp_msync();
                }

              _rankcache = (unsigned*)(cacheAnchorsPtr->rankCachePtr);

              //_numActiveRanksLocal  = cacheAnchorsPtr->numActiveRanksLocal;
              //_numActiveRanksGlobal = cacheAnchorsPtr->numActiveRanksGlobal;
              //_numActiveNodesGlobal = cacheAnchorsPtr->numActiveNodesGlobal;
              _max_rank = cacheAnchorsPtr->maxRank;
              _min_rank = cacheAnchorsPtr->minRank;
              // why can't these just be assigned???
              memcpy(&_ll, (void *)&cacheAnchorsPtr->activeLLCorner, sizeof(_ll));
              memcpy(&_ur, (void *)&cacheAnchorsPtr->activeURCorner, sizeof(_ur));
              _bgp_mbar();

              cacheAnchorsPtr->done[personality.tCoord()] = 1;  // Indicate we have seen the info.
            }
        };

        inline size_t * getMapCache ()
        {
          return _mapcache;
        }

        inline size_t * getRankCache ()
        {
          return _rankcache;
        };

        inline size_t getTask ()
        {
          return _task;
        };

        inline size_t getSize ()
        {
          return _size;
        };

        inline void getMappingInit(pami_coord_t &ll, pami_coord_t &ur, size_t &min, size_t &max) {
                ll = _ll;
                ur = _ur;
                min = _min_rank;
                max = _max_rank;
        }

      private:

        size_t   _size;
        size_t   _task;
        size_t * _mapcache;
        size_t * _rankcache;

        size_t _max_rank;
        size_t _min_rank;
        pami_coord_t _ll;
        pami_coord_t _ur;

    }; // class BgpMapCache
};     // namespace PAMI

#endif // __pami_components_mapping_bgp_bgpmapcache_h__
