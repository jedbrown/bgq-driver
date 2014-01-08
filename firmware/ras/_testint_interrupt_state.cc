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
  const std::string        BIC_TDR_0_DESCR = " [BIC_TDR_0] BIC TDR bit 0";
  const std::string        BIC_TDR_1_DESCR = " [BIC_TDR_1] BIC TDR bit 1";
  const std::string        BIC_TDR_2_DESCR = " [BIC_TDR_2] BIC TDR bit 2";
  const std::string        BIC_TDR_3_DESCR = " [BIC_TDR_3] BIC TDR bit 3";
  const std::string        BIC_TDR_4_DESCR = " [BIC_TDR_4] BIC TDR bit 4";
  const std::string        BIC_TDR_5_DESCR = " [BIC_TDR_5] BIC TDR bit 5";
  const std::string        BIC_TDR_6_DESCR = " [BIC_TDR_6] BIC TDR bit 6";
  const std::string        BIC_TDR_7_DESCR = " [BIC_TDR_7] BIC TDR bit 7";
  const std::string        BIC_TDR_8_DESCR = " [BIC_TDR_8] BIC TDR bit 8";
  const std::string        BIC_TDR_9_DESCR = " [BIC_TDR_9] BIC TDR bit 9";
  const std::string       BIC_TDR_10_DESCR = " [BIC_TDR_10] BIC TDR bit 10";
  const std::string       BIC_TDR_11_DESCR = " [BIC_TDR_11] BIC TDR bit 11";
  const std::string       BIC_TDR_12_DESCR = " [BIC_TDR_12] BIC TDR bit 12";
  const std::string       BIC_TDR_13_DESCR = " [BIC_TDR_13] BIC TDR bit 13";
  const std::string       BIC_TDR_14_DESCR = " [BIC_TDR_14] BIC TDR bit 14";
  const std::string       BIC_TDR_15_DESCR = " [BIC_TDR_15] BIC TDR bit 15";
  const std::string       BIC_TDR_16_DESCR = " [BIC_TDR_16] BIC TDR bit 16";
  const std::string       BIC_TDR_17_DESCR = " [BIC_TDR_17] BIC TDR bit 17";
  const std::string       BIC_TDR_18_DESCR = " [BIC_TDR_18] BIC TDR bit 18";
  const std::string       BIC_TDR_19_DESCR = " [BIC_TDR_19] BIC TDR bit 19";
  const std::string       BIC_TDR_20_DESCR = " [BIC_TDR_20] BIC TDR bit 20";
  const std::string       BIC_TDR_21_DESCR = " [BIC_TDR_21] BIC TDR bit 21";
  const std::string       BIC_TDR_22_DESCR = " [BIC_TDR_22] BIC TDR bit 22";
  const std::string       BIC_TDR_23_DESCR = " [BIC_TDR_23] BIC TDR bit 23";
  const std::string            GSYNC_DESCR = " [GSYNC] Global Sync pulse";
  const std::string   DCR_TO_PCB_ERR_DESCR = " [DCR_TO_PCB_ERR] DCR-to-PCB interface error";
  const std::string       LOCAL_RING_DESCR = " [LOCAL_RING] Parity error on DCR ring";
  const std::string INT_PARITY_ERROR_DESCR = " [INT_PARITY_ERROR] TestInt Parity Error";
  const std::string   INT_MBOX_01_EQ_DESCR = " [INT_MBOX_01_EQ] Mailbox_Reg0 == Mailbox_Reg1";
  const std::string   INT_MBOX_01_NE_DESCR = " [INT_MBOX_01_NE] Mailbox_Reg0 != Mailbox_Reg1";
  const std::string   INT_MBOX_23_EQ_DESCR = " [INT_MBOX_23_EQ] Mailbox_Reg2 == Mailbox_Reg3";
  const std::string   INT_MBOX_23_NE_DESCR = " [INT_MBOX_23_NE] Mailbox_Reg2 != Mailbox_Reg3";

void decode_TI_INTERRUPT_STATE( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "TI_INTERRUPT_STATE error status: ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // BIC_TDR_0
    str << BIC_TDR_0_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // BIC_TDR_1
    str << BIC_TDR_1_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // BIC_TDR_2
    str << BIC_TDR_2_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // BIC_TDR_3
    str << BIC_TDR_3_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // BIC_TDR_4
    str << BIC_TDR_4_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // BIC_TDR_5
    str << BIC_TDR_5_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // BIC_TDR_6
    str << BIC_TDR_6_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // BIC_TDR_7
    str << BIC_TDR_7_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // BIC_TDR_8
    str << BIC_TDR_8_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // BIC_TDR_9
    str << BIC_TDR_9_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // BIC_TDR_10
    str << BIC_TDR_10_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // BIC_TDR_11
    str << BIC_TDR_11_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // BIC_TDR_12
    str << BIC_TDR_12_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // BIC_TDR_13
    str << BIC_TDR_13_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // BIC_TDR_14
    str << BIC_TDR_14_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // BIC_TDR_15
    str << BIC_TDR_15_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-16) ) ) != 0 ) // BIC_TDR_16
    str << BIC_TDR_16_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-17) ) ) != 0 ) // BIC_TDR_17
    str << BIC_TDR_17_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-18) ) ) != 0 ) // BIC_TDR_18
    str << BIC_TDR_18_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-19) ) ) != 0 ) // BIC_TDR_19
    str << BIC_TDR_19_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-20) ) ) != 0 ) // BIC_TDR_20
    str << BIC_TDR_20_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-21) ) ) != 0 ) // BIC_TDR_21
    str << BIC_TDR_21_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-22) ) ) != 0 ) // BIC_TDR_22
    str << BIC_TDR_22_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-23) ) ) != 0 ) // BIC_TDR_23
    str << BIC_TDR_23_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-24) ) ) != 0 ) // GSYNC
    str << GSYNC_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-25) ) ) != 0 ) // DCR_TO_PCB_ERR
    str << DCR_TO_PCB_ERR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-26) ) ) != 0 ) // LOCAL_RING
    str << LOCAL_RING_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-27) ) ) != 0 ) // INT_PARITY_ERROR
    str << INT_PARITY_ERROR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-28) ) ) != 0 ) // INT_MBOX_01_EQ
    str << INT_MBOX_01_EQ_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-29) ) ) != 0 ) // INT_MBOX_01_NE
    str << INT_MBOX_01_NE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-30) ) ) != 0 ) // INT_MBOX_23_EQ
    str << INT_MBOX_23_EQ_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-31) ) ) != 0 ) // INT_MBOX_23_NE
    str << INT_MBOX_23_NE_DESCR + ";";
}

