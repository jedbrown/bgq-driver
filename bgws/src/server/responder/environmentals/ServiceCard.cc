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

/*! \page serviceCardEnvironmentalsResource /bg/environmentals/serviceCard

Service card environmentals.

- \subpage serviceCardEnvironmentalsResourceGet

 */

/*! \page serviceCardEnvironmentalsResourceGet GET /bg/environmentals/serviceCard

Query service card environmentals.

\note This resource supports \ref partialResponse "partial responses".

\section Authorization

The user must have hardware READ authority.


\section queryParms Query parameters

- location: \ref wildcardFormat "wildcard string"
- interval: a \ref timeIntervalFormat "time interval". If no interval, will return most recent for each location.
- v12p: \ref numberComparisonFormat "double-precision floating point number comparison"
- v12r5: \ref numberComparisonFormat "double-precision floating point number comparison"
- v15p: \ref numberComparisonFormat "double-precision floating point number comparison"
- v25p: \ref numberComparisonFormat "double-precision floating point number comparison"
- v33p: \ref numberComparisonFormat "double-precision floating point number comparison"
- v50p: \ref numberComparisonFormat "double-precision floating point number comparison"
- sort: Sets the sort order. Format is [&lt;direction&gt;]&lt;field&gt;
  - Fields are:
    - location
    - time
    - v12p
    - v12r5
    - v15p
    - v25p
    - v33p
    - v50p


\section responseFormat JSON response format

<pre>
[
  {
    "location": &quot;<i>location</i>&quot;,
    "time": &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
    "v12p": <i>number</i>,
    "v12r5": <i>number</i>,
    "v15p": <i>number</i>,
    "v25p": <i>number</i>,
    "v33p": <i>number</i>,
    "v50p": <i>number</i>
  },
  ...
]
</pre>

\section Errors

HTTP status: 403 Forbidden
- authority: The user doesn't have hardware READ authority.

 */


#include "ServiceCard.hpp"

#include "../../Error.hpp"
#include "../../RequestRange.hpp"

#include "../../query/env/ServiceCardOptions.hpp"
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


const capena::http::uri::Path ServiceCard::RESOURCE_PATH(capena::http::uri::Path() / "environmentals" / "serviceCard");


void ServiceCard::_doGet()
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

    boost::shared_ptr<query::env::ServiceCardOptions> options_ptr( new query::env::ServiceCardOptions() );

    options_ptr->setArgs( request.getUrl().getQuery().calcArguments(), req_range );

    _query_ptr.reset( new query::env::Query( options_ptr ) );

    _query_ptr->executeAsync(
            _blocking_operations_thread_pool,
            _getStrand().wrap( boost::bind( &ServiceCard::_queryComplete, this,
                    capena::server::AbstractResponder::shared_from_this(),
                    req_range,
                    _1
                ) )
        );

}


void ServiceCard::notifyDisconnect()
{
    LOG_DEBUG_MSG( "Notified client disconnected" );

    query::env::Query::Ptr query_ptr(_query_ptr);

    if ( ! _query_ptr )  return;

    _query_ptr->cancel();
}


void ServiceCard::_queryComplete(
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
                json::Object &obj(arr.addObject());
                obj.set( "location", res.rs_ptr->columns()[BGQDB::DBTServicecardenvironment::LOCATION_COL].getString() );
                obj.set( "time", res.rs_ptr->columns()[BGQDB::DBTServicecardenvironment::TIME_COL].getTimestamp() );
                obj.set( "v12p", res.rs_ptr->columns()[BGQDB::DBTServicecardenvironment::VOLTAGEV12P_COL].as<double>() );
                obj.set( "v12r5", res.rs_ptr->columns()[BGQDB::DBTServicecardenvironment::VOLTAGEV12R5_COL].as<double>() );
                obj.set( "v15p", res.rs_ptr->columns()[BGQDB::DBTServicecardenvironment::VOLTAGEV15P_COL].as<double>() );
                obj.set( "v25p", res.rs_ptr->columns()[BGQDB::DBTServicecardenvironment::VOLTAGEV25P_COL].as<double>() );
                obj.set( "v33p", res.rs_ptr->columns()[BGQDB::DBTServicecardenvironment::VOLTAGEV33P_COL].as<double>() );
                obj.set( "v50p", res.rs_ptr->columns()[BGQDB::DBTServicecardenvironment::VOLTAGEV50P_COL].as<double>() );
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
