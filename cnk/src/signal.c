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

#if __clang__
#define malloc(n) NULL
#define free(n)
#endif

#include <spi/include/l1p/pprefetch.h>
#include <asm/unistd.h>

#if __clang__
L1P_SPIContext           _L1P_Context[68];
L1P_PatternLimitPolicy_t _L1P_PatternLimitMode;
uint64_t                 _L1P_PatternLimit;
uint64_t                 _L1P_PatternCount;
#endif

// implemented in fs/shmFS.cc
void FLock_Interrupt(KThread_t *kthr);

/* List of CNK specific signals:

SIGHARDWAREFAILURE 35
SIGMUNDFATAL       36
SIGMUFIFOFULL      37
SIGDCRVIOLATION    38
SIGNOSPECALIAS     39

 */


#define SIG_MASK(sig) (1ul << ((sig) - 1))

#define SIG_DFLT_IGNORE_SET ( \
	SIG_MASK(SIGCHLD) | \
	SIG_MASK(SIGURG) | \
	SIG_MASK(SIGWINCH) | \
        SIG_MASK(SIGMUFIFOFULL) | \
        SIG_MASK(SIGCONT))

#define SIG_DFLT_COREDUMP_SET ( \
	SIG_MASK(SIGQUIT) | \
	SIG_MASK(SIGILL) | \
	SIG_MASK(SIGABRT) | \
	SIG_MASK(SIGFPE) | \
	SIG_MASK(SIGSEGV) | \
	SIG_MASK(SIGBUS) | \
	SIG_MASK(SIGSYS) | \
	SIG_MASK(SIGTRAP) | \
	SIG_MASK(SIGXCPU) | \
	SIG_MASK(SIGXFSZ) | \
	SIG_MASK(SIGMUNDFATAL) | \
        SIG_MASK(SIGDCRVIOLATION) | \
        SIG_MASK(SIGNOSPECALIAS) | \
	SIG_MASK(SIGSTKFLT)) // used for stack-guard violations

#define SIG_NONMASKABLE_SET ( \
	SIG_MASK(SIGKILL) | \
	SIG_MASK(SIGSTOP) | \
	SIG_MASK(SIGMUNDFATAL) | \
        SIG_MASK(SIGDCRVIOLATION) | \
	SIG_MASK(SIGSTKFLT)) // used for stack-guard violations

static KThread_t* isSignalNotMasked(KThread_t *kthread, int sig)
{
    if (SIG_MASK(sig) & SIG_NONMASKABLE_SET)
    {
        return kthread;
    }
    else
    {
        if (!(kthread->MaskedSignals & SIG_MASK(sig)))
        {
            // This signal is not masked. We have found a viable candidate
            return kthread;
        }
    }
    return NULL;
}

static SignalAction_t DefaultSignalAction(int sig)
{
    if ((SIG_MASK(sig) & SIG_DFLT_IGNORE_SET) != 0)
    {
	return SIG_IGNORE;
    }
    else if ((SIG_MASK(sig) & SIG_DFLT_COREDUMP_SET) != 0)
    {
	return SIG_COREDUMP;
    }
    else
    {
	return SIG_TERMINATE;
    }
}

