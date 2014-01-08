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

//! \brief  Run the fcntl64 system call.
//! \param  r3,r4,...,r8 Syscall parameters.
//! \return CNK_RC_SUCCESS(descriptor number) or CNK_RC_FAILURE(errno).

uint64_t sc_fcntl64(SYSCALL_FCN_ARGS)
{
   int fd = r3;
   int cmd = r4;
   uint64_t parm3 = r5;

   TRACESYSCALL(("(I) %s%s: fd=%d, cmd=%d, parm3=0x%lx\n", __func__, whoami(), fd, cmd, parm3));

   //! \todo It looks like fcntl64 is deprecated on 64-bit systems.  For now, print a message.  Should we remove this code later?
   printf("(E) fcntl64 is deprecated, use fcntl instead (fd=%d, cmd=%d, parm3=0x%lx)\n", fd, cmd, parm3);
   return CNK_RC_FAILURE(ENOSYS);
}
