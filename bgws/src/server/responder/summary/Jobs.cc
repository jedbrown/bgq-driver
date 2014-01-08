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

/*! \page jobsSummaryResource /bg/summary/jobs

This resource provides summary information related to jobs.

- \subpage jobsSummaryResourceGet

 */

/*! \page jobsSummaryResourceGet GET /bg/summary/jobs

Get summary information related to jobs.

\section Authority

The user must be authenticated.

\section jobsSummaryResourceGetResponse JSON response format

<pre>
[
  {
    "date": &quot;<i>string</i>&quot;, // date string is ISO8601, like yyyymmdd
    "jobCount": <i>number</i>
  },
  ... // One for each of the past 5 days.
]
</pre>

\section Errors

HTTP status: 403 Forbidden
- notAuthenticated: The user isn't authenticated.

 */


#include "Jobs.hpp"

#include "../../BlockingOperationsThreadPool.hpp"
#include "../../dbConnectionPool.hpp"
#include "../../Error.hpp"
#include "../../sqlStrings.gen.hpp"

#include "capena-http/server/exception.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <string>


using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {
namespace summary {


const capena::http::uri::Path Jobs::RESOURCE_PATH(capena::http::uri::Path() / "summary" / "jobs");


bool Jobs::matchesUrl(
        const capena::http::uri::Path& requested_resource
    )
{
    // Check is /bg/summary/jobs

    return (requested_resource == RESOURCE_PATH );
}


capena::http::Methods Jobs::_getAllowedMethods() const
{
    return { capena::http::Method::GET };
}


void Jobs::_doGet()
{
    // The user must be authenticated.
    if ( ! _isUserAuthenticated() ) {
        LOG_WARN_MSG( "Cannot get jobs summary because the user isn't authenticated." );

        BOOST_THROW_EXCEPTION( Error(
                "Cannot get jobs summary because the user isn't authenticated.",
                "getJobsSummary", "notAuthenticated", Error::Data(),
                capena::http::Status::Forbidden
            ) );
    }

    _blocking_operations_thread_pool.post( boost::bind(
            &Jobs::_doQuery, this,
            capena::server::AbstractResponder::shared_from_this()
        ) );
}


void Jobs::_doQuery(
        capena::server::ResponderPtr
    )
{
    try {
        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query(
                sql::JOBS_CHART // queries/jobsChart.txt
            ));

        _getStrand().post( boost::bind(
                &Jobs::_queryComplete, this,
                capena::server::AbstractResponder::shared_from_this(),
                conn_ptr,
                rs_ptr
            ) );
    } catch ( std::exception& e )
    {
        _inCatchPostCurrentExceptionToHandlerFn();
    }
}


void Jobs::_queryComplete(
        capena::server::ResponderPtr,
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    try {
        if ( ! rs_ptr->fetch() ) {
            BOOST_THROW_EXCEPTION( std::runtime_error( "no result for jobs chart query." ) );
        }

        json::ArrayValue arr_val;
        json::Array &arr(arr_val.get());

        for ( unsigned i(1) ; i <= 5 ; ++i ) {
            json::Object &obj(arr.addObject());

            obj.set( "date", boost::gregorian::to_iso_string( rs_ptr->columns()[string() + "d" + lexical_cast<string>(i)].getDate() ) );
            obj.set( "jobCount", rs_ptr->columns()[string() + "j" + lexical_cast<string>(i)].as<uint64_t>() );
        }

        capena::server::Response &response(_getResponse());

        response.setContentTypeJson();
        response.headersComplete();
        json::Formatter()( arr_val, response.out() );
    } catch ( std::exception& e )
    {
        _handleError( e );
    }
}


} } } // namespace bgws::responder::summary
