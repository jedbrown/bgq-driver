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
#include <hwi/include/bqc/l2_dcr.h>
#include <hwi/include/bqc/l2_central_dcr.h>
#include <hwi/include/bqc/l2_counter_dcr.h>
#include <hwi/include/bqc/l2_util.h>
#include <hwi/include/bqc/gea_dcr.h>
#include <hwi/include/bqc/testint_inlines.h>

#include "Firmware_internals.h"
#include "Firmware_RAS.h"

#define FW_NUM_L2_SLICES L2_DCR_num

typedef struct _FW_L2_Correctables {
    fw_uint64_t mask;
    fw_uint64_t error_bits;
    fw_uint64_t threshold;
    fw_uint64_t count;
    fw_uint16_t slices;
    fw_uint32_t ras_id;
} FW_L2_Correctables;


#define FW_L2_CORR_ARRAY_MASK ( L2_DCR__L2_INTERRUPT_STATE__EDR_CE_set(1) | L2_DCR__L2_INTERRUPT_STATE__EDR_OCE_set(1) | L2_DCR__L2_INTERRUPT_STATE__RQ_CE_set(1) )
#define FW_L2_CORR_DIR_MASK   (	L2_DCR__L2_INTERRUPT_STATE__COH_CE_set(1) | L2_DCR__L2_INTERRUPT_STATE__DIRB_CE_set(1) | L2_DCR__L2_INTERRUPT_STATE__LRU_CE_set(1) | L2_DCR__L2_INTERRUPT_STATE__SPR_CE_set(1) )
	

static volatile FW_L2_Correctables _fw_l2_correctables[2] = {
    { FW_L2_CORR_ARRAY_MASK, 0, 1, 0, 0, FW_RAS_L2_CORRECTABLE_ARRAY_SUMMARY },
    { FW_L2_CORR_DIR_MASK,   0, 1, 0, 0, FW_RAS_L2_CORRECTABLE_DIRECTORY_SUMMARY }
};

void fw_l2_resetCEThresholds() {
  if ( PERS_ENABLED( PERS_ENABLE_DiagnosticsMode ) ) {
      _fw_l2_correctables[0].threshold = _fw_l2_correctables[1].threshold = 1;
  }
  else if ( PERS_ENABLED( PERS_ENABLE_MaskCorrectables ) ) {
      _fw_l2_correctables[0].threshold = _fw_l2_correctables[1].threshold = 0;
  }
  else {
      _fw_l2_correctables[0].threshold = _fw_l2_correctables[1].threshold = 100;
  }
}
  
void fw_l2_flushCorrectables( int endOfJob ) {

    int i;

    for ( i = 0; i < sizeof(_fw_l2_correctables) / sizeof(_fw_l2_correctables[0]); i++ ) {

	fw_semaphore_down( BeDRAM_LOCKNUM_RAS_FLUSH_LOCK );

	if ( _fw_l2_correctables[i].count > 0 ) {

	    fw_uint64_t details[8];
	    int n = 0;

	    details[n++] = _fw_l2_correctables[i].count;
	    details[n++] = _fw_l2_correctables[i].error_bits;
	    details[n++] = _fw_l2_correctables[i].slices;
	    
	    if ( endOfJob == 0 ) {
		_fw_l2_correctables[i].threshold *= 10;
	    }


	    _fw_l2_correctables[i].count = _fw_l2_correctables[i].error_bits = _fw_l2_correctables[i].slices = 0;

	    fw_writeRASEvent( _fw_l2_correctables[i].ras_id, n, details );
	}

	fw_semaphore_up( BeDRAM_LOCKNUM_RAS_FLUSH_LOCK );
    }

    if ( endOfJob != 0 ) {
	fw_l2_resetCEThresholds();
    }
    
}  

