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

//! \addtogroup upci_spi
//@{
/**
 * \file
 *
 * \brief Low Level A2
 *  Event Select Registers. See A2 user guide section 11.5 "Unit event select registers"
 */
//@}


#define UPC_A2_C

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/common/bgq_bitnumbers.h>

#include "spi/include/kernel/upci.h"
#include "spi/include/upci/upc_a2.h"




// SPR access:
// To read:
// uint64_t val;
// val = mfspr(SPRN_xxxx);
// To write:
// mtspr(SPRN_xxxx, val);

// Macros in hwi/include/common/bgq_bitnumbers.h help extact and manipulate
// bit fields within the registers.
// These defines allows use of IBM's bit numberings (MSb=0, LSb=63) for multi-bit fields
//  b = IBM bit number of the least significant bit (highest number)
//  x = value to set in field
//  s = size
//  Write Syntax:   _BS64(b,x,s) Mask off s bits of x. shift (63-b) positions to the left.
//  Read  Syntax:   _BG64(b,x,s) Shift x (63-b) bits to the right and return right s bits.

// The A2 Event Select registers control selection of 8 event signals
//   signals 0 - 3 connect to thread 0 & 1 events.
//   signals 4 - 7 connect to thread 2 & 3 events.



void set_cesr_event(short unit_sig_num, cesr_a2_unit_t unit_type)
{
   short offset  = 39+3 + (unit_sig_num*3);   // before left most bit + field length
   uint64_t mask = _BS64( offset, 0x7, 3 );
   uint64_t val  = _BS64( offset, unit_type, 3 );
   mtspr(SPRN_CESR, ( (~mask & mfspr(SPRN_CESR)) | val) );
}



void get_cesr_event(short unit_sig_num, short * unit_type)
{
   short offset  = 39+3 + (unit_sig_num*3);   // before left most bit + field length
   uint64_t val = mfspr( SPRN_CESR );
   val          = _BG64(offset,val,3);
   *unit_type   = (short) (val & 0x7);
}



void Clear_A2_Events(cesr_count_modes_t mode)
{
    mtspr(SPRN_CESR, 0 );
    mtspr(SPRN_AESR, 0 );
    mtspr(SPRN_IESR1, 0 );
    mtspr(SPRN_IESR2, 0 );
    mtspr(SPRN_MESR1, 0 );
    mtspr(SPRN_MESR2, 0 );
    mtspr(SPRN_XESR1, 0 );
    mtspr(SPRN_XESR2, 0 );
    mtspr(SPRN_XESR3, 0 );
    mtspr(SPRN_XESR4, 0 );
}





void Init_A2_Counting(cesr_count_modes_t mode)
{
    uint64_t cesr = mfspr(SPRN_CESR);
    cesr &= SPRN_CESR__EVENTS_MASK;  // Mask off all but events bits.
    cesr |= SPRN_CESR__ENABLE_PERF_set(1) | SPRN_CESR__COUNT_MODES_set(mode) | SPRN_CESR__ENABLE_TRACE_BUS_set(1);
    mtspr(SPRN_CESR, cesr );
}



void set_axu_event(short unit_sig_num, short thread, short a2_event)
{
   short offset    = 31+4+(unit_sig_num*4);   /* 1 bit for 2 threads, 3 bits for 8 QFPU events */
   uint64_t mask = _BS64( offset, 0xF, 4 );
   uint64_t val  = _BS64( offset, ((0x1 & thread) << 3) | (0x7 & a2_event), 4 );
   mtspr(SPRN_AESR, ( (~mask & mfspr(SPRN_AESR)) | val) );
}



void get_axu_event(short unit_sig_num, short *thread, short *a2_event)
{
   //short t = GetTimeBase();
   //printf("Executing get_aesr_event(%d) at cycle %d\n", event, t);
   short offset   = 31+4+(unit_sig_num*4);  /* 1 bits for 2 threads, 3 bits for 8 QFPU events */
   uint64_t val = mfspr( SPRN_AESR );
   val          = _BG64(offset,val,4);
   *thread      = (short) (val >> 3) + ((unit_sig_num&0x4)>>1);  // sigs 4 - 8 are threads 2 & 3
   *a2_event    = (short) (val & 0x7);
}



void set_iu_event(short unit_sig_num, short thread, short a2_event)
{
	short use_2nd_spr  = (0x4 & unit_sig_num);             // spr1 for signals 0-3, spr2 for signals 4-7.
	short offset       = 31+6+( (0x3 & unit_sig_num)*6);   /* 1 bits for 2 threads, 5 bits for 32 a2_events */
	uint64_t mask    = _BS64( offset, 0x3F, 6 );
	uint64_t val     = _BS64( offset, ((0x1 & thread) << 5) | (0x1F & a2_event), 6 );
	uint64_t spr_val = (use_2nd_spr) ? mfspr(SPRN_IESR2) : mfspr(SPRN_IESR1);
	spr_val          = (~mask & spr_val) | val;
	if (use_2nd_spr) mtspr(SPRN_IESR2, spr_val);
	else             mtspr(SPRN_IESR1, spr_val);
}

