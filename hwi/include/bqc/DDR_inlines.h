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
#define	Mem_Rank_Addr	0	// 0: rank0, 4: rank1, 2: rank2, 6: rank3, 1: rank4, 5: rank5, 3:rank6, 7: rank7	-- Not used
#define	Mem_Bank_Addr	0	// 0-7
#define	Mem_Row_Addr	0	// 0-(2^15-1) for 2Gb [15/10/3 mode], 0-(2^14-1) for 1Gb [14/10/3 mode]	0x0000 - 0x3FFF
#define	Mem_Col_Addr	0	// 0-(2^10-1) (*) lower 3 bits would not be picked up (tied to 0)	0x0000 - 0x03FF
#define	Mem_Compare(x,y)	(x[0]==y[0]&&x[1]==y[1]&&x[2]==y[2]&&x[3]==y[3]&&x[4]==y[4]&&x[5]==y[5]&&x[6]==y[6]&&x[7]==y[7]&&x[8]==y[8]&& \
				x[9]==y[9]&&x[10]==y[10]&&x[11]==y[11]&&x[12]==y[12]&&x[13]==y[13]&&x[14]==y[14]&&x[15]==y[15]&&x[16]==y[16])
#define	Mem_Compare_8L(x,y)	((x[0]^y[0])|(x[2]^y[2])|(x[4]^y[4])|(x[6]^y[6])|(x[8]^y[8])|(x[10]^y[10])|(x[12]^y[12])|(x[14]^y[14]))
#define	Mem_Compare_8H(x,y)	((x[1]^y[1])|(x[3]^y[3])|(x[5]^y[5])|(x[7]^y[7])|(x[9]^y[9])|(x[11]^y[11])|(x[13]^y[13])|(x[15]^y[15]))
#define	MCMCC_TimeOut		1000
#define	MEMTEST_TimeOut		200
#define	DDRMNT_MCMODE		1
#define	MAX_NUM_UE_PRINT	10
#define	MSG_NUM_UE_COUNT	1000
#define	MAX_NUM_UE_COUNT	10000
#define	EXTERN			extern
#define	CHAR			char

#define	MC_RANK_MNT(x)	x
#define MAX_ROW_ADDR(x) (x==4096?0xFFFF:(x==2048?0x7FFF:(x==1024?0x3FFF:0x1FFF)))

#define	FW_DEBUG	0
#define	PRINTOUT	0

#if	FW_DEBUG | PRINTOUT
#define	MC_DEBUG(x)		printf x
#else
#define	MC_DEBUG(x)
#endif

#ifndef FW_RAS_DDR_DRILLDOWN
#define FW_RAS_DDR_DRILLDOWN  0x00080026
#endif

int MEM_Alter(char mc, unsigned rank, uint64_t *data)
{
	uint64_t temp;
	unsigned RRR;
	int i=0;
	EXTERN int Mem_wait_for_MC_idle(CHAR), Mem_reserve_readbuffer(CHAR);

	Mem_wait_for_MC_idle(mc);

	RRR = MC_RANK_MNT(rank);
	DCRWritePriv(_DDR_MC_MCMACA(mc), _B3(3,RRR) | _B3(10,Mem_Bank_Addr) | _B16(26,Mem_Row_Addr) | _B9(35,Mem_Col_Addr>>3));

	DCRWritePriv(_DDR_MC_MCMCT(mc), _BN(0) | _BN(2));
	Mem_reserve_readbuffer(mc);
	for(i=0;i<16;i++)
	{
		DCRWritePriv(_DDR_MC_MCDADR(mc), data[i]);
		DCRWritePriv(_DDR_MC_MCDACR(mc), _B2(1,i&0x3) | _B2(3,i>>2) | _BN(4));
	}
	DCRWritePriv(_DDR_MC_MCMCC(mc), _BN(0) | _BN(4) | _BN(5) | _BN(6));
	i=0;
	do{
		temp = DCRReadPriv(_DDR_MC_MCMCC(mc)) >> 60;
		if(i++>MCMCC_TimeOut)
		{
			MC_DEBUG(("Timeout: MCMCC2 = 0x%lx\n", temp));
			return(1);
		}
	}
	while(temp != 0x2);

	return(0);
}

int MEM_Display(char mc, unsigned rank, uint64_t *data)
{
	uint64_t temp;
	unsigned RRR;
	int i=0;
	EXTERN int Mem_wait_for_MC_idle(CHAR), Mem_reserve_readbuffer(CHAR);

	Mem_wait_for_MC_idle(mc);

	RRR = MC_RANK_MNT(rank);
	DCRWritePriv(_DDR_MC_MCMACA(mc), _B3(3,RRR) | _B3(10,Mem_Bank_Addr) | _B16(26,Mem_Row_Addr) | _B9(35,Mem_Col_Addr>>3));

	DCRWritePriv(_DDR_MC_MCMCT(mc), _BN(0) | _BN(1));
	Mem_reserve_readbuffer(mc);
	DCRWritePriv(_DDR_MC_MCECTL(mc), _B2(1,3) | _B1(2,1));
	DCRWritePriv(_DDR_MC_MCMCC(mc), _BN(0) | _BN(4) | _BN(5) | _BN(6));
	i=0;
	do{
		temp = DCRReadPriv(_DDR_MC_MCMCC(mc)) >> 60;
		if(i++>MCMCC_TimeOut)
		{
			MC_DEBUG(("Timeout: MCMCC2 = 0x%lx\n", temp));
			return(1);
		}
	}
	while(temp != 0x2);
	data[16] = DCRReadPriv(_DDR_MC_MCDADR(mc));
	for(i=0;i<16;i++)
	{
		DCRWritePriv(_DDR_MC_MCDACR(mc), _B2(1,i&0x3) | _B2(3,i>>2));
		data[i] = DCRReadPriv(_DDR_MC_MCDADR(mc));
	}

	return(0);
}

