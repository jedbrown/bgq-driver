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
 * \file math/bgp/collective_network/int16_ot.cc
 * \brief Optimized math routines for signed 16 bit integer operations on
 *        the ppc 450 dual fpu architecture.
 */
#include "pami_bg_math.h"
#include "util/common.h"
//#include "ppc450d/internal_o.h"

#ifdef NOT_USED
static void _pami_core_int16_min_conv2(uint16_t *dst, const int16_t **srcs, int nsrc, int count) {

  uint16_t *dp = (uint16_t *)dst;
  const int16_t *s0 = (const int16_t *)srcs[0];
  const int16_t *s1 = (const int16_t *)srcs[1];

  int n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "addis  14,0,0;"
      "lha     5,0(%[s0]);"

      "ori    14,14,0x8000;"
      "lha     9,0(%[s1]);"

      "lha     6,2(%[s0]);"
      "cmpw    9,5;"

      "lha     10,2(%[s1]);"
      "bge     0f;"

      "mr      5,9;"

"0:    add   5,5,14;"

      "sth     5,0(%[dp]);"
      "cmpw    10,6;"


      "lha     7,4(%[s0]);"
      "bge     1f;"

      "mr      6,10;"

"1:    add   6,6,14;"
      "lha     11,4(%[s1]);"

      "sth     6,2(%[dp]);"
      "cmpw    11,7;"

      "lha     8,6(%[s0]);"
      "bge     2f;"

      "mr      7,11;"

"2:    add   7,7,14;"
      "lha     12,6(%[s1]);"

      "sth     7,4(%[dp]);"
      "cmpw    12,8;"

      "bge     3f;"

      "mr      8,12;"

"3:    add   8,8,14;"

      "sth     8,6(%[dp]);"

      : // no outputs
      : [s0] "b" (s0),
        [s1] "b" (s1),
        [dp] "b" (dp)
      : "memory",
        "5",  "6",  "7",  "8",  "9",  "10", "11", "12", "14"
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
    if (s1_r > s0_r) *(dp) = s0_r + 0x08000;
    else *(dp) = s1_r + 0x08000;

    s0++;
    s1++;
    dp++;
  }

  return;
}

