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
 * \file math/ppc450d/fp32_o.cc
 * \brief Optimized math routines for 32 bit floating point operations on
 *        the ppc 450 dual fpu architecture.
 *
 * \note Optimized 2-way sum float turned off.
 */
#include "math_coremath.h"
#include "FloatUtil.h"
#include "Util.h"
#include "ppc450d/internal_o.h"

static inline void _sum_aligned2(float *dst, const float **srcs, int nsrc, int count) {

  uint8_t *dp = (uint8_t *)dst;
  uint8_t *s0 = (uint8_t *)srcs[0];
  uint8_t *s1 = (uint8_t *)srcs[1];

  s0 -= 8;
  s1 -= 8;
  dp -= 8;

  register int tmp = 8;

  int n = count >> 4;
  while ( n-- ) {

    asm volatile(

      "lfpsux  1,%[s0],%[offset];"
      "lfpsux  2,%[s0],%[offset];"
      "lfpsux  3,%[s0],%[offset];"
      "lfpsux  4,%[s0],%[offset];"
      "lfpsux  5,%[s1],%[offset];"
      "lfpsux  6,%[s1],%[offset];"
      "lfpsux  7,%[s1],%[offset];"
      "lfpsux  8,%[s1],%[offset];"
      "fpadd   1,1,5;"
      "stfpsux 1,%[dp],%[offset];"
      "fpadd   2,2,6;"
      "stfpsux 2,%[dp],%[offset];"
      "fpadd   3,3,7;"
      "stfpsux 3,%[dp],%[offset];"
      "fpadd   4,4,8;"
      "stfpsux 4,%[dp],%[offset];"

      "lfpsux  1,%[s0],%[offset];"
      "lfpsux  2,%[s0],%[offset];"
      "lfpsux  3,%[s0],%[offset];"
      "lfpsux  4,%[s0],%[offset];"
      "lfpsux  5,%[s1],%[offset];"
      "lfpsux  6,%[s1],%[offset];"
      "lfpsux  7,%[s1],%[offset];"
      "lfpsux  8,%[s1],%[offset];"
      "fpadd   1,1,5;"
      "stfpsux 1,%[dp],%[offset];"
      "fpadd   2,2,6;"
      "stfpsux 2,%[dp],%[offset];"
      "fpadd   3,3,7;"
      "stfpsux 3,%[dp],%[offset];"
      "fpadd   4,4,8;"
      "stfpsux 4,%[dp],%[offset];"

      : [s0] "+b" (s0),
        [s1] "+b" (s1),
        [dp] "+b" (dp)
      : [offset] "r" (tmp)
      : "memory", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7", "fr8"
    );
  }

  n = count & 0x0f;
  while ( n-- ) {
    asm volatile(

      "lfpsux  1,%[s0],%[offset];"
      "lfpsux  2,%[s1],%[offset];"
      "fpadd   1,1,2;"
      "stfpsux 1,%[dp],%[offset];"

      : [s0] "+b" (s0),
        [s1] "+b" (s1),
        [dp] "+b" (dp)
      : [offset] "r" (tmp)
      : "memory", "fr1", "fr2"
    );
  }
}

#if 0
void _pami_core_fp32_sum2(float *dst, const float **srcs, int nsrc, int count) {

        if (( ((uint32_t) dst) | ((uint32_t) srcs[0]) | ((uint32_t) srcs[1])) & 0x03) {
                // a buffer is not 8-byte aligned.
                const float *s0 = srcs[0], *s1 = srcs[1];
                register int n=0;
                register float rbuffer0, rbuffer1, rbuffer2, rbuffer3;
                register float buf00, buf01, buf02, buf03;
                register float buf10, buf11, buf12, buf13;
                for(; n<count-3; n+=4) {
                        buf00 = s0[n+0]; buf10 = s1[n+0];
                        buf01 = s0[n+1]; buf11 = s1[n+1];
                        buf02 = s0[n+2]; buf12 = s1[n+2];
                        buf03 = s0[n+3]; buf13 = s1[n+3];

                        rbuffer0 = buf00+buf10;
                        rbuffer1 = buf01+buf11;
                        rbuffer2 = buf02+buf12;
                        rbuffer3 = buf03+buf13;

                        dst[n+0] = rbuffer0;
                        dst[n+1] = rbuffer1;
                        dst[n+2] = rbuffer2;
                        dst[n+3] = rbuffer3;
                }
                for(; n<count; n++) {
                        dst[n] = s0[n]+s1[n];
                }
        } else {
                _sum_aligned2(dst, srcs[0], srcs[1], count);
        }

        return;
}
#else
//#warning optimized 2-way sum float turned off

void _pami_core_fp32_sum2(float *dst, const float **srcs, int nsrc, int count) {
#define OP(a,b) ((a)+(b))

#define TYPE float
#include "../_dual_src.x.h"
#undef TYPE
#undef OP
}
#endif

void _pami_core_fp32_max2(float *dst, const float **srcs, int nsrc, int count) {
#define OP(a,b) (((a)>(b))?(a):(b))

#define TYPE float
#include "../_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_fp32_min2(float *dst, const float **srcs, int nsrc, int count) {
#define OP(a,b) (((a)>(b))?(b):(a))

#define TYPE float
#include "../_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_fp32_prod2(float *dst, const float **srcs, int nsrc, int count) {
#define OP(a,b) ((a)*(b))

#define TYPE float
#include "../_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_fp32_int32_maxloc2(fp32_int32_t *dst, const fp32_int32_t **srcs, int nsrc, int count) {
  const fp32_int32_t *s0 = srcs[0], *s1 = srcs[1];
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

void _pami_core_fp32_fp32_maxloc2(fp32_fp32_t *dst, const fp32_fp32_t **srcs, int nsrc, int count) {
  const fp32_fp32_t *s0 = srcs[0], *s1 = srcs[1];
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

void _pami_core_fp32_int32_minloc2(fp32_int32_t *dst, const fp32_int32_t **srcs, int nsrc, int count) {
  const fp32_int32_t *s0 = srcs[0], *s1 = srcs[1];
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

void _pami_core_fp32_fp32_minloc2(fp32_fp32_t *dst, const fp32_fp32_t **srcs, int nsrc, int count) {
  const fp32_fp32_t *s0 = srcs[0], *s1 = srcs[1];
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
