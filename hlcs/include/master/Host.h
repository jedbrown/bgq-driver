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

#ifndef _HOST_H
#define _HOST_H

#include <string>
#include <boost/tokenizer.hpp>
#include <utility/include/cxxsockets/SocketTypes.h>
#include "BGMasterExceptions.h"

static log4cxx::LoggerPtr host_logger(log4cxx::Logger::getLogger( "ibm.master" ));

//! \brief Representation of a Host in an IP network.  Encapsulates
//! the name and the IPv4 or IPv6 address.
class Host {
    std::string _ip;
    std::string _name;
    //! \brief This is the primary, preferred host.
    bool _primary;

    void build(std::string& identifier) {
        try {
            CxxSockets::SockAddr sa(AF_UNSPEC, identifier, "");
            _ip = sa.getHostAddr();
            _name = sa.getHostName();
            if(_name == _ip) _name = "unresolved";
        } catch(CxxSockets::SockSoftError& e) {
            std::ostringstream errormsg;
            errormsg << e.what() << " Will use IP address instead of name.";
            LOG4CXX_WARN(host_logger, errormsg.str());
        } catch(CxxSockets::CxxError& e) {
            std::ostringstream errormsg;
            errormsg << "Invalid host specification " << identifier
                     << ".  Not resolvable on local network. "
                     << "Check DNS, hostname and local network settings."
                     << e.what();
            throw BGMasterExceptions::APIUserError(BGMasterExceptions::INFO, errormsg.str());
        }
    }

public:
    //! \brief Default constructor.
    Host() : _ip(""), _name(""), _primary(false) {};

    //! \brief Constructor
    //! \param char* that is either a host name or an IP address
    Host(const char* identifier) {
        _primary = false;
        std::string id = identifier;
        build(id);
    }

    //! \brief Constructor
    //! \param string identifier.  Either an IP address or host name.
    Host(std::string identifier) { build(identifier); _primary = false; }

    //! \brief Copy constructor
    Host(const Host& host) : _ip(host._ip), _name(host._name), _primary(host._primary) { }

    bool operator==(const Host host) const { if(host.ip() == _ip)return true;return false; }
    bool operator<(const Host& h) const { if(_name < h._name)return true;return false; }

    //! \brief return the ip address
    std::string ip() const { return _ip; }

    //! \brief Return the fully qualified host name.
    //  ex: 'foo.bar.com'
    std::string fqhn() const {
        if(_name == "unresolved")
            return "Unresolved IP:" + _ip;
        return _name;
    }

    //! \brief Return unqualified host name. ex: 'foo'
    std::string uhn() const {
        if(_name == "unresolved") {
            return "Unresolved IP: " + _ip;
        }
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep(".");
        tokenizer tok(_name, sep);
        tokenizer::iterator name_it = tok.begin();
        return *name_it;
    }

    void set_primary(bool p) { _primary = p; }
    bool get_primary() { return _primary; }
};

#endif
