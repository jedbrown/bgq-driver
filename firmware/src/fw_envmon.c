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
#include <hwi/include/bqc/en_dcr.h>
#include <hwi/include/bqc/gea_dcr.h>

#include "Firmware_internals.h"
#include "Firmware_RAS.h"

int fw_envmon_machineCheckHandler( uint64_t status[] ) {
  
  fw_uint64_t details[16];
  fw_uint16_t n = 0;

  details[n++] = EN_DCR( ENVMON_INTERRUPT_STATE__MACHINE_CHECK );
  details[n++] = EN_DCR_PRIV_PTR->envmon_interrupt_state__machine_check;

  details[n++] = EN_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK );
  details[n++] = EN_DCR_PRIV_PTR->interrupt_internal_error__machine_check;

  fw_machineCheckRas( FW_RAS_ENVMON_MACHINE_CHECK, details, n, __FILE__, __LINE__ );

  return -1;
}


int fw_envmon_init() {


    if ( PERS_ENABLED(PERS_ENABLE_FPGA) ) {
	return 0;
    }
    
#ifndef FW_PREINSTALLED_GEA_HANDLERS

  uint64_t mask[3] = { ENVMON_GEA_MASK_0, ENVMON_GEA_MASK_1, ENVMON_GEA_MASK_2 };

  fw_installGeaHandler( fw_envmon_machineCheckHandler, mask );

#endif

  DCRWritePriv( EN_DCR(ENVMON_INTERRUPT_STATE_CONTROL_HIGH), 
		EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_set(1)             | // DCR ring parity error
		// disabled EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN0_set(1)            | //  en0 input reg exceeds maximum threshold
		// disabled EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN1_set(1)            | //  en1 input reg exceeds maximum threshold
		// disabled EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN2_set(1)            | //  en2 input reg exceeds maximum threshold
		// disabled EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN3_set(1)            | //  en3 input reg exceeds maximum threshold
		// disabled EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__EXCEEDS_EN4_set(1)            | //  en4 input reg exceeds maximum threshold
		// disabled EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__GENERAL_INTERRUPT_EN5_set(1)  | //  en5 general interrupt is true
		// disabled EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN0_set(1)           | //  en0 input reg less than minimum threshold
		// disabled EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN1_set(1)           | //  en1 input reg less than minimum threshold
		// disabled EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN2_set(1)           | //  en2 input reg less than minimum threshold
		// disabled EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN3_set(1)           | //  en3 input reg less than minimum threshold
		// disabled EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__LESSTHAN_EN4_set(1)           | //  en4 input reg less than minimum threshold
		EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__FSM_CHECKSUM_FAIL_set(1)      | //  FSM checksum fail
		EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__FSM_SEQ_FAIL_set(1)           | //  FSM bad sequence detected
		// disabled EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__TVSENSE_EXCEEDS_MAX_set(1)    | //  trigger for tvsense exceeded maximum detected 
		// disabled EN_DCR__ENVMON_INTERRUPT_STATE_CONTROL_HIGH__TVSENSE_LESSTHAN_MIN_set(1)   | //  trigger for tvsense less than minimum detected 
		0 );

  DCRWritePriv( EN_DCR(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), 
		// [5470] EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
		// [5470] EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
		EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
		EN_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
		0 );

  // Need to zero out data to be sent to FPGA, to allow FPGA to poll DCAs
  DCRWritePriv(EN_DCR(USER), _BGQ_SET(4, 62, 0xA)); // user vbuf for default power measurement 

  return 0;
}



