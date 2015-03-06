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
 * \file math/ppc450d/uint8_o.cc
 * \brief Optimized math routines for unsigned 8 bit integer operations on
 *        the ppc 450 dual fpu architecture.
 */
#include "math_coremath.h"
#include "Util.h"
#include "ppc450d/internal_o.h"

void _pami_core_uint8_band2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {

  const uint8_t *s[2] = {srcs[0],srcs[1]};
  uint8_t *dp = (uint8_t *)dst;

  int n = count & 0x03;
  while (n--) {
    (*dp) = (*s[0]) & (*s[1]);
    s[0]++;
    s[1]++;
    dp++;
  }

  _pami_core_uint32_band2((uint32_t *)dp, (const uint32_t **)s, nsrc, (count >> 2));

  return;
}

void _pami_core_uint8_bor2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {

  const uint8_t *s[2] = {srcs[0],srcs[1]};
  uint8_t *dp = (uint8_t *)dst;

  int n = count & 0x03;
  while (n--) {
    (*dp) = (*s[0]) | (*s[1]);
    s[0]++;
    s[1]++;
    dp++;
  }

  _pami_core_uint32_bor2((uint32_t *)dp, (const uint32_t **)s, nsrc, (count >> 2));

  return;
}

void _pami_core_uint8_bxor2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {

  const uint8_t *s[2] = {srcs[0],srcs[1]};
  uint8_t *dp = (uint8_t *)dst;

  int n = count & 0x03;
  while (n--) {
    (*dp) = (*s[0]) ^ (*s[1]);
    s[0]++;
    s[1]++;
    dp++;
  }

  _pami_core_uint32_bxor2((uint32_t *)dp, (const uint32_t **)s, nsrc, (count >> 2));

  return;
}

void _pami_core_uint8_land2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {

  uint8_t *dp = (uint8_t *)dst;
  const uint8_t *s0 = (const uint8_t *)srcs[0];
  const uint8_t *s1 = (const uint8_t *)srcs[1];

  register uint32_t zero = 0;
  int n = count >> 2;
  while ( n-- ) {
    asm volatile (
        "mtcr    %[z];"

        "lwz     5,0(%[s0]);"
        "lwz     6,0(%[s1]);"


        "andi.   7,5,0x00ff;"
        "crmove  31,2;"

        "andi.   7,6,0x00ff;"
        "crnor   31,31,2;"


        "andi.   7,5,0xff00;"
        "crmove  23,2;"

        "andi.   7,6,0xff00;"
        "crnor   23,23,2;"


        "andis.  7,5,0x00ff;"
        "crmove  15,2;"

        "andis.  7,6,0x00ff;"
        "crnor   15,15,2;"


        "andis.  7,5,0xff00;"
        "crmove  7,2;"

        "andis.  7,6,0xff00;"
        "crnor   7,7,2;"


        "crclr   0;"
        "crclr   1;"
        "crclr   2;"

        "mfcr    7;"
        "stw     7,0(%[dp]);"

      : // no outputs
      : [s0] "b" (s0),
        [s1] "b" (s1),
        [dp] "b" (dp),
        [z] "r" (zero)
      : "memory", "5", "6", "7"
    );

    s0 += 4;
    s1 += 4;
    dp += 4;
  }

  n = count & 0x03;
  while ( n-- ) {
    *dp = (*s0) && (*s1);
    dp++;
    s0++;
    s1++;
  }

  return;
}

void _pami_core_uint8_lor2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {

  uint8_t *dp = (uint8_t *)dst;
  const uint8_t *s0 = (const uint8_t *)srcs[0];
  const uint8_t *s1 = (const uint8_t *)srcs[1];

  register uint32_t zero = 0;
  int n = count >> 2;
  while ( n-- ) {
    asm volatile (

        "lwz     5,0(%[s0]);"
        "lwz     6,0(%[s1]);"
        "mtcr    %[z];"
        "or      5,5,6;"


        "andi.   7,5,0x00ff;"
        "crnot   31,2;"

        "andi.   7,5,0xff00;"
        "crnot   23,2;"

        "andis.  7,5,0x00ff;"
        "crnot   15,2;"

        "andis.  7,5,0xff00;"
        "crnot   7,2;"


        "crclr   0;"
        "crclr   1;"
        "crclr   2;"

        "mfcr    7;"
        "stw     7,0(%[dp]);"

      : // no outputs
      : [s0] "b" (s0),
        [s1] "b" (s1),
        [dp] "b" (dp),
        [z] "r" (zero)
      : "memory", "5", "6", "7"
    );

    s0 += 4;
    s1 += 4;
    dp += 4;
  }

  n = count & 0x03;
  while ( n-- ) {
    (*dp) = (*s0) || (*s1);

    dp++;
    s0++;
    s1++;
  }

  return;
}

