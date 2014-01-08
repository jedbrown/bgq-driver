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

#define	CWL_OPT_CENTER	0
#define	NUM_CAL_REPEAT	10
#define	NUM_CAL_RETRY	4
#define	NUM_DD_RETRY	4
#define	NUM_SB_RETRY	6
#define	DDR_FW_ver	"08-23-2012"
#define	INIT_LOOP	1
#define	NEW_REFRESH	1
#define	REPRO		1


int DDR_InitSequence(struct DDRINIT_metrics ddr, int shmoo)
{
	unsigned	num_ranks=1, dram_density=1024, dram_speed;
	unsigned	i=0, caltry=0, ddtry=0, sbtry=0, rr, cal_fail_count=0;
	unsigned	MC_IOM[4] = {0x0, 0x4000, 0x40, 0x4040};
	unsigned	ODT[4];
	unsigned	ADDR_DELAY, ADDR_DELAY_step=0x04;
	unsigned	j, local_pass=0, smb_pass=1;
	char		ecc_on, correctable=0;
	uint64_t	cal_fail_chips=0;
	uint64_t	val=0, smb_err_L=0, smb_err_H=0;
	uint64_t	rddata[17];
	uint64_t	CWL_wrdata_pat[3][17] = {
	      {	0x0000000000000000, 0x1111111111111111, 0x2222222222222222, 0x3333333333333333,
		0x4444444444444444, 0x5555555555555555, 0x6666666666666666, 0x7777777777777777,
		0x8888888888888888, 0x9999999999999999, 0xAAAAAAAAAAAAAAAA, 0xBBBBBBBBBBBBBBBB,
		0xCCCCCCCCCCCCCCCC, 0xDDDDDDDDDDDDDDDD, 0xEEEEEEEEEEEEEEEE, 0xFFFFFFFFFFFFFFFF,
		0x00B83048AC86608A  },
	      {	0xFFFFFFFFFFFFFFFF, 0xEEEEEEEEEEEEEEEE, 0xDDDDDDDDDDDDDDDD, 0xCCCCCCCCCCCCCCCC,
		0xBBBBBBBBBBBBBBBB, 0xAAAAAAAAAAAAAAAA, 0x9999999999999999, 0x8888888888888888,
		0x7777777777777777, 0x6666666666666666, 0x5555555555555555, 0x4444444444444444,
		0x3333333333333333, 0x2222222222222222, 0x1111111111111111, 0x0000000000000000,
		0x00B78F1E958DDBE5  },
	      {	0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
		0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
		0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
		0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
		0x0000000000000000  },
	      };

unsigned	loop=0;
while(loop++<INIT_LOOP)
{
	caltry=0, ddtry=0, sbtry=0;
	smb_pass=1;

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 1. Disable Refresh, Periodic Cal and ECC\n\n"));

	for(i=0; i<2 ;i++)
	{
		DCRWritePriv(_DDR_MC_MCZMRINT(i), 0);
		DCRWritePriv(_DDR_MC_MCECCDIS(i), _B6(9,0x3F));
		ecc_on=0;

	// Reset Marking Store Data as Moyra has found Non zero marking registers
		if(!TI_isDD1())
			for(j=0; j<8 ;j++)
				DCRWritePriv(_DDR_MC_MRKSTDTA0(i)+j, 0);
	}

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 2. Detect Frequency\n\n"));

	DDR_Init_Freq(&dram_speed);

if(!shmoo)
{
	#include <hwi/include/bqc/DDR_parms.h>
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 3. Configure Misc\n\n"));

	DDR_Init_Misc(num_ranks, &dram_density, &ddr);

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 4. Load MCS registers\n\n"));

	DDR_Init_MCS(num_ranks, dram_density);

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 5. Load MCA registers\n\n"));

	DDR_Init_MCA(num_ranks, dram_density, &ddr, ODT);

STEP6:
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 6. Load IOM registers\n\n"));

	DDR_Init_IOM(&ddr, MC_IOM);

if(!ddr.FAST) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/ 
MC_DEBUG(("Step 7. Adjust ADDR delay\n\n"));

	DDR_Init_ADDR_Delay(MC_IOM, ddr.ADDR_DELAY, num_ranks==1? 0:ddr.TIS_SHIFT);
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 8. Reset DRAM & Assert CKE\n\n"));

	DDR_Init_Reset_DRAM(&ddr);

if(!ddr.SKIP_ALLCAL) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/ 
MC_DEBUG(("Step 9. Initial VDL calibration (CPC)\n\n"));

	DDR_Init_CPC(&ddr, MC_IOM, REPRO);
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 10. Program MRS\n\n"));

	DDR_Init_MRS(&ddr);

if(!ddr.SKIP_ALLCAL && ddr.RANK_AUTO_DETECT && ddtry==0 && sbtry==0) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 11A.Detect Ranks and Fine Write Leveling\n\n"));

	for(i=0; i<4 ;i++)
	{
		val = DDR_Init_Detect_Ranks(&num_ranks, dram_density, &ddr, MC_IOM, ODT, REPRO);
		if(num_ranks !=0)
			break;
		if(i>=1 && (ddr.IMPEDANCE & 0x300) != 0)
			ddr.IMPEDANCE -= 0x100;
	}
	if(num_ranks==0)
	{
	    fw_uint64_t details[1];
	    details[0] = FW_RAS_DDR_INIT_RANK_DETECTION_FAILED;
	    fw_writeRASEvent( FW_RAS_DDR_INIT_ERROR, 1, details );	// ==> FATAL
	    //printf("ERROR: DDR RANK DETECTION FAILED. FORCING TO 1-RANK\n");
	    num_ranks = 1;
	}
	else
	{
	    if((i > 0) && PERS_ENABLED(PERS_ENABLE_DiagnosticsMode)) {
		fw_uint64_t details[2];
		details[0] = FW_RAS_DDR_INIT_RANK_DETECTION_RETRIED;
		details[1] = i;
		fw_writeRASEvent( FW_RAS_DDR_INIT_INFO, 2, details );	// ==> INFO
		//printf("WARNING: DDR RANK DETECTION SUCCEEDED AFTER %d RETR%s\n",i,(i==1)?"Y":"IES");
	    }
	    if(val > 0) {
		MC_DEBUG2(("%d failed Bytes were ignored during DDR rank detection", (int)val));
	    }
	}

	if(caltry==0 && ddtry==0 && sbtry==0)
		ddr.IMPEDANCE = ddr.IMP_A[(num_ranks>>1) + (dram_speed==1600? 3:0)];

	ddr.RTT = ddr.RTT_A[(num_ranks>>1) + (dram_speed==1600? 3:0)];
	ddr.RON = ddr.RON_A[(num_ranks>>1) + (dram_speed==1600? 3:0)];

	if(!shmoo)
	{
		DDR_Init_IOM(&ddr, MC_IOM);
		DDR_Init_ADDR_Delay(MC_IOM, ddr.ADDR_DELAY, num_ranks==1? 0:ddr.TIS_SHIFT);
		DDR_Init_Reset_DRAM(&ddr);
		DDR_Init_CPC(&ddr, MC_IOM, REPRO);
		DDR_Init_MRS(&ddr);
		DDR_Init_FWL(num_ranks, &ddr, MC_IOM, ODT, REPRO);
	}
}

if(!ddr.SKIP_ALLCAL && !ddr.RANK_AUTO_DETECT) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 11B.Fine Write Leveling\n\n"));

	DDR_Init_FWL(num_ranks, &ddr, MC_IOM, ODT, REPRO);
}

if(!ddr.SKIP_RDCAL && !ddr.SKIP_ALLCAL) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 12. Initial Read calibration\n\n"));

	cal_fail_count=0;
	for(i=0; i<NUM_CAL_REPEAT; i++)
	{
		DDR_Init_Read_Cal(num_ranks, &ddr, MC_IOM, REPRO);
		cal_fail_count += (DDR_Init_MasterCtrlErrReg(&ddr, MC_IOM, 0)!=0)? 1:0;
	}
}
else {
	for(i=0; i<4 ;i++)
		DCRWritePriv(_DDR_MC_IOM_PHYWRITE(0) + MC_IOM[i], _B16(47,0x7FF0) | _B16(63,ddr.DQSGATE_DELAY));
}

