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

#include "CoreReservations.h"
#include "Debug.h"
#include "Lock.h"
#include "EnvVars.h"


//! \addtogroup bgpm_api_impl
//@{
/**
 * \file
 *
 * \brief track the counting hardware resources which are common for all threads on a core
 *
 */
//@}

using namespace bgpm;

extern EnvVars  envVars;


void CoreReservations::DecrRef(unsigned hwThd)
{
    BGPM_EXEMPT_L2;

    UPC_Lock(&lock);

    // If all uses expire, then free all allocated resources.
    assert(refCount > 0);
    if (--refCount == 0) {
        allocMask = Alloc_Clear;
        int i;
        for (i=0; i<CONFIG_HWTHREADS_PER_CORE; i++) {
            //fprintf(stderr, _AT_ " ctrGrpMask[%d]=0x%02x, ctrGrpRefCount[%d]=%d\n", i, ctrGrpMask[i], i, ctrGrpRefCount[i]); // <<<<<<<<<<<<
            assert(ctrGrpRefCount[i] == 0);  // \todo Let's double check for now - should be 0 by this point.
            //ctrGrpRefCount[i] = 0;
            ctrGrpMask[i] = 0;
        }
    }
    UPC_Unlock(&lock);
}



bool CoreReservations::CtrGrpInUseByOthers(unsigned grp, unsigned curHwThd)
{
    BGPM_EXEMPT_DATA(fprintf(stderr, _AT_ " this=0x%016lx, ctrGrpMasks[0,1,2,3]=0x%02x,0x%02x,0x%02x,0x%02x\n",
                                            (uint64_t)this, ctrGrpMask[0], ctrGrpMask[1], ctrGrpMask[2], ctrGrpMask[3]));
    assert(grp < CONFIG_HWTHREADS_PER_CORE);
    uint8_t grpMask = CTRGRP_MASKBIT(grp);
    uint8_t mask = 0;
    for (unsigned otherHwThd=0; otherHwThd<CONFIG_HWTHREADS_PER_CORE; ++otherHwThd) {
        //BGPM_EXEMPT_DATA(fprintf(stderr, _AT_ " grp=%d, grpMask=0x%02x, curHwThd=%d, ctrGrpMask[%d]=0x%02x, mask=0x%02x\n",
        //        grp,grpMask,curHwThd,otherHwThd,ctrGrpMask[otherHwThd],mask));
        if (otherHwThd != curHwThd) {
            mask |= ctrGrpMask[otherHwThd] & grpMask;
        }
    }
    //BGPM_EXEMPT_DATA(fprintf(stderr, _AT_ " grp=%d, curHwThd=%d, mask=0x%02x\n", grp,curHwThd,mask));
    return (mask != 0);
}



bool CoreReservations::RsvCtrGrp(unsigned grp, unsigned hwThd, bool newEvtSet)
{
    BGPM_EXEMPT_L2;

    assert(grp < CONFIG_HWTHREADS_PER_CORE);
    assert(hwThd < CONFIG_HWTHREADS_PER_CORE);
    assert(ctrGrpRefCount[hwThd] >= 0);

    // fprintf(stderr, _AT_ " grp=%d, hwThd=%d, newEvtSet=%d\n", grp, hwThd, newEvtSet); // <<<<<<<<<<

    if (UNLIKELY(CtrGrpInUseByOthers(grp, hwThd))) {
        return false;
    }

    UPC_Lock(&lock);
    if (UNLIKELY(CtrGrpInUseByOthers(grp, hwThd))) {  // Still not in use?
        UPC_Unlock(&lock);
        return false;
    }

    ctrGrpMask[hwThd] |= CTRGRP_MASKBIT(grp);
    if (newEvtSet) {
        ctrGrpRefCount[hwThd]++;
    }
    BGPM_EXEMPT_DATA(fprintf(stderr, _AT_ " grp=%d, hwThd=%d, newEvtSet=%d, ctrGrpMask=0x%02x\n", grp,hwThd,newEvtSet,ctrGrpMask[hwThd]));

    UPC_Unlock(&lock);

    return true;
}



void CoreReservations::RlsCtrGrp(unsigned hwThd) {
    BGPM_EXEMPT_L2;

    UPC_Lock(&lock);
    assert(ctrGrpRefCount[hwThd] > 0);
    if (ctrGrpRefCount[hwThd] > 0) {
        ctrGrpRefCount[hwThd]--;
        if (ctrGrpRefCount[hwThd] == 0) {
            ctrGrpMask[hwThd] = 0;
        }
    }
    UPC_Unlock(&lock);
}



int CoreReservations::DuplicateReservations(unsigned allocMask, const CoreReservations & src)
{
    BGPM_EXEMPT_L2;

    int rc = 0;
    if (allocMask) {
        if (allocMask & Alloc_l1pMode) {
            if (!RsvL1pMode(src.l1pMode)) return BGPM_EL1PMODERESERVED;
        }
        if (allocMask & Alloc_qfpuMatch) {
            if (!RsvQfpuMatch(src.qfpuMatch, src.qfpuMask, src.qfpuScale)) return BGPM_EQFPU_RSV_MISMATCH;
        }
        if (allocMask & Alloc_xuMatch) {
            if (!RsvXuMatch(src.xuMatch, src.xuMask)) return BGPM_EXU_RSV_MISMATCH;
        }
        if (allocMask & Alloc_sqrtScale) {
            if (!RsvSqrtScale(src.sqrtScale)) return BGPM_ESQRT_SCALE_MISMATCH;
        }
        if (allocMask & Alloc_divScale) {
            if (!RsvDivScale(src.divScale)) return BGPM_EDIV_SCALE_MISMATCH;
        }
        if (allocMask & (Alloc_evtCtx | Alloc_evtCtxBoth)) {
            if (!RsvContext(src.evtCtx)) return BGPM_ECTX_MISMATCH;
        }
        if (allocMask & Alloc_evtCtxMixed) {
            SetMixedContext();
        }
    }
    return rc;
}


