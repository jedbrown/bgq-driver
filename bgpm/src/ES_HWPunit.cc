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

#include "globals.h"
#include "ES_HWPunit.h"

using namespace bgpm;




ES_HWPunit::~ES_HWPunit()
{
    Detach();
}



EvtSet::ESTypes ES_HWPunit::ESType() { return ESType_HWPunit; }
const char *ES_HWPunit::ESTypeLabel() { return "Punit HW Thread"; }
//EvtSet *ES_HWPunit::Spawn() { return new ES_HWPunit(); }

EvtSet *ES_HWPunit::Clone(uint64_t target, unsigned *addEvents, unsigned addNum)
{
    BGPM_TRACE_L2;
    if (UNLIKELY(target >= CONFIG_MAX_HWTHREADS)) {
        lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
        return NULL;
    }

    ES_SWPunit*p = new ES_HWPunit((unsigned)target);
    if (p->GetCrtRC() >= 0) {
        p->MergeEventCfg(*this, (unsigned)target, addEvents, addNum);
    }
    return p;
}




void ES_HWPunit::TransferSwPunit(ES_SWPunit & src)
{
    BGPM_TRACE_L2;

    // Hold on to any further core reservations as long as the target punit exists.
    pShMem->coreRsv[src.targCore].IncrRef();

    targPuList[src.GetTargThdId()] = src.puList[0];
    src.puList[0] = NULL;
    src.ctrGrpRvd = false;  // transfer ownership of reserved counter affinity groups

    targAttachedCtrGrpMask[src.GetTargThdId()] = pShMem->coreRsv[src.targCore].ctrGrpMask[src.targHwThd];

    assert(coreRsvMask == src.coreRsvMask);   //  Should end up the same

}



int ES_HWPunit::NumTargets() const
{
    BGPM_TRACE_L2;
    unsigned count = 0;
    unsigned idx;
    for (idx=0; idx<targPuList.size(); ++idx) {
        if (targPuList[idx]) count++;
    }
    return count;
}



int ES_HWPunit::Attach(uint64_t target, uint64_t agentTarg)
{
    BGPM_TRACE_L2;
    BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " target=0x%016lx, agentTarg=0x%016lx\n", IND_STRG, target, agentTarg));

    if ((target == 0) && ((agentTarg & 0xF000000000000000ULL) == 0)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }


    // Loop through matching targets
    int rc = 0;
    unsigned targThdId;
    for (targThdId=0; targThdId<CONFIG_MAX_HWTHREADS; ++targThdId) {
        if ((target & MASK64_bit(targThdId)) ||
            ((targThdId >= 64) && (agentTarg & MASK64_bit(targThdId%64)))) {

            // Use the hwThds ctrGrpMask to determine which threads this evtSet will own
            // so no other event set can attach to these threads.  None should be attached
            // yet until the target SWEvtSet is been created below
            BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " targAttachedCtrGrpMask[%02d]=0x%02x\n", IND_STRG, targThdId, targAttachedCtrGrpMask[targThdId]));

            if (targAttachedCtrGrpMask.size() > 0 && targAttachedCtrGrpMask[targThdId]) {
                return lastErr.PrintOrExit(BGPM_WALREADY_ATTACHED, BGPM_ERRLOC);
            }

            // If this eventset is configured the same as the target thread
            if (targThdId == GetTargThdId()) {

                BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " current CtrGrpMask=0x%02x\n", IND_STRG, CtrGrpMask()));

                assert(CtrGrpMask());
                if (!RsvAttachedCtrGrps(CtrGrpMask())) {
                    return lastErr.PrintOrExit(BGPM_ECTRS_IN_USE, BGPM_ERRLOC);
                }
                targPuList[targThdId] = puList[0];
                targAttachedCtrGrpMask[targThdId] = CtrGrpMask();
            }
            else {

                BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " targThdId(%d) != evtset targThdId(%d)\n", IND_STRG, targThdId, GetTargThdId()));

                // This seems kludgy, but what we are doing is generating a cloned swpunit from the
                // swpunit portion of the hwpunit evtset.  This will configure the punit for the target
                // and resolve any needed reservations for that target and events.
                unsigned emptyEvtList;
                ES_SWPunit *p = (ES_SWPunit*)((ES_SWPunit*)this->Clone(targThdId, &emptyEvtList, 0));
                if (p == NULL) {
                    return lastErr.PrintOrExit(BGPM_ENOMEM, BGPM_ERRLOC);
                }
                rc = p->GetCrtRC();
                if (rc < 0) {
                    delete p;
                    return lastErr.PrintOrExit(rc, BGPM_ERRLOC);
                }

                // See that something has not already attached to the thread counters in use
                assert(p->CtrGrpMask());
                BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " swEvtSet CtrGrpMask()=0x%02x\n", IND_STRG, p->CtrGrpMask()));
                BGPM_TRACE_DATA_L2(p->coreRsv.Dump(IND_LVL, targThdId/4));

                if (!p->RsvAttachedCtrGrps(p->CtrGrpMask())) {
                    delete p;
                    return lastErr.PrintOrExit(BGPM_ECTRS_IN_USE, BGPM_ERRLOC);
                }

                // now, let's transfer the ownership of the punit and reservations from the sw evtset
                // to this hardware evtset.
                TransferSwPunit(*p);
                delete p;
            }


            // Apply any new core wide attributes to to the punit
            assert(targPuList[targThdId]);
            unsigned core = targThdId/CONFIG_HWTHREADS_PER_CORE;
            unsigned allocMask = pShMem->coreRsv[core].allocMask;
            // ensure no coreRsv bits have been disabled since reserved at least.
            assert((allocMask & targAppliedRsvMask[targThdId]) == targAppliedRsvMask[targThdId]);
            unsigned newAttribs = (allocMask & ~targAppliedRsvMask[targThdId]);
            if (newAttribs) {  // new bits
                SetPunitCoreAttribs(targPuList[targThdId], newAttribs);
                targAppliedRsvMask[targThdId] |= newAttribs;
            }

            // Attach to hardware
            rc = Upci_Punit_Attach(targPuList[targThdId]);
            if (rc >= 0) {
                frozen = true;
                mbar();  // other threads can't access event set until frozen.
            }
            if (rc) {
                Detach();
                return lastErr.PrintOrExit(rc, BGPM_ERRLOC);
            }
        }
    }
    return 0;
}




