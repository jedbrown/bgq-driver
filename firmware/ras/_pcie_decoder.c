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

  // Detailed summaries of specific error bits:
  const std::string RNPP_DESCR = "(RNPP) RXNP array (inbound non-posted requests FIFO) parity error.";
  const std::string RPCP_DESCR = "(RPCP) RXPC array (inbound posted requests FIFO) parity error.";
  const std::string RCIP_DESCR = "(RCIP) RXCIF array (Received completions indications FIFO) parity error.";
  const std::string RCCP_DESCR = "(RCCP) Receive completions control arrays parity error.";
  const std::string RFTP_DESCR = "(RFTP) RXFT array (inbound read tags queue array) parity error.";

void decode_UTL_System_Bus_Agent_Status_Register( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "UTL System Bus Agent Status Register error status: ";


  if ( ( status & ((uint64_t)1 << (63-32-0) ) ) != 0 ) // RNPP
    str << RNPP_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-2) ) ) != 0 ) // RPCP
    str << RPCP_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-4) ) ) != 0 ) // RCIP
    str << RCIP_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-5) ) ) != 0 ) // RCCP
    str << RCCP_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-8) ) ) != 0 ) // RFTP
    str << RFTP_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string TPCP_DESCR = "(TPCP) TXPC array (outbound posted requests FIFO) parity error.";
  const std::string TNPP_DESCR = "(TNPP) TXNP array (outbound non-posted requests FIFO) parity error.";
  const std::string TFTP_DESCR = "(TFTP) TXFT array (output non-posted requests free tags FIFO) parity error.";
  const std::string TCAP_DESCR = "(TCAP) TXCA array (transmit completions attributes array) parity error.";
  const std::string TCIP_DESCR = "(TCIP) TXCIF array (transmit completions idications array) parity error.";
  const std::string RCAP_DESCR = "(RCAP) RXCA array (receive completions attributes array) parity error.";
  const std::string PLUP_DESCR = "(PLUP) PCI Express LinkUp status.";
  const std::string PLDN_DESCR = "(PLDN) PCI Express LinkDown status.";
  const std::string OTDD_DESCR = "(OTDD) Outbound request discard status.";

void decode_UTL_Express_Port_Status_Register( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "UTL Express Port Status Register error status: ";


  if ( ( status & ((uint64_t)1 << (63-32-1) ) ) != 0 ) // TPCP
    str << TPCP_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-2) ) ) != 0 ) // TNPP
    str << TNPP_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-3) ) ) != 0 ) // TFTP
    str << TFTP_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-4) ) ) != 0 ) // TCAP
    str << TCAP_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-5) ) ) != 0 ) // TCIP
    str << TCIP_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-6) ) ) != 0 ) // RCAP
    str << RCAP_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-8) ) ) != 0 ) // PLUP
    str << PLUP_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-9) ) ) != 0 ) // PLDN
    str << PLDN_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-10) ) ) != 0 ) // OTDD
    str << OTDD_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string RLCE_DESCR = "(RLCE) Correctable Error Status.";
  const std::string RLNE_DESCR = "(RLNE) Non-fatal Error Status.";
  const std::string RLFE_DESCR = "(RLFE) Fatal Error Status.";
  const std::string RCPE_DESCR = "(RCPE) PME Status.";
  const std::string RCTO_DESCR = "(RCTO) PME Turn Off Acknowledge status.";
  const std::string PINA_DESCR = "(PINA) INTA status.";
  const std::string PINB_DESCR = "(PINB) INTB status.";
  const std::string PINC_DESCR = "(PINC) INTC status.";
  const std::string PIND_DESCR = "(PIND) INTD status.";
  const std::string ALER_DESCR = "(ALER) ASPM L1Enter request status.";
  const std::string CRSE_DESCR = "(CRSE) CRS status.";