bool CoreReservations::RsvL1pMode(UPC_EventSources_t chkL1pMode)
{
    BGPM_EXEMPT_L2;

    bool rc = true;
    //fprintf(stderr, _AT_ " chkL1pMode=%d, allocMask=0x%02x, l1pMode=%d, &allocMask=0x%016lx\n", chkL1pMode, (unsigned)allocMask, l1pMode, (uint64_t)&allocMask);
    if ((chkL1pMode >= UPC_ES_L1P_SWITCH) && (chkL1pMode <= UPC_ES_L1P_LIST)) {
        if (!(allocMask & Alloc_l1pMode)) {
            UPC_Lock(&lock);
            if (!(allocMask & Alloc_l1pMode)) {
                allocMask = (CoreDepBits)(((unsigned)allocMask) | ((unsigned)Alloc_l1pMode));
                l1pMode = chkL1pMode;
            }
            UPC_Unlock(&lock);
        }
        if (chkL1pMode != l1pMode) {
            rc = false;
        }
    }
    //fprintf(stderr, _AT_ " chkL1pMode=%d, allocMask=0x%02x, l1pMode=%d\n", chkL1pMode, (unsigned)allocMask, l1pMode);
    return rc;
}



bool CoreReservations::RsvAttachedCtrGrp(uint8_t attachMask)
{
    bool rc = false;
    if ((puAttachedMask & attachMask) == 0) {  // threads still free
        UPC_Lock(&lock);
        if ((puAttachedMask & attachMask) == 0) {
            puAttachedMask |= attachMask;
            rc = true;
        }
        UPC_Unlock(&lock);
    }
    return rc;
}



void CoreReservations::RlsAttachedCtrGrp(uint8_t attachMask)
{
    UPC_Lock(&lock);
    //fprintf(stderr, _AT_ " puAttachedMask=0x%02x, attachMask=0x%02x\n",
    //        puAttachedMask, attachMask);
    // check that evtset that caller was only owner of attached threads.
    assert((puAttachedMask & attachMask) == attachMask);
    puAttachedMask &= ~attachMask;
    UPC_Unlock(&lock);
}



bool CoreReservations::RsvContext(Bgpm_Context reqCtx)
{
    bool ret = true;
    if (reqCtx > BGPM_CTX_DEFAULT) {
        if (!(allocMask & Alloc_evtCtx)) {
            Lock guard(&lock);
            if (!(allocMask & Alloc_evtCtx)) {

                // validate requested context against possible environment value
                // Set context to winner of environment or request.
                Bgpm_Context defCtx = envVars.GetDefContext(reqCtx);
                evtCtx = defCtx;
                allocMask |= Alloc_evtCtx;
                if (evtCtx == BGPM_CTX_BOTH) {
                    allocMask |= Alloc_evtCtxBoth;
                }
            }
        }

        if (allocMask & Alloc_evtCtx) {
            if (evtCtx == reqCtx) ret = true;
            else ret = false;
        }
    }
    return ret;
}



void CoreReservations::SetMixedContext()
{
    Lock guard(&lock);
    allocMask |= Alloc_evtCtxMixed;
}



bool CoreReservations::RsvQfpuMatch(uint16_t reqMatch, uint16_t reqMask, uint8_t reqScale)
{
    if (!(allocMask & Alloc_qfpuMatch)) {
        Lock guard(&lock);
        if (!(allocMask & Alloc_qfpuMatch)) {
            qfpuMatch = reqMatch;
            qfpuMask = reqMask;
            qfpuScale = reqScale;
            allocMask |= Alloc_qfpuMatch;
        }
    }

    bool ret = false;
    if ((reqMatch == qfpuMatch) && (reqMask == qfpuMask) && (reqScale == qfpuScale)) ret = true;
    return ret;
}



bool CoreReservations::RsvXuMatch(uint16_t reqMatch, uint16_t reqMask)
{
    if (!(allocMask & Alloc_xuMatch)) {
        Lock guard(&lock);
        if (!(allocMask & Alloc_xuMatch)) {
            xuMatch = reqMatch;
            xuMask = reqMask;
            allocMask |= Alloc_xuMatch;
        }
    }

    bool ret = false;
    if ((reqMatch == xuMatch) && (reqMask == xuMask)) ret = true;
    return ret;
}



bool CoreReservations::RsvSqrtScale(uint8_t reqScale)
{
    if (!(allocMask & Alloc_sqrtScale)) {
        Lock guard(&lock);
        if (!(allocMask & Alloc_sqrtScale)) {
            sqrtScale = reqScale;
            allocMask |= Alloc_sqrtScale;
        }
    }

    bool ret = false;
    if (reqScale == sqrtScale) ret = true;
    return ret;
}



bool CoreReservations::RsvDivScale(uint8_t reqScale)
{
    if (!(allocMask & Alloc_divScale)) {
        Lock guard(&lock);
        if (!(allocMask & Alloc_divScale)) {
            divScale = reqScale;
            allocMask |= Alloc_divScale;
        }
    }

    bool ret = false;
    if (reqScale == divScale) ret = true;
    return ret;
}



