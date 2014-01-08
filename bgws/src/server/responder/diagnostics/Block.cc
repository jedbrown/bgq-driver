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

/*! \page diagnosticsBlockResource /bg/diagnostics/blocks/<i>blockId</i>

This resource provides details for a block used for diagnostics.

- \subpage diagnosticsBlockResourceGet

 */

/*! \page diagnosticsBlockResourceGet GET /bg/diagnostics/blocks/<i>blockId</i>

Get details for a block used for diagnostics.

\section Authority

The user must have hardware READ authority.

\section diagnosticsBlocksResourceGetParameters Query parameters

- runId: the run ID. By default, the most recently completed run for the block is returned.

\section diagnosticsBlocksResourceGetResponse JSON response format

<pre>
{
  "location" : &quot;<i>string</i>&quot;, // optional
  "start" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
  "end" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;, // optional
  "hardwareStatus" : &quot;<i>string</i>&quot;,
  "runId" : &quot;<i>string</i>&quot;,
  "runStart" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
  "runEnd" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;, // optional
  "runStatus" : &quot;<i>string</i>&quot;,

  "testcases" : [
      {
        "testcase" : &quot;<i>string</i>&quot;,
        "start" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;, // optional
        "end" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;, // optional
        "passed" : <i>number</i>, // optional
        "marginal" : <i>number</i>, // optional
        "unknown" : <i>number</i>, // optional
        "failed" : <i>number</i> // optional
      },
      ...
    ],

  "history" : [ // optional
      {
        "start" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
        "hardwareStatus" : &quot;<i>string</i>&quot;
        "runId" : &quot;<i>string</i>&quot;,
      },
      ...
    ]
}
</pre>


\section Errors

HTTP status: 403 Forbidden
- authority: The user doesn't have READ hardware authority.

HTTP status: 404 Not Found
- notFound: No diagnostics run on the block was found.

 */


#include "Block.hpp"

#include "Blocks.hpp"

#include "../../blue_gene.hpp"
#include "../../dbConnectionPool.hpp"
#include "../../Error.hpp"

#include "../../blue_gene/diagnostics/types.hpp"

#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <string>


