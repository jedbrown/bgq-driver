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
 * \file math/ppc450d/fp64_o.cc
 * \brief Optimized math routines for 64 bit floating point operations on
 *        the ppc 450 dual fpu architecture.
 */
#include "math_coremath.h"
#include "FloatUtil.h"
#include "Util.h"
#include "ppc450d/internal_o.h"

void _pami_core_fp64_sum2(double *dst, const double **srcs, int nsrc, int count) {
        if (count < 16) {
                // This seems necessary for good latency,
                // but its not as good as the unoptimized compile.
#define OP(a,b)	((a) + (b))
#define TYPE	double
#include "_dual_src.x.h"
#undef OP
#undef TYPE
                return;
        }
        int size128 = count >> 4;
        int remainder = count & 0xf;
        const double *f0 = srcs[0];
        const double *f1 = srcs[1];
        double *f2 = dst;
        if (size128 > 0) {
#define OP2(a,b)	(a) += (b)
#define OP3(a,b,c)	asm volatile ("fpadd %0, %1, %2" : "=f"(a) : "f"(b), "f"(c))
#include "ppc450d/_optim_fp64_dual_src.x.h"
#undef OP2
#undef OP3
        }
        int n;
        for (n = 0; n < remainder; ++n) {
                f2[n] = f0[n] + f1[n];
        }
        return;
}

void _pami_core_fp64_max2(double *dst, const double **srcs, int nsrc, int count) {
        if (count < 16) {
                // This seems necessary for good latency,
                // but its not as good as the unoptimized compile.
#define OP(a,b) (((a)>(b))?(a):(b))
#define TYPE	double
#include "_dual_src.x.h"
#undef OP
#undef TYPE
                return;
        }
        int size128 = count >> 4;
        int remainder = count & 0xf;
        const double *f0 = srcs[0];
        const double *f1 = srcs[1];
        double *f2 = dst;
        if (size128 > 0) {
#define OP2(a,b)  a=(((a)>(b))?(a):(b))
#define OP3(a,b,c)	asm volatile ("fpsub %0, %1, %2" : "=f"(a) : "f"(b), "f"(c)); \
                        asm volatile ("fpsel %0, %1, %2, %3" : "=f"(a) : "f"(a), "f"(b), "f"(c))
#include "ppc450d/_optim_fp64_dual_src.x.h"
#undef OP2
#undef OP3
        }
        int n;
        for (n = 0; n < remainder; ++n) {
          f2[n] = (((f0[n])>(f1[n]))?(f0[n]):(f1[n]));
        }
        return;
}

void _pami_core_fp64_min2(double *dst, const double **srcs, int nsrc, int count) {
        if (count < 16) {
                // This seems necessary for good latency,
                // but its not as good as the unoptimized compile.
#define OP(a,b) (((a)>(b))?(b):(a))
#define TYPE	double
#include "_dual_src.x.h"
#undef OP
#undef TYPE
                return;
        }
        int size128 = count >> 4;
        int remainder = count & 0xf;
        const double *f0 = srcs[0];
        const double *f1 = srcs[1];
        double *f2 = dst;
        if (size128 > 0) {
#define OP2(a,b)  a=(((a)>(b))?(b):(a))
#define OP3(a,b,c)	asm volatile ("fpsub %0, %1, %2" : "=f"(a) : "f"(b), "f"(c)); \
                        asm volatile ("fpsel %0, %1, %2, %3" : "=f"(a) : "f"(a), "f"(c), "f"(b))
#include "ppc450d/_optim_fp64_dual_src.x.h"
#undef OP2
#undef OP3
        }
        int n;
        for (n = 0; n < remainder; ++n) {
          f2[n] = (((f0[n])>(f1[n]))?(f1[n]):(f0[n]));
        }
        return;
}


