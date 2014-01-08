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
#include "ES_IOunit.h"

using namespace bgpm;


EvtSet::ESTypes ES_IOunit::ESType() { return ESType_IOunit; }
const char *ES_IOunit::ESTypeLabel() { return "I/O Unit"; }
//EvtSet *ES_IOunit::Spawn() { return new ES_IOunit(); }
//EvtSet *ES_IOunit::Clone(uint64_t target) { return new ES_IOunit(*this); }  // ignore target
bool ES_IOunit::IsEventIdCompatible(int evtId) {
    return ((evtId >= PEVT_MU_PKT_INJ) && (evtId <= PEVT_IOUNIT_LAST_EVENT));
}


int ES_IOunit::AddEvent(unsigned evtId, EvtSet* & pNewEvtSet)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }

    pNewEvtSet = NULL;
    IOunitEvtRec curRec(evtId);
    evtRecs.push_back(curRec);

    return 0;
}



int ES_IOunit::AddEventList(unsigned *evtIds, unsigned num, EvtSet* & pNewEvtSet)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }

    pNewEvtSet = NULL;
    int rc;
    for (unsigned i=0; i<num; ++i) {
        if ((rc = AddEvent(evtIds[i], pNewEvtSet)) < 0) { return rc; }
    }

    return 0;
}



int ES_IOunit::NumEvents() const { return evtRecs.size(); }

int ES_IOunit::Apply(int hEvtSet) {
    BGPM_TRACE_L2;
    frozen = true;
    swModeAppliedIOEvtSet = hEvtSet;
    Reset();
    return 0;
}


int ES_IOunit::Start() {
    BGPM_TRACE_L2;
    if (pShMem->perspect == BGPM_HW_PERSPECTIVE ) {
        UPC_C_Start_Sync_Counting();
    }
    else {
        UPC_IO_Start(nodeConfig);
    }
    pShMem->controlIO.SetRunning(true);

    return 0;
}


int ES_IOunit::Stop() {
    BGPM_TRACE_L2;
    if (pShMem->perspect == BGPM_HW_PERSPECTIVE ) {
        UPC_C_Stop_Sync_Counting();
    }
    else {
        UPC_IO_Stop(nodeConfig);
    }
    pShMem->controlIO.SetRunning(false);
    return 0;
}


int ES_IOunit::ResetStart() {
    BGPM_TRACE_L2;
    UPC_IO_Stop(nodeConfig);
    Reset();
    Start();
    return 0;
}


int ES_IOunit::Reset() {
    BGPM_TRACE_L2;

    bool ioRunning = (DCRReadUser(MU_DCR(UPC_MU_COUNTER_CONTROL_RW)) != 0);
    pShMem->controlIO.SetRunning(false);
    UPC_IO_Stop(nodeConfig);
    UPC_IO_Clear(nodeConfig);

    // refill any threshold counts.
    unsigned idx;
    for (idx=0; idx<evtRecs.size(); ++idx) {
        if (UNLIKELY((unsigned)(evtRecs[idx].evtAttrMask) & (unsigned)EvtAttr_Ovf)) {
            unsigned ctrIdx = evtRecs[idx].ctrIdx;
            pShMem->controlIO.accumVals[ctrIdx] = 0;
            pShMem->controlIO.ResetThreshold(ctrIdx);
        }
    }

    if (ioRunning) {
        UPC_IO_Start(nodeConfig);
        pShMem->controlIO.SetRunning(true);
    }

    return 0;
}


int ES_IOunit::ReadEvent(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;
    assert(pVal);
    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }

    unsigned ctrIdx = evtRecs[idx].ctrIdx;
    if (UNLIKELY((unsigned)(evtRecs[idx].evtAttrMask) & (unsigned)EvtAttr_Ovf)) {
        *pVal = pShMem->controlIO.accumVals[ctrIdx];

        uint64_t value = upc_c->iosram_data[ctrIdx];
        uint64_t thres = pShMem->controlIO.GetOvfThreshold(ctrIdx);
        if (value < thres) {
            // the counter has wrapped, might have missed overflow.  Note that if the current thread is the one
            // updating the accumVals and resetting the threshold in the counter, this is safe.  However:
            // If another thread is doing the updates while this thread is reading, there might be window where
            // the accumVal has already been incremented by the latest threshold, but the counter value has not
            // yet been reset.  Thus - the threshold count could get added twice to the returned value.
            // A subsequent read would only have the threshold counted once.   I don't have a good solution
            // unless we always want to assume we are in the window and report the lower value - then we potentially
            // permanently lose and thresholds worth of counts in the reporting.
            // Both threads are likely to be in the interrupt handler anyway, so the problem is "unlikely" to occur.
            *pVal += value + (~thres + 1);
        }
        else {
            *pVal += value - thres;
        }
    }
    else {
        *pVal = upc_c->iosram_data[ctrIdx];
    }

    return 0;
}


