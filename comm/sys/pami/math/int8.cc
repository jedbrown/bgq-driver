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
 * \file math/int8.cc
 * \brief Default C math routines for 8 bit signed integer operations.
 */

#include "math_coremath.h"
#include "internal.h"

void _pami_core_int8_band(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)&(b))

#define TYPE int8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int8_bor(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)|(b))

#define TYPE int8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int8_bxor(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)^(b))

#define TYPE int8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int8_land(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)&&(b))

#define TYPE int8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int8_lor(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)||(b))

#define TYPE int8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int8_lxor(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)&&(!b))||((!a)&&(b)))

#define TYPE int8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int8_max(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)>(b))?(a):(b))

#define TYPE int8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int8_min(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
#define OP(a,b) (((a)>(b))?(b):(a))

#define TYPE int8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int8_prod(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)*(b))

#define TYPE int8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}

void _pami_core_int8_sum(int8_t *dst, const int8_t **srcs, int nsrc, int count) {
#define OP(a,b) ((a)+(b))

#define TYPE int8_t
#include "_N_src.x.h"
#undef TYPE
#undef OP
}
