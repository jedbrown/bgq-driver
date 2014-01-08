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
 * \file utility/include/cxxsockets/ListeningSocket.h
 */

#ifndef CXXSOCKET_LISTENING_SOCKET_H
#define CXXSOCKET_LISTENING_SOCKET_H

#include <utility/include/cxxsockets/Socket.h>
#include <utility/include/cxxsockets/types.h>

#include <sys/socket.h>

namespace CxxSockets {

//! \brief Generic v4/v6 listening socket.  
//!
//! The version is specified by the SockAddr used to initialize it.
//!
//! \see ListenerSet
class ListeningSocket : public Socket
{
    int Accept();
public:
    //! \brief Performs a bind() and listen() for a single sockaddr
    ListeningSocket(
            const SockAddr& addr,   //<! [in] object representing address to listen on
            int backlog = SOMAXCONN //<! [in] Maximum number of pending connections to allow
            );

    //! \brief This gets a new socket
    //! 
    //! \param sock Pass in a new socket object to fill
    void AcceptNew(const TCPSocketPtr& sock);
};

} 

#endif
