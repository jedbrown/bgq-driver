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

    // use address as hostname
    _hostname = boost::lexical_cast<std::string>( ep.address() );

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
    } else {
        LOG_INFO_MSG( "resolved hostname " << endpoint_iterator->host_name() << " for " << _hostname );
        if ( _hostname != endpoint_iterator->host_name() ) {
            // shorten to basename
            _hostname = boost::filesystem::basename(
                    endpoint_iterator->host_name()
                    );
            LOG_DEBUG_MSG( "truncated hostname to " << _hostname );
        }
    }

    // disable Nagle algorithm
    _socket->lowest_layer().set_option(
            boost::asio::ip::tcp::no_delay( true )
            );

    _timer.cancel();
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
    }

    // cancel resolve
    LOG_WARN_MSG( "resolving " << _socket->next_layer().remote_endpoint() << " timed out" );
    LOG_WARN_MSG( "using address as hostname" );
    _resolver.cancel();

    // use address as hostname
    _hostname = boost::lexical_cast<std::string>( _socket->next_layer().remote_endpoint().address() );

    // notify parent class that resolve is complete
    this->resolveComplete();
}

void
Connection::resolveComplete()
{
    LOG_TRACE_MSG( __FUNCTION__ );
    this->readHeader();
}

} // runjob
