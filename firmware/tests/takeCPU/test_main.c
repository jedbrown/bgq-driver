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

static int beQuiet = 0;

#define ON_MESA()  ( (_firmware->deprecated.personalityPtr->Kernel_Config.NodeConfig & PERS_ENABLE_Simulation) ? 1 : 0 )

#define TRACE(x)
//#define TRACE(x) if (beQuiet==0) printf x
//#define TRACE(x) printf x

#define TRACE_ENTRY()  TRACE(("(>) %s        [%s:%d]\n", __func__, __FILE__, __LINE__));
#define TRACE_EXIT(rc) TRACE(("(<) %s rc=%d   [%s:%d]\n", __func__, (int)(rc), __FILE__, __LINE__));

#define INFO(x) if (beQuiet==0) printf x

#define STACK_SIZE (32*1024)

Firmware_Interface_t* _firmware = 0;
Personality_t         _personality;
unsigned              _numberOfThreads = 0;
volatile int          _abortAllTests = 0;
volatile unsigned     _testNumber = 0;
char                  _stacks[17][4][STACK_SIZE] ALIGN_L2_CACHE /* = { 0, } */ ;

typedef int (*Test_t)( unsigned testNumber );

#define MAX_TESTS 256

volatile uint64_t atomic_counters[MAX_TESTS] ALIGN_QUADWORD = { 0, };

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

unsigned rand3() {
  static unsigned long seed = 3;
  seed = ((( seed * 214013L + 2531011L) >> 16 ) & 32767);
  return (unsigned)seed;
}

void barrierAllThreads(unsigned test_number) {

  TRACE_ENTRY();

#if ! defined (JUST_ONE_THREAD)

  Fetch_and_Add( (uint64_t*)&(atomic_counters[test_number]), 1);

  while ( ( atomic_counters[test_number] < _numberOfThreads ) && (_abortAllTests == 0) )
    //_delay_cycles(100)
    ;

#endif

  TRACE_EXIT(0);

  return;
}

#if 0

int64_t FetchAndAdd( int64_t* atomic, int64_t increment ) {

  register int64_t original, tmp;

  ppc_msync();

  do {
    original = LoadReserved( (uint64_t*)atomic );
  }
  while ( !StoreConditional( (uint64_t*)atomic, original + increment ) );

  return original;
}

void semaphore_init( unsigned test_number, unsigned numberOfTickets ) {
  atomic_counters[test_number] = numberOfTickets;
}

void semaphore_down( unsigned test_number ) {
  while (1) {
    if ( atomic_counters[test_number] > 0 ) {
      if ( FetchAndAdd( &(atomic_counters[test_number]), -1ul ) > 0 ) {
	return;
      }
      else { // someone beat us to it ...
	FetchAndAdd( &(atomic_counter[test_number]), 1 );
      }
    }
  }
}

void semaphore_up( unsigned test_number ) {
  FetchAndAdd( &(atomic_counters[test_number]), 1 );
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


#define SET_STACKP() {							\
    size_t pStk = (size_t)((size_t)&(_stacks[ProcessorCoreID()][ProcessorThreadID()][STACK_SIZE-64])); \
    asm volatile ( "mr 1,%0;" : : "r" (pStk) : "r1", "memory" );	\
  }
    

#define DELAY_HW_THREAD_0() { \
  if ( ProcessorThreadID() == 0 ) { \
    int n = 1000;  \
    while ( (--n) > 0 ); \
  } \
  }

void basicTakeCpuTest_auxiliaryEntryPoint(void* arg) {

  SET_STACKP();
  TRACE_ENTRY();
  int rc = 0;
  uint64_t expected = (uint64_t)ProcessorCoreID();
  uint64_t actual  = (uint64_t)arg;
  
  ASSERT2( 
	  actual == expected,
	  ("Argument mismatch : (actual) %lX : %lX (expected)\n", actual, expected ),
	  rc );

  barrierAllThreads(_testNumber);
  TRACE_EXIT(0);
  
  DELAY_HW_THREAD_0();
  _firmware->returnCPU();
}

