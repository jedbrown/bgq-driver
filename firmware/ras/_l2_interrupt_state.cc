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
  const std::string      LOCAL_RING_DESCR = " [LOCAL_RING] Parity error on DCR ring";
  const std::string          RQ_ERR_DESCR = " [RQ_ERR] Request queue error";
  const std::string        CTRL_ERR_DESCR = " [CTRL_ERR] Controller error";
  const std::string         RDB_ERR_DESCR = " [RDB_ERR] Read buffer error";
  const std::string          COH_CE_DESCR = " [COH_CE] Coherence directory SRAM array correctable error";
  const std::string          COH_UE_DESCR = " [COH_UE] Coherence directory SRAM array uncorrectable error";
  const std::string         DIRB_CE_DESCR = " [DIRB_CE] Base directory SRAM array correctable error";
  const std::string         DIRB_UE_DESCR = " [DIRB_UE] Base directory SRAM array uncorrectable error";
  const std::string          EDR_CE_DESCR = " [EDR_CE] eDRAM correctable error";
  const std::string         EDR_OCE_DESCR = " [EDR_OCE] eDRAM operand correctable error";
  const std::string         EDR_OUE_DESCR = " [EDR_OUE] eDRAM operand uncorrectable error";
  const std::string          EDR_UE_DESCR = " [EDR_UE] eDRAM uncorrectable error";
  const std::string          LRU_CE_DESCR = " [LRU_CE] LRU directory SRAM correctable error";
  const std::string          LRU_UE_DESCR = " [LRU_UE] LRU directory SRAM uncorrectable error";
  const std::string          SPR_CE_DESCR = " [SPR_CE] Speculative reader directory correctable error";
  const std::string          SPR_UE_DESCR = " [SPR_UE] Speculative reader directory uncorrectable error";
  const std::string           RQ_CE_DESCR = " [RQ_CE] Request attribute FIFO correctable error";
  const std::string           RQ_UE_DESCR = " [RQ_UE] Request attribute FIFO uncorrectable error";
  const std::string        OVERLOCK_DESCR = " [OVERLOCK] Tried to lock more than overlock_thresh ways down";
  const std::string UPC_COUNTER_OVF_DESCR = " [UPC_COUNTER_OVF] UPC counter overflow";

void decode_L2_INTERRUPT_STATE( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "L2_INTERRUPT_STATE error status: ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // LOCAL_RING
    str << LOCAL_RING_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // RQ_ERR
    str << RQ_ERR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // CTRL_ERR
    str << CTRL_ERR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // RDB_ERR
    str << RDB_ERR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // COH_CE
    str << COH_CE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // COH_UE
    str << COH_UE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // DIRB_CE
    str << DIRB_CE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // DIRB_UE
    str << DIRB_UE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // EDR_CE
    str << EDR_CE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // EDR_OCE
    str << EDR_OCE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // EDR_OUE
    str << EDR_OUE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // EDR_UE
    str << EDR_UE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // LRU_CE
    str << LRU_CE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // LRU_UE
    str << LRU_UE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // SPR_CE
    str << SPR_CE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // SPR_UE
    str << SPR_UE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // RQ_CE
    str << RQ_CE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // RQ_UE
    str << RQ_UE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // OVERLOCK
    str << OVERLOCK_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // UPC_COUNTER_OVF
    str << UPC_COUNTER_OVF_DESCR + ";";
}

