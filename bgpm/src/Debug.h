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

#ifndef _BGPM_DEBUG_H_  // Prevent multiple inclusion
#define _BGPM_DEBUG_H_

#include <stdio.h>
#include "spi/include/kernel/upc_hw.h"
#include "spi/include/upci/upci.h"


namespace bgpm {

// The following compile time enabled macros are use to define the function trace level
// If commented out, the files can be set at make time.

//#define _UPC_TRACE_PUNIT_RESERVATIONS_  // add to SPI compile (not bgpm compile) to trace punit counter reservations

//#define BGPM_TRACE_ALL


// Default Trace Settings (trace all at level 2)
#ifdef BGPM_TRACE
#undef BGPM_TRACE
#define BGPM_TRACE 1
#define BGPM_TRACE_LEVEL 2
#define BGPM_TRACE_OUT   2
#define BGPM_TRACE_DATA  2
#define BGPM_TRACE_ERRLOC 1

#else
// Trace all - include level 3 and normally exempt stuff (detailed stuff I've used temporarily during debug)
#ifdef BGPM_TRACE_ALL
#undef BGPM_TRACE_ALL
#define BGPM_TRACE_ALL 1
#define BGPM_TRACE_LEVEL 3
#define BGPM_TRACE_OUT   3
#define BGPM_TRACE_DATA  3
#define BGPM_TRACE_LEVEL_EXEMPTED 3
#define BGPM_TRACE_DATA_EXEMPTED 3
#define BGPM_TRACE_ERRLOC 1
#else
// Disable trace
#define BGPM_TRACE_LEVEL 0          // 0 - no trace, 1 - trace only BGPM toplevel functions, 2-trace evt set functions, 3-trace upci functions
#define BGPM_TRACE_LEVEL_EXEMPTED 0 // include trace of normally exempt functions.
#define BGPM_TRACE_OUT 0            // include return from functions.
#define BGPM_TRACE_DATA 0           // Level of data to trace, 0-none, 1-common state data, 2-progress data,
#define BGPM_TRACE_DATA_EXEMPTED 0  // include trace of normally exempt functions.
#define BGPM_TRACE_ERRLOC 0         // Indicate whether to include internal line numbers in messages or not.
#endif
#endif



// macros to implement the compile time inclusion
#if (BGPM_TRACE_LEVEL > 0)
#define BGPM_TRACE_PRINT_ENABLE() Bgpm_EnableTracePrint()
#define BGPM_TRACE_PRINT_DISABLE() Bgpm_DisableTracePrint()
#else
#define BGPM_TRACE_PRINT_ENABLE()
#define BGPM_TRACE_PRINT_DISABLE()
#endif

#if (BGPM_TRACE_LEVEL > 0)
#define BGPM_TRACE_L1 TraceFunc _tf(__func__,_AT_)
#else
#define BGPM_TRACE_L1
#endif

#if (BGPM_TRACE_LEVEL > 1)
#define BGPM_TRACE_L2 TraceFunc _tf(__func__,_AT_)
#else
#define BGPM_TRACE_L2
#endif

#if (BGPM_TRACE_LEVEL > 2)
#define BGPM_TRACE_L3 TraceFunc _tf(__func__,_AT_)
#else
#define BGPM_TRACE_L3
#endif



// Trace_Pt = indicate what file,line we've reached.
#if (BGPM_TRACE_LEVEL > 0)
#define BGPM_TRACE_PT1 if (TraceFunc::prtEnabled) { fprintf(stderr, "%s%s:"_AT_ "\n", TraceFunc::indentStrg, __func__); }
#else
#define BGPM_TRACE_PT1
#endif

#if (BGPM_TRACE_LEVEL > 1)
#define BGPM_TRACE_PT2 if (TraceFunc::prtEnabled) { fprintf(stderr, "%s%s:"_AT_ "\n", TraceFunc::indentStrg, __func__); }
#else
#define BGPM_TRACE_PT2
#endif

#if (BGPM_TRACE_LEVEL > 2)
#define BGPM_TRACE_PT3 if (TraceFunc::prtEnabled) { fprintf(stderr, "%s%s:"_AT_ "\n", TraceFunc::indentStrg, __func__); }
#else
#define BGPM_TRACE_PT3
#endif


// Exempted function calls.
#if (BGPM_TRACE_LEVEL_EXEMPTED > 0)
#define BGPM_EXEMPT_L1 TraceFunc _tf(__func__,_AT_)
#else
#define BGPM_EXEMPT_L1
#endif

#if (BGPM_TRACE_LEVEL_EXEMPTED > 1)
#define BGPM_EXEMPT_L2 TraceFunc _tf(__func__,_AT_)
#else
#define BGPM_EXEMPT_L2
#endif

#if (BGPM_TRACE_LEVEL_EXEMPTED > 2)
#define BGPM_EXEMPT_L3 TraceFunc _tf(__func__,_AT_)
#else
#define BGPM_EXEMPT_L3
#endif



// Trace data: must pass in command.
// Command must explicitly prepend TraceFunc::indentStrg to get BGPM_TRACE tag and have line
// indented according to current indent level.
#if (BGPM_TRACE_DATA > 0)
#define BGPM_TRACE_DATA_L1( _cmd_ ) if (TraceFunc::prtEnabled) { _cmd_; }
#else
#define BGPM_TRACE_DATA_L1( _cmd_ )
#endif

#if (BGPM_TRACE_DATA > 1)
#define BGPM_TRACE_DATA_L2( _cmd_ ) if (TraceFunc::prtEnabled) { _cmd_; }
#else
#define BGPM_TRACE_DATA_L2( _cmd_ )
#endif

#if (BGPM_TRACE_DATA_EXEMPTED > 0)
#define BGPM_EXEMPT_DATA( _cmd_ ) if (TraceFunc::prtEnabled) { _cmd_; }
#else
#define BGPM_EXEMPT_DATA( _cmd_ )
#endif


#if (BGPM_TRACE_ERRLOC > 0)
#define BGPM_ERRLOC _AT_
#else
#define BGPM_ERRLOC NULL
#endif



//! TraceFunc Class
/*!
  TraceFunc is a func-toid framework class to trace function entries.
  Coupled with the _TRACE_LEVEL compile time macros allow us to insert code
  to create function traces.
*/
class TraceFunc
{
public:
    TraceFunc(const char *func, const char *at);
    ~TraceFunc();
    static __thread int indentLvl;
    static __thread char indentStrg[256];
    static __thread int locInit;
    static __thread bool prtEnabled;
private:
    char func_[256];
    char at_[256];

    // hide these
    TraceFunc(const TraceFunc &);
    TraceFunc & operator=(const TraceFunc &);
};

// local short version for indent string.
#define IND_STRG TraceFunc::indentStrg
#define IND_LVL  TraceFunc::indentLvl




//! INCL_NDEBUG_CODE
//! Include the following code when NDEBUG is not set, to set up assert type checks
#ifdef NDEBUG
#define INCL_NDEBUG_CODE(_code_)
#else
#define INCL_NDEBUG_CODE(_code_) _code_
#endif




//! DebugGetPunit
/*!
  Get punit from event set for debug purposes.
  If not a punit event set - returns null.
  Will always ExitOnError.
*/
#ifndef NDEBUG
extern "C" Upci_Punit_t * DebugGetPunit(unsigned hEvtSet);
#endif


}

#endif
