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

#include <assert.h>
#include "spi/include/upci/punit_config.h"


#define PRINT_STEPS(_cmd_) // _cmd_
#ifdef __KERNEL__
#define PRINT(...) printf(__VA_ARGS__)
#else
#define PRINT(...) fprintf(stderr, __VA_ARGS__)

#endif


//! \brief: Config_A2_Signal
//! @param[in]  pA2pc       A2pc structure
//! @param[in]  a2sig       overall a2 signal number
//! @param[in]  pCtr        upc_p counter number which will be used
//! @param[in]  hwThread    hardware thread to be set
//! @param[in]  value       event value to set.
//! @param[in]  evt_src     event source
static void Config_A2_Signal(Upci_A2PC_t *pA2pc, ushort a2sig, ushort pCtr, ushort hwThread, uint32_t value, UPC_EventSources_t evtSrc)
{
    ushort unit_sig = a2sig % 8;   // get A2 unit signal value
    cesr_a2_unit_t cesrType = CESR_XU_EVENT;

    switch (evtSrc) {
        case UPC_ES_A2_MMU:  {
            Upci_A2PC_SetMmuEvent(pA2pc, unit_sig, hwThread, value);
            cesrType = CESR_MMU_EVENT;
            break;
        }
        case UPC_ES_A2_AXU:  {
            Upci_A2PC_SetAxuEvent(pA2pc, unit_sig, hwThread, value);
            cesrType = CESR_AXU_EVENT;
            break;
        }
        case UPC_ES_A2_IU: {
            Upci_A2PC_SetIuEvent(pA2pc, unit_sig, hwThread, value);
            cesrType = CESR_IU_EVENT;
            break;
        }
        case UPC_ES_A2_LSU:  {
            Upci_A2PC_SetLsuEvent(pA2pc, unit_sig, hwThread, value);
            cesrType = CESR_LSU_EVENT;
            break;
        }
        case UPC_ES_A2_XU:  {
            Upci_A2PC_SetXuEvent(pA2pc, unit_sig, hwThread, value);
            cesrType = CESR_XU_EVENT;
            break;
        }
        default: {
            assert(0);
        }
    }

    PRINT_STEPS(PRINT(_AT_ " Config_A2_Signal a2sig=%d, unit_sig=%d\n", a2sig, unit_sig));

    // set up a2 perf counting bus for upper 8 counters
    if (pCtr >= 16) {
        Upci_A2PC_SetCesrEvent(pA2pc, a2sig % 8, cesrType);
    }
}



//! \brief: Config_A2_Counter
//! @param[in]  pCtrRec       pointer to assigned counter record
//! @param[in]  start         UpciTrue to independent start
//! @returns    error code
static void Config_A2_Counter(uint64_t *pCtrCfg, UPC_PunitEvtTableRec_t *pEvent)
{
   // int rc = UPC_ERR_Undefined;
    uint64_t ctrcfg = 0;
    UPC_P__COUNTER_CFG__EVENT_GROUP_SEL_insert(ctrcfg, UPC_P_CEG_A2);

    switch (pEvent->evtSrc) {
        case UPC_ES_A2_IU: {
            UPC_P__COUNTER_CFG__EVENT_SUB_SEL_insert(ctrcfg, UPC_P_SUBG_IU);
            break;
        }
        case UPC_ES_A2_MMU: {
            UPC_P__COUNTER_CFG__EVENT_SUB_SEL_insert(ctrcfg, UPC_P_SUBG_MMU);
            break;
        }
        case UPC_ES_A2_LSU: {
            UPC_P__COUNTER_CFG__EVENT_SUB_SEL_insert(ctrcfg, UPC_P_SUBG_LSU);
            break;
        }
        case UPC_ES_A2_AXU: {
            UPC_P__COUNTER_CFG__EVENT_SUB_SEL_insert(ctrcfg, UPC_P_SUBG_AXU);
            break;
        }
        default: {}
    }

    *pCtrCfg = ctrcfg;
}




