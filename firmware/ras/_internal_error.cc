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
  const std::string        BAD_ADDRESS_DESCR = " [BAD_ADDRESS] There was an error decoding the address";
  const std::string           BAD_PRIV_DESCR = " [BAD_PRIV] There was an error with the access level";
  const std::string    BAD_DATA_PARITY_DESCR = " [BAD_DATA_PARITY] There was an error with the data parity";
  const std::string BAD_ADDRESS_PARITY_DESCR = " [BAD_ADDRESS_PARITY] There was an error with the address parity";

void decode_INTERNAL_ERROR( ostringstream& str, uint64_t status ) {

  if ( status == 0 ) // If status is clear, there is nothing to do.
    return;

  str << "INTERNAL_ERROR error status: ";


  if ( ( status & ((uint64_t)1 << (63-0-0) ) ) != 0 ) // BAD_ADDRESS
    str << BAD_ADDRESS_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-1) ) ) != 0 ) // BAD_PRIV
    str << BAD_PRIV_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-2) ) ) != 0 ) // BAD_DATA_PARITY
    str << BAD_DATA_PARITY_DESCR + ";";

  if ( ( status & ((uint64_t)1 << (63-0-3) ) ) != 0 ) // BAD_ADDRESS_PARITY
    str << BAD_ADDRESS_PARITY_DESCR + ";";
}

