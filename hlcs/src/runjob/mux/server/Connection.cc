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
#include "mux/server/Connection.h"

#include "common/message/convert.h"
#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include "mux/client/Container.h"
#include "mux/client/Runjob.h"

#include "mux/server/Handshake.h"

#include "mux/Options.h"

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::mux::log );

namespace runjob {
namespace mux {
namespace server {

Connection::Connection(
        boost::asio::io_service& io_service,
        const Options& options,
        const boost::shared_ptr<client::Container>& clients
        ) :
    _strand( io_service ),
    _socket(),
    _timer(),
    _incomingHeader(),
    _incomingMessage(),
    _outgoingHeader(),
    _outgoingMessage(),
    _outbox(),
    _options(options),
    _clients(clients)
{
    // zero headers 
    bzero( &_incomingHeader, sizeof(_incomingHeader) );
}

Connection::~Connection()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

void
Connection::start()
{
    _timer.reset(
            new Timer( _options, _strand )
            ),
    _timer->start(
            _strand.wrap(
                boost::bind(
                    &Connection::connectHandler,
                    shared_from_this(),
                    _1,
                    _2
                    )
                )
            );
}

void
Connection::connectHandler(
        const Timer::SocketPtr& socket,
        const boost::system::error_code& error
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );

    if ( error ) return;

    LOG_DEBUG_MSG( "connected to " << socket->next_layer().remote_endpoint() );
    _timer.reset();

    // disable Nagle algorithm
    boost::system::error_code nagleError;
    socket->lowest_layer().set_option(
            boost::asio::ip::tcp::no_delay(true),
            nagleError
            );
    if ( nagleError ) {
        LOG_WARN_MSG( "could not disable nagle algorithm: " << boost::system::system_error(nagleError).what() );
    }

    this->keepAlive( socket );

    Handshake::create(
            socket,
            boost::bind(
                &Connection::handshakeHandler,
                shared_from_this(),
                _1,
                socket
                )
            );

}

void
Connection::handshakeHandler(
        const error_code::rc error,
        const Timer::SocketPtr& socket
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );

    if ( error ) {
        // this is fatal, stopping the I/O service will cause the process to terminate
        socket->get_io_service().stop();
        return;
    }

    _socket = socket;

    boost::asio::async_read(
            *_socket,
            boost::asio::buffer(
                &_incomingHeader,
                sizeof(_incomingHeader)
                ),
            _strand.wrap(
                boost::bind(
                    &Connection::readHeaderHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                    )
                )
            );
}

void
Connection::readHeaderHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    LOG_TRACE_MSG( __FUNCTION__ << " " <<  bytesTransferred << " bytes" );
    if ( error ) {
        LOG_ERROR_MSG( "could not read " << boost::system::system_error(error).what() );
        this->handleEof();
        return;
    }

    LOG_TRACE_MSG( "message length " << _incomingHeader._length );

    boost::asio::async_read(
            *_socket,
            _incomingMessage.prepare( _incomingHeader._length ),
            _strand.wrap(
                boost::bind(
                    &Connection::readBodyHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                    )
                )
            );
}

void
Connection::readBodyHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    LOG_TRACE_MSG( __FUNCTION__ << " " << bytesTransferred << " bytes" );
    _incomingMessage.commit( bytesTransferred );

    if ( error ) {
        LOG_ERROR_MSG( "could not read " << boost::system::system_error(error).what() );
        this->handleEof();
        return;
    }

    LOG_TRACE_MSG( 
            "converting " << 
            runjob::Message::toString( 
                static_cast<runjob::Message::Type>(_incomingHeader._type) 
                )
            << " message"
            );
    const runjob::Message::Ptr msg = message::convert(_incomingHeader, _incomingMessage);
    BOOST_ASSERT( msg );

    _clients->find(
            client::Id( msg->getClientId() ),
            boost::bind(
                &Connection::findHandler,
                shared_from_this(),
                msg,
                _1
                )
            );
}

void
Connection::findHandler(
        const runjob::Message::Ptr& msg,
        const client::Runjob::Ptr& client
        )
{
    if ( !client ) {
        switch ( msg->getType() ) {
            case runjob::Message::StdError:
            case runjob::Message::StdOut:
                LOG_DEBUG_MSG( "could not find client " << msg->getClientId() );
                break;
            default:
                LOG_WARN_MSG( "could not find client " << msg->getClientId() );
                break;
        }
    } else {
        client->handle( msg );
    }

    boost::asio::async_read(
            *_socket,
            boost::asio::buffer(
                &_incomingHeader,
                sizeof(_incomingHeader)
                ),
            _strand.wrap(
                boost::bind(
                    &Connection::readHeaderHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                    )
                )
            );
}

void
Connection::writeHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    LOG_TRACE_MSG( __FUNCTION__ << " " << bytesTransferred << " bytes" );
    _outgoingMessage.consume( bytesTransferred );

    if ( _outbox.empty() ) {
        // outbox can be empty if the server disconnected
        return;
    }

    // get first message from queue and invoke callback
    const MessagePair& pair( _outbox[0] );
    const WriteCallback& callback( pair.second );
    if ( !callback.empty() ) {
        callback( error );
    }

    // remove front of queue
    _outbox.pop_front();

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
    // ensure we have a connection
    if ( !_socket ) {
        boost::system::error_code error(
                boost::system::errc::make_error_code(
                    boost::system::errc::resource_unavailable_try_again
                    )
                );

        if ( !callback.empty() ) {
            callback( error );
        }

        return;
    }

    // add to queue
    _outbox.push_back( 
            MessageQueue::value_type(msg, callback)
            );

    if ( _outbox.size() > 1 ) {
        // let previous message finish sending
        return;
    }

    this->write();
}

