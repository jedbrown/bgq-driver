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
#include <hwi/include/bqc/l2_central_dcr.h>

using namespace std;

#include "_l2_central_interrupt_state.cc"
#include "utils.h"

extern "C" {

  Decoder L2C_DECODERS[] = {
    { L2_CENTRAL_DCR( L2_INTERRUPT_STATE__MACHINE_CHECK ),        decode_L2C_INTERRUPT_STATE },
    { L2_CENTRAL_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ),  decode_INTERNAL_ERROR },
  };

  void fw_L2C_machineCheckDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {
    walkDetails( L2C_DECODERS, sizeof(L2C_DECODERS)/sizeof(L2C_DECODERS[0]), mbox, event, "DETAILS" );
  }


}

