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
#include <hwi/include/bqc/dc_arbiter_dcr.h>

#define MAX_NUM_RANKS	4
#define MAX_DENSITY	4096
#define MIN_DENSITY	1024
#define	WAIT_CAL_CPC	20000	//   8,400
#define	WAIT_CAL_FWL	40000	//  12,500 (pass) / 22,700 (fail)
#define	WAIT_CAL_RDCAL	400000	// 154,000/2 = 77,000
#define	CKE_PAT	0xF	// 0xA for 1 rank, 0xF for 2 & 4 ranks
#define	CSB_PAT	0xFF
#define IOM_RANK_RRR(x)	(x)
#define MC_RANK_RRR(x)	((x & 1) << 2 | (x & 2) | (x & 4) >> 2)
#define MC_Mapped_Rank(x)	( TI_isDD1()? ((x & 1) << 1 | (x & 2) >> 1) : x )
#define	HARD_FAILURE	0
#define	HARD_SKEW	0x20
#define	DELAY_OVERRIDE	0


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*  Step 2. Detect Frequency */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

void DDR_Init_Freq(unsigned *dram_speed)
{
	uint64_t val;
	unsigned proc_freq=1600;

//	if ( PERS_ENABLED(PERS_ENABLE_DDRCalibration) )
//	{
//		DCRWritePriv(_DDR_MC_MCAPERFMONC1(0), _B5(36,0));
//		DCRWritePriv(_DDR_MC_MCAPERFMONC0(0), _BN(32) | _B8(63,0x80));
//		DelayTimeBase(160000000);       
//		val=DCRReadPriv(_DDR_MC_MCAPERFMON0(0));
//		MC_DEBUG(("MC[%d]: DRAM TICK   = 0x%016lx (%lu)\n",0,val,val));
//		MC_DEBUG(("DRAM CLK frequency = %uMHz",(unsigned)val/100000 + (val%100000 > 50000? 1:0)));
//		if(val > 68000000)	// Guardband = +2%
//		{
//			MC_DEBUG(("DDR3 1600Mbps\n"));
//			*dram_speed = 1600;
//		}
//		else
//		{
//			MC_DEBUG(("DDR3 1333Mbps\n"));
//			*dram_speed = 1333;
//		}
//	}
//	else
//		*dram_speed = 1333;

	val=DCRReadPriv(TESTINT_DCR(CLKTREE_CTRL18));
	MC_DEBUG(("CLKTREE_CTRL18 = 0x%lx\n",val));

	switch((val >> (63-13)) & 0x3)
	{
	case 0 /* 0b00 */ :	proc_freq = 1600; break;
	case 1 /* 0b01 */ :	proc_freq =  800; break;
	case 2 /* 0b10 */ :	proc_freq = 1200; break;
	case 3 /* 0b11 */ :	proc_freq = 2000; break;
	}
	MC_DEBUG(("proc_freq = %uMHz\n",proc_freq));

	if ( PERS_ENABLED(PERS_ENABLE_Simulation) )
		proc_freq = 1600;

	val=DCRReadPriv(TESTINT_DCR(CLKTREE_CTRL15));
	MC_DEBUG(("CLKTREE_CTRL15 = 0x%lx\n",val));

	if((val >> (63-36)) & 0x1)
		*dram_speed = proc_freq;
	else
		*dram_speed = (2*proc_freq*5/6+1)/2;
	MC_DEBUG(("ddr_speed = %uMbps\n",*dram_speed));
}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*  Step 3. Configure Misc  */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

void DDR_Init_Misc(unsigned num_ranks, unsigned *dram_density, struct DDRINIT_metrics *ddr)
{
	int i;
	uint64_t val;


	if ( !PERS_ENABLED(PERS_ENABLE_DDRCalibration) )
	{
		ddr->SKIP_ALLCAL = 1;
		ddr->SKIP_RDCAL = 1;
		ddr->SKIP_CWL = 1;
		ddr->SKIP_MEMCAL = 1;
		ddr->FAST = 1;
	}
	if ( !PERS_ENABLED(PERS_ENABLE_DDRFastInit) )
	{
		ddr->SKIP_SMBTEST = 1;
		ddr->SKIP_MEMBIST = 1;
		ddr->SKIP_FASTINIT = 1;
		ddr->SKIP_OT_SCRUB = 1;
		ddr->BG_SCRUB_DISABLE = 1;
	}
	if ( PERS_ENABLED(PERS_ENABLE_FPGA) )
	{
		MC_DEBUG(("PERS_ENABLE_FPGA detected. Forcing DRAM density to be 512Mb\n"));
		*dram_density = 512;
		ddr->ADDR_HASH_DISABLE = 1;
	}
	if ( PERS_ENABLED(PERS_ENABLE_Simulation) )
	{
		ddr->PD_DISABLE = 1;	// disabling power-down in cycle simulation to avoid assertion failure found by Timothy Moe. 02/26/2011 -KH
	}

	DCRWritePriv(DC_ARBITER_DCR(ARB_CTRL), 0x0000000006010007);

	switch(num_ranks * (*dram_density))
	{
		case 512  : val = 5; break;
		case 1024 : val = 4; break;
		case 2048 : val = 3; break;
		case 4096 : val = 2; break;
		case 8192 : val = 1; break;
		case 16384: val = 0; break;
		default: MC_DEBUG(("Invalid Memory Capacity\n")); return;
	}
	for(i=0;i<2;i++)
		DCRWritePriv(DR_ARB_DCR(i,MAP_ADDRESS), DR_ARB_DCR__MAP_ADDRESS__RANK_BIT_SEL_CFG_set(val));

#if FW_DEBUG
	MC_DEBUG(("MISC: num_ranks = %d    dram_density = %dMb\n",num_ranks,*dram_density));
#endif
}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*  Step 4. Load MCS registers  */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

void DDR_Init_MCS(unsigned num_ranks, unsigned dram_density)
{
	int i;
	uint64_t val;


	val = _B9(8, 4*dram_density*num_ranks/1024);
	switch(dram_density)
	{
		case 512  :	val |= _B4(12, 0); break;	// 512Mb x8
		case 1024 :	val |= _B4(12, 2); break;	//   1Gb x8
		case 2048 :	val |= _B4(12, 4); break;	//   2Gb x8
		case 4096 :	val |= _B4(12, 6); break;	//   4Gb x8
		case 8192 :	val |= _B4(12, 7); break;	//   8Gb x8
		default: MC_DEBUG(("Unknown DRAM density\n")); return;
	}
	switch(num_ranks)
	{
		case 1: val |= _B8(20, 0x80); break;
		case 2: val |= _B8(20, 0xC0); break;
		case 4: val |= _B8(20, 0xF0); break;
		case 8: val |= _B8(20, 0xFF); break;
		default: MC_DEBUG(("Invalid Rank Numbers\n")); return;
	}

	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCMCC(i), 0);	// Make sure there is no on-going maintenance operation (e.g. scrubbing)

	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCFGP(i), 0);	// For DD2 and further passes, when changing MCFGC0 or MCFGC1, MCFGP should be set to zero first

	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCFGC0(i), val);

	//for(i=0;i<2;i++)
	//	DCRWritePriv(_DDR_MC_MCFGC1(i), 0);

	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCFGP(i), _B10(9, 4*dram_density*num_ranks/1024));	// For DD2 and further passes, MCFGC0 and MCFGC1 must be loaded before loading MCFGP

	//for(i=0;i<2;i++)
	//	DCRWritePriv(_DDR_MC_MCBCFG(i), 0x8000000000000000ULL);

	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCTL(i), 0x1800080000002000ULL);	// issue 1488

#if FW_DEBUG
	MC_DEBUG(("MCS : num_ranks = %d    dram_density = %dMb\n",num_ranks,dram_density));
	MC_DEBUG(("MCFGP     (0x%x) = 0x%lx\n",_DDR_MC_MCFGP(0),DCRReadPriv(_DDR_MC_MCFGP(0))));
	MC_DEBUG(("MCFGC0    (0x%x) = 0x%lx\n",_DDR_MC_MCFGC0(0),DCRReadPriv(_DDR_MC_MCFGC0(0))));
#endif

}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*  Step 5. Load MCA registers  */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

void DDR_Init_MCA(unsigned num_ranks, unsigned dram_density, struct DDRINIT_metrics *ddr, unsigned *ODT)
{
	int i;
	uint64_t val=0;


	switch(dram_density)
	{
		case 512  :	val = _B4(7, 0); break;	// 512Mb x8
		case 1024 :	val = _B4(7, 2); break;	//   1Gb x8
		case 2048 :	val = _B4(7, 4); break;	//   2Gb x8
		case 4096 :	val = _B4(7, 6); break;	//   4Gb x8
		case 8192 :	val = _B4(7, 7); break;	//   8Gb x8
	}
	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCMISMODE(i), val);

if(TI_isDD1())
	val =	_B8(7,ddr->WLAT) |
		_B8(15,ddr->RLAT) |
		_B8(31,0);
