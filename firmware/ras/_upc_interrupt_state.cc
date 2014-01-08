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
  const std::string     DEVBUS_CTL_PERR_DESCR = " [DEVBUS_CTL_PERR] DevBus control parity error ";
  const std::string         PRING_ERROR_DESCR = " [PRING_ERROR] An error was detected on the Processor/L2 UPC Daisy Chain ";
  const std::string PRING_SEND_SM_ERROR_DESCR = " [PRING_SEND_SM_ERROR] Processor ring send state machine error";
  const std::string  PRING_RCV_SM_ERROR_DESCR = " [PRING_RCV_SM_ERROR] Processor ring receive state machine error";
  const std::string       MMIO_SM_ERROR_DESCR = " [MMIO_SM_ERROR] MMIO state machine error";
  const std::string      ACCUM_SM_ERROR_DESCR = " [ACCUM_SM_ERROR] Accumulate state machine error";
  const std::string     UPC_P_SRAM_PERR_DESCR = " [UPC_P_SRAM_PERR] upc_p sram parity error";
  const std::string        IO_SRAM_PERR_DESCR = " [IO_SRAM_PERR] IO sram parity error";
  const std::string IORING_RCV_SM_ERROR_DESCR = " [IORING_RCV_SM_ERROR] IO ring receive state machine error";
  const std::string        IORING_ERROR_DESCR = " [IORING_ERROR] An error was detected on the IO UPC Daisy Chain ";
  const std::string              M1_OVF_DESCR = " [M1_OVF] Mode1 counter overflow ";
  const std::string    PROC_COUNTER_OVF_DESCR = " [PROC_COUNTER_OVF] One of the Processor/L2 Upc counters overflowed";
  const std::string      IO_COUNTER_OVF_DESCR = " [IO_COUNTER_OVF] One of the IO counters overflowed";
  const std::string          MODE2_DONE_DESCR = " [MODE2_DONE] Mode2 Capture is complete ";

void decode_UPC_C_INTERRUPT_STATE( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "UPC_C_INTERRUPT_STATE error status: ";


  if ( ( status & ((uint64_t)1 << (63-0-62) ) ) != 0 ) // DEVBUS_CTL_PERR
    str << DEVBUS_CTL_PERR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-61) ) ) != 0 ) // PRING_ERROR
    str << PRING_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-60) ) ) != 0 ) // PRING_SEND_SM_ERROR
    str << PRING_SEND_SM_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-59) ) ) != 0 ) // PRING_RCV_SM_ERROR
    str << PRING_RCV_SM_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-58) ) ) != 0 ) // MMIO_SM_ERROR
    str << MMIO_SM_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-57) ) ) != 0 ) // ACCUM_SM_ERROR
    str << ACCUM_SM_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-56) ) ) != 0 ) // UPC_P_SRAM_PERR
    str << UPC_P_SRAM_PERR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-55) ) ) != 0 ) // IO_SRAM_PERR
    str << IO_SRAM_PERR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-54) ) ) != 0 ) // IORING_RCV_SM_ERROR
    str << IORING_RCV_SM_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-53) ) ) != 0 ) // IORING_ERROR
    str << IORING_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-51) ) ) != 0 ) // M1_OVF
    str << M1_OVF_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-63) ) ) != 0 ) // PROC_COUNTER_OVF
    str << PROC_COUNTER_OVF_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-62) ) ) != 0 ) // IO_COUNTER_OVF
    str << IO_COUNTER_OVF_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-61) ) ) != 0 ) // MODE2_DONE
    str << MODE2_DONE_DESCR + ";";
}

