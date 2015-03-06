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
 * \file math/ppc450d/int32_o.cc
 * \brief Optimized math routines for signed 32 bit integer operations on
 *        the ppc 450 dual fpu architecture.
 */
#include "math_coremath.h"
#include "Util.h"
#include "ppc450d/internal_o.h"

void _pami_core_int32_max2(int32_t *dst, const int32_t **srcs, int nsrc, int count) {

  int32_t *dp = (int32_t *)dst;
  const int32_t *s0 = (const int32_t *)srcs[0];
  const int32_t *s1 = (const int32_t *)srcs[1];

  int n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "lwz     5,0(%[s0]);"

      "lwz     9,0(%[s1]);"

      "lwz     6,4(%[s0]);"
      "cmpw    5,9;"

      "lwz     10,4(%[s1]);"
      "bge     0f;"

      "mr      5,9;"

"0:    stw     5,0(%[dp]);"

      "lwz     7,8(%[s0]);"

      "lwz     11,8(%[s1]);"
      "cmpw    6,10;"

      "lwz     8,12(%[s0]);"
      "bge     1f;"

      "mr      6,10;"

"1:    stw     6,4(%[dp]);"
      "cmpw    7,11;"

      "lwz     12,12(%[s1]);"
      "bge     2f;"

      "mr      7,11;"

"2:    stw     7,8(%[dp]);"

      "cmpw    8,12;"

      "bge     3f;"

      "mr      8,12;"

"3:    stw     8,12(%[dp]);"

      : // no outputs
      : [s0] "b" (s0),
        [s1] "b" (s1),
        [dp] "b" (dp)
      : "memory",
        "5",  "6",  "7",  "8",  "9",  "10", "11", "12"
        );
    dp += 4;
    s0 += 4;
    s1 += 4;
  }

  register int32_t s0_r;
  register int32_t s1_r;
  n = count & 0x03;
  while ( n-- ) {
    s0_r = *s0;
    s1_r = *s1;
    if (s0_r > s1_r) *(dp) = s0_r;
    else *(dp) = s1_r;

    s0++;
    s1++;
    dp++;
  }

  return;
}

void _pami_core_int32_min2(int32_t *dst, const int32_t **srcs, int nsrc, int count) {

  int32_t *dp = (int32_t *)dst;
  const int32_t *s0 = (const int32_t *)srcs[0];
  const int32_t *s1 = (const int32_t *)srcs[1];

  int n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "lwz     5,0(%[s0]);"
      "lwz     9,0(%[s1]);"

      "lwz     6,4(%[s0]);"
      "lwz     10,4(%[s1]);"

      "lwz     7,8(%[s0]);"
      "lwz     11,8(%[s1]);"

      "lwz     8,12(%[s0]);"
      "lwz     12,12(%[s1]);"

      "cmpw    9,5;"
      "bge     0f;"
      "mr      5,9;"
"0:    stw     5,0(%[dp]);"

      "cmpw    10,6;"
      "bge     1f;"
      "mr      6,10;"
"1:    stw     6,4(%[dp]);"

      "cmpw    11,7;"
      "bge     2f;"
      "mr      7,11;"
"2:    stw     7,8(%[dp]);"

      "cmpw    12,8;"
      "bge     3f;"
      "mr      8,12;"
"3:    stw     8,12(%[dp]);"

      : // no outputs
      : [s0] "b" (s0),
        [s1] "b" (s1),
        [dp] "b" (dp)
      : "memory",
        "5",  "6",  "7",  "8",  "9",  "10", "11", "12"
        );
    dp += 4;
    s0 += 4;
    s1 += 4;
  }

  register int32_t s0_r;
  register int32_t s1_r;
  n = count & 0x03;
  while ( n-- ) {
    s0_r = *s0;
    s1_r = *s1;
    if (s1_r > s0_r) *(dp) = s0_r;
    else *(dp) = s1_r;

    s0++;
    s1++;
    dp++;
  }

  return;
}

