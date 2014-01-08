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
#include <hwi/include/bqc/gea_dcr.h>
#include <hwi/include/bqc/BIC_inlines.h>
#include "Firmware_internals.h"
#include "Firmware_RAS.h"
#include "fw_regs.h"


FW_Regs_t FW_MachineCheckContexts[ NUM_HW_THREADS ];

extern uint64_t  _fw_Vec0_MCHK;

#ifndef FW_SINGLE_DOMAIN
extern uint64_t  _fw_Vec1_MCHK;
#endif

int fw_recoverTLBParityError( uint64_t mcsr );


int fw_installInterruptVector( void* vec, unsigned size ) {

  /*  +---------------------------------------------------------------+
   *  | WARNING: We currently only have 2K carved out for the vector. |
   *  |          If the size is greater than that, then return an     |
   *  |          error.  Increasing this value beyond 2K requires     |
   *  |          changes to the assembler (start.S) code.             |
   *  +---------------------------------------------------------------+
   */

  if (size > 0x1000) {
    return FW_INVALID;
  }

  uint64_t domain = fw_getThisDomainID();
  size_t   lock   = ( domain == 0 ) ? BeDRAM_LOCKNUM_INST_VEC0_IN_PROGRESS : BeDRAM_LOCKNUM_INST_VEC1_IN_PROGRESS;

  // First one in wins:

  if ( BeDRAM_ReadIncSat( lock ) == 0 ) {

#ifdef FW_SINGLE_DOMAIN
      uint64_t* to = &_fw_Vec0_MCHK ;
#else
    uint64_t* to = (domain == 0) ? &_fw_Vec0_MCHK : &_fw_Vec1_MCHK;
#endif
    uint64_t* from = (uint64_t*)vec;
    unsigned  i;

    // Skip over the machine check vector, which occupies the
    // first 32 bytes:

    to   += 32 / sizeof(uint64_t); /*out of range pointer*/
    from += 32 / sizeof(uint64_t);
    size -= 32;

    for ( i = 0; i < size; i += sizeof(uint64_t) ) {
      *(to++) = *(from++);
    }
  }

  // Everyone decrements (because everyone incremented):
  BeDRAM_ReadDecSat( lock );

  // If the atomic counter is non-zero, it is because some other
  // thread is busy performing the memory copy (above).  Everyone
  // waits here until that is completed just so the vector is
  // in a known state.

  while ( BeDRAM_Read( lock ) != 0 )
    /* spin */
    fw_pdelay( 1600 )
    ;


  // HW Issue 874:  If BeDRAM is cached, an icbi will result in a machine
  //   check (because BeDRAM is not cached in L2).  Instead, we can issue
  //   an ici which is not broadcast out to L2.   In reality, we are
  //   replacing the "crash" loader, which means that we have likely
  //   not fetched any of the instructions.

  ici();


  // Activate the vector:
#ifdef FW_SINGLE_DOMAIN
  mtspr( SPRN_IVPR,  &_fw_Vec0_MCHK );
#else
  mtspr( SPRN_IVPR, (domain == 0) ? &_fw_Vec0_MCHK : &_fw_Vec1_MCHK );
#endif
  isync();
  
  return FW_OK;
}

typedef struct _FW_PUEA_Handler {
  uint64_t mask;
  int (*handler)( uint64_t status );
} FW_PUEA_Handler;


typedef struct _FW_GEA_Handler {
  uint64_t mask[3];
  int (*handler)( uint64_t status[] );
} FW_GEA_Handler;

extern int fw_GEA_PUEA_Handler( uint64_t status );

#define MAX_PUEA_HANDLERS 4

FW_PUEA_Handler FW_PUEA_Handlers[MAX_PUEA_HANDLERS] = {
  { PUEA_INTERRUPT_STATUS_GEA_ALL_MASK, fw_GEA_PUEA_Handler },
  { 0, 0 },
  { 0, 0 },
  { 0, 0 }
};


