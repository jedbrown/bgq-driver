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

#include <errno.h>
#include "spi/include/upci/upc_p.h"
#include "spi/include/upci/punit_config.h"
#include "spi/include/upci/upc_a2_pc.h"
#include "spi/include/upci/upc_l1p.h"




void Upci_Punit_Cfg_Apply(Upci_Punit_Cfg_t *pCfg)
{
    assert(pCfg);

    upc_p_mmio_t *const upc_p = UPC_P_Addr(UPC_LOCAL_UNIT);

    //printf("apply upc_p_addr=0x%016lx\n", (uint64_t)upc_p);

    // make sure upc_p counters are stopped
    uint64_t ctlMask = (pCfg->thdCtrMask | pCfg->ovfCtrMask);
    upc_p->control_w1c = ctlMask;

    Upci_Delay(800);  // let upc_c counter updates stabilize with any inflight upc_p values.

    if (pCfg->opCodeMatch) {
        upc_p->opcode_cfg = pCfg->opCodeMatch;
    }

    // set counter configs and restore upc_c counts
    uint32_t cunit = pCfg->cCtrGrp; // Kernel_ProcessorCoreID();
    uint32_t ovfMask = pCfg->ovfCtrMask;
    uint32_t ctrMask = pCfg->thdCtrMask | ovfMask;
    uint32_t curCtrMask = ctrMask;
    while (curCtrMask != 0) {
        uint32_t ctr = upc_cntlz32(curCtrMask);
        uint32_t thisCtrMask = MASK32_bit(ctr);
        //printf("ctrMask=0x%08x, ctr=%d, mask=0x%08x\n", ctrMask, ctr, ~MASK32_bit(ctr));
        curCtrMask &= ~thisCtrMask;    // mask off counter bit.
        uint64_t restoreVal = pCfg->ctrSS[ctr];
        upc_c->data24.grp[cunit].counter[ctr] = restoreVal;
    }


    // upc_c will set the upc_p counter arm bit whenever bits 0-51 are set in the upc_c counter,
    // regardless whether counters are running or not.
    // So, before updating the upc_p count, we need to wait for the upc_c return to stabilize.
    // This takes about 70 cycles.
    Upci_Delay(100);

    // Now update the upc_p counters.
    curCtrMask = ctrMask;
    while (curCtrMask != 0) {
        uint32_t ctr = upc_cntlz32(curCtrMask);
        uint32_t thisCtrMask = MASK32_bit(ctr);
        //printf("ctrMask=0x%08x, ctr=%d, mask=0x%08x\n", ctrMask, ctr, ~MASK32_bit(ctr));
        curCtrMask &= ~thisCtrMask;    // mask off counter bit.
        upc_p->counter_cfg[ctr] = pCfg->ctrCfg[ctr];
        uint64_t restoreVal = pCfg->ctrSS[ctr];

        // get upc_p ctr value to restore (include armed bit if needed for overflows)
        if (UNLIKELY(thisCtrMask & ovfMask )) {
            upc_p->counter[ctr] = UPC_P__COUNTER__THRESHOLD_COUNT_set(restoreVal); // includes armed bit as necessary
        }
        else {
            upc_p->counter[ctr] = UPC_P__COUNTER__STABLE_COUNT_set(restoreVal);
        }

        //printf("ctr=%d, cfg=0x%016lx\n", ctr, upc_p->counter_cfg[ctr]);
    }


    Upci_A2PC_ApplyRegs(&(pCfg->a2sprs));

    uint64_t cfg = upc_p->cfg_invert;
    cfg &= ~(pCfg->a2EdgeMask);
    cfg |= pCfg->a2Invert;
    upc_p->cfg_invert = cfg;

    cfg = upc_p->cfg_edge;
    cfg &= ~(pCfg->a2EdgeMask);
    cfg |= pCfg->a2Edge;
    upc_p->cfg_edge = cfg;

    if (pCfg->l1pHwMode >= 0) {
        UPC_L1p_SetMode(UPC_LOCAL_UNIT, pCfg->l1pHwMode);
    }
    else {
        UPC_L1p_EnableUPC(UPC_LOCAL_UNIT);
    }
    //printf("done upc_p_addr=0x%016lx\n", (uint64_t)upc_p);


    // insure int statuses are disabled
    upc_p->int_status_w1c = ctlMask;

    cfg = upc_p->p_config;
    if (pCfg->fpWeights) {
        cfg &= ~(UPC_P__CONFIG__DIV_WEIGHT_set(0xF)
                 | UPC_P__CONFIG__SQRT_WEIGHT_set(0xF)
                 | UPC_P__CONFIG__AXU_MATCH_WEIGHT_set(0xF));
        cfg |= pCfg->fpWeights;
    }

    if (pCfg->enableInts) {
        cfg |= pCfg->intEnableMask;
    }
    upc_p->p_config = cfg;


    mbar();
}