int ES_HWPunit::Detach()
{
    BGPM_TRACE_L2;

    unsigned targHwThdId;
    for (targHwThdId=0; targHwThdId<CONFIG_MAX_HWTHREADS; targHwThdId++) {
        // Free up any attached punits
        if (targPuList[targHwThdId]) {
            unsigned core = targHwThdId/CONFIG_HWTHREADS_PER_CORE;
            unsigned thd = targHwThdId%CONFIG_HWTHREADS_PER_CORE;
            if (targAttachedCtrGrpMask[targHwThdId]) {
                pShMem->coreRsv[core].RlsAttachedCtrGrp(targAttachedCtrGrpMask[targHwThdId]);
                Upci_Punit_UnAttach(targPuList[targHwThdId]);
            }
            // leave original punit to be freed by SWPunit destructor
            // and free counter group and core reference only if target is different than
            // event sets thread.  Otherwise, the SWEvtSet destructor will free these.
            if (targPuList[targHwThdId] != puList[0]) {
                delete targPuList[targHwThdId];
                pShMem->coreRsv[core].RlsCtrGrp(thd);
                pShMem->coreRsv[core].DecrRef(thd);
            }
            targPuList[targHwThdId] = NULL;
        }
    }

    targAttachedCtrGrpMask.resize(0);
    targAppliedRsvMask.resize(0);

    return 0;
}





int ES_HWPunit::Start() {
    BGPM_TRACE_L2;
    Upci_Punit_t* pPunit = puList[0];
    assert(pPunit);

    BGPM_TRACE_DATA_L2(fprintf(stderr, "pPunit->status=0x%08x\n", pPunit->status));

#ifndef BGPM_FAST
    if (UNLIKELY(!(pPunit->status & UPCI_UnitStatus_Applied))) {
        return lastErr.PrintOrExit(BGPM_ENOT_APPLIED, BGPM_ERRLOC);
    }
#endif

    Upci_Punit_Start_Sync(pPunit, UPCI_CTL_DELAY);
    return 0;
}


int ES_HWPunit::Stop() {
    BGPM_TRACE_L2;
    Upci_Punit_t* pPunit = puList[0];
    assert(pPunit);

    Upci_Punit_Stop_Sync(pPunit, UPCI_CTL_DELAY);
    return 0;
}


int ES_HWPunit::ResetStart() { 

    ES_HWPunit::Reset();
    ES_HWPunit::Start();
    return 0;

 }

