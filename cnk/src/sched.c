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
#include <hwi/include/bqc/BIC_inlines.h>
#include <hwi/include/bqc/wu_mmio.h>

/* found in spi/src/upci/upci_syscall.c */
__C_LINKAGE void Upci_SwapOut(void *pCfg);
__C_LINKAGE void Upci_SwapIn(void *pCfg);

struct kern_ucontext_t *PushSignal(KThread_t *kthr);


/*! \brief Resumes the highest priority runnable thread
 * Picks the next thread-to-run by reading the scheduler state and performing a count-leading-zeros opcode on the
 * scheduler state.  This opcode will quickly select (O(1) with very low constant overhead) the next thread.  It will
 * then update the current thread pointer and call _bgp_KThreadFullResume or _bgp_KThreadStatelessResume to launch the thread.
 *  \note Function does not return
 */
__C_LINKAGE void __NORETURN Scheduler( void )
{
    HWThreadState_t *pHWT = GetMyHWThreadState();

    uint32_t thread_select = 0;
    KThread_t *kthread_on_entry = pHWT->pCurrentThread;

    if (kthread_on_entry->SchedulerDivert != 0) 
    {
        if (kthread_on_entry->SchedulerDivert & SCHED_DIVERT_TOOL)
        {
            Tool_SignalEncountered(kthread_on_entry);
            kthread_on_entry->SchedulerDivert &= ~SCHED_DIVERT_TOOL;
        }
        // If we are continuing a signal that was intercepted by a tool we must redrive the pushing of the signal.
        if (kthread_on_entry->SchedulerDivert &
			    (SCHED_DIVERT_TERMINATE|SCHED_DIVERT_COREDUMP))
        {
	        AppProcess_t *proc = kthread_on_entry->pAppProc;
	        if (kthread_on_entry->SchedulerDivert & SCHED_DIVERT_COREDUMP)
	        {
		        proc->coreDumpOnExit   = 1;
		        proc->coredump_kthread = kthread_on_entry;
	        }
	        kthread_on_entry->SchedulerDivert &=
			    ~(SCHED_DIVERT_TERMINATE|SCHED_DIVERT_COREDUMP);
	        int sig = kthread_on_entry->SigInfoSigno;
            //printf("Sched. Calling AppExit divert\n");
	        App_ProcessExit(sig & 0x7F); 
	        // NOTREACHED
	    }
    }
    // Is there a pending speculation restart and are all pthreads on this hardware thread not suspended
    if (!pHWT->suspended)
    {
        Speculation_CheckPendingRestart();
    }
    /* save upci counter state if applicable */
    if (kthread_on_entry->pUpci_Cfg)
    {
        Upci_SwapOut(kthread_on_entry->pUpci_Cfg);
    }

    /* Switch to scheduler thread (if not already) */
    KThread_t *pKTh_Kernel  = pHWT->SchedSlot[CONFIG_SCHED_KERNEL_SLOT_INDEX];
    pHWT->pCurrentThread = pKTh_Kernel; // set current thread pointer

    // Refresh the SchedOrder object if needed
    if (pHWT->priorityRefreshPending)
    {
        pHWT->priorityRefreshPending = 0;
        KThread_refreshPriorityData(pHWT);
    }
    //Kernel_WriteFlightLog(FLIGHTLOG, FL_SCHDENTER,  KThread_getTID(kthread_on_entry),  // tid entering scheduler
    //                                                kthread_on_entry->Reg_State.ip,   // instruction pointer
    //                                                mfspr(SPRG_SPIinfo),              // spi info spr
    //                                                kthread_on_entry->Reg_State.lr);    // link register

    // Enable interrupts while we spin in the scheduler
    mtmsr(MSR_CNK | MSR_EE);
    isync();

    for ( ; ; )
    {
        // Find a kthread to dispatch
        for ( thread_select = CONFIG_SCHED_SLOT_FIRST ; thread_select < CONFIG_AFFIN_SLOTS_PER_HWTHREAD; thread_select++ )
        {
            int slotIndex = pHWT->SchedOrderEntry[thread_select] & 0xF;
            int priorityGroup = pHWT->SchedOrderEntry[thread_select] & 0xF0;
            KThread_t *pKThr_Run  = pHWT->SchedSlot[ slotIndex ];
            uint32_t state        = pKThr_Run->State;
            int rrobin;
            union
            {
                uint64_t SchedOrder;
                uint8_t  SchedOrderEntry[8];
            } temp;
            temp.SchedOrder = 0;
            if ( state == SCHED_STATE_RUN )
            {
                // Turn off interrupts while we prepare for the dispatch of the new thread
                mtmsr(MSR_CNK);
                isync();

                // Setup the physical pid for the new kthread. Avoid the mtspr and isync if the extended thread affinity facility is not enabled.
                if (pKThr_Run->pAppProc->ThreadModel == CONFIG_THREAD_MODEL_ETA)
                {
                    //printf("Dispatching tid=%d. Changing pid from %ld to %d\n", GetTID(pKThr_Run), mfspr(SPRN_PID), pKThr_Run->physical_pid);
                    mtspr(SPRN_PID, pKThr_Run->physical_pid); // Modify the PID for the current kthread

                    isync(); // ensure the instruction has completed before issuing any additional storage operations
                }
                // We are on a path to dispatch a thread. Raise our priority back to normal
                // !! note: this should be changed to preserve the previous hardware thread priority 
                ThreadPriority_Medium(); 
                // Perform round robin in preparation for the next dispatch
                // read the SchedOrder object and set a reservation 
                temp.SchedOrder = pHWT->SchedOrder;
                uint64_t origSchedOrder = temp.SchedOrder;
                // See if the next entry in SchedOrder is the same priority as our entry
                for (rrobin = thread_select+1; rrobin < CONFIG_AFFIN_SLOTS_PER_HWTHREAD; rrobin++)
                {
                    if (priorityGroup == (temp.SchedOrderEntry[rrobin] & 0xF0))
                    {
                        // move this entry into the position of the previous entry
                        temp.SchedOrderEntry[rrobin-1] = temp.SchedOrderEntry[rrobin];
                        // move the entry we are dispatching into the slot we just opened
                        temp.SchedOrderEntry[rrobin] = slotIndex + priorityGroup;
                    }
                    else
                        break; // we moved outside our range of equal priority entries
                }
                // Write the updated SchedOrder entry in the live HwThreadState object
                if (temp.SchedOrder != origSchedOrder)
                {
                    pHWT->SchedOrder = temp.SchedOrder;
                }
                // do a dummy stcx to drop any reservations we may be holding. If we are holding a reservation 
                // then the thread context we switch to could succeed on a stcx that it did not larx, resulting 
                // in atomicity problems. Also we do this dummy stwcx to ensure that any threads that were
                // stopped in a waitrsv will get awakened when control returns to them following a pre-emption.
                StoreConditional( pHWT->dummyStwcx.data, 0);
                // Load the context. Note that the return never occurs, the rfi at the end of the 
                // context load will transition this hardware thread to begin executing in the new context.
                Kernel_WriteFlightLog(FLIGHTLOG, FL_SCHEDDISP,  
                                      GetTID(pKThr_Run) |                        // TID  
                                      (slotIndex<<16) |                          // Slot index 
                                      (((uint64_t)pKThr_Run->physical_pid)<<32), // physical PID 
                                      pKThr_Run->Reg_State.ip,                   // instruction pointer
                                      mfspr(SPRG_SPIinfo),                       // active and runnable thread counts
                                      (temp.SchedOrder |                         // ordering mask (upper 5 bytes)
                                      pKThr_Run->Pending));                      // pending condition (lower 1 byte)


                dcache_block_touch( &(pKThr_Run->Reg_State) ); // start bringing in the context

                int ThdID  = ProcessorThreadID();
            
                // Set the new current thread pointer
                pHWT->pCurrentThread = pKThr_Run; 
            
                // Set the "fired" status for the WAC register used to implement the wait/wake SPIs. 
                // ThrdID 0,1,2,3 map to SPI WAC registers starting at the WAC defined in CNK_WAKEUP_SPI_FIRST_WAC
                WAKEUP_BASE[SET_THREAD(ThdID)] = _BN(ThdID) >> CNK_WAKEUP_SPI_FIRST_WAC;
            
                // Test to see if there are special modifications that must be done to the context prior to launch
                if (pHWT->launchContextFlags.dword)
                {
                    // Set stack guard registers if guarding enabled and if we are dispatching a different kthread
                    if (pHWT->launchContextFlags.flag.GuardActive && (kthread_on_entry != pKThr_Run))
                    {
                        WAKEUP_BASE[WAC_BASE( ThdID+CNK_STACK_GUARD_FIRST_WAC )] = pKThr_Run->GuardBaseAddress;
                        WAKEUP_BASE[WAC_ENABLE( ThdID+CNK_STACK_GUARD_FIRST_WAC ) ] = pKThr_Run->GuardEnableMask;
                    }
                    if (pHWT->launchContextFlags.flag.DebugControlRegs)
                    {
                        //printf("Setting debug regs %016lx %016lx %016lx %016lx\n",pKThr_Run->Reg_State.dbcr0,pKThr_Run->Reg_State.dbcr1,pKThr_Run->Reg_State.dbcr2,pKThr_Run->Reg_State.dbcr3);
                        mtspr(SPRN_DBCR0,pKThr_Run->Reg_State.dbcr0);
                        mtspr(SPRN_DBCR1,pKThr_Run->Reg_State.dbcr1);
                        mtspr(SPRN_DBCR2,pKThr_Run->Reg_State.dbcr2);
                        mtspr(SPRN_DBCR3,pKThr_Run->Reg_State.dbcr3);
                    }
                }
                ppc_msync (); 
            
                /* restore upci counter state if applicable */
                if (pKThr_Run->pUpci_Cfg)
                {
                    Upci_SwapIn(pKThr_Run->pUpci_Cfg);
                }
                LC_ContextLaunch(&(pKThr_Run->Reg_State));
            }
            else
            {
                continue;  // This thread is not runnable or does not exist, try next.
            }
        }  // rof thread_select


        // If we reach here, no runnable KThread was found (except for the Kernel thread). 
        ppc_wait(); 
        // In rare cases, may reach here. External interrupt conditions occuring on the wait will result in a call to the top of the scheduler
        // Continue this flow, looping back to interrogate the kthread states to dispatch the most eligible thread

    } // rof forever
}

