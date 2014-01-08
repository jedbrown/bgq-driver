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
#include "cxxsockets/SocketTypes.h"

using namespace CxxSockets;

LOG_DECLARE_FILE( "utility.cxxsockets" );

CxxSockets::Socket::Socket(Socket& sock) : 
    File()
{
    _fileDescriptor = sock._fileDescriptor;
    _receiver = sock._receiver;
    _sender = sock._sender;
    CloseOnExec();
}

CxxSockets::Socket::~Socket()
{
    LOG_TRACE_MSG("Destructing Socket object for " << _fileDescriptor << ".  Will close()");
    Close();
}

void CxxSockets::Socket::Bind(SockAddr& addr) {
    CxxSockets::FileLocker locker;
    LockFile(locker);
    pBind(addr);
}

void CxxSockets::Socket::pBind(SockAddr& addr) {
    int reuse = 1;
    if(setsockopt(_fileDescriptor, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        std::ostringstream msg;
        LOG_WARN_MSG("Failed to set sockopt for reuse " << strerror(errno));
        msg << "set socket option error for reuse: " << strerror(errno);
        throw CxxSockets::SockHardError(errno, msg.str());
    }

    if(addr.fm() == AF_INET6_ONLY) {
        // Do a V6 only bind.  UNSPEC will get us two sockets.  This is a simplification
        // of the BSD API which always gets you both if you ask for six UNLESS you
        // set this sockopt
        int one = 1;
        if(setsockopt(_fileDescriptor, IPPROTO_IPV6, IPV6_V6ONLY, &one, sizeof(int)) == -1) {
            std::ostringstream msg;
            LOG_WARN_MSG("Failed to set sockopt for ipv6 " << strerror(errno));
            msg << "set socket option error for ipv6: " << strerror(errno);
            throw CxxSockets::SockHardError(errno, msg.str());
        }
    }

    int size = 0;
    if(addr.family() == AF_INET) {
        size = sizeof(sockaddr_in);
    } else if(addr.family() == AF_INET6) {
        size = sizeof(sockaddr_in6);
    } else if(addr.family() == AF_LOCAL) {
        size = SUN_LEN((sockaddr_un *)&addr);
    } else {
        std::ostringstream msg;
        msg << "invalid address family: " << addr.family();
        LOG_INFO_MSG(msg.str());
        throw CxxSockets::SockSoftError(EINVAL, msg.str());
    }

    if(bind(_fileDescriptor, (sockaddr*)(&addr), size) == -1) {
        std::ostringstream msg;
        try {
            msg << "Unable to bind() to " << addr.getHostName() << ":" 
                << addr.getServicePort() << " error " << strerror(errno);
        } catch(CxxSockets::CxxSocketInternalError& e) {
            try {
            msg << "Unable to bind() to " << addr.getHostAddr() << ":" 
                << addr.getServicePort() << " error " << strerror(errno);
            } catch(CxxSockets::SockSoftError& e) {
                msg << "Unable to bind() to specified host/port pair.  " 
                    << strerror(errno);
            }
        }
        LOG_ERROR_MSG(msg.str());
        throw CxxSockets::SockHardError(errno, msg.str());
    }
    LOG_DEBUG_MSG("Bound to " << addr.getHostAddr() << ":" << addr.getServicePort());
}

bool CxxSockets::Socket::internal_getSockName(SockAddr& sa) {
    socklen_t size = sizeof(sockaddr_storage);
    if(getsockname(_fileDescriptor, (sockaddr*)(&sa), &size) < 0) {
        LOG_WARN_MSG("cannot get sockname " << strerror(errno));
        throw CxxSockets::SockHardError(errno, "cannot get sockname");
    }
    return true;
}

bool CxxSockets::Socket::getSockName(SockAddr& sa) {
    FileLocker locker;
    LockFile(locker);
    return internal_getSockName(sa);
}

bool CxxSockets::Socket::internal_getPeerName(SockAddr& sa) {
    socklen_t size = sizeof(sockaddr_storage);
    if(getpeername(_fileDescriptor, (sockaddr*)(&sa), &size) < 0) {
        LOG_WARN_MSG("cannot get peername " << strerror(errno));
        throw CxxSockets::SockHardError(errno, "cannot get peername");
    }
    return true;
}

bool CxxSockets::Socket::getPeerName(SockAddr& sa) {
    FileLocker locker;
    LockFile(locker);
    return internal_getPeerName(sa);
}

bool CxxSockets::Socket::Shutdown(ShutDownSide rw) { 
    if(rw == RECEIVE && _receiver) {
        _receiver.reset();
        if(_fileDescriptor) {
            if(shutdown(_fileDescriptor, SHUT_RD) == -1) {
            }
        }
    }
    else if(_sender) {
        _sender.reset(); 
        if(_fileDescriptor) {
            if(shutdown(_fileDescriptor, SHUT_WR) == -1) {
            }
        }
    }
    LOG_DEBUG_MSG("Shutting down " << rw << " side");
    return true;
}

