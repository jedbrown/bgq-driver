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
#ifndef _PRINTF_REPL_H_
#define _PRINTF_REPL_H_

//! \addtogroup upci_spi
//@{
//! \file
//
// \brief UPC Printf Test Replacement
//
// This is a printf replacement to use within cyclesim tests.
// The purpose is allow "speedy" formatted output of data for debug purposes.
// The macros will write to a buffer space which can be recognized and store operations log via the L1P monitor.
// The "printf_repl_trace.pl" script can recognize and format the data from the script after the fact.
// When moving to the FPGA environment later, you need to replace these calls with real printfs.
//
// It also provides some logging functions to use within interrupt handlers
// so the results may be printed at the end for debug.
//
//
//
// How to use:
//   Modify code to use the printf_repl:
//
//   1) At top of main module to cause necessary globals to be declared once:
//        #define PRINTF_REPL_C
//        #include "printf_repl.h"
//
//   2) In other modules only include "printf_repl.h", don't use the #define value above.
//
//   3) At the top of your "test_main" function, add:
//         PRINTF_START;   // This will write a recognition pattern allowing script get buffer address.
//
//   4) Then replace printf calls with PRINTF_X... macros at the bottom of this file.
//      The XXX.s in the name indicate the type and number of arguments to print
//      (this prevents need for the simulation code to parse the format string).
//      So, for example, your test_main might look like this:
//
//         PRINTF_START;
//
//         //printf(
//         PRINTF_XXX(
//             "(I) main[%2d:%d]: Hello from thread index %d!\n",
//             PhysicalProcessorID(),      // 0..16
//             PhysicalThreadID(),         // 0..3
//             PhysicalThreadIndex() );    // 0..67
//
//   Note:
//      at the moment I've only defined macros for a number of "long decimal" types.
//      More are easily built from the submacros which support strings, and decimal values.
//      I started one for double values, but haven't tried it.
//
//   5) Enable the L1p monitor in your test.parms file:
//       #prefix A2L1pMonitor[00-17] {
//          EnableDriver=UpciTrue;
//          TraceReq=false;
//          TraceLoad=false;
//          TraceStore=UpciTrue;
//          TraceLwarxAndStwcx=false;
//          TraceInvalidate=false;
//
//   6) After the fact, run "spi/tests/upci/scripts/printf_repl_trace.pl trace.out" on the trace.out file to see the output.
//      It should look something like this:
//
//          File: FCTest.llapi.090326-180522/trace.log
//
//          core_00_0: (I) main[ 0:0]: Hello from thread index 0!
//
//
//@}



#include <stdio.h>
#include "spi/include/kernel/upci.h"
#include <hwi/include/bqc/PhysicalMap.h>
#include <hwi/include/bqc/A2_inlines.h>
//#include <hwi/include/bqc/MU_Addressing.h>

#include "spi/include/upci/upc_atomic.h"

__BEGIN_DECLS


#ifdef __FOR_CYCLESIM__
// ifdef to decide whether to use printf replacements for cyclesim, or use normal printf



// Buffer and address locations.
#ifdef PRINTF_REPL_C
uint64_t  buf[4];
volatile uint64_t  *prepl_args = &buf[0];
volatile uint64_t  *prepl_fmt  = &buf[1];
volatile uint64_t  *prepl_data = &buf[2];
volatile uint64_t  *prepl_dbg  = &buf[3];
#else
extern volatile uint64_t  *prepl_args;
extern volatile uint64_t  *prepl_fmt;
extern volatile uint64_t  *prepl_data;
extern volatile uint64_t  *prepl_dbg;
#endif

typedef union  {
    uint64_t uint;
    double   dbl;
} _int_dbl_t;

