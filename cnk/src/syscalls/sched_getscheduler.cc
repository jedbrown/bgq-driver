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

uint64_t sc_sched_getscheduler(SYSCALL_FCN_ARGS)
{
    int  tid   = (int)r3;
    KThread_t* targetKThread = NULL;
    TRACESYSCALL(("(I) %s[%d]: tid %d\n", __func__, ProcessorID(), tid));
    
    // Get the Kthread associated with this tid. Function will return a NULL if this is an invalid tid
    targetKThread = GetKThreadFromTid(tid);

    // Did we find a valid KThread
    if (!targetKThread)
    {
        return CNK_RC_FAILURE(ESRCH); // no corresponding tid in this process was found
    }
    return CNK_RC_SUCCESS(targetKThread->Policy);
}

