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
#include <hwi/include/bqc/serdes_left_dcr.h>
#include <hwi/include/bqc/serdes_right_dcr.h>

using namespace std;

#include "_serdes_interrupt_state.cc"
#include "utils.h"

extern "C" {

  Decoder SERDES_DECODERS[] = {
    { SERDES_LEFT_DCR ( SERDES_INTERRUPT_STATE__MACHINE_CHECK   ),  decode_serdes_left_interrupt_state },
    { SERDES_RIGHT_DCR( SERDES_INTERRUPT_STATE__MACHINE_CHECK   ),  decode_serdes_right_interrupt_state },
    { SERDES_LEFT_DCR ( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ), decode_INTERNAL_ERROR },
    { SERDES_RIGHT_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ), decode_INTERNAL_ERROR },
  };

  void fw_Serdes_machineCheckDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {
    walkDetails( SERDES_DECODERS, sizeof(SERDES_DECODERS)/sizeof(SERDES_DECODERS[0]), mbox, event, "DETAILS" );
  }


}

