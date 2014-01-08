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
#include "ToolControl.h"
#include <netinet/in.h>
#include "hwi/include/bqc/l2_central_inlines.h"
#include "fs/shmFS.h"
#include <ramdisk/include/services/JobctlMessages.h>
#include "flih.h"
#include "NodeController.h"

#define SIG_MASK(sig) (1ul << ((sig) - 1))

#define ROUND_UP_DWORD(p) (((p) + 0x7)  & ~0x7)

// Define the toolController object in static storage
ToolControl toolControl;


#define USE_L2ATOMIC_TRAPLOCK 1

#define TRAPTABLE_READERBLOCK 10000

//#define TRAPTABLE_TEST_HASHDEPTH 10000

#if USE_L2ATOMIC_TRAPLOCK
uint64_t TrapTableReaderLock K_ATOMIC;
uint64_t TrapTableWriterLock K_ATOMIC;
#endif

using namespace bgcios::toolctl;

// Public functions

//------------------------------
// Class ToolTable
//------------------------------

int ToolTable::init()
{
    for (int i=0; i< MAX_NUM_TOOLS; i++)
    {
        entry[i].tool_id  =  0;   
        entry[i].priority =  0; 
        entry[i].sigmask = 0; 
        entry[i].app = NULL;
        entry[i].default_thread_id = 0;
        entry[i].fastBreakpointsEnabled = 0;
        entry[i].fastWatchpointsEnabled = 0;
        entry[i].dynamicAppNotifyStart = TOOL_DYNAMIC_APP_NOTIFY_DEFAULT;
        entry[i].sndsignal = 0;
    }
    numToolsAttached = 0;
    toolWithControlAuthority = 0;
    return 0;  
}

uint32_t ToolTable::validateAdd(uint32_t tool_id, AppState_t *appstate, uint32_t priority)
{
    if (numToolsAttached == MAX_NUM_TOOLS)
    {
        return bgcios::ToolMaxAttachedExceeded; // No available slot was found
    }
    // Verify that no conflicts exists and there is room in the tool table
    for (int i=0; i<MAX_NUM_TOOLS; i++)
    {
        if (!tool_id)
        {
            return bgcios::ToolIdError;
        }

        if (tool_id == entry[i].tool_id)
        {
            return bgcios::ToolIdConflict;
        }

        if (priority && (entry[i].priority == priority))
        {
            return bgcios::ToolPriorityConflict; // return error. Priority conflict
        }
    }
    return bgcios::Success;
}

// Add tool to table. Assumes that the validation has already been done in the validateAddTool() function
void ToolTable::add(uint32_t tool_id, AppState_t *appstate, uint32_t priority, char *toolTag)
{
    int insertIndex = numToolsAttached; // initialize to the first available index at the end of the table
    for (int i=0; i<MAX_NUM_TOOLS; i++)
    {
        if (priority > entry[i].priority)
        {
            insertIndex = i;
            break;
        }
    }
    // Make space to put the new entry in the proper location
    int toEntry = numToolsAttached;
    while (toEntry > insertIndex)
    {
        int fromEntry = toEntry-1;
        entry[toEntry] = entry[fromEntry];
        toEntry--;
    }
    // Write the new entry
    entry[insertIndex].tool_id = tool_id;
    entry[insertIndex].app = appstate;
    entry[insertIndex].priority = priority;
    entry[insertIndex].default_thread_id = 0;
    strncpy(entry[insertIndex].tag, toolTag, ToolTagSize);
    numToolsAttached++;
}

int ToolTable::remove(uint32_t tool_id)
{
    int compress = 0;
    int toolfound = 0;
    // Find the tool within the table
    for (int i=0; i<MAX_NUM_TOOLS; i++)
    {
        if (compress && entry[i].tool_id)
        {
            entry[i-1] = entry[i];
        }
        if (entry[i].tool_id == tool_id)
        {
            toolfound = 1;
            numToolsAttached--;
            entry[i].tool_id  =  0;   
            entry[i].priority =  0;  
            entry[i].sigmask = 0; 
            entry[i].app = NULL;
            entry[i].default_thread_id = 0;
            compress = 1; // Compress remaining entries of the table into the newly opened slot
        }
    }
    if (toolfound)
        return 0;
    else
        return -1;
}

void ToolTable::removeAll()
{
    // Remove all tools for this process's tool table
    for (int i=0; i<MAX_NUM_TOOLS; i++)
    {
      entry[i].tool_id  =  0;   
      entry[i].priority =  0;  
      entry[i].sigmask = 0; 
      entry[i].app = NULL;
      entry[i].default_thread_id = 0;
    }
    numToolsAttached = 0;
    toolWithControlAuthority = 0;
}

uint32_t ToolTable::controllingToolId()
{
    return toolWithControlAuthority;
}

void ToolTable::enableFastBreakpoints(uint32_t tool_id)
{
    for (int i=0; i< MAX_NUM_TOOLS; i++)
    {
        if (entry[i].tool_id == tool_id)
        {
            entry[i].fastBreakpointsEnabled = 1;
            return;
        }
    }
}

void ToolTable::disableFastBreakpoints(uint32_t tool_id)
{
    for (int i=0; i< MAX_NUM_TOOLS; i++)
    {
        if (entry[i].tool_id == tool_id)
        {
            entry[i].fastBreakpointsEnabled = 0;
            return;
        }
    }
}

void ToolTable::enableFastWatchpoints(uint32_t tool_id)
{
    for (int i=0; i< MAX_NUM_TOOLS; i++)
    {
        if (entry[i].tool_id == tool_id)
        {
            entry[i].fastWatchpointsEnabled = 1;
            return;
        }
    }
}

void ToolTable::disableFastWatchpoints(uint32_t tool_id)
{
    for (int i=0; i< MAX_NUM_TOOLS; i++)
    {
        if (entry[i].tool_id == tool_id)
        {
            entry[i].fastWatchpointsEnabled = 0;
            return;
        }
    }
}

int ToolTable::isFastBreakpointEnabled(uint32_t toolid)
{
    for (int i=0; i< MAX_NUM_TOOLS; i++)
    {
        if (entry[i].tool_id == toolid)
        {
            return (entry[i].fastBreakpointsEnabled);
        }
    }
    return 0;
}

int ToolTable::isFastWatchpointEnabled(uint32_t toolid)
{
    for (int i=0; i< MAX_NUM_TOOLS; i++)
    {
        if (entry[i].tool_id == toolid)
        {
            return (entry[i].fastWatchpointsEnabled);
        }
    }
    return 0;
}



int ToolTable::signalMonitored(uint32_t tool_id, int signal)
{
    for (int i=0; i< MAX_NUM_TOOLS; i++)
    {
        if (entry[i].tool_id  == tool_id)
        {
            if (entry[i].sigmask & SIG_MASK(signal))
                return 1;
            else
                return 0;
        }
    }
    return 0;  

}

void ToolTable::setDefaultThreadId(uint32_t toolid, uint32_t threadid)
{
    for (int i=0; i<numTools(); i++)
    {
        if (toolId(i) == toolid)
        {
            entry[i].default_thread_id = threadid;
            return;
        }
    }
}

uint32_t ToolTable::getDefaultThreadId(uint32_t toolid)
{
    for (int i=0; i<numTools(); i++)
    {
        if (toolId(i) == toolid)
        {
            return (entry[i].default_thread_id);
        }
    }
    return 0;
}

int ToolTable::isDynamicAppNotifyStart(uint32_t toolid)
{
    for (int i=0; i< MAX_NUM_TOOLS; i++)
    {
        if (entry[i].tool_id == toolid)
        {
            return (entry[i].dynamicAppNotifyStart);
        }
    }
    return 0;
}

void ToolTable::setDynamicAppNotifyStart(uint32_t toolid)
{
    for (int i=0; i< MAX_NUM_TOOLS; i++)
    {
        if (entry[i].tool_id == toolid)
        {
            entry[i].dynamicAppNotifyStart = 1;
            return;
        }
    }
}

void ToolTable::setDynamicAppNotifyDLoader(uint32_t toolid)
{
    for (int i=0; i< MAX_NUM_TOOLS; i++)
    {
        if (entry[i].tool_id == toolid)
        {
            entry[i].dynamicAppNotifyStart = 0;
            return;
        }
    }
}

uint64_t ToolTable::signalMask(uint32_t toolid)
{
    for (int i=0; i< MAX_NUM_TOOLS; i++)
    {
        if (entry[i].tool_id == toolid)
        {
            return (entry[i].sigmask);
        }
    }
    return 0;
}

uint32_t ToolTable::toolPriority(uint32_t toolid)
{
    for (int i=0; i< MAX_NUM_TOOLS; i++)
    {
        if (entry[i].tool_id == toolid)
        {
            return (entry[i].priority);
        }
    }
    return 0;
}

int  ToolTable::controlAquiredSignal(uint32_t toolid)
{
    for (int i=0; i< MAX_NUM_TOOLS; i++)
    {
        if (entry[i].tool_id == toolid)
        {
            return (entry[i].priority);
        }
    }
    return 0;
}

char* ToolTable::toolTag(uint32_t toolid)
{
    for (int i=0; i< MAX_NUM_TOOLS; i++)
    {
        if (entry[i].tool_id == toolid)
        {
            return (entry[i].tag);
        }
    }
    return NULL;
}

uint32_t ToolTable::grantControlAuthority(uint32_t toolid, uint64_t sigmask, int sndsignal)
{
    if (toolWithControlAuthority && (toolid != toolWithControlAuthority))
    {
        // There is a conflict. Set the conflict indicator in our toolid
        for (int i=0; i<MAX_NUM_TOOLS; i++)
        {
            if (entry[i].tool_id  ==  toolid)
            {
                entry[i].conflict = toolWithControlAuthority;
                break;
            }
        }
        return toolWithControlAuthority;
    }
    else
    {
        // Find the entry that matches our toolid
        for (int i=0; i<MAX_NUM_TOOLS; i++)
        {
            if (entry[i].tool_id  ==  toolid)
            {
                toolWithControlAuthority = toolid;
                entry[i].sigmask = sigmask & ~(SIG_MASK(SIGKILL)); // Set the mask, do not allow SIGKILL to be intercepted
                entry[i].sndsignal = sndsignal;
                return toolid; // we are now the controlling toolid so return our id.
            }
        }
        // Did not find the requested toolID. 
        return 0; // Return 0 indicating that no tool currently has control authority.
    }
}

int ToolTable::releaseControlAuthority(uint32_t toolid)
{
    if (toolWithControlAuthority != toolid)
    {
        return -1; // This tool does not have control authority. Fail the request.
    }
    toolWithControlAuthority = 0;
    return 0; // operation was successful.
}


uint32_t ToolTable::findConflictingTool() 
{
    // Tools are listed in priority order. Return the toolid of the first tool we find with a conflict.
    for (int i=0; i<MAX_NUM_TOOLS; i++)
    {
        if (entry[i].conflict)
        {
            entry[i].conflict = 0;   // Reset the conflict. 
            return entry[i].tool_id; // return the tool id.
        }
    }
    // Did not find a conflicting toolID. 
    return 0; 
}

void ToolTable::setConflict(uint32_t toolid)
{
    for (int i=0; i<MAX_NUM_TOOLS; i++)
    {
        if (entry[i].tool_id == toolid)
        {
            entry[i].conflict = 1;   // set conflict. 
            return;
        }
    }
}

int ToolTable::isAttached(uint32_t toolid)
{
    for (int i=0; i<MAX_NUM_TOOLS; i++)
    {
        if (entry[i].tool_id == toolid)
        {
            return 1; 
        }
    }
    return 0;
}

//------------------------------
// Class Breakpoint Controller
//------------------------------
void BreakpointController::init()
{
    // Boundary align the start of the trap work area to  4K
    uint64_t alignedWorkArea = ROUND_UP_4K((uint64_t)trapWorkArea);
    for (int i=0; i<CONFIG_MAX_APP_CORES; i++, alignedWorkArea += 4096)
    {
        coreControl[i].virtualAltText = alignedWorkArea;
        coreControl[i].physicalAltText = alignedWorkArea; // Kernel storage is Virtual == Physical
        coreControl[i].trapAddress = 0;
        for (int j=0; j<CONFIG_HWTHREADS_PER_CORE; j++)
        {
            coreControl[i].hwt[j].stepAddress = 0;
            coreControl[i].hwt[j].dear = 0;
            coreControl[i].hwt[j].dbsr = 0;
            coreControl[i].hwt[j].dbcr0 = 0;
            coreControl[i].hwt[j].pendingStepKthread = NULL;
        }
    }
    trapTable.entry = NULL; // Will be set when the table is allocated.
    uint64_t tablesize = TRAP_TABLE_ENTRIES * sizeof(trapTable.entry[0]); 
    assert(tablesize <=  sizeof(NodeState.sharedWorkArea));

    trapTable.numActiveEntries = 0;
    trapAfterEnabled = 0;
    trapTable.trapTableAllocated = 0;
}

int BreakpointController::trapHandler(Regs_t *context)
{
    int coreID = ProcessorCoreID();
    uint32_t myThreadId = ProcessorThreadID();
    AppProcess_t *proc = GetMyProcess(); 

    //printf("start trapHandler() ip=%016lx\n", context->ip);

    if (!proc)
    {
        printf("(E) trapHandler() null process pointer detected\n");
        return 0; // Should not occur. If it does, record this as an unhandled trap condition so normal signal processing occurs
    }

    // Are we processing a trap breakpoint
    if (context->esr & ESR_PTR)
    {

        // Is the trapped address a valid application address?
        if (!VMM_IsAppAddress((void*)context->ip, 4))
            return 0; // Unexpected trap location detected. Return as unhandled to allow interrupt handler to deal with this. 

        // Obtain the correct instruction pointer to use during these tests
        uint64_t pTrapInstruction = context->ip;
        // If this is not an address that is shared across multiple processes, do not look in the breakpoint table
        int isUniqueAddress = VMM_IsAppProcessUniqueAddress((void*)pTrapInstruction, 4);
        if (isUniqueAddress)
        {
            // Verify that the trap instruction is still in memory. It may have been removed by the time we reach here.
            uint32_t inst = *((uint32_t*)pTrapInstruction);

            if (!TOOL_ISTRAP(inst))
            {
                // Detected a removed breakpoint
                //printf("Trap not found in table or in memory. Must have been removed. Returning 1 to re-execute\n");
                return 1;
            }
            // Untracked trap encountered
            return 0; // Trap is not being tracked in our table. return as unhandled so normal signal processing occurs
        }

        // Lock the trap table for reading.

        TrapTableReadLock();
        // Find the address in the trap table
        TrapTableEntry *tableElement = findTrapTableEntry((void*)pTrapInstruction, 1);
        if (!tableElement || !tableElement->instAddress)
        {
            // Trap not found in the trap table. It may have been removed prior to us getting the breakpoint controller lock
            // Read the ip and if we do not see a breakpoint instruction, return 1, assuming that this entry was
            // removed from the table.
            uint32_t inst = *((uint32_t*)pTrapInstruction);
            if (!TOOL_ISTRAP(inst))
            {
                // Detected a removed breakpoint
                //printf("Trap not found in table or in memory. Must have been removed. Returning 1 to re-execute\n");
                TrapTableReadUnlock();
                return 1;
            }
            // Untracked trap encountered
            TrapTableReadUnlock();
            return 0; // Trap is not being tracked in our table. return as unhandled so normal signal processing occurs
        }
        // Determine if this process has this trap address set as active
        if (_BN(proc->ProcessLeader_ProcessorID) & tableElement->processMask)
        {
            // Yes, this process is configured to process this trap.
            //printf("Process is configured to handle the trap. Returning 0\n");
            TrapTableReadUnlock();
            return 0;
        }

        // Now the fun begins. This is a false trap condition for the current process. 
        uint32_t originalInstruction = tableElement->originalValue;
        // Free the read lock on the trap table.
        TrapTableReadUnlock();
        uint64_t mask = ~(1<<(myThreadId));
        // Suspend the other threads on this core
        mtspr(SPRN_TENC, mask);
        // Note: there is an isync() within the eratilx inline after the MMUCR0 is written, therefore we do not need one here

        // Do a local erat invalidation of the erat entry corresponding to our trapped address
        eratilx((void*)context->ip, MMUCR0_TLBSEL_IERAT);

        // Write the saved "original" instruction into the trapWorkArea at the same 4K page offset as the virtual address
        int instOffset = context->ip - ROUND_DN_4K(context->ip);
        //printf("virtualAltText %016lx instOffset %08x roundedIP %016lx\n", coreControl[coreID].virtualAltText, instOffset, ROUND_DN_4K(context->ip));
        *((uint32_t*)(coreControl[coreID].virtualAltText+instOffset)) = originalInstruction; 
        // We need to invalidate the icache so that it sees the data modification that was just written through the dcache
        icache_block_inval((void*)(coreControl[coreID].virtualAltText+instOffset));

        //   Set high water mark to entry index 14 to reserve the last entry (index 15)
        eratwatermark(14, MMUCR0_TLBSEL_IERAT);

        // Prepare the data for the new ERAT entry 
        uint64_t newW0 = ERAT0_EPN(ROUND_DN_4K(context->ip)) | ERAT0_Size_4K | ERAT0_V | ERAT0_ThdID(0xf);
        // Modify the ERAT physical address.
        uint64_t newW1 = ERAT1_RPN(coreControl[coreID].physicalAltText) | ERAT1_R | ERAT1_C | ERAT1_M | 
                         ERAT1_UX | ERAT1_UR | ERAT1_SX | ERAT1_SW | ERAT1_SR | ERAT1_U3; 

        // Write new erat into the the watermarked area. Note that if this kernel code is using this ERAT entry, we should be brought
        // into a new ERAT entry below the watermark after the eratwe completes.
        eratwe(15, newW0, newW1, MMUCR0_TLBSEL_IERAT | MMUCR0_ECL(1)); // Set the ExClass=1 to prevent invalidations from other cores

        //printf("Temp ERAT w0:%016lx w1:%016lx instruction:%08x written to:%016lx\n", newW0, newW1, originalInstruction, coreControl[coreID].virtualAltText+instOffset);
        // turn off MSR[DE] in our MSR. Need to do this before enabling the Step interrupt otherwise we will get an interrupt in the kernel 
        mtmsr(MSR_CNK);
        ppc_msync();

        // Set the DBCR0 register to do a STEP operation
        uint64_t dbcr0 = mfspr(SPRN_DBCR0);
        mtspr(SPRN_DBCR0, dbcr0 | DBCR0_ICMP | DBCR0_IDM);

        // Set the trap address in the core control structure
        coreControl[coreID].trapAddress = pTrapInstruction; 

    }
    // Are we processing a DAC watchpoint
    else if (context->dbsr & TOOL_DBSR_DAC_MATCH)
    {
        //printf("TrapHandler processing a DAC match\n");

        // Save the DACs in the critical reg save since we need a place outside of the active kthread context. These DACx fields are otherwise unused.
        // This is needed when presenting deferred notify messages to a tool. We need to save these only if a match in indicated in the DBSR. 
        // If we blindly save these fields we may loose information when we are processing the TrapAfter DAC semantics (step operation following
        // the initial interrrupt.
        HWThreadState_t *hwt = GetMyHWThreadState();
        if (context->dbsr & (DBSR_DAC1R | DBSR_DAC1W))
        {
            hwt->CriticalState.dac1 = mfspr(SPRN_DAC1);
        }
        if (context->dbsr & (DBSR_DAC2R | DBSR_DAC2W))
        {
            hwt->CriticalState.dac2 = mfspr(SPRN_DAC2);
        }
        if (context->dbsr & (DBSR_DAC3R | DBSR_DAC3W))
        {
            hwt->CriticalState.dac3 = mfspr(SPRN_DAC3);
        }
        if (context->dbsr & (DBSR_DAC4R | DBSR_DAC4W))
        {
            hwt->CriticalState.dac4 = mfspr(SPRN_DAC4);
        }
        // Are we in "trap-after" mode?
        if(trapAfterEnabled)
        {
            // Save registers for restoration after the step
            coreControl[coreID].hwt[myThreadId].dbcr0 = mfspr(SPRN_DBCR0);
            coreControl[coreID].hwt[myThreadId].dbsr = context->dbsr;
            coreControl[coreID].hwt[myThreadId].dear = context->dear;
            // Turn off the DAC status bits in the DBSR
            mtspr(SPRN_DBSR,TOOL_DBSR_DAC_MATCH);

            // Turn off the DAC interrupt in the debug control register to prevent interrupt on step
            uint64_t mod_dbcr0 = mfspr(SPRN_DBCR0);
            mod_dbcr0 &= ~(DBCR0_DAC1(-1) | DBCR0_DAC2(-1) | DBCR0_DAC3(-1) | DBCR0_DAC4(-1));
            mod_dbcr0 |=  DBCR0_ICMP | DBCR0_IDM; 
            mtspr(SPRN_DBCR0, mod_dbcr0);
        }
        else
        {
            return 0; // Normal interrupt processing for this DAC interrupt condition.
        }
    }
    else if (context->dbsr & TOOL_DBSR_IAC_MATCH)
    {

        // Turn off the IAC interrupt in the debug control register. We use this for the text segment of dynamic apps.
        uint64_t mod_dbcr0 = mfspr(SPRN_DBCR0);
        mod_dbcr0 &= ~(DBCR0_IAC1 | DBCR0_IAC2 | DBCR0_IAC3 | DBCR0_IAC4);
        mtspr(SPRN_DBCR0, mod_dbcr0);
        context->dbcr0 &= ~(DBCR0_IAC1 | DBCR0_IAC2 | DBCR0_IAC3 | DBCR0_IAC4);
        return 0; // Normal interrupt processing for this IAC interrupt condition.
    }
    // Disable EE and CR in the context's MSR so that we do not take any detours when we step (i.e. expired decrementer, etc)
    context->msr &= ~(MSR_CE | MSR_EE);

    // Turn on MSR[DE] in the context's MSR so that we take the step interrupt
    context->msr |= MSR_DE;

    // Return indication that we have processed this trap interrupt.
    // We should enter the following stepHandler() function after the step occurs.
    //printf("returning 1 from trapHandler(). Should enter step handler next.\n");
    return 1; 
}
          
int BreakpointController::stepHandler(Regs_t *context)
{ 
    int coreID = ProcessorCoreID();
    uint32_t myThreadId = ProcessorThreadID();
    //printf("Entering stepHander()\n");
    // Determine if we are processing a step due to a trap breakpoint that we are bypassing.
    if (coreControl[coreID].trapAddress)
    {
        //printf("Completed stepping around a false trap. invalidate the erat and continue.\n");
        // Free the erat entry that is above the watermark by setting Valid=0. The ExClass=1 is set therefore we cannot use eratilx.
        eratwe(15, 0, 0, MMUCR0_TLBSEL_IERAT); 
        // Remove the watermark 
        eratwatermark(15, MMUCR0_TLBSEL_IERAT); 
        // Reset indicator that a trap step is in progress
        coreControl[coreID].trapAddress = 0;

        // Test to see if we are also in the process of performing a tool initiated step at the the location of a trap that
        // is set in another process. If so, we do not want to reset the step condition.
        if (coreControl[coreID].hwt[myThreadId].stepAddress == 0)
        {
            // Reset the step interrupt condition in the debug registers
            uint64_t dbcr0 = mfspr(SPRN_DBCR0);
            mtspr(SPRN_DBCR0, (dbcr0 | DBCR0_IDM) & ~(DBCR0_ICMP)); // Turn off the ICMP enablement in the DBCR0 register
            // Reset the step interrupt condition in the thread context
            context->dbcr0 &= ~(DBCR0_ICMP);    
        }
        isync();
        mtspr(SPRN_DBSR, DBSR_ICMP); // Clear the status indicator for the ICMP interrupt
        // Re-enable the other threads on the core
        uint64_t mask = ~(1<<(myThreadId));
        mtspr(SPRN_TENS, mask); // Set the enable bits for other threads on this core
        // Re-enable EE and CE interrupts in the context's MSR (these were disabled when setting up the step)
        context->msr |= (MSR_CE | MSR_EE);

        // Return indication that this step operation was handled.
        //printf("Returning 1 from stepHandler");
        return 1; 
    }
    // Is this a step operation to process a DAC interrupt condition for watchpoint processing 
    else if (coreControl[coreID].hwt[myThreadId].dbsr & TOOL_DBSR_DAC_MATCH)
    {
        //printf("StepHandler DAC trapafter condition\n");
        // Yes, the original interrupt was a DAC. Make it look like we are processing the DAC interrupt
        context->dbsr = coreControl[coreID].hwt[myThreadId].dbsr;
        context->dear = coreControl[coreID].hwt[myThreadId].dear; 
        coreControl[coreID].hwt[myThreadId].dbsr = 0;
        context->dear = coreControl[coreID].hwt[myThreadId].dear = 0; 
        // Restore the DAC enablement.
        mtspr(SPRN_DBCR0,coreControl[coreID].hwt[myThreadId].dbcr0);
        // Reset the step interrupt condition in the debug status register
        mtspr(SPRN_DBSR, DBSR_ICMP);
        isync();
        // Re-enable EE and CE interrupts in the context's MSR (these were disabled when setting up the step)
        context->msr |= (MSR_CE | MSR_EE);
        // Reset the step interrupt condition in the thread context
        context->dbcr0 &= ~(DBCR0_ICMP);    

        // allow this to be delivered through the signal path. The step condition will now look like the original DAC interrupt
        return 0;

    }
    else if(coreControl[coreID].hwt[myThreadId].stepAddress == context->ip)
    {
        // We likely did a step detour through the kernel and ended up back at the orginal instruction that we 
        // started the step request with. We will reset the dbsr status indicator and just return a value of 
        // 1 here, indicating that we have handled this flow and we want another step to be immediately done.
        mtspr(SPRN_DBSR, DBSR_ICMP);
        isync();
        return 1;
    }
    else
    {
        // Processing a tool initiated step request
        //printf("StepHandler processing tool initialiated step\n");
        // Reset the step interrupt condition in the debug registers
        uint64_t dbcr0 = mfspr(SPRN_DBCR0);
        mtspr(SPRN_DBCR0, (dbcr0 | DBCR0_IDM) & ~(DBCR0_ICMP)); // Turn off the ICMP enablement in the DBCR0 register
        isync();
        // Reset the step controls in the context
        context->dbcr0 &= ~(DBCR0_ICMP);
        // Consider the step operation requested by the tool successful. Allow it's occurrence to be signalled normally
        coreControl[coreID].hwt[myThreadId].stepAddress = 0; // reset our forward progress indicator
        coreControl[coreID].hwt[myThreadId].pendingStepKthread = NULL;
        return 0;
    }
}

