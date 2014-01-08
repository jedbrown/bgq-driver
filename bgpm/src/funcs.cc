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

#include "bgpm/include/bgpm.h"
#include "EvtSetList.h"
#include "globals.h"
#include "ES_Factory.h"
#include "ES_MXPunit.h"


using namespace bgpm;


__BEGIN_DECLS



// If the current Event set hasn't been assigned a type yet, replace it with the correct
// event set corresponding with the source type of the event passed.
static int AllocateUnassignedEvtSet(unsigned hEvtSet, unsigned eventId)
{
    EvtSet *pSet = procEvtSets[hEvtSet];
    assert(pSet);
    assert(pSet->ESType() != EvtSet::ESType_Base);
    int rc = 0;

    // Get new event set type based on mode if not already assigned.
    if (pSet->ESType() == EvtSet::ESType_Unassigned ) {
        EvtSet *newEvtSet = NULL;
        rc = GenerateEventSet(pShMem->globalMode, eventId, &newEvtSet);
        if (UNLIKELY(rc)) return rc;
        assert(newEvtSet);
        newEvtSet->user1 = pSet->user1;
        newEvtSet->user2 = pSet->user2;
        procEvtSets.ReplaceSet(hEvtSet,newEvtSet);
    }
    return 0;
}





int Bgpm_CreateEventSet()
{
    BGPM_TRACE_L1;
    BGPM_TRACE_DATA_L1(fprintf(stderr, "%s" _AT_ " hwtid=%02d, tid=%ld, procInited=%d, thdInited=%d\n", IND_STRG, Kernel_ProcessorID(), gettid(), procInited, thdInited));

    if (UNLIKELY(! thdInited)) {
        lastErr.Init();
        lastErr.SetTopLevelFunc(__func__);
        return lastErr.PrintOrExit(BGPM_ENOT_INIT, BGPM_ERRLOC);
    }
    lastErr.SetTopLevelFunc(__func__);

    if (UNLIKELY(!thdInited)) { return lastErr.PrintOrExit(BGPM_ENOT_INIT, BGPM_ERRLOC); }

    int rc = procEvtSets.AddSet(NULL);

    BGPM_TRACE_DATA_L1(
            fprintf(stderr, _AT_ " tid: %d, hEvtSet=%d\n", Kernel_ProcessorID(), rc)
            )
    ;

    return rc;
}



int Bgpm_DeleteEventSet(unsigned hEvtSet)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    BGPM_TRACE_DATA_L1(
            fprintf(stderr, _AT_ " tid: %d, hEvtSet=%d\n", Kernel_ProcessorID(), hEvtSet)
            )
    ;

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    // Be careful - these are set by the Apply() operations on the individual event sets.
    // But must be cleared when the event set is removed.  If any other operation
    // were to remove event sets from the procEvtSet list - make sure the global
    // handles are removed there too - otherwise the signal handler could try to
    // reference deleted handles.
    //
    if      (swModeAppliedPuEvtSet == (int)hEvtSet) { swModeAppliedPuEvtSet = -1; }
    else if (swModeAppliedL2EvtSet == (int)hEvtSet) { swModeAppliedL2EvtSet = -1; }
    else if (swModeAppliedIOEvtSet == (int)hEvtSet) { swModeAppliedIOEvtSet = -1; }

    procEvtSets.RmvSet(hEvtSet);

    // <<<<<<<<<  Try to open window where another thread can acquire the passed event set handle.
    //uint64_t delay = (GetTimeBase() % 16) * 2000;   // <<<<<<<<<<<<<<<<<<<<<<<<<<<
    //Upci_Delay(delay);

    return 0;
}




