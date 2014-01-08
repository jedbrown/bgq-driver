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
#include <hwi/include/bqc/testint_inlines.h>
#include <stdlib.h>

int test_main( void ) {

    
    if ( ProcessorID() == 0 ) {

	int i, rc;

	Personality_t* personality = fwext_getPersonality();

	int isPrimordialNode = 
	    ( personality->Network_Config.Acoord == 0 ) &&
	    ( personality->Network_Config.Bcoord == 0 ) &&
	    ( personality->Network_Config.Ccoord == 0 ) &&
	    ( personality->Network_Config.Dcoord == 0 ) &&
	    ( personality->Network_Config.Ecoord == 0 );

	for ( i = 0; i < 5; i++ ) {

	    // On one node .... make sure we delay a little bit

	    if ( isPrimordialNode ) {

		printf("(D) Sleeping before barrier %d\n", i);
		
		uint64_t start = GetTimeBase();
		uint64_t delta = 5ull * 1000ull * 1000ull * personality->Kernel_Config.FreqMHz;
		while ( ( GetTimeBase() - start ) < delta );

		printf("(D) Awake before barrier %d\n", i);
	    }

	    printf("(D) Before barrier %d\n", i);

	    if ( ( rc = fwext_getFwInterface()->barrier( 0 ) ) != FW_OK ) {
		printf("(E) Failed with rc=%d\n", rc );
		Terminate( rc );
	    }

	    printf("(D) After barrier %d\n", i);
	}

    }

    exit(0);
}



