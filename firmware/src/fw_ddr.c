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
#include "Firmware_RAS.h"
#include <hwi/include/bqc/gea_dcr.h>
#include <hwi/include/bqc/dr_arb_dcr.h>
#include <hwi/include/bqc/ddr.h>
#include <hwi/include/bqc/testint_inlines.h>
#include <hwi/include/bqc/DDR_metrics.h>
#include <hwi/include/bqc/DDR_inlines.h>
#include <hwi/include/bqc/DDR_subsequences.h>
#include <hwi/include/bqc/DDR_ResumeSequence.h>

#define	BRINGUP 1

#if BRINGUP
#include <hwi/include/bqc/DDR_InitSequence.h>
#else
#define INSTANTIATE_DDR_INIT_TABLE 1
#include <hwi/include/bqc/ddr_inlines.h>
#endif


int fw_resetResumeMC();

#define FW_DDR_CE_THRESHOLD_INITIAL 10000

#define FW_DDR_CORRECTABLE_MASK_0 ( MCFIR_MEMORY_CE  )
#define FW_DDR_CORRECTABLE_MASK_1 ( \
    MCFIR_POWERBUS_WRITE_BUFFER_CE | \
    MCFIR_POWERBUS_READ_BUFFER_CE | \
    MCFIR_PACKET_COUNTER_DATA_CE | \
    MCFIR_MEMORY_ECC_MARKING_STORE_UPDATED | \
    MCFIR_ECC_ERROR_COUNTER_THRESHOLD_REACHED | \
    MCFIR_MAINTENANCE_CE | \
    MCFIR_MAINTENANCE_FAST_DECODER_UE | /* see [1] below */ \
    MCFIR_MAINTENANCE_SUE | /* see [1] below */ \
    MCFIR_POWERBUS_READ_BUFFER_SUE | /* see [1] below */ \
    0 )

#define FW_DDR_CORRECTABLE_MASK ( FW_DDR_CORRECTABLE_MASK_0 | FW_DDR_CORRECTABLE_MASK_1 )

typedef struct _FW_DDR_Correctables {
    fw_uint64_t mask;
    fw_uint64_t error_bits;
    fw_uint64_t threshold;
    fw_uint64_t timeOfLastEvent;
    fw_uint16_t count;
    fw_uint32_t ras_id;
} FW_DDR_Correctables;

static volatile FW_DDR_Correctables _fw_ddr_correctables[2] = {
    { FW_DDR_CORRECTABLE_MASK_0, 0, 1, 0, 0, FW_RAS_DDR_CORRECTABLE_SUMMARY },
    { FW_DDR_CORRECTABLE_MASK_1, 0, 1, 0, 0, FW_RAS_DDR_MAINTENANCE_CORRECTABLE_SUMMARY  }
};

void fw_ddr_flushCorrectables( int endOfJobFlush ) {

    int i;

    // Acquire the lock and test again:

    ppc_msync();
    fw_semaphore_down( BeDRAM_LOCKNUM_RAS_FLUSH_LOCK );

    for ( i = 0; i < sizeof(_fw_ddr_correctables) / sizeof(_fw_ddr_correctables[0]); i++ ) {

	FW_DDR_Correctables volatile * ced = & _fw_ddr_correctables[i];

	if ( ( ced->count >= ced->threshold ) || ( ( endOfJobFlush != 0 ) && ( ced->count > 0 ) ) ) {

	    fw_uint64_t details[8];
	    int n = 0;
	    
	    details[n++] = ced->count;
	    details[n++] = ced->error_bits;

	    fw_uint64_t currentTime = GetTimeBase();

	    //details[n++] = i; // DEBUG 2
	    //details[n++] = ProcessorID(); // DEBUG 3
	    //details[n++] = ced->threshold; // DEBUG 4
	    //details[n++] = ced->threshold; // DEBUG 5
	    //details[n++] = ced->timeOfLastEvent; // DEBUG 6
	    //details[n++] = BeDRAM_Read(BeDRAM_LOCKNUM_FWEXT_FIRST-1); // DEBUG 7

	    if ( endOfJobFlush == 0 ) {

		if ( ( currentTime - ced->timeOfLastEvent ) < 5ull * 1000ull * 1000ull * FW_Personality.Kernel_Config.FreqMHz ) {
		    ced->threshold = ced->threshold * 10;
		    //details[5] = 0xCCCCCCCC; // DEBUG -- identify threshold change
		}
	    }

	    fw_writeRASEvent( ced->ras_id, n, details );

	    ced->timeOfLastEvent = currentTime;
	    ced->count = 0;
	    ced->error_bits = 0;

	}
    }
    
    ppc_msync();
    fw_semaphore_up( BeDRAM_LOCKNUM_RAS_FLUSH_LOCK );
	
    if ( endOfJobFlush ) {
	_fw_ddr_correctables[0].threshold = _fw_ddr_correctables[1].threshold = FW_DDR_CE_THRESHOLD_INITIAL;
    }

}  

