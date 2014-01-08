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
#include "ES_SWPunit.h"
#include "spi/include/upci/testlib/upci_debug.h"


//#define INCL_BTPROBE
#ifdef INCL_BTPROBE
#include "BackTraceMalloc.h"
#define BTPROBE(ptr) BTProbe(ptr, _AT_)
#else
#define BTPROBE(ptr)
#endif


using namespace bgpm;



ES_SWPunit::ES_SWPunit(unsigned targHwThdId)
: evtRecs(), numUserEvts(0), puList(), activePunitIdx(0),
  targCore(targHwThdId/CONFIG_HWTHREADS_PER_CORE), targHwThd(targHwThdId%CONFIG_HWTHREADS_PER_CORE),
  coreRsv(pShMem->coreRsv[targHwThdId/CONFIG_HWTHREADS_PER_CORE]),
  coreRsvMask(Alloc_Clear), appliedRsvMask(Alloc_Clear),
  ctrGrpRvd(false),
  frozen(false), running(false),
  pOvfHandler(NULL)
{
    BGPM_TRACE_L2;
    BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " targHwThd=%d, &coreRsv=0x%016lx\n", IND_STRG, targHwThd, (uint64_t)&coreRsv));
    BGPM_EXEMPT_DATA(fprintf(stderr, "%s" _AT_ "\n", IND_STRG); coreRsv.Dump(IND_LVL,targCore));

    coreRsv.IncrRef();  // Hold on to core reservations as long as this evtset exists.

    // Must at least be able to reserve base counters for this hwthread, otherwise
    // don't bother with the rest, won't be able to use the event set anyway.
    if (UNLIKELY(!coreRsv.RsvCtrGrp(targHwThd, targHwThd, true))) {
        crtRC = BGPM_ETHD_CTRS_RSV;
    }
    else {
        ctrGrpRvd = true;
    }
}



/**
 *
 * Most of the logic to combine an existing list of events and a set of one or more
 * new events occurs here.  Essentially, the existing list from the source event set
 * is duplicated into the new set, with the additional list of events added to the
 * end.  During the process, the events from both lists are combined and re-sorted by the
 * AddEventList method to give them the most efficient assignment order.
 * Attributes for the existing events are then duplicated.
 * We don't have to worry about attributes for new events, since they won't
 * have been assigned yet.
 */
int ES_SWPunit::MergeEventCfg(const ES_SWPunit &s, unsigned targHwThdId, unsigned *addEvents, unsigned addNum)
{
    BGPM_TRACE_L2;
    BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " targCore=%02d, targHwThd=%02d, &coreRsv=0x%016lx, addNum=%d\n", IND_STRG,
            targCore, targHwThd, (uint64_t)&coreRsv, addNum));
    BGPM_EXEMPT_DATA(fprintf(stderr, "%s" _AT_ "\n", IND_STRG); coreRsv.Dump(IND_LVL,targCore));

    assert(evtRecs.size() == 0);

    activePunitIdx = s.activePunitIdx;
    coreRsvMask = s.coreRsvMask;
    appliedRsvMask = Alloc_Clear;
    pOvfHandler = s.pOvfHandler;

    // Now - let's just repeat the operations which were used to build this source event set in
    // order to clone with the new events.

    // Duplicate Core Wide Attributes (to pre-reserve them if on a new core)
    // If we can't reserve core-wide attributes, there is no since in continuing.
    // We will duplicating individual event attributes later
    // and should end up with same results, but this allows us to catch conflicts in the
    // core wide values - which are recorded in the shared coreRsv object, not with each event
    if ((crtRC == 0) && (&coreRsv != &s.coreRsv) && coreRsvMask) {
        crtRC = coreRsv.DuplicateReservations(coreRsvMask, s.coreRsv);
    }

    // Duplicate Event Lists
    unsigned *eventList = NULL;
    if ((crtRC == 0) && (s.evtRecs.size() > 0)) {
        // populate a list of all events from both source set and new event list
        eventList = new unsigned[s.evtRecs.size()+addNum];
        for (unsigned i=0; i<s.evtRecs.size(); ++i) {
            eventList[i] = Upci_Punit_GetEventIdIdx(s.evtRecs[i].pPunit, s.evtRecs[i].puEvtIdx);
        }
        for (unsigned i=s.evtRecs.size(), iidx=0; iidx<addNum; ++i, ++iidx) {
            eventList[i] = addEvents[iidx];
        }
        // Now add events
        EvtSet *pEvtSet = NULL;
        crtRC = AddEventList(eventList, s.evtRecs.size()+addNum, pEvtSet);
        assert(pEvtSet == NULL);  // should be no change "this" event set as should be empty
    }

    // Duplicate event Attributes from source event set and use values from source punits.
    // Note: -- Context Attrib already duplicated during add of event.
    //       -- Core wide attributes are propagated to non-frozen punits during apply or attach.
    if ((crtRC == 0) && (s.evtRecs.size() > 0)) {

        for (unsigned evtIdx=0; evtIdx<s.evtRecs.size(); ++evtIdx) {

            PunitEvtRec & trgRec = evtRecs[evtIdx];
            const PunitEvtRec & srcRec = s.evtRecs[evtIdx];

            BGPM_TRACE_DATA_L2(
                    fprintf(stderr, "%s" _AT_ " evtIdx=%02d, evtId=%03d, srcAttrMask=0x%08x, \n",
                    IND_STRG, evtIdx, eventList[evtIdx], srcRec.evtAttrMask)
                    );

            if (srcRec.evtAttrMask) {  // any active attributes?
                trgRec.evtAttrMask = Or_EvtAttribs(trgRec.evtAttrMask, srcRec.evtAttrMask); // dup attribute flags

                // overflows
                if (IsAttribSet(trgRec.evtAttrMask, EvtAttr_Ovf)) {
                    uint64_t threshold = Upci_Punit_Event_GetThreshold(srcRec.pPunit, srcRec.puEvtIdx);
                    assert(threshold >= BGPM_THRESHOLD_MIN);  // double check that we are not reading a bad threshold
                    crtRC = Upci_Punit_Event_SetThreshold(trgRec.pPunit, trgRec.puEvtIdx, threshold);
                    assert(crtRC==0); // shouldn't fail or there is coding bug somewhere
                    #ifndef NDEBUG    // let's check that punit really has same threshold as sanity check (overkill??)
                    uint64_t chk = Upci_Punit_Event_GetThreshold(trgRec.pPunit, trgRec.puEvtIdx);
                    assert(chk == threshold);
                    #endif
                }
                // check override of default floating point counting setting
                if (IsAttribSet(trgRec.evtAttrMask, EvtAttr_FP)) {
                    UpciBool_t countOps = Upci_Punit_Get_OpsNotFlops(srcRec.pPunit, srcRec.puEvtIdx);
                    Upci_Punit_Set_OpsNotFlops(trgRec.pPunit, trgRec.puEvtIdx, countOps);
                    #ifndef NDEBUG    // let's check that punit really has same value as sanity check (overkill??)
                    UpciBool_t chk = Upci_Punit_Get_OpsNotFlops(trgRec.pPunit, trgRec.puEvtIdx);
                    assert(chk == countOps);
                    #endif
                }
                // check override of default edge or cycle setting
                if (IsAttribSet(trgRec.evtAttrMask,EvtAttr_Edge) ||
                         IsAttribSet(trgRec.evtAttrMask,EvtAttr_Cycle)) {
                    UpciBool_t edge = Upci_Punit_Get_Edge(srcRec.pPunit, srcRec.puEvtIdx);
                    Upci_Punit_Set_Edge(trgRec.pPunit, trgRec.puEvtIdx, edge);
                    #ifndef NDEBUG    // let's check that punit really has same value as sanity check (overkill??)
                    UpciBool_t chk = Upci_Punit_Get_Edge(trgRec.pPunit, trgRec.puEvtIdx);
                    assert(chk == edge);
                    #endif
                }
                // check override of default inversion
                if (IsAttribSet(trgRec.evtAttrMask, EvtAttr_Invert)) {
                    UpciBool_t invert = Upci_Punit_Get_Invert(srcRec.pPunit, srcRec.puEvtIdx);
                    Upci_Punit_Set_Invert(trgRec.pPunit, trgRec.puEvtIdx, invert);
                    #ifndef NDEBUG    // let's check that punit really has same value as sanity check (overkill??)
                    UpciBool_t chk = Upci_Punit_Get_Invert(trgRec.pPunit, trgRec.puEvtIdx);
                    assert(chk == invert);
                    #endif
                }
               //Check Overrides of default XuGroup mask
                 if (IsAttribSet(trgRec.evtAttrMask, EvtAttr_XuGrpMask)) {
                    uint64_t XuGrpMask = UPC_P__COUNTER_CFG__XU_OPGROUP_SEL_get(Upci_Punit_Get_OpcodeGrpMask(srcRec.pPunit, srcRec.puEvtIdx));
                    Upci_Punit_Set_OpcodeGrpMask(trgRec.pPunit, trgRec.puEvtIdx,UPC_P__COUNTER_CFG__XU_OPGROUP_SEL_set(XuGrpMask));
                    #ifndef NDEBUG    // let's check that punit really has same value as sanity check (overkill??)
                    uint64_t chk = UPC_P__COUNTER_CFG__XU_OPGROUP_SEL_get(Upci_Punit_Get_OpcodeGrpMask(trgRec.pPunit, trgRec.puEvtIdx));
                    assert(chk == XuGrpMask);
                    #endif
                }
               //Check Overrides of default QfpuGrpMask  mask
               if (IsAttribSet(trgRec.evtAttrMask, EvtAttr_QfpuGrpMask)) {
                    uint64_t QfpuGrpMask = UPC_P__COUNTER_CFG__AXU_OPGROUP_SEL_get(Upci_Punit_Get_OpcodeGrpMask(srcRec.pPunit, srcRec.puEvtIdx));
                    Upci_Punit_Set_OpcodeGrpMask(trgRec.pPunit, trgRec.puEvtIdx,UPC_P__COUNTER_CFG__AXU_OPGROUP_SEL_set(QfpuGrpMask));
                    #ifndef NDEBUG    // let's check that punit really has same value as sanity check (overkill??)
                    uint64_t chk = UPC_P__COUNTER_CFG__AXU_OPGROUP_SEL_get(Upci_Punit_Get_OpcodeGrpMask(trgRec.pPunit, trgRec.puEvtIdx));
                    assert(chk == QfpuGrpMask);
                    #endif
                }

                BGPM_TRACE_DATA_L2(
                        fprintf(stderr, "%s" _AT_ " evtIdx=%02d, evtId=%03d, srcAttrMask=0x%08x, trgAttrMask=0x%08x\n",
                        IND_STRG, evtIdx, eventList[evtIdx], srcRec.evtAttrMask, trgRec.evtAttrMask)
                        );

            }
        }
    }
    if (eventList != NULL) {
        delete[] eventList;
    }


    return crtRC;
}