#ifdef PRINTF_REPL_C
// String writer.
__INLINE__ uint64_t PR_Strlen(const char *strg) {
    uint64_t len = 0;
    while (*strg++ != '\0') { len++; }
    return len;
}
void _PRTSTRG ( const char *strg )
{
    uint64_t len = PR_Strlen(strg);
    *prepl_fmt = 0x0000000100000000 | len;

    while (len >= 8) {
        //*prepl_dbg = len;
        *prepl_data = *((uint64_t*)strg);
        strg += 8;
        len -= 8;
    }
    if (len > 0) {
        uint64_t temp = *((uint64_t*)strg);
        uint64_t mask = 0xFFFFFFFFFFFFFFFFUL >> (len<<3); // shift len * 8
        temp = temp & (~mask);
        *prepl_data = temp;
    }
}
void _PRTFMT ( const char *strg, uint64_t len )
{
    //uint64_t len = PR_Strlen(strg);
    len -= 1;
    *prepl_fmt = 0x0000000100000000 | len;

    while (len >= 8) {
        //*prepl_dbg = len;
        *prepl_data = *((uint64_t*)strg);
        strg += 8;
        len -= 8;
    }
    if (len > 0) {
        uint64_t temp = *((uint64_t*)strg);
        uint64_t mask = 0xFFFFFFFFFFFFFFFFUL >> (len<<3); // shift len * 8
        temp = temp & (~mask);
        *prepl_data = temp;
    }
}
#else
void _PRTSTRG ( const char *strg );
void _PRTFMT ( const char *strg, uint64_t len );
#endif


// #######################################################
// Sub Macros to use to build PRINTF macros
//  String macro is not inline - and can be found above.
__INLINE__ void _PRTARGS( int num_args ) {*prepl_args = num_args; }
__INLINE__ void _PRT64( uint64_t val )   {*prepl_fmt = 0x0000000200000001; *prepl_data = val; }
__INLINE__ void _PRTDBL( double val )    {*prepl_fmt = 0x0000000300000001; *prepl_data = ((_int_dbl_t)val).uint; }




// #######################################################
// printf replacement macros to use in your code
// #######################################################

// Write recognition pattern
// Pattern = "Printf Replace\0\0"
#define PRINTF_START \
    *prepl_args=0x5072696e74662052; \
    *prepl_args=0x65706c6163650000

// Printf w format and 1 decimal args
#define PRINTF(fmt) _PRTARGS(1); _PRTFMT(fmt,sizeof(fmt));

// Printf w format and 1 decimal args
#define PRINTF_X(fmt, lint) _PRTARGS(2); _PRTFMT(fmt,sizeof(fmt)); _PRT64((uint64_t)lint)

// Printf w format and 2 decimal args
#define PRINTF_XX(fmt, lint1, lint2) _PRTARGS(3); _PRTFMT(fmt,sizeof(fmt)); _PRT64((uint64_t)lint1); _PRT64((uint64_t)lint2)

// Printf w format and 3 decimal args
#define PRINTF_XXX(fmt, lint1, lint2, lint3) _PRTARGS(4); _PRTFMT(fmt,sizeof(fmt)); _PRT64((uint64_t)lint1); _PRT64((uint64_t)lint2); _PRT64((uint64_t)lint3)

// Printf w format and 4 decimal args
#define PRINTF_XXXX(fmt, lint1, lint2, lint3, lint4) _PRTARGS(5); _PRTFMT(fmt,sizeof(fmt)); _PRT64((uint64_t)lint1); _PRT64((uint64_t)lint2); _PRT64((uint64_t)lint3); _PRT64((uint64_t)lint4)

// Printf w format and 5 decimal args
#define PRINTF_XXXXX(fmt, d1,d2,d3,d4,d5) _PRTARGS(6); _PRTFMT(fmt,sizeof(fmt)); _PRT64((uint64_t)d1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3); _PRT64((uint64_t)d4); _PRT64((uint64_t)d5)

