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

#include <firmware/include/Firmware.h>
#include <hwi/include/bqc/testint_dcr.h>
#include "Firmware_internals.h"	
#include "fw_regs.h"
	
        .file "fw32_start.S"

        .extern _fw_ERAT_table
        .extern fw_main
        .extern unexpectedInterrupt
        .extern fw_stacks
        .extern _fw_resetConfig
        .extern _fw_resetConfig_end

#define FW_MVEC( label, code ) \
        .align 5; \
label:  mtspr	SPRN_GSPRG1, %r3; \
        mfspr	%r3, SPRN_GSPRG0; \
        std	%r4, REGS_OFS_GPR4(%r3); \
        mfspr	%r4, SPRN_GSPRG1;  \
        std	%r4, REGS_OFS_GPR3(%r3); \
	b machineCheckEntry


#define FW_CVEC( label, code ) \
        .align 5; \
label:  li %r3, code;\
        mfspr  %r4,SPRN_CSRR0_IP;\
        b  callCrash; \
        nop;

#define FW_SVEC( label, code ) \
        .align 5; \
label:  li %r3, code;\
        mfspr  %r4,SPRN_SRR0_IP;\
        b  callCrash;

//------------------------------------------------------------------------------
        _PROLOG_VECTORS( _fw_Vec0_MCHK )

	.global _fw_Vec0_MCHK 	;
	
        FW_MVEC(_fw_Vec0_MCHK,     IVO_MCHK     )
        FW_CVEC(_fw_Vec0_CI,       IVO_CI       )
	FW_CVEC(_fw_Vec0_DEBUG,    IVO_DEBUG    )
        FW_SVEC(_fw_Vec0_DSI,      IVO_DSI      )
        FW_SVEC(_fw_Vec0_ISI,      IVO_ISI      )
        FW_SVEC(_fw_Vec0_EI,       IVO_EI       )
        FW_SVEC(_fw_Vec0_ALGN,     IVO_ALGN     )
        FW_SVEC(_fw_Vec0_PROG,     IVO_PROG     )
        FW_SVEC(_fw_Vec0_FPU,      IVO_FPU      )
        FW_SVEC(_fw_Vec0_SC,       IVO_SC       )
        FW_SVEC(_fw_Vec0_APU,      IVO_APU      )
        FW_SVEC(_fw_Vec0_DEC,      IVO_DEC      )
        FW_SVEC(_fw_Vec0_FIT,      IVO_FIT      )
        FW_CVEC(_fw_Vec0_WDT,      IVO_WDT      )
        FW_SVEC(_fw_Vec0_DTLB,     IVO_DTLB     )
        FW_SVEC(_fw_Vec0_ITLB,     IVO_ITLB     )
        FW_SVEC(_fw_Vec0_VECTOR,   IVO_VECTOR   )
        FW_SVEC(_fw_Vec0_Undef220, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec0_Undef240, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec0_Undef260, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec0_PDBI,     IVO_PDBI     )
        FW_CVEC(_fw_Vec0_PDBCI,    IVO_PDBCI    )
        FW_SVEC(_fw_Vec0_GDBI,     IVO_GDBI     )
        FW_CVEC(_fw_Vec0_GDBCI,    IVO_GDBCI    )
        FW_SVEC(_fw_Vec0_EHVSC,    IVO_EHVSC    )
        FW_SVEC(_fw_Vec0_EHVPRIV,  IVO_EHVPRIV  )
        FW_SVEC(_fw_Vec0_LRATE,    IVO_LRATE    )
        FW_SVEC(_fw_Vec0_Undef360, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef380, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef3A0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef3C0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef3E0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef400, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec0_Undef420, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef440, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef460, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef480, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef4A0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef4C0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef4E0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef500, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec0_Undef520, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef540, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef560, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef580, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef5A0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef5C0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef5E0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef600, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec0_Undef620, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef640, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef660, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef680, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef6A0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef6C0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef6E0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef700, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec0_Undef720, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef740, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef760, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef780, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef7A0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef7C0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef7E0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UDec,     IVO_UDEC      )
        FW_SVEC(_fw_Vec0_PerfMon,  IVO_PERFMON   )
	FW_SVEC(_fw_Vec0_Undef840, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef860, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef880, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef8A0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef8C0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef8E0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef900, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec0_Undef920, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef940, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef960, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef980, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef9A0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef9C0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_Undef9E0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefA00, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec0_UndefA20, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefA40, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefA60, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefA80, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefAA0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefAC0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefAE0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefB00, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec0_UndefB20, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefB40, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefB60, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefB80, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefBA0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefBC0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefBE0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefC00, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec0_UndefC20, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefC40, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefC60, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefC80, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefCA0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefCC0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefCE0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefD00, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec0_UndefD20, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefD40, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefD60, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefD80, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefDA0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefDC0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefDE0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefE00, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec0_UndefE20, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefE40, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefE60, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefE80, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefEA0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefEC0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefEE0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefF00, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec0_UndefF20, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefF40, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefF60, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefF80, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefFA0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefFC0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec0_UndefFE0, IVO_UNDEFINED )
	

	.align 12

	#ifndef FW_SINGLE_DOMAIN
	
	.global _fw_Vec1_MCHK 	;
	
        FW_MVEC(_fw_Vec1_MCHK,     IVO_MCHK     )
        FW_CVEC(_fw_Vec1_CI,       IVO_CI       )
	FW_CVEC(_fw_Vec1_DEBUG,    IVO_DEBUG    )
        FW_SVEC(_fw_Vec1_DSI,      IVO_DSI      )
        FW_SVEC(_fw_Vec1_ISI,      IVO_ISI      )
        FW_SVEC(_fw_Vec1_EI,       IVO_EI       )
        FW_SVEC(_fw_Vec1_ALGN,     IVO_ALGN     )
        FW_SVEC(_fw_Vec1_PROG,     IVO_PROG     )
        FW_SVEC(_fw_Vec1_FPU,      IVO_FPU      )
        FW_SVEC(_fw_Vec1_SC,       IVO_SC       )
        FW_SVEC(_fw_Vec1_APU,      IVO_APU      )
        FW_SVEC(_fw_Vec1_DEC,      IVO_DEC      )
        FW_SVEC(_fw_Vec1_FIT,      IVO_FIT      )
        FW_CVEC(_fw_Vec1_WDT,      IVO_WDT      )
        FW_SVEC(_fw_Vec1_DTLB,     IVO_DTLB     )
        FW_SVEC(_fw_Vec1_ITLB,     IVO_ITLB     )
        FW_SVEC(_fw_Vec1_VECTOR,   IVO_VECTOR   )
        FW_SVEC(_fw_Vec1_Undef220, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec1_Undef240, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec1_Undef260, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec1_PDBI,     IVO_PDBI     )
        FW_CVEC(_fw_Vec1_PDBCI,    IVO_PDBCI    )
        FW_SVEC(_fw_Vec1_GDBI,     IVO_GDBI     )
        FW_CVEC(_fw_Vec1_GDBCI,    IVO_GDBCI    )
        FW_SVEC(_fw_Vec1_EHVSC,    IVO_EHVSC    )
        FW_SVEC(_fw_Vec1_EHVPRIV,  IVO_EHVPRIV  )
        FW_SVEC(_fw_Vec1_LRATE,    IVO_LRATE    )
        FW_SVEC(_fw_Vec1_Undef360, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef380, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef3A0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef3C0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef3E0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef400, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec1_Undef420, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef440, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef460, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef480, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef4A0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef4C0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef4E0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef500, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec1_Undef520, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef540, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef560, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef580, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef5A0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef5C0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef5E0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef600, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec1_Undef620, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef640, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef660, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef680, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef6A0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef6C0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef6E0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef700, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec1_Undef720, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef740, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef760, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef780, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef7A0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef7C0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef7E0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UDec,     IVO_UDEC      )
        FW_SVEC(_fw_Vec1_PerfMon,  IVO_PERFMON   )
	FW_SVEC(_fw_Vec1_Undef840, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef860, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef880, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef8A0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef8C0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef8E0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef900, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec1_Undef920, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef940, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef960, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef980, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef9A0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef9C0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_Undef9E0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefA00, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec1_UndefA20, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefA40, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefA60, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefA80, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefAA0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefAC0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefAE0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefB00, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec1_UndefB20, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefB40, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefB60, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefB80, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefBA0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefBC0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefBE0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefC00, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec1_UndefC20, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefC40, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefC60, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefC80, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefCA0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefCC0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefCE0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefD00, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec1_UndefD20, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefD40, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefD60, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefD80, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefDA0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefDC0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefDE0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefE00, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec1_UndefE20, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefE40, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefE60, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefE80, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefEA0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefEC0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefEE0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefF00, IVO_UNDEFINED )
        FW_SVEC(_fw_Vec1_UndefF20, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefF40, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefF60, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefF80, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefFA0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefFC0, IVO_UNDEFINED )
	FW_SVEC(_fw_Vec1_UndefFE0, IVO_UNDEFINED )

	#endif

	.extern fw_machineCheckHandler
	
	.global machineCheckEntry 
