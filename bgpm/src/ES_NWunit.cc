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

#include <vector>

#include "globals.h"
#include "ES_NWunit.h"

using namespace bgpm;


ES_NWunit::ES_NWunit(const ES_NWunit &s)
: ES_Active(s), evtRecs(s.evtRecs), enabledLinks(s.enabledLinks), mappedCtrMask(s.mappedCtrMask),
  attachedLinkMask(0), attachedCtlMask(0), attachedCtrCtlMask(0)
{ }



ES_NWunit::~ES_NWunit()
{
    if (attachedLinkMask) {
        Detach();
    }
}



EvtSet::ESTypes ES_NWunit::ESType() { return ESType_NWunit; }
const char *ES_NWunit::ESTypeLabel() { return "NW Unit"; }
//EvtSet *ES_NWunit::Spawn() { return new ES_NWunit(); }
EvtSet *ES_NWunit::Clone() {
    return new ES_NWunit(*this);
}
bool ES_NWunit::IsEventIdCompatible(int evtId) {
    return ((evtId >= PEVT_NW_USER_PP_SENT) && (evtId <= PEVT_NWUNIT_LAST_EVENT));
}



bool ES_NWunit::LinkConfigMatch(unsigned linkNum)
{
    // Can't check link config for disabled links - treat as though ok - since
    // results always zero anyway
    if (enabledLinks & UPC_NW_LINK_bit(linkNum)) {
        for (unsigned i=0; i<evtRecs.size(); ++i) {
            // fprintf(stderr, "%s" _AT_ " ctr[%d]=0x%04x\n", IND_STRG, i, evtRecs[i].ctrIdx);
            if (evtRecs[i].ctrIdx != UPC_NW_UC_CYCLES) {
                if (evtRecs[i].pEvent->selVal != UPC_NW_GetCtrVChannel(linkNum, evtRecs[i].ctrIdx)) {
                    return false;
                }
            }
        }
    }
    return true;
}



int ES_NWunit::Map2AvailCtr(uint16_t & ctrMask, unsigned & ctrIdx)
{
    // See sNWunitEvtTableRec for default ctrIdx Values (selVal field)
    if (ctrIdx == 1) {
        unsigned ctr=0;
        while ( (ctr<4) && (UPC_NW_CTR_bit(ctr) & ctrMask) ) {
            ctr++;
        }
        if (ctr < 4) {
            ctrIdx = ctr;
            ctrMask |= UPC_NW_CTR_bit(ctr);
        }
        else {
            return lastErr.PrintOrExit(BGPM_ECTRRESERVED, BGPM_ERRLOC);
        }
    }
    else if (UPC_NW_CTR_bit(ctrIdx) & ctrMask) {
        return lastErr.PrintOrExit(BGPM_ECTRRESERVED, BGPM_ERRLOC);
    }
    else {
        ctrMask |= UPC_NW_CTR_bit(ctrIdx);
    }

    return 0;
}



int ES_NWunit::AddEvent(unsigned evtId, EvtSet* & pNewEvtSet)
{
    BGPM_TRACE_L2;

    pNewEvtSet = NULL;
    NWunitEvtRec curRec(evtId);
    if (UNLIKELY(Map2AvailCtr(mappedCtrMask, curRec.ctrIdx) < 0)) {
        return lastErr;
    }

    evtRecs.push_back(curRec);

    return 0;
}



int ES_NWunit::AddEventList(unsigned *evtIds, unsigned num, EvtSet* & pNewEvtSet)
{
    BGPM_TRACE_L2;

    pNewEvtSet = NULL;
    int rc;
    for (unsigned i=0; i<num; ++i) {
        if ((rc = AddEvent(evtIds[i], pNewEvtSet)) < 0) { return rc; }
    }

    return 0;
}



int ES_NWunit::NumEvents() const { return evtRecs.size(); }



int ES_NWunit::NumTargets() const
{
    BGPM_TRACE_L2;
    return (int)upc_popcnt32((uint32_t)attachedLinkMask);
}



