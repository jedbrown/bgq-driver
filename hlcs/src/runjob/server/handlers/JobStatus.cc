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
    CommandHandler( server )
{

}

void
JobStatus::handle(
        const runjob::commands::Request::Ptr& request,
        const CommandConnection::Ptr& connection
        )
{
    const runjob::commands::request::JobStatus::Ptr jobStatusRequest(
            boost::static_pointer_cast<runjob::commands::request::JobStatus>( request )
            );

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    // get job object
    server->getJobs()->find(
            jobStatusRequest->getId(),
            boost::bind(
                &JobStatus::findHandler,
                shared_from_this(),
                _1,
                jobStatusRequest,
                connection
                )
            );
}

void
JobStatus::findHandler(
        const Job::Ptr& job,
        runjob::commands::request::JobStatus::Ptr request,
        CommandConnection::Ptr connection
        )
{
    if ( !job ) {
        LOG_WARN_MSG( "could not find job " << request->getId() );
        runjob::commands::response::JobStatus::Ptr response( new runjob::commands::response::JobStatus );
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
                request->getId()
                );

        if ( !validate ) {
            runjob::commands::response::JobStatus::Ptr response( new runjob::commands::response::JobStatus );
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
