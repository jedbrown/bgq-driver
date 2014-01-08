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
#include <spi/include/upci/upci_syscall.h>

// Locking to control a syscalls that use inter-processor interrupts (IPIs)
Lock_Atomic_t syscallIpiControl;

volatile int64_t ackIpi[CONFIG_MAX_HWTHREADS];

// Cleanup pending IPI interrupt conditions from other application threads and application agents that may exist in 
// the current thread. Note that all application threads and agents must be in a known state in which they will not be 
// attempting to send IPI interrupts while this cleanup is running. We are purposely not cleaning up the interface 
// between the job controller thread and this thread.
void ipi_cleanup()
{
    uint32_t myProcId = ProcessorID();
    int i;
    for (i=0; i<(CONFIG_MAX_APP_THREADS+CONFIG_MAX_APP_AGENTS); i++)
    {
        IPI_Message_t* pIPImsg = (IPI_Message_t*)&(NodeState.CoreState[i/4].HWThreads[i%4].ipi_message[myProcId]);
        pIPImsg->fcn = NULL;
        pIPImsg->param1 = 0;
        pIPImsg->param2 = 0;
    }
    BIC_WriteClearExternalRegister0(myProcId, (uint64_t)(-1)); // Clear all pending IPI interrupts from other application threads
    BIC_WriteClearExternalRegister1(myProcId, (uint64_t)(0xC));// Clear all pending IPI interrupts from application agents (hwtidss 64,65)                    

}

// Deliver an interprocessor interrupt - utility function
//      Parm1: target thread index. Valid values 0 through 67
//      Parm2: function pointer to be called 
//      Parm3: parameter supplied to the called function
IPIHANDLER_Fcn_t IPI_DeliverInterrupt(int processorID, IPIHANDLER_Fcn_t handler, uint64_t parm1, uint64_t parm2 )
{
    Kernel_WriteFlightLog(FLIGHTLOG, FL_DELIVRIPI, processorID, (uint64_t)handler, parm1, parm2);
    
    //printf("Sending IPI to threadid %d\n", thread_index);
    // Validate the target
    if ((processorID) >= 0 && (processorID < CONFIG_MAX_CORES*CONFIG_HWTHREADS_PER_CORE))
    {
        // Get my hardware thread state object
        HWThreadState_t *pHwt = GetHWThreadStateByProcessorID(ProcessorID());
        // Determine if there is a previous request that has not completed
        if (pHwt->ipi_message[processorID].fcn)
        {
            // If the target hwthread is the same as our hardware thread, do not deadlock.
            // instead return the function pointer of the IPI that has not been handled.
            // If the caller is sending an IPI to itself, it must be prepared to handle this situation
            if (processorID == ProcessorID())
                return (pHwt->ipi_message[processorID].fcn);

            // Lower our hwthread priority and spin waiting for the target thread to process a previous request 
            ThreadPriority_Low();
            while(pHwt->ipi_message[processorID].fcn) 
            {
                // Is the target hardware thread beyond the point of accepting interrupts
                if (pHwt->appExitPhase == AppExit_Phase2)
                {
                    // Just return. The target thread is essentially gone. 
                    // We should not be here if an IPI is being delivered from/to application or agent threads 
                    // of a job since in those conditions, the application processes and agents would not have 
                    // exited phase 1 of AppExit. We should only be here if the Tool control thread is attempting 
                    // to send an IPI to a thread that has since entered phase 2 of AppExit.
                    ThreadPriority_Medium();
                    return NULL;
                }
                // Is a process_exit message pending against this hwthread and is the kthread which is attempting this IPI delivery 
                // in the same process as the target of the exit operation? Also, are we trying to send an IPI to the same hardware 
                // thread that has sent the IPI exit message to us? If all of these are true, we need to get out of the way since the 
                // sender of the process_exit IPI will be waiting in a barrier for all of the threads of it's process to arrive. 
                // Just abort this IPI since at this point there is nothing more important than allowing the exit to proceed.
                int i;
                for (i=0; i<64; i++)
                {
                    IPI_Message_t* pIPImsg = (IPI_Message_t*)&(NodeState.CoreState[i/4].HWThreads[i%4].ipi_message[ProcessorID()]);
                    if ((pIPImsg->fcn == IPI_handler_process_exit) &&  // Is there a process exit message pending to this hardare thread?
                        (GetProcessByProcessorID(ProcessorID()) == GetMyKThread()->pAppProc) && // Are we running in a kthread that is part of this hardware thread's process?
                        (i == processorID)) // Are we trying to send and IPI to the same hardware thread that has sent the IPI exit message to us?
                    {
                        // This is a situation that will not clear on its own. Toss the delivery of this IPI and allow the exit to proceed.
                        ThreadPriority_Medium();
                        return NULL;
                    }
                }
                // Is there an IPI pending directed to us which requires an ACK by the sender. If this is true, we must process the
                // pending request so that the target can proceed to an interruptable point and accept the previously pending request
                // thereby allowing us to make our request pending.
                IPI_DeadlockAvoidance(processorID);
            }
            ThreadPriority_Medium();
        }
        pHwt->ipi_message[processorID].fcn = handler;
        pHwt->ipi_message[processorID].param1 = parm1;
        pHwt->ipi_message[processorID].param2 = parm2;
        ppc_msync();
        // Write to the c2c send register
        BIC_REGISTER send_value = 0;
        send_value |= ((processorID & 0x3)+1);          // Set the thread index value (values 1 through 4)
        send_value |= ((BIC_C2C_INTTYPE_EXTERNAL) << 3); // Indicate delivery as an External interrupt
        send_value |= (_BN(processorID>>2)) >> 42;      // Set the core mask
        BIC_WriteInterruptSend(send_value);
    }
    return NULL;
}

