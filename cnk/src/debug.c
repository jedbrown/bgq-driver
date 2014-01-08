/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
#include "Kernel.h"
#include <hwi/include/bqc/wu_mmio.h>
#include "tool.h"

#define DBSR_DAC_MATCH (DBSR_DAC1R | DBSR_DAC1W | DBSR_DAC2R | DBSR_DAC2W | DBSR_DAC3R | DBSR_DAC3W | DBSR_DAC4R | DBSR_DAC4W)
#define DBSR_IAC_MATCH (DBSR_IAC1 | DBSR_IAC2 | DBSR_IAC3 | DBSR_IAC4)

__C_LINKAGE void IntHandler_Debug(Regs_t *context, int code)
{
    CountException(CNK_HWTPERFCOUNT_UNKDEBUG + code);
    uint64_t timebase = GetTimeBase();

    // Pick up status registers that are not saved on normal interrupt paths.
    context->esr = mfspr(SPRN_ESR);
    context->dear = mfspr(SPRN_DEAR);
    context->dbsr = mfspr(SPRN_DBSR);

#if 0
    printf("DEBUG INTERRUPT ip:%016lx esr:%016lx dear:%016lx intcode:%d hwpid:%ld cur tid:%d\n", context->ip, context->esr, context->dear, code, mfspr(SPRN_PID), GetTID(GetMyKThread()));
    int i;
    for (i=0; i<8; i++)
    {
        printf("GPR %02d:%016lx  %02d:%016lx  %02d:%016lx  %02d:%016lx\n", i*4, context->gpr[0+i*4], 1+i*4, context->gpr[1+i*4], 2+i*4, context->gpr[2+i*4], 3+i*4, context->gpr[3+i*4]);
    }
#endif  
    Kernel_WriteFlightLog(FLIGHTLOG_high, FL_DEBUGEXCP, context->ip, (((uint64_t)context->dbsr)<<32)|context->esr, code, context->dear);
    
    if (context->msr & MSR_PR)
    {
        // If speculation active, set to restart point. DEFER the invalidation of the specid. Tool may want to inspect data.
        Speculation_CheckJailViolationDefer(context); // New way. Defer
    }
    if (code == DEBUG_CODE_DEBUG)
    {
        if (!(context->msr & MSR_PR))
        {
            // We are processing a debug interrupt in the kernel. The most likely
            // reason for this flow is an ICMP interrupt (step operation) set by a tool.  
            // If ICMP was the reason, then this will be disabled while we step through the kernel with MSR[DE] reset
            context->msr = (context->msr & ~MSR_DE); // turn off MSR[DE] in the kernel context that will be restored
            //printf("Debug interrupt code %d in the kernel. Turning off MSR[DE] and returning\n", code);
            return;
        }
    }
    if (code == DEBUG_CODE_PROG)
    {
        // Reset the status indicator
        mtspr(SPRN_ESR,0);
        isync();
    }
    // Did this condition occur while running in the problem state,
    // and is it an exception type that an application can cause?
    if ((context->msr & MSR_PR) &&
	((code == DEBUG_CODE_ALGN) || (code == DEBUG_CODE_PROG) ||
	 (code == DEBUG_CODE_DSI) || (code == DEBUG_CODE_ISI) ||
	 (code == DEBUG_CODE_DTLB) || (code == DEBUG_CODE_ITLB) ||
	 (code == DEBUG_CODE_DEBUG)))
    {
	// Alignment exceptions might be handled without generating a signal.
	if ((code == DEBUG_CODE_ALGN) && align_handler(context))
	{
            return;
	}
        // Breakpoint trap, step, and DAC handling 
        if (((code == DEBUG_CODE_PROG) && (context->esr & ESR_PTR)) ||         // Trap Breakpoint 
            ((code == DEBUG_CODE_DEBUG) && ((context->dbsr & DBSR_DAC_MATCH) || (context->dbsr & DBSR_IAC_MATCH))))  // DAC Watchpoint or IAC
        {
            if(Tool_TrapHandler(context)) 
            {
                return; // Fast exit. We are now enabled to bypass a breakpoint trap condition not meant for our process.
            }
        }
        else if ((code == DEBUG_CODE_DEBUG) && (context->dbsr &  DBSR_ICMP))
        {
            if(Tool_StepHandler(context)) 
            {
                return; // Fast exit. We have processed a step operation around a breakpoint trap not set in our process.
            }
        }
	Signal_Exception(code, timebase);
    }
    else
    {
	RASBEGIN(6);
	RASPUSH(context->ip);
	RASPUSH(context->lr);
	RASPUSH(context->esr);
	RASPUSH(context->dear);
	RASPUSH(context->msr);
        RASPUSH(code);
	RASFINAL(RAS_KERNELCRASH);
	
	printf("CNK Terminating...\n");
	printf("MSR=%lx\n", context->msr);
	printf("CODE=%d\n", code);
	printf("IP  =%lx\n", context->ip);
	printf("LR  =%lx\n", context->lr);
	printf("ESR =%lx\n", context->esr);
	printf("DEAR=%lx\n", context->dear);

	int i;
	for (i=0; i<8; i++)
	{
	    printf("GPR %02d:%016lx  %02d:%016lx  %02d:%016lx  %02d:%016lx\n", i*4, context->gpr[0+i*4], 1+i*4, context->gpr[1+i*4], 2+i*4, context->gpr[2+i*4], 3+i*4, context->gpr[3+i*4]);
	}
	// The following stack dumping may not always succeed since we got here due to an unexpected kernel condition
	// which may include corruption in the kernel stack.
	printf("Stack:\n");
	HWThreadState_t *hwt = GetMyHWThreadState();
	uint64_t *kernel_stack_start = (uint64_t *)(&(hwt->StandardStack));
	uint64_t *kernel_stack_end   = (uint64_t *)((uint64_t)&(hwt->StandardStack)) + sizeof(hwt->StandardStack);
	uint64_t *stkptr = (uint64_t*)(context->gpr[1]);
	int numStackFrames = 0; // initialize to zero    
	while (stkptr &&   // stack pointer is not NULL
	       (numStackFrames < 8) &&  // did not yet store the max number of frames
	       ( ((stkptr > kernel_stack_start) && (stkptr < kernel_stack_end))  || // stack pointer is a kernel stack address
                (GetMyProcess() && VMM_IsAppAddress(stkptr, sizeof(uint64_t[3]))))) // process exists andstack pointer is an app address
	{
	    printf("%016lx   %016lx\n", (uint64_t)stkptr, *(stkptr+2));  // current stack frame pointer and value of the saved link register
	    stkptr = (uint64_t*)(*stkptr);
	    numStackFrames++;
	}
	Kernel_Crash(code);
    }
}