int fw_ddr_productionCorrectableHandler() {

    int i, unit;
    int rc = 0;
    int flush = 0;

    for ( unit = 0; unit < DR_ARB_DCR_num; unit++ ) {

	fw_uint64_t mcfir = DCRReadPriv(_DDR_MC_MCFIR(unit));
	
	if ( ( mcfir & ~FW_DDR_CORRECTABLE_MASK ) != 0 ) {
	    rc = -1;
	}

	if ( ( mcfir & FW_DDR_CORRECTABLE_MASK ) == 0 ) {
	    continue;
	}
	
	if ( ( mcfir & ~( MCFIR_MEMORY_CE | MCFIR_MAINTENANCE_CE ) ) != 0 ) {
	    DCRWritePriv(_DDR_MC_MCMCC(unit), 0);	// If more than CE, then keep scrubbing disabled

	    if ( ( mcfir & ( MCFIR_MEMORY_ECC_MARKING_STORE_UPDATED ) ) != 0 ) {
		DDR_Marking_Handler(unit);
	    }

	    DDR_PHY_Recal(unit,200);	// If more than CE, perform dynamic recal
	}
	else if ( ( mcfir & ( MCFIR_MEMORY_CE | MCFIR_MAINTENANCE_CE ) ) != 0 ) {

	    if ( PERS_ENABLED(PERS_ENABLE_DDRDirScrub) ) {
		uint64_t mcmcc_mask =  
		    MCMCC_MAINTENANCE_COMMAND_VALID |	// In case that scrubbing is not set to stop on CE
		    MCMCC_RESERVE_READ_BUFFER_REQUEST |
		    MCMCC_READ_BUFFERED_RESERVED |
		    MCMCC_STOP_ON_MAINT_TYPE1_END_ADDRESS;

		uint64_t mcmcc_expected = ( MCMCC_RESERVE_READ_BUFFER_REQUEST | MCMCC_READ_BUFFERED_RESERVED ) & ~( MCMCC_MAINTENANCE_COMMAND_VALID | MCMCC_STOP_ON_MAINT_TYPE1_END_ADDRESS );

		if ( ( DCRReadPriv(_DDR_MC_MCMCC(unit)) & mcmcc_mask ) == mcmcc_expected ) {

		    if ( mcfir & MCFIR_MAINTENANCE_CE ) {
			// Increment scrubbing address by +1
			if ( MEM_Increment_addr(unit) != 0 ) {
			    FW_Warning("Could not increment scrubbing address.");
			}
		    }
  

		    if ( mcfir & MCFIR_MEMORY_CE ) {
			// Rewind scrubbing address to the error location
			DCRWritePriv(_DDR_MC_MCMACA(unit), DCRReadPriv(_DDR_MC_MCRADR(unit)));
		    }

		    DCRWritePriv(_DDR_MC_MCMCC(unit), MCMCC_MAINTENANCE_COMMAND_VALID | MCMCC_RESERVE_READ_BUFFER_REQUEST | MCMCC_READ_BUFFERED_RESERVED );
		}
	    }
	    
	    if ( PERS_ENABLED( PERS_ENABLE_DDRDynamicRecal ) ) {
		DDR_PHY_Recal(unit,1000);
	    }
	}


	for ( i = 0; i < sizeof(_fw_ddr_correctables)/sizeof(_fw_ddr_correctables[0]); i++ ) {

	    FW_DDR_Correctables volatile * ced = & _fw_ddr_correctables[i];

	    if ( ( mcfir & ced->mask ) != 0 ) {
		
		ced->count++;
		ced->error_bits |= ( mcfir & ced->mask );
		
		if ( ced->count >= ced->threshold ) {
		    flush = 1;
		}
                
		// Ack the bits by writing 0's in the corresponding bits of the MCFIRC register:
		DCRWritePriv( _DDR_MC_MCFIRC(unit), ~(mcfir & ced->mask) );

		// Ack the interrupt bit:
		DCRWritePriv( DR_ARB_DCR(unit, L2_INTERRUPT_STATE__STATE), DR_ARB_DCR__L2_INTERRUPT_STATE__RECOV_ERR_set(1));

	    }
	}

	// Clear out accumulated hardware error counters:
	if ( mcfir & MCFIR_ECC_ERROR_COUNTER_THRESHOLD_REACHED ) {
	    DCRWritePriv(_DDR_MC_SWECOUNTA(unit), 0);
	    DCRWritePriv(_DDR_MC_SWECOUNTB(unit), 0);
	    DCRWritePriv(_DDR_MC_SWECOUNTO(unit), 0);
	}

    }

    if ( flush ) {
	fw_ddr_flushCorrectables(0);
    }

    return rc;
}

