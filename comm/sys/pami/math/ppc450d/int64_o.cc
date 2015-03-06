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
 * \file math/ppc450d/int64_o.cc
 * \brief Optimized math routines for signed 64 bit integer operations on
 *        the ppc 450 dual fpu architecture.
 */
#include "math_coremath.h"
#include "Util.h"
#include "ppc450d/internal_o.h"

void _pami_core_int64_sum2(int64_t *dst, const int64_t **srcs, int nsrc, int count) {

  int64_t *dp = (int64_t *) dst;
  const int64_t *s0 = (const int64_t *) srcs[0];
  const int64_t *s1 = (const int64_t *) srcs[1];

  int q64 = count >> 1;
  while (q64--)
    {
      asm volatile
        (
          "lwz     5,4(%[s0]);"

          "lwz     6,4(%[s1]);"

          "lwz     7,0(%[s0]);"
          "addc    5,5,6;"

          "lwz     8,0(%[s1]);"

          "stw     5,4(%[dp]);"
          "adde    7,7,8;"

          "stw     7,0(%[dp]);"

          "lwz     9,12(%[s0]);"

          "lwz    10,12(%[s1]);"

          "lwz    11,8(%[s0]);"
          "addc    9,9,10;"

          "lwz    12,8(%[s1]);"

          "stw     9,12(%[dp]);"
          "adde   11,11,12;"

          "stw    11,8(%[dp]);"

          : // no outputs
          : [s0] "b" (s0),
            [s1] "b" (s1),
            [dp] "b" (dp)
          : "memory", "5", "6", "7", "8", "9", "10", "11", "12"
        );
      dp += 2;
      s0 += 2;
      s1 += 2;
    }

  if (count & 0x01)
    {
      (*dp) = (*s0) + (*s1);
    }

  return;
}

void _pami_core_int64_band2(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)&(b))

#define TYPE int64_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int64_bor2(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)|(b))

#define TYPE int64_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int64_bxor2(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)^(b))

#define TYPE int64_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int64_land2(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)&&(b))

#define TYPE int64_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int64_lor2(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)||(b))

#define TYPE int64_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int64_lxor2(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)&&(!b))||((!a)&&(b)))

#define TYPE int64_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int64_max2(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)>(b))?(a):(b))

#define TYPE int64_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int64_min2(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)>(b))?(b):(a))

#define TYPE int64_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int64_prod2(int64_t *dst, const int64_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)*(b))

#define TYPE int64_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}
