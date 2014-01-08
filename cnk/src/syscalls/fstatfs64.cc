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
#include <sys/statfs.h>
#include "fs/virtFS.h"

//! \brief  Run the fstatfs64 system call.
//! \param  r3,r4,...,r8 Syscall parameters.
//! \return CNK_RC_SUCCESS(0) or CNK_RC_FAILURE(errno).

uint64_t sc_fstatfs64( SYSCALL_FCN_ARGS)
{
   int fd = r3;
   int buflen = r4;
   struct statfs64 *buf = (struct statfs64 *)r5;

   TRACESYSCALL(("(I) %s%s: fd=%d, buf=0x%p\n", __func__, whoami(), fd, buf));

   // Make sure the descriptor is valid.
   if (!File_IsFDAllocated(fd)) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Check for error conditions.
   if (buflen < (int)sizeof(struct statfs64)) {
      return CNK_RC_FAILURE(EINVAL);
   }

   if (!VMM_IsAppAddress(buf, buflen)) {
      return CNK_RC_FAILURE(EFAULT);
   }

   // Run file system's fstatfs64() method.
   return File_GetFSPtr(fd)->fstatfs64(fd, buf);
}
