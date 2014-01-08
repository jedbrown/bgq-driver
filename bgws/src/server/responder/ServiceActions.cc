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

/*! \page serviceActionsResource /bg/serviceActions

This resource provides operations on service actions.

- \subpage serviceActionsResourceGet
- \subpage serviceActionsResourcePost

 */


/*! \page serviceActionsResourceGet GET /bg/serviceActions

Get a summary of service actions.

\note This resource supports \ref partialResponse "partial responses".

\section Authority

The user must have hardware READ authority.

\section serviceActionsResourceGetParameters Query parameters

- action: Only get service actions with the matching action.
  - A string of characters, where each character is an action code.
  - Only service actions with action in the string are returned.
  - Action codes and the action value that each code maps to are:
    - O (OPEN)
    - P (PREPARE)
    - E (END)
    - C (CLOSED)
- endTime: a \ref timeIntervalFormat "time interval"
- endUser: a string.
- location: \ref multiWildcardFormat "multi-wildcard string"
- prepareUser: a string.
- status: Only get service actions with the matchine status.
  - A string of characters, where each character is an status code.
  - Only service actions with status in the string are returned.
  - Status codes and the status value that each code maps to are:
    - O (Open)
    - A (Active)
    - P (Prepared)
    - C (Closed)
    - F (Forced)
    - E (Error)
- prepareTime: a \ref timeIntervalFormat "time interval"
- sort: Sets the sort order. Format is [&lt;direction&gt;]&lt;field&gt;
  - Fields are:
    - id
    - location
    - action
    - info
    - prepareUser
    - endUser
    - status
    - prepareTime
    - endTime
    - prepareLog
    - endLog

\section serviceActionsResourceGetResponse JSON response format

<pre>
[
  {
    "id": <i>number</i>,
    "URI": &quot;\ref serviceActionResource&quot;
    "location": &quot;<i>hardware-location</i>&quot;,
    "action": &quot;<i>string</i>&quot;,  // optional. Values are OPEN, PREPARE, CLOSED, END.
    "info": &quot;<i>string</i>&quot;,  // optional
    "prepareUser": &quot;<i>string</i>&quot;,
    "endUser": &quot;<i>string</i>&quot;,  // optional
    "status": &quot;<i>status-code</i>&quot;, // Values are O = Open, A = Active, P = Prepared, C = Closed, F = Forced, E = Error
    "prepareTime": &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
    "endTime": &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,  // optional
    "prepareLog": &quot;<i>string</i>&quot;,  // optional
    "endLog": &quot;<i>string</i>&quot;,  // optional
    "attentionMessages": &quot;<i>string</i>&quot;  // optional -- new for V1R2M0
  },
  ...
]
</pre>

\section Errors

HTTP status: 403 Forbidden
- authority: The user doesn't have hardware READ authority.

 */


/*! \page serviceActionsResourcePost POST /bg/serviceActions

Run a prepare service action.
Refer to documentation about Blue Gene service actions before using this.

\section Authority

The user must be an administrator to run a prepare service action.

\section serviceActionsResourcePostInput JSON request data format

<pre>
{
  "location" : &quot;<i>location</i>&quot;
}
</pre>

- where <i>location</i> is any location accepted by the service action command:
  - compute or I/O rack
  - midplane
  - node board
  - all node boards on a midplane
  - DCA on node board
  - I/O board on compute or I/O rack
  - power module on compute or I/O rack
  - clock card on compute or I/O rack


\section Response

HTTP status: 201 Created
- Location header is the URL of the new service action resource.

\section Errors

HTTP status: 400 Bad Request
- noLocation: No location was provided in the posted data.
- invalidLocation: The location is not valid for service actions.
- hardwareDoesntExist: The location is not valid because there is no hardware at the location.
- conflict: Another service action is already in progress on a conflicting location.

HTTP status: 403 Forbidden
- authority: The user doesn't have authority to start a service action.

*/


#include "ServiceActions.hpp"

#include "ServiceAction.hpp"

#include "../blue_gene.hpp"
#include "../dbConnectionPool.hpp"
#include "../Error.hpp"
#include "../MultiWildcardOption.hpp"
#include "../ras.hpp"
#include "../SortInfo.hpp"
#include "../StringDbColumnOption.hpp"
#include "../TimeIntervalOption.hpp"

