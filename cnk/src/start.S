// begin_generated_IBM_copyright_prolog                             
//                                                                  
// This is an automatically generated copyright prolog.             
// After initializing,  DO NOT MODIFY OR MOVE                       
// ================================================================ 
//                                                                  
// Licensed Materials - Property of IBM                             
//                                                                  
// Blue Gene/Q                                                      
//                                                                  
// (C) Copyright IBM Corp.  2011, 2012                              
//                                                                  
// US Government Users Restricted Rights -                          
// Use, duplication or disclosure restricted                        
// by GSA ADP Schedule Contract with IBM Corp.                      
//                                                                  
// This software is available to you under the                      
// Eclipse Public License (EPL).                                    
//                                                                  
// ================================================================ 
//                                                                  
// end_generated_IBM_copyright_prolog                               


#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

#include <hwi/include/common/compiler_support.h>
#include <hwi/include/common/asm_support.h>
#include <hwi/include/common/bgq_alignment.h>

#include <hwi/include/bqc/A2_core.h>

#include "Kernel_Linkage.h"
#include "Debug.h"
#include "CoreState.h"
#include "Regs.h"
#include "KThread.h"
#include "syscalls.h"
#include <cnk/include/SPRG_Usage.h>

#include "asmConstants.h"

#define REGS_GPR(n) (REGS_gpr + ((n)*8))
#define REGS_SPRG(n) (REGS_sprg + ((n)*8))
#define REGS_GSPRG(n) (REGS_gsprg + ((n)*8))

#if (KTHR_RegState != 0)
    #error "RegState is not at the beginning of KThread_t!"
#endif

#if (HWTHR_CriticalState != 0)
    #error "CriticalState is not at the beginning of HWThreadState_t!"
#endif

#define HWTHR_STDSTACK_BOT  (HWTHR_StandardStack + KSTACK_SIZE)
#define HWTHR_CRITSTACK_BOT (HWTHR_CriticalStack + KSTACK_SIZE)

        _PROLOG_ABS0(KERNEL_CHECKSUM)
KERNEL_CHECKSUM:
        .long 0                             // Set during kernel build.
        .long 0                             // Check before kernal launch.
        _EPILOG(KERNEL_CHECKSUM)
        _PROLOG_ABS0(KERNEL_CSPAD)
KERNEL_CSPAD:
        .long 0
        .long 0x42475121                    // The string "BGQ!"
        _EPILOG(KERNEL_CSPAD)

//------------------------------------------------------------------------------
//  Kernel_Start: entry point for CNK.
//
//  Syntax: void Kernel_Start(_Firmware_Interface_t *fwi);
//
        .extern Kernel_EntryPrimary
        .extern Kernel_Crash
        .global _start
        _PROLOG_START(Kernel_Start)
Kernel_Start:
_start:
        li     %r0,0
        IMM64(%r2, __CNK_TOC_BASE)
        bl     Kernel_EntryPrimary
        nop
        bl     Kernel_Finish
        nop
        b      Kernel_Start
        nop
        _EPILOG(Kernel_Start)

//
// Internal routine.  Saves nonvolatile GPRs to a RegState structure.
//
	_PROLOG_VECTORS(LC_Save_Nonvolatile_GPRs)
LC_Save_Nonvolatile_GPRs:
	//
	// On entry:
	//	r3:  address of Regstate structure
	//
	// Preserves all registers.
	//
	std	%r14,REGS_GPR(14)(%r3)
	std	%r15,REGS_GPR(15)(%r3)
	std	%r16,REGS_GPR(16)(%r3)
	std	%r17,REGS_GPR(17)(%r3)
	std	%r18,REGS_GPR(18)(%r3)
	std	%r19,REGS_GPR(19)(%r3)
	std	%r20,REGS_GPR(20)(%r3)
	std	%r21,REGS_GPR(21)(%r3)
	std	%r22,REGS_GPR(22)(%r3)
	std	%r23,REGS_GPR(23)(%r3)
	std	%r24,REGS_GPR(24)(%r3)
	std	%r25,REGS_GPR(25)(%r3)
	std	%r26,REGS_GPR(26)(%r3)
	std	%r27,REGS_GPR(27)(%r3)
	std	%r28,REGS_GPR(28)(%r3)
	std	%r29,REGS_GPR(29)(%r3)
	std	%r30,REGS_GPR(30)(%r3)
	std	%r31,REGS_GPR(31)(%r3)
	blr
	nop
	_EPILOG(LC_Save_Nonvolatile_GPRs)

//
// Internal routine.  Restores nonvolatile GPRs from a RegState structure.
//
	_PROLOG_VECTORS(LC_Restore_Nonvolatile_GPRs)
LC_Restore_Nonvolatile_GPRs:
	//
	// On entry:
	//	r3:  address of Regstate structure
	//
	// Preserves all volatile registers.
	//
	ld	%r14,REGS_GPR(14)(%r3)
	ld	%r15,REGS_GPR(15)(%r3)
	ld	%r16,REGS_GPR(16)(%r3)
	ld	%r17,REGS_GPR(17)(%r3)
	ld	%r18,REGS_GPR(18)(%r3)
	ld	%r19,REGS_GPR(19)(%r3)
	ld	%r20,REGS_GPR(20)(%r3)
	ld	%r21,REGS_GPR(21)(%r3)
	ld	%r22,REGS_GPR(22)(%r3)
	ld	%r23,REGS_GPR(23)(%r3)
	ld	%r24,REGS_GPR(24)(%r3)
	ld	%r25,REGS_GPR(25)(%r3)
	ld	%r26,REGS_GPR(26)(%r3)
	ld	%r27,REGS_GPR(27)(%r3)
	ld	%r28,REGS_GPR(28)(%r3)
	ld	%r29,REGS_GPR(29)(%r3)
	ld	%r30,REGS_GPR(30)(%r3)
	ld	%r31,REGS_GPR(31)(%r3)
	blr
	nop
	_EPILOG(LC_Restore_Nonvolatile_GPRs)

//
// System call semantics require us to preserve the nonvolatile FPRs (the
// primary slots of qvr14 - qvr31).  We want to store them densely to minimize
// cache misses, so we don't keep them in their natural locations in the
// Regs.qvr array.  We save the FPRs, starting with f14, at the beginning of
// the qvr space.
//
#define REGS_NV_FPR(n) (REGS_qvr + (((n) - 14) * 8))

//
// Internal routine.  Saves nonvolatile FPRs to a RegState structure.
//
	_PROLOG_VECTORS(LC_Save_Nonvolatile_FPRs)
LC_Save_Nonvolatile_FPRs:
	//
	// On entry:
	//	r3:  address of Regstate structure
	//
	// Preserves all registers except r0, r12 and f0.
	//
	mfmsr	%r12
	ori	%r0,%r12,MSR_FP
	mtmsr	%r0
	stfd	%f14,REGS_NV_FPR(14)(%r3)
	stfd	%f15,REGS_NV_FPR(15)(%r3)
	stfd	%f16,REGS_NV_FPR(16)(%r3)
	stfd	%f17,REGS_NV_FPR(17)(%r3)
	stfd	%f18,REGS_NV_FPR(18)(%r3)
	stfd	%f19,REGS_NV_FPR(19)(%r3)
	stfd	%f20,REGS_NV_FPR(20)(%r3)
	stfd	%f21,REGS_NV_FPR(21)(%r3)
	stfd	%f22,REGS_NV_FPR(22)(%r3)
	stfd	%f23,REGS_NV_FPR(23)(%r3)
	stfd	%f24,REGS_NV_FPR(24)(%r3)
	stfd	%f25,REGS_NV_FPR(25)(%r3)
	stfd	%f26,REGS_NV_FPR(26)(%r3)
	stfd	%f27,REGS_NV_FPR(27)(%r3)
	stfd	%f28,REGS_NV_FPR(28)(%r3)
	stfd	%f29,REGS_NV_FPR(29)(%r3)
	stfd	%f30,REGS_NV_FPR(30)(%r3)
	stfd	%f31,REGS_NV_FPR(31)(%r3)
	mffs	%f0
	stfd	%f0,REGS_fpscr(%r3)
	mtmsr	%r12
	blr
	nop
	_EPILOG(LC_Save_Nonvolatile_FPRs)

//
// Internal routine.  Saves QPX state to a RegState structure.
//
	_PROLOG_VECTORS(LC_Save_QPX)
LC_Save_QPX:
	//
	// On entry:
	//	r3:  address of Regstate structure
	//
	// Preserves all registers except r0, r11, r12, and f0.
	//
	mfmsr	%r12
	ori	%r0,%r12,MSR_FP
	mtmsr	%r0
	la	%r11,(REGS_qvr-32)(%r3)
	li	%r0,32
	qvstfdux %f0,%r11,%r0
	qvstfdux %f1,%r11,%r0
	qvstfdux %f2,%r11,%r0
	qvstfdux %f3,%r11,%r0
	qvstfdux %f4,%r11,%r0
	qvstfdux %f5,%r11,%r0
	qvstfdux %f6,%r11,%r0
	qvstfdux %f7,%r11,%r0
	qvstfdux %f8,%r11,%r0
	qvstfdux %f9,%r11,%r0
	qvstfdux %f10,%r11,%r0
	qvstfdux %f11,%r11,%r0
	qvstfdux %f12,%r11,%r0
	qvstfdux %f13,%r11,%r0
	qvstfdux %f14,%r11,%r0
	qvstfdux %f15,%r11,%r0
	qvstfdux %f16,%r11,%r0
	qvstfdux %f17,%r11,%r0
	qvstfdux %f18,%r11,%r0
	qvstfdux %f19,%r11,%r0
	qvstfdux %f20,%r11,%r0
	qvstfdux %f21,%r11,%r0
	qvstfdux %f22,%r11,%r0
	qvstfdux %f23,%r11,%r0
	qvstfdux %f24,%r11,%r0
	qvstfdux %f25,%r11,%r0
	qvstfdux %f26,%r11,%r0
	qvstfdux %f27,%r11,%r0
	qvstfdux %f28,%r11,%r0
	qvstfdux %f29,%r11,%r0
	qvstfdux %f30,%r11,%r0
	qvstfdux %f31,%r11,%r0
	mffs	%f0
	stfd	%f0,REGS_fpscr(%r3)
	mtmsr	%r12
	blr
	nop
	_EPILOG(LC_Save_QPX)

//
// Internal routine.  Restores QPX state from a RegState structure.
//
	_PROLOG_VECTORS(LC_Restore_QPX)
LC_Restore_QPX:
	//
	// On entry:
	//	r3:  address of Regstate structure
	//
	// Preserves all general-purpose registers except r0, r11, and r12.
	//
	mfmsr	%r12
	ori	%r0,%r12,MSR_FP
	mtmsr	%r0
	la	%r11,(REGS_qvr-32)(%r3)
	li	%r0,32
	lfd	%f0,REGS_fpscr(%r3)
	mtfsf	0,%f0,1,0
	qvlfdux	%f0,%r11,%r0
	qvlfdux	%f1,%r11,%r0
	qvlfdux	%f2,%r11,%r0
	qvlfdux	%f3,%r11,%r0
	qvlfdux	%f4,%r11,%r0
	qvlfdux	%f5,%r11,%r0
	qvlfdux	%f6,%r11,%r0
	qvlfdux	%f7,%r11,%r0
	qvlfdux	%f8,%r11,%r0
	qvlfdux	%f9,%r11,%r0
	qvlfdux	%f10,%r11,%r0
	qvlfdux	%f11,%r11,%r0
	qvlfdux	%f12,%r11,%r0
	qvlfdux	%f13,%r11,%r0
	qvlfdux	%f14,%r11,%r0
	qvlfdux	%f15,%r11,%r0
	qvlfdux	%f16,%r11,%r0
	qvlfdux	%f17,%r11,%r0
	qvlfdux	%f18,%r11,%r0
	qvlfdux	%f19,%r11,%r0
	qvlfdux	%f20,%r11,%r0
	qvlfdux	%f21,%r11,%r0
	qvlfdux	%f22,%r11,%r0
	qvlfdux	%f23,%r11,%r0
	qvlfdux	%f24,%r11,%r0
	qvlfdux	%f25,%r11,%r0
	qvlfdux	%f26,%r11,%r0
	qvlfdux	%f27,%r11,%r0
	qvlfdux	%f28,%r11,%r0
	qvlfdux	%f29,%r11,%r0
	qvlfdux	%f30,%r11,%r0
	qvlfdux	%f31,%r11,%r0
	mtmsr	%r12
	blr
	nop
	_EPILOG(LC_Restore_QPX)

//
// Internal routine.  Saves SPR content to a RegState structure.
//
	_PROLOG_VECTORS(LC_Save_SPRs)
