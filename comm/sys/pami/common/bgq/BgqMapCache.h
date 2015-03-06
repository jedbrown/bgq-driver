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
#ifndef __common_bgq_BgqMapCache_h__
#define __common_bgq_BgqMapCache_h__
///
/// \file common/bgq/BgqMapCache.h
/// \brief Blue Gene/Q Map- and Rank-Cache Object
///

#include <hwi/include/bqc/A2_inlines.h>

#include "common/bgq/BgqPersonality.h"

#include <spi/include/kernel/location.h>

/// \brief Creates valid index into _rankcache[].
///
/// It is imperative that all who create an index into _rankcache[]
/// do it in the identical way. Thus all should use this macro.
///
#define ESTIMATED_TASK(a,b,c,d,e,t,aSize,bSize,cSize,dSize,eSize,tSize) \
(((((a * bSize + b) * cSize + c)* dSize + d) * eSize + e) * tSize + t)



namespace PAMI
{
  ///
  /// \brief Blue Gene/Q coordinate structure
  ///
  /// This structure takes 32-bits on any 32/64 bit system. The a, b, c,
  /// and d fields are the same size and in the same location as the MU
  /// descriptor structure. The e dimension is sized to the current
  /// node-layout maximum (1 bit), though the MU hardware supports the full
  /// 6 bits.
  ///
  /// 1.  The reserved bit is used by BGQ Mapping to indicate whether the
  ///     task is local to our node.  1=Local, 0=Not local.
  /// 2.  The high-order bit of the A, B, C, and D coordinates is used to
  ///     produce a 4 bit value that contains the optimal MU injection fifo
  ///     number (see pinFifo).
  /// 3.  The next highest-order bit of the D coordinate is used for rget
  ///     pacing.  When set, it indicates that rgets should be paced
  ///     to this destination.
  /// 4.  The next highest-order bit of the C coordinate is used to specify
  ///     the routing to be used for rgets.  There are 2 routings, based on
  ///     the flexability metric.  __global.getFlexabilityMetricRoutingOptions()
  ///     returns a 2 element array containing the routings.  This bit is used
  ///     as an index into that array.
  ///
  /// Here's a picture of the bits:
  ///
  /// Bit      Description
  /// 0        E coordinate
  /// 1        0=dest is not local to the node
  ///          1=dest is local to the node
  /// 2        First bit of the pinFifo value
  /// 3-7      A coordinate
  /// 8        Second bit of the pinFifo value
  /// 9-13     B coordinate
  /// 14       Third bit of the pinFifo value
  /// 15       Routing bit.  0=Use routing 0.  1=Use routing 1.  See above.
  /// 16-19    C coordinate
  /// 20       Fourth bit of the pinFifo value
  /// 21       0=rgets should not be paced to this dest
  ///          1=rgets should be paced to this dest
  /// 22-25    D coordinate
  /// 26-31    T coordinate
  ///
  /// \see MUHWI_Destination_t
  ///
  typedef union bgq_coords
  {
    BG_CoordinateMapping_t mapped; ///< mapped A/B/C/D/E/T coords
    uint32_t   raw;          ///< Raw memory storage
  } bgq_coords_t;

  typedef struct
  {
    struct
    {
      bgq_coords_t * task2coords;
      uint32_t     * coords2task;
    } torus;
    struct
    {
      size_t       * local2peer;
      size_t       * peer2task;
    } node;
    size_t  size;
    size_t  local_size;//hack
    size_t  lowestTCoordOnMyNode;
    size_t  numActiveNodesGlobal;
  } bgq_mapcache_t;

  class BgqMapCache
  {
    public:

      inline BgqMapCache ()
      {
      };

      inline ~BgqMapCache ()
      {
      };

      inline size_t init (PAMI::BgqJobPersonality & personality,
                          void                   * ptr,
                          size_t                   bytes
                         )
      {
        return 0;
      };


  }; // PAMI::BgqMapCache
};     // PAMI

#endif // __pami_components_mapping_bgq_bgqmapcache_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
