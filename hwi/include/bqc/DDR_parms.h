/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
#define	ddrMAX(x,y)	((x)>(y))?(x):(y)
#define	ddrMIN(x,y)	((x)<(y))?(x):(y)

	switch(dram_speed)
	{
	    case 667:	ddr.CL  = 5;
			ddr.CWL = 5;
			ddr.TCK = 3.0;	break;

	    case 800:	ddr.CL  = 6;
			ddr.CWL = 5;
			ddr.TCK = 2.5;	break;

	    case 1000:	ddr.CL  = 8;		// 7 is not working, needs optimization
			ddr.CWL = 6;
			ddr.TCK = 2.0;	break;

	    case 1200:	ddr.CL  = 8;
			ddr.CWL = 7;
			ddr.TCK = 1.67;	break;

	    case 1333:	ddr.CL  = 9;
			ddr.CWL = 7;
			ddr.TCK = 1.5;	break;

	    case 1600:	ddr.CL  = 10;		// 11 in 1600 is not working (in current cards)
			ddr.CWL = 8;
			ddr.TCK = 1.25;	break;

	    case 1667:	ddr.CL  = 11;
			ddr.CWL = 9;
			ddr.TCK = 1.2;	break;

	    case 2000:	ddr.CL  = 13;
			ddr.CWL = 10;
			ddr.TCK = 1.0;	break;

	    default:
			printf("ERROR: UNKNOWN DRAM SPEED (%u)\n",dram_speed);
	}

	ddr.AL		= ddr.CL - 1;
	ddr.GLB_OFS	= 5;
	ddr.WLAT	= ddr.CWL + ddr.AL - 13;
	ddr.RLAT	= ddr.CL + ddr.AL + ddr.GLB_OFS - 11;
	ddr.IOM_WL	= ddr.CWL + ddr.AL;
	ddr.IOM_RL	= ddr.CL + ddr.AL;

	ddr.TRRD	= ddrMAX(6.25/ddr.TCK, 4);
	ddr.TFAW	= 30/ddr.TCK;
	ddr.TWR		= ddrMIN(15/ddr.TCK, 10);	// For 1600 to keep 10 to save tRP under tight tRC
	ddr.TRC		= ddrMIN(ddr.CL + ddr.CWL + 4 + ddr.TWR + ddr.CL, 44);
//	ddr.TXP		= ddrMAX((dram_speed<1333? 7.49:5.99)/ddr.TCK + 1, 4);
//	ddr.TXPDLL	= ddrMAX(23.9/ddr.TCK + 1 - ddr.CL, 4);
	ddr.TXP		= 4;
	ddr.TXPDLL	= 4;
	ddr.TWRAPDEN	= ddr.AL + ddr.CWL + 4 + ddr.TWR + 1;
