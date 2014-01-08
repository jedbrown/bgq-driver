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
#include <hwi/include/bqc/testint_dcr.h>
#include <hwi/include/bqc/BIC_inlines.h>
#include <hwi/include/bqc/testint_inlines.h>
#include <hwi/include/bqc/testint_dcr.h>
#include "fw_pers.h"
#include "Firmware_internals.h"
#include "Firmware_RAS.h"
#include "fw_regs.h"
#include <hwi/include/bqc/dr_arb_dcr.h>
#include <hwi/include/bqc/ddr.h>

FW_State_t FW_STATE FW_State = { 0, };

uint64_t FW_LOCKS fw_locks[ BeDRAM_LOCK_COUNT ] = { 0, };
uint8_t  FW_STACK0 fw_space_for_stack0[4*FW_STACK_SIZE];
uint8_t  FW_STACKS fw_stacks[ NUM_HW_THREADS * FW_STACK_SIZE ] ALIGN_L1D_CACHE = { 0, };

volatile char     FW_SUART       fw_suart = 0;
volatile uint64_t FW_SUART       fw_padding[127] = {0, };
volatile int      FW_TERMSTATUS  fw_term  = 0;

uint64_t pass1_threadactive0;
uint64_t pass1_threadactive1;

FW_InternalState_t FW_InternalState = {
  .nodeState = {
    .domain = { 
	    { 0x1FFFF, 0, -1, 0x10ul, 0 },  
#ifndef FW_SINGLE_DOMAIN
	    { 0x00000, 0, -1, 0x10ul, 0 },  
#endif
     },
  },
};


uint64_t fw_getDomainID( uint64_t core ) {

  uint64_t i;
  uint64_t mask = (uint64_t)1 << core;

  for ( i = 0; i < FW_ARRAY_SIZE(FW_InternalState.nodeState.domain); i++ ) {
    if ( ( FW_InternalState.nodeState.domain[i].coreMask & mask ) != 0 ) {
      return i;
    }
  }
  
  return (uint64_t)-1;

}

uint64_t fw_getThisDomainID() {
  return fw_getDomainID( ProcessorCoreID() );
}


int fw_thisThreadIsDomainLeader() {

  // HW threads 1-3 are never leaders:

  if ( ProcessorThreadID() != 0 ) {
    return 0;
  }

  // Are there any bits to the right of ours in our own
  // domain's core mask?

  uint64_t domainID = fw_getThisDomainID();
  uint64_t mask = ( ( (uint64_t) 1 ) << ProcessorCoreID() ) - 1;

  return 
    ( domainID < FW_MAX_DOMAINS ) && 
    ( ( FW_InternalState.nodeState.domain[domainID].coreMask & mask ) == 0 )
    ;
}

int fw_getDomainDescriptor( Firmware_DomainDescriptor_t* buffer ) {

  uint64_t domainID = fw_getThisDomainID();

  if ( domainID >= FW_MAX_DOMAINS ) {
    return FW_INVALID;
  }

  memcpy( buffer, &(FW_InternalState.nodeState.domain[domainID]), sizeof(Firmware_DomainDescriptor_t) );
  return FW_OK;
}

int fw_sendMessageToDomain( unsigned targetDomain, void* message, unsigned length ) {

#ifdef FW_SINGLE_DOMAIN
    return FW_ERROR;
#else

  int rc = FW_OK; // assume

  if ( targetDomain >= FW_MAX_DOMAINS ) {
    return FW_INVALID;
  }

  FW_MessageBox_t* messageBox = &( FW_InternalState.nodeState.messageBox[targetDomain] );

  if ( length > sizeof( messageBox->data ) ) {
    return FW_INVALID;
  }

  if ( fw_semaphore_down_w_timeout( BeDRAM_LOCKNUM_MSG_BOX, 100 ) != 0 ) {
    return FW_EAGAIN;
  }

  if ( messageBox->busy != 0 ) {
    rc = FW_EAGAIN;
    goto sendMessageDone;
  }

  messageBox->busy   = 1;
  messageBox->length = length;
  messageBox->from   = fw_getThisDomainID();
  memcpy( messageBox->data, message, length );
  rc = FW_OK;

 sendMessageDone:
  fw_semaphore_up( BeDRAM_LOCKNUM_MSG_BOX );
  return rc;
#endif
}

