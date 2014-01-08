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
 * \file utility/include/cxxsockets/SocketSendSide.h
 */

#ifndef CXXSOCKET_SOCKET_SEND_SIDE_H
#define CXXSOCKET_SOCKET_SEND_SIDE_H

#include <bgq_util/include/pthreadmutex.h>

namespace CxxSockets {

//! \brief Send side of the socket.  Just need it for shutdown locks.
class SocketSendSide
{
    friend class File;
    friend class Socket;
    friend class TCPSocket;
    PthreadMutex _sendLock;   // Scope lock for sending
    int Unlock() { return(_sendLock.Unlock()); }
    PthreadMutex& getLock() { return _sendLock; }

public:
    SocketSendSide() {}
};

}

#endif
