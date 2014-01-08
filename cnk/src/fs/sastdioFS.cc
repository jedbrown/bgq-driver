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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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
#include "util.h"
#include "virtFS.h"
#include "sastdioFS.h"
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_MAILBOX_LEN 1024

// Please keep methods in alphabetical order for ease of maintenance.

uint64_t sastdioFS::close(int fd)
{
   // Free the descriptor now that it is closed.
   File_FreeFD(fd);

   TRACE( TRACE_StdioFS, ("(I) sastdioFS::close%s: fd=%d rc=0\n", whoami(), fd) );
   return CNK_RC_SUCCESS(0);
}

uint64_t sastdioFS::fstat(int fd, struct stat *statbuf)
{
   return fstat64(fd, (struct stat64 *)statbuf);
}

uint64_t sastdioFS::fstat64(int fd, struct stat64 *statbuf)
{
   // Set just enough to convince runtime this is a terminal.
   memset(statbuf, 0, sizeof(struct stat64));
   statbuf->st_mode = (S_IFCHR);

   TRACE( TRACE_StdioFS, ("(I) sastdioFS::fstat64%s: fd=%d rc=0\n", whoami(), fd) );
   return CNK_RC_SUCCESS(0);
}

uint64_t sastdioFS::ioctl(int fd, unsigned long int cmd, void *parm3)
{
   uint64_t rc;

   switch (cmd) {
      case 0x402c7413: // Special value used by isatty()
	 rc = CNK_RC_SUCCESS(0);
         break;

      default:
	 rc = CNK_RC_FAILURE(EINVAL);
         break;
   }

   TRACE( TRACE_StdioFS, ("(I) sastdioFS::ioctl%s: fd=%d cmd=0x%lx parm3=0x%p rc=%s\n", whoami(), fd, cmd, parm3, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t sastdioFS::write(int fd, const void *buffer, size_t length)
{
   // Length of zero bytes is a special case.  Just return success.
   if (length == 0) {
      return CNK_RC_SUCCESS(0);
   }

   // Limit amount of data written to mailbox.
   if (length > MAX_MAILBOX_LEN) { // Leave room for null terminator
      length = MAX_MAILBOX_LEN;
   }
   char buffercopy[MAX_MAILBOX_LEN];
   memcpy(buffercopy, buffer, length);

   // Write the buffer to the mailbox.
   uint64_t rc = CNK_RC_SUCCESS(length);
   bool done = false;
   do 
   {
       int fwrc = 0;
       if((GetPersonality()->Kernel_Config.NodeConfig & (PERS_ENABLE_NodeRepro | PERS_ENABLE_PartitionRepro)) == 0)
       {
           fwrc = NodeState.FW_Interface.putn(buffercopy, length);
       }
       if (fwrc >= 0) 
       {
           done = true;
       }
       else if (fwrc == FW_TOO_BIG) 
       {
           rc = CNK_RC_FAILURE(E2BIG);
           done = true;
       }
       else if (fwrc == FW_ERROR) 
       {
           rc = CNK_RC_FAILURE(EINVAL);
           done = true;
       }
       else if (fwrc == FW_EAGAIN) 
       {
           //! \todo How can we wait for a little bit?
       }
   } while (!done);

   TRACE( TRACE_StdioFS, ("(I) sastdioFS::write%s: fd=%d buffer=0x%p length=%ld rc=%s\n", whoami(), fd, buffer, length, CNK_RC_STRING(rc)) );
   return rc;
}

uint64_t sastdioFS::writev(int fd, const struct iovec *iov, int iovcnt)
{
   // Make this simple -- write each vector individually.
   int bytesWritten = 0;
   for ( int index = 0 ; index < iovcnt ; ++index ) {
      uint64_t rc = write(fd, iov[index].iov_base, iov[index].iov_len);
      if (CNK_RC_IS_FAILURE(rc)) {
         return rc;
      }
      bytesWritten += CNK_RC_VALUE(rc);
   }

   return CNK_RC_SUCCESS(bytesWritten);
}