ES_SWPunit::~ES_SWPunit()
{
    BGPM_TRACE_L2;

    for (int i = puList.size()-1; i>=0; i--) {
        if (puList[i]) {
            BTPROBE( puList[i] );
            delete puList[i];
        }
    }

    if (ctrGrpRvd) {
        coreRsv.RlsCtrGrp(targHwThd);
    }
    coreRsv.DecrRef(targHwThd);
    //fprintf(stderr, "%s" _AT_ " targHwThdId=%d\n", IND_STRG, GetTargThdId());
};



EvtSet::ESTypes ES_SWPunit::ESType() { return ESType_SWPunit; }
const char *ES_SWPunit::ESTypeLabel() { return "Punit SW Thread"; }
//EvtSet *ES_SWPunit::Spawn() { return new ES_SWPunit(); }
bool ES_SWPunit::IsEventIdCompatible(int evtId) {
    return ((evtId > 0) && (evtId <= PEVT_PUNIT_LAST_EVENT));
}


EvtSet *ES_SWPunit::Clone(uint64_t target, unsigned *addEvents, unsigned addNum)
{
    BGPM_TRACE_L2;
    if (UNLIKELY(target >= CONFIG_MAX_HWTHREADS)) {
        lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
        return NULL;
    }

    ES_SWPunit *p = new ES_SWPunit((unsigned)target);
    if (p->GetCrtRC() >= 0) {
        p->MergeEventCfg(*this, (unsigned)target, addEvents, addNum);
    }
    return p;
}



int ES_SWPunit::AddEvent(unsigned evtId, EvtSet* & pNewEvtSet)
{
    BGPM_TRACE_L2;

    unsigned evtIds[1];
    evtIds[0] = evtId;

    int rc = AddEventList(evtIds, 1, pNewEvtSet);
    return rc;
}



void ES_SWPunit::GetBestAddOrder(unsigned *evtIds, unsigned num, std::vector<unsigned> & idxs)
{
    BGPM_TRACE_L2;

    unsigned i;
    // Look for Threaded L1p or Wakeup Events
    for (i=0; i<num; ++i) {
        UPC_EventSources_t evtSrc = punitEvtTbl[evtIds[i]].evtSrc;
        if (punitEvtTbl[evtIds[i]].scope == UPC_SCOPE_THREAD) {
            if ((evtSrc >= UPC_ES_L1P_BASE) && (evtSrc <= UPC_ES_WAKEUP)) {
                idxs.push_back(i);
            }
        }
        if ((evtSrc < UPC_ES_A2_MMU) || (evtSrc > UPC_ES_OP_ALL)) {
            assert(0); // let's make sure event is a punit event
        }
    }
    // core wide L1p or Wakeup events (cause if expanded into other thread counters can use up to 4)
    for (i=0; i<num; ++i) {
        UPC_EventSources_t evtSrc = punitEvtTbl[evtIds[i]].evtSrc;
        if (punitEvtTbl[evtIds[i]].scope != UPC_SCOPE_THREAD) {
            if ((evtSrc >= UPC_ES_L1P_BASE) && (evtSrc <= UPC_ES_WAKEUP)) {
                idxs.push_back(i);
            }
        }
    }
    // XU Events
    for (i=0; i<num; ++i) {
        UPC_EventSources_t evtSrc = punitEvtTbl[evtIds[i]].evtSrc;
        if (evtSrc == UPC_ES_A2_XU) {
            idxs.push_back(i);
        }
    }
    // Other A2 Events
    for (i=0; i<num; ++i) {
        UPC_EventSources_t evtSrc = punitEvtTbl[evtIds[i]].evtSrc;
        if ((evtSrc >= UPC_ES_A2_MMU) && (evtSrc <= UPC_ES_A2_LSU)) {
            idxs.push_back(i);
        }
    }
    // Opcode Events
    for (i=0; i<num; ++i) {
        UPC_EventSources_t evtSrc = punitEvtTbl[evtIds[i]].evtSrc;
        if ((evtSrc >= UPC_ES_OP_XU) && (evtSrc <= UPC_ES_OP_ALL)) {
            idxs.push_back(i);
        }
    }
}



// Don't handle a l1p reservation error message on failure - let the caller do this
int ES_SWPunit::ConfirmL1pCoreReservation(unsigned evtId)
{
    const UPC_PunitEvtTableRec_t * pEvent = Upci_GetPunitEventRec(evtId);
    UPC_EventSources_t evtSrc = pEvent->evtSrc;
    if (UNLIKELY((evtSrc >= UPC_ES_L1P_SWITCH) && (evtSrc <= UPC_ES_L1P_LIST))) {
        if (UNLIKELY(!coreRsv.RsvL1pMode(evtSrc))) {
            return BGPM_EL1PMODERESERVED;
        }
        coreRsvMask |= Alloc_l1pMode;
    }
    return 0;
}



// call Upci routines to reserve events.
// Note: Don't handle a punit reservation error message on failure - let the caller do this
// Hmmm : code is assuming that the passed in puEvtIdx is the next available entry in the Punit
//        object - thus the punit index assigned will end up equal (an ugly, but safe, dependency).
int ES_SWPunit::ReserveEvent(int listIdx, unsigned evtId, Upci_Punit_t *pPunit, unsigned puEvtIdx)
{
    BGPM_TRACE_L2;

    Upci_Punit_EventH_t hEvt;

    // make sure l1p mode on core agrees with event
    int rc = ConfirmL1pCoreReservation(evtId);

    // if event source is not A2, then validate that core context is set to both
    if (!Upci_IsA2Event(evtId)) {
        //fprintf(stderr, "%s" _AT_ " coreRsv: ctrGrpMask(0x%03x), evtCtx(0x%02x)\n", IND_STRG,
        //        coreRsv.allocMask, coreRsv.evtCtx);
        if (coreRsv.RsvContext(BGPM_CTX_BOTH)) {
            coreRsvMask |= Alloc_evtCtxBoth;
        }
        else if (!coreRsv.AllowMixedContext()) {
            rc = BGPM_EEVT_CTX_MISMATCH;
        }
    }

    // now add the event.
    if (rc == 0) {
        rc = Upci_Punit_AddEventAtIdx(pPunit, (Upci_EventIds)evtId, targHwThd, -1, listIdx,
                coreRsv.ctrGrpMask[targHwThd], &hEvt);
    }

    // update evtRecs with new event
    if (rc == 0) {
        evtRecs[listIdx].puEvtIdx = puEvtIdx;
        evtRecs[listIdx].pPunit = pPunit;
    }

    BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " addevent rc=%d, evtId=%d, puEvtIdx=%d, targHwThd=%d, ctrGrpMask=0x%02x, coreRsvMask=0x%08x\n", IND_STRG,
            rc, evtId, puEvtIdx, targHwThd, coreRsv.ctrGrpMask[targHwThd], coreRsvMask));

    return rc;
}



Upci_Punit_t *ES_SWPunit::AllocateFreshPunit()
{
    // start with 1 blank punit
    Upci_Punit_t *pPunit = new Upci_Punit_t();
    Upci_Punit_Init(pPunit, &(pShMem->upciMode), targCore);

    // for debug - Reserve any counters and signals which have been prereserved on core
    // Test should use undocumented function Bgpm_SetPreReserved(unsigned hEvtSet)
    if (coreRsv.preRsvSignals[targHwThd] || coreRsv.preRsvCtrs[targHwThd]) {
        Upci_Punit_SkipCtrs(pPunit, coreRsv.preRsvSignals[targHwThd], coreRsv.preRsvCtrs[targHwThd]);
    }
    return pPunit;
}



