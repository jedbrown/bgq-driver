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
#include <hwi/include/bqc/bedram_dcr.h>
#include <hwi/include/bqc/gea_dcr.h>
#include <hwi/include/bqc/testint_inlines.h>

#include "Firmware_internals.h"
#include "Firmware_RAS.h"


int fw_bedram_machineCheckHandler( uint64_t status[] ) {

  fw_uint64_t details[16];
  fw_uint16_t n = 0;

  details[n++] = BEDRAM_DCR( BEDRAM_INTERRUPT_STATUS__MACHINE_CHECK );
  details[n++] = BEDRAM_DCR_PRIV_PTR->bedram_interrupt_status__machine_check;

  details[n++] = BEDRAM_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK );
  details[n++] = BEDRAM_DCR_PRIV_PTR->interrupt_internal_error__machine_check;

  fw_machineCheckRas( FW_RAS_BEDRAM_MACHINE_CHECK, details, n, __FILE__, __LINE__ );

  return -1;
}

int fw_bedram_init( void ) {

  TRACE_ENTRY(TRACE_BeDRAM);

  // Initialize and acquire the "big stack in use" lock:
  fw_semaphore_init( BeDRAM_LOCKNUM_BIG_STACK_IN_USE, 1 );
  fw_semaphore_down( BeDRAM_LOCKNUM_BIG_STACK_IN_USE );

  if ( PERS_ENABLED( PERS_ENABLE_BeDRAM ) && ! PERS_ENABLED(PERS_ENABLE_FPGA) ) {

#ifndef FW_PREINSTALLED_GEA_HANDLERS

    uint64_t mask[3] = { BEDRAM_GEA_MASK_0, BEDRAM_GEA_MASK_1, BEDRAM_GEA_MASK_2 };

    fw_installGeaHandler( fw_bedram_machineCheckHandler, mask );

#endif

    uint64_t controlHigh =
      BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_DB_PARITY_set(1)     | // Parity error in devbus
      BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_PIPE_PARITY_set(1)   | // Parity error in pipeline
      BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_REF_PARITY_set(1)    | // Parity error in refresh unit
      BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_RES_PARITY_set(1)    | // Parity error in reset unit
      BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_SCB_PARITY_set(1)    | // Parity error in scrub unit
      BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ERR_PARITY_set(1)    | // Parity error in error counter
      BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_DB_ECC_set(1)        | // Uncorrectable ECC error in devbus
      BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_EDRAM_ECC_set(1)     | // Uncorrectable ECC error in eDRAM
      BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_PIPE_ECC_set(1)      | // Uncorrectable ECC error in pipeline
      BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ECC_CE_RAISE_set(1)  | // Total amount of correctable ECC errors exceeded maximum
      // [DISABLED] BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_ALIGNED_set(1)       | // non aligned address
      // [DISABLED] BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_BYTE_SIZE_set(1) | // wrong byte size
      // [DISABLED] BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_NON_PRIV_set(3)  | // non-privileged atomic operation
      0;

    if ( FW_DD1_WORKAROUNDS_ENABLED() ) {
	
      // +---------------------------------------------------------------------------------------------------+
      // | DD1 Workaround for Issue 1211 : tMichael Kaufman requested that the devbus parity error be masked |
      // |   by default.                                                                                     |
      // | DD1 Workaround for Issue 1423 : This is the infamous machine check in Linux/Mellanox driver.      |
      // +---------------------------------------------------------------------------------------------------+

      controlHigh &= ~( 
	  BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH__BEDRAM_DB_PARITY_set(3) |
	  0
	  );
    }


    DCRWritePriv( BEDRAM_DCR( BEDRAM_INTERRUPT_STATUS_CONTROL_HIGH ), controlHigh );

    DCRWritePriv( BEDRAM_DCR(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), 
		  // [5470] BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
		  // [5470] BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
		  BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
		  BEDRAM_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
		  0 );


  }

  TRACE_EXIT(TRACE_BeDRAM);

  return 0;
}

