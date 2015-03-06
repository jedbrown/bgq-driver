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


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/futex.h>
#include "Kernel.h"

uint64_t sc_futex( SYSCALL_FCN_ARGS )
{
    uint64_t current_time = GetTimeBase();
    KThread_t *pKThr = GetMyKThread();
    Futex_t* ft_uaddr  = (Futex_t*)r3;
    uint32_t ft_op_and_flags = r4;
    // allow the private flag to be set since our implementation is already (process) private 
    uint32_t ft_op = ft_op_and_flags & FUTEX_CMD_MASK;
    int32_t ft_val     = r5;
    const struct timespec* ft_timeout = (const struct timespec*)r6;
    int32_t max2wake = (int32_t)r6;
    Futex_t* ft_uaddr2 = (Futex_t*) r7;
    int32_t ft_val3     = (int32_t)r8;
    uint64_t futex_timeout;

    int processor_id = ProcessorID();
    TRACESYSCALL(
                ("(I) %s[%d]: op(%d)=%s, uaddr=%p, val=%d timeout=%p *uaddr=%d uaddr2=0x%08x val3=0x%08x.\n",
                 __func__, processor_id, ft_op_and_flags,
                 (ft_op == FUTEX_WAIT ? "WAIT" : (ft_op == FUTEX_WAKE ? "WAKE" : "Other")),
                 ft_uaddr, ft_val, ft_timeout, *ft_uaddr, ft_uaddr2, ft_val3 ));
    if ( !VMM_IsAppAddress( (const void*)((Futex_t*)ft_uaddr), sizeof(Futex_t) ) )
    {
        return CNK_RC_FAILURE(EFAULT);
    }

    switch ( ft_op )
    {
        case FUTEX_WAIT:
            if (ft_timeout)
            {
                if (!VMM_IsAppAddress(ft_timeout, sizeof(const struct timespec)))
                {
                    return CNK_RC_FAILURE(EFAULT);
                }

                /* strace output in Linux reveals that timeout param. is relative time, not absolute.  Convert to abs. for scheduler */
                futex_timeout = ((uint64_t)ft_timeout->tv_sec * 1000000 + (uint64_t)ft_timeout->tv_nsec/1000) * GetPersonality()->Kernel_Config.FreqMHz + current_time;
	 }
            else
            {
                futex_timeout = 0;
            }

            //printf("[%d] before: futex@%p = %d\n", core, ft_uaddr, *((int*)ft_uaddr));
            return Futex_Wait( ft_op_and_flags, pKThr, ft_uaddr, ft_val, futex_timeout, current_time );
            break;

        case FUTEX_WAKE: // val is max number of threads to wake
            return Futex_Wake( ft_op_and_flags, ft_uaddr, ft_val, 0,  -1, -1);
            break;

        case FUTEX_WAKE_OP:
            return Futex_Wake( ft_op_and_flags, ft_uaddr, ft_val, ft_uaddr2, max2wake, ft_val3);
            break;

        case FUTEX_REQUEUE:
        case FUTEX_CMP_REQUEUE:
            return Futex_Wake( ft_op_and_flags, ft_uaddr, ft_val, ft_uaddr2, -1, ft_val3);
            break;

        default:
            //printf("(W) %s[%d]: Unknown futex op: %d\n", __func__, core, ft_op);
            break;
    }

    return CNK_RC_FAILURE(ENOSYS);
}
