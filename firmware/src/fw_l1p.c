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

/*------------------------------------------------------------------*/
/* Peter Boyle, Edinburgh University (for IBM), Jan 24 2009         */
/* Additional terms in letter agreement dated July 21,2011.         */
/*  See CoO for details.                                            */
/*------------------------------------------------------------------*/


#include <firmware/include/Firmware.h>
#include <hwi/include/bqc/l1p_mmio.h>
#include <hwi/include/bqc/l1p_dcr.h>
#include <hwi/include/bqc/gea_dcr.h>
#include <hwi/include/bqc/dc_arbiter_dcr.h>
#include <hwi/include/bqc/testint_inlines.h>

#include "Firmware_internals.h"
#include "Firmware_RAS.h"

#define FW_L1P_CORRECTABLE_MASK ( L1P_ESR_err_si_ecc | L1P_ESR_err_reload_ecc_x2 | L1P_ESR_err_sda_p )


static          fw_uint64_t  _fw_l1p_correctable_threshold = 1;        // The threshold for correctable error reporting.
static volatile fw_uint64_t  _fw_l1p_correctable_count = 0;            // The accumulated correctable error count.
static volatile fw_uint32_t  _fw_l1p_correctable_cores = 0;            // The accumulated cores that encountered correctable errors.
static volatile fw_uint64_t  _fw_l1p_correctable_error_bits = 0;       // The accumulated mask of correctable ESR bits.


void fw_l1p_resetCEThresholds( void ) {
  // Set correctable thresholds based on mode.
  // @todo Expose the threshold via the personality.

  if ( PERS_ENABLED( PERS_ENABLE_DiagnosticsMode ) ) {
      _fw_l1p_correctable_threshold = 1;
  }
  else if ( PERS_ENABLED( PERS_ENABLE_MaskCorrectables ) ) {
      _fw_l1p_correctable_threshold = 0;
  }
  else {
      _fw_l1p_correctable_threshold = 10000;
  }
}

uint64_t fw_l1p_readCrossCore( uint64_t address ) {

    int retries = 1000;
    uint64_t result = 0, esr = 0;

    while ( ( --retries ) > 0 ) {

	result = in64( (void *) address );
	esr = in64( (void*)L1P_ESR_DCR(ProcessorCoreID() ) );

	if ( ( esr & L1P_ESR_err_lu_dcr_abort ) == 0 ) {
	    return result;
	}

	out64_sync( (void*)L1P_ESR_DCR(ProcessorCoreID()),   L1P_ESR_err_lu_dcr_abort );
    }

    FW_Error( "Could not perform cross-core L1P register read addr=%X esr=%X.", address, esr );
    return -1;
}

void fw_l1p_writeCrossCore( uint64_t address, uint64_t value ) {

    int retries = 1000;
    uint64_t esr = 0;

    while ( ( --retries ) > 0 ) {

	out64_sync( (void *) address, value );
	esr = in64( (void*)L1P_ESR_DCR(ProcessorCoreID() ) );

	if ( ( esr & L1P_ESR_err_lu_dcr_abort ) == 0 ) {
	    return;
	}

	out64_sync( (void*)L1P_ESR_DCR(ProcessorCoreID()),   L1P_ESR_err_lu_dcr_abort );
    }

    FW_Error( "Could not perform cross-core L1P register write addr=%X value=%X esr=%X.", address, value, esr );    
}


void fw_l1p_flushCorrectables(int endOfJob ) {


    fw_semaphore_down( BeDRAM_LOCKNUM_RAS_FLUSH_LOCK );
	
    if ( _fw_l1p_correctable_count > 0 ) {

	fw_uint64_t details[8];
	int n = 0;

	details[n++] = _fw_l1p_correctable_count;
	details[n++] = _fw_l1p_correctable_error_bits;
	details[n++] = _fw_l1p_correctable_cores;

	_fw_l1p_correctable_count = _fw_l1p_correctable_error_bits = _fw_l1p_correctable_cores = 0;

	if ( endOfJob == 0 ) {
	    _fw_l1p_correctable_threshold *= 10;
	}

	fw_writeRASEvent( FW_RAS_L1P_CORRECTABLE_SUMMARY, n, details );
    }

    if ( endOfJob != 0 ) {
	fw_l1p_resetCEThresholds();
    }

    fw_semaphore_up( BeDRAM_LOCKNUM_RAS_FLUSH_LOCK );

}

