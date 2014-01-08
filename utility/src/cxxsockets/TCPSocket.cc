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
#include <netinet/tcp.h>
#include "SocketTypes.h"
#include "CxxSocketInlines.h"

using namespace CxxSockets;

LOG_DECLARE_FILE( "utility.cxxsockets" );

void TCPSocket::replaceFd(int fd) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
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

    CloseOnExec();
}

void TCPSocket::performConnect(SockAddr& local, SockAddr& remote) {
    LOGGING_DECLARE_FD_MDC;
    pBind(local);
    if(connect(_fileDescriptor, (sockaddr*)(&remote), sizeof(struct sockaddr_storage)) == -1) {
        std::ostringstream msg;
        LOG_ERROR_MSG("cannot connect() " << strerror(errno));
        msg << "connect error: " << strerror(errno);
        throw CxxSockets::SockHardError(errno, msg.str());
    }

    if(_nonagle) {
        int flag = 1;
    
        int ret = setsockopt( _fileDescriptor, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );
        if(ret < 0) {
            throw CxxSockets::SockHardError(errno, "Disabling nagle algorithm (TCP_NODELAY) failed");
        }
    }
}

void TCPSocket::mConnect(SockAddr& remote_sa) {
    LOGGING_DECLARE_FD_MDC;
    LOG_INFO_MSG("Connecting to remote host " << remote_sa.getHostAddr() << ":" << remote_sa.getServicePort());
    CxxSockets::SockAddr local(remote_sa.family(), "", "");
    performConnect(local, remote_sa);
    LOGGING_DECLARE_FT_MDC;
    internal_getSockName(local);
    _connection_data = 
        local.getHostAddr() + ":" + boost::lexical_cast<std::string>(local.getServicePort()) + "::" + 
        remote_sa.getHostAddr() + ":" + boost::lexical_cast<std::string>(remote_sa.getServicePort()); 
}

void TCPSocket::mConnect(FourTuple& ft) {
    LOGGING_DECLARE_FD_MDC;
    performConnect(std::tr1::get<LOCAL>(ft), std::tr1::get<REMOTE>(ft));
    LOGGING_DECLARE_FT_MDC;
}

void TCPSocket::setProbe(bool onoff, int firstprobe, int probeint, int probecount ){
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    int opt = 0;
    socklen_t optlen = sizeof(opt);
    if(getsockopt(_fileDescriptor, SOL_SOCKET, SO_KEEPALIVE, &opt, &optlen)) {
        throw CxxSockets::SockHardError(errno, "Could not get socket options");
    }
    
    if(opt != onoff) {
        // Toggle keepalive if the passed value doesn't match current
        opt = 1;
        optlen = sizeof(opt);
        if(setsockopt(_fileDescriptor, SOL_SOCKET, SO_KEEPALIVE, &opt, optlen) < 0) {
            throw CxxSockets::SockHardError(errno, "Could not enable keepalive");
        }
    }

    // Now set the other options
    if(firstprobe) {
        opt = firstprobe;
        optlen = sizeof(opt);
        if(setsockopt(_fileDescriptor, SOL_TCP, TCP_KEEPIDLE, &opt, optlen) < 0) {
            throw CxxSockets::SockSoftError(errno, "Could not set keepalive first probe wait");
        }
    }

    if(probeint) {
        opt = probeint;
        optlen = sizeof(opt);
        if(setsockopt(_fileDescriptor, SOL_TCP, TCP_KEEPINTVL, &opt, optlen) < 0) {
            throw CxxSockets::SockSoftError(errno, "Could not set keepalive probe interval");
        }
    }

    if(probecount) {
        opt = probecount;
        optlen = sizeof(opt);
        if(setsockopt(_fileDescriptor, SOL_TCP, TCP_KEEPCNT, &opt, optlen) < 0) {
            throw CxxSockets::SockSoftError(errno, "Could not set keepalive probe count");
        }
    }

    return;
}

