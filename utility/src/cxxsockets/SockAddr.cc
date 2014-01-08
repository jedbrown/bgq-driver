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

#include "cxxsockets/SockAddr.h"

#include "cxxsockets/exception.h"

#include "Log.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <cerrno>
#include <cstring>

#include <arpa/inet.h>
#include <sys/un.h>

namespace CxxSockets {

LOG_DECLARE_FILE( "utility.cxxsockets" );

bool
SockAddr::Addrinf(
        struct addrinfo*& addrinf,
        const unsigned short family,
        const std::string& nodename,
        const std::string& service
        )
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(addrinfo));

    hints.ai_family = family;
    char* node;
    if (nodename.empty()) {
        node = NULL;
        hints.ai_flags = AI_PASSIVE;
    } else {
        node = (char*)(nodename.c_str());
    }

    // Note:  This is TCP only.
    hints.ai_socktype = SOCK_STREAM;
    const int retval = getaddrinfo(node, service.c_str(), &hints, &addrinf);
    if (retval != 0) {
        std::ostringstream msg;
        msg << "Failed to get address info for " << nodename << ": " << gai_strerror(retval);
        LOG_DEBUG_MSG(msg.str());
        if (addrinf) {
            freeaddrinfo(addrinf);
        }
        throw HardError(errno, msg.str());
    }

    char buff[128];
    inet_ntop(family, &((struct sockaddr_in*)(addrinf->ai_addr))->sin_addr.s_addr, buff, 128);
    if (!addrinf) {
        std::ostringstream msg;
        msg << "Unable to resolve address: " << strerror(errno);
        LOG_DEBUG_MSG(msg.str());
        throw HardError(errno, msg.str());
    }
    return true;
}

std::string
SockAddr::getServiceName()
{
    char svc_buf[NI_MAXSERV];
    const int rc = getnameinfo((sockaddr*)(this), sizeof(sockaddr_storage), NULL, 0, svc_buf, NI_MAXSERV, 0);
    if (rc != 0) {
        std::ostringstream msg;
        msg << "Problem getting service name: " << gai_strerror(rc);
        LOG_DEBUG_MSG(msg.str());
        throw InternalError(rc, msg.str());
    }
    const std::string sname(svc_buf);
    return sname;
}

int
SockAddr::getServicePort() const
{
    char svc_buf[NI_MAXSERV];
    const int rc = getnameinfo((sockaddr*)(this), sizeof(sockaddr_storage), NULL, 0, svc_buf, NI_MAXSERV, NI_NUMERICSERV);
    if (rc != 0) {
        std::ostringstream msg;
        msg << "Problem getting service port: " << gai_strerror(rc);
        LOG_DEBUG_MSG(msg.str());
        throw InternalError(rc, msg.str());
    }
    const int retval = atoi(svc_buf);
    return retval;
}

std::string
SockAddr::getHostName() const
{
    char host_buf[NI_MAXHOST];
    const int rc = getnameinfo((sockaddr*)(this), sizeof(sockaddr_storage), host_buf, sizeof(host_buf), 0, 0, 0);
    if (rc != 0) {
        std::ostringstream msg;
        msg << "Problem getting host name: " << gai_strerror(rc);
        LOG_DEBUG_MSG(msg.str());
        throw SoftError(rc, msg.str());
    }
    const std::string sname(host_buf);
    return sname;
}

std::string
SockAddr::getHostAddr() const
{
    char host_buf[NI_MAXHOST];
    socklen_t size = 0;
    if (family() == AF_INET) {
        size = sizeof(sockaddr_in);
    } else {
        size = sizeof(sockaddr_in6);
    }

    int error = getnameinfo((sockaddr*)(this), size, host_buf, sizeof(host_buf), 0, 0, NI_NUMERICHOST);
    if (error != 0) {
        std::ostringstream msg;
        msg << "Unable to find host address: " << gai_strerror(error);
        LOG_DEBUG_MSG(msg.str());
        throw InternalError(0, msg.str());
    }
    const std::string sname(host_buf);
    return sname;
}

SockAddr::SockAddr(
        sockaddr* sa
        )
{
    // Make sure there's no junk in the base object.
    bzero(this, sizeof(SockAddr));

    // Overwrite the base object with the new one.
    socklen_t size = 0;
    if (sa->sa_family == AF_INET) {
        size = sizeof(sockaddr_in);
    } else if (sa->sa_family == AF_INET6) {
        size = sizeof(sockaddr_in6);
    } else if (sa->sa_family == AF_LOCAL) {
        size = static_cast<socklen_t>(SUN_LEN((sockaddr_un *)sa));
    } else {
         std::ostringstream msg;
         msg << "Invalid address family: " << sa->sa_family;
         LOG_DEBUG_MSG(msg.str());
         throw SoftError(EINVAL, msg.str());
    }

    memcpy(this, sa, size);
}

SockAddr::SockAddr(
        const unsigned short family,
        const std::string& nodename,
        const std::string& service
        )
{
    bzero(this, sizeof(sockaddr_storage));
    struct addrinfo* addrinf = 0;
    Addrinf(addrinf, family, nodename, service);
    if (addrinf) {
        SockAddr sa(addrinf->ai_addr);
        if (family == AF_INET6_ONLY) {
            sa.setFamily( AF_INET6_ONLY );
        }
    }

    // Copy the addrinfo struct's sockaddr back in to us.
    size_t size = 0;
    if (addrinf->ai_family == AF_INET) {
        size = sizeof(sockaddr_in);
    } else {
        size = sizeof(sockaddr_in6);
    }

    memcpy(this, addrinf->ai_addr, size);

    freeaddrinfo(addrinf);
}

}
