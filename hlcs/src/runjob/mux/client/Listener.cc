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
#include "mux/client/Listener.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include "mux/client/Connection.h"
#include "mux/client/Runjob.h"

#include "mux/Multiplexer.h"
#include "mux/Options.h"

#include <boost/filesystem/exception.hpp>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

namespace runjob {
namespace mux {
namespace client {

LOG_DECLARE_FILE( runjob::mux::log );

Listener::Listener(
        const boost::shared_ptr<Multiplexer>& mux
        ) :
    _options( mux->getOptions() ),
    _mux( mux ),
    _acceptor( mux->getIoService() ),
    _id_counter( 0 )
{
    try {
        // create endpoint
        std::string path( 1, '\0' ); // first byte is NULL for anonymous namespace
        path.append( _options.getLocalSocket() );
        boost::asio::local::stream_protocol::endpoint ep(path);

        // open
        _acceptor.open( ep.protocol() );

        // bind
        _acceptor.bind( ep );

        // listen
        _acceptor.listen();
    } catch ( const boost::system::system_error& e ) {
        LOG_FATAL_MSG( e.what() );
        throw;
    } catch ( const std::runtime_error& e ) {
        LOG_FATAL_MSG( e.what() );
        throw;
    }
}

void
Listener::start()
{
    LOG_INFO_MSG( "listening for connections at " << _options.getLocalSocket() );
    this->accept();
}

Listener::~Listener()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

void
Listener::accept()
{
    LOG_TRACE_MSG( __FUNCTION__ );
    try {
        const Runjob::Ptr client(
                new Runjob( _mux.lock(), Id(++_id_counter) )
                );

        _acceptor.async_accept(
                client->getConnection()->getSocket(),
                boost::bind(
                    &Listener::acceptHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    client
                    )
                );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }
}

void
Listener::acceptHandler(
            const boost::system::error_code& error,
            const Runjob::Ptr& client
        )
{
    LOG_TRACE_MSG( "accept handler" );
    if ( error ) {
        LOG_ERROR_MSG( "could not accept " << error.message() );
        this->accept();
        return;
    }

    // start reading from client
    try {
        LOGGING_DECLARE_LOCATION_MDC( _id_counter );
        LOG_INFO_MSG( "accepted" );
        client->start();
    } catch ( const std::runtime_error& e ) {
        LOG_WARN_MSG( "could not start client: " << e.what() );
    }
    
    this->accept();
}

} // client
} // mux
} // runjob