void BreakpointController::prepareToStep(KThread_t *kthread)
{
    //printf("prepareToStep\n");
    // Make sure MSR[DE]  is off in our current MSR so that we do not get a step interrupt in the kernel.
    mtmsr(MSR_CNK_DISABLED);
    ppc_msync();
    Regs_t *context = &kthread->Reg_State;
    // Make sure that the status register indicator for ICMP is reset (still in reset state by IntHandler_Debug?)
    mtspr(SPRN_DBSR, DBSR_ICMP);

    // If the current kthread is equal to the passed in kthread, then we need to set the hardware directly since we will
    // be restoring the context coming out of the signal handling flow without passing through the scheduler. If we 
    // are not running in the kthread that we are stepping, then we must setup the context of the target kthread and tell 
    // the scheduler to restore the debug registers before dispatching.
    if (GetMyKThread() == kthread)
    {
        uint64_t dbcr0 = mfspr(SPRN_DBCR0);
        dbcr0 |= DBCR0_ICMP | DBCR0_IDM;
        mtspr(SPRN_DBCR0, dbcr0);
        ppc_msync();
    }
    else
    {
        // Enable the instruction complete debug interrupt condition. Whe the user's MSR with MSR[DE]
        // set, this will generate the ICMP interrupt when that first instruction completes
        GetMyHWThreadState()->launchContextFlags.flag.DebugControlRegs = 1; // Set flag to retore context of debug control registers
        context->dbcr0 |= DBCR0_ICMP | DBCR0_IDM;
    }
    // make sure that MSR[DE] is enabled in the users MSR
    context->msr |= MSR_DE;
    // Store the current instruction pointer so that we can determine if we make forward progress with the step. A non-zero value 
    // in this field is also used as an indication of a pending step operation on this hardware thread.
    coreControl[ProcessorCoreID()].hwt[ProcessorThreadID()].stepAddress = context->ip; 
    coreControl[ProcessorCoreID()].hwt[ProcessorThreadID()].pendingStepKthread = kthread; 
}

//! Add breakpoint
int BreakpointController::add(void *address, uint32_t trapinstruction, AppProcess_t *proc)
{
    // Obtain the write lock. (possibly not needed. CDTI ensure only one add/remove at a time, adding an entry may be safe during trap table read operations)
    TrapTableWriteLock();

    //printf("Adding Break at %016lx\n", address);
    // Determine if this address is already in our table.
    TrapTableEntry* tableElement = findTrapTableEntry(address, 0); 
    
    // if the table is full, tableElement will be NULL
    if (!tableElement)
    {
        printf("(E) Trap table is full.\n");

        TrapTableWriteUnlock();
        return -1;
    }
    // 
    // If an existing entry exists, then we need to modify it
    if (tableElement->instAddress == address)
    {
        // Verify that the breakpoint instruction matches what was already set. We can not support different processes
        // setting different trap words
        if (trapinstruction != tableElement->modifiedValue)
        {
            printf("(E) Conflicting trap instruction requested\n");
            TrapTableWriteUnlock();
            return -1;
        }
        // Adding a breakpoint for this process. Since there is an existing entry, a breakpoint already exists for
        // at least one other process. Add the thread of our process leader into the mask
        tableElement->processMask |= _BN(proc->ProcessLeader_ProcessorID);
    }
    else 
    {
        assert(tableElement->instAddress == NULL); // Design assumption is that if we didnt find our address, we have a free entry

        // Create the new entry in the table
        uint32_t prevInstruction = *((uint32_t*)address);
        tableElement->modifiedValue = trapinstruction;
        tableElement->originalValue = prevInstruction;
        tableElement->processMask = _BN(proc->ProcessLeader_ProcessorID);
        ppc_msync(); // all previous stores must be complete before we activate this entry with a valid instAddress
        tableElement->instAddress = address;
        trapTable.numActiveEntries++;

        // Write trap into the text segment
        *((uint32_t*)address) = trapinstruction;
        ppc_msync();
        // We need to invalidate the icache so that it sees the data modification that was just written through the dcache
        icache_block_inval((void*)address);
    }
    TrapTableWriteUnlock();
    return 0;
}

//! Remove breakpoint
int BreakpointController::remove(void *address, uint32_t instruction, AppProcess_t *proc)
{
    // Obtain the write lock. 
    TrapTableWriteLock();

    //printf("Removing Break at %016lx\n", address);
    // Find the entry in our table.
    TrapTableEntry* tableElement = findTrapTableEntry(address,0);
    if (!tableElement)
    {
        // The table is full and the caller provided an address that was not in the table.
        TrapTableWriteUnlock();
        return -1;
    }
    // Test to see that we found the entry
    if (tableElement->instAddress == address)
    {
        // First reset our process mask.
        tableElement->processMask &= ~_BN(proc->ProcessLeader_ProcessorID);

        // If we are the last process to remove this breakpoint, we must write the original value back into the text segment
        if (tableElement->processMask == 0)
        {
            // Test the instruction value passed in to verify that we are restoring the original instruction
            if (instruction != tableElement->originalValue)
            {
                // Caller is not providing the correct original instruction. Fail the operation

                // Restore the process mask since we are not removing this breakpoint
                tableElement->processMask |= _BN(proc->ProcessLeader_ProcessorID);

                TrapTableWriteUnlock();
                return -1;
            }
            // write the provided instruction into the the text segment
            *((uint32_t*)address) = instruction;
            ppc_msync();
            // We need to invalidate the icache so that it sees the data modification that was just written through the dcache
            icache_block_inval((void*)address);
            ppc_msync();

            // Decrement the number of active entries
            trapTable.numActiveEntries--;

            // Remove the entry from the trap table
            removeTrapTableEntry(tableElement);
        }
    }
    else
    {
        // We didnt find an entry in the trap table!
        TrapTableWriteUnlock();
        return -1;
    }
    TrapTableWriteUnlock();
    return 0;
}

uint32_t BreakpointController::read(uint32_t* address, AppProcess_t *proc)
{
    // Lock the trap table for reading.
    TrapTableReadLock();

    TrapTableEntry* tableElement = findTrapTableEntry(address,0);
    if (!tableElement || !tableElement->instAddress)
    {
        // No entry in the table exists, just read the data normally
        TrapTableReadUnlock();
        return(*address);
    }
    // This is a tracked breakpoint modification. Test to see if our process is registered for it
    if (tableElement->processMask & _BN(proc->ProcessLeader_ProcessorID))
    {
        // Yes it is registered. This means that the value in memory (i.e. the trap instruction) is the value we want to return
        TrapTableReadUnlock();
        return(*address);
    }
    else
    {
        // This is a tracked address, however this process did not participate in the modification
        TrapTableReadUnlock();
        return(tableElement->originalValue);
    }
}

void BreakpointController::trapTableAllocate()
{
    // Test to see if we have initialized the trap table. We must do this once per job.
    if (!trapTable.trapTableAllocated)
    {
        // Lock the shared kernel storage area
        Kernel_Lock(&NodeState.sharedDataAreaLock);
        // Initialize the area
        memset(NodeState.sharedWorkArea, 0x00, sizeof(NodeState.sharedWorkArea));
        // Setup addressibility
        trapTable.entry = (TrapTableEntry*)NodeState.sharedWorkArea;
        // set indicator in the breakpoint object that this has been completed.
        trapTable.trapTableAllocated = 1;
    }
}

void BreakpointController::trapTableFree()
{
    // Test indicator to see if we have aquired the shared kernel storage area for use by the breakpoint table.
    if (trapTable.trapTableAllocated)
    {
        // Free the lock on the shared area;
        Kernel_Unlock(&NodeState.sharedDataAreaLock);
        trapTable.trapTableAllocated = 0;
        trapTable.entry = NULL;

    }
}

KThread_t* BreakpointController::getPendingStepKThread(int processorID, AppProcess_t *proc)
{
    if (coreControl[processorID>>2].hwt[processorID&0x3].stepAddress)
    {
        KThread_t *kthread = coreControl[processorID>>2].hwt[processorID&0x3].pendingStepKthread;
        if (kthread->pAppProc == proc)
        {
            return kthread;
        }
    }
    return NULL;
}

TrapTableEntry* BreakpointController::nextTrapTableEntry(TrapTableEntry *entry)
{
    // Move to next entry. Wrap if necessary
    return(entry == &trapTable.entry[TRAP_TABLE_ENTRIES-1]) ? &trapTable.entry[0] : entry+1;

}

//! Hash the breakpoint address into a trap table entry
TrapTableEntry* BreakpointController::hashBreakpointAddress(void *address)
{
    // The expectation is that the least most significant address bits down to bit position 61 would provide 
    // reasonable distribution for the hashing of breakpoints. For example, if the table is 16K, then address bits 46-61
    // would select the hash class. This algorithm may need to be adjusted/improved in the future.
    uint64_t value = (uint64_t)address;
    return(&trapTable.entry[((value/4) % TRAP_TABLE_ENTRIES)]);
}

//! Remove a TrapTableEntry from the Trap table
int BreakpointController::removeTrapTableEntry(TrapTableEntry* removed)
{
    // To remove a trap table entry, we need to rehash all the following entries from this point forward until 
    // we reach an available entry in the table.
    removed->instAddress = NULL;
    ppc_msync(); // The previous instAddress field is used to indicate the validity of the remaining fields in the entry.
    removed->modifiedValue = 0;
    removed->originalValue = 0;
    removed->processMask = 0;
    TrapTableEntry *current = nextTrapTableEntry(removed);

    // We only need to iterate rehashing the addresses until we find a free entry in the list. Even if the table
    // was completely full, we now we will eventually reach a free entry because we have just removed an entry.
    while (current->instAddress) 
    {
        // We need to rehash this address and potentially move the current entry to a new location
        TrapTableEntry *newLocation = findTrapTableEntry(current->instAddress,0);
        
               
        if (newLocation != current)
        {
            assert(newLocation->instAddress == NULL); // this is expected to be a free entry
            // move the entry to its new location
            *newLocation = *current;
            current->instAddress = NULL;
            current->modifiedValue = 0;
            current->originalValue = 0;
            current->processMask = 0;
        }
        else
        {
            // We didnt need to move this entry, however, we still need to iterate to the first free entry
        }
        current=nextTrapTableEntry(current);
    }
    return 0;
}

//! Return reference to a TrapTableEntry. If the address is not found in the table, the next available entry will be returned. 
TrapTableEntry* BreakpointController::findTrapTableEntry(void* address, int fromTrapHandler)
{
    TrapTableEntry *entry = hashBreakpointAddress(address);
    TrapTableEntry *currentEntry = entry;
    int depth = 0;

#if TRAPTABLE_TEST_HASHDEPTH 
    int test_hash_depth = TRAPTABLE_TEST_HASHDEPTH;
#endif

    do
    {
        // Does this entry match the address that we want?
        if (currentEntry->instAddress == address)
        {
            // Yes, we found a matching entry. Return it.
            if (!fromTrapHandler) // Too much overhead to make an entry if we are within the trap handler. Would also flood the buffer.
            {
                Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLTTBFD, (uint64_t)address, depth, currentEntry->processMask, 0);
            }
            return currentEntry;
        }
#if TRAPTABLE_TEST_HASHDEPTH
        else if ((test_hash_depth--) == 0)
        {
            return currentEntry;
        }
#else
        else if (currentEntry->instAddress == NULL)
        {
            // Entry does not exist in the table. Return the element to the caller since this is the next available entry to be used.
            if (!fromTrapHandler)  // Too much overhead to make an entry if we are within the trap handler. Would also flood the buffer.
            {
                Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLTTBNF, (uint64_t)address, depth, 0, 0);
            }
            return currentEntry;
        }

#endif
        // Move to next entry. Wrap if necessary
        currentEntry = nextTrapTableEntry(currentEntry);
        // record the depth for the flight recorder
        depth++;
    } while (entry != currentEntry);

    // If we reached this point, the requested entry was not found and there was no additional space found in the entire table.
    // For this case we return a NULL pointer.
    if (!fromTrapHandler)  // Too much overhead to make an entry if we are within the trap handler. Would also flood the buffer.
    {
        Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLTTBFU, (uint64_t)address, depth, 0, 0);
    }
    return NULL;
}


#if USE_L2ATOMIC_TRAPLOCK

void BreakpointController::TrapTableReadLock()
{
    uint64_t value;
    do
    {
        value = L2_AtomicLoad(&TrapTableReaderLock);
        if (value < TRAPTABLE_READERBLOCK)
        {
            value = L2_AtomicLoadIncrement(&TrapTableReaderLock);
            // test again now that we did the atomic increment
            if (value < TRAPTABLE_READERBLOCK)   
                return; // We are done.
            L2_AtomicLoadDecrement(&TrapTableReaderLock); // Back out our increment because a writer is active. 
        }
        // Is there a write operation in progress or a write operation pending?
    } while (value >= TRAPTABLE_READERBLOCK);
}

void BreakpointController::TrapTableReadUnlock()
{
    L2_AtomicLoadDecrement(&TrapTableReaderLock); // Simply decrement our count.
}

void BreakpointController::TrapTableWriteLock()
{
    uint64_t value;
    do
    {
        while (L2_AtomicLoad(&TrapTableWriterLock))
        {
            asm volatile("nop;");
            asm volatile("nop;");
            asm volatile("nop;");
            asm volatile("nop;");
            asm volatile("nop;");
        }
        value = L2_AtomicLoadIncrement(&TrapTableWriterLock);
    } while(value); // loop until we are the one writer that has incremented the lock from 0 to 1

    L2_AtomicStoreAdd(&TrapTableReaderLock, TRAPTABLE_READERBLOCK); // Add the blocker value into the reader's lockword.
    // Wait for the readers to drain
    do
    {
        asm volatile("nop;");
        asm volatile("nop;");
        asm volatile("nop;");
        asm volatile("nop;");
        asm volatile("nop;");
        value = L2_AtomicLoad(&TrapTableReaderLock);
    } while( value != TRAPTABLE_READERBLOCK);
    // Ok, we can proceed. We have the exclusive lock on the breakpoint table and all readers are drained.
}

void BreakpointController::TrapTableWriteUnlock()
{
    L2_AtomicStoreAdd(&TrapTableReaderLock, (uint64_t)(-TRAPTABLE_READERBLOCK)); // allow readers by subtracting the blocker value
    L2_AtomicStore(&TrapTableWriterLock, 0); // free the writer lock to allow other writers 
}


#else

void BreakpointController::TrapTableReadLock()
{
    ppc_msync();

    uint64_t value;
    do
    {
        value = LoadReserved(&(trapTable.lock.atom));
        // Is there a write operation in progress or a write operation pending?
        if (value >= TRAPTABLE_READERBLOCK)
        {
            // Yes, wait until the write operation clears.
            ThreadPriority_Low();
            do
            {
                asm volatile("nop;");
                asm volatile("nop;");
                asm volatile("nop;");
                asm volatile("nop;");
                asm volatile("nop;");
                value = LoadReserved(&(trapTable.lock.atom));
            } while (value >= TRAPTABLE_READERBLOCK);
            ThreadPriority_Medium();
        }
    } while(!StoreConditional(&(trapTable.lock.atom), value+1));
}

void BreakpointController::TrapTableReadUnlock()
{
    fetch_and_sub64( &trapTable.lock, 1);
}

void BreakpointController::TrapTableWriteLock()
{
    ppc_msync();

    uint64_t value;
    do
    {
        value = LoadReserved(&(trapTable.lock.atom));
        // Is there a write operation in progress or a write operation pending?
        if (value >= TRAPTABLE_READERBLOCK)
        {
            // Yes, wait until the write operation clears.
            ThreadPriority_Low();
            do
            {
                asm volatile("nop;");
                asm volatile("nop;");
                asm volatile("nop;");
                asm volatile("nop;");
                asm volatile("nop;");
                value = LoadReserved(&(trapTable.lock.atom));
            } while (value >= TRAPTABLE_READERBLOCK);
            ThreadPriority_Medium();
        }
    } while(!StoreConditional(&(trapTable.lock.atom), value+TRAPTABLE_READERBLOCK));
    // Now we must wait for all of the pending read operations to drain.
    if (value != TRAPTABLE_READERBLOCK)
    {
        ThreadPriority_Low();
        do
        {
            asm volatile("nop;");
            asm volatile("nop;");
            asm volatile("nop;");
            asm volatile("nop;");
            asm volatile("nop;");
            value = trapTable.lock.atom;
        } while (value != TRAPTABLE_READERBLOCK);
        ThreadPriority_Medium();
    }
    // Ok, we can proceed. We have the exclusive lock on the breakpoint table and all readers are drained.
}

void BreakpointController::TrapTableWriteUnlock()
{
    fetch_and_sub64( &trapTable.lock, TRAPTABLE_READERBLOCK);
}


#endif
//------------------------------
// Class WatchpointManager
//------------------------------

void WatchpointManager::init()
{
    for (int i=0; i< CONFIG_MAX_APP_CORES; i++)
    {
        for (int j=0; j<2; j++)
        {
            watchpointControl[i].dac_pair[j].address_base = 0;
            watchpointControl[i].dac_pair[j].address_mask = 0;
            watchpointControl[i].dac_pair[j].refcount = 0;
        }
    }
}

CmdReturnCode WatchpointManager::setWatchpoint(uint64_t addr, int length, WatchType watchtype, KThread_t *kthread)
{
    int coreid = ProcessorCoreID();

    //printf("Set watchpoint entry. core:%d addr1:%016lx refcount1:%ld addr2:%016lx refcount2:%ld", coreid,
    //       (uint64_t)watchpointControl[coreid].dac_pair[0].address_base,
    //       (uint64_t)watchpointControl[coreid].dac_pair[0].refcount,
    //       (uint64_t)watchpointControl[coreid].dac_pair[1].address_base,
    //       (uint64_t)watchpointControl[coreid].dac_pair[1].refcount); 

    Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLSSWAT, 
           (uint64_t)watchpointControl[coreid].dac_pair[0].address_base,
           (uint64_t)watchpointControl[coreid].dac_pair[0].refcount,
           (uint64_t)watchpointControl[coreid].dac_pair[1].address_base,
           (uint64_t)watchpointControl[coreid].dac_pair[1].refcount); 

    // The length must be a power of of 2, i.e. 1,2,4,8,16,32,64,...
    if (popcnt64((uint64_t)length) != 1)
    {
        return CmdLngthErr;
    }
    // The address cannot  be NULL
    if (!addr)
    {
        //printf("Address cant be NULL for watchpoint\n");
        return CmdAddrErr;
    }
    // The address must be on a boundary equal to the length. e.g an 8 byte watch length must be aligned on an 8 byte boundary
    uint64_t addr_mask = ~((uint64_t)(length-1));
    if ((uint64_t)(length-1) & addr)
    {
        return CmdAddrErr;
    }
    HWThreadState_t *hwt = GetMyHWThreadState();
    // Determine if a DAC pair is already setup with the address and length that we need.
    int foundEntry = 0;
    int pair_index = 0;
    for (pair_index=0; pair_index<2; pair_index++)
    {
        if ((watchpointControl[coreid].dac_pair[pair_index].address_base == addr) &&
            (watchpointControl[coreid].dac_pair[pair_index].address_mask == addr_mask))
        {
            // The core is setup with this address and length. Check to see if the corresponding watchpoint slot in 
            // the kthread is available for assignment.
            if ((pair_index == 0) && kthread->watch1Type)
            {
                //printf("CmdHwdUnavail due to pair_index == 0 and kthread->watch1Type\n");
                return CmdHwdUnavail;
            }
            else if ((pair_index == 1) && kthread->watch2Type)
            {
                //printf("CmdHwdUnavail due to pair_index == 1 and kthread->watch2Type\n");
                return CmdHwdUnavail;
            }
            // We can use this active entry
            foundEntry = 1;
            break;
        }
    }
    if (!foundEntry)
    {
        for (pair_index=0; pair_index<2; pair_index++)
        {
            if (watchpointControl[coreid].dac_pair[pair_index].address_base == 0)
            {
                if (((pair_index == 0) && !(kthread->watch1Type)) ||
                    ((pair_index == 1) && !(kthread->watch2Type)))
                {
                    // Found an available entry.

                    // Before we can use this entry, we need to test to see if this address range
                    // overlaps with an address range on the other watchpoint defined in this kthread

                    uint64_t my_addr_start = addr;
                    uint64_t my_addr_end = addr + ((~addr_mask)+1);
                    uint64_t other_addr_start = 0;
                    uint64_t other_addr_end = 0;
                    if ((pair_index == 0) && (kthread->watch2Type)) // Is the other watch set?
                    {
                        other_addr_start = kthread->Reg_State.dac3;
                        other_addr_end = other_addr_start + (~(kthread->Reg_State.dac4) + 1);
                    }
                    else if ((pair_index == 1) && (kthread->watch1Type))
                    {
                        other_addr_start = kthread->Reg_State.dac1;
                        other_addr_end = other_addr_start + (~(kthread->Reg_State.dac2) + 1);
                    }
                    if ((other_addr_start && (other_addr_end > other_addr_start)) &&
                        (((my_addr_start >= other_addr_start) && (my_addr_start < other_addr_end)) ||
                         ((other_addr_start >= my_addr_start) && (other_addr_start < my_addr_end)) ||
                         ((my_addr_end > other_addr_start) && (my_addr_end <= other_addr_end))     ||
                         ((other_addr_end > my_addr_start) && (other_addr_end <= my_addr_end))))
                    {
                        //printf("CmdWatchConflict due to range overlap\n");
                        return CmdWatchConflict;
                    }
                    watchpointControl[coreid].dac_pair[pair_index].address_base = addr;
                    watchpointControl[coreid].dac_pair[pair_index].address_mask = addr_mask;
                    foundEntry = 1;
                    break;
                }
            }
        }
    }
    if (!foundEntry)
    {
        // There is a resource conflict. We cannot set this watchpoint
        //printf("CmdHwdUnavail could not find an acceptable DAC pair candidate. pair0:%016lx pair1:%016lx\n",
        //       watchpointControl[coreid].dac_pair[0].address_base,
        //       watchpointControl[coreid].dac_pair[1].address_base );
        return CmdHwdUnavail;
    }
    // Enable the scheduler to restore the debug control registers from the kthread context when dispatching a thread
    hwt->launchContextFlags.flag.DebugControlRegs = 1;

    // Turn on the debug enable bit in the saved context's msr 
    kthread->Reg_State.msr |= MSR_DE;

    // Determine if we are working with the DAC1/DAC2 or the DAC3/DAC4 pair
    switch(pair_index)
    {
    case 0:
        // We are working with DAC1/DAC2

        // Bump the reference count in the watchpoint manager
        watchpointControl[coreid].dac_pair[pair_index].refcount++;

        // Set the watch1Type flag
        kthread->watch1Type = watchtype;
        kthread->Reg_State.dac1 = addr;
        kthread->Reg_State.dac2 = addr_mask;
        mtspr(SPRN_DAC1, kthread->Reg_State.dac1);
        mtspr(SPRN_DAC2, kthread->Reg_State.dac2);
        kthread->Reg_State.dbcr0 &= ~(DBCR0_DAC1(-1) | DBCR0_DAC2(-1));
        kthread->Reg_State.dbcr2 &= ~(DBCR2_DAC1US(-1) | DBCR2_DAC1ER(-1) | DBCR2_DAC2US(-1) | DBCR2_DAC2ER(-1)); 

        // Match on user mode and effective address
        kthread->Reg_State.dbcr2 |= (DBCR2_DAC1ER_EFF | DBCR2_DAC1US_USER | DBCR2_DAC2ER_EFF | DBCR2_DAC2US_USER | DBCR2_DAC12M ); 

        switch (watchtype)
        {
        case WatchRead:
            kthread->Reg_State.dbcr0 |= (DBCR0_DAC1_LD | DBCR0_DAC2_LD  | DBCR0_IDM); // Match on read 
            break;
        case WatchWrite:
            kthread->Reg_State.dbcr0 |= (DBCR0_DAC1_ST | DBCR0_DAC2_ST  | DBCR0_IDM); // Match on write
            break;
        case WatchReadWrite:
            kthread->Reg_State.dbcr0 |= (DBCR0_DAC1_ANY | DBCR0_DAC2_ANY | DBCR0_IDM); // Match on read or write
            break;
        default:
            ;
        }
        break;
    case 1:
        // Bump the reference counts in the watchpoint manager
        watchpointControl[coreid].dac_pair[pair_index].refcount++;
        // Set the watch2Type flag
        kthread->watch2Type = watchtype;

        // We are working with DAC3/DAC4
        kthread->Reg_State.dac3 = addr;
        kthread->Reg_State.dac4 = addr_mask;
        mtspr(SPRN_DAC3, kthread->Reg_State.dac3);
        mtspr(SPRN_DAC4, kthread->Reg_State.dac4);

        kthread->Reg_State.dbcr0 &= ~(DBCR0_DAC3(-1) | DBCR0_DAC4(-1));
        kthread->Reg_State.dbcr3 &= ~(DBCR3_DAC3US(-1) | DBCR3_DAC3ER(-1) | DBCR3_DAC4US(-1) | DBCR3_DAC4ER(-1));
        // Match on user mode and effective address 
        kthread->Reg_State.dbcr3 |= (DBCR3_DAC3ER_EFF | DBCR3_DAC3US_USER | DBCR3_DAC4ER_EFF | DBCR3_DAC4US_USER | DBCR3_DAC34M); 
        switch (watchtype)
        {
        case WatchRead:
            kthread->Reg_State.dbcr0 |= (DBCR0_DAC3_LD | DBCR0_DAC4_LD); // Match on read 
            break;
        case WatchWrite:
            kthread->Reg_State.dbcr0 |= (DBCR0_DAC3_ST | DBCR0_DAC4_ST); // Match on write
            break;
        case WatchReadWrite:
            kthread->Reg_State.dbcr0 |= (DBCR0_DAC3_ANY | DBCR0_DAC4_ANY); // Match on read or write
            break;
        default:
            ;
        }
        break;
    default:
        //printf("Unexpected CmdWatchConflict. Default leg of switch\n");
        return CmdWatchConflict;
    }
    // If the current kthread is equal to the passed in kthread, then we need to set the hardware directly since we will
    // be restoring the context coming out of the signal handling flow without passing through the scheduler. We still keep
    // the launchContextFlags set so when/if this thread does context switch in the future. the correct values will be restored. 
    if (GetMyKThread() == kthread)
    {
        if (hwt->launchContextFlags.flag.DebugControlRegs)
        {
            mtspr(SPRN_DBCR0, kthread->Reg_State.dbcr0);
        }
    }
    //printf("Set watchpoint success. core:%d addr1:%016lx refcount1:%ld addr2:%016lx refcount2:%ld", coreid,
    //       (uint64_t)watchpointControl[coreid].dac_pair[0].address_base,
    //       (uint64_t)watchpointControl[coreid].dac_pair[0].refcount,
    //       (uint64_t)watchpointControl[coreid].dac_pair[1].address_base,
    //       (uint64_t)watchpointControl[coreid].dac_pair[1].refcount); 
    Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLESWAT, 
           (uint64_t)watchpointControl[coreid].dac_pair[0].address_base,
           (uint64_t)watchpointControl[coreid].dac_pair[0].refcount,
           (uint64_t)watchpointControl[coreid].dac_pair[1].address_base,
           (uint64_t)watchpointControl[coreid].dac_pair[1].refcount); 

    return CmdSuccess;
}
   
