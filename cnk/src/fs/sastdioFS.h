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

#ifndef _CNK_SASTDIOFS_H
#define _CNK_SASTDIOFS_H

// Includes
#include "virtFS.h"

//! \brief Standalone standard I/O file system.

class sastdioFS : public virtFS
{
public:

   sastdioFS(void) : virtFS() {};

   // Please keep methods in alphabetical order for ease of maintenance.

   uint64_t close(int fd);
   uint64_t fstat(int fd, struct stat *statbuf);
   uint64_t fstat64(int fd, struct stat64 *statbuf);
   uint64_t ioctl(int fd, unsigned long int cmd, void *parm3);
   uint64_t write(int fd, const void* buff, size_t cnt);
   uint64_t writev(int fd, const struct iovec *iov, int iovcnt);
};

#endif // _CNK_SASTDIOFS_H

