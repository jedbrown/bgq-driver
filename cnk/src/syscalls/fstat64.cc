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
#include "fs/virtFS.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

//! \brief  Run the fstat64 system call.
//! \param  r3,r4,...,r8 Syscall parameters.
//! \return CNK_RC_SUCCESS(0) or CNK_RC_FAILURE(errno).

uint64_t sc_fstat64( SYSCALL_FCN_ARGS )
{
    int fd  = r3;
    struct stat64 *buf = (struct stat64 *)r4;

    TRACESYSCALL(("(I) %s%s: fd=%d, buf=0x%p\n", __func__, whoami(), fd, buf));

    // Make sure the descriptor is valid.
    if (!File_IsFDAllocated(fd))
    {
        return CNK_RC_FAILURE(EBADF);
    }

    // Check for error conditions.
    if (!VMM_IsAppAddress(buf, sizeof(*buf)))
    {
        return CNK_RC_FAILURE(EFAULT);
    }

    // Run file system's fstat64() method.
    return File_GetFSPtr(fd)->fstat64(fd, buf);
}