LC_Save_SPRs:
	//
	// On entry:
	//	r3:  address of Regstate structure
	//
	// Preserves all registers except r0.
	//
	mfspr	%r0,SPRN_SPRG0
	std	%r0,REGS_SPRG(0)(%r3)
	mfspr	%r0,SPRN_SPRG1
	std	%r0,REGS_SPRG(1)(%r3)
	mfspr	%r0,SPRN_SPRG2
	std	%r0,REGS_SPRG(2)(%r3)
	mfspr	%r0,SPRN_SPRG3
	std	%r0,REGS_SPRG(3)(%r3)
	mfspr	%r0,SPRN_SPRG4
	std	%r0,REGS_SPRG(4)(%r3)
	mfspr	%r0,SPRN_SPRG5
	std	%r0,REGS_SPRG(5)(%r3)
	mfspr	%r0,SPRN_SPRG6
	std	%r0,REGS_SPRG(6)(%r3)
	mfspr	%r0,SPRN_SPRG7
	std	%r0,REGS_SPRG(7)(%r3)
	mfspr	%r0,SPRN_SPRG8
	std	%r0,REGS_SPRG(8)(%r3)
	mfspr	%r0,SPRN_GSPRG0
	std	%r0,REGS_GSPRG(0)(%r3)
	mfspr	%r0,SPRN_GSPRG1
	std	%r0,REGS_GSPRG(1)(%r3)
	mfspr	%r0,SPRN_GSPRG2
	std	%r0,REGS_GSPRG(2)(%r3)
	mfspr	%r0,SPRN_GSPRG3
	std	%r0,REGS_GSPRG(3)(%r3)
	mfspr	%r0,SPRN_PID
	std	%r0,REGS_pid(%r3)
        blr
        nop
        _EPILOG(LC_Save_SPRs)

//
// Internal routine.  Restores SPR content from a RegState structure.
//
	_PROLOG_VECTORS(LC_Restore_SPRs)
LC_Restore_SPRs:
	//
	// On entry:
	//	r3:  address of Regstate structure
	//
	// Preserves all registers except r0.
	//
	ld	%r0,REGS_SPRG(0)(%r3)
	mtspr	SPRN_SPRG0,%r0
	ld	%r0,REGS_SPRG(1)(%r3)
	mtspr	SPRN_SPRG1,%r0
	ld	%r0,REGS_SPRG(2)(%r3)
	mtspr	SPRN_SPRG2,%r0
	ld	%r0,REGS_SPRG(3)(%r3)
	mtspr	SPRN_SPRG3,%r0
	ld	%r0,REGS_SPRG(4)(%r3)
	mtspr	SPRN_SPRG4,%r0
	ld	%r0,REGS_SPRG(5)(%r3)
	mtspr	SPRN_SPRG5,%r0
	ld	%r0,REGS_SPRG(6)(%r3)
	mtspr	SPRN_SPRG6,%r0
	ld	%r0,REGS_SPRG(7)(%r3)
	mtspr	SPRN_SPRG7,%r0
	ld	%r0,REGS_SPRG(8)(%r3)
	mtspr	SPRN_SPRG8,%r0
	ld	%r0,REGS_GSPRG(0)(%r3)
	mtspr	SPRN_GSPRG0,%r0
	ld	%r0,REGS_GSPRG(1)(%r3)
	mtspr	SPRN_GSPRG1,%r0
	ld	%r0,REGS_GSPRG(2)(%r3)
	mtspr	SPRN_GSPRG2,%r0
	ld	%r0,REGS_GSPRG(3)(%r3)
	mtspr	SPRN_GSPRG3,%r0
	ld	%r0,REGS_pid(%r3)
	mtspr	SPRN_PID,%r0
        blr
        nop
        _EPILOG(LC_Restore_SPRs)

//------------------------------------------------------------------------------
//
//  Machine Check Vector and save/restore routine.
//      Firmware handles machine checks, so this stub is just a dummy.
//
#define _MCHK(label) \
        .align 5; \
        .global label; \
label:  b label

//------------------------------------------------------------------------------
//
// Critical Debug Exception Vector and save/restore routine.
//
#define _CDBG(label, code) \
	.align 5; \
	.global label; \
label: \
	mtspr	SPRN_SPRG0,%r3; \
	mtspr	SPRN_SPRG1,%r4; \
	mtspr	SPRN_SPRG2,%r5; \
	mflr	%r5; \
	bl	LC_Debug_Crit_Save; \
	/* void IntHandler_Debug(Regs_t *regs, int code); */ \
	li	%r4,code; \
	b	IntHandler_Debug
	// IntHandler will return directly to CriticalSave or StandardSave
//
	_PROLOG_VECTORS(LC_Debug_Crit_Save)
LC_Debug_Crit_Save:
	mfspr	%r4,SPRN_CSRR1_MSR
	mfcr	%r3
	andi.	%r4,%r4,MSR_PR		// check for user-mode
	bne-	1f
	// exception occurred in kernel mode: continue as in CVEC
	mtcr	%r3			// restore cr
	b	LC_Critical_Save
    1:
	// exception occurred in user mode: continue as in SVEC
	mtcr	%r3			// restore cr
	mfspr	%r3,SPRN_SPRG0		// copy crit sprg's to std sprg's
	mtspr	SPRN_SPRG3,%r3
	mfspr	%r3,SPRN_SPRG1
	mtspr	SPRN_SPRG4,%r3
	mfspr	%r3,SPRN_SPRG2
	mtspr	SPRN_SPRG5,%r3
	mfspr	%r3,SPRN_CSRR0_IP	// copy crit srr's to std srr's
	mtspr	SPRN_SRR0_IP,%r3
	mfspr	%r3,SPRN_CSRR1_MSR
	mtspr	SPRN_SRR1_MSR,%r3
	mfmsr	%r3			// enable critical interrupts
	oris	%r3,%r3,(MSR_CE>>16)
	mtmsr	%r3
	b	LC_Standard_Save
	_EPILOG(LC_Debug_Crit_Save)

//------------------------------------------------------------------------------
//
// Standard Debug Exception Vector and save/restore routine.
//
#define _SDBG(label, code) \
	.align 5; \
	.global label; \
label: \
	mtspr	SPRN_SPRG3,%r3; \
	mtspr	SPRN_SPRG4,%r4; \
	mtspr	SPRN_SPRG5,%r5; \
	mflr	%r5; \
	bl	LC_Debug_Std_Save; \
	/* void IntHandler_Debug(Regs_t *regs, int code); */ \
	li	%r4,code; \
	b	IntHandler_Debug
	// IntHandler will return directly to StandardSave or CriticalSave
//
	_PROLOG_VECTORS(LC_Debug_Std_Save)
LC_Debug_Std_Save:
	mfspr	%r4,SPRN_SRR1_MSR
	mfcr	%r3
	andi.	%r4,%r4,MSR_PR	// check for user-mode
	beq-	1f
	// exception occurred in user mode: continue as in SVEC
	mtcr	%r3			// restore cr
	b	LC_Standard_Save
    1:
	// exception occurred in kernel mode: continue as in CVEC
	mtcr	%r3			// restore cr
	mfmsr	%r3			// disable critical & debug interrupts
	oris	%r3,%r3,(MSR_CE>>16)	//     force MSR_CE on
	xoris	%r3,%r3,(MSR_CE>>16)	//     invert
	ori	%r3,%r3,MSR_DE		//     force MSR_DE on
	xori	%r3,%r3,MSR_DE		//     invert
	mtmsr	%r3
	mfspr	%r3,SPRN_SPRG3		// copy std sprg's to crit sprg's
	mtspr	SPRN_SPRG0,%r3
	mfspr	%r3,SPRN_SPRG4
	mtspr	SPRN_SPRG1,%r3
	mfspr	%r3,SPRN_SPRG5
	mtspr	SPRN_SPRG2,%r3
	mfspr	%r3,SPRN_SRR0_IP	// copy std srr's to crit srr's
	mtspr	SPRN_CSRR0_IP,%r3
	mfspr	%r3,SPRN_SRR1_MSR
	mtspr	SPRN_CSRR1_MSR,%r3
	b	LC_Critical_Save
	_EPILOG(LC_Debug_Std_Save)

//------------------------------------------------------------------------------
//
// Critical Debug Exception Vector and save/restore routine.
//
#define _CVEC(label, fcn) \
	.align 5;			\
	.global label;			\
label: \
	mtspr	SPRN_SPRG0,%r3; \
	mtspr	SPRN_SPRG1,%r4; \
	mtspr	SPRN_SPRG2,%r5; \
	mflr	%r5; \
	bl	LC_Critical_Save; \
	b	fcn
	// fcn will return directly to after the bctrl below
//
	_PROLOG_VECTORS(LC_Critical_Save)
LC_Critical_Save:
	// save volatile state
	mfspr	%r3,SPRG_pHWThread
	std	%r0, REGS_GPR(0)(%r3)
	std	%r1, REGS_GPR(1)(%r3)
	std	%r2, REGS_GPR(2)(%r3)
	std	%r6, REGS_GPR(6)(%r3)
	std	%r7, REGS_GPR(7)(%r3)
	std	%r8, REGS_GPR(8)(%r3)
	std	%r9, REGS_GPR(9)(%r3)
	std	%r10,REGS_GPR(10)(%r3)
	std	%r11,REGS_GPR(11)(%r3)
	std	%r12,REGS_GPR(12)(%r3)
	std	%r13,REGS_GPR(13)(%r3)
	// r5 still holds lr
	mfcr	%r6
	mfxer	%r7
	mfctr	%r8
	mfspr	%r9,SPRN_CSRR0_IP
	mfspr	%r10,SPRN_CSRR1_MSR
	std	%r5,REGS_lr(%r3)
	std	%r6,REGS_cr(%r3)
	std	%r7,REGS_xer(%r3)
	std	%r8,REGS_ctr(%r3)
	std	%r9,REGS_ip(%r3)
	std	%r10,REGS_msr(%r3)
	mfspr	%r4,SPRN_SPRG0		// r3
	mfspr	%r5,SPRN_SPRG1		// r4
	mfspr	%r6,SPRN_SPRG2		// r5
	std	%r4,REGS_GPR(3)(%r3)
	std	%r5,REGS_GPR(4)(%r3)
	std	%r6,REGS_GPR(5)(%r3)
	std	%r1,(HWTHR_CRITSTACK_BOT-STACK_FRAME_SIZE)(%r3)	// backchain
	la	%r1,(HWTHR_CRITSTACK_BOT-STACK_FRAME_SIZE)(%r3)	// kernel stack
	IMM32(%r2, __CNK_TOC_BASE)	// r2 = kernel TOC
	mflr	%r0			// move lr to ctr to preserve it
	mtctr	%r0
	bl	LC_Save_Nonvolatile_GPRs
	bl	LC_Save_QPX		// kills r0, r11, r12 and f0
	bl	LC_Save_SPRs		// kills r0
	bctrl				// "call" back to _CVEC for branch to
					// handler, which will return here
	// restore state
	mfspr	%r3,SPRG_pHWThread
	bl	LC_Restore_SPRs		// kills r0
	bl	LC_Restore_QPX		// kills r0, r11, and r12
	bl	LC_Restore_Nonvolatile_GPRs
	// restore volatile state
	ld	%r5,REGS_lr(%r3)
	ld	%r6,REGS_cr(%r3)
	ld	%r7,REGS_xer(%r3)
	ld	%r8,REGS_ctr(%r3)
	ld	%r9,REGS_ip(%r3)
	ld	%r10,REGS_msr(%r3)
	mtlr	%r5
	mtcr	%r6
	mtxer	%r7
	mtctr	%r8
	mtspr	SPRN_CSRR0_IP,%r9
	mtspr	SPRN_CSRR1_MSR,%r10
	ld	%r0, REGS_GPR(0)(%r3)
	ld	%r1, REGS_GPR(1)(%r3)
	ld	%r2, REGS_GPR(2)(%r3)
	// r3 must be last
	ld	%r4, REGS_GPR(4)(%r3)
	ld	%r5, REGS_GPR(5)(%r3)
	ld	%r6, REGS_GPR(6)(%r3)
	ld	%r7, REGS_GPR(7)(%r3)
	ld	%r8, REGS_GPR(8)(%r3)
	ld	%r9, REGS_GPR(9)(%r3)
	ld	%r10,REGS_GPR(10)(%r3)
	ld	%r11,REGS_GPR(11)(%r3)
	ld	%r12,REGS_GPR(12)(%r3)
	ld	%r13,REGS_GPR(13)(%r3)
	ld	%r3, REGS_GPR(3)(%r3)
	rfci
	nop
	_EPILOG(LC_Critical_Save)

//------------------------------------------------------------------------------
//
// Standard Interrupt Vector and save/restore routine. 
//
#define _SVEC(label, fcn) \
	.align 5; \
	.global label; \
label: \
	mtspr	SPRN_SPRG3,%r3; \
	mtspr	SPRN_SPRG4,%r4; \
	mtspr	SPRN_SPRG5,%r5; \
	mflr	%r5; \
	bl	LC_Standard_Save; \
	b	fcn
	// fcn will return directly to after the blrl below
//
	_PROLOG_VECTORS(LC_Standard_Save)
