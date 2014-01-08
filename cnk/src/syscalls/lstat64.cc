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
#include <sys/stat.h>

//! \brief  Run the lstat64 system call.
//! \param  r3,r4,...,r8 Syscall parameters.
//! \return CNK_RC_SUCCESS(0) or CNK_RC_FAILURE(errno).

uint64_t sc_lstat64(SYSCALL_FCN_ARGS)
{
   char *pathname = (char *)r3;
   struct stat64 *buf = (struct stat64 *)r4;

   // Check for error conditions.
   uint64_t pathlen = validatePathname(pathname);
   if (CNK_RC_IS_FAILURE(pathlen)) {
      return pathlen;
   }

   if (!VMM_IsAppAddress(buf, sizeof(*buf))) {
      return CNK_RC_FAILURE(EFAULT);
   }

   TRACESYSCALL(("(I) %s%s: path=\"%s\", buf=0x%p\n", __func__, whoami(), pathname, buf));

   // Run file system's lstat64() method.
   return File_GetFSPtrFromPath(pathname)->lstat64(pathname, buf);
}
