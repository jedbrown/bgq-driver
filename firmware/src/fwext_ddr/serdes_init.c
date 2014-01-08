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

#define _BGQ_SERDES_INIT_C_
 
#include <firmware/include/fwext/fwext.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <firmware/include/Firmware.h>
#include <hwi/include/bqc/serdes_left_dcr.h>
#include <hwi/include/bqc/serdes_right_dcr.h>
#include <firmware/include/fwext/sd_helper.h>
#include <firmware/include/fwext/serdes.h>
#include <firmware/include/fwext/core_parms.h>

int sd_dbg_checkpoint = 0;

void serdes_init(Personality_t *p)
{
  /* contains dcr defaults (e.g. reset) plus config*/
  /* todo: move to .h */

  const uint64_t sd_loop_step1 = SERDES_LEFT_DCR__TS_CTL_A__PCS_RESET_set(0x1)    // 1     _BGQ_SET(1,10,v)
    | SERDES_LEFT_DCR__TS_CTL_A__PIPE_RESETN_set(0x0)               // 0     _BGQ_SET(1,11,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RX_RESET_M_set(0xF)                // 1111  _BGQ_SET(4,15,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RX_RESET_P_set(0xF)                // 1111  _BGQ_SET(4,19,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_M_set(0x0)      // 0     _BGQ_SET(1,20,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_OVR_M_set(0x0)  // 0     _BGQ_SET(1,21,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_LAT_M_set(0x4)      // 100   _BGQ_SET(3,24,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_P_set(0x0)      // 0     _BGQ_SET(1,25,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_OVR_P_set(0x0)  // 0     _BGQ_SET(1,26,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_LAT_P_set(0x4)      // 100   _BGQ_SET(3,29,v)
    | SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_RUN_M_set(0x1)      // 1     _BGQ_SET(1,30,v)
    | SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_LAT_M_set(0x4)      // 100   _BGQ_SET(3,34,v)
    | SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_RUN_P_set(0x1)      // 1     _BGQ_SET(1,35,v)
    | SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_LAT_P_set(0x4)      // 100   _BGQ_SET(3,39,v)
    | SERDES_LEFT_DCR__TS_CTL_A__XMIT_SCRAMBLE_M_set(0x0)           // 0     _BGQ_SET(1,40,v)
    | SERDES_LEFT_DCR__TS_CTL_A__XMIT_SCRAMBLE_P_set(0x0)           // 0     _BGQ_SET(1,41,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RCV_DESCRAMBLE_M_set(0x0)          // 0     _BGQ_SET(1,42,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RCV_DESCRAMBLE_P_set(0x0)          // 0     _BGQ_SET(1,43,v)
    | SERDES_LEFT_DCR__TS_CTL_A__SEND_TRAIN_M_set(0x0)              // 0     _BGQ_SET(1,45,v)
    | SERDES_LEFT_DCR__TS_CTL_A__SEND_TRAIN_P_set(0x0)              // 0     _BGQ_SET(1,46,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RCV_TRAIN_M_set(0x0)               // 0     _BGQ_SET(1,47,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RCV_TRAIN_P_set(0x0)               // 0     _BGQ_SET(1,48,v)
    | SERDES_LEFT_DCR__TS_CTL_A__LOOPBACK_EN_set(0x1)               // 0     _BGQ_SET(1,49,v)
    | SERDES_LEFT_DCR__TS_CTL_A__DATALOOP_EN_M_set(0x0)             // 1     _BGQ_SET(1,50,v)
    | SERDES_LEFT_DCR__TS_CTL_A__DATALOOP_EN_P_set(0x0)             // 0     _BGQ_SET(1,51,v)
    | SERDES_LEFT_DCR__TS_CTL_A__PARLPBK_EN_M_set(0x0)              // 0     _BGQ_SET(1,52,v)
    | SERDES_LEFT_DCR__TS_CTL_A__PARLPBK_EN_P_set(0x0)              // 0     _BGQ_SET(1,53,v)
    | SERDES_LEFT_DCR__TS_CTL_A__REFCLK_DISABLE_set(0x0)            // 0     _BGQ_SET(1,54,v)
    | SERDES_LEFT_DCR__TS_CTL_A__LANE_RESET_M_set(0xF)              // 1111  _BGQ_SET(4,58,v)
    | SERDES_LEFT_DCR__TS_CTL_A__LANE_RESET_P_set(0xF)              // 1111  _BGQ_SET(4,62,v)
    | SERDES_LEFT_DCR__TS_CTL_A__HSS_AREFEN_set(0x0);               // 0     _BGQ_SET(1,63,v)

  const uint64_t sd_loop_step1_io = SERDES_RIGHT_DCR__TS_CTL_IO__PCS_RESET_set(0x1) //       _BGQ_SET(1,10,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__PIPE_RESETN_set(0x0)             //       _BGQ_SET(1,11,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__RX_RESET_set(0xF)                //       _BGQ_SET(4,15,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__RX_ELASTIC_BUF_RUN_set(0x0)      //       _BGQ_SET(1,20,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__RX_ELASTIC_BUF_RUN_OVR_set(0x0)  //       _BGQ_SET(1,21,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__RX_ELASTIC_BUF_LAT_set(0x4)      //       _BGQ_SET(3,24,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__TX_ELASTIC_BUF_RUN_set(0x1)      //       _BGQ_SET(1,30,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__TX_ELASTIC_BUF_LAT_set(0x4)      //       _BGQ_SET(3,34,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__XMIT_SCRAMBLE_set(0x0)           //       _BGQ_SET(1,40,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__RCV_DESCRAMBLE_set(0x0)          //       _BGQ_SET(1,42,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__SEND_TRAIN_set(0x0)              //       _BGQ_SET(1,45,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__RCV_TRAIN_set(0x0)               //       _BGQ_SET(1,47,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__LOOPBACK_EN_set(0x1)             //       _BGQ_SET(1,49,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__DATALOOP_EN_set(0x0)             //       _BGQ_SET(1,50,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__PARLPBK_EN_set(0x0)              //       _BGQ_SET(1,52,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__REFCLK_DISABLE_set(0x0)          //       _BGQ_SET(1,54,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__LANE_RESET_set(0xF)              //       _BGQ_SET(4,58,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__HSS_AREFEN_set(0x0);             //       _BGQ_SET(1,63,v)
  
  /* contains config minus resets */
  const uint64_t sd_loop_step2 = SERDES_LEFT_DCR__TS_CTL_A__PCS_RESET_set(0x1)    // 1     _BGQ_SET(1,10,v)
    | SERDES_LEFT_DCR__TS_CTL_A__PIPE_RESETN_set(0x0)               // 0     _BGQ_SET(1,11,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RX_RESET_M_set(0xf)                // 1111  _BGQ_SET(4,15,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RX_RESET_P_set(0xf)                // 1111  _BGQ_SET(4,19,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_M_set(0x0)      // 0     _BGQ_SET(1,20,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_OVR_M_set(0x0)  // 0     _BGQ_SET(1,21,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_LAT_M_set(0x4)      // 100   _BGQ_SET(3,24,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_P_set(0x0)      // 0     _BGQ_SET(1,25,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_RUN_OVR_P_set(0x0)  // 0     _BGQ_SET(1,26,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RX_ELASTIC_BUF_LAT_P_set(0x4)      // 100   _BGQ_SET(3,29,v)
    | SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_RUN_M_set(0x1)      // 1     _BGQ_SET(1,30,v)
    | SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_LAT_M_set(0x4)      // 100   _BGQ_SET(3,34,v)
    | SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_RUN_P_set(0x1)      // 1     _BGQ_SET(1,35,v)
    | SERDES_LEFT_DCR__TS_CTL_A__TX_ELASTIC_BUF_LAT_P_set(0x4)      // 100   _BGQ_SET(3,39,v)
    | SERDES_LEFT_DCR__TS_CTL_A__XMIT_SCRAMBLE_M_set(0x0)           // 0     _BGQ_SET(1,40,v)
    | SERDES_LEFT_DCR__TS_CTL_A__XMIT_SCRAMBLE_P_set(0x0)           // 0     _BGQ_SET(1,41,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RCV_DESCRAMBLE_M_set(0x0)          // 0     _BGQ_SET(1,42,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RCV_DESCRAMBLE_P_set(0x0)          // 0     _BGQ_SET(1,43,v)
    | SERDES_LEFT_DCR__TS_CTL_A__SEND_TRAIN_M_set(0x0)              // 0     _BGQ_SET(1,45,v)
    | SERDES_LEFT_DCR__TS_CTL_A__SEND_TRAIN_P_set(0x0)              // 0     _BGQ_SET(1,46,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RCV_TRAIN_M_set(0x0)               // 0     _BGQ_SET(1,47,v)
    | SERDES_LEFT_DCR__TS_CTL_A__RCV_TRAIN_P_set(0x0)               // 0     _BGQ_SET(1,48,v)
    | SERDES_LEFT_DCR__TS_CTL_A__LOOPBACK_EN_set(0x1)               // 0     _BGQ_SET(1,49,v)
    | SERDES_LEFT_DCR__TS_CTL_A__DATALOOP_EN_M_set(0x0)             // 1     _BGQ_SET(1,50,v)
    | SERDES_LEFT_DCR__TS_CTL_A__DATALOOP_EN_P_set(0x0)             // 0     _BGQ_SET(1,51,v)
    | SERDES_LEFT_DCR__TS_CTL_A__PARLPBK_EN_M_set(0x0)              // 0     _BGQ_SET(1,52,v)
    | SERDES_LEFT_DCR__TS_CTL_A__PARLPBK_EN_P_set(0x0)              // 0     _BGQ_SET(1,53,v)
    | SERDES_LEFT_DCR__TS_CTL_A__REFCLK_DISABLE_set(0x0)            // 0     _BGQ_SET(1,54,v)
    | SERDES_LEFT_DCR__TS_CTL_A__LANE_RESET_M_set(0xF)              // 1111  _BGQ_SET(4,58,v)
    | SERDES_LEFT_DCR__TS_CTL_A__LANE_RESET_P_set(0xF)              // 1111  _BGQ_SET(4,62,v)
    | SERDES_LEFT_DCR__TS_CTL_A__HSS_AREFEN_set(0x0);               // 0     _BGQ_SET(1,63,v)

  const uint64_t sd_loop_step2_io = SERDES_RIGHT_DCR__TS_CTL_IO__PCS_RESET_set(0x1) //       _BGQ_SET(1,10,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__PIPE_RESETN_set(0x0)             //       _BGQ_SET(1,11,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__RX_RESET_set(0xf)                //       _BGQ_SET(4,15,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__RX_ELASTIC_BUF_RUN_set(0x0)      //       _BGQ_SET(1,20,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__RX_ELASTIC_BUF_RUN_OVR_set(0x0)  //       _BGQ_SET(1,21,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__RX_ELASTIC_BUF_LAT_set(0x4)      //       _BGQ_SET(3,24,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__TX_ELASTIC_BUF_RUN_set(0x1)      //       _BGQ_SET(1,30,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__TX_ELASTIC_BUF_LAT_set(0x4)      //       _BGQ_SET(3,34,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__XMIT_SCRAMBLE_set(0x0)           //       _BGQ_SET(1,40,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__RCV_DESCRAMBLE_set(0x0)          //       _BGQ_SET(1,42,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__SEND_TRAIN_set(0x0)              //       _BGQ_SET(1,45,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__RCV_TRAIN_set(0x0)               //       _BGQ_SET(1,47,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__LOOPBACK_EN_set(0x1)             //       _BGQ_SET(1,49,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__DATALOOP_EN_set(0x0)             //       _BGQ_SET(1,50,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__PARLPBK_EN_set(0x0)              //       _BGQ_SET(1,52,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__REFCLK_DISABLE_set(0x0)          //       _BGQ_SET(1,54,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__LANE_RESET_set(0xF)              //       _BGQ_SET(4,58,v)
    | SERDES_RIGHT_DCR__TS_CTL_IO__HSS_AREFEN_set(0x0);             //       _BGQ_SET(1,63,v)



	/* todo: move to .h */

	if (0 < libfw_sd_debug) {
		printf("libfs_sd: get peronality map\n");
		printf("p->Network_Config.NetFlags = %llx\n", 
		       p->Network_Config.NetFlags);
	}

	sd_dbg_checkpoint = 1;
	personality_convert_map(p);

	if (0 < libfw_sd_debug) {
		printf("use_a = %d\n", use_a);
		printf("use_b = %d\n", use_b);
		printf("use_c = %d\n", use_c);
		printf("use_d = %d\n", use_d);
		printf("use_e = %d\n", use_e);
		printf("use_io= %d\n", use_io);
	}

	/* This is a short version which only sets up SD loopback for
	 * Network self test.
	 *
	 * FIXME :
	 * - respect reset value
	 * - use .h defines
	 */
	
	if (0 < libfw_sd_debug) printf("libfs_sd: init STEP1\n");
	sd_dbg_checkpoint = 2;
	/* STEP 1: configure */
	if (use_a)  sd_dcr_wr(SERDES_LEFT_DCR(TS_CTL_A),   sd_loop_step1);
	if (use_b)  sd_dcr_wr(SERDES_LEFT_DCR(TS_CTL_B),   sd_loop_step1);
	if (use_c)  sd_dcr_wr(SERDES_LEFT_DCR(TS_CTL_C),   sd_loop_step1);
	if (use_d)  sd_dcr_wr(SERDES_RIGHT_DCR(TS_CTL_D),  sd_loop_step1);
	if (use_e)  sd_dcr_wr(SERDES_RIGHT_DCR(TS_CTL_E),  sd_loop_step1);
	if (use_io) sd_dcr_wr(SERDES_RIGHT_DCR(TS_CTL_IO), sd_loop_step1_io);

	if (0 < libfw_sd_debug) printf("libfs_sd: init STEP2\n");
	sd_dbg_checkpoint = 3;
	/* STEP 2: release RX, PIPE0 and PCS reset */
	if (use_a)  sd_dcr_wr(SERDES_LEFT_DCR(TS_CTL_A),   sd_loop_step2);
	if (use_b)  sd_dcr_wr(SERDES_LEFT_DCR(TS_CTL_B),   sd_loop_step2);
	if (use_c)  sd_dcr_wr(SERDES_LEFT_DCR(TS_CTL_C),   sd_loop_step2);
	if (use_d)  sd_dcr_wr(SERDES_RIGHT_DCR(TS_CTL_D),  sd_loop_step2);
	if (use_e)  sd_dcr_wr(SERDES_RIGHT_DCR(TS_CTL_E),  sd_loop_step2);
	if (use_io) sd_dcr_wr(SERDES_RIGHT_DCR(TS_CTL_IO), sd_loop_step2_io);

	if (0 < libfw_sd_debug) printf("libfs_sd: init done\n");
	sd_dbg_checkpoint = 4;
}

