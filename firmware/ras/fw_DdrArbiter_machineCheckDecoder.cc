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
#include <hwi/include/bqc/dr_arb_dcr.h>
#include <hwi/include/bqc/ddr.h>

using namespace std;

#include "_ddr_arbiter_interrupt_status.cc"
#include "_mcfir.cc"
#include "utils.h"

extern "C" {

  Decoder DDR_ARB_DECODERS[] = {
    { DR_ARB_DCR__A( L2_INTERRUPT_STATE__MACHINE_CHECK ),        decode_DR_ARB_L2_INTERRUPT_STATE },
    { DR_ARB_DCR__A( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ),  decode_INTERNAL_ERROR },
    { DR_ARB_DCR__B( L2_INTERRUPT_STATE__MACHINE_CHECK ),        decode_DR_ARB_L2_INTERRUPT_STATE },
    { DR_ARB_DCR__B( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ),  decode_INTERNAL_ERROR },
    { _DDR_MC_MCFIR(0),                                          decode_MCFIR },
    { _DDR_MC_MCFIR(1),                                          decode_MCFIR },
  };

  void fw_DdrArbiter_machineCheckDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {
    walkDetails( DDR_ARB_DECODERS, sizeof(DDR_ARB_DECODERS)/sizeof(DDR_ARB_DECODERS[0]), mbox, event, "DETAILS" );
  }

    void fw_DdrArbiter_correctableSummaryDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {

	if ( mbox.size() < 2 ) {
	    event.setDetail( "DETAILS", "INTERNAL ERROR: details missing.");
	    return;
	}

	ostringstream details;

	uint64_t count = mbox[0];
	uint64_t error_bits = mbox[1];

	details << "count=" << count;

	details << " ";
	//decode_DR_ARB_L2_INTERRUPT_STATE( details, error_bits );
	decode_MCFIR( details, error_bits );

	if ( mbox.size() > 2 ) {
	    details << " threshold=" << mbox[2];
	}
    
	event.setDetail( "DETAILS", details.str() );


	ostringstream countStr;
	countStr << count;
	event.setDetail( "BG_COUNT", countStr.str() );
    }

}

