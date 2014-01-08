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
/* (C) Copyright IBM Corp.  2008, 2011                              */
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

#include <control/include/mcServer/MCServerRef.h>
#include "MMCSProperties.h"

// This class exists because the connection to mcserver is accessed
// concurrently from two different overlapping but not redundant
// interfaces, neither of which was serialized.  This class allows us to
// serialize access through one interface and continue to allow the use
// of the other one without rewriting huge chunks of code for either one
// or the other.

// The locker serves as an access point to the serialized interface that
// must be used before communicating with the unserialized interface.
// The locker will lock the serialized version so that the unserialized
// version doesn't have to.  In other words, it's a scope lock specialized
// for this particular use.

class McServerLocker {
  MCServerRef* _ref;
public:
  //! Instantiate this on the stack, and you get a locked
  //! mcserver connection to use for the duration of the scope.
  McServerLocker(MCServerRef* ref): _ref(ref) {
    _ref->getLock();
  }

  //! Out of scope, unlocked.
  ~McServerLocker() {
    _ref->releaseLock();
  }

  //! This gives us access to MCServerRef's public functions.
  MCServerRef* operator->() {
    return _ref;
  }

  //! MCServerRef's getConnectionFd() used to be public and
  //! the FD was passed around unprotected.  Now, it is private
  //! with this class as a friend.  So, you have to use this
  //! method to access the FD.  Since this class gets a lock
  //! when it is constructed and holds it until destruction,
  //! the FD is always serialized.
  int getConnectionFd() {
      return _ref->getConnectionFd();
  }
};
