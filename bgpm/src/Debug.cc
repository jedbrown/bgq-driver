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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <assert.h>

#include "globals.h"
#include "Debug.h"
#include "ShmemBlk.h"
#include "EvtSetList.h"
#include "ES_SWPunit.h"
#include "ES_HWPunit.h"
#include "ES_IOunit.h"
#include "ES_L2unit.h"
#include "ES_NWunit.h"
#include "ES_CNKunit.h"
#include "ES_MXPunit.h"
#include "spi/include/upci/testlib/upci_debug.h"



using namespace bgpm;


__thread char TraceFunc::indentStrg[256] = "\0";
__thread int TraceFunc::indentLvl = 0;
__thread int TraceFunc::locInit = 0;
__thread bool TraceFunc::prtEnabled = false;


extern "C" void Bgpm_EnableTracePrint() {
    TraceFunc::prtEnabled = true;
    fprintf(stderr, "Bgpm_EnableTracePrint\n");
}
extern "C" void Bgpm_DisableTracePrint() {
    fprintf(stderr, "Bgpm_DisableTracePrint\n");
    TraceFunc::prtEnabled = false;
}


TraceFunc::TraceFunc(const char *func, const char *at) {
    if (locInit == 0) {
        locInit = sprintf(indentStrg, "BGPM_TRACE (%02d) ", Kernel_ProcessorID());
    }
    strcpy(func_, func);
    strcpy(at_, at);
    if (prtEnabled) {
        fprintf(stderr, "%sEntering %s:%s\n", indentStrg, func_, at_);
    }
    indentLvl++;
    indentStrg[locInit+indentLvl*2] = '\0';  // make sure terminated before overlay with blanks in case signal interrupt
    mbar();
    memset(&indentStrg[locInit], ' ', indentLvl*2);
};

TraceFunc::~TraceFunc() {
    indentLvl--;
    //memset(indentStrg, ' ', indentLvl*2);
    indentStrg[locInit+indentLvl*2] = '\0';
    #if (BGPM_TRACE_OUT > 0)
    if (prtEnabled) {
        fprintf(stderr, "%sExiting  %s:%s\n", indentStrg, func_, at_);
    }
    #endif
};



#define INIT_INDENT() \
    char indentStrg[156];  \
    int ipos = sprintf(indentStrg, "BGPM_TRACE (%02d) ", Kernel_ProcessorID()); \
    memset(&indentStrg[ipos], ' ', indent*2); \
    indentStrg[ipos+indent*2] = '\0';



extern "C" void Bgpm_DumpPunitHandles(Bgpm_Punit_Handles_t *pH, unsigned indent)
{
    INIT_INDENT();
    assert(pH);
    fprintf(stderr, "%sPunitHandles: allCtrMask(0x%08lx) numCtrs(%d):\n",
            indentStrg, pH->allCtrMask, pH->numCtrs);
    for (unsigned i=0; i<pH->numCtrs; ++i) {
        fprintf(stderr, "%s  [%02d] @=0x%016lx\n", indentStrg, i, pH->hCtr[i]);
    }
}




extern "C" void Bgpm_DumpPunitLLHandles(Bgpm_Punit_LLHandles_t *pH, unsigned indent)
{
    INIT_INDENT();
    assert(pH);
    fprintf(stderr, "%sPunitLLHandles: allCtrMask(0x%08lx) numCtrs(%d):\n",
            indentStrg, pH->allCtrMask, pH->numCtrs);
    for (unsigned i=0; i<pH->numCtrs; ++i) {
        fprintf(stderr, "%s  [%02d] @=0x%016lx\n", indentStrg, i, pH->hCtr[i]);
    }
}






extern "C" int Bgpm_DumpEvtSet(unsigned hEvtSet, unsigned indentLvl)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    procEvtSets[hEvtSet]->Dump(indentLvl);
    return 0;
}



extern "C" int Bgpm_DumpEvtSetList(unsigned indentLvl)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    procEvtSets.Dump(indentLvl);
    return 0;
}



extern "C" void Bgpm_DebugDumpShrMem()
{
    if (pThdShMem == NULL) {
        fprintf(stderr, _AT_ " pThdShMem = NULL\n");
    }
    else {
        pThdShMem->Dump(IND_LVL);
    }
}



extern "C" void Bgpm_DumpUPCPState()
{
    UPC_P_Dump_State_Indent(-1, IND_LVL); \
    Upci_A2PC_Val_t a2qry; \
    Kernel_Upci_A2PC_GetRegs(&a2qry); \
    Upci_A2PC_DumpRegs_Indent(&a2qry, IND_LVL); \
}



