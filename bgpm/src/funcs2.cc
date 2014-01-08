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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "bgpm/include/bgpm.h"
#include "EvtSetList.h"
#include "globals.h"
#include "ES_Factory.h"



using namespace bgpm;


__BEGIN_DECLS




int Bgpm_SetEventSetUser1(unsigned hEvtSet, uint64_t value)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    procEvtSets[hEvtSet]->user1 = value;
    return 0;
}


int Bgpm_GetEventSetUser1(unsigned hEvtSet, uint64_t *pValue)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    if (UNLIKELY(pValue == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    *pValue = procEvtSets[hEvtSet]->user1;
    return 0;
}


int Bgpm_SetEventSetUser2(unsigned hEvtSet, uint64_t value)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    procEvtSets[hEvtSet]->user2 = value;
    return 0;
}


int Bgpm_GetEventSetUser2(unsigned hEvtSet, uint64_t *pValue)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    if (UNLIKELY(pValue == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    *pValue = procEvtSets[hEvtSet]->user2;
    return 0;
}


int Bgpm_SetEventUser1(unsigned hEvtSet, unsigned evtIdx, uint64_t value)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->SetEvtUser1(evtIdx, value);
}


int Bgpm_GetEventUser1(unsigned hEvtSet, unsigned evtIdx, uint64_t *pValue)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    if (UNLIKELY(pValue == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->GetEvtUser1(evtIdx, pValue);
}


int Bgpm_SetEventUser2(unsigned hEvtSet, unsigned evtIdx, uint64_t value)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->SetEvtUser2(evtIdx, value);
}


int Bgpm_GetEventUser2(unsigned hEvtSet, unsigned evtIdx, uint64_t *pValue)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    if (UNLIKELY(pValue == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->GetEvtUser2(evtIdx, pValue);
}


int Bgpm_SetOverflowHandler(unsigned hEvtSet, Bgpm_OverflowHandler_t handler)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->SetOverflowHandler(handler);
}


int Bgpm_SetOverflow(unsigned hEvtSet, unsigned evtIdx, uint64_t period)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    BGPM_TRACE_DATA_L2(
            fprintf(stderr, "%s" _AT_ " hwtid=%02d hEvtSet=%d, evtIdx==%d, period=%ld\n", IND_STRG, Kernel_ProcessorID(), hEvtSet, evtIdx, period)
            )
    ;

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    uint64_t threshold =  UPCI_PERIOD2THRES(period);
    if (UNLIKELY(threshold < BGPM_THRESHOLD_MIN)) {
        return lastErr.PrintOrExit(BGPM_EINV_OVF, BGPM_ERRLOC);
    }

    return procEvtSets[hEvtSet]->SetOverflow(evtIdx, threshold);
}



int Bgpm_GetOverflow(unsigned hEvtSet, unsigned evtIdx, uint64_t *pPeriod, Bgpm_OverflowHandler_t *pHandler)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (UNLIKELY((pPeriod == NULL) || (pHandler == NULL))) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    uint64_t threshold;
    int rc = procEvtSets[hEvtSet]->GetOverflow(evtIdx, &threshold);
    if (rc == 0) {
        *pHandler = procEvtSets[hEvtSet]->GetOverflowHandler();
        *pPeriod = UPCI_THRES2PERIOD(threshold);
    }
    return rc; 

}



int Bgpm_GetOverflowEventIndices(unsigned hEvtSet, uint64_t ovfVector, unsigned *pIndicies, unsigned *pLen)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (UNLIKELY((pIndicies == NULL) || (pLen == NULL))) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    return procEvtSets[hEvtSet]->GetOverflowIndices(ovfVector, pIndicies, pLen);
}



int Bgpm_SetQfpuGrpMask(unsigned hEvtSet, unsigned evtIdx, uint64_t mask)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->SetQfpuGrpMask(evtIdx, mask);
}



int Bgpm_GetQfpuGrpMask(unsigned hEvtSet, unsigned evtIdx, uint64_t *pMask)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (UNLIKELY(pMask == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    return procEvtSets[hEvtSet]->GetQfpuGrpMask(evtIdx, pMask);
}



int Bgpm_SetXuGrpMask(unsigned hEvtSet, unsigned evtIdx, uint64_t mask)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->SetXuGrpMask(evtIdx, mask);
}



