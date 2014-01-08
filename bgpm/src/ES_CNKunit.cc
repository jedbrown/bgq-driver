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
#include "ES_CNKunit.h"

using namespace bgpm;


EvtSet::ESTypes ES_CNKunit::ESType() { return ESType_CNKunit; }
const char *ES_CNKunit::ESTypeLabel() { return "CNK Unit"; }
bool ES_CNKunit::IsEventIdCompatible(int evtId) {
    return ((evtId >= PEVT_CNKNODE_MUINT) && (evtId <= PEVT_CNKUNIT_LAST_EVENT));
}


int ES_CNKunit::AddEvent(unsigned evtId, EvtSet* & pNewEvtSet)
{
    BGPM_TRACE_L2;

    pNewEvtSet = NULL;

    const UPC_CNKunitEvtTableRec_t *pEvent = &CNKunitEvtTbl[evtId-PEVT_CNKNODE_MUINT];
    assert(pEvent->evtId == (unsigned)evtId);

    Upci_CNKCtrType_t ctrType = (Upci_CNKCtrType_t)(pEvent->evtSrc - UPC_ES_CNKNODE);
    assert((ctrType >= CNKCtrNode) && (ctrType <= CNKCtrHWT));

    unsigned idx;
    PerfCountItem_t buffItem;
    buffItem.id = pEvent->selVal;
    buffItem.value = 0;

    idx = ctrBuffs[(int)ctrType].size();
    ctrBuffs[(int)ctrType].push_back(buffItem);

    CNKEvtRec curRec(pEvent, ctrType, idx);
    evtRecs.push_back(curRec);

    return 0;
}



int ES_CNKunit::AddEventList(unsigned *evtIds, unsigned num, EvtSet* & pNewEvtSet)
{
    BGPM_TRACE_L2;

    pNewEvtSet = NULL;
    int rc;
    for (unsigned i=0; i<num; ++i) {
        if ((rc = AddEvent(evtIds[i], pNewEvtSet)) < 0) { return rc; }
    }

    return 0;
}



int ES_CNKunit::NumEvents() const { return evtRecs.size(); }

int ES_CNKunit::Apply(int hEvtSet) {
    BGPM_TRACE_L2;
    // nothing to do
    return 0;
}



int ES_CNKunit::UpdateCountBuffers()
{
    BGPM_TRACE_L2;

    // Populate ctrBuffs with counter values from cnk
    for (int i=0; i<=(int)CNKCtrHWT; ++i) {
        if (ctrBuffs[i].size() > 0) {
            CNKCntBuff & ctrBuff = ctrBuffs[i];
            int rc = Kernel_Upci_GetCNKCounts((Upci_CNKCtrType_t)i, &ctrBuff[0], ctrBuff.size());
            if (rc < 0) {
                return lastErr.PrintOrExit(rc, BGPM_ERRLOC);
            }
        }
    }
    return 0;
}



int ES_CNKunit::UpdateIndivCount(unsigned evtIdx)
{
    BGPM_TRACE_L2;

    // Populate ctrBuffs with counter values from cnk
    CNKCntBuff & ctrBuff = ctrBuffs[(int)evtRecs[evtIdx].ctrType];
    unsigned buffIdx = evtRecs[evtIdx].buffIdx;
    int rc = Kernel_Upci_GetCNKCounts(evtRecs[evtIdx].ctrType, &ctrBuff[buffIdx], 1);
    if (rc < 0) {
        return lastErr.PrintOrExit(rc, BGPM_ERRLOC);
    }
    assert(ctrBuff[buffIdx].value >= evtRecs[evtIdx].startVal);
    uint64_t deltaVal = ctrBuff[buffIdx].value - evtRecs[evtIdx].startVal;
    evtRecs[evtIdx].accumVal += deltaVal;
    evtRecs[evtIdx].startVal = ctrBuff[buffIdx].value;
    return 0;
}