int fw_retrieveMessage( unsigned* sourceDomain, void* message, unsigned* length ) {

#ifdef FW_SINGLE_DOMAIN
    return FW_ERROR;
#else
    int rc = FW_OK;

    FW_MessageBox_t* messageBox = &( FW_InternalState.nodeState.messageBox[ fw_getThisDomainID() ] );

    if ( fw_semaphore_down_w_timeout( BeDRAM_LOCKNUM_MSG_BOX, 100 ) != 0 ) {
	return FW_EAGAIN;
    }

    if ( messageBox->busy == 0 ) { 
	rc = FW_EAGAIN; // The message box is empty
	goto retrieveMessageDone;
    }

    messageBox->busy   = 0;
    *length            = messageBox->length;
    *sourceDomain      = messageBox->from;
    memcpy( message, messageBox->data, messageBox->length );
    rc = FW_OK;

retrieveMessageDone:
    fw_semaphore_up( BeDRAM_LOCKNUM_MSG_BOX );
    return rc;
#endif
}

inline int fw_SPI_init() {

    // SPI calls in spi/include/kernel/location.h expect the hwthread index
    // to be in SPRG7.  Set that register here once and for all.
    mtspr(SPRN_SPRG7, ProcessorID());

    return 0;
}

int fw_takeCPU_init( void ) {
  
    unsigned n;

    uint8_t flags;


    /*
     * Construct the core mask by inspecting the thread active bits
     * of the testint DCRs.  A core is considered active if one or more
     * of its threads are active.  Note that the testint DCRs are
     * in big endian order, whereas the node state core mask is in
     * little endian.
     */

    // @todo : this is currently done only in the takeCPU path in order
    //         to save simulation cycles.

    int core;
    uint64_t threadActive = DCRReadPriv( TESTINT_DCR(THREAD_ACTIVE0) );
    FW_InternalState.nodeState.coreMask = 0;

    for ( core = 0; core < 16; core++ ) {
	if ( ( threadActive & ( 0xF000000000000000ull >> (4*core) ) ) != 0 ) {
	    FW_InternalState.nodeState.coreMask |= (1 << core);
	}
    } 

    threadActive = DCRReadPriv( TESTINT_DCR(THREAD_ACTIVE1) );
    if ( ( threadActive & 0xF000000000000000ull ) != 0 ) {
	FW_InternalState.nodeState.coreMask |= (1 << 16 );
    }

    FW_InternalState.nodeState.domain[0].coreMask &= FW_InternalState.nodeState.coreMask;
    FW_InternalState.nodeState.domain[1].coreMask &= FW_InternalState.nodeState.coreMask;

    if (FW_InternalState.nodeState.domain[0].ddrEnd == -1 ) {
	FW_InternalState.nodeState.domain[0].ddrEnd = (uint64_t)FW_Personality.DDR_Config.DDRSizeMB * 1024ull * 1024ull  -  1;
    }

    if (FW_InternalState.nodeState.domain[1].ddrEnd == -1 ) {
	FW_InternalState.nodeState.domain[1].ddrEnd = (uint64_t)FW_Personality.DDR_Config.DDRSizeMB * 1024ull * 1024ull  -  1;
    }

    if ( PERS_ENABLED(PERS_ENABLE_TakeCPU) ) {
	flags = 0;
    }
    else {
	flags = FW_CORE_STATE_THD_ACTIVE_ALL;
    }

    for (n = 0; n < NUM_CORES; n++) {
	FW_InternalState.coreState[n].flags = flags;
	FW_InternalState.coreState[n].arg   = (void*)0;
    }

    FW_InternalState.coreState[0].flags |= FW_CORE_STATE_THD_ACTIVE(0); // The primordial thread is taken

  
    fw_semaphore_init( BeDRAM_LOCKNUM_TAKECPU, 1 );

    // Initialize the message box lock:
    fw_semaphore_init( BeDRAM_LOCKNUM_MSG_BOX, 1 );

    return 0;
}