int MEM_Alter_addr(char mc, unsigned rank, unsigned bank, unsigned row, unsigned col, uint64_t *data)
{
	uint64_t temp;
	unsigned RRR;
	int i=0;
	EXTERN int Mem_wait_for_MC_idle(CHAR), Mem_reserve_readbuffer(CHAR);

	Mem_wait_for_MC_idle(mc);

	RRR = MC_RANK_MNT(rank);
	DCRWritePriv(_DDR_MC_MCMACA(mc), _B3(3,RRR) | _B3(10,bank) | _B16(26,row) | _B9(35,col>>3));

	DCRWritePriv(_DDR_MC_MCMCT(mc), _BN(0) | _BN(2));
	Mem_reserve_readbuffer(mc);
	for(i=0;i<16;i++)
	{
		DCRWritePriv(_DDR_MC_MCDADR(mc), data[i]);
		DCRWritePriv(_DDR_MC_MCDACR(mc), _B2(1,i&0x3) | _B2(3,i>>2) | _BN(4));
	}
	DCRWritePriv(_DDR_MC_MCMCC(mc), _BN(0) | _BN(4) | _BN(5) | _BN(6));
	i=0;
	do{
		temp = DCRReadPriv(_DDR_MC_MCMCC(mc)) >> 60;
		if(i++>MCMCC_TimeOut)
		{
			MC_DEBUG(("Timeout: MCMCC2 = 0x%lx\n", temp));
			return(1);
		}
	}
	while(temp != 0x2);

	return(0);
}

int MEM_Alter_addr_ECC(char mc, unsigned rank, unsigned bank, unsigned row, unsigned col, uint64_t *data)
{
	uint64_t temp;
	unsigned RRR;
	int i=0;
	EXTERN int Mem_wait_for_MC_idle(CHAR), Mem_reserve_readbuffer(CHAR);

	Mem_wait_for_MC_idle(mc);

	RRR = MC_RANK_MNT(rank);
	DCRWritePriv(_DDR_MC_MCMACA(mc), _B3(3,RRR) | _B3(10,bank) | _B16(26,row) | _B9(35,col>>3));

	DCRWritePriv(_DDR_MC_MCMCT(mc), _BN(0) | _BN(2) | _BN(7));
	DCRWritePriv(_DDR_MC_MCECTL(mc), _B2(1,3) | _B1(2,1));
	Mem_reserve_readbuffer(mc);
	for(i=0;i<16;i++)
	{
		DCRWritePriv(_DDR_MC_MCDADR(mc), data[i]);
		DCRWritePriv(_DDR_MC_MCDACR(mc), _B2(1,i&0x3) | _B2(3,i>>2) | _BN(4));
	}
	DCRWritePriv(_DDR_MC_MCDADR(mc), data[i]);
	DCRWritePriv(_DDR_MC_MCMCC(mc), _BN(0) | _BN(4) | _BN(5) | _BN(6));
	i=0;
	do{
		temp = DCRReadPriv(_DDR_MC_MCMCC(mc)) >> 60;
		if(i++>MCMCC_TimeOut)
		{
			MC_DEBUG(("Timeout: MCMCC2 = 0x%lx\n", temp));
			return(1);
		}
	}
	while(temp != 0x2);

	return(0);
}

/*
int MEM_Alter_addr_short(char mc, unsigned rank, unsigned bank, unsigned row, unsigned col, uint64_t *data)
{
	uint64_t temp;
	unsigned RRR;
	int i=0;
	EXTERN int Mem_wait_for_MC_idle(CHAR), Mem_reserve_readbuffer(CHAR);

	Mem_wait_for_MC_idle(mc);

	RRR = MC_RANK_MNT(rank);
	DCRWritePriv(_DDR_MC_MCMACA(mc), _B3(3,RRR) | _B3(10,bank) | _B16(26,row) | _B9(35,col>>3));

	DCRWritePriv(_DDR_MC_MCMCT(mc), _BN(0) | _BN(2));

	DCRWritePriv(_DDR_MC_MCMCC(mc), _BN(0) | _BN(4) | _BN(5) | _BN(6));
	i=0;
	do{
		temp = DCRReadPriv(_DDR_MC_MCMCC(mc)) >> 60;
		if(i++>MCMCC_TimeOut)
		{
			MC_DEBUG(("Timeout: MCMCC2 = 0x%lx\n", temp));
			return(1);
		}
	}
	while(temp != 0x2);

	return(0);
}
*/

