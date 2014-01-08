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
#include "server/job/StdioQueue.h"

#include "common/message/StdIo.h"

#include "common/logging.h"

#include "server/job/Status.h"

#include "server/mux/Connection.h"

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

StdioQueue::StdioQueue(
        const boost::weak_ptr<mux::Connection>& mux
        ) :
    _drained( false ),
    _clientDisconnected( false ),
    _mux( mux ),
    _queue()
{

}

void
StdioQueue::add(
        const message::StdIo::Ptr& message,
        const Callback& callback
        )
{
    if ( _clientDisconnected ) {
        callback();
        return;
    }

    if ( !_drained ) {
        _queue.push_back( message );
        LOG_TRACE_MSG( "size " << _queue.size() );

        callback();
        return;
    }

    if ( const mux::Connection::Ptr mux = _mux.lock() ) {
        mux->write( message, callback );
    } else {
        LOG_TRACE_MSG( "dropping" );
        callback();
    }
}

void
StdioQueue::drain()
{
    _drained = true;

    const mux::Connection::Ptr mux( _mux.lock() );
    if ( !mux ) return;

    LOG_TRACE_MSG( "draining " << _queue.size() << " entries" );
    BOOST_FOREACH( const message::StdIo::Ptr& msg, _queue ) {
        mux->write( msg );
    }

    _queue.clear();
}

void
StdioQueue::clientDisconnected()
{
    if ( !_clientDisconnected ) {
        LOG_INFO_MSG( "client disconnected, dropping output" );
    }

    _clientDisconnected = true;
}

} // job
} // server
} // runjob