// Printf w format and 6 decimal args
#define PRINTF_XXXXXX(fmt, d1,d2,d3,d4,d5,d6) _PRTARGS(7); _PRTFMT(fmt,sizeof(fmt)); _PRT64((uint64_t)d1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3); _PRT64((uint64_t)d4); _PRT64((uint64_t)d5), _PRT64((uint64_t)d6)

// Printf w format and 7 decimal args
#define PRINTF_XXXXXXX(fmt, d1,d2,d3,d4,d5,d6,d7) _PRTARGS(8); _PRTFMT(fmt,sizeof(fmt)); _PRT64((uint64_t)d1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3); _PRT64((uint64_t)d4); _PRT64((uint64_t)d5), _PRT64((uint64_t)d6), _PRT64((uint64_t)d7)

// Printf w format and 8 decimal args
#define PRINTF_XXXXXXXX(fmt, d1,d2,d3,d4,d5,d6,d7,d8) _PRTARGS(9); _PRTFMT(fmt,sizeof(fmt)); _PRT64((uint64_t)d1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3); _PRT64((uint64_t)d4); _PRT64((uint64_t)d5), _PRT64((uint64_t)d6), _PRT64((uint64_t)d7), _PRT64((uint64_t)d8)

// Printf w format and 9 decimal args
#define PRINTF_XXXXXXXXX(fmt, d1,d2,d3,d4,d5,d6,d7,d8,d9) _PRTARGS(10); _PRTFMT(fmt,sizeof(fmt)); _PRT64((uint64_t)d1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3); _PRT64((uint64_t)d4); _PRT64((uint64_t)d5), _PRT64((uint64_t)d6), _PRT64((uint64_t)d7), _PRT64((uint64_t)d8), _PRT64((uint64_t)d9)
// Printf w format and 10 decimal args
#define PRINTF_XXXXXXXXXX(fmt, d1,d2,d3,d4,d5,d6,d7,d8,d9,d10) _PRTARGS(11); _PRTFMT(fmt,sizeof(fmt)); _PRT64((uint64_t)d1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3); _PRT64((uint64_t)d4); _PRT64((uint64_t)d5), _PRT64((uint64_t)d6), _PRT64((uint64_t)d7), _PRT64((uint64_t)d8), _PRT64((uint64_t)d9), _PRT64((uint64_t)d10)
// Printf w format and 1 floating args
#define PRINTF_F(fmt, dbl) _PRTARGS(2); _PRTFMT(fmt,sizeof(fmt)); _PRTDBL(dbl)

// Printf w format and 2 floating args
#define PRINTF_FF(fmt, dbl1, dbl2) _PRTARGS(3); _PRTFMT(fmt,sizeof(fmt)); _PRTDBL(dbl1); _PRTDBL(dbl2)

// Printf w format and 2 floating args
#define PRINTF_FFF(fmt, dbl1, dbl2, dbl3) _PRTARGS(4); _PRTFMT(fmt,sizeof(fmt)); _PRTDBL(dbl1); _PRTDBL(dbl2); _PRTDBL(dbl3)



// Special Ones

#define PRINTF_XXSXXXXXXXXXX(fmt, d1,d2,s3,d4,d5,d6,d7,d8,d9,d10,d11,d12,d13 ) \
    _PRTARGS(14); _PRTFMT(fmt,sizeof(fmt)); _PRTDBL(d1); _PRTDBL(d2); _PRTSTRG(s3); \
    _PRTDBL(d4); _PRTDBL(d5); _PRTDBL(d6); _PRTDBL(d7); _PRTDBL(d8); _PRTDBL(d9); _PRTDBL(d10); _PRTDBL(d11); _PRTDBL(d12); _PRTDBL(d13)
#define PRINTF_XXS(fmt, d1,d2,s3 ) \
    _PRTARGS(4); _PRTFMT(fmt,sizeof(fmt)); _PRTDBL(d1); _PRTDBL(d2); _PRTSTRG(s3);
