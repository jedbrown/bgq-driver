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
#include "server/CommandConnection.h"

#include "common/commands/convert.h"

#include "common/logging.h"
#include "common/Exception.h"

#include "server/CommandHandler.h"

#include <hlcs/include/runjob/commands/Message.h>
#include <hlcs/include/runjob/commands/Response.h>

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {

CommandConnection::CommandConnection(
        const bgq::utility::portConfig::SocketPtr& socket,
        const bgq::utility::UserId::ConstPtr& user,
        const bgq::utility::portConfig::UserType::Value user_type,
        const Server::Ptr& server
        ) :
    Connection( socket, server ),
    _strand( socket->get_io_service() ),
    _incomingHeader(),
    _outbox(),
    _outgoingHeader(),
    _user( user ),
    _user_type( user_type )
{
    bzero( &_incomingHeader, sizeof(_incomingHeader) );
    bzero( &_outgoingHeader, sizeof(_outgoingHeader) );
}

CommandConnection::~CommandConnection()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

void
CommandConnection::readHeader()
{
    // clear incoming message buffer
    _incomingMessage.consume( _incomingMessage.size() );

    // start reading header
    boost::asio::async_read(
            *_socket,
            boost::asio::buffer(
                &_incomingHeader,
                sizeof(_incomingHeader)
                ),
            boost::bind(
                &CommandConnection::readHeaderHandler,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
                )
            );
}

void
CommandConnection::readHeaderHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _hostname );
    LOGGING_DECLARE_USER_MDC( _user->getUser() );
    LOG_TRACE_MSG( "read header handler " << bytesTransferred << " bytes" );

    if ( error == boost::asio::error::eof ) {
        LOG_INFO_MSG( "connection closed" );
        return;
    } else if ( error ) {
        LOG_ERROR_MSG( "could not read: " << boost::system::system_error(error).what() );
        return;
    }

    boost::asio::async_read(
            *_socket,
            _incomingMessage.prepare( _incomingHeader._length ),
            boost::bind(
                &CommandConnection::readBodyHandler,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
                )
            );
}

void
CommandConnection::readBodyHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _hostname );
    LOGGING_DECLARE_USER_MDC( _user->getUser() );
    LOG_TRACE_MSG( "read body handler " << bytesTransferred << " bytes" );
    _incomingMessage.commit( bytesTransferred );

    if ( error == boost::asio::error::eof ) {
        LOG_INFO_MSG( "connection closed" );
        return;
    } else if ( error ) {
        LOG_ERROR_MSG( "could not read: " << boost::system::system_error(error).what() );
        return;
    }

    LOG_TRACE_MSG( 
            "converting " << 
            runjob::commands::Message::toString( 
                static_cast<runjob::commands::Message::Tag::Type>(_incomingHeader._tag) 
                )
            << " messge"
            );

    runjob::commands::Message::Ptr request = runjob::commands::convert( _incomingHeader, _incomingMessage );
    if ( !request) {
        LOG_ERROR_MSG( "could not convert message into valid request" );
        return;
    }

    const Server::Ptr server( _server.lock() );
    if ( !server) return;

    CommandHandler::create(
            server,
            request,
            shared_from_this()
            );
}

void
CommandConnection::writeHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _hostname );
    LOGGING_DECLARE_USER_MDC( _user->getUser() );
    LOG_TRACE_MSG( "write handler " << bytesTransferred << " bytes" );
    _outgoingMessage.consume( bytesTransferred );

    // remove previous message from queue
    _outbox.pop_front();

    if ( error ) {
        LOG_ERROR_MSG( "could not write " << boost::system::system_error(error).what() );
        return;
    }

    // start another send if outbox is not empty
    if ( !_outbox.empty() ) {
        this->write();
    }
}

void
CommandConnection::write(
        const runjob::commands::Message::Ptr& msg
        )
{
    // need to wrap this in a strand since this method
    // can be invoked from multiple handlers
    _strand.post(
            boost::bind(
                &CommandConnection::writeImpl,
                shared_from_this(),
                msg
                )
            );
}

void
CommandConnection::writeImpl(
        const runjob::commands::Message::Ptr& msg
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _hostname );
    LOGGING_DECLARE_USER_MDC( _user->getUser() );
    // add to queue
    _outbox.push_back( msg );

    // if there's a previous message, let it finish sending
    if ( _outbox.size() > 1 ) {
        LOG_TRACE_MSG("added message to queue, size " << _outbox.size());
        return;
    }

    // send
    this->write();
}

void
CommandConnection::write()
{
    // get first message in queue
    runjob::commands::Message::Ptr msg = _outbox[0];

    // serialize message
    std::ostream os(&_outgoingMessage);
    msg->serialize(os);

    // create header
    bzero( &_outgoingHeader, sizeof(_outgoingHeader) );
    _outgoingHeader._type = msg->getType();
    _outgoingHeader._tag = msg->getTag();
    _outgoingHeader._length = static_cast<uint32_t>(_outgoingMessage.size());

    // use gather-write to send both header and message at the same time
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back( boost::asio::buffer(&_outgoingHeader, sizeof(_outgoingHeader) ) );
    buffers.push_back( _outgoingMessage.data() );
    boost::asio::async_write(
            *_socket,
            buffers,
            _strand.wrap(
                boost::bind(
                    &CommandConnection::writeHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                    )
                )
            );

    // get next header
    this->readHeader();
}

} // server
} // runjob
