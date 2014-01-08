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
 * \file utility/include/cxxsockets/PollingSocketSet.h
 */
#ifndef CXXSOCKET_POLLING_SOCKET_SET_H
#define CXXSOCKET_POLLING_SOCKET_SET_H

#include <utility/include/cxxsockets/PollingFileSet.h>
#include <utility/include/cxxsockets/Socket.h>
#include <utility/include/cxxsockets/types.h>

#include <vector>

namespace CxxSockets {

//! \brief Set of sockets to poll
class PollingSocketSet : public PollingFileSet
{
protected:
    //! \brief vector of polling objects.  Vectors are
    //! guaranteed to be compatable with C-style arrays.
    void pAddSock(SocketPtr sock, PollType p = RECV); 
public:
    //! \brief ctor
    PollingSocketSet();

    //!  \brief Add a socket to the set.  
    void AddSock(SocketPtr sock, PollType p = RECV); 

    //!  \brief Remove a socket
    void RemoveSock(SocketPtr sock);
};

}

#endif
