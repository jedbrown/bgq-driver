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


/*! \page rasResource /bg/ras

This resource is for RAS events.

- \subpage rasResourceGet

 */


/*!
 *  \page rasResourceGet GET /bg/ras

Query RAS events.

\note This resource supports \ref partialResponse "partial responses".


\section Authority

The user must have hardware READ authority.


\section rasResourceParameters Query parameters

- block: \ref multiWildcardFormat "multi-wildcard string"
- category: \ref multiWildcardFormat "multi-wildcard string"
- component: \ref multiWildcardFormat "multi-wildcard string"
- controlAction: Comma-separated strings
  - A control action (like COMPUTE_IN_ERROR), or a special value.
  - Special values:
    - any : any control action or no control action, this is the default
    - action : any control action.
- interval: either
  - a \ref timeIntervalFormat "time interval"
  - record ID range, formatted like ###-### -- This is new for V1R1M1.
- job: job ID
- location: \ref multiWildcardFormat "multi-wildcard string"
- message: \ref wildcardFormat "wildcard string"
- msgid: \ref multiWildcardFormat "multi-wildcard string"
- serialnumber
- severity: Only get RAS events with the matching severity
  - A string of characters, where each character is a severity code.
  - Only events with severity in the string are returned.
  - Severity codes and the severity value that each code maps to are:
    - I (INFO)
    - W (WARN)
    - F (FATAL)
    - U (UNKNOWN)
- sort: Sets the sort order. Format is [&lt;direction&gt;]&lt;field&gt;
  - Fields are:
    - id or recid
    - block
    - category
    - component
    - controlAction
    - count
    - eventTime or event_time
    - jobId
    - location
    - message
    - msgId
    - severity
    - serialNumber


\section rasResourceGetResponse JSON response format

<pre>
[
  { "id": <i>number</i>,
    "eventTime": &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
    "msg_id": &quot;<i>string</i>&quot;, // Optional
    "category": &quot;<i>string</i>&quot;, // Optional
    "component": &quot;<i>string</i>&quot;, // Optional
    "severity": &quot;<i>string</i>&quot;, // Optional
    "jobid": <i>number</i>, // Optional
    "block": &quot;<i>string</i>&quot;, // Optional
    "location": &quot;<i>string</i>&quot;, // Optional
    "serialnumber": &quot;<i>string</i>&quot;, // Optional
    "count": <i>number</i>, // Optional
    "controlAction": &quot;<i>string</i>&quot;, // Optional
    "message": &quot;<i>string</i>&quot; // Optional
    "URI": &quot;<i>\ref rasDetailsResource</i>&quot;
  },
  ...
]
</pre>


\section Errors

HTTP status: 403 Forbidden
- authority: The user doesn't have READ hardware authority.

  */


#include "Ras.hpp"

#include "RasDetails.hpp"

#include "../BlockingOperationsThreadPool.hpp"
#include "../dbConnectionPool.hpp"
#include "../Error.hpp"
#include "../RequestRange.hpp"
#include "../sqlStrings.gen.hpp"

#include "../query/Ras.hpp"

#include "common/common.hpp"

#include "capena-http/http/http.hpp"

#include "capena-http/server/exception.hpp"
#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <utility/include/Log.h>

#include <boost/shared_ptr.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>
#include <string>

#include <stdint.h>


