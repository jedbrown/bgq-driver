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

#include "server/cios/Heartbeat.h"

#include "server/block/Io.h"
#include "server/block/IoNode.h"
#include "server/cios/Connection.h"
#include "server/cios/Message.h"
#include "server/Options.h"
#include "server/Server.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace cios {

/*
<rasevent
  id="00062005" 
  category="Block" 
  component="MMCS"
  severity="FATAL"
  message="A CIOS jobctl daemon running on I/O node $(BG_LOC) failed to acknowledge a heartbeat."
  description="A heartbeat between the control system and an I/O node went unacknowledged, indicating a network or software problem."
  service_action="Reboot the I/O node."
  control_action="SOFTWARE_IN_ERROR,FREE_COMPUTE_BLOCK,END_JOB"
 />
*/


Heartbeat::Ptr
Heartbeat::create(
        const Server::Ptr& server
        )
{
    const Ptr result(
            new Heartbeat( server )
            );

    result->start();

    return result;
}

Heartbeat::Heartbeat(
        const Server::Ptr& server
        ) :
    _server( server ),
    _timer( server->getIoService() )
{
    LOG_DEBUG_MSG( __FUNCTION__ );
}

unsigned
Heartbeat::getInterval(
        const Server::Ptr& server
        ) const
{
    const std::string key( "jobctl_heartbeat" );
    int result = defaults::ServerJobctlHeartbeat;
    try {
        result = boost::lexical_cast<int>(
                server->getOptions().getProperties()->getValue(
                    runjob::server::PropertiesSection,
                    key
                    )
                );
        if ( result < 0 ) {
            result = defaults::ServerJobctlHeartbeat;
            LOG_WARN_MSG( key << " value must be greater than zero, using default of " << result );
        } else {
            // zero or positive number, use it
        }

        LOG_DEBUG_MSG( "using interval of " << result << " seconds" );
        // fall through
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( "garbage " << key << " value, using default of " << result );
        // fall through
    } catch ( const std::exception& e ) {
        LOG_DEBUG_MSG( key << " not found, using default of " << result );
    }
        
    return result;
}

void
Heartbeat::start()
{
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    const unsigned interval( this->getInterval(server) );

    // if heartbeat is disabled, interval will be 0 but we still want to async_wait
    boost::system::error_code error;
    _timer.expires_from_now( 
            interval ? boost::posix_time::seconds(interval) : boost::posix_time::seconds(defaults::ServerJobctlHeartbeat),
            error 
            );
    if ( error ) {
        LOG_WARN_MSG( "could not set expiration time: " << boost::system::system_error(error).what() );
    }

    _timer.async_wait(
            boost::bind(
                &Heartbeat::impl,
                shared_from_this(),
                boost::asio::placeholders::error,
                interval
                )
            );
}

Heartbeat::~Heartbeat()
{
    LOG_DEBUG_MSG( __FUNCTION__ );
}

void
Heartbeat::impl(
        const boost::system::error_code& error,
        const unsigned interval
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );

    if ( error == boost::asio::error::operation_aborted ) {
        // do nothing, we were asked to cancel
        return;
    }

    if ( error ) {
        LOG_ERROR_MSG( "wait failed:  " << boost::system::system_error(error).what() );
        return;
    }

    if ( !interval ) {
        // 0 seconds means we are disabled, wait again
        this->start();
        return;
    }

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    server->getBlocks()->get(
            boost::bind(
                &Heartbeat::blocksHandler,
                shared_from_this(),
                _1
                )
            );
}

void
Heartbeat::blocksHandler(
        const block::Container::Blocks& blocks
        )
{
    LOG_DEBUG_MSG( __FUNCTION__ << " " << blocks.size() << " blocks" );

    typedef std::vector<cios::Connection::Ptr> IoLinks;
    IoLinks links;

    BOOST_FOREACH( const block::Container::Blocks::value_type& i, blocks ) {
        if ( const block::Io::Ptr io = boost::dynamic_pointer_cast<block::Io>(i) ) {
            LOGGING_DECLARE_BLOCK_MDC( io->name() );
            const block::Io::Nodes& nodes = io->getNodes();
            BOOST_FOREACH( const block::Io::Nodes::value_type& j, nodes ) {
                links.push_back( j->getControl() );
            }
        }
    }

    const cios::Message::Ptr pulse(
            cios::Message::create(
                bgcios::jobctl::Heartbeat,
                time(NULL) // current timestamp as job ID
                )
            );

    LOG_DEBUG_MSG( __FUNCTION__ << " " << links.size() << " jobctld connections" );
    BOOST_FOREACH( const IoLinks::value_type& i, links ) {
        if ( !i ) continue;

        i->write( pulse );
    }

    this->start();
}

} // cios
} // server
} // runjob 
