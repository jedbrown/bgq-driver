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
//
// hss_rx_cal: HSS receiver calibration program
//

#include <firmware/include/fwext/fwext.h>
#include <firmware/include/fwext/fwext_lib.h>
#include <firmware/include/fwext/sd_rcb.h>

#include <stdio.h>

///////////////////////////////////////////////////////////////////////
// HSS RX CAL version history
// 2/1/2011 - Version 1.0 of HSS RX CAL for writing to compute VPD.
//
// The version number below needs to increment each time the algorithm changes for HSS RX calibration values.
#define HSS_RX_CAL_VERSION      1

#define DIAGS_HSS_RX_CAL_NO_CALIBRATION 0x00020300

// A flag indicating we're running at FCT. This affects certain expectations in the test code.
int runningAtFct = 0;

const char LINKS[] = { 'A', 'B', 'C', 'D', 'E', 'I' };

void hss_histogram_reg(int link, int dir, int lane, int reg)
{
    rcb_val_t reg_orig, reg_new, reg_samp;
    int sign, mag, index, i;
    int count[64];
    char hist[65];

    reg_orig = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, reg);

    for (index = 63; index >= 0; index--) {
	sign = (index < 32);
	mag = (index - 32 + sign) * (1 - (sign*2));
	reg_new = (reg_orig & ~0x003f) | ((sign << 5) | mag);
	rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, reg, reg_new);
	count[index] = 0;
	for (i = 0; i < 15; i++) {
	    reg_samp = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, reg);
	    count[index] += ((reg_samp & 0x0080) >> 7);
	}
	hist[index] = ((count[index] < 10) ? '0' : 'A'-10) + count[index];
    }
    hist[64] = '\0';
    printf("Link %c%c lane %d, reg 0x%02x (0x%04x), right-to-left: %s\n",
	   "ABCDEI"[link], "-+"[dir], (dir*4)+lane, reg, reg_orig, hist);

    for (index = 0; index < 64; index++) {
	sign = (index < 32);
	mag = (index - 32 + sign) * (1 - (sign*2));
	reg_new = (reg_orig & ~0x003f) | ((sign << 5) | mag);
	rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, reg, reg_new);
	count[index] = 0;
	for (i = 0; i < 15; i++) {
	    reg_samp = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, reg);
	    count[index] += ((reg_samp & 0x0080) >> 7);
	}
	hist[index] = ((count[index] < 10) ? '0' : 'A'-10) + count[index];
    }
    hist[64] = '\0';
    printf("Link %c%c lane %d, reg 0x%02x (0x%04x), left-to-right: %s\n",
	   "ABCDEI"[link], "-+"[dir], (dir*4)+lane, reg, reg_orig, hist);
}

// auto-calibration
void hss_calibrate_auto(int link, int dir, int lane)
{
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36, 0x0100);
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36, 0x0102);
    while (rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36) != 0x0106) /*spin*/;
}

int hss_calibrate_reg(int link, int dir, int lane, int reg)
{
    rcb_val_t reg_orig, reg_new;
    int sign, mag, left, right, middle;

    reg_orig = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, reg);

    for (left = 62; left > 0; left--) {
	sign = (left < 31);
	mag = (left - 31) * (1 - (sign*2));
	reg_new = (reg_orig & ~0x003f) | ((sign << 5) | mag);
	rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, reg, reg_new);
	if ((rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, reg) & 0x0080) != 0) {
	    break;
	}
    }

    for (right = 0; right < 62; right++) {
	sign = (right < 31);
	mag = (right - 31) * (1 - (sign*2));
	reg_new = (reg_orig & ~0x003f) | ((sign << 5) | mag);
	rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, reg, reg_new);
	if ((rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, reg) & 0x0080) == 0) {
	    break;
	}
    }

    middle = (left + right) / 2;

    sign = (middle < 31);
    mag = (middle - 31) * (1 - (sign*2));
    reg_new = (reg_orig & ~0x003f) | ((sign << 5) | mag);
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, reg, reg_new);
    reg_new = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, reg);

    return (middle - 31);
}

