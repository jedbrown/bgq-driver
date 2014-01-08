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

#include "server/realtime/Connection.h"

#include "common/logging.h"

#include "server/realtime/EventHandler.h"
#include "server/realtime/Polling.h"
#include "server/Server.h"

#include <hlcs/include/bgsched/realtime/ConnectionException.h>

#include <boost/assert.hpp>
#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace realtime {

const unsigned MaximumConnectTimeout( 30 );
const unsigned InitialConnectTimeout( 2 );

Connection::Ptr
Connection::create(
        const Server::Ptr& server
        )
{
    const Ptr result(
            new Connection( server )
            );

    result->_handler.start(
            boost::bind(
                &Connection::connect,
                result
                )
            );

    return result;
}

Connection::Connection(
        const Server::Ptr& server
        ) :
    _server( server ),
    _client( ),
    _connectTimeout( InitialConnectTimeout ), // seconds
    _timer( server->getIoService() ),
    _descriptor( ),
    _handler( server ),
    _connected( false )
{
    _client.setBlocking( false );
    _client.addListener( _handler );
}

void
Connection::setFilter()
{
    LOG_TRACE_MSG( __FUNCTION__ );
    using namespace bgsched::realtime;

    // we only care about block status events
    Filter filter( Filter::createNone() );
    filter.setBlocks( true );
    Filter::BlockStatuses statuses;
    statuses.insert( bgsched::Block::Booting );
    statuses.insert( bgsched::Block::Free );
    statuses.insert( bgsched::Block::Initialized );
    statuses.insert( bgsched::Block::Terminating );
    filter.setBlockStatuses( &statuses );

    Filter::Id filter_id;
    _client.setFilter( filter, &filter_id, NULL );
    LOG_TRACE_MSG( "filter id: " << filter_id );
}

Connection::~Connection()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

bool
Connection::status()
{
    return _connected;
}

void
Connection::connect()
{
    LOG_INFO_MSG( "connecting to real-time server" );

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    try {
        _connected = false;
        _client.connect();
        _connected = true;
    } catch ( const std::exception& e ) {
        LOG_TRACE_MSG( e.what() );
        _connectTimeout++;
        if ( _connectTimeout > MaximumConnectTimeout ) {
            _connectTimeout = MaximumConnectTimeout;
        }

        _timer.expires_from_now( boost::posix_time::seconds(_connectTimeout) );
        LOG_TRACE_MSG( "waiting for " << _connectTimeout << " seconds" );
        _timer.async_wait(
                boost::bind(
                    &Connection::connect,
                    shared_from_this()
                    )
                );

        return;
    }

    LOG_INFO_MSG( "connected" );

    this->setFilter();

    _connectTimeout = InitialConnectTimeout;

    _client.requestUpdates( NULL );

    // duplicate the real-time client descriptor since the stream_descriptor assumes ownership
    _descriptor.reset(
            new boost::asio::posix::stream_descriptor(
                server->getIoService(),
                dup( _client.getPollDescriptor() )
                )
            );

    this->write();
}

void
Connection::write()
{
    LOG_TRACE_MSG( __FUNCTION__ );
    bool interrupted(false), again(false);
    _client.sendMessages( &interrupted, &again );
    LOG_TRACE_MSG(
            __FUNCTION__ << " " <<
            std::boolalpha << 
            "interrupted=" << interrupted << " " <<
            "again=" << again
            );

    if ( again || interrupted ) {
        _descriptor->async_write_some(
                boost::asio::null_buffers(),
                boost::bind(
                    &Connection::write,
                    shared_from_this()
                    )
                );
        return;
    }
        
    // done writing, time to start reading for responses
    this->read();
}

void
Connection::read()
{
    LOG_TRACE_MSG( __FUNCTION__ );
    _descriptor->async_read_some(
            boost::asio::null_buffers(),
            boost::bind(
                &Connection::handleRead,
                shared_from_this(),
                boost::asio::placeholders::error
                )
            );
}

void
Connection::handleRead(
        const boost::system::error_code& error
        )
{
    if ( error ) {
        LOG_WARN_MSG( __FUNCTION__ << " " << boost::system::system_error(error).what() );
        this->connect();
        return;
    }

    try {
        bool interrupted(false), again(false), end(false);
        _client.receiveMessages( &interrupted, &again, &end );
        LOG_TRACE_MSG(
                __FUNCTION__ << " " <<
                std::boolalpha << 
                "interrupted=" << interrupted << " " <<
                "again=" << again << " " <<
                "end=" << end 
                );

        if ( again ) {
            this->read();
            return;
        }

        if ( interrupted ) {
            this->read();
            return;
        }

        if ( end ) {
            _handler.poll();
            this->connect();
            return;
        }

        BOOST_ASSERT( !"should not get here" );
    } catch ( const bgsched::realtime::ConnectionException& e ) {
        LOG_WARN_MSG( e.what() );
        this->connect();
        return;
    }
}

} // realtime
} // server
} // runjob
