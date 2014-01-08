/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */



#include <firmware/include/Firmware.h>
#include <hwi/include/bqc/testint_dcr.h>
#include <hwi/include/bqc/gea_dcr.h>
#include <hwi/include/bqc/testint_inlines.h>

#include "Firmware_internals.h"
#include "Firmware_RAS.h"

int fw_testint_machineCheckHandler( uint64_t status[] ) {

  fw_uint64_t details[16];
  fw_uint16_t n = 0;

  details[n++] = TESTINT_DCR( TI_INTERRUPT_STATE__MACHINE_CHECK );
  details[n++] = TESTINT_DCR_PRIV_PTR->ti_interrupt_state__machine_check;

  details[n++] = TESTINT_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK );
  details[n++] = TESTINT_DCR_PRIV_PTR->interrupt_internal_error__machine_check;

  fw_machineCheckRas( FW_RAS_TESTINT_MACHINE_CHECK, details, n, __FILE__, __LINE__ );

  return -1;
}

int fw_testint_init( void ) {

    // Set up the config register to indicate I/O node, swaps, etc.

    uint64_t config1_mask = 
	TESTINT_CONFIG1_BIT_SWAP(-1) |
	TESTINT_CONFIG1_IS_IONODE |
	TESTINT_CONFIG1_USE_PORT6_FOR_IO |
	TESTINT_CONFIG1_USE_PORT7_FOR_IO ;
    

    uint64_t nf2 = FW_Personality.Network_Config.NetFlags2;
    uint64_t config1_data = TESTINT_CONFIG1_BIT_SWAP(TI_GET_ALL_TORUS_DIM_REVERSED(nf2));

    config1_data |= ( ( nf2 & TI_USE_PORT6_FOR_IO ) ? TESTINT_CONFIG1_USE_PORT6_FOR_IO : 0 );
    config1_data |= ( ( nf2 & TI_USE_PORT7_FOR_IO ) ? TESTINT_CONFIG1_USE_PORT7_FOR_IO : 0 );
    config1_data |= PERS_ENABLED(PERS_ENABLE_IsIoNode) ? 	TESTINT_CONFIG1_IS_IONODE  : 0;

    DCRWritePriv( 
	TESTINT_DCR(CONFIG1), 
	( DCRReadPriv(TESTINT_DCR(CONFIG1)) & ~config1_mask ) | config1_data
	);


  DCRWritePriv( TESTINT_DCR( TI_INTERRUPT_STATE__STATE ), 0xFFFFFFFFFFFFFFFFull ); // Clear all bits
  ppc_msync();

  fw_pdelay(100);

#ifndef FW_PREINSTALLED_GEA_HANDLERS

  uint64_t mask[3] = { TESTINT_GEA_MASK_0, TESTINT_GEA_MASK_1, TESTINT_GEA_MASK_2  };

  fw_installGeaHandler( fw_testint_machineCheckHandler, mask );

#endif

  DCRWritePriv( TESTINT_DCR( TI_INTERRUPT_STATE_CONTROL_HIGH), 
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_0_set(1)         | // BIC TDR bit 0
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_1_set(1)         | // BIC TDR bit 1
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_2_set(1)         | // BIC TDR bit 2
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_3_set(1)         | // BIC TDR bit 3
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_4_set(1)         | // BIC TDR bit 4
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_5_set(1)         | // BIC TDR bit 5
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_6_set(1)         | // BIC TDR bit 6
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_7_set(1)         | // BIC TDR bit 7
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_8_set(1)         | // BIC TDR bit 8
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_9_set(1)         | // BIC TDR bit 9
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_10_set(1)        | // BIC TDR bit 10
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_11_set(1)        | // BIC TDR bit 11
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_12_set(1)        | // BIC TDR bit 12
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_13_set(1)        | // BIC TDR bit 13
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_14_set(1)        | // BIC TDR bit 14
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_15_set(1)        | // BIC TDR bit 15
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_16_set(1)        | // BIC TDR bit 16
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_17_set(1)        | // BIC TDR bit 17
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_18_set(1)        | // BIC TDR bit 18
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_19_set(1)        | // BIC TDR bit 19
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_20_set(1)        | // BIC TDR bit 20
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_21_set(1)        | // BIC TDR bit 21
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_22_set(1)        | // BIC TDR bit 22
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__BIC_TDR_23_set(1)        | // BIC TDR bit 23
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__GSYNC_set(1)             | // Global Sync pulse
      TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__DCR_TO_PCB_ERR_set(1)    | // DCR-to-PCB interface error
      TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_set(1)        | // DCR ring parity error.
      TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__INT_PARITY_ERROR_set(1)  | // TestInt Parity Error
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__INT_MBOX_01_EQ_set(1)    | // Mailbox_Reg0 == Mailbox_Reg1
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__INT_MBOX_01_NE_set(1)    | // Mailbox_Reg0 != Mailbox_Reg1
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__INT_MBOX_23_EQ_set(1)    | // Mailbox_Reg2 == Mailbox_Reg3
      // <DISABLED> TESTINT_DCR__TI_INTERRUPT_STATE_CONTROL_HIGH__INT_MBOX_23_NE_set(1)    | // Mailbox_Reg2 != Mailbox_Reg3
      0 );

  DCRWritePriv( TESTINT_DCR(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), 
		//[ 5470] TESTINT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
		// [5470] TESTINT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
		TESTINT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
		TESTINT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
		0 );

  return 0;
}