void _pami_core_int32_prod2(int32_t *dst, const int32_t **srcs, int nsrc, int count) {

        const int32_t *s0 = srcs[0];
        const int32_t *s1 = srcs[1];
        int num;
        int remainder = count;
        if (count <= 2048) {
                num = count >> 2;
                remainder = count - (num << 2);
                register int r0=0;
                register int r1=0;
                register int r2=0;
                register int r3=0;
                register int r4=0;
                register int r5=0;
                register int r6=0;
                register int r7=0;

                while (num--) {
                        asm volatile(
                                "lwz   %[r0],0(%[s0]);"
                                "lwz   %[r4],0(%[s1]);"
                                "lwz   %[r1],4(%[s0]);"
                                "lwz   %[r5],4(%[s1]);"

                                "mullw %[r0],%[r0],%[r4];"
                                "stw   %[r0],0(%[dp]);"

                                "lwz   %[r2],8(%[s0]);"
                                "lwz   %[r6],8(%[s1]);"
                                "mullw %[r1],%[r1],%[r5];"
                                "stw   %[r1],4(%[dp]);"

                                "lwz   %[r3],12(%[s0]);"
                                "lwz   %[r7],12(%[s1]);"
                                "mullw %[r2],%[r2],%[r6];"
                                "stw   %[r2],8(%[dp]);"

                                "mullw %[r3],%[r3],%[r7];"
                                "stw   %[r3],12(%[dp]);"

                                "addi  %[s0],%[s0],16;"
                                "addi  %[s1],%[s1],16;"
                                "addi  %[dp],%[dp],16;"

                                : [s0] "+b" (s0),
                                  [s1] "+b" (s1),
                                  [dp] "+b" (dst),
                                  [r0] "+r" (r0),
                                  [r1] "+r" (r1),
                                  [r2] "+r" (r2),
                                  [r3] "+r" (r3),
                                  [r4] "+r" (r4),
                                  [r5] "+r" (r5),
                                  [r6] "+r" (r6),
                                  [r7] "+r" (r7)
                                :
                                : "memory");
                }
                for (num = 0; num < remainder; ++num) {
                        dst[num] = s0[num] * s1[num];
                }
                return;
        }

        int32_t *s2 = dst;
        num = (count - 16) >> 3;
        remainder = (count & 0x07) + 16;
#define OP2(a,b)	asm volatile ("mullw %0, %0, %1" : "=r"(a) : "r"(b))
#include "ppc450d/_optim_uint32_dual_src.x.h"
#undef OP2
        for (num = 0; num < remainder; ++num) {
                s2[num] = s0[num] * s1[num];
        }

        //fprintf (stderr, "<< Core_uint32_prod()\n");
        return;
}

void _pami_core_int32_sum2(int32_t *dst, const int32_t **srcs, int nsrc, int count) {

        const int32_t *s0 = srcs[0];
        const int32_t *s1 = srcs[1];
        int num;
        int remainder = count;
        if (count <= 2048) {
                num = count >> 2;
                remainder = count - (num << 2);
                register int r0=0;
                register int r1=0;
                register int r2=0;
                register int r3=0;
                register int r4=0;
                register int r5=0;
                register int r6=0;
                register int r7=0;

                while (num--) {
                        asm volatile(
                                "lwz   %[r0],0(%[s0]);"
                                "lwz   %[r4],0(%[s1]);"
                                "lwz   %[r1],4(%[s0]);"
                                "lwz   %[r5],4(%[s1]);"

                                "add   %[r0],%[r0],%[r4];"
                                "stw   %[r0],0(%[dp]);"

                                "lwz   %[r2],8(%[s0]);"
                                "lwz   %[r6],8(%[s1]);"
                                "add   %[r1],%[r1],%[r5];"
                                "stw   %[r1],4(%[dp]);"

                                "lwz   %[r3],12(%[s0]);"
                                "lwz   %[r7],12(%[s1]);"
                                "add   %[r2],%[r2],%[r6];"
                                "stw   %[r2],8(%[dp]);"

                                "add   %[r3],%[r3],%[r7];"
                                "stw   %[r3],12(%[dp]);"

                                "addi  %[s0],%[s0],16;"
                                "addi  %[s1],%[s1],16;"
                                "addi  %[dp],%[dp],16;"

                                : [s0] "+b" (s0),
                                  [s1] "+b" (s1),
                                  [dp] "+b" (dst),
                                  [r0] "+r" (r0),
                                  [r1] "+r" (r1),
                                  [r2] "+r" (r2),
                                  [r3] "+r" (r3),
                                  [r4] "+r" (r4),
                                  [r5] "+r" (r5),
                                  [r6] "+r" (r6),
                                  [r7] "+r" (r7)
                                :
                                : "memory");
                }
                for (num = 0; num < remainder; ++num) {
                        dst[num] = s0[num] + s1[num];
                }
                return;
        }

        int32_t *s2 = dst;
        num = (count - 16) >> 3;
        remainder = (count & 0x07) + 16;
#define OP2(a,b)	asm volatile ("add %0, %0, %1" : "=r"(a) : "r"(b))
#include "ppc450d/_optim_uint32_dual_src.x.h"
#undef OP2
        for (num = 0; num < remainder; ++num) {
                s2[num] = s0[num] + s1[num];
        }

        //fprintf (stderr, "<< Core_uint32_sum()\n");
        return;
}

