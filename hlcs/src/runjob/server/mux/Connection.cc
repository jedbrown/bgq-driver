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
#include "server/mux/Connection.h"

#include "common/message/convert.h"

#include "common/logging.h"

#include "server/job/Container.h"
#include "server/job/Input.h"
#include "server/job/Signal.h"

#include "server/mux/InsertJob.h"
#include "server/mux/ClientContainer.h"
#include "server/mux/Input.h"
#include "server/mux/Kill.h"
#include "server/mux/Reconnect.h"
#include "server/mux/StartJob.h"
#include "server/mux/StartTool.h"

#include "server/Ras.h"

#include "server/performance/Counters.h"

#include <boost/assert.hpp>
#include <boost/bind.hpp>

namespace runjob {
namespace server {
namespace mux {

LOG_DECLARE_FILE( runjob::server::log );

Connection::Connection(
        const bgq::utility::portConfig::SocketPtr& socket,
        const Server::Ptr& server
        ) :
    runjob::server::Connection( socket, server ),
    _strand( server->getIoService() ),
    _incomingHeader(),
    _outgoingHeader(),
    _outbox(),
    _clients( ClientContainer::create(server->getIoService()) ),
    _handshakeComplete( false )
{
    // zero headers
    bzero( &_incomingHeader, sizeof(_incomingHeader) );
    
    LOG_INFO_MSG( socket->lowest_layer().remote_endpoint() );
}

Connection::~Connection()
{
    LOGGING_DECLARE_LOCATION_MDC( _shortHostname );
    LOG_INFO_MSG( __FUNCTION__ << " " << _outbox.size() );
    BOOST_FOREACH( const MessagePair& i, _outbox ) {
        const WriteCallback& callback = i.second;
        if ( callback ) callback();
    }
}

void
Connection::readHeader()
{
    boost::asio::async_read(
            *_socket,
            boost::asio::buffer(
                &_incomingHeader,
                sizeof(_incomingHeader)
                ),
            boost::bind(
                &Connection::readHeaderHandler,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
                )
            );
}

void
Connection::readHeaderHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _shortHostname );
    LOG_TRACE_MSG( "read header handler " << bytesTransferred << " bytes" );

    if ( error ) {
        LOG_ERROR_MSG( "could not read: " << boost::system::system_error(error).what() );
        _clients->eof();

        return;
    }

    // special case for Handshake message since there is no body
    if (
            _incomingHeader._type == Message::Handshake ||
            _incomingHeader._version != bgq::utility::SvnRevision
       )
    {
        const message::Result::Ptr result(
                new message::Result()
                );
        if ( _incomingHeader._version != bgq::utility::SvnRevision ) {
            result->setError( error_code::handshake_failure );
            result->setMessage(
                    "our version (" + boost::lexical_cast<std::string>(bgq::utility::Revision) + ") "
                    "your version (" + boost::lexical_cast<std::string>(_incomingHeader._version) + ")"
                    );
            LOG_WARN_MSG( result->getMessage() );
        } else {
            _handshakeComplete = true;
            LOG_INFO_MSG( "version handshake (" << bgq::utility::Revision << ") passed" );

            const Server::Ptr server( _server.lock() );
            if ( !server ) return;

            Reconnect::create(
                    server,
                    shared_from_this()
                    );
            
            this->readHeader();
        }

        this->write( result );

        return;
    } else if ( !_handshakeComplete ) {
        LOG_WARN_MSG( "version handshake skipped, disconnecting" );
        return;
    }

    // start reading message body
    boost::asio::async_read(
            *_socket,
            _incomingMessage.prepare( _incomingHeader._length ),
            boost::bind(
                &Connection::readBodyHandler,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
                )
            );
}

void
Connection::readBodyHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _shortHostname );
    LOG_TRACE_MSG( "read body handler " << bytesTransferred << " bytes" );
    _incomingMessage.commit( bytesTransferred );

    if ( error ) {
        LOG_ERROR_MSG( "could not read: " << boost::system::system_error(error).what() );
        _clients->eof();

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

    // ensure message is valid
    if ( !message) {
        LOG_ERROR_MSG( "invalid message" );
    } else {
        this->handle( message );
    }

    // get next header
    this->readHeader();
}

