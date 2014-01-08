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
#include "mux/server/Handshake.h"

#include "common/message/convert.h"

#include "common/logging.h"
#include "common/Message.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::mux::log );

namespace runjob {
namespace mux {
namespace server {

void
Handshake::create(
        const Timer::SocketPtr& socket,
        const Callback& callback
        )
{
    const boost::shared_ptr<Handshake> result(
            new Handshake( socket, callback )
            );
    result->start();
}

Handshake::Handshake(
        const Timer::SocketPtr& socket,
        const Callback& callback
        ) :
    _status( error_code::handshake_failure ),
    _socket( socket ),
    _strand( socket->get_io_service() ),
    _timer( socket->get_io_service() ),
    _myVersion(),
    _theirVersion(),
    _incomingMessage(),
    _callback( callback )
{
    _myVersion._type = Message::Handshake;
}

Handshake::~Handshake()
{
    try {
        _callback( _status );
    } catch ( const std::exception& e) {
        LOG_FATAL_MSG( e.what() );
    }
}

void
Handshake::start()
{
    boost::asio::async_write(
            *_socket,
            boost::asio::buffer( &_myVersion, sizeof(_myVersion) ),
            _strand.wrap(
                boost::bind(
                    &Handshake::writeHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                    )
                )
            );

    // start timer
    _timer.expires_from_now(
            boost::posix_time::seconds( 10 )
            );
    _timer.async_wait(
            _strand.wrap(
                boost::bind(
                    &Handshake::timerHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error
                    )
                )
            );
}

void
Handshake::writeHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    if ( error ) {
        LOG_WARN_MSG( 
                "could not write: " << boost::system::system_error(error).what()
                );
        return;
    }

    LOG_TRACE_MSG( "wrote " << bytesTransferred << " bytes" );

    boost::asio::async_read(
            *_socket,
            boost::asio::buffer( &_theirVersion, sizeof(_theirVersion) ),
            _strand.wrap(
                boost::bind(
                    &Handshake::readHeaderHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                    )
                )
            );
}

void
Handshake::readHeaderHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    LOG_TRACE_MSG( "read " << bytesTransferred << " bytes" );

    if ( error ) {
        LOG_WARN_MSG( 
                "could not read: " << boost::system::system_error(error).what()
                );
        return;
    }

    boost::asio::async_read(
            *_socket,
            _incomingMessage.prepare( _theirVersion._length ),
            _strand.wrap(
                boost::bind(
                    &Handshake::readBodyHandler,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                    )
                )
            );
}

void
Handshake::readBodyHandler(
        const boost::system::error_code& error,
        const size_t bytesTransferred
        )
{
    LOG_TRACE_MSG( "read " << bytesTransferred << " bytes" );
    _incomingMessage.commit( bytesTransferred );

    if ( error ) {
        LOG_WARN_MSG( 
                "could not read: " << boost::system::system_error(error).what()
                );
        return;
    }

    const runjob::Message::Ptr msg = message::convert(_theirVersion, _incomingMessage);
    if ( !msg ) {
        LOG_WARN_MSG( "could not convert message" );
        return;
    }

    if ( msg->getType() != Message::Result ) {
        LOG_WARN_MSG( "expected " << Message::toString( Message::Result ) << " message" );
        return;
    }
    const runjob::message::Result::ConstPtr result = boost::static_pointer_cast<message::Result>( msg );

    _status = result->getError();
    if ( _status ) {
        LOG_WARN_MSG( "failed version handshake: " << result->getMessage() );
    } else {
        LOG_DEBUG_MSG( "version handshake (" << bgq::utility::Revision << ") passed");
    }

    // stop timer
    boost::system::error_code cancelError;
    _timer.cancel( cancelError );
    if ( cancelError ) {
        LOG_WARN_MSG( "could not cancel: " << boost::system::system_error( cancelError ).what() );
    }
}

void
Handshake::timerHandler(
        const boost::system::error_code& error
        )
{
    if ( error == boost::asio::error::operation_aborted ) {
        // do nothing, we were asked to cancel
        LOG_TRACE_MSG( "aborting" );
    } else if ( error ) {
        LOG_ERROR_MSG( "wait failed:  " << boost::system::system_error(error).what() );
    } else {
        LOG_WARN_MSG( "timed out" );
        
        // stop socket
        boost::system::error_code cancelError;
        _socket->next_layer().cancel( cancelError );
        if ( cancelError ) {
            LOG_WARN_MSG( "could not cancel: " << boost::system::system_error( cancelError ).what() );
        }
    }
}

} // server
} // mux
} // runjob
