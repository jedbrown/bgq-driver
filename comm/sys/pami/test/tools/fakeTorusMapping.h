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
 * \file test/tools/fakeTorusMapping.h
 * \brief ???
 */

#ifndef __test_tools_fakeTorusMapping_h__
#define __test_tools_fakeTorusMapping_h__

/*
 * Fake TorusMapping so we don't need a real blue-gene system.
 * This must be the first thing included (before any other CCMI
 * includes, at least).
 */

// Prevent the real TorusMapping.h from being included
#define __collective_engine_torus_mapping__

// Prevent the real Mapping.h, too.
#define __collective_engine_mapping__

/*
 * The constructor is called with complete "rectangle" information,
 * including the node coordinates. This is also used for Binomial
 * by only having X-dimensions (Y,Z,T size = 1). The "ranks[]" then
 * becomes a simple list of the coordinates along X.
 */

#include <stdlib.h>

#include "algorithms/ccmi.h"

#ifndef   CCMI_TORUS_NDIMS
#define   CCMI_TORUS_NDIMS 4
#endif

#define CCMI_X_DIM 0
#define CCMI_Y_DIM 1
#define CCMI_Z_DIM 2
#define CCMI_T_DIM 3

#define CCMI_COPY_COORDS(a,b)               \
{                                           \
  int _i;                                   \
  for (_i = 0; _i < CCMI_TORUS_NDIMS; _i++) \
    a[_i] = b[_i];                          \
}

namespace CCMI
{
  class Mapping
  {
  protected:
    unsigned _rank;
  public:
    Mapping(void)
    {
      _rank = 0;
    }
    void setRank(unsigned rank)
    {
      _rank = rank;
    }
    unsigned rank()
    {
      return _rank;
    }
  };
  class TorusMapping : public Mapping
  {

  protected:
    unsigned _coords[CCMI_TORUS_NDIMS];
    unsigned _mins[CCMI_TORUS_NDIMS];
    unsigned _maxs[CCMI_TORUS_NDIMS];
    unsigned _sizes[CCMI_TORUS_NDIMS];
    unsigned _nRanks;
  public:
    TorusMapping(void)
    {
      _coords[CCMI_X_DIM] = _coords[CCMI_Y_DIM] = _coords[CCMI_Z_DIM] = _coords[CCMI_T_DIM] =
           _mins[CCMI_X_DIM] = _mins[CCMI_Y_DIM] = _mins[CCMI_Z_DIM] = _mins[CCMI_T_DIM] =
           _sizes[CCMI_X_DIM] = _sizes[CCMI_Y_DIM] = _sizes[CCMI_Z_DIM] = _sizes[CCMI_T_DIM] = 0;
    }
    TorusMapping(unsigned xs, unsigned ys, unsigned zs, unsigned ts,
                 unsigned x0, unsigned y0, unsigned z0, unsigned t0,
                 unsigned x, unsigned y, unsigned z, unsigned t)
    {

      // Always computes ranks relative to (0,0,0,0)
      _mins[CCMI_X_DIM] = x0;
      _mins[CCMI_Y_DIM] = y0;
      _mins[CCMI_Z_DIM] = z0;
      _mins[CCMI_T_DIM] = t0;
      _maxs[CCMI_X_DIM] = x0 + xs;
      _maxs[CCMI_Y_DIM] = y0 + ys;
      _maxs[CCMI_Z_DIM] = z0 + zs;
      _maxs[CCMI_T_DIM] = t0 + ts;
      _sizes[CCMI_X_DIM] = xs;
      _sizes[CCMI_Y_DIM] = ys;
      _sizes[CCMI_Z_DIM] = zs;
      _sizes[CCMI_T_DIM] = ts;
      _nRanks = _sizes[CCMI_X_DIM] * _sizes[CCMI_Y_DIM] * _sizes[CCMI_Z_DIM] * _sizes[CCMI_T_DIM];
      reset(x, y, z, t);
    }
    inline void * operator new(unsigned size)
    {
      return malloc(size);
    }
    inline void * operator new(unsigned size, void *addr)
    {
      return addr;
    }

    unsigned x()
    {
      return _coords[CCMI_X_DIM];
    }
    unsigned y()
    {
      return _coords[CCMI_Y_DIM];
    }
    unsigned z()
    {
      return _coords[CCMI_Z_DIM];
    }
    unsigned t()
    {
      return _coords[CCMI_T_DIM];
    }
    unsigned xSize()
    {
      return _sizes[CCMI_X_DIM];
    }
    unsigned ySize()
    {
      return _sizes[CCMI_Y_DIM];
    }
    unsigned zSize()
    {
      return _sizes[CCMI_Z_DIM];
    }
    unsigned tSize()
    {
      return _sizes[CCMI_T_DIM];
    }
    unsigned nRanks()
    {
      return _nRanks;
    }
    unsigned size()
    {
      return _nRanks;
    }

    pami_result_t Torus2Rank(unsigned *coords, unsigned *rank)
    {
      *rank = ((coords[CCMI_Z_DIM] * _maxs[CCMI_Y_DIM] + coords[CCMI_Y_DIM]) * _maxs[CCMI_X_DIM] + coords[CCMI_X_DIM]) * _maxs[CCMI_T_DIM] + coords[CCMI_T_DIM];
      return PAMI_SUCCESS;
    }

    const void Rank2Torus(unsigned *coords, unsigned rank)
    {
      coords[CCMI_T_DIM] = rank % _maxs[CCMI_T_DIM]; rank = rank / _maxs[CCMI_T_DIM];
      coords[CCMI_X_DIM] = rank % _maxs[CCMI_X_DIM]; rank = rank / _maxs[CCMI_X_DIM];
      coords[CCMI_Y_DIM] = rank % _maxs[CCMI_Y_DIM]; rank = rank / _maxs[CCMI_Y_DIM];
      coords[CCMI_Z_DIM] = rank % _maxs[CCMI_Z_DIM]; rank = rank / _maxs[CCMI_Z_DIM];
    }
    unsigned GetCoord(unsigned dim)
    {
      return _coords[dim];
    }
    unsigned GetDimLength(unsigned dim)
    {
      return _sizes[dim];
    }

    unsigned* Coords()
    {
      return &(_coords[0]);
    }

    unsigned* DimsLengths()
    {
      return &(_sizes[0]);
    }

    void reset(unsigned x, unsigned y, unsigned z, unsigned t)
    {
      unsigned r;
      _coords[CCMI_X_DIM] = x;
      _coords[CCMI_Y_DIM] = y;
      _coords[CCMI_Z_DIM] = z;
      _coords[CCMI_T_DIM] = t;
      (void) Torus2Rank(_coords, &r);
      setRank(r);
    }
    void reset(unsigned r)
    {
      setRank(r);
      Rank2Torus(_coords, r);
    }
  };
};

#endif
