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


#include <firmware/include/Firmware.h>
#include <hwi/include/bqc/serdes_left_dcr.h>
#include <hwi/include/bqc/serdes_right_dcr.h>
#include <hwi/include/bqc/gea_dcr.h>
#include <hwi/include/bqc/testint_dcr.h>
#include <hwi/include/bqc/dc_arbiter_dcr.h>

#include "Firmware_internals.h"
#include "Firmware_RAS.h"


int fw_serdes_machineCheckHandler( uint64_t status[] ) {

  fw_uint64_t details[16];
  fw_uint16_t n = 0;

  
  details[n++] = SERDES_LEFT_DCR( SERDES_INTERRUPT_STATE__MACHINE_CHECK );
  details[n++] = SERDES_LEFT_DCR_PRIV_PTR->serdes_interrupt_state__machine_check;

  details[n++] = SERDES_LEFT_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK );
  details[n++] = SERDES_LEFT_DCR_PRIV_PTR->interrupt_internal_error__machine_check;

  details[n++] = SERDES_RIGHT_DCR( SERDES_INTERRUPT_STATE__MACHINE_CHECK );
  details[n++] = SERDES_RIGHT_DCR_PRIV_PTR->serdes_interrupt_state__machine_check;

  details[n++] = SERDES_RIGHT_DCR( INTERRUPT_INTERNAL_ERROR__MACHINE_CHECK );
  details[n++] = SERDES_RIGHT_DCR_PRIV_PTR->interrupt_internal_error__machine_check;

  fw_machineCheckRas( FW_RAS_SERDES_MACHINE_CHECK, details, n, __FILE__, __LINE__ );

  return -1;
}

int fw_serdes_init( void ) {

  TRACE_ENTRY(TRACE_SerDes);

#ifndef FW_PREINSTALLED_GEA_HANDLERS

    uint64_t mask[3] = { SERDES_GEA_MASK_0, SERDES_GEA_MASK_1, SERDES_GEA_MASK_2 };

    fw_installGeaHandler( fw_serdes_machineCheckHandler, mask );

#endif

    /*
     * Workaround:  There may be some latent interrupt bits sitting around due to training.
     *              Clear these out before enabling interrupts.
     */

    DCRWritePriv( SERDES_RIGHT_DCR(SERDES_INTERRUPT_STATE__STATE), -1 );
    DCRWritePriv( SERDES_LEFT_DCR(SERDES_INTERRUPT_STATE__STATE), -1 );
    ppc_msync();


    uint64_t leftControlHigh =
      SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__A_PLLA_LOCK_LOST_set(1) | // Dimension A HSS PLLA lost lock
      SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__B_PLLA_LOCK_LOST_set(1) | // Dimension B HSS PLLA lost lock
      SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__C_PLLA_LOCK_LOST_set(1) | // Dimension C HSS PLLA lost lock
      0;

    DCRWritePriv( SERDES_LEFT_DCR( SERDES_INTERRUPT_STATE_CONTROL_HIGH ), leftControlHigh );

    uint64_t rightControlHigh =
      SERDES_RIGHT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__LOCAL_RING_set(1) |
      SERDES_RIGHT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__D_PLLA_LOCK_LOST_set(1) |
      SERDES_RIGHT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__E_PLLB_LOCK_LOST_set(1) |
      SERDES_RIGHT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__IO_PLLA_LOCK_LOST_set(1)
      ;

    DCRWritePriv( SERDES_RIGHT_DCR( SERDES_INTERRUPT_STATE_CONTROL_HIGH ), rightControlHigh );

    DCRWritePriv( SERDES_LEFT_DCR(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), 
		  // [5470] SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
		  // [5470] SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
		  SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
		  SERDES_LEFT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
		  0 );


    DCRWritePriv( SERDES_RIGHT_DCR(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), 
		  // [5470] SERDES_RIGHT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
		  // [5470] SERDES_RIGHT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
		  SERDES_RIGHT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
		  SERDES_RIGHT_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
		  0 );



    void fw_serdes_dump_calibration();
    fw_serdes_dump_calibration();

    TRACE_EXIT(TRACE_SerDes);

  return 0;
}

#define sd_get_std_bit16(bit)       (1     << (bit))
#define sd_std_move_upto(val,bit)   ((val) << (bit))
#define sd_ibm_move64_upto(val,bit) ((val) << (63-(bit)))
#define sd_get_ibm_bit64(bit)       (0x8000000000000000ULL >> bit)

#define RCB_BT_SUPPORT 0
#define RCB_BT_LANE    1

typedef uint64_t dcr_val_t;
typedef uint32_t dcr_addr_t;
typedef uint16_t rcb_val_t;
typedef uint32_t rcb_addr_t;

