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


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// get the goodies
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <firmware/include/BeDram_inlines.h>
#include <firmware/include/fwext/fwext_lib.h>
#include <../../src/Firmware_internals.h>

typedef int (*TestCaseT)(unsigned coreId, unsigned hwThreadId, unsigned threadId, unsigned totalThreads);

typedef struct {
  volatile __attribute__((aligned(32))) uint64_t start;
  volatile __attribute__((aligned(32))) uint64_t count;
} FwtstBarrier_t;

void fwtstBarrier(FwtstBarrier_t* b, unsigned numberOfThreads, unsigned threadId );

#define FWTST_BARRIER_LOCK          (BeDRAM_LOCKNUM_FWEXT_FIRST+0)
#define FWTST_SEMAPHORE_TEST_LOCK   (BeDRAM_LOCKNUM_FWEXT_FIRST+1)

#define TRACE(x) printf x;
//#define TRACE(x)
#define _TRACE_ENTRY() TRACE(("(>) %s() [%s:%d]\n", __func__, __FILE__, __LINE__))
#define _TRACE_EXIT()  TRACE(("(<) %s() [%s:%d]\n", __func__, __FILE__, __LINE__))

#define ASSERT( condition, message ) \
  if ( ! (condition) ) { printf( "[ERROR] : Assertion failed at  %s:%s():%d\n", __FILE__, __func__, __LINE__); \
    printf message ; \
    return -1;	     \
  }

FwtstBarrier_t _fwtstBarrier;

extern void fwtstDelayMicroseconds(uint64_t usec);
extern void fwtstDelayCycles(uint64_t cycles);



#define SEMAPHORE_TEST
#define STRLEN_TEST
#define MEMCPY_TEST

#if defined (SEMAPHORE_TEST)

int semaphoreTest(unsigned coreId, unsigned hwThreadId, unsigned threadId, unsigned totalThreads) {

  _TRACE_ENTRY();

  fwtstBarrier(&_fwtstBarrier, totalThreads, threadId);

  /* ------------------------------------------------
   * Step 0: Initialize the semaphore.  Do this on
   *   thread 0 and then barrier so everyone knows
   *   that it is initialized.
   * ------------------------------------------------ */

  switch (threadId) {

  case 0 : {
    fw_semaphore_init(FWTST_SEMAPHORE_TEST_LOCK, 1);
    break;
  }

  }

  TRACE(("(I) Step 0 complete [%s:%d]\n", __func__, __LINE__));
  
  /* ------------------------------------------------
   * Step 1: Acquire and release the semaphore on
   * four different threads
   * ------------------------------------------------ */

  fwtstBarrier(&_fwtstBarrier, totalThreads, threadId);

  switch (threadId) {
  
  case 0 :
  case 1 :
  case 2 :
  case 3 : 
    {
      fw_semaphore_down( FWTST_SEMAPHORE_TEST_LOCK );
      fwtstDelayCycles(1000ul);
      fw_semaphore_up( FWTST_SEMAPHORE_TEST_LOCK );
      break;
    }
  }

  TRACE(("(I) Step 1 complete [%s:%d]\n", __func__, __LINE__));

  /* ------------------------------------------------
   * Step 2: Acquire a lock on one thread, hold onto
   * it and then release.  Attempt to acquire it on
   * the other threads.
   * ------------------------------------------------ */

  fwtstBarrier(&_fwtstBarrier, totalThreads, threadId);

  switch (threadId) {
  
  case 0 :
    {
      fw_semaphore_down( FWTST_SEMAPHORE_TEST_LOCK );
      fwtstDelayMicroseconds(500ul);
      fw_semaphore_up( FWTST_SEMAPHORE_TEST_LOCK );
      break;
    }
  case 1 :
  case 2 :
  case 3 : 
    {
      fwtstDelayMicroseconds(200ul); // wait a little to allow thread 0 to acquire
      fw_semaphore_down( FWTST_SEMAPHORE_TEST_LOCK );
      fw_semaphore_up( FWTST_SEMAPHORE_TEST_LOCK );
      break;
    }
  }

  TRACE(("(I) Step 2 complete [%s:%d]\n", __func__, __LINE__));


  /* ------------------------------------------------
   * Step 3: Acquire a lock on one thread, hold onto
   * it and then release.  Attempt to acquire it on
   * another thread but expect a timeout
   * ------------------------------------------------ */

  fwtstBarrier(&_fwtstBarrier, totalThreads, threadId);

  switch (threadId) {
  
  case 0 :
    {
      fw_semaphore_down( FWTST_SEMAPHORE_TEST_LOCK );
      fwtstDelayMicroseconds(500ul);
      fw_semaphore_up( FWTST_SEMAPHORE_TEST_LOCK );
      break;
    }
  case 1 : 
    {
      fwtstDelayMicroseconds(50ul); // wait a little to allow thread 0 to acquire the lock
      int rc = fw_semaphore_down_w_timeout( FWTST_SEMAPHORE_TEST_LOCK, 10ul );
      ASSERT(rc < 0, ("Expected timeout from fw_semaphore_down_w_timeout() rc=%d\n", rc ));
      break;
    }

  }

  TRACE(("(I) Step 3 complete [%s:%d]\n", __func__, __LINE__));


  /* ------------------------------------------------
   * Step 4: Acquire a lock on one thread, hold onto
   * it and then release.  Attempt to acquire it on
   * the other threads.
   * ------------------------------------------------ */

  fwtstBarrier(&_fwtstBarrier, totalThreads, threadId);

  switch (threadId) {
  
  case 0 :
  case 1 :
  case 2 :
  case 3 : 
    {
      int rc;

      rc = fw_semaphore_down_w_timeout( FWTST_SEMAPHORE_TEST_LOCK, 1000ul );
      ASSERT(rc == 0, ("Unexpected return code from fw_semaphore_down_w_timeout() rc=%d\n", rc ));
      fwtstDelayMicroseconds(10ul);
      fw_semaphore_up( FWTST_SEMAPHORE_TEST_LOCK );
      break;
    }
  }

  TRACE(("(I) Step 4 complete [%s:%d]\n", __func__, __LINE__));

  fwtstBarrier(&_fwtstBarrier, totalThreads, threadId);

  _TRACE_EXIT();
  return 0;
}

