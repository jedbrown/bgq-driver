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
/******************************************************************************
 * Driver for Rambus Configuration Bus
 *
 * Author: stk@de.ibm.com
 *
 * Changes:
 * 2009-nn-nn created
 * 2009-11-06 split up link broadcasts into individual writes to be 
 *            able to exclude a link (mostly E for mixed init Torus/PCIE)
 *****************************************************************************/

#define _BGQ_SD_RCB_C_

/* Contents:
 * - Data and Types (private)
 * - Helpers (private)
 * - Executers (private) (actual code to write/read..)
 * - API calls (public)
 */

#include <stdio.h>
#include <hwi/include/bqc/serdes_left_dcr.h>
#include <hwi/include/bqc/serdes_right_dcr.h>
#include <firmware/include/fwext/fwext_lib.h>
#include <firmware/include/fwext/sd_helper.h>
#include <firmware/include/fwext/sd_init_auto.h>
#include <firmware/include/fwext/sd_rcb.h>


/*
 ################################## Data and Types ############################
 */
static int rcb_debug_level = 0;

static dcr_val_t dcr_rcconfig_l_shad     = 0;
static dcr_val_t dcr_rcconfig_l_shad_act = 0;
static dcr_val_t dcr_rcconfig_r_shad     = 0;
static dcr_val_t dcr_rcconfig_r_shad_act = 0;

static unsigned exclude_link = 99;
static unsigned suppress_bcast_left  = 0;
static unsigned suppress_bcast_right = 0;

struct rc_init_addr_t {
	dcr_addr_t combine;
	dcr_addr_t left;
	dcr_addr_t right;
};

/*
 ################################### HELPERs ##################################
 */
#define rcb_debug(fmt) \
	if (1 == rcb_debug_level) printf("RCB-DEBUG: "fmt"\n");
#define rcb_debug_1(fmt, a0) \
	if (1 == rcb_debug_level) printf("RCB-DEBUG: "fmt"\n", a0);
#define rcb_debug_2(fmt, a0, a1) \
	if (1 == rcb_debug_level) printf("RCB-DEBUG: "fmt"\n", a0, a1);
#define rcb_debug_5(fmt, a0, a1, a2, a3, a4) \
	if (1 == rcb_debug_level) printf("RCB-DEBUG: "fmt"\n", a0, a1, a2, a3, a4);
#define rcb_debug_6(fmt, a0, a1, a2, a3, a4, a5) \
	if (1 == rcb_debug_level) printf("RCB-DEBUG: "fmt"\n", a0, a1, a2, a3, a4, a5);

/*-----------------------------------
 * rcb_address
 *-----------------------------------*/
static dcr_val_t rcb_address(int fam, int link, int type, int lane, int reg)
{
	dcr_val_t address;

	if (fam != 9)     Terminate(-21);
	if (link > 0xfff) Terminate(-22);
	if (type > 0xf)   Terminate(-23);
	if (lane > 0xf)   Terminate(-24);
	if (reg > 0xff)   Terminate(-25);

	address  = fam  << 28;
	address |= link << 16;
	address |= type << 12;
	address |= lane << 8;
	address |= reg;

	return address;
}

/*-----------------------------------
 * rcb_merge_link_num
 *-----------------------------------*/
static dcr_val_t rcb_merge_link_num(int link, dcr_val_t val)
{
	if (link != 99) {
		return (val & ~0x0fff0000) | ((0xfff & link) << 16);
	}
	return val;
}

/*-----------------------------------
 * rcb_set_config_left
 *-----------------------------------*/
static void rcb_set_config_left(int link, dcr_val_t val)
{
	int config_update = 1;
	dcr_val_t addr;

	addr = rcb_merge_link_num(link, val);

	if (1 == dcr_rcconfig_l_shad_act) {
		if (dcr_rcconfig_l_shad == addr) {
			config_update = 0;
			rcb_debug("skip set config left (cached)");
		}
	}

	if (1 == config_update) {
		dcr_wr(SERDES_LEFT_DCR(RCBUS_CONFIG), addr);
		dcr_rcconfig_l_shad     = addr;
		dcr_rcconfig_l_shad_act = 1;
	}
}

