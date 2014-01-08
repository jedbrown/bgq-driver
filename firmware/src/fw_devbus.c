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
#include <hwi/include/bqc/devbus_dcr.h>
#include <hwi/include/bqc/gea_dcr.h>
#include <hwi/include/bqc/testint_inlines.h>

#include "Firmware_internals.h"
#include "Firmware_RAS.h"

int fw_devbus_machineCheckHandler( uint64_t status[] ) {


  fw_uint64_t details[16];
  fw_uint16_t n = 0;

  details[n++] = DEVBUS_DCR( DB_INTERRUPT_STATE__MACHINE_CHECK );
  details[n++] = DEVBUS_DCR_PRIV_PTR->db_interrupt_state__machine_check;

  details[n++] = DEVBUS_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK );
  details[n++] = DEVBUS_DCR_PRIV_PTR->interrupt_internal_error__machine_check;

  fw_machineCheckRas( FW_RAS_DEVBUS_MACHINE_CHECK, details, n, __FILE__, __LINE__ );

  return -1;
}

int fw_devbus_init( void ) {

  TRACE_ENTRY( TRACE_DevBus );

  //DEVBUS_DCR_PRIV_PTR->db_interrupt_state__machine_check = 0xFFFFFFFFFFFFFFFFull;
  //isync();

#ifndef FW_PREINSTALLED_GEA_HANDLERS 

  uint64_t mask[3] = { DEVBUS_GEA_MASK_0, DEVBUS_GEA_MASK_1, DEVBUS_GEA_MASK_2 };
 

  fw_installGeaHandler( fw_devbus_machineCheckHandler, mask );

#endif

  uint64_t interruptControlHigh =
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_set(1)             | // DCR ring parity error.
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__SLAVE_BAD_STORE_set(1)        | // Outbound store with ttype not equal to 100000
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__SLAVE_BAD_LOAD_set(1)         | // Outbound load with lwarx ttype (001001)
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__SLAVE_VALIDSRC_PARITY_set(1)  | // Parity error on outbound source, valid from request switch
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__SLVOUT_CTRL_ECC_set(1)        | // Uncorrectable error in outbound control from request switch
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__SLAVE_FIFO_PARITY_set(1)      | // Parity error in outbound request FIFO control
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__SLAVE_FIFO_SM_set(1)          | // State machine error in outbound request FIFO
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__SLAVE_FIFO_DATA_set(1)        | // Parity error in outbound request FIFO data
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__SLVOUT_WDATA_ECC_set(1)       | // Uncorrectable error in outbound write data from request switch
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__PCIE_IW_ADDR_PARITY_set(1)    | // Parity error in address received from PCIe inbound write interface
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__PCIE_IW_BEN_PARITY_set(1)     | // Parity error in byte enables received from PCIe inbound write interface
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__PCIE_IW_WDATA_UE_set(1)       | // Uncorrectable ECC error in write data received from PCIe inbound write interface
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__PCIE_IR_IO_RCVD_set(1)        | // IO space request received by PCIe inbound read interface
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__PCIE_IR_ATTR_set(1)           | // Parity error in attributes received from PCIe inbound read interface
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__PCIE_IR_RANGE_START_set(1)    | // Received inbound PCIe read request with out-of-range start address
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__PCIE_IR_RANGE_END_set(1)      | // Received inbound PCIe read request with out-of-range end address
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__PCIE_IR_MAX_TOP_set(1)        | // Received inbound PCIe read request exceeding 64GB maximum boundary
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__PCIE_IR_BOUNDS_set(1)         | // Received inbound PCIe read request that is not within configured bounds
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__PCIE_IR_SM_set(1)             | // Illegal state reached by PCIe inbound read state machine
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__SR_RD_ATTR_set(1)             | // Parity error in response switch read attributes
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__SR_TAG_FIFO_set(1)            | // Parity error in response switch read attribute tag FIFO
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__SW_STACK_set(1)               | // Parity error in inbound request switch bookkeeping stack
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__SW_RA_CTRL_set(1)             | // Parity error in inbound request switch control FIFO
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__SW_TGT_FIFO_set(1)            | // Parity error in inbound request switch target FIFO
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__SW_GRANT_set(1)               | // Parity error in inbound request switch grant attributes
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__MSYNC_GEN_set(1)              | // Parity error in msync generation for two cycles
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__BEDRAM_SIZE_set(1)            | // Unsupported 128-byte request made to the boot eDRAM
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__BEDRAM_FIFO_set(1)            | // Parity error in boot eDRAM attribute FIFO
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__BEDRAM_COUNTER_set(1)         | // Boot eDRAM counter overflow or underflow
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__PCIE_MEM_SIZE_set(1)          | // Outbound request exceeded 16 bytes in size
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__PCIE_CFGIO_WR_SIZE_set(1)     | // Outbound configuration or IO write request exceeded 4 bytes in size
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_HIGH__PCIE_CFGIO_RD_SIZE_set(1)     | // Outbound configuration or IO read request exceeded 4 bytes in size
    0;		

  uint64_t interruptControlLow =
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__PCIE_OR_ALIGN_set(1)           | // Outbound read request not aligned within 16-byte boundaries
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__PCIE_OR_ATTR_set(1)            | // Parity error in outbound read response attributes
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__PCIE_OR_ECC_set(1)             | // Uncorrectable ECC error in outbound read response data
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__MSI_PARITY_set(1)              | // Parity error in an MSI register
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__UPC_REQ_PARITY_set(1)          | // Parity error in UPC request attributes
    // [deleted HW Issue 1115] DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__UPC_WDATA_PARITY_set(1)        | // Parity error in UPC request write data
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__UPC_RDATA_PARITY_set(1)        | // Parity error in UPC read response data
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__UPC_REQ_TIMEOUT_set(1)         | // UPC request timeout with no ack
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__GEA_REQ_PARITY_set(1)          | // Parity error in GEA request attributes
    // [deleted HW Issue 1115] DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__GEA_WDATA_PARITY_set(1)        | // Parity error in GEA request write data
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__GEA_RDATA_PARITY_set(1)        | // Parity error in GEA read response data
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__GEA_REQ_TIMEOUT_set(1)         | // GEA request timeout with no ack
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__L2_IFC_FIFO_PARITY_set(1)      | // Parity error in L2 interface FIFOs
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__L2_SLICE_SELECT_set(1)         | // Parity error in L2 slice select input
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__RDBUF_A_STACK_PTY_set(1)       | // Parity error in BeDRAM port of outbound read return bookkeeping stack
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__RDBUF_B_STACK_PTY_set(1)       | // Parity error in shared port of outbound read return bookkeeping stack
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__RDBUF_SR_FIFO_PTY_set(1)       | // Parity error in response switch request FIFO
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__RDBUF_READY_REG_set(1)         | // Parity error in response switch ready register
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__RDBUF_SR_GNT_set(1)            | // Parity error in response switch grant attributes
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__TI_ADDR_PARITY_set(1)          | // Parity error in address received with testint request
      //DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__PCIE_INT_A_set(1)              | // PCIe legacy interrupt A
      //DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__PCIE_INT_B_set(1)              | // PCIe legacy interrupt B
      //DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__PCIE_INT_C_set(1)              | // PCIe legacy interrupt C
      //DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__PCIE_INT_D_set(1)              | // PCIe legacy interrupt D
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__UPC_COUNTER_OVF_set(1)         | // UPC counter overflow
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__SW_SWITCH_set(1)               | // Request switch error
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__SR_SWITCH_set(1)               | // Response switch error
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__SI_SWITCH_set(1)               | // Invalidate switch error
    DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__SLAVE_DECODE_set(1)            | // DD2
    0;


  if ( FW_DD1_WORKAROUNDS_ENABLED() ) {

    // +---------------------------------------------------------------------------------------+
    // | DD1 workaround for issue 1115.  The DevBus is generating false positive parity errors |
    // | for UPC and GEA write request data.  These errors are masked for DD1.                 |
    // +---------------------------------------------------------------------------------------+

    // +---------------------------------------------------------------------------------------+
    // | DD1 workaround for issue 1136.  The DevBus is generating false UPC timeout errors.    |
    // +---------------------------------------------------------------------------------------+

    interruptControlLow &= ~(
			     DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__UPC_WDATA_PARITY_set(3)        | // Parity error in UPC request write data
			     DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__GEA_WDATA_PARITY_set(3)        | // Parity error in GEA request write data
			     DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__UPC_REQ_TIMEOUT_set(3)         | // UPC request timeout with no ack
			     DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__GEA_REQ_TIMEOUT_set(3)         | // GEA request timeout with no ack
			     DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__SW_SWITCH_set(3)               | // Request switch error (Issue 1409)
			     DEVBUS_DCR__DB_INTERRUPT_STATE_CONTROL_LOW__SLAVE_DECODE_set(3)            | // DD2
			     0
			     );
  }

  DCRWritePriv( DEVBUS_DCR( DB_INTERRUPT_STATE_CONTROL_HIGH), interruptControlHigh );

  DCRWritePriv( DEVBUS_DCR( DB_INTERRUPT_STATE_CONTROL_LOW),  interruptControlLow );

  DCRWritePriv( DEVBUS_DCR(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), 
		// [5470] DEVBUS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
		// [5470] DEVBUS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
		DEVBUS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
		DEVBUS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
		0 );


  //if ( FW_DD1_WORKAROUNDS_ENABLED() ) {

  // +-------------------------------------------------------------------------------------+
  // |  In DD1, we are working around various PCIe/devbus issues.  The following twiddles  |
  // |  grant high priority to the devbus master (PCIe) at the switch level.  However,     |
  // |  we also very well may want to run this way on DD2.                                 |
  // +-------------------------------------------------------------------------------------+

  if ( PERS_ENABLED(PERS_ENABLE_PCIe) ) {
      uint64_t switchConfig = DCRReadPriv( DEVBUS_DCR(SWITCH_CFG) );
      switchConfig &= ~( DEVBUS_DCR__SWITCH_CFG__SW_CFG_set(-1) | DEVBUS_DCR__SWITCH_CFG__SR_CFG_set(-1) );
      switchConfig |= DEVBUS_DCR__SWITCH_CFG__SW_CFG_set(2) | DEVBUS_DCR__SWITCH_CFG__SR_CFG_set(1);
      DCRWritePriv( DEVBUS_DCR(SWITCH_CFG), switchConfig );
  }
  
  //}

  TRACE_EXIT(TRACE_DevBus);

  return 0;
}

