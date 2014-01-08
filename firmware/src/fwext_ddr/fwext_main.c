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



#include <firmware/include/fwext/fwext.h>
#include <firmware/include/personality.h>
#include <firmware/include/Firmware.h>
#include <hwi/include/bqc/testint_tdr.h>
#include <hwi/include/bqc/testint_inlines.h>
#include <firmware/include/fwext/fwext_lib.h>
#include "Firmware_internals.h"


FwExt_State_t FWEXT_DATA FWExtState ALIGN_L2_CACHE;

int test_main( void );    // see test_main.c

void Kernel_Exit( int status );
void Kernel_Crash( int status );

extern void* _fwext_Vec_MCHK;





void fwext_barrier(FwExt_Barrier_t* b, unsigned numberOfThreads) {

    isync();
    uint64_t target = b->start + numberOfThreads;
    uint64_t current = Fetch_and_Add( 
				     (uint64_t*)&(b->count), 
				     1ull
				     ) + 1;

    if (current == target) {
	b->start = current;
	mbar();
    } 
    else {
      while (b->start < current) /*spin*/;
    }
}

void fwext_coreBarrier( void ) {
  fwext_barrier( FWExtState.coreBarriers + ProcessorCoreID(), THREADS_PER_CORE );
}

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

Personality_t* getPersonality()
{
    return FWExtState.personality;
}