int ES_SWPunit::AddEventList(unsigned *evtIds, unsigned num, EvtSet* & pNewEvtSet)
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



    pNewEvtSet = NULL;

    // Otherwise, AddEventList always needs to start with a fresh list of events to add and
    // a fresh Punit

    // toss any existing punits - in case of prior failure with this event set.
    int idx;
    for (idx=puList.size()-1; idx>=0; --idx) {
        if (puList[idx]) delete puList[idx];
    }
    puList.resize(0);


    Upci_Punit_t *pPunit = AllocateFreshPunit();
    puList.push_back(pPunit);
    assert(puList[0]);


    // toss any existing (possibly empty) events in case of prior failure.
    evtRecs.resize(0);
    // Pre-add entries for each new event
    evtRecs.resize(num);


    // Get add order for new events (need to add most to least retrictive)
    std::vector<unsigned> addOrder;
    addOrder.reserve(num);

    #ifdef INCL_BTPROBE
    // <<<<<<<<<<<<<<<<<<<<<<
    //fprintf(stderr, _AT_ " @addOrder=%p, @@addOrder=%p\n", (void*)&addOrder, *((void**)&addOrder));
    void *pAddOrderData = *((void**)&addOrder);
    BTPROBE( pAddOrderData );
    // <<<<<<<<<<<<<<<<<<<<<<
    #endif

    GetBestAddOrder(evtIds, num, addOrder);
    assert(addOrder.size() == num);

    #ifdef INCL_BTPROBE
    // <<<<<<<<<<<<<<<<<<
    uint64_t chksum1 = BTChkSum(pAddOrderData);
    BTPROBE( *((void**)&addOrder) );
    uint64_t chksum2 = BTChkSum(pAddOrderData);
    BTChkSumAbort(chksum1, chksum2, _AT_);
    // <<<<<<<<<<<<<<<<<<<<<<
    #endif


    // make sure that current threads affinity counter group is available.
    if (UNLIKELY(!coreRsv.RsvCtrGrp(targHwThd, targHwThd, false))) {
        evtRecs.resize(0);  // make sure empty on failure (in case someone tries to deref events)
        return lastErr.PrintOrExit(BGPM_ETHD_CTRS_RSV, BGPM_ERRLOC);
    }
    //fprintf(stderr, "%s" _AT_ " coreRsv.ctrGrpRefCount=%d\n", IND_STRG, coreRsv.ctrGrpRefCount[targHwThd]);  // <<<<<<<<<<<<

    // Add events until done or failure.
    unsigned i = 0;
    int rc = 0;
    while ((rc == 0) && (i<addOrder.size())) {
        unsigned listIdx = addOrder[i];
        assert(listIdx < num);
        rc = ReserveEvent(listIdx, evtIds[listIdx], pPunit, i);
        if (rc == 0) i++;
    }

    // If we failed - expand into other thread's counters and retry.
    // If out of counters or signals and failure is on an L1p or Wakeup threaded event,
    // there is no need to retry by expanding into other counters.
    // L1p/Wakeup can't use other counters than the initial thread affinity anyway.
    if ((rc == BGPM_ECTRRESERVED) || (rc == BGPM_ESIGRESERVED) || (rc == BGPM_ECSEVT_REQALLTHDS)) {
        const UPC_PunitEvtTableRec_t * pEvent = Upci_GetPunitEventRec((Upci_EventIds)evtIds[addOrder[i]]);
        if ((pEvent->scope == UPC_SCOPE_THREAD) &&
            (pEvent->evtSrc >= UPC_ES_L1P_BASE) && (pEvent->evtSrc <= UPC_ES_WAKEUP)) {

            BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " addevent rc=%d for l1p or wakeup events - no need to expand into sibling threads\n",
                    IND_STRG, rc));

            DumpEventsInError(pPunit, rc, (Upci_EventIds)evtIds[addOrder[i]]);
            evtRecs.resize(0);  // make sure empty on failure.
            return lastErr.PrintOrExit(rc, BGPM_ERRLOC);
        }
    }


    // We might exhaust the 6 upc_p counters with the current threads "affinity",
    // And, we want to support using more counters in case the client is counting
    // on less than 4 threads per core.  So:
    // if the 1st attempt failed, let's try and expand counters into the sibling affinity group
    if ((rc == BGPM_ECTRRESERVED) || (rc == BGPM_ESIGRESERVED) || (rc == BGPM_ECSEVT_REQALLTHDS)) {

        BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " addevent rc=%d - 1 thread exhausted - try to expand into sibling thread counters\n",
                IND_STRG, rc));

        uint8_t priorAffMask = coreRsv.ctrGrpMask[targHwThd];
        if (!coreRsv.RsvCtrGrp(Upci_Rsv_SiblingThread(targHwThd), targHwThd, false)) {
            DumpEventsInError(pPunit, rc, (Upci_EventIds)evtIds[addOrder[i]]);
            evtRecs.resize(0);  // make sure empty on failure.
            return lastErr.PrintOrExit(rc, BGPM_ERRLOC);
        }
        // retry adding events here only if affinity mask has changed.
        // otherwise, there no sense as the 1st pass already tried the expanded counters.
        if (priorAffMask != coreRsv.ctrGrpMask[targHwThd]) {
            rc = 0;
            while ((rc == 0) && (i<addOrder.size())) {
                unsigned listIdx = addOrder[i];
                assert(listIdx < num);
                rc = ReserveEvent(listIdx, evtIds[listIdx], pPunit, i);
                if (rc == 0) i++;
            }
        }
    }


    // for the last attempt - let's try all the counters if not being used by other threads.
    // Only opcode events can actually expand into the rest of the counters.
    if ((rc == BGPM_ECTRRESERVED) || (rc == BGPM_ESIGRESERVED) || (rc == BGPM_ECSEVT_REQALLTHDS)) {

        BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " addevent rc=%d - last try to expand to all thread counters\n",
                IND_STRG, rc));

        uint8_t priorAffMask = coreRsv.ctrGrpMask[targHwThd];
        unsigned otherThd = (targHwThd + 2) % 4;  // 0->2, 1->3 and visa-versa.
        if (!coreRsv.RsvCtrGrp(otherThd, targHwThd, false)) {
            DumpEventsInError(pPunit, rc, (Upci_EventIds)evtIds[addOrder[i]]);
            evtRecs.resize(0);  // make sure empty on failure.
            return lastErr.PrintOrExit(rc, BGPM_ERRLOC);
        }
        if (!coreRsv.RsvCtrGrp(Upci_Rsv_SiblingThread(otherThd), targHwThd, false)) {
            DumpEventsInError(pPunit, rc, (Upci_EventIds)evtIds[addOrder[i]]);
            evtRecs.resize(0);  // make sure empty on failure.
            return lastErr.PrintOrExit(rc, BGPM_ERRLOC);
        }
        // retry adding events here only if affinity mask has changed (otherwise no sense)
        if (priorAffMask != coreRsv.ctrGrpMask[targHwThd]) {
            rc = 0;
            while ((rc == 0) && (i<addOrder.size())) {
                unsigned listIdx = addOrder[i];
                assert(listIdx < num);
                rc = ReserveEvent(listIdx, evtIds[listIdx], pPunit, i);
                if (rc == 0) i++;
            }
        }
    }


    // If we still failed, we've tried all combination of counters - time to give up.
    if (rc < 0) {
        DumpEventsInError(pPunit, rc, (Upci_EventIds)evtIds[addOrder[i]]);
        evtRecs.resize(0);  // make sure empty on failure.
        return lastErr.PrintOrExit(rc, BGPM_ERRLOC);
    }

    SetEvtSetPunitInts(pPunit);  // activate PM interrupts

    BTPROBE( pAddOrderData );


    // now that the events have been added, let's sanity check that the number
    // of added events is equivalent, and that the added events are contiguous (no holes)
    #ifndef NDEBUG
    for (unsigned i=0; i<num; ++i) {
        assert(evtRecs[i].pPunit); // punit pointer actually assigned.
    }
    #endif


    #ifdef INCL_BTPROBE
    // <<<<<<<<<<<<<<<<<<<<<<<
    uint64_t tb = GetTimeBase();
    tb &= 0xFFFFLL;
    Upci_Delay(tb);
    BTPROBE( *((void**)&addOrder) );

    chksum2 = BTChkSum(pAddOrderData);
    BTChkSumAbort(chksum1, chksum2, _AT_);
    #endif

    return 0;
}



unsigned ES_SWPunit::EvtRecIdx(Upci_Punit_t *pPunit, unsigned unitEvtIdx)
{
    unsigned i = 0;
    while (i < evtRecs.size()) {
        //fprintf(stderr, "%s" _AT_ " unitEvtIdx=%d, pPunit=0x%016lx, puEvtIdx=%d\n", IND_STRG,
        //        unitEvtIdx, (uint64_t)evtRecs[i].pPunit, evtRecs[i].puEvtIdx);

        if ((evtRecs[i].pPunit == pPunit) && (evtRecs[i].puEvtIdx == unitEvtIdx)) {
            return i;
        }
        i++;
    }
    return ES_EVTRECIDX_NOTFOUND;  // normal return, not a lastErr return
}



