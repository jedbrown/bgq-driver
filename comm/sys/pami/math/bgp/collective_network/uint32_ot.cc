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
 * \file math/bgp/collective_network/uint32_ot.cc
 * \brief Optimized math routines for unsigned 32 bit integer operations on
 *        the ppc 450 dual fpu architecture.
 */
#include "pami_bg_math.h"
#include "util/common.h"
//#include "ppc450d/internal_o.h"

// This is internally exported for use by other uint sizes...
void _pami_core_uint32_not_o(uint32_t *dst, const uint32_t *src, int count) {

  const uint32_t *sp = (const uint32_t *)src;
  uint32_t *dp = (uint32_t *)dst;

  register uint32_t n = count >> 2;

  while ( n-- ) {
    asm volatile (
        "lwz    6,0(%[sp]);"

        "lwz    7,4(%[sp]);"
        "not    6,6;"

        "not    7,7;"
        "stw    6,0(%[dp]);"

        "stw    7,4(%[dp]);"

        "lwz    6,8(%[sp]);"

        "lwz    7,12(%[sp]);"
        "not    6,6;"

        "not    7,7;"
        "stw    6,8(%[dp]);"

        "stw    7,12(%[dp]);"

      : // no outputs
      : [sp] "b" (sp),
        [dp] "b" (dp)
      : "memory", "6",  "7"
      );

    sp+=4;
    dp+=4;
  }

  n = count & 0x03;
  while ( n-- ) {
    (*dp) = ~(*sp);
    sp++;
    dp++;
  }

  return;
}

#ifdef NOT_USED
static void _pami_core_uint32_conv_o(uint32_t *dst, const uint32_t *src, int count) {

  uint32_t *dp = dst;
  const uint32_t *sp = src;

  int n = count >> 3;
  while ( n-- ) {
    asm volatile (
      "lwz    5,0(%[sp]);"

      "lwz    6,4(%[sp]);"
      "subis  5,5,0x8000;"

      "lwz    7,8(%[sp]);"
      "subis  6,6,0x8000;"

      "lwz    8,12(%[sp]);"
      "subis  7,7,0x8000;"

      "lwz    9,16(%[sp]);"
      "subis  8,8,0x8000;"

      "lwz   10,20(%[sp]);"
      "subis  9,9,0x8000;"

      "lwz   11,24(%[sp]);"
      "subis 10,10,0x8000;"

      "lwz   12,28(%[sp]);"
      "subis 11,11,0x8000;"

      "subis 12,12,0x8000;"
      "stw    5,0(%[dp]);"

      "stw    6,4(%[dp]);"
      "stw    7,8(%[dp]);"
      "stw    8,12(%[dp]);"
      "stw    9,16(%[dp]);"
      "stw   10,20(%[dp]);"
      "stw   11,24(%[dp]);"
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

static void _pami_core_uint32_conv_not_o(uint32_t *dst, const uint32_t *src, int count) {

  uint32_t *dp = dst;
  const uint32_t *sp = src;

  int n = count >> 3;
  while ( n-- ) {
    asm volatile (
      "lwz    5,0(%[sp]);"

      "lwz    6,4(%[sp]);"
      "subis  5,5,0x8000;"

      "lwz    7,8(%[sp]);"
      "not    5,5;"

      "stw    5,0(%[dp]);"
      "subis  6,6,0x8000;"

      "not    6,6;"
      "stw    6,4(%[dp]);"

      "lwz    8,12(%[sp]);"
      "subis  7,7,0x8000;"

      "lwz    9,16(%[sp]);"
      "not    7,7;"

      "stw    7,8(%[dp]);"
      "subis  8,8,0x8000;"

      "lwz   10,20(%[sp]);"
      "not    8,8;"

      "stw    8,12(%[dp]);"
      "subis  9,9,0x8000;"

      "lwz   11,24(%[sp]);"
      "not    9,9;"

      "stw    9,16(%[dp]);"
      "subis 10,10,0x8000;"

      "lwz   12,28(%[sp]);"
      "not   10,10;"

      "stw   10,20(%[dp]);"
      "subis 11,11,0x8000;"

      "not   11,11;"

      "stw   11,24(%[dp]);"
      "subis 12,12,0x8000;"

      "not   12,12;"

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
           "subis 12,12,0x8000;"
           "not   12,12;"
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
#endif /* NOT_USED */

void _pami_core_uint32_pre_min_o(uint32_t *dst, const uint32_t *src, int count) {
  _pami_core_uint32_not_o(dst, src, count);
}

void _pami_core_uint32_post_min_o(uint32_t *dst, const uint32_t *src, int count) {
  _pami_core_uint32_not_o(dst, src, count);
}
