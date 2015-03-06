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
 * \file math/fp32.cc
 * \brief Default C math routines for 32 bit floating point operations.
 */

#include "math_coremath.h"
#include "FloatUtil.h"
#include "internal.h"

void _pami_core_fp32_max(float *dst, const float **srcs, int nsrc, int count) {
#define OP(a,b) (((a)>(b))?(a):(b))

#define TYPE float
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_fp32_min(float *dst, const float **srcs, int nsrc, int count) {
#define OP(a,b) (((a)>(b))?(b):(a))

#define TYPE float
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_fp32_prod(float *dst, const float **srcs, int nsrc, int count) {
#define OP(a,b) ((a)*(b))

#define TYPE float
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_fp32_sum(float *dst, const float **srcs, int nsrc, int count) {
#define OP(a,b) ((a)+(b))

#define TYPE float
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_fp32_land(float *dst, const float **srcs, int nsrc, int count) {
#define OP(a,b) ((a)&&(b))

#define TYPE float
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_fp32_lor(float *dst, const float **srcs, int nsrc, int count) {
#define OP(a,b) ((a)||(b))

#define TYPE float
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_fp32_lxor(float *dst, const float **srcs, int nsrc, int count) {
#define OP(a,b) (((a)&&!(b))||(!(a)&&(b)))

#define TYPE float
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_fp32_band(float *dst, const float **srcs, int nsrc, int count) {
	_pami_core_uint32_band((uint32_t *)dst, (const uint32_t **)srcs, nsrc, count);
}

void _pami_core_fp32_bor(float *dst, const float **srcs, int nsrc, int count) {
	_pami_core_uint32_bor((uint32_t *)dst, (const uint32_t **)srcs, nsrc, count);
}

void _pami_core_fp32_bxor(float *dst, const float **srcs, int nsrc, int count) {
	_pami_core_uint32_bxor((uint32_t *)dst, (const uint32_t **)srcs, nsrc, count);
}

void _pami_core_fp32_int32_maxloc(fp32_int32_t *dst, const fp32_int32_t **srcs, int nsrc, int count) {
        register int n = 0, m, o;
        for (n = 0; n < count; n++) {
                m = 0;  // assume src0 > src1
                for (o = 1; o < nsrc; ++o) {
                        if (srcs[m][n].a < srcs[o][n].a ||
                            (srcs[m][n].a == srcs[o][n].a && srcs[m][n].b > srcs[o][n].b)) {
                                m = o;
                        }
                }
                dst[n].a = srcs[m][n].a;
                dst[n].b = srcs[m][n].b;
        }
}

void _pami_core_fp32_fp32_maxloc(fp32_fp32_t *dst, const fp32_fp32_t **srcs, int nsrc, int count) {
        register int n = 0, m, o;
        for (n = 0; n < count; n++) {
                m = 0;  // assume src0 > src1
                for (o = 1; o < nsrc; ++o) {
                        if (srcs[m][n].a < srcs[o][n].a ||
                            (srcs[m][n].a == srcs[o][n].a && srcs[m][n].b > srcs[o][n].b)) {
                                m = o;
                        }
                }
                dst[n].a = srcs[m][n].a;
                dst[n].b = srcs[m][n].b;
        }
}

void _pami_core_fp32_int32_minloc(fp32_int32_t *dst, const fp32_int32_t **srcs, int nsrc, int count) {
        register int n = 0, m, o;
        for (n = 0; n < count; n++) {
                m = 0;  // assume src0 < src1
                for (o = 1; o < nsrc; ++o) {
                        if (srcs[m][n].a > srcs[o][n].a ||
                            (srcs[m][n].a == srcs[o][n].a && srcs[m][n].b > srcs[o][n].b)) {
                                m = o;
                        }
                }
                dst[n].a = srcs[m][n].a;
                dst[n].b = srcs[m][n].b;
        }
}

void _pami_core_fp32_fp32_minloc(fp32_fp32_t *dst, const fp32_fp32_t **srcs, int nsrc, int count) {
        register int n = 0, m, o;
        for (n = 0; n < count; n++) {
                m = 0;  // assume src0 < src1
                for (o = 1; o < nsrc; ++o) {
                        if (srcs[m][n].a > srcs[o][n].a ||
                            (srcs[m][n].a == srcs[o][n].a && srcs[m][n].b > srcs[o][n].b)) {
                                m = o;
                        }
                }
                dst[n].a = srcs[m][n].a;
                dst[n].b = srcs[m][n].b;
        }
}

void _pami_core_fp32_fp32_cplx_prod(fp32_fp32_t *dst, const fp32_fp32_t **srcs, int nsrc, int count) {
        register int n = 0, m;
        for (n = 0; n < count; n++) {
                dst[n].a = srcs[0][n].a;
                dst[n].b = srcs[0][n].b;
                for (m = 1; m < nsrc; ++m) {
                        float a = dst[n].a * srcs[m][n].a - dst[n].b * srcs[m][n].b;
                        dst[n].b = dst[n].a * srcs[m][n].b + dst[n].b * srcs[m][n].a;
                        dst[n].a = a;
                }
        }
}
