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
#include "server/handlers/KillJob.h"

#include "server/job/Container.h"
#include "server/job/Signal.h"

#include "server/Job.h"

#include "common/defaults.h"
#include "common/logging.h"

#include "server/Security.h"
#include "server/Server.h"

#include "server/CommandConnection.h"

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace handlers {

KillJob::KillJob(
        const Server::Ptr& server
        ) :
    CommandHandler( server ),
    _request( ),
    _response( new runjob::commands::response::KillJob ),
    _connection( )
{

}

KillJob::~KillJob()
{
    LOG_TRACE_MSG( __FUNCTION__ );

    if ( !_response ) return;
    if ( !_connection ) return;

    try {
        _connection->write( _response );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    } catch ( ... ) {
        LOG_FATAL_MSG( "caught some other exception" );
    }
}

void
KillJob::lookupJob()
{
    if ( !_request->_pid ) {
        _response->setMessage( "empty pid in request" );
        _response->setError( runjob::commands::error::job_not_found );
        return;
    }

    if ( _request->_hostname.empty() ) {
        _request->_hostname = _connection->hostname();
        LOG_DEBUG_MSG( "using hostname '" << _request->_hostname << "' for pid " << _request->_pid );
    }

    const cxxdb::ConnectionPtr db(
            BGQDB::DBConnectionPool::Instance().getConnection()
            );
    if ( !db ) {
        _response->setError( runjob::commands::error::database_error );
        _response->setMessage ("could not get database connection" );
        return;
    }

    const BGQDB::job::Id job = this->getJob(
            db,
            _request->_pid,
            _request->_hostname
            );

    if ( !job ) {
        _response->setError( runjob::commands::error::job_not_found );
        _response->setMessage(
                "could not find job associated with runjob pid " +
                boost::lexical_cast<std::string>( _request->_pid ) + 
                " on host '" +
                _request->_hostname +
                "'"
                );
        return;
    }

    LOG_INFO_MSG( "associated " << job << " with pid " << _request->_pid );
    _request->_job = job;
    _response->_job = job;

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    // get job object
    server->getJobs()->find(
            job,
            boost::bind(
                &KillJob::findJobCallback,
                shared_from_this(),
                _1
                )
            );
}

void
KillJob::handle(
        const runjob::commands::Request::Ptr& request,
        const CommandConnection::Ptr& connection
        )
{
    _connection = connection;

    _request = boost::static_pointer_cast<runjob::commands::request::KillJob>( request );

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    if ( _request->_job ) {
        _response->_job = _request->_job;
        server->getJobs()->find(
                _request->_job,
                boost::bind(
                    &KillJob::findJobCallback,
                    shared_from_this(),
                    _1
                    )
                );

        return;
    }
    
    this->lookupJob();
}

void
KillJob::findJobCallback(
        const Job::Ptr& job
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );

    if ( !job ) {
        LOG_WARN_MSG( "could not find job " << _request->_job );
        _response->setError( runjob::commands::error::job_not_found );
        return;
    }

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    if ( _connection->getUserType() == bgq::utility::portConfig::UserType::Administrator ) {
        // user presented administrative certificate, let them do anything
        //
        // fall through
        LOG_DEBUG_MSG( "skipping security check for administrator: " << _connection->getUser()->getUser() );
    } else if ( _connection->getUserType() == bgq::utility::portConfig::UserType::Normal ) {
        // signaling a job requires execute authority
        const bool validate = server->getSecurity()->validate(
                _connection->getUser(),
                hlcs::security::Action::Execute,
                _request->_job
                );

        if ( !validate ) {
            _response->setError( runjob::commands::error::permission_denied );
            return;
        }
    } else {
        BOOST_ASSERT( !"unhandled user type" );
    }

    // handle invalid timeout
    if ( _request->_timeout <= 0 ) {
        _request->_timeout = defaults::ServerKillJobTimeout;
        LOG_TRACE_MSG( "set timeout to " << _request->_timeout );
    }

    job::Signal::create(
            job,
            _request->_signal,
            _request->_timeout,
            job->strand().wrap(
                boost::bind(
                    &KillJob::callback,
                    shared_from_this(),
                    job,
                    _1,
                    _2
                    )
                )
            );
}

void
KillJob::callback(
        const Job::Ptr& job,
        runjob::commands::error::rc error,
        const std::string& message
        )
{
    LOGGING_DECLARE_JOB_MDC( job->id() );
    LOGGING_DECLARE_BLOCK_MDC( job->info().getBlock() );

    if ( _connection->getUserType() == bgq::utility::portConfig::UserType::Administrator && _request->_controlActionRecordId ) {
        // only allow administrative connections to indicate the job was signaled by a control action
        job->exitStatus().setRecordId( _request->_controlActionRecordId );
    } else if ( !_request->_details.empty() ) {
        job->exitStatus().setDetails( _request->_details );
    }

    _response->setError( error );
    _response->setMessage( message );
}

} // handlers
} // server
} // runjob
