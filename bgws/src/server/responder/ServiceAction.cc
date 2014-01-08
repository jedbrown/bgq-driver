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

/*! \page serviceActionResource /bg/serviceActions/<i>id</i>

This resource represents a service action.

- \subpage serviceActionResourceGet
- \subpage serviceActionResourcePost

 */

/*! \page serviceActionResourceGet GET /bg/serviceActions/<i>id</i>

Get details on a service actions.

\section Authority

The user must have hardware READ authority.

\section serviceActionsResourceGetResponse JSON response format

<pre>
{
  "id": <i>number</i>,
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
}
</pre>

\section Errors

HTTP status: 400 Bad Request
- invalidId: The ID is not a valid ID string. It must be a number.

HTTP status: 403 Forbidden
- authority: The user doesn't have hardware READ authority.

HTTP status: 404 Not Found
- notFound: A service action with the given ID doesn't exist.

 */

/*! \page serviceActionResourcePost POST /bg/serviceActions/<i>id</i>

Perform an operation on a service action. A service action can be ended or forced closed.
<br/>
The "end" operation ends the service action. The service action must have an action of PREPARE and status of (P)REPARED to be ended.
<br/>
The "close" operation force-closes the service action. The service action must have a status of (E)RROR to be forced closed.


\section Authority

The user must be an administrator to perform an operation on a service action.

\section serviceActionResourcePostInput JSON request data format

<pre>
{
  "operation" : &quot;<i>sa-operation</i>&quot;
}
</pre>

Where <i>sa-operation</i> is "end" or "close".


\section Response

HTTP status: 204 No Content

\section Errors

HTTP status: 400 Bad Request
- invalidId: The id provided is not a valid service action ID, it must be a number.
- noOperation: No operation was provided in the posted document.
- unexpectedOperation: The operation was not an expected value. The operation must be "end" or "close".
- incorrectState: The service action isn't in the correct state. The service action must be in PREPARE/(P)REPARED state for the end operation or (E)RROR for the close operation.

HTTP status: 403 Forbidden
- authority: The user doesn't have authority to perform an operation on a service action.

HTTP status: 404 Not Found
- notFound: The service action could not be found.

 */


#include "ServiceAction.hpp"

#include "ServiceActions.hpp"

#include "../blue_gene.hpp"
#include "../dbConnectionPool.hpp"
#include "../Error.hpp"
#include "../ras.hpp"

#include "../blue_gene/service_actions/ServiceActions.hpp"

#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>

#include <stdint.h>


using BGQDB::DBTServiceaction;

using boost::lexical_cast;

