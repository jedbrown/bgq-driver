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
 * \file utility/include/cxxsockets/SockAddr.h
 */

#ifndef SOCK_ADDRS_H
#define SOCK_ADDRS_H

#include <utility/include/cxxsockets/types.h>

#include <cstring>
#include <string>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

namespace CxxSockets {

const short AF_INET6_ONLY = 21;

//! \brief SockAddr class for representing IP address structures.
//!
//! Inherit from sockaddr_storage and provide wrappers
//! for generic address calls like getnameinfo().
//! There are no data members.  This allows us to pass a pointer
//! to this object to the BSD C API.
class SockAddr : sockaddr_storage
{
    friend class SockAddrList;
    static bool Addrinf(
            struct addrinfo*& addrinf,
            unsigned short family,
            const std::string& nodename,
            const std::string& service
            );

public:
    //! Constructors:
    SockAddr() { memset(this, 0, sizeof(sockaddr_storage)); }

    //! \brief Constructor based on a sockaddr struct.
    explicit SockAddr(sockaddr* sa);

    //! \brief Build a SockAddr from passed in data.
    explicit SockAddr(
            unsigned short family,                          //!< [in] family Address family e.g. AF_INET, AF_INET6, AF_UNSPEC
            const std::string& nodename = std::string(),    //!< [in] nodename hostname or IP address string
            const std::string& service = std::string()      //!< [in] port or service name from /etc/services
            );

    //! \brief getnameinfo wrappers
    //! Returns the service associated with the port
    std::string getServiceName();

    //! \brief Returns the service port number (always use for ephemerals)
    int getServicePort() const;

    //! \brief Returns the numeric form of the IP address
    std::string getHostAddr() const;

    //! \brief Returns the host name
    // If there is no associated name, it'll throw a soft error exception.
    std::string getHostName() const;

    //! \brief Returns real address family
    unsigned short family() const { return ss_family == AF_INET6_ONLY ? AF_INET6 : ss_family; }

    //! \brief Returns real address family OR cached AF_INET6_ONLY value
    short fm() const { return ss_family; }

    //! \brief Set the address family
    void setFamily(const short fam) { ss_family = fam; }
};

}

#endif