void fw_l1p_handleCorrectable( uint64_t esr, uint64_t core ) {
    
    if ( ( _fw_l1p_correctable_threshold == 0 ) || ( _fw_l1p_correctable_threshold == 1 ) ) {

	fw_uint64_t details[4+17];
	int n = 0;

	details[n++] = L1P_DCR( core,INTERRUPT_STATE_A__MACHINE_CHECK);
	details[n++] = L1P_DCR_PRIV_PTR(core)->interrupt_state_a__machine_check;
	details[n++] = L1P_ESR_DCR(core);
	details[n++] = esr;

	if ( _fw_l1p_correctable_threshold == 0 ) { 
	    // Mask this interrupt from happening again.
	    fw_l1p_writeCrossCore( L1P_ESR_GEA_DCR(core), fw_l1p_readCrossCore(L1P_ESR_GEA_DCR(core)) & ~(esr & FW_L1P_CORRECTABLE_MASK) );
	}

	fw_machineCheckRas( FW_RAS_L1P_CORRECTABLE_MACHINE_CHECK, details, n, __FILE__, __LINE__ );
    }
    else {

	_fw_l1p_correctable_count++;
	_fw_l1p_correctable_cores |= (1 << core);
	_fw_l1p_correctable_error_bits |= esr;

	if ( _fw_l1p_correctable_count >= _fw_l1p_correctable_threshold ) {
	    fw_l1p_flushCorrectables(0);
	}
    }

    fw_l1p_writeCrossCore( L1P_ESR_DCR(core), esr & FW_L1P_CORRECTABLE_MASK ); // Ack the interrupt

}


