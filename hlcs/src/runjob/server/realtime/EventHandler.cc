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

#include "server/block/Container.h"
#include "server/block/Reconnect.h"
#include "server/realtime/EventHandler.h"
#include "server/realtime/Polling.h"
#include "server/Server.h"

#include "common/logging.h"

#include <boost/bind.hpp>

#include <sstream>

LOG_DECLARE_FILE( runjob::server::log );

namespace bgsched {

std::ostream&
operator<<(
        std::ostream& os,
        const Block::Status status
        )
{
    // these are the single character representations in the status column of tbgqblock, the order
    // here is based on the enumeration type in bgsched/Block.h
    static const std::string strings[] = {
        "ALLOCATED",
        "BOOTING",
        "FREE",
        "INITIALIZED",
        "TERMINATING"
    };

    if ( status <= static_cast<int>(sizeof(strings)) ) {
        os << strings[status];
    } else {
        os << "invalid block status: " << status;
    }

    return os;
}

} // bgsched

namespace runjob {
namespace server {
namespace realtime {

EventHandler::EventHandler(
        const Server::Ptr& server
        ) :
    _server( server ),
    _polling( ),
    _pollingSequence( 0),
    _sequence( 0 ),
    _strand( server->getIoService() ),
    _queue( )
{

}

void
EventHandler::start(
        const Callback& callback
        )
{
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    // start block reconnection to get the sequence ID that we'll
    // use for handling future real-time events
    block::Reconnect::create(
            server,
            boost::bind(
                &EventHandler::reconnectCallback,
                this,
                _1,
                callback
                )
            );
}

void
EventHandler::reconnectCallback(
        const bgsched::SequenceId sequence,
        const Callback& callback
        )
{
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    LOG_INFO_MSG( "starting with sequence " << sequence );

    _pollingSequence = sequence;
    _sequence = sequence;
    _polling = Polling::create(server, sequence);
    callback();
}

void
EventHandler::poll()
{
    LOG_TRACE_MSG( __FUNCTION__ );
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    if ( Polling::Ptr polling = _polling.lock() ) {
        LOG_TRACE_MSG( "already polling" );
    } else {
        _polling = Polling::create( server, _sequence );
    }
}

void
EventHandler::handlePollingEnded(
        const bgsched::SequenceId sequence
        )
{
    LOGGING_DECLARE_LOCATION_MDC( sequence );
    LOG_INFO_MSG( __FUNCTION__ << " " << _queue.size() );
    _pollingSequence = sequence;

    while ( !_queue.empty() ) {
        const BlockStateChangedEventInfo e = _queue.front();
        this->handleBlockStateChangedRealtimeEvent( e );
        _queue.pop();
    }
}

void
EventHandler::handleRealtimeStartedRealtimeEvent(
        const RealtimeStartedEventInfo& /* info */
        )
{
    LOG_INFO_MSG( "real-time started" );

    if ( Polling::Ptr polling = _polling.lock() ) {
        polling->stop(
                _strand.wrap(
                    boost::bind(
                        &EventHandler::handlePollingEnded,
                        this,
                        _1
                        )
                    )
                );
    } else {
        LOG_TRACE_MSG( "not polling" );
    }
}

void
EventHandler::handleRealtimeEndedRealtimeEvent(
        const RealtimeEndedEventInfo&
        )
{
    LOG_DEBUG_MSG( "Handling real-time ended event" );

    this->poll();
}

void
EventHandler::add(
        const BlockStateChangedEventInfo& event
        )
{
    _queue.push( event );
    LOG_DEBUG_MSG(
            "delaying block status change from " <<
            event.getPreviousStatus() << " to " << event.getStatus() <<
            " while polling is active"
            );
}

void
EventHandler::handleBlockStateChangedRealtimeEvent(
        const BlockStateChangedEventInfo& event
        )
{
    LOGGING_DECLARE_BLOCK_MDC( event.getBlockName() );
    LOGGING_DECLARE_LOCATION_MDC( event.getSequenceId() );

    if ( _polling.lock() ) {
        _strand.post(
                boost::bind(
                    &EventHandler::add,
                    this,
                    event
                    )
                );
        return;
    }

    if ( event.getSequenceId() <= _pollingSequence ) {
        // we already accounted for this change when polling
        LOG_DEBUG_MSG(
                "already handled block status change from " <<
                event.getPreviousStatus() << " to " << event.getStatus()
                );

        return;
    }
    
    LOG_TRACE_MSG(
            "Received block status changed real-time event." <<
            " old=" << event.getPreviousStatus() <<
            " new=" << event.getStatus()
            );

    _sequence = event.getSequenceId();

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    const bgsched::Block::Status current = event.getStatus();
    const bgsched::Block::Status previous = event.getPreviousStatus();

    try {
        if ( current == bgsched::Block::Free ) {
            server->getBlocks()->remove( 
                    event.getBlockName(),
                    boost::bind(
                        &EventHandler::blockCallback,
                        this,
                        event,
                        _1,
                        _2
                        )
                    );
        } else if ( previous == bgsched::Block::Initialized && current == bgsched::Block::Terminating ) {
            server->getBlocks()->terminating( 
                    event.getBlockName(),
                    boost::bind(
                        &EventHandler::blockCallback,
                        this,
                        event,
                        _1,
                        _2
                        )
                    );
        } else if ( previous == bgsched::Block::Allocated && current == bgsched::Block::Booting ) {
            server->getBlocks()->create( 
                    event.getBlockName(),
                    boost::shared_ptr<BGQMachineXML>(),
                    boost::bind(
                        &EventHandler::blockCallback,
                        this,
                        event,
                        _1,
                        _2
                        )
                    );
        } else if ( previous == bgsched::Block::Booting && current == bgsched::Block::Initialized ) {
            server->getBlocks()->initialized( 
                    event.getBlockName(),
                    boost::bind(
                        &EventHandler::blockCallback,
                        this,
                        event,
                        _1,
                        _2
                        )
                    );
        } else {
            LOG_WARN_MSG( "ignoring transition " << previous << " -> " << current );
        }
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }
}

void
EventHandler::blockCallback(
        const BlockStateChangedEventInfo& event,
        const error_code::rc error,
        const std::string& message
        )
{
    LOGGING_DECLARE_LOCATION_MDC( event.getSequenceId() );
    if ( error ) {
        LOG_WARN_MSG( event.getStatus() << ": " << message );
    } else {
        LOG_DEBUG_MSG( event.getPreviousStatus() << " --> " << event.getStatus() );
    }
}

} // realtime
} // server
} // runjob
