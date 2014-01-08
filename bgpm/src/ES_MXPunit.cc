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

#include <vector>

#include "globals.h"
#include "ES_MXPunit.h"
#include "spi/include/upci/testlib/upci_debug.h"

using namespace bgpm;



ES_MXPunit::ES_MXPunit(unsigned targHwThdId, uint64_t period, ushort iNormalize, int iMaxEvents)
: ES_SWPunit(targHwThdId), muxCycleEvtIdx(), muxIntStat(), muxApplies(),
  maxMuxEvtsPerPunit(0),
  normalizeEvents(iNormalize), numMuxSwitches(0), muxPeriod(period), muxSwitchTransCount(0)
{
    BGPM_TRACE_L2;
    if (iMaxEvents < 7) maxMuxEvtsPerPunit = 5;
    else if (iMaxEvents < 13) maxMuxEvtsPerPunit = 11;
    else maxMuxEvtsPerPunit = 23;
}



ES_MXPunit::~ES_MXPunit()
{
    BGPM_TRACE_L2;
};



EvtSet::ESTypes ES_MXPunit::ESType() { return ESType_SWPunit; }
const char *ES_MXPunit::ESTypeLabel() { return "Punit MUX Thread"; }



EvtSet *ES_MXPunit::Clone(uint64_t target, unsigned *addEvents, unsigned addNum)
{
    BGPM_TRACE_L2;
    if (UNLIKELY(target >= CONFIG_MAX_HWTHREADS)) {
        lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
        return NULL;
    }

    ES_SWPunit*p = new ES_MXPunit((unsigned)target, muxPeriod, normalizeEvents, maxMuxEvtsPerPunit);
    if (p->GetCrtRC() >= 0) {
        // clone all but the cycle counts used for tracking the time each mux is active
        // Muxable AddEventList will reassign them.
        int newsize = UPCI_MAX(0, evtRecs.size() - puList.size());
        evtRecs.resize(newsize);
        p->MergeEventCfg(*this, (unsigned)target, addEvents, addNum);
    }

    return p;
}



int ES_MXPunit::NumEvents() const { return (int)evtRecs.size() - (int)puList.size(); }



int ES_MXPunit::Apply(int hEvtSet) {
    BGPM_TRACE_L2;

    // Check to see if an event set is already applied to this hwthread.
    // An Apply() operation won't attach exclusively, but shouldn't apply if
    // a thread has attached.
    // Pass CONFIG_HWTHREADS_PER_CORE so that CtrGrpInUseByOthers thinks the current
    // thread is an "other", and actually checks all hw threads
    // including the current thread.
    if (!coreRsv.CtrGrpInUseByOthers(targHwThd, CONFIG_HWTHREADS_PER_CORE)) {
        return lastErr.PrintOrExit(BGPM_WALREADY_ATTACHED, BGPM_ERRLOC);
    }

    // set the global punit event set so overflow's and mux switches can process correct event set.
    swModeAppliedPuEvtSet = hEvtSet;
    assert(this == (procEvtSets[hEvtSet]));

    // Reset the counts 1st to prevent issues with residual counts when subsequent applies of an event set.
    // it also applies the 1st mux group
    int rc = Reset();
    return rc;
}



int ES_MXPunit::Stop() {
    BGPM_TRACE_L2;

    Upci_Punit_t *pPunit = puList[activePunitIdx];

#ifndef BGPM_FAST
    if (UNLIKELY(!(pPunit->status & UPCI_UnitStatus_Applied))) {
        return lastErr.PrintOrExit(BGPM_ENOT_APPLIED, BGPM_ERRLOC);
    }
#endif
    int rc = (pPunit->status & UPCI_UnitStatus_Running) ? 0 : BGPM_WALREADY_STOPPED;
    Upci_Punit_Stop_Indep(pPunit, UPCI_CTL_DELAY | UPCI_CTL_OVF);
    mbar();
    if (pPunit != puList[activePunitIdx]) {
        Upci_Punit_Stop_Indep(puList[activePunitIdx], UPCI_CTL_DELAY | UPCI_CTL_OVF);
    }
    running = false;

    BGPM_TRACE_DATA_L2( \
        fprintf(stderr, "%s" _AT_ " upc_p->control=0x%016lx\n", IND_STRG, upc_p_local->control) \
    );

    return rc;
}