int hss_calibrate_lane(int link, int dir, int lane, int fudge)
{
    rcb_val_t auxinterp_orig, auxinterp_new;
    rcb_val_t escope_orig, escope_new;
    rcb_val_t rxfectrl_orig, rxfectrl_new;
    rcb_val_t rxvosctl_orig, rxvosctl_new;
    rcb_val_t rxvoseq_orig, rxvoseq_new;
    int eqoffset, d0offset_orig, d0offset_fudged;

    //auxinterp.en = 1;
    //auxinterp.escope = 1;
    auxinterp_orig = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x02);
    auxinterp_new = (auxinterp_orig & ~0x0001) | 0x0001;
    auxinterp_new = (auxinterp_new & ~0x0002) | 0x0002;
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x02, auxinterp_new);

    //escope.en = 1;
    escope_orig = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x24);
    escope_new = (escope_orig & ~0x0100) | 0x0100;
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x24, escope_new);

    //rxfectrl.eq_ovr = 1;
    //rxfectrl.sloop = 0;
    //rxfectrl.lsen = 1;
    //rxfectrl.lsby = 0;
    rxfectrl_orig = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x22);
    rxfectrl_new = (rxfectrl_orig & ~0x0004) | 0x0004;
    rxfectrl_new = (rxfectrl_new  & ~0x0008) | 0x0000;
    rxfectrl_new = (rxfectrl_new  & ~0x0010) | 0x0010;
    rxfectrl_new = (rxfectrl_new  & ~0x0020) | 0x0000;
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x22, rxfectrl_new);

    //rxvosctl.ovr = 1;
    //rxvosctl.vosen = 0b100;
    rxvosctl_orig = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36);
    rxvosctl_new = (rxvosctl_orig & ~0x0001) | 0x0001;
    rxvosctl_new = (rxvosctl_new  & ~0x0038) | 0x0020;
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36, rxvosctl_new);

    //rxvoseq.vos = 0b000000;
    rxvoseq_orig = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x4a);
    rxvoseq_new = (rxvoseq_orig & ~0x003f) | 0x0000;
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x4a, rxvoseq_new);

    d0offset_orig = hss_calibrate_reg(link, dir, lane, 0x3e);
    hss_calibrate_reg(link, dir, lane, 0x40);
    hss_calibrate_reg(link, dir, lane, 0x42);
    hss_calibrate_reg(link, dir, lane, 0x44);
    hss_calibrate_reg(link, dir, lane, 0x46);
    hss_calibrate_reg(link, dir, lane, 0x48);

    //rxvosctl.vosen = 0b010; (for DC links (A - D))
    //rxvosctl.vosen = 0b001; (for AC links (E))
    rxvosctl_new = (rxvosctl_new  & ~0x0038) | ((link < 4) ? 0x0010 : 0x0008);
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36, rxvosctl_new);

    eqoffset = hss_calibrate_reg(link, dir, lane, 0x4a);

    if (fudge && (eqoffset == -31)) {
	d0offset_fudged = hss_calibrate_reg(link, dir, lane, 0x3e);
	hss_calibrate_reg(link, dir, lane, 0x40);
	hss_calibrate_reg(link, dir, lane, 0x42);
	hss_calibrate_reg(link, dir, lane, 0x44);
	hss_calibrate_reg(link, dir, lane, 0x46);
	hss_calibrate_reg(link, dir, lane, 0x48);
    } else {
	d0offset_fudged = d0offset_orig;
    }

    //rxvosctl.vosen = 0b000;
    rxvosctl_new = (rxvosctl_new  & ~0x0038) | 0x0000;
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36, rxvosctl_new);

    // restore rxfectrl, escope, and auxinterp
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x22, rxfectrl_orig);
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x24, escope_orig);
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x02, auxinterp_orig);

    return (d0offset_orig - d0offset_fudged);
}

void hss_calibration_dump_lane(int link, int dir, int lane)
{
    rcb_val_t v34, v36, v3e, v40, v42, v44, v46, v48, v4a;

    v34 = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x34);
    v36 = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36);
    v3e = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x3e);
    v40 = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x40);
    v42 = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x42);
    v44 = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x44);
    v46 = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x46);
    v48 = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x48);
    v4a = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x4a);
    printf("Link %c%c lane %d: 0x34=0x%04x 0x36=0x%04x 0x3e=0x%04x 0x40=0x%04x "
	   "0x42=0x%04x 0x44=0x%04x 0x4a=0x%04x\n",
	   //"0x42=0x%04x 0x44=0x%04x 0x46=0x%04x 0x48=0x%04x 0x4a=0x%04x\n",
	   "ABCDEI"[link], "-+"[dir], (dir*4)+lane,
	   v34, v36, v3e, v40, v42, v44, /*v46, v48,*/ v4a);
}

