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
#include <hwi/include/bqc/nd_500_dcr.h>
#include <hwi/include/bqc/nd_rese_dcr.h>
#include <hwi/include/bqc/nd_x2_dcr.h>

using namespace std;

#include "_nd_decoder.cc"
#include "utils.h"

extern "C" {

  Decoder ND_DECODERS[] = {

   {  ND_500_DCR(FATAL_ERR0), decode_ND_500_DCR_FATAL_ERR0 },
   {  ND_500_DCR(FATAL_ERR1), decode_ND_500_DCR_FATAL_ERR1 },
   {  ND_500_DCR(FATAL_ERR2), decode_ND_500_DCR_FATAL_ERR2 },
   {  ND_500_DCR(FATAL_ERR3), decode_ND_500_DCR_FATAL_ERR3 },
   {  ND_500_DCR(FATAL_ERR4), decode_ND_500_DCR_FATAL_ERR4 },
   {  ND_500_DCR(FATAL_ERR5), decode_ND_500_DCR_FATAL_ERR5 },
   {  ND_500_DCR(INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ), decode_INTERNAL_ERROR },
   {  ND_X2_DCR(FATAL_ERR0),  decode_ND_X2_DCR_FATAL_ERR0 },
   {  ND_X2_DCR(FATAL_ERR1),  decode_ND_X2_DCR_FATAL_ERR1 },
   {  ND_X2_DCR(INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK ), decode_INTERNAL_ERROR },

  };

  void fw_ND_machineCheckDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {

    //walkDetails( , , mbox, event, "DETAILS" );

    ostringstream details;

    // The argument list is actually a list of pairs (DCR address, DCR value).
    // Locate any known DCRs and decode themm

    int unit = -1;

    for ( unsigned i = 0; i < mbox.size(); i++ ) {

      TRACE(("[%s:%d] arg[%d]=%lX\n", __func__, __LINE__, i, mbox[i]));
      bool found = false;

      for ( unsigned j = 0; (j < sizeof(ND_DECODERS)/sizeof(ND_DECODERS[0])) && ( !found); j++ ) {

	TRACE(("[%s:%d] comparing arg=%lX to %lx=decode\n", __func__, __LINE__, mbox[i], ND_DECODERS[j].dcrAddress));

	if ( mbox[i] == ND_DECODERS[j].dcrAddress ) {
	  ND_DECODERS[j].decoder( details, mbox[++i] );
	  found = true;
	}
      }

      if ( ! found ) {
	
	if ( ( mbox[i] >= ND_500_DCR(CE_CNT) ) && ( mbox[i] <= ND_500_DCR(CE_CNT)+8 ) ) {
	  details << " CE_CNT[" << ( mbox[i] - ND_500_DCR(CE_CNT) ) << "]=" << mbox[i+1] << " ";
	  i++;
	  found = true;
	}
	else {
	  for ( unsigned j = 0; (j < ND_RESE_DCR_num) && ( ! found ); j++ ) {

	    if (mbox[i] == ND_RESE_DCR(j, FATAL_ERR)) {
		decode_ND_RESE_DCR_FATAL_ERR( details, mbox[++i], j );
		found = true;
		unit = j;
	    }
	    else if ( mbox[i] == ND_RESE_DCR(j, RE_LINK_ERR_CNT) ) {
	      details << "Receiver Link Error Count:" << mbox[++i] << " ";
	      found = true;
	    }
	    else if ( mbox[i] == ND_RESE_DCR(j, SE_RETRANS_CNT) ) {
	      details << "Sender Retransmission Count:" << mbox[++i] << " ";
	      found = true;
	    }
	    else if ( ( mbox[i] >= ND_RESE_DCR(j, CE_COUNT)+0 ) && ( mbox[i] <= ND_RESE_DCR(j, CE_COUNT)+8 ) ) {
	      details << "Correctable Error Count:" << mbox[++i] << " ";
	      found = true;
	    }
	    else if ( mbox[i] == ND_RESE_DCR(j, RESE_INTERRUPTS__MACHINE_CHECK) ) {
		decode_ND_RESE_RESE_INTERRUPTS( details, mbox[++i], j );
		found = true;
	    }
	  }
	}
      }

      if ( ! found ) {
	details << "???[" << mbox[i] << ":" << i << "] ";
      }
    }

    event.setDetail( "DETAILS", details.str() );


    event.setDetail( "DIM_DIR", ( unit != -1 ) ? nd_rese_unit2Dimension( (unsigned int) unit ) : "NA" );
  }

    const std::string CE_COUNT_Prologs[] = {
	"Receiver VC0 RF2 array CE count ",
	"Receiver VC1 RF2 array CE count ",
	"Receiver VC2 RF2 array CE count ",
	"Receiver VC3 RF2 array CE count ",
	"Receiver VC4 RF2 array CE count ",
	"Receiver VC5 RF2 array CE count ",
	"Receiver VC0 list CE count ",
	"Receiver bypass logic CE count ",
	"Sender retransmission FIFO RF2 array CE count "
    };

  void fw_ND_correctableDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {

      if ( mbox.size() < 3 ) {
	  event.setDetail( "DETAILS", "INTERNAL ERROR: details missing");
	  return;
      }

      ostringstream details;

      uint64_t unit  = mbox[0];
      uint64_t count = mbox[1];
      uint64_t dimswap = mbox[2];

      event.setDetail( "LINK", nd_rese_unit2Dimension( (unsigned int) unit ) );

      for ( unsigned i = 3; i < mbox.size(); i++ ) {

	  uint64_t dcr = mbox[i]; 

	  if ( dcr == ND_RESE_DCR(unit,FATAL_ERR) ) {
	      decode_ND_RESE_DCR_FATAL_ERR( details, mbox[++i], unit );
	  }
	  else if ( ( dcr >= ND_RESE_DCR(unit, CE_COUNT) ) && ( dcr < ( ND_RESE_DCR(unit,CE_COUNT) + ND_RESE_DCR__CE_COUNT_range ) ) ) {
	      int offset = dcr - ND_RESE_DCR(unit, CE_COUNT);
	      details << CE_COUNT_Prologs[offset];
	  }
	  else {
	      details << "??? ";
	  }
      }

      event.setDetail( "DETAILS", details.str() );
      event.setDetail( "DIM_DIR", nd_rese_unit2Dimension( (unsigned int) unit ) );

      ostringstream swapStr;
      swapStr << dimswap;
      event.setDetail( "DIM_SWAP", swapStr.str() );

      ostringstream countStr;
      countStr << count;
      event.setDetail( "BG_COUNT", countStr.str() );
  }

}

