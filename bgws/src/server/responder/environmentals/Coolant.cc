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

/*! \page coolantEnvironmentalsResource /bg/environmentals/coolant

Coolant environmentals.

- \subpage coolantEnvironmentalsResourceGet

 */

/*! \page coolantEnvironmentalsResourceGet GET /bg/environmentals/coolant

Query coolant environmentals.

\note This resource supports \ref partialResponse "partial responses".

\section Authorization

The user must have hardware READ authority.


\section queryParms Query parameters

- location: \ref wildcardFormat "wildcard string"
- interval: a \ref timeIntervalFormat "time interval". If no interval, will return most recent for each location.
- inletFlowRate: \ref numberComparisonFormat "double-precision floating point number comparison"
- outletFlowRate: \ref numberComparisonFormat "double-precision floating point number comparison"
- coolantPressure: \ref numberComparisonFormat "double-precision floating point number comparison"
- diffPressure: \ref numberComparisonFormat "double-precision floating point number comparison"
- inletCoolantTemp: \ref numberComparisonFormat "double-precision floating point number comparison"
- outletCoolantTemp: \ref numberComparisonFormat "double-precision floating point number comparison"
- dewpointTemp: \ref numberComparisonFormat "double-precision floating point number comparison"
- ambientTemp: \ref numberComparisonFormat "double-precision floating point number comparison"
- ambientHumidity: \ref numberComparisonFormat "double-precision floating point number comparison"
- systemPower: \ref numberComparisonFormat "double-precision floating point number comparison"
- shutoffCause: \ref numberComparisonFormat "integer number comparison" <b>Added in V1R1M1</b>
- sort: Sets the sort order. Format is [&lt;direction&gt;]&lt;field&gt;
  - Fields are:
    - location
    - time
    - inletFlowRate
    - outletFlowRate
    - coolantPressure
    - diffPressure
    - inletCoolantTemp
    - outletCoolantTemp
    - dewpointTemp
    - ambientTemp
    - ambientHumidity
    - systemPower
    - shutoffCause <b>Added in V1R1M1</b>

\section responseFormat JSON response format

<pre>
[
  {
    "location": &quot;<i>location</i>&quot;,
    "time": &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
    "inletFlowRate": <i>number</i>, // gal/min
    "outletFlowRate": <i>number</i>, // gal/min
    "coolantPressure": <i>number</i>, // PSI
    "diffPressure": <i>number</i>, // PSI
    "inletCoolantTemp": <i>number</i>, // degrees C
    "outletCoolantTemp": <i>number</i>, // degrees C
    "dewpointTemp": <i>number</i>, // degrees C
    "ambientTemp": <i>number</i>, // degrees C
    "ambientHumidity": <i>number</i>, // %
    "systemPower": <i>number</i>, // KW
    "shutoffCause": <i>integer</i> // <b>Added in V1R1M1</b>
  },
  ...
]
</pre>

\section Errors

HTTP status: 403 Forbidden
- authority: The user doesn't have hardware READ authority.

 */


#include "Coolant.hpp"

#include "../../Error.hpp"
#include "../../RequestRange.hpp"

#include "../../query/env/Query.hpp"
#include "../../query/env/CoolantOptions.hpp"

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


const capena::http::uri::Path Coolant::RESOURCE_PATH(capena::http::uri::Path() / "environmentals" / "coolant");


void Coolant::_doGet()
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

    boost::shared_ptr<query::env::CoolantOptions> options_ptr( new query::env::CoolantOptions() );

    options_ptr->setArgs( request.getUrl().getQuery().calcArguments(), req_range );

    _query_ptr.reset( new query::env::Query( options_ptr ) );

    _query_ptr->executeAsync(
            _blocking_operations_thread_pool,
            _getStrand().wrap( boost::bind( &Coolant::_queryComplete, this,
                    capena::server::AbstractResponder::shared_from_this(),
                    req_range,
                    _1
                ) )
        );

}


void Coolant::notifyDisconnect()
{
    LOG_DEBUG_MSG( "Notified client disconnected" );

    query::env::Query::Ptr query_ptr(_query_ptr);

    if ( ! _query_ptr )  return;

    _query_ptr->cancel();
}


void Coolant::_queryComplete(
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
                const auto &cols(res.rs_ptr->columns());

                json::Object &obj(arr.addObject());
                obj.set( "location", cols[BGQDB::DBTCoolantenvironment::LOCATION_COL].getString() );
                obj.set( "time", cols[BGQDB::DBTCoolantenvironment::TIME_COL].getTimestamp() );
                obj.set( "inletFlowRate", cols[BGQDB::DBTCoolantenvironment::INLETFLOWRATE_COL].as<int64_t>() / 100.0 );
                obj.set( "outletFlowRate", cols[BGQDB::DBTCoolantenvironment::OUTLETFLOWRATE_COL].as<int64_t>() / 100.0 );
                obj.set( "coolantPressure", cols[BGQDB::DBTCoolantenvironment::COOLANTPRESSURE_COL].as<int64_t>() / 100.0 );
                obj.set( "diffPressure", cols[BGQDB::DBTCoolantenvironment::DIFFPRESSURE_COL].as<int64_t>() / 100.0 );
                obj.set( "inletCoolantTemp", cols[BGQDB::DBTCoolantenvironment::INLETCOOLANTTEMP_COL].as<int64_t>() / 100.0 );
                obj.set( "outletCoolantTemp", cols[BGQDB::DBTCoolantenvironment::OUTLETCOOLANTTEMP_COL].as<int64_t>() / 100.0 );
                obj.set( "dewpointTemp", cols[BGQDB::DBTCoolantenvironment::DEWPOINTTEMP_COL].as<int64_t>() / 100.0 );
                obj.set( "ambientTemp", cols[BGQDB::DBTCoolantenvironment::AMBIENTTEMP_COL].as<int64_t>() / 100.0 );
                obj.set( "ambientHumidity", cols[BGQDB::DBTCoolantenvironment::AMBIENTHUMIDITY_COL].as<int64_t>() / 100.0 );
                obj.set( "systemPower", cols[BGQDB::DBTCoolantenvironment::SYSTEMPOWER_COL].as<int64_t>() / 100.0 );
                obj.set( "shutoffCause", cols[BGQDB::DBTCoolantenvironment::SHUTOFFCAUSE_COL].as<int64_t>() );
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