void FWExt_InitMMU( void ) {
   uint64_t n;
   uint64_t ddrOrigin = FWExtState.domain.ddrOrigin;

   
  if ( FWExtState.personality->Kernel_Config.NodeConfig & PERS_ENABLE_MMU ) {
    /* -----------------------------------------
     * In +MMU mode, we map all of this domain's ddr.
     * ----------------------------------------- */
    static uint64_t page_size[3] = {0x40000000ul, 0x01000000ul, 0x00100000ul};
    static uint64_t mas1_tsize[3] = {MAS1_TSIZE_1GB, MAS1_TSIZE_16MB,
							MAS1_TSIZE_1MB};
    uint64_t address = 0x40000000ul;  // 1GB is already mapped
    for (n = 0; n < 3; n++)
    {
      while ((address + page_size[n]) <= FWExtState.vaddrLimit)
      {
	tlbwe(
	      MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | mas1_tsize[n],
	      MAS2_EPN(address >> 12) |
		MAS2_W(0) | MAS2_I(0) | MAS2_M(1) | MAS2_G(0) | MAS2_E(0),
	      MAS7_3_RPN((ddrOrigin + address) >> 12) |
		MAS3_U0(1) | MAS3_U3(1) |
		MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(1) |
		MAS3_UR(0) | MAS3_UW(0) | MAS3_UX(0),
	      MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	      MMUCR3_X(0) | MMUCR3_R(1) | MMUCR3_C(1) | MMUCR3_ECL(0) |
		MMUCR3_CLASS(1) | MMUCR3_ThdID(0xF)
	      );
	address += page_size[n];
      }
    }

    /* -----------------------------------------
     * User-space mapping for MMIO:
     * ----------------------------------------- */

    tlbwe(
	  MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
	  MAS2_EPN(  PHYMAP_MINADDR_MMIO >> 12) | MAS2_W(0) | MAS2_I(1) |	MAS2_M(1) | MAS2_G(1) | MAS2_E(0),
	  MAS7_3_RPN(PHYMAP_MINADDR_MMIO >> 12) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(0) | MAS3_UR(1) | MAS3_UW(1) | MAS3_UX(0) | MAS3_U1(1),
	  MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	  MMUCR3_X(0) | MMUCR3_R(1) |	MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
	  );

    /* ------------------------------------------------------------
     * Install four 1GB data entries for Atomic Ops starting at
     * 0x100_0000_0000:
     * ------------------------------------------------------------ */

    // @todo 1) Why are these user-space accessible?  2) Why are they executable?

    uint64_t speccap = 0;
    if(!PERS_ENABLED(PERS_ENABLE_SpecCapDDR))
    {
       speccap = MAS3_U0(1);
    }
    
    tlbwe(
	  MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
	  MAS2_EPN(  (VA_MINADDR_CATOMICOPS + (ddrOrigin<<5) + (0ul<<30)) >>12 ) | MAS2_W(0) | MAS2_I(1) | MAS2_M(1) | MAS2_G(1) | MAS2_E(0),
	  MAS7_3_RPN((PHYMAP_MINADDR_CATOMICOPS + (ddrOrigin<<5) + (0ul<<30)) >>12 ) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(0) | MAS3_UR(1) | MAS3_UW(1) | MAS3_UX(0) | speccap,
	  MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	  MMUCR3_X(0) | MMUCR3_R(1) |	MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
	  );

    tlbwe(
	  MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
	  MAS2_EPN(  (VA_MINADDR_CATOMICOPS     + (ddrOrigin<<5) + (1ul<<30)) >>12 ) | MAS2_W(0) | MAS2_I(1) | MAS2_M(1) | MAS2_G(1) | MAS2_E(0),
	  MAS7_3_RPN((PHYMAP_MINADDR_CATOMICOPS + (ddrOrigin<<5) + (1ul<<30)) >>12 ) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(0) | MAS3_UR(1) | MAS3_UW(1) | MAS3_UX(0) | speccap,
	  MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	  MMUCR3_X(0) | MMUCR3_R(1) |	MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
	  );

    tlbwe(
	  MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
	  MAS2_EPN(  (VA_MINADDR_CATOMICOPS     + (ddrOrigin<<5) + (2ul<<30)) >>12 ) | MAS2_W(0) | MAS2_I(1) | MAS2_M(1) | MAS2_G(1) | MAS2_E(0),
	  MAS7_3_RPN((PHYMAP_MINADDR_CATOMICOPS + (ddrOrigin<<5) + (2ul<<30)) >>12 ) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(0) | MAS3_UR(1) | MAS3_UW(1) | MAS3_UX(0) | speccap,
	  MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	  MMUCR3_X(0) | MMUCR3_R(1) |	MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
	  );

    tlbwe(
	  MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
	  MAS2_EPN(  (VA_MINADDR_CATOMICOPS     + (ddrOrigin<<5) + (3ul<<30)) >>12 ) | MAS2_W(0) | MAS2_I(1) | MAS2_M(1) | MAS2_G(1) | MAS2_E(0),
	  MAS7_3_RPN((PHYMAP_MINADDR_CATOMICOPS + (ddrOrigin<<5) + (3ul<<30)) >>12 ) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(0) | MAS3_UR(1) | MAS3_UW(1) | MAS3_UX(0) | speccap,
	  MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	  MMUCR3_X(0) | MMUCR3_R(1) |	MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
	  );

    /* -----------------------------------------
     * speculative storage for DDR
     * V=0x8000000000000000
     * R=0x0  MAS3_U0=0
     * ----------------------------------------- */
    tlbwe(
       MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
       MAS2_EPN((0x8000000000000000LL) >> 12)   | MAS2_W(0) | MAS2_I(0) | MAS2_M(1) | MAS2_G(0) | MAS2_E(0),
       MAS7_3_RPN(ddrOrigin >> 12) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(1) | MAS3_UR(0) | MAS3_UW(0) | MAS3_UX(0) | MAS3_U3(1),
       MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
       MMUCR3_X(0) | MMUCR3_R(1) |   MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
       );
    
    for(n=1; n<5; n++)
    {
       tlbwe(
          MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
          MAS2_EPN((0x1040000000LL * n) >> 12)   | MAS2_W(0) | MAS2_I(0) | MAS2_M(1) | MAS2_G(0) | MAS2_E(0),
          MAS7_3_RPN(((0x1000000000LL * n) + ddrOrigin) >> 12) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(1) | MAS3_UR(0) | MAS3_UW(0) | MAS3_UX(0) | MAS3_U3(1),
          MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
          MMUCR3_X(0) | MMUCR3_R(1) |   MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
          );
    }
    
    /* ------------------------------------------------------------
     * Install four 1GB data entries for PCIE Device Space.  Do 
     * this for both user and supervisor addresses (a total of
     * eight TLB entries).
     * ------------------------------------------------------------ */

    tlbwe(
	  MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
	  MAS2_EPN(  (VA_MINADDR_PCIE     + (0ul<<30)) >>12 ) | MAS2_W(0) | MAS2_I(1) | MAS2_M(1) | MAS2_G(1) | MAS2_E(0),
	  MAS7_3_RPN((PHYMAP_MINADDR_PCIE + (0ul<<30)) >>12 ) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(0) | MAS3_UR(0) | MAS3_UW(0) | MAS3_UX(0),
	  MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	  MMUCR3_X(0) | MMUCR3_R(1) |	MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
	  );

    tlbwe(
	  MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
	  MAS2_EPN(  (VA_MINADDR_PCIE     + (1ul<<30)) >>12 ) | MAS2_W(0) | MAS2_I(1) | MAS2_M(1) | MAS2_G(1) | MAS2_E(0),
	  MAS7_3_RPN((PHYMAP_MINADDR_PCIE + (1ul<<30)) >>12 ) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(0) | MAS3_UR(0) | MAS3_UW(0) | MAS3_UX(0),
	  MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	  MMUCR3_X(0) | MMUCR3_R(1) |	MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
	  );

    tlbwe(
	  MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
	  MAS2_EPN(  (VA_MINADDR_PCIE     + (2ul<<30)) >>12 ) | MAS2_W(0) | MAS2_I(1) | MAS2_M(1) | MAS2_G(1) | MAS2_E(0),
	  MAS7_3_RPN((PHYMAP_MINADDR_PCIE + (2ul<<30)) >>12 ) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(0) | MAS3_UR(0) | MAS3_UW(0) | MAS3_UX(0),
	  MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	  MMUCR3_X(0) | MMUCR3_R(1) |	MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
	  );

    tlbwe(
	  MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
	  MAS2_EPN(  (VA_MINADDR_PCIE     + (3ul<<30)) >>12 ) | MAS2_W(0) | MAS2_I(1) | MAS2_M(1) | MAS2_G(1) | MAS2_E(0),
	  MAS7_3_RPN((PHYMAP_MINADDR_PCIE + (3ul<<30)) >>12 ) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(0) | MAS3_UR(0) | MAS3_UW(0) | MAS3_UX(0),
	  MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	  MMUCR3_X(0) | MMUCR3_R(1) |	MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
	  );

    tlbwe(
	  MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
	  MAS2_EPN(  (VA_MINADDR_PCIE     + PHYMAP_PRIVILEGEDOFFSET + (0ul<<30)) >>12 ) | MAS2_W(0) | MAS2_I(1) | MAS2_M(1) | MAS2_G(1) | MAS2_E(0),
	  MAS7_3_RPN((PHYMAP_MINADDR_PCIE + PHYMAP_PRIVILEGEDOFFSET + (0ul<<30)) >>12 ) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(0) | MAS3_UR(0) | MAS3_UW(0) | MAS3_UX(0),
	  MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	  MMUCR3_X(0) | MMUCR3_R(1) |	MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
	  );

    tlbwe(
	  MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
	  MAS2_EPN(  (VA_MINADDR_PCIE     + PHYMAP_PRIVILEGEDOFFSET + (1ul<<30)) >>12 ) | MAS2_W(0) | MAS2_I(1) | MAS2_M(1) | MAS2_G(1) | MAS2_E(0),
	  MAS7_3_RPN((PHYMAP_MINADDR_PCIE + PHYMAP_PRIVILEGEDOFFSET + (1ul<<30)) >>12 ) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(0) | MAS3_UR(0) | MAS3_UW(0) | MAS3_UX(0),
	  MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	  MMUCR3_X(0) | MMUCR3_R(1) |	MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
	  );

    tlbwe(
	  MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
	  MAS2_EPN(  (VA_MINADDR_PCIE     + PHYMAP_PRIVILEGEDOFFSET + (2ul<<30)) >>12 ) | MAS2_W(0) | MAS2_I(1) | MAS2_M(1) | MAS2_G(1) | MAS2_E(0),
	  MAS7_3_RPN((PHYMAP_MINADDR_PCIE + PHYMAP_PRIVILEGEDOFFSET + (2ul<<30)) >>12 ) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(0) | MAS3_UR(0) | MAS3_UW(0) | MAS3_UX(0),
	  MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	  MMUCR3_X(0) | MMUCR3_R(1) |	MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
	  );

    tlbwe(
	  MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
	  MAS2_EPN(  (VA_MINADDR_PCIE     + PHYMAP_PRIVILEGEDOFFSET + (3ul<<30)) >>12 ) | MAS2_W(0) | MAS2_I(1) | MAS2_M(1) | MAS2_G(1) | MAS2_E(0),
	  MAS7_3_RPN((PHYMAP_MINADDR_PCIE + PHYMAP_PRIVILEGEDOFFSET + (3ul<<30)) >>12 ) | MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(0) | MAS3_UR(0) | MAS3_UW(0) | MAS3_UX(0),
	  MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	  MMUCR3_X(0) | MMUCR3_R(1) |	MMUCR3_C(1) | MMUCR3_ECL(0) | MMUCR3_CLASS(1) |MMUCR3_ThdID(0xF)
	  );


    isync();
  }
  else {

    /* --------------------------------------------------------------
     * Install user-space MMIO mapping
     * -------------------------------------------------------------- */

    write_derat( 1, 
		 ERAT0_EPN(VA_MINADDR_MMIO) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(PHYMAP_MINADDR_MMIO) | ERAT1_R | ERAT1_C | ERAT1_I | ERAT1_M | ERAT1_G | ERAT1_U1 | ERAT1_UX | ERAT1_UW | ERAT1_UR | ERAT1_SX | ERAT1_SW | ERAT1_SR
		 );

    write_ierat( 1, 
		 ERAT0_EPN(VA_MINADDR_MMIO) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(PHYMAP_MINADDR_MMIO) | ERAT1_R | ERAT1_C | ERAT1_I | ERAT1_M | ERAT1_G | ERAT1_U1 | ERAT1_UX | ERAT1_UW | ERAT1_UR | ERAT1_SX | ERAT1_SW | ERAT1_SR
		 );


    /* ------------------------------------------------------------
     * Install four 1GB data ERAT entries for Atomic Ops starting
     * 0x100_0000_0000:
     * ------------------------------------------------------------ */

    // @todo 1) Why are these user-space accessible?  2) Why are they executable?

    write_derat( 3,
		 ERAT0_EPN(VA_MINADDR_CATOMICOPS) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(PHYMAP_MINADDR_CATOMICOPS + (ddrOrigin<<5)) | ERAT1_R | ERAT1_C | ERAT1_M | ERAT1_UX | ERAT1_UW | ERAT1_UR | ERAT1_SX | ERAT1_SW | ERAT1_SR | ERAT1_I | ERAT1_G
		 );

    write_derat( 16,
		 ERAT0_EPN(VA_MINADDR_CATOMICOPS     + (1ul<<30)) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(PHYMAP_MINADDR_CATOMICOPS + (ddrOrigin<<5) + (1ul<<30)) | ERAT1_R | ERAT1_C | ERAT1_M | ERAT1_UX | ERAT1_UW | ERAT1_UR | ERAT1_SX | ERAT1_SW | ERAT1_SR | ERAT1_I | ERAT1_G
		 );

    write_derat( 17,
		 ERAT0_EPN(VA_MINADDR_CATOMICOPS     + (2ul<<30)) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(PHYMAP_MINADDR_CATOMICOPS + (ddrOrigin<<5) + (2ul<<30)) | ERAT1_R | ERAT1_C | ERAT1_M | ERAT1_UX | ERAT1_UW | ERAT1_UR | ERAT1_SX | ERAT1_SW | ERAT1_SR | ERAT1_I | ERAT1_G
		 );
  
    write_derat( 18,
		 ERAT0_EPN(VA_MINADDR_CATOMICOPS     + (3ul<<30)) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(PHYMAP_MINADDR_CATOMICOPS + (ddrOrigin<<5) + (3ul<<30)) | ERAT1_R | ERAT1_C | ERAT1_M | ERAT1_UX | ERAT1_UW | ERAT1_UR | ERAT1_SX | ERAT1_SW | ERAT1_SR | ERAT1_I | ERAT1_G
		 );


    write_derat( 19,
                 ERAT0_EPN(0x8000000000000000LL) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
                 ERAT1_RPN(0x0) | ERAT1_R | ERAT1_C | ERAT1_M | ERAT1_UX | ERAT1_UW | ERAT1_UR | ERAT1_SX | ERAT1_SW | ERAT1_SR | ERAT1_U3
       );
    
    /* ------------------------------------------------------------
     * Install four 1GB data ERAT entries for PCIE Device Space
     * ------------------------------------------------------------ */

    // @todo 1) Why are these user-space accessible?  2) Why are they executable?

    write_derat( 20,
		 ERAT0_EPN(VA_MINADDR_PCIE     + (0ul<<30) ) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(PHYMAP_MINADDR_PCIE + (0ul<<30) ) | ERAT1_R | ERAT1_C | ERAT1_I | ERAT1_M | ERAT1_G | ERAT1_SW | ERAT1_SR | ERAT1_UW | ERAT1_UR
		 );

    write_derat( 21,
		 ERAT0_EPN(VA_MINADDR_PCIE     + (1ul<<30) ) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(PHYMAP_MINADDR_PCIE + (1ul<<30) ) | ERAT1_R | ERAT1_C | ERAT1_I | ERAT1_M | ERAT1_G | ERAT1_SW | ERAT1_SR | ERAT1_UW | ERAT1_UR
		 );

    write_derat( 22,
		 ERAT0_EPN(VA_MINADDR_PCIE     + (2ul<<30) ) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(PHYMAP_MINADDR_PCIE + (2ul<<30) ) | ERAT1_R | ERAT1_C | ERAT1_I | ERAT1_M | ERAT1_G | ERAT1_SW | ERAT1_SR | ERAT1_UW | ERAT1_UR
		 );

    write_derat( 23,
		 ERAT0_EPN(VA_MINADDR_PCIE     + (3ul<<30) ) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(PHYMAP_MINADDR_PCIE + (3ul<<30) ) | ERAT1_R | ERAT1_C | ERAT1_I | ERAT1_M | ERAT1_G | ERAT1_SW | ERAT1_SR | ERAT1_UW | ERAT1_UR
		 );

    write_derat( 24,
		 ERAT0_EPN(VA_MINADDR_PCIE     + PHYMAP_PRIVILEGEDOFFSET + (0ul<<30) ) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(PHYMAP_MINADDR_PCIE + PHYMAP_PRIVILEGEDOFFSET + (0ul<<30) ) | ERAT1_R | ERAT1_C | ERAT1_I | ERAT1_M | ERAT1_G | ERAT1_SW | ERAT1_SR 
		 );

    write_derat( 25,
		 ERAT0_EPN(VA_MINADDR_PCIE     + PHYMAP_PRIVILEGEDOFFSET + (1ul<<30) ) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(PHYMAP_MINADDR_PCIE + PHYMAP_PRIVILEGEDOFFSET + (1ul<<30) ) | ERAT1_R | ERAT1_C | ERAT1_I | ERAT1_M | ERAT1_G | ERAT1_SW | ERAT1_SR 
		 );

    write_derat( 26,
		 ERAT0_EPN(VA_MINADDR_PCIE     + PHYMAP_PRIVILEGEDOFFSET + (2ul<<30) ) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(PHYMAP_MINADDR_PCIE + PHYMAP_PRIVILEGEDOFFSET + (2ul<<30) ) | ERAT1_R | ERAT1_C | ERAT1_I | ERAT1_M | ERAT1_G | ERAT1_SW | ERAT1_SR 
		 );

    write_derat( 27,
		 ERAT0_EPN(VA_MINADDR_PCIE     + PHYMAP_PRIVILEGEDOFFSET + (3ul<<30) ) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(PHYMAP_MINADDR_PCIE + PHYMAP_PRIVILEGEDOFFSET + (3ul<<30) ) | ERAT1_R | ERAT1_C | ERAT1_I | ERAT1_M | ERAT1_G | ERAT1_SW | ERAT1_SR 
		 );


  }


}