using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {
namespace diagnostics {


bool Block::matchesUrl(
        const capena::http::uri::Path& requested_resource
    )
{ return ((requested_resource.calcParent() == Blocks::RESOURCE_PATH) && (! requested_resource.back().empty()) ); }


capena::http::uri::Path Block::calcUri(
        const capena::http::uri::Path& path_base,
        const std::string& block_id
    )
{
    return (path_base / Blocks::RESOURCE_PATH / block_id);
}


void Block::_doGet()
{

    if ( ! _userHasHardwareRead() ) {
        LOG_WARN_MSG( "Could not get diagnostics block because " << _getRequestUserInfo() << " doesn't have authority." );

        BOOST_THROW_EXCEPTION( Error(
                "Could not get diagnostics block because the user doesn't have authority.",
                "getDiagnosticsBlock", "authority", Error::Data(),
                capena::http::Status::Forbidden
            ) );

        return;
    }

    const string &block_id(_getRequestedResourcePath().back());

    LOG_DEBUG_MSG( "block id is '" << block_id << "'" );

    string location;

    if ( boost::algorithm::starts_with( block_id, blue_gene::diagnostics::HARDWARE_BLOCK_PREFIX ) ) {
        location = block_id.substr( blue_gene::diagnostics::HARDWARE_BLOCK_PREFIX.size() );
    }


    namespace po = boost::program_options;

    string run_id_str;

    po::options_description desc;

    desc.add_options()
            ( "runId", po::value( &run_id_str ) )
        ;

    po::variables_map vm;
    po::store( po::command_line_parser( _getRequest().getUrl().getQuery().calcArguments() ).options( desc ).allow_unregistered().run(), vm );
    po::notify( vm );


    blue_gene::diagnostics::RunId run_id(-1);

    if ( ! run_id_str.empty() ) {
        try {
            run_id = lexical_cast<blue_gene::diagnostics::RunId>( run_id_str );

            LOG_DEBUG_MSG( "run_id=" << run_id << " from '" << run_id_str << "'" );
        } catch ( std::exception& e ) {
            LOG_DEBUG_MSG( "invalid run ID option '" << run_id_str << "' ignored." );
        }
    }


    json::ObjectValue obj_val;
    json::Object &obj(obj_val.getObject());

    if ( ! location.empty() ) {
        obj.set( "location", location );
    }

    auto conn_ptr(dbConnectionPool::getConnection());

    { // Details for the block.

        const string GET_RUNID_SQL =

 "WITH finishedBlock AS"
  " ( SELECT *"
      " FROM bgqDiagBlocks"
      " WHERE blockId = ? AND"
            " endTime IS NOT NULL"
  " )"
" SELECT fb.startTime AS blockStartTime, fb.endTime AS blockEndTime, RTRIM(fb.hardwareStatus) AS hardwareStatus, fb.runId,"
       " r.startTime AS runStartTime, r.endTime AS runEndTime, r.diagStatus AS runDiagStatus"
  " FROM ( SELECT MAX(startTime) AS lastRun"
           " FROM finishedBlock"
       " ) AS lr"
       " JOIN"
       " finishedBlock AS fb"
       " ON lr.lastRun = fb.startTime"
       " JOIN"
       " bgqDiagRuns AS r"
       " ON fb.runId = r.runId";
                // 1= blockId

            ;


        const string KNOW_RUNID_SQL =

 "SELECT b.startTime AS blockStartTime, b.endTime AS blockEndTime, RTRIM(b.hardwareStatus) AS hardwareStatus,"
       " r.startTime AS runStartTime, r.endTime aS runEndTime, r.diagStatus AS runDiagStatus"
  " FROM bgqDiagBlocks AS b"
       " JOIN"
       " bgqDiagRuns AS r"
       " ON b.runId = r.runId"
  " WHERE b.blockId = ? AND b.runId = ?";
                // 1= blockId
                // 2= runId

            ;

        cxxdb::QueryStatementPtr stmt_ptr;

        if ( run_id == blue_gene::diagnostics::RunId(-1) ) {
            stmt_ptr = conn_ptr->prepareQuery( GET_RUNID_SQL, cxxdb::ParameterNames{ "blockId" } );
            stmt_ptr->parameters()[ "blockId" ].set( block_id );
        } else {
            stmt_ptr = conn_ptr->prepareQuery( KNOW_RUNID_SQL,  cxxdb::ParameterNames{ "blockId", "runId" } );
            stmt_ptr->parameters()[ "blockId" ].set( block_id );
            stmt_ptr->parameters()[ "runId" ].set( run_id );
        }

        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        if ( ! rs_ptr->fetch() ) {

            Error::Data data;
            data["blockId"] = block_id;

            BOOST_THROW_EXCEPTION( Error(
                    boost::str( boost::format( "Could not get details for diagnostics block '%1%'." ) % block_id ),
                    "getDiagnosticsBlockDetails", "notFound", data,
                    capena::http::Status::NotFound
                ) );

        }

        const cxxdb::Columns &cols(rs_ptr->columns());

        if ( cols["blockStartTime"] )  obj.set( "start", cols["blockStartTime"].getTimestamp() );
        if ( cols["blockEndTime"] )  obj.set( "end", cols["blockEndTime"].getTimestamp() );
        if ( cols["hardwareStatus"] )  obj.set( "hardwareStatus", cols["hardwareStatus"].getString() );
        if ( cols["runStartTime"] )  obj.set( "runStart", cols["runStartTime"].getTimestamp() );
        if ( cols["runEndTime"] )  obj.set( "runEnd", cols["runEndTime"].getTimestamp() );
        if ( cols["runDiagStatus"] )  obj.set( "runStatus", cols["runDiagStatus"].getString() );

        if ( run_id == blue_gene::diagnostics::RunId(-1) ) {
            run_id = cols["runId"].as<blue_gene::diagnostics::RunId>();
        }

        obj.set( "runId", lexical_cast<string>( run_id ) );
    }


    { // test case summary.

        const string sql =
            "SELECT RTRIM(testcase) AS testcase, startTime, endTime, passed, marginal, unknown, failed"
             " FROM bgqDiagTests"
             " WHERE blockId = ? AND runId = ?"
             " ORDER BY testcase ASC";

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( sql, cxxdb::ParameterNames{ "blockId", "runId" } ));

        stmt_ptr->parameters()["blockId"].set( block_id );
        stmt_ptr->parameters()["runId"].cast( run_id );

        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        json::Array *tcs_arr_p(NULL);

        while ( rs_ptr->fetch() ) {
            const cxxdb::Columns &cols(rs_ptr->columns());

            if ( ! tcs_arr_p )  tcs_arr_p = &(obj.createArray( "testcases" ));
            json::Object &tc_obj(tcs_arr_p->addObject());
            tc_obj.set( "testcase", cols["testcase"].getString() );
            if ( cols["startTime"] )  tc_obj.set( "start", cols["startTime"].getTimestamp() );
            if ( cols["endTime"] )  tc_obj.set( "end", cols["endTime"].getTimestamp() );

            int64_t passed = cols["passed"].as<int64_t>();
            int64_t marginal = cols["marginal"].as<int64_t>();
            int64_t unknown = cols["unknown"].as<int64_t>();
            int64_t failed = cols["failed"].as<int64_t>();

            if ( passed || marginal || unknown || failed ) {
                tc_obj.set( "passed", passed );
                tc_obj.set( "marginal", marginal);
                tc_obj.set( "unknown", unknown );
                tc_obj.set( "failed", failed );
            }
        }

    }


    { // History

        const string sql =

 "SELECT b.runId, b.hardwareStatus, b.startTime"
  " FROM bgqDiagBlocks AS b"
  " WHERE b.startTime IS NOT NULL AND b.blockId = ?"
  " ORDER BY b.startTime DESC";
                // 1= blockId

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( sql, cxxdb::ParameterNames{ "blockId" } ));
        stmt_ptr->parameters()[ "blockId" ].set( block_id );

        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        json::Array *h_arr_p(NULL);

        while ( rs_ptr->fetch() ) {

            const cxxdb::Columns &cols(rs_ptr->columns());

            if ( ! h_arr_p )  h_arr_p = &(obj.createArray( "history" ));
            json::Object &h_obj(h_arr_p->addObject());

            blue_gene::diagnostics::RunId run_id(cols["runId"].as<blue_gene::diagnostics::RunId>());

            h_obj.set( "runId", lexical_cast<string>( run_id ) );
            h_obj.set( "hardwareStatus", cols["hardwareStatus"].getString() );
            h_obj.set( "start", cols["startTime"].getTimestamp() );
        }

    }

    capena::server::Response &response(_getResponse());

    response.setContentTypeJson();
    response.headersComplete();

    json::Formatter()( obj_val, response.out() );
}


} } } // namespace bgws::responder::diagnostics
