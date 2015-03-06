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
 * \file math/bgp/collective_network/int8_ot.cc
 * \brief Optimized math routines for signed 8 bit integer operations on
 *        the ppc 450 dual fpu architecture.
 */
#include "pami_bg_math.h"
#include "util/common.h"
//#include "ppc450d/internal_o.h"

#ifdef NOT_USED
static void _pami_core_int8_min_conv2(uint8_t *dst, const int8_t **srcs, int nsrc, int count) {

  uint8_t *dp = (uint8_t *)dst;
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
"0:    addi    5,5,0x80;"
      "stb     5,0(%[dp]);"

      "cmpw    10,6;"
      "bge     1f;"
      "mr      6,10;"
"1:    addi    6,6,0x80;"
      "stb     6,1(%[dp]);"

      "cmpw    11,7;"
      "bge     2f;"
      "mr      7,11;"
"2:    addi    7,7,0x80;"
      "stb     7,2(%[dp]);"

      "cmpw    12,8;"
      "bge     3f;"
      "mr      8,12;"
"3:    addi    8,8,0x80;"
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

  register int s0_r;
  register int s1_r;
  n = count & 0x03;
  while ( n-- ) {
    s0_r = *s0;
    s1_r = *s1;
    if (s1_r > s0_r) *(dp) = s0_r + 0x80;
    else *(dp) = s1_r + 0x80;

    s0++;
    s1++;
    dp++;
  }

  return;
}

static void _pami_core_int8_prod_to_tree2(uint8_t *dst, const int8_t **srcs, int nsrc, int count) {

  uint8_t *dp = (uint8_t *)dst;
  const int8_t *s0 = (const int8_t *)srcs[0];
  const int8_t *s1 = (const int8_t *)srcs[1];

  int n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "lwz     5,0(%[s0]);"
      "extsb   8,5;"
      "srawi   7,5,8;"
      "extsb   7,7;"
      "srawi   6,5,16;"
      "extsb   6,6;"
      "srawi   5,5,24;"

      "lwz     9,0(%[s1]);"
      "extsb   12,9;"
      "srawi   11,9,8;"
      "extsb   11,11;"
      "srawi   10,9,16;"
      "extsb   10,10;"
      "srawi   9,9,24;"

      "mullw   5,5,9;"
      "mullw   6,6,10;"
      "mullw   7,7,11;"
      "mullw   8,8,12;"

      "addi    5,5,0x80;"
      "addi    6,6,0x80;"
      "addi    7,7,0x80;"
      "addi    8,8,0x80;"

      "stb     5,0(%[dp]);"
      "stb     6,1(%[dp]);"
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
    *dp = (s0_r * s1_r) + 0x80;

    dp++;
    s0++;
    s1++;
  }

  return;
}
#endif /* NOT_USED */