int Bgpm_GetXuGrpMask(unsigned hEvtSet, unsigned evtIdx, uint64_t *pMask)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (UNLIKELY(pMask == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    return procEvtSets[hEvtSet]->GetXuGrpMask(evtIdx, pMask);
}



int Bgpm_SetQfpuFp(unsigned hEvtSet, unsigned evtIdx, ushort countFp)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->SetCountFP(evtIdx, countFp);
}



int Bgpm_GetQfpuFp(unsigned hEvtSet, unsigned evtIdx, ushort *pCountFp)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (UNLIKELY(pCountFp == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    ushort doFp;
    int rc = procEvtSets[hEvtSet]->GetCountFP(evtIdx, &doFp);
    *pCountFp = (doFp != 0);
    //fprintf(stderr, _AT_ " evtIdx=%d, *pCountFp=%d\n", evtIdx, *pCountFp);
    return rc;
}



int Bgpm_SetXuMatch(unsigned hEvtSet, unsigned evtIdx, uint16_t match, uint16_t mask)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->SetXuMatch(evtIdx, match, mask);
}



int Bgpm_GetXuMatch(unsigned hEvtSet, unsigned evtIdx, uint16_t *pMatch, uint16_t *pMask)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (UNLIKELY(pMatch == NULL) || (pMask == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    return procEvtSets[hEvtSet]->GetXuMatch(evtIdx, pMatch, pMask);
}


int Bgpm_SetQfpuMatch(unsigned hEvtSet, unsigned evtIdx, uint16_t match, uint16_t mask, ushort fpScale)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->SetQfpuMatch(evtIdx, match, mask, fpScale);
}



int Bgpm_GetQfpuMatch(unsigned hEvtSet, unsigned evtIdx, uint16_t *pMatch, uint16_t *pMask, ushort *pFpScale)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (UNLIKELY(pMatch == NULL) || (pMask == NULL) || (pFpScale == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    return procEvtSets[hEvtSet]->GetQfpuMatch(evtIdx, pMatch, pMask, pFpScale);
}



int Bgpm_SetFpSqrScale(unsigned hEvtSet, ushort fpScale)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->SetFpSqrScale(fpScale);
}



int Bgpm_GetFpSqrScale(unsigned hEvtSet, ushort *pFpScale)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (UNLIKELY(pFpScale == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    return procEvtSets[hEvtSet]->GetFpSqrScale(pFpScale);
}



int Bgpm_SetFpDivScale(unsigned hEvtSet, ushort fpScale)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->SetFpDivScale(fpScale);
}



int Bgpm_GetFpDivScale(unsigned hEvtSet, ushort *pFpScale)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (UNLIKELY(pFpScale == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    return procEvtSets[hEvtSet]->GetFpDivScale(pFpScale);
}



int Bgpm_SetEventEdge(unsigned hEvtSet, unsigned evtIdx, ushort setEdge)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    int rc = procEvtSets[hEvtSet]->SetEdge(evtIdx, setEdge);
    //fprintf(stderr, _AT_ " rc = %d\n", rc);
    return rc;
}



int Bgpm_GetEventEdge(unsigned hEvtSet, unsigned evtIdx, ushort *pEdge)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (UNLIKELY(pEdge == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    int rc = procEvtSets[hEvtSet]->GetEdge(evtIdx, pEdge);
    //fprintf(stderr, _AT_ " *pEdge=%d\n", *pEdge);
    return rc;
}



int Bgpm_SetEventInvert(unsigned hEvtSet, unsigned evtIdx, ushort invert)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->SetInvert(evtIdx, invert);
}



int Bgpm_GetEventInvert(unsigned hEvtSet, unsigned evtIdx, ushort *pInvert)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (UNLIKELY(pInvert == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    return procEvtSets[hEvtSet]->GetInvert(evtIdx, pInvert);
}



int Bgpm_PrintCurrentPunitReservations(unsigned hEvtSet)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->PrintCurrentReservations();

}



