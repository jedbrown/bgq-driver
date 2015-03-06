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
 * \file math/pami_math.cc
 * \brief Tables used by PAMI
 *
 * Since C++ does not yet support designated initializers, this needs
 * to be C code.
 */

#include <pami.h>
#include "math/math_coremath.h"

/**
 * \brief Translates a PAMI_Dt into its size
 *        This table must match the enum definition for pami_dt
 */
int pami_dt_sizes[] = {
  sizeof(uint8_t),

  sizeof(char),
  sizeof(short),
  sizeof(int),
  sizeof(long),
  sizeof(long long),

  sizeof(unsigned char),
  sizeof(unsigned short),
  sizeof(unsigned int),
  sizeof(unsigned long),
  sizeof(unsigned long long),

  sizeof(float),
  sizeof(double),
  sizeof(long double),

  1,                    // 1-byte logical
  2,                    // 2-byte logical
  4,                    // 4-byte logical
  8,                    // 8-byte logical

  sizeof(fp32_fp32_t),  // single complex
  sizeof(fp64_fp64_t),  // double complex

  sizeof(int32_int32_t), // 2int
  sizeof(fp32_fp32_t),   // 2float
  sizeof(fp64_fp64_t),   // 2double
  sizeof(int16_int32_t), // short, int
  sizeof(fp32_int32_t),  // float, int
  sizeof(fp64_int32_t),  // double, int
  sizeof(uint32_int32_t),// long, int
  sizeof(fp128_int32_t), // long double, int
  0,
};

/**
 * \brief Translates a PAMI_Dt into its shift factor (bytes)
 *
 * This depends on pami_dt_sizes[] all being powers of 2.
 * Should probably compute this, and assert, at runtime rather
 * than hard-code it on presumed datatypes.
 */
int pami_dt_shift[] = {
 0,
 0, /* sizeof(char) = 1 */
 1, /* sizeof(short) = 2 */
 2, /* sizeof(int) = 4 */
#if defined(__64BIT__)
 3, /* sizeof(long) = 8 */
#else
 2, /* sizeof(long) = 4 */
#endif
 3, /* sizeof(long long) = 8 */

 0, /* sizeof(unsigned char) = 1 */
 1, /* sizeof(unsigned short) = 2 */
 2, /* sizeof(unsigned int) = 4 */
#if defined(__64BIT__)
 3, /* sizeof(unsigned long) = 8 */
#else
 2, /* sizeof(unsigned long) = 4 */
#endif
 3, /* sizeof(unsigned long long) = 8 */

 2, /* sizeof(float) = 4 */
 3, /* sizeof(double) = 8 */
 4, /* sizeof(long double) = 16 */

 0, /* 1 byte  */
 1, /* 2 bytes */
 2, /* 4 bytes */
 3, /* 8 bytes */

 3, /* sizeof(fp32_fp32_t) = 8 */
 4, /* sizeof(fp64_fp64_t) = 16 */

 3, /* sizeof(int32_int32_t) = 8 */
 3, /* sizeof(fp32_fp32_t) = 8 */
 4, /* sizeof(fp64_fp64_t) = 16 */
 3, /* sizeof(int16_int32_t) = 8 */
 3, /* sizeof(fp32_int32_t) = 8 */
 4, /* sizeof(fp64_int32_t) = 16 */
 5, /* sizeof(fp128_int32_t) = 32 */
 -1,
};

/**
 * \brief Select a math function for operation, datatype, and number of inputs
 * This table sohuld not be accessed directly. The inline/macro
 * MATH_OP_FUNCS(dt,op,n) should be used instead. That returns
 * the proper function (pointer) for the given combination of
 * datatype "dt", operand "op", and number of inputs "n", taking
 * into account conbinations that are not optimized.
 */
#ifndef MATH_NO_OPTIMATH
#define OPTIMATH_NSRC(dt,op,n,f) table[op][dt][n-1] = (void*)f;
#else /* MATH_NO_OPTIMATH */
#define OPTIMATH_NSRC(dt,op,n,f)
#endif /* MATH_NO_OPTIMATH */
void *math_op_funcs[PAMI_OP_COUNT][PAMI_DT_COUNT][MATH_MAX_NSRC];