// NOTES:
//   [1] KH recommends that MAINTENTANCE_FAST_DECODER_UE be correctable.  This is because this only happens during scrubbing,
//       which also doesn't retry.  If the data is actually used later, it will be retried and might become a CE.  If not, 
//       it will be reported again as a UE at that time.  Likewise, the MAINTENANCE_SUE and POWERBUS_READ_BUFFER_SUE are
//       also treated this way.

int fw_ddr_legacyCorrectableHandler( fw_uint64_t details[] ) {

    int rc = 0; // assume that only correctables are present
    unsigned unit;
    unsigned n = 0;
    int directedScrubbingEnabled =  PERS_ENABLED(PERS_ENABLE_DDRDirScrub);
    int backgroundScrubbingEnabled =  PERS_ENABLED(PERS_ENABLE_DDRBackScrub);
    uint64_t correctableMask = FW_DDR_CORRECTABLE_MASK;


    for ( unit = 0; unit < DR_ARB_DCR_num; unit++ ) {

	fw_uint64_t mcfir = DCRReadPriv(_DDR_MC_MCFIR(unit));


	// non-maintenance drilldown

	if ( mcfir & ( MCFIR_MEMORY_CE | MCFIR_MEMORY_FAST_DECODER_UE | MCFIR_MEMORY_SUE | MCFIR_MEMORY_ECC_MARKING_STORE_UPDATED | MCFIR_ECC_ERROR_COUNTER_THRESHOLD_REACHED ) ) {

	    fw_uint64_t drilldown[6];

	    drilldown[0] = unit;
	    drilldown[1] = DCRReadPriv(_DDR_MC_MCRADR(unit));
	    drilldown[2] = DCRReadPriv(_DDR_MC_MCRESY(unit));;
	    drilldown[3] = __LINE__;
	    drilldown[4] = mcfir;
	    drilldown[5] = ~correctableMask;
	    fw_writeRASEvent( FW_RAS_DDR_DRILLDOWN, sizeof(drilldown)/sizeof(drilldown[0]), drilldown );
	}

	// maintenance drilldown:

	if ( mcfir & ( MCFIR_MAINTENANCE_CE | MCFIR_MAINTENANCE_FAST_DECODER_UE | MCFIR_MAINTENANCE_SUE | MCFIR_POWERBUS_READ_BUFFER_SUE ) ) {

	    fw_uint64_t drilldown[6];

	    drilldown[0] = unit;
	    drilldown[1] = DCRReadPriv(_DDR_MC_MCMACA(unit));
	    drilldown[2] = DCRReadPriv(_DDR_MC_MCSSY(unit));;
	    drilldown[3] = __LINE__;
	    drilldown[4] = mcfir;
	    drilldown[5] = ~correctableMask;
	    fw_writeRASEvent( FW_RAS_DDR_DRILLDOWN, sizeof(drilldown)/sizeof(drilldown[0]), drilldown );

	}

	// scrubbing handling

	if ( ( ( mcfir & ~( MCFIR_MEMORY_CE | MCFIR_MAINTENANCE_CE ) ) != 0 ) && ( backgroundScrubbingEnabled != 0 ) ) {
	    DCRWritePriv(_DDR_MC_MCMCC(unit), 0);	// If more than CE, then keep scrubbing disabled
	    FW_Warning( "Stopping Background Scrubbing Permanently unit=%d MCFIR=%X.", unit, mcfir);
	}
	else if ( ( mcfir & ( MCFIR_MEMORY_CE | MCFIR_MAINTENANCE_CE ) ) != 0 ) {

	    // Resume hardware scrubbing.  We take a little extra precaution to ensure that the read
	    // buffer reservation control and status is as expected.

	    uint64_t mcmcc_mask =  
		MCMCC_MAINTENANCE_COMMAND_VALID |	// In case that scrubbing is not set to stop on CE
		MCMCC_RESERVE_READ_BUFFER_REQUEST |
		MCMCC_READ_BUFFERED_RESERVED |
		MCMCC_STOP_ON_MAINT_TYPE1_END_ADDRESS;

	    uint64_t mcmcc_expected = ( MCMCC_RESERVE_READ_BUFFER_REQUEST | MCMCC_READ_BUFFERED_RESERVED ) & ~( MCMCC_MAINTENANCE_COMMAND_VALID | MCMCC_STOP_ON_MAINT_TYPE1_END_ADDRESS );

	    if ( ( DCRReadPriv(_DDR_MC_MCMCC(unit)) & mcmcc_mask ) == mcmcc_expected ) {

		if ( ( ( mcfir & MCFIR_MAINTENANCE_CE ) != 0 ) && ( directedScrubbingEnabled != 0 ) ) {

		    // Increment scrubbing address by +1

		    if ( MEM_Increment_addr(unit) != 0 ) {
			FW_Warning("Could not increment scrubbing address.");
		    }
		}

		if ( ( mcfir & MCFIR_MEMORY_CE ) && ( directedScrubbingEnabled != 0 ) ) {

		    // Rewind scrubbing address to the error location

		    DCRWritePriv(_DDR_MC_MCMACA(unit), DCRReadPriv(_DDR_MC_MCRADR(unit)) ); 
		}

		DCRWritePriv(_DDR_MC_MCMCC(unit), MCMCC_MAINTENANCE_COMMAND_VALID | MCMCC_RESERVE_READ_BUFFER_REQUEST | MCMCC_READ_BUFFERED_RESERVED );
	    }
	    else if ( ( directedScrubbingEnabled != 0 ) || ( backgroundScrubbingEnabled != 0 ) ) {

		// NOTE: Changed to Warning because this could happen in some normal cases when one-time scrubbing (bit6==1) or any other maintenance command
		//       is called by a test (e.g., ddr_bitfail, ddr_error_stress). This also could happen after background scrubbing is stopped permanently.

	        FW_Warning( "Background Scrubbing Not Resumed unit=%d MCMCC=%X.", unit, DCRReadPriv(_DDR_MC_MCMCC(unit)) );
	    }

	    if ( PERS_ENABLED( PERS_ENABLE_DDRDynamicRecal ) ) {
		DDR_PHY_Recal(unit,1000);
	    }
	}

	// reset error counter upon CTE

	if ( mcfir & MCFIR_ECC_ERROR_COUNTER_THRESHOLD_REACHED ) {
	    DCRWritePriv(_DDR_MC_SWECOUNTA(unit), 0);
	    DCRWritePriv(_DDR_MC_SWECOUNTB(unit), 0);
	    DCRWritePriv(_DDR_MC_SWECOUNTO(unit), 0);
	}

	if ( ( mcfir & ~correctableMask ) != 0 ) {
	    rc = -1; // There are uncorrectable errors
	}

	if ( ( mcfir & correctableMask ) != 0 ) {

	    mcfir &= correctableMask; // eliminate all but the correctable bits

	    details[n++] = _DDR_MC_MCFIR(unit);
	    details[n++] = mcfir;

	    details[n++] = _DDR_MC_MCFIRM(unit);
	    details[n++] = DCRReadPriv( _DDR_MC_MCFIRM(unit) );

	    details[n++] = DR_ARB_DCR( unit, L2_INTERRUPT_STATE__MACHINE_CHECK );
	    details[n++] = DCRReadPriv(DR_ARB_DCR( unit, L2_INTERRUPT_STATE__MACHINE_CHECK ));

	    if ( _fw_ddr_correctables[0].threshold == 0 ) {
		// Mask the error by writing 1's in the corresponding bits of the MCFIRMS register:
		DCRWritePriv( _DDR_MC_MCFIRMS(unit), mcfir );
	    }

	    // Ack the bits by writing 0's in the corresponding bits of the MCFIRC register:
	    DCRWritePriv( _DDR_MC_MCFIRC(unit), ~mcfir );

	    // Ack the interrupt bit:
	    DCRWritePriv( DR_ARB_DCR(unit, L2_INTERRUPT_STATE__STATE), DR_ARB_DCR__L2_INTERRUPT_STATE__RECOV_ERR_set(1));
	}
    }

    if ( n > 0 ) {
	fw_machineCheckRas( FW_RAS_DR_ARB_CORRECTABLE_MACHINE_CHECK, details, n, __FILE__, __LINE__ );
    }

    return rc;
}

