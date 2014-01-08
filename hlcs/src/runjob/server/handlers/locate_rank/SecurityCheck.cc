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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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
#include "server/handlers/locate_rank/SecurityCheck.h"

#include "server/handlers/locate_rank/ActiveJob.h"
#include "server/handlers/locate_rank/HistoryJob.h"
#include "server/handlers/locate_rank/Job.h"

#include "common/logging.h"

#include "server/CommandConnection.h"
#include "server/Security.h"
#include "server/Server.h"

#include <boost/throw_exception.hpp>
#include <boost/weak_ptr.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace handlers {
namespace locate_rank {

SecurityCheck::SecurityCheck(
        const boost::weak_ptr<Server>& server,
        const CommandConnection::Ptr& connection
        ) :
    _server( server ),
    _connection( connection ),
    _message()
{

}

bool
SecurityCheck::operator()(
        const Job::Ptr& job
        ) 
{
    if ( _connection->getUserType() == bgq::utility::portConfig::UserType::Administrator ) {
        // user presented administrative certificate, let them do anything
        LOG_DEBUG_MSG( "skipping due to administrator: " << _connection->getUser()->getUser() );
        return true;
    }

    if ( _connection->getUserType() == bgq::utility::portConfig::UserType::Normal ) {
        if ( boost::shared_ptr<ActiveJob> active_job = boost::dynamic_pointer_cast<ActiveJob>(job) ) {
            return this->impl( active_job );
        }

        if ( boost::shared_ptr<HistoryJob> history_job = boost::dynamic_pointer_cast<HistoryJob>(job) ) {
            return this->impl( history_job );
        }

        // some other type of job
        BOOST_ASSERT( !"shouldn't get here" );
    }

    // neither administrative or command certificate was presented??
    BOOST_ASSERT( !"shouldn't get here" );
}

bool
SecurityCheck::impl(
        const boost::shared_ptr<ActiveJob>& job
        ) 
{
    // active jobs can use normal security checks
    const Server::Ptr server( _server.lock() );
    if ( !server ) return false;

    const bool valid = server->getSecurity()->validate(
            _connection->getUser(),
            hlcs::security::Action::Read,
            job->id()
            );

    if ( !valid ) {
        _message <<
            _connection->getUser()->getUser() <<
            " denied read authority on job " <<
            job->id()
            ;
    }

    LOG_TRACE_MSG( _connection->getUser()->getUser() << " granted read authority" );
    return valid;
}

bool
SecurityCheck::impl(
        const boost::shared_ptr<HistoryJob>& job
        ) 
{
    // only the owner can query a history job
    if ( _connection->getUser()->getUser() != job->owner() ) {
        _message <<
            _connection->getUser()->getUser() <<
            " is not the owner of job " << 
            job->id()
            ;
        return false;
    }

    LOG_TRACE_MSG( _connection->getUser()->getUser() << " owned the job" );

    return true;
}

} // locate_rank
} // handlers
} // server
} // runjob

