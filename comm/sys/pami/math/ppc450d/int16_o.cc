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
 * \file math/ppc450d/int16_o.cc
 * \brief Optimized math routines for signed 16 bit integer operations on
 *        the ppc 450 dual fpu architecture.
 */
#include "math_coremath.h"
#include "Util.h"
#include "ppc450d/internal_o.h"

void _pami_core_int16_max2(int16_t *dst, const int16_t **srcs, int nsrc, int count) {

  int16_t *dp = (int16_t *)dst;
  const int16_t *s0 = (const int16_t *)srcs[0];
  const int16_t *s1 = (const int16_t *)srcs[1];

  int n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "lha     5,0(%[s0]);"

      "lha     9,0(%[s1]);"

      "lha     6,2(%[s0]);"
      "cmpw    5,9;"

      "lha     10,2(%[s1]);"
      "bge     0f;"

      "mr      5,9;"

"0:    sth     5,0(%[dp]);"


      "lha     7,4(%[s0]);"
      "cmpw    6,10;"

      "lha     11,4(%[s1]);"
      "bge     1f;"

      "mr      6,10;"

"1:    sth     6,2(%[dp]);"

      "lha     8,6(%[s0]);"
      "cmpw    7,11;"

      "lha     12,6(%[s1]);"
      "bge     2f;"

      "mr      7,11;"

"2:    sth     7,4(%[dp]);"

      "cmpw    8,12;"

      "bge     3f;"

      "mr      8,12;"

"3:    sth     8,6(%[dp]);"

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

  register int16_t s0_r;
  register int16_t s1_r;
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

void _pami_core_int16_min2(int16_t *dst, const int16_t **srcs, int nsrc, int count) {

  int16_t *dp = (int16_t *)dst;
  const int16_t *s0 = (const int16_t *)srcs[0];
  const int16_t *s1 = (const int16_t *)srcs[1];

  int n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "lha     5,0(%[s0]);"

      "lha     9,0(%[s1]);"

      "lha     6,2(%[s0]);"
      "cmpw    9,5;"

      "lha     10,2(%[s1]);"
      "bge     0f;"

      "mr      5,9;"

"0:    sth     5,0(%[dp]);"

      "lha     7,4(%[s0]);"
      "cmpw    10,6;"

      "lha     11,4(%[s1]);"
      "bge     1f;"

      "mr      6,10;"

"1:    sth     6,2(%[dp]);"

      "lha     8,6(%[s0]);"
      "cmpw    11,7;"

      "lha     12,6(%[s1]);"
      "bge     2f;"

      "mr      7,11;"

"2:    sth     7,4(%[dp]);"

      "cmpw    12,8;"

      "bge     3f;"

      "mr      8,12;"

"3:    sth     8,6(%[dp]);"

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

  register int s0_r;
  register int s1_r;
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

void _pami_core_int16_prod2(int16_t *dst, const int16_t **srcs, int nsrc, int count) {

  int16_t *dp = (int16_t *)dst;
  const int16_t *s0 = (const int16_t *)srcs[0];
  const int16_t *s1 = (const int16_t *)srcs[1];

  int n = count >> 3;
  while ( n-- ) {
    asm volatile (
      "lwz     5,0(%[s0]);"

      "lwz     6,0(%[s1]);"

      "mulhhw  7,5,6;"
      "lwz     8,4(%[s0]);"

      "mullhw  6,5,6;"
      "sth     7,0(%[dp]);"

      "lwz     9,4(%[s1]);"

      "sth     6,2(%[dp]);"
      "mulhhw  10,8,9;"

      "mullhw  9,8,9;"
      "sth     10,4(%[dp]);"

      "sth     9,6(%[dp]);"



      "lwz     5,8(%[s0]);"

      "lwz     6,8(%[s1]);"

      "mulhhw  7,5,6;"
      "lwz     8,12(%[s0]);"

      "mullhw  6,5,6;"
      "sth     7,8(%[dp]);"

      "lwz     9,12(%[s1]);"

      "sth     6,10(%[dp]);"
      "mulhhw  10,8,9;"

      "mullhw  9,8,9;"
      "sth     10,12(%[dp]);"

      "sth     9,14(%[dp]);"






//       "lwz     5,0(%[s0]);"
//       "lwz     6,0(%[s1]);"
//       "mulhhw  7,5,6;"
//       "mullhw  6,5,6;"
//       "sth     7,0(%[dp]);"
//       "sth     6,2(%[dp]);"
//
//       "lwz     8,4(%[s0]);"
//       "lwz     9,4(%[s1]);"
//       "mulhhw  10,8,9;"
//       "mullhw  9,8,9;"
//       "sth     10,4(%[dp]);"
//       "sth     9,6(%[dp]);"
//
//       "lwz     5,8(%[s0]);"
//       "lwz     6,8(%[s1]);"
//       "mulhhw  7,5,6;"
//       "mullhw  6,5,6;"
//       "sth     7,8(%[dp]);"
//       "sth     6,10(%[dp]);"
//
//       "lwz     8,12(%[s0]);"
//       "lwz     9,12(%[s1]);"
//       "mulhhw  10,8,9;"
//       "mullhw  9,8,9;"
//       "sth     10,12(%[dp]);"
//       "sth     9,14(%[dp]);"


      : // no outputs
      : [s0] "b" (s0),
        [s1] "b" (s1),
        [dp] "b" (dp)
      : "memory",
        "5",  "6",  "7",  "8",  "9",  "10", "11", "12"
        );
    dp += 8;
    s0 += 8;
    s1 += 8;
  }

  register int16_t s0_r;
  register int16_t s1_r;


  n = count & 0x07;
  while ( n-- ) {
    s0_r = *s0;
    s1_r = *s1;
    *dp = s0_r * s1_r;

    dp++;
    s0++;
    s1++;
  }

  return;
}

void _pami_core_int16_sum2(int16_t *dst, const int16_t **srcs, int nsrc, int count) {

  const int16_t *s0 = (const int16_t *)srcs[0];
  const int16_t *s1 = (const int16_t *)srcs[1];
  int16_t *dp = (int16_t *)dst;

  size_t n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "lha     5,0(%[s0]);"

      "lha     9,0(%[s1]);"

      "add     5,5,9;"
      "lha     6,2(%[s0]);"

      "lha     10,2(%[s1]);"

      "sth     5,0(%[dp]);"
      "add     6,6,10;"

      "lha     7,4(%[s0]);"

      "lha     11,4(%[s1]);"

      "sth     6,2(%[dp]);"
      "add     7,7,11;"

      "lha     8,6(%[s0]);"

      "lha     12,6(%[s1]);"

      "sth     7,4(%[dp]);"
      "add     8,8,12;"

      "sth     8,6(%[dp]);"

      : [s0] "+b" (s0),
        [s1] "+b" (s1),
        [dp] "+b" (dp)
      : // no inputs
      : "memory",
        "5",  "6",  "7",  "8",  "9",  "10", "11", "12"
        );
    s0 += 4;
    s1 += 4;
    dp += 4;
  }

  register int s0_r asm("r5");
  register int s1_r asm("r6");

  n = count & 0x03;
  while ( n-- ) {
    s0_r = *(s0);
    s1_r = *(s1);
    *(dp+0) = s0_r + s1_r;

    s0++;
    s1++;
    dp++;
  }

  return;
}

void _pami_core_int16_band2(int16_t *dst, const int16_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)&(b))

#define TYPE int16_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int16_bor2(int16_t *dst, const int16_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)|(b))

#define TYPE int16_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int16_bxor2(int16_t *dst, const int16_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)^(b))

#define TYPE int16_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int16_land2(int16_t *dst, const int16_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)&&(b))

#define TYPE int16_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int16_lor2(int16_t *dst, const int16_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)||(b))

#define TYPE int16_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int16_lxor2(int16_t *dst, const int16_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)&&(!b))||((!a)&&(b)))

#define TYPE int16_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int16_int32_maxloc2(int16_int32_t *dst, const int16_int32_t **srcs, int nsrc, int count) {
  const int16_int32_t *s0 = srcs[0], *s1 = srcs[1];
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

void _pami_core_int16_int32_minloc2(int16_int32_t *dst, const int16_int32_t **srcs, int nsrc, int count) {
  const int16_int32_t *s0 = srcs[0], *s1 = srcs[1];
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