CmdReturnCode WatchpointManager::resetWatchpoint(uint64_t addr, KThread_t *kthread)
{
    int coreid = ProcessorCoreID();

    //printf("Reset watchpoint entry. core:%d addr1:%016lx refcount1:%ld addr2:%016lx refcount2:%ld", coreid,
    //       (uint64_t)watchpointControl[coreid].dac_pair[0].address_base,
    //       (uint64_t)watchpointControl[coreid].dac_pair[0].refcount,
    //       (uint64_t)watchpointControl[coreid].dac_pair[1].address_base,
    //       (uint64_t)watchpointControl[coreid].dac_pair[1].refcount); 

    Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLSRWAT, 
           (uint64_t)watchpointControl[coreid].dac_pair[0].address_base,
           (uint64_t)watchpointControl[coreid].dac_pair[0].refcount,
           (uint64_t)watchpointControl[coreid].dac_pair[1].address_base,
           (uint64_t)watchpointControl[coreid].dac_pair[1].refcount); 

    // Search the watchpoint manager for a matching address
    bool entryFound = 0;
    for (int pair_index=0; pair_index<2; pair_index++)
    {
        if (watchpointControl[coreid].dac_pair[pair_index].address_base == addr)
        {
            // Found an entry
            entryFound = 1;

            // Cleanup the registers in the kthread context and adjust reference counts
            // Determine if we are working with the DAC1/DAC2 or the DAC3/DAC4 pair
            switch(pair_index)
            {
            case 0:
                // We are working with DAC1/DAC2

                if (kthread->watch1Type)
                {
                    // Decrement the reference count within the watchpoint controller
                    watchpointControl[coreid].dac_pair[pair_index].refcount--;
                    // If the reference count is zero, free up this dac pair.
                    if (watchpointControl[coreid].dac_pair[pair_index].refcount == 0)
                    {
                        watchpointControl[coreid].dac_pair[pair_index].address_base = 0;
                    }
                    // Reset the active flag in the kthread
                    kthread->watch1Type = 0;
                    kthread->Reg_State.dac1 = 0;
                    kthread->Reg_State.dac2 = 0;
                    kthread->Reg_State.dbcr0 &= ~(DBCR0_DAC1(-1) | DBCR0_DAC2(-1));
                    mtspr(SPRN_DBCR0, kthread->Reg_State.dbcr0); // Turn off enablement of DAC1/2 matches in the current DBCR0
                }
                break;
            case 1:
                // We are working with DAC3/DAC4
                if (kthread->watch2Type)
                {
                    // Decrement the reference count within the watchpoint controller
                    watchpointControl[coreid].dac_pair[pair_index].refcount--;
                    // If the reference count is zero, free up this dac pair.
                    if (watchpointControl[coreid].dac_pair[pair_index].refcount == 0)
                    {
                        watchpointControl[coreid].dac_pair[pair_index].address_base = 0;
                    }
                    // Reset the active flag in the kthread
                    kthread->watch2Type = 0;
                    kthread->Reg_State.dac3 = 0;
                    kthread->Reg_State.dac4 = 0;
                    kthread->Reg_State.dbcr0 &= ~(DBCR0_DAC3(-1) | DBCR0_DAC4(-1));
                    mtspr(SPRN_DBCR0, kthread->Reg_State.dbcr0); // Turn off enablement of DAC3/4 matches in the curent DBCR0
                }
                break;
            default:
                ;
            }
        }
    }
    //printf("Reset watchpoint exit. core:%d addr1:%016lx refcount1:%ld addr2:%016lx refcount2:%ld", coreid,
    //       (uint64_t)watchpointControl[coreid].dac_pair[0].address_base,
    //       (uint64_t)watchpointControl[coreid].dac_pair[0].refcount,
    //       (uint64_t)watchpointControl[coreid].dac_pair[1].address_base,
    //       (uint64_t)watchpointControl[coreid].dac_pair[1].refcount); 

    Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLERWAT, 
           (uint64_t)watchpointControl[coreid].dac_pair[0].address_base,
           (uint64_t)watchpointControl[coreid].dac_pair[0].refcount,
           (uint64_t)watchpointControl[coreid].dac_pair[1].address_base,
           (uint64_t)watchpointControl[coreid].dac_pair[1].refcount); 

    if (entryFound)
    {
        //printf("CmdSuccess sucessfully reset breakpoint\n");
        return CmdSuccess;
    }
    else
    {
        //printf("CmdAddrErr did not find a watchpoint to reset\n");
        return CmdAddrErr;
    }
}

// This function is to be called when a kthread is exiting
void WatchpointManager::cleanupThreadWatchpoints(KThread_t *kthread)
{
    if (kthread->watch1Type)
    {
        // Decrement the reference count in the watchpoint manager
        int coreid = ProcessorCoreID();
        watchpointControl[coreid].dac_pair[0].refcount--;
        // If the reference count is zero, free up this dac pair.
        if (watchpointControl[coreid].dac_pair[0].refcount == 0)
        {
            watchpointControl[coreid].dac_pair[0].address_base = 0;
        }
        kthread->watch1Type = 0;
        kthread->Reg_State.dac1 = 0;
        kthread->Reg_State.dac2 = 0;
        kthread->Reg_State.dbcr0 &= ~(DBCR0_DAC1(-1) | DBCR0_DAC2(-1));
        kthread->Reg_State.dbcr2 &= ~(DBCR2_DAC1US(-1) | DBCR2_DAC1ER(-1) | DBCR2_DAC2US(-1) | DBCR2_DAC2ER(-1) | DBCR2_DAC12M );
    }
    if (kthread->watch2Type)
    {
        // Decrement the reference count in the watchpoint manager
        int coreid = ProcessorCoreID();
        watchpointControl[coreid].dac_pair[1].refcount--;
        // If the reference count is zero, free up this dac pair.
        if (watchpointControl[coreid].dac_pair[1].refcount == 0)
        {
            watchpointControl[coreid].dac_pair[1].address_base = 0;
        }
        kthread->watch2Type = 0;
        kthread->Reg_State.dac3 = 0;
        kthread->Reg_State.dac4 = 0;
        kthread->Reg_State.dbcr0 &= ~(DBCR0_DAC3(-1) | DBCR0_DAC4(-1));
        kthread->Reg_State.dbcr3 &= ~(DBCR3_DAC3US(-1) | DBCR3_DAC3ER(-1) | DBCR3_DAC4US(-1) | DBCR3_DAC4ER(-1) | DBCR3_DAC34M);
    }
}

uint64_t WatchpointManager::migrateWatchpoints(KThread_t *kthread, int new_hwthread, int clone)
{
    int fromCore = ProcessorCoreID();
    int toCore = new_hwthread>>2;
    uint64_t returnMask = 0;
    //printf("Migrating watchpoints. clone=%d\n", clone);
    for (int dacIndex = 0; dacIndex<2; dacIndex++)
    {
        if (((dacIndex == 0) && kthread->watch1Type) || ((dacIndex == 1) && kthread->watch2Type))
        {
            // Watch is active. Need to take action

            // If we are moving to a new core, we need to do some more work
            if (fromCore != toCore)
            {
                // save working copy of the address and mask from the fromCore
                uint64_t fromCoreAddress = watchpointControl[fromCore].dac_pair[dacIndex].address_base;
                uint64_t fromCoreMask = watchpointControl[fromCore].dac_pair[dacIndex].address_mask;

                // Decrement the reference count in the from core/hardware thread area if this is not a clone operation
                if (clone == 0)
                {
                    watchpointControl[fromCore].dac_pair[dacIndex].refcount--;
                    // If the ref count is now zero, clear the base address to indicate this entry is free
                    if (watchpointControl[fromCore].dac_pair[dacIndex].refcount == 0)
                    {
                        watchpointControl[fromCore].dac_pair[dacIndex].address_base = 0;
                    }
                }
                // Look in the destination core to see if the address is setup in the DAC
                if (watchpointControl[toCore].dac_pair[dacIndex].address_base == 0)
                {
                    // Setup the address and mask in the free DAC pair
                    watchpointControl[toCore].dac_pair[dacIndex].address_base = fromCoreAddress;
                    watchpointControl[toCore].dac_pair[dacIndex].address_mask = fromCoreMask;
                }
                // Look in the destination core to see if the same watchpoint is set in the new hardware thread 
                if ((fromCoreAddress == watchpointControl[toCore].dac_pair[dacIndex].address_base) &&
                    (fromCoreMask == watchpointControl[toCore].dac_pair[dacIndex].address_mask))
                {
                    //printf("Successful migration of watchpoints\n");
                    // Bump the reference count for the new thread location
                    watchpointControl[toCore].dac_pair[dacIndex].refcount++;
                }
                else
                {
                    // This is a conflict with the core scoped DAC register usage.
                    // Set bit in mask field to indicate which watchpoint failed
                    returnMask |= _BN(dacIndex);
                    // Reset the fields associated with the watchpoint in the kthread object since we could not keep the watchpoint.
                    if (dacIndex == 0)
                    {
                        kthread->watch1Type = 0;
                        kthread->Reg_State.dac1 = 0;
                        kthread->Reg_State.dac2 = 0;
                        kthread->Reg_State.dbcr0 &= ~(DBCR0_DAC1(-1) | DBCR0_DAC2(-1));
                        kthread->Reg_State.dbcr2 &= ~(DBCR2_DAC1US(-1) | DBCR2_DAC1ER(-1) | DBCR2_DAC2US(-1) | DBCR2_DAC2ER(-1) | DBCR2_DAC12M );
                    }
                    if (dacIndex == 1)
                    {
                        kthread->watch2Type = 0;
                        kthread->Reg_State.dac3 = 0;
                        kthread->Reg_State.dac4 = 0;
                        kthread->Reg_State.dbcr0 &= ~(DBCR0_DAC3(-1) | DBCR0_DAC4(-1));
                        kthread->Reg_State.dbcr3 &= ~(DBCR3_DAC3US(-1) | DBCR3_DAC3ER(-1) | DBCR3_DAC4US(-1) | DBCR3_DAC4ER(-1) | DBCR3_DAC34M);
                    }
                }
            }
            else 
            {    
                // The toCore is the same as the fromCore
                // Is this a clone operation? If so, this additional watchpoint must be counted.
                if (clone == 1) 
                {
                    // Bump the reference count for the new thread location
                    watchpointControl[toCore].dac_pair[dacIndex].refcount++;
                }
            }
        }
    }
    return returnMask;
}

//------------------------------
// Class ToolControl
//------------------------------

// Initialize the tool control object
int ToolControl::init(cnv_pd *protectionDomain, cnv_cq *completionQ)
{
    pendingIPIs.atom = 0;
    messageReturnCode = 0;
    outbound_cmd_data = NULL;
    currentMsg = NULL;
    for (int i=0; i<CONFIG_MAX_PROCESSES; i++)
    {
        processEntry[i].init();
    }
    memset((void *)ipiRequest, 0, sizeof(ipiRequest));

    // Start sequence ids from 1.
    _sequenceId = 1;

    assert(0 == cnv_open_dev(&_context));

    // Register memory regions.
    assert(0 == cnv_reg_mr(&_inMessageRegion1, protectionDomain, &_inMessage1, sizeof(_inMessage1), CNV_ACCESS_LOCAL_WRITE));
    assert(0 == cnv_reg_mr(&_inMessageRegion2, protectionDomain, &_inMessage2, sizeof(_inMessage2), CNV_ACCESS_LOCAL_WRITE));
    assert(0 == cnv_reg_mr(&_outMessageRegion, protectionDomain, &_outMessage, sizeof(_outMessage), CNV_ACCESS_LOCAL_WRITE));
    assert(0 == cnv_reg_mr(&_outNotifyMessageRegion, protectionDomain, &_outNotifyMessage, sizeof(_outNotifyMessage), CNV_ACCESS_LOCAL_WRITE));

    // Create queue pair.
    cnv_qp_init_attr attr;
    attr.send_cq = completionQ;
    attr.recv_cq = completionQ;

    assert(0 == cnv_create_qp(&_queuePair, protectionDomain, &attr));
    
    TRACE( TRACE_Toolctl, ("(I) ToolControl::init%s: create queue pair %u for tool control\n", whoami(), _queuePair.qp_num) );

    // Connect to tool daemon on I/O node.
    struct sockaddr_in destAddress;
    destAddress.sin_family = AF_INET;
    destAddress.sin_port = NodeState.toolctldPortAddressDestination;
    destAddress.sin_addr.s_addr = NodeState.ServiceDeviceAddr;
    int err = cnv_connect(&_queuePair, (struct sockaddr *)&destAddress);
    Node_ReportConnect(err, destAddress.sin_addr.s_addr, destAddress.sin_port);
    if (err != 0)
    {
        TRACE( TRACE_Toolctl, ("(E) ToolControl::init%s: cnv_connect() failed, error %d\n", whoami(), err) );
        Kernel_Crash(RAS_KERNELCNVCONNECTFAIL);
    }
    
    TRACE( TRACE_Toolctl, ("(I) ToolControl::init%s: connected to tools daemon\n", whoami()) );

    // Post a receive to get the next message.
    postRecv(_inMessageRegion1);
    postRecv(_inMessageRegion2);

    // Initialized the breakpoint controller
    breakpointController().init();

    // Initialize the watchpoint manager
    watchpointManager().init();

    return 0;
}

int ToolControl::term(void)
{
   int rc = 0;

   // Disconnect from toolctld.
   int err = cnv_disconnect(&_queuePair);
   if (err != 0) {
      TRACE( TRACE_Toolctl, ("(E) ToolControl::term%s: cnv_disconnect() failed, error %d\n", whoami(), err) );
      rc = err;
   }

   // Destroy queue pair.
   err = cnv_destroy_qp(&_queuePair);
   if (err != 0) {
      TRACE( TRACE_Toolctl, ("(E) ToolControl::term%s: cnv_destroy_qp() failed, error %d\n", whoami(), err) );
      rc = err;
   }

   // Deregister memory regions.
   err = cnv_dereg_mr(&_outMessageRegion);
   if (err != 0) {
      TRACE( TRACE_Toolctl, ("(E) ToolControl::term%s: cnv_dereg_mr() failed for outbound message region, error %d\n", whoami(), err) );
      rc = err;
   }
   err = cnv_dereg_mr(&_inMessageRegion1);
   if (err != 0) {
      TRACE( TRACE_Toolctl, ("(E) ToolControl::term%s: cnv_dereg_mr() failed for first inbound message region, error %d\n", whoami(), err) );
      rc = err;
   }
   err = cnv_dereg_mr(&_inMessageRegion2);
   if (err != 0) {
      TRACE( TRACE_Toolctl, ("(E) ToolControl::term%s: cnv_dereg_mr() failed for second inbound message region, error %d\n", whoami(), err) );
      rc = err;
   }
   err = cnv_dereg_mr(&_outNotifyMessageRegion);
   if (err != 0) {
      TRACE( TRACE_Toolctl, ("(E) ToolControl::term%s: cnv_dereg_mr() failed for outbound notify message region, error %d\n", whoami(), err) );
      rc = err;
   }

   
   TRACE( TRACE_Toolctl, ("(I) ToolControl::term%s: termination is complete\n", whoami()) );
   return rc;
}

int ToolControl::setupJob(void)
{
    AppState_t *app = GetMyAppState();
    AppProcess_t *proc = GetFirstProcess(app);

    SetupJobMessage *msg = (SetupJobMessage *)_outMessageRegion.addr;
    memset(msg, 0, sizeof (SetupJobMessage));
    msg->header.service = bgcios::ToolctlService;
    msg->header.type = SetupJob;
    msg->header.length = sizeof(SetupJobMessage); 
    msg->header.jobId = app->JobID;
    msg->header.sequenceId = _sequenceId++;
    msg->header.rank = proc->Rank;
    msg->header.version = ProtocolVersion;
    msg->userId = app->UserID;
    msg->groupId = app->GroupID;
    msg->numRanks = 0;
    Personality_t* personality = GetPersonality();
    msg->nodeId = bgcios::jobctl::coordsToNodeId(personality->Network_Config.Acoord, personality->Network_Config.Bcoord, personality->Network_Config.Ccoord,
                                                 personality->Network_Config.Dcoord, personality->Network_Config.Ecoord);

    while (proc)
    {
        if (proc->State != ProcessState_RankInactive)
        {
            msg->ranks[msg->numRanks++] = proc->Rank;
        }
        proc = proc->nextProcess;
    }

    // Send the SetupJob message to tool daemon.
    postSend(_outMessageRegion, true);
    TRACE( TRACE_Toolctl, ("(I) ToolControl::setupJob%s: sent SetupJob message for %d ranks\n", whoami(), msg->numRanks) );

    return 0;
}

void ToolControl::setupJobAck(struct cnv_mr& region)
{
    // Make sure the SetupJob message was successful.
    SetupJobAckMessage& msg = *((SetupJobAckMessage*)(region.addr));
    if (msg.header.returnCode == bgcios::VersionMismatch) {
       Kernel_WriteFlightLog(FLIGHTLOG, FL_VERMISMAT, msg.header.service, ProtocolVersion, msg.header.errorCode, 0); 
       RASBEGIN(3);
       RASPUSH(msg.header.service);
       RASPUSH(ProtocolVersion);
       RASPUSH(msg.header.errorCode);
       RASFINAL(RAS_KERNELVERSIONMISMATCH);
       Kernel_Crash(1);
    }
    if (msg.header.returnCode != bgcios::Success)
    {
        printf("(E) Tool control setup job failed, return code %u, error code %u\n", msg.header.returnCode, msg.header.errorCode);
    }
    postRecv(region); // Prepare to recieve the next message.

    // Tell waiter ack message has been received. 
    fetch_and_add(&_ackReceived, 1);

    return;
}

int ToolControl::postRecv(cnv_mr& region)
{
    // Build scatter/gather element for inbound message.
    struct cnv_sge recv_sge;
    recv_sge.addr = (uint64_t)region.addr;
    recv_sge.length = region.length;
    recv_sge.lkey = region.lkey;
    
    // Build receive work request.
    struct cnv_recv_wr recvRequest;
    recvRequest.wr_id = region.lkey;
    recvRequest.next = NULL;
    recvRequest.sg_list = &recv_sge;
    recvRequest.num_sge = 1;
    
    // Post a receive for inbound message.
    cnv_recv_wr *badRecvRequest;
    int err = cnv_post_recv(&_queuePair, &recvRequest, &badRecvRequest);
    if (err != 0) {
       return err;
    }
    
    return 0;
}

// Process a tools message
int ToolControl::processMessage(uint32_t lkey)
{
    struct cnv_mr *region;
    if (lkey == _inMessageRegion1.lkey)
    {
        region = &_inMessageRegion1;
    }
    else if (lkey == _inMessageRegion2.lkey)
    {
        region = &_inMessageRegion2;
    }
    else
    {
        printf("bad lkey\n");
        return ENOENT;
    }
    ToolMessage *inMsg = (ToolMessage *)region->addr;
    messageReturnCode = bgcios::Success;
    messageErrorData = 0;

    Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLRCMSG,(((uint64_t)inMsg->header.sequenceId)<<32) + inMsg->header.type,(((uint64_t)inMsg->header.length)<<32) + inMsg->header.rank,inMsg->toolId,0);


    // Test to see if this is truly a tool message
    if (inMsg->header.service == bgcios::ToolctlService)
    {
        // Store the current message (reference) into our object
        currentMsg = inMsg;
        switch (inMsg->header.type)
        {
        case Query:
            query(*region);
            break;
        case Update:
            update(*region);
            break;
        case NotifyAck:
            notifyAck(*region);
            break;
        case SetupJobAck:
            setupJobAck(*region);
            break;
        case Attach:
            attach(*region);
            break;
        case Detach: 
            detach(*region); 
            break;
        case Control:
            control(*region);
            break;
        default:
            TRACE( TRACE_Toolctl, ("(E) ToolControl::processMessage: message type %u is not supported\n", inMsg->header.type) );
            messageReturnCode = bgcios::UnsupportedType;
        }
    }
    else
    {
        TRACE( TRACE_Toolctl, ("(E) ToolControl::processMessage: service %u is not supported\n", inMsg->header.service) );
        messageReturnCode = bgcios::WrongService;
    }

    // If the wrong service was specified or an unsupported type was specifed, send an error ack. All other problems
    // will be reported as acks within the various message types.
    if ((messageReturnCode == bgcios::WrongService) || (messageReturnCode == bgcios::UnsupportedType))
    {   // Build and send ack message if there was an error. 
        TRACE( TRACE_Toolctl, ("(E) ToolControl::processMessage: message validation failed, %u\n", messageReturnCode) );

        ErrorAckMessage *ackMsg = (ErrorAckMessage *)_outMessageRegion.addr;
        memset(ackMsg, 0, sizeof (ErrorAckMessage));
        ackMsg->header.service = bgcios::ToolctlService;
        ackMsg->header.type = ErrorAck;
        ackMsg->header.length = sizeof(ErrorAckMessage); 
        ackMsg->header.jobId = inMsg->header.jobId;
        ackMsg->header.sequenceId = inMsg->header.sequenceId;
        ackMsg->header.rank = inMsg->header.rank; 
        ackMsg->header.version = ProtocolVersion;
        ackMsg->header.returnCode = messageReturnCode;
        ackMsg->header.errorCode = messageErrorData;
        ackMsg->toolId = inMsg->toolId;
        postRecv(*region);
        postSend(_outMessageRegion);
    }
    // return to caller
    return 0;
}

AppProcess_t* ToolControl::getProcessFromMessage()
{
    // Walk through the AppState objects in CNK, looking for 
    // A match with the JobID in the message
    AppState_t *app = NULL;
    for (int i=0; i<CONFIG_MAX_APP_APPSTATES; i++)
    {
        if (NodeState.AppState[i].JobID == currentMsg->header.jobId)
        {
            app = &(NodeState.AppState[i]);

            AppProcess_t *proc = GetFirstProcess(app);
            while (proc)
            {
                if (proc->Rank == currentMsg->header.rank)
                {
                    return proc;
                }
                proc = proc->nextProcess;
            }
            return NULL; // Did not find a process that matches the rank in the message
        }
    }
    return NULL; // Did not find an AppState that matches the JobID in the message
}

// Decrement the count of pending IPIs
void ToolControl::decrementPendingIPIs() 
{
    fetch_and_sub(&pendingIPIs,1);
}

// Increment the count of pending IPIs
void ToolControl::incrementPendingIPIs()
{
    fetch_and_add(&pendingIPIs,1);
}

// Wait for the IPIs to complete. NOTE: CONSIDER ADDING A TIMEOUT TO THIS LOOP
int ToolControl::pollAllMessageIPIsComplete()
{
    if (ATOMIC_READ( (&pendingIPIs) ))
    {
        // Spin at a low priority until lock is release and then try again
        ThreadPriority_Low();
        while (ATOMIC_READ( (&pendingIPIs) ))
        {
            // May need to also look for a kill message or CNK trying to terminate  
        }
        ThreadPriority_High(); 
    }
    TRACE( TRACE_Toolctl, ("(I) ToolControl::pollAllMessageIPIsComplete(): all IPIs completed\n") );
    return 0;
}

