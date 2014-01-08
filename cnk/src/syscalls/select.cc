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
#include <sys/select.h>

//! \brief  Run the select system call.
//! \param  r3,r4,...,r8 Syscall parameters.
//! \return CNK_RC_SUCCESS(0) or CNK_RC_FAILURE(errno).

uint64_t sc__newselect(SYSCALL_FCN_ARGS)
{
   int nfds = (int)r3;
   __kernel_fd_set *readfds = (__kernel_fd_set *)r4;
   __kernel_fd_set *writefds = (__kernel_fd_set *)r5;
   __kernel_fd_set *errorfds = (__kernel_fd_set *)r6;
   struct timeval *timeout = (struct timeval *)r7;

   TRACESYSCALL(("(I) %s%s: nfds=%d, readfds=%p, writefds=%p, errorfds=%p, timeout=%p\n",
                 __func__, whoami(), nfds, readfds, writefds, errorfds, timeout ));

   // Check for error conditions.
   if ( (nfds < 0) || (nfds > GetMyProcess()->App_Descriptors.maxfds) )
   {
      CNK_RC_FAILURE(EINVAL);
   }
   if ( (readfds != NULL) && (!VMM_IsAppAddress(readfds, sizeof(*readfds))) )
   {
      CNK_RC_FAILURE(EFAULT);
   }
   if ( (writefds != NULL) && (!VMM_IsAppAddress(writefds, sizeof(*writefds))) )
   {
      CNK_RC_FAILURE(EFAULT);
   }
   if ( (errorfds != NULL) && (!VMM_IsAppAddress(errorfds, sizeof(*errorfds))) )
   {
      CNK_RC_FAILURE(EFAULT);
   }
   if ( (timeout != NULL) && (!VMM_IsAppAddress(timeout, sizeof(*timeout))) )
   {
      CNK_RC_FAILURE(EFAULT);
   }

   // If select() is called with only a timeout parameter, perform the timeout directly without using a file system.
   if ((nfds == 0) && (readfds == NULL) && (writefds == NULL) && (errorfds == NULL) && (timeout != NULL))
   {
      // Convert struct timeval to struct timespec.
      struct timespec req;
      req.tv_sec = timeout->tv_sec;
      req.tv_nsec = timeout->tv_usec * 1000;
      Timer_NanoSleep(&req, NULL);
      return CNK_RC_SUCCESS(0);
   }

   return CNK_RC_FAILURE(ENOSYS);
}