static void ExceptionSigInfo(KThread_t *kthr)
{
    Regs_t *regs = &kthr->Reg_State;
    HWThreadState_t *hwt = GetMyHWThreadState();

    switch (kthr->ExceptionCode)
    {
	case DEBUG_CODE_DSI:
	    kthr->SigInfoSigno = SIGSEGV;
	    kthr->SigInfoCode = SEGV_ACCERR;
	    kthr->SigInfoAddr = (void *) regs->dear;
	    break;
	case DEBUG_CODE_ISI:
	    kthr->SigInfoSigno = SIGSEGV;
	    kthr->SigInfoCode = SEGV_ACCERR;
	    kthr->SigInfoAddr = (void *) regs->ip;
	    break;
	case DEBUG_CODE_DTLB:
	    kthr->SigInfoSigno = SIGSEGV;
	    kthr->SigInfoCode = SEGV_MAPERR;
	    kthr->SigInfoAddr = (void *) regs->dear;
	    break;
	case DEBUG_CODE_ITLB:
	    kthr->SigInfoSigno = SIGSEGV;
	    kthr->SigInfoCode = SEGV_MAPERR;
	    kthr->SigInfoAddr = (void *) regs->ip;
	    break;
	case DEBUG_CODE_PROG: 
	    if (regs->esr & (ESR_PIL | ESR_PUO))
	    {
		kthr->SigInfoSigno = SIGILL;
		kthr->SigInfoCode = ILL_ILLOPC;
		kthr->SigInfoAddr = (void *) regs->ip;
	    }
	    else if (regs->esr & (ESR_PPR | ESR_DLK0 | ESR_DLK1))
	    {
		kthr->SigInfoSigno = SIGILL;
		kthr->SigInfoCode = ILL_PRVOPC;
		kthr->SigInfoAddr = (void *) regs->ip;
	    }
	    else if (regs->esr & ESR_PTR)
	    {
		kthr->SigInfoSigno = SIGTRAP;
		kthr->SigInfoCode = TRAP_BRKPT;
		kthr->SigInfoAddr = (void *) regs->ip;
	    }
	    else if (regs->esr & (ESR_FP | ESR_AP))
	    {
		kthr->SigInfoSigno = SIGFPE;
		// This is a real pain.  We haven't preserved the fpscr yet,
		// so we have to enable floating point and save/restore a
		// qvr, just to look at it.
		uint64_t fpscr;
		char old_qvr0_space[64];
		char *old_qvr0 =
		    (char *) ((((uint64_t) old_qvr0_space) + 31ul) & ~31ul);
		uint64_t old_msr = mfmsr();
		mtmsr(old_msr | MSR_FP);
		asm volatile("qvstfdx 0,0,%1;"
			     "mffs 0;"
			     "stfd 0,%0;"
			     "qvlfdx 0,0,%1"
			     : "=m" (fpscr) : "b" (old_qvr0) : "memory");
		mtmsr(old_msr);
		if ((fpscr & (FPSCR_VE|FPSCR_VX)) == (FPSCR_VE|FPSCR_VX))
		{
		    kthr->SigInfoCode = FPE_FLTINV;
		}
		else if ((fpscr & (FPSCR_OE|FPSCR_OX)) == (FPSCR_OE|FPSCR_OX))
		{
		    kthr->SigInfoCode = FPE_FLTOVF;
		}
		else if ((fpscr & (FPSCR_UE|FPSCR_UX)) == (FPSCR_UE|FPSCR_UX))
		{
		    kthr->SigInfoCode = FPE_FLTUND;
		}
		else if ((fpscr & (FPSCR_ZE|FPSCR_ZX)) == (FPSCR_ZE|FPSCR_ZX))
		{
		    kthr->SigInfoCode = FPE_FLTDIV;
		}
		else if ((fpscr & (FPSCR_XE|FPSCR_XX)) == (FPSCR_XE|FPSCR_XX))
		{
		    kthr->SigInfoCode = FPE_FLTRES;
		}
		else
		{
		    // can't happen, maybe?
		    kthr->SigInfoCode = FPE_FLTINV;
		}
		kthr->SigInfoAddr = (void *) regs->ip;
	    }
	    break;
    case DEBUG_CODE_DEBUG: 
            kthr->SigInfoSigno = SIGTRAP;
            kthr->SigInfoCode = TRAP_TRACE;
            if (regs->dbsr & (DBSR_DAC1R | DBSR_DAC1W))
            {
                kthr->SigInfoCode = TRAP_HWBKPT;
                kthr->SigInfoAddr = (void *) hwt->CriticalState.dac1;
            }
            else if (regs->dbsr & (DBSR_DAC2R | DBSR_DAC2W))
            {
                kthr->SigInfoCode = TRAP_HWBKPT;
                // Are we in exact address mode with DAC1 and DAC2?
                if (!(kthr->Reg_State.dbcr2 & DBCR2_DAC12M))
                {
                    kthr->SigInfoAddr = (void *) hwt->CriticalState.dac2;
                }
            }
            if (regs->dbsr & (DBSR_DAC3R | DBSR_DAC3W))
            {
                kthr->SigInfoCode = TRAP_HWBKPT;
                // If both the DAC1 and DAC3 fired, store the match with the lower address
                // This can occur if one write operation touches both DAC ranges.
                if (!((regs->dbsr & (DBSR_DAC1R | DBSR_DAC1W)) && (hwt->CriticalState.dac1 < hwt->CriticalState.dac3)))
                {
                    kthr->SigInfoAddr = (void *) hwt->CriticalState.dac3;
                }
            }
            else if (regs->dbsr & (DBSR_DAC4R | DBSR_DAC4W))
            {
                kthr->SigInfoCode = TRAP_HWBKPT;
                // Are we in exact address mode with DAC3 and DAC4?
                if (!(kthr->Reg_State.dbcr3 & DBCR3_DAC34M))
                {
                    kthr->SigInfoAddr = (void *) hwt->CriticalState.dac4;
                }
            }
	    break;
	case DEBUG_CODE_ALGN:
	    kthr->SigInfoSigno = SIGBUS;
	    kthr->SigInfoCode = BUS_ADRALN;
	    kthr->SigInfoAddr = (void *) regs->dear;
	    break;
	default:
	    // Not sure why we'd get here, but just in case ...
	    kthr->SigInfoSigno = SIGBUS;
	    kthr->SigInfoCode = BUS_OBJERR;
	    kthr->SigInfoAddr = (void *) regs->ip;
	    break;
    }
}