// Deliver an interprocessor wakeup
void IPI_DeliverWakeup(int processorID)
{
    //printf("Sending wakeup to threadid %d\n", thread_index);
    // Write to the c2c send register
    BIC_REGISTER send_value = 0;
    send_value |= ((processorID & 0x3)+1);          // Set the thread index value (values 1 through 4)
    send_value |= ((BIC_C2C_INTTYPE_WAKE) << 3); // Indicate delivery as a wakeup event (no interrupt)
    send_value |= (_BN(processorID>>2)) >> 42;      // Set the core mask
    //printf("Sending wakeup to thread %d. Send Reg: %016lx\n", thread_index, (uint64_t)send_value);
    BIC_WriteWakeupSend(send_value);
}

// Acknowledge IPI wakeup. 
//    Bitmap of source hwthreads 0-63 stored in location pointed to by parameter 1.
//    Bitmap of source hwthreads 64-67 stored in location pointed to by parameter 2 (bits 60-63 of the register).
//    Return values:
//         0: no IPI wakeup requests were found pending.
//         1: one or more wakeup requests were found pending.
int IPI_AcknowledgeWakeup(uint64_t *mask1, uint64_t *mask2)
{
    int myProcessorThreadID = ProcessorThreadID();
    BIC_REGISTER bicreg1 = BIC_ReadStatusWakeupRegister0(myProcessorThreadID);
    BIC_REGISTER bicreg2 = BIC_ReadStatusWakeupRegister1(myProcessorThreadID);
    if (mask1) { *mask1 = bicreg1; }
    if (mask2) { *mask2 = bicreg2; }
    BIC_WriteClearWakeupRegister0(myProcessorThreadID, bicreg1);
    BIC_WriteClearWakeupRegister1(myProcessorThreadID, bicreg2);
    ppc_msync();
    return (bicreg1 || bicreg2) ? 1 : 0;
}

//----------------------------------------------------
//    Set Affinity handler/requestor functions
//----------------------------------------------------

// Set Processor Affinity Handler
void IPI_handler_setaffinity(uint64_t parm1, uint64_t parm2)
{
    KThread_t *kthread = (KThread_t *)parm1;
    // Test to see if the process is still active

    if (!IsProcessActive(kthread))  
    {
        return;
    }
    // Set to restart location if speculation is active. Do not defer the specid invalidation.
    Speculation_CheckJailViolation(&GetMyKThread()->Reg_State);
    
    //printf("In the set affinity IPI handler in hwthread index %d. parm: %016lx\n", ProcessorID(), parm);
    // Call the function to do the move of the thread
    KThread_MoveToHwThread(kthread);
}

// Initiate a request to move a Kthread to a specific hardware thread
//     parm1: pointer to the KThread that is to be moved
//     parm2: the hardware thread that is to be the new destination for the Kthread
void IPI_setaffinity(KThread_t *pKThr)
{
    // Get the hardware thread that is currently associated with the KThread being moved
    int processorID = pKThr->ProcessorID;

    IPI_DeliverInterrupt(processorID, IPI_handler_setaffinity, (uint64_t)pKThr, 0);

}

//----------------------------------------------------
//    Change Policy handler/requestor functions
//----------------------------------------------------

