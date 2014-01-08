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
#include "server/cios/ConnectionInterval.h"

#include "common/logging.h"
#include "common/properties.h"

#include "server/Options.h"

#include <db/include/api/tableapi/gensrc/DBVIoblock.h>
#include <db/include/api/tableapi/gensrc/DBVIonode.h>
#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/cxxdb/cxxdb.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace cios {

const unsigned ConnectionInterval::DefaultIntervalCeiling = 120;
const unsigned ConnectionInterval::DefaultIntervalIncrement = 5;

ConnectionInterval::ConnectionInterval(
        const Options& options,
        boost::asio::io_service& io_service,
        const Uci& location,
        const std::string& block
        ) :
    _strand( io_service ),
    _timer( io_service, boost::posix_time::seconds(0) ),
    _endpoint(),
    _socket( ),
    _location( location ),
    _interval( 0 ),
    _intervalCeiling( DefaultIntervalCeiling ),
    _intervalIncrement( DefaultIntervalIncrement ),
    _block( block )
{
    const std::string max_key( "io_connection_interval_max" );
    try {
        _intervalCeiling = boost::lexical_cast<int>(
                options.getProperties()->getValue(
                    runjob::server::PropertiesSection,
                    max_key
                    )
                );
        if ( _intervalCeiling <= 0 ) {
            LOG_WARN_MSG( "bad " << max_key << " value: " << _intervalCeiling );
            _intervalCeiling = DefaultIntervalCeiling;
            LOG_WARN_MSG( "using default value " << _intervalCeiling );
        }
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( "bad " << max_key << " value: " << e.what() );
        LOG_WARN_MSG( "using default value " << _intervalCeiling );
    } catch ( const std::invalid_argument& e ) {
        LOG_WARN_MSG(
                "could not find key " << max_key << " in " <<
                PropertiesSection << " section of properties"
                );
        LOG_WARN_MSG( "using default value " << _intervalCeiling );
    }

    const std::string increment_key( "io_connection_interval_increment" );
    try {
        _intervalIncrement = boost::lexical_cast<int>(
                options.getProperties()->getValue(
                    runjob::server::PropertiesSection,
                    increment_key
                    )
                );
        if ( _intervalIncrement <= 0 ) {
            LOG_WARN_MSG( "bad " << increment_key << " value: " << _intervalIncrement );
            _intervalIncrement = DefaultIntervalIncrement;
            LOG_WARN_MSG( "using default value " << _intervalIncrement );
        }
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( "bad " << increment_key << " value: " << e.what() );
        LOG_WARN_MSG( "using default value " << _intervalIncrement );
    } catch ( const std::invalid_argument& e ) {
        LOG_WARN_MSG(
                "could not find key " << increment_key << " in " <<
                PropertiesSection << " section of properties"
                );
        LOG_WARN_MSG( "using default value " << _intervalIncrement );
    }
}

ConnectionInterval::~ConnectionInterval()
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOG_TRACE_MSG( __FUNCTION__ );
}

void
ConnectionInterval::start(
        const boost::asio::ip::tcp::endpoint& ep,
        const Callback& callback
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );
    _socket.reset(
            new boost::asio::ip::tcp::socket(
                _strand.get_io_service()
                )
            );
    
    // remember our endpoint
    _endpoint = ep;
        
    // wait
    _timer.async_wait(
            _strand.wrap(
                boost::bind(
                    &ConnectionInterval::timerHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    callback
                    )
                )
            );
}

void
ConnectionInterval::stop()
{
    _strand.post(
            boost::bind(
                &ConnectionInterval::stopImpl,
                shared_from_this()
                )
            );
}

void
ConnectionInterval::stopImpl()
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
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

