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


#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <firmware/include/Firmware.h>
#include "Firmware_internals.h"



// #####################################################################################
// NOTE: code bracket via the FW_SUPPORTS_ERAT flag preserves the ERAT initialization
//       sequence provided by the original firmware.  It can be removed at some point
//       when MMU support completely stabilizes.
// #####################################################################################

#define FW_SUPPORTS_ERAT

#ifdef FW_SUPPORTS_ERAT

__INLINE__ void write_derat(unsigned index, uint64_t hi, uint64_t lo) {
  mtspr(SPRN_MMUCR0, MMUCR0_TLBSEL_DERAT);
  asm volatile(
	       "eratwe %1,%0,1;"
	       "eratwe %2,%0,0;"
	       : : "r" (index), "r" (lo), "r" (hi)
	       : "memory"
	       );
}

__INLINE__ void write_ierat(unsigned index, uint64_t hi, uint64_t lo) {
  mtspr(SPRN_MMUCR0, MMUCR0_TLBSEL_IERAT);
  asm volatile(
	       "eratwe %1,%0,1;"
	       "eratwe %2,%0,0;"
	       : : "r" (index), "r" (lo), "r" (hi)
	       : "memory"
	       );
}

int fw_mmu_init_erat_for_l2_only( void )
{
    /* -------------------------------------------------------------------
     * Map 16MB of instructions and 31MB of data.  The L2 holds 32MB, but
     * we can't map the last 4KB without risking a prefetch beyond 32MB.
     * We leave out the whole last megabyte to make things simple.  Even 31MB
     * is beyond what we can do with the I-ERAT, so we just assume that 16MB
     * is enough for instructions.
     * ------------------------------------------------------------------- */

    int i;
    uint64_t ddrPhysicalAddress = FW_InternalState.nodeState.domain[fw_getThisDomainID()].ddrOrigin;

    write_ierat( 15,
		 ERAT0_EPN(0) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_16M | ERAT0_ThdID(0xF),
		 ERAT1_RPN(ddrPhysicalAddress) | ERAT1_R | ERAT1_C | ERAT1_M | ERAT1_UX | ERAT1_UW | ERAT1_UR | ERAT1_SX | ERAT1_SW | ERAT1_SR
		 );

    write_derat( 31,
		 ERAT0_EPN(0) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_16M | ERAT0_ThdID(0xF),
		 ERAT1_RPN(ddrPhysicalAddress) | ERAT1_R | ERAT1_C | ERAT1_M | ERAT1_UX | ERAT1_UW | ERAT1_UR | ERAT1_SX | ERAT1_SW | ERAT1_SR
		 );

    /*
     * Current erat usage is a zoo.  These are the data erat entries currently
     * unused in fw_start.S, fw_mmu.c, and fwext_ddr/fwext_main.c.
     */
    int free_erat[] = {2,4,5,6,7,8,9,10,11,12,13,14,15,28,29,30};
    for ( i = 16; i < 31; i++ ) {
	write_derat( free_erat[i-16],
		     ERAT0_EPN(0 + (i << 20)) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1M | ERAT0_ThdID(0xF),
		     ERAT1_RPN(ddrPhysicalAddress + (i << 20)) | ERAT1_R | ERAT1_C | ERAT1_M | ERAT1_UX | ERAT1_UW | ERAT1_UR | ERAT1_SX | ERAT1_SW | ERAT1_SR
		     );
    }
  
    isync();
    return 0;
}

int fw_mmu_init_erat( void ) {

  if ( PERS_ENABLED(PERS_ENABLE_L2Only) ) {
    return fw_mmu_init_erat_for_l2_only();
  }

  // NOTE: only map DDR if enabled in the personality. 

  if ( PERS_ENABLED(PERS_ENABLE_DDR) ) {

    /* -------------------------------------------------------------------
     * Install a 1GB entry for both instructions and data for the low
     * region of DDR:
     * ------------------------------------------------------------------- */

    uint64_t ddrPhysicalAddress = FW_InternalState.nodeState.domain[fw_getThisDomainID()].ddrOrigin >> 12;

    // @todo: Why is this user accessible?

    write_ierat( 15,
		 ERAT0_EPN(0) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(ddrPhysicalAddress) | ERAT1_R | ERAT1_C | ERAT1_M | ERAT1_UX | ERAT1_UW | ERAT1_UR | ERAT1_SX | ERAT1_SW | ERAT1_SR | ERAT1_U3
		 );
    
    uint64_t speccap = 0;
    if(!PERS_ENABLED(PERS_ENABLE_SpecCapDDR))
    {
       speccap = ERAT1_U0;
    }
    write_derat( 31,
		 ERAT0_EPN(0) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(ddrPhysicalAddress) | ERAT1_R | ERAT1_C | ERAT1_M | ERAT1_UX | ERAT1_UW | ERAT1_UR | ERAT1_SX | ERAT1_SW | ERAT1_SR | ERAT1_U3 | speccap
		 );
  }

  
  isync();

  return 0;
}

