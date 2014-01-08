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
#include <hwi/include/bqc/BIC.h>
#include <hwi/include/bqc/BIC_inlines.h>
#include <hwi/include/bqc/pe_dcr.h>

#include <firmware/include/mailbox.h>
#include <firmware/include/personality.h>
#include <firmware/include/Firmware_Interface.h>
#include <firmware/include/BeDram_inlines.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "utils.h"

#define STACK_SIZE (32*1024)


#define KTEXT_ENTRY    __attribute__((__section__(".text.domain")))

char                             _stack_domain[17][4][STACK_SIZE] ALIGN_L2_CACHE /* = { 0, } */ ;
volatile Firmware_Interface_t*   _firmware_domain = 0;
Firmware_DomainDescriptor_t      _descriptor_domain;
unsigned                         _numberOfCores_domain = 0;
volatile uint64_t                _auxEntryPointBarrier_domain = 0 ;
volatile uint64_t                _traditionalEntryBarrier_domain = 0 ;
volatile int64_t                 _leader_domain = -1;


#define SET_STACKP() {							\
    size_t pStk = (size_t)((size_t)&(_stack_domain[ProcessorCoreID()][ProcessorThreadID()][STACK_SIZE-64])); \
    asm volatile ( "mr 1,%0;" : : "r" (pStk) : "r1", "memory" );	\
  }

#define ABORT()     _firmware_domain->terminate(__LINE__)
#define FIRMWARE()  _firmware_domain
#define DD()        _descriptor_domain

void KTEXT_ENTRY testInterDomainMessaging_domain( void ) {

	int rc = 0;
	InterDomainMessage_t message;
        int timeout = 10;

        printf("(>) %s\n", __func__);

        // Send a "hello" message to the other domain ... they will be doing the
	// same.

	message.hello.timestamp = GetTimeBase();
	do {
          TRACE(( "Sending message (timeout=%d)\n", timeout));
          rc = FIRMWARE()->sendMessageToDomain( (ProcessorID()==0) ? 1 : 0, (void*)&message, sizeof(InterDomainHello_t) );
        } while ( ( rc == FW_EAGAIN ) && ( --timeout > 0 ) );

        if ( rc != FW_OK ) {
          printf( "(E) failure to deliver inter-domain message rc=%d timeout=%d\n", rc, timeout );
          ABORT();
        }

        // Now retrieve the message sent from the other domain:
        unsigned source = 0;
	unsigned length = 0;

        timeout = 1000;
 
        do {
          TRACE(( "Retrieving message (timeout=%d)\n", timeout));
          rc = FIRMWARE()->retrieveMessage( &source, &message, &length );
        } while ( ( rc == FW_EAGAIN ) && ( --timeout > 0 ) );

        if ( rc != FW_OK ) {
	    printf( "(E) failure to recieve inter-domain message rc=%d timeout=%d\n", rc, timeout );
	    ABORT();
        }
       
        if ( source != ( (ProcessorID()==0) ? 1 : 0 ) ) {
          printf( "(E) invalid source domain expected %d vs. %d actual\n", (0 ? 0 : 1), source );
          ABORT();
        }

        if ( length != sizeof(InterDomainHello_t) ) {
          printf( "(E) invalid message length expected %ld vs. %d actual\n", sizeof(InterDomainHello_t), length );
          ABORT();
        }
	
	// The timestamp of the message should be in the past:
	if ( message.hello.timestamp > GetTimeBase() ) {
  	  printf( "(E) invalid timestamp in message : %lX\n", message.hello.timestamp );
          ABORT();
        }

        printf("(<) %s\n", __func__);
}

