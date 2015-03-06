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
 * \file math/bgp/collective_network/int32_ot.cc
 * \brief Optimized math routines for signed 32 bit integer operations on
 *        the ppc 450 dual fpu architecture.
 */
#include "pami_bg_math.h"
#include "util/common.h"
//#include "ppc450d/internal_o.h"

#ifdef NOT_USED
static void _pami_core_int32_min_conv2(uint32_t *dst, const int32_t **srcs, int nsrc, int count) {

  uint32_t *dp = (uint32_t *)dst;
  const int32_t *s0 = (const int32_t *)srcs[0];
  const int32_t *s1 = (const int32_t *)srcs[1];

  int n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "lwz     5,0(%[s0]);"

      "lwz     9,0(%[s1]);"

      "lwz     6,4(%[s0]);"
      "cmpw    9,5;"

      "lwz     10,4(%[s1]);"
      "bge     0f;"

      "mr      5,9;"

"0:    addis   5,5,0x8000;"
      "lwz     7,8(%[s0]);"

      "stw     5,0(%[dp]);"
      "cmpw    10,6;"

      "lwz     11,8(%[s1]);"
      "bge     1f;"

      "mr      6,10;"

"1:    addis   6,6,0x8000;"
      "lwz     8,12(%[s0]);"

      "stw     6,4(%[dp]);"
      "cmpw    11,7;"

      "lwz     12,12(%[s1]);"
      "bge     2f;"

      "mr      7,11;"

"2:    addis   7,7,0x8000;"

      "stw     7,8(%[dp]);"
      "cmpw    12,8;"

      "bge     3f;"

      "mr      8,12;"

"3:    addis   8,8,0x8000;"

      "stw     8,12(%[dp]);"

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
    if (s1_r > s0_r) *(dp) = s0_r + 0x80000000;
    else *(dp) = s1_r + 0x80000000;

    s0++;
    s1++;
    dp++;
  }

  return;
}

static void _pami_core_int32_prod_to_tree2(uint32_t *dst, const int32_t **srcs, int nsrc, int count) {

  uint32_t *dp = (uint32_t *)dst;
  const int32_t *s0 = (const int32_t *)srcs[0];
  const int32_t *s1 = (const int32_t *)srcs[1];

  register int32_t u asm("r5") = count >> 3;

  asm volatile (
"0:      lswi   6,%[s0],28;"
        "lswi  15,%[s1],32;"
        "lwz   14,28(%[s0]);"

        "mullw 15,15,6;"
        "mullw 16,16,7;"
        "mullw 17,17,8;"
        "mullw 18,18,9;"
        "mullw 19,19,10;"
        "mullw 20,20,11;"
        "mullw 21,21,12;"
        "mullw 22,22,14;"

        "addis 15,15,0x8000;"
        "addis 16,16,0x8000;"
        "addis 17,17,0x8000;"
        "addis 18,18,0x8000;"
        "addis 19,19,0x8000;"
        "addis 20,20,0x8000;"
        "addis 21,21,0x8000;"
        "addis 22,22,0x8000;"

        "stswi 15,%[dp],32;"
        "addi  %[s0],%[s0],32;"
        "addi  %[s1],%[s1],32;"
        "addi  %[dp],%[dp],32;"
        "addi 5,5,-1;"
        "cmpwi 5,0;"
        "bne   0b;"
      : [s0] "+b" (s0),
        [s1] "+b" (s1),
        [dp] "+b" (dp)
      : "r" (u)
      : "memory",
        "6",  "7",  "8",  "9",  "10", "11", "12", "14",
        "15", "16", "17", "18", "19", "20", "21", "22"
        );

  register int32_t s0_r;
  register int32_t s1_r;

  u = count & 0x07;
  while ( u-- ) {
    s0_r = *s0;
    s1_r = *s1;
    *dp = (s0_r * s1_r) + 0x80000000;

    s0++;
    s1++;
    dp++;
  }

  return;
}
#endif /* NOT_USED */

static void _pami_core_int32_conv_o(uint32_t *dst, const int32_t *src, int count) {

  uint32_t *dp = dst;
  const int32_t *sp = src;

  int n = count >> 3;
  while ( n-- ) {
    asm volatile (
      "lwz    5,0(%[sp]);"

      "lwz    6,4(%[sp]);"
      "addis  5,5,0x8000;"

      "stw    5,0(%[dp]);"

      "lwz    7,8(%[sp]);"
      "addis  6,6,0x8000;"

      "stw    6,4(%[dp]);"

      "lwz    8,12(%[sp]);"
      "addis  7,7,0x8000;"

      "stw    7,8(%[dp]);"

      "lwz    9,16(%[sp]);"
      "addis  8,8,0x8000;"

      "stw    8,12(%[dp]);"

      "lwz   10,20(%[sp]);"
      "addis  9,9,0x8000;"

      "stw    9,16(%[dp]);"

      "lwz   11,24(%[sp]);"
      "addis 10,10,0x8000;"

      "stw   10,20(%[dp]);"

      "lwz   12,28(%[sp]);"
      "addis 11,11,0x8000;"

      "stw   11,24(%[dp]);"
      "addis 12,12,0x8000;"

      "stw   12,28(%[dp]);"

      : // no outputs
      : [sp] "b" (sp),
        [dp] "b" (dp)
      : "memory",
        "5",  "6",  "7",  "8",  "9",  "10", "11", "12"
    );

    sp += 8;
    dp += 8;
  }
  n = count & 0x07;
  while ( n-- ) {
    asm volatile (
           "lwz   12,0(%[sp]);"
           "addis  12,12,0x8000;"
           "stw   12,0(%[dp]);"

          : // no outputs
          : [sp] "b" (sp),
            [dp] "b" (dp)
          : "memory", "12"
            );
    sp++;
    dp++;
  }

  return;
}

