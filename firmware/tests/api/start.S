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
/*------------------------------------------------------------------*/

#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

//-#include <hwi/include/common/compiler_support.h>
#include <hwi/include/common/asm_support.h>
//-#include <hwi/include/common/bgq_alignment.h>

#include <hwi/include/bqc/A2_core.h>
//-#include <firmware/include/Firmware.h>
//-#include "fwext_regs.h"
#include "linkage.h"

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
// Make the jump into C code at entry point "_test_entry".  We don't futz
// with stack here (that will be done on the other side).
//------------------------------------------------------------------------------
        .extern _test_entry
        .global _start
        _PROLOG_ABS0(Test_Start)
Test_Start:
_start:
        li     %r0,0
        IMM64( %r2, __FWTST_TOC_BASE )
        bl     _test_entry
        nop
	b 0
        _EPILOG(Test_Start)
//------------------------------------------------------------------------------

