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
#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "spi/include/upci/upc_atomic.h"



//! Define gymnastics to create a compile time AT string.
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define _AT_ __FILE__ ":" TOSTRING(__LINE__)

#define PRINTERR(...) fprintf(stderr, "ERROR(" _AT_ ") " __VA_ARGS__);
#define DIE(...) { PRINTERR(__VA_ARGS__); exit(-1); }

#ifndef __FWEXT__
#include "test_utils/shared_critical.h"
#endif

#include "test_utils/quikProfiler.h"

#define CYC2USEC(c) ( 0.000625 * (double) (c) )   //!<  1.6 Ghz = 1600 MHz or 1600 c/usec

#define GTB GetTimeBase()                //!< short name for convenience
typedef uint64_t cycle_t;                //!< cycle count types returned from GetTimeBase();


#ifndef LIKELY
#define LIKELY(x)       __builtin_expect(!!(x), 1)
#define UNLIKELY(x)     __builtin_expect(!!(x), 0)
#endif


#ifndef MIN
#define MIN(a,b) ( ( (a) > (b) ) ? (b) : (a) )
#define MAX(a,b) ( ( (a) > (b) ) ? (a) : (b) )
#endif



//! STAT struct to accumulate statistics
#define ADDSTAT( metric, value )  \
   metric.num++; \
   metric.min=MIN(metric.min, value); \
   metric.max=MAX(metric.max, value); \
   metric.sum+=value;

#define INITSTAT( metric ) \
	   metric.num=0; \
	   metric.min=0x7FFFFFFFFFFFFFFF; \
	   metric.max=0; \
	   metric.sum=0;

typedef struct STAT {
   uint64_t num;
   uint64_t min;
   uint64_t max;
   uint64_t sum;
} STAT;


__BEGIN_DECLS;


// Maintain a Decimal Histogram with the given number of buckets + 2
// (where 0th bucket contains # of entries less than the min value,
// and buckets+1 is # of entries > last bucket
// InitDecHistory returns a new instance of a histogram. Each thread should use it's own instance.
uint64_t *InitDecHistogram(unsigned buckets, uint64_t minVal, uint64_t maxVal);
// Add Entry
void Add2DecHistogram(uint64_t *pDecHistogram, uint64_t value);
// Print Results
void PrintDecHistogram(const char *title, uint64_t *pDecHistogram);
void DeleteDecHistogram(uint64_t *pDecHistogram);



// Return static string with node name, coordinates, and rank.
const char *WhoAmI();
__INLINE__ void PrintWhoAmI() { fprintf(stderr, "WhoAmI:  %s\n", WhoAmI()); }



typedef struct {
    uint64_t shared;
    uint64_t persist;
    uint64_t heapavail;
    uint64_t estheapavail;
    uint64_t stackavail;
    uint64_t eststackavail;
    uint64_t stack;
    uint64_t eststack;
    uint64_t heap;
    uint64_t guard;
    uint64_t heapmax;
    uint64_t mmap;
} MemSizes_t;

void GetMemSizes(MemSizes_t * memSizes);
void PrintMemSizes(const char *location, MemSizes_t * memSizes);




// char strg[50];
// fprintf(stderr, "%-50s -> %50s\n", label, UInt64toStrg(counterval, strg, 50));
__INLINE__ const char *UInt64toStrg(uint64_t val, char *strg, int maxLen) {
    char fmt[15];
    sprintf(fmt, "%%%dld", maxLen-1);
    int valLen = snprintf(strg, maxLen, "%-ld", val);
    int endPos = snprintf(strg, maxLen, fmt, val);
    int trgPos = 0;
    int srcPos = endPos-valLen;
    int digitNum = valLen;
    while (srcPos < endPos) {
        if (((digitNum % 3)==0) && (digitNum < valLen)) {
            strg[trgPos++] = ',';
        }
        strg[trgPos++] = strg[srcPos++];
        digitNum--;
    }
    strg[trgPos] = '\0';
    return strg;
}





