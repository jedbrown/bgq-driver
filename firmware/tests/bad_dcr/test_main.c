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




#include <hwi/include/bqc/A2_inlines.h>

#include <hwi/include/bqc/bedram_dcr.h>
#include <hwi/include/bqc/cs_dcr.h>
#include <hwi/include/bqc/dc_arbiter_dcr.h>
#include <hwi/include/bqc/dr_arb_dcr.h>
#include <hwi/include/bqc/ddr.h>
#include <hwi/include/bqc/devbus_dcr.h>
#include <hwi/include/bqc/en_dcr.h>
#include <hwi/include/bqc/gea_dcr.h>
#include <hwi/include/bqc/l1p_dcr.h>
#include <hwi/include/bqc/l1p_mmio.h>
#include <hwi/include/bqc/l2_dcr.h>
#include <hwi/include/bqc/l2_central_dcr.h>
#include <hwi/include/bqc/l2_counter_dcr.h>
#include <hwi/include/bqc/ms_genct_dcr.h>
#include <hwi/include/bqc/mu_dcr.h>
#include <hwi/include/bqc/nd_rese_dcr.h>
#include <hwi/include/bqc/nd_500_dcr.h>
#include <hwi/include/bqc/nd_x2_dcr.h>
#include <hwi/include/bqc/pe_dcr.h>
#include <hwi/include/bqc/serdes_left_dcr.h>
#include <hwi/include/bqc/serdes_right_dcr.h>
#include <hwi/include/bqc/testint_dcr.h>
#include <hwi/include/bqc/upc_c_dcr.h>


#include <stdlib.h>

#ifdef _CNK_

#include <stdio.h>

#define GETENV    getenv
#define STRTOUL   strtoul
#define STRCMP    strcmp

