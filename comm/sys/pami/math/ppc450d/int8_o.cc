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
 * \file math/ppc450d/int8_o.cc
 * \brief Optimized math routines for signed 8 bit integer operations on
 *        the ppc 450 dual fpu architecture.
 */
#include "math_coremath.h"
#include "Util.h"
#include "ppc450d/internal_o.h"

void _pami_core_int8_max2(int8_t *dst, const int8_t **srcs, int nsrc, int count) {

  int8_t *dp = (int8_t *)dst;
  const int8_t *s0 = (const int8_t *)srcs[0];
  const int8_t *s1 = (const int8_t *)srcs[1];

  int n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "lbz     5,0(%[s0]);"

      "extsb   5,5;"
      "lbz     6,1(%[s0]);"

      "extsb   6,6;"
      "lbz     7,2(%[s0]);"

      "extsb   7,7;"
      "lbz     8,3(%[s0]);"

      "extsb   8,8;"
      "lbz     9,0(%[s1]);"

      "extsb   9,9;"
      "lbz     10,1(%[s1]);"

      "extsb   10,10;"
      "lbz     11,2(%[s1]);"

      "extsb   11,11;"
      "lbz     12,3(%[s1]);"

      "extsb   12,12;"


      "cmpw    5,9;"
      "bge     0f;"
      "mr      5,9;"
"0:    stb     5,0(%[dp]);"

      "cmpw    6,10;"
      "bge     1f;"
      "mr      6,10;"
"1:    stb     6,1(%[dp]);"

      "cmpw    7,11;"
      "bge     2f;"
      "mr      7,11;"
"2:    stb     7,2(%[dp]);"

      "cmpw    8,12;"
      "bge     3f;"
      "mr      8,12;"
"3:    stb     8,3(%[dp]);"

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

  register int8_t s0_r;
  register int8_t s1_r;
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

void _pami_core_int8_min2(int8_t *dst, const int8_t **srcs, int nsrc, int count) {

  int8_t *dp = (int8_t *)dst;
  const int8_t *s0 = (const int8_t *)srcs[0];
  const int8_t *s1 = (const int8_t *)srcs[1];

  int n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "lbz     5,0(%[s0]);"

      "extsb   5,5;"
      "lbz     6,1(%[s0]);"

      "extsb   6,6;"
      "lbz     7,2(%[s0]);"

      "extsb   7,7;"
      "lbz     8,3(%[s0]);"

      "extsb   8,8;"
      "lbz     9,0(%[s1]);"

      "extsb   9,9;"
      "lbz     10,1(%[s1]);"

      "extsb   10,10;"
      "lbz     11,2(%[s1]);"

      "extsb   11,11;"
      "lbz     12,3(%[s1]);"

      "extsb   12,12;"

      "cmpw    9,5;"
      "bge     0f;"
      "mr      5,9;"
"0:    stb     5,0(%[dp]);"

      "cmpw    10,6;"
      "bge     1f;"
      "mr      6,10;"
"1:    stb     6,1(%[dp]);"

      "cmpw    11,7;"
      "bge     2f;"
      "mr      7,11;"
"2:    stb     7,2(%[dp]);"

      "cmpw    12,8;"
      "bge     3f;"
      "mr      8,12;"
"3:    stb     8,3(%[dp]);"

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

void _pami_core_int8_prod2(int8_t *dst, const int8_t **srcs, int nsrc, int count) {

  int8_t *dp = (int8_t *)dst;
  const int8_t *s0 = (const int8_t *)srcs[0];
  const int8_t *s1 = (const int8_t *)srcs[1];

  int n = count >> 2;
  while ( n-- ) {
    asm volatile (

      "lbz     5,0(%[s0]);"

      "extsb   5,5;"
      "lbz     6,1(%[s0]);"

      "extsb   6,6;"
      "lbz     7,2(%[s0]);"

      "extsb   7,7;"
      "lbz     8,3(%[s0]);"

      "extsb   8,8;"
      "lbz     9,0(%[s1]);"

      "extsb   9,9;"
      "lbz     10,1(%[s1]);"

      "extsb   10,10;"
      "lbz     11,2(%[s1]);"

      "extsb   11,11;"
      "lbz     12,3(%[s1]);"

      "extsb   12,12;"


      "mullw   5,5,9;"

      "mullw   6,6,10;"
      "stb     5,0(%[dp]);"

      "mullw   7,7,11;"
      "stb     6,1(%[dp]);"

      "mullw   8,8,12;"
      "stb     7,2(%[dp]);"

      "stb     8,3(%[dp]);"

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

  register int8_t s0_r;
  register int8_t s1_r;


  n = count & 0x03;
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

void _pami_core_int8_sum2(int8_t *dst, const int8_t **srcs, int nsrc, int count) {

  const int8_t *s0 = (const int8_t *)srcs[0];
  const int8_t *s1 = (const int8_t *)srcs[1];
  int8_t *dp = (int8_t *)dst;

  size_t n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "lbz     5,0(%[s0]);"

      "extsb   5,5;"
      "lbz     6,1(%[s0]);"

      "extsb   6,6;"
      "lbz     7,2(%[s0]);"

      "extsb   7,7;"
      "lbz     8,3(%[s0]);"

      "extsb   8,8;"
      "lbz     9,0(%[s1]);"

      "extsb   9,9;"
      "lbz     10,1(%[s1]);"

      "extsb   10,10;"
      "lbz     11,2(%[s1]);"

      "extsb   11,11;"
      "lbz     12,3(%[s1]);"

      "extsb   12,12;"

      "add     5,5,9;"

      "add     6,6,10;"
      "stb     5,0(%[dp]);"

      "add     7,7,11;"
      "stb     6,1(%[dp]);"

      "add     8,8,12;"
      "stb     7,2(%[dp]);"

      "stb     8,3(%[dp]);"

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

  register int s0_r;
  register int s1_r;

  n = count & 0x07;
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

void _pami_core_int8_band2(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)&(b))

#define TYPE int8_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int8_bor2(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)|(b))

#define TYPE int8_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int8_bxor2(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)^(b))

#define TYPE int8_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int8_land2(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)&&(b))

#define TYPE int8_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int8_lor2(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)||(b))

#define TYPE int8_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int8_lxor2(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)&&(!b))||((!a)&&(b)))

#define TYPE int8_t
#include "_dual_src.x.h"
#undef TYPE
#undef OP
}