/*
 * Stack layout for signal delivery.
 *
 *                          |                           |
 *                          |                           |
 *      original sp ->      +---------------------------+
 *                          |                           |
 *                          | ABI-protected stack tail  |
 *                          |                           |
 *                          +---------------------------+
 *                          | variable padding          |
 *                    ----- +---------------------------+ <- 32-byte boundary
 *                      ^   | constant padding          |
 *                          +---------------------------+
 *          SIGINFO_OFFSET  |                           |
 *                          | siginfo                   |
 *                      v   |                           |
 *                    ----- +---------------------------+ <- 32-byte boundary
 *                      ^   | constant padding          |
 *                          +---------------------------+
 *                          |                           |
 *                          |                   <----+  | <- 32-byte boundary
 *                          |     vmx_reserve        |  |
 *                          |     .................  |  |
 *                          |     vregs          ----+  |
 *                          |     .................     |
 *                          |                           |
 *                          |                           |
 *         UCONTEXT_OFFSET  |                           |
 *                          |                           |
 *                          |                           |
 *                          |     .................     |
 *                          |     fpregs                |
 *                          |     .................     | <- 32-byte boundary
 *                          |                           |
 *                          |                           |
 *                          | ucontext                  |
 *                      v   |                           |
 *                    ----- +---------------------------+
 *                      ^   | constant padding          |
 *                          +---------------------------+
 *                          |                           |
 *                          |                           |
 *            FRAME_OFFSET  |                           |
 *                          |                           |
 *                          | call frame                |
 *                      v   |     backchain             |
 *        new sp ->   ----- +---------------------------+ <- 32-byte boundary
 *
 * For efficient QPX save/restore, we want both the ucontext vregs pointer
 * and the fpregs field to be 32-byte aligned.  The pointer is easy because
 * it points into the vmx_reserve field, which has extra space to allow for
 * alignment.  The fpregs field, however, is at a fixed offset within
 * ucontext, so we shift the whole structure to get that field aligned.
 * The offsets between structures are therefore somewhat complicated, albeit
 * constant.  It's particularly important that FRAME_OFFSET be constant
 * because it's embedded in the signal trampoline code.
 */
const uint32_t SIGINFO_OFFSET =
    ((sizeof(kern_siginfo_t) + 31) & ~31);

const uint32_t UCONTEXT_OFFSET =
    (((sizeof(struct kern_ucontext_t) + 31) & ~31) +
	(offsetof(struct kern_ucontext_t, uc_mcontext.fp_regs) & 31));

const uint32_t FRAME_OFFSET =
    (STACK_FRAME_SIZE +
	(32 - (offsetof(struct kern_ucontext_t, uc_mcontext.fp_regs) & 31)));

/*!
 * \brief Push a signal-delivery frame on the kthread's user stack.
 */
struct kern_ucontext_t *PushSignal(KThread_t *kthr)
{
    AppProcess_t *proc = kthr->pAppProc;
    SignalData_t *sd;
    SignalAction_t action;
    uint64_t sp;
    int i, sig;

    Kernel_Lock(&proc->SignalLock);

