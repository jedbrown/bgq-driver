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


#include <hwi/include/common/bgq_alignment.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/testint_dcr.h>

#include <firmware/include/mailbox.h>
#include <firmware/include/personality.h>
#include <firmware/include/Firmware_Interface.h>
#include <firmware/include/BeDram_inlines.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "utils.h"

//static int beQuiet = 0;
volatile uint64_t domainIDs[MAX_THREADS] = { 0, };

#define ON_MESA()  ( (_firmware->deprecated.personalityPtr->Kernel_Config.NodeConfig & PERS_ENABLE_Simulation) ? 1 : 0 )


//#define INFO(x) if (beQuiet==0) printf x

Firmware_Interface_t* _firmware = 0;

int printf( const char *fmt, ... ) {
  char buffer[256];
    va_list args;
    va_start( args, fmt );
    int len = vsprintf( buffer, fmt, args );
    _firmware->putn(buffer,len);
    va_end(args);
    return len;
}

size_t strlen(const char* s) {
  size_t count = 0;
  while ( *(s++) != 0 )
    count++;
  return count;
}

int puts( const char* s ) {
  return _firmware->putn(s, strlen(s));
}

void fw_memset( char* ptr, char value,  size_t size) {
  int i;
  for (i = 0; i < size; i++) {
    ptr[i] = value;
  }
}

void setFirmware(Firmware_Interface_t* f) {
  _firmware = f;
}


int64_t fetchAndAdd( int64_t* atomic, int64_t increment ) {

  register int64_t original;

  ppc_msync();

  do {
    original = LoadReserved( (uint64_t*)atomic );
  }
  while ( !StoreConditional( (uint64_t*)atomic, original + increment ) );

  return original;
}

int64_t testAndSet( volatile int64_t* atomic, int64_t preCondition, int64_t value ) {

  register int64_t original, newValue;

  ppc_msync();

  do {
    original = LoadReserved( (uint64_t*)atomic );
    if ( original == preCondition ) {
      newValue = value;
    }
    else {
      newValue = original;
    }
  }
  while ( !StoreConditional( (uint64_t*)atomic, newValue ) );

  return newValue;
}


void barrierThreads( volatile uint64_t* barrier, unsigned numberOfThreads ) {

  int64_t timeout = 1000000;

  Fetch_and_Add( (uint64_t*)barrier, 1);

  while ( ( *barrier < numberOfThreads ) && (--timeout > 0 ) )
    /* spin */
    ;

  if ( timeout <= 0 ) {
    printf( "(E) Barrier @%p->%ld timed out (number of threads = %d)\n", barrier, *barrier, numberOfThreads ) ;
    _firmware->terminate(__LINE__);
  }
  return;
}

#if 0


void semaphore_init( unsigned test_number, unsigned numberOfTickets ) {
  atomic_counters[test_number] = numberOfTickets;
}

void semaphore_down( unsigned test_number ) {
  while (1) {
    if ( atomic_counters[test_number] > 0 ) {
      if ( fetchAndAdd( &(atomic_counters[test_number]), -1ul ) > 0 ) {
	return;
      }
      else { // someone beat us to it ...
	fetchAndAdd( &(atomic_counter[test_number]), 1 );
      }
    }
  }
}

void semaphore_up( unsigned test_number ) {
  fetchAndAdd( &(atomic_counters[test_number]), 1 );
}
#endif


#define ASSERT( condition, message )					\
  if ( ! (condition) ) { printf( "[ERROR] : Assertion failed at  %s:%s():%d\n", __FILE__, __func__, __LINE__); \
    printf message ;							\
    _abortAllTests = 1;							\
    TRACE_EXIT(-1);							\
    return -1;								\
  }

#define ASSERT2( condition, message, rc )					\
  if ( ! (condition) ) { printf( "[ERROR] : Assertion failed at  %s:%s():%d\n", __FILE__, __func__, __LINE__); \
    printf message ;							\
    _abortAllTests = 1;							\
    rc = -1;								\
  }

    

#define DELAY_HW_THREAD_0() { \
  if ( ProcessorThreadID() == 0 ) { \
    int n = 1000;  \
    while ( (--n) > 0 ); \
  } \
  }

unsigned getNumberOfThreads() {

    unsigned v =
      popcnt64( TESTINT_DCR_PRIV_PTR->thread_active0_rb ) + 
      popcnt64(TESTINT_DCR_PRIV_PTR->thread_active1_rb );
    
    if ( (v == 0) || (v > 68) ) {
      _firmware->terminate(__LINE__);
    }

    return v;
}


