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
#include "spi/include/kernel/process.h"

//! \brief  Implement the sc_ISGUESTTHREAD system call.
//! \param  r3,r4,...,r8 Syscall parameters.
//! \return CNK_RC_SPI(0) or CNK_RC_SPI(1). 

uint64_t  sc_ISGUESTTHREAD(SYSCALL_FCN_ARGS)
{
    AppProcess_t *proc = GetMyProcess();
    if (_BN(ProcessorID()) & proc->HwthreadMask)
    {
        return 0; // This software thread is executing on a local hardware thread.
    }
    return 1; // This software thread is not executing on a local hardware thread.
}
