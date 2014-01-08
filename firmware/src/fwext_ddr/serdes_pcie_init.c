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

#define _BGQ_SERDES_PCIE_INIT_C_
 
#include <firmware/include/fwext/fwext.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <firmware/include/Firmware.h>
#include <hwi/include/bqc/serdes_left_dcr.h>
#include <hwi/include/bqc/serdes_right_dcr.h>
#include <hwi/include/bqc/dc_arbiter_dcr.h>
#include <firmware/include/fwext/sd_helper.h>
#include <firmware/include/fwext/sd_rcb.h>
#include <firmware/include/fwext/serdes.h>

void hss_pcie_init(Personality_t *p)
{
	dcr_val_t dval;

	/* De-Assert PCS_RESET */
	dval = sd_dcr_rd(SERDES_RIGHT_DCR(SD_CONFIG));
	dval = dval & ~(SERDES_RIGHT_DCR__SD_CONFIG__PCIE_DISABLE_set(1));
	sd_dcr_wr(SERDES_RIGHT_DCR(SD_CONFIG), dval);

	/* Save TS_CTL_E */
	dval = sd_dcr_rd(SERDES_RIGHT_DCR(TS_CTL_E));

	/* assert AREFEN */
	dval = dval | SERDES_RIGHT_DCR__TS_CTL_E__HSS_AREFEN_set(1);
	sd_dcr_wr(SERDES_RIGHT_DCR(TS_CTL_E), dval );
	dval = sd_dcr_rd(SERDES_RIGHT_DCR(TS_CTL_E));

	/* assert SBRESET */
	rcb_write(9, 4, RCB_BT_SUPPORT, 0, 0x1c,
		sd_get_std_bit16(0) | // override
		sd_get_std_bit16(6)); // sbreset

	/* de-assert SBRESET */
	rcb_write(9, 4, RCB_BT_SUPPORT, 0, 0x1c, 0);

	/* De-Assert PCS_RESET */
	dval = dval & ~(SERDES_RIGHT_DCR__TS_CTL_E__PCS_RESET_set(1));
	sd_dcr_wr(SERDES_RIGHT_DCR(TS_CTL_E), dval);

	/* De-Assert RESET_N */
	dval = dval | SERDES_RIGHT_DCR__TS_CTL_E__PIPE_RESETN_set(1);
	sd_dcr_wr(SERDES_RIGHT_DCR(TS_CTL_E), dval );

	/* Wait for PLL up (0.3ms) */
	_Delay_dbnz(1600000000ULL/400ULL); //practically wait 2.5ms
	dval = sd_dcr_rd(SERDES_RIGHT_DCR(TS_HSS_PLL_STAT));
	if (0 == (dval & SERDES_RIGHT_DCR__TS_HSS_PLL_STAT__E_PLLB_LOCKED_set(1))) {
		printf("PCIE PLL did not lock\n");
	}
}

void serdes_pcie_init(Personality_t *p)
{
	dcr_val_t dval;

	//first set timeout to max for rcbus
	dval  = sd_dcr_rd(DC_ARBITER_DCR(ARB_CTRL));
	dval |= DC_ARBITER_DCR__ARB_CTRL__TIMER_VAL_set(0xf);
	sd_dcr_wr(DC_ARBITER_DCR(ARB_CTRL), dval);

	// Enable all HSS cores
	sd_dcr_wr(TESTINT_DCR(CONFIG1)+1,
		sd_get_ibm_bit64(43) |
		sd_get_ibm_bit64(44) |
		sd_get_ibm_bit64(45) |
		sd_get_ibm_bit64(46) |
		sd_get_ibm_bit64(47) |
		sd_get_ibm_bit64(48));

	hss_pcie_init(p);
}