void
Connection::write()
{
    // get first message in queue
    const MessagePair& pair = _outbox[0];
    const runjob::Message::Ptr msg = pair.first;

    // serialize message
    std::ostream os(&_outgoingMessage);
    msg->serialize(os);

    // create header
    _outgoingHeader._type = msg->getType();
    _outgoingHeader._length = static_cast<uint32_t>(_outgoingMessage.size());

    // use gather-write to send both header and message at the same time
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back( boost::asio::buffer( &_outgoingHeader, sizeof(_outgoingHeader) ) );
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
Connection::status(
        const runjob::commands::response::MuxStatus::Ptr& response,
        const StatusCallback& callback
        )
{
    _strand.post(
            boost::bind(
                &Connection::statusImpl,
                shared_from_this(),
                response,
                callback
                )
            );
}

void
Connection::statusImpl(
        const runjob::commands::response::MuxStatus::Ptr& response,
        const StatusCallback& callback
        )
{
    if ( _socket ) {
        try {
            response->setServer(
                    _socket->next_layer().remote_endpoint()
                    );
        } catch ( const boost::system::system_error& e ) {
            LOG_WARN_MSG( e.what() );
        }
    }

    callback( response );
}

void
Connection::handleEof()
{
    _socket.reset();

    LOG_WARN_MSG( "draining " << _outbox.size() << " outbox entries" );
    for ( MessageQueue::iterator i = _outbox.begin(); i != _outbox.end(); ) {
        const MessagePair& pair = *i;
        const WriteCallback& callback = pair.second;

        if ( !callback.empty() ) {
            callback( 
                    boost::system::errc::make_error_code(
                        boost::system::errc::resource_unavailable_try_again
                        )
                    );
        }

        const MessagePtr& message = pair.first;
        LOG_WARN_MSG(
                "draining " << runjob::Message::toString( message->getType() ) << " message " <<
                "from client " << message->getClientId()
                );

        i = _outbox.erase( i );
    }

    this->start();
}

void
Connection::keepAlive(
        const Timer::SocketPtr& socket
        )
{
    const std::string keep_alive_key( "tcp_keep_alive" );
    bool enabled = true;
    try {
        const std::string value = _options.getProperties()->getValue(
                runjob::server::PropertiesSection,
                keep_alive_key
                );
        std::istringstream is( value );
        is >> std::boolalpha >> enabled;
        if ( !enabled ) {
            LOG_TRACE_MSG( "TCP keep alive disabled" );
            return;
        }
    } catch ( const std::exception& e ) {
        // key not found, assume enabled and fall through
    }

    boost::system::error_code error;
    socket->lowest_layer().set_option(
            boost::asio::socket_base::keep_alive( true ),
            error
            );
    if ( error ) {
        LOG_WARN_MSG( "could not enable TCP keep alive: " << boost::system::system_error(error).what() );
        return;
    }

    int wait = 300; // 5 minutes
    const std::string probe_wait_key( "tcp_keep_alive_probe_wait" );
    try {
        wait = boost::lexical_cast<int>(
                _options.getProperties()->getValue(
                    runjob::server::PropertiesSection,
                    probe_wait_key
                    )
                );
    } catch ( const std::exception& e ) {
        // not found, fall through
        LOG_TRACE_MSG( probe_wait_key << ": " << e.what() );
    }
    typedef boost::asio::detail::socket_option::integer<SOL_TCP, TCP_KEEPIDLE> TcpWait;
    socket->lowest_layer().set_option(
            TcpWait(wait),
            error
            );
    if ( error ) {
        LOG_WARN_MSG( "could not enable TCP keep alive first probe wait: " << boost::system::system_error(error).what() );
    }

    int interval = 15; // seconds
    const std::string probe_interval_key( "tcp_keep_alive_probe_interval" );
    try {
        interval = boost::lexical_cast<int>(
                _options.getProperties()->getValue(
                    runjob::server::PropertiesSection,
                    probe_interval_key
                    )
                );
    } catch ( const std::exception& e ) {
        // not found, fall through
        LOG_TRACE_MSG( probe_interval_key << ": " << e.what() );
    }
    typedef boost::asio::detail::socket_option::integer<SOL_TCP, TCP_KEEPINTVL> TcpInterval;
    socket->lowest_layer().set_option(
            TcpInterval(interval),
            error
            );
    if ( error ) {
        LOG_WARN_MSG( "could not enable TCP keep alive probe interval: " << boost::system::system_error(error).what() );
    }

    int count = 4;
    const std::string probe_count_key( "tcp_keep_alive_probe_count" );
    try {
        count = boost::lexical_cast<int>(
                _options.getProperties()->getValue(
                    runjob::server::PropertiesSection,
                    probe_count_key
                    )
                );
    } catch ( const std::exception& e ) {
        // not found, fall through
        LOG_TRACE_MSG( probe_count_key << ": " << e.what() );
    }
    typedef boost::asio::detail::socket_option::integer<SOL_TCP, TCP_KEEPCNT> TcpCount;
    socket->lowest_layer().set_option(
            TcpCount(count),
            error
            );
    if ( error ) {
        LOG_WARN_MSG( "could not enable TCP keep alive probe count: " << boost::system::system_error(error).what() );
    }
}

} // server
} // mux
} // runjob
