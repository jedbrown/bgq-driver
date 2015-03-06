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
 * \file math/bgp/collective_network/int32_t.cc
 * \brief Default C math routines for 32 bit signed integer operations.
 */

#include "pami_bg_math.h"
#include "util/common.h"
//#include "internal.h"

static void _pami_core_int32_conv(uint32_t *dst, const int32_t *src, int count) {
#define OP(a) ((a)+(0x80000000UL))

#define TYPE uint32_t
#include "math/_single_src.x.h"
#undef TYPE
#undef OP
}

static void _pami_core_int32_conv_not(uint32_t *dst, const int32_t *src, int count) {
#define OP(a) (~((a)+(0x80000000UL)))

#define TYPE uint32_t
#include "math/_single_src.x.h"
#undef TYPE
#undef OP
}

static void _pami_core_int32_unconv(int32_t *dst, const uint32_t *src, int count) {
#define OP(a) ((a)-(0x80000000UL))

#define TYPE int32_t
#include "math/_single_src.x.h"
#undef TYPE
#undef OP
}

static void _pami_core_int32_unconv_not(int32_t *dst, const uint32_t *src, int count) {
#define OP(a) (~((a)-(0x80000000UL)))

#define TYPE int32_t
#include "math/_single_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int32_pre_all(uint32_t *dst, const int32_t *src, int count) {
  _pami_core_int32_conv(dst, src, count);
}

void _pami_core_int32_post_all(int32_t *dst, const uint32_t *src, int count) {
  _pami_core_int32_unconv(dst, src, count);
}

void _pami_core_int32_pre_min(uint32_t *dst, const int32_t *src, int count) {
  _pami_core_int32_conv_not(dst, src, count);
}

void _pami_core_int32_post_min(int32_t *dst, const uint32_t *src, int count) {
  _pami_core_int32_unconv_not(dst, src, count);
}

void _pami_core_int32_int32_pre_maxloc(uint32_int32_t *dst, const int32_int32_t *src, int count) {
  register int n = 0;
  register unsigned shift = 0x80000000UL;
  for (n = 0; n < count; n++)
    {
      dst[n].a = (uint32_t)src[n].a + shift;
      dst[n].b = LOC_INT_TOTREE(src[n].b);
    }
}

void _pami_core_int32_int32_post_maxloc(int32_int32_t *dst, const uint32_int32_t *src, int count) {
  register int n = 0;
  register unsigned shift = 0x80000000UL;
  for (n = 0; n < count; n++)
    {
      dst[n].a = src[n].a - shift;
      dst[n].b = LOC_INT_FRTREE(src[n].b);
    }
}

void _pami_core_int32_int32_pre_minloc(uint32_int32_t *dst, const int32_int32_t *src, int count) {
  register int n = 0;
  register unsigned shift = 0x80000000UL;
  for (n = 0; n < count; n++)
    {
      dst[n].a = ~((uint32_t)src[n].a + shift);
      dst[n].b = LOC_INT_TOTREE(src[n].b);
    }
}

void _pami_core_int32_int32_post_minloc(int32_int32_t *dst, const uint32_int32_t *src, int count) {
  register int n = 0;
  register unsigned shift = 0x80000000UL;
  for (n = 0; n < count; n++)
    {
      dst[n].a = ~(src[n].a - shift);
      dst[n].b = LOC_INT_FRTREE(src[n].b);
    }
}
