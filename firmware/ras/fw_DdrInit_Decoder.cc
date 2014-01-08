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

	if ( mbox.size() == 0 ) {
	    details << "INTERNAL ERROR: details missing.";
	}
	else {


	    switch (mbox[0]) {

	    case FW_RAS_DDR_INIT_VTT_DD : {
		details << "Strong address driver used to fix VTT failure (Density Detection).  Retries = " << mbox[1];
		break;
	    }

	    case FW_RAS_DDR_INIT_VTT_SB : {
		uint64_t retries = mbox[1];
		uint64_t ecc = mbox[2];
		uint64_t imp1 = mbox[3];
		uint64_t imp2 = mbox[4];
		uint64_t imp3 = mbox[5];
		uint64_t rtt = mbox[6];
		details 
		    << "Strong address driver used to fix VTT failure (Small Block Test)"
		    << " Retries=" << retries
		    << " ECC " << ( ecc ? "enabled" : "disabled" )
		    << " Impedence=0x" << imp1 << imp2 << imp3
		    << " Rtt=" << rtt
		    ;
		break;
	    }

	    case FW_RAS_DDR_INIT_RANK_DETECTION_FAILED : {
		details << "DDR rank detection failed.";
		break;
	    }

	    case FW_RAS_DDR_INIT_RANK_DETECTION_RETRIED : {
		details << "DDR rank detection succeeded after " << mbox[1] << " retries.";
		break;
	    }

	    case FW_RAS_DDR_INIT_IOM_CALIBRATION_FAILED : {
		uint64_t retries = mbox[1];
		uint64_t rdcal = mbox[2];
		uint64_t rddly = mbox[3];
		uint64_t wrlvl = mbox[4];
		uint64_t cpc = mbox[5];
		details 
		    << "DDR IOM calibration failed after " << retries << " retries.  Using redundant chip."
		    << " RdCal=" << rdcal 
		    << " RdDly=" << rddly
		    << " WrLvl=" << wrlvl
		    << " CPC=" << cpc
		    ;
		break;
	    }

	    case FW_RAS_DDR_INIT_IOM_CALIBRATION_RETRIED : {
		uint64_t retries = mbox[1];
		details 
		    << "DDR IOM calibration succeeded after " << retries << " retries."
		    ;
		break;
	    }

	    case FW_RAS_DDR_INIT_DENSITY_DETECTION_FAILED : {
		details << "DDR density detection failed.";
		break;
	    }

	    case FW_RAS_DDR_INIT_SMALL_BLOCK_TEST_FAILED : {
		details << "DDR small block test failed 0x" << hex << mbox[1] << "-0x" << hex << mbox[2];
		break;
	    }

	    case FW_RAS_DDR_INIT_NARROW_WRITE_DATA_WINDOW : {
		uint64_t mc = mbox[1];
		uint64_t window_max = mbox[2];
		uint64_t vref_max = mbox[3];
		details 
		    << "MC" << mc << " has narrow write data window:"
		    << " window_max=" << window_max
		    << " vref_max=" << vref_max
		    ;
		break;
	    }

	    case FW_RAS_DDR_INIT_NARROW_ADDRESS_WINDOW : {
		uint64_t mc = mbox[1];
		uint64_t window_max = mbox[2];
		details 
		    << "MC" << mc << " has narrow address window:"
		    << " window_max=" << window_max
		    ;
		break;
	    }

	    }
	}

	event.setDetail( "DETAILS", details.str() );
    
    }

}
