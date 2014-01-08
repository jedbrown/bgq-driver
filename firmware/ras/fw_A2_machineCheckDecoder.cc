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

#include <sstream>
#include <iostream>
#include <vector>
#include <ras/include/RasEvent.h>

using namespace std;

#include "_mcsr.cc"

extern "C" {

  void fw_A2_machineCheckDecoder(RasEvent& event, const vector<uint64_t>& mbox ) {

    ostringstream details;
    
    if ( mbox.size() < 2 ) {
	details << "INTERNAL ERROR: missing details.";
    }
    else {
	decode_A2_MCSR( details, mbox[0], mbox[1] );
    }
    event.setDetail( "DETAILS", details.str() );
  }

  void fw_A2_tlbParityErrorDecoder(RasEvent& event, const vector<uint64_t>& mbox ) {

    ostringstream details;
    
    if ( mbox.size() < 2 ) {
	details << "INTERNAL ERROR: missing details.";
    }
    else {
	decode_A2_MCSR( details, mbox[1], -1 );
    }
    event.setDetail( "MCSR_DETAILS", details.str() );
  }

}