int main( int argc, char** argv ) {

#else

#include <firmware/include/Firmware_Interface.h>
#include <firmware/include/fwext/fwext.h>
#include <firmware/include/fwext/fwext_lib.h>

#define GETENV    fwext_getenv
#define STRTOUL   fwext_strtoul
#define STRCMP    fwext_strcmp

int test_main( void ) {

    if ( ProcessorID() == 0 ) {

#endif



	char* test_str     = GETENV("TEST");
	char* read_str     = GETENV("READ");
	char* bad_addr_str = GETENV("BAD_ADDR");
	char* instance_str = GETENV("INSTANCE");

	int read = read_str ? STRTOUL( read_str, 0, 0 ) : 0;
	int bad_addr = bad_addr_str ? STRTOUL( bad_addr_str, 0, 0 ) : 0;
	int instance = instance_str ? STRTOUL( instance_str, 0, 0 ) : 0;

	unsigned dcr_addr = 0;

	if ( STRCMP( test_str, "BeDRAM" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = BEDRAM_DCR_base + BEDRAM_DCR_size + 1;
	    }
	    else {
		dcr_addr = BEDRAM_DCR( INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}

	if ( STRCMP( test_str, "CS" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = CS_DCR_base + CS_DCR_size + 1;
	    }
	    else {
		dcr_addr = CS_DCR( INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}

	if ( STRCMP( test_str, "DCR" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = DC_ARBITER_DCR_base + DC_ARBITER_DCR_size + 1;
	    }
	    else {
		dcr_addr = DC_ARBITER_DCR( INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}


	if ( STRCMP( test_str, "DDR" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = DR_ARB_DCR_base + DR_ARB_DCR_size + 1;
	    }
	    else {
		dcr_addr = DR_ARB_DCR( instance, INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}

	if ( STRCMP( test_str, "DevBus" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = DEVBUS_DCR_base + DEVBUS_DCR_size + 1;
	    }
	    else {
		dcr_addr = DEVBUS_DCR( INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}

	if ( STRCMP( test_str, "EnvMon" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = EN_DCR_base + EN_DCR_size + 1;
	    }
	    else {
		dcr_addr = EN_DCR( INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}

#if 0

	// GEA is not really DCRs ... it is MMIO
	
	if ( STRCMP( test_str, "GEA" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = GEA_DCR_base + GEA_DCR_size + 1;
	    }
	    else {
		dcr_addr = GEA_DCR( INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}

#endif

	if ( STRCMP( test_str, "L1P" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = L1P_DCR_base /* + L1P_DCR_size + 1 */;
	    }
	    else {
		dcr_addr = L1P_DCR( instance, INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}

	if ( STRCMP( test_str, "L2" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = L2_DCR_base + L2_DCR_size + 1;
	    }
	    else {
		dcr_addr = L2_DCR( instance, INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}

	if ( STRCMP( test_str, "L2Central" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = L2_CENTRAL_DCR_base + L2_CENTRAL_DCR_size + 1;
	    }
	    else {
		dcr_addr = L2_CENTRAL_DCR( INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}

	if ( STRCMP( test_str, "L2Counter" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = L2_COUNTER_DCR_base + L2_COUNTER_DCR_size + 1;
	    }
	    else {
		dcr_addr = L2_COUNTER_DCR( instance, INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}

	if ( STRCMP( test_str, "MS_Genct" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = MS_GENCT_DCR_base + MS_GENCT_DCR_size + 1;
	    }
	    else {
		dcr_addr = MS_GENCT_DCR( INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}


	if ( STRCMP( test_str, "MU" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = MU_DCR_base + MU_DCR_size + 1;
	    }
	    else {
		dcr_addr = MU_DCR( INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}

	if ( STRCMP( test_str, "ND_RESE" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = ND_RESE_DCR_base + ND_RESE_DCR_size + 1;
	    }
	    else {
		dcr_addr = ND_RESE_DCR( instance, INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}

	if ( STRCMP( test_str, "ND_X2" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = ND_X2_DCR_base + ND_X2_DCR_size + 1;
	    }
	    else {
		dcr_addr = ND_X2_DCR( INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}

	if ( STRCMP( test_str, "ND_500" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = ND_500_DCR_base + ND_500_DCR_size + 1;
	    }
	    else {
		dcr_addr = ND_500_DCR( INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}


	if ( STRCMP( test_str, "PE" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = PE_DCR_base + PE_DCR_size + 1;
	    }
	    else {
		dcr_addr = PE_DCR( INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}

	if ( STRCMP( test_str, "SERDES_LEFT" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = SERDES_LEFT_DCR_base + SERDES_LEFT_DCR_size + 1;
	    }
	    else {
		dcr_addr = SERDES_LEFT_DCR( INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}

	if ( STRCMP( test_str, "SERDES_RIGHT" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = SERDES_RIGHT_DCR_base + SERDES_RIGHT_DCR_size + 1;
	    }
	    else {
		dcr_addr = SERDES_RIGHT_DCR( INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}

	if ( STRCMP( test_str, "TESTINT" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = TESTINT_DCR_base + TESTINT_DCR_size + 1;
	    }
	    else {
		dcr_addr = TESTINT_DCR( INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}

#if 0
	// UPC_C is not really DCRs ... it is MMIO 

	if ( STRCMP( test_str, "UPC_C" ) == 0 )  {
	    if ( bad_addr ) {
		dcr_addr = UPC_C_DCR_base + UPC_C_DCR_size + 1;
	    }
	    else {
		dcr_addr = UPC_C_DCR( INTERRUPT_INTERNAL_ERROR__FORCE );
	    }
	}
#endif


	printf("Test:%s %s %s DCR %X instance=%d\n", test_str, ( read ? "read" : "write" ), ( bad_addr ? "bad_addr" : "bad_priv" ), dcr_addr, instance );

	if ( read ) {
	    DCRReadUser( dcr_addr );
	}
	else {
	    DCRWriteUser( dcr_addr, 0 );
	}

#ifdef _CNK_
	return 0;
#else
    }
    exit(0);
#endif

}