int ES_MXPunit::AddEventList(unsigned *evtIds, unsigned num, EvtSet* & pNewEvtSet)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }

    // If there is already something in this event set, then build a new one
    // including the additional events.
    if (evtRecs.size() > 0) {
        pNewEvtSet = Clone(GetTargThdId(), evtIds, num);
        if (pNewEvtSet == NULL) {
            return lastErr.PrintOrExit(BGPM_ENOMEM, BGPM_ERRLOC);
        }
        int rc = pNewEvtSet->GetCrtRC();
        if (rc >= 0) {
            return rc;
        }
        else {
            delete pNewEvtSet;
            pNewEvtSet = NULL;
        }
        return rc;
    }



    // Otherwise, AddEventList always needs to start with a fresh list of events to add and
    // a fresh Punit
    pNewEvtSet = NULL;

    // toss any existing punits - in case of prior failure with this event set.
    int idx;
    for (idx=puList.size()-1; idx>=0; --idx) {
        if (puList[idx]) delete puList[idx];
    }
    puList.resize(0);
    activePunitIdx = 0;

    // toss any existing (possibly empty) events in case of prior failure.
    // and pre reserve memory for the list of events
    evtRecs.resize(0);
    evtRecs.resize(num);

    std::vector<unsigned> addOrder;
    GetBestAddOrder(evtIds, num, addOrder);
    assert(addOrder.size() == num);

    // make sure that current threads affinity counter group is available.
    if (UNLIKELY(!coreRsv.RsvCtrGrp(targHwThd, targHwThd, false))) {
        evtRecs.resize(0);  // make sure empty on failure (in case someone tries to deref events)
        return lastErr.PrintOrExit(BGPM_ETHD_CTRS_RSV, BGPM_ERRLOC);
    }
    //  if max is creater than 6 - try sister affinity group.
    if (maxMuxEvtsPerPunit > (UPC_P_NUM_COUNTERS/4)) {
        if (!coreRsv.RsvCtrGrp(Upci_Rsv_SiblingThread(targHwThd), targHwThd, false)) {
            evtRecs.resize(0);  // make sure empty on failure (in case someone tries to deref events)
            return lastErr.PrintOrExit(BGPM_ETHD_CTRS_RSV, BGPM_ERRLOC);
        }
    }
    //  if max is creater than 12 - try all affinity groups
    if (maxMuxEvtsPerPunit > (UPC_P_NUM_COUNTERS/2)) {
        bool allGrpsRsved = (coreRsv.RsvCtrGrp(0, targHwThd, false) &&
                coreRsv.RsvCtrGrp(1, targHwThd, false) &&
                coreRsv.RsvCtrGrp(2, targHwThd, false) &&
                coreRsv.RsvCtrGrp(3, targHwThd, false));
        if (!allGrpsRsved) {
            evtRecs.resize(0);  // make sure empty on failure (in case someone tries to deref events)
            return lastErr.PrintOrExit(BGPM_ETHD_CTRS_RSV, BGPM_ERRLOC);
        }
    }


    // Add events until done or failure.
    unsigned addIdx = 0;
    int rc = 0;
    while ((rc == 0) && (addIdx<addOrder.size())) {
        int evtIdx = addOrder[addIdx];
        unsigned puIdx = 0;
        bool addDone = false;
        Upci_Punit_t *pCurPunit = NULL;

        while (!addDone) {

            // Allocate a new punit if needed
            if (puIdx >= puList.size()) {
                pCurPunit = AllocateFreshPunit();
                puList.push_back(pCurPunit);
            }
            pCurPunit = puList[puIdx];

            BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " pCurPunit=0x%016lx, evtIdx=%d, puIdx=%d, puList.size()=%ld, maxMuxEventsPerPunit=%d\n",
                    IND_STRG, (uint64_t)pCurPunit, evtIdx, puIdx, puList.size(), maxMuxEvtsPerPunit));
            assert(pCurPunit);

            if (pCurPunit->numEvts < maxMuxEvtsPerPunit) {
                rc = ReserveEvent(evtIdx, evtIds[evtIdx], pCurPunit, pCurPunit->numEvts);
                BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " rc=%d\n", IND_STRG, rc));
            }
            else {
                BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " reached max muxable events in punit=%d; rc=%d\n", IND_STRG, maxMuxEvtsPerPunit, rc));
                rc = BGPM_ECTRRESERVED;
            }

            // If this event requires all counter groups be reserved,
            // let's setup to retry with all affinity groups
            // It's needed to allow these particular core-wide events to be allocated.
            // This is the only exception to the 6 counter limit for Muxing.
            if (UNLIKELY(rc == BGPM_ECSEVT_REQALLTHDS)) {
                bool allGrpsRsved = (coreRsv.RsvCtrGrp(0, targHwThd, false) &&
                        coreRsv.RsvCtrGrp(1, targHwThd, false) &&
                        coreRsv.RsvCtrGrp(2, targHwThd, false) &&
                        coreRsv.RsvCtrGrp(3, targHwThd, false));
                maxMuxEvtsPerPunit = UPC_P_NUM_COUNTERS-1;  // all counters available now
                if (!allGrpsRsved) {
                    DumpEventsInError(pCurPunit, rc, (Upci_EventIds)evtIds[evtIdx]);
                    evtRecs.resize(0);  // make sure empty on failure.
                    return lastErr.PrintOrExit(rc, BGPM_ERRLOC);
                }
            }
            // if the counters or signals are reserved and this is a new punit (no event yet)
            // then we have failed, otherwise let's set to retry with a new punit.
            else if ((pCurPunit->numEvts > 0) && ((rc == BGPM_ECTRRESERVED) || (rc == BGPM_ESIGRESERVED))) {
                puIdx++;
            }
            else if (rc < 0) {
                DumpEventsInError(pCurPunit, rc, (Upci_EventIds)evtIds[evtIdx]);
                evtRecs.resize(0);  // make sure empty on failure.
                return lastErr.PrintOrExit(rc, BGPM_ERRLOC);
            }
            else {
                addDone = true;
            }
        }
        addIdx++;
    }

    // Add reserved cycle count event to each punit
    muxCycleEvtIdx.resize(puList.size());
    muxIntStat.resize(puList.size());
    muxApplies.resize(puList.size());

    for (unsigned puIdx=0; puIdx<puList.size(); ++puIdx) {
        int evtIdx = evtRecs.size();
        evtRecs.resize(evtIdx+1);
        Upci_Punit_t *pCurPunit = puList[puIdx];
        muxCycleEvtIdx[puIdx] = evtIdx;
        muxIntStat[puIdx] = 0;
        muxApplies[puIdx] = 0;

        rc = ReserveEvent(evtIdx, PEVT_CYCLES, pCurPunit, pCurPunit->numEvts);
        BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " rc=%d\n", IND_STRG, rc));
        if (rc) {
            DumpEventsInError(puList[puIdx], rc, PEVT_CYCLES);
            evtRecs.resize(0);  // make sure empty on failure.
            lastErr.PrintOrExit(rc, BGPM_ERRLOC);
            assert(0);   // Adding a cycle event should never fail - or there is a programming error!
            return rc;
        }

        // setup cycle event to count cycles mux group is active, or cause overflows.
        if (muxPeriod > 0) {
            uint64_t threshold = UPCI_PERIOD2THRES(muxPeriod);
            Upci_Punit_Event_SetThreshold(pCurPunit, evtRecs[evtIdx].puEvtIdx, threshold);
            muxIntStat[puIdx] = Upci_Punit_GetIntMask(pCurPunit, evtRecs[evtIdx].puEvtIdx);
            Upci_Punit_EnablePMInt(pCurPunit);
        }
    }

    for (unsigned i=0; i<puList.size(); ++i) {
        Upci_Punit_t *pCurPunit = puList[i];
        BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " puList[%d]=0x%016lx\n", IND_STRG, i, (uint64_t)pCurPunit));
        SetEvtSetPunitInts(pCurPunit);  // activate PM interrupts
    }

    return 0;
}