int Bgpm_AddEvent(unsigned hEvtSet, unsigned eventId)
{
    BGPM_TRACE_L1;
    if (UNLIKELY(! thdInited)) {
        lastErr.Init();
        lastErr.SetTopLevelFunc(__func__);
        return lastErr.PrintOrExit(BGPM_ENOT_INIT, BGPM_ERRLOC);
    }
    lastErr.SetTopLevelFunc(__func__);

    BGPM_TRACE_DATA_L2(
            fprintf(stderr, "%s" _AT_ " hwtid=%02d hEvtSet=%d, eventId=%d\n", IND_STRG, Kernel_ProcessorID(), hEvtSet, eventId)
            )
    ;


    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    int rc = AllocateUnassignedEvtSet(hEvtSet, eventId);
    if (UNLIKELY(rc)) return rc;

    EvtSet *pSet = procEvtSets[hEvtSet];

    if (UNLIKELY(!pSet->IsEventIdCompatible(eventId))) {
        if (pSet->IsEventIdValid(eventId)) {
            char strg[128];
            snprintf(strg, 128, "Event %d (%s) is incompatable with event set type %s\n",
                    eventId, Bgpm_GetEventIdLabel(eventId), pSet->ESTypeLabel());
            lastErr.PrintMsg(strg);
            return lastErr.PrintOrExit(BGPM_ECONF_UNIT, BGPM_ERRLOC);
        }
        else  return lastErr.PrintOrExit(BGPM_EINV_EVENT, BGPM_ERRLOC);
    }

    EvtSet *pReplSet = NULL;
    rc = pSet->AddEvent(eventId, pReplSet);
    if (pReplSet) {
        procEvtSets.ReplaceSet(hEvtSet,pReplSet);
        pSet = pReplSet;
    }
    BGPM_EXEMPT_DATA(fprintf(stderr, "%s" _AT_ "\n", IND_STRG); pSet->Dump(IND_LVL));
    return rc;
}



int Bgpm_AddEventList(unsigned hEvtSet, unsigned *eventIds, unsigned num)
{
    BGPM_TRACE_L1;

    if (UNLIKELY(! thdInited)) {
        lastErr.Init();
        lastErr.SetTopLevelFunc(__func__);
        return lastErr.PrintOrExit(BGPM_ENOT_INIT, BGPM_ERRLOC);
    }
    lastErr.SetTopLevelFunc(__func__);

    if ((eventIds == NULL) || (num == 0)) {
        return lastErr.PrintOrExit(UPCI_EINV_PARM, BGPM_ERRLOC);
    }

    BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ " hwtid=%02d hEvtSet=%d, numEvents=%d\n", IND_STRG, Kernel_ProcessorID(), hEvtSet, num));
    //fprintf(stderr, "%s" _AT_ " hwtid=%02d hEvtSet=%d, numEvents=%d\n", IND_STRG, Kernel_ProcessorID(), hEvtSet, num); // <<<<<<<<<<<<<


    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    int rc = AllocateUnassignedEvtSet(hEvtSet, eventIds[0]);
    if (UNLIKELY(rc)) return rc;

    EvtSet *pSet = procEvtSets[hEvtSet];

    for (unsigned i=0; i<num; ++i) {
        if (UNLIKELY(!pSet->IsEventIdCompatible(eventIds[i]))) {
            if (pSet->IsEventIdValid(eventIds[i])) {
                lastErr.PrintMsg("Event ID (%d) is incompatible with %s event set type\n",
                                  eventIds[i], pSet->ESTypeLabel());
                return lastErr.PrintOrExit(BGPM_ECONF_UNIT, BGPM_ERRLOC);
            }
            else {
                lastErr.PrintMsg("Event ID (%d) is not a valid event\n", eventIds[i]);
                return lastErr.PrintOrExit(BGPM_EINV_EVENT, BGPM_ERRLOC);
            }
        }
    }

    EvtSet *pReplSet = NULL;
    rc = pSet->AddEventList(eventIds, num, pReplSet);
    if (pReplSet) {
        procEvtSets.ReplaceSet(hEvtSet,pReplSet);
        pSet = pReplSet;
    }

    BGPM_TRACE_DATA_L2(fprintf(stderr, "%s" _AT_ "\n", IND_STRG); pSet->Dump(IND_LVL));
    return rc;
}