void CoreReservations::Dump(unsigned indent, unsigned coreIdx)
{
    INIT_INDENT();
    printf("%sCoreReservations[%d] (0x%016lx):\n", indentStrg, coreIdx, (uint64_t)this);
    printf("%s  lock(%d), refCount(%d), allocMask(0x%08x)\n", indentStrg, lock, refCount, allocMask);
    printf("%s  alloc(%d), l1pMode(%d)\n", indentStrg, Alloc_l1pMode & allocMask, l1pMode);
    printf("%s  alloc(%d), ctxBoth(%d), evtCtx(%d)\n", indentStrg,  Alloc_evtCtx & allocMask, Alloc_evtCtxBoth & allocMask, evtCtx);
    printf("%s  alloc(%d), qfpuMatch(%d), qfpuMask(%d), qfpuScale(%d)\n", indentStrg, Alloc_qfpuMatch & allocMask, qfpuMatch, qfpuMask, qfpuScale);
    printf("%s  alloc(%d), xuMatch(0x%04x), xuMask(0x%04x)\n", indentStrg, Alloc_xuMatch & allocMask, xuMatch, xuMask);
    printf("%s  alloc(%d), sqrtScale(%d)\n", indentStrg, Alloc_sqrtScale & allocMask, sqrtScale);
    printf("%s  alloc(%d), divScale(%d)\n", indentStrg, Alloc_divScale & allocMask, divScale);
    printf("%s  puAttachedMask(0x%02x)\n", indentStrg, puAttachedMask);
    for (unsigned hwThd=0; hwThd<CONFIG_HWTHREADS_PER_CORE; ++hwThd) {
        printf("%s  preRsvSignals[%d](0x%016lx), preRsvCtrs[%d](0x%08x)\n", indentStrg, hwThd, preRsvSignals[hwThd], hwThd, preRsvCtrs[hwThd]);
    }

    for (unsigned hwThd=0; hwThd<CONFIG_HWTHREADS_PER_CORE; ++hwThd) {
        printf("%s  ctrGrpMask[%d](0x%02x), ctrGrpRefCount[%d](%d)\n",
                indentStrg, hwThd, ctrGrpMask[hwThd], hwThd, ctrGrpRefCount[hwThd]);
    }
}



void ShmemBlk::Dump(unsigned indent, int core)
{
    INIT_INDENT();
    fprintf(stderr,"%sShmemBlk:\n", indentStrg);
    fprintf(stderr,"%s  refLock(%d), thdRefCount(%d), timesInited(%d), inited(%d), dbgLock(%d)\n",
            indentStrg, refLock, refCount, timesInited, inited, dbgLock);
    fprintf(stderr,"%s  globalModeLock(%d), globalMode(%d), perspect(%d), globalModeInited(%d), envFeedback(%d)\n",
            indentStrg, globalModeLock, globalMode, perspect, globalModeInited, evtFeedback);
    Upci_Mode_Dump(indent+1, &upciMode);
    fprintf(stderr,"%s  l2ModeLock(%d), l2ModeRefCount(%d), combineL2(%d)\n",
            indentStrg, l2ModeLock, l2ModeRefCount, combineL2);
    fprintf(stderr,"%s  linkRefLock(%d)\n", indentStrg, linkRefLock);
    for (unsigned i=0; i<UPC_NW_NUM_LINKS; i++) {
        fprintf(stderr,"%s  [%02d] linkRefCounts(%d) linkController(%d) linkStartCycles(0x%016lx) linkElapsedCycles(0x%016lx)\n",
                indentStrg, i, linkRefCounts[i], linkController[i], linkStartCycles[i], linkElapsedCycles[i]);
    }

    int startCore = 0;
    int lastCore = CONFIG_MAX_CORES - 1;
    if (core >=0) { startCore = lastCore = core; }
    for (core=startCore; core<=lastCore; core++) {
        coreRsv[core].Dump(indent+1, core);
    }
}



void SharedControlIO::Dump(unsigned indent)
{
    INIT_INDENT();
    fprintf(stderr,"%sSharedIOControl:\n", indentStrg);
    fprintf(stderr,"%s  lock(%d), refCount(%d), running(%d), ovfMask(0x%016lx)\n",
            indentStrg, lock, refCount, running, ovfMask);
    for (int i=0; i<UPC_IO_Counter_Range; i++) {
        fprintf(stderr,"%s    [%02d] accumVals(0x%016lx) thresholds(0x%016lx)\n",
                indentStrg, i, accumVals[i], thresholds[i]);
    }
}