static void _pami_core_int32_conv_not_o(uint32_t *dst, const int32_t *src, int count) {
#define OP(a) (~((a)+(0x80000000UL)))

#define TYPE uint32_t
#include "math/_single_src.x.h"
#undef TYPE
#undef OP
}

static void _pami_core_int32_unconv_o(int32_t *dst, const uint32_t *src, int count) {

  int32_t *dp = dst;
  const uint32_t *sp = src;

  int n = count >> 3;
  while ( n-- ) {
    asm volatile (
      "lwz    5,0(%[sp]);"

      "lwz    6,4(%[sp]);"
      "subis  5,5,0x8000;"

      "stw    5,0(%[dp]);"

      "lwz    7,8(%[sp]);"
      "subis  6,6,0x8000;"

      "stw    6,4(%[dp]);"

      "lwz    8,12(%[sp]);"
      "subis  7,7,0x8000;"

      "stw    7,8(%[dp]);"

      "lwz    9,16(%[sp]);"
      "subis  8,8,0x8000;"

      "stw    8,12(%[dp]);"

      "lwz   10,20(%[sp]);"
      "subis  9,9,0x8000;"

      "stw    9,16(%[dp]);"

      "lwz   11,24(%[sp]);"
      "subis 10,10,0x8000;"

      "stw   10,20(%[dp]);"

      "lwz   12,28(%[sp]);"
      "subis 11,11,0x8000;"

      "stw   11,24(%[dp]);"
      "subis 12,12,0x8000;"

      "stw   12,28(%[dp]);"

      : // no outputs
      : [sp] "b" (sp),
        [dp] "b" (dp)
      : "memory",
        "5",  "6",  "7",  "8",  "9",  "10", "11", "12"
    );

    sp += 8;
    dp += 8;
  }
  n = count & 0x07;
  while ( n-- ) {
    asm volatile (
           "lwz   12,0(%[sp]);"
           "subis  12,12,0x8000;"
           "stw   12,0(%[dp]);"

          : // no outputs
          : [sp] "b" (sp),
            [dp] "b" (dp)
          : "memory", "12"
            );
    sp++;
    dp++;
  }

  return;
}

static void _pami_core_int32_unconv_not_o(int32_t *dst, const uint32_t *src, int count) {
#define OP(a) (~((a)-(0x80000000UL)))

#define TYPE uint32_t
#include "math/_single_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int32_pre_all_o(uint32_t *dst, const int32_t *src, int count) {
        _pami_core_int32_conv_o(dst, src, count);
}

void _pami_core_int32_post_all_o(int32_t *dst, const uint32_t *src, int count) {
        _pami_core_int32_unconv_o(dst, src, count);
}

void _pami_core_int32_pre_min_o(uint32_t *dst, const int32_t *src, int count) {
  _pami_core_int32_conv_not_o(dst, src, count);
}

void _pami_core_int32_post_min_o(int32_t *dst, const uint32_t *src, int count) {
        _pami_core_int32_unconv_not_o(dst, src, count);
}

void _pami_core_int32_int32_pre_maxloc_o(uint32_int32_t *dst, const int32_int32_t *src, int count) {
  register int n = 0;
  register unsigned shift = 0x80000000UL;
  for (n = 0; n < count; n++)
    {
      dst[n].a = src[n].a + shift;
      dst[n].b = LOC_INT_TOTREE(src[n].b);
    }
}

void _pami_core_int32_int32_post_maxloc_o(int32_int32_t *dst, const uint32_int32_t *src, int count) {
  register int n = 0;
  register unsigned shift = 0x80000000UL;
  for (n = 0; n < count; n++)
    {
      dst[n].a = src[n].a - shift;
      dst[n].b = LOC_INT_FRTREE(src[n].b);
    }
}

void _pami_core_int32_int32_pre_minloc_o(uint32_int32_t *dst, const int32_int32_t *src, int count) {
  register int n = 0;
  register unsigned shift = 0x80000000UL;
  for (n = 0; n < count; n++)
    {
      dst[n].a = ~(src[n].a + shift);
      dst[n].b = LOC_INT_TOTREE(src[n].b);
    }
}

void _pami_core_int32_int32_post_minloc_o(int32_int32_t *dst, const uint32_int32_t *src, int count) {
  register int n = 0;
  register unsigned shift = 0x80000000UL;
  for (n = 0; n < count; n++)
    {
      dst[n].a = ~(src[n].a - shift);
      dst[n].b = LOC_INT_FRTREE(src[n].b);
    }
}