#ifdef FW_PREINSTALLED_GEA_HANDLERS
#define MAX_GEA_HANDLERS
#else
#define MAX_GEA_HANDLERS 16
#endif

FW_GEA_Handler FW_GEA_Handlers[MAX_GEA_HANDLERS] = {

#ifdef FW_PREINSTALLED_GEA_HANDLERS
  { { BEDRAM_GEA_MASK_0,    BEDRAM_GEA_MASK_1,    BEDRAM_GEA_MASK_2    }, fw_bedram_machineCheckHandler },
  { { CLOCKSTOP_GEA_MASK_0, CLOCKSTOP_GEA_MASK_1, CLOCKSTOP_GEA_MASK_2 }, fw_clockstop_machineCheckHandler },
  { { DCR_ARB_GEA_MASK_0,   DCR_ARB_GEA_MASK_1,   DCR_ARB_GEA_MASK_2   }, fw_dcr_arbiter_machineCheckHandler },
  { { DDR_GEA_MASK_0,       DDR_GEA_MASK_1,       DDR_GEA_MASK_2       }, fw_ddr_machineCheckHandler },
  { { DEVBUS_GEA_MASK_0,    DEVBUS_GEA_MASK_1,    DEVBUS_GEA_MASK_2    }, fw_devbus_machineCheckHandler },
  { { ENVMON_GEA_MASK_0,    ENVMON_GEA_MASK_1,    ENVMON_GEA_MASK_2    }, fw_envmon_machineCheckHandler },
  { { GEA_GEA_MASK_0,       GEA_GEA_MASK_1,       GEA_GEA_MASK_2       }, fw_gea_machineCheckHandler },
  { { L1P_GEA_MASK_0,       L1P_GEA_MASK_1,       L1P_GEA_MASK_2       }, fw_l1p_machineCheckHandler },
  { { L2_GEA_MASK_0,        L2_GEA_MASK_1,        L2_GEA_MASK_2        }, fw_l2_machineCheckHandler },
  { { L2C_GEA_MASK_0,       L2C_GEA_MASK_1,       L2C_GEA_MASK_2       }, fw_l2_central_machineCheckHandler },
  { { L2CTR_GEA_MASK_0,     L2CTR_GEA_MASK_1,     L2CTR_GEA_MASK_2     }, fw_l2_counter_machineCheckHandler },
  { { MSGC_GEA_MASK_0,      MSGC_GEA_MASK_1,      MSGC_GEA_MASK_2      }, fw_ms_genct_machineCheckHandler },
  { { MU_GEA_MASK_0,        MU_GEA_MASK_1,        MU_GEA_MASK_2        }, fw_mu_machineCheckHandler },
  { { ND_GEA_MASK_0,        ND_GEA_MASK_1,        ND_GEA_MASK_2        }, fw_nd_machineCheckHandler },
  { { PCIE_GEA_MASK_0,      PCIE_GEA_MASK_1,      PCIE_GEA_MASK_2      }, fw_pcie_machineCheckHandler },
  { { SERDES_GEA_MASK_0,    SERDES_GEA_MASK_1,    SERDES_GEA_MASK_2    }, fw_serdes_machineCheckHandler },
  { { TESTINT_GEA_MASK_0,   TESTINT_GEA_MASK_1,   TESTINT_GEA_MASK_2   }, fw_testint_machineCheckHandler },
  { { UPC_GEA_MASK_0,       UPC_GEA_MASK_1,       UPC_GEA_MASK_2       }, fw_upc_machineCheckHandler },
  { { WU_GEA_MASK_0,        WU_GEA_MASK_1,        WU_GEA_MASK_2        }, fw_wu_machineCheckHandler },
#else
  { { 0, 0, 0 }, 0 },
#endif

};

