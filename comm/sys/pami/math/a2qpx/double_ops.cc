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
 * \file math/double.cc
 * \brief Default C math routines for 64 bit floating point operations.
 */
#include "stdio.h"
void _pami_core_double_sum_16way(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, double* src8, double *src9, double* src10, double* src11,
    double* src12, double* src13, double* src14, double* src15, unsigned num_dbls){

#define OP(a,b) (a+b)

#include "_double_16way.x.h"
#undef OP
}

void _pami_core_double_sum_8way(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, unsigned num_dbls){

#define OP(a,b) (a+b)

#include "_double_8way.x.h"
#undef OP
}

void _pami_core_double_sum_4way(double* dst, double* src0, double *src1, double* src2, double* src3, unsigned num_dbls){

#define OP(a,b) (a+b)

#include "_double_4way.x.h"
#undef OP
}

void _pami_core_double_sum_3way(double* dst, double* src0, double *src1, double* src2, unsigned num_dbls){

#define OP(a,b) (a+b)

#include "_double_3way.x.h"
#undef OP
}

void _pami_core_double_sum_2way(double* dst, double* src0, double *src1, unsigned num_dbls){

#define OP(a,b) (a+b)

#include "_double_2way.x.h"
#undef OP
}

void _pami_core_double_max_16way(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, double* src8, double *src9, double* src10, double* src11,
    double* src12, double* src13, double* src14, double* src15, unsigned num_dbls){

#define OP(a,b) (((a)>(b))?(a):(b))

#include "_double_16way.x.h"
#undef OP
}

void _pami_core_double_max_8way(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, unsigned num_dbls){

#define OP(a,b) (((a)>(b))?(a):(b))

#include "_double_8way.x.h"
#undef OP
}

void _pami_core_double_max_4way(double* dst, double* src0, double *src1, double* src2, double* src3, unsigned num_dbls){

#define OP(a,b) (((a)>(b))?(a):(b))

#include "_double_4way.x.h"
#undef OP
}

void _pami_core_double_max_3way(double* dst, double* src0, double *src1, double* src2, unsigned num_dbls){

#define OP(a,b) (((a)>(b))?(a):(b))

#include "_double_3way.x.h"
#undef OP
}

void _pami_core_double_max_2way(double* dst, double* src0, double *src1, unsigned num_dbls){

#define OP(a,b) (((a)>(b))?(a):(b))

#include "_double_2way.x.h"
#undef OP
}

void _pami_core_double_min_16way(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, double* src8, double *src9, double* src10, double* src11,
    double* src12, double* src13, double* src14, double* src15, unsigned num_dbls){
#define OP(a,b) (((a)<(b))?(a):(b))

#include "_double_16way.x.h"
#undef OP
}

void _pami_core_double_min_8way(double* dst, double* src0, double *src1, double* src2, double* src3,
    double* src4, double* src5, double* src6, double* src7, unsigned num_dbls){

#define OP(a,b) (((a)<(b))?(a):(b))

#include "_double_8way.x.h"
#undef OP
}

void _pami_core_double_min_4way(double* dst, double* src0, double *src1, double* src2, double* src3, unsigned num_dbls){

#define OP(a,b) (((a)<(b))?(a):(b))

#include "_double_4way.x.h"
#undef OP
}

void _pami_core_double_min_3way(double* dst, double* src0, double *src1, double* src2, unsigned num_dbls){

#define OP(a,b) (((a)<(b))?(a):(b))

#include "_double_3way.x.h"
#undef OP
}

void _pami_core_double_min_2way(double* dst, double* src0, double *src1, unsigned num_dbls){

#define OP(a,b) (((a)<(b))?(a):(b))

#include "_double_2way.x.h"
#undef OP
}