UpciBool_t Upci_Punit_Cfg_Edge_Masks(Upci_Punit_Cfg_t *pCfg, ushort sig, UPC_PunitEvtTableRec_t *pEvent, ushort edge)
{
    ushort unit_sig = sig % 8;   // get possible A2 unit signal value

    PRINT_STEPS(PRINT(_AT_ " evtSrc=%d, sig=%d, unit_sig=%d\n", pEvent->evtSrc, sig, unit_sig));
    UpciBool_t ret = UpciTrue;

    switch (pEvent->evtSrc) {
        case UPC_ES_A2_MMU:  {
            pCfg->a2EdgeMask |= UPC_P__CONFIG_EDGE__MMU_EDGE_bit(unit_sig);
            if (edge) pCfg->a2Edge |=  UPC_P__CONFIG_EDGE__MMU_EDGE_bit(unit_sig);
            else      pCfg->a2Edge &= ~UPC_P__CONFIG_EDGE__MMU_EDGE_bit(unit_sig);
            break;
        }
        case UPC_ES_A2_AXU:  {
            pCfg->a2EdgeMask |= UPC_P__CONFIG_EDGE__AXU_EDGE_bit(unit_sig);
            if (edge) pCfg->a2Edge |=  UPC_P__CONFIG_EDGE__AXU_EDGE_bit(unit_sig);
            else      pCfg->a2Edge &= ~UPC_P__CONFIG_EDGE__AXU_EDGE_bit(unit_sig);
            break;
        }
        case UPC_ES_A2_IU: {
            pCfg->a2EdgeMask |= UPC_P__CONFIG_EDGE__IU_EDGE_bit(unit_sig);
            if (edge) pCfg->a2Edge |=  UPC_P__CONFIG_EDGE__IU_EDGE_bit(unit_sig);
            else      pCfg->a2Edge &= ~UPC_P__CONFIG_EDGE__IU_EDGE_bit(unit_sig);
            break;
        }
        case UPC_ES_A2_LSU:  {
            pCfg->a2EdgeMask |= UPC_P__CONFIG_EDGE__LSU_EDGE_bit(unit_sig);
            if (edge) pCfg->a2Edge |=  UPC_P__CONFIG_EDGE__LSU_EDGE_bit(unit_sig);
            else      pCfg->a2Edge &= ~UPC_P__CONFIG_EDGE__LSU_EDGE_bit(unit_sig);
            break;
        }
        case UPC_ES_A2_XU:  {
            pCfg->a2EdgeMask |= UPC_P__CONFIG_EDGE__XU_EDGE_bit(unit_sig);
            if (edge) pCfg->a2Edge |=  UPC_P__CONFIG_EDGE__XU_EDGE_bit(unit_sig);
            else      pCfg->a2Edge &= ~UPC_P__CONFIG_EDGE__XU_EDGE_bit(unit_sig);
            break;
        }
        case UPC_ES_L1P_BASE: {
            unsigned evtId = pEvent->evtId;
            // Drain has bit per thread
            if ((evtId == PEVT_L1P_BAS_LU_DRAIN) || (evtId == PEVT_L1P_BAS_LU_DRAIN_CYC)) {
                pCfg->a2EdgeMask |= UPC_P__CONFIG_EDGE__L1P_DRAIN_bit(sig);
                if (edge) pCfg->a2Edge |= UPC_P__CONFIG_EDGE__L1P_DRAIN_bit(sig);
                else      pCfg->a2Edge &= ~UPC_P__CONFIG_EDGE__L1P_DRAIN_bit(sig);
            }
            // Rest of l1p events are single signal events.
            else if ((evtId == PEVT_L1P_BAS_LU_STALL_SRT) || (evtId == PEVT_L1P_BAS_LU_STALL_SRT_CYC)) {
                pCfg->a2EdgeMask |= UPC_P__CONFIG_EDGE__L1P_STALL_SRT_set(1UL);
                UPC_P__CONFIG_EDGE__L1P_STALL_SRT_insert(pCfg->a2Edge, edge);
            }
            else if ((evtId == PEVT_L1P_BAS_LU_STALL_MMIO_DCR) || (evtId == PEVT_L1P_BAS_LU_STALL_MMIO_DCR_CYC)) {
                pCfg->a2EdgeMask |= UPC_P__CONFIG_EDGE__L1P_MMIO_DCR_set(1UL);
                UPC_P__CONFIG_EDGE__L1P_MMIO_DCR_insert(pCfg->a2Edge, edge);
            }
            else if ((evtId == PEVT_L1P_BAS_LU_STALL_STRM_DET) || (evtId == PEVT_L1P_BAS_LU_STALL_STRM_DET_CYC)) {
                pCfg->a2EdgeMask |= UPC_P__CONFIG_EDGE__L1P_STREAM_DET_set(1UL);
                UPC_P__CONFIG_EDGE__L1P_STREAM_DET_insert(pCfg->a2Edge, edge);
            }
            else if ((evtId == PEVT_L1P_BAS_LU_STALL_LIST_RD) || (evtId == PEVT_L1P_BAS_LU_STALL_LIST_RD_CYC)) {
                pCfg->a2EdgeMask |= UPC_P__CONFIG_EDGE__L1P_LIST_READ_set(1UL);
                UPC_P__CONFIG_EDGE__L1P_LIST_READ_insert(pCfg->a2Edge, edge);
            }
            else if ((evtId == PEVT_L1P_BAS_LU_STALL_LIST_WRT) || (evtId == PEVT_L1P_BAS_LU_STALL_LIST_WRT_CYC)) {
                pCfg->a2EdgeMask |= UPC_P__CONFIG_EDGE__L1P_LIST_WRITE_set(1UL);
                UPC_P__CONFIG_EDGE__L1P_LIST_WRITE_insert(pCfg->a2Edge, edge);
            }
            else {
                ret = UpciFalse;
            }
            break;
        }
        default: {
            ret = UpciFalse;
        }

        PRINT_STEPS(PRINT(_AT_ " edgeReg = 0x%016lx, edgeMask = 0x%016lx\n", pCfg->a2Edge, pCfg->a2EdgeMask));

    }
    return ret;
}




