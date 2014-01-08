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
struct DDRINIT_metrics {
	unsigned	CL, AL, CWL, GLB_OFS;
	unsigned	WLAT, RLAT, IOM_WL, IOM_RL;
	unsigned	TRRD, TWR, TFAW, TRC;
	unsigned	TXP, TXPDLL, TWRAPDEN;
	unsigned	RR_S, WW_S, RW_S, WR_S;
	unsigned	RR_D, WW_D, RW_D, WR_D;
	unsigned	ADDR_DELAY, ADDR_DELAY_START, TIS_SHIFT;
	unsigned	DQSGATE_DELAY;
	int		DQSGATE_GB;
	uint64_t	VREG_CNTL0;
	uint64_t	RDDLY_CFG0, RDDLY_CFG1;
	uint64_t	RDCAL_CNTLF, RDCAL_CNTL6, RDCAL_CNTL3; 
	uint64_t	RDCAL_CNTL9, RDCAL_CNTLB, RDCAL_CNTLD; 
	uint64_t	IMPEDANCE, IMP_A[5];
	unsigned	RON, RON_A[5];
	unsigned	RTT, RTT_A[5];
	unsigned	RTT_WR;
	unsigned	VREF_CA, VREF_DQ, VREF_INT;
	unsigned	BG_SCRUB_RATE;
	unsigned	MEMCAL_PERIOD;
	unsigned	MEMCAL_OPER;
	unsigned	REFRESH_SCOM_DELAY;
	float		REFRESH_RATE;
	float		TCK;
	float		VDD;
	char		MEMCAL_MPR_DISABLE;
	char		MEMCAL_DATACPC;
	char		MEMCAL_ADDRCPC;
	char		MEMCAL_DESKEW;
	char		MEMCAL_RDDLY;
	unsigned	MRKCHIP[2][4];
	char		FailDQ[2][4][18];
	char		HIGH_PRIORITY_REFRESH;
	char		RANK_MAP_MODE;
	char		WRITE_QUEUE_PRIORITY;
	char		PD_DISABLE;
	char		PD_FAST;
	char		SKIP_ALLCAL;
	char		SKIP_RDCAL;
	char		SKIP_CWL;
	char		SKIP_ZQCAL;
	char		SKIP_MEMCAL;
	char		SKIP_MCZMRINT;
	char		SKIP_SMBTEST;
	char		SKIP_WRCAL;
	char		SKIP_ADDRCAL;
	char		SKIP_MEMBIST;
	char		SKIP_FASTINIT;
	char		SKIP_OT_SCRUB;
	char		DRAM_ECC_DISABLE;
	char		PBUS_ECC_DISABLE;
	char		DATA_INV_DISABLE;
	char		BG_SCRUB_DISABLE;
	char		ADDR_HASH_DISABLE;
	char		FAST;
	char		RANK_AUTO_DETECT;
	char		DENSITY_AUTO_DETECT;
	};
