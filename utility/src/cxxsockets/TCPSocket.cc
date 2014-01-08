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
    _connection_data(),
    _nonagle(true)
{
    #ifdef SOCK_CLOEXEC
    _fileDescriptor = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
    #else
    _fileDescriptor = socket(AF_INET, SOCK_STREAM , 0);
    #endif
}

TCPSocket::TCPSocket(
        const int family,
        const int fd
        ) :
    Socket(),
    _connection_data(),
    _nonagle(true)
{
    if(fd == 0 && family == 0) {
        throw UserError(-1, "Cannot create socket. Family and fd are null.");
    }
    if(fd) // We already have a file descriptor
        _fileDescriptor = fd;
    else {// We need a new fd.
        #ifdef SOCK_CLOEXEC
        _fileDescriptor = socket(family, SOCK_STREAM | SOCK_CLOEXEC, 0);
        #else
        _fileDescriptor = socket(family, SOCK_STREAM , 0);
        #endif
    }
}

void
TCPSocket::replaceFd(
        const int fd
        )
{
    CXXSOCKET_LOGGING_DECLARE_FD_MDC; CXXSOCKET_LOGGING_DECLARE_FT_MDC;
    FileLocker locker;
    LockFile(locker);
    ::close(_fileDescriptor);
    _fileDescriptor = fd;
    SockAddr local, remote_sa;
    internal_getSockName(local);
    internal_getPeerName(remote_sa);
    _connection_data =
        local.getHostAddr() + ":" + boost::lexical_cast<std::string>(local.getServicePort()) + "::" +
        remote_sa.getHostAddr() + ":" + boost::lexical_cast<std::string>(remote_sa.getServicePort());
}

void
TCPSocket::releaseFd()
{
    CXXSOCKET_LOGGING_DECLARE_FD_MDC;
    LOG_DEBUG_MSG( "releasing ownership of descriptor" );
    _fileDescriptor = -1;
}

