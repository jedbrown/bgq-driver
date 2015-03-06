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
 * \file algorithms/protocols/allreduce/ReduceFunctions.h
 * \brief Support routines for allreduce operations
 */
#ifndef __algorithms_protocols_allreduce_ReduceFunctions_h__
#define __algorithms_protocols_allreduce_ReduceFunctions_h__



#include "util/common.h"  // need this first to define throw() before stdlib.h is included (for xlC -noeh)
#include "algorithms/ccmi.h"
#include "util/ccmi_debug.h"
#include "math/math_coremath.h"

namespace CCMI
{
  namespace Adaptor
  {
    namespace Allreduce
    {
      ///
      ///  \brief Stub in a function for double complex that
      /// will just call double float sum with count*2.
      ///
      inline void double_complex_sum(double *dst, const double **srcs,
                                     int nsrc, int count)
      {
        Core_fp64_sum(dst, srcs, nsrc, count*2);
      }

      ///
      ///  \brief Stub in a function for single complex that
      /// will just call float sum with count*2.
      ///
      inline void single_complex_sum (float *dst, const float **srcs,
                                      int nsrc, int count)
      {
        Core_fp32_sum(dst, srcs, nsrc, count*2);
      }
///
/// \brief Get the reduce function and datatype size
///
      inline void getReduceFunction(pami_dt           dtype,
                                    pami_op           op,
                                    unsigned        & sizeOfType,
                                    coremath        & func )
      {
        TRACE_ADAPTOR ((stderr, "<          >CCMI::Adaptor::getReduceFunction(dtype %#X,op %#X)\n",
                        dtype, op));

	//Read select math functions from table. Currently we get
	//operations sum, min, max 
	//types signed/unsigned char,short,int 
	//and floating point 32,64 and 128 bits from the table. 
	//The rest are set in the switch
	func = MATH_OP_FUNCS(dtype, op, 2);
        switch (op)
          {
            case PAMI_SUM:

              switch (dtype)
                {
                  case PAMI_SIGNED_INT:
                    sizeOfType = sizeof(int);
                    break;
                  case PAMI_UNSIGNED_INT:
                    sizeOfType = sizeof(unsigned int);
                    break;
                  case PAMI_SIGNED_LONG:
                    sizeOfType = sizeof(signed long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_int32_sum;
                    else
                      func = (coremath) Core_int64_sum;
                    break;
                  case PAMI_UNSIGNED_LONG:
                    sizeOfType = sizeof(unsigned long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_uint32_sum;
                    else
                      func = (coremath) Core_uint64_sum;
                    break;
                  case PAMI_SIGNED_LONG_LONG:
                    sizeOfType = sizeof(long long);
                    func = (coremath) Core_int64_sum;
                    break;
                  case PAMI_UNSIGNED_LONG_LONG:
                    sizeOfType = sizeof(long long);
                    func = (coremath) Core_uint64_sum;
                    break;
                  case PAMI_SIGNED_SHORT:
                    sizeOfType = sizeof(short);
                    break;
                  case PAMI_UNSIGNED_SHORT:
                    sizeOfType = sizeof(unsigned short);
                    break;
                  case PAMI_DOUBLE:
                    sizeOfType = sizeof(double);
                    break;
                  case PAMI_FLOAT:
                    sizeOfType = sizeof(float);
                    break;
                  case PAMI_LONG_DOUBLE:
                    sizeOfType = sizeof(long double);
                    break;
                  case PAMI_DOUBLE_COMPLEX:
                    sizeOfType = sizeof(fp64_fp64_t);
                    func = (coremath) double_complex_sum;
                    break;
                  case PAMI_SINGLE_COMPLEX:
                    sizeOfType = sizeof(fp32_fp32_t);
                    func = (coremath) single_complex_sum;
                    break;
                  case PAMI_SIGNED_CHAR:
                    sizeOfType = sizeof(char);
                    break;
                  case PAMI_UNSIGNED_CHAR:
                    sizeOfType = sizeof(unsigned char);
                    break;
                  default:
                    fprintf(stderr, "<          >CCMI::Adaptor::getReduceFunction(dtype %#X,op %#X)\n",
                            dtype, op);
                    PAMI_abort();
                }

              break;
            case PAMI_MAX:

              switch (dtype)
                {
                  case PAMI_SIGNED_INT:
                    sizeOfType = sizeof(int);
                    break;
                  case PAMI_SIGNED_LONG_LONG:
                    sizeOfType = sizeof(long long);
                    func = (coremath) Core_int64_max;
                    break;
                  case PAMI_SIGNED_LONG:
                    sizeOfType = sizeof(signed long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_int32_max;
                    else
                      func = (coremath) Core_int64_max;
                    break;
                  case PAMI_UNSIGNED_LONG:
                    sizeOfType = sizeof(unsigned long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_uint32_max;
                    else
                      func = (coremath) Core_uint64_max;
                    break;
                  case PAMI_UNSIGNED_LONG_LONG:
                    sizeOfType = sizeof(long long);
                    func = (coremath) Core_uint64_max;
                    break;
                  case PAMI_SIGNED_SHORT:
                    sizeOfType = sizeof(short);
                    break;
                  case PAMI_UNSIGNED_INT:
                    sizeOfType = sizeof(unsigned int);
                    break;
                  case PAMI_UNSIGNED_SHORT:
                    sizeOfType = sizeof(unsigned short);
                    break;
                  case PAMI_FLOAT:
                    sizeOfType = sizeof(float);
                    break;
                  case PAMI_DOUBLE:
                    sizeOfType = sizeof(double);
                    break;
                  case PAMI_LONG_DOUBLE:
                    sizeOfType = sizeof(long double);
                    break;
                  case PAMI_SIGNED_CHAR:
                    sizeOfType = sizeof(char);
                    break;
                  case PAMI_UNSIGNED_CHAR:
                    sizeOfType = sizeof(unsigned char);
                    break;
                  default:
                    fprintf(stderr, "<          >CCMI::Adaptor::getReduceFunction(dtype %#X,op %#X)\n",
                            dtype, op);
                    PAMI_abort();
                }

              break;
            case PAMI_MIN:

              switch (dtype)
                {
                  case PAMI_SIGNED_INT:
                    sizeOfType = sizeof(int);
                    break;
                  case PAMI_SIGNED_LONG:
                    sizeOfType = sizeof(signed long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_int32_min;
                    else
                      func = (coremath) Core_int64_min;
                    break;
                  case PAMI_UNSIGNED_LONG:
                    sizeOfType = sizeof(unsigned long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_uint32_min;
                    else
                      func = (coremath) Core_uint64_min;
                    break;
                  case PAMI_SIGNED_LONG_LONG:
                    sizeOfType = sizeof(long long);
                    func = (coremath) Core_int64_min;
                    break;
                  case PAMI_UNSIGNED_LONG_LONG:
                    sizeOfType = sizeof(unsigned long long);
                    func = (coremath) Core_uint64_min;
                    break;
                  case PAMI_SIGNED_SHORT:
                    sizeOfType = sizeof(short);
                    break;
                  case PAMI_UNSIGNED_INT:
                    sizeOfType = sizeof(unsigned int);
                    break;
                  case PAMI_UNSIGNED_SHORT:
                    sizeOfType = sizeof(unsigned short);
                    break;
                  case PAMI_FLOAT:
                    sizeOfType = sizeof(float);
                    break;
                  case PAMI_DOUBLE:
                    sizeOfType = sizeof(double);
                    break;
                  case PAMI_LONG_DOUBLE:
                    sizeOfType = sizeof(long double);
                    break;
                  case PAMI_UNSIGNED_CHAR:
                    sizeOfType = sizeof(unsigned char);
                    break;
                  case PAMI_SIGNED_CHAR:
                    sizeOfType = sizeof(char);
                    break;
                  default:
                    fprintf(stderr, "<          >CCMI::Adaptor::getReduceFunction(dtype %#X,op %#X)\n",
                            dtype, op);
                    PAMI_abort();
                }

              break;
            case PAMI_BAND:

              switch (dtype)
                {
                  case PAMI_FLOAT:
                    sizeOfType = sizeof(float);
                    func = (coremath) Core_uint32_band;
                    break;
                  case PAMI_UNSIGNED_INT:
                    sizeOfType = sizeof(unsigned int);
                    func = (coremath) Core_uint32_band;
                    break;
                  case PAMI_SIGNED_LONG:
                    sizeOfType = sizeof(signed long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_int32_band;
                    else
                      func = (coremath) Core_int64_band;
                    break;
                  case PAMI_UNSIGNED_LONG:
                    sizeOfType = sizeof(unsigned long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_uint32_band;
                    else
                      func = (coremath) Core_uint64_band;
                    break;
                  case PAMI_SIGNED_INT:
                    sizeOfType = sizeof(int);
                    func = (coremath) Core_int32_band;
                    break;
                  case PAMI_DOUBLE:
                    sizeOfType = sizeof(double);
                    func = (coremath) Core_uint64_band;
                    break;
                  case PAMI_SIGNED_LONG_LONG:
                    sizeOfType = sizeof(long long);
                    func = (coremath) Core_int64_band;
                    break;
                  case PAMI_UNSIGNED_LONG_LONG:
                    sizeOfType = sizeof(unsigned long long);
                    func = (coremath) Core_uint64_band;
                    break;
                  case PAMI_UNSIGNED_SHORT:
                    sizeOfType = sizeof(unsigned short);
                    func = (coremath) Core_uint16_band;
                    break;
                  case PAMI_SIGNED_SHORT:
                    sizeOfType = sizeof(short);
                    func = (coremath) Core_int16_band;
                    break;
                  case PAMI_BYTE:
                  case PAMI_UNSIGNED_CHAR:
                    sizeOfType = sizeof(unsigned char);
                    func = (coremath) Core_uint8_band;
                    break;
                  case PAMI_SIGNED_CHAR:
                    sizeOfType = sizeof(char);
                    func = (coremath) Core_int8_band;
                    break;
                  case PAMI_LOGICAL1:
                    sizeOfType = 1;
                    func = (coremath) Core_uint8_band;
                    break;
                  case PAMI_LOGICAL2:
                    sizeOfType = 2;
                    func = (coremath) Core_uint16_band;
                    break;
                  case PAMI_LOGICAL4:
                    sizeOfType = 4;
                    func = (coremath) Core_uint32_band;
                    break;
                  case PAMI_LOGICAL8:
                    sizeOfType = 8;
                    func = (coremath) Core_uint64_band;
                    break;
                  default:
                    fprintf(stderr, "<          >CCMI::Adaptor::getReduceFunction(dtype %#X,op %#X)\n",
                            dtype, op);
                    PAMI_abort();
                }

              break;
            case PAMI_BOR:

              switch (dtype)
                {
                  case PAMI_FLOAT:
                    sizeOfType = sizeof(float);
                    func = (coremath) Core_uint32_bor;
                    break;
                  case PAMI_UNSIGNED_INT:
                    sizeOfType = sizeof(unsigned int);
                    func = (coremath) Core_uint32_bor;
                    break;
                  case PAMI_SIGNED_INT:
                    sizeOfType = sizeof(int);
                    func = (coremath) Core_int32_bor;
                    break;
                  case PAMI_SIGNED_LONG:
                    sizeOfType = sizeof(signed long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_int32_bor;
                    else
                      func = (coremath) Core_int64_bor;
                    break;
                  case PAMI_UNSIGNED_LONG:
                    sizeOfType = sizeof(unsigned long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_uint32_bor;
                    else
                      func = (coremath) Core_uint64_bor;
                    break;
                  case PAMI_DOUBLE:
                    sizeOfType = sizeof(double);
                    func = (coremath) Core_uint64_bor;
                    break;
                  case PAMI_SIGNED_LONG_LONG:
                    sizeOfType = sizeof(long long);
                    func = (coremath) Core_int64_bor;
                    break;
                  case PAMI_UNSIGNED_LONG_LONG:
                    sizeOfType = sizeof(unsigned long long);
                    func = (coremath) Core_uint64_bor;
                    break;
                  case PAMI_UNSIGNED_SHORT:
                    sizeOfType = sizeof(unsigned short);
                    func = (coremath) Core_uint16_bor;
                    break;
                  case PAMI_SIGNED_SHORT:
                    sizeOfType = sizeof(short);
                    func = (coremath) Core_int16_bor;
                    break;
                  case PAMI_BYTE:
                  case PAMI_UNSIGNED_CHAR:
                    sizeOfType = sizeof(unsigned char);
                    func = (coremath) Core_uint8_bor;
                    break;
                  case PAMI_SIGNED_CHAR:
                    sizeOfType = sizeof(char);
                    func = (coremath) Core_int8_bor;
                    break;
                  case PAMI_LOGICAL1:
                    sizeOfType = 1;
                    func = (coremath) Core_uint8_bor;
                    break;
                  case PAMI_LOGICAL2:
                    sizeOfType = 2;
                    func = (coremath) Core_uint16_bor;
                    break;
                  case PAMI_LOGICAL4:
                    sizeOfType = 4;
                    func = (coremath) Core_uint32_bor;
                    break;
                  case PAMI_LOGICAL8:
                    sizeOfType = 8;
                    func = (coremath) Core_uint64_bor;
                    break;
                  default:
                    fprintf(stderr, "<          >CCMI::Adaptor::getReduceFunction(dtype %#X,op %#X)\n",
                            dtype, op);
                    PAMI_abort();
                }

              break;
            case PAMI_BXOR:

              switch (dtype)
                {
                  case PAMI_FLOAT:
                    sizeOfType = sizeof(float);
                    func = (coremath) Core_uint32_bxor;
                    break;
                  case PAMI_UNSIGNED_INT:
                    sizeOfType = sizeof(unsigned int);
                    func = (coremath) Core_uint32_bxor;
                    break;
                  case PAMI_SIGNED_INT:
                    sizeOfType = sizeof(int);
                    func = (coremath) Core_int32_bxor;
                    break;
                  case PAMI_SIGNED_LONG:
                    sizeOfType = sizeof(signed long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_int32_bxor;
                    else
                      func = (coremath) Core_int64_bxor;
                    break;
                  case PAMI_UNSIGNED_LONG:
                    sizeOfType = sizeof(unsigned long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_uint32_bxor;
                    else
                      func = (coremath) Core_uint64_bxor;
                    break;
                  case PAMI_DOUBLE:
                    sizeOfType = sizeof(double);
                    func = (coremath) Core_uint64_bxor;
                    break;
                  case PAMI_SIGNED_LONG_LONG:
                    sizeOfType = sizeof(long long);
                    func = (coremath) Core_int64_bxor;
                    break;
                  case PAMI_UNSIGNED_LONG_LONG:
                    sizeOfType = sizeof(unsigned long long);
                    func = (coremath) Core_uint64_bxor;
                    break;
                  case PAMI_UNSIGNED_SHORT:
                    sizeOfType = sizeof(unsigned short);
                    func = (coremath) Core_uint16_bxor;
                    break;
                  case PAMI_SIGNED_SHORT:
                    sizeOfType = sizeof(short);
                    func = (coremath) Core_int16_bxor;
                    break;
                  case PAMI_BYTE:
                  case PAMI_UNSIGNED_CHAR:
                    sizeOfType = sizeof(unsigned char);
                    func = (coremath) Core_uint8_bxor;
                    break;
                  case PAMI_SIGNED_CHAR:
                    sizeOfType = sizeof(char);
                    func = (coremath) Core_int8_bxor;
                    break;
                  case PAMI_LOGICAL1:
                    sizeOfType = 1;
                    func = (coremath) Core_uint8_bxor;
                    break;
                  case PAMI_LOGICAL2:
                    sizeOfType = 2;
                    func = (coremath) Core_uint16_bxor;
                    break;
                  case PAMI_LOGICAL4:
                    sizeOfType = 4;
                    func = (coremath) Core_uint32_bxor;
                    break;
                  case PAMI_LOGICAL8:
                    sizeOfType = 8;
                    func = (coremath) Core_uint64_bxor;
                    break;
                  default:
                    fprintf(stderr, "<          >CCMI::Adaptor::getReduceFunction(dtype %#X,op %#X)\n",
                            dtype, op);
                    PAMI_abort();
                }

              break;
            case PAMI_LAND:

              switch (dtype)
                {
                  case PAMI_FLOAT:
                    sizeOfType = sizeof(float);
                    func = (coremath) Core_uint32_land;
                    break;
                  case PAMI_UNSIGNED_INT:
                    sizeOfType = sizeof(unsigned int);
                    func = (coremath) Core_uint32_land;
                    break;
                  case PAMI_SIGNED_INT:
                    sizeOfType = sizeof(int);
                    func = (coremath) Core_int32_land;
                    break;
                  case PAMI_SIGNED_LONG:
                    sizeOfType = sizeof(signed long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_int32_land;
                    else
                      func = (coremath) Core_int64_land;
                    break;
                  case PAMI_UNSIGNED_LONG:
                    sizeOfType = sizeof(unsigned long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_uint32_land;
                    else
                      func = (coremath) Core_uint64_land;
                    break;
                  case PAMI_DOUBLE:
                    sizeOfType = sizeof(double);
                    func = (coremath) Core_uint64_land;
                    break;
                  case PAMI_SIGNED_LONG_LONG:
                    sizeOfType = sizeof(long long);
                    func = (coremath) Core_int64_land;
                    break;
                  case PAMI_UNSIGNED_LONG_LONG:
                    sizeOfType = sizeof(unsigned long long);
                    func = (coremath) Core_uint64_land;
                    break;
                  case PAMI_UNSIGNED_SHORT:
                    sizeOfType = sizeof(unsigned short);
                    func = (coremath) Core_uint16_land;
                    break;
                  case PAMI_SIGNED_SHORT:
                    sizeOfType = sizeof(short);
                    func = (coremath) Core_int16_land;
                    break;
                  case PAMI_UNSIGNED_CHAR:
                    sizeOfType = sizeof(unsigned char);
                    func = (coremath) Core_uint8_land;
                    break;
                  case PAMI_SIGNED_CHAR:
                    sizeOfType = sizeof(char);
                    func = (coremath) Core_int8_land;
                    break;
                  case PAMI_LOGICAL1:
                    sizeOfType = 1;
                    func = (coremath) Core_uint8_land;
                    break;
                  case PAMI_LOGICAL2:
                    sizeOfType = 2;
                    func = (coremath) Core_uint16_land;
                    break;
                  case PAMI_LOGICAL4:
                    sizeOfType = 4;
                    func = (coremath) Core_uint32_land;
                    break;
                  case PAMI_LOGICAL8:
                    sizeOfType = 8;
                    func = (coremath) Core_uint64_land;
                    break;
                  default:
                    fprintf(stderr, "<          >CCMI::Adaptor::getReduceFunction(dtype %#X,op %#X)\n",
                            dtype, op);
                    PAMI_abort();
                }

              break;
            case PAMI_LOR:

              switch (dtype)
                {
                  case PAMI_FLOAT:
                    sizeOfType = sizeof(float);
                    func = (coremath) Core_uint32_lor;
                    break;
                  case PAMI_UNSIGNED_INT:
                    sizeOfType = sizeof(unsigned int);
                    func = (coremath) Core_uint32_lor;
                    break;
                  case PAMI_SIGNED_INT:
                    sizeOfType = sizeof(int);
                    func = (coremath) Core_int32_lor;
                    break;
                  case PAMI_SIGNED_LONG:
                    sizeOfType = sizeof(signed long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_int32_lor;
                    else
                      func = (coremath) Core_int64_lor;
                    break;
                  case PAMI_UNSIGNED_LONG:
                    sizeOfType = sizeof(unsigned long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_uint32_lor;
                    else
                      func = (coremath) Core_uint64_lor;
                    break;
                  case PAMI_DOUBLE:
                    sizeOfType = sizeof(double);
                    func = (coremath) Core_uint64_lor;
                    break;
                  case PAMI_SIGNED_LONG_LONG:
                    sizeOfType = sizeof(long long);
                    func = (coremath) Core_int64_lor;
                    break;
                  case PAMI_UNSIGNED_LONG_LONG:
                    sizeOfType = sizeof(unsigned long long);
                    func = (coremath) Core_uint64_lor;
                    break;
                  case PAMI_UNSIGNED_SHORT:
                    sizeOfType = sizeof(unsigned short);
                    func = (coremath) Core_uint16_lor;
                    break;
                  case PAMI_SIGNED_SHORT:
                    sizeOfType = sizeof(short);
                    func = (coremath) Core_int16_lor;
                    break;
                  case PAMI_UNSIGNED_CHAR:
                    sizeOfType = sizeof(unsigned char);
                    func = (coremath) Core_uint8_lor;
                    break;
                  case PAMI_SIGNED_CHAR:
                    sizeOfType = sizeof(char);
                    func = (coremath) Core_int8_lor;
                    break;
                  case PAMI_LOGICAL1:
                    sizeOfType = 1;
                    func = (coremath) Core_uint8_lor;
                    break;
                  case PAMI_LOGICAL2:
                    sizeOfType = 2;
                    func = (coremath) Core_uint16_lor;
                    break;
                  case PAMI_LOGICAL4:
                    sizeOfType = 4;
                    func = (coremath) Core_uint32_lor;
                    break;
                  case PAMI_LOGICAL8:
                    sizeOfType = 8;
                    func = (coremath) Core_uint64_lor;
                    break;
                  default:
                    fprintf(stderr, "<          >CCMI::Adaptor::getReduceFunction(dtype %#X,op %#X)\n",
                            dtype, op);
                    PAMI_abort();
                }

              break;
            case PAMI_LXOR:

              switch (dtype)
                {
                  case PAMI_FLOAT:
                    sizeOfType = sizeof(float);
                    func = (coremath) Core_fp32_lxor;
                    break;
                  case PAMI_UNSIGNED_INT:
                    sizeOfType = sizeof(unsigned int);
                    func = (coremath) Core_uint32_lxor;
                    break;
                  case PAMI_SIGNED_INT:
                    sizeOfType = sizeof(int);
                    func = (coremath) Core_int32_lxor;
                    break;
                  case PAMI_SIGNED_LONG:
                    sizeOfType = sizeof(signed long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_int32_lxor;
                    else
                      func = (coremath) Core_int64_lxor;
                    break;
                  case PAMI_UNSIGNED_LONG:
                    sizeOfType = sizeof(unsigned long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_uint32_lxor;
                    else
                      func = (coremath) Core_uint64_lxor;
                    break;
                  case PAMI_DOUBLE:
                    sizeOfType = sizeof(double);
                    func = (coremath) Core_fp64_lxor;
                    break;
                  case PAMI_SIGNED_LONG_LONG:
                    sizeOfType = sizeof(long long);
                    func = (coremath) Core_int64_lxor;
                    break;
                  case PAMI_UNSIGNED_LONG_LONG:
                    sizeOfType = sizeof(unsigned long long);
                    func = (coremath) Core_uint64_lxor;
                    break;
                  case PAMI_UNSIGNED_SHORT:
                    sizeOfType = sizeof(unsigned short);
                    func = (coremath) Core_uint16_lxor;
                    break;
                  case PAMI_SIGNED_SHORT:
                    sizeOfType = sizeof(short);
                    func = (coremath) Core_int16_lxor;
                    break;
                  case PAMI_UNSIGNED_CHAR:
                    sizeOfType = sizeof(unsigned char);
                    func = (coremath) Core_uint8_lxor;
                    break;
                  case PAMI_SIGNED_CHAR:
                    sizeOfType = sizeof(char);
                    func = (coremath) Core_int8_lxor;
                    break;
                  case PAMI_LOGICAL1:
                    sizeOfType = 1;
                    func = (coremath) Core_uint8_lxor;
                    break;
                  case PAMI_LOGICAL2:
                    sizeOfType = 2;
                    func = (coremath) Core_uint16_lxor;
                    break;
                  case PAMI_LOGICAL4:
                    sizeOfType = 4;
                    func = (coremath) Core_uint32_lxor;
                    break;
                  case PAMI_LOGICAL8:
                    sizeOfType = 8;
                    func = (coremath) Core_uint64_lxor;
                    break;
                  default:
                    fprintf(stderr, "<          >CCMI::Adaptor::getReduceFunction(dtype %#X,op %#X)\n",
                            dtype, op);
                    PAMI_abort();
                }

              break;
            case PAMI_MAXLOC:

              switch (dtype)
                {
                  case PAMI_LOC_2INT:
                    func = (coremath) Core_int32_int32_maxloc;
                    sizeOfType = sizeof(int32_int32_t);
                    break;
                  case PAMI_LOC_SHORT_INT:
                    func = (coremath) Core_int16_int32_maxloc;
                    sizeOfType = sizeof(int16_int32_t);
                    break;
                  case PAMI_LOC_FLOAT_INT:
                    func = (coremath) Core_fp32_int32_maxloc;
                    sizeOfType = sizeof(fp32_int32_t);
                    break;
                  case PAMI_LOC_DOUBLE_INT:
                    func = (coremath) Core_fp64_int32_maxloc;
                    sizeOfType = sizeof(fp64_int32_t);
                    break;
                  case PAMI_LOC_LONG_INT:
                    if (sizeof(long) == 4)
                    {
                      func = (coremath) Core_int32_int32_maxloc;
                      sizeOfType = sizeof(int32_int32_t);
                    }
                    else // if (sizeof(long) == 8)
                    {
                      func = (coremath) Core_int64_int32_maxloc;
                      sizeOfType = sizeof(int64_int32_t);
                    }
                    break;
                  case PAMI_LOC_LONGDOUBLE_INT:
                    func = (coremath) Core_fp128_int32_maxloc;
                    sizeOfType = sizeof(fp128_int32_t);
                    break;
                  case PAMI_LOC_2FLOAT:
                    func = (coremath) Core_fp32_fp32_maxloc;
                    sizeOfType = sizeof(fp32_fp32_t);
                    break;
                  case PAMI_LOC_2DOUBLE:
                    func = (coremath) Core_fp64_fp64_maxloc;
                    sizeOfType = sizeof(fp64_fp64_t);
                    break;
                  default:
                    fprintf(stderr, "<          >CCMI::Adaptor::getReduceFunction(dtype %#X,op %#X)\n",
                            dtype, op);
                    PAMI_abort();
                }

              break;
            case PAMI_MINLOC:

              switch (dtype)
                {
                  case PAMI_LOC_2INT:
                    func = (coremath) Core_int32_int32_minloc;
                    sizeOfType = sizeof(int32_int32_t);
                    break;
                  case PAMI_LOC_SHORT_INT:
                    func = (coremath) Core_int16_int32_minloc;
                    sizeOfType = sizeof(int16_int32_t);
                    break;
                  case PAMI_LOC_FLOAT_INT:
                    func = (coremath) Core_fp32_int32_minloc;
                    sizeOfType = sizeof(fp32_int32_t);
                    break;
                  case PAMI_LOC_DOUBLE_INT:
                    func = (coremath) Core_fp64_int32_minloc;
                    sizeOfType = sizeof(fp64_int32_t);
                    break;
                  case PAMI_LOC_LONG_INT:
                    if (sizeof(long) == 4)
                    {
                      func = (coremath) Core_int32_int32_minloc;
                      sizeOfType = sizeof(int32_int32_t);
                    }
                    else // if (sizeof(long) == 8)
                    {
                      func = (coremath) Core_int64_int32_minloc;
                      sizeOfType = sizeof(int64_int32_t);
                    }
                    break;
                  case PAMI_LOC_LONGDOUBLE_INT:
                    func = (coremath) Core_fp128_int32_minloc;
                    sizeOfType = sizeof(fp128_int32_t);
                    break;
                  case PAMI_LOC_2FLOAT:
                    func = (coremath) Core_fp32_fp32_minloc;
                    sizeOfType = sizeof(fp32_fp32_t);
                    break;
                  case PAMI_LOC_2DOUBLE:
                    func = (coremath) Core_fp64_fp64_minloc;
                    sizeOfType = sizeof(fp64_fp64_t);
                    break;
                  default:
                    fprintf(stderr, "<          >CCMI::Adaptor::getReduceFunction(dtype %#X,op %#X)\n",
                            dtype, op);
                    PAMI_abort();
                }

              break;
            case PAMI_PROD:

              switch (dtype)
                {
                  case PAMI_SIGNED_INT:
                    sizeOfType = sizeof(int);
                    func = (coremath) Core_int32_prod;
                    break;
                  case PAMI_SIGNED_LONG:
                    sizeOfType = sizeof(signed long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_int32_prod;
                    else
                      func = (coremath) Core_int64_prod;
                    break;
                  case PAMI_UNSIGNED_LONG:
                    sizeOfType = sizeof(unsigned long);
                    if(sizeOfType == 4)
                      func = (coremath) Core_uint32_prod;
                    else
                      func = (coremath) Core_uint64_prod;
                    break;
                  case PAMI_UNSIGNED_INT:
                    sizeOfType = sizeof(unsigned int);
                    func = (coremath) Core_uint32_prod;
                    break;
                  case PAMI_SIGNED_LONG_LONG:
                    sizeOfType = sizeof(long long);
                    func = (coremath) Core_int64_prod;
                    break;
                  case PAMI_UNSIGNED_LONG_LONG:
                    sizeOfType = sizeof(unsigned long long);
                    func = (coremath) Core_uint64_prod;
                    break;
                  case PAMI_SIGNED_SHORT:
                    sizeOfType = sizeof(short);
                    func = (coremath) Core_int16_prod;
                    break;
                  case PAMI_UNSIGNED_SHORT:
                    sizeOfType = sizeof(unsigned short);
                    func = (coremath) Core_uint16_prod;
                    break;
                  case PAMI_DOUBLE:
                    sizeOfType = sizeof(double);
                    func = (coremath) Core_fp64_prod;
                    break;
                  case PAMI_FLOAT:
                    sizeOfType = sizeof(float);
                    func = (coremath) Core_fp32_prod;
                    break;
                  case PAMI_LONG_DOUBLE:
                    sizeOfType = sizeof(long double);
                    func = (coremath) Core_fp128_prod;
                    break;

                  case PAMI_SIGNED_CHAR:
                    sizeOfType = sizeof(char);
                    func = (coremath) Core_int8_prod;
                    break;
                  case PAMI_UNSIGNED_CHAR:
                    sizeOfType = sizeof(unsigned char);
                    func = (coremath) Core_uint8_prod;
                    break;
                  case PAMI_SINGLE_COMPLEX:
                    sizeOfType = sizeof(fp32_fp32_t);
                    func = (coremath) Core_fp32_fp32_cmplx_prod;
                    break;
                  case PAMI_DOUBLE_COMPLEX:
                    sizeOfType = sizeof(fp64_fp64_t);
                    func = (coremath) Core_fp64_fp64_cmplx_prod;
                    break;
                  default:
                    fprintf(stderr, "<          >CCMI::Adaptor::getReduceFunction(dtype %#X,op %#X)\n",
                            dtype, op);
                    PAMI_abort();
                }

              break;
            case PAMI_NOOP:
              func = (coremath) NULL;

              switch (dtype)
                {
                  case PAMI_BYTE:
                  case PAMI_SIGNED_CHAR:
                    sizeOfType = sizeof(char);
                    break;
                  case PAMI_UNSIGNED_CHAR:
                    sizeOfType = sizeof(unsigned char);
                    break;
                  case PAMI_SIGNED_SHORT:
                    sizeOfType = sizeof(short);
                    break;
                  case PAMI_UNSIGNED_SHORT:
                    sizeOfType = sizeof(unsigned short);
                    break;
                  case PAMI_SIGNED_INT:
                    sizeOfType = sizeof(int);
                    break;
                  case PAMI_UNSIGNED_INT:
                    sizeOfType = sizeof(unsigned int);
                    break;
                  case PAMI_SIGNED_LONG:
                    sizeOfType = sizeof(signed long);
                    break;
                  case PAMI_UNSIGNED_LONG:
                    sizeOfType = sizeof(unsigned long);
                    break;
                  case PAMI_SIGNED_LONG_LONG:
                    sizeOfType = sizeof(long long);
                    break;
                  case PAMI_UNSIGNED_LONG_LONG:
                    sizeOfType = sizeof(unsigned long long);
                    break;
                  case PAMI_FLOAT:
                    sizeOfType = sizeof(float);
                    break;
                  case PAMI_DOUBLE:
                    sizeOfType = sizeof(double);
                    break;
                  case PAMI_LONG_DOUBLE:
                    sizeOfType = sizeof(long double);
                    break;
                  case PAMI_LOGICAL1:
                    sizeOfType = 1;
                    break;
                  case PAMI_LOGICAL2:
                    sizeOfType = 2;
                    break;
                  case PAMI_LOGICAL4:
                    sizeOfType = 4;
                    break;
                  case PAMI_LOGICAL8:
                    sizeOfType = 8;
                    break;
                  case PAMI_SINGLE_COMPLEX:
                    sizeOfType = sizeof(fp32_fp32_t);
                    break;
                  case PAMI_DOUBLE_COMPLEX:
                    sizeOfType = sizeof(fp64_fp64_t);
                    break;
                  case PAMI_LOC_2INT:
                    sizeOfType = sizeof(int32_int32_t);
                    break;
                  case PAMI_LOC_SHORT_INT:
                    sizeOfType = sizeof(int16_int32_t);
                    break;
                  case PAMI_LOC_FLOAT_INT:
                    sizeOfType = sizeof(fp32_int32_t);
                    break;
                  case PAMI_LOC_DOUBLE_INT:
                    sizeOfType = sizeof(fp64_int32_t);
                    break;
                  case PAMI_LOC_LONG_INT:
                    if (sizeof(long) == 4)
                      sizeOfType = sizeof(int32_int32_t);
                    else // if (sizeof(long) == 8)
                      sizeOfType = sizeof(int64_int32_t);
                    break;
                  case PAMI_LOC_LONGDOUBLE_INT:
                    sizeOfType = sizeof(fp128_int32_t);
                    break;
                  case PAMI_LOC_2FLOAT:
                    sizeOfType = sizeof(fp32_fp32_t);
                    break;
                  case PAMI_LOC_2DOUBLE:
                    sizeOfType = sizeof(fp64_fp64_t);
                    break;
                  default:
                    fprintf(stderr, "<          >CCMI::Adaptor::getReduceFunction(dtype %#X,op %#X)\n",
                            dtype, op);
                    PAMI_abort();
                }

              break;
            default:
              fprintf(stderr, "<          >CCMI::Adaptor::getReduceFunction(dtype %#X,op %#X)\n",
                      dtype, op);
              PAMI_abort();
          }

        TRACE_ADAPTOR ((stderr, "<          >CCMI::Adaptor::getReduceFunction()"
                        " size %#X, function %p)\n",
                        sizeOfType, func));
      }
    }
  }
}

#endif // __algorithms_protocols_allreduce_ReduceFunctions_h__
