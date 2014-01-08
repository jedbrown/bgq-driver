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

//! \file  FileSystemUserIdentity.cc
//! \brief Methods for bgcios::FileSystemUserIdentity class.

// Includes
#include <ramdisk/include/services/common/FileSystemUserIdentity.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <sys/fsuid.h>
#include <errno.h>
#include <sstream>

using namespace bgcios;

LOG_DECLARE_FILE("cios.common");

bgcios::MessageResult
FileSystemUserIdentity::swapTo(uid_t uid, gid_t gid, uint16_t numGroups, gid_t *groupList)
{
   bgcios::MessageResult result;

   // Set the secondary groups.
   LOG_CIOS_TRACE_MSG("setting " << numGroups << " secondary groups: " << gidListToNames(numGroups, groupList));
   if (setgroups(numGroups, groupList) != 0) {
      result.set(bgcios::SecondaryGroupIdError, errno);
      LOG_ERROR_MSG("error setting " << numGroups << "secondary groups: " << bgcios::errorString(result.errorCode()));
      return result;
   }
   _numGroups = numGroups;
   memcpy(_groupList, groupList, sizeof(gid_t) * numGroups);

   // Set the file system group id.
   LOG_CIOS_TRACE_MSG("setting fsgid to " << gid << " (" << gidToName(gid) << ")");
   if (setfsgid(gid) == -1) {
      result.set(bgcios::PrimaryGroupIdError, errno);
      LOG_ERROR_MSG("error swapping fsgid to " << gid << " (" << gidToString(gid) << "): " << bgcios::errorString(result.errorCode()));
      swapBack();
      return result;
   }
   _groupId = gid;

   // Set the file system user id.
   LOG_CIOS_TRACE_MSG("setting fsuid to " << uid << " (" <<  uidToName(uid) << ")");
   if (setfsuid(uid) == -1) {
      result.set(bgcios::UserIdError, errno);
      LOG_ERROR_MSG("error swapping fsuid to " << uid << " (" << uidToString(uid) << "): " << bgcios::errorString(result.errorCode()));
      swapBack();
      return result;
   }
   _userId = uid;

   return result;
}