#ifdef TEST_CASE_VERBOSE
#undef TEST_CASE_VERBOSE
#define TEST_CASE_VERBOSE (1)
#else
#define TEST_CASE_VERBOSE (0)
#endif

#define TEST_INIT() \
    int numFailures = 0;

#define TEST_AT() \
    fprintf(stderr, "%02d " _AT_ "\n", Kernel_ProcessorID());

#if TEST_CASE_VERBOSE
    #define VERBOSE_PRINT1(_fmt_) \
        fprintf(stderr, "%02d " _AT_ " " _fmt_, Kernel_ProcessorID());
    #define VERBOSE_PRINT(_fmt_, ...) \
        fprintf(stderr, "%02d " _AT_ " " _fmt_, Kernel_ProcessorID(), __VA_ARGS__);
    #define IFVERBOSE(_cmd_) _cmd_
#else
    #define VERBOSE_PRINT(_fmt_, ...)
    #define VERBOSE_PRINT1(_fmt_)
    #define IFVERBOSE(_cmd_)
#endif

#define TEST_MODULE(...) \
{ \
    int _thd = Kernel_ProcessorID(); \
    fprintf(stderr, "%02d \n", _thd); \
    fprintf(stderr, "%02d #############################################\n", _thd); \
    fprintf(stderr, "%02d TEST_MODULE ("_AT_"): %s\n", _thd, #__VA_ARGS__); \
    fprintf(stderr, "%02d #############################################\n", _thd); \
}

#define TEST_MODULE_PRINT0(...) \
{ \
    int _thd = Kernel_ProcessorID(); \
    if (_thd == 0) { \
        fprintf(stderr, "%02d \n", _thd); \
        fprintf(stderr, "%02d #############################################\n", _thd); \
        fprintf(stderr, "%02d TEST_MODULE ("_AT_"): %s\n", _thd, #__VA_ARGS__); \
        fprintf(stderr, "%02d #############################################\n", _thd); \
    } \
}

#define TEST_CASE(...) \
{ \
    int _thd = Kernel_ProcessorID(); \
    fprintf(stderr, "%02d \n", _thd); \
    fprintf(stderr, "%02d ---------------------------------------------\n", _thd); \
    fprintf(stderr, "%02d TEST_CASE ("_AT_"): %s\n", _thd, #__VA_ARGS__); \
    fprintf(stderr, "%02d ---------------------------------------------\n", _thd); \
}

#define TEST_CASE_PRINT0(...) \
{ \
    int _thd = Kernel_ProcessorID(); \
    if (_thd == 0) { \
        fprintf(stderr, "%02d \n", _thd); \
        fprintf(stderr, "%02d ---------------------------------------------\n", _thd); \
        fprintf(stderr, "%02d TEST_CASE ("_AT_"): %s\n", _thd, #__VA_ARGS__); \
        fprintf(stderr, "%02d ---------------------------------------------\n", _thd); \
    } \
}

#define TEST_CHECK(...) \
{ \
    int _thd = Kernel_ProcessorID(); \
    if (!(__VA_ARGS__)) {  \
        fprintf(stderr, "%02d TEST_CHECK FAIL (" _AT_ "): %s\n", _thd, #__VA_ARGS__); \
        numFailures++; \
    } \
    else if (TEST_CASE_VERBOSE) { \
        fprintf(stderr, "%02d TEST_CHECK GOOD (" _AT_ "): %s\n", _thd, #__VA_ARGS__); \
    } \
}

#define TEST_CHECK_EQUAL(_lv_, _rv_) \
{ \
    int _thd = Kernel_ProcessorID(); \
    uint64_t lv = (uint64_t)(_lv_); \
    uint64_t rv = (uint64_t)(_rv_); \
    if (lv != rv) {  \
        fprintf(stderr, "%02d TEST_CHECK_EQUAL FAIL (" _AT_ "): " #_lv_ "{%ld} != " #_rv_ "{%ld}\n", _thd, lv, rv); \
        numFailures++; \
    } \
    else if (TEST_CASE_VERBOSE) { \
        fprintf(stderr, "%02d TEST_CHECK_EQUAL GOOD (" _AT_ "): " #_lv_ "{%ld} == " #_rv_ "{%ld}\n", _thd, lv, rv); \
    } \
}

#define TEST_CHECK_EQUAL_HEX(_lv_, _rv_) \
{ \
    int _thd = Kernel_ProcessorID(); \
    uint64_t lv = (uint64_t)(_lv_); \
    uint64_t rv = (uint64_t)(_rv_); \
    if (lv != rv) {  \
        fprintf(stderr, "%02d TEST_CHECK_EQUAL_HEX FAIL (" _AT_ "): " #_lv_ "{0x%016lx} != " #_rv_ "{0x%016lx}\n", _thd, lv, rv); \
        numFailures++; \
    } \
    else if (TEST_CASE_VERBOSE) { \
        fprintf(stderr, "%02d TEST_CHECK_EQUAL_HEX GOOD (" _AT_ "): " #_lv_ "{0x%016lx} == " #_rv_ "{0x%016lx}\n", _thd, lv, rv); \
    } \
}

#define TEST_CHECK_EQUALDBL(_lv_, _rv_) \
{ \
    int _thd = Kernel_ProcessorID(); \
    double lv = (_lv_); \
    double rv = (_rv_); \
    if (lv != rv) {  \
        fprintf(stderr, "%02d TEST_CHECK_EQUALDBL FAIL (" _AT_ "): " #_lv_ "{%f} != " #_rv_ "{%f}\n", _thd, lv, rv); \
        numFailures++; \
    } \
    else if (TEST_CASE_VERBOSE) { \
        fprintf(stderr, "%02d TEST_CHECK_EQUALDBL GOOD (" _AT_ "): " #_lv_ "{%f} == " #_rv_ "{%f}\n", _thd, lv, rv); \
    } \
}

#define TEST_CHECK_NONZERO(_v_) \
{ \
    int _thd = Kernel_ProcessorID(); \
    uint64_t v = (uint64_t)(_v_); \
    if (v == 0) {  \
        fprintf(stderr, "%02d TEST_CHECK_NONZERO FAIL (" _AT_ "): " #_v_ "{%ld} == 0\n", _thd, v); \
        numFailures++; \
    } \
    else if (TEST_CASE_VERBOSE) { \
        fprintf(stderr, "%02d TEST_CHECK_NONZERO GOOD (" _AT_ "): " #_v_ "{%ld} != 0\n", _thd, v); \
    } \
}

#define TEST_CHECK_RANGE(_lv_, _rlo_, _rhi_) \
{ \
    int _thd = Kernel_ProcessorID(); \
    uint64_t lv = (uint64_t)(_lv_); \
    uint64_t rlo = (uint64_t)(_rlo_); \
    uint64_t rhi = (uint64_t)(_rhi_); \
    if ((lv < rlo) || (lv > rhi)) {  \
        fprintf(stderr, "%02d TEST_CHECK_RANGE FAIL (" _AT_ "): " #_lv_ "{%ld} is not within {%ld} and {%ld} \n", _thd, lv, rlo, rhi); \
        numFailures++; \
    } \
    else if (TEST_CASE_VERBOSE) { \
        fprintf(stderr, "%02d TEST_CHECK_RANGE GOOD (" _AT_ "): " #_lv_ "{%ld} is within {%ld} and {%ld} \n", _thd, lv, rlo, rhi); \
    } \
}

#define TEST_CHECK_RANGE_wLABEL(_lv_, _rlo_, _rhi_, _label_) \
{ \
    int _thd = Kernel_ProcessorID(); \
    double lv = (_lv_); \
    double rlo = (_rlo_); \
    double rhi = (_rhi_); \
    if ((lv < rlo) || (lv > rhi)) {  \
        fprintf(stderr, "%02d TEST_CHECK_RANGE_wLABEL FAIL (" _AT_ "): " #_lv_ "{%ld} is not within {%ld} and {%ld} for %s\n", _thd, lv, rlo, rhi, _label_); \
        numFailures++; \
    } \
    else if (TEST_CASE_VERBOSE) { \
        fprintf(stderr, "%02d TEST_CHECK_RANGE_wLABEL GOOD (" _AT_ "): " #_lv_ "{%ld} is within {%ld} and {%ld} for %s\n", _thd, lv, rlo, rhi, _label_); \
    } \
}

#define TEST_CHECK_RANGEDBL_wLABEL(_lv_, _rlo_, _rhi_, _label_) \
{ \
    int _thd = Kernel_ProcessorID(); \
    double lv = (_lv_); \
    double rlo = (_rlo_); \
    double rhi = (_rhi_); \
    if ((lv < rlo) || (lv > rhi)) {  \
        fprintf(stderr, "%02d TEST_CHECK_RANGEDBL_wLABEL FAIL (" _AT_ "): " #_lv_ "{%lf} is not within {%lf} and {%lf} for %s\n", _thd, lv, rlo, rhi, _label_); \
        numFailures++; \
    } \
    else if (TEST_CASE_VERBOSE) { \
        fprintf(stderr, "%02d TEST_CHECK_RANGEDBL_wLABEL GOOD (" _AT_ "): " #_lv_ "{%lf} is within {%lf} and {%lf} for %s\n", _thd, lv, rlo, rhi, _label_); \
    } \
}

#define TEST_CHECK_RANGEDBL(_lv_, _rlo_, _rhi_) \
{ \
    int _thd = Kernel_ProcessorID(); \
    double lv = (_lv_); \
    double rlo = (_rlo_); \
    double rhi = (_rhi_); \
    if ((lv < rlo) || (lv > rhi)) {  \
        fprintf(stderr, "%02d TEST_CHECK_RANGEDBL FAIL (" _AT_ "): " #_lv_ "{%lf} is not within {%lf} and {%lf} \n", _thd, lv, rlo, rhi); \
        numFailures++; \
    } \
    else if (TEST_CASE_VERBOSE) { \
        fprintf(stderr, "%02d TEST_CHECK_RANGEDBL GOOD (" _AT_ "): " #_lv_ "{%lf} is within {%lf} and {%lf} \n", _thd, lv, rlo, rhi); \
    } \
}

#define TEST_FUNCTION(...) \
{ \
    fprintf(stderr, "                                                                   \n"); \
    fprintf(stderr, "                                                                   \n"); \
    fprintf(stderr, "===================================================================\n"); \
    fprintf(stderr, "TEST_FUNCTION ("_AT_"): %s\n", #__VA_ARGS__); \
    fprintf(stderr, "===================================================================\n"); \
}
#define TEST_FUNCTION_PARM(...) \
{ \
    fprintf(stderr, "                                                                   \n"); \
    fprintf(stderr, "PARAMETER("_AT_"): %s\n", #__VA_ARGS__); \
    fprintf(stderr, "---------------------------------------------------\n"); \
}

#define TEST_CHECK_ABORT(...) \
{ \
    int _thd = Kernel_ProcessorID(); \
    if ((__VA_ARGS__)) {  \
        fprintf(stderr, "%02d TEST_CHECK_ABORT FAIL (" _AT_ "): %s\n", _thd, #__VA_ARGS__); \
        numFailures++; \
        exit(numFailures); \
    } \
    else if (TEST_CASE_VERBOSE) { \
        fprintf(stderr, "%02d TEST_CHECK_ABORT GOOD (" _AT_ "): %s\n", _thd, #__VA_ARGS__); \
    } \
}

#define TEST_RETURN() \
{ \
    int _thd = Kernel_ProcessorID(); \
    if (numFailures != 0) { \
        fprintf(stderr, "%02d TEST_EXIT FAIL (" _AT_ "): %d\n", _thd, numFailures); \
    } \
    else {\
        fprintf(stderr, "%02d TEST_EXIT SUCCESS (" _AT_ "): %d\n", _thd, numFailures); \
    } \
    return numFailures; \
}







__END_DECLS


#endif