int ES_CNKunit::Start() {
    BGPM_TRACE_L2;

    if (UNLIKELY(running)) { return BGPM_WALREADY_STARTED; }

    // Populate ctrBuffs with counter values from cnk
    int rc = UpdateCountBuffers();

    if (rc >= 0) {
        // capture current start values for each event.
        for (unsigned i=0; i<evtRecs.size(); ++i) {
            CNKCntBuff & ctrBuff = ctrBuffs[(int)evtRecs[i].ctrType];
            evtRecs[i].startVal = ctrBuff[evtRecs[i].buffIdx].value;
        }
    }
    running = true;
    return 0;
}



int ES_CNKunit::Stop() {
    BGPM_TRACE_L2;

    if (UNLIKELY(!running)) { return BGPM_WALREADY_STOPPED; }

    // Populate ctrBuffs with counter values from cnk
    int rc = UpdateCountBuffers();

    if (rc >= 0) {
        // update accum values from start value delta.
        for (unsigned i=0; i<evtRecs.size(); ++i) {
            CNKCntBuff & ctrBuff = ctrBuffs[(int)evtRecs[i].ctrType];
            assert(ctrBuff[evtRecs[i].buffIdx].value >= evtRecs[i].startVal);
            uint64_t deltaVal = ctrBuff[evtRecs[i].buffIdx].value - evtRecs[i].startVal;
            evtRecs[i].accumVal += deltaVal;
        }
    }
    running = false;
    return rc;
}



int ES_CNKunit::ResetStart() {
    BGPM_TRACE_L2;
    for (unsigned i=0; i<evtRecs.size(); ++i) {
        evtRecs[i].accumVal = 0;
    }
    running = false;  // make start update current start values as well in case already running.
    return Start();
}



int ES_CNKunit::Reset() {
    BGPM_TRACE_L2;
    int rc = 0;
    if (running) {
        rc = ResetStart();
    }
    else {
        for (unsigned i=0; i<evtRecs.size(); ++i) {
            evtRecs[i].accumVal = 0;
        }
    }
    return rc;
}


int ES_CNKunit::ReadEvent(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;
    assert(pVal);
    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }

    if (running) {
        int rc;
        if ((rc = UpdateIndivCount(idx)) < 0) { return rc; }
    }
    *pVal = evtRecs[idx].accumVal;

    return 0;
}



int ES_CNKunit::SampleCNKEvent(unsigned idx, uint64_t *pVal)
{
    BGPM_TRACE_L2;
    assert(pVal);
    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }

    CNKCntBuff & ctrBuff = ctrBuffs[(int)evtRecs[idx].ctrType];
    unsigned buffIdx = evtRecs[idx].buffIdx;
    int rc = Kernel_Upci_GetCNKCounts(evtRecs[idx].ctrType, &ctrBuff[buffIdx], 1);
    if (rc < 0) {
        return lastErr.PrintOrExit(rc, BGPM_ERRLOC);
    }
    *pVal = ctrBuff[idx].value;

    return 0;
}



int ES_CNKunit::WriteEvent(unsigned idx, uint64_t val) {
    BGPM_TRACE_L2;

    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    if (running) {
        int rc;
        if ((rc = UpdateIndivCount(idx)) < 0) { return rc; }
    }
    evtRecs[idx].accumVal = val;

    return 0;
}


const char * ES_CNKunit::GetEventLabel(unsigned idx) {
    BGPM_TRACE_L2;
    if (UNLIKELY(idx >= evtRecs.size())) {
        lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
        return NULL;
    }
    return evtRecs[idx].pEvent->label;
}


int ES_CNKunit::GetEventId(unsigned idx) {
    BGPM_TRACE_L2;
    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    return evtRecs[idx].pEvent->evtId;
}


int ES_CNKunit::GetEventIndex(unsigned evtId, unsigned startIdx) {
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



int ES_CNKunit::SetEvtUser1(unsigned idx, uint64_t val) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    evtRecs[idx].user1 = val;
    return 0;
}
int ES_CNKunit::GetEvtUser1(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    *pVal = evtRecs[idx].user1;
    return 0;
}
int ES_CNKunit::SetEvtUser2(unsigned idx, uint64_t val) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    evtRecs[idx].user2 = val;
    return 0;
}
int ES_CNKunit::GetEvtUser2(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    *pVal = evtRecs[idx].user2;
    return 0;
}