void __NORETURN  fw_takeCPU_spin( void ) {

    // In order to support Linux boot scenarios in Mambo, we try to be efficient with
    // our thread usage.  If no threads are active on a given core, we will freeze 
    // threads 1-3 now, and thaw them all if *any* thread on the core is activated.

    if ( BeDRAM_Read( BeDRAM_LOCKNUM_TAKECPU_SIG( ProcessorCoreID() ) ) == 0 ) {

	if ( ProcessorThreadID() == 0 ) {
      
	    // Wait for the signal from takeCPU that one or more threads are being activated
	    // on this core.

	    while ( BeDRAM_Read( BeDRAM_LOCKNUM_TAKECPU_SIG( ProcessorCoreID() ) ) == 0 )
		/* spin */
		fw_pdelay(1600)
		    ;

	    // Thaw all hardware threads on this core:
	
	    mtspr( SPRN_TENS, 0xF );
	}
	else {
	    mtspr( SPRN_TENC, 1 << ProcessorThreadID() ); // put ourself to sleep
	}
    }


    while (1) {
    
	if ( FW_InternalState.coreState[ProcessorCoreID()].flags & FW_CORE_STATE_THD_ACTIVE(ProcessorThreadID()) ) {

	    // This thread has been "taken" ... transition to its entry point:

	    fw_l1p_unmaskCorrectableErrors();      

	    // ----------------------------------------------------------------------- //
	    // @hypervisor This code would have to transition into guest mode if/when  //
	    //             firmware supports kernels as guests.                        //
	    // ----------------------------------------------------------------------- //

	    OPD_Entry_t* ep         = (OPD_Entry_t*)FW_InternalState.coreState[ProcessorCoreID()].entryPoint;
	    uint64_t     kernel_MSR = MSR_KERNEL_DISABLED;
	    void*        arg        = FW_InternalState.coreState[ProcessorCoreID()].arg;
      
	    mtspr( SPRN_SRR0_IP,  ep->Entry_Addr );
	    mtspr( SPRN_SRR1_MSR, kernel_MSR );

	    asm volatile ("mr 2,%0;"
			  "mr 3,%1;"
			  "rfi;" 
			  : 
			  : "r" (ep->TOC_Addr), "r" (arg)
			  : "memory" );

	    /* Should never get here */
	    // @todo
	}
    }
}
 
int fw_takeCPU( unsigned coreNumber, unsigned threadMask, void (*entryPoint)(void*), void* arg ) {

    if ( coreNumber >= NUM_CORES ) {
	return FW_ERROR;
    }
  
    if ( threadMask & ~FW_CORE_STATE_THD_ACTIVE_ALL ) {
	return FW_ERROR;
    }

    if ( fw_getDomainID( coreNumber ) != fw_getThisDomainID() ) { // attempting to grab a core from another domain?
	return FW_ERROR;
    }

    if ( fw_semaphore_down_w_timeout(BeDRAM_LOCKNUM_TAKECPU, 200ul) != 0 ) {
	return FW_EAGAIN;
    }

    // We have arrived at the point where correctable errors may be enabled:

    fw_ddr_unmaskCorrectableErrors();
    fw_l1p_unmaskCorrectableErrors();
    fw_l2_unmaskCorrectableErrors();

    int rc = 0;

    if ( ( threadMask & FW_InternalState.coreState[coreNumber].flags ) != 0 ) {

	// At least one requested thread has already been activated.  Punt on the
	// entire request.

	rc = FW_ERROR;
	goto fw_takeCPU_end;
    }

    FW_InternalState.coreState[coreNumber].entryPoint = entryPoint;
    FW_InternalState.coreState[coreNumber].flags     |= threadMask;
    FW_InternalState.coreState[coreNumber].arg        = arg;
  
    rc = 0;


    // Signal that at least one thread on this core has been taken:

    BeDRAM_Write( BeDRAM_LOCKNUM_TAKECPU_SIG( coreNumber ), (uint64_t) 1 );

    // This takes care of the scenario where threads 1-3 get woken up on core 0.  However
    // it is more general in that it doesn't assume that core 0/thread 0 is the primordial
    // thread:
  
    if ( ProcessorCoreID() == coreNumber ) {
	mtspr( SPRN_TENS, threadMask ); 
    }
  
fw_takeCPU_end:
    fw_semaphore_up(BeDRAM_LOCKNUM_TAKECPU); // Release the lock
    return rc;
}


