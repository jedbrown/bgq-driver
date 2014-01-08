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
#include <hwi/include/bqc/dr_arb_dcr.h>
#include <hwi/include/bqc/ddr.h>
#include <firmware/include/personality.h>
#include "../src/Firmware_RAS.h"

using namespace std;

extern "C" {

    void fw_DdrInit_Decoder( RasEvent& event, const vector<uint64_t>& mbox ) {

	ostringstream details;

	switch (mbox[0]) {

	case FW_RAS_DDR_INIT_VTT_DD : {
	    details << "Strong address driver used to fix VTT failure (Density Detection)";
	    break;
	}

	case FW_RAS_DDR_INIT_VTT_SB : {
	    details << "Strong address driver used to fix VTT failure (Small Block Test)";
	    break;
	}

	case FW_RAS_DDR_INIT_RANK_DETECTION_FAILED : {
	    details << "DDR rank detection failed.  Proceeding with one rank.";
	    break;
	}

	case FW_RAS_DDR_INIT_RANK_DETECTION_RETRIED : {
	    details << "DDR rank detection succeeded after " << mbox[1] << " retries.";
	    break;
	}

	case FW_RAS_DDR_INIT_IOM_CALIBRATION_FAILED : {
	    details << "DDR IOM calibration failed after " << mbox[1] << " retries.";
	    break;
	}

	case FW_RAS_DDR_INIT_IOM_CALIBRATION_RETRIED : {
	    details << "DDR IOM calibration succeeded after " << mbox[1] << " retries.";
	    break;
	}

	case FW_RAS_DDR_INIT_DENSITY_DETECTION_FAILED : {
	    details << "DDR density detection failed.  Proceeding with 1GB of memory.";
	    break;
	}

	case FW_RAS_DDR_INIT_SMALL_BLOCK_TEST_FAILED : {
	    details << "DDR small block test failed 0x" << hex << mbox[1] << "-0x" << hex << mbox[2];
	    break;
	}
	}

	event.setDetail( "DETAILS", details.str() );
    
    }

}
