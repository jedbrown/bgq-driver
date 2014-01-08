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

#include "cxxsockets/TCPSocket.h"

#include "cxxsockets/FileLocker.h"
#include "cxxsockets/SockAddr.h"

#include <netinet/tcp.h>

namespace CxxSockets {

LOG_DECLARE_FILE( "utility.cxxsockets" );

TCPSocket::TCPSocket() :
    Socket(),
    _nonagle(true)
{
    #ifdef SOCK_CLOEXEC
    _fileDescriptor = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
    #else
    _fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    #endif
}

TCPSocket::TCPSocket(
        const int family,
        const int fd
        ) :
    Socket(),
    _nonagle(true)
{
    if (fd == 0 && family == 0) {
        throw UserError(-1, "Cannot create socket, family and fd are null.");
    }
    if (fd) { // We already have a file descriptor
        _fileDescriptor = fd;
    } else { // We need a new fd.
        #ifdef SOCK_CLOEXEC
        _fileDescriptor = socket(family, SOCK_STREAM | SOCK_CLOEXEC, 0);
        #else
        _fileDescriptor = socket(family, SOCK_STREAM, 0);
        #endif
    }
}

void
TCPSocket::replaceFd(
        const int fd
        )
{
    FileLocker locker;
    LockFile(locker);
    if ( _fileDescriptor != -1 ) {
        ::close(_fileDescriptor);
    }
    _fileDescriptor = fd;
}

void
TCPSocket::releaseFd()
{
    _fileDescriptor = -1;
}

void
TCPSocket::performConnect(
        const SockAddr& remote
        )
{
    const int rc = connect(_fileDescriptor, (sockaddr*)(&remote), sizeof(struct sockaddr_storage));
    if (rc == -1) {
        const int error = errno;
        std::ostringstream msg;
        char buf[256];
        msg << "Connect failed with: " << strerror_r(error, buf, sizeof(buf));
        LOG_DEBUG_MSG( msg.str() );
        switch (error) {
            case EINTR:
            case EADDRNOTAVAIL:
                throw SoftError(error, msg.str());
            default:
                throw HardError(error, msg.str());
        }
    }

    _nonagle = false;
    this->toggleNoDelay();
}

void
TCPSocket::mConnect(
        const SockAddr& remote_sa
        )
{
    LOG_TRACE_MSG("Connecting to remote host " << remote_sa.getHostAddr() << ":" << remote_sa.getServicePort());
    performConnect(remote_sa);
}

void
TCPSocket::setProbe(
        const bool onoff,
        const int firstprobe,
        const int probeint,
        const int probecount
        )
{
    int opt = 0;
    socklen_t optlen = sizeof(opt);
    if (getsockopt(_fileDescriptor, SOL_SOCKET, SO_KEEPALIVE, &opt, &optlen)) {
        std::ostringstream msg;
        msg << "Could not get socket options.";
        LOG_DEBUG_MSG( msg.str() );
        throw HardError(errno, msg.str());
    }

    if (opt != onoff) {
        // Toggle keepalive if the passed value doesn't match current
        opt = 1;
        optlen = sizeof(opt);
        if (setsockopt(_fileDescriptor, SOL_SOCKET, SO_KEEPALIVE, &opt, optlen) < 0) {
            std::ostringstream msg;
            msg << "Could not enable keepalive.";
            LOG_DEBUG_MSG( msg.str() );
            throw HardError(errno, msg.str());
        }
    }

    // Now set the other options
    if (firstprobe) {
        opt = firstprobe;
        optlen = sizeof(opt);
        if (setsockopt(_fileDescriptor, SOL_TCP, TCP_KEEPIDLE, &opt, optlen) < 0) {
            std::ostringstream msg;
            msg << "Could not set keepalive first probe wait.";
            LOG_DEBUG_MSG( msg.str() );
            throw SoftError(errno, msg.str());
        }
    }

    if (probeint) {
        opt = probeint;
        optlen = sizeof(opt);
        if (setsockopt(_fileDescriptor, SOL_TCP, TCP_KEEPINTVL, &opt, optlen) < 0) {
            std::ostringstream msg;
            msg << "Could not set keepalive probe interval.";
            LOG_DEBUG_MSG( msg.str() );
            throw SoftError(errno, msg.str());
        }
    }

    if (probecount) {
        opt = probecount;
        optlen = sizeof(opt);
        if (setsockopt(_fileDescriptor, SOL_TCP, TCP_KEEPCNT, &opt, optlen) < 0) {
            std::ostringstream msg;
            msg << "Could not set keepalive probe count.";
            LOG_DEBUG_MSG( msg.str() );
            throw SoftError(errno, msg.str());
        }
    }

    return;
}

bool
TCPSocket::toggleNoDelay()
{
    if (_nonagle) {
        // nagle is off, turn it on
        int flag = 0;

        const int ret = setsockopt( _fileDescriptor, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );
        if (ret < 0) {
            std::ostringstream msg;
            msg << "Disabling nagle algorithm (TCP_NODELAY) failed.";
            LOG_DEBUG_MSG( msg.str() );
            throw HardError(errno, msg.str());
        }
        _nonagle = false;
    } else {
        // nagle is on, turn it off
        int flag = 1;

        const int ret = setsockopt( _fileDescriptor, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );
        if (ret < 0) {
            std::ostringstream msg;
            msg << "Enabling nagle algorithm (TCP_NODELAY) failed.";
            LOG_DEBUG_MSG( msg.str() );
            throw HardError(errno, msg.str());
        }
        _nonagle = true;
    }
    return !_nonagle;
}

void
TCPSocket::Connect(
        const SockAddr& remote_sa
        )
{
    FileLocker locker;
    LockFile(locker);
    mConnect(remote_sa);
}

int
TCPSocket::Send(
        Message& msg
        )
{
    // We do the locking and build the functor for the
    // send method we need to pass to the send logic.
    PthreadMutexHolder mutex;
    const int lockrc = LockSend(mutex);
    if (lockrc != 0) {
        std::ostringstream msg;
        if (lockrc != -1) {
            msg << "Send error, socket send side lock error: " << strerror(lockrc);
        } else {
            msg << "Send error, socket send side closed.";
        }
        LOG_DEBUG_MSG(msg.str());
        throw SoftError(lockrc, msg.str());
    }
    TCPSendFunctor sendf;
    return InternalSend(msg, sendf);
}

int
TCPSocket::Receive(
        Message& msg
        )
{
    PthreadMutexHolder mutex;
    const int lockrc = LockReceive(mutex);

    if (lockrc != 0) {
        std::ostringstream msg;
        if (lockrc != -1) {
            msg << "Receive error, socket receive side lock error: " << strerror(lockrc);
        } else {
            msg << "Receive error, socket receive side closed.";
        }
        LOG_DEBUG_MSG(msg.str());
        throw SoftError(lockrc, msg.str());
    }

    TCPReceiveFunctor recvf;
    return InternalReceive(msg, recvf);
}

int
TCPSendFunctor::operator()(
        const int fileDescriptor,
        const void* msg,
        const size_t length
        )
{
    const int flags = 0;
    const ssize_t bytes = send(fileDescriptor, msg, length, flags);
    return static_cast<int>(bytes);
}

int
TCPReceiveFunctor::operator()(
        const int fileDescriptor,
        const void* msg,
        const size_t length
        )
{
    const int flags = MSG_WAITALL;
    const ssize_t bytes_received = recv(fileDescriptor, (void*)msg, length, flags);
    return static_cast<int>(bytes_received);
}

} // namespace CxxSockets
