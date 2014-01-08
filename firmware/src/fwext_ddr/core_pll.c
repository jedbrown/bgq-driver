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
#include <firmware/include/fwext/sd_rcb.h>
#include <firmware/include/fwext/core_parms.h>

////########################################################################
// This wakes up all PLLAs
//
void pma_wake_pll () 
{
	if ((0 != (use_em + use_ep)) && 
    	    (0 == (use_am + use_ap + use_bm + use_bp + use_cm + use_cp + use_dm + use_dp + use_io))) {
		pma_wake_pll_eonly();
	} else {
		pma_wake_pll_uni();
	}
}

void pma_wake_pll_eonly () 
{
	if (0 != (loopback_sd_en)) {
		return;
	}

	//spec page 75
	rcb_write(9, 4, RCB_BT_SUPPORT, 0, 0x1c, 
		get_std_bit16(0) | // override
		get_std_bit16(6) | // sbreset
		get_std_bit16(9)); // biasen

	//redo for Link E
	//spec page 69, quiesce PLL-A status, FIXME
	rcb_write(9, 4, RCB_BT_SUPPORT, 0, 0x02,
		get_std_bit16(15)); // trimforce, bit 14 is left 0 

	//spec page 75
	rcb_write(9, 4, RCB_BT_SUPPORT, 0, 0x1a, 
		get_std_bit16(8) |  // override
		get_std_bit16(15)); // bandgap enable

	//change for Link E
	rcb_write(9, 4, RCB_BT_SUPPORT, 0, 0x1c,
		get_std_bit16(0) | // override
		get_std_bit16(9) | // biasen
		std_move_upto(1,5)); // PLLB

	//redo for Link E
	rcb_write(9, 4, RCB_BT_SUPPORT, 0, 0x0c,
		get_std_bit16(0)    | // override
		get_std_bit16(1)    | // power-up enable for PLL
		std_move_upto(5, 3) | // refclk_mult = 50
		0                   | // pclkdiv8=0 for div10
		get_std_bit16(10)   | // gclken, lanes 0..3 active
		get_std_bit16(11));   // gclken, lanes 4..7 active
}

void pma_wake_pll_uni () 
{
	uint64_t val=0; /* not a valid default, just to make compiler happy */

	if (0 != (loopback_sd_en)) {
		return;
	}

        //################################################################
	// This is for Links A B C D IO,
	// Link E will also get the first broadcast settings but will be
	// overitten with specific values for PLL and Tenbits
	//#################################################################

	//spec page 75
	rcb_write(9, 0xfff, RCB_BT_SUPPORT, 0, 0x1c, 
		get_std_bit16(0) | // override
		get_std_bit16(6) | // sbreset
		get_std_bit16(9)); // biasen

	if (use_em + use_ep) {
		// save pllb calib reg default for undo
		val = rcb_read(9, 4, RCB_BT_SUPPORT, 0, 0x0e);
	}

	//spec page 69, quiesce PLL-B status, FIXME
	rcb_write(9, 0xfff, RCB_BT_SUPPORT, 0, 0x0e,
		get_std_bit16(15)); // trimforce, bit 14 is left 0 
		                    // to create PLLb_not_locked

	if (use_em + use_ep) {
		//undo for link E
		rcb_write(9, 4, RCB_BT_SUPPORT, 0, 0x0e, val);

		//redo for Link E
		//spec page 69, quiesce PLL-A status, FIXME
		rcb_write(9, 4, RCB_BT_SUPPORT, 0, 0x02,
			get_std_bit16(15)); // trimforce, bit 14 is left 0 
	}

	//spec page 75
	rcb_write(9, 0xfff, RCB_BT_SUPPORT, 0, 0x1a, 
		get_std_bit16(8) |  // override
		get_std_bit16(15)); // bandgap enable

	//spec page 75, reroute to use PLLA
	rcb_write(9, 0xfff, RCB_BT_SUPPORT, 0, 0x1c, 
		get_std_bit16(0) | // override
		get_std_bit16(9) | // biasen
		std_move_upto(0,5)); // PLLA

	if (0 != (use_em + use_ep)) {
		//change for Link E
		rcb_write(9, 4, RCB_BT_SUPPORT, 0, 0x1c,
			get_std_bit16(0) | // override
			get_std_bit16(9) | // biasen
			std_move_upto(1,5)); // PLLB
	}

	//spec page 68, we use PLLA for Torus
	rcb_write(9, 0xfff, RCB_BT_SUPPORT, 0, 0x00, 
		get_std_bit16(0)    | // override
		get_std_bit16(1)    | // power-up enable for PLL
		std_move_upto(4, 3) | // refclk_mult = 4
		get_std_bit16(7)    | // pclkdiv8
		get_std_bit16(10)   | // gclken, lanes 0..3 active
		get_std_bit16(11));   // gclken, lanes 4..7 active
	//get_std_bit16(8)    | # pclk enable
	
	if (0 != (use_em + use_ep)) {
		//undo for Link E
		rcb_write(9, 4, RCB_BT_SUPPORT, 0, 0x00, 0);

		//redo for Link E
		rcb_write(9, 4, RCB_BT_SUPPORT, 0, 0x0c,
			get_std_bit16(0)    | // override
			get_std_bit16(1)    | // power-up enable for PLL
			std_move_upto(5, 3) | // refclk_mult = 50
			0                   | // pclkdiv8=0 for div10
			get_std_bit16(10)   | // gclken, lanes 0..3 active
			get_std_bit16(11));   // gclken, lanes 4..7 active
	}
}

void pma_enable_pclk ()
{	

	if (0 != (use_am + use_ap + use_bm + use_bp + use_cm + use_cp + use_dm + use_dp + use_io)) {
		//spec page 68, we use PLLA for Torus
		rcb_write(9, 0xfff, RCB_BT_SUPPORT, 0, 0x00, 
			get_std_bit16(0)    | // override
			get_std_bit16(1)    | // power-up enable for PLL
			std_move_upto(4, 3) | // refclk_mult = 1
			get_std_bit16(7)    | // pclkdiv8
			get_std_bit16(8)    | // pclk enable
			get_std_bit16(10)   | // gclken, lanes 0..3 active
			get_std_bit16(11));   // gclken, lanes 4..7 active
		
		if (0 != (use_em + use_ep)) {
			//undo for Link E
			rcb_write(9, 4, RCB_BT_SUPPORT, 0, 0x00, 0);
		}
	}
	
	if (0 != (use_em + use_ep)) {
		//redo for Link E
		rcb_write(9, 4, RCB_BT_SUPPORT, 0, 0x0c,
			get_std_bit16(0)    | // override
			get_std_bit16(1)    | // power-up enable for PLL
			std_move_upto(5, 3) | // refclk_mult = 50
			0                   | // pclkdiv8=0 for div10
			get_std_bit16(8)    | // pclk enable
			get_std_bit16(10)   | // gclken, lanes 0..3 active
			get_std_bit16(11));   // gclken, lanes 4..7 active
	}
}
