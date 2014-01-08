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

#include "cxxsockets/Host.h"
#include "cxxsockets/exception.h"
#include "cxxsockets/SockAddr.h"

#include "Log.h"

#include <boost/tokenizer.hpp>

#include <sstream>

LOG_DECLARE_FILE( "utility.cxxsockets" );

namespace CxxSockets {

void
Host::build(
        const std::string& identifier
        )
{
    try {
        SockAddr sa(AF_UNSPEC, identifier, "");
        _ip = sa.getHostAddr();
        _name = sa.getHostName();
        if (_name == _ip) {
            LOG_DEBUG_MSG( "Unresolved IP: " << _ip );
            _name = _ip;
        }
    } catch (const SoftError& e) {
        LOG_DEBUG_MSG(e.what() << " Will use IP address instead of name.");
    } catch (const Error& e) {
        std::ostringstream errormsg;
        errormsg << "Invalid host specification " << identifier
                 << ". Not resolvable on local network. "
                 << "Check DNS, hostname and local network settings. "
                 << e.what();
        LOG_DEBUG_MSG( errormsg.str() );
        throw Error( e.errcode, errormsg.str() );
    }
}

const std::string&
Host::fqhn() const
{
    return _name;
}

std::string
Host::uhn() const
{
    if (_name == _ip) {
        return _ip;
    }
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(".");
    tokenizer tok(_name, sep);
    tokenizer::iterator name_it = tok.begin();
    return *name_it;
}

}