    sig = kthr->SigInfoSigno;
    //printf("PushSignal. signal %u bypassNotify %d\n", sig, bypassNotify );
    if (sig != 0)
    {
	// We have a signal already in process.  Continue with it.
	sd = &proc->SignalData[sig - 1];
	action = sd->action;
	if (action == SIG_IGNORE)
	{
	    // Action has changed, so drop this signal.
	    kthr->SigInfoSigno = 0;
	    sig = 0;
	}
    }
    if (sig == 0)
    {
	// We didn't have a signal in process, or we dropped it.  Keep looking.
	if (kthr->ExceptionCode != DEBUG_CODE_NONE)
	{
	    ExceptionSigInfo(kthr);
	    sig = kthr->SigInfoSigno;
	    sd = &proc->SignalData[sig - 1];
	    action = sd->action;
	    if ((action == SIG_IGNORE) ||
		((SIG_MASK(sig) & kthr->MaskedSignals) != 0))
	    {
		// Cannot ignore or mask exceptions.
		action = DefaultSignalAction(sig);
	    }
	}
	else
	{
	    do
	    {
		sig = 64 - cntlz64(kthr->PendingSignals &
					~(kthr->MaskedSignals));
		if (sig == 0)
		{
		    // Nothing to deliver after all.
		    Kernel_Unlock(&proc->SignalLock);
		    return NULL;
		}
		kthr->PendingSignals &= ~SIG_MASK(sig);
		sd = &proc->SignalData[sig - 1];
		action = sd->action;
	    } while (action == SIG_IGNORE);
	    kthr->SigInfoSigno = sig;
	    // HACK:  We're supposed to set si_code based on how the signal
	    //        was generated.  We're not currently tracking that
	    //        information, so simply set si_code based on the
	    //        signal number.
	    switch (sig)
	    {
		case SIGALRM:
		case SIGPROF:
		    kthr->SigInfoCode = SI_TIMER;
		    break;
		case SIGSTKFLT:
		    kthr->SigInfoCode = SI_KERNEL;
		    break;
		default:
		    kthr->SigInfoCode = SI_TKILL;
	    }
	    kthr->SigInfoAddr = NULL;
	}
        if (proc->toolAttached && Tool_IsNotifyRequired(kthr)) 
        {
            kthr->SchedulerDivert |= SCHED_DIVERT_TOOL;
            kthr->Pending |= (KTHR_PENDING_YIELD | KTHR_PENDING_SIGNAL);
            Kernel_Unlock(&proc->SignalLock);
            return NULL;
        }
    }
    // Test to see if there is a pending Jail Violation to process
    if (Speculation_CheckPendingRestart() && (kthr->ExceptionCode != DEBUG_CODE_NONE))
    {
        // We need to drop the signal and make like it didnt happen.
        kthr->SigInfoSigno = 0;
        kthr->ExceptionCode = DEBUG_CODE_NONE;
        Kernel_Unlock(&proc->SignalLock);
        return NULL;
    }
    if ((action == SIG_TERMINATE) || (action == SIG_COREDUMP))
    {
	kthr->SchedulerDivert |= ((action == SIG_TERMINATE) ?
				    SCHED_DIVERT_TERMINATE :
				    SCHED_DIVERT_COREDUMP);
	kthr->Pending |= (KTHR_PENDING_YIELD | KTHR_PENDING_SIGNAL);
	Kernel_Unlock(&proc->SignalLock);
	return NULL;
    }

    // action must be SIG_HANDLER
    if ((sd->flags & SA_RESETHAND) != 0)
    {
	// Reset the action, as requested.
	sd->action = DefaultSignalAction(sig);
    }
    uint64_t hndlr_desc = sd->handler.descriptor;
    uint64_t hndlr_addr = sd->handler.address;
    uint64_t hndlr_toc = sd->handler.toc;
    uint64_t hndlr_siginfo = (sd->flags & SA_SIGINFO);

    uint64_t oldmask = kthr->MaskedSignals;
    kthr->MaskedSignals |= sd->mask;  // Mask additional signals.
    if ((sd->flags & SA_NODEFER) == 0)
    {
	// Mask this signal unless explicitly asked not to.
	kthr->MaskedSignals |= SIG_MASK(sig);
    }

    uint64_t l1p_pp_ctrl_save = 0ul;
    if ((sd->flags & SA_BGQ_PP_PAUSE) != 0)
    {
	// Pause the list prefetcher before making any stack references.
	uint64_t *l1p_pp_ctrl = (uint64_t *)
	    (Kernel_L1pBaseAddress() + L1P_PP_CTRL +
		(L1P_PP_THREADOFFSET * Kernel_ProcessorThreadID()));
	l1p_pp_ctrl_save = *l1p_pp_ctrl;
	(*l1p_pp_ctrl) = l1p_pp_ctrl_save | L1P_PP_CTRL_PAUSE;
	l1p_pp_ctrl_save = ~l1p_pp_ctrl_save; // complement guarantees nonzero
    }

    Kernel_Unlock(&proc->SignalLock);

    if ((kthr->PendingSignals & ~(kthr->MaskedSignals)) != 0)
    {
	// There are more signals waiting to be delivered.
	kthr->Pending |= KTHR_PENDING_SIGNAL;
    }