int fw_l2_reportAndMaskCorrectableErrorHandler( fw_uint64_t details[] ) {


    int rc = 0; // assume that only correctables are present
    unsigned slice;
    unsigned n = 0;

    uint64_t correctableMask =
	L2_DCR__L2_INTERRUPT_STATE__COH_CE_set(1) |
	L2_DCR__L2_INTERRUPT_STATE__DIRB_CE_set(1) |
	L2_DCR__L2_INTERRUPT_STATE__EDR_CE_set(1) |
	L2_DCR__L2_INTERRUPT_STATE__EDR_OCE_set(1) |
	L2_DCR__L2_INTERRUPT_STATE__LRU_CE_set(1) |
	L2_DCR__L2_INTERRUPT_STATE__SPR_CE_set(1) |
	L2_DCR__L2_INTERRUPT_STATE__RQ_CE_set(1) |
	0;

    for ( slice = 0; slice < L2_DCR_num; slice++ ) {

	uint64_t state = L2_DCR_PRIV_PTR(slice)->l2_interrupt_state__machine_check;

	if ( ( state & ~correctableMask ) != 0 ) {
	    rc = -1; // There are uncorrectable errors
	}

	if ( ( state & correctableMask ) != 0 ) {

	    state &= correctableMask; // eliminate all but the correctable bits

	    details[n++] = L2_DCR(slice, L2_INTERRUPT_STATE__MACHINE_CHECK);
	    details[n++] = state;

	    if ( _fw_l2_correctables[0].threshold == 0 ) {

		// Mask the error by disabling them in the interrupt control register:

		DCRWritePriv( L2_DCR( slice, L2_INTERRUPT_STATE_CONTROL_HIGH ), 
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_set(1)       | // DCR ring parity error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__RQ_ERR_set(1)           | // Request queue error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__CTRL_ERR_set(1)         | // Controller error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__RDB_ERR_set(1)          | // Read buffer error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__COH_CE_set(0)           | // Coherence array correctable error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__COH_UE_set(1)           | // Coherence array uncorrectable error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__DIRB_CE_set(0)          | // Directory array correctable error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__DIRB_UE_set(1)          | // Directory array uncorrectable error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__EDR_CE_set(0)           | // Coherence array correctable error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__EDR_OCE_set(0)          | // Coherence array correctable error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__EDR_OUE_set(1)          | // Coherence array correctable error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__EDR_UE_set(1)           | // Coherence array ucorrectable error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LRU_CE_set(0)           | // LRU correctable error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LRU_UE_set(1)           | // LRU uncorrectable error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__SPR_CE_set(0)           | // SPR correctable error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__SPR_UE_set(1)           | // SPR uncorrectable error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__RQ_CE_set(0)            | // Read queue correctable error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__RQ_UE_set(1)            | // Read queue uncorrectable error
			      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__OVERLOCK_set(1)         | // Tried to lock more than overlock_thresh ways down
			      // [disabled]  L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__UPC_COUNTER_OVF_set(1)  | // UPC counter overflow
			      0 );
	    }

	    // Ack the interrupt bits:
	    DCRWritePriv( L2_DCR(slice, L2_INTERRUPT_STATE__STATE), state );
	}
    }

    if ( n > 0 ) {
	fw_machineCheckRas( FW_RAS_L2_CORRECTABLE_MACHINE_CHECK, details, n, __FILE__, __LINE__ );
    }

    return rc;

}

int fw_l2_accumulateCorrectablesHandler( void ) {

    int rc = 0; // assume that only correctables are present
    int flush = 0;
    unsigned s, i;

    for ( s = 0; s < L2_DCR_num; s++ ) {

	uint64_t state = L2_DCR_PRIV_PTR(s)->l2_interrupt_state__machine_check;

	if ( state == 0 ) {
	    continue;
	}

	for ( i = 0; i < sizeof(_fw_l2_correctables)/sizeof(_fw_l2_correctables[0]); i++ ) {
	    
	    if ( ( state & _fw_l2_correctables[i].mask ) != 0 ) {


		_fw_l2_correctables[i].count++;
		_fw_l2_correctables[i].slices |= ( 1 << s );
		_fw_l2_correctables[i].error_bits |= (state & _fw_l2_correctables[i].mask);

		if ( _fw_l2_correctables[i].count >= _fw_l2_correctables[i].threshold ) {
		    flush = 1;
		}

		// Ack the interrupt bits:
		DCRWritePriv( L2_DCR(s, L2_INTERRUPT_STATE__STATE), state & _fw_l2_correctables[i].mask );

		state &= ~_fw_l2_correctables[i].mask;
	    }
	}

	if ( state != 0 ) {
	    rc = -1; // There are uncorrectable errors
	}
    }

    if ( flush ) {
	fw_l2_flushCorrectables( 0 );
    }

    return rc;
}

int fw_l2_correctableErrorHandler( fw_uint64_t details[] ) {

    if ( ( _fw_l2_correctables[0].threshold == 0 ) || ( _fw_l2_correctables[0].threshold == 1 ) ) {
	return fw_l2_reportAndMaskCorrectableErrorHandler( details );
    }
    else {
	return fw_l2_accumulateCorrectablesHandler();
    }
}