void fw_returnCPU( void ) {

    // Acquire the take CPU lock and twiddle our own run state to "not taken":

    fw_semaphore_down(BeDRAM_LOCKNUM_TAKECPU);
    FW_InternalState.coreState[ProcessorCoreID()].flags &= ~FW_CORE_STATE_THD_ACTIVE(ProcessorThreadID());  

    // If no threads are active on this core, we clear the signal 
    if ( ( FW_InternalState.coreState[ProcessorCoreID()].flags & FW_CORE_STATE_THD_ACTIVE_ALL ) == 0 ) {
	BeDRAM_ReadDecSat( BeDRAM_LOCKNUM_TAKECPU_SIG( ProcessorCoreID() ) );
    }

    fw_semaphore_up(BeDRAM_LOCKNUM_TAKECPU);

    // @todo : re-establish Firmware stack here

    // Now spin
    fw_takeCPU_spin();
}

int fw_termCheck( void ) {
  
    int rc = 0;
  
    if ( ProcessorID() == 0 ) {
	fw_flushAllRasHistory();
    }
   
    return rc;
}


int fw_A2_init( void ) {

    if ( PERS_ENABLED(PERS_ENABLE_A2_Errata) ) {

	// XUCR0[CLKG_CTL] <- 0b01100  	[HW088506, HW083670, Errata 164, Errata 88]
    
	uint64_t xucr0 = mfspr(SPRN_XUCR0);

	xucr0 &= ~XUCR0_CLKG_CTL(-1);
	xucr0 |=  XUCR0_CLKG_CTL(0x0C);

	mtspr( SPRN_XUCR0, xucr0 );

	// IUCR0[BP_BCLR_EN] <- 0

	uint64_t iucr0 = mfspr(SPRN_IUCR0);
	iucr0 &= ~IUCR0_BP_BCLR_EN;
	mtspr( SPRN_IUCR0, iucr0 );
    
    }

    if ( PERS_ENABLED(PERS_ENABLE_A2_IU_LLB) ) {

	// Recommended settings from the A2 team:
	//   IULLCR[LL_EN] 		= 1
	//   IULLCR[LL_HOLD_DELAY] 	= 0b10_0000

	uint64_t iullcr = mfspr( SPRN_IULLCR );

	iullcr &= ~IULLCR_LL_HOLD_DLY(-1);
	iullcr |= (IULLCR_IULL_EN | IULLCR_LL_HOLD_DLY(0x20) );

	mtspr( SPRN_IULLCR, iullcr );
    }

    return 0;
}


int fw_A2thread_init( void ) {

    if ( PERS_ENABLED(PERS_ENABLE_A2_XU_LLB) ) {

	// Recommended settings from the A2 team:
	// XUCR1[LL_EN] = 1
	// XUCR1[LL_TB_SEL] = 0b111

	uint64_t xucr1 = mfspr( SPRN_XUCR1 );

	xucr1 &= ~XUCR1_LL_TB_SEL(-1);
	xucr1 |= (XUCR1_LL_EN | XUCR1_LL_TB_SEL_27 );

	mtspr( SPRN_XUCR1, xucr1 );
    }

    return 0;

}



extern void exit(int);

int fw_backdoor_test(void* arg) {

    int rc = 0;

    printf( "(>) %s [%s:%d]\n", __func__, __FILE__, __LINE__ );

#if 0
  
    // cram a message into the inbox:
  
    extern uint8_t* fw_MBox2Core;

    printf("inbox:%lX\n", (uint64_t)fw_MBox2Core);
    char* ptr = (char*)0x000003ffffff2dc0ull; //fw_MBox2Core;

    volatile MailBoxHeader_t* hdr = (volatile MailBoxHeader_t*)ptr;

    hdr->usCmd = JMB_CMD2CORE_CONTROL_SYSTEM_REQUEST;
    hdr->usPayloadLen = sizeof(MailBoxPayload_ControlSystemRequest_t);
    hdr->usID = 0;
    hdr->usCrc = 0;

    ptr += sizeof(MailBoxHeader_t);

    volatile MailBoxPayload_ControlSystemRequest_t* sysreq = (volatile MailBoxPayload_ControlSystemRequest_t*)ptr;

    sysreq->sysreq_id = JMB_CTRLSYSREQ_SHUTDOWN_IO_LINK;
    sysreq->details.shutdown_io_link.block_id = 0x12345678;

    TESTINT_DCR_PRIV_PTR->mailbox_reg2[0] += sizeof(MailBoxHeader_t) + sizeof(MailBoxPayload_ControlSystemRequest_t);
    ppc_msync();

#endif

#if 0
    extern int fw_pcie_machineCheckHandler(uint64_t*);
    uint64_t status[3];
    fw_pcie_machineCheckHandler(status);
    return 0;
#endif

#if 0
    rc = fw_mailbox_perform_section_crc( (MailBoxPayload_crcEntry_t*) arg, 0 );
#endif

#if 0

    // Drive CRC32 code:
    uint64_t* args = (uint64_t*)arg;

    uint64_t start = GetTimeBase();
    uint64_t crc = Crc32n( args[0], (unsigned char*)args[1], args[2] );
    uint64_t end = GetTimeBase();
    printf("crc=%lX time=%ld\n", crc, (end-start));
    rc = 0;
#endif

#if 0
    int fw_nd_machineCheckHandler( uint64_t status[] );
    fw_nd_machineCheckHandler( 0);
#endif


    printf( "(<>) %s rc=%d [%s:%d]\n", __func__, rc, __FILE__, __LINE__ );
    return rc;
}

