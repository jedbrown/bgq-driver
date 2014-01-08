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

#define _UPCI_PUNIT_C_
#include "spi/include/upci/punit.h"
#include "spi/include/kernel/upci.h"
#include "spi/include/upci/testlib/upci_debug.h"


int Upci_Punit_AddEventAtIdx(Upci_Punit_t *pPunit, Upci_EventIds evtId, unsigned thdId, int evtIdx, int bkRefIdx,
                             uint8_t affGrpMask, Upci_Punit_EventH_t *phEvt)
{
    if(pPunit->numEvts >=  UPC_P_NUM_COUNTERS){
     printf(_AT_ "Returning from punit.c: Number of events in teh event set exceeds Maximum num of events\n ");
     return(UPCI_ECTRRESERVED);
    }

    assert(!(thdId & ~0x3));
    assert((evtIdx >= -1) && (evtIdx < UPC_P_NUM_COUNTERS));

    #if 0
    printf(_AT_ " thdId=%d, numEvts=%d, maxEvtIdx=%d, evtIdx=%d\n",
            thdId, pPunit->numEvts, pPunit->maxEvtIdx, evtIdx);
    #endif

    int rc = 0;

    if (evtIdx < 0) {
        evtIdx = pPunit->maxEvtIdx;
        evtIdx++;
    }
    Upci_PunitEventRec_t *curEvtRec = &(pPunit->evts[evtIdx]);

    rc = Upci_Punit_ReserveEvent(&(pPunit->rsv), evtId, curEvtRec, pPunit->unit, thdId, affGrpMask);
    if (!rc) {

        Upci_Punit_Cfg_AddEvent(&(pPunit->cfg), curEvtRec, bkRefIdx);

        Upci_Punit_EventH_t hEvt = Ucpi_InvEventHandle;
        hEvt.evtIdx = evtIdx;
        hEvt.ctr = curEvtRec->p_Ctr;
        hEvt.ctrGrp = curEvtRec->c_Cgrp;
        hEvt.valid = UPCI_VALID_HANDLE_VALUE;
        *phEvt = hEvt;
        //printf(_AT_ " hEvt=0x%016lx, evtIdx=%d, ctr=%d\n", *((uint64_t*)phEvt), hEvt.evtIdx, hEvt.ctr);

        pPunit->numEvts++;
        pPunit->maxEvtIdx = UPCI_MAX(evtIdx, pPunit->maxEvtIdx);
        //fprintf(stderr, "(D) "_AT_": numEvts=%d, maxIdx=%d\n", pPunit->numEvts, pPunit->maxEvtIdx);
    }
    else {
        *phEvt = Ucpi_InvEventHandle;
        curEvtRec->pEvent = NULL;
        //printf(_AT_ " hEvt=0x%016lx\n", *((uint64_t*)phEvt));
    }

    //printf(_AT_ " numEvts=%d, maxEvtIdx=%d\n", pPunit->numEvts, pPunit->maxEvtIdx); // <<<<<<<<<<<<<<<<

    return rc;
}



int Upci_Punit_Apply(Upci_Punit_t *pPunit)
{
    assert(pPunit);
    // numEvts and maxEvtIdx should be equivalent at apply time or there was
    // a problem in the assignments by the caller.  There should be no
    // holes left in the punit event list.
    //fprintf(stderr, "(D) "_AT_": numEvts=%d, maxIdx=%d\n", pPunit->numEvts, pPunit->maxEvtIdx);
    assert(pPunit->numEvts == (pPunit->maxEvtIdx+1));

    int rc = Kernel_Upci_Punit_Cfg_Apply( &(pPunit->cfg), pPunit->unit);
    //Upci_Punit_Dump(0, pPunit);
    //UPC_P_Dump_State_Indent(-1, 0);

    pPunit->status |= UPCI_UnitStatus_Applied;
    return rc;
}



int Upci_Punit_Attach(Upci_Punit_t *pPunit)
{
    assert(pPunit);
    // numEvts and maxEvtIdx should be equivalent at apply time or there was
    // a problem in the assignments by the caller.  There should be no
    // holes left in the punit event list.
    //fprintf(stderr, "(D) "_AT_": numEvts=%d, maxIdx=%d\n", pPunit->numEvts, pPunit->maxEvtIdx);
    assert(pPunit->numEvts == (pPunit->maxEvtIdx+1));

    int rc = Kernel_Upci_Punit_Cfg_Attach( &(pPunit->cfg), pPunit->unit);

    pPunit->status |= UPCI_UnitStatus_Applied;
    return rc;
}