using boost::shared_ptr;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {


const capena::http::uri::Path &Ras::RESOURCE_PATH(::bgws::common::resource_path::RAS);
const capena::http::uri::Path Ras::RESOURCE_PATH_EMPTY_CHILD(RESOURCE_PATH / "");


bool Ras::matchesUrl(
        const capena::http::uri::Path& requested_resource
    )
{
    // Check is /bg/ras or /bg/ras/
    return (requested_resource == RESOURCE_PATH || requested_resource == RESOURCE_PATH_EMPTY_CHILD);
}


capena::http::Methods Ras::_getAllowedMethods() const
{
    return { capena::http::Method::GET };
}


void Ras::_doGet()
{
    _checkAuthority();

    const capena::server::Request &request(_getRequest());

    static const unsigned DefaultRangeSize(50), MaxRangeSize(200);
    RequestRange req_range( request, DefaultRangeSize, MaxRangeSize );

    query::RasOptions query_options(
            request.getUrl().getQuery().calcArguments(),
            req_range
        );

    _blocking_operations_thread_pool.post( boost::bind(
            &Ras::_doQuery, this,
            capena::server::AbstractResponder::shared_from_this(),
            req_range,
            query_options
        ) );
}


void Ras::_checkAuthority() const
{
    if ( _userHasHardwareRead() ) {
        return;
    }

    LOG_WARN_MSG( "Could not get ras because " << _getRequestUserInfo() << " doesn't have authority." );

    BOOST_THROW_EXCEPTION( Error(
            "Could not get ras because the user doesn't have authority.",
            "getRas", "authority", Error::Data(),
            capena::http::Status::Forbidden
        ) );
}


void Ras::_doQuery(
        capena::server::ResponderPtr,
        const RequestRange& req_range,
        const query::RasOptions& query_options
    )
{
    try {
        query::Ras ras_query( query_options );

        auto conn_ptr(dbConnectionPool::getConnection());

        uint64_t row_count(0);
        cxxdb::ResultSetPtr rs_ptr;

        ras_query.execute( conn_ptr, &row_count, &rs_ptr );

        _getStrand().post( boost::bind(
                &Ras::_queryComplete, this,
                capena::server::AbstractResponder::shared_from_this(),
                req_range,
                row_count,
                conn_ptr,
                rs_ptr
            ) );
    } catch ( std::exception& e )
    {
        _inCatchPostCurrentExceptionToHandlerFn();
    }
}


void Ras::_queryComplete(
        capena::server::ResponderPtr,
        const RequestRange& req_range,
        uint64_t row_count,
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    try {
        LOG_DEBUG_MSG( "Event log row count=" << row_count );

        capena::server::Response &response(_getResponse());

        json::ArrayValue arr_value;
        json::Array &arr(arr_value.get());

        if ( row_count == 0 ) {

            // Fast-path when no rows match the filter.

            response.setContentTypeJson();
            response.headersComplete();

            json::Formatter()( arr_value, response.out() );
            response.out() << std::endl;

            return;

        }


        while ( rs_ptr->fetch() ) {
            json::Object &js_obj(arr.addObject());

            const cxxdb::Columns &cols(rs_ptr->columns());

            uint64_t recid(cols["recid"].as<int64_t>());

            js_obj.set( "id", recid );
            js_obj.set( "URI", RasDetails::calcPath( _getDynamicConfiguration().getPathBase(), recid ).toString() );
            if ( cols["msg_id"] )  js_obj.set( "msgId", cols["msg_id"].getString() );
            if ( cols["category"] )  js_obj.set( "category", cols["category"].getString() );
            if ( cols["component"] )  js_obj.set( "component", cols["component"].getString() );
            if ( cols["severity"] )  js_obj.set( "severity", cols["severity"].getString() );
            js_obj.set( "eventTime", cols["event_time"].getTimestamp() );
            if ( cols["jobid"] )  js_obj.set( "jobId", cols["jobid"].as<int64_t>() );
            if ( cols["block"] )  js_obj.set( "block", cols["block"].getString() );
            if ( cols["location"] )  js_obj.set( "location", cols["location"].getString() );
            if ( cols["serialnumber"] )  js_obj.set( "serialNumber", cols["serialnumber"].getString() );
            if ( cols["ctlaction"] )  js_obj.set( "controlAction", cols["ctlaction"].getString() );
            if ( cols["count"] )  js_obj.set( "count", cols["count"].as<int64_t>() );
            if ( cols["message"] )  js_obj.set( "message", cols["message"].getString() );
        }


        req_range.updateResponse( response, arr.size(), row_count );

        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( arr_value, response.out() );
        response.out() << std::endl;
    } catch ( std::exception& e ) {
        _handleError( e );
    }
}


} } // namespace bgws::responder
