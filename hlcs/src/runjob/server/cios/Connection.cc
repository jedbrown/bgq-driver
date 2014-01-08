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
#include "server/cios/Connection.h"

#include "common/logging.h"
#include "common/properties.h"

#include "server/block/Io.h"

#include "server/cios/Authenticate.h"
#include "server/cios/ConnectionInterval.h"
#include "server/cios/Message.h"

#include "server/job/Handle.h"
#include "server/job/Container.h"

#include "server/Job.h"
#include "server/Options.h"
#include "server/Server.h"
#include "server/Ras.h"

#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/StdioMessages.h>
#include <ramdisk/include/services/MessageUtility.h>

#include <boost/system/system_error.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace cios {

Connection::Connection(
        const uint8_t service,
        const std::string& location,
        const boost::shared_ptr<Server>& server,
        const boost::shared_ptr<block::Io>& block
        ) :
    _server( server ),
    _service( service ),
    _options( server->getOptions() ),
    _socket( ),
    _strand( server->getIoService() ),
    _endpoint( ),
    _location( location ),
    _interval( ),
    _authenticate( ),
    _jobs( server->getJobs() ),
    _header(),
    _incoming(),
    _outbox(),
    _block( block ),
    _sequence( 0 ),
    _pulse( boost::posix_time::microsec_clock::local_time() )
{

}

Connection::~Connection()
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOGGING_DECLARE_BLOCK_MDC( _service == bgcios::JobctlService ? "jobctl" : "stdio" );
    LOG_TRACE_MSG( __FUNCTION__ );
}

void
Connection::start(
        const boost::asio::ip::tcp::endpoint& ep
        )
{
    _endpoint = ep;
    this->start();
}

void
Connection::start()
{
    // if our I/O block is no longer valid, it means we shouldn't
    // try to reconnect
    const block::Io::Ptr block( _block.lock() );
    if ( !block ) return;

    _socket.reset();
    _interval.reset(
            new ConnectionInterval(
                _options,
                _strand.get_io_service(),
                _location,
                block->name()
                )
            );
    _interval->start(
            _endpoint,
            _strand.wrap(
                boost::bind(
                    &Connection::connectHandler,
                    shared_from_this(),
                    _1
                    )
                )
            );
}

void
Connection::stop(
        const StopCallback& callback
        )
{
    _strand.post(
            boost::bind(
                &Connection::stopImpl,
                shared_from_this(),
                callback
                )
            );
}

void
Connection::status(
        const StatusCallback& callback
        )
{
    _strand.post(
            boost::bind(
                &Connection::statusImpl,
                shared_from_this(),
                callback
                )
            );
}

void
Connection::stopImpl(
        const StopCallback& callback
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOGGING_DECLARE_BLOCK_MDC( _service == bgcios::JobctlService ? "jobctl" : "stdio" );
    LOG_TRACE_MSG( __FUNCTION__ );

    // stop timer
    if ( _interval ) {
        _interval->stop();
    }

    // stop authenticate
    if ( _authenticate ) {
        _authenticate->stop();
    }

    // stop socket
    if ( _socket ) {
        boost::system::error_code error;
        _socket->cancel( error );
        if ( error ) {
            LOG_WARN_MSG( "could not cancel: " << boost::system::system_error(error).what() );
        }
    }

    if ( callback ) callback();
}

void
Connection::statusImpl(
        const StatusCallback& callback
        )
{
    callback( _socket );
}

void
Connection::read()
{
    // assume this handler is protected by _strand
    
    if ( !_socket ) return;

    // start reading header
    boost::asio::async_read(
            *_socket,
            boost::asio::buffer(
                &_header,
                sizeof(_header)
                ),
            boost::bind(
                &Connection::readHeaderHandler,
                shared_from_this(),
                boost::asio::placeholders::error
                )
            );
}

void
Connection::write(
        const Message::Ptr& msg
        )
{
    _strand.post(
            boost::bind(
                &Connection::writeImpl,
                shared_from_this(),
                msg
                )
            );
}

void
Connection::writeImpl(
        const Message::Ptr& msg
        )
{
    // assume this handler is protected by _strand

    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOGGING_DECLARE_BLOCK_MDC( _service == bgcios::JobctlService ? "jobctl" : "stdio" );

    if ( !_socket ) return;

    // add to queue
    _outbox.push_back( msg );

    // if there's a previous message, let it finish sending
    if ( _outbox.size() > 1 ) {
        LOG_TRACE_MSG( "added message to queue, size " << _outbox.size() );
        return;
    }

    // send
    this->write();
}

