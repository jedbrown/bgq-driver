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
#include <hwi/include/bqc/dc_arbiter_dcr.h>
#include <hwi/include/bqc/gea_dcr.h>

#include "Firmware_internals.h"
#include "Firmware_RAS.h"


int fw_dcr_arbiter_machineCheckHandler( uint64_t status[] ) {

  fw_uint64_t details[8];
  fw_uint16_t n = 0;

  fw_uint64_t internal_error = DCRReadPriv( DC_ARBITER_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ) );;
  fw_uint64_t int_reg = DCRReadPriv( DC_ARBITER_DCR( INT_REG__MACHINE_CHECK ) );

  // First check for an illegal DCR access (NO_ACK_AFTER_REQ).  If found, report
  // this separately, including the status_reg, which contains the bogus DCR
  // address.


  if ( int_reg & DC_ARBITER_DCR__INT_REG__NO_ACK_AFTER_REQ_set(1) ) {
      n = 0;
      details[n++] = DC_ARBITER_DCR( INT_REG__MACHINE_CHECK );
      details[n++] = int_reg;
      details[n++] = DC_ARBITER_DCR(STATUS_REG);
      details[n++] = DCRReadPriv( DC_ARBITER_DCR(STATUS_REG) );
      
      fw_machineCheckRas( FW_RAS_BAD_DCR_ACCESS, details, n, __FILE__, __LINE__ );

      // Ack the condition:

      DCRWritePriv( DC_ARBITER_DCR(INT_REG__STATE),  DC_ARBITER_DCR__INT_REG__NO_ACK_AFTER_REQ_set(1) );

      int_reg &= ~DC_ARBITER_DCR__INT_REG__NO_ACK_AFTER_REQ_set(1);
  }

  // If there was only a DCR access error, then return (treat it as recoverable):
  if ( ( internal_error == 0 ) && ( int_reg == 0 ) ) {
      return 0;
  }


  n = 0;

  details[n++] = DC_ARBITER_DCR( INT_REG__MACHINE_CHECK );
  details[n++] = DC_ARBITER_DCR_PRIV_PTR->int_reg__machine_check;

  details[n++] = DC_ARBITER_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK );
  details[n++] = DC_ARBITER_DCR_PRIV_PTR->interrupt_internal_error__machine_check;

  fw_machineCheckRas( FW_RAS_DC_ARBITER_MACHINE_CHECK, details, n, __FILE__, __LINE__ );
  
  return -1;
}

int fw_dcr_arbiter_init() {

  /* ################################################################
   * #  Hardware Issue 814: The default bus timeout for DCR writes  #
   * #  is not sufficient for all units.  Thus we increase it here. #
   * ################################################################ */
  
  DC_ARBITER_DCR_PRIV_PTR->arb_ctrl = DC_ARBITER_DCR__ARB_CTRL_reset | DC_ARBITER_DCR__ARB_CTRL__TIMER_VAL_set(1);

#ifndef FW_PREINSTALLED_GEA_HANDLERS

  uint64_t mask[3] = { DCR_ARB_GEA_MASK_0, DCR_ARB_GEA_MASK_1, DCR_ARB_GEA_MASK_2 };
   
  fw_installGeaHandler( fw_dcr_arbiter_machineCheckHandler, mask );

#endif

  DCRWritePriv( DC_ARBITER_DCR( INT_REG_CONTROL_HIGH), 
		DC_ARBITER_DCR__INT_REG_CONTROL_HIGH__LOCAL_RING_set(1)            | // DCR ring parity error.
		0 );


  DCRWritePriv( DC_ARBITER_DCR( INT_REG_CONTROL_LOW), 
		DC_ARBITER_DCR__INT_REG_CONTROL_LOW__ADD_ERR_set(1)            | // Address parity error.
		DC_ARBITER_DCR__INT_REG_CONTROL_LOW__DATA_ERR_set(1)           | // Data parity error.
		DC_ARBITER_DCR__INT_REG_CONTROL_LOW__NO_REQ_AFTER_GRT_set(1)   | // Time out: no request after a grant.
		DC_ARBITER_DCR__INT_REG_CONTROL_LOW__NO_ACK_AFTER_REQ_set(1)   | // Time out: no acknowlege after a request.
		// [mask per issues 2762 & 1602 (hw)] DC_ARBITER_DCR__INT_REG_CONTROL_LOW__NO_REQ_AFTER_ACK_set(1)   | // Time out: request not removed after an acknowlege.
		DC_ARBITER_DCR__INT_REG_CONTROL_LOW__RING_NOT_CLEAN_set(1)     | // Time out: a pending request or acknowlege still active at the end of an operation.
		DC_ARBITER_DCR__INT_REG_CONTROL_LOW__INV_ARB_STATE_set(1)      | // Invalid arbiter state machine value 
		DC_ARBITER_DCR__INT_REG_CONTROL_LOW__INV_MON_STATE_set(1)      | // Invalid monitor state machine value 
		DC_ARBITER_DCR__INT_REG_CONTROL_LOW__INV_PTY_STATE_set(1)      | // Invalid parity state machine value 
		0 );

  DCRWritePriv( DC_ARBITER_DCR(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), 
		// [5470] DC_ARBITER_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
		// [5470] DC_ARBITER_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
		DC_ARBITER_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
		DC_ARBITER_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
		0 );



  return 0;
}
