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
#include <firmware/include/Firmware_Interface.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <firmware/include/fwext/fwext_lib.h>
#include <firmware/include/fwext/fwext.h>

#include <string.h>
#include <stdio.h>

//#define VERBOSE 1

typedef int (*Test_t)(void);

#define MAX_TESTS 64

volatile uint64_t atomic_counters[MAX_TESTS] ALIGN_QUADWORD = { 0, };


void exit(int);
void writeRASEvent(uint32_t, uint16_t, uint64_t*);


__INLINE__ void _delay_cycles( uint64_t cycles ) {
   do
      {
      asm volatile("nop;");
      }
   while( --cycles );
}


void testBarrier(unsigned test_number, unsigned barrier_count) {

#ifdef VERBOSE
  printf("(>) %s test=%d\n", __func__, test_number);
#endif

#if 1

  Fetch_and_Add( (uint64_t*)&(atomic_counters[test_number]), 1);

  while ( atomic_counters[test_number] < barrier_count )
    //_delay_cycles(100)
      ;

#endif
#ifdef VERBOSE
  printf("(<) %s test=%d\n", __func__, test_number);
#endif

  return;
}

int MyLog10(unsigned num) {
  int result = 0;
  unsigned base = 10;
  while (num > base) {
    result++;
    base *= 10;
  }
  return result;
}

void encode(unsigned number, unsigned upper, char* str) {

  int log = MyLog10(upper);
  
  for (; log >= 0; log-- ) {
    str[log] = '0' + (number%10);
    number /= 10;
  }

  //str[log+1] = ' ';
}


int SimpleTest() {

  if (ProcessorID() > 0) {
    return 0;
  }

  int rc;

  rc = puts("Hello world!");

  if (rc <= 0) {
    return -1;
  }

  rc = puts("Hello again!");
  
  if (rc <= 0) {
    return -1;
  }

  return 0;
}


/*
 * This test issues a lot of messages on thread 0.  All
 * other threads do nothing
 */

int LotsOfMessages() {

  int i;
  int rc = 0;

  if (ProcessorID() > 0) {
    return 0;
  }

  
  char* env = fwext_getenv("LOTS.OF.MESSAGES.DISABLE");

  unsigned long disabled = (env != 0) ? fwext_strtoul(env, 0, 0) : 0;

  if ( disabled ) {
    printf( "(W) Test disabled.  Use environment variable LOTS.OF.MESSAGES.DISABLE=0 to enable.  [%s]\n", __func__);
    return 0;
  }

  unsigned long numberOfMessages = 100;

  if ( ( env = fwext_getenv("LOTS.OF.MESSAGES.NUM.MSGS" ) ) != 0 ) {
    numberOfMessages = fwext_strtoul(env, 0, 0);
  }

  char msg[7];

  for (i = 0; i < numberOfMessages; i++) {
    encode(i, 100000, msg );
    msg[5] = 0;
    rc = puts(msg);

    if (rc <= 0) {
      return -1;
    }
  }

  return 0;
}


/*
 * This test issues messages of varying lengths on thread 0.
 */

char VarLenBuffer[ 16 * 1024 ];

int VariableLengthMsgTest() {

  if (ProcessorID() > 0) {
    return 0;
  }

  
  char* env = fwext_getenv("VARIABLE.LENGTH.MSG.TEST.DISABLE");
  unsigned long disabled = (env != 0) ? fwext_strtoul(env, 0, 0) : 0;

  if ( disabled ) {
    printf("(W) Test disabled.  Use environment variable VARIABLE.LENGTH.MSG.TEST.DISABLE=0 to enable.  [%s]\n", __func__);
    return 0;
  }

  unsigned long maxSize = sizeof(VarLenBuffer) - 2;

  if ( ( env = fwext_getenv("VARIABLE.LENGTH.MSG.TEST.MAX.SIZE" ) ) != 0 ) {
    maxSize = fwext_strtoul( env, 0, 0 );
  }

  int i, j;

  for (i = 32; i < maxSize; i++ ) {

    for (j = 0; j < i; j++) {
      VarLenBuffer[j] = '0' + (i%10);
    }

    encode(i, 100000, VarLenBuffer);
    VarLenBuffer[5] = ' ';
    
    VarLenBuffer[i]   = 0;
    if ( puts(VarLenBuffer) <= 0 ) {
      return -1;
    }
  }
  return 0;
}


/*
 * This test issues messages on all threads and thus stresses the
 * internal locking of the mailbox code.
 */

int ThreadedTest() {

  char* env = fwext_getenv("THREADED.TEST.DISABLE");

  unsigned long disabled = (env != 0) ? fwext_strtoul(env, 0, 0) : 0;

  if ( disabled ) {
    if  ( ProcessorID() == 0 ) {
      printf("(W) Test disabled.  Use environment variable THREADED.TEST.DISABLE=0 to enable.  [%s]\n", __func__);
    }
    return 0;
  }

  unsigned long messagesPerThread = 10;

  if ( ( env = fwext_getenv("THREADED.TEST.MSGS.PER.THREAD") ) != 0 ) {
    messagesPerThread = fwext_strtoul(env, 0, 0);
  }


  char msg[32];

  strcpy(msg, "Hello from thread " );
  encode(ProcessorID(),100, msg + 18);
  msg[20]='-';

  int i;
  for (i = 0; i < messagesPerThread; i++) {
    encode(i,1000,msg+21);
    msg[24]=0;
    if ( puts(msg) <= 0 ) {
      return -1;
    }
  }
  return 0;
}

