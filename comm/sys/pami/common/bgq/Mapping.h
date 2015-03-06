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
/// \file common/bgq/Mapping.h
/// \brief ???
///
#ifndef __common_bgq_Mapping_h__
#define __common_bgq_Mapping_h__

#include <hwi/include/bqc/MU_PacketCommon.h>

#include <pami.h>
#include "Platform.h"

#include "common/BaseMappingInterface.h"
#include "common/TorusMappingInterface.h"
#include "common/NodeMappingInterface.h"

#include "common/bgq/BgqPersonality.h"
#include "common/bgq/BgqMapCache.h"

#include "util/common.h"

#define PAMI_MAPPING_CLASS PAMI::Mapping

#undef TRACE_ERR
#ifndef TRACE_ERR
  #define TRACE_ERR(x) //fprintf x
#endif

namespace PAMI
{
#define BGQ_TDIMS 5
#define BGQ_LDIMS 1
  class Mapping :
  public Interface::Mapping::Base<Mapping>,
  public Interface::Mapping::Torus<Mapping, BGQ_TDIMS>,
  public Interface::Mapping::Node<Mapping,BGQ_LDIMS>
  {
  public:

    inline Mapping (PAMI::BgqJobPersonality &pers) :
    Interface::Mapping::Base<Mapping>(),
    Interface::Mapping::Torus<Mapping, BGQ_TDIMS>(),
    Interface::Mapping::Node<Mapping,BGQ_LDIMS> (),
    _pers(pers),
    _task((size_t)-1),
    _a (_pers.aCoord()), // These are relative to the sub-block (job's lower-left).
    _b (_pers.bCoord()),
    _c (_pers.cCoord()),
    _d (_pers.dCoord()),
    _e (_pers.eCoord()),
    _t (_pers.tCoord())
    {
      // ----------------------------------------------------------------
      // Compile-time assertions
      // ----------------------------------------------------------------

      // Make sure that the coords structure and the MUHWI_Destination_t
      // structure match
      COMPILE_TIME_ASSERT(sizeof(bgq_coords_t) == sizeof(MUHWI_Destination_t));

      // _myCoords are relative to the job's lower-left corner.
      _myCoords.mapped.t        = _t;
      _myCoords.mapped.a        = _a;
      _myCoords.mapped.b        = _b;
      _myCoords.mapped.c        = _c;
      _myCoords.mapped.d        = _d;
      _myCoords.mapped.e        = _e;
      _myCoords.mapped.reserved =  0;

      // Get the coordinates of the job relative to the block.
      // The block lower-left (LL) coords are 0,0,0,0,0.
      pami_coord_t jobLL, jobUR;
      _pers.jobRectangle( jobLL, jobUR );

      // Save the job's lower-left corner coords in map-cache format.
      // This will be added to the map cache element to produce block-relative
      // coordinates needed by the MU.
      _jobLLcoords.mapped.reserved = 0;
      _jobLLcoords.mapped.a        = jobLL.u.n_torus.coords[0];
      _jobLLcoords.mapped.b        = jobLL.u.n_torus.coords[1];
      _jobLLcoords.mapped.c        = jobLL.u.n_torus.coords[2];
      _jobLLcoords.mapped.d        = jobLL.u.n_torus.coords[3];
      _jobLLcoords.mapped.e        = jobLL.u.n_torus.coords[4];
      _jobLLcoords.mapped.t        = 0;

      // Save our MU coordinates relative to the block LL...that's what the MU needs.
      _myMUcoords.Destination.Reserved2     = 0;
      _myMUcoords.Destination.A_Destination = _a + jobLL.u.n_torus.coords[0];
      _myMUcoords.Destination.B_Destination = _b + jobLL.u.n_torus.coords[1];
      _myMUcoords.Destination.C_Destination = _c + jobLL.u.n_torus.coords[2];
      _myMUcoords.Destination.D_Destination = _d + jobLL.u.n_torus.coords[3];
      _myMUcoords.Destination.E_Destination = _e + jobLL.u.n_torus.coords[4];
      
      TRACE_ERR((stderr,"_myMUcoords=%u,%u,%u,%u,%u  _jobLLcoords=%u,%u,%u,%u,%u\n",_myMUcoords.Destination.A_Destination,_myMUcoords.Destination.B_Destination,_myMUcoords.Destination.C_Destination,_myMUcoords.Destination.D_Destination,_myMUcoords.Destination.E_Destination,_jobLLcoords.mapped.a,_jobLLcoords.mapped.b,_jobLLcoords.mapped.c,_jobLLcoords.mapped.d,_jobLLcoords.mapped.e));

      coord2node (_a, _b, _c, _d, _e, _t,      //fix?
                  _nodeaddr.global, _nodeaddr.local);

      _torusInfo[0]= _pers.isTorusA();
      _torusInfo[1]= _pers.isTorusB();
      _torusInfo[2]= _pers.isTorusC();
      _torusInfo[3]= _pers.isTorusD();
      _torusInfo[4]= _pers.isTorusE();

      _pers.location (_location);

      TRACE_ERR((stderr, "Mapping() coords(%zu %zu %zu %zu %zu %zu), node: (%#lX %#lX)\n", _a, _b, _c, _d, _e, _t, _nodeaddr.global, _nodeaddr.local));
      TRACE_ERR((stderr, "Mapping() torusInfo(%u %u %u %u %u )", _torusInfo[0],_torusInfo[1],_torusInfo[2],_torusInfo[3],_torusInfo[4]));

    };

    inline ~Mapping ()
    {
    };

    const char * getLocation ()
    {
      return (const char *) _location;
    };

  protected:
    PAMI::BgqJobPersonality &_pers;
    size_t _task;
//    size_t _nodes;
//    size_t _peers;

    // These are relative to the sub-block (job's lower-left).
    size_t _a;
    size_t _b;
    size_t _c;
    size_t _d;
    size_t _e;
    size_t _t;

    char _torusInfo[BGQ_TDIMS];

    // _myCoords are relative to the sub-block (job), while
    // _jobLLcoords and _myMUcoords are relative to the block (as needed by the MU).
    bgq_coords_t        _myCoords;
    MUHWI_Destination_t _myMUcoords;
    bgq_coords_t        _jobLLcoords;

    Interface::Mapping::nodeaddr_t _nodeaddr;

    bgq_mapcache_t _mapcache;
    char _location[256];

    inline void coord2node (size_t   a,
                            size_t   b,
                            size_t   c,
                            size_t   d,
                            size_t   e,
                            size_t   t,
                            size_t & global,
                            size_t & local)
    {
      global = e | (d << 1) | (c << 7) | (b << 13) | (a << 19);
      local  = t;
      TRACE_ERR((stderr, "Mapping() coords2node((%zu %zu %zu %zu %zu %zu),(%#lX %#lX)\n", a, b, c, d, e, t, global, local));
    };

    inline void node2coord (size_t & a,
                            size_t & b,
                            size_t & c,
                            size_t & d,
                            size_t & e,
                            size_t & t,
                            size_t   global,
                            size_t   local)
    {
      e = global         & 0x00000001;
      d = (global >> 1)  & 0x0000003f;
      c = (global >> 7)  & 0x0000003f;
      b = (global >> 13) & 0x0000003f;
      a = global >> 19;

      t = local;

      TRACE_ERR((stderr, "Mapping() node2coord((%zu %zu %zu %zu %zu %zu),(%#lX %#lX)\n", a, b, c, d, e, t, global, local));

    };

  public:

    inline pami_result_t task2peer_impl(size_t task, size_t &peer)
      {
        Interface::Mapping::nodeaddr_t address;
        task2node(task,address);
        node2peer(address,peer);
        return PAMI_SUCCESS;
      }


    ///
    /// \brief Retrieve pointer to a mu destination structure for this task
    ///
    /// The intent here is to provide access to a mu destination structure
    /// initialized to the coordinates of the local node.
    ///
    inline MUHWI_Destination_t * getMuDestinationSelf ()
    {
      return(MUHWI_Destination_t *) &_myMUcoords;
    };

    ///
    /// \brief Retrieve mu destination structure for a specific task
    ///
    /// The intent here is to provide access to a mu destination structure
    /// initialized to the coordinates of the destination node.
    ///
    /// \note Does not provide process, core, or hardware thread addressing.
    ///
    inline void getMuDestinationTask (size_t               task,
                                      MUHWI_Destination_t &dest,
                                      size_t              &tcoord,
                                      uint32_t            &fifoPin)
    {
      uint32_t raw = _mapcache.torus.task2coords[task].raw;
      raw += _jobLLcoords.raw; // Add block origin to obtain MU coords.

      tcoord       = (size_t) raw & 0x0000003f; // 't' coordinate

      // raw & 0x1f7cf3c0 turns off the e, reserved, and t coordinate bits, AND
      // the high bit of A, B, C, and D which are used for the fifoPin AND
      // the next highest bit of D which is used for rget pacing AND
      // the next highest bit of C which is used for routing.
      // OR in the e coord at the LSB.
      dest.Destination.Destination = (raw & 0x1f7cf3c0) | (raw >> 31);

      // Extract the MSB from each of A, B, C, and D, and construct the
      // number used for fifo pinning.  Should be a number between 0 and 9,
      // inclusive.
      fifoPin = ( ( (raw & 0x20000000) >> 26 ) |
                  ( (raw & 0x00800000) >> 21 ) |
                  ( (raw & 0x00020000) >> 16 ) |
                  ( (raw & 0x00000800) >> 11 ) );
    };

    ///
    /// \brief Retrieve mu destination structure for a specific task with rget pacing indicator
    ///
    /// The intent here is to provide access to a mu destination structure
    /// initialized to the coordinates of the destination node.
    ///
    /// \note Does not provide process, core, or hardware thread addressing.
    ///
    inline void getMuDestinationTask (size_t               task,
                                      MUHWI_Destination_t &dest,
                                      size_t              &tcoord,
                                      uint32_t            &fifoPin,
				      uint32_t            &paceRgetsToThisDest,
                                      uint32_t            &routingIndex)
    {
      uint32_t raw = _mapcache.torus.task2coords[task].raw;
      raw += _jobLLcoords.raw; // Add block origin to obtain MU coords.

      tcoord       = (size_t) raw & 0x0000003f; // 't' coordinate

      // raw & 0x1f7cf3c0 turns off the e, reserved, and t coordinate bits, AND
      // the high bit of A, B, C, and D which are used for the fifoPin AND
      // the next highest bit of D which is used for rget pacing AND
      // the next highest bit of C which is used for routing.
      // OR in the e coord at the LSB.
      dest.Destination.Destination = (raw & 0x1f7cf3c0) | (raw >> 31);

      // Extract the MSB from each of A, B, C, and D, and construct the
      // number used for fifo pinning.  Should be a number between 0 and 9,
      // inclusive.
      fifoPin = ( ( (raw & 0x20000000) >> 26 ) |
                  ( (raw & 0x00800000) >> 21 ) |
                  ( (raw & 0x00020000) >> 16 ) |
                  ( (raw & 0x00000800) >> 11 ) );

      // Extract the rget pacing indicator from the map cache.
      paceRgetsToThisDest = raw & 0x00000400;

      // Extract the routing index from the map cache.
      routingIndex = ( raw & 0x00010000 ) >> 16;
    };

    ///
    /// \brief Retrieve mu destination structure for a specific task
    ///
    /// The intent here is to provide access to a mu destination structure
    /// initialized to the coordinates of the destination node.
    ///
    /// \note Does not provide process, core, or hardware thread addressing.
    ///
    inline void getMuDestinationTask (size_t               task,
                                      uint32_t            &dest,
                                      size_t              &tcoord,
                                      uint32_t            &fifoPin)
    {
      uint32_t raw = _mapcache.torus.task2coords[task].raw;
      raw += _jobLLcoords.raw; // Add block origin to obtain MU coords.

      tcoord       = (size_t) raw & 0x0000003f; // 't' coordinate

      // raw & 0x1f7cf3c0 turns off the e, reserved, and t coordinate bits, AND
      // the high bit of A, B, C, and D which are used for the fifoPin AND
      // the next highest bit of D which is used for rget pacing AND
      // the next highest bit of C which is used for routing.
      // OR in the e coord at the LSB.
      dest = (raw & 0x1f7cf3c0) | (raw >> 31);

      // Extract the MSB from each of A, B, C, and D, and construct the
      // number used for fifo pinning.  Should be a number between 0 and 9,
      // inclusive.
      fifoPin = ( ( (raw & 0x20000000) >> 26 ) |
                  ( (raw & 0x00800000) >> 21 ) |
                  ( (raw & 0x00020000) >> 16 ) |
                  ( (raw & 0x00000800) >> 11 ) );
    };


    inline void getMuDestinationTask (size_t               task,
                                      MUHWI_Destination_t &dest)
    {
      uint32_t raw = _mapcache.torus.task2coords[task].raw;
      raw += _jobLLcoords.raw; // Add block origin to obtain MU coords.

      // raw & 0x1f7cf3c0 turns off the e, reserved, and t coordinate bits, AND
      // the high bit of A, B, C, and D which are used for the fifoPin AND
      // the next highest bit of D which is used for rget pacing AND
      // the next highest bit of C which is used for routing.
      // OR in the e coord at the LSB.
      dest.Destination.Destination = (raw & 0x1f7cf3c0) | (raw >> 31);
    };

    ///
    /// \brief Set Fifo Pinning Info into the Mapcache
    ///
    /// The intent here is to provide access to a mu destination structure
    /// initialized to the coordinates of the destination node.
    ///
    /// \note Does not provide process, core, or hardware thread addressing.
    ///
    inline void setFifoPin ( size_t   task,
                             uint16_t fifo )
    {
      // OR in the fifo pin bits.
      // The LSB 4 bits of the fifo pin are placed into the high order bit of
      // the A, B, C, and D coords in the structure.
      _mapcache.torus.task2coords[task].raw |=
        ( ((fifo & 0x8) << 26) |
          ((fifo & 0x4) << 21) |
          ((fifo & 0x2) << 16) |
          ((fifo & 0x1) << 11) );
    };

    /////////////////////////////////////////////////////////////////////////
    //
    // PAMI::Interface::Mapping::Base interface implementation
    //
    /////////////////////////////////////////////////////////////////////////

    ///
    /// \brief Initialize the mapping
    /// \see PAMI::Interface::Mapping::Base::init()
    ///
    inline pami_result_t init(bgq_mapcache_t &mapcache,
                             PAMI::BgqJobPersonality &personality);

    ///
    /// \brief Return the BGP global task for this process
    /// \see PAMI::Interface::Mapping::Base::task()
    ///
    inline size_t task_impl()
    {
      return _task;
    }

    ///
    /// \brief Returns the number of global tasks
    /// \see PAMI::Interface::Mapping::Base::size()
    ///
    inline size_t size_impl()
    {
      return _mapcache.size;
    }

    ///
    /// \brief Returns the number of global dimensions
    /// \see PAMI::Interface::Mapping::Base::globalDims()
    ///
    inline size_t globalDims_impl()
    {
      return BGQ_TDIMS + BGQ_LDIMS;
    }

    /////////////////////////////////////////////////////////////////////////
    //
    // PAMI::Interface::Mapping::Torus interface implementation
    //
    /////////////////////////////////////////////////////////////////////////

    ///
    /// \brief Get the specified torus dimension for this task
    /// \see PAMI::Interface::Mapping::Torus::torusgetcoord()
    ///
    inline size_t       torusgetcoord_impl (size_t dimension)
    {
      if(dimension >= BGQ_TDIMS + BGQ_LDIMS)
        PAMI_abortf("dimension %zu requested\n",dimension);
      size_t addr[BGQ_TDIMS + BGQ_LDIMS];
      torusAddr_impl(addr);
      return addr[dimension];
    }
    ///
    /// \brief Get the BGQ torus address for this task
    /// \see PAMI::Interface::Mapping::Torus::torusAddr()
    ///
    //template <>
    inline void torusAddr_impl (size_t (&addr)[BGQ_TDIMS + BGQ_LDIMS])
    {
      addr[0] = _a;
      addr[1] = _b;
      addr[2] = _c;
      addr[3] = _d;
      addr[4] = _e;
      addr[5] = _t;
      TRACE_ERR((stderr, "Mapping::torusAddr_impl(%zu, %zu, %zu, %zu, %zu, %zu}\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]));
    }

    ///
    /// \brief Get the BGQ torus address for a task
    /// \see PAMI::Interface::Mapping::Torus::task2torus()
    ///
    /// \todo Error path
    ///
    inline pami_result_t task2torus_impl (size_t task, size_t (&addr)[BGQ_TDIMS])
    {
      uint32_t abcdet = _mapcache.torus.task2coords[task].raw;

      // Mask off the high bit of ABCD, since it is used for fifo pinning,
      // and the next highest bit of D, since it is used for rget pacing,
      // and the next highest bit of C, since it is used for routing.
      addr[0] = (abcdet >> 24) & 0x00000001f; // 'a' coordinate
      addr[1] = (abcdet >> 18) & 0x00000001f; // 'b' coordinate
      addr[2] = (abcdet >> 12) & 0x00000000f; // 'c' coordinate
      addr[3] = (abcdet >>  6) & 0x00000000f; // 'd' coordinate
      addr[4] = (abcdet >> 31); // 'e' coordinate

      TRACE_ERR((stderr, "Mapping::task2torus(%zu, {%zu, %zu, %zu, %zu, %zu}) <<\n", task, addr[0], addr[1], addr[2], addr[3], addr[4]));
      return PAMI_SUCCESS;
    }

    ///
    /// \brief Get the BGQ torus address for a task
    /// \see PAMI::Interface::Mapping::Torus::task2torus()
    ///
    /// \todo Error path
    ///
    inline pami_result_t task2global (size_t task, size_t (&addr)[BGQ_TDIMS + BGQ_LDIMS])
    {
      uint32_t abcdet = _mapcache.torus.task2coords[task].raw;
      TRACE_ERR((stderr, "Mapping::task2global(%zu, ...),  _mapcache.torus.task2coords[%zu].raw = 0x%08x\n", task, task, abcdet));

      // Mask off the high bit of ABCD since it is used for fifo pinning,
      // and the next highest bit of D, since it is used for rget pacing,
      // and the next highest bit of C, since it is used for routing.
      addr[0] = (abcdet >> 24) & 0x00000001f; // 'a' coordinate
      addr[1] = (abcdet >> 18) & 0x00000001f; // 'b' coordinate
      addr[2] = (abcdet >> 12) & 0x00000000f; // 'c' coordinate
      addr[3] = (abcdet >>  6) & 0x00000000f; // 'd' coordinate
      addr[4] = (abcdet >> 31)              ; // 'e' coordinate
      addr[5] = (abcdet)       & 0x00000003f; // 't' coordinate

      TRACE_ERR((stderr, "Mapping::task2global(%zu, {%zu, %zu, %zu, %zu, %zu, %zu}) <<\n", task, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]));
      return PAMI_SUCCESS;
    }

    ///
    /// \brief Get the global task for a BGQ torus address
    /// \see PAMI::Interface::Mapping::Torus::torus2task()
    ///
    /// \todo Error path
    ///
    /// \todo why is there task2global but no global2task?  stub it in for Rectangle.h
    inline pami_result_t global2task (size_t (&addr)[BGQ_TDIMS + BGQ_LDIMS], size_t & task)
    {
      return torus2task_impl (addr,task);
    }
    inline pami_result_t torus2task_impl (size_t (&addr)[BGQ_TDIMS + BGQ_LDIMS], size_t & task)
    {

      size_t aSize = _pers.aSize();
      size_t bSize = _pers.bSize();
      size_t cSize = _pers.cSize();
      size_t dSize = _pers.dSize();
      size_t eSize = _pers.eSize();
      size_t tSize = _pers.tSize();

      // Verify that the bgq address is valid.
      if (unlikely((addr[0] >= aSize) ||
                   (addr[1] >= bSize) ||
                   (addr[2] >= cSize) ||
                   (addr[3] >= dSize) ||
                   (addr[4] >= eSize) ||
                   (addr[5] >= tSize)))
      {
        return PAMI_INVAL;
      }

      // Estimate the task id based on the bgq coordinates.
      size_t hash = ESTIMATED_TASK(addr[0],addr[1],addr[2],addr[3],addr[4],addr[5],
                                   aSize,bSize,cSize,dSize,eSize,tSize);

      // Verify that the estimated task is mapped.
      if (unlikely(_mapcache.torus.coords2task[hash] == (unsigned) - 1))
      {
        return PAMI_ERROR;
      }

      task = _mapcache.torus.coords2task[hash];

      TRACE_ERR((stderr, "Mapping::torus2task_impl({%zu, %zu, %zu, %zu, %zu, %zu}, %zu) <<\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], task));

      return PAMI_SUCCESS;
    };

    ///
    /// \brief Get the BGQ network address for a task
    /// \see PAMI::Interface::Mapping::Torus::task2network()
    ///
    /// \todo Error path
    ///
    inline pami_result_t task2network_impl (pami_task_t task,
                                           pami_coord_t *addr,
                                           pami_network type)
    {
      TRACE_ERR((stderr,"task2network %d\n",task));
      uint32_t abcdet = _mapcache.torus.task2coords[task].raw;
      addr->network = PAMI_N_TORUS_NETWORK;
      // Mask off the high bit of ABCD since it is used for fifo pinning,
      // and the next highest bit of D, since it is used for rget pacing,
      // and the next highest bit of C, since it is used for routing.
      addr->u.n_torus.coords[0] = (abcdet >> 24) & 0x00000001f; // 'a' coordinate
      addr->u.n_torus.coords[1] = (abcdet >> 18) & 0x00000001f; // 'b' coordinate
      addr->u.n_torus.coords[2] = (abcdet >> 12) & 0x00000000f; // 'c' coordinate
      addr->u.n_torus.coords[3] = (abcdet >>  6) & 0x00000000f; // 'd' coordinate
      addr->u.n_torus.coords[4] = (abcdet >> 31)              ; // 'e' coordinate
      addr->u.n_torus.coords[5] = (abcdet)       & 0x00000003f; // 't' coordinate
      TRACE_ERR((stderr, "Mapping::task2network_impl(%d, {%zu, %zu, %zu, %zu, %zu, %zu}, %d) <<\n", task, addr->u.n_torus.coords[0], addr->u.n_torus.coords[1], addr->u.n_torus.coords[2], addr->u.n_torus.coords[3], addr->u.n_torus.coords[4], addr->u.n_torus.coords[5], addr->network));
      return PAMI_SUCCESS;
    }

    ///
    /// \brief Get the BGQ network address for a task
    /// \see PAMI::Interface::Mapping::Torus::network2task()
    ///
    /// \todo Error path
    ///
    inline pami_result_t network2task_impl (const pami_coord_t *addr,
                                           pami_task_t *task,
                                           pami_network *type)
    {
      size_t aSize = _pers.aSize();
      size_t bSize = _pers.bSize();
      size_t cSize = _pers.cSize();
      size_t dSize = _pers.dSize();
      size_t eSize = _pers.eSize();
      size_t tSize = _pers.tSize();
      size_t a = addr->u.n_torus.coords[0];
      size_t b = addr->u.n_torus.coords[1];
      size_t c = addr->u.n_torus.coords[2];
      size_t d = addr->u.n_torus.coords[3];
      size_t e = addr->u.n_torus.coords[4];
      size_t t = addr->u.n_torus.coords[5];
      // Verify that the bgq address is valid.
      // TODO convert to "unlikely if"
      if ((a >= aSize) ||
          (b >= bSize) ||
          (c >= cSize) ||
          (d >= dSize) ||
          (e >= eSize) ||
          (t >= tSize))
      {
        return PAMI_INVAL;
      }

      size_t hash = ESTIMATED_TASK(a,b,c,d,e,t,
                                   aSize,bSize,cSize,dSize,eSize,tSize);

      if (_mapcache.torus.coords2task[hash] == (uint32_t)-1)
      {
        return PAMI_ERROR;
      }
      *task = _mapcache.torus.coords2task[hash];
      *type = PAMI_N_TORUS_NETWORK;
      TRACE_ERR((stderr, "Mapping::network2task_impl({%zu, %zu, %zu, %zu, %zu, %zu}, %d, %d) <<\n", addr->u.n_torus.coords[0], addr->u.n_torus.coords[1], addr->u.n_torus.coords[2], addr->u.n_torus.coords[3], addr->u.n_torus.coords[4], addr->u.n_torus.coords[5], *task, addr->network));
      return PAMI_SUCCESS;
    }


    /////////////////////////////////////////////////////////////////////////
    //
    // PAMI::Interface::Mapping::Node interface implementation
    //
    /////////////////////////////////////////////////////////////////////////

    /// \see PAMI::Interface::Mapping::Node::nodeTasks()
    inline pami_result_t nodeTasks_impl (size_t global, size_t & tasks)
    {
      TRACE_ERR((stderr, "Mapping::nodeTasks_impl(%zu) >>\n", global));
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/// \todo #warning implement this!
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
      TRACE_ERR((stderr, "Mapping::nodeTasks_impl(%zu) <<\n", global));
      return PAMI_UNIMPL;
    };

    /// \see PAMI::Interface::Mapping::Node::nodePeers()
    inline pami_result_t nodePeers_impl (size_t & peers)
    {
      peers = _mapcache.local_size;
      return PAMI_SUCCESS;
    };

    /// \see PAMI::Interface::Mapping::Node::isLocal()
    inline bool isLocal_impl (size_t task)
    {
      return _mapcache.torus.task2coords[task].mapped.reserved;
    }

    /// \see PAMI::Interface::Mapping::Node::isPeer()
    inline bool isPeer_impl (size_t task1, size_t task2)
    {
      uint32_t coord1 = _mapcache.torus.task2coords[task1].raw;
      uint32_t coord2 = _mapcache.torus.task2coords[task2].raw;

      return ((coord1 & 0x9f7cf3c0) == (coord2 & 0x9f7cf3c0));
      // The above bit masks take the following into account:
      // - Ignore the reserved bit
      // - Ignore the high order bit of ABCD - it is used to store the fifo pin value
      // - Ignore the next highest bit of D - it is used to store the rget pacing flag
      // - Ignore the next highest bit of C - it is used to store the routing flag
      // - Ignore the t coord
    }

    /// \see PAMI::Interface::Mapping::Node::nodeAddr()
    inline void nodeAddr_impl (Interface::Mapping::nodeaddr_t & address)
    {
      TRACE_ERR((stderr, "Mapping::nodeAddr_impl() >>\n"));
      // Can this just be:  address = _nodeaddr; ???
      address.global = _nodeaddr.global;
      address.local  = _nodeaddr.local;
      TRACE_ERR((stderr, "Mapping::nodeAddr_impl(%zu, %zu) <<\n", address.global, address.local));
    };

    /// \see PAMI::Interface::Mapping::Node::task2node()
    inline pami_result_t task2node_impl (size_t task, Interface::Mapping::nodeaddr_t & address)
    {
      TRACE_ERR((stderr, "Mapping::task2node_impl(%zu) >>\n", task));

      uint32_t coords = _mapcache.torus.task2coords[task].raw;
      TRACE_ERR((stderr, "Mapping::coords(%x) >>\n", coords));

      // global coordinate is just the a,b,c,d,e torus coords.
      // We shift the coords by 5 to eliminate the tcoord and to
      // leave room to insert the ecoord in the bottom bit.
      // We also mask off the high bit of ABCD which is used for
      // fifo pinning, and the next highest bit of D which is used
      // for rget pacing, and the next highest bit of C which is
      // used for routing.
      address.global = ((coords >> 5) & 0x00fbe79e) | (coords >> 31);

      // local coordinate is the thread id (t) in the most significant
      // position followed by the core id (p) in the least significant
      // position.
      address.local = (coords & 0x0000003f);

      TRACE_ERR((stderr, "Mapping::task2node_impl(%zu, {%zu, %zu}) <<\n", task, address.global, address.local));
      return PAMI_SUCCESS;
    };

    ///
    /// \brief Get torus link information
    /// \see PAMI::Interface::Mapping::Torus::torusInformation()
    ///
    inline void torusInformation_impl(pami_coord_t &ll, pami_coord_t &ur, unsigned char info[])
    {
      size_t sizes[] = {_pers.aSize(),
                        _pers.bSize(),
                        _pers.cSize(),
                        _pers.dSize(),
                        _pers.eSize()
        };
      for(int i = 0;i < 5; ++i)
      {
       if(_torusInfo[i] && 
          (
           ((size_t)abs(ur.u.n_torus.coords[i] - ll.u.n_torus.coords[i])+1) == sizes[i]
           )
          )
         info[i] = true;
       else 
         info[i] = false;
      }
    }

    inline size_t torusSize_impl (size_t i) 
    {
      PAMI_assert (i < 6);
      size_t sizes[6];
      sizes[0] = _pers.aSize();
      sizes[1] = _pers.bSize();
      sizes[2] = _pers.cSize();
      sizes[3] = _pers.dSize();
      sizes[4] = _pers.eSize();
      sizes[5] = _pers.tSize();
      
      return sizes[i];
    }

    /// \see PAMI::Interface::Mapping::Node::node2task()
    inline pami_result_t node2task_impl (Interface::Mapping::nodeaddr_t address, size_t & task)
    {
      TRACE_ERR((stderr, "Mapping::node2task_impl({%zu, %zu}, ...) >>\n", address.global, address.local));

      size_t aSize = _pers.aSize();
      size_t bSize = _pers.bSize();
      size_t cSize = _pers.cSize();
      size_t dSize = _pers.dSize();
      size_t eSize = _pers.eSize();
      size_t tSize = _pers.tSize();

      // global coordinate is just the a,b,c,d,e torus coords.
      size_t aCoord =  address.global >> 19;
      size_t bCoord = (address.global >> 13) & 0x0000003f;
      size_t cCoord = (address.global >>  7) & 0x0000003f;
      size_t dCoord = (address.global >>  1) & 0x0000003f;
      size_t eCoord =  address.global        & 0x00000001;

      // local coordinate is the thread id (t) in the most significant
      // position followed by the core id (p) in the least significant
      // position.
      size_t tCoord = address.local;

      // Verify that the bgq address is valid.
      if (unlikely((aCoord >= aSize) ||
                   (bCoord >= bSize) ||
                   (cCoord >= cSize) ||
                   (dCoord >= dSize) ||
                   (eCoord >= eSize) ||
                   (tCoord >= tSize)))
      {
        return PAMI_INVAL;
      }

      // Estimate the task id based on the bgq coordinates.
      size_t hash = ESTIMATED_TASK(aCoord,bCoord,cCoord,dCoord,eCoord,tCoord,
                                   aSize,bSize,cSize,dSize,eSize,tSize);
      // Verify that the estimated task is mapped.
      if (unlikely(_mapcache.torus.coords2task[hash] == (unsigned)-1))
      {
        return PAMI_ERROR;
      }

      task = _mapcache.torus.coords2task[hash];

      TRACE_ERR((stderr, "Mapping::node2task_impl({%zu, %zu}, %zu) <<\n", address.global, address.local, task));
      return PAMI_SUCCESS;
    };

    inline pami_result_t node2peer_impl (PAMI::Interface::Mapping::nodeaddr_t & address, size_t & peer)
    {
      TRACE_ERR((stderr, "Mapping::node2peer_impl({%zu, %zu}, ...) >>\n", address.global, address.local));

      size_t tSize = _pers.tSize();
      //size_t peerSize = tSize * pSize;

      size_t tCoord = address.local;

      // Verify that the local node address is valid.
      if (unlikely(tCoord >= tSize))
      {
        return PAMI_INVAL;
      }

      // Estimate the task id based on the bgq coordinates.
      size_t hash = ESTIMATED_TASK(0,0,0,0,0,tCoord,
                                   1,1,1,1,1,tSize);

      // Verify that the address hash is valid.
      if (unlikely(_mapcache.node.local2peer[hash] == (unsigned) - 1))
      {
        return PAMI_ERROR;
      }

      peer = _mapcache.node.local2peer[hash];

      TRACE_ERR((stderr, "Mapping::node2peer_impl({%zu, %zu}, %zu) <<\n", address.global, address.local, peer));
      return PAMI_SUCCESS;
    };

    inline size_t a ()
    {
      return _a;
    }
    inline size_t b ()
    {
      return _b;
    }
    inline size_t c ()
    {
      return _c;
    }
    inline size_t d ()
    {
      return _d;
    }
    inline size_t e ()
    {
      return _e;
    }
    inline size_t t ()
    {
      return _t;
    }
    inline size_t tSize ()
    {
      return _pers.tSize();
    }

    // \brief Get Lowest T Coordinate on This Node
    inline size_t lowestT ()
    {
      return _mapcache.lowestTCoordOnMyNode;
    }

    // \brief Return Whether We Are The Lowest T Coordinate on This Node
    inline bool isLowestT ()
    {
      if ( _mapcache.lowestTCoordOnMyNode == _t )
        return true;
      else
        return false;
    }

    // \brief Get Number of Active Nodes
    inline size_t numActiveNodes ()
    {
      return _mapcache.numActiveNodesGlobal;
    }

    const size_t * getPeer2TaskArray ()
    {
      return _mapcache.node.peer2task;
    };

  };  // class Mapping
};  // namespace PAMI

pami_result_t PAMI::Mapping::init(bgq_mapcache_t &mapcache,
                                PAMI::BgqJobPersonality &personality)
{
  _mapcache.torus.task2coords = mapcache.torus.task2coords;
  _mapcache.torus.coords2task = mapcache.torus.coords2task;
  _mapcache.node.local2peer   = mapcache.node.local2peer;
  _mapcache.node.peer2task    = mapcache.node.peer2task;
  _mapcache.size              = mapcache.size;
  _mapcache.local_size        = mapcache.local_size;
  _mapcache.lowestTCoordOnMyNode   = mapcache.lowestTCoordOnMyNode;
  _mapcache.numActiveNodesGlobal   = mapcache.numActiveNodesGlobal;

  size_t
    //asize =_pers.aSize(),// aSize isn't currently used in the calculation so we get warnings
    bsize =_pers.bSize(),
    csize =_pers.cSize(),
    dsize =_pers.dSize(),
    esize =_pers.eSize(),
    tsize =_pers.tSize();

  TRACE_ERR((stderr,"Mapping() size a/b/c/d/e/t = %zu/%zu/%zu/%zu/%zu/%zu\n", _pers.aSize(), bsize, csize, dsize, esize, tsize));

  size_t hash = ESTIMATED_TASK(_a,_b,_c,_d,_e,_t, _pers.aSize(),bsize,csize,dsize,esize,tsize); // asize isn't used but just in case...

  _task = _mapcache.torus.coords2task[hash];

  TRACE_ERR((stderr,"Mapping::init() task %zu, estimated task %zu, size %zu, peers %zu\n", _task, hash, _mapcache.size, _mapcache.local_size));

  return PAMI_SUCCESS;
}
#undef TRACE_ERR
#endif // __components_mapping_bgq_bgqmapping_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