int ToolControl::postSend(cnv_mr& region, bool waitForAck)
{
    // Lock the use of the queue pair
    Kernel_Lock(&_qpLock);
    int rc = 0;
    // Build scatter/gather element for outbound message.
    bgcios::MessageHeader *msghdr = (bgcios::MessageHeader *)region.addr;
    struct cnv_sge send_sge;
    send_sge.addr = (uint64_t)region.addr;
    send_sge.length = msghdr->length;
    send_sge.lkey = region.lkey;

    // Build send work request.
    struct cnv_send_wr sendRequest;
    memset(&sendRequest, 0, sizeof(sendRequest));
    sendRequest.next = NULL;
    sendRequest.sg_list = &send_sge;
    sendRequest.num_sge = 1;
    sendRequest.opcode = CNV_WR_SEND;
    sendRequest.wr_id = 0;

    // Post a send for outbound message.
    cnv_send_wr *badSendRequest;
    TRACE( TRACE_Toolctl, ("(I) ToolControl::postSend: posted send of %u bytes\n", send_sge.length) );
    rc = cnv_post_send(&_queuePair, &sendRequest, &badSendRequest);

    // Unlock the use of the queue pair
    Kernel_Unlock(&_qpLock);

    // If requested, wait for an ack message to be received.
    if (waitForAck) {
        if (fetch(&_ackReceived) == 0)
        {
            do {
                Delay(1000);
                IntHandler_IPI_FlushToolCommands();
            } while (fetch(&_ackReceived) == 0);
        }
        fetch_and_sub(&_ackReceived, 1);
        TRACE( TRACE_Toolctl, ("(I) ToolControl::postSend: ack received for last message\n") );
    }
    return rc;
}

// Attach a tool
void ToolControl::attach(struct cnv_mr& region)
{
    AttachMessage& attachMsg = *((AttachMessage*)(region.addr));
    TRACE( TRACE_Toolctl, ("(I) ToolControl::attach: Attach message received from tool %u\n", attachMsg.toolId) );

    // Find the AppState object that corresponds to the supplied jobID.
    AppState_t *app = App_GetAppFromJobid(attachMsg.header.jobId);

    uint16_t numprocesses = 0; 

    // Initialize the attach ack message area 
    AttachAckMessage *ackMsg = (AttachAckMessage *)_outMessageRegion.addr;
    memset(ackMsg, 0, sizeof (AttachAckMessage));


    if (app)
    {
        messageReturnCode = bgcios::ToolRankNotFound;
        Kernel_Lock(&_toolTableLock);
        // We first need to validate that we can add this tool to all of the processes that are being requested
        // This is necessary because if we have a failure in one process, we need to not have added the tool to 
        // any of the processes.
        AppProcess_t *proc = GetFirstProcess(app);
        while (proc)
        {
            if ((attachMsg.procSelect == RanksInNode) || 
                ((attachMsg.procSelect == RankInHeader) && (proc->Rank==currentMsg->header.rank)))
            {
                if ((proc->State == ProcessState_Active) || (proc->State == ProcessState_Started))
                {
                    int procindex = proc->ProcessLeader_ProcessorID;
                    messageReturnCode = processEntry[procindex].validateAdd(attachMsg.toolId, app, attachMsg.priority);
                    if ((messageReturnCode != bgcios::Success) && (attachMsg.procSelect == RankInHeader))
                    {
                        break; // Error detected validating the add of the tool. Exit the while loop 
                    }
                }
            }
            proc = proc->nextProcess;
        }
        // Now it time to do it again, but this time we really mean it.

        // If at least one targetted rank is active, then Success will be set and we can proceed. 
        if (messageReturnCode == bgcios::Success)
        {
            proc = GetFirstProcess(app);
            while (proc)
            {
                if ((attachMsg.procSelect == RanksInNode) || 
                    ((attachMsg.procSelect == RankInHeader) && (proc->Rank==currentMsg->header.rank)))
                {
                    if ((proc->State == ProcessState_Active) || (proc->State == ProcessState_Started))
                    {
                        int procindex = proc->ProcessLeader_ProcessorID;
                        processEntry[procindex].add(attachMsg.toolId, app, attachMsg.priority, attachMsg.toolTag);
                        // set the rank number of this process and bump the counter of the number of processes
                        ackMsg->rank[numprocesses++] = proc->Rank;
                        // Refresh the toolAttached indicator in the process
                        proc->toolAttached = (processEntry[procindex].numTools() ? 1 : 0);
                        ppc_msync();
                    }
                }
                // Iterate to the next process in the job
                proc = proc->nextProcess;
            }
        }
        Kernel_Unlock(&_toolTableLock);
    }
    else
    {
        messageReturnCode = bgcios::JobIdError;
    }
    // Build and send the acknowledgement message
    ackMsg->header.service = bgcios::ToolctlService;
    ackMsg->header.type = AttachAck;
    ackMsg->header.length = sizeof(AttachAckMessage); 
    ackMsg->header.jobId = attachMsg.header.jobId;
    ackMsg->header.sequenceId = attachMsg.header.sequenceId;
    ackMsg->header.rank = attachMsg.header.rank; 
    ackMsg->header.version = ProtocolVersion;
    ackMsg->header.returnCode = messageReturnCode;
    ackMsg->header.errorCode = messageErrorData;
    ackMsg->toolId = attachMsg.toolId;
    ackMsg->numProcess = numprocesses;

    // Send the message to tool daemon
    Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLMSGAK, (((uint64_t)ackMsg->header.sequenceId)<<32) + ackMsg->header.type, (((uint64_t)ackMsg->header.length)<<32) + ackMsg->header.rank, ackMsg->toolId,0);
    postRecv(region); // Prepare to recieve the next message.
    postSend(_outMessageRegion);

    TRACE( TRACE_Toolctl, ("(I) ToolControl::attach: AttachAck message sent to tool %u, return code %u, error code %u\n",
                           ackMsg->toolId, ackMsg->header.returnCode, ackMsg->header.errorCode) );
    return;
}

// Detach a tool
void ToolControl::detach(struct cnv_mr& region)
{
    DetachMessage& msg = *((DetachMessage*)(region.addr));
    TRACE( TRACE_Toolctl, ("(I) ToolControl::detach: Detach message received from tool %u\n", msg.toolId) );

    // Build the acknowledgement message
    DetachAckMessage *ackMsg = (DetachAckMessage *)_outMessageRegion.addr;
    memset(ackMsg, 0, sizeof (DetachAckMessage));

    // Find the AppState object that corresponds to the supplied jobID.
    AppState_t *app = App_GetAppFromJobid(msg.header.jobId);

    if (app)
    {
        messageReturnCode = bgcios::Success;
        Kernel_Lock(&_toolTableLock);
        // Detach this tool from all of the processes that it is registered with.
        AppProcess_t *proc = GetFirstProcess(app);
        while (proc)
        {
            // Is this a targeted process?
            if ((msg.procSelect == RanksInNode) ||
                ((msg.procSelect == RankInHeader) && (msg.header.rank == proc->Rank)))
            {
                int procindex = proc->ProcessLeader_ProcessorID;
                if ((msg.toolId == processEntry[procindex].controllingToolId()) && 
                    (proc->State != ProcessState_ExitPending)) // If the process is exiting, we dont care about the controlling tool
                {
                    messageReturnCode = bgcios::ToolControlReleaseRequired;
                    break;
                }
            }
            // Iterate to the next process in the job
            proc = proc->nextProcess;
        }
        if (messageReturnCode == bgcios::Success)
        {
            AppProcess_t *proc = GetFirstProcess(app);
            while (proc)
            {
                // Is this a targeted process?
                if ((msg.procSelect == RanksInNode) ||
                    ((msg.procSelect == RankInHeader) && (msg.header.rank == proc->Rank)))
                {
                    int procindex = proc->ProcessLeader_ProcessorID;
                    // Note that the tool may not be attached to this particular process. 
                    // Let the following call to the remove function figure this out.
                    if (processEntry[procindex].remove(msg.toolId) == 0)
                    {
                        // Remove was sucessful. Add info to the ack msg
                        ackMsg->rank[ackMsg->numProcess++] = proc->Rank;

                        // Refresh the attached tool indicator
                        proc->toolAttached = (processEntry[procindex].numTools() ? 1 : 0);
                    }
                    else
                    {
                        // Targeted rank did not have this requested tool_id attached. For now
                        // we will not have a special return code to indicate this.
                    }
                }
                // Iterate to the next process in the job
                proc = proc->nextProcess;

            }
        }
        Kernel_Unlock(&_toolTableLock);
    }
    else
    {
        messageReturnCode = bgcios::JobIdError;
    }

    // Build and send the acknowledgement message
    ackMsg->header.service = bgcios::ToolctlService;
    ackMsg->header.type = DetachAck;
    ackMsg->header.length = sizeof(DetachAckMessage); 
    ackMsg->header.jobId = msg.header.jobId;
    ackMsg->header.sequenceId = msg.header.sequenceId;
    ackMsg->header.rank = msg.header.rank; 
    ackMsg->header.version = ProtocolVersion;
    ackMsg->header.returnCode = messageReturnCode;
    ackMsg->header.errorCode = messageErrorData;
    ackMsg->toolId = msg.toolId;

    // Send the message to tool daemon
    Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLMSGAK, (((uint64_t)ackMsg->header.sequenceId)<<32) + ackMsg->header.type, (((uint64_t)ackMsg->header.length)<<32) + ackMsg->header.rank, ackMsg->toolId,0);
    postRecv(region); // Prepare to recieve the next message.
    postSend(_outMessageRegion);

    TRACE( TRACE_Toolctl, ("(I) ToolControl::detach: DetachAck message sent to tool %u, return code %u, error code %u\n",
                           ackMsg->toolId, ackMsg->header.returnCode, ackMsg->header.errorCode) );
    return;
}

void ToolControl::control(struct cnv_mr& region)
{
    ControlMessage& controlMsg = *((ControlMessage*)(region.addr));
    // Initialize the attach ack message area 
    ControlAckMessage *ackMsg = (ControlAckMessage *)_outMessageRegion.addr;
    memset(ackMsg, 0, sizeof (ControlAckMessage));
    // Indicator for a pending conflict notify message.
    int pendingConflictNotifyMessage = 0;


    uint32_t controllingTool = 0;
    uint8_t controllingToolPriority = 0;
    char toolTag[ToolTagSize];
    memset(toolTag,0x00, sizeof(toolTag));

    // Find the AppState object that corresponds to the supplied jobID.
    AppState_t *app = App_GetAppFromJobid(controlMsg.header.jobId);
    AppProcess_t *proc = NULL;

    if (app)
    {
        proc = getProcessFromMessage();
        if (proc)
        {
            if (isToolAttached())
            {
                int procindex = proc->ProcessLeader_ProcessorID;
                // Lock the tool table
                Kernel_Lock(&_toolTableLock);
                // Try to obtain control authority
                controllingTool = processEntry[procindex].grantControlAuthority(controlMsg.toolId, controlMsg.notifySet, controlMsg.sndSignal);

                // Did we obtain control authority?
                if (controllingTool == controlMsg.toolId)
                {
                    // Yes, we now have control authority

                    // Allocate the trap table
                    breakpointController().trapTableAllocate();

                    // Is this tool requesting that the DAC trap mode be set
                    if (controlMsg.dacTrapMode == TrapOnDAC)
                    {
                        breakpointController().setTrapOnDAC();
                    }
                    else if (controlMsg.dacTrapMode == TrapAfterDAC)
                    {
                        breakpointController().setTrapAfterDAC();
                    }
                    // Is the tool requesting that a signal be sent after we attach to the running process?
                    if (controlMsg.sndSignal)
                    {
                        // Has the process been started yet?
                        if (proc->State != ProcessState_Started)
                        {
                            if (controlMsg.dynamicNotifyMode == DynamicNotifyDLoader)
                            {
                                processEntry[procindex].setDynamicAppNotifyDLoader(currentMsg->toolId);
                            }
                            else if (controlMsg.dynamicNotifyMode == DynamicNotifyStart)
                            {
                                processEntry[procindex].setDynamicAppNotifyStart(currentMsg->toolId);
                            }
                            proc->toolAttachSignal = controlMsg.sndSignal;
                            proc->toolIdAttachedAtStart = controlMsg.toolId;
                        }
                        else
                        {
                            // Deliver signal, starting with the process leader
                            TRACE( TRACE_Toolctl, ("(I) ToolControl::control: delivering signal %d to process leader\n", controlMsg.sndSignal) );
                            Signal_Deliver(proc, 0, controlMsg.sndSignal);
                        }
                    }
                    // Is the tool requesting that we propogate watchpoints on clone operations
                    // Note: this field was added at protocol version 5. If we are talking to a tool compiled 
                    // against an older protocol version, then we will default this new field to propogate watchpoints
                    if (controlMsg.header.version <  5)
                    {
                        proc->propogateWatchpoints = 1;
                    }
                    else
                    {
                        proc->propogateWatchpoints = (controlMsg.watchpointPolicy == Watch_Propogate) ? 1 : 0;
                    }
                    messageReturnCode = bgcios::Success;
                }
                else
                {
                    // We did not obtain control authority. 
                    if (controllingTool)
                    {
                        // There is a conflicting tool. 
                        messageReturnCode = bgcios::ToolControlConflict;
                        // Send a message to the tool that currently has control authority, but do it through an IPI to the process leader.
                        pendingNotifyControl.notifyControlReason = NotifyControl_Conflict;
                        pendingNotifyControl.toolID_2 = controlMsg.toolId;
                        pendingNotifyControl.toolPriority = processEntry[procindex].toolPriority(controlMsg.toolId);
                        pendingNotifyControl.toolID_1 = controllingTool;
                        pendingNotifyControl.jobid = controlMsg.header.jobId;
                        pendingNotifyControl.rank = controlMsg.header.rank;
                        pendingNotifyControl.process = proc;
                        pendingNotifyControl.resumeAllThreads = 0;
                        strncpy(pendingNotifyControl.toolTag,processEntry[procindex].toolTag(controlMsg.toolId), ToolTagSize);
                        pendingConflictNotifyMessage = 1; 
                    }
                    else
                    {
                        // No tool has control however we could not find this tool in the list of attached tools for this process.
                        messageReturnCode = bgcios::ToolIdError;
                    }
                }
                // Obtain info from the table needed for the ack message while we are holding the table lock
                controllingToolPriority = processEntry[procindex].toolPriority(controllingTool);
                strncpy(toolTag, processEntry[procindex].toolTag(controllingTool), sizeof(toolTag));

                // Unlock the tool table
                Kernel_Unlock(&_toolTableLock);

            }
            else
            {
                messageReturnCode = bgcios::ToolIdError;
            }

        }
        else
        {
            messageReturnCode = bgcios::ProcessIdError;
        }
    }
    else
    {
        messageReturnCode = bgcios::JobIdError;
    }
    ackMsg->header.service = bgcios::ToolctlService;
    ackMsg->header.type = ControlAck;
    ackMsg->header.length = sizeof(ControlAckMessage); 
    ackMsg->header.jobId = controlMsg.header.jobId;
    ackMsg->header.sequenceId = controlMsg.header.sequenceId;
    ackMsg->header.rank = controlMsg.header.rank; 
    ackMsg->header.version = ProtocolVersion;
    ackMsg->header.returnCode = messageReturnCode;
    ackMsg->header.errorCode = messageErrorData;
    ackMsg->toolId = controlMsg.toolId;
    ackMsg->controllingToolId = controllingTool;
    strncpy(ackMsg->toolTag, toolTag, ToolTagSize);
    ackMsg->priority = controllingToolPriority;

    // Send the message to tool daemon
    Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLMSGAK, (((uint64_t)ackMsg->header.sequenceId)<<32) + ackMsg->header.type, (((uint64_t)ackMsg->header.length)<<32) + ackMsg->header.rank, ackMsg->toolId,0);
    postRecv(region); // Prepare to recieve the next message.
    postSend(_outMessageRegion);

    TRACE( TRACE_Toolctl, ("(I) ToolControl::control: ControlAck message sent to tool %u, return code %u, error code %u\n",
                           ackMsg->toolId, ackMsg->header.returnCode, ackMsg->header.errorCode) );


    // Was there a notify message for a conflict situation that most be sent?
    if (pendingConflictNotifyMessage && proc)
    {
        IPI_tool_notifycontrol(proc->ProcessLeader_ProcessorID, &pendingNotifyControl);
    }
}


uint16_t ToolControl::prepareCommand(CommandDescriptor& cmd_descriptor, CommandDescriptor& cmd_ack_descriptor)
{
    // Set the ACK message type in the ack descriptor
    cmd_ack_descriptor.type = cmd_descriptor.type + 1;

    uint16_t command = cmd_descriptor.type;

    // We need to determine the hardware thread to be targeted. 
    ToolCommand* command_data = (ToolCommand*)((uint64_t)currentMsg + cmd_descriptor.offset); 

    // Find the target process. 
    AppProcess_t *proc = getProcessFromMessage();

    // Was a tid specified 
    if (!command_data->threadID)
    {
        // No TID was specified. We are expected to pick one. Use the TID that began 
        // the tool message exchange via the notify message. 
        command_data->threadID = getDefaultThreadId(proc);
        // If we still do not have a thread id, pick the process leader
        if (!command_data->threadID && proc)
        {
            command_data->threadID = GetTID(proc->ProcessLeader_KThread);
        }
    }
    KThread_t *kthread = ((command_data->threadID) ? GetKThreadFromTid(command_data->threadID) : NULL); 
    if (kthread &&                   // Did we find a kthread that corresponds to the provided TID
        proc &&                      // Did we find a process object associated with the message
        (kthread->pAppProc == proc)) // Is the kthread within the targetted process
    {
        // See if the IPI request data area is available. If it is busy from a previous request to the same
        // hardware thread, we need to wait for it to be available.
        int processorID = kthread->ProcessorID;
        while (ipiRequest[processorID].state == TOOL_IPI_STATE_BUSY)
        {
            // spin waiting for a previous IPI to this hardware thread to complete
        }
        // Set up the structure in preparation to issue the IPI
        int descriptor_index = ipiRequest[processorID].numCommands++;
        ipiRequest[processorID].state = TOOL_IPI_STATE_PENDING;
        ipiRequest[processorID].descriptor[descriptor_index].cmd = (void*)&cmd_descriptor;
        ipiRequest[processorID].descriptor[descriptor_index].ack = (void*)&cmd_ack_descriptor;
        TRACE( TRACE_Toolctl, ("(I) ToolControl::prepareCommand: added thread ID %u running on processor ID %d to IPI request index %d for command %u\n",
                                   command_data->threadID, processorID, descriptor_index, command) );
    }
    else
    {
        // error. Bad TID was specified.
        TRACE( TRACE_Toolctl, ("(E) ToolControl::sendCommand: thread ID %u is not valid for command %u\n", command_data->threadID, command) );
        invalidTID(*((ToolCommand*)command_data), cmd_ack_descriptor);
    }
    return 0;
}

//
void ToolControl::query(struct cnv_mr& region)
{
    QueryMessage& msg = *((QueryMessage *)(region.addr));
    TRACE(TRACE_Toolctl, ("(I) ToolControl::query: Query message received from tool %u\n", msg.toolId));

    // Initialize the acknowledgement message
    QueryAckMessage *ackMsg = (QueryAckMessage *)_outMessageRegion.addr;
    memset(ackMsg, 0, sizeof(QueryAckMessage));
    int numCmdAcks = 0;

    // initialize the outbound pointer for use by the target of the IPI operations
    outbound_cmd_data = (uint64_t)_outMessageRegion.addr + sizeof(QueryMessage);

    // Find the AppState object that corresponds to the supplied jobID.
    AppState_t *app = App_GetAppFromJobid(msg.header.jobId);

    if (app)
    {
        AppProcess_t *proc = getProcessFromMessage();
        if (proc)
        {
            if (proc->State != ProcessState_ExitPending)
            {
                if (isToolAttached())
                {
                    if (msg.numCommands <= MaxQueryCommands)
                    {
                        // set the number of expected ack cmds
                        numCmdAcks = msg.numCommands;
                        // Iterate through the Command List, sending IPIs to all the hardware threads involved in the requests
                        for (int i = 0; i < numCmdAcks; i++)
                        {
                            // prepare the commmand for the appropriate hardware thread
                            prepareCommand(msg.cmdList[i], ackMsg->cmdList[i]);
                        }
                        // Send the pending IPI interrupts to all targeted hardware threads
                        for (int i = 0; i < CONFIG_MAX_APP_THREADS; i++)
                        {
                            if (ipiRequest[i].state == TOOL_IPI_STATE_PENDING)
                            {
                                ipiRequest[i].state = TOOL_IPI_STATE_BUSY;
                                // Bump count of IPIs issued
                                incrementPendingIPIs();
                                // Send IPI request to the target hardware thread
                                IPI_tool_cmd(i, &(ipiRequest[i]));
                            }
                        }
                        TRACE(TRACE_Toolctl, ("(I) ToolControl::query: sent IPIs to all targeted threads, waiting for completion ...\n"));        // Spin until all IPI messages complete
                        pollAllMessageIPIsComplete();
                    }
                    else
                    {
                        messageReturnCode = bgcios::ToolNumberOfCmdsExceeded;
                    }
                }
                else
                {
                    messageReturnCode = bgcios::ToolIdError;
                }
            }
            else
            {
                messageReturnCode = bgcios::ToolProcessExiting;
            }
        }
        else
        {
            messageReturnCode = bgcios::ToolRankNotFound;
        }
    }
    else
    {
        messageReturnCode = bgcios::JobIdError;
    }
    // Finish building the ACK message and send it
    ackMsg->header.service = bgcios::ToolctlService;
    ackMsg->header.type = QueryAck;
    uint32_t calculated_length = (uint32_t)((uint64_t)outbound_cmd_data - (uint64_t)_outMessageRegion.addr);
    ackMsg->header.length =  (calculated_length < sizeof(_outMessage)) ? calculated_length : sizeof(_outMessage);
    ackMsg->header.jobId = msg.header.jobId;
    ackMsg->header.sequenceId = msg.header.sequenceId;
    ackMsg->header.rank = msg.header.rank;
    ackMsg->header.version = ProtocolVersion;
    ackMsg->header.returnCode = messageReturnCode;
    ackMsg->header.errorCode = messageErrorData;
    ackMsg->toolId = msg.toolId;
    ackMsg->numCommands = numCmdAcks;

    // Send the message to tool daemon
    Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLMSGAK, (((uint64_t)ackMsg->header.sequenceId) << 32) + ackMsg->header.type, (((uint64_t)ackMsg->header.length) << 32) + ackMsg->header.rank, ackMsg->toolId, 0);
    postRecv(region); // Prepare to recieve the next message.
    postSend(_outMessageRegion);

    TRACE(TRACE_Toolctl, ("(I) ToolControl::query: QueryAck message sent to tool %u, return code %u, error code %u\n",
                          ackMsg->toolId, ackMsg->header.returnCode, ackMsg->header.errorCode));
    return;
}
void ToolControl::update(struct cnv_mr& region)
{
    UpdateMessage& msg = *((UpdateMessage*)(region.addr));
    TRACE( TRACE_Toolctl, ("(I) ToolControl::update: Update message received from tool %u\n", msg.toolId) );

    // Initialize the acknowledgement message
    UpdateAckMessage *ackMsg = (UpdateAckMessage *)_outMessageRegion.addr;
    memset(ackMsg, 0, sizeof (UpdateAckMessage));
    int numCmdAcks = 0;

    // initialize the outbound pointer for use by the target of the IPI operations
    outbound_cmd_data = (uint64_t)_outMessageRegion.addr + sizeof(UpdateMessage);

    // Find the AppState object that corresponds to the supplied jobID.
    AppState_t *app = App_GetAppFromJobid(msg.header.jobId);
    AppProcess_t *proc = NULL;

    if (app)
    {
        // Are we allowed to process this update message
        proc = getProcessFromMessage();
        int procindex = 0; 
        if (proc)
        {
            if (proc->State != ProcessState_ExitPending)
            {
                // Is the tool ID specified in the message attached to the compute node.
                if (isToolAttached())
                {

                    procindex =  proc->ProcessLeader_ProcessorID;
                    if (processEntry[procindex].controllingToolId() == msg.toolId)
                    {
                        if (msg.numCommands <= MaxUpdateCommands)
                        {
                            // Validate the combination of commands within this message.
                            //  -only one action command per update message is allowed
                            int actionCmds = 0;
                            for (int i=0; i<msg.numCommands; i++)
                            {
                                if ((msg.cmdList[i].type == StepThread) ||
                                    (msg.cmdList[i].type == ContinueProcess) ||
                                    (msg.cmdList[i].type == ReleaseControl))
                                {
                                    ++actionCmds;
                                }
                            }
                            if (actionCmds <= 1)
                            {
                                // set the number of expected ack cmds
                                numCmdAcks = msg.numCommands;  
                                // Iterate through the Command List, sending IPIs to all the hardware threads involved in the requests
                                for (int i=0; i<numCmdAcks; i++ )
                                {
                                    // prepare the commmand to the appropriate hardware thread for processing
                                    prepareCommand(msg.cmdList[i], ackMsg->cmdList[i]);
                                }
                                // Send the pending IPI interrupts to all targeted hardware threads
                                for (int i=0; i<CONFIG_MAX_APP_THREADS; i++)
                                {
                                    if (ipiRequest[i].state == TOOL_IPI_STATE_PENDING)
                                    {
                                        ipiRequest[i].state = TOOL_IPI_STATE_BUSY;
                                        // Bump count of IPIs issued
                                        incrementPendingIPIs();
                                        // Send IPI request to the target hardware thread
                                        IPI_tool_cmd(i, &(ipiRequest[i]));
                                    }
                                }
                                // Spin until all IPI messages complete
                                pollAllMessageIPIsComplete();
                            }
                            else
                            {
                                messageReturnCode = bgcios::ToolConflictingCmds;
                            }
                        }
                        else
                        {
                            messageReturnCode = bgcios::ToolNumberOfCmdsExceeded;
                        }
                    }
                    else
                    {
                        messageReturnCode = bgcios::ToolControlRequired;
                    }
                }
                else
                {
                    messageReturnCode = bgcios::ToolIdError;
                }
            }
            else
            {
                messageReturnCode = bgcios::ToolProcessExiting;
            }
        }
        else
        {
            messageReturnCode = bgcios::ToolRankNotFound;
        }
    }
    else
    {
        messageReturnCode = bgcios::JobIdError;
    }
    // Finish building the ACK message and send it
    ackMsg->header.service = bgcios::ToolctlService;
    ackMsg->header.type = UpdateAck;
    uint32_t calculated_length = (uint32_t)((uint64_t)outbound_cmd_data - (uint64_t)_outMessageRegion.addr);
    ackMsg->header.length =  (calculated_length < sizeof(_outMessage)) ? calculated_length : sizeof(_outMessage); 
    ackMsg->header.jobId = msg.header.jobId;
    ackMsg->header.sequenceId = msg.header.sequenceId;
    ackMsg->header.rank = msg.header.rank; 
    ackMsg->header.version = ProtocolVersion;
    ackMsg->header.returnCode = messageReturnCode;
    ackMsg->header.errorCode = messageErrorData;
    ackMsg->toolId = msg.toolId;
    ackMsg->numCommands = numCmdAcks;

    // Send the message ack to tool daemon
    Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLMSGAK, (((uint64_t)ackMsg->header.sequenceId)<<32) + ackMsg->header.type, (((uint64_t)ackMsg->header.length)<<32) + ackMsg->header.rank, ackMsg->toolId,0);
    postRecv(region); // Prepare to recieve the next message.
    postSend(_outMessageRegion);

    // Process any pending operations 
    // It is important that the destination thread for any IPIs issued in the following pending action conditions
    // must be gauranteed to be in an interruptable state and not attempting to send a jobctl or toolcltl message. 
    // If any destination thread is attempting to send a  job control message or a tool control message, and this 
    // tool controller thread is trying to issue an IPI to that thread, it will hang waiting for an ACK and this 
    // thread will hang waiting for the IPI to be sent.
    // !!! do we need to send these to one of the appagent threads????
    if (proc)
    {
        switch (proc->pendingAction)
        {
        case ToolPendingAction_NotifySignal:
            // must be done on a thread other than this thread since this thread needs to poll the completion queue for the notify ack
            IPI_tool_notifysignal(proc->actionPendingKthread);
            break;
        case ToolPendingAction_NotifyAvail:
            // must be done on a thread other than this thread since this thread needs to poll the completion queue for the notify ack
            if (pendingNotifyControl.resumeAllThreads)
            {
                resumeProcess(proc);
            }
            IPI_tool_notifycontrol(proc->ProcessLeader_ProcessorID, &pendingNotifyControl);
            break;
        case ToolPendingAction_ResumeProc:
            resumeProcess(proc);
            break;
        case ToolPendingAction_ResumeThd:
            resumePendingSteps(proc);
            break;
        default: {}
        }
        proc->pendingAction = ToolPendingAction_Reset;
    }
}