int ES_HWPunit::Reset() {
    BGPM_TRACE_L2;
    int rc=0;

    unsigned targThdId;
    for (targThdId=0; targThdId<CONFIG_MAX_HWTHREADS; ++targThdId) {
        if (targPuList[targThdId]) {
            rc = Upci_Punit_Attach(targPuList[targThdId]);
            if (rc >= 0) {
            frozen = true;
            mbar();  // other threads can't access event set until frozen.
            }
        }
    }
    return 0;
}


int ES_HWPunit::ReadEvent(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;
#ifndef BGPM_FAST
    assert(pVal);
    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
#endif

    unsigned targThdId;
    *pVal = 0;
    for (targThdId=0; targThdId<CONFIG_MAX_HWTHREADS; ++targThdId) {
        if (targPuList[targThdId]) {
            //fprintf(stderr, "%s" _AT_ " targThdId=%02d, puEvtIdx=%02d, *pVal=%ld\n", IND_STRG, targThdId, evtRecs[idx].puEvtIdx, *pVal);
            *pVal += Upci_Punit_Event_ReadIdx(targPuList[targThdId], evtRecs[idx].puEvtIdx);
        }
    }
    //fprintf(stderr, "%s" _AT_ "                         *pVal=%ld\n", IND_STRG, *pVal);

    mbar();

    return 0;
}



int ES_HWPunit::WriteEvent(unsigned idx, uint64_t val) {
    BGPM_TRACE_L2;
#ifndef BGPM_FAST
   if (UNLIKELY(idx >= evtRecs.size())) {
       return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
   }
#endif
   unsigned targThdId;
   for (targThdId=0; targThdId<CONFIG_MAX_HWTHREADS; ++targThdId) {
       if (targPuList[targThdId]) {
           Upci_Punit_Event_WriteIdx(targPuList[targThdId], evtRecs[idx].puEvtIdx, val);
       }
   }

   return 0;
}



int ES_HWPunit::ReadThreadEvent(unsigned idx, uint64_t appMask, uint64_t agentMask, uint64_t *pVal) {
    BGPM_TRACE_L2;
    int rc=0;
#ifndef BGPM_FAST
    assert(pVal);
    if (UNLIKELY((idx >= evtRecs.size())|| (idx < 0))) {
       rc=BGPM_EINV_EVT_IDX;
       return rc;
    }
#endif

    unsigned targThdId;
    *pVal = 0;
    for (targThdId=0; targThdId<CONFIG_MAX_HWTHREADS; ++targThdId) {
        if ((appMask & MASK64_bit(targThdId)) ||
            ((targThdId >= 64) && (agentMask & MASK64_bit(targThdId%64)))) {

            if (targPuList[targThdId]) {
                *pVal += Upci_Punit_Event_ReadIdx(targPuList[targThdId], evtRecs[idx].puEvtIdx);
            }
            else {
                rc = BGPM_WNOT_ATTACHED;
            }
        }
    }
    mbar();

    return rc;
}





int ES_HWPunit::Punit_GetHandles(Bgpm_Punit_Handles_t *pH){ return DefaultError(); }
int ES_HWPunit::Punit_GetHandles4Thread(int hwThd, Bgpm_Punit_Handles_t *pH){ return DefaultError(); }
int ES_HWPunit::Punit_GetLLHandles(Bgpm_Punit_LLHandles_t *pH){ return DefaultError(); }
void ES_HWPunit::SetEvtSetPunitInts(Upci_Punit_t *pPunit) {
    // Upci_Punit_EnablePMInt(&punit);  no interrupts for HW Mode
}

int ES_HWPunit::SetOverflowHandler(Bgpm_OverflowHandler_t handler) { return DefaultError(); }
int ES_HWPunit::SetOverflow(unsigned idx, uint64_t threshold) { return DefaultError(); }








EvtSet::ESTypes ES_HWDetailPunit::ESType() { BGPM_TRACE_L2; return ESType_HWDetailPunit; }
const char *ES_HWDetailPunit::ESTypeLabel() { return "Punit HW Detail"; }
//EvtSet *ES_HWDetailPunit::Spawn() { return new ES_HWDetailPunit(); }

EvtSet *ES_HWDetailPunit::Clone(uint64_t target, unsigned *addEvents, unsigned addNum)
{
    BGPM_TRACE_L2;
    if (UNLIKELY(target >= CONFIG_MAX_HWTHREADS)) {
        lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
        return NULL;
    }

    ES_SWPunit *p = new ES_HWDetailPunit((unsigned)target);
    if (p->GetCrtRC() >= 0) {
        p->MergeEventCfg(*this, (unsigned)target, addEvents, addNum);
    }

    return p;
}


