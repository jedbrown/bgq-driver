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

//! \file  UserIdentity.cc
//! \brief Methods for bgcios::UserIdentity class.

// Includes
#include <ramdisk/include/services/common/UserIdentity.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <errno.h>
#include <sstream>

using namespace bgcios;

LOG_DECLARE_FILE("cios.common");

UserIdentity::UserIdentity()
{
   _initialUserId = _userId = geteuid();
   _initialGroupId = _groupId = getegid();
   int numGroups = getgroups(bgcios::jobctl::MaxGroups, _groupList);
   if (numGroups > 0) {
      if (numGroups > bgcios::jobctl::MaxGroups) {
         _numInitialGroups = _numGroups = bgcios::jobctl::MaxGroups;
         LOG_CIOS_WARN_MSG(numGroups << " exceeded maximum of " << bgcios::jobctl::MaxGroups << " secondary groups");
      }
      else {
         _numInitialGroups = _numGroups = (uint16_t)numGroups;
      }
      memcpy(_initialGroupList, _groupList, sizeof(_groupList));
   }
   else {
      _numInitialGroups = _numGroups = 0;
   }

   strcpy(_unknown, "unknown");
   _passwdStringBuffer = NULL;
   _passwdStringLength = 1024;
   _groupStringBuffer = NULL;
   _groupStringLength = 8192;
}

void
UserIdentity::set(uid_t uid, gid_t gid, uint16_t numGroups, gid_t *groupList)
{
   // Set the secondary groups.
   LOG_CIOS_TRACE_MSG("setting " << numGroups << " secondary groups: " << gidListToNames(numGroups, groupList));
   if (numGroups > 0) {
      if (numGroups > bgcios::jobctl::MaxGroups) {
         _numGroups = bgcios::jobctl::MaxGroups;
         LOG_CIOS_WARN_MSG(numGroups << " exceeded maximum of " << bgcios::jobctl::MaxGroups << " secondary groups");
      }
      else {
         _numGroups = (uint16_t)numGroups;
      }
      memcpy(_groupList, groupList, sizeof(gid_t) * numGroups);
   }

   // Set the group id.
   LOG_CIOS_TRACE_MSG("setting gid to " << gid << " (" << gidToName(gid) << ")");
   _groupId = gid;

   // Set the user id.
   LOG_CIOS_TRACE_MSG("setting uid to " << uid << " (" <<  uidToName(uid) << ")");
   _userId = uid;

   return;
}

bgcios::MessageResult
UserIdentity::swap(void)
{
   bgcios::MessageResult result;

   // Swap the secondary groups.
   LOG_CIOS_TRACE_MSG("setting " << _numGroups << " secondary groups: " << gidListToNames(_numGroups, _groupList));
   if (setgroups(_numGroups, _groupList) != 0) {
      result.set(bgcios::SecondaryGroupIdError, errno);
      LOG_ERROR_MSG("error setting " << _numGroups << " secondary groups: " << bgcios::errorString(result.errorCode()));
      set(_initialUserId, _initialGroupId, _numInitialGroups, _initialGroupList);
      swap();
      return result;
   }

   // Swap the primary group id.
   LOG_CIOS_TRACE_MSG("setting gid to " << _groupId << " (" << gidToName(_groupId) << ")");
   if (setgid(_groupId) == -1) {
      result.set(bgcios::PrimaryGroupIdError, errno);
      LOG_ERROR_MSG("error swapping gid to " << _groupId << " (" << gidToName(_groupId) << "): " << bgcios::errorString(result.errorCode()));
      set(_initialUserId, _initialGroupId, _numInitialGroups, _initialGroupList);
      swap();
      return result;
   }

   // Swap the user id.
   LOG_CIOS_TRACE_MSG("setting uid to " << _userId << " (" <<  uidToName(_userId) << ")");
   if (setuid(_userId) == -1) {
      result.set(bgcios::UserIdError, errno);
      LOG_ERROR_MSG("error swapping uid to " << _userId << " (" << uidToName(_userId) << "): " << bgcios::errorString(result.errorCode()));
      set(_initialUserId, _initialGroupId, _numInitialGroups, _initialGroupList);
      swap();
      return result;
   }

   return result;
}

const std::string
UserIdentity::uidToString(uid_t uid) const
{
   std::ostringstream uidString;
   uidString << uid;
   return uidString.str();
}

const std::string
UserIdentity::uidToName(uid_t uid)
{
   if (_passwdStringBuffer == NULL) {
      _passwdStringBuffer = new char [_passwdStringLength];
   }

   bool retry = true;
   do {
      struct passwd *pwbufp = NULL;
      int rc = getpwuid_r(uid, &_passwdBuffer, _passwdStringBuffer, _passwdStringLength, &pwbufp);
      if ((rc == 0) && (pwbufp != NULL)) {
         return _passwdBuffer.pw_name;
      }

      if (rc != ERANGE) {
         return _unknown;
      }

      // String buffer wasn't big enough so increase it and try again.
      delete[] _passwdStringBuffer;
      _passwdStringLength *= 2;
      _passwdStringBuffer = new char [_passwdStringLength];

   } while (retry);

   return _unknown;
}

const std::string
UserIdentity::gidToString(gid_t gid) const
{
   std::ostringstream gidString;
   gidString << gid;
   return gidString.str();
}

const std::string
UserIdentity::gidToName(gid_t gid)
{
   if (_groupStringBuffer == NULL) {
      _groupStringBuffer = new char [_groupStringLength];
   }

   bool retry = true;
   do {
      struct group *grpbufp = NULL;
      int rc = getgrgid_r(gid, &_groupBuffer, _groupStringBuffer, _groupStringLength, &grpbufp);
      if ((rc == 0) && (grpbufp != NULL)) {
         return _groupBuffer.gr_name;
      }

      if (rc != ERANGE) {
         return _unknown;
      }

      // String buffer wasn't big enough so increase it and try again.
      delete[] _groupStringBuffer;
      _groupStringLength *= 2;
      _groupStringBuffer = new char [_groupStringLength];

   } while (retry);

   return _unknown;
}

const std::string
UserIdentity::gidListToString(int listSize, gid_t *gidList)
{
   std::ostringstream gidString;
   for (int index = 0; index < listSize; ++index) {
      gidString << gidList[index] << " ";
   }
   return gidString.str();
}

const std::string
UserIdentity::gidListToNames(int listSize, gid_t *gidList)
{
   std::ostringstream groupString;
   for (int index = 0; index < listSize; ++index) {
      groupString << gidList[index] << " (" << gidToName(gidList[index]) << ") ";
   }
   return groupString.str();
}