    sp = kthr->Reg_State.gpr[1];
    sp -= _ABI_STACK_TAIL;
    sp &= ~31ul;    // start with 32-byte alignment
    sp -= SIGINFO_OFFSET;
    kern_siginfo_t *si = (kern_siginfo_t *) sp;
    sp -= UCONTEXT_OFFSET;
    struct kern_ucontext_t *uc = (struct kern_ucontext_t *) sp;
    sp -= FRAME_OFFSET;

    if (!VMM_IsAppAddress((void *) sp, kthr->Reg_State.gpr[1] - sp))
    {
	// Stack isn't writable.  Just dump core.
	kthr->SchedulerDivert |= SCHED_DIVERT_COREDUMP;
	kthr->Pending |= (KTHR_PENDING_YIELD | KTHR_PENDING_SIGNAL);
        return NULL;
    }

    Kernel_WriteFlightLog(FLIGHTLOG, FL_SIGHANDLE,
			  GetTID(kthr), sig, hndlr_desc, (uint64_t) uc);

    *((uint64_t *) sp) = kthr->Reg_State.gpr[1]; // backchain

    // Setup siginfo
    memset(si, 0, sizeof(*si));
    si->si_signo = kthr->SigInfoSigno;
    si->si_code = kthr->SigInfoCode;
    si->si_addr = kthr->SigInfoAddr;
    kthr->SigInfoSigno = 0;  // Signal is no longer in process
    kthr->ExceptionCode = DEBUG_CODE_NONE;  // Exception (if any) handled

    // Setup ucontext
    struct pt_regs *regs = (struct pt_regs *) &uc->uc_mcontext.gp_regs;
    uc->uc_mcontext.regs = regs;

    uc->uc_flags = 0;
    uc->uc_link = NULL;
    uc->uc_stack.ss_sp = NULL;
    uc->uc_stack.ss_flags = 0;
    uc->uc_stack.ss_size = 0;
    uc->uc_sigmask.sig[0] = oldmask;
    uc->__unused[0].sig[0] = l1p_pp_ctrl_save; // an otherwise unused word
    uc->uc_mcontext.signal = sig;
    uc->uc_mcontext.handler = hndlr_desc;
    uc->uc_mcontext.oldmask = 0;
    // copy volatile registers into ucontext
    regs->nip = kthr->Reg_State.ip;
    regs->msr = kthr->Reg_State.msr;
    regs->ctr = kthr->Reg_State.ctr;
    regs->link = kthr->Reg_State.lr;
    regs->xer = kthr->Reg_State.xer;
    regs->ccr = kthr->Reg_State.cr;
    regs->dar = kthr->Reg_State.dear;
    for (i = 0; i < 14; i++)
    {
	regs->gpr[i] = kthr->Reg_State.gpr[i];
    }
    // non-volatile registers and QPX state will be saved when we return

    kthr->Reg_State.ip = hndlr_addr;
    if (proc->DebugRegSyscallsEnabled && !proc->toolAttached)
    {
        kthr->Reg_State.msr &= ~MSR_DE;
    }
    kthr->Reg_State.lr =
	(uint64_t) &((KernelInfo_t *) proc->KernelInfoAddress)->SigTramp;
    kthr->Reg_State.gpr[1] = sp;
    kthr->Reg_State.gpr[2] = hndlr_toc;
    kthr->Reg_State.gpr[3] = sig;
    if (hndlr_siginfo)
    {
	kthr->Reg_State.gpr[4] = (uint64_t) si;
	kthr->Reg_State.gpr[5] = (uint64_t) uc;
    }
    else
    {
	// Provide undocumented, obsolete second arg to non-siginfo handlers.
	kthr->Reg_State.gpr[4] = (uint64_t) (&uc->uc_mcontext);
	kthr->Reg_State.gpr[5] = 0;
    }

    return uc;
}

