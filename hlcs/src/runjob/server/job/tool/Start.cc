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
#include "server/job/tool/Start.h"

#include "common/logging.h"

#include "server/CommandConnection.h"
#include "server/Job.h"

#include <db/include/api/tableapi/gensrc/DBTJobtool.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <db/include/api/job/InsertInfo.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <hlcs/include/runjob/commands/error.h>

#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {
namespace tool {

const unsigned Start::MaximumToolsPerJob = 4;

void
Start::create(
        const Job::Ptr& job,
        const runjob::tool::Daemon& tool,
        runjob::tool::Proctable& proctable,
        const Tool::Callback& callback
        )
{
    const Ptr result(
            new Start( job, tool, proctable, callback )
            );

    try {
        const cxxdb::ConnectionPtr db(
                BGQDB::DBConnectionPool::Instance().getConnection()
                );
        if ( !db ) {
            result->_status = runjob::commands::error::database_error;
            result->_message << "could not get database connection";
            return;
        }

        const boost::shared_ptr<cxxdb::Transaction> transaction(
                new cxxdb::Transaction( *db )
                );
        result->query( job, db, transaction );
    } catch ( const std::exception& e ) {
        result->_status = runjob::commands::error::database_error;
        result->_message.str( e.what() );
    }
}

Start::Start(
        const Job::Ptr& job,
        const runjob::tool::Daemon& tool,
        runjob::tool::Proctable& proctable,
        const Tool::Callback& callback
        ) :
    _job( job ),
    _tool( tool ),
    _proctable( proctable ),
    _id( 0 ),
    _status( runjob::commands::error::unknown_failure ),
    _message(),
    _callback( callback )
{

}

Start::~Start()
{
    LOG_TRACE_MSG( __FUNCTION__ );

    // only invoke callback if we were not successful starting the tool
    if ( _status ) {
        _callback( _id, _status, _message.str() );
    }
}

void
Start::query(
        const Job::Ptr& job,
        const cxxdb::ConnectionPtr& connection,
        const boost::shared_ptr<cxxdb::Transaction>& transaction
        )
{
    // count the number of tools running or starting, we can only have 4 running concurrently
    const cxxdb::ResultSetPtr results(
            connection->query(
                std::string("SELECT COUNT(*) as count ") +
                "FROM " + BGQDB::DBTJobtool().getTableName() + " " +
                "WHERE " + BGQDB::DBTJobtool::JOB_COL + "=" + boost::lexical_cast<std::string>( job->id() ) + " " +
                "AND " + BGQDB::DBTJobtool::STATUS_COL + " in ('R','S')"
                )
            );
    if ( results->fetch() ) {
        const cxxdb::Columns& columns = results->columns();
        const unsigned count =  columns[ "count" ].getInt32();
        LOG_TRACE_MSG( count << " tools starting or running" );
        if ( count >= MaximumToolsPerJob ) {
            _status = runjob::commands::error::maximum_tool_count;
            _message << "job " << job->id() << " already has " << count << " tools starting or running.";
            return;
        }
    }

    this->calculateId( job, connection, transaction );
}

void
Start::calculateId(
        const Job::Ptr& job,
        const cxxdb::ConnectionPtr& connection,
        const boost::shared_ptr<cxxdb::Transaction>& transaction
        )
{
    const cxxdb::QueryStatementPtr query(
            connection->prepareQuery(
                "SELECT max(" + BGQDB::DBTJobtool::ID_COL + ") as " + BGQDB::DBTJobtool::ID_COL + " " +
                "FROM " + BGQDB::DBTJobtool().getTableName() + " " +
                "WHERE " + BGQDB::DBTJobtool::JOB_COL + "=" + boost::lexical_cast<std::string>( job->id() )
                )
            );
    const cxxdb::ResultSetPtr results = query->execute();
    if ( results->fetch() ) {
        const cxxdb::Columns& columns = results->columns();
        if ( columns[ BGQDB::DBTJobtool::ID_COL ].isNull() ) {
            LOG_TRACE_MSG( "no tools found" );
            _id = 1;
        } else {
            _id = columns[ BGQDB::DBTJobtool::ID_COL ].getInt32() + 1;
        }
    } else {
        LOG_TRACE_MSG( "no tools found" );
    }

    LOG_TRACE_MSG( "using tool ID " << _id );

    this->insert( job, connection, transaction );
}

void
Start::insert(
        const Job::Ptr& job,
        const cxxdb::ConnectionPtr& connection,
        const boost::shared_ptr<cxxdb::Transaction>& transaction
        )
{
    const cxxdb::UpdateStatementPtr update(
            connection->prepareUpdate(
                std::string("INSERT into ") + BGQDB::DBTJobtool().getTableName() + " (" +
                BGQDB::DBTJobtool::ID_COL + "," +
                BGQDB::DBTJobtool::JOB_COL + "," +
                BGQDB::DBTJobtool::STATUS_COL + "," +
                BGQDB::DBTJobtool::EXECUTABLE_COL + "," +
                BGQDB::DBTJobtool::ARGS_COL + ", " +
                BGQDB::DBTJobtool::SUBSET_COL + " " +
                ") VALUES (?,?,?,?,?,?)",
                boost::assign::list_of
                ( BGQDB::DBTJobtool::ID_COL )
                ( BGQDB::DBTJobtool::JOB_COL )
                ( BGQDB::DBTJobtool::STATUS_COL )
                ( BGQDB::DBTJobtool::EXECUTABLE_COL )
                ( BGQDB::DBTJobtool::ARGS_COL )
                ( BGQDB::DBTJobtool::SUBSET_COL )
                )
            );
    update->parameters()[ BGQDB::DBTJobtool::ID_COL ].cast( _id );
    update->parameters()[ BGQDB::DBTJobtool::JOB_COL ].cast( job->id() );
    update->parameters()[ BGQDB::DBTJobtool::STATUS_COL ].set( "S" );
    update->parameters()[ BGQDB::DBTJobtool::EXECUTABLE_COL ].set( _tool.getExecutable() );
    update->parameters()[ BGQDB::DBTJobtool::ARGS_COL ].set( BGQDB::job::InsertInfo::format(_tool.getArguments()) );
    update->parameters()[ BGQDB::DBTJobtool::SUBSET_COL ].set( boost::lexical_cast<std::string>(_tool.getSubset()) );

    update->execute();
    LOG_DEBUG_MSG( "inserted tool " << _id << ": " << _tool.getExecutable() );

    job->strand().post(
            boost::bind(
                &Start::validate,
                shared_from_this(),
                connection,
                transaction
                )
            );
}

void
Start::validate(
        const cxxdb::ConnectionPtr& connection,
        const boost::shared_ptr<cxxdb::Transaction>& transaction
        )
{
    const Job::Ptr job( _job.lock() );
    if ( !job ) return;

    LOGGING_DECLARE_JOB_MDC( job->id() );
    LOGGING_DECLARE_BLOCK_MDC( job->info().getBlock() );
    LOG_TRACE_MSG( "start" );

    if ( 
            job->status().get() != job::Status::Running &&
            job->status().get() != job::Status::Debug
       )
    {
        _status = runjob::commands::error::job_status_invalid;
        _message << "cannot start a tool when job status is " << std::string( job->status() );
        return;
    }

    if ( job->info().getSubBlock().corner().isCoreSet() ) {
        _status = runjob::commands::error::job_status_invalid;
        _message << "tool launching is not supported for sub-node jobs";
        return;
    }

    if ( _tool.getExecutable().empty() ) {
        _status = runjob::commands::error::tool_path_invalid;
        _message << "missing tool path in request";
        return;
    }

    (void)transaction;
    connection->commit();

    this->impl();
}

void
Start::impl()
{
    const Job::Ptr job( _job.lock() );
    if ( !job ) return;

    // remember tool started
    job->tools().add( job, _tool, _proctable, _id, _callback );

    _status = runjob::commands::error::success;
}

} // tool
} // job
} // server 
} // runjob