#define PRINTF_XXXXS(fmt, d1,d2,d3,d4,s1 ) \
    _PRTARGS(6); _PRTFMT(fmt,sizeof(fmt)); _PRTDBL(d1); _PRTDBL(d2);  _PRTDBL(d3); _PRTDBL(d4); _PRTSTRG(s1);
#define PRINTF_XS(fmt, d1,s1) _PRTARGS(3); _PRTFMT(fmt,sizeof(fmt)); _PRT64((uint64_t)d1); _PRTSTRG(s1);
#define PRINTF_SX(fmt, s1,d1) _PRTARGS(3); _PRTFMT(fmt,sizeof(fmt)); _PRTSTRG(s1); _PRT64((uint64_t)d1);
#define PRINTF_SXX(fmt, s1,d1,d2) _PRTARGS(4); _PRTFMT(fmt,sizeof(fmt)); _PRTSTRG(s1); _PRT64((uint64_t)d1); _PRT64((uint64_t)d2);
#define PRINTF_SXXX(fmt, s1,d1,d2,d3) _PRTARGS(5); _PRTFMT(fmt,sizeof(fmt)); _PRTSTRG(s1); _PRT64((uint64_t)d1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3);
#define PRINTF_SXXXX(fmt, s1,d1,d2,d3,d4) _PRTARGS(6); _PRTFMT(fmt,sizeof(fmt)); _PRTSTRG(s1); _PRT64((uint64_t)d1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3); _PRT64((uint64_t)d4)
#define PRINTF_SXXXXX(fmt, s1,d1,d2,d3,d4,d5) _PRTARGS(7); _PRTFMT(fmt,sizeof(fmt)); _PRTSTRG(s1); _PRT64((uint64_t)d1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3); _PRT64((uint64_t)d4); _PRT64((uint64_t)d5)
#define PRINTF_XSXXXX(fmt, d1,s1,d2,d3,d4,d5) _PRTARGS(7); _PRTFMT(fmt,sizeof(fmt)); _PRT64((uint64_t)d1); _PRTSTRG(s1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3); _PRT64((uint64_t)d4); _PRT64((uint64_t)d5)
#define PRINTF_SXXXXXXX(fmt, s1,d1,d2,d3,d4,d5,d6,d7) _PRTARGS(9); _PRTFMT(fmt,sizeof(fmt)); _PRTSTRG(s1); _PRT64((uint64_t)d1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3); _PRT64((uint64_t)d4); _PRT64((uint64_t)d5); _PRT64((uint64_t)d6); _PRT64((uint64_t)d7)
#define PRINTF_XSXXXXXX(fmt, d1,s1,d2,d3,d4,d5,d6,d7) _PRTARGS(9); _PRTFMT(fmt,sizeof(fmt)); _PRT64((uint64_t)d1); _PRTSTRG(s1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3); _PRT64((uint64_t)d4); _PRT64((uint64_t)d5); _PRT64((uint64_t)d6); _PRT64((uint64_t)d7)
#define PRINTF_SXXXXXXXX(fmt, s1,d1,d2,d3,d4,d5,d6,d7,d8) _PRTARGS(10); _PRTFMT(fmt,sizeof(fmt)); _PRTSTRG(s1); _PRT64((uint64_t)d1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3); _PRT64((uint64_t)d4); _PRT64((uint64_t)d5); _PRT64((uint64_t)d6); _PRT64((uint64_t)d7); _PRT64((uint64_t)d8)
#define PRINTF_XSXXXXXXX(fmt, d1,s1,d2,d3,d4,d5,d6,d7,d8) _PRTARGS(10); _PRTFMT(fmt,sizeof(fmt)); _PRT64((uint64_t)d1); _PRTSTRG(s1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3); _PRT64((uint64_t)d4); _PRT64((uint64_t)d5); _PRT64((uint64_t)d6); _PRT64((uint64_t)d7); _PRT64((uint64_t)d8)
#define PRINTF_SXXXXXXXXX(fmt, s1,d1,d2,d3,d4,d5,d6,d7,d8,d9) _PRTARGS(11); _PRTFMT(fmt,sizeof(fmt)); _PRTSTRG(s1); _PRT64((uint64_t)d1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3); _PRT64((uint64_t)d4); _PRT64((uint64_t)d5); _PRT64((uint64_t)d6); _PRT64((uint64_t)d7); _PRT64((uint64_t)d8); _PRT64((uint64_t)d9)
#define PRINTF_XSXXXXXXXX(fmt, d1,s1,d2,d3,d4,d5,d6,d7,d8,d9) _PRTARGS(11); _PRTFMT(fmt,sizeof(fmt)); _PRT64((uint64_t)d1); _PRTSTRG(s1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3); _PRT64((uint64_t)d4); _PRT64((uint64_t)d5); _PRT64((uint64_t)d6); _PRT64((uint64_t)d7); _PRT64((uint64_t)d8); _PRT64((uint64_t)d9)
#define PRINTF_SXXXXXXXXXX(fmt, s1,d1,d2,d3,d4,d5,d6,d7,d8,d9,d10) _PRTARGS(12); _PRTFMT(fmt,sizeof(fmt)); _PRTSTRG(s1); _PRT64((uint64_t)d1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3); _PRT64((uint64_t)d4); _PRT64((uint64_t)d5); _PRT64((uint64_t)d6); _PRT64((uint64_t)d7); _PRT64((uint64_t)d8); _PRT64((uint64_t)d9); _PRT64((uint64_t)d10)
#define PRINTF_XSXXXXXXXXX(fmt, d1,s1,d2,d3,d4,d5,d6,d7,d8,d9,d10) _PRTARGS(12); _PRTFMT(fmt,sizeof(fmt)); _PRT64((uint64_t)d1); _PRTSTRG(s1); _PRT64((uint64_t)d2); _PRT64((uint64_t)d3); _PRT64((uint64_t)d4); _PRT64((uint64_t)d5); _PRT64((uint64_t)d6); _PRT64((uint64_t)d7); _PRT64((uint64_t)d8); _PRT64((uint64_t)d9); _PRT64((uint64_t)d10)