// Fill a preallocated Feedback buffer with the current signal and counter allocations
// for this event set.
// \todo This routine is very invasive to fields and knowledge which is the role of the spi
//       punit.  The operations could be pushed down to that level.  However, the
//       problem is we use libc functions which are not available in firmware, so some
//       packaging considerations would need to be worked out.
bool ES_SWPunit::FillFeedbackBuffer(char *buf, size_t bufSize, Upci_Punit_t *pPunit, unsigned failedEvtId, int muxGrp)
{
    assert(buf);
    assert(pPunit);
    assert(pShMem != NULL);
    assert(bufSize > 40); // need to minimally fit OUTOFSPACEMSG

    #define OUTOFSPACEMSG "... need larger feedback buffer\n"
    #define RETURN_IF_NO_SPACE \
        if (UNLIKELY((bufPos+pos) >= ((int)bufSize-40))) { \
            bufPos+=snprintf(&buf[bufPos], 40, OUTOFSPACEMSG); \
            return false; \
        }

    buf[0] = '\0';
    int bufPos = 0;

    // calculate size for event label field.
    unsigned idx = 0;
    unsigned lblWidth = 21;
    for (idx=0; idx<pPunit->numEvts; ++idx) {
        lblWidth = UPCI_MAX(lblWidth, strlen(pPunit->evts[idx].pEvent->label));
    }
    lblWidth = UPCI_MAX(lblWidth, strlen(Upci_GetEventLabel(failedEvtId)));
    #define lblFormatSize 14
    char lblFormat[lblFormatSize];
    snprintf(lblFormat, lblFormatSize, "%%-%ds ", lblWidth);

    // print field headers
    int pos = 0;
    #define olineSize 1024
    char oline[olineSize];
    pos += snprintf(&oline[pos], olineSize-pos, lblFormat, "");
    pos += snprintf(&oline[pos], olineSize-pos, "%38s ",  "");
    pos += snprintf(&oline[pos], olineSize-pos, "%-26s ", "-------- Counters --------");
    pos += snprintf(&oline[pos], olineSize-pos, "\n");
    //fprintf(stderr, "bufPos=%d, pos=%d, bufSize=%ld\n", bufPos, pos, bufSize);
    RETURN_IF_NO_SPACE;
    bufPos += snprintf(&buf[bufPos], bufSize-bufPos, oline);

    pos = 0;
    if (muxGrp < 0) {
        pos += snprintf(&oline[pos], olineSize-pos, lblFormat, "");
    }
    else {
        char muxStrg[25];
        snprintf(muxStrg, 25, "***MuxGrp[%d]***", muxGrp);
        pos += snprintf(&oline[pos], olineSize-pos, lblFormat, muxStrg);
    }
    pos += snprintf(&oline[pos], olineSize-pos, "%23s ",  "");
    pos += snprintf(&oline[pos], olineSize-pos, "%-8s ", "UnitSigs");
    pos += snprintf(&oline[pos], olineSize-pos, "%5s ",  "Ctrs");
    pos += snprintf(&oline[pos], olineSize-pos, "%-26s ", "00000000 00111111 11112222");
    pos += snprintf(&oline[pos], olineSize-pos, "\n");
    RETURN_IF_NO_SPACE;
    bufPos += snprintf(&buf[bufPos], bufSize-bufPos, oline);

    pos = 0;
    pos += snprintf(&oline[pos], olineSize-pos, lblFormat, "EvtLabel");
    pos += snprintf(&oline[pos], olineSize-pos, "%6s ",  "EvtIdx");
    pos += snprintf(&oline[pos], olineSize-pos, "%8s ",  "PuEvtIdx");
    pos += snprintf(&oline[pos], olineSize-pos, "%-7s ", "Unit");
    pos += snprintf(&oline[pos], olineSize-pos, "%-8s ", "01234567");
    pos += snprintf(&oline[pos], olineSize-pos, "%5s ",  "Avail");
    pos += snprintf(&oline[pos], olineSize-pos, "%-26s ", "01234567 89012345 67890123");
    pos += snprintf(&oline[pos], olineSize-pos, "\n");
    RETURN_IF_NO_SPACE;
    bufPos += snprintf(&buf[bufPos], bufSize-bufPos, oline);

    // Show counters reserved by other core threads
    unsigned otherThd;
    for (otherThd=0; otherThd<CONFIG_HWTHREADS_PER_CORE; ++otherThd) {
        uint8_t affMask = coreRsv.ctrGrpMask[otherThd];
        if (affMask) {
            unsigned thd;
            uint32_t ctrMask = 0;
            for (thd=0; thd<CONFIG_HWTHREADS_PER_CORE; thd++) {
                if (affMask & (0x80 >> thd)) {
                    ctrMask |= (0x88888800 >> thd);
                }
            }
            char thdLbl[15];
            snprintf(thdLbl, 15, "hwThread[%d]", otherThd);

            pos = 0;
            pos += snprintf(&oline[pos], olineSize-pos, lblFormat, thdLbl);
            pos += snprintf(&oline[pos], olineSize-pos, "%6s ", "-");
            pos += snprintf(&oline[pos], olineSize-pos, "%8s ", "-");
            pos += snprintf(&oline[pos], olineSize-pos, "%-7s ", "-");
            pos += snprintf(&oline[pos], olineSize-pos, "%-8s ", "--------");
            int maxCtrs = upc_popcnt32( ctrMask );
            pos += snprintf(&oline[pos], olineSize-pos, "%5d ", maxCtrs);
            int ctr = otherThd == targHwThd ? -3 : -2;  // -2 shows all reserved, -3, none.
            pos += snprintf(&oline[pos], olineSize-pos, "%-26s ", Upci_Punit_Reserve_CtrMaskStrg(ctr, ctrMask));
            pos += snprintf(&oline[pos], olineSize-pos, "\n");
            RETURN_IF_NO_SPACE;
            bufPos += snprintf(&buf[bufPos], bufSize-bufPos, oline);
        }
    }

    // Show unit signal to counter mapping
    pos = 0;
    pos += snprintf(&oline[pos], olineSize-pos, lblFormat, "UnitSignals->Counters");
    pos += snprintf(&oline[pos], olineSize-pos, "%23s ",  "");
    pos += snprintf(&oline[pos], olineSize-pos, "%-8s ", "");
    pos += snprintf(&oline[pos], olineSize-pos, "%5s ",  "");
    pos += snprintf(&oline[pos], olineSize-pos, "%-26s ", "02461357 02461357 02461357");
    pos += snprintf(&oline[pos], olineSize-pos, "\n");
    RETURN_IF_NO_SPACE;
    bufPos += snprintf(&buf[bufPos], bufSize-bufPos, oline);


    // Print field if there are prereserved counters
    if (coreRsv.preRsvCtrs[targHwThd] || coreRsv.preRsvSignals[targHwThd]) {
        UPC_EventSources_t srcs[] = { UPC_ES_A2_MMU, UPC_ES_A2_AXU, UPC_ES_A2_IU, UPC_ES_A2_LSU, UPC_ES_A2_XU };
        for (int i=0; i<5; ++i) {
            pos = 0;
            pos += snprintf(&oline[pos], olineSize-pos, lblFormat, "prereserved");
            pos += snprintf(&oline[pos], olineSize-pos, "%6s ", "-");
            pos += snprintf(&oline[pos], olineSize-pos, "%8s ", "-");
            pos += snprintf(&oline[pos], olineSize-pos, "%-7s ", Upci_GetEventSrcName(srcs[i]));
            pos += snprintf(&oline[pos], olineSize-pos, "%-8s ", Upci_Punit_Reserve_SigMaskStrgAll(srcs[i], coreRsv.preRsvSignals[targHwThd]));
            pos += snprintf(&oline[pos], olineSize-pos, "%5s ", "-");
            pos += snprintf(&oline[pos], olineSize-pos, "%-26s ", Upci_Punit_Reserve_CtrMaskStrg(-2, coreRsv.preRsvCtrs[targHwThd]));
            pos += snprintf(&oline[pos], olineSize-pos, "\n");
            RETURN_IF_NO_SPACE;
            bufPos += snprintf(&buf[bufPos], bufSize-bufPos, oline);
        }
    }

    // Print fields for each successfully allocated event
    for (idx=0; idx<pPunit->numEvts; ++idx) {
        const UPC_PunitEvtTableRec_t * pEvent = pPunit->evts[idx].pEvent;
        const Upci_PunitEventRec_t * pEvtRec = &(pPunit->evts[idx]);
        pos = 0;
        pos += snprintf(&oline[pos], olineSize-pos, lblFormat, pEvent->label);
        pos += snprintf(&oline[pos], olineSize-pos, "%6d ",  EvtRecIdx(pPunit, idx));
        pos += snprintf(&oline[pos], olineSize-pos, "%8d ",  idx);
        pos += snprintf(&oline[pos], olineSize-pos, "%-7s ", Upci_GetEventSrcName(pEvent->evtId));
        uint32_t ctrMask = Upci_Punit_Reserve_PossibleCtrMask(&(pPunit->rsv), pEvtRec->hwThread, pEvent);
        uint8_t  sigMask = Upci_Punit_Reserve_PossibleA2UnitSignalMask(ctrMask);
        pos += snprintf(&oline[pos], olineSize-pos, "%-8s ",
                Upci_Punit_Reserve_SigMaskStrg(pEvtRec->evtSrc, pEvtRec->a2_Sig, sigMask));
        int maxCtrs = upc_popcnt32( ctrMask );
        pos += snprintf(&oline[pos], olineSize-pos, "%5d ", maxCtrs);
        pos += snprintf(&oline[pos], olineSize-pos, "%-26s ", Upci_Punit_Reserve_CtrMaskStrg(pEvtRec->p_Ctr, ctrMask));
        pos += snprintf(&oline[pos], olineSize-pos, "\n");
        RETURN_IF_NO_SPACE;
        bufPos += snprintf(&buf[bufPos], bufSize-bufPos, oline);
    }

    // add failed event to log
    if (failedEvtId > 0) {
        pos = 0;
        pos += snprintf(&oline[pos], olineSize-pos, lblFormat, Upci_GetEventLabel(failedEvtId));
        pos += snprintf(&oline[pos], olineSize-pos, "%6s ", "-");
        pos += snprintf(&oline[pos], olineSize-pos, "%8s ", "-");
        pos += snprintf(&oline[pos], olineSize-pos, "%-7s ", Upci_GetEventSrcName(failedEvtId));
        uint32_t ctrMask = Upci_Punit_Reserve_PossibleCtrMask(&(pPunit->rsv), pPunit->evts[0].hwThread,
                                                              Upci_GetPunitEventRec(failedEvtId));
        uint8_t  sigMask = Upci_Punit_Reserve_PossibleA2UnitSignalMask(ctrMask);
        pos += snprintf(&oline[pos], olineSize-pos, "%-8s ", Upci_Punit_Reserve_SigMaskStrg(Upci_GetEventSrc(failedEvtId), -1, sigMask));
        int maxCtrs = upc_popcnt32( ctrMask );
        pos += snprintf(&oline[pos], olineSize-pos, "%5d ", maxCtrs);
        pos += snprintf(&oline[pos], olineSize-pos, "%-26s ", Upci_Punit_Reserve_CtrMaskStrg(-1, ctrMask));
        pos += snprintf(&oline[pos], olineSize-pos, "\n");
        RETURN_IF_NO_SPACE;
        bufPos += snprintf(&buf[bufPos], bufSize-bufPos, oline);
    }
    return true;
}



void ES_SWPunit::PrintLines(char *buf, size_t bufSize) {
    char *ostrg = strndup(buf, bufSize);
    char *tokTrack = NULL;
    char *line = strtok_r(ostrg, "\n", &tokTrack);
    while (line) {
        fprintf(stderr, "%s\n", line);
        line = strtok_r(NULL, "\n", &tokTrack);
    }
    free(ostrg);
}


