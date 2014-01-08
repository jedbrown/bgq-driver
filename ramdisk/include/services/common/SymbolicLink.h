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

//! \file  SymbolicLink.h
//! \brief Declaration and inline methods for bgcios::SymbolicLink class.

#ifndef COMMON_SYMBOLICLINK_H
#define COMMON_SYMBOLICLINK_H

// Includes
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace bgcios
{

//! Exception for symbolic link errors.

class LinkError : public std::runtime_error
{
public:

   //! \brief  Default constructor.
   //! \param  err Error code value.
   //! \param  what String describing error.

   LinkError(int err=0, const std::string what="") : std::runtime_error(what), _errcode(err) { }

   int errcode(void) const { return _errcode; }

protected:

   //! Error code (typically errno from link function).
   int _errcode;
};

//! \brief Manage a symbolic link.

class SymbolicLink
{
public:

   //! \brief  Default constructor.
   //! \param  target Path to target object pointed to by symbolic link.
   //! \param  link Path to symbolic link.
   //! \param  resolveTarget True to resolve the path to the target.
   //! \throws LinkError.

   SymbolicLink(const std::string target, const std::string link, bool resolveTarget = false);

   //! \brief  Set the owner of the symbolic link.
   //! \param  userId User id of owner.
   //! \param  groupId Group of owner.
   //! \return Nothing.
   //! \throws LinkError.

   void setOwner(uid_t userId, gid_t groupId);

   //! \brief  Remove the symbolic link.
   //! \return Nothing.
   //! \throws LinkError.

   void remove(void);

   //! \brief  Get the path of the target object.
   //! \return Path to target object.

   const std::string& getTarget(void) const { return _target; }

   //! \brief  Get the path of the symbolic link.
   //! \return Path to symbolic link.

   const std::string& getLink(void) const { return _link; }

private:

   //! Path to target object pointed to by symbolic link.
   std::string _target;

   //! Path to symbolic link.
   std::string _link;

};

} // namespace bgcios

#endif // COMMON_SYMBOLICLINK_H