void
Connection::write()
{
    // get first message from queue
    const Message::Ptr msg = _outbox[0];
    
    // every outgoing message has a unique sequence ID
    msg->header()->sequenceId = _sequence++;

    if ( msg->header()->type == bgcios::jobctl::Heartbeat ) {
        if ( _pulse.is_not_a_date_time() ) {
            LOG_WARN_MSG( "has not responded to a Heartbeat" );

            const block::Io::Ptr block( _block.lock() );
            Ras::create( Ras::HeartbeatFailure ).
                detail( RasEvent::LOCATION, _location ).
                detail( "DAEMON", (_service == bgcios::JobctlService ? "jobctl" : "stdio") ).
                block( block ? block->name() : std::string() )
                ;

            _outbox.clear();
            _pulse = boost::posix_time::microsec_clock::local_time();

            this->start( _endpoint );
            _jobs->eof( _location );

            return;
        }

        const boost::posix_time::ptime now( boost::posix_time::microsec_clock::local_time() );
        LOG_DEBUG_MSG( "last message received " << (now - _pulse).total_seconds() << "s ago" );
        _pulse = boost::posix_time::not_a_date_time;
    }

    // log contents of header
    LOG_TRACE_MSG( "write " << bgcios::printHeader( *msg->header() ) );

    boost::asio::async_write(
            *_socket,
            msg->buffer(),
            _strand.wrap(
                boost::bind(
                    &Connection::writeHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error
                    )
                )
            );
}

void
Connection::connectHandler(
        const SocketPtr& socket
        )
{
    // assume this handler is protected by _strand

    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOGGING_DECLARE_BLOCK_MDC( _service == bgcios::JobctlService ? "jobctl" : "stdio" );
    LOG_TRACE_MSG( __FUNCTION__ );

    if ( !socket ) {
        LOG_WARN_MSG( "could not connect" );
        return;
    }

    const block::Io::Ptr block( _block.lock() );
    if ( !block ) return;

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    // disable Nagle algorithm
    boost::system::error_code error;
    socket->set_option(
                boost::asio::ip::tcp::no_delay( true ),
                error
            );
    if ( error ) {
        LOG_WARN_MSG( "could not enable TCP no delay: " << boost::system::system_error(error).what() );
    }

    this->keepAlive( socket );

    // done with connection interval
    _interval.reset();

    // begin authentication
    _authenticate = Authenticate::create( 
            server,
            _service,
            _location
            );
    _authenticate->start(
            socket,
            block->securityKey(),
            _strand.wrap(
                boost::bind(
                    &Connection::authenticateHandler,
                    shared_from_this(),
                    _1
                    )
                )
            );
}

void
Connection::authenticateHandler(
        const SocketPtr& socket
        )
{
    // assume this handler is protected by _strand

    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOGGING_DECLARE_BLOCK_MDC( _service == bgcios::JobctlService ? "jobctl" : "stdio" );
    LOG_TRACE_MSG( __FUNCTION__ );

    if ( !socket ) {
        const block::Io::Ptr block( _block.lock() );

        LOG_WARN_MSG( "could not authenticate" );
        Ras::create( Ras::CiosAuthenticationFailure ).
            detail( RasEvent::LOCATION, _location ).
            detail( "DAEMON", (_service == bgcios::JobctlService ? "jobctl" : "stdio") ).
            block( block ? block->name() : std::string() )
            ;

        return;
    }

    LOG_INFO_MSG( "authenticated" );

    _socket = socket;
    _authenticate.reset();

    this->read();
}

void
Connection::writeHandler(
        const boost::system::error_code& error
        )
{
    // assume this handler is protected by _strand

    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOGGING_DECLARE_BLOCK_MDC( _service == bgcios::JobctlService ? "jobctl" : "stdio" );
    LOG_TRACE_MSG( __FUNCTION__ );

    // remove previous message from queue
    _outbox.pop_front();

    // handle errors
    if ( error ) {
        LOG_ERROR_MSG( "write failed: " << boost::system::system_error(error).what());
        return;
    }

    // if non-empty, start another
    if ( !_outbox.empty() ) {
        this->write();
    }
}

void
Connection::readHeaderHandler(
        const boost::system::error_code& error
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOGGING_DECLARE_BLOCK_MDC( _service == bgcios::JobctlService ? "jobctl" : "stdio" );
    
    if ( error == boost::asio::error::operation_aborted ) {
        // do nothing, we were asked to terminate
        return;
    }
    
    if ( error ) {
        LOG_ERROR_MSG( "read failed: " << boost::system::system_error(error).what());
        this->start( _endpoint );
        
        // kill jobs using this I/O node
        _jobs->eof( _location );

        return;
    }

    // log header contents
    LOG_TRACE_MSG( __FUNCTION__ << " " << bgcios::printHeader(_header) );

    _incoming = cios::Message::create();

    // start read for data size
    boost::asio::async_read(
            *_socket,
            boost::asio::buffer( _incoming->prepare(_header) ),
            boost::bind(
                &Connection::readDataHandler,
                shared_from_this(),
                boost::asio::placeholders::error
                )
            );
}