int ES_MXPunit::ResetStart() {
    BGPM_TRACE_L2;
    Reset();

    Upci_Punit_t *pPunit = puList[activePunitIdx];
    Upci_Punit_Start_Indep(pPunit, UPCI_CTL_OVF); // UPCI_CTL_SHARED |
    running = true;
    return 0;
}


int ES_MXPunit::Reset() {
    BGPM_TRACE_L2;

#ifndef BGPM_FAST
    if (UNLIKELY(running)) {
        return lastErr.PrintOrExit(BGPM_EUPDATE_DISALLOWED, BGPM_ERRLOC);
    }
    if (UNLIKELY(this != (procEvtSets[swModeAppliedPuEvtSet]))) {
        return lastErr.PrintOrExit(BGPM_ENOT_APPLIED, BGPM_ERRLOC);
    }
#endif

    unsigned puIdx;
    for (puIdx=0; puIdx<puList.size(); ++puIdx) {
        if (puIdx == activePunitIdx) {
            Upci_Punit_t *pPunit = puList[activePunitIdx];
            Upci_Punit_Reset_Counts(pPunit, UPCI_CTL_OVF);
        }
        else {
            Upci_Punit_t *pPunit = puList[puIdx];
            Upci_Punit_Reset_SnapShots(pPunit);
        }
        muxApplies[puIdx] = 0;
    }

    activePunitIdx = 0;
    numMuxSwitches = 0;
    muxApplies[activePunitIdx] = 1;
    int rc = ES_SWPunit::Apply(swModeAppliedPuEvtSet);
    return rc;
}




