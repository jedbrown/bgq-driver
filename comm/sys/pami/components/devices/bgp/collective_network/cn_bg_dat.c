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
 * \file components/devices/bgp/collective_network/cn_bg_dat.c
 * \brief Tables used by PAMI
 */

#include <pami.h>
#include "math/bgp/collective_network/pami_bg_math.h"

#ifndef PAMI_NO_OPTIMATH
#define OPTIMATH_NSRC(dt,op,n,f)	[op][dt][n-1] = f,
#define OPTIMATH_UNARY(dt,op,f)		[op][dt][1] = f,
#else /* PAMI_NO_OPTIMATH */
#define OPTIMATH_NSRC(dt,op,n,f)
#define OPTIMATH_UNARY(dt,op,f)
#endif /* PAMI_NO_OPTIMATH */

/**
 * \brief Translate operation, datatype, and optimization to math function
 *
 * This table should not be accessed directly. The inline/macro
 * PAMI_PRE_OP_FUNCS(dt,op,n) should be used instead. That returns
 * the proper function (pointer) for the given combination of
 * datatype "dt", operand "op", and optimization level "n", taking
 * into account conbinations that are not optimized.
 */
void *pami_pre_op_funcs[PAMI_OP_COUNT][PAMI_DT_COUNT][2] = {

OPTIMIZED_int16_pre_all(PAMI_MAX)
OPTIMIZED_int32_pre_all(PAMI_MAX)
OPTIMIZED_int64_pre_all(PAMI_MAX)
OPTIMIZED_uint64_pre_all(PAMI_MAX)
OPTIMIZED_fp32_pre_max
OPTIMIZED_fp64_pre_max

OPTIMIZED_int16_pre_min
OPTIMIZED_uint16_pre_min
OPTIMIZED_int32_pre_min
OPTIMIZED_uint32_pre_min
OPTIMIZED_int64_pre_min
OPTIMIZED_uint64_pre_min
OPTIMIZED_fp32_pre_min
OPTIMIZED_fp64_pre_min

OPTIMIZED_fp64_pre_sum

OPTIMIZED_int32_int32_pre_maxloc
OPTIMIZED_int16_int32_pre_maxloc
OPTIMIZED_fp32_int32_pre_maxloc
OPTIMIZED_fp64_int32_pre_maxloc
OPTIMIZED_fp32_fp32_pre_maxloc
OPTIMIZED_fp64_fp64_pre_maxloc

OPTIMIZED_int32_int32_pre_minloc
OPTIMIZED_int16_int32_pre_minloc
OPTIMIZED_fp32_int32_pre_minloc
OPTIMIZED_fp64_int32_pre_minloc
OPTIMIZED_fp32_fp32_pre_minloc
OPTIMIZED_fp64_fp64_pre_minloc

/*
 * Default, generic routines are "n == 0".
 * If an entry does not exist here, that combination will be silently
 * passed to the tree. Use "(void *)PAMI_UNIMPL" for unsupported operations/types.
 */
[PAMI_MAX][PAMI_SIGNED_CHAR][0] =	(void *)PAMI_UNIMPL,
[PAMI_MAX][PAMI_UNSIGNED_CHAR][0] =	(void *)PAMI_UNIMPL,
[PAMI_MAX][PAMI_SIGNED_SHORT][0] =	_pami_core_int16_pre_all,
[PAMI_MAX][PAMI_UNSIGNED_SHORT][0] =	NULL,
[PAMI_MAX][PAMI_SIGNED_INT][0] =	_pami_core_int32_pre_all,
[PAMI_MAX][PAMI_UNSIGNED_INT][0] =	NULL,
[PAMI_MAX][PAMI_SIGNED_LONG_LONG][0] =	_pami_core_int64_pre_all,
[PAMI_MAX][PAMI_UNSIGNED_LONG_LONG][0] =_pami_core_uint64_pre_all,
[PAMI_MAX][PAMI_FLOAT][0] =		_pami_core_fp32_pre_max,
[PAMI_MAX][PAMI_DOUBLE][0] =		_pami_core_fp64_pre_max,
[PAMI_MAX][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_MAX][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_MAX][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_MAX][PAMI_LOGICAL][0] =		(void *)PAMI_UNIMPL,

[PAMI_MIN][PAMI_SIGNED_CHAR][0] =	(void *)PAMI_UNIMPL,
[PAMI_MIN][PAMI_UNSIGNED_CHAR][0] =	(void *)PAMI_UNIMPL,
[PAMI_MIN][PAMI_SIGNED_SHORT][0] =	_pami_core_int16_pre_min,
[PAMI_MIN][PAMI_UNSIGNED_SHORT][0] =	_pami_core_uint16_pre_min,
[PAMI_MIN][PAMI_SIGNED_INT][0] =	_pami_core_int32_pre_min,
[PAMI_MIN][PAMI_UNSIGNED_INT][0] =	_pami_core_uint32_pre_min,
[PAMI_MIN][PAMI_SIGNED_LONG_LONG][0] =	_pami_core_int64_pre_min,
[PAMI_MIN][PAMI_UNSIGNED_LONG_LONG][0] =_pami_core_uint64_pre_min,
[PAMI_MIN][PAMI_FLOAT][0] =		_pami_core_fp32_pre_min,
[PAMI_MIN][PAMI_DOUBLE][0] =		_pami_core_fp64_pre_min,
[PAMI_MIN][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_MIN][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_MIN][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_MIN][PAMI_LOGICAL][0] =		(void *)PAMI_UNIMPL,

[PAMI_SUM][PAMI_SIGNED_CHAR][0] =	(void *)PAMI_UNIMPL,
[PAMI_SUM][PAMI_UNSIGNED_CHAR][0] =	(void *)PAMI_UNIMPL,
[PAMI_SUM][PAMI_SIGNED_SHORT][0] =	NULL,
[PAMI_SUM][PAMI_UNSIGNED_SHORT][0] =	NULL,
[PAMI_SUM][PAMI_SIGNED_INT][0] =	NULL,
[PAMI_SUM][PAMI_UNSIGNED_INT][0] =	NULL,
[PAMI_SUM][PAMI_SIGNED_LONG_LONG][0] =	NULL,
[PAMI_SUM][PAMI_UNSIGNED_LONG_LONG][0] =NULL,
[PAMI_SUM][PAMI_FLOAT][0] =		(void *)PAMI_UNIMPL,
[PAMI_SUM][PAMI_DOUBLE][0] =		_pami_core_fp64_pre_sum,
[PAMI_SUM][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_SUM][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_SUM][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_SUM][PAMI_LOGICAL][0] =		(void *)PAMI_UNIMPL,

[PAMI_PROD][PAMI_SIGNED_CHAR][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_UNSIGNED_CHAR][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_SIGNED_SHORT][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_UNSIGNED_SHORT][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_SIGNED_INT][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_UNSIGNED_INT][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_SIGNED_LONG_LONG][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_UNSIGNED_LONG_LONG][0] =(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_FLOAT][0] =		(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_DOUBLE][0] =		(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_LOGICAL][0] =		(void *)PAMI_UNIMPL,

[PAMI_LAND][PAMI_SIGNED_CHAR][0] =	NULL,
[PAMI_LAND][PAMI_UNSIGNED_CHAR][0] =	NULL,
[PAMI_LAND][PAMI_SIGNED_SHORT][0] =	NULL,
[PAMI_LAND][PAMI_UNSIGNED_SHORT][0] =	NULL,
[PAMI_LAND][PAMI_SIGNED_INT][0] =	NULL,
[PAMI_LAND][PAMI_UNSIGNED_INT][0] =	NULL,
[PAMI_LAND][PAMI_SIGNED_LONG_LONG][0] =	NULL,
[PAMI_LAND][PAMI_UNSIGNED_LONG_LONG][0] =NULL,
[PAMI_LAND][PAMI_FLOAT][0] =		NULL,
[PAMI_LAND][PAMI_DOUBLE][0] =		NULL,
[PAMI_LAND][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_LAND][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_LAND][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_LAND][PAMI_LOGICAL][0] =		NULL,

[PAMI_LOR][PAMI_SIGNED_CHAR][0] =	NULL,
[PAMI_LOR][PAMI_UNSIGNED_CHAR][0] =	NULL,
[PAMI_LOR][PAMI_SIGNED_SHORT][0] =	NULL,
[PAMI_LOR][PAMI_UNSIGNED_SHORT][0] =	NULL,
[PAMI_LOR][PAMI_SIGNED_INT][0] =	NULL,
[PAMI_LOR][PAMI_UNSIGNED_INT][0] =	NULL,
[PAMI_LOR][PAMI_SIGNED_LONG_LONG][0] =	NULL,
[PAMI_LOR][PAMI_UNSIGNED_LONG_LONG][0] =NULL,
[PAMI_LOR][PAMI_FLOAT][0] =		NULL,
[PAMI_LOR][PAMI_DOUBLE][0] =		NULL,
[PAMI_LOR][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_LOR][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_LOR][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_LOR][PAMI_LOGICAL][0] =		NULL,

[PAMI_LXOR][PAMI_SIGNED_CHAR][0] =	NULL,
[PAMI_LXOR][PAMI_UNSIGNED_CHAR][0] =	NULL,
[PAMI_LXOR][PAMI_SIGNED_SHORT][0] =	NULL,
[PAMI_LXOR][PAMI_UNSIGNED_SHORT][0] =	NULL,
[PAMI_LXOR][PAMI_SIGNED_INT][0] =	NULL,
[PAMI_LXOR][PAMI_UNSIGNED_INT][0] =	NULL,
[PAMI_LXOR][PAMI_SIGNED_LONG_LONG][0] =	NULL,
[PAMI_LXOR][PAMI_UNSIGNED_LONG_LONG][0] =NULL,
[PAMI_LXOR][PAMI_FLOAT][0] =		NULL,
[PAMI_LXOR][PAMI_DOUBLE][0] =		NULL,
[PAMI_LXOR][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_LXOR][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_LXOR][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_LXOR][PAMI_LOGICAL][0] =		NULL,

[PAMI_BAND][PAMI_SIGNED_CHAR][0] =	NULL,
[PAMI_BAND][PAMI_UNSIGNED_CHAR][0] =	NULL,
[PAMI_BAND][PAMI_SIGNED_SHORT][0] =	NULL,
[PAMI_BAND][PAMI_UNSIGNED_SHORT][0] =	NULL,
[PAMI_BAND][PAMI_SIGNED_INT][0] =	NULL,
[PAMI_BAND][PAMI_UNSIGNED_INT][0] =	NULL,
[PAMI_BAND][PAMI_SIGNED_LONG_LONG][0] =	NULL,
[PAMI_BAND][PAMI_UNSIGNED_LONG_LONG][0] =NULL,
[PAMI_BAND][PAMI_FLOAT][0] =		NULL,
[PAMI_BAND][PAMI_DOUBLE][0] =		NULL,
[PAMI_BAND][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_BAND][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_BAND][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_BAND][PAMI_LOGICAL][0] =		NULL,

[PAMI_BOR][PAMI_SIGNED_CHAR][0] =	NULL,
[PAMI_BOR][PAMI_UNSIGNED_CHAR][0] =	NULL,
[PAMI_BOR][PAMI_SIGNED_SHORT][0] =	NULL,
[PAMI_BOR][PAMI_UNSIGNED_SHORT][0] =	NULL,
[PAMI_BOR][PAMI_SIGNED_INT][0] =	NULL,
[PAMI_BOR][PAMI_UNSIGNED_INT][0] =	NULL,
[PAMI_BOR][PAMI_SIGNED_LONG_LONG][0] =	NULL,
[PAMI_BOR][PAMI_UNSIGNED_LONG_LONG][0] =NULL,
[PAMI_BOR][PAMI_FLOAT][0] =		NULL,
[PAMI_BOR][PAMI_DOUBLE][0] =		NULL,
[PAMI_BOR][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_BOR][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_BOR][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_BOR][PAMI_LOGICAL][0] =		NULL,

[PAMI_BXOR][PAMI_SIGNED_CHAR][0] =	NULL,
[PAMI_BXOR][PAMI_UNSIGNED_CHAR][0] =	NULL,
[PAMI_BXOR][PAMI_SIGNED_SHORT][0] =	NULL,
[PAMI_BXOR][PAMI_UNSIGNED_SHORT][0] =	NULL,
[PAMI_BXOR][PAMI_SIGNED_INT][0] =	NULL,
[PAMI_BXOR][PAMI_UNSIGNED_INT][0] =	NULL,
[PAMI_BXOR][PAMI_SIGNED_LONG_LONG][0] =	NULL,
[PAMI_BXOR][PAMI_UNSIGNED_LONG_LONG][0] =NULL,
[PAMI_BXOR][PAMI_FLOAT][0] =		NULL,
[PAMI_BXOR][PAMI_DOUBLE][0] =		NULL,
[PAMI_BXOR][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_BXOR][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_BXOR][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_BXOR][PAMI_LOGICAL][0] =		NULL,

[PAMI_MAXLOC][PAMI_LOC_2INT][0] =	_pami_core_int32_int32_pre_maxloc,
[PAMI_MAXLOC][PAMI_LOC_SHORT_INT][0] =	_pami_core_int16_int32_pre_maxloc,
[PAMI_MAXLOC][PAMI_LOC_FLOAT_INT][0] =	_pami_core_fp32_int32_pre_maxloc,
[PAMI_MAXLOC][PAMI_LOC_DOUBLE_INT][0] =	_pami_core_fp64_int32_pre_maxloc,
[PAMI_MAXLOC][PAMI_LOC_2FLOAT][0] =	_pami_core_fp32_fp32_pre_maxloc,
[PAMI_MAXLOC][PAMI_LOC_2DOUBLE][0] =	_pami_core_fp64_fp64_pre_maxloc,

[PAMI_MINLOC][PAMI_LOC_2INT][0] =	_pami_core_int32_int32_pre_minloc,
[PAMI_MINLOC][PAMI_LOC_SHORT_INT][0] =	_pami_core_int16_int32_pre_minloc,
[PAMI_MINLOC][PAMI_LOC_FLOAT_INT][0] =	_pami_core_fp32_int32_pre_minloc,
[PAMI_MINLOC][PAMI_LOC_DOUBLE_INT][0] =	_pami_core_fp64_int32_pre_minloc,
[PAMI_MINLOC][PAMI_LOC_2FLOAT][0] =	_pami_core_fp32_fp32_pre_minloc,
[PAMI_MINLOC][PAMI_LOC_2DOUBLE][0] =	_pami_core_fp64_fp64_pre_minloc,

};