machineCheckEntry:	
        std	%r5,REGS_OFS_GPR5(%r3)
        std	%r6,REGS_OFS_GPR6(%r3)
        mfspr	%r5,SPRN_MCSRR0_IP
        mfspr	%r6,SPRN_MCSRR1_MSR
        std	%r5,REGS_OFS_IP(%r3)
        std	%r6,REGS_OFS_MSR(%r3)
        mflr	%r5
        std	%r5,REGS_OFS_LR(%r3)
        std	%r0,REGS_OFS_GPR0(%r3)
        std	%r1,REGS_OFS_GPR1(%r3)
        std	%r2,REGS_OFS_GPR2(%r3)
        std	%r7,REGS_OFS_GPR7(%r3)
        std	%r8,REGS_OFS_GPR8(%r3)
        std	%r9,REGS_OFS_GPR9(%r3)
        std	%r10,REGS_OFS_GPR10(%r3)
        std	%r11,REGS_OFS_GPR11(%r3)
        std	%r12,REGS_OFS_GPR12(%r3)
        std	%r13,REGS_OFS_GPR13(%r3)
	std	%r14,REGS_OFS_GPR14(%r3)
        std	%r15,REGS_OFS_GPR15(%r3)
        std	%r16,REGS_OFS_GPR16(%r3)
        std	%r17,REGS_OFS_GPR17(%r3)
        std	%r18,REGS_OFS_GPR18(%r3)
	std	%r19,REGS_OFS_GPR19(%r3)
        std	%r20,REGS_OFS_GPR20(%r3)
        std	%r21,REGS_OFS_GPR21(%r3)
        std	%r22,REGS_OFS_GPR22(%r3)
        std	%r23,REGS_OFS_GPR23(%r3)
	std	%r24,REGS_OFS_GPR24(%r3)
        std	%r25,REGS_OFS_GPR25(%r3)
        std	%r26,REGS_OFS_GPR26(%r3)
        std	%r27,REGS_OFS_GPR27(%r3)
        std	%r28,REGS_OFS_GPR28(%r3)
	std	%r29,REGS_OFS_GPR29(%r3)
        std	%r30,REGS_OFS_GPR30(%r3)
        std	%r31,REGS_OFS_GPR31(%r3)
        mfxer	%r5
        mfctr	%r6
        mfcr	%r7
        std	%r5,REGS_OFS_XER(%r3)
        std	%r6,REGS_OFS_CTR(%r3)
        std	%r7,REGS_OFS_CR(%r3)

        //
        // set the stack to BeDRAM :	
        //
        mfspr  %r8,SPRN_PIR                   // r8 = ThdID
        IMM64( %r7,fw_stacks )                // r7 = &(fw_stacks[0]);
        rldic  %r8,%r8,10,46                  // r8 *= FW_STACK_SIZE;
        add    %r7,%r7,%r8                    // r7 += r8;
	addi   %r1,%r7,FW_STACK_SIZE          // r1 = (r7 + FW_STACK_SIZE - 64)

	IMM64(%r2, fw_machineCheckHandler)
	ld	%r2,8(%r2)
	bl      fw_machineCheckHandler
	# return from handler:	
		
        mfspr	%r3, SPRN_GSPRG0
        ld	%r5, REGS_OFS_XER(%r3)
        ld	%r6, REGS_OFS_CTR(%r3)
        ld	%r7, REGS_OFS_CR(%r3)
        mtxer	%r5
        mtctr	%r6
        mtcr	%r7
	ld	%r5,REGS_OFS_LR(%r3)
	mtlr	%r5
        
        ld	%r0, REGS_OFS_GPR0(%r3)
        ld	%r1, REGS_OFS_GPR1(%r3)
        ld	%r2, REGS_OFS_GPR2(%r3)
        ld	%r6, REGS_OFS_GPR6(%r3)
        ld	%r7, REGS_OFS_GPR7(%r3)
        ld	%r8, REGS_OFS_GPR8(%r3)
        ld	%r9, REGS_OFS_GPR9(%r3)
        ld	%r10,REGS_OFS_GPR10(%r3)
        ld	%r11,REGS_OFS_GPR11(%r3)
        ld	%r12,REGS_OFS_GPR12(%r3)
        ld	%r13,REGS_OFS_GPR13(%r3)
        ld	%r14,REGS_OFS_GPR14(%r3)
        ld	%r15,REGS_OFS_GPR15(%r3)
        ld	%r16,REGS_OFS_GPR16(%r3)
        ld	%r17,REGS_OFS_GPR17(%r3)
        ld	%r18,REGS_OFS_GPR18(%r3)
        ld	%r19,REGS_OFS_GPR19(%r3)
        ld	%r20,REGS_OFS_GPR20(%r3)
        ld	%r21,REGS_OFS_GPR21(%r3)
        ld	%r22,REGS_OFS_GPR22(%r3)
        ld	%r23,REGS_OFS_GPR23(%r3)
        ld	%r24,REGS_OFS_GPR24(%r3)
        ld	%r25,REGS_OFS_GPR25(%r3)
        ld	%r26,REGS_OFS_GPR26(%r3)
        ld	%r27,REGS_OFS_GPR27(%r3)
        ld	%r28,REGS_OFS_GPR28(%r3)
        ld	%r29,REGS_OFS_GPR29(%r3)
        ld	%r30,REGS_OFS_GPR30(%r3)
        ld	%r31,REGS_OFS_GPR31(%r3)
        
        ld	%r4, REGS_OFS_IP(%r3)
        ld	%r5, REGS_OFS_MSR(%r3)
        mtspr	SPRN_MCSRR0_IP, %r4
        mtspr	SPRN_MCSRR1_MSR, %r5
        ld	%r4, REGS_OFS_GPR4(%r3)
        ld	%r5, REGS_OFS_GPR5(%r3)
        mfspr	%r3, SPRN_GSPRG1
        rfmci