else
	val =	_B8(7,ddr->WLAT) |
		_B8(15,ddr->RLAT) |
		_B8(23,ddr->ADDR_HASH_DISABLE? 0:ddr->WLAT+8) |		// For DD2 and later, address hashing should be disabled at this stage, MCADTAL(16:23)=0x00
		_B8(31,ddr->RLAT+2);

	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCADTAL(i), val);


	val =	_B5(4,ddr->TRRD-4) |	// tRRD
		_B5(9,ddr->TFAW-4) |	// tFAW
		_B6(15,ddr->TRC-4) |	// tRC
		_B6(21,ddr->TRC-4) |	// W_AS
		_B6(27,ddr->TRC-4) |	// R_AS
		_B5(32,ddr->WW_D-4) |	// W_WD
		_B5(37,ddr->WR_S-4) |	// W_RS
		_B5(42,ddr->WR_D-4) |	// W_RD
		_B5(47,ddr->RR_D-4) |	// R_RD
		_B5(52,ddr->RW_S-4) |	// R_WS
		_B5(57,ddr->RW_D-4);	// R_WD
	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCTCNTL(i), val);


	val =	_B8(19,0xFF) |		// PAGE MODE STALL
	 	_B5(24,ddr->RR_S-4) |	// R_RS
		_B5(29,ddr->WW_S-4) |	// W_WS
		_B2(33,ddr->CL-ddr->CWL) |	// RWLAT_DELTA
		_B7(43,0) |		// NUM_IDLES
		_B9(52,256);		// NUM_CMDS_128_WINDOW
	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCAMISC(i), val);


//	The below was based on the description in the workbook which was incorrect.
//	-------------------------------------
//	val =	_B3(2,0x4) |		// RAS#/CAS#/WE# = 0(2)/1(2)/1(2) (compensated by CS# = 1(8) & BA = 0(6) & ODT_B = 0(4))
//		_B4(6,0x0) |		// ODT = 0(8) (compensated by CKE = 1(4))
//		_B3(9,0x0) |		// BA[0:2]
//		_B16(25,0xF0F0) |	// ADDR[0:15] (for 2Gb, Addr[0:14])
//		_BN(26);		// ISC Enabled
//	-------------------------------------
//	New estimation:
//	-------------------------------------
//	CS<0:3> = H		(total 8 'H')
//	RAS#/CAS#/WE# = H/H/H	(total 6 'H')
//	ODT<0:3> = L		(total 8 'L')
//	CKE<0:1> = L		(total 4 'L')
//	-------------------------------------
//	SUM			(total 2 'H')
//	-------------------------------------
//
	val =	_B16(15,0x5555) |	// ADDR[0:15]
		_B3(18,0x2) |		// BA[0:2]
		_BN(19) |		// ISC Enabled
		_B44(63,0x55555555555);
	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCAISC(i), val);


	switch(num_ranks)
	{
		case 1: val = _B8(7, 0x80); break;
		case 2: val = _B8(7, 0xC0); break;
		case 4: val = _B8(7, 0xF0); break;
		case 8: val = _B8(7, 0xFF); break;
		default: MC_DEBUG(("Invalid Rank Numbers\n")); return;
	}
	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCACFG0(i), val);


	val =	_B2(1,ddr->WRITE_QUEUE_PRIORITY) |
		_B1(9,ddr->HIGH_PRIORITY_REFRESH) |
		_B2(12,ddr->HIGH_PRIORITY_REFRESH? 3:0) |		// Outstanding Refresh count				-- set to 1 if high_priority refresh
		_BN(13) |						// Don't Accumulate Refreshes During CAL		-- enabled to avoid burst refresh
		_B1(17,ddr->MEMCAL_PERIOD>128? 1:0) |			// 01-30-2012
	//	_BN(20) |						// Take account missed Refresh due to calibration	-- disabled to avoid burst refresh
		_B1(21,ddr->HIGH_PRIORITY_REFRESH) |			// 01-30-2012
		_B1(24,1) |						// Disable Power Down Mode for initialization sequence
		_BN(26) |						// Write Queue Priority Enhanced Mode
		_B5(31,(ddr->PD_FAST?ddr->TXP:ddr->TXPDLL) - 4) |	// Power down exit time in mca clock (= DRAM clock)
		_B32(63,ddr->TWRAPDEN);					// Last CMD to CKE falling in 2x clk
	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCACFG1(i), val);


	switch(num_ranks)
	{
#if 1
		case 1:	val	= _B8(39,0x80)|	// ODT0 enabled when writing to rank0
				  _B8(47,0x00)|	// ODT1 disabled always
				  _B8(55,0x80)|	// ODT2 enabled when writing to rank0
				  _B8(63,0x00);	// ODT3 disabled always

  		        ODT[0]	= 8 /* 0b1000 */ ;	// bit3=ODT0, bit2=ODT1, bit1=ODT2, bit2=ODT3 when calibrating(writing) to rank0
			ODT[1]	= 0;
			ODT[2]	= 0;
			ODT[3]	= 0;
			break;

		case 2:	val	= _B8(39,0x40)|	// ODT0 enabled when writing to rank1
				  _B8(47,0x80)|	// ODT1 enabled when writing to rank0
				  _B8(55,0x40)|	// ODT2 enabled when writing to rank1
				  _B8(63,0x80);	// ODT3 enabled when writing to rank0

		        ODT[0]	= 5  /*0b0101*/; // bit3=ODT0, bit2=ODT1, bit1=ODT2, bit2=ODT3 when calibrating(writing) to rank0
			ODT[1]	= 10 /*0b1010*/; // bit3=ODT0, bit2=ODT1, bit1=ODT2, bit2=ODT3 when calibrating(writing) to rank1
			ODT[2]	= 0;
			ODT[3]	= 0;
			break;

		case 4:	val	= _B8(39,0x50)|	// ODT0 enabled when writing to rank1 or rank 3
				  _B8(47,0xA0)|	// ODT1 enabled when writing to rank0 or rank 2
				  _B8(55,0x50)|	// ODT2 enabled when writing to rank1 or rank 3
				  _B8(63,0xA0);	// ODT3 enabled when writing to rank0 or rank 2

		        ODT[0]	= 5   /*0b0101*/; // bit3=ODT0, bit2=ODT1, bit1=ODT2, bit2=ODT3 when calibrating(writing) to rank0
			ODT[1]	= 10  /*0b1010*/; // bit3=ODT0, bit2=ODT1, bit1=ODT2, bit2=ODT3 when calibrating(writing) to rank1
			ODT[2]	= 5   /*0b0101*/; // bit3=ODT0, bit2=ODT1, bit1=ODT2, bit2=ODT3 when calibrating(writing) to rank2
			ODT[3]	= 10  /*0b1010*/; // bit3=ODT0, bit2=ODT1, bit1=ODT2, bit2=ODT3 when calibrating(writing) to rank3
			break;
#endif
		default: val	= 0;
			ODT[0]	= 0;
			ODT[1]	= 0;
			ODT[2]	= 0;
			ODT[3]	= 0;
	}
	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCAODT0(i), val);


	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCECCDIS(i), _B5(9,0x3F) | _BN(27) | _BN(29));	// Disable all ECC before calibrating


	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCFIRM(i), _B3(37,7));	// Mask MNT ECC FIR bit before calibrating

#if FW_DEBUG
	MC_DEBUG(("MCA : num_ranks = %d    dram_density = %dMb\n",num_ranks,dram_density));
	MC_DEBUG(("MCMISMODE (0x%x) = 0x%lx\n",_DDR_MC_MCMISMODE(0),DCRReadPriv(_DDR_MC_MCMISMODE(0))));
	MC_DEBUG(("MCACFG0   (0x%x) = 0x%lx\n",_DDR_MC_MCACFG0(0),DCRReadPriv(_DDR_MC_MCACFG0(0))));
	MC_DEBUG(("CL=%d CWL=%d AL=%d RL=%d TRRD=%d TWR=%d TFAW=%d TRC=%d\n",ddr->CL,ddr->CWL,ddr->AL,ddr->IOM_RL,ddr->TRRD,ddr->TWR,ddr->TFAW,ddr->TRC));
#endif
	if(ddr->TWR != 5 && ddr->TWR != 6 && ddr->TWR != 7 && ddr->TWR != 8 && ddr->TWR != 10 && ddr->TWR != 12 && ddr->TWR != 14 && ddr->TWR != 16)
		MC_DEBUG(("Invalid tWR [%d]\n",ddr->TWR));

}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*  Step 6. Load IOM registers  */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

void DDR_Init_IOM(struct DDRINIT_metrics *ddr, unsigned *MC_IOM, int repro)
{
	int i, NN, chip;//, loop;
	uint64_t val=0;


#if FW_DEBUG
	MC_DEBUG(("IOM reset\n\n"));
#endif
	for(i=0; i<4 ;i++)
		DCRWritePriv(_DDR_MC_IOM_FIFO_CNTL(0) + MC_IOM[i], _BN(32));	// IOM reset
	for(i=0; i<4 ;i++)
		DCRWritePriv(_DDR_MC_IOM_FIFO_CNTL(0) + MC_IOM[i], 0);		// IOM reset release


#if FW_DEBUG
	MC_DEBUG(("ADR Vreg Control Register 0\n\n"));
#endif
	for(i=0; i<4 ;i++)
		for(NN=0; NN<3 ;NN++)
			DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B8(39,NN) | _B16(47,0x0010) | _B8(63,ddr->VREG_CNTL0));	// changing from 0x8010  -- HW157760


#if FW_DEBUG
	MC_DEBUG(("WRLVL Vreg Control Register 0\n\n"));
#endif
	for(i=0; i<4 ;i++)
		for(chip=0; chip<9; chip++)
			DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B4(42,chip) | _B16(47,0x8400) | _B8(63,ddr->VREG_CNTL0));


#if FW_DEBUG
	MC_DEBUG(("Enable Vref drivers\n\n"));
#endif
	for(i=0; i<4 ;i++)
	{
		DCRWritePriv(_DDR_MC_IOM_VREF_CNTL(0) + MC_IOM[i], _BN(50) | _B5(55,ddr->VREF_CA) | _BN(58) | _B5(63,ddr->VREF_DQ));	// Enable VREF drivers
		DCRWritePriv(_DDR_MC_IOM_INT_VREF(0) + MC_IOM[i], _B5(63,ddr->VREF_INT));	// INT_VREF
	}


