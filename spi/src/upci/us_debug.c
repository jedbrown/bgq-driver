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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <assert.h>
#include "spi/include/upci/punit.h"
#include "spi/include/upci/punit_config.h"
#include "spi/include/upci/upc_nw.h"
#include "spi/include/upci/node_parms.h"
#include "spi/include/upci/upc_io.h"
#include "spi/include/kernel/upci.h"
#include "us_debug.h"

extern const UPC_IOunitEvtTableRec_t IOunitEvtTbl[];



//! \file
//! User state debug routines
//


#ifdef UPCI_INCL_DEBUG_DATA
Upci_Debug_Counts_t upciDebugCounts = UPCI_DEBUG_COUNTS_INIT;

void Upci_DumpDebugCounts(unsigned indent)
{
    INIT_INDENT();
    printf("%ssyscallRetries(%ld) syscallInits(%ld) "
            "syscallFrees(%ld) syscallApplies(%ld) "
            "syscallXCoreApplies(%ld)\n",
            indentStrg,
            upciDebugCounts.syscallRetries,
            upciDebugCounts.syscallInits,
            upciDebugCounts.syscallFrees,
            upciDebugCounts.syscallApplies,
            upciDebugCounts.syscallXCoreApplies );
}
#endif



void Upci_DumpNodeParms(unsigned indent)
{
    INIT_INDENT();
    const Upci_Node_Parms_t *np = Upci_GetNodeParms();
    printf("%scpuSpeed=%16ld, nwSpeed=%16ld, cycleRatio=%lf, spareCore=%2d, dd1Enabled=%d, nodeConfig=0x%016lx\n",
            indentStrg, np->cpuSpeed, np->nwSpeed, np->cycleRatio,
            np->spareCore, np->dd1Enabled, np->nodeConfig);
}



void Upci_DumpKDebug(unsigned indent)
{
    INIT_INDENT();
    Upci_KDebug_t kdata;
    Kernel_Upci_GetKDebug(&kdata);
    printf("%sxucr1(0x%016lx) iullcr(0x%016lx) jobid(0x%016lx) "
            "coreInitMask(0x%08x)\n",
            indentStrg, kdata.xucr1, kdata.iullcr, kdata.jobid,
            kdata.coreInitMask);
}


void Upci_Mode_Dump(unsigned indent, Upci_Mode_t * o)
{
    assert(o);
    INIT_INDENT();
    printf("%sMode: upcMode(%d), ctrMode(%d)\n",
            indentStrg, o->upcMode, o->ctrMode);
}



void Upci_UnitStatus_Dump(unsigned indent, Upci_UnitStatus_t * o)
{
    assert(o);
    INIT_INDENT();
    printf("%sStatus: running(%d), applied(%d)\n",
            indentStrg,
            *o & UPCI_UnitStatus_Running,
            *o & UPCI_UnitStatus_Applied
            );
}


#if 0
void Upci_RsvCtr_Dump(unsigned indent, unsigned idx, Upci_RsvCtr_t * o)
{
    assert(o);
    INIT_INDENT();
    printf("%sRsvCtr[%02d]: evtId(%03d), a2Sig(%02d)\n",
            indentStrg, idx, o->evtId, o->a2Sig);
}
#endif



void Upci_Punit_EventH_Dump(unsigned indent, unsigned idx, Upci_Punit_EventH_t * o)
{
    assert(o);
    INIT_INDENT();
    printf("%sEvtH[%02d]: val(%d), pidx(%02d), grp(%02d), ctr(%02d)\n",
            indentStrg, idx, o->valid, o->evtIdx, o->ctrGrp, o->ctr);
}