int fw_installGeaHandler( int (*handler)(uint64_t status[]), uint64_t mask[] ) {

  int i;
  
  for ( i = 0; i < FW_ARRAY_SIZE(FW_GEA_Handlers); i++ ) {
    FW_GEA_Handler* h = &(FW_GEA_Handlers[i]);
    if ( ( h->mask[0] == 0 ) && ( h->mask[1] == 0 ) && ( h->mask[2] == 0 ) ) {
      h->mask[0] = mask[0];
      h->mask[1] = mask[1];
      h->mask[2] = mask[2];
      h->handler = handler;
      return 0;
    }
  }

  Terminate( __LINE__ );

  return -1;
}


int fw_machineCheckIsFatal = 0;

void fw_machineCheckHandler( void ) {

    /* --------------------------------------------------------------------
     *  If we are not on thread 0, we will attempt to switch over to the
     *  "big stack".  If we cannot do so, then we will continue on the
     *  current small stack, but will assume that some amount of stack
     *  corruptions has occurred (fatal).  Note that the flag indicating
     *  this situation is global -- since we are switching stacks, it is
     *  not practical to use a local variable.
     * -------------------------------------------------------------------- */

    BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_MC_PENDING);

    if ( ( ProcessorID() > 0 ) || ( BeDRAM_Read(BeDRAM_LOCKNUM_BIG_STACK_IN_USE) > 0 ) ) {
	fw_semaphore_down(BeDRAM_LOCKNUM_BIG_STACK_IN_USE );
    }

    if ( ProcessorID() > 0 ) {
	uint64_t stackPointer = (uint64_t)&(fw_stacks[0]) + FW_STACK_SIZE - 32;

	asm volatile(
	    "mr 0,1;"
	    "mr 1,%0;"
	    "std 0,0(1);"
	    "addi 1,1,-32;"
	    :
	    : "r" (stackPointer)
	    :"memory"
	    );
    }

    FW_Regs_t* regs = &( FW_MachineCheckContexts[ProcessorID()] );

    int tracing = TRACE_ENABLED(TRACE_MChk);


    unsigned n;
    uint64_t pueaSummary = BIC_ReadMachineCheckIntSummary(ProcessorThreadID());

    if (tracing) {

	printf(
	    "MachineCheck IAR:%lX LR:%lX MSR:%0lX MCSR=%lX sum:%lX\n",
	    regs->ip,
	    regs->lr,
	    regs->msr,
	    mfspr(SPRN_MCSR),
	    pueaSummary
	    );

#if 0
	int i;
	for ( i=0; i < 32; i += 4 ) {
	    printf("  G%d:%lX G%d:%lX G%d:%lX G%d:%lX \n",
		   i + 0, regs->gpr[i+0],
		   i + 1, regs->gpr[i+1],
		   i + 2, regs->gpr[i+2],
		   i + 3, regs->gpr[i+3]
		);
	}
#endif

    }

  for ( n = 0; ( n < sizeof(FW_PUEA_Handlers)/sizeof(FW_PUEA_Handlers[0]) ) && ( pueaSummary != 0 ); n++ ) {
    if ( ( pueaSummary & FW_PUEA_Handlers[n].mask ) != 0 ) {
      fw_machineCheckIsFatal |= FW_PUEA_Handlers[n].handler( pueaSummary & FW_PUEA_Handlers[n].mask );
      pueaSummary &= ~ FW_PUEA_Handlers[n].mask;
    }
  }
  
  if ( pueaSummary != 0 ) {
    FW_Warning( "Unrecognized PUEA status bits: %lX", pueaSummary );
    fw_machineCheckIsFatal |= 1; 
  }

  // Check for A2 internal machine check status:
  int rc;
  uint64_t mcsr = mfspr(SPRN_MCSR);

  if ( ( mcsr & ~MCSR_EXT ) != 0 ) {

      // A2 Erratum 589: Incorrect Machine Check When MMU_MCHK = 0
      //
      // Description When a parity error or multi-hit error occurs on an instruction translation (i.e. not an ERAT instruction), in TLB mode
      // (CCR2[NOTLB]=0) the recovery method is defined by 
      //   - XUCR4[MMU_MCHK]: MMU_MCHK = 0 is hardware recovery mode, I-ERAT entries are automatically invalidated by the hardware. This 
      //     mode is not valid in CCR2[NOTLB]=1 mode as there is no backing TLB entry so a machine check exception will always be raised 
      //     when CCR2[NOTLB]=1.
      //   - MMU_MCHK = 1 is software recovery mode, a machine check exception is taken and no invalidations are performed by the hardware.
      // The problem is that when in MMU_MCHK=0 mode, the A2 still generates a machine check exception when it should not for I-ERAT 
      // multi-hit, I-ERAT parity, or TLB LRU parity errors. The hardware recovery (invalidations) is still performed which is correct 
      // operation.
      //
      // It will not be fixed for DD2 or subsequent passes (permanent workaround).
      //
      // The recommended workaround for DD1 and subsequent passes is for the software machine check handler to do the
      // following:
      //   1. If the following statement is true:
      //        ((MCSR[IEMH] OR MCSR[IEPE]) = 1) AND (XUCR4[MMU_MCHK] = 0) AND (CCR2[NOTLB]=0) AND
      //        the instruction at MCSRR0 is not a ERAT management instruction.
      //   2. Then, read MMUCR1[52:63] to reset those bits and then, exit the handler (rfmci).


      int isErratum589 = 0;

      if ( ( ( mcsr & ( MCSR_IEMH | MCSR_IEPE ) ) != 0 ) && ( ( mfspr(SPRN_XUCR4) & XUCR4_MMU_MCHK) == 0 )  && ( ( mfspr(SPRN_CCR2) & CCR2_NOTLB ) == 0 ) ) {

	  uint32_t instr  = *((uint32_t*)mfspr(SPRN_MCSRR0));
      
	  int bits_0_5 = (instr & 0xFC000000) >> (31-5);
	  int bits_21_30 = (instr & 0x7FE) >> (31-30);
	  int isEratManagement = (bits_0_5 == 31) && (
	      (bits_21_30 == 179) /* eratere */  ||
	      (bits_21_30 == 211) /* eratwe */ ||
	      (bits_21_30 == 147) /* eratsx */ ||
	      (bits_21_30 == 819) /* erativax */ ||
	      (bits_21_30 ==  51) /* eratilx */ 
	      );

	  if ( isEratManagement == 0 ) {
	      isErratum589 = 1;
	  }
      }

      if ( isErratum589 ) {
	  mfspr(SPRN_MMUCR1); // read bits 52-63 clears those bits
	  // This is not a fatal error!
      }
      else {

	  fw_uint64_t details[3];

	  // NOTE: we categorize multi-hit errors as software errors.  Do not mark hardware in error for
	  //       this case

          #define MCSR_SOFTWARE_ERRORS ( MCSR_IEMH | MCSR_DEMH | MCSR_TLBMH )
          
          if ( ( mcsr & ( MCSR_TLBPE | MCSR_TLBLRUPE ) ) != 0 )
          {
              rc = fw_recoverTLBParityError( mcsr & ( MCSR_TLBPE | MCSR_TLBLRUPE ) );

              if (rc) {
                  details[0] = mcsr & ~MCSR_SOFTWARE_ERRORS;
                  details[1] = ProcessorCoreID();
		  details[2] = rc;
                  fw_machineCheckRas( FW_RAS_A2_HARDWARE_MACHINE_CHECK, details, 3, __FILE__, __LINE__ );
                  fw_machineCheckIsFatal |= 1; 
              }
              
              mcsr &= ~MCSR_TLBPE;
          }
          
          if ( ( mcsr & MCSR_SOFTWARE_ERRORS ) != 0 ) {
              details[0] = mcsr & MCSR_SOFTWARE_ERRORS;
              details[1] = ProcessorCoreID();
              fw_machineCheckRas( FW_RAS_A2_SOFTWARE_MACHINE_CHECK, details, 2, __FILE__, __LINE__ );
              fw_machineCheckIsFatal |= 1; 
          }
          
          if ( ( mcsr & ~MCSR_SOFTWARE_ERRORS ) != 0 ) {
              details[0] = mcsr & ~MCSR_SOFTWARE_ERRORS;
              details[1] = ProcessorCoreID();
              fw_machineCheckRas( FW_RAS_A2_HARDWARE_MACHINE_CHECK, details, 2, __FILE__, __LINE__ );
              fw_machineCheckIsFatal |= 1; 
          }
          
      }
  }

  //fw_machineCheckIsFatal |= 1; 



  /* ---------------------------------------------------------------------
   *   If we hadn't finished initialization of the current hardware
   *   thread, we have quite likely clobbered our own stack .... hence a fatal
   *   condition:
   * --------------------------------------------------------------------- */

  if ( BeDRAM_Read(BeDRAM_LOCKNUM_FW_THREADS_INITD) < BeDRAM_Read(BeDRAM_LOCKNUM_FW_THREADS_PRESENT) ) {
      fw_machineCheckRas( FW_RAS_UNRECOVERABLE_MACHINE_CHECK, 0, 0, __FILE__, __LINE__ );
      fw_machineCheckIsFatal |= 1;
  }

  BeDRAM_ReadDecSat(BeDRAM_LOCKNUM_MC_PENDING);
  fw_semaphore_up( BeDRAM_LOCKNUM_BIG_STACK_IN_USE);

  if ( fw_machineCheckIsFatal ) {

      // Wait here if there are any other machine checks on other threads
      while ( BeDRAM_Read(BeDRAM_LOCKNUM_MC_PENDING) > 0 ) {
	  fw_pdelay(1600);
      }

    //!< @todo This situation should, perhaps, be reported as a RAS event
      
    Terminate(__LINE__);
  }


  /* --------------------------------------------------------------------
   *  Restore the stack and release the big stack lock if we are not 
   *  on core 0 / thread 0.
   * -------------------------------------------------------------------- */

  if ( ProcessorID() > 0 ) {

    asm volatile(
		 "addi 1,1,32;"
		 "ld   1,0(1);"
		 );


  }
}

