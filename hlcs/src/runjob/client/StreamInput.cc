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
#include "client/StreamInput.h"

#include "common/message/StdIo.h"

#include "client/MuxConnection.h"

#include "common/logging.h"

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::client::log );

namespace runjob {
namespace client {

StreamInput::StreamInput(
        boost::asio::io_service& io_service,
        const boost::weak_ptr<MuxConnection>& mux,
        int fd
        ) :
    Input( mux ),
    _input( io_service, fd ),
    _buffer()
{
    LOG_DEBUG_MSG( "stdin " << fd );
}

void
StreamInput::read(
        uint32_t rank,
        size_t length,
        const Uci& location
        )
{
    LOG_TRACE_MSG( "reading " << length << " bytes" );
    boost::asio::async_read(
            _input,
            _buffer.prepare( length ),
            boost::bind(
                &StreamInput::readHandler,
                shared_from_this(),
                rank,
                location,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
                )
            );
}

void
StreamInput::stop()
{
    boost::system::error_code error;
    _input.cancel( error );
    if ( error ) {
        LOG_TRACE_MSG( "cancel: " << boost::system::system_error(error).what() );
    }
}

void
StreamInput::readHandler(
        uint32_t rank,
        const Uci& location,
        const boost::system::error_code& error,
        size_t length
        )
{
    LOG_TRACE_MSG( "read " << length << " bytes" );
    _buffer.commit( length );

    if ( error == boost::asio::error::operation_aborted ) {
        // asked to stop reading
        LOG_TRACE_MSG( "done reading stdin" );

        return;
    }

    if ( error ) {
        if ( error != boost::asio::error::eof ) {
            // some other error
            boost::system::system_error e(error);
            LOG_ERROR_MSG( "could not read input: " << e.what() );

            return;
        }

        LOG_TRACE_MSG( "eof on stdin" );
        // fall through
    }

    const message::StdIo::Ptr msg( new message::StdIo() );
    msg->setType( Message::StdIn );
    if ( length > 0 ) {
        msg->setData(
                boost::asio::buffer_cast<const char*>( _buffer.data() ),
                length
                );
    }
    msg->setRank( rank );
    msg->setLocation( location );
    
    const boost::shared_ptr<MuxConnection> mux( _mux.lock() );
    if ( !mux ) return;

    // forward to mux
    mux->write( msg );
}

} // client
} // runjob
