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
#if 0
#include <firmware/include/fwext/fwext_lib.h>
#include <hwi/include/bqc/testint_dcr.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/dc_arbiter_dcr.h>
#include <hwi/include/bqc/dr_arb_dcr.h>
#include <hwi/include/bqc/ddr.h>
#include <hwi/include/common/bgq_bitnumbers.h>
#endif
#include <stdint.h>

#define	IOM_I_DATA_SIZE	1231

#define	RRRR	0x8	// Rank
#define	BBBB	0x4	// Byte
#define	NNNN	0x2	// instaNce
#define	bbbb	0x1	// bit

#define	DELTA_ARB_B_A	(DR_ARB_DCR__B_base - DR_ARB_DCR__A_base)


struct	IOM_direct_t {unsigned addr; uint64_t data[4];};
struct	IOM_indirect_addr_t {unsigned addr; char type;};	// type = RBNb

static struct IOM_direct_t IOM_d[]=
{
	{0x02,{0,0,0,0}},
	{0x03,{0,0,0,0}},
	{0x04,{0,0,0,0}},
	{0x05,{0,0,0,0}},
	{0x06,{0,0,0,0}},
	{0x07,{0,0,0,0}},
	{0x0A,{0,0,0,0}},
	{0x0B,{0,0,0,0}},
	{0x0C,{0,0,0,0}},
	{0x0D,{0,0,0,0}}
};

static struct IOM_indirect_addr_t IOM_i[]=
{
	{0x0000,NNNN + bbbb},
	{0x8010,NNNN},
	{0x8011,NNNN},
	{0x8012,NNNN},
	{0x8013,NNNN},
	{0x00FF,NNNN},
	{0x00F6,NNNN},
	{0x2000,RRRR + bbbb},
	{0x2004,RRRR + bbbb},
	{0x3168,RRRR},
	{0x3F88,0},
	{0x200F,BBBB},
	{0x200E,BBBB},
	{0x200D,BBBB},
	{0x2FFE,0},
	{0x200C,BBBB},
	{0x3FFC,0},
	{0x2FFD,0},
	{0x2FFF,0},
	{0x8400,BBBB},
	{0x8401,BBBB},
	{0x8402,BBBB},
	{0x8403,BBBB},
	{0x4000,RRRR + BBBB},
	{0x4001,RRRR + BBBB},
	{0x4002,RRRR + BBBB},
	{0x4003,BBBB},
	{0x5003,BBBB},
	{0x4004,BBBB},
	{0x4005,BBBB},
	{0x4006,BBBB},
	{0x4007,BBBB},
	{0x4008,RRRR + BBBB + bbbb},
	{0x5000,RRRR + BBBB},
	{0x5001,RRRR + BBBB},
	{0x5002,BBBB},
	{0x6000,RRRR + BBBB},
	{0x71E0,RRRR},
	{0x7FF0,0},
	{0x6001,BBBB},
	{0x71E1,0},
	{0x6002,RRRR},
	{0x6003,BBBB},
	{0x6004,BBBB},
	{0x6FE7,0},
	{0x6FE8,0},
	{0x6FF9,0},
	{0x6FFB,0},
	{0x6FEC,0},
	{0x6FFD,0},
	{0x6FEA,0},
	{0x6FFE,0},
	{0x6FE5,0},
	{0x6FFF,0},
	{0x6FF6,0},
	{0x6FF3,0},
	{0xEFF5,0},
	{0xE005,0},
	{0xE004,BBBB},
	{0xEFFD,0},
	{0xEFFC,0},
	{0xEFF4,0},
	{0xEFFF,0},
	{0xEFFE,0},
	{0xE000,BBBB},
	{0xE001,0},
	{0xE002,0},
	{0xE003,0},
	{0xE007,0},
	{0xE000,0}
};

unsigned short	IOM_i_data[4][IOM_I_DATA_SIZE];
