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
/* (C) Copyright IBM Corp.  2008, 2012                              */
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

#ifndef _BGPM_TYPES_H_  // Prevent multiple inclusion
#define _BGPM_TYPES_H_

#include "hwi/include/common/compiler_support.h"
#include "spi/include/upci/upci.h"


//! \addtogroup bgpm_user_api
//@{
/**
 *
 * \file
 *
 * \brief BGPM User API Types and constants.
 *
 */
//@}

__BEGIN_DECLS


#define BGPM_TRUE (1)
#define BGPM_FALSE (0)


/**
 * \typedef Bgpm_Modes
 *
 * There are quite a few BGPM operation modes. Normally, you want to use
 * the default BGPM_MODE_SWDISTRIB, and only use other modes for special needs.
 *
 * - BGPM_MODE_DEFAULT
 *     - Use \ref BGPM_MODE environment variable to get actual mode.
 *     - Use \ref BGPM_MODE_SWDISTRIB mode if not defined.
 *
 * - BGPM_MODE_LLDISTRIB
 *     - Use with Low Latency methods for "fine-grained" control of Punit counters.
 *     - Only current SW/HW Thread perspective (can only control counters associated with current
 *       HW thread)
 *     - Only \ref counter_ll_handle_functions are useful for Punit.
 *     - Punit counters only have 14 bit capacity.
 *     - No support for overflow interrupt handling, multiplexing, or context switches.
 *
 * - BGPM_MODE_SWDISTRIB
 *     - SW Thread Perspective
 *     - Each Punit provides at least 6 x 64 bit simultaneous counters for each thread.
 *       Two threads per core can achieve 12 counters per thread.  However attempts to
 *       configure more than 6 punit events with more than 2 threads per core will
 *       fail with an out of resources error.
 *     - Each punit thread has independent control of it's own counters.
 *     - \ref bgpm_swfeatures_multiplex of Punit counters is supported
 *     - \ref bgpm_swfeatures_fast_overflow handling of Punit counters is possible.
 *     - The \ref L2unit_desc and \ref iounit_desc support \ref bgpm_swfeatures_slow_overflow handling
 *     - The standard access methods can be used (\ref counter_functions and \ref fast_punit_functions).
 *
 * - BGPM_MODE_HWDISTRIB
 *     - HW Thread Perspective with one controlling sw thread.
 *       That is, use when 1 software thread is desired to control configuration and
 *       control of all hardware events and counting.
 *     - Each Punit provides 6 x 64 bit counters for each HW Thread.
 *     - Control (start/stop) is synchronized for all punits, L2 and I/O units
 *       (other units are independent).
 *     - \ref bgpm_swfeatures_multiplex of Punit counters is NOT supported
 *     - \ref bgpm_swfeatures_overflow is also NOT supported.
 *     - Only the standard access methods can be used (\ref counter_functions and \ref fast_punit_functions).
 *
 */
#if 0
/*
 * Not Implemented:
 *
 * - BGPM_MODE_HWDETAIL (Support for this mode is deferred to a later release)
 *     - HW Thread Perspective.
 *     - UPC hardware is configured to be able to gather more event counts from a single Punit.
 *       In this case, all A2 event signals (thought still limited to 2 or 4 per subunit)
 *       or all L1p signals (for a single L1p mode) and wakeup event signals
 *       can be gathered simultaneously, rather than being limited to 24 counters for a core.
 *       However, the BGPM must select events in groups, which are organized by hardware thread.
 *     - Control (start/stop) is synchronized for the Punit and I/O units.  The L2 Unit is not available.
 *     - Only the standard access methods can be used (\ref counter_functions and \ref fast_counter_functions).
 *     - All counters are subject to an 800 cycle update delay.
 * - BGPM_MODE_TRACE (Planned support for this mode is cancelled - does not work as intended).
 *     - hardware is configured to collect a 48 bit processor cycle trace
 *       of the A2 debug and trigger bus for only one Punit A2 core.
 *     - Only provides for the trace from a hardware perspective (\ref BGPM_HW_PERSPECTIVE)
 *     - The trace can only be 1536 trace events long, but can have a varying trigger position by indicating
 *       the number of events to continue to trace after the trigger.
 *     - The trace may be triggered in one of 3 ways:
 *         - via a software trigger (inline function call),
 *         - on an overflow of one of the local 24 Punit counters (Opcode counts cannot be used).
 *         - Via one A2 trigger bus signal.
 *       See \ref bgpm_swfeatures_trace_mode for more information.
 */