LC_Standard_Save:
	mfspr	%r4,SPRG_pHWThread
	ld	%r3,HWTHR_pCurrentThread(%r4)
	std	%r0,REGS_GPR(0)(%r3)
	std	%r1,REGS_GPR(1)(%r3)
	std	%r2,REGS_GPR(2)(%r3)
	// Switch to kernel stack, chaining back to user stack for debugging.
	std	%r1,(HWTHR_STDSTACK_BOT-STACK_FRAME_SIZE)(%r4)	// backchain
	la	%r1,(HWTHR_STDSTACK_BOT-STACK_FRAME_SIZE)(%r4)	// kernel stack
	mfspr	%r0,SPRN_SRR1_MSR	// r0 = original msr
	mfcr	%r4			// r4 = original cr
	andi.	%r2,%r0,MSR_PR		// test for kernel mode
	IMM32(%r2, __CNK_TOC_BASE)	// r2 = kernel TOC
	bne+	1f
	// A standard interrupt in kernel mode can occur only inside
	// Scheduler(), which explicitly enables external interrupts.  We can
	// simply abandon the current context, including the stack content.
	// After calling the handler we go back into Scheduler(), starting
	// from scratch.
	blrl				// "call" back to _SVEC for branch to
					// handler, which will return here
	bl	Scheduler		// branch to Scheduler(void)
	// NORETURN
	trap
    1:
	// Exception occurred in user mode.  Save remaining volatile state.
	std	%r6,REGS_GPR(6)(%r3)
	std	%r7,REGS_GPR(7)(%r3)
	std	%r8,REGS_GPR(8)(%r3)
	std	%r9,REGS_GPR(9)(%r3)
	mfxer	%r6			// r6 = original xer
	mfctr	%r7			// r7 = original ctr
	mfspr	%r8,SPRN_SRR0_IP	// r8 = original ip
	std	%r10,REGS_GPR(10)(%r3)
	std	%r11,REGS_GPR(11)(%r3)
	std	%r12,REGS_GPR(12)(%r3)
	std	%r13,REGS_GPR(13)(%r3)
	mfspr	%r10,SPRN_SPRG3		// r10 = original r3
	mfspr	%r11,SPRN_SPRG4		// r11 = original r4
	mfspr	%r12,SPRN_SPRG5		// r12 = original r5
	std	%r0,REGS_msr(%r3)
	std	%r4,REGS_cr(%r3)
	std	%r5,REGS_lr(%r3)
	std	%r6,REGS_xer(%r3)
	std	%r7,REGS_ctr(%r3)
	std	%r8,REGS_ip(%r3)
	std	%r10,REGS_GPR(3)(%r3)
	std	%r11,REGS_GPR(4)(%r3)
	std	%r12,REGS_GPR(5)(%r3)
	blrl				// "call" back to _SVEC for branch to
					// handler, which will return here
	mfspr	%r11,SPRG_pHWThread	// retrieve HWThread and
					//     KThread pointers
	ld	%r12,HWTHR_pCurrentThread(%r11)
	b	LC_Interrupt_Return
	nop
	_EPILOG(LC_Standard_Save)

	_PROLOG_VECTORS(LC_Interrupt_Return)
LC_Interrupt_Return:
	//
	// On entry:
	//	r1:  Kernel stack
	//	r2:  Kernel TOC
	//	r11: Current HWThread pointer
	//	r12: Current KThread pointer
	//
	//	Volatile fixed-point registers have been saved in KThread.
	//	Non-volatile fixed-point registers and all QPX registers
	//		have their original values.
	//
	ld	%r0,KTHR_Pending(%r12)
	cmpldi	%r0,0
	bne-	LC_Interrupt_Handle_Pending
	ld	%r5,REGS_lr(%r12)
	ld	%r6,REGS_cr(%r12)
	ld	%r7,REGS_xer(%r12)
	ld	%r8,REGS_ctr(%r12)
	ld	%r9,REGS_ip(%r12)
	ld	%r10,REGS_msr(%r12)
	mtlr	%r5
	mtcr	%r6
	mtxer	%r7
	mtctr	%r8
	mtspr	SPRN_SRR0_IP,%r9
	mtspr	SPRN_SRR1_MSR,%r10
	ld	%r13,REGS_GPR(13)(%r12)
	ld	%r0,REGS_GPR(0)(%r12)
	ld	%r1,REGS_GPR(1)(%r12)
	ld	%r2,REGS_GPR(2)(%r12)
	ld	%r3,REGS_GPR(3)(%r12)
	ld	%r4,REGS_GPR(4)(%r12)
	ld	%r5,REGS_GPR(5)(%r12)
	ld	%r6,REGS_GPR(6)(%r12)
	ld	%r7,REGS_GPR(7)(%r12)
	ld	%r8,REGS_GPR(8)(%r12)
	ld	%r9,REGS_GPR(9)(%r12)
	ld	%r10,REGS_GPR(10)(%r12)
	ld	%r11,REGS_GPR(11)(%r12)
	ld	%r12,REGS_GPR(12)(%r12)	// must be last
	rfi
	nop
	_EPILOG(LC_Interrupt_Return)

	_PROLOG_VECTORS(LC_Interrupt_Handle_Pending)
LC_Interrupt_Handle_Pending:
	//
	// On entry:
	//	r0:  Pending bit vector
	//	r1:  Kernel stack
	//	r2:  Kernel TOC
	//	r11: Current HWThread pointer
	//	r12: Current KThread pointer
	//
	//	Volatile fixed-point registers have been saved in KThread.
	//	Non-volatile fixed-point registers and all QPX registers
	//		have their original values.
	//
	andi.	%r3,%r0,KTHR_PENDING_NVGPR
	bne-	LC_Interrupt_NVGPR
	andi.	%r3,%r0,KTHR_PENDING_SNAPSHOT
	bne-	LC_Interrupt_Snapshot
	andi.	%r3,%r0,KTHR_PENDING_MIGRATE
	bne-	LC_Interrupt_Migrate
	andi.	%r3,%r0,KTHR_PENDING_YIELD
	bne-	LC_Interrupt_Yield
	andi.	%r3,%r0,KTHR_PENDING_SIGNAL
	bne-	LC_Interrupt_Signal
	trap
	nop
	_EPILOG(LC_Interrupt_Handle_Pending)

	_PROLOG_VECTORS(LC_Interrupt_NVGPR)
LC_Interrupt_NVGPR:
	//
	// On entry:
	//	r0:  Pending bit vector
	//	r1:  Kernel stack
	//	r2:  Kernel TOC
	//	r11: Current HWThread pointer
	//	r12: Current KThread pointer
	//
	//	Volatile fixed-point registers have been saved in KThread.
	//	Non-volatile fixed-point registers and all QPX registers
	//		have their original values.
	//
	xori	%r0,%r0,KTHR_PENDING_NVGPR
	std	%r0,KTHR_Pending(%r12)
	// An interrupt handler has updated a non-volatile GPR in the kthread,
	// and we need to pick up the new value.  The target register number
	// is in PendingNVGPR.
	ld	%r3,KTHR_PendingNVGPR(%r12)
	subi	%r3,%r3,14	// non-volatiles start with r14
	tdlgei	%r3,18		// range check
	sldi	%r3,%r3,3	// 2 instructions (8 bytes) per case
	bl	1f
    1:	mflr	%r4		// r4 = address of 1:
	addi	%r4,%r4,2f-1b	// r4 = address of 2:
	add	%r4,%r4,%r3	// r4 = address of selected load below
	mtlr	%r4		// go do it
	blr
    2:	ld	%r14,REGS_GPR(14)(%r12); b LC_Interrupt_Return
	ld	%r15,REGS_GPR(15)(%r12); b LC_Interrupt_Return
	ld	%r16,REGS_GPR(16)(%r12); b LC_Interrupt_Return
	ld	%r17,REGS_GPR(17)(%r12); b LC_Interrupt_Return
	ld	%r18,REGS_GPR(18)(%r12); b LC_Interrupt_Return
	ld	%r19,REGS_GPR(19)(%r12); b LC_Interrupt_Return
	ld	%r20,REGS_GPR(20)(%r12); b LC_Interrupt_Return
	ld	%r21,REGS_GPR(21)(%r12); b LC_Interrupt_Return
	ld	%r22,REGS_GPR(22)(%r12); b LC_Interrupt_Return
	ld	%r23,REGS_GPR(23)(%r12); b LC_Interrupt_Return
	ld	%r24,REGS_GPR(24)(%r12); b LC_Interrupt_Return
	ld	%r25,REGS_GPR(25)(%r12); b LC_Interrupt_Return
	ld	%r26,REGS_GPR(26)(%r12); b LC_Interrupt_Return
	ld	%r27,REGS_GPR(27)(%r12); b LC_Interrupt_Return
	ld	%r28,REGS_GPR(28)(%r12); b LC_Interrupt_Return
	ld	%r29,REGS_GPR(29)(%r12); b LC_Interrupt_Return
	ld	%r30,REGS_GPR(30)(%r12); b LC_Interrupt_Return
	ld	%r31,REGS_GPR(31)(%r12); b LC_Interrupt_Return
	nop
	_EPILOG(LC_Interrupt_NVGPR)

	_PROLOG_VECTORS(LC_Interrupt_Snapshot)
LC_Interrupt_Snapshot:
	//
	// On entry:
	//	r0:  Pending bit vector
	//	r1:  Kernel stack
	//	r2:  Kernel TOC
	//	r11: Current HWThread pointer
	//	r12: Current KThread pointer
	//
	//	Volatile fixed-point registers have been saved in KThread.
	//	Non-volatile fixed-point registers and all QPX registers
	//		have their original values.
	//
	xori	%r0,%r0,KTHR_PENDING_SNAPSHOT
	std	%r0,KTHR_Pending(%r12)
        
        // Save non-volatile GPRs to rollback state
	la	%r3,HWTHR_RollbackState(%r11)
	bl	LC_Save_Nonvolatile_GPRs
        
        // Copy volatile state from KThread structure to rollback state structure
        ld     %r6, REGS_GPR(0)(%r12)
        ld     %r7, REGS_GPR(1)(%r12)
        ld     %r8, REGS_GPR(2)(%r12)
        ld     %r9, REGS_GPR(3)(%r12)
        std    %r6, REGS_GPR(0)(%r3)
        std    %r7, REGS_GPR(1)(%r3)
        std    %r8, REGS_GPR(2)(%r3)
        std    %r9, REGS_GPR(3)(%r3)

        ld     %r6, REGS_GPR(4)(%r12)
        ld     %r7, REGS_GPR(5)(%r12)
        ld     %r8, REGS_GPR(6)(%r12)
        ld     %r9, REGS_GPR(7)(%r12)
        std    %r6, REGS_GPR(4)(%r3)
        std    %r7, REGS_GPR(5)(%r3)
        std    %r8, REGS_GPR(6)(%r3)
        std    %r9, REGS_GPR(7)(%r3)

        ld     %r6, REGS_GPR(8)(%r12)
        ld     %r7, REGS_GPR(9)(%r12)
        ld     %r8, REGS_GPR(10)(%r12)
        ld     %r9, REGS_GPR(11)(%r12)
        std    %r6, REGS_GPR(8)(%r3)
        std    %r7, REGS_GPR(9)(%r3)
        std    %r8, REGS_GPR(10)(%r3)
        std    %r9, REGS_GPR(11)(%r3)
        
        ld     %r6, REGS_GPR(12)(%r12)
        ld     %r7, REGS_GPR(13)(%r12)
        ld     %r8, REGS_ip(%r12)
        ld     %r9, REGS_msr(%r12)
        std    %r6, REGS_GPR(12)(%r3)
        std    %r7, REGS_GPR(13)(%r3)
        std    %r8, REGS_ip(%r3)
        std    %r9, REGS_msr(%r3)

        ld     %r6, REGS_cr(%r12)
        ld     %r7, REGS_lr(%r12)
        ld     %r8, REGS_xer(%r12)
        ld     %r9, REGS_ctr(%r12)
        std    %r6, REGS_cr(%r3)
        std    %r7, REGS_lr(%r3)
        std    %r8, REGS_xer(%r3)
        std    %r9, REGS_ctr(%r3)
        // end copy
        
        // copy QPX state
	bl	LC_Save_QPX	// kills r0, r11, r12 and f0
        
        // Save SPRGs (?)
	bl     LC_Save_SPRs	// kills r0
        
        bl Rollback_Barrier

	mfspr	%r11,SPRG_pHWThread	// retrieve HWThread and
					//     KThread pointers
	ld	%r12,HWTHR_pCurrentThread(%r11)
	b	LC_Interrupt_Return
	nop
        _EPILOG(LC_Interrupt_Snapshot)


	_PROLOG_VECTORS(LC_RestoreFromRollback)
