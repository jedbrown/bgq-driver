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


/*! \page diagnosticsRunsResource /bg/diagnostics/runs

This resource provides operations on diagnostics runs,
getting a summary of diagnostics runs
and starting a new diagnostics run.

- \subpage diagnosticsRunsResourceGet
- \subpage diagnosticsRunsResourcePost

 */


/*! \page diagnosticsRunsResourceGet GET /bg/diagnostics/runs

Get a summary of diagnostics runs.

\section Authority

The user must have hardware READ authority.

\section diagnosticsRunsResourceGetParameters Query parameters

- blocks: Comma-separated list of block IDs, runs with any of the blocks will be returned.
- end: a \ref timeIntervalFormat "time interval", nullable
- sort: Sort column,
  - Format is [&lt;direction&gt;]&lt;field&gt;.
  - by default, sorts by runId descending.
  - direction is + for ascending or - for descending, the default is ascending.
  - field is one of:
    - runId
    - end
    - start
    - status
- start: a \ref timeIntervalFormat "time interval"
- status: Run status
  - The argument is a string containing status codes. The response will
  - The default is to return all diagnostics runs (i.e., any status code).
  - Status codes:
    - R : running
    - C : completed
    - F : failed
    - L : canceled


\section diagnosticsRunsResourceGetResponse JSON response format

<pre>
[
  {
    "runId" : &quot;<i>string</i>&quot;,
    "status" : &quot;<i>status</i>&quot;,
    "start" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
    "end" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;, // optional
    "logDirectory" : &quot;<i>string</i>&quot;,
    "cancelable" : &quot;<i>cancelable</i>&quot;, // optional, default is not cancelable
    "user" : &quot;<i>string</i>&quot;, // optional
    "blocks" : [
        {
          "blockId" : &quot;<i>string</i>&quot, // optional -- one of blockId or location will be present.
          "location" : &quot;<i>string</i>&quot, // optional -- one of blockId or location will be present.
          "testsAnalyzed" : <i>number</i>, // optional -- only present if run end is not present.
          "testsToAnalyze" : <i>number</i>, // optional -- only present if run end is not present.
          "testsFailed" : <i>number</i> // optional -- only present if run end is not present.
        },
        ... // one for each block in the run.
      ]
  },
  ...
]
</pre>

- status is one of running, completed, failed, canceled
- cancelable is one of cancelable, canceled

\section Errors

HTTP status: 403 Forbidden
- authority: The user doesn't have READ hardware authority.

 */

/*! \page diagnosticsRunsResourcePost POST /bg/diagnostics/runs

Start a new diagnostics run.

\section Authority

The user must be an administrator to start a new diagnostics run.

\section diagnosticsRunsResourcePostInput JSON request data format

<pre>
{
  "midplanes" : [ &quot;<i>midplane-location</i>&quot;, ... ], // Optional
  "io" : [ &quot;<i>io-drawer-location</i>&quot;, ... ], // Optional
  "blocks" : [ &quot;<i>block-id</i>&quot;, ... ], // Optional
  "tests" : [ &quot;<i>test-name</i>&quot;, ... ], // Optional
  "buckets" : [ &quot;<i>bucket-name</i>&quot;, ... ], // Optional
  "stopOnError" : <i>Boolean</i>, // Optional
  "midplanesPerRow" : <i>integer</i>, // Optional
  "midplanesPerProc" : <i>integer</i>, // Optional
  "saveAllOutput" : <i>Boolean</i>, // Optional
  "deleteBlock" : <i>Boolean</i>, // Optional
  "killJobs" : <i>Boolean</i>, // Optional
  "disruptMasterClock" : <i>Boolean</i>, // Optional
  "disruptIoRack" : <i>Boolean</i>, // Optional
  "insertRas" : <i>Boolean</i> // Optional
}
</pre>


\section Response

HTTP status: 201 Created
- Location header is the URL of the new diagnostics run resource.

\section Errors

HTTP status: 400 Bad Requst
- failedToStart: Diagnostics failed to start. The message in the error document contains the startup error.
- notAnObject: The input JSON document was not an object.

HTTP status: 403 Forbidden
- authority: The user doesn't have authority to start a diagnostics run.

HTTP status: 500 Internal Server Error
- noRunIdReturned: The diagnostics program didn't provide the run ID.

*/