// This hidden function will allow the punit signals and counters used by the evtset
// to be prereserved and skipped by all subsequently created punits on the same core.
// It's the exact signals and counters, so only one thread on the core will be effected.
// This allows us to create two punit events sets which can run independently on the same
// hwthread - use one to measure ourselves.
int Bgpm_SetPreReserved(unsigned hEvtSet)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->SetPreReserved();
}



int Bgpm_NumEvents(unsigned hEvtSet)
{
    BGPM_TRACE_L1;
    if (! thdInited) {
        lastErr.Init();
        lastErr.SetTopLevelFunc(__func__);
        return lastErr.PrintOrExit(BGPM_ENOT_INIT, BGPM_ERRLOC);
    }
    lastErr.SetTopLevelFunc(__func__);

    int vrc = procEvtSets.ValidSetIdx(hEvtSet, AnyThreadOk);
    if (UNLIKELY(vrc)) return 0;

    EvtSetHazardGuard hazardGuard(hEvtSet);

    return procEvtSets[hEvtSet]->NumEvents();
}



int Bgpm_NumTargets(unsigned hEvtSet)
{
    BGPM_TRACE_L1;
    if (! thdInited) {
        lastErr.Init();
        lastErr.SetTopLevelFunc(__func__);
        return lastErr.PrintOrExit(BGPM_ENOT_INIT, BGPM_ERRLOC);
    }
    lastErr.SetTopLevelFunc(__func__);

    int vrc = procEvtSets.ValidSetIdx(hEvtSet, AnyThreadOk);
    if (UNLIKELY(vrc)) return 0;

    EvtSetHazardGuard hazardGuard(hEvtSet);

    return procEvtSets[hEvtSet]->NumTargets();
}



int Bgpm_Start(unsigned hEvtSet)
{
    BGPM_TRACE_L1;

#ifndef BGPM_FAST
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);
#endif

    return procEvtSets[hEvtSet]->Start();
}


int Bgpm_Apply(unsigned hEvtSet)
{
    BGPM_TRACE_L1;
    int rc;
#ifndef BGPM_FAST
    lastErr.SetTopLevelFunc(__func__);

    rc = procEvtSets.ValidSetIdx(hEvtSet, MustBeControlThread);
    if (UNLIKELY(rc)) return rc;
#endif

    if (curHwThdId != Kernel_ProcessorID()) {
        return lastErr.PrintOrExit(BGPM_ESWTHD_MOVED, BGPM_ERRLOC);
    }

    rc = procEvtSets[hEvtSet]->Apply(hEvtSet);
    return rc;
}



int Bgpm_Stop(unsigned hEvtSet)
{
    BGPM_TRACE_L1;
#ifndef BGPM_FAST
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);
#endif

    int rc = procEvtSets[hEvtSet]->Stop();
    BGPM_TRACE_DATA_L2(
        if (dbgThdVars.ovfEnabled) {
            unsigned thd=Kernel_ProcessorThreadID();
            fprintf(stderr, "%s" _AT_ " upcp_intstatus=0x%016lx, upcp_ctl=0x%016lx, upcc_intenable=0x%016lx, upcc_intstatus=0x%016lx\n",
                    IND_STRG, upc_p_local->int_status[thd], upc_p_local->p_config, upc_c->ccg_int_en, upc_c->ccg_int_status_w1c);
            BgpmDebugPrintThreadVars();
        };
    );
    return rc;
}



int Bgpm_ResetStart(unsigned hEvtSet)
{
    BGPM_TRACE_L1;

#ifndef BGPM_FAST
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);
#endif

    return procEvtSets[hEvtSet]->ResetStart();
}



