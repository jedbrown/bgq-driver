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
#include "client/MuxConnection.h"

#include "client/options/Parser.h"

#include "client/Job.h"

#include "common/message/convert.h"

#include "common/defaults.h"
#include "common/logging.h"

#include <boost/bind.hpp>

#include <vector>

LOG_DECLARE_FILE( runjob::client::log );

namespace runjob {
namespace client {

MuxConnection::MuxConnection(
        boost::asio::io_service& io_service,
        const boost::shared_ptr<Job>& job,
        const boost::shared_ptr<const options::Parser>& options
        ) :
    _options( options ),
    _job( job ),
    _socket( io_service ),
    _io_service( io_service ),
    _incomingHeader(),
    _incomingMessage(),
    _outbox(),
    _outgoingHeader(),
    _outgoingMessage()
{
    bzero( &_incomingHeader, sizeof(_incomingHeader) );
}

void
MuxConnection::start(
        const Callback& callback
        )
{
    // create remote endpoint
    std::string path( 1, '\0' ); // first byte is NULL for anonymous namespace
    const std::string socket = _options->getSocket();
    path.append( socket );
    LOG_DEBUG_MSG( "connecting to " << socket );
    const boost::asio::local::stream_protocol::endpoint ep( path );

    // start connect
    _socket.async_connect(
            ep,
            boost::bind(
                &MuxConnection::connectHandler,
                shared_from_this(),
                callback,
                boost::asio::placeholders::error
                )
            );
}

MuxConnection::~MuxConnection()
{
    LOG_DEBUG_MSG( __FUNCTION__ );
}

void
MuxConnection::write(
        const Message::Ptr& msg
        )
{
    _outbox.push_back( msg );

    // check if a send is in progress
    if ( _outbox.size() > 1 ) {
        LOG_TRACE_MSG("added message to queue, size " << _outbox.size());
        return;
    }

    this->writeImpl( msg );
}

void
MuxConnection::writeImpl(
        const Message::Ptr& msg
        )
{
    // serialize message
    std::ostream os( &_outgoingMessage );
    msg->serialize( os );

    // create header
    _outgoingHeader._type = msg->getType();
    _outgoingHeader._length = static_cast<uint32_t>(_outgoingMessage.size());

    // use gather-write to send both header and message at the same time
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back( boost::asio::buffer( &_outgoingHeader, sizeof(_outgoingHeader) ) );
    buffers.push_back( _outgoingMessage.data() );

    boost::asio::async_write(
            _socket,
            buffers,
            boost::bind(
                &MuxConnection::writeHandler,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
                )
            );
}

void
MuxConnection::connectHandler(
        const Callback& callback,
        const boost::system::error_code& error
        )
{
    LOGGING_DECLARE_JOB_MDC( _job->getId() );
    LOG_TRACE_MSG( "connect handler" );

    callback( error );

    if ( error ) return;

    // start reading
    boost::asio::async_read(
            _socket,
            boost::asio::buffer(
                &_incomingHeader,
                sizeof(_incomingHeader)
                ),
            boost::bind(
                &MuxConnection::readHeaderHandler,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
                )
            );
}

void
MuxConnection::readHeaderHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    LOGGING_DECLARE_JOB_MDC( _job->getId() );

    LOG_TRACE_MSG( "read header handler " << bytesTransferred << " bytes" );

    if ( error ) {
        LOG_FATAL_MSG( "could not read: " << boost::system::system_error(error).what() );

        _io_service.stop();
        return;
    }

    // start reading message
    boost::asio::async_read(
            _socket,
            _incomingMessage.prepare( _incomingHeader._length ),
            boost::bind(
                &MuxConnection::readBodyHandler,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
                )
            );
}

void
MuxConnection::readBodyHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    LOGGING_DECLARE_JOB_MDC( _job->getId() );

    LOG_TRACE_MSG( "read body handler " << bytesTransferred << " bytes" );
    _incomingMessage.commit( bytesTransferred );

    if ( error ) {
        LOG_ERROR_MSG( "could not read: " << error.message() );
        _io_service.stop();
        return;
    }

    LOG_TRACE_MSG( 
            "converting " << 
            Message::toString( 
                static_cast<Message::Type>(_incomingHeader._type) 
                )
            << " message"
            );
    const Message::Ptr message = message::convert(_incomingHeader, _incomingMessage);
    if ( !message ) {
        _io_service.stop();
        return;
    }

    // handle message
    if ( !_job->handle(message) ) {
        LOG_ERROR_MSG( "could not handle message" );
        _io_service.stop();
        return;
    }

    // get next header if we're not done
    if ( _job->getStatus() != Job::Terminated ) {
        boost::asio::async_read(
                _socket,
                boost::asio::buffer(
                    &_incomingHeader,
                    sizeof(_incomingHeader)
                    ),
                boost::bind(
                    &MuxConnection::readHeaderHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                    )
                );
    } else {
        // fall through
    }
}

void
MuxConnection::writeHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    LOGGING_DECLARE_JOB_MDC( _job->getId() );

    LOG_TRACE_MSG( "write handler " << bytesTransferred << " bytes" );
    _outgoingMessage.consume( bytesTransferred );
    _outbox.pop_front();

    if ( error ) {
        LOG_ERROR_MSG( 
                "could not write " << Message::toString(static_cast<Message::Type>(_outgoingHeader._type)) << 
                " message: " << boost::system::system_error(error).what()
                );
        _io_service.stop();
        return;
    }

    if ( !_outbox.empty() ) {
        // get next queued message
        const Message::Ptr msg = _outbox[0];
        this->writeImpl(msg);
    }
}

} // client
} // runjob