ushort Upci_Punit_Cfg_GetEdgeValue(Upci_Punit_Cfg_t *pCfg, ushort sig, UPC_PunitEvtTableRec_t *pEvent)
{
    ushort unit_sig = sig % 8;   // get possible A2 unit signal value

    PRINT_STEPS(printf(_AT_ " evtSrc=%d, sig=%d, unit_sig=%d\n", pEvent->evtSrc, sig, unit_sig));
    uint64_t edge = 0;
    PRINT_STEPS(uint64_t edgeReg = pCfg->a2Edge);

    switch (pEvent->evtSrc) {
        case UPC_ES_A2_MMU:  edge = pCfg->a2Edge & UPC_P__CONFIG_EDGE__MMU_EDGE_bit(unit_sig); break;
        case UPC_ES_A2_AXU:  edge = pCfg->a2Edge & UPC_P__CONFIG_EDGE__AXU_EDGE_bit(unit_sig); break;
        case UPC_ES_A2_IU:   edge = pCfg->a2Edge & UPC_P__CONFIG_EDGE__IU_EDGE_bit(unit_sig);  break;
        case UPC_ES_A2_LSU:  edge = pCfg->a2Edge & UPC_P__CONFIG_EDGE__LSU_EDGE_bit(unit_sig); break;
        case UPC_ES_A2_XU:   edge = pCfg->a2Edge & UPC_P__CONFIG_EDGE__XU_EDGE_bit(unit_sig);  break;
        case UPC_ES_L1P_BASE: {
            unsigned evtId = pEvent->evtId;
            // Drain has bit per thread
            if ((evtId == PEVT_L1P_BAS_LU_DRAIN) || (evtId == PEVT_L1P_BAS_LU_DRAIN_CYC)) {
                edge = pCfg->a2Edge & UPC_P__CONFIG_EDGE__L1P_DRAIN_bit(sig);
            }
            // Rest of l1p events are single signal events.
            else if ((evtId == PEVT_L1P_BAS_LU_STALL_SRT) || (evtId == PEVT_L1P_BAS_LU_STALL_SRT_CYC)) {
                edge = pCfg->a2Edge & UPC_P__CONFIG_EDGE__L1P_STALL_SRT_set(1UL);
            }
            else if ((evtId == PEVT_L1P_BAS_LU_STALL_MMIO_DCR) || (evtId == PEVT_L1P_BAS_LU_STALL_MMIO_DCR_CYC)) {
                edge = pCfg->a2Edge & UPC_P__CONFIG_EDGE__L1P_MMIO_DCR_set(1UL);
            }
            else if ((evtId == PEVT_L1P_BAS_LU_STALL_STRM_DET) || (evtId == PEVT_L1P_BAS_LU_STALL_STRM_DET_CYC)) {
                edge = pCfg->a2Edge & UPC_P__CONFIG_EDGE__L1P_STREAM_DET_set(1UL);
            }
            else if ((evtId == PEVT_L1P_BAS_LU_STALL_LIST_RD) || (evtId == PEVT_L1P_BAS_LU_STALL_LIST_RD_CYC)) {
                edge = pCfg->a2Edge & UPC_P__CONFIG_EDGE__L1P_LIST_READ_set(1UL);
            }
            else if ((evtId == PEVT_L1P_BAS_LU_STALL_LIST_WRT) || (evtId == PEVT_L1P_BAS_LU_STALL_LIST_WRT_CYC)) {
                edge = pCfg->a2Edge & UPC_P__CONFIG_EDGE__L1P_LIST_WRITE_set(1UL);
            }
            break;
        }
        default: { }
    }
    PRINT_STEPS(PRINT(_AT_ " unit_sig=%4d, edge = 0x%016lx,  edgeReg = 0x%016lx\n", unit_sig, edge, edgeReg));
    PRINT_STEPS(PRINT(_AT_ " edge=%d\n", (ushort)(edge != 0)));

    return (ushort)(edge != 0);
}




