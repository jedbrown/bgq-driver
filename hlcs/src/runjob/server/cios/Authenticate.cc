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
#include "server/cios/Authenticate.h"

#include "common/logging.h"
#include "common/properties.h"

#include "server/block/SecurityKey.h"

#include "server/Options.h"
#include "server/Server.h"

#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/MessageUtility.h>
#include <ramdisk/include/services/StdioMessages.h>

#include <boost/assert.hpp>
#include <boost/bind.hpp>

#include <sstream>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace cios {

/*
<rasevent
  id="00062001" 
  category="Block" 
  component="MMCS"
  severity="FATAL"
  message="Failed to authenticate with the CIOS $(DAEMON) daemon running on I/O node $(BG_LOC)."
  description="A challenge/response handshake failed between the Control System and a daemon running on an I/O node."
  service_action="Check I/O node log for possible causes of this failure. Verify the I/O node's IP address is correct in the database and the block security key has not been modified, then reboot the I/O block."
  control_action="SOFTWARE_IN_ERROR"
 />
*/

Authenticate::Ptr
Authenticate::create(
        const Server::Ptr& server,
        const uint8_t service,
        const Uci& location
        )
{
    const boost::shared_ptr<Authenticate> result(
            new Authenticate(
                server,
                service,
                location
                )
            );

    return result;
}

Authenticate::Authenticate(
        const Server::Ptr& server,
        const uint8_t service,
        const Uci& location
        ) :
    _server( server ),
    _options( server->getOptions() ),
    _strand( server->getIoService() ),
    _timer( server->getIoService() ),
    _service( service ),
    _location( location ),
    _socket( ),
    _callback( ),
    _request( ),
    _header( ),
    _ack( cios::Message::create() ),
    _timedOut( false )
{

}

Authenticate::~Authenticate()
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOGGING_DECLARE_BLOCK_MDC( _service == bgcios::JobctlService ? "jobctl" : "stdio" );
    LOG_TRACE_MSG( __FUNCTION__ );
}

void
Authenticate::start(
        const ConnectionInterval::SocketPtr& socket,
        const block::SecurityKey& securityKey,
        const ConnectionInterval::Callback& callback
        )
{
    _socket = socket;
    _callback = callback;

    switch ( _service ) {
        case bgcios::JobctlService:
            _request = Message::create( bgcios::jobctl::Authenticate, 0 /* job ID */ );
            memcpy(
                    _request->as<bgcios::jobctl::AuthenticateMessage>()->plainData,
                    securityKey.clear().data(),
                    bgcios::jobctl::PlainDataSize
                  );
            memcpy(
                    _request->as<bgcios::jobctl::AuthenticateMessage>()->encryptedData,
                    securityKey.encrypted().data(),
                    bgcios::jobctl::EncryptedDataSize
                  );
            break;
        case bgcios::StdioService:
            _request = Message::create( bgcios::stdio::Authenticate, 0 /* job ID */ );
            memcpy(
                    _request->as<bgcios::stdio::AuthenticateMessage>()->plainData,
                    securityKey.clear().data(),
                    bgcios::stdio::PlainDataSize
                  );
            memcpy(
                    _request->as<bgcios::stdio::AuthenticateMessage>()->encryptedData,
                    securityKey.encrypted().data(),
                    bgcios::stdio::EncryptedDataSize
                  );
            break;
        default:
            BOOST_ASSERT( !"unhandled service" );
    }

    // start a deadline timer to time out if we never get a response
    if ( int timeout = this->getTimeout() ) {
        _timer.expires_from_now( 
                boost::posix_time::seconds(timeout)
                );

        _timer.async_wait(
                _strand.wrap(
                    boost::bind(
                        &Authenticate::timerHandler,
                        shared_from_this(),
                        boost::asio::placeholders::error
                        )
                    )
                );
    }

    boost::asio::async_write(
            *_socket,
            _request->buffer(),
            _strand.wrap(
                boost::bind(
                    &Authenticate::writeHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error
                    )
                )
            );
}

void
Authenticate::writeHandler(
        const boost::system::error_code& error
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOGGING_DECLARE_BLOCK_MDC( _service == bgcios::JobctlService ? "jobctl" : "stdio" );
    LOG_TRACE_MSG( __FUNCTION__ );

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    if ( server->stopped() ) {
        LOG_TRACE_MSG( "server is shutting down" );
        return;
    }

    if ( !_timedOut && error == boost::asio::error::operation_aborted ) {
        // skip callback since this isn't a failure
        LOG_DEBUG_MSG( boost::system::system_error(error).what() );
        return;
    } else if ( error ) {
        LOG_ERROR_MSG( "write failed: " << boost::system::system_error(error).what());
        _callback( ConnectionInterval::SocketPtr() );
        return;
    }

    boost::asio::async_read(
            *_socket,
            boost::asio::buffer(
                &_header,
                sizeof(_header)
                ),
            _strand.wrap(
                boost::bind(
                    &Authenticate::readHeaderHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error
                    )
                )
            );
}

