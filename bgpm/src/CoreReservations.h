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

#ifndef _CORE_RESERVATIONS_H_  // Prevent multiple inclusion
#define _CORE_RESERVATIONS_H_

#include <assert.h>
#include <unistd.h>

#include "cnk/include/Config.h"
#include "spi/include/upci/upci.h"
#include "bgpm/include/types.h"
#include "bgpm/include/err.h"

//! \addtogroup bgpm_api_impl
//@{
/**
 * \file
 *
 * \brief track the counting hardware resources which are common for all threads on a core
 *
 */
//@}

namespace bgpm {


//! Definition of allocMask bits.
typedef enum  {
    Alloc_Clear       = 0x0000,
    Alloc_l1pMode     = 0x0001,
    Alloc_qfpuMatch   = 0x0004,
    Alloc_xuMatch     = 0x0008,
    Alloc_sqrtScale   = 0x0010,
    Alloc_divScale    = 0x0020,
    Alloc_evtCtx      = 0x0040,  //!< Event Context has been set
    Alloc_evtCtxBoth  = 0x0080,  //!< Event has relience on context being set to both.
    Alloc_evtCtxMixed = 0x0100,  //!< Allow mixed event context senstivity.
} CoreDepBits;


//////////////////////////////////////////////////////////////////
/*!
 *
 * CoreReservations structure tracks common core counting resources
 * reserved by a sw thread.
 *
 * Reference counts for the CoreReservation instance is incremented as used by
 * an event set.  Individual resources are reserved only when needed, but only freed
 * when all referencing event sets are destroyed (ref count reaches 0).
 * Thus, a resource might be needed by event set A, not needed by event set B, but
 * even if event set A is destroyed, event set C cannot use it till event set B is also
 * destroyed.
 *
 * **The ctrGrpMask field requires some explanation:
 * The UPC_P counters have a thread affinity and those with the same thread affinity are used
 * first during allocations.  Thus,
 * - counters 0,4,8,12,16,20 belong to the hw thread 0 group
 * - counters 1,5,9,13,17,21 belong to the hw thread 1 group
 * - etc
 *
 * However, the allocation of a hw threads events is allowed to overflow into counters
 * of another threads group if they are not in use.  This allows a single thread
 * to use more than just 6 counters if there are fewer than 4 threads on a core actually
 * doing performance counting. Thus, if only one of thread 0 & 1 are counting, it can use 12 counters.
 * If only one of all 4 threads is counting, then all 24 counters are available, though events are
 * still restricted by A2 event unit limitations and l1p counter assignments.
 *
 * Shared counters should only be allowed when there is only 1 hw thread per core doing counting.
 * the caller must do this check.
 *
 * ** The allocMask field is used to track which core-wide attributes have been reserved.
 * Each punit event set keeps track of which attributes are used by events in the eventset.
 * Then, on Bgpm_Apply or Bgpm_Attach, the current "allocMask" is captured and the
 * corresponding attributes set in the Upci_Punit for application to the hardware.
 *
 * On subsequent operations, the "allocMask" is verified against the captured version in
 * case there are any necessary modifications.
 *
 * Note: the structure instances reside in shared memory and all contents are cleared
 * at shared memory initialization - so no construction dependencies are allowed.
*/
struct CoreReservations
{
    UPC_Lock_t         lock;
    int                refCount;          //!< punit Ref Count (num evtsets + attached punits using core)

    int                ctrGrpRefCount[CONFIG_HWTHREADS_PER_CORE]; //!< event sets using indexed ctrGrpMask
    uint8_t            ctrGrpMask[CONFIG_HWTHREADS_PER_CORE];     //!< left aligned mask of which thread affinity counter groups are in use by indexed thread
                                                                  //!< values must not overlap (how we check for conflict).
    #define            CTRGRP_MASKBIT(grp) (0x80 >> grp)

    uint8_t            puAttachedMask;    //!< mask of ctr groups which have punit evtsets attached

    // Reserved Value Allocation Mask (See enum CoreDepBits)
    unsigned           allocMask;         //!< CoreDepBits: which core resources have values reserved for all threads on core.
                                          //!< Otherwise the reserved values have no meaning
    // Reserved values.
    UPC_EventSources_t l1pMode;           //!< active mode for l1p.
    Bgpm_Context       evtCtx;            //!< how a2 events should be filtered

    uint16_t           qfpuMatch;         //!< qfpuMatch & Mask & scale values for opcode matches
    uint16_t           qfpuMask;
    uint8_t            qfpuScale;

    uint16_t           xuMatch;           //!< xuMatch & Mask value for opcode matches
    uint16_t           xuMask;

    uint8_t            sqrtScale;         //!< Punit FP square root scaling value for core (0,1,2,3) means 1,2,4,8
    uint8_t            divScale;          //!< Punit FP division scaling value for core    (0,1,2,3) means 1,2,4,8

    // Prereserved ctrs and signals for this core (any new eventsets should skip these)
    // Used for debug purposes.
    uint64_t           preRsvSignals[CONFIG_HWTHREADS_PER_CORE];     //!< Prereserved A2 Signals
    uint32_t           preRsvCtrs[CONFIG_HWTHREADS_PER_CORE];        //!< Prereserved UPC_P Counters.



    void IncrRef()
    {
        UPC_Lock(&lock);
        assert(refCount >= 0);
        refCount++;
        UPC_Unlock(&lock);
    }


    void DecrRef(unsigned hwThd);


    bool CtrGrpInUseByOthers(unsigned grp, unsigned curHwThd);


    // Match or reserve the Core's L1p Mode
    bool RsvL1pMode(UPC_EventSources_t chkL1pMode);


    // Attempt to reserve the counter group for use by the passed hwThd,
    // and increase the hw threads evtSet refCount if it's a new evtSet.
    // Return false when group already owned by a different thread.
    bool RsvCtrGrp(unsigned grp, unsigned hwThd, bool newEvtSet);

    // Release the affinity group settings by an evtSet.
    void RlsCtrGrp(unsigned hwThd);


    // using the passed allocMask, attempt to duplicate the reservations from the
    // source CoreReservations structure.
    int DuplicateReservations(unsigned allocMask, const CoreReservations & src);

    void Dump(unsigned indent, unsigned coreIdx);

    // reserve core counter groups indicated in mask by an individual event set
    // Return false when already attached.
    bool RsvAttachedCtrGrp(uint8_t attachMask);
    void RlsAttachedCtrGrp(uint8_t attachMask);


    UPC_EventDomain_t Context2Dmn(Bgpm_Context ctx) {
        UPC_EventDomain_t ret;
        switch (ctx) {
            case BGPM_CTX_KERNEL: ret = UPC_Domain_Kernel; break;
            case BGPM_CTX_USER:   ret = UPC_Domain_User; break;
            case BGPM_CTX_BOTH:   ret = UPC_Domain_Any; break;
            default:              ret = UPC_Domain_NotDef; break;
        }
        return ret;
    }

    bool RsvContext(Bgpm_Context reqCtx);
    bool AllowMixedContext() { return ((allocMask & Alloc_evtCtxMixed) != 0); }
    void SetMixedContext();
    bool RsvQfpuMatch(uint16_t reqMatch, uint16_t reqMask, uint8_t reqScale);
    bool RsvXuMatch(uint16_t reqMatch, uint16_t reqMask);
    bool RsvSqrtScale(uint8_t reqScale);
    bool RsvDivScale(uint8_t reqScale);
};



}

#endif
