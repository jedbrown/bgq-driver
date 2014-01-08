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
#include <firmware/include/Firmware.h>
#include <firmware/include/fwext/fwext_regs.h>
#include <firmware/include/fwext/fwext_Linkage.h>

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
        .extern interrupt_bic
        .global _start
        _PROLOG_ABS0(Kernel_Start)
Kernel_Start:
_start:
        li     %r0,0
        IMM64( %r2, __FWEXT_TOC_BASE )
        bl     Kernel_Entry
        nop
        bl     Kernel_Exit
        nop
        b      Kernel_Start
        nop
        _EPILOG(Kernel_Start)
//------------------------------------------------------------------------------


#define FW_MVEC( label, code ) \
        .align 5; \
label:   \
        mtspr SPRN_SPRG1, %r3; \
        mfspr %r3, SPRN_SPRG0; \
        std %r4, REGS_OFS_GPR4(%r3); \
        li %r4, code; \
        b machinecheck_interrupt_entry;

#define FW_CVEC( label, code ) \
        .align 5; \
label:   \
        mtspr SPRN_SPRG2, %r3; \
        mfspr %r3, SPRN_SPRG0; \
        addi %r3, %r3, REGS_SIZEOF; \
        std %r4, REGS_OFS_GPR4(%r3); \
        li %r4, code; \
        b critical_interrupt_entry;


#define FW_SVEC( label, code ) \
        .align 5; \
label:   \
        mtspr SPRN_SPRG3, %r3; \
        mfspr %r3, SPRN_SPRG0; \
        addi %r3, %r3, 2*REGS_SIZEOF; \
        std %r4, REGS_OFS_GPR4(%r3); \
        li %r4, code; \
        b standard_interrupt_entry;

//------------------------------------------------------------------------------
        _PROLOG_KVECTORS( _fwext_Vec_MCHK )
        FW_MVEC(_fwext_Vec_MCHK,     IVO_MCHK     )
	FW_CVEC(_fwext_Vec_CI,       IVO_CI       )
        FW_CVEC(_fwext_Vec_DEBUG,    IVO_DEBUG    )
	FW_SVEC(_fwext_Vec_DSI,      IVO_DSI      )
        FW_SVEC(_fwext_Vec_ISI,      IVO_ISI      )
        FW_SVEC(_fwext_Vec_EI,       IVO_EI       )
        FW_SVEC(_fwext_Vec_ALGN,     IVO_ALGN     )
        FW_SVEC(_fwext_Vec_PROG,     IVO_PROG     )
        FW_SVEC(_fwext_Vec_FPU,      IVO_FPU      )
        FW_SVEC(_fwext_Vec_SC,       IVO_SC       )
        FW_SVEC(_fwext_Vec_APU,      IVO_APU      )
        FW_SVEC(_fwext_Vec_DEC,      IVO_DEC      )
        FW_SVEC(_fwext_Vec_FIT,      IVO_FIT      )
        FW_CVEC(_fwext_Vec_WDT,      IVO_WDT      )
        FW_SVEC(_fwext_Vec_DTLB,     IVO_DTLB     )
        FW_SVEC(_fwext_Vec_ITLB,     IVO_ITLB     )
        FW_SVEC(_fwext_Vec_VECTOR,   IVO_VECTOR   )
        FW_SVEC(_fwext_Vec_Undef220, IVO_UNDEFINED )
        FW_SVEC(_fwext_Vec_Undef240, IVO_UNDEFINED )
        FW_SVEC(_fwext_Vec_Undef260, IVO_UNDEFINED )
        FW_SVEC(_fwext_Vec_PDBI,     IVO_PDBI     )
        FW_CVEC(_fwext_Vec_PDBCI,    IVO_PDBCI    )
        FW_SVEC(_fwext_Vec_GDBI,     IVO_GDBI     )
        FW_CVEC(_fwext_Vec_GDBCI,    IVO_GDBCI    )
        FW_CVEC(_fwext_Vec_EHVSC,    IVO_EHVSC    )
        FW_CVEC(_fwext_Vec_EHVPRIV,  IVO_EHVPRIV  )
        FW_CVEC(_fwext_Vec_LRATE,    IVO_LRATE    )
	FW_SVEC(_fwext_Vec_Undefined_360, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_380, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_3a0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_3c0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_3e0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_400, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_420, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_440, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_460, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_480, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_4a0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_4c0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_4e0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_500, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_520, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_540, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_560, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_580, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_5a0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_5c0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_5e0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_600, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_620, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_640, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_660, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_680, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_6a0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_6c0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_6e0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_700, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_720, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_740, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_760, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_780, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_7a0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_7c0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_7e0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_UDec,          IVO_UDEC )
	FW_SVEC(_fwext_Vec_PerfMon,       IVO_PERFMON )
	FW_SVEC(_fwext_Vec_Undefined_840, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_860, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_880, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_8a0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_8c0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_8e0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_900, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_920, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_940, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_960, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_980, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_9a0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_9c0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_9e0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_a00, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_a20, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_a40, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_a60, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_a80, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_aa0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_ac0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_ae0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_b00, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_b20, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_b40, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_b60, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_b80, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_ba0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_bc0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_be0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_c00, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_c20, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_c40, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_c60, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_c80, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_ca0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_cc0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_ce0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_d00, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_d20, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_d40, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_d60, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_d80, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_da0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_dc0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_de0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_e00, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_e20, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_e40, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_e60, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_e80, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_ea0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_ec0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_ee0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_f00, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_f20, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_f40, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_f60, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_f80, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_fa0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_fc0, IVO_UNDEFINED )
	FW_SVEC(_fwext_Vec_Undefined_fe0, IVO_UNDEFINED )
	
        //
        // 3KB of space (3,232 bytes) available between end of vectors and reset page.
        //  (assuming full reset page, so prolly more like 4.5K total)
        //
        _EPILOG( _fwext_Vec_DEBUG )

