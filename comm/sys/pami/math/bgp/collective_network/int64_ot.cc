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
 * \file math/bgp/collective_network/int64_ot.cc
 * \brief Optimized math routines for signed 64 bit integer operations on
 *        the ppc 450 dual fpu architecture.
 */
#include "pami_bg_math.h"
#include "util/common.h"
//#include "ppc450d/internal_o.h"

static void _pami_core_int64_conv_o(uint64_t *dst, const int64_t *src, int count) {
#define OP(a) ((a)+(0x8000000000000000ULL))

#define TYPE uint64_t
#include "math/_single_src.x.h"
#undef TYPE
#undef OP
}

static void _pami_core_int64_conv_not_o(uint64_t *dst, const int64_t *src, int count) {
#define OP(a) (~((a)+(0x8000000000000000ULL)))

#define TYPE uint64_t
#include "math/_single_src.x.h"
#undef TYPE
#undef OP
}

static void _pami_core_int64_unconv_o(int64_t *dst, const uint64_t *src, int count) {
#define OP(a) ((a)-(0x8000000000000000ULL))

#define TYPE uint64_t
#include "math/_single_src.x.h"
#undef TYPE
#undef OP
}

static void _pami_core_int64_unconv_not_o(int64_t *dst, const uint64_t *src, int count) {
#define OP(a) (~((a)-(0x8000000000000000ULL)))

#define TYPE uint64_t
#include "math/_single_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int64_pre_all_o(uint64_t *dst, const int64_t *src, int count) {
  _pami_core_int64_conv_o(dst, src, count);
}

void _pami_core_int64_post_all_o(int64_t *dst, const uint64_t *src, int count) {
  _pami_core_int64_unconv_o(dst, src, count);
}

void _pami_core_int64_pre_min_o(uint64_t *dst, const int64_t *src, int count) {
  _pami_core_int64_conv_not_o(dst, src, count);
}

void _pami_core_int64_post_min_o(int64_t *dst, const uint64_t *src, int count) {
  _pami_core_int64_unconv_not_o(dst, src, count);
}