int Bgpm_GetEventIdInfo(unsigned evtId, Bgpm_EventInfo_t *pInfo)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);
    if (UNLIKELY(pInfo == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    memset((void*)pInfo, 0, sizeof(Bgpm_EventInfo_t));
    Bgpm_EventInfo_t &info = *pInfo;
    unsigned idx = 0;
    if (evtId == PEVT_UNDEF) {
        return lastErr.PrintOrExit(BGPM_EUNREC_EVTID, BGPM_ERRLOC);
    }
    else if (evtId <= PEVT_PUNIT_LAST_EVENT) {
        idx = evtId - PEVT_UNDEF;
        const UPC_PunitEvtTableRec_t & evtRec = punitEvtTbl[idx];
        info.evtId     = evtRec.evtId;
        info.label     = evtRec.label;
        info.desc      = evtRec.desc;
        info.unitType  = BGPMUnitType_PUNIT;
        info.srcType   = evtRec.evtSrc;
        info.scope     = (UPC_EventScope_t)evtRec.scope;
        info.metric    = (UPC_EventMetric_t)evtRec.metric;
        info.edge      = evtRec.cycle ? 0 : 1;
        info.inverted  = evtRec.invert;
        info.selVal    = evtRec.selVal;
    }
    else if (evtId <= PEVT_L2UNIT_LAST_EVENT) {
        idx = evtId - PEVT_PUNIT_LAST_EVENT - 1;
        const UPC_L2unitEvtTableRec_t & evtRec = l2unitEvtTbl[idx];
        info.evtId     = evtRec.evtId;
        info.label     = evtRec.label;
        info.desc      = evtRec.desc;
        info.unitType  = BGPMUnitType_L2;
        info.srcType   = evtRec.evtSrc;
        info.scope     = UPC_SCOPE_NODESHARED;
        info.metric    = UPC_METRIC_EVENTS;
        info.selVal    = evtRec.selVal;

    }
    else if (evtId <= PEVT_IOUNIT_LAST_EVENT) {
        idx = evtId - PEVT_L2UNIT_LAST_EVENT - 1;
        const UPC_IOunitEvtTableRec_t & evtRec = IOunitEvtTbl[idx];
        info.evtId     = evtRec.evtId;
        info.label     = evtRec.label;
        info.desc      = evtRec.desc;
        info.unitType  = BGPMUnitType_IO;
        info.srcType   = evtRec.evtSrc;
        info.scope     = UPC_SCOPE_NODESHARED;
        info.metric    = UPC_METRIC_EVENTS;
        info.selVal    = evtRec.selVal;
    }
    else if (evtId <= PEVT_NWUNIT_LAST_EVENT) {
        idx = evtId - PEVT_IOUNIT_LAST_EVENT - 1;
        const UPC_NWunitEvtTableRec_t & evtRec = NWunitEvtTbl[idx];
        info.evtId     = evtRec.evtId;
        info.label     = evtRec.label;
        info.desc      = evtRec.desc;
        info.unitType  = BGPMUnitType_NW;
        info.srcType   = evtRec.evtSrc;
        info.scope     = UPC_SCOPE_EXCLUSIVE;
        info.metric    = UPC_METRIC_EVENTS;
        info.selVal    = evtRec.selVal;
    }
    else if (evtId <= PEVT_CNKUNIT_LAST_EVENT) {
        idx = evtId - PEVT_NWUNIT_LAST_EVENT - 1;
        const UPC_CNKunitEvtTableRec_t & evtRec = CNKunitEvtTbl[idx];
        info.evtId     = evtRec.evtId;
        info.label     = evtRec.label;
        info.desc      = evtRec.desc;
        info.unitType  = BGPMUnitType_CNK;
        info.srcType   = evtRec.evtSrc;
        info.scope     = UPC_SCOPE_NODE;
        info.metric    = UPC_METRIC_EVENTS;
        info.selVal    = evtRec.selVal;
    }
    else {
        return lastErr.PrintOrExit(BGPM_EUNREC_EVTID, BGPM_ERRLOC);
    }
    return 0;
}



int Bgpm_GetUnitType(unsigned hEvtSet)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    EvtSet::ESTypes estype = procEvtSets[hEvtSet]->ESType();

    int rc = 0;
    switch (estype) {
        case EvtSet::ESType_LLPunit:
        case EvtSet::ESType_SWPunit:
        case EvtSet::ESType_HWPunit:
        case EvtSet::ESType_HWDetailPunit: rc = (int)BGPMUnitType_PUNIT; break;
        case EvtSet::ESType_L2unit: rc = (int)BGPMUnitType_L2; break;
        case EvtSet::ESType_IOunit: rc = (int)BGPMUnitType_IO; break;
        case EvtSet::ESType_NWunit: rc = (int)BGPMUnitType_NW; break;
        case EvtSet::ESType_CNKunit: rc = (int)BGPMUnitType_NW; break;
        default: rc = (int)BGPMUnitType_UNDEF;
    }
    return rc;
}



