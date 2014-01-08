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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#include "McServerConnection.h"

#include "common/Properties.h"

#include <control/include/mcServer/defaults.h>

#include <utility/include/portConfiguration/Connector.h>
#include <utility/include/Log.h>
#include <xml/include/c_api/MCServerMessageSpec.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using mmcs::common::Properties;

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace env {

McServerConnection::Ptr
McServerConnection::create(
        boost::asio::io_service& io_service
        )
{
    const Ptr result(
            new McServerConnection(
                io_service
                )
            );

    return result;
}

McServerConnection::McServerConnection(
        boost::asio::io_service& io_service
        ) :
    _io_service( io_service ),
    _socket( ),
    _outgoingHeader( ),
    _outgoingHeaderSize( 0 ),
    _outgoingMessage( ),
    _outgoingMessageSize( 0 ),
    _incomingHeader( ),
    _incomingHeaderSize( 0 ),
    _incomingMessage( ),
    _incomingMessageSize( 0 )
{

}

void
McServerConnection::start(
        const std::string& user,
        const ConnectCallback& callback
        )
{
    using bgq::utility::ClientPortConfiguration;
    using bgq::utility::Connector;
    using bgq::utility::PortConfiguration;

    ClientPortConfiguration config(
            mc_server::ClientPort,
            ClientPortConfiguration::ConnectionType::Administrative
            );
    config.setProperties( Properties::getProperties(), std::string() );
    PortConfiguration::Pairs portpairs;
    PortConfiguration::parsePortsStr(
            Properties::getProperty(MC_SERVER_IP) + ":" + Properties::getProperty(MC_SERVER_PORT),
            boost::lexical_cast<std::string>(mc_server::ClientPort),
            portpairs
            );
    config.setPairs( portpairs );
    config.notifyComplete();

    const boost::shared_ptr<Connector> connector(
            new Connector(
                _io_service,
                config
                )
            );

    connector->async_connect(
            boost::bind(
                &McServerConnection::connectHandler,
                shared_from_this(),
                _1,
                connector,
                callback,
                user
                )
            );
}

McServerConnection::~McServerConnection()
{
    // LOG_TRACE_MSG( __FUNCTION__ );
}

void
McServerConnection::connectHandler(
        const bgq::utility::Connector::ConnectResult& result,
        const boost::shared_ptr<bgq::utility::Connector>& connector,
        const ConnectCallback& callback,
        const std::string& user
        )
{
    if ( result.error ) {
        callback( result.error, result.error_str );
        return;
    }

    LOG_DEBUG_MSG("Connected to mc_server at " << result.socket_ptr->lowest_layer().remote_endpoint() );
    _socket = result.socket_ptr;

    MCServerMessageSpec::ConnectRequest request;
    request._version = boost::lexical_cast<unsigned>(MCServerMessageSpec::VERSION);
    request._user = "env mon " + user;
    request._hostname = boost::lexical_cast<std::string>( result.socket_ptr->lowest_layer().local_endpoint().address() );

    LOG_DEBUG_MSG("Connecting as " << request._user << "@" << request._hostname );
    this->send(
            request.getClassName(),
            request,
            boost::bind(
                &McServerConnection::connectRequestHandler,
                shared_from_this(),
                _1,
                callback
                )
            );
}

void
McServerConnection::connectRequestHandler(
        std::istream& response,
        const ConnectCallback& callback
        )
{
    MCServerMessageSpec::ConnectReply reply;
    reply.read( response );
    if ( reply._rc ) {
        LOG_ERROR_MSG( reply._rt );
        callback( bgq::utility::Connector::Error::ConnectError, reply._rt );
        return;
    }

    callback( bgq::utility::Connector::Error::Success, std::string() );
}

void
McServerConnection::send(
        const std::string& name,
        const XML::Serializable& request,
        const SendCallback& callback
        )
{
    _outgoingHeader = name;
    _outgoingHeaderSize = _outgoingHeader.size();
    std::ostream os( &_outgoingMessage );
    os << request;
    _outgoingMessageSize = _outgoingMessage.size();

    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back( boost::asio::buffer(&_outgoingHeaderSize, sizeof(_outgoingHeaderSize)) );
    buffers.push_back( boost::asio::buffer(_outgoingHeader) );
    buffers.push_back( boost::asio::buffer(&_outgoingMessageSize, sizeof(_outgoingMessageSize)) );
    buffers.push_back( _outgoingMessage.data() );

    boost::asio::async_write(
            *_socket,
            buffers,
            boost::bind(
                &McServerConnection::writeHandler,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred,
                callback
                )
            );
}

void
McServerConnection::writeHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred,
        const SendCallback& callback
        )
{
    _outgoingMessage.consume( bytesTransferred );

    if ( error ) {
        LOG_WARN_MSG("Could not write: " << boost::system::system_error(error).what() );
        return;
    }
    LOG_TRACE_MSG( __FUNCTION__ << " " << bytesTransferred << " bytes transferred" );

    boost::asio::async_read(
            *_socket,
            boost::asio::buffer( &_incomingHeaderSize, sizeof(_incomingHeaderSize) ),
            boost::bind(
                &McServerConnection::readHeaderSizeHandler,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred,
                callback
                )
            );
}

void
McServerConnection::readHeaderSizeHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred,
        const SendCallback& callback
        )
{
    if ( error ) {
        LOG_ERROR_MSG("Could not read header size: " << boost::system::system_error(error).what());
        return;
    }
    LOG_TRACE_MSG( __FUNCTION__ << " " << bytesTransferred << " bytes transferred" );

    _incomingHeader.resize( _incomingHeaderSize );
    boost::asio::async_read(
            *_socket,
            boost::asio::buffer( &_incomingHeader[0], _incomingHeaderSize ),
            boost::bind(
                &McServerConnection::readHeaderHandler,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred,
                callback
                )
            );
}

void
McServerConnection::readHeaderHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred,
        const SendCallback& callback
        )
{
    if ( error ) {
        LOG_ERROR_MSG("Could not read header: " << boost::system::system_error(error).what());
        return;
    }
    LOG_TRACE_MSG( __FUNCTION__ << " " << bytesTransferred << " bytes transferred" );

    boost::asio::async_read(
            *_socket,
            boost::asio::buffer( &_incomingMessageSize, sizeof(_incomingMessageSize) ),
            boost::bind(
                &McServerConnection::readMessageSizeHandler,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred,
                callback
                )
            );
}

void
McServerConnection::readMessageSizeHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred,
        const SendCallback& callback
        )
{
    if ( error ) {
        LOG_ERROR_MSG("Could not read message size: " << boost::system::system_error(error).what());
        return;
    }
    LOG_TRACE_MSG( __FUNCTION__ << " " << bytesTransferred << " bytes transferred" );

    boost::asio::async_read(
            *_socket,
            _incomingMessage.prepare( _incomingMessageSize ),
            boost::bind(
                &McServerConnection::readMessageHandler,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred,
                callback
                )
            );
}

void
McServerConnection::readMessageHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred,
        const SendCallback& callback
        )
{
    _incomingMessage.commit( bytesTransferred );
    if ( error ) {
        LOG_ERROR_MSG("Could not read message: " << boost::system::system_error(error).what());
        return;
    }
    LOG_TRACE_MSG( __FUNCTION__ << " " << bytesTransferred << " bytes transferred" );

    std::istream is( &_incomingMessage );
    callback( is );
}

} } } // namespace mmcs::server::env
