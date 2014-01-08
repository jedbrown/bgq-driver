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
#include "server/job/Reconnect.h"

#include "common/logging.h"
#include "common/JobInfo.h"

#include "server/block/Compute.h"
#include "server/block/Container.h"

#include "server/cios/Reconnect.h"

#include "server/database/Delete.h"
#include "server/database/Init.h"

#include "server/job/Create.h"
#include "server/job/Status.h"

#include "server/mux/Listener.h"

#include "server/Job.h"
#include "server/Options.h"
#include "server/Server.h"

#include <db/include/api/tableapi/gensrc/DBTJob.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

void
Reconnect::create(
        const Server::Ptr& server
        )
{
    try {
        const Reconnect::Ptr jobs(
                new Reconnect( server )
                );
        jobs->nextJob();
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }
}

Reconnect::~Reconnect()
{
    LOG_TRACE_MSG( __FUNCTION__ );
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    cios::Reconnect::create( server );

    server->getMuxListener()->start();
}

Reconnect::Reconnect(
        const Server::Ptr& server
        ) :
    _server( server ),
    _connection( BGQDB::DBConnectionPool::instance().getConnection() ),
    _results()
{
    if ( !_connection ) {
        LOG_WARN_MSG( "could not get database connection" );
        return;
    }

    _results = _connection->query(
            "SELECT " +
            BGQDB::DBTJob::ID_COL + "," + 
            BGQDB::DBTJob::BLOCKID_COL + "," + 
            BGQDB::DBTJob::STATUS_COL + ", " +
            BGQDB::DBTJob::CORNER_COL + ", " +
            BGQDB::DBTJob::SHAPEA_COL + ", " + 
            BGQDB::DBTJob::SHAPEB_COL + ", " +
            BGQDB::DBTJob::SHAPEC_COL + ", " +
            BGQDB::DBTJob::SHAPED_COL + ", " +
            BGQDB::DBTJob::SHAPEE_COL + ", " +
            BGQDB::DBTJob::NP_COL + ", " +
            BGQDB::DBTJob::PROCESSESPERNODE_COL + ", " +
            BGQDB::DBTJob::MAPPING_COL + ", " +
            BGQDB::DBTJob::CLIENT_COL + ", " +
            BGQDB::DBTJob::USERNAME_COL + " " +
            " FROM TBGQJob"
            );
}

void
Reconnect::nextJob()
{
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    if ( !_results->fetch() ) {
        LOG_INFO_MSG( "done reconnecting" );
        return;
    }
    const cxxdb::Columns& columns = _results->columns();

    if ( columns[BGQDB::DBTJob::STATUS_COL].getString() != BGQDB::job::status_code::Running ) {
        this->removeJob(
                error_code::job_failed_to_start,
                "job with status " + columns[BGQDB::DBTJob::STATUS_COL].getString() + " cannot be reconnected"
                );
        return;
    }
    
    if ( server->getOptions().reconnect().scope() != runjob::server::Reconnect::Scope::Jobs ) {
        this->removeJob(
                error_code::runjob_server_restart,
                "job reconnect is not configured"
                );
        return;
    }
        
    // found a running job that should be reconnected
    server->getBlocks()->find(
            columns[BGQDB::DBTJob::BLOCKID_COL].getString(),
            boost::bind(
                &Reconnect::findBlockCallback,
                shared_from_this(),
                _1
                )
            );
}

void
Reconnect::findBlockCallback(
        const block::Compute::Ptr& block
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );

    const cxxdb::Columns& columns = _results->columns();

    LOGGING_DECLARE_BLOCK_MDC( columns[BGQDB::DBTJob::BLOCKID_COL].getString() );
    LOGGING_DECLARE_JOB_MDC( columns[BGQDB::DBTJob::ID_COL].as<BGQDB::job::Id>() );

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    if ( !block ) {
        this->removeJob(
                error_code::block_not_found,
                "could not find block '" + columns[BGQDB::DBTJob::BLOCKID_COL].getString() + "' to reconnect job"
                );
        return;
    }

    boost::shared_ptr<job::Create> job;
    try {
        // creating JobInfo can throw in some scenarios
        job = boost::make_shared<job::Create>( this->getJobInfo() );
    } catch ( const std::exception& e ) {
        this->removeJob(
                error_code::runjob_server_restart,
                e.what()
                );
        return;
    }

    job->
        server( server )->
        compute_block( block )->
        reconnect( columns[BGQDB::DBTJob::ID_COL].as<BGQDB::job::Id>() )->
        client_id( columns[BGQDB::DBTJob::CLIENT_COL].getInt64() )->
        callback(
                boost::bind(
                    &Reconnect::arbitrateCallback,
                    shared_from_this(),
                    _1,
                    _2
                    )
                )
        ;

    // start arbitration on the compute block
    block->arbitrate( job );
}

