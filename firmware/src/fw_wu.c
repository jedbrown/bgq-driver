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
#include <hwi/include/bqc/wu_dcr.h>
#include <hwi/include/bqc/gea_dcr.h>

#include "Firmware_internals.h"

int fw_wu_machineCheckHandler( uint64_t status[] ) {

#if 0
  /**
   * @todo RAS message goes here
   */

  printf(
	 "Wakeup status : machine-check=%lX critical=%lX standard=%lX internal-error=%lX\n",
	 WU_DCR_PRIV_PTR->ti_interrupt_state__machine_check, 
	 WU_DCR_PRIV_PTR->ti_interrupt_state__critical,
	 WU_DCR_PRIV_PTR->ti_interrupt_state__noncritical, 
	 WU_DCR_PRIV_PTR->interrupt_internal_error__machine_check
	 );
#endif
  return -1;
}

int fw_wu_init( void ) {

#if 0

#ifndef FW_PREINSTALLED_GEA_HANDLERS
  uint64_t mask[3] = { WU_GEA_MASK_0, WU_GEA_MASK_1, WU_GEA_MASK_2  };
  fw_installGeaHandler( fw_testint_machineCheckHandler, mask );
#endif

  DCRWritePriv( WU_DCR(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), 
      WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
      WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
      WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
      WU_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
      0 );

#endif
  return 0;
}