/*-----------------------------------
 * rcb_set_config_right
 *-----------------------------------*/
static void rcb_set_config_right(int link, dcr_val_t val)
{
	int config_update = 1;
	dcr_val_t addr;

	addr = rcb_merge_link_num(link, val);

	if (1 == dcr_rcconfig_r_shad_act) {
		if (dcr_rcconfig_r_shad == addr) {
			config_update = 0;
			rcb_debug("skip set config right (cached)");
		}
	}

	if (1 == config_update) {
		dcr_wr(SERDES_RIGHT_DCR(RCBUS_CONFIG), addr);
		dcr_rcconfig_r_shad     = addr;
		dcr_rcconfig_r_shad_act = 1;
	}
}

/*
 ################################### Executers ################################
 */

/*-----------------------------------
 * rcb_addr_read
 *-----------------------------------*/
static rcb_val_t rcb_addr_read(rcb_addr_t addr)
{
	dcr_val_t  rcbus_config; 
	dcr_addr_t reg;
	rcb_val_t  rc;
	int        read_left;
	int        read_right;

	/* extract register and target link from rc address */
	/* prepare rc config value                          */
	reg           = (dcr_addr_t) (addr & 0xff);
	rcbus_config  = 0xffffff00 & addr;

	if ((addr & 0x00ff0000) < 0x00030000) {
		read_left  = 1;
		read_right = 0;
	} else {
		read_left  = 0;
		read_right = 1;
	}

	/* now process the read */
	if (read_left) {
		rcb_set_config_left(99, rcbus_config);
		rc = sd_dcr_rd(reg | SERDES_LEFT_DCR(RCBUS));
	}

	if (read_right) {
		rcb_set_config_right(99, rcbus_config);
		rc = sd_dcr_rd(reg | SERDES_RIGHT_DCR(RCBUS));
	}
	return rc;
}

/*-----------------------------------
 * rcb_addr_write
 *-----------------------------------*/