int MEM_Display_addr(char mc, unsigned rank, unsigned bank, unsigned row, unsigned col, uint64_t *data)
{
	uint64_t temp;
	unsigned RRR;
	int i=0;
	EXTERN int Mem_wait_for_MC_idle(CHAR), Mem_reserve_readbuffer(CHAR);

	Mem_wait_for_MC_idle(mc);

	RRR = MC_RANK_MNT(rank);
	DCRWritePriv(_DDR_MC_MCMACA(mc), _B3(3,RRR) | _B3(10,bank) | _B16(26,row) | _B9(35,col>>3));

	DCRWritePriv(_DDR_MC_MCMCT(mc), _BN(0) | _BN(1));
	Mem_reserve_readbuffer(mc);
	DCRWritePriv(_DDR_MC_MCECTL(mc), _B2(1,3) | _B1(2,1));
	DCRWritePriv(_DDR_MC_MCMCC(mc), _BN(0) | _BN(4) | _BN(5) | _BN(6));
	i=0;
	do{
		temp = DCRReadPriv(_DDR_MC_MCMCC(mc)) >> 60;
		if(i++>MCMCC_TimeOut)
		{
			MC_DEBUG(("Timeout: MCMCC2 = 0x%lx\n", temp));
			return(1);
		}
	}
	while(temp != 0x2);
	data[16] = DCRReadPriv(_DDR_MC_MCDADR(mc));
	for(i=0;i<16;i++)
	{
		DCRWritePriv(_DDR_MC_MCDACR(mc), _B2(1,i&0x3) | _B2(3,i>>2));
		data[i] = DCRReadPriv(_DDR_MC_MCDADR(mc));
	}

	return(0);
}

#if 0	// OLD
int MEM_Increment_addr(char mc)
{
    uint64_t temp, mcmct;
    int i=0;

    mcmct = DCRReadPriv(_DDR_MC_MCMCT(mc));
    DCRWritePriv(_DDR_MC_MCMCT(mc), _BN(0) | _BN(3));
    DCRWritePriv(_DDR_MC_MCMCC(mc), _BN(0) | _BN(4) | _BN(5));
    i=0;
    do{
	temp = DCRReadPriv(_DDR_MC_MCMCC(mc)) >> 60;
	if(i++>MCMCC_TimeOut)
	{
	    MC_DEBUG(("Timeout: MCMCC2 = 0x%lx\n", temp));
	    return(1);
	}
    }
    while(temp != 0x2);
    
    DCRWritePriv(_DDR_MC_MCMCT(mc), mcmct);
    
    return(0);
}
#endif

int MEM_Increment_addr(char mc)
{
	uint64_t mcmaca, mcmea;
	unsigned rank, bank, row, col;
	unsigned max_rank, max_bank, max_row, max_col;

	// 0=DIMM 1:3=rank 8:10=bank 11:26=row 27:35=col

	mcmea = DCRReadPriv(_DDR_MC_MCMEA(mc));
	max_rank = (mcmea >> 60) & 0xF;
	max_bank = (mcmea >> 53) & 0x7;
	max_row  = (mcmea >> 37) & 0xFFFF;
	max_col  = (mcmea >> 28) & 0x1FF;

	mcmaca = DCRReadPriv(_DDR_MC_MCMACA(mc));
	rank = (mcmaca >> 60) & 0xF;
	bank = (mcmaca >> 53) & 0x7;
	row  = (mcmaca >> 37) & 0xFFFF;
	col  = (mcmaca >> 28) & 0x1FF;

	if(col < max_col) {
		col++;
	}
	else if(row < max_row) {
		row++; col=0;
	}
	else if(bank < max_bank) {
		bank++; row=0; col=0;
	}
	else if(rank < max_rank) {
		rank++; bank=0; row=0; col=0;
	}
	else {
		rank=0; bank=0; row=0; col=0;
	}

	DCRWritePriv(_DDR_MC_MCMACA(mc), _B4(3,rank) | _B3(10,bank) | _B16(26,row) | _B9(35,col));

	MC_DEBUG(("MCMACA_OLD = %016lx	MCMACA_NEW = %016lx\n",	mcmaca, DCRReadPriv(_DDR_MC_MCMACA(mc))));

	return(0);
}

/*
int MEM_FastInit(char mc, unsigned end_rank, unsigned end_bank, unsigned end_row, unsigned end_col, uint64_t *data)
{
	uint64_t temp;
	unsigned RRR;
	int i=0;
	unsigned rank, bank;
	EXTERN int Mem_wait_for_MC_idle(CHAR), Mem_reserve_readbuffer(CHAR);

	Mem_wait_for_MC_idle(mc);

	DCRWritePriv(_DDR_MC_MCMACA(mc), 0);

	DCRWritePriv(_DDR_MC_MCMCT(mc), _BN(0) | _BN(2) | _BN(3));
	Mem_reserve_readbuffer(mc);
	for(i=0;i<16;i++)
	{
		DCRWritePriv(_DDR_MC_MCDADR(mc), data[i]);
		DCRWritePriv(_DDR_MC_MCDACR(mc), _B2(1,i&0x3) | _B2(3,i>>2) | _BN(4));
	}

//	switch(end_rank+1)
//	{
//		case 4: temp = _BN(3);
//		case 2: temp = _BN(1);
//		case 1: temp = _BN(0);
//	}
//	DCRWritePriv(_DDR_MC_MCACFG0(mc), temp);

	for(rank=0;rank<=end_rank;rank++)
	for(bank=0;bank<=end_bank;bank++)
	{
	MC_DEBUG(("Rank%u Bank%u\n",rank,bank));
	
		RRR = MC_RANK_MNT(rank);
		DCRWritePriv(_DDR_MC_MCMEA(mc), _B3(3,RRR) | _B3(10,bank) | _B16(26,end_row) | _B9(35,end_col>>3));
	
		DCRWritePriv(_DDR_MC_MCMCC(mc), _BN(0) | _BN(4) | _BN(5) | _BN(6));
		i=0;
		do{
			DelayTimeBase(1600000000);
			temp  = DCRReadPriv(_DDR_MC_MCMCC(mc)) >> 60;
			if(i++>MEMTEST_TimeOut)
			{
				MC_DEBUG(("Timeout: MCMCC_FastInit = 0x%lx\n", temp));
				break;
			}
		}
		while(temp != 0x2);
	}

	return(0);
}
*/