#endif
typedef enum eBgpm_Modes {
    BGPM_MODE_DEFAULT    = 0,  //!<  Use Default mode defined by job env var \ref BGPM_MODE; Otherwise use \ref BGPM_MODE_HWDISTRIB
    BGPM_MODE_LLDISTRIB  = 1,  //!<  Use the Low Latency SW mode - punit provides 14 bit local counters.
    BGPM_MODE_SWDISTRIB  = 2,  //!<  Use SW Mode - UPC Distributed Mode from SW Perspective - 6 or12 x 64 bit counters for each punit thread (
    BGPM_MODE_HWDISTRIB  = 3,  //!<  Use HW Distributed Mode - 6 x 64 bit counters for each hw thread.
    //BGPM_MODE_HWDETAIL   = 4,  //!<  deferred.  Use HW Detailed Mode - ?? x 64 bit counters for one PUnit
    //BGPM_MODE_TRACE      = 5,  //!<  Use HW Trace mode to trace debug bus of one Punit A2 CPU
    BGPM_MAX_MODES       = 4
} Bgpm_Modes;


/** 
 * \typedef RsvMuxCtrs
 * This acts as parameter to SetMultiplex2 function and indicates how many events will be multiplexed
 */
typedef enum eRsvMuxCtrs {
    BGPMMuxMinEvts = 5,
    BGPMMuxMedEvts = 11,
    BGPMMuxMaxEvts = 23
}RsvMuxCtrs;


/**
 * \typedef Bgpm_Context
 *
 * These values control the PUnits ability to filter A2 events for kernel or user state.
 * The ability is a core-wide setting, so 1st thread to set for a core wins. If a thread does
 * not explicitly set the context, than another thread has the ability to override and effect
 * the operation of prior threads on the core.
 *
 * WARNING: This is a A2 event attribute only, and the Instruction Opcode or L1p/Wakeup events
 * are not sensitive to the context setting.  You will not be allow to mix A2 events with
 * Instruction Opcode or L1p/Wakeup event if the context is other than both.
 *
 * \note The context must be set prior to added any events to a punit event set, otherwise
 * the context will be set to BGPM_CTX_BOTH or whatever is passed in the BGPM_CONTEXT environment
 * variable.
 *
 *
 * - BGPM_CTX_DEFAULT - Get context from job env var \ref BGPM_CONTEXT or use \ref BGPM_CTX_BOTH
 * - BGPM_CTX_KERNEL  - Have A2 events counted only while in kernel state.
 * - BGPM_CTX_USER    - Have A2 events counted only while in user state.
 * - BGPM_CTX_BOTH    - Have A2 and other events be counted in both user and kernel state
 */
typedef enum eBgpm_Context{
    BGPM_CTX_DEFAULT  = 0,  //!<  Use Default context defined by job env var \ref BGPM_CONTEXT; Otherwise use \ref BGPM_CTX_USER
    BGPM_CTX_KERNEL   = 1,  //!<  include events while in kernel state
    BGPM_CTX_USER     = 2,  //!<  include events while in user state
    BGPM_CTX_BOTH     = 3,  //!<  include events while in both kernel and user state
} Bgpm_Context;



/**
 * \typedef Bgpml1pModes
 *
 * Possible L1p Modes set via L1p events assigned to an event set.
 * There may only be one active mode for an entire node.
 *
 * - BGPM_CXT_DEFAULT - Get context from job env var \ref BGPM_CONTEXT or use BGPM_CXT_USER
 * - BGPM_CXT_KERNEL  - Have A2 events counted only while in kernel state.
 * - BGPM_CXT_USER    - Have A2 events counted only while in user state.
 * - BGPM_CTX_BOTH    - Have A2 events counted while either in user or kernel state
 */
