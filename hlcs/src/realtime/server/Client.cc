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

#include "Client.h"

#include "DbChangesMonitor.h"
#include "Status.h"

#include "bgsched/realtime/StartRealtimeMessage.h"

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include <sstream>
#include <stdexcept>
#include <string>


using boost::bind;
using boost::lexical_cast;
using boost::shared_ptr;

using std::istream;
using std::string;

LOG_DECLARE_FILE( "realtime.server" );

namespace realtime {
namespace server {

Client::Client(
        _SocketPtr socket_ptr,
        DbChangesMonitor& db_changes_monitor,
        Status& status
    ) :
        _socket_ptr(socket_ptr),
        _db_changes_monitor(db_changes_monitor),
        _status(status),
        _id(lexical_cast<string>( _socket_ptr->next_layer().remote_endpoint() )),
        _strand( _socket_ptr->get_io_service() ),
        _started(false),
        _shutdown_ind(false),
        _waiting_connect_to_db_monitor_result(false),
        _connected_to_db_monitor(false)
{
    // Nothing to do.
}

void Client::start()
{
    static const unsigned BUFFER_SIZES(5 * 1024);
    _socket_ptr->next_layer().set_option( boost::asio::socket_base::receive_buffer_size( BUFFER_SIZES ) );
    _socket_ptr->next_layer().set_option( boost::asio::socket_base::send_buffer_size( BUFFER_SIZES ) );

    _startRead();
    _status.clientConnected();
    _started = true;
}

void Client::notifyConnectedToDbMonitor( bool is_connected )
{
    _strand.post( bind( &Client::_notifyConnectedToDbMonitorImpl, shared_from_this(), is_connected ) );
}

void Client::notifyDbChanges(
        const DbChanges& db_changes
    )
{
    _strand.post( bind( &Client::_notifyDbChangesImpl, shared_from_this(), db_changes ) );
}

Client::~Client()
{
    LOG_DEBUG_MSG( "Destroyed client " << _id );

    if ( _started ) {
        _status.clientDisconnected();
    }
}

void Client::_notifyConnectedToDbMonitorImpl( bool is_connected )
{
    if ( _shutdown_ind ) {
        LOG_DEBUG_MSG( "Notified connected when shutting down." );
        _db_changes_monitor.removeClient( shared_from_this() ); // Make sure I get removed!
        return;
    }

    if ( is_connected ) {
        if ( _connected_to_db_monitor ) {
            LOG_WARN_MSG( "Client " << _id << " notified connected to database monitor when already connected." );
            return;
        }

        if ( _waiting_connect_to_db_monitor_result ) {
            LOG_DEBUG_MSG( "Client " << _id << " connected to database monitor." );
            _waiting_connect_to_db_monitor_result = false;
        } else {
            LOG_DEBUG_MSG( "Client " << _id << " reconnected to database monitor." );
        }

        _connected_to_db_monitor = true;

        _out_msgs.push_back( bgsched::realtime::AbstractMessage::ConstPtr( new bgsched::realtime::ClientEventListener::RealtimeStartedEventInfo::Impl( _filter_id ) ) );
        _startWriting();

        return;
    }

    // Disconnected.

    if ( ! _connected_to_db_monitor ) {
        LOG_WARN_MSG( "Client " << _id << " notified disconnected from database monitor when not connected." );
        return;
    }

    if ( _waiting_connect_to_db_monitor_result ) {
        LOG_INFO_MSG( "Client " << _id << " REJECTED by database monitor." );
        _waiting_connect_to_db_monitor_result = false;
    } else {
        LOG_INFO_MSG( "Client " << _id << " DISCONNECTED from database monitor." );
    }

    _connected_to_db_monitor = false;

    _out_msgs.push_back( bgsched::realtime::AbstractMessage::ConstPtr( new bgsched::realtime::ClientEventListener::RealtimeEndedEventInfo::Impl() ) );
    _startWriting();
}

void Client::_notifyDbChangesImpl(
        const DbChanges& db_changes
    )
{
    if ( _shutdown_ind ) {
        LOG_DEBUG_MSG( "Client " << _id << " notified of DB changes when shutting down" );
        _db_changes_monitor.removeClient( shared_from_this() ); // Make sure I get removed!
        return;
    }

    if ( ! _connected_to_db_monitor ) {
        LOG_DEBUG_MSG( "Client " << _id << " notified of DB changes when not connected." );
        _db_changes_monitor.removeClient( shared_from_this() ); // Make sure I'm not in the list.
        return;
    }

    _OutMsgs filtered_db_change_msgs;

    // Apply the filter to each of the changes and gather up the messages.

    BOOST_FOREACH( const DbChanges::value_type& db_change_ptr, db_changes ) {
        if ( ! _filter.check( *db_change_ptr ) ) {
            LOG_DEBUG_MSG( "change filtered for real-time client " << _id );
            continue;
        }

        filtered_db_change_msgs.push_back( db_change_ptr );
    }

    LOG_TRACE_MSG( "Client " << _id << " notified of " << db_changes.size() << " changes,"
                     " filtered to " << filtered_db_change_msgs.size() << ","
                     " have " << _out_msgs.size() );

    if ( filtered_db_change_msgs.empty() ) {
        // All messages have been filtered out, nothing left to do.
        return;
    }

    // Check to see if will have too many messages if these are accepted.

    const uint64_t total_messages(_out_msgs.size() + filtered_db_change_msgs.size());

    if ( total_messages > MAX_MSGS ) {
        LOG_WARN_MSG( "Removing client " << _id << " because it's too far behind." );

        _shutdown();

        return;
    }


    // Print out a message if the client is falling behind. Only every 50.
    static const unsigned MESSAGE_OUTPUT_INTERVAL(50);

    if ( (_out_msgs.size() % MESSAGE_OUTPUT_INTERVAL > total_messages % MESSAGE_OUTPUT_INTERVAL) || filtered_db_change_msgs.size() >= MESSAGE_OUTPUT_INTERVAL ) {
        LOG_DEBUG_MSG( "Client " << _id << " falling behind. Now has " << total_messages << " outstanding messages. Will disconnect if over " << MAX_MSGS << "." );
    }


    _out_msgs.insert( _out_msgs.end(), filtered_db_change_msgs.begin(), filtered_db_change_msgs.end() );
    _startWriting();
}

void Client::_startRead()
{
    // Start reading a line of input (the message)
    boost::asio::async_read_until(
            *_socket_ptr, _in_sb, '\n',
            _strand.wrap( bind( &Client::_readComplete,
                  shared_from_this(),
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred
              ) )
        );
}

void Client::_readComplete( const boost::system::error_code& err, size_t /*bytes_transferred*/ )
{
    if ( _shutdown_ind ) {
        return;
    }

    try {
        if ( err ) {
            const boost::system::system_error e(err);
            LOG_DEBUG_MSG( "Client read error: " << e.what() );
            throw std::runtime_error( std::string("read error: ") + e.what() );
        }

        istream is( &_in_sb );

        string line;

        std::getline( is, line );

        LOG_DEBUG_MSG( "Got line " << line );

        bgsched::realtime::AbstractMessage::ConstPtr msg_ptr(bgsched::realtime::AbstractMessage::fromString( line ));

        // Handle the message.
        _handleMessage( *msg_ptr );

        _startRead();
    } catch ( std::exception& e ) {
        LOG_WARN_MSG( "Error reading from client " << _id << ", " << e.what() );
        _shutdown();
    }
}

void Client::_handleMessage( const bgsched::realtime::AbstractMessage& msg )
{
    {
        const bgsched::realtime::StartRealtimeMessage *start_msg_p(dynamic_cast<const bgsched::realtime::StartRealtimeMessage*>( &msg ));
        if ( start_msg_p ) {
            _filter.set( start_msg_p->getFilter() ); // compiles patterns in the filter
            _filter_id = start_msg_p->getFilterId(); // store the current filter id

            LOG_DEBUG_MSG( "Got filter " << _filter_id << " '" << start_msg_p->getFilter() << "'" );

            // If already connected to db monitor, just send back start_ack,
            // otherwise will respond when get response from DB monitor.

            if ( _waiting_connect_to_db_monitor_result || _connected_to_db_monitor ) {

                _out_msgs.push_back( bgsched::realtime::AbstractMessage::ConstPtr( new bgsched::realtime::ClientEventListener::RealtimeStartedEventInfo::Impl( _filter_id ) ) );
                _startWriting();

            } else {

                _waiting_connect_to_db_monitor_result = true;
                _db_changes_monitor.addClient( shared_from_this() );

            }

            return;
        }
    }

    LOG_WARN_MSG( "Forgot to implement handling of message" );

    throw std::runtime_error( "Don't know how to handle message" );
}

void Client::_startWriting()
{
    if ( ! _to_write.empty() ) {
        // already writing.
        return;
    }

    if ( _out_msgs.empty() ) {
        LOG_TRACE_MSG( "Waiting for something to write." );
        return;
    }

    bgsched::realtime::AbstractMessage::ConstPtr msg_ptr(_out_msgs.front());
    _out_msgs.pop_front();

    string msg_str(bgsched::realtime::AbstractMessage::toString( msg_ptr ));

    LOG_TRACE_MSG( "Sending message '" << msg_str << "'" );

    _to_write = msg_str + "\n";

    boost::asio::async_write(
            *_socket_ptr,
            boost::asio::buffer( _to_write ),
            _strand.wrap( bind( &Client::_writeComplete,
                  shared_from_this(),
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred
              ) )
        );
}

void Client::_writeComplete( const boost::system::error_code& err, size_t /*bytes_transferred*/ )
{
    if ( _shutdown_ind ) {
        return;
    }

    if ( err ) {
        LOG_DEBUG_MSG( "Client write error: " << err );
        _shutdown();
        return;
    }

    _to_write.clear();
    _startWriting();
}

void Client::_shutdown()
{
    if ( _shutdown_ind ) {
        // Already shutting down.
        return;
    }

    LOG_DEBUG_MSG( "Client " << _id << " shutting down..." );
    _shutdown_ind = true;
    _socket_ptr->next_layer().close();
    _db_changes_monitor.removeClient( shared_from_this() );
}

} // namespace realtime::server
} // namespace realtime