int Upci_Punit_UnApply(Upci_Punit_t *pPunit)
{
    assert(pPunit);
    assert(pPunit->status & UPCI_UnitStatus_Applied);

    Upci_Punit_Stop(pPunit, UPCI_CTL_OVF); // UPCI_CTL_SHARED | UPCI_CTL_OVF);

    pPunit->status &= ~UPCI_UnitStatus_Applied;

    return 0;
}



void Upci_Punit_Reset_Counts(Upci_Punit_t *pPunit, uint32_t opt)
{
    assert(pPunit);
    // alg:
    //  stop upc_p counters
    //  reset upc_p counters
    //  wait 800 cycles for values in flight to propagate to sram
    //    worst case: curr val in flight, then reset occurs.
    //                takes cur value 800 cycles to update sram
    //                the reset upc_p ctrs are in flight another 800 cycles till sram update
    //                   but we don't need to wait for this.
    //  If sync counting have to wait 800 cycles for stop to occur before resetting, because
    //    the stop is staggered depending on the core and current ring state.
    //  reset sram counters (write zeros)
    //    upc_p reset value could arrive before or after sram write of zeros - don't care,
    //    end result is the same - whole sram counter is cleared.
    //  start upc_p counters
    //    ok for start to occur before upc_p reset value has arrived at sram.

    // create mask of counters to reset
    Upci_Punit_Cfg_t *cfg = &(pPunit->cfg);
    uint32_t unit = pPunit->unit;
    upc_p_mmio_t *const upc_p = UPC_P_Addr(unit);

    // get current run state of ctrs
    uint64_t ctrMask = cfg->thdCtrMask;
    if (opt & UPCI_CTL_OVF) { ctrMask |= cfg->ovfCtrMask; }
    uint64_t runMask = upc_p->control & ctrMask;

    // stop and reset upc_p counters.
    UpciBool_t syncRunning = UpciFalse;
    if (pPunit->mode.ctrMode == UPC_CM_SYNC) {
        syncRunning = UPC_C_Stop_Sync_Counting();
    }
    Upci_Punit_Reset_Stopped_Counters(pPunit, opt);

    // restart if running
    if (syncRunning) UPC_C_Start_Sync_Counting();
    if (runMask) upc_p->control_w1s = runMask;

    mbar();
}



void Upci_Punit_Reset_Stopped_Counters(Upci_Punit_t *pPunit, uint32_t opt)
{
    assert(pPunit);
    // alg:
    //  reset sram counters (write zeros)
    //    upc_p reset value could arrive before or after sram write of zeros - don't care,
    //    end result is the same - whole sram counter is cleared.

    // create mask of counters to reset
    uint64_t ctrMask = 0;
    uint64_t ovfMask = 0;
    Upci_Punit_Cfg_t *cfg = &(pPunit->cfg);
    ctrMask |= cfg->thdCtrMask;
    if (opt & UPCI_CTL_OVF) { ovfMask |= cfg->ovfCtrMask; }

    uint32_t unit = pPunit->unit;
    uint32_t cCtrGrp = pPunit->cfg.cCtrGrp;
    upc_p_mmio_t *const upc_p = UPC_P_Addr(unit);

    // stop and reset upc_p counters.
    if (pPunit->mode.ctrMode == UPC_CM_SYNC) {
        upc_p->control_w1s = (ctrMask << 32);
    }
    else {
        upc_p->control_w1c = ctrMask | ovfMask;
        upc_p->control_w1s = (ctrMask << 32);  // reset counters

        // reset back to threshold the ovf counters
        if (ovfMask) {
            uint32_t oCtrMask = ovfMask;
            while (oCtrMask != 0) {
                uint32_t ctr = upc_cntlz32(oCtrMask);
                oCtrMask &= ~MASK32_bit(ctr);    // mask off counter bit.
                upc_p->counter[ctr] = UPC_P__COUNTER__THRESHOLD_COUNT_set(cfg->ctrOvfThres[ctr]); // includes armed bit as necessary
            }
        }
    }
    mbar();

    if ((pPunit->mode.ctrMode != UPC_CM_DISCON) && (pPunit->mode.upcMode != UPC_TRACE_MODE)) {

        // write zeros to sram
        // \todo might try a modification to use the accumulation engine to reset multiple counters
        //       (make source of accum the last 16 counter entry in SRAM and already initialized to 0's)
        //       and check when it is faster both based on the number of counters to reset and
        //       based on reset collisions between threads.
        //       But, that wouldn't help for counters with thresholds, they have to be dealt with one at a time.
        uint32_t resetMask = (uint32_t)ctrMask;
        while (resetMask != 0) {
            uint32_t ctr = upc_cntlz32(resetMask);
            resetMask &= ~MASK32_bit(ctr);    // mask off counter bit.
            upc_c->data24.grp[cCtrGrp].counter[ctr] = 0;
            cfg->ctrOvfAccum[ctr] = 0;  // make sure accum vals are clear (protect against any issues)
            cfg->ctrSS[ctr] = 0;
        }

        // reset back to threshold the ovf counters
        if (ovfMask) {
            uint32_t oCtrMask = ovfMask;
            while (oCtrMask != 0) {
                uint32_t ctr = upc_cntlz32(oCtrMask);
                oCtrMask &= ~MASK32_bit(ctr);    // mask off counter bit.

                upc_c->data24.grp[cCtrGrp].counter[ctr] = cfg->ctrOvfThres[ctr];
                cfg->ctrSS[ctr] = cfg->ctrOvfThres[ctr];
                cfg->ctrOvfAccum[ctr] = 0;
            }
        }
    }
    mbar();
}