if(!ddr.SKIP_ALLCAL) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 12A.Master_Control_Error_Register check\n\n"));

	if(cal_fail_count > 0)
	{
		MC_DEBUG2(("%d out of %d calibrations failed", cal_fail_count, NUM_CAL_REPEAT));
		if(caltry < NUM_CAL_RETRY)
		{
			caltry++;
			ddr.RANK_AUTO_DETECT = 0;
			goto STEP6;
		}
		else
			cal_fail_chips = DDR_Cal_Error_Recovery(num_ranks, MC_IOM);
	}


    if(!ddr.SKIP_CWL) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 13. Coarse Write Leveling\n\n"));

#if CWL_OPT_CENTER
	unsigned	ADDR_DELAY_FP=0, ADDR_DELAY_PF=0, global_pass=0;

	for(ADDR_DELAY=0; ADDR_DELAY<0x200; ADDR_DELAY+=ADDR_DELAY_step)
#else
	for(ADDR_DELAY=ddr.ADDR_DELAY_START; ADDR_DELAY<=ddr.ADDR_DELAY_START+0x200; ADDR_DELAY+=ADDR_DELAY_step)
#endif
	{
		if(ADDR_DELAY!=ddr.ADDR_DELAY_START || ddr.ADDR_DELAY!=ddr.ADDR_DELAY_START)
		{
			DDR_Init_ADDR_Delay(MC_IOM, ADDR_DELAY%0x200, num_ranks==1? 0:ddr.TIS_SHIFT);
			DDR_Init_Reset_DRAM(&ddr);
			DDR_Init_MRS(&ddr);
			DDR_Init_FWL(num_ranks, &ddr, MC_IOM, ODT, REPRO);
			if(!ddr.SKIP_RDCAL)
				DDR_Init_Read_Cal(num_ranks, &ddr, MC_IOM, REPRO);
		}
	//	for(i=0;i<2;i++)
	//	{
	//		DCRWritePriv(_DDR_MC_MCAPOS(i), 0x000019FE1C000000);
	//		DCRWritePriv(_DDR_MC_MCAPOS(i), 0x00000001FC000000);
	//	}
		for(local_pass=1,i=0;i<2;i++)
		{
			for(rr=0;rr<num_ranks;rr++)
			{
				MEM_Alter(i, rr, CWL_wrdata_pat[1]);
				for(j=0;j<2;j++)
				{
					MEM_Alter(i, rr, CWL_wrdata_pat[j]);
					MEM_Display(i, rr, rddata);
					local_pass &= Mem_Compare(CWL_wrdata_pat[j], rddata);
#if FW_DEBUG
					char msg[30] = "MC[x] rank[y] pat[z] got:";
					msg[3]=i+'0';
					msg[11]=rr+'0';
					msg[18]=j+'0';
					Mem_print_vector(rddata, msg, 17);
#endif
				}
			}
		}
		MC_DEBUG(("ADDR_DELAY = 0x%x: %s\n", ADDR_DELAY%0x200, local_pass? "PASS":"FAIL"));
#if CWL_OPT_CENTER
		if(global_pass==0 && local_pass==1)
		{
			ADDR_DELAY_FP = ADDR_DELAY;
			global_pass = 1;
		}
		if(global_pass==1 && local_pass==0)
		{
			ADDR_DELAY_PF = ADDR_DELAY;
			break;
		}
	}
	ADDR_DELAY = (ADDR_DELAY_FP + ADDR_DELAY_PF) / 2;
	MC_DEBUG(("ADDR_DELAY_FP = 0x%x, ADDR_DELAY_PF = 0x%x, setting ADDR_DELAY = 0x%x\n", ADDR_DELAY_FP, ADDR_DELAY_PF, ADDR_DELAY));
	DDR_Init_ADDR_Delay(MC_IOM, ADDR_DELAY%0x200, num_ranks==1? 0:ddr.TIS_SHIFT, REPRO);
	DDR_Init_Reset_DRAM(&ddr);
	DDR_Init_MRS(&ddr);
	DDR_Init_FWL(num_ranks, &ddr, MC_IOM, ODT, REPRO);
	if(!ddr.SKIP_RDCAL)
		DDR_Init_Read_Cal(num_ranks, &ddr, MC_IOM, REPRO);
	for(i=0;i<2;i++)
	{
		DCRWritePriv(_DDR_MC_MCAPOS(i), 0x000019FE1C000000);
		DCRWritePriv(_DDR_MC_MCAPOS(i), 0x00000001FC000000);
	}
	for(i=0;i<2;i++)
		for(rr=0;rr<num_ranks;rr++)
			MEM_Alter(i, rr, CWL_wrdata_pat[1]);