int Bgpm_Reset(unsigned hEvtSet)
{
    BGPM_TRACE_L1;
#ifndef BGPM_FAST
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);
#endif

    return procEvtSets[hEvtSet]->Reset();
}



int Bgpm_ReadEvent(unsigned hEvtSet, unsigned evtIdx, uint64_t *pValue)
{
    BGPM_TRACE_L1;
#ifndef BGPM_FAST
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (!pValue) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }
#endif

    return procEvtSets[hEvtSet]->ReadEvent(evtIdx, pValue);
}



int Bgpm_ReadEventList(unsigned hEvtSet, uint64_t *pValues, unsigned *pLen)
{
    BGPM_TRACE_L1;
    int rc = 0;
#ifndef BGPM_FAST
    lastErr.SetTopLevelFunc(__func__);

    rc = procEvtSets.ValidSetIdx(hEvtSet, AnyThreadOk);
    if (UNLIKELY(rc)) return rc;

    if (!pValues) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }
    if (!pLen) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }
#endif
    unsigned numEvents = Bgpm_NumEvents(hEvtSet);
    if (*pLen < numEvents) {
        *pLen = numEvents;
        return lastErr.PrintOrExit(BGPM_EARRAY_TOO_SMALL, BGPM_ERRLOC);
    }
    *pLen = numEvents;
    unsigned idx;
    for (idx = 0; (idx<numEvents) && (rc>=0); ++idx)  {
        rc = procEvtSets[hEvtSet]->ReadEvent(idx, &(pValues[idx]));
    }
    return rc;
}



int Bgpm_WriteEvent(unsigned hEvtSet, unsigned evtIdx, uint64_t value)
{
    BGPM_TRACE_L1;
#ifndef BGPM_FAST
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);
#endif

    return procEvtSets[hEvtSet]->WriteEvent(evtIdx, value);
}



int Bgpm_L2_ReadEvent(unsigned hEvtSet, unsigned slice, unsigned idx, uint64_t *pValue)
{
    BGPM_TRACE_L1;
#ifndef BGPM_FAST
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (!pValue) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }
#endif

    return procEvtSets[hEvtSet]->L2_ReadEvent(slice, idx, pValue);
}



int Bgpm_L2_WriteEvent(unsigned hEvtSet, unsigned slice, unsigned idx, uint64_t value)
{
    BGPM_TRACE_L1;
#ifndef BGPM_FAST

    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);
#endif

    return procEvtSets[hEvtSet]->L2_WriteEvent(slice, idx, value);
}



int Bgpm_ReadThreadEvent(unsigned hEvtSet, unsigned evtIdx,
        uint64_t thdMask, uint64_t agentMask, uint64_t *pValue)
{
    BGPM_TRACE_L1;
#ifndef BGPM_FAST
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (!pValue) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }
#endif

    return procEvtSets[hEvtSet]->ReadThreadEvent(evtIdx, thdMask, agentMask, pValue);
}



int Bgpm_NW_ReadLinkEvent(unsigned hEvtSet, unsigned evtIdx, UPC_NW_LinkMasks mask, uint64_t *pValue)
{
    BGPM_TRACE_L1;
#ifndef BGPM_FAST

    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (!pValue) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }
#endif

    return procEvtSets[hEvtSet]->ReadLinkEvent(evtIdx, mask, pValue);
}



int Bgpm_CNK_SampleEvent(unsigned hEvtSet, unsigned evtIdx, uint64_t *pValue)
{
    BGPM_TRACE_L1;
#ifndef BGPM_FAST

    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (!pValue) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }
#endif

    return procEvtSets[hEvtSet]->SampleCNKEvent(evtIdx, pValue);
}



int Bgpm_Punit_GetHandles(unsigned hEvtSet, Bgpm_Punit_Handles_t *pH)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (!pH) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    return procEvtSets[hEvtSet]->Punit_GetHandles(pH);
}