Firmware_TLBEntries_t* fw_MMUCache[17];
int fw_A2_setTLBTable(Firmware_TLBEntries_t* mmu)
{
    uint64_t core = PhysicalProcessorID();
    uint64_t mask = ~(1<<(ProcessorThreadID()));
    mtspr(SPRN_TENC,((mask) & 0xf));
    isync();
    
    fw_MMUCache[core] = mmu;
    
    mtspr(SPRN_TENS,0xf);
    isync();
    return 0;
}

int fw_recoverTLBParityError( uint64_t mcsr ) {

    int notHandled = 1;
    int setway;
    uint64_t core = PhysicalProcessorID();
    uint64_t mask = ~(1<<(ProcessorThreadID()));
    uint64_t mmucr1;
    Firmware_TLBEntries_t* mmu;
    fw_uint64_t details[10];
    int n = 0;
    mtspr(SPRN_TENC,((mask) & 0xf));
    isync();
    
    mmucr1 = mfspr(SPRN_MMUCR1); // read bits 52-63 clears those bits
    setway = (mmucr1 & 0x1ff);
    
    details[n++] = mmucr1;
    details[n++] = mcsr;
    details[n++] = (fw_uint64_t)fw_MMUCache[core];
    
    
    // This code is currently assuming that fw_MMUCache[core]'s TLB is good
    if(fw_MMUCache[core] != NULL) {
        mmu = fw_MMUCache[core];
        details[n++] = setway;
        details[n++] = mmu[setway].mas1;
        details[n++] = mmu[setway].mas2;
        details[n++] = mmu[setway].mas7_3;
        details[n++] = mmu[setway].mas8;
        details[n++] = mmu[setway].mmucr3;
        
        tlbwe_slot(setway%4, 
                   mmu[setway].mas1, mmu[setway].mas2, mmu[setway].mas7_3, 
                   mmu[setway].mas8, mmu[setway].mmucr3);
        isync();
        notHandled = 0;
    }

    fw_machineCheckRas( FW_RAS_A2_TLBPE_MACHINE_CHECK, details, n, __FILE__, __LINE__ );

    
    mtspr(SPRN_TENS,0xf);
    isync();
    return notHandled;
}