callCrash:

	.global callCrash
	
	IMM64( %r2, unexpectedInterrupt )	; 
	ld     %r2,8(%r2)	;
	b     unexpectedInterrupt		; 
        .align 5

        _EPILOG( machineCheckEntry )
	
#_fw_Vectors_End:
#        .global _fw_Vectors_End
        //
        // 3KB of space (3,232 bytes) available between end of vectors and reset page.
        //  (assuming full reset page, so prolly more like 4.5K total)
        //

	.align 5
_fw_zero32:
	.space 32,0


//------------------------------------------------------------------------------
// _fw_resetConfig: performs final step of cycle reproducible reset
//
_fw_resetConfig:
        .global _fw_resetConfig
        mr %r9, %r3
        IMM64(%r5, PHYMAP_MINADDR_DCR + PHYMAP_PRIVILEGEDOFFSET)
	
#define TIDCRWrite(dcrname, value) \
        IMM64(%r4, (TESTINT_DCR(dcrname) << 3)); \
        add %r4, %r4, %r5; \
        IMM64(%r3, value); \
        std %r3, 0(%r4);

#define TIDCRWrite2(dcrname) \
        IMM64(%r4, (TESTINT_DCR(dcrname) << 3)); \
        add %r4, %r4, %r5; \
        std %r6, 0(%r4);