static void _pami_core_int8_conv_o(uint8_t *dst, const int8_t *src, int count) {

  uint8_t *dp = dst;
  const int8_t *sp = src;
  int n = count >> 3;

  while ( n-- ) {
    asm volatile (

      "lbz     5,0(%[sp]);"

      "extsb   5,5;"
      "lbz     6,1(%[sp]);"

      "extsb   6,6;"
      "lbz     7,2(%[sp]);"

      "extsb   7,7;"
      "lbz     8,3(%[sp]);"

      "extsb   8,8;"

      "addi   5,5,0x80;"

      "addi   6,6,0x80;"
      "stb    5,0(%[dp]);"

      "addi   7,7,0x80;"
      "stb    6,1(%[dp]);"

      "addi   8,8,0x80;"
      "stb    7,2(%[dp]);"

      "stb    8,3(%[dp]);"


      "lbz     5,4(%[sp]);"

      "extsb   5,5;"
      "lbz     6,5(%[sp]);"

      "extsb   6,6;"
      "lbz     7,6(%[sp]);"

      "extsb   7,7;"
      "lbz     8,7(%[sp]);"

      "extsb   8,8;"

      "addi   5,5,0x80;"

      "addi   6,6,0x80;"
      "stb    5,4(%[dp]);"

      "addi   7,7,0x80;"
      "stb    6,5(%[dp]);"

      "addi   8,8,0x80;"
      "stb    7,6(%[dp]);"

      "stb    8,7(%[dp]);"

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
      "lbz   12,0(%[sp]);"
      "extsb 12,12;"
      "addi  12,12,0x80;"
      "stb   12,0(%[dp]);"

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

static void _pami_core_int8_conv_not_o(uint8_t *dst, const int8_t *src, int count) {

  uint8_t *dp = dst;
  const int8_t *sp = src;
  int n = count >> 3;

  while ( n-- ) {
    asm volatile (

      "lbz     5,0(%[sp]);"

      "extsb   5,5;"

      "addi    5,5,0x80;"

      "not     5,5;"
      "lbz     6,1(%[sp]);"

      "stb     5,0(%[dp]);"
      "extsb   6,6;"

      "addi    6,6,0x80;"

      "not     6,6;"
      "lbz     7,2(%[sp]);"

      "stb     6,1(%[dp]);"
      "extsb   7,7;"

      "addi    7,7,0x80;"

      "not     7,7;"
      "lbz     8,3(%[sp]);"

      "stb     7,2(%[dp]);"
      "extsb   8,8;"

      "addi    8,8,0x80;"

      "not     8,8;"
      "lbz     5,4(%[sp]);"

      "stb     8,3(%[dp]);"
      "extsb   5,5;"

      "addi    5,5,0x80;"

      "not     5,5;"
      "lbz     6,5(%[sp]);"

      "stb     5,4(%[dp]);"
      "extsb   6,6;"

      "addi    6,6,0x80;"

      "not     6,6;"
      "lbz     7,6(%[sp]);"

      "stb     6,5(%[dp]);"
      "extsb   7,7;"

      "addi    7,7,0x80;"

      "not     7,7;"
      "lbz     8,7(%[sp]);"

      "stb     7,6(%[dp]);"
      "extsb   8,8;"

      "addi    8,8,0x80;"

      "not     8,8;"

      "stb     8,7(%[dp]);"


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
      "lbz   12,0(%[sp]);"
      "extsb 12,12;"
      "addi  12,12,0x80;"
      "not   12,12;"
      "stb   12,0(%[dp]);"

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

static void _pami_core_int8_unconv_o(int8_t *dst, const uint8_t *src, int count) {

  int8_t *dp = dst;
  const uint8_t *sp = src;
  int n = count >> 3;

  while ( n-- ) {
    asm volatile (

      "lbz     5,0(%[sp]);"

      "extsb   5,5;"
      "lbz     6,1(%[sp]);"

      "extsb   6,6;"
      "lbz     7,2(%[sp]);"

      "extsb   7,7;"
      "lbz     8,3(%[sp]);"

      "extsb   8,8;"

      "subi   5,5,0x80;"

      "subi   6,6,0x80;"
      "stb    5,0(%[dp]);"

      "subi   7,7,0x80;"
      "stb    6,1(%[dp]);"

      "subi   8,8,0x80;"
      "stb    7,2(%[dp]);"

      "stb    8,3(%[dp]);"


      "lbz     5,4(%[sp]);"

      "extsb   5,5;"
      "lbz     6,5(%[sp]);"

      "extsb   6,6;"
      "lbz     7,6(%[sp]);"

      "extsb   7,7;"
      "lbz     8,7(%[sp]);"

      "extsb   8,8;"

      "subi   5,5,0x80;"

      "subi   6,6,0x80;"
      "stb    5,4(%[dp]);"

      "subi   7,7,0x80;"
      "stb    6,5(%[dp]);"

      "subi   8,8,0x80;"
      "stb    7,6(%[dp]);"

      "stb    8,7(%[dp]);"

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
      "lbz   12,0(%[sp]);"
      "extsb 12,12;"
      "subi  12,12,0x80;"
      "stb   12,0(%[dp]);"

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

static void _pami_core_int8_unconv_not_o(int8_t *dst, const uint8_t *src, int count) {

  int8_t *dp = dst;
  const uint8_t *sp = src;
  int n = count >> 3;

  while ( n-- ) {
    asm volatile (

      "lbz     5,0(%[sp]);"

      "extsb   5,5;"

      "subi    5,5,0x80;"

      "not     5,5;"
      "lbz     6,1(%[sp]);"

      "stb     5,0(%[dp]);"
      "extsb   6,6;"

      "subi    6,6,0x80;"

      "not     6,6;"
      "lbz     7,2(%[sp]);"

      "stb     6,1(%[dp]);"
      "extsb   7,7;"

      "subi    7,7,0x80;"

      "not     7,7;"
      "lbz     8,3(%[sp]);"

      "stb     7,2(%[dp]);"
      "extsb   8,8;"

      "subi    8,8,0x80;"

      "not     8,8;"
      "lbz     5,4(%[sp]);"

      "stb     8,3(%[dp]);"
      "extsb   5,5;"

      "subi    5,5,0x80;"

      "not     5,5;"
      "lbz     6,5(%[sp]);"

      "stb     5,4(%[dp]);"
      "extsb   6,6;"

      "subi    6,6,0x80;"

      "not     6,6;"
      "lbz     7,6(%[sp]);"

      "stb     6,5(%[dp]);"
      "extsb   7,7;"

      "subi    7,7,0x80;"

      "not     7,7;"
      "lbz     8,7(%[sp]);"

      "stb     7,6(%[dp]);"
      "extsb   8,8;"

      "subi    8,8,0x80;"

      "not     8,8;"

      "stb     8,7(%[dp]);"


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
      "lbz   12,0(%[sp]);"
      "extsb 12,12;"
      "subi  12,12,0x80;"
      "not   12,12;"
      "stb   12,0(%[dp]);"

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

void _pami_core_int8_pre_all_o(uint8_t *dst, const int8_t *src, int count) {
  _pami_core_int8_conv_o(dst, src, count);
}

void _pami_core_int8_post_all_o(int8_t *dst, const uint8_t *src, int count) {
  _pami_core_int8_unconv_o(dst, src, count);
}

void _pami_core_int8_pre_min_o(uint8_t *dst, const int8_t *src, int count) {
  _pami_core_int8_conv_not_o(dst, src, count);
}

void _pami_core_int8_post_min_o(int8_t *dst, const uint8_t *src, int count) {
  _pami_core_int8_unconv_not_o(dst, src, count);
}