#include "../blue_gene/service_actions/errors.hpp"
#include "../blue_gene/service_actions/ServiceActions.hpp"

#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <set>
#include <string>


using BGQDB::DBTServiceaction;

using boost::lexical_cast;

using std::set;
using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {


const capena::http::uri::Path ServiceActions::RESOURCE_PATH(capena::http::uri::Path() / "serviceActions");
const capena::http::uri::Path ServiceActions::RESOURCE_PATH_SLASH_EMPTY(RESOURCE_PATH / "");


void ServiceActions::setCommonFields(
        json::Object& obj,
        const cxxdb::Columns& cols
    )
{
    obj.set( "id", cols[DBTServiceaction::ID_COL].as<uint64_t>() );
    obj.set( "location", cols[DBTServiceaction::LOCATION_COL].getString() );
    if ( cols[DBTServiceaction::SERVICEACTION_COL] )  obj.set( "action", cols[DBTServiceaction::SERVICEACTION_COL].getString() );
    if ( cols[DBTServiceaction::INFOSERVICEACTION_COL] )  obj.set( "info", cols[DBTServiceaction::INFOSERVICEACTION_COL] .getString() );
    obj.set( "prepareUser", cols[DBTServiceaction::USERNAMEPREPAREFORSERVICE_COL].getString() );
    if ( cols[DBTServiceaction::USERNAMEENDSERVICEACTION_COL] )  obj.set( "endUser", cols[DBTServiceaction::USERNAMEENDSERVICEACTION_COL] .getString() );
    obj.set( "status", cols[DBTServiceaction::STATUS_COL].getString() );
    obj.set( "prepareTime", cols[DBTServiceaction::TSPREPAREFORSERVICE_COL].getTimestamp() );
    if ( cols[DBTServiceaction::TSENDSERVICEACTION_COL] )  obj.set( "endTime", cols[DBTServiceaction::TSENDSERVICEACTION_COL].getTimestamp() );
    if ( cols[DBTServiceaction::LOGFILENAMEPREPAREFORSERVICE_COL] )  obj.set( "prepareLog", cols[DBTServiceaction::LOGFILENAMEPREPAREFORSERVICE_COL].getString() );
    if ( cols[DBTServiceaction::LOGFILENAMEENDSERVICEACTION_COL] )  obj.set( "endLog", cols[DBTServiceaction::LOGFILENAMEENDSERVICEACTION_COL].getString() );
}


void ServiceActions::_doGet()
{

    if ( ! _userHasHardwareRead() ) {

        LOG_WARN_MSG( "Could not get service actions because " << _getRequestUserInfo() << " doesn't have authority." );

        BOOST_THROW_EXCEPTION( Error(
                "Could not get service actions because the user doesn't have authority.",
                "getServiceActions", "authority", Error::Data(),
                capena::http::Status::Forbidden
            ) );

    }


    // Request a snapshot of all the attention messages.
    _service_actions.getAttentionMessagesSnapshot(
            _getStrand().wrap( boost::bind(
                    &ServiceActions::_gotAttentionMessagesSnapshot, this,
                    capena::server::AbstractResponder::shared_from_this(),
                    _1
                ) )
        );

}


void ServiceActions::_doPost( json::ConstValuePtr val_ptr )
{
    if ( ! _isUserAdministrator() ) {
        LOG_WARN_MSG( "Could not start service action because " << _getRequestUserInfo() << " doesn't have authority." );

        ras::postAdminAuthorityFailure( _getRequestUserInfo(), "start service action" );

        BOOST_THROW_EXCEPTION( Error(
                "Could not start service action because the user doesn't have authority.",
                "startServiceAction", "authority", Error::Data(),
                capena::http::Status::Forbidden
            ) );

        return;
    }


    LOG_DEBUG_MSG( "start service action with data: \n" << json::Formatter()( *val_ptr ) );

    const json::Object &obj(val_ptr->getObject());

    if ( ! obj.contains( "location" ) || ! obj.isString( "location" ) ) {

        BOOST_THROW_EXCEPTION( Error(
                "Invalid data starting service action, no location provided.",
                "startServiceAction", "noLocation", Error::Data(),
                capena::http::Status::BadRequest
            ) );

    }

    const string &location(obj.getString( "location" ));

    Error::Data error_data;
    error_data["location"] = location;

    _service_actions.start(
            location,
            getRequestUserName(),
            _getStrand().wrap( boost::bind( &ServiceActions::_started, this, capena::server::AbstractResponder::shared_from_this(), location, _1, _2 ) )
        );
}


void ServiceActions::_query(
        cxxdb::Connection& conn,
        const RequestRange& req_range,
        uint64_t* row_count_out,
        cxxdb::ResultSetPtr* rs_ptr_out
    )
{
    namespace po = boost::program_options;


    static const SortInfo::IdToCol ID_TO_COL = boost::assign::map_list_of
            ( "id", DBTServiceaction::ID_COL )
            ( "location", DBTServiceaction::LOCATION_COL )
            ( "action", DBTServiceaction::SERVICEACTION_COL )
            ( "info", DBTServiceaction::INFOSERVICEACTION_COL )
            ( "prepareUser", DBTServiceaction::USERNAMEPREPAREFORSERVICE_COL )
            ( "endUser", DBTServiceaction::USERNAMEENDSERVICEACTION_COL )
            ( "status", DBTServiceaction::STATUS_COL )
            ( "prepareTime", DBTServiceaction::TSPREPAREFORSERVICE_COL )
            ( "endTime", DBTServiceaction::TSENDSERVICEACTION_COL )
            ( "prepareLog", DBTServiceaction::LOGFILENAMEPREPAREFORSERVICE_COL )
            ( "endLog", DBTServiceaction::LOGFILENAMEENDSERVICEACTION_COL )
        ;

    static const SortInfo::KeyCols KEY_COLS = boost::assign::list_of( DBTServiceaction::ID_COL );
    static const string DEFAULT_COLUMN_NAME( DBTServiceaction::ID_COL );
    static const utility::SortDirection::Value DEFAULT_DIRECTION(bgws::utility::SortDirection::Descending);

    static const SortInfo sort_info( ID_TO_COL, KEY_COLS, DEFAULT_COLUMN_NAME, DEFAULT_DIRECTION );

    string actions_str;
    TimeIntervalOption end_interval( "endTime" );
    StringDbColumnOption end_user( "endUser", DBTServiceaction::USERNAMEENDSERVICEACTION_SIZE );
    MultiWildcardOption location( "location", DBTServiceaction::LOCATION_SIZE, DBTServiceaction::LOCATION_COL_TYPE );
    StringDbColumnOption prepare_user( "prepareUser", DBTServiceaction::USERNAMEPREPAREFORSERVICE_SIZE );
    string statuses_str;
    TimeIntervalOption prepare_interval( "prepareTime" );

    SortSpec sort_spec;

    po::options_description desc;

    desc.add_options()
            ( "action", po::value( &actions_str ) )
            ( "status", po::value( &statuses_str ) )
            ( "sort", po::value( &sort_spec ) )
        ;

    end_interval.addTo( desc );
    end_user.addTo( desc );
    location.addTo( desc );
    prepare_interval.addTo( desc );
    prepare_user.addTo( desc );


    po::variables_map vm;
    po::store(
            po::command_line_parser( _getRequest().getUrl().getQuery().calcArguments() ).options( desc ).allow_unregistered().run(),
            vm
        );
    po::notify( vm );


    WhereClause where_clause;
    cxxdb::ParameterNames parameter_names;

    if ( actions_str != string() ) {
        set<string> action_values;

        BOOST_FOREACH( char action_code, actions_str ) {
            if ( action_code == 'C' )  action_values.insert( blue_gene::service_actions::db_actions::CLOSED );
            else if ( action_code == 'E' )  action_values.insert( blue_gene::service_actions::db_actions::END );
            else if ( action_code == 'O' )  action_values.insert( blue_gene::service_actions::db_actions::OPEN );
            else if ( action_code == 'P' )  action_values.insert( blue_gene::service_actions::db_actions::PREPARE );
            else continue;
        }

        if ( action_values.empty() || action_values.size() == blue_gene::service_actions::db_actions::Count ) {
            // skip it.
        } else {
            string action_sql = DBTServiceaction::SERVICEACTION_COL + " IN ( '" + boost::algorithm::join( action_values, "','" ) + "' )";
            where_clause.add( action_sql );
        }
    }

    end_interval.addTo( where_clause, parameter_names, DBTServiceaction::TSENDSERVICEACTION_COL );
    end_user.addTo( where_clause, parameter_names, DBTServiceaction::USERNAMEENDSERVICEACTION_COL );
    location.addTo( where_clause, parameter_names, DBTServiceaction::LOCATION_COL );
    prepare_interval.addTo( where_clause, parameter_names, DBTServiceaction::TSPREPAREFORSERVICE_COL );
    prepare_user.addTo( where_clause, parameter_names, DBTServiceaction::USERNAMEPREPAREFORSERVICE_COL );

    if ( statuses_str != string() ) {
        set<string> status_values;

        BOOST_FOREACH( char status_code, statuses_str ) {
            if ( status_code == 'A' )  status_values.insert( blue_gene::service_actions::db_statuses::ACTIVE );
            else if ( status_code == 'E' )  status_values.insert( blue_gene::service_actions::db_statuses::ERROR );
            else if ( status_code == 'O' )  status_values.insert( blue_gene::service_actions::db_statuses::OPEN );
            else if ( status_code == 'C' )  status_values.insert( blue_gene::service_actions::db_statuses::CLOSED );
            else if ( status_code == 'F' )  status_values.insert( blue_gene::service_actions::db_statuses::FORCED );
            else if ( status_code == 'P' )  status_values.insert( blue_gene::service_actions::db_statuses::PREPARED );
            else LOG_DEBUG_MSG( "Unexpected status code '" << status_code << "'" );
        }

        if ( status_values.empty() || status_values.size() == blue_gene::service_actions::db_statuses::Count ) {
            // skip it.
        } else {
            string status_sql = DBTServiceaction::STATUS_COL + " IN ( '" + boost::algorithm::join( status_values, "','" ) + "' )";
            where_clause.add( status_sql );
        }
    }

    {
        string sql = string() +

 " SELECT COUNT(*) AS c FROM " + DBTServiceaction().getTableName() + " "
    + where_clause.getString()
            ;

        LOG_DEBUG_MSG( "Preparing: " << sql );

        cxxdb::QueryStatementPtr stmt_ptr(conn.prepareQuery( sql, parameter_names ));
        cxxdb::Parameters &params(stmt_ptr->parameters());

        end_interval.bindParameters( params );
        end_user.bindParameters( params );
        location.bindParameters( params );
        prepare_interval.bindParameters( params );
        prepare_user.bindParameters( params );

        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        if ( rs_ptr->fetch() ) {
            *row_count_out = rs_ptr->columns()["c"].as<uint64_t>();
        } else {
            *row_count_out = 0;
        }

        if ( *row_count_out == 0 ) {
            *rs_ptr_out = cxxdb::ResultSetPtr();
            return;
        }
    }

    {
        string sql = string() +

 "WITH sa AS ("
  " SELECT tsa.*,"
         " ROW_NUMBER() OVER ( ORDER BY " + sort_info.calcSortClause( sort_spec ) + " ) AS row_num"
    " FROM " + DBTServiceaction().getTableName() + " AS tsa "
    + where_clause.getString() +
" )"
" SELECT *"
  " FROM sa"
  " WHERE row_num BETWEEN ? AND ?"
  " ORDER BY row_num"

             ;

        parameter_names.push_back( "rangeStart" );
        parameter_names.push_back( "rangeEnd" );

        LOG_DEBUG_MSG( "preparing " << sql );

        cxxdb::QueryStatementPtr stmt_ptr(conn.prepareQuery( sql, parameter_names ));
        cxxdb::Parameters &params(stmt_ptr->parameters());

        end_interval.bindParameters( params );
        end_user.bindParameters( params );
        location.bindParameters( params );
        prepare_interval.bindParameters( params );
        prepare_user.bindParameters( params );

        req_range.bindParameters( params, "rangeStart", "rangeEnd" );

        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        rs_ptr->internalize( stmt_ptr );

        *rs_ptr_out = rs_ptr;
        return;
    }
}


void ServiceActions::_started(
        capena::server::ResponderPtr /*responder_ptr*/,
        const std::string& location,
        std::exception_ptr exc_ptr,
        const std::string& service_action_id
    )
{
    try {

        Error::Data error_data;
        error_data["location"] = location;

        try {

            if ( exc_ptr != 0 )  std::rethrow_exception( exc_ptr );

            _getResponse().setCreated( ServiceAction::calcPath( _getDynamicConfiguration().getPathBase(), lexical_cast<uint64_t>(service_action_id) ) );

        } catch ( blue_gene::service_actions::InvalidLocationError& ile ) {

            BOOST_THROW_EXCEPTION( Error(
                    "Could not start service action because the location is not valid for a service action.",
                    "startServiceAction", "invalidLocation", error_data,
                    capena::http::Status::BadRequest
                ) );
            return;

        } catch ( blue_gene::service_actions::HardwareDoesntExistError& hdee ) {

            LOG_WARN_MSG( "An attempt to start a service action failed. The location at " << location << " doesn't exist." );

            BOOST_THROW_EXCEPTION( Error(
                    "Could not start service action because the location is not valid. The location must exist.",
                    "startServiceAction", "hardwareDoesntExist", error_data,
                    capena::http::Status::BadRequest
                ) );
            return;

        } catch ( blue_gene::service_actions::NoIdProvidedError& nipe ) {

            error_data["message"] = nipe.getErrorMessage();

            BOOST_THROW_EXCEPTION( Error(
                    string() + "Failed to start service action on " + location + ". Error is '" + nipe.getErrorMessage() + "'",
                    "startServiceAction", "startFailed", error_data,
                    capena::http::Status::InternalServerError
                ) );
            return;

        } catch ( blue_gene::service_actions::ConflictError& ce ) {

            error_data["message"] = "Could not start service action because another service action that conflicts is already in progress.";

            BOOST_THROW_EXCEPTION( Error(
                    string() + "Failed to start service action on " + location + " because a another service action on a conflicting location is already in progress.",
                    "startServiceAction", "conflict", error_data,
                    capena::http::Status::BadRequest
                ) );
            return;

        }

    } catch ( std::exception& e ) {

        _handleError( e );

    }
}


void ServiceActions::_gotAttentionMessagesSnapshot(
        capena::server::ResponderPtr /*responder_ptr*/,
        const blue_gene::service_actions::AttentionMessagesMap& attention_messages
    )
{

    try {
        // Query to get the service actions...

        static const unsigned DefaultRangeSize(50), MaxRangeSize(100);
        RequestRange req_range( _getRequest(), DefaultRangeSize, MaxRangeSize );


        auto conn_ptr(dbConnectionPool::getConnection());

        uint64_t row_count(0);
        cxxdb::ResultSetPtr rs_ptr;

        _query(
                *conn_ptr,
                req_range,
                &row_count,
                &rs_ptr
            );

        if ( row_count == 0 ) {
            // No matches for the filter options, return empty array.

            auto &response(_getResponse());

            response.setContentTypeJson();
            response.headersComplete();

            json::Formatter()( json::ArrayValue(), response.out() );
            return;
        }


        json::ArrayValue arr_val;
        json::Array &arr(arr_val.get());

        while ( rs_ptr->fetch() ) {
            // FIXME: check attention messages!

            const cxxdb::Columns& cols(rs_ptr->columns());

            json::Object &obj(arr.addObject());

            setCommonFields( obj, cols );

            uint64_t id(cols[DBTServiceaction::ID_COL].as<uint64_t>());
            obj.set( "URI", ServiceAction::calcPath( _getDynamicConfiguration().getPathBase(), id ).toString() );


            auto i(attention_messages.find( lexical_cast<string>( id ) ));
            if ( i != attention_messages.end() ) {
                obj.set( "attentionMessages", i->second );
            }

        }

        auto &response(_getResponse());

        req_range.updateResponse( response, arr.size(), row_count );

        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( arr_val, response.out() );

    } catch ( std::exception& e ) {

        _handleError( e );

    }
}


} } // namespace bgws::responder