#else
		if(local_pass==1)
			break;
	}
	if(local_pass==0)
	{
		ADDR_DELAY=ddr.ADDR_DELAY;
		MC_DEBUG2(("ERROR: CWL failed. Forcing ADDR_DELAY to default value (%X)\n",ADDR_DELAY));	// 1-30-2012
	}
	else if(ADDR_DELAY!=ddr.ADDR_DELAY_START)
		MC_DEBUG2(("WARNING: CWL adjusted ADDR_DELAY to %X\n",ADDR_DELAY));		// 1-30-2012
#endif
    }

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 14. Re-run VDL calibration (CPC)\n\n"));

	DDR_Init_CPC(&ddr, MC_IOM, REPRO);

    if(ecc_on==0 && ddr.DENSITY_AUTO_DETECT) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 14A.Detect Density\n\n"));

	val = DDR_Init_Detect_Density(&dram_density, num_ranks, &ddr, ODT, CWL_wrdata_pat, rddata);
//	if(dram_density == 256)
	if(dram_density < 1024)
	{
		if(ddtry < NUM_DD_RETRY)
		{
			if((ddr.IMPEDANCE & 0x300) != 0)
				ddr.IMPEDANCE -= 0x100;
		//	else if((ddr.IMPEDANCE & 0x003) > 0x1)
		//		ddr.IMPEDANCE -= 0x001;
			caltry=0;
			ddtry++;
			MC_DEBUG2(("Density Detection Retrying with Impedace = 0x%lx",ddr.IMPEDANCE));
			goto STEP6;
		}

		fw_uint64_t details[2];
		details[0] = FW_RAS_DDR_INIT_DENSITY_DETECTION_FAILED;
	  	details[1] = ddtry;
		fw_writeRASEvent( FW_RAS_DDR_INIT_ERROR, 2, details );	// ==> FATAL
		// printf("ERROR: DDR DENSITY DETECTION FAILED. FORCING TO 1Gb\n");

		dram_density = 1024;
		ddr.SKIP_CWL = 1;
		ddr.SKIP_SMBTEST = 1;
	}
	else {
	    if ((ddtry > 0) && PERS_ENABLED(PERS_ENABLE_DiagnosticsMode)) {
		fw_uint64_t details[2];
		details[0] = FW_RAS_DDR_INIT_VTT_DD;	
		details[1] = ddtry;
		fw_writeRASEvent( FW_RAS_DDR_INIT_INFO, 2, details );	// ==> INFO
	    }
	    if(val > 0) {
		MC_DEBUG2(("%d failed DQs were ignored during DDR density detection", (int)val));
	    }
	}
    }
    else
    {
	dram_density = FW_Personality.DDR_Config.DDRSizeMB/num_ranks/4;
	DDR_Init_Misc(num_ranks, &dram_density, &ddr);
	DDR_Init_MCS(num_ranks, dram_density);
	DDR_Init_MCA(num_ranks, dram_density, &ddr, ODT);
    }

    if(!ddr.SKIP_MEMCAL) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 15. Enable periodic calibrations\n\n"));

	for(i=0; i<4 ;i++)
	{
		//Periodic RDCAL (bit53) has a bug in DD1 and DD2
		DCRWritePriv(_DDR_MC_IOM_CAL_CNTL(0) + MC_IOM[i], _B1(51,ddr.MEMCAL_DATACPC) | _B1(52,ddr.MEMCAL_ADDRCPC) | _B1(54,ddr.MEMCAL_DESKEW) | _B1(55,ddr.MEMCAL_RDDLY));
	}
    }
}

if ( FW_IS_NODE_0 && sbtry==0 ) {
    printf(
	"DDR3 %dMbps, %d%sb SDRAM, %d-Rank%s %dGByte, DD%d%s, PD_%s, BackScrub=%s DirScrub=%s DDR_FW:%s\n", 
	dram_speed, 
	(dram_density>512)?dram_density/1024:dram_density, (dram_density>512)?"G":"M", 
	num_ranks, (num_ranks==1)?",":"s,", 4*dram_density*num_ranks/1024, 
	TI_isDD1()?1:2, FW_DD1_WORKAROUNDS_ENABLED()?"(*)":"", 
	ddr.PD_DISABLE?"Off":(ddr.PD_FAST?"Fast":"Slow"), 
	(ddr.BG_SCRUB_DISABLE?"Off":"On"),
	PERS_ENABLED(PERS_ENABLE_DDRDirScrub)?"On":"Off",
	DDR_FW_ver);
}

if ( FW_Personality.DDR_Config.DDRSizeMB != 4*dram_density*num_ranks )
  {
      fw_uint32_t event_id;

      fw_uint64_t details[2];
      details[0] = 4*dram_density*num_ranks;
      details[1] = FW_Personality.DDR_Config.DDRSizeMB;

      if ( ( FW_Personality.Kernel_Config.NodeConfig & PERS_ENABLE_DDRAutoSize ) != 0 ) {
	  event_id = FW_DDR_AUTO_DETECTION_SIZE_MISMATCH;
	  FW_Personality.DDR_Config.DDRSizeMB = 4*dram_density*num_ranks;
      }
      else if ( FW_Personality.DDR_Config.DDRSizeMB < 4*dram_density*num_ranks ) {
	  event_id = FW_DDR_SIZE_MISMATCH_WARNING;
      }
      else { 
	  event_id = FW_DDR_SIZE_MISMATCH_ERROR;
	  FW_Personality.DDR_Config.DDRSizeMB = 4*dram_density*num_ranks; // retain the larger size, which preserves old behavior
      }

      fw_writeRASEvent( event_id, 2, details );
  }

#if NEW_REFRESH

