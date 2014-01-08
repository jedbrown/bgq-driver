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

//! \brief  Function ship the rename system call.
//! \param  r3,r4,...,r8 Syscall parameters.
//! \return CNK_RC_SUCCESS(0) or CNK_RC_FAILURE(errno).

uint64_t sc_rename(SYSCALL_FCN_ARGS)
{
   char *oldpath = (char *)r3;
   char *newpath = (char *)r4;

   // Check for error conditions.
   uint64_t oldlen = validatePathname(oldpath);
   if (CNK_RC_IS_FAILURE(oldlen)) {
      return oldlen;
   }

   uint64_t newlen = validatePathname(newpath);
   if (CNK_RC_IS_FAILURE(newlen)) {
      return newlen;
   }

   TRACESYSCALL(("(I) %s%s: old=\"%s\", new=\"%s\"\n", __func__, whoami(), oldpath, newpath));

   // Run file system's rename() method.
   return File_GetFSPtrFromPath(oldpath)->rename(oldpath, newpath);
}

