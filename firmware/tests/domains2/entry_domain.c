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
volatile Firmware_Interface_t*   _firmware_for_this_domain = 0;
Firmware_DomainDescriptor_t      _domain_descriptor;
unsigned                         _number_of_cores_in_this_domain = 0;
volatile uint64_t                _barrier_for_auxiliary_entry_point = 0;
volatile uint64_t                _barrier_for_traditional_entry_point = 0 ;
volatile int64_t                 _domain_leader = -1;


#define SET_STACKP() {							\
    size_t pStk = (size_t)((size_t)&(_stack_domain[ProcessorCoreID()][ProcessorThreadID()][STACK_SIZE-64])); \
    asm volatile ( "mr 1,%0;" : : "r" (pStk) : "r1", "memory" );	\
}

#define ABORT()     _firmware_for_this_domain->terminate(__LINE__)
#define FIRMWARE()  _firmware_for_this_domain
#define DD()        _domain_descriptor

void KTEXT_ENTRY testInterDomainMessaging_domain( void ) {

    int rc = 0;
    InterDomainMessage_t message;
    int timeout = 10;

    TRACE(("(>) %s\n", __func__));

    // Send a "hello" message to the other domain ... they will be doing the
    // same.

    message.hello.timestamp = GetTimeBase();
    do {
	TRACE(( "Sending message (timeout=%d)\n", timeout));
	rc = FIRMWARE()->sendMessageToDomain( (ProcessorID()==0) ? 1 : 0, (void*)&message, sizeof(InterDomainHello_t) );
    } while ( ( rc == FW_EAGAIN ) && ( --timeout > 0 ) );

    if ( rc != FW_OK ) {
	ERROR(( "(E) failure to deliver inter-domain message rc=%d timeout=%d\n", rc, timeout ));
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
	ERROR(( "(E) failure to recieve inter-domain message rc=%d timeout=%d\n", rc, timeout ));
	ABORT();
    }
       
    if ( source != ( (ProcessorID()==0) ? 1 : 0 ) ) {
	ERROR(( "(E) invalid source domain expected %d vs. %d actual\n", (0 ? 0 : 1), source ));
	ABORT();
    }

    if ( length != sizeof(InterDomainHello_t) ) {
	ERROR(( "(E) invalid message length expected %ld vs. %d actual\n", sizeof(InterDomainHello_t), length ));
	ABORT();
    }
	
    // The timestamp of the message should be in the past:
    if ( message.hello.timestamp > GetTimeBase() ) {
	ERROR(( "(E) invalid timestamp in message : %lX\n", message.hello.timestamp ));
	ABORT();
    }

    TRACE(("(<) %s\n", __func__));
}