if(!ddr.SKIP_MCZMRINT) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 16. MCZMRINT\n\n"));

	unsigned proc_freq;

	switch((DCRReadPriv(TESTINT_DCR(CLKTREE_CTRL18)) >> (63-13)) & 0x3)
	{
		case 0: proc_freq = 1600; break;
		case 1: proc_freq =  800; break;
		case 2: proc_freq = 1200; break;
		case 3: proc_freq = 2000; break;
	}

	unsigned coord = FW_Personality.Network_Config.Acoord * 16
		       + FW_Personality.Network_Config.Bcoord * 8
		       + FW_Personality.Network_Config.Ccoord * 4
		       + FW_Personality.Network_Config.Dcoord * 2
		       + FW_Personality.Network_Config.Ecoord * 1;

	float board_fraction;

	switch((coord/32)%16)
	{
		case  0: board_fraction =  0/16; break;
		case  1: board_fraction =  8/16; break;
		case  2: board_fraction =  4/16; break;
		case  3: board_fraction = 12/16; break;
		case  4: board_fraction =  2/16; break;
		case  5: board_fraction = 10/16; break;
		case  6: board_fraction =  6/16; break;
		case  7: board_fraction = 14/16; break;
		case  8: board_fraction =  1/16; break;
		case  9: board_fraction =  9/16; break;
		case 10: board_fraction =  5/16; break;
		case 11: board_fraction = 13/16; break;
		case 12: board_fraction =  3/16; break;
		case 13: board_fraction = 11/16; break;
		case 14: board_fraction =  7/16; break;
		case 15: board_fraction = 15/16; break;
	}

	unsigned ref_time_base = 7800 * (float)proc_freq / 1000 / (float)num_ranks / ddr.REFRESH_RATE / 2;      // 2 -> two MC
	unsigned ref_base = 7800/ddr.TCK/32/(float)num_ranks/ddr.REFRESH_RATE;
	unsigned nd_delay = ( ref_time_base*(coord%32) + ref_time_base*board_fraction ) / 32;
	unsigned rank_delay = ref_time_base - ddr.REFRESH_SCOM_DELAY*proc_freq/1000;

	DelayTimeBase(nd_delay);	// distributes Refresh operations over 32 nodes & node boards
	MC_DEBUG(("ND_DELAY = %d\n",nd_delay));
	MC_DEBUG(("REF_PER = %d\n",ref_base + (coord%(unsigned)(ref_base*ddr.REFRESH_RATE/10+1)) - 1));

	val =	_B1(0,ddr.SKIP_ZQCAL?0:1) |		// Enable periodic ZQ cal
		_B7(7,127) |				// ZQ interval = 128*256*7.8us = 256ms
		_B1(8,0) |				// ZQCS enabled (0: Short / 1: Long)
		_B7(15,1) |				// tZQCS = 64tCK
		_B1(16,ddr.SKIP_MEMCAL?0:1) |		// Enable MEMCAL
		_B7(23,(ddr.MEMCAL_PERIOD>128? ddr.MEMCAL_PERIOD/128 : ddr.MEMCAL_PERIOD) - 1) |	// 01-30-2012
		_B1(24,ddr.MEMCAL_MPR_DISABLE) |	// MEMCAL MPR disable				// 02-02-2012
		_B7(31,ddr.MEMCAL_OPER-1) |		// 01-30-2012
		_BN(32) |				// Enable Refresh
		_B10(43,ref_base - 1) |
		_B7(50,(dram_density<=1024?110:(dram_density==2048?160:(dram_density==4096?260:350)))/(unsigned)(ddr.TCK*4) + 0) |	// Ru[tRFC/tCK/4]-1
		_B6(61,0x40) |				// 01-30-2012
		0;

	for(i=0; i<2 ;i++)
	{
		DCRWritePriv(_DDR_MC_MCZMRINT(i), val);
	//      DelayTimeBase(3744);    // wait for 2.34us => MC0 to MC1 gap = 2.34us + 1.2us(SCOM delay) = 3.54us = 7.08us/2
	//      DelayTimeBase(912);     // wait for 0.57us => MC0 to MC1 gap = 0.57us + 1.2us(SCOM delay) = 1.77us = 7.08us/4
		DelayTimeBase(rank_delay);
	}

	DelayTimeBase(ref_time_base*32 - nd_delay);  // resyn
}

#else

if(!ddr.SKIP_MCZMRINT) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 16. MCZMRINT\n\n"));

        unsigned ref_base = 7800/ddr.TCK/32/(float)num_ranks/ddr.REFRESH_RATE;
        unsigned coord  = FW_Personality.Network_Config.Acoord * 16
                        + FW_Personality.Network_Config.Bcoord * 8
                        + FW_Personality.Network_Config.Ccoord * 4
                        + FW_Personality.Network_Config.Dcoord * 2
                        + FW_Personality.Network_Config.Ecoord * 1;
        unsigned nd_delay = ref_base*(coord%32) + ref_base*(coord/32)/8;

        Delay(nd_delay);        // distributes Refresh operations over 32 nodes & node boards
        MC_DEBUG(("ND_DELAY = %d\n",nd_delay));
        MC_DEBUG(("REF_PER = %d\n",ref_base + (coord%(unsigned)(ref_base*ddr.REFRESH_RATE/10+1)) - 1));

        for(i=0; i<2 ;i++)
        {
                DCRWritePriv(_DDR_MC_MCZMRINT(i),
                _B1(0,ddr.SKIP_ZQCAL?0:1) |     // Enable periodic ZQ cal
                _B7(7,127) |    // ZQ interval = 128*256*7.8us = 256ms
                _B1(8,0) |      // ZQCS enabled (0: Short / 1: Long)
                _B7(15,1) |     // tZQCS = 64tCK
                _B1(16,ddr.SKIP_MEMCAL?0:1) |      // Enable MEMCAL
                _B7(23,127) |   // MEMCAL interval = 128*256*7.8us = 256ms
                _B7(31,2) |     // tMEMCALoper = 384 clocks
                _BN(32) |       // Enable Refresh
                _B10(43,ref_base + (coord%(unsigned)(ref_base*ddr.REFRESH_RATE/10+1)) - 1) |
                _B7(50,(dram_density<=1024?110:(dram_density==2048?160:(dram_density==4096?260:350)))/(unsigned)(ddr.TCK*4) + 0) |      // Ru[tRFC/tCK/4]-1
                _B6(61,0) |     // Write command guard time = 0 (parity is not used)
                0);
        //      DelayTimeBase(3744);    // wait for 2.34us => MC0 to MC1 gap = 2.34us + 1.2us(SCOM delay) = 3.54us = 7.08us/2
                DelayTimeBase(912);     // wait for 0.57us => MC0 to MC1 gap = 0.57us + 1.2us(SCOM delay) = 1.77us = 7.08us/4
        }
}