int fw_mailbox_pollInbox_wrapper( void* message, fw_uint32_t* messageType, unsigned bufferLength ) {

    int rc = fw_mailbox_pollInbox( message, messageType, bufferLength );

    if ( TRACE_ENABLED(TRACE_Entry) ) {
	if ( rc != FW_EAGAIN ) {
	    switch ( *messageType ) {
	    case JMB_CMD2CORE_STDIN :
		printf("(*) %s STDIN message=%s buf-len=%d rc=%d\n", __func__, (char*)message, bufferLength, rc );
		break;
	
	    case JMB_CMD2CORE_CONTROL_SYSTEM_REQUEST : {
		MailBoxPayload_ControlSystemRequest_t* sysreq = (MailBoxPayload_ControlSystemRequest_t*)message;
		printf("(*) %s CONTROL_SYS_REQ id=%X block=%d buf-len=%d, rc=%d\n",__func__, sysreq->sysreq_id, sysreq->details.shutdown_io_link.block_id, bufferLength, rc);
		break;
	    }

	    default :  {
		printf("(*) %s ???? rc=%d messageType=%d buffer-len=%d\n", __func__, rc, *messageType, bufferLength);
	    }
	    }
    
	}
    }
    return rc;
}

int fw_mailbox_sendBlockStatus_wrapper(fw_uint16_t status, fw_uint16_t numArgs, fw_uint64_t args[] ) {

    int rc = fw_mailbox_sendBlockStatus( status, numArgs, args );
    
    if ( TRACE_ENABLED(TRACE_Entry) ) {
	printf("(*) %s rc=%d status=%d numArgs=%d arg[0]=%lld\n", __func__, rc, status, numArgs, (numArgs>0)?args[0]:-1);
    }

    return rc;
}

int fw_mailbox_barrierWrapper( fw_uint64_t timeout ) {
    return fw_mailbox_barrier( timeout, 0 );
}

int fw_ThreadPriority_VeryHigh() {
    ThreadPriority_VeryHigh();
    return FW_OK;
}

int fw_ThreadPriority_MediumHigh() {
    ThreadPriority_MediumHigh();
    return FW_OK;
}


Firmware_Interface_t FW_Interface = 
  {
    .Crc                    = 0,                     // Crc via crc32n()
    .Version                = BGQ_FIRMWARE_VERSION, // Version
    .exit                   = exit,
    .getPersonality         = fw_getPersonality,
    .isIoNode               = fw_isIoNode,
    .putn                   = fw_putn,
    .terminate              = Terminate,
    .writeRASEvent          = fw_writeRASEvent,
    .writeRASString         = fw_writeRASString,
    .takeCPU                = fw_takeCPU,
    .returnCPU              = fw_returnCPU,
    .termCheck              = fw_termCheck,
    .installInterruptVector = fw_installInterruptVector,    
    .getDomainDescriptor    = fw_getDomainDescriptor,
    .sendMessageToDomain    = fw_sendMessageToDomain,
    .retrieveMessage        = fw_retrieveMessage,
    .barrier                = fw_mailbox_barrierWrapper,
    .get                    = fw_mailbox_get,
    .sendBlockStatus        = fw_mailbox_sendBlockStatus_wrapper,
    .pollInbox              = fw_mailbox_pollInbox_wrapper,
    .flushRasBuffers        = fw_flushAllRasHistory,
    .pollOutbox             = fw_mailbox_pollOutbox,
    .setTLBTable            = fw_A2_setTLBTable,
    .deprecated = 
    { 
      .personalityPtr = &FW_Personality,
      .sendReadyMessage = fw_mailbox_sendReadyMessage,
      .backdoorTest = fw_backdoor_test,
      .machineCheckHandler =  fw_machineCheckHandler,
    }
  };