void get_iu_event(short unit_sig_num, short *thread, short *a2_event)
{
	short use_2nd_spr  = (0x4 & unit_sig_num);            // spr0 for signals 0-3, spr1 for signals 4-7.
	short offset       = 31+6+( (0x3 & unit_sig_num)*6);  /* 1 bit for 2 threads, 5 bits for 32 a2_events */
	uint64_t val     = (use_2nd_spr) ? mfspr(SPRN_IESR2) : mfspr(SPRN_IESR1);
	val              = _BG64(offset,val,6);
	*thread          = (short) (val >> 5) + ((unit_sig_num&0x4)>>1);  // sigs 4 - 8 are threads 2 & 3
	*a2_event        = (short) (val & 0x1F);
}



void set_xu_event(short unit_sig_num, short thread, short a2_event)
{
	short use_2nd_spr  = (0x4 & unit_sig_num);             // spr1 for signals 0-3, spr2 for signals 4-7.
	short offset       = 31+6+( (0x3 & unit_sig_num)*6);   /* 1 bits for 2 threads, 5 bits for 32 a2_events */
	uint64_t mask    = _BS64( offset, 0x3F, 6 );
	uint64_t val     = _BS64( offset, ((0x1 & thread) << 5) | (0x1F & a2_event), 6 );
	uint64_t spr_val = (use_2nd_spr) ? mfspr(SPRN_XESR2) : mfspr(SPRN_XESR1);
	spr_val          = (~mask & spr_val) | val;
	if (use_2nd_spr) mtspr(SPRN_XESR2, spr_val);
	else             mtspr(SPRN_XESR1, spr_val);
}

void get_xu_event(short unit_sig_num, short *thread, short *a2_event)
{
	short use_2nd_spr  = (0x4 & unit_sig_num);            // spr0 for signals 0-3, spr1 for signals 4-7.
	short offset    = 31+6+( (0x3 & unit_sig_num)*6);  /* 1 bit for 2 threads, 5 bits for 32 a2_events */
	uint64_t val  = (use_2nd_spr) ? mfspr(SPRN_XESR2) : mfspr(SPRN_XESR1);
	val           = _BG64(offset,val,6);
	*thread       = (short) (val >> 5) + ((unit_sig_num&0x4)>>1);  // sigs 4 - 8 are threads 2 & 3
	*a2_event     = (short) (val & 0x1F);
}



void set_lsu_event(short unit_sig_num, short thread, short a2_event)
{
	short use_2nd_spr  = (0x4 & unit_sig_num);             // spr1 for signals 0-3, spr2 for signals 4-7.
	short offset       = 31+6+( (0x3 & unit_sig_num)*6);   /* 1 bits for 2 threads, 5 bits for 32 a2_events */
	uint64_t mask    = _BS64( offset, 0x3F, 6 );
	uint64_t val     = _BS64( offset, ((0x1 & thread) << 5) | (0x1F & a2_event), 6 );
	uint64_t spr_val = (use_2nd_spr) ? mfspr(SPRN_XESR4) : mfspr(SPRN_XESR3);
	spr_val          = (~mask & spr_val) | val;
	if (use_2nd_spr) mtspr(SPRN_XESR4, spr_val);
	else             mtspr(SPRN_XESR3, spr_val);
}

void get_lsu_event(short unit_sig_num, short *thread, short *a2_event)
{
	short use_2nd_spr  = (0x4 & unit_sig_num);            // spr0 for signals 0-3, spr1 for signals 4-7.
	short offset    = 31+6+( (0x3 & unit_sig_num)*6);  /* 1 bit for 2 threads, 5 bits for 32 a2_events */
	uint64_t val  = (use_2nd_spr) ? mfspr(SPRN_XESR4) : mfspr(SPRN_XESR3);
	val           = _BG64(offset,val,6);
	*thread       = (short) (val >> 5) + ((unit_sig_num&0x4)>>1);  // sigs 4 - 8 are threads 2 & 3
	*a2_event     = (short) (val & 0x1F);
}



void set_mmu_event(short unit_sig_num, short thread, short a2_event)
{
	short use_2nd_spr  = (0x4 & unit_sig_num);             // spr1 for signals 0-3, spr2 for signals 4-7.
	short offset       = 31+5+( (0x3 & unit_sig_num)*5);   /* 1 bits for 2 threads, 4 bits for 16 a2_events */
	uint64_t mask    = _BS64( offset, 0x1F, 5 );
	uint64_t val     = _BS64( offset, ((0x1 & thread) << 4) | (0xF & a2_event), 5 );
	uint64_t spr_val = (use_2nd_spr) ? mfspr(SPRN_MESR2) : mfspr(SPRN_MESR1);
	spr_val          = (~mask & spr_val) | val;
	if (use_2nd_spr) mtspr(SPRN_MESR2, spr_val);
	else             mtspr(SPRN_MESR1, spr_val);
}



void get_mmu_event(short unit_sig_num, short *thread, short *a2_event)
{
	short use_2nd_spr  = (0x4 & unit_sig_num);            // spr0 for signals 0-3, spr1 for signals 4-7.
	short offset    = 31+5+( (0x3 & unit_sig_num)*5);  /* 1 bit for 2 threads, 5 bits for 16 a2_events */
	uint64_t val  = (use_2nd_spr) ? mfspr(SPRN_MESR2) : mfspr(SPRN_MESR1);
	val           = _BG64(offset,val,5);
	*thread       = (short) (val >> 4) + ((unit_sig_num&0x4)>>1);  // sigs 4 - 8 are threads 2 & 3
	*a2_event     = (short) (val & 0xF);
}