JobInfo
Reconnect::getJobInfo()
{
    const cxxdb::Columns& columns = _results->columns();

    JobInfo info;

    info.setBlock( columns[BGQDB::DBTJob::BLOCKID_COL].getString() );

    if ( !columns[BGQDB::DBTJob::CORNER_COL].isNull() ) {
        info.setCorner( columns[BGQDB::DBTJob::CORNER_COL].getString() );
        info.setShape(
                boost::lexical_cast<std::string>( columns[BGQDB::DBTJob::SHAPEA_COL].getInt32() ) + "x" +
                boost::lexical_cast<std::string>( columns[BGQDB::DBTJob::SHAPEB_COL].getInt32() ) + "x" +
                boost::lexical_cast<std::string>( columns[BGQDB::DBTJob::SHAPEC_COL].getInt32() ) + "x" +
                boost::lexical_cast<std::string>( columns[BGQDB::DBTJob::SHAPED_COL].getInt32() ) + "x" +
                boost::lexical_cast<std::string>( columns[BGQDB::DBTJob::SHAPEE_COL].getInt32() )
                );
    }

    info.setNp( columns[BGQDB::DBTJob::NP_COL].getInt32() );

    info.setMapping(
            Mapping(
                Mapping::getType( columns[BGQDB::DBTJob::MAPPING_COL].getString() ),
                columns[BGQDB::DBTJob::MAPPING_COL].getString(),
                false // skip mapping file validation
                )
            );

    info.setRanksPerNode( columns[BGQDB::DBTJob::PROCESSESPERNODE_COL].getInt32() );

    const bool allowRemoteUser( true );
    info.setUserId( 
            bgq::utility::UserId::Ptr(
                new bgq::utility::UserId(
                    columns[BGQDB::DBTJob::USERNAME_COL].getString(),
                    allowRemoteUser
                    )
                )
            );

    return info;
}

void
Reconnect::removeJob(
        const error_code::rc error,
        const std::string& message
        )
{
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    const cxxdb::Columns& columns = _results->columns();
    LOGGING_DECLARE_BLOCK_MDC( columns[BGQDB::DBTJob::BLOCKID_COL].getString() );
    LOGGING_DECLARE_JOB_MDC( columns[BGQDB::DBTJob::ID_COL].as<BGQDB::job::Id>() );

    const database::Init::Ptr database = server->getDatabase();

    LOG_WARN_MSG( message );

    job::ExitStatus reason;
    reason.set( 
            message,
            error
            );

    database->getDelete().execute(
            _results->columns()[ BGQDB::DBTJob::ID_COL ].as<BGQDB::job::Id>(),
            reason,
            boost::bind(
                &Reconnect::nextJob,
                shared_from_this()
                )
            );
}

void
Reconnect::arbitrateCallback(
        const error_code::rc error,
        const Job::Ptr& job
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );

    const cxxdb::Columns& columns = _results->columns();

    LOGGING_DECLARE_BLOCK_MDC( columns[BGQDB::DBTJob::BLOCKID_COL].getString() );
    LOGGING_DECLARE_JOB_MDC( columns[BGQDB::DBTJob::ID_COL].as<BGQDB::job::Id>() );

    if ( error ) {
        this->removeJob(
                error_code::runjob_server_restart,
                std::string("could not arbitrate: ") + toString(error)
                );
        return;
    }

    job->status().set(
            Status::Running,
            job
            );

    this->nextJob();
}

} // job
} // server
} // runjob