uint64_t ras_details[4];

int RASTest() {

  int i;

  if (ProcessorID() > 0)
    return 0;

  for (i = 0; i < sizeof(ras_details)/sizeof(ras_details[0]); i++) {
    ras_details[i] = i+1;
  }
  
  writeRASEvent( 0x12345678, sizeof(ras_details)/sizeof(ras_details[0]), ras_details);
  return 0;
}

int RASAsciiTest() {

  if (ProcessorID() > 0)
    return 0;
  
  fwext_getFwInterface()->writeRASString( 0x12345678, "This is only a test." );
  return 0;
}

/*
 * This test pushes lots of large messages through on multiple threads.
 * It stresses the buffer overrun logic in the firmware mailbox code.
 */

#define NUM_THREADS 68
#define BIG_MSG_SIZE (8*1024 + 1)
char BigMessageTestBuffer[NUM_THREADS][BIG_MSG_SIZE];

int BigMessageTest() {

  int i;
  unsigned thread_id = ProcessorID();

  char* env = fwext_getenv("BIG.MSG.TEST.DISABLE");

  unsigned long disabled = (env != 0) ? fwext_strtoul(env, 0, 0) : 0;

  if ( disabled ) {
    if ( ProcessorID() == 0 ) {
      printf("(W) Test disabled.  Use environment variable VARIABLE.LENGTH.MSG.TEST.DISABLE=0 to enable.  [%s]\n", __func__);
    }
    return 0;
  }

  //if (thread_id > 2) while (1);

  unsigned long numMessagesPerThread = 100;

  if ( ( env = fwext_getenv("BIG.MSG.TEST.MSGS.PER.THREAD") ) != 0 ) {
    numMessagesPerThread = fwext_strtoul( env, 0, 0 );
  }

  for (i = 0; i < BIG_MSG_SIZE-1; i++) {
    BigMessageTestBuffer[thread_id][i] = '0' + thread_id;
  }
  BigMessageTestBuffer[thread_id][BIG_MSG_SIZE-1] = 0;

  for (i = 0; i < numMessagesPerThread; i++) {
    puts(BigMessageTestBuffer[thread_id]);
  }
      
  return 0;
}

char HugeMessage[16*1024];

int HugeMessageTest() {

    int i, j;
  unsigned thread_id = ProcessorID();

  if ( thread_id != 0 ) {
      return 0;
  }

  char* env = fwext_getenv("HUGE.MSG.TEST.DISABLE");

  unsigned long disabled = (env != 0) ? fwext_strtoul(env, 0, 0) : 0;

  if ( disabled ) {
    if ( ProcessorID() == 0 ) {
      printf("(W) Test disabled.  Use environment variable HUGE.MSG.TEST.DISABLE=0 to enable.  [%s]\n", __func__);
    }
    return 0;
  }

  for ( j = 0; j < 32; j++ ) {

      int hugeLength=16*1024 - j;

      for (i = 0; i < hugeLength; i++) {
	  HugeMessage[i] = 'H';
      }
      HugeMessage[hugeLength-1] = 0;


      //int rc = puts(HugeMessage);
      int rc = fwext_getFwInterface()->putn(HugeMessage, hugeLength+1);

      printf("j=%d len=%d rc=%d\n", j, hugeLength, rc);
      if ( j <= 8 ) {
	  if ( rc != FW_TOO_BIG ) {
	      printf("(E) did not expect message to fit!\n");
	      return -__LINE__;
	  }
      }
      else {
	  if ( rc != (hugeLength+1) ) {
	      printf("(E) bad rc\n");
	      return -__LINE__;
	  }
      }


  }
      
  return 0;
}

Test_t ALL_TESTS[] = {
  SimpleTest,
  LotsOfMessages,
  VariableLengthMsgTest,
  ThreadedTest,
  BigMessageTest,
  HugeMessageTest,
  //RASTest,
  //RASAsciiTest,
};

int run_tests() {

  int t;
  int rc = 0;
  unsigned numberOfThreads = BgGetNumThreads();
  for (t = 0; (t < sizeof(ALL_TESTS) / sizeof(ALL_TESTS[0])) && (rc == 0); t++) {
    testBarrier( t, numberOfThreads ); 
    rc |= ALL_TESTS[t]();
  }

  if (rc == 0) {
    testBarrier( t, numberOfThreads );
  }
  return rc;
}

int test_main( void ) {

  int rc;

  rc = run_tests();

  //if (rc) {
    exit(rc);
    //}
  
  return 0;

}


