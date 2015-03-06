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
 * \file math/ppc450d/math_optimath.h
 * \brief Optimized math routines
 *
 * This file describes any optimized math routines and facilitates
 * plugging them into the core math routine inlines in math_coremath.h
 *
 * Ordinarely, this file should not be included directly. Only
 * math_coremath.h should appear in source code #include's.
 *
 * This file ends up being included in all cases, i.e. users that
 * include math_coremath.h as well as those that use xxxx_bg_math.h.
 *
 * The define MATH_NO_OPTIMATH is used to turn off all optimized math
 * functions, using only the default, generic, N-way math.
 *
 * Caution, this file is exported to the distro and used by app compiles.
 * It should not be changed after building the messaging
 * libraries.  Application compiles should see the same file as was
 * used for building the product.
 */

#ifndef __math_ppc450d_math_optimath_h__
#define __math_ppc450d_math_optimath_h__

#undef MATH_NO_OPTIMATH

#if defined(__cplusplus)
extern "C" {
#endif /* C++ */
extern void _pami_core_int8_band(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_bor(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_bxor(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_land(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_lor(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_lxor(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_max(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_min(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_prod(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_sum(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_band(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_bor(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_bxor(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_land(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_lor(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_lxor(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_max(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_min(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_prod(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_sum(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_int16_band(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_bor(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_bxor(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_land(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_lor(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_lxor(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_max(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_min(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_prod(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_sum(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_int32_maxloc(int16_int32_t *dst, const int16_int32_t **srcs, int nsrc, int count);
extern void _pami_core_int16_int32_minloc(int16_int32_t *dst, const int16_int32_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_band(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_bor(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_bxor(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_land(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_lor(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_lxor(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_max(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_min(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_prod(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_sum(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_int32_band(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_bor(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_bxor(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_land(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_lor(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_lxor(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_max(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_min(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_prod(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_sum(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_int32_maxloc(int32_int32_t *dst, const int32_int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_int32_minloc(int32_int32_t *dst, const int32_int32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_band(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_bor(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_bxor(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_land(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_lor(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_lxor(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_max(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_min(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_prod(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_sum(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_int64_band(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_bor(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_bxor(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_land(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_lor(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_lxor(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_max(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_min(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_prod(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_sum(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_band(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_bor(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_bxor(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_land(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_lor(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_lxor(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_max(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_min(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_prod(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_sum(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_fp32_max(float *dst, const float **srcs, int nsrc, int count);
extern void _pami_core_fp32_min(float *dst, const float **srcs, int nsrc, int count);
extern void _pami_core_fp32_prod(float *dst, const float **srcs, int nsrc, int count);
extern void _pami_core_fp32_sum(float *dst, const float **srcs, int nsrc, int count);
extern void _pami_core_fp32_land(float *dst, const float **srcs, int nsrc, int count);
extern void _pami_core_fp32_lor(float *dst, const float **srcs, int nsrc, int count);
extern void _pami_core_fp32_lxor(float *dst, const float **srcs, int nsrc, int count);
extern void _pami_core_fp32_band(float *dst, const float **srcs, int nsrc, int count);
extern void _pami_core_fp32_bor(float *dst, const float **srcs, int nsrc, int count);
extern void _pami_core_fp32_bxor(float *dst, const float **srcs, int nsrc, int count);
extern void _pami_core_fp32_int32_maxloc(fp32_int32_t *dst, const fp32_int32_t **srcs, int nsrc, int count);
extern void _pami_core_fp32_fp32_maxloc(fp32_fp32_t *dst, const fp32_fp32_t **srcs, int nsrc, int count);
extern void _pami_core_fp32_int32_minloc(fp32_int32_t *dst, const fp32_int32_t **srcs, int nsrc, int count);
extern void _pami_core_fp32_fp32_minloc(fp32_fp32_t *dst, const fp32_fp32_t **srcs, int nsrc, int count);
extern void _pami_core_fp32_fp32_cplx_prod(fp32_fp32_t *dst, const fp32_fp32_t **srcs, int nsrc, int count);
extern void _pami_core_fp64_max(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_fp64_min(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_fp64_prod(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_fp64_sum(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_fp64_land(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_fp64_lor(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_fp64_lxor(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_fp64_band(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_fp64_bor(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_fp64_bxor(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_fp64_int32_maxloc(fp64_int32_t *dst, const fp64_int32_t **srcs, int nsrc, int count);
extern void _pami_core_fp64_fp64_maxloc(fp64_fp64_t *dst, const fp64_fp64_t **srcs, int nsrc, int count);
extern void _pami_core_fp64_int32_minloc(fp64_int32_t *dst, const fp64_int32_t **srcs, int nsrc, int count);
extern void _pami_core_fp64_fp64_minloc(fp64_fp64_t *dst, const fp64_fp64_t **srcs, int nsrc, int count);
extern void _pami_core_fp64_fp64_cplx_prod(fp64_fp64_t *dst, const fp64_fp64_t **srcs, int nsrc, int count);
extern void _pami_core_fp128_max(long double *dst, const long double **srcs, int nsrc, int count);
extern void _pami_core_fp128_min(long double *dst, const long double **srcs, int nsrc, int count);
extern void _pami_core_fp128_prod(long double *dst, const long double **srcs, int nsrc, int count);
extern void _pami_core_fp128_sum(long double *dst, const long double **srcs, int nsrc, int count);

/* Optimized routines */

extern void _pami_core_fp128_max2(long double *dst, const long double **srcs, int nsrc, int count);
extern void _pami_core_fp128_min2(long double *dst, const long double **srcs, int nsrc, int count);
extern void _pami_core_fp128_prod2(long double *dst, const long double **srcs, int nsrc, int count);
extern void _pami_core_fp128_sum2(long double *dst, const long double **srcs, int nsrc, int count);
extern void _pami_core_fp32_fp32_maxloc2(fp32_fp32_t *dst, const fp32_fp32_t **srcs, int nsrc, int count);
extern void _pami_core_fp32_fp32_minloc2(fp32_fp32_t *dst, const fp32_fp32_t **srcs, int nsrc, int count);
extern void _pami_core_fp32_int32_maxloc2(fp32_int32_t *dst, const fp32_int32_t **srcs, int nsrc, int count);
extern void _pami_core_fp32_int32_minloc2(fp32_int32_t *dst, const fp32_int32_t **srcs, int nsrc, int count);
extern void _pami_core_fp32_max2(float *dst, const float **srcs, int nsrc, int count);
extern void _pami_core_fp32_min2(float *dst, const float **srcs, int nsrc, int count);
extern void _pami_core_fp32_prod2(float *dst, const float **srcs, int nsrc, int count);
extern void _pami_core_fp32_sum2(float *dst, const float **srcs, int nsrc, int count);
extern void _pami_core_fp64_fp64_maxloc2(fp64_fp64_t *dst, const fp64_fp64_t **srcs, int nsrc, int count);
extern void _pami_core_fp64_fp64_minloc2(fp64_fp64_t *dst, const fp64_fp64_t **srcs, int nsrc, int count);
extern void _pami_core_fp64_int32_maxloc2(fp64_int32_t *dst, const fp64_int32_t **srcs, int nsrc, int count);
extern void _pami_core_fp64_int32_minloc2(fp64_int32_t *dst, const fp64_int32_t **srcs, int nsrc, int count);
extern void _pami_core_fp64_max2(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_fp64_min2(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_fp64_prod2(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_fp64_sum2(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_fp64_max4(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_fp64_min4(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_fp64_prod4(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_fp64_sum4(double *dst, const double **srcs, int nsrc, int count);
extern void _pami_core_int16_band2(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_bor2(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_bxor2(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_int32_maxloc2(int16_int32_t *dst, const int16_int32_t **srcs, int nsrc, int count);
extern void _pami_core_int16_int32_minloc2(int16_int32_t *dst, const int16_int32_t **srcs, int nsrc, int count);
extern void _pami_core_int16_land2(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_lor2(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_lxor2(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_max2(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_min2(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_prod2(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int16_sum2(int16_t *dst, const int16_t **srcs, int nsrc, int count);
extern void _pami_core_int32_band2(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_bor2(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_bxor2(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_int32_maxloc2(int32_int32_t *dst, const int32_int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_int32_minloc2(int32_int32_t *dst, const int32_int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_land2(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_lor2(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_lxor2(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_max2(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_min2(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_prod2(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_sum2(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_band4(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_bor4(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_bxor4(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_max4(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_min4(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_prod4(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int32_sum4(int32_t *dst, const int32_t **srcs, int nsrc, int count);
extern void _pami_core_int64_band2(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_bor2(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_bxor2(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_land2(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_lor2(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_lxor2(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_max2(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_min2(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_prod2(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int64_sum2(int64_t *dst, const int64_t **srcs, int nsrc, int count);
extern void _pami_core_int8_band2(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_bor2(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_bxor2(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_land2(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_lor2(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_lxor2(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_max2(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_min2(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_prod2(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_int8_sum2(int8_t *dst, const int8_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_band2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_bor2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_bxor2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_land2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_lor2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_lxor2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_max2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_min2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_prod2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint16_sum2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_band2(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_bor2(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_bxor2(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_land2(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_lor2(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_lxor2(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_max2(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_min2(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_prod2(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_sum2(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_band4(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_bor4(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_bxor4(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_max4(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_min4(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_prod4(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint32_sum4(uint32_t *dst, const uint32_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_band2(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_bor2(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_bxor2(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_land2(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_lor2(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_lxor2(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_max2(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_min2(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_prod2(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint64_sum2(uint64_t *dst, const uint64_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_band2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_bor2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_bxor2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_land2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_lor2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_lxor2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_max2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_min2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_prod2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);
extern void _pami_core_uint8_sum2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count);

#if defined(__cplusplus)
}; // extern "C"
#endif /* C++ */

/**
 * N-way (N >= 2) routines.
 *
 * \param[out] dst	Results buffer pointer
 * \param[in] srcs	Pointer to array of pointers to source buffers
 * \param[in] nsrc	Number of source buffers
 * \param[in] count	Number of elements in each buffer
 *
 * If a particular routine has optimized versions, for specific source
 * buffer counts, then the corresponding define here will have
 * statements added in the form:
 *
 *	OPTIMATH_NSRC(dt,op,N,_pami_core_<type>_<oper>N)
 *	...
 *
 * Where "N" is the number of input buffers, "<type>" is the datatype,
 * "<oper>" is the operand mnemonic, "dt" is the datatype, and "op"
 * is the operand.  Additionally, there must be a prototype
 * declaration for the optimized routine(s). More than one statement
 * may be present in the macro.
 *
 * Don't forget the backslashes at the end of each line.
 *
 * The code in math_coremath.h that uses these will
 * be (note: no semicolon after macro):
 *
 * #define OPTIMATH_NSRC(dt,op,n,f) case n: \
 *					f(dst, srcs, nsrc, count); \
 *					break;
 *
 * inline void Core_<type>_<oper>(params...) {
 * 	switch(nsrc) {
 *	OPTIMIZED_<type>_<oper>
 *	default:
 *		_pami_core_<type>_<oper>(params...);
 *		break;
 *	}
 * }
 *
 * These defines will also be used in xxxx_dat.c to build a table
 * of optimized routines, by datatype, operand, and number of inputs.
 * (Note, nsrc will always be at least 2)
 *
 * #define OPTIMATH_NSRC(dt,op,n,f)	[dt][op][n-2] = f,
 *
 * void *math_op_funcs[ndt][nop][nin] = {
 * OPTIMIZED_<type>_<oper>
 * ...
 * };
 *
 * This table is accessed by using the MATH_OP_FUNCS(dt,op,n) macro
 * (inline).
 */
#define OPTIMIZED_int8_band	\
        OPTIMATH_NSRC(PAMI_SIGNED_CHAR,PAMI_BAND,2,_pami_core_int8_band2)
#define OPTIMIZED_int8_bor	\
        OPTIMATH_NSRC(PAMI_SIGNED_CHAR,PAMI_BOR,2,_pami_core_int8_bor2)
#define OPTIMIZED_int8_bxor	\
        OPTIMATH_NSRC(PAMI_SIGNED_CHAR,PAMI_BXOR,2,_pami_core_int8_bxor2)
#define OPTIMIZED_int8_land	\
        OPTIMATH_NSRC(PAMI_SIGNED_CHAR,PAMI_LAND,2,_pami_core_int8_land2)
#define OPTIMIZED_int8_lor	\
        OPTIMATH_NSRC(PAMI_SIGNED_CHAR,PAMI_LOR,2,_pami_core_int8_lor2)
#define OPTIMIZED_int8_lxor	\
        OPTIMATH_NSRC(PAMI_SIGNED_CHAR,PAMI_LXOR,2,_pami_core_int8_lxor2)
#define OPTIMIZED_int8_max	\
        OPTIMATH_NSRC(PAMI_SIGNED_CHAR,PAMI_MAX,2,_pami_core_int8_max2)
#define OPTIMIZED_int8_min	\
        OPTIMATH_NSRC(PAMI_SIGNED_CHAR,PAMI_MIN,2,_pami_core_int8_min2)
#define OPTIMIZED_int8_prod	\
        OPTIMATH_NSRC(PAMI_SIGNED_CHAR,PAMI_PROD,2,_pami_core_int8_prod2)
#define OPTIMIZED_int8_sum	\
        OPTIMATH_NSRC(PAMI_SIGNED_CHAR,PAMI_SUM,2,_pami_core_int8_sum2)
#define OPTIMIZED_uint8_band	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_CHAR,PAMI_BAND,2,_pami_core_uint8_band2)
#define OPTIMIZED_uint8_bor	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_CHAR,PAMI_BOR,2,_pami_core_uint8_bor2)
#define OPTIMIZED_uint8_bxor	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_CHAR,PAMI_BXOR,2,_pami_core_uint8_bxor2)
#define OPTIMIZED_uint8_land	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_CHAR,PAMI_LAND,2,_pami_core_uint8_land2)
#define OPTIMIZED_uint8_lor	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_CHAR,PAMI_LOR,2,_pami_core_uint8_lor2)
#define OPTIMIZED_uint8_lxor	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_CHAR,PAMI_LXOR,2,_pami_core_uint8_lxor2)
#define OPTIMIZED_uint8_max	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_CHAR,PAMI_MAX,2,_pami_core_uint8_max2)
#define OPTIMIZED_uint8_min	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_CHAR,PAMI_MIN,2,_pami_core_uint8_min2)
#define OPTIMIZED_uint8_prod	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_CHAR,PAMI_PROD,2,_pami_core_uint8_prod2)
#define OPTIMIZED_uint8_sum	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_CHAR,PAMI_SUM,2,_pami_core_uint8_sum2)
#define OPTIMIZED_int16_band	\
        OPTIMATH_NSRC(PAMI_SIGNED_SHORT,PAMI_BAND,2,_pami_core_int16_band2)
#define OPTIMIZED_int16_bor	\
        OPTIMATH_NSRC(PAMI_SIGNED_SHORT,PAMI_BOR,2,_pami_core_int16_bor2)
#define OPTIMIZED_int16_bxor	\
        OPTIMATH_NSRC(PAMI_SIGNED_SHORT,PAMI_BXOR,2,_pami_core_int16_bxor2)
#define OPTIMIZED_int16_land	\
        OPTIMATH_NSRC(PAMI_SIGNED_SHORT,PAMI_LAND,2,_pami_core_int16_land2)
#define OPTIMIZED_int16_lor	\
        OPTIMATH_NSRC(PAMI_SIGNED_SHORT,PAMI_LOR,2,_pami_core_int16_lor2)
#define OPTIMIZED_int16_lxor	\
        OPTIMATH_NSRC(PAMI_SIGNED_SHORT,PAMI_LXOR,2,_pami_core_int16_lxor2)
#define OPTIMIZED_int16_max	\
        OPTIMATH_NSRC(PAMI_SIGNED_SHORT,PAMI_MAX,2,_pami_core_int16_max2)
#define OPTIMIZED_int16_min	\
        OPTIMATH_NSRC(PAMI_SIGNED_SHORT,PAMI_MIN,2,_pami_core_int16_min2)
#define OPTIMIZED_int16_prod	\
        OPTIMATH_NSRC(PAMI_SIGNED_SHORT,PAMI_PROD,2,_pami_core_int16_prod2)
#define OPTIMIZED_int16_sum	\
        OPTIMATH_NSRC(PAMI_SIGNED_SHORT,PAMI_SUM,2,_pami_core_int16_sum2)
#define OPTIMIZED_int16_int32_maxloc	\
        OPTIMATH_NSRC(PAMI_LOC_SHORT_INT,PAMI_MAXLOC,2,_pami_core_int16_int32_maxloc2)
#define OPTIMIZED_int16_int32_minloc	\
        OPTIMATH_NSRC(PAMI_LOC_SHORT_INT,PAMI_MINLOC,2,_pami_core_int16_int32_minloc2)
#define OPTIMIZED_uint16_band	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_SHORT,PAMI_BAND,2,_pami_core_uint16_band2)
#define OPTIMIZED_uint16_bor	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_SHORT,PAMI_BOR,2,_pami_core_uint16_bor2)
#define OPTIMIZED_uint16_bxor	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_SHORT,PAMI_BXOR,2,_pami_core_uint16_bxor2)
#define OPTIMIZED_uint16_land	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_SHORT,PAMI_LAND,2,_pami_core_uint16_land2)
#define OPTIMIZED_uint16_lor	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_SHORT,PAMI_LOR,2,_pami_core_uint16_lor2)
#define OPTIMIZED_uint16_lxor	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_SHORT,PAMI_LXOR,2,_pami_core_uint16_lxor2)
#define OPTIMIZED_uint16_max	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_SHORT,PAMI_MAX,2,_pami_core_uint16_max2)
#define OPTIMIZED_uint16_min	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_SHORT,PAMI_MIN,2,_pami_core_uint16_min2)
#define OPTIMIZED_uint16_prod	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_SHORT,PAMI_PROD,2,_pami_core_uint16_prod2)
#define OPTIMIZED_uint16_sum	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_SHORT,PAMI_SUM,2,_pami_core_uint16_sum2)
#define OPTIMIZED_int32_band	\
        OPTIMATH_NSRC(PAMI_SIGNED_INT,PAMI_BAND,2,_pami_core_int32_band2)\
        OPTIMATH_NSRC(PAMI_SIGNED_INT,PAMI_BAND,4,_pami_core_int32_band4)
#define OPTIMIZED_int32_bor	\
        OPTIMATH_NSRC(PAMI_SIGNED_INT,PAMI_BOR,2,_pami_core_int32_bor2)\
        OPTIMATH_NSRC(PAMI_SIGNED_INT,PAMI_BOR,4,_pami_core_int32_bor4)
#define OPTIMIZED_int32_bxor	\
        OPTIMATH_NSRC(PAMI_SIGNED_INT,PAMI_BXOR,2,_pami_core_int32_bxor2)\
        OPTIMATH_NSRC(PAMI_SIGNED_INT,PAMI_BXOR,4,_pami_core_int32_bxor4)
#define OPTIMIZED_int32_land	\
        OPTIMATH_NSRC(PAMI_SIGNED_INT,PAMI_LAND,2,_pami_core_int32_land2)
#define OPTIMIZED_int32_lor	\
        OPTIMATH_NSRC(PAMI_SIGNED_INT,PAMI_LOR,2,_pami_core_int32_lor2)
#define OPTIMIZED_int32_lxor	\
        OPTIMATH_NSRC(PAMI_SIGNED_INT,PAMI_LXOR,2,_pami_core_int32_lxor2)
#define OPTIMIZED_int32_max	\
        OPTIMATH_NSRC(PAMI_SIGNED_INT,PAMI_MAX,2,_pami_core_int32_max2) \
        OPTIMATH_NSRC(PAMI_SIGNED_INT,PAMI_MAX,4,_pami_core_int32_max4)
#define OPTIMIZED_int32_min	\
        OPTIMATH_NSRC(PAMI_SIGNED_INT,PAMI_MIN,2,_pami_core_int32_min2) \
        OPTIMATH_NSRC(PAMI_SIGNED_INT,PAMI_MIN,4,_pami_core_int32_min4)
#define OPTIMIZED_int32_prod	\
        OPTIMATH_NSRC(PAMI_SIGNED_INT,PAMI_PROD,2,_pami_core_int32_prod2)\
        OPTIMATH_NSRC(PAMI_SIGNED_INT,PAMI_PROD,4,_pami_core_int32_prod4)
#define OPTIMIZED_int32_sum	\
        OPTIMATH_NSRC(PAMI_SIGNED_INT,PAMI_SUM,2,_pami_core_int32_sum2) \
        OPTIMATH_NSRC(PAMI_SIGNED_INT,PAMI_SUM,4,_pami_core_int32_sum4)
#define OPTIMIZED_int32_int32_maxloc	\
        OPTIMATH_NSRC(PAMI_LOC_2INT,PAMI_MAXLOC,2,_pami_core_int32_int32_maxloc2)
#define OPTIMIZED_int32_int32_minloc	\
        OPTIMATH_NSRC(PAMI_LOC_2INT,PAMI_MINLOC,2,_pami_core_int32_int32_minloc2)
#define OPTIMIZED_uint32_band	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_INT,PAMI_BAND,2,_pami_core_uint32_band2)\
        OPTIMATH_NSRC(PAMI_UNSIGNED_INT,PAMI_BAND,4,_pami_core_uint32_band4)
#define OPTIMIZED_uint32_bor	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_INT,PAMI_BOR,2,_pami_core_uint32_bor2) \
        OPTIMATH_NSRC(PAMI_UNSIGNED_INT,PAMI_BOR,4,_pami_core_uint32_bor4)
#define OPTIMIZED_uint32_bxor	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_INT,PAMI_BXOR,2,_pami_core_uint32_bxor2)\
        OPTIMATH_NSRC(PAMI_UNSIGNED_INT,PAMI_BXOR,4,_pami_core_uint32_bxor4)
#define OPTIMIZED_uint32_land	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_INT,PAMI_LAND,2,_pami_core_uint32_land2)
#define OPTIMIZED_uint32_lor	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_INT,PAMI_LOR,2,_pami_core_uint32_lor2)
#define OPTIMIZED_uint32_lxor	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_INT,PAMI_LXOR,2,_pami_core_uint32_lxor2)
#define OPTIMIZED_uint32_max	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_INT,PAMI_MAX,2,_pami_core_uint32_max2) \
        OPTIMATH_NSRC(PAMI_UNSIGNED_INT,PAMI_MAX,4,_pami_core_uint32_max4)
#define OPTIMIZED_uint32_min	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_INT,PAMI_MIN,2,_pami_core_uint32_min2) \
        OPTIMATH_NSRC(PAMI_UNSIGNED_INT,PAMI_MIN,4,_pami_core_uint32_min4)
#define OPTIMIZED_uint32_prod	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_INT,PAMI_PROD,2,_pami_core_uint32_prod2)\
        OPTIMATH_NSRC(PAMI_UNSIGNED_INT,PAMI_PROD,4,_pami_core_uint32_prod4)
#define OPTIMIZED_uint32_sum	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_INT,PAMI_SUM,2,_pami_core_uint32_sum2) \
        OPTIMATH_NSRC(PAMI_UNSIGNED_INT,PAMI_SUM,4,_pami_core_uint32_sum4)
#define OPTIMIZED_int64_band	\
        OPTIMATH_NSRC(PAMI_SIGNED_LONG_LONG,PAMI_BAND,2,_pami_core_int64_band2)
#define OPTIMIZED_int64_bor	\
        OPTIMATH_NSRC(PAMI_SIGNED_LONG_LONG,PAMI_BOR,2,_pami_core_int64_bor2)
#define OPTIMIZED_int64_bxor	\
        OPTIMATH_NSRC(PAMI_SIGNED_LONG_LONG,PAMI_BXOR,2,_pami_core_int64_bxor2)
#define OPTIMIZED_int64_land	\
        OPTIMATH_NSRC(PAMI_SIGNED_LONG_LONG,PAMI_LAND,2,_pami_core_int64_land2)
#define OPTIMIZED_int64_lor	\
        OPTIMATH_NSRC(PAMI_SIGNED_LONG_LONG,PAMI_LOR,2,_pami_core_int64_lor2)
#define OPTIMIZED_int64_lxor	\
        OPTIMATH_NSRC(PAMI_SIGNED_LONG_LONG,PAMI_LXOR,2,_pami_core_int64_lxor2)
#define OPTIMIZED_int64_max	\
        OPTIMATH_NSRC(PAMI_SIGNED_LONG_LONG,PAMI_MAX,2,_pami_core_int64_max2)
#define OPTIMIZED_int64_min	\
        OPTIMATH_NSRC(PAMI_SIGNED_LONG_LONG,PAMI_MIN,2,_pami_core_int64_min2)
#define OPTIMIZED_int64_prod	\
        OPTIMATH_NSRC(PAMI_SIGNED_LONG_LONG,PAMI_PROD,2,_pami_core_int64_prod2)
#define OPTIMIZED_int64_sum	\
        OPTIMATH_NSRC(PAMI_SIGNED_LONG_LONG,PAMI_SUM,2,_pami_core_int64_sum2)
#define OPTIMIZED_uint64_band	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_LONG_LONG,PAMI_BAND,2,_pami_core_uint64_band2)
#define OPTIMIZED_uint64_bor	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_LONG_LONG,PAMI_BOR,2,_pami_core_uint64_bor2)
#define OPTIMIZED_uint64_bxor	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_LONG_LONG,PAMI_BXOR,2,_pami_core_uint64_bxor2)
#define OPTIMIZED_uint64_land	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_LONG_LONG,PAMI_LAND,2,_pami_core_uint64_land2)
#define OPTIMIZED_uint64_lor	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_LONG_LONG,PAMI_LOR,2,_pami_core_uint64_lor2)
#define OPTIMIZED_uint64_lxor	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_LONG_LONG,PAMI_LXOR,2,_pami_core_uint64_lxor2)
#define OPTIMIZED_uint64_max	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_LONG_LONG,PAMI_MAX,2,_pami_core_uint64_max2)
#define OPTIMIZED_uint64_min	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_LONG_LONG,PAMI_MIN,2,_pami_core_uint64_min2)
#define OPTIMIZED_uint64_prod	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_LONG_LONG,PAMI_PROD,2,_pami_core_uint64_prod2)
#define OPTIMIZED_uint64_sum	\
        OPTIMATH_NSRC(PAMI_UNSIGNED_LONG_LONG,PAMI_SUM,2,_pami_core_uint64_sum2)
#define OPTIMIZED_fp32_max	\
        OPTIMATH_NSRC(PAMI_FLOAT,PAMI_MAX,2,_pami_core_fp32_max2)
#define OPTIMIZED_fp32_min	\
        OPTIMATH_NSRC(PAMI_FLOAT,PAMI_MIN,2,_pami_core_fp32_min2)
#define OPTIMIZED_fp32_prod	\
        OPTIMATH_NSRC(PAMI_FLOAT,PAMI_PROD,2,_pami_core_fp32_prod2)
#define OPTIMIZED_fp32_sum	\
        OPTIMATH_NSRC(PAMI_FLOAT,PAMI_SUM,2,_pami_core_fp32_sum2)
#define OPTIMIZED_fp32_int32_maxloc	\
        OPTIMATH_NSRC(PAMI_LOC_FLOAT_INT,PAMI_MAXLOC,2,_pami_core_fp32_int32_maxloc2)
#define OPTIMIZED_fp32_fp32_maxloc	\
        OPTIMATH_NSRC(PAMI_LOC_2FLOAT,PAMI_MAXLOC,2,_pami_core_fp32_fp32_maxloc2)
#define OPTIMIZED_fp32_int32_minloc	\
        OPTIMATH_NSRC(PAMI_LOC_FLOAT_INT,PAMI_MINLOC,2,_pami_core_fp32_int32_minloc2)
#define OPTIMIZED_fp32_fp32_minloc	\
        OPTIMATH_NSRC(PAMI_LOC_2FLOAT,PAMI_MINLOC,2,_pami_core_fp32_fp32_minloc2)
#define OPTIMIZED_fp32_fp32_cmplx_prod	\
        OPTIMATH_NSRC(PAMI_SINGLE_COMPLEX,PAMI_PROD,2,_pami_core_fp32_fp32_cmplx_prod)
#define OPTIMIZED_fp64_max	\
        OPTIMATH_NSRC(PAMI_DOUBLE,PAMI_MAX,2,_pami_core_fp64_max2)	\
        OPTIMATH_NSRC(PAMI_DOUBLE,PAMI_MAX,4,_pami_core_fp64_max4)
#define OPTIMIZED_fp64_min	\
        OPTIMATH_NSRC(PAMI_DOUBLE,PAMI_MIN,2,_pami_core_fp64_min2)	\
        OPTIMATH_NSRC(PAMI_DOUBLE,PAMI_MIN,4,_pami_core_fp64_min4)
#define OPTIMIZED_fp64_prod	\
        OPTIMATH_NSRC(PAMI_DOUBLE,PAMI_PROD,2,_pami_core_fp64_prod2)	\
        OPTIMATH_NSRC(PAMI_DOUBLE,PAMI_PROD,4,_pami_core_fp64_prod4)
#define OPTIMIZED_fp64_sum	\
        OPTIMATH_NSRC(PAMI_DOUBLE,PAMI_SUM,2,_pami_core_fp64_sum2)	\
        OPTIMATH_NSRC(PAMI_DOUBLE,PAMI_SUM,4,_pami_core_fp64_sum4)
#define OPTIMIZED_fp64_int32_maxloc	\
        OPTIMATH_NSRC(PAMI_LOC_DOUBLE_INT,PAMI_MAXLOC,2,_pami_core_fp64_int32_maxloc2)
#define OPTIMIZED_fp64_fp64_maxloc	\
        OPTIMATH_NSRC(PAMI_LOC_2DOUBLE,PAMI_MAXLOC,2,_pami_core_fp64_fp64_maxloc2)
#define OPTIMIZED_fp64_int32_minloc	\
        OPTIMATH_NSRC(PAMI_LOC_DOUBLE_INT,PAMI_MINLOC,2,_pami_core_fp64_int32_minloc2)
#define OPTIMIZED_fp64_fp64_minloc	\
        OPTIMATH_NSRC(PAMI_LOC_2DOUBLE,PAMI_MINLOC,2,_pami_core_fp64_fp64_minloc2)
#define OPTIMIZED_fp64_fp64_cmplx_prod	\
        OPTIMATH_NSRC(PAMI_DOUBLE_COMPLEX,PAMI_PROD,2,_pami_core_fp64_fp64_cmplx_prod)
#define OPTIMIZED_fp128_max	\
        OPTIMATH_NSRC(PAMI_LONG_DOUBLE,PAMI_MAX,2,_pami_core_fp128_max2)
#define OPTIMIZED_fp128_min	\
        OPTIMATH_NSRC(PAMI_LONG_DOUBLE,PAMI_MIN,2,_pami_core_fp128_min2)
#define OPTIMIZED_fp128_prod	\
        OPTIMATH_NSRC(PAMI_LONG_DOUBLE,PAMI_PROD,2,_pami_core_fp128_prod2)
#define OPTIMIZED_fp128_sum	\
        OPTIMATH_NSRC(PAMI_LONG_DOUBLE,PAMI_SUM,2,_pami_core_fp128_sum2)

/**
 * The simple, unary, routines.
 *
 * \param[out] dst	Results buffer pointer
 * \param[in] src	Source buffer pointer
 * \param[in] count	Number of elements to process
 *
 * If a particular routine has an optimized version,
 * then the corresponding define here will have
 * statements added in the form:
 *
 *	OPTIMATH_UNARY(dt,op,_pami_core_<type>_<oper>_o)
 *
 * Where "<type>" is the datatype,
 * "<oper>" is the operand mnemonic, "dt" is the datatype, and "op"
 * is the operand.  Additionally, there must be a prototype
 * declaration for the optimized routine. Only one statement
 * may be present in the macro.
 *
 * The code in xxxx_bg_math.h that uses these will
 * be (note: no semicolon after macro):
 *
 * #define OPTIMATH_UNARY(dt,op,f) case 1: \
 *					f(dst, src, count); \
 *					break;
 *
 * inline void Core_<type>_<oper>(params...) {
 * 	switch(1) {
 *	OPTIMIZED_<type>_<oper>
 *	default:
 *		_pami_core_<type>_<oper>(params...);
 *		break;
 *	}
 * }
 *
 * At present there are no general purpose unary functions.
 */

#endif /* _math_optimath_h_ */