// Allocate storage in the outbound buffer
void* ToolControl::allocateOutbound(int size)
{
    uint64_t previousValue = Fetch_and_Add((uint64_t*)&outbound_cmd_data, size);

    if ((previousValue+size) > ((uint64_t)_outMessage + sizeof(_outMessage)))
    {
        return NULL;
    }
    // initialize the buffer
    memset((void*)previousValue, 0, size);
    // Return the old value which represents the space allocated by this request
    return(void*)previousValue;
}

// Get the tool id to be notified for a particular signal from a specific TID
int ToolControl::findToolToNotify(KThread_t *kthread, int signal)
{
    // Determine if there is a tool that should be notified
   AppProcess_t *proc = kthread->pAppProc;
   int procindex = proc->ProcessLeader_ProcessorID;
   // Is there a controlling tool attached to this node
   int toolid = processEntry[procindex].controllingToolId();

   // Is this tool interested in the specified signal?
   if (toolid && (SIG_MASK(signal) & processEntry[procindex].signalMask(toolid)))
   {
       return toolid;
   }
   return 0;
}

// Notify a tool about a control availability
void ToolControl::notifyControl(uint32_t target_toolid, NotifyControlReason reason, uint32_t toolid, char* toolTag, uint8_t priority, uint64_t jobid, uint32_t rank )
{
    // Obtain lock to serialize Notify messages (lock is released when NotifyAck message is received).
    while(Kernel_Lock_WithTimeout(&_notifyLock, 10000) != 0) // attempt to lock. Timeout after 10ms
    {
        // The attempt to grab the kernel lock timed out. The Tool controller thread may be hung, preventing another notify to complete. 
        // Execute any tool IPI commands that the tool controller thread may have pending against us.
        IntHandler_IPI_FlushToolCommands();
    }
    // Build and send the Notify message
    NotifyMessage *msg = (NotifyMessage *)_outNotifyMessageRegion.addr;
    memset(msg, 0, sizeof (NotifyMessage));
    msg->notifyMessageType = NotifyMessageType_Control;
    msg->header.service = bgcios::ToolctlService;
    msg->header.type = Notify;
    msg->header.version = ProtocolVersion;
    msg->header.length = sizeof(NotifyMessage); 
    msg->header.jobId = jobid;
    msg->header.sequenceId = _sequenceId++;
    msg->header.rank = rank;
    msg->toolId = target_toolid; 
    msg->type.control.reason = reason;
    strncpy(msg->type.control.toolTag, toolTag, ToolTagSize);
    msg->type.control.priority = (uint8_t)priority;
    msg->type.control.toolid = toolid;
    Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLNTIFY, (((uint64_t)msg->header.sequenceId) << 32) + NotifyMessageType_Control, (((uint64_t)msg->header.rank) << 32) + reason, target_toolid, toolid);
    // Send the message to tool daemon
    postSend(_outNotifyMessageRegion, true);

    TRACE( TRACE_Toolctl, ("(I) ToolControl::notifyControlAvailable: Notify message sent to tool %u was encountered\n", toolid) );
}

// Notify a  tool that a signal has occurred
int ToolControl::notify(int toolId, KThread_t *kthread, int signal)
{
    int rc = 0;
    AppProcess_t *proc = kthread->pAppProc;
    AppState_t *app = proc->app;
    int rank = proc->Rank;
    int jobId = app->JobID;

    proc->notifiedKThread = kthread; // Save the kthread being notified.
    proc->continuationSignal = 0;    // Initialize the continuation signal to zero.
    setDefaultThreadId(toolId, proc, GetTID(kthread)); // Set the default thread id for messages.

    // Obtain lock to serialize Notify messages (lock is released when NotifyAck message is received).
    while(Kernel_Lock_WithTimeout(&_notifyLock, 10000) != 0) // attempt to lock. Timeout after 10ms
    {
        // The attempt to grab the kernel lock timed out. The Tool controller thread may be hung, preventing another notify to complete. 
        // Execute any tool IPI commands that the tool controller thread may have pending against us.
        IntHandler_IPI_FlushToolCommands();
    }
    // Build and send the Notify message
    NotifyMessage *msg = (NotifyMessage *)_outNotifyMessageRegion.addr;
    memset(msg, 0, sizeof (NotifyMessage));
    msg->notifyMessageType = NotifyMessageType_Signal;

    msg->header.service = bgcios::ToolctlService;
    msg->header.type = Notify;
    msg->header.version = ProtocolVersion;
    msg->header.length = sizeof(NotifyMessage); 
    msg->header.jobId = jobId;
    msg->header.sequenceId = _sequenceId++;
    msg->header.rank = rank;
    msg->toolId = toolId; 
    msg->type.signal.signum = signal;
    msg->type.signal.threadID = GetTID(kthread);
    msg->type.signal.instAddress = kthread->Reg_State.ip;

    HWThreadState_t *hwt = kthread->pHWThread;
    if (hwt->pendingSpecRestartContext)
    {
        msg->type.signal.executeMode = NotifySignal_Speculative;
        // Save the restart values in the notify message.
        msg->type.signal.spec.instAddress = proc->SpecState->hwt_state[kthread->ProcessorID].ip;
        msg->type.signal.spec.gpr1 = proc->SpecState->hwt_state[kthread->ProcessorID].gpr1;
        msg->type.signal.spec.gpr2 = proc->SpecState->hwt_state[kthread->ProcessorID].gpr2;
        msg->type.signal.spec.gpr3 = Kernel_SpecReturnCode_JMV;
        //printf("Signal in Speculation. TID=%d Restart values: ip=%016lx gpr1=%016lx gpr3=%016lx\n",msg->type.signal.threadID, msg->type.signal.spec.instAddress,msg->type.signal.spec.gpr1,msg->type.signal.spec.gpr3);
    }
    if (signal == SIGTRAP)
    {
        if (kthread->Reg_State.dbsr & (DBSR_DAC1R | DBSR_DAC2R | DBSR_DAC3R | DBSR_DAC4R) )
        {
            msg->type.signal.reason = NotifySignal_WatchpointRead;
        }
        else if (kthread->Reg_State.dbsr & (DBSR_DAC1W | DBSR_DAC2W | DBSR_DAC3W | DBSR_DAC4W))
        {
            msg->type.signal.reason = NotifySignal_WatchpointWrite;
        }
        else if (kthread->Reg_State.dbsr & DBSR_ICMP)
        {
            msg->type.signal.reason = NotifySignal_StepComplete;
            //printf("Notify Step Complete\n");
        }
        else 
        {
            msg->type.signal.reason = NotifySignal_Breakpoint;
        }
        // Is the dac1 status indicator set for reads or writes.
        if (kthread->Reg_State.dbsr & (DBSR_DAC1R | DBSR_DAC1W))
        {
            // dac1 contains our matching address. Store it.
            msg->type.signal.dataAddress = hwt->CriticalState.dac1;
        }
        // is the dac2 status indicator set for reads or writes.
        else if (kthread->Reg_State.dbsr & (DBSR_DAC2R | DBSR_DAC2W))
        {
            // Are we in exact address mode with DAC1 and DAC2?
            if (!(kthread->Reg_State.dbcr2 & DBCR2_DAC12M))
            {
                // dac2 contains our matching address.
                msg->type.signal.dataAddress = hwt->CriticalState.dac2;
            }
        }
        // Is the dac3 status indicator set for reads or writes.
        if (kthread->Reg_State.dbsr & (DBSR_DAC3R | DBSR_DAC3W))
        {
            // If both the DAC1 and DAC3 fired, store the match with the lower address
            // This can occur if one write operation touches both DAC ranges.
            if (!((kthread->Reg_State.dbsr & (DBSR_DAC1R | DBSR_DAC1W)) && (hwt->CriticalState.dac1 < hwt->CriticalState.dac3)))
            {
                msg->type.signal.dataAddress = hwt->CriticalState.dac3;
            }
        }
        else if (kthread->Reg_State.dbsr & (DBSR_DAC4R | DBSR_DAC4W))
        {
            // Are we in exact address mode with DAC3 and DAC4?
            if (!(kthread->Reg_State.dbcr3 & DBCR3_DAC34M))
            {
                msg->type.signal.dataAddress = hwt->CriticalState.dac4;
            }
        }
    }
    else
    {
        msg->type.signal.reason = NotifySignal_Generic;
    }
    Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLNTIFY, (((uint64_t)msg->header.sequenceId) << 32) + NotifyMessageType_Signal, (((uint64_t)msg->header.rank) << 32) + signal, (((uint64_t)msg->type.signal.executeMode) << 32) + GetTID(kthread), toolId);

    // Send the message to tool daemon
    postSend(_outNotifyMessageRegion, true);

    TRACE( TRACE_Toolctl, ("(I) ToolControl::notify: Notify message sent to tool %u, signal %d was encountered\n", toolId, signal) );
    return rc; 
}

void ToolControl::notifyAck(struct cnv_mr& region)
{
    // Make sure the Notify message was successful.
    NotifyAckMessage& ackMsg = *((NotifyAckMessage*)(region.addr));
    if (ackMsg.header.returnCode != 0)
    {
        //printf("(E) Tool control Notify message failed, return code %u, error code %u\n", ackMsg.header.returnCode, ackMsg.header.errorCode);
        // Ignore. The tool may have vanished.
    }
    postRecv(region); // Prepare to recieve the next message.

    // Tell waiter ack message has been received. 
    fetch_and_add(&_ackReceived, 1);

    // Release lock now that message exchange is complete.
    Kernel_Unlock(&_notifyLock);

    return;
}

// Notify a  tool that a process is ending
void ToolControl::exitProcess(AppProcess_t *proc)
{
    int procindex = proc->ProcessLeader_ProcessorID;
    // Loop through all the tools attached to this process
    for(int i=0; i<MAX_NUM_TOOLS; i++)
    {
        int toolid = processEntry[procindex].toolId(i);
        if (toolid != 0)
        {
            // Obtain lock to serialize Notify messages (lock is released when NotifyAck message is received).
            while(Kernel_Lock_WithTimeout(&_notifyLock, 10000) != 0) // attempt to lock. Timeout after 10ms
            {
                // The attempt to grab the kernel lock timed out. The Tool controller thread may be hung, preventing another notify to complete. 
                // Execute any tool IPI commands that the tool controller thread may have pending against us.
                IntHandler_IPI_FlushToolCommands();
            }
            // Build and send the Notify message
            NotifyMessage *msg = (NotifyMessage *)_outNotifyMessageRegion.addr;
            memset(msg, 0, sizeof (NotifyMessage));
            msg->header.service = bgcios::ToolctlService;
            msg->header.type = Notify;
            msg->header.length = sizeof(NotifyMessage); 
            msg->header.jobId = proc->app->JobID;
            msg->header.sequenceId = _sequenceId++;
            msg->header.rank = proc->Rank;
            msg->toolId = toolid; 
            msg->notifyMessageType = NotifyMessageType_Termination;
            msg->type.termination.exitStatus = proc->ExitStatus;
    
            Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLNTIFY, (((uint64_t)msg->header.sequenceId) << 32) + NotifyMessageType_Termination, proc->ExitStatus, proc->Rank, toolid);
            // Send the message to tool daemon
            postSend(_outNotifyMessageRegion, true);
            TRACE( TRACE_Toolctl, ("(I) ToolControl::exitProcess: exitProcess message sent to tool %u\n", toolid) );
        }
    }
    return;
}

// Detach all tools associated with the job
void ToolControl::detachAll(AppState_t *app)
{
    Kernel_Lock(&_toolTableLock);
    // Detach all tools from all of the processes that it is registered with.
    AppProcess_t *proc = GetFirstProcess(app);
    while (proc)
    {
        int procindex = proc->ProcessLeader_ProcessorID;
        processEntry[procindex].removeAll();
        // Initialize the pending action fields. Should already be zero, but cleanup anyway.
        proc->pendingAction = ToolPendingAction_Reset;
        // Iterate to the next process in the job
        proc = proc->nextProcess;
    }
    Kernel_Unlock(&_toolTableLock);
    // Free the lock on the shared kernel data area
    breakpointController().trapTableFree(); 
    // Initialize the trap work area pointers and the trap table to reset state
    breakpointController().init();
    // Initialize the watchpoint manager
    watchpointManager().init();

}

// Puts all of the TIDs of a process into the Suspend state  
int ToolControl::suspendProcess(AppProcess_t* proc)
{
    Kernel_Lock(&proc->suspendResumeLock);

    // Suspend the threads on our hardware thread
    suspend(proc);

    // Loop through all of the hardware threads associated with this process and send suspend IPI's to them.
    int myProcessorID = ProcessorID();
    uint64_t hwtmask =  proc->HwthreadMask;
    if (proc->ThreadModel == CONFIG_THREAD_MODEL_ETA) 
    {
        // Include remote hardware threads that are hosting threads from this process
        hwtmask |= proc->HwthreadMask_Remote;
    }
    hwtmask &= ~(_BN(myProcessorID)); // remove ours.
    for (int i=cntlz64(hwtmask); hwtmask; i++)
    {
        uint64_t curmask = _BN(i);
        if (hwtmask & curmask)
        {
            // Send suspend IPI
            IPI_tool_suspend(i, proc);
            // remove bit from mask
            hwtmask &= ~curmask;
        }
    }
    Kernel_Unlock(&proc->suspendResumeLock);
    return 0;
}

// Removes the SUSPEND state from all TIDs within an process 
int ToolControl::resumePendingSteps(AppProcess_t* proc)
{
    Kernel_Lock(&proc->suspendResumeLock);

    // Loop through all of the hardware threads associated with this process and send resume IPIs to them
    int myProcessorID = ProcessorID();
    uint64_t hwtmask =  proc->HwthreadMask;
    if (proc->ThreadModel == CONFIG_THREAD_MODEL_ETA) 
    {
        // Include remote hardware threads that are hosting threads from this process
        hwtmask |= proc->HwthreadMask_Remote;
    }
    hwtmask &= ~(_BN(myProcessorID)); // remove ours.
    for (int i=cntlz64(hwtmask); hwtmask; i++)
    {
        uint64_t curmask = _BN(i);
        if (hwtmask & curmask)
        {
            // Ask the breakpoint controller if there is a pending step operation on this hardware thread
            KThread_t *pendingStepKthread = breakpointController().getPendingStepKThread(i, proc);
            if (pendingStepKthread)
            {
                // Yes, unblock the suspend state.
                Sched_Unblock(pendingStepKthread, SCHED_STATE_SUSPEND);
            }
            // remove bit from mask
            hwtmask &= ~curmask;
        }
    }
    Kernel_Unlock(&proc->suspendResumeLock);
    return 0;
}

// Removes the SUSPEND state from all TIDs within an process 
int ToolControl::resumeProcess(AppProcess_t* proc)
{
    Kernel_Lock(&proc->suspendResumeLock);

    // This is expected to be called from the tool controller thread therefore we do not need to worry about the threads on our core.

    // Loop through all of the hardware threads associated with this process and send resume IPIs to them
    int myProcessorID = ProcessorID();
    uint64_t hwtmask =  proc->HwthreadMask;
    if (proc->ThreadModel == CONFIG_THREAD_MODEL_ETA) 
    {
        // Include remote hardware threads that are hosting threads from this process
        hwtmask |= proc->HwthreadMask_Remote;
    }
    hwtmask &= ~(_BN(myProcessorID)); // remove ours.
    for (int i=cntlz64(hwtmask); hwtmask; i++)
    {
        uint64_t curmask = _BN(i);
        if (hwtmask & curmask)
        {
            // Send resume IPI
            IPI_tool_resume(i, proc);
            // remove bit from mask
            hwtmask &= ~curmask;
        }
    }
    Kernel_Unlock(&proc->suspendResumeLock);
    return 0;
}

// Removes the SUSPEND state from all TIDs within an process 
int ToolControl::releaseProcess(AppProcess_t* proc)
{
    // Resume the threads on our hardware thread.
    release(proc);

    // Loop through all of the hardware threads associated with this process and send resume IPIs to them
    int myProcessorID = ProcessorID();
    uint64_t hwtmask =  proc->HwthreadMask;
    if (proc->ThreadModel == CONFIG_THREAD_MODEL_ETA) 
    {
        // Include remote hardware threads that are hosting threads from this process
        hwtmask |= proc->HwthreadMask_Remote;
    }
    hwtmask &= ~(_BN(myProcessorID)); // remove ours.
    for (int i=cntlz64(hwtmask); hwtmask; i++)
    {
        uint64_t curmask = _BN(i);
        if (hwtmask & curmask)
        {
            // Send resume IPI
            IPI_tool_release(i, proc);
            // remove bit from mask
            hwtmask &= ~curmask;
        }
    }
    return 0;
}

// Puts all of the TIDs on the current hwthread for the indicated process into the Suspend state  
int ToolControl::suspend(AppProcess_t* proc)
{

    // Suspend the threads on our hardware thread
    HWThreadState_t *hwt = GetMyHWThreadState();

    // Walk through the kthreads on this hardware thread and set suspend state.
    for (int i = 0; i< CONFIG_AFFIN_SLOTS_PER_HWTHREAD; i++)
    {
        KThread_t *kthread = hwt->SchedSlot[i];
        if (kthread->pAppProc == proc)
        {
            if (!(kthread->State & SCHED_STATE_FREE))
            {
		// kthread is not free, so we can manipulate its state locally
                Sched_Block(kthread, SCHED_STATE_SUSPEND);
            }
            else
            {
		// kthread is currently free.  We have to cooperate with remote
		// threads trying to allocate it for clone or migration.
		uint32_t curState;
                do
                {
                    curState = LoadReserved32(&kthread->State);
                    if (!(curState & SCHED_STATE_FREE))
                    {
                        // kthread is no longer free, so revert to local manipulation
                        Sched_Block(kthread, SCHED_STATE_SUSPEND);
                        break;
                    }
                } while (!StoreConditional32(&kthread->State,
                                             curState | SCHED_STATE_SUSPEND));

            }
        }
    }
    hwt->suspended = 1; // Set fast-path flag for scheduler so that it knows all software threads are suspended on this hardware thread.
    // If the interrupt arrived while we were already in the scheduler, we don't have to do anything.
    // Scheduler() will be called instead of resuming the interrupted code.  If we interrupted an application
    // thread, set a Pending flag to cause us to save its full state and call Scheduler() just before we
    // resume it.
    KThread_t *cur_kthr = hwt->pCurrentThread;
    if (cur_kthr->SlotIndex != CONFIG_SCHED_KERNEL_SLOT_INDEX) {
	cur_kthr->Pending |= KTHR_PENDING_YIELD;
    }
    return 0;
}

// Removes the SUSPEND state from all TIDs on the current hardware thread for the indicated process 
int ToolControl::resume(AppProcess_t* proc)
{
    // Suspend the threads on our hardware thread
    HWThreadState_t *hwt = GetMyHWThreadState();

    // Resume the threads on our hardware thread.
    for (int i = 0; i< CONFIG_AFFIN_SLOTS_PER_HWTHREAD; i++)
    {
        KThread_t *kthread = hwt->SchedSlot[i];
        if (kthread->pAppProc == proc)
        {
            if (!(kthread->State & SCHED_STATE_FREE))
            {
		// kthread is not free, so we can manipulate its state locally
                Sched_Unblock(kthread, SCHED_STATE_SUSPEND);
            }
            else
            {
		// kthread is currently free.  We have to cooperate with remote
		// threads trying to allocate it for clone or migration.
		uint32_t curState;
                do
                {
                    curState = LoadReserved32(&kthread->State);
                    if (!(curState & SCHED_STATE_FREE))
                    {
                        // kthread is no longer free, so revert to local manipulation
                        Sched_Unblock(kthread, SCHED_STATE_SUSPEND);
                        break;
                    }
                } while (!StoreConditional32(&kthread->State,
                                             curState & ~SCHED_STATE_SUSPEND));

            }
        }
    }
    return 0;
}

// Removes the SUSPEND state from all TIDs on the current hardware thread for the indicated process 
int ToolControl::release(AppProcess_t* proc)
{
    // Suspend the threads on our hardware thread
    HWThreadState_t *hwt = GetMyHWThreadState();

    // Resume the threads on our hardware thread.
    for (int i = 0; i< CONFIG_AFFIN_SLOTS_PER_HWTHREAD; i++)
    {
        KThread_t *kthread = hwt->SchedSlot[i];
        if (kthread->pAppProc == proc)
        {
            Sched_Unblock(kthread, SCHED_STATE_HOLD);
        }
    }
    return 0;
}