int ES_MXPunit::ReadEvent(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;

#ifndef BGPM_FAST

    //assert(pVal);
    if (UNLIKELY(idx >= (evtRecs.size()-puList.size()))) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
#endif

    return ReadAnyEvent(idx, pVal);
}


int ES_MXPunit::ReadAnyEvent(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;

    Upci_Punit_t *pPunit = evtRecs[idx].pPunit;

    // Need to check muxSwitchTransCount to insure we don't attempt to read hardware counts while a switch is in
    // transition.  See SwitchMuxGrp() for a detailed explanation of how this works.
    unsigned curPunitIdx = activePunitIdx;  // local copy of active punit index
    uint64_t curTransitionCount = muxSwitchTransCount;
    uint64_t value = 0;
    curPunitIdx = (volatile unsigned)activePunitIdx;
    mbar();  // make sure these memory accesses have occurred before the next steps.

    // If mux grp is not in transition, and the current punit is applied, let's read from hardware.
    if ((curTransitionCount%2 == 0) && (pPunit == puList[curPunitIdx])) {
        value = Upci_Punit_Event_ReadIdx(pPunit, evtRecs[idx].puEvtIdx);
        //fprintf(stderr, "%s" _AT_ " curTransitionCount=%10d, curPunitIdx=%02d, evtIdx=%02d, puEvtIdx=%02d, value=0x%016lx\n",
        //       IND_STRG, curTransitionCount, curPunitIdx, idx, evtRecs[idx].puEvtIdx, value);
    }
    else {
        value = Upci_Punit_Event_ReadSnapShot(pPunit, evtRecs[idx].puEvtIdx);
        //fprintf(stderr, "%s" _AT_ " curTransitionCount=%10d, curPunitIdx=%02d, evtIdx=%02d, puEvtIdx=%02d, value=0x%016lx\n",
        //        IND_STRG, curTransitionCount, curPunitIdx, idx, evtRecs[idx].puEvtIdx, value);
    }

    // If the transition count has changed, let's retry the snapshot
    if (curTransitionCount != (volatile uint64_t)muxSwitchTransCount) {
        value = Upci_Punit_Event_ReadSnapShot(pPunit, evtRecs[idx].puEvtIdx);
        //fprintf(stderr, "%s" _AT_ " curTransitionCount=%10d, curPunitIdx=%02d, evtIdx=%02d, puEvtIdx=%02d, value=0x%016lx\n",
        //        IND_STRG, curTransitionCount, curPunitIdx, idx, evtRecs[idx].puEvtIdx, value);
    }

    *pVal = value;

    if (normalizeEvents) {
        uint64_t curEvtCycles = GetMuxEventCycles(curPunitIdx, false);
        if (curEvtCycles > 0) {
            uint64_t maxCycles = GetMaxMuxCycles();
            double scale = (double)maxCycles / (double)curEvtCycles;
            *pVal = (uint64_t)(*pVal * scale);
        }
    }

    BGPM_TRACE_DATA_L2( \
        fprintf(stderr, "%s" _AT_ " idx=%02d, *pVal=0x%016lx\n", IND_STRG, idx, *pVal) \
    );

    return 0;
}