bool TCPSocket::toggleNoDelay() {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    if(_nonagle) {
        // nagle is off, turn it on
        if(_fileDescriptor > 0) {
            int flag = 0;
            
            int ret = setsockopt( _fileDescriptor, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );
            if(ret < 0) {
                throw CxxSockets::SockHardError(errno, "Disabling nagle algorithm (TCP_NODELAY) failed");
            }
        }
        _nonagle = false;
    } else {
        // nagle is on, turn it off
        int flag = 1;
            
        int ret = setsockopt( _fileDescriptor, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );
        if(ret < 0) {
            throw CxxSockets::SockHardError(errno, "Enabling nagle algorithm (TCP_NODELAY) failed");
        }
        _nonagle = true;
    }
    return !_nonagle;
}

void TCPSocket::Connect(FourTuple& ft) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    FileLocker locker;
    LockFile(locker);
    LOG_INFO_MSG("Connected socket");
    mConnect(ft);
}

void TCPSocket::Connect(SockAddr& remote_sa) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    FileLocker locker;
    LockFile(locker);
    mConnect(remote_sa);        
    LOG_INFO_MSG("Connected socket");
}

int TCPSocket::Send(Message& msg, int flags) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    // We do the locking and build the functor for the
    // send method we need to pass to the send logic
    PthreadMutexHolder mutex;
    int lockrc = LockSend(mutex);
    if(lockrc != 0) {
        std::ostringstream msg;
        if(lockrc != -1)
            msg << "Send error.  Socket send side lock error: " << strerror(lockrc);
        else
            msg << "Send error.  Socket send side closed: " << 0;
        LOG_INFO_MSG(msg.str());
        throw CxxSockets::SockSoftError(lockrc, msg.str());
    }
    TCPSendFunctor sendf;
    return InternalSend(msg, flags, sendf);
}

int TCPSocket::SendUnManaged(Message& msg, int flags) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    PthreadMutexHolder mutex;
    int lockrc = LockSend(mutex);
    if(lockrc != 0) {
        std::ostringstream msg;
        if(lockrc != -1)
            msg << "SendUnManaged error.  Socket send side lock error: " << strerror(lockrc);
        else
            msg << "SendUnManaged error.  Socket send side closed: " << 0;
        throw CxxSockets::SockSoftError(lockrc, msg.str());
    }

    TCPSendFunctor sendf;
    return InternalSendUnmanaged(msg, flags, sendf);
}

int TCPSocket::Receive(Message& msg, int flags) {
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    PthreadMutexHolder mutex;
    int lockrc = LockReceive(mutex);

    if(lockrc != 0) {
        std::ostringstream msg;
        if(lockrc != -1)
            msg << "Receive error.  Socket receive side lock error: " << strerror(lockrc);
        else
            msg << "Receive error.  Socket receive side closed: " << 0;
        LOG_INFO_MSG(msg.str());
        throw CxxSockets::SockSoftError(lockrc, msg.str());
    }

    TCPReceiveFunctor recvf;
    return InternalReceive(msg, flags, recvf);
}

int TCPSocket::ReceiveUnManaged(Message& msg, unsigned int bytes, int flags){
    LOGGING_DECLARE_FD_MDC; LOGGING_DECLARE_FT_MDC;
    PthreadMutexHolder mutex;
    int lockrc = LockReceive(mutex);
    if(lockrc != 0) {
        std::ostringstream msg;
        if(lockrc != -1)
            msg << "ReceiveUnManaged error.  Socket receive side lock error: " << strerror(lockrc);
        else
            msg << "ReceiveUnManaged error.  Socket receive side closed: " << 0;
        LOG_INFO_MSG(msg.str());
        throw CxxSockets::SockSoftError(lockrc, msg.str());
    }

    TCPReceiveFunctor recvf;
    return InternalReceiveUnManaged(msg, bytes, flags, recvf);
}
