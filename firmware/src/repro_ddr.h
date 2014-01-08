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


#define DELTA_ARB_B_A   (DR_ARB_DCR__B_base - DR_ARB_DCR__A_base)


uint64_t fw_DDR_SCOM(unsigned addr, char rw, uint64_t pattern, char *title, uint64_t value, int i)
{
	uint64_t	temp=value;
	int		j=0;

//	printf("[%d] %s\n", i, title);

	// rw==0: write only, 1: read only, 2: write & wait-until-reset, 3: write & wait-until-match, 4: write & read, 5: delay by value cycles
	if(rw==0 || rw==2 || rw==3 || rw==4) {
//		printf("[%d-%d] 	WRITE 0x%08X: 0x%016lX\n", i, j, addr, value);
		DCRWritePriv(addr, value);
	}
	if(rw>0 && rw<5) {
		do {
			if((addr & 0xFFFFFFBF) !=_DDR_MC_IOM_PHYREGINDEX(0)) {
				temp=DCRReadPriv(addr);
//				printf("[%d-%d] 	READ  0x%08X: 0x%016lX\n", i, j, addr, temp);
			}
			else {
				DCRWritePriv(addr + 0x01, value);
				temp=0xFFFF & DCRReadPriv(addr + 0x01);
//				printf("[%d-%d] 	READ  0x%08X: 0x%016lX\n", i, j, addr, temp);
			}
			j++;
		} while((rw==2 && (temp & pattern)!=0) || (rw==3 && (temp != pattern)));
	}
	if(rw==5) {
		temp = GetTimeBase() + value;
		while (GetTimeBase() < temp);
	}
	return temp;
}


void fw_DDR_SCOM_2MC(unsigned addr, char rw, uint64_t pattern, char *title, uint64_t value, int i)
{
//	char	title0[100];
//	char	title1[100];
//	int	n=0;

//	for(n=0;n<strlen(title)+1;n++)	title0[n]=title[n];
//	for(n=0;n<strlen(title)+1;n++)	title1[n]=title[n];
//	strcat(title0," (MC0)");
//	strcat(title1," (MC1)");
//	DDR_SCOM(addr, rw, pattern, title0, value, i);
	fw_DDR_SCOM(addr, rw, pattern, title, value, i);
//	DDR_SCOM(addr + DELTA_ARB_B_A, rw, pattern, title1, value, i);
	fw_DDR_SCOM(addr + DELTA_ARB_B_A, rw, pattern, title, value, i);
}