#if FW_DEBUG
	MC_DEBUG(("Master Control Config Register 0\n\n"));
#endif
	for(i=0; i<4 ;i++)
		DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B16(47,0xEFFD) | _B1(49,ddr->VDD<1.5? 1:0) | _B2(54,ddr->RANK_MAP_MODE) | _B8(63,0xFF));	// MASTER_CNTL_CONFIG_0 - set x8 mode


#if FW_DEBUG
	MC_DEBUG(("FIFO Control Register\n\n"));
#endif
	val =	_BN(44) |		// Use GLB_OFS
		_B3(47,ddr->GLB_OFS) |	// GLB_OFS = 2
		_B5(55,ddr->IOM_WL) |	// IOM_WL = CWL + AL = 7 + 8 = 15
		_B5(63,ddr->IOM_RL); 	// IOM_RL = CL + AL = 9 + 8 = 17
	for(i=0; i<4 ;i++)
		DCRWritePriv(_DDR_MC_IOM_FIFO_CNTL(0) + MC_IOM[i], val);


#if FW_DEBUG
	MC_DEBUG(("Disable ADDR driver PVT update\n\n"));
#endif
	for(i=0; i<4 ;i++)
		for(NN=0; NN<3 ;NN++)
			DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B8(39,NN) | _B8(47,0xFF) | _B16(63,TI_isDD1()? 0x0007:0x0));	// Disable ADDR driver PVT update


#if FW_DEBUG
	MC_DEBUG(("Reset Active\n\n"));
#endif
	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCAPOS(i), _B22(21, 0x155555) | _BN(36) | _BN(37));		// Reset Active (bit35 = 0), CMD bus enabled (bit36), send to Ch0 (bit37)
	if(!repro)
		do
			val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
		while(val);


#if FW_DEBUG
	MC_DEBUG(("IO_Impedance set, AutoCal off\n\n"));
#endif
	for(i=0; i<4 ;i++)
		DCRWritePriv(_DDR_MC_IOM_IO_IMPEDANCE(0) + MC_IOM[i], _B1(51, ddr->VDD < 1.5? 1:0) | ddr->IMPEDANCE | 0x0);

	Delay_ns(ddr->FAST?1000:1000000, 1000);

#if FW_DEBUG
	MC_DEBUG(("IO_Impedance set, AutoCal on\n\n"));
#endif
	for(i=0; i<4 ;i++)
		DCRWritePriv(_DDR_MC_IOM_IO_IMPEDANCE(0) + MC_IOM[i], _B1(51, ddr->VDD < 1.5? 1:0) | ddr->IMPEDANCE | 0x888);

	Delay_ns(ddr->FAST?1000:1000000, 1000);


if(TI_isDD1())
{
#if FW_DEBUG
	MC_DEBUG(("IO_Impedance set, AutoCal off\n\n"));
#endif
	for(i=0; i<4 ;i++)
		DCRWritePriv(_DDR_MC_IOM_IO_IMPEDANCE(0) + MC_IOM[i], _B1(51, ddr->VDD < 1.5? 1:0) | ddr->IMPEDANCE | 0x0);
}


else
{
//	for(loop=0; loop<32; loop++)
//	{
//#if FW_DEBUG
//		MC_DEBUG(("Loop %2d: IO_Impedance set, AutoCal off\n", loop));
//#endif
//		for(i=0; i<4 ;i++)
//			DCRWritePriv(_DDR_MC_IOM_IO_IMPEDANCE(0) + MC_IOM[i], _B1(51, ddr->VDD < 1.5? 1:0) | ddr->IMPEDANCE | 0x0);
//
//#if FW_DEBUG
//		MC_DEBUG(("Loop %2d: IO_Impedance set, AutoCal on\n", loop));
//#endif
//		for(i=0; i<4 ;i++)
//			DCRWritePriv(_DDR_MC_IOM_IO_IMPEDANCE(0) + MC_IOM[i], _B1(51, ddr->VDD < 1.5? 1:0) | ddr->IMPEDANCE | 0x800);
//	}

#if FW_DEBUG
	MC_DEBUG(("IO_Impedance set, AutoCal on\n\n"));
#endif
	for(i=0; i<4 ;i++)
	//	DCRWritePriv(_DDR_MC_IOM_IO_IMPEDANCE(0) + MC_IOM[i], _B1(51, ddr->VDD < 1.5? 1:0) | ddr->IMPEDANCE | 0x888);
		DCRWritePriv(_DDR_MC_IOM_IO_IMPEDANCE(0) + MC_IOM[i], _B1(51, ddr->VDD < 1.5? 1:0) | ddr->IMPEDANCE | 0x088);
}


	for(i=0; i<4 ;i++)
	{
		val = DCRReadPriv(_DDR_MC_IOM_IOM_HW_ERR(0) + MC_IOM[i]);
		if(val!=0)
		{
#if FW_DEBUG
	MC_DEBUG(("MC[%d]_SAT[%d] IOM_HW_ERR=0x%016lx -> Clearing\n",i%2,i/2,val));
#endif
			DCRWritePriv(_DDR_MC_IOM_IOM_HW_ERR(0) + MC_IOM[i], val);
			val = DCRReadPriv(_DDR_MC_IOM_IOM_HW_ERR(0) + MC_IOM[i]);
#if FW_DEBUG
	MC_DEBUG(("MC[%d]_SAT[%d] IOM_HW_ERR=0x%016lx\n",i%2,i/2,val));
#endif
		}
	}


}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*  Step 7. Adjust ADDR Delay  */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

void DDR_Init_ADDR_Delay(unsigned *MC_IOM, unsigned ADDR_DELAY, unsigned TIS_SHIFT, int repro)
{
	int i, NN, bbbb;
	uint64_t val=0, val2;


#if FW_DEBUG
	MC_DEBUG(("Address bit delay setting\n\n"));
#endif
/*
	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCAPOS(i), _B19(18, 0x55555) | _BN(19) | _BN(20) | _B8(30,CSB_PAT) | _BN(35) | _BN(36) | _BN(37));	// Enter Self-Refresh
	if(!repro)
		do
			val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
		while(val);
*/


	for(i=0; i<4 ;i++)	// ADDR bit Delay setting
	{
		for(NN=0; NN<3 ;NN++)
		{
			DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B8(39,NN) | _B8(47,0xFF));			// PVT compensated mode	-- 01/30/2012
		//	DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B8(39,NN) | _B8(47,0xFF) | _B3(63,0x7));	// Raw uncompensated mode

		//	for(bbbb=0; bbbb < (NN<2? 16:12) ;bbbb++)
			for(bbbb=0; bbbb < 16 ;bbbb++)
			{
#if FW_DEBUG & 0
				DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B8(39,NN) | _B8(47,bbbb));
				val = 0x1FF & DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i]);
				if(val!=0)
					MC_DEBUG(("This ADDR (MC[%d]_SAT[%d], NN=%d, bbbb=%d) has non zero reset value of 0x%016lx (probably CLK)\n",i%2,i/2,NN,bbbb,val));
#endif
				val = _B4(55,i) | _B4(59,NN) | _B4(63,bbbb);
				if(val == 0x000 || val == 0x010 || val == 0x028 || val == 0x100 || val == 0x118 || val == 0x120 || val == 0x200 || val == 0x308 ||
				   val == 0x001 || val == 0x011 || val == 0x029 || val == 0x101 || val == 0x119 || val == 0x121 || val == 0x201 || val == 0x309)
				{
					if(val%2 == 0)
						val2 = (uint64_t)ADDR_DELAY + 0x100;
					else
						val2 = (uint64_t)ADDR_DELAY;
				}
				else
					val2 = (uint64_t)ADDR_DELAY + TIS_SHIFT;
				DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B8(39,NN) | _B8(47,bbbb) | val2%0x200);
			}
		}
	}


/*
	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCAPOS(i), _B22(21, 0x155555) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));     // Exit Self-Refresh
	if(!repro)
		do
			val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
		while(val);

	Delay_ns(300, 265);	// tXS


	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCAPOS(i), _B19(18, 0x55555) | _BN(19) | _BN(21) | _B8(30,CSB_PAT) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// All Banks Precharge
	if(!repro)
		do
			val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
		while(val);
*/

#if FW_DEBUG
	i=0;NN=0;bbbb=1;
	DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B8(39,NN) | _B8(47,bbbb));
	val = 0x1FF & DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i]);
	MC_DEBUG(("MC0 Instance0 ADDR1 Delay = 0x%016lx\n",val));
#endif


}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*  Step 8. Reset DRAM & Assert CKE  */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

