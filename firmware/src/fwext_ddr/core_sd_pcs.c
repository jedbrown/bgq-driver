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

//#########################################################################
// This wakes up the PCS for HSS01
//
void pma_reset_pcs_rcb () // Changed to match perl version (10/19/2010)
{
	if (0 == (loopback_sd_en)) { 
		//PCS spec page 68 ,do rc resets on left and right
		if (use_left())  {
			dcr_clr(LEFT_RCBUS_CONFIG, get_ibm_bit64(LEFT_RCBUS_CONFIG_RESET[1]));
		}
		if (use_right()) {
			dcr_clr(RIGHT_RCBUS_CONFIG, get_ibm_bit64(RIGHT_RCBUS_CONFIG_RESET[1]));
		}
	}
	
	//check if torus E is used, if so then place in Torums mode
	if ((0 != (use_em + use_ep)) && (en_pcie == 0)) {  /*constant condition*/
		dcr_set(RIGHT_SD_CONFIG, get_ibm_bit64(RIGHT_SD_CONFIG_PCIE_DISABLE[1]));
	}
}

void sd_eb_forcestop (int usem, int usep, int addr) // Changed to match perl version (10/19/2010)
{
	if (usem == 1) {
		dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_RX_ELASTIC_BUF_RUN_OVR_M[1]));
		dcr_clr_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_RX_ELASTIC_BUF_RUN_M[1]));
	}

	if (usep == 1) {
		dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_RX_ELASTIC_BUF_RUN_OVR_P[1]));
		dcr_clr_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_RX_ELASTIC_BUF_RUN_P[1]));
	}

	if (usem+usep > 0) {
		dcr_acc_flush(addr);
	}
}

void sd_init_dataloop (int usem, int usep, int addr) // Changed to match perl version (10/19/2010)
{
	if (usem == 1) {
		dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_DATALOOP_EN_M[1]));
	}
	if (usep == 1) {
		dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_DATALOOP_EN_P[1]));
	}
	if (usem+usep > 0) {
		dcr_acc_flush(addr);
	}
}

void sd_forcestop_elastic_buffers () // Changed to match perl version (10/19/2010)
{
	sd_eb_forcestop(use_am, use_ap, LEFT_TS_CTL_A);
	sd_eb_forcestop(use_bm, use_bp, LEFT_TS_CTL_B);
	sd_eb_forcestop(use_cm, use_cp, LEFT_TS_CTL_C);
	sd_eb_forcestop(use_dm, use_dp, RIGHT_TS_CTL_D);
	sd_eb_forcestop(use_em, use_ep, RIGHT_TS_CTL_E);
	sd_eb_forcestop(use_io, 0     , RIGHT_TS_CTL_IO);
}

void sd_eb_release (int usem, int usep, int addr) // Changed to match perl version (10/19/2010)
{
	if (usem == 1) {
		dcr_clr_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_RX_ELASTIC_BUF_RUN_OVR_M[1]));
	}

	if (usep == 1) {
		dcr_clr_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_RX_ELASTIC_BUF_RUN_OVR_P[1]));
	}

	if (usem+usep > 0) {
		dcr_acc_flush(addr);
	}
}

void sd_release_elastic_buffers () // Changed to match perl version (10/19/2010)
{
	sd_eb_release(use_am, use_ap, LEFT_TS_CTL_A);
	sd_eb_release(use_bm, use_bp, LEFT_TS_CTL_B);
	sd_eb_release(use_cm, use_cp, LEFT_TS_CTL_C);
	sd_eb_release(use_dm, use_dp, RIGHT_TS_CTL_D);
	sd_eb_release(use_em, use_ep, RIGHT_TS_CTL_E);
	sd_eb_release(use_io, 0     , RIGHT_TS_CTL_IO);
}