void
Connection::writeHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _shortHostname );
    LOG_TRACE_MSG( "write handler " << bytesTransferred << " bytes" );
    _outgoingMessage.consume( bytesTransferred );

    // invoke callback and remove previous message from queue
    const WriteCallback& callback = _outbox[0].second;
    if ( callback ) callback();
    _outbox.pop_front();
    LOG_TRACE_MSG( "queue size: " << _outbox.size() );

    if ( error ) {
        LOG_ERROR_MSG( "could not write: " << boost::system::system_error(error).what() );
        return;
    }

    // start another send if outbox is not empty
    if ( !_outbox.empty() ) {
        this->write();
    }
}

void
Connection::write(
        const runjob::Message::Ptr& msg,
        const WriteCallback& callback
        )
{
    _strand.post(
            boost::bind(
                &Connection::writeImpl,
                shared_from_this(),
                msg,
                callback
                )
            );
}

void
Connection::writeImpl(
        const runjob::Message::Ptr& msg,
        const WriteCallback& callback
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _shortHostname );
    
    _outbox.push_back( std::make_pair(msg, callback) );

    // if there's a previous message, let it finish sending
    if ( _outbox.size() > 1 ) {
        return;
    }

    this->write();
}

void
Connection::write()
{
    // get first message in queue
    const runjob::Message::Ptr& msg = _outbox[0].first;
    std::ostream os( &_outgoingMessage );
    msg->serialize( os );
    LOG_TRACE_MSG( "sending " << runjob::Message::toString( msg->getType() ) << " message" );

    // create header
    _outgoingHeader._type = msg->getType();
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
                    &Connection::writeHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                    )
                )
            );
}

void
Connection::handle(
        const runjob::Message::Ptr& msg
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _shortHostname );
    LOGGING_DECLARE_JOB_MDC( msg->getClientId() );
    LOG_DEBUG_MSG( "received " << runjob::Message::toString(msg->getType()) << " message" );

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    if ( msg->getType() == runjob::Message::InsertJob ) {
        InsertJob::create()->
            // named parameter idiom
            server( server )->
            mux( shared_from_this() )->
            message( msg )->
            hostname( _hostname, _shortHostname )->
            clients( _clients )->
            start()
            ;
    } else if ( msg->getType() == runjob::Message::StdIn ) {
        Input::create( server, msg, _shortHostname );
    } else if ( msg->getType() == runjob::Message::KillJob ) {
        Kill::create()->
            // named parameter idiom
            message( msg )->
            clients( _clients )->
            hostname( _shortHostname )->
            start()
            ;
    } else if ( msg->getType() == runjob::Message::StartJob ) {
        StartJob::create( server, msg, _shortHostname );
    } else if ( msg->getType() == runjob::Message::StartTool ) {
        StartTool::create( server, msg, shared_from_this() );
    } else if ( msg->getType() == runjob::Message::PerfCounters ) {
        const message::PerfCounters::Ptr counters = boost::static_pointer_cast<message::PerfCounters>(msg);

        BOOST_FOREACH( const bgq::utility::performance::DataPoint& i, counters->getData() ) {
            server->getPerformanceCounters()->getJobs()->add( i );
        }
    } else if ( msg->getType() == runjob::Message::InsertRas ) {
        const message::InsertRas::Ptr ras = boost::static_pointer_cast<message::InsertRas>(msg);
        LOG_DEBUG_MSG( ras->_details[ "USER" ] );
        LOG_DEBUG_MSG( ras->_details[ "COMMAND" ] );
        Ras::create( Ras::AdministrativeAuthorityDenied ).
            detail( "USER", ras->_details["USER"] ).
            detail( "COMMAND", "runjob_mux " + this->hostname() + " command: " + ras->_details["COMMAND"] )
            ;
    } else {
        LOG_ERROR_MSG( "cannot handle message, yet..." );
    }
}

} // mux
} // server
} // runjob
