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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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

/*! \page perfResource /bg/perf

This resource provides queries of the control system performance counters.

- \subpage perfResourceGet

 */

/*! \page perfResourceGet GET /bg/perf

Get control system performance counter data.

\note This resource supports \ref partialResponse "partial responses" when returning multiple detail or block collections.

\section Authority

The user must have hardware READ authority.

\section perfResourceGetParameters Query parameters

- block: A block ID.
- detail: performance counter detail value.
- grouping: Grouping of performance data. A single-letter code, either D for daily, W for weekly, or M for monthly. Only applies if block or detail is set.
- interval: a \ref timeIntervalFormat "time interval", defaults to previous week (7 days).

\section perfResourceGetOutput JSON response format

Responses come back in two formats, one when multiple detail or block collections are returned,
and one when a single block or detail is returned.

The following format is used when multiple detail or block collections are returned.
This is the case when neither a block or detail is provided in the query parameters,
or when a block or detail and grouping is provided.

<pre>
[
  {
    "detail" : &quot;<i>detail</i>&quot;, // optional, either detail or group and interval will be returned.

    "group" : &quot;<i>group</i>&quot;, // optional either detail or group and interval will be returned.
    "interval" : &quot;<i>\ref timeIntervalFormat "time interval"</i>&quot;, // optional, either detail or group and interval will be returned.

    "durations" : {
      &quot;<i>function</i>&quot; : <i>number</i>,
      ...
    }
  }, ...
]
</pre>

The following format is used when a single detail or block is returned.
This is the case when a block or detail and not grouping is provided in the query parameters.

<pre>
{
  &quot;<i>function</i>&quot; : <i>number</i>,
  ...
}
</pre>


\section Errors

HTTP status: 403 Forbidden
- authority: The user doesn't have READ hardware authority.

*/

#include "PerformanceMonitoring.hpp"

#include "../BlockingOperationsThreadPool.hpp"
#include "../blue_gene.hpp"
#include "../dbConnectionPool.hpp"
#include "../Error.hpp"
#include "../RequestRange.hpp"
#include "../StringDbColumnOption.hpp"
#include "../WhereClause.hpp"

#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/filtering/TimeInterval.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <utility/include/performance/Mode.h>

#include <boost/bind.hpp>
#include <boost/date_time.hpp>
#include <boost/lexical_cast.hpp>


using BGQDB::DBVComponentperf;