int fw_ddr_correctableHandler( fw_uint64_t details[] ) {

    if ( _fw_ddr_correctables[0].threshold > 1 ) {
	return fw_ddr_productionCorrectableHandler();
    }
    else {
	return fw_ddr_legacyCorrectableHandler(details);
    }
}

int fw_ddr_machineCheckHandler( uint64_t status[] ) {

  fw_uint64_t details[16];
  fw_uint16_t n = 0;

  // [DEBUG] BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_FWEXT_FIRST-1);

  // Handle correctables first.  If all clear after doing that, then we will go on.
  if ( fw_ddr_correctableHandler(details) == 0 ) {
      return 0;
  }


  // Otherwise, there are some uncorrectable errors.  Handle them now as a fatal
  // condition:

  details[n++] = DR_ARB_DCR__A( L2_INTERRUPT_STATE__MACHINE_CHECK );
  details[n++] = DR_ARB_DCR_PRIV_PTR__A->l2_interrupt_state__machine_check;

  details[n++] = DR_ARB_DCR__A( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK );
  details[n++] = DR_ARB_DCR_PRIV_PTR__A->interrupt_internal_error__machine_check;

  details[n++] = DR_ARB_DCR__B( L2_INTERRUPT_STATE__MACHINE_CHECK );
  details[n++] = DR_ARB_DCR_PRIV_PTR__B->l2_interrupt_state__machine_check;

  details[n++] = DR_ARB_DCR__B( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK );
  details[n++] = DR_ARB_DCR_PRIV_PTR__B->interrupt_internal_error__machine_check;

  details[n++] = _DDR_MC_MCFIR(0);
  details[n++] = DCRReadPriv( _DDR_MC_MCFIR(0) );

  details[n++] = _DDR_MC_MCFIR(1);
  details[n++] = DCRReadPriv( _DDR_MC_MCFIR(1) );

  details[n++] = _DDR_MC_MCFIRM(0);
  details[n++] = DCRReadPriv( _DDR_MC_MCFIRM(0) );

  details[n++] = _DDR_MC_MCFIRM(1);
  details[n++] = DCRReadPriv( _DDR_MC_MCFIRM(1) );

  fw_machineCheckRas( FW_RAS_DR_ARB_MACHINE_CHECK, details, n, __FILE__, __LINE__ );

  DDR_UE_Diagnose(0);		// Callout of UE diagnosing routine
  DDR_UE_Diagnose(1);		// Callout of UE diagnosing routine


  return -1;
}

