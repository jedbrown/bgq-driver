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
/* (C) Copyright IBM Corp.  2008, 2012                              */
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
#include <sched.h>

uint64_t sc_sched_setaffinity(SYSCALL_FCN_ARGS)
{
    int  tid   = (int)r3;
    unsigned int cpusetsize = (unsigned int)r4;
    cpu_set_t *cpu_mask = (cpu_set_t*)r5;
    unsigned int AffinityMaskSize = ((CONFIG_MAX_APP_CORES * CONFIG_HWTHREADS_PER_CORE)+7)/8;
    // Get the Kthread associated with this tid. Function will return a NULL if this is an invalid tid
    // printf("set_affinity tid from app: %d\n",pid_or_tid);
    KThread_t* targetKThread = GetKThreadFromTid(tid);
    AppProcess_t *proc = GetMyProcess();

    // Did we find a valid KThread
    if (!targetKThread)
    {
        return CNK_RC_FAILURE(ESRCH); // no corresponding tid in this process was found
    }
    // is this kthread a process leader?
    if  (targetKThread->isProcessLeader)
    {
        //printf("setaffinity EINVAL due to target set to the process leader thread id\n");
        return CNK_RC_FAILURE(EINVAL); // A process leader thread is not a valid target
    }
    if (cpusetsize < AffinityMaskSize)
    {
        //printf("setaffinity EINVAL due to cpusetsize= %d and kernel mask size=%d\n",cpusetsize, AffinityMaskSize);
        return CNK_RC_FAILURE(EINVAL); // No cpu bit was set in the mask
    }
    // Determine the requested target hardware thread
    uint32_t hwthread_index;
    for (hwthread_index=0; hwthread_index < AffinityMaskSize*8; hwthread_index++)
    {
        if (CPU_ISSET(hwthread_index, cpu_mask)) break;
    }
    // Did we find a valid bit on in the cpu mask?
    if (hwthread_index/8 >= AffinityMaskSize)
    {
        //printf("setaffinity EINVAL due to request to set hwthread %d greater than mask size.\n",hwthread_index);
        return CNK_RC_FAILURE(EINVAL); // No cpu bit was set in the mask
    }
    // Determine if the target hardware thread index is a resource owned by this process. 
    if (!((_BN(hwthread_index)) & (GetMyProcess()->HwthreadMask)))
    {
        // The targetted hardware thread is not a thread belonging to this process.
        //printf("setaffinity EINVAL due to request to set hwthread %d not belonging to this process\n",hwthread_index);
        return CNK_RC_FAILURE(EINVAL); // No cpu bit was set in the mask
    }
    // Is the target kthread already assigned to the target hardware thread?
    if ((uint32_t)(targetKThread->ProcessorID) == hwthread_index)
    {
        // Unfortunately we need to do some un-natural things here to deal with comm threads.
        if (proc->HWThread_Count == 2)
        {
            // Reinitialize the layout counter at its starting position so that a user pthread can be placed
            // on top of a comm thread before it is placed on the main thread.
            proc->HwtRecycleListMgr.threadLayout_count = 1;
        }
       // Nothing more to do.
        return CNK_RC_SUCCESS(0);
    }
    // See if we can reserve a kthread on the target hardware thread. If we can, there should be nothing stopping us from proceeding with the migration
    int reserved_slot = KThread_ReserveForMigration(hwthread_index);
    // if no slot was available on the target hardware thread, fail the request.
    if (reserved_slot < 0)
    {
        return CNK_RC_FAILURE(EINVAL); 
    }
    // Set the pending migration information in the kthread (target hwt and slot) (compare swap, of fails assume some other thread is
    // moving this kthread.
    if (KThread_SetMigrationTarget(targetKThread, hwthread_index, reserved_slot) < 0)
    {
        KThread_ReleaseForMigration(hwthread_index,reserved_slot);
        return CNK_RC_FAILURE(EINVAL); 
    }

    // There is no turning back now. Request that the thread be moved.

    //    Send the IPI to the target kthread to initiate the move
    int targetProcessorID = targetKThread->ProcessorID;
    if (ProcessorID() == targetProcessorID)
    {
        KThread_MoveToHwThread(targetKThread);
    }
    else
    {
        IPI_setaffinity(targetKThread);
    }
    return CNK_RC_SUCCESS(0);
}

