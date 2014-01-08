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
#include "ctype.h"
#include <hwi/include/bqc/wu_mmio.h>


// Called when an MMAP or heap operation has modified 
// the heap break address or the maximum allocated mmap address.
// The process leader core will be determined by this routine and the IPI will be delivered to it
int Guard_Adjust( uint64_t high_mark, MoveGuardDirection_t direction, int isIPIredrive )
{
    // Get current core and process object
    int processorID = ProcessorID();
    AppProcess_t* pProc = GetProcessByProcessorID(processorID);
    HWThreadState_t *pProcLeader_HwThState = GetHWThreadStateByProcessorID(pProc->ProcessLeader_ProcessorID);
    KThread_t *ProcLeaderKthread = pProcLeader_HwThState->SchedSlot[CONFIG_SCHED_SLOT_FIRST];

    // only continue if guarding enabled
    if (!pProc->Guard_Enable)
    {
        TRACE( TRACE_MemAlloc, ("(I) %s[%d]: Stack Guarding Disabled\n",
                                __func__, ProcessorID()));
        return (0); // normal return, guarding is not enabled so nothing to do
    }
    TRACE( TRACE_MemAlloc, ("(I) %s[%d]: Guarding new high_mark=0x%016lx current high_mark=>0x%016lx MoveDown=%d\n",
                            __func__, ProcessorID(), high_mark,pProc->Guard_HighMark, direction ));

    // Test to see if we are running in the same CORE as the process leader thread
    // If we are, we will modify the WAC for the hardware thread of the process leader. We DO NOT SEND AN IPI to a
    // hardware thread on the same core since the WAC reg is on this core and if we sent the IPI, we would need to
    // wait for the IPI to complete before safely continuing to run in this hardware thread. 
    if ((pProc->ProcessLeader_ProcessorID>>2) != (processorID>>2))
    {
        // We are not running on the same core as the process leader. 
        // Send and IPI to the process leader's hardware thread to complete this operation. 
        IPI_guard_adjust(pProc->ProcessLeader_ProcessorID, high_mark, direction);
        return 0;
    }

    if (((direction == MoveGuardUp) && (high_mark > pProc->Guard_HighMark)) ||
        ((direction == MoveGuardDown) && (high_mark < pProc->Guard_HighMark)))
    {
        pProc->Guard_HighMark = high_mark; // set the new HighMark
    }

    // set new guard value into the AppProcess 
    uint64_t guardMask = pProc->Guard_Size-1;
    pProc->Guard_Start = (pProc->Guard_HighMark + guardMask) & ~guardMask; 

    // Clear the pattern from the original guarded area and set the pattern in the new guarded area. Also remove
    // the page from the L1 cache 
    void *virt_GuardBaseAddress = (void*)(pProc->Heap_VStart + (ProcLeaderKthread->GuardBaseAddress - pProc->Heap_PStart));
    int processLeaderThreadID = pProc->ProcessLeader_ProcessorID & 0x3;
    KThread_t *myKThread = GetMyKThread();

    if ((pProc->ProcessLeader_ProcessorID != processorID) || // Are we not running on the same hardware thread as the process leader? (process leader could be active, we dont know)
        (ProcLeaderKthread == myKThread)  ||  // Is the process leader kthread is active 
        ((myKThread->SlotIndex == CONFIG_SCHED_KERNEL_SLOT_INDEX) && (pProc->ProcessLeader_ProcessorID == processorID))) // Is this the process leader hdwr thread and we are in the kernel thread

    {
        // Put a value in the WAC register so that we do not generate a match condition while we are clearing the old guard area
        WAKEUP_BASE[WAC_BASE( processLeaderThreadID+CNK_STACK_GUARD_FIRST_WAC )] = -1;
        ppc_msync();
    }
    //printf("HeapAdjust prepare: %016lx - %016lx  oldguard:%016lx\n", (uint64_t)pProc->Guard_Start, (uint64_t)pProc->Guard_Start+pProc->Guard_Size-1, (uint64_t)virt_GuardBaseAddress);
    Guard_Prepare(virt_GuardBaseAddress, (void*)pProc->Guard_Start, pProc->Guard_Size);
    // Modify the pointer in the kthread structure (for use by the scheduler). 
    ProcLeaderKthread->GuardBaseAddress = pProc->Heap_PStart + (pProc->Guard_Start - pProc->Heap_VStart);
    // If the current kthread is the process leader kthread or if the current kthread is the kernel thread (e.g. in futex wait), 
    // modify the current guard register in the hardware. Otherwise, when the process leader thread is dispatched, the new value will be loaded.
    if ((pProc->ProcessLeader_ProcessorID != processorID) || // Are we not running on the same hardware thread as the process leader? (process leader could be active, we dont know)
        (ProcLeaderKthread == myKThread) ||  // Is the process leader kthread is active 
        ((myKThread->SlotIndex == CONFIG_SCHED_KERNEL_SLOT_INDEX) && (pProc->ProcessLeader_ProcessorID == processorID))) // Is this the process leader hdwr thread and we are in the kernel thread
    {
        WAKEUP_BASE[WAC_BASE( processLeaderThreadID+CNK_STACK_GUARD_FIRST_WAC )] = ProcLeaderKthread->GuardBaseAddress;
        ppc_msync();
    }

    TRACE( TRACE_MemAlloc, ("(I) %s[%d]: Guarding modified. Guard(real): %016lx Mask: %016lx\n",
                            __func__, ProcessorID(), ProcLeaderKthread->GuardBaseAddress, ProcLeaderKthread->GuardEnableMask));
    //printf( "(I) %s[%d]: Guard modified. Guard: %016lx Mask: %016lx\n",
    //        __func__, ProcessorID(), ProcLeaderKthread->GuardBaseAddress, ProcLeaderKthread->GuardEnableMask);

    // Note that for the case when we send the guard IPI to the process leader we dont for sure if we have generated a conflict. 
    // We are returning true here and this may result in the clearing of the stack being used by the process leader when
    // the mmap code prepares (zeroes) the allocated storage.  This is the one scenario we cannot detect soon enough 
    // therefore the result may be a failure other than the SIGSTKFLT signal. Likely a SIGILL trying to execute an 
    // illegal (zero) instruction when restoring context from the zeroed stack
    //
    return 0; 
}

