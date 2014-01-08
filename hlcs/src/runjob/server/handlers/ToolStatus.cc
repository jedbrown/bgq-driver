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
#include "server/handlers/ToolStatus.h"

#include "common/logging.h"

#include "server/job/Container.h"
#include "server/job/tool/ToolStatus.h"

#include "server/CommandConnection.h"
#include "server/Job.h"
#include "server/Options.h"
#include "server/Security.h"
#include "server/Server.h"

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace handlers {

ToolStatus::ToolStatus(
        const Server::Ptr& server
        ) :
    CommandHandler( server ),
    _request(),
    _response( new runjob::commands::response::ToolStatus ),
    _connection(),
    _job()
{

}

ToolStatus::~ToolStatus()
{
    LOG_TRACE_MSG( __FUNCTION__ );

    // only send response if an error occurred
    if ( _response->getError() ) {
        LOG_WARN_MSG( _response->getMessage() );
        _connection->write( _response );
    }
}

void
ToolStatus::handle(
        const runjob::commands::Request::Ptr& request,
        const CommandConnection::Ptr& connection
        )
{
    _request = boost::static_pointer_cast<runjob::commands::request::ToolStatus>( request );
    _connection = connection;

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    if ( _request->_job ) {
        // get job object
        server->getJobs()->find(
                _request->_job,
                boost::bind(
                    &ToolStatus::findHandler,
                    shared_from_this(),
                    _1
                    )
                );
        return;
    }

    this->lookupJob();
}

void
ToolStatus::lookupJob()
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
        _response->setMessage( "could not get database connection" );
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

    _request->_job = job;

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    // get job object
    server->getJobs()->find(
            job,
            boost::bind(
                &ToolStatus::findHandler,
                shared_from_this(),
                _1
                )
            );
}

void
ToolStatus::findHandler(
        const Job::Ptr& job
        )
{
    if ( !job ) {
        _response->setMessage( 
                "could not find job " + 
                boost::lexical_cast<std::string>( _request->_job )
                );
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
        // tool status requires read authority
        const bool validate = server->getSecurity()->validate(
                _connection->getUser(),
                hlcs::security::Action::Read,
                _request->_job
                );

        if ( !validate ) {
            _response->setError( runjob::commands::error::permission_denied );
            return;
        }
    } else {
        BOOST_ASSERT( !"unhandled user type" );
    }

    job::tool::ToolStatus::create( job, _request, _connection );
}

} // handlers
} // server
} // runjob
