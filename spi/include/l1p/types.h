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
/* (C) Copyright IBM Corp.  2009, 2012                              */
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
#ifndef _L1P_TYPES_H_ // Prevent multiple inclusion.
#define _L1P_TYPES_H_

#include <hwi/include/common/compiler_support.h>

__BEGIN_DECLS

#ifndef DD1_L1P_Workaround
#define DD1_L1P_Workaround 0
#endif

typedef enum 
{
    L1P_stream_optimistic = 1,
    L1P_stream_confirmed,
    L1P_confirmed_or_dcbt,
    L1P_stream_disable
} L1P_StreamPolicy_t;

typedef enum 
{
    L1P_NestingSaveContext = 1,
    L1P_NestingIgnore,
    L1P_NestingFlat,
    L1P_NestingError
} L1P_PatternNest_t;

typedef enum 
{
    L1P_PatternLimit_Disable = 1,
    L1P_PatternLimit_Error,
    L1P_PatternLimit_Assert,
    L1P_PatternLimit_Prune
} L1P_PatternLimitPolicy_t;

typedef enum
{
    L1P_NOMEMORY=1,
    L1P_PARMRANGE,
    L1P_PATTERNACTIVE,
    L1P_NOTAPATTERN,
    L1P_ALREADYCONFIGURED,
    L1P_NOTCONFIGURED
} L1P_ErrorCode;
    
#define L1P_CACHELINESIZE     128
#define L1P_MISSLISTENTRYSIZE 4

typedef struct
{
    size_t size;
    void* ReadPattern;
    void* WritePattern;
} L1P_Pattern_t;

typedef union
{
    uint64_t status;
    struct 
    {
        uint64_t pad       : 60;
        uint64_t finished  : 1;
        uint64_t abandoned : 1;
        uint64_t maximum   : 1;
	uint64_t endoflist : 1;
    } s;
} L1P_Status_t;
    
typedef struct
{
    uint64_t pad                : 52;
    uint64_t ctrl_prefetchdepth : 3;
    uint64_t ctrl_maxlistaddr   : 3;
    uint64_t ctrl_inhibitlist   : 1;
    uint64_t ctrl_ignoremaxtol  : 1;
    uint64_t ctrl_start         : 1;
    uint64_t ctrl_load          : 1;
    uint64_t ctrl_pause         : 1;
} L1P_MMIO_t;

typedef struct
{
    volatile uint32_t  criticalatom;  // dd1 workaround lock
    unsigned int       valid;
    unsigned int       ignorePatternActions;
    unsigned int       nestDepth;
    L1P_PatternNest_t  nestPolicy;
    
    unsigned int       implicitPatternAllocate;
    L1P_Pattern_t      currentPattern;
    void*              nestedContext;
} L1P_SPIContext;

__END_DECLS

#endif // Add nothing below this line.