#else
// Use normal printf instead

#define PRINTF_START

#define PRINTF(...)     printf(__VA_ARGS__ )
#define PRINTF_X(...)   printf(__VA_ARGS__ )
#define PRINTF_XX(...)  printf(__VA_ARGS__ )
#define PRINTF_XXX(...)  printf(__VA_ARGS__ )
#define PRINTF_XXXX(...)  printf(__VA_ARGS__ )
#define PRINTF_XXXXX(...)  printf(__VA_ARGS__ )
#define PRINTF_XXXXXX(...)  printf(__VA_ARGS__ )
#define PRINTF_XXXXXXX(...)  printf(__VA_ARGS__ )
#define PRINTF_XXXXXXXX(...)  printf(__VA_ARGS__ )
#define PRINTF_XXXXXXXXX(...)  printf(__VA_ARGS__ )
#define PRINTF_XXXXXXXXXX(...)  printf(__VA_ARGS__ )

#define PRINTF_F(...)   printf(__VA_ARGS__ )
#define PRINTF_FF(...)   printf(__VA_ARGS__ )
#define PRINTF_FFF(...)   printf(__VA_ARGS__ )

#define PRINTF_XXSXXXXXXXXXX(...) printf(__VA_ARGS__ )
#define PRINTF_XXS(...) printf(__VA_ARGS__ )
#define PRINTF_XXXXS(...) printf(__VA_ARGS__ )
#define PRINTF_XS(...) printf(__VA_ARGS__ )
#define PRINTF_SX(...) printf(__VA_ARGS__ )
#define PRINTF_SXX(...) printf(__VA_ARGS__ )
#define PRINTF_SXXX(...) printf(__VA_ARGS__ )
#define PRINTF_SXXXX(...) printf(__VA_ARGS__ )
#define PRINTF_SXXXXX(...) printf(__VA_ARGS__ )
#define PRINTF_XSXXXX(...) printf(__VA_ARGS__ )
#define PRINTF_SXXXXXXX(...) printf(__VA_ARGS__ )
#define PRINTF_XSXXXXXX(...) printf(__VA_ARGS__ )
#define PRINTF_SXXXXXXXX(...) printf(__VA_ARGS__ )
#define PRINTF_XSXXXXXXX(...) printf(__VA_ARGS__ )
#define PRINTF_SXXXXXXXXX(...) printf(__VA_ARGS__ )
#define PRINTF_XSXXXXXXXX(...) printf(__VA_ARGS__ )
#define PRINTF_SXXXXXXXXXX(...) printf(__VA_ARGS__ )
#define PRINTF_XSXXXXXXXXX(...) printf(__VA_ARGS__ )