int ES_NWunit::Attach(uint64_t target, uint64_t agentTarg) {
    BGPM_TRACE_L2;
    // target is a link mask.
    if (UNLIKELY(target & ~(UPC_NW_ALL_LINKS))) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }
    int rc = 0;

    uint16_t tgtLinkMask = (uint16_t)target;

    // Handle each requested link separately to allow multiple Attach operations
    // on same event set.
    unsigned linkNum;
    for (linkNum=0; linkNum<UPC_NW_NUM_LINKS; ++linkNum) {

        Lock guard(&(pShMem->linkRefLock));
        uint16_t curLinkMask = UPC_NW_CTR_bit(linkNum);

        if (tgtLinkMask & curLinkMask) {
            // Check if link is already attached by someone
            if (pShMem->linkRefCounts[linkNum] > 0)  {

                // Don't reattach if already attached by this event set
                if (attachedLinkMask & curLinkMask) {
                    rc = BGPM_WALREADY_ATTACHED;
                }
                // Check that the event config is equal for other attachments
                else if (LinkConfigMatch(linkNum)) {
                    pShMem->linkRefCounts[linkNum]++;
                    attachedLinkMask |= curLinkMask;
                }
                else {
                    return lastErr.PrintOrExit(BGPM_ELINK_CTRS_INCOMPAT, BGPM_ERRLOC);
                }
            }

            // Not already attached, let's attach to link as controller
            else {
                pShMem->linkRefCounts[linkNum]++;
                attachedLinkMask |= curLinkMask;
                attachedCtlMask |= curLinkMask;
                pShMem->linkController[linkNum] = Kernel_ProcessorID();
                pShMem->linkStartCycles[linkNum] = 0;
                pShMem->linkElapsedCycles[linkNum] = 0;
            }
        }
    }

    BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " ctrMask=0x%04x, attachedLinkMask=0x%04x, ctrCtlMask=0x%04x\n", IND_STRG,
                                      mappedCtrMask, attachedLinkMask, attachedCtlMask));
    //pShMem->Dump(IND_LVL);



    // Update NW regs is not atomic - make safe
    Lock guard(&(pShMem->linkRefLock));

    uint16_t tgtCtlMask = attachedCtlMask & tgtLinkMask;
    // control all ctrs for link together - regardless of which counters are configured.
    attachedCtrCtlMask = UPC_NW_CtrCtlMask(tgtCtlMask, UPC_NW_ALL_CTRS);
    UPC_NW_Stop(attachedCtrCtlMask);
    UPC_NW_Clear(attachedCtrCtlMask);

    for (unsigned i=0; i<evtRecs.size(); ++i) {
        // fprintf(stderr, "%s" _AT_ " ctr[%d]=0x%04x\n", IND_STRG, i, evtRecs[i].ctrIdx);
        if (evtRecs[i].ctrIdx != UPC_NW_UC_CYCLES) {
            UPC_NW_SetLinkCtrVChannel(tgtCtlMask, evtRecs[i].ctrIdx, evtRecs[i].vchanMask);
        }
    }

    return 0;
}



int ES_NWunit::Detach() {
    BGPM_TRACE_L2;

    if (UNLIKELY(! attachedLinkMask)) {
        return BGPM_WNOT_LINK_CTL;
    }

    Stop();

    if (attachedLinkMask) {
        unsigned linkNum;
        for (linkNum=0; linkNum<UPC_NW_NUM_LINKS; ++linkNum) {
            uint16_t curLinkMask = UPC_NW_CTR_bit(linkNum);
            if (attachedLinkMask & curLinkMask) {
                Lock guard(&(pShMem->linkRefLock));
                pShMem->linkRefCounts[linkNum]--;
                mbar();
                attachedLinkMask &= ~UPC_NW_CTR_bit(linkNum);
                attachedCtlMask &= ~UPC_NW_CTR_bit(linkNum);
            }
        }
        attachedCtrCtlMask = 0;
    }

    return 0;
}