int ToolControl::processCommands(ToolIpiRequest& toolIpiRequest)
{

    int failedCommands = 0; // Counter for the number of commands that failed within this request.

    for (int i=0; i<toolIpiRequest.numCommands; i++ )
    {
        CommandDescriptor* cmd_desc = (CommandDescriptor*)toolIpiRequest.descriptor[i].cmd;
        uint16_t command = cmd_desc->type; 
        void *cmd_data = (void*)((uint64_t)currentMsg + cmd_desc->offset);
        uint64_t origpid = 0;
        int restorePID = 0;
        Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLCMD__, command, ((ToolCommand*)cmd_data)->threadID, currentMsg->header.rank, currentMsg->toolId);
        // Test to see if the process is still active
        // Get the kthread associated with the specified TID
        KThread_t* kthread = GetKThreadFromTid(((ToolCommand*)cmd_data)->threadID);
        if (!kthread)
        {
            invalidTID(*((ToolCommand*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
        }
        else if (!IsProcessActive(kthread))
        {
            processExitingCmdFailure(*((ToolCommand*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
        }
        else
        {
            // If the targeted kthread has a different hwpid than the current hardware thread, we may need to modify the hwpid while we run this command.
            // Either we interrupted a remote thread with a local request or we interrupted a local thread with a remote thread request.
            if (kthread->pAppProc->ThreadModel == CONFIG_THREAD_MODEL_ETA)
            {
                origpid = mfspr(SPRN_PID);
                if ((origpid != kthread->physical_pid) &&  // Does the current hwpid match the pid in the targetted kthread? 
                    (origpid != GetMyHWThreadState()->PhysicalSpecPID)) // Does the current thread match the speculative hwpid for this hwt?
                {
                    // We need to force the hwpid to the value for the targetted kthread.
                    mtspr(SPRN_PID, kthread->physical_pid); 
                    isync();
                    restorePID = 1;
                }
            }
            switch(currentMsg->header.type)
            {
            case Update:
                switch (command)
                {
                case SetGeneralReg:     
                    setGeneralReg(*((SetGeneralRegCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case SetFloatReg:       
                    setFloatReg(*((SetFloatRegCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case SetDebugReg:      
                    setDebugReg(*((SetDebugRegCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case SetGeneralRegs:     
                    setGeneralRegs(*((SetGeneralRegsCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case SetFloatRegs:       
                    setFloatRegs(*((SetFloatRegsCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case SetDebugRegs:      
                    setDebugRegs(*((SetDebugRegsCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case SetSpecialReg:      
                    setSpecialReg(*((SetSpecialRegCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case SetSpecialRegs:      
                    setSpecialRegs(*((SetSpecialRegsCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case SetMemory:         
                    setMemory(*((SetMemoryCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case HoldThread:        
                    holdThread(*((HoldThreadCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case ReleaseThread:     
                    releaseThread(*((ReleaseThreadCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case InstallTrapHandler:   
                    installTrapHandler(*((InstallTrapHandlerCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case AllocateMemory:         
                    allocateMemory(*((AllocateMemoryCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case FreeMemory:         
                    freeMemory(*((FreeMemoryCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case ContinueProcess: 
                    if (!failedCommands)
                    {
                        continueProcess(*((ContinueProcessCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    }
                    else
                    {
                        previousCmdFailure(*((ToolCommand*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    }
                    break;
                case StepThread: 
                    if (!failedCommands)
                    {
                        stepThread(*((StepThreadCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    }
                    else
                    {
                        previousCmdFailure(*((ToolCommand*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    }
                    break;
                case SetBreakpoint:     
                    setBreakpoint(*((SetBreakpointCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case ResetBreakpoint:     
                    resetBreakpoint(*((ResetBreakpointCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case SetWatchpoint:     
                    setWatchpoint(*((SetWatchpointCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case ResetWatchpoint:     
                    resetWatchpoint(*((ResetWatchpointCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case RemoveTrapHandler:
                    removeTrapHandler(*((RemoveTrapHandlerCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case SetPreferences:
                    setPreferences(*((SetPreferencesCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case SendSignal:
                    sendSignal(*((SendSignalCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack)); 
                    break;
                case SendSignalToProcess:
                    sendSignalToProcess(*((SendSignalToProcessCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack)); 
                    break;
                case ReleaseControl:
                    if (!failedCommands)
                    {
                        releaseControl(*((ReleaseControlCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack)); 
                    }
                    else
                    {
                        previousCmdFailure(*((ToolCommand*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    }
                    break;
                case SetContinuationSignal:
                    setContinuationSignal(*((SetContinuationSignalCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack)); 
                    break;

                default:
                    invalidCmd(*((ToolCommand*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                }
                break;
            case Query:
                switch (command)
                {
                case GetMemory:         
                    getMemory(*((GetMemoryCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case GetThreadList:     
                    getThreadList(*((GetThreadListCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case GetSpecialRegs: 
                    getSpecialRegs(*((GetSpecialRegsCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case GetGeneralRegs:    
                    getGeneralRegs(*((GetGeneralRegsCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case GetFloatRegs:      
                    getFloatRegs(*((GetFloatRegsCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case GetDebugRegs:      
                    getDebugRegs(*((GetDebugRegsCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case GetAuxVectors:     
                    getAuxVectors(*((GetAuxVectorsCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case GetProcessData:    
                    getProcessData(*((GetProcessDataCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case GetThreadData:     
                    getThreadData(*((GetThreadDataCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case GetPreferences:     
                    getPreferences(*((GetPreferencesCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case GetFilenames:
                    getFilenames(*((GetFilenamesCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case GetFileStatData:
                    getFileStatData(*((GetFileStatDataCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                case GetFileContents:
                    getFileContents(*((GetFileContentsCmd*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                    break;
                default:
                    invalidCmd(*((ToolCommand*)cmd_data), *((CommandDescriptor*)toolIpiRequest.descriptor[i].ack));
                }
                break;
            default:
                // Should never get here. This function should only be called for Query and Update messages
                printf("(E) ToolControl unexpected condition in processCommand()\n");
            }
            // Do we need to restore a hwpid that we modified?
            if (restorePID) 
            {
                // We need to restore the hwpid. 
                mtspr(SPRN_PID, origpid); 
                isync();
            }
        }
        // If a non-zero return code has occurred, set an indicator flag
        if (((CommandDescriptor*)toolIpiRequest.descriptor[i].ack)->returnCode != CmdSuccess)
        {
            failedCommands++;
        }



        Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLCMDCP, command, ((ToolCommand*)cmd_data)->threadID, 
                              ((CommandDescriptor*)toolIpiRequest.descriptor[i].ack)->returnCode,0 );
    }
    // Initialize the block and make it available for reuse.
    memset(&toolIpiRequest, 0, sizeof(ToolIpiRequest));    
    // Decrement the count of pending IPIs
    decrementPendingIPIs();

    return 0;
}

void ToolControl::cleanupThreadWatchpoints(KThread_t *kthread)
{
    lockWatchpointManager();
    watchpointManager().cleanupThreadWatchpoints(kthread);
    unlockWatchpointManager();
}

//-----------------------------------------------------------------------
//  Tool Command Implementations
//-----------------------------------------------------------------------

void ToolControl::getGeneralRegs(GetGeneralRegsCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(GetGeneralRegsAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    GetGeneralRegsAckCmd& ackcmd = *((GetGeneralRegsAckCmd *)msgarea);
    ackdesc.length = sizeof(GetGeneralRegsAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;

    for (uint32_t i=0; i<NumGPRegs; i++)
    {
        ackcmd.gpr[i] = kthread->Reg_State.gpr[i];
    }
    ackcmd.threadID = cmd.threadID;
    TRACE( TRACE_Toolctl, ("(I) ToolControl::getGeneralRegs: stored data for thread ID %u at %p\n", ackcmd.threadID, msgarea) );
}

void ToolControl::getSpecialRegs(GetSpecialRegsCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(GetSpecialRegsAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    GetSpecialRegsAckCmd& ackcmd = *((GetSpecialRegsAckCmd *)msgarea);
    ackdesc.length = sizeof(GetSpecialRegsAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    ackcmd.sregs.iar = kthread->Reg_State.ip; 
    ackcmd.sregs.lr  = kthread->Reg_State.lr;
    ackcmd.sregs.msr = kthread->Reg_State.msr;
    ackcmd.sregs.cr  = kthread->Reg_State.cr;
    ackcmd.sregs.ctr = kthread->Reg_State.ctr;
    ackcmd.sregs.xer = kthread->Reg_State.xer;
    ackcmd.sregs.fpscr = kthread->Reg_State.fpscr;
    ackcmd.sregs.dear = kthread->Reg_State.dear;
    ackcmd.sregs.esr = kthread->Reg_State.esr;

    ackdesc.returnCode = CmdSuccess;
    TRACE( TRACE_Toolctl, ("(I) ToolControl::getSpecialRegs: stored data for thread ID %u at %p\n", ackcmd.threadID, msgarea) );
    return;
}

void ToolControl::getFloatRegs(GetFloatRegsCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(GetFloatRegsAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    GetFloatRegsAckCmd& ackcmd = *((GetFloatRegsAckCmd *)msgarea);
    ackdesc.length = sizeof(GetFloatRegsAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID; 

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;

    for (uint32_t i=0; i<NumFPRegs; i++)
    {
        for (int j=0; j<4; j++)
        {
            ackcmd.fpr[i].ll[j] = kthread->Reg_State.qvr[i].ll[j];
        }
    }
    ackcmd.threadID = cmd.threadID;
    TRACE( TRACE_Toolctl, ("(I) ToolControl::getFloatRegs: stored data for thread ID %u at %p\n", ackcmd.threadID, msgarea) );
    return;
}

void ToolControl::getDebugRegs(GetDebugRegsCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(GetDebugRegsAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    GetDebugRegsAckCmd& ackcmd = *((GetDebugRegsAckCmd *)msgarea);
    ackdesc.length = sizeof(GetDebugRegsAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
   ackcmd.dbregs.dbcr0 = kthread->Reg_State.dbcr0;
   ackcmd.dbregs.dbcr1 = kthread->Reg_State.dbcr1;
   ackcmd.dbregs.dbcr2 = kthread->Reg_State.dbcr2;
   ackcmd.dbregs.dbcr3 = kthread->Reg_State.dbcr3;
   ackcmd.dbregs.dac1  = kthread->Reg_State.dac1;
   ackcmd.dbregs.dac2  = kthread->Reg_State.dac2;
   ackcmd.dbregs.dac3 = kthread->Reg_State.dac3;                      
   ackcmd.dbregs.dac4 = kthread->Reg_State.dac4;
   ackcmd.dbregs.iac1  = kthread->Reg_State.iac1;
   ackcmd.dbregs.iac2  = kthread->Reg_State.iac2;
   ackcmd.dbregs.iac3 = kthread->Reg_State.iac3;                      
   ackcmd.dbregs.iac4 = kthread->Reg_State.iac4;                      
   ackcmd.dbregs.dbsr = kthread->Reg_State.dbsr;
                         
   ackdesc.returnCode = CmdSuccess;
}
void ToolControl::getMemory(GetMemoryCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data

    // Adjust to size of the ack to the number of bytes requested, not the maximum allowed
    void *msgarea;
    uint32_t modifiedAckCmdSize = ((cmd.length>MaxMemorySize) ? sizeof(GetMemoryAckCmd) : (sizeof(GetMemoryAckCmd) + cmd.length));
    msgarea = allocateOutbound(ROUND_UP_DWORD(modifiedAckCmdSize));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    GetMemoryAckCmd& ackcmd = *((GetMemoryAckCmd *)msgarea);
    ackdesc.length = modifiedAckCmdSize;
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    if (cmd.length > MaxMemorySize)
    {
        ackdesc.returnCode = CmdLngthErr;
        return;
    }

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }

    // Execute the requested command
    uint8_t *address =(uint8_t*)cmd.addr;
    ackcmd.addr = cmd.addr;
    ackcmd.length = cmd.length;

    // Make sure the address is valid. Need to pass in the process since we may have interrupted a kthread on a different process
    // if the target kthread is a pthread running on a remote hardware thread in the Extended Affinity facility.
    if ( !VMM_IsAppAddressForProcess(address, ackcmd.length, kthread->pAppProc) )
    {
        ackdesc.returnCode = CmdParmErr;
        return;
    }

    // Copy the data

    // Are we in the shared text segment of a static executable?
    // If so we must read the aligned words from the breakpoint manager.
    int isUniqueAddress = VMM_IsAppProcessUniqueAddress((void*)cmd.addr, cmd.length);

    if (!isUniqueAddress)
    {
        uint32_t i = 0;

        while (i < ackcmd.length)
        {
            // Is the source address on a 4 byte boundary and is our remaining length >= 4 bytes?
            uint64_t source = (uint64_t)(&(address[i]));
            if (!(source % 4) && ((ackcmd.length - i) >= 4))
            {
                *((uint32_t*)(&ackcmd.data[i])) = breakpointController().read((uint32_t*)source, kthread->pAppProc);
                i += 4;
            }
            else
            {
                ackcmd.data[i] = address[i];
                i += 1;
            }
        }
    }
    else
    {
        L2C_SPECID_t specid = 0;
        uint64_t     origpid = mfspr(SPRN_PID);
        if (cmd.specAccess == SpecAccess_ForceNonSpeculative)
        {
            mtspr(SPRN_PID, kthread->pAppProc->PhysicalPID); // long-speculative mode
            isync();
            
            specid = SPEC_GetSpeculationIDSelf_priv() & 0x1ff;
            SPEC_SetSpeculationIDSelf_priv(0x000);
            ppc_msync();
            ppc_msync();
            ppc_msync();
            asm volatile ("dci 2");
        }
        // read the requested data
        memcpy(ackcmd.data, address, ackcmd.length);

        if (cmd.specAccess == SpecAccess_ForceNonSpeculative)
        {
            SPEC_SetSpeculationIDSelf_priv(specid);
            ppc_msync();
            ppc_msync();
            ppc_msync();
            mtspr(SPRN_PID, origpid);
            isync();
            asm volatile ("dci 2");
        }
    }
    ackdesc.returnCode = CmdSuccess;
    TRACE( TRACE_Toolctl, ("(I) ToolControl::getMemory: stored data for thread ID %u at %p\n", ackcmd.threadID, msgarea) );
    return;
}

void ToolControl::getAuxVectors(GetAuxVectorsCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(GetAuxVectorsAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    GetAuxVectorsAckCmd& ackcmd = *((GetAuxVectorsAckCmd *)msgarea);
    ackdesc.length = sizeof(GetAuxVectorsAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;  
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    uint64_t *pAuxVect = kthread->pAppProc->pAuxVectors;
    ackcmd.length = 0;
    for (uint32_t i=0; i<MaxAuxVecDWords; i++)
    {
        ackcmd.data[i] = *pAuxVect++;
        ackcmd.length++;
        // test to see if we have seen a pair of NULL values. If so, we are at the end of the list
        if (i%2 && !ackcmd.data[i-1] && !ackcmd.data[i]) break;
    }
    ackdesc.returnCode = CmdSuccess;
}
void ToolControl::getProcessData(GetProcessDataCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(GetProcessDataAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    GetProcessDataAckCmd& ackcmd = *((GetProcessDataAckCmd *)msgarea);
    ackdesc.length = sizeof(GetProcessDataAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    AppState_t *app = GetMyAppState();
    AppProcess_t *proc = kthread->pAppProc;
    Personality_t *pers = GetPersonality();
    ackcmd.rank = proc->Rank;                  
    ackcmd.tgid = proc->PID;                  
    ackcmd.aCoord = pers->Network_Config.Acoord;                
    ackcmd.bCoord = pers->Network_Config.Bcoord;                
    ackcmd.cCoord = pers->Network_Config.Ccoord;                
    ackcmd.dCoord = pers->Network_Config.Dcoord;                
    ackcmd.eCoord = pers->Network_Config.Ecoord;                
    ackcmd.tCoord = proc->Tcoord;                
    ackcmd.sharedMemoryStartAddr = NodeState.SharedMemory.VStart;
    ackcmd.sharedMemoryEndAddr = (NodeState.SharedMemory.Size ? (NodeState.SharedMemory.VStart + NodeState.SharedMemory.Size - 1) : NodeState.SharedMemory.VStart);   
    ackcmd.persistMemoryStartAddr  = NodeState.PersistentMemory.VStart;
    ackcmd.persistMemoryEndAddr = (NodeState.PersistentMemory.Size ? (NodeState.PersistentMemory.VStart + NodeState.PersistentMemory.Size - 1) : NodeState.PersistentMemory.VStart);   
    ackcmd.heapStartAddr = proc->Heap_Start;         
    ackcmd.heapEndAddr = proc->Heap_End;        
    ackcmd.heapBreakAddr = proc->Heap_Break;         
    ackcmd.mmapStartAddr = proc->MMap_Start;         
    ackcmd.mmapEndAddr = proc->MMap_End;
    uint64_t raw_usecs_start = app->JobStartTime - app->Timeshift;
    uint64_t cyclesPerMicro = GetPersonality()->Kernel_Config.FreqMHz;
    uint64_t raw_usecs_now = GetTimeBase() / cyclesPerMicro;
    uint64_t job_usecs = raw_usecs_now - raw_usecs_start;
    ackcmd.jobTime.tv_sec = job_usecs/1000000;              
    ackcmd.jobTime.tv_usec = job_usecs%1000000;              

    ackdesc.returnCode = CmdSuccess;

}
void ToolControl::getThreadData(GetThreadDataCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(GetThreadDataAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    GetThreadDataAckCmd& ackcmd = *((GetThreadDataAckCmd *)msgarea);
    ackdesc.length = sizeof(GetThreadDataAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    AppProcess_t *proc = kthread->pAppProc;
    ackcmd.core = ProcessorCoreID();                     
    ackcmd.thread = ProcessorThreadID();
    if (proc->Guard_Enable)
    {
        ackcmd.guardStartAddr = proc->Heap_VStart + (kthread->GuardBaseAddress - proc->Heap_PStart);
        ackcmd.guardEndAddr =  ackcmd.guardStartAddr | (~(kthread->GuardEnableMask));
    }
    else
    {
        ackcmd.guardStartAddr = 0;
        ackcmd.guardEndAddr =  0;
    }
    
    // Calculate specState field
    L2C_SPECID_t specid = SPEC_GetSpeculationIDSelf_priv() & 0xff;
    if(specid == 0)
        ackcmd.specState = NonSpeculative;
    else
    {
        specid &= 0x7f;
        uint64_t domainMode = SPEC_GetDomainMode_priv(specid / SPEC_GetNumberOfDomains()) & L2C_DOMAINATTR_MODE(-1);
        uint64_t idstate = SPEC_GetIDState(specid);
        if(((domainMode == L2C_DOMAINATTR_MODE_LTLS)||(domainMode == L2C_DOMAINATTR_MODE_STLS)) && (idstate != L2C_IDSTATE_INVAL))
            ackcmd.specState = SpeculativeExecution;
        else if(((domainMode == L2C_DOMAINATTR_MODE_LTLS)||(domainMode == L2C_DOMAINATTR_MODE_STLS)) && (idstate == L2C_IDSTATE_INVAL))
            ackcmd.specState = SpeculativeExecution_Invalid;
        else if(((domainMode == L2C_DOMAINATTR_MODE_LTM)||(domainMode == L2C_DOMAINATTR_MODE_STM)) && (idstate != L2C_IDSTATE_INVAL))
            ackcmd.specState = TransactionalMemory;
        else
            ackcmd.specState = TransactionalMemory_Invalid;
    }
    
    HWThreadState_t *hwt = GetMyHWThreadState();
    Regs_t* context = &kthread->Reg_State;
    uint64_t *kernel_stack_start = (uint64_t *)(&(hwt->StandardStack));
    uint64_t *kernel_stack_end   = (uint64_t *)((uint64_t)&(hwt->StandardStack)) + sizeof(hwt->StandardStack);
    uint64_t *stkptr = (uint64_t*)(context->gpr[1]);
    ackcmd.numStackFrames = 0; // initialize to zero    
    while (stkptr &&   // stack pointer is not NULL
           (ackcmd.numStackFrames < MaxStackFrames) &&  // did not yet store the max number of frames
           (VMM_IsAppAddressForProcess(stkptr, sizeof(uint64_t[3]), proc) || // Stack pointer is a valid Application address or
            ((stkptr > kernel_stack_start) && (stkptr < kernel_stack_end)))) // stack pointer is a kernel stack address
    {
        ackcmd.stackInfo[ackcmd.numStackFrames].frameAddr = (BG_Addr_t)stkptr;  // curent stack pointer
        ackcmd.stackInfo[ackcmd.numStackFrames].savedLR = *(stkptr+2); // value of the saved link register in this frame
        ackcmd.numStackFrames++;           
        stkptr = (uint64_t*)(*stkptr);
    }
    ackcmd.stackCurrentAddr = context->gpr[1];
    ackcmd.stackStartAddr = (BG_Addr_t)kthread->pUserStack_Bot;
    ackcmd.state = Run; // initialize to running. Modify if any block indicators are active
    if(kthread->State & SCHED_STATE_FREE)
        ackcmd.state = Idle;
    else if (kthread->State & SCHED_STATE_FUTEX)
        ackcmd.state = FutexWait;
    else if (kthread->State & SCHED_STATE_SLEEP)
        ackcmd.state = Sleep;
    else if (kthread->State & SCHED_STATE_FLOCK)
        ackcmd.state = FLock;
    if ((kthread->State & SCHED_STATE_SUSPEND) && (kthread->State & SCHED_STATE_HOLD))
        ackcmd.toolState = HoldSuspend;
    else if (kthread->State & SCHED_STATE_HOLD)
        ackcmd.toolState = Hold;
    else if (kthread->State & SCHED_STATE_SUSPEND)
        ackcmd.toolState = Suspend;
    else
        ackcmd.toolState = Active;
    ackdesc.returnCode = CmdSuccess;

}
void ToolControl::setSpecialReg(SetSpecialRegCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(SetSpecialRegAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    SetSpecialRegAckCmd& ackcmd = *((SetSpecialRegAckCmd *)msgarea);
    ackdesc.length = sizeof(SetSpecialRegAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;
    switch (cmd.reg_select)
    {
    case bgcios::toolctl::iar:
        kthread->Reg_State.ip = cmd.value;
        break;
    case bgcios::toolctl::lr:
        kthread->Reg_State.lr = cmd.value;
        break;
    case bgcios::toolctl::msr:
        kthread->Reg_State.msr = cmd.value;
        break;
    case bgcios::toolctl::cr:
        kthread->Reg_State.cr = cmd.value;
        break;
    case bgcios::toolctl::ctr:
        kthread->Reg_State.ctr = cmd.value;
        break;
    case bgcios::toolctl::xer:
        kthread->Reg_State.xer = cmd.value;
        break;
    case bgcios::toolctl::fpscr:
        kthread->Reg_State.fpscr = cmd.value;
        break;
    case bgcios::toolctl::dear:
        kthread->Reg_State.dear = cmd.value;
        break;
    case bgcios::toolctl::esr:
        kthread->Reg_State.esr = cmd.value;
        break;

    default:
        ackdesc.returnCode = CmdParmErr;
    }
}
void ToolControl::setSpecialRegs(SetSpecialRegsCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(SetSpecialRegsAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    SetSpecialRegsAckCmd& ackcmd = *((SetSpecialRegsAckCmd *)msgarea);
    ackdesc.length = sizeof(SetSpecialRegsAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;
    kthread->Reg_State.ip = cmd.sregs.iar; 
    kthread->Reg_State.lr = cmd.sregs.lr;
    kthread->Reg_State.msr = cmd.sregs.msr;
    kthread->Reg_State.cr = cmd.sregs.cr;
    kthread->Reg_State.ctr = cmd.sregs.ctr;
    kthread->Reg_State.xer = cmd.sregs.xer;
    kthread->Reg_State.fpscr = cmd.sregs.fpscr;
    kthread->Reg_State.dear = cmd.sregs.dear;
    kthread->Reg_State.esr = cmd.sregs.esr;
}

void ToolControl::setGeneralReg(SetGeneralRegCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(SetGeneralRegAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    SetGeneralRegAckCmd& ackcmd = *((SetGeneralRegAckCmd *)msgarea);
    ackdesc.length = sizeof(SetGeneralRegAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    if (!((cmd.reg_select >= gpr0) || (cmd.reg_select <= gpr31)))
    {
        ackdesc.returnCode =  CmdParmErr;
    }

    // Set the requested register
    kthread->Reg_State.gpr[cmd.reg_select] = cmd.value;
    ackdesc.returnCode = CmdSuccess;
}

void ToolControl::setGeneralRegs(SetGeneralRegsCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(SetGeneralRegsAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    SetGeneralRegsAckCmd& ackcmd = *((SetGeneralRegsAckCmd *)msgarea);
    ackdesc.length = sizeof(SetGeneralRegsAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Set the registers
    for (uint32_t i=0; i<NumGPRegs; i++)
    {
        kthread->Reg_State.gpr[i] = cmd.gpr[i];
    }
    ackdesc.returnCode = CmdSuccess;
}

void ToolControl::setFloatReg(SetFloatRegCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(SetFloatRegAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    SetFloatRegAckCmd& ackcmd = *((SetFloatRegAckCmd *)msgarea);
    ackdesc.length = sizeof(SetFloatRegAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;

    // Set the floating point register data
    for (int i=0; i<4; i++)
    {
        kthread->Reg_State.qvr[cmd.reg_select].ll[i] = cmd.value.ll[i];
    }
    ackcmd.threadID = cmd.threadID;

}

void ToolControl::setFloatRegs(SetFloatRegsCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(SetFloatRegsAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    SetFloatRegsAckCmd& ackcmd = *((SetFloatRegsAckCmd *)msgarea);
    ackdesc.length = sizeof(SetFloatRegsAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;

    // Set the floating point register data
    for (uint32_t i=0; i<NumFPRegs; i++)
    {
        for (int j=0; j<4; j++)
        {
             kthread->Reg_State.qvr[i].ll[j] = cmd.fpr[i].ll[j];
        }
    }
    ackcmd.threadID = cmd.threadID;

}

void ToolControl::setDebugReg(SetDebugRegCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(SetDebugRegAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    SetDebugRegAckCmd& ackcmd = *((SetDebugRegAckCmd *)msgarea);
    ackdesc.length = sizeof(SetDebugRegAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;
    HWThreadState_t *hwt = GetMyHWThreadState();
    switch (cmd.reg_select)
    {
    case dbcr0:
        hwt->launchContextFlags.flag.DebugControlRegs = 1;
        kthread->Reg_State.dbcr0 = cmd.value;
        break;
    case dbcr1:
        hwt->launchContextFlags.flag.DebugControlRegs = 1;
        kthread->Reg_State.dbcr1 = cmd.value;
        break;
    case dbcr2:
        hwt->launchContextFlags.flag.DebugControlRegs = 1;
        kthread->Reg_State.dbcr2 = cmd.value;
        break;
    case dbcr3:
        hwt->launchContextFlags.flag.DebugControlRegs = 1;
        kthread->Reg_State.dbcr3 = cmd.value;
        break;
    case dac1:
        kthread->Reg_State.dac1 = cmd.value;
        mtspr(SPRN_DAC1, kthread->Reg_State.dac1);
        break;
    case dac2:
        kthread->Reg_State.dac2 = cmd.value;
        mtspr(SPRN_DAC2, kthread->Reg_State.dac2);
        break;
    case dac3:
        kthread->Reg_State.dac3 = cmd.value;
        mtspr(SPRN_DAC3, kthread->Reg_State.dac3);
        break;
    case dac4:
        kthread->Reg_State.dac4 = cmd.value;
        mtspr(SPRN_DAC4, kthread->Reg_State.dac4);
        break;
    case iac1:
        kthread->Reg_State.iac1 = cmd.value;
        mtspr(SPRN_IAC1, kthread->Reg_State.iac1);
        break;
    case iac2:
        kthread->Reg_State.iac2 = cmd.value;
        mtspr(SPRN_IAC2, kthread->Reg_State.iac2);
        break;
    case iac3:
        kthread->Reg_State.iac3 = cmd.value;
        mtspr(SPRN_IAC3, kthread->Reg_State.iac3);
        break;
    case iac4:
        kthread->Reg_State.iac4 = cmd.value;
        mtspr(SPRN_IAC4, kthread->Reg_State.iac4);
        break;

    case dbsr:
        // We keep the DBSR value live. Writing a 1 causes the corresponding bit to be reset in the hardware!
        mtspr(SPRN_DBSR, cmd.value); 
        break;
    default:
        ackdesc.returnCode = CmdParmErr;
    }
}

void ToolControl::setDebugRegs(SetDebugRegsCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(SetDebugRegsAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    SetDebugRegsAckCmd& ackcmd = *((SetDebugRegsAckCmd *)msgarea);
    ackdesc.length = sizeof(SetDebugRegsAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;
    HWThreadState_t *hwt = GetMyHWThreadState();
    hwt->launchContextFlags.flag.DebugControlRegs = 1;
    kthread->Reg_State.dbcr0 = cmd.dbregs.dbcr0;
    hwt->launchContextFlags.flag.DebugControlRegs = 1;
    kthread->Reg_State.dbcr1 = cmd.dbregs.dbcr1;
    hwt->launchContextFlags.flag.DebugControlRegs = 1;
    kthread->Reg_State.dbcr2 = cmd.dbregs.dbcr2;
    hwt->launchContextFlags.flag.DebugControlRegs = 1;
    kthread->Reg_State.dbcr3 = cmd.dbregs.dbcr3;
    kthread->Reg_State.dac1 = cmd.dbregs.dac1;
    mtspr(SPRN_DAC1, kthread->Reg_State.dac1);
    kthread->Reg_State.dac2 = cmd.dbregs.dac2;
    mtspr(SPRN_DAC2, kthread->Reg_State.dac2);
    kthread->Reg_State.dac3 = cmd.dbregs.dac3;
    mtspr(SPRN_DAC3, kthread->Reg_State.dac3);
    kthread->Reg_State.dac4 = cmd.dbregs.dac4;
    mtspr(SPRN_DAC4, kthread->Reg_State.dac4);
    kthread->Reg_State.iac1 = cmd.dbregs.iac1;
    mtspr(SPRN_IAC1, kthread->Reg_State.iac1);
    kthread->Reg_State.iac2 = cmd.dbregs.iac2;
    mtspr(SPRN_IAC2, kthread->Reg_State.iac2);
    kthread->Reg_State.iac3 = cmd.dbregs.iac3;
    mtspr(SPRN_IAC3, kthread->Reg_State.iac3);
    kthread->Reg_State.iac4 = cmd.dbregs.iac4;
    mtspr(SPRN_IAC4, kthread->Reg_State.iac4);
    // We keep the DBSR value live. Writing a 1 causes the corresponding bit to be reset in the hardware!
    mtspr(SPRN_DBSR, cmd.dbregs.dbsr); 
}

void ToolControl::setMemory(SetMemoryCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(SetMemoryAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    SetMemoryAckCmd& ackcmd = *((SetMemoryAckCmd *)msgarea);
    ackdesc.length = sizeof(SetMemoryAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    if (cmd.length > MaxMemorySize)
    {
        // length parameter is invalid
        ackdesc.returnCode = CmdParmErr;
        return;
    }
    if (cmd.length == 0)
    {
        ackdesc.returnCode = CmdSuccess;
        return;
    }
    if (!VMM_IsAppAddressForProcess((void*)cmd.addr, cmd.length, kthread->pAppProc))
    {
        // Address parameter is invalid
        ackdesc.returnCode = CmdParmErr;
        return;
    }
    // GDB client does support sending/receiving a setBreakpoint and resetBreakpoint command, however the client does not
    // pass the original instruction data on the resetBreakpoint command to the server. Also this CDTI interface does not 
    // track all breakpoints, therefore the CDTI resetBreakpoint interface requires the caller to pass the orginal instruction. 
    // Because of this, the GDB server uses setMemory to set and reset breakpoints. The follow section of code provides 
    // the setBreakpoint and ResetBreakpoint support through the setMemory interface for use by GDB and and potentially other tools. 
    int isUniqueAddress = VMM_IsAppProcessUniqueAddress((void*)cmd.addr, cmd.length);
    if (!isUniqueAddress && (cmd.length == 4))
    {
        if (TOOL_ISTRAP(*((uint32_t*)cmd.data)))
        {
            // Use the set breakpoint interface

            // Call the breakpoint controller
            //printf("Calling add breakoint. Address %016lx, data %08x, size %d\n", cmd.addr, *((int32_t*)cmd.data), cmd.length);
            int result = breakpointController().add((void*)cmd.addr, *((uint32_t*)cmd.data), kthread->pAppProc); 
            Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLABRKM, (uint64_t)cmd.addr, (uint64_t)cmd.data, (uint64_t)result,0);

            if (result == -1)
            {
                ackdesc.returnCode = CmdBrkptFail;
                return;
            }
            icache_block_inval((void*)cmd.addr);
            ppc_msync();
            ackcmd.addr = cmd.addr;
            ackcmd.length = cmd.length;
            return;
 
        }
        else
        {
            // We may be reseting a breakpoint. Attempt to reset.
            // Call the breakpoint controller 
            //printf("Calling remove breakoint. Address %016lx, data %08x, size %d\n", cmd.addr, *((int32_t*)cmd.data), cmd.length);
            int result = breakpointController().remove((void*)cmd.addr, *((uint32_t*)cmd.data), kthread->pAppProc);
            Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLRBRKM, (uint64_t)cmd.addr, (uint64_t)cmd.data, (uint64_t)result,0);

            if (result == 0)
            {
                // Reset of the breakpoint was successful, we are done.
                ackcmd.addr = cmd.addr;
                ackcmd.length = cmd.length;
                return;
            }
            // If reach here, the breakpoint reset was unsuccesful. This is not an error. We must
            // allow this flow to continue and deal with this as a normal 4-byte write operation.
        }
    }
    if (!isUniqueAddress &&  // Is this shared process memory
        (cmd.sharedMemoryAccess == SharedMemoryAccess_NotAllow))        // Is shared write not allowed
    {
        // Address parameter is invalid
        ackdesc.returnCode = CmdParmErr;
        return;
    }
    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;

    L2C_SPECID_t specid = 0;
    if (cmd.specAccess == SpecAccess_ForceNonSpeculative)
    {
        specid = SPEC_GetSpeculationIDSelf_priv() & 0x1ff;
        SPEC_SetSpeculationIDSelf_priv(0x000);
        ppc_msync();
        ppc_msync();
        ppc_msync();
    }
    memcpy_Inline((void*)cmd.addr, cmd.data, cmd.length);
    if (cmd.specAccess == SpecAccess_ForceNonSpeculative)
    {
        SPEC_SetSpeculationIDSelf_priv(specid);
        ppc_msync();
        ppc_msync();
        ppc_msync();
    }
    
    // Invalidate the icache for the blocks that we are writing
    for (uint64_t blockaddr = cmd.addr; blockaddr < cmd.addr+cmd.length; blockaddr += L1I_CACHE_LINE_SIZE)
    {
       // We need to invalidate the icache so that it sees the data modification that was just written through the dcache
       icache_block_inval((void*)blockaddr);
    }
    ppc_msync();
    ackcmd.addr = cmd.addr;
    ackcmd.length = cmd.length;
}


void ToolControl::holdThread(HoldThreadCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(HoldThreadAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    HoldThreadAckCmd& ackcmd = *((HoldThreadAckCmd *)msgarea);
    ackdesc.length = sizeof(HoldThreadAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;

    // Set the HOLD blocking code into the scheduler state for the target kthread
    Sched_Block(kthread,SCHED_STATE_HOLD);
}
void ToolControl::releaseAllThreads(ReleaseAllThreadsCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(ReleaseAllThreadsAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    ReleaseAllThreadsAckCmd& ackcmd = *((ReleaseAllThreadsAckCmd *)msgarea);
    ackdesc.length = sizeof(ReleaseThreadAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;

    // Remove the HOLD blocking code for all threads in the process
    releaseProcess(kthread->pAppProc);

}
void ToolControl::releaseThread(ReleaseThreadCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(ReleaseThreadAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    ReleaseThreadAckCmd& ackcmd = *((ReleaseThreadAckCmd *)msgarea);
    ackdesc.length = sizeof(ReleaseThreadAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;

    // Remove the HOLD blocking code for the target kthread
    Sched_Unblock(kthread,SCHED_STATE_HOLD);
}

void ToolControl::stepThread(StepThreadCmd& cmd, CommandDescriptor& ackdesc)
{
    //printf("StepThread Command\n");
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(StepThreadAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    StepThreadAckCmd& ackcmd = *((StepThreadAckCmd *)msgarea);
    ackdesc.length = sizeof(StepThreadAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // We can be stepping the thread that notified the tool or stepping a suspended thread due to some other thread's notify.

    // If the target thread has any blocking codes set other than suspend. Fail the step operation since the step would not complete
    if (((kthread->State & ~SCHED_STATE_SUSPEND) != 0) || // Make sure no other blocking codes area active.
        !(kthread->State & SCHED_STATE_SUSPEND))          // Make sure we are in the suspended state.
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }

    // Get a pointer to the process associated with the target thread.
    AppProcess_t *proc = kthread->pAppProc;

    // Need to reset the bit in the NotifyControlMask. Note that we may be clearing a bit associated with 
    // a different hardware thread than we are running on if we are stepping a different thread than the thread 
    // that was last notified.
    KThread_t *notifiedKThread = proc->notifiedKThread;
    int notifiedHwthread =  notifiedKThread->ProcessorID;
    uint64_t resetmask = ~(_BN(notifiedHwthread));
    uint64_t prevNotifyControlMask = fetch_and_and64(&proc->NotifyControlMask, resetmask);
    Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLCMDST, prevNotifyControlMask, notifiedHwthread, GetTID(notifiedKThread), cmd.threadID);

    if ((prevNotifyControlMask & ~resetmask) == 0)
    {
        printf("(E) Tool Step Thread unexpected condition! NotifyControlMask=%016lx NotifiedHwThread=%d\n", prevNotifyControlMask, notifiedHwthread);
        ackdesc.returnCode = CmdParmErr;
        return;
    }
    // Set the continuation signal information
    // Modify the continuation signal
    if (proc->notifiedKThread->SigInfoSigno != proc->continuationSignal)
    {
        proc->notifiedKThread->SigInfoSigno = proc->continuationSignal;
        proc->notifiedKThread->SigInfoAddr = NULL;
        proc->notifiedKThread->SigInfoCode = 0;
        if (!proc->continuationSignal)
        {
            proc->notifiedKThread->ExceptionCode = 0;
        }
    }
    // We have processed the notification information. Reset it.
    proc->notifiedKThread = NULL;
    proc->continuationSignal = 0;

    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;

    // NOTE: We are purposely not obtaining the breakpoint controller lock. The following call does not need to be
    //       under the control of the lock.
    breakpointController().prepareToStep(kthread);

    // Are there more pending notify conditions than the just the one we reset?
    //printf("Ready to step. prevNotifyControlMask %016lx resetMask %016lx notifiedHwthread %d notifiedKthread %016lx\n", prevNotifyControlMask, resetmask, notifiedHwthread, (uint64_t)notifiedKThread);
    if (popcnt64(prevNotifyControlMask) != 1)
    {
        // There are other threads waiting to send their notifications. Select another thread and
        // send the required notification message.
        int pendingHwt = cntlz64(prevNotifyControlMask & resetmask);
        proc->actionPendingKthread = pendingNotifyKThread[pendingHwt];
        proc->pendingAction = ToolPendingAction_NotifySignal;
        // Do not unblock the suspend condition on the thread yet. Just let the next notification get processed.
        return;
    }
    // Set indicator that will remove the SUSPEND blocking code in the target kthread after we send the Ack for this message
    proc->actionPendingKthread = kthread;
    proc->pendingAction = ToolPendingAction_ResumeThd;
    // When this kthread is dispatched the scheduler will setup the step operation. 
}

void ToolControl::installTrapHandler(InstallTrapHandlerCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(InstallTrapHandlerAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    InstallTrapHandlerAckCmd& ackcmd = *((InstallTrapHandlerAckCmd *)msgarea);
    ackdesc.length = sizeof(InstallTrapHandlerAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command

    // Verify that the trap handler is a valid address in the process address space.
    if (VMM_IsAppAddressForProcess((const void*)cmd.trap_handler,16,kthread->pAppProc)) // check 16 bytes at this address to cover the code and toc pointers
    {
        struct kern_sigaction_t sa;
        memset(&sa,0x00, sizeof(sa));
        sa.sa_handler = (void (*)(int))cmd.trap_handler;
        sa.sa_flags = SA_SIGINFO;
        Signal_SetAction(SIGTRAP, &sa, NULL);
        ackdesc.returnCode = CmdSuccess;
    }
    else
    {
        ackdesc.returnCode = CmdAddrErr;
    }
}

void ToolControl::removeTrapHandler(RemoveTrapHandlerCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(RemoveTrapHandlerAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    RemoveTrapHandlerAckCmd& ackcmd = *((RemoveTrapHandlerAckCmd *)msgarea);
    ackdesc.length = sizeof(RemoveTrapHandlerAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    struct kern_sigaction_t sa;
    memset(&sa,0x00, sizeof(sa));
    sa.sa_handler = SIG_DFL;
    Signal_SetAction(SIGTRAP, &sa, NULL);
    ackdesc.returnCode = CmdSuccess;
}

void ToolControl::allocateMemory(AllocateMemoryCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(AllocateMemoryAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    AllocateMemoryAckCmd& ackcmd = *((AllocateMemoryAckCmd *)msgarea);
    ackdesc.length = sizeof(AllocateMemoryAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    uint64_t alloc_addr = sc_mmap( NULL, cmd.alloc_size, 0, 0, 0, 0);
    if (CNK_RC_IS_SUCCESS(alloc_addr) )
    {
        ackdesc.returnCode = CmdSuccess;
        ackcmd.alloc_addr = (void*)alloc_addr;
    }
    else
    {
        ackdesc.returnCode = CmdMemUnavail;
        ackcmd.alloc_addr = NULL;
    }
}

void ToolControl::freeMemory(FreeMemoryCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(FreeMemoryAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    FreeMemoryAckCmd& ackcmd = *((FreeMemoryAckCmd *)msgarea);
    ackdesc.length = sizeof(FreeMemoryAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    MMap_Free(cmd.alloc_addr, cmd.alloc_size);
    ackdesc.returnCode = CmdSuccess;
}

void ToolControl::getThreadList(GetThreadListCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(GetThreadListAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    GetThreadListAckCmd& ackcmd = *((GetThreadListAckCmd *)msgarea);
    ackdesc.length = sizeof(GetThreadListAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;

    // Loop through all the kthreads and find the ones allocated to this process and currently active
    ackcmd.numthreads = 0;
    for (uint32_t i=0; i<NUM_KTHREADS && i<MaxThreadIds; i++)
    {
        if (NodeState.KThreads[i].pAppProc == kthread->pAppProc)
        {
            KThread_t* candidate = &NodeState.KThreads[i]; 
            // We do not want to report the special kernel kthreads
            if (candidate->SlotIndex != CONFIG_SCHED_KERNEL_SLOT_INDEX)
            {
                // We do not want to report kthreads that are not currently associated with an application thread (pthread)
                if (!(candidate->State & SCHED_STATE_FREE) &&
                    !(candidate->State & SCHED_STATE_RESET) &&
                    !(candidate->State & SCHED_STATE_POOF) &&
                    !(candidate->State & SCHED_STATE_APPEXIT))
                {
                    ackcmd.threadlist[ackcmd.numthreads].tid = GetTID(candidate);
                    ackcmd.threadlist[ackcmd.numthreads].info.commthread = ((candidate->Policy == SCHED_COMM) ? 1 : 0);
                    ackcmd.numthreads++;
                }
            }
        }
    }
}

ContinueProcessRC ToolControl::continueProcess(KThread_t *targetThread)
{
    uint64_t myMask = _BN(ProcessorID());
    // Reset our Notify Control Mask bit
    AppProcess_t *proc = targetThread->pAppProc;

    // Is the target kthread the last kthread that was notified?
    if (targetThread != proc->notifiedKThread)
    {
        return ContinueProcessRC_InvalidThread;
    }
    uint64_t prevMask = fetch_and_and64(&(proc->NotifyControlMask), ~myMask);
    int notifiedHwthread = proc->notifiedKThread->ProcessorID;
    Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLCMDCO, prevMask, notifiedHwthread, GetTID(proc->notifiedKThread), GetTID(targetThread));
    // If the previous mask did not have our notify bit set, then an invalid targetThread was specified.
    if (!(prevMask & myMask))
    {
        return ContinueProcessRC_InvalidThread;
    }

    // Modify the continuation signal
    if (proc->notifiedKThread->SigInfoSigno != proc->continuationSignal)
    {
        proc->notifiedKThread->SigInfoSigno = proc->continuationSignal;
        proc->notifiedKThread->SigInfoAddr = NULL;
        proc->notifiedKThread->SigInfoCode = 0;
        if (!proc->continuationSignal)
        {
            proc->notifiedKThread->ExceptionCode = 0;
        }
    }
    proc->notifiedKThread = 0;
    proc->continuationSignal = 0;

    // Was our mask bit the last mask bit to be turned off? If so we must do the resume.
    if ((prevMask & ~myMask) == 0)
    {
        // Remove the Suspend blocking condition from all threads (including ours) in the process
        proc->actionPendingKthread = targetThread;
        proc->pendingAction = ToolPendingAction_ResumeProc;

        return ContinueProcessRC_Success;
    }
    else
    {
        // There are other threads waiting to send their notifications. Select another thread and
        // setup to send the required notification message.
        int pendingHwt = cntlz64(prevMask & ~myMask);
        proc->actionPendingKthread = pendingNotifyKThread[pendingHwt];
        proc->pendingAction = ToolPendingAction_NotifySignal;

        return ContinueProcessRC_PendingNotify;
    }
}

void ToolControl::continueProcess(ContinueProcessCmd& cmd, CommandDescriptor& ackdesc)
{
    //printf("ContinueProcess\n");
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(ContinueProcessAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    ContinueProcessAckCmd& ackcmd = *((ContinueProcessAckCmd *)msgarea);
    ackdesc.length = sizeof(ContinueProcessAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    ContinueProcessRC rc =continueProcess(kthread);

    if ((rc == ContinueProcessRC_Success) || (rc == ContinueProcessRC_PendingNotify))
    {
        ackdesc.returnCode = CmdSuccess;
    }
    else 
    {
        ackdesc.returnCode = CmdTIDinval;
    }
    // Return from this command processing. When we get back to the scheduler, this thread will be dispatched
}

void ToolControl::sendSignal(SendSignalCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(SendSignalAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    SendSignalAckCmd& ackcmd = *((SendSignalAckCmd *)msgarea);
    ackdesc.length = sizeof(SendSignalAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }

    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;

    // Command was to send a signal to the process. Call the Send Signal interface. 
    if (cmd.signum)
    {
        // Deliver the signal normally. It will be added to the pending mask of the target kthread and processed normally. Note that
        // If we are in the middle of a suspended signal delivery, we will not exit that signal delivery until a ContinueProcess, Step,
        // or ReleaseControl is done.

        Signal_Deliver(kthread->pAppProc, cmd.threadID, cmd.signum);
        TRACE( TRACE_Toolctl, ("(I) ToolControl::sendSignal: sent signal %u to thread ID %u\n", cmd.signum, cmd.threadID) );
    }
    return;
}


void ToolControl::sendSignalToProcess(SendSignalToProcessCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(SendSignalToProcessAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    SendSignalToProcessAckCmd& ackcmd = *((SendSignalToProcessAckCmd *)msgarea);
    ackdesc.length = sizeof(SendSignalToProcessAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }

    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;

    // Command was to send a signal to the process. Call the Send Signal interface. 
    if (cmd.signum)
    {
        // Deliver the signal normally. 
        Signal_Deliver(kthread->pAppProc, 0, cmd.signum);
        TRACE( TRACE_Toolctl, ("(I) ToolControl::sendSignalToProcess: sent signal %u to thread ID %u\n", cmd.signum, cmd.threadID) );
    }
    return;
}

void ToolControl::invalidTID(ToolCommand& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(ToolCommand));
    if (msgarea)
    {
        ToolCommand& ackcmd = *((ToolCommand *)msgarea);
        ackdesc.length = sizeof(ToolCommand);
        ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
        ackdesc.returnCode = CmdTIDinval;
        ackcmd.threadID = cmd.threadID;
    }
    else
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
    }
}

void ToolControl::invalidCmd(ToolCommand& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(ToolCommand));
    if (msgarea)
    {
        ToolCommand& ackcmd = *((ToolCommand *)msgarea);
        ackdesc.length = sizeof(ToolCommand);
        ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
        ackdesc.returnCode = CmdInval;
        ackcmd.threadID = cmd.threadID;
    }
    else
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
    }
}

void ToolControl::previousCmdFailure(ToolCommand& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(ToolCommand));
    if (msgarea)
    {
        ToolCommand& ackcmd = *((ToolCommand *)msgarea);
        ackdesc.length = sizeof(ToolCommand);
        ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
        ackdesc.returnCode = CmdPrevCmdFail;
        ackcmd.threadID = cmd.threadID;
    }
    else
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
    }
}

void ToolControl::processExitingCmdFailure(ToolCommand& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(ToolCommand));
    if (msgarea)
    {
        ToolCommand& ackcmd = *((ToolCommand *)msgarea);
        ackdesc.length = sizeof(ToolCommand);
        ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
        ackdesc.returnCode = CmdProcessExiting;
        ackcmd.threadID = cmd.threadID;
    }
    else
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
    }
}


int ToolControl::getAttachedTools(AppProcess_t* proc, int size, uint32_t* toolIds)
{
    int range = MIN(size, MAX_NUM_TOOLS);
    int procindex = proc->ProcessLeader_ProcessorID;
    int numreturned = 0;

    for (int i=0; i < range; i++)
    {
        int toolid = processEntry[procindex].toolId(i);
        if (toolid)
        {
            toolIds[numreturned] = toolid;
            numreturned++;
        }
        else
            break; // early exit from loop
    }
    return numreturned;
}

void ToolControl::setDefaultThreadId(int toolid, AppProcess_t *proc, uint32_t threadid)
{
    ToolTable& toolTable = processEntry[proc->ProcessLeader_ProcessorID];
    toolTable.setDefaultThreadId(toolid, threadid);
}


uint32_t ToolControl::getDefaultThreadId(AppProcess_t *proc)
{
    ToolTable& toolTable = processEntry[proc->ProcessLeader_ProcessorID];
    uint32_t threadid = toolTable.getDefaultThreadId(currentMsg->toolId);
    return threadid;
}

BreakpointController& ToolControl::breakpointController()
{ 
    return brkptController;
}

WatchpointManager& ToolControl::watchpointManager()
{ 
    return watchpointmanager;
}

void ToolControl::lockWatchpointManager()
{
    Kernel_Lock(&_watchpointManagerLock);    
}


void ToolControl::unlockWatchpointManager()
{
    Kernel_Unlock(&_watchpointManagerLock);    
}

void ToolControl::setBreakpoint(SetBreakpointCmd& cmd,  CommandDescriptor& ackdesc)
{
    //printf("Set Breakpoint %016lx\n", cmd.addr);
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(SetBreakpointAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    SetBreakpointAckCmd& ackcmd = *((SetBreakpointAckCmd *)msgarea);
    ackdesc.length = sizeof(SetBreakpointAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    if (!VMM_IsAppAddressForProcess((void*)cmd.addr, 4,kthread->pAppProc) ||
        (cmd.addr & 0x3))
    {
        // Address parameter is invalid
        ackdesc.returnCode = CmdParmErr;
        return;
    }
    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;
    
    if (!(TOOL_ISTRAP(cmd.instruction)))
    {
        // trap instruction parameter is invalid
        ackdesc.returnCode = CmdParmErr;
        return;
    }
    int isUniqueAddress = VMM_IsAppProcessUniqueAddress((void*)cmd.addr, 4);
    if (isUniqueAddress)
    {
        // Write the breakpoint into memory. No need to register breakpoint in the table.
        *((uint32_t*)cmd.addr) = cmd.instruction;
        ppc_msync();
        // We need to invalidate the icache so that it sees the data modification that was just written through the dcache
        icache_block_inval((void*)cmd.addr);
    }
    else
    {
        // Call the breakpoint controller 
        int result = breakpointController().add((void*)cmd.addr, cmd.instruction, kthread->pAppProc);
        Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLABRKB, (uint64_t)cmd.addr, (uint64_t)cmd.instruction, (uint64_t)result,0);
        if (result == -1)
        {
            ackdesc.returnCode = CmdBrkptFail;
        }
    }

    TRACE( TRACE_Toolctl, ("(I) ToolControl::setBreakpoint: set breakpoint for thread ID %u at 0x%lx\n", cmd.threadID, cmd.addr) );
    return;
}

void ToolControl::resetBreakpoint(ResetBreakpointCmd& cmd,  CommandDescriptor& ackdesc)
{
    //printf("Reset Breakpoint %016lx\n", cmd.addr);
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(ResetBreakpointAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    ResetBreakpointAckCmd& ackcmd = *((ResetBreakpointAckCmd *)msgarea);
    ackdesc.length = sizeof(ResetBreakpointAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    if (!VMM_IsAppAddressForProcess((void*)cmd.addr, 4, kthread->pAppProc) ||  // Is this a valid address
        (cmd.addr &0x3)) // Must be 4 byte aligned.
    {
        // Address parameter is invalid
        ackdesc.returnCode = CmdParmErr;
        return;
    }
    if (TOOL_ISTRAP(cmd.instruction))
    {
        // Instruction parameter is invalid
        ackdesc.returnCode = CmdParmErr;
        return;
    }
    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;

    int isUniqueAddress = VMM_IsAppProcessUniqueAddress((void*)cmd.addr, 4);
    if (isUniqueAddress)
    {
        // Write the instruction into memory. No need to call the breakpoint manager.
        *((uint32_t*)cmd.addr) = cmd.instruction;
        ppc_msync();
        // We need to invalidate the icache so that it sees the data modification that was just written through the dcache
        icache_block_inval((void*)cmd.addr);
    }
    else
    {
        // Call the breakpoint controller 
        int result = breakpointController().remove((void*)cmd.addr, cmd.instruction, kthread->pAppProc);
        Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLRBRKB, (uint64_t)cmd.addr, (uint64_t)cmd.instruction, (uint64_t)result,0);
        if (result == -1)
        {
            ackdesc.returnCode = CmdBrkptFail;
        }
    }
    TRACE( TRACE_Toolctl, ("(I) ToolControl::resetBreakpoint: reset breakpoint for thread ID %u at 0x%lx\n", cmd.threadID, cmd.addr) );
    return;
}


void ToolControl::setWatchpoint(SetWatchpointCmd& cmd,  CommandDescriptor& ackdesc)
{
    //printf("Set Watchpoint tid %04x %016lx length=%0d\n", cmd.threadID, cmd.addr, cmd.length);
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(SetWatchpointAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        //printf("Set Watchpoint return CmdAllocError\n");
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    SetWatchpointAckCmd& ackcmd = *((SetWatchpointAckCmd *)msgarea);
    ackdesc.length = sizeof(SetWatchpointAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;
    for (int i=0; i<2; i++)
    {
        ackcmd.watch[i].addr = 0;
        ackcmd.watch[i].length = 0;
        ackcmd.watch[i].type = WatchNone;
    }
    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        //printf("Set Watchpoint return CmdTIDinval %04x\n",cmd.threadID);
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Initialize the ack fields with the current watch info.

    if (kthread->watch1Type)
    {
        ackcmd.watch[0].type = (WatchType)kthread->watch1Type;
        ackcmd.watch[0].addr = kthread->Reg_State.dac1;
        ackcmd.watch[0].length = (~(kthread->Reg_State.dac2)) + 1;
    }
    if (kthread->watch2Type)
    {
        ackcmd.watch[1].type = (WatchType)kthread->watch2Type;
        ackcmd.watch[1].addr = kthread->Reg_State.dac3;
        ackcmd.watch[1].length = (~(kthread->Reg_State.dac4)) + 1;
    }
    if (cmd.length == 0)
    {
        // Address parameter is invalid
        //printf("Set Watchpoint return CmdLngthErr %0d\n",cmd.length);
        ackdesc.returnCode = CmdLngthErr;
        return;
    }
    if (!VMM_IsAppAddressForProcess((void*)cmd.addr, cmd.length, kthread->pAppProc))
    {
        // Address parameter is invalid
        //printf("Set Watchpoint return CmdAddrErr %016lx\n",cmd.addr);
        ackdesc.returnCode = CmdAddrErr;
        return;
    }
    // Execute the requested command
    lockWatchpointManager();
    ackdesc.returnCode = watchpointManager().setWatchpoint(cmd.addr, cmd.length, cmd.type, kthread);
    unlockWatchpointManager();
    if (ackdesc.returnCode == CmdSuccess)
    {
        //printf("Success setting watchpoint\n");
        // refresh the watch data in the ack message
        if (kthread->watch1Type)
        {
            ackcmd.watch[0].type = (WatchType)kthread->watch1Type;
            ackcmd.watch[0].addr = kthread->Reg_State.dac1;
            ackcmd.watch[0].length = (~(kthread->Reg_State.dac2)) + 1;
        }
        if (kthread->watch2Type)
        {
            ackcmd.watch[1].type = (WatchType)kthread->watch2Type;
            ackcmd.watch[1].addr = kthread->Reg_State.dac3;
            ackcmd.watch[1].length = (~(kthread->Reg_State.dac4)) + 1;
        }
    }
    TRACE( TRACE_Toolctl, ("(I) ToolControl::setWatchpoint: set watchpoint for thread ID %u at 0x%lx for %u bytes\n", cmd.threadID, cmd.addr, cmd.length) );
    return;
}

void ToolControl::resetWatchpoint(ResetWatchpointCmd& cmd,  CommandDescriptor& ackdesc)
{
    //printf("Reset Watchpoint tid %04x %016lx\n", cmd.threadID, cmd.addr);
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(ResetWatchpointAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    ResetWatchpointAckCmd& ackcmd = *((ResetWatchpointAckCmd *)msgarea);
    ackdesc.length = sizeof(ResetWatchpointAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;
    for (int i=0; i<2; i++)
    {
        ackcmd.watch[i].addr = 0;
        ackcmd.watch[i].length = 0;
        ackcmd.watch[i].type = WatchNone;
    }
    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        //printf("CmdTIDinval\n");
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command


    lockWatchpointManager();
    ackdesc.returnCode = watchpointManager().resetWatchpoint(cmd.addr, kthread);
    unlockWatchpointManager();
    if (ackdesc.returnCode == CmdSuccess)
    {
        if (kthread->watch1Type)
        {
            ackcmd.watch[0].type = (WatchType)kthread->watch1Type;
            ackcmd.watch[0].addr = kthread->Reg_State.dac1;
            ackcmd.watch[0].length = (~(kthread->Reg_State.dac2)) + 1;
        }
        if (kthread->watch2Type)
        {
            ackcmd.watch[1].type = (WatchType)kthread->watch2Type;
            ackcmd.watch[1].addr = kthread->Reg_State.dac3;
            ackcmd.watch[1].length = (~(kthread->Reg_State.dac4)) + 1;
        }
    }

    TRACE( TRACE_Toolctl, ("(I) ToolControl::resetWatchpoint: reset watchpoint for thread ID %u at 0x%lx\n", cmd.threadID, cmd.addr) );
    return;
}

void ToolControl::setPreferences(SetPreferencesCmd& cmd,  CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(SetPreferencesAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    SetPreferencesAckCmd& ackcmd = *((SetPreferencesAckCmd *)msgarea);
    ackdesc.length = sizeof(SetPreferencesAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command

    if (cmd.dacMode == TrapOnDAC )
    {
        breakpointController().setTrapOnDAC();
    }
    else if(cmd.dacMode == TrapAfterDAC)
    {
        breakpointController().setTrapAfterDAC();
    }
    int procindex = kthread->pAppProc->ProcessLeader_ProcessorID;
    if (cmd.breakpointMode == FastBreak_Enable)
    {
        processEntry[procindex].enableFastBreakpoints(currentMsg->toolId);
    }
    else if (cmd.breakpointMode == FastBreak_Disable)
    {
        processEntry[procindex].disableFastBreakpoints(currentMsg->toolId);
    }
    if (cmd.watchpointMode == FastWatch_Enable)
    {
        processEntry[procindex].enableFastWatchpoints(currentMsg->toolId);
    }
    else if (cmd.watchpointMode == FastWatch_Disable)
    {
        processEntry[procindex].disableFastWatchpoints(currentMsg->toolId);
    }
    ackdesc.returnCode = CmdSuccess;
    TRACE( TRACE_Toolctl, ("(I) ToolControl::setPreferences. dacMode:%d brk mode:%d watch mode:%d\n", cmd.dacMode, cmd.breakpointMode, cmd.watchpointMode) );
    return;
}

void ToolControl::getPreferences(GetPreferencesCmd& cmd,  CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(GetPreferencesAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    GetPreferencesAckCmd& ackcmd = *((GetPreferencesAckCmd *)msgarea);
    ackdesc.length = sizeof(GetPreferencesAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command

    if (breakpointController().isTrapAfterDAC())
    {
        ackcmd.dacMode = TrapAfterDAC;
    }
    else
    {
        ackcmd.dacMode = TrapOnDAC;
    }
    int procindex = kthread->pAppProc->ProcessLeader_ProcessorID;
    if (processEntry[procindex].isFastBreakpointEnabled(currentMsg->toolId))
    {
        ackcmd.breakpointMode = FastBreak_Enable;
    }
    else
    {
        ackcmd.breakpointMode = FastBreak_Disable;
    }
    if (processEntry[procindex].isFastWatchpointEnabled(currentMsg->toolId))
    {
        ackcmd.watchpointMode = FastWatch_Enable;
    }
    else
    {
        ackcmd.watchpointMode = FastWatch_Disable;
    }
    ackdesc.returnCode = CmdSuccess;
    TRACE( TRACE_Toolctl, ("(I) ToolControl::getPreferences.\n") );
    return;
}

// Get list of persistent files
void ToolControl::getFilenames(GetFilenamesCmd& cmd,  CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(GetFilenamesAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    GetFilenamesAckCmd& ackcmd = *((GetFilenamesAckCmd *)msgarea);
    ackdesc.length = sizeof(GetFilenamesAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }

    // Execute the requested command
    ackcmd.numFiles = 0;
    virtFS* persistFS = File_GetFSPtrFromType(FD_PERSIST_MEM);
    persistFS->getFilenames(ackcmd.numFiles, ackcmd.pathname);
    
    ackdesc.returnCode = CmdSuccess;
    
    TRACE( TRACE_Toolctl, ("(I) ToolControl::getFilenames\n"));
    return;
}

// Stat persistent file
void ToolControl::getFileStatData(GetFileStatDataCmd& cmd,  CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(GetFileStatDataAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    GetFileStatDataAckCmd& ackcmd = *((GetFileStatDataAckCmd *)msgarea);
    ackdesc.length = sizeof(GetFileStatDataAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }

    // Execute the requested command
    virtFS* persistFS = File_GetFSPtrFromType(FD_PERSIST_MEM);
    uint64_t cnkrc = persistFS->stat64(cmd.pathname, &(ackcmd.stat));
    ackdesc.returnCode = cnkrc ? CmdFileNotFound : CmdSuccess;

    TRACE( TRACE_Toolctl, ("(I) ToolControl::getStatData\n"));
    return;
}

// Read persistent file
void ToolControl::getFileContents(GetFileContentsCmd& cmd,  CommandDescriptor& ackdesc)
{

    uint32_t modifiedAckCmdSize = ((cmd.numbytes>MaxMemorySize) ? sizeof(GetFileContentsAckCmd) : (sizeof(GetFileContentsAckCmd) + cmd.numbytes));

    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(modifiedAckCmdSize);
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    GetFileContentsAckCmd& ackcmd = *((GetFileContentsAckCmd *)msgarea);
    ackdesc.length = modifiedAckCmdSize;
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    if (cmd.numbytes > MaxMemorySize)
    {
        ackdesc.returnCode = CmdLngthErr;
        return;
    }
    // Execute the requested command
    virtFS* persistFS = File_GetFSPtrFromType(FD_PERSIST_MEM);
    
    ackcmd.numbytes = cmd.numbytes;
    uint64_t cnkrc = persistFS->getFileContents(cmd.pathname, cmd.offset, ackcmd.numbytes, ackcmd.data);
    if (CNK_RC_IS_FAILURE(cnkrc))
    {
        int rc_errno = CNK_RC_ERRNO(cnkrc);
        switch (rc_errno)
        {
        case ENOENT:
            ackdesc.returnCode = CmdFileNotFound;
            break;
        default:
            ackdesc.returnCode = CmdParmErr;
        }
    }
    else
    {
        ackdesc.returnCode = CmdSuccess;
    }
    
    TRACE( TRACE_Toolctl, ("(I) ToolControl::getFileContents\n"));
    return;
}

void ToolControl::releaseControl(ReleaseControlCmd& cmd,  CommandDescriptor& ackdesc)
{

    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(ReleaseControlAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    ReleaseControlAckCmd& ackcmd = *((ReleaseControlAckCmd *)msgarea);
    ackdesc.length = sizeof(ReleaseControlAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }

    // Execute the requested command

    AppProcess_t *proc = getProcessFromMessage();
    int procindex = proc->ProcessLeader_ProcessorID;

    // Lock the tool table
    Kernel_Lock(&_toolTableLock);

    if (processEntry[procindex].controllingToolId() != currentMsg->toolId)
    {
        Kernel_Unlock(&_toolTableLock);
        ackdesc.returnCode = CmdParmErr;
        return;
    }
    // Process any pending Notifications that may be waiting 

    // Are there any unsatisfied notifications?
    if (proc->NotifyControlMask.atom)
    {
        ContinueProcessRC rc = continueProcess(kthread);
        if (rc == ContinueProcessRC_InvalidThread)
        {
            Kernel_Unlock(&_toolTableLock);
            ackdesc.returnCode = CmdTIDinval;
            return;
        }
        else if (rc == ContinueProcessRC_PendingNotify)
        {
            // There is still a pending notification that must be processed by the tool. 
            Kernel_Unlock(&_toolTableLock);
            ackdesc.returnCode = CmdPendingNotify;
            return;
        }
    }
    processEntry[procindex].releaseControlAuthority(currentMsg->toolId);
    // Find the highest priority conflicting toolid and reset the condition
    uint32_t conflictingToolId = processEntry[procindex].findConflictingTool();

    // If we have a conflicting toolid, we must send it a Notify message indicating that control has been released
    if ((ackdesc.returnCode == CmdSuccess) &&
        (conflictingToolId && (conflictingToolId != currentMsg->toolId)))
    {
        pendingNotifyControl.toolID_1 = conflictingToolId;
        pendingNotifyControl.toolID_2 = currentMsg->toolId;
        pendingNotifyControl.toolPriority = processEntry[procindex].toolPriority(currentMsg->toolId);
        pendingNotifyControl.notifyControlReason = NotifyControl_Available;
        pendingNotifyControl.jobid = currentMsg->header.jobId;
        pendingNotifyControl.rank = currentMsg->header.rank;
        pendingNotifyControl.process = proc;
        // Send a message to the highest priority tool that previously conflicted with this tool
        strncpy(pendingNotifyControl.toolTag, processEntry[procindex].toolTag(currentMsg->toolId), ToolTagSize);
        pendingNotifyControl.resumeAllThreads = (proc->pendingAction == ToolPendingAction_ResumeProc) ? 1 : 0; 
        proc->pendingAction = ToolPendingAction_NotifyAvail;
    }

    // Reset the indicator within the process object that migrates watchpoints to child threads. Since no tool is in control,
    // we do not want clone to incur this unnecessary overhead
    proc->propogateWatchpoints = 0;

    // If the calling tool wishes to be notified when the next tool releases control, set the indicator to allow this to happen
    if (cmd.notify == ReleaseControlNotify_Active)
    {
        processEntry[procindex].setConflict(currentMsg->toolId);
    }
    // Unlock the tool table
    Kernel_Unlock(&_toolTableLock);

    TRACE( TRACE_Toolctl, ("(I) ToolControl::releaseControl\n") );
}

void ToolControl::setContinuationSignal(SetContinuationSignalCmd& cmd, CommandDescriptor& ackdesc)
{
    // Allocate space in the ACK message area to hold the requested data
    void *msgarea = allocateOutbound(sizeof(SetContinuationSignalAckCmd));
    if (!msgarea)
    {
        // Could not allocate the necessary space in the outbound message area.
        ackdesc.returnCode = CmdAllocErr;
        return;
    }
    SetContinuationSignalAckCmd& ackcmd = *((SetContinuationSignalAckCmd *)msgarea);
    ackdesc.length = sizeof(SetContinuationSignalAckCmd);
    ackdesc.offset = (uint64_t)&ackcmd - (uint64_t)_outMessageRegion.addr;
    ackcmd.threadID = cmd.threadID;

    // Get the kthread associated with the specified TID
    KThread_t* kthread = GetKThreadFromTid(cmd.threadID);
    if (!kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    AppProcess_t *proc = kthread->pAppProc;
    // If the last notify message was for a different thread then indicate that the TID was invalid.  
    if (proc->notifiedKThread != kthread)
    {
        // TID was invalid. 
        ackdesc.returnCode = CmdTIDinval;
        return;
    }
    // Execute the requested command
    ackdesc.returnCode = CmdSuccess;
    // Set the new signal
    proc->continuationSignal = cmd.signum;
}


int ToolControl::isToolAttached()
{
    AppProcess_t *proc = getProcessFromMessage();
    if (proc)
    {
        return (processEntry[proc->ProcessLeader_ProcessorID].isAttached(currentMsg->toolId));
    }
    return 0;
}

//----------------------------------------------------------------------------
// Wrapper functions for interfacing C++ with C 
//----------------------------------------------------------------------------
//
// Interface from the C IPI handler to the C++ ToolControl classes 
int Tool_ProcessCommands(ToolIpiRequest* toolIpiRequest)
{
    return (toolControl.processCommands(*toolIpiRequest));
}

// Determine if a tool needs to be notified of the occurrence of a signal
int Tool_IsNotifyRequired(KThread_t *kthread)
{
    AppProcess_t *proc = kthread->pAppProc;
    int signal = kthread->SigInfoSigno;
    int si_code = kthread->SigInfoCode;
    Regs_t *context = &kthread->Reg_State;

    int procindex = proc->ProcessLeader_ProcessorID;
    // Are there any updater tools attached to this node
    int toolid = toolControl.processEntry[procindex].controllingToolId();
    if (toolid) // if no updater present, this will be zero.
    {
        // Updater tool is attached. 
        if (kthread->Reg_State.dbsr)
        {
            // Assume the tool is responsible for the non-zero DBSR.
            // Clear active bits in the dbsr by writing the saved value from the register context
            mtspr(SPRN_DBSR,kthread->Reg_State.dbsr);
            isync();
        }

        TRACE( TRACE_Toolctl, ("(I) checking if tool %d is interested in signal %d\n", toolid, signal) );
        // Is this a Fast Breakpoint? If so, we bypass the notify
        if ((signal == SIGTRAP) && 
            (si_code == TRAP_BRKPT) &&
            (toolControl.processEntry[procindex].isFastBreakpointEnabled(toolid)))
        {
            // Determine if the trap instruction is the fast-trap variety
            uint32_t trap_inst = *((uint32_t*)kthread->Reg_State.ip);
            if (TOOL_ISFASTTRAP(trap_inst))
            {
                // This is a fast breakpoint. Do not notify the tool
                return 0;
            }
        }
        //printf("Is Notify Required. Signal=%d si_code=%d dbsr=%016lx FastWatchEnable=%d\n", signal, si_code, context->dbsr, toolControl.processEntry[procindex].isFastWatchpointEnabled(toolid));
        // Is this a Fast Watchpoint? If so, we bypass the notify
        if ((signal == SIGTRAP) &&
            (si_code == TRAP_HWBKPT) && // Watchpoints reported as TRAP_HWBKPT coming through the DEBUG interrupt vector
            (context->dbsr & TOOL_DBSR_DAC_MATCH) &&
            (toolControl.processEntry[procindex].isFastWatchpointEnabled(toolid)))
        {
            // This is a fast watchpoint. Bypass notification.
            return 0;
        }
        // Is this tool interested in the specfied signal
        return(toolControl.processEntry[procindex].signalMonitored(toolid, signal));
    }
    return 0;
}

// C wrapper for handling a signal while attached to a tool
int Tool_SignalEncountered(KThread_t *kthread)
{
    // Find the tool that should be notified
    int signum = kthread->SigInfoSigno;
    TRACE( TRACE_Toolctl, ("(I) signal %d encountered by thread \n", signum) );
    if (signum)
    {
        int toolId = toolControl.findToolToNotify(kthread, signum);
        if (toolId)
        {
            AppProcess_t *proc = kthread->pAppProc;
            if (proc)
            {
                // We are entering a pending Notification situation. Save our kthread within the hardware thread state.
                toolControl.pendingNotifyKThread[ProcessorID()] = kthread;

                // Determine if we are the first/only thread attempting to notify a tool
                uint64_t prevNotifyMask = fetch_and_or64(&proc->NotifyControlMask, _BN(ProcessorID()));
                if (prevNotifyMask == 0 )
                {   // We obtained Notify control


                    TRACE( TRACE_Toolctl, ("(I) Calling suspendProcess() \n") );

                    // Sets the "suspend" all the kthreads associated with this process, including ourself.
                    toolControl.suspendProcess(proc); 

                    // Send message to the tool indicating the signal encountered condition.
                    toolControl.notify(toolId, kthread, signum);
                    return 0; 
                }
                else
                {   // We did not obtain notify authority. 
                    Kernel_WriteFlightLog(FLIGHTLOG, FL_TOOLDEFER, signum, GetTID(kthread), prevNotifyMask | _BN(ProcessorID()),0 );
                    // Suspend the kthreads on our hardware thread
                    toolControl.suspend(proc);
                }
            }
        }
    }
    return 0; // Return 0 to indicate that no special action is required.
}

// C wrapper for handling a deferred notification
int Tool_DeferredSignalNotification(KThread_t *kthread)
{
    // Find the tool that should be notified
    int signum = kthread->SigInfoSigno;
    TRACE( TRACE_Toolctl, ("(I) deferred signal notification %d\n", signum) );
    if (signum)
    {
        int toolId = toolControl.findToolToNotify(kthread, signum);
        if (toolId)
        {
            // Send message to the tool indicating the signal encountered condition.
            toolControl.notify(toolId, kthread, signum);
        }
    }
    return 0; 
}


// Called when a Trap exception occurs 
int Tool_TrapHandler(Regs_t *context)
{
    // We are here due to a debug exception generated due to trap instruction occurring
    int result = toolControl.breakpointController().trapHandler(context);
    return result;
}

// Called when an ICMP interrupt is being handled and MSR[DE]=1, MSR[PR]=1
int Tool_StepHandler(Regs_t *context)
{
    int result = toolControl.breakpointController().stepHandler(context);

    return result;
}


uint64_t Tool_MigrateWatchpoints(KThread_t* kthread, int new_hwthread, int clone)
{
    uint64_t returnCode = 0;
    toolControl.lockWatchpointManager();
    returnCode = toolControl.watchpointManager().migrateWatchpoints(kthread, new_hwthread, clone);
    toolControl.unlockWatchpointManager();
    return returnCode;
}

int Tool_AppStart(KThread_t* kthread)
{
    AppState_t *app = GetMyAppState();
    AppProcess_t *proc = kthread->pAppProc;
    HWThreadState_t *hwt = GetMyHWThreadState();
    int sendsignal = proc->toolAttachSignal;

    if (proc->toolAttachSignal)
    {
        // If we are dealing with a dynamic app, we must set a stop after ld.so starts main.
        if (app->invokeWithInterpreter)
        {
            // Find the updater tool owning this process
            ToolTable *toolTable = &toolControl.processEntry[proc->ProcessLeader_ProcessorID];
            int toolid = toolTable->controllingToolId();
            if (toolid) // if no updater present, this will be zero.
            {
                // Does the tool want to stop at .dl_start in ld.so  or in ._start of the application
                if (toolTable->isDynamicAppNotifyStart(toolid))
                {
                    // Setup the IAR to cause an interrupt when the first address of the program executes. 
                    // When the IAR interrupt condition occurs, the proc->toolAttachSignal value will be sent 
                    // to the tool.
                    hwt->launchContextFlags.flag.DebugControlRegs = 1; // Enable debug control regs
                    kthread->Reg_State.iac1 = proc->Text_VStart;
                    kthread->Reg_State.iac2 = ~((uint64_t)0xFFFFF); // Match any instruction in the 1M block of storage at the text start
                    mtspr(SPRN_IAC1, kthread->Reg_State.iac1);
                    mtspr(SPRN_IAC2, kthread->Reg_State.iac2);
                    kthread->Reg_State.dbcr0 |= DBCR0_IAC1 | DBCR0_IAC2 | DBCR0_IDM;
                    kthread->Reg_State.dbcr1 &= ~(DBCR1_IAC1US(-1) | DBCR1_IAC1ER(-1) | DBCR1_IAC2US(-1) | DBCR1_IAC2ER(-1)); 
                    kthread->Reg_State.dbcr1 |= (DBCR1_IAC1ER_EFF | DBCR1_IAC1US_USER | DBCR1_IAC2ER_EFF | DBCR1_IAC2US_USER | DBCR1_IAC12M ); 
                    // Turn on MSR[DE] in the context's MSR so that we take the iac interrupt
                    kthread->Reg_State.msr |= MSR_DE;

                    // Do not send the signal at the first instruction in the loader.
                    sendsignal = 0;
                }
            }
        }
    }

    return sendsignal;
}

// C++ wrapper for suspending process threads on the current hardware thread
int Tool_Suspend(AppProcess_t* proc)
{
    return (toolControl.suspend(proc));
}

// C++ wrapper for resume process threads on a the current hardware thread
int Tool_Resume(AppProcess_t* proc)
{
    return (toolControl.resume(proc)); 
}

// C++ wrapper for release process threads on a the current hardware thread
int Tool_Release(AppProcess_t* proc)
{
    return (toolControl.release(proc)); 
}


// C++ wrapper for IPI to send a notify conflict message
int Tool_NotifyControl(ToolIpiNotifyControl *parms)
{
    toolControl.notifyControl(parms->toolID_1, 
                              (NotifyControlReason)parms->notifyControlReason, 
                              parms->toolID_2, parms->toolTag, 
                              parms->toolPriority,
                              parms->jobid,
                              parms->rank);
    if (parms->resumeAllThreads && parms->process)
    {
        toolControl.resumeProcess(parms->process);
    }
    return 0;
}


void Tool_CleanupForSIGKILL(KThread_t *targetThread)
{
    // Remove any pending signal notifications. Clear the path for the SIGKILL to succeed.
    targetThread->SigInfoSigno = 0;               // clear any in-progress signals that may have been left around
    targetThread->ExceptionCode = 0;              // clear any in-progress exception code
    // Is this kthread is the suspended state due to a tool action?
    if (targetThread->State & SCHED_STATE_SUSPEND)
    {
        Sched_Unblock(targetThread, SCHED_STATE_SUSPEND);
    }
    if (targetThread->State & SCHED_STATE_HOLD)
    {
        Sched_Unblock(targetThread, SCHED_STATE_HOLD);
    }
}