void hss_explore_lane(int link, int dir, int lane, int fudge)
{
    int rxeq, rxeq_orig;
    int f[8];
    rxeq_orig = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x34);
    for (rxeq = 0; rxeq < 8; rxeq++) {
	rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x34, rxeq);
	f[rxeq] = hss_calibrate_lane(link, dir, lane, fudge);
	hss_calibration_dump_lane(link, dir, lane);
    }
    printf("Link %c%c lane %d: fudge factor %2d %2d %2d %2d %2d %2d %2d %2d\n",
	   "ABCDEI"[link], "-+"[dir], (dir*4)+lane,
	   f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7]);
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x34, rxeq_orig);
    hss_calibrate_lane(link, dir, lane, fudge);
}

int hss_lane_active(int link, int dir, int lane)
{
  // A flag indicating if the given lane is active.
  int laneActive = 0;

  // A flag indicating if the link is a valid torus link.
  int isValidABCDELink = (link < 5) && (dir < 2);

  // A flag indicating if the link is a valid I/O link.
  int isValidIOLink = (link == 5) && (dir == 0);

  // Ensure we're checking a valid link.
  if(isValidABCDELink || isValidIOLink)
  {
    // Pull out the calibration complete bit from the Receive voltage-offset FSM controls register (rxvosctl).
    int calibrationComplete = (rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36) & 0x0004);

    // Check to see if the given lane completed calibration.
    if(calibrationComplete)
    {
      // If so mark the lane as active.
      laneActive = 1;
    }
  }

  return laneActive;
}

void hss_calibrate_all(int fudge)
{
    int link, dir, lane;

    for (link = 0; link < 6; link++) {
	for (dir = 0; dir < 2; dir++) {
	    for (lane = 0; lane < 4; lane++) {
		if (hss_lane_active(link, dir, lane)) {
		    hss_calibrate_lane(link, dir, lane, fudge);
		}
	    }
	}
    }
}

void hss_explore_all(int fudge)
{
  int link, dir, lane;

  // Check all links ABCDEI.
  for (link = 0; link < 6; link++)
  {
    // Check all directions -/+.
    for (dir = 0; dir < 2; dir++)
    {
      // Check for I+ (this does not exist).
      if((link == 5) && (dir == 1))
      {
        // Skip the invalid I+ direction.
        continue;
      }

      // Check all four lanes for each dimension/direction.
      for (lane = 0; lane < 4; lane++)
      {
        // Ensure this lane is active before using it.
        if (hss_lane_active(link, dir, lane))
        {
          // Dump the calibration settings.
          hss_explore_lane(link, dir, lane, fudge);
        }
        else
        {
          // If we're running at FCT we expect all lanes to be active.
          if(runningAtFct)
          {
            // Post a RAS event indicating a lane did not complete calibration when it should have.
            fw_uint64_t details[3];
            details[0] = LINKS[link];
            details[1] = (dir == 0) ? '-' : '+';
            details[2] = lane;
            fwext_getFwInterface()->writeRASEvent(DIAGS_HSS_RX_CAL_NO_CALIBRATION, 3, details);
          }
        }
      }
    }
  }
}

void hss_calibration_dump()
{
    int link, dir, lane;

    for (link = 0; link < 6; link++) {
	for (dir = 0; dir < 2; dir++) {
	    for (lane = 0; lane < 4; lane++) {
		if (hss_lane_active(link, dir, lane)) {
		    hss_calibration_dump_lane(link, dir, lane);
		}
	    }
	}
    }
}

int test_main()
{
  if (ProcessorID() > 0)
  {
    // Run single-threaded.
    test_exit(0);
  }

  printf("hss_rx_cal version %d", HSS_RX_CAL_VERSION);

  // Check to see if we're running at FCT.
  if (fwext_getenv("FCT") != 0)
  {
    runningAtFct = 1;
  }

  if (fwext_getenv("HSS_EXPLORE_ALL") != 0)
  {
    // To generate calibrations for all rxeq settings:
    hss_explore_all(1);
  }
  else if (fwext_getenv("HSS_CALIBRATION_DUMP") != 0)
  {
    // "dump-only" mode:
    hss_calibration_dump();
  }
  else
  {
    // Calibrate and dump:
    hss_calibrate_all(1); // 1 for fudging
    hss_calibration_dump();
  }

  test_exit(0);
}
