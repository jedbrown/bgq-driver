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
#include <fcntl.h>
#include "fs/virtFS.h"

//! \brief  Run the dup2 system call.
//! \param  r3,r4,...,r8 Syscall parameters.
//! \return CNK_RC_SUCCESS(descriptor number) or CNK_RC_FAILURE(errno).

uint64_t sc_dup2(SYSCALL_FCN_ARGS)
{
   int fd = r3;
   int targetFD = r4;

   TRACESYSCALL(("(I) %s%s: fd=%d, fd2=%d\n", __func__, whoami(), fd, targetFD));

   // Make sure the descriptor is valid.
   if (!File_IsFDAllocated(fd)) {
      return CNK_RC_FAILURE(EBADF);
   }

   // Make sure the target descriptor is valid.
   if ( (targetFD < 0) || (targetFD >= GetMyProcess()->App_Descriptors.maxfds) ) {
      return CNK_RC_FAILURE(EBADF);  // NOTE: different error than fcntl F_DUPFD
   }

   // If source and target descriptors are the same don't close file - just return with no error.
   if (fd == targetFD ) {
      return CNK_RC_SUCCESS(fd);
   }

   // Close the existing target descriptor if it is in use.
   if (File_IsFDAllocated(targetFD)) {
       File_GetFSPtr(targetFD)->close(targetFD);
   }

   // Run file system's fcntl() method.
   uint64_t rc = File_GetFSPtr(fd)->fcntl(fd, F_DUPFD, targetFD);

   // There was an error or got descriptor requested - rc has correct value so return.
   if (CNK_RC_IS_FAILURE(rc) || ((int)CNK_RC_VALUE(rc) == targetFD)) {
       return rc;
   } 

   // Operation succeeded but we got a descriptor other than target descriptor.
   else  {
       int wrongFD = CNK_RC_VALUE(rc);
       File_GetFSPtr(wrongFD)->close(wrongFD);  // free unwanted descriptor
       return CNK_RC_FAILURE(EBUSY);   // indicate we lost race to get target descriptor
   }

}

