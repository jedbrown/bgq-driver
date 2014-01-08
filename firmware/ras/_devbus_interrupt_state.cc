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
  const std::string            LOCAL_RING_DESCR = " [LOCAL_RING] Parity error on DCR ring";
  const std::string       SLAVE_BAD_STORE_DESCR = " [SLAVE_BAD_STORE] Outbound store with ttype not equal to 100000";
  const std::string        SLAVE_BAD_LOAD_DESCR = " [SLAVE_BAD_LOAD] Outbound load with lwarx ttype (001001)";
  const std::string SLAVE_VALIDSRC_PARITY_DESCR = " [SLAVE_VALIDSRC_PARITY] Parity error on outbound source, valid from request switch";
  const std::string       SLVOUT_CTRL_ECC_DESCR = " [SLVOUT_CTRL_ECC] Uncorrectable error in outbound control from request switch";
  const std::string     SLAVE_FIFO_PARITY_DESCR = " [SLAVE_FIFO_PARITY] Parity error in outbound request FIFO control";
  const std::string         SLAVE_FIFO_SM_DESCR = " [SLAVE_FIFO_SM] State machine error in outbound request FIFO";
  const std::string       SLAVE_FIFO_DATA_DESCR = " [SLAVE_FIFO_DATA] Parity error in outbound request FIFO data";
  const std::string      SLVOUT_WDATA_ECC_DESCR = " [SLVOUT_WDATA_ECC] Uncorrectable error in outbound write data from request switch";
  const std::string   PCIE_IW_ADDR_PARITY_DESCR = " [PCIE_IW_ADDR_PARITY] Parity error in address received from PCIe inbound write interface";
  const std::string    PCIE_IW_BEN_PARITY_DESCR = " [PCIE_IW_BEN_PARITY] Parity error in byte enables received from PCIe inbound write interface";
  const std::string      PCIE_IW_WDATA_UE_DESCR = " [PCIE_IW_WDATA_UE] Uncorrectable ECC error in write data received from PCIe inbound write interface";
  const std::string       PCIE_IR_IO_RCVD_DESCR = " [PCIE_IR_IO_RCVD] IO space request received by PCIe inbound read interface";
  const std::string          PCIE_IR_ATTR_DESCR = " [PCIE_IR_ATTR] Parity error in attributes received from PCIe inbound read interface";
  const std::string   PCIE_IR_RANGE_START_DESCR = " [PCIE_IR_RANGE_START] Received inbound PCIe read request with out-of-range start address";
  const std::string     PCIE_IR_RANGE_END_DESCR = " [PCIE_IR_RANGE_END] Received inbound PCIe read request with out-of-range end address";
  const std::string       PCIE_IR_MAX_TOP_DESCR = " [PCIE_IR_MAX_TOP] Received inbound PCIe read request exceeding 64GB maximum boundary";
  const std::string        PCIE_IR_BOUNDS_DESCR = " [PCIE_IR_BOUNDS] Received inbound PCIe read request that is not within configured bounds";
  const std::string            PCIE_IR_SM_DESCR = " [PCIE_IR_SM] Illegal state reached by PCIe inbound read state machine";
  const std::string            SR_RD_ATTR_DESCR = " [SR_RD_ATTR] Parity error in response switch read attributes";
  const std::string           SR_TAG_FIFO_DESCR = " [SR_TAG_FIFO] Parity error in response switch read attribute tag FIFO";
  const std::string              SW_STACK_DESCR = " [SW_STACK] Parity error in inbound request switch bookkeeping stack";
  const std::string            SW_RA_CTRL_DESCR = " [SW_RA_CTRL] Parity error in inbound request switch control FIFO";
  const std::string           SW_TGT_FIFO_DESCR = " [SW_TGT_FIFO] Parity error in inbound request switch target FIFO";
  const std::string              SW_GRANT_DESCR = " [SW_GRANT] Parity error in inbound request switch grant attributes";
  const std::string             MSYNC_GEN_DESCR = " [MSYNC_GEN] Parity error in msync generation for two cycles";
  const std::string           BEDRAM_SIZE_DESCR = " [BEDRAM_SIZE] Unsupported 128-byte request made to the boot eDRAM";
  const std::string           BEDRAM_FIFO_DESCR = " [BEDRAM_FIFO] Parity error in boot eDRAM attribute FIFO";
  const std::string        BEDRAM_COUNTER_DESCR = " [BEDRAM_COUNTER] Boot eDRAM counter overflow or underflow";
  const std::string         PCIE_MEM_SIZE_DESCR = " [PCIE_MEM_SIZE] Outbound request exceeded 16 bytes in size";
  const std::string    PCIE_CFGIO_WR_SIZE_DESCR = " [PCIE_CFGIO_WR_SIZE] Outbound configuration or IO write request exceeded 4 bytes in size";
  const std::string    PCIE_CFGIO_RD_SIZE_DESCR = " [PCIE_CFGIO_RD_SIZE] Outbound configuration or IO read request exceeded 4 bytes in size";
  const std::string         PCIE_OR_ALIGN_DESCR = " [PCIE_OR_ALIGN] Outbound read request not aligned within 16-byte boundaries";
  const std::string          PCIE_OR_ATTR_DESCR = " [PCIE_OR_ATTR] Parity error in outbound read response attributes";
  const std::string           PCIE_OR_ECC_DESCR = " [PCIE_OR_ECC] Uncorrectable ECC error in outbound read response data";
  const std::string            MSI_PARITY_DESCR = " [MSI_PARITY] Parity error in an MSI register";
  const std::string        UPC_REQ_PARITY_DESCR = " [UPC_REQ_PARITY] Parity error in UPC request attributes";
  const std::string      UPC_WDATA_PARITY_DESCR = " [UPC_WDATA_PARITY] Parity error in UPC request write data";
  const std::string      UPC_RDATA_PARITY_DESCR = " [UPC_RDATA_PARITY] Parity error in UPC read response data";
  const std::string       UPC_REQ_TIMEOUT_DESCR = " [UPC_REQ_TIMEOUT] UPC request timeout with no ack";
  const std::string        GEA_REQ_PARITY_DESCR = " [GEA_REQ_PARITY] Parity error in GEA request attributes";
  const std::string      GEA_WDATA_PARITY_DESCR = " [GEA_WDATA_PARITY] Parity error in GEA request write data";
  const std::string      GEA_RDATA_PARITY_DESCR = " [GEA_RDATA_PARITY] Parity error in GEA read response data";
  const std::string       GEA_REQ_TIMEOUT_DESCR = " [GEA_REQ_TIMEOUT] GEA request timeout with no ack";
  const std::string    L2_IFC_FIFO_PARITY_DESCR = " [L2_IFC_FIFO_PARITY] Parity error in L2 interface FIFOs";
  const std::string       L2_SLICE_SELECT_DESCR = " [L2_SLICE_SELECT] Parity error in L2 slice select input";
  const std::string     RDBUF_A_STACK_PTY_DESCR = " [RDBUF_A_STACK_PTY] Parity error in BeDRAM port of outbound read return bookkeeping stack";
  const std::string     RDBUF_B_STACK_PTY_DESCR = " [RDBUF_B_STACK_PTY] Parity error in shared port of outbound read return bookkeeping stack";
  const std::string     RDBUF_SR_FIFO_PTY_DESCR = " [RDBUF_SR_FIFO_PTY] Parity error in response switch request FIFO";
  const std::string       RDBUF_READY_REG_DESCR = " [RDBUF_READY_REG] Parity error in response switch ready register";
  const std::string          RDBUF_SR_GNT_DESCR = " [RDBUF_SR_GNT] Parity error in response switch grant attributes";
  const std::string        TI_ADDR_PARITY_DESCR = " [TI_ADDR_PARITY] Parity error in address received with testint request";
  const std::string            PCIE_INT_A_DESCR = " [PCIE_INT_A] PCIe legacy interrupt A";
  const std::string            PCIE_INT_B_DESCR = " [PCIE_INT_B] PCIe legacy interrupt B";
  const std::string            PCIE_INT_C_DESCR = " [PCIE_INT_C] PCIe legacy interrupt C";
  const std::string            PCIE_INT_D_DESCR = " [PCIE_INT_D] PCIe legacy interrupt D";
  const std::string       UPC_COUNTER_OVF_DESCR = " [UPC_COUNTER_OVF] UPC counter overflow";
  const std::string             SW_SWITCH_DESCR = " [SW_SWITCH] Request switch error";
  const std::string             SR_SWITCH_DESCR = " [SR_SWITCH] Response switch error";
  const std::string             SI_SWITCH_DESCR = " [SI_SWITCH] Invalidate switch error";
  const std::string          SLAVE_DECODE_DESCR = " [SLAVE_DECODE] Decode miss in outbound request";