void DDR_Init_Reset_DRAM(struct DDRINIT_metrics *ddr)
{
	int i, loop;
	uint64_t val=0;


	for(i=0; i<2 ;i++)
		DCRWritePriv(_DDR_MC_MCAPOS(i), _B22(21, 0x155555) | _BN(36) | _BN(37));	     // RESET = 'L' (bit35 = 0), CMD bus enabled (bit36), send to Ch0 (bit37)
	do
		val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
	while(val);

	Delay_ns(ddr->FAST?1000:200000, 265);

	for(i=0; i<2 ;i++)
		DCRWritePriv(_DDR_MC_MCAPOS(i), _B22(21, 0x155555) | _BN(35) | _BN(36) | _BN(37));   // RESET = 'H'
	do
		val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
	while(val);

	Delay_ns(ddr->FAST?1000:500000, 265);

	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCAPOS(i), _B22(21, 0x155555) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));     // CKE = 'H'
	do
		val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
	while(val);

	Delay_ns(300, 265);

	if ( !PERS_ENABLED(PERS_ENABLE_FPGA) )	// Skip for FPGA: Issue 6190
		for(loop=0; loop<32 ;loop++)	// Clear out TestMode (ELPIDA)
		{
			for(i=0; i<2 ;i++)	// MR0
				DCRWritePriv(_DDR_MC_MCAPOS(i),
				_BN(7) |	// TM
				_BN(19) | _BN(20) | _BN(21) | _B8(30,CSB_PAT) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// common for all MRS
			do
				val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
			while(val);
	
			for(i=0; i<2 ;i++)	// MR0
				DCRWritePriv(_DDR_MC_MCAPOS(i),
				_BN(19) | _BN(20) | _BN(21) | _B8(30,CSB_PAT) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// common for all MRS
			do
				val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
			while(val);
		}
}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*  Step 9. Initial VDL calibration (CPC)  */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

void DDR_Init_CPC(struct DDRINIT_metrics *ddr, unsigned *MC_IOM, int ext_repro)
{
	int i;
	uint64_t val=0, effd=0;
	int recal = ext_repro & 0x2;
	int repro = recal==0? (ext_repro & 0x01) : 0;
	int mc    = recal==0? 0 : (ext_repro & 0x1);
	int step  = recal==0? 1 : 2;


    if(TI_isDD1())
    {
	if(recal!=0)
	{
		DCRWritePriv(_DDR_MC_IOM_PHYREAD(mc), _B16(47,0xEFFD));
		effd = 0xFF00 & DCRReadPriv(_DDR_MC_IOM_PHYREAD(mc));
	}
	else
	{
		effd = _B1(49,ddr->VDD<1.5? 1:0) | _B2(54,ddr->RANK_MAP_MODE);
	}

	for(i=mc; i<4 ;i+=step)
		DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B16(47,0xEFFD) | effd | _B8(63,0x00));	// Rank_Map_Mode & temporary x4 mode for CPC
    }


//long long time0 = GetTimeBase();
	for(i=mc; i<4 ;i+=step)
		DCRWritePriv(_DDR_MC_IOM_CAL_CNTL(0) + MC_IOM[i], _BN(58));	// CPC DATA
	if(!repro)
		do {
			for(val=0,i=mc; i<4 ;i+=step)
				val |= DCRReadPriv(_DDR_MC_IOM_CAL_CNTL(0) + MC_IOM[i]) & _BN(58);
		} while(val);
	else
		DelayTimeBase(WAIT_CAL_CPC);
//long long time1 = GetTimeBase();
//printf("DATA CPC time = %lld\n", time1-time0);


//time0 = GetTimeBase();
	for(i=mc; i<4 ;i+=step)
		DCRWritePriv(_DDR_MC_IOM_CAL_CNTL(0) + MC_IOM[i], _BN(59));	// CPC ADDR
	if(!repro)
		do {
			for(val=0,i=mc; i<4 ;i+=step)
				val |= DCRReadPriv(_DDR_MC_IOM_CAL_CNTL(0) + MC_IOM[i]) & _BN(59);
		} while(val);
	else
		DelayTimeBase(WAIT_CAL_CPC);
//time1 = GetTimeBase();
//printf("ADDR CPC time = %lld\n", time1-time0);


#if FW_DEBUG
	for(i=mc; i<4 ;i+=step)
	{
		DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B16(47,0xEFFF));
		val = 0xFFFF & DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i]);
		MC_DEBUG(("CPC_X4: MASTER_CNTL_ERROR_REG: MC[%d]_SAT[%d] = 0x%08lx\n", i%2, i/2, val));
	}
#endif


    if(TI_isDD1())
	for(i=mc; i<4 ;i+=step)
		DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B16(47,0xEFFD) | effd | _B8(63,0xFF));	// Recover to x8 mode 

}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*  Step 10. Program MRS  */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