int fw_l1p_machineCheckHandler( uint64_t mappedStatus[] ) {

    uint64_t core, pass;
    fw_uint64_t details[17*2*6];
    unsigned n = 0;
    int rc = 0; // unless set otherwise below
    uint64_t correctableMask = FW_L1P_CORRECTABLE_MASK;

    // +--------------------------------------------------------------------+
    // | HW Issue 1596 workaround:  Disable DCR Arbiter Timeouts            |
    // +--------------------------------------------------------------------+

    uint64_t dcr_arbiter_ctrl_low = DCRReadPriv(DC_ARBITER_DCR( INT_REG_CONTROL_LOW));
    DCRWritePriv( DC_ARBITER_DCR( INT_REG_CONTROL_LOW), dcr_arbiter_ctrl_low & ~DC_ARBITER_DCR__INT_REG_CONTROL_LOW__NO_ACK_AFTER_REQ_set(-1) );

    for ( pass = 0; pass < 2; pass++ ) {

	n = 0;

	uint64_t statusMask = GEA_DCR__GEA_MAPPED_INTERRUPT_STATUS0_0__L1P0_RT_INT_set(1);

	for (core = 0; core < 17; core++, statusMask >>= 1 ) {

	    // +---------------------------------------------------------------------------------------+
	    // |  HW Issue 1596 : Only perform a cross-core ESR access if the mapped status indicates  |
	    // |                  that it is interesting to do so.  This reduces the exposure to the   |
	    // |                  hardware bug documented in that issue.                               |
	    // +---------------------------------------------------------------------------------------+

	    if ( ( mappedStatus[0] & statusMask ) != 0 ) {

		uint64_t status, error;

		status = fw_l1p_readCrossCore( L1P_ESR_DCR(core) );

		status &= ( ( pass == 0 ) ?  correctableMask :  ~correctableMask );

		if ( status == 0 )
		    continue;

		if ( pass == 0 ) {

		    fw_l1p_handleCorrectable( status, core );

		    // +------------------------------------------------------------------------------------------+
		    // | This funky little sequence seems to be required to clean up the A2 machine check bit     |
		    // | (bit 0) of the ESR.  It came from Krishnan  and does the following:                      |
		    // |                                                                                          |
		    // |     o   masks interrupts                                                                 |
		    // |     o   clears the MCSR[EXT] bit                                                         |
		    // |     o   clears the L1P_ESR[0] bit                                                        |
		    // |     o   re-enables interrupts                                                            |
		    // |                                                                                          |
		    // +------------------------------------------------------------------------------------------+
		
		    uint64_t thisCore = ProcessorCoreID();
		    uint64_t esrMask = in64( (void*)L1P_ESR_GEA_DCR(thisCore));
		    out64_sync( (void*)L1P_ESR_GEA_DCR(thisCore), 0 );
		    mtspr( SPRN_MCSR, mfspr(SPRN_MCSR) & ~MCSR_EXT );
		    out64_sync( (void*)L1P_ESR_DCR(thisCore), L1P_ESR_a2_machine_check );
		    out64_sync( (void*)L1P_ESR_GEA_DCR(thisCore), esrMask );
		}
		else {

		    details[n++] = L1P_ESR_DCR(core);
		    details[n++] = status;

		    status = L1P_DCR_PRIV_PTR(core)->interrupt_state_a__machine_check;
		    error  = L1P_DCR_PRIV_PTR(core)->interrupt_internal_error__machine_check;

		    if ( status != 0 ) {
			details[n++] = L1P_DCR( core,INTERRUPT_STATE_A__MACHINE_CHECK);
			details[n++] = status;
		    }
	    
		    if ( error != 0 ) {
			details[n++] = L1P_DCR(core,INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH);
			details[n++] = error;
		    }

		    rc = -1;
		}
	    }
 	}

	if ( n > 0 ) {
	    fw_machineCheckRas( FW_RAS_L1P_MACHINE_CHECK, details, n, __FILE__, __LINE__ );
	}
    }

    // +--------------------------------------------------------------------------+
    // | HW Issue 1596 workaround:  Force-clear any DCR timeouts and restore the  |
    // |   control register.                                                      |
    // +--------------------------------------------------------------------------+

    DCRWritePriv( DC_ARBITER_DCR( INT_REG__STATE ), DC_ARBITER_DCR__INT_REG__NO_ACK_AFTER_REQ_set(1) );
    ppc_msync();
    DCRWritePriv( DC_ARBITER_DCR( INT_REG_CONTROL_LOW), dcr_arbiter_ctrl_low );
	
    return rc;
}


/*!
  \brief Initializes the Blue Gene/Q L1p unit for the caller's core
  
  \warning The other 3 threads on the caller's core must be quiesced and cannot access L2 during this function.  
 */

