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


#include "portConfiguration/Connector.h"

#include "Handshaker.h"
#include "PairConnector.h"
#include "pc_util.h"

#include "Log.h"
#include "ScopeGuard.h"
#include "UserId.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <boost/spirit/home/phoenix/core.hpp>
#include <boost/spirit/home/phoenix/operator.hpp>
#include <boost/spirit/home/phoenix/statement.hpp>

#include <boost/spirit/home/phoenix/object/construct.hpp>

#include <iostream>
#include <sstream>


using boost::lexical_cast;

using boost::asio::ip::tcp;

using std::ostringstream;
using std::string;


LOG_DECLARE_FILE( "utility" );


namespace bgq {
namespace utility {


// Class Connector::ConnectResult


Connector::ConnectResult::ConnectResult()
    :
        error(Error::ResolveError),
        error_str( "Not initialized" )
{
    // Nothing to do.
}


Connector::ConnectResult::ConnectResult(
        portConfig::SocketPtr& socket_ptr,
        Error::Type error,
        const std::string& error_str,
        std::string server_cn
    ) :
        socket_ptr(socket_ptr),
        error(error),
        error_str(error_str),
        server_cn(server_cn)
{
    // Nothing to do.
}


// Class Connector

Connector::Connector(
        boost::asio::io_service& io_service,
        const ClientPortConfiguration& port_configuration
    ) :
    _io_service(io_service),
    _port_config(port_configuration),
    _context_ptr()
{
    // Nothing to do.
}


portConfig::SocketPtr
Connector::connect()
{
    using namespace boost::phoenix;
    using namespace boost::phoenix::arg_names;
    ConnectResult result;

    // reset I/O service when we go out of scope
    ScopeGuard reset(
            boost::bind(
                &boost::asio::io_service::reset,
                boost::ref( _io_service )
                )
            );

    // connect
    this->async_connect(
            boost::phoenix::ref( result ) = arg1
        );

    // run I/O service
    _io_service.run();

    if ( result.error != Error::Success )
    {
        switch ( result.error )
        {
            case Error::ResolveError: throw ResolveError( result.error_str );
            case Error::ConnectError: throw ConnectError( result.error_str );
            case Error::HandshakeError: throw HandshakeError( result.error_str );
            default: throw ConnectError( result.error_str );
        }
    }

    return result.socket_ptr;
}


portConfig::SocketPtr
Connector::connect(
        Error::Type& error_out
    )
{
    error_out = Error::Success;

    try {
        return connect();
    } catch ( ResolveError& e ) {
        LOG_WARN_MSG( e.what() );
        error_out = Error::ResolveError;
    } catch ( HandshakeError& e ) {
        LOG_WARN_MSG( e.what() );
        error_out = Error::HandshakeError;
    } catch ( ConnectError& e ) {
        LOG_WARN_MSG( e.what() );
        error_out = Error::ConnectError;
    }

    return portConfig::SocketPtr();
}


void
Connector::async_connect(
        ConnectHandler connect_handler
    )
{
    // create context
    _context_ptr = _port_config.createSslConfiguration().createContext( _io_service );

    // create socket
    portConfig::SocketPtr socket_ptr( new portConfig::Socket( _io_service, *_context_ptr ) );

    // start connecting
    PairConnector::Ptr pair_connector_ptr(
            new PairConnector(
                _io_service,
                _port_config.getPairs(),
                socket_ptr->next_layer()
                )
            );

    pair_connector_ptr->start(
            bind(
                &Connector::_connectHandler,
                this,
                socket_ptr,
                connect_handler,
                _1,
                _2
                )
            );
}


void
Connector::_connectHandler(
        portConfig::SocketPtr socket_ptr,
        ConnectHandler connect_handler,
        Error::Type err,
        const std::string& error_str
    )
{
    if ( err != Error::Success ) {
        // TCP connect failed.

        socket_ptr.reset();

        string no_server_cn;
        connect_handler( ConnectResult( socket_ptr, err, error_str, no_server_cn ) );

        return;
    }

    // Socket connected, now start handshake.
    Handshaker::create()->handshake(
            socket_ptr,
            _port_config,
            boost::bind(
                &Connector::_handleHandshake,
                this,
                _1,
                connect_handler,
                _2,
                _3
                )
            );
}


void
Connector::_handleHandshake(
        portConfig::SocketPtr socket_ptr,
        ConnectHandler connect_handler,
        const std::string& message,
        const std::string& server_cn
    )
{
    if ( !socket_ptr ) {
        Error::Type connector_err(Error::HandshakeError);

        connect_handler( ConnectResult( socket_ptr, connector_err, message, server_cn ) );
    } else {
        connect_handler( ConnectResult( socket_ptr, Error::Success, std::string(), server_cn ) );
    }
}


} // namespace bgq::utility
} // namespace bgq
