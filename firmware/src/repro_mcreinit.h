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
#include <firmware/include/fwext/fwext_lib.h>
#include <hwi/include/bqc/testint_dcr.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/dc_arbiter_dcr.h>
#include <hwi/include/bqc/dr_arb_dcr.h>
#include <hwi/include/bqc/ddr.h>
#include <hwi/include/common/bgq_bitnumbers.h>


struct SCOM_adr_val_t{unsigned addr; char rw; uint64_t pattern; char title[100]; uint64_t value;};

static struct SCOM_adr_val_t ddr_seq1[]=
{
	{_DDR_MC_MCFGP(0),0,0,
	"MCFGP:	Primary Configuration - Memory size = 16*512MB",
	_B10(9, 8*2)
	},

	{_DDR_MC_MCFGC0(0),0,0,
	"MCFGC0:	Configuring channel 0",
	// size in 512MB
	_B10(8, 8*2) |
	// Row/Col/Bnk 15/10/3
	_B4(12, 4) |
	// Rank 0 active
	_BN(13) |
	// Rank 1 active
	_BN(14)
	},


	{_DDR_MC_MCBCFG(0),0,0,
	"MCBCFG:	Read data = 4",
	// number of read data credits
	_B3(2,4)
	},

	{_DDR_MC_MCMISMODE(0),0,0,
	"MCMISMODE:	Memory size in channel 0 (Row/Col/Bank = 15/10/3)",
	_B4(7, 4)
	},


	{_DDR_MC_MCADTAL(0),0,0,
	"MCADTAL:	WLAT=2 RLAT=8",
	_B8(7,2) |	// Write latency
	_B8(15,8)	// Read latency
	},

	{_DDR_MC_MCTCNTL(0),0,0,
	"MCTCNTL:	Setting DDR timing parameters",
	_B5(4,4-4) |	// tRRD
	_B5(9,20-4) |	// tFAW
	_B6(15,33-4) |	// tRC
	_B6(21,33-4) |	// W_AS
	_B6(27,33-4) |	// R_AS
	_B5(32,6-4) | 	// W_WD
	_B5(37,24-4) |	// W_RS
	_B5(42,4-4) |	// W_RD
	_B5(47,6-4) |	// R_RD
	_B5(52,8-4) |	// R_WS
	_B5(57,8-4)	// R_WD
	},

	{_DDR_MC_MCACFG0(0),0,0,
	"MCACFG0:	Ranks 0 and 1 on dimm 0 ch 0 present",
	_B8(7,0xc0)
	//"MCACFG0:	Ranks 0 on dimm 0 ch 0 present",
	//_B8(7,0x80)
	},


	{_DDR_MC_MCAMISC(0),0,0,
	"MCAMISC:	R_RS, W_WS, NUM_IDLES",
	_B5(24,4-4) |	// R_RS
	_B5(29,4-4) |	// W_WS
	_B7(43,0)|	// NUM_IDLES -> if > 0, set also MCACFG1<0:1>=2
	// _B7(43,128-2)|	// NUM_IDLES -> if > 0, set also MCACFG1<0:1>=2
	_B9(52,256)	// NUM_CMDS_128_WINDOW
	},

	{_DDR_MC_IOM_PHYREGINDEX(0),0,0,
	"PHYWRITE:	0xEFFD = 0x00FF: configure x8 for all ranks",
	_B16(47,0xEFFD) |
	_B16(63,0x04FF)	// CCF170
	},

	{_DDR_MC_IOM_PHYREGINDEX(0) + 0x40,0,0,
	"PHYWRITE:	0xEFFD = 0x00FF: configure x8 for all ranks",
	_B16(47,0xEFFD) |
	_B16(63,0x04FF)	// CCF170
	},

	{_DDR_MC_IOM_PHYREGINDEX(0)+0x0A,0,0,
	"FIFO_CNTL:	GLB_OFS=2, IOM_WL=15, IOM_RL=17",
	_BN(44) |
	_B3(47,2) |
	_B5(55,15) |
	_B5(63,17)
	},

	{_DDR_MC_IOM_PHYREGINDEX(0)+0x4A,0,0,
	"FIFO_CNTL:	GLB_OFS=2, IOM_WL=15, IOM_RL=17",
	_BN(44) |
	_B3(47,2) |
	_B5(55,15) |
	_B5(63,17)
	},

	{_DDR_MC_IOM_PHYREGINDEX(0),0,0,
	"PHYWRITE:	0x7FF0 = 0x0002: DQS GATE delay set",
	_B16(47,0x7FF0) |
	_B16(63,0x0002)
	},

	{_DDR_MC_IOM_PHYREGINDEX(0) + 0x40,0,0,
	"PHYWRITE:	0x7FF0 = 0x0002: DQS GATE delay set",
	_B16(47,0x7FF0) |
	_B16(63,0x0002)
	},

	// clocks on
	// wait 500us
	//{_DDR_MC_MCAPOS(0),0,0,
	//"MCAPOS:	CKE enabled",
	//_B4(34,0xF) |
	//_BN(35) |
	//_BN(36) |
	//_BN(37)
	//},
};
