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

//! \file  SymbolicLink.cc
//! \brief Methods for bgcios::SymbolicLink class.

// Includes
#include <ramdisk/include/services/common/SymbolicLink.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

LOG_DECLARE_FILE("cios.common");

using namespace bgcios;

SymbolicLink::SymbolicLink(const std::string target, const std::string link, bool resolveTarget)
{
   // Resolve the path to the target.  Note that if the path cannot be resolved, just use the input path.
   // For example, a path to a compute node file system might not be found on the I/O node.
   if (resolveTarget) {
      char resolvedPathBuffer[PATH_MAX];
      if (::realpath(target.c_str(), resolvedPathBuffer) != NULL) {
         _target = resolvedPathBuffer;
      }
      else {
         LOG_CIOS_TRACE_MSG("error resolving path '" << target << "': " << bgcios::errorString(errno));
         _target = target;
      }
   }
   else {
      _target = target;
   }
   
   // Create the symbolic link to the target.
   _link = link;
   if (::symlink(_target.c_str(), _link.c_str()) != 0) {
      int err = errno;
      LOG_ERROR_MSG("error creating symbolic link '" << _link << "' to '" << _target << "': " << bgcios::errorString(err));
      LinkError e(err, "symlink() failed");
      throw e;
   }
}

void
SymbolicLink::setOwner(uid_t userId, gid_t groupId)
{
   // Set the owner and group of the symbolic link.
   if (::lchown(_link.c_str(), userId, groupId) != 0) {
      int err = errno;
      LOG_ERROR_MSG("error changing ownership of link '" << link << "' using uid " << userId << " and gid " << groupId << ": " << bgcios::errorString(err));
      LinkError e(err, "lchown() failed");
      throw e;
   }
   return;
}

void
SymbolicLink::remove(void)
{
   // Remove the symbolic link.
   if (::unlink(_link.c_str()) != 0) {
      int err = errno;
      LOG_ERROR_MSG("error removing symbolic link '" << _link << "' to '" << _target << "': " << bgcios::errorString(err));
      LinkError e(err, "unlink() failed");
      throw e;
   }
   return;
}