#endif

#if defined ( STRLEN_TEST )

int strlenTest(unsigned coreId, unsigned hwThreadId, unsigned threadId, unsigned totalThreads) {

  _TRACE_ENTRY();
  
  size_t strlen(const char*);

  size_t answer;

  answer = strlen( (const char*)0 ); /*passing null object*/
  ASSERT( answer == 0, ("Expected strlen(NULL)==0 (got %d)\n", (int)answer));

  answer = strlen( "" );
  ASSERT( answer == 0, ("Expected strlen(\"\")==0 (got %d)\n", (int)answer));

  answer = strlen("1");
  ASSERT( answer == 1, ("Expected strlen(\"1\")==1 (got %d)\n", (int)answer));

  _TRACE_EXIT();
  return 0;
}

#endif


#if defined ( MEMCPY_TEST )

int memcpyTest(unsigned coreId, unsigned hwThreadId, unsigned threadId, unsigned totalThreads) {
  _TRACE_ENTRY();
  // There's no point in doing this on more than one thread

  if ( threadId == 0 ) {

    extern void *memcpy( void *dest, const void *src, size_t n );

#define MEMCPY_SRC_SIZE 64
#define MEMCPY_DEST_SIZE 64

    char source[MEMCPY_SRC_SIZE];
    char dest[MEMCPY_DEST_SIZE];

    int i; // offset into dest buffer;
    int j; // offset into source buffer
    int k; // size of copy
    
    int n;
    
    // Initialize the source buffer to be SSSS...S
    for (n = 0; n < MEMCPY_SRC_SIZE; n++) {
      source[n] = 'S';
    }

    for (i = 0; i <= 16; i++) {
      for (j = 0; j <= 16; j++) {
	for (k = 0; k <= 16; k++) {

	  // Reset the dest buffer to be DDDD...D
	  for (n = 0; n < MEMCPY_DEST_SIZE; n++) dest[n] = 'D';

	  // Copy source into dest:
	  memcpy( dest + i, source + j, k );

	  // Afterwords, the destination buffer should now look like
	  // 
	  //     DD..D | SS..S | DD..D
	  //      ^       ^       ^
	  //      |       |       |
	  //      |       |       +---> SIZE - (k+i) 'D' characters
	  //      |       |
	  //      |       +-----------> k 'S' characters
	  //      |
	  //      +-------------------> i 'D' characters

	  for (n = 0; n < i; n++) {
	    ASSERT( dest[n] == 'D', ("Prefix error (%d,%d,%d) dest[%d]=%c\n", i, j, k, n, dest[n]));
	  }

	  for (n = 0; n < k; n++) {
	    ASSERT( dest[i+n] == 'S', ("Target error (%d,%d,%d) dest[%d]=%c\n", i, j, k, i+n, dest[i+n]));
	  }

	  for (n = 0; n < MEMCPY_DEST_SIZE - (i+k); n++) {
	    ASSERT( dest[i+k+n] == 'D', ("Suffix error (%d,%d,%d) dest[%d]=%c\n", i, j, k, i+k+n, dest[i+k+n]));
	  }

	  //TRACE(("(I) memcpy test (%d,%d,%d) complete\n", i, j, k));
	}
      }
    }
  }

  _TRACE_EXIT();
  return 0;
}