// FIX: This is invasive to the punit contents - probably should have abstracted some spi functions instead.
int ES_SWPunit::DumpEventsInError(Upci_Punit_t *pPunit, int rc, unsigned failedEvtId)
{
    assert(pShMem != NULL);
    assert(pPunit);

    //fprintf(stderr, "%s" _AT_ " printOnError=%d, rc=%d, feedback=%d, evtFeedback=%d, failedEvtId=%d\n", IND_STRG, printOnError, rc, addEventErrFeedback, pShMem->evtFeedback, failedEvtId);

    if (printOnError == 0) {
        return 0;
    }

    if (addEventErrFeedback == BGPM_EVT_FEEDBACK_NONE) {
        return 0;
    }
    else if (addEventErrFeedback == BGPM_EVT_FEEDBACK_PROCLEADER) {
        int pid = getpid();
        int tid = gettid();
        BGPM_TRACE_DATA_L1(fprintf(stderr, "pid=%d, tid=%d\n", pid, tid));
        if (pid != tid) {
            return 0;
        }
    }

    // Let's not print if cause was Core-Shared conflict
    if (rc == BGPM_ECSEVT_REQALLTHDS) {
        return 0;
    }

    ShMemLock guard(&(pShMem->refLock));
    if ((addEventErrFeedback == BGPM_EVT_FEEDBACK_1ST) && (pShMem->evtFeedback)) {
        return 0;
    }
    pShMem->evtFeedback = true;


    char buf[BGPM_FEEDBACK_BUFSIZE];
    if (puList.size() > 1) {
        for (unsigned i=0; i<puList.size(); ++i) {
            FillFeedbackBuffer(buf, BGPM_FEEDBACK_BUFSIZE, pPunit, failedEvtId, i);
            PrintLines(buf, BGPM_FEEDBACK_BUFSIZE);
        }
    }
    else if (puList.size() == 1) {
        FillFeedbackBuffer(buf, BGPM_FEEDBACK_BUFSIZE, pPunit, failedEvtId);
        PrintLines(buf, BGPM_FEEDBACK_BUFSIZE);
    }

    return 0;
}



int ES_SWPunit::SetPreReserved()
{
    if ((coreRsv.preRsvSignals[targHwThd] || coreRsv.preRsvCtrs[targHwThd])) {
        return lastErr.PrintOrExit(BGPM_EPRERSV_EXISTS, BGPM_ERRLOC);
    }
    else {
        coreRsv.preRsvSignals[targHwThd] = Upci_Punit_GetUsedSignalMask(puList[0]);
        coreRsv.preRsvCtrs[targHwThd] = Upci_Punit_GetUsedCtrMask(puList[0]);
    }
    return 0;
}



int ES_SWPunit::NumEvents() const { return (int)evtRecs.size(); }



// This is invasive to the punit contents - probably should have created spi functions instead.
void ES_SWPunit::SetPunitCoreAttribs(Upci_Punit_t *pPunit, unsigned coreRsvMask)
{
    coreRsvMask &= ~Alloc_l1pMode;  // l1p assigned by punit based on event types, nothing extra to do.
    if (coreRsvMask) {
        if (coreRsvMask & Alloc_qfpuMatch) {
            pPunit->cfg.opCodeMatch |= ((uint64_t)coreRsv.qfpuMatch) << 16 | coreRsv.qfpuMask;
            pPunit->cfg.fpWeights |= UPC_P__CONFIG__AXU_MATCH_WEIGHT_set((uint64_t)coreRsv.qfpuScale);
        }
        if (coreRsvMask & Alloc_xuMatch) {
            pPunit->cfg.opCodeMatch |= ((uint64_t)coreRsv.xuMatch) << 48 | ((uint64_t)coreRsv.xuMask) << 32;
        }
        if (coreRsvMask & Alloc_sqrtScale) {
            pPunit->cfg.fpWeights |= UPC_P__CONFIG__SQRT_WEIGHT_set((uint64_t)coreRsv.sqrtScale);
        }
        if (coreRsvMask & Alloc_divScale) {
            pPunit->cfg.fpWeights |= UPC_P__CONFIG__DIV_WEIGHT_set((uint64_t)coreRsv.divScale);
        }
        if (coreRsvMask & Alloc_evtCtx) {
            Upci_Punit_Cfg_SetContext(&(pPunit->cfg), coreRsv.Context2Dmn(coreRsv.evtCtx));
        }
    }
}



int ES_SWPunit::Apply(int hEvtSet) {
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

    Upci_Punit_t *pPunit = puList[activePunitIdx];

    // Check if core-wide attributes have changes since last apply and update punit
    assert((coreRsv.allocMask & appliedRsvMask) == appliedRsvMask); // ensure no coreRsv bits have been disabled at least.
    unsigned newAttribs = coreRsv.allocMask & ~appliedRsvMask;
    if (newAttribs) {  // new bits
        SetPunitCoreAttribs(pPunit, newAttribs);
        appliedRsvMask |= newAttribs;
    }

    int rc;
    if (pPunit->status & UPCI_UnitStatus_Applied) {
       Upci_Punit_UnApply(pPunit); // clean up from prior application (i.e. make sure counters are stopped 1st)
    }
    rc = Upci_Punit_Apply(pPunit);
    if (rc >= 0) {
        frozen = true;
        mbar(); // other threads can't access event set until frozen.
    }
    if (rc) { return lastErr.PrintOrExit(rc, BGPM_ERRLOC); }

    swModeAppliedPuEvtSet = hEvtSet;
    assert(this == (procEvtSets[hEvtSet]));

    BGPM_TRACE_DATA_L2( \
            Dump(_tf.indentLvl, true); \
            UPC_P_Dump_State_Indent(-1, _tf.indentLvl); \
            Upci_A2PC_Val_t a2qry; \
            Kernel_Upci_A2PC_GetRegs(&a2qry); \
            Upci_A2PC_DumpRegs_Indent(&a2qry, _tf.indentLvl); \
    );

    return 0;
}




int ES_SWPunit::Start() {
    BGPM_TRACE_L2;

    Upci_Punit_t *pPunit = puList[activePunitIdx];

    BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " pPunit->status=0x%08x\n", IND_STRG, pPunit->status));

#ifndef BGPM_FAST
    if (UNLIKELY(!(pPunit->status & UPCI_UnitStatus_Applied))) {
        return lastErr.PrintOrExit(BGPM_ENOT_APPLIED, BGPM_ERRLOC);
    }
#endif

    Upci_Punit_Start_Indep(pPunit, UPCI_CTL_OVF); // UPCI_CTL_SHARED |
    running = true;

    BGPM_TRACE_DATA_L2( \
        fprintf(stderr, "%s" _AT_ " upc_p->control=0x%016lx\n", IND_STRG, upc_p_local->control) \
    );

    return 0;
}



int ES_SWPunit::Stop() {
    BGPM_TRACE_L2;

    Upci_Punit_t *pPunit = puList[activePunitIdx];

#ifndef BGPM_FAST
    if (UNLIKELY(!(pPunit->status & UPCI_UnitStatus_Applied))) {
        return lastErr.PrintOrExit(BGPM_ENOT_APPLIED, BGPM_ERRLOC);
    }
#endif
    int rc = (pPunit->status & UPCI_UnitStatus_Running) ? 0 : BGPM_WALREADY_STOPPED;
    Upci_Punit_Stop_Indep(pPunit, UPCI_CTL_DELAY | UPCI_CTL_OVF); //UPCI_CTL_SHARED |
    running = false;

    BGPM_TRACE_DATA_L2( \
        fprintf(stderr, "%s" _AT_ " upc_p->control=0x%016lx\n", IND_STRG, upc_p_local->control) \
    );

    return rc;
}


int ES_SWPunit::ResetStart() {
    BGPM_TRACE_L2;
    Upci_Punit_t *pPunit = puList[activePunitIdx];

#ifndef BGPM_FAST
    if (UNLIKELY(!(pPunit->status & UPCI_UnitStatus_Applied))) {
        return lastErr.PrintOrExit(BGPM_ENOT_APPLIED, BGPM_ERRLOC);
    }
    if (UNLIKELY(running)) {
        return lastErr.PrintOrExit(BGPM_EUPDATE_DISALLOWED, BGPM_ERRLOC);
    }
#endif
    Upci_Punit_Reset_Counts(pPunit, UPCI_CTL_OVF); // UPCI_CTL_SHARED |
    Upci_Punit_Start_Indep(pPunit, UPCI_CTL_OVF); // UPCI_CTL_SHARED |
    running = true;

    BGPM_TRACE_DATA_L2( \
        fprintf(stderr, "%s" _AT_ " upc_p->control=0x%016lx\n", IND_STRG, upc_p_local->control) \
    );

    return 0;
}


int ES_SWPunit::Reset() {
    BGPM_TRACE_L2;
    Upci_Punit_t *pPunit = puList[activePunitIdx];

#ifndef BGPM_FAST
    if (UNLIKELY(!(pPunit->status & UPCI_UnitStatus_Applied))) {
        return lastErr.PrintOrExit(BGPM_ENOT_APPLIED, BGPM_ERRLOC);
    }
    if (UNLIKELY(running)) {
        return lastErr.PrintOrExit(BGPM_EUPDATE_DISALLOWED, BGPM_ERRLOC);
    }
#endif
    Upci_Punit_Reset_Counts(pPunit, UPCI_CTL_OVF); // UPCI_CTL_SHARED |
    return 0;
}



int ES_SWPunit::ReadEvent(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;

#ifndef BGPM_FAST
    Upci_Punit_t *pPunit = puList[activePunitIdx];

    assert(pVal);
    if (UNLIKELY(!(pPunit->status & UPCI_UnitStatus_Applied))) {
        return lastErr.PrintOrExit(BGPM_ENOT_APPLIED, BGPM_ERRLOC);
    }
    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
#endif

    // in case a signal handler could mess up the value being read, make sure it doesn't
    // occur while actively reading a counter.
    uint64_t curSigCount;
    do {
        curSigCount = (volatile uint64_t)(dbgThdVars.numPmSignals);
        *pVal = Upci_Punit_Event_ReadIdx(evtRecs[idx].pPunit, evtRecs[idx].puEvtIdx);
    } while (curSigCount != (volatile uint64_t)(dbgThdVars.numPmSignals));

    BGPM_TRACE_DATA_L2( \
        fprintf(stderr, "%s" _AT_ " *pVal=0x%016lx\n", IND_STRG, *pVal) \
    );

    return 0;
}