void _pami_core_int32_band4(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
const int32_t *src0 = srcs[0];
const int32_t *src1 = srcs[1];
const int32_t *src2 = srcs[2];
const int32_t *src3 = srcs[3];
#define OP(a,b,c,d)	(a&b&c&d)
#define TYPE		int32_t
#include "_quad_src.x.h"
#undef OP
#undef TYPE
        return;
}

void _pami_core_int32_bor4(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
const int32_t *src0 = srcs[0];
const int32_t *src1 = srcs[1];
const int32_t *src2 = srcs[2];
const int32_t *src3 = srcs[3];
#define OP(a,b,c,d)	(a|b|c|d)
#define TYPE		int32_t
#include "_quad_src.x.h"
#undef OP
#undef TYPE
        return;
}

void _pami_core_int32_bxor4(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
const int32_t *src0 = srcs[0];
const int32_t *src1 = srcs[1];
const int32_t *src2 = srcs[2];
const int32_t *src3 = srcs[3];
#define OP(a,b,c,d)	(a^b^c^d)
#define TYPE		int32_t
#include "_quad_src.x.h"
#undef OP
#undef TYPE
        return;
}

void _pami_core_int32_max4(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
const int32_t *src0 = srcs[0];
const int32_t *src1 = srcs[1];
const int32_t *src2 = srcs[2];
const int32_t *src3 = srcs[3];
#define OP(a,b,c,d)	MAX(MAX(a,b),MAX(c,d))
#define TYPE		int32_t
#include "_quad_src.x.h"
#undef OP
#undef TYPE
        return;
}

void _pami_core_int32_min4(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
const int32_t *src0 = srcs[0];
const int32_t *src1 = srcs[1];
const int32_t *src2 = srcs[2];
const int32_t *src3 = srcs[3];
#define OP(a,b,c,d)	MIN(MIN(a,b),MIN(c,d))
#define TYPE		int32_t
#include "_quad_src.x.h"
#undef OP
#undef TYPE
        return;
}

void _pami_core_int32_prod4(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
const int32_t *src0 = srcs[0];
const int32_t *src1 = srcs[1];
const int32_t *src2 = srcs[2];
const int32_t *src3 = srcs[3];
#define OP(a,b,c,d)	(a*b*c*d)
#define TYPE		int32_t
#include "_quad_src.x.h"
#undef OP
#undef TYPE
        return;
}

void _pami_core_int32_sum4(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
const int32_t *src0 = srcs[0];
const int32_t *src1 = srcs[1];
const int32_t *src2 = srcs[2];
const int32_t *src3 = srcs[3];
#define OP(a,b,c,d)	(a+b+c+d)
#define TYPE		int32_t
#include "_quad_src.x.h"
#undef OP
#undef TYPE
        return;
}

void _pami_core_int32_band2(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)&(b))

#define TYPE int32_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int32_bor2(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)|(b))

#define TYPE int32_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int32_bxor2(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)^(b))

#define TYPE int32_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int32_land2(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)&&(b))

#define TYPE int32_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int32_lor2(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)||(b))

#define TYPE int32_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int32_lxor2(int32_t *dst, const int32_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)&&(!b))||((!a)&&(b)))

#define TYPE int32_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int32_int32_maxloc2(int32_int32_t *dst, const int32_int32_t **srcs, int nsrc, int count) {
  const int32_int32_t *s0 = srcs[0], *s1 = srcs[1];
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

void _pami_core_int32_int32_minloc2(int32_int32_t *dst, const int32_int32_t **srcs, int nsrc, int count) {
  const int32_int32_t *s0 = srcs[0], *s1 = srcs[1];
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
