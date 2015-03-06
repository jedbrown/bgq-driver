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
 * \file math/uint8.cc
 * \brief Default C math routines for 8 bit unsigned integer operations.
 */

#include "math_coremath.h"
#include "internal.h"

void _pami_core_uint8_band(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)&(b))

#define TYPE uint8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint8_bor(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)|(b))

#define TYPE uint8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint8_bxor(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)^(b))

#define TYPE uint8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint8_land(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)&&(b))

#define TYPE uint8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint8_lor(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)||(b))

#define TYPE uint8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint8_lxor(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)&&(!b))||((!a)&&(b)))

#define TYPE uint8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint8_max(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)>(b))?(a):(b))

#define TYPE uint8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint8_min(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)>(b))?(b):(a))

#define TYPE uint8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint8_prod(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)*(b))

#define TYPE uint8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint8_sum(uint8_t *dst, const uint8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)+(b))

#define TYPE uint8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}
