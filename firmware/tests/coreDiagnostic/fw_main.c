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


#include <firmware/include/Firmware.h>
#include "Firmware_internals.h"
#include <hwi/include/bqc/testint_inlines.h>
#include <hwi/include/bqc/l1p_dcr.h>

#include "fw_regs.h"


FW_State_t FW_STATE FW_State = { 0, };

uint64_t FW_LOCKS fw_locks[ BeDRAM_LOCK_COUNT ] = { 0, };
uint8_t  FW_STACK0 fw_space_for_stack0[2*FW_STACK_SIZE];
uint8_t  FW_STACKS fw_stacks[ NUM_HW_THREADS * FW_STACK_SIZE ] ALIGN_L1D_CACHE = { 0, };

volatile char     FW_SUART       fw_suart = 0;
volatile uint64_t FW_SUART       fw_padding[127] = {0, };
volatile int      FW_TERMSTATUS  fw_term  = 0;

Personality_t FW_PERSONALITY FW_Personality  = PERSONALITY_DEFAULT();

FW_InternalState_t FW_InternalState = {
  .nodeState = {
    .domain = { 
       { 0x1FFFF, 0, -1, 0x10ul },  
#ifndef FW_SINGLE_DOMAIN
       { 0x00000, 0, -1, 0x10ul },  
#endif
     },
  },
};

Firmware_Interface_t FW_Interface;

extern int testGprs( uint64_t pattern );
extern int testFprs( unsigned long* output, unsigned long* pattern );


volatile uint64_t testStatus[NUM_HW_THREADS] = { 0, };

unsigned long ALIGN_QUADWORD FloatingPointPattern[2][4] = {
  { 0x5555555555555555ul, 0x5555555555555555ul, 0x5555555555555555ul, 0x5555555555555555ul },
  { 0xAAAAAAAAAAAAAAAAul, 0xAAAAAAAAAAAAAAAAul, 0xAAAAAAAAAAAAAAAAul, 0xAAAAAAAAAAAAAAAAul }
};

unsigned long ALIGN_QUADWORD FloatingPointResults[NUM_HW_THREADS][32][4];

uint64_t GprPattern[2] = { 0x5555555555555555ull, 0xAAAAAAAAAAAAAAAAull };

#define SLEEP() mtspr( SPRN_TENC, 0x1 << ProcessorThreadID() )

