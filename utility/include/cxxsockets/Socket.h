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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

/*!
 * \file utility/include/cxxsockets/Socket.h
 */

#ifndef CXXSOCKET_SOCKET_H
#define CXXSOCKET_SOCKET_H

#include <utility/include/cxxsockets/File.h>
#include <utility/include/cxxsockets/types.h>

namespace CxxSockets {

//! \brief Base class for all types of sockets.
//!
//! Locking strategy.
//! 1)  There are three classes of socket lock operations:
//!     a) sending
//!     b) receiving
//!     c) global socket ops
//! 2)  Sending locks provide mutual exclusion on sending and
//!     polling for sending operations
//! 3)  Receiving locks provide mutual exclusion on receiving
//!     and polling for receiving operations
//! 4)  Acquisition of BOTH locks is required for operations
//!     like socket(), bind(), connect() and close().
class Socket : public File
{
protected:
    Socket() : File() {}

    //! \brief Protected, non-locking bind.  Called from locking ops.
    void pBind(const SockAddr& addr);

    bool internal_getSockName(SockAddr& sa) const;
    bool internal_getPeerName(SockAddr& sa) const;

public:
    //! \brief Which side of the connection to shut down
    enum ShutDownSide { RECEIVE, SEND };

    //! \brief Make sure we close on destruction
    virtual ~Socket()  = 0;

    //! \brief bind() the socket
    void Bind(const SockAddr& addr);

    //! \brief Wraps get getsockname
    bool getSockName(SockAddr& sa) const;

    //! \brief Wraps get getpeername
    bool getPeerName(SockAddr& sa) const;

    //! \brief Shutdown one side of the connection.
    bool Shutdown(ShutDownSide rw);
};

}

#endif
