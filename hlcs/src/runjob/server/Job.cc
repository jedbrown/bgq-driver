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
#include "common/error.h"
#include "common/Exception.h"
#include "common/logging.h"
#include "common/JobInfo.h"
#include "common/properties.h"

#include "server/block/Compute.h"
#include "server/block/IoNode.h"

#include "server/database/Init.h"
#include "server/database/NodeStatus.h"

#include "server/job/Container.h"
#include "server/job/Create.h"
#include "server/job/Destroy.h"
#include "server/job/SubNodePacing.h"

#include "server/mux/Connection.h"

#include "server/Job.h"
#include "server/Options.h"
#include "server/Server.h"

#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {

Job::Job(
        const job::Create& create
        ) :
    _status( create._server ),
    _info( create._job_info ),
    _strand( create._server->getIoService() ),
    _id( create._reconnect ? create._reconnect : 0 ),
    _io( create._io ),
    _mux( create._mux ),
    _queue( create._mux ),
    _exitStatus(),
    _killTimer( create._server, create._job_info ),
    _heartbeat( create._server ),
    _block( create._block ),
    _client( create._client_id ),
    _server( create._server ),
    _tools(),
    _pacing( create._pacing ),
    _outstandingSignal( 0 )
{
    LOGGING_DECLARE_JOB_MDC( _id ? _id : _client );
    LOGGING_DECLARE_BLOCK_MDC( _info.getBlock() );
    LOGGING_DECLARE_USER_MDC( _info.getUserId()->getUser() );

    if ( create._error ) {
        this->setError(
                create._message,
                create._error
                );

        return;
    }
    
    if ( !_id ) {
        LOG_INFO_MSG( "creating" );
        return;
    }

    LOG_INFO_MSG( "reconnecting" );
    _queue.drain();
    BOOST_FOREACH( auto& i, _io ) {
        job::IoNode& io = i.second;
        io._flags.set( job::Status::Running );
        io.drained( true );
    }
}

Job::~Job()
{
    LOGGING_DECLARE_JOB_MDC( _id );
    LOGGING_DECLARE_BLOCK_MDC( _info.getBlock() );
    LOG_DEBUG_MSG( __FUNCTION__ );

    if ( _exitStatus.getError() == runjob::error_code::runjob_server_restart ) {
        // special case when we are shutting down, there's no need to queue
        // up any messages to the mux or database operations if the I/O service
        // is about to be stopped
        return;
    }

    job::Destroy::create( _server, _exitStatus )->
        id( _id )->
        mux( _mux )->
        client_id( _client )->
        started( _status.started() )->
        client_disconnected( _queue.isClientDisconnected() )
        ;
}

void
Job::setError(
        const std::string& message,
        const runjob::error_code::rc error
        )
{
    if ( error != error_code::runjob_server_shutdown ) {
        LOG_INFO_MSG( "error code:    " << runjob::error_code::toString(error) );
        LOG_INFO_MSG( "error message: " << "'" << message << "'" );
    } else {
        LOG_DEBUG_MSG( "error code:    " << runjob::error_code::toString(error) );
        LOG_DEBUG_MSG( "error message: " << "'" << message << "'" );
    }

    _exitStatus.set( message, error );
}

void
Job::remove()
{
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    bool skipSoftwareFailure = false;
    try {
        const std::string& value = server->getOptions().getProperties()->getValue(
                PropertiesSection,
                "skip_software_failure"
                );
        std::istringstream is( value );
        is >> std::boolalpha >> skipSoftwareFailure;
    } catch ( const std::exception& e ) {
        // not specified, assume we should handle
        // kill timeouts normally
    }

    if ( !skipSoftwareFailure ) {
        // can also be specified on a per job basis with an environment variable
        const JobInfo::EnvironmentVector& envs = _info.getEnvs();
        BOOST_FOREACH( const Environment& i, envs ) {
            skipSoftwareFailure = ( i.getKey() == "RUNJOB_SKIP_SOFTWARE_FAILURE" );
            if ( skipSoftwareFailure ) break;
        }
    }

    try {
        const bool killTimeout = _exitStatus.getError() == error_code::kill_timeout;
        if ( skipSoftwareFailure && killTimeout ) {
            LOG_WARN_MSG( "skipping Software Failure step" );
        } else if ( killTimeout ) {
            // mark the nodes in use as SOFTWARE (F)AILURE
            database::NodeStatus update(
                    _block,
                    _info
                    );
            update.execute();
        }
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    if ( mux::Connection::Ptr mux = _mux.lock() ) {
        server->getJobs()->remove( _id );
    } else if ( _queue.isClientDisconnected() ) {
        server->getJobs()->remove( _id );
    } else {
        // no mux connection and the client has not been disconnected,
        // means this job ended during a failover event.
        _status.set( job::Status::Terminating, shared_from_this() );
    }

    // cancel kill and heartbeat timers
    _killTimer.stop();
    _heartbeat.stop();
}

} // server
} // runjob