#endif

void fw_mmu_init_tlb_for_l2_only()
{
    /* -------------------------------------------------------------------
     * Map 31MB for both instructions and data.  The L2 holds 32MB, but
     * we can't map the last 4KB without risking a prefetch beyond 32MB.
     * We leave out the whole last megabyte to make things simple.
     * ------------------------------------------------------------------- */

    int i;
    uint64_t ddrPhysicalAddress =
	FW_InternalState.nodeState.domain[fw_getThisDomainID()].ddrOrigin >> 12;

    tlbwe(
	  MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_16MB,
	  MAS2_EPN(0) | MAS2_W(0) | MAS2_I(0) | MAS2_M(1) | MAS2_G(0) | MAS2_E(0),
	  MAS7_3_RPN(ddrPhysicalAddress) | MAS3_U0(1) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(1) | MAS3_UR(0) | MAS3_UW(0) | MAS3_UX(0),
	  MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	  MMUCR3_X(0) | MMUCR3_R(1) | MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) | MMUCR3_ThdID(0xF)
	  );

    for (i = 16; i < 31; i++) {
	tlbwe(
	      MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1MB,
	      MAS2_EPN(0 + (i << (20-12))) | MAS2_W(0) | MAS2_I(0) | MAS2_M(1) | MAS2_G(0) | MAS2_E(0),
	      MAS7_3_RPN(ddrPhysicalAddress + (i << (20-12))) | MAS3_U0(1) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(1) |MAS3_UR(0) | MAS3_UW(0) | MAS3_UX(0),
	      MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	      MMUCR3_X(0) | MMUCR3_R(1) | MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) | MMUCR3_ThdID(0xF)
	      );
    }
}


int fw_mmu_init_tlb()
{
  /* -----------------------------------------
   * The chip might come up in ERAT-only mode
   * and so let's clear that bit:
   * ----------------------------------------- */

  mtspr( SPRN_CCR2, mfspr( SPRN_CCR2 ) & ~CCR2_NOTLB );

  /* -----------------------------------------
   * Privileged mapping for MMIO & BeDRAM:
   * ----------------------------------------- */

  // NOTE: we force this into way 3 of the TLB set in order to avoid an A2 defect 
  //       that does not properly honor IPROT (Linux relies on IPROT to keep the
  //       firmware TLB resident).

  tlbwe_slot(
      3,
      MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB | MAS1_IPROT(1),
      MAS2_EPN(   (PHYMAP_MINADDR_MMIO | PHYMAP_PRIVILEGEDOFFSET) >> 12) | MAS2_W(0) | MAS2_I(1) | MAS2_M(1) | MAS2_G(1) | MAS2_E(0),
      MAS7_3_RPN( (PHYMAP_MINADDR_MMIO | PHYMAP_PRIVILEGEDOFFSET) >> 12) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(1) | MAS3_UR(0) | MAS3_UW(0) | MAS3_UX(0) | MAS3_U1(1),
      MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
      MMUCR3_X(0) | MMUCR3_R(1) |	MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
      );


  // NOTE: only map DDR if enabled in the personality.  This is required to avoid any speculative
  //       accesses to DDR.

  if ( PERS_ENABLED(PERS_ENABLE_L2Only) ) {
      fw_mmu_init_tlb_for_l2_only();
  } 
  else if ( PERS_ENABLED(PERS_ENABLE_DDR) ) {

    uint64_t ddrPhysicalAddress = FW_InternalState.nodeState.domain[fw_getThisDomainID()].ddrOrigin >> 12;

    /* -------------------------------------------------------------------
     * Install a 1GB entry for both instructions and data for the low
     * region of DDR.  Note that the "low region" is defined physically
     * as the root of the domain.
     * ------------------------------------------------------------------- */

    tlbwe(
	  MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
	  MAS2_EPN(0)   | MAS2_W(0) | MAS2_I(0) | MAS2_M(1) | MAS2_G(0) | MAS2_E(0),
	  MAS7_3_RPN(ddrPhysicalAddress) | MAS3_U0(1) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(1) | MAS3_UR(0) | MAS3_UW(0) | MAS3_UX(0) | MAS3_U3(1),
	  MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	  MMUCR3_X(0) | MMUCR3_R(1) |	MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
	  );
  }

  isync();

  /* -------------------------------------------------------------------
   * Now scrape the ERATs clean.  Not that we do not remove the entries
   * for BeDRAM.  This is because the unified entry must make the area
   * cache-inhibited and guarded, and thus has a substantial impact to
   * cycle-sim time.  Perhaps this entry should be removed on real
   * hardware
   * ------------------------------------------------------------------- */
  
  int i, j;
  
  for (i = 0; i < 2; i++) {

    unsigned erat_size;

    if (i==0) {
      erat_size = 32;
      mtspr(SPRN_MMUCR0, MMUCR0_TLBSEL_DERAT);
    }
    else {
      erat_size = 16;
      mtspr(SPRN_MMUCR0, MMUCR0_TLBSEL_IERAT);
    }

    for (j = 0; j < erat_size; j++) {
      uint64_t erat_entry_hi;
      asm volatile( "eratre %0,%1,0;" : "=&r" (erat_entry_hi) : "r" (j) : "memory" );
      if (erat_entry_hi & ERAT0_V) {

	if ( ERAT0_GET_EPN(erat_entry_hi) == ( ( PHYMAP_MINADDR_MMIO | PHYMAP_PRIVILEGEDOFFSET ) >> 12 ) ) {
	  continue;
	}

	if (i == 0) {
	  write_derat(j, 0, 0);
	}
	else {
	  write_ierat(j, 0, 0);
	}
      }
    }
  }

  return 0;
}

