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
 * \file math/ppc450d/fp128_o.cc
 * \brief Optimized math routines for 128 bit floating point operations on
 *        the ppc 450 dual fpu architecture.
 */
#include "math_coremath.h"
#include "FloatUtil.h"
#include "Util.h"
#include "ppc450d/internal_o.h"

void _pami_core_fp128_max2(long double *dst, const long double **srcs, int nsrc, int count) {
#define OP(a,b) (((a)>(b))?(a):(b))

#define TYPE long double
#include "../_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_fp128_min2(long double *dst, const long double **srcs, int nsrc, int count) {
#define OP(a,b) (((a)>(b))?(b):(a))

#define TYPE long double
#include "../_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_fp128_prod2(long double *dst, const long double **srcs, int nsrc, int count) {
#define OP(a,b) ((a)*(b))

#define TYPE long double
#include "../_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_fp128_sum2(long double *dst, const long double **srcs, int nsrc, int count) {
#define OP(a,b) ((a)+(b))

#define TYPE long double
#include "../_dual_src.x.h"
#undef TYPE
#undef OP
}
