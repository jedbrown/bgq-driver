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
  const std::string           EXCEEDS_EN0_DESCR = " [EXCEEDS_EN0] en0 input reg exceeds maximum threshold";
  const std::string           EXCEEDS_EN1_DESCR = " [EXCEEDS_EN1] en1 input reg exceeds maximum threshold";
  const std::string           EXCEEDS_EN2_DESCR = " [EXCEEDS_EN2] en2 input reg exceeds maximum threshold";
  const std::string           EXCEEDS_EN3_DESCR = " [EXCEEDS_EN3] en3 input reg exceeds maximum threshold";
  const std::string           EXCEEDS_EN4_DESCR = " [EXCEEDS_EN4] en4 input reg exceeds maximum threshold";
  const std::string GENERAL_INTERRUPT_EN5_DESCR = " [GENERAL_INTERRUPT_EN5] en5 general interrupt is true";
  const std::string          LESSTHAN_EN0_DESCR = " [LESSTHAN_EN0] en0 input reg less than minimum threshold";
  const std::string          LESSTHAN_EN1_DESCR = " [LESSTHAN_EN1] en1 input reg less than minimum threshold";
  const std::string          LESSTHAN_EN2_DESCR = " [LESSTHAN_EN2] en2 input reg less than minimum threshold";
  const std::string          LESSTHAN_EN3_DESCR = " [LESSTHAN_EN3] en3 input reg less than minimum threshold";
  const std::string          LESSTHAN_EN4_DESCR = " [LESSTHAN_EN4] en4 input reg less than minimum threshold";
  const std::string     FSM_CHECKSUM_FAIL_DESCR = " [FSM_CHECKSUM_FAIL] FSM checksum fail";
  const std::string          FSM_SEQ_FAIL_DESCR = " [FSM_SEQ_FAIL] FSM bad sequence detected";
  const std::string   TVSENSE_EXCEEDS_MAX_DESCR = " [TVSENSE_EXCEEDS_MAX] trigger for tvsense exceeded maximum detected ";
  const std::string  TVSENSE_LESSTHAN_MIN_DESCR = " [TVSENSE_LESSTHAN_MIN] trigger for tvsense less than minimum detected ";

void decode_ENVMON_INTERRUPT_STATE( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "ENVMON_INTERRUPT_STATE error status: ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // LOCAL_RING
    str << LOCAL_RING_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // EXCEEDS_EN0
    str << EXCEEDS_EN0_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // EXCEEDS_EN1
    str << EXCEEDS_EN1_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // EXCEEDS_EN2
    str << EXCEEDS_EN2_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // EXCEEDS_EN3
    str << EXCEEDS_EN3_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // EXCEEDS_EN4
    str << EXCEEDS_EN4_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // GENERAL_INTERRUPT_EN5
    str << GENERAL_INTERRUPT_EN5_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-7) ) ) != 0 ) // LESSTHAN_EN0
    str << LESSTHAN_EN0_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-8) ) ) != 0 ) // LESSTHAN_EN1
    str << LESSTHAN_EN1_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-9) ) ) != 0 ) // LESSTHAN_EN2
    str << LESSTHAN_EN2_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-10) ) ) != 0 ) // LESSTHAN_EN3
    str << LESSTHAN_EN3_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-11) ) ) != 0 ) // LESSTHAN_EN4
    str << LESSTHAN_EN4_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-12) ) ) != 0 ) // FSM_CHECKSUM_FAIL
    str << FSM_CHECKSUM_FAIL_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-13) ) ) != 0 ) // FSM_SEQ_FAIL
    str << FSM_SEQ_FAIL_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-14) ) ) != 0 ) // TVSENSE_EXCEEDS_MAX
    str << TVSENSE_EXCEEDS_MAX_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-15) ) ) != 0 ) // TVSENSE_LESSTHAN_MIN
    str << TVSENSE_LESSTHAN_MIN_DESCR + ";";
}

