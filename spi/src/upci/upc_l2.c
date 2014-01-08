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

#define _UPC_L2_C_
#include "spi/include/upci/upc_l2.h"

//! \addtogroup upci_spi
//@{
/**
 * \file
 *
 * \brief L2 Unit methods
 */
//@}



void UPC_L2_EnableUPC(UpciBool_t indepCtl, UpciBool_t combine)
{
    int i;
    for (i=0; i < L2_DCR_num; i++) {
        DCRWriteUser(L2_DCR(i, UPC_L2_COUNTER_CONTROL_W1S),
                L2_DCR__UPC_L2_COUNTER_CONTROL_RW__RESET_set(0xFFFFUL) |
                L2_DCR__UPC_L2_COUNTER_CONTROL_RW__ENABLE_set( (indepCtl ? 0 : 0xFFFFUL) ));
        DCRWriteUser(L2_DCR(i, UPC_L2_CONFIG),
                L2_DCR__UPC_L2_CONFIG__UPC_L2_RING_ENABLE_set(1));
                //| L2_DCR__UPC_L2_CONFIG__SYNC_OVERRIDE_set( (indepCtl ? 0xFFFFUL : 0) ) );
                // Switch to rely on sync start - rely on other counter types having override set.
    }
    UPC_C_DCR__CONFIG__L2_COMBINE_insert(upc_c->c_config, (combine==UpciTrue?1:0));

    mbar();
}


//! \brief: UPC_L2_DisableUPC
void UPC_L2_DisableUPC()
{
    int i;
    for (i=0; i < L2_DCR_num; i++) {
        DCRWriteUser(L2_DCR(i, UPC_L2_CONFIG),
                L2_DCR__UPC_L2_CONFIG__UPC_L2_RING_ENABLE_set(0) );
        DCRWriteUser(L2_DCR(i, UPC_L2_COUNTER_CONTROL_W1C),
                L2_DCR__UPC_L2_COUNTER_CONTROL_RW__ENABLE_set(0) );
    }
    mbar();
}




void UPC_L2_Reset()
{
    int i;
    int j;

    // Switch to using UPC_C Sync Control
    uint64_t saveRunState = DCRReadUser(UPC_C_DCR(COUNTER_START));
    UPC_C_Stop_Sync_Counting();

    // Don't need to wait for counts to flush on L2
    // Low granularity counts are cleared each 800 cycles
    // and residuals considered low enough to ignore.

    // check if combining counts
    if (UPC_C_DCR__CONFIG__L2_COMBINE_get(upc_c->c_config)) {
        // clear only combined upc_c counts
        for (j=0; j < UPC_L2_NUM_COUNTERS; j++) {
            upc_c->data16.grp[UPC_C_SRAM_BASE_L2_GROUP].counter[j] = 0;
        }
    }
    else {
        // clear all upc_c counts
        for (i=0; i < L2_DCR_num; i++) {
            for (j=0; j < UPC_L2_NUM_COUNTERS; j++) {
                upc_c->data16.grp[UPC_C_SRAM_BASE_L2_GROUP + i].counter[j] = 0;
            }
        }
    }

    if (saveRunState) {
        UPC_C_Start_Sync_Counting();
    }

    mbar();
}

