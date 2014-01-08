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
#include <firmware/include/fwext/sd_rcb.h>
#include <firmware/include/fwext/core_dcr.h>

//#########################################################################
// This wakes up the HSS lanes
//
void pma_wake_lanes ()
{
	if (0 != (loopback_sd_en)) {
		return;
	}

	if ((0 != (use_em + use_ep)) && 
    	    (0 == (use_am + use_ap + use_bm + use_bp + use_cm + use_cp + use_dm + use_dp + use_io))) {
		pma_wake_lanes_eonly();
	} else {
		pma_wake_lanes_uni();
	}
	lanes_release_reset();
}

void pma_wake_lanes_eonly ()
{
	int subrate = (int)(sd_en_halfrate != 0);

	// Enable Termination
	rcb_write(9, 4, RCB_BT_LANE, 15, 0x22,                                                                    
		  get_std_bit16(0) |      // termination control override
		  get_std_bit16(1));      // termination enable

	// RX Control and Status Register, page 55
	uint16_t val = get_std_bit16(0)    | // enable override
		       get_std_bit16(2)    | // receiver clock enable
	  	       get_std_bit16(3)    | // analog enable
		       get_std_bit16(8)    | // subrate_override
		       std_move_upto(subrate, 9) | // subrate = 0
		       get_std_bit16(15);    // vreg enable

	// write RXCSR with reset
	rcb_write(9, 4, RCB_BT_LANE, 15, 0x20, val | get_std_bit16(14));

	// Link E uses strappings, FIXME should be done for others also
	rcb_write(9, 4, RCB_BT_LANE, 15, 0x00, 0);

	// TX Control and Status Regiser, page 63
	val = get_std_bit16(0)    | // enable override
	      get_std_bit16(1)    | // clock enable
	      get_std_bit16(2)    | // drive enable
	      get_std_bit16(3)    | // analog enable
	      get_std_bit16(5)    | // subrate override
	      std_move_upto(subrate, 6);  // subrate 0

	// flush rcb
	rcb_read(9, 4, RCB_BT_LANE, 0, 0x50);

	// write TXCSR with txreset
	rcb_write(9, 4, RCB_BT_LANE, 15, 0x50, 
		  val | get_std_bit16(14) | get_std_bit16(15));

	// flush rcb - 0
	rcb_read(9, 4, RCB_BT_LANE, 0, 0x50);
}

void pma_wake_lanes_uni ()
{
	int subrate = (int)(sd_en_halfrate != 0);

	// Enable Termination
	rcb_write(9, 0xfff, RCB_BT_LANE, 15, 0x22,                                                                    
		get_std_bit16(0) |      // termination control override
	        get_std_bit16(1));      // termination enable

	// RX Control and Status Register, page 55
	uint16_t val = get_std_bit16(0)    | // enable override
		       get_std_bit16(2)    | // receiver clock enable
		       get_std_bit16(3)    | // analog enable
		       get_std_bit16(8)    | // subrate_override
		       std_move_upto(subrate, 9) | // subrate 
		       get_std_bit16(15);    // vreg enable

	// write RXCSR (no rxreset)
	rcb_write(9, 0xfff, RCB_BT_LANE, 15, 0x20, val);

	//################################################################
	// This is for Links A B C D IO,
	// Link E will also get the first broadcast settings but will be
	// overitten with specific values for PLL and Tenbits
	//################################################################

	// Link Configuration Register, page 49
	rcb_write(9, 0xfff, RCB_BT_LANE, 15, 0x00,
		get_std_bit16(0)    | // tenbits_ovr=1
		                      // tenbits is left 0
		get_std_bit16(2));    // pll_select_ovr=1
		                      // pll_select is left 0 for PLL!

	if (use_em + use_ep) {
		// Now Link E again
		rcb_write(9, 4, RCB_BT_LANE, 15, 0x00, 0);
	}

	// TX Control and Status Regiser, page 63
	val = get_std_bit16(0)    | // enable override
	      get_std_bit16(1)    | // clock enable
	      get_std_bit16(2)    | // drive enable
	      get_std_bit16(3)    | // analog enable
	      get_std_bit16(5)    | // subrate override
	      std_move_upto(subrate, 6);  // subrate

	// write TXCSR
	rcb_write(9, 0xfff, RCB_BT_LANE, 15, 0x50, 
			val | get_std_bit16(14) | get_std_bit16(15));

	// "Flush rcb - 0 \n";
	if(use_left())  rcb_read(9, 0, RCB_BT_LANE, 0, 0x50);
	if(use_right()) rcb_read(9, 3, RCB_BT_LANE, 0, 0x50);
}

