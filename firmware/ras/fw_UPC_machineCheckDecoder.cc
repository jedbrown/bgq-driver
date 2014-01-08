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
#include <hwi/include/bqc/upc_c_dcr.h>

using namespace std;

#include "_upc_interrupt_state.cc"
#include "utils.h"

extern "C" {

  void fw_UPC_machineCheckDecoder( RasEvent& event, const std::vector<uint64_t>& mbox ) {

    // +----------------------------------------------------------------------+
    // | NOTE:  UPC is memory-mapped (not DCR based) and so there are no DCR  |
    // |        addresses passed to the decoder.  Instead, we assume that the |
    // |        first argument contains the interrupt status and the second   |
    // |        contains the internal error status.                           |
    // +----------------------------------------------------------------------+

    //uint64_t* args = 0; 
    // /*uint16_t  numArgs = */ getArgs( event, args );
    ostringstream details;
    
    if ( mbox.size() >= 2 ) {
	decode_UPC_C_INTERRUPT_STATE( details, mbox[0] );
	decode_INTERNAL_ERROR(details, mbox[1] );
	event.setDetail( "DETAILS", details.str() );
    }
    else {
	event.setDetail("DETAILS", "INTERNAL ERROR: missing details.");
    }
  }
}