using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {


const std::string PerformanceMonitoring::_FUNCTION_BOOT_MODE_BASIC_SQL(
        DBVComponentperf::FUNCTION_COL + "='" + blue_gene::performance_monitoring::getFunctionBoot() + "' AND"
        " " + DBVComponentperf::MODE_COL + "='" + bgq::utility::performance::Mode::toString( bgq::utility::performance::Mode::Value::Basic ) + "'"
    );


bool PerformanceMonitoring::matchesUrl(
        const capena::http::uri::Path& requested_resource
    )
{
    static const capena::http::uri::Path RESOURCE_PATH(capena::http::uri::Path() / "perf");
    return (requested_resource == RESOURCE_PATH);
}


PerformanceMonitoring::_Grouping PerformanceMonitoring::_parseGroupingCode( const std::string& grouping_code )
{
    if ( grouping_code == string() || grouping_code == "N" )  return _Grouping::None;
    if ( grouping_code == "D" ) return _Grouping::Daily;
    if ( grouping_code == "W" ) return _Grouping::Weekly;
    if ( grouping_code == "M" ) return _Grouping::Monthly;

    LOG_WARN_MSG( "Invalid grouping code, ignoring. The grouping is '" << grouping_code << "'." );

    return _Grouping::None;
}


void PerformanceMonitoring::_doGet()
{
    namespace po = boost::program_options;

    _checkAuthority();

    const capena::server::Request &request(_getRequest());

    static const unsigned DefaultRangeSize(20), MaxRangeSize(100);
    RequestRange req_range( request, DefaultRangeSize, MaxRangeSize );

    static const BGQDB::filtering::TimeInterval DEFAULT_INTERVAL( BGQDB::filtering::Duration( BGQDB::filtering::Duration::Date( 7 ) ) );

    StringDbColumnOption block( "block", DBVComponentperf::ID_SIZE );
    StringDbColumnOption detail( "detail", DBVComponentperf::DETAIL_SIZE );
    string grouping_code;

    TimeIntervalOption interval( "interval", DEFAULT_INTERVAL );

    po::options_description desc;

    desc.add_options()
            ( "grouping", po::value( &grouping_code ) )
        ;

    block.addTo( desc );
    detail.addTo( desc );

    interval.addTo( desc );

    po::variables_map vm;
    po::store(
            po::command_line_parser( request.getUrl().getQuery().calcArguments() ).options( desc ).allow_unregistered().run(),
            vm
        );
    po::notify( vm );


    _Grouping grouping(_parseGroupingCode( grouping_code ));


    if ( (block.hasValue() || detail.hasValue()) && grouping != _Grouping::None ) {

        _blocking_operations_thread_pool.post( boost::bind(
                &PerformanceMonitoring::_getGroupedQuery, this,
                capena::server::AbstractResponder::shared_from_this(),
                req_range,
                interval,
                detail.hasValue() ? detail.getValue() : string(),
                block.hasValue() ? block.getValue() : string(),
                grouping
            ) );

        return;

    }

    if ( block.hasValue() || detail.hasValue() ) {

        _blocking_operations_thread_pool.post( boost::bind(
                &PerformanceMonitoring::_getSingleQuery, this,
                capena::server::AbstractResponder::shared_from_this(),
                interval,
                detail.hasValue() ? detail.getValue() : string(),
                block.hasValue() ? block.getValue() : string()
            ) );

        return;

    }

    _blocking_operations_thread_pool.post( boost::bind(
            &PerformanceMonitoring::_getMultiDetailsQuery, this,
            capena::server::AbstractResponder::shared_from_this(),
            req_range,
            interval
        ) );
}


void PerformanceMonitoring::notifyDisconnect()
{
    cxxdb::QueryStatementPtr stmt_ptr(_stmt_ptr);

    if ( ! stmt_ptr ) {
        LOG_DEBUG_MSG( "Notified client disconnected, no current query statement." );
        return;
    }

    LOG_INFO_MSG( "Notified client disconnected, canceling current query statement." );

    stmt_ptr->cancel();
}


void PerformanceMonitoring::_checkAuthority()
{
    if ( _userHasHardwareRead() ) {
        return;
    }

    LOG_WARN_MSG( "Could not get control system performance data because " << _getRequestUserInfo() << " doesn't have authority." );

    BOOST_THROW_EXCEPTION( Error(
            "Could not get control system performance data because the user doesn't have authority.",
            "getPerf", "authority", Error::Data(),
            capena::http::Status::Forbidden
        ) );
}


void PerformanceMonitoring::_getSingleQuery(
        capena::server::ResponderPtr,
        const TimeIntervalOption& interval,
        const std::string& detail,
        const std::string& block
    )
{
    try {
        auto conn_ptr(dbConnectionPool::getConnection());

        WhereClause where_clause;
        cxxdb::ParameterNames parameter_names;

        where_clause.add( _FUNCTION_BOOT_MODE_BASIC_SQL );

        if ( detail != string() ) {
            where_clause.add( DBVComponentperf::DETAIL_COL + " = ?" );
            parameter_names.push_back( "detail" );
        }
        if ( block != string() ) {
            where_clause.add( DBVComponentperf::ID_COL + " = ?" );
            parameter_names.push_back( "id" );
        }

        interval.addTo( where_clause, parameter_names, DBVComponentperf::ENTRYDATE_COL );

        const string sql(string() +

     "SELECT " + DBVComponentperf::COMPONENT_COL + ", " + DBVComponentperf::SUBFUNCTION_COL + ", AVG(" + DBVComponentperf::DURATION_COL + ") AS duration"
      " FROM " + DBVComponentperf().getTableName() +
      where_clause.getString() +
      " GROUP BY " + DBVComponentperf::COMPONENT_COL + ", " + DBVComponentperf::SUBFUNCTION_COL

            );

        _stmt_ptr = conn_ptr->createQuery();

        LOG_DEBUG_MSG( "Preparing " << sql );

        _stmt_ptr->prepare( sql, parameter_names );

        if ( detail != string() )  _stmt_ptr->parameters()["detail"].set( detail );
        if ( block != string() )  _stmt_ptr->parameters()["id"].set( block );
        interval.bindParameters( _stmt_ptr->parameters() );

        cxxdb::ResultSetPtr rs_ptr(_stmt_ptr->execute());

        _getStrand().post( boost::bind(
                &PerformanceMonitoring::_getSingleQueryComplete, this,
                capena::server::AbstractResponder::shared_from_this(),
                conn_ptr,
                rs_ptr
            ) );

    } catch ( std::exception& e ) {

        _inCatchPostCurrentExceptionToHandlerFn();

    }

}


void PerformanceMonitoring::_getSingleQueryComplete(
        capena::server::ResponderPtr,
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    try {

        json::ObjectValue obj_val;
        json::Object &obj(obj_val.get());

        while ( rs_ptr->fetch() ) {
            const cxxdb::Columns &cols(rs_ptr->columns());

            string function(cols[DBVComponentperf::COMPONENT_COL].getString() + "/" + cols[DBVComponentperf::SUBFUNCTION_COL].getString());

            obj.set( function, cols["duration"].as<double>() );
        }

        rs_ptr.reset();
        _stmt_ptr.reset();

        capena::server::Response &response(_getResponse());
        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( obj_val, response.out() );

    } catch ( std::exception& e ) {

        _handleError( e );

    }
}


void PerformanceMonitoring::_getMultiDetailsQuery(
        capena::server::ResponderPtr,
        const RequestRange& req_range,
        const TimeIntervalOption& interval
    )
{
    try {

        WhereClause where_clause;
        cxxdb::ParameterNames parameter_names;

        where_clause.add(_FUNCTION_BOOT_MODE_BASIC_SQL );

        interval.addTo( where_clause, parameter_names, DBVComponentperf::ENTRYDATE_COL );


        auto conn_ptr(dbConnectionPool::getConnection());

        unsigned total_count(0);

        {
            const string detail_count_sql(string() +

 "WITH ds AS ("

" SELECT " + DBVComponentperf::DETAIL_COL +
  " FROM " + DBVComponentperf().getTableName() +
  where_clause.getString() +
  " GROUP BY " + DBVComponentperf::DETAIL_COL +

" )"

" SELECT COUNT(*) AS c"
  " FROM ds"

            );

            LOG_DEBUG_MSG( "Preparing " + detail_count_sql );

            _stmt_ptr = conn_ptr->createQuery();
            _stmt_ptr->prepare( detail_count_sql, parameter_names );

            interval.bindParameters( _stmt_ptr->parameters() );

            cxxdb::ResultSetPtr rs_ptr(_stmt_ptr->execute());

            if ( rs_ptr->fetch() ) {
                total_count = rs_ptr->columns()["c"].as<unsigned>();
            }

        }

        _stmt_ptr.reset();

        LOG_DEBUG_MSG( "total_count=" << total_count );

        if ( total_count == 0 ) {
            _postEmptyResult();
            return;
        }


        const string detail_list_sql(string() +

 "WITH all_d AS ("

" SELECT " + DBVComponentperf::DETAIL_COL +
  " FROM " + DBVComponentperf().getTableName() +
  where_clause.getString() +
  " GROUP BY " + DBVComponentperf::DETAIL_COL +

" ),"

" d_c_i AS ("

" SELECT " + DBVComponentperf::DETAIL_COL + ","
       " INTEGER(SUBSTR(" + DBVComponentperf::DETAIL_COL + ",2,POSITION('i', " + DBVComponentperf::DETAIL_COL + ", OCTETS) - 2)) AS c,"
       " INTEGER(SUBSTR(" + DBVComponentperf::DETAIL_COL + ",POSITION('i', " + DBVComponentperf::DETAIL_COL + ", OCTETS)+1)) AS i"
  " FROM all_d"

" ),"

" d_r AS ("

" SELECT " + DBVComponentperf::DETAIL_COL + ", ROW_NUMBER() OVER ( ORDER BY c, i ) AS row_num"
  " FROM d_c_i"

" )"

" SELECT " + DBVComponentperf::DETAIL_COL +
  " FROM d_r"
  " WHERE row_num BETWEEN ? AND ?"
  " ORDER BY row_num"

        );

        cxxdb::ParameterNames parameter_names_range(parameter_names);
        parameter_names_range.push_back( "rangeStart" );
        parameter_names_range.push_back( "rangeEnd" );

        LOG_DEBUG_MSG( "Preparing " << detail_list_sql );

        _stmt_ptr = conn_ptr->createQuery();
        _stmt_ptr->prepare( detail_list_sql, parameter_names_range );

        interval.bindParameters( _stmt_ptr->parameters() );

        req_range.bindParameters( _stmt_ptr->parameters(), "rangeStart", "rangeEnd" );

        cxxdb::ResultSetPtr rs_ptr(_stmt_ptr->execute());


        where_clause.add( DBVComponentperf::DETAIL_COL + "=?" );
        parameter_names.push_back( "detail" );

        const string comps_sql(

 "SELECT " + DBVComponentperf::COMPONENT_COL + ", " + DBVComponentperf::SUBFUNCTION_COL + ", AVG(" + DBVComponentperf::DURATION_COL + ") AS duration"
  " FROM " + DBVComponentperf().getTableName() +
  where_clause.getString() +
  " GROUP BY " + DBVComponentperf::COMPONENT_COL + ", " + DBVComponentperf::SUBFUNCTION_COL

            );

        LOG_DEBUG_MSG( "Preparing " << comps_sql );

        auto comps_stmt_ptr(conn_ptr->prepareQuery( comps_sql, parameter_names ));

        interval.bindParameters( comps_stmt_ptr->parameters() );


        _getStrand().post( boost::bind(
                &PerformanceMonitoring::_getMultiDetailsQueryComplete, this,
                capena::server::AbstractResponder::shared_from_this(),
                conn_ptr,
                rs_ptr,
                comps_stmt_ptr,
                req_range,
                total_count
            ) );

    } catch ( std::exception& e ) {

        _inCatchPostCurrentExceptionToHandlerFn();

    }
}


void PerformanceMonitoring::_getMultiDetailsQueryComplete(
        capena::server::ResponderPtr,
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr,
        cxxdb::QueryStatementPtr comps_stmt_ptr,
        const RequestRange& req_range,
        unsigned total_count
    )
{
    try {

        json::ArrayValue arr_val;
        json::Array &arr(arr_val.get());

        while ( rs_ptr->fetch() ) {

            const string detail(rs_ptr->columns()["detail"].getString());

            json::Object &detail_obj(arr.addObject());

            detail_obj.set( "detail", detail );

            json::Object &durations_obj(detail_obj.createObject( "durations" ));

            comps_stmt_ptr->parameters()["detail"].set( detail );

            cxxdb::ResultSetPtr comps_rs_ptr(comps_stmt_ptr->execute());

            while ( comps_rs_ptr->fetch() ) {
                const cxxdb::Columns &cols(comps_rs_ptr->columns());

                string function(cols[DBVComponentperf::COMPONENT_COL].getString() + "/" + cols[DBVComponentperf::SUBFUNCTION_COL].getString());

                durations_obj.set( function, cols["duration"].as<double>() );
            }
        }

        rs_ptr.reset();
        _stmt_ptr.reset();

        capena::server::Response &response(_getResponse());

        req_range.updateResponse( response, arr.size(), total_count );

        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( arr_val, response.out() );

    } catch ( std::exception& e )
    {
        _handleError( e );
    }
}


void PerformanceMonitoring::_getGroupedQuery(
        capena::server::ResponderPtr,
        const RequestRange& req_range,
        const TimeIntervalOption& interval,
        const std::string& detail,
        const std::string& block,
        _Grouping grouping
    )
{
    try {

        WhereClause where_clause;
        cxxdb::ParameterNames parameter_names;

        where_clause.add( _FUNCTION_BOOT_MODE_BASIC_SQL );

        if ( detail != string() ) {
            where_clause.add( DBVComponentperf::DETAIL_COL + " = ?" );
            parameter_names.push_back( "detail" );
        }
        if ( block != string() ) {
            where_clause.add( DBVComponentperf::ID_COL + " = ?" );
            parameter_names.push_back( "id" );
        }

        interval.addTo( where_clause, parameter_names, DBVComponentperf::ENTRYDATE_COL );


        auto conn_ptr(dbConnectionPool::getConnection());

        unsigned total_count(0);


        string g1_fn;
        string g2_fn;

        if ( grouping == _Grouping::Daily ) {
            g1_fn = "MONTH";
            g2_fn = "DAY";
        } else if ( grouping == _Grouping::Weekly ) {
            g1_fn = "YEAR";
            g2_fn = "WEEK";
        } else if ( grouping == _Grouping::Monthly ) {
            g1_fn = "YEAR";
            g2_fn = "MONTH";
        }

        {
            const string group_count_sql = string() +

 "WITH gs AS ("

" SELECT " + g1_fn + "(" + DBVComponentperf::ENTRYDATE_COL + ") AS g1, " + g2_fn + "(" + DBVComponentperf::ENTRYDATE_COL + ") AS g2"
  " FROM " + DBVComponentperf().getTableName() +
    where_clause.getString() +
  " GROUP BY " + g1_fn + "(" + DBVComponentperf::ENTRYDATE_COL + "), " + g2_fn + "(" + DBVComponentperf::ENTRYDATE_COL + ")"

" )"

" SELECT COUNT(*) AS c"
  " FROM gs"

                ;

            LOG_DEBUG_MSG( "Preparing " + group_count_sql );

            _stmt_ptr = conn_ptr->createQuery();
            _stmt_ptr->prepare( group_count_sql, parameter_names );

            if ( detail != string() )  _stmt_ptr->parameters()["detail"].set( detail );
            if ( block != string() )  _stmt_ptr->parameters()["id"].set( block );
            interval.bindParameters( _stmt_ptr->parameters() );

            cxxdb::ResultSetPtr rs_ptr(_stmt_ptr->execute());

            if ( rs_ptr->fetch() ) {
                total_count = rs_ptr->columns()["c"].as<unsigned>();
            }
        }

        _stmt_ptr.reset();

        LOG_DEBUG_MSG( "total_count=" << total_count );

        if ( total_count == 0 ) {
            _postEmptyResult();
            return;
        }


        const string group_list_sql(string() +

 "WITH all_g AS ("

" SELECT " + g1_fn + "(" + DBVComponentperf::ENTRYDATE_COL + ") AS g1, " + g2_fn + "(" + DBVComponentperf::ENTRYDATE_COL + ") AS g2"
  " FROM " + DBVComponentperf().getTableName() +
    where_clause.getString() +
  " GROUP BY " + g1_fn + "(" + DBVComponentperf::ENTRYDATE_COL + "), " + g2_fn + "(" + DBVComponentperf::ENTRYDATE_COL + ")"

" ),"

" g_r AS ("

" SELECT g1, g2, ROW_NUMBER() OVER ( ORDER BY g1, g2 ) AS row_num"
  " FROM all_g"

" )"

" SELECT g1, g2"
  " FROM g_r"
  " WHERE row_num BETWEEN ? AND ?"
  " ORDER BY row_num"

            );

        parameter_names.push_back( "rangeStart" );
        parameter_names.push_back( "rangeEnd" );

        LOG_DEBUG_MSG( "Preparing " + group_list_sql );

        _stmt_ptr = conn_ptr->createQuery();
        _stmt_ptr->prepare( group_list_sql, parameter_names );

        if ( detail != string() )  _stmt_ptr->parameters()["detail"].set( detail );
        if ( block != string() )  _stmt_ptr->parameters()["id"].set( block );
        interval.bindParameters( _stmt_ptr->parameters() );

        req_range.bindParameters( _stmt_ptr->parameters(), "rangeStart", "rangeEnd" );

        cxxdb::ResultSetPtr groups_rs_ptr(_stmt_ptr->execute());

        WhereClause comps_where_clause;
        cxxdb::ParameterNames comps_param_names;

        if ( detail != string() ) {
            comps_where_clause.add( DBVComponentperf::DETAIL_COL + " = ?" );
            comps_param_names.push_back( "detail" );
        }
        if ( block != string() ) {
            comps_where_clause.add( DBVComponentperf::ID_COL + " = ?" );
            comps_param_names.push_back( "id" );
        }
        comps_where_clause.add( g1_fn + "(" + DBVComponentperf::ENTRYDATE_COL + ") = ? AND " + g2_fn + "(" + DBVComponentperf::ENTRYDATE_COL + ") = ?" );
        comps_param_names.push_back( "g1" );
        comps_param_names.push_back( "g2" );


        const string comps_sql(

 "SELECT " + DBVComponentperf::COMPONENT_COL + ", " + DBVComponentperf::SUBFUNCTION_COL + ", AVG(" + DBVComponentperf::DURATION_COL + ") AS duration"
  " FROM " + DBVComponentperf().getTableName() +
    comps_where_clause.getString() +
  " GROUP BY " + DBVComponentperf::COMPONENT_COL + ", " + DBVComponentperf::SUBFUNCTION_COL

            );

        LOG_DEBUG_MSG( "Preparing " + comps_sql );

        auto comps_stmt_ptr(conn_ptr->prepareQuery( comps_sql, comps_param_names ));

        if ( detail != string() )  comps_stmt_ptr->parameters()["detail"].set( detail );
        if ( block != string() )  comps_stmt_ptr->parameters()["id"].set( block );

        _getStrand().post( boost::bind(
                &PerformanceMonitoring::_getGroupedQueryComplete, this,
                capena::server::AbstractResponder::shared_from_this(),
                req_range,
                grouping,
                conn_ptr,
                groups_rs_ptr,
                comps_stmt_ptr,
                total_count
            ) );

    } catch ( std::exception& e )
    {

        _inCatchPostCurrentExceptionToHandlerFn();

    }
}


void PerformanceMonitoring::_getGroupedQueryComplete(
        capena::server::ResponderPtr,
        const RequestRange& req_range,
        _Grouping grouping,
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr groups_rs_ptr,
        cxxdb::QueryStatementPtr comps_stmt_ptr,
        unsigned total_count
    )
{
    try {

        json::ArrayValue arr_val;
        json::Array &arr(arr_val.get());


        const boost::gregorian::first_day_of_the_week_in_month fdm( boost::gregorian::Sunday, boost::gregorian::Jan );

        while ( groups_rs_ptr->fetch() ) {

            const int g1(groups_rs_ptr->columns()["g1"].as<int>());
            const int g2(groups_rs_ptr->columns()["g2"].as<int>());

            json::Object &group_obj(arr.addObject());

            group_obj.set( "group", lexical_cast<string>(g1) + "/" + lexical_cast<string>(g2) );

            // Figure out the interval.

            boost::gregorian::date interval_start_date;
            boost::gregorian::date interval_end_date;

            if ( grouping == _Grouping::Daily ) {
                boost::gregorian::date current_day(boost::gregorian::day_clock::local_day());

                interval_start_date = boost::gregorian::date( current_day.year(), g1, g2 );

                if ( interval_start_date > current_day ) {
                    interval_start_date = boost::gregorian::date( current_day.year() - 1, g1, g2 );
                }

                interval_end_date = interval_start_date + boost::gregorian::days( 1 );

            } else if ( grouping == _Grouping::Weekly ) {

                const boost::gregorian::date first_day_of_first_week(fdm.get_date( g1 ));

                int wk_off = (first_day_of_first_week.week_number() > 3 ? 1 : first_day_of_first_week.week_number()) + 1;

                interval_start_date = first_day_of_first_week + boost::gregorian::weeks( g2 - wk_off );
                interval_end_date = interval_start_date + boost::gregorian::days( 7 );

            } else { // monthly

                interval_start_date = boost::gregorian::date( g1, g2, 1 );
                interval_end_date = interval_start_date + boost::gregorian::months( 1 );

            }

            boost::posix_time::ptime interval_start( interval_start_date ), interval_end( interval_end_date );

            BGQDB::filtering::TimeInterval interval( interval_start, interval_end );

            string interval_str(lexical_cast<string>( interval ));

            group_obj.set( "interval", interval_str );

            json::Object &durations_obj(group_obj.createObject( "durations" ));

            comps_stmt_ptr->parameters()["g1"].cast( g1 );
            comps_stmt_ptr->parameters()["g2"].cast( g2 );

            cxxdb::ResultSetPtr comps_rs_ptr(comps_stmt_ptr->execute());

            while ( comps_rs_ptr->fetch() ) {
                const cxxdb::Columns &cols(comps_rs_ptr->columns());

                string function(cols[DBVComponentperf::COMPONENT_COL].getString() + "/" + cols[DBVComponentperf::SUBFUNCTION_COL].getString());

                durations_obj.set( function, cols["duration"].as<double>() );
            }
        }

        _stmt_ptr.reset();

        capena::server::Response &response(_getResponse());

        req_range.updateResponse( response, arr.size(), total_count );

        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( arr_val, response.out() );

    } catch ( std::exception& e ) {

        _handleError( e );

    }
}


}} // namespace bgws::responder