void KTEXT_ENTRY auxiliaryEntryPoint_domain( void* arg ) {

  //  +--------------------------------------------------------------------------------+
  //  | NOTE: This function represents the common entry point for all threads in the   |
  //  |       domain.  Thus it provides the high level implementation of the test code |
  //  |       that is executed on every thread.                                        |
  //  +--------------------------------------------------------------------------------+

  // If we are in takeCPU mode and we are not the leader thread, then we are still on 
  // the BeDRAM (tiny) stack.  Get off immediately.
  // @todo : move this into an aux functtion

  if ( ( FIRMWARE()->deprecated.personalityPtr->Kernel_Config.NodeConfig & PERS_ENABLE_TakeCPU ) != 0 ) { 
    if ( _leader_domain != ProcessorID() ) {
      SET_STACKP();
    }
  }

  //uint64_t domain = (uint64_t)arg;
  TRACE(("(>) %s arg=%p : leader=%ld this=%d\n", __func__, arg, _leader_domain, ProcessorID() ));


  // Thread 0 on every node installs the interrupt vector:
  extern uint64_t _vec_domain_MCHK;
  if ( ProcessorThreadID() == 0 ) {
      TRACE(("(*) Installing IVEC @%p\n", &_vec_domain_MCHK));
      if ( FIRMWARE()->installInterruptVector( &_vec_domain_MCHK, 0x400 ) != FW_OK ) {
	  printf( "(E) bad rc from installInterruptVector\n" );
	  ABORT();
      }
  }
 
  // Wait for all threads in this domain to arrive at this point:
  TRACE(("(*) Before barrier numcores=%d\n", _numberOfCores_domain));
  barrierThreads( &_auxEntryPointBarrier_domain, _numberOfCores_domain * 4 );
  TRACE(("(*) After barrier numcores=%d\n", _numberOfCores_domain));


  // The leaders test the message passing mechanism:
  if ( _leader_domain == ProcessorID() ) {
    testInterDomainMessaging_domain();
  }

  // We are done:
  if ( _leader_domain == ProcessorID() ) {
      printf("(!) Domain leader shutting down.\n");
  }

  FIRMWARE()->exit(0);
}




void KTEXT_ENTRY mainCommon_domain( void ) {

    TRACE(("(>) %s this=%d\n", __func__, ProcessorID() ));
    int rc = 0;

    // Fetch the domain descriptor from firmware:

    if ( ( rc = FIRMWARE()->getDomainDescriptor( &_descriptor_domain ) ) != FW_OK ) {
	printf("(E) bad return code (%d) from getDomainDescriptor()\n", rc );
	ABORT();
    }



    // Count the number of active cores in this domain:

    uint32_t mask = 1;

    while ( mask ) {
	if ( DD().coreMask & mask ) {
	    _numberOfCores_domain++;
	}
	mask <<= 1;
    }

    printf( "(*) %s : domain-descriptor={cores=%05X (%d), ddr=%016llX-%016llX entry=%016llX}\n", __func__, DD().coreMask, _numberOfCores_domain, DD().ddrOrigin, DD().ddrEnd, DD().entryPoint );
 
   TRACE(("(<) %s this=%d numberOfCores=%d\n", __func__, ProcessorID(), _numberOfCores_domain ));
}

int KTEXT_ENTRY traditionalMain_domain( uint64_t domain ) {

  //int rc = 0;

  TRACE(( "(>) %s : domain:%ld fw:%p descriptor:%p\n", __func__, domain, FIRMWARE(), &_descriptor_domain ));

  // Execute common main code on the leader:
  if ( _leader_domain == ProcessorID() ) {
    mainCommon_domain();
  }

  // Wait for the domain to get here:
  barrierThreads( &_traditionalEntryBarrier_domain, _numberOfCores_domain * 4 );
  
  // Finally, jump to the same place that all other threads are (rendesvous).

  auxiliaryEntryPoint_domain( (void*)0 );

  // should never get here

  while (1)
	/* spin */
	;

}

