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
#include "virtFS.h"

//  mambo class - mambo supported functions are defined here
//                implementation is in mamboFS.cc
//
// Note that if ENABLE_MAMBOIO is not defined it will default to the base class
// for the member functions (which will return ENOSYS).
// Please keep functions in alphabetical order for ease of maintenance
//
class mamboFS : public virtFS {
   public:

      mamboFS(void) : virtFS() {};

#ifdef ENABLE_MAMBOIO
      bool isMatch(const char *path);
      uint64_t chdir(const char *path);
      uint64_t close(int fd);
      uint64_t fstat(int fd, struct stat *statbuf);
      uint64_t fstat64(int fd, struct stat64 *statbuf);
      uint64_t ftruncate(int fd, off_t offset);
      uint64_t lseek(int fd, off_t offset, int whence);
      uint64_t open(const char *path,int oflags, mode_t mode);
      uint64_t read(int fd, void *buff, size_t cnt);
      uint64_t stat(const char *__restrict__ path, struct stat *__restrict__ statbuf);
      uint64_t stat64(const char *__restrict__ path, struct stat64 *__restrict__ statbuf);
      uint64_t unlink(const char* path);
      uint64_t write(int fd, const void *buff,size_t cnt);
#endif
};


