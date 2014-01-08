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

/*! \page diagnosticsRunResource /bg/diagnostics/runs/<i>runId</i>

This resource provides operations on a Blue Gene diagnostics run, getting details and cancelling.

- \subpage diagnosticsRunResourceGet
- \subpage diagnosticsRunResourcePost

 */

/*! \page diagnosticsRunResourceGet GET /bg/diagnostics/run/<i>runId</i>

Get details for a diagnostics run.

\section Authority

The user must have hardware READ authority.

\section diagnosticsRunResourceGetResponse JSON response format

<pre>
{
    "status" : &quot;<i>status</i>&quot;,
    "start" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
    "end" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;, // optional
    "logDirectory" : &quot;<i>string</i>&quot;,
    "user" : &quot;<i>string</i>&quot; // optional -- only present if running under BGWS server.
    "cancelable" : &quot;<i>cancelable</i>&quot;, // optional, default is not cancelable
    "blocks" : [
        {
          "blockId" : &quot;<i>string</i>&quot;,
          "location" : &quot;<i>string</i>&quot;, // optional
          "start" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;, // optional
          "end" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;, // optional
          "hardwareStatus" : &quot;<i>hardwareStatus</i>&quot;,
          "testsPassed" : <i>number</i>,
          "testsMarginal" : <i>number</i>,
          "testsUnknown" : <i>number</i>,
          "testsFailed" : <i>number</i>,
          "testsToAnalyze" : <i>number</i> // optional -- only present if end is not present (is running).
        },
        ...
      ]
}
</pre>

- status is one of running, completed, failed, canceled
- cancelable is one of cancelable, canceled

\section Errors

HTTP status: 403 Forbidden
- authority: The user doesn't have READ hardware authority.

 */

/*! \page diagnosticsRunResourcePost POST /bg/diagnostics/run/<i>runId</i>

Perform an operation on a diagnostics run.
The only operation is cancel, which signals a diagnostics running under BGWS server to abort.

\section Authority

The user must be an administrator to cancel a diagnostics run.

\section diagnosticsRunResourcePostInput JSON request data format

<pre>
{
  "operation" : "cancel"
}
</pre>


\section Response

HTTP status: 204 No Content


\section Errors

HTTP status: 400 Bad Request
- noOperation: Invalid request, no operation provided.
- operationNotString: Invalid request, operation is not a string.
- unexpectedOperation: The operation is not valid. It must be cancel.

HTTP status: 403 Forbidden
- authority: The user doesn't have authority to cancel the run.

HTTP status: 404 Not Found
- invalidRunId: The run ID is not valid. Run IDs are numbers.
- notFound: There is no current run with the run ID.

 */


#include "Run.hpp"

#include "Runs.hpp"

#include "../../blue_gene.hpp"
#include "../../dbConnectionPool.hpp"
#include "../../Error.hpp"
#include "../../ras.hpp"
#include "../../types.hpp"

#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <string>


