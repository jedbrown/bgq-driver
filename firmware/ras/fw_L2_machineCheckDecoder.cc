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
#include <hwi/include/bqc/l2_dcr.h>

using namespace std;


#include "_l2_interrupt_state.cc"
#include "utils.h"

extern "C" {

  void fw_L2_machineCheckDecoder(RasEvent& event, const vector<uint64_t>& mbox ) {

    ostringstream details;
    
    for ( unsigned i = 0; i < mbox.size(); i++ ) {
      for ( unsigned slice = 0; slice < L2_DCR_num; slice++ ) {
	
	TRACE(( "Looking for %X or %X\n", L2_DCR( slice, L2_INTERRUPT_STATE__MACHINE_CHECK ), L2_DCR( slice, INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ) ) );

	if ( mbox[i] == (uint64_t)L2_DCR( slice, L2_INTERRUPT_STATE__MACHINE_CHECK ) ) {
	  details << " L2 slice " << slice << " : ";
	  decode_L2_INTERRUPT_STATE( details, mbox[++i] );
	  break;
	}
	else if ( mbox[i] == (uint64_t)L2_DCR( slice, INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ) ) {
	  details << " L2 slice " << slice << " : ";
	  decode_INTERNAL_ERROR( details, mbox[++i] );
	  break;
	}
      }
    }

    event.setDetail( "DETAILS", details.str() );
  }

 void fw_L2_correctableSummaryDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {

    ostringstream details;
    int i = 0;

    uint64_t count = mbox[i++];
    uint64_t error_bits = mbox[i++];
    uint64_t slices = mbox[i++];


    details << "count=" << count;

    details << " slices=";
    bool addComma = false;
    for ( unsigned j = 0; j < 17; j++ ) {
	if ( ( slices & (1<<j) ) != 0 ) {
	    if ( addComma )
		details << ",";
	    details << j;
	    addComma = true;
	}
    }

    details << " ";
    decode_L2_INTERRUPT_STATE( details, error_bits );

    event.setDetail( "DETAILS", details.str() );


    ostringstream countStr;
    countStr << count;
    event.setDetail( "BG_COUNT", countStr.str() );
  }

}

