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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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

#ifndef _BGPM_GLOBALS_H_  // Prevent multiple inclusion
#define _BGPM_GLOBALS_H_

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include "cnk/include/Config.h"
#include "spi/include/upci/upci.h"
#include "bgpm/include/types.h"
#include "bgpm/include/err.h"
#include "impl_types.h"
#include "CoreReservations.h"
#include "ShmemBlk.h"
#include "EnvVars.h"
#include "PodSharePtr.h"
#include "ErrBkt.h"
#include "Debug.h"
#include "EvtSetList.h"
#include "ES_SWPunit.h"
#include "Lock.h"


#ifndef __HWSTUBS__
#include <unistd.h>
#include <sys/syscall.h>
#define gettid() syscall(207, 0) // not defined by glibc
#endif

//! \addtogroup bgpm_api_impl
//@{
/**
 * \file
 *
 * \brief BGPM key global variables.
 *
 */
//@}

using namespace bgpm;

#ifndef LIKELY
#define LIKELY(x)       __builtin_expect(!!(x), 1)
#define UNLIKELY(x)     __builtin_expect(!!(x), 0)
#endif


typedef unsigned short ushort;

// ---------------------------------
// Process Wide Global Instances
// These all are initialized by Bgpm_Init()
typedef PodSharedPtr<ShmemBlk> ShmemBlkPtr;



#ifdef _BGPM_INIT_C_
bool        procInited = false;  //!< track whether global process vars are inited by bgpm_init
ShmemBlkPtr pShMem;       //!< smart ptr to shared memory block for processs - atomically init with Bgpm_Init.
EvtSetList  procEvtSets; //!< shared pointer to process level event sets list.
EnvVars     envVars;      //!< Bgpm understood environment variables
uint64_t    nodeConfig;   //!< personality nodeconfig to indicate what hardware is active and avoid duplicate syscalls
UPC_Lock_t  globalDebugLock = 0;

#else
extern bool        procInited;
extern ShmemBlkPtr pShMem;
extern EvtSetList  procEvtSets;
extern EnvVars     envVars;
extern uint64_t    nodeConfig;
extern UPC_Lock_t  globalDebugLock;
#endif


// ---------------------------------
// Thread Specific Global Instances
#ifdef _BGPM_INIT_C_
__thread bool thdInited = false;  // track whether thread global vars are initialized by bgpm_init.
// Can't have globals to thread specific objects - only ordinary data (scalars or structures).
// Thus, the thread specific global data are pointers to smart pointers.
// We'll use the smart pointers to keep track of owner ship, but will be quicker to use
// a raw pointer for access rather than coping smart pointers around - so be careful.
__thread ShmemBlkPtr  pThdShMem;  //!< Ptr to smart ptr to shared memory block for thread - just to keep ref count dependency by thread
                                            //!< use pShMem for faster access
__thread unsigned    curHwThdId;  //!< set at bgpm init time to cache current thread id, and double check at apply or attach time.
__thread ErrBkt      lastErr;            //!< object to maintain last bgpm thread error.
__thread short       printOnError = -1; //!< global only usable thru functions in ErrBkt.h, -1=uninit, 0=false, 1=true.
__thread short       exitOnError = -1;
__thread short       dumpStackOnError = -1;
__thread short       addEventErrFeedback = BGPM_EVT_FEEDBACK_1ST; //!< print some feedback on when event add fails.
__thread int         swModeAppliedPuEvtSet = -1;  //!< Active SW Mode Punit Event Set (for overflow operations)
__thread int         swModeAppliedL2EvtSet = -1;  //!< Active SW Mode L2 Event Set (for overflow operations)
__thread int         swModeAppliedIOEvtSet = -1;  //!< Active SW Mode I/O Event Set (for overflow operations)
__thread BgpmDebugThreadVars_t dbgThdVars;

#else
extern __thread bool thdInited;
extern __thread ShmemBlkPtr pThdShMem;
extern __thread unsigned    curHwThdId;
extern __thread ErrBkt      lastErr;
extern __thread short       printOnError;
extern __thread short       exitOnError;
extern __thread short       dumpStackOnError;
extern __thread short       addEventErrFeedback;
extern __thread int         swModeAppliedPuEvtSet;
extern __thread int         swModeAppliedL2EvtSet;
extern __thread int         swModeAppliedIOEvtSet;
extern __thread BgpmDebugThreadVars_t dbgThdVars;


#endif


#include "EvtSetList_Inlines.h"


//! Minimum Overflow Threadshold value.
#define BGPM_THRESHOLD_MIN 0x8000000000000000ULL


namespace bgpm
{



inline bool GetPrintOnError() {
    if (UNLIKELY(printOnError < 0)) printOnError = envVars.PrintOnError();
    return printOnError;
}
inline bool GetExitOnError() {
    if (UNLIKELY(exitOnError < 0)) exitOnError = envVars.ExitOnError();
    return exitOnError;
}
inline bool GetDumpOnError() {
    if (UNLIKELY(dumpStackOnError < 0)) dumpStackOnError = envVars.DumpOnError();
    return dumpStackOnError;
}


/**
 * BGPM_MALLOC
 * Malloc with fatal error on out of memory condition
 */
#define BGPM_MALLOC(_type_) \
    (_type_*)( pTmpMalloc = malloc(sizeof(_type_))) ? pTmpMalloc : Bgpm_Die(BGPM_ENOMEM, _AT_)


}


// RAS Messages which can be posted by BGPM
#define BGPM_RAS_BIGOVF_ERR 0x000E0000
/*
  <rasevent
    id="000E0000"
    category="UPC"
    component="BGPM"
    severity="WARN"
    message="Bgpm Performance Monitor interrupts have been disabled due to low level UPC Counter maximum overflow. This is likely a soft hardware error. Details: $(DETAILS)"
    description="A upc_p 14 bit counter counter wrapped instead of capture by upc_c hardware. This likely indicates a soft hardware error, but effects the integrity of gathered performance counts."
    control_action=""
    service_action="$(Diagnostics)"
    relevant_diags="processor"
    decoder=""
    />
 */






#endif
