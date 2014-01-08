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
/******************************************************************************
 * Library for misc support e.g. DCR
 * FIXME: separate DCR stuff into a separate file
 * Stefan Koch
 *****************************************************************************/

#define _BGQ_SD_HELPER_C_

//#define FORCE_ALL_USED
//#define SD_DCR_CHECK
//#define SD_DCR_SIMULATE

#include <stdio.h>
#include <hwi/include/bqc/dcr_support.h>
#include <firmware/include/fwext/fwext_lib.h>
#include <firmware/include/fwext/serdes.h>
#include <firmware/include/fwext/sd_helper.h>
#include <hwi/include/bqc/testint_dcr.h>

#define USE_TORUS(p,link) ND_GET_TORUS(link, p->Network_Config.NetFlags)
#define USE_IO(p)         ND_GET_ENABLE_ION_LINK(p->Network_Config.NetFlags)
#define LOOPMODE(p,link)  ND_GET_LOOPBACK(link,p->Network_Config.NetFlags)

void personality_convert_map(Personality_t *p)
{
#ifdef FORCE_ALL_USED
	use_a = use_b = use_c = use_d = use_e = use_io = 1;
	use_am = use_ap = 1;
	use_bm = use_bp = 1;
	use_cm = use_cp = 1;
	use_dm = use_dp = 1;
	use_em = use_ep = 1;
	return;
#endif
	/************************************************/
	/* first see what dimenuions are used for Torus */
	use_a  = USE_TORUS(p, 0)? 1: 0;
	use_b  = USE_TORUS(p, 1)? 1: 0;
	use_c  = USE_TORUS(p, 2)? 1: 0;
	use_d  = USE_TORUS(p, 3)? 1: 0;

	/* FIXME E can also be PCIE */
	use_e  = USE_TORUS(p, 4)? 1: 0;
	use_io = USE_IO(p)?       1: 0;

	/***************************************************/
	/* second see what link is in serdes loopback      */
	/* for now only torus+serdes_loopback will be init */
      /* Since we also have external loopback we cannot disable it here - MWK
	use_a  &= (LOOPMODE(p,0) == ND_LOOPBACK_SERDES)? 1: 0;
	use_b  &= (LOOPMODE(p,1) == ND_LOOPBACK_SERDES)? 1: 0;
	use_c  &= (LOOPMODE(p,2) == ND_LOOPBACK_SERDES)? 1: 0;
	use_d  &= (LOOPMODE(p,3) == ND_LOOPBACK_SERDES)? 1: 0;
	use_e  &= (LOOPMODE(p,4) == ND_LOOPBACK_SERDES)? 1: 0;
	use_io &= (LOOPMODE(p,5) == ND_LOOPBACK_SERDES)? 1: 0;
      */
	use_am = use_ap = use_a;
	use_bm = use_bp = use_b;
	use_cm = use_cp = use_c;
	use_dm = use_dp = use_d;
	use_em = use_ep = use_e;
}

void sd_dcr_wr(int addr, uint64_t val)
{
	if (0 != libfw_sd_debug) {
		printf("DCR-DEBUG: write addr=0x%08x value=0x%016lx\n", addr, val);
#ifdef SD_DCR_SIMULATE
		return;
#endif
	}
	DCRWritePriv(addr, val);

#ifdef SD_DCR_CHECK
	uint64_t cval;
	cval = DCRReadPriv(addr);
	if (cval != val) {
		Terminate(-1);
	}
#endif
}

uint64_t sd_dcr_rd(int addr)
{
	uint64_t val;
#ifdef SD_DCR_SIMULATE
	printf("DCR READ\n");
	return;
#endif

	val = DCRReadPriv(addr);
	if (0 != libfw_sd_debug) {
		printf("DCR-DEBUG: read  addr=0x%08x value=0x%016lx\n", addr, val);
	}
	return val;
}

//wrappers for desi stuff
void dcr_write(int addr, uint64_t val, int d0, int d1, int d2, int d3)
{
	sd_dcr_wr(addr, val);
}

uint64_t dcr_read(int addr, int d0, int d1, int d2)
{
	return sd_dcr_rd(addr);
}

void sd_get_frequencies(uint32_t *coreFrequency, uint32_t *networkFrequency) {
  uint64_t frequencyDCR     = DCRReadPriv(TESTINT_DCR(CLKTREE_CTRL18));

  // Determine the core frequency:
  // DCR bits 12-13:
  // 00 ==> 1600 MHz
  // 01 ==>  800
  // 10 ==> 1200    
  switch ((frequencyDCR & _B2(13,0xF))) {
  case 0:
    *coreFrequency = 1600;
    break;
  case _B2(13,0x01):
    *coreFrequency = 800;
    break;
  case _B2(13,0x02):
    *coreFrequency = 1200;
    break;
  default:
    printf("Core frequency DCR = 0x%016lx, not expected frequency of 1600, 800, or 1200\n",frequencyDCR);
#ifdef __FWEXT__
    test_exit(1);
#else
    exit (1);
#endif
    break;
  }
    
  // Determine the network frequency:
  // DCR bit 14:
  // 0 ==> Full speed 500 MHz
  // 1 ==> Half speed 250
  switch ((frequencyDCR & _BN(14))) {
  case 0:
    *networkFrequency = 500;
    break;
  case _BN(14):
    *networkFrequency = 250;
    break;
  default:
    printf("Network frequency DCR = 0x%016lx, not expected frequency of 500 or 250\n",frequencyDCR);
#ifdef __FWEXT__
    test_exit(1);
#else
    exit (1);
#endif
    break;
  }
}