#define FW_TRAMPOLINE( label, code ) \
        .align 5; \
label ## _trampoline:   \
	ba label		;

	
	.align 12
	#.section ".ktext.vectors", "ax" ;
	.global _fwext_Vec_MCHK_trampoline ; 

	FW_TRAMPOLINE(_fwext_Vec_MCHK,     IVO_MCHK     )
	FW_TRAMPOLINE(_fwext_Vec_CI,       IVO_CI       )
        FW_TRAMPOLINE(_fwext_Vec_DEBUG,    IVO_DEBUG    )
	FW_TRAMPOLINE(_fwext_Vec_DSI,      IVO_DSI      )
        FW_TRAMPOLINE(_fwext_Vec_ISI,      IVO_ISI      )
        FW_TRAMPOLINE(_fwext_Vec_EI,       IVO_EI       )
        FW_TRAMPOLINE(_fwext_Vec_ALGN,     IVO_ALGN     )
        FW_TRAMPOLINE(_fwext_Vec_PROG,     IVO_PROG     )
        FW_TRAMPOLINE(_fwext_Vec_FPU,      IVO_FPU      )
        FW_TRAMPOLINE(_fwext_Vec_SC,       IVO_SC       )
        FW_TRAMPOLINE(_fwext_Vec_APU,      IVO_APU      )
        FW_TRAMPOLINE(_fwext_Vec_DEC,      IVO_DEC      )
        FW_TRAMPOLINE(_fwext_Vec_FIT,      IVO_FIT      )
        FW_TRAMPOLINE(_fwext_Vec_WDT,      IVO_WDT      )
        FW_TRAMPOLINE(_fwext_Vec_DTLB,     IVO_DTLB     )
        FW_TRAMPOLINE(_fwext_Vec_ITLB,     IVO_ITLB     )
        FW_TRAMPOLINE(_fwext_Vec_VECTOR,   IVO_VECTOR   )
        FW_TRAMPOLINE(_fwext_Vec_Undef220, IVO_UNDEFINED )
        FW_TRAMPOLINE(_fwext_Vec_Undef240, IVO_UNDEFINED )
        FW_TRAMPOLINE(_fwext_Vec_Undef260, IVO_UNDEFINED )
        FW_TRAMPOLINE(_fwext_Vec_PDBI,     IVO_PDBI     )
        FW_TRAMPOLINE(_fwext_Vec_PDBCI,    IVO_PDBCI    )
        FW_TRAMPOLINE(_fwext_Vec_GDBI,     IVO_GDBI     )
        FW_TRAMPOLINE(_fwext_Vec_GDBCI,    IVO_GDBCI    )
        FW_TRAMPOLINE(_fwext_Vec_EHVSC,    IVO_EHVSC    )
        FW_TRAMPOLINE(_fwext_Vec_EHVPRIV,  IVO_EHVPRIV  )
        FW_TRAMPOLINE(_fwext_Vec_LRATE,    IVO_LRATE    )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_360, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_380, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_3a0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_3c0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_3e0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_400, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_420, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_440, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_460, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_480, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_4a0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_4c0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_4e0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_500, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_520, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_540, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_560, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_580, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_5a0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_5c0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_5e0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_600, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_620, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_640, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_660, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_680, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_6a0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_6c0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_6e0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_700, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_720, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_740, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_760, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_780, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_7a0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_7c0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_7e0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_UDec,          IVO_UDEC )
	FW_TRAMPOLINE(_fwext_Vec_PerfMon,       IVO_PERFMON )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_840, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_860, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_880, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_8a0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_8c0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_8e0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_900, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_920, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_940, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_960, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_980, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_9a0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_9c0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_9e0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_a00, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_a20, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_a40, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_a60, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_a80, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_aa0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_ac0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_ae0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_b00, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_b20, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_b40, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_b60, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_b80, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_ba0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_bc0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_be0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_c00, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_c20, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_c40, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_c60, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_c80, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_ca0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_cc0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_ce0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_d00, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_d20, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_d40, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_d60, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_d80, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_da0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_dc0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_de0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_e00, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_e20, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_e40, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_e60, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_e80, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_ea0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_ec0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_ee0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_f00, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_f20, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_f40, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_f60, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_f80, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_fa0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_fc0, IVO_UNDEFINED )
	FW_TRAMPOLINE(_fwext_Vec_Undefined_fe0, IVO_UNDEFINED )

        .align 5
