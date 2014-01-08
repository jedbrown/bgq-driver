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
#include "client/FileInput.h"

#include "common/message/StdIo.h"

#include "client/MuxConnection.h"

#include "common/logging.h"

#include <boost/bind.hpp>
#include <boost/scoped_array.hpp>

LOG_DECLARE_FILE( runjob::client::log );

namespace runjob {
namespace client {

FileInput::FileInput(
        const boost::weak_ptr<MuxConnection>& mux,
        const int fd
        ) :
    Input( mux ),
    _fd( fd )
{
    LOG_DEBUG_MSG( "stdin " << fd );
}

void
FileInput::read(
        const uint32_t rank,
        const size_t length,
        const Uci& location
        )
{
    const boost::scoped_array<char> buf( new char[length] );

    LOG_TRACE_MSG( "reading " << length << " bytes" );
    const ssize_t rc = ::read( _fd, buf.get(), length );
    const int error = errno;

    if ( rc == 0 ) {
        // done reading
        LOG_TRACE_MSG( "eof on stdin" );

        // fall through
    } else if ( rc == -1 ) {
        LOG_ERROR_MSG( "could not read stdin: " << strerror(error) );
        return;
    }

    LOG_TRACE_MSG( "read " << rc << " bytes" );

    const message::StdIo::Ptr msg( new message::StdIo() );
    msg->setType( Message::StdIn );
    msg->setRank( rank );
    msg->setLocation( location );
    if ( rc ) {
        msg->setData(
                buf.get(),
                rc
                );
    }

    const boost::shared_ptr<MuxConnection> mux( _mux.lock() );
    if ( !mux ) return;
    mux->write( msg );
}

} // client
} // runjob