void __NORETURN fw_main( void ) {

  uint64_t 
    status = 0, 
    i = 0,
    j = 0,
    p = 0,
    tid = ProcessorID();

  //if (ProcessorID() != 0 ) SLEEP(); // DEBUG ONLY
  // [TEST] if ( ProcessorID() == 13 ) while(1);
  // [TEST] if ( ProcessorID() == 37 ) while(1);
  // [TEST] if ( ProcessorID() == 0  ) while(1);

  testStatus[tid] = 0xDEADBEEF;

  /*
   * Perform a bit-striping test on the GPRs
   */

  for ( p = 0; p < 2; p++ ) {

    if ( ( status =  testGprs( GprPattern[p] ) ) != 0 ) {

      testStatus[tid] = 
	(p << 48) |
	(status << 32) |
	__LINE__;

      SLEEP();
    }
  }

  /*
   * Perform a bit-striping test on the FPRs.  There are two test patterns
   * of alternating bits.
   */

  for ( p = 0; p < 2; p++ ) {

    testFprs( &(FloatingPointResults[tid][0][0]), &(FloatingPointPattern[p][0]) );

    // [TEST] if ( tid == 43 ) FloatingPointResults[43][12][1] = 99;

    for ( i = 0; i < 32; i++ ) {
      for ( j = 0; j < 4; j++ ) {
	if ( FloatingPointResults[tid][i][j] != FloatingPointPattern[p][j] ) {
	  testStatus[tid] = 
	    (i << 48) |
	    (j << 32) |
	    __LINE__;
	  SLEEP();
	}
      }
    }
  }

  /*
   * Read the JTAG ID.  This is a simple test of the DCR ring.
   */

  uint64_t jtagId = TI_GetIDCODE();
  
  if ( ( jtagId != BGQ_JTAG_IDCODE_VALUE_BQC_DD1_DCR ) && ( jtagId != BGQ_JTAG_IDCODE_VALUE_BQC_DD2_DCR ) ) {
    if ( ( FW_Personality.Kernel_Config.NodeConfig & PERS_ENABLE_Mambo ) == 0 ) {
      testStatus[tid] = __LINE__;
      SLEEP();
    }
  }
  

  /*
   * Write and read L1P
   */

  uint64_t expected = L1P_DCR__INTERRUPT_STATE_A_CONTROL_HIGH__LOCAL_RING_set(1);
  DCRWritePriv( L1P_DCR( ProcessorCoreID(), INTERRUPT_STATE_A_CONTROL_HIGH ), expected );
  uint64_t actual = DCRReadPriv( L1P_DCR( ProcessorCoreID(), INTERRUPT_STATE_A_CONTROL_HIGH ) );
  if ( actual != expected ) {
    testStatus[tid] = __LINE__;
  }

  testStatus[tid] = 0xC00DF00DC00DF00Dull;

  /*
   * Every thread bumps the counter for it's own core.  This will
   * be used below to isolate failing cores.
   */

  BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_COREINIT( tid >> 2 ));

  

  /*
   * All threads bump the "present" counter.  The first thread in 
   * assumes control and begins monitoring the entire chip.  Other
   * threads have nothing left to do and just put themselves to
   * sleep.
   */

  if ( BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_PRESENT) > 0 ) { 
    SLEEP();
  }
  else {


    /*
     * Initialize mailbox services so we can issue a status code.
     */

    if ( fw_mailbox_init( ProcessorCoreID() ) != 0 ) {
      Terminate( - __LINE__ );
    }

    
    uint64_t timeout = GetTimeBase() + 10ull * 1000ull * FW_Personality.Kernel_Config.FreqMHz ; /* 10 milliseconds */

    // uint64_t timeout = GetTimeBase() + 10000;

    while ( ( BeDRAM_Read(BeDRAM_LOCKNUM_PRESENT) < 68 ) && ( GetTimeBase() < timeout ) )
      /* spin */
      ;

    if ( BeDRAM_Read(BeDRAM_LOCKNUM_PRESENT) < 68 ) {

      unsigned i ;
      unsigned status = 0;
      
      for ( i = 0; i < 17; i++ ) {
	if ( BeDRAM_Read( BeDRAM_LOCKNUM_COREINIT(i) ) < 4 ) {
	  printf("Core %d is bad\n", i);
	  printf("  Status: %016lX %016lX %016lX %016lX\n", testStatus[(i*4)+0], testStatus[(i*4)+1], testStatus[(i*4)+2], testStatus[(i*4)+3] );
	  status |= (1 << i);
	}
      }

      Terminate( status );
    }
	      
  }

  Terminate( 0 );

}  

void fw_machineCheckHandler( void ) {
  mtspr( SPRN_TENC, 0x1 << ProcessorThreadID() ); // Sleep
}





// Stubs

int fw_sim_writeRASEvent(uint32_t a, uint16_t b, uint64_t* c) { return -1; }
int fw_sim_writeRASString(uint32_t message_id, char* msg ) { return -1; }
int fw_sim_putn(const char* a, unsigned b) { return -1; }
int fw_mmu_remap(void) { return -1; }
void sim_exit(int status) { while(1);};
void fw_l1p_flushCorrectables(int x) {}
void fw_l2_flushCorrectables(int x) {}
void fw_ddr_flushCorrectables(int x) {}
void fw_nd_flushCorrectables(void) {}
void fw_mu_flushCorrectables(void) {}
void fw_pcie_shutdown(void) {}