LC_RestoreFromRollback:
	//
	// On entry:
	//	r3:  Current Rollback save region
	//
        // Save non-volatile GPRs to rollback state


        // copy QPX state
        mfmsr    %r12
        ori      %r12,%r12,MSR_FP
        mtmsr    %r12
        isync
        
        addi     %r11, %r3, REGS_qvr
        li       %r12, 32
        
        qvlfdx  %f0,    0, %r11
        qvlfdux %f1, %r11, %r12
        qvlfdux %f2, %r11, %r12
        qvlfdux %f3, %r11, %r12
        qvlfdux %f4, %r11, %r12
        qvlfdux %f5, %r11, %r12
        qvlfdux %f6, %r11, %r12
        qvlfdux %f7, %r11, %r12
        qvlfdux %f8, %r11, %r12
        qvlfdux %f9, %r11, %r12
        qvlfdux %f10, %r11, %r12
        qvlfdux %f11, %r11, %r12
        qvlfdux %f12, %r11, %r12
        qvlfdux %f13, %r11, %r12
        qvlfdux %f14, %r11, %r12
        qvlfdux %f15, %r11, %r12
        qvlfdux %f16, %r11, %r12
        qvlfdux %f17, %r11, %r12
        qvlfdux %f18, %r11, %r12
        qvlfdux %f19, %r11, %r12
        qvlfdux %f20, %r11, %r12
        qvlfdux %f21, %r11, %r12
        qvlfdux %f22, %r11, %r12
        qvlfdux %f23, %r11, %r12
        qvlfdux %f24, %r11, %r12
        qvlfdux %f25, %r11, %r12
        qvlfdux %f26, %r11, %r12
        qvlfdux %f27, %r11, %r12
        qvlfdux %f28, %r11, %r12
        qvlfdux %f29, %r11, %r12
        qvlfdux %f30, %r11, %r12
        qvlfdux %f31, %r11, %r12
        
        lfd      %f0, REGS_fpscr(%r3)
	mtfsf    0,%f0,1,0
        
        ld     %r4,  REGS_ip(%r3)
        ld     %r5,  REGS_msr(%r3)
        ld     %r6,  REGS_cr(%r3)
        ld     %r7,  REGS_lr(%r3)
        ld     %r8,  REGS_xer(%r3)
        ld     %r9,  REGS_ctr(%r3)
        ld     %r10, REGS_pid(%r3)
        mtsrr0 %r4        
        mtsrr1 %r5
        mtcr   %r6
        mtlr   %r7        
        mtxer  %r8
        mtctr  %r9
        mtspr  SPRN_PID,%r10
        

        // Load SPRGs
        ld   %r4, REGS_SPRG(0)(%r5)
        ld   %r5, REGS_SPRG(1)(%r5)
        ld   %r6, REGS_SPRG(2)(%r5)
        ld   %r7, REGS_SPRG(3)(%r5)
        ld   %r8, REGS_SPRG(4)(%r5)
        ld   %r9, REGS_SPRG(5)(%r5)
        ld   %r10, REGS_SPRG(6)(%r5)
        ld   %r11, REGS_SPRG(7)(%r5)
        ld   %r12, REGS_SPRG(8)(%r5)        
        ld   %r13, REGS_GSPRG(0)(%r5)
        ld   %r14, REGS_GSPRG(1)(%r5)
        ld   %r15, REGS_GSPRG(2)(%r5)
        ld   %r16, REGS_GSPRG(3)(%r5)

        mtspr SPRN_SPRG0, %r4
        mtspr SPRN_SPRG1, %r5
        mtspr SPRN_SPRG2, %r6
        mtspr SPRN_SPRG3, %r7
        mtspr SPRN_SPRG4, %r8
        mtspr SPRN_SPRG5, %r9
        mtspr SPRN_SPRG6, %r10
        mtspr SPRN_SPRG7, %r11
        mtspr SPRN_SPRG8, %r12
        mtspr SPRN_GSPRG0, %r13
        mtspr SPRN_GSPRG1, %r14
        mtspr SPRN_GSPRG2, %r15
        mtspr SPRN_GSPRG3, %r16

        ld    %r0,REGS_GPR(0)(%r3)
        ld    %r1,REGS_GPR(1)(%r3)
        ld    %r2,REGS_GPR(2)(%r3)

        ld    %r4,REGS_GPR(4)(%r3)
        ld    %r5,REGS_GPR(5)(%r3)
        ld    %r6,REGS_GPR(6)(%r3)
        ld    %r7,REGS_GPR(7)(%r3)
        ld    %r8,REGS_GPR(8)(%r3)
        ld    %r9,REGS_GPR(9)(%r3)
        ld    %r10,REGS_GPR(10)(%r3)
        ld    %r11,REGS_GPR(11)(%r3)
        ld    %r12,REGS_GPR(12)(%r3)
        ld    %r13,REGS_GPR(13)(%r3)

	bl    LC_Restore_Nonvolatile_GPRs

        ld    %r3,REGS_GPR(3)(%r3)
        
        blr
        
	nop
        _EPILOG(LC_RestoreFromRollback)


	_PROLOG_VECTORS(LC_Interrupt_Migrate)
LC_Interrupt_Migrate:
	//
	// On entry:
	//	r0:  Pending bit vector
	//	r1:  Kernel stack
	//	r2:  Kernel TOC
	//	r11: Current HWThread pointer
	//	r12: Current KThread pointer
	//
	//	Volatile fixed-point registers have been saved in KThread.
	//	Non-volatile fixed-point registers and all QPX registers
	//		have their original values.
	//
	xori	%r0,%r0,KTHR_PENDING_MIGRATE
	std	%r0,KTHR_Pending(%r12)
	mr	%r3,%r12	// arg to Save_Nonvolatiles, Save_QPX, and
				// MigrateSelf
	bl	LC_Save_Nonvolatile_GPRs
	bl	LC_Save_QPX	// kills r0, r11, r12 and f0
	bl	KThread_MigrateSelf
	// NORETURN
	trap
	nop
	_EPILOG(LC_Interrupt_Migrate)

	_PROLOG_VECTORS(LC_Interrupt_Yield)
LC_Interrupt_Yield:
	//
	// On entry:
	//	r0:  Pending bit vector
	//	r1:  Kernel stack
	//	r2:  Kernel TOC
	//	r11: Current HWThread pointer
	//	r12: Current KThread pointer
	//
	//	Volatile fixed-point registers have been saved in KThread.
	//	Non-volatile fixed-point registers and all QPX registers
	//		have their original values.
	//
	xori	%r0,%r0,KTHR_PENDING_YIELD
	std	%r0,KTHR_Pending(%r12)
	mr	%r3,%r12	// arg to Save_Nonvolatiles and Save_QPX
	bl	LC_Save_Nonvolatile_GPRs
	bl	LC_Save_QPX	// kills r0, r11, r12 and f0
	bl	Scheduler
	// NORETURN
	trap
	nop
	_EPILOG(LC_Interrupt_Yield)

#define UCTX_GPR(n) (UCTX_gp_regs + (n)*8)
#define UCTX_FPR(n) (UCTX_fp_regs + (n)*8)

	_PROLOG_VECTORS(LC_Interrupt_Signal)
LC_Interrupt_Signal:
	//
	// On entry:
	//	r0:  Pending bit vector
	//	r1:  Kernel stack
	//	r2:  Kernel TOC
	//	r11: Current HWThread pointer
	//	r12: Current KThread pointer
	//
	//	Volatile fixed-point registers have been saved in KThread.
	//	Non-volatile fixed-point registers and all QPX registers
	//		have their original values.
	//
	xori	%r0,%r0,KTHR_PENDING_SIGNAL
	std	%r0,KTHR_Pending(%r12)
	mr	%r3,%r12	// kthread arg to PushSignal
	bl	PushSignal	// call ucontext_t *PushSignal(Kthread *)
	// PushSignal returns NULL if there was in fact no signal to deliver.
	cmpldi	%r3,0
	beq-	1f
	//
	// At this point, a signal frame has been pushed on the user stack,
	// the kthread's volatile state has been copied to the signal frame,
	// and the volatile state has been updated.  A (validated) pointer to
	// the ucontext_t in the signal frame has been returned in r3.  We
	// need to save the non-volatile state and QPX state into the
	// ucontext_t and then resume the kthread.
	std	%r14,UCTX_GPR(14)(%r3)
	std	%r15,UCTX_GPR(15)(%r3)
	std	%r16,UCTX_GPR(16)(%r3)
	std	%r17,UCTX_GPR(17)(%r3)
	std	%r18,UCTX_GPR(18)(%r3)
	std	%r19,UCTX_GPR(19)(%r3)
	std	%r20,UCTX_GPR(20)(%r3)
	std	%r21,UCTX_GPR(21)(%r3)
	std	%r22,UCTX_GPR(22)(%r3)
	std	%r23,UCTX_GPR(23)(%r3)
	std	%r24,UCTX_GPR(24)(%r3)
	std	%r25,UCTX_GPR(25)(%r3)
	std	%r26,UCTX_GPR(26)(%r3)
	std	%r27,UCTX_GPR(27)(%r3)
	std	%r28,UCTX_GPR(28)(%r3)
	std	%r29,UCTX_GPR(29)(%r3)
	std	%r30,UCTX_GPR(30)(%r3)
	std	%r31,UCTX_GPR(31)(%r3)
	// Save QPX state in ucontext.  The primary slot of each vector goes
	// into the fp_regs area.  The three secondary slots go into the
	// vmx_reserve area, properly aligned.  PushSignal guarantees that
	// fp_regs is 32-byte aligned.  We do our own alignment of vmx_reserve
	// and set the v_regs field accordingly.
	mfmsr	%r12			// enable fpu
	ori	%r0,%r12,MSR_FP
	mtmsr	%r0
	la	%r4,UCTX_fp_regs(%r3)	// r4 = moving pointer into fp_regs
	// r5 = &vmx_reserve aligned up to 32-byte boundary
	la	%r5,(UCTX_vmx_reserve+31)(%r3)
	rldicr	%r5,%r5,0,58		// r5 = moving pointer into vmx_reserve
	std	%r5,UCTX_v_regs(%r3)	// set v_regs to pointer
	li	%r0,32			// increment for qvstfdux
	subi	%r5,%r5,32		// prepare v_regs pointer for qvstfdux
	// Shuffle and save the first block of 4 vectors.
	// Let "ABCD EFGH IJKL MNOP" represent the 16 values in the 4 vectors,
	// with x denoting a value that is no longer interesting.
					//  f0   f1   f2   f3
					// ABCD EFGH IJKL MNOP
	stfd	%f0,(0*8)(%r4)		// xBCD EFGH IJKL MNOP
	stfd	%f1,(1*8)(%r4)		// xBCD xFGH IJKL MNOP
	stfd	%f2,(2*8)(%r4)		// xBCD xFGH xJKL MNOP
	stfd	%f3,(3*8)(%r4)		// xBCD xFGH xJKL xNOP
	qvaligni %f1,%f1,%f1,1		// xBCD FGHx xJKL xNOP
	qvaligni %f0,%f0,%f1,1		// BCDF xGHx xJKL xNOP
	qvstfdux %f0,%r5,%r0		// xxxx xGHx xJKL xNOP
	qvgpci	 %f0,01256		// 1256 xGHx xJKL xNOP
	qvfperm	 %f0,%f1,%f2,%f0	// GHJK xxxx xxxL xNOP
	qvstfdux %f0,%r5,%r0		// xxxx xxxx xxxL xNOP
	qvgpci	 %f0,03567		// 3567 xxxx xxxL xNOP
	qvfperm	 %f0,%f2,%f3,%f0	// LNOP xxxx xxxx xxxx
	qvstfdux %f0,%r5,%r0		// xxxx xxxx xxxx xxxx
	// Define permutations for coalescing primary slots.
	qvgpci	 %f0,00400		// f0 = 04xx
	qvgpci	 %f1,00140		// f1 = 014x
	qvgpci	 %f2,00124		// f2 = 0124
	// Shuffle and save the second block of 4 vectors.
					//  f3   -   f4   f5   f6   f7
					// xxxx  -  ABCD EFGH IJKL MNOP
	qvfperm	 %f3,%f4,%f5,%f0	// AExx  -  xBCD xFGH IJKL MNOP
	qvfperm	 %f3,%f3,%f6,%f1	// AEIx  -  xBCD xFGH xJKL MNOP
	qvfperm	 %f3,%f3,%f7,%f2	// AEIM  -  xBCD xFGH xJKL xNOP
	qvstfdux %f3,%r4,%r0		// xxxx  -  xBCD xFGH xJKL xNOP
	qvgpci	 %f3,01235		// 1235  -  xBCD xFGH xJKL xNOP
	qvfperm	 %f4,%f4,%f5,%f3	// 1235  -  BCDF xxGH xJKL xNOP
	qvstfdux %f4,%r5,%r0		// 1235  -  xxxx xxGH xJKL xNOP
	qvgpci	 %f4,02356		// 1235  -  2356 xxGH xJKL xNOP
	qvfperm	 %f5,%f5,%f6,%f4	// 1235  -  2356 GHJK xxxL xNOP
	qvstfdux %f5,%r5,%r0		// 1235  -  2356 xxxx xxxL xNOP
	qvgpci	 %f5,03567		// 1235  -  2356 3567 xxxL xNOP
	qvfperm	 %f6,%f6,%f7,%f5	// 1235  -  2356 3567 LNOP xxxx
	qvstfdux %f6,%r5,%r0		// 1235  -  2356 3567 xxxx xxxx
	// At this point, f0-f2 contain permutations for condensing primary
	// slots, f3-f5 contain permutations for condensing secondary slots,
	// and f6-f7 are free.  We can now shuffle and save the remaining
	// six blocks of 4 vectors.
