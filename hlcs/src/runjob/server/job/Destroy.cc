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

#include "server/job/Destroy.h"

#include "common/message/ExitJob.h"
#include "common/message/Result.h"

#include "server/mux/ClientContainer.h"
#include "server/mux/Connection.h"
#include "server/database/Delete.h"
#include "server/database/Init.h"
#include "server/job/RasQuery.h"
#include "server/Server.h"

#include "common/logging.h"

#include <boost/bind.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

Destroy::Ptr
Destroy::create(
        const Server::WeakPtr& server,
        const job::ExitStatus& exitStatus
        )
{
    const Ptr result( new Destroy(server, exitStatus) ); 
   
    return result;
}

Destroy::Destroy(
        const Server::WeakPtr& server,
        const job::ExitStatus& exitStatus
        ) :
    _id( 0 ),
    _exitStatus( exitStatus ),
    _mux( ),
    _client( 0 ),
    _server( server ),
    _clientDisconnected( false )
{

}

Destroy::~Destroy()
{
    LOGGING_DECLARE_JOB_MDC( _id );
    LOG_DEBUG_MSG( __FUNCTION__ );

    // let client know job is done
    runjob::Message::Ptr message;

    if ( _started ) {
        const message::ExitJob::Ptr ej( new message::ExitJob() );
        ej->_status = _exitStatus.getStatus().get();
        ej->_error = _exitStatus.getError();
        ej->_message = _exitStatus.getMessage();

        if ( !_clientDisconnected ) {
            // end-of-job RAS query is only useful if there's a client
            // to actually display the results
            job::RasQuery( _id, _exitStatus ).add( ej );
        }

        message = ej;
    } else {
        const message::Result::Ptr result( new message::Result() );
        result->setError( _exitStatus.getError() );
        result->setMessage( _exitStatus.getMessage() );
        message = result;
    }

    message->setClientId( _client );
    message->setJobId( _id );
    if ( const mux::Connection::Ptr mux = _mux.lock() ) {
        mux->write( message );
    }

    // always remove client from mux client container
    if ( const mux::Connection::Ptr mux = _mux.lock() ) {
        mux->clients()->remove( _client );
    }
}

Destroy*
Destroy::mux(
        const boost::weak_ptr<mux::Connection>& m
        )
{
    _mux = m;
    return this;
}

Destroy*
Destroy::client_id(
        const uint64_t c
        )
{
    _client = c;
    return this;
}

Destroy*
Destroy::started(
        const bool s
        )
{
    _started = s;
    return this;
}

Destroy*
Destroy::id(
        const BGQDB::job::Id id
        )
{
    _id = id;

    const Server::Ptr server( _server.lock() );
    if ( !server ) return this;
    
    // remove entry from database if a job ID was generated
    if ( _id ) {
        server->getDatabase()->getDelete().execute(
                id,
                _exitStatus,
                boost::bind(
                    &Destroy::callback,
                    shared_from_this()
                    )
                );
    }

    return this;
}

Destroy*
Destroy::client_disconnected(
        const bool c
        )
{
    _clientDisconnected = c;
    return this;
}

void
Destroy::callback()
{
    LOGGING_DECLARE_JOB_MDC( _id );
    LOG_DEBUG_MSG( __FUNCTION__ );
}

} // job
} // server
} // runjob