void DDR_Init_MRS(struct DDRINIT_metrics *ddr)
{
	int i;
	uint64_t val=0;


	for(i=0; i<2 ;i++)	// MR2
		DCRWritePriv(_DDR_MC_MCAPOS(i),
		_B1(3,((ddr->CWL - 5) >> 0) & 0x1) |
		_B1(4,((ddr->CWL - 5) >> 1) & 0x1) |
		_B1(5,((ddr->CWL - 5) >> 2) & 0x1) |
		_B1(9, (ddr->RTT_WR==60)?1:0) |
		_B1(10,(ddr->RTT_WR==120)?1:0) |
		_B1(16,0) |	// MR2
		_B1(17,1) |	// MR2
		_BN(19) | _BN(20) | _BN(21) | _B8(30,CSB_PAT) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// common for all MRS
	do
		val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
	while(val);


	for(i=0; i<2 ;i++)	// MR3
		DCRWritePriv(_DDR_MC_MCAPOS(i),
		_B1(16,1) |	// MR3
		_B1(17,1) |	// MR3
		_BN(19) | _BN(20) | _BN(21) | _B8(30,CSB_PAT) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// common for all MRS
	do
		val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
	while(val);


	for(i=0; i<2 ;i++)	// MR1
		DCRWritePriv(_DDR_MC_MCAPOS(i),
		_B1(1,(ddr->RON==34)?1:0) |
		_B1(2,(ddr->RTT==30 || ddr->RTT==40 || ddr->RTT==60)?1:0) |
		_B1(3,(ddr->AL==ddr->CL-1)?1:0) |
		_B1(4,(ddr->AL==ddr->CL-2)?1:0) |
		_B1(6,(ddr->RTT==40 || ddr->RTT==120)?1:0) |
		_B1(9,(ddr->RTT==20 || ddr->RTT==30)?1:0) |
		_B1(11,1) |	// TDQS enable (DM disable)
		_B1(16,1) |	// MR1
		_B1(17,0) |	// MR1
		_BN(19) | _BN(20) | _BN(21) | _B8(30,CSB_PAT) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// common for all MRS
	do
		val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
	while(val);


	for(i=0; i<2 ;i++)	// MR0
		DCRWritePriv(_DDR_MC_MCAPOS(i),
		_B1(2,((ddr->CL - 4) >> 3) & 0x1) |
		_B1(4,((ddr->CL - 4) >> 0) & 0x1) |
		_B1(5,((ddr->CL - 4) >> 1) & 0x1) |
		_B1(6,((ddr->CL - 4) >> 2) & 0x1) |
		_BN(8) |	// DLL reset
		_B1(9, (ddr->TWR==5 || ddr->TWR==7 || ddr->TWR==10 || ddr->TWR==14)?1:0) |
		_B1(10,(ddr->TWR==6 || ddr->TWR==7 || ddr->TWR==12 || ddr->TWR==14)?1:0) |
		_B1(11,(ddr->TWR>=8 && ddr->TWR<=14)?1:0) |
		_B1(12, ddr->PD_FAST) |	// Power down type (Slow vs. Fast)
		_B1(16,0) |	// MR0
		_B1(17,0) |	// MR0
		_BN(19) | _BN(20) | _BN(21) | _B8(30,CSB_PAT) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// common for all MRS
	do
		val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
	while(val);


	for(i=0; i<2 ;i++)	// ZQCL
		DCRWritePriv(_DDR_MC_MCAPOS(i),
		_BN(10) |	// A10 (Long not Short)
		_BN(21) | _B8(30,CSB_PAT) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// ZQ command
	do
		val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
	while(val);


	Delay_ns(1000, 265);	// tDLLK
}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*  Step 11A.Detect Ranks and Fine Write Leveling  */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

void DDR_Init_Detect_Ranks(unsigned *num_ranks, unsigned dram_density, struct DDRINIT_metrics *ddr, unsigned *MC_IOM, unsigned *ODT, int repro)
{
	void DDR_Init_FWL();
	int  DDR_Init_MasterCtrlErrReg();


	for(*num_ranks=MAX_NUM_RANKS; *num_ranks >= 1; *num_ranks /= 2)
	{
#if FW_DEBUG
		MC_DEBUG(("Num_ranks = %d    dram_density = %dMb\n",*num_ranks,dram_density));
#endif
		DDR_Init_Misc(*num_ranks, &dram_density, ddr);
		DDR_Init_MCS(*num_ranks, dram_density);
		DDR_Init_MCA(*num_ranks, dram_density, ddr, ODT);

		DDR_Init_FWL(*num_ranks, ddr, MC_IOM, ODT, repro);	// PASS-1
		if(DDR_Init_MasterCtrlErrReg(ddr, MC_IOM, 1)==0)
			break;

		DDR_Init_FWL(*num_ranks, ddr, MC_IOM, ODT, repro);	// PASS-2
		if(DDR_Init_MasterCtrlErrReg(ddr, MC_IOM, 1)==0)
			break;
	}
	MC_DEBUG(("%d RANK%c\n", *num_ranks, (*num_ranks==1)?' ':'S'));
}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*  Step 11B.Fine Write Leveling  */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

void DDR_Init_FWL(unsigned num_ranks, struct DDRINIT_metrics *ddr, unsigned *MC_IOM, unsigned *ODT, int repro)
{
	int i, RRR, rr;
	uint64_t val=0;


	for(i=0; i<4 ;i++)
		DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B16(47,0x3F88) |  _B16(63,0));

#if CWL_TEST==1
	for(rr=0; rr<num_ranks; rr++)
		for(i=0; i<2 ;i++)
			for(val2=0x0, chip=0; chip<18; chip++, val2+=0x3F)
			{
				val=val2%0x3FF;
				DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[chip<9 ? i:i+2],
					_B16(47,0x2004) |
					_B2(38,rr) | _B4(42,chip%9) |
					_B16(63,val));
				for(bbbb=0; bbbb<8; bbbb++)
					DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[chip<9 ? i:i+2],
						_B16(47,0x2000) |
						_B2(38,rr) | _B4(42,chip%9) | _B1(43,bbbb/4) | _B2(47,bbbb%4) |
						_B16(63,val));
			}
#endif


	for(i=0; i<2 ;i++)	// MR1 WL_en QOFF_en
		DCRWritePriv(_DDR_MC_MCAPOS(i),
		_B1(1,(ddr->RON==34)?1:0) |
		_B1(2,(ddr->RTT==30 || ddr->RTT==40 || ddr->RTT==60)?1:0) |
		_B1(3,(ddr->AL==ddr->CL-1)?1:0) |
		_B1(4,(ddr->AL==ddr->CL-2)?1:0) |
		_B1(6,(ddr->RTT==40 || ddr->RTT==120)?1:0) |
		_B1(9,(ddr->RTT==20 || ddr->RTT==30)?1:0) |
		_BN(7) |	// WL enable
		_BN(11) |	// TDQS enable (DM disable)
		_BN(12) |	// Qoff enable
		_B1(16,1) |	// MR1
		_B1(17,0) |	// MR1
		_BN(19) | _BN(20) | _BN(21) | _B8(30,CSB_PAT) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// common for all MRS
	if(!repro)
		do
			val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
		while(val);


	for(rr=0; rr<num_ranks; rr++)
	{
		RRR = MC_RANK_RRR(rr);
#if FW_DEBUG
		MC_DEBUG(("Fine Write Leveling: rank%d\n", rr));
		MC_DEBUG(("RRR = %x (%d)\n",RRR,RRR));
#endif
		for(i=0; i<2 ;i++)	// MR1 WL_en QOFF_dis
			DCRWritePriv(_DDR_MC_MCAPOS(i),
			_B1(1,(ddr->RON==34)?1:0) |
			_B1(2,(ddr->RTT==30 || ddr->RTT==40 || ddr->RTT==60)?1:0) |
			_B1(3,(ddr->AL==ddr->CL-1)?1:0) |
			_B1(4,(ddr->AL==ddr->CL-2)?1:0) |
			_B1(6,(ddr->RTT==40 || ddr->RTT==120)?1:0) |
			_B1(9,(ddr->RTT==20 || ddr->RTT==30)?1:0) |
			_BN(7) |	// WL enable
			_BN(11) |	// TDQS enable (DM disable)
	    	/*	_BN(12) | */	// Qoff disable	
			_B1(16,1) |	// MR1
			_B1(17,0) |	// MR1
			_BN(19) | _BN(20) | _BN(21) | _B8(30,0x80 >> rr) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// common for all MRS
		if(!repro)
			do
				val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
		while(val);

		for(i=0; i<2 ;i++)
			DCRWritePriv(_DDR_MC_MCAPOS(i), _B22(21, 0x155555) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37) | _B4(42,ODT[rr]) | _B3(45,RRR));
		if(!repro)
			do
				val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
		while(val);
	
//long long time0 = GetTimeBase();
		for(i=0; i<4 ;i++)
			DCRWritePriv(_DDR_MC_IOM_CAL_CNTL(0) + MC_IOM[i], _BN(63));
		if(!repro)
			do {
				for(val=0,i=0; i<4 ;i++)
					val |= DCRReadPriv(_DDR_MC_IOM_CAL_CNTL(0) + MC_IOM[i]) & _BN(63);
			} while(val);
		else
			DelayTimeBase(WAIT_CAL_FWL);
//long long time1 = GetTimeBase();
//printf("FWL time = %lld\n", time1-time0);

		for(i=0; i<2 ;i++)
			DCRWritePriv(_DDR_MC_MCAPOS(i),
			_B1(1,(ddr->RON==34)?1:0) |
			_B1(2,(ddr->RTT==30 || ddr->RTT==40 || ddr->RTT==60)?1:0) |
			_B1(3,(ddr->AL==ddr->CL-1)?1:0) |
			_B1(4,(ddr->AL==ddr->CL-2)?1:0) |
			_B1(6,(ddr->RTT==40 || ddr->RTT==120)?1:0) |
			_B1(9,(ddr->RTT==20 || ddr->RTT==30)?1:0) |
			_BN(7) |	// WL enable
			_BN(11) |	// TDQS enable (DM disable)
			_BN(12) |	// Qoff enable
			_B1(16,1) |	// MR1
			_B1(17,0) |	// MR1
			_BN(19) | _BN(20) | _BN(21) | _B8(30,0x80 >> rr) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// common for all MRS
		if(!repro)
			do
				val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
		while(val);

#if FW_DEBUG
		int rr2;
		for(rr2=0; rr2<4; rr2++)
		{
			DCRWritePriv(_DDR_MC_IOM_PHYREAD(0), _B16(47,0x2004) | _B2(38,rr2) | _B4(42,0%9));
			MC_DEBUG(("MC[0] DQS0[rank%d/%d] WRITE delay = %lx\n",MC_Mapped_Rank(rr2),MC_Mapped_Rank(rr2)+4,0xFFFF & DCRReadPriv(_DDR_MC_IOM_PHYREAD(0))));
		}
	//	DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0), _B16(47,0x3F88) | _B8(63,0x0F));
#endif

	}


	for(i=0; i<2 ;i++)	// MR1 WL_dis QOFF_dis
		DCRWritePriv(_DDR_MC_MCAPOS(i),
		_B1(1,(ddr->RON==34)?1:0) |
		_B1(2,(ddr->RTT==30 || ddr->RTT==40 || ddr->RTT==60)?1:0) |
		_B1(3,(ddr->AL==ddr->CL-1)?1:0) |
		_B1(4,(ddr->AL==ddr->CL-2)?1:0) |
		_B1(6,(ddr->RTT==40 || ddr->RTT==120)?1:0) |
		_B1(9,(ddr->RTT==20 || ddr->RTT==30)?1:0) |
	    /*	_BN(7) | */	// WL disable	
		_BN(11) |	// TDQS enable (DM disable)
	    /*	_BN(12) | */	// Qoff disable	
		_B1(16,1) |	// MR1
		_B1(17,0) |	// MR1
		_BN(19) | _BN(20) | _BN(21) | _B8(30,CSB_PAT) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// common for all MRS
	if(!repro)
		do
			val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
		while(val);

#if HARD_FAILURE
// Hard failure injection (WRLVL)
//
//	DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0), _B16(47,0x2004) | _B2(38,0) | 0x300);
//	DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0), _B16(47,0x2004) | _B2(38,1) | 0x300);
//	DCRWritePriv(_DDR_MC_IOM_PHYWRITE(1), _B16(47,0x2004) | _B2(38,0) | 0x300);
//	DCRWritePriv(_DDR_MC_IOM_PHYWRITE(1), _B16(47,0x2004) | _B2(38,1) | 0x300);
#endif

}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*  Step 12. Initial Read Calibration  */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

void DDR_Init_Read_Cal(unsigned num_ranks, struct DDRINIT_metrics *ddr, unsigned *MC_IOM, int ext_repro)
{
	int i, k, RRR, rr;
	uint64_t val=0;
	int recal = ext_repro & 0x2;
	int repro = recal==0? (ext_repro & 0x1) : 0;
	int mc    = recal==0? 0 : (ext_repro & 0x1);
	int step  = recal==0? 1 : 2;
	int check = recal==0? 0 : mc;

#if DELAY_OVERRIDE
	int chip;
	unsigned rdgat[2][18]={ {0x9,0xA,0xA,0xB,0xC,0x9,0x9,0xA,0xB,0xB,0x9,0x9,0xA,0xB,0xB,0x9,0xA,0xB},
				{0x9,0xA,0xA,0xB,0xB,0x8,0x9,0xA,0xB,0xB,0x9,0x9,0xA,0xB,0xB,0xA,0xB,0xC} };
	unsigned rdcal[2][18]={ {0x18,0x10,0x08,0x10,0x18,0x08,0x10,0x18,0x20,0x08,0x20,0x10,0x18,0x20,0x08,0x08,0x10,0x18},
				{0x18,0x08,0x10,0x18,0x20,0x08,0x10,0x20,0x20,0x10,0x18,0x20,0x10,0x18,0x20,0x20,0x18,0x08} };

	for(i=mc; i<2; i+=step)
	{
		for(rr=0; rr<num_ranks; rr++)
		{
			for(chip=0; chip<18; chip++)
			{
				DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[chip<9 ? i:i+2], _B16(47,0x6000) | _B3(38,IOM_RANK_RRR(rr)) | _B4(42,chip%9) | _B16(63,rdgat[i][chip]));
				DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[chip<9 ? i:i+2], _B16(47,0x6001) | _B3(38,IOM_RANK_RRR(0)) | _B4(42,chip%9) | _B16(63,rdcal[i][chip]));
			}
			for(k=0; k<2; k++)
			{
				DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i+2*k], _B16(47,0x4000) | _B3(38,IOM_RANK_RRR(rr)) | _B4(42,0xF) | _B16(63,0x25));
				DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i+2*k], _B16(47,0x4008) | _B3(38,IOM_RANK_RRR(rr)) | _B4(42,0xF) | _B16(63,0x3));
			}
		}
	}
	return;
