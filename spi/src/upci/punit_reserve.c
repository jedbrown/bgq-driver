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

#include "spi/include/upci/punit_reserve.h"
#include "spi/include/upci/node_parms.h"
#include "spi/include/upci/upc_a2_pc.h"


extern const UPC_PunitEvtTableRec_t punitEvtTbl[];


#define PRINT_STEPS(_cmd_) //_cmd_



__INLINE__ unsigned Ctr2UnitSigNum(unsigned ctr)
{
    ctr %= 8;
    return ((ctr / 4) + ((ctr % 4) * 2));
}

//! GetA2SigNum
//! get a2 pc bus signal number given a unit signal number and event source.
//! @param[in]  unitSigNum     The a2 unit specific signal number (0-7)
//! @param[in]  evtSrc         The a2 unit event source
//! @returns    overall a2 signal number (0-39).
__INLINE__ unsigned GetA2PcSigNum(unsigned unitSigNum, UPC_EventSources_t evtSrc)
{
    switch (evtSrc) {
        case UPC_ES_A2_MMU: { break; }
        case UPC_ES_A2_AXU: { unitSigNum+=8; break; }
        case UPC_ES_A2_IU:  { unitSigNum+=16; break; }
        case UPC_ES_A2_LSU: { unitSigNum+=24; break; }
        case UPC_ES_A2_XU:  { unitSigNum+=32; break; }
        default: {}
    };

    return unitSigNum;
}



// Get masks representing all possible counters given the
// current affGrpMask (indicating which threaded counters reserved) and
// the given event.  Later, A2 signals to be used will be mapped from the chosen counter.
#define CTR_MASK_BIT(ctr) (0x80000000 >> ctr)
uint32_t Upci_Punit_Reserve_PossibleCtrMask(Upci_Punit_Rsv_t *pRsv, unsigned hwThd, const UPC_PunitEvtTableRec_t *pEvent)
{
    assert(hwThd < CONFIG_HWTHREADS_PER_CORE);

    uint32_t ctrMask = 0;
    uint8_t thdMask = pRsv->affGrpMask;

    // Get all possible counters & signals based on reserved affGrpMask
    int thd;
    for (thd=0; thd<CONFIG_HWTHREADS_PER_CORE; thd++) {
        if (thdMask & (0x80 >> thd)) {
            ctrMask |= (0x88888800 >> thd);
        }
    }
    PRINT_STEPS(printf(_AT_ " ctrMask(0x%08x), thdMask(0x%02x), hwThd(%d)\n", ctrMask, thdMask, hwThd));

    // remove threaded A2 signals/cntrs which can't be used for target thread
    if (pEvent->scope == UPC_SCOPE_THREAD) {
        switch (pEvent->evtSrc) {
             // two counters available per thread for A2 units
             case UPC_ES_A2_MMU:
             case UPC_ES_A2_AXU:
             case UPC_ES_A2_IU:
             case UPC_ES_A2_LSU:
             case UPC_ES_A2_XU:  {
                 ctrMask &= (0xCCCCCC00 >> ((hwThd/2) * 2));
                 break;
             }
             // one counter available per thread for L1p/Wakeup units
             case UPC_ES_L1P_BASE:
             case UPC_ES_L1P_SWITCH:
             case UPC_ES_L1P_STREAM:
             case UPC_ES_L1P_LIST:
             case UPC_ES_WAKEUP : {
                 ctrMask &= (0x88888800 >> hwThd);
                 break;
             }
             // all ctrs available to opcode unit.
             default: {}
        }
    }
    PRINT_STEPS(fprintf(stderr, _AT_ " ctrMask(0x%08x), evtId(%d), scope(%d), src(%d)\n",
                 ctrMask, pEvent->evtId, pEvent->scope, pEvent->evtSrc));
    // all ctrs available by default to core-shared or core-wide events.

    // Then mask off all other counters which cannot be used by specific event
    // Later, we will chose an available ctr from ctrMask, then map to an associated available signal.
    ctrMask &= (pEvent->ctrMask << 8);  // event table value is right adjusted, so left adjust here
    PRINT_STEPS(fprintf(stderr, _AT_ " ctrMask(0x%08x), evtId(%d), evtCtrMask(0x%08x)\n",
                 ctrMask, pEvent->evtId, (pEvent->ctrMask << 8)));
    return ctrMask;
}




