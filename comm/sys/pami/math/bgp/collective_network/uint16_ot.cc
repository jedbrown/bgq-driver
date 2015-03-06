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
 * \file math/bgp/collective_network/uint16_ot.cc
 * \brief Optimized math routines for unsigned 16 bit integer operations on
 *        the ppc 450 dual fpu architecture.
 */
#include "pami_bg_math.h"
#include "util/common.h"
//#include "ppc450d/internal_o.h"

#ifdef NOT_USED
static void _pami_core_uint16_conv_o(uint16_t *dst, const uint16_t *src, int count) {

  uint16_t *dp = dst;
  const uint16_t *sp = src;

  register uint32_t add = 0x8000;
  int n = count >> 3;
  while ( n-- ) {
    asm volatile (
      "lha    5,0(%[sp]);"

      "sub    5,5,%[add];"
      "lha    6,2(%[sp]);"

      "sub    6,6,%[add];"
      "lha    7,4(%[sp]);"

      "sub    7,7,%[add];"
      "lha    8,6(%[sp]);"

      "sub    8,8,%[add];"
      "lha    9,8(%[sp]);"

      "sub    9,9,%[add];"
      "lha   10,10(%[sp]);"

      "sub   10,10,%[add];"
      "lha   11,12(%[sp]);"

      "lha   12,14(%[sp]);"
      "sub   11,11,%[add];"

      "sub   12,12,%[add];"
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
      "sub   12,12,%[add];"
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

static void _pami_core_uint16_conv_not_o(uint16_t *dst, const uint16_t *src, int count) {

  uint16_t *dp = dst;
  const uint16_t *sp = src;

  register uint32_t add = 0x8000;
  int n = count >> 3;
  while ( n-- ) {
    asm volatile (
      "lha    5,0(%[sp]);"

      "sub    5,5,%[add];"
      "lha    6,2(%[sp]);"

      "not    5,5;"
      "lha    7,4(%[sp]);"

      "sth    5,0(%[dp]);"
      "sub    6,6,%[add];"

      "not    6,6;"
      "lha    8,6(%[sp]);"

      "sth    6,2(%[dp]);"
      "sub    7,7,%[add];"

      "not    7,7;"
      "lha    9,8(%[sp]);"

      "sth    7,4(%[dp]);"
      "sub    8,8,%[add];"

      "not    8,8;"
      "lha   10,10(%[sp]);"

      "sth    8,6(%[dp]);"
      "sub    9,9,%[add];"

      "not    9,9;"
      "lha   11,12(%[sp]);"

      "sth    9,8(%[dp]);"
      "sub   10,10,%[add];"

      "not   10,10;"
      "lha   12,14(%[sp]);"

      "sth   10,10(%[dp]);"
      "sub   11,11,%[add];"

      "not   11,11;"

      "sth   11,12(%[dp]);"
      "sub   12,12,%[add];"

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
      "sub   12,12,%[add];"
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
#endif /* NOT_USED */

static void _pami_core_uint16_not_o(uint16_t *dst, const uint16_t *src, int count) {

  uint16_t *dp = (uint16_t *)dst;
  uint16_t *sp = (uint16_t *)src;

  if ( count & 0x01 ) {
    (*dp) = ~(*sp);
    sp++;
    dp++;
  }

  _pami_core_uint32_not_o((uint32_t *)dp, (const uint32_t *)sp, count>>1);
}

void _pami_core_uint16_pre_min_o(uint16_t *dst, const uint16_t *src, int count) {
  _pami_core_uint16_not_o(dst, src, count);
}

void _pami_core_uint16_post_min_o(uint16_t *dst, const uint16_t *src, int count) {
  _pami_core_uint16_not_o(dst, src, count);
}