#endif

	if(recal==0)
	    for(i=mc; i<4 ;i+=step)
	    {
		DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B16(47,0x4003) | _B16(63,ddr->RDDLY_CFG0));	// RDDLY CONFIG0
		DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B16(47,0x5003) | _B16(63,ddr->RDDLY_CFG1));	// RDDLY CONFIG1
		DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B16(47,0x6FFF) | _B16(63,ddr->RDCAL_CNTLF));	// RDCAL CONFIG	
		DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B16(47,0x6FF6) | _B16(63,ddr->RDCAL_CNTL6));	// RDCAL CONFIG1
		DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B16(47,0x6FF3) | _B16(63,ddr->RDCAL_CNTL3));	// RDCAL CONFIG2
		DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B16(47,0x6FF9) | _B16(63,ddr->RDCAL_CNTL9));	// RDCAL RDDLY Override
		DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B16(47,0x6FFB) | _B16(63,ddr->RDCAL_CNTLB));	// RDCAL Global Delay Setting
		DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B16(47,0x6FFD) | _B16(63,ddr->RDCAL_CNTLD));	// RDCAL MISC Register
	    }


	for(rr=0; rr<num_ranks; rr++)
	{
#if FW_DEBUG
		MC_DEBUG(("Read calibration: rank%d\n", rr));
#endif

		for(i=mc;i<2;i+=step)
			DCRWritePriv(_DDR_MC_MCAPOS(i), _B19(18, 0x55555) | _BN(19) | _BN(21) | _B8(30,CSB_PAT) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// PREA
		if(!repro)
			do
				val = DCRReadPriv(_DDR_MC_MCAPOS(check)) & _BN(37);
			while(val);
	
		//-----------------------------------------
		//-- Adding DLL RESET right before RdCal --
		//-----------------------------------------
		for(i=mc; i<2 ;i+=step)	// MR0
			DCRWritePriv(_DDR_MC_MCAPOS(i),
			_B1(2,((ddr->CL - 4) >> 3) & 0x1) |
			_B1(4,((ddr->CL - 4) >> 0) & 0x1) |
			_B1(5,((ddr->CL - 4) >> 1) & 0x1) |
			_B1(6,((ddr->CL - 4) >> 2) & 0x1) |
			_BN(8) |	// DLL reset
			_B1(9, (ddr->TWR==5 || ddr->TWR==7 || ddr->TWR==10 || ddr->TWR==14)?1:0) |
			_B1(10,(ddr->TWR==6 || ddr->TWR==7 || ddr->TWR==12 || ddr->TWR==14)?1:0) |
			_B1(11,(ddr->TWR>=8 && ddr->TWR<=14)?1:0) |
			_B1(12, ddr->PD_FAST) |	// Power down type (Slow vs. Fast)
			_B1(16,0) |	// MR0
			_B1(17,0) |	// MR0
			_BN(19) | _BN(20) | _BN(21) | _B8(30,0x80 >> rr) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// common for all MRS
		do
			val = DCRReadPriv(_DDR_MC_MCAPOS(check)) & _BN(37);
		while(val);


		for(i=mc;i<2;i+=step)
			DCRWritePriv(_DDR_MC_MCAPOS(i), _B19(18, 0x55555) | _BN(19) | _BN(20) | _B8(30,CSB_PAT) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// Refresh (Micron)
		if(!repro)
			do
				val = DCRReadPriv(_DDR_MC_MCAPOS(check)) & _BN(37);
			while(val);
	
		Delay_ns(300, 265);	// tRFC


		for(i=mc; i<2 ;i+=step)	// MR3_CAL
			DCRWritePriv(_DDR_MC_MCAPOS(i),
			_BN(2) |	// MPR enable
			_B1(16,1) |	// MR3
			_B1(17,1) |	// MR3
			_BN(19) | _BN(20) | _BN(21) | _B8(30,0x80 >> rr) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// common for all MRS
		if(!repro)
			do
				val = DCRReadPriv(_DDR_MC_MCAPOS(check)) & _BN(37);
			while(val);

	
		RRR = MC_RANK_RRR(rr);
		for(i=mc; i<2 ;i+=step)
			DCRWritePriv(_DDR_MC_MCAPOS(i), _B22(21, 0x155555) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37) | _B3(45,RRR));
		if(!repro)
			do
				val = DCRReadPriv(_DDR_MC_MCAPOS(check)) & _BN(37);
		while(val);

//long long time0 = GetTimeBase();
		for(k=0; k<2; k++)
		{
			for(i=2*k+mc; i<2*(k+1) ;i+=step)
					DCRWritePriv(_DDR_MC_IOM_CAL_CNTL(0) + MC_IOM[i], _BN(60)|_BN(61)|_BN(62));
			if(!repro)
				do {
					for(val=0,i=2*k+mc; i<2*(k+1) ;i+=step)
						val |= DCRReadPriv(_DDR_MC_IOM_CAL_CNTL(0) + MC_IOM[i]) & _B3(62,0x7);
				} while(val!=0);
			else
				DelayTimeBase(WAIT_CAL_RDCAL);
		}
//long long time1 = GetTimeBase();
//printf("RDCAL time = %lld\n", time1-time0);


		for(i=mc; i<2 ;i+=step)
			DCRWritePriv(_DDR_MC_MCAPOS(i), _B19(18, 0x55555) | _BN(19) | _BN(21) | _B8(30,CSB_PAT) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// PREA (Elpida)
		if(!repro)
			do
				val = DCRReadPriv(_DDR_MC_MCAPOS(check)) & _BN(37);
			while(val);
	

		for(i=mc; i<2 ;i+=step)	// MR3 (MPR disable)
			DCRWritePriv(_DDR_MC_MCAPOS(i),
			_B1(16,1) |	// MR3
			_B1(17,1) |	// MR3
			_BN(19) | _BN(20) | _BN(21) | _B8(30,0x80 >> rr) | _B4(34,CKE_PAT) | _BN(35) | _BN(36) | _BN(37));	// common for all MRS
		if(!repro)
			do
				val = DCRReadPriv(_DDR_MC_MCAPOS(check)) & _BN(37);
			while(val);

#if FW_DEBUG
		int rr2;
		for(rr2=0; rr2<8; rr2++)
		{
			DCRWritePriv(_DDR_MC_IOM_PHYREAD(0), _B16(47,0x6000) | _B3(38,IOM_RANK_RRR(rr2)) | _B4(42,0%9));
			MC_DEBUG(("MC[0] DQS0[rank%d] RDCAL delay = %lx\n",rr2,0xFFFF & DCRReadPriv(_DDR_MC_IOM_PHYREAD(0))));
		}
	//	DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0), _B16(47,0x7FF0) | _B8(63,0x0F));
#endif
	}


#if HARD_FAILURE
// Hard failure injection (RDDLY)
//

	DCRWritePriv(_DDR_MC_IOM_PHYREAD(0), _B16(47,0x4000));
	val = 0xFFFF& DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[0]);
	DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0), _B16(47,0x4000) | _B3(38,0) | _B4(42,1%9) | (val+HARD_SKEW));
	DCRWritePriv(_DDR_MC_IOM_PHYWRITE(1), _B16(47,0x4000) | _B3(38,0) | _B4(42,1%9) | (val+HARD_SKEW));
//	printf("Forcing DQS delay from %lX to %lX\n",val,DCRReadPriv(_DDR_MC_IOM_PHYWRITE(0), _B16(47,0x4000) | _B3(38,0) | _B4(42,1%9)));

	DCRWritePriv(_DDR_MC_IOM_PHYREAD(0), _B16(47,0x4001));
	val = 0xFFFF& DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[0]);
	DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0), _B16(47,0x4001) | _B3(38,0) | _B4(42,1%9) | (val+HARD_SKEW));
	DCRWritePriv(_DDR_MC_IOM_PHYWRITE(1), _B16(47,0x4001) | _B3(38,0) | _B4(42,1%9) | (val+HARD_SKEW));

	DCRWritePriv(_DDR_MC_IOM_PHYREAD(0), _B16(47,0x4002));
	val = 0xFFFF& DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[0]);
	DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0), _B16(47,0x4002) | _B3(38,0) | _B4(42,1%9) | (val+HARD_SKEW));
	DCRWritePriv(_DDR_MC_IOM_PHYWRITE(1), _B16(47,0x4002) | _B3(38,0) | _B4(42,1%9) | (val+HARD_SKEW));

//	DCRWritePriv(_DDR_MC_IOM_PHYWRITE(1) + MC_IOM[0], _B16(47,0x4000) | _B3(38,0) | _B4(42,1%9) | 0x200);	// mc=1, rank=0, chip=1
//	DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0), _B16(47,0x4000) | _B3(38,0) | 0x200);
//	DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0), _B16(47,0x4000) | _B3(38,1) | 0x200);
//	DCRWritePriv(_DDR_MC_IOM_PHYWRITE(1), _B16(47,0x4000) | _B3(38,0) | 0x200);
//	DCRWritePriv(_DDR_MC_IOM_PHYWRITE(1), _B16(47,0x4000) | _B3(38,1) | 0x200);
#endif

}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*  Step 12A. Master Control Error Register  */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

