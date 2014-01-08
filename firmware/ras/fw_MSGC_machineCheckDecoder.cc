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
#include <hwi/include/bqc/ms_genct_dcr.h>

using namespace std;

#include "_msgc_interrupt_state.cc"
#include "utils.h"

extern "C" {

  Decoder MSGC_DECODERS[] = {
    { MS_GENCT_DCR( L2_INTERRUPT_STATE__MACHINE_CHECK ),        decode_MSGC_INTERRUPT_STATE },
    { MS_GENCT_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ),  decode_INTERNAL_ERROR },
  };

  void fw_MSGC_machineCheckDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {
    walkDetails( MSGC_DECODERS, sizeof(MSGC_DECODERS)/sizeof(MSGC_DECODERS[0]), mbox, event, "DETAILS" );
  }


}