void Upci_Punit_Rsv_Dump(unsigned indent, Upci_Punit_Rsv_t * o)
{
    assert(o);
    INIT_INDENT();
    printf("%sPunit_Rsv: \n", indentStrg);
    Upci_Mode_Dump(indent+1, &(o->mode));
    printf("%s  l1pMode(%d), affGrpMask(0x%02x), upcpRsvCtrMask(0x%08x), a2RsvSigMask(0x%016lx)\n",
            indentStrg, o->l1pMode, o->affGrpMask, o->upcpRsvCtrMask, o->a2RsvSigMask);

#if 0
    short i;
    printf("%s  upcpCtrEvt[]={\n", indentStrg);
    for (i=0; i<UPC_P_NUM_COUNTERS; i++) {
        Upci_RsvCtr_Dump(indent+2, i, &(o->upcpCtrEvt[i]));
    }
    printf("%s  }\n", indentStrg);

    printf("%s  a2SigEvent[]={", indentStrg);
    for (i=0; i<UPC_P_NUM_A2_UNITS * UPC_P_NUM_A2_UNIT_SIGS; i++) {
        printf("%d,", o->a2SigEvent[i]);
    }
    printf("}\n");
#endif
}



void Upci_Punit_Dump(unsigned indent, Upci_Punit_t * o)
{
    assert(o);
    INIT_INDENT();
    printf("%sPunit (@=0x%016lx): \n", indentStrg, (uint64_t)o);
    Upci_Mode_Dump(indent+1, &(o->mode));
    Upci_UnitStatus_Dump(indent+1, &(o->status));
    Upci_Punit_Rsv_Dump(indent+1, &(o->rsv));
    printf("%s  unit=%d\n", indentStrg, o->unit);
    printf("%s  numEvts=%d\n", indentStrg, o->numEvts);
    printf("%s  maxEvtIdx=%d\n", indentStrg, o->maxEvtIdx);

    short i;
    for (i=0; i < o->numEvts; i++) {
        Upci_PunitEventRec_Dump(indent+1, &(o->evts[i]));
    }

    Upci_Punit_Cfg_Dump(indent+1, &(o->cfg));
}




void Upci_PunitEventRec_Dump(unsigned indent, Upci_PunitEventRec_t * o)
{
    assert(o);
    INIT_INDENT();
    printf("%sPunitEventRec: pEvent(%03d) evtSrc(%02d) hwThd(%d)"
           " p_Num(%02d) p_Ctr(%02d) a2_Sig(%02d) c_Cgrp(%02d) c_Ctr(%02d)\n",
            indentStrg, o->pEvent->evtId, o->evtSrc, o->hwThread,
            o->p_Num,  o->p_Ctr, o->a2_Sig, o->c_Cgrp, o->c_Ctr);
}





void Upci_A2PC_Dump(unsigned indent, Upci_A2PC_t *o)
{
    assert(o);
    INIT_INDENT();
    printf("%scesr: 0x%08x(0x%08x)\n", indentStrg, o->cesr, o->cesrMask );
    printf("%saesr: 0x%08x(0x%08x)\n", indentStrg, o->aesr, o->aesrMask );
    printf("%siesr: 0x%08x(0x%08x)  0x%08x(0x%08x)\n", indentStrg, o->iesr1, o->iesr1Mask, o->iesr2, o->iesr2Mask );
    printf("%sxesr: 0x%08x(0x%08x)  0x%08x(0x%08x)\n", indentStrg, o->xesr1, o->xesr1Mask, o->xesr2, o->xesr2Mask );
    printf("%smesr: 0x%08x(0x%08x)  0x%08x(0x%08x)\n", indentStrg, o->mesr1, o->mesr1Mask, o->mesr2, o->mesr2Mask );
    printf("%slusr: 0x%08x(0x%08x)  0x%08x(0x%08x)\n", indentStrg, o->xesr3, o->xesr3Mask, o->xesr4, o->xesr4Mask );
}



