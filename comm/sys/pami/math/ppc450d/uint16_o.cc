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
 * \file math/ppc450d/uint16_o.cc
 * \brief Optimized math routines for unsigned 16 bit integer operations on
 *        the ppc 450 dual fpu architecture.
 */
#include "math_coremath.h"
#include "Util.h"
#include "ppc450d/internal_o.h"

void _pami_core_uint16_band2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {

  const uint16_t *s[2] = {srcs[0],srcs[1]};
  uint16_t *dp = (uint16_t *)dst;

  if (count & 0x01) {
    (*dp) = (*s[0]) & (*s[1]);

    s[0]++;
    s[1]++;
    dp++;
  }
  int num = count >> 1;
  if (num) {
    _pami_core_uint32_band2((uint32_t *)dp, (const uint32_t **)s, 2, num);
  }
  return;
}

void _pami_core_uint16_bor2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {

  const uint16_t *s[2] = {srcs[0],srcs[1]};
  uint16_t *dp = (uint16_t *)dst;

  if (count & 0x01) {
    (*dp) = (*s[0]) | (*s[1]);

    s[0]++;
    s[1]++;
    dp++;
  }
  int num = count >> 1;
  if (num) {
    _pami_core_uint32_bor2((uint32_t *)dp, (const uint32_t **)s, 2, num);
  }
  return;
}

void _pami_core_uint16_bxor2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {

  const uint16_t *s[2] = {srcs[0],srcs[1]};
  uint16_t *dp = (uint16_t *)dst;

  if (count & 0x01) {
    (*dp) = (*s[0]) ^ (*s[1]);

    s[0]++;
    s[1]++;
    dp++;
  }
  int num = count >> 1;
  if (num) {
    _pami_core_uint32_bxor2((uint32_t *)dp, (const uint32_t **)s, 2, num);
  }
  return;
}

void _pami_core_uint16_land2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {

  uint16_t *dp = (uint16_t *)dst;
  const uint16_t *s0 = (const uint16_t *)srcs[0];
  const uint16_t *s1 = (const uint16_t *)srcs[1];

  int n = count >> 3;
  while ( n-- ) {
    asm volatile (
        "lhz     5,0(%[s0]);"

        "cmpwi   0,5,0;"
        "lhz     6,0(%[s1]);"

        "cmpwi   1,6,0;"

        "crnor   24,2,6;"
        "lhz     5,2(%[s0]);"

        "cmpwi   0,5,0;"
        "lhz     6,2(%[s1]);"

        "cmpwi   1,6,0;"

        "crnor   25,2,6;"
        "lhz     5,4(%[s0]);"

        "cmpwi   0,5,0;"
        "lhz     6,4(%[s1]);"

        "cmpwi   1,6,0;"

        "crnor   26,2,6;"
        "lhz     5,6(%[s0]);"

        "cmpwi   0,5,0;"
        "lhz     6,6(%[s1]);"

        "cmpwi   1,6,0;"

        "crnor   27,2,6;"
        "lhz     5,8(%[s0]);"

        "cmpwi   0,5,0;"
        "lhz     6,8(%[s1]);"

        "cmpwi   1,6,0;"

        "crnor   28,2,6;"
        "lhz     5,10(%[s0]);"

        "cmpwi   0,5,0;"
        "lhz     6,10(%[s1]);"

        "cmpwi   1,6,0;"

        "crnor   29,2,6;"
        "lhz     5,12(%[s0]);"

        "cmpwi   0,5,0;"
        "lhz     6,12(%[s1]);"

        "cmpwi   1,6,0;"

        "crnor   30,2,6;"
        "lhz     5,14(%[s0]);"

        "cmpwi   0,5,0;"
        "lhz     6,14(%[s1]);"

        "cmpwi   1,6,0;"

        "crnor   31,2,6;"


        "mfcr    5;"


        "rlwinm  6,5,25,31,31;"

        "sth     6,0(%[dp]);"
        "rlwinm  7,5,26,31,31;"

        "sth     7,2(%[dp]);"
        "rlwinm  6,5,27,31,31;"

        "sth     6,4(%[dp]);"
        "rlwinm  7,5,28,31,31;"

        "sth     7,6(%[dp]);"
        "rlwinm  6,5,29,31,31;"

        "sth     6,8(%[dp]);"
        "rlwinm  7,5,30,31,31;"

        "sth     7,10(%[dp]);"
        "rlwinm  6,5,31,31,31;"

        "sth     6,12(%[dp]);"
        "andi.   7,5,0x01;"

        "sth     7,14(%[dp]);"




      : // no outputs
      : [s0] "b" (s0),
        [s1] "b" (s1),
        [dp] "b" (dp)
      : "memory", "5", "6", "7"
    );

    s0 += 8;
    s1 += 8;
    dp += 8;
  }

  n = count & 0x07;
  while ( n-- ) {
    *dp = (*s0) && (*s1);
    dp++;
    s0++;
    s1++;
  }

  return;
}