/*! \brief Update scheduler state for calling thread, then invoke scheduler.
 * \param[in] new_state Bitmask of reason(s) why the thread is no longer runnable.
 * \note The hardware thread target of the block operation must later call Scheduler() to perform the block function. 
 */ 
__C_LINKAGE void Sched_Block(KThread_t *pKThr, uint32_t block_state )
{

    uint64_t spiInfo = mfspr(SPRG_SPIinfo);
    HWThreadState_t *myHWT = GetMyHWThreadState();
    if (pKThr->pHWThread == myHWT)
    {
        if ( block_state )
        {
            if (pKThr->State == SCHED_STATE_RUN) // are we comming from a runnable state?
            {
                spiInfo -= (1 << SPRG_SPIinfo_Runnable); // decrement the counter in the SPR that represents runnable threads
                mtspr(SPRG_SPIinfo, spiInfo);
                //printf("(decr runnable. blockstate %08x\n", block_state);

            }
            if (block_state == SCHED_STATE_RESET) // are we entering the RESET state?
            {
                // This is a thread that once existed and is now gone. 
                spiInfo -= (1 << SPRG_SPIinfo_NumThds); // Decrement counter in the SPR that represents the number of threads
                mtspr(SPRG_SPIinfo, spiInfo);
            }
            pKThr->State |= block_state;
        }
        Kernel_WriteFlightLog(FLIGHTLOG, FL_SCHEDBLKL, GetTID(pKThr), pKThr->Reg_State.ip, spiInfo,block_state);
    }
    else
    {
        Kernel_WriteFlightLog(FLIGHTLOG, FL_SCHEDBLKR, 
                              GetTID(pKThr) + GetTID(myHWT->pCurrentThread)*1000, // put the "from" and "to" TIDs in the data 
                              myHWT->pCurrentThread->Reg_State.ip,  spiInfo, block_state);
        // Send an IPI to the hardware thread that contains the target kthread
        IPI_block_thread(pKThr->ProcessorID, pKThr, block_state);
    }
}