void fwext_llb_check_init( void ) {

    char* interval = fwext_getenv("BG_LLBCHECK");

    if ( ( interval != 0 ) && ( ! TI_isDD1() ) ) {

	// Set the LLB interval:

	unsigned long llb_interval = fwext_strtoul(interval, 0, 0);
	uint64_t xucr1 = mfspr( SPRN_XUCR1 );
	xucr1 &= ~XUCR1_LL_TB_SEL(-1); // clear out LLB config
	xucr1 |=  XUCR1_LL_TB_SEL(llb_interval);  
	mtspr(SPRN_XUCR1,xucr1);

	// Clear the error counters:
	unsigned targetSlave = BGQCFAM_CoreToSlave(ProcessorCoreID());
	static uint64_t llbits = _BN(56) | _BN(57) | _BN(58) | _BN(59);
	BgDcrPcbWrite (BGQ_TI_SCOM_CFAM_A2SCOM_FIR0_AND(targetSlave), (~llbits));

	uint64_t pccr0 = BgDcrPcbRead (BGQ_TI_SCOM_CFAM_A2SCOM_PCCR0(targetSlave));
	pccr0 &= ~(_B4(51, 0xF));
	BgDcrPcbWrite (BGQ_TI_SCOM_CFAM_A2SCOM_PCCR0(targetSlave), pccr0);

	//printf("(*) %s fir0=%016lX fir1=%016lX \n", __func__, (uint64_t)BgDcrPcbRead (BGQ_TI_SCOM_CFAM_A2SCOM_FIR0(targetSlave)), (uint64_t)BgDcrPcbRead (BGQ_TI_SCOM_CFAM_A2SCOM_FIR1(targetSlave)) );

    }
}

