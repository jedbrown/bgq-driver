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
#ifndef KCOUNTERS_H_
#define KCOUNTERS_H_

#include <hwi/include/common/compiler_support.h>
__BEGIN_DECLS

enum
{
#define NODEPERFCOUNTER(name, description) name,
#include <cnk/include/kcounters.h>
    CNK_NODEPERFCOUNT_NUM
};

enum
{
#define PROCPERFCOUNTER(name, description) name,
#include <cnk/include/kcounters.h>
    CNK_PROCPERFCOUNT_NUM
};

enum
{
#define HWTPERFCOUNTER(name, description) name,
#include <cnk/include/kcounters.h>
    CNK_HWTPERFCOUNT_NUM
};

typedef struct PerfCountItem
{
    uint64_t id;
    uint64_t value;
} PerfCountItem_t;

extern int PerfCtr_GetNodeData(int numitems, PerfCountItem_t* items);
extern int PerfCtr_GetProcData(int numitems, PerfCountItem_t* items);
extern int PerfCtr_GetHWTData(int numitems, PerfCountItem_t* items);

__END_DECLS

#endif

#ifndef NODEPERFCOUNTER
#define NODEPERFCOUNTER(name, description)
#endif
#ifndef PROCPERFCOUNTER
#define PROCPERFCOUNTER(name, description)
#endif
#ifndef HWTPERFCOUNTER
#define HWTPERFCOUNTER(name, description)
#endif

    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_SYSCALL,     "System Calls")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_STANDARD,    "External Input Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_CRITICAL,    "Critical Input Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_DECREMENTER, "Decrementer Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_FIT,         "Fixed Interval Timer Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_WATCHDOG,    "Watchdog Timer Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_UDEC,        "User Decrementer Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_PERFMON,     "Performance Monitor interrupts")
    
    // NOTE:  The remaining counters must be kept consistent with the
    //        corresponding DEBUG_CODE's defined in cnk/src/Debug.h.
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_UNKDEBUG,    "Unknown/Invalid Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_DEBUG,       "Debug Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_DSI,         "Data Storage Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_ISI,         "Instruction Storage Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_ALIGNMENT,   "Alignment Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_PROGRAM,     "Program Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_FPU,         "FPU Unavailable Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_APU,         "APU Unavailable Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_DTLB,        "Data TLB Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_ITLB,        "Instruction TLB Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_VECTOR,      "Vector Unavailable Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_UNDEF,       "Undefined Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_PDBI,        "Processor Doorbell Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_PDBCI,       "Processor Doorbell Critical Ints")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_GDBI,        "Guest Doorbell Interrupts")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_GDBCI,       "Guest Doorbell Crit or MChk Ints")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_EHVSC,       "Embedded Hypervisor System Calls")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_EHVPRIV,     "Embedded Hypervisor Privileged")
    HWTPERFCOUNTER(CNK_HWTPERFCOUNT_LRATE,       "LRAT exception")

    NODEPERFCOUNTER(CNK_NODEPERFCOUNT_MU,         "MU Non-fatal interrupt")
    NODEPERFCOUNTER(CNK_NODEPERFCOUNT_ND,         "ND Non-fatal interrupt")

#undef NODEPERFCOUNTER
#undef PROCPERFCOUNTER
#undef HWTPERFCOUNTER