static void _pami_core_int16_prod_conv2(uint16_t *dst, const int16_t **srcs, int nsrc, int count) {

  uint16_t *dp = (uint16_t *)dst;
  const int16_t *s0 = (const int16_t *)srcs[0];
  const int16_t *s1 = (const int16_t *)srcs[1];

  register uint32_t add = 0x8000;
  int n = count >> 3;
  while ( n-- ) {
    asm volatile (
      "lwz     5,0(%[s0]);"
      "mr      7,%[add];"

      "lwz     6,0(%[s1]);"
      "mr      8,%[add];"

      "machhwu 7,5,6;"

      "maclhwu 8,5,6;"
      "sth     7,0(%[dp]);"

      "sth     8,2(%[dp]);"


      "lwz     5,4(%[s0]);"
      "mr      7,%[add];"

      "lwz     6,4(%[s1]);"
      "mr      8,%[add];"

      "machhwu 7,5,6;"

      "maclhwu 8,5,6;"
      "sth     7,4(%[dp]);"

      "sth     8,6(%[dp]);"


      "lwz     5,8(%[s0]);"
      "mr      7,%[add];"

      "lwz     6,8(%[s1]);"
      "mr      8,%[add];"

      "machhwu 7,5,6;"

      "maclhwu 8,5,6;"
      "sth     7,8(%[dp]);"

      "sth     8,10(%[dp]);"


      "lwz     5,12(%[s0]);"
      "mr      7,%[add];"

      "lwz     6,12(%[s1]);"
      "mr      8,%[add];"

      "machhwu 7,5,6;"

      "maclhwu 8,5,6;"
      "sth     7,12(%[dp]);"

      "sth     8,14(%[dp]);"



    /*
      "addis  9,0,0;"
      "ori    9,9,0x8000;"

      "lwz     5,0(%[s0]);"
      "lwz     6,0(%[s1]);"
      "mulhhw  7,5,6;"
      "mullhw  8,5,6;"
      "add     7,7,9;"
      "add     8,8,9;"
      "sth     7,0(%[dp]);"
      "sth     8,2(%[dp]);"

      "lwz     5,4(%[s0]);"
      "lwz     6,4(%[s1]);"
      "mulhhw  7,5,6;"
      "mullhw  8,5,6;"
      "add     7,7,9;"
      "add     8,8,9;"
      "sth     7,4(%[dp]);"
      "sth     8,6(%[dp]);"

      "lwz     5,8(%[s0]);"
      "lwz     6,8(%[s1]);"
      "mulhhw  7,5,6;"
      "mullhw  8,5,6;"
      "add     7,7,9;"
      "add     8,8,9;"
      "sth     7,8(%[dp]);"
      "sth     8,10(%[dp]);"

      "lwz     5,12(%[s0]);"
      "lwz     6,12(%[s1]);"
      "mulhhw  7,5,6;"
      "mullhw  8,5,6;"
      "add     7,7,9;"
      "add     8,8,9;"
      "sth     7,12(%[dp]);"
      "sth     8,14(%[dp]);"*/

      : // no outputs
      : [s0] "b" (s0),
        [s1] "b" (s1),
        [dp] "b" (dp),
        [add] "r" (add)
      : "memory",
        "5",  "6",  "7",  "8",  "9"
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
    *dp = (s0_r * s1_r) + 0x08000;

    dp++;
    s0++;
    s1++;
  }

  return;
}
#endif /* NOT_USED */

static void _pami_core_int16_conv_o(uint16_t *dst, const int16_t *src, int count) {

  uint16_t *dp = dst;
  const int16_t *sp = src;
  int n = count >> 3;

  register uint32_t add = 0x8000;
  while ( n-- ) {
    asm volatile (
      "lha    5,0(%[sp]);"

      "add    5,5,%[add];"
      "lha    6,2(%[sp]);"

      "add    6,6,%[add];"
      "lha    7,4(%[sp]);"

      "add    7,7,%[add];"
      "lha    8,6(%[sp]);"

      "add    8,8,%[add];"
      "lha    9,8(%[sp]);"

      "add    9,9,%[add];"
      "lha   10,10(%[sp]);"

      "add   10,10,%[add];"
      "lha   11,12(%[sp]);"

      "add   11,11,%[add];"
      "lha   12,14(%[sp]);"

      "add   12,12,%[add];"
      "sth    5,0(%[dp]);"

      "sth    6,2(%[dp]);"

      "sth    7,4(%[dp]);"

      "sth    8,6(%[dp]);"

      "sth    9,8(%[dp]);"

      "sth   10,10(%[dp]);"

      "sth   11,12(%[dp]);"

      "sth   12,14(%[dp]);"

      : // no outputs
      : [sp] "b" (sp),
        [dp] "b" (dp),
        [add] "r" (add)
      : "memory",
        "5",  "6",  "7",  "8",  "9",  "10", "11", "12"
    );

    sp += 8;
    dp += 8;
  }

  n = count & 0x07;
  while ( n-- ) {
    asm volatile (
      "lha   12,0(%[sp]);"
      "add   12,12,%[add];"
      "sth   12,0(%[dp]);"

      : // no outputs
      : [sp] "b" (sp),
        [dp] "b" (dp),
        [add] "r" (add)
      : "memory", "12"
    );

    sp++;
    dp++;
  }

  return;
}

static void _pami_core_int16_unconv_o(int16_t *dst, const uint16_t *src, int count) {

  int16_t *dp = dst;
  const uint16_t *sp = src;
  int n = count >> 3;

  register uint32_t sub = 0x8000;
  while ( n-- ) {
    asm volatile (
      "lha    5,0(%[sp]);"

      "sub    5,5,%[sub];"
      "lha    6,2(%[sp]);"

      "sub    6,6,%[sub];"
      "lha    7,4(%[sp]);"

      "sub    7,7,%[sub];"
      "lha    8,6(%[sp]);"

      "sub    8,8,%[sub];"
      "lha    9,8(%[sp]);"

      "sub    9,9,%[sub];"
      "lha   10,10(%[sp]);"

      "sub   10,10,%[sub];"
      "lha   11,12(%[sp]);"

      "sub   11,11,%[sub];"
      "lha   12,14(%[sp]);"

      "sub   12,12,%[sub];"
      "sth    5,0(%[dp]);"

      "sth    6,2(%[dp]);"

      "sth    7,4(%[dp]);"

      "sth    8,6(%[dp]);"

      "sth    9,8(%[dp]);"

      "sth   10,10(%[dp]);"

      "sth   11,12(%[dp]);"

      "sth   12,14(%[dp]);"

      : // no outputs
      : [sp] "b" (sp),
        [dp] "b" (dp),
        [sub] "r" (sub)
      : "memory",
        "5",  "6",  "7",  "8",  "9",  "10", "11", "12"
    );

    sp += 8;
    dp += 8;
  }

  n = count & 0x07;
  while ( n-- ) {
    asm volatile (
      "lha   12,0(%[sp]);"
      "sub   12,12,%[sub];"
      "sth   12,0(%[dp]);"

      : // no outputs
      : [sp] "b" (sp),
        [dp] "b" (dp),
        [sub] "r" (sub)
      : "memory", "12"
    );

    sp++;
    dp++;
  }

  return;
}

static void _pami_core_int16_conv_not_o(uint16_t *dst, const int16_t *src, int count) {

  uint16_t *dp = dst;
  const int16_t *sp = src;
  int n = count >> 3;

  register uint32_t add = 0x8000;
  while ( n-- ) {
    asm volatile (
      "lha    5,0(%[sp]);"

      "add    5,5,%[add];"
      "lha    6,2(%[sp]);"

      "not    5,5;"
      "lha    7,4(%[sp]);"

      "sth    5,0(%[dp]);"
      "add    6,6,%[add];"

      "not    6,6;"
      "lha    8,6(%[sp]);"

      "sth    6,2(%[dp]);"
      "add    7,7,%[add];"

      "not    7,7;"
      "lha    9,8(%[sp]);"

      "sth    7,4(%[dp]);"
      "add    8,8,%[add];"

      "not    8,8;"
      "lha   10,10(%[sp]);"

      "sth    8,6(%[dp]);"
      "add    9,9,%[add];"

      "not    9,9;"
      "lha   11,12(%[sp]);"

      "sth    9,8(%[dp]);"
      "add   10,10,%[add];"

      "not   10,10;"
      "lha   12,14(%[sp]);"

      "sth   10,10(%[dp]);"
      "add   11,11,%[add];"

      "not   11,11;"

      "sth   11,12(%[dp]);"
      "add   12,12,%[add];"

      "not   12,12;"

      "sth   12,14(%[dp]);"

      : // no outputs
      : [sp] "b" (sp),
        [dp] "b" (dp),
        [add] "r" (add)
      : "memory",
        "5",  "6",  "7",  "8",  "9",  "10", "11", "12"
    );

    sp += 8;
    dp += 8;
  }

  n = count & 0x07;
  while ( n-- ) {
    asm volatile (
      "lha   12,0(%[sp]);"
      "add   12,12,%[add];"
      "not   12,12;"
      "sth   12,0(%[dp]);"

      : // no outputs
      : [sp] "b" (sp),
        [dp] "b" (dp),
        [add] "r" (add)
      : "memory", "12"
    );

    sp++;
    dp++;
  }

  return;
}

static void _pami_core_int16_unconv_not_o(int16_t *dst, const uint16_t *src, int count) {

  int16_t *dp = dst;
  const uint16_t *sp = src;
  int n = count >> 3;

  register uint32_t sub = 0x8000;
  while ( n-- ) {
    asm volatile (
      "lha    5,0(%[sp]);"

      "sub    5,5,%[sub];"
      "lha    6,2(%[sp]);"

      "not    5,5;"
      "lha    7,4(%[sp]);"

      "sth    5,0(%[dp]);"
      "sub    6,6,%[sub];"

      "not    6,6;"
      "lha    8,6(%[sp]);"

      "sth    6,2(%[dp]);"
      "sub    7,7,%[sub];"

      "not    7,7;"
      "lha    9,8(%[sp]);"

      "sth    7,4(%[dp]);"
      "sub    8,8,%[sub];"

      "not    8,8;"
      "lha   10,10(%[sp]);"

      "sth    8,6(%[dp]);"
      "sub    9,9,%[sub];"

      "not    9,9;"
      "lha   11,12(%[sp]);"

      "sth    9,8(%[dp]);"
      "sub   10,10,%[sub];"

      "not   10,10;"
      "lha   12,14(%[sp]);"

      "sth   10,10(%[dp]);"
      "sub   11,11,%[sub];"

      "not   11,11;"

      "sth   11,12(%[dp]);"
      "sub   12,12,%[sub];"

      "not   12,12;"

      "sth   12,14(%[dp]);"

      : // no outputs
      : [sp] "b" (sp),
        [dp] "b" (dp),
        [sub] "r" (sub)
      : "memory",
        "5",  "6",  "7",  "8",  "9",  "10", "11", "12"
    );

    sp += 8;
    dp += 8;
  }

  n = count & 0x07;
  while ( n-- ) {
    asm volatile (
      "lha   12,0(%[sp]);"
      "sub   12,12,%[sub];"
      "not   12,12;"
      "sth   12,0(%[dp]);"

      : // no outputs
      : [sp] "b" (sp),
        [dp] "b" (dp),
        [sub] "r" (sub)
      : "memory", "12"
    );

    sp++;
    dp++;
  }

  return;
}

void _pami_core_int16_pre_all_o(uint16_t *dst, const int16_t *src, int count) {
        _pami_core_int16_conv_o(dst, src, count);
}

void _pami_core_int16_post_all_o(int16_t *dst, const uint16_t *src, int count) {
        _pami_core_int16_unconv_o(dst, src, count);
}

void _pami_core_int16_pre_min_o(uint16_t *dst, const int16_t *src, int count) {
        _pami_core_int16_conv_not_o(dst, src, count);
}

void _pami_core_int16_post_min_o(int16_t *dst, const uint16_t *src, int count) {
        _pami_core_int16_unconv_not_o(dst, src, count);
}

void _pami_core_int16_int32_pre_maxloc_o(uint16_int32_t *dst, const int16_int32_t *src, int count) {
  register int n = 0;
  register unsigned shift = 0x8000UL;
  for (n = 0; n < count; n++)
    {
      dst[n].a = src[n].a + shift;
      dst[n].z = 0;
      dst[n].b = LOC_INT_TOTREE(src[n].b);
    }
}

void _pami_core_int16_int32_post_maxloc_o(int16_int32_t *dst, const uint16_int32_t *src, int count) {
  register int n = 0;
  register unsigned shift = 0x8000UL;
  for (n = 0; n < count; n++)
    {
      dst[n].a = src[n].a - shift;
      dst[n].b = LOC_INT_FRTREE(src[n].b);
    }
}

void _pami_core_int16_int32_pre_minloc_o(uint16_int32_t *dst, const int16_int32_t *src, int count) {
  register int n = 0;
  register unsigned shift = 0x8000UL;
  for (n = 0; n < count; n++)
    {
      dst[n].a = ~(src[n].a + shift);
      dst[n].z = 0;
      dst[n].b = LOC_INT_TOTREE(src[n].b);
    }
}

void _pami_core_int16_int32_post_minloc_o(int16_int32_t *dst, const uint16_int32_t *src, int count) {
  register int n = 0;
  register unsigned shift = 0x8000UL;
  for (n = 0; n < count; n++)
    {
      dst[n].a = ~(src[n].a - shift);
      dst[n].b = LOC_INT_FRTREE(src[n].b);
    }
}