#define TIDCRWrite3(dcrname) \
        IMM64(%r4, (TESTINT_DCR(dcrname) << 3)); \
        add %r4, %r4, %r5; \
        std %r9, 0(%r4);
	
	TIDCRWrite(COUNTER, 0x0000000000000000)
        li %r7, 0
_fw_resetConfig_retry:  
        mr %r6, %r7
//        IMM64(%r7, (16 << 40) | (64 << 20) | 128)
        IMM64(%r7, _B1(54,1))  // ctrlnet will now set the wakeup counter with svn rev r37957/r37959
        TIDCRWrite(THREAD_ACTIVE0, 0x8000000000000000)
        TIDCRWrite(THREAD_ACTIVE1, 0x0000000000000000)
        nop
        nop
        nop
        nop
        isync
        msync
        isync
        TIDCRWrite(CONFIG0A, 0)
        TIDCRWrite3(CONFIG1A)
        TIDCRWrite(THREAD_ACTIVE0A, 0)
        TIDCRWrite(THREAD_ACTIVE1A, 0)
	isync
        TIDCRWrite2(GLOB_ATT)
        isync
        cmpwi %r6, 0
        beq _fw_resetConfig_goback
        wait
_fw_resetConfig_goback: 
        b   _fw_resetConfig_retry

