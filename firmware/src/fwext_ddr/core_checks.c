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

int pma_debug=1;
int rcb_debug=1;

// Init Sequence
//

//#########################################################################
// Show PLL Lock status
void pma_wait_lock_pll (int mode)
{
	int lock_e;

	if (0 != loopback_sd_en) {
		return;
	}

	if (1 == mode) {
		lock_e = 1;
	} else {
		lock_e = use_em+use_ep;
	}

	uint64_t pll_left_goal  = ((0 != (use_am+use_ap)) ? get_ibm_bit64(LEFT_TS_HSS_PLL_STAT_A_PLLA_LOCKED[1]) : 0) |
	                          ((0 != (use_bm+use_bp)) ? get_ibm_bit64(LEFT_TS_HSS_PLL_STAT_B_PLLA_LOCKED[1]) : 0) |
	                          ((0 != (use_cm+use_cp)) ? get_ibm_bit64(LEFT_TS_HSS_PLL_STAT_C_PLLA_LOCKED[1]) : 0) ;


	uint64_t pll_right_goal = ((0 != (use_dm+use_dp)) ? get_ibm_bit64(RIGHT_TS_HSS_PLL_STAT_D_PLLA_LOCKED[1]) : 0) |
	                          ((0 !=  lock_e        ) ? get_ibm_bit64(RIGHT_TS_HSS_PLL_STAT_E_PLLB_LOCKED[1]) : 0) |
	                          ((0 !=  use_io        ) ? get_ibm_bit64(RIGHT_TS_HSS_PLL_STAT_IO_PLLA_LOCKED[1]): 0) ;

	uint64_t pll_left_stat  = 0;
	uint64_t pll_right_stat = 0;

	while (1) {
		if (use_left()) {
			pll_left_stat = pll_left_goal & dcr_rd(LEFT_TS_HSS_PLL_STAT);
		}

		if (use_right()) {
			pll_right_stat = pll_right_goal & dcr_rd(RIGHT_TS_HSS_PLL_STAT);
		}
		if ( (pll_left_goal  == pll_left_stat) && 
		     (pll_right_goal == pll_right_stat)) {
			break;
		}
	}
	return;
}

//#########################################################################
// Show Lane ready of HSS01
//
void pma_wait_lanes ()
{
	if (0 != (loopback_sd_en + loopback_pcs_en)) {
		return;
	}

	uint64_t lane_left_goal  = ((0 != use_am) ? quickmask(LEFT_TS_HSS_LANE_STAT_LANE_READY_A_M) : 0) |
	                           ((0 != use_ap) ? quickmask(LEFT_TS_HSS_LANE_STAT_LANE_READY_A_P) : 0) |
	                           ((0 != use_bm) ? quickmask(LEFT_TS_HSS_LANE_STAT_LANE_READY_B_M) : 0) |
	                           ((0 != use_bp) ? quickmask(LEFT_TS_HSS_LANE_STAT_LANE_READY_B_P) : 0) |
	                           ((0 != use_cm) ? quickmask(LEFT_TS_HSS_LANE_STAT_LANE_READY_C_M) : 0) |
	                           ((0 != use_cp) ? quickmask(LEFT_TS_HSS_LANE_STAT_LANE_READY_C_P) : 0) ;

	uint64_t lane_right_goal = ((0 != use_dm) ? quickmask(RIGHT_TS_HSS_LANE_STAT_LANE_READY_D_M) : 0) |
	                           ((0 != use_dp) ? quickmask(RIGHT_TS_HSS_LANE_STAT_LANE_READY_D_P) : 0) |
	                           ((0 != use_em) ? quickmask(RIGHT_TS_HSS_LANE_STAT_LANE_READY_E_M) : 0) |
	                           ((0 != use_ep) ? quickmask(RIGHT_TS_HSS_LANE_STAT_LANE_READY_E_P) : 0) |
	                           ((0 != use_io) ? quickmask(RIGHT_TS_HSS_LANE_STAT_LANE_READY_IO): 0) ;

	uint64_t lane_left_stat  = 0;
	uint64_t lane_right_stat = 0;
	uint64_t left            = use_ap + use_am + use_bp + use_bm + use_cp + use_cm;
	uint64_t right           = use_dp + use_dm + use_ep + use_em + use_io;

	while (1) {
		if (0 != left) {
			lane_left_stat = lane_left_goal & dcr_rd(LEFT_TS_HSS_LANE_STAT);
		}

		if (0 != right) {
			lane_right_stat = lane_right_goal & dcr_rd(RIGHT_TS_HSS_LANE_STAT);
		}

		if ( (lane_left_goal  == lane_left_stat) && 
		     (lane_right_goal == lane_right_stat)) {
			break;
		}
	}
}