//	ddr.TWRAPDEN	= 1;
//	ddr.TWRAPDEN	= ddrMAX(80/ddr.TCK, 64);
	ddr.RR_S	= 4;
	ddr.RR_D	= 6;
	ddr.WW_S	= 4;
	ddr.WW_D	= 6 + TI_isDD1()?1:0;	// However, 5 also works
	ddr.RW_S	= ddr.CL - ddr.CWL + 7;
	ddr.RW_D	= ddr.CL - ddr.CWL + 7;
	ddr.WR_S	= ddr.AL + ddr.CWL + 4 + ddr.TWR;	// This is the optimum value for Wr:Rd=1:1
	ddr.WR_D	= ddrMAX(ddr.CWL - ddr.CL + 6, 4 + 1);	// Adding 1 more clk to optimize performance

	ddr.IMPEDANCE	= 0x0021;	// Default
	ddr.IMP_A[0]	= 0x0333;	// 1333 1-rank
	ddr.IMP_A[1]	= 0x0073;	// 1333 2-rank
	ddr.IMP_A[2]	= 0x0172;	// 1333 4-rank
	ddr.IMP_A[3]	= 0x0271;	// 1600 1-rank
	ddr.IMP_A[4]	= 0x0161;	// 1600 2-rank

	ddr.RTT	= 40;			// Default
	ddr.RTT_A[0]	= 0;		// 1333 1-rank
        ddr.RTT_A[1]	= PERS_ENABLED(PERS_ENABLE_DDRNoTerm) ? 0 : 120;		// 1333 2-rank
	ddr.RTT_A[2]	= 60;		// 1333 4-rank
	ddr.RTT_A[3]	= 60;		// 1600 1-rank
	ddr.RTT_A[4]	= 40;		// 1600 2-rank

	ddr.RON	= 34;			// Default
	ddr.RON_A[0]	= 40;		// 1333 1-rank
	ddr.RON_A[1]	= 34;		// 1333 2-rank
	ddr.RON_A[2]	= 34;		// 1333 4-rank
	ddr.RON_A[3]	= 40;		// 1600 1-rank
	ddr.RON_A[4]	= 34;		// 1600 2-rank

	ddr.RTT_WR	= 0;
	ddr.TIS_SHIFT	= 0x20;

	ddr.DQSGATE_DELAY	= 0x0005;
	ddr.DQSGATE_GB	= 1;
	ddr.VDD		= 1.35;
	ddr.VREG_CNTL0	= 0x008C | ( (ddr.VDD > 1.35? 0x0 : (ddr.VDD > 1.25? 0x1 : 0x3)) << 5 );
	ddr.RDDLY_CFG0	= 0x0404;	// 4003	-- changed from 0x0400 [Prism RDCAL fail]
	ddr.RDDLY_CFG1	= 0x801F;	// 5003
	ddr.RDCAL_CNTLF	= 0xE002	// 6FFF
			| _B1(52,ddr.DQSGATE_GB>0?1:0)
			| _B3(55,ddr.DQSGATE_GB>0? ddr.DQSGATE_GB : -ddr.DQSGATE_GB);
	ddr.RDCAL_CNTL6	= 0x88DC;	// 6FF6	-- changed from 0xA8DC (sticky valid mode) [HW166433] 06/18/2011
	ddr.RDCAL_CNTL3	= 0x000C;	// 6FF3
	ddr.RDCAL_CNTL9	= 0x8788;	// 6FF9
	ddr.RDCAL_CNTLB	= 0x0781;	// 6FFB
	ddr.RDCAL_CNTLD	= 0x0080;	// 6FFD

	ddr.VREF_CA	= (ddr.VDD==1.5 || TI_isDD1())? 0x10:0x10;
	ddr.VREF_DQ	= (ddr.VDD==1.5 || TI_isDD1())? 0x18:0x18;
	ddr.VREF_INT	= (ddr.VDD==1.5 || TI_isDD1())?  0x1: 0x0;
	ddr.RANK_MAP_MODE	= TI_isDD1()?2:0;

	ddr.REFRESH_RATE	= PERS_ENABLED(PERS_ENABLE_DiagnosticsMode)?1.0:1.1;
	ddr.REFRESH_SCOM_DELAY	= 0;
	ddr.HIGH_PRIORITY_REFRESH = 1;	// Enabled to avoid burst Refreshes
	ddr.WRITE_QUEUE_PRIORITY = TI_isDD1()?3:0;
	ddr.MEMCAL_PERIOD	= 128;	// 128 * 2ms = 256ms	-- 01/30/2012	; value greater than 128 is divided by 128 (MCACFG1 bit17 doesn't work)
	ddr.MEMCAL_OPER		= 8;	// 8 * 128tCK = 1024tCK	-- 01/30/2012
	ddr.MEMCAL_MPR_DISABLE	= 1;
	ddr.MEMCAL_DATACPC	= 0;
	ddr.MEMCAL_ADDRCPC	= 0;
	ddr.MEMCAL_DESKEW	= 1;
	ddr.MEMCAL_RDDLY	= 1;
	ddr.BG_SCRUB_RATE	= 7;	// n = 0 - 4095 (interval = 8 mins * (n+1) in 2Gb 2-ranks
	ddr.PD_DISABLE		= DDR_GET_POWER_DOWN(FW_Personality.DDR_Config.DDRFlags) == DDR_POWER_DOWN_OFF ? 1:(TI_isDD1()?1:0);
	ddr.PD_FAST		= DDR_GET_POWER_DOWN(FW_Personality.DDR_Config.DDRFlags) == DDR_POWER_DOWN_FAST ? 1:0;

	ddr.SKIP_ALLCAL		= 0;
	ddr.SKIP_RDCAL		= 0;
	ddr.SKIP_CWL		= 0;
	ddr.SKIP_ZQCAL		= 0;
	ddr.SKIP_MEMCAL		= ddr.PD_DISABLE?0:1;
	ddr.SKIP_MCZMRINT	= 0;
	ddr.SKIP_SMBTEST	= 0;
	ddr.SKIP_WRCAL		= TI_isDD1()?1:(PERS_ENABLED(PERS_ENABLE_DiagnosticsMode)?1:0);
	ddr.SKIP_ADDRCAL	= TI_isDD1()?1:(PERS_ENABLED(PERS_ENABLE_DiagnosticsMode)?1:0);
	ddr.ADDR_DELAY		= ddr.SKIP_ADDRCAL? 0x80:0x100;
	ddr.ADDR_DELAY_START	= ddr.SKIP_ADDRCAL? 0x80:0x100;
	ddr.SKIP_MEMBIST	= 1;
	ddr.SKIP_FASTINIT	= 0;
	ddr.SKIP_OT_SCRUB	= PERS_ENABLED(PERS_ENABLE_DDRCellTest)?0:1;
	ddr.DRAM_ECC_DISABLE	= 0;
	ddr.PBUS_ECC_DISABLE	= 0;
	ddr.DATA_INV_DISABLE	= 0;
        ddr.BG_SCRUB_DISABLE	= (TI_isDD1() || (PERS_ENABLED(PERS_ENABLE_DDRBackScrub)==0))?1:0;
	ddr.ADDR_HASH_DISABLE	= TI_isDD1()?1:0;
	ddr.FAST		= 0;

	ddr.RANK_AUTO_DETECT	= 1;
	ddr.DENSITY_AUTO_DETECT	= 1;

#undef	ddrMAX
#undef	ddrMIN
