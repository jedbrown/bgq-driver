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
#include "core_dcr.h"

// Testcase t1 removed

int error_inject_dataphase=0;

volatile uint64_t cp;

//#########################################################################
// Testcase
// wake up whole HSS01 path for data test
void pma_t2 ()
{
	cp = 0xcafebabe00000ede;
	pma_reset_pcs_rcb();
	cp = 0xcafebabe00001ede;
	pma_wake_pll();
	cp = 0xcafebabe00002ede;
	pma_wait_lock_pll();
	cp = 0xcafebabe00003ede;
	pma_enable_pclk();
	cp = 0xcafebabe00004ede;
	pma_wake_lanes();
	cp = 0xcafebabe00005ede;
	sd_top_init();
	cp = 0xcafebabe00006ede;
	pma_wait_lanes();
	cp = 0xcafebabe00007ede;
	pcs_reset();
	cp = 0xcafebabe00008ede;
}

void pma_t4 ()
{
	cp = 0xcafebabe00009ede;
	pma_t2();
	cp = 0xcafebabe0000aede;
	pma_train();
	cp = 0xcafebabe0000bede;
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

void pma_t5 ()
{
	cp = 0xcafebabe0000cede;
	pma_t4();
	cp = 0xcafebabe0000dede;

	if (0 == loopback_sd_en) {
		training_disable(use_am, use_ap, LEFT_TS_CTL_A);
		training_disable(use_bm, use_bp, LEFT_TS_CTL_B);
		training_disable(use_cm, use_cp, LEFT_TS_CTL_C);
		training_disable(use_dm, use_dp, RIGHT_TS_CTL_D);
		training_disable(use_em, use_ep, RIGHT_TS_CTL_E);
		training_disable(use_io, 0     , RIGHT_TS_CTL_IO);
	} else {
		// nothing
	}

	if (prbs_en == 1) {
		//prbs_enable();
		//prbs_monitor();
	} else {
		//datatransfer();
	}
	cp = 0xcafebabeeeeeeede;
}