using boost::assign::list_of;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {


namespace statics {

namespace operation_name {

static const std::string CLOSE("close");
static const std::string END("end");

} // namespace statics::operation_name

} // namespace statics



capena::http::uri::Path ServiceAction::calcPath(
        const capena::http::uri::Path& path_base,
        uint64_t id
    )
{
    return (path_base / ServiceActions::RESOURCE_PATH / lexical_cast<string>( id ));
}


void ServiceAction::_doGet()
{
    const string &id_str(_getRequestedResourcePath().back());

    Error::Data error_data;
    error_data["id"] = id_str;

    uint64_t id;

    try {

        id = lexical_cast<int64_t>( id_str );

    } catch ( std::exception& e ) {

        BOOST_THROW_EXCEPTION( Error(
                boost::str( boost::format( "Could not get details for service action '%1%' because the service action ID is not valid, it must be an integer." ) % id_str ),
                "getServiceActionDetails", "invalidId", error_data,
                capena::http::Status::BadRequest
            ) );

    }

    if ( ! _userHasHardwareRead() ) {

        LOG_WARN_MSG( "Could not get service action details because " << _getRequestUserInfo() << " doesn't have authority." );

        BOOST_THROW_EXCEPTION( Error(
                "Could not get service action details because the user doesn't have authority.",
                "getServiceAction", "authority", error_data,
                capena::http::Status::Forbidden
            ) );

    }


    _service_actions.getAttentionMessages(
            id_str,
            _getStrand().wrap( boost::bind(
                    &ServiceAction::_gotAttentionMessages, this,
                    capena::server::AbstractResponder::shared_from_this(),
                    id,
                    _1
                ) )
        );

}


void ServiceAction::_doPost( json::ConstValuePtr val_ptr )
{
    const string &id_str(_getRequestedResourcePath().back());

    Error::Data error_data;
    error_data["id"] = id_str;

    uint64_t id;

    try {

        id = lexical_cast<int64_t>( id_str );

    } catch ( std::exception& e ) {

        BOOST_THROW_EXCEPTION( Error(
                boost::str( boost::format( "Could not perform operation on service action '%1%' because the service action ID is not valid, it must be an integer." ) % id_str ),
                "serviceActionOperation", "invalidId", error_data,
                capena::http::Status::BadRequest
            ) );

    }

    if ( ! _isUserAdministrator() ) {
        LOG_WARN_MSG( "Could not perform operation on service action because " << _getRequestUserInfo() << " doesn't have authority." );

        ras::postAdminAuthorityFailure( _getRequestUserInfo(), "service action operation" );

        BOOST_THROW_EXCEPTION( Error(
                "Could not perform operation on service action because the user doesn't have authority.",
                "serviceActionOperation", "authority", error_data,
                capena::http::Status::Forbidden
            ) );

        return;
    }

    LOG_DEBUG_MSG( "service action operation" << id << " with data: \n" << json::Formatter()( *val_ptr ) );

    const json::Object &obj(val_ptr->getObject());

    if ( ! obj.contains( "operation" ) || ! obj.isString( "operation" ) ) {

        BOOST_THROW_EXCEPTION( Error(
                "Invalid data performing service action operation, no operation provided.",
                "serviceActionOperation", "noOperation", error_data,
                capena::http::Status::BadRequest
            ) );

    }

    const string &operation(obj.getString( "operation" ));

    error_data["operation"] = operation;

    if ( operation == statics::operation_name::END || operation == statics::operation_name::CLOSE ) {
        // This is OK.
    } else { // Invalid operation.

        // Invalid operation.

        BOOST_THROW_EXCEPTION( Error(
                boost::str( boost::format( "Unexpected operation '%1%'." ) % operation ),
                "serviceActionOperation", "unexpectedOperation", error_data,
                capena::http::Status::BadRequest
            ) );

    }

    // Get the info on the service action with the ID, its location and state.

    auto conn_ptr(dbConnectionPool::getConnection());

    string sql = string() +

"SELECT " + DBTServiceaction::LOCATION_COL + ", " + DBTServiceaction::SERVICEACTION_COL + ", " + DBTServiceaction::STATUS_COL +
" FROM " + DBTServiceaction().getTableName() +
" WHERE " + DBTServiceaction::ID_COL + " = ?";

    cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( sql, list_of( "id" ) ));

    stmt_ptr->parameters()["id"].cast( id );

    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

    if ( ! rs_ptr->fetch() ) {
        BOOST_THROW_EXCEPTION( Error(
                boost::str( boost::format( "Could not perform operation on service action %1% because the service action does not exist." ) % id ),
                "serviceActionOperation", "notFound", error_data,
                capena::http::Status::NotFound
            ) );
    }

    const cxxdb::Columns &cols(rs_ptr->columns());

    string location(cols[DBTServiceaction::LOCATION_COL].getString());

    string action(cols[DBTServiceaction::SERVICEACTION_COL].getString());
    string status(cols[DBTServiceaction::STATUS_COL].getString());

    error_data["location"] = location;
    error_data["serviceActionAction"] = action;
    error_data["serviceActionStatus"] = status;


    if ( operation == statics::operation_name::END ) {

        _end(
                id,
                location,
                action,
                status,
                error_data
            );

    } else { // operation is close.

        _close(
                id,
                location,
                status,
                error_data
            );

    }

}


void ServiceAction::_gotAttentionMessages(
        capena::server::ResponderPtr /*responder_ptr*/,
        uint64_t id,
        const std::string& attention_messages
    )
{
    try {

        Error::Data error_data;
        error_data["id"] = lexical_cast<string>( id );


        auto conn_ptr(dbConnectionPool::getConnection());

        string sql = string() + "SELECT * FROM " + DBTServiceaction().getTableName() + " WHERE " + DBTServiceaction::ID_COL + " = ?";

        cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( sql, list_of( "id" ) ));

        stmt_ptr->parameters()["id"].cast( id );

        cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

        if ( ! rs_ptr->fetch() ) {
            BOOST_THROW_EXCEPTION( Error(
                    boost::str( boost::format( "Could not get details for service action %1% because the service action does not exist." ) % id ),
                    "getServiceActionDetails", "notFound", error_data,
                    capena::http::Status::NotFound
                ) );
        }


        const cxxdb::Columns& cols(rs_ptr->columns());

        json::ObjectValue obj_val;
        json::Object &obj(obj_val.get());

        ServiceActions::setCommonFields( obj, cols );

        if ( ! attention_messages.empty() ) {
            obj.set( "attentionMessages", attention_messages );
        }

        auto &response(_getResponse());

        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( obj_val, response.out() );

    } catch ( std::exception& e ) {
        _handleError( e );
    }
}


