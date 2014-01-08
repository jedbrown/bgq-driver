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
#include <firmware/include/personality.h>
#include "../src/Firmware_RAS.h"

#define __KERNEL__
#include <hwi/include/bqc/A2_core.h>

using namespace std;

enum {
    INT_CATEGORY_CRITICAL = 1,
    INT_CATEGORY_STANDARD = 2,
    INT_CATEGORY_MACHINE_CHECK = 3
};

const std::string _ESR_PIL = "Illegal Instruction Exception";
const std::string _ESR_PPR = "Privileged Instruction Exception";
const std::string _ESR_PTR = "Trap Exception";
const std::string _ESR_FP  = "Floating Point Operation";
const std::string _ESR_ST  = "Store Operation"; // 40
const std::string _ESR_DLK0 = "Data Locking Exception 0";
const std::string _ESR_DLK1 = "Data Locking Exception 1";
const std::string _ESR_AP   = "Auxiliary Processor Operation";
const std::string _ESR_PUO  = "Unimplemented Operation Exception";
const std::string _ESR_BO   = "Byte Ordering Exception";
const std::string _ESR_PIE  = "Imprecise Exception";
const std::string _ESR_UCT  = "Unavailable Coprocessor Type";
const std::string _ESR_DATA = "Data Access";
const std::string _ESR_TLBI = "TLB Inelligible";
const std::string _ESR_PT   = "Page Table";
const std::string _ESR_SPV  = "Vector Operation";
const std::string _ESR_EPID = "External Process ID Operation";

    
extern "C" {

    static uint64_t addInDcr( const char* label, const uint64_t sprn, const vector<uint64_t>& mbox, ostringstream& details ) {

	// Start at 1 since the first argument is always the crash code:
	for ( unsigned int i = 1; i < mbox.size(); i+=2 ) {
	    if ( mbox[i] == sprn ) {
		details << " " << label << "=" << hex << mbox[++i];
		return mbox[i];
	    }
	}
	return 0;
    }

    static void decodeEsr( const uint64_t esr, ostringstream& details ) {

	details << " [";

	if ( ( esr & (1ull << (63-36) ) ) != 0 )
	    details << _ESR_PIL << ",";


	if ( ( esr & (1ull << (63-37) ) ) != 0 )
	    details << _ESR_PPR << ",";

	if ( ( esr & (1ull << (63-38) ) ) != 0 )
	    details << _ESR_PTR << ",";

	if ( ( esr & (1ull << (63-39) ) ) != 0 )
	    details << _ESR_FP << ",";

	if ( ( esr & (1ull << (63-40) ) ) != 0 )
	    details << _ESR_ST << ",";

	if ( ( esr & (1ull << (63-42) ) ) != 0 )
	    details << _ESR_DLK0 << ",";

	if ( ( esr & (1ull << (63-43) ) ) != 0 )
	    details << _ESR_DLK1 << ",";

	if ( ( esr & (1ull << (63-44) ) ) != 0 )
	    details << _ESR_AP << ",";

	if ( ( esr & (1ull << (63-45) ) ) != 0 )
	    details << _ESR_PUO << ",";

	if ( ( esr & (1ull << (63-46) ) ) != 0 )
	    details << _ESR_BO << ",";

	if ( ( esr & (1ull << (63-47) ) ) != 0 )
	    details << _ESR_PIE << ",";

	if ( ( esr & (1ull << (63-49) ) ) != 0 )
	    details << _ESR_UCT << ",";

	if ( ( esr & (1ull << (63-53) ) ) != 0 )
	    details << _ESR_DATA << ",";

	if ( ( esr & (1ull << (63-54) ) ) != 0 )
	    details << _ESR_TLBI << ",";

	if ( ( esr & (1ull << (63-55) ) ) != 0 )
	    details << _ESR_PT << ",";

	if ( ( esr & (1ull << (63-56) ) ) != 0 )
	    details << _ESR_SPV << ",";

	if ( ( esr & (1ull << (63-57) ) ) != 0 )
	    details << _ESR_EPID << ",";


	details << "]";
    }

    void fw_UnexpectedInterrupt_Decoder( RasEvent& event, const vector<uint64_t>& mbox ) {

	if ( mbox.size() == 0 ) {
	    event.setDetail( "DETAILS", "INTERNAL ERROR: syndrome details are missing." );
	    return;
	}

	ostringstream details;

	int category = INT_CATEGORY_STANDARD;

	bool addDear = false;
	bool addEsr = false;

	switch (mbox[0]) {

	case IVO_MCHK    : {
	    details << "Machine Check"; 
	    category = INT_CATEGORY_MACHINE_CHECK;
	    break;
	}

	case IVO_CI      : {
	    details << "Critical Input"; 
	    category = INT_CATEGORY_CRITICAL;
	    break;
	}

	case IVO_DEBUG   : {
	    details << "Debug"; 
	    category = INT_CATEGORY_CRITICAL;
	    break;
	}

	case IVO_DSI     : {
	    details << "Data Storage Interrupt"; 
	    addDear = addEsr = true;
	    break;
	}

	case IVO_ISI     : {
	    details << "Instruction  Storage Interrupt"; 
	    addEsr = true;
	    break;
	}

	case IVO_EI      : details << "External Input Interrupt"; break;

	case IVO_ALGN    : {
	    details << "Alignment Interrupt"; 
	    addEsr = true;
	    break;
	}

	case IVO_PROG    : {
	    details << "Program Interrupt"; 
	    addEsr = true;
	    break;
	}

	case IVO_FPU     : details << "Floating Point Unavailable"; break;
	case IVO_SC      : details << "System Call"; break;
	case IVO_APU     : details << "Auxillary Processo Unavailable"; break;
	case IVO_DEC     : details << "Decrementer Interrupt"; break;
	case IVO_FIT     : details << "Fixed Interval Timer"; break;
	
	case IVO_WDT     : {
	    details << "Watchdog Timer"; 
	    category = INT_CATEGORY_CRITICAL;
	    break;
	}

	case IVO_DTLB    : {
	    details << "Data TLB Error"; 
	    addDear = addEsr = true;
	    break;
	}

	case IVO_ITLB    : details << "Instruction TLB Error"; break;
	case IVO_VECTOR  : details << "Vector Unavailable Interrupt"; break;
	case IVO_PDBI    : details << "Processor DoorBell Interrupt"; break;

	case IVO_PDBCI   : {
	    details << "Processor DoorBell Critical Interrupt"; 
	    category = INT_CATEGORY_CRITICAL;
	    break;
	}

	case IVO_GDBI    : details << "Guest DoorBell Interrupt"; break;
	
	case IVO_GDBCI   : {
	    details << "Guest DoorBell Critical or Machine Check Interrupt"; 
	    category = INT_CATEGORY_CRITICAL;
	    break;
	}

	case IVO_EHVSC   : details << "Embedded Hypervisor System Call"; break;
	case IVO_EHVPRIV : details << "Embedded Hypervisor Privilege"; break;
	
	case IVO_LRATE   : {
	    details << "LRAT Error Interrupt"; 
	    addEsr = true;
	    break;
	}

	case IVO_UDEC    : details << "User Decrementer"; break;
	case IVO_PERFMON : details << "Performance Monitor";  break;
	default : details << "????"; break;
	}

	switch ( category ) {
	case INT_CATEGORY_STANDARD : {
	    addInDcr( "SRR0", SPRN_SRR0, mbox, details );
	    addInDcr( "SRR1", SPRN_SRR1, mbox, details );
	    break;
	}
	case INT_CATEGORY_CRITICAL : {
	    addInDcr( "CSRR0", SPRN_CSRR0, mbox, details );
	    addInDcr( "CSRR1", SPRN_CSRR1, mbox, details );
	    break;
	}
	case INT_CATEGORY_MACHINE_CHECK : {
	    addInDcr( "MCSRR0", SPRN_MCSRR0, mbox, details );
	    addInDcr( "MCSRR1", SPRN_MCSRR1, mbox, details );
	    break;
	}
	}

	if ( addDear ) addInDcr( "DEAR", SPRN_DEAR, mbox, details );

	if ( addEsr ) {
	    uint64_t esr = addInDcr( "ESR", SPRN_ESR, mbox, details );
	    decodeEsr( esr, details );
	}

	addInDcr( "LR", SPRN_LR, mbox, details );
	
	event.setDetail( "DETAILS", details.str() );
    
    }

}
