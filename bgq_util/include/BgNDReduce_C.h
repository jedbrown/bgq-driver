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

// BNDReduce_C.h:
// - is a bitwise exact imitation of collective reductions in the network
// - except for *multiNode*, *allNode*, etc. routines, is a later C port of the C++ BgNDReduce.h. See that for more comments.
// - is implemented in:        bgq_util/src/BgNDReduce_C.c
//   with verify* routines in: bgq_util/src/BgNDReduce_C_verify.c
//   . The verify* routines are called for example from:
//      . $BGQHOME/bgq/hwverify/src/bringup/mu_nd/tests/BgNDReduce_test/BgNDReduce_test.c
//      . $BGQHOME/bgq/hwverify/src/fullchip/mu_nd/BgNDReduce/test_main.c


#ifndef __BG_ND_REDUCE_H__
#define __BG_ND_REDUCE_H__

#include <stdint.h>
#include <hwi/include/common/compiler_support.h> // __INLINE__

#include "BgRandom.h" //MTRandomArg
#include "BgNDReduceConstants.h"

#include "assert.h"
#undef NDEBUG

__BEGIN_DECLS


#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef max
#define max(a,b) (((a)>(b))?(a):(b))
#endif



  // Default BgNDReduce expects all input data in vContribution and produces all output data in vResult
  // in BGQ-endianness.
  // Following global indicates that data is in x86 endianness.
  // So when on x86, reading and writing the data using 32-bit integers, initially do:
  //    BgNDReduce::x86endian = 32;
  // Only 32bit x86 endianness is currently supported, so the only valid values are 0 and 32.
  extern int BgNDReduce_x86endian;

  // Routines assume 32bit data is aligned as such. Is this assumption ok?

  int BgNDReduce_getFlip(void);

  // Return value is or-reduce of vExceptions.
  uint64_t BgNDReduce_vectorReduce(
    int numElements,      // number of elements in vector contributed by each participant.
    int elementWords,     // length in 4-byte words of each element. So uint32_t has elementWords=1.
    int32_t opCode,       // operator for reduce
    int numParticipants,  // number of participants in reduction
    char *Tree,           // See below
    void **vContribution, // Array of pointers, with one pointer for each participant. Each pointer points to the contribution vector of a participant. Each contribuion vector has length numElements.
    uint64_t *vFlagsContribution, // Array of input flags, with one flag for each participant. Can be NULL. Currently, the only flag NdOrderBGNdReduce indicates that multi-word-integer-add contribution is in network order, not in default processor order.
    uint64_t vFlagsResult, // Input flags for result.  Currently, the only flag NdOrderBGNdReduce indicates that multi-word-integer-add result should be provided in network order, not in default processor order.
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
  uint64_t BgNDReduce_scalarReduce(
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
  uint64_t BgNDReduce_logicOpScalarReduce(
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
  uint64_t BgNDReduce_minORmaxScalarReduce(
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
  uint64_t BgNDReduce_u_addScalarReduce(
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
uint64_t BgNDReduce_treeAddScalarReduce(
    int numParticipants,
    char *Tree,
    uint32_t *inCarry,
    uint32_t *outCarry,
    uint32_t *signedOverflow,
    uint32_t *vContribution,
    uint32_t *Result
    );

  // Return value is Exceptions in this reduction.
  uint64_t BgNDReduce_addScalarReduce(
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
  uint64_t BgNDReduce_fp_ScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
  );

  uint64_t BgNDReduce_verify_uint_ScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result);

  uint64_t BgNDReduce_verify_int_ScalarReduce(
    int offset,
    int elementWords,
    int32_t opCode,
    int numParticipants,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result);

  // Return value is Exceptions in this reduction.
  uint64_t BgNDReduce_fp_addScalarReduce(
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
  uint64_t BgNDReduce_native_fp_minScalarReduce(
    int numParticipants,
    double *vContribution,
    double *Result
    );


  // Does not recognize input Nans, if any, and thus always returns 0. Ie no exceptions.
  uint64_t BgNDReduce_native_fp_maxScalarReduce(
    int numParticipants,
    double *vContribution,
    double *Result
    );


  // Does not allow input Nans. Does raise exceptions as needed.
  uint64_t BgNDReduce_native_fp_addScalarReduce(
    int numParticipants,
    double *vContribution,
    double *Result
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

#if 0
  const uint64_t fp_s_bit  =               1ull<<63;
  const uint64_t fp_e_bits =           0x7FFull<<52;
  const uint64_t fp_m_bits = 0xFFFFFFFFFFFFFull;
#else
#define NUMBITS_EXP  (           11          )
#define NUMBITS_MANT (           52          )
#define MAX_EXP      (           0x7FFull    )
#define MAX_MANT     ( 0xFFFFFFFFFFFFFull    )
#define fp_s_bit     (               1ull<<63)
#define fp_e_bits    (            MAX_EXP<<52)
#define fp_m_bits    (           MAX_MANT    )
#endif

__INLINE__ uint64_t fp_trim_exp(int64_t i) {return min(MAX_EXP,max(0,i));} // force exp value into valid range. -ve is forced to 0. >MAX_EXP is forced to MAX_EXP.

__INLINE__ uint64_t fp_trim_mant(int64_t i) {return min(MAX_MANT,max(0,i));} // force mant value into valid range. -ve is forced to 0. >MAX_MANT is forced to MAX_MANT.

  __INLINE__ uint64_t fp_is_Neg(uint64_t u) {return (u&fp_s_bit) >> 63;}   // Returns 0 means +ve or 1 means -ve
  __INLINE__ uint64_t fp_is_Pos(uint64_t u) {return !fp_is_Neg(u);}   // Returns 1 means +ve or 0 means -ve
  __INLINE__ uint64_t fp_sign(uint64_t u) {return fp_is_Neg(u);}   // DEPRECATED NAME. Returns 0 means +ve or 1 means -ve

  __INLINE__ uint64_t fp_exp(uint64_t u) {return (u&fp_e_bits)>>52;}

  __INLINE__ uint64_t fp_mant(uint64_t u) {return u&fp_m_bits;}

  __INLINE__ void fp_set_sign(uint64_t s, uint64_t *u) {assert(s<=1);                  *u&=~fp_s_bit; *u |= s<<63;}
  __INLINE__ void fp_set_exp( uint64_t e, uint64_t *u) {assert(e<=0x7FF);              *u&=~fp_e_bits; *u |= e<<52;}
  __INLINE__ void fp_set_mant(uint64_t m, uint64_t *u) {assert(m<=0xFFFFFFFFFFFFFull); *u&=~fp_m_bits; *u |= m;}

  __INLINE__ void fp_set_sem(uint64_t s, uint64_t e, uint64_t m, uint64_t *u) {fp_set_sign(s,u); fp_set_exp(e,u); fp_set_mant(m,u);}

  __INLINE__ int fp_is_Inf(uint64_t u) {return (u&fp_e_bits)==fp_e_bits && !fp_mant(u);}

  __INLINE__ int fp_is_Nan(uint64_t u) {return (u&fp_e_bits)==fp_e_bits &&  fp_mant(u);}

  __INLINE__ int fp_is_Zero(uint64_t u) {return !(u&~fp_s_bit);}

  // fp_is_Denorm() does NOT include 0.
  __INLINE__ int fp_is_Denorm(uint64_t u) {return !fp_exp(u) && fp_mant(u);}

  __INLINE__ int fp_is_non0Norm(uint64_t u) {return !fp_is_Zero(u) && fp_exp(u)>0 && fp_exp(u)<MAX_EXP;}


  // double_is_* routines exist since had much trouble with stale data when type casting.
  __INLINE__ int double_is_Inf(double d) {union {double d; uint64_t u;} u; u.d = d; return fp_is_Inf(u.u);}
  __INLINE__ int double_is_Nan(double d) {union {double d; uint64_t u;} u; u.d = d; return fp_is_Nan(u.u);}

  // //////////////////////////

  __INLINE__ int firstBit(uint64_t u) {
    int i;
    for (i=0; i<=63; i++) {
      if ( ( 1ull<<(63-i) ) & u ) return i;
    }
    return -1;
  }


// BEGIN code new to BgNDReduce_C.h, not ported from C++ BgNDReduce.h --------------------------

  // Notes on the argument: int (*willParticipate)(void *parms, int node, int link)
  // - the parms argument is passed thru from the user.
  // - the input link argument 0-11 means A-, A+, ..., E-, E+, IO link, local input. See bgq/hwi/include/bqc/classroute.h
  // - the output <0 means no contribution.
  //   Other output refers to the node providing the contribution.
  //   So for local input, (*willParticipate)(parms,nodeX, 11) must return <0 or nodeX.

  // Return value is Exceptions in this reduction.
  // Starting at thisNode, this will recursively call each participating node.
  // So user should give root node value for thisNode.
  // i: means input, o: means output, io: means modified.
  uint64_t BgNDReduce_multiNodeScalarReduce(
    int thisNode,         // i: The node to be reduced.
    int (*willParticipate)(void *parms, int node, int link), // i: See above.
    void *willParticipate_parms,
    int maxParticipant,   // i: Just an assert() on value returned by int (*willParticipate)(void *parms, int node, int link)
    uint64_t **vNodeExceptions,  // o: Can be NULL. Array of pointers, with one pointer for each participant. Each pointer points to the partial-reduction exception vector of a participant. Each exception vector has length numElements and is accessed at offset/elementWords (historical ugliness).
    void **vNodeResult,      // o: Can be NULL. Array of pointers, with one pointer for each participant. Each pointer points to the partial-reduction result vector of a participant. Each result vector has length numElements and is accessed at offset.

    int participationGeneration, // i:
    int *nodeParticipation, // io: 0-maxParticipant array. 0 on entry. Counts how often each node participates. Illegal for any node to participate twice. 
    int *totalParticipation, // io: Total number of participating nodes.

    int offset,           // i: offset into each contribution, in units of unit32_t.
    int elementWords,     // i: length in 4-byte words of each element. So uint32_t has elementWords=1.												       
    int32_t opCode,	  // i: operator for reduce
    char *Tree,		  // i: The 12-way within-node Tree. See above for format of Tree string.
    void **vContribution, // i: Array of pointers, with one pointer for each participant. Each pointer points to the contribution vector of a participant. Each contribution vector has length numElements and is accessed at offset.
    uint64_t *vFlagsContribution, // i: Array of input flags, with one flag for each participant. Can be NULL. Currently, the only flag NdOrderBGNdReduce indicates that multi-word-integer-add contribution is in network order, not in default processor order.
    uint64_t vFlagsResult, // i: Input flags for result.  Currently, the only flag NdOrderBGNdReduce indicates that multi-word-integer-add result should be provided in network order, not in default processor order.
    void *Result                // o: Result of reduction.
  );


  // Return value is Exceptions in this reduction.
  // Uses and is like above BgNDReduce_multiNodeScalarReduce(), except that vFlatContribution,vFlatNodeExceptions,vFlatNodeResult are simple arrays of elements indexed by participant. 
  uint64_t BgNDReduce_multiNodeFlatScalarReduce(
    int thisNode,
    int (*willParticipate)(void *parms, int node, int link),
    void *willParticipate_parms,
    int maxParticipant,
    uint64_t *vFlatNodeExceptions,
    void *vFlatNodeResult,
    int participationGeneration,
    int *nodeParticipation,
    int *totalParticipation,
    int elementWords,
    int32_t opCode,
    char *Tree,
    void *vFlatContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
  );


  // Return value is Exceptions in this reduction.
  // Uses above BgNDReduce_multiNodeScalarReduce() and ensures that all nodes 0-maxParticipant are in reduction.
  uint64_t BgNDReduce_allNodeScalarReduce(
    int rootNode,
    int (*willParticipate)(void *parms, int node, int link),
    void *willParticipate_parms,
    int maxParticipant,
    uint64_t **vNodeExceptions,
    void **vNodeResult,

    int offset,
    int elementWords,
    int32_t opCode,
    char *Tree,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
  );


  // Return value is Exceptions in this reduction.
  // Uses above BgNDReduce_multiNodeScalarReduce() and ensures that numNodesInSubComm nodes, which are true in IsNodeInSubComm[], are in reduction.
  uint64_t BgNDReduce_subCommScalarReduce(
    int numNodesInSubComm,
    int *IsNodeInSubComm,
    int rootNode,
    int (*willParticipate)(void *parms, int node, int link),
    void *willParticipate_parms,
    int maxParticipant,
    uint64_t **vNodeExceptions,
    void **vNodeResult,

    int offset,
    int elementWords,
    int32_t opCode,
    char *Tree,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
  );


  // Return value is Exceptions in this reduction.
  // Uses above BgNDReduce_allNodeScalarReduce(), except that vFlatContribution,vFlatNodeExceptions,vFlatNodeResult are simple arrays of elements indexed by participant.
  uint64_t BgNDReduce_allNodeFlatScalarReduce(
    int rootNode,
    int (*willParticipate)(void *parms, int node, int link),
    void *willParticipate_parms,
    int maxParticipant,
    uint64_t *vFlatNodeExceptions,
    void *vFlatNodeResult,

    int elementWords,
    int32_t opCode,
    char *Tree,
    void *vFlatContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
  );


  // Return value is Exceptions in this reduction.
  // Uses above BgNDReduce_subCommScalarReduce(), except that vFlatContribution,vFlatNodeExceptions,vFlatNodeResult are simple arrays of elements indexed by participant.
  uint64_t BgNDReduce_subCommFlatScalarReduce(
    int numNodesInSubComm,
    int *IsNodeInSubComm,
    int rootNode,
    int (*willParticipate)(void *parms, int node, int link),
    void *willParticipate_parms,
    int maxParticipant,
    uint64_t *vFlatNodeExceptions,
    void *vFlatNodeResult,

    int elementWords,
    int32_t opCode,
    char *Tree,
    void *vFlatContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *Result
  );


  // Return value is Exceptions in this reduction.
  uint64_t BgNDReduce_multiNodeVectorReduce(
    int thisNode,
    int (*willParticipate)(void *parms, int node, int link),
    void *willParticipate_parms,
    int maxParticipant,
    uint64_t **vNodeExceptions,
    void **vNodeResult,

    int *participationGeneration,
    int *nodeParticipation,
    int *totalParticipation,

    int numElements,
    int elementWords,
    int32_t opCode,
    char *Tree,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *vResult,
    uint64_t *vExceptions
  );


  // Return value is Exceptions in this reduction.
  uint64_t BgNDReduce_allNodeVectorReduce(
    int rootNode,
    int (*willParticipate)(void *parms, int node, int link),
    void *willParticipate_parms,
    int maxParticipant,
    uint64_t **vNodeExceptions,
    void **vNodeResult,

    int numElements,
    int elementWords,
    int32_t opCode,
    char *Tree,
    void **vContribution,
    uint64_t *vFlagsContribution,
    uint64_t vFlagsResult,
    void *vResult,
    uint64_t *vExceptions
  );

// END   code new to BgNDReduce_C.h, not prorted from C++ BgNDReduce.h --------------------------

// BEGIN verify*

void verifyLogicalOps2wordElement(MTRandomArg *R);
void verifyLogicalOpsVariousWordLengths(MTRandomArg *R);
void verifyMinOrMax(MTRandomArg *R);
void verify_u_add(MTRandomArg *R);
void verify_add(MTRandomArg *R);
void verify_fp_min_or_fp_max(void);
void verify_native_fp_minScalarReduce(void);
void verify_fp_add_loops(void);

int verify_allNode(int what, MTRandomArg *R, int numParticipants, int rootNode, int (*willParticipate)(void *parms ,int node, int link), void *willParticipate_parms, int max_words, int max_elements);

int subComm_verify_allNode(int numNodesInSubComm, int *IsNodeInSubComm, int debugWhat, MTRandomArg *R, int numParticipants, int rootNode, int (*willParticipate)(void *parms, int node, int link), void *willParticipate_parms, int max_words, int max_elements);

void linearClass_verify_allNode(int what, MTRandomArg *R);


// END   verify*

__END_DECLS
#endif