int ES_MXPunit::WriteEvent(unsigned idx, uint64_t val) {
    BGPM_TRACE_L2;

    Upci_Punit_t *pPunit = puList[activePunitIdx];

#ifndef BGPM_FAST

    if (UNLIKELY(!(pPunit->status & UPCI_UnitStatus_Applied))) {
        return lastErr.PrintOrExit(BGPM_ENOT_APPLIED, BGPM_ERRLOC);
    }
    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    if (UNLIKELY(evtRecs[idx].evtAttrMask & (unsigned)EvtAttr_Ovf)) {
        return lastErr.PrintOrExit(BGPM_EINV_WRT2OVF, BGPM_ERRLOC);
    }

    //! Note:  Write is not valid while the event set is running, so we don't have to
    //!        worry about pm interrupts switching the active event set while this is running.
    if (UNLIKELY(running)) {
        return lastErr.PrintOrExit(BGPM_EUPDATE_DISALLOWED, BGPM_ERRLOC);
    }
#endif

    if (pPunit == puList[activePunitIdx]) {
        Upci_Punit_Event_WriteIdx(evtRecs[idx].pPunit, evtRecs[idx].puEvtIdx, val);
    }
    else {
        Upci_Punit_Event_WriteSnapShot(evtRecs[idx].pPunit, evtRecs[idx].puEvtIdx, val);
    }

    BGPM_TRACE_DATA_L2( \
        fprintf(stderr, "%s" _AT_ " *pVal=0x%016lx\n", IND_STRG, \
                Upci_Punit_Event_ReadIdx(evtRecs[idx].pPunit, evtRecs[idx].puEvtIdx)) \
    );

    return 0;
}







int ES_MXPunit::Punit_GetHandles(Bgpm_Punit_Handles_t *pH){ return DefaultError(); }
int ES_MXPunit::Punit_GetLLHandles(Bgpm_Punit_LLHandles_t *pH){ return DefaultError(); }


int ES_MXPunit::PrintCurrentReservations()
{
    BGPM_TRACE_L2;

    char buf[BGPM_FEEDBACK_BUFSIZE];

    for (unsigned i=0; i<puList.size(); ++i) {
        FillFeedbackBuffer(buf, BGPM_FEEDBACK_BUFSIZE, puList[i], 0, (int)i);
        PrintLines(buf, BGPM_FEEDBACK_BUFSIZE);
    }
    return 0;
}



