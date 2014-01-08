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
#include "ES_L2unit.h"

using namespace bgpm;


EvtSet::ESTypes ES_L2unit::ESType() { return ESType_L2unit; }
const char *ES_L2unit::ESTypeLabel() { return "L2 Unit"; }
//EvtSet *ES_L2unit::Spawn() { return new ES_L2unit(combine); }
//EvtSet *ES_L2unit::Clone(uint64_t target) { return new ES_L2unit(*this); }  // ignore target
bool ES_L2unit::IsEventIdCompatible(int evtId) {
    return ((evtId >= PEVT_L2_HITS) && (evtId <= PEVT_L2UNIT_LAST_EVENT));
}
bool ES_L2unit::L2ModeOk(int evtId) {
    if ((evtId <= PEVT_L2_SPEC_RETRY) != pShMem->combineL2) {
        lastErr.PrintOrExit(BGPM_ECONF_L2_COMBINE, BGPM_ERRLOC);
        return false;
    }

    return true;
}


int ES_L2unit::AddEvent(unsigned evtId, EvtSet* & pNewEvtSet)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }

    pNewEvtSet = NULL;
    if (!L2ModeOk(evtId)) { return lastErr; }

    L2unitEvtRec curRec(evtId);
    evtRecs.push_back(curRec);

    return 0;
}



int ES_L2unit::AddEventList(unsigned *evtIds, unsigned num, EvtSet* & pNewEvtSet)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }

    pNewEvtSet = NULL;
    // precheck event ids
    int rc;
    for (unsigned i=0; i<num; ++i) {
        if (!L2ModeOk(evtIds[i])) { return lastErr; }
    }

    for (unsigned i=0; i<num; ++i) {
        if ((rc = AddEvent(evtIds[i], pNewEvtSet)) < 0) { return rc; }
    }

    return 0;
}


int ES_L2unit::NumEvents() const { return evtRecs.size(); }


int ES_L2unit::Apply(int hEvtSet) {
    BGPM_TRACE_L2;
    frozen = true;
    swModeAppliedL2EvtSet = hEvtSet;

    Reset();

    if (ovfInUse) {
        Kernel_Upci_ResetInts(UPC_P__INT_STATUS__UPC_C_THREAD_INT_bit(Kernel_ProcessorThreadID())); // Clear any active interrupts and enable overflow interrupts for this thread.
    }

    return 0;
}


int ES_L2unit::Start() {
    BGPM_TRACE_L2;
    if (pShMem->perspect == BGPM_HW_PERSPECTIVE ) {
        UPC_C_Start_Sync_Counting();
    }
    else {
        UPC_L2_Start();
    }
    pShMem->controlL2.SetRunning(true);

    return 0;
}


int ES_L2unit::Stop() {
    BGPM_TRACE_L2;
    if (pShMem->perspect == BGPM_HW_PERSPECTIVE ) {
        UPC_C_Stop_Sync_Counting();
    }
    else {
        UPC_L2_Stop();
    }
    pShMem->controlL2.SetRunning(false);
    return 0;
}


int ES_L2unit::ResetStart() {
    BGPM_TRACE_L2;
    UPC_L2_Stop();
    Reset();
    Start();
    return 0;
}


int ES_L2unit::Reset() {
    BGPM_TRACE_L2;
    // make sure counts are stopped.
    uint64_t saveRunState = DCRReadUser(UPC_C_DCR(COUNTER_START));
    UPC_C_Stop_Sync_Counting();
    pShMem->controlL2.SetRunning(false);


    UPC_L2_Reset();

    // refill any threshold counts.
    unsigned idx;
    for (idx=0; idx<evtRecs.size(); ++idx) {
        if (UNLIKELY((unsigned)(evtRecs[idx].evtAttrMask) & (unsigned)EvtAttr_Ovf)) {
            unsigned ctrIdx = evtRecs[idx].ctrIdx;
            pShMem->controlL2.accumVals[ctrIdx] = 0;
            pShMem->controlL2.ResetThreshold(ctrIdx);
        }
    }

    if (saveRunState) {
        UPC_C_Start_Sync_Counting();
        pShMem->controlL2.SetRunning(true);
    }
    return 0;
}


int ES_L2unit::ReadEvent(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;
    assert(pVal);
    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }

    unsigned ctrIdx = evtRecs[idx].ctrIdx;
    assert(ctrIdx < UPC_L2_NUM_COUNTERS);

    *pVal = UPC_L2_ReadCtr(0, ctrIdx);  // Combined events always in 0th slice position.

    // combine other values if slices are separate
    // BTW - we don't support overflow when using separate slice events
    if (! pShMem->combineL2) {
        for (int i=1; i<L2_DCR_num; ++i) {
            *pVal += UPC_L2_ReadCtr(i, ctrIdx);
        }
    }
    // Otherwise check for overflow ctr
    else {
        if (UNLIKELY((unsigned)(evtRecs[idx].evtAttrMask) & (unsigned)EvtAttr_Ovf)) {
            uint64_t value = *pVal;  // keep value read from hardware.
            *pVal = pShMem->controlL2.accumVals[ctrIdx];
            //fprintf(stderr, _AT_ " *pVal=0x%016lx\n", *pVal);
            uint64_t thres = pShMem->controlL2.GetOvfThreshold(ctrIdx);
            if (value < thres) {
                // the counter has wrapped.  We might also be in a window where the accumVal
                // might have been updated, but the counter may not have been reset back to the
                // threshold yet.
                // We could also be in the window where the accumval has not be updated, and
                // the threshold has not be reset.
                // We need to report the smaller of the two values so that the count is ever
                // increasing.
                *pVal += value;
            }
            else {
                *pVal += value - thres;
            }

            //fprintf(stderr, _AT_ " *pVal=0x%016lx, ctr=0x%016lx, threshold=0x%016lx\n",
            //        *pVal, UPC_L2_ReadCtr(0, ctrIdx), pShMem->GetIOL2OvfThreshold(ctrIdx+IOMASK_L2CTR_OVERLAY_OFFSET));
        }
    }
    return 0;
}