/*
int MEM_FW_Init(char mc, unsigned end_rank, unsigned end_bank, unsigned end_row, unsigned end_col, uint64_t *data)
{
	unsigned rank, bank, row, col;

	MEM_Alter(mc, 0, data);
	for(rank=0;rank<=end_rank;rank++)
		for(bank=0;bank<=end_bank;bank++)
			for(row=0;row<=end_row;row+=0x1)
			{
				if(row%0xFF == 0)
					MC_DEBUG(("Rank%u Bank%u Row:0x%4X\n",rank,bank,row));
				for(col=0;col<=end_col;col+=0x1)
					MEM_Alter_addr_short(mc, rank,bank, row, col,  data);
			}
	return(0);
}
*/

int MEM_FastInit_allMC(unsigned end_rank, unsigned end_bank, unsigned end_row, unsigned end_col, uint64_t *data)
{
	uint64_t temp, tmp[2];
	unsigned RRR;
	int i=0, mc;
	EXTERN int Mem_wait_for_MC_idle(CHAR), Mem_reserve_readbuffer(CHAR);

	for(mc=0;mc<2;mc++)
	{
		Mem_wait_for_MC_idle(mc);

		DCRWritePriv(_DDR_MC_MCMACA(mc), 0);

		RRR = MC_RANK_MNT(end_rank);
		DCRWritePriv(_DDR_MC_MCMEA(mc), _B3(3,RRR) | _B3(10,end_bank) | _B16(26,end_row) | _B9(35,end_col>>3));
		temp = DCRReadPriv(_DDR_MC_MCMEA(mc));
		MC_DEBUG(("End Address: MC%d: Rank%d Bank%d Row=0x%4X Col=0x%4X\n",mc,(int)(temp>>60)&0x7,(int)(temp>>53)&0x7,(unsigned)(temp>>37)&0xFFFF,(unsigned)(temp>>(28-3))&0xFFF));
		DCRWritePriv(_DDR_MC_MCMCT(mc), _BN(0) | _BN(2) | _BN(3) | _B1(4,TI_isDD1()?0:1));	// Super Fast Init for DD2 -- 01/18/2011
		Mem_reserve_readbuffer(mc);

	//	switch(end_rank+1)
	//	{
	//		case 4: temp = _BN(3);
	//		case 2: temp = _BN(1);
	//		case 1: temp = _BN(0);
	//	}
	//	DCRWritePriv(_DDR_MC_MCACFG0(mc), temp);

		for(i=0;i<16;i++)
		{
			DCRWritePriv(_DDR_MC_MCDADR(mc), data[i]);
			DCRWritePriv(_DDR_MC_MCDACR(mc), _B2(1,i&0x3) | _B2(3,i>>2) | _BN(4));
		}

		DCRWritePriv(_DDR_MC_MCMCC(mc), _BN(0) | _BN(4) | _BN(5) | _BN(6));
	}

	MC_DEBUG(("Started MEM_FastInit for MC0 and MC1\n"));

	i=0;
	do{
		DelayTimeBase(80000000);
		for(mc=0;mc<2;mc++)
			tmp[mc]  = DCRReadPriv(_DDR_MC_MCMCC(mc)) >> 60;
	//	if(i++>MEMTEST_TimeOut)
	//		break;
	//	MC_DEBUG(("%d seconds\n",i));
	}
	while(tmp[0] != 0x2 || tmp[1] != 0x2);
	for(mc=0;mc<2;mc++)
		if(tmp[mc] != 0x2)
		{
			temp = DCRReadPriv(_DDR_MC_MCMACA(mc));
			MC_DEBUG(("Timeout: MC%d: Rank%d Bank%d Row=0x%4X Col=0x%4X\n",mc,(int)(temp>>60)&0x7,(int)(temp>>53)&0x7,(unsigned)(temp>>37)&0xFFFF,(unsigned)(temp>>(28-3))&0xFFF));
		}

	return(0);
}

/*
int MEM_FastInit_1MC(char mc, unsigned end_rank, unsigned end_bank, unsigned end_row, unsigned end_col, uint64_t *data)
{
	uint64_t temp;
	unsigned RRR;
	int i=0;
	EXTERN int Mem_wait_for_MC_idle(CHAR), Mem_reserve_readbuffer(CHAR);

	Mem_wait_for_MC_idle(mc);

	DCRWritePriv(_DDR_MC_MCMACA(mc), 0);

	RRR = MC_RANK_MNT(end_rank);
	DCRWritePriv(_DDR_MC_MCMEA(mc), _B3(3,RRR) | _B3(10,end_bank) | _B16(26,end_row) | _B9(35,end_col>>3));
	DCRWritePriv(_DDR_MC_MCMCT(mc), _BN(0) | _BN(2) | _BN(3));
	Mem_reserve_readbuffer(mc);

//	switch(end_rank+1)
//	{
//		case 4: temp = _BN(3);
//		case 2: temp = _BN(1);
//		case 1: temp = _BN(0);
//	}
//	DCRWritePriv(_DDR_MC_MCACFG0(mc), temp);

	for(i=0;i<16;i++)
	{
		DCRWritePriv(_DDR_MC_MCDADR(mc), data[i]);
		DCRWritePriv(_DDR_MC_MCDACR(mc), _B2(1,i&0x3) | _B2(3,i>>2) | _BN(4));
	}

	DCRWritePriv(_DDR_MC_MCMCC(mc), _BN(0) | _BN(4) | _BN(5) | _BN(6));

	i=0;
	do{
		DelayTimeBase(1600000000);
		temp  = DCRReadPriv(_DDR_MC_MCMCC(mc)) >> 60;
		if(i++>MEMTEST_TimeOut)
			break;
		MC_DEBUG(("%d seconds\n",i));
	}
	while(temp != 0x2);
	if(temp != 0x2)
	{
		temp = DCRReadPriv(_DDR_MC_MCMACA(mc));
		MC_DEBUG(("Timeout: MC0: Rank%d Bank%d Row=0x%4X Col=0x%4X\n",(int)(temp>>60)&0x7,(int)(temp>>53)&0x7,(unsigned)(temp>>37)&0xFFFF,(unsigned)(temp>>(28-3))&0xFFF));
	}

	return(0);
}
*/