int fw_l2_machineCheckHandler( uint64_t status[] ) {

  fw_uint64_t details[FW_NUM_L2_SLICES*4];
  fw_uint16_t n = 0;
  unsigned slice;

  if ( fw_l2_correctableErrorHandler(details) == 0 ) {
      return 0;
  }

  for ( slice = 0; slice < FW_NUM_L2_SLICES; slice++ ) {
    
    uint64_t state = L2_DCR_PRIV_PTR(slice)->l2_interrupt_state__machine_check;
    uint64_t error = L2_DCR_PRIV_PTR(slice)->interrupt_internal_error__machine_check;

    if ( ( state != 0 ) || ( error != 0 ) ) {
      details[n++] = L2_DCR(slice, L2_INTERRUPT_STATE__MACHINE_CHECK);
      details[n++] = state;
      details[n++] = L2_DCR(slice, INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK);
      details[n++] = error;
    }
  }

  fw_machineCheckRas( FW_RAS_L2_MACHINE_CHECK, details, n, __FILE__, __LINE__ );
  return -1;
}


__INLINE__ void write_derat(unsigned index, uint64_t hi, uint64_t lo) {
  mtspr(SPRN_MMUCR0, MMUCR0_TLBSEL_DERAT);
  asm volatile(
	       "eratwe %1,%0,1;"
	       "eratwe %2,%0,0;"
	       : : "r" (index), "r" (lo), "r" (hi)
	       : "memory"
	       );
}