int Bgpm_GetEventIdFromLabel(const char *evtLabel)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    if (UNLIKELY(evtLabel == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    int idx = 0;
    int ret = PEVT_UNDEF;
    if (strncasecmp(evtLabel, "PEVT_L2_", 8) == 0) {
        while (idx < PEVT_L2UNIT_LAST_EVENT-PEVT_PUNIT_LAST_EVENT) {
            if (strcasecmp(evtLabel, l2unitEvtTbl[idx].label) == 0) {
                ret = l2unitEvtTbl[idx].evtId;
                break;
            }
            idx++;
        }
    }
    else if ((strncasecmp(evtLabel, "PEVT_MU_", 8) == 0) ||
             (strncasecmp(evtLabel, "PEVT_PCIE_", 10) == 0) ||
             (strncasecmp(evtLabel, "PEVT_DB_", 8) == 0)) {
        while (idx < PEVT_IOUNIT_LAST_EVENT-PEVT_L2UNIT_LAST_EVENT) {
            if (strcasecmp(evtLabel, IOunitEvtTbl[idx].label) == 0) {
                ret = IOunitEvtTbl[idx].evtId;
                break;
            }
            idx++;
        }
    }
    else if ((strncasecmp(evtLabel, "PEVT_NW_", 8) == 0)) {
        while (idx < PEVT_NWUNIT_LAST_EVENT-PEVT_IOUNIT_LAST_EVENT) {
            if (strcasecmp(evtLabel, NWunitEvtTbl[idx].label) == 0) {
                ret = NWunitEvtTbl[idx].evtId;
                break;
            }
            idx++;
        }
    }
    else if ((strncasecmp(evtLabel, "PEVT_CNK", 8) == 0) || 
             (strncasecmp(evtLabel, "PEVT_CNKHWT_", 12) == 0)) {
        while (idx < PEVT_CNKUNIT_LAST_EVENT-PEVT_NWUNIT_LAST_EVENT) {
            if (strcasecmp(evtLabel, CNKunitEvtTbl[idx].label) == 0) {
                ret = CNKunitEvtTbl[idx].evtId;
                break;
            }
            idx++;
        }
    }
    else if ((strncasecmp(evtLabel, "PEVT_", 5) == 0)) {
        while (idx <= PEVT_PUNIT_LAST_EVENT-PEVT_UNDEF) {
            if (strcasecmp(evtLabel, punitEvtTbl[idx].label) == 0) {
                ret = punitEvtTbl[idx].evtId;
                break;
            }
            idx++;
        }
    }

    return ret;
}



int Bgpm_GetLongDesc(unsigned evtId, char *pBuff, int *pLen)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);
    if (UNLIKELY((pBuff == NULL) || (pLen == NULL))) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    if (evtId > PEVT_CNKUNIT_LAST_EVENT) {
        return lastErr.PrintOrExit(BGPM_EUNREC_EVTID, BGPM_ERRLOC);
    }

    #define EMSGLEN 1023
    char emsg[EMSGLEN+1];
    pBuff[0] = '\0';
    int maxLen = *pLen;
    *pLen = 0;

    const char *descFile = envVars.DescFile();
    if (descFile == NULL) { descFile = LONG_DESC_FILE; }

    int fd = open(descFile, O_RDONLY, 0);
    if (fd < 0) {
        snprintf(emsg, EMSGLEN, "unable to open %s", descFile);
        return lastErr.PrintOrExitOp(-errno, emsg, BGPM_ERRLOC);
    }


    // seek to offset for this event.
    #define OFFSETS_REC_LEN 16
    int offset = OFFSETS_REC_LEN * evtId;
    int rc = lseek(fd, offset, SEEK_SET);
    if (rc < 0) {
        snprintf(emsg, EMSGLEN, "lseek for event %d, offset %d in %s", evtId, offset, descFile);
        return lastErr.PrintOrExitOp(-errno, emsg, BGPM_ERRLOC);
    }


    // read offset data for event
    char buff[40];
    rc = read(fd, (void*)buff, 18);
    buff[18] = '\0';
    if (rc < 0) {
        snprintf(emsg, EMSGLEN, "read for evtid %d, offset %d in file %s", evtId, offset, descFile);
        return lastErr.PrintOrExitOp(-errno, emsg, BGPM_ERRLOC);
    }
    char *savePtr = NULL;
    //fprintf(stderr, _AT_ " buff=%s\n", buff);
    char *strg1 = strtok_r(buff, " \n", &savePtr);
    char *strg2 = strtok_r(NULL, " \n", &savePtr);
    if ((strg1 == NULL) || (strg2 == NULL)) {
        snprintf(emsg, EMSGLEN, "strtok_r of offset data returned NULL for evtid %d, offset %d in file %s", evtId, offset, descFile);
        return lastErr.PrintOrExitOp(BGPM_ELONGDESC_CORRUPT, emsg, BGPM_ERRLOC);
    }
    int detOffset, detLen;
    detOffset = atoi(strg1);
    detLen = atoi(strg2);
    //fprintf(stderr, _AT_ " detOffset=%d, detLen=%d\n", detOffset, detLen);
    if ((detOffset < 100) || (detLen < 5)) {
        snprintf(emsg, EMSGLEN, "detOffset(%d) and detLen(%d) values are unexpected from evtid %d, offset %d, in file %s", detOffset, detLen, evtId, offset, descFile);
        return lastErr.PrintOrExitOp(BGPM_ELONGDESC_CORRUPT, emsg, BGPM_ERRLOC);
    }


    // Read Detail Data
    rc = lseek(fd, detOffset, SEEK_SET);
    if (rc < 0) {
        snprintf(emsg, EMSGLEN, "lseek for event %d detail offset %d in %s", evtId, detOffset, descFile);
        return lastErr.PrintOrExitOp(-errno, emsg, BGPM_ERRLOC);
    }

    maxLen = maxLen - 6;
    maxLen = UPCI_MIN(maxLen,detLen-6);
    unsigned detEvtId = 0;
    rc = read(fd, (void*)buff, 5);  // hardcode to 4 digits plus blank at head of detail
    buff[5] = '\0';
    if (rc < 0) {
        snprintf(emsg, EMSGLEN, "read failed for evtid %d, offset %d in file %s", evtId, detOffset, descFile);
        return lastErr.PrintOrExitOp(-errno, emsg, BGPM_ERRLOC);
    }
    savePtr = NULL;
    strg1 = strtok_r(buff, " \n", &savePtr);
    detEvtId = atoi(strg1);
    if (detEvtId != evtId) {
        snprintf(emsg, EMSGLEN, "unexpected detEvtId (%d) != evtId(%d) at offset %d in file %s", detEvtId, evtId, detOffset, descFile);
        return lastErr.PrintOrExitOp(BGPM_ELONGDESC_CORRUPT, emsg, BGPM_ERRLOC);
    }

    if (maxLen > 0) {
        rc = read(fd, (void*)pBuff, maxLen);
        if (rc < 0) {
            snprintf(emsg, EMSGLEN, "read of detail for evtid %d at offset %d for file %s", evtId, detOffset, descFile);
            return lastErr.PrintOrExitOp(-errno, emsg, BGPM_ERRLOC);
        }
        *pLen = maxLen;
        pBuff[maxLen]='\0';
    }

    close(fd);
    return 0;
}