// Prepares a new guard area by populating it with a know data pattern. Also will clear the memory of 
// a previous guard area if a non-NULL address is provided in the first parameter.
void Guard_Prepare(void *original_guardbase, void *new_guardbase, int size)
{
    if (original_guardbase)
    {
        memset(original_guardbase, 0x00, size);
    }
    uint64_t *ptr1 = (uint64_t*)new_guardbase;
    uint64_t *ptr1_end = (uint64_t*)((uint8_t*)new_guardbase + size);
    while (ptr1 < ptr1_end)
    {
        uint64_t *ptr2 = ptr1;
        uint64_t *ptr2_end = ptr1+L1D_CACHE_LINE_SIZE/8;
        while (ptr2 < ptr2_end)
        {
            *ptr2 = 0xDEADBEEFDEADBEEF;
            ptr2++;
        }
        dcache_block_inval(ptr1);
        ptr1 += L1D_CACHE_LINE_SIZE/8;
    }
    ppc_msync();

}

// Resets the guard registers for the currently running hardware thread. Address values that 
// correspond to impossible real addresses are provided to guarantee that no match occurs.
void Guard_Reset()
{
    int thdid = ProcessorThreadID();
    WAKEUP_BASE[WAC_BASE( thdid+CNK_STACK_GUARD_FIRST_WAC )] = -1;
    WAKEUP_BASE[WAC_ENABLE( thdid+CNK_STACK_GUARD_FIRST_WAC )] = -1;
    ppc_msync();

}