class MathOpsInit
{
public:
  MathOpsInit(void *table[PAMI_OP_COUNT][PAMI_DT_COUNT][MATH_MAX_NSRC])
  {
#if defined(__64BIT__)
    COMPILE_TIME_ASSERT(sizeof(unsigned long) == 8);
#else
    COMPILE_TIME_ASSERT(sizeof(unsigned long) == 4);
#endif
    memset(table, 0, sizeof(table));
    OPTIMIZED_int8_max;
    OPTIMIZED_uint8_max;
    OPTIMIZED_int16_max;
    OPTIMIZED_uint16_max;
    OPTIMIZED_int32_max;
    OPTIMIZED_uint32_max;
    OPTIMIZED_int64_max;
    OPTIMIZED_uint64_max;
    OPTIMIZED_fp32_max;
    OPTIMIZED_fp64_max;
    OPTIMIZED_fp128_max;

    OPTIMIZED_int8_min;
    OPTIMIZED_uint8_min;
    OPTIMIZED_int16_min;
    OPTIMIZED_uint16_min;
    OPTIMIZED_int32_min;
    OPTIMIZED_uint32_min;
    OPTIMIZED_int64_min;
    OPTIMIZED_uint64_min;
    OPTIMIZED_fp32_min;
    OPTIMIZED_fp64_min;
    OPTIMIZED_fp128_min;

    OPTIMIZED_int8_sum;
    OPTIMIZED_uint8_sum;
    OPTIMIZED_int16_sum;
    OPTIMIZED_uint16_sum;
    OPTIMIZED_int32_sum;
    OPTIMIZED_uint32_sum;
    OPTIMIZED_int64_sum;
    OPTIMIZED_uint64_sum;
    OPTIMIZED_fp32_sum;
    OPTIMIZED_fp64_sum;
    OPTIMIZED_fp128_sum;

    OPTIMIZED_int8_prod;
    OPTIMIZED_uint8_prod;
    OPTIMIZED_int16_prod;
    OPTIMIZED_uint16_prod;
    OPTIMIZED_int32_prod;
    OPTIMIZED_uint32_prod;
    OPTIMIZED_int64_prod;
    OPTIMIZED_uint64_prod;
    OPTIMIZED_fp32_prod;
    OPTIMIZED_fp64_prod;
    OPTIMIZED_fp128_prod;

    OPTIMIZED_int8_land;
    OPTIMIZED_uint8_land;
    OPTIMIZED_int16_land;
    OPTIMIZED_uint16_land;
    OPTIMIZED_int32_land;
    OPTIMIZED_uint32_land;
    OPTIMIZED_int64_land;
    OPTIMIZED_uint64_land;

    OPTIMIZED_int8_lor;
    OPTIMIZED_uint8_lor;
    OPTIMIZED_int16_lor;
    OPTIMIZED_uint16_lor;
    OPTIMIZED_int32_lor;
    OPTIMIZED_uint32_lor;
    OPTIMIZED_int64_lor;
    OPTIMIZED_uint64_lor;

    OPTIMIZED_int8_lxor;
    OPTIMIZED_uint8_lxor;
    OPTIMIZED_int16_lxor;
    OPTIMIZED_uint16_lxor;
    OPTIMIZED_int32_lxor;
    OPTIMIZED_uint32_lxor;
    OPTIMIZED_int64_lxor;
    OPTIMIZED_uint64_lxor;

    OPTIMIZED_int8_band;
    OPTIMIZED_uint8_band;
    OPTIMIZED_int16_band;
    OPTIMIZED_uint16_band;
    OPTIMIZED_int32_band;
    OPTIMIZED_uint32_band;
    OPTIMIZED_int64_band;
    OPTIMIZED_uint64_band;

    OPTIMIZED_int8_bor;
    OPTIMIZED_uint8_bor;
    OPTIMIZED_int16_bor;
    OPTIMIZED_uint16_bor;
    OPTIMIZED_int32_bor;
    OPTIMIZED_uint32_bor;
    OPTIMIZED_int64_bor;
    OPTIMIZED_uint64_bor;

    OPTIMIZED_int8_bxor;
    OPTIMIZED_uint8_bxor;
    OPTIMIZED_int16_bxor;
    OPTIMIZED_uint16_bxor;
    OPTIMIZED_int32_bxor;
    OPTIMIZED_uint32_bxor;
    OPTIMIZED_int64_bxor;
    OPTIMIZED_uint64_bxor;

    OPTIMIZED_int64_int32_maxloc;
    OPTIMIZED_int32_int32_maxloc;
    OPTIMIZED_int16_int32_maxloc;
    OPTIMIZED_fp32_int32_maxloc;
    OPTIMIZED_fp64_int32_maxloc;
    OPTIMIZED_fp32_fp32_maxloc;
    OPTIMIZED_fp64_fp64_maxloc;

    OPTIMIZED_int64_int32_minloc;
    OPTIMIZED_int32_int32_minloc;
    OPTIMIZED_int16_int32_minloc;
    OPTIMIZED_fp32_int32_minloc;
    OPTIMIZED_fp64_int32_minloc;
    OPTIMIZED_fp32_fp32_minloc;
    OPTIMIZED_fp64_fp64_minloc;

    table[PAMI_MAX][PAMI_SIGNED_CHAR][0] =	(void*)_pami_core_int8_max;
    table[PAMI_MAX][PAMI_UNSIGNED_CHAR][0] =	(void*)_pami_core_uint8_max;
    table[PAMI_MAX][PAMI_SIGNED_SHORT][0] =	(void*)_pami_core_int16_max;
    table[PAMI_MAX][PAMI_UNSIGNED_SHORT][0] =	(void*)_pami_core_uint16_max;
    table[PAMI_MAX][PAMI_SIGNED_INT][0] =	(void*)_pami_core_int32_max;
    table[PAMI_MAX][PAMI_UNSIGNED_INT][0] =	(void*)_pami_core_uint32_max;
#if defined(__64BIT__)
    table[PAMI_MAX][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int64_max;
    table[PAMI_MAX][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint64_max;
#else
    table[PAMI_MAX][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int32_max;
    table[PAMI_MAX][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint32_max;
#endif
    table[PAMI_MAX][PAMI_SIGNED_LONG_LONG][0] =	(void*)_pami_core_int64_max;
    table[PAMI_MAX][PAMI_UNSIGNED_LONG_LONG][0] =(void*)_pami_core_uint64_max;
    table[PAMI_MAX][PAMI_FLOAT][0] =		(void*)_pami_core_fp32_max;
    table[PAMI_MAX][PAMI_DOUBLE][0] =		(void*)_pami_core_fp64_max;
    table[PAMI_MAX][PAMI_LONG_DOUBLE][0] =	(void*)_pami_core_fp128_max;

    table[PAMI_MIN][PAMI_SIGNED_CHAR][0] =	(void*)_pami_core_int8_min;
    table[PAMI_MIN][PAMI_UNSIGNED_CHAR][0] =	(void*)_pami_core_uint8_min;
    table[PAMI_MIN][PAMI_SIGNED_SHORT][0] =	(void*)_pami_core_int16_min;
    table[PAMI_MIN][PAMI_UNSIGNED_SHORT][0] =	(void*)_pami_core_uint16_min;
    table[PAMI_MIN][PAMI_SIGNED_INT][0] =	(void*)_pami_core_int32_min;
    table[PAMI_MIN][PAMI_UNSIGNED_INT][0] =	(void*)_pami_core_uint32_min;
#if defined(__64BIT__)
    table[PAMI_MIN][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int64_min;
    table[PAMI_MIN][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint64_min;
#else
    table[PAMI_MIN][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int32_min;
    table[PAMI_MIN][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint32_min;
#endif
    table[PAMI_MIN][PAMI_SIGNED_LONG_LONG][0] =	(void*)_pami_core_int64_min;
    table[PAMI_MIN][PAMI_UNSIGNED_LONG_LONG][0] =(void*)_pami_core_uint64_min;
    table[PAMI_MIN][PAMI_FLOAT][0] =		(void*)_pami_core_fp32_min;
    table[PAMI_MIN][PAMI_DOUBLE][0] =		(void*)_pami_core_fp64_min;
    table[PAMI_MIN][PAMI_LONG_DOUBLE][0] =	(void*)_pami_core_fp128_min;

    table[PAMI_SUM][PAMI_SIGNED_CHAR][0] =	(void*)_pami_core_int8_sum;
    table[PAMI_SUM][PAMI_UNSIGNED_CHAR][0] =	(void*)_pami_core_uint8_sum;
    table[PAMI_SUM][PAMI_SIGNED_SHORT][0] =	(void*)_pami_core_int16_sum;
    table[PAMI_SUM][PAMI_UNSIGNED_SHORT][0] =	(void*)_pami_core_uint16_sum;
    table[PAMI_SUM][PAMI_SIGNED_INT][0] =	(void*)_pami_core_int32_sum;
    table[PAMI_SUM][PAMI_UNSIGNED_INT][0] =	(void*)_pami_core_uint32_sum;
#if defined(__64BIT__)
    table[PAMI_SUM][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int64_sum;
    table[PAMI_SUM][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint64_sum;
#else
    table[PAMI_SUM][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int32_sum;
    table[PAMI_SUM][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint32_sum;
#endif
    table[PAMI_SUM][PAMI_SIGNED_LONG_LONG][0] =	(void*)_pami_core_int64_sum;
    table[PAMI_SUM][PAMI_UNSIGNED_LONG_LONG][0] =(void*)_pami_core_uint64_sum;
    table[PAMI_SUM][PAMI_FLOAT][0] =		(void*)_pami_core_fp32_sum;
    table[PAMI_SUM][PAMI_DOUBLE][0] =		(void*)_pami_core_fp64_sum;
    table[PAMI_SUM][PAMI_LONG_DOUBLE][0] =	(void*)_pami_core_fp128_sum;

    table[PAMI_PROD][PAMI_SIGNED_CHAR][0] =	(void*)_pami_core_int8_prod;
    table[PAMI_PROD][PAMI_UNSIGNED_CHAR][0] =	(void*)_pami_core_uint8_prod;
    table[PAMI_PROD][PAMI_SIGNED_SHORT][0] =	(void*)_pami_core_int16_prod;
    table[PAMI_PROD][PAMI_UNSIGNED_SHORT][0] =	(void*)_pami_core_uint16_prod;
    table[PAMI_PROD][PAMI_SIGNED_INT][0] =	(void*)_pami_core_int32_prod;
    table[PAMI_PROD][PAMI_UNSIGNED_INT][0] =	(void*)_pami_core_uint32_prod;
#if defined(__64BIT__)
    table[PAMI_PROD][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int64_prod;
    table[PAMI_PROD][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint64_prod;
#else
    table[PAMI_PROD][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int32_prod;
    table[PAMI_PROD][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint32_prod;
#endif
    table[PAMI_PROD][PAMI_SIGNED_LONG_LONG][0] =	(void*)_pami_core_int64_prod;
    table[PAMI_PROD][PAMI_UNSIGNED_LONG_LONG][0] =(void*)_pami_core_uint64_prod;
    table[PAMI_PROD][PAMI_FLOAT][0] =		(void*)_pami_core_fp32_prod;
    table[PAMI_PROD][PAMI_DOUBLE][0] =		(void*)_pami_core_fp64_prod;
    table[PAMI_PROD][PAMI_LONG_DOUBLE][0] =	(void*)_pami_core_fp128_prod;
    table[PAMI_PROD][PAMI_SINGLE_COMPLEX][0] =	(void*)_pami_core_fp32_fp32_cplx_prod;
    table[PAMI_PROD][PAMI_DOUBLE_COMPLEX][0] =	(void*) _pami_core_fp64_fp64_cplx_prod;

    table[PAMI_LAND][PAMI_SIGNED_CHAR][0] =	(void*)_pami_core_int8_land;
    table[PAMI_LAND][PAMI_UNSIGNED_CHAR][0] =	(void*)_pami_core_uint8_land;
    table[PAMI_LAND][PAMI_SIGNED_SHORT][0] =	(void*)_pami_core_int16_land;
    table[PAMI_LAND][PAMI_UNSIGNED_SHORT][0] =	(void*)_pami_core_uint16_land;
    table[PAMI_LAND][PAMI_SIGNED_INT][0] =	(void*)_pami_core_int32_land;
    table[PAMI_LAND][PAMI_UNSIGNED_INT][0] =	(void*)_pami_core_uint32_land;
#if defined(__64BIT__)
    table[PAMI_LAND][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int64_land;
    table[PAMI_LAND][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint64_land;
#else
    table[PAMI_LAND][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int32_land;
    table[PAMI_LAND][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint32_land;
#endif
    table[PAMI_LAND][PAMI_SIGNED_LONG_LONG][0] =	(void*)_pami_core_int64_land;
    table[PAMI_LAND][PAMI_UNSIGNED_LONG_LONG][0] =(void*)_pami_core_uint64_land;
    table[PAMI_LAND][PAMI_FLOAT][0] =		(void*)_pami_core_fp32_land;
    table[PAMI_LAND][PAMI_DOUBLE][0] =		(void*)_pami_core_fp64_land;

    table[PAMI_LOR][PAMI_SIGNED_CHAR][0] =	(void*)_pami_core_int8_lor;
    table[PAMI_LOR][PAMI_UNSIGNED_CHAR][0] =	(void*)_pami_core_uint8_lor;
    table[PAMI_LOR][PAMI_SIGNED_SHORT][0] =	(void*)_pami_core_int16_lor;
    table[PAMI_LOR][PAMI_UNSIGNED_SHORT][0] =	(void*)_pami_core_uint16_lor;
    table[PAMI_LOR][PAMI_SIGNED_INT][0] =	(void*)_pami_core_int32_lor;
    table[PAMI_LOR][PAMI_UNSIGNED_INT][0] =	(void*)_pami_core_uint32_lor;
#if defined(__64BIT__)
    table[PAMI_LOR][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int64_lor;
    table[PAMI_LOR][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint64_lor;
#else
    table[PAMI_LOR][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int32_lor;
    table[PAMI_LOR][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint32_lor;
#endif
    table[PAMI_LOR][PAMI_SIGNED_LONG_LONG][0] =	(void*)_pami_core_int64_lor;
    table[PAMI_LOR][PAMI_UNSIGNED_LONG_LONG][0] =(void*)_pami_core_uint64_lor;
    table[PAMI_LOR][PAMI_FLOAT][0] =		(void*)_pami_core_fp32_lor;
    table[PAMI_LOR][PAMI_DOUBLE][0] =		(void*)_pami_core_fp64_lor;

    table[PAMI_LXOR][PAMI_SIGNED_CHAR][0] =	(void*)_pami_core_int8_lxor;
    table[PAMI_LXOR][PAMI_UNSIGNED_CHAR][0] =	(void*)_pami_core_uint8_lxor;
    table[PAMI_LXOR][PAMI_SIGNED_SHORT][0] =	(void*)_pami_core_int16_lxor;
    table[PAMI_LXOR][PAMI_UNSIGNED_SHORT][0] =	(void*)_pami_core_uint16_lxor;
    table[PAMI_LXOR][PAMI_SIGNED_INT][0] =	(void*)_pami_core_int32_lxor;
    table[PAMI_LXOR][PAMI_UNSIGNED_INT][0] =	(void*)_pami_core_uint32_lxor;
#if defined(__64BIT__)
    table[PAMI_LXOR][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int64_lxor;
    table[PAMI_LXOR][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint64_lxor;
#else
    table[PAMI_LXOR][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int32_lxor;
    table[PAMI_LXOR][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint32_lxor;
#endif
    table[PAMI_LXOR][PAMI_SIGNED_LONG_LONG][0] =	(void*)_pami_core_int64_lxor;
    table[PAMI_LXOR][PAMI_UNSIGNED_LONG_LONG][0] =(void*)_pami_core_uint64_lxor;
    table[PAMI_LXOR][PAMI_FLOAT][0] =		(void*)_pami_core_fp32_lxor;
    table[PAMI_LXOR][PAMI_DOUBLE][0] =		(void*)_pami_core_fp64_lxor;

    table[PAMI_BAND][PAMI_SIGNED_CHAR][0] =	(void*)_pami_core_int8_band;
    table[PAMI_BAND][PAMI_UNSIGNED_CHAR][0] =	(void*)_pami_core_uint8_band;
    table[PAMI_BAND][PAMI_SIGNED_SHORT][0] =	(void*)_pami_core_int16_band;
    table[PAMI_BAND][PAMI_UNSIGNED_SHORT][0] =	(void*)_pami_core_uint16_band;
    table[PAMI_BAND][PAMI_SIGNED_INT][0] =	(void*)_pami_core_int32_band;
    table[PAMI_BAND][PAMI_UNSIGNED_INT][0] =	(void*)_pami_core_uint32_band;
#if defined(__64BIT__)
    table[PAMI_BAND][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int64_band;
    table[PAMI_BAND][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint64_band;
#else
    table[PAMI_BAND][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int32_band;
    table[PAMI_BAND][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint32_band;
#endif
    table[PAMI_BAND][PAMI_SIGNED_LONG_LONG][0] =	(void*)_pami_core_int64_band;
    table[PAMI_BAND][PAMI_UNSIGNED_LONG_LONG][0] =(void*)_pami_core_uint64_band;
    table[PAMI_BAND][PAMI_FLOAT][0] =		(void*)_pami_core_fp32_band;
    table[PAMI_BAND][PAMI_DOUBLE][0] =		(void*)_pami_core_fp64_band;

    table[PAMI_BOR][PAMI_SIGNED_CHAR][0] =	(void*)_pami_core_int8_bor;
    table[PAMI_BOR][PAMI_UNSIGNED_CHAR][0] =	(void*)_pami_core_uint8_bor;
    table[PAMI_BOR][PAMI_SIGNED_SHORT][0] =	(void*)_pami_core_int16_bor;
    table[PAMI_BOR][PAMI_UNSIGNED_SHORT][0] =	(void*)_pami_core_uint16_bor;
    table[PAMI_BOR][PAMI_SIGNED_INT][0] =	(void*)_pami_core_int32_bor;
    table[PAMI_BOR][PAMI_UNSIGNED_INT][0] =	(void*)_pami_core_uint32_bor;
#if defined(__64BIT__)
    table[PAMI_BOR][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int64_bor;
    table[PAMI_BOR][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint64_bor;
#else
    table[PAMI_BOR][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int32_bor;
    table[PAMI_BOR][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint32_bor;
#endif
    table[PAMI_BOR][PAMI_SIGNED_LONG_LONG][0] =	(void*)_pami_core_int64_bor;
    table[PAMI_BOR][PAMI_UNSIGNED_LONG_LONG][0] =(void*)_pami_core_uint64_bor;
    table[PAMI_BOR][PAMI_FLOAT][0] =		(void*)_pami_core_fp32_bor;
    table[PAMI_BOR][PAMI_DOUBLE][0] =		(void*)_pami_core_fp64_bor;

    table[PAMI_BXOR][PAMI_SIGNED_CHAR][0] =	(void*)_pami_core_int8_bxor;
    table[PAMI_BXOR][PAMI_UNSIGNED_CHAR][0] =	(void*)_pami_core_uint8_bxor;
    table[PAMI_BXOR][PAMI_SIGNED_SHORT][0] =	(void*)_pami_core_int16_bxor;
    table[PAMI_BXOR][PAMI_UNSIGNED_SHORT][0] =	(void*)_pami_core_uint16_bxor;
    table[PAMI_BXOR][PAMI_SIGNED_INT][0] =	(void*)_pami_core_int32_bxor;
    table[PAMI_BXOR][PAMI_UNSIGNED_INT][0] =	(void*)_pami_core_uint32_bxor;
#if defined(__64BIT__)
    table[PAMI_BXOR][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int64_bxor;
    table[PAMI_BXOR][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint64_bxor;
#else
    table[PAMI_BXOR][PAMI_SIGNED_LONG][0] =	(void*)_pami_core_int32_bxor;
    table[PAMI_BXOR][PAMI_UNSIGNED_LONG][0] =(void*)_pami_core_uint32_bxor;
#endif
    table[PAMI_BXOR][PAMI_SIGNED_LONG_LONG][0] =	(void*)_pami_core_int64_bxor;
    table[PAMI_BXOR][PAMI_UNSIGNED_LONG_LONG][0] =(void*)_pami_core_uint64_bxor;
    table[PAMI_BXOR][PAMI_FLOAT][0] =		(void*)_pami_core_fp32_bxor;
    table[PAMI_BXOR][PAMI_DOUBLE][0] =		(void*)_pami_core_fp64_bxor;

    table[PAMI_MAXLOC][PAMI_LOC_2INT][0] =	(void*)_pami_core_int32_int32_maxloc;
    table[PAMI_MAXLOC][PAMI_LOC_SHORT_INT][0] =	(void*)_pami_core_int16_int32_maxloc;
    table[PAMI_MAXLOC][PAMI_LOC_FLOAT_INT][0] =	(void*)_pami_core_fp32_int32_maxloc;
    table[PAMI_MAXLOC][PAMI_LOC_DOUBLE_INT][0] =	(void*)_pami_core_fp64_int32_maxloc;
    table[PAMI_MAXLOC][PAMI_LOC_2FLOAT][0] =	(void*)_pami_core_fp32_fp32_maxloc;
    table[PAMI_MAXLOC][PAMI_LOC_2DOUBLE][0] =	(void*)_pami_core_fp64_fp64_maxloc;
    if (sizeof(long) == 4)
      table[PAMI_MAXLOC][PAMI_LOC_LONG_INT][0] =	(void*)_pami_core_int32_int32_maxloc;
    else
      table[PAMI_MAXLOC][PAMI_LOC_LONG_INT][0] =	(void*)_pami_core_int64_int32_maxloc;
    table[PAMI_MAXLOC][PAMI_LOC_LONGDOUBLE_INT][0] =	(void*)_pami_core_fp128_int32_maxloc;

    table[PAMI_MINLOC][PAMI_LOC_2INT][0] =	(void*)_pami_core_int32_int32_minloc;
    table[PAMI_MINLOC][PAMI_LOC_SHORT_INT][0] =	(void*)_pami_core_int16_int32_minloc;
    table[PAMI_MINLOC][PAMI_LOC_FLOAT_INT][0] =	(void*)_pami_core_fp32_int32_minloc;
    table[PAMI_MINLOC][PAMI_LOC_DOUBLE_INT][0] =	(void*)_pami_core_fp64_int32_minloc;
    table[PAMI_MINLOC][PAMI_LOC_2FLOAT][0] =	(void*)_pami_core_fp32_fp32_minloc;
    table[PAMI_MINLOC][PAMI_LOC_2DOUBLE][0] =	(void*)_pami_core_fp64_fp64_minloc;
    if (sizeof(long) == 4)
      table[PAMI_MINLOC][PAMI_LOC_LONG_INT][0] =	(void*)_pami_core_int32_int32_minloc;
    else
      table[PAMI_MINLOC][PAMI_LOC_LONG_INT][0] =	(void*)_pami_core_int64_int32_minloc;
    table[PAMI_MINLOC][PAMI_LOC_LONGDOUBLE_INT][0] =	(void*)_pami_core_fp128_int32_minloc;


    COMPILE_TIME_ASSERT(sizeof(long double) == 16);
    COMPILE_TIME_ASSERT(sizeof(long) == sizeof(ssize_t));
    COMPILE_TIME_ASSERT(sizeof(unsigned long) == sizeof(size_t));

  }
};
MathOpsInit __mathOpsInit(math_op_funcs);
#undef OPTIMATH_NSRC