int fw_GEA_PUEA_Handler( uint64_t pueaStatus ) {

  // NOTE : We only expect to see lane 0 interrupts.  We'll process them first and
  //        then scrape up & report any stragglers:

  unsigned n;
  int rc = 0;
  int tracing = TRACE_ENABLED(TRACE_MChk);

  if ( ( pueaStatus & PUEA_INTERRUPT_STATUS_GEA(0) ) != 0 ) {

    uint64_t geaStatus[3];

    geaStatus[0] = GEA_DCR_PRIV_PTR->gea_mapped_interrupt_status0_0;
    geaStatus[1] = GEA_DCR_PRIV_PTR->gea_mapped_interrupt_status0_1;
    geaStatus[2] = GEA_DCR_PRIV_PTR->gea_mapped_interrupt_status0_2;

    if ( tracing ) {
      printf("GEA mapped status for lane %d %lX-%lX-%lX\n", 0, geaStatus[0], geaStatus[1], geaStatus[2] );
    }

    for ( n = 0; ( n < FW_ARRAY_SIZE(FW_GEA_Handlers) ) ; n++ ) {
      
      if ( 
	  ( ( geaStatus[0] & FW_GEA_Handlers[n].mask[0] ) != 0 ) ||
	  ( ( geaStatus[1] & FW_GEA_Handlers[n].mask[1] ) != 0 ) ||
	  ( ( geaStatus[2] & FW_GEA_Handlers[n].mask[2] ) != 0 )
	   )
	{
	  rc |= FW_GEA_Handlers[n].handler( geaStatus );

	  geaStatus[0] &= ~FW_GEA_Handlers[n].mask[0];
	  geaStatus[1] &= ~FW_GEA_Handlers[n].mask[1];
	  geaStatus[2] &= ~FW_GEA_Handlers[n].mask[2];
	}
    }

    if ( ( geaStatus[0] != 0 ) || ( geaStatus[1] != 0 ) || ( geaStatus[2] != 0 ) ) {
      FW_Error("(E) unhandled GEA status : %lX-%lX-%lX", geaStatus[0], geaStatus[1], geaStatus[2] );
      rc = -1;
    }

    // todo
    pueaStatus &= ~PUEA_INTERRUPT_STATUS_GEA(0);
  }

  if ( pueaStatus != 0 ) {
    //!< @todo Handle GEA machine check status for other lanes
  }

  return rc;
}