int Bgpm_Punit_GetHandles4Thread(unsigned hEvtSet, unsigned hwThd, Bgpm_Punit_Handles_t *pH)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (!pH) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    if (hwThd >= CONFIG_MAX_HWTHREADS) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }
    return procEvtSets[hEvtSet]->Punit_GetHandles4Thread(hwThd, pH);
}



int Bgpm_Punit_GetLLHandles(unsigned hEvtSet, Bgpm_Punit_LLHandles_t *pH)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (!pH) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    return procEvtSets[hEvtSet]->Punit_GetLLHandles(pH);
}



const char * Bgpm_GetEventLabel(unsigned hEvtSet, unsigned evtIdx)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    int rc = procEvtSets.ValidSetIdx(hEvtSet, AnyThreadOk);
    if (UNLIKELY(rc)) return NULL;

    return procEvtSets[hEvtSet]->GetEventLabel(evtIdx);
}



const char * Bgpm_GetEventIdLabel(unsigned evtId)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    if (evtId <= PEVT_PUNIT_LAST_EVENT) return punitEvtTbl[evtId].label;
    if (evtId <= PEVT_L2UNIT_LAST_EVENT) return l2unitEvtTbl[evtId-PEVT_PUNIT_LAST_EVENT-1].label;
    if (evtId <= PEVT_IOUNIT_LAST_EVENT) return IOunitEvtTbl[evtId-PEVT_L2UNIT_LAST_EVENT-1].label;
    if (evtId <= PEVT_NWUNIT_LAST_EVENT) return NWunitEvtTbl[evtId-PEVT_IOUNIT_LAST_EVENT-1].label;
    if (evtId <= PEVT_CNKUNIT_LAST_EVENT) return CNKunitEvtTbl[evtId-PEVT_NWUNIT_LAST_EVENT-1].label;

    lastErr.PrintOrExit(UPCI_EINV_EVENT, BGPM_ERRLOC);
    return NULL;
}



int Bgpm_GetEventId(unsigned hEvtSet, unsigned evtIdx)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->GetEventId(evtIdx);
}



int Bgpm_GetEventIndex(unsigned hEvtSet, unsigned evtId, unsigned startIdx)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (UNLIKELY(!procEvtSets[hEvtSet]->IsEventIdCompatible(evtId))) {
        if (procEvtSets[hEvtSet]->IsEventIdValid(evtId)) return lastErr.PrintOrExit(BGPM_ECONF_UNIT, BGPM_ERRLOC);
        else                                             return lastErr.PrintOrExit(BGPM_EINV_EVENT, BGPM_ERRLOC);
    }

    return procEvtSets[hEvtSet]->GetEventIndex(evtId, startIdx);
}



int Bgpm_SetContext(unsigned hEvtSet, Bgpm_Context context)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    int rc = AllocateUnassignedEvtSet(hEvtSet, PEVT_AXU_INSTR_COMMIT);  // use any punit event
    if (rc) return rc;

    if ( ((int)context < (int)BGPM_CTX_DEFAULT) || ((int)context > (int)BGPM_CTX_BOTH) ) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    return procEvtSets[hEvtSet]->SetContext(context);
}



int Bgpm_GetContext(unsigned hEvtSet, Bgpm_Context *pContext)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (UNLIKELY(pContext == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    return procEvtSets[hEvtSet]->GetContext(pContext);
}



int Bgpm_AllowMixedContext(unsigned hEvtSet)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    int rc = AllocateUnassignedEvtSet(hEvtSet, PEVT_AXU_INSTR_COMMIT);  // use any punit event
    if (rc) return rc;

    return procEvtSets[hEvtSet]->AllowMixedContext();
}



