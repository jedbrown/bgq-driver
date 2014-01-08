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

#include "Alert.hpp"

#include "../dbConnectionPool.hpp"
#include "../Error.hpp"
#include "../MultiWildcardOption.hpp"
#include "../RequestRange.hpp"
#include "../SortInfo.hpp"
#include "../SortSpec.hpp"
#include "../TimeIntervalOption.hpp"
#include "../WhereClause.hpp"

#include "../teal/Teal.hpp"

#include "capena-http/http/http.hpp"

#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <set>
#include <string>

#include <stdint.h>


using boost::lexical_cast;

using std::set;
using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {


const capena::http::uri::Path Alerts::RESOURCE_PATH(capena::http::uri::Path() / "alerts");


void Alerts::doGet()
{
    namespace po = boost::program_options;


    static const SortInfo::IdToCol ID_TO_COL = boost::assign::map_list_of
            ( "recId", "\"rec_id\"" )
            ( "alertId", "\"alert_id\"" )
            ( "created", "\"creation_time\"" )
            ( "severity", "\"severity\"" )
            ( "urgency", "\"urgency\"" )
            ( "eventLocationType", "\"event_loc_type\"" )
            ( "eventLocation", "\"event_loc\"" )
            ( "fruLocation", "\"fru_loc\"" )
            ( "recommendation", "\"recommendation\"" )
            ( "reason", "\"reason\"" )
            ( "source", "\"src_name\"" )
            ( "state", "\"state\"" )
        ;

    static const SortInfo::KeyCols KEY_COLS = { "\"rec_id\"" };
    static const string DEFAULT_COLUMN_NAME( "\"rec_id\"" );
    static const utility::SortDirection::Value DEFAULT_DIRECTION(utility::SortDirection::Descending);

    static const SortInfo sort_info( ID_TO_COL, KEY_COLS, DEFAULT_COLUMN_NAME, DEFAULT_DIRECTION );


    _checkAuthority();

    const auto &request(_getRequest());

    static const unsigned DEFAULT_ITEM_COUNT(20), MAX_COUNT(100);
    RequestRange req_range( request, DEFAULT_ITEM_COUNT, MAX_COUNT );


    string dup_str;
    TimeIntervalOption interval( "interval" );
    MultiWildcardOption location( "location", teal::Teal::LOCATION_DB_COLUMN_SIZE, BGQDB::DBObj::ColumnType::Varchar );
    string location_types_str;
    string severities_str;
    SortSpec sort_spec;
    string states_str;
    string urgencies_str;

    po::options_description desc;
    desc.add_options()
            ( "dup", po::value( &dup_str ) )
            ( "locationType", po::value( &location_types_str ) )
            ( "severity", po::value( &severities_str ) )
            ( "sort", po::value( &sort_spec ) )
            ( "state", po::value( &states_str ) )
            ( "urgency", po::value( &urgencies_str ) )
        ;

    interval.addTo( desc );
    location.addTo( desc );

    po::variables_map vm;
    po::store(
            po::command_line_parser( request.getUrl().getQuery().calcArguments() ).options( desc ).allow_unregistered().run(),
            vm
        );
    po::notify( vm );


    bool dups(false);
    if ( dup_str == "T" )  dups = true;

    set<string> location_types;

    BOOST_FOREACH( char ch, location_types_str ) {
        if ( ch == 'A' || ch == 'J' || ch == 'C' || ch == 'I' ) {
            location_types.insert( lexical_cast<string>( ch ) );
        }
    }

    if ( location_types.size() == 4 )  location_types.clear();


    set<string> states;

    BOOST_FOREACH( char ch, states_str ) {
        try {
            int32_t severity_val(lexical_cast<int32_t>( string() + ch ));
            if ( severity_val == 1 || severity_val == 2 ) {
                states.insert( lexical_cast<string>( severity_val ) );
            }
        } catch ( std::bad_cast& e ) {
            // Ignore.
        }
    }

    if ( states.size() == 2 )  states.clear();

    set<string> severities;

    BOOST_FOREACH( char ch, severities_str ) {
        if ( ch == 'F' || ch == 'E' || ch == 'W' || ch == 'I' ) {
            severities.insert( lexical_cast<string>( ch ) );
        }
    }

    if ( severities.size() == 4 )  severities.clear();

    set<string> urgencies;

    BOOST_FOREACH( char ch, urgencies_str ) {
        if ( ch == 'I' || ch == 'S' || ch == 'N' || ch == 'D' || ch == 'O' ) {
            urgencies.insert( lexical_cast<string>( ch ) );
        }
    }

    if ( urgencies.size() == 5 )  urgencies.clear();


    json::ArrayValue arr_val;
    auto &arr(arr_val.get());

    auto conn_ptr(dbConnectionPool::getConnection());


    WhereClause where_clause;
    cxxdb::ParameterNames param_names;

    if ( ! dups ) {
        where_clause.add( "\"rec_id\" NOT IN ( SELECT \"t_alert_recid\" FROM x_tealAlert2Alert WHERE \"assoc_type\"='D' )" );
    }
    if ( ! location_types.empty() ) {
        where_clause.add( string() + "\"event_loc_type\" IN ( '" + boost::algorithm::join( location_types, "','" ) + "')" );
    }
    if ( ! states.empty() ) {
        where_clause.add( string() + "\"state\" IN (" + boost::algorithm::join( states, "," ) + ")" );
    }
    if ( ! severities.empty() ) {
        where_clause.add( string() + "\"severity\" IN ( '" + boost::algorithm::join( severities, "','" ) + "' )" );
    }
    if ( ! urgencies.empty() ) {
        where_clause.add( string() + "\"urgency\" IN ( '" + boost::algorithm::join( urgencies, "','" ) + "' )" );
    }

    interval.addTo( where_clause, param_names, "\"creation_time\"" );
    location.addTo( where_clause, param_names, "\"event_loc\"" );


    uint64_t total_count(0);

    {
        string sql = string() +

 "SELECT COUNT(*) AS c"
  " FROM x_tealalertlog"
  + where_clause.getString()

            ;

        auto stmt_ptr(conn_ptr->prepareQuery( sql, param_names ));

        interval.bindParameters( stmt_ptr->parameters() );
        location.bindParameters( stmt_ptr->parameters() );

        auto rs_ptr(stmt_ptr->execute());

        if ( rs_ptr->fetch() ) {
            total_count = rs_ptr->columns()["c"].as<uint64_t>();
        }
    }

    if ( total_count > 0 ) {

        string sort_clause_sql(sort_info.calcSortClause( sort_spec ));

        string sql;

        sql +=
"WITH"
            ;

        if ( dups ) {

            sql +=
" dups AS ("

" SELECT \"t_alert_recid\" AS dupRecId, \"alert_recid\" AS duplicateOf"
  " FROM x_tealalert2alert"
  " WHERE \"assoc_type\" = 'D'"

" ),"
                ;

        }

        sql +=
" all_ordered AS ("

" SELECT \"rec_id\", \"alert_id\", \"creation_time\", \"severity\", \"urgency\", \"event_loc_type\", \"event_loc\", \"fru_loc\", \"recommendation\", \"reason\", \"src_name\", \"state\", \"raw_data\","
       " ROW_NUMBER() OVER ( ORDER BY " + sort_clause_sql + " ) AS row_num"
  " FROM x_tealalertlog"
  + where_clause.getString() +

" )"

" SELECT a.\"rec_id\", a.\"alert_id\", a.\"creation_time\", a.\"severity\", a.\"urgency\", a.\"event_loc_type\", a.\"event_loc\", a.\"fru_loc\", a.\"recommendation\", a.\"reason\", a.\"src_name\", a.\"state\", a.\"raw_data\","
            ;

        if ( dups ) {
            sql +=
       " dups.duplicateOf"
                ;
        } else {
            sql +=
       " CAST(NULL AS INTEGER) AS duplicateOf"
                ;
        }

        sql +=
 " FROM all_ordered AS a"
            ;

        if ( dups ) {
            sql +=
      " LEFT OUTER JOIN"
      " dups"
      " ON a.\"rec_id\" = dups.dupRecId"
                ;

        }

        sql +=
 " WHERE row_num BETWEEN ? AND ?"
 " ORDER BY row_num"
            ;

        param_names.push_back( "row_start" );
        param_names.push_back( "row_end" );

        auto stmt_ptr(conn_ptr->prepareQuery( sql, param_names ));

        auto &parameters(stmt_ptr->parameters());
        interval.bindParameters( parameters );
        location.bindParameters( parameters );
        req_range.bindParameters( parameters, "row_start", "row_end" );

        auto rs_ptr(stmt_ptr->execute());

        while ( rs_ptr->fetch() ) {
            const auto &cols(rs_ptr->columns());

            auto &obj(arr.addObject());

            teal::Id rec_id(cols["rec_id"].as<teal::Id>());

            obj.set( "recId", rec_id );
            obj.set( "alertId", cols["alert_id"].getString() );
            obj.set( "created", cols["creation_time"].getTimestamp() );
            obj.set( "severity", cols["severity"].getString() );
            obj.set( "urgency", cols["urgency"].getString() );
            obj.set( "eventLocationType", cols["event_loc_type"].getString() );
            obj.set( "eventLocation", cols["event_loc"].getString() );
            if ( cols["fru_loc"] )  obj.set( "fruLocation", cols["fru_loc"].getString() );
            obj.set( "recommendation", cols["recommendation"].getString() );
            obj.set( "reason", cols["reason"].getString() );
            obj.set( "source", cols["src_name"].getString() );
            if ( cols["state"] )  obj.set( "state", cols["state"].as<teal::State>() );
            if ( cols["raw_data"] )  obj.set( "rawData", cols["raw_data"].getString() );
            if ( cols["duplicateOf"] ) obj.set( "duplicateOf", Alert::calcPath( getDynamicConfiguration().getPathBase(), cols["duplicateOf"].as<teal::Id>() ).toString() );
            obj.set( "URI", Alert::calcPath( getDynamicConfiguration().getPathBase(), rec_id ).toString() );
        }
    }

    auto &response(getResponse());

    req_range.updateResponse(
            response,
            arr.size(),
            total_count
        );

    response.setContentTypeJson();
    response.headersComplete();

    json::Formatter()( arr_val, response.out() );
}


void Alerts::_checkAuthority()
{
    if ( _userHasHardwareRead() ) {
        return;
    }

    LOG_WARN_MSG( "Could not get alerts because " << getRequestUserInfo() << " doesn't have authority." );

    BOOST_THROW_EXCEPTION( Error(
            "Could not get alerts because the user doesn't have authority.",
            "getAlerts", "authority", Error::Data(),
            capena::http::Status::Forbidden
        ) );
}


}} // namespace bgws::responder