void
Authenticate::readHeaderHandler(
        const boost::system::error_code& error
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOGGING_DECLARE_BLOCK_MDC( _service == bgcios::JobctlService ? "jobctl" : "stdio" );
    LOG_TRACE_MSG( __FUNCTION__ << " " << boost::system::system_error(error).what() );
    
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    if ( server->stopped() ) {
        LOG_TRACE_MSG( "server is shutting down" );
        return;
    }

    if ( !_timedOut && error == boost::asio::error::operation_aborted ) {
        // skip callback since this isn't a failure
        LOG_DEBUG_MSG( boost::system::system_error(error).what() );
        return;
    } else if ( error ) {
        LOG_ERROR_MSG( "read failed: " << boost::system::system_error(error).what());
        _callback( ConnectionInterval::SocketPtr() );
        return;
    }

    LOG_TRACE_MSG( "read " << bgcios::printHeader(_header) );

    if ( _service == bgcios::JobctlService && _header.type != bgcios::jobctl::AuthenticateAck ) {
        LOG_ERROR_MSG(
                "expected " << bgcios::jobctl::toString( bgcios::jobctl::AuthenticateAck ) << 
                " but got " << bgcios::jobctl::toString( _header.type )
                );

        _callback( ConnectionInterval::SocketPtr() );
        return;
    } else if ( _service == bgcios::StdioService && _header.type != bgcios::stdio::AuthenticateAck ) {
        LOG_ERROR_MSG(
                "expected " << bgcios::stdio::toString( bgcios::stdio::AuthenticateAck ) << 
                " but got " << bgcios::stdio::toString( _header.type )
                );

        _callback( ConnectionInterval::SocketPtr() );
        return;
    }

    // start read for data size
    boost::asio::async_read(
            *_socket,
            boost::asio::buffer( _ack->prepare(_header) ),
            _strand.wrap(
                boost::bind(
                    &Authenticate::readDataHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error
                    )
                )
            );
}

void
Authenticate::readDataHandler(
        const boost::system::error_code& 
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOGGING_DECLARE_BLOCK_MDC( _service == bgcios::JobctlService ? "jobctl" : "stdio" );
    LOG_TRACE_MSG( __FUNCTION__ );

    // stop timer
    boost::system::error_code error;
    _timer.cancel( error );
    if ( error ) {
        LOG_WARN_MSG( "could not cancel timer: " << boost::system::system_error(error).what() );
    }

    _callback( _socket );
}

void
Authenticate::timerHandler(
        const boost::system::error_code& error
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOGGING_DECLARE_BLOCK_MDC( _service == bgcios::JobctlService ? "jobctl" : "stdio" );
    LOG_TRACE_MSG( __FUNCTION__ << " " << boost::system::system_error(error).what() );

    if ( error == boost::asio::error::operation_aborted ) {
        // do nothing, we were asked to cancel
        return;
    }

    if ( error ) {
        LOG_ERROR_MSG( "wait failed:  " << boost::system::system_error(error).what() );
        return;
    }

    try {
        _timedOut = true;
        _socket->cancel();
    } catch ( const boost::system::system_error& e ) {
        LOG_WARN_MSG( "could not cancel socket: " << e.what() );
    }
}

void
Authenticate::stop()
{
    _strand.post(
            boost::bind(
                &Authenticate::stopImpl,
                shared_from_this()
                )
            );
}

void
Authenticate::stopImpl()
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOGGING_DECLARE_BLOCK_MDC( _service == bgcios::JobctlService ? "jobctl" : "stdio" );
    LOG_TRACE_MSG( __FUNCTION__ );

    boost::system::error_code error;

    _timer.cancel( error );
    if ( error ) {
        LOG_WARN_MSG( "could not cancel timer: " << boost::system::system_error(error).what() );
    }

    if ( _socket ) {
        _socket->cancel( error );
        if ( error ) {
            LOG_WARN_MSG( "could not cancel socket: " << boost::system::system_error(error).what() );
        }
    }
}

int
Authenticate::getTimeout() const
{
    int result = 0;
    try {
        const std::string key = "cios_authenticate_timeout";
        const std::string value = _options.getProperties()->getValue(
                PropertiesSection, key
                );
        result = boost::lexical_cast<int>( value );
    } catch ( const std::invalid_argument& e ) {
        // missing key is ok since this key is hidden
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    LOG_TRACE_MSG( "timeout " << result );
    return result;
}

} // cios
} // server
} // runjob
