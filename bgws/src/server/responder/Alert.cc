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

/*! \page alertResource /bg/alerts/<i>id</i>

This resource represents an alert.

- \subpage alertResourceGet
- \subpage alertResourcePost
- \subpage alertResourceDelete

 */

/*! \page alertResourceGet GET /bg/alerts/<i>id</i>

Get details for an alert.

\section Authority

The user must have hardware READ authority.

\section alertResourceGetResponse JSON response format

<pre>
{
  "recId" : <i>integer</i>,
  "alertId" : &quot;<i>string</i>&quot;,
  "created" : &quot;<i>\ref timestampFormat "timestamp"</i>&quot;,
  "severity" : &quot;<i>string</i>&quot;,
  "urgency" : &quot;<i>string</i>&quot;,
  "eventLocationType" : &quot;<i>string</i>&quot;,
  "eventLocation" : &quot;<i>string</i>&quot;,
  "fruLocation" : &quot;<i>string</i>&quot;, // optional
  "recommendation" : &quot;<i>string</i>&quot;,
  "reason" : &quot;<i>string</i>&quot;,
  "source" : &quot;<i>string</i>&quot;,
  "state" : <i>integer</i>, // optional
  "rawData" : &quot;<i>string</i>&quot;, // optional
  "duplicateOf" : &quot;\ref alertResource&quot, // value is the alert that this alert is a duplicate of, optional
  "duplicates" : [ &quot;\ref alertResource&quot, ... ], // value is the alerts that are duplicates of this alert, optional
  "comment" : &quot;<i>string</i>&quot;, // optional
  "disable" : &quot;<i>string</i>&quot;  // optional
}
</pre>


\section Errors

HTTP status: 400 Bad Request
- invalidRecordId: The record ID is not a valid record ID.

HTTP status: 403 Forbidden
- authority: The user doesn't have READ hardware authority.

HTTP status: 404 Not Found
- notFound: The alert doesn't exist.

 */


/*! \page alertResourcePost POST /bg/alerts/<i>id</i>

Perform an operation on an alert.
The only operation is close.

\section Authority

The user must be an administrator to close an alert.

\section alertResourcePostInput JSON request data format

<pre>
{
  "operation" : "close"
}
</pre>


\section Response

HTTP status: 204 No Content


\section Errors

HTTP status: 400 Bad Request
- invalidRecordId: record ID is not a valid record ID string. It must be an integer.
- notAnObject: Posted JSON data is not an object.
- noOperation: Posted JSON data doesn't contain an operation string.
- unexpectedOperation: The operation is not expected, it must be "close".
- invalidState: The alert isn't in the correct state for the operation.
- duplicate: The alert cannot be closed because it's a duplicate.

HTTP status: 403 Forbidden
- authority: The user doesn't have authority to close the alert.

HTTP status: 404 Not Found
- notFound: The alert doesn't exist.

 */


/*! \page alertResourceDelete DELETE /bg/alerts/<i>id</i>

Remove an alert.

\section Authority

The user must be an administrator to remove an alert.

\section Response

HTTP status: 204 No Content

\section Errors

HTTP status: 400 Bad Request
- invalidRecordId: record ID is not a valid record ID string. It must be an integer.
- invalidState: The alert isn't in the correct state for the operation.

HTTP status: 403 Forbidden
- authority: The user doesn't have READ hardware authority.

HTTP status: 404 Not Found
- notFound: The alert doesn't exist.

 */


#include "Alert.hpp"

#include "../dbConnectionPool.hpp"
#include "../Error.hpp"
#include "../ras.hpp"

#include "../teal/errors.hpp"
#include "../teal/Teal.hpp"

#include "capena-http/http/http.hpp"

#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/BGQDBlib.h>

#include <ras/include/RasEventHandlerChain.h>
#include <ras/include/RasEventImpl.h>

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <string>

#include <stdint.h>


