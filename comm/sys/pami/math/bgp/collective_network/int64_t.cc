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
 * \file math/bgp/collective_network/int64_t.cc
 * \brief Default C math routines for 32 bit signed integer operations.
 */

#include "pami_bg_math.h"
#include "util/common.h"
//#include "internal.h"

static void _pami_core_int64_conv(uint64_t *dst, const int64_t *src, int count) {
#define OP(a) ((a)+(0x8000000000000000ULL))

#define TYPE uint64_t
#include "math/_single_src.x.h"
#undef TYPE
#undef OP
}

static void _pami_core_int64_conv_not(uint64_t *dst, const int64_t *src, int count) {
#define OP(a) (~((a)+(0x8000000000000000ULL)))

#define TYPE uint64_t
#include "math/_single_src.x.h"
#undef TYPE
#undef OP
}

static void _pami_core_int64_unconv(int64_t *dst, const uint64_t *src, int count) {
#define OP(a) ((a)-(0x8000000000000000ULL))

#define TYPE int64_t
#include "math/_single_src.x.h"
#undef TYPE
#undef OP
}

static void _pami_core_int64_unconv_not(int64_t *dst, const uint64_t *src, int count) {
#define OP(a) (~((a)-(0x8000000000000000ULL)))

#define TYPE int64_t
#include "math/_single_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int64_pre_all(uint64_t *dst, const int64_t *src, int count) {
  _pami_core_int64_conv(dst, src, count);
}

void _pami_core_int64_post_all(int64_t *dst, const uint64_t *src, int count) {
  _pami_core_int64_unconv(dst, src, count);
}

void _pami_core_int64_pre_min(uint64_t *dst, const int64_t *src, int count) {
  _pami_core_int64_conv_not(dst, src, count);
}

void _pami_core_int64_post_min(int64_t *dst, const uint64_t *src, int count) {
  _pami_core_int64_unconv_not(dst, src, count);
}
