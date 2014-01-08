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

void train_set (int usem, int usep, int addr) 
{
	if (usem == 1) {
		dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_SEND_TRAIN_M[1]));
		dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_RCV_TRAIN_M[1]));
	}

	if (usep == 1) {
		dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_SEND_TRAIN_P[1]));
		dcr_set_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_RCV_TRAIN_P[1]));
	}
	if (usem+usep > 0) {
		dcr_acc_flush(addr);
	}
}

//#########################################################################
// Testcase
// send some data down the TX path 
void pma_train () 
{
	uint64_t trainmatch_m;
	uint64_t trainmatch_p;
	uint64_t traingoal;
	uint64_t trainstat;
	uint64_t v;

	if (0 != (loopback_sd_pcs + loopback_sd_en)) {
		return;
	}
	
	// FIXME (maybe) use same value for all registers
	trainmatch_m=	quickmask(LEFT_TS_ALIGN_STAT_A_BYTE_ALIGNED_M) |
	  		get_ibm_bit64(LEFT_TS_ALIGN_STAT_A_FOURBYTE_ALIGNED_M[1]);

	trainmatch_p=	quickmask(LEFT_TS_ALIGN_STAT_A_BYTE_ALIGNED_P) |
	  		get_ibm_bit64(LEFT_TS_ALIGN_STAT_A_FOURBYTE_ALIGNED_P[1]);

	traingoal = get_goal_mask();
	trainstat = 0;
	
	while (1) {
		//***********************************************************
		if (use_am+use_ap > 0) {
			v=dcr_rd(LEFT_TS_ALIGN_STAT_A);
			if (trainmatch_m == (trainmatch_m & v)) {
				trainstat |= 1 << 0;
			}
			if (trainmatch_p == (trainmatch_p & v)) {
				trainstat |= 1 << 1;
			}
		}

		//***********************************************************
		if (use_bm+use_bp > 0) {
			v=dcr_rd(LEFT_TS_ALIGN_STAT_B);
			if (trainmatch_m == (trainmatch_m & v)) {
				trainstat |= 1 << 2;
			}
			if (trainmatch_p == (trainmatch_p & v)) {
				trainstat |= 1 << 3;
			}
		}

		//***********************************************************
		if (use_cm+use_cp > 0) {
			v=dcr_rd(LEFT_TS_ALIGN_STAT_C);
			if (trainmatch_m == (trainmatch_m & v)) {
				trainstat |= 1 << 4;
			}
			if (trainmatch_p == (trainmatch_p & v)) {
				trainstat |= 1 << 5;
			}
		}

		//***********************************************************
		if (use_dm+use_dp > 0) {
			v=dcr_rd(RIGHT_TS_ALIGN_STAT_D);
			if (trainmatch_m == (trainmatch_m & v)) {
				trainstat |= 1 << 6;
			}
			if (trainmatch_p == (trainmatch_p & v)) {
				trainstat |= 1 << 7;
			}
		}

		//***********************************************************
		if (use_em+use_ep > 0) {
			v=dcr_rd(RIGHT_TS_ALIGN_STAT_E);
			if (trainmatch_m == (trainmatch_m & v)) {
				trainstat |= 1 << 8;
			}
			if (trainmatch_p == (trainmatch_p & v)) {
				trainstat |= 1 << 9;
			}
		}

		//***********************************************************
		if (use_io > 0) {
			v=dcr_rd(RIGHT_TS_ALIGN_STAT_IO);
			if (trainmatch_m == (trainmatch_m & v)) {
				trainstat |= 1 << 10;
			}
		}

		if (traingoal == (traingoal & trainstat)) {
			break;
		}
	}
}

void training_disable (int usem, int usep, int addr)
{
	if (usem == 1) {
		dcr_clr_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_RCV_TRAIN_M[1]));
		dcr_clr_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_SEND_TRAIN_M[1]));
	}
	if (usep == 1) {
		dcr_clr_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_RCV_TRAIN_P[1]));
		dcr_clr_acc(addr, get_ibm_bit64(LEFT_TS_CTL_A_SEND_TRAIN_P[1]));
	}
	if (usem+usep > 0) {
		dcr_acc_flush(addr);
	}
}
