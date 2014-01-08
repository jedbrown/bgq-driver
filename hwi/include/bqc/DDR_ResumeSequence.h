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
#include <hwi/include/bqc/ddr.h>
#include <hwi/include/bqc/dr_arb_dcr.h>
#include <hwi/include/bqc/dc_arbiter_dcr.h>

int DDR_ResumeSequence(struct DDRINIT_metrics ddr)
{
	unsigned	num_ranks=1, dram_density=1024, dram_speed;
	unsigned	i;
	unsigned	MC_IOM[4] = {0x0, 0x4000, 0x40, 0x4040};
	unsigned	ODT[4];
	uint64_t	val=0;
	uint64_t	rddata[17];
	uint64_t	CWL_wrdata_pat[3][17] = {
	      { 0x0000000000000000, 0x1111111111111111, 0x2222222222222222, 0x3333333333333333,
		0x4444444444444444, 0x5555555555555555, 0x6666666666666666, 0x7777777777777777,
		0x8888888888888888, 0x9999999999999999, 0xAAAAAAAAAAAAAAAA, 0xBBBBBBBBBBBBBBBB,
		0xCCCCCCCCCCCCCCCC, 0xDDDDDDDDDDDDDDDD, 0xEEEEEEEEEEEEEEEE, 0xFFFFFFFFFFFFFFFF,
		0x00B83048AC86608A  },
	      { 0xFFFFFFFFFFFFFFFF, 0xEEEEEEEEEEEEEEEE, 0xDDDDDDDDDDDDDDDD, 0xCCCCCCCCCCCCCCCC,
		0xBBBBBBBBBBBBBBBB, 0xAAAAAAAAAAAAAAAA, 0x9999999999999999, 0x8888888888888888,
		0x7777777777777777, 0x6666666666666666, 0x5555555555555555, 0x4444444444444444,
		0x3333333333333333, 0x2222222222222222, 0x1111111111111111, 0x0000000000000000,
		0x00B78F1E958DDBE5  },
	      { 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
		0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
		0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
		0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
		0x0000000000000000  },
	      };

	void DDR_Init_Freq();
	void DDR_Init_Misc();
	void DDR_Init_MCS();
	void DDR_Init_MCA();
	void DDR_Init_IOM();
	void DDR_Init_CPC();
	void DDR_Init_ADDR_Delay();
	void DDR_Init_Read_Cal();
	int DDR_Init_Detect_Ranks();
	void DDR_Init_FWL();
	int DDR_Init_Detect_Density();
	int  DDR_Init_MasterCtrlErrReg();
//	int  MEM_Alter_addr();
//	int  MEM_Display_addr();
	void Mem_print_vector();


/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 2. Detect Frequency\n\n"));

	DDR_Init_Freq(&dram_speed);

	#include <hwi/include/bqc/DDR_parms.h>

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 3. Configure Misc\n\n"));

	DDR_Init_Misc(num_ranks, &dram_density, &ddr);

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 4. Load MCS registers\n\n"));

	DDR_Init_MCS(num_ranks, dram_density);

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 5. Load MCA registers\n\n"));

	DDR_Init_MCA(num_ranks, dram_density, &ddr, ODT);

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 6. Load IOM registers\n\n"));

	DDR_Init_IOM(&ddr, MC_IOM);

if(!ddr.FAST) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/ 
MC_DEBUG(("Step 7.Adjust ADDR delay\n\n"));

	DDR_Init_ADDR_Delay(MC_IOM, ddr.ADDR_DELAY, ddr.TIS_SHIFT);
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 8. Reinit DRAM\n\n"));


	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCAPOS(i), _BN(35) | _BN(36) | _BN(37));			// RESET = 'H', CKE = 'L'

	DCRWritePriv(TESTINT_DCR(CONFIG1)+0x2, _BN(33));					// TI_DDR_SR_ENABLE_OVERRIDE = '0'

	Delay(100);						// Wait for 100 cycles for TI_DDR_SR_ENABLE_OVERRID to take effect

	DCRWritePriv(TESTINT_DCR(CONFIG1)+0x2, _BN(32));					// TI_DDR_SR_VALUE_OVERRIDE = '0'

	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCAPOS(i), _B4(34,0xF) | _BN(35) | _BN(36) | _BN(37));	// Exit Self-Refresh
	Delay(1000);

	for(i=0; i<2 ;i++)
		DCRWritePriv(_DDR_MC_MCAPOS(i),
		_BN(10) |       // A10 (Long not Short)
		_BN(21) | _B8(30,0xFF) | _B4(34,0xF) | _BN(35) | _BN(36) | _BN(37));		// ZQCL
	Delay(2000);


if(!ddr.SKIP_ALLCAL) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/ 
MC_DEBUG(("Step 9. Initial VDL calibration (CPC)\n\n"));

	DDR_Init_CPC(&ddr, MC_IOM, 1);
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 10. Program MRS\n\n"));

	DDR_Init_MRS(&ddr);

if(!ddr.SKIP_ALLCAL && ddr.RANK_AUTO_DETECT) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 11A.Detect Ranks and Fine Write Leveling\n\n"));

	DDR_Init_Detect_Ranks(&num_ranks, dram_density, &ddr, MC_IOM, ODT, 1);
}