_fwext_Vectors_End:
        .global _fwext_Vectors_End
	

	
_PROLOG_START(machinecheck_interrupt_entry)
machinecheck_interrupt_entry:
        std %r5, REGS_OFS_GPR5(%r3)
        std %r6, REGS_OFS_GPR6(%r3)
        mfspr  %r5,SPRN_MCSRR0_IP
        mfspr  %r6,SPRN_MCSRR1_MSR
        std %r5, REGS_OFS_IP(%r3)
        std %r6, REGS_OFS_MSR(%r3)
        mflr %r5
        std %r5, REGS_OFS_LR(%r3)
        std %r0, REGS_OFS_GPR0(%r3)
	std %r1, REGS_OFS_GPR1(%r3)
        std %r2, REGS_OFS_GPR2(%r3)
        std %r7, REGS_OFS_GPR7(%r3)
        std %r8, REGS_OFS_GPR8(%r3)
        std %r9, REGS_OFS_GPR9(%r3)
        std %r10, REGS_OFS_GPR10(%r3)
        std %r11, REGS_OFS_GPR11(%r3)
        std %r12, REGS_OFS_GPR12(%r3)
        mfxer %r5
        mfctr %r6
        mfcr %r7
        std %r5, REGS_OFS_XER(%r3)
        std %r6, REGS_OFS_CTR(%r3)
        std %r7, REGS_OFS_CR(%r3)
        
        IMM64( %r2, __FWEXT_TOC_BASE )
        mfspr %r5, SPRN_SPRG4
        mtctr %r5
	la %r1,-INT_FRAME_SIZE(%r1)
        bctrl

	# return from handler:	
		
        mfspr %r3, SPRN_SPRG0
        ld %r5, REGS_OFS_XER(%r3)
        ld %r6, REGS_OFS_CTR(%r3)
        ld %r7, REGS_OFS_CR(%r3)
        mtxer %r5
        mtctr %r6
        mtcr %r7

	ld %r5,REGS_OFS_LR(%r3)
	mtlr %r5
        
        ld %r0, REGS_OFS_GPR0(%r3)
	ld %r1, REGS_OFS_GPR1(%r3)
        ld %r2, REGS_OFS_GPR2(%r3)
        ld %r6, REGS_OFS_GPR6(%r3)
        ld %r7, REGS_OFS_GPR7(%r3)
        ld %r8, REGS_OFS_GPR8(%r3)
        ld %r9, REGS_OFS_GPR9(%r3)
        ld %r10, REGS_OFS_GPR10(%r3)
        ld %r11, REGS_OFS_GPR11(%r3)
        ld %r12, REGS_OFS_GPR12(%r3)
        
        ld %r4, REGS_OFS_IP(%r3)
        ld %r5, REGS_OFS_MSR(%r3)
        mtspr  SPRN_MCSRR0_IP, %r4
        mtspr  SPRN_MCSRR1_MSR, %r5
        ld %r4, REGS_OFS_GPR4(%r3)
        ld %r5, REGS_OFS_GPR5(%r3)
        mfspr %r3, SPRN_SPRG1
        rfmci
        _EPILOG( machinecheck_interrupt_entry )

        