int MEM_Scrubbing_allMC(unsigned end_rank, unsigned end_bank, unsigned end_row, unsigned end_col)
{
	uint64_t mcmea, temp, mcmaca, mcamisc=0;
	unsigned RRR, num_errors=0;
	int	mc, done[2]={0,0};
	EXTERN int Mem_wait_for_MC_idle(CHAR), Mem_reserve_readbuffer(CHAR);

	RRR = MC_RANK_MNT(end_rank);
	mcmea = _B3(3,RRR) | _B3(10,end_bank) | _B16(26,end_row) | _B9(35,end_col>>3);

	// Enable page mode for fast test
	if(!TI_isDD1())
		for(mc=0;mc<2;mc++)
		{
			mcamisc = DCRReadPriv(_DDR_MC_MCAMISC(mc));
			temp =	mcamisc	|
				_BN(10)	|	// Enable read page mode
				_BN(11);	// Enable write page mode
			DCRWritePriv(_DDR_MC_MCAMISC(mc), temp);
		}

	for(mc=0; mc<2; mc++)
	{
		Mem_wait_for_MC_idle(mc);
		DCRWritePriv(_DDR_MC_MCMACA(mc), 0);
		DCRWritePriv(_DDR_MC_MCMEA(mc), mcmea);
		DCRWritePriv(_DDR_MC_MCMCT(mc), _BN(0) | _BN(1) | _BN(3) | _BN(5) | _BN(7));
		DCRWritePriv(_DDR_MC_MCSCTL(mc), _BN(0) | _BN(2) | _BN(3) | _BN(4) | _BN(5) | _BN(6));                  // Stop on CTE/CE/UE/SUE
		Mem_reserve_readbuffer(mc);
		DCRWritePriv(_DDR_MC_MCMCC(mc), _BN(0) | _BN(4) | _BN(5) | _BN(6));
	}

	MC_DEBUG(("Started MEM_Scrubbing for MC0 and MC1\n"));

	do
	{
	    //      DelayTimeBase(80000000);        -- In the early control system, if I polled SCOM too much, it hanged. I think we can remove it now.
	    for(mc=0; mc<2; mc++)
	    {
		if(!done[mc])
		{
		    temp = DCRReadPriv(_DDR_MC_MCMCC(mc)) >> 60;
		    if(temp == 0x2)
		    {
			mcmaca = DCRReadPriv(_DDR_MC_MCMACA(mc));
			if(mcmaca == mcmea)
			    done[mc] = 1;
			else
			{
			    fw_uint64_t errsyn = DCRReadPriv(_DDR_MC_MCSSY(mc));
			    fw_uint64_t drilldown[3];
			    drilldown[0] = mc;
			    drilldown[1] = mcmaca;
			    drilldown[2] = errsyn;
			    fw_writeRASEvent( FW_RAS_DDR_DRILLDOWN, 3, drilldown );
			    
			    DCRWritePriv(_DDR_MC_MCMCC(mc), _BN(0) | _BN(4) | _BN(5) | _BN(6));     // Resume one time scrubbing
			    num_errors++;
			}
		    }
		}
	    }
	    if(num_errors > 1000)   // Exit upon abnormally many errors (may happen when there's b ad PHY or a bad DRAM). 1000 is just arbitrary number. You may change it accordingly.
	    {
		// RAS Info like "Too many errors so stopping" or FATAL
		break;
	    }
	} while ( !done[0] || !done[1] );
                                                                                           
	if(!TI_isDD1())
		for(mc=0;mc<2;mc++)
			DCRWritePriv(_DDR_MC_MCAMISC(mc), mcamisc);
	
	return(num_errors);
}

