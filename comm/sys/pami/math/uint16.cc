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
 * \file math/uint16.cc
 * \brief Default C math routines for 16 bit unsigned integer operations.
 */

#include "math_coremath.h"
#include "internal.h"


void _pami_core_uint16_band(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)&(b))

#define TYPE uint16_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint16_bor(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)|(b))

#define TYPE uint16_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint16_bxor(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)^(b))

#define TYPE uint16_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint16_land(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)&&(b))

#define TYPE uint16_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint16_lor(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)||(b))

#define TYPE uint16_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint16_lxor(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)&&(!b))||((!a)&&(b)))

#define TYPE uint16_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint16_max(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)>(b))?(a):(b))

#define TYPE uint16_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint16_min(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)>(b))?(b):(a))

#define TYPE uint16_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint16_prod(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)*(b))

#define TYPE uint16_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_uint16_sum(uint16_t *dst, const uint16_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)+(b))

#define TYPE uint16_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}