int fw_ddr_init( void ) {
   
  TRACE_ENTRY(TRACE_DDR);

  if ( ProcessorID() == 0 ) {

    if ( PERS_ENABLED( PERS_ENABLE_DDRINIT ) ) {

      if(BeDRAM_Read(BeDRAM_LOCKNUM_REPROCOUNTER) > 1) {
	fw_resetResumeMC();
      }
      else {
#if BRINGUP


	  // NOTE: the new sequence uses floating point instructions.
	  //       The FPU has not, in general, been initialized yet
	  //       so we force it here.

	  extern int fw_fpu_init( void );

	  fw_fpu_init();


	  struct DDRINIT_metrics ddr = { 0, };

	DDR_InitSequence(ddr, 0);
#else
	DDR_InitializeController();
#endif
#undef BRINGUP

	if ( ( PERS_ENABLED( PERS_ENABLE_DDRDynamicRecal ) ) && ( FW_IS_NODE_0 ) ) {
	    printf("(I) Dynamic Recalibration Enabled\n");
	}
      }
    }

    // +-----------------------------------------------------------------------------+
    // | NOTE: For now, the DDR controllers are missing in the FPGA environment and  |
    // |       so will machine check if we try to enable any of their interrupts.    |
    // |       @todo Re-enable if/when DDR controllers come alive on FPGA.           |
    // +-----------------------------------------------------------------------------+

    if ( PERS_ENABLED( PERS_ENABLE_FPGA ) == 0 ) {


#ifndef FW_PREINSTALLED_GEA_HANDLERS

      uint64_t mask[3] = { DDR_GEA_MASK_0, DDR_GEA_MASK_1, DDR_GEA_MASK_2 };

      fw_installGeaHandler( fw_ddr_machineCheckHandler, mask );

#endif

      uint64_t interruptControl =
	DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_set(1) | // DCR ring parity error
	DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__MC_ERR_set(1)     | // MC SCOM error
	DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__MCS_ERR_set(1)    | // MCS SCOM error
	DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__IOM_ERR_set(1)    | // IOM SCOM error
	DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__XSTOP_set(1)      | // MC check stop
	DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__SPEC_ATTN_set(1)  | // MC special attention
	DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__RECOV_ERR_set(1)  | // MC recoverable error
	0;

      uint64_t internalErrorControl =
	  // [5470] DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
	  // [5470] DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
	DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
	DR_ARB_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
	0;

      if ( FW_DD1_WORKAROUNDS_ENABLED() ) {

	// +---------------------------------------------------------------------------------------------------+
	// | DD1 Workaround for Issue 1224 : there is a one cycle delay in the parity generated by SCOM master |
	// | resulting in false positive parity errors.  These errors are disabled on DD1 hardware.            |
	// +---------------------------------------------------------------------------------------------------+

	interruptControl &= ~(
			      DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__MC_ERR_set(3)     | // MC SCOM error
			      DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__MCS_ERR_set(3)    | // MCS SCOM error
			      DR_ARB_DCR__L2_INTERRUPT_STATE_CONTROL_HIGH__IOM_ERR_set(3)    | // IOM SCOM error
			      0
			      );

      }

      DCRWritePriv( DR_ARB_DCR__A(L2_INTERRUPT_STATE_CONTROL_HIGH),       interruptControl );
      DCRWritePriv( DR_ARB_DCR__A(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), internalErrorControl );
      DCRWritePriv( DR_ARB_DCR__B(L2_INTERRUPT_STATE_CONTROL_HIGH),       interruptControl );
      DCRWritePriv( DR_ARB_DCR__B(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), internalErrorControl );


      //
      // Unmask errors at the FIR. 
      //
      // +-------------------------------------------------------------------------------------------+
      // |  NOTE: the polority of the bits in the memory controller's "clear" register are opposite  |
      // |        of what is common.  I.e., to clear a bit, you write a '0' ... to preserve a bit,   |
      // |        you write a '1'.                                                                   |
      // |                                                                                           |
      // |  DD1 NOTE: We are seeing spurious Marking Store Parity Errors in DD1 and this is a        |
      // |        documented bug in the pass 1 MC (HW092044).  For now, we ignore these by masking   |
      // |        them.                                                                              |
      // |                                                                                           |
      // |  DD1 NOTE: The read buffer underrun problem is being fixed on DD2. It is a false check    |
      // |        which may be set during the execution of a maintenance write operation.            |
      // |        Recommended workaround is just to mask this checker if it is being experienced.    |           
      // +-------------------------------------------------------------------------------------------+

      uint64_t fir = 0;

      if ( FW_DD1_WORKAROUNDS_ENABLED() ) {
	fir = 	  MCFIR_MEMORY_ECC_MARKING_STORE_PARITY_ERROR
		| MCFIR_READ_BUFFER_UNDERRUN;
      }

 
      // +------------------------------------------------------------------------------------------+
      // | NOTE: For production environments, we mask DDR correctables during the early part of the |
      // |       boot.  The TakeCPU hook is what allows us to unmask.                               |
      // +------------------------------------------------------------------------------------------+
 
      if ( PERS_ENABLED(PERS_ENABLE_TakeCPU) ) {
	  fir |= (
	      MCFIR_MEMORY_CE |
	      MCFIR_POWERBUS_WRITE_BUFFER_CE | 
	      MCFIR_POWERBUS_READ_BUFFER_CE |  
	      MCFIR_PACKET_COUNTER_DATA_CE |
	      MCFIR_MEMORY_ECC_MARKING_STORE_UPDATED | 
	      MCFIR_ECC_ERROR_COUNTER_THRESHOLD_REACHED |     
	      MCFIR_MAINTENANCE_CE |		       
	      MCFIR_MAINTENANCE_FAST_DECODER_UE | 
	      MCFIR_MAINTENANCE_SUE | 
	      MCFIR_POWERBUS_READ_BUFFER_SUE |
	      0 );
      }


#if 0
	uint64_t firact;

	firact = DCRReadPriv( _DDR_MC_MCFIRACT0(0) );
	firact &= ~MCFIR_MEMORY_ECC_MARKING_STORE_PARITY_ERROR;
	DCRWritePriv( _DDR_MC_MCFIRACT0(0), firact );

	firact = DCRReadPriv( _DDR_MC_MCFIRACT1(0) );
	firact &= ~MCFIR_MEMORY_ECC_MARKING_STORE_PARITY_ERROR;
	DCRWritePriv( _DDR_MC_MCFIRACT1(0), firact );

	firact = DCRReadPriv( _DDR_MC_MCFIRACT0(1) );
	firact &= ~MCFIR_MEMORY_ECC_MARKING_STORE_PARITY_ERROR;
	DCRWritePriv( _DDR_MC_MCFIRACT0(1), firact );

	firact = DCRReadPriv( _DDR_MC_MCFIRACT1(1) );
	firact &= ~MCFIR_MEMORY_ECC_MARKING_STORE_PARITY_ERROR;
	DCRWritePriv( _DDR_MC_MCFIRACT1(1), firact );
#endif

      DCRWritePriv( _DDR_MC_MCFIRMC(0), fir );   
      DCRWritePriv( _DDR_MC_MCFIRMC(1), fir );

#if 0
      printf( "FIRM  %lX %lX\n", DCRReadPriv( _DDR_MC_MCFIRM(0) ), DCRReadPriv( _DDR_MC_MCFIRM(1) ) );
      printf( "FIRA0 %lX %lX\n", DCRReadPriv( _DDR_MC_MCFIRACT0(0) ), DCRReadPriv( _DDR_MC_MCFIRACT0(1) ) );
      printf( "FIRA1 %lX %lX\n", DCRReadPriv( _DDR_MC_MCFIRACT1(0) ), DCRReadPriv( _DDR_MC_MCFIRACT1(1) ) );
#endif

    }

    if ( PERS_ENABLED( PERS_ENABLE_DiagnosticsMode ) ) {
	_fw_ddr_correctables[0].threshold = _fw_ddr_correctables[1].threshold = 1;
    }
    else if ( PERS_ENABLED( PERS_ENABLE_MaskCorrectables ) ) {
	_fw_ddr_correctables[0].threshold = _fw_ddr_correctables[1].threshold = 0;
    }
    else {
	_fw_ddr_correctables[0].threshold = _fw_ddr_correctables[1].threshold = FW_DDR_CE_THRESHOLD_INITIAL;
    }



   
    BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_DDRINIT);

  }

  while ( BeDRAM_Read(BeDRAM_LOCKNUM_DDRINIT) != 1 )
    /* spin */
    fw_pdelay(1600)
      ;

  TRACE_EXIT(TRACE_DDR);

  return( 0 );
}

