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

/*! \page alertsResource /bg/alerts

This resource is a container for system alerts.

- \subpage alertsResourceGet

 */

/*! \page alertsResourceGet GET /bg/alerts

Get a summary of alerts.

\note This resource supports \ref partialResponse "partial responses".


\section Authority

The user must have hardware READ authority.


\section alertsResourceGetParameters Query parameters

- dup: Set to T to include duplicate events in the response.
- interval: a \ref timeIntervalFormat "time interval" -- <b>New for V1R1M1</b>
- location: \ref multiWildcardFormat "multi-wildcard string"
- locationType: The location types to return.
  - By default, all location types are returned.
  - A string where each character is a code for a location type.
    - A : Application
    - J : Job
    - C : Compute
    - I : I/O
- state: The states to return.
  - By default, all states are returned.
  - A string where each character is a code for an alert state. The states are numbers.
    - 1 : Open
    - 2 : Closed
- severity: The severities to return.
  - By default, all severities are returned.
  - A string where each character is a code for a severity.
    - F : Fatal
    - E : Error
    - W : Warning
    - I : Info
- urgency: The urgencies to return.
  - By default, all urgencies are returned.
  - A string where each character is a code for an urgency.
    - I : Immediate
    - S : Schedule
    - N : Normal
    - D : Defer
    - O : Optional
- sort: Sets the sort order. Format is [&lt;direction&gt;]&lt;field&gt;
  - direction is + for ascending or - for descending, the default is ascending.
  - field is one of:
    - recId
    - alertId
    - created
    - severity
    - urgency
    - eventLocationType
    - eventLocation
    - fruLocation
    - recommendation
    - reason
    - source
    - state


\section alertsResourceGetResponse JSON response format

<pre>
[
  { "recId": <i>integer</i>,
    "alertId" : &quot;<i>string</i>&quot;,
    "created" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
    "severity" : &quot;<i>string</i>&quot;,
    "urgency" : &quot;<i>string</i>&quot;,
    "eventLocationType" : &quot;<i>string</i>&quot;,
    "eventLocation" : &quot;<i>string</i>&quot;,
    "fruLocation" : &quot;<i>string</i>&quot;, // optional
    "recommendation" : &quot;<i>string</i>&quot;,
    "reason" : &quot;<i>string</i>&quot;,
    "rawData" : &quot;<i>string</i>&quot;, // optional
    "source" : &quot;<i>string</i>&quot;,
    "state": <i>integer</i>, // optional
    "duplicateOf" : &quot;\ref alertResource&quot;, // value is the alert that this alert is a duplicate of, optional
    "URI" : &quot;\ref alertResource&quot;
  },
  ...
]
</pre>


\section Errors

HTTP status: 403 Forbidden
- authority: The user doesn't have READ hardware authority.

 */


#include "Alerts.hpp"

#include "../Error.hpp"
#include "../RequestRange.hpp"

#include "capena-http/http/http.hpp"

#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/throw_exception.hpp>


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {


const capena::http::uri::Path Alerts::RESOURCE_PATH(capena::http::uri::Path() / "alerts");


void Alerts::_doGet()
{
    _checkAuthority();

    const auto &request(_getRequest());

    static const unsigned DEFAULT_ITEM_COUNT(20), MAX_COUNT(100);
    RequestRange req_range( request, DEFAULT_ITEM_COUNT, MAX_COUNT );


    query::AlertsOptions options(
            req_range,
            request.getUrl().getQuery()
        );


    boost::shared_ptr<query::Alerts> query_ptr( new query::Alerts(
            _blocking_operations_thread_pool,
            _getDynamicConfiguration().getPathBase()
        ) );

    query_ptr->executeAsync(
            options,
            _getStrand().wrap( boost::bind(
                    &Alerts::_queryComplete, this,
                    _1, _2, _3,
                    req_range,
                    capena::server::AbstractResponder::shared_from_this(),
                    query_ptr
                ) )
        );

}


void Alerts::_checkAuthority()
{
    if ( _userHasHardwareRead() ) {
        return;
    }

    LOG_WARN_MSG( "Could not get alerts because " << _getRequestUserInfo() << " doesn't have authority." );

    BOOST_THROW_EXCEPTION( Error(
            "Could not get alerts because the user doesn't have authority.",
            "getAlerts", "authority", Error::Data(),
            capena::http::Status::Forbidden
        ) );
}


void Alerts::_queryComplete(
        std::exception_ptr exc_ptr,
        json::ArrayValuePtr arr_val_ptr,
        uint64_t total_count,
        RequestRange req_range,
        capena::server::ResponderPtr,
        boost::shared_ptr<query::Alerts>
    )
{
    try {

        if ( exc_ptr != 0 ) {
            std::rethrow_exception( exc_ptr );
        }


        auto &response(_getResponse());

        req_range.updateResponse(
                response,
                arr_val_ptr->get().size(),
                total_count
            );

        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( *arr_val_ptr, response.out() );

    } catch ( std::exception& e ) {
        _handleError( e );
    }

}


}} // namespace bgws::responder