_fw_resetConfig_end:
        .global _fw_resetConfig_end
        nop
        
	
//------------------------------------------------------------------------------
// _fw_Reset64: Now that 64bit Mode has been enabled, complete Core/Chip Initialization
//
_fw_Reset64:
        li      %r0,0
        li      %r1,0
      //  li      %r2,0
        li      %r3,0
        li      %r4,0
        li      %r5,0
        li      %r6,0
        li      %r7,0
        li      %r8,0
        li      %r9,0
        li      %r10,0
        li      %r11,0
        li      %r12,0
        li      %r13,0
        li      %r14,0
        li      %r15,0
        li      %r16,0
        li      %r17,0
        li      %r18,0
        li      %r19,0
        li      %r20,0
        li      %r21,0
        li      %r22,0
        li      %r23,0
        li      %r24,0
        li      %r25,0
        li      %r26,0
        li      %r27,0
        li      %r28,0
        li      %r29,0
        li      %r30,0
        li      %r31,0


        // 10. setup ERAT or TLB: Thread 0 of each Core, while others spin
        // 10.1. wipe TLB
        // 10.2. init MMUCR3
        // 10.3. setup MAS0
        // 10.4. setup RPN in MAS registers for initial program page
        // 10.5. setup EPN in MAS registers for initial program page
        // 10.6. tlbwe to install
        // 10.7. init PID
        // 10.8. MSR[IS,DS]
        // 10.9. SSR1 and SRR0
        // 10.10. isync or rfi
        // 11. Pinned ERAT entries
        // 11.1. ERAT LRU watermark
        // 11.2. MMUCR0
        // 11.3. RPN of ERAT
        // 11.4. EPN of ERAT, then eratwe
        // 12. Init interrupt resources
        // 12.1. (G)IVPR
        // 12.2. memory contents of interrupt handlers
        // 12.3. init EHCSR for hypervisor vs guest
        // 12.4. msync
        // 13. Debug facilities
        // 13.1 DBCR1-DBCR3
        // 13.2 IMMR and IMR
        // 13.3 IAC1-IAC4, DAC1-DAC4, DVC1-DCV2
        // 13.4 THRCTL[DBA]
        // 13.5 EHCSR when MSR[GS]=0
        // 13.6 DBCR0 and DBCR3
        // 13.7 isync
        // 14. Timer facilities
        // 15. External Interrupts
        // 16. Zero MSR[CE,EE,DE,ME], then isync
        // 17. GPRs, SPRs as required.
        // 18. External logic as required.
        // 19. Memory
        // 20. Go.

        //
        // setup my stack address
        //
        mfspr  %r8,SPRN_PIR                   // r8 = ThdID
        IMM64( %r7,fw_stacks )                // r7 = &(fw_stacks[0]);
        rldic  %r8,%r8,10,46                  // r8 *= FW_STACK_SIZE;
        add    %r7,%r7,%r8                    // r7 += r8;
	//      addi   %r1,%r7,(FW_STACK_SIZE - 64)  // r1 = (r7 + FW_STACK_SIZE - 64)
	addi   %r1,%r7,FW_STACK_SIZE          // r1 = (r7 + FW_STACK_SIZE - 64)
        //
        // branch to fw_main with LR pointing to Crash
        //
        IMM64( %r5, crash )
        ld     %r5,0(%r5)
        mtlr   %r5
        IMM64( %r3, MSR_KERNEL_FULL_DISABLED )
        IMM64( %r4, fw_main )
        ld     %r2,8(%r4)
        ld     %r4,0(%r4)
        //
        mtspr SPRN_SRR1_MSR,%r3
        mtspr SPRN_SRR0_IP,%r4
        rfi
        nop
        b    crash
        _EPILOG( _fw_Reset64 )