void SharedControlL2::Dump(unsigned indent)
{
    INIT_INDENT();
    fprintf(stderr,"%sSharedL2Control:\n", indentStrg);
    fprintf(stderr,"%s  lock(%d), refCount(%d), running(%d), ovfMask(0x%016lx)\n",
            indentStrg, lock, refCount, running, ovfMask);
    for (int i=0; i<UPC_IO_Counter_Range; i++) {
        fprintf(stderr,"%s    [%02d] accumVals(0x%016lx) thresholds(0x%016lx)\n",
                indentStrg, i, accumVals[i], thresholds[i]);
    }
}



void EvtSetList::Dump(unsigned indent)
{
    INIT_INDENT();
    DumpCounts();
    unsigned idx;
    for (idx = 0; idx < numRecs; ++idx) {
        if (pSets[idx].ctlThdId >= 0) {
            fprintf(stderr, "%sset[%d]: ctlThdId=%d\n", indentStrg, idx, pSets[idx].ctlThdId);
            pSets[idx].pEvtSet->Dump(indent+2);
        }
    }
}


void EvtSet::Dump(unsigned indent) {
    INIT_INDENT();
    fprintf(stderr, "%s%s: crtRC(%d), inSigHandler(%d), user1(0x%016lx), user2(0x%016lx)\n",
            indentStrg, ESTypeLabel(), crtRC, inSigHandler, user1, user2);
}


void ES_SWPunit::PunitEvtRec::Dump(unsigned indent, unsigned idx) {
    INIT_INDENT();
    fprintf(stderr, "%s[%02d]: puEvtIdx(%02d), pPunit(0x%016lx), "
            "evtAttrMask(0x%03x), user1(0x%016lx), user2(0x%016lx)\n",
            indentStrg, idx, puEvtIdx, (uint64_t)pPunit, evtAttrMask,
            user1, user2);
}



void ES_SWPunit::Dump(unsigned indent, bool limit) {
    INIT_INDENT();
    this->EvtSet::Dump(indent);
    //fprintf(stderr, "%s%s: user1(0x%016lx), user2(0x%016lx)\n", indentStrg, ESTypeLabel(), user1, user2);
    fprintf(stderr, "%s  swModeAppliedPuEvtSet=%d,  running=%d\n", indentStrg, swModeAppliedPuEvtSet, running);
    fprintf(stderr, "%s  targCore(%d), targHwThd(%d), coreRsvMask(0x%03x), appliedRsvMask(0x%03x), ctrGrpRvd(%d), frozen(%d), crtRC(%d)\n",
                    indentStrg, targCore, targHwThd, coreRsvMask, appliedRsvMask, ctrGrpRvd, frozen, crtRC);
    fprintf(stderr, "%s  EvtRecs=%d:\n", indentStrg, (int)evtRecs.size());
    for (unsigned i=0; i<evtRecs.size(); ++i) {
        evtRecs[i].Dump(indent+1, i);
    }
    fprintf(stderr, "%s  puList.size()=%d, activePunitIdx=%02d\n", indentStrg, (int)puList.size(), activePunitIdx);
    if (limit) {
        fprintf(stderr, "%s  puList[%02d]:\n", indentStrg, activePunitIdx);
        Upci_Punit_Dump(indent+2, puList[activePunitIdx]);
    }
    else {
        for (unsigned i=0; i<puList.size(); ++i) {
            fprintf(stderr, "%s  puList[%02d]:\n", indentStrg, i);
            Upci_Punit_Dump(indent+2, puList[i]);
        }
    }
    coreRsv.Dump(indent+1, targCore);
    fprintf(stderr, "%s  pOvfHandler(0x%016lx)\n", indentStrg, (uint64_t)pOvfHandler);
}



void ES_SWPunit::Dump(unsigned indent) {
    Dump(indent, false);
}


void ES_HWPunit::Dump(unsigned indent) {
    INIT_INDENT();
    ES_SWPunit::Dump(indent);
    unsigned idx;
    unsigned numTargets = 0;
    for (idx=0; idx<targPuList.size(); idx++) if (targPuList[idx]) numTargets++;

    fprintf(stderr, "%s  Targets: (%d)\n", indentStrg, numTargets);
    for (idx=0; idx<targPuList.size(); idx++) {
        if (targPuList[idx]) {
            fprintf(stderr, "%s    [%02d]: targAppliedRsvMask(0x%08x), targAttachedCtrGrpMask(0x%02x)\n",
                    indentStrg, idx, targAppliedRsvMask[idx], targAttachedCtrGrpMask[idx]);
            Upci_Punit_Dump(indent+5, targPuList[idx]);
        }
    }
}