#endif

if(!ddr.SKIP_SMBTEST) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 17. Small Block Test\n\n"));


	MC_DEBUG(("Writing...\n"));
	for(val=0; val<MAX_ROW_ADDR(dram_density); val++)
		for(rr=0; rr<num_ranks ;rr++)
			for(i=0; i<2 ;i++)
				MEM_Alter_addr(i, rr, val&0x7, val, val&0xFF8, CWL_wrdata_pat[(2*val+rr)%3]);
	MC_DEBUG(("Reading...\n"));
	uint64_t FpatL[2][2]={{0,0},{0,0}};
	uint64_t FpatH[2][2]={{0,0},{0,0}};
	uint64_t FpatE[2][2]={{0,0},{0,0}};
	for(local_pass=1,val=0; val<MAX_ROW_ADDR(dram_density) && local_pass; val++)
		for(rr=0; rr<num_ranks; rr++)
			for(i=0; i<2; i++)
			{
				MEM_Display_addr(i, rr, val&0x7, val, val&0xFF8, rddata);
				local_pass &= Mem_Compare(CWL_wrdata_pat[(2*val+rr)%3], rddata);
				FpatL[i][rr] |= Mem_Compare_8L(CWL_wrdata_pat[(2*val+rr)%3], rddata);
				FpatH[i][rr] |= Mem_Compare_8H(CWL_wrdata_pat[(2*val+rr)%3], rddata);
				FpatE[i][rr] |= CWL_wrdata_pat[(2*val+rr)%3][16] ^ rddata[16];
			}

	if(local_pass!=1)
	{
		int mismatch[2][2]={{0,0},{0,0}};
		for(i=0; i<2; i++)
		{
			for(rr=0; rr<num_ranks; rr++)
			{
				for(j=0; j<64; j++)
					mismatch[i][rr] += (FpatL[i][rr] >> j) & 0x1;
				for(j=0; j<64; j++)
					mismatch[i][rr] += (FpatH[i][rr] >> j) & 0x1;
				for(j=0; j<16; j++)
					mismatch[i][rr] += (FpatE[i][rr] & _B8((j/2)*8 + 7, 0xAA >> (j%2)))!=0? 1:0;
				
				Mem_ErrPatDecode(&FpatE[i][rr]);
			}
		}

		if(ecc_on == 0)
		{
			int chip;
			int err_symbols;
			int marked_chip_err_symbols=0;
			int rank_err_symbols;
			correctable=1;
			for(i=0; i<2; i++)
			{
				for(rr=0; rr<num_ranks; rr++)
				{
					ddr.MRKCHIP[i][rr] = 0;
					DCRWritePriv(_DDR_MC_MRKSTDTA0(i)+rr, 0);
					rank_err_symbols=0;
					marked_chip_err_symbols=0;
					for(chip=0; chip<18; chip++)
					{
						err_symbols=0;
						for(j=0; j<4; j++)
						{
							if(chip<8)
								err_symbols += (FpatL[i][rr] & _B2((chip%9)*8+2*j+1, 0x3))!=0? 1:0;
							else if(chip>8 && chip<17)
								err_symbols += (FpatH[i][rr] & _B2((chip%9)*8+2*j+1, 0x3))!=0? 1:0;
							else //if(chip==8 || chip==17)
								err_symbols += (FpatE[i][rr] & _B8((chip/9)*32+8*j+7, 0xFF))!=0? 1:0;
						}
						if(err_symbols >= 2)
						{
							ddr.MRKCHIP[i][rr] |= (1<<chip);
							marked_chip_err_symbols = err_symbols;
							if(!TI_isDD1())
							{
								Mem_Marking(i,rr,chip);
							//	printf("Memory Controller Initialization Warning: MC%d Rank%d Chip%d was Marked by Firmware (%d symbol errors)"
							//		,i,rr,chip,err_symbols);	// ==> WARNING
							}
						}
						rank_err_symbols += err_symbols;

						if(chip<8)
							ddr.FailDQ[i][rr][chip] = (char)((FpatL[i][rr] >> (56-(chip%9)*8)) & 0xFF);
						else if(chip>8 && chip<17)
							ddr.FailDQ[i][rr][chip] = (char)((FpatH[i][rr] >> (56-(chip%9)*8)) & 0xFF);
						else
							ddr.FailDQ[i][rr][chip] = (char)((val >> (63-(chip==8? 7:15))) & 0xFF);
					}

					if(marked_chip_err_symbols >= 1 && rank_err_symbols >= marked_chip_err_symbols + 2)
					{
					//	printf("Memory Controller Initialization Warning: MC%d Rank%d has %d more symbol errors beside the marked chip, and is uncorrectable"
					//		,i,rr,(rank_err_symbols - marked_chip_err_symbols));
						correctable=0;
					}
					else if(marked_chip_err_symbols == 0 && rank_err_symbols >= 3)
					{
					//	printf("Memory Controller Initialization Warning: MC%d Rank%d has %d symbol errors all on different chips, and is uncorrectable"
					//		,i,rr,rank_err_symbols);
						correctable=0;
					}
				}
			}
			smb_err_L = FpatL[0][0] | FpatL[0][1] | FpatL[1][0] | FpatL[1][1];
			smb_err_H = FpatH[0][0] | FpatH[0][1] | FpatH[1][0] | FpatH[1][1];
		}
		MC_DEBUG2(("Small Block Test (%d%s try) Mismatch: MC0_Rank0=%d MC0_Rank1=%d MC1_Rank0=%d MC1_Rank1=%d\n"
			,sbtry+1,sbtry>0?(sbtry>1?(sbtry>2?"th":"rd"):"nd"):"st",mismatch[0][0],mismatch[0][1],mismatch[1][0],mismatch[1][1]));
		for(i=0; i<2; i++)
		{
			for(rr=0; rr<num_ranks; rr++)
			{
				if(FpatL[i][rr]!=0 || FpatH[i][rr]!=0 || FpatE[i][rr]!=0)
					MC_DEBUG2(("Small Block Error Pattern: MC%d Rank%d: 0x%016lx-0x%016lx-0x%016lx\n",i,rr,FpatL[i][rr],FpatH[i][rr],FpatE[i][rr]));
			}
		}

		if(sbtry < NUM_SB_RETRY)
		{
			if(ecc_on == 0 && (correctable || sbtry == NUM_SB_RETRY-1))
			{
				for(i=0; i<2 ;i++)
					DCRWritePriv(_DDR_MC_MCECCDIS(i), _B2(25,0x1));
				ecc_on=1;
				MC_DEBUG2(("Retrying with ECC enabled\n"));
			}
			else
			{
				if((ddr.IMPEDANCE & 0x300) != 0)
					ddr.IMPEDANCE -= 0x100;
				else
				{
					if((ddr.IMPEDANCE & 0x003) > 0x1)
						ddr.IMPEDANCE -= 0x001;
					else if(ddr.RTT == 120)
						ddr.RTT = 60;
					else if((ddr.IMPEDANCE & 0x030) > 0x10)
						ddr.IMPEDANCE -= 0x010;
				}
				MC_DEBUG2(("Retrying with Impedance=0x%d%d%d Rtt=%d\n",(int)(ddr.IMPEDANCE>>8)&0x7,(int)(ddr.IMPEDANCE>>4)&0x7,(int)(ddr.IMPEDANCE>>0)&0x7,ddr.RTT));
			}
			for(i=0; i<4 ;i++)
				DCRWritePriv(_DDR_MC_IOM_CAL_CNTL(0) + MC_IOM[i], 0);	//Disable periodic calibration
			for(i=0; i<2 ;i++)
				DCRWritePriv(_DDR_MC_MCZMRINT(i), 0);	// Disable ZQcal, MEMCAL, Refresh
			caltry=0;
			ddtry=0;
			sbtry++;
			goto STEP6;
		}

		fw_uint64_t details[3];
		details[0] = FW_RAS_DDR_INIT_SMALL_BLOCK_TEST_FAILED;
		details[1] = smb_err_L;
		details[2] = smb_err_H;
		fw_writeRASEvent( FW_RAS_DDR_INIT_ERROR, 3, details );	// ==> FATAL
		//printf("ERROR: SMALL BLOCK TEST FAILED, %016lx-%016lx\n",FpatL,FpatH);
		smb_pass=0;
	}
	else
	{
		MC_DEBUG(("PASS\n"));
		if((sbtry > 0) && PERS_ENABLED(PERS_ENABLE_DiagnosticsMode)) {
		    fw_uint64_t details[7];
		    details[0] = FW_RAS_DDR_INIT_VTT_SB;
		    details[1] = sbtry;
		    details[2] = ecc_on;
		    details[3] = (ddr.IMPEDANCE>>8)&0x7;
		    details[4] = (ddr.IMPEDANCE>>4)&0x7;
		    details[5] = (ddr.IMPEDANCE>>0)&0x7;
		    details[6] = ddr.RTT;
		    fw_writeRASEvent( FW_RAS_DDR_INIT_INFO, 7, details ); // ==> INFO
		    //FW_Warning("Memory Controller Initialization Info: Small Block Test Passed after %d retr%s (ECC %s, Impedance=0x%d%d%d, Rtt=%d)"	// ==> INFO
			//	,sbtry,(sbtry>1)?"ies":"y",ecc_on?"enabled":"disabled",(int)(ddr.IMPEDANCE>>8)&0x7,(int)(ddr.IMPEDANCE>>4)&0x7,(int)(ddr.IMPEDANCE>>0)&0x7,ddr.RTT);
		}
	}
}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 17A.Initial Read Calibration Report\n\n"));

        if(cal_fail_count > 0)
	{
		fw_uint64_t details[6];
		details[0] = FW_RAS_DDR_INIT_IOM_CALIBRATION_FAILED;
		details[1] = caltry;
		details[2] = (int)(cal_fail_chips >> 48) & 0xFF;
		details[3] = (int)(cal_fail_chips >> 32) & 0xFF;
		details[4] = (int)(cal_fail_chips >> 16) & 0xFF;
		details[5] = (int)(cal_fail_chips >>  0) & 0xFF;
		fw_writeRASEvent( FW_RAS_DDR_INIT_WARNING, 6, details );	// ==> WARNING
		//printf("ERROR: DDR HW IOM CALIBRATION FAILED AFTER %d RETRIES\n",caltry);
                //FW_Warning("Memory Controller Initialization Info: IOM failed bytes: RdCal=%d RdDly=%d WrLvl=%d CPC=%d"
		//	,(int)(cal_fail_chips >> 48) & 0xFF,(int)(cal_fail_chips >> 32) & 0xFF,(int)(cal_fail_chips >> 16) & 0xFF,(int)(cal_fail_chips >>  0) & 0xFF);
	}
        else if((caltry > 0) && PERS_ENABLED(PERS_ENABLE_DiagnosticsMode)) {
		fw_uint64_t details[2];
		details[0] = FW_RAS_DDR_INIT_IOM_CALIBRATION_RETRIED;
		details[1] = caltry;
		fw_writeRASEvent( FW_RAS_DDR_INIT_INFO, 2, details );	// ==> INFO
		//printf("WARNING: HW IOM CALIBRATION SUCCEEDED AFTER %d RETR%s\n",caltry,(caltry==1)?"Y":"IES");
	}