int MEM_Scrubbing(char mc, unsigned end_rank, unsigned end_bank, unsigned end_row, unsigned end_col, uint64_t *data, uint64_t *rddata, uint64_t *syndrome)
{
	uint64_t mcmea, mceccdis, temp=0x0;
	uint64_t fail_word[4][2]={{0,0},{0,0},{0,0},{0,0}};
	uint64_t fail_ecc[4]={0,0,0,0};
	unsigned RRR;
	int	i=0, rank;
	unsigned num_UE=0;
	unsigned num_CE, num_Syndrome;
	EXTERN void Mem_print_vector();
	EXTERN int Mem_wait_for_MC_idle(CHAR), Mem_reserve_readbuffer(CHAR);
	char msg[16] = "MC[*] UE DATA:";

	msg[3] = mc + '0';
	Mem_wait_for_MC_idle(mc);

	DCRWritePriv(_DDR_MC_MCMACA(mc), 0);
	RRR = MC_RANK_MNT(end_rank);
	mcmea = _B3(3,RRR) | _B3(10,end_bank) | _B16(26,end_row) | _B9(35,end_col>>3);
	DCRWritePriv(_DDR_MC_MCMEA(mc), mcmea);
	DCRWritePriv(_DDR_MC_MCMCT(mc), _BN(0) | _BN(1) | _BN(3) | _BN(5) | _BN(7));
//	DCRWritePriv(_DDR_MC_MCSCTL(mc), _BN(3) | _BN(4) | _BN(5) | _BN(6));			//     UE, SUE
	DCRWritePriv(_DDR_MC_MCSCTL(mc), _BN(2) | _BN(3) | _BN(4) | _BN(5) | _BN(6));		// CE, UE, SUE
	mceccdis = DCRReadPriv(_DDR_MC_MCECCDIS(mc));
	Mem_reserve_readbuffer(mc);

	do
	{
		DCRWritePriv(_DDR_MC_MCMCT(mc), _BN(0) | _BN(1) | _BN(3) | _BN(5) | _BN(7));
		DCRWritePriv(_DDR_MC_MCMCC(mc), _BN(0) | _BN(4) | _BN(5) | _BN(6));
		while(1){
			if(num_UE > MAX_NUM_UE_COUNT)
			{
				DelayTimeBase(80000000);
#if 0
				temp = DCRReadPriv(_DDR_MC_MCMACA(mc));
				rank = (int)(temp>>60)&0x7;
				MC_DEBUG(("MC[%d] MCMACA: Rank%d Bank%d Row=0x%x Col=0x%x\n",
					mc,rank,(int)(temp>>53)&0x7,(unsigned)(temp>>37)&0xFFFF,(unsigned)(temp>>(28-3))&0xFFF));
#endif
			}
			temp  = DCRReadPriv(_DDR_MC_MCMCC(mc)) >> 60;
			if(temp == 0x2)
				break;
		}

		temp = DCRReadPriv(_DDR_MC_MCMACA(mc));
		if(temp!=mcmea)
		{
			num_UE++;
			if(num_UE < MAX_NUM_UE_COUNT)
			{
				rank = (int)(temp>>60)&0x7;
				if(num_UE < MAX_NUM_UE_PRINT)
					MC_DEBUG(("MC[%d] UE ADDR: Rank%d Bank%d Row=0x%x Col=0x%x\n",
						mc,rank,(int)(temp>>53)&0x7,(unsigned)(temp>>37)&0xFFFF,(unsigned)(temp>>(28-3))&0xFFF));
				else if(num_UE == MAX_NUM_UE_PRINT)
					MC_DEBUG(("MC[%d] UE ADDR: More than %d UE's. Further data is suppressed\n",mc,MAX_NUM_UE_PRINT));

			//	DCRWritePriv(_DDR_MC_MCECCDIS(mc), _B5(9,0x3F) | _BN(25));
			//	DCRWritePriv(_DDR_MC_MCECCDIS(mc), _B5(9,0x3F) | _BN(25) | _BN(27) | _BN(29));
				MEM_Display_addr(mc,rank,(int)(temp>>53)&0x7,(unsigned)(temp>>37)&0xFFFF,(unsigned)(temp>>(28-3))&0xFFF,rddata);

				for(i=0; i<16; i++)
					fail_word[rank][i%2] |= data[i] ^ rddata[i];
				fail_ecc[rank] |=  0xA0A0A0A0A0A0A0A0ULL & ((data[16] ^ rddata[16]) << 0);
				fail_ecc[rank] |=  0x0A0A0A0A0A0A0A0AULL & ((data[16] ^ rddata[16]) << 4);
				fail_ecc[rank] |=  0x5050505050505050ULL & ((data[16] ^ rddata[16]) >> 4);
				fail_ecc[rank] |=  0x0505050505050505ULL & ((data[16] ^ rddata[16]) >> 0);
		#if 1	
				if(num_UE < MAX_NUM_UE_PRINT)
					Mem_print_vector(rddata,msg,17);
		#endif
			//	DCRWritePriv(_DDR_MC_MCECCDIS(mc), mceccdis);
			}
			else if(num_UE == MAX_NUM_UE_COUNT)
			{
				MC_DEBUG(("MC[%d] UE ADDR: More than %d UE's. Suspending\n",mc,MAX_NUM_UE_COUNT));
				DCRWritePriv(_DDR_MC_MCSCTL(mc), _BN(6));
			}
			else if(num_UE % MSG_NUM_UE_COUNT == 0)
			{
				MC_DEBUG(("MC[%d] UE ADDR: More than %d UE's\n",mc,num_UE));
			}
			DCRWritePriv(_DDR_MC_MCMCT(mc), _BN(0) | _BN(3));
			DCRWritePriv(_DDR_MC_MCMCC(mc), _BN(0) | _BN(4) | _BN(5) | _BN(6));
		}
	}
	while(mcmea!=temp);
	temp = DCRReadPriv(_DDR_MC_MCSSC(mc));
	num_Syndrome = (temp >> 52) & 0xFFF;
	num_CE = (temp >> 40) & 0xFFF;	// not supported in DD1

	if(num_UE > 0 || num_CE > 0)
	{
	//	printf("MC[%d]: UE_count %c %u    CE_count = %u    CE_syndrome_count = %u\n", mc, num_UE >= MAX_NUM_UE_COUNT? '>':'=', num_UE, num_CE, num_Syndrome);
		printf("MC[%d]: UE + CE %c %u    CE_syndrome_count = %u\n", mc, num_UE >= MAX_NUM_UE_COUNT? '>':'=', num_UE, num_Syndrome);

		int	k, chip;

		for(rank=0;rank<=end_rank;rank++)
		{
			MC_DEBUG(("fail_word[%d][0] = 0x%lx\n",rank,fail_word[rank][0]));
			MC_DEBUG(("fail_word[%d][1] = 0x%lx\n",rank,fail_word[rank][1]));
			MC_DEBUG(("fail_ecc[%d] = 0x%lx\n",rank,fail_ecc[rank]));

			for(chip=0; chip<18; chip++)
			{
				if((chip+1)%9 != 0)
				{
					if((fail_word[rank][chip/9] & _B8(8*(chip%9)+7,0xFF)) != 0)
						for(k=0;k<8;k++)
							if((fail_word[rank][chip/9] & _BN(8*(chip%9)+k)) != 0)
								syndrome[chip] |= 0x80000000 >> (8*rank+k);
				}
				else
				{
					if((fail_ecc[rank] & _B32(32*(chip-8)/9+31,0xFFFFFFFF)) != 0)
						for(k=0;k<8;k++)
							if((fail_ecc[rank] & _B4(32*(chip-8)/9+4*k+3,0xF)) != 0)
								syndrome[chip] |= 0x80000000 >> (8*rank+k);
				}
			}
		}
	}
	return(0);
}