int ES_SWPunit::WriteEvent(unsigned idx, uint64_t val) {
    BGPM_TRACE_L2;

#ifndef BGPM_FAST
    Upci_Punit_t *pPunit = puList[activePunitIdx];

    if (UNLIKELY(!(pPunit->status & UPCI_UnitStatus_Applied))) {
        return lastErr.PrintOrExit(BGPM_ENOT_APPLIED, BGPM_ERRLOC);
    }
    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    if (UNLIKELY(evtRecs[idx].evtAttrMask & (unsigned)EvtAttr_Ovf)) {
        return lastErr.PrintOrExit(BGPM_EINV_WRT2OVF, BGPM_ERRLOC);
    }
    if (UNLIKELY(running)) {
        return lastErr.PrintOrExit(BGPM_EUPDATE_DISALLOWED, BGPM_ERRLOC);
    }
#endif
    Upci_Punit_Event_WriteIdx(evtRecs[idx].pPunit, evtRecs[idx].puEvtIdx, val);

    BGPM_TRACE_DATA_L2( \
        fprintf(stderr, "%s" _AT_ " *pVal=0x%016lx\n", IND_STRG, \
                Upci_Punit_Event_ReadIdx(evtRecs[idx].pPunit, evtRecs[idx].puEvtIdx)) \
    );

    return 0;
}



int ES_SWPunit::Punit_GetHandles(Bgpm_Punit_Handles_t *pH) {
    BGPM_TRACE_L2;
    assert(pH);
    Upci_Punit_t *pPunit = puList[activePunitIdx];

    Upci_Punit_Cfg_t *pCfg = &(pPunit->cfg);

    pH->allCtrMask = pCfg->ovfCtrMask | pCfg->thdCtrMask; // pCfg->sharedCtrMask |
    // pH->thdCtrMask = pCfg->thdCtrMask | pCfg->ovfCtrMask;
    pH->numCtrs = evtRecs.size();
    unsigned i;
    for (i=0; i<evtRecs.size(); i++) {
        Upci_PunitEventRec_t *pEvt = &(pPunit->evts[evtRecs[i].puEvtIdx]);
        pH->hCtr[i] = (uint64_t)&(upc_c->data24.grp[pEvt->c_Cgrp].counter[pEvt->c_Ctr]);
    }
    // Fill others with empty upc_c ptr (to avoid accidental segfaults)
    for (i=evtRecs.size(); i<BGPM_MAX_PUNIT_HANDLES; i++) {
        pH->hCtr[i] = (uint64_t)&(upc_c->iosram_data[255]);
    }
    frozen = true;
    return 0;
}



//int ES_SWPunit::Punit_GetLLHandles(Bgpm_Punit_LLHandles_t *pH) {
//    BGPM_TRACE_L2;
//    frozen = true;
//    return DefaultError();
//}



int ES_SWPunit::Punit_GetLLHandles(Bgpm_Punit_LLHandles_t *pH) {
    BGPM_TRACE_L2;
    assert(pH);
    Upci_Punit_t *pPunit = puList[activePunitIdx];
    Upci_Punit_Cfg_t *pCfg = &(pPunit->cfg);

    pH->allCtrMask = pCfg->ovfCtrMask | pCfg->thdCtrMask; // pCfg->sharedCtrMask |
    //pH->thdCtrMask = pCfg->thdCtrMask | pCfg->ovfCtrMask;
    pH->numCtrs = evtRecs.size();
    unsigned i;
    for (i=0; i<evtRecs.size(); i++) {
        Upci_PunitEventRec_t *pEvt = &(pPunit->evts[evtRecs[i].puEvtIdx]);
        pH->hCtr[i] = (uint64_t)&(upc_p_local->counter[pEvt->p_Ctr]);
    }
    // Fill others with empty upc_c ptr (to avoid accidental segfaults)
    for (i=evtRecs.size(); i<UPC_P_NUM_COUNTERS; i++) {
        pH->hCtr[i] = (uint64_t)&(upc_c->iosram_data[255]);
    }
    frozen = true;
    return 0;
}



uint64_t ES_SWPunit::GetActiveCtrMask()
{
    BGPM_TRACE_L2;
    Upci_Punit_t *pPunit = puList[activePunitIdx];

    Upci_Punit_Cfg_t *pCfg = &(pPunit->cfg);
    return (pCfg->ovfCtrMask | pCfg->thdCtrMask); // pCfg->sharedCtrMask
}



const char * ES_SWPunit::GetEventLabel(unsigned idx) {
    BGPM_TRACE_L2;
    if (UNLIKELY(idx >= evtRecs.size())) {
        lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
        return NULL;
    }
    return Upci_Punit_GetEventLabelIdx(evtRecs[idx].pPunit, evtRecs[idx].puEvtIdx);
}


int ES_SWPunit::GetEventId(unsigned idx) {
    BGPM_TRACE_L2;
    if (UNLIKELY(idx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    return Upci_Punit_GetEventIdIdx(evtRecs[idx].pPunit, evtRecs[idx].puEvtIdx);
}


int ES_SWPunit::GetEventIndex(unsigned evtId, unsigned startIdx) {
    BGPM_TRACE_L2;
    if (UNLIKELY(startIdx >= evtRecs.size())) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    unsigned idx = startIdx;
    bool found = false;
    while ((idx<evtRecs.size()) && (!found)) {
       if (evtRecs[idx].pPunit->evts[evtRecs[idx].puEvtIdx].pEvent->evtId == (unsigned)evtId) {
           found = true;
       }
       else {
           idx++;
       }
    }
    return idx;
}



int ES_SWPunit::SetEvtUser1(unsigned idx, uint64_t val) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }

    evtRecs[idx].user1 = val;
    return 0;
}
int ES_SWPunit::GetEvtUser1(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    *pVal = evtRecs[idx].user1;
    return 0;
}
int ES_SWPunit::SetEvtUser2(unsigned idx, uint64_t val) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }

    evtRecs[idx].user2 = val;
    return 0;
}
int ES_SWPunit::GetEvtUser2(unsigned idx, uint64_t *pVal) {
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    *pVal = evtRecs[idx].user2;
    return 0;
}


int ES_SWPunit::SetOverflowHandler(Bgpm_OverflowHandler_t handler)
{
    BGPM_TRACE_L2;
    pOvfHandler = handler;
    return 0;
}


int ES_SWPunit::SetOverflow(unsigned idx, uint64_t threshold)
{
    BGPM_TRACE_L2;

    BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " evtIdx=%d, threshold=0x%016lx\n", IND_STRG, idx, threshold));

    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    if (threshold < BGPM_THRESHOLD_MIN) {
        return lastErr.PrintOrExit(BGPM_EINV_OVF, BGPM_ERRLOC);
    }
    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }


    PunitEvtRec & evtRec = evtRecs[idx];
    evtRec.evtAttrMask = Or_EvtAttribs(evtRec.evtAttrMask, EvtAttr_Ovf); // flag that overflow is active

    int rc = Upci_Punit_Event_SetThreshold(evtRec.pPunit, evtRec.puEvtIdx, threshold);
    assert(rc==0); // shouldn't fail or there is coding bug somewhere
    #ifndef NDEBUG    // let's check that punit really has same threshold as sanity check (overkill??)
    uint64_t chk = Upci_Punit_Event_GetThreshold(evtRec.pPunit, evtRec.puEvtIdx);
    assert(chk == threshold);
    #endif

    dbgThdVars.ovfEnabled = 1;
    Upci_Punit_EnablePMInt(evtRec.pPunit);

    return rc;
}



int ES_SWPunit::GetOverflow(unsigned idx, uint64_t *pThreshold)
{
    BGPM_TRACE_L2;
    if (idx >= evtRecs.size()) {
        return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    }
    assert(pThreshold);

    PunitEvtRec & evtRec = evtRecs[idx];
    *pThreshold = Upci_Punit_Event_GetThreshold(evtRec.pPunit, evtRec.puEvtIdx);

    return 0;
}



Bgpm_OverflowHandler_t ES_SWPunit::GetOverflowHandler() { return pOvfHandler; }


void ES_SWPunit::SetEvtSetPunitInts(Upci_Punit_t *pPunit)
{
    assert(pPunit);
    Upci_Punit_EnablePMInt(pPunit);
}



bool ES_SWPunit::AccumResetThresholds(uint64_t intMask)
{
    Upci_Punit_t *pPunit = puList[activePunitIdx];

    int activeOvf = Upci_Punit_AccumResetThresholds(pPunit, intMask);
    return (activeOvf != 0);
}



int ES_SWPunit::GetOverflowIndices(uint64_t ovfVector, unsigned *pIndicies, unsigned *pLen)
{
    assert(pIndicies);
    assert(pLen);

    Upci_Punit_t *pPunit = puList[activePunitIdx];

    int toosmall = Upci_Punit_GetOvfBackRefs(pPunit, ovfVector, pIndicies, pLen);
    if (toosmall) {
        *pLen = upc_popcnt32(pPunit->cfg.ovfCtrMask);
        return lastErr.PrintOrExit(BGPM_EARRAY_TOO_SMALL, BGPM_ERRLOC);
    }
    return 0;
}



int ES_SWPunit::SetContext(Bgpm_Context ctx)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }

    int rc = 0;
    if (coreRsv.RsvContext(ctx)) {
        coreRsvMask |= Alloc_evtCtx;
        if (ctx == BGPM_CTX_BOTH) {
            coreRsvMask |= Alloc_evtCtxBoth;
        }
    }
    else {
        rc = BGPM_ECTX_MISMATCH;
    }
    return rc;
}



