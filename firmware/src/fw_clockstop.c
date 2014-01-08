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
#include <hwi/include/bqc/cs_dcr.h>
#include <hwi/include/bqc/gea_dcr.h>

#include "Firmware_internals.h"
#include "Firmware_RAS.h"


int fw_clockstop_machineCheckHandler( uint64_t status[] ) {
  
  fw_uint64_t details[16];
  fw_uint16_t n = 0;

  details[n++] = CS_DCR( CLOCKSTOP_INTERRUPT_STATE__MACHINE_CHECK );
  details[n++] = CS_DCR_PRIV_PTR->clockstop_interrupt_state__machine_check;

  details[n++] = CS_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK );
  details[n++] = CS_DCR_PRIV_PTR->interrupt_internal_error__machine_check;

  fw_machineCheckRas( FW_RAS_CLOCKSTOP_MACHINE_CHECK, details, n, __FILE__, __LINE__ );

  return -1;
}


int fw_clockstop_init() {

  TRACE_ENTRY(TRACE_ClockStop);

  if ( ! PERS_ENABLED(PERS_ENABLE_FPGA) ) {

    /* ################################################################
     * #  Hardware Issue 930: We want to disable the BIC-to-clockstop #
     * #  connection.                                                 #
     * ################################################################ */
    
    CS_DCR_PRIV_PTR->controlreg_0 = CS_DCR__CONTROLREG_0_reset & ~CS_DCR__CONTROLREG_0__ENABLE_BIC_CS_STOP_set(1);
  }


  if ( PERS_ENABLED( PERS_ENABLE_ClockStop ) ) {

#ifndef FW_PREINSTALLED_GEA_HANDLERS 

    uint64_t mask[3] = { CLOCKSTOP_GEA_MASK_0, CLOCKSTOP_GEA_MASK_1, CLOCKSTOP_GEA_MASK_2 };

    fw_installGeaHandler( fw_clockstop_machineCheckHandler, mask );

#endif

    DCRWritePriv( CS_DCR( CLOCKSTOP_INTERRUPT_STATE_CONTROL_HIGH), 
		  CS_DCR__CLOCKSTOP_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_set(1)  | // DCR ring parity error.
		  // CS_DCR__CLOCKSTOP_INTERRUPT_STATE_CONTROL_HIGH__STOPPED_set(1)     | // the clocks will stop (if enabled)
		  0 );

    DCRWritePriv( CS_DCR(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), 
		  // [5470] CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
		  // [5470] CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
		  CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
		  CS_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
		  0 );

    if ( FW_Personality.Kernel_Config.ClockStop != 0 ) {
      
      /*
       * Clear the counter and clock stop status:
       */

      DCRWritePriv( CS_DCR(CONTROLREG_0), CS_DCR__CONTROLREG_0__CLEAR_COUNT_set(1) | CS_DCR__CONTROLREG_0__CLEAR_CLOCK_STOP_set(1) );
      ppc_msync();

      /*
       * Set the counter (threshold and phase), and enable counting:
       */

      DCRWritePriv( CS_DCR(THRESHOLD), CS_DCR__THRESHOLD__THRESHOLD_set( FW_Personality.Kernel_Config.ClockStop / 48 ));
      DCRWritePriv( CS_DCR(PHASE), CS_DCR__PHASE__PHASE_set( FW_Personality.Kernel_Config.ClockStop % 48 ));
      DCRWritePriv( CS_DCR(CONTROLREG_0), CS_DCR__CONTROLREG_0__ENABLE_STOP_OUTPUT_set(1));
      ppc_msync();
    }
  }
  
  TRACE_EXIT(TRACE_ClockStop);
  return 0;
}

int fw_clockstop_increment()
{
    if ( FW_Personality.Kernel_Config.ClockStop != 0 ) 
    {
        /*
         * Start incrementing:
         */
        DCRWritePriv( CS_DCR(CONTROLREG_1), CS_DCR__CONTROLREG_1__INCREMENT_set(1));
        isync();
        mtspr(SPRN_GSRR0, GetTimeBase());
    }
    return 0;
}
