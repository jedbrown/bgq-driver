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
  const std::string LOCAL_RING_DESCR = " [LOCAL_RING] Parity error on DCR ring";
  const std::string     MC_ERR_DESCR = " [MC_ERR] MC SCOM error";
  const std::string    MCS_ERR_DESCR = " [MCS_ERR] MCS SCOM error";
  const std::string    IOM_ERR_DESCR = " [IOM_ERR] IOM SCOM error";
  const std::string      XSTOP_DESCR = " [XSTOP] MC check stop";
  const std::string  SPEC_ATTN_DESCR = " [SPEC_ATTN] MC special attention";
  const std::string  RECOV_ERR_DESCR = " [6] Memory controller error";

void decode_DR_ARB_L2_INTERRUPT_STATE( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "DR_ARB_L2_INTERRUPT_STATE : ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // LOCAL_RING
    str << LOCAL_RING_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // MC_ERR
    str << MC_ERR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // MCS_ERR
    str << MCS_ERR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // IOM_ERR
    str << IOM_ERR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // XSTOP
    str << XSTOP_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // SPEC_ATTN
    str << SPEC_ATTN_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-6) ) ) != 0 ) // RECOV_ERR
    str << RECOV_ERR_DESCR + ";";
}

