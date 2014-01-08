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
#include "server/job/tool/ToolStatus.h"

#include "common/commands/ToolStatus.h"

#include "common/logging.h"

#include "server/cios/Message.h"

#include "server/CommandConnection.h"
#include "server/Job.h"

#include <db/include/api/tableapi/gensrc/DBTJobtool.h>

#include <db/include/api/cxxdb/cxxdb.h>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {
namespace tool {

void
ToolStatus::create(
        const Job::Ptr& job,
        const runjob::commands::request::ToolStatus::Ptr& request,
        const CommandConnection::Ptr& connection
        )
{
    const Ptr result(
            new ToolStatus( job, request, connection )
            );

    job->strand().post(
            boost::bind(
                &ToolStatus::impl,
                result
                )
            );
}

ToolStatus::ToolStatus(
        const Job::Ptr& job,
        const runjob::commands::request::ToolStatus::Ptr& request,
        const CommandConnection::Ptr& connection
        ) :
    _job( job ),
    _request( request ),
    _response( new runjob::commands::response::ToolStatus() ),
    _connection( connection ),
    _status( runjob::commands::error::unknown_failure ),
    _message()
{

}

ToolStatus::~ToolStatus()
{
    try {
        if ( _status ) {
            LOG_INFO_MSG( _message.str() );
        }
        _response->setError( _status );
        _response->setMessage( _message.str() );
        _connection->write( _response );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    } catch ( ... ) {
        LOG_WARN_MSG( "caught some other exception" );
    }
}

void
ToolStatus::impl()
{
    LOGGING_DECLARE_JOB_MDC( _job->id() );
    LOGGING_DECLARE_BLOCK_MDC( _job->info().getBlock() );
    LOG_TRACE_MSG( __FUNCTION__ );

    try {
        const cxxdb::ConnectionPtr db(
                BGQDB::DBConnectionPool::Instance().getConnection()
                );
        if ( !db ) {
            _status = runjob::commands::error::database_error;
            _message << "could not get database connection";
            return;
        }

        std::ostringstream query;
        query <<
            "SELECT " <<
            BGQDB::DBTJobtool::ID_COL << "," <<
            BGQDB::DBTJobtool::STATUS_COL << "," <<
            BGQDB::DBTJobtool::EXECUTABLE_COL << ", " <<
            BGQDB::DBTJobtool::ENTRYDATE_COL << ", " <<
            BGQDB::DBTJobtool::ERRTEXT_COL << ", " <<
            BGQDB::DBTJobtool::SUBSET_COL << " " <<
            "FROM " << BGQDB::DBTJobtool().getTableName() << " " <<
            "WHERE " << BGQDB::DBTJobtool::JOB_COL << "=" + boost::lexical_cast<std::string>( _job->id() )
            ;

        if ( _request->_tool ) {
            query << " AND " << BGQDB::DBTJobtool::ID_COL << "=" << _request->_tool;
        }

        const cxxdb::ResultSetPtr results(
                db->query( query.str() )
                );

        while ( results->fetch() ) {
            const cxxdb::Columns& columns = results->columns();
            runjob::commands::response::ToolStatus::Tool tool;
            tool._id = columns[ BGQDB::DBTJobtool::ID_COL ].getInt32();
            tool._path = columns[ BGQDB::DBTJobtool::EXECUTABLE_COL ].getString();
            tool._timestamp = columns[ BGQDB::DBTJobtool::ENTRYDATE_COL ].getTimestamp();
            tool._subset = columns[ BGQDB::DBTJobtool::SUBSET_COL ].getString();
            if ( !columns[ BGQDB::DBTJobtool::ERRTEXT_COL].isNull() ) {
                tool._error = columns[ BGQDB::DBTJobtool::ERRTEXT_COL ].getString();
            }

            // convert status column from single char to something that is easier to understand
            Status status;
            std::istringstream is( columns[ BGQDB::DBTJobtool::STATUS_COL ].getString() );
            is >> status;
            if ( is ) {
                tool._status = boost::lexical_cast<std::string>( status );
            } else {
                tool._status = columns[ BGQDB::DBTJobtool::STATUS_COL ].getString();
            }

            _response->_tools.push_back( tool );
        }

        if ( _response->_tools.empty() && _request->_tool ) {
            _status = runjob::commands::error::tool_not_found;
        } else {
            _status = runjob::commands::error::success;
        }
    } catch ( const std::exception& e ) {
        _status = runjob::commands::error::database_error;
        _message << e.what();
    }
}

} // tool
} // job
} // server 
} // runjob

