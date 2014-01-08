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
#include <hwi/include/bqc/ms_genct_dcr.h>
#include <hwi/include/bqc/gea_dcr.h>

#include "Firmware_internals.h"
#include "Firmware_RAS.h"


int fw_ms_genct_machineCheckHandler( uint64_t status[] ) {
  
  fw_uint64_t details[16];
  fw_uint16_t n = 0;

  details[n++] = MS_GENCT_DCR( L2_INTERRUPT_STATE__MACHINE_CHECK );
  details[n++] = MS_GENCT_DCR_PRIV_PTR->l2_interrupt_state__machine_check;

  details[n++] = MS_GENCT_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK );
  details[n++] = MS_GENCT_DCR_PRIV_PTR->interrupt_internal_error__machine_check;

  fw_machineCheckRas( FW_RAS_MSGC_MACHINE_CHECK, details, n, __FILE__, __LINE__ );

  return -1;
}


int fw_ms_genct_init( void ) {


#ifndef FW_PREINSTALLED_GEA_HANDLERS

  uint64_t mask[3] = { MSGC_GEA_MASK_0, MSGC_GEA_MASK_1, MSGC_GEA_MASK_2 };
  fw_installGeaHandler( fw_ms_genct_machineCheckHandler, mask );

#endif


  DCRWritePriv( MS_GENCT_DCR( L2_INTERRUPT_STATE_CONTROL_HIGH), 
		MS_GENCT_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_set(1) | // DCR ring parity error
		MS_GENCT_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__TIMEOUT_E_set(1)  | // Timeout occurred
		0 );

  DCRWritePriv( MS_GENCT_DCR(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), 
		// [5470] MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
		// [5470] MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
		MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
		MS_GENCT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
		0 );

  DCRWritePriv( MS_GENCT_DCR(CTRL), MS_GENCT_DCR__CTRL__REUSE_DELAY_set(10));

  return 0;
}