int ES_L2unit::WriteEvent(unsigned idx, uint64_t val) {
    BGPM_TRACE_L2;

    if (UNLIKELY(!pShMem->combineL2)) {
        return lastErr.PrintOrExit(BGPM_EINV_OP, BGPM_ERRLOC);
    }

    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }

    UPC_L2_WriteCtr(0, evtRecs[idx].ctrIdx, val);

    return 0;
}


int ES_L2unit::L2_ReadEvent(unsigned slice, unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;
    assert(pVal);
    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    if (UNLIKELY((slice < 0) || (slice >= L2_DCR_num))) {
        return lastErr.PrintOrExit(BGPM_EINV_L2_SLICE, BGPM_ERRLOC);
    }

    *pVal = UPC_L2_ReadCtr(slice, evtRecs[idx].ctrIdx);

    return 0;
}


int ES_L2unit::L2_WriteEvent(unsigned slice, unsigned idx, uint64_t val) {
    BGPM_TRACE_L2;

    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    if (UNLIKELY((slice < 0) || (slice >= L2_DCR_num))) {
        return lastErr.PrintOrExit(BGPM_EINV_L2_SLICE, BGPM_ERRLOC);
    }

    UPC_L2_WriteCtr(slice, evtRecs[idx].ctrIdx, val);

    return 0;
}


const char * ES_L2unit::GetEventLabel(unsigned idx) {
    BGPM_TRACE_L2;
    if (UNLIKELY(idx >= evtRecs.size())) {
        lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
        return NULL;
    }
    return evtRecs[idx].pEvent->label;
}


int ES_L2unit::GetEventId(unsigned idx) {
    BGPM_TRACE_L2;
    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    return evtRecs[idx].pEvent->evtId;
}


int ES_L2unit::GetEventIndex(unsigned evtId, unsigned startIdx) {
    BGPM_TRACE_L2;
    if (UNLIKELY(startIdx >= evtRecs.size())) {
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



int ES_L2unit::SetEvtUser1(unsigned idx, uint64_t val) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    evtRecs[idx].user1 = val;
    return 0;
}
int ES_L2unit::GetEvtUser1(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    *pVal = evtRecs[idx].user1;
    return 0;
}
int ES_L2unit::SetEvtUser2(unsigned idx, uint64_t val) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    evtRecs[idx].user2 = val;
    return 0;
}
int ES_L2unit::GetEvtUser2(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    *pVal = evtRecs[idx].user2;
    return 0;
}



int ES_L2unit::SetOverflowHandler(Bgpm_OverflowHandler_t handler)
{
    BGPM_TRACE_L2;
    pOvfHandler = handler;
    return 0;
}


int ES_L2unit::SetOverflow(unsigned idx, uint64_t threshold)
{
    BGPM_TRACE_L2;
    BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " evtIdx=%d, threshold=0x%016lx\n", IND_STRG, idx, threshold));

    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }

    unsigned bit = evtRecs[idx].ctrIdx;

    if (UNLIKELY(!pShMem->controlL2.RsvOvfThreshold(bit, threshold))) {
        return lastErr.PrintOrExit(BGPM_ETHRES_MISMATCH, BGPM_ERRLOC);
    }

    if (!ovfInUse) {
        ovfInUse = true;
        pShMem->controlL2.IncrRef();
    }

    evtRecs[idx].evtAttrMask = (EvtAttribs)((unsigned)(evtRecs[idx].evtAttrMask) | (unsigned)EvtAttr_Ovf);
    UPC_L2_WriteCtr(0, evtRecs[idx].ctrIdx, threshold);

    dbgThdVars.ovfEnabled = 1;
    upc_c->ccg_int_en |= MASK64_bit(UPC_C_SRAM_BASE_L2_GROUP);  // enable interrupts in L2 combined counter group only

    return 0;
}



int ES_L2unit::GetOverflow(unsigned idx, uint64_t *pThreshold)
{
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    assert(pThreshold);
    assert(pShMem != NULL);

    *pThreshold = pShMem->controlL2.GetOvfThreshold(evtRecs[idx].ctrIdx);
    return 0;
}



Bgpm_OverflowHandler_t ES_L2unit::GetOverflowHandler() { return pOvfHandler; }


int ES_L2unit::GetOverflowIndices(uint64_t ovfVector, unsigned *pIndicies, unsigned *pLen)
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