int fw_l2_init( void ) {

  TRACE_ENTRY(TRACE_L2);

  uint64_t l2_dcr_ctrl = 0;
  
#ifndef FW_PREINSTALLED_GEA_HANDLERS
  
  uint64_t mask[3] = { L2_GEA_MASK_0, L2_GEA_MASK_1, L2_GEA_MASK_2 };
 
  fw_installGeaHandler( fw_l2_machineCheckHandler, mask );

#endif

  // NOTE: we have to initialize 16 L2 slices.  This could be done in parallel
  //       but that assumes that at least 16 cores are active (not true in
  //       various simulation environments).  Since slices are not inherently
  //       tied to cores, we loop here in one thread:

  int slice = 0;

  for ( slice = 0; slice < FW_NUM_L2_SLICES; slice++ ) {


      uint64_t control =
	  L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_set(1)       | // DCR ring parity error
	  L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__RQ_ERR_set(1)           | // Request queue error
	  L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__CTRL_ERR_set(1)         | // Controller error
	  L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__RDB_ERR_set(1)          | // Read buffer error
	  L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__COH_UE_set(1)           | // Coherence array uncorrectable error
	  L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__DIRB_UE_set(1)          | // Directory array uncorrectable error
	  L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__EDR_OUE_set(1)          | // Coherence array correctable error
	  L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__EDR_UE_set(1)           | // Coherence array ucorrectable error
	  L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LRU_UE_set(1)           | // LRU uncorrectable error
	  L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__SPR_UE_set(1)           | // SPR uncorrectable error
	  L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__RQ_UE_set(1)            | // Read queue uncorrectable error
	  L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__OVERLOCK_set(1)         | // Tried to lock more than overlock_thresh ways down
	  // [disabled]  L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__UPC_COUNTER_OVF_set(1)  | // UPC counter overflow
	  0;

      // +------------------------------------------------------------------------------------------+
      // | NOTE: For production environments, we mask DDR correctables during the early part of the |
      // |       boot.  The TakeCPU hook is what allows us to unmask.                               |
      // +------------------------------------------------------------------------------------------+
 
      if ( ! PERS_ENABLED(PERS_ENABLE_TakeCPU) ) {
	  control |=
	      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__COH_CE_set(1)           | // Coherence array correctable error
	      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__DIRB_CE_set(1)          | // Directory array correctable error
	      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__EDR_CE_set(1)           | // Coherence array correctable error
	      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__EDR_OCE_set(1)          | // Coherence array correctable error
	      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LRU_CE_set(1)           | // LRU correctable error
	      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__SPR_CE_set(1)           | // SPR correctable error
	      L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__RQ_CE_set(1)            | // Read queue correctable error
	      0;

      }

      DCRWritePriv( L2_DCR( slice, L2_INTERRUPT_STATE_CONTROL_HIGH ), control );

      DCRWritePriv( L2_DCR( slice, INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), 
		  // [5470] L2_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
		  // [5470] L2_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
		  L2_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
		  L2_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
		  0 );

  
    if (FW_DD1_WORKAROUNDS_ENABLED()) 
    {
      // Set the L2 refresh rate very high.  Revisit this setting for DD2
      uint64_t l2_dcr_refctrl = DCRReadPriv(L2_DCR(slice, REFCTRL));
      L2_DCR__REFCTRL__INTERVAL_insert(l2_dcr_refctrl, 0x0060);
      DCRWritePriv(L2_DCR(slice, REFCTRL), l2_dcr_refctrl);
    }

    l2_dcr_ctrl = DCRReadPriv(L2_DCR(slice, CTRL));
    if (FW_DD1_WORKAROUNDS_ENABLED()) 
    {
        // DD1 workaround for bqcbugs issue 1250.  Disable L2 out-of-order stuff.
        L2_DCR__CTRL__OOO_CTRL_insert(l2_dcr_ctrl, 0);
    }
    int ddrbitsize = 32 - cntlz32(FW_Personality.DDR_Config.DDRSizeMB) - 9 - 1; /* log2(DDRSize/512) */
    L2_DCR__CTRL__DDR_SIZE_insert(l2_dcr_ctrl, ddrbitsize);
    L2_DCR__CTRL__SI_DELAY_insert(l2_dcr_ctrl, 2);
    DCRWritePriv(L2_DCR(slice, CTRL), l2_dcr_ctrl);
    // This path should be activated for DD2.1. DD2.0 does not care, DD1 will hang on this DCR access
    if(ProcessorVersion() != SPRN_PVR_DD1){
      // issue 1606 fix activation
      // preload the L2 misshandler read counter to 4
      DCRWritePriv(L2_DCR(slice, CONFL_STAT)+8, 4);
    }
  }

  // Set correctable thresholds based on mode.
  // @todo Expose the threshold via the personality.

  fw_l2_resetCEThresholds();

  if (FW_DD1_WORKAROUNDS_ENABLED() && PERS_ENABLED(PERS_ENABLE_DDR) && ( ( DCRReadPriv(TESTINT_DCR(SLICE_SEL_CTRL)) & 0xFFFF000000000000ull ) == 0 ) )
  {


    // DD1 pseudo-workaround for bqcbugs issue 1213.  This is more of a
    // risk-mitigation measure than a true workaround.  The point is to run
    // dd1 tests in a way that can be reproduced on dd2, if necessary.  On
    // dd1, L2 way 0 is almost never evicted.  By locking way 0, we can control
    // what's in it and can also achieve the same behavior on dd2, should there
    // prove to be a problem with the issue 1213 ECO.
    // Map 16 megabytes temporarily, lock 2MB (one l2 way, presumably way 0),
    // and remove the temporary mapping.

    // NOTE: we avoid this if L2 slice control is prescribed (issue 5902)

    write_derat(4,
		ERAT0_EPN(0) | ERAT0_Class(1) | ERAT0_V |
		    ERAT0_Size_16M | ERAT0_ThdID(0xF),
		ERAT1_RPN(0) | ERAT1_R | ERAT1_C | ERAT1_M |
		    ERAT1_UX | ERAT1_UW | ERAT1_UR |
		    ERAT1_SX | ERAT1_SW | ERAT1_SR
		);
    isync();
    l2_lock_range((void *) 0, 2*1024*1024);
    write_derat(4, 0, 0);
    isync();
  }

  // See BQCBUGS 1620.  Disable L2 line locking.
  if ( ! PERS_ENABLED( PERS_ENABLE_DiagnosticsMode ) )
  {
      l2_set_overlock_threshold(0);
      l2_set_spec_threshold(0);
  }
  // --
  
#if 0
  
  if ( ProcessorID() == 0 ) {
      //uint64_t inject = L2_DCR__L2_INTERRUPT_STATE__DIRB_CE_set(1);
      uint64_t inject = L2_DCR__L2_INTERRUPT_STATE__EDR_CE_set(1);
      DCRWritePriv( L2_DCR( 7, L2_INTERRUPT_STATE__FORCE ), inject );
      ppc_msync();
  }

#endif 

  TRACE_EXIT(TRACE_L2);

  return 0;
}