static dcr_val_t dcr_rcconfig_l_shad     = 0;
static dcr_val_t dcr_rcconfig_l_shad_act = 0;
static dcr_val_t dcr_rcconfig_r_shad     = 0;
static dcr_val_t dcr_rcconfig_r_shad_act = 0;

//static unsigned exclude_link = 99;
//static unsigned suppress_bcast_left  = 0;
//static unsigned suppress_bcast_right = 0;

struct rc_init_addr_t {
	dcr_addr_t combine;
	dcr_addr_t left;
	dcr_addr_t right;
};

/*
 ################################### HELPERs ##################################
 */
#define rcb_debug(fmt) 
//-	if (1 == rcb_debug_level) printf("RCB-DEBUG: "fmt"\n");
#define rcb_debug_1(fmt, a0) 
//-	if (1 == rcb_debug_level) printf("RCB-DEBUG: "fmt"\n", a0);
#define rcb_debug_2(fmt, a0, a1) 
//-	if (1 == rcb_debug_level) printf("RCB-DEBUG: "fmt"\n", a0, a1);
#define rcb_debug_5(fmt, a0, a1, a2, a3, a4) 
//-	if (1 == rcb_debug_level) printf("RCB-DEBUG: "fmt"\n", a0, a1, a2, a3, a4);
#define rcb_debug_6(fmt, a0, a1, a2, a3, a4, a5) 
//-	if (1 == rcb_debug_level) printf("RCB-DEBUG: "fmt"\n", a0, a1, a2, a3, a4, a5);

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
		DCRWritePriv(SERDES_LEFT_DCR(RCBUS_CONFIG), addr);
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
		DCRWritePriv(SERDES_RIGHT_DCR(RCBUS_CONFIG), addr);
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
		rc = DCRReadPriv(reg | SERDES_LEFT_DCR(RCBUS));
	}

	if (read_right) {
		rcb_set_config_right(99, rcbus_config);
		rc = DCRReadPriv(reg | SERDES_RIGHT_DCR(RCBUS));
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

	/* determine and setup target links */
	if (target_link == 0xfff) {
	    bcast_left = 1;
	    bcast_right = 1;
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
	    FW_Error("RCB Bug [%s:%d]", __func__, __LINE__);
	    Terminate(-1);
	}

	/* now process the writes */
	if (bcast_left) {
	    rcb_set_config_left(99, rcbus_config);
	    DCRWritePriv(reg | SERDES_LEFT_DCR(RCBUS), value);
	}

	if (bcast_right) {
		rcb_set_config_right(99, rcbus_config);
		DCRWritePriv(reg | SERDES_RIGHT_DCR(RCBUS), value);
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
				DCRWritePriv(reg | SERDES_LEFT_DCR(RCBUS), value);
			}
		}
	}

	if (ucast_right) {
		for (i = 3; i < 6; i++) {
			if (1 == link[i]) {
				rcb_set_config_right(i, rcbus_config);
				DCRWritePriv(reg | SERDES_RIGHT_DCR(RCBUS), value);
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
 * rcb_read
 *-----------------------------------*/
rcb_val_t rcb_read(int fam, int link, int type, int lane, int reg)
{
	rcb_addr_t rca;
	rcb_val_t  rc;

	rca = rcb_address(fam, link, type, lane, reg);
	rc  = rcb_addr_read(rca);

	rcb_debug_6("read from fam=%d link=%xh type=%d lane=%d reg=%xh val=%x\n",
			fam, link, type, lane, reg, rc);

	return rc;
}

/*-----------------------------------
 * rcb_write
 *-----------------------------------*/
void rcb_write(int fam, int link, int type, int lane, int reg, rcb_val_t value)
{
	rcb_addr_t rca;

	rcb_debug_6("write to fam =%d link=%xh type=%d lane=%d reg=%xh val=%x\n",
			fam, link, type, lane, reg, value);

	rca =  rcb_address(fam, link, type, lane, reg);
	rcb_addr_write(rca, value);
}


void fw_hss_pcie_init( void ) {

    uint64_t dval;

    /* De-Assert PCS_RESET */
    dval = DCRReadPriv(SERDES_RIGHT_DCR(SD_CONFIG));
    dval = dval & ~(SERDES_RIGHT_DCR__SD_CONFIG__PCIE_DISABLE_set(1));
    DCRWritePriv(SERDES_RIGHT_DCR(SD_CONFIG), dval);

    /* Save TS_CTL_E */
    dval = DCRReadPriv(SERDES_RIGHT_DCR(TS_CTL_E));

    /* assert AREFEN */
    dval = dval | SERDES_RIGHT_DCR__TS_CTL_E__HSS_AREFEN_set(1);
    DCRWritePriv(SERDES_RIGHT_DCR(TS_CTL_E), dval );
    dval = DCRReadPriv(SERDES_RIGHT_DCR(TS_CTL_E));

    /* assert SBRESET */
    rcb_write(9, 4, RCB_BT_SUPPORT, 0, 0x1c,
	      sd_get_std_bit16(0) | // override
	      sd_get_std_bit16(6)); // sbreset

    /* de-assert SBRESET */
    rcb_write(9, 4, RCB_BT_SUPPORT, 0, 0x1c, 0);

    /* De-Assert PCS_RESET */
    dval = dval & ~(SERDES_RIGHT_DCR__TS_CTL_E__PCS_RESET_set(1));
    DCRWritePriv(SERDES_RIGHT_DCR(TS_CTL_E), dval);

    /* De-Assert RESET_N */
    dval = dval | SERDES_RIGHT_DCR__TS_CTL_E__PIPE_RESETN_set(1);
    DCRWritePriv(SERDES_RIGHT_DCR(TS_CTL_E), dval );

    /* Wait for PLL up (0.3ms) */
    fw_udelay(2500ull);
    dval = DCRReadPriv(SERDES_RIGHT_DCR(TS_HSS_PLL_STAT));
    if (0 == (dval & SERDES_RIGHT_DCR__TS_HSS_PLL_STAT__E_PLLB_LOCKED_set(1))) {
	FW_Warning("PCIe PLL did not lock.");
    }
}


void fw_serdes_pcie_init(void) {

    dcr_val_t dval;

    // Mask off PLL Lock Interrupts

    dval = DCRReadPriv( SERDES_RIGHT_DCR( SERDES_INTERRUPT_STATE_CONTROL_HIGH ) );
    dval &= ~SERDES_RIGHT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__E_PLLB_LOCK_LOST_set(3);
    DCRWritePriv( SERDES_RIGHT_DCR( SERDES_INTERRUPT_STATE_CONTROL_HIGH ), dval );
    ppc_msync();


    //first set timeout to max for rcbus
    dval  = DCRReadPriv(DC_ARBITER_DCR(ARB_CTRL));
    dval |= DC_ARBITER_DCR__ARB_CTRL__TIMER_VAL_set(0xf);
    DCRWritePriv(DC_ARBITER_DCR(ARB_CTRL), dval);

    // Enable all HSS cores
    DCRWritePriv(TESTINT_DCR(CONFIG1)+1,
		 sd_get_ibm_bit64(43) |
		 sd_get_ibm_bit64(44) |
		 sd_get_ibm_bit64(45) |
		 sd_get_ibm_bit64(46) |
		 sd_get_ibm_bit64(47) |
		 sd_get_ibm_bit64(48));

    fw_hss_pcie_init();

    // Ack any PLL loss interrupts that may have occurred
    
    DCRWritePriv( SERDES_RIGHT_DCR(SERDES_INTERRUPT_STATE__STATE), SERDES_RIGHT_DCR__SERDES_INTERRUPT_STATE__E_PLLB_LOCK_LOST_set(1) );
    ppc_msync();

    dval = DCRReadPriv( SERDES_RIGHT_DCR( SERDES_INTERRUPT_STATE_CONTROL_HIGH ) );
    dval |= SERDES_RIGHT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__E_PLLB_LOCK_LOST_set(1);
    DCRWritePriv( SERDES_RIGHT_DCR( SERDES_INTERRUPT_STATE_CONTROL_HIGH ), dval );
    ppc_msync();

}


void fw_serdes_dump_calibration() {
    if ( TRACE_ENABLED(TRACE_SerDes) ) {
	int link, dir, lane;

	for (link = 0; link < 6; link++) {
	    for (dir = 0; dir < 2; dir++) {
		for (lane = 0; lane < 4; lane++) {
		    if ( 1 /*hss_lane_active(link, dir, lane)*/ ) {
			rcb_val_t v3e, v40, v42, v44, v4a;
			v3e = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x3e) & 0x2F;
			v40 = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x40) & 0x2F;
			v42 = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x42) & 0x2F;
			v44 = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x44) & 0x2F;
			v4a = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x4a) & 0x2F;
			printf("Lane %c%c:%d " "rxvosd0=%d rxvosd1=%d rxvose0=%d rxvose1=%d rxvoseq=%d\n",
			       "ABCDEI"[link], "-+"[dir], (dir*4)+lane,
			       v3e, v40, v42, v44, v4a);

		    }
		}
	    }
	}
    }
}