/*
void fw_IOM_REG_READ(int x, unsigned base_address)
{
	uint64_t	temp_addr;
	unsigned	i, j;
	uint64_t	N, R, B, b;

#if 0
	printf("IOM register data size (Bytes):\n");
	printf("	Direct register = %lu\n",sizeof(IOM_d));
	printf("	Indirect register addr = %lu\n",sizeof(IOM_i));
	printf("	Indirect register data = 4*%lu\n",sizeof(IOM_i_data));
	printf("Total = %lu\n",sizeof(IOM_d)+sizeof(IOM_i)+4*sizeof(IOM_i_data));

	printf("IOM direct register read: satellite-%d\n",x);
#endif

	for(i=0; i<sizeof(IOM_d)/sizeof(IOM_d[0]);++i)
	{
		IOM_d[i].data[x] = DCRReadPriv(base_address + IOM_d[i].addr);
//		printf("[%d][%d] 	READ  0x%08X: 0x%016lX\n", x, i, base_address + IOM_d[i].addr, IOM_d[i].data[x]);
//		if (!(i%5)) printf("step %d\n", i);
	}

#if 0
	printf("IOM indirect register read: satellite-%d\n",x);
#endif
	for(i=0,j=0; i<sizeof(IOM_i)/sizeof(IOM_i[0]);++i)
	{
        //if (!(i%10)) printf("step %d\n", i);
		for(R=0; R==0 || (R< 4*0x200 && (IOM_i[i].type & RRRR)); R+=0x200)
		for(N=0; N==0 || (N< 4*0x100 && (IOM_i[i].type & NNNN)); N+=0x100)
		for(B=0; B==0 || (B< 9*0x020 && (IOM_i[i].type & BBBB)); B+=0x020)
		for(b=0; b==0 || (b<16*0x001 && (IOM_i[i].type & bbbb)); b+=0x001)
		{
			temp_addr = (IOM_i[i].addr + N + R + B + b) << 16;
			DCRWritePriv(base_address + 0x01, temp_addr);
			IOM_i_data[x][j]= (short) (0xFFFF & DCRReadPriv(base_address + 0x01));
//			printf("[%d][%d]:%d 	READ  0x%08X->0x%04X: 0x%04X\n", x, i, j, base_address + 0x01, (unsigned) (temp_addr >> 16), IOM_i_data[x][j]);
			j++;
		}
	}
#if 0
	if(j!=IOM_I_DATA_SIZE)
		printf("\nIOM_i_data array size (%d) is different than expected (%d)!!!\n",j,IOM_I_DATA_SIZE);
	printf("IOM read completed: satellite-%d\n",x);
#endif
}


void fw_IOM_REG_WRITE(int x, unsigned base_address)
{
	uint64_t	temp_addr;
	unsigned	i, j;
	uint64_t	N, R, B, b;

	// printf("IOM direct register write: satellite-%d\n",x);
	for(i=0; i<sizeof(IOM_d)/sizeof(IOM_d[0]);++i)
	{
        // if (!(i%5)) printf("step %d\n", i);
		DCRWritePriv(base_address + IOM_d[i].addr, IOM_d[i].data[x]);
//		printf("[%d][%d] 	WRITE 0x%08X: 0x%016lX\n", x, i, base_address + IOM_d[i].addr, IOM_d[i].data[x]);
#if 0
		temp_data = DCRReadPriv(base_address + IOM_d[i].addr);
//		printf("[%d][%d] 	READ  0x%08X: 0x%016lX\n", x, i, base_address + IOM_d[i].addr, temp_data);
		if(temp_data!=IOM_d[i].data[x])
			printf("	Mismatch!!!\n");
		else
			;
//			printf("	OK...\n");
#endif
	}

	//printf("IOM indirect register write: satellite-%d\n",x);
	for(i=0,j=0; i<sizeof(IOM_i)/sizeof(IOM_i[0]);++i)
	{
       //if (!(i%10)) printf("step %d\n", i);
		for(R=0; R==0 || (R< 4*0x200 && (IOM_i[i].type & RRRR)); R+=0x200)
		for(N=0; N==0 || (N< 4*0x100 && (IOM_i[i].type & NNNN)); N+=0x100)
		for(B=0; B==0 || (B< 9*0x020 && (IOM_i[i].type & BBBB)); B+=0x020)
		for(b=0; b==0 || (b<16*0x001 && (IOM_i[i].type & bbbb)); b+=0x001)
		{
			temp_addr = (IOM_i[i].addr + N + R + B + b) << 16 | IOM_i_data[x][j];
			DCRWritePriv(base_address + 0x00, temp_addr);
//			printf("[%d][%d]:%d 	WRITE 0x%08X->0x%04X: 0x%04X\n", x, i, j, base_address + 0x00, (unsigned) (temp_addr >> 16), IOM_i_data[x][j]);
			DCRWritePriv(base_address + 0x01, 0xFFFF0000 & temp_addr);
#if 0
			temp_data = DCRReadPriv(base_address + 0x01);
			if((0xFFFF & temp_data)!=IOM_i_data[x][j])
				printf("[%d][%d]:%d 	READ  0x%08X->0x%04X: 0x%04lX	Mismatch!!!\n", x, i, j, base_address + 0x01, (unsigned) ((0xFFFF0000 & temp_data) >> 16), 0xFFFF & temp_data);
			else
				;
//				printf("[%d][%d]:%d 	READ  0x%08X->0x%04X: 0x%04lX	OK...\n", x, i, j, base_address + 0x01, (unsigned) ((0xFFFF0000 & temp_data) >> 16), 0xFFFF & temp_data);
#endif
            
			j++;
		}
	}
    
#if 0
	if(j!=IOM_I_DATA_SIZE)
		printf("\nIOM_i_data array size is different than expected!!!\n");
	printf("IOM write completed: satellite-%d\n",x);
#endif
}
*/
