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
 * \file math/bgp/collective_network/pami_optibgmath.h
 * \brief Optimized collective network math routines
 *
 * This file describes any optimized math routines and facilitates
 * plugging them into the core math routine inlines in pami_bg_math.h
 *
 * Ordinarely, this file should not be included directly. Only
 * pami_bg_math.h should appear in source code #include's.
 *
 * These routines are the ones used by the collective network to
 * process data for use on the tree. These are not public interfaces.
 *
 * Caution, this file should not be changed after building the PAMI
 * libraries.  Application compiles should see the same file as was
 * used for building the product.
 */

#ifndef __math_bgp_collective_network_pami_optibgmath_h__
#define __math_bgp_collective_network_pami_optibgmath_h__

/**
 * \brief Convert a LOC value TO a tree value
 * Convert LOC (MINLOC/MAXLOC) values to collective network
 * values that ensures signs are observed.
 */
#define LOC_INT_TOTREE(i)     ((i) ^ 0x7fffffffUL)
/**
 * \brief Convert a LOC value FROM a tree value
 * Convert LOC (MINLOC/MAXLOC) values to collective network
 * values that ensures signs are observed.
 */
#define LOC_INT_FRTREE(i)     ((i) ^ 0x7fffffffUL)

#if defined(__cplusplus)
extern "C" {
#endif /* C++ */

extern void _pami_core_fp32_fp32_post_maxloc(fp32_fp32_t *dst, const fp32_fp32_t *src, int count);
extern void _pami_core_fp32_fp32_post_minloc(fp32_fp32_t *dst, const fp32_fp32_t *src, int count);
extern void _pami_core_fp32_fp32_pre_maxloc(fp32_fp32_t *dst, const fp32_fp32_t *src, int count);
extern void _pami_core_fp32_fp32_pre_minloc(fp32_fp32_t *dst, const fp32_fp32_t *src, int count);
extern void _pami_core_fp32_int32_post_maxloc(fp32_int32_t *dst, const fp32_int32_t *src, int count);
extern void _pami_core_fp32_int32_post_minloc(fp32_int32_t *dst, const fp32_int32_t *src, int count);
extern void _pami_core_fp32_int32_pre_maxloc(fp32_int32_t *dst, const fp32_int32_t *src, int count);
extern void _pami_core_fp32_int32_pre_minloc(fp32_int32_t *dst, const fp32_int32_t *src, int count);
extern void _pami_core_fp32_post_max(float *dst, const float *src, int count);
extern void _pami_core_fp32_post_min(float *dst, const float *src, int count);
extern void _pami_core_fp32_pre_max(float *dst, const float *src, int count);
extern void _pami_core_fp32_pre_min(float *dst, const float *src, int count);
extern void _pami_core_fp64_fp64_post_maxloc(fp64_fp64_t *dst, const fp64_fp64_t *src, int count);
extern void _pami_core_fp64_fp64_post_minloc(fp64_fp64_t *dst, const fp64_fp64_t *src, int count);
extern void _pami_core_fp64_fp64_pre_maxloc(fp64_fp64_t *dst, const fp64_fp64_t *src, int count);
extern void _pami_core_fp64_fp64_pre_minloc(fp64_fp64_t *dst, const fp64_fp64_t *src, int count);
extern void _pami_core_fp64_int32_post_maxloc(fp64_int32_t *dst, const fp64_int32_t *src, int count);
extern void _pami_core_fp64_int32_post_minloc(fp64_int32_t *dst, const fp64_int32_t *src, int count);
extern void _pami_core_fp64_int32_pre_maxloc(fp64_int32_t *dst, const fp64_int32_t *src, int count);
extern void _pami_core_fp64_int32_pre_minloc(fp64_int32_t *dst, const fp64_int32_t *src, int count);
extern void _pami_core_fp64_post_all(double *dst, const double *src, int count);
extern void _pami_core_fp64_post_max(double *dst, const double *src, int count);
extern void _pami_core_fp64_post_min(double *dst, const double *src, int count);
extern void _pami_core_fp64_post_sum(double *dst, const double *src, int count);
extern void _pami_core_fp64_pre_all(double *dst, const double *src, int count);
extern void _pami_core_fp64_pre_max(double *dst, const double *src, int count);
extern void _pami_core_fp64_pre_min(double *dst, const double *src, int count);
extern void _pami_core_fp64_pre_sum(double *dst, const double *src, int count);
extern void _pami_core_int16_int32_post_maxloc(int16_int32_t *dst, const uint16_int32_t *src, int count);
extern void _pami_core_int16_int32_post_minloc(int16_int32_t *dst, const uint16_int32_t *src, int count);
extern void _pami_core_int16_int32_pre_maxloc(uint16_int32_t *dst, const int16_int32_t *src, int count);
extern void _pami_core_int16_int32_pre_minloc(uint16_int32_t *dst, const int16_int32_t *src, int count);
extern void _pami_core_int16_post_all(int16_t *dst, const uint16_t *src, int count);
extern void _pami_core_int16_post_min(int16_t *dst, const uint16_t *src, int count);
extern void _pami_core_int16_pre_all(uint16_t *dst, const int16_t *src, int count);
extern void _pami_core_int16_pre_min(uint16_t *dst, const int16_t *src, int count);
extern void _pami_core_int32_int32_post_maxloc(int32_int32_t *dst, const uint32_int32_t *src, int count);
extern void _pami_core_int32_int32_post_minloc(int32_int32_t *dst, const uint32_int32_t *src, int count);
extern void _pami_core_int32_int32_pre_maxloc(uint32_int32_t *dst, const int32_int32_t *src, int count);
extern void _pami_core_int32_int32_pre_minloc(uint32_int32_t *dst, const int32_int32_t *src, int count);
extern void _pami_core_int32_post_all(int32_t *dst, const uint32_t *src, int count);
extern void _pami_core_int32_post_min(int32_t *dst, const uint32_t *src, int count);
extern void _pami_core_int32_pre_all(uint32_t *dst, const int32_t *src, int count);
extern void _pami_core_int32_pre_min(uint32_t *dst, const int32_t *src, int count);
extern void _pami_core_int64_post_all(int64_t *dst, const uint64_t *src, int count);
extern void _pami_core_int64_post_min(int64_t *dst, const uint64_t *src, int count);
extern void _pami_core_int64_pre_all(uint64_t *dst, const int64_t *src, int count);
extern void _pami_core_int64_pre_min(uint64_t *dst, const int64_t *src, int count);
extern void _pami_core_int8_post_all(int8_t *dst, const uint8_t *src, int count);
extern void _pami_core_int8_post_min(int8_t *dst, const uint8_t *src, int count);
extern void _pami_core_int8_pre_all(uint8_t *dst, const int8_t *src, int count);
extern void _pami_core_int8_pre_min(uint8_t *dst, const int8_t *src, int count);
extern void _pami_core_uint16_post_all(uint16_t *dst, const uint16_t *src, int count);
extern void _pami_core_uint16_post_min(uint16_t *dst, const uint16_t *src, int count);
extern void _pami_core_uint16_pre_all(uint16_t *dst, const uint16_t *src, int count);
extern void _pami_core_uint16_pre_min(uint16_t *dst, const uint16_t *src, int count);
extern void _pami_core_uint32_post_all(uint32_t *dst, const uint32_t *src, int count);
extern void _pami_core_uint32_post_min(uint32_t *dst, const uint32_t *src, int count);
extern void _pami_core_uint32_pre_all(uint32_t *dst, const uint32_t *src, int count);
extern void _pami_core_uint32_pre_min(uint32_t *dst, const uint32_t *src, int count);
extern void _pami_core_uint64_post_all(uint64_t *dst, const uint64_t *src, int count);
extern void _pami_core_uint64_post_min(uint64_t *dst, const uint64_t *src, int count);
extern void _pami_core_uint64_pre_all(uint64_t *dst, const uint64_t *src, int count);
extern void _pami_core_uint64_pre_min(uint64_t *dst, const uint64_t *src, int count);
extern void _pami_core_uint8_post_all(uint8_t *dst, const uint8_t *src, int count);
extern void _pami_core_uint8_post_min(uint8_t *dst, const uint8_t *src, int count);
extern void _pami_core_uint8_pre_all(uint8_t *dst, const uint8_t *src, int count);
extern void _pami_core_uint8_pre_min(uint8_t *dst, const uint8_t *src, int count);

/* These are non-standard prototypes */
extern void _pami_core_fp64_pre1_2pass(uint16_t *dst_e, uint32_t *dst_m, const double *src, int count);
extern void _pami_core_fp64_pre2_2pass(uint32_t *dst_src_m, uint16_t *src_e, uint16_t *src_ee, int count);
extern void _pami_core_fp64_post_2pass(double *dst, uint16_t *src_e, uint32_t *src_m, int count);


/* Optimized math routines */

extern void _pami_core_uint32_not_o(uint32_t *dst, const uint32_t *src, int count);

extern void _pami_core_fp32_fp32_post_maxloc_o(fp32_fp32_t *dst, const fp32_fp32_t *src, int count);
extern void _pami_core_fp32_fp32_post_minloc_o(fp32_fp32_t *dst, const fp32_fp32_t *src, int count);
extern void _pami_core_fp32_fp32_pre_maxloc_o(fp32_fp32_t *dst, const fp32_fp32_t *src, int count);
extern void _pami_core_fp32_fp32_pre_minloc_o(fp32_fp32_t *dst, const fp32_fp32_t *src, int count);
extern void _pami_core_fp32_int32_post_maxloc_o(fp32_int32_t *dst, const fp32_int32_t *src, int count);
extern void _pami_core_fp32_int32_post_minloc_o(fp32_int32_t *dst, const fp32_int32_t *src, int count);
extern void _pami_core_fp32_int32_pre_maxloc_o(fp32_int32_t *dst, const fp32_int32_t *src, int count);
extern void _pami_core_fp32_int32_pre_minloc_o(fp32_int32_t *dst, const fp32_int32_t *src, int count);
extern void _pami_core_fp32_post_max_o(float *dst, const float *src, int count);
extern void _pami_core_fp32_post_min_o(float *dst, const float *src, int count);
extern void _pami_core_fp32_pre_max_o(float *dst, const float *src, int count);
extern void _pami_core_fp32_pre_min_o(float *dst, const float *src, int count);
extern void _pami_core_fp64_fp64_post_maxloc_o(fp64_fp64_t *dst, const fp64_fp64_t *src, int count);
extern void _pami_core_fp64_fp64_post_minloc_o(fp64_fp64_t *dst, const fp64_fp64_t *src, int count);
extern void _pami_core_fp64_fp64_pre_maxloc_o(fp64_fp64_t *dst, const fp64_fp64_t *src, int count);
extern void _pami_core_fp64_fp64_pre_minloc_o(fp64_fp64_t *dst, const fp64_fp64_t *src, int count);
extern void _pami_core_fp64_int32_post_maxloc_o(fp64_int32_t *dst, const fp64_int32_t *src, int count);
extern void _pami_core_fp64_int32_post_minloc_o(fp64_int32_t *dst, const fp64_int32_t *src, int count);
extern void _pami_core_fp64_int32_pre_maxloc_o(fp64_int32_t *dst, const fp64_int32_t *src, int count);
extern void _pami_core_fp64_int32_pre_minloc_o(fp64_int32_t *dst, const fp64_int32_t *src, int count);
extern void _pami_core_fp64_post_max_o(double *dst, const double *src, int count);
extern void _pami_core_fp64_post_min_o(double *dst, const double *src, int count);
extern void _pami_core_fp64_pre_max_o(double *dst, const double *src, int count);
extern void _pami_core_fp64_pre_min_o(double *dst, const double *src, int count);
extern void _pami_core_int16_int32_post_maxloc_o(int16_int32_t *dst, const uint16_int32_t *src, int count);
extern void _pami_core_int16_int32_post_minloc_o(int16_int32_t *dst, const uint16_int32_t *src, int count);
extern void _pami_core_int16_int32_pre_maxloc_o(uint16_int32_t *dst, const int16_int32_t *src, int count);
extern void _pami_core_int16_int32_pre_minloc_o(uint16_int32_t *dst, const int16_int32_t *src, int count);
extern void _pami_core_int16_post_all_o(int16_t *dst, const uint16_t *src, int count);
extern void _pami_core_int16_post_min_o(int16_t *dst, const uint16_t *src, int count);
extern void _pami_core_int16_pre_all_o(uint16_t *dst, const int16_t *src, int count);
extern void _pami_core_int16_pre_min_o(uint16_t *dst, const int16_t *src, int count);
extern void _pami_core_int32_int32_post_maxloc_o(int32_int32_t *dst, const uint32_int32_t *src, int count);
extern void _pami_core_int32_int32_post_minloc_o(int32_int32_t *dst, const uint32_int32_t *src, int count);
extern void _pami_core_int32_int32_pre_maxloc_o(uint32_int32_t *dst, const int32_int32_t *src, int count);
extern void _pami_core_int32_int32_pre_minloc_o(uint32_int32_t *dst, const int32_int32_t *src, int count);
extern void _pami_core_int32_post_all_o(int32_t *dst, const uint32_t *src, int count);
extern void _pami_core_int32_post_min_o(int32_t *dst, const uint32_t *src, int count);
extern void _pami_core_int32_pre_all_o(uint32_t *dst, const int32_t *src, int count);
extern void _pami_core_int32_pre_min_o(uint32_t *dst, const int32_t *src, int count);
extern void _pami_core_int64_post_all_o(int64_t *dst, const uint64_t *src, int count);
extern void _pami_core_int64_post_min_o(int64_t *dst, const uint64_t *src, int count);
extern void _pami_core_int64_pre_all_o(uint64_t *dst, const int64_t *src, int count);
extern void _pami_core_int64_pre_min_o(uint64_t *dst, const int64_t *src, int count);
extern void _pami_core_int8_post_all_o(int8_t *dst, const uint8_t *src, int count);
extern void _pami_core_int8_post_min_o(int8_t *dst, const uint8_t *src, int count);
extern void _pami_core_int8_pre_all_o(uint8_t *dst, const int8_t *src, int count);
extern void _pami_core_int8_pre_min_o(uint8_t *dst, const int8_t *src, int count);
extern void _pami_core_uint16_post_min_o(uint16_t *dst, const uint16_t *src, int count);
extern void _pami_core_uint16_pre_min_o(uint16_t *dst, const uint16_t *src, int count);
extern void _pami_core_uint32_post_min_o(uint32_t *dst, const uint32_t *src, int count);
extern void _pami_core_uint32_pre_min_o(uint32_t *dst, const uint32_t *src, int count);
extern void _pami_core_uint64_post_min_o(uint64_t *dst, const uint64_t *src, int count);
extern void _pami_core_uint64_pre_min_o(uint64_t *dst, const uint64_t *src, int count);
extern void _pami_core_uint8_post_min_o(uint8_t *dst, const uint8_t *src, int count);
extern void _pami_core_uint8_pre_min_o(uint8_t *dst, const uint8_t *src, int count);

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
 * "<oper>" is the operand mnemonic, "dt" is the PAMI datatype, and "op"
 * is the PAMI operand.  Additionally, there must be a prototype
 * declaration for the optimized routine(s). More than one statement
 * may be present in the macro.
 *
 * Don't forget the backslashes at the end of each line.
 *
 * The code in pami_bg_math.h that uses these will
 * be (note: no semicolon after macro):
 *
 * #define OPTIMATH_NSRC(dt,op,n,f) case n: \
 *					f(dst, srcs, nsrc, count);\
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
 * These defines will also be used in pami_dat.c to build a table
 * of optimized routines, by datatype, operand, and number of inputs.
 * (Note, nsrc will always be at least 2)
 *
 * #define OPTIMATH_NSRC(dt,op,n,f)	[dt][op][n-2] = f,
 *
 * void *pami_op_funcs[ndt][nop][nin] = {
 * OPTIMIZED_<type>_<oper>
 * ...
 * };
 *
 * This table is accessed by using the PAMI_OP_FUNCS(dt,op,n) macro
 * (inline).
 */

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
 * "<oper>" is the operand mnemonic, "dt" is the PAMI datatype, and "op"
 * is the PAMI operand.  Additionally, there must be a prototype
 * declaration for the optimized routine. Only one statement
 * may be present in the macro.
 *
 * The code in pami_bg_math.h that uses these will
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
 * These defines will also be used in pami_bg_dat.c to build a table
 * of optimized routines, by datatype, operand, and number of inputs.
 * (Note, nsrc will always be at least 2) For unary routines the third
 * subscript will be either "0" (unoptimized) or "1" (optimized, if present).
 *
 * #define OPTIMATH_UNARY(dt,op,f)	[dt][op][1] = f,
 *
 * void *pami_pre_op_funcs[ndt][nop][2] = {
 * OPTIMIZED_<type>_<oper>
 * ...
 * [dt][op][0] = _pami_core_<type>_<oper>,	// unoptimized
 * };
 *
 * This table is accessed by using the PAMI_PRE_OP_FUNCS(dt,op,n) macro
 * where "n" is a flag indicating whether optimized or unoptimized routine
 * is to be selected.
 *
 * Similar tables/functions exist for POST and MARSHALL routines.
 *
 * Note, many of these routines do not relate to PAMI_Op operations and thus
 * cannot be selected by "op". These defines will work in a Core_<type>_<oper>
 * function but not in a table/selector function. Look for PAMI_OP_COUNT.
 * At present, it seems none of the functions are actually used.
 */
#define OPTIMIZED_int8_pre_all(op)
#define OPTIMIZED_int8_post_all(op)
#define OPTIMIZED_int8_pre_min	\
        OPTIMATH_UNARY(PAMI_SIGNED_CHAR,PAMI_MIN,_pami_core_int8_pre_min_o)
#define OPTIMIZED_int8_post_min	\
        OPTIMATH_UNARY(PAMI_SIGNED_CHAR,PAMI_MIN,_pami_core_int8_post_min_o)
#define OPTIMIZED_uint8_pre_all(op)
#define OPTIMIZED_uint8_post_all(op)
#define OPTIMIZED_uint8_pre_min	\
        OPTIMATH_UNARY(PAMI_UNSIGNED_CHAR,PAMI_MIN,_pami_core_uint8_pre_min_o)
#define OPTIMIZED_uint8_post_min	\
        OPTIMATH_UNARY(PAMI_UNSIGNED_CHAR,PAMI_MIN,_pami_core_uint8_post_min_o)
#define OPTIMIZED_int16_pre_all(op)	\
        OPTIMATH_UNARY(PAMI_SIGNED_SHORT,op,_pami_core_int16_pre_all_o)
#define OPTIMIZED_int16_post_all(op)	\
        OPTIMATH_UNARY(PAMI_SIGNED_SHORT,op,_pami_core_int16_post_all_o)
#define OPTIMIZED_int16_pre_min		\
        OPTIMATH_UNARY(PAMI_SIGNED_SHORT,PAMI_MIN,_pami_core_int16_pre_min_o)
#define OPTIMIZED_int16_post_min	\
        OPTIMATH_UNARY(PAMI_SIGNED_SHORT,PAMI_MIN,_pami_core_int16_post_min_o)
#define OPTIMIZED_int16_int32_pre_maxloc
#define OPTIMIZED_int16_int32_post_maxloc
#define OPTIMIZED_int16_int32_pre_minloc
#define OPTIMIZED_int16_int32_post_minloc
#define OPTIMIZED_uint16_pre_all(op)
#define OPTIMIZED_uint16_post_all(op)
#define OPTIMIZED_uint16_pre_min	\
        OPTIMATH_UNARY(PAMI_UNSIGNED_SHORT,PAMI_MIN,_pami_core_uint16_pre_min_o)
#define OPTIMIZED_uint16_post_min	\
        OPTIMATH_UNARY(PAMI_UNSIGNED_SHORT,PAMI_MIN,_pami_core_uint16_post_min_o)
#define OPTIMIZED_int32_pre_all(op)
#define OPTIMIZED_int32_post_all(op)
#define OPTIMIZED_int32_pre_min
#define OPTIMIZED_int32_post_min	\
        OPTIMATH_UNARY(PAMI_SIGNED_INT,PAMI_MIN,_pami_core_int32_post_min_o)
#define OPTIMIZED_int32_int32_pre_maxloc
#define OPTIMIZED_int32_int32_post_maxloc
#define OPTIMIZED_int32_int32_pre_minloc
#define OPTIMIZED_int32_int32_post_minloc
#define OPTIMIZED_uint32_pre_all(op)
#define OPTIMIZED_uint32_post_all(op)
#define OPTIMIZED_uint32_pre_min	\
        OPTIMATH_UNARY(PAMI_UNSIGNED_INT,PAMI_MIN,_pami_core_uint32_pre_min_o)
#define OPTIMIZED_uint32_post_min	\
        OPTIMATH_UNARY(PAMI_UNSIGNED_INT,PAMI_MIN,_pami_core_uint32_post_min_o)
#define OPTIMIZED_int64_pre_all(op)
#define OPTIMIZED_int64_post_all(op)
#define OPTIMIZED_int64_pre_min
#define OPTIMIZED_int64_post_min
#define OPTIMIZED_uint64_pre_all(op)
#define OPTIMIZED_uint64_post_all(op)
#define OPTIMIZED_uint64_pre_min	\
        OPTIMATH_UNARY(PAMI_UNSIGNED_LONG_LONG,PAMI_MIN,_pami_core_uint64_pre_min_o)
#define OPTIMIZED_uint64_post_min	\
        OPTIMATH_UNARY(PAMI_UNSIGNED_LONG_LONG,PAMI_MIN,_pami_core_uint64_post_min_o)
#define OPTIMIZED_fp32_pre_max
#define OPTIMIZED_fp32_post_max
#define OPTIMIZED_fp32_int32_pre_maxloc
#define OPTIMIZED_fp32_int32_post_maxloc
#define OPTIMIZED_fp32_fp32_pre_maxloc
#define OPTIMIZED_fp32_fp32_post_maxloc
#define OPTIMIZED_fp32_pre_min
#define OPTIMIZED_fp32_post_min
#define OPTIMIZED_fp32_int32_pre_minloc
#define OPTIMIZED_fp32_int32_post_minloc
#define OPTIMIZED_fp32_fp32_pre_minloc
#define OPTIMIZED_fp32_fp32_post_minloc
#define OPTIMIZED_fp64_pre_sum
#define OPTIMIZED_fp64_post_sum
#define OPTIMIZED_fp64_pre_max
#define OPTIMIZED_fp64_post_max
#define OPTIMIZED_fp64_pre_all(op)
#define OPTIMIZED_fp64_post_all(op)
#define OPTIMIZED_fp64_int32_pre_maxloc
#define OPTIMIZED_fp64_int32_post_maxloc
#define OPTIMIZED_fp64_fp64_pre_maxloc
#define OPTIMIZED_fp64_fp64_post_maxloc
#define OPTIMIZED_fp64_pre_min
#define OPTIMIZED_fp64_post_min
#define OPTIMIZED_fp64_int32_pre_minloc
#define OPTIMIZED_fp64_int32_post_minloc
#define OPTIMIZED_fp64_fp64_pre_minloc
#define OPTIMIZED_fp64_fp64_post_minloc

#endif /* _pami_optibgmath_h_ */