int fw_mmu_init()
{
  int rc = 0;

  // We synchronize at the core level before and after changing the address
  // space, to avoid yanking old mappings out from under lagging threads, and
  // to prevent threads from racing ahead and using new mappings before they
  // are ready.
  fw_core_barrier();

  if ( ProcessorThreadID() == 0 ) {
#ifdef FW_SUPPORTS_ERAT
    if ( ! PERS_ENABLED(PERS_ENABLE_MMU) )
    {
      rc = fw_mmu_init_erat();
    }
    else
#endif
    {
      rc = fw_mmu_init_tlb();
    }
  }

  fw_core_barrier();

  return rc;
}

int fw_mmu_remap( void ) {

    int rc = 0;

  // We synchronize at the core level before and after changing the address
  // space, to avoid yanking old mappings out from under lagging threads, and
  // to prevent threads from racing ahead and using new mappings before they
  // are ready.
  fw_core_barrier();

  if ( ProcessorThreadID() == 0 ) {

      if ( PERS_ENABLED(PERS_ENABLE_MMU) && PERS_ENABLED(PERS_ENABLE_DDR) ) {

	  uint64_t domain = fw_getThisDomainID();
	  uint64_t index = tlbsx( 0x0000000000000000ul, MAS5_SGS(0) | MAS5_SLPID(0), MAS6_SPID(0) | MAS6_SAS(0) );

	  if ( index != -1 ) {

	      uint64_t rpn = mfspr( SPRN_MAS7_MAS3 ) & 0xFFFFFFFF000ull;

	      if ( rpn != FW_InternalState.nodeState.domain[domain].ddrOrigin ) {
		  rpn = FW_InternalState.nodeState.domain[domain].ddrOrigin >> 12;
		  tlbwe_slot(
		      index, 
		      MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
		      MAS2_EPN(0)   | MAS2_W(0) | MAS2_I(0) | MAS2_M(1) | MAS2_G(0) | MAS2_E(0),
		      MAS7_3_RPN(rpn) | MAS3_U0(1) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(1) | MAS3_UR(0) | MAS3_UW(0) | MAS3_UX(0) | MAS3_U3(1),
		      MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
		      MMUCR3_X(0) | MMUCR3_R(1) |	MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
		      );

	      }
	  }
	  else {
	      rc = - __LINE__;
	  }
      }
      else { // Unsupported mode
	  rc = -__LINE__;
      }
  }

  fw_core_barrier();

  return rc;

}