_PROLOG_START(critical_interrupt_entry)
critical_interrupt_entry:
        std %r5, REGS_OFS_GPR5(%r3)
        std %r6, REGS_OFS_GPR6(%r3)
        mfspr  %r5,SPRN_CSRR0_IP
        mfspr  %r6,SPRN_CSRR1_MSR
        std %r5, REGS_OFS_IP(%r3)
        std %r6, REGS_OFS_MSR(%r3)
        mflr %r5
        std %r5, REGS_OFS_LR(%r3)
        std %r0, REGS_OFS_GPR0(%r3)
	std %r1, REGS_OFS_GPR1(%r3)
        std %r2, REGS_OFS_GPR2(%r3)
        std %r7, REGS_OFS_GPR7(%r3)
        std %r8, REGS_OFS_GPR8(%r3)
        std %r9, REGS_OFS_GPR9(%r3)
        std %r10, REGS_OFS_GPR10(%r3)
        std %r11, REGS_OFS_GPR11(%r3)
        std %r12, REGS_OFS_GPR12(%r3)
        mfxer %r5
        mfctr %r6
        mfcr %r7
        std %r5, REGS_OFS_XER(%r3)
        std %r6, REGS_OFS_CTR(%r3)
        std %r7, REGS_OFS_CR(%r3)
        
        IMM64( %r2, __FWEXT_TOC_BASE )
        mfspr %r5, SPRN_SPRG4
        mtctr %r5
	la %r1,-INT_FRAME_SIZE(%r1)
	bctrl

	# return from handler:	
		
        mfspr %r3, SPRN_SPRG0
        addi %r3, %r3, REGS_SIZEOF
	
        ld %r5, REGS_OFS_XER(%r3)
        ld %r6, REGS_OFS_CTR(%r3)
        ld %r7, REGS_OFS_CR(%r3)
        mtxer %r5
        mtctr %r6
        mtcr %r7

	ld %r5,REGS_OFS_LR(%r3)
	mtlr %r5
        
        ld %r0, REGS_OFS_GPR0(%r3)
        ld %r1, REGS_OFS_GPR1(%r3)
        ld %r2, REGS_OFS_GPR2(%r3)
        ld %r6, REGS_OFS_GPR6(%r3)
        ld %r7, REGS_OFS_GPR7(%r3)
        ld %r8, REGS_OFS_GPR8(%r3)
        ld %r9, REGS_OFS_GPR9(%r3)
        ld %r10, REGS_OFS_GPR10(%r3)
        ld %r11, REGS_OFS_GPR11(%r3)
        ld %r12, REGS_OFS_GPR12(%r3)
        
        ld %r4, REGS_OFS_IP(%r3)
        ld %r5, REGS_OFS_MSR(%r3)
        mtspr  SPRN_CSRR0_IP, %r4
        mtspr  SPRN_CSRR1_MSR, %r5
        ld %r4, REGS_OFS_GPR4(%r3)
        ld %r5, REGS_OFS_GPR5(%r3)
        mfspr %r3, SPRN_SPRG2
        rfci
        _EPILOG( critical_interrupt_entry )

