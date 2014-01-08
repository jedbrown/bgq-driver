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

void decode_L1P_INTERRUPT_STATE( ostringstream& str, uint64_t status, unsigned core ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "L1P_INTERRUPT_STATE(" << core << ") : ";


  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // LOCAL_RING
    str << LOCAL_RING_DESCR + ";";
}