int fw_l2_central_machineCheckHandler( uint64_t status[] ) {

  fw_uint64_t details[16];
  fw_uint16_t n = 0;

  details[n++] = L2_CENTRAL_DCR( L2_INTERRUPT_STATE__MACHINE_CHECK );
  details[n++] = L2_CENTRAL_DCR_PRIV_PTR->l2_interrupt_state__machine_check;

  details[n++] = L2_CENTRAL_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK );
  details[n++] = L2_CENTRAL_DCR_PRIV_PTR->interrupt_internal_error__machine_check;

  fw_machineCheckRas( FW_RAS_L2C_MACHINE_CHECK, details, n, __FILE__, __LINE__ );

  return -1;
}


int fw_l2_central_init( void ) {

  TRACE_ENTRY(TRACE_L2);

  // +-----------------------------------------------------------------------------------------------------+
  // | HW Issues 1496, 1516 : In order to avoid unitialized data and FIFOs in L2 Central, we force a write |
  // | followed by 4 reads.  This is technically a DD2 workaround but we do not antipate that it will be   |
  // | fixed in DD 2.1.                                                                                    |
  // +-----------------------------------------------------------------------------------------------------+

  volatile uint64_t* L2Central = (volatile uint64_t*)(PHYMAP_MINADDR_L2 | PHYMAP_PRIVILEGEDOFFSET);
  uint64_t tmp = 0;

  // Perform one write ....
  L2Central[0] = tmp; mbar();

  // Followed by 4 reads ...
  tmp = L2Central[0]; mbar();
  tmp = L2Central[1]; mbar();
  tmp = L2Central[2]; mbar();
  tmp = L2Central[3]; mbar();



#ifndef FW_PREINSTALLED_GEA_HANDLERS

  uint64_t mask[3] = { L2C_GEA_MASK_0, L2C_GEA_MASK_1, L2C_GEA_MASK_2 };

  fw_installGeaHandler( fw_l2_central_machineCheckHandler, mask );

#endif

  uint64_t interruptControl =
    L2_CENTRAL_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_set(1)  | // DCR ring parity error
    L2_CENTRAL_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__ECC_UE_set(1)      | // Uncorrectible devbus write data ECC
    L2_CENTRAL_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__ECC_CE_set(1)      | // Correctible devbus write data ECC
    0;

  if ( FW_DD1_WORKAROUNDS_ENABLED() ) {

    // +---------------------------------------------------------------------------------------------------+
    // | DD1 Workaround for Issue 1496 : Devbus is sending inverted ECC                                    |
    // +---------------------------------------------------------------------------------------------------+

    interruptControl &= ~(
			  L2_CENTRAL_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__ECC_UE_set(3)      | // Uncorrectible devbus write data ECC
			  L2_CENTRAL_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__ECC_CE_set(3)      | // Correctible devbus write data ECC
			  0
			  );
    
  }

  DCRWritePriv( L2_CENTRAL_DCR( L2_INTERRUPT_STATE_CONTROL_HIGH), 
		interruptControl);

  DCRWritePriv( L2_CENTRAL_DCR(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), 
		// [5470] L2_CENTRAL_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
		// [5470] L2_CENTRAL_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
		L2_CENTRAL_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
		L2_CENTRAL_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
		0 );


  TRACE_EXIT(TRACE_L2);
  return 0;
}


int fw_l2_counter_machineCheckHandler( uint64_t status[] ) {

  
  fw_uint64_t details[16];
  fw_uint16_t n = 0;
  unsigned unit ;

  for ( unit = 0; unit < L2_COUNTER_DCR_num; unit++ ) {
    
    uint64_t state = L2_COUNTER_DCR_PRIV_PTR(unit)->l2_interrupt_state__machine_check;
    uint64_t error = L2_COUNTER_DCR_PRIV_PTR(unit)->interrupt_internal_error__machine_check;

    if ( ( state != 0 ) || ( error != 0 ) ) {

      details[n++] = L2_COUNTER_DCR( unit, L2_INTERRUPT_STATE__MACHINE_CHECK );
      details[n++] = state;

      details[n++] = L2_COUNTER_DCR( unit, INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK );
      details[n++] = error;
    }
  }

  fw_machineCheckRas( FW_RAS_L2CTR_MACHINE_CHECK, details, n, __FILE__, __LINE__ );

  return -1;
}