int fw_ddr_flush_fifos()
{

  // +---------------------------------------------------------------------------------+
  // | DD1 Hardware Workaround (Issue 1241): The memory controllers' FIFOs come up in  |
  // |   an uninitialized state.  This is addressed by touching the first double-word  |
  // |   of each 128 byte line for the first 256 lines.  Idle cycles are inserted      |
  // |   between requests.                                                             |
  // +---------------------------------------------------------------------------------+
   
  if((PERS_ENABLED( PERS_ENABLE_Simulation ) == 0) &&
     (PERS_ENABLED( PERS_ENABLE_FPGA ) == 0) &&
     (FW_DD1_WORKAROUNDS_ENABLED()))
    {

      if ( ProcessorID() == 0 ) {

	int i;
	volatile uint64_t* p = (uint64_t*)0x0; /*dereferencing NULL*/

	uint64_t miscinit = DCRReadPriv(_DDR_MC_MCAMISC(0));
	uint64_t miscinit_alt = miscinit | MCAMISC_NUM_IDLES_set(120);
      
	ppc_msync();
	DCRWritePriv(_DDR_MC_MCAMISC(0),miscinit_alt);
	DCRWritePriv(_DDR_MC_MCAMISC(1),miscinit_alt);
 
	for(i=128*2; i; --i,p+=16) 
	  {
	    *p; /*dereferencing NULL*/
	  }

	ppc_msync();

	DCRWritePriv(_DDR_MC_MCAMISC(0),miscinit);
	DCRWritePriv(_DDR_MC_MCAMISC(1),miscinit);

	BeDRAM_ReadIncSat( BeDRAM_LOCKNUM_DDR_FIFOS_FLUSHED );
      }
      else {

	while ( BeDRAM_Read(BeDRAM_LOCKNUM_DDR_FIFOS_FLUSHED) == 0 )
	  fw_pdelay( 1600 );  /* spin */
	
      }
   }
   
   return 0;
}