#endif




// this is kludge to log printfs in quick fashion for
// quick "printing" in time sensitive or restrictive routines.
// The "log" created will need to be printed at end via printlog;
typedef union {
    char *   c;
    uint64_t x;
    double   f;
} logvar_t;

typedef struct {
    long type;  // 0 : no args,  1-5 - num uint64_t values,  11-15, num floating pt values
    long tIdx;       // thread index
    char *   f;
    logvar_t a1;
    logvar_t a2;
    logvar_t a3;
    logvar_t a4;
    logvar_t a5;
    logvar_t a6;
    logvar_t a7;
    logvar_t a8;
} log_entry_t;


#define PLOG_LEN 32000
#ifdef PRINTF_REPL_C

UPC_Atomic_Int_t plogIdx = 0;
log_entry_t prtlog[PLOG_LEN];

#else

extern UPC_Atomic_Int_t plogIdx;
extern log_entry_t prtlog[];

#endif


#ifdef __FOR_CYCLESIM__

#define LOGPRINTF(...)            PRINTF(__VA_ARGS__)
#define LOGPRINTF_X(...)          PRINTF_X(__VA_ARGS__)
#define LOGPRINTF_XX(...)         PRINTF_XX(__VA_ARGS__)
#define LOGPRINTF_XXX(...)        PRINTF_XXX(__VA_ARGS__)
#define LOGPRINTF_XXXX(...)       PRINTF_XXXX(__VA_ARGS__)
#define LOGPRINTF_XXXXX(...)      PRINTF_XXXXX(__VA_ARGS__)
#define LOGPRINTF_XXXXXX(...)     PRINTF_XXXXXX(__VA_ARGS__)
#define LOGPRINTF_XXXXXXX(...)    PRINTF_XXXXXXX(__VA_ARGS__)
#define LOGPRINTF_XXXXXXXX(...)   PRINTF_XXXXXXXX(__VA_ARGS__)


#else

