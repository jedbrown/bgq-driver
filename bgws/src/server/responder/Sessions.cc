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


/*! \page sessionsResource /bg/bgws/sessions

This resource provides operations on BGWS sessions.

- \subpage sessionsResourceGet
- \subpage sessionsResourcePost

 */


/*! \page sessionsResourceGet GET /bg/bgws/sessions

Get sessions.

\section Authority

The user must be authenticated.
The response will only include those sessions that the user started, or all sessions if the user is an administrator.

\section sessionsResourceGetResponse JSON response format

<pre>
[
  {
    "ID": &quot;<i>string</i>&quot;,
    "URI": &quot;\ref sessionResource&quot;
  },
  ...
]
</pre>

Where
- <i>ID</i> is the session ID.


\section Errors

HTTP status: 403 Forbidden
- notAuthenticated: The user isn't authenticated.

 */


/*! \page sessionsResourcePost POST /bg/bgws/sessions

Start a new session.


\section Authority

Anybody can do this.


\section sessionsResourcePostInput JSON request data format

<pre>
{
  "auth": &quot;<i>auth-info</i>&quot;
}
</pre>

Where
- <i>auth-info</i> is the username and password, separated by a ':' and base64 encoded. <pre>base64( username + ":" + password )</pre>


\section Response

HTTP status: 201 Created

- Location header is the URL of the new session resource, \ref sessionResource .

The content body contains the session data, see \ref sessionResourceGet .


\section Errors

HTTP status: 400 Bad Request

- invalidAuthorizationData : The format of the username and password are not valid (invalid base64 or no ':').
- invalidUserPass : The username or password was not valid.

 */



#include "Sessions.hpp"

#include "Session.hpp"

#include "../CheckUserAdminExecutor.hpp"
#include "../constants.hpp"
#include "../Error.hpp"
#include "../PwauthExecutor.hpp"
#include "../Sessions.hpp"

#include "../utility/base64.hpp"

#include "capena-http/http/http.hpp"

#include "capena-http/server/exception.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>

#include <boost/assign.hpp>
#include <boost/throw_exception.hpp>

#include <boost/exception/diagnostic_information.hpp>

#include <string>

#include <stdio.h>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace responder {


const capena::http::uri::Path Sessions::RESOURCE_PATH(constants::BGWS_PATH / "sessions");
const capena::http::uri::Path Sessions::RESOURCE_PATH_EMPTY_CHILD(RESOURCE_PATH / "");


bool Sessions::matchesUrl( const capena::http::uri::Path& request_path )
{
    return (request_path == RESOURCE_PATH || request_path == RESOURCE_PATH_EMPTY_CHILD);
}


capena::http::Methods Sessions::_getAllowedMethods() const
{
    return { capena::http::Method::GET, capena::http::Method::POST };
}


void Sessions::_doGet()
{
    // The user must be authenticated.
    if ( ! _isUserAuthenticated() ) {
        LOG_WARN_MSG( "Cannot get sessions because the user isn't authenticated." );

        BOOST_THROW_EXCEPTION( Error(
                "Cannot get sessions because the user isn't authenticated.",
                "getSessions", "notAuthenticated", Error::Data(),
                capena::http::Status::Forbidden
            ) );
    }


    json::ArrayValuePtr sessions_arr_ptr(json::Array::create());

    bgws::Sessions::SessionInfos session_infos(_sessions.getSessionInfos(
            _getRequestUserInfo()
        ));

    for ( bgws::Sessions::SessionInfos::const_iterator i(session_infos.begin()) ; i != session_infos.end() ; ++i ) {
        const string &session_id(*i);

        json::Object &session_obj(sessions_arr_ptr->get().addObject());
        session_obj.set( "id", session_id );
        session_obj.set(
                "URI",
                Session::calcPath( session_id, _getDynamicConfiguration().getPathBase() ).toString()
            );
    }

    capena::server::Response &response(_getResponse());

    response.setContentTypeJson();
    response.headersComplete();

    json::Formatter()( *sessions_arr_ptr, response.out() );
}


