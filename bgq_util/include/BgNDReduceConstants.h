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

#ifndef __BG_ND_REDUCE_CONSTANTS_H__
#define __BG_ND_REDUCE_CONSTANTS_H__

#include <stdint.h>

// BEGIN - opcode as in packet -----------------
static const int32_t    andColOpcode =  0;
static const int32_t     orColOpcode =  1;
static const int32_t    xorColOpcode =  2;

static const int32_t  u_addColOpcode =  4;
static const int32_t  u_minColOpcode =  5;
static const int32_t  u_maxColOpcode =  6;

static const int32_t    addColOpcode =  8;
static const int32_t    minColOpcode =  9;
static const int32_t    maxColOpcode = 10;

static const int32_t fp_addColOpcode = 12;
static const int32_t fp_minColOpcode = 13;
static const int32_t fp_maxColOpcode = 14;
// END   - opcode as in packet -----------------


static const int iBgNDReduce = 1;
#define isBGQendian() ( (*(char*)&iBgNDReduce) == 0 )

// BEGIN - flags for BgNDReduce -----------------

// For u_,addColOpcode, the data is in network order. In other words, in least significant word first.
static const uint64_t NdOrderBGNdReduce = 1 << 0;

// END   - flags for BgNDReduce -----------------



// BEGIN - exception codes from BgNDReduce -----------------

static const uint64_t invalidOpCodeBGNdReduce          = 0x0000001ull;
static const uint64_t illegalx86endianBGNdReduce       = 0x0000002ull;
static const uint64_t internalErrorBGNdReduce          = 0x0000004ull;
static const uint64_t u_addOverflowBGNdReduce          = 0x0000008ull;
static const uint64_t addOverflowBGNdReduce            = 0x0000010ull;
static const uint64_t addUnderflowBGNdReduce           = 0x0000020ull;
static const uint64_t fpWordsNot2BGNdReduce            = 0x0000040ull;
static const uint64_t NaNinputBGNdReduce               = 0x0000080ull;
static const uint64_t NaNcreatedBGNdReduce             = 0x0000100ull;
static const uint64_t fp_addParticipantsGT15BGNdReduce = 0x0000200ull;
static const uint64_t verificationFailBGNdReduce       = 0x0000400ull;

// BEGIN for uint64_t BgNDReduce::treeAddScalarReduce() --------------
static const uint64_t strtolTroubleBGNdReduce          = 0x0004000ull;
static const uint64_t noParticipantBGNdReduce          = 0x0008000ull;
static const uint64_t doubleParticipantBGNdReduce      = 0x0010000ull;
static const uint64_t unexpectedParticipantBGNdReduce  = 0x0020000ull;
static const uint64_t lt2OnStackBGNdReduce             = 0x0040000ull;
static const uint64_t nothingOnStackBGNdReduce         = 0x0080000ull;
static const uint64_t tooMuchOnStackBGNdReduce         = 0x0100000ull;
static const uint64_t unusedParticipantBGNdReduce      = 0x0200000ull;
static const uint64_t illegalParticipantBGNdReduce     = 0x0400000ull;
static const uint64_t whichCarryTroubleBGNdReduce      = 0x0800000ull;
static const uint64_t illegalCharBGNdReduce            = 0x1000000ull;
// END   for uint64_t BgNDReduce::treeAddScalarReduce() --------------

// END   - exception codes from BgNDReduce -----------------

#endif