void Signal_Deliver(AppProcess_t *proc, int tid, int sig)
{
    KThread_t *kthr = NULL;
    // Is this signal a process scoped signal to be delivered to any thread in the process? 
    if (tid <= 0)
    {
        if ((tid == 0) &&
            (proc->ProcessLeader_KThread->ProcessorID != ProcessorID()))
        {
            // We're not on the right hwthread yet.
            IPI_signal(proc->ProcessLeader_KThread->ProcessorID, proc, tid, sig);
            return;
        }
        // We need to find any kthread within the process
        // Our first choice is the currently active thread. If the currently active thread is not the kernel thread,
        // and it is associated with our target process, then send the signal to this thread.
        // Note: because of the extended affinity model support, this test of the process is needed.
        HWThreadState_t *hwt = GetMyHWThreadState();
        if ( (hwt->pCurrentThread->SlotIndex != CONFIG_SCHED_KERNEL_SLOT_INDEX ) &&
             (hwt->pCurrentThread->pAppProc == proc)) // Is the current thread same as the requested kthread's process
        {
            // If we are processing an unmaskable signal or if the kthread signal mask is not set then select this kthread.
            kthr = isSignalNotMasked(hwt->pCurrentThread, sig);
        }
        // If no kthread was found, our next choice will be a kthread on the same hardware thread.
        if (!kthr)
        {
            int index;
            for (index=0; index<CONFIG_AFFIN_SLOTS_PER_HWTHREAD; index++)
            {
                if (!(((hwt->SchedSlot[index])->State  & SCHED_STATE_FREE) || 
                      (((hwt->SchedSlot[index])->State & SCHED_STATE_HOLD) && (sig != SIGKILL))))
                {
                    kthr = isSignalNotMasked(hwt->SchedSlot[index], sig);
                    if (kthr) break;
                }
            }
        }
        // Did we find a appropriate kthread for our target yet? If not we will now look on other hardware threads
        if (!kthr)
        {
            // No kthread exists on this hardware thread. We must look to see if kthreads exist on other
            // hardware threads that can handle this signal. 
            uint64_t hwtmask = proc->HwthreadMask;
            int my_hwt_index = ProcessorID();
            int hwtindex;
            int next_hwt = 0; // Zero will never be a found value for the next hwt since this is always a "plus one".
            for (hwtindex=my_hwt_index+1; hwtindex< (64-my_hwt_index); hwtindex++)
            {
                if (_BN(hwtindex) & hwtmask)
                {
                    // Found another hardware thread in the process
                    next_hwt = hwtindex;
                    break;
                }
            }
            if (next_hwt)
            {
                // This is the next hwthread configured for this process. Attempt to deliver the signal to this hwthread
                IPI_signal(next_hwt, proc, -1, sig); // pass -1 for the TID so we know that we are in the hardware thread search mode when we re-enter
                return;
            }
            else
            {
                // No more hardware threads within this process to search. No thread exists to take this signal.
                return;
            }
        }
    }
    else
    {
        // Thread targeted signal delivery.
        kthr = GetKThreadFromTid(tid);
        if (!kthr)
        {
            // Kthread no longer exists (thread may have exited)
            return;
        }
        if (kthr->ProcessorID != ProcessorID())
        {
            // We're not on the right hwthread yet.
            IPI_signal(kthr->ProcessorID, proc, tid, sig);
            return;
        }
    }
    if (sig == SIGKILL)
    {
        // Make no question as to the next signal we are going to process
        kthr->PendingSignals = SIG_MASK(sig); // clobber any previous pending signals and set SIGKILL pending
        Tool_CleanupForSIGKILL(kthr);
    }
    else
    {
        kthr->PendingSignals |= SIG_MASK(sig); // Add the signal to the currently pending signals
    }
    if ((kthr->PendingSignals & ~(kthr->MaskedSignals)) != 0)
    {
	// If the signal is destined to be ignored, just drop it here to avoid
	// spurious syscall interruptions and unnecessary Scheduler() calls.
	// We're looking at only one field in the application SignalData
	// structure (and not changing anything) so don't bother with the lock.
	if (kthr->pAppProc->SignalData[sig - 1].action == SIG_IGNORE)
	{
	    kthr->PendingSignals &= ~SIG_MASK(sig);
	    return;
	}

	kthr->Pending |= KTHR_PENDING_SIGNAL;
	if (kthr->State & SCHED_STATE_FUTEX)
	{
	    Futex_Interrupt(kthr);
	}
	else if (kthr->State & SCHED_STATE_SLEEP)
	{
	    Timer_Interrupt(kthr);
	}
	else if (kthr->State & SCHED_STATE_FLOCK)
	{
	    FLock_Interrupt(kthr);
	}

	KThread_t *cur_kthr = GetMyKThread();
	if ((kthr != cur_kthr) && (kthr->Priority > cur_kthr->Priority))
	{
	    cur_kthr->Pending |= KTHR_PENDING_YIELD;
	}
    }
}

