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

#include "cxxsockets/Socket.h"

#include "cxxsockets/FileLocker.h"
#include "cxxsockets/SockAddr.h"
#include "cxxsockets/exception.h"

#include "Log.h"

#include <sys/socket.h>
#include <sys/un.h>

LOG_DECLARE_FILE( "utility.cxxsockets" );

namespace CxxSockets {

Socket::~Socket()
{
    Close();
}

void
Socket::Bind(
        const SockAddr& addr
        )
{
    FileLocker locker;
    LockFile(locker);
    pBind(addr);
}

void
Socket::pBind(
        const SockAddr& addr
        )
{
    const int reuse = 1;
    if (setsockopt(_fileDescriptor, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        std::ostringstream msg;
        msg << "Set socket option error for reuse: " << strerror(errno);
        LOG_DEBUG_MSG(msg.str());
        throw HardError(errno, msg.str());
    }

    if (addr.fm() == AF_INET6_ONLY) {
        // Do a v6 only bind. UNSPEC will get us two sockets. This is a simplification
        // of the BSD API which always gets you both if you ask for six UNLESS you
        // set this sockopt
        int one = 1;
        if (setsockopt(_fileDescriptor, IPPROTO_IPV6, IPV6_V6ONLY, &one, sizeof(int)) == -1) {
            std::ostringstream msg;
            msg << "Failed to set socket option for ipv6: " << strerror(errno);
            LOG_DEBUG_MSG(msg.str());
            throw HardError(errno, msg.str());
        }
    }

    socklen_t size = 0;
    if (addr.family() == AF_INET) {
        size = sizeof(sockaddr_in);
    } else if (addr.family() == AF_INET6) {
        size = sizeof(sockaddr_in6);
    } else if (addr.family() == AF_LOCAL) {
        size = static_cast<socklen_t>(SUN_LEN((sockaddr_un *)&addr));
    } else {
        std::ostringstream msg;
        msg << "Invalid address family: " << addr.family();
        LOG_DEBUG_MSG(msg.str());
        throw SoftError(EINVAL, msg.str());
    }

    if (bind(_fileDescriptor, (sockaddr*)(&addr), size) == -1) {
        std::ostringstream msg;
        msg << "Unable to bind to host " << addr.getHostName() << " and port "
            << addr.getServicePort() << ". Error is: " << strerror(errno);
        LOG_DEBUG_MSG(msg.str());
        throw HardError(errno, msg.str());
    }
}

bool
Socket::internal_getSockName(
        SockAddr& sa
        ) const
{
    socklen_t size = sizeof(sockaddr_storage);
    if (getsockname(_fileDescriptor, (sockaddr*)(&sa), &size) < 0) {
        std::ostringstream msg;
        msg << "Problem getting socket name: " << strerror(errno);
        LOG_DEBUG_MSG(msg.str());
        throw HardError(errno, msg.str());
    }
    return true;
}

bool
Socket::getSockName(
        SockAddr& sa
        ) const
{
    FileLocker locker;
    LockFile(locker);
    return internal_getSockName(sa);
}

bool
Socket::internal_getPeerName(
        SockAddr& sa
        ) const
{
    socklen_t size = sizeof(sockaddr_storage);
    if (getpeername(_fileDescriptor, (sockaddr*)(&sa), &size) < 0) {
        std::ostringstream msg;
        msg << "Problem getting peer name: " << strerror(errno);
        LOG_DEBUG_MSG(msg.str());
        throw HardError(errno, msg.str());
    }
    return true;
}

bool
Socket::getPeerName(
        SockAddr& sa
        ) const
{
    FileLocker locker;
    LockFile(locker);
    return internal_getPeerName(sa);
}

bool
Socket::Shutdown(
        const ShutDownSide rw
        )
{
    if (rw == RECEIVE && _receiver) {
        _receiver.reset();
        if (_fileDescriptor) {
            // Ignore any errors returned from shutdown()
            shutdown(_fileDescriptor, SHUT_RD);
        }
    } else if (_sender) {
        _sender.reset();
        if (_fileDescriptor) {
            // Ignore any errors returned from shutdown()
            shutdown(_fileDescriptor, SHUT_WR);
        }
    }
    return true;
}

}
