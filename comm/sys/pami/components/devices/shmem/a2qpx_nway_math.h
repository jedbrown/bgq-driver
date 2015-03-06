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
 * \file components/devices/shmem/a2qpx_nway_math.h
 * \brief ???
 */
#ifndef __components_devices_shmem_a2qpx_nway_math_h__
#define __components_devices_shmem_a2qpx_nway_math_h__
#include "math/a2qpx/qpx_2way_sum.h"
#include "math/a2qpx/qpx_4way_sum.h"
#include "math/a2qpx/qpx_8way_sum.h"
#include "math/a2qpx/qpx_16way_sum.h"
#include "math/a2qpx/qpx_2way_max.h"
#include "math/a2qpx/qpx_4way_max.h"
#include "math/a2qpx/qpx_8way_max.h"
#include "math/a2qpx/qpx_16way_max.h"
#include "math/a2qpx/qpx_2way_min.h"
#include "math/a2qpx/qpx_4way_min.h"
#include "math/a2qpx/qpx_8way_min.h"
#include "math/a2qpx/qpx_16way_min.h"

inline unsigned quad_double_math_2way(double* dst, double* src0, double *src1, unsigned num_dbls, pami_op opcode)
{
  switch (opcode)
  {
    case PAMI_SUM:
   quad_double_sum_2way( dst,  src0,  src1, num_dbls );
    break;

    case PAMI_MAX:
  quad_double_max_2way( dst,  src0,  src1, num_dbls );
      break;

    case PAMI_MIN:
  quad_double_min_2way( dst,  src0,  src1, num_dbls );
      break;

    case PAMI_COPY:
    case PAMI_NOOP:
    case PAMI_PROD:
    case PAMI_LAND:
    case PAMI_LOR:
    case PAMI_LXOR:
    case PAMI_BAND:
    case PAMI_BOR:
    case PAMI_BXOR:
    {
      double* srcs[16] = {src0,src1};
      coremath func = MATH_OP_FUNCS(PAMI_DOUBLE, opcode, 2);
      func(dst, (void**)srcs,  2, num_dbls);
      break;
    }
    case PAMI_MAXLOC:
    case PAMI_MINLOC:
    case PAMI_OP_COUNT:
    {
      PAMI_abortf("not supported\n");
      break;
    }
  } 
  return num_dbls;
}

inline unsigned quad_double_math_4way(double* dst, double* src0, double *src1, double* src2, double* src3, unsigned num_dbls, pami_op opcode)
{
  switch (opcode )
  {
   case PAMI_SUM:
   quad_double_sum_4way( dst,  src0,  src1, src2,  src3, num_dbls );
      break;

    case PAMI_MAX:
   quad_double_max_4way( dst,  src0,  src1, src2,  src3, num_dbls );
      break;

    case PAMI_MIN:
   quad_double_min_4way( dst,  src0,  src1, src2,  src3, num_dbls );
      break;

    case PAMI_COPY:
    case PAMI_NOOP:
    case PAMI_PROD:
    case PAMI_LAND:
    case PAMI_LOR:
    case PAMI_LXOR:
    case PAMI_BAND:
    case PAMI_BOR:
    case PAMI_BXOR:
    {
      double* srcs[4] = {src0,src1,src2,src3};
      coremath func = MATH_OP_FUNCS(PAMI_DOUBLE, opcode, 4);
      func(dst, (void**)srcs,  4, num_dbls);
      break;
    }
    case PAMI_MAXLOC:
    case PAMI_MINLOC:
    case PAMI_OP_COUNT:
    {
      PAMI_abortf("not supported\n");
      break;
    }
  } 
  return num_dbls;

}

inline unsigned quad_double_math_8way(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, unsigned num_dbls, pami_op opcode)
{
  switch (opcode) 
  {
    case PAMI_SUM:
   quad_double_sum_8way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7, num_dbls );
      break;

    case PAMI_MAX:
   quad_double_max_8way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7,  num_dbls );
      break;

    case PAMI_MIN:
   quad_double_min_8way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7,  num_dbls );
      break;

    case PAMI_COPY:
    case PAMI_NOOP:
    case PAMI_PROD:
    case PAMI_LAND:
    case PAMI_LOR:
    case PAMI_LXOR:
    case PAMI_BAND:
    case PAMI_BOR:
    case PAMI_BXOR:
    {
      double* srcs[8] = {src0,src1,src2,src3,src4,src5,src6,src7};
      coremath func = MATH_OP_FUNCS(PAMI_DOUBLE, opcode, 8);
      func(dst, (void**)srcs,  8, num_dbls);
      break;
    }
    case PAMI_MAXLOC:
    case PAMI_MINLOC:
    case PAMI_OP_COUNT:
    {
      PAMI_abortf("not supported\n");
      break;
    }
  } 
  return num_dbls;

}

inline unsigned quad_double_math_16way(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, double* src8, double *src9, double* src10, double* src11,
    double* src12, double* src13, double* src14, double* src15, unsigned num_dbls, pami_op opcode)
{

  switch (opcode) 
  {
    case PAMI_SUM:
   quad_double_sum_16way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7, src8, src9, src10, src11, src12, src13, src14, src15,  num_dbls );
      break;

    case PAMI_MAX:
   quad_double_max_16way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7, src8, src9, src10, src11, src12, src13, src14, src15,  num_dbls );
      break;

    case PAMI_MIN:
   quad_double_min_16way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7, src8, src9, src10, src11, src12, src13, src14, src15,  num_dbls );
      break;

    case PAMI_COPY:
    case PAMI_NOOP:
    case PAMI_PROD:
    case PAMI_LAND:
    case PAMI_LOR:
    case PAMI_LXOR:
    case PAMI_BAND:
    case PAMI_BOR:
    case PAMI_BXOR:
    {
      double* srcs[16] = {src0,src1,src2,src3,src4,src5,src6,src7,src8,src9,src10,src11,src12,src13,src14,src15};
      coremath func = MATH_OP_FUNCS(PAMI_DOUBLE, opcode, 16);
      func(dst, (void**)srcs,  16, num_dbls);
      break;
    }
    case PAMI_MAXLOC:
    case PAMI_MINLOC:
    case PAMI_OP_COUNT:
    {
      PAMI_abortf("not supported\n");
      break;
    }
  } 
  return num_dbls;

}

#endif
