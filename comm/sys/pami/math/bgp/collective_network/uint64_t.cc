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
 * \file math/bgp/collective_network/uint64_t.cc
 * \brief Default C math routines for 64 bit unsigned integer operations.
 */

#include "pami_bg_math.h"
#include "util/common.h"
//#include "internal.h"

static void _pami_core_uint64_not(uint64_t *dst, const uint64_t *src, int count) {
#define OP(a) (~(a))

#define TYPE uint64_t
#include "math/_single_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint64_pre_all(uint64_t *dst, const uint64_t *src, int count) {
  Core_memcpy((void *)dst, (const void *)src, (size_t)count * sizeof(uint64_t));
}

void _pami_core_uint64_post_all(uint64_t *dst, const uint64_t *src, int count) {
  Core_memcpy((void *)dst, (const void *)src, (size_t)count * sizeof(uint64_t));
}

void _pami_core_uint64_pre_min(uint64_t *dst, const uint64_t *src, int count) {
  _pami_core_uint64_not(dst, src, count);
}

void _pami_core_uint64_post_min(uint64_t *dst, const uint64_t *src, int count) {
  _pami_core_uint64_not(dst, src, count);
}