if(!ddr.SKIP_ALLCAL && !ddr.RANK_AUTO_DETECT) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 11B.Fine Write Leveling\n\n"));

	DDR_Init_FWL(num_ranks, &ddr, MC_IOM, ODT, 1);
}

#if 1
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 16Z.MCZMRINT\n\n"));

	for(i=0; i<2 ;i++)
		DCRWritePriv(_DDR_MC_MCZMRINT(i),
		_BN(32) |	// Enable Refresh
		_B10(43,(unsigned)(7800/ddr.TCK/32/(float)num_ranks/8 - 1)) |
		_B7(50,(unsigned)(260)/(unsigned)(ddr.TCK*4) + 0));	// Ru[tRFC/tCK/4]-1

	Delay(100000000);

	for(i=0; i<2 ;i++)
		DCRWritePriv(_DDR_MC_MCZMRINT(i),0);
#endif

if(!ddr.SKIP_RDCAL && !ddr.SKIP_ALLCAL) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 12. Initial Read calibration\n\n"));

	DDR_Init_Read_Cal(num_ranks, &ddr, MC_IOM, 1);
}
else {
	for(i=0; i<4 ;i++)
		DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B16(47,0x7FF0) | _B16(63,ddr.DQSGATE_DELAY));
}

#if 1
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 16A.MCZMRINT\n\n"));

	for(i=0; i<2 ;i++)
		DCRWritePriv(_DDR_MC_MCZMRINT(i),
		_BN(32) |	// Enable Refresh
		_B10(43,(unsigned)(7800/ddr.TCK/32/(float)num_ranks/8 - 1)) |
		_B7(50,(unsigned)(260)/(unsigned)(ddr.TCK*4) + 0));	// Ru[tRFC/tCK/4]-1
#endif

if(!ddr.SKIP_ALLCAL) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 12A.Master_Control_Error_Register check\n\n"));

	DDR_Init_MasterCtrlErrReg(&ddr, MC_IOM, 0);

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 14. Re-run VDL calibration (CPC)\n\n"));

	DDR_Init_CPC(&ddr, MC_IOM, 1);

    if(ddr.DENSITY_AUTO_DETECT) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 14A.Detect Density\n\n"));

	DDR_Init_Detect_Density(&dram_density, num_ranks, &ddr, ODT, CWL_wrdata_pat, rddata);
    }
    else
    {
	dram_density = FW_Personality.DDR_Config.DDRSizeMB/num_ranks/4;
	DDR_Init_Misc(num_ranks, &dram_density, &ddr);
	DDR_Init_MCS(num_ranks, dram_density);
	DDR_Init_MCA(num_ranks, dram_density, &ddr, ODT);
    }

#if 0
    if(!ddr.SKIP_MEMCAL) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 15. Enable periodic calibrations\n\n"));


	for(i=0; i<4 ;i++)
	//	DCRWritePriv(_DDR_MC_IOM_CAL_CNTL(0) + MC_IOM[i], _BN(51) | _BN(52) | _BN(53) | _BN(54) | _BN(55));
	//	DCRWritePriv(_DDR_MC_IOM_CAL_CNTL(0) + MC_IOM[i], _BN(53) | _BN(54) | _BN(55));		// <Data> CPC should not use periodic cal -- DD1 bug
		DCRWritePriv(_DDR_MC_IOM_CAL_CNTL(0) + MC_IOM[i], _BN(54) | _BN(55));			// Periodic RDCAL duration was not clear
    }
#endif
}

#if 1
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 16B.MCZMRINT\n\n"));


	for(i=0; i<2 ;i++)
		DCRWritePriv(_DDR_MC_MCZMRINT(i),
		_B1(0,ddr.SKIP_ZQCAL?0:1) |	// Enable periodic ZQ cal
		_B7(7,127) |	// ZQ interval = 128*256*7.8us = 256ms
		_B1(8,0) |      // ZQCS enabled (0: Short / 1: Long)
		_B7(15,1) |	// tZQCS = 64tCK
	//	_B1(16,ddr.SKIP_MEMCAL?0:1) |	// Enable MEMCAL
	//	_B7(23,127) |	// MEMCAL interval = 128*256*7.8us = 256ms
	//	_B7(31,2) |	// tMEMCALoper = 384 clocks
		_BN(32) |	// Enable Refresh
		_B10(43,(unsigned)(7800/ddr.TCK/32/(float)num_ranks/ddr.REFRESH_RATE - 1)) |
		_B7(50,(unsigned)(dram_density<=1024?110:(dram_density==2048?160:(dram_density==4096?260:350)))/(unsigned)(ddr.TCK*4) + 0) |	// Ru[tRFC/tCK/4]-1
	//	_B6(61,0) |	// Write command guard time = ?
		0);
#endif


/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 19. Load Final MCA settings\n\n"));