int basicTakeCpuTest( unsigned testNumber ) {

  TRACE_ENTRY();
  int rc = 0;

  rc = _firmware->takeCPU( 0, 0xE, basicTakeCpuTest_auxiliaryEntryPoint, 0 );
  ASSERT ( rc == 0, ( "Unexpected error response from takeCPU rc=%d \n", (int)rc ) );

  uint64_t cpu;

  for ( cpu = 1; cpu < _numberOfThreads/4; cpu++) {
    rc = _firmware->takeCPU( cpu, 0xF, basicTakeCpuTest_auxiliaryEntryPoint, (void*)cpu );
    ASSERT ( rc == 0, ( "Unexpected error response from takeCPU rc=%d \n", (int)rc ) );
  }


  barrierAllThreads(testNumber);

  TRACE_EXIT(0);
  return 0;
}

#if 0
int testRandomness( unsigned testNumber ) {

#define NUM_BUCKETS 4
  unsigned n = 0;
  unsigned bucket[NUM_BUCKETS];

  for (n = 0; n < NUM_BUCKETS; n++) {
    bucket[n] = 0;
  }

  for (n = 0; n < 1024; n++) {
    bucket[rand3()%NUM_BUCKETS]++;
  }

  for (n = 0; n < NUM_BUCKETS; n++) {
    printf("%2d) %4d\n", n, bucket[n]);
  }

  return 0;
}
#endif


#define RAND_THREAD() ((rand3() + 1) % _numberOfThreads) /* avoid thread 0 */
static volatile unsigned _hops;

void tagEntryPoint() {

  SET_STACKP();
  
  TRACE_ENTRY();

  // Decrement the hop count.  If we have counted down to zero,
  // then we are done (update the atomic flag to indicate that).  
  // Otherwise, elect some other thread to be  "it".  

  _hops--; 

  if ( _hops > 0 ) {
    unsigned tid;

    do {
      tid = RAND_THREAD();
    } while ( (tid == ProcessorID()) || (tid==0) );

    TRACE(("(D) taking thread=%d remaining-hops=%d\n",tid, _hops));

    unsigned cpu = tid / 4;
    tid          = tid % 4;

    TRACE(("(D) taking cpu=%d thread=%d\n", cpu, tid));

    int rc = _firmware->takeCPU( cpu, 1 << tid, tagEntryPoint, 0 );

    ASSERT2 ( rc == 0, ( "Unexpected error response from takeCPU rc=%d \n", (int)rc ), rc );
  }
  else {
    Fetch_and_Add((uint64_t*)&(atomic_counters[_testNumber]), 1);
  }
  
  TRACE_EXIT(0);
  DELAY_HW_THREAD_0();
  _firmware->returnCPU();
}


int tag(unsigned testNumber ) {

  // ---------------------------------------------------------------------------
  // A reincarnation of an old test that I created in my Java Message Service
  // days.  It is effectively an elaborate game of tag.  The primordial thread
  // activates one other thread.  The code on that thread then tags another
  // thread and then shuts itself down.  And so on.  After a specified number
  // of tags (hops), the process is shut down.
  // ---------------------------------------------------------------------------

  int rc = 0;

  TRACE_ENTRY();

  _hops = 1000;

  if ( ON_MESA() ) {
    _hops = 10;
  }

  unsigned tid;

  do {
      tid = RAND_THREAD();
      TRACE(("(D) choosing thread=%d\n",tid));
  } while ( tid == ProcessorID() );

  TRACE(("(D) taking thread=%d\n",tid));
  unsigned cpu = tid / 4;
  tid          = tid % 4;

  TRACE(("(D) taking cpu=%d thread=%d\n", cpu, tid));
  rc = _firmware->takeCPU( cpu, 1<<tid, tagEntryPoint, 0);
  
  ASSERT ( rc == 0, ( "Unexpected error response from takeCPU rc=%d \n", (int)rc ) );

  TRACE(("(D) waiting for circus to end ...\n"));
  while ( atomic_counters[testNumber] == 0 ) {
    TRACE(("(D) %d hops remainding ...\n", _hops));
  }

  TRACE_EXIT(0);
  return 0;
}

int errorTests( unsigned testNumber ) {

  int rc = 0;

  TRACE_ENTRY();

  // We cannot take all threads on our own processor (because we are already here!):

  rc = _firmware->takeCPU( ProcessorCoreID(), 0xF, basicTakeCpuTest_auxiliaryEntryPoint, 0 );
  ASSERT ( rc < 0, ( "Unexpected non-error response from takeCPU rc=%d \n", (int)rc ) );

  // We cannot take threads for a processor that doesn't exist:
  
  rc = _firmware->takeCPU( 17, 0xF, basicTakeCpuTest_auxiliaryEntryPoint, 0 );
  ASSERT ( rc < 0, ( "Unexpected non-error response from takeCPU rc=%d \n", (int)rc ) );


  TRACE_EXIT(0);
  return 0;
}