#define LOGPRINTF(fmt)           { int i=UPC_Atomic_Incr(&plogIdx); prtlog[i].tIdx=PhysicalThreadIndex(); prtlog[i].type=0; prtlog[i].f=fmt; }
#define LOGPRINTF_X(fmt, d1)     { int i=UPC_Atomic_Incr(&plogIdx); prtlog[i].tIdx=PhysicalThreadIndex(); prtlog[i].type=1; prtlog[i].f=fmt; prtlog[i].a1.x=(uint64_t)d1; }
#define LOGPRINTF_XX(fmt, d1,d2) { int i=UPC_Atomic_Incr(&plogIdx); prtlog[i].tIdx=PhysicalThreadIndex(); prtlog[i].type=2; prtlog[i].f=fmt; prtlog[i].a1.x=(uint64_t)d1; prtlog[i].a2.x=(uint64_t)d2; }
#define LOGPRINTF_XXX(fmt, d1,d2,d3) { int i=UPC_Atomic_Incr(&plogIdx); prtlog[i].tIdx=PhysicalThreadIndex(); prtlog[i].type=3; prtlog[i].f=fmt; prtlog[i].a1.x=(uint64_t)d1; prtlog[i].a2.x=(uint64_t)d2; prtlog[i].a3.x=(uint64_t)d3; }
#define LOGPRINTF_XXXX(fmt, d1,d2,d3,d4) { int i=UPC_Atomic_Incr(&plogIdx); prtlog[i].tIdx=PhysicalThreadIndex(); prtlog[i].type=4; prtlog[i].f=fmt; prtlog[i].a1.x=(uint64_t)d1; prtlog[i].a2.x=(uint64_t)d2; prtlog[i].a3.x=(uint64_t)d3; prtlog[i].a4.x=(uint64_t)d4; }
#define LOGPRINTF_XXXXX(fmt, d1,d2,d3,d4,d5) { int i=UPC_Atomic_Incr(&plogIdx); prtlog[i].tIdx=PhysicalThreadIndex(); prtlog[i].type=5; prtlog[i].f=fmt; prtlog[i].a1.x=(uint64_t)d1; prtlog[i].a2.x=(uint64_t)d2; prtlog[i].a3.x=(uint64_t)d3; prtlog[i].a4.x=(uint64_t)d4; prtlog[i].a5.x=(uint64_t)d5; }
#define LOGPRINTF_XXXXXX(fmt, d1,d2,d3,d4,d5,d6) { int i=UPC_Atomic_Incr(&plogIdx); prtlog[i].tIdx=PhysicalThreadIndex(); prtlog[i].type=6; prtlog[i].f=fmt; prtlog[i].a1.x=(uint64_t)d1; prtlog[i].a2.x=(uint64_t)d2; prtlog[i].a3.x=(uint64_t)d3; prtlog[i].a4.x=(uint64_t)d4; prtlog[i].a5.x=(uint64_t)d5; prtlog[i].a6.x=(uint64_t)d6; }
#define LOGPRINTF_XXXXXXX(fmt, d1,d2,d3,d4,d5,d6,d7) { int i=UPC_Atomic_Incr(&plogIdx); prtlog[i].tIdx=PhysicalThreadIndex(); prtlog[i].type=7; prtlog[i].f=fmt; prtlog[i].a1.x=(uint64_t)d1; prtlog[i].a2.x=(uint64_t)d2; prtlog[i].a3.x=(uint64_t)d3; prtlog[i].a4.x=(uint64_t)d4; prtlog[i].a5.x=(uint64_t)d5; prtlog[i].a6.x=(uint64_t)d6; prtlog[i].a7.x=(uint64_t)d7; }
#define LOGPRINTF_XXXXXXXX(fmt, d1,d2,d3,d4,d5,d6,d7,d8) { int i=UPC_Atomic_Incr(&plogIdx); prtlog[i].tIdx=PhysicalThreadIndex(); prtlog[i].type=8; prtlog[i].f=fmt; prtlog[i].a1.x=(uint64_t)d1; prtlog[i].a2.x=(uint64_t)d2; prtlog[i].a3.x=(uint64_t)d3; prtlog[i].a4.x=(uint64_t)d4; prtlog[i].a5.x=(uint64_t)d5; prtlog[i].a6.x=(uint64_t)d6; prtlog[i].a7.x=(uint64_t)d7; prtlog[i].a8.x=(uint64_t)d8; }

#endif