void
Connection::readDataHandler(
        const boost::system::error_code& error
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOGGING_DECLARE_BLOCK_MDC( _service == bgcios::JobctlService ? "jobctl" : "stdio" );
    LOG_TRACE_MSG( __FUNCTION__ );
    
    if ( error == boost::asio::error::operation_aborted ) {
        // do nothing, we were asked to terminate
        return;
    }

    if ( error ) {
        LOG_ERROR_MSG( "read failed: " << boost::system::system_error(error).what());
        this->start( _endpoint );

        // kill jobs using this I/O node
        _jobs->eof( _location );

        return;
    }

    if ( _incoming->unhandled() )  {
        // don't handle this message since we don't know what it is
        this->read();
        return;
    }

    if ( _service == bgcios::JobctlService ) {
        _strand.post(
                boost::bind( &Connection::pulse, shared_from_this() )
                );
        return;
    }

    // get job object
    _jobs->find(
            _header.jobId,
            boost::bind(
                &Connection::findJobHandler,
                shared_from_this(),
                _1
                )
            );
}

void
Connection::findJobHandler(
        const Job::Ptr& job
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOGGING_DECLARE_BLOCK_MDC( _service == bgcios::JobctlService ? "jobctl" : "stdio" );
    LOG_TRACE_MSG( __FUNCTION__ );

    const uint16_t type = _header.type;
    const uint32_t returnCode = _header.returnCode;

    if ( !job ) {
        // special case for change config and reconnect ack due to no job ID
        if (
                type == bgcios::jobctl::ChangeConfigAck ||
                type == bgcios::stdio::ChangeConfigAck ||
                type == bgcios::jobctl::ReconnectAck ||
                type == bgcios::stdio::ReconnectAck
           )
        {
            if ( returnCode != bgcios::Success ) {
                LOG_WARN_MSG( 
                        (
                        _service == bgcios::JobctlService ? 
                        bgcios::jobctl::toString(type) : bgcios::stdio::toString(type)
                        ) << 
                        " failed with rc " << returnCode << ": " <<
                        bgcios::returnCodeToString( returnCode )
                        );
            } else {
                LOG_DEBUG_MSG(
                        (
                        _service == bgcios::JobctlService ? 
                        bgcios::jobctl::toString(type) : bgcios::stdio::toString(type)
                        ) << " success"
                        );
            }
        } else if ( 
                _service == bgcios::StdioService &&
                ( type == bgcios::stdio::WriteStdout || type == bgcios::stdio::WriteStderr )
                )
        {
            // we don't need to log stdout/stderr messages not found at a high severity
            LOG_DEBUG_MSG( "Could not find job: " << bgcios::printHeader(_header) );
        } else  {
            LOG_WARN_MSG( "Could not find job: " << bgcios::printHeader(_header) );
        }

        _strand.post( boost::bind(&Connection::read, shared_from_this()) );
        return;
    }

    job->strand().post(
            boost::bind(
                &Connection::handleJob,
                shared_from_this(),
                job,
                _incoming
                )
            );

    if ( _service == bgcios::JobctlService ) {
        // service connection immediately rather than callback after servicing job
        _strand.post( boost::bind(&Connection::read, shared_from_this()) );
    }
}

void
Connection::handleJob(
        const Job::Ptr& job,
        const cios::Message::Ptr& incoming
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOGGING_DECLARE_BLOCK_MDC( _service == bgcios::JobctlService ? "jobctl" : "stdio" );
    LOG_TRACE_MSG( __FUNCTION__ );

    job::Handle handle( job );

    if ( _service == bgcios::StdioService ) {
        handle.data(
                _location,
                incoming,
                _strand.wrap( 
                    boost::bind(
                        &Connection::read, 
                        shared_from_this()
                        )
                    )
                );
    } else if ( _service == bgcios::JobctlService ) {
        handle.control( _location, incoming );
    } else {
        BOOST_ASSERT( !"unhandled service" );
    }
}

void
Connection::keepAlive(
        const SocketPtr& socket
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
    socket->set_option(
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
    socket->set_option(
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
    socket->set_option(
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
    socket->set_option(
            TcpCount(count),
            error
            );
    if ( error ) {
        LOG_WARN_MSG( "could not enable TCP keep alive probe count: " << boost::system::system_error(error).what() );
    }
}

void
Connection::pulse()
{
    // assume this handler is protected by _strand

    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOGGING_DECLARE_BLOCK_MDC( _service == bgcios::JobctlService ? "jobctl" : "stdio" );

    _pulse = boost::posix_time::microsec_clock::local_time();

    if ( _header.type == bgcios::jobctl::HeartbeatAck ) {
        // log round trip time
        LOG_DEBUG_MSG( "round trip time " << time(NULL) - _header.jobId << "s" );
        this->read();
        return;
    }

    _jobs->find(
            _header.jobId,
            boost::bind(
                &Connection::findJobHandler,
                shared_from_this(),
                _1
                )
            );
}

} // cios
} // server
} // runjob