#include "Runs.hpp"

#include "common.hpp"
#include "Run.hpp"
#include "RunsQueryOptions.hpp"

#include "../../BlockingOperationsThreadPool.hpp"
#include "../../blue_gene.hpp"
#include "../../dbConnectionPool.hpp"
#include "../../Error.hpp"
#include "../../ras.hpp"
#include "../../WhereClause.hpp"

#include "../../blue_gene/diagnostics/Runs.hpp"
#include "../../blue_gene/diagnostics/RunOptions.hpp"

#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <string>


using boost::lexical_cast;

using std::set;
using std::string;
using std::vector;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {
namespace diagnostics {


const capena::http::uri::Path Runs::RESOURCE_PATH(common::getBaseUrlPath() / "runs");
const capena::http::uri::Path Runs::RESOURCE_PATH_EMPTY_CHILD(RESOURCE_PATH / "");


bool Runs::matchesUrl(
        const capena::http::uri::Path& requested_resource
    )
{
    return (requested_resource == RESOURCE_PATH || requested_resource == RESOURCE_PATH_EMPTY_CHILD);
}


void Runs::_doGet()
{
    if ( ! _userHasHardwareRead() ) {
        LOG_WARN_MSG( "Could not get diagnostics runs because " << _getRequestUserInfo() << " doesn't have authority." );

        BOOST_THROW_EXCEPTION( Error(
                "Could not get diagnostics runs because the user doesn't have authority.",
                "getDiagnosticsRuns", "authority", Error::Data(),
                capena::http::Status::Forbidden
            ) );

        return;
    }


    RunsQueryOptions query_options( _getRequest().getUrl().getQuery().calcArguments() );

    // If response includes running diagnostics, get a snapshot of the current runs in progress that the BGWS server started.

    if ( query_options.includesRunning() ) {

        LOG_DEBUG_MSG( "Query includes running, getting snapshot." );

        _diagnostics_runs.getSnapshot(
                boost::bind(
                        &Runs::_gotSnapshot, this,
                        capena::server::AbstractResponder::shared_from_this(),
                        _1,
                        query_options
                    )
            );

    } else { // Did not request running diagnostics.

        LOG_DEBUG_MSG( "Query does not include running, skipping snapshot." );

        _blocking_operations_thread_pool.post( boost::bind(
                &Runs::_startQuery, this,
                capena::server::AbstractResponder::shared_from_this(),
                blue_gene::diagnostics::Runs::SnapshotPtr(),
                query_options
            ) );

    }

}


void Runs::_doPost( json::ConstValuePtr val_ptr )
{
    if ( ! _isUserAdministrator() ) {
        LOG_WARN_MSG( "Could not start diagnostics run because " << _getRequestUserInfo() << " doesn't have authority." );

        ras::postAdminAuthorityFailure( _getRequestUserInfo(), "start diagnostics run" );

        BOOST_THROW_EXCEPTION( Error(
                "Could not start diagnostics run because the user doesn't have authority.",
                "startDiagnosticsRun", "authority", Error::Data(),
                capena::http::Status::Forbidden
            ) );

        return;
    }


    if ( ! val_ptr->isObject() ) {
        BOOST_THROW_EXCEPTION( Error(
                "Could not start diagnostics run because the data provided is not an object.",
                "startDiagnosticsRun", "notAnObject", Error::Data(),
                capena::http::Status::BadRequest
            ) );
    }


    const json::Object &obj(val_ptr->getObject());

    blue_gene::diagnostics::RunOptions::Args run_args;

    if ( obj.contains( "midplanes" ) && obj.isArray( "midplanes" ) ) {
        vector<string> midplanes;

        BOOST_FOREACH( const json::Array::value_type &elem, obj.getArray( "midplanes" ) ) {
            midplanes.push_back( elem->getString() );
        }

        run_args.push_back( "--midplanes" );
        run_args.push_back( boost::algorithm::join( midplanes, "," ) );
    }

    if ( obj.contains( "io" ) && obj.isArray( "io" ) ) {
        vector<string> ios;

        BOOST_FOREACH( const json::Array::value_type &elem, obj.getArray( "io" ) ) {
            ios.push_back( elem->getString() );
        }

        run_args.push_back( "--io" );
        run_args.push_back( boost::algorithm::join( ios, "," ) );

    }

    if ( obj.contains( "blocks" ) && obj.isArray( "blocks" ) ) {
        vector<string> blocks;

        BOOST_FOREACH( const json::Array::value_type &elem, obj.getArray( "blocks" ) ) {
            blocks.push_back( elem->getString() );
        }

        run_args.push_back( "--blocks" );
        run_args.push_back( boost::algorithm::join( blocks, "," ) );
    }


    if ( obj.contains( "tests" ) && obj.isArray( "tests" ) ) {
        vector<string> tests;

        BOOST_FOREACH( const json::Array::value_type &elem, obj.getArray( "tests" ) ) {
            tests.push_back( elem->getString() );
        }

        run_args.push_back( "--tests" );
        run_args.push_back( boost::algorithm::join( tests, "," ) );
    }

    if ( obj.contains( "buckets" ) && obj.isArray( "buckets" ) ) {
        vector<string> buckets;

        BOOST_FOREACH( const json::Array::value_type &elem, obj.getArray( "buckets" ) ) {
            buckets.push_back( elem->getString() );
        }

        run_args.push_back( "--buckets" );
        run_args.push_back( boost::algorithm::join( buckets, "," ) );
    }

    if ( obj.contains( "stopOnError" ) && obj.isBool( "stopOnError" ) && obj.getBool( "stopOnError" ) ) {
        run_args.push_back( "--stoponerror" );
    }

    if ( obj.contains( "midplanesPerRow" ) && obj.is<int64_t>( "midplanesPerRow") ) {
        run_args.push_back( "--midplanesperrow" );
        run_args.push_back( lexical_cast<string>( obj.as<int64_t>( "midplanesPerRow" ) ) );
    }

    if ( obj.contains( "midplanesPerProc" ) && obj.is<int64_t>( "midplanesPerProc") ) {
        run_args.push_back( "--midplanesperproc" );
        run_args.push_back( lexical_cast<string>( obj.as<int64_t>( "midplanesPerProc" ) ) );
    }

    if ( obj.contains( "saveAllOutput" ) && obj.isBool( "saveAllOutput" ) && obj.getBool( "saveAllOutput" ) ) {
        run_args.push_back( "--savealloutput" );
    }

    if ( obj.contains( "deleteBlock" ) && obj.isBool( "deleteBlock" ) && obj.getBool( "deleteBlock" ) ) {
        run_args.push_back( "--deleteblock" );
    }

    if ( obj.contains( "killJobs" ) && obj.isBool( "killJobs" ) && obj.getBool( "killJobs" ) ) {
        run_args.push_back( "--killjobs" );
    }

    if ( obj.contains( "disruptMasterClock" ) && obj.isBool( "disruptMasterClock" ) && obj.getBool( "disruptMasterClock" ) ) {
        run_args.push_back( "--disruptmasterclock" );
    }

    if ( obj.contains( "disruptIoRack" ) && obj.isBool( "disruptIoRack" ) && obj.getBool( "disruptIoRack" ) ) {
        run_args.push_back( "--disruptiorack" );
    }

    if ( obj.contains( "insertRas") && obj.isBool( "insertRas" ) && obj.getBool( "insertRas" ) ) {
        run_args.push_back( "--insertras" );
    }

    run_args.push_back( "--properties" );
    run_args.push_back( _getDynamicConfiguration().getPropertiesFilename() );


    LOG_INFO_MSG( "New diagnostics run requested by " << _getRequestUserInfo() << " args: " << boost::algorithm::join( run_args, " " ) );

    blue_gene::diagnostics::RunOptions run_options;
    run_options.setUserName( getRequestUserName() );
    run_options.setArgs( run_args );

    _diagnostics_runs.startNewRun(
            run_options,
            _getStrand().wrap(
                    boost::bind(
                            &Runs::_handleGotNewDiagnosticsRunId,
                            this,
                            capena::server::AbstractResponder::shared_from_this(),
                            _1,
                            _2
                        )
                )
        );
}


void Runs::_gotSnapshot(
        capena::server::ResponderPtr /*responder_ref_ptr*/,
        blue_gene::diagnostics::Runs::SnapshotPtr snapshot_ptr,
        const RunsQueryOptions& query_options
    )
{
    try { // This is a callback function not in the Responder's strand, so needs to wrap in try/catch to call _inCatchPostCurrentExceptionToHandlerFn if error.

        _blocking_operations_thread_pool.post( boost::bind(
                &Runs::_startQuery, this,
                capena::server::AbstractResponder::shared_from_this(),
                snapshot_ptr,
                query_options
            ) );

    } catch ( std::exception& e ) {

        _inCatchPostCurrentExceptionToHandlerFn();

    }
}


void Runs::_startQuery(
        capena::server::ResponderPtr,
        blue_gene::diagnostics::Runs::SnapshotPtr snapshot_ptr,
        const RunsQueryOptions& query_options
    )
{
    try {
        WhereClause where_clause;
        cxxdb::ParameterNames param_names;
        string sort_clause_sql;

        query_options.addToWhereClause(
                &where_clause,
                &param_names,
                &sort_clause_sql
            );


        string runs_sql = string() +

"SELECT " + BGQDB::DBTDiagruns::RUNID_COL + ", " + BGQDB::DBTDiagruns::STARTTIME_COL + ", " + BGQDB::DBTDiagruns::ENDTIME_COL + ", " + BGQDB::DBTDiagruns::DIAGSTATUS_COL + ", " + BGQDB::DBTDiagruns::LOGDIR_COL +
 " FROM " + BGQDB::DBTDiagruns().getTableName() +
   where_clause.getString() +
 " ORDER BY " + sort_clause_sql

           ;

        auto conn_ptr(dbConnectionPool::getConnection());

        LOG_DEBUG_MSG( "preparing: " << runs_sql );

        cxxdb::QueryStatementPtr runs_stmt_ptr(conn_ptr->prepareQuery( runs_sql, param_names ));

        query_options.bindParameters( *runs_stmt_ptr );

        cxxdb::ResultSetPtr runs_rs_ptr(runs_stmt_ptr->execute());


        string blocks_sql =

"SELECT bs.blockId,"
      " COALESCE( tr.testsAnalyzed, 0 ) AS testsAnalyzed,"
      " COALESCE( tr.testsToAnalyze, 0 ) AS testsToAnalyze,"
      " COALESCE( tr.testsFailed, 0 ) AS testsFailed"
 " FROM bgqDiagBlocks as bs"
      " LEFT OUTER JOIN"
      " ( SELECT blockId,"
               " COUNT(*) AS testsToAnalyze,"
               " SUM( CASE WHEN passed <> 0 OR marginal <> 0 OR unknown <> 0 OR failed <> 0 THEN 1 ELSE 0 END ) AS testsAnalyzed,"
               " SUM( CASE WHEN failed <> 0 THEN 1 ELSE 0 END ) AS testsFailed"
          " FROM bgqDiagTests"
          " WHERE runId = ?" // runId
          " GROUP BY blockID"
      " ) AS tr"
      " ON bs.blockId = tr.blockId"
 " WHERE bs.runId = ?"; // runId

            ;


        cxxdb::QueryStatementPtr blocks_stmt_ptr(conn_ptr->prepareQuery( blocks_sql, boost::assign::list_of( "runId" )( "runId" ) ));


        json::ArrayValuePtr arr_val_ptr(json::Array::create());

        json::Array &arr(arr_val_ptr->get());

        while ( runs_rs_ptr->fetch() )
        {
            json::Object &obj(arr.addObject());

            const cxxdb::Columns &run_cols(runs_rs_ptr->columns());

            int64_t run_id = run_cols["runId"].as<int64_t>();

            obj.set( "runId", lexical_cast<string>( run_id ) );
            obj.set( "start", run_cols["startTime"].getTimestamp() );
            if ( run_cols["endTime"] )   obj.set( "end", run_cols["endTime"].getTimestamp() );

            string diag_status( "unknown" );

            if ( run_cols["diagStatus"] ) {
                diag_status = run_cols["diagStatus"].getString();
                obj.set( "status", diag_status );
            }

            obj.set( "logDirectory", run_cols["logdir"].getString() );

            bool is_running(! run_cols["endTime"]);


            json::Array &blocks_arr(obj.createArray( "blocks" ));

            blocks_stmt_ptr->parameters()["runId"].cast( run_id );

            cxxdb::ResultSetPtr blocks_rs_ptr(blocks_stmt_ptr->execute());

            while ( blocks_rs_ptr->fetch() ) {
                const cxxdb::Columns &block_cols(blocks_rs_ptr->columns());

                json::Object &block_obj(blocks_arr.addObject());

                const string block_id(block_cols["blockId"].getString());
                if ( boost::algorithm::starts_with( block_id, blue_gene::diagnostics::HARDWARE_BLOCK_PREFIX ) ) {
                    block_obj.set( "location", block_id.substr( blue_gene::diagnostics::HARDWARE_BLOCK_PREFIX.size() ) );
                } else {
                    block_obj.set( "blockId", block_id );
                }

                if ( is_running ) {
                    block_obj.set( "testsAnalyzed", block_cols["testsAnalyzed"].as<int64_t>() );
                    block_obj.set( "testsToAnalyze", block_cols["testsToAnalyze"].as<int64_t>() );
                    block_obj.set( "testsFailed", block_cols["testsFailed"].as<int64_t>() );
                }
            }


            if ( snapshot_ptr ) {
                blue_gene::diagnostics::Runs::Snapshot::const_iterator running_diags_i(snapshot_ptr->find( run_id ));

                if ( running_diags_i != snapshot_ptr->end() ) {
                    obj.set( "user", running_diags_i->second.user );
                    obj.set( "cancelable", running_diags_i->second.canceled ? "canceled" : "cancelable" );
                }
            }
        }

        _getStrand().post( boost::bind(
                &Runs::_queryComplete, this,
                capena::server::AbstractResponder::shared_from_this(),
                arr_val_ptr
            ) );

    } catch ( std::exception& e ) {

        _inCatchPostCurrentExceptionToHandlerFn();

    }
}


void Runs::_queryComplete(
        capena::server::ResponderPtr,
        json::ArrayValuePtr arr_val_ptr
    )
{
    try {
        capena::server::Response &response(_getResponse());

        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( *arr_val_ptr, response.out() );

    } catch ( std::exception& e ) {
        _handleError( e );
    }
}


void Runs::_handleGotNewDiagnosticsRunId(
        capena::server::ResponderPtr /*shared_ptr*/,
        blue_gene::diagnostics::RunId run_id,
        const std::string& error_msg
    )
{
    try {

        if ( run_id == blue_gene::diagnostics::RunId(-1) ) {
            // Failed to get a run ID, send back error response.

            if ( error_msg == string() ) {

                LOG_WARN_MSG( "Diagnostics failed to start." );

                BOOST_THROW_EXCEPTION( Error(
                        error_msg,
                        "startDiagnosticsRun",
                        "noRunIdReturned",
                        Error::Data(),
                        capena::http::Status::InternalServerError
                    ) );

            }

            LOG_WARN_MSG( "Diagnostics failed to start, error is '" << error_msg << "'" );

            Error::Data error_data;
            error_data["msg"] = error_msg;

            BOOST_THROW_EXCEPTION( Error(
                    error_msg,
                    "startDiagnosticsRun",
                    "failedToStart",
                    error_data,
                    capena::http::Status::BadRequest
                ) );

            return;
        }

        // Got a run ID, respond with Created with the location of the new diagnostics run.
        _getResponse().setCreated( Run::calcUri( _getDynamicConfiguration().getPathBase(), run_id ) );

    } catch ( std::exception& e ) {

        _handleError( e );

    }
}


} } } // namespace bgws::responder::diagnostics
