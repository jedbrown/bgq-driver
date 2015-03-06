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
 * \file math/int.cc
 * \brief Default C math routines for 64 bit floating point operations.
 */
#include "stdio.h"
void _pami_core_int_sum_16way(int* dst, int* src0, int *src1, int* src2, int* src3,
    int* src4, int* src5, int* src6, int* src7, int* src8, int *src9, int* src10, int* src11,
    int* src12, int* src13, int* src14, int* src15, unsigned num_ints){

#define OP(a,b) (a+b)
#define DTYPE int

#include "_int_16way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_int_sum_8way(int* dst, int* src0, int *src1, int* src2, int* src3,
    int* src4, int* src5, int* src6, int* src7, unsigned num_ints){

#define OP(a,b) (a+b)
#define DTYPE int

#include "_int_8way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_int_sum_4way(int* dst, int* src0, int *src1, int* src2, int* src3, unsigned num_ints){

#define OP(a,b) (a+b)
#define DTYPE int

#include "_int_4way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_int_sum_2way(int* dst, int* src0, int *src1, unsigned num_ints){

#define OP(a,b) (a+b)
#define DTYPE int

#include "_int_2way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_int_max_16way(int* dst, int* src0, int *src1, int* src2, int* src3,
    int* src4, int* src5, int* src6, int* src7, int* src8, int *src9, int* src10, int* src11,
    int* src12, int* src13, int* src14, int* src15, unsigned num_ints){

#define OP(a,b) (((a)>(b))?(a):(b))
#define DTYPE int

#include "_int_16way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_int_max_8way(int* dst, int* src0, int *src1, int* src2, int* src3,
    int* src4, int* src5, int* src6, int* src7, unsigned num_ints){

#define OP(a,b) (((a)>(b))?(a):(b))
#define DTYPE int

#include "_int_8way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_int_max_4way(int* dst, int* src0, int *src1, int* src2, int* src3, unsigned num_ints){

#define OP(a,b) (((a)>(b))?(a):(b))
#define DTYPE int

#include "_int_4way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_int_max_2way(int* dst, int* src0, int *src1, unsigned num_ints){

#define OP(a,b) (((a)>(b))?(a):(b))
#define DTYPE int

#include "_int_2way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_int_min_16way(int* dst, int* src0, int *src1, int* src2, int* src3,
    int* src4, int* src5, int* src6, int* src7, int* src8, int *src9, int* src10, int* src11,
    int* src12, int* src13, int* src14, int* src15, unsigned num_ints){
#define OP(a,b) (((a)<(b))?(a):(b))
#define DTYPE int

#include "_int_16way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_int_min_8way(int* dst, int* src0, int *src1, int* src2, int* src3,
    int* src4, int* src5, int* src6, int* src7, unsigned num_ints){

#define OP(a,b) (((a)<(b))?(a):(b))
#define DTYPE int

#include "_int_8way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_int_min_4way(int* dst, int* src0, int *src1, int* src2, int* src3, unsigned num_ints){

#define OP(a,b) (((a)<(b))?(a):(b))
#define DTYPE int

#include "_int_4way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_int_min_2way(int* dst, int* src0, int *src1, unsigned num_ints){

#define OP(a,b) (((a)<(b))?(a):(b))
#define DTYPE int

#include "_int_2way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_uint_sum_16way(unsigned* dst, unsigned* src0, unsigned *src1, unsigned* src2, unsigned* src3,
    unsigned* src4, unsigned* src5, unsigned* src6, unsigned* src7, unsigned* src8, unsigned *src9, unsigned* src10, unsigned* src11, unsigned* src12, unsigned* src13, unsigned* src14, unsigned* src15, unsigned num_ints){

#define OP(a,b) (a+b)
#define DTYPE unsigned

#include "_int_16way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_uint_sum_8way(unsigned* dst, unsigned* src0, unsigned *src1, unsigned* src2, unsigned* src3,
    unsigned* src4, unsigned* src5, unsigned* src6, unsigned* src7, unsigned num_ints){

#define OP(a,b) (a+b)
#define DTYPE unsigned

#include "_int_8way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_uint_sum_4way(unsigned* dst, unsigned* src0, unsigned *src1, unsigned* src2, unsigned* src3, unsigned num_ints){

#define OP(a,b) (a+b)
#define DTYPE unsigned

#include "_int_4way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_uint_sum_2way(unsigned* dst, unsigned* src0, unsigned *src1, unsigned num_ints){

#define OP(a,b) (a+b)
#define DTYPE unsigned

#include "_int_2way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_uint_max_16way(unsigned* dst, unsigned* src0, unsigned *src1, unsigned* src2, unsigned* src3,
    unsigned* src4, unsigned* src5, unsigned* src6, unsigned* src7, unsigned* src8, unsigned *src9, unsigned* src10, unsigned* src11,
    unsigned* src12, unsigned* src13, unsigned* src14, unsigned* src15, unsigned num_ints){

#define OP(a,b) (((a)>(b))?(a):(b))
#define DTYPE unsigned

#include "_int_16way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_uint_max_8way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned* src2, unsigned* src3,
    unsigned* src4, unsigned* src5, unsigned* src6, unsigned* src7, unsigned num_ints){

#define OP(a,b) (((a)>(b))?(a):(b))
#define DTYPE unsigned

#include "_int_8way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_uint_max_4way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned* src2, unsigned* src3, unsigned num_ints){

#define OP(a,b) (((a)>(b))?(a):(b))
#define DTYPE unsigned

#include "_int_4way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_uint_max_2way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned num_ints){

#define OP(a,b) (((a)>(b))?(a):(b))
#define DTYPE unsigned

#include "_int_2way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_uint_min_16way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned* src2, unsigned* src3,
    unsigned* src4, unsigned* src5, unsigned* src6, unsigned* src7, unsigned* src8, unsigned* src9, unsigned* src10, unsigned* src11,
    unsigned* src12, unsigned* src13, unsigned* src14, unsigned* src15, unsigned num_ints){
#define OP(a,b) (((a)<(b))?(a):(b))
#define DTYPE unsigned

#include "_int_16way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_uint_min_8way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned* src2, unsigned* src3,
    unsigned* src4, unsigned* src5, unsigned* src6, unsigned* src7, unsigned num_ints){

#define OP(a,b) (((a)<(b))?(a):(b))
#define DTYPE unsigned

#include "_int_8way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_uint_min_4way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned* src2, unsigned* src3, unsigned num_ints){

#define OP(a,b) (((a)<(b))?(a):(b))
#define DTYPE unsigned

#include "_int_4way.x.h"
#undef  DTYPE 
#undef OP
}

void _pami_core_uint_min_2way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned num_ints){

#define OP(a,b) (((a)<(b))?(a):(b))
#define DTYPE unsigned

#include "_int_2way.x.h"
#undef  DTYPE 
#undef OP
}
