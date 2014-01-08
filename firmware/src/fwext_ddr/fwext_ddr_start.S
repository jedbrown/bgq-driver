// begin_generated_IBM_copyright_prolog                             
//                                                                  
// This is an automatically generated copyright prolog.             
// After initializing,  DO NOT MODIFY OR MOVE                       
//  --------------------------------------------------------------- 
//                                                                  
// Licensed Materials - Property of IBM                             
// Blue Gene/Q                                                      
// (C) Copyright IBM Corp.  2010, 2012                              
// US Government Users Restricted Rights - Use, duplication or      
//   disclosure restricted by GSA ADP Schedule Contract with IBM    
//   Corp.                                                          
//                                                                  
// This software is available to you under the Eclipse Public       
// License (EPL).                                                   
//                                                                  
//  --------------------------------------------------------------- 
//                                                                  
// end_generated_IBM_copyright_prolog                               

#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

#include <hwi/include/common/compiler_support.h>
#include <hwi/include/common/asm_support.h>
#include <hwi/include/common/bgq_alignment.h>

#include <hwi/include/bqc/A2_core.h>

#include "fwext_Linkage.h"

#include <asm/unistd.h>  // for __NR_clone, where syscall vector must save all GPR state

         .file "start.S"

        _PROLOG_ABS0( KERNEL_CHECKSUM )
KERNEL_CHECKSUM:
        .long 0                             // Set during kernel build.
        .long 0                             // Check before kernal launch.
        _EPILOG( KERNEL_CHECKSUM )
        _PROLOG_ABS0( KERNEL_CSPAD )
KERNEL_CSPAD:
        .long 0
        .long 0x42475121                    // The string "BGQ!"
        _EPILOG( KERNEL_CSPAD )
//------------------------------------------------------------------------------
//  Kernel_Start: entry point for DDR-based Firmware Extension.
//
//  Syntax: void Kernel_Start( _Firmware_Interface_t *fwi );
//
        .extern Kernel_Entry
        .extern Kernel_Crash
        .global _start
        _PROLOG_START(Kernel_Start)
Kernel_Start:
_start:
        li     %r0,0
        IMM64( %r2, __FWEXT_TOC_BASE )
        bl     fwext_Entry
        nop
        bl     fwext_Exit
        nop
        b      Kernel_Start
        nop
        _EPILOG(Kernel_Start)
//------------------------------------------------------------------------------
