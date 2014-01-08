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

//! \file  dumpthreadstate.cc

// Includes
#include "Kernel.h"

const char yes_string[]  = "Y";
const char no_string[] = "N";
int dumpThreadState()
{
    int core;
    int hwt;
    KThread_t *kthread;
    // Dump the active kthreads in the node.
    for (core = 0; core <  CONFIG_MAX_CORES; core++)
    {
        for (hwt = 0; (hwt < CONFIG_HWTHREADS_PER_CORE); hwt++)
        {
            int kindex;
            for (kindex = 0; kindex < CONFIG_SCHED_KERNEL_SLOT_INDEX; kindex++)
            {
                // get the kthread pointer
                kthread = NodeState.CoreState[core].HWThreads[hwt].SchedSlot[kindex];
                {
                    if (!(kthread->State & SCHED_STATE_FREE))
                    {
                        int curHwt = ((core<<2) + hwt); 
                        int kHwt = kthread->ProcessorID;
                        int curSlot = kindex;
                        int kSlot = kthread->SlotIndex;
                        const char *isMigrated = (kthread != &(NodeState.KThreads[ kindex + (((core<<2) + hwt) * CONFIG_SCHED_SLOTS_PER_HWTHREAD)])) ? yes_string : no_string;
                        const char *isRunning = (NodeState.CoreState[core].HWThreads[hwt].pCurrentThread == kthread) ? yes_string : no_string;
                        const char *isRemote = (GetProcessByProcessorID((core<<2)+hwt) == kthread->pAppProc) ? no_string : yes_string;
                        const char *isMain = (kthread->isProcessLeader) ? yes_string : no_string;
                        const char *isShared = (kthread->FutexIsShared) ? yes_string : no_string;
                        // limit the string printed but keep the fields aligned for viewing
                        printf("TID=%03d State=%04x HWT=%02d Slot=%01d kHWT=%02d kSlot=%01d IAR=%016lx Main=%s Run=%s Moved=%s Rmt=%s Futex=%016lx Shr=%s Sig=%02d Pri=%02d Pend=%02lx Divert=%02lx sc=%04d\n",
                               GetTID(kthread),kthread->State,curHwt,curSlot, kHwt, kSlot, kthread->Reg_State.ip, isMain, isRunning,isMigrated,isRemote,(uint64_t)kthread->FutexVAddr,isShared,kthread->SigInfoSigno,kthread->Priority,kthread->Pending, kthread->SchedulerDivert, (uint16_t)kthread->syscallNum);      
                    }
                }
            }
        }
    }
    return 0;
}