void sd_init_0 (int usem, int usep, int addr) // Changed to match perl version (10/19/2010)
{
	uint64_t common=0;

	// common settings
	if (usem+usep > 0) {
		if (loopback_sd_en == 1) {
			common |= get_ibm_bit64(LEFT_TS_CTL_A_LOOPBACK_EN[1]);
		}
		if (loopback_pma_en == 1) {
			common |= get_ibm_bit64(LEFT_TS_CTL_A_DATALOOP_EN_M[1]);
			common |= get_ibm_bit64(LEFT_TS_CTL_A_DATALOOP_EN_P[1]);
		}
	}

	if (usem == 1) {
		dcr_set_acc(addr, common);

		if (scramble_enable == 1) {
			dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_XMIT_SCRAMBLE_M[1]));
			dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_RCV_DESCRAMBLE_M[1]));
		}

		dcr_set_acc(addr, quickval(latency_m, LEFT_TS_CTL_A_RX_ELASTIC_BUF_LAT_M));
		dcr_set_acc(addr, quickval(latency_m, LEFT_TS_CTL_A_TX_ELASTIC_BUF_LAT_M));
		dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_TX_ELASTIC_BUF_RUN_M[1]));
	}

	if (usep == 1) {
		dcr_set_acc(addr, common);

		if (scramble_enable == 1) {
			dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_XMIT_SCRAMBLE_P[1]));
			dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_RCV_DESCRAMBLE_P[1]));
		}

		dcr_set_acc(addr, quickval(latency_p, LEFT_TS_CTL_A_RX_ELASTIC_BUF_LAT_P));
		dcr_set_acc(addr, quickval(latency_p, LEFT_TS_CTL_A_TX_ELASTIC_BUF_LAT_P));
		dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_TX_ELASTIC_BUF_RUN_P[1]));
	}

	if (usem+usep > 0) {
		dcr_acc_flush(addr);
	}
}

void sd_init_1 (int usem, int usep, int addr) // Changed to match perl version (10/19/2010)
{
	if (loopback_sd_en == 1) {
		return;
	}

	if (usem == 1) {
		dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_SEND_TRAIN_M[1]));
	}

	if (usep == 1) {
		dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_SEND_TRAIN_P[1]));
	}
	if (usem+usep > 0) {
		dcr_acc_flush(addr);
	}
}

void sd_init_2 (int usem, int usep, int addr) // Changed to match perl version (10/19/2010)
{
	if (loopback_sd_en == 1) {
		return;
	}

	if (usem == 1) {
		dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_RCV_TRAIN_M[1]));
	}

	if (usep == 1) {
		dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_RCV_TRAIN_P[1]));
	}

	if (usem+usep > 0) {
		dcr_acc_flush(addr);
	}
}

void check_dataloop() // Changed to match perl version (10/19/2010)
{
	if (loopback_pma_en != 1) {
		return;
	}
	
	sd_init_dataloop(use_am, use_ap, LEFT_TS_CTL_A);
	sd_init_dataloop(use_bm, use_bp, LEFT_TS_CTL_B);
	sd_init_dataloop(use_cm, use_cp, LEFT_TS_CTL_C);
	sd_init_dataloop(use_dm, use_dp, RIGHT_TS_CTL_D);
	sd_init_dataloop(use_em, use_ep, RIGHT_TS_CTL_E);
	sd_init_dataloop(use_io, 0     , RIGHT_TS_CTL_IO);
}

//#########################################################################
// This wakes up the sync buffers (elastic buffers) of HSS01
//
void sd_top_init () // Changed to match perl version (10/19/2010)
{
	sd_init_0(use_am, use_ap, LEFT_TS_CTL_A);
	sd_init_0(use_bm, use_bp, LEFT_TS_CTL_B);
	sd_init_0(use_cm, use_cp, LEFT_TS_CTL_C);
	sd_init_0(use_dm, use_dp, RIGHT_TS_CTL_D);
	sd_init_0(use_em, use_ep, RIGHT_TS_CTL_E);
	sd_init_0(use_io, 0     , RIGHT_TS_CTL_IO);

	sd_init_1(use_am, use_ap, LEFT_TS_CTL_A);
	sd_init_1(use_bm, use_bp, LEFT_TS_CTL_B);
	sd_init_1(use_cm, use_cp, LEFT_TS_CTL_C);
	sd_init_1(use_dm, use_dp, RIGHT_TS_CTL_D);
	sd_init_1(use_em, use_ep, RIGHT_TS_CTL_E);
	sd_init_1(use_io, 0     , RIGHT_TS_CTL_IO);
}