void  lanes_release_reset ()
{
	// Now release lanes from reset (p and m) , FIXME use same values for all DCRs
	if (1 == use_am) {dcr_clr_acc(LEFT_TS_CTL_A, quickmask(LEFT_TS_CTL_A_LANE_RESET_M));}
	if (1 == use_ap) {dcr_clr_acc(LEFT_TS_CTL_A, quickmask(LEFT_TS_CTL_A_LANE_RESET_P));}
	if (use_am+use_ap > 0) {dcr_acc_flush(LEFT_TS_CTL_A);}

	if (1 == use_bm) {dcr_clr_acc(LEFT_TS_CTL_B, quickmask(LEFT_TS_CTL_B_LANE_RESET_M));}
	if (1 == use_bp) {dcr_clr_acc(LEFT_TS_CTL_B, quickmask(LEFT_TS_CTL_B_LANE_RESET_P));}
	if (use_bm+use_bp > 0) {dcr_acc_flush(LEFT_TS_CTL_B);}

	if (1 == use_cm) {dcr_clr_acc(LEFT_TS_CTL_C, quickmask(LEFT_TS_CTL_C_LANE_RESET_M));}
	if (1 == use_cp) {dcr_clr_acc(LEFT_TS_CTL_C, quickmask(LEFT_TS_CTL_C_LANE_RESET_P));}
	if (use_cm+use_cp > 0) {dcr_acc_flush(LEFT_TS_CTL_C);}

	if (1 == use_dm) {dcr_clr_acc(RIGHT_TS_CTL_D, quickmask(RIGHT_TS_CTL_D_LANE_RESET_M));}
	if (1 == use_dp) {dcr_clr_acc(RIGHT_TS_CTL_D, quickmask(RIGHT_TS_CTL_D_LANE_RESET_P));}
	if (use_dm+use_dp > 0) {dcr_acc_flush(RIGHT_TS_CTL_D);}

	if (1 == use_em) {dcr_clr_acc(RIGHT_TS_CTL_E, quickmask(RIGHT_TS_CTL_E_LANE_RESET_M));}
	if (1 == use_ep) {dcr_clr_acc(RIGHT_TS_CTL_E, quickmask(RIGHT_TS_CTL_E_LANE_RESET_P));}
	if (use_em+use_ep > 0) {dcr_acc_flush(RIGHT_TS_CTL_E);}

	if (1 == use_io) {dcr_clr_acc(RIGHT_TS_CTL_IO, quickmask(RIGHT_TS_CTL_IO_LANE_RESET));}
	if (use_io > 0)  {dcr_acc_flush(RIGHT_TS_CTL_IO);}
}

void lanes_rxtxrst_deassert ()
{
        int subrate = (int)(sd_en_halfrate != 0);

	int txval = get_std_bit16(0) |         // enable override
		    get_std_bit16(1) |         // clock enable
		    get_std_bit16(2) |         // drive enable
		    get_std_bit16(3) |         // analog enable
		    get_std_bit16(5) |         // subrate override
		    std_move_upto(subrate, 6); // subrate 

	rx_reset_deassert(use_am, use_ap, LEFT_TS_CTL_A);
	rx_reset_deassert(use_bm, use_bp, LEFT_TS_CTL_B);
	rx_reset_deassert(use_cm, use_cp, LEFT_TS_CTL_C);
	rx_reset_deassert(use_dm, use_dp, RIGHT_TS_CTL_D);
	rx_reset_deassert(use_em, use_ep, RIGHT_TS_CTL_E);
	rx_reset_deassert(use_io, 0     , RIGHT_TS_CTL_IO);

	// write TXCSR without txreset
	rcb_write(9, 0xfff, RCB_BT_LANE, 15, 0x50, 
		  txval | get_std_bit16(14));

	if (use_left()) {
		rcb_read(9, 0, RCB_BT_LANE, 0, 0x50);
	}
	if (use_right()) {
		rcb_read(9, 3, RCB_BT_LANE, 0, 0x50);
	}
}