// Set Processor Affinity Handler
void IPI_handler_changepolicy(uint64_t parm1, uint64_t parm2)
{
    KThread_t *kthread = (KThread_t *)parm1;
    int newPriority =   (int)parm2;      // lower word is the priority
    int newPolicy = (int)(parm2 >> 32);  // upper word is the policy

    // Test to see if the process is still active
    if (!IsProcessActive(kthread))
    {
        return;
    }
    // Set to restart location if speculation is active. Do not defer the specid invalidation.
    Speculation_CheckJailViolation(&GetMyKThread()->Reg_State);
    
    KThread_ChangePolicy(kthread, newPolicy, newPriority);
}

// Initiate a request to move a Kthread to a specific hardware thread
//     parm1: pointer to the KThread that is to be modified
//     parm2: new policy and priority
void IPI_changepolicy(KThread_t *pKThr, int newPolicy, int newPriority)
{
    // Get the hardware thread that is currently associated with the KThread being moved
    int processorID = pKThr->ProcessorID;
    uint64_t parm1 = (uint64_t)pKThr;
    uint64_t parm2 = ((uint64_t)newPolicy << 32) + newPriority;

    IPI_DeliverInterrupt(processorID, IPI_handler_changepolicy, parm1, parm2);
}




//----------------------------------------------------
//    Complete the migration of a kthread to its destination
//----------------------------------------------------

// Set Processor Affinity Handler
void IPI_handler_complete_migration(uint64_t parm1, uint64_t parm2)
{
    KThread_t *kthread = (KThread_t *)parm1;
    // Test to see if the process is still active
    if (!IsProcessActive(kthread))
    {
        return;
    }
    // Set to restart location if speculation is active. Do not defer the specid invalidation.
    Speculation_CheckJailViolation(&GetMyKThread()->Reg_State);
    
    //printf("Complete Migration IPI handler. hwt %d. kthread: %016lx\n", ProcessorID(), parm1);
    // Call the function to do the move of the thread
    KThread_CompleteMigration(kthread);
}

// Initiate a request to move a Kthread to a specific hardware thread
//     parm1: pointer to the KThread that is to be moved
//     parm2: the hardware thread that is to be the new destination for the Kthread
void IPI_complete_migration(KThread_t *pKThr)
{
    // Get the hardware thread that is currently associated with the KThread being moved
    int processorID = pKThr->ProcessorID;

    IPI_DeliverInterrupt(processorID, IPI_handler_complete_migration, (uint64_t)pKThr, 0);
}

//----------------------------------------------------
//  Scheduler handler/requestor functions
//----------------------------------------------------

// Run the scheduler to dispatch the most eligible pthread
void IPI_handler_run_scheduler(uint64_t kthread, uint64_t unblock_state)
{
    // Test to see if the process associated with the kthread is still active
    if (kthread && !IsProcessActive((KThread_t*)kthread))
    {
        return;
    }
    // Set to restart location if speculation is active. Do not defer the specid invalidation.
    Speculation_CheckJailViolation(&GetMyKThread()->Reg_State);
    
    // We can't call Scheduler() directly at this point for two reasons:
    //     1) We're in the middle of interrupt-handling loops that should be allowed to finish.
    //     2) The non-volatile state of the current thread will not have been saved yet.
    // If the interrupt arrived while we were already in the scheduler, we don't have to do anything.
    // Scheduler() will be called instead of resuming the interrupted code.  If we interrupted an application
    // thread, set a Pending flag to cause us to save its full state and call Scheduler() just before we
    // resume it.
    KThread_t *cur_kthr = GetMyHWThreadState()->pCurrentThread;
    if (cur_kthr->SlotIndex != CONFIG_SCHED_KERNEL_SLOT_INDEX) {
	cur_kthr->Pending |= KTHR_PENDING_YIELD;
    }
    // If a kthread pointer and a new state was passed to this handler, then we are responsible for
    // modifying the state of the kthread before we run the scheduler.
    if (kthread && unblock_state)
    {
        Sched_Unblock((KThread_t*)kthread, unblock_state);
    }
}
// Initiate a request to run the scheduler on a specific hardware thread
void IPI_run_scheduler(int processorID, KThread_t *kthread, int new_state)
{
    IPI_DeliverInterrupt( processorID, IPI_handler_run_scheduler, (uint64_t)kthread, (uint64_t)new_state);
}

