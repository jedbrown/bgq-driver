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

#include "cxxsockets/SockAddrList.h"

#include "cxxsockets/exception.h"

#include "Log.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

namespace CxxSockets {

LOG_DECLARE_FILE( "utility.cxxsockets" );

SockAddrList::SockAddrList(
        const unsigned short family,
        const std::string& nodename,
        const std::string& service
        )
{
    if (service.empty() && nodename.empty()) {
        std::ostringstream msg;
        msg << "Both service and node cannot be zero.";
        LOG_DEBUG_MSG(msg.str());
        throw UserError(-1, msg.str());
    }

    struct addrinfo* addrinf = 0;
    SockAddr::Addrinf(addrinf, family, nodename, service);
    const boost::shared_ptr<struct addrinfo> guard( addrinf, boost::bind(&freeaddrinfo, _1) );
    while (addrinf) {
        SockAddr sa(addrinf->ai_addr);
        if (family == AF_INET6_ONLY) {
            sa.setFamily( AF_INET6_ONLY );
        }
        if (sa.family() == AF_INET) {
            _sockque.push_back(sa);
        } else if (sa.family() == AF_INET6) { // v6 goes first
            _sockque.push_front(sa);
        }
        addrinf = addrinf->ai_next;
    }
}

}
