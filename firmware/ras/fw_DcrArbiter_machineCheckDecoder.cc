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
#include <hwi/include/bqc/dc_arbiter_dcr.h>

using namespace std;

#include "_dc_arbiter_interrupt_status.cc"
#include "utils.h"

extern "C" {

  Decoder DC_ARBITER_DECODERS[] = {
    { DC_ARBITER_DCR( INT_REG__MACHINE_CHECK ),                   decode_INT_REG },
    { DC_ARBITER_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ),  decode_INTERNAL_ERROR },
  };

  void fw_DcrArbiter_machineCheckDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {
    walkDetails( DC_ARBITER_DECODERS, sizeof(DC_ARBITER_DECODERS)/sizeof(DC_ARBITER_DECODERS[0]), mbox, event, "DETAILS" );
  }

  void fw_DcrArbiter_badDcrAccessDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {

      ostringstream details;

      for ( unsigned i = 0; i < mbox.size(); i++ ) {
	  if ( mbox[i] == DC_ARBITER_DCR(STATUS_REG) ) {
	      details << "DCR address: 0x" << hex << DC_ARBITER_DCR__STATUS_REG__RING_ADD_get(mbox[++i]) ;
	  }
      }
      event.setDetail( "DETAILS", details.str() );
  }

}

