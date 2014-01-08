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

#include <assert.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>

#include "SigHandler.h"
#include "globals.h"
#include "spi/include/kernel/debug.h"
//#include "spi/include/upci/testlib/upci_debug.h"


//#define INCL_SIGTRACE
// We don't normally want to do prints from a signal handler, because prints are not
// reentrant - and can cause seg faults - though I've never had it actually happen.
// However, when debug is enabled, other bgpm functions are printing, since they don't know it's in a signal
// handler, so might as well include the routines which make it clear we are in the signal handler.
#ifdef INCL_SIGTRACE
  #if (BGPM_TRACE_LEVEL > 0)
    #define SIG_TRACE( _cmd_ )  BGPM_TRACE_DATA_L2( _cmd_ )
  #else
    #define SIG_TRACE( _cmd_ )  _cmd_
  #endif
#else
    #define SIG_TRACE( _cmd_ )
#endif


using namespace bgpm;

/*
Shared Overflow Algorithm Notes
1   Only current thread evtset knows which are it's counters, which of it's counters have overflow active,
    and what are the threshold values.
2   The "any" and "upcc" thread int status bits are sticky, and stay active until reset by the particular
    threads signal handler, to make sure the interrupt is seen.
3   Stop thread's counters at start of signal handler,
    restore original run state of counters at end
4   Get active int status at start - then process only those counters indicated.
    Any new counter overflows during processing will remain active and prevent the "any" thread
    and UPC_C thread interrupt status bits from being reset (will get subsequent interrupt)
5   Queue the int status to be seen by all thread signal handlers,
    which might include int status for counters belonging to another thread.
6   More counters might overflow while processing, and this thread won't know about it.
    Still, all of the ctr int status captured at the start
    must be reset to prevent a subsequent signal by another thread from thinking
    a new overflow has occurred.
7   Only the int status from what is captured at start will be reset,
    so any new counter interrupt status bits will remain active.
8   Only thresholds known by current thread are reset.
9   Thus, because the int status for other overflowing counters is still being reset
    by this thread, those counters may unfreeze and continue to increment
    a residual value past the threshold.
9   When the other thread (which owns the counter with the residual count) is signaled,
    it will use the queued interrupt status to find the overflowed counter
    and reset that counters threshold (while also accumulating the residual count in software).
*/


