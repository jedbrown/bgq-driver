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

/*! \page diagnosticsTestcasesResource /bg/diagnostics/testcases

This resource provides a querying of diagnostics testcase results.

- \subpage diagnosticsTestcasesResourceGet

 */

/*! \page diagnosticsTestcasesResourceGet GET /bg/diagnostics/testcases

Query diagnostics testcases.

\section Authority

The user must have hardware READ authority.

\section diagnosticsTestcasesResourceGetParameters Query parameters

- blockId : Block ID, string.
- hardwareStatus : Hardware status, string, one of "unknown", "failed", "marginal", "success".
- location : Location string.
- runId : Diagnostics run ID, string.
- testcase : testcase name, string.

If the query parameters are only location,
the response will be the most recent result for each testcase at that location.


\section diagnosticsTestcasesResourceGetResponse JSON response format

<pre>
[
  {
    "runId" : &quot;<i>string</i>&quot;,
    "blockId" : &quot;<i>string</i>&quot;,
    "testcase" : &quot;<i>string</i>&quot;,
    "location" : &quot;<i>string</i>&quot;,
    "serialnumber" : &quot;<i>string</i>&quot;, // optional
    "end" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
    "hardwareStatus" : &quot;<i>string</i>&quot;,
    "hardwareReplace" : <i>Boolean</i>,
    "logfile" : &quot;<i>string</i>&quot;, //optional
    "analysis" : &quot;<i>string</i>&quot; // optional
  },
  ...
]
</pre>

\section Errors

HTTP status: 403 Forbidden
- authority: The user doesn't have READ hardware authority.

 */


#include "Testcases.hpp"

#include "common.hpp"

#include "../../dbConnectionPool.hpp"
#include "../../DiagnosticsRunIdOption.hpp"
#include "../../Error.hpp"
#include "../../StringDbColumnOption.hpp"
#include "../../WhereClause.hpp"

#include "../../blue_gene/diagnostics/types.hpp"

#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <string>


using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {
namespace diagnostics {


const capena::http::uri::Path Testcases::RESOURCE_PATH(common::getBaseUrlPath() / "testcases");
const capena::http::uri::Path Testcases::RESOURCE_PATH_EMPTY_CHILD(RESOURCE_PATH / "");


void Testcases::_doGet()
{
    if ( ! _userHasHardwareRead() ) {
        LOG_WARN_MSG( "Could not get diagnostics testcases because " << _getRequestUserInfo() << " doesn't have authority." );

        BOOST_THROW_EXCEPTION( Error(
                "Could not get diagnostics testcases because the user doesn't have authority.",
                "getDiagnosticsTestcases", "authority", Error::Data(),
                capena::http::Status::Forbidden
            ) );

        return;
    }


    StringDbColumnOption block_id( "blockId", BGQDB::DBTDiagtests::BLOCKID_SIZE );
    StringDbColumnOption hardware_status( "hardwareStatus", BGQDB::DBTDiagtests::HARDWARESTATUS_SIZE );
    StringDbColumnOption location( "location", BGQDB::DBTDiagresults::LOCATION_SIZE );
    DiagnosticsRunIdOption run_id_option;
    StringDbColumnOption testcase( "testcase", BGQDB::DBTDiagtests::TESTCASE_SIZE );

    namespace po = boost::program_options;

    po::options_description desc;

    desc.add_options()
            ( "runId", po::value( &run_id_option ) )
        ;

    block_id.addTo( desc );
    hardware_status.addTo( desc );
    location.addTo( desc );
    testcase.addTo( desc );

    po::variables_map vm;
    po::store( po::command_line_parser( _getRequest().getUrl().getQuery().calcArguments() ).options( desc ).allow_unregistered().run(), vm );
    po::notify( vm );


    WhereClause where_clause;
    cxxdb::ParameterNames param_names;

    block_id.addTo( where_clause, param_names, BGQDB::DBTDiagtests::BLOCKID_COL );
    hardware_status.addTo( where_clause, param_names, BGQDB::DBTDiagtests::HARDWARESTATUS_COL );
    location.addTo( where_clause, param_names, BGQDB::DBTDiagresults::LOCATION_COL );
    testcase.addTo( where_clause, param_names, BGQDB::DBTDiagtests::TESTCASE_COL );

    if ( run_id_option.getOpt() ) {
        where_clause.add( "runId = ?" );
        param_names.push_back( "runId" );
    }

    bool only_location(location.hasValue() && (! block_id.hasValue()) && (! hardware_status.hasValue()) && (! run_id_option.getOpt()) && (! testcase.hasValue()));


    auto conn_ptr(dbConnectionPool::getConnection());

    string sql;

    if ( only_location ) {

        sql =

 "WITH mr AS ("
  " SELECT testcase, MAX(endTime) AS et"
    " FROM bgqDiagResults"
    " WHERE location = ?"
    " GROUP BY testcase"
" )"
" SELECT dr.runId, dr.blockId, dr.testcase, dr.location, dr.serialnumber, dr.endTime, dr.hardwareStatus, dr.hardwareReplace, dr.logfile, dr.analysis"
  " FROM bgqDiagResults AS dr"
       " JOIN"
       " mr"
       " ON dr.testcase = mr.testcase AND dr.endTime = mr.et"
  " " + where_clause.getString() +
  " ORDER BY testcase"

            ;

        param_names.insert( param_names.begin(), "location" );

    } else {

        sql = string() +

 "SELECT runId, blockId, testcase, location, serialnumber, endTime, hardwareStatus, hardwareReplace, logfile, analysis"
  " FROM bgqDiagResults"
  " " + where_clause.getString() +
  " ORDER BY runId, blockId, testcase, location"

            ;

    }

    cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( sql, param_names ));

    cxxdb::Parameters &params(stmt_ptr->parameters());

    block_id.bindParameters( params );
    hardware_status.bindParameters( params );
    location.bindParameters( params );
    testcase.bindParameters( params );

    if ( run_id_option.getOpt() ) {
        DiagnosticsRunIdOption::RunIdOpt run_id_opt(run_id_option.getOpt());
        params["runId"].cast( *run_id_opt );
    }

    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());


    json::ArrayValue arr_val;
    json::Array &arr(arr_val.get());

    while ( rs_ptr->fetch() ) {
        json::Object &obj(arr.addObject());

        const cxxdb::Columns &cols(rs_ptr->columns());

        obj.set( "runId", lexical_cast<string>( cols["runId"].as<blue_gene::diagnostics::RunId>() ) );
        obj.set( "blockId", cols["blockId"].getString() );
        obj.set( "testcase", cols["testcase"].getString() );
        obj.set( "location", cols["location"].getString() );
        if ( cols["serialnumber"] )  obj.set( "serialnumber", cols["serialnumber"].getString() );
        obj.set( "end", cols["endTime"].getTimestamp() );
        obj.set( "hardwareStatus", cols["hardwareStatus"].getString() );
        obj.set( "hardwareReplace", bool(cols["hardwareReplace"].getChar() == 'T') );
        if ( cols["logfile"] )  obj.set( "logfile", cols["logfile"].getString() );
        if ( cols["analysis"] )  obj.set( "analysis", cols["analysis"].getString() );
    }

    capena::server::Response &response(_getResponse());

    response.setContentTypeJson();
    response.headersComplete();

    json::Formatter()( arr_val, response.out() );
}


} } } // namespace bgws::responder::diagnostics
