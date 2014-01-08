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
 * \file utility/include/cxxsockets/SocketReceiveSide.h
 */

#ifndef CXXSOCKET_SOCKET_RECEIVE_SIDE_H
#define CXXSOCKET_SOCKET_RECEIVE_SIDE_H

#include <bgq_util/include/pthreadmutex.h>

namespace CxxSockets {

//! \brief Receive side of the socket.  Just need it for shutdown locks.
class SocketReceiveSide
{
    friend class File;
    friend class Socket;
    friend class TCPSocket;
    PthreadMutex _recvLock;   // Scope lock for receiving
    int Unlock() { return(_recvLock.Unlock()); }
    PthreadMutex& getLock() { return _recvLock; }

public:
    SocketReceiveSide() {}
};

}

#endif