UpciBool_t Upci_Punit_Cfg_Invert_Masks(Upci_Punit_Cfg_t *pCfg, ushort sig, UPC_PunitEvtTableRec_t *pEvent, ushort invert)
{
    uint64_t unit_sig = sig % 8;   // get possible A2 unit signal value

    PRINT_STEPS(PRINT(_AT_ " evtSrc=%d, sig=%d, unit_sig=%ld\n", pEvent->evtSrc, sig, unit_sig));
    UpciBool_t ret = UpciTrue;

    switch (pEvent->evtSrc) {
        case UPC_ES_A2_MMU:  {
            pCfg->a2EdgeMask |= UPC_P__CONFIG_EDGE__MMU_EDGE_bit(unit_sig);
            if (invert) pCfg->a2Invert |=  UPC_P__CONFIG_INVERT__MMU_INVERT_bit(unit_sig);
            else        pCfg->a2Invert &= ~UPC_P__CONFIG_INVERT__MMU_INVERT_bit(unit_sig);
            break;
        }
        case UPC_ES_A2_AXU:  {
            pCfg->a2EdgeMask |= UPC_P__CONFIG_EDGE__AXU_EDGE_bit(unit_sig);
            if (invert) pCfg->a2Invert |=  UPC_P__CONFIG_INVERT__AXU_INVERT_bit(unit_sig);
            else        pCfg->a2Invert &= ~UPC_P__CONFIG_INVERT__AXU_INVERT_bit(unit_sig);
            break;
        }
        case UPC_ES_A2_IU: {
            pCfg->a2EdgeMask |= UPC_P__CONFIG_EDGE__IU_EDGE_bit(unit_sig);
            if (invert) pCfg->a2Invert |=  UPC_P__CONFIG_INVERT__IU_INVERT_bit(unit_sig);
            else        pCfg->a2Invert &= ~UPC_P__CONFIG_INVERT__IU_INVERT_bit(unit_sig);
            break;
        }
        case UPC_ES_A2_LSU:  {
            pCfg->a2EdgeMask |= UPC_P__CONFIG_EDGE__LSU_EDGE_bit(unit_sig);
            if (invert) pCfg->a2Invert |=  UPC_P__CONFIG_INVERT__LSU_INVERT_bit(unit_sig);
            else        pCfg->a2Invert &= ~UPC_P__CONFIG_INVERT__LSU_INVERT_bit(unit_sig);
            break;
        }
        case UPC_ES_A2_XU:  {
            pCfg->a2EdgeMask |= UPC_P__CONFIG_EDGE__XU_EDGE_bit(unit_sig);
            if (invert) pCfg->a2Invert |=  UPC_P__CONFIG_INVERT__XU_INVERT_bit(unit_sig);
            else        pCfg->a2Invert &= ~UPC_P__CONFIG_INVERT__XU_INVERT_bit(unit_sig);
            break;
        }
        default: {
            ret = UpciFalse;
        }
    }
    return ret;
}