void sd_set_halfrate(int enable) {
  uint64_t dval = 0;
  if (enable == 0) {
    // Disable halfrate mode
    sd_en_halfrate = 0;
    
    dval  = sd_dcr_rd(SERDES_LEFT_DCR(SD_CONFIG));
    dval &= ~SERDES_LEFT_DCR__SD_CONFIG__HALFRATE_set(1);
    sd_dcr_wr(SERDES_LEFT_DCR(SD_CONFIG), dval);
    
    dval  = sd_dcr_rd(SERDES_RIGHT_DCR(SD_CONFIG));
    dval &= ~SERDES_RIGHT_DCR__SD_CONFIG__HALFRATE_set(1);
    sd_dcr_wr(SERDES_RIGHT_DCR(SD_CONFIG), dval);
  } else {
    // Enable halfrate mode
    sd_en_halfrate = 1;
    
    dval  = sd_dcr_rd(SERDES_LEFT_DCR(SD_CONFIG));
    dval |= SERDES_LEFT_DCR__SD_CONFIG__HALFRATE_set(1);
    sd_dcr_wr(SERDES_LEFT_DCR(SD_CONFIG), dval);
    
    dval  = sd_dcr_rd(SERDES_RIGHT_DCR(SD_CONFIG));
    dval |= SERDES_RIGHT_DCR__SD_CONFIG__HALFRATE_set(1);
    sd_dcr_wr(SERDES_RIGHT_DCR(SD_CONFIG), dval);
  }
}
