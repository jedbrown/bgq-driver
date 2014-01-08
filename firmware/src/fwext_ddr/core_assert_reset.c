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
#include <firmware/include/fwext/sd_init_auto.h>
#include <firmware/include/fwext/core_parms.h>
#include <firmware/include/fwext/sd_helper.h>
#include <firmware/include/fwext/core_dcr.h>

void assert_resets () {

	if (0 != (loopback_sd_en)) {
		return;
	}

	/******************************
	 * RCRESET                    *
	 ******************************/
	if (use_left()) {
		dcr_set(LEFT_RCBUS_CONFIG, get_ibm_bit64(LEFT_RCBUS_CONFIG_RESET[1]));
	}
	if (use_right()) {
		dcr_set(RIGHT_RCBUS_CONFIG, get_ibm_bit64(RIGHT_RCBUS_CONFIG_RESET[1]));
	}

	/******************************
	 * PCSRESET                   *
	 ******************************/
	pcs_reset_assert(use_am, use_ap, LEFT_TS_CTL_A);
	pcs_reset_assert(use_bm, use_bp, LEFT_TS_CTL_B);
	pcs_reset_assert(use_cm, use_cp, LEFT_TS_CTL_C);
	pcs_reset_assert(use_dm, use_dp, RIGHT_TS_CTL_D);
	pcs_reset_assert(use_em, use_ep, RIGHT_TS_CTL_E);
	pcs_reset_assert(use_io, 0     , RIGHT_TS_CTL_IO);

	/******************************
	 * PIPE0RESET                 *
	 ******************************/
	// Is active after power on

	/******************************
	 * SBRESET                    *
	 ******************************/
	if ((0 != (use_em + use_ep)) && 
    	    (0 == (use_am + use_ap + use_bm + use_bp + use_cm + use_cp + use_dm + use_dp + use_io))) {
		// assert sbreset
		rcb_write(9, 4, RCB_BT_SUPPORT, 0, 0x1c, 
			  get_std_bit16(0) | // override
			  get_std_bit16(6)); // sbreset
	} else {
		// assert sbreset
		rcb_write(9, 0xfff, RCB_BT_SUPPORT, 0, 0x1c, 
			  get_std_bit16(0) | // override
			  get_std_bit16(6)); // sbreset
	}

	/******************************
	 * LANERESET                  *
	 ******************************/
	// Is active after power on

	/******************************
	 * RXSET                      *
	 ******************************/
	// Is active after power on

	/******************************
	 * TXRESET                    *
	 ******************************/
	if ((0 != (use_em + use_ep)) && 
    	    (0 == (use_am + use_ap + use_bm + use_bp + use_cm + use_cp + use_dm + use_dp + use_io))) {
		// write TXCSR with txreset
		rcb_write(9, 4, RCB_BT_LANE, 15, 0x50, get_std_bit16(14) | get_std_bit16(15));
	} else {
		// write TXCSR with txreset
		rcb_write(9, 0xfff, RCB_BT_LANE, 15, 0x50, get_std_bit16(14) | get_std_bit16(15));
	}
}