int DDR_Init_MasterCtrlErrReg(struct DDRINIT_metrics *ddr, unsigned *MC_IOM, int ext_rank_detect_mode)
{
	int i, error, chip;
	uint64_t val=0, val2, val3;
	char msg[10],pf;
	int recal		= ext_rank_detect_mode & 0x2;
	int rank_detect_mode	= recal==0? (ext_rank_detect_mode & 0x1) : 0;
	int mc			= recal==0? 0 : (ext_rank_detect_mode & 0x1);


	if(!rank_detect_mode)
	{
		for(error=0,i=mc; i<(ddr->FAST?1:4) ;)
		{
			DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B16(47,0xEFFF));
			val3 = DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i]);
			if((val3 & 0xFFFF)!=0x0)
			{
				error=1;
				MC_DEBUG(("---\n"));
				MC_DEBUG(("MASTER_CNTL_ERROR_REG: MC[%d]_SAT[%d] = 0x%08lx\n", i%2, i/2, val3));
				if((val3 & _BN(59))!=0x0)
				{
					DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B16(47,0x6FFE));
					val = DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i]);
					MC_DEBUG(("    RDCAL_STATUS_REG : MC[%d]_SAT[%d] = 0x%08lx\n", i%2, i/2, val));
			
					if((val & _BN(49))!=0x0)
					{
						DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B16(47,0x6FE8));
						val = DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i]);
						DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B16(47,0x6FF8));
						val2 = DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i]);
						MC_DEBUG(("       RDCAL_DATA_ERR_REG: MC[%d]_SAT[%d] = 0x%08lx-0x%08lx\n", i%2, i/2, val, val2));
			
						DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B16(47,0x6FEC));
						val = DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i]);
						DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B16(47,0x6FFC));
						val2 = DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i]);
						MC_DEBUG(("       RDCAL_PREAMBLE_REG: MC[%d]_SAT[%d] = 0x%08lx-0x%08lx\n", i%2, i/2, val, val2));
					}
			
					DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B16(47,0x6FE5));
					val = DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i]);
					DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B16(47,0x6FF5));
					val2 = DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i]);
					MC_DEBUG(("       RDCAL_BYTE_ERR_DQS: MC[%d]_SAT[%d] = 0x%08lx-0x%08lx\n", i%2, i/2, val, val2));
				}
				if((val3 & _BN(60))!=0x0)
				{
					DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B16(47,0x2FFE));
					val = DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i]);
					MC_DEBUG(("    WR_LEVEL_ERR_REG : MC[%d]_SAT[%d] = 0x%08lx\n", i%2, i/2, val));
				}
				if((val3 & _BN(61))!=0x0)
				{
					DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B16(47,0x5002));
					val = DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i]);
					MC_DEBUG(("    RDDLY_ERR_REG    : MC[%d]_SAT[%d] = 0x%08lx\n", i%2, i/2, val));
				}
				if((val3 & _BN(63))!=0x0)
				{
					DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B16(47,0xE005));
					val = DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i]);
					MC_DEBUG(("    MASTER_CNTL_CPC_ERR_REG(CMD/ADR): MC[%d]_SAT[%d] = 0x%08lx\n", i%2, i/2, val));
					DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B16(47,0xE006));
					val = DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i]);
					MC_DEBUG(("    MASTER_CNTL_CPC_ERROR_REG_(DATA): MC[%d]_SAT[%d] = 0x%08lx\n", i%2, i/2, val));
				}
			}
#if FW_DEBUG
			else
				MC_DEBUG(("HW IOM CALIBRATION SUCCEEDED!!!: MC[%d]_SAT[%d] = 0x%08lx\n", i%2, i/2, val));
#endif
			switch(i)
			{
				case 0: i=2; break;
				case 1: i=3; break;
				case 2: i=(recal==0? 1:4); break;
				case 3: i=4; break;
			}
		}
	}
	else
	{
		msg[9]='\0';
		for(error=0,i=0; i<4 ;i++)
		{
			for(chip=0; chip<9; chip++)
			{
				DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i], _B4(42,chip) | _B16(47,0x200E));
				pf=DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[i]) & 0x7;
				if(pf != 0x0)
					error++;
				msg[chip]=pf + '0';
			}
			MC_DEBUG(("WRLVL: MC[%d]_SAT[%d] = %s\n", i%2, i/2, msg));
		}
		if(error<9)	// up to 1/4th of chips can be bad
			error=0;
	}

	if(!rank_detect_mode || FW_DEBUG)
	{
		if(!error)
			MC_DEBUG(("HW IOM CALIBRATION SUCCEEDED!!!\n"));
		else
			MC_DEBUG(("ERROR: DDR HW IOM CALIBRATION FAILED\n"));
	}
	return error;
}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*  Step 14A. Detect Density  */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

void DDR_Init_Detect_Density(unsigned *dram_density, unsigned num_ranks, struct DDRINIT_metrics *ddr, unsigned *ODT, uint64_t CWL_wrdata_pat[][17], uint64_t *rddata)
{
	char msg[30] = "xGb MC[y] rank[z] got:";
	int local_pass, i, rr, k, mismatch;
	uint64_t temp1[17], temp2[17], comp;


	for(*dram_density=MAX_DENSITY; *dram_density >= MIN_DENSITY; *dram_density /= 2)
	{
#if FW_DEBUG
		MC_DEBUG(("Num_ranks = %d    dram_density = %dMb\n",num_ranks,*dram_density));
#endif
		msg[0] = (*dram_density)/1024+'0';
		DDR_Init_Misc(num_ranks, dram_density, ddr);
		DDR_Init_MCS(num_ranks, *dram_density); 
		DDR_Init_MCA(num_ranks, *dram_density, ddr, ODT);
		for(local_pass=1,i=0;i<2;i++)
		{
			msg[7] = i+'0';
			for(rr=0;rr<num_ranks;rr++)
			{
					MEM_Display_addr(i, rr, 7, MAX_ROW_ADDR(*dram_density), 0x3FF, temp1);
					MEM_Display_addr(i, rr, 7, MAX_ROW_ADDR(*dram_density)>>1, 0x3FF, temp2);

					MEM_Alter_addr(i, rr, 7, MAX_ROW_ADDR(*dram_density), 0x3FF, CWL_wrdata_pat[rr%2]);
					MEM_Alter_addr(i, rr, 7, MAX_ROW_ADDR(*dram_density)>>1, 0x3FF, CWL_wrdata_pat[(rr+1)%2]);
					MEM_Display_addr(i, rr, 7, MAX_ROW_ADDR(*dram_density), 0x3FF, rddata);
					comp = Mem_Compare_8L(CWL_wrdata_pat[rr%2], rddata) | Mem_Compare_8H(CWL_wrdata_pat[rr%2], rddata);
					for(mismatch=0,k=0; k<64; k++)
						mismatch += (comp>>k) & 0x1;
					MC_DEBUG(("Compare pattern = 0x%016lX	Number of mismatch bits = %d\n",comp,mismatch));
					ddr->MRKCHIP[i][rr] = -1;
					if(mismatch > 8)	// okay up to 1 bad chip per rank per MC
						local_pass = 0;
					else
					{
						for(k=0; k<18; k++)
						{
							if(k<8)
								comp = Mem_Compare_8L(CWL_wrdata_pat[rr%2], rddata) & (0xFFull << (56 - k*8));
							else if(k==8)
								comp = (CWL_wrdata_pat[rr%2][16] ^ rddata[16]) & 0xFFFFFFFF00000000;
							else if(k<17)
								comp = Mem_Compare_8H(CWL_wrdata_pat[rr%2], rddata)  & (0xFFull << (56 - (k-9)*8));
							else if(k==17)
								comp = (CWL_wrdata_pat[rr%2][16] ^ rddata[16]) & 0x00000000FFFFFFFF;
							if(comp != 0x0)
								break;
						}
						if(k < 18)
						{
						//	ddr->SKIP_SMBTEST = 1;
							ddr->MRKCHIP[i][rr] = k;
						}
					}
#if FW_DEBUG
					msg[15]= rr+'0';
					Mem_print_vector(rddata, msg, 17);
#endif
					MEM_Alter_addr(i, rr, 7, MAX_ROW_ADDR(*dram_density), 0x3FF, temp1);
					MEM_Alter_addr(i, rr, 7, MAX_ROW_ADDR(*dram_density)>>1, 0x3FF, temp2);
			}
		}
		if(local_pass)
			break;
	}
	MC_DEBUG(("%dMb DRAM\n", *dram_density));
}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*  Step xx. Dump Delays */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

void DDR_Init_Dump_Delays(unsigned num_ranks, unsigned *MC_IOM)
{
	int i, chip, rr;
	uint64_t Delay[18];
	void Mem_print_vector();

	for(i=0; i<2; i++)
	{
		for(rr=0; rr<num_ranks; rr++)
		{
				//		   1		 2
				//	  2     8  1		 5
			char msg[40] = "MC* RANK*: WRLVL delay DQS[0:17]";
			msg[2]=i+'0';
			msg[8]=rr+'0';
			for(chip=0; chip<18; chip++)
			{
				DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[chip<9 ? i:i+2], _B16(47,0x2004) | _B2(38,MC_Mapped_Rank(rr)) | _B4(42,chip%9));
				Delay[chip] = 0xFFFF & DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[chip<9 ? i:i+2]);
			}
			Mem_print_vector(Delay, msg, 18);

		//	msg[25]='0';
		//	for(chip=0; chip<18; chip++)
		//	{
		//		DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[chip<9 ? i:i+2], _B16(47,0x2000) | _B2(38,MC_Mapped_Rank(rr)) | _B4(42,chip%9));
		//		Delay[chip] = 0xFFFF & DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[chip<9 ? i:i+2]);
		//	}
		//	Mem_print_vector(Delay, msg, 18);

			msg[11]='R';
			msg[12]='D';
			msg[13]='G';
			msg[14]='A';
			msg[15]='T';
			msg[25]='S';
			for(chip=0; chip<18; chip++)
			{
				DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[chip<9 ? i:i+2], _B16(47,0x6000) | _B3(38,IOM_RANK_RRR(rr)) | _B4(42,chip%9));
				Delay[chip] = 0xFFFF & DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[chip<9 ? i:i+2]);
			}
			Mem_print_vector(Delay, msg, 18);

		if(rr==0) {
			msg[13]='C';
			msg[15]='L';
			for(chip=0; chip<18; chip++)
			{
				DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[chip<9 ? i:i+2], _B16(47,0x6001) | _B3(38,IOM_RANK_RRR(rr)) | _B4(42,chip%9));
				Delay[chip] = 0xFFFF & DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[chip<9 ? i:i+2]);
			}
			Mem_print_vector(Delay, msg, 18);
		}

			msg[13]='D';
			msg[14]='L';
			msg[15]='Y';
			for(chip=0; chip<18; chip++)
			{
				DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[chip<9 ? i:i+2], _B16(47,0x4000) | _B3(38,IOM_RANK_RRR(rr)) | _B4(42,chip%9));
				Delay[chip] = 0xFFFF & DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[chip<9 ? i:i+2]);
			}
			Mem_print_vector(Delay, msg, 18);

			msg[25]='0';
			for(chip=0; chip<18; chip++)
			{
				DCRWritePriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[chip<9 ? i:i+2], _B16(47,0x4008) | _B3(38,IOM_RANK_RRR(rr)) | _B4(42,chip%9));
				Delay[chip] = 0xFFFF & DCRReadPriv(_DDR_MC_IOM_PHYREAD(0) + MC_IOM[chip<9 ? i:i+2]);
			}
			Mem_print_vector(Delay, msg, 18);
		}
	}
}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*  Step yy. PHY Recal */
/*----------------------------------------------------------------------------------------------------------------------------------------*/


