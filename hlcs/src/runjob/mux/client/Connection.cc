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
#include "mux/client/Connection.h"

#include "common/message/convert.h"

#include "common/logging.h"

#include "mux/client/Message.h"
#include "mux/client/Status.h"

#include "mux/Multiplexer.h"

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::mux::log );

namespace runjob {
namespace mux {
namespace client {

Connection::Connection(
        const Multiplexer::Ptr& mux,
        const Id& id
        ) :
    _socket( mux->getIoService() ),
    _strand( mux->getIoService() ),
    _id( id ),
    _incomingHeader(),
    _incomingMessage(),
    _outbox( mux->getOptions(), id ),
    _terminating( false )
{
    bzero( &_incomingHeader, sizeof(_incomingHeader) );
}

Connection::~Connection()
{
    LOGGING_DECLARE_LOCATION_MDC( _id );
    LOG_DEBUG_MSG( __FUNCTION__ );
}

void
Connection::start(
        const MessageHandler& handler
        )
{
    boost::asio::async_read(
            _socket,
            boost::asio::buffer(
                &_incomingHeader,
                sizeof(_incomingHeader)
                ),
            boost::bind(
                &Connection::readHeaderHandler,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred,
                handler
                )
            );
}

void
Connection::stop()
{
    if ( !_socket.is_open() ) return;

    boost::system::error_code error;
    _socket.cancel( error );
    if ( error ) {
        LOG_ERROR_MSG( "could not cancel: " << boost::system::system_error(error).what() );
    }
}

void
Connection::readBodyHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred,
        const MessageHandler& handler
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _id );
    LOG_TRACE_MSG( __FUNCTION__ << " " << bytesTransferred << " bytes" );

    _incomingMessage.commit( bytesTransferred );

    if ( error ) {
        if ( error == boost::asio::error::eof && _terminating ) {
            LOG_TRACE_MSG( "lost connection" );
        } else {
            LOG_ERROR_MSG( "could not read: " << boost::system::system_error(error).what() );
        }

        handler( runjob::Message::Ptr(), error );
        return;
    }

    LOG_TRACE_MSG( 
            "converting " << 
            runjob::Message::toString( 
                static_cast<runjob::Message::Type>(_incomingHeader._type) 
                )
            << " message"
            );

    const runjob::Message::Ptr message = message::convert( _incomingHeader, _incomingMessage );
    handler( message, error );
}

void
Connection::readHeaderHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred,
        const MessageHandler& handler
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _id );
    LOG_TRACE_MSG( __FUNCTION__ << " " << bytesTransferred << " bytes " << boost::system::system_error(error).what() );

    // handle errors
    if ( error ) {
        if ( error == boost::asio::error::eof && _terminating ) {
            LOG_TRACE_MSG( "lost connection" );
        } else if ( error == boost::asio::error::operation_aborted ) {
            // asked to terminate
        } else {
            LOG_ERROR_MSG( "could not read: " << boost::system::system_error(error).what() );
        }

        handler( runjob::Message::Ptr(), error );
        return;
    }

    // version check
    if ( _incomingHeader._version != bgq::utility::SvnRevision ) {
        LOG_WARN_MSG(
                "version mismatch (" << _incomingHeader._version << " != " << bgq::utility::SvnRevision << ")"
                );

        handler( runjob::Message::Ptr(), boost::asio::error::operation_aborted );
        return;
    }

    LOG_TRACE_MSG( "message length " << _incomingHeader._length );

    // start reading message
    boost::asio::async_read(
            _socket,
            _incomingMessage.prepare( _incomingHeader._length ),
            boost::bind(
                &Connection::readBodyHandler,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred,
                handler
                )
            );
}

void
Connection::writeHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _id );
    LOG_TRACE_MSG( __FUNCTION__ << " " << bytesTransferred << " bytes" );

    // remove previous message from queue
    _outbox.pop_front();

    // handle errors
    if ( error ) {
        LOG_ERROR_MSG( "could not write " << error.message() );
        return;
    }

    // start another send if outbox is not empty
    if ( !_outbox.empty() ) {
        this->write();
    }
}

void
Connection::write(
        const runjob::Message::Ptr& message,
        const Status& status
        )
{
    _strand.post(
            boost::bind(
                &Connection::writeImpl,
                shared_from_this(),
                message,
                status
                )
            );
}

void
Connection::status(
        const runjob::commands::response::MuxStatus::Client& client,
        runjob::commands::response::MuxStatus::Ptr response,
        const StatusCallback& callback
        )
{
    _strand.post(
            boost::bind(
                &Connection::statusImpl,
                shared_from_this(),
                client,
                response,
                callback
                )
            );
}

void
Connection::writeImpl(
        const runjob::Message::Ptr& message,
        const Status& status
        )
{
    // remember that this connection is terminating
    if (
            !_terminating &&
            ( status == Status::Terminated || status == Status::Error )
       )
    {
        _terminating = true;
    }

    if ( !_outbox.empty() ) {
        // there's a previous message in flight, let it finish sending
        _outbox.push_back( message );
        return;
    }

    if ( !_outbox.push_back(message) ) {
        // message was dropped
        return;
    }

    this->write();
}

void
Connection::statusImpl(
        runjob::commands::response::MuxStatus::Client& client,
        const runjob::commands::response::MuxStatus::Ptr& response,
        const StatusCallback& callback
        )
{
    client._queueSize = _outbox.getCurrentSize();
    client._queueMaximumSize = _outbox.getMaximumSize();
    client._queueDropped = _outbox.getDropped();
    response->addClient( client );
    callback( response );
}

void
Connection::write()
{
    // get first message in queue
    const runjob::mux::client::Message::Ptr msg = _outbox.front();

    // use gather-write to send both header and message at the same time
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back( boost::asio::buffer(&msg->_header, sizeof(msg->_header)) );
    buffers.push_back( msg->_buffer.data() );
    boost::asio::async_write(
            _socket,
            buffers,
            _strand.wrap(
                boost::bind(
                    &Connection::writeHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                    )
                )
            );
}

} // client
} // mux
} // runjob
