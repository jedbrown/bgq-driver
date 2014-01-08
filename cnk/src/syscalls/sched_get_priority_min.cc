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

uint64_t sc_sched_get_priority_min(SYSCALL_FCN_ARGS)
{
    int  sched_policy   = (int)r3;

    TRACESYSCALL(("(I) %s[%d]: policy %d\n", __func__, ProcessorID(), sched_policy));
    
    if (sched_policy == SCHED_FIFO)
    {
        return CNK_RC_SUCCESS(CONFIG_SCHED_MIN_FIFO_PRIORITY);
    }
    else if (sched_policy == SCHED_COMM)
    {
        return CNK_RC_SUCCESS(CONFIG_SCHED_MIN_COMM_PRIORITY);
    }
    else
    {
        return CNK_RC_FAILURE(EINVAL); // a supported scheduling policy was not provided
    }
}