int ES_SWPunit::GetContext(Bgpm_Context *pCtx)
{
    BGPM_TRACE_L2;

    assert(pCtx);
    if (coreRsv.allocMask & Alloc_evtCtx) {
        *pCtx = coreRsv.evtCtx;
    }
    else {
        *pCtx = BGPM_CTX_DEFAULT;
    }
    return 0;
}



int ES_SWPunit::AllowMixedContext()
{
    BGPM_TRACE_L2;

    coreRsv.SetMixedContext();
    coreRsvMask |= Alloc_evtCtxMixed;

    return 0;
}



int ES_SWPunit::SetQfpuGrpMask(unsigned idx, uint64_t mask)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(idx >= evtRecs.size())) return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }

    Upci_Punit_t *pPunit = evtRecs[idx].pPunit;
    unsigned puEvtIdx = evtRecs[idx].puEvtIdx;

    unsigned evtId = Upci_Punit_GetEventIdIdx(pPunit, puEvtIdx);
    if (UNLIKELY(evtId != PEVT_INST_QFPU_GRP_MASK)) return lastErr.PrintOrExit(BGPM_EUNSUP_ATTRIB, BGPM_ERRLOC);

    Upci_Punit_Set_OpcodeGrpMask(pPunit, puEvtIdx, UPC_P__COUNTER_CFG__AXU_OPGROUP_SEL_set(mask));
   
    evtRecs[idx].evtAttrMask = Or_EvtAttribs(evtRecs[idx].evtAttrMask, EvtAttr_QfpuGrpMask);

    return 0;
}



int ES_SWPunit::GetQfpuGrpMask(unsigned idx, uint64_t *pMask)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(idx >= evtRecs.size())) return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);

    Upci_Punit_t *pPunit = evtRecs[idx].pPunit;
    unsigned puEvtIdx = evtRecs[idx].puEvtIdx;

    unsigned evtId = Upci_Punit_GetEventIdIdx(pPunit, puEvtIdx);
    if (UNLIKELY(evtId != PEVT_INST_QFPU_GRP_MASK)) return lastErr.PrintOrExit(BGPM_EUNSUP_ATTRIB, BGPM_ERRLOC);

    *pMask = UPC_P__COUNTER_CFG__AXU_OPGROUP_SEL_get(Upci_Punit_Get_OpcodeGrpMask(pPunit, puEvtIdx));

    return 0;
}



int ES_SWPunit::SetXuGrpMask(unsigned idx, uint64_t mask)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(idx >= evtRecs.size())) return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }

    Upci_Punit_t *pPunit = evtRecs[idx].pPunit;
    unsigned puEvtIdx = evtRecs[idx].puEvtIdx;

    unsigned evtId = Upci_Punit_GetEventIdIdx(pPunit, puEvtIdx);
    if (UNLIKELY(evtId != PEVT_INST_XU_GRP_MASK)) return lastErr.PrintOrExit(BGPM_EUNSUP_ATTRIB, BGPM_ERRLOC);

    Upci_Punit_Set_OpcodeGrpMask(pPunit, puEvtIdx, UPC_P__COUNTER_CFG__XU_OPGROUP_SEL_set(mask));
    
    evtRecs[idx].evtAttrMask = Or_EvtAttribs(evtRecs[idx].evtAttrMask, EvtAttr_XuGrpMask); // flag that overridden 

    return 0;
}



int ES_SWPunit::GetXuGrpMask(unsigned idx, uint64_t *pMask)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(idx >= evtRecs.size())) return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);

    Upci_Punit_t *pPunit = evtRecs[idx].pPunit;
    unsigned puEvtIdx = evtRecs[idx].puEvtIdx;

    unsigned evtId = Upci_Punit_GetEventIdIdx(pPunit, puEvtIdx);
    if (UNLIKELY(evtId != PEVT_INST_XU_GRP_MASK)) return lastErr.PrintOrExit(BGPM_EUNSUP_ATTRIB, BGPM_ERRLOC);
;

    *pMask = UPC_P__COUNTER_CFG__XU_OPGROUP_SEL_get(Upci_Punit_Get_OpcodeGrpMask(pPunit, puEvtIdx));

    return 0;
}



int ES_SWPunit::SetCountFP(unsigned idx, ushort countFp)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(idx >= evtRecs.size())) return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }

    Upci_Punit_t *pPunit = evtRecs[idx].pPunit;
    unsigned puEvtIdx = evtRecs[idx].puEvtIdx;

    unsigned evtId = Upci_Punit_GetEventIdIdx(pPunit, puEvtIdx);
    UPC_EventSources_t src = Upci_GetEventSrc(evtId);

    if (UNLIKELY((src != UPC_ES_OP_AXU)
                  && (src != UPC_ES_OP_FP_OPS)
                  && (src != UPC_ES_OP_FP_INSTR))) return lastErr.PrintOrExit(BGPM_EUNSUP_ATTRIB, BGPM_ERRLOC);

    //fprintf(stderr, "%s" _AT_ " puEvtIdx=%d, countFp=%d\n", IND_STRG, puEvtIdx, countFp);
    Upci_Punit_Set_OpsNotFlops(pPunit, puEvtIdx, (countFp == false));

    evtRecs[idx].evtAttrMask = Or_EvtAttribs(evtRecs[idx].evtAttrMask, EvtAttr_FP); // flag that overridden

    return 0;
}



int ES_SWPunit::GetCountFP(unsigned idx, ushort *pCountFp)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(idx >= evtRecs.size())) return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);

    Upci_Punit_t *pPunit = evtRecs[idx].pPunit;
    unsigned puEvtIdx = evtRecs[idx].puEvtIdx;

    unsigned evtId = Upci_Punit_GetEventIdIdx(pPunit, puEvtIdx);
    UPC_EventSources_t src = Upci_GetEventSrc(evtId);

    if (UNLIKELY((src != UPC_ES_OP_AXU)
                   && (src != UPC_ES_OP_FP_OPS)
                   && (src != UPC_ES_OP_FP_INSTR))) {
        *pCountFp = false;
        return 0;
    }

    ushort ops = Upci_Punit_Get_OpsNotFlops(pPunit, puEvtIdx);
    //fprintf(stderr, "%s" _AT_ " puEvtIdx=%d, ops=%d, ops==false = %d\n", IND_STRG, puEvtIdx, ops, (ops==false));
    *pCountFp = ops ? 0 : 1;
    return 0;
}



int ES_SWPunit::SetEdge(unsigned idx, ushort edge)
{
    BGPM_TRACE_L2;
    if (UNLIKELY(idx >= evtRecs.size())) return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }

    Upci_Punit_t *pPunit = evtRecs[idx].pPunit;
    unsigned puEvtIdx = evtRecs[idx].puEvtIdx;

    ushort ok = Upci_Punit_Set_Edge(pPunit, puEvtIdx, edge);
    //fprintf(stderr, _AT_ " ok = %d, edge=%d\n", ok, edge);
    if (!ok) return lastErr.PrintOrExit(BGPM_EUNSUP_ATTRIB, BGPM_ERRLOC);

    if (edge) {
        evtRecs[idx].evtAttrMask = Or_EvtAttribs(evtRecs[idx].evtAttrMask, EvtAttr_Edge); // flag that overridden
    }
    else {
        evtRecs[idx].evtAttrMask = Or_EvtAttribs(evtRecs[idx].evtAttrMask, EvtAttr_Cycle); // flag that overridden
    }

    return 0;
}



int ES_SWPunit::GetEdge(unsigned idx, ushort *pEdge)
{
    BGPM_TRACE_L2;
    if (UNLIKELY(idx >= evtRecs.size())) return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    Upci_Punit_t *pPunit = evtRecs[idx].pPunit;
    unsigned puEvtIdx = evtRecs[idx].puEvtIdx;

    *pEdge = Upci_Punit_Get_Edge(pPunit, puEvtIdx);
    //fprintf(stderr, _AT_ " *pEdge=%d\n", *pEdge);
    return 0;
}



int ES_SWPunit::SetInvert(unsigned idx, ushort invert)
{
    BGPM_TRACE_L2;
    if (UNLIKELY(idx >= evtRecs.size())) return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }

    Upci_Punit_t *pPunit = evtRecs[idx].pPunit;
    unsigned puEvtIdx = evtRecs[idx].puEvtIdx;

    ushort ok = Upci_Punit_Set_Invert(pPunit, puEvtIdx, invert);
    if (!ok) return lastErr.PrintOrExit(BGPM_EUNSUP_ATTRIB, BGPM_ERRLOC);

    evtRecs[idx].evtAttrMask = Or_EvtAttribs(evtRecs[idx].evtAttrMask, EvtAttr_Invert); // flag that overridden

    return 0;
}



int ES_SWPunit::GetInvert(unsigned idx, ushort *pInvert)
{
    BGPM_TRACE_L2;
    if (UNLIKELY(idx >= evtRecs.size())) return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    Upci_Punit_t *pPunit = evtRecs[idx].pPunit;
    unsigned puEvtIdx = evtRecs[idx].puEvtIdx;

    *pInvert = Upci_Punit_Get_Invert(pPunit, puEvtIdx);
    *pInvert = *pInvert ? 1 : 0;
    return 0;
}



int ES_SWPunit::SetXuMatch(unsigned idx, uint16_t match, uint16_t mask)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(idx >= evtRecs.size())) return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }

    Upci_Punit_t *pPunit = evtRecs[idx].pPunit;
    unsigned puEvtIdx = evtRecs[idx].puEvtIdx;
    unsigned evtId = Upci_Punit_GetEventIdIdx(pPunit, puEvtIdx);
    if (UNLIKELY(evtId != PEVT_INST_XU_MATCH)) return lastErr.PrintOrExit(BGPM_EUNSUP_ATTRIB, BGPM_ERRLOC);

    if (!coreRsv.RsvXuMatch(match,mask)) return lastErr.PrintOrExit(BGPM_EXU_RSV_MISMATCH, BGPM_ERRLOC);

    return 0;
}



