/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2011                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

// BNDReduce.h:
// - is a bitwise exact imitation of collective reductions in the network
// - is verified by void oceanGen::testBgNDReduce() in $BGQHOME/bgq/hwsim_unitsim/src/fusion/msg_unit/mut/oceanGen.C
// - is the original C++ version. A later port to C is in BgNDReduce_C.h

#ifndef __BG_ND_REDUCE_H__
#define __BG_ND_REDUCE_H__

#include <stdint.h>
#include "ArbitraryLengthNumber.h"

#include "BgNDReduceConstants.h"

#include "assert.h"
#undef NDEBUG

 

class BgNDReduce 
{
public:

  // Default BgNDReduce expects all input data in vContribution and produces all output data in vResult
  // in BGQ-endianness.
  // Following flag indicates that data is in x86 endianness.
  // So when on x86, reading and writing the data using 32-bit integers, initially do:
  //    BgNDReduce::x86endian = 32;
  // Only 32bit x86 endianness is currently supported, so the only valid values are 0 and 32.
  static int x86endian;

  // Routines assume 32bit data is aligned as such. Is this assumption ok?

  static int getFlip(void);

  // Return value is or-reduce of vExceptions.
  static uint64_t vectorReduce(
    int numElements,      // number of elements in vector contributed by each participant.
    int elementWords,     // length in 4-byte words of each element. So uint32_t has elementWords=1.
    int32_t opCode,       // operator for reduce
    int numParticipants,  // number of participants in reduction
    char *Tree,           // See below
    void **vContribution, // Array of pointers, with one pointer for each participant. Each pointer points to the contribution vector of a participant. Each contribuion vector has length numElements.
    uint64_t *vFlagsContribution, // Array of input flags, with one flag for each participant. Can be NULL.
    uint64_t vFlagsResult, // Input flags for result.
    void *vResult,         // Vector result of reduction. Vector length is numElements.
    uint64_t *vExceptions  // Any exceptions in each reduction. Each contribuion vector has length numElements.  Vector length is numElements.
  );

  // Notes on above Tree argument:
  // . The Tree argument is needed for BG/Q bitwise-identical results and exceptions for:
  //   - floating point add, due to order-dependant rounding and/or over/underflow.
  //   - signed integer add, if over/underflow depends on reduction order.
  // . The Tree argument can also be used for other reduction operations.
  //   In this case, the result should not depend on the reduction order given in the string.
  // . If Tree==NULL, the participants are reduced in linear order 0+1+2+....
  // . If Tree!=NULL, then Tree string specifies redcution-order using following syntax:
  //   - Ignored characters: ()[]{} whitespace
  //   - 'decimal_number' refers to a participant
  //   - 'S' pushes onto stack
  //   - '+' pops and adds top 2 elements from stack and pushes result onto stack
  //   Example below to add 12 participants in order 0-11
  //       "0S 1S + 2S + 3S + 4S + 5S + 6S + 7S + 8S + 9S + 10S + 11S +";
  //   Example below to add all 12 participants on a single BG/Q node (See Issue 148).
  //       "[ { (0S 1S +) (2S 3S +) +}  { (4S 5S +) (6S 7S +) +}  +] [{(8S 9S +) (10S 11S +) +] +}";



  // Return value is Exceptions in this reduction.
  static uint64_t scalarReduce(
    int offset, // offset into each contribution, in units of unit32_t.
    int elementWords,
    int32_t opCode,
    int numParticipants,
    char *Tree,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
  );

  // Return value is Exceptions in this reduction.
  static uint64_t logicOpScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
  );

  // Return value is Exceptions in this reduction.
  static uint64_t minORmaxScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
  );

  // Return value is Exceptions in this reduction.
  static uint64_t u_addScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
  );

