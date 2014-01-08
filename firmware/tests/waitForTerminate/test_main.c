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
#include <firmware/include/fwext/fwext_lib.h>


#include <stdlib.h>

void usleep( uint64_t micros ) {

    uint64_t delta = micros * fwext_getPersonality()->Kernel_Config.FreqMHz;

    uint64_t start = GetTimeBase();
    
    while ( ( GetTimeBase() - start ) < delta );
}

int test_main( void ) {


    if (ProcessorID() > 0) {
	_Bg_Sleep_Forever();
    }

  unsigned long sleepTime = fwext_strtoul( fwext_getenv("SLEEP"), 0, 0 );
  unsigned long termCode  = fwext_strtoul( fwext_getenv("RC"), 0, 0 );

  printf("Sleeping for %ld seconds ...\n", sleepTime);

  usleep( sleepTime * 1000ull * 1000ull );

  //Delay( sleepTime * 1600ull * 1024ull * 1024ull );

  printf("Awake!\n\n");

  printf("Terminating with rc=%ld\n", termCode );
  Terminate(termCode);
}
 