void Upci_Punit_Cfg_Dump(unsigned indent, Upci_Punit_Cfg_t * o)
{
    assert(o);
    INIT_INDENT();

    printf("%sPunit_Cfg:\n", indentStrg);
    printf("%s  cfgVersion(0x%08x), cCtrGrp(%d)\n", indentStrg, o->cfgVersion, o->cCtrGrp);

    Upci_Mode_Dump(indent+1, &(o->mode));

    printf("%s  syncStart(%d)  enableInts(%d)  ovfCtrMask(0x%08x)  thdCtrMask(0x%08x)\n",
            indentStrg, o->syncStart, o->enableInts, o->ovfCtrMask, o->thdCtrMask);

    printf("%s  l1pHwMode(%d)  intStatusSav(0x%016lx)  intMaskSav(0x%016lx)  intEnableMask(0x%016lx)\n",
            indentStrg, o->l1pHwMode, o->intStatusSav, o->intMaskSav, o->intEnableMask);

    printf("%s  opCodeMatch: 0x%016lx,  fpWeights: 0x%016lx\n",
            indentStrg, o->opCodeMatch, o->fpWeights);


    short i;
    for (i=0; i<UPC_P_NUM_COUNTERS; i++) {
        printf("%s  ctr[%02d] pcfg=0x%016lx, ctrBkRefIdx=0x%08x, ctrOvfThres=0x%016lx, ctrOvfAccum=0x%016lx, ctrSS=0x%016lx\n",
                indentStrg, i, o->ctrCfg[i], o->ctrBkRefIdx[i], o->ctrOvfThres[i], o->ctrOvfAccum[i], o->ctrSS[i]);
    }

    printf("%s   invert,edge,edgeMask = 0x%016lx, 0x%016lx, 0x%016lx\n",
            indentStrg, o->a2Invert, o->a2Edge, o->a2EdgeMask);

    printf("%s   a2sprs:\n", indentStrg);

    Upci_A2PC_Dump(indent+1, &(o->a2sprs));
}




void UPC_NW_Dump()
{
    uint64_t ctl = DCRReadUser(ND_500_DCR(CTRL_UPC_STOP));
    uint16_t enLinks = UPC_NW_GetEnabledLinks();
    printf("UPC NW Dump:  ctlMask=0x%016lx, ~ctlMask=0x%016lx, enabledLinks=0x%04x\n", ctl, ~ctl, enLinks);
    int linkidx,ctridx;
    for (linkidx=0; linkidx<UPC_NW_NUM_LINKS; linkidx++) {
        char markEnabled = '-';
        if (enLinks & UPC_NW_LINK_bit(linkidx)) markEnabled = '+';
        for (ctridx=0; ctridx<UPC_NW_NUM_CTRS; ctridx++) {
            uint8_t vChan = UPC_NW_GetCtrVChannel(linkidx, ctridx);
            uint64_t ctrVal = UPC_NW_ReadCtr(linkidx, ctridx);
            printf("   %clink=%02d, ctr=%d, vChan=0x%02x, ctrVal=0x%016lx\n",
                    markEnabled, linkidx, ctridx, vChan, ctrVal);
        }
    }
}



void UPC_NW_GetCtrArray(UPC_NW_CtrArray_t *pCtrs)
{
    assert(pCtrs);

    const Upci_Node_Parms_t * pNodeParms = Upci_GetNodeParms();
    pCtrs->nwSpeed = pNodeParms->nwSpeed;
    int linkidx,ctridx;
    for (linkidx=0; linkidx<UPC_NW_NUM_LINKS; linkidx++) {
        for (ctridx=0; ctridx<UPC_NW_NUM_CTRS; ctridx++) {
            pCtrs->ctr[linkidx][ctridx] = UPC_NW_ReadCtr(linkidx, ctridx);
        }
    }
}




void UPC_NW_DumpCtrArray(unsigned indent, UPC_NW_CtrArray_t *o)
{
    assert(o);
    INIT_INDENT();
    printf("%sNW Counters: \n", indentStrg);
    printf("%s  nwSpeed         = %d  Mhz\n", indentStrg, o->nwSpeed);
    int linkidx;
    for (linkidx=0; linkidx<UPC_NW_NUM_LINKS; linkidx++) {
        printf("%s  link[%02d]: 0x%016lx   0x%016lx   0x%016lx   0x%016lx   0x%016lx   0x%016lx\n",
                indentStrg, linkidx,
                o->ctr[linkidx][0], o->ctr[linkidx][1], o->ctr[linkidx][2],
                o->ctr[linkidx][3], o->ctr[linkidx][4], o->ctr[linkidx][5]);
    }
}



