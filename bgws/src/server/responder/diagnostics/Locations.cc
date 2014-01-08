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

/*! \page diagnosticsLocationsResource /bg/diagnostics/locations

This resource provides diagnostics queries on locations.

- \subpage diagnosticsLocationsResourceGet

 */

/*! \page diagnosticsLocationsResourceGet GET /bg/diagnostics/locations

Diagnostics queries by location.

\note This resource supports \ref partialResponse "partial responses".

\section Authority

The user must have hardware READ authority.

\section diagnosticsLocationsResourceGetParameters Query parameters

- hardwareStatus: Hardware status
  - A string of characters, where each character is a status code.
  - Only results with status in the string are returned.
  - Status codes and the status value that each code maps to are:
    - S: Success
    - M: Marginal
    - F: Failed
    - K: Unknown
    - U: Uninitialized

- interval: Run interval, a \ref timeIntervalFormat "time interval".
- location: \ref wildcardFormat "wildcard string".
- replace: Replace indicator, one of "any", "true", or "false". The default is "any".

- sort: Sets the sort order. Format is [&lt;direction&gt;]&lt;field&gt;
  - direction is + for ascending or - for descending, the default is ascending.
  - field is one of:
    - location
    - lastRun
    - hardwareStatus
    - replace


\section diagnosticsLocationsResourceGetResponse JSON response format

<pre>
[
  {
    "location" : &quot;<i>string</i>&quot;,
    "lastRun" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
    "hardwareStatus" : &quot;<i>string</i>&quot;,
    "replace" : <i>Boolean</i>
  },
  ...
]
</pre>

\section Errors

HTTP status: 403 Forbidden
- authority: The user doesn't have READ hardware authority.

 */


#include "Locations.hpp"

#include "common.hpp"

#include "../../BlockingOperationsThreadPool.hpp"
#include "../../dbConnectionPool.hpp"
#include "../../Error.hpp"
#include "../../RequestRange.hpp"

#include "../../query/diagnostics/Locations.hpp"
#include "../../query/diagnostics/LocationsOptions.hpp"

#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>

#include <boost/program_options.hpp>

#include <string>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {
namespace diagnostics {


const capena::http::uri::Path Locations::RESOURCE_PATH(common::getBaseUrlPath() / "locations");
const capena::http::uri::Path Locations::RESOURCE_PATH_EMPTY_CHILD(RESOURCE_PATH / "");


void Locations::_doGet()
{
    if ( ! _userHasHardwareRead() ) {
        LOG_WARN_MSG( "Could not get diagnostics locations because " << _getRequestUserInfo() << " doesn't have authority." );

        BOOST_THROW_EXCEPTION( Error(
                "Could not get diagnostics locations because the user doesn't have authority.",
                "getDiagnosticsLocations", "authority", Error::Data(),
                capena::http::Status::Forbidden
            ) );

        return;
    }

    const capena::server::Request &request(_getRequest());

    RequestRange req_range( request );

    query::diagnostics::LocationsOptions options(
            request.getUrl().getQuery().calcArguments(),
            req_range
        );

    _blocking_operations_thread_pool.post( boost::bind(
            &Locations::_startQuery, this,
            capena::server::AbstractResponder::shared_from_this(),
            options,
            req_range
        ) );

}


void Locations::_startQuery(
        capena::server::ResponderPtr,
        const query::diagnostics::LocationsOptions& options,
        const RequestRange& req_range
    )
{
    try {
        query::diagnostics::Locations query( options );

        auto conn_ptr(dbConnectionPool::getConnection());
        uint64_t all_count(0);
        cxxdb::ResultSetPtr rs_ptr;

        query.execute( conn_ptr, &all_count, &rs_ptr );


        _getStrand().post( boost::bind(
                &Locations::_queryComplete, this,
                capena::server::AbstractResponder::shared_from_this(),
                all_count,
                conn_ptr,
                rs_ptr,
                req_range
            ) );
    } catch ( std::exception& e ) {

        _inCatchPostCurrentExceptionToHandlerFn();

    }

}


void Locations::_queryComplete(
        capena::server::ResponderPtr,
        uint64_t all_count,
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr,
        const RequestRange& req_range
    )
{
    try {
        json::ArrayValue arr_val;
        json::Array &arr(arr_val.get());

        capena::server::Response &response(_getResponse());

        if ( all_count != 0 && rs_ptr ) {

            while ( rs_ptr->fetch() ) {
                const cxxdb::Columns &cols(rs_ptr->columns());

                json::Object &obj(arr.addObject());

                obj.set( "location", cols["location"].getString() );
                obj.set( "lastRun", cols["lastRun"].getTimestamp() );
                obj.set( "hardwareStatus", cols["hardwareStatus"].getString() );
                obj.set( "replace", (cols["hardwareReplace"].getString() == "T" ? true : false) );
            }

            req_range.updateResponse( response, arr.size(), all_count );
        }

        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( arr_val, response.out() );

    } catch ( std::exception& e ) {
        _handleError( e );
    }
}



} } } // namespace bgws::responder::diagnostics
