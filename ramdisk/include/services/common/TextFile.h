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

//! \file  TextFile.h
//! \brief Declaration and inline methods for bgcios::TextFile class.

#ifndef COMMON_TEXTFILE_H
#define COMMON_TEXTFILE_H

// Includes
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

namespace bgcios
{

//! Exception for text file errors.

class TextFileError : public std::runtime_error
{
public:

   //! \brief  Default constructor.
   //! \param  err Error code value.
   //! \param  what String describing error.

   TextFileError(int err=0, const std::string what="") : std::runtime_error(what), _errcode(err) { }

   //! \brief  Get error code.
   //! \return Error code value.

   int errcode(void) const { return _errcode; }

protected:

   //! Error code (typically errno).
   int _errcode;
};

//! \brief Manage a text file.

class TextFile
{
public:

   //! \brief  Default constructor to open a text file.
   //! \param  fileName Path to text file.
   //! \throws TextFileError.

   TextFile(const std::string fileName);

   //! \brief  Constructor to create text file.
   //! \param  fileName Path to text file.
   //! \param  permissions File permissions.
   //! \throws TextFileError.

   TextFile(const std::string fileName, mode_t permissions);

   //! \brief  Default destructor.

   ~TextFile() { close(); }

   //! \brief  Set the owner of the text file.
   //! \param  userId User id of owner.
   //! \param  groupId Group id of owner.
   //! \return Nothing.
   //! \throws TextFileError.

   void setOwner(uid_t userId, gid_t groupId);

   //! \brief  Get the last modification time of the text file.
   //! \return Modification time.

   time_t getModificationTime(void);

   //! \brief  Get the last access time of the text file.
   //! \return Access time.

   time_t getAccessTime(void);

   //! \brief  Get the last change time of the text file.
   //! \return Access time.

   time_t getChangeTime(void);

   //! \brief  Write a vector of strings to the text file.
   //! \param  strings Pointer to vector of strings.
   //! \param  length Total length of strings in vector.
   //! \return Nothing.
   //! \throws TextFileError.

   void writeVector(const char *strings, size_t length);

   //! \brief  Write a single string to the text file.
   //! \param  str String to write to text file.
   //! \return Nothing.
   //! \throws TextFileError.

   void writeString(std::string str);

   //! \brief  Close the text file.
   //! \return Nothing.
   //! \throws TextFileError.

   void close(void);

   //! \brief  Remove the text file.
   //! \return Nothing.
   //! \throws LinkError.

   void remove(void);

   //! \brief  Get the path to the text file.
   //! \return Path to text file.

   const std::string& getName(void) const { return _fileName; }

private:

   //! Path to text file.
   std::string _fileName;

   //! File descriptor.
   int _fd;

   //! Special value to indicate descriptor is closed.
   static const int Closed = -1;

};

} // namespace bgcios

#endif // COMMON_TEXTFILE_H