uint16_t threadStatus[68];

#ifdef MEASURE_FW_BOOT_TIMES
FW_StopWatch_t _fw_stopwatch;
#endif
typedef struct _FW_InitEntry_t {
    int (*init)(void);
    fw_uint64_t mask;
} FW_InitEntry_t;

#define PRIMORDIAL_THREAD PERS_ENABLE__RESRVD_59
#define CORE_THREAD_ZERO  PERS_ENABLE__RESRVD_60

FW_InitEntry_t INIT_TABLE[] = {
    { fw_ThreadPriority_VeryHigh,      CORE_THREAD_ZERO | 0 },
    { fw_dcr_arbiter_init,             CORE_THREAD_ZERO | PRIMORDIAL_THREAD | PERS_ENABLE_DrArbiter },
    { fw_mailbox_init_on_core_0,       CORE_THREAD_ZERO | PERS_ENABLE_Mailbox },
    { fw_A2_init,                      CORE_THREAD_ZERO | 0 },
    { fw_testint_init ,                CORE_THREAD_ZERO | PRIMORDIAL_THREAD | PERS_ENABLE_TestInt },
    { fw_interrupts_init,              CORE_THREAD_ZERO | PRIMORDIAL_THREAD | PERS_ENABLE_BIC }, 
    { fw_devbus_init,                  CORE_THREAD_ZERO | PERS_ENABLE_DevBus },
    { fw_bedram_init,                  CORE_THREAD_ZERO | PRIMORDIAL_THREAD },
    { fw_ddr_init,                     CORE_THREAD_ZERO | PERS_ENABLE_DDR },
    { fw_l1p_init,                     CORE_THREAD_ZERO | PERS_ENABLE_L1P },
    { fw_l2_init,                      CORE_THREAD_ZERO | PRIMORDIAL_THREAD | PERS_ENABLE_L2 },
    { fw_l2_central_init,              CORE_THREAD_ZERO | PRIMORDIAL_THREAD | PERS_ENABLE_L2 },
    { fw_l2_counter_init,              CORE_THREAD_ZERO | PRIMORDIAL_THREAD | PERS_ENABLE_L2 },
    { fw_l2_preload ,                  CORE_THREAD_ZERO | PRIMORDIAL_THREAD | PERS_ENABLE_L2Only },
    { fw_serdes_init,                  CORE_THREAD_ZERO | PRIMORDIAL_THREAD | PERS_ENABLE_SerDes },
    { fw_mu_resetRelease,              CORE_THREAD_ZERO | PRIMORDIAL_THREAD | PERS_ENABLE_MU },
    { fw_nd_resetRelease,              CORE_THREAD_ZERO | PRIMORDIAL_THREAD | PERS_ENABLE_ND },
    { fw_nd_init_global_barrier,       CORE_THREAD_ZERO | PRIMORDIAL_THREAD | PERS_ENABLE_ND },
    { fw_pcie_init,                    CORE_THREAD_ZERO | PRIMORDIAL_THREAD | PERS_ENABLE_PCIe },
    { fw_ms_genct_init,                CORE_THREAD_ZERO | PRIMORDIAL_THREAD | PERS_ENABLE_MSGC },
    { fw_envmon_init,                  CORE_THREAD_ZERO | PRIMORDIAL_THREAD | PERS_ENABLE_EnvMon },
    { fw_upc_init,                     CORE_THREAD_ZERO | PRIMORDIAL_THREAD | PERS_ENABLE_UPC },
    { fw_wu_init,                      CORE_THREAD_ZERO | PRIMORDIAL_THREAD | PERS_ENABLE_Wakeup },
    { fw_takeCPU_init,                 CORE_THREAD_ZERO | PRIMORDIAL_THREAD },
    { fw_ThreadPriority_MediumHigh,    CORE_THREAD_ZERO | 0 },
    { fw_A2thread_init,                0 },
    { fw_mmu_init,                     0 },
    { fw_fpu_init,                     PERS_ENABLE_FPU },
    { fw_SPI_init,                     0 },
    { fw_sync_timebase,                PERS_ENABLE_TimeSync },
    { fw_ddr_flush_fifos,              PERS_ENABLE_DDR }
};