typedef enum eBgpml1pModes {
    BGPMl1pMode_UNDEF = 0,  //!<  Mode has not been reserved by any event set.
    BGPMl1pMode_STREAM = 1,  //!< L1p Stream events possible
    BGPMl1pMode_SWITCH = 2,  //!< L1p Switch Events possible
    BGPMl1pMode_LIST  = 3,  //!< L1p List events possible
} Bgpml1pModes;



/**
 * \typedef BgpmUnitTypes
 *
 * Supported Unit types
 *
 */
typedef enum eBgpmUnitTypes {
    BGPMUnitType_UNDEF = 0,
    BGPMUnitType_PUNIT = 1,
    BGPMUnitType_L2    = 2,
    BGPMUnitType_IO    = 3,
    BGPMUnitType_NW    = 4,
    BGPMUnitType_CNK   = 5
} BgpmUnitTypes;




#define BGPM_MAX_PUNIT_HANDLES (UPC_P_NUM_COUNTERS+20)
/**
 * \typedef Bgpm_Punit_Handles_t
 *
 * Structure to contain Fast Control Handles for a Punit event set.
 * See Bgpm_Punit_GetHandles()
 *
 */
typedef struct sBgpm_Punit_Handles {
    uint64_t allCtrMask;     //!< Control mask for all counters (incl shared counters)
    //uint64_t thdCtrMask;     //!< Control mask for threaded counters (excluding shared counters)
    unsigned numCtrs;        //!< Number of counters
    uint64_t hCtr[BGPM_MAX_PUNIT_HANDLES];  //!< Array of ctr handles - hold both distrib and detailed array of ctrs per thread.
} Bgpm_Punit_Handles_t;



/**
 * \typedef Bgpm_Punit_LLHandles_t
 *
 * Structure to contain Fast Control Handles for a Punit event set.
 * See Bgpm_Punit_GetLLHandles()
 *
 */
typedef struct sBgpm_Punit_LLHandles {
    uint64_t allCtrMask;     //!< Control mask for all counters (incl shared counters)
    //uint64_t thdCtrMask;     //!< Control mask for threaded counters (excluding shared counters)
    unsigned numCtrs;        //!< Number of counters
    uint64_t hCtr[UPC_P_NUM_COUNTERS];  //!< Array of ctr handles - hold both distrib and detailed array of ctrs per thread.
} Bgpm_Punit_LLHandles_t;






/**
 * \typedef Bgpm_EventInfo_t
 *
 * Structure to contain characteristics of a particular event.
 * See Bgpm_GetEventIdInfo()
 *
 */
typedef struct sEventInfo {
    unsigned           evtId;
    const char *       label;
    const char *       desc;
    BgpmUnitTypes      unitType;
    UPC_EventSources_t srcType;        //!< subunit granular type
    UPC_EventScope_t   scope;
    UPC_EventMetric_t  metric;
    ushort             edge;
    ushort             inverted;
    uint32_t           selVal;         //!< internal unit selection value (for test purposes)
} Bgpm_EventInfo_t;
#define LONG_DESC_FILE "/bgsys/drivers/ppcfloor/bgpm/lib/Bgpm_EventDetails.txt"



/**
 * \typedef Bgpm_MuxStats_t
 *
 * Structure to contain settings and statics for Punit Mulitiplexing operations
 * See Bgpm_GetMultiplex()
 *
 */
typedef struct sMuxStats {
    int                active;         //!< 0 or 1 if Muliplexing is active on this event set
    uint64_t           period;         //!< cycles between mux switches if handled by Bgpm
    int                normalize;      //!< nonzero if reported event counts should be normalized
    unsigned           numGrps;        //!< number of mux groups currently in this event set
    unsigned           maxEvtsPerGrp;  //!< Maximum events usable per group (based on number of threads set)
    unsigned           numSwitches;    //!< number of switches between groups that have occurred since reset
    uint64_t           elapsedCycles;  //!< total elapsed cycles across all groups
} Bgpm_MuxStats_t;





/**
 * macros to help build instruction matching parameters from instruction opcodes and extended opcodes.
 */