// Run the scheduler to yield a kthread on a specific hardware thread.
void IPI_handler_block_thread(uint64_t kthread, uint64_t new_state)
{
    // Test to see if the process is still active
    if (!IsProcessActive((KThread_t *)kthread))
    {
        return;
    }
    // Set to restart location if speculation is active. Do not defer the specid invalidation.
    Speculation_CheckJailViolation(&GetMyKThread()->Reg_State);
    
    // We can't call Scheduler() directly at this point for two reasons:
    //     1) We're in the middle of interrupt-handling loops that should be allowed to finish.
    //     2) The non-volatile state of the current thread will not have been saved yet.
    // If the interrupt arrived while we were already in the scheduler, we don't have to do anything.
    // Scheduler() will be called instead of resuming the interrupted code.  If we interrupted an application
    // thread, set a Pending flag to cause us to save its full state and call Scheduler() just before we
    // resume it.
    KThread_t *cur_kthr = GetMyHWThreadState()->pCurrentThread;
    if (cur_kthr->SlotIndex != CONFIG_SCHED_KERNEL_SLOT_INDEX) {
	cur_kthr->Pending |= KTHR_PENDING_YIELD;
    }

    // Modify the state of the kthread before we run the scheduler.
    if (kthread && new_state)
    {
        Sched_Block((KThread_t *)kthread, new_state );
    }
}

// Initiate a request to yield a kthread on a specific hardware thread
void IPI_block_thread(int processorID, KThread_t *kthread, int new_state)
{
    IPI_DeliverInterrupt( processorID, IPI_handler_block_thread, (uint64_t)kthread, (uint64_t)new_state);
}


//----------------------------------------------------
// Signal handler/requestor functions
//----------------------------------------------------

// Deliver a signal
void IPI_handler_signal(uint64_t parm1, uint64_t parm2)
{
    AppProcess_t *proc  = (AppProcess_t *) parm1;
    int signum = (int)parm2;
    uint32_t tid = parm2 >> 32;

    if (!(proc && (proc->State >= ProcessState_Active)))
    {
        return;
    }
    // Cannot defer the JMV invalidate since we may not deliver the signal or enter the scheduler.
    // We do no want to leave the JMV processing incomplete and return to the application.
    Speculation_CheckJailViolation(&GetMyKThread()->Reg_State); 
    
    //printf("In the signal IPI handler in hwthread index %d. kthr %p, sig %d\n", ProcessorID(), kthr, sig);

    Signal_Deliver(proc, tid, signum);
}

// Deliver a signal to a specific hardware thread (kthread? The passed parm info may need to be more than just the sign_num)
void IPI_signal(int processorID, AppProcess_t *proc, int tid, int sig)
{
    IPIHANDLER_Fcn_t pendingFunction =
	IPI_DeliverInterrupt(processorID, IPI_handler_signal,
			     (uint64_t) proc, (uint64_t)((((uint64_t)tid)<<32) + sig));
    if (pendingFunction != NULL)
    {
        // Deadlock situation was avoided. An IPI action from this hwthread to this hardware thread is already pending.
        if (pendingFunction != IPI_handler_signal)
        {
            // May need to crash the kernel here. This signal cannot be delivered to this hwthread. Depending on the signal,
            // maybe we can deliver it to another pthread in the process that is on a different hwthread.
            printf("(E) Recursion in the IPI handler sending to self. Busy function: %016lx\n",(uint64_t)IPI_handler_signal);
        }
        else
        {
            // Possibly not a problem, since we already have a signal request pending. May depend on the signal that is
            // trying to be delivered vs the signal that is currently pending delivery. More work needed here......
            printf("(E) Recursion in the IPI handler - signal %d is already being delivered to processorID %d\n", sig, processorID);
        }
    }
}

//----------------------------------------------------
// Process exit
//----------------------------------------------------

// Inform all other threads within the process that the process is exiting
void IPI_handler_process_exit(uint64_t phase, uint64_t parm2)
{
    GetMyHWThreadState()->IpiExitInterruptPending = 0;
    // Verify that a process environment still exists. Note that we cannot use the IsProcessActive() 
    // test in this location since we may be transitioning from AppExit phase1 to AppExit phase2
    if (!GetMyProcess())   return;

    //printf("In the Process Exit IPI handler in hwthread index %d. status: %ld\n", ProcessorID(), status);
    App_Exit(phase, 0);
}
// Initiate a request to terminate the process
void IPI_process_exit(int processorID, int phase)
{
    // Set indicator that an IPI exit interrupt is pending against this hardware thread. 
    GetHWThreadStateByProcessorID(processorID)->IpiExitInterruptPending = 1;
    IPI_DeliverInterrupt(processorID, IPI_handler_process_exit, phase, 0);
}

//----------------------------------------------------
// Agent exit
//----------------------------------------------------