void UPC_IO_PrintSubList(char *indentStrg, UPC_IO_SnapShot_t *o, unsigned firstEvt, unsigned lastEvt)
{
    unsigned i;
    unsigned firstIdx = firstEvt - PEVT_MU_PKT_INJ;
    unsigned lastIdx = lastEvt - PEVT_MU_PKT_INJ;
    unsigned delta = lastIdx - firstIdx + 1;
    if (delta%2) lastIdx--;
    for (i=firstIdx; i<=lastIdx; i+=2) {
        printf("%s0x%016lx <= %-40s   0x%016lx <= %s\n", indentStrg,
                o->ctr[IOunitEvtTbl[i].selVal], IOunitEvtTbl[i].label,
                o->ctr[IOunitEvtTbl[i+1].selVal], IOunitEvtTbl[i+1].label);
    }
    if (delta%2) {
        printf("%s0x%016lx <= %s\n", indentStrg,
                o->ctr[IOunitEvtTbl[lastIdx+1].selVal], IOunitEvtTbl[lastIdx+1].label);
    }
}



void  UPC_IO_DumpSnapShot(unsigned indent, UPC_IO_SnapShot_t *o)
{
    assert(o);
    INIT_INDENT();
    printf("%sI/O Counters: \n", indentStrg);
    UPC_IO_PrintSubList(indentStrg, o, PEVT_MU_PKT_INJ, PEVT_MU_PORT2_PEND_RD);
    UPC_IO_PrintSubList(indentStrg, o, PEVT_PCIE_INB_RD_BYTES, PEVT_PCIE_TX_TLP_SIZE);
    UPC_IO_PrintSubList(indentStrg, o, PEVT_DB_PCIE_INB_WRT_BYTES, PEVT_DB_MSG_SIG_INTS);
}



uint64_t UPC_IO_ReadEvtCtr(unsigned evtNum)
{
    assert((evtNum >= PEVT_MU_PKT_INJ) && (evtNum <= PEVT_DB_MSG_SIG_INTS));
    unsigned idx = evtNum-PEVT_MU_PKT_INJ;
    return upc_c->iosram_data[IOunitEvtTbl[idx].selVal];
}