void fw_ddr_unmaskCorrectableErrors() {

      uint64_t fir = 0;

      // +------------------------------------------------------------------------------------------+
      // | NOTE: For production environments, we mask DDR correctables during the early part of the |
      // |       boot.  The TakeCPU hook is what allows us to unmask.                               |
      // +------------------------------------------------------------------------------------------+

      if ( PERS_ENABLED(PERS_ENABLE_TakeCPU) ) {
	  fir |= 
	      MCFIR_MEMORY_CE |
	      MCFIR_POWERBUS_WRITE_BUFFER_CE | 
	      MCFIR_POWERBUS_READ_BUFFER_CE |  
	      MCFIR_PACKET_COUNTER_DATA_CE |
	      MCFIR_MEMORY_ECC_MARKING_STORE_UPDATED | 
	      MCFIR_ECC_ERROR_COUNTER_THRESHOLD_REACHED |     
	      MCFIR_MAINTENANCE_CE |		       
	      MCFIR_MAINTENANCE_FAST_DECODER_UE | 
	      MCFIR_MAINTENANCE_SUE | 
	      MCFIR_POWERBUS_READ_BUFFER_SUE |
	      0;
      }

      if ( FW_DD1_WORKAROUNDS_ENABLED() ) {
	  fir = MCFIR_MEMORY_ECC_MARKING_STORE_PARITY_ERROR | MCFIR_READ_BUFFER_UNDERRUN;
      }

      DCRWritePriv( _DDR_MC_MCFIRMC(0), ~fir ); // Note polarity of FIRMC bits
      DCRWritePriv( _DDR_MC_MCFIRMC(1), ~fir ); // Note polarity of FIRMC bits
}
