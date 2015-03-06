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
 * \file math/bgp/collective_network/uint16_t.cc
 * \brief Default C math routines for 16 bit unsigned integer operations.
 */

#include "pami_bg_math.h"
#include "util/common.h"
//#include "internal.h"

static void _pami_core_uint16_not(uint16_t *dst, const uint16_t *src, int count) {
#define OP(a) (~(a))

#define TYPE uint16_t
#include "math/_single_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint16_pre_all(uint16_t *dst, const uint16_t *src, int count) {
  Core_memcpy((void *)dst, (const void *)src, (size_t)count * sizeof(uint16_t));
}

void _pami_core_uint16_post_all(uint16_t *dst, const uint16_t *src, int count) {
  Core_memcpy((void *)dst, (const void *)src, (size_t)count * sizeof(uint16_t));
}

void _pami_core_uint16_pre_min(uint16_t *dst, const uint16_t *src, int count) {
  _pami_core_uint16_not(dst, src, count);
}

void _pami_core_uint16_post_min(uint16_t *dst, const uint16_t *src, int count) {
  _pami_core_uint16_not(dst, src, count);
}
