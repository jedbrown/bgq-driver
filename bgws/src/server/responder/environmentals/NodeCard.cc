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

/*! \page nodeCardEnvironmentalsResource /bg/environmentals/nodeCard

Node card environmentals.

- \subpage nodeCardEnvironmentalsResourceGet

 */

/*! \page nodeCardEnvironmentalsResourceGet GET /bg/environmentals/nodeCard

Query node card environmentals.

\note This resource supports \ref partialResponse "partial responses".

\section Authorization

The user must have hardware READ authority.


\section queryParms Query parameters

- location: \ref wildcardFormat "wildcard string"
- interval: a \ref timeIntervalFormat "time interval". If no interval, will return most recent for each location.
- v08: \ref numberComparisonFormat "double-precision floating point number comparison"
- v14: \ref numberComparisonFormat "double-precision floating point number comparison"
- v25: \ref numberComparisonFormat "double-precision floating point number comparison"
- v33: \ref numberComparisonFormat "double-precision floating point number comparison"
- v120p: \ref numberComparisonFormat "double-precision floating point number comparison"
- v15: \ref numberComparisonFormat "double-precision floating point number comparison"
- v09: \ref numberComparisonFormat "double-precision floating point number comparison"
- v10: \ref numberComparisonFormat "double-precision floating point number comparison"
- v33p: \ref numberComparisonFormat "double-precision floating point number comparison"
- v12a: \ref numberComparisonFormat "double-precision floating point number comparison"
- v12b: \ref numberComparisonFormat "double-precision floating point number comparison"
- v18: \ref numberComparisonFormat "double-precision floating point number comparison"
- v25p: \ref numberComparisonFormat "double-precision floating point number comparison"
- v12p: \ref numberComparisonFormat "double-precision floating point number comparison"
- v18p: \ref numberComparisonFormat "double-precision floating point number comparison"
- temp0: \ref numberComparisonFormat "integer number comparison"
- temp1: \ref numberComparisonFormat "integer number comparison"
- sort: Sets the sort order. Format is [&lt;direction&gt;]&lt;field&gt;
  - Fields are:
    - location
    - time
    - v08
    - v14
    - v25
    - v33
    - v120p
    - v15
    - v09
    - v10
    - v33p
    - v12a
    - v12b
    - v18
    - v25p
    - v12p
    - v18p
    - temp0
    - temp1


\section responseFormat JSON response format

<pre>
[
  {
    "location": &quot;<i>location</i>&quot;,
    "time": &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
    "v08": <i>number</i>
    "v14": <i>number</i>
    "v25": <i>number</i>
    "v33": <i>number</i>
    "v120p": <i>number</i>
    "v15": <i>number</i>
    "v09": <i>number</i>
    "v10": <i>number</i>
    "v33p": <i>number</i>
    "v12a": <i>number</i>
    "v12b": <i>number</i>
    "v18": <i>number</i>
    "v25p": <i>number</i>
    "v12p": <i>number</i>
    "v18p": <i>number</i>
    "temp0": <i>number</i>
    "temp1": <i>number</i>
  },
  ...
]
</pre>

\section Errors

HTTP status: 403 Forbidden
- authority: The user doesn't have hardware READ authority.

 */


#include "NodeCard.hpp"

#include "../../Error.hpp"
#include "../../RequestRange.hpp"

#include "../../query/env/NodeCardOptions.hpp"
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


const capena::http::uri::Path NodeCard::RESOURCE_PATH(capena::http::uri::Path() / "environmentals" / "nodeCard");


