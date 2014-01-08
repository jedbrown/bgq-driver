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

uint64_t sc_sched_setparam(SYSCALL_FCN_ARGS)
{
    int  tid   = (int)r3;
    sched_param *pSchedParam = (struct sched_param *)r4;
    KThread_t* targetKThread = NULL;

    TRACESYSCALL(("(I) %s[%d]: sched_param ptr %08lx\n", __func__, ProcessorID(), (uint64_t)pSchedParam));

    // Get the Kthread associated with this tid. Function will return a NULL if this is an invalid tid
    targetKThread = GetKThreadFromTid(tid);
    // Did the user supply a pointer to the sched_parm structure?
    if (!pSchedParam)
    {
        return CNK_RC_FAILURE(EINVAL); // no pointer to the param structure was provided
    }
    // Did we find a valid KThread
    if (!targetKThread)
    {
        return CNK_RC_FAILURE(ESRCH); // no corresponding tid in this process was found
    }
    // Is the parameter specified within the min and max ranges allowed?
    if ((targetKThread->Policy == SCHED_FIFO) &&
        ((pSchedParam->sched_priority < CONFIG_SCHED_MIN_FIFO_PRIORITY) || 
        (pSchedParam->sched_priority >  CONFIG_SCHED_MAX_FIFO_PRIORITY)))
    {
        return CNK_RC_FAILURE(EINVAL); // invalid priority specifed for this policy

    }
    // Is the parameter specified within the min and max ranges allowed?
    if ((targetKThread->Policy == SCHED_COMM) &&
        ((pSchedParam->sched_priority < CONFIG_SCHED_MIN_COMM_PRIORITY) || 
        (pSchedParam->sched_priority >  CONFIG_SCHED_MAX_COMM_PRIORITY)))
    {
        return CNK_RC_FAILURE(EINVAL); // invalid priority specifed for this policy

    }

    // Validations are complete. Set the new priority
    KThread_ChangePriority(targetKThread, pSchedParam->sched_priority);

    return 0;
}