extern "C" void Bgpm_SigHandler(int sig, siginfo_t *si, void *ctx)
{
    // stop this event sets active punit counters.
    // They will normally already be frozen except if counter groups are "shared"
    // between threads (more than 6 counters per thread, or L2/IO interrupts)
    uint64_t runningPuCtrMask = 0;   // keep track if (and which) upc_p counters are running.
    uint64_t puCtrMask = 0;          // which upc_p counters are being controlled by this thread.
    bool applicableOvflow = false;   // Did we find an overflow applicable to this thread.
    EvtSet *puEvtSet = NULL;         // what's the active punit event set object for this thread.
    if (swModeAppliedPuEvtSet >= 0) {
        runningPuCtrMask = upc_p_local->control;
        puEvtSet = procEvtSets[swModeAppliedPuEvtSet];
        puEvtSet->SignalHandlerIn();
        puCtrMask = puEvtSet->GetActiveCtrMask();
        upc_p_local->control_w1c = puCtrMask;  // make sure the evtset ctrs are stopped
        runningPuCtrMask &= puCtrMask;         // running mask should reflect only this threads ctrs
        mbar();
    }

    ucontext_t *uc = (ucontext_t *) ctx;
    uint64_t ipaddr = (uint64_t)(uc->uc_mcontext.regs->nip);
    unsigned thd = Kernel_ProcessorThreadID();
    unsigned core = Kernel_ProcessorCoreID();
    uint64_t upcpThdIntStatus = upc_p_local->int_status[thd];

    // Set to clear thread interrupt status bits when done.
    //
    // upcpThdIntStatus and UPC_C interrupt status bits are used to indicate which
    // counters overflowed.  The value is trimmed of bits as each interrupt type is handled.
    // If new counter overflows in the
    // meantime, it would not be represented in the upcpThdIntStatus bits, and since it
    // are not reset, the new int status will hold the thread int status bits active even
    // as we attempt to reset later (causing a new interrupt as desired).
    //
    // While upcpThdIntStatus is trimmed as we go along, the upcpIntStatusClear
    // value is built up as we progress to indicate which bits need to be reset.
    // Always attempt to clear upc_c and int on any thread sticky bits.
    uint64_t upcpIntStatusClear = UPC_P__INT_STATUS__THREAD_STATUS(thd);
    dbgThdVars.numPmSignals++;


    SIG_TRACE(
       fprintf(stderr, "%s" _AT_
       " Bgpm_SigHandler:  hwThd %02d, sig %02d, si %p, ctx %p, "
       "ipaddr %p, upcpThdIntStatus=0x%016lx, runningPuCtrMask=0x%016lx, upc_p->p_config=0x%016lx\n",
       IND_STRG, Kernel_ProcessorID(), sig, si, ctx, (void*)ipaddr,
       upcpThdIntStatus, runningPuCtrMask, upc_p_local->p_config)
       );

    // only keep the any int overflow status bits and upc_c overflow status bits for the current thread.
    // Each thread is responsible for these themselves.
    // But, keep all counter status bits so the 1st thread on shared overflows (when the 'any' status bit are set)
    // will still reset these status bits after capturing the results in the shared queue.
    upcpThdIntStatus &= UPC_P__INT_STATUS__COUNTER_STATUS | UPC_P__INT_STATUS__THREAD_STATUS(thd);

    // ---------------------------------------
    // Check for UPC_C Overflow
    if (UNLIKELY(upcpThdIntStatus & UPC_P__INT_STATUS__UPC_C_THREAD_INT_bit(thd))) {

        //fprintf(stderr, "%s" _AT_ " upccIntStatus = 0x%016lx, ioIntStatus = 0x%016lx\n", IND_STRG, upccIntStatus, ioIntStatus);

        SharedControlL2 & controlL2 = pShMem->controlL2;
        SharedControlIO & controlIO = pShMem->controlIO;
        // just use the L2 leader lock - really only need one.
        if (LLATCH_LEADER == LeaderLatch2(&controlL2.ovfQue.leaderLock, &upc_c->ccg_int_status_w1s, &upc_c->io_ccg_int_status_w1s)) {

            // Clear the status 1st, as we will read counters afterwards and want to make sure we lose a new overflow
            uint64_t upccIntStatus = upc_c->ccg_int_status_w1s;
            uint64_t ioIntStatus = upc_c->io_ccg_int_status_w1s;
            controlL2.EnqueIfActiveOvf(upccIntStatus);
            controlIO.EnqueIfActiveOvf(ioIntStatus);
            LeaderUnLatch(&controlL2.ovfQue.leaderLock);
        }

        dbgThdVars.numUpcCOvfs++;
        applicableOvflow = controlL2.DequeAndCallOvfHandler(swModeAppliedL2EvtSet, ipaddr, uc);
        applicableOvflow = controlIO.DequeAndCallOvfHandler(swModeAppliedIOEvtSet, ipaddr, uc) || applicableOvflow;
     }



    //fprintf(stderr, "%s" _AT_ " upcpIntStatusClear = 0x%016lx, upcpThdIntStatus = 0x%016lx\n", IND_STRG, upcpIntStatusClear, upcpThdIntStatus);


    // ---------------------------------------------------
    // Check for fast but shared upc_p overflow
    upcpThdIntStatus &= ~UPC_P__INT_STATUS__UPC_C_THREAD_INT_set(0xFUL);  // ensure no upc_c int status visable
    if (UNLIKELY(upcpThdIntStatus & UPC_P__INT_STATUS__ANY_OVF_THREAD_INT_bit(thd))) {

        SharedOvfQueue & ovfQue = pShMem->coreOvfQue[core];
        //fprintf(stderr, "%s" _AT_ " upcpIntStatusClear = 0x%016lx, upcpThdIntStatus = 0x%016lx\n", IND_STRG, upcpIntStatusClear, upcpThdIntStatus);
        unsigned intDroppedStart = ovfQue.sharedOvfDropped;
        if (LLATCH_LEADER == LeaderLatch(&ovfQue.leaderLock, &(upc_p_local->int_status[thd]), UPC_P__INT_STATUS__COUNTER_STATUS)) {
            //fprintf(stderr, "%s" _AT_ " upcpIntStatusClear = 0x%016lx, upcpThdIntStatus = 0x%016lx\n", IND_STRG, upcpIntStatusClear, upcpThdIntStatus);

            // replace current counter interrupt status
            upcpThdIntStatus = upc_p_local->int_status[thd] & UPC_P__INT_STATUS__COUNTER_STATUS;
            ovfQue.Enque(upcpThdIntStatus);
            // Clear current counter int status bits
            upc_p_local->int_status_w1c = upcpThdIntStatus;
            upcpThdIntStatus &= UPC_P__INT_STATUS__LTLINT_STATUS;  // clear handled interrupts (leave big int active)
            LeaderUnLatch(&ovfQue.leaderLock);
        }

        //fprintf(stderr, "%s" _AT_ " upcpIntStatusClear = 0x%016lx, upcpThdIntStatus = 0x%016lx\n", IND_STRG, upcpIntStatusClear, upcpThdIntStatus);

        // Now process the queue entries till at end
        // Be careful: if multi-thread muxing (broadcasted core interrupts) the int status is recorded
        //  for all threads, but the mux group and counters definitions are switched independently.
        //  Thus, processed int status must only apply to current hw thread. Underlying functions
        //  should be insuring that.
        dbgThdVars.numSharedUpcpOvfs++;

        if (puEvtSet) {
            Bgpm_OverflowHandler_t pHandler = puEvtSet->GetOverflowHandler();

            uint64_t muxStatus = 0;   // accumulate mux counter Interrupt status to do switch only once in case multiple records in queue
            unsigned curQueLen = 0;
            uint64_t allIntMask = 0;  // get accumulated mask if multiple entries.
            bool again;
            do {
               again = false;
               OvfQueEntry rec = ovfQue.Deque(thd);
               if (rec.recId != 0) {
                   applicableOvflow = true;
                   curQueLen++;
                   // reset this evt sets thresholds
                   bool doOvf = puEvtSet->AccumResetThresholds(rec.intStatus);
                   muxStatus |= puEvtSet->GetMuxIntStatus(rec.intStatus);
                   uint64_t upcpUsrOvfStatus = rec.intStatus & ~muxStatus;

                   if (pHandler && doOvf && upcpUsrOvfStatus) {
                       dbgThdVars.numSharedUpcpOvfsDelivered++;
                      (*pHandler)(swModeAppliedPuEvtSet, ipaddr, upcpUsrOvfStatus, uc);
                   }
                   allIntMask |= rec.intStatus;
                   again = true;
               }
            } while (again);

            if (muxStatus) {
                dbgThdVars.numMuxOvfs++;
                puEvtSet->SwitchMuxGrp();
                runningPuCtrMask = puEvtSet->GetActiveCtrMask();  // switch restart to new punit
            }
            if (intDroppedStart != ovfQue.sharedOvfDropped) {
                dbgThdVars.droppedSharedUpcPOvfs++;
            }
            dbgThdVars.maxUpcPSharedQueLen = UPCI_MAX(dbgThdVars.maxUpcPSharedQueLen, curQueLen);
        }

        upcpIntStatusClear |= upcpThdIntStatus | UPC_P__INT_STATUS__ANY_OVF_THREAD_INT_bit(thd);
        upcpThdIntStatus &= UPC_P__INT_STATUS__BIGINT_STATUS; // keep big ints

        //fprintf(stderr, "%s" _AT_ " upcpIntStatusClear = 0x%016lx, upcpThdIntStatus = 0x%016lx\n", IND_STRG, upcpIntStatusClear, upcpThdIntStatus);
    }

    // -----------------------------------------------------
    // handle the simple fast interrupt status that's left
    else if (upcpThdIntStatus & UPC_P__INT_STATUS__LTLINT_STATUS) {
        upcpThdIntStatus &= UPC_P__INT_STATUS__COUNTER_STATUS; // ensure only ltl & big non-shared int status visable

        if (puEvtSet) {

            // Handle Punit Overflows
            applicableOvflow = true;
            bool doOvf = puEvtSet->AccumResetThresholds(upcpThdIntStatus);
            Bgpm_OverflowHandler_t pHandler = puEvtSet->GetOverflowHandler();
            uint64_t muxStatus = puEvtSet->GetMuxIntStatus(upcpThdIntStatus);
            uint64_t upcpUsrOvfStatus = upcpThdIntStatus & ~muxStatus;

            if (pHandler && doOvf && upcpUsrOvfStatus) {
                dbgThdVars.numFastOvfs++;
               (*pHandler)(swModeAppliedPuEvtSet, ipaddr, upcpUsrOvfStatus, uc);
            }

            if (muxStatus) {
                dbgThdVars.numMuxOvfs++;
                //fprintf(stderr, _AT_ " Switch\n");  // <<<<<<<<<<<<<<<<<<<<<
                puEvtSet->SwitchMuxGrp();
                runningPuCtrMask = puEvtSet->GetActiveCtrMask();  // switch restart to new punit
            }
        }


        // generate value to Clear these Counters and this Threads Interrupt Status
        upcpIntStatusClear |= upcpThdIntStatus | UPC_P__INT_STATUS__ANY_OVF_THREAD_INT_bit(thd);
        upcpThdIntStatus &= UPC_P__INT_STATUS__BIGINT_STATUS; // keep big ints

        //fprintf(stderr, "%s" _AT_ " upcpIntStatusClear = 0x%016lx, upcpThdIntStatus = 0x%016lx\n", IND_STRG, upcpIntStatusClear, upcpThdIntStatus);

    }


    // ------------------------------------
    // handle big overflows - on any big overflow - let's leave pm interrupts disabled.
    // We are done processing overflows of any kind.
    if (UNLIKELY(upcpThdIntStatus & UPC_P__INT_STATUS__BIGINT_STATUS)) {
        applicableOvflow = true;

        char detail[1024];
        snprintf(detail, 1024, _AT_ " hwthd=%d, upcpIntStatusClear=0x%016lx, remaining upcpThdIntStatus=0x%016lx",
                thd, upcpIntStatusClear, upcpThdIntStatus);
        Kernel_InjectASCIIRAS(BGPM_RAS_BIGOVF_ERR, (uint8_t*)detail);

        // clear the interrupt status, but don't call Kernel_Upci_ResetInts to reenable the interrupt masks which
        // allow interrupts to be delivered.
        upc_p_local->int_status_w1c = upcpIntStatusClear | upcpThdIntStatus;

        SIG_TRACE(
                fprintf(stderr, "%s" _AT_ " upcpIntStatusClear = 0x%016lx, upcpThdIntStatus = 0x%016lx\n", IND_STRG, upcpIntStatusClear, upcpThdIntStatus)
                );
        //assert(0);
    }
    else {
        // Reset Interrupt status and reenable interrupt
        // Required to do syscall to make atomic between threads
        Kernel_Upci_ResetInts(upcpIntStatusClear);
        SIG_TRACE(
                fprintf(stderr, "%s" _AT_ " upcpIntStatusClear = 0x%016lx, upcpThdIntStatus = 0x%016lx\n", IND_STRG, upcpIntStatusClear, upcpThdIntStatus)
                );
    }



    if (!applicableOvflow) dbgThdVars.numPmSigsWOInt++;

    SIG_TRACE(
        fprintf(stderr, "%s" _AT_ " upc_p->p_config=0x%016lx, upc_p->int_status[%d]=0x%016lx, runningPuCtrMask=0x%016lx\n", IND_STRG,
                upc_p_local->p_config, thd, upc_p_local->int_status[thd], runningPuCtrMask)
    );

    // debug instructions needed to return from syscall
    //upc_p_local->control_w1c = 0x00000000FFFFFFFFULL; mbar();
    //Upci_Delay(800);
    //uint64_t cnt2 = upc_c->data24.grp[0].counter[8];
    //uint64_t thres2 = UPCI_PERIOD2THRES(100000);
    //uint64_t pthres2 = upc_p_local->counter[8];
    //fprintf(stderr, "%s" _AT_ " cnt2=0x%016lx, thres2=0x%016lx, pthres2=0x%016lx, instrs=%ld\n", IND_STRG, cnt2, thres2, pthres2, (uint64_t)(cnt2 - thres2));


    // restart counters for this thread
    if (pShMem->controlIO.GetRunning()) {
        UPC_IO_Start(nodeConfig);
    }
    if (pShMem->controlL2.GetRunning()) {
        UPC_C_Start_Sync_Counting();  // start L2 without delay
    }
    if (puEvtSet) {
        puEvtSet->SignalHandlerOut();
    }
    if (runningPuCtrMask) {
        upc_p_local->control_w1s = runningPuCtrMask;
        mbar();
    }
    //UPC_P_Dump_State(-1);

}



int bgpm::RegisterSigHandler(int sig)
{
    struct sigaction sa;

    sa.sa_sigaction = Bgpm_SigHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;

    int ret = sigaction(sig, &sa, NULL);
    if (ret < 0) {
        return lastErr.PrintOrExitOp(-errno, "sigaction", BGPM_ERRLOC);
    }

    ret = Kernel_Upci_SetPmSig(sig);
    if (ret < 0) {
        return lastErr.PrintOrExitOp(ret, "Kernel_Upci_SetPmSig", BGPM_ERRLOC);
    }

    return 0;
}