void NodeCard::_doGet()
{

    if ( ! _userHasHardwareRead() ) {
        LOG_WARN_MSG( "Could not get environmentals because " << _getRequestUserInfo() << " doesn't have authority." );

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

    boost::shared_ptr<query::env::NodeCardOptions> options_ptr( new query::env::NodeCardOptions() );

    options_ptr->setArgs( request.getUrl().getQuery().calcArguments(), req_range );

    _query_ptr.reset( new query::env::Query( options_ptr ) );

    _query_ptr->executeAsync(
            _blocking_operations_thread_pool,
            _getStrand().wrap( boost::bind( &NodeCard::_queryComplete, this,
                    capena::server::AbstractResponder::shared_from_this(),
                    req_range,
                    _1
                ) )
        );
}


void NodeCard::notifyDisconnect()
{
    LOG_DEBUG_MSG( "Notified client disconnected" );

    query::env::Query::Ptr query_ptr(_query_ptr);

    if ( ! _query_ptr )  return;

    _query_ptr->cancel();
}


void NodeCard::_queryComplete(
        capena::server::ResponderPtr,
        RequestRange req_range,
        query::env::Query::Result res
    )
{
    try {

        if ( res.exc_ptr != 0 ) {
            std::rethrow_exception( res.exc_ptr );
        }


        json::ArrayValue arr_val;
        json::Array &arr(arr_val.get());

        if ( res.all_count != 0 ) {
            while ( res.rs_ptr->fetch() ) {
                const cxxdb::Columns &cols(res.rs_ptr->columns());

                json::Object &obj(arr.addObject());
                obj.set( "location", cols[BGQDB::DBTNodecardenvironment::LOCATION_COL].getString() );
                obj.set( "time", cols[BGQDB::DBTNodecardenvironment::TIME_COL].getTimestamp() );
                obj.set( "v08", cols[BGQDB::DBTNodecardenvironment::VOLTAGEV08_COL].as<double>() );
                obj.set( "v14", cols[BGQDB::DBTNodecardenvironment::VOLTAGEV14_COL].as<double>() );
                obj.set( "v25", cols[BGQDB::DBTNodecardenvironment::VOLTAGEV25_COL].as<double>() );
                obj.set( "v33", cols[BGQDB::DBTNodecardenvironment::VOLTAGEV33_COL].as<double>() );
                obj.set( "v120p", cols[BGQDB::DBTNodecardenvironment::VOLTAGEV120P_COL].as<double>() );
                obj.set( "v15", cols[BGQDB::DBTNodecardenvironment::VOLTAGEV15_COL].as<double>() );
                obj.set( "v09", cols[BGQDB::DBTNodecardenvironment::VOLTAGEV09_COL].as<double>() );
                obj.set( "v10", cols[BGQDB::DBTNodecardenvironment::VOLTAGEV10_COL].as<double>() );
                obj.set( "v33p", cols[BGQDB::DBTNodecardenvironment::VOLTAGEV33P_COL].as<double>() );
                obj.set( "v12a", cols[BGQDB::DBTNodecardenvironment::VOLTAGEV12A_COL].as<double>() );
                obj.set( "v12b", cols[BGQDB::DBTNodecardenvironment::VOLTAGEV12B_COL].as<double>() );
                obj.set( "v18", cols[BGQDB::DBTNodecardenvironment::VOLTAGEV18_COL].as<double>() );
                obj.set( "v25p", cols[BGQDB::DBTNodecardenvironment::VOLTAGEV25P_COL].as<double>() );
                obj.set( "v12p", cols[BGQDB::DBTNodecardenvironment::VOLTAGEV12P_COL].as<double>() );
                obj.set( "v18p", cols[BGQDB::DBTNodecardenvironment::VOLTAGEV18P_COL].as<double>() );
                obj.set( "temp0", cols[BGQDB::DBTNodecardenvironment::TEMPMONITOR0_COL].as<int32_t>() );
                obj.set( "temp1", cols[BGQDB::DBTNodecardenvironment::TEMPMONITOR1_COL].as<int32_t>() );
            }
        }


        capena::server::Response &response(_getResponse());

        req_range.updateResponse( response, arr.size(), res.all_count );

        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( arr_val, response.out() );

    } catch ( std::exception& e ) {

        _handleError( e );

    }
}


} } } // namespace bgws::responder::environmentals