using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {
namespace diagnostics {


bool Run::matchesUrl( const capena::http::uri::Path& requested_resource )
{
    return ((requested_resource.calcParent() == Runs::RESOURCE_PATH) && (! requested_resource.back().empty()) );
}


capena::http::uri::Path Run::calcUri(
        const capena::http::uri::Path& path_base,
        blue_gene::diagnostics::RunId run_id
    )
{
    return (path_base / Runs::RESOURCE_PATH / lexical_cast<string>(run_id));
}


void Run::_doGet()
{
    if ( ! _userHasHardwareRead() ) {
        LOG_WARN_MSG( "Could not get diagnostics run because " << _getRequestUserInfo() << " doesn't have authority." );

        BOOST_THROW_EXCEPTION( Error(
                "Could not get diagnostics run because the user doesn't have authority.",
                "getDiagnosticsRun", "authority", Error::Data(),
                capena::http::Status::Forbidden
            ) );

        return;
    }

    const string &run_id_str(_getRequestedResourcePath().back());

    Error::Data error_data;
    error_data["runId"] = run_id_str;


    blue_gene::diagnostics::RunId run_id;

    try {

        run_id = lexical_cast<blue_gene::diagnostics::RunId>( run_id_str );

    } catch ( std::exception& e ) {

        BOOST_THROW_EXCEPTION( Error(
                string() + "Invalid run ID '" + run_id_str + "'",
                "getDiagnosticsRunDetails",
                "invalidRunId",
                error_data,
                capena::http::Status::NotFound
            ) );
    }


    auto conn_ptr(dbConnectionPool::getConnection());

    json::ObjectValuePtr obj_val_ptr( new json::ObjectValue() );
    json::Object &obj(obj_val_ptr->getObject());

    bool is_running(false);

    {
        string sql =

 "SELECT startTime, endTime, diagStatus, logDir"
  " FROM bgqDiagRuns"
  " WHERE runId = ?"

            ;

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( sql, cxxdb::ParameterNames{ "runId" } ));

        stmt_ptr->parameters()["runId"].cast( run_id );

        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        if ( ! rs_ptr->fetch() ) {

            BOOST_THROW_EXCEPTION( Error(
                    string() + "No diagnostics run with run ID " + run_id_str + " found",
                    "getDiagnosticsRunDetails",
                    "notFound",
                    error_data,
                    capena::http::Status::NotFound
                ) );

        }

        const cxxdb::Columns &cols(rs_ptr->columns());

        is_running = (cols["endTime"]);

        obj.set( "status", cols["diagStatus"].getString() );
        obj.set( "start", cols["startTime"].getTimestamp() );
        if ( is_running )  obj.set( "end", cols["endTime"].getTimestamp() );
        obj.set( "logDirectory", cols["logDir"].getString() );
    }

    json::Array &blocks_arr(obj.createArray( "blocks" ));

    {
        string sql =

 "SELECT db.blockId, db.startTime, db.endTime, db.hardwareStatus,"
       " SUM( dt.passed ) AS passed,"
       " SUM( dt.marginal ) AS marginal,"
       " SUM( dt.unknown ) AS unknown,"
       " SUM( dt.failed ) AS failed,"
       " COUNT(*) AS testsToAnalyze"
  " FROM bgqDiagBlocks AS db"
       " LEFT OUTER JOIN"
       " bgqDiagTests AS dt"
       " ON db.runId = dt.runId AND db.blockId = dt.blockId"
  " WHERE db.runId = ?"
  " GROUP BY db.blockId, db.startTime, db.endTime, db.hardwareStatus"
" ORDER BY blockId"

            ;

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( sql, cxxdb::ParameterNames{ "runId" } ));

        stmt_ptr->parameters()["runId"].cast( run_id );

        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        while ( rs_ptr->fetch() ) {
            json::Object &block_obj(blocks_arr.addObject());

            const cxxdb::Columns &cols(rs_ptr->columns());

            string block_id(cols["blockId"].getString());

            block_obj.set( "blockId", block_id );

            if ( boost::algorithm::starts_with( block_id, blue_gene::diagnostics::HARDWARE_BLOCK_PREFIX ) ) {
                block_obj.set( "location", block_id.substr( blue_gene::diagnostics::HARDWARE_BLOCK_PREFIX.size() ) );
            }

            if ( cols["startTime"] )  block_obj.set( "start", cols["startTime"].getTimestamp() );
            if ( cols["endTime"] )  block_obj.set( "end", cols["endTime"].getTimestamp() );
            if ( cols["hardwareStatus"] )  block_obj.set( "hardwareStatus", cols["hardwareStatus"].getString() );

            block_obj.set( "testsPassed", cols["passed"] ? cols["passed"].as<int64_t>() : 0 );
            block_obj.set( "testsMarginal", cols["marginal"] ? cols["marginal"].as<int64_t>() : 0 );
            block_obj.set( "testsUnknown", cols["unknown"] ? cols["unknown"].as<int64_t>() : 0 );
            block_obj.set( "testsFailed", cols["failed"] ? cols["failed"].as<int64_t>() : 0 );

            if ( is_running ) {
                block_obj.set( "testsToAnalyze", cols["testsToAnalyze"] ? cols["testsToAnalyze"].as<int64_t>() : 0 );
            }
        }
    }


    _diagnostics_runs.getRunSnapshot(
            run_id,
            _getStrand().wrap( boost::bind(
                    &Run::_gotRunSnapshot,
                    this,
                    capena::server::AbstractResponder::shared_from_this(),
                    obj_val_ptr,
                    _1
                ) )
        );
}