// Force termination of the application agent
void IPI_handler_agent_exit(uint64_t phase, uint64_t parm2)
{
    GetMyHWThreadState()->IpiExitInterruptPending = 0;
    // Verify that a process environment still exists. Note that we cannot use the IsProcessActive() 
    // test in this location since we may be transitioning from AppExit phase1 to AppExit phase2
    if (!GetMyProcess())   return;

    //printf("In the Agent Exit IPI handler in hwthread index %d. status: %ld\n", ProcessorID(), status);
    App_AgentExit(phase);
}
// Initiate a request to an application agent
void IPI_agent_exit(int processorID, int phase)
{
    GetHWThreadStateByProcessorID(processorID)->IpiExitInterruptPending = 1;
    // note: msync() exists in the following call before interrupt is sent, so not needed here.
    IPI_DeliverInterrupt(processorID, IPI_handler_agent_exit, phase, 0);
}

//----------------------------------------------------
//  Stack Guard handler/requestor functions
//----------------------------------------------------

// Adjust the stack guard
void IPI_handler_guard_adjust(uint64_t parm1, uint64_t parm2)
{
    uint64_t high_mark = parm1;
    MoveGuardDirection_t direction = (MoveGuardDirection_t)parm2;

    if (!IsMyProcessActive())
    {
        return;
    }
    Guard_Adjust(high_mark, direction, 1);
}
// Initiate a request to adjust the stack guard
void IPI_guard_adjust(int processorID, uint64_t high_mark, MoveGuardDirection_t direction)
{
    IPI_DeliverInterrupt(processorID, IPI_handler_guard_adjust, high_mark, (uint64_t)direction);
}

//---------------------------------------------------------
//  Load an application on the application leader 
//---------------------------------------------------------
// Adjust the stack guard
void IPI_handler_load_application(uint64_t parm1, uint64_t parm2)
{
    // Verify that a process environment still exists. This could be a latent interrupt.
    if (!IsMyProcessActive())
    {
        return;
    }
    App_Load();
}
// Initiate a request to load an application
void IPI_load_application(int processorID)
{
    IPI_DeliverInterrupt(processorID, IPI_handler_load_application, 0, 0);
}

//---------------------------------------------------------
//  Load an application agent 
//---------------------------------------------------------
// Adjust the stack guard
void IPI_handler_load_agent(uint64_t parm1, uint64_t parm2)
{
    // Test to see if the process is still active
    if (!IsMyProcessActive())
    {
        return;
    }
    App_LoadAgent();
}
// Initiate a request to load an application agent
void IPI_load_agent(int processorID)
{
    IPI_DeliverInterrupt(processorID, IPI_handler_load_agent, 0, 0);
}


void Kernel_Barrier_Internal(Lock_Atomic_t* pLock, uint32_t rendezvous_num);

//---------------------------------------------------------
//  Update MMU entry
//---------------------------------------------------------
void IPI_handler_update_MMU(uint64_t ackParm, uint64_t slot)
{
    uint32_t tcoord = slot>>32;
    slot &= 0xffff;
    Kernel_Lock(&NodeState.CoreState[ProcessorCoreID()].coreLock);
    vmm_UpdateMMU(tcoord, slot);
    Kernel_Unlock(&NodeState.CoreState[ProcessorCoreID()].coreLock);
    *((int64_t*)ackParm) = 1;
}

void IPI_update_MMU(int processorID, uint64_t slot)
{
    volatile int64_t *ackParm = &(ackIpi[ProcessorID()]);
    *ackParm = 0;
    IPI_DeliverInterrupt(processorID, IPI_handler_update_MMU, (uint64_t)ackParm, slot);
    ThreadPriority_Low();
    while (!(*ackParm) && !IPI_AbortForProcessExit() )
    {
        asm volatile("nop;");
        asm volatile("nop;");
        asm volatile("nop;");
        asm volatile("nop;");
        asm volatile("nop;");
    }
    ThreadPriority_Medium();
}

//---------------------------------------------------------
//  Tool command
//---------------------------------------------------------
void IPI_handler_tool_cmd(uint64_t toolIpiRequest, uint64_t parm2)
{
    Tool_ProcessCommands((ToolIpiRequest*)toolIpiRequest);
}

void IPI_tool_cmd(int processorID, ToolIpiRequest* toolIpiRequest)
{
    IPI_DeliverInterrupt(processorID, IPI_handler_tool_cmd, (uint64_t)toolIpiRequest, 0);
}

//---------------------------------------------------------
//  Tool suspend command
//---------------------------------------------------------
void IPI_handler_tool_suspend(uint64_t proc, uint64_t suspendAck)
{
    Speculation_CheckJailViolationDefer(&GetMyKThread()->Reg_State); 
    Tool_Suspend((AppProcess_t*)proc);
    *((volatile int *)suspendAck) = 1;
    ppc_msync();
}

