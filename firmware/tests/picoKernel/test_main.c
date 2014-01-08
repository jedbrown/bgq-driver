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


#include <hwi/include/bqc/A2_inlines.h>
#include <firmware/include/Firmware_Interface.h>


#define STACK_SIZE (1024 * 4)

unsigned char Stack[STACK_SIZE];

void test_main( Firmware_Interface_t *fw_interface ) {
  fw_interface->putn("Hello\n",  7);
  fw_interface->terminate(0);
  asm volatile ("b 0"); // shouldn't get here
}


void __NORETURN _test_entry( Firmware_Interface_t *fw_interface ) {

  if (ProcessorID() != 0) {
    asm volatile ("b 0");
  }

  do {
      
    OPD_Entry_t *opd_main = (OPD_Entry_t *)test_main;

    unsigned char* pStk = &(Stack[STACK_SIZE-64]);

    mtspr( SPRN_SRR0_IP,   opd_main->Entry_Addr );
    mtspr( SPRN_SRR1_MSR,  MSR_KERNEL );
    isync();
      
    asm volatile (
		  "li    0,0;"
		  "mr    1,%0;"
		  "rfi;"
		  "nop;"
		  :
		  : "r" (pStk)
		  : "r1", "memory" );
    } while (1);
}


