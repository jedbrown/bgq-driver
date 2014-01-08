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
#include <hwi/include/bqc/l2_counter_dcr.h>

using namespace std;

#include "_l2_counter_interrupt_state.cc"
#include "utils.h"

extern "C" {

  void fw_L2Ctr_machineCheckDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {

    ostringstream details;

    for ( unsigned i = 0; i < mbox.size(); i++ ) {
      for ( unsigned unit = 0; unit < L2_COUNTER_DCR_num; unit++ ) {
	TRACE(( "decode %X or %X\n", L2_COUNTER_DCR( unit, L2_INTERRUPT_STATE__MACHINE_CHECK ), L2_COUNTER_DCR( unit, INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ) ));
	if ( mbox[i] == (uint64_t)L2_COUNTER_DCR( unit, L2_INTERRUPT_STATE__MACHINE_CHECK ) ) {
	  details << " Unit " << unit << " : ";
	  decode_L2CTR_INTERRUPT_STATE( details, mbox[++i] );
	  break;
	}
	else if ( mbox[i] == (uint64_t)L2_COUNTER_DCR( unit, INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ) ) {
	  details << " Unit " << unit << " : ";
	  decode_INTERNAL_ERROR( details, mbox[++i] );
	  break;
	}
      }
    }

    event.setDetail( "DETAILS", details.str() );
  }


}

