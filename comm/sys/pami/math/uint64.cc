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
 * \file math/uint64.cc
 * \brief Default C math routines for 64 bit unsigned integer operations.
 */

#include "math_coremath.h"
#include "internal.h"

void _pami_core_uint64_band(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)&(b))

#define TYPE uint64_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint64_bor(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)|(b))

#define TYPE uint64_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint64_bxor(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)^(b))

#define TYPE uint64_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint64_land(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)&&(b))

#define TYPE uint64_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint64_lor(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)||(b))

#define TYPE uint64_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint64_lxor(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)&&(!b))||((!a)&&(b)))

#define TYPE uint64_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint64_max(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)>(b))?(a):(b))

#define TYPE uint64_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint64_min(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)>(b))?(b):(a))

#define TYPE uint64_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint64_prod(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)*(b))

#define TYPE uint64_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint64_sum(uint64_t *dst, const uint64_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)+(b))

#define TYPE uint64_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}
