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

//! \file  UserIdentity.h
//! \brief Declaration and inline methods for bgcios::UserIdentity class.

#ifndef COMMON_USERIDENTITY_H
#define COMMON_USERIDENTITY_H

// Includes
#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/common/MessageResult.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <string>

namespace bgcios
{

//! \brief Manage user identity.

class UserIdentity
{
public:

   //! \brief  Default constructor.

   UserIdentity();

   //! \brief  Set the specified user identity.
   //! \param  uid User id.
   //! \param  gid Primary group id.
   //! \param  numGroups Number of secondary group ids.
   //! \param  groupList List of secondary group ids.
   //! \return Nothing.

   void set(uid_t uid, gid_t gid, uint16_t numGroups, gid_t *groupList);

   //! \brief  Swap to the user identity.
   //! \return Result of operation.
   //! \note   If there is an error, the user identity is set back to the initial user identity.

   bgcios::MessageResult swap(void);

   //! \brief  Get the current user id.
   //! \return Current user id value.

   uid_t getUserId(void) const { return _userId; }

   //! \brief  Get the current user id as a string.
   //! \return User id string.

   const std::string getUserIdString(void) const { return uidToString(_userId); }

   //! \brief  Get the current user id name.
   //! \return User name string.

   const std::string getUserName(void) { return uidToName(_userId); }

   //! \brief  Get the current primary group id.
   //! \return Primary group id value.

   gid_t getGroupId(void) const { return _groupId; }

   //! \brief  Get the current primary group id as a string.
   //! \return Group id string.

   const std::string getGroupIdString(void) const { return gidToString(_groupId); } 

   //! \brief  Get the current primary group id name.
   //! \return Group name string.

   const std::string getGroupName(void) { return gidToName(_groupId); }

   //! \brief  Get the initial user id.
   //! \return Current user id value.

   uid_t getInitialUserId(void) const { return _initialUserId; }

   //! \brief  Get the initial user id as a string.
   //! \return User id string.

   const std::string getInitialUserIdString(void) const { return uidToString(_initialUserId); }

   //! \brief  Get the initial user id name.
   //! \return User name string.

   const std::string getInitialUserName(void) { return uidToName(_initialUserId); }

   //! \brief  Get the initial primary group id.
   //! \return Primary group id value.

   gid_t getInitialGroupId(void) const { return _initialGroupId; }

   //! \brief  Get the initial primary group id as a string.
   //! \return Group id string.

   const std::string getInitialGroupIdString(void) const { return gidToString(_initialGroupId); } 

   //! \brief  Get the initial primary group id name.
   //! \return Group name string.

   const std::string getInitialGroupName(void) { return gidToName(_initialGroupId); }

   //! \brief  Get the current group list as a sting.
   //! \return List of group ids string.

   const std::string getGroupListString(void) { return gidListToString(_numGroups, _groupList); }

protected:

   //! \brief  Convert a numeric user id to a string.
   //! \param  uid User id value.
   //! \return Number as a string.

   const std::string uidToString(uid_t uid) const;

   //! \brief  Convert a numeric user id to a name.
   //! \param  uid User id value.
   //! \return User name string.

   const std::string uidToName(uid_t uid);

   //! \brief  Convert a numeric group id to a string.
   //! \param  gid Group id value.
   //! \return Number as a string.

   const std::string gidToString(gid_t gid) const;

   //! \brief  Convert a numeric group id to a name.
   //! \param  gid Group id value.
   //! \return Group name string.

   const std::string gidToName(gid_t gid);

   //! \brief  Convert a list of numeric group ids to a string.
   //! \param  listSize Number of group ids in the list.
   //! \param  gidList List of group id values.
   //! \return Group list as a string.

   const std::string gidListToString(int listSize, gid_t *gidList);

   //! \brief  Convert a list of numeric group ids to a names.
   //! \param  listSize Number of group ids in the list.
   //! \param  gidList List of group id values.
   //! \return Group names string.

   const std::string gidListToNames(int listSize, gid_t *gidList);

   //! Initial user id.
   uid_t _initialUserId;

   //! User id.
   uid_t _userId;

   //! Initial group id.
   gid_t _initialGroupId;

   //! Group id.
   gid_t _groupId;

   //! Number of initial secondary group ids.
   uint16_t _numInitialGroups;

   //! List of initial secondary group ids.
   gid_t _initialGroupList[bgcios::jobctl::MaxGroups];

   //! Number of secondary group ids.
   uint16_t _numGroups;

   //! List of secondary group ids.
   gid_t _groupList[bgcios::jobctl::MaxGroups];

   //! Buffer for passwd structure.
   struct passwd _passwdBuffer;

   //! Buffer for passwd strings.
   char *_passwdStringBuffer;

   //! Size of passwd strings buffer.
   size_t _passwdStringLength;

   //! Buffer for group structure.
   struct group _groupBuffer;

   //! Buffer for group strings.
   char *_groupStringBuffer;

   //! Size of group strings buffer.
   size_t _groupStringLength;

   //! Buffer for unknown string.
   char _unknown[10];

};

} // namespace bgcios

#endif // COMMON_USERIDENTITY_H

