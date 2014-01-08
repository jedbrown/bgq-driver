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

#include <firmware/include/Firmware.h>
#include "Firmware_internals.h"



int main( int argc, char *argv[], char **envp )
{

  // printf("(I) main[%2d:%d]: Launching Kernel.\n", ProcessorCoreID(), ProcessorThreadID() );

   do {

     uint64_t kernel_MSR       = MSR_KERNEL_DISABLED;
     Firmware_Interface_t* fwi = &FW_Interface;

     mtspr( SPRN_SRR0_IP,  FW_InternalState.nodeState.domain[fw_getThisDomainID()].entryPoint );
     mtspr( SPRN_SRR1_MSR, kernel_MSR );

     asm volatile ("mr  3,%0;"
                    "rfi;"
                    "nop;"
                    :
                    : "r" (fwi)
                    : "memory" );
      }
      while(1);
}


