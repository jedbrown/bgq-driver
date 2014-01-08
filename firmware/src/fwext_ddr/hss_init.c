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

#define _BGQ_HSS_INIT_C_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <firmware/include/Firmware.h>
#include <firmware/include/fwext/fwext.h>
#include <firmware/include/fwext/sd_helper.h>
#include <firmware/include/fwext/serdes.h>
#include <firmware/include/fwext/sd_init_auto.h>
#include <firmware/include/fwext/core_dcr.h>
#include <firmware/include/fwext/sd_rcb.h>

#include <hwi/include/bqc/serdes_left_dcr.h>
#include <hwi/include/bqc/serdes_right_dcr.h>
#include <hwi/include/bqc/testint_dcr.h>
#include <hwi/include/bqc/dc_arbiter_dcr.h>
#include <hwi/include/bqc/nd_500_dcr.h>


static int hss_debug = 0;
static int hss_sim = 0;

char versionhint[] = "vhint-4";

#define hss_debug(fmt) \
	if (1 == hss_debug) printf("HSS-DEBUG: [45m"fmt"[K[0m\n");
#define hss_debug_plain(fmt) \
	if (1 == hss_debug) printf("HSS-DEBUG: "fmt"\n");

void
hss_set_sim_mode(int level)
{
	if (level > 0) {
		hss_sim = 1;
	} else {
		hss_sim = 0;
	}
}

void
hss_set_debug_level(int level)
{
	if (level > 0) {
		hss_debug = 1;
	} else {
		hss_debug = 0;
	}
	rcb_set_debug_level(level);
}

void hss_enable_all_links(void)
{
	use_a = use_b = use_c = use_d = use_e = use_io = 1;
	use_am = use_ap = 1;
	use_bm = use_bp = 1;
	use_cm = use_cp = 1;
	use_dm = use_dp = 1;
	use_em = use_ep = 1;
}

/* hss_setup
 * =========
 * 0 - init all links in torus mode
 * 1 - init all links except E in torus mode, init PCIE mode on E
 *
 * on mode 1 torus E gets de-configured
 *
 */
void
hss_setup(int mode)
{
	uint64_t dval = 0;

	// Enable all HSS cores
	DCRWritePriv(TESTINT_DCR(CONFIG1) + 1,
		     get_ibm_bit64(43) |
		     get_ibm_bit64(44) |
		     get_ibm_bit64(45) |
		     get_ibm_bit64(46) | 
		     get_ibm_bit64(47) | 
		     get_ibm_bit64(48));

	dval = sd_dcr_rd(DC_ARBITER_DCR(ARB_CTRL));
	dval |= DC_ARBITER_DCR__ARB_CTRL__TIMER_VAL_set(0xF);
	sd_dcr_wr(DC_ARBITER_DCR(ARB_CTRL), dval);
	sd_dcr_rd(DC_ARBITER_DCR(ARB_CTRL));

	if (1 == mode) {
		use_em = 0;
		use_ep = 0;
		hss_debug("serdes_pcie_init ");
		hss_pcie_init(NULL);

		//This blocks Link E RCB access
		//A-0 B-1 C-2 D-3 E-4 IO-5
		rcb_set_excluded_link(4);
	}

	hss_debug("ASSERT_RESETS");
	assert_resets(); // 1

	hss_debug("PMA_RESET_PCS_RCB");
	pma_reset_pcs_rcb(); // 2

	hss_debug("SD_FORCESTOP_ELASTIC_BUFFERS");
	sd_forcestop_elastic_buffers(); // 3

	hss_debug("PMA_WAKE_PLL");
	pma_wake_pll(); // 4

	hss_debug("PMA_WAIT_LOCK_PLL");
	if (0 == hss_sim) {
		pma_wait_lock_pll(); // 5
	} else {
		hss_debug_plain("not executed in sim mode");
	}

	hss_debug("PMA_ENABLE_PCLK");
	pma_enable_pclk(); // 6

	hss_debug("PMA_WAKE_LANES");
	pma_wake_lanes(); // 7

	hss_debug("PMA_WAIT_LANES");
	if (0 == hss_sim) {
		pma_wait_lanes(); // 8
	} else {
		hss_debug_plain("not executed in sim mode");
	}

	hss_debug("SD_TOP_INIT");
	sd_top_init(); // 9

	hss_debug("SD_RELEASE_ELASTIC_BUFFERS");
	sd_release_elastic_buffers(); // 10

	hss_debug("LANES_RXTXRST_DEASSERT");
	lanes_rxtxrst_deassert(); // 11

	hss_debug("PCS_RESET");
	pcs_reset(); // 12

	hss_debug("SD_BOTTOM_INIT");
	sd_bottom_init(); // 13

	hss_debug("PMA_TRAIN");
	if (0 == hss_sim) {
		pma_train();
	} else {
		hss_debug_plain("not executed in sim mode");
	}

	hss_debug("TRAINING DONE");
	training_disable(use_am, use_ap, LEFT_TS_CTL_A);
	training_disable(use_bm, use_bp, LEFT_TS_CTL_B);
	training_disable(use_cm, use_cp, LEFT_TS_CTL_C);
	training_disable(use_dm, use_dp, RIGHT_TS_CTL_D);
	training_disable(use_em, use_ep, RIGHT_TS_CTL_E);
	training_disable(use_io, 0, RIGHT_TS_CTL_IO);
}

void
hss_init()
{
	hss_setup(0);		//0-no pcie, 1-with pcie
}

uint32_t nd_get_loopback_mode() {
  // if any link is in loopback mode then this will return 1, 0 otherwise.
  uint64_t val = sd_dcr_rd(ND_500_DCR(CTRL_INTERNAL));
  return (uint32_t)(ND_500_DCR__CTRL_INTERNAL__LOOPBACK_get(val) > 0);
}

uint32_t sd_get_loopback_mode() {
  // if any link is in loopback mode then this will return 1, 0 otherwise.
  uint32_t loopback = 0;
  
  loopback |= SERDES_LEFT_DCR__TS_CTL_A__LOOPBACK_EN_get(sd_dcr_rd(SERDES_LEFT_DCR(TS_CTL_A))) << 0;
  loopback |= SERDES_LEFT_DCR__TS_CTL_B__LOOPBACK_EN_get(sd_dcr_rd(SERDES_LEFT_DCR(TS_CTL_B))) << 1;
  loopback |= SERDES_LEFT_DCR__TS_CTL_C__LOOPBACK_EN_get(sd_dcr_rd(SERDES_LEFT_DCR(TS_CTL_C))) << 2;

  loopback |= SERDES_RIGHT_DCR__TS_CTL_D__LOOPBACK_EN_get(sd_dcr_rd(SERDES_RIGHT_DCR(TS_CTL_D))) << 3;
  loopback |= SERDES_RIGHT_DCR__TS_CTL_E__LOOPBACK_EN_get(sd_dcr_rd(SERDES_RIGHT_DCR(TS_CTL_E))) << 4;
  loopback |= SERDES_RIGHT_DCR__TS_CTL_IO__LOOPBACK_EN_get(sd_dcr_rd(SERDES_RIGHT_DCR(TS_CTL_IO))) << 5;

  return loopback;
}

