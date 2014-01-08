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
#include <firmware/include/fwext/fwext_lib.h>
#include "../../src/Firmware_RAS.h"

#include <stdlib.h>

int basicTest( unsigned numberOfThreads ) {
  fwext_getFwInterface()->writeRASEvent( 0x11110000, 0, NULL );
  return 0;
}

int alternatingEvents( unsigned numberOfThreads ) {

  unsigned i;

  if ( ProcessorID() == 0 ) {
    for ( i = 0; i <= 100; i++ ) {
      fwext_getFwInterface()->writeRASEvent( 0x11110011, 0, NULL );
      fwext_getFwInterface()->writeRASEvent( 0x11110012, 0, NULL );
    }
  }

  return 0;
}

int singleThreadStorm( unsigned numberOfThreads ) {

  unsigned i;

  if ( ProcessorID() == 0 ) {
    for ( i = 0; i <= 100; i++ ) {
      fwext_getFwInterface()->writeRASEvent( 0x11110001, 0, NULL );
    }
  }

  return 0;
}

int rotationalTest( unsigned numberOfThreads ) {

  unsigned i, j;
  
  for ( i = 0; i <= 10; i++ ) {
    for ( j = 0; j < 5; j++ ) {
      fwext_getFwInterface()->writeRASEvent( 0x11110030 + j, 0, NULL );
    }
  }

  return 0;
}

int stormWarningTest( unsigned numberOfThreads ) {

  unsigned i;

  if ( ProcessorID() == 0 ) {
    uint64_t start = GetTimeBase();
    for ( i = 0; i < FW_RAS_EVENT_ERROR_THRESHOLD; i++ ) {
      fwext_getFwInterface()->writeRASEvent( 0x11110400, 0, NULL );
    }
    uint64_t end = start + (uint64_t)FW_RAS_EVENT_ERROR_THRESHOLD * (uint64_t)FW_RAS_STORM_THRESHOLD_MILLIS * (uint64_t)fwext_getPersonality()->Kernel_Config.FreqMHz * (uint64_t)1000;

    while ( GetTimeBase() < end )
      /* spin */ ;

    fwext_getFwInterface()->writeRASEvent( 0x11110400, 0, NULL );
  }


  return 0;
}

typedef int (*Test_t)( unsigned numberOfThreads );
typedef struct TestList_t {
  const char* name;
  Test_t      test;
} TestList_t;

TestList_t TESTS[] = {
  { "Basic Test", basicTest },
  { "Single Thread Storm", singleThreadStorm },
  { "Alternating events", alternatingEvents },
  { "Rotational Test", rotationalTest },
#if 0
  { "Storm Warning Test", stormWarningTest },
#endif
};

FwExt_Barrier_t testBarrier = { 0, 0 };

int test_main( void ) {
  
  unsigned numberOfThreads = BgGetNumThreads();
  unsigned i;

  for ( i = 0 ; i < sizeof(TESTS)/sizeof(TESTS[0]); i++ ) {

    if ( ProcessorID() == 0 ) {
      printf("-------- Test %d : %s ------------\n", i, TESTS[i].name );
    }

    fwext_barrier( &testBarrier, numberOfThreads );
    TESTS[i].test( numberOfThreads );
    fwext_barrier( &testBarrier, numberOfThreads );


  }
  

  exit(0);
}



