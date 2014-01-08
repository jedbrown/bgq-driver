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
#include <spi/include/kernel/debug.h>

uint64_t sc_SETDEBUGREG(SYSCALL_FCN_ARGS)
{
    int reg   = (int)r3;
    uint64_t value  = (uint64_t)r4;
    AppState_t *app = GetMyAppState();
    AppProcess_t *proc = GetMyProcess();
    KThread_t *kthr = GetMyKThread();

    if (!proc->DebugRegSyscallsEnabled)
    {
	// syscall has to be explicitly enabled by environment variable
        return CNK_RC_SPI(ENOSYS);
    }

    if (app->Active_Processes > 16)
    {
	// process does not own the whole core
        return CNK_RC_SPI(EPERM);
    }

    if (proc->toolAttached)
    {
	// CDTI-based tools take precedence in use of the debug registers
        return CNK_RC_SPI(EPERM);
    }

    switch (reg)
    {
        case Kernel_Debug_DAC1:  // CORE SCOPED
            mtspr(SPRN_DAC1, value);
            break;
        case Kernel_Debug_DAC2:  // CORE SCOPED
            mtspr(SPRN_DAC2, value);
            break;
        case Kernel_Debug_DAC3:  // CORE SCOPED
            mtspr(SPRN_DAC3, value);
            break;
        case Kernel_Debug_DAC4:  // CORE SCOPED
            mtspr(SPRN_DAC4, value);
            break;
        case Kernel_Debug_IAC1:  // CORE SCOPED
            mtspr(SPRN_IAC1, value);
            break;
        case Kernel_Debug_IAC2:  // CORE SCOPED
            mtspr(SPRN_IAC2, value);
            break;
        case Kernel_Debug_IAC3:  // CORE SCOPED
            mtspr(SPRN_IAC3, value);
            break;
        case Kernel_Debug_IAC4:  // CORE SCOPED
            mtspr(SPRN_IAC4, value);
            break;
        case Kernel_Debug_DBCR0:
            mtspr(SPRN_DBCR0, value);
            break;
        case Kernel_Debug_DBCR1:
            mtspr(SPRN_DBCR1, value);
            break;
        case Kernel_Debug_DBCR2:
            mtspr(SPRN_DBCR2, value);
            break;
        case Kernel_Debug_DBCR3:
            mtspr(SPRN_DBCR3, value);
            break;
        case Kernel_Debug_DVC1:  // CORE SCOPED
            mtspr(SPRN_DVC1, value);
            break;
        case Kernel_Debug_DVC2:  // CORE SCOPED
            mtspr(SPRN_DVC2, value);
            break;
        case Kernel_Debug_DBSR:  
            mtspr(SPRN_DBSR, value);
            break;
        case Kernel_Debug_MSRDE: 
            if (value)
                kthr->Reg_State.msr |= MSR_DE;
            else
                kthr->Reg_State.msr &= ~MSR_DE;
            break;
        default:
            return CNK_RC_SPI(EINVAL);
    }
    ppc_msync();

    return CNK_RC_SPI(0);
}

uint64_t sc_GETDEBUGREG(SYSCALL_FCN_ARGS)
{
    int reg = (int) r3;
    uint64_t *value_ptr = (uint64_t *) r4;
    AppState_t *app = GetMyAppState();
    AppProcess_t *proc = GetMyProcess();
    KThread_t *kthr = GetMyKThread();

    if (!proc->DebugRegSyscallsEnabled)
    {
	// syscall has to be explicitly enabled by environment variable
        return CNK_RC_SPI(ENOSYS);
    }

    if (app->Active_Processes > 16)
    {
	// process does not own the whole core
        return CNK_RC_SPI(EPERM);
    }

    if (proc->toolAttached)
    {
	// CDTI-based tools take precedence in use of the debug registers
        return CNK_RC_SPI(EPERM);
    }

    if (!VMM_IsAppAddress(value_ptr, sizeof(*value_ptr)))
    {
	return CNK_RC_SPI(EFAULT);
    }

    switch (reg)
    {
        case Kernel_Debug_DAC1:  // CORE SCOPED
            (*value_ptr) = mfspr(SPRN_DAC1);
            break;
        case Kernel_Debug_DAC2:  // CORE SCOPED
            (*value_ptr) = mfspr(SPRN_DAC2);
            break;
        case Kernel_Debug_DAC3:  // CORE SCOPED
            (*value_ptr) = mfspr(SPRN_DAC3);
            break;
        case Kernel_Debug_DAC4:  // CORE SCOPED
            (*value_ptr) = mfspr(SPRN_DAC4);
            break;
        case Kernel_Debug_IAC1:  // CORE SCOPED
            (*value_ptr) = mfspr(SPRN_IAC1);
            break;
        case Kernel_Debug_IAC2:  // CORE SCOPED
            (*value_ptr) = mfspr(SPRN_IAC2);
            break;
        case Kernel_Debug_IAC3:  // CORE SCOPED
            (*value_ptr) = mfspr(SPRN_IAC3);
            break;
        case Kernel_Debug_IAC4:  // CORE SCOPED
            (*value_ptr) = mfspr(SPRN_IAC4);
            break;
        case Kernel_Debug_DBCR0:
            (*value_ptr) = mfspr(SPRN_DBCR0);
            break;
        case Kernel_Debug_DBCR1:
            (*value_ptr) = mfspr(SPRN_DBCR1);
            break;
        case Kernel_Debug_DBCR2:
            (*value_ptr) = mfspr(SPRN_DBCR2);
            break;
        case Kernel_Debug_DBCR3:
            (*value_ptr) = mfspr(SPRN_DBCR3);
            break;
        case Kernel_Debug_DVC1:  // CORE SCOPED
            (*value_ptr) = mfspr(SPRN_DVC1);
            break;
        case Kernel_Debug_DVC2:  // CORE SCOPED
            (*value_ptr) = mfspr(SPRN_DVC2);
            break;
        case Kernel_Debug_DBSR:
            (*value_ptr) = mfspr(SPRN_DBSR);
            break;
        case Kernel_Debug_MSRDE:
	    (*value_ptr) = ((kthr->Reg_State.msr & MSR_DE) != 0);
            break;
        default:
            return CNK_RC_SPI(EINVAL);
    }

    return CNK_RC_SPI(0);
}