void Run::_doPost( json::ConstValuePtr val_ptr )
{
    const string &run_id_str(_getRequestedResourcePath().back());

    Error::Data error_data;
    error_data["runId"] = run_id_str;

    blue_gene::diagnostics::RunId run_id;

    try {

        run_id = lexical_cast<blue_gene::diagnostics::RunId>( run_id_str );

    } catch ( std::exception& e ) {

        BOOST_THROW_EXCEPTION( Error(
                string() + "Invalid run ID '" + run_id_str + "'",
                "changeDiagnosticsRun",
                "invalidRunId",
                error_data,
                capena::http::Status::NotFound
            ) );
    }


    if ( ! _isUserAdministrator() ) {
        LOG_WARN_MSG( "Could not end diagnostics run because " << _getRequestUserInfo() << " doesn't have authority (must be administrator)." );

        ras::postAdminAuthorityFailure( _getRequestUserInfo(), "end diagnostics run" );

        BOOST_THROW_EXCEPTION( Error(
                "Could not end diagnostics run because the user doesn't have authority.",
                "changeDiagnosticsRun", "authority", error_data,
                capena::http::Status::Forbidden
            ) );

        return;
    }


    json::Object obj;

    try {

        obj = val_ptr->getObject();

    } catch ( std::exception& e ) {

        BOOST_THROW_EXCEPTION( Error(
                "Invalid request, data is not a valid JSON document.",
                "diagnosticsRunOperation",
                "dataFormat",
                error_data,
                capena::http::Status::BadRequest
            ) );

    }

    json::Object::const_iterator op_i(obj.find( "operation" ));

    if ( op_i == obj.end() ) {

        BOOST_THROW_EXCEPTION( Error(
                "Invalid request, no operation provided.",
                "diagnosticsRunOperation",
                "noOperation",
                error_data,
                capena::http::Status::BadRequest
            ) );
    }

    if ( ! op_i->second->isString() ) {

        BOOST_THROW_EXCEPTION( Error(
                "Invalid request, operation is not a string.",
                "diagnosticsRunOperation",
                "operationNotString",
                error_data,
                capena::http::Status::BadRequest
            ) );
    }

    string operation(op_i->second->getString());

    error_data["operation"] = operation;

    if ( operation != "cancel" ) {

        BOOST_THROW_EXCEPTION( Error(
                string() + "Operation '" + operation + "' not valid for run ID " + run_id_str,
                "diagnosticsRunOperation",
                "unexpectedOperation",
                error_data,
                capena::http::Status::BadRequest
            ) );
    }

    LOG_INFO_MSG( "Request to end run " << run_id << " by " << _getRequestUserInfo() );

    _diagnostics_runs.cancelRun(
            run_id,
            _getStrand().wrap( boost::bind(
                    &Run::_gotCancelResult,
                    this,
                    capena::server::AbstractResponder::shared_from_this(),
                    run_id,
                    operation,
                    _1
                ) )
        );

}


void Run::_gotRunSnapshot(
        capena::server::ResponderPtr /*shared_ptr*/,
        json::ObjectValuePtr obj_val_ptr,
        blue_gene::diagnostics::Runs::RunInfoPtr run_info_ptr
    )
{
    try {

        if ( run_info_ptr ) {

            json::Object &obj(obj_val_ptr->getObject());

            obj.set( "user", run_info_ptr->user );
            obj.set( "cancelable", run_info_ptr->canceled ? "canceled" : "cancelable" );

        }

        capena::server::Response &response(_getResponse());

        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( *obj_val_ptr, response.out() );

    } catch ( std::exception& e ) {

        _handleError( e );

    }

}


void Run::_gotCancelResult(
        capena::server::ResponderPtr /*shared_ptr*/,
        blue_gene::diagnostics::RunId run_id,
        std::string operation,
        blue_gene::diagnostics::Runs::CancelResult::Value res
    )
{
    try {

        if ( res == blue_gene::diagnostics::Runs::CancelResult::NotFound ) {

            Error::Data error_data;
            error_data["runId"] = lexical_cast<string>( run_id );
            error_data["operation"] = operation;

            BOOST_THROW_EXCEPTION( Error(
                    string() + "There is no diagnostics run with run ID " + lexical_cast<string>( run_id ) + " currently running.",
                    "cancelDiagnosticsRun",
                    "notFound",
                    error_data,
                    capena::http::Status::NotFound
                ) );

        }

        capena::server::Response &response(_getResponse());

        response.setStatus( capena::http::Status::NoContent );
        response.headersComplete();

    } catch ( std::exception& e ) {

        _handleError( e );

    }
}


} } } // namespace bgws::responder::diagnostics
