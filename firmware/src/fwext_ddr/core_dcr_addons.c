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
#include <firmware/include/fwext/sd_init_auto.h>
#include <firmware/include/fwext/sd_helper.h>
//WARNING: only last byte of address is taken, (rcb space not shadowed)
//left  0x140000
//right 0x144000
//            ^^-taken for address (7:0 from right)
//          ^----taken for side (14 from right)
//shadow base address is
//left     0x000
//right    0x100
#define DCR_SHADOW(addr)       (sd_dcr_shadow      [ (0x100&(addr>>6)) | (addr&0xff) ])
#define DCR_SHADOW_AVAIL(addr) (sd_dcr_shadow_avail[ (0x100&(addr>>6)) | (addr&0xff) ])

static uint64_t sd_dcr_shadow[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*   0..  1f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*  20..  3f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*  40..  5f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*  60..  7f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*  80..  9f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*  a0..  bf */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*  c0..  df */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*  e0..  ff */

 				   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 100.. 11f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 120.. 13f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 140.. 15f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 160.. 17f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 180.. 19f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 1a0.. 1bf */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 1c0.. 1df */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  /* 1e0.. 1ff */};

static char sd_dcr_shadow_avail[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*   0..  1f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*  20..  3f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*  40..  5f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*  60..  7f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*  80..  9f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*  a0..  bf */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*  c0..  df */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /*  e0..  ff */

 				   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 100.. 11f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 120.. 13f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 140.. 15f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 160.. 17f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 180.. 19f */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 1a0.. 1bf */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 1c0.. 1df */
	                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  /* 1e0.. 1ff */};

// args
// 1 - address
// 2 - value
void dcr_wr (int addr, uint64_t val) 
{

	dcr_write(addr, val, 1, 0, 0, 0);

	// update shadow reg
	DCR_SHADOW(addr)=val;
	DCR_SHADOW_AVAIL(addr)=1;
}

// args
// 1 - address
uint64_t dcr_rd (int addr) 
{
	uint64_t v;

	v = dcr_read(addr, 1, 0, 0);

	// update shadow reg
	DCR_SHADOW(addr)=v;
	DCR_SHADOW_AVAIL(addr)=1;

	return v;
}

// args
// 1 - address
// 2 - bitmask for turning on bits
void dcr_set (int addr, uint64_t setmask) 
{
	if (DCR_SHADOW_AVAIL(addr) == 0) {
		//printf("dcr_set: refresh 0x%06x to shadow reg\n", addr);
		DCR_SHADOW(addr) = dcr_rd(addr);
		DCR_SHADOW_AVAIL(addr)  = 1;
	}
	dcr_wr(addr, DCR_SHADOW(addr) | setmask);
}

// args
// 1 - address
// 2 - bitmask for turning on bits
void dcr_set_acc (int addr, uint64_t setmask) 
{
	if (DCR_SHADOW_AVAIL(addr) == 0) {
		//printf("dcr_set_acc: refresh 0x%06x to shadow reg\n", addr);
		DCR_SHADOW(addr) = dcr_rd(addr);
		DCR_SHADOW_AVAIL(addr)  = 1;
	}
	DCR_SHADOW(addr) |= setmask;
}

// args
// 1 - address
// 2 - bitmask for turning off bits
void dcr_clr_acc (int addr, uint64_t clrmask) 
{
	if (DCR_SHADOW_AVAIL(addr) == 0) {
		//printf("dcr_clr_acc: refresh 0x%06x to shadow reg\n", addr);
		DCR_SHADOW(addr) = dcr_rd(addr);
		DCR_SHADOW_AVAIL(addr)  = 1;
	}
	DCR_SHADOW(addr) &= ~clrmask;
}

// args
// 1 - address
void dcr_acc_flush (int addr) 
{
	if (DCR_SHADOW_AVAIL(addr) == 0) {
		return;
	}
	dcr_wr(addr, DCR_SHADOW(addr));
}

// args
// 1 - address
// 2 - bitmask for clearing bits
void dcr_clr (int addr, uint64_t clrmask) 
{
	if (DCR_SHADOW_AVAIL(addr) == 0) {
		//printf("dcr_clr: refresh 0x%06x to shadow reg\n", addr);
		DCR_SHADOW(addr) = dcr_rd(addr);
		DCR_SHADOW_AVAIL(addr)  = 1;
	}

	dcr_wr(addr, DCR_SHADOW(addr) & ~clrmask);
}