//------------------------------------------------------------------------------
        _PROLOG_RESET( _fw_Reset32 )
_fw_Reset32:
	# DD1 Workaround: We want to delay the first msync by 8200 cycles.  A spin
	#   loop of 80 iterations gives us what we need.
	li	  %r0,80
	mtctr	  %r0
	bdnz	  0
	# end of DD1 workaround
        li        %r0,0                    // Enter here from reset vector
        li        %r31,-1
        mtcr      %r0
        ici       0                      // 2. ici
        dci       0                      // 3. dci
        msync                              // 4. msync
        //
        SET_SPRC( %r3, SPRN_XUCR0, XUCR0_INIT)
        //
        mtspr     SPRN_DBCR0,%r0           // 5. clear dbcr0, dbcr3
        mtspr     SPRN_DBCR1,%r0
        mtspr     SPRN_DBCR2,%r0
        mtspr     SPRN_DBCR3,%r0
                                           // 6,7,8. clear thrctl, errinj, RAMC (via SCOM only)
        mtspr     SPRN_DBSR,%r31           // 9. write-clear dbsr
        //
        SET_SPRC( %r3, SPRN_EHCSR, (EHCSR_ICM | EHCSR_GICM) )
        //
        SET_SPRC( %r3, SPRN_MMUCR1, MMUCR1_INIT ) // isync does not invaliate ERATs and disable Erat Round-Robbin
        //
        IMM32C(   %r3, MSR_CM | MSR_ICM ) // Only enable 64 bit mode (no FPU, no machine checks, etc.)
        mtmsr     %r3
        isync
        // set IUCR1 for all threads, balanced performance
        SET_SPRC( %r3, SPRN_IUCR1, IUCR1_INIT );
        //
	// Thread 0 reaches this point first.  It will release the other 3
	// threads after doing some erat fiddling.  When those threads get
	// here, they need to skip the erat stuff.
        mfspr     %r3,SPRN_PIR             // who am I?
        clrlwi.   %r4,%r3,30               // r4 = (r3 & 0x3)
        bne-      5f                       // ThdID != 0 ? skip ahead

	// The core starts up with I_ERAT[15] and D_ERAT[31] mapping
	// virtual 0 to physical 0x3ff00000000.  The I_ERAT entry (at least)
	// causes problems because it allows the core to issue speculative
	// loads from virtual 0, which for BG/Q winds up reading a DCR value.
	// We simply invalidate both entries here to make 0 inaccessible.
        li        %r4,0
        li        %r7,15
        SET_SPRC( %r6, SPRN_MMUCR0, (MMUCR0_ECL(3) | MMUCR0_TLBSEL_IERAT) )
        eratwe    %r4,%r7,1
        eratwe    %r4,%r7,0
        li        %r7,31
        SET_SPRC( %r6, SPRN_MMUCR0, (MMUCR0_ECL(3) | MMUCR0_TLBSEL_DERAT) )
        eratwe    %r4,%r7,1
        eratwe    %r4,%r7,0

        //
        // ERAT for Privileged Boot eDRAM and MMIO
        // @todo: remove user-space access from these!!!! 
        IMM64(    %r4, (ERAT0_EPN(VA_MINADDR_MMIO | VA_PRIVILEGEDOFFSET) | ERAT0_Class(1) | ERAT0_V | ERAT0_Size_1G | ERAT0_ThdID(0xF)) )
        IMM64(    %r5, (ERAT1_RPN(PHYMAP_MINADDR_MMIO | PHYMAP_PRIVILEGEDOFFSET) | ERAT1_R | ERAT1_C | ERAT1_M | ERAT1_U1 | ERAT1_SX | ERAT1_SW | ERAT1_SR) )
        //
        li %r7,0
        SET_SPRC( %r6, SPRN_MMUCR0, (MMUCR0_ECL(3) | MMUCR0_TLBSEL_IERAT) )
        eratwe    %r5,%r7,1
        eratwe    %r4,%r7,0
        //
	ori       %r5,%r5,(ERAT1_I | ERAT1_G)  // Guarded, Inhibited for data-side
	//
        li        %r7,0
        SET_SPRC( %r6, SPRN_MMUCR0, (MMUCR0_ECL(3) | MMUCR0_TLBSEL_DERAT) )
        eratwe    %r5,%r7,1
        eratwe    %r4,%r7,0
	
	
	