ushort Upci_Punit_Cfg_GetInvertValue(Upci_Punit_Cfg_t *pCfg, ushort sig, UPC_PunitEvtTableRec_t *pEvent)
{
    ushort unit_sig = sig % 8;   // get possible A2 unit signal value

    PRINT_STEPS(PRINT(_AT_ " evtSrc=%d, sig=%d, unit_sig=%d\n", pEvent->evtSrc, sig, unit_sig));
    uint64_t invert = 0;

    switch (pEvent->evtSrc) {
        case UPC_ES_A2_MMU: invert = pCfg->a2Invert & UPC_P__CONFIG_INVERT__MMU_INVERT_bit(unit_sig); break;
        case UPC_ES_A2_AXU: invert = pCfg->a2Invert & UPC_P__CONFIG_INVERT__AXU_INVERT_bit(unit_sig); break;
        case UPC_ES_A2_IU:  invert = pCfg->a2Invert & UPC_P__CONFIG_INVERT__IU_INVERT_bit(unit_sig);  break;
        case UPC_ES_A2_LSU: invert = pCfg->a2Invert & UPC_P__CONFIG_INVERT__LSU_INVERT_bit(unit_sig); break;
        case UPC_ES_A2_XU:  invert = pCfg->a2Invert & UPC_P__CONFIG_INVERT__XU_INVERT_bit(unit_sig);  break;
        default: { }
    }
    PRINT_STEPS(PRINT(_AT_ " invert=0x%016lx, return=%d\n", invert, (invert != 0)));
    return (invert != 0);
}




//! \brief: Config_L1pWake_Counter
//! @param[in]  pCtrCfg       ptr to Counter Config work
//! @param[in]  selVal        l1p/Wake event upc_p selection value.
static void Config_L1pWake_Counter(uint64_t *pCtrCfg, uint32_t selVal)
{
    if (selVal <= 3) {
        UPC_P__COUNTER_CFG__EVENT_GROUP_SEL_insert(*pCtrCfg, UPC_P_CEG_L1PWU1);
    }
    else {
        UPC_P__COUNTER_CFG__EVENT_GROUP_SEL_insert(*pCtrCfg, UPC_P_CEG_L1PWU2);
    }
    UPC_P__COUNTER_CFG__EVENT_SUB_SEL_insert(*pCtrCfg, selVal & 0x3);
}