#ifdef __FOR_CYCLESIM__
__INLINE__ void printlog()
#if 1
{}
#else
{
    int i;
    for (i=1; i<=plogIdx; i++) {
        PRINTF_XX("core_%02d_%d: ", (prtlog[i].tIdx / 4), (prtlog[i].tIdx % 4));
        _PRTARGS( (prtlog[i].type + 1) );
        _PRTSTRG(prtlog[i].f);
        switch (prtlog[i].type) {
        case 0: break;
        case 1: _PRT64(prtlog[i].a1.x); break;
        case 2: _PRT64(prtlog[i].a1.x); _PRT64(prtlog[i].a2.x); break;
        case 3: _PRT64(prtlog[i].a1.x); _PRT64(prtlog[i].a2.x); _PRT64(prtlog[i].a3.x); break;
        case 4: _PRT64(prtlog[i].a1.x); _PRT64(prtlog[i].a2.x); _PRT64(prtlog[i].a3.x); _PRT64(prtlog[i].a4.x); break;
        case 5: _PRT64(prtlog[i].a1.x); _PRT64(prtlog[i].a2.x); _PRT64(prtlog[i].a3.x); _PRT64(prtlog[i].a4.x); _PRT64(prtlog[i].a5.x); break;
        case 6: _PRT64(prtlog[i].a1.x); _PRT64(prtlog[i].a2.x); _PRT64(prtlog[i].a3.x); _PRT64(prtlog[i].a4.x); _PRT64(prtlog[i].a5.x); _PRT64(prtlog[i].a6.x); break;
        case 7: _PRT64(prtlog[i].a1.x); _PRT64(prtlog[i].a2.x); _PRT64(prtlog[i].a3.x); _PRT64(prtlog[i].a4.x); _PRT64(prtlog[i].a5.x); _PRT64(prtlog[i].a6.x); _PRT64(prtlog[i].a7.x); break;
        case 8: _PRT64(prtlog[i].a1.x); _PRT64(prtlog[i].a2.x); _PRT64(prtlog[i].a3.x); _PRT64(prtlog[i].a4.x); _PRT64(prtlog[i].a5.x); _PRT64(prtlog[i].a6.x); _PRT64(prtlog[i].a7.x); _PRT64(prtlog[i].a8.x); break;
        default: ;
        };
    }
}
#endif

#else
__INLINE__ void printlog()
{
    int i;
    printf("index: %d\n", plogIdx);
    printf("prtlog[1].type: %ld\n", prtlog[1].type);
    printf("prtlog[1].f ptr: 0x%016lx\n", (uint64_t)(prtlog[1].f));
    printf("prtlog[1].f: %s\n", prtlog[1].f);
    for (i=1; i<=plogIdx; i++) {
        printf("core_%02ld_%ld: ", (prtlog[i].tIdx / 4), (prtlog[i].tIdx % 4));
        switch (prtlog[i].type) {
        case 0: printf(prtlog[i].f); break;
        case 1: printf(prtlog[i].f, prtlog[i].a1.x); break;
        case 2: printf(prtlog[i].f, prtlog[i].a1.x, prtlog[i].a2.x); break;
        case 3: printf(prtlog[i].f, prtlog[i].a1.x, prtlog[i].a2.x, prtlog[i].a3.x); break;
        case 4: printf(prtlog[i].f, prtlog[i].a1.x, prtlog[i].a2.x, prtlog[i].a3.x, prtlog[i].a4.x); break;
        case 5: printf(prtlog[i].f, prtlog[i].a1.x, prtlog[i].a2.x, prtlog[i].a3.x, prtlog[i].a4.x, prtlog[i].a5.x); break;
        case 6: printf(prtlog[i].f, prtlog[i].a1.x, prtlog[i].a2.x, prtlog[i].a3.x, prtlog[i].a4.x, prtlog[i].a5.x, prtlog[i].a6.x); break;
        case 7: printf(prtlog[i].f, prtlog[i].a1.x, prtlog[i].a2.x, prtlog[i].a3.x, prtlog[i].a4.x, prtlog[i].a5.x, prtlog[i].a6.x, prtlog[i].a7.x); break;
        case 8: printf(prtlog[i].f, prtlog[i].a1.x, prtlog[i].a2.x, prtlog[i].a3.x, prtlog[i].a4.x, prtlog[i].a5.x, prtlog[i].a6.x, prtlog[i].a7.x, prtlog[i].a8.x); break;
        default: ;
        };
    }
}
#endif

__END_DECLS


#endif
