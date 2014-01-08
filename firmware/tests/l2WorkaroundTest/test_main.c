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


#include <firmware/include/Firmware_Interface.h>
#include <firmware/include/fwext/fwext.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/l2_dcr.h>
#include <hwi/include/bqc/testint_inlines.h>

#include <stdlib.h>

int test_main( void ) {

    if ( ProcessorID() != 0 ) {
	exit(0);
    }

    uint64_t magic_or = 0;
    uint64_t magic_and = 0xffff;

    uint64_t base;
    for (base = L2_DCR__Core00_base; base <= L2_DCR__Core15_base; base += L2_DCR_stride) {
	uint64_t magic = DCRReadPriv(base + L2_DCR__BACKADDR_offset);
	magic_or |= magic;
	magic_and &= magic;
    }

    if ( (magic_or == 0xACDC) && (magic_and == 0xACDC) ) {
	if ( TI_isDD2() ) {
	    printf("DD2 : L2 Workaround is enabled\n");
	    exit(0);
	}
	else {
	    printf("(E) L2 workaround enabled on a non-DD2 node\n");
	}
    }
    else if ( ( magic_or == 0 ) && ( magic_and == 0 ) ) {
	if ( TI_isDD1() ) {
	    printf( "DD1 : L2 workaround is disabled (OK)\n");
	    exit(0);
	}
	else {
	    printf("(E) L2 workaround is disabled\n");
	}
    }
    else {
	printf("(E) L2 workaround is inconsistently configured.\n");
    }

    // If we got here .... things are not good.  Start dumping.

    printf("JTAGID: %016lX\n", TI_GetIDCODE());
    printf("DD1   : %d\n", TI_isDD1());
    printf("DD2   : %d\n", TI_isDD2());

    for (base = L2_DCR__Core00_base; base <= L2_DCR__Core15_base; base += L2_DCR_stride) {
	uint64_t magic = DCRReadPriv(base + L2_DCR__BACKADDR_offset);
	printf("L2[%ld].magic = %016lX\n", base, magic );
    }

    exit(-1);
}



