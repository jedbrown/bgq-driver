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
  const std::string A_PLLA_LOCK_LOST_DESCR = "(a_plla_lock_lost) Dimension A HSS PLLA lost lock. ";
  const std::string B_PLLA_LOCK_LOST_DESCR = "(b_plla_lock_lost) Dimension B HSS PLLA lost lock. ";
  const std::string C_PLLA_LOCK_LOST_DESCR = "(c_plla_lock_lost) Dimension C HSS PLLA lost lock. ";

void decode_serdes_left_interrupt_state( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "SERDES_LEFT_INTERRUPT_STATE error status: ";


  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // a_plla_lock_lost
    str << A_PLLA_LOCK_LOST_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // b_plla_lock_lost
    str << B_PLLA_LOCK_LOST_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // c_plla_lock_lost
    str << C_PLLA_LOCK_LOST_DESCR;
}


  // Detailed summaries of specific error bits:
  const std::string D_PLLA_LOCK_LOST_DESCR = "(d_plla_lock_lost) Dimension D HSS PLLA lost lock. ";
  const std::string E_PLLB_LOCK_LOST_DESCR = "(e_pllb_lock_lost) Dimension E HSS PLLB lost lock. ";
  const std::string IO_PLLA_LOCK_LOST_DESCR = "(io_plla_lock_lost) Dimension IO HSS PLLA lost lock. ";

void decode_serdes_right_interrupt_state( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "SERDES_RIGHT_INTERRUPT_STATE error status: ";


  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // d_plla_lock_lost
    str << D_PLLA_LOCK_LOST_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // e_pllb_lock_lost
    str << E_PLLB_LOCK_LOST_DESCR;

  if ( ( status & ((uint64_t)1 << (63-0-4) ) ) != 0 ) // io_plla_lock_lost
    str << IO_PLLA_LOCK_LOST_DESCR;
}

