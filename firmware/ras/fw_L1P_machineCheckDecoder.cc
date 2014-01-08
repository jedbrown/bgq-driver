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
#include <hwi/include/bqc/l1p_dcr.h>
#include <hwi/include/bqc/l1p_mmio.h>

using namespace std;

#include "_l1p_interrupt_state.cc"
#include "_l1p_esr.cc"
#include "utils.h"

extern "C" {

  void fw_L1P_machineCheckDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {

    ostringstream details;

    for ( unsigned i = 0; i < mbox.size(); i++ ) {
 
      if ( mbox[i] == L1P_ESR ) {
	  decode_L1P_ESR_STATE( details, mbox[++i], 0 );
      }
      else {

	for ( unsigned core = 0; core < 17; core++ ) {
	
	  TRACE(( "Looking for %X or %X\n", L1P_DCR( core,INTERRUPT_STATE_A__MACHINE_CHECK), L1P_DCR(core,INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH) ));

	  if ( mbox[i] == (uint64_t)L1P_DCR( core,INTERRUPT_STATE_A__MACHINE_CHECK) ) {
	    decode_L1P_INTERRUPT_STATE( details, mbox[++i], core );
	    break;
	  }
	  else if ( mbox[i] == (uint64_t)L1P_DCR(core,INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH) ) {
	      decode_INTERNAL_ERROR_core( details, mbox[++i], core );
	    break;
	  }
	  else if ( mbox[i] == (uint64_t)L1P_ESR_DCR(core) ) {
	      decode_L1P_ESR_STATE( details, mbox[++i], core );
	  }
	}
      }
    }

    event.setDetail( "DETAILS", details.str() );

  }

 void fw_L1P_correctableSummaryDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {

    ostringstream details;
    int i = 0;

    uint64_t count = mbox[i++];
    //uint64_t threshold = mbox[i++];
    uint64_t esr = mbox[i++];
    uint64_t cores = mbox[i++];

    int numcores = 0;
    for ( unsigned j = 0; j < 17; j++ ) {
	if ( ( cores & (1<<j) ) != 0 ) {
	    numcores++;
	}
    }

    details << "count=" << count;
    details << " core";

    if ( numcores > 1 ) {
	details << "s";
    }

    details << "=";

    bool addComma = false;
    for ( unsigned j = 0; j < 17; j++ ) {
	if ( ( cores & (1<<j) ) != 0 ) {
	    if ( addComma )
		details << ",";
	    details << j;
	    addComma = true;
	}
    }

    details << " ";
    decode_L1P_ESR( details, esr );

    event.setDetail( "DETAILS", details.str() );

    ostringstream countStr;
    countStr << count;
    event.setDetail( "BG_COUNT", countStr.str() );
  }

}