int ES_IOunit::WriteEvent(unsigned idx, uint64_t val) {
    BGPM_TRACE_L2;

    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    upc_c->iosram_data[evtRecs[idx].ctrIdx] = val;

    return 0;
}


const char * ES_IOunit::GetEventLabel(unsigned idx) {
    BGPM_TRACE_L2;
    if (UNLIKELY(idx >= evtRecs.size())) {
        lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
        return NULL;
    }
    return evtRecs[idx].pEvent->label;
}


int ES_IOunit::GetEventId(unsigned idx) {
    BGPM_TRACE_L2;
    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    return evtRecs[idx].pEvent->evtId;
}


int ES_IOunit::GetEventIndex(unsigned evtId, unsigned startIdx) {
    BGPM_TRACE_L2;
    if (startIdx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    unsigned idx = startIdx;
    bool found = false;
    while ((idx<evtRecs.size()) && (!found)) {
       if (evtRecs[idx].pEvent->evtId == evtId) {
           found = true;
       }
       else {
           idx++;
       }
    }
    return idx;
}



int ES_IOunit::SetEvtUser1(unsigned idx, uint64_t val) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    evtRecs[idx].user1 = val;
    return 0;
}
int ES_IOunit::GetEvtUser1(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    *pVal = evtRecs[idx].user1;
    return 0;
}
int ES_IOunit::SetEvtUser2(unsigned idx, uint64_t val) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    evtRecs[idx].user2 = val;
    return 0;
}
int ES_IOunit::GetEvtUser2(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    *pVal = evtRecs[idx].user2;
    return 0;
}



int ES_IOunit::SetOverflowHandler(Bgpm_OverflowHandler_t handler)
{
    BGPM_TRACE_L2;
    pOvfHandler = handler;
    return 0;
}


int ES_IOunit::SetOverflow(unsigned idx, uint64_t threshold)
{
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }

    unsigned bit = evtRecs[idx].ctrIdx;

    if (UNLIKELY(!pShMem->controlIO.RsvOvfThreshold(bit, threshold))) {
        return lastErr.PrintOrExit(BGPM_ETHRES_MISMATCH, BGPM_ERRLOC);
    }

    if (!ovfInUse) {
        ovfInUse = true;
        pShMem->controlIO.IncrRef();
    }

    evtRecs[idx].evtAttrMask = (EvtAttribs)((unsigned)(evtRecs[idx].evtAttrMask) | (unsigned)EvtAttr_Ovf);
    upc_c->iosram_data[bit] = threshold;

    dbgThdVars.ovfEnabled = 1;
    {
        ShMemLock guard(&(pShMem->controlIO.lock));
        upc_c->io_ccg_int_en |= UPC_C_DCR__IO_CCG_INT_EN__IE_set(MASK16_bit(bit/16));
    }
    Kernel_Upci_ResetInts(UPC_P__INT_STATUS__UPC_C_THREAD_INT_bit(Kernel_ProcessorThreadID()));
 
    return 0;
}



int ES_IOunit::GetOverflow(unsigned idx, uint64_t *pThreshold)
{
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    assert(pThreshold);
    assert(pShMem != NULL);

    *pThreshold = pShMem->controlIO.GetOvfThreshold(evtRecs[idx].ctrIdx);
    return 0;
}



Bgpm_OverflowHandler_t ES_IOunit::GetOverflowHandler() { return pOvfHandler; }


int ES_IOunit::GetOverflowIndices(uint64_t ovfVector, unsigned *pIndicies, unsigned *pLen)
{
    assert(pIndicies);
    assert(pLen);

    unsigned numInts = upc_popcnt64(ovfVector);

    if (*pLen < numInts) {
        *pLen = numInts;
        return lastErr.PrintOrExit(BGPM_EARRAY_TOO_SMALL, BGPM_ERRLOC);
    }

    *pLen = 0;
    for (unsigned idx=0; idx<evtRecs.size() && ovfVector; ++idx) {
        if ((unsigned)(evtRecs[idx].evtAttrMask) & (unsigned)EvtAttr_Ovf) {
            if (ovfVector & MASK64_bit(evtRecs[idx].ctrIdx)) {
                pIndicies[(*pLen)++] = idx;
                ovfVector &= ~MASK64_bit(evtRecs[idx].ctrIdx);
            }
        }
    }
    return 0;
}