// if ( TESTINT_DCR_PRIV_PTR->ti_interrupt_state__state != 0 ) { while (1); }

//
// Enter here from fw_start.S.  Initialize/setup BQC, then call main(), which is either
//  a) main.c in this directory which launches the kernel, or
//  b) a firmware-extension testcase supplied main program.
//
void __NORETURN fw_main( void ) {

  int rc = 0;
  int processorThreadId = ProcessorThreadID(); // 0..3
  int primordialThread = ( (ProcessorCoreID() == 0) && (ProcessorThreadID() == 0) );
  uint64_t config = FW_Personality.Kernel_Config.NodeConfig;
  uint64_t repropass = 0;
  uint64_t x;

#if 0

  // This piece of code is useful when investigating stack overflow.  It scribbles
  // a fixed value into the extended primordial stack.  Thus, it can be used in
  // conjunction with dump_mem to locate the high water mark of the stack.  One
  // can also typically spot a LR (return) address on the 2nd from the top stack 
  // frame and thus identify the function associated with the high water mark.
  
  if ( primordialThread ) {
      memset (fw_space_for_stack0, 0xEE, sizeof(fw_space_for_stack0));
  }

#endif

  // Stuff GSPRG0 with this thread's context area.  This is done very early
  // in order to support the machine check path:
  // @todo : do this in fw_start.S?

#ifdef MEASURE_FW_BOOT_TIMES
  if ( ProcessorID() == 0 ) LAPTIME(_fw_stopwatch,"genesys"); // Prime the first interval time
#endif

  extern FW_Regs_t FW_MachineCheckContexts[];
  mtspr( SPRN_GSPRG0, &( FW_MachineCheckContexts[ProcessorID()] ) );
  mtmsr( mfmsr() | MSR_ME ); // Enable Machine Checks
  isync();
  
  FW_INIT( primordialThread,                                                fw_clockstop_init               );
  
  if (primordialThread) {

     repropass = BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_REPROCOUNTER);
     if(repropass > 0)
     {
        fw_clockstop_increment();
        BeDRAM_ReadClear(BeDRAM_LOCKNUM_DDRINIT);
        BeDRAM_ReadClear(BeDRAM_LOCKNUM_DDR_FIFOS_FLUSHED);
        
        for(x=0; x<NUM_CORES; x++)
        {
           BeDRAM_ReadClear( BeDRAM_LOCKNUM_COREINIT(x));
        }
        
        // 1st pass has all threads.  On subsequent passes, only the main thread is active
        DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE0), pass1_threadactive0);
        DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE1), pass1_threadactive1);
     }
     else
     {
        // Record thread active enables for subsequent repro passes.
        pass1_threadactive0 = DCRReadPriv(TESTINT_DCR(THREAD_ACTIVE0));
        pass1_threadactive1 = DCRReadPriv(TESTINT_DCR(THREAD_ACTIVE1));
     }
  }
  
  // Only bump attendance counters on the 1st pass.  On 1st pass, REPROCOUNTER could be 0 or 1 (depending on which thread reaches
  // these instructions first.  On subsequent passes, the REPROCOUNTER is guaranteed to be incremented by the main thread before
  // other threads are enabled.
  if(BeDRAM_Read(BeDRAM_LOCKNUM_REPROCOUNTER) < 2)
  {
     // Bump the attendance counter to indicate that this thread is active
     BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_FW_THREADS_PRESENT);
  }

  {
      int i;

      for ( i = 0; i < sizeof(INIT_TABLE)/sizeof(INIT_TABLE[0]); i++ ) {
  

	  if ( ( ( INIT_TABLE[i].mask & CORE_THREAD_ZERO ) != 0 ) && ( processorThreadId != 0 ) ) {
	      continue;
	  }

	  if ( ( ( INIT_TABLE[i].mask & PRIMORDIAL_THREAD ) != 0 ) && ( primordialThread == 0 ) ) {
	      continue;
	  }

	  fw_uint64_t mask = INIT_TABLE[i].mask & ~( PRIMORDIAL_THREAD | CORE_THREAD_ZERO );

	  if ( ( mask != 0 ) && (  ( config & mask ) != mask ) ) {
	      continue;
	  }
	  
	  threadStatus[ProcessorID()] = __LINE__;
	  rc = INIT_TABLE[i].init();

	  if (rc) {
	      FW_Error("FW: Initialization failed in entry %d.  rc=0x%x.", i, rc); 
	      crash(-__LINE__);
	  }
      }
  }

  repropass = BeDRAM_Read(BeDRAM_LOCKNUM_REPROCOUNTER);
  if(repropass <= 1)    // first pass only
      FW_INIT( _PERS_ENABLED(PERS_ENABLE_JTagLoader,config),                fw_mailbox_load_via_jtag  ); 
  
  if(repropass < 2) {   // is less than the last pass
      if (( _PERS_ENABLED(PERS_ENABLE_NodeRepro, config) ) || (_PERS_ENABLED(PERS_ENABLE_PartitionRepro, config)) ) {
          fw_doCycleReproReset();
      }
  }

  if ( FW_DD1_WORKAROUNDS_ENABLED() && !TI_isDD1() ) {
    FW_Warning("DD1 workarounds are enabled on DD2 hardware.  Check your svchost settings.");
  }