void Signal_Exception(int code, uint64_t timebase)
{
    KThread_t *kthr = GetMyKThread();
    kthr->ExceptionCode = code;
    kthr->ExceptionTime = timebase;
    kthr->Pending |= KTHR_PENDING_SIGNAL;
}

struct kern_ucontext_t *PopSignal(KThread_t *kthr)
{
    AppProcess_t *app = kthr->pAppProc;
    struct kern_ucontext_t *uc =
	(struct kern_ucontext_t *) kthr->Reg_State.gpr[1];
    int i;

    if (!VMM_IsAppAddress(uc, sizeof(*uc)))
    {
	// Kill the app with a simulated segfault.
	kthr->Reg_State.dear = (uint64_t) uc;
	Signal_Exception(DEBUG_CODE_DTLB, GetTimeBase());
	return NULL;
    }
    struct pt_regs *regs = uc->uc_mcontext.regs;
    if (regs != ((struct pt_regs *) &uc->uc_mcontext.gp_regs))
    {
	// regs is supposed to point to the gp_regs array.
	// Kill the app with a simulated segfault.
	kthr->Reg_State.dear = (uint64_t) regs;
	Signal_Exception(DEBUG_CODE_DTLB, GetTimeBase());
	return NULL;
    }
    uint64_t v_regs = (uint64_t) uc->uc_mcontext.v_regs;
    if ((v_regs < ((uint64_t) &uc->uc_mcontext.vmx_reserve[0])) ||
	(v_regs > ((uint64_t) &uc->uc_mcontext.vmx_reserve[3])))
    {
	// v_regs is supposed to point into the vmx_reserve space.
	// Kill the app with a simulated segfault.
	kthr->Reg_State.dear = (uint64_t) v_regs;
	Signal_Exception(DEBUG_CODE_DTLB, GetTimeBase());
	return NULL;
    }

    Kernel_WriteFlightLog(FLIGHTLOG, FL_SIGRETURN,
			  GetTID(kthr), (uint64_t) uc, regs->nip, 0);

    kthr->MaskedSignals = (uc->uc_sigmask.sig[0] & ~SIG_NONMASKABLE_SET);

    if ((kthr->PendingSignals & ~(kthr->MaskedSignals)) != 0)
    {
	// There are more signals waiting to be delivered.
	kthr->Pending |= KTHR_PENDING_SIGNAL;
    }

    uint64_t l1p_pp_ctrl_save = uc->__unused[0].sig[0];

    // copy volatile registers from ucontext
    kthr->Reg_State.ip = regs->nip;
    kthr->Reg_State.msr = ((regs->msr & ~app->msrForbidden) | app->msrRequired);
    kthr->Reg_State.ctr = regs->ctr;
    kthr->Reg_State.lr = regs->link;
    kthr->Reg_State.xer = regs->xer;
    kthr->Reg_State.cr = regs->ccr;
    for (i = 0; i < 14; i++)
    {
	kthr->Reg_State.gpr[i] = regs->gpr[i];
    }

    uint64_t fp_regs = (uint64_t) &uc->uc_mcontext.fp_regs;
    if ((((fp_regs | v_regs) & 31ul) != 0) || (l1p_pp_ctrl_save != 0))
    {
	// fp_regs or v_regs is not 32-byte aligned, or the list prefetcher
	// was paused.  In the former case, the signal handler has returned
	// with a ucontext different from what was set up in PushSignal().
	// The QPX restoration code to which we're about to return requires
	// 32-byte alignment, so instead of returning, we'll copy the
	// remaining state to the kthread structure and then take the
	// LaunchContext path.
	// We also take this path if we're restoring the list prefetcher
	// control state, because in that case we want to make all stack
	// references before possibly resuming the list.
	for (i = 14; i < 32; i++)
	{
	    kthr->Reg_State.gpr[i] = regs->gpr[i];
	}
	for (i = 0; i < 32; i++)
	{
	    kthr->Reg_State.qvr[i].ll[0] = ((uint64_t *) fp_regs)[i];
	    kthr->Reg_State.qvr[i].ll[1] = ((uint64_t *) v_regs)[(i*3)+0];
	    kthr->Reg_State.qvr[i].ll[2] = ((uint64_t *) v_regs)[(i*3)+1];
	    kthr->Reg_State.qvr[i].ll[3] = ((uint64_t *) v_regs)[(i*3)+2];
	}
	if (l1p_pp_ctrl_save != 0)
	{
	    uint64_t *l1p_pp_ctrl = (uint64_t *)
		(Kernel_L1pBaseAddress() + L1P_PP_CTRL +
		    (L1P_PP_THREADOFFSET * Kernel_ProcessorThreadID()));
	    (*l1p_pp_ctrl) = ~l1p_pp_ctrl_save; // saved value was complemented
	}
	LC_ContextLaunch(&kthr->Reg_State);
	// NOTREACHED
    }