#endif

#if defined ( _SOME_TEST_TEMPLATE )

int _some_test_template_Test(unsigned coreId, unsigned hwThreadId, unsigned threadId, unsigned totalThreads) {

  _TRACE_ENTRY();


  _TRACE_EXIT();
  return 0;
}

#endif


/*
static inline uint64_t LOAD_INCREMENT(volatile uint64_t *addr, unsigned threadId) {
    uint64_t *ptr = (uint64_t *)(0x10000000000+(2<<3)+((threadId&3) <<6)+(((uint64_t)addr)<<5));
    return (*ptr);
}
*/


void fwtstBarrier(FwtstBarrier_t* b, unsigned numberOfThreads, unsigned threadId) {
  
  _TRACE_ENTRY();

  isync();
  uint64_t target = b->start + numberOfThreads;
  //uint64_t current = LOAD_INCREMENT(  &(b->count), threadId ) + 1;
  uint64_t current = b->count = BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_FWEXT_FIRST) + 1;

  if (current == target) {
	b->start = current;
	mbar();
    } 
    else {
      while (b->start < current) /*spin*/;
    }
  _TRACE_EXIT();
}

void fwtstDelayCycles(uint64_t cycles) {
  do {
    asm volatile ( "nop;" );
  } while (--cycles);
}

void fwtstDelayMicroseconds(uint64_t usec) {
  fwtstDelayCycles( usec * FW_Personality.Kernel_Config.FreqMHz );
}



TestCaseT ALL_TESTS[] = {

#if defined ( SEMAPHORE_TEST )
  semaphoreTest,
#endif

#if defined ( STRLEN_TEST )
  strlenTest,
#endif

#if defined ( MEMCPY_TEST )
  memcpyTest,
#endif

};

#define STACK(i) ( ( ((i)+1) * 1024 * 32 ) - 64 )

int main( int argc, char *argv[], char **envp ) {

  // Establish a stack in DDR (get off the tiny BeDRAM stack

  unsigned stackp = STACK(ProcessorID());
  asm volatile ( "or 1,%0,%0" : : "r" (stackp) : "memory" );

  
  int i;
  int rc = 0;

  unsigned coreId       = ProcessorCoreID(); // [0..16]
  unsigned hwThreadId   = ProcessorThreadID();    // [0..3]
  unsigned threadId     = ProcessorID(); // [0..67]
  unsigned totalThreads = BgGetNumThreads(); 

  for ( i = 0; (i < sizeof(ALL_TESTS)/sizeof(ALL_TESTS[0])) && (rc == 0); i++) {
    if (ProcessorID() == 0 )TRACE( ("Starting test %d\n",i) );

    rc |= ALL_TESTS[i]( coreId, hwThreadId, threadId, totalThreads );
  }

  rc ? crash(rc) : exit(0);
    
  return( 0 ); // just to shut up the compiler
}