int ES_SWPunit::GetXuMatch(unsigned idx, uint16_t *pMatch, uint16_t *pMask)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(idx >= evtRecs.size())) return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);

    Upci_Punit_t *pPunit = evtRecs[idx].pPunit;
    unsigned puEvtIdx = evtRecs[idx].puEvtIdx;
    unsigned evtId = Upci_Punit_GetEventIdIdx(pPunit, puEvtIdx);
    if (UNLIKELY(evtId != PEVT_INST_XU_MATCH)) return lastErr.PrintOrExit(BGPM_EUNSUP_ATTRIB, BGPM_ERRLOC);

    *pMatch = coreRsv.xuMatch;
    *pMask = coreRsv.xuMask;

    #ifndef NDEBUG
    // Let's verify that what is stored in punit is identical to what's in coreRsv space
    if (appliedRsvMask & Alloc_xuMatch) {
        uint32_t existValue = (pPunit->cfg.opCodeMatch & 0xFFFFFFFF00000000ULL) >> 32;
        uint16_t existMatch = (existValue & 0xFFFF0000) >> 16;
        uint16_t existMask  = (existValue & 0x0000FFFF);
        assert (coreRsv.xuMatch == existMatch);
        assert (coreRsv.xuMask  == existMask);
    }
    #endif

    return 0;
}



int ES_SWPunit::SetQfpuMatch(unsigned idx, uint16_t match, uint16_t mask, ushort fpScale)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(idx >= evtRecs.size())) return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);
    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }

    Upci_Punit_t *pPunit = evtRecs[idx].pPunit;
    unsigned puEvtIdx = evtRecs[idx].puEvtIdx;
    unsigned evtId = Upci_Punit_GetEventIdIdx(pPunit, puEvtIdx);
    if (UNLIKELY(evtId != PEVT_INST_QFPU_MATCH)) return lastErr.PrintOrExit(BGPM_EUNSUP_ATTRIB, BGPM_ERRLOC);

    if (UNLIKELY(fpScale > 3)) return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);

    if (!coreRsv.RsvQfpuMatch(match,mask,fpScale)) return lastErr.PrintOrExit(BGPM_EQFPU_RSV_MISMATCH, BGPM_ERRLOC);

    return 0;
}



int ES_SWPunit::GetQfpuMatch(unsigned idx, uint16_t *pMatch, uint16_t *pMask, ushort *pFpScale)
{
    BGPM_TRACE_L2;

    if (UNLIKELY(idx >= evtRecs.size())) return lastErr.PrintOrExit(BGPM_EINV_EVT_IDX, BGPM_ERRLOC);

    Upci_Punit_t *pPunit = evtRecs[idx].pPunit;
    unsigned puEvtIdx = evtRecs[idx].puEvtIdx;
    unsigned evtId = Upci_Punit_GetEventIdIdx(pPunit, puEvtIdx);
    if (UNLIKELY(evtId != PEVT_INST_QFPU_MATCH)) return lastErr.PrintOrExit(BGPM_EUNSUP_ATTRIB, BGPM_ERRLOC);

    *pMatch = coreRsv.qfpuMatch;
    *pMask = coreRsv.qfpuMask;
    *pFpScale = coreRsv.qfpuScale;

    #ifndef NDEBUG
    // Let's verify that what is stored in punit is identical to what's in coreRsv space
    if (appliedRsvMask & Alloc_qfpuMatch) {
        uint32_t existValue = pPunit->cfg.opCodeMatch & 0x00000000FFFFFFFFULL;
        uint16_t existMatch = (existValue & 0xFFFF0000) >> 16;
        uint16_t existMask  = (existValue & 0x0000FFFF);
        uint8_t  existScale = UPC_P__CONFIG__AXU_MATCH_WEIGHT_get(pPunit->cfg.fpWeights);
        assert (coreRsv.qfpuMatch == existMatch);
        assert (coreRsv.qfpuMask  == existMask);
        assert (coreRsv.qfpuScale == existScale);
    }
    #endif

    return 0;
}



int ES_SWPunit::SetFpSqrScale(ushort fpScale)
{
    BGPM_TRACE_L2;
    if (UNLIKELY(fpScale > 3)) return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }

    if (!coreRsv.RsvSqrtScale(fpScale)) return lastErr.PrintOrExit(BGPM_ESQRT_SCALE_MISMATCH, BGPM_ERRLOC);
    return 0;
}



int ES_SWPunit::GetFpSqrScale(ushort *pFpScale)
{
    BGPM_TRACE_L2;

    *pFpScale = coreRsv.sqrtScale;

    return 0;
}



int ES_SWPunit::SetFpDivScale(ushort fpScale)
{
    BGPM_TRACE_L2;
    if (UNLIKELY(fpScale > 3)) return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    if (UNLIKELY(frozen)) { return lastErr.PrintOrExit(BGPM_EES_FROZEN, BGPM_ERRLOC); }

    if (!coreRsv.RsvSqrtScale(fpScale)) return lastErr.PrintOrExit(BGPM_EDIV_SCALE_MISMATCH, BGPM_ERRLOC);
    return 0;
}



int ES_SWPunit::GetFpDivScale(ushort *pFpScale)
{
    BGPM_TRACE_L2;

    *pFpScale = coreRsv.divScale;

    return 0;
}



int ES_SWPunit::PrintCurrentReservations()
{
    BGPM_TRACE_L2;

    Upci_Punit_t *pPunit = puList[activePunitIdx];

    char buf[BGPM_FEEDBACK_BUFSIZE];
    FillFeedbackBuffer(buf, BGPM_FEEDBACK_BUFSIZE, pPunit, 0);
    PrintLines(buf, BGPM_FEEDBACK_BUFSIZE);
    return 0;
}



int ES_SWPunit::GetMux(Bgpm_MuxStats_t* pMuxStats)
{
    BGPM_TRACE_L2;
    assert(pMuxStats);
    Bgpm_MuxStats_t & stats = *pMuxStats;
    stats.active = 0;
    stats.elapsedCycles = 0;
    stats.numGrps = 1;
    stats.numSwitches = 0;
    stats.period = 0;
    stats.normalize = 0;
    return 0;
}



int ES_SWPunit::GetMuxGrps() { return 1; }


uint64_t ES_SWPunit::GetMuxCycles(unsigned muxGrp) { return 0; }
uint64_t ES_SWPunit::GetMuxEventCycles(unsigned evtIdx, ushort normalize) { return 0; }



EvtSet::ESTypes ES_LLPunit::ESType() { return ESType_LLPunit; }
const char *ES_LLPunit::ESTypeLabel() { return "Low Lat Punit"; }
//EvtSet *ES_LLPunit::Spawn() { return new ES_LLPunit(); }



EvtSet *ES_LLPunit::Clone(uint64_t target, unsigned *addEvents, unsigned addNum)
{
    BGPM_TRACE_L2;
    if (UNLIKELY(target >= CONFIG_MAX_HWTHREADS)) {
        lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
        return NULL;
    }

    ES_SWPunit*p = new ES_LLPunit();
    if (p->GetCrtRC() >= 0) {
        p->MergeEventCfg(*this, (unsigned)target, addEvents, addNum);
    }
    return p;
}




int ES_LLPunit::Start() { BGPM_TRACE_L2; return DefaultError(); }
int ES_LLPunit::Stop() { BGPM_TRACE_L2; return DefaultError(); }
int ES_LLPunit::ResetStart() { BGPM_TRACE_L2; return DefaultError(); }
int ES_LLPunit::Reset() { BGPM_TRACE_L2; return DefaultError(); }
int ES_LLPunit::ReadEvent(unsigned idx, uint64_t *pVal) { BGPM_TRACE_L2; return DefaultError(); }
int ES_LLPunit::WriteEvent(unsigned idx, uint64_t val) { BGPM_TRACE_L2; return DefaultError(); }
int ES_LLPunit::Punit_GetHandles(Bgpm_Punit_Handles_t *pH) { BGPM_TRACE_L2; return DefaultError(); }


#if 0  // Move to SwPunit - allow LL handles to be used with caution for debug
int ES_LLPunit::Punit_GetLLHandles(Bgpm_Punit_LLHandles_t *pH) {
    BGPM_TRACE_L2;
    assert(pH);
    Upci_Punit_t *pPunit = puList[activePunitIdx];

    Upci_Punit_Cfg_t *pCfg = &(pPunit->cfg);

    pH->allCtrMask = pCfg->ovfCtrMask | pCfg->thdCtrMask; // pCfg->sharedCtrMask |
    //pH->thdCtrMask = pCfg->thdCtrMask | pCfg->ovfCtrMask;
    pH->numCtrs = evtRecs.size();
    unsigned i;
    for (i=0; i<evtRecs.size(); i++) {
        Upci_PunitEventRec_t *pEvt = &(pPunit->evts[evtRecs[i].puEvtIdx]);
        pH->hCtr[i] = (uint64_t)&(upc_p_local->counter[pEvt->p_Ctr]);
    }
    // Fill others with empty upc_c ptr (to avoid accidental segfaults)
    for (i=evtRecs.size(); i<UPC_P_NUM_COUNTERS; i++) {
        pH->hCtr[i] = (uint64_t)&(upc_c->iosram_data[255]);
    }
    frozen = true;
    return 0;
}
#endif


void ES_LLPunit::SetEvtSetPunitInts(Upci_Punit_t *pPunit) {
    // Upci_Punit_EnablePMInt(&punit);  no interrupts for LL Mode
}



int ES_LLPunit::SetOverflowHandler(Bgpm_OverflowHandler_t handler) { return DefaultError(); }
int ES_LLPunit::SetOverflow(unsigned idx, uint64_t threshold) { return DefaultError(); }