void Upci_Punit_Reset_SnapShots(Upci_Punit_t *pPunit)
{
    Upci_Punit_Cfg_t *cfg = &(pPunit->cfg);
    uint32_t resetMask = cfg->thdCtrMask;
    uint32_t ovfMask   = cfg->ovfCtrMask;

    while (resetMask != 0) {
        uint32_t ctr = upc_cntlz32(resetMask);
        resetMask &= ~MASK32_bit(ctr);    // mask off counter bit.
        pPunit->cfg.ctrSS[ctr] = 0;
    }

    while (ovfMask != 0) {
        uint32_t ctr = upc_cntlz32(ovfMask);
        ovfMask &= ~MASK32_bit(ctr);    // mask off counter bit.
        pPunit->cfg.ctrSS[ctr] = cfg->ctrOvfThres[ctr];
        cfg->ctrOvfAccum[ctr] = 0;
    }
}



void Upci_Punit_Event_Write(Upci_Punit_t *pPunit, Upci_Punit_EventH_t hEvt, uint64_t value)
{
    assert(pPunit);
    assert(hEvt.valid);

    upc_p_mmio_t *const upc_p = UPC_P_Addr(pPunit->unit);
    uint64_t ctlMask = upc_p->control & MASK32_bit(hEvt.ctr);  // get ctr run state
    upc_p->control_w1c = ctlMask;
    upc_p->counter[hEvt.ctr] = UPC_P__COUNTER__COUNT_set(value);
    mbar();
    Upci_Delay(800);
    upc_c->data24.grp[hEvt.ctrGrp].counter[hEvt.ctr] = value;
    upc_p->control_w1s = ctlMask;  // restore ctr run state
    mbar();
}



void Upci_Punit_Event_WriteIdx(Upci_Punit_t *pPunit, unsigned idx, uint64_t value)
{
    assert(pPunit);
    assert((idx >= 0) && (idx < pPunit->numEvts));

    Upci_PunitEventRec_t *pEvtRec = &(pPunit->evts[idx]);
    upc_p_mmio_t *const upc_p = UPC_P_Addr(pPunit->unit);

    uint64_t ctlMask = upc_p->control & MASK32_bit(pEvtRec->p_Ctr);  // get ctr run state
    upc_p->control_w1c = ctlMask;
    upc_p->counter[pEvtRec->p_Ctr] = UPC_P__COUNTER__COUNT_set(value);
    mbar();
    Upci_Delay(800);
    upc_c->data24.grp[pEvtRec->c_Cgrp].counter[pEvtRec->c_Ctr] = value;
    upc_p->control_w1s = ctlMask;  // restore ctr run state
    mbar();
}



void Upci_Punit_Event_WriteSnapShot(Upci_Punit_t *pPunit, unsigned idx, uint64_t value)
{
    assert(pPunit);
    assert((idx >= 0) && (idx < pPunit->numEvts));

    Upci_PunitEventRec_t *pEvtRec = &(pPunit->evts[idx]);
    pPunit->cfg.ctrSS[pEvtRec->p_Ctr] = value;
}