if(!ddr.SKIP_WRCAL) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 17B.Initial Write Calibration\n\n"));

	int Vref_DQ;
	unsigned window_vref, window_max, vref_max=0;
	for(i=0; i<2; i++)
		DCRWritePriv(_DDR_MC_MCECCDIS(i), _B6(9,0x3F));
	for(i=0; i<2; i++)
	{
		for(window_max=0, Vref_DQ=0; Vref_DQ>=-4; Vref_DQ--)
		{
			window_vref = DDR_Init_Write_Cal(num_ranks, &ddr, i, Vref_DQ, 0, CWL_wrdata_pat[2], rddata);
			if(window_vref > window_max) {
				window_max = window_vref;
				vref_max = Vref_DQ;
			}
			else
				break;
		}
#if 0
		if(window_max < 190) {
		    fw_uint64_t details[4];
		    details[0] = FW_RAS_DDR_INIT_NARROW_WRITE_DATA_WINDOW;
		    details[1] = i;
		    details[2] = window_max;
		    details[3] = vref_max;
		    fw_writeRASEvent( FW_RAS_DDR_INIT_WARNING, 4, details );	// ==> WARN
		    //FW_Warning("MC%d has Narrow Write Data Window: window_max=%d vref_max=%d",i,window_max,vref_max);
		}
#endif
		DDR_Init_Write_Cal(num_ranks, &ddr, i, vref_max, 0, CWL_wrdata_pat[2], rddata);
	}
}