void ES_MXPunit::Dump(unsigned indent) {
    INIT_INDENT();

    this->ES_SWPunit::Dump(indent);

    fprintf(stderr, "%s  maxMuxEvtsPerPunit(%d), numMuxSwitches(%ld), muxPeriod(0x%016lx), normalizeEvents(%d), muxSwitchTransCount(%ld)\n",
            indentStrg, maxMuxEvtsPerPunit, numMuxSwitches, muxPeriod, normalizeEvents, muxSwitchTransCount);

    for (unsigned i=0; i<puList.size(); ++i) {
        fprintf(stderr, "%s    grp[%02d]  muxCycleEvtIdx=%03d, muxIntStat=0x%016lx, muxApplies=%d\n",
                indentStrg, i, muxCycleEvtIdx[i], muxIntStat[i], muxApplies[i]);
    }
}



void ES_IOunit::IOunitEvtRec::Dump(unsigned indent, unsigned idx) {
    INIT_INDENT();
    fprintf(stderr, "%s[%d]: pEvent(0x%016lx), ctrIdx(%d), evtAttrMask(0x%03x), user1(0x%016lx), user2(0x%016lx)\n",
            indentStrg, idx, (uint64_t)pEvent, ctrIdx, evtAttrMask, user1, user2);
}

void ES_IOunit::Dump(unsigned indent) {
    INIT_INDENT();
    fprintf(stderr, "%s%s: user1(0x%016lx), user2(0x%016lx)\n", indentStrg, ESTypeLabel(), user1, user2);
    fprintf(stderr, "%s  ovfInUse(%d), pOvfHandler(0x%016lx), frozen(%d), crtRC(%d)\n", indentStrg, ovfInUse, (uint64_t)pOvfHandler, frozen, crtRC);
    for (unsigned i=0; i<evtRecs.size(); ++i) {
        evtRecs[i].Dump(indent+1, i);
    }
}




void ES_L2unit::L2unitEvtRec::Dump(unsigned indent, unsigned idx) {
    INIT_INDENT();
    fprintf(stderr, "%s[%d]: pEvent(0x%016lx), ctrIdx(%d), evtAttrMask(0x%03x), user1(0x%016lx), user2(0x%016lx)\n",
            indentStrg, idx, (uint64_t)pEvent, ctrIdx, evtAttrMask, user1, user2);
}

void ES_L2unit::Dump(unsigned indent) {
    INIT_INDENT();
    fprintf(stderr, "%s%s: user1(0x%016lx), user2(0x%016lx) combine(%d)\n", indentStrg, ESTypeLabel(), user1, user2, combine);
    fprintf(stderr, "%s  ovfInUse(%d), pOvfHandler(0x%016lx), frozen(%d), crtRC(%d)\n", indentStrg, ovfInUse, (uint64_t)pOvfHandler, frozen, crtRC);
    for (unsigned i=0; i<evtRecs.size(); ++i) {
        evtRecs[i].Dump(indent+1, i);
    }
}




void ES_NWunit::NWunitEvtRec::Dump(unsigned indent, unsigned idx) {
    INIT_INDENT();
    fprintf(stderr, "%s[%d]: pEvent(0x%016lx), ctrIdx(%d), vchanMask(0x%08x), user1(0x%016lx), user2(0x%016lx)\n",
            indentStrg, idx, (uint64_t)pEvent, ctrIdx, vchanMask, user1, user2);
}

void ES_NWunit::Dump(unsigned indent) {
    INIT_INDENT();
    fprintf(stderr, "%s%s: user1(0x%016lx), user2(0x%016lx)\n", indentStrg, ESTypeLabel(), user1, user2);
    fprintf(stderr, "%smappedCtrMask(0x%04x), attachedLinkMask(0x%04x), attachedCtlMask(0x%04x)\n",
            indentStrg, mappedCtrMask, attachedLinkMask, attachedCtlMask);

    for (unsigned i=0; i<evtRecs.size(); ++i) {
        evtRecs[i].Dump(indent+1, i);
    }
}


void ES_CNKunit::CNKEvtRec::Dump(unsigned indent, unsigned idx) {
    INIT_INDENT();
    fprintf(stderr, "%s[%02d]: pEvent(0x%016lx), ctrType(%d), buffIdx(%d), accumVal(0x%016lx),\n"
                    "%s      startVal(0x%016lx), user1(0x%016lx), user2(0x%016lx)\n",
            indentStrg, idx, (uint64_t)pEvent, ctrType, buffIdx, accumVal,
            indentStrg, startVal, user1, user2);
}