_PROLOG_START(standard_interrupt_entry)
standard_interrupt_entry:
        std %r5, REGS_OFS_GPR5(%r3)
        std %r6, REGS_OFS_GPR6(%r3)
        mfspr  %r5,SPRN_SRR0_IP
        mfspr  %r6,SPRN_SRR1_MSR
        std %r5, REGS_OFS_IP(%r3)
        std %r6, REGS_OFS_MSR(%r3)
        mflr %r5
        std %r5, REGS_OFS_LR(%r3)
        std %r0, REGS_OFS_GPR0(%r3)
        std %r1, REGS_OFS_GPR1(%r3)
        std %r2, REGS_OFS_GPR2(%r3)
        std %r7, REGS_OFS_GPR7(%r3)
        std %r8, REGS_OFS_GPR8(%r3)
        std %r9, REGS_OFS_GPR9(%r3)
        std %r10, REGS_OFS_GPR10(%r3)
        std %r11, REGS_OFS_GPR11(%r3)
        std %r12, REGS_OFS_GPR12(%r3)
        mfxer %r5
        mfctr %r6
        mfcr %r7
        std %r5, REGS_OFS_XER(%r3)
        std %r6, REGS_OFS_CTR(%r3)
        std %r7, REGS_OFS_CR(%r3)
        
        IMM64( %r2, __FWEXT_TOC_BASE )
        mfspr %r5, SPRN_SPRG4
        mtctr %r5
	la %r1,-INT_FRAME_SIZE(%r1)
        bctrl

	# return from handler:	
		
        mfspr %r3, SPRN_SPRG0
        addi %r3, %r3, 2*REGS_SIZEOF
	
        ld %r5, REGS_OFS_XER(%r3)
        ld %r6, REGS_OFS_CTR(%r3)
        ld %r7, REGS_OFS_CR(%r3)
        mtxer %r5
        mtctr %r6
        mtcr %r7

	ld %r5,REGS_OFS_LR(%r3)
	mtlr %r5
	
        ld %r0, REGS_OFS_GPR0(%r3)
        ld %r1, REGS_OFS_GPR1(%r3)
        ld %r2, REGS_OFS_GPR2(%r3)
        ld %r6, REGS_OFS_GPR6(%r3)
        ld %r7, REGS_OFS_GPR7(%r3)
        ld %r8, REGS_OFS_GPR8(%r3)
        ld %r9, REGS_OFS_GPR9(%r3)
        ld %r10, REGS_OFS_GPR10(%r3)
        ld %r11, REGS_OFS_GPR11(%r3)
        ld %r12, REGS_OFS_GPR12(%r3)
        
        ld %r4, REGS_OFS_IP(%r3)
        ld %r5, REGS_OFS_MSR(%r3)
        mtspr  SPRN_SRR0_IP, %r4
        mtspr  SPRN_SRR1_MSR, %r5
        ld %r4, REGS_OFS_GPR4(%r3)
        ld %r5, REGS_OFS_GPR5(%r3)
        mfspr %r3, SPRN_SPRG3
        rfi
        _EPILOG( standard_interrupt_entry )