int ES_NWunit::Start() {
    BGPM_TRACE_L2;
    if (UNLIKELY(! attachedCtlMask)) {
        return BGPM_WNOT_LINK_CTL;
    }

    //fprintf(stderr, "%s" _AT_ " attachedCtlMask=0x%4x, attacheCtrCtlMask=0x%016lx\n", IND_STRG,
    //        attachedCtlMask, attachedCtrCtlMask);

    int rc = BGPM_WALREADY_STARTED;   // assume all links already started - clear if any not.

    uint64_t startCycle;
    {
        // Update NW regs is not atomic - make safe
        Lock guard(&(pShMem->linkRefLock));
        UPC_NW_Start(attachedCtrCtlMask);
        startCycle = GetTimeBase();
    }

    unsigned linkNum;
    for (linkNum=0; linkNum<UPC_NW_NUM_LINKS; ++linkNum) {
        uint16_t curLinkMask = UPC_NW_CTR_bit(linkNum);
        if (attachedCtlMask & curLinkMask) {
            //fprintf(stderr, "%s" _AT_ " linkStartCycles[%0d]=0x%016lx\n", IND_STRG,
            //        linkNum, pShMem->linkStartCycles[linkNum]);

            // Set startcycle if link was not already running
            if (pShMem->linkStartCycles[linkNum] == 0) {
                pShMem->linkStartCycles[linkNum] = startCycle;
                rc = 0;
            }
        }
    }
    return rc;
}



int ES_NWunit::Stop() {
    BGPM_TRACE_L2;
    if (UNLIKELY(! attachedCtlMask)) {
        return BGPM_WNOT_LINK_CTL;
    }

    uint64_t endCycle;
    {
        Lock guard(&(pShMem->linkRefLock));
        UPC_NW_Stop(attachedCtrCtlMask);
        endCycle = GetTimeBase();
    }

    int rc = BGPM_WALREADY_STOPPED;   // assume all links already stopped - clear if any not.

    unsigned linkNum;
    for (linkNum=0; linkNum<UPC_NW_NUM_LINKS; ++linkNum) {
        uint16_t curLinkMask = UPC_NW_CTR_bit(linkNum);
        if (attachedCtlMask & curLinkMask) {
            // Set elapsed cycle if link was running
            if (pShMem->linkStartCycles[linkNum] != 0) {
                pShMem->linkElapsedCycles[linkNum] = endCycle - pShMem->linkStartCycles[linkNum];
                pShMem->linkStartCycles[linkNum] = 0;  // indicated stopped
                rc = 0;
            }
        }
    }
    return rc;
}



int ES_NWunit::ResetStart() {
    BGPM_TRACE_L2;
    if (UNLIKELY(! attachedCtlMask)) {
        return BGPM_WNOT_LINK_CTL;
    }

    Stop();
    Reset();
    return Start();
}


int ES_NWunit::Reset() {
    BGPM_TRACE_L2;
    if (UNLIKELY(! attachedCtlMask)) {
        return BGPM_WNOT_LINK_CTL;
    }

    uint64_t startCycle;
    {
        Lock guard(&(pShMem->linkRefLock));
        UPC_NW_Clear(attachedCtrCtlMask);
        startCycle = GetTimeBase();
    }

    unsigned linkNum;
    for (linkNum=0; linkNum<UPC_NW_NUM_LINKS; ++linkNum) {
        uint16_t curLinkMask = UPC_NW_CTR_bit(linkNum);
        if (attachedCtlMask & curLinkMask) {
            pShMem->linkElapsedCycles[linkNum] = 0;
            mbar();  // make sure another reader thread gets elapsed before start.
            // Set startcycle if link is already running
            if (pShMem->linkStartCycles[linkNum] != 0) {
                pShMem->linkStartCycles[linkNum] = startCycle;
            }
        }
    }
    return 0;
}



int ES_NWunit::ReadEvent(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;
    assert(pVal);
    if (UNLIKELY(! attachedLinkMask)) {
        *pVal = 0;  // return zero if not attached, rather than err code
        return BGPM_WNOT_ATTACHED;
    }



    if (evtRecs[idx].ctrIdx != UPC_NW_UC_CYCLES) {
        //fprintf(stderr, "%s" _AT_ " attachedLinkMask=0x%04x\n", IND_STRG, attachedLinkMask);
        *pVal = UPC_NW_LinkCtrTotal(attachedLinkMask, evtRecs[idx].ctrIdx);
    }
    else {

        uint64_t maxCycles = 0;

        unsigned linkNum;
        for (linkNum=0; linkNum<UPC_NW_NUM_LINKS; ++linkNum) {
            uint16_t curLinkMask = UPC_NW_CTR_bit(linkNum);
            if (attachedLinkMask & curLinkMask) {

                uint64_t curCycles = pShMem->linkElapsedCycles[linkNum];

                // if running add in current elapsed
                if (pShMem->linkStartCycles[linkNum] != 0) {
                    curCycles += GetTimeBase() - pShMem->linkStartCycles[linkNum];
                }
                maxCycles = UPCI_MAX(maxCycles, curCycles);
            }
        }

        *pVal = UPC_NW_Convert2NWCycles(maxCycles);
    }

    return 0;
}