void
TCPSocket::performConnect(
        const SockAddr& remote
        )
{
    CXXSOCKET_LOGGING_DECLARE_FD_MDC;
    const int rc = connect(_fileDescriptor, (sockaddr*)(&remote), sizeof(struct sockaddr_storage));
    if (rc == -1) {
        const int error = errno;
        std::ostringstream msg;
        char buf[256];
        msg << "cannot connect: " << strerror_r(error, buf, sizeof(buf));
        LOG_ERROR_MSG( msg.str() );
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
    CXXSOCKET_LOGGING_DECLARE_FD_MDC;
    LOG_INFO_MSG("Connecting to remote host " << remote_sa.getHostAddr() << ":" << remote_sa.getServicePort());
    performConnect(remote_sa);
    CXXSOCKET_LOGGING_DECLARE_FT_MDC;
    SockAddr local(remote_sa.family(), "", "");
    internal_getSockName(local);
    _connection_data =
        local.getHostAddr() + ":" + boost::lexical_cast<std::string>(local.getServicePort()) + "::" +
        remote_sa.getHostAddr() + ":" + boost::lexical_cast<std::string>(remote_sa.getServicePort());
}

void
TCPSocket::setProbe(
        const bool onoff,
        const int firstprobe,
        const int probeint,
        const int probecount
        )
{
    CXXSOCKET_LOGGING_DECLARE_FD_MDC; CXXSOCKET_LOGGING_DECLARE_FT_MDC;
    int opt = 0;
    socklen_t optlen = sizeof(opt);
    if(getsockopt(_fileDescriptor, SOL_SOCKET, SO_KEEPALIVE, &opt, &optlen)) {
        throw HardError(errno, "Could not get socket options");
    }

    if(opt != onoff) {
        // Toggle keepalive if the passed value doesn't match current
        opt = 1;
        optlen = sizeof(opt);
        if(setsockopt(_fileDescriptor, SOL_SOCKET, SO_KEEPALIVE, &opt, optlen) < 0) {
            throw HardError(errno, "Could not enable keepalive");
        }
    }

    // Now set the other options
    if(firstprobe) {
        opt = firstprobe;
        optlen = sizeof(opt);
        if(setsockopt(_fileDescriptor, SOL_TCP, TCP_KEEPIDLE, &opt, optlen) < 0) {
            throw SoftError(errno, "Could not set keepalive first probe wait");
        }
    }

    if(probeint) {
        opt = probeint;
        optlen = sizeof(opt);
        if(setsockopt(_fileDescriptor, SOL_TCP, TCP_KEEPINTVL, &opt, optlen) < 0) {
            throw SoftError(errno, "Could not set keepalive probe interval");
        }
    }

    if(probecount) {
        opt = probecount;
        optlen = sizeof(opt);
        if(setsockopt(_fileDescriptor, SOL_TCP, TCP_KEEPCNT, &opt, optlen) < 0) {
            throw SoftError(errno, "Could not set keepalive probe count");
        }
    }

    return;
}

bool
TCPSocket::toggleNoDelay()
{
    CXXSOCKET_LOGGING_DECLARE_FD_MDC; CXXSOCKET_LOGGING_DECLARE_FT_MDC;
    if(_nonagle) {
        // nagle is off, turn it on
        int flag = 0;

        const int ret = setsockopt( _fileDescriptor, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );
        if(ret < 0) {
            throw HardError(errno, "Disabling nagle algorithm (TCP_NODELAY) failed");
        }
        _nonagle = false;
    } else {
        // nagle is on, turn it off
        int flag = 1;

        const int ret = setsockopt( _fileDescriptor, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );
        if(ret < 0) {
            throw HardError(errno, "Enabling nagle algorithm (TCP_NODELAY) failed");
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
    CXXSOCKET_LOGGING_DECLARE_FD_MDC; CXXSOCKET_LOGGING_DECLARE_FT_MDC;
    FileLocker locker;
    LockFile(locker);
    mConnect(remote_sa);
    LOG_INFO_MSG("Connected socket");
}

int
TCPSocket::Send(
        Message& msg,
        const int flags
        )
{
    CXXSOCKET_LOGGING_DECLARE_FD_MDC; CXXSOCKET_LOGGING_DECLARE_FT_MDC;
    // We do the locking and build the functor for the
    // send method we need to pass to the send logic
    PthreadMutexHolder mutex;
    const int lockrc = LockSend(mutex);
    if(lockrc != 0) {
        std::ostringstream msg;
        if(lockrc != -1)
            msg << "Send error.  Socket send side lock error: " << strerror(lockrc);
        else
            msg << "Send error.  Socket send side closed: " << 0;
        LOG_INFO_MSG(msg.str());
        throw SoftError(lockrc, msg.str());
    }
    TCPSendFunctor sendf;
    return InternalSend(msg, flags, sendf);
}

int
TCPSocket::SendUnManaged(
        Message& msg,
        const int flags
        )
{
    CXXSOCKET_LOGGING_DECLARE_FD_MDC; CXXSOCKET_LOGGING_DECLARE_FT_MDC;
    PthreadMutexHolder mutex;
    const int lockrc = LockSend(mutex);
    if(lockrc != 0) {
        std::ostringstream msg;
        if(lockrc != -1)
            msg << "SendUnManaged error.  Socket send side lock error: " << strerror(lockrc);
        else
            msg << "SendUnManaged error.  Socket send side closed: " << 0;
        throw SoftError(lockrc, msg.str());
    }

    TCPSendFunctor sendf;
    return InternalSendUnmanaged(msg, flags, sendf);
}

int
TCPSocket::Receive(
        Message& msg,
        const int flags
        )
{
    CXXSOCKET_LOGGING_DECLARE_FD_MDC; CXXSOCKET_LOGGING_DECLARE_FT_MDC;
    PthreadMutexHolder mutex;
    const int lockrc = LockReceive(mutex);

    if(lockrc != 0) {
        std::ostringstream msg;
        if(lockrc != -1)
            msg << "Receive error.  Socket receive side lock error: " << strerror(lockrc);
        else
            msg << "Receive error.  Socket receive side closed: " << 0;
        LOG_INFO_MSG(msg.str());
        throw SoftError(lockrc, msg.str());
    }

    TCPReceiveFunctor recvf;
    return InternalReceive(msg, flags, recvf);
}

int
TCPSocket::ReceiveUnManaged(
        Message& msg,
        const unsigned int bytes,
        const int flags
        )
{
    CXXSOCKET_LOGGING_DECLARE_FD_MDC; CXXSOCKET_LOGGING_DECLARE_FT_MDC;
    PthreadMutexHolder mutex;
    const int lockrc = LockReceive(mutex);
    if(lockrc != 0) {
        std::ostringstream msg;
        if(lockrc != -1)
            msg << "ReceiveUnManaged error.  Socket receive side lock error: " << strerror(lockrc);
        else
            msg << "ReceiveUnManaged error.  Socket receive side closed: " << 0;
        LOG_INFO_MSG(msg.str());
        throw SoftError(lockrc, msg.str());
    }

    TCPReceiveFunctor recvf;
    return InternalReceiveUnManaged(msg, bytes, flags, recvf);
}

int
TCPSendFunctor::operator()(
        const int _fileDescriptor,
        const void* msg,
        const int length,
        const int flags
        )
{
    LOG_DEBUG_MSG("TCP Sending unencrypted data");
    const int bytes = send(_fileDescriptor, msg, length, flags);
    if(bytes < 0)
        error = errno;
    return bytes;
}

int
TCPReceiveFunctor::operator()(
        const int _fileDescriptor,
        const void* msg,
        const int length,
        const int flags
        )
{
    LOG_DEBUG_MSG("TCP Receiving unencrypted data");
    const int bytes_received = recv(_fileDescriptor, (void*)msg, length, flags);
    LOG_TRACE_MSG("Data received");
    if(bytes_received < 0)
        error = errno;
    return bytes_received;
}

}