void IPI_tool_suspend(int processorID, AppProcess_t *proc)
{
    volatile int suspendAck = 0;
    IPI_DeliverInterrupt(processorID, IPI_handler_tool_suspend, (uint64_t)proc, (uint64_t)&suspendAck);
    // Wait for the acknowledgement that the command was received.
    ThreadPriority_Low();
    while (!suspendAck)
    {
        // Calling an IPI that requires an Ack from the target should be using the Syscall_GetIpiControl() interface to obtain
        // a global IPI lock. This is done by UPC and L2 atomic operations. However, there is at least one IPI that also requires an
        // ack that cannot use this system lock interface. Any IPIs that require an ACK and cannot using the Syscall_GetIpiControl() lock will
        // need to execute this code in its loop that polls for the ack in order to eliminate a deadlock. 
        IPI_DeadlockAvoidance(processorID);
    }
    ThreadPriority_Medium();
}

//---------------------------------------------------------
//  Tool resume command
//---------------------------------------------------------
void IPI_handler_tool_resume(uint64_t proc, uint64_t parm2)
{
    Tool_Resume((AppProcess_t*)proc);
    ppc_msync();
}

void IPI_tool_resume(int processorID, AppProcess_t *proc)
{
    IPI_DeliverInterrupt(processorID, IPI_handler_tool_resume, (uint64_t)proc, 0);
    // Do NOT wait for the acknowledgement that the command was received. Need to continue in the ToolControl thread flow.
}

//---------------------------------------------------------
//  Tool release command
//---------------------------------------------------------
void IPI_handler_tool_release(uint64_t proc, uint64_t parm2)
{
    Tool_Release((AppProcess_t*)proc);
    ppc_msync();
}

void IPI_tool_release(int processorID, AppProcess_t *proc)
{
    IPI_DeliverInterrupt(processorID, IPI_handler_tool_release, (uint64_t)proc, 0);
    // Do NOT wait for the acknowledgement that the command was received. Need to continue in the ToolControl thread flow.
}


//---------------------------------------------------------
//  Tool signal notify command
//---------------------------------------------------------
void IPI_handler_tool_notifysignal(uint64_t parm1, uint64_t parm2)
{
    //uint32_t toolid = (uint32_t)parm1;
    KThread_t *kthread = (KThread_t *)parm1;
    Tool_DeferredSignalNotification(kthread);
}

void IPI_tool_notifysignal(KThread_t *kthread)
{
    IPI_DeliverInterrupt(kthread->ProcessorID, IPI_handler_tool_notifysignal, (uint64_t)kthread, 0);
    // Cannot wait for an ack. Must allow the calling thread to continue so it can poll the completion queue.
}

//---------------------------------------------------------
//  Tool available and conflict notify operations
//---------------------------------------------------------
void IPI_handler_tool_notifycontrol(uint64_t parm1, uint64_t parm2)
{
    ToolIpiNotifyControl *notifyParms = (ToolIpiNotifyControl *)parm1;
    int *pAckReceived  = (int*)parm2;
    // Copy the parameters in the interface to local variables. We need to do this and then release
    // the caller before we send the notify message so that we do not get deadlocked.
    ToolIpiNotifyControl localParms = *notifyParms;
    *pAckReceived = 1;
    ppc_msync();

    Tool_NotifyControl(&localParms);
}

void IPI_tool_notifycontrol(int processorID, ToolIpiNotifyControl *notifyParms)
{
    volatile int ackReceived = 0;
    IPI_DeliverInterrupt(processorID, IPI_handler_tool_notifycontrol, (uint64_t)notifyParms, (uint64_t)&ackReceived);

    ThreadPriority_Low();
    while (!ackReceived)
    {
        // Spin waiting for the ack.
        asm volatile("nop;");
        asm volatile("nop;");
        asm volatile("nop;");
        asm volatile("nop;");
        asm volatile("nop;");
    }
    ThreadPriority_Medium();
}

//---------------------------------------------------------
//  Jitterbug test tool
//---------------------------------------------------------
void IPI_handler_jitterbug(uint64_t seed, uint64_t parm2)
{
    Kernel_jitterbug(seed);
}

void IPI_start_jitterbug(uint64_t seed)
{
    // Send this request to the second app agent thread
    IPI_DeliverInterrupt(65, IPI_handler_jitterbug, seed, 0);
}



//----------------------------------------------------
//    UPC Apply handler/requestor functions
//----------------------------------------------------
extern void Upci_Punit_Cfg_Apply(struct sPunit_Cfg *pCfg);