void everyThreadIsALeader_Follower() {
  TRACE_ENTRY();
  barrierAllThreads(_testNumber);
  TRACE_EXIT(0);
  DELAY_HW_THREAD_0();
  _firmware->returnCPU();
}

void everyThreadIsALeader_Leader() {

  TRACE_ENTRY();

  unsigned t;
  unsigned myThreadId = ProcessorID();


  for (t = 1; t < _numberOfThreads; t++) {
    
    if (t != myThreadId) {
      unsigned cpu = t / 4;
      unsigned tid = t % 4;
      int rc = _firmware->takeCPU( cpu, 1<<tid, everyThreadIsALeader_Follower, 0);
      ASSERT2( rc == 0, ( "Unexpected error response from takeCPU(%d,%X) rc=%d \n", cpu, 1<<tid,(int)rc ), rc );
    }
  }

  barrierAllThreads(_testNumber);
  TRACE_EXIT(0);
  DELAY_HW_THREAD_0();
  _firmware->returnCPU();

}

int everyThreadIsALeaderTest( unsigned testNumber ) {

  // Skip this test in cycle sim ... its way to long

  if (ON_MESA()) {
    return 0;
  }

  TRACE_ENTRY();

  unsigned l;

  for ( l = 1; l < _numberOfThreads; l++ ) {

    TRACE(("(D) Leading with thread %d\n", l ));
    printf("(D) Leading with thread %d\n", l );

    atomic_counters[testNumber] = 0; // clear out the barrier

    unsigned cpu = l / 4;
    unsigned thd = l % 4;

    // 
    int rc;
    int retries = 10;
    do {
      rc = _firmware->takeCPU( cpu, 1 << thd, everyThreadIsALeader_Leader, 0 );
      if (rc != 0) {
      }
    } while ( (rc != 0) && --retries);

    ASSERT ( rc == 0, ( "Unexpected error response from takeCPU cpu=%d thread=%d rc=%d \n", cpu, thd, (int)rc ) );

    barrierAllThreads(testNumber);
  }

  TRACE_EXIT(0);
  return 0;
}

#define MAX_PRIMES 1024*10
unsigned long primes[MAX_PRIMES];
unsigned long distance[MAX_PRIMES];
unsigned long num_primes = 0;


int isPrime(unsigned long n) {

  int i;

  for (i = 0; i < num_primes; i++) {
    if ( (n % primes[i]) == 0 ) {
      return 0;
    }
  }

  return 1;
}

unsigned long averageDistance() {

  unsigned long average = 0;
  unsigned n;
  for (n = 0; n < num_primes; n++) {
    average += distance[n];
  }
  return average * 100 / num_primes;
}

int linuxBootSimulation( unsigned testNumber ) {

#if 0
  if ( ON_MESA() ) {
    return 0;
  }
#endif

  TRACE_ENTRY();

  int rc;

  unsigned long n, last;


  primes[num_primes++] = 2;
  primes[num_primes++] = 3;

  distance[0] = distance[1] = distance[2] = 1;

  last = 3;
  for (n = 4; n < 50; n++) {
    if ( isPrime(n) ) {
      primes[num_primes] = n;
      distance[num_primes] = n - last;
      num_primes++;
#if 0
      if ( ( num_primes % 10 ) == 0 ) {
	printf( "%lu is the %luth prime, distance=%3lu avg-distance=%3lu\n", n, num_primes, (n-last), averageDistance());
      }
#endif
      last = n;
    }
  }
  

  TRACE(("Taking threads 1-3 on the primordial core ...\n"));
  rc = _firmware->takeCPU( 0, 0xE, basicTakeCpuTest_auxiliaryEntryPoint, 0 );
  ASSERT ( rc == 0, ( "Unexpected error response from takeCPU rc=%d \n", (int)rc ) );

  unsigned cpu;

  TRACE(("Taking remaining CPUs (%d of them ....%d threads total)\n", _numberOfThreads/4, _numberOfThreads));
  for ( cpu = 1; cpu < _numberOfThreads/4; cpu++) {
    rc = _firmware->takeCPU( cpu, 0xF, basicTakeCpuTest_auxiliaryEntryPoint, (void*)((uintptr_t)cpu) );
    ASSERT ( rc == 0, ( "Unexpected error response from takeCPU rc=%d \n", (int)rc ) );
  }


  barrierAllThreads(testNumber);

  TRACE_EXIT(0);
  return 0;


}