#define QPX_SAVE_BLOCK(a,b,c,d)	/*  f6   f7  -  a    b    c    d   */; \
				/* xxxx xxxx - ABCD EFGH IJKL MNOP */; \
	qvfperm	 %f7,a,b,%f3	/* xxxx BCDF - Axxx ExGH IJKL MNOP */; \
	qvfperm	 %f6,a,b,%f0	/* AExx BCDF - xxxx xxGH IJKL MNOP */; \
	qvstfdux %f7,%r5,%r0	/* AExx xxxx - xxxx xxGH IJKL MNOP */; \
	qvfperm	 %f7,b,c,%f4	/* AExx GHJK - xxxx xxxx IxxL MNOP */; \
	qvfperm	 %f6,%f6,c,%f1	/* AEIx GHJK - xxxx xxxx xxxL MNOP */; \
	qvstfdux %f7,%r5,%r0	/* AEIx xxxx - xxxx xxxx xxxL MNOP */; \
	qvfperm	 %f7,c,d,%f5	/* AEIx LNOP - xxxx xxxx xxxx Mxxx */; \
	qvfperm	 %f6,%f6,d,%f2	/* AEIM LNOP - xxxx xxxx xxxx xxxx */; \
	qvstfdux %f7,%r5,%r0	/* AEIM xxxx - xxxx xxxx xxxx xxxx */; \
	qvstfdux %f6,%r4,%r0	/* xxxx xxxx - xxxx xxxx xxxx xxxx */
	QPX_SAVE_BLOCK(%f8,%f9,%f10,%f11)
	QPX_SAVE_BLOCK(%f12,%f13,%f14,%f15)
	QPX_SAVE_BLOCK(%f16,%f17,%f18,%f19)
	QPX_SAVE_BLOCK(%f20,%f21,%f22,%f23)
	QPX_SAVE_BLOCK(%f24,%f25,%f26,%f27)
	QPX_SAVE_BLOCK(%f28,%f29,%f30,%f31)
#undef QPX_SAVE_BLOCK
	mffs	%f0
	stfd	%f0,UCTX_FPR(32)(%r3)	// fpscr is saved as "33rd" fpr
	fsub	%f0,%f0,%f0		// clear fpscr for the signal handler
	mtfsf	0,%f0,1,0
	mtmsr	%r12			// restore msr
    1:
	mfspr	%r11,SPRG_pHWThread	// retrieve HWThread and
					//     KThread pointers
	ld	%r12,HWTHR_pCurrentThread(%r11)
	b	LC_Interrupt_Return
	nop
	_EPILOG(LC_Interrupt_Signal)

//------------------------------------------------------------------------------
//
//  System Call Vector and save/restore code.
//
#define _SYSC(label) \
	.align 5; \
	.global label; \
label: \
	mfspr	%r9,SPRG_pHWThread; \
	ld	%r10,HWTHR_pCurrentThread(%r9); \
	mfspr	%r11,SPRN_SRR0_IP; \
	mfspr	%r12,SPRN_SRR1_MSR; \
	b	LC_Syscall_Save
//
        _PROLOG_VECTORS(LC_Syscall_Save)
LC_Syscall_Save:
	std	%r11,REGS_ip(%r10)
	std	%r12,REGS_msr(%r10)
	std	%r1,REGS_GPR(1)(%r10)
	std	%r2,REGS_GPR(2)(%r10)
	std	%r13,REGS_GPR(13)(%r10)
	mfcr	%r11
	mflr	%r12
	std	%r1,(HWTHR_STDSTACK_BOT-STACK_FRAME_SIZE)(%r9)	// backchain
	la	%r1,(HWTHR_STDSTACK_BOT-STACK_FRAME_SIZE)(%r9)	// kernel stack
        IMM32(%r2, __CNK_TOC_BASE)	// kernel toc
	std	%r11,REGS_cr(%r10)
	std	%r12,REGS_lr(%r10)
	mfxer	%r11
	mfctr	%r12
	std	%r11,REGS_xer(%r10)
	std	%r12,REGS_ctr(%r10)
	mr	%r9,%r0			// syscall number
	bl	IntHandler_Syscall	// r3 = IntHandler_Syscall(r3, ..., r8,
					//                         sc_num,
					//                         kthread);
	mfspr	%r11,SPRG_pHWThread	// retrieve HWThread and
					//     KThread pointers
	ld	%r12,HWTHR_pCurrentThread(%r11)
	//
	// IntHandler_Syscall indicates failure by returning a value between
	// -(CNK_ERRNO_RANGE-1) and -1.  A value in this range is converted to
	// an errno (by adding CNK_ERRNO_RANGE) and returned in r3, along with
	// a failure indication set in the summary overflow (SO) bit of cr0.
	// All other values are returned unchanged in r3, with the cr0 field
	// cleared to indicate success.
	//
	ld	%r4,REGS_cr(%r12)	// retrieve original cr
	addi	%r0,%r3,CNK_ERRNO_RANGE	// tentatively convert rc to an errno
	cmpldi	%r0,CNK_ERRNO_RANGE	// check if it's in the errno range
	rldicl	%r4,%r4,0,36		// clear cr0 field of cr to be restored
	bge+	LC_Syscall_Return	// successful return, r3 unchanged
	mr	%r3,%r0			// r3 is errno
	oris	%r4,%r4,0x1000		// set cr0 SO bit to indicate failure
	b	LC_Syscall_Return
	nop
        _EPILOG(LC_Syscall_Save)

        _PROLOG_VECTORS(LC_Syscall_Return)
LC_Syscall_Return:
	//
	// On entry:
	//	r1:  Kernel stack
	//	r2:  Kernel TOC
	//	r3:  Syscall return value
	//	r4:  CR value to be restored
	//	r11: Current HWThread pointer
	//	r12: Current KThread pointer
	//
	//	Critical volatile fixed-point registers (ip, msr, lr, xer, ctr,
	//		r1, r2, and r13) have been saved in KThread.
	//	Non-volatile fixed-point registers and all QPX registers
	//		have their original values.
	//
	ld	%r0,KTHR_Pending(%r12)
	cmpldi	%r0,0
	bne-	LC_Syscall_Handle_Pending
	ld	%r5,REGS_ip(%r12)
	ld	%r6,REGS_msr(%r12)
	ld	%r7,REGS_lr(%r12)
	ld	%r8,REGS_xer(%r12)
	ld	%r9,REGS_ctr(%r12)
        ld	%r1,REGS_GPR(1)(%r12)
        ld	%r2,REGS_GPR(2)(%r12)
        ld	%r13,REGS_GPR(13)(%r12)
	mtcr	%r4
	mtspr	SPRN_SRR0_IP,%r5
	mtspr	SPRN_SRR1_MSR,%r6
	mtlr	%r7
	mtxer	%r8
	mtctr	%r9
	// for determinism, clear volatile registers that aren't preserved
	li	%r0,0
	li	%r4,0
	li	%r5,0
	li	%r6,0
	li	%r7,0
	li	%r8,0
	li	%r9,0
	li	%r10,0
	li	%r11,0
	li	%r12,0
	rfi
	nop
        _EPILOG(LC_Syscall_Return)

	_PROLOG_VECTORS(LC_Syscall_Handle_Pending)
LC_Syscall_Handle_Pending:
	//
	// On entry:
	//	r0:  Pending bit vector
	//	r1:  Kernel stack
	//	r2:  Kernel TOC
	//	r3:  Syscall return value
	//	r4:  CR value to be restored
	//	r11: Current HWThread pointer
	//	r12: Current KThread pointer
	//
	//	Critical volatile fixed-point registers (ip, msr, lr, xer, ctr,
	//		r1, r2, and r13) have been saved in KThread.
	//	Non-volatile fixed-point registers and all QPX registers
	//		have their original values.
	//
	std	%r3,REGS_GPR(3)(%r12)	// preserve syscall return value
	std	%r4,REGS_cr(%r12)	// preserve cr
	andi.	%r10,%r0,KTHR_PENDING_CLONE
	bne-	LC_Syscall_Clone
	andi.	%r10,%r0,KTHR_PENDING_SIGRETURN
	bne-	LC_Syscall_Sigreturn
	andi.	%r10,%r0,KTHR_PENDING_MIGRATE
	bne-	LC_Syscall_Migrate
	andi.	%r10,%r0,KTHR_PENDING_YIELD
	bne-	LC_Syscall_Yield
	andi.	%r10,%r0,KTHR_PENDING_SIGNAL
	bne-	LC_Syscall_Signal
	trap
	nop
	_EPILOG(LC_Syscall_Handle_Pending)

	_PROLOG_VECTORS(LC_Syscall_Clone)
LC_Syscall_Clone:
	//
	// On entry:
	//	r0:  Pending bit vector
	//	r1:  Kernel stack
	//	r2:  Kernel TOC
	//	r11: Current HWThread pointer
	//	r12: Current KThread pointer
	//
	//	Critical volatile fixed-point registers (ip, msr, lr, xer, ctr,
	//		cr, r1, r2, r3, and r13) have been saved in KThread.
	//	Non-volatile fixed-point registers and all QPX registers
	//		have their original values.
	//
	xori	%r0,%r0,KTHR_PENDING_CLONE
	std	%r0,KTHR_Pending(%r12)
	// The current kthread has created a new kthread and left its address
	// in PendingKThread.  We need to copy the non-volatile registers to
	// the new thread and then complete its launch.
	ld	%r3,KTHR_PendingKThread(%r12)
	li	%r0,0			// clear PendingKThread for good measure
	std	%r0,KTHR_PendingKThread(%r12)
	bl	LC_Save_Nonvolatile_GPRs
	bl	LC_Save_Nonvolatile_FPRs // kills r0, r12, and f0
	li	%r0,1			// set flag for ContextLaunch
	std	%r0,KTHR_SyscallReturn(%r3)
	// void KThread_CompleteAppSibling(KThread_t *)
	bl	KThread_CompleteAppSibling
	// Now continue with the current thread's syscall return.
	mfspr	%r11,SPRG_pHWThread	// retrieve HWThread and
					//     KThread pointers
	ld	%r12,HWTHR_pCurrentThread(%r11)
	// retrieve return value details for Syscall_Return
	ld	%r3,REGS_GPR(3)(%r12)
	ld	%r4,REGS_cr(%r12)
	b	LC_Syscall_Return
	nop
	_EPILOG(LC_Syscall_Clone)

	_PROLOG_VECTORS(LC_Syscall_Sigreturn)
LC_Syscall_Sigreturn:
	//
	// On entry:
	//	r0:  Pending bit vector
	//	r1:  Kernel stack
	//	r2:  Kernel TOC
	//	r11: Current HWThread pointer
	//	r12: Current KThread pointer
	//
	//	Critical volatile fixed-point registers (ip, msr, lr, xer, ctr,
	//		cr, r1, r2, r3, and r13) have been saved in KThread.
	//	Non-volatile fixed-point registers and all QPX registers
	//		have their original values.
	//
	xori	%r0,%r0,KTHR_PENDING_SIGRETURN
	std	%r0,KTHR_Pending(%r12)
	// Rather than return from this syscall, we pop out of the signal
	// context and restore the user state from the stack, continuing as
	// from an interrupt.
	mr	%r3,%r12	// kthread arg to PopSignal
	bl	PopSignal	// call ucontext_t *PopSignal(Kthread *)
	cmpldi	%r3,0
	bne+	1f
	// PopSignal returns NULL if the stacked ucontext is not accessible.
	// Retrieve HWThread and KThread pointers and return value details
	// for Syscall_Return.
	mfspr	%r11,SPRG_pHWThread
	ld	%r12,HWTHR_pCurrentThread(%r11)
	ld	%r3,REGS_GPR(3)(%r12)
	ld	%r4,REGS_cr(%r12)
	b	LC_Syscall_Return
    1:
	// At this point, the volatile state will have been copied to the
	// kthread structure, and a pointer to the ucontext structure will
	// have been returned in r3.  We need to restore the non-volatile
	// and QPX state.
	// Restore QPX state from ucontext.  The primary value for each vector
	// is in the fp_regs area.  The three secondary values are in the area
	// pointed to by v_regs, which must point into the vmx_reserve area.
	// PopSignal returns here only if both areas are 32-byte aligned.
	// Otherwise it copies the full state to the kthread and branches to
	// LC_ContextLaunch.
	// PopSignal validated v_regs, but we can't use it directly.  Some
	// other thread could have changed it.  We round &vmx_reserve up to a
	// 32-byte boundary instead.
	mfmsr	%r12			// enable fpu
	ori	%r0,%r12,MSR_FP
	mtmsr	%r0
	lfd	%f0,UCTX_FPR(32)(%r3)	// fpscr was saved as "33rd" fpr
	mtfsf	0,%f0,1,0
	la	%r4,UCTX_fp_regs(%r3)	// r4 = moving pointer into fp_regs
	la	%r5,(UCTX_vmx_reserve+31)(%r3)
	rldicr	%r5,%r5,0,58		// r5 = moving pointer into vmx_reserve
	li	%r0,32			// increment for qvlfdux
	subi	%r4,%r4,32		// prepare fp_regs pointer for qvlfdux
	subi	%r5,%r5,32		// prepare v_regs pointer for qvlfdux
	// Define permutations for reconstituting a block of 4 original
	// vectors from the shuffled form stored in the ucontext.
	qvgpci	%f28,00456	// f28 = 0456   extract ABCD from AEIM,BCDF
	qvgpci	%f29,01700	// f29 = 17xx   extract EFxx from AEIM,BCDF
	qvgpci	%f30,02670	// f30 = 267x   extract IJKx from AEIM,GHJK
	qvgpci	%f31,03567	// f31 = 3567   extract MNOP from AEIM,LNOP
	qvgpci	%f2,00145	// f2  = 0145   extract EFGH from EFxx,GHJK
	qvgpci	%f3,00124	// f3  = 0124   extract IJKL from IJKx,LNOP
	// Load and reconstitute a block of 4 vectors, using f0 as scratch.
