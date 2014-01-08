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

#include "../../BlockingOperationsThreadPool.hpp"
#include "../../dbConnectionPool.hpp"
#include "../../Error.hpp"

#include "../../blue_gene/diagnostics/types.hpp"

#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>

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


    query::diagnostics::TestcasesOptions query_options(
            _getRequest().getUrl().getQuery()
        );

    _blocking_operations_thread_pool.post( boost::bind(
            &Testcases::_startQuery, this,
            capena::server::AbstractResponder::shared_from_this(),
            query_options
        ) );

}


void Testcases::_startQuery(
        capena::server::ResponderPtr,
        const query::diagnostics::TestcasesOptions& query_options
    )
{
    query::diagnostics::Testcases query;

    auto conn_ptr(dbConnectionPool::getConnection());

    cxxdb::ResultSetPtr rs_ptr;

    query.execute(
            query_options,
            conn_ptr,
            &rs_ptr
        );

    _getStrand().post( boost::bind(
            &Testcases::_queryComplete, this,
            capena::server::AbstractResponder::shared_from_this(),
            conn_ptr, rs_ptr
        ) );
}


void Testcases::_queryComplete(
        capena::server::ResponderPtr,
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
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
