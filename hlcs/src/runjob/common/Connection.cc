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
#include "common/Connection.h"

#include "common/logging.h"

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

LOG_DECLARE_FILE( runjob::log );

namespace runjob {

Connection::Connection(
        const bgq::utility::portConfig::SocketPtr& socket
        ) :
    _socket( socket ),
    _resolver( socket->get_io_service() ),
    _hostname(),
    _shortHostname(),
    _incomingMessage(),
    _outgoingMessage(),
    _timer( socket->get_io_service() ),
    _strand( socket->get_io_service() )
{
    BOOST_ASSERT( socket );

    // enable close on exec
    (void)fcntl(
            _socket->lowest_layer().native(),
            F_SETFD,
            fcntl(_socket->lowest_layer().native(), F_GETFD) | FD_CLOEXEC
            );
}

void
Connection::start()
{
    // log remote endpoint
    const boost::asio::ip::tcp::endpoint& ep = _socket->next_layer().remote_endpoint();
    LOG_INFO_MSG( "connection from " << ep);

    // resolve hostname
    _resolver.async_resolve(
            ep,
            _strand.wrap(
                boost::bind(
                    &Connection::resolveHandler,
                    this->getShared(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::iterator
                    )
                )
            );

    // cancel resolve in 10 seconds if it's not complete
    _timer.expires_from_now( boost::posix_time::seconds(10) );
    _timer.async_wait(
            _strand.wrap(
                boost::bind(
                    &Connection::timerHandler,
                    this->getShared(),
                    boost::asio::placeholders::error
                    )
                )
            );
}

void
Connection::resolveHandler(
        const boost::system::error_code& error,
        const boost::asio::ip::tcp::resolver::iterator& endpoint_iterator
        )
{
    if ( error ) {
        LOG_WARN_MSG( "failed to resolve " << _socket->next_layer().remote_endpoint() );
        LOG_WARN_MSG( boost::system::system_error(error).what() );
    } else if ( !_hostname.empty() ) {
        LOG_DEBUG_MSG( __FUNCTION__ << " hostname already resolved: " << _hostname );
    } else {
        LOG_INFO_MSG( "resolved hostname " << endpoint_iterator->host_name() << " for " << endpoint_iterator->endpoint() );
        if ( _hostname != endpoint_iterator->host_name() ) {
            _shortHostname = _hostname = endpoint_iterator->host_name();
            // shorten to basename
            const std::string::size_type period( _shortHostname.find_first_of('.') );
            if ( period != std::string::npos ) {
                _shortHostname.erase(period);
                LOG_DEBUG_MSG( "shortened hostname to " << _shortHostname );
            }
        }
    }

    // disable Nagle algorithm
    _socket->lowest_layer().set_option(
            boost::asio::ip::tcp::no_delay( true )
            );

    boost::system::error_code ec;
    _timer.cancel( ec ); // don't care about error

    this->resolveComplete();
}

void
Connection::timerHandler(
        const boost::system::error_code& error
        )
{
    if ( error == boost::asio::error::operation_aborted ) {
        // do nothing, resolve completed
        return;
    } else if ( !_hostname.empty() ) {
        LOG_DEBUG_MSG( __FUNCTION__ << " hostname already resolved: " << _hostname );
        return;
    }

    LOG_WARN_MSG( "resolving " << _socket->next_layer().remote_endpoint() << " timed out" );
    LOG_WARN_MSG( "using address as hostname" );

    // use address as hostname
    _shortHostname = _hostname = boost::lexical_cast<std::string>( _socket->next_layer().remote_endpoint().address() );

    // disable Nagle algorithm
    _socket->lowest_layer().set_option(
            boost::asio::ip::tcp::no_delay( true )
            );

    _resolver.cancel();

    this->resolveComplete();
}

void
Connection::resolveComplete()
{
    LOG_TRACE_MSG( __FUNCTION__ );
    this->readHeader();
}

} // runjob
