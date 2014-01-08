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

#include "SecureConnect.h"

#include "common/Properties.h"

#include <utility/include/cxxsockets/SockAddrList.h>
#include <utility/include/cxxsockets/SecureTCPSocket.h>
#include <utility/include/portConfiguration/ClientPortConfiguration.h>
#include <utility/include/Log.h>

#include <boost/foreach.hpp>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {

namespace SecureConnect {

void
Connect(
        const bgq::utility::PortConfiguration::Pairs& portpairs,
        CxxSockets::SecureTCPSocketPtr& result
        )
{
    result.reset();

    if ( portpairs.empty() ) {
        return;
    }

    using namespace bgq::utility;

    BOOST_FOREACH( const PortConfiguration::Pair& portpair, portpairs) {
        if ( result )  {
            break;
        }

        try {
            CxxSockets::SockAddrList remote_list(AF_UNSPEC, portpair.first, portpair.second);
            BOOST_FOREACH( const CxxSockets::SockAddr& remote, remote_list ) {
                if ( result ) {
                    break;
                }

                bool done = false;
                while ( !done ) {
                    try {
                        const CxxSockets::SecureTCPSocketPtr sock(new CxxSockets::SecureTCPSocket(remote.family(), 0));
                        ClientPortConfiguration port_config(0, ClientPortConfiguration::ConnectionType::Administrative);
                        port_config.setProperties(common::Properties::getProperties(), "");
                        port_config.notifyComplete();
                        sock->Connect(remote, port_config);
                        result = sock;
                        LOG_DEBUG_MSG(
                                "Connected to " << remote.getHostAddr() << ":" << remote.getServicePort() <<
                                " successfully."
                                );
                        done = true;
                    } catch ( const CxxSockets::SoftError& e ) {
                        LOG_WARN_MSG( e.what() << ", trying again." );
                        sleep(3);
                    } catch ( const CxxSockets::Error& e ) {
                        LOG_ERROR_MSG(
                                "Connecting to " << remote.getHostAddr() << ":" << remote.getServicePort()
                                << " failed.");
                        done = true;
                    }
                }
            }
        } catch ( const CxxSockets::Error& e ) {
            LOG_ERROR_MSG(
                "Connect attempt failed to " << portpair.first << ":" << portpair.second  <<
                " " << e.what()
                );
        }
    }

    if ( !result ) {
        throw std::runtime_error("Unable to connect.");
    }
}

} // namespace SecureConnect

} } // namespace mmcs::server