int fw_gea_machineCheckHandler( uint64_t status[] ) {

  fw_uint64_t details[16];
  fw_uint16_t n = 0;

  details[n++] = GEA_DCR( GEA_INTERRUPT_STATE__MACHINE_CHECK );
  details[n++] = GEA_DCR_PRIV_PTR->gea_interrupt_state__machine_check;

  details[n++] = GEA_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK );
  details[n++] = GEA_DCR_PRIV_PTR->interrupt_internal_error__machine_check;

  fw_machineCheckRas( FW_RAS_GEA_MACHINE_CHECK, details, n, __FILE__, __LINE__ );

  return -1;
}

int fw_interrupts_init( void ) {
  
    //TRACE_ENTRY(TRACE_MChk);
  
#if 0

  // +----------------------------------------------------------------------------+
  // | NOTE:  This is ver conservative code that reassigns all GEA input sources  |
  // |        not used by firmware to lane 1.                                     |
  // +----------------------------------------------------------------------------+

  DCRWritePriv ( GEA_DCR( GEA_INTERRUPT_MAP1 ),
		 GEA_DCR__GEA_INTERRUPT_MAP1__L2C_MAP1_set(1) | /* NOTE: L2 Central bit 0 is the machine check bit; bit 1 is not */
		 GEA_DCR__GEA_INTERRUPT_MAP1__PCIE_MAP0_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP1__PCIE_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP1__ND_MAP0_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP1__MU_MAP6_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP1__SD0_MAP0_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP1__SD0_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP1__SD1_MAP0_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP1__SD1_MAP1_set(1) 
		 );

  DCRWritePriv ( GEA_DCR( GEA_INTERRUPT_MAP2 ),
		 GEA_DCR__GEA_INTERRUPT_MAP2__CS_MAP0_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP2__CS_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP2__EN_MAP0_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP2__EN_MAP1_set(1) 
		 );

  DCRWritePriv ( GEA_DCR( GEA_INTERRUPT_MAP3 ),
		 GEA_DCR__GEA_INTERRUPT_MAP3__DB_PCIE_MAP0_set(1) | 
		 GEA_DCR__GEA_INTERRUPT_MAP3__DB_PCIE_MAP1_set(1) | 
		 GEA_DCR__GEA_INTERRUPT_MAP3__DB_PCIE_MAP2_set(1) | 
		 GEA_DCR__GEA_INTERRUPT_MAP3__DB_PCIE_MAP3_set(1) | 
		 GEA_DCR__GEA_INTERRUPT_MAP3__UPC_MAP0_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP3__UPC_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP3__DB_MAP0_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP3__DB_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP3__GEA_MAP0_set(1)
		 );

  DCRWritePriv ( GEA_DCR( GEA_INTERRUPT_MAP4 ),
		 GEA_DCR__GEA_INTERRUPT_MAP4__L1P0_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP4__L1P0_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP4__L1P0_MAP3_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP4__L1P1_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP4__L1P1_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP4__L1P1_MAP3_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP4__L1P2_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP4__L1P2_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP4__L1P2_MAP3_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP4__L1P3_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP4__L1P3_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP4__L1P3_MAP3_set(1)
		 );


  DCRWritePriv ( GEA_DCR( GEA_INTERRUPT_MAP5 ),
		 GEA_DCR__GEA_INTERRUPT_MAP5__L1P4_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP5__L1P4_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP5__L1P4_MAP3_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP5__L1P5_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP5__L1P5_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP5__L1P5_MAP3_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP5__L1P6_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP5__L1P6_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP5__L1P6_MAP3_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP5__L1P7_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP5__L1P7_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP5__L1P7_MAP3_set(1)
		 );


  DCRWritePriv ( GEA_DCR( GEA_INTERRUPT_MAP6 ),
		 GEA_DCR__GEA_INTERRUPT_MAP6__L1P8_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP6__L1P8_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP6__L1P8_MAP3_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP6__L1P9_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP6__L1P9_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP6__L1P9_MAP3_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP6__L1P10_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP6__L1P10_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP6__L1P10_MAP3_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP6__L1P11_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP6__L1P11_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP6__L1P11_MAP3_set(1)
		 );


  DCRWritePriv ( GEA_DCR( GEA_INTERRUPT_MAP7 ),
		 GEA_DCR__GEA_INTERRUPT_MAP7__L1P12_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP7__L1P12_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP7__L1P12_MAP3_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP7__L1P13_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP7__L1P13_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP7__L1P13_MAP3_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP7__L1P14_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP7__L1P14_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP7__L1P14_MAP3_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP7__L1P15_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP7__L1P15_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP7__L1P15_MAP3_set(1)
		 );

  DCRWritePriv ( GEA_DCR( GEA_INTERRUPT_MAP8 ),
		 GEA_DCR__GEA_INTERRUPT_MAP8__L1P16_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP8__L1P16_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP8__L1P16_MAP3_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP8__TESTINT_MAP0_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP8__TESTINT_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP8__DCR_ARB_MAP0_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP8__DCR_ARB_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP8__MU_MAP0_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP8__MU_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP8__MU_MAP2_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP8__MU_MAP3_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP8__MU_MAP4_set(1) 
		 );

  DCRWritePriv ( GEA_DCR( GEA_INTERRUPT_MAP9 ),
		 GEA_DCR__GEA_INTERRUPT_MAP9__MU_MAP5_set(1) | 
		 GEA_DCR__GEA_INTERRUPT_MAP9__DDR0_MAP1_set(1) | 
		 GEA_DCR__GEA_INTERRUPT_MAP9__DDR0_MAP2_set(1) | 
		 GEA_DCR__GEA_INTERRUPT_MAP9__DDR1_MAP1_set(1) | 
		 GEA_DCR__GEA_INTERRUPT_MAP9__DDR1_MAP2_set(1) | 
		 GEA_DCR__GEA_INTERRUPT_MAP9__SW_MAP0_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP9__SW_MAP1_set(1) |
		 GEA_DCR__GEA_INTERRUPT_MAP9__EDRAM_MAP0_set(1) | 
		 GEA_DCR__GEA_INTERRUPT_MAP9__EDRAM_MAP1_set(1) | 
		 GEA_DCR__GEA_INTERRUPT_MAP9__GEA_MAP1_set(1)
		 );


#endif

  // Enable GEA Lane 0 Machine Checks
  BIC_WriteInterruptMap( 0, _B2( BIC_MAP_GEA_LANE(0) * 2  +  1, BIC_MACHINE_CHECK ) );


#ifndef FW_PREINSTALLED_GEA_HANDLERS

  uint64_t mask[3] = { GEA_GEA_MASK_0, GEA_GEA_MASK_1, GEA_GEA_MASK_2 };

  fw_installGeaHandler( fw_gea_machineCheckHandler, mask );

#endif

  DCRWritePriv( GEA_DCR( GEA_INTERRUPT_STATE_CONTROL_HIGH), 
		GEA_DCR__GEA_INTERRUPT_STATE_CONTROL_HIGH__DEVBUS_CTL_PERR_set(1)  | // DevBus control parity error 
		// <disabled>  GEA_DCR__GEA_INTERRUPT_STATE_CONTROL_HIGH__TIMER0_INT_set(1)       | // Global Timer 0 Interrupt
		// <disabled>  GEA_DCR__GEA_INTERRUPT_STATE_CONTROL_HIGH__TIMER1_INT_set(1)       | // Global Timer 1 Interrupt
		// <disabled>  GEA_DCR__GEA_INTERRUPT_STATE_CONTROL_HIGH__TIMER2_INT_set(1)       | // Global Timer 2 Interrupt
		// <disabled>  GEA_DCR__GEA_INTERRUPT_STATE_CONTROL_HIGH__TIMER3_INT_set(1)       | // Global Timer 3 Interrupt
		0 );

  DCRWritePriv( GEA_DCR(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), 
		GEA_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
		GEA_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
		GEA_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
		GEA_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
		0 );

  //TRACE_EXIT(TRACE_MChk);

  return 0;
}

void fw_machineCheckRas( fw_uint32_t rasMessageCode, fw_uint64_t details[], fw_uint16_t numDetails, const char* file, int lineNumber ) {

    if ( TRACE_ENABLED(TRACE_MChk) ) {
	int i;
	printf("Machine Check : message-id=%X details=%d source=%s:%d\n", rasMessageCode, numDetails, file, lineNumber );
	for (i = 0; i < numDetails; i+=4 ) {
	    printf("    Details   : %llX-%llX-%llX-%llx\n", details[i], details[i+1], details[i+2], details[i+3] );
	}
    }

    fw_writeRASEvent( rasMessageCode, numDetails, details );

}

