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
#include "spi/include/kernel/memory.h"

//! \brief  Implement the sc_SETLOCALFSWINDOW system call.
//! \param  r3,r4,...,r8 Syscall parameters.
//! \return CNK_RC_SPI(0) or CNK_RC_SPI(errno). 

uint64_t  sc_SETLOCALFSWINDOW(SYSCALL_FCN_ARGS)
{
    size_t size                 = (size_t)r3;
    void*  buffer               = (void*)r4;
    
    if(VMM_IsAppAddress(buffer, size) == 0)
    {
        return CNK_RC_SPI(EFAULT);
    }
    
    // Get current core and process object
    AppProcess_t* pProc = GetMyProcess();
    pProc->LocalDisk.VStart = (uint64_t)buffer;
    pProc->LocalDisk.Size   = size;
    
    return 0;
}