void _pami_core_uint16_lor2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {

  const uint16_t *s0 = (const uint16_t *)srcs[0];
  const uint16_t *s1 = (const uint16_t *)srcs[1];
  uint16_t *dp = (uint16_t *)dst;

  int n = count >> 3;
  while ( n-- ) {
    asm volatile (
        "lhz    6,0(%[s0]);"

        "lhz    7,0(%[s1]);"

        "lhz    8,2(%[s0]);"
        "or.    5,6,7;"

        "crnot  24,2;"
        "lhz    9,2(%[s1]);"

        "or.    5,8,9;"
        "lhz    6,4(%[s0]);"

        "crnot  25,2;"
        "lhz    7,4(%[s1]);"

        "or.    5,6,7;"
        "lhz    8,6(%[s0]);"

        "crnot  26,2;"
        "lhz    9,6(%[s1]);"

        "or.    5,8,9;"
        "lhz    6,8(%[s0]);"

        "crnot  27,2;"
        "lhz    7,8(%[s1]);"

        "or.    5,6,7;"
        "lhz    8,10(%[s0]);"

        "crnot  28,2;"
        "lhz    9,10(%[s1]);"

        "or.    5,8,9;"
        "lhz    6,12(%[s0]);"

        "crnot  29,2;"
        "lhz    7,12(%[s1]);"

        "or.    5,6,7;"
        "lhz    8,14(%[s0]);"

        "crnot  30,2;"
        "lhz    9,14(%[s1]);"

        "or.    5,8,9;"

        "crnot  31,2;"


        "mfcr   5;"

        "rlwinm 6,5,25,31,31;"

        "sth    6,0(%[dp]);"
        "rlwinm 7,5,26,31,31;"

        "sth    7,2(%[dp]);"
        "rlwinm 8,5,27,31,31;"

        "sth    8,4(%[dp]);"
        "rlwinm 9,5,28,31,31;"

        "sth    9,6(%[dp]);"
        "rlwinm 6,5,29,31,31;"

        "sth    6,8(%[dp]);"
        "rlwinm 7,5,30,31,31;"

        "sth    7,10(%[dp]);"
        "rlwinm 8,5,31,31,31;"

        "sth    8,12(%[dp]);"
        "rlwinm 9,5,0,31,31;"

        "sth    9,14(%[dp]);"

      : // no outputs
      : [s0] "b" (s0),
        [s1] "b" (s1),
        [dp] "b" (dp)
      : "memory", "5",  "6",  "7",  "8",  "9"
        );
    s0 += 8;
    s1 += 8;
    dp += 8;
  }

  n = count & 0x07;
  while ( n-- ) {
    (*dp) = (*s0) || (*s1);

    s0++;
    s1++;
    dp++;
  }

  return;
}