void  UPC_IO_DumpState(unsigned indent, uint64_t nodeConfig)
{
    INIT_INDENT();

    int i;
    printf("%sMU UPC Config: \n", indentStrg);
    printf("%s  dcr reset: 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(RESET)));
    for (i=0; i<MU_DCR__UPC_MU_COUNTER_range; i++) {
        printf("%s  mu ctr[%02d]: 0x%016lx\n", indentStrg, i, DCRReadUser(MU_DCR(UPC_MU_COUNTER)+i));
    }
    printf("%s  mu ctr ctl: 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(UPC_MU_COUNTER_CONTROL_RW)));
    printf("%s  mu upc cfg: 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(UPC_MU_CONFIG)));
    printf("%s  mu int internal error sw info  : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(INTERRUPT_INTERNAL_ERROR_SW_INFO)));
    printf("%s  mu int internal error hw info  : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(INTERRUPT_INTERNAL_ERROR_HW_INFO)));
    printf("%s  mu int internal error data info: 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(INTERRUPT_INTERNAL_ERROR_DATA_INFO)));
    printf("%s  mu int internal error state    : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(INTERRUPT_INTERNAL_ERROR__STATE)));
    printf("%s  mu int internal error first    : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(INTERRUPT_INTERNAL_ERROR__FIRST)));
    printf("%s  mu int internal error machchk  : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK)));
    printf("%s  mu int internal error critical : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(INTERRUPT_INTERNAL_ERROR__CRITICAL)));
    printf("%s  mu int internal error noncrit  : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(INTERRUPT_INTERNAL_ERROR__NONCRITICAL)));
    printf("%s  mu misc int state    : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(MISC_INTERRUPTS__STATE)));
    printf("%s  mu misc int first    : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(MISC_INTERRUPTS__FIRST)));
    printf("%s  mu misc int machchk  : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(MISC_INTERRUPTS__MACHINE_CHECK)));
    printf("%s  mu misc int critical : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(MISC_INTERRUPTS__CRITICAL)));
    printf("%s  mu misc int noncrit  : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(MISC_INTERRUPTS__NONCRITICAL)));
    printf("%s  mu misc int ctl high : 0x%016lx\n", indentStrg, DCRReadUser(MU_DCR(MISC_INTERRUPTS_CONTROL_HIGH)));


    if (nodeConfig & PERS_ENABLE_PCIe) {
        printf("%sPCIe UPC Config: \n", indentStrg);
        for (i=0; i<PE_DCR__UPC_PCIE_COUNTER_range; i++) {
            printf("%s  pe ctr[%02d]: 0x%016lx\n", indentStrg, i, DCRReadUser(PE_DCR(UPC_PCIE_COUNTER)+i));
        }
        printf("%s  pe ctr ctl: 0x%016lx\n", indentStrg, DCRReadUser(PE_DCR(UPC_PCIE_COUNTER_CONTROL_RW)));
        printf("%s  pe upc cfg: 0x%016lx\n", indentStrg, DCRReadUser(PE_DCR(UPC_PCIE_CONFIG)));
        printf("%s  pe int state    : 0x%016lx\n", indentStrg, DCRReadUser(PE_DCR(PCIE_INTERRUPT_STATE__STATE)));
        printf("%s  pe int first    : 0x%016lx\n", indentStrg, DCRReadUser(PE_DCR(PCIE_INTERRUPT_STATE__FIRST)));
        printf("%s  pe int machchk  : 0x%016lx\n", indentStrg, DCRReadUser(PE_DCR(PCIE_INTERRUPT_STATE__MACHINE_CHECK)));
        printf("%s  pe int critical : 0x%016lx\n", indentStrg, DCRReadUser(PE_DCR(PCIE_INTERRUPT_STATE__CRITICAL)));
        printf("%s  pe int noncrit  : 0x%016lx\n", indentStrg, DCRReadUser(PE_DCR(PCIE_INTERRUPT_STATE__NONCRITICAL)));
        printf("%s  pe int state ctl low : 0x%016lx\n", indentStrg, DCRReadUser(PE_DCR(PCIE_INTERRUPT_STATE_CONTROL_LOW)));

    }

    printf("%sDevBus UPC Config: \n", indentStrg);
    for (i=0; i<DEVBUS_DCR__UPC_DB_COUNTER_range; i++) {
        printf("%s  db ctr[%02d]: 0x%016lx\n", indentStrg, i, DCRReadUser(DEVBUS_DCR(UPC_DB_COUNTER)+i));
    }
    printf("%s  db ctr ctl: 0x%016lx\n", indentStrg, DCRReadUser(DEVBUS_DCR(UPC_DB_COUNTER_CONTROL_RW)));
    printf("%s  db upc cfg: 0x%016lx\n", indentStrg, DCRReadUser(DEVBUS_DCR(UPC_DB_CONFIG)));
    printf("%s  db int state    : 0x%016lx\n", indentStrg, DCRReadUser(DEVBUS_DCR(DB_INTERRUPT_STATE__STATE)));
    printf("%s  db int first    : 0x%016lx\n", indentStrg, DCRReadUser(DEVBUS_DCR(DB_INTERRUPT_STATE__FIRST)));
    printf("%s  db int machchk  : 0x%016lx\n", indentStrg, DCRReadUser(DEVBUS_DCR(DB_INTERRUPT_STATE__MACHINE_CHECK)));
    printf("%s  db int critical : 0x%016lx\n", indentStrg, DCRReadUser(DEVBUS_DCR(DB_INTERRUPT_STATE__CRITICAL)));
    printf("%s  db int noncrit  : 0x%016lx\n", indentStrg, DCRReadUser(DEVBUS_DCR(DB_INTERRUPT_STATE__NONCRITICAL)));
    printf("%s  db int state ctl low : 0x%016lx\n", indentStrg, DCRReadUser(DEVBUS_DCR(DB_INTERRUPT_STATE_CONTROL_LOW)));

}