#if 0
if(!ddr.SKIP_ADDRCAL) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 17C.Initial Address Calibration\n\n"));

	val = DCRReadPriv(_DDR_MC_MCZMRINT(0));
	for(i=0; i<2 ;i++)
	{
		DCRWritePriv(_DDR_MC_MCZMRINT(i), 0);	// Disable ZQcal, MEMCAL, Refresh
		DCRWritePriv(_DDR_MC_MCECCDIS(i), _B6(9,0x3F));
	}
	unsigned window_max;
	for(i=0; i<2; i++)
	{
		DDR_Init_Address_Cal(num_ranks, &ddr, i, 0, CWL_wrdata_pat[2], rddata);
		window_max=DDR_Init_Address_Cal(num_ranks, &ddr, i, 0, CWL_wrdata_pat[2], rddata); 
#if 0
		if((dram_speed<=1333 && window_max < 240) || (dram_speed>1333 && window_max < 200)) {
		    fw_uint64_t details[3];
		    details[0] = FW_RAS_DDR_INIT_NARROW_ADDRESS_WINDOW;
		    details[1] = i;
		    details[2] = window_max;
		    fw_writeRASEvent( FW_RAS_DDR_INIT_WARNING, 3, details );	// ==> WARN
		    //FW_Warning("MC%d has Narrow Address Window: window_max=%d",i,window_max);
		}
#endif
	}
	for(i=0; i<2 ;i++)
		DCRWritePriv(_DDR_MC_MCZMRINT(i), val);	// Enable ZQcal, MEMCAL, Refresh
}
#endif


#if 0
if(!ddr.SKIP_MEMBIST) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 18. MEM BIST\n\n"));

	for(i=0; i<2 ;i++)
	{
		MC_DEBUG(("MEM BIST MC[%d]\n",i));
		MEM_BIST(i, num_ranks);
	}
}
#endif

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 19. Load Final MCA settings\n\n"));

//	val = DCRReadPriv(_DDR_MC_MCADTAL(0));
//	val &= ~(_B8(23,0xFF));
//	val |=	_B8(23,ddr.ADDR_HASH_DISABLE? 0:ddr.WLAT+8);		// For DD2 and later, if address hashing is desired (recommended) then load MCADTAL(16:23) now
//
//	for(i=0;i<2;i++)
//		DCRWritePriv(_DDR_MC_MCADTAL(i), val);

	val = DCRReadPriv(_DDR_MC_MCACFG1(0));
	val &= ~(_B1(24,1));
	val |=	_B1(24,ddr.PD_DISABLE);

	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCACFG1(i), val);


if(!ddr.SKIP_FASTINIT && !ddr.SKIP_OT_SCRUB) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 20. Memory Initialization\n\n"));

#if FW_DEBUG
		MC_DEBUG(("Num_ranks = %d    dram_density = %dMb\n",num_ranks,dram_density));
#endif
	MEM_FastInit_allMC(num_ranks-1,7,MAX_ROW_ADDR(dram_density),0x3FF,CWL_wrdata_pat[0]);
	MC_DEBUG(("Done\n\n"));
}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 20A.DRAM & PBUS ECC Setting\n\n"));

	if(!ddr.DRAM_ECC_DISABLE)
	{
		if(!ddr.PBUS_ECC_DISABLE)
		{
			MC_DEBUG(("DRAM ECC ENABLED, PBUS ECC ENABLED\n\n"));
			val =  _B6(9,0x00);
		}
		else
		{
			MC_DEBUG(("DRAM ECC ENABLED, PBUS ECC DISABLED\n\n"));
			val =  _B6(9,0x3C);
		}
	}

	else
	{
		if(!ddr.PBUS_ECC_DISABLE)
		{
			MC_DEBUG(("DRAM ECC DISABLED, PBUS ECC ENABLED\n\n"));
			val =  _B6(9,0x03);
		}
		else
		{
			MC_DEBUG(("DRAM ECC DISABLED, PBUS ECC DISABLED\n\n"));
			val =  _B6(9,0x3F);
		}
	}

	if(!ddr.DATA_INV_DISABLE)
		val |= _BN(24);		// data inversion enable
	val |= _BN(25);			// x8 mode
//	val |= _BN(26);			// single wire mode
//	val |= _BN(27);			// disable slow decoder
//	val |= _BN(29);			// disable hardware marking

	for(i=0; i<2 ;i++)
	{
		DCRWritePriv(_DDR_MC_MCECCDIS(i), val);
		if(!TI_isDD1())
		{
		//	DCRWritePriv(_DDR_MC_MCARETRY(i), _B3(37,6) | _B3(40,1) | _BN(41));	// [35:37] Fastdecoder_UE injection
			DCRWritePriv(_DDR_MC_MCARETRY(i), _B3(40,6) | _BN(41));
		}
		if(!smb_pass || ecc_on)
		{
			for(rr=0; rr<num_ranks; rr++)
			{
				for(j=0;j<18;j++)
				{
					if((ddr.MRKCHIP[i][rr] & (1<<j)) != 0)
					{
						if((ddr.MRKCHIP[i][(num_ranks-1) - rr] & (1<<j)) == 0)
						{
						//	if(!TI_isDD1())
						//		Mem_Marking(i,rr,j);
						//	printf("WARNING: BAD DRAM WAS %s - MC%d RANK%d CHIP%d (U%d%d%s%s)\n", TI_isDD1()?"DETECTED":"MARKED", i, rr, j, (j+1)/10, (j+1)%10,
						//		(rr==0?"T":(rr==1?"B":(rr==2?"U":"D"))), i==0?"R":"L");
		
		
					    		fw_uint64_t details[3];
					    		details[0] = i;
					    		details[1] = rr;
					    		details[2] = j+1;
					    		fw_writeRASEvent( FW_RAS_BAD_DRAM_WARNING, 3, details );	// ==> Warning
						}
						else if(rr < num_ranks/2)
						{
							fw_uint64_t details[2];
							details[0] = i;
							details[1] = j;
							fw_writeRASEvent( FW_RAS_BAD_PHY_WARNING, 2, details );	// ==> Warning
						}
					}
				}
			}
		}
	}