void sd_bottom_init () // Changed to match perl version (10/19/2010)
{
	sd_init_2(use_am, use_ap, LEFT_TS_CTL_A);
	sd_init_2(use_bm, use_bp, LEFT_TS_CTL_B);
	sd_init_2(use_cm, use_cp, LEFT_TS_CTL_C);
	sd_init_2(use_dm, use_dp, RIGHT_TS_CTL_D);
	sd_init_2(use_em, use_ep, RIGHT_TS_CTL_E);
	sd_init_2(use_io, 0     , RIGHT_TS_CTL_IO);
}

void rx_reset_assert (int usem, int usep, int addr) // Changed to match perl version (10/19/2010)
{
	if (usem+usep > 0) {
		if (usem == 1) {
			dcr_set_acc(addr, quickmask(LEFT_TS_CTL_A_RX_RESET_M));
		}
		if (usep == 1) {
			dcr_set_acc(addr, quickmask(LEFT_TS_CTL_A_RX_RESET_P));
		}
		dcr_acc_flush(addr);
	}
}

void rx_reset_deassert (int usem, int usep, int addr) // Changed to match perl version (10/19/2010)
{
	if (usem+usep > 0) {
		if (usem == 1) {
			dcr_clr_acc(addr, quickmask(LEFT_TS_CTL_A_RX_RESET_M));
		}
		if (usep == 1) {
			dcr_clr_acc(addr, quickmask(LEFT_TS_CTL_A_RX_RESET_P));
		}
		dcr_acc_flush(addr);
	}
}

void pcs_reset_assert (int usem, int usep, int addr) // Changed to match perl version (10/19/2010)
{
	if (usem+usep > 0) {
		dcr_set(addr, get_ibm_bit64(LEFT_TS_CTL_A_PCS_RESET[1]));
	}
}

void pcs_reset_deassert (int usem, int usep, int addr) // Changed to match perl version (10/19/2010)
{
	if (usem+usep > 0) {
		dcr_clr_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_PCS_RESET[1]));
		// flush happens in rx rst deassert
	}
}

void pipe0_reset_deassert (int usem, int usep, int addr) // Changed to match perl version (10/19/2010)
{
	if (usem+usep > 0) {
		dcr_set(addr, get_ibm_bit64(LEFT_TS_CTL_A_PIPE_RESETN[1]));
	}
}

void pcs_reset () // Changed to match perl version (10/19/2010)
{
	//de-assert rx reset and pcs reset at the same time
	//NOTE: flush happens in rx_reset_deassert
	pcs_reset_deassert(use_am, use_ap, LEFT_TS_CTL_A);
	pcs_reset_deassert(use_bm, use_bp, LEFT_TS_CTL_B);
	pcs_reset_deassert(use_cm, use_cp, LEFT_TS_CTL_C);
	pcs_reset_deassert(use_dm, use_dp, RIGHT_TS_CTL_D);
	pcs_reset_deassert(use_em, use_ep, RIGHT_TS_CTL_E);
	pcs_reset_deassert(use_io, 0      , RIGHT_TS_CTL_IO);
	//no acc flush here

	pipe0_reset_deassert(use_am, use_ap, LEFT_TS_CTL_A);
	pipe0_reset_deassert(use_bm, use_bp, LEFT_TS_CTL_B);
	pipe0_reset_deassert(use_cm, use_cp, LEFT_TS_CTL_C);
	pipe0_reset_deassert(use_dm, use_dp, RIGHT_TS_CTL_D);
	pipe0_reset_deassert(use_em, use_ep, RIGHT_TS_CTL_E);
	pipe0_reset_deassert(use_io, 0      , RIGHT_TS_CTL_IO);
}