void Sessions::_doPost( json::ConstValuePtr val_ptr )
{
    // No authority required here.

    string password;
    _parseSessionPostData( val_ptr, _username, password );

    LOG_INFO_MSG( "New session requested by " << _username );

    _pwauth_executor.start(
            _username,
            password,
            _getStrand().wrap(
                boost::bind(
                        &Sessions::_handlePwauthComplete,
                        this,
                        capena::server::AbstractResponder::shared_from_this(),
                        _1
                    )
                )
        );
}


void Sessions::_handlePwauthComplete(
        capena::server::ResponderPtr /*shared_ptr*/,
        bool authenticated
    )
{
    try {

        if ( ! authenticated ) {

            LOG_WARN_MSG( "User " << _username << " failed to authenticate." );

            Error::Data error_data;
            error_data["username"] = _username;

            BOOST_THROW_EXCEPTION( Error(
                    string() + "invalid user name or password for user '" + _username + "'",
                    "createSession",
                    "invalidUserPass",
                    error_data,
                    capena::http::Status::BadRequest
                ) );
        }


        _check_user_admin_executor.start(
                _username,
                _getStrand().wrap(
                    boost::bind(
                            &Sessions::_handleCheckUserAdminComplete,
                            this,
                            capena::server::AbstractResponder::shared_from_this(),
                            _1
                        )
                    )
            );

    } catch ( std::exception& e ) {

        _handleError( e );

    }
}


void Sessions::_handleCheckUserAdminComplete(
        capena::server::ResponderPtr ,// shared_ptr,
        UserInfo::UserType user_type
    )
{
    try {

        SessionPtr session_ptr;

        _sessions.createSession( _username, user_type, &session_ptr );

        json::ValuePtr val_ptr(Session::calcUserJson(
                session_ptr->getUserInfo(),
                session_ptr->getId(),
                _getEnforcer()
            ));

        auto &response(_getResponse());

        response.setStatus( capena::http::Status::Created );
        response.setLocationHeader( Session::calcPath( session_ptr->getId(), _getDynamicConfiguration().getPathBase() ) );
        response.setContentTypeJson();
        response.headersComplete();

        json::Formatter()( *val_ptr, response.out() );

    } catch ( std::exception& e ) {

        _handleError( e );

    }
}


void Sessions::_parseSessionPostData(
        json::ConstValuePtr val_ptr,
        std::string& username_out,
        std::string& password_out
    )
{
    if ( ! val_ptr->isObject() ) {
        BOOST_THROW_EXCEPTION( Error(
                string() + "invalid authorization data, request data is not an object",
                "createSession",
                "invalidAuthorizationData",
                Error::Data(),
                capena::http::Status::BadRequest
            ) );
    }

    const json::Object &obj(val_ptr->getObject());

    if ( ! obj.isString( "auth" ) ) {
        if ( ! obj.contains( "auth" ) ) {
            BOOST_THROW_EXCEPTION( Error(
                    string() + "invalid authorization data, auth is not present",
                    "createSession",
                    "invalidAuthorizationData",
                    Error::Data(),
                    capena::http::Status::BadRequest
                ) );
        }

        BOOST_THROW_EXCEPTION( Error(
                string() + "invalid authorization data, auth is not a string",
                "createSession",
                "invalidAuthorizationData",
                Error::Data(),
                capena::http::Status::BadRequest
            ) );
    }

    const string &auth_b64_str(obj.getString( "auth" ));

    utility::Bytes auth_bytes(utility::base64::decode( auth_b64_str ));

    utility::Bytes::iterator sep_loc(std::find( auth_bytes.begin(), auth_bytes.end(), ':' ));

    if ( sep_loc == auth_bytes.end() ) {
        BOOST_THROW_EXCEPTION( Error(
                string() + "invalid authorization data, could not find separator",
                "createSession",
                "invalidAuthorizationData",
                Error::Data(),
                capena::http::Status::BadRequest
            ) );
    }

    username_out = string( auth_bytes.begin(), sep_loc );
    password_out = string( sep_loc + 1, auth_bytes.end() );
}


} // namespace bgws::responder
} // namespace bgws