int pollForShutdown() {

    char message_buffer[64];
    fw_uint32_t message_type = 0;

    if ( FIRMWARE()->pollInbox(message_buffer, &message_type, sizeof(message_buffer) ) == FW_EAGAIN ) {
	return 0;
    }

    switch ( message_type ) {

    case JMB_CMD2CORE_CONTROL_SYSTEM_REQUEST :
    {
	MailBoxPayload_ControlSystemRequest_t* msg = (MailBoxPayload_ControlSystemRequest_t*)message_buffer;
	switch ( msg->sysreq_id ) {
        case JMB_CTRLSYSREQ_SHUTDOWN_IO_LINK  :
        {
	    fw_uint64_t block_id = msg->details.shutdown_io_link.block_id;
	    FIRMWARE()->sendBlockStatus( JMB_BLOCKSTATE_IO_LINK_CLOSED, 1, &block_id );
	    break;
        }

        case JMB_CTRLSYSREQ_SHUTDOWN :
        {
	    FIRMWARE()->sendBlockStatus( JMB_BLOCKSTATE_HALTED, 0, 0 );
	    return 1;
	    break;
        }
	}
	break;
    }
    }

    return 0;
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
    if ( _domain_leader != ProcessorID() ) {
      SET_STACKP();
    }
  }

  //uint64_t domain = (uint64_t)arg;
  DEBUG(("(>) %s arg=%p : leader=%ld this=%d\n", __func__, arg, _domain_leader, ProcessorID() ));


  // Thread 0 on every node installs the interrupt vector:
  extern uint64_t _vec_domain_MCHK;
  if ( ProcessorThreadID() == 0 ) {
      DEBUG(("(*) Installing IVEC @%p\n", &_vec_domain_MCHK));
      if ( FIRMWARE()->installInterruptVector( &_vec_domain_MCHK, 0x400 ) != FW_OK ) {
	  ERROR(( "(E) bad rc from installInterruptVector\n" ));
	  ABORT();
      }
  }
 
  // Wait for all threads in this domain to arrive at this point:
  DEBUG(("(*) Before barrier numcores=%d\n", _number_of_cores_in_this_domain));
  barrierThreads( &_barrier_for_auxiliary_entry_point, _number_of_cores_in_this_domain * 4 );
  DEBUG(("(*) After barrier numcores=%d\n", _number_of_cores_in_this_domain));

  /*
   * Issue a "block initialized" message to the control system so that the block leaves 'B' state
   * and the boot does not time out.
   */

  if ( ProcessorID() == 0 ) {
      FIRMWARE()->sendBlockStatus( JMB_BLOCKSTATE_INITIALIZED, 0, 0 );
  }


  // The leaders test the message passing mechanism:
  if ( _domain_leader == ProcessorID() ) {
    testInterDomainMessaging_domain();
  }

  // Signal that we are done.  Do this only from thread 0 of core 0:
  if ( ProcessorID() == 0 ) {
    while ( pollForShutdown() == 0 );
  }

  // We are done:
  if ( _domain_leader == ProcessorID() ) {
      TRACE(("(!) Domain leader shutting down.\n"));
  }

  FIRMWARE()->exit(0);
}




void KTEXT_ENTRY mainCommon_domain( void ) {

    TRACE(("(>) %s this=%d\n", __func__, ProcessorID() ));
    int rc = 0;

    // Fetch the domain descriptor from firmware:

    if ( ( rc = FIRMWARE()->getDomainDescriptor( &_domain_descriptor ) ) != FW_OK ) {
	ERROR(("(E) bad return code (%d) from getDomainDescriptor()\n", rc ));
	ABORT();
    }



    // Count the number of active cores in this domain:

    uint32_t mask = 1;

    while ( mask ) {
	if ( DD().coreMask & mask ) {
	    _number_of_cores_in_this_domain++;
	}
	mask <<= 1;
    }

    TRACE(( "(*) %s : domain-descriptor={cores=%05X (%d), ddr=%016llX-%016llX entry=%016llX config=%016llX:%d}\n", __func__, DD().coreMask, _number_of_cores_in_this_domain, DD().ddrOrigin, DD().ddrEnd, DD().entryPoint, DD().configAddress, DD().configLength ));
 
   TRACE(("(<) %s this=%d numberOfCores=%d\n", __func__, ProcessorID(), _number_of_cores_in_this_domain ));
}

int KTEXT_ENTRY traditionalMain_domain( uint64_t domain ) {

  //int rc = 0;

  TRACE(( "(>) %s : domain:%ld fw:%p descriptor:%p\n", __func__, domain, FIRMWARE(), &_domain_descriptor ));

  // Execute common main code on the leader:
  if ( _domain_leader == ProcessorID() ) {
    mainCommon_domain();
  }

  // Wait for the domain to get here:
  barrierThreads( &_barrier_for_traditional_entry_point, _number_of_cores_in_this_domain * 4 );
  
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

  TRACE(( "(>) %s : fw:%p\n", __func__, FIRMWARE() ));

  
  mainCommon_domain();

 // Take the remaining threads 3 from our own core:
 
  rc =  FIRMWARE()->takeCPU( ProcessorCoreID(), 0xF ^ ( 1 << ProcessorThreadID() ),  auxiliaryEntryPoint_domain, (void*)0 );

  if ( rc != FW_OK ) {
      ERROR(( "(E) couldnt take auxiliary threads on domain leader (rc=%d)\n", rc ));
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
    ERROR(("I am here unexpetedly -> %s:%s:%d\n", __func__, __FILE__, __LINE__));
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

    testAndSet( &_domain_leader, (int64_t)-1, ProcessorID() );

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



