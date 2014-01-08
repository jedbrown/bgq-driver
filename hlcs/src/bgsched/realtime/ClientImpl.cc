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

#include "ClientImpl.h"

#include "ClientEventListenerImpl.h"
#include "ClientEventListenerV2Impl.h"
#include "StartRealtimeMessage.h"

#include "../utility.h"

#include <bgsched/InternalException.h>

#include <bgsched/realtime/ClientConfiguration.h>
#include <bgsched/realtime/ClientStateException.h>
#include <bgsched/realtime/ConnectionException.h>
#include <bgsched/realtime/InternalErrorException.h>
#include <bgsched/realtime/ProtocolException.h>

#include "utility/include/Log.h"

#include "utility/include/portConfiguration/Connector.h"

#include <boost/bind.hpp>

#include <boost/algorithm/string.hpp>

#include <sstream>
#include <stdexcept>


using bgq::utility::Connector;

using std::istream;
using std::istringstream;
using std::ostringstream;
using std::string;


namespace bgsched {
namespace realtime {


LOG_DECLARE_FILE( "bgsched" );


Client::Impl::Impl( const ClientConfiguration& client_configuration )
    : _config( client_configuration ),
      _blocking(true),
      _receiving_events(false),
      _filter(Filter::Impl::DEFAULT),
      _filter_id(0),
      _connected(false),
      _async_write_in_progress(false),
      _async_read_in_progress(false)
{
    _listener_ps.insert( &_default_client_event_listener );

    LOG_DEBUG_MSG( "created client @" << this );
}


void Client::Impl::addListener( ClientEventListener& l )
{
    _LockGuard lg( _mtx );

    _listener_ps.erase( &_default_client_event_listener );

    if ( ! _listener_ps.insert( &l ).second ) {
        LOG_WARN_MSG( "The application called addListener() with a listener that was already added." );
    }
}


void Client::Impl::removeListener( ClientEventListener& l )
{
    _LockGuard lg( _mtx );

    if ( _listener_ps.erase( &l ) == 0 ) {
        LOG_WARN_MSG( "The application called removeListener() with a listener that was not added." );
    }

    if ( _listener_ps.empty() ) {
        _listener_ps.insert( &_default_client_event_listener );
    }
}


void Client::Impl::setBlocking( bool blocking )
{
    _LockGuard lg( _mtx );

    _blocking = blocking;
    if ( _blocking ) {
        LOG_DEBUG_MSG( "real-time client is now blocking." );
    } else {
        LOG_DEBUG_MSG( "real-time client is now not blocking." );
    }
}


bool Client::Impl::isBlocking() const
{
    _LockGuard lg( _mtx );

    return _blocking;
}


void Client::Impl::connect()
{
    _LockGuard lg( _mtx );

    if ( _connected ) {
        LOG_WARN_MSG( "The application called connect() on a client that is already connected." );
        return;
    }

    try {

        _io_service.reset();

        Connector connector(
                _io_service,
                _config.getPortConfiguration()
            );

        _socket_ptr = connector.connect();

        static const unsigned BUFFER_SIZES(5 * 1024);

        _socket_ptr->next_layer().set_option( boost::asio::socket_base::receive_buffer_size( BUFFER_SIZES ) );
        _socket_ptr->next_layer().set_option( boost::asio::socket_base::send_buffer_size( BUFFER_SIZES ) );

    } catch ( bgq::utility::Connector::ResolveError& re ) {
        THROW_EXCEPTION(
                ConnectionException,
                ConnectionErrors::CannotResolve,
                "Could not resolve the real-time server host or service name."
            );
    } catch ( bgq::utility::Connector::ConnectError& ce  ) {
        THROW_EXCEPTION(
                ConnectionException,
                ConnectionErrors::CannotConnect,
                "Failed to connect to the real-time server."
            );
    }

    if ( ! _socket_ptr ) {
        THROW_EXCEPTION(
                InternalException,
                bgsched::InternalErrors::UnexpectedError,
                "Unexpected error connecting to the real-time server."
            );
    }

    _connected = true;
    _receiving_events = false;

    _out_msgs.clear();
    _write_buf.clear();

    LOG_DEBUG_MSG( "Connected to real-time server on " << _socket_ptr->lowest_layer().remote_endpoint() );
}


void Client::Impl::disconnect()
{
    // We can't lock right away here because then would have to wait for receiveMessages to exit.

    boost::system::error_code error;

    if ( ! _socket_ptr ) {
        LOG_DEBUG_MSG( "Already disconnected." );
        return;
    }

    _socket_ptr->lowest_layer().close( error );

    if ( error ) {
        _connected = false;

        THROW_EXCEPTION(
                InternalErrorException,
                InternalErrors::ApiUnexpectedFailure,
                "close() failed closing socket when disconnect() called."
                    " The error is " << error
            );
    }

    _LockGuard lg( _mtx );

    if ( ! _connected ) {
        LOG_DEBUG_MSG( "Already disconnected." );
        return;
    }

    _connected = false;
}


void Client::Impl::requestUpdates( bool *interrupted_out )
{
    _LockGuard lg( _mtx );
    _error = boost::system::error_code();

    if ( ! _connected ) {
        THROW_EXCEPTION(
                ClientStateException,
                ClientStateErrors::MustBeConnected,
                "Client must be connected when requesting updates."
            );
    }

    _receiving_events = true;

    LOG_DEBUG_MSG( "requestUpdates, sending filter='" << _filter << "' filterID=" << _filter_id );

    _out_msgs.push_back( AbstractMessage::ConstPtr( new StartRealtimeMessage( _filter, _filter_id ) ) );

    LOG_TRACE_MSG( "out messages size=" << _out_msgs.size() );

    if ( _blocking ) {
        _write( interrupted_out, NULL /*again_out*/, NULL /*end_out*/ );
    }
}


void Client::Impl::setFilter(
      const Filter::Impl& filter,
      Filter::Id* filter_id_out,
      bool* interrupted_out
    )
{
    _LockGuard lg( _mtx );
    _error = boost::system::error_code();

    _filter = filter;

    LOG_DEBUG_MSG( "setFilter, setting filter='" << _filter << "' filterID=" << _filter_id );

    // Only send the filter if connected and receiving events.

    if ( ! _connected || ! _receiving_events ) {
        *filter_id_out = _filter_id;
        return;
    }

    *filter_id_out = ++_filter_id;

    _out_msgs.push_back( AbstractMessage::ConstPtr( new StartRealtimeMessage( _filter, _filter_id ) ) );

    LOG_TRACE_MSG( "out messages size=" << _out_msgs.size() );

    if ( _blocking ) {
        _write( interrupted_out, NULL /*again_out*/, NULL /*end_out*/ );
    }
}


int Client::Impl::getPollDescriptor()
{
    _LockGuard lg( _mtx );

    if ( ! _connected ) {
        THROW_EXCEPTION(
                ClientStateException,
                ClientStateErrors::MustBeConnected,
                "Client must be connected when getting the poll descriptor."
            );
    }

    return _socket_ptr->lowest_layer().native();
}


void Client::Impl::sendMessages(
        bool* interrupted_out,
        bool* again_out
    )
{
    _LockGuard lg( _mtx );

    _error = boost::system::error_code();

    if ( ! _connected ) {
        THROW_EXCEPTION(
                ClientStateException,
                ClientStateErrors::MustBeConnected,
                "Client must be connected when sending messages."
            );
    }

    _write( interrupted_out, again_out, NULL /*end_out*/ );
}


void Client::Impl::receiveMessages(
        bool* interrupted_out,
        bool* again_out,
        bool* end_out
    )
{
    bool dummy;
    if ( ! again_out )  again_out = &dummy;
    if ( ! end_out )  end_out = &dummy;

    if ( interrupted_out )  *interrupted_out = false;
    *again_out = false;
    *end_out = false;

    _LockGuard lg( _mtx );

    _error = boost::system::error_code();

    if ( ! _connected ) {
        THROW_EXCEPTION(
                ClientStateException,
                ClientStateErrors::MustBeConnected,
                "Client must be connected when receiving messages."
            );
    }

    LOG_DEBUG_MSG( "receiving messages" );

    bool continue_reading(true);

    while ( continue_reading ) {
        // Continue reading messages until error or callback function indicates to quit

        // read a message from the socket.

        boost::system::error_code error;

        LOG_TRACE_MSG( "reading (" << (_blocking ? "blocking" : "non-blocking") << ")" );

        if ( ! _async_read_in_progress ) {
            LOG_TRACE_MSG( "calling async_read_until" );

            boost::asio::async_read_until(
                    *_socket_ptr,
                    _in_sb,
                    '\n',
                    boost::bind( &Impl::_readComplete, this, boost::asio::placeholders::error )
                );

            _async_read_in_progress = true;
        } else {
            LOG_DEBUG_MSG( "An async read was already in progress, waiting for it to finish." );
        }

        _io_service.reset();

        if ( _blocking ) {
            _io_service.run( error );
        } else {
            _io_service.poll( error );
        }

        LOG_TRACE_MSG( "after poll error=" << error << " _async_read_in_progress=" << _async_read_in_progress << " _in_sb=" << _in_sb.size() );

        if ( error ) {
            _disconnect();
            THROW_EXCEPTION(
                    ConnectionException,
                    ConnectionErrors::LostConnection,
                    "Lost connection to real-time server when polling for read. error=" << error
                );
        }

        if ( _async_read_in_progress ) {
            *again_out = true;
            return;
        }

        error = _error;

        // check for an error.
        if ( error ) {

            if ( error == boost::asio::error::interrupted ) { // interrupted
                if ( interrupted_out ) { // if client cares about interrupted then return with the indication.
                    *interrupted_out = true;
                    return;
                }
                // if caller doesn't care about interrupted just try again.
                _error = boost::system::error_code();
                continue;
            }

            if ( error == boost::asio::error::operation_aborted ) {
                // In this case, another thread probably called disconnect().

                LOG_DEBUG_MSG( "read operation was aborted" );

                *end_out = true;
                return;
            }

            _disconnect();

            if ( error == boost::asio::error::eof ) {
                LOG_INFO_MSG( "real-time server disconnected. The server may have shut down." );

                *end_out = true;
                return;
            }

            // Otherwise it's an unexpected error.

            THROW_EXCEPTION(
                    ConnectionException,
                    ConnectionErrors::LostConnection,
                    "Lost connection to real-time server when reading. error=" << error
                );
        }

        // Read the message off the input stream.

        istream is( &_in_sb );

        string msg_str;
        std::getline( is, msg_str );

        LOG_DEBUG_MSG( "in msg: '" << msg_str << "'" );

        _handleMessage( msg_str, &continue_reading );
    }

    LOG_TRACE_MSG( "receive messages completed" );
}


Client::Impl::~Impl()
{
    LOG_DEBUG_MSG( "destroyed client @" << this );
}


void Client::Impl::_write(
        bool *interrupted_out,
        bool *again_out,
        bool *end_out
    )
{
    bool dummy;
    if ( ! again_out )  again_out = &dummy;
    if ( ! end_out )  end_out = &dummy;

    *again_out = false;
    if ( interrupted_out )  *interrupted_out = false;

    boost::system::error_code error;

    while ( true ) { // keep writing until run out of messages to write or would block & non-blocking.
        if ( ! _async_write_in_progress ) {
            // No write in progress, so start a new one.

            // fill up the write buffer.
            _putOutMsgsToWriteBuf();

            // if the write buffer is empty then nothing to do.
            if ( _write_buf.empty() ) {
                return;
            }

            LOG_TRACE_MSG( "Writing " << _write_buf.size() << " bytes" );

            boost::asio::async_write( *_socket_ptr, boost::asio::buffer( _write_buf ),
                        boost::bind( &Impl::_writeComplete, this, boost::asio::placeholders::error() )
                    );

            _async_write_in_progress = true;
        } else {
            LOG_TRACE_MSG( "waiting for previous write to finish..." );
        }

        _io_service.reset();
        if ( _blocking ) {
            _io_service.run( error );
        } else {
            _io_service.poll( error );
        }

        if ( ! error )  error = _error; // error is either the error from io_service.run/poll or from write.

        if ( error ) {
            if ( error == boost::asio::error::interrupted ) {
                if ( interrupted_out ) { // if client cares about interrupted then return with the indication.
                    *interrupted_out = true;
                    return;
                }
                // if caller doesn't care about interrupted just try again.
                _error = boost::system::error_code();
                continue;
            }

            if ( error == boost::asio::error::operation_aborted ) {
                // In this case, another thread probably called disconnect().

                LOG_DEBUG_MSG( "write operation was aborted" );

                *end_out = true;
                return;
            }

            _disconnect();

            THROW_EXCEPTION(
                    ConnectionException,
                    ConnectionErrors::LostConnection,
                    "Lost connection to real-time server when polling for write. error=" << error
                );
        }

        if ( _async_write_in_progress ) { // did not complete (must be non-blocking)
            *again_out = true;
            return;
        }
    }
}


void Client::Impl::_handleMessage(
        const string& msg_str,
        bool *continue_reading
    )
{
    *continue_reading = true;

    AbstractMessage::ConstPtr msg_ptr;

    try {

        msg_ptr = AbstractMessage::fromString( msg_str );

    } catch ( std::exception& e ) {

        LOG_DEBUG_MSG( "Error deserializing message, exception is " << e.what() );

        THROW_EXCEPTION(
                ProtocolException,
                ProtocolErrors::MessageNotValid,
                "Could not parse message from server."
            );

    }


    {
        ClientEventListener::RealtimeStartedEventInfo::Pimpl pimpl(
                boost::dynamic_pointer_cast<const ClientEventListener::RealtimeStartedEventInfo::Impl>( msg_ptr )
            );

        if ( pimpl ) {

            ClientEventListener::RealtimeStartedEventInfo info( pimpl );

            _callOnEachListener(
                    boost::bind( &ClientEventListener::handleRealtimeStartedRealtimeEvent, _1, boost::ref(info) ),
                    continue_reading
                );

            return;
        }
    }

    {
        ClientEventListener::RealtimeEndedEventInfo::Pimpl pimpl(
                boost::dynamic_pointer_cast<const ClientEventListener::RealtimeEndedEventInfo::Impl>( msg_ptr )
            );

        if ( pimpl ) {

            ClientEventListener::RealtimeEndedEventInfo info( pimpl );

            _callOnEachListener(
                    boost::bind( &ClientEventListener::handleRealtimeEndedRealtimeEvent, _1, boost::ref(info) ),
                    continue_reading
                );

            return;
        }
    }

    {
        ClientEventListener::BlockAddedEventInfo::Pimpl pimpl(
                boost::dynamic_pointer_cast<const ClientEventListener::BlockAddedEventInfo::Impl>( msg_ptr )
            );

        if ( pimpl ) {

            ClientEventListener::BlockAddedEventInfo info( pimpl );

            _callOnEachListener(
                    boost::bind( &ClientEventListener::handleBlockAddedRealtimeEvent, _1, boost::ref(info) ),
                    continue_reading
                );

            return;
        }
    }

    {
        ClientEventListener::BlockStateChangedEventInfo::Pimpl pimpl(
                boost::dynamic_pointer_cast<const ClientEventListener::BlockStateChangedEventInfo::Impl>( msg_ptr )
            );

        if ( pimpl ) {

            ClientEventListener::BlockStateChangedEventInfo info( pimpl );

            _callOnEachListener(
                    boost::bind( &ClientEventListener::handleBlockStateChangedRealtimeEvent, _1, boost::ref(info) ),
                    continue_reading
                );

            return;
        }
    }

    {
        ClientEventListener::BlockDeletedEventInfo::Pimpl pimpl(
                boost::dynamic_pointer_cast<const ClientEventListener::BlockDeletedEventInfo::Impl>( msg_ptr )
            );

        if ( pimpl ) {

            ClientEventListener::BlockDeletedEventInfo info( pimpl );

            _callOnEachListener(
                    boost::bind( &ClientEventListener::handleBlockDeletedRealtimeEvent, _1, boost::ref(info) ),
                    continue_reading
                );

            return;
        }
    }

    {
        ClientEventListener::JobAddedEventInfo::Pimpl pimpl(
                boost::dynamic_pointer_cast<const ClientEventListener::JobAddedEventInfo::Impl>( msg_ptr )
            );

        if ( pimpl ) {

            ClientEventListener::JobAddedEventInfo info( pimpl );

            _callOnEachListener(
                    boost::bind( &ClientEventListener::handleJobAddedRealtimeEvent, _1, boost::ref(info) ),
                    continue_reading
                );

            return;
        }
    }

    {
        ClientEventListener::JobStateChangedEventInfo::Pimpl pimpl(
                boost::dynamic_pointer_cast<const ClientEventListener::JobStateChangedEventInfo::Impl>( msg_ptr )
            );

        if ( pimpl ) {

            ClientEventListener::JobStateChangedEventInfo info( pimpl );

            _callOnEachListener(
                    boost::bind( &ClientEventListener::handleJobStateChangedRealtimeEvent, _1, boost::ref(info) ),
                    continue_reading
                );

            return;
        }
    }

    {
        ClientEventListener::JobDeletedEventInfo::Pimpl pimpl(
                boost::dynamic_pointer_cast<const ClientEventListener::JobDeletedEventInfo::Impl>( msg_ptr )
            );

        if ( pimpl ) {

            ClientEventListener::JobDeletedEventInfo info( pimpl );

            _callOnEachListener(
                    boost::bind( &ClientEventListener::handleJobDeletedRealtimeEvent, _1, boost::ref(info) ),
                    continue_reading
                );

            return;
        }
    }

    {
        ClientEventListener::MidplaneStateChangedEventInfo::Pimpl pimpl(
                boost::dynamic_pointer_cast<const ClientEventListener::MidplaneStateChangedEventInfo::Impl>( msg_ptr )
            );

        if ( pimpl ) {

            ClientEventListener::MidplaneStateChangedEventInfo info( pimpl );

            _callOnEachListener(
                    boost::bind( &ClientEventListener::handleMidplaneStateChangedRealtimeEvent, _1, boost::ref(info) ),
                    continue_reading
                );

            return;
        }
    }

    {
        ClientEventListener::NodeBoardStateChangedEventInfo::Pimpl pimpl(
                boost::dynamic_pointer_cast<const ClientEventListener::NodeBoardStateChangedEventInfo::Impl>( msg_ptr )
            );

        if ( pimpl ) {

            ClientEventListener::NodeBoardStateChangedEventInfo info( pimpl );

            _callOnEachListener(
                    boost::bind( &ClientEventListener::handleNodeBoardStateChangedRealtimeEvent, _1, boost::ref(info) ),
                    continue_reading
                );

            return;
        }
    }

    {
        ClientEventListener::NodeStateChangedEventInfo::Pimpl pimpl(
                boost::dynamic_pointer_cast<const ClientEventListener::NodeStateChangedEventInfo::Impl>( msg_ptr )
            );

        if ( pimpl ) {

            ClientEventListener::NodeStateChangedEventInfo info( pimpl );

            _callOnEachListener(
                    boost::bind( &ClientEventListener::handleNodeStateChangedRealtimeEvent, _1, boost::ref(info) ),
                    continue_reading
                );

            return;
        }
    }

    {
        ClientEventListener::SwitchStateChangedEventInfo::Pimpl pimpl(
                boost::dynamic_pointer_cast<const ClientEventListener::SwitchStateChangedEventInfo::Impl>( msg_ptr )
            );

        if ( pimpl ) {

            ClientEventListener::SwitchStateChangedEventInfo info( pimpl );

            _callOnEachListener(
                    boost::bind( &ClientEventListener::handleSwitchStateChangedRealtimeEvent, _1, boost::ref(info) ),
                    continue_reading
                );

            return;
        }
    }

    {
        ClientEventListener::TorusCableStateChangedEventInfo::Pimpl pimpl(
                boost::dynamic_pointer_cast<const ClientEventListener::TorusCableStateChangedEventInfo::Impl>( msg_ptr )
            );

        if ( pimpl ) {

            ClientEventListener::TorusCableStateChangedEventInfo info( pimpl );

            _callOnEachListener(
                    boost::bind( &ClientEventListener::handleTorusCableStateChangedRealtimeEvent, _1, boost::ref(info) ),
                    continue_reading
                );

            return;
        }
    }

    {
        ClientEventListener::IoCableStateChangedEventInfo::Pimpl pimpl(
                boost::dynamic_pointer_cast<const ClientEventListener::IoCableStateChangedEventInfo::Impl>( msg_ptr )
            );

        if ( pimpl ) {

            ClientEventListener::IoCableStateChangedEventInfo info( pimpl );

            _callOnEachListener(
                    boost::bind( &ClientEventListener::handleIoCableStateChangedRealtimeEvent, _1, boost::ref(info) ),
                    continue_reading
                );

            return;
        }
    }

    {
        ClientEventListener::RasEventInfo::Pimpl pimpl(
                boost::dynamic_pointer_cast<const ClientEventListener::RasEventInfo::Impl>( msg_ptr )
            );

        if ( pimpl ) {

            ClientEventListener::RasEventInfo info( pimpl );

            _callOnEachListener(
                    boost::bind( &ClientEventListener::handleRasRealtimeEvent, _1, boost::ref(info) ),
                    continue_reading
                );

            return;
        }
    }

    {
        ClientEventListenerV2::IoDrawerStateChangedEventInfo::Pimpl pimpl(
                boost::dynamic_pointer_cast<const ClientEventListenerV2::IoDrawerStateChangedEventInfo::Impl>( msg_ptr )
            );

        if ( pimpl ) {

            ClientEventListenerV2::IoDrawerStateChangedEventInfo info( pimpl );

            for ( _Listeners::iterator i(_listener_ps.begin()) ; i != _listener_ps.end() ; ++i ) {
                ClientEventListener &l(**i);

                ClientEventListenerV2 *l2_p(dynamic_cast<ClientEventListenerV2*>(&l));

                if ( l2_p ) {
                    l2_p->handleIoDrawerStateChangedEvent( info );
                } else {
                    LOG_DEBUG_MSG( "Got IoDrawerStateChangedEvent but listener is version 1, ignoring." );
                }

                bool l_cont(l.getRealtimeContinue());
                if ( ! l_cont ) {
                    *continue_reading = false;
                }
            }

            return;
        }
    }

    {
        ClientEventListenerV2::IoNodeStateChangedEventInfo::Pimpl pimpl(
                boost::dynamic_pointer_cast<const ClientEventListenerV2::IoNodeStateChangedEventInfo::Impl>( msg_ptr )
            );

        if ( pimpl ) {

            ClientEventListenerV2::IoNodeStateChangedEventInfo info( pimpl );

            for ( _Listeners::iterator i(_listener_ps.begin()) ; i != _listener_ps.end() ; ++i ) {
                ClientEventListener &l(**i);

                ClientEventListenerV2 *l2_p(dynamic_cast<ClientEventListenerV2*>(&l));

                if ( l2_p ) {
                    l2_p->handleIoNodeStateChangedEvent( info );
                } else {
                    LOG_DEBUG_MSG( "Got IoNodeStateChangedEvent but listener is version 1, ignoring." );
                }

                bool l_cont(l.getRealtimeContinue());
                if ( ! l_cont ) {
                    *continue_reading = false;
                }
            }

            return;
        }
    }


    THROW_EXCEPTION(
            ProtocolException,
            ProtocolErrors::UnexpectedMessageType,
            "Improper message from real-time server has unexpected type."
        );
}


void Client::Impl::_putOutMsgsToWriteBuf()
{
    for ( _OutMsgs::const_iterator i(_out_msgs.begin()) ; i != _out_msgs.end() ; ++i ) {

        string msg_str(AbstractMessage::toString( *i ));

        LOG_DEBUG_MSG( "Sending message '" << msg_str << "'" );

        _write_buf += msg_str;
        _write_buf += "\n";
    }

    _out_msgs.clear();
}


void Client::Impl::_writeComplete( const boost::system::error_code &error )
{
    LOG_TRACE_MSG( "async write complete, error=" << error );

    if ( error ) {
        _error = error;
        return;
    }

    _async_write_in_progress = false;
    _write_buf.clear();
}


void Client::Impl::_readComplete( const boost::system::error_code &error )
{
    LOG_TRACE_MSG( "async read complete, error=" << error );

    _async_read_in_progress = false;

    if ( error ) {
        _error = error;
        return;
    }
}


void Client::Impl::_disconnect()
{
    LOG_DEBUG_MSG( "Disconnecting client." );

    _connected = false;

    boost::system::error_code error;
    _socket_ptr->next_layer().close( error );

    if ( error ) {
        THROW_EXCEPTION(
                InternalErrorException,
                InternalErrors::ApiUnexpectedFailure,
                "close() failed closing socket when disconnecting."
                    " The error is " << error
            );
    }
}


void Client::Impl::_callOnEachListener(
        boost::function<void (ClientEventListener&)> fn,
        bool* continue_reading_out
    )
{
    for ( _Listeners::iterator i(_listener_ps.begin()) ; i != _listener_ps.end() ; ++i ) {
        fn( **i );
        bool l_cont((*i)->getRealtimeContinue());
        if ( ! l_cont ) {
            *continue_reading_out = false;
        }
    }
}


} // namespace bgsched::realtime
} // namespace bgsched