/**
 * \brief Translate operation, datatype, and optimization to math function
 *
 * This table should not be accessed directly. The inline/macro
 * PAMI_POST_OP_FUNCS(dt,op,n) should be used instead. That returns
 * the proper function (pointer) for the given combination of
 * datatype "dt", operand "op", and optimization level "n", taking
 * into account conbinations that are not optimized.
 */
void *pami_post_op_funcs[PAMI_OP_COUNT][PAMI_DT_COUNT][2] = {

OPTIMIZED_int16_post_all(PAMI_MAX)
OPTIMIZED_int32_post_all(PAMI_MAX)
OPTIMIZED_int64_post_all(PAMI_MAX)
OPTIMIZED_uint64_post_all(PAMI_MAX)
OPTIMIZED_fp32_post_max
OPTIMIZED_fp64_post_max

OPTIMIZED_int16_post_min
OPTIMIZED_uint16_post_min
OPTIMIZED_int32_post_min
OPTIMIZED_uint32_post_min
OPTIMIZED_int64_post_min
OPTIMIZED_uint64_post_min
OPTIMIZED_fp32_post_min
OPTIMIZED_fp64_post_min

OPTIMIZED_fp64_post_sum

OPTIMIZED_int32_int32_post_maxloc
OPTIMIZED_int16_int32_post_maxloc
OPTIMIZED_fp32_int32_post_maxloc
OPTIMIZED_fp64_int32_post_maxloc
OPTIMIZED_fp32_fp32_post_maxloc
OPTIMIZED_fp64_fp64_post_maxloc

OPTIMIZED_int32_int32_post_minloc
OPTIMIZED_int16_int32_post_minloc
OPTIMIZED_fp32_int32_post_minloc
OPTIMIZED_fp64_int32_post_minloc
OPTIMIZED_fp32_fp32_post_minloc
OPTIMIZED_fp64_fp64_post_minloc

/*
 * Default, generic routines are "n == 0".
 * If an entry does not exist here, that combination will be silently
 * passed to the tree. Use "(void *)PAMI_UNIMPL" for unsupported operations/types.
 */
[PAMI_MAX][PAMI_SIGNED_CHAR][0] =	(void *)PAMI_UNIMPL,
[PAMI_MAX][PAMI_UNSIGNED_CHAR][0] =	(void *)PAMI_UNIMPL,
[PAMI_MAX][PAMI_SIGNED_SHORT][0] =	_pami_core_int16_post_all,
[PAMI_MAX][PAMI_UNSIGNED_SHORT][0] =	NULL,
[PAMI_MAX][PAMI_SIGNED_INT][0] =	_pami_core_int32_post_all,
[PAMI_MAX][PAMI_UNSIGNED_INT][0] =	NULL,
[PAMI_MAX][PAMI_SIGNED_LONG_LONG][0] =	_pami_core_int64_post_all,
[PAMI_MAX][PAMI_UNSIGNED_LONG_LONG][0] =_pami_core_uint64_post_all,
[PAMI_MAX][PAMI_FLOAT][0] =		_pami_core_fp32_post_max,
[PAMI_MAX][PAMI_DOUBLE][0] =		_pami_core_fp64_post_max,
[PAMI_MAX][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_MAX][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_MAX][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_MAX][PAMI_LOGICAL][0] =		(void *)PAMI_UNIMPL,

[PAMI_MIN][PAMI_SIGNED_CHAR][0] =	(void *)PAMI_UNIMPL,
[PAMI_MIN][PAMI_UNSIGNED_CHAR][0] =	(void *)PAMI_UNIMPL,
[PAMI_MIN][PAMI_SIGNED_SHORT][0] =	_pami_core_int16_post_min,
[PAMI_MIN][PAMI_UNSIGNED_SHORT][0] =	_pami_core_uint16_post_min,
[PAMI_MIN][PAMI_SIGNED_INT][0] =	_pami_core_int32_post_min,
[PAMI_MIN][PAMI_UNSIGNED_INT][0] =	_pami_core_uint32_post_min,
[PAMI_MIN][PAMI_SIGNED_LONG_LONG][0] =	_pami_core_int64_post_min,
[PAMI_MIN][PAMI_UNSIGNED_LONG_LONG][0] =_pami_core_uint64_post_min,
[PAMI_MIN][PAMI_FLOAT][0] =		_pami_core_fp32_post_min,
[PAMI_MIN][PAMI_DOUBLE][0] =		_pami_core_fp64_post_min,
[PAMI_MIN][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_MIN][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_MIN][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_MIN][PAMI_LOGICAL][0] =		(void *)PAMI_UNIMPL,

[PAMI_SUM][PAMI_SIGNED_CHAR][0] =	(void *)PAMI_UNIMPL,
[PAMI_SUM][PAMI_UNSIGNED_CHAR][0] =	(void *)PAMI_UNIMPL,
[PAMI_SUM][PAMI_SIGNED_SHORT][0] =	NULL,
[PAMI_SUM][PAMI_UNSIGNED_SHORT][0] =	NULL,
[PAMI_SUM][PAMI_SIGNED_INT][0] =	NULL,
[PAMI_SUM][PAMI_UNSIGNED_INT][0] =	NULL,
[PAMI_SUM][PAMI_SIGNED_LONG_LONG][0] =	NULL,
[PAMI_SUM][PAMI_UNSIGNED_LONG_LONG][0] =NULL,
[PAMI_SUM][PAMI_FLOAT][0] =		(void *)PAMI_UNIMPL,
[PAMI_SUM][PAMI_DOUBLE][0] =		_pami_core_fp64_post_sum,
[PAMI_SUM][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_SUM][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_SUM][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_SUM][PAMI_LOGICAL][0] =		(void *)PAMI_UNIMPL,

[PAMI_PROD][PAMI_SIGNED_CHAR][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_UNSIGNED_CHAR][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_SIGNED_SHORT][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_UNSIGNED_SHORT][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_SIGNED_INT][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_UNSIGNED_INT][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_SIGNED_LONG_LONG][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_UNSIGNED_LONG_LONG][0] =(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_FLOAT][0] =		(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_DOUBLE][0] =		(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_PROD][PAMI_LOGICAL][0] =		(void *)PAMI_UNIMPL,

[PAMI_LAND][PAMI_SIGNED_CHAR][0] =	NULL,
[PAMI_LAND][PAMI_UNSIGNED_CHAR][0] =	NULL,
[PAMI_LAND][PAMI_SIGNED_SHORT][0] =	NULL,
[PAMI_LAND][PAMI_UNSIGNED_SHORT][0] =	NULL,
[PAMI_LAND][PAMI_SIGNED_INT][0] =	NULL,
[PAMI_LAND][PAMI_UNSIGNED_INT][0] =	NULL,
[PAMI_LAND][PAMI_SIGNED_LONG_LONG][0] =	NULL,
[PAMI_LAND][PAMI_UNSIGNED_LONG_LONG][0] =NULL,
[PAMI_LAND][PAMI_FLOAT][0] =		NULL,
[PAMI_LAND][PAMI_DOUBLE][0] =		NULL,
[PAMI_LAND][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_LAND][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_LAND][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_LAND][PAMI_LOGICAL][0] =		NULL,

[PAMI_LOR][PAMI_SIGNED_CHAR][0] =	NULL,
[PAMI_LOR][PAMI_UNSIGNED_CHAR][0] =	NULL,
[PAMI_LOR][PAMI_SIGNED_SHORT][0] =	NULL,
[PAMI_LOR][PAMI_UNSIGNED_SHORT][0] =	NULL,
[PAMI_LOR][PAMI_SIGNED_INT][0] =	NULL,
[PAMI_LOR][PAMI_UNSIGNED_INT][0] =	NULL,
[PAMI_LOR][PAMI_SIGNED_LONG_LONG][0] =	NULL,
[PAMI_LOR][PAMI_UNSIGNED_LONG_LONG][0] =NULL,
[PAMI_LOR][PAMI_FLOAT][0] =		NULL,
[PAMI_LOR][PAMI_DOUBLE][0] =		NULL,
[PAMI_LOR][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_LOR][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_LOR][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_LOR][PAMI_LOGICAL][0] =		NULL,

[PAMI_LXOR][PAMI_SIGNED_CHAR][0] =	NULL,
[PAMI_LXOR][PAMI_UNSIGNED_CHAR][0] =	NULL,
[PAMI_LXOR][PAMI_SIGNED_SHORT][0] =	NULL,
[PAMI_LXOR][PAMI_UNSIGNED_SHORT][0] =	NULL,
[PAMI_LXOR][PAMI_SIGNED_INT][0] =	NULL,
[PAMI_LXOR][PAMI_UNSIGNED_INT][0] =	NULL,
[PAMI_LXOR][PAMI_SIGNED_LONG_LONG][0] =	NULL,
[PAMI_LXOR][PAMI_UNSIGNED_LONG_LONG][0] =NULL,
[PAMI_LXOR][PAMI_FLOAT][0] =		NULL,
[PAMI_LXOR][PAMI_DOUBLE][0] =		NULL,
[PAMI_LXOR][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_LXOR][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_LXOR][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_LXOR][PAMI_LOGICAL][0] =		NULL,

[PAMI_BAND][PAMI_SIGNED_CHAR][0] =	NULL,
[PAMI_BAND][PAMI_UNSIGNED_CHAR][0] =	NULL,
[PAMI_BAND][PAMI_SIGNED_SHORT][0] =	NULL,
[PAMI_BAND][PAMI_UNSIGNED_SHORT][0] =	NULL,
[PAMI_BAND][PAMI_SIGNED_INT][0] =	NULL,
[PAMI_BAND][PAMI_UNSIGNED_INT][0] =	NULL,
[PAMI_BAND][PAMI_SIGNED_LONG_LONG][0] =	NULL,
[PAMI_BAND][PAMI_UNSIGNED_LONG_LONG][0] =NULL,
[PAMI_BAND][PAMI_FLOAT][0] =		NULL,
[PAMI_BAND][PAMI_DOUBLE][0] =		NULL,
[PAMI_BAND][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_BAND][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_BAND][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_BAND][PAMI_LOGICAL][0] =		NULL,

[PAMI_BOR][PAMI_SIGNED_CHAR][0] =	NULL,
[PAMI_BOR][PAMI_UNSIGNED_CHAR][0] =	NULL,
[PAMI_BOR][PAMI_SIGNED_SHORT][0] =	NULL,
[PAMI_BOR][PAMI_UNSIGNED_SHORT][0] =	NULL,
[PAMI_BOR][PAMI_SIGNED_INT][0] =	NULL,
[PAMI_BOR][PAMI_UNSIGNED_INT][0] =	NULL,
[PAMI_BOR][PAMI_SIGNED_LONG_LONG][0] =	NULL,
[PAMI_BOR][PAMI_UNSIGNED_LONG_LONG][0] =NULL,
[PAMI_BOR][PAMI_FLOAT][0] =		NULL,
[PAMI_BOR][PAMI_DOUBLE][0] =		NULL,
[PAMI_BOR][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_BOR][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_BOR][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_BOR][PAMI_LOGICAL][0] =		NULL,

[PAMI_BXOR][PAMI_SIGNED_CHAR][0] =	NULL,
[PAMI_BXOR][PAMI_UNSIGNED_CHAR][0] =	NULL,
[PAMI_BXOR][PAMI_SIGNED_SHORT][0] =	NULL,
[PAMI_BXOR][PAMI_UNSIGNED_SHORT][0] =	NULL,
[PAMI_BXOR][PAMI_SIGNED_INT][0] =	NULL,
[PAMI_BXOR][PAMI_UNSIGNED_INT][0] =	NULL,
[PAMI_BXOR][PAMI_SIGNED_LONG_LONG][0] =	NULL,
[PAMI_BXOR][PAMI_UNSIGNED_LONG_LONG][0] =NULL,
[PAMI_BXOR][PAMI_FLOAT][0] =		NULL,
[PAMI_BXOR][PAMI_DOUBLE][0] =		NULL,
[PAMI_BXOR][PAMI_LONG_DOUBLE][0] =	(void *)PAMI_UNIMPL,
[PAMI_BXOR][PAMI_SINGLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_BXOR][PAMI_DOUBLE_COMPLEX][0] =	(void *)PAMI_UNIMPL,
[PAMI_BXOR][PAMI_LOGICAL][0] =		NULL,

[PAMI_MAXLOC][PAMI_LOC_2INT][0] =	_pami_core_int32_int32_post_maxloc,
[PAMI_MAXLOC][PAMI_LOC_SHORT_INT][0] =	_pami_core_int16_int32_post_maxloc,
[PAMI_MAXLOC][PAMI_LOC_FLOAT_INT][0] =	_pami_core_fp32_int32_post_maxloc,
[PAMI_MAXLOC][PAMI_LOC_DOUBLE_INT][0] =	_pami_core_fp64_int32_post_maxloc,
[PAMI_MAXLOC][PAMI_LOC_2FLOAT][0] =	_pami_core_fp32_fp32_post_maxloc,
[PAMI_MAXLOC][PAMI_LOC_2DOUBLE][0] =	_pami_core_fp64_fp64_post_maxloc,

[PAMI_MINLOC][PAMI_LOC_2INT][0] =	_pami_core_int32_int32_post_minloc,
[PAMI_MINLOC][PAMI_LOC_SHORT_INT][0] =	_pami_core_int16_int32_post_minloc,
[PAMI_MINLOC][PAMI_LOC_FLOAT_INT][0] =	_pami_core_fp32_int32_post_minloc,
[PAMI_MINLOC][PAMI_LOC_DOUBLE_INT][0] =	_pami_core_fp64_int32_post_minloc,
[PAMI_MINLOC][PAMI_LOC_2FLOAT][0] =	_pami_core_fp32_fp32_post_minloc,
[PAMI_MINLOC][PAMI_LOC_2DOUBLE][0] =	_pami_core_fp64_fp64_post_minloc,

};

#undef OPTIMATH_NSRC
#undef OPTIMATH_UNARY
