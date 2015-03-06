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
 * \file math/common/math_optimath.h
 * \brief Optimized math routines
 *
 * This file describes any optimized math routines and facilitates
 * plugging them into the core math routine inlines in math_coremath.h
 *
 * Ordinarely, this file should not be included directly. Only
 * math_coremath.h should appear in source code #include's.
 *
 * Caution, this file is exported to the distro and used in application compiles.
 * It should not be changed after building the messaging
 * libraries.  Application compiles should see the same file as was
 * used for building the product.
 */

#ifndef __math_common_math_optimath_h__
#define __math_common_math_optimath_h__

/**
 * \fn void OPTIMATH_NSRC(PAMI_Dt dt, PAMI_Op op, int nsrc, coremath func)
 * \brief Macro used to generate code to call optimized math functions
 * \param dt	Datatype of math to perform
 * \param op	Math operation to perform
 * \param nsrc	Number of inputs (buffers) to work on
 * \param func	Function to use
 * Used in multiple contexts to generate different code.
 */

/**
 * \fn void OPTIMATH_UNARY(PAMI_Dt dt, PAMI_Op op, coremath1 func)
 * \brief Macro used to generate code to call optimized math functions
 * \param dt	Datatype of math to perform
 * \param op	Math operation to perform
 * \param func	Function to use
 * Used in multiple contexts to generate different code.
 */

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
extern void _pami_core_int64_int32_maxloc(int64_int32_t *dst, const int64_int32_t **srcs, int nsrc, int count);
extern void _pami_core_int64_int32_minloc(int64_int32_t *dst, const int64_int32_t **srcs, int nsrc, int count);
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
extern void _pami_core_fp128_int32_maxloc(fp128_int32_t *dst, const fp128_int32_t **srcs, int nsrc, int count);
extern void _pami_core_fp128_int32_minloc(fp128_int32_t *dst, const fp128_int32_t **srcs, int nsrc, int count);

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
#define OPTIMIZED_int8_band
#define OPTIMIZED_int8_bor
#define OPTIMIZED_int8_bxor
#define OPTIMIZED_int8_land
#define OPTIMIZED_int8_lor
#define OPTIMIZED_int8_lxor
#define OPTIMIZED_int8_max
#define OPTIMIZED_int8_min
#define OPTIMIZED_int8_prod
#define OPTIMIZED_int8_sum
#define OPTIMIZED_uint8_band
#define OPTIMIZED_uint8_bor
#define OPTIMIZED_uint8_bxor
#define OPTIMIZED_uint8_land
#define OPTIMIZED_uint8_lor
#define OPTIMIZED_uint8_lxor
#define OPTIMIZED_uint8_max
#define OPTIMIZED_uint8_min
#define OPTIMIZED_uint8_prod
#define OPTIMIZED_uint8_sum
#define OPTIMIZED_int16_band
#define OPTIMIZED_int16_bor
#define OPTIMIZED_int16_bxor
#define OPTIMIZED_int16_land
#define OPTIMIZED_int16_lor
#define OPTIMIZED_int16_lxor
#define OPTIMIZED_int16_max
#define OPTIMIZED_int16_min
#define OPTIMIZED_int16_prod
#define OPTIMIZED_int16_sum
#define OPTIMIZED_int16_int32_maxloc
#define OPTIMIZED_int16_int32_minloc
#define OPTIMIZED_uint16_band
#define OPTIMIZED_uint16_bor
#define OPTIMIZED_uint16_bxor
#define OPTIMIZED_uint16_land
#define OPTIMIZED_uint16_lor
#define OPTIMIZED_uint16_lxor
#define OPTIMIZED_uint16_max
#define OPTIMIZED_uint16_min
#define OPTIMIZED_uint16_prod
#define OPTIMIZED_uint16_sum
#define OPTIMIZED_int32_band
#define OPTIMIZED_int32_bor
#define OPTIMIZED_int32_bxor
#define OPTIMIZED_int32_land
#define OPTIMIZED_int32_lor
#define OPTIMIZED_int32_lxor
#define OPTIMIZED_int32_max
#define OPTIMIZED_int32_min
#define OPTIMIZED_int32_prod
#define OPTIMIZED_int32_sum
#define OPTIMIZED_int32_int32_maxloc
#define OPTIMIZED_int32_int32_minloc
#define OPTIMIZED_uint32_band
#define OPTIMIZED_uint32_bor
#define OPTIMIZED_uint32_bxor
#define OPTIMIZED_uint32_land
#define OPTIMIZED_uint32_lor
#define OPTIMIZED_uint32_lxor
#define OPTIMIZED_uint32_max
#define OPTIMIZED_uint32_min
#define OPTIMIZED_uint32_prod
#define OPTIMIZED_uint32_sum
#define OPTIMIZED_int64_band
#define OPTIMIZED_int64_bor
#define OPTIMIZED_int64_bxor
#define OPTIMIZED_int64_land
#define OPTIMIZED_int64_lor
#define OPTIMIZED_int64_lxor
#define OPTIMIZED_int64_max
#define OPTIMIZED_int64_min
#define OPTIMIZED_int64_prod
#define OPTIMIZED_int64_sum
#define OPTIMIZED_int64_int32_maxloc
#define OPTIMIZED_int64_int32_minloc
#define OPTIMIZED_uint64_band
#define OPTIMIZED_uint64_bor
#define OPTIMIZED_uint64_bxor
#define OPTIMIZED_uint64_land
#define OPTIMIZED_uint64_lor
#define OPTIMIZED_uint64_lxor
#define OPTIMIZED_uint64_max
#define OPTIMIZED_uint64_min
#define OPTIMIZED_uint64_prod
#define OPTIMIZED_uint64_sum
#define OPTIMIZED_fp32_max
#define OPTIMIZED_fp32_min
#define OPTIMIZED_fp32_prod
#define OPTIMIZED_fp32_sum
#define OPTIMIZED_fp32_int32_maxloc
#define OPTIMIZED_fp32_fp32_maxloc
#define OPTIMIZED_fp32_int32_minloc
#define OPTIMIZED_fp32_fp32_minloc
#define OPTIMIZED_fp32_fp32_cmplx_prod
#define OPTIMIZED_fp64_max
#define OPTIMIZED_fp64_min
#define OPTIMIZED_fp64_prod
#define OPTIMIZED_fp64_sum
#define OPTIMIZED_fp64_int32_maxloc
#define OPTIMIZED_fp64_fp64_maxloc
#define OPTIMIZED_fp64_int32_minloc
#define OPTIMIZED_fp64_fp64_minloc
#define OPTIMIZED_fp64_fp64_cmplx_prod
#define OPTIMIZED_fp128_max
#define OPTIMIZED_fp128_min
#define OPTIMIZED_fp128_prod
#define OPTIMIZED_fp128_sum
#define OPTIMIZED_fp128_int32_maxloc
#define OPTIMIZED_fp128_int32_minloc

#endif /* _math_optimath_h_ */
