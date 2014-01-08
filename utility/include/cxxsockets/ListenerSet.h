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
 * \file utility/include/cxxsockets/ListenerSet.h
 */
#ifndef CXXSOCKET_LISTENERSET_H
#define CXXSOCKET_LISTENERSET_H

#include <utility/include/cxxsockets/types.h>
#include <utility/include/cxxsockets/FileSet.h>

#include <sys/socket.h>

namespace CxxSockets {

//! \brief Set of Listeners.
//!
//! Treat multiple listeners as one object for performing accepts.
//! More transparent v4/v6 support. 
class ListenerSet : public FileSet
{
public:
    //! \brief Constructor.  You need a list of sockaddrs even if you
    //! just want a single element.
    ListenerSet(
            const SockAddrList& sal,    //!< [in]
            int backlog = SOMAXCONN     //!< [in]
            );

    //! \brief Performs a blocking accept.
    //!
    //! \param sock TCP socket to accept
    //!
    //! \throws HardError if poll error.
    //!
    //! \return true if a socket was accepted.
    bool AcceptNew(const TCPSocketPtr& sock);
};

}

#endif