void _pami_core_uint16_lxor2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {

  uint16_t *dp = (uint16_t *)dst;
  const uint16_t *s0 = (const uint16_t *)srcs[0];
  const uint16_t *s1 = (const uint16_t *)srcs[1];

  int n = count >> 3;
  while ( n-- ) {
    asm volatile (
        "lhz     5,0(%[s0]);"

        "cmpwi   0,5,0;"
        "lhz     6,0(%[s1]);"

        "cmpwi   1,6,0;"

        "crxor   24,2,6;"
        "lhz     5,2(%[s0]);"

        "cmpwi   0,5,0;"
        "lhz     6,2(%[s1]);"

        "cmpwi   1,6,0;"

        "crxor   25,2,6;"
        "lhz     5,4(%[s0]);"

        "cmpwi   0,5,0;"
        "lhz     6,4(%[s1]);"

        "cmpwi   1,6,0;"

        "crxor   26,2,6;"
        "lhz     5,6(%[s0]);"

        "cmpwi   0,5,0;"
        "lhz     6,6(%[s1]);"

        "cmpwi   1,6,0;"

        "crxor   27,2,6;"
        "lhz     5,8(%[s0]);"

        "cmpwi   0,5,0;"
        "lhz     6,8(%[s1]);"

        "cmpwi   1,6,0;"

        "crxor   28,2,6;"
        "lhz     5,10(%[s0]);"

        "cmpwi   0,5,0;"
        "lhz     6,10(%[s1]);"

        "cmpwi   1,6,0;"

        "crxor   29,2,6;"
        "lhz     5,12(%[s0]);"

        "cmpwi   0,5,0;"
        "lhz     6,12(%[s1]);"

        "cmpwi   1,6,0;"

        "crxor   30,2,6;"
        "lhz     5,14(%[s0]);"

        "cmpwi   0,5,0;"
        "lhz     6,14(%[s1]);"

        "cmpwi   1,6,0;"

        "crxor   31,2,6;"


        "mfcr    5;"



        "rlwinm  6,5,25,31,31;"
        "sth     6,0(%[dp]);"

        "rlwinm  7,5,26,31,31;"

        "sth     7,2(%[dp]);"
        "rlwinm  6,5,27,31,31;"

        "sth     6,4(%[dp]);"
        "rlwinm  7,5,28,31,31;"

        "sth     7,6(%[dp]);"
        "rlwinm  6,5,29,31,31;"

        "sth     6,8(%[dp]);"
        "rlwinm  7,5,30,31,31;"

        "sth     7,10(%[dp]);"
        "rlwinm  6,5,31,31,31;"

        "sth     6,12(%[dp]);"
        "andi.   7,5,0x01;"

        "sth     7,14(%[dp]);"




      : // no outputs
      : [s0] "b" (s0),
        [s1] "b" (s1),
        [dp] "b" (dp)
      : "memory", "5", "6", "7"
    );

    s0 += 8;
    s1 += 8;
    dp += 8;
  }

  n = count & 0x07;
  register uint16_t s0_r;
  register uint16_t s1_r;
  while ( n-- ) {

    s0_r = *s0;
    s1_r = *s1;

    *dp = (s0_r && !s1_r) || (!s0_r && s1_r);

    dp++;
    s0++;
    s1++;
  }

  return;
}