int ES_NWunit::ReadLinkEvent(unsigned idx, uint16_t linkMask, uint64_t *pVal)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(idx >= evtRecs.size())|| idx < 0) { 
        lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
        return BGPM_EINV_EVT_IDX;
    }

    assert(pVal);
    // check that linkMask is subset of active links
    if (UNLIKELY((attachedLinkMask & linkMask) != linkMask)) {
        *pVal = 0;  // return zero if not attached, rather than err code
        return BGPM_WNOT_ATTACHED;
    }

    if (evtRecs[idx].ctrIdx != UPC_NW_UC_CYCLES) {
        *pVal = UPC_NW_LinkCtrTotal(linkMask, evtRecs[idx].ctrIdx);
    }
    else {

        uint64_t maxCycles = 0;

        unsigned linkNum;
        for (linkNum=0; linkNum<UPC_NW_NUM_LINKS; ++linkNum) {
            uint16_t curLinkMask = UPC_NW_CTR_bit(linkNum);
            if (linkMask & curLinkMask) {

                uint64_t curCycles = pShMem->linkElapsedCycles[linkNum];

                // if running add in current elapsed
                if (pShMem->linkStartCycles[linkNum] != 0) {
                    curCycles += GetTimeBase() - pShMem->linkStartCycles[linkNum];
                }

                maxCycles = UPCI_MAX(maxCycles, curCycles);
            }
        }

        *pVal = UPC_NW_Convert2NWCycles(maxCycles);
    }

    return 0;
}




const char * ES_NWunit::GetEventLabel(unsigned idx) {
    BGPM_TRACE_L2;
    if (UNLIKELY(idx >= evtRecs.size())) {
        lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
        return NULL;
    }
    return evtRecs[idx].pEvent->label;
}


int ES_NWunit::GetEventId(unsigned idx) {
    BGPM_TRACE_L2;
    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    return evtRecs[idx].pEvent->evtId;
}


int ES_NWunit::GetEventIndex(unsigned evtId, unsigned startIdx) {
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



int ES_NWunit::SetEvtUser1(unsigned idx, uint64_t val) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    evtRecs[idx].user1 = val;
    return 0;
}
int ES_NWunit::GetEvtUser1(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    *pVal = evtRecs[idx].user1;
    return 0;
}
int ES_NWunit::SetEvtUser2(unsigned idx, uint64_t val) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    evtRecs[idx].user2 = val;
    return 0;
}
int ES_NWunit::GetEvtUser2(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    *pVal = evtRecs[idx].user2;
    return 0;
}



int ES_NWunit::SetVirtChannels(unsigned idx, UPC_NW_Vchannels vchan)
{
    BGPM_TRACE_L2;
    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    if ((evtRecs[idx].pEvent->evtId == PEVT_NW_SENT) ||
        (evtRecs[idx].pEvent->evtId == PEVT_NW_RECV) ||
        (evtRecs[idx].pEvent->evtId == PEVT_NW_RECV_FIFO)) {
        evtRecs[idx].vchanMask = (unsigned)vchan;
    }
    else {
        return lastErr.PrintOrExit(BGPM_EUNSUP_ATTRIB, BGPM_ERRLOC);
    }
    return 0;
}



int ES_NWunit::GetVirtChannels(unsigned idx, UPC_NW_Vchannels * pVchan)
{
    BGPM_TRACE_L2;
    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    if ((evtRecs[idx].pEvent->evtId == PEVT_NW_SENT) ||
        (evtRecs[idx].pEvent->evtId == PEVT_NW_RECV) ||
        (evtRecs[idx].pEvent->evtId == PEVT_NW_RECV_FIFO)) {
        *pVchan = (UPC_NW_Vchannels)(evtRecs[idx].vchanMask);
    }
    else {
        return lastErr.PrintOrExit(BGPM_EUNSUP_ATTRIB, BGPM_ERRLOC);
    }
    return 0;
}