// UPC Apply Handler
void IPI_handler_upc_attach(uint64_t parm1, uint64_t ackParm)
{
    Upci_Kernel_Punit_Cfg_Apply((struct sPunit_Cfg *)parm1);
    *((int64_t*)ackParm) = 1;
}

// Initiate a request to apply punit config to another processor core
// The actual hw thread effect is determined by the contents of the config structure.
// Caller is required to clear "result" value in pCfg, and wait for it to be set by the
// apply to indicate that it is finished.
void IPI_upc_attach(unsigned coreID, struct sPunit_Cfg *pCfg)
{
    if (coreID != ProcessorCoreID()) {
        volatile int64_t *ackParm = &(ackIpi[ProcessorID()]);
        *ackParm = 0;
        IPI_DeliverInterrupt(coreID*CONFIG_HWTHREADS_PER_CORE, IPI_handler_upc_attach, (uint64_t)pCfg, (uint64_t)ackParm);

        // wait for IPI to complete.
        ThreadPriority_Low();
        while (!(*ackParm) && !IPI_AbortForProcessExit()) {
            // avoid theoretical live lock
            asm volatile("nop;");
            asm volatile("nop;");
            asm volatile("nop;");
            asm volatile("nop;");
            asm volatile("nop;");
        }
        ThreadPriority_Medium();
    }
    else {
        Upci_Kernel_Punit_Cfg_Apply(pCfg);
    }
}

//----------------------------------------------------
//    UPCP Init and Disable handler/requestor functions
// Note: Assume core2core operations are serialized by caller
//----------------------------------------------------
//! Define gymnastics to create a compile time AT string.
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define _AT_ __FILE__ ":" TOSTRING(__LINE__)

// UPCP Init Handler
void IPI_handler_upcp_init(uint64_t parm1, uint64_t ackParm)
{
    //printf( _AT_ " core=%d\n", ProcessorCoreID());
    int mode = (int)(parm1 >> 32);
    unsigned ctrMode = (unsigned)(parm1 & 0xffffffffull);
    Upci_Kernel_Punit_Init_Mode( ProcessorCoreID(), mode, ctrMode);
    *((int64_t*)ackParm) = 1;
    mbar();
    //printf( _AT_ " core=%d\n", ProcessorCoreID());
}
// UPCP Disable Handler
void IPI_handler_upcp_disable(uint64_t ackParm, uint64_t parm2)
{
    Upci_Kernel_Punit_Disable( ProcessorCoreID() );
    *((int64_t*)ackParm) = 1;
    mbar();
}

// upc_p init to another processor core as necessary
void IPI_upcp_init(unsigned coreID, int mode, unsigned ctrMode)
{
    if (coreID != ProcessorCoreID()) {
        uint64_t parm1 = ((uint64_t)mode) << 32; // place the mode field in the upper 32 bits of the dword
        parm1 += ctrMode; // place the ctrMode field in the lower 32 bits of the dword
        volatile int64_t *ackParm = &(ackIpi[ProcessorID()]);
        *ackParm = 0;
        mbar();
        IPI_DeliverInterrupt(coreID*CONFIG_HWTHREADS_PER_CORE, IPI_handler_upcp_init, parm1, (uint64_t)ackParm);
        //printf( _AT_ " core=%d, proccore=%d\n", coreID, ProcessorCoreID());
        //int loop = 0;
        //Delay(1000000);

        // wait for IPI to complete.
        ThreadPriority_Low();
        while (!(*ackParm) && !IPI_AbortForProcessExit()) {
            //if ((loop++ % 10000) == 0) {
                //printf("core=%d, parm1.done=%d\n", coreID, parm1.done);
            //}
            // avoid theoretical live lock
            asm volatile("nop;");
            asm volatile("nop;");
            asm volatile("nop;");
            asm volatile("nop;");
            asm volatile("nop;");
        }
        ThreadPriority_Medium();
    }
    else {
        Upci_Kernel_Punit_Init_Mode( coreID, mode, ctrMode);
    }
}

// upc_p disable to another processor core as necessary
void IPI_upcp_disable(unsigned coreID)
{
    if (coreID != ProcessorCoreID()) {
        volatile int64_t *ackParm = &(ackIpi[ProcessorID()]);
        *ackParm = 0;
        mbar();
        IPI_DeliverInterrupt(coreID*CONFIG_HWTHREADS_PER_CORE, IPI_handler_upcp_disable, (uint64_t)ackParm, 0);
        ThreadPriority_Low();
        while (!(*ackParm) &&   !IPI_AbortForProcessExit()) {
            // avoid theoretical live lock
            asm volatile("nop;");
            asm volatile("nop;");
            asm volatile("nop;");
            asm volatile("nop;");
            asm volatile("nop;");
        }
        ThreadPriority_Medium();
    }
    else {
        Upci_Kernel_Punit_Disable( coreID );
    }
}