void _pami_core_uint8_lxor2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {

  uint8_t *dp = (uint8_t *)dst;
  const uint8_t *s0 = (const uint8_t *)srcs[0];
  const uint8_t *s1 = (const uint8_t *)srcs[1];

  register uint32_t zero = 0;
  int n = count >> 2;
  while ( n-- ) {
    asm volatile (

        "mtcr    %[z];"

        "lwz     5,0(%[s0]);"
        "lwz     6,0(%[s1]);"


        "andi.   7,5,0x00ff;"
        "crmove  31,2;"

        "andi.   7,6,0x00ff;"
        "crxor   31,31,2;"


        "andi.   7,5,0xff00;"
        "crmove  23,2;"

        "andi.   7,6,0xff00;"
        "crxor   23,23,2;"


        "andis.  7,5,0x00ff;"
        "crmove  15,2;"

        "andis.  7,6,0x00ff;"
        "crxor   15,15,2;"


        "andis.  7,5,0xff00;"
        "crmove  7,2;"

        "andis.  7,6,0xff00;"
        "crxor   7,7,2;"


        "crclr   0;"
        "crclr   1;"
        "crclr   2;"

        "mfcr    7;"
        "stw     7,0(%[dp]);"

      : // no outputs
      : [s0] "b" (s0),
        [s1] "b" (s1),
        [dp] "b" (dp),
        [z] "r" (zero)
      : "memory", "5", "6", "7"
    );

    s0 += 4;
    s1 += 4;
    dp += 4;
  }

  n = count & 0x03;
  register uint8_t s0_r;
  register uint8_t s1_r;
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

void _pami_core_uint8_max2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
  uint8_t *dp = (uint8_t *)dst;
  const uint8_t *s0 = (const uint8_t *)srcs[0];
  const uint8_t *s1 = (const uint8_t *)srcs[1];

  int n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "lbz     5,0(%[s0]);"

      "lbz     9,0(%[s1]);"

      "lbz     6,1(%[s0]);"
      "cmpw    5,9;"

      "lbz     10,1(%[s1]);"
      "bge     0f;"

      "mr      5,9;"

"0:    stb     5,0(%[dp]);"



      "lbz     7,2(%[s0]);"
      "cmpw    6,10;"

      "lbz     11,2(%[s1]);"
      "bge     1f;"

      "mr      6,10;"

"1:    stb     6,1(%[dp]);"



      "lbz     8,3(%[s0]);"
      "cmpw    7,11;"

      "lbz     12,3(%[s1]);"
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

  register uint8_t s0_r;
  register uint8_t s1_r;
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

void _pami_core_uint8_min2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
  uint8_t *dp = (uint8_t *)dst;
  const uint8_t *s0 = (const uint8_t *)srcs[0];
  const uint8_t *s1 = (const uint8_t *)srcs[1];

  int n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "lbz     5,0(%[s0]);"
      "lbz     9,0(%[s1]);"
      "lbz     6,1(%[s0]);"
      "cmpw    9,5;"
      "lbz     10,1(%[s1]);"
      "bge     0f;"
      "mr      5,9;"
"0:    stb     5,0(%[dp]);"

      "lbz     7,2(%[s0]);"
      "cmpw    10,6;"
      "lbz     11,2(%[s1]);"
      "bge     1f;"
      "mr      6,10;"
"1:    stb     6,1(%[dp]);"

      "lbz     8,3(%[s0]);"
      "cmpw    11,7;"
      "lbz     12,3(%[s1]);"
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

  register uint8_t s0_r;
  register uint8_t s1_r;
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

void _pami_core_uint8_prod2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {

  uint8_t *dp = (uint8_t *)dst;
  const uint8_t *s0 = (const uint8_t *)srcs[0];
  const uint8_t *s1 = (const uint8_t *)srcs[1];

  int n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "lbz     5,0(%[s0]);"

      "lbz     6,0(%[s1]);"

      "mullw   5,5,6;"
      "lbz     7,1(%[s0]);"

      "lbz     8,1(%[s1]);"

      "stb     5,0(%[dp]);"
      "mullw   7,7,8;"

      "stb     7,1(%[dp]);"

      "lbz     5,2(%[s0]);"

      "lbz     6,2(%[s1]);"

      "mullw   5,5,6;"
      "lbz     7,3(%[s0]);"

      "lbz     8,3(%[s1]);"

      "stb     5,2(%[dp]);"
      "mullw   7,7,8;"

      "stb     7,3(%[dp]);"


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

  n = count & 0x03;
  while ( n-- ) {
    (*dp) = (*s0) * (*s1);

    dp++;
    s0++;
    s1++;
  }

  return;
}

void _pami_core_uint8_sum2(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {

  const uint8_t *s0 = (const uint8_t *)srcs[0];
  const uint8_t *s1 = (const uint8_t *)srcs[1];
  uint8_t *dp = (uint8_t *)dst;

  int n = count >> 2;
  while ( n-- ) {
    asm volatile (
      "lbz     5,0(%[s0]);"

      "lbz     6,0(%[s1]);"

      "add     5,5,6;"
      "lbz     7,1(%[s0]);"

      "stb     5,0(%[dp]);"

      "lbz     8,1(%[s1]);"

      "add     7,7,8;"
      "lbz     9,2(%[s0]);"

      "stb     7,1(%[dp]);"

      "lbz     10,2(%[s1]);"

      "add     9,9,10;"
      "lbz     11,3(%[s0]);"

      "lbz     12,3(%[s1]);"

      "stb     9,2(%[dp]);"
      "add     11,11,12;"

      "stb     11,3(%[dp]);"

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

  n = count & 0x07;
  while ( n-- ) {
    (*dp) = (*s0) + (*s1);

    s0++;
    s1++;
    dp++;
  }

  return;
}