void ES_CNKunit::Dump(unsigned indent) {
    INIT_INDENT();
    fprintf(stderr, "%s%s: running(%d), user1(0x%016lx), user2(0x%016lx)\n", indentStrg, ESTypeLabel(), running, user1, user2);

    for (unsigned i=0; i<evtRecs.size(); ++i) {
        evtRecs[i].Dump(indent+1, i);
    }
    for (unsigned i=0; i<=(unsigned)CNKCtrHWT; ++i) {
        fprintf(stderr, "%s  ctrBuffs[%d]:\n", indentStrg, i);
        CNKCntBuff & ctrBuff = ctrBuffs[i];
        for (unsigned j=0; j<ctrBuff.size(); ++j) {
            fprintf(stderr, "%s    [%d] id=%ld, val=0x%016lx\n", indentStrg, j, ctrBuff[j].id, ctrBuff[j].value);
        }
    }
}


#ifndef NDEBUG
extern "C" Upci_Punit_t * DebugGetPunit(unsigned hEvtSet, unsigned punitIdx)
{
    short priorExitVal = Bgpm_ExitOnError(true);

    procEvtSets.ValidSetIdx(hEvtSet, MustBeControlThread);
    Upci_Punit_t *ret = procEvtSets[hEvtSet]->DebugGetPunit(punitIdx);

    Bgpm_ExitOnError(priorExitVal);
    return ret;
}
extern "C" Upci_Punit_t * DebugGetTargPunit(unsigned hEvtSet, unsigned punitIdx)
{
    short priorExitVal = Bgpm_ExitOnError(true);

    procEvtSets.ValidSetIdx(hEvtSet, MustBeControlThread);
    Upci_Punit_t *ret = procEvtSets[hEvtSet]->DebugGetTargPunit(punitIdx);

    Bgpm_ExitOnError(priorExitVal);
    return ret;
}
#endif



const BgpmDebugThreadVars_t *Bgpm_DebugGetThreadVars()
{
    return &dbgThdVars;
}



void BgpmDebugPrintThreadVars()
{
    const BgpmDebugThreadVars_t *pDbgCnts = Bgpm_DebugGetThreadVars();

    fprintf(stderr, "%sBgpm Threaded Debug Counts:\n", IND_STRG);
    fprintf(stderr, "%s    ovfEnabled                 = %8d\n",  IND_STRG, pDbgCnts->ovfEnabled);
    fprintf(stderr, "%s    numPmSignals               = %8ld\n", IND_STRG, pDbgCnts->numPmSignals);
    fprintf(stderr, "%s    numFastOvfs                = %8ld\n", IND_STRG, pDbgCnts->numFastOvfs);
    fprintf(stderr, "%s    numMuxOvfs                 = %8ld\n", IND_STRG, pDbgCnts->numMuxOvfs);
    fprintf(stderr, "%s    numSharedUpcpOvfs          = %8ld\n", IND_STRG, pDbgCnts->numSharedUpcpOvfs);
    fprintf(stderr, "%s    numSharedUpcpOvfsDelivered = %8ld\n", IND_STRG, pDbgCnts->numSharedUpcpOvfsDelivered);
    fprintf(stderr, "%s    numUpcCOvfs                = %8ld\n", IND_STRG, pDbgCnts->numUpcCOvfs);
    fprintf(stderr, "%s    numUpcCOvfsDelivered       = %8ld\n", IND_STRG, pDbgCnts->numUpcCOvfsDelivered);
    fprintf(stderr, "%s    numPmSigsWOInt             = %8ld\n", IND_STRG, pDbgCnts->numPmSigsWOInt);
    fprintf(stderr, "%s    droppedSharedUpcPOvfs      = %8ld\n", IND_STRG, pDbgCnts->droppedSharedUpcPOvfs);
    fprintf(stderr, "%s    maxUpcPSharedQueLen        = %8ld\n", IND_STRG, pDbgCnts->maxUpcPSharedQueLen);
    fprintf(stderr, "%s    droppedSharedUpcCOvfs      = %8ld\n", IND_STRG, pDbgCnts->droppedSharedUpcCOvfs);
    fprintf(stderr, "%s    maxUpcCSharedQueLen        = %8ld\n", IND_STRG, pDbgCnts->maxUpcCSharedQueLen);
}



