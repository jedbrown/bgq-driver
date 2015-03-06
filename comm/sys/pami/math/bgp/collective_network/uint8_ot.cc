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
 * \file math/bgp/collective_network/uint8_ot.cc
 * \brief Optimized math routines for unsigned 8 bit integer operations on
 *        the ppc 450 dual fpu architecture.
 */
#include "pami_bg_math.h"
#include "util/common.h"
//#include "ppc450d/internal_o.h"

static void _pami_core_uint8_not_o(uint8_t *dst, const uint8_t *src, int count) {

  uint8_t *dp = (uint8_t *)dst;
  uint8_t *sp = (uint8_t *)src;

  int n = count & 0x03;
  while ( n-- ) {
    (*dp) = ~(*sp);
    sp++;
    dp++;
  }

  _pami_core_uint32_not_o((uint32_t *)dp, (const uint32_t *)sp, count>>2);
}

#ifdef NOT_USED
static void _pami_core_uint8_conv_o(uint8_t *dst, const uint8_t *src, int count) {

  uint8_t *dp = dst;
  const uint8_t *sp = src;
  int n = count >> 3;

  while ( n-- ) {
    asm volatile (
      "lbz     5,0(%[sp]);"

      "extsb   5,5;"
      "lbz     6,1(%[sp]);"

      "subi    5,5,0x80;"

      "stb     5,0(%[dp]);"
      "extsb   6,6;"

      "subi    6,6,0x80;"
      "lbz     7,2(%[sp]);"

      "stb     6,1(%[dp]);"
      "extsb   7,7;"

      "subi    7,7,0x80;"
      "lbz     8,3(%[sp]);"

      "stb     7,2(%[dp]);"
      "extsb   8,8;"

      "subi    8,8,0x80;"
      "lbz     5,4(%[sp]);"

      "stb     8,3(%[dp]);"
      "extsb   5,5;"

      "subi    5,5,0x80;"
      "lbz     6,5(%[sp]);"

      "stb     5,4(%[dp]);"
      "extsb   6,6;"

      "subi    6,6,0x80;"
      "lbz     7,6(%[sp]);"

      "stb     6,5(%[dp]);"
      "extsb   7,7;"

      "subi    7,7,0x80;"
      "lbz     8,7(%[sp]);"

      "stb     7,6(%[dp]);"
      "extsb   8,8;"

      "subi    8,8,0x80;"

      "stb     8,7(%[dp]);"

      : // no outputs
      : [sp] "b" (sp),
        [dp] "b" (dp)
      : "memory",
        "5",  "6",  "7",  "8"
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

static void _pami_core_uint8_conv_not_o(uint8_t *dst, const uint8_t *src, int count) {

  uint8_t *dp = dst;
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
        "5",  "6",  "7",  "8"
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
#endif /* NOT_USED */

void _pami_core_uint8_pre_min_o(uint8_t *dst, const uint8_t *src, int count) {
  _pami_core_uint8_not_o(dst, src, count);
}

void _pami_core_uint8_post_min_o(uint8_t *dst, const uint8_t *src, int count) {
  _pami_core_uint8_not_o(dst, src, count);
}
