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
 * \file Host.h
 */

#ifndef UTILITY_CXXSOCKETS_HOST_H_
#define UTILITY_CXXSOCKETS_HOST_H_

#include <string>

namespace CxxSockets {

//! \brief Representation of a Host in an IP network.  Encapsulates
//! the name and the IPv4 or IPv6 address.
class Host
{
    std::string _ip;
    std::string _name;
    //! \brief This is the primary, preferred host.
    bool        _primary;

    void build(const std::string& identifier);

public:
    //! \brief Default constructor.
    Host() : _ip(), _name(), _primary(false) {}

    //! \brief Constructor
    //! \param identifier Either an IP address or host name.
    explicit Host(const std::string& identifier) { build(identifier); _primary = false; }

    bool operator==(const Host& host) const { return host.ip() == _ip; }
    bool operator<(const Host& h) const { return _name < h._name; }

    //! \brief return the ip address
    const std::string& ip() const { return _ip; }

    //! \brief Return the fully qualified host name.
    //  ex: 'foo.bar.com'
    const std::string& fqhn() const;

    //! \brief Return unqualified host name. ex: 'foo'
    std::string uhn() const;

    void set_primary(bool p) { _primary = p; }
    bool get_primary() const { return _primary; }
};

}

#endif