#define NUM_RECAL_RETRY	4
#define RECAL_WINDOW	2	// seconds
#define	WAIT_MCMODE	40000

void DDR_PHY_Recal(int mc)
{
	if ( TI_isDD1() || !PERS_ENABLED(PERS_ENABLE_DDRDynamicRecal) ) return;

	struct DDRINIT_metrics ddr = { 0, };
	void DDR_Init_CPC();
	void DDR_Init_Read_Cal();
        int DDR_Init_MasterCtrlErrReg();
	unsigned MC_IOM[4] = {0x0, 0x4000, 0x40, 0x4040};
	int pid=ProcessorID(), i, caltry=0;
	
	uint64_t val = (DCRReadPriv(_DDR_MC_MCFGC0(mc)) >> (63-20)) & 0xFF;
	unsigned num_ranks = (val == 0xFF)? 8 : ((val == 0xF0)? 4 : ((val == 0xC0)? 2 : 1));

	uint64_t previous_time = DCRReadPriv(_DDR_MC_MCAPERFMON2(0));	// 0: reset on RDCAL
	if(previous_time < RECAL_WINDOW * 256000 * num_ranks)		// 256000 = 1000msec / 32msec * 8192
		return;

	ddr.CL		= ((DCRReadPriv(_DDR_MC_IOM_FIFO_CNTL(mc)) & 0x1F) + 1) >> 1;	// CL = (IOM_RL + 1) / 2
	ddr.TWR 	= (ddr.CL + 1 < 10)? (ddr.CL + 1) : 10;
	ddr.PD_FAST	= DDR_GET_POWER_DOWN(FW_Personality.DDR_Config.DDRFlags) == DDR_POWER_DOWN_FAST ? 1:0;
	ddr.FAST	= 0;

//	printf("DDR%d PHY Recal...\n",mc);

	uint64_t mcmcc    = DCRReadPriv(_DDR_MC_MCMCC(mc));
	uint64_t mczmrint = DCRReadPriv(_DDR_MC_MCZMRINT(mc));
	uint64_t mcfgp	  = DCRReadPriv(_DDR_MC_MCFGP(mc));

	uint64_t threada0 = DCRReadPriv(TESTINT_DCR(THREAD_ACTIVE0_RB));
	uint64_t threada1 = DCRReadPriv(TESTINT_DCR(THREAD_ACTIVE1_RB));
	uint64_t threadx0 = threada0 & ( pid < 64 ? ~_BN(pid) : ~0 );
	uint64_t threadx1 = threada1 & ( pid < 64 ? ~0 : ~_BN(pid-64) );

	if((mcmcc & _BN(1)) != 0) DCRWritePriv(_DDR_MC_MCMCC(mc), mcmcc & ~(_BN(0) | _BN(1)) );
	DCRWritePriv(_DDR_MC_MCZMRINT(mc), mczmrint & ~(_BN(0)|_BN(8)|_BN(16)|_BN(32)));

	uint64_t time0 = GetTimeBase();

	DCRWritePriv(TESTINT_DCR(THREAD_ACTIVE0)+2, threadx0);
	DCRWritePriv(TESTINT_DCR(THREAD_ACTIVE1)+2, threadx1);

	for(i=0; i<2; i++)  ppc_msync();

	DelayTimeBase(WAIT_MCMODE);

	DCRWritePriv(_DDR_MC_MCFGP(mc), 0);

	for(caltry=0; caltry < NUM_RECAL_RETRY; caltry++)
	{
		DDR_Init_CPC(&ddr, MC_IOM, 2 + mc);
		DDR_Init_Read_Cal(num_ranks, &ddr, MC_IOM, 2 + mc);
		if(DDR_Init_MasterCtrlErrReg(&ddr, MC_IOM, 2 + mc) == 0)
			break;
	}

	if(caltry == NUM_RECAL_RETRY)
	{
		fw_uint64_t details[2];
		details[0] = FW_RAS_DDR_INIT_IOM_CALIBRATION_FAILED;
		details[1] = caltry;
		fw_writeRASEvent( FW_RAS_DDR_INIT_WARNING, 2, details );
	}
	else if(caltry > 0) {
		fw_uint64_t details[2];
		details[0] = FW_RAS_DDR_INIT_IOM_CALIBRATION_RETRIED;
		details[1] = caltry;
		fw_writeRASEvent( FW_RAS_DDR_INIT_WARNING, 2, details );
	}

	DCRWritePriv(_DDR_MC_MCFGP(mc), mcfgp);
	DCRWritePriv(_DDR_MC_MCZMRINT(mc), mczmrint);
	if((mcmcc & _BN(1)) != 0) DCRWritePriv(_DDR_MC_MCMCC(mc), _BN(0) | (mcmcc & ~_BN(1)) );

	DCRWritePriv(TESTINT_DCR(THREAD_ACTIVE0)+1, threadx0);
	DCRWritePriv(TESTINT_DCR(THREAD_ACTIVE1)+1, threadx1);

	uint64_t time1 = GetTimeBase();

	printf("DDR%d PHY was recalibrated: time taken = %ld usec. Previous cal was %ld.%ld seconds ago\n",
		mc,(time1-time0)/1600,(long)previous_time/256000/num_ranks,(long)(previous_time/25600/num_ranks)%10);

	DCRWritePriv(_DDR_MC_MCAPERFMONC0(0), _BN(32) | _B8(63,0xFF));
}
#undef	NUM_RECAL_RETRY
#undef	RECAL_WINDOW
#undef	WAIT_MCMODE


/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*  Step zz. Refresh Speedup */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

#define tREFI_SCALE	10	// percent
#define tREFI_MIN	3900	// nsec
#define	ReCal_TO_RefSU	200	// usec
#define	RefSU_TO_RefSU	10	// sec

void DDR_Refresh_Speedup(int mc)
{
	if (!PERS_ENABLED(PERS_ENABLE_DDRDynamicRecal) ) return;

	uint64_t val = (DCRReadPriv(_DDR_MC_MCFGC0(mc)) >> (63-20)) & 0xFF;
	unsigned num_ranks = (val == 0xFF)? 8 : ((val == 0xF0)? 4 : ((val == 0xC0)? 2 : 1));

	uint64_t previous_ReCal = DCRReadPriv(_DDR_MC_MCAPERFMON2(0));	// 0: reset on ReCal
	uint64_t previous_RefSU = DCRReadPriv(_DDR_MC_MCAPERFMON2(1));	// 1: reset on RefSU
	if(previous_ReCal > ReCal_TO_RefSU * 256 * num_ranks / 1000	// 256/1000 = 1usec / 32msec * 8192
	|| previous_RefSU < RefSU_TO_RefSU * 256000 * num_ranks)	// 256000 = 1000msec / 32msec * 8192
		return;

	void DDR_Init_Freq();
	unsigned dram_speed;
	DDR_Init_Freq(&dram_speed);
        unsigned ref_min = tREFI_MIN*dram_speed/2000/32/num_ranks;

	uint64_t mczmrint = DCRReadPriv(_DDR_MC_MCZMRINT(mc));
	unsigned ref_rate = ((mczmrint >> 20) & 0x3FF) + 1;
	unsigned step = (ref_rate - ref_min) * tREFI_SCALE/100;

	if(ref_rate > ref_min)
		ref_rate -= (step > 1 ? step : 1);
	else
		return;

	DCRWritePriv(_DDR_MC_MCZMRINT(mc), (mczmrint & ~_B10(43,0x3FF)) | _B10(43,ref_rate - 1));
	int tREFI = ref_rate*2000*32*num_ranks/dram_speed;

	FW_Warning("DDR%d Refresh interval was adjusted to %d (%d.%d%d usec). Previous ReCal was %ld usec ago, and previous RefSU was %ld sec ago\n",
		mc,ref_rate,tREFI/1000,(tREFI/100)%10,(tREFI/10)%10,(long)previous_ReCal*1000/256/num_ranks,(long)previous_RefSU/256000/num_ranks);

	if(ref_rate <= ref_min)
		FW_Warning("DDR%d Refresh interval hit the minimum %d (%d.%d%d usec)\n", mc, ref_rate, tREFI/1000, (tREFI/100)%10, (tREFI/10)%10);

	DCRWritePriv(_DDR_MC_MCAPERFMONC0(1), _BN(32) | _B8(63,0xFF));
}


#undef	WAIT_CAL_CPC
#undef	WAIT_CAL_FWL
#undef	WAIT_CAL_RDCAL
#undef	CKE_PAT
#undef	CSB_PAT
#undef	MAX_NUM_RANKS
#undef	MAX_DENSITY
#undef	MIN_DENSITY
#undef	MC_RANK_RRR
#undef	MC_Mapped_Rank
#undef	HARD_FAILURE
#undef	HARD_SKEW
#undef	DELAY_OVERRIDE
#undef	tREFI_SCALE
#undef	tREFI_MIN
#undef	ReCal_TO_RefSU
#undef	RefSU_TO_RefSU
