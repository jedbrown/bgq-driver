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

uint64_t sc_sched_setscheduler(SYSCALL_FCN_ARGS)
{
    int  tid   = (int)r3;
    unsigned policy_new = (int)r4;
    KThread_t *targetKThread = NULL;
    struct sched_param *pSchedParam = (struct sched_param *)r5;
    int priority_new = pSchedParam->sched_priority;

    TRACESYSCALL(("(I) %s[%d]: tid %d policy %d sched_param %d\n", __func__, ProcessorID(), tid, policy_new, pSchedParam->sched_priority));

    // Get the Kthread associated with this tid. Function will return a NULL if this is an invalid tid
    targetKThread = GetKThreadFromTid(tid);

    // Did we find a valid KThread
    if (!targetKThread)
    {
        return CNK_RC_FAILURE(ESRCH); // no corresponding tid in this process was found
    }
    // Make adjustments if the SCHED_OTHER policy was specified since this maps to SCHED_FIFO in our environment
    if (policy_new == SCHED_OTHER)
    {
        policy_new = SCHED_FIFO;
        if (priority_new == 0)
        {
            priority_new = CONFIG_SCHED_MIN_FIFO_PRIORITY;
        }
    }
    if ((policy_new == SCHED_FIFO) &&
        ((priority_new < CONFIG_SCHED_MIN_FIFO_PRIORITY) || 
         (priority_new >  CONFIG_SCHED_MAX_FIFO_PRIORITY)))
    {
        return CNK_RC_FAILURE(EINVAL); // invalid policy requested
    }
    if ((policy_new == SCHED_COMM) &&                          // COMM thread?
        ((priority_new < CONFIG_SCHED_MIN_COMM_PRIORITY) ||   // priority less than min allowed?
         (priority_new >  CONFIG_SCHED_MAX_COMM_PRIORITY)))   // priority greater than max allowed?
    {
        return CNK_RC_FAILURE(EINVAL); // invalid policy requested
    }
    // Only allow a limited number of policy transitions to keep it simple. 
    // For example: turning a normal pthread (SCHED_FIFO) into a comm thread (SCHED_COMM) is a one-way trip
    // May need to support additional transitions in the future, especially if we add Remote thread policies
    if (!(((targetKThread->Policy == SCHED_FIFO) && (policy_new == SCHED_COMM)) ||
         (targetKThread->Policy == policy_new)))
    {
        return CNK_RC_FAILURE(EINVAL); // invalid policy requested
    }
    // Validations are complete. We now need to change the policy and parameter if they do not match the current settings

    // Perform the policy change. 
    KThread_ChangePolicy(targetKThread, policy_new, priority_new);

    return CNK_RC_SUCCESS(0);
}
