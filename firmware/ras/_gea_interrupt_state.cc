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
  const std::string DEVBUS_CTL_PERR_DESCR = " [DEVBUS_CTL_PERR] DevBus control parity error ";
  const std::string      TIMER0_INT_DESCR = " [TIMER0_INT] Global Timer 0 Interrupt";
  const std::string      TIMER1_INT_DESCR = " [TIMER1_INT] Global Timer 1 Interrupt";
  const std::string      TIMER2_INT_DESCR = " [TIMER2_INT] Global Timer 2 Interrupt";
  const std::string      TIMER3_INT_DESCR = " [TIMER3_INT] Global Timer 3 Interrupt";

void decode_GEA_INTERRUPT_STATE( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "GEA_INTERRUPT_STATE error status: ";


  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // DEVBUS_CTL_PERR
    str << DEVBUS_CTL_PERR_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // TIMER0_INT
    str << TIMER0_INT_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // TIMER1_INT
    str << TIMER1_INT_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // TIMER2_INT
    str << TIMER2_INT_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-5) ) ) != 0 ) // TIMER3_INT
    str << TIMER3_INT_DESCR + ";";
}