void decode_Root_Complex_Status_Register( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "Root Complex Status Register error status: ";


  if ( ( status & ((uint64_t)1 << (63-32-0) ) ) != 0 ) // RLCE
    str << RLCE_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-1) ) ) != 0 ) // RLNE
    str << RLNE_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-2) ) ) != 0 ) // RLFE
    str << RLFE_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-6) ) ) != 0 ) // RCPE
    str << RCPE_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-7) ) ) != 0 ) // RCTO
    str << RCTO_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-8) ) ) != 0 ) // PINA
    str << PINA_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-9) ) ) != 0 ) // PINB
    str << PINB_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-10) ) ) != 0 ) // PINC
    str << PINC_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-11) ) ) != 0 ) // PIND
    str << PIND_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-12) ) ) != 0 ) // ALER
    str << ALER_DESCR;

  if ( ( status & ((uint64_t)1 << (63-32-13) ) ) != 0 ) // CRSE
    str << CRSE_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string serr_DESCR = "(serr) PCIE got a SERR.";
  const std::string pci_cfg_wr_error_DESCR = "(pci_cfg_wr_error) Outbound PCIe configuration or IO write completed in error.";
  const std::string pci_cfg_wr_error_ur_DESCR = "(pci_cfg_wr_error_ur) Outbound PCIe configuration or IO write completed with an unsupported request error.";
  const std::string pci_cfg_wr_error_retry_DESCR = "(pci_cfg_wr_error_retry) Outbound PCIe configuraiton or IO write completed with a retry error.";
  const std::string pci_ow_pty_DESCR = "(pci_ow_pty) Parity error on an outbound PCIe write request received from the device bus.";
  const std::string pci_cfg_rd_error_DESCR = "(pci_cfg_rd_error) Outbound PCIe configuration or IO read cmpleted in error.";
  const std::string pci_cfg_rd_error_ur_DESCR = "(pci_cfg_rd_error_ur) Outbound PCIe configuration or IO read completed with an unsupported request error.";
  const std::string pci_cfg_rd_error_retry_DESCR = "(pci_cfg_rd_error_retry) Outbound PCIe configuration or IO read completed with a retry error.";
  const std::string pci_or_pty_DESCR = "(pci_or_pty) Parity error in an outbound PCIe read request received from the device bus.";
  const std::string pci_or_tag_store_DESCR = "(pci_or_tag_store) Parity error in some entry of the outbound PCIe read tag store.";
  const std::string pci_or_poisoned_DESCR = "(pci_or_poisoned) Outbound PCIe read data was poisoned.";
  const std::string pci_iw_pty_DESCR = "(pci_iw_pty) Parity error in inbound PCIe write request register.";
  const std::string pci_iw_req_size_DESCR = "(pci_iw_req_size) Received inbound PCIe write request with size exceeding 512 bytes.";
  const std::string pci_iw_req_type_DESCR = "(pci_iw_req_type) Received inbound PCIe write request that was for IO or message space.";
  const std::string pci_iw_req_snoop_DESCR = "(pci_iw_req_snoop) Received coherent inbound PCIe write request.";
  const std::string pci_iw_range_start_DESCR = "(pci_iw_range_start) Received inbound PCIe write request with out-of-range start address.";
  const std::string pci_iw_range_end_DESCR = "(pci_iw_range_end) Received inbound PCIe write request with out-of-range end address.";
  const std::string pci_iw_max_top_DESCR = "(pci_iw_max_top) Received inbound PCIe write request exceeding 64GB maximum boundary.";
  const std::string pci_iw_bounds_DESCR = "(pci_iw_bounds) Received inbound PCIe write request that is not within configured bounds.";
  const std::string pci_iw_sm_state_DESCR = "(pci_iw_sm_state) Inbound PCIe write state machine reached an illegal state.";
  const std::string pci_ir_req_snoop_DESCR = "(pci_ir_req_snoop) Received coherent inbound PCIe read requet.";
  const std::string pci_ir_resp_DESCR = "(pci_ir_resp) Parity error in inbound PCIe read response received from the device bus.";
  const std::string pci_ir_ecc_DESCR = "(pci_ir_ecc) Uncorrectable ECC error in inbound PCIe read response data received from the device bus.";
  const std::string cfg_perr_DESCR = "(cfg_perr) Configuration core parity error.";
  const std::string utl_pclk_err_DESCR = "(utl_pclk_err) PCIe error in the PCLK domain of the UTL.";
  const std::string utl_bclk_err_DESCR = "(utl_bclk_err) PCIE error in the BCLK domain.";
  const std::string tdlp_pclk_err_DESCR = "(tdlp_pclk_err) PCIE error in the TLDLP core.";
  const std::string al_pint_DESCR = "(al_pint) UTL Primary Interrupt.";
  const std::string al_sint_DESCR = "(al_sint) UTL Secondary Interrupt.";
  const std::string utl_sm_pty_DESCR = "(utl_sm_pty) UTL internal register state machine error.";
  const std::string upc_counter_ovf_DESCR = "(upc_counter_ovf) UPC counter overflow.";
  const std::string ei_sm_error_DESCR = "(ei_sm_error) Error injection state machine error.";

void decode_PCI_Express_Interrupt_State_Register( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "PCI Express Interrupt State Register error status: ";


  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // serr
    str << serr_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // pci_cfg_wr_error
    str << pci_cfg_wr_error_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // pci_cfg_wr_error_ur
    str << pci_cfg_wr_error_ur_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // pci_cfg_wr_error_retry
    str << pci_cfg_wr_error_retry_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // pci_ow_pty
    str << pci_ow_pty_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // pci_cfg_rd_error
    str << pci_cfg_rd_error_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // pci_cfg_rd_error_ur
    str << pci_cfg_rd_error_ur_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // pci_cfg_rd_error_retry
    str << pci_cfg_rd_error_retry_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // pci_or_pty
    str << pci_or_pty_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // pci_or_tag_store
    str << pci_or_tag_store_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // pci_or_poisoned
    str << pci_or_poisoned_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // pci_iw_pty
    str << pci_iw_pty_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // pci_iw_req_size
    str << pci_iw_req_size_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // pci_iw_req_type
    str << pci_iw_req_type_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // pci_iw_req_snoop
    str << pci_iw_req_snoop_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // pci_iw_range_start
    str << pci_iw_range_start_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // pci_iw_range_end
    str << pci_iw_range_end_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // pci_iw_max_top
    str << pci_iw_max_top_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-20) ) ) != 0 ) // pci_iw_bounds
    str << pci_iw_bounds_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-21) ) ) != 0 ) // pci_iw_sm_state
    str << pci_iw_sm_state_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-22) ) ) != 0 ) // pci_ir_req_snoop
    str << pci_ir_req_snoop_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-23) ) ) != 0 ) // pci_ir_resp
    str << pci_ir_resp_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-24) ) ) != 0 ) // pci_ir_ecc
    str << pci_ir_ecc_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-25) ) ) != 0 ) // cfg_perr
    str << cfg_perr_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-26) ) ) != 0 ) // utl_pclk_err
    str << utl_pclk_err_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-27) ) ) != 0 ) // utl_bclk_err
    str << utl_bclk_err_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-28) ) ) != 0 ) // tdlp_pclk_err
    str << tdlp_pclk_err_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-29) ) ) != 0 ) // al_pint
    str << al_pint_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-30) ) ) != 0 ) // al_sint
    str << al_sint_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-31) ) ) != 0 ) // utl_sm_pty
    str << utl_sm_pty_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-32) ) ) != 0 ) // upc_counter_ovf
    str << upc_counter_ovf_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-33) ) ) != 0 ) // ei_sm_error
    str << ei_sm_error_DESCR;
}