// Return value is Exceptions in this reduction.
static uint64_t treeAddScalarReduce(
    int numParticipants,
    char *Tree,
    uint32_t *inCarry,
    uint32_t *outCarry,
    uint32_t *signedOverflow,
    uint32_t *vContribution,
    uint32_t &Result
    );

  // Return value is Exceptions in this reduction.
  static uint64_t addScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    char *Tree,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
  );

  // Return value is Exceptions in this reduction.
  static uint64_t fp_ScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
  );

  static uint64_t verify_uint_ScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result);

  static uint64_t verify_int_ScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result);

  // Return value is Exceptions in this reduction.
  static uint64_t fp_addScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
  );

  // Does not recognize input Nans, if any, and thus always returns 0. Ie no exceptions.
  static uint64_t native_fp_minScalarReduce(
    int numParticipants,
    double *vContribution,
    double &Result
    );


  // Does not recognize input Nans, if any, and thus always returns 0. Ie no exceptions.
  static uint64_t native_fp_maxScalarReduce(
    int numParticipants,
    double *vContribution,
    double &Result
    );


  // Does not allow input Nans. Does raise exceptions as needed.
  static uint64_t native_fp_addScalarReduce(
    int numParticipants,
    double *vContribution,
    double &Result
    );


  // //////////////////////////

  /* ---------------------------------
http://www.psc.edu/general/software/packages/ieee/ieee.php
Double Precision
The IEEE double precision floating point standard representation requires a 64 bit word, which may be represented as numbered from 0 to 63, left to right. The first bit is the sign bit, S, the next eleven bits are the exponent bits, 'E', and the final 52 bits are the fraction 'F':
  S EEEEEEEEEEE FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
  0 1        11 12                                                63

The value V represented by the word may be determined as follows:
If E=2047 and F is nonzero, then V=NaN ("Not a number") 
If E=2047 and F is zero and S is 1, then V=-Infinity 
If E=2047 and F is zero and S is 0, then V=Infinity 
If 0<E<2047 then V=(-1)**S * 2 ** (E-1023) * (1.F) where "1.F" is intended to represent the binary number created by prefixing F with an implicit leading 1 and a binary point. 
If E=0 and F is nonzero, then V=(-1)**S * 2 ** (-1022) * (0.F) These are "unnormalized" values. 
If E=0 and F is zero and S is 1, then V=-0 
If E=0 and F is zero and S is 0, then V=0 
--------------------------------- */

  static const uint64_t fp_s_bit  =               1ull<<63;
  static const uint64_t fp_e_bits =           0x7FFull<<52;
  static const uint64_t fp_m_bits = 0xFFFFFFFFFFFFFull;

  static uint64_t fp_sign(uint64_t u) {return (u&fp_s_bit) >> 63;}   // Returns 0 means +ve or 1 means -ve

  static uint64_t fp_exp(uint64_t u) {return (u&fp_e_bits)>>52;}

  static uint64_t fp_mant(uint64_t u) {return u&fp_m_bits;}

  static void fp_set_sign(uint64_t s, uint64_t &u) {assert(s<=1);                  u&=~fp_s_bit; u |= s<<63;}
  static void fp_set_exp( uint64_t e, uint64_t &u) {assert(e<=0x7FF);              u&=~fp_e_bits; u |= e<<52;}
  static void fp_set_mant(uint64_t m, uint64_t &u) {assert(m<=0xFFFFFFFFFFFFFull); u&=~fp_m_bits; u |= m;}

  static void fp_set_sem(uint64_t s, uint64_t e, uint64_t m, uint64_t &u) {fp_set_sign(s,u); fp_set_exp(e,u); fp_set_mant(m,u);}

  static int fp_is_Inf(uint64_t u) {return (u&fp_e_bits)==fp_e_bits && !fp_mant(u);}

  static int fp_is_Nan(uint64_t u) {return (u&fp_e_bits)==fp_e_bits &&  fp_mant(u);}

  static int fp_is_Zero(uint64_t u) {return !(u&~fp_s_bit);}

  static int fp_is_Denorm(uint64_t u) {return !fp_exp(u) && fp_mant(u);}
  // //////////////////////////

  static int firstBit(uint64_t u) {
    for (int i=0; i<=63; i++) {
      if ( ( 1ull<<(63-i) ) & u ) return i;
    }
    return -1;
  }

  // Returns number of elements read
  // -ve on errors
  static int read12fromNuts(char *filename, int max, uint32_t *(d[12]), uint32_t answer[], uint32_t rec[]);

protected:

private:

};

#endif


