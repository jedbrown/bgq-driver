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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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

//! \file  TextFile.cc
//! \brief Methods for bgcios::TextFile class.

// Includes
#include <ramdisk/include/services/common/TextFile.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

LOG_DECLARE_FILE("cios.common");

using namespace bgcios;

TextFile::TextFile(std::string fileName)
{
   // Set private data.
   _fileName = fileName;
   _fd = Closed;

   // Create the file.
   _fd = ::open(_fileName.c_str(), O_RDWR);
   if (_fd < 0) {
      int err = errno;
      LOG_ERROR_MSG("error creating file '" << _fileName << "': " << bgcios::errorString(err));
      TextFileError e(err, "open() failed");
      throw e;
   }
}

TextFile::TextFile(std::string fileName, mode_t permissions)
{
   // Set private data.
   _fileName = fileName;
   _fd = Closed;

   // Create the file.
   _fd = ::open(_fileName.c_str(), O_CREAT|O_WRONLY, permissions);
   if (_fd < 0) {
      int err = errno;
      LOG_ERROR_MSG("error creating file '" << _fileName << "' with permissions " << permissions << ": " << bgcios::errorString(err));
      TextFileError e(err, "open() failed");
      throw e;
   }
}

void
TextFile::setOwner(uid_t userId, gid_t groupId)
{
   // Set the owner and group of the file.
   if (::fchown(_fd, userId, groupId) != 0) {
      int err = errno;
      LOG_ERROR_MSG("error changing ownership of file '" << _fileName << "' using uid " << userId << " and gid " << groupId << ": " << bgcios::errorString(err));
      TextFileError e(err, "fchown() failed");
      throw e;
   }
   return;
}

time_t
TextFile::getModificationTime(void)
{
   struct stat info;
   if (::fstat(_fd, &info) != 0) {
      int err = errno;
      LOG_ERROR_MSG("error getting modification time of file '" << _fileName << "': " << bgcios::errorString(err));
      TextFileError e(err, "fstat() failed");
      throw e;
   }
   return info.st_mtime;
}

time_t
TextFile::getAccessTime(void)
{
   struct stat info;
   if (::fstat(_fd, &info) != 0) {
      int err = errno;
      LOG_ERROR_MSG("error getting access time of file '" << _fileName << "': " << bgcios::errorString(err));
      TextFileError e(err, "fstat() failed");
      throw e;
   }
   return info.st_atime;
}

time_t
TextFile::getChangeTime(void)
{
   struct stat info;
   if (::fstat(_fd, &info) != 0) {
      int err = errno;
      LOG_ERROR_MSG("error getting creation time of file '" << _fileName << "': " << bgcios::errorString(err));
      TextFileError e(err, "fstat() failed");
      throw e;
   }
   return info.st_ctime;
}

void
TextFile::writeVector(const char *strings, size_t length)
{
   // Write the strings to the file.
   ssize_t rc = 0;
   char nullByte = 0;
   if (length != 0) {
      rc = ::write(_fd, strings, length);
      if (rc != (ssize_t)length) {
         int err = errno;
         LOG_ERROR_MSG("error writing " << length << " bytes to file '" << _fileName << "': rc " << rc << ", " << bgcios::errorString(err));
         TextFileError e(err, "write() failed");
         throw e;
      }
   }
   else {
      rc = ::write(_fd, &nullByte, sizeof(nullByte));
      if (rc != sizeof(nullByte)) {
         int err = errno;
         LOG_ERROR_MSG("error writing null byte to file '" << _fileName << "': rc " << rc << ", " << bgcios::errorString(err));
         TextFileError e(err, "write() failed");
         throw e;
      }
   }

   // Write a final null byte to the file.
   rc = ::write(_fd, &nullByte, sizeof(nullByte));
   if (rc != sizeof(nullByte)) {
      int err = errno;
      LOG_ERROR_MSG("error writing final null byte to file '" << _fileName << "': rc " << rc << ", " << bgcios::errorString(err));
      TextFileError e(err, "write() failed");
      throw e;
   }

   return;
}

void
TextFile::writeString(std::string str)
{
   ssize_t rc = ::write(_fd, str.c_str(), str.length()+1);
   if (rc != (ssize_t)str.length()+1) {
      int err = errno;
      LOG_ERROR_MSG("error writing string to file '" << _fileName << "': rc " << rc << ", " << bgcios::errorString(err));
      TextFileError e(err, "write() failed");
      throw e;
   }

   return;
}

void
TextFile::close(void)
{
   if (_fd != Closed) {
      ::close(_fd);
      _fd = Closed;
   }
   return;
}

