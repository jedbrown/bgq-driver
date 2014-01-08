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
#include "server/handlers/Status.h"

#include "common/logging.h"

#include "common/ConnectionContainer.h"

#include "server/block/Io.h"
#include "server/block/IoNode.h"

#include "server/mux/Connection.h"

#include "server/performance/Counters.h"

#include "server/realtime/Connection.h"

#include "server/Block.h"
#include "server/CommandConnection.h"
#include "server/Job.h"
#include "server/Options.h"
#include "server/Server.h"

#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/StdioMessages.h>

#include <utility/include/version.h>

#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace handlers {

Status::Status(
        const Server::Ptr& server
        ) :
    CommandHandler( server ),
    _links( boost::make_shared<IoLinks>() ),
    _link( _links->begin() ),
    _connection(),
    _response( boost::make_shared<runjob::commands::response::ServerStatus>() )
{

}

Status::~Status()
{
    LOG_TRACE_MSG( __FUNCTION__ );

    if ( !_connection ) return;

    _connection->write( _response );
}

void
Status::handle(
        const runjob::commands::Request::Ptr& request,
        const CommandConnection::Ptr& connection
        )
{
    // there's nothing in the request we need (yet)
    // do this to avoid unused parameter warning
    (void)request;

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    _connection = connection;

    try {
        // add boilerplate stuff
        _response->setDriver( bgq::utility::DriverName );
        _response->setRevision( boost::lexical_cast<uint32_t>(bgq::utility::Revision) );
        _response->setProperties( _options.getProperties()->getFilename() );
        _response->_simulation = _options.getSim();
        _response->_jobctlProtocol = bgcios::jobctl::ProtocolVersion;
        _response->_stdioProtocol = bgcios::stdio::ProtocolVersion;
        _response->_realtime = server->getRealtimeConnection()->status();
        _response->_connectionPoolSize = BGQDB::DBConnectionPool::instance().size();
        _response->_connectionPoolAvailable = BGQDB::DBConnectionPool::instance().availableCount();
        _response->_connectionPoolUsed = BGQDB::DBConnectionPool::instance().usedCount();
        _response->_connectionPoolMax = BGQDB::DBConnectionPool::instance().maxCount();
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( e.what() );
    }

    // add performance counter metrics
    const performance::Counters::Ptr& counters = server->getPerformanceCounters();
    _response->_jobCounters = counters->getJobs()->getCount();
    _response->_miscCounters = counters->getMisc()->getCount();

    // measure load of asio io_service based on how long it takes
    // to invoke this handler
    const boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();
    server->getIoService().post(
            boost::bind(
                &Status::loadHandler,
                shared_from_this(),
                start
                )
            );
}

void
Status::loadHandler(
        const boost::posix_time::ptime& start
        )
{
    // calculate duration spent in reactor queue
    const boost::posix_time::ptime end = boost::posix_time::microsec_clock::local_time();
    const boost::posix_time::time_duration load = end - start;
    _response->setLoad( load.total_microseconds() );
    LOG_DEBUG_MSG( _response->getLoad() << " microseconds in asio queue" );

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    server->getConnections()->get(
            boost::bind(
                &Status::connectionsHandler,
                shared_from_this(),
                _1
                )
            );
}

void
Status::connectionsHandler(
        const runjob::ConnectionContainer::Container& connections
        )
{
    // iterate through connections
    BOOST_FOREACH( const runjob::ConnectionContainer::Container::value_type& i, connections ) {
        const Connection::Ptr connection = i.second.lock();
        if ( !connection ) continue;

        // create status connection
        runjob::commands::response::Status::Connection status;

        try {
            const boost::asio::ip::tcp::endpoint& ep( connection->getEndpoint() );
            status._address = ep.address().to_string();
            status._port = ep.port();

            status._type = "unknown";
            if ( boost::dynamic_pointer_cast<mux::Connection>( connection ) ) {
                status._type = "mux";
            } else if ( boost::dynamic_pointer_cast<CommandConnection>( connection ) ) {
                status._type = "command";
            }

            // add connection to message
            _response->addConnection( status );
        } catch ( const boost::system::system_error& e ) {
            LOG_WARN_MSG( e.what() );
        }
    }

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    server->getJobs()->get(
            boost::bind(
                &Status::jobsHandler,
                shared_from_this(),
                _1
                )
            );
}

void
Status::jobsHandler(
        const job::Container::Jobs& jobs
        )
{
    LOG_DEBUG_MSG( "adding " << jobs.size() << " jobs" );

    BOOST_FOREACH( const job::Container::Jobs::value_type& i, jobs ) {
        const Job::Ptr& job = i.second;
        _response->_jobs.push_back( job->id() );
    }

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    server->getBlocks()->get(
            boost::bind(
                &Status::blocksHandler,
                shared_from_this(),
                _1
                )
            );
}

void
Status::blocksHandler(
        const block::Container::Blocks& blocks
        )
{
    LOG_DEBUG_MSG( "adding " << blocks.size() << " blocks" );

    BOOST_FOREACH( const block::Container::Blocks::value_type& i, blocks ) {
        runjob::commands::response::ServerStatus::Block block;
        block._id = i->name();
        _response->_blocks.push_back( block );

        // look for I/O blocks
        if ( const block::Io::Ptr io = boost::dynamic_pointer_cast<block::Io>(i) ) {
            // remember both control and data connections so we can include
            // their status in the response
            const block::Io::Nodes& nodes = io->getNodes();
            BOOST_FOREACH( const block::Io::Nodes::value_type& j, nodes ) {
                _links->push_back( j->getControl() );
                _links->push_back( j->getData() );
            }
        }
    }

    _link = _links->begin();
    while ( 1 ) {
        if ( _link == _links->end() ) break;
        if ( *_link ) break;

        ++_link;
    }

    if ( _link == _links->end() ) {
        return;
    }

    const cios::Connection::Ptr& ioLink = *_link;
    ioLink->status(
            boost::bind(
                &Status::linksHandler,
                shared_from_this(),
                _1
                )
            );
}

void
Status::linksHandler(
        const cios::Connection::SocketPtr& socket
        )
{
    cios::Connection::Ptr& ioLink = *_link;
    if ( ioLink ) {
        LOGGING_DECLARE_LOCATION_MDC( ioLink->getLocation() );
        LOG_TRACE_MSG( "status: " << (socket ? "Up" : "Down") );

        runjob::commands::response::ServerStatus::IoLink link;
        link._address = boost::lexical_cast<std::string>( ioLink->getEndpoint().address() );
        link._port = ioLink->getEndpoint().port();
        link._service = ioLink->getService();
        link._location = ioLink->getLocation().get();
        link._status = socket; // implicit conversion to bool

        _response->_ioLinks.push_back( link );
    }

    while ( 1 ) {
        ++_link;
        if ( _link == _links->end() ) return;
        if ( *_link ) break;
    }

    ioLink = *_link;

    ioLink->status(
            boost::bind(
                &Status::linksHandler,
                shared_from_this(),
                _1
                )
            );
}

} // handlers
} // server
} // runjob