int Bgpm_IsController(unsigned hEvtSet)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    //! \todo Add cross process check for NW Link controller
    //EvtSet::ESTypes estype = procEvtSets[hEvtSet]->ESType();
    //if (esType == ESType_NWunit()) {
    //    return procEvtSets[hEvtSet]->IsLinkController();
    //}
    if (procEvtSets.IsControlThread(hEvtSet)) {
        return 1;
    }
    return 0;
}



int Bgpm_SetEventVirtChannels(unsigned hEvtSet, unsigned evtIdx, UPC_NW_Vchannels vchan)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    CONTROLTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    return procEvtSets[hEvtSet]->SetVirtChannels(evtIdx, vchan);
}



int Bgpm_GetEventVirtChannels(unsigned hEvtSet, unsigned evtIdx, UPC_NW_Vchannels * pVchan)
{
    BGPM_TRACE_L1;
    lastErr.SetTopLevelFunc(__func__);

    READERTHREAD_VALIDATE_SET_OR_RETURN(hEvtSet);

    if (UNLIKELY(pVchan == NULL)) {
        return lastErr.PrintOrExit(BGPM_EINV_PARM, BGPM_ERRLOC);
    }

    return procEvtSets[hEvtSet]->GetVirtChannels(evtIdx, pVchan);
}





__END_DECLS
