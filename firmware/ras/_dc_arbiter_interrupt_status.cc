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
  const std::string    INV_PTY_STATE_DESCR = " [INV_PTY_STATE] Invalid parity state machine value ";
  const std::string    INV_MON_STATE_DESCR = " [INV_MON_STATE] Invalid monitor state machine value ";
  const std::string    INV_ARB_STATE_DESCR = " [INV_ARB_STATE] Invalid arbiter state machine value ";
  const std::string   RING_NOT_CLEAN_DESCR = " [RING_NOT_CLEAN] Time out: a pending request or acknowlege still active at the end of an operation";
  const std::string NO_REQ_AFTER_ACK_DESCR = " [NO_REQ_AFTER_ACK] Time out: request not removed after an acknowlege";
  const std::string NO_ACK_AFTER_REQ_DESCR = " [NO_ACK_AFTER_REQ] Time out: no acknowlege after a request";
  const std::string NO_REQ_AFTER_GRT_DESCR = " [NO_REQ_AFTER_GRT] Time out: no request after a grant";
  const std::string         DATA_ERR_DESCR = " [DATA_ERR] Data parity error";
  const std::string          ADD_ERR_DESCR = " [ADD_ERR] Address parity error";

void decode_INT_REG( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "INT_REG error status: ";


  if ( ( status & ((uint64_t)1 << (63-0-53) ) ) != 0 ) // INV_PTY_STATE
    str << INV_PTY_STATE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-54) ) ) != 0 ) // INV_MON_STATE
    str << INV_MON_STATE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-55) ) ) != 0 ) // INV_ARB_STATE
    str << INV_ARB_STATE_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-58) ) ) != 0 ) // RING_NOT_CLEAN
    str << RING_NOT_CLEAN_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-59) ) ) != 0 ) // NO_REQ_AFTER_ACK
    str << NO_REQ_AFTER_ACK_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-60) ) ) != 0 ) // NO_ACK_AFTER_REQ
    str << NO_ACK_AFTER_REQ_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-61) ) ) != 0 ) // NO_REQ_AFTER_GRT
    str << NO_REQ_AFTER_GRT_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-62) ) ) != 0 ) // DATA_ERR
    str << DATA_ERR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-63) ) ) != 0 ) // ADD_ERR
    str << ADD_ERR_DESCR + ";";
}