int ES_MXPunit::SwitchMuxGrp()
{
    BGPM_TRACE_L2;

    // Stop counting if active.
    Upci_Punit_t *pPunit = puList[activePunitIdx];
    uint64_t ctrCntl = upc_p_local->control & (pPunit->cfg.thdCtrMask | pPunit->cfg.ovfCtrMask);
    //fprintf(stderr, _AT_ " ctrCntrl=0x%016lx, upc_p->control=0x%016lx, ctrMask=0x%08x\n",
    //        ctrCntl, upc_p_local->control, (pPunit->cfg.thdCtrMask | pPunit->cfg.ovfCtrMask) );
    Upci_Punit_StopN_Snapshot_Counts(pPunit);

    // Swap to next punit

    // Use muxSwitchTransCount to make this operation thread safe.  The muxSwitchTransCount value works this way:
    //
    // Only the controlling thread (the creator of the event set), can do a mux switch, but the operation is not atomic.
    // The Apply operation takes some time to setup the hardware and copy restore the snapshot values to the hardware counters
    // and another thread or the currents thread's overflow signal handler could attempt to read during the transition.
    // We don't have the ability to create a critical section, and can't add locking because the signal handler could create
    // a deadlock.  So, we take advantage that only the controlling single thread can be doing the apply.  We also
    // assume that the controlling thread cannot do a SwitchMuxGrp operation both on the main level, and within the
    // signal handler.
    //
    // But, once the "activePunitIdx" value has changed, a reader thread (or signal handler) would think that the
    // hardware contains associated counter value, but it may not be there yet.
    // So, a reading thread or the overflow signal handler needs to know if a switch is in progress to know whether to
    // read the counter from the hardware, or read the snapshot.
    //
    // Thus  - each switch will increment this value at the start of the switch, and increment it again at the end of switch.
    // The reader thread or handler must check the value before the read,
    //    if the value is odd, then use the snapshot instead of hardware value, because the hardware is in transition.
    // At the end of the read, double check if the muxSwitchTransCount value has changed,
    //    if so use the snapshot value instead - the hardware may have been in transition.
    muxSwitchTransCount++;
    if (++activePunitIdx == puList.size()) activePunitIdx = 0;
    mbar();  // make sure the memory updates don't get reordered.

    assert(swModeAppliedPuEvtSet>=0);
    pPunit = puList[activePunitIdx];
    if (inSigHandler) Upci_Punit_DisablePMInt(pPunit);
    ES_SWPunit::Apply(swModeAppliedPuEvtSet); // apply without resetting counters 1st
    if (inSigHandler) Upci_Punit_EnablePMInt(pPunit);


    #if 0
    // <<<<<<<<<<<< Debug func/ovfFastThd results -- seeing too many ovfs - some coming too early
    // before the threshold count has expired.
    // I discovered some cases where upc_p counter arm bit is already active in counter value
    // from prior mux group event.
    // Then, when an apply of the current punit apply overwrites the upc_p counter,
    // the arm bit is not always being reset.
    // The problem was that the upc_c sends the arm bit whenever bits 0-51 are set in the upc_c
    // counter.  The Punit_Apply() operation needed wait at least 74 cycles after writing a upc_c
    // counter value to disable the arm bit to releabably clearn it.
    // Leaving this comment it place just because it shows how to query the hardware for a particular
    // counter value if needed again.
    unsigned tmpEvtIdx      = 2;
    Upci_Punit_t *pTmpPunit = evtRecs[tmpEvtIdx].pPunit;
    unsigned tmpPuIdx       = evtRecs[tmpEvtIdx].puEvtIdx;
    unsigned tmpCGrp        = pTmpPunit->evts[tmpPuIdx].c_Cgrp;
    unsigned tmpPCtr        = pTmpPunit->evts[tmpPuIdx].p_Ctr;
    if (pTmpPunit == pPunit) {
        uint64_t value          = upc_c->data24.grp[tmpCGrp].counter[tmpPCtr];
        fprintf(stderr, _AT_ " ctr=%d, raw instr counter = %ld, curupcp=0x%08lx, curctrmask=0x%08x\n",
                tmpPCtr, ~value + 1, upc_p_local->counter[tmpPCtr],
                pPunit->cfg.ovfCtrMask | pPunit->cfg.thdCtrMask);

        //uint64_t ssvalue              = pTmpPunit->cfg.ctrSS[tmpPCtr];
        //upc_p_local->counter[tmpPCtr] = UPC_P__COUNTER__THRESHOLD_COUNT_set(ssvalue); // includes armed bit as necessary
        //fprintf(stderr, _AT_ " ctr=%d, raw instr counter = %ld, curupcp=0x%08lx, curctrmask=0x%08x\n",
        //        tmpPCtr, ~value + 1, upc_p_local->counter[tmpPCtr],
        //        pPunit->cfg.ovfCtrMask | pPunit->cfg.thdCtrMask);

    }
    else {
        uint64_t value          = pTmpPunit->cfg.ctrSS[tmpPCtr];
        fprintf(stderr, _AT_ " ctr=%d, ss  instr counter = %ld, curupcp=0x%08lx, curctrmask=0x%08x\n",
                tmpPCtr, ~value + 1, upc_p_local->counter[tmpPCtr],
                pPunit->cfg.ovfCtrMask | pPunit->cfg.thdCtrMask);
    }
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    #endif

    muxSwitchTransCount++;
    mbar();

    muxApplies[activePunitIdx]++;
    pPunit = puList[activePunitIdx];


    numMuxSwitches++;

    //Dump(IND_LVL);

    // Restart counting again if the prior set was active.
    //fprintf(stderr, _AT_ " new ctrMask=0x%08x\n", (pPunit->cfg.thdCtrMask | pPunit->cfg.ovfCtrMask) );
    if (ctrCntl) {
        Upci_Punit_Start_Indep(pPunit, UPCI_CTL_OVF);
    }

    return 0;
}