//! \brief: Config_OpCode_Counter
//! @param[in]  pCtrCfg       ptr to Counter Config work
//! @param[in]  hwThd         target hardware thread.
//! @param[in]  pEvent        src event
static void Config_OpCode_Counter(uint64_t *pCtrCfg, ushort hwThd, UPC_PunitEvtTableRec_t *pEvent)
{
    uint64_t ctrcfg = UPC_P__COUNTER_CFG__OP_THREAD_MASK_bit(hwThd);

    unsigned selVal = pEvent->selVal;      // select opcode group
    if (pEvent->evtSrc == UPC_ES_OP_XU) {
        UPC_P__COUNTER_CFG__XU_OPGROUP_SEL_insert(ctrcfg, selVal);
        UPC_P__COUNTER_CFG__OPS_NOT_FLOPS_insert(ctrcfg, 1UL); // just in case.
    }
    else if (pEvent->evtSrc == UPC_ES_OP_AXU) {
        UPC_P__COUNTER_CFG__AXU_OPGROUP_SEL_insert(ctrcfg, selVal);
        UPC_P__COUNTER_CFG__OPS_NOT_FLOPS_insert(ctrcfg, 1UL);
    }
    else if (pEvent->evtSrc == UPC_ES_OP_FP_OPS) {
        UPC_P__COUNTER_CFG__AXU_OPGROUP_SEL_insert(ctrcfg, selVal);
        UPC_P__COUNTER_CFG__OPS_NOT_FLOPS_insert(ctrcfg, 0UL);  // count flops
    }
    else if (pEvent->evtSrc == UPC_ES_OP_FP_INSTR) {
        UPC_P__COUNTER_CFG__AXU_OPGROUP_SEL_insert(ctrcfg, selVal);
        UPC_P__COUNTER_CFG__OPS_NOT_FLOPS_insert(ctrcfg, 1UL);  // count instructions
    }
    else if (pEvent->evtSrc == UPC_ES_OP_ALL) {
        UPC_P__COUNTER_CFG__XU_OPGROUP_SEL_insert(ctrcfg, UPC_P_XU_OGRP_ALL);
        UPC_P__COUNTER_CFG__AXU_OPGROUP_SEL_insert(ctrcfg, UPC_P_AXU_OGRP_ALL);
        UPC_P__COUNTER_CFG__OPS_NOT_FLOPS_insert(ctrcfg, 1UL);
    }
    else {
        assert(0);  // Invalid src
    }

    UPC_P__COUNTER_CFG__EVENT_GROUP_SEL_insert(ctrcfg, UPC_P_CEG_OPCODES);

    *pCtrCfg = ctrcfg;
}