int fw_l1p_init( void )  {

  TRACE_ENTRY(TRACE_L1P);

  uint64_t cfg_spec   = L1P_CFG_SPEC_l1_hit_fwd_l2;

  uint64_t cfg_pf_usr = L1P_CFG_PF_USR_dfetch_depth(2)
                      | L1P_CFG_PF_USR_dfetch_max_footprint(7)   
                      | L1P_CFG_PF_USR_ifetch_depth(0)       
                      | L1P_CFG_PF_USR_ifetch_max_footprint(2)   
                      | L1P_CFG_PF_USR_pf_stream_est_on_dcbt 
      // [DISABLED] | L1P_CFG_PF_USR_pf_stream_optimistic  
                      | L1P_CFG_PF_USR_pf_stream_prefetch_enable
                      | L1P_CFG_PF_USR_pf_stream_establish_enable
                      | L1P_CFG_PF_USR_pf_adaptive_enable    
                      | L1P_CFG_PF_USR_pf_adaptive_throttle(0xF) ;
    /* UNUSED         | L1P_CFG_PF_USR_pf_list_enable        */
  
  uint64_t cfg_pf_sys = 
      L1P_CFG_PF_SYS_msync_timer(7+3)            
      |  L1P_CFG_PF_SYS_pfhint_enable             
      |  L1P_CFG_PF_SYS_whint_evict_enable        
      |  L1P_CFG_PF_SYS_whint_cracked_enable      
      |  L1P_CFG_PF_SYS_lock_prefetch             
      |  L1P_CFG_PF_SYS_dcbfl_discard             
      |  L1P_CFG_PF_SYS_pf_adaptive_total_depth(24)
      |  L1P_CFG_PF_SYS_pf_hit_enable              
      |  L1P_CFG_PF_SYS_pf_stream_l2_op_immediate  ;
  
  if(!FW_DD1_WORKAROUNDS_ENABLED())
  {
      cfg_pf_sys |= L1P_CFG_PF_SYS_wrap_bug_dd2_bhv;
  }
  
  uint64_t cfg_wc    =  L1P_CFG_WC_wc_enable
      | L1P_CFG_WC_wc_suppress_if_all_be 
      | L1P_CFG_WC_wc_aging ;

  uint64_t cfg_to    =  L1P_CFG_TO_to_en 
      | L1P_CFG_TO_to_reload_en
      | L1P_CFG_TO_to_duration(0x3) ;

  uint64_t cfg_upc   =  L1P_CFG_UPC_ENABLE
      | L1P_CFG_UPC_STREAM;

  out64_sync((void *)L1P_CFG_SPEC,cfg_spec);
  
  out64_sync((void *)L1P_CFG_PF_USR,cfg_pf_usr);
  
  out64_sync((void *)L1P_CFG_PF_SYS,L1P_CFG_PF_SYS_pf_adaptive_reset|cfg_pf_sys);
  out64_sync((void *)L1P_CFG_PF_SYS,cfg_pf_sys);
  
  out64_sync((void *)L1P_CFG_WC,cfg_wc);
  
  out64_sync((void *)L1P_CFG_TO,cfg_to);

  out64_sync((void *)L1P_CFG_UPC,cfg_upc);

  /* Enable L1p hardware error interrupts */

  uint64_t esr_gea =
      // [disabled] L1P_ESR_int_list_0 |
      // [disabled] L1P_ESR_int_list_1 |
      // [disabled] L1P_ESR_int_list_2 |
      // [disabled] L1P_ESR_int_list_3 |
      // [disabled] L1P_ESR_int_list_4	|
      // [disabled] L1P_ESR_int_speculation_0 |
      // [disabled] L1P_ESR_int_speculation_1 |
      // [disabled] L1P_ESR_int_speculation_2 |
      // [disabled] L1P_ESR_int_speculation_3 |
      // [disabled] L1P_ESR_err_valid_timeout | [see bqcbugs #1612]
      L1P_ESR_err_luq_ovfl |
      L1P_ESR_err_sr_p |
      L1P_ESR_err_sr_rd_valid_p |
      L1P_ESR_err_sw_p |
      L1P_ESR_err_si_ecc_ue |
      L1P_ESR_err_si_p |
      L1P_ESR_err_sda_p_ue |
      L1P_ESR_err_rqra_p |
      L1P_ESR_err_reload_ecc_ue_x2 |
      L1P_ESR_err_rira_p |
      L1P_ESR_err_gctr_p	|
      L1P_ESR_err_lu_state_p |
      L1P_ESR_err_lu_ttype |
      // [5470] L1P_ESR_err_lu_dcr_abort |
      L1P_ESR_err_mmio_async |
      L1P_ESR_err_mmio_state_p |
      L1P_ESR_err_mmio_timeout |
      L1P_ESR_err_mmio_priv |
      L1P_ESR_err_mmio_rdata_p |
      L1P_ESR_err_mmio_wdata_p |
      L1P_ESR_err_mmio_dcrs_timeout |
      L1P_ESR_err_mmio_dcrs_priv |
      L1P_ESR_err_mmio_dcrs_par |
      L1P_ESR_err_dcrm_crit |
      L1P_ESR_err_dcrm_noncrit |
      // [5470] L1P_ESR_err_dcrm_mc |
      L1P_ESR_err_tag_timeout |
      L1P_ESR_err_hold_timeout |
      L1P_ESR_err_ditc_req_x2 |
      L1P_ESR_err_pfd_addr_p |
      L1P_ESR_err_pfd_avalid_p |
      L1P_ESR_err_pfd_fill_pnd_p |
      L1P_ESR_err_pfd_hit_pnd_p |
      L1P_ESR_err_pfd_stream_p |
      L1P_ESR_err_pfd_depth_p |
      L1P_ESR_err_pfd_clone_p |
      L1P_ESR_err_hitq_p |
      L1P_ESR_err_sd_p |
      L1P_ESR_err_pf2dfc_p |
      L1P_ESR_err_wccm_p_x2 |
      L1P_ESR_err_wccm_wcd_p_x2 |
      L1P_ESR_err_lu_wcd_p |
      L1P_ESR_err_lu_current_p |
      L1P_ESR_err_l2cmd |
      L1P_ESR_err_lu_dcr_dbus_p |
      L1P_ESR_err_luq_p |
      L1P_ESR_err_sda_phase_p |
      L1P_ESR_slice_sel_ctrl_perr |
      L1P_ESR_redun_ctrl_perr
      ;

  // +------------------------------------------------------------------------------------------+
  // | NOTE: For production environments, we mask L1P correctables during the early part of the |
  // |       boot.  The TakeCPU hook is what allows us to unmask.                               |
  // +------------------------------------------------------------------------------------------+

  if ( ! PERS_ENABLED(PERS_ENABLE_TakeCPU) ) { 
      esr_gea |=
	  L1P_ESR_err_si_ecc |
	  L1P_ESR_err_reload_ecc_x2 |
	  L1P_ESR_err_sda_p
	  ;

  }

  out64_sync(
      (void *)L1P_ESR_GEA, 
      esr_gea
      );


#ifndef FW_PREINSTALLED_GEA_HANDLERS 

  uint64_t mask[3] = { L1P_GEA_MASK_0, L1P_GEA_MASK_1, L1P_GEA_MASK2  };

  fw_installGeaHandler( fw_l1p_machineCheckHandler, mask );

#endif


  unsigned core = ProcessorCoreID();

  DCRWritePriv( L1P_DCR(core,INTERRUPT_STATE_A_CONTROL_HIGH), 
		L1P_DCR__INTERRUPT_STATE_A_CONTROL_HIGH__LOCAL_RING_set(1)  | // Global Interrupt
		0 );

  DCRWritePriv( L1P_DCR(core,INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), 
		// [5470] L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
		// [5470] L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
		L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
		L1P_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
		0 );

  if(A2_isDD1())
  {
      *(volatile uint64_t*)L1P_CFG_CLK_GATE = _B1(61,1);
  }
  else
  {
      *(volatile uint64_t*)L1P_CFG_CLK_GATE = L1P_CFG_CLK_GATE_clk_on_sw_req;
  }

  if(!FW_DD1_WORKAROUNDS_ENABLED()) 
  {
      *(volatile uint64_t*)L1P_CFG_CHICKEN |= L1P_CFG_CHICKEN_DD2;
  }
  

  fw_l1p_resetCEThresholds();


#if 0

  if ( ProcessorCoreID() == 2 ) {
  // DO NOT INTEGRATE THIS CODE!!!!!!!!!!!
  uint64_t inject = 
      L1P_ESR_err_reload_ecc_x2 |
      //L1P_ESR_err_si_ecc |
      //L1P_ESR_err_reload_ecc_ue_x2 |
      0  ;
	
  out64_sync((void *)L1P_ESR_INJ_DCR(ProcessorCoreID()), inject );
  ppc_msync();
  out64_sync((void *)L1P_ESR_INJ_DCR(ProcessorCoreID()), 0 );
  ppc_msync();
  }

#endif


  TRACE_EXIT(TRACE_L1P);

  return( 0 );
}


void fw_l1p_unmaskCorrectableErrors() {

    if ( PERS_ENABLED(PERS_ENABLE_TakeCPU) ) {

	uint64_t esr_gea = in64( (void *)L1P_ESR_GEA );
	
	esr_gea |=
	  L1P_ESR_err_si_ecc |
	  L1P_ESR_err_reload_ecc_x2 |
	  L1P_ESR_err_sda_p
	  ;

	out64_sync(  (void *)L1P_ESR_GEA,  esr_gea );

    }
}