void decode_DB_INTERRUPT_STATE( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "DB_INTERRUPT_STATE error status: ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // LOCAL_RING
    str << LOCAL_RING_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // SLAVE_BAD_STORE
    str << SLAVE_BAD_STORE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // SLAVE_BAD_LOAD
    str << SLAVE_BAD_LOAD_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // SLAVE_VALIDSRC_PARITY
    str << SLAVE_VALIDSRC_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // SLVOUT_CTRL_ECC
    str << SLVOUT_CTRL_ECC_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // SLAVE_FIFO_PARITY
    str << SLAVE_FIFO_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // SLAVE_FIFO_SM
    str << SLAVE_FIFO_SM_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // SLAVE_FIFO_DATA
    str << SLAVE_FIFO_DATA_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // SLVOUT_WDATA_ECC
    str << SLVOUT_WDATA_ECC_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // PCIE_IW_ADDR_PARITY
    str << PCIE_IW_ADDR_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // PCIE_IW_BEN_PARITY
    str << PCIE_IW_BEN_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // PCIE_IW_WDATA_UE
    str << PCIE_IW_WDATA_UE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // PCIE_IR_IO_RCVD
    str << PCIE_IR_IO_RCVD_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // PCIE_IR_ATTR
    str << PCIE_IR_ATTR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // PCIE_IR_RANGE_START
    str << PCIE_IR_RANGE_START_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // PCIE_IR_RANGE_END
    str << PCIE_IR_RANGE_END_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // PCIE_IR_MAX_TOP
    str << PCIE_IR_MAX_TOP_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // PCIE_IR_BOUNDS
    str << PCIE_IR_BOUNDS_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // PCIE_IR_SM
    str << PCIE_IR_SM_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // SR_RD_ATTR
    str << SR_RD_ATTR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-20) ) ) != 0 ) // SR_TAG_FIFO
    str << SR_TAG_FIFO_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-21) ) ) != 0 ) // SW_STACK
    str << SW_STACK_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-22) ) ) != 0 ) // SW_RA_CTRL
    str << SW_RA_CTRL_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-23) ) ) != 0 ) // SW_TGT_FIFO
    str << SW_TGT_FIFO_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-24) ) ) != 0 ) // SW_GRANT
    str << SW_GRANT_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-25) ) ) != 0 ) // MSYNC_GEN
    str << MSYNC_GEN_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-26) ) ) != 0 ) // BEDRAM_SIZE
    str << BEDRAM_SIZE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-27) ) ) != 0 ) // BEDRAM_FIFO
    str << BEDRAM_FIFO_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-28) ) ) != 0 ) // BEDRAM_COUNTER
    str << BEDRAM_COUNTER_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-29) ) ) != 0 ) // PCIE_MEM_SIZE
    str << PCIE_MEM_SIZE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-30) ) ) != 0 ) // PCIE_CFGIO_WR_SIZE
    str << PCIE_CFGIO_WR_SIZE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-31) ) ) != 0 ) // PCIE_CFGIO_RD_SIZE
    str << PCIE_CFGIO_RD_SIZE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-32) ) ) != 0 ) // PCIE_OR_ALIGN
    str << PCIE_OR_ALIGN_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-33) ) ) != 0 ) // PCIE_OR_ATTR
    str << PCIE_OR_ATTR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-34) ) ) != 0 ) // PCIE_OR_ECC
    str << PCIE_OR_ECC_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-35) ) ) != 0 ) // MSI_PARITY
    str << MSI_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-36) ) ) != 0 ) // UPC_REQ_PARITY
    str << UPC_REQ_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-37) ) ) != 0 ) // UPC_WDATA_PARITY
    str << UPC_WDATA_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-38) ) ) != 0 ) // UPC_RDATA_PARITY
    str << UPC_RDATA_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-39) ) ) != 0 ) // UPC_REQ_TIMEOUT
    str << UPC_REQ_TIMEOUT_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-40) ) ) != 0 ) // GEA_REQ_PARITY
    str << GEA_REQ_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-41) ) ) != 0 ) // GEA_WDATA_PARITY
    str << GEA_WDATA_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-42) ) ) != 0 ) // GEA_RDATA_PARITY
    str << GEA_RDATA_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-43) ) ) != 0 ) // GEA_REQ_TIMEOUT
    str << GEA_REQ_TIMEOUT_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-44) ) ) != 0 ) // L2_IFC_FIFO_PARITY
    str << L2_IFC_FIFO_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-45) ) ) != 0 ) // L2_SLICE_SELECT
    str << L2_SLICE_SELECT_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-46) ) ) != 0 ) // RDBUF_A_STACK_PTY
    str << RDBUF_A_STACK_PTY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-47) ) ) != 0 ) // RDBUF_B_STACK_PTY
    str << RDBUF_B_STACK_PTY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-48) ) ) != 0 ) // RDBUF_SR_FIFO_PTY
    str << RDBUF_SR_FIFO_PTY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-49) ) ) != 0 ) // RDBUF_READY_REG
    str << RDBUF_READY_REG_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-50) ) ) != 0 ) // RDBUF_SR_GNT
    str << RDBUF_SR_GNT_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-51) ) ) != 0 ) // TI_ADDR_PARITY
    str << TI_ADDR_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-52) ) ) != 0 ) // PCIE_INT_A
    str << PCIE_INT_A_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-53) ) ) != 0 ) // PCIE_INT_B
    str << PCIE_INT_B_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-54) ) ) != 0 ) // PCIE_INT_C
    str << PCIE_INT_C_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-55) ) ) != 0 ) // PCIE_INT_D
    str << PCIE_INT_D_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-56) ) ) != 0 ) // UPC_COUNTER_OVF
    str << UPC_COUNTER_OVF_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-57) ) ) != 0 ) // SW_SWITCH
    str << SW_SWITCH_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-58) ) ) != 0 ) // SR_SWITCH
    str << SR_SWITCH_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-59) ) ) != 0 ) // SI_SWITCH
    str << SI_SWITCH_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-60) ) ) != 0 ) // SLAVE_DECODE
    str << SLAVE_DECODE_DESCR + ";";
}

