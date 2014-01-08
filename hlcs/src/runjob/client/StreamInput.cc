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
#include <boost/scoped_array.hpp>

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
    LOG_DEBUG_MSG( "stdin  " << fd );
    LOG_DEBUG_MSG( "isatty " << (isatty(fd) ? "true" : "false") );
}

void
StreamInput::read(
        const uint32_t rank,
        const size_t length,
        const Uci& location
        )
{
    if ( !isatty(_input.native()) ) {
        LOG_TRACE_MSG( "reading " << length << " bytes" );
        boost::asio::async_read(
                _input,
                _buffer.prepare( length ),
                boost::bind(
                    &StreamInput::readHandler,
                    shared_from_this(),
                    rank,
                    length,
                    location,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                    )
                );
    } else {
        LOG_TRACE_MSG( "reading until newline" );
        boost::asio::async_read_until(
                _input,
                _buffer,
                '\n',
                boost::bind(
                    &StreamInput::readHandler,
                    shared_from_this(),
                    rank,
                    length,
                    location,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                    )
                );
    }
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
        const uint32_t rank,
        const uint32_t length,
        const Uci& location,
        const boost::system::error_code& error,
        const size_t bytes_transferred
        )
{
    LOG_TRACE_MSG( 
            "read " << bytes_transferred << " bytes of requested " << length <<
            ": " << boost::system::system_error(error).what()
            );

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
    if ( !bytes_transferred ) {
        // no data, send an empty message
    } else if ( !isatty(_input.native()) ) {
        // block buffered input, send everything they asked for
        _buffer.commit( bytes_transferred );
        msg->setData(
                boost::asio::buffer_cast<const char*>( _buffer.data() ),
                bytes_transferred
                );
    } else if ( bytes_transferred > length ) {
        // line buffered input, but we read more than was requested
        std::istream is( &_buffer );
        boost::scoped_array<char> buf( new char[length] );
        is.get( buf.get(), length );
        msg->setData( buf.get(), length );
    } else {
        // line buffered input
        std::istream is( &_buffer );
        std::string line;
        std::getline( is, line );
        line.append( "\n" );
        msg->setData( line.c_str(), line.size() );
    }
    LOG_TRACE_MSG( msg->getData() );
    msg->setRank( rank );
    msg->setLocation( location );
    
    const boost::shared_ptr<MuxConnection> mux( _mux.lock() );
    if ( !mux ) return;

    // forward to mux
    mux->write( msg );
}

} // client
} // runjob
