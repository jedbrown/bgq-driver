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

/*! \page utilizationSummaryResource /bg/summary/utilization

This resource provides summary information related to system utilization.

- \subpage utilizationSummaryResourceGet

 */

/*! \page utilizationSummaryResourceGet GET /bg/summary/utilization

Get summary information related to system utilization.

\section Authority

The user must be authenticated.

\section utilizationSummaryResourceGetResponse JSON response format

<pre>
[
  {
    "date": &quot;<i>string</i>&quot;, // date string is ISO8601, like yyyymmdd
    "utilization": <i>number</i>
  },
  ... // One for each of the past 5 days.
]
</pre>

\section Errors

HTTP status: 403 Forbidden
- notAuthenticated: The user isn't authenticated.

 */


#include "Utilization.hpp"

#include "../../BlockingOperationsThreadPool.hpp"
#include "../../BlueGene.hpp"
#include "../../dbConnectionPool.hpp"
#include "../../Error.hpp"
#include "../../sqlStrings.gen.hpp"

#include "capena-http/server/exception.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/BGQDBlib.h>

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


Utilization::Utilization(
        CtorArgs& args
    ) :
        AbstractResponder( args ),
        _system_cpu_count(args.blue_gene.getMachineInfo().midplane_count * BGQDB::Nodes_Per_Midplane ),
        _blocking_operations_thread_pool(args.blocking_operations_thread_pool)
{ /* Nothing to do */ }


void Utilization::_doGet()
{

    // The user must be authenticated.
    if ( ! _isUserAuthenticated() ) {
        LOG_WARN_MSG( "Cannot get utilization summary because the user isn't authenticated." );

        BOOST_THROW_EXCEPTION( Error(
                "Cannot get utilization summary because the user isn't authenticated.",
                "getUtilizationSummary", "notAuthenticated", Error::Data(),
                capena::http::Status::Forbidden
            ) );
    }


    _blocking_operations_thread_pool.post( boost::bind(
            &Utilization::_doQuery, this,
            capena::server::AbstractResponder::shared_from_this()
        ) );
}


void Utilization::_doQuery(
        capena::server::ResponderPtr
    )
{
    try {

        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query(
                sql::UTILIZATION_CHART // queries/utilizationChart.txt
            ));

        _getStrand().post( boost::bind(
                &Utilization::_queryComplete, this,
                capena::server::AbstractResponder::shared_from_this(),
                conn_ptr,
                rs_ptr
            ) );

    } catch ( std::exception& e )
    {
        _inCatchPostCurrentExceptionToHandlerFn();
    }
}


void Utilization::_queryComplete(
        capena::server::ResponderPtr,
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    try {

        rs_ptr->fetch();

        const cxxdb::Columns &cols(rs_ptr->columns());

        json::ArrayValue arr_val;
        json::Array &arr(arr_val.get());

        const double SECONDS_IN_A_DAY(86400);
        const double SECONDS_TODAY_DB(cols["seconds4"].as<double>());
        const double SECONDS_TODAY(SECONDS_TODAY_DB == 0 ? 1.0 : SECONDS_TODAY_DB);
            // seconds4 would be 0 at midnight, change it to 1 so don't get 0 seconds (infinite utilization).

        const double NORMAL_DAY_TOTAL_CPU_SECONDS(_system_cpu_count * SECONDS_IN_A_DAY);
        const double TODAY_TOTAL_CPU_SECONDS(_system_cpu_count * SECONDS_TODAY);

        for ( unsigned i(0) ; i < 5 ; ++i ) {
            json::Object &obj(arr.addObject());

            const std::string DAY_COL_NAME(string() + "day" + lexical_cast<string>(i));
            const std::string CPU_SECONDS_COL_NAME(string() + "t" + lexical_cast<string>(i));

            obj.set( "date", boost::gregorian::to_iso_string( cols[DAY_COL_NAME].getDate() ) );

            const double JOB_CPU_SECONDS(cols[CPU_SECONDS_COL_NAME].as<double>());

            // Last one is today, where don't have a full day yet, so get number of seconds so far today from "seconds4" col.
            const double TOTAL_CPU_SECONDS(i == 4 ? TODAY_TOTAL_CPU_SECONDS : NORMAL_DAY_TOTAL_CPU_SECONDS);

            const double UTILIZATION(JOB_CPU_SECONDS / TOTAL_CPU_SECONDS * 100.0);

            obj.set( "utilization", UTILIZATION );
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
