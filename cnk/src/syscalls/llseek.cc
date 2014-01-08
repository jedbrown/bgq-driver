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

//! \brief  Run the llseek system call.
//! \param  r3,r4,...,r8 Syscall parameters.
//! \return CNK_RC_SUCCESS(0) or CNK_RC_FAILURE(errno).

uint64_t sc__llseek(SYSCALL_FCN_ARGS)
{
   int fd  = r3;
   off64_t offset = ((off64_t)(r4) << 32) | (r5 );
   off64_t *result = (off64_t *) r6;
   int whence = r7;

   TRACESYSCALL(("(I) %s%s: fd=%d, offset=0x%016lx, result=0x%016lx, whence=%d\n", 
                 __func__, whoami(), fd, offset, result, whence ));

   // Make sure the descriptor is valid.
   if (!File_IsFDAllocated(fd))
   {
       return CNK_RC_FAILURE(EBADF);
   }

   // Check for error conditions.
   if (!VMM_IsAppAddress(result, sizeof(*result)))
   {
       return CNK_RC_FAILURE(EFAULT);
   }

   // Run file system's llseek() method.
   return File_GetFSPtr(fd)->llseek(fd, offset, result, whence);
}
