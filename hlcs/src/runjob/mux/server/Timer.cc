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
#include "mux/server/Timer.h"

#include "common/logging.h"
#include "common/properties.h"

#include "mux/Options.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

LOG_DECLARE_FILE( runjob::mux::log );

namespace runjob {
namespace mux {
namespace server {

const unsigned Timer::DefaultInterval = 10;

Timer::Timer(
        const Options& options,
        boost::asio::strand& strand
        ) :
    _strand( strand ),
    _timer( strand.get_io_service() ),
    _options( options ),
    _connector( strand.get_io_service(), options.getClientPort() )
{

}

void
Timer::start(
        const Callback& callback
        )
{
    // start timer, we want it to trigger immediately since this
    // is our first attempt
    _timer.expires_from_now( boost::posix_time::seconds(0) );
        
    // wait
    _timer.async_wait(
            _strand.wrap(
                boost::bind(
                    &Timer::timerHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    callback
                    )
                )
            );
}

void
Timer::stop(
        boost::system::error_code& error
        )
{
    _timer.cancel( error );
}

unsigned
Timer::getInterval() const
{
    int result = DefaultInterval;
    const std::string interval_key( "server_connection_interval" );
    try {
        result = boost::lexical_cast<int>(
                _options.getProperties()->getValue(
                    runjob::mux::PropertiesSection,
                    interval_key
                    )
                );
        if ( result <= 0 ) {
            LOG_WARN_MSG( interval_key << " value must be positive: " << result );
            result = DefaultInterval;
            LOG_WARN_MSG( "using default value " << result );
        }
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( "bad " << interval_key << " value: " << e.what() );
        LOG_WARN_MSG( "using default value " << result );
    } catch ( const std::invalid_argument& e ) {
        LOG_WARN_MSG(
                "could not find key " << interval_key << " in " <<
                PropertiesSection << " section of properties"
                );
        LOG_WARN_MSG( "using default value " << result );
    }

    return result;
}

void
Timer::timerHandler(
        const boost::system::error_code& error,
        const Callback& callback
        )
{
    if ( error == boost::asio::error::operation_aborted ) {
        // do nothing, we were asked to cancel
    } else if ( error ) {
        LOG_ERROR_MSG( "wait failed:  " << boost::system::system_error(error).what() );
        callback( SocketPtr(), error );
    } else {
        // start connecting
        _connector.async_connect(
                _strand.wrap(
                    boost::bind(
                        &Timer::connectHandler,
                        shared_from_this(),
                        _1,
                        callback
                        )
                    )
                );
    }
}

void
Timer::connectHandler(
        const bgq::utility::Connector::ConnectResult& res,
        const Callback& callback
        )
{
    LOG_TRACE_MSG( "connect handler" );

    if ( res.error == bgq::utility::Connector::Error::Success ) {
        LOG_DEBUG_MSG( "connected to " << res.socket_ptr->next_layer().remote_endpoint() );

        boost::system::error_code error( 
                boost::system::errc::success, 
                boost::system::get_system_category()
                );
        callback( res.socket_ptr, error );
    } else {
        LOG_ERROR_MSG( 
                "connection failed: " << res.error_str
                );
        const unsigned interval = this->getInterval();
        LOG_TRACE_MSG( "retrying in " << interval << " seconds" );
        _timer.expires_from_now( boost::posix_time::seconds( interval) );

        // wait a bit
        _timer.async_wait(
                _strand.wrap(
                    boost::bind(
                        &Timer::timerHandler,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        callback
                        )
                    )
                );
    }
}

} // server
} // mux
} // runjob