int ES_MXPunit::GetMux(Bgpm_MuxStats_t* pMuxStats)
{
    BGPM_TRACE_L2;
    assert(pMuxStats);
    Bgpm_MuxStats_t & stats = *pMuxStats;
    stats.active = 1;

    stats.elapsedCycles = 0;
    for (unsigned i=0; i<puList.size(); ++i) {
        stats.elapsedCycles += GetMuxCycles(i);
    }

    stats.numGrps = puList.size();

    stats.numSwitches = numMuxSwitches;

    stats.normalize = normalizeEvents;

    stats.maxEvtsPerGrp = maxMuxEvtsPerPunit;

    stats.period = muxPeriod;
    return 0;
}



int ES_MXPunit::GetMuxGrps()
{
    return puList.size();
}



uint64_t ES_MXPunit::GetMuxCycles(unsigned muxGrp)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(muxGrp >= puList.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    uint64_t cycles = 0;
    unsigned idx = muxCycleEvtIdx[muxGrp];

    ushort saveNormal = normalizeEvents;
    normalizeEvents = 0;

    ReadAnyEvent(idx,&cycles);

    normalizeEvents = saveNormal;

    BGPM_TRACE_DATA_L2( \
        fprintf(stderr, "%s" _AT_ " cycles=0x%016lx\n", IND_STRG, cycles) \
    );
    return cycles;
}



uint64_t ES_MXPunit::GetMuxEventCycles(unsigned evtIdx, ushort normalize)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(evtIdx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }

    // find punit index for event
    Upci_Punit_t *pPunit = evtRecs[evtIdx].pPunit;

    uint64_t cycles = 0;
    if (normalizeEvents && normalize) {
        cycles = GetMaxMuxCycles();
    }
    else {
        unsigned cycEvtIdx;
        unsigned puIdx = 0;
        // cycle events are always at the end - in punit order - one per punit.
        // find the right punit
        for (cycEvtIdx = evtRecs.size() - puList.size();
             cycEvtIdx<evtRecs.size(); ++cycEvtIdx) {
            if (evtRecs[cycEvtIdx].pPunit == pPunit) {
                assert(pPunit == puList[puIdx]);  // insure order of cycle counts matches event list (so we use the right puIdx)
                cycles = GetMuxCycles(puIdx);
                break;
            }
            puIdx++;
        }
    }


    BGPM_TRACE_DATA_L2( \
        fprintf(stderr, "%s" _AT_ " cycles=0x%016lx\n", IND_STRG, cycles) \
    );
    return cycles;
}



uint64_t ES_MXPunit::GetMuxIntStatus(uint64_t intMask)
{
    return intMask & muxIntStat[activePunitIdx];
}