int MEM_Syndrome(char mc, unsigned end_rank, uint64_t *syndrome)
{
	int	rank, k, chip, n;
	char	msg[80];

	for(rank=0;rank<=end_rank;rank++)
	{
		for(chip=0; chip<18; chip++)
		{
			if((syndrome[chip] & (0xFF000000 >> 8*rank))!= 0)
			{
				n=0;
				msg[n++]='U';
				msg[n++]='0'+(chip+1)/10;
				msg[n++]='0'+(chip+1)%10;
				msg[n++]=(rank==0?'T':(rank==1?'B':(rank==2?'U':'D')));
			//	msg[n++]=(rank%2==0?'T':'B');
				msg[n++]=(mc==0?'R':'L');
				msg[n++]=' ';
				msg[n++]=' ';
				msg[n++]='[';
				for(k=0;k<8;k++)
					if((syndrome[chip] & (0x80000000 >> (8*rank+k))) != 0)
					{
						switch(k)
						{
							case 0:	msg[n++] = rank%2==0? 'E':'D';	msg[n++] = rank%2==0? '8':'2';	break;
							case 1:	msg[n++] = rank%2==0? 'C':'B';	msg[n++] = rank%2==0? '7':'3';	break;
							case 2:	msg[n++] = rank%2==0? 'B':'C';	msg[n++] = rank%2==0? '3':'7';	break;
							case 3:	msg[n++] = rank%2==0? 'E':'E';	msg[n++] = rank%2==0? '3':'7';	break;
							case 4:	msg[n++] = rank%2==0? 'C':'C';	msg[n++] = rank%2==0? '8':'2';	break;
							case 5:	msg[n++] = rank%2==0? 'E':'E';	msg[n++] = rank%2==0? '7':'3';	break;
							case 6:	msg[n++] = rank%2==0? 'C':'C';	msg[n++] = rank%2==0? '2':'8';	break;
							case 7:	msg[n++] = rank%2==0? 'D':'E';	msg[n++] = rank%2==0? '2':'8';	break;
						}
						msg[n++]=',';
					}
				n--;
				msg[n++]=']';
				msg[n]='\0';
				printf("    %s\n",msg);
			}
		}
	}
	return(0);
}

int MEM_BIST(char mc, unsigned num_ranks)
{
	uint64_t temp, temp2/*, mcamisc*/;
	unsigned rr;
	int i=0;
	EXTERN int Mem_wait_for_MC_idle(CHAR), Mem_reserve_readbuffer(CHAR);

	Mem_wait_for_MC_idle(mc);

	for(temp=0, rr=0; rr<num_ranks; rr++)
		temp |= _BN(rr);
	DCRWritePriv(_DDR_MC_MCMTRS(mc), temp);
	Mem_reserve_readbuffer(mc);

/*
	// Enable page mode for fast test
	mcamisc = DCRReadPriv(_DDR_MC_MCAMISC(mc));
	temp =	mcamisc	|
		_BN(10)	|	// Enable read page mode
		_BN(11);	// Enable write page mode
	DCRWritePriv(_DDR_MC_MCAMISC(mc), temp);
*/

	// Test start, Channel 0, Sequential address, Random data, Single duration, Continue on any error, Fast R/W
//	DCRWritePriv(_DDR_MC_MCMTC(mc), _BN(0) | _BN(2) | _BN(7));
//	DCRWritePriv(_DDR_MC_MCMTC(mc), _BN(0) | _BN(2) | _BN(7) | _BN(11));
	DCRWritePriv(_DDR_MC_MCMTC(mc), _BN(0) | _BN(2) | _BN(4) | _BN(7));

	i=0;
	do
	{
		DelayTimeBase(1600000000);
		temp = DCRReadPriv(_DDR_MC_MCMTC(mc)) >> 48;
		if(i++>MEMTEST_TimeOut)
		{
			MC_DEBUG(("Timeout: MCMTC = 0x%lx\n", temp));
			break;
		}
		MC_DEBUG(("%d seconds\n",i));

	}
	while ((temp & 0x8000) != 0x0);

//	DCRWritePriv(_DDR_MC_MCMTC(mc), 0);

	temp  = DCRReadPriv(_DDR_MC_MCMTSBEC(mc)) >> 48;
	temp2 = DCRReadPriv(_DDR_MC_MCMTMBEC(mc)) >> 48;

	MC_DEBUG(("MC[%d]: SBE=%d  MBE=%d\n", mc, (unsigned)temp, (unsigned)temp2));

/*
	DCRWritePriv(_DDR_MC_MCAMISC(mc), mcamisc);
*/

	return(0);
}

