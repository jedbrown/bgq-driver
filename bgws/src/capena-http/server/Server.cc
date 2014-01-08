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

#include "Server.hpp"

#include "Connection.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>


using std::string;


LOG_DECLARE_FILE( "capena-http" );


namespace capena {
namespace server {


Server::Ptr Server::create(
        boost::asio::io_service& io_service,
        const bgq::utility::ServerPortConfiguration& port_config,
        const ResponderCreatorFn& responder_creator_fn
    )
{
    return Ptr( new Server(
            io_service,
            port_config,
            responder_creator_fn
        ) );
}


Server::Server(
        boost::asio::io_service& io_service,
        const bgq::utility::ServerPortConfiguration& port_config,
        const ResponderCreatorFn& responder_creator_fn
    ) :
        _io_service(io_service),
        _responder_creator_fn(responder_creator_fn)
{
    _acceptor_ptr.reset( new bgq::utility::Acceptor(
            _io_service,
            port_config,
            bgq::utility::portConfig::UserIdHandling::Skip
        ) );
}


void Server::start()
{
    _acceptor_ptr->start(
            boost::bind( &Server::_acceptorCallback, this, _1 )
        );
}


void Server::_acceptorCallback(
        const bgq::utility::Acceptor::AcceptArguments& args
    )
{
    // We don't get the User ID because configured the handshaker to not transmit it, instead an application could send the User ID info in a header

    if ( args.status == bgq::utility::Acceptor::Status::NowAccepting ) {
        LOG_INFO_MSG( "Now accepting HTTP connections" );
        return;
    }
    if ( args.status != bgq::utility::Acceptor::Status::OK ) {
        LOG_WARN_MSG( "Problem with HTTP client acceptor: " << args.status );
        return;
    }

    LOG_INFO_MSG( "Got new connection. user_type=" << args.user_type << " clientCN=" << (args.client_cn.empty() ? "None" : string() + "'" + args.client_cn + "'") );

    ConnectionPtr connection_ptr(Connection::create(
            args.socket_ptr,
            args.user_type,
            _responder_creator_fn
        ));

    connection_ptr->start();
}

} } // namespace capena::server
