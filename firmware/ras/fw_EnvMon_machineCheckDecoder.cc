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
#include <hwi/include/bqc/en_dcr.h>

using namespace std;

#include "_envmon_interrupt_status.cc"
#include "utils.h"

extern "C" {

  Decoder ENVMON_DECODERS[] = {
    { EN_DCR( ENVMON_INTERRUPT_STATE__MACHINE_CHECK ),    decode_ENVMON_INTERRUPT_STATE },
    { EN_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ),  decode_INTERNAL_ERROR },
  };

  void fw_EnvMon_machineCheckDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {
    walkDetails( ENVMON_DECODERS, sizeof(ENVMON_DECODERS)/sizeof(ENVMON_DECODERS[0]), mbox, event, "DETAILS" );
  }


}