#define BGPM_XOBITS(b,x,s)   (((x) & (0x3FF >> (10 - (s)))) << (30 - (b)))  // define XO field based on original instr form bit positions.
#define BGPM_XOBITS31(b,x,s) ((((x) & (0x7FF >> (11 - (s)))) >> 1) << (31 - (b)))  // define XO field based on original instr form bit positions - but remove bit 31
#define BGPM_OCBITS(x)       ((x)<<10)
#define BGPM_INST_X_FORM_CODE(_opcd,_xo)      (BGPM_OCBITS(_opcd) | BGPM_XOBITS(30,(_xo  ),10))
#define BGPM_INST_X_FORM_MASK                 (BGPM_OCBITS(0x3F ) | BGPM_XOBITS(30,(0x3FF),10))  // minor = (21-30)
#define BGPM_INST_I_FORM_CODE(_opcd)          (BGPM_OCBITS(_opcd))
#define BGPM_INST_I_FORM_MASK                 (BGPM_OCBITS(0x3F ))
#define BGPM_INST_B_FORM_CODE(_opcd)          (BGPM_OCBITS(_opcd))
#define BGPM_INST_B_FORM_MASK                 (BGPM_OCBITS(0x3F ))
#define BGPM_INST_SC_FORM_CODE(_opcd)         (BGPM_OCBITS(_opcd))
#define BGPM_INST_SC_FORM_MASK                (BGPM_OCBITS(0x3F ))
#define BGPM_INST_D_FORM_CODE(_opcd)          (BGPM_OCBITS(_opcd))
#define BGPM_INST_D_FORM_MASK                 (BGPM_OCBITS(0x3F ))
#define BGPM_INST_DS_FORM_CODE(_opcd,_xo)     (BGPM_OCBITS(_opcd) | BGPM_XOBITS31(31,(_xo  ),2))
#define BGPM_INST_DS_FORM_MASK                (BGPM_OCBITS(0x3F ) | BGPM_XOBITS31(31,(0x7FF),2)) // minor = (30-31) and bit 31 is ignored
#define BGPM_INST_DQ_FORM_CODE(_opcd)         (BGPM_OCBITS(_opcd))
#define BGPM_INST_DQ_FORM_MASK                (BGPM_OCBITS(0x3F ))
#define BGPM_INST_XL_FORM_CODE(_opcd,_xo)     (BGPM_OCBITS(_opcd) | BGPM_XOBITS(30,(_xo  ),10))
#define BGPM_INST_XL_FORM_MASK                (BGPM_OCBITS(0x3F ) | BGPM_XOBITS(30,(0x3FF),10))  // minor = (21-30)
#define BGPM_INST_XFL_FORM_CODE(_opcd,_xo)    (BGPM_OCBITS(_opcd) | BGPM_XOBITS(30,(_xo  ),10))
#define BGPM_INST_XFL_FORM_MASK               (BGPM_OCBITS(0x3F ) | BGPM_XOBITS(30,(0x3FF),10))  // minor = (21-30)
#define BGPM_INST_XFX_FORM_CODE(_opcd,_xo)    (BGPM_OCBITS(_opcd) | BGPM_XOBITS(30,(_xo  ),10))
#define BGPM_INST_XFX_FORM_MASK               (BGPM_OCBITS(0x3F ) | BGPM_XOBITS(30,(0x3FF),10))  // minor = (21-30)
#define BGPM_INST_XS_FORM_CODE(_opcd,_xo)     (BGPM_OCBITS(_opcd) | BGPM_XOBITS(29,(_xo  ), 9))
#define BGPM_INST_XS_FORM_MASK                (BGPM_OCBITS(0x3F ) | BGPM_XOBITS(29,(0x3FF), 9))  // minor = (21-29)
#define BGPM_INST_XO_FORM_CODE(_opcd,_xo)     (BGPM_OCBITS(_opcd) | BGPM_XOBITS(30,(_xo  ), 9))
#define BGPM_INST_XO_FORM_MASK                (BGPM_OCBITS(0x3F ) | BGPM_XOBITS(30,(0x3FF), 9))  // minor = (22-30)
#define BGPM_INST_A_FORM_CODE(_opcd,_xo)      (BGPM_OCBITS(_opcd) | BGPM_XOBITS(30,(_xo  ), 5))
#define BGPM_INST_A_FORM_MASK                 (BGPM_OCBITS(0x3F ) | BGPM_XOBITS(30,(0x3FF), 5))  // minor = (26-30)
#define BGPM_INST_M_FORM_CODE(_opcd)          (BGPM_OCBITS(_opcd))
#define BGPM_INST_M_FORM_MASK                 (BGPM_OCBITS(0x3F ))
#define BGPM_INST_MD_FORM_CODE(_opcd,_xo)     (BGPM_OCBITS(_opcd) | BGPM_XOBITS(29,(_xo  ), 3))
#define BGPM_INST_MD_FORM_MASK                (BGPM_OCBITS(0x3F ) | BGPM_XOBITS(29,(0x3FF), 3))  // minor = (27-29)
#define BGPM_INST_MDS_FORM_CODE(_opcd,_xo)    (BGPM_OCBITS(_opcd) | BGPM_XOBITS(30,(_xo  ), 4))
#define BGPM_INST_MDS_FORM_MASK               (BGPM_OCBITS(0x3F ) | BGPM_XOBITS(30,(0x3FF), 4))  // minor = (27-30)
#define BGPM_INST_EVX_FORM_CODE(_opcd,_xo)    (BGPM_OCBITS(_opcd) | BGPM_XOBITS31(31,(_xo  ),11))
#define BGPM_INST_EVX_FORM_MASK               (BGPM_OCBITS(0x3F ) | BGPM_XOBITS31(31,(0x7FF),11))  // minor = (21-31), but bit 31 is ignored
#define BGPM_INST_Z23_FORM_CODE(_opcd,_xo)    (BGPM_OCBITS(_opcd) | BGPM_XOBITS(30,(_xo  ), 8))
#define BGPM_INST_Z23_FORM_MASK               (BGPM_OCBITS(0x3F ) | BGPM_XOBITS(30,(0x7FF), 8))  // minor = (23-30)