int fw_l2_counter_init( void ) {

  TRACE_ENTRY(TRACE_L2);

  unsigned unit;

#ifndef FW_PREINSTALLED_GEA_HANDLERS

  uint64_t mask[3] = { L2CTR_GEA_MASK_0, L2CTR_GEA_MASK_1, L2CTR_GEA_MASK_2 };

  fw_installGeaHandler( fw_l2_counter_machineCheckHandler, mask );

#endif

  for ( unit = 0; unit < L2_COUNTER_DCR_num; unit++ ) {

    DCRWritePriv( L2_COUNTER_DCR( unit, L2_INTERRUPT_STATE_CONTROL_HIGH ), 
		L2_COUNTER_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_set(1)    | // Parity error on DCR ring.
		L2_COUNTER_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__BDRY_PAR_ERR_set(1)  | // Parity error on incoming counter request
		0 );

    DCRWritePriv( L2_COUNTER_DCR( unit, INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH ), 
		  // [5470] L2_COUNTER_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
		  // [5470] L2_COUNTER_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
		  L2_COUNTER_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
		  L2_COUNTER_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
		  0 );
  }


  TRACE_EXIT(TRACE_L2);

  return 0;
}

int fw_l2_preload()
{
    // Map 32 megabytes temporarily.  31 megabytes will be remapped later in
    // fw_mmu_init().  The last megabyte is initialized but left unmapped to
    // ensure that no prefetches hit uninitialized lines.
    write_derat(4,
		ERAT0_EPN(0) | ERAT0_Class(1) | ERAT0_V |
		    ERAT0_Size_16M | ERAT0_ThdID(0xF),
		ERAT1_RPN(0) | ERAT1_R | ERAT1_C | ERAT1_M |
		    ERAT1_UX | ERAT1_UW | ERAT1_UR |
		    ERAT1_SX | ERAT1_SW | ERAT1_SR
		);
    write_derat(5,
		ERAT0_EPN(16*1024*1024) | ERAT0_Class(1) | ERAT0_V |
		    ERAT0_Size_16M | ERAT0_ThdID(0xF),
		ERAT1_RPN(16*1024*1024) | ERAT1_R | ERAT1_C | ERAT1_M |
		    ERAT1_UX | ERAT1_UW | ERAT1_UR |
		    ERAT1_SX | ERAT1_SW | ERAT1_SR
		);

    if (PERS_ENABLED(PERS_ENABLE_DDR) && !PERS_ENABLED(PERS_ENABLE_JTagLoader))
    {
	// The test program has been preloaded into DDR.  We pull 31MB of DDR
	// up into L2 and then clear the last megabyte.  There should be no
	// further DDR accesses.
	uint64_t a;
	for (a = 0; a < (31*1024*1024); a += L2_CACHE_LINE_SIZE) {
	    dcbt_l2((void *) a);  // Pull line from DDR up into L2
	}
	for (a = (31*1024*1024); a < (32*1024*1024); a += L1_CACHE_LINE_SIZE) {
	    dcache_block_zero((void *) a);  // Clear line
	}
    } else {
	// Clear all 32 megabytes of the L2.  Either we're a bedram-only test
	// program that wants to exercise L2, or the test program will be
	// downloaded through the mailbox and written into L2 without
	// triggering DDR accesses.
	uint64_t a;
	for (a = 0; a < (32*1024*1024); a += L1_CACHE_LINE_SIZE) {
	    dcache_block_zero((void *) a); /*operating on NULL*/  // Clear line
	}
    }

    // Remove the temporary mappings.
    write_derat(4, 0, 0);
    write_derat(5, 0, 0);

    return 0;
}

void fw_l2_unmaskCorrectableErrors() {

    if ( PERS_ENABLED(PERS_ENABLE_TakeCPU) ) {
	
	int slice = 0;

	for ( slice = 0; slice < FW_NUM_L2_SLICES; slice++ ) {

	    uint64_t control = DCRReadPriv( L2_DCR( slice, L2_INTERRUPT_STATE_CONTROL_HIGH ) );

	    control |=
		L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__COH_CE_set(1)           | // Coherence array correctable error
		L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__DIRB_CE_set(1)          | // Directory array correctable error
		L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__EDR_CE_set(1)           | // Coherence array correctable error
		L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__EDR_OCE_set(1)          | // Coherence array correctable error
		L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LRU_CE_set(1)           | // LRU correctable error
		L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__SPR_CE_set(1)           | // SPR correctable error
		L2_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__RQ_CE_set(1)            | // Read queue correctable error
		0;

	    DCRWritePriv( L2_DCR( slice, L2_INTERRUPT_STATE_CONTROL_HIGH ), control );
	}
    }
}
