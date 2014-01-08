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
#include <firmware/include/fwext/fwext.h>
#include <firmware/include/fwext/fwext_lib.h>



void anInterruptHandler(FwExt_Regs_t* context, uint64_t code) {

  switch (code) {

  case IVO_DTLB : {
    context->ip += 4; // advance to the next instruction
    break;
  }

  default : {
    Terminate(__LINE__);
  }

  }    
}

#if 0

 void write_derat(unsigned index, uint64_t hi, uint64_t lo) {
  mtspr(SPRN_MMUCR0, MMUCR0_TLBSEL_DERAT);
  asm volatile(
	       "eratwe %1,%0,1;"
	       "eratwe %2,%0,0;"
	       : : "r" (index), "r" (lo), "r" (hi)
	       : "memory"
	       );
}

void write_ierat(unsigned index, uint64_t hi, uint64_t lo) {
   mtspr(SPRN_MMUCR0, MMUCR0_TLBSEL_IERAT);
  asm volatile(
	       "eratwe %1,%0,1;"
	       "eratwe %2,%0,0;"
	       : : "r" (index), "r" (lo), "r" (hi)
	       : "memory"
	       );
}

#endif

extern int _fwext_Vec_MCHK_trampoline ; 

int test_main( void ) {

  fwext_setInterruptHandler( anInterruptHandler );

    // ----------------------------------------------------------
    // Install the trampoline vector on thread 0 of every core:
    // ----------------------------------------------------------

    if ( ProcessorThreadID() == 0 ) {
      if ( fwext_getFwInterface()->installInterruptVector( &_fwext_Vec_MCHK_trampoline, 0x400 ) != FW_OK ) {
	Terminate(__LINE__);
      }
    }

    // ----------------------------------------------------------    
    // Wait for everyone to arrive here:
    // ----------------------------------------------------------

    BgBarrier( 0, BgGetNumThreads() );


    // ----------------------------------


    if ( ProcessorID() != 2 ) {
      exit(0);//_Bg_Sleep_Forever();
    }

#if 0
    write_derat( 7,
		 ERAT0_EPN(0x1234567800000000ull) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(0) | ERAT1_R | ERAT1_C | ERAT1_W | ERAT1_I | ERAT1_G | ERAT1_UX | ERAT1_UW | ERAT1_UR | ERAT1_SX | ERAT1_SW | ERAT1_SR
		 );
    write_derat( 8,
		 ERAT0_EPN(0x1234567800000000ull) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF),
		 ERAT1_RPN(0) | ERAT1_R | ERAT1_C | ERAT1_M | ERAT1_UX | ERAT1_UW | ERAT1_UR | ERAT1_SX | ERAT1_SW | ERAT1_SR
		 );
    isync();


    volatile char* badPointer = (volatile char*)0x1234567800000000;
    *(badPointer) = 'x';

#else

    // ----------------------------------------------------------
    // Now dereference a bad pointer ... if we've done this 
    // right, we'll end up in the interrupt handler (above) via 
    // the trampoline vector.
    // ----------------------------------------------------------

    volatile char* badPointer = (volatile char*)0x123456789abcdef;
    *(badPointer) = 'x';
#endif

    exit(0);
}