#define QPX_RESTORE_BLOCK(a,b,c,d) \
				/*  a    b    c    d   -  f0  */; \
				/* xxxx xxxx xxxx xxxx - xxxx */; \
	qvlfdux	%f0,%r4,%r0	/* xxxx xxxx xxxx xxxx - AEIM */; \
	qvlfdux	b,%r5,%r0	/* xxxx BCDF xxxx xxxx - AEIM */; \
	qvlfdux	c,%r5,%r0	/* xxxx BCDF GHJK xxxx - AEIM */; \
	qvlfdux	d,%r5,%r0	/* xxxx BCDF GHJK LNOP - AEIM */; \
	qvfperm	a,%f0,b,%f28	/* ABCD xxxF GHJK LNOP - xEIM */; \
	qvfperm	b,%f0,b,%f29	/* ABCD EFxx GHJK LNOP - xxIM */; \
	qvfperm	b,b,c,%f2	/* ABCD EFGH xxJK LNOP - xxIM */; \
	qvfperm c,%f0,c,%f30	/* ABCD EFGH IJKx LNOP - xxxM */; \
	qvfperm c,c,d,%f3	/* ABCD EFGH IJKL xNOP - xxxM */; \
	qvfperm d,%f0,d,%f31	/* ABCD EFGH IJKL MNOP - xxxx */
	// Restore the first block, not where it belongs, but into f4-f7.
	// Then save it on the stack.  We're using ABI_STACK_TAIL space here.
	QPX_RESTORE_BLOCK(%f4,%f5,%f6,%f7)
	la	%r6,-(5*32)(%r1)	// *5*, for auto-update purposes
	rldicr	%r6,%r6,0,58		// ensure 32-byte alignment
	qvstfdux %f4,%r6,%r0
	qvstfdux %f5,%r6,%r0
	qvstfdux %f6,%r6,%r0
	qvstfdux %f7,%r6,%r0
	// Now restore the next 6 blocks in place.
	QPX_RESTORE_BLOCK(%f4,%f5,%f6,%f7)
	QPX_RESTORE_BLOCK(%f8,%f9,%f10,%f11)
	QPX_RESTORE_BLOCK(%f12,%f13,%f14,%f15)
	QPX_RESTORE_BLOCK(%f16,%f17,%f18,%f19)
	QPX_RESTORE_BLOCK(%f20,%f21,%f22,%f23)
	QPX_RESTORE_BLOCK(%f24,%f25,%f26,%f27)
#undef QPX_RESTORE_BLOCK
	// Restore the last block (f28-f31) in place -- very carefully,
	// because we're now overwriting some of the permutations.
					//  f28  f29  f30  f31 -  f0   f1
					// 0456 17xx 267x 3567 - xxxx xxxx
	qvlfdux	%f0,%r4,%r0		// 0456 17xx 267x 3567 - AEIM xxxx
	qvlfdux	%f1,%r5,%r0		// 0456 17xx 267x 3567 - AEIM BCDF
	qvfperm	%f28,%f0,%f1,%f28	// ABCD 17xx 267x 3567 - xEIM xxxF
	qvfperm	%f29,%f0,%f1,%f29	// ABCD EFxx 267x 3567 - xxIM xxxx
	qvlfdux	%f1,%r5,%r0		// ABCD EFxx 267x 3567 - xxIM GHJK
	qvfperm	%f29,%f29,%f1,%f2	// ABCD EFGH 267x 3567 - xxIM xxJK
	qvfperm %f30,%f0,%f1,%f30	// ABCD EFGH IJKx 3567 - xxxM xxxx
	qvlfdux	%f1,%r5,%r0		// ABCD EFGH IJKx 3567 - xxxM LNOP
	qvfperm %f30,%f30,%f1,%f3	// ABCD EFGH IJKL 3567 - xxxM xNOP
	qvfperm %f31,%f0,%f1,%f31	// ABCD EFGH IJKL MNOP - xxxx xxxx
	// Finally, reload the first block from the stack.
	la	%r6,-(4*32)(%r6)	// return pointer to where it started
	qvlfdux %f0,%r6,%r0
	qvlfdux %f1,%r6,%r0
	qvlfdux %f2,%r6,%r0
	qvlfdux %f3,%r6,%r0
	mtmsr	%r12			// restore msr
	// restore non-volatile GPRs from ucontext
	ld	%r14,UCTX_GPR(14)(%r3)
	ld	%r15,UCTX_GPR(15)(%r3)
	ld	%r16,UCTX_GPR(16)(%r3)
	ld	%r17,UCTX_GPR(17)(%r3)
	ld	%r18,UCTX_GPR(18)(%r3)
	ld	%r19,UCTX_GPR(19)(%r3)
	ld	%r20,UCTX_GPR(20)(%r3)
	ld	%r21,UCTX_GPR(21)(%r3)
	ld	%r22,UCTX_GPR(22)(%r3)
	ld	%r23,UCTX_GPR(23)(%r3)
	ld	%r24,UCTX_GPR(24)(%r3)
	ld	%r25,UCTX_GPR(25)(%r3)
	ld	%r26,UCTX_GPR(26)(%r3)
	ld	%r27,UCTX_GPR(27)(%r3)
	ld	%r28,UCTX_GPR(28)(%r3)
	ld	%r29,UCTX_GPR(29)(%r3)
	ld	%r30,UCTX_GPR(30)(%r3)
	ld	%r31,UCTX_GPR(31)(%r3)
	// continue as from an interrupt
	mfspr	%r11,SPRG_pHWThread	// retrieve HWThread and
					//     KThread pointers
	ld	%r12,HWTHR_pCurrentThread(%r11)
	b	LC_Interrupt_Return
	nop
	_EPILOG(LC_Syscall_Sigreturn)

	_PROLOG_VECTORS(LC_Syscall_Migrate)
LC_Syscall_Migrate:
	//
	// On entry:
	//	r0:  Pending bit vector
	//	r1:  Kernel stack
	//	r2:  Kernel TOC
	//	r11: Current HWThread pointer
	//	r12: Current KThread pointer
	//
	//	Critical volatile fixed-point registers (ip, msr, lr, xer, ctr,
	//		cr, r1, r2, r3, and r13) have been saved in KThread.
	//	Non-volatile fixed-point registers and all QPX registers
	//		have their original values.
	//
	xori	%r0,%r0,KTHR_PENDING_MIGRATE
	std	%r0,KTHR_Pending(%r12)
	li	%r0,1			// set flag for ContextLaunch
	std	%r0,KTHR_SyscallReturn(%r12)
	mr	%r3,%r12		// arg to Save_Nonvolatiles and
					// MigrateSelf
	bl	LC_Save_Nonvolatile_GPRs
	bl	LC_Save_Nonvolatile_FPRs // kills r0, r12, and f0
	bl	KThread_MigrateSelf
	// NORETURN
	trap
	nop
	_EPILOG(LC_Syscall_Migrate)

	_PROLOG_VECTORS(LC_Syscall_Yield)
LC_Syscall_Yield:
	//
	// On entry:
	//	r0:  Pending bit vector
	//	r1:  Kernel stack
	//	r2:  Kernel TOC
	//	r11: Current HWThread pointer
	//	r12: Current KThread pointer
	//
	//	Critical volatile fixed-point registers (ip, msr, lr, xer, ctr,
	//		cr, r1, r2, r3, and r13) have been saved in KThread.
	//	Non-volatile fixed-point registers and all QPX registers
	//		have their original values.
	//
	xori	%r0,%r0,KTHR_PENDING_YIELD
	std	%r0,KTHR_Pending(%r12)
	li	%r0,1			// set flag for ContextLaunch
	std	%r0,KTHR_SyscallReturn(%r12)
	mr	%r3,%r12		// arg to Save_Nonvolatiles
	bl	LC_Save_Nonvolatile_GPRs
	bl	LC_Save_Nonvolatile_FPRs // kills r0, r12, and f0
	bl	Scheduler
	// NORETURN
	trap
	nop
	_EPILOG(LC_Syscall_Yield)

	_PROLOG_VECTORS(LC_Syscall_Signal)
LC_Syscall_Signal:
	//
	// On entry:
	//	r0:  Pending bit vector
	//	r1:  Kernel stack
	//	r2:  Kernel TOC
	//	r11: Current HWThread pointer
	//	r12: Current KThread pointer
	//
	//	Critical volatile fixed-point registers (ip, msr, lr, xer, ctr,
	//		cr, r1, r2, r3, and r13) have been saved in KThread.
	//	Non-volatile fixed-point registers and all QPX registers
	//		have their original values.
	//
	// We need to convert the syscall return to an interrupt return, making
	// it appear as if an interrupt carrying the signal arrived just after
	// the syscall returned.
	//
	// for determinism, clear volatile registers that weren't preserved
	li	%r3,0
	std	%r3,REGS_GPR(0)(%r12)
	std	%r3,REGS_GPR(4)(%r12)
	std	%r3,REGS_GPR(5)(%r12)
	std	%r3,REGS_GPR(6)(%r12)
	std	%r3,REGS_GPR(7)(%r12)
	std	%r3,REGS_GPR(8)(%r12)
	std	%r3,REGS_GPR(9)(%r12)
	std	%r3,REGS_GPR(10)(%r12)
	std	%r3,REGS_GPR(11)(%r12)
	std	%r3,REGS_GPR(12)(%r12)
	b	LC_Interrupt_Signal	// continue as an interrupt return
	nop
	_EPILOG(LC_Syscall_Signal)

//------------------------------------------------------------------------------
// LC_ContextLaunch:  load up a Regs_t structure and RFI.
//
//   syntax: LC_ContextLaunch(Regs_t *pRegs);
//
        _PROLOG(LC_ContextLaunch)
