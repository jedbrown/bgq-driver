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
 * \file math/bgp/collective_network/uint8_t.cc
 * \brief Default C math routines for 8 bit unsigned integer operations.
 */

#include "pami_bg_math.h"
#include "util/common.h"
//#include "internal.h"

static void _pami_core_uint8_not(uint8_t *dst, const uint8_t *src, int count) {
#define OP(a) (~(a))

#define TYPE uint8_t
#include "math/_single_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint8_pre_all(uint8_t *dst, const uint8_t *src, int count) {
  Core_memcpy((void *)dst, (const void *)src, (size_t)count);
}

void _pami_core_uint8_post_all(uint8_t *dst, const uint8_t *src, int count) {
  Core_memcpy((void *)dst, (const void *)src, (size_t)count);
}

void _pami_core_uint8_pre_min(uint8_t *dst, const uint8_t *src, int count) {
  _pami_core_uint8_not(dst, src, count);
}

void _pami_core_uint8_post_min(uint8_t *dst, const uint8_t *src, int count) {
  _pami_core_uint8_not(dst, src, count);
}