void _pami_core_fp64_prod2(double *dst, const double **srcs, int nsrc, int count) {
#define OP(a,b) ((a)*(b))

#define TYPE double
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_fp64_int32_maxloc2(fp64_int32_t *dst, const fp64_int32_t **srcs, int nsrc, int count) {
  const fp64_int32_t *s0 = srcs[0], *s1 = srcs[1];
  register int n = 0;
  for (n = 0; n < count; n++)
    {
      if(s0[n].a > s1[n].a)
        dst[n].b = s0[n].b;
      else if(s0[n].a < s1[n].a)
        dst[n].b = s1[n].b;
      else
        dst[n].b = MIN(s0[n].b, s1[n].b);
      dst[n].a = MAX(s0[n].a,s1[n].a);
    }
}

void _pami_core_fp64_fp64_maxloc2(fp64_fp64_t *dst, const fp64_fp64_t **srcs, int nsrc, int count) {
  const fp64_fp64_t *s0 = srcs[0], *s1 = srcs[1];
  register int n = 0;
  for (n = 0; n < count; n++)
    {
      if(s0[n].a > s1[n].a)
        dst[n].b = s0[n].b;
      else if(s0[n].a < s1[n].a)
        dst[n].b = s1[n].b;
      else
        dst[n].b = MIN(s0[n].b, s1[n].b);
      dst[n].a = MAX(s0[n].a,s1[n].a);
    }
}

void _pami_core_fp64_int32_minloc2(fp64_int32_t *dst, const fp64_int32_t **srcs, int nsrc, int count) {
  const fp64_int32_t *s0 = srcs[0], *s1 = srcs[1];
  register int n = 0;
  for (n = 0; n < count; n++)
    {
      if(s0[n].a < s1[n].a)
        dst[n].b = s0[n].b;
      else if(s0[n].a > s1[n].a)
        dst[n].b = s1[n].b;
      else
        dst[n].b = MIN(s0[n].b, s1[n].b);
      dst[n].a = MIN(s0[n].a,s1[n].a);
    }
}

void _pami_core_fp64_fp64_minloc2(fp64_fp64_t *dst, const fp64_fp64_t **srcs, int nsrc, int count) {
  const fp64_fp64_t *s0 = srcs[0], *s1 = srcs[1];
  register int n = 0;
  for (n = 0; n < count; n++)
    {
      if(s0[n].a < s1[n].a)
        dst[n].b = s0[n].b;
      else if(s0[n].a > s1[n].a)
        dst[n].b = s1[n].b;
      else
        dst[n].b = MIN(s0[n].b, s1[n].b);
      dst[n].a = MIN(s0[n].a,s1[n].a);
    }
}

void _pami_core_fp64_max4(double *dst, const double **srcs, int nsrc, int count) {
const double *src0 = srcs[0];
const double *src1 = srcs[1];
const double *src2 = srcs[2];
const double *src3 = srcs[3];
#define OP(a,b,c,d)	MAX(MAX(a,b),MAX(c,d))
#define TYPE		double
#include "_quad_src.x.h"
#undef OP
#undef TYPE
        return;
}

void _pami_core_fp64_min4(double *dst, const double **srcs, int nsrc, int count) {
const double *src0 = srcs[0];
const double *src1 = srcs[1];
const double *src2 = srcs[2];
const double *src3 = srcs[3];
#define OP(a,b,c,d)	MIN(MIN(a,b),MIN(c,d))
#define TYPE		double
#include "_quad_src.x.h"
#undef OP
#undef TYPE
        return;
}

void _pami_core_fp64_prod4(double *dst, const double **srcs, int nsrc, int count) {
const double *src0 = srcs[0];
const double *src1 = srcs[1];
const double *src2 = srcs[2];
const double *src3 = srcs[3];
#define OP(a,b,c,d)	(a*b*c*d)
#define TYPE		double
#include "_quad_src.x.h"
#undef OP
#undef TYPE
        return;
}

void _pami_core_fp64_sum4(double *dst, const double **srcs, int nsrc, int count) {
const double *src0 = srcs[0];
const double *src1 = srcs[1];
const double *src2 = srcs[2];
const double *src3 = srcs[3];
#define OP(a,b,c,d)	(a+b+c+d)
#define TYPE		double
#include "_quad_src.x.h"
#undef OP
#undef TYPE
        return;
}