// Debug Variable Structures to access from user space.
// Internal Note: Each thread has independent copies of these variables,
// so no locking is needed for update.
typedef struct sBgpmDebugThreadVars
{
    ushort   ovfEnabled;    // an event has had overflows enabled (in debugtrace this causes this object to be dumped after stop operation)
    uint64_t numPmSignals;  // Number of times pm signal called for this thread.
    uint64_t numFastOvfs;   // Number of fast overflow signals encountered.
    uint64_t numMuxOvfs;    // Number of overflow signals delivered to mux switching
    uint64_t numSharedUpcpOvfs;            // Number of upcp shared overflow signals encountered.
    uint64_t numSharedUpcpOvfsDelivered;   // Number of upcp shared overflow delivered to handlers.
    uint64_t numUpcCOvfs;                  // Number of upcc shared overflows.
    uint64_t numUpcCOvfsDelivered;         // Number of upcC shared overflows delivered to handlers

    // number of pm signals w/o a matching interrupt status.
    // This can occur on fast overflows if counter overflows are broadcast
    // when upc_p control shared bit is set (causes "any" int status bit).
    // This is set when more than 6 counters allocated to thread.
    // Thus, a 2nd thread could get interrupt, even though none of the
    // overflow are for it's counters.
    // It also occur with UPC_C interrupts on PM processes which don't care about
    // active L2 or I/O overflows.
    uint64_t numPmSigsWOInt;

    // Number of Ovfs when upc_p ctr groups are shared which had to be dropped because
    // the overflows were happening too fast to keep up.
    uint64_t droppedSharedUpcPOvfs;

    // max length of que entries which had to be processed in a single signal instance.
    uint64_t maxUpcPSharedQueLen;

    // Number of Ovfs when upc_c ctr groups are shared which had to be dropped because
    // the overflows were happening too fast to keep up.
    uint64_t droppedSharedUpcCOvfs;

    // max length of que entries which had to be processed in a single signal instance.
    uint64_t maxUpcCSharedQueLen;

} BgpmDebugThreadVars_t;







__END_DECLS

#endif