using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {


const std::string Alert::_GET_OPERATION_NAME("getAlert");
const std::string Alert::_OPERATE_OPERATION_NAME("alertOperation");
const std::string Alert::_CLOSE_OPERATION_NAME("closeAlert");
const std::string Alert::_REMOVE_OPERATION_NAME("removeAlert");


capena::http::uri::Path Alert::calcPath(
        const capena::http::uri::Path& path_base,
        teal::Id id
    )
{
    return (path_base / Alerts::RESOURCE_PATH / lexical_cast<string>(id));
}


void Alert::_doGet()
{
    _checkGetAuthority();

    Error::Data error_data;

    string record_id_str;
    teal::Id record_id;

    _calcRecordId(
            _GET_OPERATION_NAME,
            error_data,
            record_id_str,
            record_id
        );

    auto conn_ptr(dbConnectionPool::getConnection());

    json::ObjectValue obj_val;
    auto &obj(obj_val.get());

    {
        static const string SQL = "SELECT * FROM x_tealalertlog WHERE \"rec_id\" = ?";
        static const cxxdb::ParameterNames param_names = { "recId" };

        auto stmt_ptr(conn_ptr->prepareQuery( SQL, param_names ));

        stmt_ptr->parameters()["recId"].cast( record_id );

        auto rs_ptr(stmt_ptr->execute());

        if ( ! rs_ptr->fetch() ) {

            BOOST_THROW_EXCEPTION( Error(
                    string() + "Alert record ID " + record_id_str + " not found",
                    _GET_OPERATION_NAME,
                    "notFound",
                    error_data,
                    capena::http::Status::NotFound
                ) );

            return;

        }

        const auto &cols(rs_ptr->columns());

        obj.set( "recId", cols["rec_id"].as<teal::Id>() );
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
        if ( cols["state"] )  obj.set( "state", cols["state"].as<int32_t>() );
        if ( cols["raw_data"] )  obj.set( "rawData", cols["raw_data"].getString() );
        if ( cols["comment"] )  obj.set( "comment", cols["comment"].getString() );
        if ( cols["disable"] )  obj.set( "disable", cols["disable"].getString() );
    }

    {
        static const string SQL = "SELECT \"alert_recid\" AS recId FROM x_tealalert2alert WHERE \"t_alert_recid\" = ?";
        static const cxxdb::ParameterNames param_names = { "recId" };

        auto stmt_ptr(conn_ptr->prepareQuery( SQL, param_names ));

        stmt_ptr->parameters()["recId"].cast( record_id );

        auto rs_ptr(stmt_ptr->execute());

        if ( rs_ptr->fetch() ) {
            const auto &cols(rs_ptr->columns());

            obj.set( "duplicateOf", calcPath( _getDynamicConfiguration().getPathBase(), cols["recId"].as<teal::Id>() ).toString() );
        }
    }

    {
        static const string SQL = "SELECT \"t_alert_recid\" AS recId FROM x_tealalert2alert WHERE \"alert_recid\" = ?";
        static const cxxdb::ParameterNames param_names = { "recId" };

        auto stmt_ptr(conn_ptr->prepareQuery( SQL, param_names ));

        stmt_ptr->parameters()["recId"].cast( record_id );

        auto rs_ptr(stmt_ptr->execute());

        json::Array *dups_arr_p(NULL);

        while ( rs_ptr->fetch() ) {

            if ( ! dups_arr_p )  dups_arr_p = &(obj.createArray( "duplicates" ));

            const auto &cols(rs_ptr->columns());

            dups_arr_p->add( calcPath( _getDynamicConfiguration().getPathBase(), cols["recId"].as<teal::Id>() ).toString() );
        }
    }

    auto &response(_getResponse());

    response.setContentTypeJson();
    response.headersComplete();

    json::Formatter()( obj_val, response.out() );
}


void Alert::_doPost( json::ConstValuePtr val_ptr )
{
    _checkPostAuthority();

    Error::Data error_data;

    string record_id_str;
    teal::Id record_id;

    _calcRecordId(
            _OPERATE_OPERATION_NAME,
            error_data,
            record_id_str,
            record_id
        );

    if ( ! val_ptr->isObject() ) {

        LOG_WARN_MSG( "Invalid data posted to alert, request data doesn't contain an object." );

        BOOST_THROW_EXCEPTION( Error(
                "Invalid data posted to alert, request data doesn't contain an object.",
                _OPERATE_OPERATION_NAME, "notAnObject", error_data,
                capena::http::Status::BadRequest
            ) );

        return;
    }

    const json::Object &obj(val_ptr->getObject());

    if ( ! obj.isString( "operation" ) ) {

        LOG_WARN_MSG( "Invalid data posted to alert, request data doesn't contain an operation string." );

        BOOST_THROW_EXCEPTION( Error(
                "Invalid data posted to alert, request data doesn't contain an operation string.",
                _OPERATE_OPERATION_NAME, "noOperation", error_data,
                capena::http::Status::BadRequest
            ) );

        return;
    }

    const string &operation(obj.getString( "operation" ));

    error_data["operation"] = operation;

    if ( operation != "close" ) {

        LOG_WARN_MSG( "Invalid data posted to alert, operation is not valid." );

        BOOST_THROW_EXCEPTION( Error(
                "Invalid data posted to alert, operation is not valid.",
                _OPERATE_OPERATION_NAME, "unexpectedOperation", error_data,
                capena::http::Status::BadRequest
            ) );

        return;
    }

    _teal.closeAlert(
            record_id,
            _getStrand().wrap( boost::bind(
                    &Alert::_closed,
                    this,
                    capena::server::AbstractResponder::shared_from_this(),
                    record_id,
                    _1
                ) )
        );
}


void Alert::_calcRecordId(
        const std::string& operation_name,
        Error::Data& error_data_in_out,
        std::string& record_id_str_out,
        teal::Id& record_id_out
    )
{
    record_id_str_out = _getRequestedResourcePath().back();

    error_data_in_out["id"] = record_id_str_out;

    try {

        record_id_out = lexical_cast<teal::Id>( record_id_str_out );

    } catch ( std::bad_cast& e ) {

        BOOST_THROW_EXCEPTION( Error(
                "Invalid record ID",
                operation_name, "invalidRecordId",
                error_data_in_out,
                capena::http::Status::BadRequest
            ) );

    }
}


void Alert::_checkGetAuthority()
{
    if ( _userHasHardwareRead() ) {
        return;
    }

    LOG_WARN_MSG( "Could not get alerts because " << _getRequestUserInfo() << " doesn't have authority." );

    BOOST_THROW_EXCEPTION( Error(
            "Could not get alert because the user doesn't have authority.",
            _GET_OPERATION_NAME, "authority", Error::Data(),
            capena::http::Status::Forbidden
        ) );
}


void Alert::_checkPostAuthority()
{
    if ( _isUserAdministrator() ) {
        // User has authority.
        return;
    }

    LOG_WARN_MSG( "Could not perform operation on alert because " << _getRequestUserInfo() << " isn't an administrator." );

    ras::postAdminAuthorityFailure( _getRequestUserInfo(), "close alert" );

    BOOST_THROW_EXCEPTION( Error(
            "Could not perform operation on alert because the user doesn't have authority.",
            _OPERATE_OPERATION_NAME, "authority", Error::Data(),
            capena::http::Status::Forbidden
        ) );
}


void Alert::_checkDeleteAuthority()
{
    if ( _isUserAdministrator() ) {
        // User has authority.
        return;
    }

    LOG_WARN_MSG( "Could not remove alert because " << _getRequestUserInfo() << " isn't an administrator." );

    ras::postAdminAuthorityFailure( _getRequestUserInfo(), "remove alert" );

    BOOST_THROW_EXCEPTION( Error(
            "Could not remove alert because the user doesn't have authority.",
            _REMOVE_OPERATION_NAME, "authority", Error::Data(),
            capena::http::Status::Forbidden
        ) );
}


void Alert::_closed(
        capena::server::ResponderPtr /*shared_ptr*/,
        teal::Id record_id,
        std::exception_ptr& exc_ptr
    )
{
    try {
        Error::Data error_data;

        error_data["id"] = lexical_cast<string>( record_id );

        try {

            if ( exc_ptr != 0 )  std::rethrow_exception( exc_ptr );

            // Closed OK.

            auto &response(_getResponse());

            response.setStatus( capena::http::Status::NoContent );
            response.headersComplete();

        } catch ( teal::errors::NotFound& nfe ) {

            error_data["output"] = nfe.getOutput();

            BOOST_THROW_EXCEPTION( Error(
                    string() + "Cannot close alert " + lexical_cast<string>(record_id) + " because it doesn't exist.",
                    _CLOSE_OPERATION_NAME,
                    "notFound",
                    error_data,
                    capena::http::Status::NotFound
                ) );

        } catch ( teal::errors::InvalidState& ise ) {

            error_data["output"] = ise.getOutput();

            BOOST_THROW_EXCEPTION( Error(
                    string() + "Cannot close alert " + lexical_cast<string>(record_id) + " because its state is not correct.",
                    _CLOSE_OPERATION_NAME,
                    "invalidState",
                    error_data,
                    capena::http::Status::BadRequest
                ) );

        } catch ( teal::errors::Duplicate& de ) {

            error_data["output"] = de.getOutput();

            BOOST_THROW_EXCEPTION( Error(
                    string() + "Cannot close alert " + lexical_cast<string>(record_id) + " because its a duplicate.",
                    _CLOSE_OPERATION_NAME,
                    "duplicate",
                    error_data,
                    capena::http::Status::BadRequest
                ) );

        }

    } catch ( std::exception& e  ) {

        _handleError( e );

    }
}


void Alert::_doDelete()
{
    _checkDeleteAuthority();

    Error::Data error_data;

    string record_id_str;
    teal::Id record_id;

    _calcRecordId(
            _REMOVE_OPERATION_NAME,
            error_data,
            record_id_str,
            record_id
        );

    _teal.removeAlert(
            record_id,
            _getStrand().wrap( boost::bind(
                    &Alert::_removed,
                    this,
                    capena::server::AbstractResponder::shared_from_this(),
                    record_id,
                    _1
                ) )
        );
}


void Alert::_removed(
        capena::server::ResponderPtr /*shared_ptr*/,
        teal::Id record_id,
        std::exception_ptr exc_ptr
    )
{
    LOG_DEBUG_MSG( "Alert " << record_id << " removed" );

    try {

        Error::Data error_data;

        error_data["id"] = lexical_cast<string>( record_id );

        try {

            if ( exc_ptr != 0 )  std::rethrow_exception( exc_ptr );

            // The alert was removed.

            auto &response(_getResponse());

            response.setStatus( capena::http::Status::NoContent );
            response.headersComplete();


        } catch ( teal::errors::NotFound& nfe ) {

            error_data["output"] = nfe.getOutput();

            BOOST_THROW_EXCEPTION( Error(
                    string() + "Cannot remove alert " + lexical_cast<string>(record_id) + " because it doesn't exist.",
                    _REMOVE_OPERATION_NAME,
                    "notFound",
                    error_data,
                    capena::http::Status::NotFound
                ) );

        } catch ( teal::errors::InvalidState& ise ) {

            error_data["output"] = ise.getOutput();

            BOOST_THROW_EXCEPTION( Error(
                    string() + "Cannot remove alert " + lexical_cast<string>(record_id) + " because its state is not correct.",
                    _REMOVE_OPERATION_NAME,
                    "invalidState",
                    error_data,
                    capena::http::Status::BadRequest
                ) );

        } catch ( std::exception& e ) {

            error_data["message"] = e.what();

            BOOST_THROW_EXCEPTION( Error(
                    string() + "Failed to remove alert " + lexical_cast<string>(record_id) + " due to unexpected error.",
                    _REMOVE_OPERATION_NAME,
                    "internalError",
                    error_data,
                    capena::http::Status::InternalServerError
                ) );

        }

    } catch ( std::exception& e ) {

        _handleError( e );

    }
}


}} // namespace bgws::responder