//	val = DCRReadPriv(_DDR_MC_MCADTAL(0));
//	val &= ~(_B8(23,0xFF));
//	val |=  _B8(23,ddr.ADDR_HASH_DISABLE? 0:ddr.WLAT+8);	     // For DD2 and later, if address hashing is desired (recommended) then load MCADTAL(16:23) now
//
//	for(i=0;i<2;i++)
//		DCRWritePriv(_DDR_MC_MCADTAL(i), val);

	val = DCRReadPriv(_DDR_MC_MCACFG1(0));
	val &= ~(_B1(24,1));
	val |=  _B1(24,ddr.PD_DISABLE);	

	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCACFG1(i), val);


/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 20A.DRAM & PBUS ECC Setting\n\n"));

	if(!ddr.DRAM_ECC_DISABLE)
	{
		if(!ddr.PBUS_ECC_DISABLE)
		{
			MC_DEBUG(("DRAM ECC ENABLED, PBUS ECC ENABLED\n\n"));
			val =  _B6(9,0x00) | _BN(25);
		}
		else
		{
			MC_DEBUG(("DRAM ECC ENABLED, PBUS ECC DISABLED\n\n"));
			val =  _B6(9,0x3C) | _BN(25);
		}
	}

	else
	{
		if(!ddr.PBUS_ECC_DISABLE)
		{
			MC_DEBUG(("DRAM ECC DISABLED, PBUS ECC ENABLED\n\n"));
			val =  _B6(9,0x03) | _BN(25);
		}
		else
		{
			MC_DEBUG(("DRAM ECC DISABLED, PBUS ECC DISABLED\n\n"));
			val =  _B6(9,0x3F) | _BN(25);
		}
	}

	if(!ddr.DATA_INV_DISABLE)
		val |= _BN(24);		// data inversion enable
	val |= _BN(25);			// x8 mode
//	val |= _BN(26);			// single wire mode
//	val |= _BN(27) | _BN(29);	// disable slow decoder

	for(i=0; i<2 ;i++)
		DCRWritePriv(_DDR_MC_MCECCDIS(i), val);


//if(!ddr.BG_SCRUB_DISABLE) {
///*----------------------------------------------------------------------------------------------------------------------------------------*/
//MC_DEBUG(("Step 21B.Background Memory Scrubbing Setting\n\n"));
//
//	for(i=0; i<2 ;i++)
//	{
//		DCRWritePriv(_DDR_MC_MCMACA(i), 0);
//		DCRWritePriv(_DDR_MC_MCMEA(i), _B3(3,num_ranks-1) | _B3(10,7) | _B16(26,MAX_ROW_ADDR(dram_density)) | _B9(35,0x3FF>>3)); 
//		DCRWritePriv(_DDR_MC_MCSCTL(i), _BN(5) | _BN(6) | _BN(19) | _B12(31,ddr.BG_SCRUB_RATE));
//		DCRWritePriv(_DDR_MC_MCMCT(i), _BN(0) | _BN(1) | _BN(3) | _BN(5) | _BN(7));
//		Mem_reserve_readbuffer(i);
//		DCRWritePriv(_DDR_MC_MCMCC(i), _BN(0) | _BN(4) | _BN(5));
//	}
//}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 22. MCFIR\n\n"));

	for(i=0; i<2 ;i++)
	{
	//	DCRWritePriv(_DDR_MC_MCSCTL(i), _BN(5));	// Clear Scrub Counter and Syndrome

		DCRWritePriv(_DDR_MC_MCFIRC(i), 0);	// Clear FIR bits

		DCRWritePriv(_DDR_MC_MCFIRM(i), -1);	// Mask all FIR bits
		DCRWritePriv(_DDR_MC_MCFIRMS(i), -1);	// Mask all FIR bits

		DCRWritePriv(_DDR_MC_MCSPAAND(i), 0);
		DCRWritePriv(_DDR_MC_MCSPAMSK(i), -1);

		DCRWritePriv(_DDR_MC_MCECTL(i), _B2(1,3));	// Enable Error Logging
	}

	DCRWritePriv(DR_ARB_DCR__A( L2_INTERRUPT_STATE__STATE )+0x0000, -1);	// Clear DR_ARB bits
	DCRWritePriv(DR_ARB_DCR__A( L2_INTERRUPT_STATE__STATE )+0x4000, -1);	// Clear DR_ARB bits


/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 23. Performance Monitor\n\n"));

	for(i=0; i<2 ;i++)
	{
		DCRWritePriv(_DDR_MC_MCMCC(i), _B5(12,13) | _B5(17,14) | _B5(22,15) | _B5(27,16) | _B5(32,18) | _B5(37,19) | _B5(42,20) | _B5(47,29) | _B8(55,0xFF) | _B8(63,0xFF));

		DCRWritePriv(_DDR_MC_MCAPERFMONC1(i), _B5(36,0)  | _B5(41,4) | _B5(46,8) | _B5(51,9) | _B5(56,10) | _B5(61,12));
		DCRWritePriv(_DDR_MC_MCAPERFMONC2(i), _B5(36,13) | _B5(41,14));
		DCRWritePriv(_DDR_MC_MCAPERFMONC0(i), _B8(63,0xFF));
	}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("End of DDR Resume Sequence\n\n"));
return 0;

}
