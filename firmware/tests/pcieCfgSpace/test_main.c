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

#include <firmware/include/Firmware_Interface.h>
#include <firmware/include/fwext/fwext.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/testint_inlines.h>

#if 0
#include <hwi/include/common/bgq_alignment.h>

#include <firmware/include/mailbox.h>



#include <string.h>
#include <stdio.h>
#endif

#include <stdlib.h>

#define PCIE_CONFIG_SPACE (PHYMAP_MINADDR_PCICFG + PHYMAP_PRIVILEGEDOFFSET)

void pcie_set_prefix(fw_uint32_t bus, fw_uint32_t dev, fw_uint32_t func) {

    fw_uint32_t *ptr = (fw_uint32_t*)(PCIE_CONFIG_SPACE + 0x1000);
    fw_uint32_t prefix;

    prefix = (0xfff & func) << 12;
    prefix |= dev << 15; 
    prefix |= bus << 20;
    *ptr = prefix;
}

int test_main( void ) {

    unsigned i;

    if ( ProcessorID() == 0 ) {
	pcie_set_prefix(0, 0, 0);

	printf("Bus 0, Device 0, Function 0\n");
	printf("---------------------------\n");
    
	for (i = 0x0; i < 256/4; i+= 4) {
	    volatile uint32_t* ptr = (uint32_t*)(PCIE_CONFIG_SPACE + (i*4));
	    printf("%04X : %08X %08X %08X %08X\n", i*4, ptr[0], ptr[1], ptr[2], ptr[3]);
	}
    }

    
  exit(0);
}