#ifdef MEASURE_FW_BOOT_TIMES
  if ( ProcessorID() == 0 ) {
      uint64_t first = _fw_stopwatch.timestamps[0];
      uint64_t last  = _fw_stopwatch.timestamps[_fw_stopwatch.count-1];

      printf("Stopwatch details (%d items)\n", _fw_stopwatch.count);
      int jj;
      for ( jj = 0; jj < _fw_stopwatch.count; jj++ ) {
	  int percent   = ( _fw_stopwatch.timestamps[jj] - first ) * 100 / ( last - first );
	  int deltap    = 0;
	  uint64_t micros = 0;
	  if ( jj > 0 ) {
	      micros = ( _fw_stopwatch.timestamps[jj] - _fw_stopwatch.timestamps[jj-1] ) / 1600ull;
 	      deltap   = ( _fw_stopwatch.timestamps[jj] - _fw_stopwatch.timestamps[jj-1] ) * 100 / ( last - first );
	  }
	  printf("%s%d [%d] %lX %s%d %s%d %ld [%s]\n", 
		 ( (jj<10) ? "  " : " " ), 
		 jj, 
		 _fw_stopwatch.line[jj], _fw_stopwatch.timestamps[jj], 
		 ( percent < 10 ? "  " : " " ), percent, 
		 ( deltap  < 10 ? "  " : " " ), deltap, 
		 micros, 
		 _fw_stopwatch.name[jj] 
	      );
      }
  }
#endif
  
  // In "takeCPU" mode, only the domain leader threads will proceed to
  // main() ... other threads go into a spin loop.  We must ensure
  // that all auxiliary threads have arrived here before unleashing
  // the domain leaders.

  if ( _PERS_ENABLED(PERS_ENABLE_TakeCPU,config) ) {

    BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_TAKE_CPU_READY);

    if ( fw_thisThreadIsDomainLeader() == 0 ) {
      BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_FW_THREADS_INITD);
      fw_takeCPU_spin();
    }

    unsigned numThreads = BeDRAM_Read(BeDRAM_LOCKNUM_FW_THREADS_PRESENT);

    while ( BeDRAM_Read(BeDRAM_LOCKNUM_TAKE_CPU_READY) != numThreads )
      /* spin */
      fw_pdelay(1600)  ;
  }

  // Indicate that we are done with the big stack.  This makes it available for
  // other use, such as machine check handling.

  if ( primordialThread ) {
    fw_semaphore_up( BeDRAM_LOCKNUM_BIG_STACK_IN_USE );
  }

  // Indicate that this firmware thread has completed initialization:
  BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_FW_THREADS_INITD);

  // launch main(which typically will launch kernel)

  int status = main( 0, (void *)0, (void *)0 );

  exit( status );
}