void
ConnectionInterval::timerHandler(
        const boost::system::error_code& error,
        const Callback& callback
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOG_TRACE_MSG( __FUNCTION__ );

    if ( error == boost::asio::error::operation_aborted ) {
        // do nothing, we were asked to cancel
        return;
    }

    if ( error ) {
        LOG_ERROR_MSG( "wait failed:  " << boost::system::system_error(error).what() );
        callback( SocketPtr() );
        return;
    }

    // ensure our I/O node's state is Available
    if ( !this->available() ) {
        LOG_DEBUG_MSG( "waiting for node state to become Available" );
        this->wait( callback );
        return;
    }

    // start connect
    LOG_DEBUG_MSG( "connecting to " << _endpoint );
    _socket->async_connect(
            _endpoint,
            _strand.wrap(
                boost::bind(
                    &ConnectionInterval::connectHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    callback
                    )
                )
            );

    // enable close on exec
    const int rc =  fcntl(
            _socket->native(),
            F_SETFD,
            fcntl(_socket->native(), F_GETFD) | FD_CLOEXEC
            );
    if ( rc == -1 ) {
        const int error = errno;
        LOG_WARN_MSG( "could not enable close on exec for descriptor " << _socket->native() << ": " << strerror(error) );
    }
}

void
ConnectionInterval::connectHandler(
        const boost::system::error_code& error,
        const Callback& callback
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOG_TRACE_MSG( __FUNCTION__ );

    if ( error == boost::asio::error::operation_aborted ) {
        // do nothing, we were asked to cancel
        return;
    }

    if ( error ) {
        LOG_ERROR_MSG( 
                "connection failed: " << boost::system::system_error(error).what()
                );

        this->wait( callback );
    } else {
        LOG_INFO_MSG( "connected successfully to " << _endpoint  );
        callback( _socket );
    }
}

void
ConnectionInterval::wait(
        const Callback& callback
        )
{
    // increment interval so we can wait a bit before retrying
    _interval += _intervalIncrement;
    if ( _interval > _intervalCeiling ) {
        _interval = _intervalCeiling;
    }
    LOG_DEBUG_MSG( "retrying in " << _interval << " seconds" );

    boost::system::error_code error;
    _timer.expires_from_now( 
            boost::posix_time::seconds(_interval),
            error
            );
    if ( error ) {
        LOG_WARN_MSG( "setting timer expiration: " << boost::system::system_error(error).what() );
    }

    _timer.async_wait(
            _strand.wrap(
                boost::bind(
                    &ConnectionInterval::timerHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    callback
                    )
                )
            );
}

bool
ConnectionInterval::available()
{
    try {
        const cxxdb::ConnectionPtr connection(
                BGQDB::DBConnectionPool::instance().getConnection()
            );
        if ( !connection ) {
            LOG_WARN_MSG( "could not get database connection" );
            return false;
        }

        // get node and block status, we need the node status to be Available
        // and the block status to be Initialized to proceed
        const cxxdb::ResultSetPtr results(
                connection->query(
                    "SELECT node." + BGQDB::DBVIonode::STATUS_COL + " as node, " + 
                    "block." + BGQDB::DBVIoblock::STATUS_COL + " as block FROM " +
                    BGQDB::DBVIonode().getTableName() + " node, " +
                    BGQDB::DBVIoblock().getTableName() + " block WHERE " +
                    BGQDB::DBVIonode::LOCATION_COL + "=" +
                    "'" + boost::lexical_cast<std::string>(_location) + "' AND " +
                    BGQDB::DBVIoblock::BLOCKID_COL + "=" +
                    "'" + _block + "'"
                    )
                );
        if ( !results->fetch() ) {
            LOG_WARN_MSG( "could not get block and node status" );
            return false;
        }

        const std::string nodeStatus( results->columns()["node"].getString() );
        const std::string blockStatus( results->columns()["block"].getString() );
        LOG_DEBUG_MSG( "node " << nodeStatus << " block " << blockStatus );

        return nodeStatus == BGQDB::HARDWARE_AVAILABLE && blockStatus == BGQDB::BLOCK_INITIALIZED;;
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( "could not get node status: " <<  e.what() );
        return false;
    }
}

} // cios
} // server
} // runjob