// Attempt to obtain IPI control for use within a syscall
int Syscall_GetIpiControl()
{
    uint32_t oldValue = 0;
    uint32_t newValue = ProcessorID()+1000; // Use our processorID for debug and guarantee a non-zero value. 
    if (!compare_and_swap(&syscallIpiControl, &oldValue, newValue))
    {
        return -1;
    }
    return 0;
}

// Release IPI control 
int Syscall_ReleaseIpiControl()
{
    uint32_t oldValue = ProcessorID()+1000; // Use our processorID for debug and guarantee a non-zero value.
    uint32_t newValue = 0;
    if (!compare_and_swap(&syscallIpiControl, &oldValue, newValue))
    {
        return -1;
    }
    return 0;
}

// Test to see if there is a pending IPI request for process termination directed to the curently running hardware thread.
int IPI_AbortForProcessExit()
{
    return (GetMyHWThreadState()->IpiExitInterruptPending);
}

//---------------------------------------------------------
//  Invalidate I-cache.
//---------------------------------------------------------
void IPI_handler_invalidate_icache(uint64_t parm1, uint64_t parm2)
{
    isync();
    ici();
}

void IPI_invalidate_icache(int coreID)
{
    IPI_DeliverInterrupt(coreID*CONFIG_HWTHREADS_PER_CORE,
			 IPI_handler_invalidate_icache, 0, 0);
}

//----------------------------------------------------
// Remote thread exit
//----------------------------------------------------

// Inform all other threads within the process that the process is exiting
void IPI_handler_remote_thread_exit(uint64_t proc, uint64_t parm2)
{
    // Verify that a process environment still exists. 
    if (!GetMyProcess())   return;

    App_RemoteThreadExit((AppProcess_t*)proc);
}
// Initiate a request to terminate threads running on a remote hardware thread.
void IPI_remote_thread_exit(int processorID, AppProcess_t *proc)
{
    // Deliver the interrupt.
    IPI_DeliverInterrupt(processorID, IPI_handler_remote_thread_exit, (uint64_t)proc, 0);
}

// Flush pending IPIs from the specified hwt.
void IPI_DeadlockAvoidance(int targetProcessorID)
{
    int myProcessorID = ProcessorID();
    int myProcessorThreadID = ProcessorThreadID();
    uint64_t c2c_status = BIC_ReadStatusExternalRegister0(myProcessorThreadID);
    // Are there any interrupts pending against this hardware thread
    // from the passed-in processor id
    uint64_t mask = _BN(targetProcessorID);
    // Is an interrupt pending from the selected hardware thread?
    if ((myProcessorID != targetProcessorID) && (mask & c2c_status))
    {
        // We need to investigate this interrupt to see if it is one of the IPI requests that require an ack from its target
        IPI_Message_t* pIPImsg = (IPI_Message_t*)&(NodeState.CoreState[targetProcessorID/4].HWThreads[targetProcessorID%4].ipi_message[myProcessorID]);
        if ((pIPImsg->fcn == IPI_handler_update_MMU) ||
            (pIPImsg->fcn == IPI_handler_upc_attach) ||
            (pIPImsg->fcn == IPI_handler_upcp_disable) ||
            (pIPImsg->fcn == IPI_handler_upcp_init) ||
            (pIPImsg->fcn == IPI_handler_upc_attach) ||
            (pIPImsg->fcn == IPI_handler_tool_suspend) ||
            (pIPImsg->fcn == IPI_handler_complete_migration) // note: this does not require an ack but may be stacked with another IPI
            )
        {
            IPI_Message_t IPImsg_local = *pIPImsg;
            BIC_WriteClearExternalRegister0(myProcessorThreadID, mask);                    
            // Reset fcn field in the IPI message data to enable subsequent IPIs
            pIPImsg->fcn = NULL;
            ppc_msync();
            //printf("(W) IPI Deadlock avoidance. Flushing handler: %016lx\n", (uint64_t)IPImsg_local.fcn);
            Kernel_WriteFlightLog(FLIGHTLOG, FL_ADLOCKIPI, targetProcessorID, (uint64_t)IPImsg_local.fcn, IPImsg_local.param1, mfspr(SPRN_SRR0));
            // Call the handler
            if (IPImsg_local.fcn)
            {
                IPImsg_local.fcn(IPImsg_local.param1, IPImsg_local.param2);
            }
        }
    }
}