void Mem_print_vector(uint64_t *data, char *header, int size)
{
	int	i;

	MC_DEBUG(("%s\n",header));
	if(size==16)
		for(i=0;i<8;i++)
			MC_DEBUG(("0x%016lX  0x%016lX\n",data[2*i],data[2*i+1]));
	else if(size==17)
	{
		for(i=0;i<7;i++)
			MC_DEBUG(("0x%016lX  0x%016lX\n",data[2*i],data[2*i+1]));
		MC_DEBUG(("0x%016lX  0x%016lX  0x%016lX\n",data[2*i],data[2*i+1],data[2*i+2]));
	}
	else if(size==18)
	{
		for(i=0;i<6;i++)
			MC_DEBUG(("%c[%d]0x%016lX  %c[%d]0x%016lX  %c[%d]0x%016lX\n",3*i<10?' ':0,3*i,data[3*i],3*i<9?' ':0,3*i+1,data[3*i+1],3*i<8?' ':0,3*i+2,data[3*i+2]));
	}
}

int Mem_reserve_readbuffer(char mc)
{
	uint64_t temp;
	int	i=0;

	temp = (DCRReadPriv(_DDR_MC_MCMCC(mc)) >> 56) & 0xC;
	if( temp != 0xC)
	{
		DCRWritePriv(_DDR_MC_MCMCC(mc), _BN(4));
		do{
			temp = 0xF & (DCRReadPriv(_DDR_MC_MCMCC(mc)) >> 56);
			if(i++>MCMCC_TimeOut)
			{
				MC_DEBUG(("Timeout: Reserve Read Buffer = 0x%lx\n", temp));
				return(1);
			}
		}
		while(temp != 0xC);
	}
	return(0);
}

int Mem_wait_for_MC_idle(char mc)
{
#if DDRMNT_MCMODE
	uint64_t temp;
	int	i=0;

	do{
		temp = 0xF & (DCRReadPriv(_DDR_MC_MCMODE(mc)) >> 60);
		if(i++ > MCMCC_TimeOut) {MC_DEBUG(("Timeout MCMODE temp=0x%lx\n",temp)); return(1);}
	}
	while(!(temp & 0x2));
#endif
	return(0);
}

void Mem_MC_diagnose()
{
	int	mc, k;

	for(mc=0;mc<2;mc++)
	{
		MC_DEBUG(("MC[%d]: MCMCC    = %016lx\n",mc,DCRReadPriv(_DDR_MC_MCMCC(mc))));
		MC_DEBUG(("MC[%d]: MCFIR    = %016lx\n",mc,DCRReadPriv(_DDR_MC_MCFIR(mc))));
		MC_DEBUG(("MC[%d]: MCMODE   = %016lx\n",mc,DCRReadPriv(_DDR_MC_MCMODE(mc))));
		for(k=0;k<8;k++)
			MC_DEBUG(("MC[%d]: MCSPERFMON%d = %llu\n",mc,k,(long long unsigned)DCRReadPriv(_DDR_MC_MCSPERFMON0(mc)+k)));
		for(k=0;k<8;k++)
			MC_DEBUG(("MC[%d]: MCAPERFMON%d = %llu\n",mc,k,(long long unsigned)DCRReadPriv(_DDR_MC_MCAPERFMON0(mc)+k)));
	}
}

void Mem_Marking(int mc, int rank, int chip)
{
	unsigned	mrkdata;

	switch(chip)
	{
		case  0:	mrkdata = 0x41;	break;
		case  1:	mrkdata = 0x3A;	break;
		case  2:	mrkdata = 0x7B;	break;
		case  3:	mrkdata = 0x25;	break;
		case  4:	mrkdata = 0xD9;	break;
		case  5:	mrkdata = 0xFC;	break;
		case  6:	mrkdata = 0xA7;	break;
		case  7:	mrkdata = 0xB4;	break;
		case  8:	mrkdata = 0x13;	break;
		case  9:	mrkdata = 0xED;	break;
		case 10:	mrkdata = 0x8F;	break;
		case 11:	mrkdata = 0x62;	break;
		case 12:	mrkdata = 0x33;	break;
		case 13:	mrkdata = 0x77;	break;
		case 14:	mrkdata = 0x44;	break;
		case 15:	mrkdata = 0x60;	break;
		case 16:	mrkdata = 0xE0;	break;
		case 17:	mrkdata = 0x80;	break;
		default:	mrkdata = 0x00;
	}

	MC_DEBUG(("Marking MC%d Rank%d Chip%d\n",mc,rank,chip));
	DCRWritePriv(_DDR_MC_MRKSTDTA0(mc)+rank, _B8(7,mrkdata));
}

#undef	MCMCC_TimeOut
#undef	MEMTEST_TimeOut
#undef	DDRMNT_MCMODE
#undef	MAX_NUM_UE_PRINT
#undef	MSG_NUM_UE_COUNT
#undef	MAX_NUM_UE_COUNT
#undef	EXTERN
#undef	CHAR
#undef	MC_RANK_MNT
#undef	PRINTOUT