int Bgpm_SetMultiplex(unsigned hEvtSet, uint64_t period, int normalize)
{
    BGPM_TRACE_L1;
    if (UNLIKELY(! thdInited)) {
        lastErr.Init();
        lastErr.SetTopLevelFunc(__func__);
        return lastErr.PrintOrExit(BGPM_ENOT_INIT, BGPM_ERRLOC);
    }
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if(normalize < 0 || normalize > 1) 
    {
      return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    EvtSet *pSet = procEvtSets[hEvtSet];
    int rc = 0;
    // make sure no events have been assigned yet
    if (pSet->ESType() != EvtSet::ESType_Unassigned ) {
        return lastErr.PrintOrExit(BGPM_ENOT_EMPTY, BGPM_ERRLOC);
    }

    EvtSet *pNewEvtSet = new ES_MXPunit(Kernel_ProcessorID(), period, normalize);
    if (pNewEvtSet->GetCrtRC() < 0) {
        rc = pNewEvtSet->GetCrtRC();
        lastErr.PrintOrExit(rc, BGPM_ERRLOC);
        delete pNewEvtSet;
        pNewEvtSet = NULL;
        return rc;
    }

    pNewEvtSet->user1 = pSet->user1;
    pNewEvtSet->user2 = pSet->user2;
    procEvtSets.ReplaceSet(hEvtSet,pNewEvtSet);


    BGPM_EXEMPT_DATA(fprintf(stderr, "%s" _AT_ "\n", IND_STRG); procEvtSets[hEvtSet]->Dump(IND_LVL));
    return rc;
}
int Bgpm_SetMultiplex2(unsigned hEvtSet, uint64_t period, int normalize,RsvMuxCtrs maxEventsPerGroup)
{
    BGPM_TRACE_L1;
    if (UNLIKELY(! thdInited)) {
        lastErr.Init();
        lastErr.SetTopLevelFunc(__func__);
        return lastErr.PrintOrExit(BGPM_ENOT_INIT, BGPM_ERRLOC);
    }
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if(normalize < 0 || normalize > 1)
    {
      return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    EvtSet *pSet = procEvtSets[hEvtSet];
    int rc = 0;
    // make sure not events have been assigned yet
    if (pSet->ESType() != EvtSet::ESType_Unassigned ) {
      return lastErr.PrintOrExit(BGPM_ENOT_EMPTY, BGPM_ERRLOC);
    }
    EvtSet *pNewEvtSet = new ES_MXPunit(Kernel_ProcessorID(), period, normalize,maxEventsPerGroup);
    if (pNewEvtSet->GetCrtRC() < 0) {
       rc = pNewEvtSet->GetCrtRC();
       lastErr.PrintOrExit(rc, BGPM_ERRLOC);
       delete pNewEvtSet;
       pNewEvtSet = NULL;
       return rc;
    }

    pNewEvtSet->user1 = pSet->user1;
    pNewEvtSet->user2 = pSet->user2;
    procEvtSets.ReplaceSet(hEvtSet,pNewEvtSet);
    BGPM_EXEMPT_DATA(fprintf(stderr, "%s" _AT_ "\n", IND_STRG); procEvtSets[hEvtSet]->Dump(IND_LVL));
    return rc;
}



int Bgpm_GetMultiplex(unsigned hEvtSet, Bgpm_MuxStats_t* pMuxStats)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    if (UNLIKELY(pMuxStats == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->GetMux(pMuxStats);
}



int Bgpm_SwitchMux(unsigned hEvtSet)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->SwitchMuxGrp();
}



uint64_t Bgpm_GetMuxElapsedCycles(unsigned hEvtSet, unsigned muxGrpNum)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->GetMuxCycles(muxGrpNum);
}



uint64_t Bgpm_GetMuxEventElapsedCycles(unsigned hEvtSet, unsigned evtIdx)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->GetMuxEventCycles(evtIdx, BGPM_TRUE);
}



int Bgpm_GetMuxGroups(unsigned hEvtSet)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->GetMuxGrps();
}



int Bgpm_LibLinked() { return 1; }



__END_DECLS