int KTEXT_ENTRY takeCpuMain_domain( void ) {
 
  //  +-------------------------------------------------------------------------+
  //  |  NOTE: this function is the domain's "leader" thread for the test.  It  |
  //  |        will pull other threads out of firmware and thus launch the      |
  //  |        domain's test.                                                   |
  //  +-------------------------------------------------------------------------+

  int rc = 0;

  printf( "(>) %s : fw:%p\n", __func__, FIRMWARE() );

  
  mainCommon_domain();

 // Take the remaining threads 3 from our own core:
 
  rc =  FIRMWARE()->takeCPU( ProcessorCoreID(), 0xF ^ ( 1 << ProcessorThreadID() ),  auxiliaryEntryPoint_domain, (void*)0 );

  if ( rc != FW_OK ) {
      printf( "(E) couldnt take auxiliary threads on domain leader (rc=%d)\n", rc );
      ABORT();
  }

  // And now take all remaining threads from remaining cores.  Attempts to steal
  // cores/threads from the other domain should fail:

  int cpu;

  for ( cpu = 0; cpu < 17; cpu++ ) {

    if ( cpu == ProcessorCoreID() ) 
       continue;

    rc = FIRMWARE()->takeCPU( cpu, 0xF, auxiliaryEntryPoint_domain, (void*)0 );

    if ( rc != FW_OK ) {
      if ( ( DD().coreMask & (1<<cpu) ) == 0 ) {
        rc = 0;
      }
    }	
  }

  // Finally, jump to the same place that all other threads are (rendesvous).

  auxiliaryEntryPoint_domain( (void*)0 );

  // should never get here
  
  while (1)
	/* spin */
	;
}






__C_LINKAGE void __NORETURN KTEXT_ENTRY crash_domain( int status ) {
  printf("I am here unexpetedly -> %s:%s:%d\n", __func__, __FILE__, __LINE__);
  ABORT();
  while (1)
    /* spin */
    ;
}


//
// The entry point for C code (via start.S via firmware)
//
//  Note: (1) We're still running on the Firmware's 1KB stack, be careful.
//        (2) Firmware owns interrupt vectors (IVPR) until we're ready to handle interrupts.
//

__C_LINKAGE void __NORETURN _enter_domain( Firmware_Interface_t *fw_interface ) {

    //int threadID       = ProcessorID(); // 0..67
    int processorIndex = ProcessorCoreID(); // 0..16
    int threadIndex    = ProcessorThreadID();    // 0..3

    OPD_Entry_t* opd_main = 0;

    if ( ( fw_interface->deprecated.personalityPtr->Kernel_Config.NodeConfig & PERS_ENABLE_TakeCPU ) == 0 ) {
        opd_main = (OPD_Entry_t*)traditionalMain_domain;
    }
    else {
        opd_main = (OPD_Entry_t *)takeCpuMain_domain;
    }

    // Nominate a domain leader (first one in wins!).  In takeCPU mode, there is really
    // no race here ... but in traditional (thundering herd) mode, all threads may arrive
    // here at around the same time.

    testAndSet( &_leader_domain, (int64_t)-1, ProcessorID() );

     FIRMWARE() = fw_interface;
     setFirmware( fw_interface );


    // Relaunch threads on their own stack

    do {
      
      OPD_Entry_t *opd_exit = (OPD_Entry_t *)crash_domain;

      size_t pStk = (size_t)((size_t)&(_stack_domain[processorIndex][threadIndex][STACK_SIZE-64]));

      mtspr( SPRN_SRR0_IP,   opd_main->Entry_Addr );
      mtspr( SPRN_SRR1_MSR,  MSR_KERNEL );
      isync();
      
      asm volatile (
		     "mtlr  %2;"
                     "li    0,0;"
                     "mr    1,%0;"
                     "mr    2,%1;"
                     "li    3,%3;"
                     "rfi;"
                     "nop;"
                     :
                     : "r" (pStk),
                     "r" (opd_main->TOC_Addr),
                     "r" (opd_exit->Entry_Addr),
	             "i" (0)
                     : "r0", "r1", "r2", "r3", "lr", "cc", "memory" );
    } while (1);
}