void ServiceAction::_end(
        uint64_t id,
        const std::string& location,
        const std::string& action,
        const std::string& status,
        Error::Data& error_data_in_out
    )
{
    if ( ! (action == blue_gene::service_actions::db_actions::PREPARE && status == blue_gene::service_actions::db_statuses::PREPARED) ) {

        string error_msg(boost::str( boost::format( "Cannot end service action %1% at %2% because it is not in the correct state. The current state is %3%/%4%." ) % id % location % action % status ));

        LOG_WARN_MSG( error_msg );

        BOOST_THROW_EXCEPTION( Error(
                error_msg,
                "endServiceAction", "incorrectState", error_data_in_out,
                capena::http::Status::BadRequest
            ) );

    }

    LOG_INFO_MSG( _getRequestUserInfo() << " ending service action " << id << " at " << location );

    _service_actions.end(
            location,
            getRequestUserName(),
            _getStrand().wrap( boost::bind( &ServiceAction::_ended, this, capena::server::AbstractResponder::shared_from_this(), _1, id, location, error_data_in_out ) )
        );
}


void ServiceAction::_ended(
        capena::server::ResponderPtr /*responder_ptr*/,
        const std::string& error_message,
        uint64_t id,
        const std::string& location,
        Error::Data error_data
    )
{
    try {

        if ( error_message != string() ) {

            // Error occurred, report it.

            error_data["message"] = error_message;

            string err_text(boost::str( boost::format( "Failed to end service action %1% on %2%. The error message is '%3%'" ) % id % location % error_message ));

            LOG_WARN_MSG( err_text );

            BOOST_THROW_EXCEPTION( Error(
                    err_text,
                    "endServiceAction", "endFailed", error_data,
                    capena::http::Status::InternalServerError
                ) );

        }

        capena::server::Response &response(_getResponse());
        response.setStatus( capena::http::Status::NoContent );
        response.headersComplete();

    } catch ( std::exception& e ) {

        _handleError( e );

    }
}


void ServiceAction::_close(
        uint64_t id,
        const std::string& location,
        const std::string& status,
        Error::Data& error_data_in_out
    )
{
    if ( status != blue_gene::service_actions::db_statuses::ERROR ) {

        string error_msg(boost::str( boost::format( "Cannot close service action %1% at %2% because it is not in the correct state. The current state is %3%." ) % id % location % status ));

        LOG_WARN_MSG( error_msg );

        BOOST_THROW_EXCEPTION( Error(
                error_msg,
                "closeServiceAction", "incorrectState", error_data_in_out,
                capena::http::Status::BadRequest
            ) );

    }

    LOG_INFO_MSG( _getRequestUserInfo() << " closing service action " << id << " at " << location );

    _service_actions.close(
            location,
            getRequestUserName(),
            _getStrand().wrap( boost::bind( &ServiceAction::_closed, this, capena::server::AbstractResponder::shared_from_this(), _1, id, location, error_data_in_out ) )
        );
}


void ServiceAction::_closed(
        capena::server::ResponderPtr /*responder_ptr*/,
        std::exception_ptr exc_ptr,
        uint64_t id,
        const std::string& location,
        Error::Data error_data
    )
{
    try {

        try {

            if ( exc_ptr != 0 )  std::rethrow_exception( exc_ptr );

            capena::server::Response &response(_getResponse());
            response.setStatus( capena::http::Status::NoContent );
            response.headersComplete();

        } catch ( std::exception& e ) {

            // Error occurred, report it.

            error_data["message"] = e.what();

            string err_text(boost::str( boost::format( "Failed to close service action %1% on %2%. The error message is '%3%'" ) % id % location % e.what() ));

            LOG_WARN_MSG( err_text );

            BOOST_THROW_EXCEPTION( Error(
                    err_text,
                    "closeServiceAction", "closeFailed", error_data,
                    capena::http::Status::InternalServerError
                ) );

        }

    } catch ( std::exception& e ) {

        _handleError( e );

    }
}


} } // namespace bgws::responder