/*! \brief Update scheduler state for indicated thread by clearing a blocking condition.
 * \param[in] pKThr Pointer to thread-to-wake
 * \param[in] new_state Reason to unblock the thread. Thread is runnable when there are no reasons to block.
 */
__C_LINKAGE void Sched_Unblock( KThread_t *pKThr, uint32_t unblock_state )
{

    if ( unblock_state )
    {
        // Determine if we are trying to wake a thread on our own hardware thread
        uint64_t spiInfo = mfspr(SPRG_SPIinfo);
        HWThreadState_t *myHWT = GetMyHWThreadState();
        if (pKThr->pHWThread == myHWT)
        {
            int sendsignal = 0;

            // If the state we are clearing is specifically the RESET state, then we must bump the value that represents the
            // number of software threads on this hardware thread within the SPR that contains the Kernel SPIinfo
            if (unblock_state == SCHED_STATE_RESET)
            {
                spiInfo += (1 << SPRG_SPIinfo_NumThds); // bump the counter in the SPI SPR that represents the number of threads
                mtspr(SPRG_SPIinfo, spiInfo);

                AppProcess_t *proc = pKThr->pAppProc;
                if (proc && (proc->ProcessLeader_KThread == pKThr)) // Is this the kthread of the process leader
                {
                    if (proc->State==ProcessState_RankInactive)
                    {
                        // This rank is not active. allow it to prematurely enter the exit flow
                        proc->State = ProcessState_ExitPending;
                        App_Exit(AppExit_Phase1, 1); 
                        // Previous call does not return
                    }
                    else if (proc->State==ProcessState_AgentInactive)
                    {
                        // This rank is not active. allow it to prematurely enter the exit flow
                        proc->State = ProcessState_ExitPending;
                        App_AgentExit(AppExit_Phase1); 
                        // Previous call does not return
                    }
                    else if (proc->State != ProcessState_Active)
                    {
                        printf("(E) Unexpected process state %d in Sched_Unblock()", proc->State);
                    }
                    else
                    {
                        // Set the process state to indicate that we are starting a process for the first time
                        proc->State = ProcessState_Started;
                        // If we are starting an app with a tool active, additional setup may be required
                        sendsignal = Tool_AppStart(pKThr); 
                    }
                }
                else
                {
                    // This is not the main thread of a process. This unblock of the reset state is due to either a clone syscall or a
                    // migration due to a sched_setaffinity syscall.
                    if (pKThr->Reg_State.dac1)
                    {
                        //printf("Sched unblock reset. Setting DAC1 %016lx\n", pKThr->Reg_State.dac1);
                        mtspr(SPRN_DAC1, pKThr->Reg_State.dac1);
                        myHWT->launchContextFlags.flag.DebugControlRegs = 1;
                    }
                    if (pKThr->Reg_State.dac2)
                    {
                        //printf("Sched unblock reset. Setting DAC2 %016lx\n", pKThr->Reg_State.dac2);
                        mtspr(SPRN_DAC2, pKThr->Reg_State.dac2);
                        myHWT->launchContextFlags.flag.DebugControlRegs = 1;
                    }
                    if (pKThr->Reg_State.dac3)
                    {
                        mtspr(SPRN_DAC3, pKThr->Reg_State.dac3);
                        myHWT->launchContextFlags.flag.DebugControlRegs = 1;
                    }
                    if (pKThr->Reg_State.dac4)
                    {
                        mtspr(SPRN_DAC4, pKThr->Reg_State.dac4);
                        myHWT->launchContextFlags.flag.DebugControlRegs = 1;
                    }
                }
            }
            else if (unblock_state == SCHED_STATE_SUSPEND)
            {
                // Are all software threads on this hardware thread suspended?
                if (myHWT->suspended)
                {
                    // Indicate that all threads are no longer suspended
                    myHWT->suspended = 0;

                    // Is there a pending speculation restart
                    Speculation_CheckPendingRestart();
                }
            }
            // Modify the kthread state
            uint32_t prevState = pKThr->State;
            // TEMP PROBLEM ANALYSIS START
            //if (unblock_state == SCHED_STATE_FUTEX)
            //{
            //   pKThr->pad3 = 0;
            //}
            // TEMP PROBLEM ANALYSIS END
            pKThr->State &= ~unblock_state;
            if ((prevState != SCHED_STATE_RUN) && (pKThr->State == SCHED_STATE_RUN)) // are we now runnable?
            {
                spiInfo += (1 << SPRG_SPIinfo_Runnable); // bump the counter in the SPI SPR that represents runnable threads
                mtspr(SPRG_SPIinfo, spiInfo);
                //printf("(bump runnable. unblock state %08x\n", unblock_state);
            }
            Kernel_WriteFlightLog(FLIGHTLOG, FL_SCHEDUNBL,GetTID(pKThr),pKThr->Reg_State.ip, spiInfo, unblock_state);

            if (sendsignal)
            {
                Signal_Deliver(pKThr->pAppProc, GetTID(pKThr), sendsignal);
            }
        }
        else 
        {
            Kernel_WriteFlightLog(FLIGHTLOG, FL_SCHEDUNBR,
                                  GetTID(pKThr) + GetTID(myHWT->pCurrentThread)*1000, // put the "from" and "to" TIDs in the data 
                                  myHWT->pCurrentThread->Reg_State.ip, spiInfo, unblock_state);
            // Send an IPI to the target hardware thread
            // note: if the target is in a hwthread wait, the following IPI interrupt will awaken the thread

            // TEMP PROBLEM ANALYSIS START
            //if (unblock_state == SCHED_STATE_FUTEX)
            //{
            //    pKThr->pad3 |= 0x8; // dont disturb the values set when the vaddr was zeroed, but indicate the ipi was issued.
            //}
            // TEMP PROBLEM ANALYSIS END

            IPI_run_scheduler(pKThr->ProcessorID, pKThr, unblock_state);
        }
    }
}