    // non-volatile registers and QPX state will be restored after we return
    return uc;
}

void Signal_TrampolineInit(SignalTrampoline_t *tramp)
{
    asm("    .globl SignalTrampolineStart\n"
	"    .globl SignalTrampolineEnd\n"
	"    b SignalTrampolineEnd\n"
	"SignalTrampolineStart:\n"
	"    la %%r1,%0(%%r1)\n"
	"    li %%r0,%1\n"
	"    sc\n"
	"    trap\n"
	"SignalTrampolineEnd:\n"
	: : "i" (FRAME_OFFSET), "i" (__NR_rt_sigreturn)
	);

    extern char SignalTrampolineStart[], SignalTrampolineEnd[];
    
    assert((SignalTrampolineEnd - SignalTrampolineStart) == sizeof(tramp->instruction));
    memcpy(tramp->instruction, SignalTrampolineStart,
	    SignalTrampolineEnd - SignalTrampolineStart);
}

uint64_t Signal_Return()
{
    // We set a Pending flag to get the state to be restored from the ucontext
    // structure on the stack.
    GetMyKThread()->Pending |= KTHR_PENDING_SIGRETURN;
    return CNK_RC_SUCCESS(0);
}

uint64_t Signal_SetAction(int sig,
			  const struct kern_sigaction_t *act,
			  struct kern_sigaction_t *oldact)
{
    AppProcess_t *app = GetMyProcess();

    Kernel_Lock(&app->SignalLock);

    SignalData_t *sd = &app->SignalData[sig - 1];

    if (oldact != NULL)
    {
	memset(oldact, 0, sizeof(*oldact));
	oldact->sa_handler = (__sighandler_t) sd->handler.descriptor;
	oldact->sa_flags = sd->flags;
	oldact->sa_mask.sig[0] = sd->mask;
    }
    
    if (act != NULL)
    {
	if (act->sa_handler == SIG_IGN)
	{
	    sd->action = SIG_IGNORE;
	}
	else if (act->sa_handler == SIG_DFL)
	{
	    sd->action = DefaultSignalAction(sig);
	}
	else
	{
	    sd->action = SIG_HANDLER;
	    sd->handler.address = ((uint64_t *) (act->sa_handler))[0];
	    sd->handler.toc = ((uint64_t *) (act->sa_handler))[1];
	}

	sd->handler.descriptor = (uint64_t) act->sa_handler;
	sd->flags = act->sa_flags;
	sd->mask = (act->sa_mask.sig[0] & ~SIG_NONMASKABLE_SET);
    }

    Kernel_Unlock(&app->SignalLock);

    return CNK_RC_SUCCESS(0);
}

uint64_t Signal_SetMask(int how,
			const kern_sigset_t *set,
			kern_sigset_t *oldset)
{
    KThread_t *kthr = GetMyKThread();

    if (oldset != NULL)
    {
	oldset->sig[0] = kthr->MaskedSignals;
    }

    if (set != NULL)
    {
	switch (how)
	{
	    case SIG_BLOCK :
		kthr->MaskedSignals |= (set->sig[0] & ~SIG_NONMASKABLE_SET);
		break;
	    case SIG_UNBLOCK :
		kthr->MaskedSignals &= ~(set->sig[0] & ~SIG_NONMASKABLE_SET);
		break;
	    case SIG_SETMASK :
		kthr->MaskedSignals = (set->sig[0] & ~SIG_NONMASKABLE_SET);
		break;
	    default :
		return CNK_RC_FAILURE(EINVAL);
	}
    }

    if ((kthr->PendingSignals & ~(kthr->MaskedSignals)) != 0)
    {
	// There are now signals that can be delivered.
	kthr->Pending |= KTHR_PENDING_SIGNAL;
    }

    return CNK_RC_SUCCESS(0);
}

void Signal_ProcessInit(SignalData_t *sd)
{
    int i;
    for (i = 0; i < KERN_NSIG; i++)
    {
	sd[i].action = DefaultSignalAction(i+1);
	sd[i].handler.descriptor = 0;
	sd[i].handler.address = 0;
	sd[i].handler.toc = 0;
	sd[i].mask = 0;
	sd[i].flags = 0;
    }
}