if(!ddr.SKIP_OT_SCRUB) {
    if ( FW_IS_NODE_0 ) {
	if(!ddr.SKIP_FASTINIT)
		printf("Memory cell initialization done. Wait for verfication of good ECC ....\n");
	else
		printf("Wait for DDR Self Test ....\n");
    }

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 21. One Time Memory Scrubbing\n\n"));

	val = MEM_Scrubbing_allMC(num_ranks-1,7,MAX_ROW_ADDR(dram_density),0x3FF);
	if(val==0)
		MC_DEBUG(("Clean\n\n"));
}

#if FW_DEBUG
//	DDR_Init_Dump_Delays(num_ranks, MC_IOM);
#endif


if(!ddr.SKIP_FASTINIT) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 21A.Memory Re-initialization to all zero\n\n"));

#if FW_DEBUG
		MC_DEBUG(("Num_ranks = %d    dram_density = %dMb\n",num_ranks,dram_density));
#endif
	for(i=0;i<17;i++)
		CWL_wrdata_pat[2][i]=0;
	MEM_FastInit_allMC(num_ranks-1,7,MAX_ROW_ADDR(dram_density),0x3FF,CWL_wrdata_pat[2]);
	MC_DEBUG(("Done\n\n"));
}

#if 0	// 01-30-2012
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 21B.All Banks Precharge\n\n"));

	for(i=0;i<2;i++)
		DCRWritePriv(_DDR_MC_MCAPOS(i), _B19(18, 0x55555) | _BN(19) | _BN(21) | _B8(30,0xFF) | _B4(34,0xF) | _BN(35) | _BN(36) | _BN(37));       // PREA
	do
		val = DCRReadPriv(_DDR_MC_MCAPOS(1)) & _BN(37);
	while(val);
#endif


if(!ddr.BG_SCRUB_DISABLE) {
/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 21C.Background Memory Scrubbing Setting\n\n"));

	for(i=0; i<2 ;i++)
	{
		DCRWritePriv(_DDR_MC_MCMACA(i), 0);
		DCRWritePriv(_DDR_MC_MCMEA(i), _B3(3,num_ranks-1) | _B3(10,7) | _B16(26,MAX_ROW_ADDR(dram_density)) | _B9(35,0x3FF>>3)); 
		DCRWritePriv(_DDR_MC_MCSCTL(i), _BN(0) | _BN(2) | _BN(3) | _BN(4) | _BN(5) | _BN(6) | _BN(19) | _B12(31,ddr.BG_SCRUB_RATE));    // Stop on CTE/CE/UE/SUE
		DCRWritePriv(_DDR_MC_MCMCT(i), _BN(0) | _BN(1) | _BN(3) | _BN(5) | _BN(7));
		Mem_reserve_readbuffer(i);
		DCRWritePriv(_DDR_MC_MCMCC(i), _BN(0) | _BN(4) | _BN(5));
	}
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 22. MCFIR\n\n"));

	for(i=0; i<2 ;i++)
	{
	//	DCRWritePriv(_DDR_MC_MCSCTL(i), _BN(5));	// Clear Scrub Counter and Syndrome

	// Reset Marking Store Data as Moyra has found Non zero marking registers
		if(!TI_isDD1())
			for(j=0; j<8 ;j++)
			    if(ddr.MRKCHIP[i][j] == 0)
				DCRWritePriv(_DDR_MC_MRKSTDTA0(i)+j, 0);

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
	//	DCRWritePriv(_DDR_MC_MCMODE(i), _B5(12,13) | _B5(17,14) | _B5(22,15) | _B5(27,16) | _B5(32,18) | _B5(37,19) | _B5(42,20) | _B5(47,29) | _B8(55,0xFF) | _B8(63,0xFF));
		DCRWritePriv(_DDR_MC_MCMODE(i), _B5(12,1) | _B5(17,2) | _B5(22,3) | _B5(27,4) | _B5(32,13) | _B5(37,14) | _B5(42,15) | _B5(47,16) | _B8(55,0xFF) | _B8(63,0xFF));

		DCRWritePriv(_DDR_MC_MCAPERFMONC1(i), _B5(36,0)  | _B5(41,4) | _B5(46,8) | _B5(51,9) | _B5(56,10) | _B5(61,12));
		DCRWritePriv(_DDR_MC_MCAPERFMONC2(i), _B5(36,5) | _B5(41,6));
		DCRWritePriv(_DDR_MC_MCAPERFMONC0(i), _BN(32) | _B8(63,0xFF));
	}


/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("Step 24. Final Read Calibration\n\n"));

	DDR_PHY_Recal(0, 0);
	DDR_PHY_Recal(1, 0);

/*----------------------------------------------------------------------------------------------------------------------------------------*/
MC_DEBUG(("HW CALIBRATION RETRY = %d\n", caltry));
if ( FW_IS_NODE_0 )
    printf("Done\n");
MC_DEBUG(("End of DDR Init Sequence\n\n"));

}

return 0;

}


#undef	FW_DEBUG
#undef	CWL_OPT_CENTER
#undef	NUM_CAL_REPEAT
#undef	NUM_CAL_RETRY
#undef	NUM_DD_RETRY
#undef	NUM_SB_RETRY
#undef	INIT_LOOP
#undef	NEW_REFRESH
#undef	REPRO
