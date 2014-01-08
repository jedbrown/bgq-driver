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
#include "mux/handlers/Status.h"

#include "mux/client/Credentials.h"
#include "mux/client/Container.h"
#include "mux/client/Runjob.h"

#include "common/commands/MuxStatus.h"

#include "common/ConnectionContainer.h"
#include "common/logging.h"

#include "mux/CommandConnection.h"
#include "mux/Multiplexer.h"
#include "mux/Options.h"

#include <hlcs/include/bgsched/bgsched.h>

#include <utility/include/ScopeGuard.h>
#include <utility/include/version.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::mux::log );

namespace runjob {
namespace mux {
namespace handlers {

Status::Status(
        const boost::weak_ptr<Multiplexer>& mux
        ) :
    CommandHandler(),
    _mux( mux ),
    _clientMap()
{

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

    const Multiplexer::Ptr mux( _mux.lock() );
    if ( !mux ) return;

    const runjob::commands::response::MuxStatus::Ptr response( new runjob::commands::response::MuxStatus );

    try {
        // add boilerplate stuff
        response->setDriver( bgq::utility::DriverName );
        response->setRevision( boost::lexical_cast<uint32_t>(bgq::utility::Revision) );
        response->setProperties( mux->getOptions().getProperties()->getFilename() );
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( e.what() );
    }

    // measure load of asio io_service based on how long it takes
    // to invoke this handler
    const boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();
    mux->getIoService().post(
            boost::bind(
                &Status::loadHandler,
                shared_from_this(),
                start,
                connection,
                response
                )
            );

}

void
Status::loadHandler(
        const boost::posix_time::ptime& start,
        const boost::shared_ptr<CommandConnection>& connection,
        const runjob::commands::response::MuxStatus::Ptr& response
        )
{
    // calculate duration spent in reactor queue
    const boost::posix_time::ptime end = boost::posix_time::microsec_clock::local_time();
    const boost::posix_time::time_duration load = end - start;
    response->setLoad( load.total_microseconds() );
    LOG_DEBUG_MSG( response->getLoad() << " microseconds in asio queue" );

    const Multiplexer::Ptr mux( _mux.lock() );
    if ( !mux ) return;

    mux->getConnections()->get(
            boost::bind(
                &Status::connectionCallback,
                shared_from_this(),
                connection,
                response,
                _1
                )
            );
}

void
Status::connectionCallback(
        const CommandConnection::Ptr& connection,
        const runjob::commands::response::MuxStatus::Ptr& response,
        const runjob::ConnectionContainer::Container& connections
        )
{
    Multiplexer::Ptr mux( _mux.lock() );
    if ( !mux ) return;

    // iterate through connections
    BOOST_FOREACH( const runjob::ConnectionContainer::Container::value_type& i, connections ) {
        // convert weak ptr to shared ptr
        Connection::Ptr connection = i.second.lock();
        if ( connection ) {
            // create status connection
            runjob::commands::response::Status::Connection status;

            // get endpoint
            const boost::asio::ip::tcp::endpoint& ep( connection->getEndpoint() );
            status._address = ep.address().to_string();
            status._port = ep.port();

            // set type
            status._type = "unknown";
            if ( boost::dynamic_pointer_cast<CommandConnection>( connection ) ) {
                status._type = "command";
            }
            
            // add connection to message
            response->addConnection( status );
        }
    }

    // get server connection information
    mux->getServer()->status(
            response,
            boost::bind(
                &Status::serverStatusCallback,
                shared_from_this(),
                connection,
                _1
                )
            );

}

void
Status::serverStatusCallback(
        const CommandConnection::Ptr& connection,
        const runjob::commands::response::MuxStatus::Ptr& response
        )
{
    Multiplexer::Ptr mux( _mux.lock() );
    if ( !mux ) return;

    // get plugin status
    mux->getPlugin()->get(
            boost::bind(
                &Status::pluginCallback,
                shared_from_this(),
                connection,
                response,
                _1
                )
            );
}

void
Status::pluginCallback(
        const CommandConnection::Ptr& connection,
        const runjob::commands::response::MuxStatus::Ptr& response,
        const Plugin::WeakPtr& plugin
        )
{
    if ( Plugin::Ptr p = plugin.lock() ) {
        response->setPlugin( p->getPath() );
    }

    response->_bgschedMajor = bgsched::version::major;
    response->_bgschedMinor = bgsched::version::minor;
    response->_bgschedMod = bgsched::version::mod;

    // get client information
    Multiplexer::Ptr mux( _mux.lock() );
    if ( !mux ) return;
    mux->getClientContainer()->get(
            boost::bind(
                &Status::getClientsCallback,
                shared_from_this(),
                connection,
                response,
                _1
                )
            );
}

void
Status::getClientsCallback(
        const CommandConnection::Ptr& connection,
        const runjob::commands::response::MuxStatus::Ptr& response,
        const client::Container::Map& clients
        )
{
    LOG_TRACE_MSG( clients.size() << " clients" );

    // this guard will send the response
    bgq::utility::ScopeGuard guard(
            boost::bind(
                &CommandConnection::write,
                connection,
                response
                )
            );

    if ( clients.empty() ) return;

    // copy container into our member so we can obtain status from each client
    _clientMap = clients;

    // find first entry that has not expired
    client::Container::Map::const_iterator i = _clientMap.begin();
    client::Runjob::Ptr client = i->second.lock();
    while ( i != _clientMap.end() && !client ) {
        ++i;
        client = i->second.lock();
    }

    // it's possible all entries have expired
    if ( !client ) return;

    guard.dismiss();
    client->status(
            response,
            boost::bind(
                &Status::clientStatusCallback,
                shared_from_this(),
                connection,
                _1,
                ++i
                )
            );
}
    
void
Status::clientStatusCallback(
        const CommandConnection::Ptr& connection,
        const runjob::commands::response::MuxStatus::Ptr& response,
        client::Container::Map::const_iterator& i
        )
{
    bgq::utility::ScopeGuard guard(
            boost::bind(
                &CommandConnection::write,
                connection,
                response
                )
            );

    // send response if this is the last entry
    if ( i == _clientMap.end() ) return;

    // find next entry that has not expired
    client::Runjob::Ptr client = i->second.lock();
    while ( i != _clientMap.end() && !client ) {
        ++i;
        client = i->second.lock();
    }

    // it's possible all entries have expired
    if ( !client ) return;

    guard.dismiss();
    client->status(
            response,
            boost::bind(
                &Status::clientStatusCallback,
                shared_from_this(),
                connection,
                _1,
                ++i
                )
            );
}

} // handlers
} // mux
} // runjob