// Initialize the state and rebuild the list of all application kthreads controlled by the scheduler on this 
// hardware thread. During the execution of the application, affinity operations may have moved around the
// KThreads to different hardware threads.
__C_LINKAGE void Sched_Reset()
{
    // Rebuild the list of KThreads managed by this hardware thread and initialize all associated objects
    KThread_InitHwThread();
    
    // Initialize SPR used for Kernel Sched Snoop data. Reset numthreads and numrunnable fields
    uint64_t spiInfo = mfspr(SPRG_SPIinfo); 
    uint64_t numthreads =  (0xFF << SPRG_SPIinfo_NumThds); 
    uint64_t numrunnable = (0xFF << SPRG_SPIinfo_Runnable);
    spiInfo &= ~(numthreads | numrunnable);
    mtspr(SPRG_SPIinfo, spiInfo);
    // ensure debug control register is reset
    mtspr(SPRN_DBCR0,0);
}



// Set all Kthreads on this hardware thread to the Blocked for AppExit state.
void Sched_BlockForExit()
{
    int core = ProcessorCoreID();
    int hwtid = ProcessorThreadID();
    CoreState_t *pCS = &(NodeState.CoreState[core]);
    HWThreadState_t *pHWT = &(pCS->HWThreads[hwtid]);
    AppProcess_t *hwtproc = GetProcessByProcessorID(ProcessorID());

    uint32_t slot;
    for ( slot = 0 ; slot < CONFIG_SCHED_KERNEL_SLOT_INDEX ; slot++)
    {
        KThread_t *kthread = pHWT->SchedSlot[slot];
        if (kthread && (kthread->pAppProc == hwtproc))
        {
            // Set the blocking code, but do not destroy the existing blocking codes since these 
            // are tested by the coredump code to determine if a kthread should be dumped.
            kthread->State |= SCHED_STATE_APPEXIT;
        }
    }
}


