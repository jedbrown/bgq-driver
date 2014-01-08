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
#include "server/handlers/JobStatus.h"

#include "common/commands/JobStatus.h"

#include "common/logging.h"

#include "server/job/Container.h"
#include "server/job/JobStatus.h"

#include "server/CommandConnection.h"
#include "server/Job.h"
#include "server/Security.h"
#include "server/Server.h"

#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace handlers {

JobStatus::JobStatus(
        const Server::Ptr& server
        ) :
    CommandHandler( server ),
    _request( )
{

}

void
JobStatus::lookupJob(
        const CommandConnection::Ptr& connection
        )
{
    if ( !_request->_pid ) {
        const runjob::commands::response::JobStatus::Ptr response( new runjob::commands::response::JobStatus );
        response->setMessage( "empty pid in request" );
        response->setError( runjob::commands::error::job_not_found );
        connection->write( response );
        return;
    }

    if ( _request->_hostname.empty() ) {
        _request->_hostname = connection->hostname();
        LOG_DEBUG_MSG( "using hostname '" << _request->_hostname << "' for pid " << _request->_pid );
    }

    const cxxdb::ConnectionPtr db(
            BGQDB::DBConnectionPool::Instance().getConnection()
            );
    if ( !db ) {
        const runjob::commands::response::JobStatus::Ptr response( new runjob::commands::response::JobStatus );
        response->setError( runjob::commands::error::database_error );
        response->setMessage ("could not get database connection" );
        connection->write( response );
        return;
    }

    const BGQDB::job::Id job = this->getJob(
            db,
            _request->_pid,
            _request->_hostname
            );

    if ( !job ) {
        const runjob::commands::response::JobStatus::Ptr response( new runjob::commands::response::JobStatus );
        response->setError( runjob::commands::error::job_not_found );
        response->setMessage(
                "could not find job associated with runjob pid " +
                boost::lexical_cast<std::string>( _request->_pid ) + 
                " on host '" +
                _request->_hostname +
                "'"
                );
        connection->write( response );
        return;
    }

    LOG_INFO_MSG( "associated " << job << " with pid " << _request->_pid );
    _request->_job = job;

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    // get job object
    server->getJobs()->find(
            job,
            boost::bind(
                &JobStatus::findHandler,
                shared_from_this(),
                _1,
                connection
                )
            );
}

void
JobStatus::handle(
        const runjob::commands::Request::Ptr& request,
        const CommandConnection::Ptr& connection
        )
{
    _request = boost::static_pointer_cast<runjob::commands::request::JobStatus>( request );

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    if ( _request->_job ) {
        server->getJobs()->find(
            _request->_job,
            boost::bind(
                &JobStatus::findHandler,
                shared_from_this(),
                _1,
                connection
                )
            );

        return;
    }

    this->lookupJob( connection );
}

void
JobStatus::findHandler(
        const Job::Ptr& job,
        const CommandConnection::Ptr& connection
        )
{
    if ( !job ) {
        LOG_WARN_MSG( "could not find job " << _request->_job );
        const runjob::commands::response::JobStatus::Ptr response( new runjob::commands::response::JobStatus );
        response->setError( runjob::commands::error::job_not_found );
        connection->write( response );
        return;
    }

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    if ( connection->getUserType() == bgq::utility::portConfig::UserType::Administrator ) {
        // user presented administrative certificate, let them do anything
        //
        // fall through
        LOG_DEBUG_MSG( "skipping security check for administrator: " << connection->getUser()->getUser() );
    } else if ( connection->getUserType() == bgq::utility::portConfig::UserType::Normal ) {
        // info about a job requires read authority
        const bool validate = server->getSecurity()->validate(
                connection->getUser(),
                hlcs::security::Action::Read,
                _request->_job
                );

        if ( !validate ) {
            const runjob::commands::response::JobStatus::Ptr response( new runjob::commands::response::JobStatus );
            response->setError( runjob::commands::error::permission_denied );
            connection->write( response );
            return;
        }
    } else {
        BOOST_ASSERT( !"unhandled user type" );
    }

    job::JobStatus::create( job, connection );
}

} // handlers
} // server
} // runjob