#if 0
        //
        // wipe 2-13 (inclusive) of IERAT (IERAT[14] is reset vector, IERAT[15] is 4K at 0).
        //
        li        %r7,2
        li        %r0,0
        SET_SPRC( %r6, SPRN_MMUCR0, (MMUCR0_ECL(3) | MMUCR0_TLBSEL_IERAT) )
3:      eratwe    %r0,%r7,1
        eratwe    %r0,%r7,0
        addi      %r7,%r7,1
        cmpwi     %r7,14
        blt       3b
        //
        // wipe 4-29 (inclusive) of DERAT (DERAT[30] is reset vector, DERAT[31] is 4K at 0).
        //
        li        %r7,4
        SET_SPRC( %r6, SPRN_MMUCR0, (MMUCR0_ECL(3) | MMUCR0_TLBSEL_DERAT) )
4:      eratwe    %r0,%r7,1
        eratwe    %r0,%r7,0
        addi      %r7,%r7,1
        cmpwi     %r7,30
        blt       4b
#endif

	 //
        isync
        //
        IMM64(    %r8, _fw_Vec0_DEBUG )
        mtspr     SPRN_IVPR, %r8
        isync
	// release the other 3 threads
	SET_SPRC( %r3, SPRN_TENS, TEN_TEN_ALL )
	isync

        // all 4 threads reach this point
5:      li        %r0,0
	IMM64(    %r3, MSR_CM | MSR_ICM ) // Do not enable the FPU
        IMM64(    %r4, _fw_Reset64 )
        mtspr     SPRN_SRR1_MSR,%r3
        mtspr     SPRN_SRR0_IP,%r4
        rfi
        _EPILOG( _fw_Reset32 )
//------------------------------------------------------------------------------
// A2_ResetVector: Reset Vector entry point.
//
        _PROLOG_RESET_VECTOR( _ResetVector )
_ResetVector:
        //  1. Entry at V=0x0000_0000_FFFF_FFFC P=0x0000_03ff_ffff_fffc in 32bit mode.
        b       _fw_Reset32
        _EPILOG( _ResetVector )
//------------------------------------------------------------------------------
