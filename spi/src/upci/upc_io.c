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


//! \addtogroup upci_spi
//@{
/**
 * \file
 *
 * \brief UPC IO Operation Implementation
 */
//@}


#define _UPC_IO_C_
#include "spi/include/upci/upc_io.h"



void UPC_IO_Clear(uint64_t nodeConfig)
{
    UpciBool_t ioRunning = UpciFalse;
    if (!(nodeConfig & PERS_ENABLE_FPGA)) {
        ioRunning = (DCRReadUser(MU_DCR(UPC_MU_COUNTER_CONTROL_RW)) != 0);

        // stop counters
        DCRWriteUser(MU_DCR(UPC_MU_COUNTER_CONTROL_W1C), MU_DCR__UPC_MU_COUNTER_CONTROL_W1C__ENABLE_set(1));
        if (nodeConfig & PERS_ENABLE_PCIe) DCRWriteUser(PE_DCR(UPC_PCIE_COUNTER_CONTROL_W1C), PE_DCR__UPC_PCIE_COUNTER_CONTROL_W1C__ENABLE_set(1));
        DCRWriteUser(DEVBUS_DCR(UPC_DB_COUNTER_CONTROL_W1C), DEVBUS_DCR__UPC_DB_COUNTER_CONTROL_W1C__ENABLE_set(1));

        // reset low level counters
        DCRWriteUser(MU_DCR(UPC_MU_COUNTER_CONTROL_W1S), MU_DCR__UPC_MU_COUNTER_CONTROL_W1S__RESET_set(1));
        if (nodeConfig & PERS_ENABLE_PCIe) DCRWriteUser(PE_DCR(UPC_PCIE_COUNTER_CONTROL_W1S), PE_DCR__UPC_PCIE_COUNTER_CONTROL_W1S__RESET_set(1));
        DCRWriteUser(DEVBUS_DCR(UPC_DB_COUNTER_CONTROL_W1S), DEVBUS_DCR__UPC_DB_COUNTER_CONTROL_W1S__RESET_set(1));
    }
    // let ring clear
    Upci_Delay(800);

    // clear sram counters
    int i;
    for (i=0; i<64; i++) {
        upc_c->iosram_data[i] = 0;
    }

    if (!(nodeConfig & PERS_ENABLE_FPGA)) {
        if (ioRunning) {
            // start counters
            DCRWriteUser(MU_DCR(UPC_MU_COUNTER_CONTROL_W1S), MU_DCR__UPC_MU_COUNTER_CONTROL_W1S__ENABLE_set(1));
            if (nodeConfig & PERS_ENABLE_PCIe) DCRWriteUser(PE_DCR(UPC_PCIE_COUNTER_CONTROL_W1S), PE_DCR__UPC_PCIE_COUNTER_CONTROL_W1S__ENABLE_set(1));
            DCRWriteUser(DEVBUS_DCR(UPC_DB_COUNTER_CONTROL_W1S), DEVBUS_DCR__UPC_DB_COUNTER_CONTROL_W1S__ENABLE_set(1));
        }
    }
    mbar();
}



void  UPC_IO_GetSnapShot(UPC_IO_SnapShot_t *pss)
{
    assert(pss);
    int i;
    for (i=0; i<UPC_IO_Counter_Range; i++) {
        pss->ctr[i] = upc_c->iosram_data[i];
    }
}


