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
#include <utime.h>

#define MILLION 1000000 

//! \brief  Run the utime system call.
//! \param  r3,r4,...,r8 Syscall parameters.
//! \return CNK_RC_SUCCESS(0) or CNK_RC_FAILURE(errno).

uint64_t sc_utime(SYSCALL_FCN_ARGS)
{
   char *pathname = (char *)r3;
   const struct utimbuf *usrbuf = (const struct utimbuf *)r4;
   struct utimbuf buf;
   const struct utimbuf *bufp;

   // Check for error conditions.
   uint64_t pathlen = validatePathname(pathname);
   if (CNK_RC_IS_FAILURE(pathlen)) {
      return pathlen;
   }

   if (usrbuf != 0) {
      if (!VMM_IsAppAddress(usrbuf, sizeof(*usrbuf))) {
         return CNK_RC_FAILURE(EFAULT);
      }
      bufp = usrbuf;
   }
   else {
      buf.actime = GetCurrentTimeInMicroseconds() / MILLION;
      buf.modtime = buf.actime;
      bufp = &buf;
   }

   TRACESYSCALL(("(I) %s%s: path=\"%s\", utimes=0x%p\n", __func__, whoami(), pathname, usrbuf));

   // Run file system's utime() method.
   return File_GetFSPtrFromPath(pathname)->utime(pathname, bufp);
}