void _pami_core_uint16_max2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {

  uint16_t *dp = (uint16_t *)dst;
  const uint16_t *s0 = (const uint16_t *)srcs[0];
  const uint16_t *s1 = (const uint16_t *)srcs[1];

  int n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "lhz     5,0(%[s0]);"

      "lhz     9,0(%[s1]);"

      "lhz     6,2(%[s0]);"
      "cmpw    5,9;"

      "lhz     10,2(%[s1]);"
      "bge     0f;"

      "mr      5,9;"

"0:    sth     5,0(%[dp]);"

      "lhz     7,4(%[s0]);"

      "lhz     8,6(%[s0]);"
      "cmpw    6,10;"

      "lhz     11,4(%[s1]);"
      "bge     1f;"

      "mr      6,10;"

"1:    sth     6,2(%[dp]);"
      "cmpw    7,11;"

      "lhz     12,6(%[s1]);"
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

  register uint16_t s0_r;
  register uint16_t s1_r;
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

void _pami_core_uint16_min2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {

  uint16_t *dp = (uint16_t *)dst;
  const uint16_t *s0 = (const uint16_t *)srcs[0];
  const uint16_t *s1 = (const uint16_t *)srcs[1];

  int n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "lhz     5,0(%[s0]);"

      "lhz     9,0(%[s1]);"

      "lhz     6,2(%[s0]);"
      "cmpw    9,5;"

      "lhz     10,2(%[s1]);"
      "bge     0f;"

      "mr      5,9;"

"0: "
      "sth     5,0(%[dp]);"

      "lhz     7,4(%[s0]);"

      "lhz     11,4(%[s1]);"
      "cmpw    10,6;"

      "lhz     8,6(%[s0]);"
      "bge     1f;"

      "mr      6,10;"
"1: "

      "sth     6,2(%[dp]);"
      "cmpw    11,7;"

      "lhz     12,6(%[s1]);"
      "bge     2f;"

      "mr      7,11;"

"2: "
      "sth     7,4(%[dp]);"
      "cmpw    12,8;"

      "bge     3f;"

      "mr      8,12;"

"3: "
      "sth     8,6(%[dp]);"

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

  register uint16_t s0_r;
  register uint16_t s1_r;
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

void _pami_core_uint16_prod2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {

  uint16_t *dp = (uint16_t *)dst;
  const uint16_t *s0 = (const uint16_t *)srcs[0];
  const uint16_t *s1 = (const uint16_t *)srcs[1];

  int n = count >> 3;
  while ( n-- ) {
    asm volatile (
      "lwz     5,0(%[s0]);"

      "lwz     6,0(%[s1]);"

      "lwz     9,4(%[s0]);"
      "mulhhw  7,5,6;"

      "mullhw  8,5,6;"
      "sth     7,0(%[dp]);"

      "lwz    10,4(%[s1]);"

      "sth     8,2(%[dp]);"
      "mulhhw 11,9,10;"

      "mullhw 12,9,10;"
      "sth    11,4(%[dp]);"

      "sth    12,6(%[dp]);"


      "lwz     5,8(%[s0]);"

      "lwz     6,8(%[s1]);"

      "lwz     9,12(%[s0]);"
      "mulhhw  7,5,6;"

      "mullhw  8,5,6;"
      "sth     7,8(%[dp]);"

      "lwz    10,12(%[s1]);"

      "sth     8,10(%[dp]);"
      "mulhhw 11,9,10;"

      "mullhw 12,9,10;"
      "sth    11,12(%[dp]);"

      "sth    12,14(%[dp]);"


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

  register int s0_r;
  register int s1_r;

  n = count & 0x07;
  while ( n-- ) {
    s0_r = *s0;
    s1_r = *s1;
    *(dp+0) = s0_r * s1_r;

    dp++;
    s0++;
    s1++;
  }

  return;
}

void _pami_core_uint16_sum2(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {

  const uint16_t *s0 = (const uint16_t *)srcs[0];
  const uint16_t *s1 = (const uint16_t *)srcs[1];
  uint16_t *dp = (uint16_t *)dst;

  int n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "lhz     5,0(%[s0]);"

      "lhz     9,0(%[s1]);"

      "lhz     6,2(%[s0]);"

      "lhz     10,2(%[s1]);"

      "lhz     7,4(%[s0]);"
      "add     12,5,9;"

      "lhz     11,4(%[s1]);"

      "sth     12,0(%[dp]);"

      "add     5,6,10;"
      "lhz     8,6(%[s0]);"

      "sth     5,2(%[dp]);"
      "add     6,7,11;"

      "lhz     12,6(%[s1]);"

      "sth     6,4(%[dp]);"
      "add     7,8,12;"

      "sth     7,6(%[dp]);"
      : // no outputs
      : [s0] "b" (s0),
        [s1] "b" (s1),
        [dp] "b" (dp)
      : "memory",
        "5",  "6",  "7",  "8",  "9",  "10", "11", "12"
        );
    s0 += 4;
    s1 += 4;
    dp += 4;
  }

  register uint16_t s0_r;
  register uint16_t s1_r;

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
