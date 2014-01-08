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

//! \file  FileSystemUserIdentity.h
//! \brief Declaration and inline methods for bgcios::FileSystemUserIdentity class.

#ifndef COMMON_FILESYSTEMUSERIDENTITY_H
#define COMMON_FILESYSTEMUSERIDENTITY_H

// Includes
#include "UserIdentity.h"

namespace bgcios
{

//! \brief Manage user identity for file system authority checks.

class FileSystemUserIdentity : public UserIdentity
{
public:

   //! \brief  Default constructor.

   FileSystemUserIdentity() : UserIdentity() { }

   //! \brief  Default destructor.

   ~FileSystemUserIdentity() { swapBack(); }

   //! \brief  Swap to the specified user identity for file system checks.
   //! \param  uid User id.
   //! \param  gid Primary group id.
   //! \param  numGroups Number of secondary group ids.
   //! \param  groupList List of secondary group ids.
   //! \return Result of operation.

   bgcios::MessageResult swapTo(uid_t uid, gid_t gid, uint16_t numGroups, gid_t *groupList);

   //! \brief  Swap back to the initial user identity.
   //! \return Result of operation.

   bgcios::MessageResult swapBack(void) { return swapTo(_initialUserId, _initialGroupId, _numInitialGroups, _initialGroupList); }

};

} // namespace bgcios

#endif // COMMON_FILESYSTEMUSERIDENTITY_H

