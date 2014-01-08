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
/* (C) Copyright IBM Corp.  2007, 2012                              */
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


// Includes                                                                                                                                           
#include "Kernel.h"
#include "spi/include/kernel/memory.h"

//! \brief  Implement the sc_GETMEMORYREGION system call.
//! \param  r3,r4,...,r8 Syscall parameters.
//! \return CNK_RC_SPI(0) or CNK_RC_SPI(errno). 

uint64_t  sc_GETMEMORYSIZE(SYSCALL_FCN_ARGS)
{
    enum KERNEL_MEMSIZETYPE type = (enum KERNEL_MEMSIZETYPE)r3;
    size_t* size                 = (size_t*)r4;
    
    if(VMM_IsAppAddress(size, sizeof(uint64_t)) == 0)
    {
        return CNK_RC_SPI(EFAULT);
    }
    
    // Get current core and process object
    int processorID = ProcessorID();
    AppProcess_t* pProc = GetProcessByProcessorID(processorID);
    HWThreadState_t *pProcLeader_HwThState = GetHWThreadStateByProcessorID(pProc->ProcessLeader_ProcessorID);
    KThread_t *ProcLeaderKthread = pProcLeader_HwThState->SchedSlot[CONFIG_SCHED_SLOT_FIRST];
    Regs_t *firstAppThrRegs =  &(ProcLeaderKthread->Reg_State);  // process leader is always in the first slot
    
    switch(type)
    {
        case KERNEL_MEMSIZE_SHARED:
            *size = NodeState.SharedMemory.Size;
            break;
        case KERNEL_MEMSIZE_PERSIST:
            *size = NodeState.PersistentMemory.Size;
            break;
        case KERNEL_MEMSIZE_HEAP:
            *size = pProc->MmapMgr.high_mark - pProc->Heap_VStart;
            break;
        case KERNEL_MEMSIZE_HEAPMAX:
            *size = pProc->MmapMgr.high_mark_max - pProc->Heap_VStart;
            break;
        case KERNEL_MEMSIZE_HEAPAVAIL:
            if(!IsProcessLeader())
            {
                return CNK_RC_SPI(EINVAL);
            }
            /*fallthru*/
        case KERNEL_MEMSIZE_ESTHEAPAVAIL:
            *size = firstAppThrRegs->gpr[1] - pProc->MmapMgr.high_mark;
            break;
        case KERNEL_MEMSIZE_STACKAVAIL:
            if(!IsProcessLeader())
            {
                return CNK_RC_SPI(EINVAL);
            }
            /*fallthru*/
        case KERNEL_MEMSIZE_ESTSTACKAVAIL:
            *size = firstAppThrRegs->gpr[1] - pProc->MmapMgr.high_mark;
            break;
        case KERNEL_MEMSIZE_STACK:
            if(!IsProcessLeader())
            {
                return CNK_RC_SPI(EINVAL);
            }
            /*fallthru*/
        case KERNEL_MEMSIZE_ESTSTACK:
            *size = pProc->Heap_VEnd - firstAppThrRegs->gpr[1];
            break;
        case KERNEL_MEMSIZE_GUARD:
            *size = pProc->Guard_Size;
            break;
        case KERNEL_MEMSIZE_MMAP:
            *size = pProc->MmapMgr.mem_busy;     
            break;
        default:
            return CNK_RC_SPI(EINVAL);
    }
    return 0;
}