LC_ContextLaunch:
	mfmsr	%r10
	ori	%r0,%r10,MSR_FP
	mtmsr	%r0
	lfd	%f0,REGS_fpscr(%r3)
	ld	%r4,REGS_SPRG(7)(%r3)
	ld	%r5,REGS_SPRG(8)(%r3)
	ld	%r0,KTHR_SyscallReturn(%r3)	// early load
	bl	LC_Restore_Nonvolatile_GPRs
	cmpldi	%r0,0
	mtfsf	0,%f0,1,0
	//mtspr	SPRN_SPRG7,%r4  This REG is being managed live. Do not restore
	mtspr	SPRN_SPRG8,%r5
	beq-	LC_Launch_From_Interrupt
    LC_Launch_From_Syscall:
	li	%r0,0
	std	%r0,KTHR_SyscallReturn(%r3)
	lfd	%f0,KTHR_SyscallReturn(%r3)	// grab a convenient zero
	// only the primary slots of the non-volatile registers were saved
	lfd	%f14,REGS_NV_FPR(14)(%r3)
	lfd	%f15,REGS_NV_FPR(15)(%r3)
	lfd	%f16,REGS_NV_FPR(16)(%r3)
	lfd	%f17,REGS_NV_FPR(17)(%r3)
	lfd	%f18,REGS_NV_FPR(18)(%r3)
	lfd	%f19,REGS_NV_FPR(19)(%r3)
	lfd	%f20,REGS_NV_FPR(20)(%r3)
	lfd	%f21,REGS_NV_FPR(21)(%r3)
	lfd	%f22,REGS_NV_FPR(22)(%r3)
	lfd	%f23,REGS_NV_FPR(23)(%r3)
	lfd	%f24,REGS_NV_FPR(24)(%r3)
	lfd	%f25,REGS_NV_FPR(25)(%r3)
	lfd	%f26,REGS_NV_FPR(26)(%r3)
	lfd	%f27,REGS_NV_FPR(27)(%r3)
	lfd	%f28,REGS_NV_FPR(28)(%r3)
	lfd	%f29,REGS_NV_FPR(29)(%r3)
	lfd	%f30,REGS_NV_FPR(30)(%r3)
	lfd	%f31,REGS_NV_FPR(31)(%r3)
	// for determinism, clear volatile registers that weren't preserved
	qvfmr	%f1,%f0
	qvfmr	%f2,%f0
	qvfmr	%f3,%f0
	qvfmr	%f4,%f0
	qvfmr	%f5,%f0
	qvfmr	%f6,%f0
	qvfmr	%f7,%f0
	qvfmr	%f8,%f0
	qvfmr	%f9,%f0
	qvfmr	%f10,%f0
	qvfmr	%f11,%f0
	qvfmr	%f12,%f0
	qvfmr	%f13,%f0
	mtmsr	%r10	// re-disable the FPU
	// retrieve HWThread ptr and return value details for Syscall_Return
	mr	%r12,%r3
	mfspr	%r11,SPRG_pHWThread
	ld	%r3,REGS_GPR(3)(%r12)
	ld	%r4,REGS_cr(%r12)
	b	LC_Syscall_Return
    LC_Launch_From_Interrupt:
	la	%r11,(REGS_qvr-32)(%r3)
	li	%r0,32
	qvlfdux	%f0,%r11,%r0
	qvlfdux	%f1,%r11,%r0
	qvlfdux	%f2,%r11,%r0
	qvlfdux	%f3,%r11,%r0
	qvlfdux	%f4,%r11,%r0
	qvlfdux	%f5,%r11,%r0
	qvlfdux	%f6,%r11,%r0
	qvlfdux	%f7,%r11,%r0
	qvlfdux	%f8,%r11,%r0
	qvlfdux	%f9,%r11,%r0
	qvlfdux	%f10,%r11,%r0
	qvlfdux	%f11,%r11,%r0
	qvlfdux	%f12,%r11,%r0
	qvlfdux	%f13,%r11,%r0
	qvlfdux	%f14,%r11,%r0
	qvlfdux	%f15,%r11,%r0
	qvlfdux	%f16,%r11,%r0
	qvlfdux	%f17,%r11,%r0
	qvlfdux	%f18,%r11,%r0
	qvlfdux	%f19,%r11,%r0
	qvlfdux	%f20,%r11,%r0
	qvlfdux	%f21,%r11,%r0
	qvlfdux	%f22,%r11,%r0
	qvlfdux	%f23,%r11,%r0
	qvlfdux	%f24,%r11,%r0
	qvlfdux	%f25,%r11,%r0
	qvlfdux	%f26,%r11,%r0
	qvlfdux	%f27,%r11,%r0
	qvlfdux	%f28,%r11,%r0
	qvlfdux	%f29,%r11,%r0
	qvlfdux	%f30,%r11,%r0
	qvlfdux	%f31,%r11,%r0
	mtmsr	%r10	// re-disable the FPU
	mr	%r12,%r3
	mfspr	%r11,SPRG_pHWThread	// retrieve HWThread pointer
	b	LC_Interrupt_Return
	nop
        _EPILOG(LC_ContextLaunch)

//------------------------------------------------------------------------------
//
//  Exception Vectors
//
        .extern IntHandler_Critical     // see flih.c
        .extern IntHandler_Standard     // see flih.c
        .extern IntHandler_Syscall      // see syscalls.c
        .extern IntHandler_Debug        // see debug.c
        .extern IntHandler_DEC          // see timers.c
        .extern IntHandler_FIT          // see timers.c
        .extern IntHandler_WDT          // see timers.c
        .extern IntHandler_UDEC         // see timers.c
        .extern IntHandler_PERFMON      // see flih.c

        .align  12                      // Core architecture requires that
					// this virtual address be 4K aligned.
        _PROLOG_VECTORS(LC_Exception_Code)
LC_Exception_Code:
        _MCHK(Vector_MCHK)                          // Firmware handles mchk's.
        _CVEC(Vector_CI,       IntHandler_Critical) // Critical. Async.
        _CDBG(Vector_DEBUG,    DEBUG_CODE_DEBUG)    // Critical. Sync or Async.
						    //   Precise or Imprecise.
        _SDBG(Vector_DSI,      DEBUG_CODE_DSI)      // Standard, Sync, Precise.
        _SDBG(Vector_ISI,      DEBUG_CODE_ISI)      // Standard, Sync, Precise.
        _SVEC(Vector_EI,       IntHandler_Standard) // Standard, Sync, Precise.
        _SDBG(Vector_ALGN,     DEBUG_CODE_ALGN)     // Standard, Sync, Precise.
        _SDBG(Vector_PROG,     DEBUG_CODE_PROG)     // Standard, Sync.
						    //   Precise or Imprecise.
        _SDBG(Vector_FPU,      DEBUG_CODE_FPU)      // Standard, Sync, Precise.
        _SYSC(Vector_SC)                            // Standard, Sync, Precise.
						    //   (IntHandler_Syscall)
        _SDBG(Vector_APU,      DEBUG_CODE_APU)      // Standard, Sync, Precise.
        _SVEC(Vector_DEC,      IntHandler_DEC)      // Standard, Async.
        _SVEC(Vector_FIT,      IntHandler_FIT)      // Standard, Async.
        _CVEC(Vector_WDT,      IntHandler_WDT)      // Critical, Async.
        _SDBG(Vector_DTLB,     DEBUG_CODE_DTLB)     // Standard, Sync, Precise.
        _SDBG(Vector_ITLB,     DEBUG_CODE_ITLB)     // Standard, Sync, Precise.
        _SDBG(Vector_VECTOR,   DEBUG_CODE_VECT)     //
        _SDBG(Vector_Undef220, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef240, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef260, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_PDBI,     DEBUG_CODE_PDBI)     // Standard, Async.
        _CDBG(Vector_PDBCI,    DEBUG_CODE_PDBCI)    // Critical, Async.
        _SDBG(Vector_GDBI,     DEBUG_CODE_GDBI)     // Standard, Async.
        _CDBG(Vector_GDBCI,    DEBUG_CODE_GDBCI)    // Critical, Async.
        _SDBG(Vector_EHVSC,    DEBUG_CODE_EHVSC)    // Standard, Sync, Precise.
        _SDBG(Vector_EHVPRIV,  DEBUG_CODE_EHVPRIV)  // Standard, Sync, Precise.
        _SDBG(Vector_LRATE,    DEBUG_CODE_LRATE)    // Standard, Sync, Precise.
        _SDBG(Vector_Undef360, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef380, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef3a0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef3c0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef3e0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef400, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef420, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef440, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef460, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef480, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef4a0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef4c0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef4e0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef500, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef520, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef540, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef560, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef580, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef5a0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef5c0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef5e0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef600, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef620, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef640, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef660, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef680, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef6a0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef6c0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef6e0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef700, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef720, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef740, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef760, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef780, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef7a0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef7c0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef7e0, DEBUG_CODE_UNDEF)    //
        _SVEC(Vector_UDEC,     IntHandler_UDEC)     // Standard, Async.
        _SVEC(Vector_PERFMON,  IntHandler_PERFMON)  // Standard, Async.
        _SDBG(Vector_Undef840, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef860, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef880, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef8a0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef8c0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef8e0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef900, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef920, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef940, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef960, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef980, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef9a0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef9c0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undef9e0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefa00, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefa20, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefa40, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefa60, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefa80, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefaa0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefac0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefae0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefb00, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefb20, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefb40, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefb60, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefb80, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefba0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefbc0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefbe0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefc00, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefc20, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefc40, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefc60, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefc80, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefca0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefcc0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefce0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefd00, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefd20, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefd40, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefd60, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefd80, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefda0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefdc0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefde0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefe00, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefe20, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefe40, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefe60, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefe80, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefea0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefec0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undefee0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undeff00, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undeff20, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undeff40, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undeff60, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undeff80, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undeffa0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undeffc0, DEBUG_CODE_UNDEF)    //
        _SDBG(Vector_Undeffe0, DEBUG_CODE_UNDEF)    //
        .align 5
        .global LC_Exception_Code_End
LC_Exception_Code_End:
        _EPILOG(LC_Exception_Code)

// --------------------------------------------------------------------------
// The following code is a "trampoline".  That is, it is an interrupt vector 
// that contains simple branches to the primary interrupt handler (defined
// above).  The code produced here gets installed into BeDRAM so that machine
// checks may be handled without touching DDR ;  all other interrupts and
// exceptions get reflected (trampolined) back into CNK.
// --------------------------------------------------------------------------

#define LC_TRAMPOLINE(target) \
        .align 5; \
target ## _trampoline: \
	ba target; \
	nop; \
	nop; \
	nop; \
	nop; \
	nop; \
	nop; \
	nop

	.align 12
	_PROLOG_VECTORS(LC_Trampoline)
LC_Trampoline:
        LC_TRAMPOLINE(Vector_MCHK)
        LC_TRAMPOLINE(Vector_CI)
        LC_TRAMPOLINE(Vector_DEBUG)
        LC_TRAMPOLINE(Vector_DSI)
        LC_TRAMPOLINE(Vector_ISI)
        LC_TRAMPOLINE(Vector_EI)
        LC_TRAMPOLINE(Vector_ALGN)
        LC_TRAMPOLINE(Vector_PROG)
        LC_TRAMPOLINE(Vector_FPU)
        LC_TRAMPOLINE(Vector_SC)
        LC_TRAMPOLINE(Vector_APU)
        LC_TRAMPOLINE(Vector_DEC)
        LC_TRAMPOLINE(Vector_FIT)
        LC_TRAMPOLINE(Vector_WDT)
        LC_TRAMPOLINE(Vector_DTLB)
        LC_TRAMPOLINE(Vector_ITLB)
        LC_TRAMPOLINE(Vector_VECTOR)
        LC_TRAMPOLINE(Vector_Undef220)
        LC_TRAMPOLINE(Vector_Undef240)
        LC_TRAMPOLINE(Vector_Undef260)
        LC_TRAMPOLINE(Vector_PDBI)
        LC_TRAMPOLINE(Vector_PDBCI)
        LC_TRAMPOLINE(Vector_GDBI)
        LC_TRAMPOLINE(Vector_GDBCI)
        LC_TRAMPOLINE(Vector_EHVSC)
        LC_TRAMPOLINE(Vector_EHVPRIV)
        LC_TRAMPOLINE(Vector_LRATE)
        LC_TRAMPOLINE(Vector_Undef360)
        LC_TRAMPOLINE(Vector_Undef380)
        LC_TRAMPOLINE(Vector_Undef3a0)
        LC_TRAMPOLINE(Vector_Undef3c0)
        LC_TRAMPOLINE(Vector_Undef3e0)
        LC_TRAMPOLINE(Vector_Undef400)
        LC_TRAMPOLINE(Vector_Undef420)
        LC_TRAMPOLINE(Vector_Undef440)
        LC_TRAMPOLINE(Vector_Undef460)
        LC_TRAMPOLINE(Vector_Undef480)
        LC_TRAMPOLINE(Vector_Undef4a0)
        LC_TRAMPOLINE(Vector_Undef4c0)
        LC_TRAMPOLINE(Vector_Undef4e0)
        LC_TRAMPOLINE(Vector_Undef500)
        LC_TRAMPOLINE(Vector_Undef520)
        LC_TRAMPOLINE(Vector_Undef540)
        LC_TRAMPOLINE(Vector_Undef560)
        LC_TRAMPOLINE(Vector_Undef580)
        LC_TRAMPOLINE(Vector_Undef5a0)
        LC_TRAMPOLINE(Vector_Undef5c0)
        LC_TRAMPOLINE(Vector_Undef5e0)
        LC_TRAMPOLINE(Vector_Undef600)
        LC_TRAMPOLINE(Vector_Undef620)
        LC_TRAMPOLINE(Vector_Undef640)
        LC_TRAMPOLINE(Vector_Undef660)
        LC_TRAMPOLINE(Vector_Undef680)
        LC_TRAMPOLINE(Vector_Undef6a0)
        LC_TRAMPOLINE(Vector_Undef6c0)
        LC_TRAMPOLINE(Vector_Undef6e0)
        LC_TRAMPOLINE(Vector_Undef700)
        LC_TRAMPOLINE(Vector_Undef720)
        LC_TRAMPOLINE(Vector_Undef740)
        LC_TRAMPOLINE(Vector_Undef760)
        LC_TRAMPOLINE(Vector_Undef780)
        LC_TRAMPOLINE(Vector_Undef7a0)
        LC_TRAMPOLINE(Vector_Undef7c0)
        LC_TRAMPOLINE(Vector_Undef7e0)
        LC_TRAMPOLINE(Vector_UDEC)
        LC_TRAMPOLINE(Vector_PERFMON)
        LC_TRAMPOLINE(Vector_Undef840)
        LC_TRAMPOLINE(Vector_Undef860)
        LC_TRAMPOLINE(Vector_Undef880)
        LC_TRAMPOLINE(Vector_Undef8a0)
        LC_TRAMPOLINE(Vector_Undef8c0)
        LC_TRAMPOLINE(Vector_Undef8e0)
        LC_TRAMPOLINE(Vector_Undef900)
        LC_TRAMPOLINE(Vector_Undef920)
        LC_TRAMPOLINE(Vector_Undef940)
        LC_TRAMPOLINE(Vector_Undef960)
        LC_TRAMPOLINE(Vector_Undef980)
        LC_TRAMPOLINE(Vector_Undef9a0)
        LC_TRAMPOLINE(Vector_Undef9c0)
        LC_TRAMPOLINE(Vector_Undef9e0)
        LC_TRAMPOLINE(Vector_Undefa00)
        LC_TRAMPOLINE(Vector_Undefa20)
        LC_TRAMPOLINE(Vector_Undefa40)
        LC_TRAMPOLINE(Vector_Undefa60)
        LC_TRAMPOLINE(Vector_Undefa80)
        LC_TRAMPOLINE(Vector_Undefaa0)
        LC_TRAMPOLINE(Vector_Undefac0)
        LC_TRAMPOLINE(Vector_Undefae0)
        LC_TRAMPOLINE(Vector_Undefb00)
        LC_TRAMPOLINE(Vector_Undefb20)
        LC_TRAMPOLINE(Vector_Undefb40)
        LC_TRAMPOLINE(Vector_Undefb60)
        LC_TRAMPOLINE(Vector_Undefb80)
        LC_TRAMPOLINE(Vector_Undefba0)
        LC_TRAMPOLINE(Vector_Undefbc0)
        LC_TRAMPOLINE(Vector_Undefbe0)
        LC_TRAMPOLINE(Vector_Undefc00)
        LC_TRAMPOLINE(Vector_Undefc20)
        LC_TRAMPOLINE(Vector_Undefc40)
        LC_TRAMPOLINE(Vector_Undefc60)
        LC_TRAMPOLINE(Vector_Undefc80)
        LC_TRAMPOLINE(Vector_Undefca0)
        LC_TRAMPOLINE(Vector_Undefcc0)
        LC_TRAMPOLINE(Vector_Undefce0)
        LC_TRAMPOLINE(Vector_Undefd00)
        LC_TRAMPOLINE(Vector_Undefd20)
        LC_TRAMPOLINE(Vector_Undefd40)
        LC_TRAMPOLINE(Vector_Undefd60)
        LC_TRAMPOLINE(Vector_Undefd80)
        LC_TRAMPOLINE(Vector_Undefda0)
        LC_TRAMPOLINE(Vector_Undefdc0)
        LC_TRAMPOLINE(Vector_Undefde0)
        LC_TRAMPOLINE(Vector_Undefe00)
        LC_TRAMPOLINE(Vector_Undefe20)
        LC_TRAMPOLINE(Vector_Undefe40)
        LC_TRAMPOLINE(Vector_Undefe60)
        LC_TRAMPOLINE(Vector_Undefe80)
        LC_TRAMPOLINE(Vector_Undefea0)
        LC_TRAMPOLINE(Vector_Undefec0)
        LC_TRAMPOLINE(Vector_Undefee0)
        LC_TRAMPOLINE(Vector_Undeff00)
        LC_TRAMPOLINE(Vector_Undeff20)
        LC_TRAMPOLINE(Vector_Undeff40)
        LC_TRAMPOLINE(Vector_Undeff60)
        LC_TRAMPOLINE(Vector_Undeff80)
        LC_TRAMPOLINE(Vector_Undeffa0)
        LC_TRAMPOLINE(Vector_Undeffc0)
        LC_TRAMPOLINE(Vector_Undeffe0)
        .align 5
        .global LC_Trampoline_End