uint64_t BeDramStackPointer[NUM_HW_THREADS];

//
// Enter here from Kernel_Start in start.S which was launched by the Firmware.
//
//  Note: (1) We're still running on the Firmware's 1KB stack, be careful.
//        (2) Firmware owns interrupt vectors (IVPR) until we're ready to handle interrupts.
//
__C_LINKAGE void __NORETURN Kernel_Entry( Firmware_Interface_t *fw_interface )
{
    int ThdNdx = ProcessorID(); // 0..67
//    int CoreID = ProcessorCoreID(); // 0..16
//    int ThdID  = ProcessorThreadID();    // 0..3
    static char *Stacks = NULL;
    static uint64_t StackSize = FWEXT_STACK_SIZE;

    BeDramStackPointer[ThdNdx] = CurrentStackAddress();


    if ( 0 == ThdNdx )
      {
	FWExtState.firmware = fw_interface;
	//fw_interface->getPersonality( &FWExtState.Personality, sizeof(Personality_t) );
	FWExtState.personality = fw_interface->deprecated.personalityPtr;

	(void) FWExtState.firmware->getDomainDescriptor(&FWExtState.domain);
	FWExtState.vaddrLimit = (FWExtState.domain.ddrEnd + 1) -
				    FWExtState.domain.ddrOrigin;
	if ( ((FWExtState.personality->Kernel_Config.NodeConfig &
						PERS_ENABLE_MMU) == 0) &&
	     (FWExtState.vaddrLimit > 0x40000000ul) )
	{
	  // In -MMU mode, we'll only have 1GB mapped.
	  FWExtState.vaddrLimit = 0x40000000ul;
	}

	if (PERS_ENABLED(PERS_ENABLE_L2Only)) {
	  // Reduce stack size to 64K if running out of L2
	  StackSize = 64 * 1024;
	}
	// Offset stacks to lessen cache contention at L1 and L2.
	StackSize += (0x8000 +     // 32MB L2 cache / 16 ways / ~64 threads
		       0x200);     // 16KB L1 cache / 8 ways / 4 threads
	Stacks = malloc((NUM_HW_THREADS * StackSize) + (L2_CACHE_LINE_SIZE-1));
	if (Stacks == NULL)
	{
	  (void) FWExtState.firmware->putn( "STACK ALLOCATION FAILED\n", 24 );
	  Kernel_Crash(1);
	}
	Stacks = (char *) ((((uint64_t) Stacks) + (L2_CACHE_LINE_SIZE-1)) &
						      ~(L2_CACHE_LINE_SIZE-1));

	ppc_msync();
	
	BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_KERNEL_INIT);
      }
    else // block until Firmware Interface is available
       {
       // non-init threads wait until setup complete
	 //	 Kernel_ThreadPriority_Low();
	 while ( ! BeDRAM_Read(BeDRAM_LOCKNUM_KERNEL_INIT) )
	     /* spin */
	     ;
	 //	 Kernel_ThreadPriority_Medium();
       }

    // Now we can print etc.

    // Perform any initializations on thread 0 of each core.  We barrier to ensure that this is completed
    // before performing additional processing on the other threads.

    if ( ProcessorThreadID() == 0 ) {
      FWExt_InitMMU();
      fwext_llb_check_init();
    }


    // ------------------------------------------------------------------------------------
    // @deprecated  This ugly piece of code is useful in transitioning "TakeCPU" mode to
    //   be the default mode.  There is scriptint support (runfctest.sh) that will force
    //   -TakeCPU, but user's still need to pick up that update in their sandbox.  When
    //   this flushes through, this code can be removed.
    //
    // NOTE: we cannot call puts here because we still aren't on the DDR stack, but the
    //    puts/printf implementation in FW extensions uses a big big stack frame .... we
    //    go kablooey and overflow the BeDRAM stack.
    // ------------------------------------------------------------------------------------

    if (PERS_ENABLED(PERS_ENABLE_TakeCPU)) {
	/*                           1         2         3         4         5         6         7 */
	/*                  1234567890123456789012345678901234567890123456789012345678901234567890 */
	fw_interface->putn("################################################################\n", 65);
	fw_interface->putn("(E) takeCPU mode detected. Update bgq/scripts and reconfigure.\n", 63 );
	fw_interface->putn("(E) See issues 1340 and 2085 for details.                     \n", 63 );
	fw_interface->putn("################################################################\n", 65);
	fw_interface->terminate(__LINE__);
	/*printf("%s:%d HANG\n", __func__, __LINE__);*/   //asm volatile ("b 0");
	//puts("(W) takeCPU mode detected.\n\n");
    }

    fwext_coreBarrier();

    // Become a KThread
    do {
       OPD_Entry_t *opd_main = (OPD_Entry_t *)test_main;
       OPD_Entry_t *opd_exit = (OPD_Entry_t *)Kernel_Exit;

       uint64_t pStk = (uint64_t) (Stacks + ((StackSize * (ThdNdx+1)) - 64));

//       printf("(I) Kernel_Entry[%2d:%d]: Stack:0x%08lx. Entry at 0x%08lx.\n",
//	      CoreID, ThdID, pStk, (uint64_t) GetTimeBase() );
       
       extern FwExt_Regs_t FWExt_InterruptState[NUM_HW_THREADS][3];

       mtspr( SPRN_SPRG0, &FWExt_InterruptState[ThdNdx][0]);
       fwext_setInterruptHandler(fwext_DefaultInterruptHandler);

#if 1
       // Have firmware handle machine checks
       extern int _fwext_Vec_MCHK_trampoline ; 
       fwext_getFwInterface()->installInterruptVector( &_fwext_Vec_MCHK_trampoline, 0x840 );
#else
       mtspr( SPRN_IVPR,  &_fwext_Vec_MCHK);
#endif
      

        mtspr( SPRN_SRR0_IP,   opd_main->Entry_Addr );
        mtspr( SPRN_SRR1_MSR,  MSR_KERNEL );
        isync();
        asm volatile ("mtlr  %2;"
                     "li    0,0;"
                     "mr    1,%0;"
                     "mr    2,%1;"
                     "li    3,0;"
                     "rfi;"
                     "nop;"
                     :
                     : "r" (pStk),
                     "r" (opd_main->TOC_Addr),
                     "r" (opd_exit->Entry_Addr)
                     : "r0", "r1", "r2", "r3", "lr", "cc", "memory" );
       }
       while (1);
}


__C_LINKAGE void __NORETURN Kernel_Exit( int status )
{
   do {
      Kernel_Crash(status);
      }
      while(1);
}

