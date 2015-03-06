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
 * \file math/FloatUtil.h
 * \brief Floating point operation utilities
 *
 * Detailed description.
 */
#ifndef __math_FloatUtil_h__
#define __math_FloatUtil_h__
#include <stdint.h>
/* ********************************************************* */
/* Datatype-to-tree conversion macros                        */
/* from bglsw/sysbringup/tree/include/BGL_TreeReduction.h    */
/* ********************************************************* */

/* ********************************************************* */
/* DOUBLE_MACROS                                             */
/* ********************************************************* */
#define DOUBLE_BITS_SIGN      (0x8000000000000000ull)
#define DOUBLE_NEG(u) ((u)&DOUBLE_BITS_SIGN)
#define DOUBLE_POS(u) (!((u)&DOUBLE_BITS_SIGN))
#define DOUBLE_BITS_EXP       (0x7FF0000000000000ull)
#define DOUBLE_BITS_IMPLICIT1 (0x0010000000000000ull)
#define DOUBLE_BITS_MANTISSA  (0x000FFFFFFFFFFFFFull)
#define DOUBLE_BITS_MANTISSA0 (0x000FFFFF00000000ull)
#define DOUBLE_BITS_MANTISSA1 (0x00000000FFFFFFFFull)
#define DOUBLE_SHIFT_SIGN      (63)
#define DOUBLE_SHIFT_EXP       (52)
#define DOUBLE_SHIFT_MANTISSA0 (32)
#define DOUBLE_SIGN(u)       ((u) >> DOUBLE_SHIFT_SIGN) // Result is 1 or 0.
#define DOUBLE_EXP(u)       (((u) & DOUBLE_BITS_EXP) >> DOUBLE_SHIFT_EXP)
#define DOUBLE_EXP_MAX       (2047)
#define DOUBLE_MANTISSA(u)   ((u) & DOUBLE_BITS_MANTISSA)
#define DOUBLE_MANTISSA0(u) (((u) & DOUBLE_BITS_MANTISSA0) >> DOUBLE_SHIFT_MANTISSA0)
#define DOUBLE_MANTISSA1(u)  ((u) & DOUBLE_BITS_MANTISSA1)
#define SET_DOUBLE_EXP(u,r)  ((u) &= ~DOUBLE_BITS_EXP, ((u) |= ( ((uint64_t)(r) \
                             << DOUBLE_SHIFT_EXP) & DOUBLE_BITS_EXP) ) )
#define SET_DOUBLE_MANTISSA(u,r)  ((u) &= ~DOUBLE_BITS_MANTISSA, ((u) |= \
                                  ((uint64_t)(r) & DOUBLE_BITS_MANTISSA) ) )
#define DOUBLE_NAN(u)  ( DOUBLE_EXP(u) == DOUBLE_EXP_MAX  &&  DOUBLE_MANTISSA(u) )
#define DOUBLE_ZERO(u) ((u&~DOUBLE_BITS_SIGN)==0)
#define DOUBLE_NAN_VALUE      (0x7FF1000000000000ull)
#define DOUBLE_POS_INF_VALUE  (0x7FF0000000000000ull)
#define DOUBLE_NEG_INF_VALUE  (0xFFF0000000000000ull)
#define DOUBLE_IGNORE_LSB(u)  ( (*(uint64_t*)&(u)) & (0xFFFFFFFFFFFFFFFEull))

/* ********************************************************* */
/* FLOAT_MACROS                                             */
/* ********************************************************* */
#define FLOAT_BITS_SIGN      (0x80000000ul)
#define FLOAT_NEG(u) ((u)&FLOAT_BITS_SIGN)
#define FLOAT_POS(u) (!((u)&FLOAT_BITS_SIGN))
#define FLOAT_BITS_EXP       (0x7F800000ul)
#define FLOAT_BITS_IMPLICIT1       (0x00800000ul)
#define FLOAT_BITS_MANTISSA  (0x007FFFFFul)
#define FLOAT_SHIFT_SIGN      (31)
#define FLOAT_SHIFT_EXP       (23)
#define FLOAT_SIGN(u)       ((u) >> FLOAT_SHIFT_SIGN)
#define FLOAT_EXP(u)       (((u) & FLOAT_BITS_EXP) >> FLOAT_SHIFT_EXP)
#define FLOAT_EXP_MAX       (255)
#define FLOAT_MANTISSA(u)   ((u) & FLOAT_BITS_MANTISSA)
#define SET_FLOAT_EXP(u,r)  ((u) &= ~FLOAT_BITS_EXP, ((u) |= ( ((uint32_t)(r) \
                            << FLOAT_SHIFT_EXP) & FLOAT_BITS_EXP) ) )
#define SET_FLOAT_MANTISSA(u,r)  ((u) &= ~FLOAT_BITS_MANTISSA, ((u) |= \
                                 ( (uint32_t)(r) & FLOAT_BITS_MANTISSA) ) )
#define FLOAT_NAN(u)   ( FLOAT_EXP(u) == FLOAT_EXP_MAX  &&  FLOAT_MANTISSA(u) )
#define FLOAT_NAN_VALUE      (0x7F810000ul)
#define FLOAT_POS_INF_VALUE  (0x7F800000ul)
#define FLOAT_NEG_INF_VALUE  (0xFF800000ul)
#define FLOAT_IGNORE_LSB(u)  ( (*(uint32_t*)&(u)) & 0xFFFFFFFE)
#define FLOAT_IGNORE_MANTISSA(u)  ( (*(uint32_t*)&(u)) & ~FLOAT_BITS_MANTISSA)
#define FLOAT_ZERO(u) ((u&~FLOAT_BITS_SIGN)==0)

#define ADD_OP_T  uint32_t
#define ADD_OP2_T uint64_t
#define OP_FFFF  (0xFFFFFFFFu)
#define OP_10000 (0x100000000ull)

#define BGL_TR_HUMUNGOUS	(0x0000000040000000ull)
#define BGL_TR_BAD_OPERAND	(0x0000800000000000ull)

#ifdef __bgq__
#include <hwi/include/bqc/A2_inlines.h>
#endif

/* ***********************************/
/* XXX replace with cntlz instruction*/
/* ***********************************/

#ifndef __bgq__
/* This function signature collides with an A2 inline function (bgq) */
static inline uint64_t
cntlz64 (uint64_t u)
{
  int i;
  for (i = 0; i < 64 && 0 == (u & 0x8000000000000000ull); i++)
    {
      u = u << 1;
    }
  return i;
}
#endif
/* ***********************************/
/* Returns 32 if u==0.               */
/* ***********************************/
#ifndef __bgq__
/* This function signature collides with an A2 inline function (bgq) */
static inline uint32_t
cntlz32 (uint32_t x)
{
  unsigned lz;
asm ("cntlzw %0,%1": "=r" (lz):"r" (x));
  return lz;
}
#endif
/* ***********************************************/
/* Replace by a 440 instruction.                 */
/* Returns 32 if u==0.                        */
/* ***********************************************/
static inline uint32_t
cntlz96 (const uint32_t u[3])
{
  int i;
  i = cntlz32 (u[0]);
  if (i == 32)
    {
      i += cntlz32 (u[1]);
      if (i == 64)
        {
          i += cntlz32 (u[2]);
        }
    }
  return i;
}

int double2uint(double *d, int ulen, uint64_t *u);
int uint2double(int participants, double *d, int ulen, uint64_t *u);

#endif