void Upci_Punit_Cfg_AddEvent(Upci_Punit_Cfg_t *pCfg, Upci_PunitEventRec_t *pEvtRec, int bkRefIdx)
{
    assert(pCfg);
    assert(pEvtRec);

    ushort p_Ctr = pEvtRec->p_Ctr;
    pCfg->ctrBkRefIdx[p_Ctr] = (unsigned)bkRefIdx;
    pCfg->ctrOvfThres[p_Ctr] = 0;     // make sure inited.
    pCfg->ctrOvfAccum[p_Ctr] = 0;        // make sure inited.
    pCfg->ctrSS[p_Ctr] = 0;
    pCfg->cCtrGrp = pEvtRec->c_Cgrp;  // value will be the same for all assigned counters, so let's just keep the last one.

    PRINT_STEPS(PRINT(_AT_ " Upci_Punit_Cfg_AddEvent evt=%d, pCtr=%d, bkRefIdx=%d\n", pEvtRec->pEvent->evtId, p_Ctr, bkRefIdx));

    pCfg->thdCtrMask |= UPC_P__CONTROL32_bit(p_Ctr);

    //pCfg->ctrAccum[p_Ctr] = 0;
    //pCfg->c_Ctr[p_Ctr] = pEvtRec->c_Ctr;

    //! Add cesr mode (context) is core-wide setting which is configured independently prior to apply.

    UPC_PunitEvtTableRec_t *pEvent = pEvtRec->pEvent;

    // Based on the event - set counter and signal config
    switch (pEvtRec->evtSrc) {
        case UPC_ES_A2_MMU:
        case UPC_ES_A2_AXU:
        case UPC_ES_A2_IU:
        case UPC_ES_A2_LSU:
        case UPC_ES_A2_XU: {
            Config_A2_Signal(&(pCfg->a2sprs), pEvtRec->a2_Sig, p_Ctr, pEvtRec->hwThread, pEvent->selVal, pEvtRec->evtSrc);
            Config_A2_Counter(&(pCfg->ctrCfg[p_Ctr]), pEvent);
            ushort edge = pEvent->cycle ? 0 : 1;
            Upci_Punit_Cfg_Edge_Masks(pCfg, pEvtRec->a2_Sig, pEvent, edge);
            ushort invert = pEvent->invert;
            Upci_Punit_Cfg_Invert_Masks(pCfg, pEvtRec->a2_Sig, pEvent, invert);
            break;
        }

        case UPC_ES_L1P_SWITCH: {
            Config_L1pWake_Counter(&(pCfg->ctrCfg[p_Ctr]), pEvent->selVal);
            pCfg->l1pHwMode = L1P_CFG_UPC_ENABLE | L1P_CFG_UPC_SWITCH;
            break;
        }
        case UPC_ES_L1P_STREAM: {
            Config_L1pWake_Counter(&(pCfg->ctrCfg[p_Ctr]), pEvent->selVal);
            pCfg->l1pHwMode = L1P_CFG_UPC_ENABLE | L1P_CFG_UPC_STREAM;
            break;
        }
        case UPC_ES_L1P_LIST: {
            Config_L1pWake_Counter(&(pCfg->ctrCfg[p_Ctr]), pEvent->selVal);
            pCfg->l1pHwMode = L1P_CFG_UPC_ENABLE | L1P_CFG_UPC_LIST;
            break;
        }
        case UPC_ES_L1P_BASE: {
            Config_L1pWake_Counter(&(pCfg->ctrCfg[p_Ctr]), pEvent->selVal);
            ushort edge = pEvent->cycle ? 0 : 1;
            Upci_Punit_Cfg_Edge_Masks(pCfg, pEvtRec->a2_Sig, pEvent, edge);
            break;
        }
        case UPC_ES_WAKEUP: {
            Config_L1pWake_Counter(&(pCfg->ctrCfg[p_Ctr]), pEvent->selVal);
            break;
        }

        //! arbitrary opcode masking for events is set independently in structure.
        //! default sqrt/div values are set independently in structure
        case UPC_ES_OP_XU:
        case UPC_ES_OP_AXU:
        case UPC_ES_OP_FP_OPS:
        case UPC_ES_OP_FP_INSTR:
        case UPC_ES_OP_ALL: {
            Config_OpCode_Counter(&(pCfg->ctrCfg[p_Ctr]), pEvtRec->hwThread, pEvent);
            break;
        }
        case UPC_ES_Undef: {
            assert(0);  // UPCI_EINV_EVENT;
            break;
        }
        default: {
            assert(0); // UPCI_ENOTIMPL;
            break;
        }
    }

    uint64_t overrideSync = pCfg->syncStart ? 0 : 1;
    PRINT_STEPS(PRINT(_AT_ " overrideSync=%ld\n", overrideSync));
    UPC_P__COUNTER_CFG__SYNC_OVERRIDE_insert(pCfg->ctrCfg[p_Ctr], overrideSync);

    pCfg->intEnableMask |= UPC_P__CONFIG__BIG_OVF_INT_ENABLE_bit(pEvtRec->hwThread)
                           | UPC_P__CONFIG__UPC_P_INT_ENABLE_bit(pEvtRec->hwThread);

    // If allocated counters come from mixed thread affinity groups, then we need to enable
    // shared interrupts. We can tell if there are other counters active when we mask off
    // counters from the current threads affinity group.
    uint32_t allCtrMask = pCfg->thdCtrMask | pCfg->ovfCtrMask;
    if (allCtrMask & (~0x88888800 >> pEvtRec->hwThread)) {
        pCfg->intEnableMask |= UPC_P__CONFIG__SHARED_MODE_set(0x1ULL)
                               | UPC_P__CONFIG__INTERRUPT_ON_ANY_bit(pEvtRec->hwThread);
    }

}



void Upci_Punit_Cfg_StopN_Snapshot_Counts(Upci_Punit_Cfg_t *pCfg)
{
    upc_p_mmio_t *const upc_p = UPC_P_Addr(UPC_LOCAL_UNIT);
    uint32_t ctrMask = pCfg->thdCtrMask | pCfg->ovfCtrMask;
    upc_p->control_w1c = ctrMask;  // stop counting
    mbar();
    Upci_Delay(800);
    while (ctrMask != 0) {
        uint32_t ctr = upc_cntlz32(ctrMask);
        ctrMask &= ~MASK32_bit(ctr);    // mask off counter bit.
        pCfg->ctrSS[ctr] = upc_c->data24.grp[pCfg->cCtrGrp].counter[ctr];
    }
}