Upci_Ctr_CtlMask_t Upci_Punit_GetLLCtlMask(Upci_Punit_t *pPunit, uint32_t opt)
{
    assert(pPunit);

    // uint64_t ctlMask = 0;
    Upci_Punit_Cfg_t *cfg = &(pPunit->cfg);
    //if (opt & UPCI_CTL_SHARED) { ctlMask |= cfg->sharedCtrMask; }
    uint64_t ctlMask = cfg->thdCtrMask;
    if (opt & UPCI_CTL_OVF) { ctlMask |= cfg->ovfCtrMask; }
    return ctlMask;
}



int Upci_Punit_Event_SetThreshold(Upci_Punit_t *pPunit, unsigned idx, uint64_t threshold)
{
    assert(pPunit);

    int rc = 0;
    if (UNLIKELY(idx >= pPunit->numEvts)) {
        rc = UPCI_EINV_EVT_IDX;
    }
    else {
        unsigned ctr = pPunit->evts[idx].p_Ctr;
        pPunit->cfg.ovfCtrMask |= MASK32_bit(ctr);
        pPunit->cfg.ctrOvfThres[ctr] = threshold;
        pPunit->cfg.ctrSS[ctr] = threshold;

        // remove ctr from threaded ctr mask
        pPunit->cfg.thdCtrMask &= ~MASK32_bit(ctr);

        // Enable little overflow in ctr config
        pPunit->cfg.ctrCfg[ctr] |= UPC_P__COUNTER_CFG__INT_ENABLE_set(1UL);
    }
    return rc;
}



int Upci_Punit_AccumResetThresholds(Upci_Punit_t *pPunit, uint64_t intMask)
{
    assert(pPunit);
    Upci_Punit_Cfg_t *pCfg = &(pPunit->cfg);
    unsigned c_Cgrp = pCfg->cCtrGrp;  // all events are in same upc_c counter group (i.e. don't need to look at individual event assignment)

    uint32_t ovfMask = pCfg->ovfCtrMask;
    uint32_t intMask32 = (uint32_t)intMask;
    intMask32 &= ovfMask;  // only interested in this punit's ovf counters
    int rc = (intMask32 != 0);

    while (intMask32) {
        int intCtr = upc_cntlz32(intMask32);
        intMask32 &= ~MASK32_bit(intCtr);

        uint64_t threshold = pCfg->ctrOvfThres[intCtr];
        uint64_t delta = (~(threshold)+1);  // calc period
        delta += upc_c->data24.grp[c_Cgrp].counter[intCtr];

        pCfg->ctrOvfAccum[intCtr] += delta;  // accumlate delta

        //fprintf(stderr, _AT_ " intMask32=0x%08x, intCtr=%02d, delta=%10ld, accum=%14ld", intMask32, intCtr, delta, pCfg->ctrAccum[intCtr]);

        // reset counter back to threshold
        // safe to write upc_p and upc_c without waiting because counters are frozen when
        // this routine is called. upc_c counter low 13 bits "might" get set back momentarily if upc_p count is
        // already in flight - but will be updated correct in next 800 cycles.
        upc_p_local->counter[intCtr] = UPC_P__COUNTER__THRESHOLD_COUNT_set(threshold);
        upc_c->data24.grp[c_Cgrp].counter[intCtr] = threshold;
        pCfg->ctrSS[intCtr] = threshold;
    }
    return rc;
}



int Upci_Punit_GetOvfBackRefs(Upci_Punit_t *pPunit, uint64_t intMask, unsigned *pIndicies, unsigned *pLen)
{
    //assert(pPunit);
    uint32_t ovfMask = pPunit->cfg.ovfCtrMask;
    uint32_t intMask32 = (uint32_t)intMask;
    intMask32 &= ovfMask;  // only interested in this punit's ovf counters
    unsigned outIdx = 0;
    unsigned outLen = *pLen;
    while ((intMask32) && (outIdx < outLen)) {
        int intCtr = upc_cntlz32(intMask32);
        if (intCtr < UPC_P_NUM_COUNTERS) {
            pIndicies[outIdx++] = pPunit->cfg.ctrBkRefIdx[intCtr];
        }
        intMask32 &= ~MASK32_bit(intCtr);
    }
    *pLen = outIdx;
    if (intMask32) return -1;
    return 0;
}

