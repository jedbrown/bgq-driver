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
#include <sys/prctl.h>

//! \brief  Run the prctl system call.
//! \param  r3,r4,...,r8 Syscall parameters.
//! \return CNK_RC_SUCCESS(0) or CNK_RC_FAILURE(errno).


uint64_t sc_prctl(SYSCALL_FCN_ARGS)
{
    int option = (int) r3;
    uint64_t arg2 = r4;
    uint64_t arg3 = r5;
    uint64_t arg4 = r6;
    uint64_t arg5 = r7;

    TRACESYSCALL(("(I) %s%s: option=%d\n, "
		 "arg2=0x%lx, arg3=0x%lx, arg4=0x%lx, arg5=0x%lx",
		 __func__, ProcessorID(), option, arg2, arg3, arg4, arg5));

    switch (option)
    {
    case PR_SET_FPEXC:
	Regs_t *regs;
	regs = &(GetMyKThread()->Reg_State);
	switch (arg2)
	{
	case PR_FP_EXC_DISABLED:
	    regs->msr = (regs->msr & ~(MSR_FE0 | MSR_FE1));
	    break;
	case PR_FP_EXC_NONRECOV:
	    regs->msr = (regs->msr & ~(MSR_FE0 | MSR_FE1)) | MSR_FE1;
	    break;
	case PR_FP_EXC_ASYNC:
	    regs->msr = (regs->msr & ~(MSR_FE0 | MSR_FE1)) | MSR_FE0;
	    break;
	case PR_FP_EXC_PRECISE:
	    regs->msr = (regs->msr & ~(MSR_FE0 | MSR_FE1)) | MSR_FE0 | MSR_FE1;
	    break;
	default:
	    return CNK_RC_FAILURE(EINVAL);
	}
	break;
    default:
	// We only support PR_SET_FPEXC for now.
	return CNK_RC_FAILURE(ENOSYS);
    }

    return CNK_RC_SUCCESS(0);
}
