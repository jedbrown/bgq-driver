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
#include <hwi/include/bqc/upc_c_dcr.h>
#include <hwi/include/bqc/gea_dcr.h>

#include "Firmware_internals.h"
#include "Firmware_RAS.h"

//  +------------------------------------------------------------------------+
//  | NOTE:  Lesson Learned from Hardware Issue 1138: The UPC_C unit is      |
//  |        not on the DCR ring.  Thus it needs to be accessed via the      |
//  |        devbus via MMIO.                                                |
//  +------------------------------------------------------------------------+

#define UPC_C_MMIO ((upc_c_dcr_t*)(PHYMAP_MINADDR_UPC | PHYMAP_PRIVILEGEDOFFSET))

int fw_upc_machineCheckHandler( uint64_t status[] ) {

  fw_uint64_t details[16];
  fw_uint16_t n = 0;

  details[n++] = UPC_C_MMIO->upc_c_interrupt_state__machine_check;
  details[n++] = UPC_C_MMIO->interrupt_internal_error__machine_check;

  fw_machineCheckRas( FW_RAS_UPC_MACHINE_CHECK, details, n, __FILE__, __LINE__ );

  return -1;
}

int fw_upc_init( void ) {

#ifndef FW_PREINSTALLED_GEA_HANDLERS

  uint64_t mask[3] = { UPC_GEA_MASK_0, UPC_GEA_MASK_1, UPC_GEA_MASK_2  };

  fw_installGeaHandler( fw_upc_machineCheckHandler, mask );

#endif

  UPC_C_MMIO->upc_c_interrupt_state_control_low = 
    UPC_C_DCR__UPC_C_INTERRUPT_STATE_CONTROL_LOW__DEVBUS_CTL_PERR_set(1)       | // DevBus control parity error 
    UPC_C_DCR__UPC_C_INTERRUPT_STATE_CONTROL_LOW__PRING_ERROR_set(1)           | // An error was detected on the Processor/L2 UPC Daisy Chain 
    UPC_C_DCR__UPC_C_INTERRUPT_STATE_CONTROL_LOW__PRING_SEND_SM_ERROR_set(1)   | // Processor ring send state machine error. 
    UPC_C_DCR__UPC_C_INTERRUPT_STATE_CONTROL_LOW__PRING_RCV_SM_ERROR_set(1)    | // Processor ring receive state machine error. 
    UPC_C_DCR__UPC_C_INTERRUPT_STATE_CONTROL_LOW__MMIO_SM_ERROR_set(1)         | // MMIO state machine error. 
    UPC_C_DCR__UPC_C_INTERRUPT_STATE_CONTROL_LOW__ACCUM_SM_ERROR_set(1)        | // Accumulate state machine error. 
    UPC_C_DCR__UPC_C_INTERRUPT_STATE_CONTROL_LOW__UPC_P_SRAM_PERR_set(1)       | // upc_p sram parity error
    UPC_C_DCR__UPC_C_INTERRUPT_STATE_CONTROL_LOW__IO_SRAM_PERR_set(1)          | // IO sram parity error
    UPC_C_DCR__UPC_C_INTERRUPT_STATE_CONTROL_LOW__IORING_RCV_SM_ERROR_set(1)   | // IO ring receive state machine error. 
    UPC_C_DCR__UPC_C_INTERRUPT_STATE_CONTROL_LOW__IORING_ERROR_set(1)          | // An error was detected on the IO UPC Daisy Chain 
    // <DISABLED> UPC_C_DCR__UPC_C_INTERRUPT_STATE_CONTROL_LOW__M1_OVF_set(1)                | // Mode1 counter overflow 
    0;

  ppc_msync();

  UPC_C_MMIO->interrupt_internal_error_control_high =
    UPC_C_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
    UPC_C_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
    UPC_C_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
    UPC_C_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
    0 ;

  ppc_msync();

  return 0;
}


