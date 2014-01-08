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

/*! \page linkChipEnvironmentalsResource /bg/environmentals/linkChip

Link chip environmentals.

- \subpage linkChipEnvironmentalsResourceGet

 */

/*! \page linkChipEnvironmentalsResourceGet GET /bg/environmentals/linkChip

Query link chip environmentals.

\note This resource supports \ref partialResponse "partial responses".

\section Authorization

The user must have hardware READ authority.


\section queryParms Query parameters

- location: \ref wildcardFormat "wildcard string"
- interval: a \ref timeIntervalFormat "time interval". If no interval, will return most recent for each location.
- temp: \ref numberComparisonFormat "integer number comparison"
- sort: Sets the sort order. Format is [&lt;direction&gt;]&lt;field&gt;
  - Fields are:
    - location
    - time
    - temp


\section responseFormat JSON response format

<pre>
[
  {
    "location": &quot;<i>location</i>&quot;,
    "time": &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
    "temp": <i>number</i>
  },
  ...
]
</pre>

\section Errors

HTTP status: 403 Forbidden
- authority: The user doesn't have hardware READ authority.

 */


#include "LinkChip.hpp"

#include "../../dbConnectionPool.hpp"
#include "../../Error.hpp"
#include "../../RequestRange.hpp"

#include "../../query/env/LinkChipOptions.hpp"
#include "../../query/env/Query.hpp"

#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <string>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {
namespace environmentals {


const capena::http::uri::Path LinkChip::RESOURCE_PATH(capena::http::uri::Path() / "environmentals" / "linkChip");


void LinkChip::doGet()
{

    if ( ! _userHasHardwareRead() ) {
        LOG_WARN_MSG( "Could not get environmentals because " << getRequestUserInfo() << " doesn't have authority." );

        BOOST_THROW_EXCEPTION( Error(
                "Could not get environmentals because the user doesn't have authority.",
                "getEnvironmentals", "authority", Error::Data(),
                capena::http::Status::Forbidden
            ) );

        return;
    }


    const capena::server::Request &request(_getRequest());

    static const unsigned DefaultRangeSize(50), MaxRangeSize(100);
    RequestRange req_range( request, DefaultRangeSize, MaxRangeSize );

    auto conn_ptr(dbConnectionPool::getConnection());
    uint64_t all_count(0);
    cxxdb::ResultSetPtr rs_ptr;

    boost::shared_ptr<query::env::LinkChipOptions> options_ptr( new query::env::LinkChipOptions() );

    options_ptr->setArgs( request.getUrl().getQuery().calcArguments(), req_range );

    query::env::Query query( options_ptr );

    query.execute(
            conn_ptr,
            &all_count,
            &rs_ptr
        );

    json::ArrayValue arr_val;
    json::Array &arr(arr_val.get());

    if ( all_count != 0 ) {
        while ( rs_ptr->fetch() ) {
            json::Object &obj(arr.addObject());
            obj.set( "location", rs_ptr->columns()[BGQDB::DBTLinkchipenvironment::LOCATION_COL].getString() );
            obj.set( "time", rs_ptr->columns()[BGQDB::DBTLinkchipenvironment::TIME_COL].getTimestamp() );
            obj.set( "temp", rs_ptr->columns()[BGQDB::DBTLinkchipenvironment::ASICTEMP_COL].as<int32_t>() );
        }
    }


    capena::server::Response &response(_getResponse());

    req_range.updateResponse( response, arr.size(), all_count );

    response.setContentTypeJson();
    response.headersComplete();

    json::Formatter()( arr_val, response.out() );
}


} } } // namespace bgws::responder::environmentals
