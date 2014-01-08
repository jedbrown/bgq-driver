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
#include "bgpm/include/err.h"
#include "ErrBkt.h"
#include "EvtSetList.h"
#include "EnvVars.h"

using namespace bgpm;

#ifndef LIKELY
#define LIKELY(x)       __builtin_expect(!!(x), 1)
#define UNLIKELY(x)     __builtin_expect(!!(x), 0)
#endif


typedef unsigned short ushort;

#ifdef _TEST_INIT_C_
EvtSetList  procEvtSets; //!< shared pointer to process level event sets list.
EnvVars     envVars;      //!< Bgpm understood environment variables
#else
extern EvtSetList  procEvtSets;
extern EnvVars     envVars;      //!< Bgpm understood environment variables
#endif


// ---------------------------------
// Thread Specific Global Instances
#ifdef _TEST_INIT_C_
__thread bool thdInited = false;        // track whether thread global vars are initialized by bgpm_init.
__thread unsigned    curHwThdId;        //!< set at bgpm init time to cache current thread id, and double check at apply or attach time.
__thread ErrBkt      lastErr;           //!< object to maintain last bgpm thread error.
__thread short       printOnError = -1; //!< global only usable thru functions in ErrBkt.h, -1=uninit, 0=false, 1=true.
__thread short       exitOnError = -1;
__thread short       dumpStackOnError = -1;
__thread short       addEventErrFeedback = BGPM_EVT_FEEDBACK_1ST; //!< print some feedback on when event add fails.
#else
extern __thread bool        thdInited;
extern __thread unsigned    curHwThdId;
extern __thread ErrBkt      lastErr;
extern __thread short       printOnError;
extern __thread short       exitOnError;
extern __thread short       dumpStackOnError;
extern __thread short       addEventErrFeedback;
#endif


#include "EvtSetList_Inlines.h"




#endif
