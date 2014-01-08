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
#include "server/mux/Reconnect.h"

#include "server/job/Container.h"

#include "server/mux/ClientContainer.h"
#include "server/mux/Connection.h"

#include "server/Job.h"
#include "server/Options.h"
#include "server/Server.h"

#include "common/logging.h"

#include <db/include/api/tableapi/gensrc/DBTJob.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace mux {

void
Reconnect::create(
        const Server::Ptr& server,
        const Connection::Ptr& mux
        )
{
    if ( server->getOptions().reconnect().scope() != runjob::server::Reconnect::Scope::Jobs ) {
        LOG_INFO_MSG( "skipping" );
        return;
    }

    try {
        const Ptr result(
                new Reconnect( server, mux )
                );

        result->nextJob();
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }
}

Reconnect::Reconnect(
        const Server::Ptr& server,
        const Connection::Ptr& mux
        ) :
    _server( server ),
    _mux( mux ),
    _connection( BGQDB::DBConnectionPool::instance().getConnection() ),
    _query(),
    _results()
{
    LOGGING_DECLARE_LOCATION_MDC( mux->hostname() );

    if ( !_connection ) {
        LOG_WARN_MSG( "could not get database connection" );
        return;
    }

    _query = _connection->prepareQuery(
            "SELECT " +
            BGQDB::DBTJob::ID_COL + 
            " FROM TBGQJob" +
            " WHERE " +
            BGQDB::DBTJob::HOSTNAME_COL + "=?",
            boost::assign::list_of
            (BGQDB::DBTJob::HOSTNAME_COL)
            );
    _query->parameters()[ BGQDB::DBTJob::HOSTNAME_COL ].set( mux->hostname() );
    LOG_INFO_MSG( "querying for jobs" );
    _results = _query->execute();
}

Reconnect::~Reconnect()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

void
Reconnect::nextJob()
{
    if ( !_results->fetch() ) {
        LOG_INFO_MSG( "done" );
        return;
    }

    const Connection::Ptr mux( _mux.lock() );
    if ( !mux ) return;

    const cxxdb::Columns& columns = _results->columns();

    LOGGING_DECLARE_LOCATION_MDC( mux->hostname() );
    LOGGING_DECLARE_JOB_MDC( columns[BGQDB::DBTJob::ID_COL].as<BGQDB::job::Id>() );

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    server->getJobs()->find(
            columns[BGQDB::DBTJob::ID_COL].as<BGQDB::job::Id>(),
            boost::bind(
                &Reconnect::findJob,
                shared_from_this(),
                _1
                )
            );
}

void
Reconnect::findJob(
        const Job::Ptr& job
        )
{
    const Connection::Ptr mux( _mux.lock() );
    if ( !mux ) return;

    const cxxdb::Columns& columns = _results->columns();

    LOGGING_DECLARE_LOCATION_MDC( mux->hostname() );
    LOGGING_DECLARE_JOB_MDC( columns[BGQDB::DBTJob::ID_COL].as<BGQDB::job::Id>() );

    if ( !job ) {
        LOG_WARN_MSG( "could not find job" );
        this->nextJob();
        return;
    }

    LOG_TRACE_MSG( __FUNCTION__ );

    mux->clients()->add(
            job->client(),
            boost::bind(
                &Reconnect::addClient,
                shared_from_this(),
                job
                )
            );
}

void
Reconnect::addClient(
        const Job::Ptr& job
        )
{
    const Connection::Ptr mux( _mux.lock() );
    if ( !mux ) return;

    LOGGING_DECLARE_LOCATION_MDC( mux->hostname() );
    LOGGING_DECLARE_JOB_MDC( job->id() );

    LOG_TRACE_MSG( __FUNCTION__ << " " << job->client() );

    mux->clients()->update(
            job->client(),
            job,
            boost::bind(
                &Reconnect::updateClient,
                shared_from_this(),
                job
                )
            );
}

void
Reconnect::updateClient(
        const Job::Ptr& job
        )
{
    const Connection::Ptr mux( _mux.lock() );
    if ( !mux ) return;

    LOGGING_DECLARE_LOCATION_MDC( mux->hostname() );
    LOGGING_DECLARE_JOB_MDC( job->id() );

    LOG_TRACE_MSG( __FUNCTION__ );

    job->strand().post(
            boost::bind(
                &Reconnect::setConnection,
                shared_from_this(),
                job
                )
            );
}

void
Reconnect::setConnection(
        const Job::Ptr& job
        )
{
    const Connection::Ptr mux( _mux.lock() );
    if ( !mux ) return;

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    LOGGING_DECLARE_LOCATION_MDC( mux->hostname() );
    LOGGING_DECLARE_JOB_MDC( job->id() );

    job->_mux = mux;
    job->_queue._mux = mux;

    if ( job->status().get() == job::Status::Terminating ) {
        // jobs that terminated during failover can be removed
        server->getJobs()->remove( job->id() );
        LOG_INFO_MSG( "removed" );
    } else {
        LOG_INFO_MSG( "reconnected" );
    }

    this->nextJob();
}

} // mux
} // server
} // runjob
