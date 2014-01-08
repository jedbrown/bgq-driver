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
/* (C) Copyright IBM Corp.  2012                                    */
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

#ifndef _CNK_PROCFS_H
#define _CNK_PROCFS_H

// Includes
#include "shmFS.h"


//! \brief Standard I/O file system.

class procFS : public shmFS
{
public:

   //! \brief  Default constructor.

   procFS(void) : shmFS() {};

   //! \brief  Initialize file system when node is booted.
   //! \return 0 when successful, errno when unsuccessful.

   int init(void);

   // Please keep methods in alphabetical order for ease of maintenance.
   uint64_t mmap(int fd, uint64_t length, uint64_t offset);
   uint64_t write(int fd, const void *buffer, size_t length);
   uint64_t open(const char *pathname, int oflags, mode_t mode);
   uint64_t readlink(const char *path, void *buf, size_t bufsiz);
   
#if 0 
   // other fs calls that may need intercepting for /cwd or /exe
   uint64_t access(const char *path, int mode);
   uint64_t chdir(const char *path);
   uint64_t lstat(const char *__restrict__ path, struct stat  *__restrict__ statbuf);
   uint64_t lstat64(const char *path, struct stat64 *statbuf);
   uint64_t stat(const char *__restrict__ path, struct stat  *__restrict__ statbuf);
   uint64_t stat64(const char *__restrict__ path, struct stat64 *__restrict__statbuf);
   uint64_t utime(const char *path, const struct utimbuf *utimes);


   // even more questionable...
   uint64_t chmod(const char *path, mode_t mode);
   uint64_t chown(const char *path, uid_t uid, gid_t gid);
   uint64_t lchown(const char *path, uid_t uid, gid_t gid);
   uint64_t link(const char *path, const char *link);
   uint64_t mkdir(const char *path, mode_t mode);
   uint64_t rename(const char *oldpath, const char *newpath);
   uint64_t rmdir(const char *path);
   uint64_t statfs(const char *__restrict__ path, struct statfs  *__restrict__ statbuf);
   uint64_t statfs64(const char *__restrict__ path, struct statfs64 *__restrict__statbuf);
   uint64_t symlink(const char *path, const char *link);
   uint64_t truncate(const char* path, off_t len);
   uint64_t truncate64(const char *path, off64_t len);
   uint64_t unlink(const char* path);
#endif
   
   bool isMatch(const char *path);
   virtual ShmMgr_t* getShmManager();
   virtual uint64_t  getDeviceID()   { return 0x01021997; };
   int cleanupJob(int fs);
};

#endif // _CNK_PROCFS_H


