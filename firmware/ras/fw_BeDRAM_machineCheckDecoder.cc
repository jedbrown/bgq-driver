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
#include <hwi/include/bqc/bedram_dcr.h>

using namespace std;

#include "_bedram_interrupt_status.cc"
#include "utils.h"

extern "C" {

  Decoder BEDRAM_DECODERS[] = {
    { BEDRAM_DCR( BEDRAM_INTERRUPT_STATUS__MACHINE_CHECK ),  decode_BEDRAM_INTERRUPT_STATUS },
    { BEDRAM_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ), decode_INTERNAL_ERROR },
  };

  void fw_BeDRAM_machineCheckDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {
    walkDetails( BEDRAM_DECODERS, sizeof(BEDRAM_DECODERS)/sizeof(BEDRAM_DECODERS[0]), mbox, event, "DETAILS" );
  }


}