LC_Trampoline_End:
	_EPILOG(LC_Trampoline)

//
// Alternate system-call and standard-interrupt sequences optimized for
// fast handling of jail-mode enter/exit and speculative conflict interrupts.
//
// These alternative paths are patched into the firmware exception trampoline
// in Speculation_EnableFastSpeculationPaths().  The default paths are restored
// in Speculation_CleanupJob().  See cnk/src/speculation.c for details.
//

#include <hwi/include/bqc/l2_central_mmio.h>
#include <hwi/include/bqc/PhysicalMap.h>

        _PROLOG(Vector_FastSpec_SC)
Vector_FastSpec_SC:
	cmpldi	%r0,SC_ENTERJAILMODE
	beq-	LC_FastSpecEnter
	cmpldi	%r0,SC_EXITJAILMODE
	beq-	LC_FastSpecExit
	b	Vector_SC
	_EPILOG(Vector_FastSpec_SC)

        _PROLOG(LC_FastSpecEnter)
LC_FastSpecEnter:
	// get PID for aliased speculation space
	mfspr	%r11,SPRG_pHWThread
	ld	%r0,HWTHR_PhysicalSpecPID(%r11)
	// activate aliased speculative space
	mtspr	SPRN_PID,%r0
	// clear out L1P cache
	ld	%r3,HWTHR_L1P_cfg_pf_sys(%r11)
	li	%r12,((PHYMAP_MINADDR_L1P+PHYMAP_PRIVILEGEDOFFSET)>>27)
	sldi	%r12,%r12,27
	ori	%r0,%r3,L1P_CFG_PF_SYS_pf_invalidate_all
	std	%r0,L1P_CFG_PF_SYS_OFFSET(%r12)
	// switch off invalidate_all flag again
	std	%r3,L1P_CFG_PF_SYS_OFFSET(%r12)
	// flash invalidate L1 to provide clean aliased space
	dci 2
	// touch regular PID location, to bring it back into L1 after the dci
	// we will need it when switching back later
	li	%r12,HWTHR_PhysicalPID
	dcbtct	%r11,%r12,0
	// success, return from system call
	li	%r3,0
	rfi
	nop
	_EPILOG(LC_FastSpecEnter)

        _PROLOG(LC_FastSpecExit)
LC_FastSpecExit:
	// retrieve regular PID, switch back to unaliased space
	mfspr	%r11,SPRG_pHWThread
	ld	%r0,HWTHR_PhysicalPID(%r11)
	mtspr	SPRN_PID,%r0
	// success, return from system call
	li	%r3,0
	rfi
	nop
	_EPILOG(LC_FastSpecExit)

        _PROLOG(Vector_FastSpec_EI)
Vector_FastSpec_EI:
	// minimal context save
	mtspr	SPRN_SPRG3,%r3
	mtspr	SPRN_SPRG4,%r4
	mtspr	SPRN_SPRG5,%r5

	// were we privileged when getting the IRQ? then default handler
	mfmsr	%r3
	mfcr	%r4
	andi.	%r3,%r3,MSR_PR
	beq-	99f

	// is it a spec interrupt? if not, then default handler
	mfspr	%r5,SPRN_PIR
	// BIC device is at the beginning of L1P MMIO space
	li	%r3,((PHYMAP_MINADDR_L1P+PHYMAP_PRIVILEGEDOFFSET)>>27)
	sldi	%r3,%r3,27
	rldimi	%r3,%r5,3,59
	ld	%r3,BIC__ext_int_summary(%r3)
	rldicl.	%r3,%r3,34,60
	beq-	99f

	// Clear IRQ state, allowing us to catch the next event and avoid a race. Lower bits of the reg are not changed
	//SPEC_SetSpeculationIDSelf_priv(0x600);
	li	%r3,((PHYMAP_MINADDR_L1P+PHYMAP_PRIVILEGEDOFFSET)>>27)
	sldi	%r3,%r3,27
	li	%r5,0x600
	std	%r5,L1P_THR_SPECULATION_SELF_OFFSET(%r3)

	// get local ID state, resume if non-spec
	ld	%r5,L1P_THR_SPECULATION_SELF_OFFSET(%r3)
	andi.	%r3,%r5,0xfe
	beq-	90f

	// if still spec, check for priority
	cmpldi	%r3,2
	bne-	80f

	// we are invalid, start rolling back
    70:
	// now we do the rollback, we can now step on all registers!!!!

	// Go non-spec
	// SPEC_SetSpeculationIDSelf_priv(0x400);
	li	%r3,((PHYMAP_MINADDR_L1P+PHYMAP_PRIVILEGEDOFFSET)>>27)
	sldi	%r3,%r3,27
	li	%r5,0x400
	std	%r5,L1P_THR_SPECULATION_SELF_OFFSET(%r3)

	// get specid from TLS struct
	mfspr	%r3,SPRG_pHWThread
	ld	%r3,HWTHR_SpecStateAddr(%r3)
	mfspr	%r5,SPRN_PIR
	sldi	%r5,%r5,7
	add	%r3,%r3,%r5
	ld	%r5,(SPEC_hwt_state+SPEC_THR_specid)(%r3)

	// invalidate ID
	IMM32(%r6, (L2_CENTRAL_PRIV_BASE_TLS_ADDRESS + L2C_id) >> 15)
	sldi	%r6,%r6,15
	li	%r7,(L2C_IDSTATE_PRED_SPEC|L2C_IDSTATE_INVAL)
	rldimi	%r6,%r5,7,50
	std	%r7,L2C_ID_trychangestate(%r6)

	// leave jail
	mfspr	%r5,SPRG_pHWThread
	ld	%r5,HWTHR_PhysicalPID(%r5)
	mtspr	SPRN_PID,%r5

	// restore context
	ld	%r0,SPEC_THR_ip(%r3)
	ld	%r1,SPEC_THR_gpr1(%r3)
	ld	%r2,SPEC_THR_gpr2(%r3)
	li	%r3,SE_ROLLBACK_RC
	mtspr	SPRN_SRR0_IP,%r0
	rfi

	// we are still spec, check priority
    80:
	// SPID | x80/x180 in r5, cr in r4, r3 available

	// we will need some more regs, move stuff to the hwthread scratch area
	mfspr	%r3,SPRG_pHWThread
	std	%r0,HWTHR_SpecSaveR0(%r3)
	std	%r6,HWTHR_SpecSaveR6(%r3)
	std	%r7,HWTHR_SpecSaveR7(%r3)
	dcbtct	0,%r3,0

	// get conflict reg
	IMM32(%r7, (L2_CENTRAL_PRIV_BASE_TLS_ADDRESS + L2C_id) >> 15)
	sldi	%r7,%r7,15
	rldimi	%r7,%r5,7,50
	ld	%r6,L2C_ID_conflict(%r7)

	// if non-recoverable conflict, go to invalidate/rollback
	andi.	%r0,%r6,(L2C_CONFLICT_INVALIDATE | \
			    L2C_CONFLICT_NONSPEC | \
			    L2C_CONFLICT_MULTI)
	bne-	70b

	// no conflict? a race glitch, just return and continue
	andi.	%r0,%r6,L2C_CONFLICT_SINGLE
	beq-	85f

	// we have a single conflict, check the priorities

	ld	%r3,HWTHR_SpecStateAddr(%r3)
#if (SPEC_id_state != 0)
	addi	%r3,%r3,SPEC_id_state
#endif
#if (SPEC_CTX_priority_key != 0)
	addi	%r3,%r3,SPEC_CTX_priority_key
#endif

	// get our prio
	rldic	%r5,%r5,7,50
	ldx	%r5,%r3,%r5

	// get other prio
	rldic	%r0,%r6,7,50
	ldx	%r0,%r3,%r0

	// if we lose (other timebase less than our timebase), go to rollback
	cmpd	%r0,%r5
	blt-	70b

	// now kill the other
	rldimi	%r7,%r6,7,50
	li	%r0,(L2C_IDSTATE_PRED_SPEC|L2C_IDSTATE_INVAL)
	std	%r0,L2C_ID_trychangestate(%r7)

	// and continue own execution (we won!!)
	mfspr	%r3,SPRG_pHWThread

	// restore some GPRs and continue thread
    85:
	ld	%r0,HWTHR_SpecSaveR0(%r3)
	ld	%r6,HWTHR_SpecSaveR6(%r3)
	ld	%r7,HWTHR_SpecSaveR7(%r3)

	// committed, we just return
    90:
	mtcr	%r4
	mfspr	%r3,SPRN_SPRG3
	mfspr	%r4,SPRN_SPRG4
	mfspr	%r5,SPRN_SPRG5
	rfi

	// irq inside kernel or not a spec irq
    99:
	mtcr	%r4
#if 0
	mfspr	%r3,SPRN_SPRG3
	mfspr	%r4,SPRN_SPRG4
	mfspr	%r5,SPRN_SPRG5
	b	Vector_EI
#else
	b	Vector_EI+12
#endif
	_EPILOG(Vector_FastSpec_EI)

        LC_TRAMPOLINE(Vector_FastSpec_EI)
        LC_TRAMPOLINE(Vector_FastSpec_SC)

        .global	Vector_EI_trampoline
        .global	Vector_SC_trampoline
        .global	Vector_FastSpec_EI_trampoline
        .global	Vector_FastSpec_SC_trampoline