void rcb_addr_write(rcb_addr_t addr, rcb_val_t value)
{
	int        link[6]     = {99, 99, 99, 99, 99, 99}; //links to use
	dcr_val_t  rcbus_config; 
	dcr_addr_t reg;
	int        bcast_left  = 0;
	int        bcast_right = 0;
	int        ucast_left  = 0;
	int        ucast_right = 0;
	int        target_link;
	int	   i;

	/* extract register and target link from rc address */
	/* prepare rc config value                          */
	reg           = (dcr_addr_t) (addr & 0xff);
	target_link   = (addr & 0x0fff0000) >> 16;
	rcbus_config  = 0xffffff00 & addr;
	rcb_debug_2("addr_write: addr=%xh target_link=%xh", addr, target_link);

	/* determine and setup target links */
	if (target_link == 0xfff) {
		/* -----------Broadcast write----------- */
		rcb_debug("addr_write: broadcast");

		/* 
		 * check left side, if bcast suppression then init
		 * the links manually, links may be deleted later
		 * based on exclude_link
		 */
		if (0 == suppress_bcast_left)  {
			bcast_left = 1;
		} else {
			rcb_debug("addr_write: suppress_bcast_left");
			ucast_left = 1; //switch to ucast writes
			link[0]    = 1;
			link[1]    = 1;
			link[2]    = 1;
		}

		/* check right side */
		if (0 == suppress_bcast_right) {
			bcast_right = 1;
		} else {
			rcb_debug("addr_write: suppress_bcast_right");
			ucast_right = 1; //switch to ucast writes
			link[3]     = 1;
			link[4]     = 1;
			link[5]     = 1;
		}

		/* now check exclude_link, gets set in any case
		 * only relevant if any suppressed links
		 */
		if (exclude_link < 6) {
			rcb_debug_1("addr_write: excluded link=%d", exclude_link);
			link[exclude_link] = 99;
		}

	} else if (target_link < 6) {
		/* -----------Unicast write----------- */
		if ((addr & 0x00ff0000) < 0x00030000) {
			rcb_debug("addr_write: unicast left");
			ucast_left  = 1;
		} else {
			rcb_debug("addr_write: unicast right");
			ucast_right = 1;
		}
		link[target_link] = 1;
	} else {
		printf("RCB Bug\n");
		Terminate(-1);
	}

	/* now process the writes */
	if (bcast_left) {
		rcb_set_config_left(99, rcbus_config);
		sd_dcr_wr(reg | SERDES_LEFT_DCR(RCBUS), value);
	}

	if (bcast_right) {
		rcb_set_config_right(99, rcbus_config);
		sd_dcr_wr(reg | SERDES_RIGHT_DCR(RCBUS), value);
	}

	/* mask out rcbus link index value, because if broadcast is 
	 * suppressed the write happens through ucast_left, need to
	 * replace link id in this case
	 */
	rcbus_config = rcbus_config & ~0x0fff0000;

	if (ucast_left) {
		for (i = 0; i < 3; i++) {
			if (1 == link[i]) {
				rcb_set_config_left(i, rcbus_config);
				sd_dcr_wr(reg | SERDES_LEFT_DCR(RCBUS), value);
			}
		}
	}

	if (ucast_right) {
		for (i = 3; i < 6; i++) {
			if (1 == link[i]) {
				rcb_set_config_right(i, rcbus_config);
				sd_dcr_wr(reg | SERDES_RIGHT_DCR(RCBUS), value);
			}
		}
	}
}

/*
 ##############################################################################
 ################################### RCB API Calls ############################
 ##############################################################################
 */

/*-----------------------------------
 * rcb_set_debug_level 0, 1
 *-----------------------------------*/
void rcb_set_debug_level(int level)
{
	if (level > 0) {
		rcb_debug_level = 1;
	} else {
		rcb_debug_level = 0;
	}
}

/*-----------------------------------
 * rcb_set_excluded_link
 *-----------------------------------*/
void rcb_set_excluded_link(int link)
{
	exclude_link = link;

	if (link > 2) {
		suppress_bcast_left  = 0;
		suppress_bcast_right = 1;
	} else {
		suppress_bcast_left  = 1;
		suppress_bcast_right = 0;
	}
	rcb_debug_1("exclude link %d", link);
}

/*-----------------------------------
 * rcb_clear_excluded_link
 *-----------------------------------*/
void rcb_clear_excluded_link(unsigned int link)
{
	/* only support one excluded link */
	exclude_link = 99;
	suppress_bcast_left  = 0;
	suppress_bcast_right = 0;
	return;
}

/*-----------------------------------
 * rcb_read
 *-----------------------------------*/
rcb_val_t rcb_read(int fam, int link, int type, int lane, int reg)
{
	rcb_addr_t rca;
	rcb_val_t  rc;

	rca = rcb_address(fam, link, type, lane, reg);
	rc  = rcb_addr_read(rca);

	rcb_debug_6("[44m[37m[1mread from fam=%d link=%xh type=%d lane=%d reg=%xh val=%xh[K[0m",
			fam, link, type, lane, reg, rc);

	return rc;
}

/*-----------------------------------
 * rcb_write
 *-----------------------------------*/
void rcb_write(int fam, int link, int type, int lane, int reg, rcb_val_t value)
{
	rcb_addr_t rca;

	rcb_debug_6("[44m[37m[1mwrite to fam =%d link=%xh type=%d lane=%d reg=%xh val=%xh[K[0m",
			fam, link, type, lane, reg, value);

	rca =  rcb_address(fam, link, type, lane, reg);
	rcb_addr_write(rca, value);
}