uint8_t Upci_Punit_Reserve_PossibleA2UnitSignalMask(uint32_t ctrMask)
{
    uint8_t sigMask = 0;
    unsigned ctr;
    while (ctrMask) {
        ctr = upc_cntlz32(ctrMask);
        ctrMask &= ~(0x80000000 >> ctr);
        sigMask |= (0x80 >> Ctr2UnitSigNum(ctr));
    }
    return sigMask;
}


#define sigMaskStrgSize 12
static __thread char sigMaskStrg[sigMaskStrgSize];
const char *Upci_Punit_Reserve_SigMaskStrg(UPC_EventSources_t evtSrc, int a2Sig, uint8_t sigMask)
{
    #ifdef __FWEXT__
    sprintf(sigMaskStrg,
    #else
    snprintf(sigMaskStrg, sigMaskStrgSize,
    #endif
        "--------");
    if ((evtSrc >= UPC_ES_A2_MMU) && (evtSrc <= UPC_ES_A2_XU)) {
        unsigned bit;
        while (sigMask) {
            bit = upc_cntlz8(sigMask);
            sigMask &= ~(0x80 >> bit);
            if ((a2Sig < 0) || (bit != (a2Sig % 8))) {
                sigMaskStrg[bit] = '*';
            }
            else {
                sigMaskStrg[bit] = 'X';
            }
        }
    }
    return sigMaskStrg;
}

const char *Upci_Punit_Reserve_SigMaskStrgAll(UPC_EventSources_t evtSrc, uint32_t pcBusSigMask)
{
    #ifdef __FWEXT__
    sprintf(sigMaskStrg,
    #else
    snprintf(sigMaskStrg, sigMaskStrgSize,
    #endif
            "--------");
    if ((evtSrc >= UPC_ES_A2_MMU) && (evtSrc <= UPC_ES_A2_XU)) {
        int unitBitPos = GetA2PcSigNum(0, evtSrc);
        int i;
        for (i=unitBitPos; i<unitBitPos+8; ++i) {
            if (pcBusSigMask & (0x80000000 >> i)) {
                sigMaskStrg[i] = 'X';
            }
        }
    }
    return sigMaskStrg;
}


#define ctrMaskStrgSize 32
static __thread char ctrMaskStrg[ctrMaskStrgSize];
const char *Upci_Punit_Reserve_CtrMaskStrg(int ctr, uint32_t ctrMask)
{
    #ifdef __FWEXT__
    sprintf(ctrMaskStrg,
    #else
    snprintf(ctrMaskStrg, ctrMaskStrgSize,
    #endif
            "-------- -------- --------");
    unsigned bit;
    while (ctrMask) {
        bit = upc_cntlz32(ctrMask);
        ctrMask &= ~(0x80000000 >> bit);
        if (bit == (ctr)) {
            ctrMaskStrg[bit+(bit/8)] = 'X';
        }
        else if (ctr == -2) {  // use to mark all of them
            ctrMaskStrg[bit+(bit/8)] = 'X';
        }
        else {
            ctrMaskStrg[bit+(bit/8)] = '*';
        }
    }
    return ctrMaskStrg;
}



//! \brief: ReserveL1pMode
//! @param[in]      pRsv     current reserve object
//! @param[in]      L1pVal   value to check and assign
//! @return         UpciTrue if ok, UpciFalse if l1p conflict
static UpciBool_t ReserveL1pMode(Upci_Punit_Rsv_t *pRsv, UPC_EventSources_t L1pVal)
{
    assert(pRsv);
    UpciBool_t ret = UpciFalse;
    if (L1pVal == UPC_ES_L1P_BASE) {
        ret = UpciTrue;
    }
    else {
        if (pRsv->l1pMode == UPC_ES_Undef) {
            pRsv->l1pMode = L1pVal;
            ret = UpciTrue;
        }
        else if (pRsv->l1pMode == L1pVal) {
            ret = UpciTrue;
        }
    }
    return ret;
}



static int FindAvailableSignalNCounter(Upci_Punit_Rsv_t *pRsv, Upci_PunitEventRec_t *pEvtRec)
{
    // Get possible counters for this event.
    uint32_t ctrMask = Upci_Punit_Reserve_PossibleCtrMask(pRsv, pEvtRec->hwThread, pEvtRec->pEvent);

    uint32_t freeCtrMask = ~(pRsv->upcpRsvCtrMask);
    uint32_t tryCtrMask = ctrMask & freeCtrMask;

    PRINT_STEPS(fprintf(stderr, _AT_ " hwThd=%02d, ctrMask(0x%08x), freeCtrMask(0x%08x), tryCtrMask(0x%08x)\n",
            pEvtRec->hwThread, ctrMask, freeCtrMask, tryCtrMask));



    //unsigned evtId = pEvtRec->pEvent->evtId;

    // Search for an available counter
    int rc = UPCI_ECTRRESERVED;  // assume all used in case tryCtrMask indicates none to try.
    unsigned ctr;
    UpciBool_t found = UpciFalse;
    while (tryCtrMask && (!found)) {
        rc = UPCI_EOK;  // only return bad rc value at final loop.

        // Rather than trying counters sequentially, let's try them by thread
        // affinity groups to reduce likelyhood of collisions
        // when using fwext tests which don't know about other threads using the
        // same counters.
        int ctrFound = -1; // set to counter value when found.
        // Try current thread counters
        ctr = pEvtRec->hwThread;
        do {
            if (CTR_MASK_BIT(ctr) & tryCtrMask) {
                ctrFound = ctr;
                // if (pRsv->upcpCtrEvt[ctr].evtId == PEVT_UNDEF) ctrFound = ctr;
            }
            tryCtrMask &= ~CTR_MASK_BIT(ctr);
            ctr += 4;
        } while ((ctrFound < 0) && (ctr < UPC_P_NUM_COUNTERS));

        // try sister thread counters next (0-1 or 2-3)
        if (ctrFound < 0) {
            ctr = Upci_Rsv_SiblingThread(pEvtRec->hwThread);
            do {
                if (CTR_MASK_BIT(ctr) & tryCtrMask) {
                    ctrFound = ctr;
                    //if (pRsv->upcpCtrEvt[ctr].evtId == PEVT_UNDEF) ctrFound = ctr;
                }
                tryCtrMask &= ~CTR_MASK_BIT(ctr);
                ctr += 4;
            } while ((ctrFound < 0) && (ctr < UPC_P_NUM_COUNTERS));
        }

        // if still not found, try everything else left
        if (ctrFound < 0) {
            // while ((ctrFound < 0) && tryCtrMask) {
            ctr = upc_cntlz32(tryCtrMask);
            tryCtrMask &= ~CTR_MASK_BIT(ctr);
            if (ctr < UPC_P_NUM_COUNTERS) ctrFound = ctr;
            // if (pRsv->upcpCtrEvt[ctr].evtId == PEVT_UNDEF) ctrFound = ctr;
        }

        if (ctrFound < 0) {
            rc = UPCI_ECTRRESERVED;
        }
        else {
            ctr = ctrFound;
            uint64_t freeSigMask = ~(pRsv->a2RsvSigMask);
            // calculate corresponding a2 unit signal in case needed.
            unsigned a2UnitSig = Ctr2UnitSigNum(ctr);

            switch (pEvtRec->evtSrc) {
                case UPC_ES_A2_MMU:
                case UPC_ES_A2_AXU:
                case UPC_ES_A2_IU:
                case UPC_ES_A2_LSU:
                case UPC_ES_A2_XU:  {
                    unsigned a2Sig = GetA2PcSigNum(a2UnitSig, pEvtRec->evtSrc);

                    PRINT_STEPS(fprintf(stderr, _AT_ " ctr(%0d), a2UnitSig(%d), a2Sig(%02d), trySigMask(0x%016lx), freeSigMask(0x%016lx)\n",
                                  ctr, a2UnitSig, a2Sig, (uint64_t)MASK64_bit(a2Sig), freeSigMask));

                    if (freeSigMask & MASK64_bit(a2Sig)) {
                    // if (pRsv->a2SigEvent[ a2Sig ] == PEVT_UNDEF) {
                        found = UpciTrue;
                        pRsv->a2RsvSigMask |= MASK64_bit(a2Sig);
                        pRsv->upcpRsvCtrMask |= UPC_P__CONTROL32_bit(ctr);
                        //pRsv->a2SigEvent[ a2Sig ] = evtId;
                        //pRsv->upcpCtrEvt[ ctr ].evtId = evtId; // reserve counter
                        //pRsv->upcpCtrEvt[ ctr ].a2Sig = a2Sig; // related signal
                        pEvtRec->a2_Sig = a2Sig;
                    }
                    else {
                        rc = UPCI_ESIGRESERVED;
                    }
                    break;
                }

                // Validate l1p mode
                case UPC_ES_L1P_SWITCH: {
                    if (!ReserveL1pMode(pRsv, UPC_ES_L1P_SWITCH)) {
                        return UPCI_EL1PMODERESERVED;
                    }
                    //Upci_Punit_Core_Rsv_Dump(0, pCRsv);
                    found = UpciTrue;
                    break;
                }
                case UPC_ES_L1P_STREAM: {
                    if (!ReserveL1pMode(pRsv, UPC_ES_L1P_STREAM)) {
                        return UPCI_EL1PMODERESERVED;
                    }
                    //Upci_Punit_Core_Rsv_Dump(0, pCRsv);
                    found = UpciTrue;
                    break;
                }
                case UPC_ES_L1P_LIST: {
                    if (!ReserveL1pMode(pRsv, UPC_ES_L1P_LIST)) {
                        return UPCI_EL1PMODERESERVED;
                    }
                    //Upci_Punit_Core_Rsv_Dump(0, pCRsv);
                    found = UpciTrue;
                    break;
                }
                case UPC_ES_L1P_BASE:  // any l1p mode ok.
                case UPC_ES_WAKEUP:
                case UPC_ES_OP_XU:
                case UPC_ES_OP_AXU:
                case UPC_ES_OP_FP_OPS:
                case UPC_ES_OP_FP_INSTR:
                case UPC_ES_OP_ALL: {
                    found = UpciTrue;
                    break;
                }
                case UPC_ES_Undef: {
                    return UPCI_EINV_EVENT;
                    break;
                }
                default: {
                    return UPCI_ENOTIMPL;
                }
            }
        }
    }

    if (found) {
        pRsv->upcpRsvCtrMask |= CTR_MASK_BIT(ctr);
        //pRsv->upcpCtrEvt[ ctr ].evtId = evtId; // reserve counter
        pEvtRec->p_Ctr = ctr;
        pEvtRec->c_Cgrp = pEvtRec->p_Num;    // in mode 0, group is same as core.
        pEvtRec->c_Ctr = ctr;                // in mode 0, counter in same as upc_p counter

        PRINT_STEPS(uint8_t  sigMask = Upci_Punit_Reserve_PossibleA2UnitSignalMask(ctrMask));
        PRINT_STEPS(fprintf(stderr, _AT_ " core(%d) thd(%d) src(%d) sigMask(%s) rsvSigMask(0x%016lx) ctrMask(%s) rsvCtrMask(0x%08x)\n",
                      pEvtRec->p_Num, pEvtRec->hwThread, pEvtRec->evtSrc,
                      Upci_Punit_Reserve_SigMaskStrg(pEvtRec->evtSrc, pEvtRec->a2_Sig, sigMask), pRsv->a2RsvSigMask,
                      Upci_Punit_Reserve_CtrMaskStrg(pEvtRec->p_Ctr, ctrMask), pRsv->upcpRsvCtrMask));

        // Adjust the upc_c counter group if we have a spare core to compensate for
        const Upci_Node_Parms_t *pNodeParms = pRsv->mode.pNodeParms;
        if ( ((pNodeParms->dd1Enabled) || (pRsv->mode.ctrMode==(int)UPC_CM_INDEP))
                && (pEvtRec->c_Cgrp >= pNodeParms->spareCore)) {
            pEvtRec->c_Cgrp++;
        }
    }

    return rc;
}




int Upci_Punit_ReserveEvent(Upci_Punit_Rsv_t *pRsv, Upci_EventIds evtId,
                            Upci_PunitEventRec_t *pEvtRec,
                            unsigned unitId, unsigned hwThdId, uint8_t affGrpMask)
{
    pEvtRec->pEvent = &(punitEvtTbl[evtId]);

    assert(pEvtRec->pEvent->evtId == (unsigned)evtId);  // check that table entry matches

    pEvtRec->evtSrc   = pEvtRec->pEvent->evtSrc;
    pEvtRec->hwThread = hwThdId;
    pEvtRec->p_Num    = unitId;

    pRsv->affGrpMask = affGrpMask ? affGrpMask : 0xF0;

    PRINT_STEPS(fprintf(stderr, _AT_ " evtId=%03d, unitId=%02d, hwThdId=%02d, affGrpMask=0x%02x\n", evtId, unitId, hwThdId, affGrpMask));


    // Check that counters for all threads are available if this is a core-shared event
    // That is - only one thread can be counting if core-shared events are involved.
    int rc = 0;
    if ((pEvtRec->pEvent->scope == UPC_SCOPE_CORESHARED) && (pRsv->affGrpMask != 0xF0)) {
        rc = UPCI_ECSEVT_REQALLTHDS;
    }
    else {
        rc = FindAvailableSignalNCounter(pRsv, pEvtRec);
    }

    PRINT_STEPS(fprintf(stderr, _AT_ " rc=%d\n", rc));

    return rc;
}



void Upci_Punit_Reserve_SkipCtrs(Upci_Punit_Rsv_t *pRsv, uint64_t a2Signals, uint32_t upcPCtrs)
{
    assert(pRsv);

    //int i;
    pRsv->a2RsvSigMask |= a2Signals;
    pRsv->upcpRsvCtrMask |= upcPCtrs;

    //while (a2Signals) {
    //    i = upc_cntlz64(a2Signals);
    //    pRsv->a2SigEvent[i] = -1; // assign dummy value to make reserved.
    //    a2Signals &= ~(0x8000000000000000ULL >> i);
    //}
    //while (upcPCtrs) {
    //    i = upc_cntlz32(upcPCtrs);
    //    pRsv->upcpCtrEvt[i].evtId = -1; // assign dummy value to make reserved.
    //    upcPCtrs &= ~(0x80000000 >> i);
    //}
}



uint64_t Upci_Punit_Reserve_GetUsedSignalMask(Upci_Punit_Rsv_t *pRsv)
{
    assert(pRsv);

    //int i;
    //uint64_t a2Signals = 0;
    // Don't include prereserved signals (evtId = -1) in the return value
    //for (i=0; i<(UPC_P_NUM_A2_UNITS * UPC_P_NUM_A2_UNIT_SIGS); i++) {
    //    if (pRsv->a2SigEvent[i] > 0) {
    //        a2Signals |= (0x8000000000000000ULL >> i);
    //    }
    //}
    //return a2Signals;
    return pRsv->a2RsvSigMask;
}


uint32_t Upci_Punit_Reserve_GetUsedCtrMask(Upci_Punit_Rsv_t *pRsv)
{
    assert(pRsv);

    //int i;
    //uint32_t ctrMask = 0;
    //// Don't include prereserved counters (evtId = -1) in the return value
    //for (i=0; i<UPC_P_NUM_COUNTERS; i++) {
    //    if (pRsv->upcpCtrEvt[i].evtId > 0) {
    //        ctrMask |= (0x80000000 >> i);
    //    }
    //}
    //return ctrMask;
    return pRsv->upcpRsvCtrMask;
}
