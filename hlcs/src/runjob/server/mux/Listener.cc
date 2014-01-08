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
#include "server/mux/Listener.h"

#include "common/logging.h"

#include "server/mux/Connection.h"

#include "server/Options.h"
#include "server/Server.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

namespace runjob {
namespace server {
namespace mux {

LOG_DECLARE_FILE( runjob::server::log );

Listener::Listener(
        const Server::Ptr& server
        ) :
    _io_service( server->getIoService() ),
    _options( server->getOptions()),
    _server( server),
    _acceptor( _io_service, _options.getMuxPort() )
{
    // this->start() will be invoked after job reconnection
}

void
Listener::start()
{
    LOG_TRACE_MSG( __FUNCTION__ );

    // start accepting connection
    _acceptor.start(
            boost::bind(
                &Listener::acceptHandler,
                this,
                _1
                )
            );
}

void
Listener::acceptHandler(
        const bgq::utility::Acceptor::AcceptArguments& args
        )
{
    if ( args.status == bgq::utility::Acceptor::Status::OK ) {
        const Server::Ptr server( _server.lock() );
        if ( !server ) return;

        const boost::shared_ptr<Connection> connection(
                new Connection(
                    args.socket_ptr,
                    server
                    )
                );

        connection->start();
    } else if ( args.status == bgq::utility::Acceptor::Status::NowAccepting ) {
        LOG_INFO_MSG( "accepting connections" );
        BOOST_FOREACH( const auto& i, args.endpoints ) {
            LOG_INFO_MSG( "accepting on " << i );
        }
    } else if (
            args.status == bgq::utility::Acceptor::Status::ListenError ||
            args.status == bgq::utility::Acceptor::Status::ResolveError ||
            args.status == bgq::utility::Acceptor::Status::NoAcceptors
            )
    {
        _io_service.stop();
    } else if ( args.status == bgq::utility::Acceptor::Status::AcceptError ) {
        // still accepting on other ports
        LOG_WARN_MSG( "lost an acceptor" );
    }
}

} // mux
} // server
} // runjob