typedef struct _TestList_t {
  Test_t test;
  const char* name;
} TestList_t;

TestList_t ALL_TESTS[] = {
#if 0
  { basicTakeCpuTest,   "basicTakeCpuTest" },
  //{ testRandomness,   "testRandomness" },
  { tag,                "tag" },
  { errorTests,         "errorTests"      },
  { everyThreadIsALeaderTest, "everyThreadIsALeaderTest" },
#endif
  { linuxBootSimulation, "linuxBootSimulation" },
};




int run_tests() {

  TRACE(("(>) %s [%s:%d]\n", __func__, __FILE__, __LINE__));

  unsigned t;
  int rc = 0;

  for (t = 0; (t < sizeof(ALL_TESTS) / sizeof(ALL_TESTS[0])) && (rc == 0); t++) {

    if ( ALL_TESTS[t].test == 0 ) 
      break;

    if (_abortAllTests != 0) {
      return -1;
    }

    INFO(("--- Test %d : %s ---\n", t, ALL_TESTS[t].name ));

    _testNumber = t;

    rc |= ALL_TESTS[t].test( t );
  }
  return rc;
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

int test_main( void ) {
  
  TRACE(("(>) %s [%s:%d]\n", __func__, __FILE__, __LINE__));

  int rc = 0;

  _numberOfThreads = getNumberOfThreads();

  TRACE(("(D) number of threads : %d\n", _numberOfThreads));

  rc = run_tests();

  TRACE(("(<) %s rc=%d [%s:%d]\n", __func__, rc, __FILE__, __LINE__));

  _firmware->terminate(rc);
    
  return 0;

}



__C_LINKAGE void __NORETURN crash( int status ) {
  printf("I am here unexpetedly -> %s:%s:%d\n", __func__, __FILE__, __LINE__);
  _firmware->terminate(-1);
  while (1);
}



//
// The entry point for C code (via start.S via firmware)
//
//  Note: (1) We're still running on the Firmware's 1KB stack, be careful.
//        (2) Firmware owns interrupt vectors (IVPR) until we're ready to handle interrupts.
//

__C_LINKAGE void __NORETURN _test_entry( Firmware_Interface_t *fw_interface ) {

    int threadID       = ProcessorID(); // 0..67
    int processorIndex = ProcessorCoreID(); // 0..16
    int threadIndex    = ProcessorThreadID();    // 0..3

    if ( 0 == threadID ) {
      _firmware = fw_interface;
      beQuiet = (fw_interface->deprecated.personalityPtr->Kernel_Config.NodeConfig & PERS_ENABLE_Simulation) ? 1 : 0;
      BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_KERNEL_INIT);
    }
    else { 

      // block until primordial thread is initialized

      while ( ! BeDRAM_Read(BeDRAM_LOCKNUM_KERNEL_INIT) )
	/* spin */
	;
    }

    // Relaunch threads on their own stack

    do {
      
      OPD_Entry_t *opd_main = (OPD_Entry_t *)test_main;
      OPD_Entry_t *opd_exit = (OPD_Entry_t *)crash;

      size_t pStk = (size_t)((size_t)&(_stacks[processorIndex][threadIndex][STACK_SIZE-64]));

      mtspr( SPRN_SRR0_IP,   opd_main->Entry_Addr );
      mtspr( SPRN_SRR1_MSR,  MSR_KERNEL );
      isync();
      
      asm volatile (
		     "mtlr  %2;"
                     "li    0,0;"
                     "mr    1,%0;"
                     "mr    2,%1;"
                     "li    3,0;"
                     "rfi;"
                     "nop;"
                     :
                     : "r" (pStk),
                     "r" (opd_main->TOC_Addr),
                     "r" (opd_exit->Entry_Addr)
                     : "r0", "r1", "r2", "r3", "lr", "cc", "memory" );
    } while (1);
}


